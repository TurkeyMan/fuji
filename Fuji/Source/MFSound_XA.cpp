#include "Fuji_Internal.h"

#if defined(XA_STREAM)

#include "MFSound_Internal.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemCachedFile_Internal.h"
#include "MFHeap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#define NO_XA_HEADER

#define USE_FXD 1

#define kNumOfSamples   224
#define kNumOfSGs       18
#define TTYWidth		80

#ifndef max
#define max(a,b) ( ((a) > (b)) ? (a) : (b))
#endif /* max */

#ifndef min
#define min(a,b) ( ((a) < (b)) ? (a) : (b))
#endif /* min */

/* ADPCM */
#define XA_DATA_START   (0x44-48)

/* WAVE */
#define WAV_DATA_START  0x2c

enum {
        FALSE,
		TRUE
};

#define FXD_FxdToPCM(dt)        (max(min((short)((dt)>>16), 32767), -32768))
#define DblToPCM(dt)            (short)(max(min((dt), 32767), -32768))

#define WHP_READ68_AUTO(fp, dt)         WHP_Read68(dt, sizeof(*(dt)), 1, fp)
#define WHP_WRITE68_AUTO(fp, dt)        WHP_Write68(dt, sizeof(*(dt)), 1, fp)

#define WHP_CNV_SHORT68(dt, ndt)                WHP_CnvEndianShort((dt), (ndt))
#define WHP_CNV_LONG68(dt, ndt)                 WHP_CnvEndianLong((dt), (ndt))

#if USE_FXD
#define FXD_FxdToPcm16(dt)      (max(min((dt)/2, 32767), -32768))
#define FXD_Pcm16ToFxd(dt)      ((long)dt*2)
#endif

typedef int BOOL;

typedef char SoundGroup[128];

typedef struct SoundSector {
	char            sectorFiller[48];
	SoundGroup      SoundGroups[18];
} SoundSector;

typedef unsigned long DWORD;
typedef unsigned short WORD;

typedef struct {
	        char id[4];
			DWORD size;
} CHK_HD;

typedef struct {
	WORD formatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD nbitsPerSample;
} WAVE_FMT;

/*      PCM  */
/* LCH this is actually properly in WAVE_FMT, which means that "extra bytes" is 0, the "fmt " header is 16 bytes exactly.
typedef struct {
	WORD nbitsPerSample;
} WAVE_PCM_SPEC;
*/

#if USE_FXD
typedef long FXD;
#endif

char name[4096];
long flen;

SoundSector ssct;
char decodeBuf[kNumOfSamples*2];

#if USE_FXD
static FXD      K0[4] = {
	0x00000000,
	0x0000F000,
	0x0001CC00,
	0x00018800
};
static FXD      K1[4] = {
	0x00000000,
	0x00000000,
	0xFFFF3000,
	0xFFFF2400
};
FXD t1, t2;
FXD t1_x, t2_x;
#else
static double   K0[4] = {
	0.0,
	0.9375,
	1.796875,
	1.53125
};
static double   K1[4] = {
	0.0,
	0.0,
	-0.8125,
	-0.859375
};
double t1, t2;
double t1_x, t2_x;
#endif

BOOL convXaToWave(FILE *adp, char *wave_name, int cn, int fn_s, int fn_e);
BOOL checkXaHeader(char *xa_hdr);
long decodeSoundSect(SoundSector *ssct, FILE *wav);
long decodeSoundSect1(SoundSector *ssct, FILE *wav);
FILE *openWaveFile(char *fname, int channelNumber, int blobNumber);
void writeWaveHeader(FILE *wav, DWORD sampleRate, int channels, long totalSize);
signed char getSoundData(char *buf, long unit, long sample);
signed char getFilter(char *buf, long unit);
signed char getRange(char *buf, long unit);
#if USE_FXD
FXD FXD_FixMul(FXD a, FXD b);
#endif

