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

#include "MFArray.h"

/**
 * Copy memory.
 * Copy memory from one location to another.
 * @param pDest Pointer to the destination buffer.
 * @param pSrc Pointer to the source buffer.
 * @param size Size of data to copy in bytes.
 * @return Returns pDest for convenience.
 */
MF_API void *MFCopyMemory(void *pDest, const void *pSrc, size_t size);

/**
 * Zero memory.
 * Initialise a region of memory to zero.
 * @param pDest Pointer to the target buffer.
 * @param size Size of data to initialise in bytes.
 * @return Returns pDest for convenience.
 */
MF_API void *MFZeroMemory(void *pDest, size_t size);

/**
 * Initialise memory.
 * Initialise a region of memory to a specified value.
 * @param pDest Pointer to the target buffer.
 * @param value Value written to each byte in the memory region.
 * @param size Number of bytes to initialise.
 * @return Returns pDest for convenience.
 */
MF_API void *MFMemSet(void *pDest, int value, size_t size);

/**
 * Compare memory.
 * Compare bytes returning the difference.
 * @param pBuf1 Pointer to the first buffer.
 * @param pBuf2 Pointer to the second buffer.
 * @param size Number of bytes to compate.
 * @return Return the difference of the first encountered differing byte. Returns 0 if buffers are identical.
 */
MF_API int MFMemCompare(const void *pBuf1, const void *pBuf2, size_t size);

/**
 * Get the length of a string.
 * Get the length of a string, in bytes.
 * @param pString String to find the length of.
 * @return The length of the string, in bytes, excluding the terminating NULL character.
 * @see MFString_Copy()
 * @see MFString_CaseCmp()
 */
size_t MFString_Length(const char *pString);

/**
 * Get the length of a string clamping to a given maximum.
 * Count the number of bytes in a string, clamping to a given maximum.
 * @param pString String to find the length of.
 * @param maxChars Maximum number of chars to count.
 * @return The length of the string, in bytes, excluding the terminating NULL character.
 * @see MFString_Length()
 */
size_t MFString_LengthN(const char *pString, size_t maxChars);

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
char* MFString_CopyN(char *pBuffer, const char *pString, size_t maxChars);

/**
 * Copy a string with custom terminator.
 * Copies characters from the source string to the target buffer until reaching the specified terminating character.
 * @param pBuffer Target buffer to receive a copy of the string.
 * @param pString Source string.
 * @param terminator Terminating character.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFString_Copy()
 */
char* MFString_CopyUntil(char *pBuffer, const char *pString, int terminator);

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
 * Duplicate a string.
 * Duplicates the source string into a newly allocated buffer.
 * @param pString Source string.
 * @return A newly allocated buffer copied from \a pString.
 * @see MFString_Copy()
 */
MF_API char* MFString_Dup(const char *pString);

/**
 * Convert a string to lower case.
 * Generates a copy of a string converted to lower case in the MFStr buffer.
 * @param pString String to convert.
 * @return A pointer to a string in the MFStr buffer converted to lower case.
 * @see MFString_ToUpper()
 */
MF_API const char * MFString_ToLower(const char *pString);

/**
 * Convert a string to upper case.
 * Generates a copy of a string converted to upper case in the MFStr buffer.
 * @param pString String to convert.
 * @return A pointer to a string in the MFStr buffer converted to upper case.
 * @see MFString_ToLower()
 */
MF_API const char * MFString_ToUpper(const char *pString);

/**
 * Compares 2 strings.
 * Compares 2 strings and returns the difference between them.
 * @param pString1 First source string.
 * @param pString2 Second source string.
 * @return The difference between the 2 strings. 0 if the strings are identical.
 * @see MFString_CompareN()
 */
MF_API int MFString_Compare(const char *pString1, const char *pString2);

