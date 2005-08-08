/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "Display.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFFileSystem.h"
#include "MFIni.h"
#include "PtrList.h"

#include "Primitive.h"
#include "Font.h"
#include "Input.h"

#include "SysLogo-256.h"
#include "SysLogo-64.h"


/**** internal functions ****/

void		MaterialInternal_Update(MFMaterial *pMaterial);
const char*	MaterialInternal_GetIDString(MFMaterial *pMaterial);

void		MaterialInternal_InitialiseFromDefinition(MFIni *pDefIni, MFMaterial *pMat, const char *pDefinition);


/**** Globals ****/

struct MaterialDefinition
{
	const char *pName;
	MFIni *pIni;
	bool ownsIni;

	MaterialDefinition *pNextDefinition;
};

void MFMaterial_DestroyDefinition(MaterialDefinition *pDefinition);

PtrListDL<MaterialDefinition> gMaterialDefList;
MaterialDefinition *pDefinitionRegistry = NULL;

PtrList<MFMaterial> gMaterialList;
PtrList<MFMaterialType> gMaterialRegistry;

MFMaterial *pCurrentMaterial = NULL;

MFMaterial *pNoneMaterial = NULL;
MFMaterial *pWhiteMaterial = NULL;
MFMaterial *pSysLogoLarge = NULL;
MFMaterial *pSysLogoSmall = NULL;

#if defined(_PSP)
#include "connected.h"
#include "disconnected.h"
#include "power.h"
#include "charging.h"
#include "usb_icon.h"

MFMaterial *pConnected;
MFMaterial *pDisconnected;
MFMaterial *pPower;
MFMaterial *pCharging;
MFMaterial *pUSB;
#endif

char matDesc[32][4] = {"M","Na","Ds","Ad","T","","A","A3","L","Ls","Le","Dm","E","Es","","","P","C","B","N","D","Ec","E","Es","D2","Lm","D","U","","","",""};

void Mat_Standard_Register();

MaterialBrowser matBrowser;

/**** Functions ****/

void MFMaterial_InitModule()
{
	CALLSTACK;

	gMaterialRegistry.Init("Material Registry", gDefaults.material.maxMaterialTypes);
	gMaterialDefList.Init("Material Definitions List", gDefaults.material.maxMaterialDefs);
	gMaterialList.Init("Material List", gDefaults.material.maxMaterials);

	DebugMenu_AddItem("Material Browser", "Fuji Options", &matBrowser);

	Mat_Standard_Register();

	if(MFMaterial_AddDefinitionsFile("Materials.ini", "Materials.ini"))
	{
		LOGD("Warning: Failed to load Materials.ini");
	}

	// create the logo textures from raw data
	MFTexture *pSysLogoLargeTexture = MFTexture_CreateFromRawData("SysLogoLarge", SysLogo_256_data, SysLogo_256_width, SysLogo_256_height, TexFmt_A8R8G8B8, TEX_VerticalMirror);
	MFTexture *pSysLogoSmallTexture = MFTexture_CreateFromRawData("SysLogoSmall", SysLogo_64_data, SysLogo_64_width, SysLogo_64_height, TexFmt_A8R8G8B8, TEX_VerticalMirror);

	// create standard materials
	pNoneMaterial = MFMaterial_Create("_None");
	pWhiteMaterial = MFMaterial_Create("_White");
	pSysLogoLarge = MFMaterial_Create("SysLogoLarge");
	pSysLogoSmall = MFMaterial_Create("SysLogoSmall");

	// release a reference to the logo textures
	MFTexture_Destroy(pSysLogoLargeTexture);
	MFTexture_Destroy(pSysLogoSmallTexture);

#if defined(_PSP)
	// create PSP specific stock materials
	MFTexture *pConnectedTexture = MFTexture_CreateFromRawData("connected", connected_data, connected_width, connected_height, TexFmt_A8R8G8B8, TEX_VerticalMirror);
	MFTexture *pDisconnectedTexture = MFTexture_CreateFromRawData("disconnected", disconnected_data, disconnected_width, disconnected_height, TexFmt_A8R8G8B8, TEX_VerticalMirror);
	MFTexture *pPowerTexture = MFTexture_CreateFromRawData("power", power_data, power_width, power_height, TexFmt_A8R8G8B8, TEX_VerticalMirror);
	MFTexture *pChargingTexture = MFTexture_CreateFromRawData("charging", charging_data, charging_width, charging_height, TexFmt_A8R8G8B8, TEX_VerticalMirror);
	MFTexture *pUSBTexture = MFTexture_CreateFromRawData("usb_icon", usb_icon_data, usb_icon_width, usb_icon_height, TexFmt_A8R8G8B8, TEX_VerticalMirror);

	pConnected = MFMaterial_Create("connected");
	pDisconnected = MFMaterial_Create("disconnected");
	pPower = MFMaterial_Create("power");
	pCharging = MFMaterial_Create("charging");
	pUSB = MFMaterial_Create("usb_icon");

	MFTexture_Destroy(pConnectedTexture);
	MFTexture_Destroy(pDisconnectedTexture);
	MFTexture_Destroy(pPowerTexture);
	MFTexture_Destroy(pChargingTexture);
	MFTexture_Destroy(pUSBTexture);
#endif
}

