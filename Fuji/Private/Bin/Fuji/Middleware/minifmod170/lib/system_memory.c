/******************************************************************************/
/* SYSTEM_MEMORY.C                                                            */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Technologies will not support    */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) Firelight Technologies, 2000-2004.    */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Technologies is a registered company.                            */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "minifmod.h"
#include "system_memory.h"

extern void *(*FSOUND_Memory_AllocCallback)(unsigned int bytes) = NULL;
extern void (*FSOUND_Memory_FreeCallback)(void *pointer) = NULL;

void FSOUND_Memory_SetCallbacks(void *(*AllocCallback)(unsigned int bytes), void (*FreeCallback)(void *pointer))
{
	if (!AllocCallback || !FreeCallback)
	{
		FSOUND_Memory_AllocCallback = NULL;
		FSOUND_Memory_FreeCallback = NULL;
	}
	else
	{
		FSOUND_Memory_AllocCallback = AllocCallback;
		FSOUND_Memory_FreeCallback = FreeCallback;
	}
}

void *FSOUND_Memory_Alloc(unsigned int bytes)
{
	if(FSOUND_Memory_AllocCallback)
		return FSOUND_Memory_AllocCallback(bytes);

	return malloc(bytes);
}

void *FSOUND_Memory_Calloc(unsigned int bytes)
{
	if(FSOUND_Memory_AllocCallback)
	{
		void *mem = FSOUND_Memory_AllocCallback(bytes);
		memset(mem, 0, bytes);
		return mem;
	}

	return calloc(bytes, 1);
}

void FSOUND_Memory_Free(void *pointer)
{
	if(FSOUND_Memory_FreeCallback)
		FSOUND_Memory_FreeCallback(pointer);
	else
		free(pointer);
}
