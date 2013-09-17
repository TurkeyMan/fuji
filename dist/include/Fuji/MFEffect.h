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

/**
 * @struct MFEffect
 * Represents a Fuji Effect.
 */
struct MFEffect;

/**
 * Create an effect.
 * Creates an effect from the filesystem.
 * @param pFilename Name of the effect to create.
 * @return Pointer to an MFEffect structure representing the newly created effect.
 * @remarks If the specified effect has already been created, MFEffect_Create will return a new reference to the already created effect.
 * @see MFTexture_Release()
 */
MF_API MFEffect* MFEffect_Create(const char *pFilename);

/**
 * Release an MFEffect instance.
 * Release a reference to an MFEffect. If the reference count reaches 0, the effect is destroyed.
 * @param pEffect Effect instance to be destroyed.
 * @return The new reference count of the effect. If the returned reference count is 0, the effect is destroyed.
 * @see MFTexture_Create()
 */
MF_API int MFEffect_Release(MFEffect *pEffect);

#endif // _MFEFFECT_H

/** @} */