void MFMaterial_DeinitModule()
{
	CALLSTACK;

	// destroy stock materials
	MFMaterial_Destroy(pNoneMaterial);
	MFMaterial_Destroy(pWhiteMaterial);
	MFMaterial_Destroy(pSysLogoLarge);
	MFMaterial_Destroy(pSysLogoSmall);

#if defined(_PSP)
	// destroy PSP specific stock materials
	MFMaterial_Destroy(pConnected);
	MFMaterial_Destroy(pDisconnected);
	MFMaterial_Destroy(pPower);
	MFMaterial_Destroy(pCharging);
	MFMaterial_Destroy(pUSB);
#endif

	MaterialDefinition *pDef = pDefinitionRegistry;

	while(pDef)
	{
		MaterialDefinition *pNext = pDef->pNextDefinition;
		MFMaterial_DestroyDefinition(pDef);
		pDef = pNext;
	}

	gMaterialList.Deinit();
	gMaterialDefList.Deinit();
	gMaterialRegistry.Deinit();
}

void MFMaterial_Update()
{
	CALLSTACK;

	MFMaterial **ppMatIterator = gMaterialList.Begin();

	while(*ppMatIterator)
	{
		if((*ppMatIterator)->pType->materialCallbacks.pUpdate)
			(*ppMatIterator)->pType->materialCallbacks.pUpdate(*ppMatIterator);

		ppMatIterator++;
	}
}

// interface functions
int MFMaterial_AddDefinitionsFile(const char *pName, const char *pFilename)
{
	CALLSTACK;

	MaterialDefinition *pDef = gMaterialDefList.Create();

	pDef->pName = pName;
	pDef->pIni = MFIni::Create(pFilename);
	if (!pDef->pIni)
	{
		gMaterialDefList.Destroy(pDef);
		LOGD("Error: Couldnt create material definitions...");
		return 1;
	}

	pDef->ownsIni = true;

	pDef->pNextDefinition = pDefinitionRegistry;
	pDefinitionRegistry = pDef;

	return 0;
}

int MFMaterial_AddDefinitionsIni(const char *pName, MFIni *pMatDefs)
{
	CALLSTACK;

	MaterialDefinition *pDef = gMaterialDefList.Create();

	pDef->pName = pName;
	pDef->pIni = pMatDefs;
	pDef->ownsIni = false;

	pDef->pNextDefinition = pDefinitionRegistry;
	pDefinitionRegistry = pDef;

	return 0;
}

void MFMaterial_DestroyDefinition(MaterialDefinition *pDefinition)
{
	CALLSTACK;

	if(pDefinition->ownsIni)
	{
		MFIni::Destroy(pDefinition->pIni);
		pDefinition->pIni = NULL;
	}

	gMaterialDefList.Destroy(pDefinition);
}

void MFMaterial_RemoveDefinitions(const char *pName)
{
	CALLSTACK;

	MaterialDefinition *pDef = pDefinitionRegistry;

	if(!pDef)
		return;

	if(!strcmp(pDef->pName, pName))
	{
		pDefinitionRegistry = pDef->pNextDefinition;
		MFMaterial_DestroyDefinition(pDef);
		return;
	}

	while(pDef->pNextDefinition)
	{
		if(!strcmp(pDef->pNextDefinition->pName, pName))
		{
			MaterialDefinition *pDestroy = pDef->pNextDefinition;
			pDef->pNextDefinition = pDef->pNextDefinition->pNextDefinition;

			MFMaterial_DestroyDefinition(pDestroy);
			return;
		}

		pDef = pDef->pNextDefinition;
	}
}

void MFMaterial_RegisterMaterialType(const char *pName, const MFMaterialCallbacks *pCallbacks)
{
	CALLSTACK;

	MFMaterialType *pMatType = (MFMaterialType*)Heap_Alloc(sizeof(MFMaterialType) + strlen(pName) + 1);

	pMatType->pTypeName = (char*)&pMatType[1];
	strcpy(pMatType->pTypeName, pName);

	memcpy(&pMatType->materialCallbacks, pCallbacks, sizeof(MFMaterialCallbacks));

	gMaterialRegistry.Create(pMatType);
}

