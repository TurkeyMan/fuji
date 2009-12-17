#if !defined(_DISPLAY_INTERNAL_H)
#define _DISPLAY_INTERNAL_H

#include "Display.h"

// internal functions
void MFDisplay_InitModule();
void MFDisplay_DeinitModule();

// these are used by Fuji internally and not exposed to the game
int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive);
void MFDisplay_DestroyDisplay();

// display settings
struct MFDisplaySettings
{
	int width, height;
	int fullscreenWidth, fullscreenHeight;
	int refreshRate;
	int colourDepth;
	bool windowed;
	bool wide;			// wide screen aspect
	bool progressive;	// progressive scan
};

extern MFDisplaySettings gDisplay;

class MFRenderable
{
public:
	virtual ~MFRenderable();

	virtual void Draw() = 0;

	uint32 flags;
};
/*
// vertex formats.. to be removed later
#if !defined(_FUJI_UTIL)
#if defined(MF_WINDOWS) || defined(MF_XBOX)

class Vec3
{
public:
	inline void Set(float _x, float _y, float _z) { x=_x; y=_y; z=_z; }

	float x,y,z;
};

struct Vertex
{
	enum
	{
		FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE
	};

	Vec3 pos;
	unsigned int colour;
};

struct FontVertex
{
	enum
	{
		FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1
	};

	Vec3 pos;
	unsigned int colour;
	float u,v;
};

struct LitVertex
{
	enum
	{
		FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1
	};

	Vec3 pos;
	Vec3 normal;
	unsigned int colour;
	float u,v;
};

struct FileVertex
{
	enum
	{
		FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1
	};

	Vec3 pos;
	Vec3 normal;
	unsigned int colour;
	float u,v;
};

#elif defined(_LINUX)

class Vec3
{
public:
	inline void Set(float _x, float _y, float _z) { x=_x; y=_y; z=_z; }

	GLfloat x,y,z;
};

struct Vertex
{
	Vec3 pos;
	GLuint colour;
};

struct FontVertex
{
	Vec3 pos;
	GLuint colour;
	GLfloat u,v;
};

struct LitVertex
{
	Vec3 pos;
	Vec3 normal;
	GLuint colour;
	GLfloat u,v;
};

struct FileVertex
{
	Vec3 pos;
	Vec3 normal;
	GLuint colour;
	GLfloat u,v;
};

#endif // #if defined(_LINUX)
#endif // #!define(_FUJI_UTIL)
*/
#endif
