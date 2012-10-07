//===============================================================================================
// SIMPLE.EXE
// Copyright (c), Firelight Technologies, 2000-2004.
//
// This is a simple but descriptive way to get FMOD going, by loading a song and a few wav files
// and then playing them back.  It also shows how to do device enumeration and how to tweak
// various playback settings.
//
//===============================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#define USEMEMLOAD
//#define USEMEMLOADRESOURCE

#define USEFMOD TRUE

#ifdef USEFMOD
	#include "lib/minifmod.h"
#endif


// this is if you want to replace the samples with your own (in case you have compressed them)
void sampleloadcallback(void *buff, int lenbytes, int numbits, int instno, int sampno)
{
	printf("pointer = %p length = %d bits = %d instrument %d sample %d\n", buff, lenbytes, numbits, instno, sampno);
}



#ifndef USEMEMLOAD

unsigned int fileopen(char *name)
{
	return (unsigned int)fopen(name, "rb");
}

void fileclose(unsigned int handle)
{
	fclose((FILE *)handle);
}

int fileread(void *buffer, int size, unsigned int handle)
{
	return fread(buffer, 1, size, (FILE *)handle);
}

void fileseek(unsigned int handle, int pos, signed char mode)
{
	fseek((FILE *)handle, pos, mode);
}

int filetell(unsigned int handle)
{
	return ftell((FILE *)handle);
}

#else

typedef struct 
{
	int length;
	int pos;
	void *data;
} MEMFILE;


unsigned int memopen(char *name)
{
	MEMFILE *memfile;

	memfile = (MEMFILE *)calloc(sizeof(MEMFILE),1);

#ifndef USEMEMLOADRESOURCE
	{	// load an external file and read it
		FILE *fp;
		fp = fopen(name, "rb");
		if (fp)
		{
			fseek(fp, 0, SEEK_END);
			memfile->length = ftell(fp);
			memfile->data = calloc(memfile->length,1);
			memfile->pos = 0;

			fseek(fp, 0, SEEK_SET);
			fread(memfile->data, 1, memfile->length, fp);
			fclose(fp);
		}
	}
#else
	{	// hey look some load from resource code!
		HRSRC		rec;
		HGLOBAL		handle;

		rec = FindResource(NULL, name, RT_RCDATA);
		handle = LoadResource(NULL, rec);
		
		memfile->data = LockResource(handle);
		memfile->length = SizeofResource(NULL, rec);
		memfile->pos = 0;
	}
#endif

	return (unsigned int)memfile;
}

void memclose(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

#ifndef USEMEMLOADRESOURCE
	free(memfile->data);			// dont free it if it was initialized with LockResource
#endif

	free(memfile);
}

int memread(void *buffer, int size, unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (memfile->pos + size >= memfile->length)
		size = memfile->length - memfile->pos;

	memcpy(buffer, (char *)memfile->data+memfile->pos, size);
	memfile->pos += size;
	
	return size;
}

void memseek(unsigned int handle, int pos, signed char mode)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (mode == SEEK_SET) 
		memfile->pos = pos;
	else if (mode == SEEK_CUR) 
		memfile->pos += pos;
	else if (mode == SEEK_END)
		memfile->pos = memfile->length + pos;

	if (memfile->pos > memfile->length)
		memfile->pos = memfile->length;
}

int memtell(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	return memfile->pos;
}
#endif

/*
void songcallback(FMUSIC_MODULE *mod, unsigned char param)
{
	printf("order = %d, row = %d      \r", FMUSIC_GetOrder(mod), FMUSIC_GetRow(mod));
}
*/

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
void main(int argc, char *argv[])
{
#ifdef USEFMOD
	FMUSIC_MODULE *mod;

#ifndef USEMEMLOAD
	FSOUND_File_SetCallbacks(fileopen, fileclose, fileread, fileseek, filetell);
#else
	FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
#endif
#endif

	if (argc < 2)
	{
		printf("-------------------------------------------------------------\n");
		printf("MINIFMOD example XM player.\n");
		printf("Copyright (c) Firelight Technologies, 2000-2004.\n");
		printf("-------------------------------------------------------------\n");
		printf("Syntax: simplest infile.xm\n\n");
		return;
	}


#ifdef USEFMOD

	// ==========================================================================================
	// LOAD SONG
	// ==========================================================================================
	mod = FMUSIC_LoadSong(argv[1], NULL); //sampleloadcallback);
	if (!mod)
	{
		printf("Error loading song\n");
		return;
	}

	// ==========================================================================================
	// PLAY SONG
	// ==========================================================================================
	FMUSIC_PlaySong(mod);

#endif

	printf("Press any key to quit\n");
	printf("=========================================================================\n");
	printf("Playing song...\n");

	{
		char key = 0;
		do
		{
			int		ord = 0, row = 0;
			float	mytime = 0;
			if (kbhit())
			{
				key = getch();
			}

#ifdef USEFMOD
			ord = FMUSIC_GetOrder(mod);
			row = FMUSIC_GetRow(mod);
			mytime = (float)FMUSIC_GetTime(mod) / 1000.0f;
#endif

			printf("ord %2d row %2d seconds %5.02f %s      \r", ord, row, mytime, (row % 8 ? "    " : "TICK"));

		} while (key != 27);
	}


	printf("\n");

#ifdef USEFMOD
	FMUSIC_FreeSong(mod);
#endif
}
