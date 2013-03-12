#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFMat_Standard_RegisterMaterial MFMat_Standard_RegisterMaterial_OpenGL
	#define MFMat_Standard_UnregisterMaterial MFMat_Standard_UnregisterMaterial_OpenGL
	#define MFMat_Standard_Begin MFMat_Standard_Begin_OpenGL
	#define MFMat_Standard_CreateInstancePlatformSpecific MFMat_Standard_CreateInstancePlatformSpecific_OpenGL
	#define MFMat_Standard_DestroyInstancePlatformSpecific MFMat_Standard_DestroyInstancePlatformSpecific_OpenGL
#endif

#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFView_Internal.h"
#include "Materials/MFMat_Standard_Internal.h"

#include "../MFOpenGL.h"

static MFMaterial *pSetMaterial = 0;

static const GLint glTexFilters[] =
{
	// mip none
	GL_NEAREST,					// MFMatStandard_TexFilter_None
	GL_NEAREST,					// MFMatStandard_TexFilter_Point
	GL_LINEAR,					// MFMatStandard_TexFilter_Linear
	GL_LINEAR,					// MFMatStandard_TexFilter_Anisotropic

	// mip nearest
	GL_NEAREST_MIPMAP_NEAREST,	// MFMatStandard_TexFilter_None
	GL_NEAREST_MIPMAP_NEAREST,	// MFMatStandard_TexFilter_Point
	GL_LINEAR_MIPMAP_NEAREST,	// MFMatStandard_TexFilter_Linear
	GL_LINEAR_MIPMAP_NEAREST,	// MFMatStandard_TexFilter_Anisotropic

	// mip linear
	GL_NEAREST_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_None
	GL_NEAREST_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Point
	GL_LINEAR_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Linear
	GL_LINEAR_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Anisotropic

	// mip anisotropic
	GL_NEAREST_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_None
	GL_NEAREST_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Point
	GL_LINEAR_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Linear
	GL_LINEAR_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Anisotropic
};

static const GLint glTexAddressing[MFMatStandard_TexAddress_Max] =
{
	GL_REPEAT,					// MFMatStandard_TexAddress_Wrap
	GL_MIRRORED_REPEAT,			// MFMatStandard_TexAddress_Mirror
	GL_CLAMP_TO_EDGE,			// MFMatStandard_TexAddress_Clamp
	GL_CLAMP_TO_BORDER,			// MFMatStandard_TexAddress_Border
	GL_MIRROR_CLAMP_TO_EDGE_EXT // MFMatStandard_TexAddress_MirrorOnce
};

#if defined(MF_OPENGL_SUPPORT_SHADERS)
	static GLuint gDefVertexShader = 0;
	static GLuint gDefFragmentShaderUntextured = 0;
	static GLuint gDefFragmentShaderTextured = 0;
	static GLuint gDefFragmentShaderMultiTextured = 0;

	static GLuint gDefShaderProgramUntextured = 0;
	static GLuint gDefShaderProgramTextured = 0;
	static GLuint gDefShaderProgramMultiTextured = 0;

	static const GLchar gVertexShader[] = "					\n\
		uniform mat4 wvMatrix;								\n\
		uniform mat4 wvpMatrix;								\n\
		uniform mat4 texMatrix;								\n\
															\n\
		attribute vec3 vPos;								\n\
		attribute vec3 vNormal;								\n\
		attribute vec4 vColour;								\n\
		attribute vec2 vUV0;								\n\
		attribute vec2 vUV1;								\n\
															\n\
		varying vec4 oColour;								\n\
		varying vec2 oUV0;									\n\
		varying vec2 oUV1;									\n\
															\n\
		void main()											\n\
		{													\n\
			oColour = vColour;								\n\
			oUV0 = vUV0;									\n\
			oUV1 = vUV1;									\n\
			gl_Position = wvpMatrix * vec4(vPos, 1.0);		\n\
		}													";

	static const GLchar gFragmentShaderUntextured[] = "			\n\
		varying vec4 oColour;									\n\
		void main(void)											\n\
		{														\n\
			gl_FragColor = oColour;								\n\
		}														";

	static const GLchar gFragmentShaderTextured[] = "			\n\
		uniform sampler2D diffuse;								\n\
		varying vec4 oColour;									\n\
		varying vec2 oUV0;										\n\
		void main(void)											\n\
		{														\n\
			gl_FragColor = texture2D(diffuse, oUV0) * oColour;	\n\
		}														";

	static const GLchar gFragmentShaderMultiTextured[] = "		\n\
		uniform sampler2D diffuse;								\n\
		uniform sampler2D detail;								\n\
		varying vec4 oColour;									\n\
		varying vec2 oUV0;										\n\
		varying vec2 oUV1;										\n\
		void main(void)											\n\
		{														\n\
			vec4 image = texture2D(diffuse, oUV0);				\n\
			vec4 colour = texture2D(detail, oUV0) * oColour;	\n\
			gl_FragColor = vec4(image.xyz, 0) + colour;			\n\
		}														";
