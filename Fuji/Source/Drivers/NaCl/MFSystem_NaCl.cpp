#include "Fuji_Internal.h"

#if MF_SYSTEM == MF_DRIVER_NACL

#include "MFSystem_NaCl.h"

#include <sys/time.h>

#include "MFSystem_Internal.h"
#include "MFHeap.h"
#include "MFThread.h"
#include "MFFileSystem.h"

extern MFInitParams gInitParams;

Fuji *FujiModule::pInstance = NULL;

const int32_t kUpdateInterval = 17;  // milliseconds

#if !defined(_FUJI_UTIL)
void MFSystem_InitModulePlatformSpecific()
{
	gpEngineInstance->currentPlatform = FP_NativeClient;
}

void MFSystem_DeinitModulePlatformSpecific()
{
}

void MFSystem_HandleEventsPlatformSpecific()
{
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
	MFDebug_Assert(false, "todo");
}

MF_API void MFSystem_SystemTimeToFileTime(const MFSystemTime *pSystemTime, MFFileTime *pFileTime)
{
	MFDebug_Assert(false, "todo");
}

MF_API void MFSystem_FileTimeToSystemTime(const MFFileTime *pFileTime, MFSystemTime *pSystemTime)
{
	MFDebug_Assert(false, "todo");
}

MF_API void MFSystem_SystemTimeToLocalTime(const MFSystemTime *pSystemTime, MFSystemTime *pLocalTime)
{
	MFDebug_Assert(false, "todo");
}

MF_API void MFSystem_LocalTimeToSystemTime(const MFSystemTime *pLocalTime, MFSystemTime *pSystemTime)
{
	MFDebug_Assert(false, "todo");
}

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
	static char buffer[128] = "NativeClient";
//	uint32 bufferSize = sizeof(buffer);
	//...
	return buffer;
}
#include <stdio.h>

UpdateScheduler::UpdateScheduler(int32_t delay, Fuji* fuji)
: delay_(delay), fuji_(fuji)
{
}

UpdateScheduler::~UpdateScheduler()
{
	pp::Core* core = pp::Module::Get()->core();
	core->CallOnMainThread(delay_, pp::CompletionCallback(UpdateCallback, fuji_));
}

// Callback that is called as a result of pp::Core::CallOnMainThread
void UpdateScheduler::UpdateCallback(void *pData, int32_t /*result*/)
{
	Fuji* pFuji = (Fuji*)pData;
	pFuji->Update();
}

Fuji::Fuji(PP_Instance instance)
: pp::Instance(instance)
{
	// Request to receive input events.
	RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_KEYBOARD);
}

Fuji::~Fuji()
{
	// Deinit the game
	void *pUserData;
	MFSystemCallbackFunction pCallback = MFSystem_GetSystemCallback(MFCB_Deinit, &pUserData);
	if(pCallback)
		pCallback(pUserData);

	// Deinit fuji
//	MFSystem_Deinit();
}

bool Fuji::Init(uint32_t argc, const char* argn[], const char* argv[])
{
	MFDebug_Message("Fuji::Init()");

	// Register the engine modules..
	MFModule_RegisterModules();

	// queue up the next update
	UpdateScheduler(kUpdateInterval, this);

	// Run one frame to kick off the init
	MFSystem_RunFrame();

	return true;
}

// Update the graphics context to the new size, and regenerate |pixel_buffer_|
// to fit the new size as well.
void Fuji::DidChangeView(const pp::Rect& position, const pp::Rect& clip)
{
MFDebug_Message("Fuji::DidChangeView");
//			pp::Size view_size = view_->GetSize();
//			const bool view_was_empty = view_size.IsEmpty();
//			view_->UpdateView(position, clip, this);
//			if (view_was_empty)
//				ResetPositions();

/*
int cube_width = cube_ ? cube_->width() : 0;
int cube_height = cube_ ? cube_->height() : 0;
if (position.size().width() == cube_width &&
  position.size().height() == cube_height)
return;  // Size didn't change, no need to update anything.

if (opengl_context_ == NULL)
opengl_context_.reset(new OpenGLContext(this));
opengl_context_->InvalidateContext(this);
opengl_context_->ResizeContext(position.size());
if (!opengl_context_->MakeContextCurrent(this))
return;
if (cube_ == NULL) {
cube_ = new Cube(opengl_context_);
cube_->PrepareOpenGL();
}
cube_->Resize(position.size().width(), position.size().height());
DrawSelf();
*/
}

bool Fuji::HandleInputEvent(const pp::InputEvent& event)
{
	if (event.GetType() == PP_INPUTEVENT_TYPE_MOUSEUP)
	{
		// By notifying the browser mouse clicks are handled, the application window
		// is able to get focus and receive key events.
		return true;
	}
	else if (event.GetType() == PP_INPUTEVENT_TYPE_KEYUP)
	{
		pp::KeyboardInputEvent key = pp::KeyboardInputEvent(event);
		//...
		return true;
	}
	else if (event.GetType() == PP_INPUTEVENT_TYPE_KEYDOWN)
	{
		pp::KeyboardInputEvent key = pp::KeyboardInputEvent(event);
		//...
		return true;
	}
	return false;
}

// Called by the browser to handle the postMessage() call in Javascript.
// The message in this case is expected to contain the string 'update', or
// 'resetScore' in order to invoke either the Update or ResetScore function
// respectively.
void Fuji::HandleMessage(const pp::Var& var_message)
{
	// handle message
}

void Fuji::Update()
{
	MFDebug_Message("Fuji::Update()");

	// Schedule another update
	UpdateScheduler(kUpdateInterval, this);

	MFSystem_RunFrame();
}

FujiModule::FujiModule()
: pp::Module()
{
}

FujiModule::~FujiModule()
{
	glTerminatePPAPI();
}

bool FujiModule::Init()
{
	return glInitializePPAPI(get_browser_interface()) == GL_TRUE;
}

// Create and return a PiGeneratorInstance object.
pp::Instance* FujiModule::CreateInstance(PP_Instance instance)
{
	// fuji doesn't support multiple instances
	assert(!pInstance);

	pInstance = new Fuji(instance);
	return pInstance;
}

// Fuji module factory
namespace pp
{
	Module* CreateModule()
	{
		return new FujiModule();
	}
}

#endif // MF_SYSTEM
