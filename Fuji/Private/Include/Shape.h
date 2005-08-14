#ifndef _SHAPE_H
#define _SHAPE_H

#include "MFVector.h"
#include "MFMatrix.h"

//void Blt();
void Shape_DrawQuad(float x, float y, float w, float h, const MFVector &colour=MFVector::one, float su=0.0f, float sv=0.0f, float du=1.0f, float dv=1.0f);
void Shape_DrawQuad(const MFVector& pos, float w, float h, const MFVector &colour=MFVector::one, float su=0.0f, float sv=0.0f, float du=1.0f, float dv=1.0f);
void Shape_DrawUntexturedQuad(float x, float y, float w, float h, const MFVector &colour=MFVector::one);
void Shape_DrawUntexturedQuad(const MFVector& pos, float w, float h, const MFVector &colour=MFVector::one);

void Shape_DrawBox(const MFVector &boxMin, const MFVector &boxMax, const MFVector &colour=MFVector::one, const MFMatrix &mat = MFMatrix::identity, bool wireframe = true);
void Shape_DrawSphere(const MFVector &position, float radius, int segments = 8, int slices = 5, const MFVector &colour=MFVector::one, const MFMatrix &mat = MFMatrix::identity, bool wireframe = true);
void Shape_DrawCapsule(const MFVector &startPoint, const MFVector &endPoint, float radius, int segments = 8, int slices = 5, const MFVector &colour=MFVector::one, const MFMatrix &mat = MFMatrix::identity, bool wireframe = true);
void Shape_DrawCylinder(const MFVector &startPoint, const MFVector &endPoint, float radius, int segments = 8, int slices = 5, const MFVector &colour=MFVector::one, const MFMatrix &mat = MFMatrix::identity, bool wireframe = true);
void Shape_DrawPlane(const MFVector &point, const MFVector &normal, float span, const MFVector &colour=MFVector::one, const MFMatrix &mat = MFMatrix::identity, bool wireframe = true);
void Shape_DrawCone(const MFVector &base, const MFVector &point, float radius, int segments = 8, const MFVector &colour=MFVector::one, const MFMatrix &mat = MFMatrix::identity, bool wireframe = true);
void Shape_DrawArrow(const MFVector& pos, const MFVector& dir, float length, float radius, const MFVector &colour=MFVector::one, const MFMatrix &mat = MFMatrix::identity, bool wireframe = false);
void Shape_DrawTransform(const MFMatrix& _mat, float scale, bool lite = false);

#endif //_SHAPE_H
