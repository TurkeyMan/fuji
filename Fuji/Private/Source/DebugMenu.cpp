#include "Common.h"
#include "Display.h"
#include "DebugMenu.h"
#include "Input.h"
#include "Font.h"
#include "Primitive.h"

bool debugMenuEnabled;
bool buttonsDown;

Menu rootMenu;
MenuObject *pCurrentMenu = &rootMenu;
Vector3 debugMenuPos = { 50.0f, 50.0f, 0.0f };

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
	strcpy(rootMenu.name, "root");
	rootMenu.menuDepth = 0;
	rootMenu.numChildren = 0;
	rootMenu.pCallback = NULL;
	rootMenu.pParent = NULL;
	rootMenu.pUserData = NULL;
	rootMenu.selection = 0;
}

void DebugMenu_DeinitModule()
{
	// destroy all menu's
	DebugMenu_DestroyMenuTree(&rootMenu);
}

bool DebugMenu_IsEnabled()
{
	return debugMenuEnabled;
}

void DebugMenu_Update()
{
	if(!buttonsDown && Input_ReadGamepad(0, Button_LThumb) && Input_ReadGamepad(0, Button_RThumb))
	{
		debugMenuEnabled = !debugMenuEnabled;
		buttonsDown = true;
	}

	if(buttonsDown && (!Input_ReadGamepad(0, Button_LThumb) || !Input_ReadGamepad(0, Button_RThumb)))
	{
		buttonsDown = false;
	}

	if(debugMenuEnabled)
		pCurrentMenu->Update();
}

void DebugMenu_Draw()
{
	if(Input_ReadGamepad(0, Button_RTrig)) return;

	bool o = SetOrtho(true);

	if(debugMenuEnabled)
		pCurrentMenu->Draw();

	SetOrtho(o);
}

void DebugMenu_AddItem(const char *name, Menu *pParent, MenuObject *pObject, DebugCallback callback, void *userData)
{
	DBGASSERT(pParent, "Invalid parent menu.");
	DBGASSERT(pParent->type == MenuType_Menu, STR("Cant add menu '%s', Parent is not of Menu type.", name));
	DBGASSERT(strlen(name) < 64, "Max of 64 characters in Menu Name.");
	DBGASSERT(pParent->numChildren < MENU_MAX_CHILDREN, STR("Maximum number of items in menu: '&s'", pParent->name)); 

	strcpy(pObject->name, name);

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
	DBGASSERT(pParent, "Invalid parent menu.");
	DBGASSERT(pParent->type == MenuType_Menu, STR("Cant add menu '%s', Parent is not of Menu type.", name));
	DBGASSERT(strlen(name) < 64, "Max of 64 characters in Menu Name.");
	DBGASSERT(pParent->numChildren < MENU_MAX_CHILDREN, STR("Maximum number of items in menu: '&s'", pParent->name)); 

	Menu *pMenu = new Menu;

	strcpy(pMenu->name, name);
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
			if(!stricmp(pSearchMenu->pChildren[a]->name, pName))
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
	for(int a=0; a<pMenu->numChildren; a++)
	{
		if(pMenu->pChildren[a]->type == MenuType_Menu)
		{
			DebugMenu_DestroyMenuTree((Menu*)pMenu->pChildren[a]);
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
			if(!stricmp(pSearchMenu->pChildren[a]->name , name))
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
	if(Input_WasPressed(0, Button_Y))
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

float Menu::ListDraw(bool selected, Vector3 pos, float maxWidth)
{
	debugFont.DrawText(pos, MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFF80A0FF, name);
	return MENU_FONT_HEIGHT;
}

void Menu::ListUpdate(bool selected)
{
	if(selected)
	{
		if(Input_WasPressed(0, Button_A))
			pCurrentMenu = this;
	}
}

void Menu::Draw()
{
	Vector3 dimensions = { 0.0f, 0.0f, 0.0f };
	Vector3 currentPos;
	float requestedWidth = menuWidth-40.0f;
	float selStart, selEnd;
	int a;

	// get menu size
	for(a=0; a<numChildren; a++)
	{
		Vector3 dim = pChildren[a]->GetDimensions(requestedWidth);

		if(selection==a)
		{
			selStart = dimensions.y;
			selEnd = selStart + dim.y;

			if(selStart < -yOffset)
			{
				targetOffset = -selStart;
			}

			if(selEnd > menuHeight - 75.0f - yOffset)
			{
				targetOffset = -(selEnd-(menuHeight-75.0f));
			}
		}

		dimensions.y += dim.y;
		dimensions.x = max(dimensions.x, dim.x);
	}

	if(targetOffset != yOffset)
	{
		yOffset -= abs(yOffset-targetOffset) < 0.1f ? yOffset-targetOffset : (yOffset-targetOffset)*0.1f;
	}

	currentPos = Vector(menuX+20.0f, menuY+50.0f + yOffset, 0.0f);

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(4);
	MFSetColour(0x80000060);
	MFSetPosition(menuX, menuY, 0);
	MFSetColour(0x800000B0);
	MFSetPosition(menuX+menuWidth, menuY, 0);
	MFSetColour(0x80000080);
	MFSetPosition(menuX, menuY+menuHeight, 0);
	MFSetColour(0x800000FF);
	MFSetPosition(menuX+menuWidth, menuY+menuHeight, 0);
	MFEnd();

	debugFont.DrawText(menuX+10.0f, menuY+5.0f, MENU_FONT_HEIGHT*1.5f, 0xFFFFB080, name);

	pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);

	MFPrimitive(PT_TriStrip|PT_Untextured);
	MFBegin(4);
	MFSetColour(0xA0000000);
	MFSetPosition(menuX+15, menuY+45, 0);
	MFSetColour(0xA0000000);
	MFSetPosition(menuX+menuWidth-15, menuY+45, 0);
	MFSetColour(0xA0000000);
	MFSetPosition(menuX+15, menuY+menuHeight-15, 0);
	MFSetColour(0xA0000000);
	MFSetPosition(menuX+menuWidth-15, menuY+menuHeight-15, 0);
	MFEnd();

	pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESS);
	pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0);

	for(a=0; a<numChildren; a++)
	{
		if(currentPos.y > menuY + menuHeight - 15.0f) break;

		if(selection==a)
		{
			float height = pChildren[a]->GetDimensions(requestedWidth).y;

			if(currentPos.y + height < menuY + 45.0f)
			{
				currentPos.y += height;
				continue;
			}

			MFPrimitive(PT_TriStrip|PT_Untextured);

			MFBegin(4);
			MFSetColour(0xC0000080);
			MFSetPosition(menuX+15, currentPos.y, 0);
			MFSetColour(0xC00000D0);
			MFSetPosition(menuX+menuWidth-15, currentPos.y, 0);
			MFSetColour(0xC0000090);
			MFSetPosition(menuX+15, currentPos.y + height, 0);
			MFSetColour(0xC00000FF);
			MFSetPosition(menuX+menuWidth-15, currentPos.y + height, 0);
			MFEnd();
		}

		currentPos.y += pChildren[a]->ListDraw(selection==a, currentPos, requestedWidth);
	}

	pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
}

