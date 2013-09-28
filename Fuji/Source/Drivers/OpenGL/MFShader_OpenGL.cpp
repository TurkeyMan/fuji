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

bool MFShader_CreatePlatformSpecific(MFShader *pShader, MFShaderMacro *pMacros, const char *pFilename, const char *pSource)
{
	char *pCode = NULL;
	size_t size = 0;

	if(pSource)
	{
		size = MFString_Length(pSource);
		pCode = (char*)MFCopyMemory(MFHeap_Alloc(size), pSource, size + 1);
	}
	else if(pFilename)
	{
		pCode = MFFileSystem_Load(pFilename, &size, 1);
	}

	if(pCode)
	{
		// TODO: OpenGL only compiles shaders from source...
		pShader->bytes = size;
		pShader->pProgram = pCode;

		MFDebug_Assert(pShader->shaderType < MFST_DomainShader, "Shader type not supported in OpenGL");

		static const MFOpenGL_ShaderType type[] =
		{
			MFOGL_ShaderType_VertexShader,
			MFOGL_ShaderType_FragmentShader,
			MFOGL_ShaderType_GeometryShader
		};

		GLuint shader = MFRenderer_OpenGL_CompileShader(pCode, type[pShader->shaderType]);
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
