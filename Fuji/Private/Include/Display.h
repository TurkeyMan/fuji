#if !defined(_DISPLAY_H)
#define _DISPLAY_H

#define WIDE_ASPECT 1.7777777777777777777777777777778f
#define STANDARD_ASPECT 1.3333333333333333333333333333333f

int CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive);
void DestroyDisplay();
void Display_BeginFrame();
void Display_EndFrame();
void ClearScreen();

void SetProjection(float fov);
bool SetOrtho(bool enable, float width = 640.0f, float height = 480.0f);

#if defined(_XBOX)

struct DisplaySettings
{
	int width, height;
	int rate;
	bool wide;
	bool progressive;
};



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

extern IDirect3DDevice8 *pd3dDevice;
extern DisplaySettings display;

#endif // _XBOX

#endif // _DISPLAY_H

