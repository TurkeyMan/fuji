#if !defined(_RENDERER_H)
#define _RENDERER_H

class MFMatrix;
struct MFMeshChunk;

// renderer interface

// renderer setup
int  Renderer_Begin(); // returns number of passes remaining..

// renderer setup
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
