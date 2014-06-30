/**
 * @file MFModel.h
 * @brief A set of functions for managing and rendering 3D models.
 * @author Manu Evans
 * @defgroup MFModel Model Management
 * @{
 */

#if !defined(_MFMODEL_H)
#define _MFMODEL_H

#include "MFMatrix.h"
#include "MFBoundingVolume.h"

struct MFRenderLayerSet;
struct MFStateBlock;

/**
 * @struct MFModel
 * Represents a Fuji model.
 */
struct MFModel;

struct MFMeshChunk;
struct MFAnimation;

/**
 * Creates a model from the filesystem.
 * Creates a model from the filesystem.
 * @param pFilename Filename of model to load.
 * @return A new instance of the specified model.
 * @see MFModel_Release()
 */
MF_API MFModel* MFModel_Create(const char *pFilename);

/**
 * Creates a model from the filesystem.
 * Creates a model from the filesystem.
 * @param pFilename Filename of model to load.
 * @param pAnimationFilename Filename of an animation to load and bind to the model instance. If \a pAnimationFilename is NULL, \a pFilename will be used instead.
 * @return A new instance of the specified model with animation already loaded and bound.
 * @see MFModel_Create()
 */
MF_API MFModel* MFModel_CreateWithAnimation(const char *pFilename, const char *pAnimationFilename = NULL);

/**
 * Destroy a model.
 * Destroys a model instance.
 * @param pModel Model instance to be destroyed.
 * @return None.
 * @see MFModel_Create()
 */
MF_API void MFModel_Release(MFModel *pModel);

/**
 * Submit a model for rendering.
 * Submits a model to the renderer. Geometry will be sorted among the layers provided by \a pLayerSet.
 * @param pModel Model instance to render.
 * @param pLayerSet An MFRenderLayerSet that will be used to render the geometry.
 * @param pMaterialOverride Optional stateblock which can be used to override material settings.
 * @param pView Optional stateblock which can be used to provide view/camera configuration.
 * @return None.
 * @see MFModel_Create()
 */
MF_API void MFModel_SubmitGeometry(MFModel *pModel, MFRenderLayerSet *pLayerSet, const MFStateBlock *pMaterialOverride, const MFStateBlock *pView);

/**
 * Set the model world matrix.
 * Sets the models local to world matrix.
 * @param pModel Model instance.
 * @param worldMatrix World matrix to assign to the model.
 * @return None.
 */
MF_API void MFModel_SetWorldMatrix(MFModel *pModel, const MFMatrix &worldMatrix);

/**
 * Get a models name.
 * Gets the name of a model.
 * @param pModel Model instance.
 * @return The models name.
 * @see MFModel_Create()
 */
MF_API const char* MFModel_GetName(const MFModel *pModel);

/**
 * Get the number of subobjects.
 * Get the number of subobjects in a model.
 * @param pModel Model instance.
 * @return The number of subobjects in \a pModel.
 * @see MFModel_Create()
 */
MF_API int MFModel_GetNumSubObjects(const MFModel *pModel);

/**
 * Get the index of a named subobject.
 * Gets the index of a named subobject.
 * @param pModel Model instance.
 * @param pSubobjectName Name of a subobject.
 * @return The index of the named subobject. If the subobject doesn't exist, -1 is returned.
 * @see MFModel_GetSubObjectName()
 */
MF_API int MFModel_GetSubObjectIndex(const MFModel *pModel, const char *pSubobjectName);

/**
 * Get the name of a specified subobject.
 * Get the name of a specified subobject.
 * @param pModel Model instance.
 * @param index Subobject index.
 * @return The subobjects name.
 * @see MFModel_GetNumSubObjects()
 */
MF_API const char* MFModel_GetSubObjectName(const MFModel *pModel, int index);

/**
 * Enabe or disable a subobject.
 * Enabe or disable the specified subobject.
 * @param pModel Model instance.
 * @param index Subobject index.
 * @param enable Bool to enable or disable the subobject.
 * @return None.
 * @see MFModel_GetNumSubObjects(), MFModel_IsSubobjectEnabed()
 */
