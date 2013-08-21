#if !defined(_MATERIAL_INTERNAL_H)
#define _MATERIAL_INTERNAL_H

#include "MFMaterial.h"
#include "MFVector.h"
#include "MFTexture.h"
#include "MFMatrix.h"
#include "MFResource.h"

#include "DebugMenu_Internal.h"

struct MFMaterialType;
struct MFStateBlock;

// material functions
MFInitStatus MFMaterial_InitModule();
void MFMaterial_DeinitModule();

void MFMaterial_Update();

MFMaterialType *MaterialInternal_GetMaterialType(const char *pTypeName);

// MFMaterial structure
struct MFMaterial : public MFResource
{
	MFMaterialType *pType;
	void *pInstanceData;

	MFStateBlock *pMaterialState;
	bool bStateDirty;
};

// a debug menu material information display object
class MaterialBrowser : public MenuObject
{
public:
	MaterialBrowser();

	virtual void Draw();
	virtual void Update();

	virtual float ListDraw(bool selected, const MFVector &pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual MFVector GetDimensions(float maxWidth);

	int selection;
};

#endif
