#import "FujiView_IPhone.h"

#include "OpenGLES/ES1/glext.h"

static id g_context;
static int g_buffer;
static int g_frame;

@implementation FujiView

+ (Class) layerClass 
{
  return [CAEAGLLayer class];
}

- (id) initWithFrame:(CGRect)frame
{
   self = [super initWithFrame:frame];
   if ( self != nil )
   {
	   CAEAGLLayer* gllayer = (CAEAGLLayer*) [self layer];
	   gllayer.opaque = YES;
	   gllayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
									   [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

	   // new EAGLContext instance
	   context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES1];
	   [EAGLContext setCurrentContext:context];
	   g_context = context;

	   [self createFramebuffer];
	   [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable: gllayer];
   }

   return self;
}

-(void) layoutSubviews
{
	[EAGLContext setCurrentContext:context];
	[self destroyFramebuffer];
	[self createFramebuffer];
    CGRect bounds = [self bounds];
//	resize(bounds.size.width, bounds.size.height);
}

#define USE_DEPTH_BUFFER YES 

- (BOOL)createFramebuffer {
	glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);

	g_buffer = viewRenderbuffer;
	g_frame = viewFramebuffer;

	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);

	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);

	if (USE_DEPTH_BUFFER)
	{
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
	}

	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}

	return YES;
}

- (void)destroyFramebuffer {
	glDeleteFramebuffersOES(1, &viewFramebuffer);
	viewFramebuffer = 0;
	glDeleteRenderbuffersOES(1, &viewRenderbuffer);
	viewRenderbuffer = 0;

	if(depthRenderbuffer) {
		glDeleteRenderbuffersOES(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
}

@end

int MFRendererIPhone_MakeCurrent()
{
   return NO == [EAGLContext setCurrentContext:g_context];
}

int MFRendererIPhone_SwapBuffers()
{
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, g_buffer);
	return NO == [g_context presentRenderbuffer:GL_RENDERBUFFER_OES];
}
