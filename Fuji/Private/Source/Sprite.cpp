#include "Common.h"
#include "Texture.h"
#include "Display.h"
#include "Sprite.h"
#include "Primitive.h"

void Sprite::Create(char *filename, int xFrame, int yFrames, uint32 colourKey)
{
	pTexture = Texture::LoadTexture(filename);

	pivot = Vector(0.0f, 0.0f);
	position = Vector(0.0f, 0.0f);
	scale = Vector((float)pTexture->width, (float)pTexture->height);
	angle = 0.0f;
	visible = false;
}

LitVertex triangle[6];

void Sprite::Draw()
{
	// if not visible dont draw anything and just return
	if(!visible) return;

	// set orthographic mode
	bool old = SetOrtho(true);

	// generate rotation and translation matrix
	Matrix world;

	world.SetIdentity();

	world.m[0][0] = (float)cos(angle);
	world.m[0][1] = (float)sin(angle);
	world.m[1][0] = (float)-sin(angle);
	world.m[1][1] = (float)cos(angle);
	world.m[3][0] = position.x;
	world.m[3][1] = position.y;

	pTexture->SetTexture();

	// set texture
	MFPrimitive(PT_TriStrip);

	MFSetMatrix(world);

	MFBegin(4);
	MFSetColour(0xFFFFFFFF);
	MFSetTexCoord1(0,0);
	MFSetPosition((-pivot.x) * scale.x, (-pivot.y) * scale.y, 0);
	MFSetTexCoord1(1,0);
	MFSetPosition((1.0f-pivot.x) * scale.x, (-pivot.y) * scale.y, 0);
	MFSetTexCoord1(0,1);
	MFSetPosition((-pivot.x) * scale.x, (1.0f-pivot.y) * scale.y, 0);
	MFSetTexCoord1(1,1);
	MFSetPosition((1.0f-pivot.x) * scale.x, (1.0f-pivot.y) * scale.y, 0);
	MFEnd();

	SetOrtho(old);
}

void Sprite::Release()
{
	pTexture->Release();
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

void Sprite::Move(const Vector3 &pos)
{
	position = pos;
}

Vector3 Sprite::GetPos()
{
	Vector3 t;

	t.x = position.x;
	t.y = position.y;
	t.z = 0.0f;

	return t;
}

void Sprite::Scale(const Vector3 &_scale)
{
	scale = _scale;
}

Vector3 Sprite::GetScale()
{
	Vector3 t;

	t.x = scale.x;
	t.y = scale.y;
	t.z = 0.0f;

	return t;
}

void Sprite::SetPivot(const Vector3 &_pivot)
{
	pivot = _pivot;
}

Vector3 Sprite::GetPivot()
{
	Vector3 t;

	t.x = pivot.x;
	t.y = pivot.y;
	t.z = 0.0f;

	return t;
}


