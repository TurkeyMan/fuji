/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "Display.h"
#include "Texture.h"
#include "Material.h"
#include "FileSystem.h"
#include "IniFile.h"

#include "SysLogo-256.h"
#include "SysLogo-64.h"

/**** internal functions ****/

void		MaterialInternal_Update(Material *pMaterial);
const char*	MaterialInternal_GetIDString(Material *pMaterial);

Material*	MaterialInternal_CreateDefault(const char *pName);
void		MaterialInternal_InitialiseFromDefinition(Material *pMat, const char *pDefinition);

/**** Globals ****/

IniFile gMaterialDefinitions;

PtrListDL<Material> materialList;

Material *pCurrentMaterial = NULL;

Material *pNoneMaterial = NULL;
Material *pWhiteMaterial = NULL;
Material *pSysLogoLarge = NULL;
Material *pSysLogoSmall = NULL;

char matDesc[32][4] = {"M","Na","Ds","Ad","T","","A","A3","L","Ls","Le","Dm","E","Es","","","P","C","B","N","D","Ec","E","Es","D2","Lm","D","U","","","",""};

/**** Functions ****/

void Material_InitModule()
{
	CALLSTACK;

	materialList.Init("Material List", gDefaults.material.maxMaterials);

	if(gMaterialDefinitions.Create(File_SystemPath("Materials.ini")))
	{
		LOGD("Failed to load Materials.ini");
	}

	// create the logo textures from raw data
	Texture *pSysLogoLargeTexture = Texture_CreateFromRawData("SysLogoLarge", SysLogo_256_data, SysLogo_256_width, SysLogo_256_height, TEXF_A8R8G8B8, TEX_VerticalMirror);
	Texture *pSysLogoSmallTexture = Texture_CreateFromRawData("SysLogoSmall", SysLogo_64_data, SysLogo_64_width, SysLogo_64_height, TEXF_A8R8G8B8, TEX_VerticalMirror);

	// create standard materials
	pNoneMaterial = Material_Create("_None");
	pWhiteMaterial = Material_Create("_White");
	pSysLogoLarge = Material_Create("SysLogoLarge");
	pSysLogoSmall = Material_Create("SysLogoSmall");

	// release a reference to the logo textures
	Texture_Destroy(pSysLogoLargeTexture);
	Texture_Destroy(pSysLogoSmallTexture);
}

void Material_DeinitModule()
{
	CALLSTACK;

	Material_Destroy(pNoneMaterial);
	Material_Destroy(pWhiteMaterial);
	Material_Destroy(pSysLogoLarge);
	Material_Destroy(pSysLogoSmall);

	gMaterialDefinitions.Release();

	materialList.Deinit();
}

void Material_Update()
{
	Material **ppMatIterator = materialList.Begin();

	while(*ppMatIterator)
	{
		MaterialInternal_Update(*ppMatIterator);
		ppMatIterator++;
	}
}

// interface functions

Material* Material_Create(const char *pName)
{
	CALLSTACK;

	Material *pMat = Material_Find(pName);

	if(!pMat)
	{
		pMat = MaterialInternal_CreateDefault(pName);

		if(gMaterialDefinitions.FindSection(pName))
		{
			MaterialInternal_InitialiseFromDefinition(pMat, pName);
		}
		else
		{
			Texture *pTexture = Texture_Create(pName);

			pMat->pTextures[0] = pTexture;
			pMat->diffuseMapIndex = 0;
			pMat->textureCount = 1;
		}
	}

	pMat->refCount++;

	return pMat;
}

int Material_Destroy(Material *pMaterial)
{
	CALLSTACK;

	pMaterial->refCount--;

	if(!pMaterial->refCount)
	{
		for(uint16 a=0; a<pMaterial->textureCount; a++)
		{
			Texture_Destroy(pMaterial->pTextures[a]);
		}

		materialList.Destroy(pMaterial);

		return 0;
	}

	return pMaterial->refCount;
}

