/******************************************************************************/
/* FMUSIC.C                                                                   */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Technologies will not support    */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) Firelight Technologies, 2000-2004.    */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Technologies is a registered company.                            */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#include <string.h>

#include "minifmod.h"

#include "mixer_fpu_ramp.h"
#include "music.h"
#include "music_formatxm.h"
#include "sound.h"
#include "system_file.h"
#include "system_memory.h"

FMUSIC_CHANNEL		FMUSIC_Channel[32];		// channel array for this song
FSOUND_SAMPLE		FMUSIC_DummySample;
FSOUND_CHANNEL		FMUSIC_DummyChannel;
FMUSIC_INSTRUMENT	FMUSIC_DummyInstrument;

//= PRIVATE FUNCTIONS ==============================================================================

void FMUSIC_SetBPM(FMUSIC_MODULE *module, int bpm)
{
	float hz;

	module->bpm = bpm;

	hz = (float)bpm * 2.0f / 5.0f;
	
	// number of samples
	module->mixer_samplespertick = (int)((float)FSOUND_MixRate * (1000.0f / hz) / 1000.0f);	
}


//= API FUNCTIONS ==============================================================================

/*
[API]
[
	[DESCRIPTION]
	To load a module with a given filename.  FMUSIC Supports loading of 
	- .MOD (protracker/fasttracker modules)
	- .S3M (screamtracker 3 modules)
	- .XM  (fasttracker 2 modules)
	- .IT  (impulse tracker modules)

	[PARAMETERS]
	'name'		Filename of module to load.
 
	[RETURN_VALUE]
	On success, a pointer to a FMUSIC_MODULE handle is returned.
	On failure, NULL is returned.

	[REMARKS]
	This function autodetects the format type, regardless of filename.

	[SEE_ALSO]
	FMUSIC_FreeSong
]
*/
FMUSIC_MODULE * FMUSIC_LoadSong(signed char *name, SAMPLELOADCALLBACK sampleloadcallback)
{
    FMUSIC_MODULE		*mod;
    signed char			retcode=FALSE;
    FSOUND_FILE_HANDLE	*fp;
	int					count;

    // create a mod instance
    mod = FSOUND_Memory_Calloc(sizeof(FMUSIC_MODULE));

    fp = FSOUND_File_Open(name, 0, 0);
    if (!fp)
    {
        return NULL;
    }

    mod->samplecallback = sampleloadcallback;

    // try opening all as all formats until correct loader is found
	retcode = FMUSIC_LoadXM(mod, fp);

    FSOUND_File_Close(fp);

    if (!retcode)
    {
        FMUSIC_FreeSong(mod);
        return NULL;
    }

	// ========================================================================================================
	// INITIALIZE SOFTWARE MIXER 
	// ========================================================================================================
	FSOUND_OOMixRate    = 1.0f / (float)FSOUND_MixRate;

	mix_volumerampsteps      = FSOUND_MixRate * FSOUND_VOLUMERAMP_STEPS / 44100;
	mix_1overvolumerampsteps = 1.0f / mix_volumerampsteps;

	//=======================================================================================
	// ALLOC GLOBAL CHANNEL POOL
	//=======================================================================================
	memset(FSOUND_Channel, 0, sizeof(FSOUND_CHANNEL) * 256);

	// ========================================================================================================
	// SET UP CHANNELS
	// ========================================================================================================
	for (count=0; count < 256; count++)
	{
		FSOUND_Channel[count].index = count;
		FSOUND_Channel[count].speedhi = 1;
	}

	mod->globalvolume       = mod->defaultglobalvolume;
 	mod->speed              = (int)mod->defaultspeed;
	mod->row                = 0;
	mod->order              = 0;
	mod->nextorder          = -1;
	mod->nextrow            = -1;
	mod->mixer_samplesleft  = 0;
	mod->tick               = 0;
	mod->patterndelay       = 0;
	mod->time_ms            = 0;

	FMUSIC_SetBPM(mod, mod->defaultbpm);

	memset(FMUSIC_Channel, 0, mod->numchannels * sizeof(FMUSIC_CHANNEL));

	for (count=0; count < mod->numchannels; count++)
	{
		FMUSIC_CHANNEL *cptr = &FMUSIC_Channel[count];
		cptr->cptr = &FSOUND_Channel[count];
	}

    return mod;
}