/**
 * Compares 2 strings with a maximum character limit.
 * Compares 2 strings with a maximum character limit and returns the difference between them.
 * @param pString1 First source string.
 * @param pString2 Second source string.
 * @param n Maximum number of characters to compare.
 * @return The difference between the 2 strings. 0 if the strings are identical.
 * @see MFString_Compare()
 */
MF_API int MFString_CompareN(const char *pString1, const char *pString2, size_t n);

/**
 * Compares 2 strings with case insensitivity.
 * Compares 2 strings with case insensitivity and returns the difference between them.
 * @param pString1 First source string.
 * @param pString2 Second source string.
 * @return The difference between the 2 strings. 0 if the strings are identical.
 * @see MFString_CaseCmpN()
 */
MF_API int MFString_CaseCmp(const char *pString1, const char *pString2);

/**
 * Compares 2 strings with case insensitivity and a maximum character limit.
 * Compares 2 strings with case insensitivity and a maximum character limit and returns the difference between them.
 * @param pString1 First source string.
 * @param pString2 Second source string.
 * @param n Maximum number of characters to compare.
 * @return The difference between the 2 strings. 0 if the strings are identical.
 * @see MFString_CaseCmp()
 */
MF_API int MFString_CaseCmpN(const char *pString1, const char *pString2, size_t n);

/**
 * Searches through a string for the specified character.
 * Searches through a string for the specified character.
 * @param pString String to search.
 * @param c Character to search for.
 * @return A pointer to the first instance of the character \a c in \a pString.
 * @see MFString_RChr()
 */
char* MFString_Chr(const char *pString, int c);

/**
 * Searches backwards through a string for the specified character.
 * Searches backwards through a string for the specified character.
 * @param pString String to search.
 * @param c Character to search for.
 * @return A pointer to the last instance of the character \a c in \a pString.
 * @see MFString_Chr()
 */
char* MFString_RChr(const char *pString, int c);

/**
 * Get file extension from filename.
 * Get the file extension part from a filename.
 * @param pFilename String containing the filename.
 * @return A pointer to the file extension (the part after the final '.' in the filename).
 * @remarks The pointer returned points into the source string. It is only valid while the source string remains unchanged.
 */
char* MFString_GetFileExtension(const char *pFilename);

/**
 * Get the filename from a path.
 * Gets the filename part from a path.
 * @param pFilename String containing a file path.
 * @return A string containing just the filename taken from the path.
 * @remarks The pointer returned points into the source string. It is only valid while the source string remains unchanged.
 */
const char* MFStr_GetFileName(const char *pFilename);

/**
 * Get the filename without its extension from a path.
 * Gets the filename without its extension from a path.
 * @param pFilename String containing a file path.
 * @return A string containing just the filename taken from the path with the file extension removed.
 * @remarks The pointer returned is in the MFStr buffer. If persistence is desired, the client should take a copy.
 */
const char* MFStr_GetFileNameWithoutExtension(const char *pFilename);

/**
 * Get the file path from a path string.
 * Gets the file path with the filename removed from a path string.
 * @param pFilename String containing a file path.
 * @return A string containing just the file path (everything preceeding the final '/').
 * @remarks The pointer returned is in the MFStr buffer. If persistence is desired, the client should take a copy.
 */
const char* MFStr_GetFilePath(const char *pFilename);

/**
 * Truncate the file extension.
 * Truncates the file extension from a filename.
 * @param pFilename String containing a filename.
 * @return A string containing the filename with the final file extension truncated.
 * @remarks The pointer returned is in the MFStr buffer. If persistence is desired, the client should take a copy.
 */
const char* MFStr_TruncateExtension(const char *pFilename);

/**
 * String pattern match.
 * Performs a string pattern match.
 * @param pPattern String containing the filename pattern to test. May contain wild cards, etc.
 * @param pFilename String containing the filename to compare against.
 * @param ppMatchDirectory Cant remember what this is used for. Dont use it...
 * @param bCaseSensitive Perform the pattern match with case sensitivity.
 * @return True if the pattern matches the filename.
 */
