#if !defined(_RENDERER_H)
#define _RENDERER_H

class Matrix;
struct Material;
struct MFMeshChunk;

// renderer interface

// init/deinit
void Renderer_InitModule();
void Renderer_DeinitModule();

// renderer setup
int  Renderer_Begin(); // returns number of passes remaining..

// renderer setup
//void Renderer_SetLocalToWorldMatrix(const Matrix &localToWorldMatrix); // Matrix[0] == localToWorld (use SetMatrices)
void Renderer_SetMatrices(Matrix *pMatrices, int numMatrices);

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

// enums
enum RenderSource
{
	RS_Unknown = 0,

	RS_MeshChunk,
	RS_ImmediateMode,
	RS_Custom
};

enum RenderState
{
	RS_MaterialOverride,
	RS_NoZRead,
	RS_NoZWrite,
	RS_Untextured,
	RS_PreLit,
	RS_PrimType,

	RS_ShowZBuffer,
	RS_ShowOverDraw,

	RS_Max
};

// deprecated
void Renderer_SetRenderer(uint32 additionalRendererFlags, uint32 flags = NULL, uint32 renderSource = RS_Custom);

#endif
