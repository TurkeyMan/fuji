/**** Defines ****/

#define MAX_MATERIALS 254

/**** Includes ****/

#include "Common.h"
#include "Texture.h"
#include "Material.h"
#include "FileSystem.h"
#include "IniFile.h"

/**** Globals ****/

IniFile Material::materialDefinitions;

PtrListDL<Material> materialList;

Material Material::current;
Material *Material::pNone = NULL;

char matDesc[32][4] = {"M","Na","Ds","Ad","T","","A","A3","L","Ls","Le","Dm","E","Es","","","P","C","B","N","D","Ec","E","Es","D2","Lm","D","U","","","",""};

/**** Functions ****/

void Material_InitModule()
{
	CALLSTACK;

	materialList.Init("Material List", MAX_MATERIALS);

	Material::materialDefinitions.Create(File_SystemPath("Materials.ini"));
	Material::pNone = Material::Create("None");
}

void Material_DeinitModule()
{
	CALLSTACK;

	Material::materialDefinitions.Release();

	materialList.Deinit();
}

char* Material::GetIDString()
{
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

	return pMat;
}

void Material::CreateMaterialFromDefinition(Material *pMat, char *pDefinition)
{
	if(materialDefinitions.FindSection(pDefinition))
	{
		materialDefinitions.GetNextLine();

		while(!materialDefinitions.EndOfFile() && !materialDefinitions.IsSection())
		{
			char *pName = materialDefinitions.GetName();
			if(!stricmp(pName, "alias"))
			{
				materialDefinitions.PushMarker();

				char *pAlias = materialDefinitions.AsString();

				if(materialDefinitions.FindSection(pAlias))
				{
					CreateMaterialFromDefinition(pMat, pAlias);
				}

				materialDefinitions.PopMarker();
			}
			else if(!stricmp(pName, "lit"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Lit) | (materialDefinitions.AsBool(0) ? MF_Lit : NULL);
			}
			else if(!stricmp(pName, "prelit"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Lit) | (!materialDefinitions.AsBool(0) ? MF_Lit : NULL);
			}
			else if(!stricmp(pName, "texture"))
			{
				pMat->pTextures[pMat->textureCount] = Texture::LoadTexture(File_SystemPath(materialDefinitions.AsString()));
				pMat->textureCount++;
			}
			else if(!stricmp(pName, "diffuse"))
			{
				pMat->diffuse = materialDefinitions.AsVector4();
			}
			else if(!stricmp(pName, "ambient"))
			{
				pMat->ambient = materialDefinitions.AsVector4();
			}
			else if(!stricmp(pName, "specular"))
			{
				pMat->specular = materialDefinitions.AsVector4();
			}
			else if(!stricmp(pName, "power"))
			{
				pMat->specularPow = materialDefinitions.AsFloat(0);
			}
			else if(!stricmp(pName, "emissive"))
			{
				pMat->illum = materialDefinitions.AsVector4();
			}
			else if(!stricmp(pName, "mask"))
			{
				pMat->materialType = (pMat->materialType & ~MF_Mask) | (materialDefinitions.AsBool(0) ? MF_Mask : NULL);
			}
			else if(!stricmp(pName, "doublesided"))
			{
				pMat->materialType = (pMat->materialType & ~MF_DoubleSided) | (materialDefinitions.AsBool(0) ? MF_DoubleSided : NULL);
			}
			else if(!stricmp(pName, "backfacecull"))
			{
				pMat->materialType = (pMat->materialType & ~MF_DoubleSided) | (!materialDefinitions.AsBool(0) ? MF_DoubleSided : NULL);
			}
			else if(!stricmp(pName, "additive"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (materialDefinitions.AsBool(0) ? MF_Additive : NULL);
			}
			else if(!stricmp(pName, "subtractive"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (materialDefinitions.AsBool(0) ? MF_Subtractive : NULL);
			}
			else if(!stricmp(pName, "alpha"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (materialDefinitions.AsBool(0) ? MF_AlphaBlend : NULL);
			}
			else if(!stricmp(pName, "blend"))
			{
				pMat->materialType = (pMat->materialType & ~MF_BlendMask) | (materialDefinitions.AsInt(0) << 1);
			}
			else if(!stricmp(pName, "normalmap"))
			{
				pMat->materialType |= MF_NormalMap;
				pMat->normalMapIndex = materialDefinitions.AsInt(0);
			}
			else if(!stricmp(pName, "detailmap"))
			{
				pMat->materialType |= MF_DetailTexture;
				pMat->detailMapIndex = materialDefinitions.AsInt(0);
			}
			else if(!stricmp(pName, "envmap"))
			{
				pMat->materialType |= MF_SphereEnvMap;
				pMat->envMapIndex = materialDefinitions.AsInt(0);
			}
			else if(!stricmp(pName, "lightmap"))
			{
				pMat->materialType |= MF_LightMap;
				pMat->lightMapIndex = materialDefinitions.AsInt(0);
			}
			else if(!stricmp(pName, "celshading"))
			{
				pMat->materialType |= MF_CelShading;
	//				pMat-> = materialDefinitions.AsInt(0);
			}
			else if(!stricmp(pName, "bumpmap"))
			{
				pMat->materialType |= MF_BumpMap;
				pMat->bumpMapIndex = materialDefinitions.AsInt(0);
			}
			else if(!stricmp(pName, "phong"))
			{
				pMat->materialType |= MF_PerPixelLighting;
	//				pMat-> = materialDefinitions.AsInt(0);
			}

			materialDefinitions.GetNextLine();
		}
	}
}

Material* Material::Create(char *pName)
{
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
			pMat = Material::CreateDefault();
			Texture *pTex = Texture::LoadTexture(pTexName);

			pMat->refCount = 1;
		}
	}

	return pMat;
}

void Material::Release()
{
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
	current = *this;

	// set some render states

	// choose renderer

	// configure renderer
}

