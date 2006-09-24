#include "Fuji.h"
#include "Display_Internal.h"
#include "MFView.h"
#include "DebugMenu_Internal.h"
#include "MFMaterial.h"
#include "MFInput.h"
#include "MFFont.h"
#include "MFPrimitive.h"
#include "MFSystem.h"

#if defined(_WINDOWS)
#include <d3d9.h>
extern IDirect3DDevice9 *pd3dDevice;
#endif
#if defined(_MFXBOX)
extern IDirect3DDevice8 *pd3dDevice;
#endif

bool debugMenuEnabled;
bool buttonsDown;

Menu rootMenu;
MenuObject *pCurrentMenu = &rootMenu;
MFVector debugMenuPos = MakeVector(50.0f, 50.0f, 0.0f);

MFVector Menu::menuPosition = MakeVector(MENU_X, MENU_Y, 0.0f);
MFVector Menu::menuDimensions = MakeVector(MENU_WIDTH, MENU_HEIGHT, 0.0f);
MFVector Menu::colour = MakeVector(0.0f, 0.0f, 1.0f, 0.5f);
MFVector Menu::folderColour = MakeVector(0.5f, 0.627f, 1.0f, 1.0f);

MenuItemPosition2D menuPos(&Menu::menuPosition, 60.0f);
MenuItemPosition2D menuDim(&Menu::menuDimensions, 60.0f);
MenuItemColour menuCol(&Menu::colour);
MenuItemColour menuItemCom(&Menu::folderColour);

const MFVector MenuItemColour::colourInit = MakeVector(1.0f, 1.0f, 1.0f, 1.0f);

#define COLOUR_PRESETS 10
uint32 MenuItemColour::presets[COLOUR_PRESETS] =
{
	0xFFFFFFFF, // white
	0xFFFF0000, // red
	0xFF00FF00, // green
	0xFF0000FF, // blue
	0xFFFFFF00, // yellow
	0xFF00FFFF, // cyan
	0xFFFF00FF, // magenta
	0xFFFFAE00, // orange
	0xFF0090FF, // sky blue
	0xFF000000	// black
};

void DebugMenu_InitModule()
{
	// create root menu
	MFString_Copy(rootMenu.name, "root");
	rootMenu.menuDepth = 0;
	rootMenu.numChildren = 0;
	rootMenu.pCallback = NULL;
	rootMenu.pParent = NULL;
	rootMenu.pUserData = NULL;
	rootMenu.selection = 0;

	DebugMenu_AddMenu("Fuji Options", &rootMenu);
	DebugMenu_AddMenu("Debug Menu Options", "Fuji Options");

	DebugMenu_AddItem("Menu Position", "Debug Menu Options", &menuPos);
	DebugMenu_AddItem("Menu Dimensions", "Debug Menu Options", &menuDim);
	DebugMenu_AddItem("Menu Colour", "Debug Menu Options", &menuCol);
	DebugMenu_AddItem("Menu Item Colour", "Debug Menu Options", &menuItemCom);
}

void DebugMenu_DeinitModule()
{
	// destroy all menu's
	DebugMenu_DestroyMenuTree(&rootMenu);
}

Menu* DebugMenu_GetRootMenu()
{
	return &rootMenu;
}

bool DebugMenu_IsEnabled()
{
	return debugMenuEnabled;
}

void DebugMenu_Update()
{
#if defined(_PSP)
	if(!buttonsDown && MFInput_Read(Button_PP_Start, IDD_Gamepad) && MFInput_Read(Button_PP_Select, IDD_Gamepad))
#else
	if(!buttonsDown && MFInput_Read(Button_XB_LThumb, IDD_Gamepad) && MFInput_Read(Button_XB_RThumb, IDD_Gamepad))
#endif
	{
		debugMenuEnabled = !debugMenuEnabled;
		buttonsDown = true;
	}

#if defined(_PSP)
	if(buttonsDown && (!MFInput_Read(Button_PP_Start, IDD_Gamepad) || !MFInput_Read(Button_PP_Select, IDD_Gamepad)))
#else
	if(buttonsDown && (!MFInput_Read(Button_XB_LThumb, IDD_Gamepad) || !MFInput_Read(Button_XB_RThumb, IDD_Gamepad)))
#endif
	{
		buttonsDown = false;
	}

	if(debugMenuEnabled)
		pCurrentMenu->Update();
}

