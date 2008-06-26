#include "Fuji.h"

#if defined(MAD_STREAM)

#include "MFSound_Internal.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemCachedFile_Internal.h"
#include "MFHeap.h"

// MAD stream handler

#define SAMPLES_PER_MP3_FRAME 1152

#define INPUT_BUFFER_SIZE	(5*8192)
#define OUTPUT_BUFFER_SIZE	8192 /* Must be an integer multiple of 4. */

#include <mad.h>

struct MFID3
{
	unsigned char *pID3Data;
	uint32 size;
	uint8 major, minor;
	uint8 flags;
	uint8 reserved;
};

struct ID3Chunk
{
	uint32 id;
	uint32 size;
	uint16 flags;
};

struct MFMADDecoder
{
	unsigned char inputBuffer[INPUT_BUFFER_SIZE+MAD_BUFFER_GUARD];
	unsigned char overflow[OUTPUT_BUFFER_SIZE];
	mad_stream stream;
	mad_frame frame;
	mad_synth synth;
	mad_header firstHeader;
	mad_timer_t timer;
	MFFile *pFile;
	MFID3 *pID3;

	uint32 *pFrameOffsets;
	uint32 numFrameOffsetsAllocated;
	uint32 frameOffsetCount;

	unsigned char *pGuardPtr;
	uint32 currentFrame;
	uint32 frameCount;
	uint32 overflowOffset;
	uint32 overflowBytes;
};

int GetSynchSafeInt(unsigned char *pStream)
{
	return (uint32)pStream[3] | ((uint32)pStream[2] << 7) | ((uint32)pStream[1] << 14) | ((uint32)pStream[0] << 21);
}

static inline signed short MadFixedToSshort(mad_fixed_t fixed)
{
	// clipping
	if(fixed >= MAD_F_ONE)
		return 32767;
	if(fixed <= -MAD_F_ONE)
		return -32767;

	// conversion
	fixed = fixed >> (MAD_F_FRACBITS - 15);
	return (signed short)fixed;
}

void ParseID3(MFAudioStream *pStream, MFID3 *pID3, int dataSize)
{
	unsigned char *pData = pID3->pID3Data;

	while(pData - pID3->pID3Data <= dataSize)
	{
		if(pID3->major == 2)
		{
			// for the moment, to be safe...
			return;

			int size = (int)pData[5] | ((int)pData[4] << 7) | ((int)pData[3] << 14);
			pData += 6 + size;
		}
		if(pID3->major > 2)
		{
			ID3Chunk *pChunkHeader = (ID3Chunk*)pData;
			uint32 size = GetSynchSafeInt(pData + 4);

			if(*pData = 'T')
			{
				uint8 encoding = pData[10];
				const char *pString = (const char*)pData + 11;

				switch(pChunkHeader->id)
				{
					case MFMAKEFOURCC('T', 'I', 'T', '2'):
						MFString_CopyN(pStream->streamInfo.songName, pString, MFMin(size-1, sizeof(pStream->streamInfo.songName)-1));
						break;
					case MFMAKEFOURCC('T', 'A', 'L', 'B'):
						MFString_CopyN(pStream->streamInfo.albumName, pString, MFMin(size-1, sizeof(pStream->streamInfo.albumName)-1));
						break;
					case MFMAKEFOURCC('T', 'P', 'E', '1'):
					case MFMAKEFOURCC('T', 'P', 'E', '2'):
						MFString_CopyN(pStream->streamInfo.artistName, pString, MFMin(size-1, sizeof(pStream->streamInfo.artistName)-1));
						break;
					case MFMAKEFOURCC('T', 'C', 'O', 'N'):
						MFString_CopyN(pStream->streamInfo.genre, pString, MFMin(size-1, sizeof(pStream->streamInfo.genre)-1));
						break;
				}
			}

			pData += 10 + size;
		}
	}
}