Material* Material_Find(const char *pName)
{
	CALLSTACK;

	Material **ppIterator = materialList.Begin();

	while(*ppIterator)
	{
		if(!StrCaseCmp(pName, (*ppIterator)->name)) return *ppIterator;

		ppIterator++;
	}

	return NULL;
}

Material* Material_GetCurrent()
{
	return pCurrentMaterial;
}

void Material_Use(Material *pMaterial)
{
	pCurrentMaterial = pMaterial;
}

void Material_UseWhite()
{
	Material_Use(pWhiteMaterial);
}


// internal functions
const char* MaterialInternal_GetIDString()
{
	CALLSTACK;
/*
	char *id = &stringBuffer[stringBufferOffset];
	*id = NULL;

	for(int a=0; a<32; a++)
	{
		if((flags>>a)&1) strcat(id, matDesc[a]);
	}

	stringBufferOffset += strlen(id) + 1;
*/
	return NULL;
}

Material* MaterialInternal_CreateDefault(const char *pName)
{
	CALLSTACK;

	Material *pMat = materialList.Create();
	memset(pMat, 0, sizeof(Material));

	strcpy(pMat->name, pName);

	pMat->ambient = Vector4::one;
	pMat->diffuse = Vector4::one;
/*
	pMat->illum = Vector4::zero;
	pMat->specular = Vector4::zero;
	pMat->specularPow = 0;
*/
	pMat->materialType = MF_AlphaBlend;
	pMat->opaque = true;

	pMat->textureMatrix = Matrix::identity;
	pMat->uFrames = 1;
	pMat->vFrames = 1;
/*
	pMat->curTime = 0.0f;
	pMat->frameTime = 0.0f;
	pMat->curFrame = 0;

	pMat->refCount = 0;
*/
	return pMat;
}