void DebugMenu_Draw()
{
	if(MFInput_Read(Button_XB_RTrig, IDD_Gamepad)) return;

	MFView_Push();
	MFView_SetOrtho();

	if(debugMenuEnabled)
		pCurrentMenu->Draw();

	MFView_Pop();
}

void DebugMenu_AddItem(const char *name, Menu *pParent, MenuObject *pObject, DebugCallback callback, void *userData)
{
	MFDebug_Assert(pParent, "Invalid parent menu.");
	MFDebug_Assert(pParent->type == MenuType_Menu, MFStr("Cant add menu '%s', Parent is not of Menu type.", name));
	MFDebug_Assert(strlen(name) < 64, "Max of 64 characters in Menu Name.");
	MFDebug_Assert(pParent->numChildren < MENU_MAX_CHILDREN, MFStr("Maximum number of items in menu: '&s'", pParent->name)); 

	MFString_Copy(pObject->name, name);

	pObject->pParent = pParent;
	pObject->menuDepth = pParent->menuDepth+1;

	pObject->pCallback = callback;
	pObject->pUserData = userData;

	pParent->pChildren[pParent->numChildren] = pObject;
	++pParent->numChildren;
}

void DebugMenu_AddItem(const char *name, const char *pParentName, MenuObject *pObject, DebugCallback callback, void *userData)
{
	DebugMenu_AddItem(name, DebugMenu_GetMenuByName(pParentName), pObject, callback, userData);
}

void DebugMenu_AddMenu(const char *name, Menu *pParent, DebugCallback callback, void *userData)
{
	MFDebug_Assert(pParent, "Invalid parent menu.");
	MFDebug_Assert(pParent->type == MenuType_Menu, MFStr("Cant add menu '%s', Parent is not of Menu type.", name));
	MFDebug_Assert(strlen(name) < 64, "Max of 64 characters in Menu Name.");
	MFDebug_Assert(pParent->numChildren < MENU_MAX_CHILDREN, MFStr("Maximum number of items in menu: '&s'", pParent->name)); 

//	Menu *pMenu = Heap_New(Menu);
	Menu *pMenu = new Menu;

	MFString_Copy(pMenu->name, name);
	pMenu->type = MenuType_Menu;

	pMenu->pParent = pParent;
	pMenu->menuDepth = pParent->menuDepth+1;

	pMenu->numChildren = 0;
	pMenu->selection = 0;

	pMenu->pCallback = callback;
	pMenu->pUserData = userData;

	pParent->pChildren[pParent->numChildren] = pMenu;
	++pParent->numChildren;
}

void DebugMenu_AddMenu(const char *name, const char *pParentName, DebugCallback callback, void *userData)
{
	DebugMenu_AddMenu(name, DebugMenu_GetMenuByName(pParentName), callback, userData);
}

bool DebugMenu_DestroyMenu(const char *pName, Menu *pSearchMenu)
{
	for(int a=0; a<pSearchMenu->numChildren; a++)
	{
		if(pSearchMenu->pChildren[a]->type == MenuType_Menu)
		{
			if(!MFString_CaseCmp(pSearchMenu->pChildren[a]->name, pName))
			{
				DebugMenu_DestroyMenuTree((Menu*)pSearchMenu->pChildren[a]);
				return true;
			}

			if(DebugMenu_DestroyMenu(pName, (Menu*)pSearchMenu->pChildren[a]))
				return true;
		}
	}

	return false;
}

void DebugMenu_DestroyMenuTree(Menu *pMenu)
{
	int a;

	for(a=0; a<pMenu->numChildren; a++)
	{
		if(pMenu->pChildren[a]->type == MenuType_Menu)
		{
			DebugMenu_DestroyMenuTree((Menu*)pMenu->pChildren[a]);
		}
	}

	if(pMenu->pParent)
	{
		for(a=0; a<pMenu->pParent->numChildren; a++)
		{
			if(pMenu->pParent->pChildren[a] == pMenu)
			{
				pMenu->pParent->pChildren[a] = pMenu->pParent->pChildren[pMenu->pParent->numChildren-1];
				pMenu->pParent->numChildren--;
				break;
			}
		}
	}

	if(pMenu != &rootMenu)
		delete pMenu;
}

