/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "Timer.h"
#include "System.h"
#include "DebugMenu.h"

/**** Globals ****/

int Timer::avgSamples = AVERAGE_SAMPLES;

MenuItemFloat playRate(1.0f, 1.0f, 0.0f, 100.0f);

/**** Functions ****/

void UpdateRate(MenuObject *pObject, void *pData)
{
	gSystemTimer.SetRate(((MenuItemFloat*)pObject)->data);
}

void Timer_InitModule()
{
	DebugMenu_AddItem("Game Playback Rate", "Fuji Options", &playRate, UpdateRate);
}

void Timer_DeinitModule()
{

}

void Timer::Init()
{
	accumulator=0;
	lastUpdate=0;

	thisCall = lastCall = RDTSC();
	freq = GetTSCFrequency();

	FPS=0.0;

	memset(history, 0, sizeof(history));
	avgSamples = 0;

	deltaD=0.0;
	deltaF=0.0f;

	rate=1.0f;

	fixed=false;
}


void Timer::Update()
{
	if(flags&Timer_Paused)
	{
		deltaD=0.0;
		deltaF=0.0f;

		FPS=0.0;
	}
	else
	{
		if(fixed)
		{
			lastCall=thisCall;
			thisCall = RDTSC();

			lastUpdate=accumulator;
			accumulator+=freq/fixedFPS;
		}
		else
		{
			lastCall=thisCall;
			thisCall = RDTSC();

			lastUpdate=accumulator;
			accumulator+=(uint64)((thisCall-lastCall)*rate);
		}

		deltaD=(double)(accumulator-lastUpdate)/freq;
		deltaF=(float)deltaD;

		if(avgSamples)
		{
			current++;
			current%=avgSamples;

			history[current]=1.0/((double)(thisCall-lastCall)/freq);

			FPS=0;

			for(int a=0; a<avgSamples; a++)
			{
				FPS+=history[a];
			}

			FPS/=(double)avgSamples;
		}
		else
		{
			FPS=1.0/((double)(thisCall-lastCall)/freq);
		}
	}
}

void Timer::Reset()
{
	accumulator=0;
	lastUpdate=0;

	lastCall = RDTSC();

	FPS=0.0;
	deltaD=0.0;
	deltaF=0.0f;
}

void Timer::Pause(bool pause)
{
	if(pause)
	{
		FLAG(flags, Timer_Paused);
	}
	else
	{
		UNFLAG(flags, Timer_Paused);
		thisCall = RDTSC();
	}
}
