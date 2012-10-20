module fuji.animation;

public import fuji.fuji;
import fuji.matrix;
import fuji.model;

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
* @see MFAnimation_Destroy(), MFAnimation_CalculateMatrices()
*/
extern (C) MFAnimation* MFAnimation_Create(const(char*) pFilename, MFModel* pModel);

/**
* Destroy an animation.
* Destroys an animation instance.
* @param pAnimation Animation instance to be destroyed.
* @return Returns the new reference count of the animation. If the returned reference count is 0, the animation is model.
* @see MFAnimation_Create()
*/
extern (C) int MFAnimation_Destroy(MFAnimation* pAnimation);

/**
* Calculate the animation matrices.
* Calculates the animation matrices for the current frame.
* @param pAnimation Animation instance.
* @param pLocalToWorld Optional pointer to a LocalToWorld matrix that will be multiplied into the animation matrices.
* @return Returns a pointer to the array of calculated animation matrices.
* @see MFAnimation_Create()
*/
extern (C) MFMatrix* MFAnimation_CalculateMatrices(MFAnimation* pAnimation, MFMatrix* pLocalToWorld);

/**
* Get the animations frame range.
* Gets the animations valid frame range.
* @param pAnimation Animation instance.
* @param pStartTime Pointer to a float that will receive the start time. Can be NULL.
* @param pEndTime Pointer to a float that will receive the end time. Can be NULL.
* @return None.
* @see MFAnimation_SetFrame()
*/
extern (C) void MFAnimation_GetFrameRange(MFAnimation* pAnimation, float* pStartTime, float* pEndTime);

/**
* Set the current frame.
* Sets the current frame time.
* @param pAnimation Animation instance.
* @param frameTime Frame time to be set as the current frame.
* @return None.
* @see MFAnimation_GetFrameRange()
*/
extern (C) void MFAnimation_SetFrame(MFAnimation* pAnimation, float frameTime);

