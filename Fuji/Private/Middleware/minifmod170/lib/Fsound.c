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

#include "minifmod.h"
#include "mixer.h"
#include "mixer_clipcopy.h"
#include "mixer_fpu_ramp.h"
#include "music.h"
#include "sound.h"
#include "system_file.h"
#include "system_memory.h"

#include <windows.h>
#include <mmsystem.h>

//= GLOBALS ================================================================================

FSOUND_CHANNEL		FSOUND_Channel[256];             // channel pool 
int					FSOUND_MixRate      = 44100;     // mixing rate in hz.
int					FSOUND_BufferSizeMs = 1000;
HWAVEOUT			FSOUND_WaveOutHandle;
FSOUND_SoundBlock	FSOUND_MixBlock;

// mixing info
signed char		*	FSOUND_MixBufferMem;		// mix buffer memory block
signed char		*	FSOUND_MixBuffer;			// mix output buffer (16bit or 32bit)
float				FSOUND_OOMixRate;			// mixing rate in hz.
int					FSOUND_BufferSize;			// size of 1 'latency' ms buffer in bytes
int					FSOUND_BlockSize;			// LATENCY ms worth of samples

// thread control variables
volatile signed char	FSOUND_Software_Exit			= FALSE;		// mixing thread termination flag
volatile signed char	FSOUND_Software_UpdateMutex		= FALSE;
volatile signed char	FSOUND_Software_ThreadFinished	= TRUE;
volatile HANDLE			FSOUND_Software_hThread			= NULL;
volatile int			FSOUND_Software_FillBlock		= 0;
volatile int			FSOUND_Software_RealBlock;


/*
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
*/
void FSOUND_Software_Fill()
{
	void  *	mixbuffer; 
	int		mixpos		= FSOUND_Software_FillBlock * FSOUND_BlockSize;
	int		totalblocks = FSOUND_BufferSize / FSOUND_BlockSize; 


	mixbuffer = (char *)FSOUND_MixBuffer + (mixpos << 3);

	//==============================================================================
	// MIXBUFFER CLEAR
	//==============================================================================

	memset(mixbuffer, 0, FSOUND_BlockSize << 3);

	//==============================================================================
	// UPDATE MUSIC
	//==============================================================================

	{
		int MixedSoFar = 0;
		int MixedLeft = FMUSIC_PlayingSong->mixer_samplesleft;
		int SamplesToMix;
		signed char *MixPtr;

		// keep resetting the mix pointer to the beginning of this portion of the ring buffer
		MixPtr = mixbuffer;

		while (MixedSoFar < FSOUND_BlockSize)
		{
			if (!MixedLeft)
			{
				FMUSIC_PlayingSong->Update(FMUSIC_PlayingSong);		// update new mod tick
				SamplesToMix = FMUSIC_PlayingSong->mixer_samplespertick;
				MixedLeft = SamplesToMix;
			}
			else 
            {
                SamplesToMix = MixedLeft;
            }

			if (MixedSoFar + SamplesToMix > FSOUND_BlockSize) 
            {
				SamplesToMix = FSOUND_BlockSize - MixedSoFar;
            }

			FSOUND_Mixer_FPU_Ramp(MixPtr, SamplesToMix, FALSE); 

			MixedSoFar	+= SamplesToMix;
			MixPtr		+= (SamplesToMix << 3);
			MixedLeft	-= SamplesToMix;

			FMUSIC_PlayingSong->time_ms += (int)(((float)SamplesToMix * FSOUND_OOMixRate) * 1000);
		}

		FMUSIC_TimeInfo[FSOUND_Software_FillBlock].ms    = FMUSIC_PlayingSong->time_ms;
		FMUSIC_TimeInfo[FSOUND_Software_FillBlock].row   = FMUSIC_PlayingSong->row;
		FMUSIC_TimeInfo[FSOUND_Software_FillBlock].order = FMUSIC_PlayingSong->order;
		
		FMUSIC_PlayingSong->mixer_samplesleft = MixedLeft;
	}


	// ====================================================================================
	// CLIP AND COPY BLOCK TO OUTPUT BUFFER
	// ====================================================================================
	{
		void  			*ptr1, *ptr2; 
		unsigned int	len1, len2;

		ptr1 = FSOUND_MixBlock.data + (mixpos << 2);
		ptr2 = NULL;
		len1 = FSOUND_BlockSize << 2;
		len2 = 0;

		FSOUND_MixerClipCopy_Float32(ptr1, mixbuffer, len1>>2); 
	}

	FSOUND_Software_FillBlock++;

	if (FSOUND_Software_FillBlock >= totalblocks)
    {
		FSOUND_Software_FillBlock = 0;
    }
}


/*
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
*/
DWORD FSOUND_Software_DoubleBufferThread(LPDWORD lpdwParam)
{
	int totalblocks; 

	FSOUND_Software_ThreadFinished = FALSE;

	totalblocks = FSOUND_BufferSize / FSOUND_BlockSize; 

	while (!FSOUND_Software_Exit)
	{
		int		cursorpos,cursorblock,prevblock;
		MMTIME	mmt;

		mmt.wType = TIME_BYTES;
		waveOutGetPosition(FSOUND_WaveOutHandle, &mmt, sizeof(MMTIME));
		mmt.u.cb >>= 2;
		cursorpos = mmt.u.cb;

		cursorpos %= FSOUND_BufferSize;
		cursorblock = cursorpos / FSOUND_BlockSize;

		prevblock = cursorblock - 1;
		if (prevblock < 0)
        {
			prevblock = totalblocks - 1;
        }

		while (FSOUND_Software_FillBlock != cursorblock)
		{
			FSOUND_Software_UpdateMutex = TRUE;

			FSOUND_Software_Fill();
	
			FSOUND_Software_RealBlock++;
			if (FSOUND_Software_RealBlock >= totalblocks)
            {
				FSOUND_Software_RealBlock = 0;
            }

			FSOUND_Software_UpdateMutex = FALSE;
		}

		Sleep(5);
	};

	FSOUND_Software_ThreadFinished = TRUE;

	return 0;
}