int main(int argc, char *argv[])
{
	FILE *adp;
	int cn=0, fn;

	if (argc < 3 || argc > 5) {
		fprintf(stderr,
			"Usage : %s xaOrStrFile wavfileBasename [channel [ fileno ] ]\n", argv[0]);
		exit(1);
	}

	if(strcmp(argv[1],"-"))
	{
		adp = fopen(argv[1], "rb");
		if (!adp)
		{
			perror(argv[0]);
			exit(1);
		}
		fseek(adp,0,SEEK_END);
		flen=ftell(adp);
		fseek(adp,0,SEEK_SET);
	}
	else
	{
		flen=1;
		adp=stdin;
	}
	if (argc > 3)
	{
		cn = atoi(argv[3]);
/*
		if (cn < 0 || 31 < cn)
		{
			fprintf(stderr, "Channel must be betwwen 0 and 31\n");
			exit(2);
		}
*/
	}
	if (argc == 3)
	{
		if(adp==stdin)
		{
			fprintf(stderr,"multichannel decoding is not available for stdout\n");
			exit(1);
		}
		for (cn = 0; cn < 256; ++cn)
		{
			if (convXaToWave(adp, argv[2], cn, 1, 255))
			{
				fclose(adp);
				exit(3);
			}
			fseek(adp, 0, SEEK_SET);
		}
	}
	else if (argc == 4)
	{
		convXaToWave(adp, argv[2], cn, 1, 255);
	}
	else
	{
		fn = atoi(argv[4]);
		if (fn < 1)
		{
			fprintf(stderr, "fn must be >= 1\n");
			exit(2);
		}
		convXaToWave(adp, argv[2], cn, fn, fn);
	}
	fclose(adp);
	return(0);
}

BOOL convXaToWave(FILE *adp, char *wave_name, int desiredChannelNumber, int fileNumberStart, int fileNumberEnd)
{
	long totalSize=0;
	BOOL retVal = TRUE;
	int fileNumber;
	FILE *wav=NULL;
	char xa_hdr[XA_DATA_START];
	int  audioChannels = 2;
	int sampleRate = 0;
	int submode;
	int coding;
	int channelNumber;
	int openedFileNumber = fileNumberStart;
	int openedSampleRate = -1;
	int openedAudioChannels = -1;
	int formatCounter = 0;

	t1 = t2 = 0;
	t1_x = t2_x = 0;
#ifndef NO_XA_HEADER
	if (fread(&xa_hdr, sizeof(xa_hdr), 1, adp) != 1) {
		fprintf(stderr, "Error read input file\n");
		return retVal;
	}
	if (checkXaHeader(xa_hdr)) {
		fprintf(stderr, "Input file not XA format\n");
		return retVal;
	}
#endif
	memset(&ssct, 0,sizeof(SoundSector));
	while (fread(&ssct, sizeof(SoundSector), 1, adp) == 1)
	{
		/*fprintf(stderr,"reading %d\n",ssct.sectorFiller[45]&0xff);
		{
			static int i,j;
			for(j=0;j<4;j++)
			{
				for(i=0;i<16;i++)
					fprintf(stderr,"%02x ",ssct.sectorFiller[i+(j*16)]&0xff);
				fprintf(stderr,"\n");
			}
		}*/

		fileNumber = ssct.sectorFiller[44];
		channelNumber = ssct.sectorFiller[45];
		submode = ssct.sectorFiller[46];
		coding = ssct.sectorFiller[47];


		audioChannels = (coding & 3) + 1;
		sampleRate = (((coding >> 2) & 3) == 1) ? 18900 : 37800;

#define SUBMODE_REAL_TIME_SECTOR (1 << 6)
#define SUBMODE_FORM             (1 << 5)
#define SUBMODE_AUDIO_DATA       (1 << 2)

		/*
		if (submode != (SUBMODE_REAL_TIME_SECTOR | SUBMODE_FORM | SUBMODE_AUDIO_DATA))
		{
		char submodeBuffer[12];
		char codingBuffer[12];
		sprintf(submodeBuffer, "%08x", submode);
		sprintf(codingBuffer, "%08x", coding);

		printf("cn%02d fn%02d s%2s c%s + %d\n", channelNumber, fileNumber, submodeBuffer + 6, codingBuffer + 6, totalSize);
		}
		*/

		if ((channelNumber == desiredChannelNumber) && (submode == (SUBMODE_REAL_TIME_SECTOR | SUBMODE_FORM | SUBMODE_AUDIO_DATA)))
		{
			/*
			LCH
			hard-coded to 2 channels
			otherwise this gets set to 1 at the end of a Spyro hunk
			and then the WAV header is goofed up.
			*/
			if (openedFileNumber != fileNumber)
			{
				if (fileNumber > fileNumberEnd)
					break;
				if (wav != NULL)
				{
					writeWaveHeader(wav, openedSampleRate, audioChannels, totalSize);
					fclose(wav);
					openedSampleRate = 0;
					openedAudioChannels = 0;
					formatCounter++;
					/*
					LCH
					fprintf(stderr,"\033[8;1H%d.%d) %ld\033[K\n",cn,ssct.sectorFiller[44]&0xff,totalSize);
					*/
					wav=NULL;
				}
				openedFileNumber = fileNumber;
			}
			if (openedFileNumber == fileNumber)
			{
				if (wav && ((sampleRate != openedSampleRate) || (audioChannels != openedAudioChannels)))
					{
					writeWaveHeader(wav, openedSampleRate, openedAudioChannels, totalSize);
					fclose(wav);
					openedSampleRate = 0;
					openedAudioChannels = 0;
					formatCounter++;
					/*
					LCH
					fprintf(stderr,"\033[8;1H%d.%d) %ld\033[K\n",cn,ssct.sectorFiller[44]&0xff,totalSize);
					*/
					wav=NULL;
					}
				if(!wav)
				{
					openedSampleRate = sampleRate;
					openedAudioChannels = audioChannels;
					wav = openWaveFile(wave_name, channelNumber, formatCounter);
					if(wav!=stdout)
						fseek(wav, WAV_DATA_START, SEEK_SET);
					totalSize = 0;
				}
				if (openedAudioChannels == 2)
					totalSize += decodeSoundSect1(&ssct, wav);
				else
					totalSize += decodeSoundSect(&ssct, wav);
				/*
				LCH
				if(adp==stdin)
					fprintf(stderr,"\033[8;1H%d.%d/%d) %ld %ld:%02ld\033[K\r",cn,fn,maxch,totalSize,totalSize/(37800*4)/60,totalSize/(37800*4)%60);
				else
					fprintf(stderr,"\033[8;1H%d.%d/%d) %ld@%ld/%ld % 3.2f%%\033[K\r",cn,ssct.sectorFiller[44]&0xff,maxch,totalSize,ftell(adp),flen,ftell(adp)/(float)flen*100);
				*/
				retVal = FALSE;
			}
		}
	}	
	if(wav)
	{
		if(wav!=stdout);
			writeWaveHeader(wav, openedSampleRate, openedAudioChannels, totalSize);
		fclose(wav);
		/*
		LCH
		fprintf(stderr,"\033[8;1H%d.%d) %ld\033[K\n",cn,fn,totalSize);
		*/
	}
	return retVal;
}