MF_API void MFModel_EnableSubobject(MFModel *pModel, int index, bool enable);

/**
 * Find if a subobject is enabled.
 * Finds if a specified subobject is enabled or disabled.
 * @param pModel Model instance.
 * @param index Subobject index.
 * @return True if the subobject is enabled.
 * @see MFModel_GetNumSubObjects(), MFModel_EnableSubobject()
 */
MF_API bool MFModel_IsSubobjectEnabed(const MFModel *pModel, int index);

/**
 * Get the models bounding volume.
 * Gets the models bounding volume.
 * @param pModel Model instance.
 * @return Pointer to the models bounding volume.
 */
MF_API const MFBoundingVolume* MFModel_GetBoundingVolume(const MFModel *pModel);

/**
 * Get an MFModel mesh chunk.
 * Gets a pointer to the specified MFModel mesh chunk.
 * @param pModel Model instance.
 * @param subobjectIndex Subobject index.
 * @param meshChunkIndex Mesh chunk index.
 * @return Pointer to the specified mesh chunk.
 */
MF_API MFMeshChunk* MFModel_GetMeshChunk(MFModel *pModel, int subobjectIndex, int meshChunkIndex);

/**
 * Get a pointer to an MFAnimation associated with the model.
 * Gets a pointer to an MFAnimation associated with the model.
 * @param pModel Model instance.
 * @return Pointer to the MFAnimation associated with this model. If no animation is loaded, MFModel_GetAnimation returns NULL.
 */
MF_API MFAnimation *MFModel_GetAnimation(MFModel *pModel);

/**
 * Get the number of bones in the model.
 * Gets the number of bones in the model.
 * @param pModel Model instance.
 * @return The number of bones in the model.
 */
MF_API int MFModel_GetNumBones(const MFModel *pModel);

/**
 * Get the name of a bone.
 * Gets the name of the specified bone.
 * @param pModel Model instance.
 * @param boneIndex Target bone index.
 * @return The name of the specified bone.
 */
MF_API const char* MFModel_GetBoneName(const MFModel *pModel, int boneIndex);

/**
 * Get a bones origin matrix.
 * Gets the specified bones origin or 'bind' matrix.
 * @param pModel Model instance.
 * @param boneIndex Target bone index.
 * @return The bones origin matrix.
 */
MF_API const MFMatrix& MFModel_GetBoneOrigin(const MFModel *pModel, int boneIndex);

/**
 * Get the index of a bone by name.
 * Gets the index of a bone by name.
 * @param pModel Model instance.
 * @param pName The target bones name.
 * @return The index of the named bone, or -1 if the bone does not exist.
 */
MF_API int MFModel_GetBoneIndex(const MFModel *pModel, const char *pName);

/**
 * Get the number of tags in the model.
 * Gets the number of tags in the model.
 * @param pModel Model instance.
 * @return The number of tags in the model.
 */
MF_API int MFModel_GetNumTags(const MFModel *pModel);

/**
 * Get the name of a tag.
 * Gets the name of the specified tag.
 * @param pModel Model instance.
 * @param tagIndex Target tag index.
 * @return The name of the specified tag.
 */
MF_API const char* MFModel_GetTagName(const MFModel *pModel, int tagIndex);

/**
 * Get a tag matrix.
 * Gets the specified tag matrix.
 * @param pModel Model instance.
 * @param tagIndex Target tag index.
 * @return The tag matrix in the models local space.
 */
MF_API const MFMatrix& MFModel_GetTagMatrix(const MFModel *pModel, int tagIndex);

/**
 * Get the index of a tag by name.
 * Gets the index of a tag by name.
 * @param pModel Model instance.
 * @param pName The target tags name.
 * @return The index of the named tag, or -1 if the tag does not exist.
 */
MF_API int MFModel_GetTagIndex(const MFModel *pModel, const char *pName);

#endif

/** @} */
