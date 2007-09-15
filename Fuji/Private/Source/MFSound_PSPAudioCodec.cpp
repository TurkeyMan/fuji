#include "Fuji.h"

#if defined(PSPAUDIOCODEC_STREAM)

#include "MFSound_Internal.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemCachedFile_Internal.h"
#include "MFHeap.h"

// libpspaudiocodec stream handler

#include <pspsdk.h>
#include <pspaudiocodec.h>
#include <pspaudio.h>
#include <pspmpeg.h>

#define SAMPLES_PER_MP3_FRAME 1152

static int bitrates[] = { 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 };

static unsigned long mp3_codec_buffer[65] __attribute__((aligned(64)));
static short mp3_data_buffer[2048] __attribute__((aligned(64)));
static short mp3_mix_buffer[SAMPLES_PER_MP3_FRAME * 2] __attribute__((aligned(64)));

struct MFID3
{
	unsigned char *pID3Data;
	uint32 size;
	uint8 major, minor;
	uint8 flags;
	uint8 reserved;
};

struct MP3Header
{
	int audioVersion;
	int layer;
	int bitrate;
	int samplerate;
	int channelMode;
	int modeExt;
	int padding;

	int frameSize;

	bool protectionBit;
	bool copyright;
	bool original;
};

struct MFMP3Decoder
{
	unsigned char inputBuffer[8 * 1024];
	unsigned char overflow[SAMPLES_PER_MP3_FRAME * 4];
	MP3Header firstHeader;
	MFFile *pFile;
	MFID3 *pID3;

	int inputBufferOffset;
	int inputBufferDataLen;

	uint32 *pFrameOffsets;
	uint32 numFrameOffsetsAllocated;
	uint32 frameOffsetCount;

	uint32 currentFrame;
	uint32 frameCount;
	uint32 overflowOffset;
	uint32 overflowBytes;
	uint32 playbackTime;	// in samples

	bool hasEDRAM;
};

int GetSynchSafeInt(unsigned char *pStream)
{
	return (uint32)pStream[3] | ((uint32)pStream[2] << 7) | ((uint32)pStream[1] << 14) | ((uint32)pStream[0] << 21);
}

void RefillInputBuffer(MFMP3Decoder *pDecoder)
{
	pDecoder->inputBufferDataLen -= pDecoder->inputBufferOffset;
	if(pDecoder->inputBufferDataLen)
		MFCopyMemory(pDecoder->inputBuffer, pDecoder->inputBuffer + pDecoder->inputBufferOffset, pDecoder->inputBufferDataLen);
	pDecoder->inputBufferDataLen += MFFile_Read(pDecoder->pFile, pDecoder->inputBuffer + pDecoder->inputBufferDataLen, sizeof(pDecoder->inputBuffer) - pDecoder->inputBufferDataLen);
	pDecoder->inputBufferOffset = 0;
}

void ReadMP3Header(MP3Header *pHeader, const unsigned char *pHeaderBuffer)
{
	// header is big endian
	uint32 header = pHeaderBuffer[0];
	header = (header<<8) | pHeaderBuffer[1];
	header = (header<<8) | pHeaderBuffer[2];
	header = (header<<8) | pHeaderBuffer[3];

	// assert we are looking at a frame header
	MFDebug_Assert(header & 0xFFE00000, MFStr("Not an MP3 frame header: 0x%08X", header));

	// extract details from frame header
	int bitrate = (header & 0xf000) >> 12;
	pHeader->bitrate = bitrates[bitrate];
	pHeader->padding = (header & 0x200) >> 9;

	// need to read this and the rest of the stuff properly...
	pHeader->samplerate = 44100;

	// calculate frame size
	pHeader->frameSize = 144000*pHeader->bitrate/pHeader->samplerate + pHeader->padding;
}

