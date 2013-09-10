/**
 * @file MFEffect.h
 * @brief Manage rendering effects.
 * @author Manu Evans
 * @defgroup MFEffect Rendering Effects
 * @{
 */

#pragma once
#if !defined(_MFEFFECT_H)
#define _MFEFFECT_H

struct MFEffect;

MF_API MFEffect* MFEffect_Create(const char *pFilename);
MF_API void MFEffect_Destroy(MFEffect *pEffect);

#endif // _MFEFFECT_H

/** @} */
