#pragma once
#if !defined(_MFSYSTEM_NACL_H)
#define _MFSYSTEM_NACL_H

//#include "ppapi/c/pp_file_info.h"
//#include "ppapi/c/ppb_file_io.h"
//#include "ppapi/cpp/rect.h"
//#include "ppapi/cpp/graphics_2d.h"
//#include "ppapi/cpp/image_data.h"

#include <ppapi/c/ppp.h>

#include <ppapi/cpp/module.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/input_event.h>
#include <ppapi/cpp/var.h>
#include <ppapi/cpp/size.h>

#include <GLES2/gl2.h>
#include <ppapi/gles2/gl2ext_ppapi.h>
#include <ppapi/cpp/graphics_3d_client.h>
#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/c/ppb_opengles2.h>

//#include "ppapi/cpp/file_system.h"
//#include "ppapi/cpp/file_ref.h"
//#include "ppapi/cpp/file_io.h"
#include <ppapi/c/ppb_file_io.h>
#include <ppapi/c/ppb_file_ref.h>
#include <ppapi/c/ppb_file_system.h>

// The Instance class.  One of these exists for each instance of your NaCl
// module on the web page.  The browser will ask the Module object to create
// a new Instance for each occurrence of the <embed> tag that has these
// attributes:
//     type="application/x-nacl"
//     nacl="fuji.nmf"
class Fuji : public pp::Instance
{
public:
	explicit Fuji(PP_Instance instance);
	virtual ~Fuji();

	virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);

	// Update the graphics context to the new size, and regenerate |pixel_buffer_|
	// to fit the new size as well.
	virtual void DidChangeView(const pp::Rect& position, const pp::Rect& clip);

	virtual bool HandleInputEvent(const pp::InputEvent& event);

	// Called by the browser to handle the postMessage() call in Javascript.
	// The message in this case is expected to contain the string 'update', or
	// 'resetScore' in order to invoke either the Update or ResetScore function
	// respectively.
	virtual void HandleMessage(const pp::Var& var_message);

	void Update();
};

class UpdateScheduler
{
public:
	UpdateScheduler(int32_t delay, Fuji* fuji);
	~UpdateScheduler();

private:
	static void UpdateCallback(void *pData, int32_t result);

	int32_t delay_;  // milliseconds
	Fuji* fuji_;  // weak
};

// The Module class.  The browser calls the CreateInstance() method to create
// an instance of your NaCl module on the web page.  The browser creates a new
// instance for each <embed> tag with type="application/x-nacl".
class FujiModule : public pp::Module
{
public:
	FujiModule();
	virtual ~FujiModule();

	/// Called by the browser when the module is first loaded and ready to run.
	/// This is called once per module, not once per instance of the module on
	/// the page.
	virtual bool Init();

	// Create and return a PiGeneratorInstance object.
	virtual pp::Instance* CreateInstance(PP_Instance instance);

	static inline Fuji *GetInstance() { return pInstance; }

private:
	static Fuji *pInstance;
};

// OpenGLContext manages an OpenGL rendering context in the browser.
class OpenGLContext : public pp::Graphics3DClient
{
public:
	explicit OpenGLContext(pp::Instance* instance);

	// Release all the in-browser resources used by this context, and make this context invalid.
	virtual ~OpenGLContext();

	// The Graphics3DClient interfcace.
	virtual void Graphics3DContextLost();

	bool MakeContextCurrent();

	/// Flush the contents of this context to the browser's 3D device.
	void FlushContext();

	/// Make the underlying 3D device invalid, so that any subsequent rendering
	/// commands will have no effect.  The next call to MakeContextCurrent() will
	/// cause the underlying 3D device to get rebound and start receiving
	/// receiving rendering commands again.  Use InvalidateContext(), for
	/// example, when resizing the context's viewing area.
	void InvalidateContext();

	/// Resize the context.
	void ResizeContext(const pp::Size& size);

	/// The OpenGL ES 2.0 interface.
	inline const struct PPB_OpenGLES2* gles2() const { return pGLES2Interface; }

	/// The PP_Resource needed to make GLES2 calls through the Pepper interface.
	inline const PP_Resource gl_context() const { return context_.pp_resource(); }

	/// Indicate whether a flush is pending.  This can only be called from the
	/// main thread; it is not thread safe.
	inline bool flush_pending() const { return bFlushPending; }
	inline void set_flush_pending(bool bFlush)	{ bFlushPending = bFlush; }

private:
	static void FlushCallback(void* data, int32_t result);

	Fuji *pInstance;

	pp::Size size_;
	pp::Graphics3D context_;
	bool bFlushPending;

	const struct PPB_OpenGLES2* pGLES2Interface;
};

#endif
