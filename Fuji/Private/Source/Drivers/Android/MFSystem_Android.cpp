#include "Fuji.h"

#if MF_SYSTEM == MF_DRIVER_ANDROID

#include <jni.h>
#include <errno.h>

#include <sys/time.h>

#include "android_native_app_glue.h"

#include "MFSystem_Internal.h"
#include "MFHeap.h"
#include "MFThread.h"

extern MFInitParams gInitParams;

extern int gQuit;

MFPlatform gCurrentPlatform = FP_Android;

static android_app* gpApplication;

char *gpCommandLineBuffer = NULL;

#if !defined(_FUJI_UTIL)
void MFSystem_InitModulePlatformSpecific()
{
}

void MFSystem_DeinitModulePlatformSpecific()
{
}

void MFSystem_HandleEventsPlatformSpecific()
{
	// Use -1 here to block infinitely waiting for events,
	// 0 for non-blocking poll
	// or a value in milliseconds to specify a timeout.
	const int timeout = 0;

	// Read all pending events.
	int result;
	int events;
	android_poll_source* pSource;

	for(;;)
	{
		const int result = ALooper_pollAll(timeout, 0, & events, reinterpret_cast< void** >(&pSource));

		if(result < 0)
			break;

		// Process this event.
		if(pSource)
			pSource->process(gpApplication, pSource);
	}
}

void MFSystem_UpdatePlatformSpecific()
{
}

void MFSystem_DrawPlatformSpecific()
{
}
#endif

MF_API uint64 MFSystem_ReadRTC()
{
	// CRAPPY TIMER CODE I TOOK FROM LINUX
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
	static char buffer[128] = "Android";
//	uint32 bufferSize = sizeof(buffer);
	//...
	return buffer;
}


// Process the next main command.
static void android_handle_command(struct android_app* app, int32_t cmd)
{
	switch(cmd)
	{
		case APP_CMD_INPUT_CHANGED:
			/**
			* Command from main thread: the AInputQueue has changed.  Upon processing
			* this command, android_app->inputQueue will be updated to the new queue
			* (or NULL).
			*/
			break;

		case APP_CMD_INIT_WINDOW:
			/**
			* Command from main thread: a new ANativeWindow is ready for use.  Upon
			* receiving this command, android_app->window will contain the new window
			* surface.
			*/
			break;

		case APP_CMD_TERM_WINDOW:
			/**
			* Command from main thread: the existing ANativeWindow needs to be
			* terminated.  Upon receiving this command, android_app->window still
			* contains the existing window; after calling android_app_exec_cmd
			* it will be set to NULL.
			*/
			break;

		case APP_CMD_WINDOW_RESIZED:
			/**
			* Command from main thread: the current ANativeWindow has been resized.
			* Please redraw with its new size.
			*/
			break;

		case APP_CMD_WINDOW_REDRAW_NEEDED:
			/**
			* Command from main thread: the system needs that the current ANativeWindow
			* be redrawn.  You should redraw the window before handing this to
			* android_app_exec_cmd() in order to avoid transient drawing glitches.
			*/
			break;

		case APP_CMD_CONTENT_RECT_CHANGED:
			/**
			* Command from main thread: the content area of the window has changed,
			* such as from the soft input window being shown or hidden.  You can
			* find the new content rect in android_app::contentRect.
			*/
			break;

		case APP_CMD_GAINED_FOCUS:
			/**
			* Command from main thread: the app's activity window has gained
			* input focus.
			*/
			break;

		case APP_CMD_LOST_FOCUS:
			/**
			* Command from main thread: the app's activity window has lost
			* input focus.
			*/
			break;

		case APP_CMD_CONFIG_CHANGED:
			/**
			* Command from main thread: the current device configuration has changed.
			*/
			break;

		case APP_CMD_LOW_MEMORY:
			/**
			* Command from main thread: the system is running low on memory.
			* Try to reduce your memory use.
			*/
			break;

		case APP_CMD_START:
			/**
			* Command from main thread: the app's activity has been started.
			*/
			break;

		case APP_CMD_RESUME:
			/**
			* Command from main thread: the app's activity has been resumed.
			*/
			break;

		case APP_CMD_SAVE_STATE:
			/**
			* Command from main thread: the app should generate a new saved state
			* for itself, to restore from later if needed.  If you have saved state,
			* allocate it with malloc and place it in android_app.savedState with
			* the size in android_app.savedStateSize.  The will be freed for you
			* later.
			*/
			break;

		case APP_CMD_PAUSE:
			/**
			* Command from main thread: the app's activity has been paused.
			*/
			break;

		case APP_CMD_STOP:
			/**
			* Command from main thread: the app's activity has been stopped.
			*/
			break;

		case APP_CMD_DESTROY:
			/**
			* Command from main thread: the app's activity is being destroyed,
			* and waiting for the app thread to clean up and exit before proceeding.
			*/
			break;
	}
}

static int32_t android_handle_input(struct android_app* app, AInputEvent* pEvent)
{
	switch(AInputEvent_getType(pEvent))
	{
		case AINPUT_EVENT_TYPE_KEY:
			/* Indicates that the input event is a key event. */
			break;

		case AINPUT_EVENT_TYPE_MOTION:
			/* Indicates that the input event is a motion event. */
			break;
	}

	return 0;
}

// This is the function that application code must implement, representing
// the main entry to the app.
void android_main(struct android_app* pApplication) 
{
	gpApplication = pApplication;

	// Dummy function you can call to ensure glue code isn't stripped.
	app_dummy();

	// The application can place a pointer to its own state object
	// here if it likes.
	pApplication->userData = 0;

	// Fill this in with the function to process main app commands (APP_CMD_*)
	pApplication->onAppCmd = android_handle_command;

	// Fill this in with the function to process input events.  At this point
	// the event has already been pre-dispatched, and it will be finished upon
	// return.  Return 1 if you have handled the event, 0 for any default
	// dispatching.
	pApplication->onInputEvent = android_handle_input;

	// Enter main()
	int main(int argc, const char *argv[]);
	main(0, NULL);
}

#endif // MF_SYSTEM
