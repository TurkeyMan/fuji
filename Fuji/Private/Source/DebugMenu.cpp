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
	bool o = SetOrtho(true);

	if(debugMenuEnabled)
		pCurrentMenu->Draw();

	SetOrtho(o);
}

void DebugMenu_AddItem(char *name, Menu *pParent, MenuObject *pObject, DebugCallback callback, void *userData)
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

void DebugMenu_AddMenu(char *name, Menu *pParent, DebugCallback callback, void *userData)
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

bool DebugMenu_DestroyMenu(char *pName, Menu *pSearchMenu)
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

Menu* DebugMenu_GetMenuByName(char *name, Menu *pSearchMenu)
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
	debugFont.DrawText(pos, MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, name);
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
	Vector3 currentPos = Vector(120.0f, 150.0f, 0.0f);
	float requestedWidth = 400.0f;
	int a;

	// get menu size
	for(a=0; a<numChildren; a++)
	{
		Vector3 dim = pChildren[a]->GetDimensions(requestedWidth);
		dimensions.y += dim.y;
		dimensions.x = max(dimensions.x, dim.x);
	}

	BeginPrimitive(PT_TriStrip|PT_Untextured);

	PrimBegin(4);
	PrimSetColour(0x80000060);
	PrimSetPosition(100, 100, 0);
	PrimSetColour(0x800000B0);
	PrimSetPosition(540, 100, 0);
	PrimSetColour(0x80000080);
	PrimSetPosition(100, 380, 0);
	PrimSetColour(0x800000FF);
	PrimSetPosition(540, 380, 0);
	PrimEnd();

	PrimBegin(4);
	PrimSetColour(0x60000000);
	PrimSetPosition(115, 145, 0);
	PrimSetColour(0x60000000);
	PrimSetPosition(525, 145, 0);
	PrimSetColour(0x60000000);
	PrimSetPosition(115, 365, 0);
	PrimSetColour(0x60000000);
	PrimSetPosition(525, 365, 0);
	PrimEnd();

	debugFont.DrawText(110.0f, 105.0f, MENU_FONT_HEIGHT*1.5f, 0xFFFFB080, name);

	// draw menu background

	for(a=0; a<numChildren; a++)
	{
		if(selection==a)
		{
			float height = pChildren[a]->GetDimensions(requestedWidth).y;
			BeginPrimitive(PT_TriStrip|PT_Untextured);

			PrimBegin(4);
			PrimSetColour(0xC0000080);
			PrimSetPosition(115, currentPos.y, 0);
			PrimSetColour(0xC00000D0);
			PrimSetPosition(525, currentPos.y, 0);
			PrimSetColour(0xC0000090);
			PrimSetPosition(115, currentPos.y + height, 0);
			PrimSetColour(0xC00000FF);
			PrimSetPosition(525, currentPos.y + height, 0);
			PrimEnd();
		}

		currentPos.y += pChildren[a]->ListDraw(selection==a, currentPos, requestedWidth);
	}
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