int GetMADSamples(MFAudioStream *pStream, void *pBuffer, uint32 bytes)
{
	MFMADDecoder *pDecoder = (MFMADDecoder*)pStream->pStreamData;

	int written = 0;

	if(pDecoder->overflowBytes)
	{
		// grab from the overflow until we run out...
		uint32 numBytes = MFMin(pDecoder->overflowBytes - pDecoder->overflowOffset, bytes);
		MFCopyMemory(pBuffer, pDecoder->overflow + pDecoder->overflowOffset, numBytes);
		pDecoder->overflowOffset += numBytes;
		if(pDecoder->overflowOffset == pDecoder->overflowBytes)
			pDecoder->overflowBytes = 0;

		// increment timer
		mad_timer_t t = { 0, MAD_TIMER_RESOLUTION / pDecoder->firstHeader.samplerate };
		mad_timer_multiply(&t, numBytes >> 2);
		mad_timer_add(&pDecoder->timer, t);

		if(bytes == numBytes)
			return numBytes;

		bytes -= numBytes;
		(char*&)pBuffer += numBytes;
		written = (int)numBytes;
	}

	do
	{
		if(pDecoder->stream.buffer==NULL || pDecoder->stream.error == MAD_ERROR_BUFLEN)
		{
			size_t readSize, remaining;
			unsigned char *readStart;

			if(pDecoder->stream.next_frame != NULL)
			{
				remaining = pDecoder->stream.bufend - pDecoder->stream.next_frame;
				for(size_t a=0; a<remaining; ++a)
					pDecoder->inputBuffer[a] = pDecoder->stream.next_frame[a];
//				memmove(pDecoder->inputBuffer, pDecoder->stream.next_frame, remaining);
				readStart = pDecoder->inputBuffer + remaining;
				readSize = INPUT_BUFFER_SIZE - remaining;
			}
			else
			{
				readSize = INPUT_BUFFER_SIZE;
				readStart = pDecoder->inputBuffer;
				remaining = 0;
			}

			readSize = MFFile_Read(pDecoder->pFile, readStart, readSize, false);
			if(readSize <= 0)
			{
				if(MFFile_IsEOF(pDecoder->pFile))
					MFDebug_Warn(3, "End of input stream.");
				else
					MFDebug_Warn(2, "Read error on bit-stream.");
				break;
			}
			if(MFFile_IsEOF(pDecoder->pFile))
			{
				pDecoder->pGuardPtr = readStart + readSize;
				MFZeroMemory(pDecoder->pGuardPtr, MAD_BUFFER_GUARD);
				readSize += MAD_BUFFER_GUARD;
			}

			mad_stream_buffer(&pDecoder->stream, pDecoder->inputBuffer, readSize + remaining);
			pDecoder->stream.error = MAD_ERROR_NONE;
		}

		if(mad_frame_decode(&pDecoder->frame, &pDecoder->stream))
		{
			if(MAD_RECOVERABLE(pDecoder->stream.error))
			{
				if(pDecoder->stream.error != MAD_ERROR_LOSTSYNC || pDecoder->stream.this_frame != pDecoder->pGuardPtr)
					MFDebug_Warn(3, MFStr("Recoverable frame level error (%s)\n", mad_stream_errorstr(&pDecoder->stream)));
				continue;
			}
			else
				if(pDecoder->stream.error == MAD_ERROR_BUFLEN)
					continue;
				else
				{
					MFDebug_Warn(3, MFStr("Unrecoverable frame level error (%s)\n", mad_stream_errorstr(&pDecoder->stream)));
					break;
				}
		}

		// we'll keep a copy of the first frames header (it has some useful information)...
		if(pDecoder->frameCount == 0)
			pDecoder->firstHeader = pDecoder->frame.header;

		// we need to keep a backlog of frame offsets for seeking...
		if(pDecoder->currentFrame == pDecoder->frameCount)
		{
			// we'll only take a frame offset once every 10 frames (otherwise we waste loads of memory!)
			// we need to push the counter along from time to time
			if(pDecoder->frameCount % 10 == 0)
			{
				++pDecoder->frameOffsetCount;

				// if we have overflowed the frame history, we need to realloc...
				if(pDecoder->frameOffsetCount == pDecoder->numFrameOffsetsAllocated)
				{
					pDecoder->numFrameOffsetsAllocated *= 2;
					pDecoder->pFrameOffsets = (uint32*)MFHeap_Realloc(pDecoder->pFrameOffsets, sizeof(uint32)*pDecoder->numFrameOffsetsAllocated);
				}

				pDecoder->pFrameOffsets[pDecoder->frameOffsetCount] = pDecoder->pFrameOffsets[pDecoder->frameOffsetCount - 1];
			}

			// add current frames size to frame offset counter
			int frameSize = pDecoder->stream.next_frame - pDecoder->stream.this_frame;
			pDecoder->pFrameOffsets[pDecoder->frameOffsetCount] += frameSize;

			// increase counters
			++pDecoder->frameCount;
		}
		++pDecoder->currentFrame;

/*
		// filters?? eq/etc?
		if(DoFilter)
			ApplyFilter(&pDecoder->frame);
*/
		const int bytesPerFrame = SAMPLES_PER_MP3_FRAME*4;
		if(pBuffer || bytes < bytesPerFrame)
			mad_synth_frame(&pDecoder->synth, &pDecoder->frame);

		// increment timer
		mad_timer_t t = { 0, MAD_TIMER_RESOLUTION / pDecoder->firstHeader.samplerate };
		mad_timer_multiply(&t, MFMin((uint32)pDecoder->synth.pcm.length, bytes >> 2));
		mad_timer_add(&pDecoder->timer, t);

		bool bStereo = MAD_NCHANNELS(&pDecoder->frame.header) == 2;
		int16 *pOutputPtr = (int16*)pBuffer;

		int i=0;
		for(; i < pDecoder->synth.pcm.length && bytes; i++)
		{
			if(pOutputPtr)
			{
				signed short sample = MadFixedToSshort(pDecoder->synth.pcm.samples[0][i]);
				*(pOutputPtr++) = sample;
				if(bStereo)
					sample = MadFixedToSshort(pDecoder->synth.pcm.samples[1][i]);
				*(pOutputPtr++) = sample;

				written += 4;
			}
			bytes -= 4;
		}
		pBuffer = pOutputPtr;

		// write any remaining samples to the overflow buffer
		pOutputPtr = (int16*)pDecoder->overflow;
		pDecoder->overflowOffset = 0;
		pDecoder->overflowBytes = (pDecoder->synth.pcm.length - i) * 4;

		for(; i < pDecoder->synth.pcm.length; i++)
		{
			signed short sample = MadFixedToSshort(pDecoder->synth.pcm.samples[0][i]);
			*(pOutputPtr++) = sample;
			if(bStereo)
				sample = MadFixedToSshort(pDecoder->synth.pcm.samples[1][i]);
			*(pOutputPtr++) = sample;
		}
	}
	while(bytes);

	return written;
}

