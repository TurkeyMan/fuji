#include "Common.h"
#include "Renderer_XB.h"
#include "Material.h"
#include "Model.h"
#include "Animation.h"

void SetRenderer(uint32 renderType, uint32 flags)
{
	Material* pMat = Material::GetCurrent();

	pMat->pTextures[pMat->diffuseMapIndex]->SetTexture(0);
}


