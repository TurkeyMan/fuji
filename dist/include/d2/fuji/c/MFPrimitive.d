module fuji.c.MFPrimitive;

import fuji.c.MFTypes;
import fuji.c.MFMaterial;
import fuji.c.MFMatrix;
import fuji.c.MFRenderState: MFStateBlock;

nothrow:
@nogc:

/**
* Primitive Type for an immediate render block.
* Primitive Type for an immediate render block.
*/
enum PrimType : uint
{
	PointList,			/**< Point List */
	LineList,			/**< Line List */
	LineStrip,			/**< List Strip */
	TriList,			/**< Triangle List */
	TriStrip,			/**< Triangle Strip */
	TriFan,				/**< Triangle Fan */
	QuadList,			/**< Quad List */
	PrimMask = 0x07,	/**< Primitive type mask */

	Prelit = 64,			/**< Instructs the renderer to disable lighting */
	Untextured = 128,		/**< Instructs the renderer to disable texturing */
	LargeStateBlock = 256	/**< Allocate a large mesh stateblock, sufficient to store user data */
}

/**
* Begin a primitive block.
* Begins a primitive block.
* @param primType Type of primitives to render.
* @param hint Hints to pass to the immediate renderer.
* @return None.
*/
extern (C) void MFPrimitive(PrimType primType, uint hint = 0);

/**
* Begin a vertex submition block.
* Begins a vertex submition block.
* @param vertexCount Number of vertices to be rendered.
* @return None.
*/
extern (C) void MFBegin(uint vertexCount);

/**
* Set the world matrix for the primitive block.
* Sets the world matrix for the primitive block.
* @param mat Matrix to set as the current world matrix.
* @return None.
*/
extern (C) void MFSetMatrix(const ref MFMatrix mat);

/**
* Set the current vertex colour.
* Sets the current vertex colour.
* @param r The red component to set for all proceeding vertices.
* @param g The red component to set for all proceeding vertices.
* @param b The red component to set for all proceeding vertices.
* @param a The red component to set for all proceeding vertices.
* @return None.
*/
extern (C) void MFSetColour(float r, float g, float b, float a = 1.0f);

/**
* Set the current vertex colour.
* Sets the current vertex colour.
* @param colour The colour to set for all proceeding vertices.
* @return None.
*/
/+inline+/ void MFSetColourV(const ref MFVector colour)
{
	MFSetColour(colour.x, colour.y, colour.z, colour.w);
}

/**
* Set the current texture coordinate.
* Sets the current texture coordinate.
* @param u The u coordinate to set for all proceeding vertices.
* @param v The v coordinate to set for all proceeding vertices.
* @return None.
*/
extern (C) void MFSetTexCoord1(float u, float v);

/**
* Set the current vertex normal.
* Sets the current vertex normal.
* @param x The vertex normal x component.
* @param y The vertex normal y component.
* @param z The vertex normal z component.
* @return None.
*/
extern (C) void MFSetNormal(float x, float y, float z);

/**
* Set the current vertex normal.
* Sets the current vertex normal.
* @param normal The vertex normal to use for all proceeding vertices.
* @return None.
*/
/+inline+/ void MFSetNormalV(const ref MFVector normal)
{
	MFSetNormal(normal.x, normal.y, normal.z);
}

/**
* Set the current vertex position.
* Sets the current vertex position.
* @param x The x coordinate of the next vertex.
* @param y The y coordinate of the next vertex.
* @param z The z coordinate of the next vertex.
* @return None.
* @remarks Setting the vertex position applies all current values to the current vertex and moves to the next vertex.
*/
extern (C) void MFSetPosition(float x, float y, float z);

/**
* Set the current vertex position.
* Sets the current vertex position.
* @param pos The position of the next vertex.
* @return None.
* @remarks Setting the vertex position applies all current values to the current vertex and moves to the next vertex.
*/
/+inline+/ void MFSetPositionV(const ref MFVector pos)
{
	MFSetPosition(pos.x, pos.y, pos.z);
}

/**
* End the vertex submition block.
* Ends the vertex submition block.
* @return None.
*/
extern (C) void MFEnd();

/**
* Get the primitive stateblock.
* Gets the mesh stateblock for the current MFPrimitive scope.
* @return Pointer to a MFStateBlock used by the current primitive block's mesh.
* @see MFStateBlock, MFStateBlock_Create()
*/
extern (C) MFStateBlock* MFPrimitive_GetPrimStateBlock();

/**
* Begin a blitter block.
* Begins a blitter block.
* @param numBlits Number of blit's that will be performed in this block.
* @return None.
*/
extern (C) void MFPrimitive_BeginBlitter(int numBlits);

