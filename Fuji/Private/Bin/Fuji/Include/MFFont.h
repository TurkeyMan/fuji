/**
 * @file MFFont.h
 * @brief Font rendering interface.
 * @author Manu Evans
 * @defgroup MFFont Font Rendering
 * Provides a set of functions to perform text rendering tasks.
 * Fuji Fonts are generated from source data produced by the excellent BMFont tool written and maintained by Andreas Jönsson.\n
 * The tool can be downloaded here: http://www.angelcode.com/products/bmfont/\n
 * Basic unicode support is available by submitting unicode strings beginning with the unicode character 0xFEFF to the standard ascii versions of these functions.\n
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

/**
 * Font Justification.
 * These are the available text justifications.
 */
enum MFFontJustify
{
	MFFontJustify_Top_Left,				/**< Justify top left. */
	MFFontJustify_Top_Center,			/**< Justify top center. */
	MFFontJustify_Top_Right,			/**< Justify top right. */
	MFFontJustify_Top_Full,				/**< Justify top full. */

	MFFontJustify_Center_Left,			/**< Justify center left. */
	MFFontJustify_Center,				/**< Justify center. */
	MFFontJustify_Center_Right,			/**< Justify center right. */
	MFFontJustify_Center_Full,			/**< Justify center full. */

	MFFontJustify_Bottom_Left,			/**< Justify bottom left. */
	MFFontJustify_Bottom_Center,		/**< Justify bottom center. */
	MFFontJustify_Bottom_Right,			/**< Justify bottom right. */
	MFFontJustify_Bottom_Full,			/**< Justify bottom full. */

	MFFontJustify_Max,					/**< Max justifications. */
	MFFontJustify_ForceInt = 0x7FFFFFFF	/**< Force MFFontJustify to an int value. */
};

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
 * Get the height of a font.
 * Gets the native height of a font.
 * @param pFont Pointer to a font.
 * @return The native height (in texels) of the specified font.
 * @see MFFont_Create()
 */
float MFFont_GetFontHeight(MFFont *pFont);

/**
 * Get the width of a character.
 * Gets the native width of a character in a font.
 * @param pFont Pointer to a font.
 * @param character Character to get the width for.
 * @return The native width (in texels) of the specified character.
 * @see MFFont_Create()
 */
float MFFont_GetCharacterWidth(MFFont *pFont, int character);

/**
 * Get the character offset into a string.
 * Gets the offset of the specified character from the string's starting position.
 * @param pFont Pointer to a font.
 * @param pText Text containing the target character.
 * @param charIndex Index of the character in the string to find the position of.
 * @param height Height of the text.
 * @return The physical offset of the specified character in the string from the string starting position.
 * @see MFFont_GetStringWidth()
 */
MFVector MFFont_GetCharPos(MFFont *pFont, const char *pText, int charIndex, float height);

/**
 * Get the physical width of a string.
 * Gets the physical width of a string.
 * @param pFont Pointer to a font.
 * @param pText Text to get the width for.
 * @param height Height of the text (width is relative to the height).
 * @param lineWidth The maximum line width before the string is wrapped onto a new line.
 * @param maxLen Maximum number of characters to consider from the source string.
 * @param pTotalHeight Optional pointer to a float that receives the total height of the specified string.
 * @return The physical width of the specified string rendered by the specified font.
 * @see MFFont_Create()
 */
float MFFont_GetStringWidth(MFFont *pFont, const char *pText, float height, float lineWidth = 0.0f, int maxLen = -1, float *pTotalHeight = NULL);

/**
 * Blit a string to the screen.
 * Renders text to the screen (perfect texel to pixel mapping, no scaling).
 * @param pFont Pointer to a font.
 * @param x x coordinate in screen space.
 * @param y y coordinate in screen space.
 * @param colour Colour of the text.
 * @param pText Pointer to a string containing the text to render.
 * @param maxChars Maximum number of chars to render from the source string.
 * @return Returns 0 if there were no errors.
 */
