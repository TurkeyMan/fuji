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
void		MaterialInternal_InitialiseFromDefinition(IniFile *pDefIni, Material *pMat, const char *pDefinition);


/**** Globals ****/

struct MaterialDefinition
{
	const char *pName;
	IniFile *pIniFile;
	bool ownsIni;

	MaterialDefinition *pNextDefinition;
};

void Material_DestroyDefinition(MaterialDefinition *pDefinition);

PtrListDL<MaterialDefinition> gMaterialDefList;
PtrListDL<IniFile> gMaterialDefInis;
MaterialDefinition *pDefinitionRegistry = NULL;

PtrListDL<Material> gMaterialList;

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

	gMaterialDefList.Init("Material Definitions List", gDefaults.material.maxMaterialDefs);
	gMaterialDefInis.Init("Material Definitions Ini Files", gDefaults.material.maxMaterialDefs);

	gMaterialList.Init("Material List", gDefaults.material.maxMaterials);

	if(Material_AddDefinitionsFile("Materials.ini", "Materials.ini"))
	{
		LOGD("Warning: Failed to load Materials.ini");
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

	MaterialDefinition *pDef = pDefinitionRegistry;

	while(pDef)
	{
		MaterialDefinition *pNext = pDef->pNextDefinition;
		Material_DestroyDefinition(pDef);
		pDef = pNext;
	}

	gMaterialList.Deinit();

	gMaterialDefList.Deinit();
	gMaterialDefInis.Deinit();
}

void Material_Update()
{
	Material **ppMatIterator = gMaterialList.Begin();

	while(*ppMatIterator)
	{
		MaterialInternal_Update(*ppMatIterator);
		ppMatIterator++;
	}
}

// interface functions
int Material_AddDefinitionsFile(const char *pName, const char *pFilename)
{
	MaterialDefinition *pDef = gMaterialDefList.Create();

	pDef->pName = pName;
	pDef->pIniFile = gMaterialDefInis.Create();

	if(pDef->pIniFile->Create(pFilename))
	{
		gMaterialDefInis.Destroy(pDef->pIniFile);
		gMaterialDefList.Destroy(pDef);

		LOGD("Error: Couldnt create material definitions...");
		return 1;
	}

	pDef->ownsIni = true;

	pDef->pNextDefinition = pDefinitionRegistry;
	pDefinitionRegistry = pDef;

	return 0;
}

int Material_AddDefinitionsFileInMemory(const char *pName, const char *pBuffer)
{
	MaterialDefinition *pDef = gMaterialDefList.Create();

	pDef->pName = pName;
	pDef->pIniFile = gMaterialDefInis.Create();
	pDef->pIniFile->CreateFromPointer(pBuffer);
	pDef->ownsIni = true;

	pDef->pNextDefinition = pDefinitionRegistry;
	pDefinitionRegistry = pDef;

	return 0;
}

int Material_AddDefinitionsIniFile(const char *pName, IniFile *pMatDefs)
{
	MaterialDefinition *pDef = gMaterialDefList.Create();

	pDef->pName = pName;
	pDef->pIniFile = pMatDefs;
	pDef->ownsIni = false;

	pDef->pNextDefinition = pDefinitionRegistry;
	pDefinitionRegistry = pDef;

	return 0;
}

void Material_DestroyDefinition(MaterialDefinition *pDefinition)
{
	if(pDefinition->ownsIni)
	{
		pDefinition->pIniFile->Release();
		gMaterialDefInis.Destroy(pDefinition->pIniFile);
		pDefinition->pIniFile = NULL;
	}

	gMaterialDefList.Destroy(pDefinition);
}

void Material_RemoveDefinitions(const char *pName)
{
	MaterialDefinition *pDef = pDefinitionRegistry;

	if(!pDef)
		return;

	if(!strcmp(pDef->pName, pName))
	{
		pDefinitionRegistry = pDef->pNextDefinition;
		Material_DestroyDefinition(pDef);
		return;
	}

	while(pDef->pNextDefinition)
	{
		if(!strcmp(pDef->pNextDefinition->pName, pName))
		{
			MaterialDefinition *pDestroy = pDef->pNextDefinition;
			pDef->pNextDefinition = pDef->pNextDefinition->pNextDefinition;

			Material_DestroyDefinition(pDestroy);
			return;
		}

		pDef = pDef->pNextDefinition;
	}
}

Material* Material_Create(const char *pName)
{
	CALLSTACK;

	Material *pMat = Material_Find(pName);

	if(!pMat)
	{
		pMat = MaterialInternal_CreateDefault(pName);

		MaterialDefinition *pDef = pDefinitionRegistry;

		while(pDef)
		{
			if(pDef->pIniFile->FindSection(pName))
			{
				MaterialInternal_InitialiseFromDefinition(pDef->pIniFile, pMat, pName);
				break;
			}

			pDef = pDef->pNextDefinition;
		}

		if(!pDef)
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

		gMaterialList.Destroy(pMaterial);

		return 0;
	}

	return pMaterial->refCount;
}