void Menu::Update()
{
	// test controls and move cursor
	if(Input_WasPressed(0, Button_DUp))
		selection = selection > 0 ? selection-1 : numChildren-1;

	if(Input_WasPressed(0, Button_DDown))
		selection = selection < numChildren-1 ? selection+1 : 0;

	if(Input_WasPressed(0, Button_Y) && pParent)
		pCurrentMenu = pParent;

	for(int a=0; a<numChildren; a++)
	{
		pChildren[a]->ListUpdate(a == selection);
	}
}

Vector3 Menu::GetDimensions(float maxWidth)
{
	return Vector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}

// MenuItemStatic
float MenuItemStatic::ListDraw(bool selected, Vector3 pos, float maxWidth)
{
	debugFont.DrawText(pos, MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, name);
	return MENU_FONT_HEIGHT;
}

void MenuItemStatic::ListUpdate(bool selected)
{
	if(selected)
		if(pCallback && Input_WasPressed(0, Button_A))
			pCallback(this, pUserData);
}

Vector3 MenuItemStatic::GetDimensions(float maxWidth)
{
	return Vector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}

// MenuItemInt
float MenuItemInt::ListDraw(bool selected, Vector3 pos, float maxWidth)
{
	debugFont.DrawText(pos, MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, STR("%s: %d", name, data));
	return MENU_FONT_HEIGHT;
}