int GetMP3Samples(MFAudioStream *pStream, void *pBuffer, uint32 bytes)
{
	MFMP3Decoder *pDecoder = (MFMP3Decoder*)pStream->pStreamData;

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
		pDecoder->playbackTime += numBytes >> 2;

		if(bytes == numBytes)
			return numBytes;

		bytes -= numBytes;
		(char*&)pBuffer += numBytes;
		written = (int)numBytes;
	}

	do
	{
		// check theres enough data in the file
		if(pDecoder->inputBufferOffset >= pDecoder->inputBufferDataLen - 4)
			RefillInputBuffer(pDecoder);
		if(pDecoder->inputBufferDataLen < 4)
			return written; // eof

		// decode header
		MP3Header header;
		ReadMP3Header(&header, pDecoder->inputBuffer + pDecoder->inputBufferOffset);
		int frame_size = header.frameSize;

		// we'll keep a copy of the first frames header (it has some useful information)...
		if(pDecoder->frameCount == 0)
			pDecoder->firstHeader = header;

		// check theres enough data in the file
		if(pDecoder->inputBufferOffset >= pDecoder->inputBufferDataLen - (frame_size + 4))
			RefillInputBuffer(pDecoder);
		if(pDecoder->inputBufferDataLen < frame_size)
			return written; // eof

		// if we're seeking we dont need to decode... (make the seek a little faster)
		if(pBuffer || bytes < SAMPLES_PER_MP3_FRAME*4)
		{
			// copy samples into the decode buffer (this is unnecessarily allocating every single frame?)
			MFCopyMemory(mp3_data_buffer, pDecoder->inputBuffer + pDecoder->inputBufferOffset, frame_size);

			// decode samples...
			MFZeroMemory(mp3_mix_buffer, sizeof(mp3_mix_buffer));

			mp3_codec_buffer[6] = (unsigned long)mp3_data_buffer;
			mp3_codec_buffer[8] = (unsigned long)mp3_mix_buffer;

			mp3_codec_buffer[7] = mp3_codec_buffer[10] = frame_size;
			mp3_codec_buffer[9] = SAMPLES_PER_MP3_FRAME * 4;

			int r = sceAudiocodecDecode(mp3_codec_buffer, 0x1002);
			if(r < 0)
				MFDebug_Warn(1, "sceAudiocodecDecode() failed.");
		}

		// push the input buffer forward
		pDecoder->inputBufferOffset += frame_size;

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
			pDecoder->pFrameOffsets[pDecoder->frameOffsetCount] += frame_size;

			// increase counters
			++pDecoder->frameCount;
		}
		++pDecoder->currentFrame;

		// copy samples into the output buffer...
		int samplesToCopy = MFMin(SAMPLES_PER_MP3_FRAME, (int)bytes >> 2);
		int bytesToCopy = samplesToCopy*4;

//		bool bStereo = header.channelMode != 3;
		if(pBuffer)
		{
			MFCopyMemory(pBuffer, mp3_mix_buffer, bytesToCopy);
			written += bytesToCopy;
		}
		bytes -= bytesToCopy;

		// increment timer
		pDecoder->playbackTime += samplesToCopy;

		// copy left over samples into the overflow
		if(samplesToCopy < SAMPLES_PER_MP3_FRAME)
		{
			pDecoder->overflowBytes = (SAMPLES_PER_MP3_FRAME - samplesToCopy)*4;
			pDecoder->overflowOffset = 0;
			MFCopyMemory(pDecoder->overflow, mp3_mix_buffer + samplesToCopy*2, pDecoder->overflowBytes);
		}
	}
	while(bytes);

	return written;
}

void DestroyMP3Stream(MFAudioStream *pStream)
{
	MFMP3Decoder *pDecoder = (MFMP3Decoder*)pStream->pStreamData;

	MFFile_Close(pDecoder->pFile);

	if(pDecoder->hasEDRAM)
		sceAudiocodecReleaseEDRAM(mp3_codec_buffer);

	if(pDecoder->pID3)
		MFHeap_Free(pDecoder->pID3);
	if(pDecoder->pFrameOffsets)
		MFHeap_Free(pDecoder->pFrameOffsets);
	MFHeap_Free(pDecoder);
}