/**
* Blit an image or part of an image to the render target.
* Blits an image or part of an image to the render target.
* @param x The x coordinate, in screen space, where the image will be drawn.
* @param y The y coordinate, in screen space, where the image will be drawn.
* @param tx The x texel coordinate of the source image.
* @param ty The y texel coordinate of the source image.
* @param tw The width, in texels, if the image to be blitted.
* @param th The height, in texels, if the image to be blitted.
* @return None.
*/
extern (C) void MFPrimitive_Blit(int x, int y, int tx, int ty, int tw, int th);

/**
* Blit an image or part of an image to the render target with scaling.
* Blits an image or part of an image to the render target with scaling.
* @param x The x coordinate, in screen space, where the image will be drawn.
* @param y The y coordinate, in screen space, where the image will be drawn.
* @param uvs An MFRect containing and segment of the image to blit.
* @return None.
*/
extern (C) void MFPrimitive_BlitRect(int x, int y, const ref MFRect uvs);

/**
* Blit an image or part of an image to the render target with scaling.
* Blits an image or part of an image to the render target with scaling.
* @param x The x coordinate, in screen space, where the image will be drawn.
* @param y The y coordinate, in screen space, where the image will be drawn.
* @param w The width, in pixels, of the image to be drawn.
* @param h The width, in pixels, of the image to be drawn.
* @param tx The x texel coordinate of the source image.
* @param ty The y texel coordinate of the source image.
* @param tw The width, in texels, if the image to be blitted.
* @param th The height, in texels, if the image to be blitted.
* @return None.
*/
extern (C) void MFPrimitive_StretchBlit(int x, int y, int w, int h, int tx, int ty, int tw, int th);

/**
* Blit an image or part of an image to the render target with scaling.
* Blits an image or part of an image to the render target with scaling.
* @param x The x coordinate, in screen space, where the image will be drawn.
* @param y The y coordinate, in screen space, where the image will be drawn.
* @param w The width, in pixels, of the image to be drawn.
* @param h The width, in pixels, of the image to be drawn.
* @param uvs An MFRect containing and segment of the image to blit.
* @return None.
*/
extern (C) void MFPrimitive_StretchBlitRect(int x, int y, int w, int h, const ref MFRect uvs);

/**
* End a blitter block.
* Ends a blitter block.
* @return None.
*/
extern (C) void MFPrimitive_EndBlitter();

/**
* Draw a textured square.
* Draws a textured square.
* @param x X coordinate of square.
* @param y Y coordinate of square.
* @param w Width of square.
* @param h Height of square.
* @param colour Colour of square.
* @param su Source U coordinate.
* @param sv Source V coordinate.
* @param du Destination U coordinate.
* @param dv Destination V coordinate.
* @return None.
*/
extern (C) void MFPrimitive_DrawQuad(float x, float y, float w, float h, const ref MFVector colour = MFVector.one, float su=0.0f, float sv=0.0f, float du=1.0f, float dv=1.0f, const ref MFMatrix mat = MFMatrix.identity);

/**
* Draw a textured square.
* Draws a textured square.
* @param min Min coordinate of the square.
* @param max Max coordinate of the square.
* @param colour Colour of square.
* @param su Source U coordinate.
* @param sv Source V coordinate.
* @param du Destination U coordinate.
* @param dv Destination V coordinate.
* @return None.
*/
extern (C) void MFPrimitive_DrawQuadV(const ref MFVector min, const ref MFVector max, const ref MFVector colour = MFVector.one, float su=0.0f, float sv=0.0f, float du=1.0f, float dv=1.0f, const ref MFMatrix mat = MFMatrix.identity);

/**
* Draw an untextured square.
* Draws an untextured square.
* @param x X coordinate of square.
* @param y Y coordinate of square.
* @param w Width of square.
* @param h Height of square.
* @param colour Colour of square.
* @return None.
*/
extern (C) void MFPrimitive_DrawUntexturedQuad(float x, float y, float w, float h, const ref MFVector colour = MFVector.one, const ref MFMatrix mat = MFMatrix.identity);

/**
* Draw an untextured square.
* Draws an untextured square.
* @param min Min coordinate of the square.
* @param max Max coordinate of the square.
* @param colour Colour of square.
* @return None.
*/
extern (C) void MFPrimitive_DrawUntexturedQuadV(const ref MFVector min, const ref MFVector max, const ref MFVector colour = MFVector.one, const ref MFMatrix mat = MFMatrix.identity);

