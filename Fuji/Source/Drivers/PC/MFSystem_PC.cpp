#include "Fuji_Internal.h"

#if MF_SYSTEM == MF_DRIVER_PC

#include "MFSystem_Internal.h"
#include "MFHeap.h"
#include "MFThread.h"
#include "MFFileSystem.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// support delay loading of DLL's in windows build.
#pragma comment(lib, "delayimp")

extern MFInitParams gInitParams;

HINSTANCE apphInstance;

#if !defined(_FUJI_UTIL)
void MFSystem_InitModulePlatformSpecific()
{
	gpEngineInstance->currentPlatform = FP_Windows;

	apphInstance = (HINSTANCE)gInitParams.hInstance;

	// HACK: this is to force dual-core or multi-processor systems to use the first cpu for timing.
//	DWORD proc, system;
//	GetProcessAffinityMask(GetCurrentProcess(), &proc, &system);
	SetProcessAffinityMask(GetCurrentProcess(), 1);

	if(gDefaults.system.threadPriority != MFPriority_Normal)
	{
		int priority = THREAD_PRIORITY_NORMAL;

		switch(gDefaults.system.threadPriority)
		{
			case MFPriority_Normal:			priority = THREAD_PRIORITY_NORMAL;			break;
			case MFPriority_AboveNormal:	priority = THREAD_PRIORITY_ABOVE_NORMAL;	break;
			case MFPriority_BelowNormal:	priority = THREAD_PRIORITY_BELOW_NORMAL;	break;
			case MFPriority_Maximum:		priority = THREAD_PRIORITY_HIGHEST;			break;
			case MFPriority_Low:			priority = THREAD_PRIORITY_LOWEST;			break;
			case MFPriority_Idle:			priority = THREAD_PRIORITY_IDLE;			break;
			default:
				break;
		}

		SetThreadPriority(GetCurrentThread(), priority);
	}
}

void MFSystem_DeinitModulePlatformSpecific()
{
}