Material* Material_Find(const char *pName)
{
	CALLSTACK;

	Material **ppIterator = gMaterialList.Begin();

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

	Material *pMat = gMaterialList.Create();
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

void MaterialInternal_InitialiseFromDefinition(IniFile *pDefIni, Material *pMat, const char *pDefinition)
{
	CALLSTACK;

	if(pDefIni->FindSection(pDefinition))
	{
		pDefIni->GetNextLine();

		while(!pDefIni->EndOfFile() && !pDefIni->IsSection())
		{
			char *pName = pDefIni->GetName();
			if(!StrCaseCmp(pName, "alias"))
			{
#pragma message("Alias's should be able to come from other material ini's")
				pDefIni->PushMarker();

				char *pAlias = pDefIni->AsString();

				if(pDefIni->FindSection(pAlias))
				{
					MaterialInternal_InitialiseFromDefinition(pDefIni, pMat, pAlias);
				}

				pDefIni->PopMarker();
			}
			else if(!StrCaseCmp(pName, "lit"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Lit) | (pDefIni->AsBool(0) ? MF_Lit : NULL);
			}
			else if(!StrCaseCmp(pName, "prelit"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Lit) | (!pDefIni->AsBool(0) ? MF_Lit : NULL);
			}
			else if(!StrCaseCmp(pName, "diffusecolour"))
			{
				pMat->diffuse = pDefIni->AsVector4();
			}
			else if(!StrCaseCmp(pName, "ambientcolour"))
			{
				pMat->ambient = pDefIni->AsVector4();
			}
			else if(!StrCaseCmp(pName, "specularcolour"))
			{
				pMat->specular = pDefIni->AsVector4();
			}
			else if(!StrCaseCmp(pName, "specularpower"))
			{
				pMat->specularPow = pDefIni->AsFloat(0);
			}
			else if(!StrCaseCmp(pName, "emissivecolour"))
			{
				pMat->illum = pDefIni->AsVector4();
			}
			else if(!StrCaseCmp(pName, "mask"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Mask) | (pDefIni->AsBool(0) ? MF_Mask : NULL);
			}
			else if(!StrCaseCmp(pName, "doublesided"))
			{
				pMat->materialType = (pMat->materialType & ~MF_DoubleSided) | (pDefIni->AsBool(0) ? MF_DoubleSided : NULL);
			}
			else if(!StrCaseCmp(pName, "backfacecull"))
			{
				pMat->materialType = (pMat->materialType & ~MF_DoubleSided) | (!pDefIni->AsBool(0) ? MF_DoubleSided : NULL);
			}
			else if(!StrCaseCmp(pName, "additive"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (pDefIni->AsBool(0) ? MF_Additive : NULL);
			}
			else if(!StrCaseCmp(pName, "subtractive"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (pDefIni->AsBool(0) ? MF_Subtractive : NULL);
			}
			else if(!StrCaseCmp(pName, "alpha"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (pDefIni->AsBool(0) ? MF_AlphaBlend : NULL);
			}
			else if(!StrCaseCmp(pName, "blend"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (pDefIni->AsInt(0) << 1);
			}
			else if(!StrCaseCmp(pName, "texture"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(pDefIni->AsString());
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "diffusemap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(pDefIni->AsString());
				pMat->diffuseMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "diffusemap2"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(pDefIni->AsString());
				pMat->materialType |= MF_DiffuseMap2;
				pMat->diffuseMap2Index = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "normalmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(pDefIni->AsString());
				pMat->materialType |= MF_NormalMap;
				pMat->normalMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "detailmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(pDefIni->AsString());
				pMat->materialType |= MF_DetailTexture;
				pMat->detailMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "envmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(pDefIni->AsString());
				pMat->materialType |= MF_SphereEnvMap;
				pMat->envMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "lightmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(pDefIni->AsString());
				pMat->materialType |= MF_LightMap;
				pMat->lightMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "bumpmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(pDefIni->AsString());
				pMat->materialType |= MF_BumpMap;
				pMat->bumpMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "reflectionmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(pDefIni->AsString());
				pMat->materialType |= MF_BumpMap;
				pMat->bumpMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "specularmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture_Create(pDefIni->AsString());
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
				pMat->uFrames = pDefIni->AsInt(0);
				pMat->vFrames = pDefIni->AsInt(1);
				pMat->frameTime = pDefIni->AsFloat(2);

				pMat->textureMatrix.SetIdentity();
				pMat->textureMatrix.Scale(Vector(1.0f/(float)pMat->uFrames, 1.0f/(float)pMat->vFrames, 1.0f));
			}

			pDefIni->GetNextLine();
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


