#include "Fuji.h"

#if MF_DISPLAY == MF_DRIVER_NACL

#include "MFDisplay_Internal.h"
#include "MFRenderer_Internal.h"
#include "DebugMenu_Internal.h"
#include "MFSystem.h"

#include "MFSystem_NaCl.h"

void MFDisplay_ResetDisplay();

extern MFSystemCallbackFunction pSystemCallbacks[MFCB_Max];

bool isortho = false;
float fieldOfView;

extern MFInitParams gInitParams;

bool initialised = false;

OpenGLContext *pNaClGLContext = NULL;


OpenGLContext::OpenGLContext(pp::Instance* instance)
: pp::Graphics3DClient(instance)
, pInstance((Fuji*)instance)
, bFlushPending(false)
{
	pp::Module* module = pp::Module::Get();
	assert(module);
	pGLES2Interface = (const PPB_OpenGLES2*)module->GetBrowserInterface(PPB_OPENGLES2_INTERFACE);
	assert(pGLES2Interface);
}

// Release all the in-browser resources used by this context, and make this context invalid.
OpenGLContext::~OpenGLContext()
{
	glSetCurrentContextPPAPI(0);
}

// The Graphics3DClient interfcace.
void OpenGLContext::Graphics3DContextLost()
{
	assert(!"Unexpectedly lost graphics context");
}

bool OpenGLContext::MakeContextCurrent()
{
	// Lazily create the Pepper context.
	if(context_.is_null())
	{
		int32_t attribs[] =
		{
			PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
			PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
			PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
			PP_GRAPHICS3DATTRIB_SAMPLES, 0,
			PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, 0,
			PP_GRAPHICS3DATTRIB_WIDTH, size_.width(),
			PP_GRAPHICS3DATTRIB_HEIGHT, size_.height(),
			PP_GRAPHICS3DATTRIB_NONE
		};
		context_ = pp::Graphics3D(pInstance, pp::Graphics3D(), attribs);

		if(context_.is_null())
		{
			glSetCurrentContextPPAPI(0);
			return false;
		}

		pInstance->BindGraphics(context_);
	}

	glSetCurrentContextPPAPI(context_.pp_resource());
	return true;
}

/// Flush the contents of this context to the browser's 3D device.
void OpenGLContext::FlushContext()
{
	if(flush_pending())
	{
		// A flush is pending so do nothing; just drop this flush on the floor.
		return;
	}
	set_flush_pending(true);
	context_.SwapBuffers(pp::CompletionCallback(&FlushCallback, this));
}

/// Make the underlying 3D device invalid, so that any subsequent rendering
/// commands will have no effect.  The next call to MakeContextCurrent() will
/// cause the underlying 3D device to get rebound and start receiving
/// receiving rendering commands again.  Use InvalidateContext(), for
/// example, when resizing the context's viewing area.
void OpenGLContext::InvalidateContext()
{
	glSetCurrentContextPPAPI(0);
}

/// Resize the context.
void OpenGLContext::ResizeContext(const pp::Size& size)
{
MFDebug_Message("ogl: resize");
	size_ = size;
	if(!context_.is_null())
	{
		context_.ResizeBuffers(size.width(), size.height());
MFDebug_Message("ogl: resized");
	}
}

void OpenGLContext::FlushCallback(void* data, int32_t result)
{
	static_cast<OpenGLContext*>(data)->set_flush_pending(false);
}


void MFDisplay_DestroyWindow()
{
	MFCALLSTACK;

	MFRenderer_DestroyDisplay();
}

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	MFCALLSTACK;

	FujiModule *pModule = (FujiModule*)pp::Module::Get();
	pNaClGLContext = new OpenGLContext(pModule->GetInstance());

	pNaClGLContext->MakeContextCurrent();

	gDisplay.fullscreenWidth = gDisplay.width = width;
	gDisplay.fullscreenHeight = gDisplay.height = height;
	gDisplay.refreshRate = rate;
	gDisplay.colourDepth = 32;
	gDisplay.windowed = true;
	gDisplay.wide = false;
	gDisplay.progressive = true;

	MFRenderer_CreateDisplay();

	initialised = true;

	return 0;
}

void MFDisplay_ResetDisplay()
{
	MFCALLSTACK;

	MFRenderer_ResetDisplay();
}

void MFDisplay_DestroyDisplay()
{
	MFCALLSTACK;

	MFRenderer_DestroyDisplay();

	delete pNaClGLContext;
	pNaClGLContext = NULL;
}

bool MFDisplay_SetDisplayMode(int width, int height, bool bFullscreen)
{

	return MFRenderer_SetDisplayMode(width, height, bFullscreen);
}

void MFDisplay_GetNativeRes(MFRect *pRect)
{
	pRect->x = pRect->y = 0.f;
	pRect->width = 800.f;
	pRect->height = 480.f;
}

void MFDisplay_GetDefaultRes(MFRect *pRect)
{
	return MFDisplay_GetNativeRes(pRect);
}

float MFDisplay_GetNativeAspectRatio()
{
	MFRect rect;
	MFDisplay_GetNativeRes(&rect);

	return rect.width / rect.height;
}

bool MFDisplay_IsWidescreen()
{
	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);

	return rect.width / rect.height >= 1.6f;
}

#endif // MF_DISPLAY
