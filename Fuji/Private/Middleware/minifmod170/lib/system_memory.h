/******************************************************************************/
/* SYSTEM_MEMORY.H                                                            */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Technologies will not support    */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) Firelight Technologies, 2000-2004.    */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Technologies is a registered company.                            */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#ifndef _SYSTEM_MEMORY_H_
#define _SYSTEM_MEMORY_H_

void	FSOUND_Memory_SetCallbacks(void *(*AllocCallback)(unsigned int bytes), void (*FreeCallback)(void *pointer));

void *	FSOUND_Memory_Alloc(unsigned int bytes);
void *	FSOUND_Memory_Calloc(unsigned int bytes);
void	FSOUND_Memory_Free(void *pointer);

extern void *(*FSOUND_Memory_AllocCallback)(unsigned int bytes);
extern void (*FSOUND_Memory_FreeCallback)(void *pointer);

#endif
