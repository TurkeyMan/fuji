#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_PS2

#include "Display_Internal.h"
#include "MFView.h"
#include "MFVector.h"
#include "MFMatrix.h"
#include "MFPrimitive.h"
#include "MFTexture.h"
#include "MFRenderer.h"
#include "MFMaterial.h"
#include "MFMaterial_Internal.h"
#include "MFTexture_Internal.h"

#include "PS2_Internal.h"


#include "../../Source/Materials/MFMat_Standard.h"

#include <packet.h>
#include <graph_registers.h>

uint32 primType;
uint32 beginCount;
uint32 currentVert;

PACKET packet;
extern MFMaterial *pSetMaterial;

int  r, g, b, a;

#define NUM_VERTS	(4096*4)
#define GIF_AD		0x0E

MFInitStatus MFPrimitive_InitModule(int moduleId, bool bPerformInitialisation)
{
	MFCALLSTACK;
	if(packet_allocate(&packet, 1024) < 0)
	{
		while(1) MFDebug_Message("NO PACKET");
	}
}

void MFPrimitive_DeinitModule()
{
	MFCALLSTACK;
	packet_free(&packet);
}

MF_API void MFPrimitive(uint32 type, uint32 hint)
{
	MFCALLSTACK;

	primType = type & PT_PrimMask;

	if(type & PT_Untextured)
	{
		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));
	}

	MFRenderer_Begin();
}

int width, height;


// Here we create the beginnings of the DMA chain, whig gets filled by other
// immediate calls, and kicked to the GS in MFEnd()

// TODO: Remove all this and replace with a custom format that is sent to vu1 for
// transformation, lighting and being able to deal with other prim types!
MF_API void MFBegin(uint32 vertexCount)
{
	MFCALLSTACK;

	packet_reset(&packet);
	beginCount = vertexCount;
	currentVert = 0;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pSetMaterial->pInstanceData;
	MFTexture *pTexture = pData->pTextures[pData->diffuseMapIndex];
	width = pTexture->pTemplateData->pSurfaces[0].width;
	height = pTexture->pTemplateData->pSurfaces[0].height;

	int tw = mylog2(width);
	int th = mylog2(height);

	int num = vertexCount*2 + 9;

	packet_append_64(&packet, DMA_SET_TAG(num, 0, DMA_TAG_END, 0, 0, 0));
	packet_append_64(&packet, 0);

	packet_append_64(&packet, GIF_SET_TAG(num-1, 1, 0, 0, 0, 1 ));
	packet_append_64(&packet, GIF_AD);

	/* GIF_SET_TEX0(tex base, tex width, tex psm, width, height, texcure alpha component, texture function, CLUT stuff */
	packet_append_64(&packet, GIF_SET_TEX0( tex_addr>>8,  width >> 6, 0,tw,th,1,1,0,0,0,0,0));
	packet_append_64(&packet, GIF_REG_TEX0_1);

	packet_append_64(&packet, GIF_SET_TEX1(1, 0, 1, 1, 0, 0, 0));
	packet_append_64(&packet, GIF_REG_TEX1_1);

	packet_append_64(&packet, GIF_SET_TEXA(0, 1, 255));
	packet_append_64(&packet, GIF_REG_TEXA);

	packet_append_64(&packet, GIF_SET_CLAMP(0, 0, 0, 0, 0, 0));
	packet_append_64(&packet, GIF_REG_CLAMP_1);

	float one = 1.0f; /* cludgy way to obtain 1.0f as a u32 */
	packet_append_64(&packet, GIF_SET_RGBAQ( r, g, b, a, *((int*)(&one)) )  );
	packet_append_64(&packet, GIF_REG_RGBAQ);

	packet_append_64(&packet, GIF_SET_ALPHA(0, 1,0,1,0));
	packet_append_64(&packet, GIF_REG_ALPHA_1);

	/* GIF_SET_PRIM(prim type, Shading, Texture, Fog, Alpha, AA, STQ/UV, context, fragmtnt*/
	packet_append_64(&packet, GIF_SET_PRIM (6, 0, 1, 0, 1, 0, 1, 0, 0));
	packet_append_64(&packet, GIF_REG_PRIM);
}

MF_API void MFSetMatrix(const MFMatrix &mat)
{
	MFCALLSTACK;
}

MF_API void MFSetColour(float rr, float gg, float bb, float aa)
{
	r = (unsigned int)(256*rr);
	g = (unsigned int)(256*gg);
	b = (unsigned int)(256*bb);
	a = (unsigned int)(128*aa);
}

MF_API void MFSetTexCoord1(float u, float v)
{
	unsigned int iu = (unsigned int)(u * width);
	unsigned int iv = (unsigned int)(v * height);

	packet_append_64(&packet, GIF_SET_UV (iu<<4,iv<<4));
	packet_append_64(&packet, GIF_REG_UV);
}

void MFPrimitive_DrawStats()
{
	MFCALLSTACK;
}

MF_API void MFSetNormal(float x, float y, float z)
{
}

MF_API void MFSetPosition(float x, float y, float z)
{
	MFCALLSTACK;

	int ix, iy;
	ix = (int) x;
	iy = (int) y;

	packet_append_64(&packet, GIF_SET_XYZ((2048-320+ix)<<4,(2078-256+iy)<<4,0));
	packet_append_64(&packet, GIF_REG_XYZ2);

	++currentVert;
}

void textured_sprite(unsigned int vaddr, int x, int y, int width, int height);
MF_API void MFEnd()
{
	MFCALLSTACK;

	MFDebug_Assert(currentVert == beginCount, "Incorrect number of vertices.");

	// Send the DMA chain we have built
	if(packet_send(&packet, DMA_CHANNEL_GIF, DMA_FLAG_CHAIN) < 0)
	{
		MFDebug_Message("CANT SEND PACKET");
		while(1);
	}

}

#endif