void CreateMP3Stream(MFAudioStream *pStream, const char *pFilename)
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
	MFMP3Decoder *pDecoder = (MFMP3Decoder*)MFHeap_Alloc(sizeof(MFMP3Decoder));
	MFZeroMemory(pDecoder, sizeof(MFMP3Decoder));
	pStream->pStreamData = pDecoder;
	pDecoder->pID3 = NULL;
	pDecoder->pFile = hFile;

	MFZeroMemory(mp3_codec_buffer, sizeof(mp3_codec_buffer));

	if(sceAudiocodecCheckNeedMem(mp3_codec_buffer, 0x1002) < 0)
	{
		MFDebug_Warn(1, "Not enough memory for MP3 decoder.");
		DestroyMP3Stream(pStream);
		return;
	}
	if(sceAudiocodecGetEDRAM(mp3_codec_buffer, 0x1002) < 0)
	{
		MFDebug_Warn(1, "sceAudiocodecGetEDRAM() failed.");
		DestroyMP3Stream(pStream);
		return;
	}
	pDecoder->hasEDRAM = true;
	if(sceAudiocodecInit(mp3_codec_buffer, 0x1002) < 0)
	{
		MFDebug_Warn(1, "sceAudiocodecInit() failed.");
		DestroyMP3Stream(pStream);
		return;
	}

	pDecoder->currentFrame = 0;
	pDecoder->frameCount = 0;
	pDecoder->overflowOffset = 0;
	pDecoder->overflowBytes = 0;
	pDecoder->pFrameOffsets = (uint32*)MFHeap_Alloc(sizeof(uint32)*2048);
	pDecoder->numFrameOffsetsAllocated = 2048;
	pDecoder->frameOffsetCount = 0;

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
		pDecoder->pFrameOffsets[0] = read + size;
		read = 0;
	}
	else
	{
		// no ID3, we'll copy what we read into the input buffer...
		MFCopyMemory(pDecoder->inputBuffer, buffer, read);
		pDecoder->pFrameOffsets[0] = 0;
		pDecoder->inputBufferDataLen = read;
	}

	// prime the input buffer
	read += MFFile_Read(hFile, pDecoder->inputBuffer + read, sizeof(pDecoder->inputBuffer) - read, false);
	pDecoder->inputBufferDataLen += read;

	// decode the first frame so we can get the frame header
	GetMP3Samples(pStream, NULL, 0);

	int sampleRate = pDecoder->firstHeader.samplerate;
	if(!sampleRate)
	{
		DestroyMP3Stream(pStream);
		return;
	}

	pStream->trackLength = 1000.0f;				// mp3 sucks! we have no idea without skipping through the whole thing... :/
	pStream->bufferSize = sampleRate * 2 * 2;	// 1 second, 2 channels, 16bits per sample

	pStream->pStreamBuffer = MFSound_CreateDynamic(pFilename, sampleRate, 2, 16, sampleRate, MFSF_Dynamic | MFSF_Circular);

	if(!pStream->pStreamBuffer)
		DestroyMP3Stream(pStream);
}

float GetMP3Time(MFAudioStream *pStream)
{
	MFMP3Decoder *pDecoder = (MFMP3Decoder*)pStream->pStreamData;
	return (float)pDecoder->playbackTime / (float)pDecoder->firstHeader.samplerate;
}

void SeekMP3Stream(MFAudioStream *pStream, float seconds)
{
	MFMP3Decoder *pDecoder = (MFMP3Decoder*)pStream->pStreamData;

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
	int bytes = MFFile_Read(pDecoder->pFile, pDecoder->inputBuffer, sizeof(pDecoder->inputBuffer), false);
	pDecoder->inputBufferDataLen = bytes;
	pDecoder->inputBufferOffset = 0;
	pDecoder->overflowBytes = 0;

	// initialise timer to our song position
	pDecoder->playbackTime = pDecoder->currentFrame*SAMPLES_PER_MP3_FRAME;
	MFDebug_Log(4, MFStr("Request: %.2f Start: %.2f", seconds, GetMP3Time(pStream)));

	// skip samples to where we want to be..
	int skipFrames = frame - pDecoder->currentFrame;
	int skipSamples = skipFrames*SAMPLES_PER_MP3_FRAME + sampleInFrame;
	int skipBytes = skipSamples * 4;
	GetMP3Samples(pStream, NULL, skipBytes);

	MFDebug_Log(4, MFStr("Resolve: %.2f", GetMP3Time(pStream)));
}

