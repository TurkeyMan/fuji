#if !defined(_DISPLAY_H)
#define _DISPLAY_H

#define WIDE_ASPECT 1.7777777777777777777777777777778f
#define STANDARD_ASPECT 1.3333333333333333333333333333333f

void Display_InitModule();
void Display_DeinitModule();

int Display_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive);
void Display_DestroyDisplay();
void Display_BeginFrame();
void Display_EndFrame();
void ClearScreen();

void SetProjection(float fov);
bool SetOrtho(bool enable, float width = 640.0f, float height = 480.0f);

void SetViewport(float x, float y, float width, float height);
void ResetViewport();

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

#if defined(_XBOX)

extern IDirect3DDevice8 *pd3dDevice;
extern DisplaySettings display;

#elif defined(_WINDOWS)

extern IDirect3DDevice9 *pd3dDevice;
extern DisplaySettings display;

#endif

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

#endif

#endif // _DISPLAY_H