MF_API bool MFString_PatternMatch(const char *pPattern, const char *pFilename, const char **ppMatchDirectory = NULL, bool bCaseSensitive = false);

/**
 * URL encode a string.
 * Encodes a string for use in a URL or HTTP 'POST' content.
 * @param pString String to encode.
 * @param pExcludeChars A string containing characters to be excluded from URL encoding.
 * @return The URL encoded string.
 * @remarks The pointer returned is in the MFStr buffer. If persistence is desired, the client should take a copy.
 */
MF_API const char* MFStr_URLEncodeString(const char *pString, const char *pExcludeChars = NULL);

/**
 * URL encode a string.
 * Encodes a string for use in a URL or HTTP 'POST' content.
 * @param pDest Output buffer, may be NULL.
 * @param pString String to encode.
 * @param pExcludeChars A string containing characters to be excluded from URL encoding.
 * @return Return the length of the output string, excluding the terminating NULL.
 * @remarks If pDest is NULL, the output length will be calculated but no output will be written.
 */
MF_API size_t MFString_URLEncode(char *pDest, const char *pString, const char *pExcludeChars = NULL);

/**
 * Get a formatted a string.
 * Generates a new string from a format specifier and parameters.
 * @param format The format string. Supports all the standard formatting provided by the CRT like printf().
 * @param ... Variable argument list.
 * @return Pointer to the newly created string.
 * @remarks MFStr() uses a circular buffer for storing the strings it produces. You should NOT keep a long term string generated by MFStr. MFStr() is best suited to generating temporary and intermediate strings for immediate use.
 * @see MFStrN()
 */
MF_API const char* MFStr(const char *format, ...) MFPRINTF_FUNC(1, 2);

/**
 * Copy a specified number of characters from one string into a new string.
 * Generates a new string from a format specifier and parameters.
 * @param source The string from which characters will be copied.
 * @param n Number of characters to copy from the source string.
 * @return Pointer to the newly created string.
 * @remarks MFStrN() uses a circular buffer for storing the strings it produces. You should NOT keep a long term string generated by MFStrN. MFStrN() is best suited to generating temporary and intermediate strings for immediate use.
 * @see MFStrN()
 */
MF_API const char* MFStrN(const char *source, size_t n);

/**
 * Test if a string is a number.
 * Tests to see if a given string contains a decimal or hexadecimal number (using '0x' hex notation).
 * @param pString String to test.
 * @param bAllowHex Test also if the string is a hexadecimal number.
 * @return True if the string was a valid number.
 */
MF_API bool MFString_IsNumber(const char *pString, bool bAllowHex = true);

/**
 * Parse an integer from a string.
 * Parses a decimal, hexadecimal or binary integer from a given string.
 * @param pString String to parse.
 * @param bDetectBase Detect the numeric base of the string via prefixes '0x' or '$' for hex, or 'b' for binary.
 * @param base If \a bDetectBase is false, specifies the numeric base to use. Accepted values are 10, 16 and 2.
 * @param ppNextChar Optional pointer that receives the following character in the string.
 * @return The integer parsed from the string.
 */
MF_API int MFString_AsciiToInteger(const char *pString, bool bDetectBase = true, int base = 10, const char **ppNextChar = NULL);

/**
 * Parse a float from a string.
 * Parses a float from a given string.
 * @param pString String to parse.
 * @param ppNextChar Optional pointer that receives the following character in the string.
 * @return The float parsed from the string.
 */
MF_API float MFString_AsciiToFloat(const char *pString, const char **ppNextChar = NULL);

/**
 * Find a string within a list of strings.
 * Finds a string within a list of strings and return the item index.
 * @param pString String to find.
 * @param ppKeys Array of keys to search.
 * @param numKeys Number of items in the array. If -1 is given, ppKeys is assumed to be a NULL terminated array.
 * @param bCaseSensitive Perform case sensitive string comparisons.
 * @return The index of \a pString if it was present in the list, or -1 if it was not present.
 */