void DestroyMADStream(MFAudioStream *pStream)
{
	MFMADDecoder *pDecoder = (MFMADDecoder*)pStream->pStreamData;

	mad_synth_finish(&pDecoder->synth);
	mad_frame_finish(&pDecoder->frame);
	mad_stream_finish(&pDecoder->stream);

	MFFile_Close(pDecoder->pFile);

	if(pDecoder->pID3)
		MFHeap_Free(pDecoder->pID3);
	MFHeap_Free(pDecoder->pFrameOffsets);
	MFHeap_Free(pDecoder);
}

void CreateMADStream(MFAudioStream *pStream, const char *pFilename)
{
	MFCALLSTACK;

	// open mp3 file
	MFFile* hFile = MFFileSystem_Open(pFilename);
	if(!hFile)
		return;

	// attempt to cache the mp3 stream
	MFOpenDataCachedFile cachedOpen;
	cachedOpen.cbSize = sizeof(MFOpenDataCachedFile);
	cachedOpen.openFlags = MFOF_Read | MFOF_Binary | MFOF_Cached_CleanupBaseFile;
	cachedOpen.maxCacheSize = 256*1024; // 256k cache for mp3 stream should be heaps!!
	cachedOpen.pBaseFile = hFile;

	MFFile *pCachedFile = MFFile_Open(MFFileSystem_GetInternalFileSystemHandle(MFFSH_CachedFileSystem), &cachedOpen);
	if(pCachedFile)
		hFile = pCachedFile;

	// init the decoder
	MFMADDecoder *pDecoder = (MFMADDecoder*)MFHeap_Alloc(sizeof(MFMADDecoder));
	pStream->pStreamData = pDecoder;
	pDecoder->pID3 = NULL;
	pDecoder->pFile = hFile;
	pDecoder->pGuardPtr = NULL;
	pDecoder->currentFrame = 0;
	pDecoder->frameCount = 0;
	pDecoder->overflowOffset = 0;
	pDecoder->overflowBytes = 0;
	pDecoder->pFrameOffsets = (uint32*)MFHeap_Alloc(sizeof(uint32)*2048);
	pDecoder->numFrameOffsetsAllocated = 2048;
	pDecoder->frameOffsetCount = 0;

	mad_stream_init(&pDecoder->stream);
	mad_frame_init(&pDecoder->frame);
	mad_synth_init(&pDecoder->synth);
	mad_header_init(&pDecoder->firstHeader);
	mad_timer_reset(&pDecoder->timer);

	// read the ID3 tag, if present...
	unsigned char buffer[10];
	int read = MFFile_Read(hFile, buffer, 10, false);
	if(!MFString_CompareN((char*)buffer, "ID3", 3))
	{
		uint32 size = GetSynchSafeInt(buffer + 6);
		pDecoder->pID3 = (MFID3*)MFHeap_Alloc(sizeof(MFID3) + size);
		pDecoder->pID3->pID3Data = (unsigned char*)(pDecoder->pID3 + 1);
		pDecoder->pID3->size = size;
		pDecoder->pID3->major = buffer[3];
		pDecoder->pID3->minor = buffer[4];
		pDecoder->pID3->flags = buffer[5];
		MFFile_Read(hFile, pDecoder->pID3->pID3Data, size, false);
		ParseID3(pStream, pDecoder->pID3, size);
		pDecoder->pFrameOffsets[0] = read + size;
		read = 0;
	}
	else
	{
		// no ID3, we'll copy what we read into the input buffer...
		MFCopyMemory(pDecoder->inputBuffer, buffer, read);
		pDecoder->pFrameOffsets[0] = 0;
	}

	// prime the input buffer
	read += MFFile_Read(hFile, pDecoder->inputBuffer + read, INPUT_BUFFER_SIZE - read, false);
	mad_stream_buffer(&pDecoder->stream, pDecoder->inputBuffer, read);
	pDecoder->stream.error = MAD_ERROR_NONE;

	// decode the first frame so we can get the frame header
	GetMADSamples(pStream, NULL, 0);

	int sampleRate = pDecoder->firstHeader.samplerate;
	if(!sampleRate)
	{
		DestroyMADStream(pStream);
		return;
	}

	pStream->trackLength = 1000.0f;				// mp3 sucks! we have no idea without skipping through the whole thing... :/
	pStream->bufferSize = sampleRate * 2 * 2;	// 1 second, 2 channels, 16bits per sample

	pStream->pStreamBuffer = MFSound_CreateDynamic(pFilename, sampleRate, 2, 16, sampleRate, MFSF_Dynamic | MFSF_Circular);

	if(!pStream->pStreamBuffer)
		DestroyMADStream(pStream);
}

