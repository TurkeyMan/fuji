#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFEffect_InitModulePlatformSpecific MFEffect_InitModulePlatformSpecific_OpenGL
	#define MFEffect_DeinitModulePlatformSpecific MFEffect_DeinitModulePlatformSpecific_OpenGL
	#define MFEffect_PlatformDataSize MFEffect_PlatformDataSize_OpenGL
	#define MFEffect_CreatePlatformSpecific MFEffect_CreatePlatformSpecific_OpenGL
	#define MFEffect_DestroyPlatformSpecific MFEffect_DestroyPlatformSpecific_OpenGL
#endif

#include "MFEffect_Internal.h"
#include "MFOpenGL.h"

void MFEffect_InitModulePlatformSpecific()
{
}

void MFEffect_DeinitModulePlatformSpecific()
{
}

size_t MFEffect_PlatformDataSize()
{
	return 0;
}

bool MFEffect_CreatePlatformSpecific(MFEffect *pEffect)
{
	MFEffectData_OpenGL *pPlatformData = (MFEffectData_OpenGL*)MFHeap_Alloc(sizeof(MFEffectData_OpenGL)*pEffect->numTechniques);
	pEffect->pPlatformData = pPlatformData;

	// we need to link the shaders for each technique
	for(int i=0; i<pEffect->numTechniques; ++i)
	{
		MFEffectTechnique &t = pEffect->pTechniques[i];
		MFEffectData_OpenGL &techniqueData = pPlatformData[i];
		t.pPlatformData = &techniqueData;

		// create and link a program
		GLuint program = glCreateProgram();

		if(t.shaders[MFST_VertexShader].pShader)
			glAttachShader(program, (GLuint)(size_t)t.shaders[MFST_VertexShader].pShader->pPlatformData);
		if(t.shaders[MFST_FragmentShader].pShader)
			glAttachShader(program, (GLuint)(size_t)t.shaders[MFST_FragmentShader].pShader->pPlatformData);
		if(t.shaders[MFST_GeometryShader].pShader)
			glAttachShader(program, (GLuint)(size_t)t.shaders[MFST_GeometryShader].pShader->pPlatformData);

		glLinkProgram(program);

		// make sure we lank properly
		GLint result;
		glGetProgramiv(program, GL_LINK_STATUS, &result);
		if(result == GL_FALSE)
		{
			// get the program info log
//			glGetProgramiv(g_program, GL_INFO_LOG_LENGTH, &length);
			char log[1024];
			glGetProgramInfoLog(program, sizeof(log), &result, log);

			// print an error message and the info log
			MFDebug_Assert(1, MFStr("MFRenderer_OpenGL_CreateProgram(): Unable to link program for MFShaderTechnique '%s': %s\n", t.pName, log));

			glDeleteProgram(program);
			program = 0;
		}
		else
		{
			// we need to collect the active uniforms post-linking
			GLint numUniforms;
			glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
			for(GLint j=0; j<numUniforms; ++j)
			{
 				GLchar name[256];
				GLsizei nameLen;
				GLenum type;
				GLint size;

				glGetActiveUniform(program, j, sizeof(name), &nameLen, &size, &type, name);
				char *pLBracket = MFString_Chr(name, '[');
				if(pLBracket)
					*pLBracket = 0;

				for(int a=0; a<MFSB_CT_TypeCount; ++a)
				{
					int rsCount = MFStateBlock_GetNumRenderStates((MFStateBlockConstantType)a);
					for(int b=0; b<rsCount; ++b)
					{
						const char *pName = MFStateBlock_GetRenderStateName((MFStateBlockConstantType)a, b);
						if(!MFString_Compare(name, pName))
						{
							techniqueData.uniformLocation[a][b] = (uint8)j;
							t.renderStateRequirements[a] |= 1 << b;
							b = rsCount;
							a = MFSB_CT_TypeCount;
						}
					}
				}
			}

			t.renderStateRequirements[MFSB_CT_Texture] = (t.renderStateRequirements[MFSB_CT_RenderState] >> MFSCRS_DiffuseSamplerState) & (MFBIT(MFSCT_Max)-1);
		}

		techniqueData.program = program;
	}

	return true;
}

void MFEffect_DestroyPlatformSpecific(MFEffect *pEffect)
{
	for(int i=0; i<pEffect->numTechniques; ++i)
	{
		MFEffectData_OpenGL &techniqueData = *(MFEffectData_OpenGL*)pEffect->pTechniques[i].pPlatformData;

		if(techniqueData.program)
			glDeleteProgram(techniqueData.program);
	}

	MFHeap_Free(pEffect->pPlatformData);
}

#endif // MF_RENDERER
