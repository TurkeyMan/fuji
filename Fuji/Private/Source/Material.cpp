/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "Display.h"
#include "Texture.h"
#include "Material.h"
#include "FileSystem.h"
#include "IniFile.h"

/**** Globals ****/

IniFile Material::materialDefinitions;

PtrListDL<Material> materialList;

Material *Material::pCurrent = NULL;
Material *Material::pNone = NULL;

char matDesc[32][4] = {"M","Na","Ds","Ad","T","","A","A3","L","Ls","Le","Dm","E","Es","","","P","C","B","N","D","Ec","E","Es","D2","Lm","D","U","","","",""};

/**** Functions ****/

void Material_InitModule()
{
	CALLSTACK;

	materialList.Init("Material List", gDefaults.material.maxMaterials);

	if(Material::materialDefinitions.Create(File_SystemPath("Materials.ini")))
	{
		DBGASSERT(false, "Failed to load Materials.ini");
	}

	Material::pNone = Material::Create("None");
}

void Material_DeinitModule()
{
	CALLSTACK;

	Material::materialDefinitions.Release();

	materialList.Deinit();
}

void Material_Update()
{
	Material **ppMatIterator = materialList.Begin();

	while(*ppMatIterator)
	{
		(*ppMatIterator)->Update();
		ppMatIterator++;
	}

}

char* Material::GetIDString()
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

Material* Material::CreateDefault()
{
	CALLSTACK;

	static int defaultCount = 0;

	Material *pMat = materialList.Create();

	memset(pMat, 0, sizeof(Material));

	strcpy(pMat->name, STR("Default%d", defaultCount++));;

	pMat->ambient = Vector4::one;
	pMat->diffuse = Vector4::one;
	pMat->illum = Vector4::zero;
	pMat->specular = Vector4::zero;
	pMat->specularPow = 0;

	pMat->materialType = 0;
	pMat->opaque = true;

	pMat->refCount = 1;

	pMat->textureMatrix = Matrix::identity;
	pMat->curTime = 0.0f;
	pMat->frameTime = 0.0f;
	pMat->uFrames = 1;
	pMat->vFrames = 1;
	pMat->curFrame = 0;

	return pMat;
}