void SeekMADStream(MFAudioStream *pStream, float seconds)
{
	MFMADDecoder *pDecoder = (MFMADDecoder*)pStream->pStreamData;

	// seeking mp3's is a bitch!!!
	uint32 sample = (uint32)((float)pDecoder->firstHeader.samplerate * seconds);
	uint32 frame = sample / SAMPLES_PER_MP3_FRAME;
	uint32 sampleInFrame = sample % SAMPLES_PER_MP3_FRAME;

	uint32 frameRecord = frame/10;
	if(frameRecord >= pDecoder->frameOffsetCount)
		frameRecord = pDecoder->frameOffsetCount-1;
	pDecoder->currentFrame = frameRecord*10;

	// seek to frame
	uint32 frameOffset = pDecoder->pFrameOffsets[frameRecord];
	MFFile_Seek(pDecoder->pFile, frameOffset, MFSeek_Begin);

	// prime input buffer
	int bytes = MFFile_Read(pDecoder->pFile, pDecoder->inputBuffer, INPUT_BUFFER_SIZE, false);
	mad_stream_buffer(&pDecoder->stream, pDecoder->inputBuffer, bytes);
	pDecoder->stream.error = MAD_ERROR_NONE;
	pDecoder->overflowBytes = 0;

	// initialise timer to our song position
	pDecoder->timer.seconds = 0;
	pDecoder->timer.fraction = MAD_TIMER_RESOLUTION / pDecoder->firstHeader.samplerate;
	mad_timer_multiply(&pDecoder->timer, pDecoder->currentFrame*SAMPLES_PER_MP3_FRAME);

	// skip samples to where we want to be..
	int skipFrames = frame - pDecoder->currentFrame;
	int skipSamples = skipFrames*SAMPLES_PER_MP3_FRAME + sampleInFrame;
	int skipBytes = skipSamples * 4;
	GetMADSamples(pStream, NULL, skipBytes);
}

float GetMADTime(MFAudioStream *pStream)
{
	MFMADDecoder *pDecoder = (MFMADDecoder*)pStream->pStreamData;
	return (float)mad_timer_count(pDecoder->timer, MAD_UNITS_MILLISECONDS) * 0.001f;
}

void MFSound_RegisterMAD()
{
	MFStreamCallbacks callbacks;
	callbacks.pCreateStream = CreateMADStream;
	callbacks.pDestroyStream = DestroyMADStream;
	callbacks.pGetSamples = GetMADSamples;
	callbacks.pGetTime = GetMADTime;
	callbacks.pSeekStream = SeekMADStream;

	MFSound_RegisterStreamHandler("MP3 Audio (MAD)", ".mp3", &callbacks);
}
#endif
