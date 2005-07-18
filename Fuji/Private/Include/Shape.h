#ifndef _SHAPE_H
#define _SHAPE_H

//void Blt();
void Shape_DrawQuad(float x, float y, float w, float h, const Vector4 &colour=Vector4::one, float su=0.0f, float sv=0.0f, float du=1.0f, float dv=1.0f);
void Shape_DrawQuad(const Vector3& pos, float w, float h, const Vector4 &colour=Vector4::one, float su=0.0f, float sv=0.0f, float du=1.0f, float dv=1.0f);
void Shape_DrawUntexturedQuad(float x, float y, float w, float h, const Vector4 &colour=Vector4::one);
void Shape_DrawUntexturedQuad(const Vector3& pos, float w, float h, const Vector4 &colour=Vector4::one);

void Shape_DrawBox(const Vector3 &boxMin, const Vector3 &boxMax, const Vector4 &colour=Vector4::one, const Matrix &mat = Matrix::identity, bool wireframe = true);
void Shape_DrawSphere(const Vector3 &position, float radius, int segments = 8, int slices = 5, const Vector4 &colour=Vector4::one, const Matrix &mat = Matrix::identity, bool wireframe = true);
void Shape_DrawCapsule(const Vector3 &startPoint, const Vector3 &endPoint, float radius, int segments = 8, int slices = 5, const Vector4 &colour=Vector4::one, const Matrix &mat = Matrix::identity, bool wireframe = true);
void Shape_DrawCylinder(const Vector3 &startPoint, const Vector3 &endPoint, float radius, int segments = 8, int slices = 5, const Vector4 &colour=Vector4::one, const Matrix &mat = Matrix::identity, bool wireframe = true);
void Shape_DrawPlane(const Vector3 &point, const Vector3 &normal, float span, const Vector4 &colour=Vector4::one, const Matrix &mat = Matrix::identity, bool wireframe = true);
void Shape_DrawCone(const Vector3 &base, const Vector3 &point, float radius, int segments = 8, const Vector4 &colour=Vector4::one, const Matrix &mat = Matrix::identity, bool wireframe = true);
void Shape_DrawArrow(const Vector3& pos, const Vector3& dir, float length, float radius, const Vector4 &colour=Vector4::one, const Matrix &mat = Matrix::identity, bool wireframe = false);
void Shape_DrawTransform(const Matrix& _mat, float scale, bool lite = false);

#endif //_SHAPE_H