#endif

int MFMat_Standard_RegisterMaterial(void *pPlatformData)
{
#if defined(MF_OPENGL_SUPPORT_SHADERS)
	if(MFOpenGL_UseShaders())
	{
		// probably compile the shaders now i guess...
		gDefVertexShader = MFRenderer_OpenGL_CompileShader(gVertexShader, MFOGL_ShaderType_VertexShader);
		gDefFragmentShaderUntextured = MFRenderer_OpenGL_CompileShader(gFragmentShaderUntextured, MFOGL_ShaderType_FragmentShader);
		gDefFragmentShaderTextured = MFRenderer_OpenGL_CompileShader(gFragmentShaderTextured, MFOGL_ShaderType_FragmentShader);
		gDefFragmentShaderMultiTextured = MFRenderer_OpenGL_CompileShader(gFragmentShaderMultiTextured, MFOGL_ShaderType_FragmentShader);

#if defined(MF_OPENGL_ES)
		glReleaseShaderCompiler();
#endif

		// create and link a program
		gDefShaderProgramUntextured = MFRenderer_OpenGL_CreateProgram(gDefVertexShader, gDefFragmentShaderUntextured);
		gDefShaderProgramTextured = MFRenderer_OpenGL_CreateProgram(gDefVertexShader, gDefFragmentShaderTextured);
		gDefShaderProgramMultiTextured = MFRenderer_OpenGL_CreateProgram(gDefVertexShader, gDefFragmentShaderMultiTextured);
	}
#endif

	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
#if defined(MF_OPENGL_SUPPORT_SHADERS)
	if(MFOpenGL_UseShaders())
	{
		glDeleteProgram(gDefShaderProgramUntextured);
		glDeleteProgram(gDefShaderProgramTextured);
		glDeleteProgram(gDefShaderProgramMultiTextured);

		glDeleteShader(gDefVertexShader);
		glDeleteShader(gDefFragmentShaderUntextured);
		glDeleteShader(gDefFragmentShaderTextured);
		glDeleteShader(gDefFragmentShaderMultiTextured);
	}
#endif
}

inline void MFMat_Standard_SetTextureFlags(MFMat_Standard_Data::Texture &tex)
{
	GLuint texId = (GLuint)(size_t)tex.pTexture->pInternalData;

	glBindTexture(GL_TEXTURE_2D, texId);

	int minFilter = tex.pTexture->pTemplateData->mipLevels > 1 ? (tex.minFilter | (tex.mipFilter << 2)) : tex.minFilter;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glTexFilters[minFilter]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glTexFilters[tex.magFilter]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glTexAddressing[tex.addressU]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glTexAddressing[tex.addressV]);
}