MF_API int MFString_Enumerate(const char *pString, const char **ppKeys, size_t numKeys = -1, bool bCaseSensitive = false);

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
 * Test if the specified character is a hexadecimal character.
 */
bool MFIsHex(int c);

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
 * Returns a pointer to the first instance of a specified delimeter.
 */
char* MFSeekDelimiter(char *pC, const char *pDelimiters = " \t\r\n");

/**
 * Returns a pointer to the first instance of a specified delimeter.
 */
const char* MFSeekDelimiter(const char *pC, const char *pDelimiters = " \t\r\n");

/**
 * Returns a pointer to the start of the next line in the provided string.
 */
char* MFSeekNewline(char *pC);

/**
 * Returns a pointer to the start of the next line in the provided string.
 */
const char* MFSeekNewline(const char *pC);

/**
 * Returns a pointer to the start of the next word.
 */
char* MFSeekNextWord(char *pC);

/**
 * Returns a pointer to the start of the next word.
 */
const char* MFSeekNextWord(const char *pC);

/**
 * Returns a pointer to the next non-white space character in the provided string.
 */
char* MFSkipDelimiters(char *pC, const char *pDelimiters = " \t\r\n");

/**
 * Returns a pointer to the next non-white space character in the provided string.
 */
const char* MFSkipDelimiters(const char *pC, const char *pDelimiters = " \t\r\n");

/**
 * Returns a pointer to the next non-white space character in the provided string.
 */
char* MFSkipWhite(char *pC);

/**
 * Returns a pointer to the next non-white space character in the provided string.
 */
const char* MFSkipWhite(const char *pC);

//
// UTF8 support
//

/**
 * Get the number of actual characters in a string.
 * Gets the number of actual characters in a string, with consideration to UTF8 multibyte encoding.
 * @param pString String to find the number of characters.
 * @return The number of actual characters in the string, excluding the terminating NULL character.
 * @remarks Takes into account UTF8 multibyte encoding when calculating the number of characters.
 * @see MFString_GetCharacterOffset()
 */
int MFString_GetNumChars(const char *pString);

/**
 * Get the character offset from the start of a string.
 * Gets the character offset in bytes from the start of a string, with consideration to UTF8 multibyte encoding.
 * @param pString String to find the character offset.
 * @param character Character index to find the offset of.
 * @return The character offset in bytes of the specified character index.
 * @remarks Takes into account UTF8 multibyte encoding when calculating the character offset.
 * @see MFString_GetNumChars()
 */
size_t MFString_GetCharacterOffset(const char *pString, int character);

/**
 * Encode a unicode character to a UTF-8 sequence.
 * Encodes the give unicode character to it's respective UTF-8 sequence.
 * @param c Unicode character to encode.
 * @param pMBChar Pointer to an output buffer to receive the encoded bytes.
 * @return The number of bytes used by the UTF-8 encoding.
 * @see MFString_DecodeUTF8()
 */
int MFString_EncodeUTF8(int c, char *pMBChar);

/**
 * Decode a UTF-8 sequence.
 * Decodes a UTF-8 sequence.
 * @param pMBChar Pointer to a UTF-8 sequence.
 * @param pNumBytes Pointer to an int that receives the number of bytes in the sequence.
 * @return The decoded unicode character.
 * @see MFString_EncodeUTF8()
 */
int MFString_DecodeUTF8(const char *pMBChar, int *pNumBytes);

/**
 * Move to the next character in a UTF-8 encoded string.
 * Moves to the next character in a UTF-8 encoded string.
 * @param pChar Pointer to a UTF-8 string.
 * @return A pointer to the next character in the UTF-8 string.
 * @see MFString_PrevChar()
 */
char *MFString_NextChar(const char *pChar);

