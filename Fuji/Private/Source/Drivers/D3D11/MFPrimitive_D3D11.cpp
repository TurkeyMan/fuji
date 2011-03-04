#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFPrimitive_InitModule MFPrimitive_InitModule_D3D11
	#define MFPrimitive_DeinitModule MFPrimitive_DeinitModule_D3D11
	#define MFPrimitive_DrawStats MFPrimitive_DrawStats_D3D11
	#define MFPrimitive MFPrimitive_D3D11
	#define MFBegin MFBegin_D3D11
	#define MFSetMatrix MFSetMatrix_D3D11
	#define MFSetColour MFSetColour_D3D11
	#define MFSetTexCoord1 MFSetTexCoord1_D3D11
	#define MFSetNormal MFSetNormal_D3D11
	#define MFSetPosition MFSetPosition_D3D11
	#define MFEnd MFEnd_D3D11
	#define MFPrimitive_BeginBlitter MFPrimitive_BeginBlitter_D3D11
	#define MFPrimitive_Blit MFPrimitive_Blit_D3D11
	#define MFPrimitive_StretchBlit MFPrimitive_StretchBlit_D3D11
	#define MFPrimitive_EndBlitter MFPrimitive_EndBlitter_D3D11
#endif

#include "MFPrimitive.h"

#include <D3D11.h>


static const int primBufferSize = 1536;

static bool gRenderQuads = false;

struct LitVertexD3D11
{
	struct LitPos
	{
		float x, y, z;
	} pos;

	struct LitNormal
	{
		float x, y, z;
	} normal;

	unsigned int colour;

	float u,v;
};

static LitVertexD3D11 primBuffer[primBufferSize];
static LitVertexD3D11 current;

void MFPrimitive_InitModule()
{
	MFCALLSTACK;
}

void MFPrimitive_DeinitModule()
{
	MFCALLSTACK;
}

void MFPrimitive_DrawStats()
{

}

void MFPrimitive(uint32 type, uint32 hint)
{
	MFCALLSTACK;
}

void MFBegin(uint32 vertexCount)
{
	MFCALLSTACK;
}

void MFSetMatrix(const MFMatrix &mat)
{
	MFCALLSTACK;
}

void MFSetColour(float r, float g, float b, float a)
{
}

void MFSetTexCoord1(float u, float v)
{
}

void MFSetNormal(float x, float y, float z)
{
}

void MFSetPosition(float x, float y, float z)
{
	MFCALLSTACK;
}

void MFEnd()
{
	MFCALLSTACK;
}

void MFPrimitive_BeginBlitter(int numBlits)
{
}

void MFPrimitive_Blit(int x, int y, int tx, int ty, int tw, int th)
{
	MFPrimitive_StretchBlit(x, y, tw, th, tx, ty, tw, th);
}

void MFPrimitive_StretchBlit(int x, int y, int w, int h, int tx, int ty, int tw, int th)
{
}

void MFPrimitive_EndBlitter()
{
}

#endif // MF_RENDERER
