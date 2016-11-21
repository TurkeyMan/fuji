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

inline tm MFTimeToTm(const MFSystemTime &t)
{
	tm r;
	t.tm_year = t.year - 1900;
	t.tm_mon = t.month - 1;
	t.tm_wday = t.dayOfWeek;
	t.tm_mday = t.day;
	t.tm_hour = t.hour;
	t.tm_min = t.minute;
	t.tm_sec = t.second;
	t.tm_yday = 0;
	t.tm_isdst = 0;
	return r;
}
inline MFSystemTime TmToMFTime(const tm *t)
{
	MFSystemTime r;
	r.year = 1900 + t->tm_year;
	r.month = t->tm_mon + 1;
	r.dayOfWeek = t->tm_wday;
	r.day = t->tm_mday;
	r.hour = t->tm_hour;
	r.minute = t->tm_min;
	r.second = t->tm_sec;
	r.microsecond = 0;
	return r;
}

MF_API void MFSystem_SystemTime(MFSystemTime* pSystemTime)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	tm *utc = gmtime(&now.tv_sec);
	*pSystemTime = TmToMFTime(utc);
	pSystemTime->microsecond = now.tv_usec;
}

MF_API void MFSystem_SystemTimeToFileTime(const MFSystemTime *pSystemTime, MFFileTime *pFileTime)
{
	tm utc = MFTimeToTm(*pSystemTime);
	time_t t = timegm(&utc);
	*pFileTime = ((uint64)t << 32) | pSystemTime->microsecond;
}

MF_API void MFSystem_FileTimeToSystemTime(const MFFileTime *pFileTime, MFSystemTime *pSystemTime)
{
	time_t t = (time_t)(*pFileTime >> 32);
	tm *utc = gmtime(&t);
	*pSystemTime = TmToMFTime(utc);
	pSystemTime->microsecond = *pFileTime & 0xFFFFF;
}

MF_API void MFSystem_SystemTimeToLocalTime(const MFSystemTime *pSystemTime, MFSystemTime *pLocalTime)
{
	tm utc = MFTimeToTm(*pSystemTime);
	time_t t = timegm(&utc);
	tm *lt = localtime(&t);
	*pLocalTime = TmToMFTime(lt);
	pLocalTime->microsecond = pSystemTime->microsecond;
}

MF_API void MFSystem_LocalTimeToSystemTime(const MFSystemTime *pLocalTime, MFSystemTime *pSystemTime)
{
	tm lt = MFTimeToTm(*pLocalTime);
	time_t t = timelocal(&lt);
	tm *utc = gmtime(&t);
	*pSystemTime = TmToMFTime(utc);
	pSystemTime->microsecond = pLocalTime->microsecond;
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
