#include "Fuji.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFDisplay.h"
#include "MFView.h"
#include "Sprite.h"
#include "MFPrimitive.h"

#include "Materials/MFMat_Standard.h"

void Sprite::Create(const char *pFilename, int xFrame, int yFrames, uint32 colourKey)
{
	pMaterial = MFMaterial_Create(pFilename);

	MFDebug_Assert(!MFString_Compare(pMaterial->pType->pTypeName, "Standard"), "Sprites MUST be created from a 'Standard' material.");

	MFTexture *pTex = MFMaterial_GetParameterT(pMaterial, MFMatStandard_Texture, MFMatStandard_Tex_DifuseMap);

	int texW, texH;
	MFTexture_GetTextureDimensions(pTex, &texW, &texH);

	pivot = MakeVector(0.0f, 0.0f);
	position = MakeVector(0.0f, 0.0f);
	scale = MakeVector((float)texW, (float)texH);
	angle = 0.0f;
	visible = false;
}

void Sprite::Draw()
{
	// if not visible dont draw anything and just return
	if(!visible) return;

	// generate rotation and translation matrix
	MFMatrix world;

	world.SetIdentity();

	world.m[0] = MFCos(angle);
	world.m[1] = MFSin(angle);
	world.m[4] = -MFSin(angle);
	world.m[5] = MFCos(angle);
	world.m[12] = position.x;
	world.m[13] = position.y;

	MFMaterial_SetMaterial(pMaterial);

	// set texture
	MFPrimitive(PT_TriStrip);

	MFSetMatrix(world);

	MFBegin(4);
	MFSetColourV(MFVector::white);
	MFSetTexCoord1(0,0);
	MFSetPosition((-pivot.x) * scale.x, (-pivot.y) * scale.y, 0);
	MFSetTexCoord1(1,0);
	MFSetPosition((1.0f-pivot.x) * scale.x, (-pivot.y) * scale.y, 0);
	MFSetTexCoord1(0,1);
	MFSetPosition((-pivot.x) * scale.x, (1.0f-pivot.y) * scale.y, 0);
	MFSetTexCoord1(1,1);
	MFSetPosition((1.0f-pivot.x) * scale.x, (1.0f-pivot.y) * scale.y, 0);
	MFEnd();
}

void Sprite::Release()
{
	MFMaterial_Release(pMaterial);
}

void Sprite::SetFlag(uint32 flag, bool enable)
{
	if(enable)
		flags |= flag;
	else
		flags &= ~flag;
}

void Sprite::Rotate(float _angle)
{
	angle = _angle;
}

float Sprite::GetRotation()
{
	return angle;
}

void Sprite::SetFrame(int _frame)
{
	frame = _frame;
}

void Sprite::Enable(bool enable)
{
	visible = enable;
}

void Sprite::Move(const MFVector &pos)
{
	position = pos;
}

MFVector Sprite::GetPos()
{
	MFVector t;

	t.x = position.x;
	t.y = position.y;
	t.z = 0.0f;

	return t;
}

void Sprite::Scale(const MFVector &_scale)
{
	scale = _scale;
}

MFVector Sprite::GetScale()
{
	MFVector t;

	t.x = scale.x;
	t.y = scale.y;
	t.z = 0.0f;

	return t;
}

void Sprite::SetPivot(const MFVector &_pivot)
{
	pivot = _pivot;
}

MFVector Sprite::GetPivot()
{
	MFVector t;

	t.x = pivot.x;
	t.y = pivot.y;
	t.z = 0.0f;

	return t;
}


