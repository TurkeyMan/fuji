/**
 * @file MFPrimitive.h
 * @brief Provides access to a basic immediate mode renderer.
 * @author Manu Evans
 * @defgroup MFPrimitive Immediate Mode Interface
 * @{
 */

#if !defined(_MFPRIMITIVE_H)
#define _MFPRIMITIVE_H

#include "MFVector.h"
#include "MFMatrix.h"

/**
 * Primitive Type for an immediate render block.
 * Primitive Type for an immediate render block.
 */
enum PrimType
{
	PT_PointList,		/**< Point List */
	PT_LineList,		/**< Line List */
	PT_LineStrip,		/**< List Strip */
	PT_TriList,			/**< Triangle List */
	PT_TriStrip,		/**< Triangle Strip */
	PT_TriFan,			/**< Triangle Fan */
	PT_PrimMask = 0x07,	/**< Primitive type mask */

	PT_Prelit = 64,		/**< Instructs the renderer to disable lighting */
	PT_Untextured = 128	/**< Instructs the renderer to disable texturing */
};

/**
 * Begin a primitive block.
 * Begins a primitive block.
 * @param primType Type of primitives to render.
 * @param hint Hints to pass to the immediate renderer.
 * @return None.
 */
void MFPrimitive(uint32 primType, uint32 hint = 0);

/**
 * Begin a vertex submition block.
 * Begins a vertex submition block.
 * @param vertexCount Number of vertices to be rendered.
 * @return None.
 */
void MFBegin(uint32 vertexCount);

/**
 * Set the world matrix for the primitive block.
 * Sets the world matrix for the primitive block.
 * @param mat Matrix to set as the current world matrix.
 * @return None.
 */
void MFSetMatrix(const MFMatrix &mat);

/**
 * Set the current vertex colour.
 * Sets the current vertex colour.
 * @param colour The colour to set for all proceeding vertices.
 * @return None.
 */
void MFSetColour(const MFVector &colour);

/**
 * Set the current vertex colour.
 * Sets the current vertex colour.
 * @param r The red component to set for all proceeding vertices.
 * @param g The red component to set for all proceeding vertices.
 * @param b The red component to set for all proceeding vertices.
 * @param a The red component to set for all proceeding vertices.
 * @return None.
 */
void MFSetColour(float r, float g, float b, float a = 1.0f);

/**
 * Set the current vertex colour.
 * Sets the current vertex colour.
 * @param col The colour to set for all proceeding vertices as a platform specific 32bit packed format.
 * @return None.
 */
void MFSetColour(uint32 col);

/**
 * Set the current texture coordinate.
 * Sets the current texture coordinate.
 * @param u The u coordinate to set for all proceeding vertices.
 * @param v The v coordinate to set for all proceeding vertices.
 * @return None.
 */
void MFSetTexCoord1(float u, float v);

/**
 * Set the current vertex normal.
 * Sets the current vertex normal.
 * @param normal The vertex normal to use for all proceeding vertices.
 * @return None.
 */
void MFSetNormal(const MFVector &normal);

/**
 * Set the current vertex normal.
 * Sets the current vertex normal.
 * @param x The vertex normal x component.
 * @param y The vertex normal y component.
 * @param z The vertex normal z component.
 * @return None.
 */
void MFSetNormal(float x, float y, float z);

/**
 * Set the current vertex position.
 * Sets the current vertex position.
 * @param pos The position of the next vertex.
 * @return None.
 * @remarks Setting the vertex position applies all current values to the current vertex and moves to the next vertex.
 */
void MFSetPosition(const MFVector &pos);

/**
 * Set the current vertex position.
 * Sets the current vertex position.
 * @param x The x coordinate of the next vertex.
 * @param y The y coordinate of the next vertex.
 * @param z The z coordinate of the next vertex.
 * @return None.
 * @remarks Setting the vertex position applies all current values to the current vertex and moves to the next vertex.
 */
void MFSetPosition(float x, float y, float z);

/**
 * End the vertex submition block.
 * Ends the vertex submition block.
 * @return None.
 */
void MFEnd();

#endif // _MFPRIMITIVE_H

/** @} */