BOOL checkXaHeader(char *xa_hdr)
{
	if (memcmp(xa_hdr, "RIFF", 4))
		return TRUE;
	if (memcmp(&xa_hdr[8], "CDXA", 4))
		return TRUE;
	return FALSE;
}

long decodeSoundSect(SoundSector *ssct, FILE *wav)
{
	long count, outputBytes;
	signed char snddat, filt, range;
	short decoded;
	long unit, sample;
	long sndgrp;
#if USE_FXD
	FXD tmp2, tmp3, tmp4, tmp5;
#else
	double tmp2, tmp3, tmp4, tmp5;
#endif
	
	outputBytes = 0;

	for (sndgrp = 0; sndgrp < kNumOfSGs; sndgrp++)
	{
		count = 0;
		for (unit = 0; unit < 8; unit++)
		{
			range = getRange(ssct->SoundGroups[sndgrp], unit);
			filt = getFilter(ssct->SoundGroups[sndgrp], unit);
			for (sample = 0; sample < 28; sample++)
			{
				snddat = getSoundData(ssct->SoundGroups[sndgrp], unit, sample);
#if USE_FXD
				tmp2 = (long)(snddat) << (12 - range);
				tmp3 = FXD_Pcm16ToFxd(tmp2);
				tmp4 = FXD_FixMul(K0[filt], t1);
				tmp5 = FXD_FixMul(K1[filt], t2);
				t2 = t1;
				t1 = tmp3 + tmp4 + tmp5;
				decoded = FXD_FxdToPcm16(t1);
#else
				tmp2 = (double)(1 << (12 - range));
				tmp3 = (double)snddat * tmp2;
				tmp4 = t1 * K0[filt];
				tmp5 = t2 * K1[filt];
				t2 = t1;
				t1 = tmp3 + tmp4 + tmp5;
				decoded = DblToPCM(t1);
#endif
				decodeBuf[count++] = (char)(decoded & 0x0000ffff);
				decodeBuf[count++] = (char)(decoded >> 8);
			}
		}
		fwrite(decodeBuf, 1, sizeof(decodeBuf), wav);
		outputBytes += count;
	}
	return outputBytes;
}

