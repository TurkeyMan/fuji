#pragma once
#if !defined(_MFINTEFFECT_H)
#define _MFINTEFFECT_H

struct MFIntEffect;
struct MFEffect;

MF_API MFIntEffect *MFIntEffect_CreateFromSourceData(const char *pFilename);

MF_API void MFIntEffect_Destroy(MFIntEffect *pEffect);

MF_API void MFIntEffect_CreateRuntimeData(MFIntEffect *pEffect, MFEffect **ppOutputEffect, size_t *pSize, MFPlatform platform, size_t extraBytes = 0);

#endif
