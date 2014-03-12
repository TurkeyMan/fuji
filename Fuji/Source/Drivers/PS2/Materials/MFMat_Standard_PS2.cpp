#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_PS2

#include "Display_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFView_Internal.h"
#include "MFHeap.h"

#include "PS2_Internal.h"

#include "../../Source/Materials/MFMat_Standard.h"

#include <stdio.h>
#include <graph.h>
#include <packet.h>
#include <kernel.h>

MFMaterial *pSetMaterial;
extern uint32 renderSource;
extern uint32 currentRenderFlags;

int MFMat_Standard_RegisterMaterial(MFMaterialType *pType)
{
	MFCALLSTACK;

	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
	MFCALLSTACK;

}

unsigned int tex_addr;

extern PACKET  packet;

#define GIF_AD          0x0E
void textured_sprite(unsigned int vaddr, int x, int y, int width, int height){
    int tw = mylog2(width);
    int th = mylog2(height);

    packet_reset(&packet);

    packet_append_64(&packet, DMA_SET_TAG(9, 0, DMA_TAG_END, 0, 0, 0));
    packet_append_64(&packet, 0);

    packet_append_64(&packet, GIF_SET_TAG(8, 1, 0, 0, 0, 1 ));
    packet_append_64(&packet, GIF_AD);

    /* GIF_SET_TEX0(tex base, tex width, tex psm, width, height, texcure alpha component, texture function, CLUT stuff */
    packet_append_64(&packet, GIF_SET_TEX0( vaddr>>8,  width >> 6, 0,tw,th,1,1,0,0,0,0,0));
    packet_append_64(&packet, GIF_REG_TEX0_1);

    float one = 1.0f; /* cludgy way to obtain 1.0f as a u32 */
    packet_append_64(&packet, GIF_SET_RGBAQ( 128, 128, 128, 0x0, *((int*)(&one)) )  );
    packet_append_64(&packet, GIF_REG_RGBAQ);

    packet_append_64(&packet, GIF_SET_ALPHA(0, 1,0,1,0));
    packet_append_64(&packet, GIF_REG_ALPHA_1);

    /* GIF_SET_PRIM(prim type, Shading, Texture, Fog, Alpha, AA, STQ/UV, context, fragmtnt*/
    packet_append_64(&packet, GIF_SET_PRIM (6, 0, 1, 0, 1, 0, 1, 0, 0));
    packet_append_64(&packet, GIF_REG_PRIM);

    packet_append_64(&packet, GIF_SET_UV (0<<4,0<<4));
    packet_append_64(&packet, GIF_REG_UV);

    packet_append_64(&packet, GIF_SET_XYZ((2048-320+x)<<4,(2078-256+y)<<4,0));
    packet_append_64(&packet, GIF_REG_XYZ2);
    packet_append_64(&packet, GIF_SET_UV (64 << 4, 64<< 4));
    packet_append_64(&packet, GIF_REG_UV);

    packet_append_64(&packet, GIF_SET_XYZ( (2048-320+width+x) << 4, (2078-256+height+y)<< 4 , 0x0000));
    packet_append_64(&packet, GIF_REG_XYZ2);


            FlushCache(2);
            FlushCache(0);
    //        FlushCache(1);


    if (packet_send(&packet, DMA_CHANNEL_GIF, DMA_FLAG_CHAIN) < 0){
	printf("CANT SEND PACKET\n");
	while(1) ;
    }
}


int MFMat_Standard_Begin(MFMaterial *pMaterial, MFRendererState &state)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	if(pSetMaterial != pMaterial)
	{
	    pSetMaterial = pMaterial;

	    MFTexture *pTexture = pData->pTextures[pData->diffuseMapIndex];
	    int width = pTexture->pTemplateData->pSurfaces[0].width;
	    int height = pTexture->pTemplateData->pSurfaces[0].height;
	    char *pImageData = pTexture->pTemplateData->pSurfaces[0].pImageData;
	    
	    // TODO: Should rally be a more sophisticated allocation scheme
	    tex_addr =  (graph_get_size() + graph_get_zsize());

	    graph_vram_write( tex_addr, width, height, 0, pImageData, width*height*4);

	    // Since all we currently do is flat sprites, and the logo is drawn by the system
	    // as tris, we inject it here so it still looks good :)
	    // Remove when immediate mode is finished
	    //
	    if (pMaterial->pName[0] != 'A'){
		textured_sprite( tex_addr, 30, 512-95, width, height);
	    }
	}

	return 0;
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	pMaterial->pInstanceData = MFHeap_Alloc(sizeof(MFMat_Standard_Data));
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	MFZeroMemory(pData, sizeof(MFMat_Standard_Data));

	pData->ambient = MFVector::one;
	pData->diffuse = MFVector::one;

	pData->materialType = MF_AlphaBlend;
	pData->opaque = true;

	pData->textureMatrix = MFMatrix::identity;
	pData->uFrames = 1;
	pData->vFrames = 1;

	pData->alphaRef = 1.0f;
}

void MFMat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	for(uint32 a=0; a<pData->textureCount; a++)
	{
		MFTexture_Destroy(pData->pTextures[a]);
	}

	MFHeap_Free(pMaterial->pInstanceData);
}

#endif
