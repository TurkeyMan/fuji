#if !defined(_VIEW_H)
#define _VIEW_H

#include "Matrix.h"

class View
{
public:
	View();

	// Static Methods
	static View* GetCurrent();
	static void UseDefault();

	// Satic Members
	static View currentView;
	static View defaultView;

	// Methods
	void Use();

	void SetProjection(float fov);
	bool SetOrtho(bool enabled, float width = 640.0f, float height = 480.0f);

	void SetCameraMatrix(const Matrix &viewMat);

	inline Matrix *GetWorldToViewMatrix() { return &view; }
	inline Matrix *GetViewToScreenMatrix() { return &projection; }

	Matrix *GetWorldToScreenMatrix();

	Matrix *GetLocalToScreen(const Matrix& localToWorld, Matrix *pOutput);
	Matrix *GetLocalToView(const Matrix& localToWorld, Matrix *pOutput);

	// Members
	Matrix projection;
	Matrix view;

	Matrix viewProj;
	bool viewProjDirty;

	float FOV;
	bool isOrtho;
};

#endif