void MenuItemInt::ListUpdate(bool selected)
{
	if(selected)
	{
		if(Input_WasPressed(0, Button_B)) data = 0;

		if(Input_WasPressed(0, Button_DLeft))
		{
			data -= increment;
			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(Input_WasPressed(0, Button_DRight))
		{
			data += increment;
			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

Vector3 MenuItemInt::GetDimensions(float maxWidth)
{
	return Vector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}

// MenuItemFloat
float MenuItemFloat::ListDraw(bool selected, Vector3 pos, float maxWidth)
{
	debugFont.DrawText(pos, MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, STR("%s: %.2f", name, data));
	return MENU_FONT_HEIGHT;
}

void MenuItemFloat::ListUpdate(bool selected)
{
	if(selected)
	{
		float t = data;
		float input;

		if(Input_WasPressed(0, Button_B)) data = 0.0f;

		if(Input_WasPressed(0, Button_DLeft))
		{
			data -= increment;
		}
		else if(Input_WasPressed(0, Button_DRight))
		{
			data += increment;
		}

		if((input=Input_ReadGamepad(0, Axis_RX)))
		{
			input = input < 0.0f ? -(input*input) : input*input;
			data += input*increment*TIMEDELTA;
		}

		if(pCallback && t != data)
			pCallback(this, pUserData);
	}
}

Vector3 MenuItemFloat::GetDimensions(float maxWidth)
{
	return Vector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}

// MenuItemBool
float MenuItemBool::ListDraw(bool selected, Vector3 pos, float maxWidth)
{
	debugFont.DrawText(pos, MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, STR("%s: %s", name, data ? "true" : "false"));
	return MENU_FONT_HEIGHT;
}

void MenuItemBool::ListUpdate(bool selected)
{
	if(selected)
	{
		if(Input_WasPressed(0, Button_DLeft) || Input_WasPressed(0, Button_DRight) || Input_WasPressed(0, Button_A))
		{
			data = !data;

			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

Vector3 MenuItemBool::GetDimensions(float maxWidth)
{
	return Vector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}

// MenuItemIntString
float MenuItemIntString::ListDraw(bool selected, Vector3 pos, float maxWidth)
{
	debugFont.DrawText(pos, MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, STR("%s: %s", name, values[data]));
	return MENU_FONT_HEIGHT;
}

void MenuItemIntString::ListUpdate(bool selected)
{
	if(selected)
	{
		if(Input_WasPressed(0, Button_DLeft))
		{
			--data;

			if(data<0)
				while(values[data+1]) data++;

			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(Input_WasPressed(0, Button_DRight))
		{
			++data;

			if(!values[data])
				data = 0;

			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

Vector3 MenuItemIntString::GetDimensions(float maxWidth)
{
	return Vector(maxWidth, MENU_FONT_HEIGHT, 0.0f);
}


// MenuItemColour

void MenuItemColour::Draw()
{

}

void MenuItemColour::Update()
{
	if(Input_WasPressed(0, Button_Y)) pCurrentMenu = pParent;
}

float MenuItemColour::ListDraw(bool selected, Vector3 pos, float maxWidth)
{
	debugFont.DrawText(pos+Vector(0.0f, MENU_FONT_HEIGHT*0.25f, 0.0f), MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, STR("%s: 0x%08X", name, colour.ToColour()));

	pos += Vector(maxWidth - 55.0f, 2.0f, 0.0f);

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(4);
	MFSetColour(0xFFFFFFFF);
	MFSetPosition(pos);
	MFSetPosition(pos + Vector(45.0f, 0.0f, 0.0f));
	MFSetPosition(pos + Vector(0.0f, MENU_FONT_HEIGHT*1.5f-4.0f, 0.0f));
	MFSetPosition(pos + Vector(45.0f, MENU_FONT_HEIGHT*1.5f-4.0f, 0.0f));
	MFEnd();

	pos += Vector(2.0f, 2.0f, 0.0f);

	MFBegin(4);
	MFSetColour(colour);
	MFSetPosition(pos);
	MFSetPosition(pos + Vector(41.0f, 0.0f, 0.0f));
	MFSetPosition(pos + Vector(0.0f, MENU_FONT_HEIGHT*1.5f-8.0f, 0.0f));
	MFSetPosition(pos + Vector(41.0f, MENU_FONT_HEIGHT*1.5f-8.0f, 0.0f));
	MFEnd();

	return MENU_FONT_HEIGHT*1.5f;
}

void MenuItemColour::ListUpdate(bool selected)
{
	if(selected)
	{
		if(Input_WasPressed(0, Button_A))
			pCurrentMenu = this;

		if(Input_WasPressed(0, Button_DLeft))
		{
			preset = preset <= 0 ? COLOUR_PRESETS-1 : preset-1;

			colour.FromColour(presets[preset]);

			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(Input_WasPressed(0, Button_DRight))
		{
			preset = preset >= COLOUR_PRESETS-1 ? 0 : preset+1;

			colour.FromColour(presets[preset]);

			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

Vector3 MenuItemColour::GetDimensions(float maxWidth)
{
	return Vector(maxWidth, MENU_FONT_HEIGHT*1.5f, 0.0f);
}