long decodeSoundSect1(SoundSector *ssct, FILE *wav)
{
	long count, outputBytes;
	signed char snddat, filt, range;
	signed char filt1, range1;
	short decoded;
	long unit, sample;
	long sndgrp;
#if USE_FXD
	FXD tmp2, tmp3, tmp4, tmp5;
#else
	double tmp2, tmp3, tmp4, tmp5;
#endif

	outputBytes = 0;

	for (sndgrp = 0; sndgrp < kNumOfSGs; sndgrp++)
	{
		count = 0;
		for (unit = 0; unit < 8; unit+= 2)
		{
			range = getRange(ssct->SoundGroups[sndgrp], unit);
			filt = getFilter(ssct->SoundGroups[sndgrp], unit);
			range1 = getRange(ssct->SoundGroups[sndgrp], unit+1);
			filt1 = getFilter(ssct->SoundGroups[sndgrp], unit+1);

			for (sample = 0; sample < 28; sample++)
			{
				// Channel 1
				snddat = getSoundData(ssct->SoundGroups[sndgrp], unit, sample);
#if USE_FXD
				tmp2 = (long)(snddat) << (12 - range);
				tmp3 = FXD_Pcm16ToFxd(tmp2);
				tmp4 = FXD_FixMul(K0[filt], t1);
				tmp5 = FXD_FixMul(K1[filt], t2);
				t2 = t1;
				t1 = tmp3 + tmp4 + tmp5;
				decoded = FXD_FxdToPcm16(t1);
#else
				tmp2 = (double)(1 << (12 - range));
				tmp3 = (double)snddat * tmp2;
				tmp4 = t1 * K0[filt];
				tmp5 = t2 * K1[filt];
				t2 = t1;
				t1 = tmp3 + tmp4 + tmp5;
				decoded = DblToPCM(t1);
#endif
				decodeBuf[count++] = (char)(decoded & 0x0000ffff);
				decodeBuf[count++] = (char)(decoded >> 8);

				// Channel 2
				snddat = getSoundData(ssct->SoundGroups[sndgrp], unit+1, sample);
#if USE_FXD
				tmp2 = (long)(snddat) << (12 - range1);
				tmp3 = FXD_Pcm16ToFxd(tmp2);
				tmp4 = FXD_FixMul(K0[filt1], t1_x);
				tmp5 = FXD_FixMul(K1[filt1], t2_x);
				t2_x = t1_x;
				t1_x = tmp3 + tmp4 + tmp5;
				decoded = FXD_FxdToPcm16(t1_x);
#else
				tmp2 = (double)(1 << (12 - range1));
				tmp3 = (double)snddat * tmp2;
				tmp4 = t1_x * K0[filt1];
				tmp5 = t2_x * K1[filt1];
				t2_x = t1_x;
				t1_x = tmp3 + tmp4 + tmp5;
				decoded = DblToPCM(t1_x);
#endif
				decodeBuf[count++] = (char)(decoded & 0x0000ffff);
				decodeBuf[count++] = (char)(decoded >> 8);
			}
		}
		fwrite(decodeBuf, 1, sizeof(decodeBuf), wav);
		outputBytes += count;
	}
	return outputBytes;
}

FILE *openWaveFile(char *fname, int channelNumber, int blobNumber)
{
	FILE *wav;

	if(!strcmp(fname,"-"))
		wav=stdout;
	else
	{
		sprintf(name, "%s.%02d.%02d.wav", fname, channelNumber, blobNumber);
		fprintf(stderr, "%s\n", name);
		wav = fopen(name, "wb");
		if (wav == NULL) {
			fprintf(stderr, "Error creating %s\n", name);
			exit(1);
		}
	}
	return wav;
}

