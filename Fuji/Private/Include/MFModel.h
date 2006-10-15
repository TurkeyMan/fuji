/**
 * @file MFModel.h
 * @brief A set of functions for managing and rendering 3D models.
 * @author Manu Evans
 * @defgroup MFModel Model Management
 * @{
 */

#if !defined(_MFMODEL_H)
#define _MFMODEL_H

#include "MFBoundingVolume.h"

/**
 * @struct MFModel
 * Represents a Fuji model.
 */
struct MFModel;

struct MFMeshChunk;

/**
 * Creates a model from the filesystem.
 * Creates a model from the filesystem.
 * @param pFilename Filename of model to load.
 * @return Returns a new instance of the specified model.
 * @see MFModel_Destroy()
 * @see MFModel_Draw()
 */
MFModel* MFModel_Create(const char *pFilename);

/**
 * Dstroy a model.
 * Destroys a model instance.
 * @param pModel Model instance to be destroyed.
 * @return None.
 * @see MFModel_Create()
 * @see MFModel_Draw()
 */
void MFModel_Destroy(MFModel *pModel);

/**
 * Draw a model.
 * Renders a model using the current scene configuration.
 * @param pModel Model instance to render.
 * @return None.
 * @see MFModel_Create()
 */
void MFModel_Draw(MFModel *pModel);

/**
 * Set the model world matrix.
 * Sets the models local to world matrix.
 * @param pModel Model instance.
 * @param worldMatrix World matrix to assign to the model.
 * @return None.
 * @see MFModel_Draw()
 */
void MFModel_SetWorldMatrix(MFModel *pModel, const MFMatrix &worldMatrix);

/**
 * Set the model colour.
 * Sets the models colour.
 * @param pModel Model instance.
 * @param colour Colour to assign to the model instance.
 * @return None.
 * @see MFModel_Create()
 */
void MFModel_SetColour(MFModel *pModel, const MFVector &colour);

/**
 * Get a models name.
 * Gets the name of a model.
 * @param pModel Model instance.
 * @return The models name.
 * @see MFModel_Create()
 */
const char* MFModel_GetName(MFModel *pModel);

/**
 * Get the number of subobjects.
 * Get the number of subobjects in a model.
 * @param pModel Model instance.
 * @return The number of subobjects in \a pModel.
 * @see MFModel_Create()
 */
int MFModel_GetNumSubObjects(MFModel *pModel);

/**
 * Get the index of a named subobject.
 * Gets the index of a named subobject.
 * @param pModel Model instance.
 * @param pSubobjectName Name of a subobject.
 * @return The index of the named subobject. If the subobject doesn't exist, -1 is returned.
 * @see MFModel_GetSubObjectName()
 */
int MFModel_GetSubObjectIndex(MFModel *pModel, const char *pSubobjectName);

/**
 * Get the name of a specified subobject.
 * Get the name of a specified subobject.
 * @param pModel Model instance.
 * @param index Subobject index.
 * @return The subobjects name.
 * @see MFModel_GetNumSubObjects()
 */
const char* MFModel_GetSubObjectName(MFModel *pModel, int index);

/**
 * Get the models bounding volume.
 * Gets the models bounding volume.
 * @param pModel Model instance.
 * @return Pointer to the models bounding volume.
 */
MFBoundingVolume* MFModel_GetBoundingVolume(MFModel *pModel);

/**
 * Get an MFModel mesh chunk.
 * Gets a pointer to the specified MFModel mesh chunk.
 * @param pModel Model instance.
 * @param subobjectIndex Subobject index.
 * @param meshChunkIndex Mesh chunk index.
 * @return Pointer to the specified mesh chunk.
 */
MFMeshChunk* MFModel_GetMeshChunk(MFModel *pModel, int subobjectIndex, int meshChunkIndex);

// bone structure
int MFModel_GetNumBones(MFModel *pModel);
const char* MFModel_GetBoneName(MFModel *pModel, int boneIndex);
const MFMatrix& MFModel_GetBoneOrigin(MFModel *pModel, int boneIndex);
int MFModel_GetBoneIndex(MFModel *pModel, const char *pName);

// tag's
int MFModel_GetNumTags(MFModel *pModel);
const char* MFModel_GetTagName(MFModel *pModel, int tagIndex);
const MFMatrix& MFModel_GetTagMatrix(MFModel *pModel, int tagIndex);
int MFModel_GetTagIndex(MFModel *pModel, const char *pName);

// ... etc

#endif

/** @} */
