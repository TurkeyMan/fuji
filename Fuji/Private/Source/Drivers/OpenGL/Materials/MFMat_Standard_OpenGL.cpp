#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFMat_Standard_RegisterMaterial MFMat_Standard_RegisterMaterial_OpenGL
	#define MFMat_Standard_UnregisterMaterial MFMat_Standard_UnregisterMaterial_OpenGL
	#define MFMat_Standard_Begin MFMat_Standard_Begin_OpenGL
	#define MFMat_Standard_CreateInstance MFMat_Standard_CreateInstance_OpenGL
	#define MFMat_Standard_DestroyInstance MFMat_Standard_DestroyInstance_OpenGL
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
	GL_REPEAT,	// MFMatStandard_TexAddress_Wrap
	GL_CLAMP,	// MFMatStandard_TexAddress_Mirror
	GL_CLAMP,	// MFMatStandard_TexAddress_Clamp
	GL_CLAMP,	// MFMatStandard_TexAddress_Border
	GL_CLAMP	// MFMatStandard_TexAddress_MirrorOnce
};


int MFMat_Standard_RegisterMaterial(void *pPlatformData)
{
	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
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

			glActiveTexture(GL_TEXTURE0);
		    glEnable(GL_TEXTURE_2D);
			MFMat_Standard_SetTextureFlags(detail);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glActiveTexture(GL_TEXTURE1);
		    glEnable(GL_TEXTURE_2D);
			MFMat_Standard_SetTextureFlags(diffuse);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

			glMatrixMode(GL_TEXTURE);
			glLoadMatrixf((GLfloat *)&pData->textureMatrix);
		}
		else if(pData->textures[pData->diffuseMapIndex].pTexture)
		{
			MFMat_Standard_Data::Texture &diffuse = pData->textures[pData->diffuseMapIndex];

			glActiveTexture(GL_TEXTURE0);
		    glEnable(GL_TEXTURE_2D);
			MFMat_Standard_SetTextureFlags(diffuse);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			premultipliedAlpha = !!(pData->textures[pData->diffuseMapIndex].pTexture->pTemplateData->flags & TEX_PreMultipliedAlpha);

			glMatrixMode(GL_TEXTURE);
			glLoadMatrixf((GLfloat *)&pData->textureMatrix);

			glActiveTexture(GL_TEXTURE1);
		    glDisable(GL_TEXTURE_2D);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
		    glDisable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE1);
		    glDisable(GL_TEXTURE_2D);
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

	return 0;
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
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

void MFMat_Standard_DestroyInstance(MFMaterial *pMaterial)
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
