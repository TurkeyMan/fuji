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
#include <stdlib.h>

#include "minifmod.h"
#include "music_formatxm.h"
#include "mixer_fpu_ramp.h"
#include "mixer_clipcopy.h"
#include "music.h"

FSOUND_SAMPLE		FMUSIC_DummySample;
FSOUND_CHANNEL		FMUSIC_DummyChannel;
FMUSIC_INSTRUMENT	FMUSIC_DummyInstrument;

char				mixTemp[35280];

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
	// INITIALIZE
	// ========================================================================================================
	mix_volumerampsteps      = FSOUND_MixRate * FSOUND_VOLUMERAMP_STEPS / 44100;
	mix_1overvolumerampsteps = 1.0f / mix_volumerampsteps;

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

	//=======================================================================================
	// ALLOC CHANNELS
	//=======================================================================================
	mod->FMUSIC_Channel = (FMUSIC_CHANNEL*)FSOUND_Memory_Calloc(sizeof(FMUSIC_CHANNEL) * mod->numchannels);
	memset(mod->FSOUND_Channel, 0, sizeof(FSOUND_CHANNEL) * 64);
	memset(mod->FMUSIC_Channel, 0, sizeof(FMUSIC_CHANNEL) * mod->numchannels);

	// ========================================================================================================
	// SET UP CHANNELS
	// ========================================================================================================
	for (count=0; count < 64; count++)
	{
		FSOUND_CHANNEL *pChannel = &mod->FSOUND_Channel[count];
		pChannel->index = count;
		pChannel->speedhi = 1;
		if(count < mod->numchannels)
			mod->FMUSIC_Channel[count].cptr = pChannel;
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

	// free channels
	if (mod->FMUSIC_Channel)
		FSOUND_Memory_Free(mod->FMUSIC_Channel);

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

/*
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
*/
void FMUSIC_GetSamples(FMUSIC_MODULE *pMod, char *pBuffer, int bytes)
{
	while(bytes)
	{
		int numSamples = min(bytes >> 2, sizeof(mixTemp)>>3);
		bytes -= numSamples << 2;

		//==============================================================================
		// MIXBUFFER CLEAR
		//==============================================================================
		memset(mixTemp, 0, numSamples << 3);

		//==============================================================================
		// UPDATE MUSIC
		//==============================================================================
		{
			int MixedSoFar = 0;
			int MixedLeft = pMod->mixer_samplesleft;
			int SamplesToMix;
			signed char *MixPtr = mixTemp;

			while (MixedSoFar < numSamples)
			{
				if (!MixedLeft)
				{
					pMod->Update(pMod);		// update new mod tick
					SamplesToMix = pMod->mixer_samplespertick;
					MixedLeft = SamplesToMix;
				}
				else 
				{
					SamplesToMix = MixedLeft;
				}

				if (MixedSoFar + SamplesToMix > numSamples) 
				{
					SamplesToMix = numSamples - MixedSoFar;
				}

				FSOUND_Mixer_FPU_Ramp(MixPtr, SamplesToMix, FALSE, pMod->FSOUND_Channel); 

				MixedSoFar	+= SamplesToMix;
				MixPtr		+= (SamplesToMix << 3);
				MixedLeft	-= SamplesToMix;

				pMod->time_ms += (int)(((float)SamplesToMix * FSOUND_OOMixRate) * 1000);
			}

			pMod->timeInfo.ms    = pMod->time_ms;
			pMod->timeInfo.row   = pMod->row;
			pMod->timeInfo.order = pMod->order;

			pMod->mixer_samplesleft = MixedLeft;
		}

		// ====================================================================================
		// CLIP AND COPY BLOCK TO OUTPUT BUFFER
		// ====================================================================================
		FSOUND_MixerClipCopy_Float32(pBuffer, mixTemp, numSamples);
		pBuffer += numSamples << 2;
	}
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
