#include "Fuji_Internal.h"

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
#if defined(MF_OPENGL_ES)
	glReleaseShaderCompiler();
#endif
}

bool MFShader_CreatePlatformSpecific(MFShader *pShader)
{
	MFShaderTemplate *pTemplate = pShader->pTemplate;
	if(pTemplate->pProgram)
	{
		MFDebug_Assert(pTemplate->shaderType < MFST_DomainShader, "Shader type not supported in OpenGL");

		static const GLenum shaterTypes[] =
		{
			GL_VERTEX_SHADER,	// MFST_VertexShader
			GL_FRAGMENT_SHADER,	// MFST_FragmentShader
			GL_GEOMETRY_SHADER	// MFST_GeometryShader
		};

		GLuint shader = glCreateShader(shaterTypes[pTemplate->shaderType]);

		glShaderSource(shader, 1, (const char**)&pTemplate->pProgram, NULL);
		glCompileShader(shader);

		// make sure the compilation was successful
		GLint result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if(result == GL_FALSE)
		{
//			GLint maxLength = 0;
//			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
 
			// get the shader info log
			GLchar log[1024];
			glGetShaderInfoLog(shader, sizeof(log), &result, log);

			// print an error message and the info log
			MFDebug_Warn(1, MFStr("MFRenderer_OpenGL_CompileShader(): Unable to compile shader:\n%s\n", log));

			glDeleteShader(shader);
			shader = 0;
		}

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
