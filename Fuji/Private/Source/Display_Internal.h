#if !defined(_DISPLAY_INTERNAL_H)
#define _DISPLAY_INTERNAL_H

#if defined(_LINUX)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#endif

#include "Display.h"

// internal functions
void Display_InitModule();
void Display_DeinitModule();

// these are used by Fuji internally and not exposed to the game
int Display_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive);
void Display_DestroyDisplay();
void Display_BeginFrame();
void Display_EndFrame();

// display settings
struct DisplaySettings
{
	int width, height;
	int fullscreenWidth, fullscreenHeight;
	int refreshRate;
	int colourDepth;
	bool windowed;
	bool wide;			// wide screen aspect
	bool progressive;	// progressive scan
};

extern DisplaySettings display;

// renderable interface
enum RenderFlags
{
	RF_OrderMask	= 0x0000000F,

	RF_Transparent	= (1<<4),
	RF_Terrain		= (1<<5),
	RF_Overlay		= (1<<6),
	RF_BelowWater	= (1<<7),
	RF_AboveWater	= (1<<8),

	RF_ForceInt		= 0x7FFFFFFF
};

class Renderable
{
public:
	virtual ~Renderable();

	virtual void Draw() = 0;

	uint32 flags;
};

// platform specific stuff
#if defined(_XBOX)
	extern IDirect3DDevice8 *pd3dDevice;
#elif defined(_WINDOWS)
	extern IDirect3DDevice9 *pd3dDevice;
#endif

// vertex formats.. to be removed later
#if !defined(_FUJI_UTIL)
#if defined(_XBOX) || defined(_WINDOWS)

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

#endif
