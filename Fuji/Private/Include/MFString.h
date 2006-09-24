/**
 * @file MFString.h
 * @brief Mount Fuji string funcitons.
 * Provides a set of useful string management functions.
 * @author Manu Evans
 * @defgroup MFString String Functions
 * @{
 */

#if !defined(_MFSTRING_H)
#define _MFSTRING_H

/**
 * Get the length of a string.
 * Get the length of a string, in characters.
 * @param pString String to find the length of.
 * @return Returns the length of the string, in characters, excluding the terminating NULL character.
 * @see MFString_Copy()
 * @see MFString_CaseCmp()
 */
int MFString_Length(const char *pString);

/**
 * Copy a string.
 * Copies the source string to the target buffer.
 * @param pBuffer Target buffer to receive a copy of the string.
 * @param pString Source string.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFString_CopyN()
 */
char* MFString_Copy(char *pBuffer, const char *pString);

/**
 * Copy a string with a maximum number of characters.
 * Copies the source string or the maximum number of characters from the source string to the target buffer.
 * @param pBuffer Target buffer to receive a copy of the string.
 * @param pString Source string.
 * @param maxChars Maximum characters to copy.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFString_Copy()
 */
char* MFString_CopyN(char *pBuffer, const char *pString, int maxChars);

/**
 * Concatinate a string.
 * Concatinate the source string with the target buffer.
 * @param pBuffer Target buffer to receive the concatinated string.
 * @param pString Source string.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFString_Copy()
 */
char* MFString_Cat(char *pBuffer, const char *pString);

/**
 * Concatinate 2 strings into a target buffer.
 * Concatinates the 2 strings into the target buffer.
 * @param pBuffer Target buffer to receive a copy of the string.
 * @param pString Source string.
 * @param pString2 Second source string.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFString_CopyN()
 */
char* MFString_CopyCat(char *pBuffer, const char *pString, const char *pString2);

/**
 * Convert a string to lower case.
 * Generates a copy of a string converted to lower case in the MFStr buffer.
 * @param pString String to convert.
 * @return Returns a pointer to a string in the MFStr buffer converted to lower case.
 * @see MFString_ToUpper()
 */
const char * MFString_ToLower(const char *pString);

/**
 * Convert a string to upper case.
 * Generates a copy of a string converted to upper case in the MFStr buffer.
 * @param pString String to convert.
 * @return Returns a pointer to a string in the MFStr buffer converted to upper case.
 * @see MFString_ToLower()
 */
const char * MFString_ToUpper(const char *pString);

/**
 * Compares 2 strings.
 * Compares 2 strings and returns the difference between them.
 * @param pString1 First source string.
 * @param pString2 Second source string.
 * @return Returns the difference between the 2 strings. 0 if the strings are identical.
 * @see MFString_CompareN()
 */
int MFString_Compare(const char *pString1, const char *pString2);

/**
 * Compares 2 strings with a maximum character limit.
 * Compares 2 strings with a maximum character limit and returns the difference between them.
 * @param pString1 First source string.
 * @param pString2 Second source string.
 * @param n Maximum number of characters to compare.
 * @return Returns the difference between the 2 strings. 0 if the strings are identical.
 * @see MFString_Compare()
 */
int MFString_CompareN(const char *pString1, const char *pString2, int n);

/**
 * Compares 2 strings with case insensitivity.
 * Compares 2 strings with case insensitivity and returns the difference between them.
 * @param pString1 First source string.
 * @param pString2 Second source string.
 * @return Returns the difference between the 2 strings. 0 if the strings are identical.
 * @see MFString_CaseCmpN()
 */
int MFString_CaseCmp(const char *pString1, const char *pString2);

/**
 * Compares 2 strings with case insensitivity and a maximum character limit.
 * Compares 2 strings with case insensitivity and a maximum character limit and returns the difference between them.
 * @param pString1 First source string.
 * @param pString2 Second source string.
 * @param n Maximum number of characters to compare.
 * @return Returns the difference between the 2 strings. 0 if the strings are identical.
 * @see MFString_CaseCmp()
 */
int MFString_CaseCmpN(const char *pString1, const char *pString2, uint32 n);

/**
 * Searches through a string for the specified character.
 * Searches through a string for the specified character.
 * @param pString String to search.
 * @param c Character to search for.
 * @return Returns a pointer to the first instance of the character \a c in \a pString.
 * @see MFString_RChr()
 */
char* MFString_Chr(const char *pString, int c);

/**
 * Searches backwards through a string for the specified character.
 * Searches backwards through a string for the specified character.
 * @param pString String to search.
 * @param c Character to search for.
 * @return Returns a pointer to the last instance of the character \a c in \a pString.
 * @see MFString_Chr()
 */
