#if !defined(_DEBUGMENU_H)
#define _DEBUGMENU_H

#include "Vector4.h"

#define MENU_MAX_MENUS 32
#define MENU_MAX_CHILDREN 32

#define MENU_FONT_HEIGHT 20.0f

class MenuObject;

// (menu_item*, user_data*)
typedef void (*DebugCallback)(MenuObject*, void*);

enum MenuType
{
	MenuType_Menu,
	MenuType_Static,
	MenuType_Int,
	MenuType_Float,
	MenuType_IntString,
	MenuType_Bool,
	MenuType_Colour,
	MenuType_TextureBrowser
};

class Menu;

class MenuObject
{
public:
	// when selected as current menu
	virtual void Draw();
	virtual void Update();

	// when being draw'n in a menu's list
	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth) = 0;
	virtual void ListUpdate(bool selected) = 0;
	virtual Vector3 GetDimensions(float maxWidth) = 0;

	char name[64];
	uint32 type;
	Menu *pParent;
	int menuDepth;

	void *pUserData;
	DebugCallback pCallback;
};

class MenuItemStatic : public MenuObject
{
public:
	MenuItemStatic() { type = MenuType_Static; }

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);
};

class MenuItemInt : public MenuObject
{
public:
	MenuItemInt(int value = 0, int inc = 1) { type = MenuType_Int; data = value; increment = inc; }
	operator int() const { return data; }

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	int data;
	int increment;
};

class MenuItemFloat : public MenuObject
{
public:
	MenuItemFloat(float value = 0.0f, float inc = 1.0f) { type = MenuType_Float; data = value; increment = inc; }
	operator float() const { return data; }

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	float data;
	float increment;
};

class MenuItemIntString : public MenuObject
{
public:
	MenuItemIntString(char *strings[], int value = 0) { type = MenuType_IntString; data = value; values = strings; DBGASSERT(values[0] != NULL, "Must be at least one item in the strings array."); }
	operator int() const { return data; }

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	int data;
	char **values;
};

class MenuItemBool : public MenuObject
{
public:
	MenuItemBool(bool value = false) { type = MenuType_Bool; data = value; }
	operator bool() const { return data; }

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	bool data;
};

static Vector4 colourInit = {1,1,1,1};

class MenuItemColour : public MenuObject
{
public:
	MenuItemColour(Vector4 def = colourInit) { type = MenuType_Colour; colour = def; preset = 0; }
	operator Vector4() const { return colour; }
	operator uint32() const { return (uint32)(colour.w*255.0f)<<24 | (uint32)(colour.x*255.0f)<<16 | (uint32)(colour.y*255.0f)<<8 | (uint32)(colour.z*255.0f); }

	virtual void Draw();
	virtual void Update();

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	Vector4 colour;

	int preset;
	static uint32 presets[10];
};

#define MENU_X		100.0f
#define MENU_Y		100.0f
#define MENU_WIDTH	640.0f-MENU_X*2.0f
#define MENU_HEIGHT	480.0f-MENU_Y*2.0f

class Menu : public MenuObject
{
public:
	Menu() { targetOffset = yOffset = 0.0f; menuX = MENU_X; menuY = MENU_Y; menuWidth = MENU_WIDTH; menuHeight = MENU_HEIGHT; }

	virtual int GetSelected();
	virtual int GetItemCount();

	virtual void Draw();
	virtual void Update();

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	MenuObject *pChildren[MENU_MAX_CHILDREN];
	int numChildren;
	int selection;

	float menuX, menuY, menuWidth, menuHeight;
	float yOffset, targetOffset;
};

extern Menu rootMenu;
extern MenuObject *pCurrentMenu;

void DebugMenu_InitModule();
void DebugMenu_DeinitModule();

bool DebugMenu_IsEnabled();

void DebugMenu_Update();
void DebugMenu_Draw();

void DebugMenu_AddItem(const char *name, Menu *parent, MenuObject *pObject, DebugCallback callback = NULL, void *userData = NULL);
void DebugMenu_AddItem(const char *name, const char *pParentName, MenuObject *pObject, DebugCallback callback = NULL, void *userData = NULL);
void DebugMenu_AddMenu(const char *name, Menu *parent, DebugCallback callback = NULL, void *userData = NULL);
void DebugMenu_AddMenu(const char *name, const char *pParentName, DebugCallback callback = NULL, void *userData = NULL);
bool DebugMenu_DestroyMenu(const char *name, Menu *pSearchMenu = &rootMenu);
void DebugMenu_DestroyMenuTree(Menu *pMenu);

Menu* DebugMenu_GetMenuByName(const char *name, Menu *pSearchMenu = &rootMenu);

#endif