void MaterialInternal_InitialiseFromDefinition(Material *pMat, const char *pDefinition)
{
	CALLSTACK;

	if(gMaterialDefinitions.FindSection(pDefinition))
	{
		gMaterialDefinitions.GetNextLine();

		while(!gMaterialDefinitions.EndOfFile() && !gMaterialDefinitions.IsSection())
		{
			char *pName = gMaterialDefinitions.GetName();
			if(!StrCaseCmp(pName, "alias"))
			{
				gMaterialDefinitions.PushMarker();

				char *pAlias = gMaterialDefinitions.AsString();

				if(gMaterialDefinitions.FindSection(pAlias))
				{
					MaterialInternal_InitialiseFromDefinition(pMat, pAlias);
				}

				gMaterialDefinitions.PopMarker();
			}
			else if(!StrCaseCmp(pName, "lit"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Lit) | (gMaterialDefinitions.AsBool(0) ? MF_Lit : NULL);
			}
			else if(!StrCaseCmp(pName, "prelit"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Lit) | (!gMaterialDefinitions.AsBool(0) ? MF_Lit : NULL);
			}
			else if(!StrCaseCmp(pName, "diffusecolour"))
			{
				pMat->diffuse = gMaterialDefinitions.AsVector4();
			}
			else if(!StrCaseCmp(pName, "ambientcolour"))
			{
				pMat->ambient = gMaterialDefinitions.AsVector4();
			}
			else if(!StrCaseCmp(pName, "specularcolour"))
			{
				pMat->specular = gMaterialDefinitions.AsVector4();
			}
			else if(!StrCaseCmp(pName, "specularpower"))
			{
				pMat->specularPow = gMaterialDefinitions.AsFloat(0);
			}
			else if(!StrCaseCmp(pName, "emissivecolour"))
			{
				pMat->illum = gMaterialDefinitions.AsVector4();
			}
			else if(!StrCaseCmp(pName, "mask"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Mask) | (gMaterialDefinitions.AsBool(0) ? MF_Mask : NULL);
			}
			else if(!StrCaseCmp(pName, "doublesided"))
			{
				pMat->materialType = (pMat->materialType & ~MF_DoubleSided) | (gMaterialDefinitions.AsBool(0) ? MF_DoubleSided : NULL);
			}
			else if(!StrCaseCmp(pName, "backfacecull"))
			{
				pMat->materialType = (pMat->materialType & ~MF_DoubleSided) | (!gMaterialDefinitions.AsBool(0) ? MF_DoubleSided : NULL);
			}
			else if(!StrCaseCmp(pName, "additive"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (gMaterialDefinitions.AsBool(0) ? MF_Additive : NULL);
			}
			else if(!StrCaseCmp(pName, "subtractive"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (gMaterialDefinitions.AsBool(0) ? MF_Subtractive : NULL);
			}
			else if(!StrCaseCmp(pName, "alpha"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (gMaterialDefinitions.AsBool(0) ? MF_AlphaBlend : NULL);
			}
			else if(!StrCaseCmp(pName, "blend"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (gMaterialDefinitions.AsInt(0) << 1);
			}
			else if(!StrCaseCmp(pName, "texture"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(gMaterialDefinitions.AsString());
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "diffusemap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(gMaterialDefinitions.AsString());
				pMat->diffuseMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "diffusemap2"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(gMaterialDefinitions.AsString());
				pMat->materialType |= MF_DiffuseMap2;
				pMat->diffuseMap2Index = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "normalmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(gMaterialDefinitions.AsString());
				pMat->materialType |= MF_NormalMap;
				pMat->normalMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "detailmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(gMaterialDefinitions.AsString());
				pMat->materialType |= MF_DetailTexture;
				pMat->detailMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "envmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(gMaterialDefinitions.AsString());
				pMat->materialType |= MF_SphereEnvMap;
				pMat->envMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "lightmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(gMaterialDefinitions.AsString());
				pMat->materialType |= MF_LightMap;
				pMat->lightMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "bumpmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(gMaterialDefinitions.AsString());
				pMat->materialType |= MF_BumpMap;
				pMat->bumpMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "reflectionmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(gMaterialDefinitions.AsString());
				pMat->materialType |= MF_BumpMap;
				pMat->bumpMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "specularmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(gMaterialDefinitions.AsString());
				pMat->materialType |= MF_BumpMap;
				pMat->bumpMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "celshading"))
			{
				pMat->materialType |= MF_CelShading;
	//				pMat-> = gMaterialDefinitions.AsInt(0);
			}
			else if(!StrCaseCmp(pName, "phong"))
			{
				pMat->materialType |= MF_LitPerPixel;
	//				pMat-> = gMaterialDefinitions.AsInt(0);
			}
			else if(!StrCaseCmp(pName, "animated"))
			{
				pMat->materialType |= MF_Animating;
				pMat->uFrames = gMaterialDefinitions.AsInt(0);
				pMat->vFrames = gMaterialDefinitions.AsInt(1);
				pMat->frameTime = gMaterialDefinitions.AsFloat(2);

				pMat->textureMatrix.SetIdentity();
				pMat->textureMatrix.Scale(Vector(1.0f/(float)pMat->uFrames, 1.0f/(float)pMat->vFrames, 1.0f));
			}

			gMaterialDefinitions.GetNextLine();
		}
	}
}

void MaterialInternal_Update(Material *pMaterial)
{
	if(pMaterial->materialType & MF_Animating)
	{
		pMaterial->curTime += TIMEDELTA;

		while(pMaterial->curTime >= pMaterial->frameTime)
		{
			pMaterial->curTime -= pMaterial->frameTime;

			pMaterial->curFrame++;
			pMaterial->curFrame = pMaterial->curFrame % (pMaterial->uFrames*pMaterial->vFrames);

			pMaterial->textureMatrix.SetZAxis(Vector((1.0f/(float)pMaterial->uFrames) * (float)(pMaterial->curFrame%pMaterial->uFrames), (1.0f/(float)pMaterial->vFrames) * (float)(pMaterial->curFrame/pMaterial->vFrames), 0.0f));
		}
	}
}


