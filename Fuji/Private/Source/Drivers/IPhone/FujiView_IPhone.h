#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#include <OpenGLES/ES1/gl.h>
/**
 * OpenGL ES View
 * This view has capacity to display OpenGL ES content.
 * But the OpenGL ES system is not initialized inside,
 * it depends on extern c function void init_egl(NativeWindow) to do it.
 * It doesn't call rendering too, the user has to hold the context and surface by himself.
 *
 * Apple finally hide all EGL details and provide a wrapper class in objc
 * So that we have to do all initialization inside now.
 * We still may trigger some callbacks using gl prefixed functions only.
 */
@interface FujiView : UIView 
{
	@private
	GLint backingWidth;
	GLint backingHeight;

	EAGLContext * context;
	GLuint viewRenderbuffer, viewFramebuffer;

	GLuint depthRenderbuffer;
}

- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;

@end

extern "C"
{
	int MFRendererIPhone_MakeCurrent();
	void MFRendererIPhone_SetBackBuffer();
	int MFRendererIPhone_SwapBuffers();
}
