#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFRenderTarget_InitModulePlatformSpecific MFRenderTarget_InitModulePlatformSpecific_OpenGL
	#define MFRenderTarget_DeinitModulePlatformSpecific MFRenderTarget_DeinitModulePlatformSpecific_OpenGL
	#define MFRenderTarget_CreatePlatformSpecific MFRenderTarget_CreatePlatformSpecific_OpenGL
	#define MFRenderTarget_DestroyPlatformSpecific MFRenderTarget_DestroyPlatformSpecific_OpenGL
#endif


/**** Defines ****/

/**** Includes ****/

#include "MFRenderTarget_Internal.h"
#include "MFTexture_Internal.h"

#include "MFOpenGL.h"

/**** Globals ****/

/**** Functions ****/

bool MFRenderTarget_InitModulePlatformSpecific()
{
	return true;
}

void MFRenderTarget_DeinitModulePlatformSpecific()
{
}

bool MFRenderTarget_CreatePlatformSpecific(MFRenderTarget *pRenderTarget)
{
	if(pRenderTarget->availableColourTargets == 1 && !pRenderTarget->pColourTargets[0]->pInternalData && pRenderTarget->pDepthStencil && !pRenderTarget->pDepthStencil->pInternalData)
	{
		// checial case: create the default framebuffer
		pRenderTarget->pPlatformData = NULL;
		return true;
	}

	GLuint frameBufferID;
	glGenFramebuffers(1, &frameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

	for(int a=0; a<8; ++a)
	{
		if(pRenderTarget->pColourTargets[a])
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + a, GL_TEXTURE_2D, (GLuint)(uintp)pRenderTarget->pColourTargets[a]->pInternalData, 0);
	}
	if(pRenderTarget->pDepthStencil)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, (GLuint)(uintp)pRenderTarget->pDepthStencil->pInternalData, 0);

	MFDebug_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Incomplete frame buffer!");

	pRenderTarget->pPlatformData = (void*)(size_t)frameBufferID;

	return true;
}

void MFRenderTarget_DestroyPlatformSpecific(MFRenderTarget *pRenderTarget)
{
	GLuint fb = (GLuint)(size_t)pRenderTarget->pPlatformData;
	if(fb)
		glDeleteFramebuffers(1, &fb);
}

#endif // MF_RENDERER