void Materual_UnregisterMaterialType(const char *pName)
{
	CALLSTACK;

	MFMaterialType *pMatType = MaterialInternal_GetMaterialType(pName);

	DBGASSERT(pMatType, STR("Material type '%s' doesn't exist!", pName));

	if(pMatType)
		gMaterialRegistry.Destroy(pMatType);
}

MFMaterialType *MaterialInternal_GetMaterialType(const char *pTypeName)
{
	CALLSTACK;

	MFMaterialType **ppIterator = gMaterialRegistry.Begin();

	while(*ppIterator)
	{
		if(!StrCaseCmp(pTypeName, (*ppIterator)->pTypeName)) return *ppIterator;

		ppIterator++;
	}

	return NULL;
}

MFMaterial* MFMaterial_Create(const char *pName)
{
	CALLSTACK;

	MFMaterial *pMat = MFMaterial_Find(pName);

	if(!pMat)
	{
		pMat = (MFMaterial*)Heap_Alloc(sizeof(MFMaterial) + strlen(pName) + 1);
		memset(pMat, 0, sizeof(MFMaterial));

		pMat->pName = (char*)&pMat[1];
		strcpy(pMat->pName, pName);

		gMaterialList.Create(pMat);

		MaterialDefinition *pDef = pDefinitionRegistry;
		while(pDef)
		{
			MFIniLine *pLine = pDef->pIni->GetFirstLine()->FindEntry("Material",pName);
			if (pLine)
			{
				MaterialInternal_InitialiseFromDefinition(pDef->pIni, pMat, pName);
				break;
			}

			pDef = pDef->pNextDefinition;
		}

		if(!pDef)
		{
			// assign material type
			pMat->pType = MaterialInternal_GetMaterialType("Standard");
			pMat->pType->materialCallbacks.pCreateInstance(pMat);

			// set diffuse map parameter

			int index = MFMaterial_GetParamaterIndexFromName(pMat, "diffusemap");

			if(index > -1)
				MFMaterial_SetParamater(pMat, index, 0, (uint32)pName);
		}
	}

	pMat->refCount++;

	return pMat;
}

int MFMaterial_Destroy(MFMaterial *pMaterial)
{
	CALLSTACK;

	pMaterial->refCount--;

	if(!pMaterial->refCount)
	{
		pMaterial->pType->materialCallbacks.pDestroyInstance(pMaterial);

		gMaterialList.Destroy(pMaterial);

		return 0;
	}

	return pMaterial->refCount;
}

MFMaterial* MFMaterial_Find(const char *pName)
{
	CALLSTACK;

	MFMaterial **ppIterator = gMaterialList.Begin();

	while(*ppIterator)
	{
		if(!StrCaseCmp(pName, (*ppIterator)->pName)) return *ppIterator;

		ppIterator++;
	}

	return NULL;
}

MFMaterial* MFMaterial_GetCurrent()
{
	return pCurrentMaterial;
}

void MFMaterial_SetMaterial(MFMaterial *pMaterial)
{
	if(!pMaterial)
		pMaterial = MFMaterial_GetStockMaterial(Mat_White);

	pCurrentMaterial = pMaterial;
}

