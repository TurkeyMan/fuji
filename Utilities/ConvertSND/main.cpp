#include "Fuji.h"
#include "MFSystem.h"
#include "MFStringCache.h"
#include "MFSound_Internal.h"
#include "MFHeap.h"

#include "stdarg.h"

struct WAVChunk
{
	int id;
	long size;
};

struct WAVFormatChunk : public WAVChunk
{
	short wFormatTag;
	unsigned short nChannels;
	unsigned long nSamplesPerSec;
	unsigned long nAvgBytesPerSec;
	unsigned short wBlockAlign;
	unsigned short wBitsPerSample;
	unsigned short cbSize; 
};

struct WAVDataChunk : public WAVChunk
{
	char data[1];
};

struct WAVHeader
{
	int RIFF;
	long size;
	int WAVE;
};

int bytesPerSample[] =
{
	1,
	2,
	4
};

void LOGERROR(const char *pFormat, ...)
{
	va_list arglist;
	va_start(arglist, pFormat);

	vprintf(pFormat, arglist);
	getc(stdin);
}

int main(int argc, char *argv[])
{
	MFPlatform platform = FP_Unknown;
	MFWaveFormat targetFormat = MFWF_PCM_s16;

	char fileName[256] = "";
	char outFile[256] = "";

	int a;

	// process command line
	for(a=1; a<argc; a++)
	{
		if(argv[a][0] == '-' || argv[a][0] == '/')
		{
			for(int b=0; b<FP_Max; b++)
			{
				if(!MFString_CaseCmp(&argv[a][1], MFSystem_GetPlatformString(b)))
				{
					platform = (MFPlatform)b;
					break;
				}
			}

			if(!MFString_CaseCmp(&argv[a][1], "v") || !MFString_CaseCmp(&argv[a][1], "version"))
			{
				// print version
				return 0;
			}
		}
		else
		{
			if(!fileName[0])
				MFString_Copy(fileName, argv[a]);
			else if(!outFile[0])
				MFString_Copy(outFile, argv[a]);
		}
	}

	if(platform == FP_Unknown)
	{
		LOGERROR("No platform specified...\n");
		return 1;
	}

	if(!fileName[0])
	{
		LOGERROR("No file specified...\n");
		return 1;
	}

	if(!outFile[0])
	{
		// generate output filename
		MFString_Copy(outFile, fileName);
		for(int i=MFString_Length(outFile); --i; )
		{
			if(outFile[i] == '.')
			{
				outFile[i] = 0;
				break;
			}
		}

		MFString_Cat(outFile, ".snd");
	}

	// load image
	char *pBuffer;

	FILE *pFile = fopen(fileName, "rb");
	if(!pFile)
	{
		LOGERROR("Couldnt open source file '%s' for reading.", fileName);
		return 1;
	}

	fseek(pFile, 0, SEEK_END);
	int bytes = ftell(pFile) + 1;
	fseek(pFile, 0, SEEK_SET);

	pBuffer = (char*)malloc(bytes);
	fread(pBuffer, bytes, 1, pFile);
	fclose(pFile);

	FILE *pOutFile = fopen(outFile, "wb");

	if(!pOutFile)
	{
		LOGERROR("Couldnt open output file '%s' for writing.", outFile);
		return 2;
	}

	// parse the sound file...
	WAVHeader *pHeader = (WAVHeader*)pBuffer;

	if(pHeader->RIFF != MFMAKEFOURCC('R', 'I', 'F', 'F') || pHeader->WAVE != MFMAKEFOURCC('W', 'A', 'V', 'E'))
	{
		LOGERROR("Source file '%s' is not a wave file.", fileName);
		return 1;
	}

	WAVChunk *pChunk = (WAVChunk*)&pHeader[1];

	WAVFormatChunk *pFormat = NULL;
	WAVDataChunk *pData = NULL;

	while((char*)pChunk - pBuffer < bytes)
	{
		if(pChunk->id == MFMAKEFOURCC('f', 'm', 't', ' '))
		{
			pFormat = (WAVFormatChunk*)pChunk;
		}
		else if(pChunk->id == MFMAKEFOURCC('d', 'a', 't', 'a'))
		{
			pData = (WAVDataChunk*)pChunk;
		}

		pChunk = (WAVChunk*)((char*)pChunk + sizeof(WAVChunk) + ((pChunk->size+1) & ~1));
	}

	if(!pFormat || !pData)
	{
		LOGERROR("WAVE file '%s' has no format or data chunks.", fileName);
		return 2;
	}

	// we only output 1 stream for now..
	int numStreams = 1;
	int numSamples = ((pData->size*8) / ((pFormat->wBitsPerSample + 7) & ~7)) / pFormat->nChannels;

	int fileSize = sizeof(MFSoundTemplate) + sizeof(char*)*numStreams + bytesPerSample[targetFormat]*pFormat->nChannels*numSamples*numStreams;
	char *pSndFile = (char*)MFHeap_Alloc(fileSize);

	MFSoundTemplate *pTemplate = (MFSoundTemplate*)pSndFile;
	pTemplate->ppStreams = (char **)&pTemplate[1];
	char *pWaveData = (char*)&pTemplate->ppStreams[numStreams];
	for(int a=0; a<numStreams; a++)
	{
		pTemplate->ppStreams[a] = pWaveData;
		pWaveData += bytesPerSample[targetFormat]*pFormat->nChannels*numSamples;
	}

	pTemplate->magic = MFMAKEFOURCC('S', 'N', 'D', '1');
	pTemplate->flags = 0;
	pTemplate->format = targetFormat;
	pTemplate->numSamples = numSamples;
	pTemplate->numStreams = numStreams;
	pTemplate->numChannels = pFormat->nChannels;
	pTemplate->bitsPerSample = bytesPerSample[targetFormat]*8;
	pTemplate->sampleRate = pFormat->nSamplesPerSec;

	// write wave data into file
	if(pTemplate->bitsPerSample == pFormat->wBitsPerSample)
	{
		for(int a=0; a<pTemplate->numStreams; a++)
			MFCopyMemory(pTemplate->ppStreams[a], pData->data, bytesPerSample[targetFormat]*pTemplate->numChannels*pTemplate->numSamples);
	}
	else
	{
		// do a format conversion
		LOGERROR("Format conversion not written.");
		return 3;
	}

	// fix up template
	for(int a=0; a<numStreams; a++)
		MFFixUp(pTemplate->ppStreams[a], pTemplate, false);
	MFFixUp(pTemplate->ppStreams, pTemplate, false);

	// write to disk
	fwrite(pSndFile, fileSize, 1, pOutFile);
	fclose(pOutFile);

	printf("> %s\n", outFile);

	return 0;
}
