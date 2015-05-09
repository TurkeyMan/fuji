module fuji.c.MFRenderTarget;

import fuji.c.MFTexture;
import fuji.c.MFResource;

nothrow:
@nogc:

/**
 * @struct MFRenderTarget
 * Represents a Fuji render target.
 */
struct MFRenderTarget;

/**
 * Render target description.
 * Describes a render target.
 */
struct MFRenderTargetDesc
{
	/**
	 * Surface description.
	 * Describes target surface.
	 */
	struct TargetDesc
	{
		MFImageFormat format = MFImageFormat.Unknown;	/**< Image format of the surface, or MFImageFormat.Unknown if no surface should be created for this target. */
		MFTexture* pSurface;					/**< MFTexture to use for this target surface, or NULL to create a surface with \a format, or if no surface should be used for this target. */
	}

	const(char)* pName;				/**< A name for the render target. */

	int width;						/**< Width of the render target. */
	int height;						/**< Height of the render target. */

	TargetDesc[8] colourTargets;	/**< Colour targets. */
	TargetDesc depthStencil;		/**< Depth/stencil target. */
}

// interface functions

/**
 * Create a render target.
 * Creates a render target object.
 * @param pDesc Pointer to an \a MFRenderTargetDesc that describes the render target.
 * @return Pointer to an MFRenderTarget representing the newly created render target.
 * @see MFRenderTarget_CreateSimple(), MFRenderTarget_Release()
 */
extern (C) MFRenderTarget* MFRenderTarget_Create(MFRenderTargetDesc* pDesc);

/**
 * Create a simple render target.
 * Creates a simple render target object.
 * @param pName A name for the render target.
 * @param width Width of the render target.
 * @param height Height of the render target.
 * @param colourTargetFormat Format for the colour target. This may be any format supported but the platform, a combination of flags from the MFImageFormat.Select* formats, or MFImageFormat.Unknown if no colour target should be used.
 * @param depthStencilFormat Format for the depth/stencil target. This may be any depth format supported but the platform, a combination of flags from the MFImageFormat.Select* formats, or MFImageFormat.Unknown if no depth/stencil target should be used.
 * @return Pointer to an MFRenderTarget representing the newly created render target.
 * @see MFRenderTarget_Create(), MFRenderTarget_Release()
 */
extern (C) MFRenderTarget* MFRenderTarget_CreateSimple(const(char)* pName, int width, int height, MFImageFormat colourTargetFormat = MFImageFormat.SelectRenderTarget, MFImageFormat depthStencilFormat = MFImageFormat.SelectDepth);

/**
 * Release an MFRenderTarget instance.
 * Release a reference to an MFRenderTarget and destroy when the reference count reaches 0.
 * @param pRenderTarget Render target instance to release.
 * @return The new reference count of the render target. If the returned reference count is 0, the render target is destroyed.
 * @see MFRenderTarget_Create()
 */
extern (C) int MFRenderTarget_Release(MFRenderTarget* pRenderTarget);

/**
 * Get a colour target from a render target.
 * Gets a colour target from a render target.
 * @param pRenderTarget Pointer to a render target.
 * @param index Index of the colour target.
 * @return An MFTexture for the colour target, or NULL if no colour target is set.
 */
extern (C) MFTexture* MFRenderTarget_GetColourTarget(MFRenderTarget* pRenderTarget, int index);

/**
 * Get the depth/stencil target from a render target.
 * Gets the depth/stencil target from a render target.
 * @param pRenderTarget Pointer to a render target.
 * @return An MFTexture for the depth/stencil target, or NULL if no depth/stencil target is set.
 */
extern (C) MFTexture* MFRenderTarget_GetDepthStencil(MFRenderTarget* pRenderTarget);

/** @} */