/**
 * Move to the previous character in a UTF-8 encoded string.
 * Moves to the previous character in a UTF-8 encoded string.
 * @param pChar Pointer to a UTF-8 string.
 * @return A pointer to the previous character in the UTF-8 string.
 * @see MFString_NextChar()
 */
char *MFString_PrevChar(const char *pChar);

/**
 * Copy a UTF-8 string to a UTF-16 string.
 * Copies a UTF-8 string to a UTF-16 output buffer.
 * @param pBuffer Pointer to a UTF-8 string to copy.
 * @param pString Pointer to the UTF-16 output buffer.
 * @return The number of wchar_t's written to the output buffer.
 */
MF_API size_t MFWString_CopyUTF8ToUTF16(wchar_t *pBuffer, const char *pString);

/**
 * Convert a UTF-8 string to a temporary UTF-16 string.
 * Converts a UTF-8 string to a temporary UTF-16 string, using the MFStr() buffer.
 * @param pUTF8String Pointer to a UTF-8 string to copy.
 * @param pNumChars Optional pointer to a size_t that receives the number of wchar_t's in the output buffer.
 * @return A pointer to the converted UTF-16 string in a temporary buffer.
 * @remarks MFString_UFT8AsWChar() uses the MFStr() temporary buffer for storing the output. Refer to MFStr() for usage details.
 * @see MFWString_CopyUTF8ToUTF16(), MFStr(), MFStrN()
 */
MF_API wchar_t* MFString_UFT8AsWChar(const char *pUTF8String, size_t *pNumChars = NULL);

/**
 * Convert a UTF-16 string to a temporary UTF-8 string.
 * Converts a UTF-16 string to a temporary UTF-8 string, using the MFStr() buffer.
 * @param pWString Pointer to a UTF-16 string to copy.
 * @param pNumBytes Optional pointer to a size_t that receives the number of char's in the output buffer.
 * @return A pointer to the converted UTF-8 string in a temporary buffer.
 * @remarks MFString_WCharAsUTF8() uses the MFStr() temporary buffer for storing the output. Refer to MFStr() for usage details.
 * @see MFString_CopyUTF16ToUTF8(), MFStr(), MFStrN()
 */
MF_API char* MFString_WCharAsUTF8(const wchar_t *pWString, size_t *pNumBytes = NULL);


//
// unicode support
//

/**
 * Convert a UTF16 string to a UTF8 string.
 * Converts a UTF16 string to a UTF8 string placing the result in the target buffer.
 * @param pBuffer Target buffer to output the resulting string.
 * @param pString UTF16 string to convert.
 * @return The length of the result string in bytes.
 * @see MFString_UFT8ToWChar()
 * @see MFString_UFT8AsWChar()
 */
MF_API size_t MFString_CopyUTF16ToUTF8(char *pBuffer, const wchar_t *pString);

/**
 * Get the length of a unicode string.
 * Get the length of a unicode string, in wide chars.
 * @param pString String to find the length of.
 * @return The length of the unicode string, in wide chars, excluding the terminating NULL character.
 * @see MFWString_Copy()
 * @see MFWString_CaseCmp()
 */
size_t MFWString_Length(const wchar_t *pString);

/**
 * Copy a unicode string.
 * Copies the source unicode string to the target buffer.
 * @param pBuffer Target buffer to receive a copy of the string.
 * @param pString Source string.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFWString_CopyN()
 */
wchar_t* MFWString_Copy(wchar_t *pBuffer, const wchar_t *pString);

/**
 * Copy a unicode string with a maximum number of characters.
 * Copies the source unicode string or the maximum number of characters from the source unicode string to the target buffer.
 * @param pBuffer Target buffer to receive a copy of the string.
 * @param pString Source string.
 * @param maxChars Maximum characters to copy.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFWString_Copy()
 */
wchar_t* MFWString_CopyN(wchar_t *pBuffer, const wchar_t *pString, size_t maxChars);