int MFFont_BlitText(MFFont *pFont, int x, int y, const MFVector &colour, const char *pText, int maxChars = -1);

/**
 * Blit a formatted string to the screen.
 * Blits formatted text to the screen (perfect texel to pixel mapping, no scaling).
 * @param pFont Pointer to a font.
 * @param x x coordinate in screen space.
 * @param y y coordinate in screen space.
 * @param colour Colour of the text.
 * @param pFormat Pointer to the format string. This complies with all the regular printf format standards.
 * @param ... Additional parameters used according to the format specifier.
 * @return Returns 0 if there were no errors.
 * @remarks The MFFont_BlitTextf format string conforms with all the standard printf format standards.
 */
int MFFont_BlitTextf(MFFont *pFont, int x, int y, const MFVector &colour, const char *pFormat, ...);

/**
 * Render a string.
 * Renders text to the screen.
 * @param pFont Pointer to a font.
 * @param pos Position in world space to render the string.
 * @param height Height of the text.
 * @param colour Colour of the text.
 * @param pText Pointer to a string containing the text to render.
 * @param maxChars Maximum number of chars to render from the source string.
 * @param ltw Local to world matrix used to render the string.
 * @return Returns the height of the text rendered.
 */
float MFFont_DrawText(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pText, int maxChars = -1, const MFMatrix &ltw = MFMatrix::identity);

/**
 * Render a string.
 * Renders text to the screen.
 * @param pFont Pointer to a font.
 * @param x X coordinate in world space to render the string.
 * @param y Y coordinate in world space to render the string.
 * @param height Height of the text.
 * @param colour Colour of the text.
 * @param pText Pointer to a string containing the text to render.
 * @param maxChars Maximum number of chars to render from the source string.
 * @param ltw Local to world matrix used to render the string.
 * @return Returns the height of the text rendered.
 */
float MFFont_DrawText(MFFont *pFont, float x, float y, float height, const MFVector &colour, const char *pText, int maxChars = -1, const MFMatrix &ltw = MFMatrix::identity);

/**
 * Render a formatted string.
 * Renders formatted text to the screen.
 * @param pFont Pointer to a font.
 * @param pos Position in world space to render the string.
 * @param height Height of the text.
 * @param colour Colour of the text.
 * @param pFormat Pointer to the format string. This complies with all the regular printf format standards.
 * @param ... Additional parameters used according to the format specifier.
 * @return Returns the height of the text rendered.
 * @remarks The MFFont_DrawTextf format string conforms with all the standard printf format standards.
 */
float MFFont_DrawTextf(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pFormat, ...);

/**
 * Render a formatted string.
 * Renders formatted text to the screen.
 * @param pFont Pointer to a font.
 * @param x X coordinate in world space to render the string.
 * @param y Y coordinate in world space to render the string.
 * @param height Height of the text.
 * @param colour Colour of the text.
 * @param pFormat Pointer to the format string. This complies with all the regular printf format standards.
 * @param ... Additional parameters used according to the format specifier.
 * @return Returns the height of the text rendered.
 * @remarks The MFFont_DrawTextf format string conforms with all the standard printf format standards.
 */
float MFFont_DrawTextf(MFFont *pFont, float x, float y, float height, const MFVector &colour, const char *pFormat, ...);

float MFFont_DrawTextJustified(MFFont *pFont, const char *pText, const MFVector &pos, float boxWidth, float boxHeight, MFFontJustify justification, float textHeight, const MFVector &color, int numChars = -1, const MFMatrix &ltw = MFMatrix::identity);

float MFFont_DrawTextAnchored(MFFont *pFont, const char *pText, const MFVector &pos, MFFontJustify justification, float lineWidth, float textHeight, const MFVector &color, int numChars = -1, const MFMatrix &ltw = MFMatrix::identity);


/**
 * Get the debug font.
 * Gets a pointer to the debug font.
 * @return Returns a pointer to the debug font.
 */
MFFont* MFFont_GetDebugFont();

#endif // _MFFONT_H

/** @} */
