#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFShader_InitModulePlatformSpecific MFShader_InitModulePlatformSpecific_OpenGL
	#define MFShader_DeinitModulePlatformSpecific MFShader_DeinitModulePlatformSpecific_OpenGL
	#define MFShader_CreatePlatformSpecific MFShader_CreatePlatformSpecific_OpenGL
	#define MFShader_DestroyPlatformSpecific MFShader_DestroyPlatformSpecific_OpenGL
#endif

#include "MFShader_Internal.h"
#include "MFOpenGL.h"
#include "MFFileSystem.h"

void MFShader_InitModulePlatformSpecific()
{
}

void MFShader_DeinitModulePlatformSpecific()
{
}

bool MFShader_CreatePlatformSpecific(MFShader *pShader)
{
	MFShaderTemplate *pTemplate = pShader->pTemplate;
	if(pTemplate->pProgram)
	{
		MFDebug_Assert(pTemplate->shaderType < MFST_DomainShader, "Shader type not supported in OpenGL");

		static const MFOpenGL_ShaderType type[] =
		{
			MFOGL_ShaderType_VertexShader,
			MFOGL_ShaderType_FragmentShader,
			MFOGL_ShaderType_GeometryShader
		};

		GLuint shader = MFRenderer_OpenGL_CompileShader((const char*)pTemplate->pProgram, type[pTemplate->shaderType]);
		pShader->pPlatformData = (void*)(size_t)shader;
	}

	return true;
}

void MFShader_DestroyPlatformSpecific(MFShader *pShader)
{
	GLuint shader = (GLuint)(size_t)pShader->pPlatformData;
	glDeleteShader(shader);
}

#endif // MF_RENDERER
