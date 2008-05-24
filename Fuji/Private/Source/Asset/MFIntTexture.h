#if !defined(_MFINTTEXTURE_H)
#define _MFINTTEXTURE_H

enum MFIntTextureFormat
{
	MFIMF_Unknown = -1,

	MFIMF_TGA = 0,
	MFIMF_BMP,
	MFIMF_PNG,

	MFIMF_Max,
	MFIMF_ForceInt = 0x7FFFFFFF
};

void MFIntTexture_CreateFromFile(const char *pFilename, MFIntTexture **ppTexture);
void MFIntTexture_CreateFromFileInMemory(const void *pMemory, uint32 size, MFIntTexture **ppTexture);

void MFIntTexture_CreateRuntimeData(MFIntTexture *pTexture, void **ppOutput, uint32 *pSize, MFPlatform platform = MFP_Current);

void MFIntTexture_Destroy(MFIntTexture *pTexture);

#endif
