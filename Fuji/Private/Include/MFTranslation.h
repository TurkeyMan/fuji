/**
 * @file MFTranslation.h
 * @brief Mount Fuji localisation interface.
 * Provides a set of functions to manage translation and localisation.
 * @author Manu Evans
 * @defgroup MFTranslation Localisation Functions
 * @{
 */

#if !defined(_MFTRANSLATION_H)
#define _MFTRANSLATION_H

/**
 * @struct MFStringTable
 * Represents a Fuji translation string table.
 */
struct MFStringTable;

/**
 * Language enums.
 * Various language available at runtime.
 */
enum MFLanguage
{
	MFLang_Unknown = -1,	/**< Unknown language */

	// European Languages
	MFLang_English = 0,		/**< English (Default) */
	MFLang_French,			/**< French */
	MFLang_German,			/**< German */
	MFLang_Dutch,			/**< Dutch */
	MFLang_Italian,			/**< Italian */
	MFLang_Spanish,			/**< Spanish */
	MFLang_Portuguese,		/**< Portuguese */

	// Scandinavian Languages
	MFLang_Swedish,			/**< Swedish */
	MFLang_Norwegian,		/**< Norwegian */
	MFLang_Finnish,			/**< Finnish */
	MFLang_Danish,			/**< Danish */

	// Eastern European
	MFLang_Russian,			/**< Russian */
	MFLang_Greek,			/**< Greek */

	// Asian Languages
	MFLang_Japanese,		/**< Japanese */
	MFLang_Korean,			/**< Korean */
	MFLang_Chinese,			/**< Chinese */

	MFLang_Max,				/**< Max language */
	MFLang_ForceInt = 0x7FFFFFFF	/**< Force enumeration to an int type */
};


/**
 * Get the name of a language.
 * Get the name of the specified language as spoken either natively or in english.
 * @param language Language to get the name of.
 * @param native If true, name in the native language is returned otherwise the language name in English. (ie, "Francies" vs "French")
 * @return Returns a string containing the name of the language.
 * @remarks If \a native is true, the returned string may require characters from an extended font page.
 */
const char* MFTranslation_GetLanguageName(MFLanguage language, bool native = false);

/**
 * Get the system default language.
 * Gets the systems default language (Usually the language set in the system BIOS).
 * @return Returns the system default language.
 */
MFLanguage MFTranslation_GetDefaultLanguage();

/**
 * Load a translation string table.
 * Loads a translation string table. If the specified language is not available, the fallback language is used instead. If the fallback language is not available, 'English' is used instead.
 * @param pFilename Filename of the string table to load.
 * @param language Language to load.
 * @param fallback Fallback language in the cawse \a language is unavailable.
 * @return Return a pointer to the created MFStringTable. If the file was unavailable, NULL is returned.
 */
MFStringTable* MFTranslation_LoadStringTable(const char *pFilename, MFLanguage language, MFLanguage fallback = MFLang_English);

/**
 * Load a translation table containing enum keys.
 * Loads a translation string table containing enum keys.
 * @param pFilename Filename of the enum table to load.
 * @return Return a pointer to the created MFStringTable. If the file was unavailable, NULL is returned.
 */
MFStringTable* MFTranslation_LoadEnumStringTable(const char *pFilename);

/**
 * Destroy a loaded string table.
 * Destroys a loaded string table.
 * @param pTable Pointer to the table to destroy.
 * @return None.
 */
void MFTranslation_DestroyStringTable(MFStringTable *pTable);

/**
 * Get the number of strings in the table.
 * Gets the number of strings in the string table.
 * @param pTable Pointer to a string table.
 * @return Returns the number of strings in the table.
 */
int MFTranslation_GetNumStrings(MFStringTable *pTable);

/**
 * Find a string in the table.
 * Finds a string in the table.
 * @param pTable Pointer to a string table.
 * @param pString The string to find in the table.
 * @return Returns the id of the requested string.
 */
int MFTranslation_FindString(MFStringTable *pTable, const char *pString);

/**
 * Get a string from the string table.
 * Gets a string Table to get string from.
 * @param pTable Pointer to a string table.
 * @param stringID ID if the string to get.
 * @return Returns the string specified by \a stringID from the specified string table.
 */
const char* MFTranslation_GetString(MFStringTable *pTable, int stringID);

#endif // _MFTRANSLATION_H

/** @} */