MFMaterial* MFMaterial_GetStockMaterial(StockMaterials materialIdentifier)
{
	switch(materialIdentifier)
	{
		case Mat_White:
			return pWhiteMaterial;
		case Mat_Unavailable:
			return pNoneMaterial;
		case Mat_SysLogoSmall:
			return pSysLogoSmall;
		case Mat_SysLogoLarge:
			return pSysLogoLarge;
#if defined(_PSP)
		case Mat_USB:
			return pUSB;
		case Mat_Connected:
			return pConnected;
		case Mat_Disconnected:
			return pDisconnected;
		case Mat_Power:
			return pPower;
		case Mat_Charging:
			return pCharging;
#endif
		default:
			DBGASSERT(false, "Invalid Stock Material");
	}

	return NULL;
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

void MaterialInternal_InitialiseFromDefinition(MFIni *pDefIni, MFMaterial *pMat, const char *pDefinition)
{
	CALLSTACK;

	MFIniLine *pLine = pDefIni->GetFirstLine()->FindEntry("material", pDefinition);

	if(pLine)
	{
		pLine = pLine->Sub();

		if(pLine && pLine->IsString(0, "type"))
		{
			pMat->pType = MaterialInternal_GetMaterialType(pLine->GetString(1));

			if(!pMat->pType)
				pMat->pType = MaterialInternal_GetMaterialType("Standard");

			pLine = pLine->Next();
		}
		else
		{
			pMat->pType = MaterialInternal_GetMaterialType("Standard");
		}

		pMat->pType->materialCallbacks.pCreateInstance(pMat);

		while(pLine)
		{
			if(pLine->IsString(0,"type"))
			{
				LOGD(STR("'type' MUST be the first parameter in a material definition... Ignored, Using type '%s'.", pMat->pType->pTypeName));
			}
			else if(pLine->IsString(0,"alias"))
			{
				LOGD("'alias' MUST be the first parameter in a material definition... Ignored.");

DBGASSERT(false, "Fix Me!!!");
//				const char *pAlias = pLine->GetString(1);
//				MaterialInternal_InitialiseFromDefinition(pDefIni, pMat, pAlias);
			}
			else
			{
				const char *pParam = pLine->GetString(0);

				int paramIndex = MFMaterial_GetParamaterIndexFromName(pMat, pParam);
				int numArgs = MFMaterial_GetNumParamaterArgs(pMat, paramIndex);

				for(int a=0; a<numArgs; a++)
				{
					ParamType pt = MFMaterial_GetParamaterArgType(pMat, paramIndex, a);

					switch(pt)
					{
						case ParamType_String:
						{
							const char *pString = pLine->GetString(a+1);
							MFMaterial_SetParamater(pMat, paramIndex, a, (uint32)pString);
							break;
						}

						case ParamType_Float:
						{
							float value = pLine->GetFloat(a+1);
							MFMaterial_SetParamater(pMat, paramIndex, a, (uint32&)value);
							break;
						}

						case ParamType_Int:
						{
							int value = pLine->GetStringCount() > 1 ? pLine->GetInt(a+1) : 1;
							MFMaterial_SetParamater(pMat, paramIndex, a, (uint32&)value);
							break;
						}

						case ParamType_Bool:
						{
							bool value = pLine->GetStringCount() > 1 ? pLine->GetBool(a+1) : true;
							MFMaterial_SetParamater(pMat, paramIndex, a, value ? 1 : 0);
							break;
						}

						case ParamType_Vector3:
						{
							Vector3 vector = pLine->GetVector3(a+1);
							MFMaterial_SetParamater(pMat, paramIndex, a, (uint32)&vector);
							break;
						}

						case ParamType_Vector4:
						{
							Vector4 vector = pLine->GetVector4(a+1);
							MFMaterial_SetParamater(pMat, paramIndex, a, (uint32)&vector);
							break;
						}

						default:
							DBGASSERT(false, "Unknown parameter type..");
					}
				}
			}

			pLine = pLine->Next();
		}
	}
}

int MFMaterial_GetNumParamaters(MFMaterial *pMaterial)
{
	return pMaterial->pType->materialCallbacks.pGetNumParams();
}

const char* MFMaterial_GetParamaterName(MFMaterial *pMaterial, int paramaterIndex)
{
	MFMaterialParamaterInfo *pInfo;

	pInfo = (MFMaterialParamaterInfo*)pMaterial->pType->materialCallbacks.pGetParamaterInfo(paramaterIndex);

	return pInfo->pParamaterName;
}

int MFMaterial_GetParamaterIndexFromName(MFMaterial *pMaterial, const char *pParameterName)
{
	MFMaterialParamaterInfo *pInfo;

	int numParams = pMaterial->pType->materialCallbacks.pGetNumParams();

	for(int a=0; a<numParams; a++)
	{
		pInfo = (MFMaterialParamaterInfo*)pMaterial->pType->materialCallbacks.pGetParamaterInfo(a);
		if(!stricmp(pInfo->pParamaterName, pParameterName))
			return a;
	}

	return -1;
}

int MFMaterial_GetNumParamaterArgs(MFMaterial *pMaterial, int paramaterIndex)
{
	MFMaterialParamaterInfo *pInfo;

	pInfo = (MFMaterialParamaterInfo*)pMaterial->pType->materialCallbacks.pGetParamaterInfo(paramaterIndex);

	return pInfo->numArgs;
}

ParamType MFMaterial_GetParamaterArgType(MFMaterial *pMaterial, int paramaterIndex, int argIndex)
{
	MFMaterialParamaterInfo *pInfo;

	pInfo = (MFMaterialParamaterInfo*)pMaterial->pType->materialCallbacks.pGetParamaterInfo(paramaterIndex);

	return pInfo->pArgTypes[argIndex];
}

void MFMaterial_SetParamater(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 value)
{
	pMaterial->pType->materialCallbacks.pSetParameter(pMaterial, paramaterIndex, argIndex, value);
}

int MFMaterial_GetParamater(MFMaterial *pMaterial, int paramaterIndex, int argIndex, uint32 *pValue)
{
	*pValue = pMaterial->pType->materialCallbacks.pGetParameter(pMaterial, paramaterIndex, argIndex);

	return 0;
}


// material browser
MaterialBrowser::MaterialBrowser()
{
	selection = 0;
	type = MenuType_TextureBrowser;
}

void MaterialBrowser::Draw()
{

}

void MaterialBrowser::Update()
{
	if(Input_WasPressed(IDD_Gamepad, 0, Button_XB_Y))
		pCurrentMenu = pParent;
}

#define TEX_SIZE 64.0f
float MaterialBrowser::ListDraw(bool selected, const Vector3 &_pos, float maxWidth)
{
	Vector3 pos = _pos;

	MFMaterial **i;
	i = gMaterialList.Begin();

	for(int a=0; a<selection; a++) i++;

	MFMaterial *pMaterial = *i;

	Font_DrawText(gpDebugFont, pos+Vector(0.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)-MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? Vector(1,1,0,1) : Vector4::one, STR("%s:", name));
	Font_DrawText(gpDebugFont, pos+Vector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f), 0.0f), MENU_FONT_HEIGHT, selected ? Vector(1,1,0,1) : Vector4::one, STR("%s", pMaterial->pName));
	Font_DrawText(gpDebugFont, pos+Vector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)+MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? Vector(1,1,0,1) : Vector4::one, STR("Type: %s Refs: %d", pMaterial->pType->pTypeName, pMaterial->refCount));

	pos += Vector(maxWidth - (TEX_SIZE + 4.0f + 5.0f), 2.0f, 0.0f);

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(4);
	MFSetColour(0xFFFFFFFF);
	MFSetPosition(pos);
	MFSetPosition(pos + Vector(TEX_SIZE + 4.0f, 0.0f, 0.0f));
	MFSetPosition(pos + Vector(0.0f, TEX_SIZE + 4.0f, 0.0f));
	MFSetPosition(pos + Vector(TEX_SIZE + 4.0f, TEX_SIZE + 4.0f, 0.0f));
	MFEnd();

	pos += Vector(2.0f, 2.0f, 0.0f);

	const int numSquares = 7;
	for(int a=0; a<numSquares; a++)
	{
		for(int b=0; b<numSquares; b++)
		{
			float x, y, w, h;
			w = TEX_SIZE/(float)numSquares;
			h = TEX_SIZE/(float)numSquares;
			x = pos.x + (float)b*w;
			y = pos.y + (float)a*h;

			MFBegin(4);
			MFSetColour(((a+b)&1) ? 0xFFC0C0C0 : 0xFF303030);
			MFSetPosition(x,y,0);
			MFSetPosition(x+w,y,0);
			MFSetPosition(x,y+h,0);
			MFSetPosition(x+w,y+h,0);
			MFEnd();
		}
	}

	MFMaterial_SetMaterial(pMaterial);

	MFPrimitive(PT_TriStrip);

	MFBegin(4);
	MFSetColour(0xFFFFFFFF);
	MFSetTexCoord1(0.0f,0.0f);
	MFSetPosition(pos);
	MFSetTexCoord1(1.0f,0.0f);
	MFSetPosition(pos + Vector(TEX_SIZE, 0.0f, 0.0f));
	MFSetTexCoord1(0.0f,1.0f);
	MFSetPosition(pos + Vector(0.0f, TEX_SIZE, 0.0f));
	MFSetTexCoord1(1.0f,1.0f);
	MFSetPosition(pos + Vector(TEX_SIZE, TEX_SIZE, 0.0f));
	MFEnd();

	return TEX_SIZE + 8.0f;
}

void MaterialBrowser::ListUpdate(bool selected)
{
	if(selected)
	{
		int texCount = gMaterialList.GetLength();

		if(Input_WasPressed(IDD_Gamepad, 0, Button_DLeft))
		{
			selection = selection <= 0 ? texCount-1 : selection-1;

			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(Input_WasPressed(IDD_Gamepad, 0, Button_DRight))
		{
			selection = selection >= texCount-1 ? 0 : selection+1;

			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

Vector3 MaterialBrowser::GetDimensions(float maxWidth)
{
	return Vector(maxWidth, TEX_SIZE + 8.0f, 0.0f);
}