/**
 * Concatinate a unicode string.
 * Concatinate the source unicode string with the target buffer.
 * @param pBuffer Target buffer to receive the concatinated string.
 * @param pString Source string.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFWString_Copy()
 */
wchar_t* MFWString_Cat(wchar_t *pBuffer, const wchar_t *pString);

/**
 * Concatinate 2 unicode strings into a target buffer.
 * Concatinates the 2 unicode strings into the target buffer.
 * @param pBuffer Target buffer to receive a copy of the string.
 * @param pString Source string.
 * @param pString2 Second source string.
 * @return \a pBuffer which can be used as a paramater to other functions.
 * @see MFWString_CopyN()
 */
wchar_t* MFWString_CopyCat(wchar_t *pBuffer, const wchar_t *pString, const wchar_t *pString2);

/**
 * Compares 2 unicode strings.
 * Compares 2 unicode strings and returns the difference between them.
 * @param pString1 First source string.
 * @param pString2 Second source string.
 * @return The difference between the 2 strings. 0 if the strings are identical.
 * @see MFWString_CompareN()
 */
int MFWString_Compare(const wchar_t *pString1, const wchar_t *pString2);

/**
 * Compares 2 unicode strings with case insensitivity.
 * Compares 2 unicode strings with case insensitivity and returns the difference between them.
 * @param pString1 First source string.
 * @param pString2 Second source string.
 * @return The difference between the 2 unicode strings. 0 if the strings are identical.
 * @see MFWString_CaseCmpN()
 */
int MFWString_CaseCmp(const wchar_t *pString1, const wchar_t *pString2);

int MFWString_CompareUTF8(const wchar_t *pString1, const char *pString2);

/**
 * Returns a pointer to the first instance of a specified delimeter.
 */
wchar_t* MFSeekDelimiterW(wchar_t *pC, const wchar_t *pDelimiters = L" \t\r\n");

/**
 * Returns a pointer to the first instance of a specified delimeter.
 */
const wchar_t* MFSeekDelimiterW(const wchar_t *pC, const wchar_t *pDelimiters = L" \t\r\n");

/**
 * Returns a pointer to the start of the next line in the provided string.
 */
wchar_t* MFSeekNewlineW(wchar_t *pC);

/**
 * Returns a pointer to the start of the next line in the provided string.
 */
const wchar_t* MFSeekNewlineW(const wchar_t *pC);

/**
 * Returns a pointer to the start of the next word.
 */
wchar_t* MFSeekNextWordW(wchar_t *pC);

/**
 * Returns a pointer to the start of the next word.
 */
const wchar_t* MFSeekNextWordW(const wchar_t *pC);

/**
 * Returns a pointer to the next non-white space character in the provided string.
 */
wchar_t* MFSkipWhiteW(wchar_t *pC);

/**
 * Returns a pointer to the next non-white space character in the provided string.
 */
const wchar_t* MFSkipWhiteW(const wchar_t *pC);

////// String Class //////

struct MFStringData
{
	char *pMemory;
	size_t bytes;
	size_t allocated;
	int refCount;
};


/**
 * Fuji String class.
 * String management class via the Fuji string heap.
 */
class MFString
{
public:
	static MFString Format(const char *pFormat, ...) MFPRINTF_FUNC(1, 2);
	static MFString Static(const char *pString);

	MFString();
	MFString(const MFString &string);
	MFString(const char *pString, bool bHoldStaticPointer = false);
	MFString(const char *pString, size_t numChars);
	~MFString();

	bool operator!() const;									/**< Boolean 'not' operator. */
	operator bool() const;									/**< bool typecast operator. */

	char operator[](size_t index);

	MFString& operator=(const char *pString);				/**< Assignment operator. */
	MFString& operator=(const MFString &string);			/**< Assignment operator. */
	MFString& operator+=(char c);							/**< Append operator. */
	MFString& operator+=(const char *pString);				/**< Append operator. */
	MFString& operator+=(const MFString &string);			/**< Append operator. */
	MFString operator+(char c) const;						/**< Concatinate operator. */
	MFString operator+(const char *pString) const;			/**< Concatinate operator. */
	MFString operator+(const MFString &string) const;		/**< Concatinate operator. */