void MFSystem_HandleEventsPlatformSpecific()
{
	MFCALLSTACK;

	MSG msg;

	while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if(!GetMessage(&msg, NULL, 0, 0))
			gpEngineInstance->bQuit = true;
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void MFSystem_UpdatePlatformSpecific()
{
}

void MFSystem_DrawPlatformSpecific()
{
}
#endif

MF_API void MFSystem_SystemTime(MFSystemTime* pSystemTime)
{
	SYSTEMTIME time;
	GetSystemTime(&time);

	pSystemTime->year = time.wYear;
	pSystemTime->month = time.wMonth;
	pSystemTime->dayOfWeek = time.wDayOfWeek;
	pSystemTime->day = time.wDay;
	pSystemTime->hour = time.wHour;
	pSystemTime->minute = time.wMinute;
	pSystemTime->second = time.wSecond;
	pSystemTime->tenthMillisecond = time.wMilliseconds * 10;
}

MF_API void MFSystem_SystemTimeToFileTime(const MFSystemTime *pSystemTime, MFFileTime *pFileTime)
{
	SYSTEMTIME systime;
	FILETIME filetime;

	systime.wYear = pSystemTime->year;
	systime.wMonth = pSystemTime->month;
	systime.wDayOfWeek = pSystemTime->dayOfWeek;
	systime.wDay = pSystemTime->day;
	systime.wHour = pSystemTime->hour;
	systime.wMinute = pSystemTime->minute;
	systime.wSecond = pSystemTime->second;
	systime.wMilliseconds = pSystemTime->tenthMillisecond / 10;

	SystemTimeToFileTime(&systime, &filetime);

	pFileTime->ticks = (uint64)filetime.dwHighDateTime << 32 | (uint64)filetime.dwLowDateTime;
}

MF_API void MFSystem_FileTimeToSystemTime(const MFFileTime *pFileTime, MFSystemTime *pSystemTime)
{
	SYSTEMTIME systime;
	FILETIME filetime;

	filetime.dwLowDateTime = (DWORD)(pFileTime->ticks & 0xFFFFFFFF);
	filetime.dwHighDateTime = (DWORD)(pFileTime->ticks >> 32);

	FileTimeToSystemTime(&filetime, &systime);

	pSystemTime->year = systime.wYear;
	pSystemTime->month = systime.wMonth;
	pSystemTime->dayOfWeek = systime.wDayOfWeek;
	pSystemTime->day = systime.wDay;
	pSystemTime->hour = systime.wHour;
	pSystemTime->minute = systime.wMinute;
	pSystemTime->second = systime.wSecond;
	pSystemTime->tenthMillisecond = systime.wMilliseconds * 10;
}

MF_API void MFSystem_SystemTimeToLocalTime(const MFSystemTime *pSystemTime, MFSystemTime *pLocalTime)
{
	SYSTEMTIME systime;
	SYSTEMTIME localtime;

	TIME_ZONE_INFORMATION tz;
	GetTimeZoneInformation(&tz);

	systime.wYear = pSystemTime->year;
	systime.wMonth = pSystemTime->month;
	systime.wDayOfWeek = pSystemTime->dayOfWeek;
	systime.wDay = pSystemTime->day;
	systime.wHour = pSystemTime->hour;
	systime.wMinute = pSystemTime->minute;
	systime.wSecond = pSystemTime->second;
	systime.wMilliseconds = pSystemTime->tenthMillisecond / 10;

	SystemTimeToTzSpecificLocalTime(&tz, &systime, &localtime);

	pLocalTime->year = localtime.wYear;
	pLocalTime->month = localtime.wMonth;
	pLocalTime->dayOfWeek = localtime.wDayOfWeek;
	pLocalTime->day = localtime.wDay;
	pLocalTime->hour = localtime.wHour;
	pLocalTime->minute = localtime.wMinute;
	pLocalTime->second = localtime.wSecond;
	pLocalTime->tenthMillisecond = localtime.wMilliseconds * 10;
}

MF_API void MFSystem_LocalTimeToSystemTime(const MFSystemTime *pLocalTime, MFSystemTime *pSystemTime)
{
	SYSTEMTIME systime;
	SYSTEMTIME localtime;

	TIME_ZONE_INFORMATION tz;
	GetTimeZoneInformation(&tz);

	localtime.wYear = pLocalTime->year;
	localtime.wMonth = pLocalTime->month;
	localtime.wDayOfWeek = pLocalTime->dayOfWeek;
	localtime.wDay = pLocalTime->day;
	localtime.wHour = pLocalTime->hour;
	localtime.wMinute = pLocalTime->minute;
	localtime.wSecond = pLocalTime->second;
	localtime.wMilliseconds = pLocalTime->tenthMillisecond / 10;

	TzSpecificLocalTimeToSystemTime(&tz, &localtime, &systime);

	pSystemTime->year = systime.wYear;
	pSystemTime->month = systime.wMonth;
	pSystemTime->dayOfWeek = systime.wDayOfWeek;
	pSystemTime->day = systime.wDay;
	pSystemTime->hour = systime.wHour;
	pSystemTime->minute = systime.wMinute;
	pSystemTime->second = systime.wSecond;
	pSystemTime->tenthMillisecond = systime.wMilliseconds * 10;
}

MF_API uint64 MFSystem_ReadRTC()
{
	uint64 tickCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&tickCount);
	return tickCount;
}

MF_API uint64 MFSystem_GetRTCFrequency()
{
	uint64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	return freq;
}

MF_API const char * MFSystem_GetSystemName()
{
	wchar_t wbuffer[128];
	static char buffer[128];
	DWORD bufferSize = sizeof(buffer);
	GetComputerName(wbuffer, &bufferSize);
	MFString_CopyUTF16ToUTF8(buffer, wbuffer);
	return buffer;
}

const char * MFSystemPC_GetLastError()
{
	wchar_t errorMessage[256];
//	int len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), LANG_USER_DEFAULT, errorMessage, sizeof(errorMessage), NULL);
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), LANG_USER_DEFAULT, errorMessage, sizeof(errorMessage), NULL);
	return MFString_WCharAsUTF8(errorMessage);
}

#if _MSC_VER == 1700
	#include <intrin.h>
	#pragma intrinsic (_InterlockedIncrement)
	#pragma intrinsic (_InterlockedDecrement)

	extern "C"
	{
		long _Atomic_fetch_add_4(long volatile *addend)
		{
			return _InterlockedIncrement(addend);
		}

		long _Atomic_fetch_sub_4(long volatile *addend)
		{
			return _InterlockedDecrement(addend);
		}
	}
#endif

#endif // MF_SYSTEM