void MFSound_RegisterPSPAudioCodec()
{
	MFDebug_Log(2, "Attempting to load AV modules");
	int result = 0;
	bool loaded = false;
/*
	// this tries to load the AV modules in user mode for 3.03+ users
	int firmwareVersion = sceKernelDevkitVersion();
	if(firmwareVersion >= 0x02070000)
	{
		MFDebug_Log(2, "  Loading from sceUtilityLoadAvModule()");

		loaded = true;
		result = sceUtilityLoadAvModule(0);
		if(result < 0)
			loaded = false;
		if(loaded)
		{
			result = sceUtilityLoadAvModule(3);
			if(result < 0)
			{
				sceUtilityUnloadAvModule(0);
				loaded = false;
			}
		}
	}
*/
	if(!loaded)
	{
		loaded = true;

		MFDebug_Log(2, "  Loading flash0:/kd/me_for_vsh.prx...");
		result = pspSdkLoadStartModule("flash0:/kd/me_for_vsh.prx", PSP_MEMORY_PARTITION_KERNEL);

		MFDebug_Log(2, "  Loading flash0:/kd/audiocodec.prx...");
		result = pspSdkLoadStartModule("flash0:/kd/videocodec.prx", PSP_MEMORY_PARTITION_KERNEL);

		MFDebug_Log(2, "  Loading flash0:/kd/videocodec.prx...");
		result = pspSdkLoadStartModule("flash0:/kd/audiocodec.prx", PSP_MEMORY_PARTITION_KERNEL);

		MFDebug_Log(2, "  Loading flash0:/kd/mpegbase.prx...");
		result = pspSdkLoadStartModule("flash0:/kd/mpegbase.prx", PSP_MEMORY_PARTITION_KERNEL);

		MFDebug_Log(2, "  Loading flash0:/kd/mpeg_vsh.prx...");
		result = pspSdkLoadStartModule("flash0:/kd/mpeg_vsh.prx", PSP_MEMORY_PARTITION_USER);
	}

	if(loaded)
	{
		sceMpegInit();

		MFStreamCallbacks callbacks;
		callbacks.pCreateStream = CreateMP3Stream;
		callbacks.pDestroyStream = DestroyMP3Stream;
		callbacks.pGetSamples = GetMP3Samples;
		callbacks.pGetTime = GetMP3Time;
		callbacks.pSeekStream = SeekMP3Stream;

		MFDebug_Log(2, "Register .mp3 stream handler");
		MFSound_RegisterStreamHandler("MP3 Audio (libaudiocodec.prx)", ".mp3", &callbacks);
//		MFSound_RegisterStreamHandler("AAC Audio (libaudiocodec.prx)", ".aac", &callbacks);
//		MFSound_RegisterStreamHandler("AT3 Audio (libaudiocodec.prx)", ".at3", &callbacks);
//		MFSound_RegisterStreamHandler("AA3 Audio (libaudiocodec.prx)", ".aa3", &callbacks);
	}
}
#endif

#if 0
SceCtrlData input;

static int bitrates[] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 };

unsigned long aac_codec_buffer[65] __attribute__((aligned(64)));
short aac_mix_buffer[1024 * 2] __attribute__((aligned(64)));

SceUID aac_handle;
u8* aac_data_buffer;
u16 aac_data_align;
u32 aac_sample_per_frame;
u16 aac_channel_mode;
u32 aac_data_start;
u32 aac_data_size;
u8 aac_getEDRAM;
u32 aac_channels;
u32 aac_samplerate;

