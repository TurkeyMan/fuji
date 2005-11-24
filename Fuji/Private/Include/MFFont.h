/**
 * @file MFFont.h
 * @brief Font rendering interface.
 * @author Manu Evans
 * @defgroup MFFont Font Rendering
 * @{
 */

#if !defined(_MFFONT_H)
#define _MFFONT_H

#include "MFVector.h"

// windows decides to steal these names for its self..
#if defined(DrawText)
#undef DrawText
#endif

/**
 * @struct MFFont
 * Represents a Fuji Font.
 */
struct MFFont;

// member functions

/**
 * Create a font.
 * Creates a font from the filesystem.
 * @param pFilename Filename of the font to load.
 * @return Pointer to an MFFont structure representing the newly created font.
 * @see MFFont_Destroy()
 */
MFFont* MFFont_Create(const char *pFilename);

/**
 * Destroy a font.
 * Destroys a font from the filesystem.
 * @param pFont Pointer to a font to destroy.
 * @return None.
 * @see MFFont_Create()
 */
void MFFont_Destroy(MFFont *pFont);

/**
 * Render a string.
 * Renders text to the screen.
 * @param pFont Pointer to a font.
 * @param pos Position in world space to render the string.
 * @param height Height of the text.
 * @param colour Colour of the text.
 * @param pText Pointer to a string containing the text to render.
 * @return Returns 0 if there were no errors.
 */
int MFFont_DrawText(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pText);

/**
 * Render a string.
 * Renders text to the screen.
 * @param pFont Pointer to a font.
 * @param x X coordinate in world space to render the string.
 * @param y Y coordinate in world space to render the string.
 * @param height Height of the text.
 * @param colour Colour of the text.
 * @param pText Pointer to a string containing the text to render.
 * @return Returns 0 if there were no errors.
 */
int MFFont_DrawText(MFFont *pFont, float x, float y, float height, const MFVector &colour, const char *pText);

/**
 * Render a formatted string.
 * Renders formatted text to the screen.
 * @param pFont Pointer to a font.
 * @param pos Position in world space to render the string.
 * @param height Height of the text.
 * @param colour Colour of the text.
 * @param pFormat Pointer to the format string. This complies with all the regular printf format standards.
 * @param ... Additional paramaters used according to the format specifier.
 * @return Returns 0 if there were no errors.
 * @remarks The MFFont_DrawTextf format string conforms with all the standard printf format standards.
 */
int MFFont_DrawTextf(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pFormat, ...);

/**
 * Render a formatted string.
 * Renders formatted text to the screen.
 * @param pFont Pointer to a font.
 * @param x X coordinate in world space to render the string.
 * @param y Y coordinate in world space to render the string.
 * @param height Height of the text.
 * @param colour Colour of the text.
 * @param pFormat Pointer to the format string. This complies with all the regular printf format standards.
 * @param ... Additional paramaters used according to the format specifier.
 * @return Returns 0 if there were no errors.
 * @remarks The MFFont_DrawTextf format string conforms with all the standard printf format standards.
 */
int MFFont_DrawTextf(MFFont *pFont, float x, float y, float height, const MFVector &colour, const char *pFormat, ...);

/**
 * Get the debug font.
 * Gets a pointer to the debug font.
 * @return Returns a pointer to the debug font.
 */
MFFont* MFFont_GetDebugFont();

#endif // _MFFONT_H

/** @} */