/*
[API]
[
	[DESCRIPTION]
	Frees memory allocated for a song and removes it from the FMUSIC system.

	[PARAMETERS]
	'mod'		Pointer to the song to be freed.
 
	[RETURN_VALUE]
	void

	[REMARKS]

	[SEE_ALSO]
	FMUSIC_LoadSong
]
*/
signed char FMUSIC_FreeSong(FMUSIC_MODULE *mod)
{
	int count;

	if (!mod) 
		return FALSE;

	// free samples
	if (mod->instrument)
	{
		for (count=0; count<(int)mod->numinsts; count++) 
		{
			int count2;

			FMUSIC_INSTRUMENT	*iptr = &mod->instrument[count];
			for (count2=0; count2<iptr->numsamples; count2++) 
			{
				if (iptr->sample[count2])
				{
					FSOUND_SAMPLE *sptr = iptr->sample[count2];
					FSOUND_Memory_Free(sptr->buff);
					FSOUND_Memory_Free(sptr);
				}
			}
		}
	}

	// free instruments
	if (mod->instrument)
    {
		FSOUND_Memory_Free(mod->instrument);
    }

	// free patterns
	if (mod->pattern)
	{
		for (count=0; count<mod->numpatternsmem; count++)
        {
			if (mod->pattern[count].data) 
            {
				FSOUND_Memory_Free(mod->pattern[count].data);
            }
        }

		if (mod->pattern) 
        {
			FSOUND_Memory_Free(mod->pattern);
        }
	}

	// free song
	FSOUND_Memory_Free(mod);

	return TRUE;
}

//= INFORMATION FUNCTIONS ======================================================================

/*
[API]
[
	[DESCRIPTION]
	Returns the song's current order number

	[PARAMETERS]
	'mod'		Pointer to the song to retrieve current order number from.
 
	[RETURN_VALUE]
	The song's current order number.
	On failure, 0 is returned.

	[REMARKS]

	[SEE_ALSO]
	FMUSIC_GetPattern
]
*/
int FMUSIC_GetOrder(FMUSIC_MODULE *mod)
{
	if (!mod)
		return 0;
	return mod->timeInfo.order;
}

/*
[API]
[
	[DESCRIPTION]
	Returns the song's current row number.

	[PARAMETERS]
 	'mod'		Pointer to the song to retrieve current row from.

	[RETURN_VALUE]
	On success, the song's current row number is returned.
	On failure, 0 is returned.

	[REMARKS]

	[SEE_ALSO]
]
*/
int FMUSIC_GetRow(FMUSIC_MODULE *mod)
{
	if (!mod) 
		return 0;
	return mod->timeInfo.row;
}

/*
[API]
[
	[DESCRIPTION]
	Returns the time in milliseconds since the song was started.  This is useful for
	synchronizing purposes becuase it will be exactly the same every time, and it is 
	reliably retriggered upon starting the song.  Trying to synchronize using other 
	windows timers can lead to varying results, and inexact performance.  This fixes that
	problem by actually using the number of samples sent to the soundcard as a reference.

	[PARAMETERS]
	'mod'		Pointer to the song to get time from.
 
	[RETURN_VALUE]
	On success, the time played in milliseconds is returned.
	On failure, 0 is returned.

	[REMARKS]

	[SEE_ALSO]
]
*/
unsigned int FMUSIC_GetTime(FMUSIC_MODULE *mod)
{
	if (!mod) 
		return 0;
	return mod->timeInfo.ms;
}
