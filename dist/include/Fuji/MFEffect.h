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

MFEffect* MFEffect_Create();
void MFEffect_Destroy(MFEffect *pEffect);

#endif // _MFEFFECT_H

/** @} */