char* MFString_RChr(const char *pString, int c);

/**
 * Get a formatted a string.
 * Generates a new string from a format specifier and parameters.
 * @param format The format string. Supports all the standard formatting provided by the CRT like printf().
 * @param ... Variable argument list.
 * @return Pointer to the newly created string.
 * @remarks MFStr() uses a circular buffer for storing the strings it produces. You should NOT keep a long term string generated by MFStr. MFStr() is best suited to generating temporary and intermediate strings for immediate use.
 * @see MFStrN()
 */
const char* MFStr(const char *format, ...);

/**
 * Copy a specified number of characters from one string into a new string.
 * Generates a new string from a format specifier and parameters.
 * @param source The string from which characters will be copied.
 * @param n Number of characters to copy from the source string.
 * @return Pointer to the newly created string.
 * @remarks MFStrN() uses a circular buffer for storing the strings it produces. You should NOT keep a long term string generated by MFStrN. MFStrN() is best suited to generating temporary and intermediate strings for immediate use.
 * @see MFStrN()
 */
const char* MFStrN(const char *source, int n);

/**
 * Test if the specified character is a while space character.
 */
bool MFIsWhite(int c);

/**
 * Test if the specified character is an alphabetic character.
 */
bool MFIsAlpha(int c);

/**
 * Test if the specified character is a numeric character.
 */
bool MFIsNumeric(int c);

/**
 * Test if the specified character is an alphabetic or numeric character.
 */
bool MFIsAlphaNumeric(int c);

/**
 * Test if the specified character is a newline character.
 */
bool MFIsNewline(int c);

/**
 * Test if a character is a lower case alphabetic character.
 */
bool MFIsLower(int c);

/**
 * Test if a character is an upper case alphabetic character.
 */
bool MFIsUpper(int c);

/**
 * Converts a character to lower case.
 */
int MFToLower(int c);

/**
 * Converts a character to lower case.
 */
int MFToUpper(int c);

/**
 * Returns a pointer to the start of the next line in the provided string.
 */
char* MFSeekNewline(char *pC);

/**
 * Returns a pointer to the next non-white space character in the provided string.
 */
char* MFSkipWhite(char *pC);

//
// unicode support
//

/**
 * Get the length of a unicode string.
 * Get the length of a unicode string, in characters.
 * @param pString String to find the length of.
 * @return Returns the length of the unicode string, in characters, excluding the terminating NULL character.
 * @see MFWString_Copy()
 * @see MFWString_CaseCmp()
 */
int MFWString_Length(const uint16 *pString);

/**
 * Copy a unicode string.
 * Copies the source unicode string to the target buffer.
 * @param pBuffer Target buffer to receive a copy of the string.
 * @param pString Source string.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFWString_CopyN()
 */
uint16* MFWString_Copy(uint16 *pBuffer, const uint16 *pString);

/**
 * Copy a unicode string with a maximum number of characters.
 * Copies the source unicode string or the maximum number of characters from the source unicode string to the target buffer.
 * @param pBuffer Target buffer to receive a copy of the string.
 * @param pString Source string.
 * @param maxChars Maximum characters to copy.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFWString_Copy()
 */
uint16* MFWString_CopyN(uint16 *pBuffer, const uint16 *pString, int maxChars);

/**
 * Concatinate a unicode string.
 * Concatinate the source unicode string with the target buffer.
 * @param pBuffer Target buffer to receive the concatinated string.
 * @param pString Source string.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFWString_Copy()
 */
uint16* MFWString_Cat(uint16 *pBuffer, const uint16 *pString);

/**
 * Concatinate 2 unicode strings into a target buffer.
 * Concatinates the 2 unicode strings into the target buffer.
 * @param pBuffer Target buffer to receive a copy of the string.
 * @param pString Source string.
 * @param pString2 Second source string.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFWString_CopyN()
 */
uint16* MFWString_CopyCat(uint16 *pBuffer, const uint16 *pString, const uint16 *pString2);

/**
 * Compares 2 unicode strings.
 * Compares 2 unicode strings and returns the difference between them.
 * @param pString1 First source string.
 * @param pString2 Second source string.
 * @return Returns the difference between the 2 strings. 0 if the strings are identical.
 * @see MFString_CompareN()
 */
int MFWString_Compare(const uint16 *pString1, const uint16 *pString2);

/**
 * Returns a pointer to the start of the next line in the provided unicode string.
 */
uint16* MFSeekNewlineW(uint16 *pC);

/**
 * Returns a pointer to the next non-white space character in the provided unicode string.
 */
uint16* MFSkipWhiteW(uint16 *pC);

#include "MFString.inl"

#endif // _MFSTRING_H

/** @} */
