#include "Common.h"
#include "Texture.h"
#include "Display.h"
#include "Sprite.h"

void Sprite::Create(char *filename, int xFrame, int yFrames, uint32 colourKey)
{
	texture.LoadTexture(filename);
}

void Sprite::Draw()
{
	// if not visible dont draw anything and just return
	if(!visible) return;

	// set orthographic mode
	int old = SetOrtho(true);

	// vertex buffer for drawing the quad
	IDirect3DVertexBuffer8 *vb;
	FontVertex triangle[6];
	FontVertex *v;

	D3DXMATRIX world;

	// fill out the vertex data
	triangle[0].pos.Set((-pivot.x) * scale.x, (-pivot.y) * scale.y, 0);
	triangle[0].colour = 0xFFFFFFFF;
	triangle[0].u = 0; triangle[0].v = 0;
	triangle[1].pos.Set((1.0f-pivot.x) * scale.x, (-pivot.y) * scale.y, 0);
	triangle[1].colour = 0xFFFFFFFF;
	triangle[1].u = 1; triangle[1].v = 0;
	triangle[2].pos.Set((-pivot.x) * scale.x, (1.0f-pivot.y) * scale.y, 0);
	triangle[2].colour = 0xFFFFFFFF;
	triangle[2].u = 0; triangle[2].v = 1;

	triangle[3].pos.Set((1.0f-pivot.x) * scale.x, (-pivot.y) * scale.y, 0);
	triangle[3].colour = 0xFFFFFFFF;
	triangle[3].u = 1; triangle[3].v = 0;
	triangle[4].pos.Set((1.0f-pivot.x) * scale.x, (1.0f-pivot.y) * scale.y, 0);
	triangle[4].colour = 0xFFFFFFFF;
	triangle[4].u = 1; triangle[4].v = 1;
	triangle[5].pos.Set((-pivot.x) * scale.x, (1.0f-pivot.y) * scale.y, 0);
	triangle[5].colour = 0xFFFFFFFF;
	triangle[5].u = 0; triangle[5].v = 1;

	// create the vertex buffer and copy in the vertex data
	pd3dDevice->CreateVertexBuffer(6*sizeof(FontVertex), NULL, NULL, NULL, &vb);

	vb->Lock(0, 0, (BYTE**)&v, NULL);
	memcpy(v, &triangle[0], 6*sizeof(FontVertex));
	vb->Unlock();

	// generate rotation and translation matrix
	D3DXMatrixIdentity(&world);

	world._11 = (float)cos(angle);
	world._12 = (float)sin(angle);
	world._21 = (float)-sin(angle);
	world._22 = (float)cos(angle);
	world._41 = position.x;
	world._42 = position.y;

	// apply matrix
	pd3dDevice->SetTransform(D3DTS_WORLD, &world);

	// set texture
	texture.SetTexture();

	// draw sprite
	pd3dDevice->SetStreamSource(0, vb, sizeof(FontVertex));
	pd3dDevice->SetVertexShader(FontVertex::FVF);
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// release temporary vertex buffer
	vb->Release();
}

void Sprite::Release()
{
	texture.ReleaseTexture();
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