void writeWaveHeader(FILE *wav, DWORD cycle, int channels, long totalSize)
{
	static CHK_HD rifhd = {{'R','I','F','F'}};
	static char wave_id[] = {'W','A','V','E'};
	static CHK_HD fmthd = {{'f','m','t',' '}};
	/* LCH static WAVE_PCM_SPEC pcmspec; */
	/* LCH static */ WAVE_FMT fmt;
	static CHK_HD datahd = {{'d', 'a', 't', 'a'}};

	memset(&fmt, 0, sizeof(fmt)); /* LCH added this line */
	fmthd.size = sizeof(WAVE_FMT) /* LCH +sizeof(WAVE_PCM_SPEC) */;
	fmt.formatTag = 1;
	fmt.nChannels = channels;
	fmt.nSamplesPerSec = cycle;
	fmt.nAvgBytesPerSec = cycle * 2 * fmt.nChannels;
	fmt.nBlockAlign = 2 * fmt.nChannels;
	/* LCH pcmspec.nbitsPerSample = 16; */
	/* LCH added, instead */ fmt.nbitsPerSample = 16;
	datahd.size = totalSize;
	rifhd.size = sizeof(wave_id) +
		sizeof(fmthd) + fmthd.size + sizeof(datahd) + datahd.size;

	rewind(wav);
	fwrite(&rifhd, sizeof(rifhd), 1, wav);
	fwrite(wave_id, sizeof(wave_id), 1, wav);
	fwrite(&fmthd, sizeof(fmthd), 1, wav);
	fwrite(&fmt, sizeof(fmt), 1, wav);
	/* LCH fwrite(&pcmspec, sizeof(pcmspec), 1, wav); */
	fwrite(&datahd, sizeof(datahd), 1, wav);
}

signed char getSoundData(char *buf, long unit, long sample)
{
	signed char ret;
	char *p;
	long offset, shift;

	p = buf;
	shift = (unit%2) * 4;

	offset = 16 + (unit / 2) + (sample * 4);
	p += offset;

	ret = (*p >> shift) & 0x0F;

	if (ret > 7) {
		ret -= 16;
	}
	return ret;
}

signed char getFilter(char *buf, long unit)
{
	return (*(buf + 4 + unit) >> 4) & 0x03;
}


signed char getRange(char *buf, long unit)
{
	return *(buf + 4 + unit) & 0x0F;
}

#if USE_FXD
FXD FXD_FixMul(FXD a, FXD b)
{
	long                high_a, low_a, high_b, low_b;
	long                hahb, halb, lahb;
	unsigned long       lalb;
	FXD                 ret;

	high_a = a >> 16;
	low_a = a & 0x0000FFFF;
	high_b = b >> 16;
	low_b = b & 0x0000FFFF;

	hahb = (high_a * high_b) << 16;
	halb = high_a * low_b;
	lahb = low_a * high_b;
	lalb = (unsigned long)(low_a * low_b) >> 16;

	ret = hahb + halb + lahb + lalb;

	return ret;
}
#endif

struct MFXAStream
{
	WAVFormatChunk format;

	MFFile *pStream;

	size_t dataOffset;
	size_t dataSize;
	size_t sampleOffset;
	size_t sampleSize;
};

void DestroyXAStream(MFAudioStream *pStream)
{
	MFXAStream *pWS = (MFXAStream*)pStream->pStreamData;
	MFFile_Close(pWS->pStream);
	MFHeap_Free(pWS);
}

