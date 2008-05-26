/******************************************************************************/
/* FSOUND.C                                                                   */
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
#include "mixer.h"
#include "mixer_clipcopy.h"
#include "mixer_fpu_ramp.h"
#include "music.h"
#include "sound.h"
#include "system_file.h"
#include "system_memory.h"

//= GLOBALS ================================================================================

FSOUND_CHANNEL		FSOUND_Channel[256];             // channel pool 
int					FSOUND_MixRate      = 44100;     // mixing rate in hz.

// mixing info
float				FSOUND_OOMixRate;			// mixing rate in hz.
char				mixTemp[35280];

/*
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
*/
void FSOUND_Software_Fill(FMUSIC_MODULE *pMod, char *pBuffer, int bytes)
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

				FSOUND_Mixer_FPU_Ramp(MixPtr, SamplesToMix, FALSE); 

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