void Material::CreateMaterialFromDefinition(Material *pMat, const char *pDefinition)
{
	CALLSTACK;

	if(materialDefinitions.FindSection(pDefinition))
	{
		materialDefinitions.GetNextLine();

		while(!materialDefinitions.EndOfFile() && !materialDefinitions.IsSection())
		{
			char *pName = materialDefinitions.GetName();
			if(!StrCaseCmp(pName, "alias"))
			{
				materialDefinitions.PushMarker();

				char *pAlias = materialDefinitions.AsString();

				if(materialDefinitions.FindSection(pAlias))
				{
					CreateMaterialFromDefinition(pMat, pAlias);
				}

				materialDefinitions.PopMarker();
			}
			else if(!StrCaseCmp(pName, "lit"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Lit) | (materialDefinitions.AsBool(0) ? MF_Lit : NULL);
			}
			else if(!StrCaseCmp(pName, "prelit"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Lit) | (!materialDefinitions.AsBool(0) ? MF_Lit : NULL);
			}
			else if(!StrCaseCmp(pName, "diffusecolour"))
			{
				pMat->diffuse = materialDefinitions.AsVector4();
			}
			else if(!StrCaseCmp(pName, "ambientcolour"))
			{
				pMat->ambient = materialDefinitions.AsVector4();
			}
			else if(!StrCaseCmp(pName, "specularcolour"))
			{
				pMat->specular = materialDefinitions.AsVector4();
			}
			else if(!StrCaseCmp(pName, "specularpower"))
			{
				pMat->specularPow = materialDefinitions.AsFloat(0);
			}
			else if(!StrCaseCmp(pName, "emissivecolour"))
			{
				pMat->illum = materialDefinitions.AsVector4();
			}
			else if(!StrCaseCmp(pName, "mask"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Mask) | (materialDefinitions.AsBool(0) ? MF_Mask : NULL);
			}
			else if(!StrCaseCmp(pName, "doublesided"))
			{
				pMat->materialType = (pMat->materialType & ~MF_DoubleSided) | (materialDefinitions.AsBool(0) ? MF_DoubleSided : NULL);
			}
			else if(!StrCaseCmp(pName, "backfacecull"))
			{
				pMat->materialType = (pMat->materialType & ~MF_DoubleSided) | (!materialDefinitions.AsBool(0) ? MF_DoubleSided : NULL);
			}
			else if(!StrCaseCmp(pName, "additive"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (materialDefinitions.AsBool(0) ? MF_Additive : NULL);
			}
			else if(!StrCaseCmp(pName, "subtractive"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (materialDefinitions.AsBool(0) ? MF_Subtractive : NULL);
			}
			else if(!StrCaseCmp(pName, "alpha"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (materialDefinitions.AsBool(0) ? MF_AlphaBlend : NULL);
			}
			else if(!StrCaseCmp(pName, "blend"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (materialDefinitions.AsInt(0) << 1);
			}
			else if(!StrCaseCmp(pName, "texture"))
			{
				pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(File_SystemPath(materialDefinitions.AsString()));
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "diffusemap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(File_SystemPath(materialDefinitions.AsString()));
				pMat->diffuseMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "diffusemap2"))
			{
				pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(File_SystemPath(materialDefinitions.AsString()));
				pMat->materialType |= MF_DiffuseMap2;
				pMat->diffuseMap2Index = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "normalmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(File_SystemPath(materialDefinitions.AsString()));
				pMat->materialType |= MF_NormalMap;
				pMat->normalMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "detailmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(File_SystemPath(materialDefinitions.AsString()));
				pMat->materialType |= MF_DetailTexture;
				pMat->detailMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "envmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(File_SystemPath(materialDefinitions.AsString()));
				pMat->materialType |= MF_SphereEnvMap;
				pMat->envMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "lightmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(File_SystemPath(materialDefinitions.AsString()));
				pMat->materialType |= MF_LightMap;
				pMat->lightMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "bumpmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(File_SystemPath(materialDefinitions.AsString()));
				pMat->materialType |= MF_BumpMap;
				pMat->bumpMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "reflectionmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(File_SystemPath(materialDefinitions.AsString()));
				pMat->materialType |= MF_BumpMap;
				pMat->bumpMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "specularmap"))
			{
				pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(File_SystemPath(materialDefinitions.AsString()));
				pMat->materialType |= MF_BumpMap;
				pMat->bumpMapIndex = pMat->textureCount;
				pMat->textureCount++;
			}
			else if(!StrCaseCmp(pName, "celshading"))
			{
				pMat->materialType |= MF_CelShading;
	//				pMat-> = materialDefinitions.AsInt(0);
			}
			else if(!StrCaseCmp(pName, "phong"))
			{
				pMat->materialType |= MF_LitPerPixel;
	//				pMat-> = materialDefinitions.AsInt(0);
			}
			else if(!StrCaseCmp(pName, "animated"))
			{
				pMat->materialType |= MF_Animating;
				pMat->uFrames = materialDefinitions.AsInt(0);
				pMat->vFrames = materialDefinitions.AsInt(1);
				pMat->frameTime = materialDefinitions.AsFloat(2);

				pMat->textureMatrix.SetIdentity();
				pMat->textureMatrix.Scale(Vector(1.0f/(float)pMat->uFrames, 1.0f/(float)pMat->vFrames, 1.0f));
			}

			materialDefinitions.GetNextLine();
		}
	}
}

Material* Material::Create(const char *pName)
{
	CALLSTACK;

	Material *pMat = NULL;

	// if material already exists, bump refCount

	if(materialDefinitions.FindSection(pName))
	{
		pMat = Material::CreateDefault();

		strcpy(pMat->name, pName);

		CreateMaterialFromDefinition(pMat, pName);

		pMat->refCount = 1;
	}
	else
	{
		char *pTexName = File_SystemPath(STR("%s.tga", pName));

		if(File_Exists(pTexName))
		{
			LOGD(STR("No material definition for '%s'. Using default material.\n", pName));

			pMat = Material::CreateDefault();
			pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(pTexName);
			pMat->diffuseMapIndex = pMat->textureCount;
			pMat->textureCount++;

			pMat->refCount = 1;
		}
		else
		{
			LOGD(STR("No material definition or texture found for material '%s'. Using 'None'.\n", pName));

			pMat = Material::Create("None");
		}
	}

	return pMat;
}

void Material::Release()
{
	CALLSTACK;

	refCount--;

	if(!refCount)
	{
		for(uint16 a=0; a<textureCount; a++)
		{
			pTextures[a]->Release();
		}

		materialList.Destroy(materialList.Find(this));
	}
}

void Material::Use()
{
	CALLSTACK;

	pCurrent = this;
}

void Material::Update()
{
	if(materialType & MF_Animating)
	{
		curTime += TIMEDELTA;

		while(curTime >= frameTime)
		{
			curTime -= frameTime;

			curFrame++;
			curFrame = curFrame % (uFrames*vFrames);

			textureMatrix.SetZAxis(Vector((1.0f/(float)uFrames) * (float)(curFrame%uFrames), (1.0f/(float)vFrames) * (float)(curFrame/vFrames), 0.0f));
		}
	}
}