Menu* DebugMenu_GetMenuByName(const char *name, Menu *pSearchMenu)
{
	Menu *pResult = NULL;

	for(int a=0; a<pSearchMenu->numChildren; a++)
	{
		if(pSearchMenu->pChildren[a]->type == MenuType_Menu)
		{
			if(!MFString_CaseCmp(pSearchMenu->pChildren[a]->name , name))
				return (Menu*)pSearchMenu->pChildren[a];

			pResult = DebugMenu_GetMenuByName(name, (Menu*)pSearchMenu->pChildren[a]);

			if(pResult) return pResult;
		}
	}

	return NULL;
}

void MenuObject::Draw()
{
	// dont draw anything
}

void MenuObject::Update()
{
	// allow back to parent
	if(MFInput_WasPressed(Button_XB_Y, IDD_Gamepad))
		pCurrentMenu = pParent;
}

int Menu::GetSelected()
{
	return this->selection;
}

int Menu::GetItemCount()
{
	return numChildren;
}

float Menu::ListDraw(bool selected, const MFVector &pos, float maxWidth)
{
	MFFont_DrawText(MFFont_GetDebugFont(), pos, MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : folderColour, name);
	return MENU_FONT_HEIGHT;
}

void Menu::ListUpdate(bool selected)
{
	if(selected)
	{
		if(MFInput_WasPressed(Button_XB_A, IDD_Gamepad))
			pCurrentMenu = this;
	}
}

void Menu::Draw()
{
	MFVector dimensions = MakeVector(0.0f, 0.0f, 0.0f);
	MFVector currentPos;
	float requestedWidth = menuDimensions.x-40.0f;
	float selStart, selEnd;
	int a;

	// get menu size
	for(a=0; a<numChildren; a++)
	{
		MFVector dim = pChildren[a]->GetDimensions(requestedWidth);

		if(selection==a)
		{
			selStart = dimensions.y;
			selEnd = selStart + dim.y;

			if(selStart < -yOffset)
			{
				targetOffset = -selStart;
			}

			if(selEnd > menuDimensions.y - 75.0f - yOffset)
			{
				targetOffset = -(selEnd-(menuDimensions.y-75.0f));
			}
		}

		dimensions.y += dim.y;
		dimensions.x = MFMax(dimensions.x, dim.x);
	}

	if(targetOffset != yOffset)
	{
		yOffset -= MFAbs(yOffset-targetOffset) < 0.1f ? yOffset-targetOffset : (yOffset-targetOffset)*0.1f;
	}

	currentPos = MakeVector(menuPosition.x+20.0f, menuPosition.y+50.0f + yOffset, 0.0f);

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(4);
	MFSetColour(colour*0.4f);
	MFSetPosition(menuPosition.x, menuPosition.y, 0);
	MFSetColour(colour*0.8f);
	MFSetPosition(menuPosition.x+menuDimensions.x, menuPosition.y, 0);
	MFSetColour(colour*0.6f);
	MFSetPosition(menuPosition.x, menuPosition.y+menuDimensions.y, 0);
	MFSetColour(colour);
	MFSetPosition(menuPosition.x+menuDimensions.x, menuPosition.y+menuDimensions.y, 0);
	MFEnd();

	MFFont_DrawText(MFFont_GetDebugFont(), menuPosition.x+10.0f, menuPosition.y+5.0f, MENU_FONT_HEIGHT*1.5f, MakeVector(1,0.6875f,0.5f,1), name);

	MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_SysLogoSmall));
	float logoMargin = 5.0f;
	float iconSize = 35.0f;

	MFPrimitive(PT_TriStrip);
	MFBegin(4);
	MFSetColour(1,1,1,1);
	MFSetTexCoord1(0,0);
	MFSetPosition((menuPosition.x+menuDimensions.x) - logoMargin*2 - iconSize, menuPosition.y + logoMargin, 0);
	MFSetTexCoord1(1,0);
	MFSetPosition((menuPosition.x+menuDimensions.x) - logoMargin*2, menuPosition.y + logoMargin, 0);
	MFSetTexCoord1(0,1);
	MFSetPosition((menuPosition.x+menuDimensions.x) - logoMargin*2 - iconSize, menuPosition.y + logoMargin + iconSize, 0);
	MFSetTexCoord1(1,1);
	MFSetPosition((menuPosition.x+menuDimensions.x) - logoMargin*2, menuPosition.y + logoMargin + iconSize, 0);
	MFEnd();

