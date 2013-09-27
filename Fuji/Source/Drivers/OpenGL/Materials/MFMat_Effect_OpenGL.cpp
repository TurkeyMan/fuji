#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFMat_Effect_RegisterMaterial MFMat_Effect_RegisterMaterial_OpenGL
	#define MFMat_Effect_UnregisterMaterial MFMat_Effect_UnregisterMaterial_OpenGL
	#define MFMat_Effect_Begin MFMat_Effect_Begin_OpenGL
	#define MFMat_Effect_CreateInstancePlatformSpecific MFMat_Effect_CreateInstancePlatformSpecific_OpenGL
	#define MFMat_Effect_DestroyInstancePlatformSpecific MFMat_Effect_DestroyInstancePlatformSpecific_OpenGL
#endif

#include "MFRenderState_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Materials/MFMat_Effect.h"
#include "MFShader_Internal.h"

#include "../MFOpenGL.h"


int MFMat_Effect_RegisterMaterial(MFMaterialType *pType)
{
	// create default shaders
	//...

	return 0;
}

void MFMat_Effect_UnregisterMaterial()
{
	MFCALLSTACK;

}

int MFMat_Effect_Begin(MFMaterial *pMaterial, MFRendererState &state)
{
//	MFMat_Effect_Data *pData = (MFMat_Effect_Data*)pMaterial->pInstanceData;

	return 0;
}

void MFMat_Effect_CreateInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

void MFMat_Effect_DestroyInstancePlatformSpecific(MFMaterial *pMaterial)
{
}


#endif // MF_RENDERER