void CreateXAStream(MFAudioStream *pStream, const char *pFilename)
{
	MFCALLSTACK;

	// open XA file
	MFFile* hFile = MFFileSystem_Open(pFilename);
	if(!hFile)
		return;

	// attempt to cache the vorbis stream
	MFOpenDataCachedFile cachedOpen;
	cachedOpen.cbSize = sizeof(MFOpenDataCachedFile);
	cachedOpen.openFlags = MFOF_Read | MFOF_Binary | MFOF_Cached_CleanupBaseFile;
	cachedOpen.maxCacheSize = 256*1024; // 256k cache for wav stream should be heaps!!
	cachedOpen.pBaseFile = hFile;

	MFFile *pCachedFile = MFFile_Open(MFFileSystem_GetInternalFileSystemHandle(MFFSH_CachedFileSystem), &cachedOpen);
	if(pCachedFile)
		hFile = pCachedFile;

	RIFFHeader header;
	MFFile_Read(hFile, &header, sizeof(header));

	if(header.RIFF != MFMAKEFOURCC('R', 'I', 'F', 'F') || header.WAVE != MFMAKEFOURCC('W', 'A', 'V', 'E'))
		return;	// not a .wav file...

	// if everything's good, and it appears to be a valid wav file
	MFXAStream *pWS = (MFXAStream*)MFHeap_AllocAndZero(sizeof(MFXAStream));
	pStream->pStreamData = pWS;
	pWS->pStream = hFile;

	int read;
	int fptr = sizeof(header);
	do
	{
		MFFile_Seek(hFile, fptr, MFSeek_Begin);

		WAVChunk dataChunk;
		MFZeroMemory(&dataChunk, sizeof(dataChunk));
		read = MFFile_Read(hFile, &dataChunk, sizeof(dataChunk));
		fptr += sizeof(dataChunk) + dataChunk.size;

		if(dataChunk.id == MFMAKEFOURCC('f', 'm', 't', ' '))
		{
			read = MFFile_Read(hFile, &pWS->format, dataChunk.size);
			if(pWS->format.cbSize)
				read = MFFile_Seek(hFile, pWS->format.cbSize, MFSeek_Current);
		}
		else if(dataChunk.id == MFMAKEFOURCC('d', 'a', 't', 'a'))
		{
			pWS->dataOffset = MFFile_Tell(hFile);
			pWS->dataSize = dataChunk.size;
		}
	}
	while(read);

	// return to the start of the audio data
	MFFile_Seek(pWS->pStream, (int)pWS->dataOffset, MFSeek_Begin);

	// calculate the track length
	pWS->sampleSize = (pWS->format.nChannels * pWS->format.wBitsPerSample) >> 3;
	pStream->trackLength = (float)(pWS->dataSize / pWS->sampleSize) / (float)pWS->format.nSamplesPerSec;

	// fill out the stream info
	pStream->streamInfo.sampleRate = pWS->format.nSamplesPerSec;
	pStream->streamInfo.channels = pWS->format.nChannels;
	pStream->streamInfo.bitsPerSample = pWS->format.wBitsPerSample;
	pStream->streamInfo.bufferLength = pWS->format.nSamplesPerSec;
}

size_t GetXASamples(MFAudioStream *pStream, void *pBuffer, size_t bytes)
{
	MFXAStream *pWS = (MFXAStream*)pStream->pStreamData;
	size_t read = MFFile_Read(pWS->pStream, pBuffer, MFMin(bytes, pWS->dataSize - pWS->sampleOffset));
	pWS->sampleOffset += read;
	return read;
}

void SeekXAStream(MFAudioStream *pStream, float seconds)
{
	MFXAStream *pWS = (MFXAStream*)pStream->pStreamData;
	pWS->sampleOffset = (uint32)(seconds * (float)pWS->format.nSamplesPerSec) * pWS->sampleSize;
	MFFile_Seek(pWS->pStream, (int)(pWS->dataOffset + pWS->sampleOffset), MFSeek_Begin);
}

float GetXATime(MFAudioStream *pStream)
{
	MFXAStream *pWS = (MFXAStream*)pStream->pStreamData;
	return (float)(pWS->sampleOffset / pWS->sampleSize) / (float)pWS->format.nSamplesPerSec;
}

void MFSound_RegisterXA()
{
	MFStreamCallbacks callbacks;
	callbacks.pCreateStream = CreateXAStream;
	callbacks.pDestroyStream = DestroyXAStream;
	callbacks.pGetSamples = GetXASamples;
	callbacks.pGetTime = GetXATime;
	callbacks.pSeekStream = SeekXAStream;

	MFSound_RegisterStreamHandler("XA Audio", ".xa", &callbacks);
}
#endif