#if defined(_WINDOWS) || defined(_MFXBOX)
	pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
#endif

	MFPrimitive(PT_TriStrip|PT_Untextured);
	MFBegin(4);
	MFSetColour(0xA0000000);
	MFSetPosition(menuPosition.x+15, menuPosition.y+45, 0);
	MFSetColour(0xA0000000);
	MFSetPosition(menuPosition.x+menuDimensions.x-15, menuPosition.y+45, 0);
	MFSetColour(0xA0000000);
	MFSetPosition(menuPosition.x+15, menuPosition.y+menuDimensions.y-15, 0);
	MFSetColour(0xA0000000);
	MFSetPosition(menuPosition.x+menuDimensions.x-15, menuPosition.y+menuDimensions.y-15, 0);
	MFEnd();

#if defined(_WINDOWS) || defined(_MFXBOX)
	pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESS);
	pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0);
#endif

	for(a=0; a<numChildren; a++)
	{
		if(currentPos.y > menuPosition.y + menuDimensions.y - 15.0f) break;

		if(selection==a)
		{
			float height = pChildren[a]->GetDimensions(requestedWidth).y;

			if(currentPos.y + height < menuPosition.y + 45.0f)
			{
				currentPos.y += height;
				continue;
			}

			MFPrimitive(PT_TriStrip|PT_Untextured);

			MFBegin(4);
			MFSetColour(0xC0000080);
			MFSetPosition(menuPosition.x+15, currentPos.y, 0);
			MFSetColour(0xC00000D0);
			MFSetPosition(menuPosition.x+menuDimensions.x-15, currentPos.y, 0);
			MFSetColour(0xC0000090);
			MFSetPosition(menuPosition.x+15, currentPos.y + height, 0);
			MFSetColour(0xC00000FF);
			MFSetPosition(menuPosition.x+menuDimensions.x-15, currentPos.y + height, 0);
			MFEnd();
		}

		currentPos.y += pChildren[a]->ListDraw(selection==a, currentPos, requestedWidth);
	}

#if defined(_WINDOWS) || defined(_MFXBOX)
	pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
#endif
}

void Menu::Update()
{
	static int gMenuHeld = 0;
	static float gHoldTimeout = 0.3f;

	// test controls and move cursor
	if(MFInput_WasPressed(Button_DUp, IDD_Gamepad) || (gMenuHeld == Button_DUp && gHoldTimeout<=0.0f))
	{
		selection = selection > 0 ? selection-1 : numChildren-1;

		if(gMenuHeld)
			gHoldTimeout += 0.1f;
		else
			gMenuHeld = Button_DUp;
	}

	if(MFInput_WasPressed(Button_DDown, IDD_Gamepad) || (gMenuHeld == Button_DDown && gHoldTimeout<=0.0f))
	{
		selection = selection < numChildren-1 ? selection+1 : 0;

		if(gMenuHeld)
			gHoldTimeout += 0.1f;
		else
			gMenuHeld = Button_DDown;
	}

	if(gMenuHeld)
	{
		gHoldTimeout -= MFSystem_TimeDelta();

		if(!MFInput_Read(gMenuHeld, IDD_Gamepad))
		{
			gMenuHeld = 0;
			gHoldTimeout = 0.3f;
		}
	}

	if(MFInput_WasPressed(Button_XB_Y, IDD_Gamepad) && pParent)
		pCurrentMenu = pParent;

	for(int a=0; a<numChildren; a++)
	{
		pChildren[a]->ListUpdate(a == selection);
	}
}

MFVector Menu::GetDimensions(float maxWidth)
{
	return MakeVector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}

// MenuItemStatic
float MenuItemStatic::ListDraw(bool selected, const MFVector &pos, float maxWidth)
{
	MFFont_DrawText(MFFont_GetDebugFont(), pos, MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, name);
	return MENU_FONT_HEIGHT;
}

void MenuItemStatic::ListUpdate(bool selected)
{
	if(selected)
		if(pCallback && MFInput_WasPressed(Button_XB_A, IDD_Gamepad))
			pCallback(this, pUserData);
}

