#if !defined(_VIEW_H)
#define _VIEW_H

#include "Matrix.h"

class View
{
public:
	static View currentView;
	static View defaultView;

	static View* GetCurrent();
	static void UseDefault();
	void Use();

	void SetProjection(float fov);
	bool SetOrtho(bool enabled, float width = 640.0f, float height = 480.0f);

	void SetCameraMatrix(const Matrix &viewMat);

	Matrix projection;
	Matrix view;

	float FOV;
	bool isOrtho;
};

#endif
