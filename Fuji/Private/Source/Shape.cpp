/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "Display.h"
#include "View.h"
#include "Primitive.h"
#include "Shape.h"

/**** Globals ****/

/**** Functions ****/

void Shape_DrawQuad(float x, float y, float x2, float y2, const Vector4& colour, float su, float sv, float du, float dv)
{
	CALLSTACK;

//	D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
//	D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	MFPrimitive(PT_TriStrip | PT_Prelit);
	MFBegin(4);

	MFSetColour(colour);

	MFSetTexCoord1(su, sv);
	MFSetPosition(x, y, 0);

	MFSetTexCoord1(du, sv);
	MFSetPosition(x2, y, 0);

	MFSetTexCoord1(su, dv);
	MFSetPosition(x, y2, 0);

	MFSetTexCoord1(du, dv);
	MFSetPosition(x2, y2, 0);

	MFEnd();

//	D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
//	D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void Shape_DrawQuad(const Vector3& pos, float w, float h, const Vector4& colour, float su, float sv, float du, float dv)
{
	CALLSTACK;

//	D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
//	D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	MFPrimitive(PT_TriStrip | PT_Prelit);
	MFBegin(4);

	MFSetColour(colour);

	MFSetTexCoord1(su, sv);
	MFSetPosition(pos.x, pos.y, pos.z);

	MFSetTexCoord1(du, sv);
	MFSetPosition(pos.x+w, pos.y, pos.z);

	MFSetTexCoord1(su, dv);
	MFSetPosition(pos.x, pos.y+h, pos.z);

	MFSetTexCoord1(du, dv);
	MFSetPosition(pos.x+w, pos.y+h, pos.z);

	MFEnd();

//	D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
//	D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void Shape_DrawUntexturedQuad(float x, float y, float x2, float y2, const Vector4& colour)
{
	CALLSTACK;

//	D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
//	D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	MFPrimitive(PT_TriStrip | PT_Prelit | PT_Untextured);
	MFBegin(4);

	MFSetColour(colour);
	MFSetPosition(x, y, 0);
	MFSetPosition(x2, y, 0);
	MFSetPosition(x, y2, 0);
	MFSetPosition(x2, y2, 0);

	MFEnd();

//	D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
//	D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void Shape_DrawUntexturedQuad(const Vector3& pos, float w, float h, const Vector4& colour)
{
	CALLSTACK;

//	D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
//	D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	MFPrimitive(PT_TriStrip | PT_Prelit | PT_Untextured);
	MFBegin(4);

	MFSetColour(colour);
	MFSetPosition(pos.x, pos.y, pos.z);
	MFSetPosition(pos.x+w, pos.y, pos.z);
	MFSetPosition(pos.x, pos.y+h, pos.z);
	MFSetPosition(pos.x+w, pos.y+h, pos.z);

	MFEnd();

//	D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
//	D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

// draw a box from a min and a max
void Shape_DrawBox(const Vector3 &boxMin, const Vector3 &boxMax, const Vector4& colour, const Matrix &mat, bool wireframe)
{
	CALLSTACK;

	if(wireframe)
	{
		MFPrimitive(PT_LineList|PT_Prelit|PT_Untextured);

		MFSetMatrix(mat);

		MFBegin(24);

		MFSetColour(colour);

		MFSetPosition(boxMin.x, boxMin.y, boxMin.z);
		MFSetPosition(boxMax.x, boxMin.y, boxMin.z);
		MFSetPosition(boxMin.x, boxMax.y, boxMin.z);
		MFSetPosition(boxMax.x, boxMax.y, boxMin.z);
		MFSetPosition(boxMin.x, boxMin.y, boxMax.z);
		MFSetPosition(boxMax.x, boxMin.y, boxMax.z);
		MFSetPosition(boxMin.x, boxMax.y, boxMax.z);
		MFSetPosition(boxMax.x, boxMax.y, boxMax.z);

		MFSetPosition(boxMin.x, boxMin.y, boxMin.z);
		MFSetPosition(boxMin.x, boxMax.y, boxMin.z);
		MFSetPosition(boxMax.x, boxMin.y, boxMin.z);
		MFSetPosition(boxMax.x, boxMax.y, boxMin.z);
		MFSetPosition(boxMin.x, boxMin.y, boxMax.z);
		MFSetPosition(boxMin.x, boxMax.y, boxMax.z);
		MFSetPosition(boxMax.x, boxMin.y, boxMax.z);
		MFSetPosition(boxMax.x, boxMax.y, boxMax.z);

		MFSetPosition(boxMin.x, boxMin.y, boxMin.z);
		MFSetPosition(boxMin.x, boxMin.y, boxMax.z);
		MFSetPosition(boxMax.x, boxMin.y, boxMin.z);
		MFSetPosition(boxMax.x, boxMin.y, boxMax.z);
		MFSetPosition(boxMin.x, boxMax.y, boxMin.z);
		MFSetPosition(boxMin.x, boxMax.y, boxMax.z);
		MFSetPosition(boxMax.x, boxMax.y, boxMin.z);
		MFSetPosition(boxMax.x, boxMax.y, boxMax.z);

		MFEnd();

		Vector3 center = boxMin + ((boxMax-boxMin)*0.5f);

		// draw the axii's
		MFBegin(2);
		MFSetColour(0xFF00FF00);
		MFSetPosition(center.x, center.y + 10.0f, center.z);
		MFSetPosition(center.x, center.y, center.z);
		MFEnd();
		MFBegin(2);
		MFSetColour(0xFFFF0000);
		MFSetPosition(center.x, center.y, center.z);
		MFSetPosition(center.x + 10.0f, center.y, center.z);
		MFEnd();
		MFBegin(2);
		MFSetColour(0xFF0000FF);
		MFSetPosition(center.x, center.y, center.z);
		MFSetPosition(center.x, center.y, center.z + 10.0f);
		MFEnd();
	}
	else
	{
		MFPrimitive(PT_TriStrip|PT_Untextured);

		MFSetMatrix(mat);

		MFBegin(34);

		MFSetColour(colour);

		MFSetNormal(0.0f,0.0f,-1.0f);
		MFSetPosition(boxMin.x, boxMax.y, boxMin.z); // front
		MFSetPosition(boxMax.x, boxMax.y, boxMin.z);
		MFSetPosition(boxMin.x, boxMin.y, boxMin.z);
		MFSetPosition(boxMax.x, boxMin.y, boxMin.z);

		MFSetPosition(boxMax.x, boxMin.y, boxMin.z); // degenerates
		MFSetPosition(boxMin.x, boxMin.y, boxMin.z);

		MFSetNormal(0.0f,-1.0f,0.0f);
		MFSetPosition(boxMin.x, boxMin.y, boxMin.z); // bottom
		MFSetPosition(boxMax.x, boxMin.y, boxMin.z);
		MFSetPosition(boxMin.x, boxMin.y, boxMax.z);
		MFSetPosition(boxMax.x, boxMin.y, boxMax.z);

		MFSetPosition(boxMax.x, boxMin.y, boxMax.z); // degenerates
		MFSetPosition(boxMin.x, boxMin.y, boxMax.z);

		MFSetNormal(0.0f,0.0f,1.0f);
		MFSetPosition(boxMin.x, boxMin.y, boxMax.z); // back
		MFSetPosition(boxMax.x, boxMin.y, boxMax.z);
		MFSetPosition(boxMin.x, boxMax.y, boxMax.z);
		MFSetPosition(boxMax.x, boxMax.y, boxMax.z);

		MFSetPosition(boxMax.x, boxMax.y, boxMax.z); // degenerates
		MFSetPosition(boxMin.x, boxMax.y, boxMax.z);

		MFSetNormal(0.0f,1.0f,0.0f);
		MFSetPosition(boxMin.x, boxMax.y, boxMax.z); // top
		MFSetPosition(boxMax.x, boxMax.y, boxMax.z);
		MFSetPosition(boxMin.x, boxMax.y, boxMin.z);
		MFSetPosition(boxMax.x, boxMax.y, boxMin.z);

		MFSetPosition(boxMax.x, boxMax.y, boxMin.z); // degenerates
		MFSetPosition(boxMin.x, boxMax.y, boxMax.z);

		MFSetNormal(-1.0f,0.0f,0.0f);
		MFSetPosition(boxMin.x, boxMax.y, boxMax.z); // left
		MFSetPosition(boxMin.x, boxMax.y, boxMin.z);
		MFSetPosition(boxMin.x, boxMin.y, boxMax.z);
		MFSetPosition(boxMin.x, boxMin.y, boxMin.z);

		MFSetPosition(boxMin.x, boxMin.y, boxMin.z); // degenerates
		MFSetPosition(boxMax.x, boxMax.y, boxMin.z);

		MFSetNormal(1.0f,0.0f,0.0f);
		MFSetPosition(boxMax.x, boxMax.y, boxMin.z); // right
		MFSetPosition(boxMax.x, boxMax.y, boxMax.z);
		MFSetPosition(boxMax.x, boxMin.y, boxMin.z);
		MFSetPosition(boxMax.x, boxMin.y, boxMax.z);

		MFEnd();
	}
}

// draw's a sphere .. position.w defines position.w
void Shape_DrawSphere(const Vector3 &position, float radius, int segments, int slices, const Vector4& colour, const Matrix &mat, bool wireframe)
{
	CALLSTACK;

	DBGASSERT(segments >= 3, "DrawSphere requires at least 3 segments!");
	DBGASSERT(slices >= 1, "DrawSphere requires at least 1 slices!");

	int i, j, inc;
	float around = 0.0f, aroundInc = (PI*2.0f)/(float)segments;
	float yWave=0.0f, yWaveInc = PI/((float)slices+1.0f);
	float siny;

	MFPrimitive(PT_LineStrip|PT_Prelit|PT_Untextured);

	MFSetMatrix(mat);

	MFBegin(segments*(slices+1)+1);
	MFSetColour(colour);

	for(i=0, j=0, inc=1; j<segments;)
	{
		siny = MFSin(yWave);

		MFSetPosition(position.x + (MFSin(around)*radius) * siny, position.y + MFCos(yWave)*radius, position.z + (MFCos(around)*radius) * siny);

		yWave += yWaveInc;

		i+=inc;

		if(i==0 || i==slices+1)
		{
			inc = -inc;
			yWaveInc = -yWaveInc;

			j++;
			around += aroundInc;
		}
	}

	siny = MFSin(yWave);
	MFSetPosition(position.x + (MFSin(around)*radius) * siny, position.y + MFCos(yWave)*radius, position.z + (MFCos(around)*radius) * siny);

	MFEnd();

	yWaveInc = MFAbs(yWaveInc);
	yWave = yWaveInc;

	for(i=0; i<slices; i++)
	{
		float cosy = MFCos(yWave)*radius;
		siny = MFSin(yWave);
		around = 0.0f;

		MFBegin(segments+1);
		MFSetColour(colour);

		for(int j=0; j<segments+1; j++)
		{
			MFSetPosition(position.x + (MFSin(around)*radius)*siny, position.y + cosy, position.z + (MFCos(around)*radius)*siny);
			around += aroundInc;
		}

		MFEnd();

		yWave += yWaveInc;
	}

	// draw the axii's
	MFBegin(2);
	MFSetColour(0xFF00FF00);
	MFSetPosition(position.x, position.y + radius * 0.5f, position.z);
	MFSetPosition(position.x, position.y, position.z);
	MFEnd();
	MFBegin(2);
	MFSetColour(0xFFFF0000);
	MFSetPosition(position.x, position.y, position.z);
	MFSetPosition(position.x + radius * 0.5f, position.y, position.z);
	MFEnd();
	MFBegin(2);
	MFSetColour(0xFF0000FF);
	MFSetPosition(position.x, position.y, position.z);
	MFSetPosition(position.x, position.y, position.z + radius * 0.5f);
	MFEnd();
}

// draw's a capsule from a start and end point and a position.w
void Shape_DrawCapsule(const Vector3 &startPoint, const Vector3 &endPoint, float radius, int segments, int slices, const Vector4& colour, const Matrix &mat, bool wireframe)
{
	CALLSTACK;

	slices += 1 - (slices&0x1);

	DBGASSERT(segments >= 3, "DrawCapsule requires at least 3 segments!");
	DBGASSERT(slices >= 1, "DrawCapsule requires at least 1 slices!");

	Matrix m, m2 = mat;

	Vector3 t = endPoint-startPoint;
	float len = t.Magnitude();
	t /= len;

	// if capsule has no length .. might as well just draw a sphere ..
	if(len<0.1f)
	{
		Shape_DrawSphere(startPoint, radius, segments, slices, colour, mat, wireframe);
		return;
	}

	m.SetYAxis(t);
	m.SetZAxis(m.GetYAxis3().Cross(Vector(13.67f, 3.72f, 0.0f)).Normalise()); // cross product with a magic vector
	m.SetXAxis(m.GetZAxis3().Cross(m.GetYAxis3()).Normalise());
	m.SetTrans(startPoint);
	m.ClearW();

	m2.Multiply(m, m2);

	int i, j, inc;
	float around = 0.0f, aroundInc = (PI*2.0f)/(float)segments;
	float yWave=0.0f, yWaveInc = PI/((float)slices+1.0f);
	float yAdd = len;
	float siny;

	MFPrimitive(PT_LineStrip|PT_Prelit|PT_Untextured);

	MFSetMatrix(m2);

	MFBegin(segments*(slices+2)+1);
	MFSetColour(colour);

	for(i=0, j=0, inc=1; j<segments;)
	{
		siny = MFSin(yWave);

		MFSetPosition(MFSin(around)*radius * siny, MFCos(yWave)*radius + yAdd, MFCos(around)*radius * siny);

		if(i == (slices+1)/2)
		{
			yAdd -= len * (float)inc;
			MFSetPosition(MFSin(around)*radius * siny, MFCos(yWave)*radius + yAdd, MFCos(around)*radius * siny);
		}

		yWave += yWaveInc;

		i+=inc;

		if(i==0 || i==slices+1)
		{
			inc = -inc;
			yWaveInc = -yWaveInc;

			j++;
			around += aroundInc;
		}
	}

	siny = MFSin(yWave);
	MFSetPosition(MFSin(around)*radius * siny, MFCos(yWave)*radius + yAdd, MFCos(around)*radius * siny);

	MFEnd();

	yWaveInc = MFAbs(yWaveInc);
	yWave = yWaveInc;
	yAdd = len;

	for(i=0; i<slices; i++)
	{
		float cosy = MFCos(yWave)*radius;
		siny = MFSin(yWave);
		around = 0.0f;

		if(i == (slices+1)/2)
		{
			yAdd -= len;
		}

		MFBegin(segments+1);
		MFSetColour(colour);

		for(int j=0; j<segments+1; j++)
		{
			MFSetPosition(MFSin(around)*radius*siny, cosy + yAdd, MFCos(around)*radius*siny);
			around += aroundInc;
		}

		MFEnd();

		yWave += yWaveInc;
	}

	int centerSegs = (int)(len / ((radius*2)/(float)slices));
	yAdd = 0.0f;

	for(i=0; i<centerSegs; i++)
	{
		around = 0.0f;

		MFBegin(segments+1);
		MFSetColour(colour);

		for(int j=0; j<segments+1; j++)
		{
			MFSetPosition(MFSin(around)*radius, yAdd, MFCos(around)*radius);
			around += aroundInc;
		}

		MFEnd();

		yAdd+=len/(float)centerSegs;
	}

	m2=mat;
	m.SetIdentity();
	m.SetTrans(startPoint);
	m2.Multiply(m, m2);

	MFSetMatrix(m2);

	// draw the axii's
	MFBegin(2);
	MFSetColour(0xFF00FF00);
	MFSetPosition(0.0f, radius * 0.5f, 0.0f);
	MFSetPosition(0.0f, 0.0f, 0.0f);
	MFEnd();
	MFBegin(2);
	MFSetColour(0xFFFF0000);
	MFSetPosition(0.0f, 0.0f, 0.0f);
	MFSetPosition(radius * 0.5f, 0.0f, 0.0f);
	MFEnd();
	MFBegin(2);
	MFSetColour(0xFF0000FF);
	MFSetPosition(0.0f, 0.0f, 0.0f);
	MFSetPosition(0.0f, 0.0f, radius * 0.5f);
	MFEnd();
}

// draw's a cylinder from a position position.w and height
void Shape_DrawCylinder(const Vector3 &startPoint, const Vector3 &endPoint, float radius, int segments, int slices, const Vector4& colour, const Matrix &mat, bool wireframe)
{
	CALLSTACK;

	DBGASSERT(segments >= 3, "DrawCylinder requires at least 3 segments!");

	Matrix m, m2 = mat;

	Vector3 t = endPoint-startPoint;
	float len = t.Magnitude();
	t /= len;

	m.SetYAxis(t);
	m.SetZAxis(m.GetYAxis3().Cross(Vector(13.67f, 3.72f, 0.0f)).Normalise()); // cross product with a magic vector
	m.SetXAxis(m.GetZAxis3().Cross(m.GetYAxis3()).Normalise());
	m.SetTrans(startPoint);
	m.ClearW();

	m2.Multiply(m, m2);

	int i, j, inc;
	float aroundInc = (PI*2.0f)/(float)segments;
	float around = 0.0f;

	if(wireframe)
	{
		around = -aroundInc;

		MFPrimitive(PT_LineStrip|PT_Prelit|PT_Untextured);

		MFSetMatrix(m2);

		MFBegin(segments*3 + 1);
		MFSetColour(colour);

		for(i=0, j=-1, inc=-1; j<segments; i+=inc)
		{
			// tests weather number is zero or 3 ..
			if(!i || i==3)
			{
				MFSetPosition(0.0f, (i&2)?len:0.0f, 0.0f);

				inc = -inc;
				j++;
				around += aroundInc;
			}
			else
			{
				MFSetPosition(MFSin(around)*radius, (i&2)?len:0.0f, MFCos(around)*radius);
			}
		}

		MFEnd();

		slices+=2;
		float yOffset = 0.0f, yInc = len/(float)(slices-1);

		for(i=0; i<slices; i++)
		{
			around = 0.0f;

			MFBegin(segments+1);
			MFSetColour(colour);

			for(int j=0; j<segments+1; j++)
			{
				MFSetPosition(MFSin(around)*radius, yOffset, MFCos(around)*radius);
				around += aroundInc;
			}

			MFEnd();

			yOffset += yInc;
		}

		m2=mat;
		m.SetIdentity();
		m.SetTrans(startPoint);
		m2.Multiply(m, m2);

		MFSetMatrix(m2);

		// draw the axii's
		MFBegin(2);
		MFSetColour(0xFF00FF00);
		MFSetPosition(0.0f, radius * 0.5f, 0.0f);
		MFSetPosition(0.0f, 0.0f, 0.0f);
		MFEnd();
		MFBegin(2);
		MFSetColour(0xFFFF0000);
		MFSetPosition(0.0f, 0.0f, 0.0f);
		MFSetPosition(radius * 0.5f, 0.0f, 0.0f);
		MFEnd();
		MFBegin(2);
		MFSetColour(0xFF0000FF);
		MFSetPosition(0.0f, 0.0f, 0.0f);
		MFSetPosition(0.0f, 0.0f, radius * 0.5f);
		MFEnd();
	}
	else
	{
		MFPrimitive(PT_TriStrip|PT_Prelit|PT_Untextured);
		MFSetMatrix(m2);

		// bottom cap
		MFBegin((segments+1)*2);
		MFSetColour(colour);
		MFSetNormal(0.0f,1.0f,0.0f);
		MFSetPosition(0.0f, len, 0.0f);
		for(i=0; i<segments; i++)
		{
			MFSetPosition(MFSin(around)*radius, len, MFCos(around)*radius);
			MFSetPosition(0.0f, len, 0.0f);
			around += aroundInc;
		}
		MFSetPosition(0.0f, len, radius);
		MFEnd();

		// top cap
		around = PI*2;

		MFBegin((segments+1)*2);
		MFSetColour(colour);
		MFSetNormal(0.0f,-1.0f,0.0f);
		MFSetPosition(0.0f, 0.0f, 0.0f);
		for(i=0; i<segments; i++)
		{
			MFSetPosition(MFSin(around)*radius, 0.0f, MFCos(around)*radius);
			MFSetPosition(0.0f, 0.0f, 0.0f);
			around -= aroundInc;
		}
		MFSetPosition(0.0f, 0.0f, radius);
		MFEnd();

		// surface
		MFPrimitive(PT_TriStrip|PT_Prelit|PT_Untextured);
		MFSetMatrix(m2);
		MFBegin((segments+1)*2);
		MFSetColour(colour);
		around = 0.0f;
		for(i=0; i<segments; i++)
		{
			float s = MFSin(around), c=MFCos(around);
			MFSetNormal(s,0.0f,c);
			MFSetPosition(s*radius, len, c*radius);
			MFSetPosition(s*radius, 0.0f, c*radius);
			around += aroundInc;
		}
		MFSetPosition(0.0f, len, radius);
		MFSetPosition(0.0f, 0.0f, radius);
		MFEnd();
	}
}

// draw's a plane from a position normal and span
void Shape_DrawPlane(const Vector3 &point, const Vector3 &normal, float span, const Vector4& colour, const Matrix &mat, bool wireframe)
{
	CALLSTACK;

	int segments = 12;

	Matrix m, m2 = mat;

	m.SetYAxis(normal);
	m.SetZAxis(m.GetYAxis3().Cross(Vector(13.67f, 3.72f, 0.0f)).Normalise()); // cross product with a magic vector
	m.SetXAxis(m.GetZAxis3().Cross(m.GetYAxis3()).Normalise());
	m.SetTrans(point);
	m.ClearW();

	m2.Multiply(m, m2);

	int i;

	float aroundInc = (PI*2.0f)/(float)segments;
	float around = -aroundInc;

	MFPrimitive(PT_LineList|PT_Prelit|PT_Untextured);
	MFSetMatrix(m2);

	MFBegin(segments*2+6);
	MFSetColour(colour);

	for(i=0; i<segments; i++)
	{
		MFSetPosition(0.0f, 0.0f, 0.0f);
		MFSetPosition(MFSin(around)*span, 0.0f, MFCos(around)*span);
		around += aroundInc;
	}

	float normalLen = span*0.25f;

	MFSetColour(Vector(1,1,0,colour.w));
	MFSetPosition(0.0f, 0.0f, 0.0f);
	MFSetPosition(0.0f, normalLen, 0.0f);
	MFSetPosition(0.0f, normalLen, 0.0f);
	MFSetPosition(-normalLen*0.15f, normalLen - normalLen*0.25f, 0.0f);
	MFSetPosition(0.0f, normalLen, 0.0f);
	MFSetPosition(normalLen*0.15f, normalLen - normalLen*0.25f, 0.0f);

	MFEnd();

	MFPrimitive(PT_LineStrip|PT_Prelit|PT_Untextured);
	MFSetMatrix(m2);

	around = 0.0f;

	MFBegin(segments+1);
	MFSetColour(colour);

	for(int j=0; j<segments+1; j++)
	{
		MFSetPosition(MFSin(around)*span, 0.0f, MFCos(around)*span);
		around += aroundInc;
	}

	MFEnd();
}

void Shape_DrawCone(const Vector3 &base, const Vector3 &point, float radius, int segments, const Vector4& colour, const Matrix &mat, bool wireframe)
{
	CALLSTACK;

	DBGASSERT(segments >= 3, "DrawCylinder requires at least 3 segments!");

	Matrix m, m2 = mat;

	Vector3 t = point-base;
	float height = t.Magnitude();
	t /= height;

	m.SetYAxis(t);
	m.SetZAxis(m.GetYAxis3().Cross(Vector(13.67f, 3.72f, 0.0f)).Normalise()); // cross product with a magic vector
	m.SetXAxis(m.GetZAxis3().Cross(m.GetYAxis3()).Normalise());
	m.SetTrans(base);
	m.ClearW();

	m2.Multiply(m, m2);

	int i;
	float aroundInc = (PI*2.0f)/(float)segments;
	float around = 0.0f;

	if(wireframe)
		MFPrimitive(PT_LineList|PT_Prelit|PT_Untextured);
	else
		MFPrimitive(PT_TriStrip|PT_Prelit|PT_Untextured);

	MFSetMatrix(m2);

	MFBegin(wireframe?segments*4:(segments+1)*2);

	MFSetColour(colour);

	if(!wireframe)
	{
		MFSetNormal(0.0f, 1.0f, 0.0f);
		MFSetPosition(0.0f, height, 0.0f);
	}

	for(i=0; i<segments; i++)
	{
		float s=MFSin(around), c=MFCos(around);
		if(wireframe)
		{
			if(i) MFSetPosition(s*radius, 0.0f, c*radius);
			MFSetPosition(0.0f, height, 0.0f);
			MFSetPosition(s*radius, 0.0f, c*radius);
			MFSetPosition(s*radius, 0.0f, c*radius);
		}
		else
		{
			MFSetPosition(s*radius, 0.0f, c*radius);
			MFSetPosition(0.0f, height, 0.0f);
		}

		around += aroundInc;
	}

	MFSetPosition(0.0f, 0.0f, radius);

	MFEnd();

	// draw base
	MFBegin(wireframe?segments*2:(segments+1)*2);

	MFSetColour(colour);

	around = PI*2.0f;

	if(!wireframe)
	{
		MFSetPosition(0.0f, 0.0f, 0.0f);
	}

	for(i=0; i<segments; i++)
	{
		MFSetPosition(MFSin(around)*radius, 0.0f, MFCos(around)*radius);
		MFSetPosition(0.0f, 0.0f, 0.0f);

		around -= aroundInc;
	}

	if(!wireframe)
		MFSetPosition(0.0f, 0.0f, radius);

	MFEnd();
}

void Shape_DrawArrow(const Vector3& pos, const Vector3& dir, float length, float radius, const Vector4& colour, const Matrix &mat, bool wireframe)
{
	CALLSTACK;
	Vector3 v = dir;
	v.Normalise();

	v*= length * 0.7f;
	if(radius == 0.0f)
	{
		MFPrimitive(PT_LineList|PT_Prelit|PT_Untextured);
		MFSetMatrix(mat);
		MFBegin(2);
		MFSetColour(colour);
		MFSetPosition(pos);
		MFSetPosition(pos+v);
		MFEnd();
	}
	else
		Shape_DrawCylinder(pos, pos+v, radius, 5, 0, colour, mat, wireframe);

	Shape_DrawCone(pos+v, pos+v+v*0.25f, length*0.03f + radius*2.0f, 5, colour, mat, wireframe);
}

void Shape_DrawTransform(const Matrix& _mat, float scale, bool lite)
{
	CALLSTACK;

	if(lite)
	{
		MFPrimitive(PT_LineList|PT_Prelit|PT_Untextured);
		MFSetMatrix(_mat);
		MFBegin(6);

		MFSetColour(0xFFFF0000);
		MFSetPosition(0.0f,0.0f,0.0f);
		MFSetPosition(scale*0.8f,0.0f,0.0f);

		MFSetColour(0xFF00FF00);
		MFSetPosition(0.0f,0.0f,0.0f);
		MFSetPosition(0.0f,scale*0.8f,0.0f);

		MFSetColour(0xFF0000FF);
		MFSetPosition(0.0f,0.0f,0.0f);
		MFSetPosition(0.0f,0.0f,scale*0.8f);
		MFEnd();

		//MFShape_DrawCone(Vector(scale*0.8f,0.0f,0.0f), Vector(scale,0.0f,0.0f), scale*0.04f, 5, MFVector(1.0f, 0.0f, 0.0f, 1.0f), _mat);
		//MFShape_DrawCone(Vector(0.0f, scale*0.8f,0.0f), Vector(0.0f,scale,0.0f), scale*0.04f, 5, MFVector(0.0f, 1.0f, 0.0f, 1.0f), _mat);
		//MFShape_DrawCone(Vector(0.0f, 0.0f, scale*0.8f), Vector(0.0f,0.0f,scale), scale*0.04f, 5, MFVector(0.0f, 0.0f, 1.0f, 1.0f), _mat);
	}
	else
	{
		Shape_DrawArrow(Vector3::zero, Vector(1.0f,0.0f,0.0f), scale, scale * 0.02f, Vector(1,0,0,1), _mat);
		Shape_DrawArrow(Vector3::zero, Vector(0.0f,1.0f,0.0f), scale, scale * 0.02f, Vector(0,1,0,1), _mat);
		Shape_DrawArrow(Vector3::zero, Vector(0.0f,0.0f,1.0f), scale, scale * 0.02f, Vector(0,0,1,1), _mat);
	}
}