MFVector MenuItemStatic::GetDimensions(float maxWidth)
{
	return MakeVector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}

// MenuItemInt
float MenuItemInt::ListDraw(bool selected, const MFVector &pos, float maxWidth)
{
	MFFont_DrawText(MFFont_GetDebugFont(), pos, MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s: %d", name, *pData));
	return MENU_FONT_HEIGHT;
}

void MenuItemInt::ListUpdate(bool selected)
{
	if(selected)
	{
		int t = *pData;

		if(MFInput_WasPressed(Button_XB_B, IDD_Gamepad)) *pData = defaultValue;
		if(MFInput_WasPressed(Button_XB_X, IDD_Gamepad)) *pData = 0;

		if(MFInput_WasPressed(Button_DLeft, IDD_Gamepad))
		{
			*pData -= increment;
		}
		else if(MFInput_WasPressed(Button_DRight, IDD_Gamepad))
		{
			*pData += increment;
		}

		if(*pData < minimumValue) *pData = maximumValue+(*pData-minimumValue);
		if(*pData > maximumValue) *pData = minimumValue+(*pData-maximumValue);

		if(pCallback && t != *pData)
			pCallback(this, pUserData);
	}
}

MFVector MenuItemInt::GetDimensions(float maxWidth)
{
	return MakeVector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}

// MenuItemFloat
float MenuItemFloat::ListDraw(bool selected, const MFVector &pos, float maxWidth)
{
	MFFont_DrawText(MFFont_GetDebugFont(), pos, MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s: %.2f", name, *pData));
	return MENU_FONT_HEIGHT;
}

void MenuItemFloat::ListUpdate(bool selected)
{
	if(selected)
	{
		float t = *pData;
		float input;

		if(MFInput_WasPressed(Button_XB_B, IDD_Gamepad)) *pData = defaultValue;
		if(MFInput_WasPressed(Button_XB_X, IDD_Gamepad)) *pData = 0.0f;

		if(MFInput_WasPressed(Button_DLeft, IDD_Gamepad))
		{
			*pData -= increment;
		}
		else if(MFInput_WasPressed(Button_DRight, IDD_Gamepad))
		{
			*pData += increment;
		}

		if((input=MFInput_Read(Axis_RX, IDD_Gamepad)))
		{
			input = input < 0.0f ? -(input*input) : input*input;
			*pData += input*increment*MFSystem_TimeDelta();
		}

		if(*pData < minimumValue) *pData = maximumValue+(*pData-minimumValue);
		if(*pData > maximumValue) *pData = minimumValue+(*pData-maximumValue);

		if(pCallback && t != *pData)
			pCallback(this, pUserData);
	}
}

MFVector MenuItemFloat::GetDimensions(float maxWidth)
{
	return MakeVector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}

// MenuItemBool
float MenuItemBool::ListDraw(bool selected, const MFVector &pos, float maxWidth)
{
	MFFont_DrawText(MFFont_GetDebugFont(), pos, MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s: %s", name, data ? "true" : "false"));
	return MENU_FONT_HEIGHT;
}

void MenuItemBool::ListUpdate(bool selected)
{
	if(selected)
	{
		if(MFInput_WasPressed(Button_DLeft, IDD_Gamepad) || MFInput_WasPressed(Button_DRight, IDD_Gamepad) || MFInput_WasPressed(Button_XB_A, IDD_Gamepad))
		{
			data = !data;

			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

MFVector MenuItemBool::GetDimensions(float maxWidth)
{
	return MakeVector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}

// MenuItemIntString
float MenuItemIntString::ListDraw(bool selected, const MFVector &pos, float maxWidth)
{
	MFFont_DrawText(MFFont_GetDebugFont(), pos, MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s: %s", name, ppValues[data]));
	return MENU_FONT_HEIGHT;
}

void MenuItemIntString::ListUpdate(bool selected)
{
	if(selected)
	{
		if(MFInput_WasPressed(Button_DLeft, IDD_Gamepad))
		{
			--data;

			if(data<0)
				while(ppValues[data+1]) data++;

			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(MFInput_WasPressed(Button_DRight, IDD_Gamepad))
		{
			++data;

			if(!ppValues[data])
				data = 0;

			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(MFInput_WasPressed(Button_P2_Cross, IDD_Gamepad))
		{
			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

MFVector MenuItemIntString::GetDimensions(float maxWidth)
{
	return MakeVector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}


// MenuItemColour

void MenuItemColour::Draw()
{

}

void MenuItemColour::Update()
{
	if(MFInput_WasPressed(Button_XB_Y, IDD_Gamepad)) pCurrentMenu = pParent;
}

float MenuItemColour::ListDraw(bool selected, const MFVector &_pos, float maxWidth)
{
	MFVector pos = _pos;

	MFFont_DrawText(MFFont_GetDebugFont(), pos+MakeVector(0.0f, MENU_FONT_HEIGHT*0.25f, 0.0f), MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s: 0x%08X", name, pData->ToPackedColour()));

	pos += MakeVector(maxWidth - 55.0f, 2.0f, 0.0f);

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(4);
	MFSetColour(0xFFFFFFFF);
	MFSetPosition(pos);
	MFSetPosition(pos + MakeVector(45.0f, 0.0f, 0.0f));
	MFSetPosition(pos + MakeVector(0.0f, MENU_FONT_HEIGHT*1.5f-4.0f, 0.0f));
	MFSetPosition(pos + MakeVector(45.0f, MENU_FONT_HEIGHT*1.5f-4.0f, 0.0f));
	MFEnd();

	pos += MakeVector(2.0f, 2.0f, 0.0f);

	MFBegin(4);
	MFSetColour(*pData);
	MFSetPosition(pos);
	MFSetPosition(pos + MakeVector(41.0f, 0.0f, 0.0f));
	MFSetPosition(pos + MakeVector(0.0f, MENU_FONT_HEIGHT*1.5f-8.0f, 0.0f));
	MFSetPosition(pos + MakeVector(41.0f, MENU_FONT_HEIGHT*1.5f-8.0f, 0.0f));
	MFEnd();

	return MENU_FONT_HEIGHT*1.5f;
}

void MenuItemColour::ListUpdate(bool selected)
{
	if(selected)
	{
		if(MFInput_WasPressed(Button_XB_A, IDD_Gamepad))
			pCurrentMenu = this;

		if(MFInput_WasPressed(Button_DLeft, IDD_Gamepad))
		{
			preset = preset <= 0 ? COLOUR_PRESETS-1 : preset-1;

			pData->FromPackedColour(presets[preset]);

			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(MFInput_WasPressed(Button_DRight, IDD_Gamepad))
		{
			preset = preset >= COLOUR_PRESETS-1 ? 0 : preset+1;

			pData->FromPackedColour(presets[preset]);

			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

MFVector MenuItemColour::GetDimensions(float maxWidth)
{
	return MakeVector(maxWidth, MENU_FONT_HEIGHT*1.5f, 0.0f);
}

// MenuItemPosition2D
float MenuItemPosition2D::ListDraw(bool selected, const MFVector &pos, float maxWidth)
{
	MFFont_DrawText(MFFont_GetDebugFont(), pos, MENU_FONT_HEIGHT, selected ? MakeVector(1,1,0,1) : MFVector::one, MFStr("%s: %.2f, %.2f", name, pData->x, pData->y));
	return MENU_FONT_HEIGHT;
}

void MenuItemPosition2D::ListUpdate(bool selected)
{
	if(selected)
	{
		MFVector t = *pData;
		float input;

		if(MFInput_WasPressed(Button_XB_B, IDD_Gamepad)) *pData = defaultValue;
		if(MFInput_WasPressed(Button_XB_X, IDD_Gamepad)) *pData = MakeVector(0.0f, 0.0f, 0.0f);

		if((input=MFInput_Read(Axis_RX, IDD_Gamepad)))
		{
			input = input < 0.0f ? -(input*input) : input*input;
			pData->x += input*increment*MFSystem_TimeDelta();
		}

		if((input=MFInput_Read(Axis_RY, IDD_Gamepad)))
		{
			input = input < 0.0f ? -(input*input) : input*input;
			pData->y -= input*increment*MFSystem_TimeDelta();
		}

		if(pCallback && t != *pData)
			pCallback(this, pUserData);
	}
}

MFVector MenuItemPosition2D::GetDimensions(float maxWidth)
{
	return MakeVector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}
