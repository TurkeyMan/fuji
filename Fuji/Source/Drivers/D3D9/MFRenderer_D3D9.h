#if !defined(_MFRENDERER_D3D9_H)
#define _MFRENDERER_D3D9_H

#include <d3d9.h>

void MFRendererPC_SetWorldToScreenMatrix(const MFMatrix &worldToScreen);
void MFRendererPC_SetAnimationMatrix(int boneID, const MFMatrix &animationMatrix);
void MFRendererPC_SetTextureMatrix(const MFMatrix &textureMatrix);
void MFRendererPC_SetModelColour(const MFVector &colour);
void MFRendererPC_SetNumWeights(int numWeights);
int MFRendererPC_GetNumWeights();

#endif // _MFRENDERER_D3D9_H
