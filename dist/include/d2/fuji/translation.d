module fuji.translation;

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
	Unknown = -1,	/**< Unknown language */

	// European Languages
	English = 0,	/**< English (Default) */
	French,			/**< French */
	German,			/**< German */
	Dutch,			/**< Dutch */
	Italian,		/**< Italian */
	Spanish,		/**< Spanish */
	Portuguese,		/**< Portuguese */

	// Scandinavian Languages
	Swedish,		/**< Swedish */
	Norwegian,		/**< Norwegian */
	Finnish,		/**< Finnish */
	Danish,			/**< Danish */

	// Eastern European
	Russian,		/**< Russian */
	Greek,			/**< Greek */

	// Asian Languages
	Japanese,		/**< Japanese */
	Korean,			/**< Korean */
	Chinese			/**< Chinese */
}


/**
 * Get the name of a language.
 * Get the name of the specified language as spoken either natively or in english.
 * @param language Language to get the name of.
 * @param native If true, name in the native language is returned otherwise the language name in English. (ie, "Francies" vs "French")
 * @return Returns a string containing the name of the language.
 * @remarks If \a native is true, the returned string may require characters from an extended font page.
 */
extern (C) const(char*) MFTranslation_GetLanguageName(MFLanguage language, bool native = false);

/**
 * Get the system default language.
 * Gets the systems default language (Usually the language set in the system BIOS).
 * @return Returns the system default language.
 */
extern (C) MFLanguage MFTranslation_GetDefaultLanguage();

/**
 * Get the id for a language.
 * Gets the language id of a given language.
 * @param pLanguageName The name of the language requested.
 * @return Returns the specified language.
 */
extern (C) MFLanguage MFTranslation_GetLanguageByName(const(char*) pLanguageName);

/**
 * Load a translation string table.
 * Loads a translation string table. If the specified language is not available, the fallback language is used instead. If the fallback language is not available, 'English' is used instead.
 * @param pFilename Filename of the string table to load.
 * @param language Language to load.
 * @param fallback Fallback language in the case \a language is unavailable.
 * @return Return a pointer to the created MFStringTable. If the file was unavailable, NULL is returned.
 */
extern (C) MFStringTable* MFTranslation_LoadStringTable(const(char*) pFilename, MFLanguage language, MFLanguage fallback = MFLanguage.English);

/**
 * Load a translation table containing enum keys.
 * Loads a translation string table containing enum keys.
 * @param pFilename Filename of the enum table to load.
 * @return Return a pointer to the created MFStringTable. If the file was unavailable, NULL is returned.
 */
extern (C) MFStringTable* MFTranslation_LoadEnumStringTable(const(char*) pFilename);

/**
 * Destroy a loaded string table.
 * Destroys a loaded string table.
 * @param pTable Pointer to the table to destroy.
 * @return None.
 */
extern (C) void MFTranslation_DestroyStringTable(MFStringTable* pTable);

/**
 * Get the number of strings in the table.
 * Gets the number of strings in the string table.
 * @param pTable Pointer to a string table.
 * @return Returns the number of strings in the table.
 */
extern (C) int MFTranslation_GetNumStrings(MFStringTable* pTable);

/**
 * Find a string in the table.
 * Finds a string in the table.
 * @param pTable Pointer to a string table.
 * @param pString The string to find in the table.
 * @return Returns the id of the requested string.
 */
extern (C) int MFTranslation_FindString(MFStringTable* pTable, const(char*) pString);

/**
 * Get a string from the string table.
 * Gets a string Table to get string from.
 * @param pTable Pointer to a string table.
 * @param stringID ID if the string to get.
 * @return Returns the string specified by \a stringID from the specified string table.
 */
extern (C) const(char*) MFTranslation_GetString(MFStringTable* pTable, int stringID);