int main(void)
{
   aac_handle = sceIoOpen("ms0:/Test.AAC", PSP_O_RDONLY, 0777);
   if (  ! aac_handle )
      goto wait;
   
   aac_channels = 2;
   aac_samplerate = 44100; //this is aac file's samplerate, also can be 48000,....
   aac_sample_per_frame = 1024;
   
   aac_data_start = sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR);
   
   memset(aac_codec_buffer, 0, sizeof(aac_codec_buffer));
   
   if ( sceAudiocodecCheckNeedMem(aac_codec_buffer, 0x1003) < 0 )
      goto wait;
   if ( sceAudiocodecGetEDRAM(aac_codec_buffer, 0x1003) < 0 )
         goto wait;
   aac_getEDRAM = 1;
   
   aac_codec_buffer[10] = aac_samplerate;
   if ( sceAudiocodecInit(aac_codec_buffer, 0x1003) < 0 )
      goto wait;
   
   int eof = 0;   
   while( !eof )
   {
      int samplesdecoded;
      memset(aac_mix_buffer, 0, aac_sample_per_frame*2*2);
      unsigned char aac_header_buf[7];
      if ( sceIoRead( aac_handle, aac_header_buf, 7 ) != 7 )
	  {
         eof = 1;
         continue;
      }
      int aac_header = aac_header_buf[3];
      aac_header = (aac_header<<8) | aac_header_buf[4];
      aac_header = (aac_header<<8) | aac_header_buf[5];
      aac_header = (aac_header<<8) | aac_header_buf[6];
      
      int frame_size = aac_header & 67100672;
      frame_size = frame_size >> 13;
      frame_size = frame_size - 7;
      
      if ( aac_data_buffer )
         free(aac_data_buffer);
      aac_data_buffer = (u8*)memalign(64, frame_size);
      
      if ( sceIoRead( aac_handle, aac_data_buffer, frame_size ) != frame_size )
	  {
         eof = 1;
         continue;
      }
      
      aac_data_start += (frame_size+7);
      
      aac_codec_buffer[6] = (unsigned long)aac_data_buffer;
      aac_codec_buffer[8] = (unsigned long)aac_mix_buffer;
      
      aac_codec_buffer[7] = frame_size;
      aac_codec_buffer[9] = aac_sample_per_frame * 4;
      
   
      int res = sceAudiocodecDecode(aac_codec_buffer, 0x1003);
      if ( res < 0 )
	  {
         eof = 1;
         continue;
      }
      samplesdecoded = aac_sample_per_frame;
   }

wait:
   
   if ( aac_handle )
      sceIoClose(aac_handle);
   if ( aac_data_buffer)
      free(aac_data_buffer);
   if ( aac_getEDRAM )
      sceAudiocodecReleaseEDRAM(aac_codec_buffer);
   
   sceCtrlReadBufferPositive(&input, 1);
   while(!(input.Buttons & PSP_CTRL_TRIANGLE))
   {
      sceKernelDelayThread(10000);   // wait 10 milliseconds
      sceCtrlReadBufferPositive(&input, 1);
   }
   
   sceKernelExitGame();
   return 0;
}





AT3 file decode:
Code:

SceCtrlData input;

#define TYPE_ATRAC3 0x270
#define TYPE_ATRAC3PLUS 0xFFFE

unsigned long at3_codec_buffer[65] __attribute__((aligned(64)));
short at3_mix_buffer[2048 * 2] __attribute__((aligned(64)));


SceUID at3_handle;
u16 at3_type;
u8* at3_data_buffer;
u16 at3_data_align;
u32 at3_sample_per_frame;
u16 at3_channel_mode;
u8 at3_at3plus_flagdata[2];
u32 at3_data_start;
u32 at3_data_size;
u8 at3_getEDRAM;
u32 at3_channels;
u32 at3_samplerate;


