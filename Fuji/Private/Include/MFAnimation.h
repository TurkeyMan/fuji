/**
 * @file MFAnimation.h
 * @brief A set of functions for managing mesh animation.
 * @author Manu Evans
 * @defgroup MFAnimation Animation Management
 * @{
 */

#if !defined(_MFANIMATION_H)
#define _MFANIMATION_H

#include "MFModel.h"

/**
 * @struct MFAnimation
 * Represents a Fuji animation.
 */
struct MFAnimation;

/**
 * Create an animation from the filesystem.
 * Creates an animation from the filesystem.
 * @param pFilename Filename of the animation to load.
 * @param pModel MFModel instance the animation will be bound to.
 * @return Returns a new instance of the specified animation.
 * @see MFAnimation_Destroy()
 */
MFAnimation* MFAnimation_Create(const char *pFilename, MFModel *pModel);

/**
 * Dstroy an animation.
 * Destroys an animation instance.
 * @param pAnimation Animation instance to be destroyed.
 * @return None.
 * @see MFAnimation_Create()
 */
void MFAnimation_Destroy(MFAnimation *pAnimation);

MFMatrix *MFAnimation_CalculateMatrices(MFAnimation *pAnimation);

int MFAnimation_GetNumFrames(MFAnimation *pAnimation);
void MFAnimation_SetFrame(MFAnimation *pAnimation, int frame);

#endif

/** @} */
