#if !defined(_DEBUGMENU_H)
#define _DEBUGMENU_H

#include "Vector4.h"

#define MENU_MAX_MENUS 32
#define MENU_MAX_CHILDREN 32

#define MENU_FONT_HEIGHT 20.0f

typedef void (*DebugCallback)(void*, void*);

enum MenuType
{
	MenuType_Menu,
	MenuType_Int,
	MenuType_Float,
	MenuType_IntString,
	MenuType_Bool,
	MenuType_Colour
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

class MenuItemInt : public MenuObject
{
public:
	MenuItemInt(int value = 0, int inc = 1) { type = MenuType_Int; data = value; increment = inc; }
	operator int() const { return data; }

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void Update(bool selected);
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
	virtual void Update(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	float data;
	float increment;
};

class MenuItemIntString : public MenuObject
{
public:
	MenuItemIntString(char *strings[], int value = 0) { type = MenuType_IntString; data = value; values = strings; }
	operator int() const { return data; }

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void Update(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	int data;
	char **values;
};

class MenuItemBool : public MenuObject
{
public:
	MenuItemBool(bool value = true) { type = MenuType_Bool; data = value; }
	operator bool() const { return data; }

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void Update(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	bool data;
};

class MenuItemColour : public MenuObject
{
public:
	MenuItemColour(Vector4 def = Vector4::one) { type = MenuType_Colour; colour = def; }
	operator Vector4() const { return colour; }
	operator uint32() const { return (uint32)(colour.w*255.0f)<<24 | (uint32)(colour.x*255.0f)<<16 | (uint32)(colour.y*255.0f)<<8 | (uint32)(colour.z*255.0f); }

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void Update(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	Vector4 colour;
};

class Menu : public MenuObject
{
public:
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
};

extern Menu rootMenu;

void DebugMenu_InitModule();
void DebugMenu_DeinitModule();

bool DebugMenu_IsEnabled();

void DebugMenu_Update();
void DebugMenu_Draw();

void DebugMenu_AddItem(char *name, Menu *parent, MenuObject *pObject, DebugCallback callback = NULL, void *userData = NULL);
void DebugMenu_AddMenu(char *name, Menu *parent, DebugCallback callback = NULL, void *userData = NULL);
bool DebugMenu_DestroyMenu(char *name, Menu *pSearchMenu = &rootMenu);
void DebugMenu_DestroyMenuTree(Menu *pMenu);

Menu* DebugMenu_GetMenuByName(char *name, Menu *pSearchMenu = &rootMenu);

#endif
