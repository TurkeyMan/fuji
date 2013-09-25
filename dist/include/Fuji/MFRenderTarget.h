/**
 * @file MFRenderTarget.h
 * @brief Render target interface.
 * @author Manu Evans
 * @defgroup MFRenderTarget Render Target Management
 * @{
 */

#pragma once
#if !defined(_MFRENDERTARGET_H)
#define _MFRENDERTARGET_H

#include "MFTexture.h"

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
		MFImageFormat format;		/**< Image format of the surface, or ImgFmt_Unknown if no surface should be created for this target. */
		MFTexture *pSurface;		/**< MFTexture to use for this target surface, or NULL to create a surface with \a format, or if no surface should be used for this target. */

		TargetDesc() : format(ImgFmt_Unknown), pSurface(NULL) {}
	};

	const char *pName;				/**< A name for the render target. */

	int width;						/**< Width of the render target. */
	int height;						/**< Height of the render target. */

	TargetDesc colourTargets[8];	/**< Colour targets. */
	TargetDesc depthStencil;		/**< Depth/stencil target. */

	MFRenderTargetDesc() : width(0), height(0) {}
};

// interface functions

/**
 * Create a render target.
 * Creates a render target object.
 * @param pDesc Pointer to an \a MFRenderTargetDesc that describes the render target.
 * @return Pointer to an MFRenderTarget representing the newly created render target.
 * @see MFRenderTarget_CreateSimple(), MFRenderTarget_Release()
 */
MF_API MFRenderTarget* MFRenderTarget_Create(MFRenderTargetDesc *pDesc);

/**
 * Create a simple render target.
 * Creates a simple render target object.
 * @param pName A name for the render target.
 * @param width Width of the render target.
 * @param height Height of the render target.
 * @param colourTargetFormat Format for the colour target. This may be any format supported but the platform, a combination of flags from the ImgFmt_Select* formats, or ImgFmt_Unknown if no colour target should be used.
 * @param depthStencilFormat Format for the depth/stencil target. This may be any depth format supported but the platform, a combination of flags from the ImgFmt_Select* formats, or ImgFmt_Unknown if no depth/stencil target should be used.
 * @return Pointer to an MFRenderTarget representing the newly created render target.
 * @see MFRenderTarget_Create(), MFRenderTarget_Release()
 */
MF_API MFRenderTarget* MFRenderTarget_CreateSimple(const char *pName, int width, int height, MFImageFormat colourTargetFormat = ImgFmt_SelectRenderTarget, MFImageFormat depthStencilFormat = ImgFmt_SelectDepth);

/**
 * Release an MFRenderTarget instance.
 * Release a reference to an MFRenderTarget and destroy when the reference count reaches 0.
 * @param pRenderTarget Render target instance to release.
 * @return The new reference count of the render target. If the returned reference count is 0, the render target is destroyed.
 * @see MFRenderTarget_Create()
 */
MF_API int MFRenderTarget_Release(MFRenderTarget *pRenderTarget);

/**
 * Get a colour target from a render target.
 * Gets a colour target from a render target.
 * @param pRenderTarget Pointer to a render target.
 * @param index Index of the colour target.
 * @return An MFTexture for the colour target, or NULL if no colour target is set.
 */
MF_API MFTexture* MFRenderTarget_GetColourTarget(MFRenderTarget *pRenderTarget, int index);

/**
 * Get the depth/stencil target from a render target.
 * Gets the depth/stencil target from a render target.
 * @param pRenderTarget Pointer to a render target.
 * @return An MFTexture for the depth/stencil target, or NULL if no depth/stencil target is set.
 */
MF_API MFTexture* MFRenderTarget_GetDepthStencil(MFRenderTarget *pRenderTarget);

// C++ API
#include "Fuji/MFResource.h"

namespace Fuji
{
	class RenderTarget : public Resource
	{
	public:
/*
		struct Dimensions
		{
			int width, height;
		};

		inline Texture()											{}
		inline Texture(const Texture &from)							: Resource(from) {}
		inline Texture(MFTexture *pFrom)							: Resource((MFResource*)pFrom) {}
		inline Texture(const char *pName)							{ pResource = (MFResource*)MFTexture_Create(pName); }

		inline Texture& operator=(const Texture& from)
		{
			Release();
			pResource = from.pResource;
			AddRef();
			return *this;
		}

		inline operator MFTexture*()								{ return (MFTexture*)pResource; }

		inline Texture& Create(const char *pName)
		{
			Release();
			pResource = (MFResource*)MFTexture_Create(pName);
			return *this;
		}

		inline Dimensions Size() const								{ Dimensions dim; MFTexture_GetTextureDimensions((MFTexture*)pResource, &dim.width, &dim.height); return dim; }
*/
	};
}

#endif // _MFRENDERTARGET_H

/** @} */
