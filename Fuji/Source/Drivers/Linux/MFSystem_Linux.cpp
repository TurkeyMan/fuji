#include "Fuji_Internal.h"

#if MF_SYSTEM == MF_DRIVER_LINUX

#include "MFSystem_Internal.h"
#include "MFHeap.h"
#include "MFInput.h"
#include "MFFileSystem.h"

#include <sys/time.h>
#include <sys/utsname.h>
#include <time.h>

#include <stdio.h>

#if MF_DISPLAY == MF_DRIVER_SDL2 || MF_INPUT == MF_DRIVER_SDL2 || MF_SOUND == MF_DRIVER_SDL2
	#include <SDL2/SDL.h>
#endif

#if !defined(_FUJI_UTIL)
void MFSystem_InitModulePlatformSpecific()
{
	gpEngineInstance->currentPlatform = FP_Linux;

#if MF_DISPLAY == MF_DRIVER_SDL2 || MF_INPUT == MF_DRIVER_SDL2 || MF_SOUND == MF_DRIVER_SDL2
	SDL_Init(0);
#endif
}

void MFSystem_DeinitModulePlatformSpecific()
{
#if MF_DISPLAY == MF_DRIVER_SDL2 || MF_INPUT == MF_DRIVER_SDL2 || MF_SOUND == MF_DRIVER_SDL2
	SDL_Quit();
#endif
}

void MFSystem_HandleEventsPlatformSpecific()
{
	MFCALLSTACK;

#if MF_DISPLAY == MF_DRIVER_X11
  void MFDisplay_HandleEventsX11();
  MFDisplay_HandleEventsX11();
#endif
}

void MFSystem_UpdatePlatformSpecific()
{
}

void MFSystem_DrawPlatformSpecific()
{
}
#endif // !defined(_FUJI_UTIL)

MF_API void MFSystem_SystemTime(MFSystemTime* pSystemTime)
{
	time_t now;
	time(&now);
	tm *utc = gmtime(&now);

	pSystemTime->year = 1900 + utc->tm_year;
	pSystemTime->month = utc->tm_mon + 1;
	pSystemTime->dayOfWeek = utc->tm_wday;
	pSystemTime->day = utc->tm_mday;
	pSystemTime->hour = utc->tm_hour;
	pSystemTime->minute = utc->tm_min;
	pSystemTime->second = utc->tm_sec;
	pSystemTime->tenthMillisecond = 0;
}

MF_API void MFSystem_SystemTimeToFileTime(const MFSystemTime *pSystemTime, MFFileTime *pFileTime)
{
	tm utc;
	utc.tm_year = pSystemTime->year - 1900;
	utc.tm_mon = pSystemTime->month - 1;
	utc.tm_mday = pSystemTime->day;
	utc.tm_hour = pSystemTime->hour;
	utc.tm_min = pSystemTime->minute;
	utc.tm_sec = pSystemTime->second;
	time_t t = timegm(&utc);
	pFileTime->ticks = (uint64)t;
}

MF_API void MFSystem_FileTimeToSystemTime(const MFFileTime *pFileTime, MFSystemTime *pSystemTime)
{
	time_t t = (time_t)pFileTime->ticks;
	tm *utc = gmtime(&t);

	pSystemTime->year = 1900 + utc->tm_year;
	pSystemTime->month = utc->tm_mon + 1;
	pSystemTime->dayOfWeek = utc->tm_wday;
	pSystemTime->day = utc->tm_mday;
	pSystemTime->hour = utc->tm_hour;
	pSystemTime->minute = utc->tm_min;
	pSystemTime->second = utc->tm_sec;
	pSystemTime->tenthMillisecond = 0;
}

MF_API void MFSystem_SystemTimeToLocalTime(const MFSystemTime *pSystemTime, MFSystemTime *pLocalTime)
{
	tm utc;
	utc.tm_year = pSystemTime->year - 1900;
	utc.tm_mon = pSystemTime->month - 1;
	utc.tm_mday = pSystemTime->day;
	utc.tm_hour = pSystemTime->hour;
	utc.tm_min = pSystemTime->minute;
	utc.tm_sec = pSystemTime->second;

	time_t t = timegm(&utc);
	tm *lt = localtime(&t);

	pLocalTime->year = 1900 + lt->tm_year;
	pLocalTime->month = lt->tm_mon + 1;
	pLocalTime->dayOfWeek = lt->tm_wday;
	pLocalTime->day = lt->tm_mday;
	pLocalTime->hour = lt->tm_hour;
	pLocalTime->minute = lt->tm_min;
	pLocalTime->second = lt->tm_sec;
	pLocalTime->tenthMillisecond = 0;
}

MF_API void MFSystem_LocalTimeToSystemTime(const MFSystemTime *pLocalTime, MFSystemTime *pSystemTime)
{
	tm lt;
	lt.tm_year = pLocalTime->year - 1900;
	lt.tm_mon = pLocalTime->month - 1;
	lt.tm_mday = pLocalTime->day;
	lt.tm_hour = pLocalTime->hour;
	lt.tm_min = pLocalTime->minute;
	lt.tm_sec = pLocalTime->second;

	time_t t = timelocal(&lt);
	tm *utc = gmtime(&t);

	pSystemTime->year = 1900 + utc->tm_year;
	pSystemTime->month = utc->tm_mon + 1;
	pSystemTime->dayOfWeek = utc->tm_wday;
	pSystemTime->day = utc->tm_mday;
	pSystemTime->hour = utc->tm_hour;
	pSystemTime->minute = utc->tm_min;
	pSystemTime->second = utc->tm_sec;
	pSystemTime->tenthMillisecond = 0;
}

MF_API uint64 MFSystem_ReadRTC()
{
	MFCALLSTACK;

	static bool firstcall=1;
	static struct timeval last;
	struct timeval current;

	if(firstcall)
	{
		gettimeofday(&last, NULL);
		firstcall = 0;
		return 0;
	}
	else
	{
		struct timeval diff;

		gettimeofday(&current, NULL);
		diff.tv_sec = current.tv_sec - last.tv_sec;
		diff.tv_usec = current.tv_usec - last.tv_usec;

		return ((uint64)diff.tv_sec) * 1000000 + ((uint64)diff.tv_usec);
	}
}

MF_API uint64 MFSystem_GetRTCFrequency()
{
	return 1000000; // microseconds
}

MF_API const char * MFSystem_GetSystemName()
{
	static utsname name;
	uname(&name);
	return name.nodename;
}

#endif