/**
* Draw a box.
* Draws a box.
* @param boxMin Min coordinate of the box.
* @param boxMax Max coordinate of the box.
* @param colour Colour of box.
* @param mat World matrix for rendering.
* @param wireframe Renders the box in wireframe.
* @return None.
*/
extern (C) void MFPrimitive_DrawBox(const ref MFVector boxMin, const ref MFVector boxMax, const ref MFVector colour = MFVector.one, const ref MFMatrix mat = MFMatrix.identity, bool wireframe = true);

/**
* Draw a sphere.
* Draws a sphere.
* @param position Position of the sphere.
* @param radius Radius of the sphere.
* @param segments Number of segments (Subdivisions around the sphere).
* @param slices Number of slices (Subdivisions down the sphere).
* @param colour Colour of sphere.
* @param mat World matrix for rendering.
* @param wireframe Renders the sphere in wireframe.
* @return None.
*/
extern (C) void MFPrimitive_DrawSphere(const ref MFVector position, float radius, int segments = 8, int slices = 5, const ref MFVector colour = MFVector.one, const ref MFMatrix mat = MFMatrix.identity, bool wireframe = true);

/**
* Draw a capsule.
* Draws a capsule.
* @param startPoint Point in the center of the first end sphere.
* @param endPoint Point in the center of the second end sphere.
* @param radius Radius of the capsule.
* @param segments Number of segments (Subdivisions around the capsule).
* @param slices Number of slices (Subdivisions down the capsule).
* @param colour Colour of capsule.
* @param mat World matrix for rendering.
* @param wireframe Renders the capsule in wireframe.
* @return None.
*/
extern (C) void MFPrimitive_DrawCapsule(const ref MFVector startPoint, const ref MFVector endPoint, float radius, int segments = 8, int slices = 5, const ref MFVector colour = MFVector.one, const ref MFMatrix mat = MFMatrix.identity, bool wireframe = true);

/**
* Draw a cylinder.
* Draws a cylinder.
* @param startPoint Point in the center of the first end.
* @param endPoint Point in the center of the second end.
* @param radius Radius of the cylinder.
* @param segments Number of segments (Subdivisions around the cylinder).
* @param slices Number of slices (Subdivisions down the cylinder).
* @param colour Colour of cylinder.
* @param mat World matrix for rendering.
* @param wireframe Renders the cylinder in wireframe.
* @return None.
*/
extern (C) void MFPrimitive_DrawCylinder(const ref MFVector startPoint, const ref MFVector endPoint, float radius, int segments = 8, int slices = 5, const ref MFVector colour = MFVector.one, const ref MFMatrix mat = MFMatrix.identity, bool wireframe = true);

/**
* Draw a disc representing a plane.
* Draws a disc representing a plane.
* @param point Point on the plane where the plane will be drawn.
* @param normal The planes	normal vector.
* @param span How far the plane spans from the point.
* @param colour Colour of plane.
* @param mat World matrix for plane.
* @param wireframe Renders the plane in wireframe.
* @return None.
*/
extern (C) void MFPrimitive_DrawPlane(const ref MFVector point, const ref MFVector normal, float span, const ref MFVector colour = MFVector.one, const ref MFMatrix mat = MFMatrix.identity, bool wireframe = true);

/**
* Draw a cone.
* Draws a cone.
* @param base Point in the center of the base.
* @param point The tip of the cone.
* @param radius Radius of the base of the cone.
* @param segments Number of segments (Subdivisions around the cone).
* @param colour Colour of cone.
* @param mat World matrix for rendering.
* @param wireframe Renders the cone in wireframe.
* @return None.
*/
extern (C) void MFPrimitive_DrawCone(const ref MFVector base, const ref MFVector point, float radius, int segments = 8, const ref MFVector colour = MFVector.one, const ref MFMatrix mat = MFMatrix.identity, bool wireframe = true);

/**
* Draw an arrow.
* Draws an arrow.
* @param pos Point at the base of the arrow.
* @param dir Direction of the arrow.
* @param length The length of the arrow.
* @param radius Radius of the arrow.
* @param colour Colour of the arrow.
* @param mat World matrix for rendering.
* @param wireframe Renders the arrow in wireframe.
* @return None.
*/
extern (C) void MFPrimitive_DrawArrow(const ref MFVector pos, const ref MFVector dir, float length, float radius, const ref MFVector colour = MFVector.one, const ref MFMatrix mat = MFMatrix.identity, bool wireframe = false);

/**
* Draw a transform.
* Draws a transform used for visualising a matrix.
* @param mat Matrix representing the transform to render.
* @param scale Scale for the transform.
* @param lite Renders the transform as axis lines as lines instead of solid cylinders.
* @return None.
*/
extern (C) void MFPrimitive_DrawTransform(const ref MFMatrix mat, float scale, bool lite = false);