int main(void)
{
   SetupCallbacks();
   
   int result = pspSdkLoadStartModule("flash0:/kd/audiocodec.prx", PSP_MEMORY_PARTITION_KERNEL);
   pspSdkFixupImports(result);
   
   SceUID at3_handle = sceIoOpen("ms0:/Test.AT3", PSP_O_RDONLY, 0777);
   if (  ! at3_handle )
      goto wait;
   
   u32 riff_header[2];
   if ( sceIoRead( at3_handle, riff_header, 8 ) != 8 )
      goto wait;
   if ( riff_header[0] != 0x46464952 )
      goto wait;
   u32 wavefmt_header[3];
   if ( sceIoRead( at3_handle, wavefmt_header, 12 ) != 12 )
      goto wait;
   if ( wavefmt_header[0] != 0x45564157 || wavefmt_header[1] != 0x20746D66 )
      goto wait;
   u8* wavefmt_data = (u8*)malloc(wavefmt_header[2]);
   if ( wavefmt_data == NULL )
      goto wait;
   if ( sceIoRead( at3_handle, wavefmt_data, wavefmt_header[2] ) != wavefmt_header[2] ) {
      free(wavefmt_data);
      goto wait;
   }
   at3_type = *((u16*)wavefmt_data);
   at3_channels = *((u16*)(wavefmt_data+2));
   at3_samplerate = *((u32*)(wavefmt_data+4));
   at3_data_align = *((u16*)(wavefmt_data+12));
   
   if ( at3_type == TYPE_ATRAC3PLUS) {
      at3_at3plus_flagdata[0] = wavefmt_data[42];
      at3_at3plus_flagdata[1] = wavefmt_data[43];
   }
   
   free(wavefmt_data);
   
   u32 data_header[2];
   if ( sceIoRead( at3_handle, data_header, 8 ) != 8 )
      goto wait;
   while(data_header[0] != 0x61746164 ) {
      sceIoLseek32(at3_handle, data_header[1], PSP_SEEK_CUR);
      if ( sceIoRead( at3_handle, data_header, 8 ) != 8 )
         goto wait;
   }
   
   at3_data_start = sceIoLseek32(at3_handle, 0, PSP_SEEK_CUR);
   at3_data_size = data_header[1];
   
   if ( at3_data_size % at3_data_align != 0 )
      goto wait;
   
   memset(at3_codec_buffer, 0, sizeof(at3_codec_buffer));
   
   if ( at3_type == TYPE_ATRAC3 ) {
      at3_channel_mode = 0x0;
      if ( at3_data_align == 0xC0 ) // atract3 have 3 bitrate, 132k,105k,66k, 132k align=0x180, 105k align = 0x130, 66k align = 0xc0
         at3_channel_mode = 0x1;
      at3_sample_per_frame = 1024;
      at3_data_buffer = (u8*)memalign(64, 0x180);
      if ( at3_data_buffer == NULL)
         goto wait;
      at3_codec_buffer[26] = 0x20;
      if ( sceAudiocodecCheckNeedMem(at3_codec_buffer, 0x1001) < 0 )
         goto wait;
      if ( sceAudiocodecGetEDRAM(at3_codec_buffer, 0x1001) < 0 )
         goto wait;
      at3_getEDRAM = 1;
      at3_codec_buffer[10] = 4;
      at3_codec_buffer[44] = 2;
      if ( at3_data_align == 0x130 )
         at3_codec_buffer[10] = 6;
      if ( sceAudiocodecInit(at3_codec_buffer, 0x1001) < 0 ) {
         goto wait;
      }
   }
   else if ( at3_type == TYPE_ATRAC3PLUS ) {
      at3_sample_per_frame = 2048;
      int temp_size = at3_data_align+8;
      int mod_64 = temp_size & 0x3f;
      if (mod_64 != 0) temp_size += 64 - mod_64;
      at3_data_buffer = (u8*)memalign(64, temp_size);
      if ( at3_data_buffer == NULL)
         goto wait;
      at3_codec_buffer[5] = 0x1;
      at3_codec_buffer[10] = at3_at3plus_flagdata[1];
      at3_codec_buffer[10] = (at3_codec_buffer[10] << 8 ) | at3_at3plus_flagdata[0];
      at3_codec_buffer[12] = 0x1;
      at3_codec_buffer[14] = 0x1;
      if ( sceAudiocodecCheckNeedMem(at3_codec_buffer, 0x1000) < 0 )
         goto wait;
      if ( sceAudiocodecGetEDRAM(at3_codec_buffer, 0x1000) < 0 )
         goto wait;
      at3_getEDRAM = 1;
      if ( sceAudiocodecInit(at3_codec_buffer, 0x1000) < 0 ) {
         goto wait;
      }
   }
   else
      goto wait;
   
   int eof = 0;   
   while( !eof ) {
      int samplesdecoded;
      memset(at3_mix_buffer, 0, 2048*2*2);
      unsigned long decode_type = 0x1001;
      if ( at3_type == TYPE_ATRAC3 ) {
         memset( at3_data_buffer, 0, 0x180);
         if (sceIoRead( at3_handle, at3_data_buffer, at3_data_align ) != at3_data_align) {
            eof = 1;
            continue;
         }
         if ( at3_channel_mode ) {
            memcpy(at3_data_buffer+at3_data_align, at3_data_buffer, at3_data_align);
         }
         decode_type = 0x1001;
      }
      else {
         memset( at3_data_buffer, 0, at3_data_align+8);
         at3_data_buffer[0] = 0x0F;
         at3_data_buffer[1] = 0xD0;
         at3_data_buffer[2] = at3_at3plus_flagdata[0];
         at3_data_buffer[3] = at3_at3plus_flagdata[1];
         if (sceIoRead( at3_handle, at3_data_buffer+8, at3_data_align ) != at3_data_align) {
            eof = 1;
            continue;
         }
         decode_type = 0x1000;
      }
   
      at3_codec_buffer[6] = (unsigned long)at3_data_buffer;
      at3_codec_buffer[8] = (unsigned long)at3_mix_buffer;
   
      int res = sceAudiocodecDecode(at3_codec_buffer, decode_type);
      if ( res < 0 ) {
         eof = 1;
         continue;
      }
      samplesdecoded = at3_sample_per_frame;
   }

wait:
   
   if ( at3_handle ) {
      sceIoClose(at3_handle);
   }
   if ( at3_data_buffer) {
      free(at3_data_buffer);
   }
   if ( at3_getEDRAM ) {
      sceAudiocodecReleaseEDRAM(at3_codec_buffer);
   }
   
   sceCtrlReadBufferPositive(&input, 1);
   while(!(input.Buttons & PSP_CTRL_TRIANGLE))
   {
      sceKernelDelayThread(10000);   // wait 10 milliseconds
      sceCtrlReadBufferPositive(&input, 1);
   }
   
   sceKernelExitGame();
   return 0;
}




