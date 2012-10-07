/**** Defines ****/

#define RCP_60 (1.0/60.0)
#define RCP_60F (1.0f/60.0f)

/**** Includes ****/

#include "Fuji.h"
#include "MFSystem.h"
#include "Timer.h"
#include "DebugMenu.h"

/**** Globals ****/

MenuItemFloat playRate(1.0f, 1.0f, 0.0f, 100.0f);

Timer gSystemTimer;

/**** Functions ****/

void UpdateRate(MenuObject *pObject, void *pData)
{
	gSystemTimer.SetRate(((MenuItemFloat*)pObject)->data);
}

MFInitStatus Timer_InitModule()
{
	DebugMenu_AddItem("Game Playback Rate", "Fuji Options", &playRate, UpdateRate);

	return MFAIC_Succeeded;
}

void Timer_DeinitModule()
{

}

void Timer::Init(Timer *pRefTimer)
{
	accumulator = 0;
	lastUpdate = 0;

	thisCall = lastCall = MFSystem_ReadRTC();
	freq = MFSystem_GetRTCFrequency();

	FPS = 60.0;

	deltaD = 1.0/60.0;
	deltaF = 1.0f/60.0f;
	smoothDeltaF = 1.0f/60.0f;

	rate = 1.0f;

	fixed = false;

	pReferenceTimer = pRefTimer;
}

void Timer::Update()
{
	if(!(flags&Timer_Paused))
	{
		if(fixed)
		{
			lastCall = thisCall;
			thisCall = MFSystem_ReadRTC();

			lastUpdate = accumulator;
			accumulator += freq/fixedFPS;
		}
		else
		{
			lastCall = thisCall;
			thisCall = MFSystem_ReadRTC();

			lastUpdate = accumulator;
			accumulator += (uint64)((thisCall-lastCall)*rate);
		}

		deltaD = (double)(accumulator-lastUpdate)/freq;
		deltaF = (float)deltaD;
		smoothDeltaF = deltaF*RCP_60F + smoothDeltaF*RCP_60F*59.0f;

		if(1)
		{
			double newFPS = 1.0/((double)(thisCall-lastCall)/freq);

			FPS = newFPS*RCP_60 + FPS*RCP_60*59.0;
		}
		else
		{
			FPS = 1.0/((double)(thisCall-lastCall)/freq);
		}
	}
}

void Timer::Reset()
{
	accumulator = 0;
	lastUpdate = 0;

	lastCall = MFSystem_ReadRTC();

	smoothDeltaF = 1.0f/60.0f;
}

void Timer::Pause(bool pause)
{
	if(pause)
	{
		MFFLAG(flags, Timer_Paused);

		deltaD = 0.0;
		deltaF = 0.0f;
		smoothDeltaF = 0.0f;
		FPS = 0.0;
	}
	else
	{
		MFUNFLAG(flags, Timer_Paused);
		thisCall = MFSystem_ReadRTC();

		smoothDeltaF = 1.0f/60.0f;
		FPS = 60.0;
	}
}