	bool operator==(const char *pString) const;				/**< Case-sensitive compare 'equal' operator. */
	bool operator==(const MFString &string) const;			/**< Case-sensitive compare 'equal' operator. */
	bool operator!=(const char *pString) const;				/**< Case-sensitive compare 'not-equal' operator. */
	bool operator!=(const MFString &string) const;			/**< Case-sensitive compare 'not-equal' operator. */
	bool operator>=(const char *pString) const;				/**< Case-sensitive compare 'greater or equal' operator. */
	bool operator>=(const MFString &string) const;			/**< Case-sensitive compare 'greater or equal' operator. */
	bool operator<=(const char *pString) const;				/**< Case-sensitive compare 'less or equal' operator. */
	bool operator<=(const MFString &string) const;			/**< Case-sensitive compare 'less or equal' operator. */
	bool operator>(const char *pString) const;				/**< Case-sensitive compare 'greater' operator. */
	bool operator>(const MFString &string) const;			/**< Case-sensitive compare 'greater' operator. */
	bool operator<(const char *pString) const;				/**< Case-sensitive compare 'less' operator. */
	bool operator<(const MFString &string) const;			/**< Case-sensitive compare 'less' operator. */

	MFString& Detach(size_t reserveBytes = 0);
	MFString& Reserve(size_t bytes, bool bClearString = false);
	MFString Duplicate() const;

	MFString& SetStaticString(const char *pStaticString);
	MFString& FromUTF16(const wchar_t *pString);
	MFString& FromInt(int number);
	MFString& FromFloat(float number);

	MFString& Sprintf(const char *pFormat, ...) MFPRINTF_METHOD(1, 2);
	int Parse(const char *pFormat, ...);

	const char *CStr() const;
	int ToInt(int base = 10) const;
	float ToFloat() const;

	int NumBytes() const;
	int NumChars() const;
	bool IsNull() const;
	bool IsEmpty() const;
	bool IsNumeric() const;

	uint32 GetHash() const;									/**< Generate a fast hash for this string. */

	bool Equals(const char *pString) const;					/**< Case-sensitive 'equal' comparison. */
	bool Equals(const MFString &string) const;				/**< Case-sensitive 'equal' comparison. */
	bool EqualsInsensitive(const char *pString) const;		/**< Case-insensitive 'equal' comparison. */
	bool EqualsInsensitive(const MFString &string) const;	/**< Case-insensitive 'equal' comparison. */

	bool BeginsWith(const char *pString) const;
	bool BeginsWith(const MFString &string) const;
	bool EndsWith(const char *pString) const;
	bool EndsWith(const MFString &string) const;

	int FindChar(int c, int startOffset = 0) const;
	int FindCharReverse(int c) const;

	MFString Upper() const;
	MFString Lower() const;
	MFString& Trim(bool bFront = true, bool bEnd = true, const char *pCharacters = " \t\r\n");
	MFString& PadLeft(int minLength, const char *pPadding = " ");
	MFString& PadRight(int minLength, const char *pPadding = " ", bool bAlignPadding = false);

	MFString SubStr(int offset, int count = -1) const;
	MFString& Truncate(int length);
	MFString GetExtension() const;
	MFString& TruncateExtension();
	MFString& ClearRange(int offset, int length);

	MFString& Insert(int offset, MFString string);
	MFString& Replace(int offset, int range, MFString string);

	MFString& Join(const MFArray<MFString> &strings, const char *pSeparator = NULL, const char *pTokenPrefix = NULL, const char *pTokenSuffix = NULL, const char *pBefore = NULL, const char *pAfter = NULL);
	MFArray<MFString>& Split(MFArray<MFString> &output, const char *pDelimiters = " \t\r\n");
	MFArray<MFString>& SplitLines(MFArray<MFString> &output);