AA3 file decode:
Code:

SceCtrlData input;

#define TYPE_ATRAC3 0x270
#define TYPE_ATRAC3PLUS 0xFFFE

unsigned long aa3_codec_buffer[65] __attribute__((aligned(64)));
short aa3_mix_buffer[2048 * 2] __attribute__((aligned(64)));


SceUID aa3_handle;
u16 aa3_type;
u8* aa3_data_buffer;
u16 aa3_data_align;
u32 aa3_sample_per_frame;
u16 aa3_channel_mode;
u8 aa3_at3plus_flagdata[2];
u32 aa3_data_start;
u32 aa3_data_size;
u8 aa3_getEDRAM;
u32 aa3_channels;
u32 aa3_samplerate;


int main(void)
{
   SetupCallbacks();
   
   int result = pspSdkLoadStartModule("flash0:/kd/audiocodec.prx", PSP_MEMORY_PARTITION_KERNEL);
   pspSdkFixupImports(result);
   
   SceUID aa3_handle = sceIoOpen("ms0:/Test.AA3", PSP_O_RDONLY, 0777); // or ms0:/Test.OMA
   if (  ! aa3_handle )
      goto wait;
   
   sceIoLseek32(aa3_handle, 0x0C00, PSP_SEEK_SET);
   
   u8 ea3_header[0x60];
   if ( sceIoRead( aa3_handle, ea3_header, 0x60 ) != 0x60 )
      goto wait;
   if ( ea3_header[0] != 0x45 || ea3_header[1] != 0x41 || ea3_header[2] != 0x33 || ea3_header[3] != 0x01 )
      goto wait;
   
   aa3_at3plus_flagdata[0] = ea3_header[0x22];
   aa3_at3plus_flagdata[1] = ea3_header[0x23];
   
   aa3_type = (ea3_header[0x22] == 0x20) ? TYPE_ATRAC3 : ((ea3_header[0x22] == 0x28) ? TYPE_ATRAC3PLUS : 0x0);
   
   if ( aa3_type != TYPE_ATRAC3 && aa3_type != TYPE_ATRAC3PLUS )
      goto wait;
   
   aa3_channels = 2;
   aa3_samplerate = 44100;
   if ( aa3_type == TYPE_ATRAC3 )
      aa3_data_align = ea3_header[0x23]*8;
   else
      aa3_data_align = (ea3_header[0x23]+1)*8;
   
   aa3_data_start = 0x0C60;
   aa3_data_size = sceIoLseek32(aa3_handle, 0, PSP_SEEK_END) - aa3_data_start;
   
   if ( aa3_data_size % aa3_data_align != 0 )
      goto wait;
   
   sceIoLseek32(aa3_handle, aa3_data_start, PSP_SEEK_SET);
   
   memset(aa3_codec_buffer, 0, sizeof(aa3_codec_buffer));
   
   if ( aa3_type == TYPE_ATRAC3 ) {
      aa3_channel_mode = 0x0;
      if ( aa3_data_align == 0xC0 ) // atract3 have 3 bitrate, 132k,105k,66k, 132k align=0x180, 105k align = 0x130, 66k align = 0xc0
         aa3_channel_mode = 0x1;
      aa3_sample_per_frame = 1024;
      aa3_data_buffer = (u8*)memalign(64, 0x180);
      if ( aa3_data_buffer == NULL)
         goto wait;
      aa3_codec_buffer[26] = 0x20;
      if ( sceAudiocodecCheckNeedMem(aa3_codec_buffer, 0x1001) < 0 )
         goto wait;
      if ( sceAudiocodecGetEDRAM(aa3_codec_buffer, 0x1001) < 0 )
         goto wait;
      aa3_getEDRAM = 1;
      aa3_codec_buffer[10] = 4;
      aa3_codec_buffer[44] = 2;
      if ( aa3_data_align == 0x130 )
         aa3_codec_buffer[10] = 6;
      if ( sceAudiocodecInit(aa3_codec_buffer, 0x1001) < 0 ) {
         goto wait;
      }
   }
   else if ( aa3_type == TYPE_ATRAC3PLUS ) {
      aa3_sample_per_frame = 2048;
      int temp_size = aa3_data_align+8;
      int mod_64 = temp_size & 0x3f;
      if (mod_64 != 0) temp_size += 64 - mod_64;
      aa3_data_buffer = (u8*)memalign(64, temp_size);
      if ( aa3_data_buffer == NULL)
         goto wait;
      aa3_codec_buffer[5] = 0x1;
      aa3_codec_buffer[10] = aa3_at3plus_flagdata[1];
      aa3_codec_buffer[10] = (aa3_codec_buffer[10] << 8 ) | aa3_at3plus_flagdata[0];
      aa3_codec_buffer[12] = 0x1;
      aa3_codec_buffer[14] = 0x1;
      if ( sceAudiocodecCheckNeedMem(aa3_codec_buffer, 0x1000) < 0 )
         goto wait;
      if ( sceAudiocodecGetEDRAM(aa3_codec_buffer, 0x1000) < 0 )
         goto wait;
      aa3_getEDRAM = 1;
      if ( sceAudiocodecInit(aa3_codec_buffer, 0x1000) < 0 ) {
         goto wait;
      }
   }
   else
      goto wait;
   
   int eof = 0;   
   while( !eof ) {
      int samplesdecoded;
      memset(aa3_mix_buffer, 0, 2048*2*2);
      unsigned long decode_type = 0x1001;
      if ( aa3_type == TYPE_ATRAC3 ) {
         memset( aa3_data_buffer, 0, 0x180);
         if (sceIoRead( aa3_handle, aa3_data_buffer, aa3_data_align ) != aa3_data_align) {
            eof = 1;
            continue;
         }
         if ( aa3_channel_mode ) {
            memcpy(aa3_data_buffer+aa3_data_align, aa3_data_buffer, aa3_data_align);
         }
         decode_type = 0x1001;
      }
      else {
         memset( aa3_data_buffer, 0, aa3_data_align+8);
         aa3_data_buffer[0] = 0x0F;
         aa3_data_buffer[1] = 0xD0;
         aa3_data_buffer[2] = aa3_at3plus_flagdata[0];
         aa3_data_buffer[3] = aa3_at3plus_flagdata[1];
         if (sceIoRead( aa3_handle, aa3_data_buffer+8, aa3_data_align ) != aa3_data_align) {
            eof = 1;
            continue;
         }
         decode_type = 0x1000;
      }
   
      aa3_codec_buffer[6] = (unsigned long)aa3_data_buffer;
      aa3_codec_buffer[8] = (unsigned long)aa3_mix_buffer;
   
      int res = sceAudiocodecDecode(aa3_codec_buffer, decode_type);
      if ( res < 0 ) {
         eof = 1;
         continue;
      }
      samplesdecoded = aa3_sample_per_frame;
   }

wait:
   
   if ( aa3_handle ) {
      sceIoClose(aa3_handle);
   }
   if ( aa3_data_buffer) {
      free(aa3_data_buffer);
   }
   if ( aa3_getEDRAM ) {
      sceAudiocodecReleaseEDRAM(aa3_codec_buffer);
   }
   
   sceCtrlReadBufferPositive(&input, 1);
   while(!(input.Buttons & PSP_CTRL_TRIANGLE))
   {
      sceKernelDelayThread(10000);   // wait 10 milliseconds
      sceCtrlReadBufferPositive(&input, 1);
   }
   
   sceKernelExitGame();
   return 0;
}
#endif