int MFMat_Standard_Begin(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	if(pSetMaterial != pMaterial)
	{
#if MFMatStandard_TexFilter_Max > 4
		#error "glTexFilters only supports 4 mip filters..."
#endif

	    bool premultipliedAlpha = false;

		// set some render states
		if(pData->detailMapIndex)
		{
			// HACK: for compound multitexturing
			MFMat_Standard_Data::Texture &diffuse = pData->textures[pData->diffuseMapIndex];
			MFMat_Standard_Data::Texture &detail = pData->textures[pData->detailMapIndex];

#if defined(MF_OPENGL_SUPPORT_SHADERS)
			if(MFOpenGL_UseShaders())
			{
				MFRenderer_OpenGL_SetShaderProgram(gDefShaderProgramMultiTextured);

				glActiveTexture(GL_TEXTURE0);
				MFMat_Standard_SetTextureFlags(detail);
				MFRenderer_OpenGL_SetUniformS("detail", 0);

				glActiveTexture(GL_TEXTURE1);
				MFMat_Standard_SetTextureFlags(diffuse);
				MFRenderer_OpenGL_SetUniformS("diffuse", 1);
			}
			else
#endif
			{
#if !defined(MF_OPENGL_ES) || MF_OPENGL_ES_VER < 2
				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);

				MFMat_Standard_SetTextureFlags(detail);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

				glActiveTexture(GL_TEXTURE1);
				glEnable(GL_TEXTURE_2D);

				MFMat_Standard_SetTextureFlags(diffuse);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
#endif
			}

			MFRenderer_OpenGL_SetMatrix(MFOGL_ShaderType_Texture, pData->textureMatrix);
		}
		else if(pData->textures[pData->diffuseMapIndex].pTexture)
		{
			MFMat_Standard_Data::Texture &diffuse = pData->textures[pData->diffuseMapIndex];

			premultipliedAlpha = !!(pData->textures[pData->diffuseMapIndex].pTexture->pTemplateData->flags & TEX_PreMultipliedAlpha);

#if defined(MF_OPENGL_SUPPORT_SHADERS)
			if(MFOpenGL_UseShaders())
			{
				MFRenderer_OpenGL_SetShaderProgram(gDefShaderProgramTextured);

				glActiveTexture(GL_TEXTURE0);
				MFMat_Standard_SetTextureFlags(diffuse);
				MFRenderer_OpenGL_SetUniformS("diffuse", 0);
			}
			else
#endif
			{
#if !defined(MF_OPENGL_ES) || MF_OPENGL_ES_VER < 2
				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);

				MFMat_Standard_SetTextureFlags(diffuse);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

				glActiveTexture(GL_TEXTURE1);
				glDisable(GL_TEXTURE_2D);
#endif
			}

			MFRenderer_OpenGL_SetMatrix(MFOGL_ShaderType_Texture, pData->textureMatrix);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
		    glDisable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE1);
		    glDisable(GL_TEXTURE_2D);

#if defined(MF_OPENGL_SUPPORT_SHADERS)
			if(MFOpenGL_UseShaders())
				MFRenderer_OpenGL_SetShaderProgram(gDefShaderProgramUntextured);
#endif
		}

		switch(pData->materialType&MF_BlendMask)
		{
			case 0:
				glDisable(GL_BLEND);
				break;
			case MF_AlphaBlend:
				glEnable(GL_BLEND);
				glBlendFunc(premultipliedAlpha ? GL_ONE : GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case MF_Additive:
				glEnable(GL_BLEND);
				glBlendFunc(premultipliedAlpha ? GL_ONE : GL_SRC_ALPHA, GL_ONE);
				break;
			case MF_Subtractive:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
				break;
		}

		switch(pData->materialType&MF_CullMode)
		{
			case 0<<6:
				glDisable(GL_CULL_FACE);
				break;
			case 1<<6:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;
			case 2<<6:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;
			case 3<<6:
				// 'default' ?
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc((pData->materialType&MF_NoZRead) ? GL_ALWAYS : GL_LEQUAL);
		glDepthMask((pData->materialType&MF_NoZWrite) ? 0 : 1);
	}

	MFCheckForOpenGLError(true);

	return 0;
}

void MFMat_Standard_CreateInstancePlatformSpecific(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	pMaterial->pInstanceData = MFHeap_Alloc(sizeof(MFMat_Standard_Data));
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	MFZeroMemory(pData, sizeof(MFMat_Standard_Data));

	pData->ambient = MFVector::one;
	pData->diffuse = MFVector::one;

	pData->materialType = MF_AlphaBlend | 1<<6 /* back face culling */;
	pData->opaque = true;

	pData->textureMatrix = MFMatrix::identity;
	pData->uFrames = 1;
	pData->vFrames = 1;

	pData->alphaRef = 1.0f;
}

void MFMat_Standard_DestroyInstancePlatformSpecific(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	for(uint32 a=0; a<pData->textureCount; a++)
	{
		MFTexture_Destroy(pData->textures[a].pTexture);
	}

	MFHeap_Free(pMaterial->pInstanceData);
}

#endif