	int Enumerate(const MFArray<MFString> keys, bool bCaseSensitive = false);
	int Enumerate(const char **ppKeys, size_t numKeys = -1, bool bCaseSensitive = false);

	MFString StripToken(const char *pDelimiters = " \t\r\n");

private:
	MFStringData *pData;
};

/**
 * Concatinate operator.
 */
MFString operator+(const char *pString, const MFString &string);

/**
 * Static string class.
 * Creates a pre-allocated string buffer, which automatically overflows into an allocation in the case of long strings.
 */
template<size_t Bytes>
class MFStaticString
{
public:
	MFStaticString(const char *pString = NULL);
	~MFStaticString();

	bool operator!() const;								/**< Boolean 'not' operator. */
	operator bool() const;								/**< bool typecast operator. */

	MFStaticString& operator=(const char *pString);		/**< Assignment operator. */
	MFStaticString& operator+=(char c);					/**< Append operator. */
	MFStaticString& operator+=(const char *pString);		/**< Append operator. */

	const char *CStr() const;

	int NumBytes() const;
	int NumChars() const;

private:
	uint32 length;

	static const size_t BufferLen = Bytes - sizeof(length);
	union
	{
		char buffer[BufferLen];
		struct
		{
			char *pBuffer;
			size_t allocated;
		} alloc;
	} u;
};

/**
 * Implements a D language string.
 */
struct DString : public DSlice<const char>
{
	// constructors
	DString() {}
	DString(const char *ptr, size_t length) : DSlice<const char>(ptr, length) {}
	DString(DSlice<const char> rh) : DSlice<const char>(rh) {}
	DString(const char *pString) : DSlice<const char>(pString, pString ? MFString_Length(pString) : 0) {}

	// assignment
	DString& operator =(DSlice<const char> rh) { length = rh.length; ptr = rh.ptr; return *this; }
	DString& operator =(const char *pString) { ptr = pString; length = pString ? MFString_Length(pString) : (size_t)0; return *this; }

	operator MFString() const { return MFString(ptr, length); }

	// contents
	DString slice(size_t first, size_t last) const
	{
		MFDebug_Assert(first <= last && last <= length, "Index out of range!");
		return DString(ptr + first, last - first);
	}

	// comparison
	template <typename U>
	bool eq(DSlice<U> rh) const  // ***!!!*** do not understand, why need this here? it should call through to the base class just fine!? **TEST ON GCC**
	{
		return DSlice<const char>::eq(rh);
	}
	bool eq(const char *pString) const
	{
		if(ptr && pString)
			return MFString_CompareN(ptr, pString, length) == 0;
		return false;
	}
	bool eqi(DSlice<const char> rh) const
	{
		if(length != rh.length)
			return false;
		return MFString_CaseCmpN(ptr, rh.ptr, length) == 0;
	}
	bool eqi(const char *pString) const
	{
		if(ptr && pString)
			return MFString_CaseCmpN(ptr, pString, length) == 0;
		return false;
	}

	// c-string compatibility
	char* toStringz(char *pBuffer, size_t bufferLen) const
	{
		size_t len = length < bufferLen-1 ? length : bufferLen-1;
		MFCopyMemory(pBuffer, ptr, len);
		pBuffer[len] = 0;
		return pBuffer;
	}
};

#include "MFString.inl"

/**
 * Generate a string describing the state of the string heap.
 * Generates a string describing the current state of the string heap.
 * @return String describing the state of the string heap.
 * @see MFString_Dump()
 */
MF_API MFString MFString_GetStats();

/**
 * Dump the state of the string heap.
 * Dumps the current state of the string heap.
 * @return None.
 * @see MFString_GetStats()
 */
MF_API void MFString_Dump();

#endif // _MFSTRING_H

/** @} */
