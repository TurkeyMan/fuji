#include "Common.h"
#include "Texture.h"
#include "Display.h"
#include "Input.h"
#include "Font.h"
#include "Primitive.h"

PtrList<Texture> gTextureBank;
TextureBrowser texBrowser;

void Texture_InitModule()
{
	gTextureBank.Init("Texture Bank", gDefaults.texture.maxTextures);

	DebugMenu_AddItem("Texture Browser", "Fuji Options", &texBrowser);
}

void Texture_DeinitModule()
{

}

Texture *FindTexture(const char *pName)
{
	Texture **ppIterator = gTextureBank.Begin();

	while(*ppIterator)
	{
		if(!StrCaseCmp(pName, (*ppIterator)->name)) return *ppIterator;

		ppIterator++;
	}

	return NULL;
}


// texture browser
TextureBrowser::TextureBrowser()
{
	selection = 0;
	type = MenuType_TextureBrowser;
}

void TextureBrowser::Draw()
{

}

void TextureBrowser::Update()
{
	if(Input_WasPressed(0, Button_XB_Y))
		pCurrentMenu = pParent;
}

#define TEX_SIZE 64.0f
float TextureBrowser::ListDraw(bool selected, const Vector3 &_pos, float maxWidth)
{
	Vector3 pos = _pos;

	Texture **i;
	i = gTextureBank.Begin();

	for(int a=0; a<selection; a++) i++;

	Texture *pTexture = *i;

	debugFont.DrawText(pos+Vector(0.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)-MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, STR("%s:", name));
	debugFont.DrawText(pos+Vector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f), 0.0f), MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, STR("%s", pTexture->name));
	debugFont.DrawText(pos+Vector(10.0f, ((TEX_SIZE+8.0f)*0.5f)-(MENU_FONT_HEIGHT*0.5f)+MENU_FONT_HEIGHT, 0.0f), MENU_FONT_HEIGHT, selected ? 0xFFFFFF00 : 0xFFFFFFFF, STR("Dimensions: %dx%d Refs: %d", pTexture->width, pTexture->height, pTexture->refCount));

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

	MFBegin(4);
	MFSetColour(0xFF000000);
	MFSetPosition(pos);
	MFSetPosition(pos + Vector(TEX_SIZE, 0.0f, 0.0f));
	MFSetPosition(pos + Vector(0.0f, TEX_SIZE, 0.0f));
	MFSetPosition(pos + Vector(TEX_SIZE, TEX_SIZE, 0.0f));
	MFEnd();

	float xaspect, yaspect;

	if(pTexture->width > pTexture->height)
	{
		xaspect = 0.5f;
		yaspect = ((float)pTexture->height/(float)pTexture->width) * 0.5f;
	}
	else
	{
		yaspect = 0.5f;
		xaspect = ((float)pTexture->width/(float)pTexture->height) * 0.5f;
	}

	pTexture->SetTexture();

	MFBegin(4);
	MFSetColour(0xFFFFFFFF);
	MFSetTexCoord1(0.0f,0.0f);
	MFSetPosition(pos + Vector(TEX_SIZE*0.5f - TEX_SIZE*xaspect, TEX_SIZE*0.5f - TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(1.0f,0.0f);
	MFSetPosition(pos + Vector(TEX_SIZE*0.5f + TEX_SIZE*xaspect, TEX_SIZE*0.5f - TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(0.0f,1.0f);
	MFSetPosition(pos + Vector(TEX_SIZE*0.5f - TEX_SIZE*xaspect, TEX_SIZE*0.5f + TEX_SIZE*yaspect, 0.0f));
	MFSetTexCoord1(1.0f,1.0f);
	MFSetPosition(pos + Vector(TEX_SIZE*0.5f + TEX_SIZE*xaspect, TEX_SIZE*0.5f + TEX_SIZE*yaspect, 0.0f));
	MFEnd();

	return TEX_SIZE + 8.0f;
}

void TextureBrowser::ListUpdate(bool selected)
{
	if(selected)
	{
		int texCount = gTextureBank.GetLength();

		if(Input_WasPressed(0, Button_DLeft))
		{
			selection = selection <= 0 ? texCount-1 : selection-1;

			if(pCallback)
				pCallback(this, pUserData);
		}
		else if(Input_WasPressed(0, Button_DRight))
		{
			selection = selection >= texCount-1 ? 0 : selection+1;

			if(pCallback)
				pCallback(this, pUserData);
		}
	}
}

Vector3 TextureBrowser::GetDimensions(float maxWidth)
{
	return Vector(maxWidth, TEX_SIZE + 8.0f, 0.0f);
}


