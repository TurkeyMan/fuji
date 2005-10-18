#if !defined(_RENDERER_H)
#define _RENDERER_H

#include "MFMatrix.h"
struct MFMeshChunk;

// Renderer Interface

// Renderer Enums
enum MatrixType
{
	MFMT_Unknown,

	// these can be set directly
	MFMT_WorldMatrix,
	MFMT_CameraMatrix,
	MFMT_ProjectionMatrix,

	// these can not be set directly, only read
	MFMT_ViewMatrix,
	MFMT_WorldViewMatrix,
	MFMT_ViewProjectionMatrix,
	MFMT_WorldViewProjectionMatrix,

	MFMT_Max,
	MFMT_ForceInt = 0x7FFFFFFF
};

// Renderer Setup
int  Renderer_Begin(); // returns number of passes remaining..

// Get and Set the matrices used for geometry transformation
const MFMatrix& MKRenderer_GetMatrix(MatrixType type, MFMatrix *pMatrix);
void MKRenderer_SetMatrix(MatrixType type, const MFMatrix &matrix);

//void Renderer_SetLocalToWorldMatrix(const Matrix &localToWorldMatrix); // Matrix[0] == localToWorld (use SetMatrices)
void Renderer_SetMatrices(MFMatrix *pMatrices, int numMatrices);

// render a mesh chunk
void Renderer_RenderMeshChunk(MFMeshChunk *pMeshChunk);

// immediate mode renderer
int  Renderer_BeginImmediate(uint32 primType);
int  Renderer_RenderVertices(int numVertices);
void Renderer_EndVertices();
void Renderer_EndImmediate();

// renderer settings
uint32 Renderer_SetRendererOverride(uint32 renderState, uint32 value);
uint32 Renderer_GetRendererOverride(uint32 renderState);

#endif
