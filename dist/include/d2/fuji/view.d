module fuji.view;

public import fuji.fuji;
import fuji.matrix;
import fuji.renderstate;

/**
* Push the current view state.
* Push the current view state onto the view stack.
* @return None.
* @see MFView_Pop()
*/
extern (C) void MFView_Push();

/**
* Pop the top view state.
* Pop the top view state from the view stack.
* @return None.
* @see MFView_Push()
*/
extern (C) void MFView_Pop();

/**
* Configure the view to defaults.
* Configure the current view state to defaults.
* @return None.
* @see MFView_Push()
* @see MFView_Pop()
*/
extern (C) void MFView_SetDefault();

/**
* Configure the projection matrix.
* Configure the projection of the current view.
* @param fieldOfView Field of view, in radians.
* @param nearPlane Distance to near clipping plane.
* @param farPlane Distance to far clipping plane.
* @return None.
* @see MFView_SetProjection()
* @see MFView_SetAspectRatio()
*/
extern (C) void MFView_ConfigureProjection(float fieldOfView, float nearPlane, float farPlane);

/**
* Set a custom projection matrix.
* Configure the projection to use a custom projection matrix.
* @param projectionMatrix Matrix to be used as the custom projection matrix.
* @param bYIsUp Set to true is the Y axis extends in the 'upward' direction. (ie. towards the top of the screen)
* @return None.
* @see MFView_SetProjection()
* @see MFView_SetOrtho()
*/
extern (C) void MFView_SetCustomProjection(ref const(MFMatrix) projectionMatrix, bool bYIsUp);

/**
* Set the aspect ratio.
* Set the screen aspect ratio of the current view.
* @param aspectRatio Value to set as the screen aspect ratio.
* @return None.
* @see MFView_ConfigureProjection()
*/
extern (C) void MFView_SetAspectRatio(float aspectRatio);

/**
* Set the view to projection mode.
* Set the view to projection mode using the current projection settings.
* @return None.
* @see MFView_SetOrtho()
*/
extern (C) void MFView_SetProjection();

/**
* Set the view to orthographic mode.
* Set the view to orthographic mode using the current ortho settings.
* @param pOrthoRect Optional pointer to an ortho rect that describes the ortho space. Pass NULL to use the current ortho rect.
* @return None.
* @see MFView_SetProjection()
*/
extern (C) void MFView_SetOrtho(const(MFRect)* pOrthoRect = null);

/**
* Get the current ortho rect.
* Get the current orthographic projection rect.
* @param pOrthoRect Pointer to an MFRect structure that receives the current ortho rect parameters.
* @return None.
* @see MFView_SetOrtho()
*/
extern (C) void MFView_GetOrthoRect(MFRect *pOrthoRect);

/**
* Test for ortho state.
* Gets the current views ortho state.
* @return Returns true if ortho mode is enabled.
* @see MFView_SetOrtho()
*/
extern (C) bool MFView_IsOrtho();

/**
* Sets the camera matrix.
* Sets the current views camera matrix.
* @param cameraMatrix A matrix that represents the cameras position and orientation in world space.
* @return None.
* @see MFView_GetCameraMatrix()
*/
extern (C) void MFView_SetCameraMatrix(ref const(MFMatrix) cameraMatrix);

/**
* Get the view stateblock.
* Produce an MFStateBlock for the current view state.
* @return Returns an MFStateBlock which represents the current view state.
* @see MFStateBlock_Create()
*/
extern (C) const MFStateBlock* MFView_GetViewState();

/**
* Gets the camera matrix.
* Gets the current views camera matrix.
* @return Const ref to the current camera matrix.
* @see MFView_SetCameraMatrix()
*/
extern (C) const(MFMatrix)* MFView_GetCameraMatrix();

/**
* Gets the World to View matrix.
* Gets the current views World to View transformation matrix.
* @return Const ref to the current World to View matrix.
* @see MFView_GetViewToScreenMatrix()
* @see MFView_GetWorldToScreenMatrix()
*/
extern (C) const(MFMatrix)* MFView_GetWorldToViewMatrix();

/**
* Gets the View to Screen matrix.
* Gets the current views View to Screen transformation matrix.
* @return Const ref to the current View to Screen matrix.
* @see MFView_GetWorldToViewMatrix()
* @see MFView_GetWorldToScreenMatrix()
*/
extern (C) const(MFMatrix)* MFView_GetViewToScreenMatrix();

/**
* Gets the World to Screen matrix.
* Gets the current views World to Screen transformation matrix.
* @return Const ref to the current World to Screen matrix.
* @see MFView_GetWorldToViewMatrix()
* @see MFView_GetViewToScreenMatrix()
*/
extern (C) const(MFMatrix)* MFView_GetWorldToScreenMatrix();

/**
* Builds a Local to Screen matrix.
* Builds a Local to Screen matrix from the provided Local matrix.
* @param localToWorld Local to world matrix to be used to generate the Local to Screen matrix.
* @param pOutput Pointer to a matrix that receives the result of the operation.
* @return The same pointer passed as pOutput, This way MFView_GetLocalToScreen() can be used as a parameter to another function.
* @see MFView_GetWorldToScreenMatrix()
* @see MFView_GetLocalToView()
*/
extern (C) MFMatrix* MFView_GetLocalToScreen(const ref MFMatrix localToWorld, MFMatrix *pOutput);

/**
* Builds a Local to View matrix.
* Builds a Local to View matrix from the provided Local matrix.
* @param localToWorld Local to world matrix to be used to generate the Local to Screen matrix.
* @param pOutput Pointer to a matrix that receives the result of the operation.
* @return The same pointer passed as pOutput, This way MFView_GetLocalToView() can be used as a parameter to another function.
* @see MFView_GetWorldToViewMatrix()
* @see MFView_GetLocalToScreen()
*/
extern (C) MFMatrix* MFView_GetLocalToView(const ref MFMatrix localToWorld, MFMatrix *pOutput);


// not sure how to do these, since we dont store either matrix, and we only have one available at a time...
extern (C) void MFView_TransformPoint2DTo3D(const ref MFVector point, MFVector *pResult, MFVector *pResultRayDir);
extern (C) void MFView_TransformPoint3DTo2D(const ref MFVector point, MFVector *pResult);

