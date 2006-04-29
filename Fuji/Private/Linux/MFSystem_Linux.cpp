#include "Fuji.h"
#include "MFSystem_Internal.h"
#include "MFHeap.h"
#include "Input.h"

#include <sys/time.h>
#include <X11/Xlib.h>

extern Display *xdisplay;
extern Window window;
extern Atom wm_delete_window;

extern int gQuit;

MFPlatform gCurrentPlatform = FP_Linux;

int main(int argc, char *argv[])
{
	MFCALLSTACK;

	MFSystem_GameLoop();

	return 0;
}

void MFSystem_InitModulePlatformSpecific()
{
}

void MFSystem_DeinitModulePlatformSpecific()
{
}

void MFSystem_HandleEventsPlatformSpecific()
{
	CALLSTACK;

	XEvent event;
	
	while(XPending(xdisplay))
	{
		XNextEvent(xdisplay, &event);
		switch(event.type)
		{
			case ClientMessage:
				Atom atom;
				
				if(event.xclient.format == 8)
				{
					atom = event.xclient.data.b[0];
				}
				else if(event.xclient.format == 16)
				{
					atom = event.xclient.data.s[0];
				}
				else if(event.xclient.format == 32)
				{
					atom = event.xclient.data.l[0];
				}
				
				if(atom == wm_delete_window)
				{
					gQuit = 1;
				}
				break;
		}
	}
}

void MFSystem_UpdatePlatformSpecific()
{
}

void MFSystem_DrawPlatformSpecific()
{
}

uint64 MFSystem_ReadRTC()
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

uint64 MFSystem_GetRTCFrequency()
{
	MFCALLSTACK;

	return 1000000;
}

const char * MFSystem_GetSystemName()
{
	return "Insert code to find system name here...";
}
