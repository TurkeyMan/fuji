#include "Fuji.h"

#include "MFSystem.h"
#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFString.h"
#include "MFStringCache.h"
#include "MFTranslation_Internal.h"
#include "Asset/MFIntStringTable.h"

#include "MFArray.h"


struct MFIntStringTable
{
	struct Table
	{
		MFLanguage language;
		MFStringTable *pTemplate;
		size_t size;
	};

	Table *pLanguages;
	int numLanguages;

	int numStrings;

	int enumList;
	int englishList;
};


/**** Globals ****/

static const char *gFileExtensions[MFISTF_Max] =
{
	".csv"
};


/**** Functions ****/

MF_API MFIntStringTable *MFIntStringTable_CreateFromFile(const char *pFilename)
{
	// find format
	const char *pExt = MFString_GetFileExtension(pFilename);

	int format;
	for(format=0; format<MFISTF_Max; ++format)
	{
		if(!MFString_Compare(pExt, gFileExtensions[format]))
			break;
	}
	if(format == MFISTF_Max)
		return NULL;

	// load file
	size_t size;
	char *pData = MFFileSystem_Load(pFilename, &size, true);
	if(!pData)
		return NULL;

	// load the image
	MFIntStringTable *pStringTable = MFIntStringTable_CreateFromFileInMemory(pData, size, (MFIntStringTableFormat)format);

	// free file
	MFHeap_Free(pData);

	return pStringTable;
}

static bool ReadCSVLine(const char *&pFile, size_t &bytesRemaining, MFArray<MFString> &row)
{
#define CSV_ADVANCE(x) { pFile += (x); bytesRemaining -= (x); }

	if(!pFile)
		return false;

	// read the line
	bool bEmptyLine = true;
	while(bEmptyLine)
	{
		row.clear();

		if(bytesRemaining == 0)
			return false;

		while(bytesRemaining > 0 && !MFIsNewline(*pFile))
		{
			// skip any white space
			while(bytesRemaining > 0 && MFIsWhite(*pFile))
				CSV_ADVANCE(1)

			// check if the field was empty
			if(*pFile == ',')
			{
				row.push(NULL);
				CSV_ADVANCE(1)
				continue;
			}

			// check for quote
			if(*pFile == '"')
			{
				CSV_ADVANCE(1)

				const char *pFieldStart = pFile;
				while(bytesRemaining > 0 && *pFile && !MFIsNewline(*pFile))
				{
					// check for closing quote
					if(*pFile == '"')
					{
						// double quote is a csv escape sequence
						if(pFile[1] == '"')
							CSV_ADVANCE(1)
						else
							break;
					}

					// we need to keep any escaped codes
					if(*pFile == '\\')
						CSV_ADVANCE(1)

					if(bytesRemaining == 0) // if we ended the file in the middle of an escape sequence...
						return false;

					CSV_ADVANCE(1)
				}

				// add field to row
				MFString field(pFieldStart, (int)(pFile - pFieldStart));
				row.push(field);
				bEmptyLine = false;

				// check the quote was closed
				MFDebug_Assert(*pFile == '"', "Expected close quote. Malformed csv file!");
				if(*pFile == '"')
					CSV_ADVANCE(1)

				// skip any white space
				while(bytesRemaining > 0 && MFIsWhite(*pFile))
					CSV_ADVANCE(1)
			}
			else
			{
				const char *pFieldStart = pFile;
				while(bytesRemaining > 0 && *pFile && !MFIsNewline(*pFile) && *pFile != ',')
				{
					// we need to keep any escaped codes
					if(*pFile == '\\')
						CSV_ADVANCE(1)
					if(bytesRemaining == 0) // if we ended the file in the middle of an escape sequence...
						return false;
					CSV_ADVANCE(1)
				}

				// gather the field, and strip any trailing white space
				MFString field(pFieldStart, (int)(pFile - pFieldStart));
				field.Trim(false, true);

				// and add it to the row
				row.push(field);
				bEmptyLine = false;
			}

			MFDebug_Assert(MFIsNewline(*pFile) || *pFile == ',', "Expected comma or end of line. Malformed csv file!");
			if(*pFile == ',')
				CSV_ADVANCE(1)
		}

		// seek the end of the line
		while(MFIsNewline(*pFile) && bytesRemaining > 0)
			CSV_ADVANCE(1)
	}

	return true;
}

MF_API MFIntStringTable *MFIntStringTable_CreateFromFileInMemory(const void *pMemory, size_t size, MFIntStringTableFormat format)
{
	const uint8 *pBOM = (const uint8 *)pMemory;
	if(pBOM[0] == 0xEF && pBOM[1] == 0xBB && pBOM[2] == 0xBF)
	{
		// is UTF-8, skip the first 3 bytes
		pMemory = pBOM + 3;
		size -= 3;
	}
	else if(pBOM[0] == 0xFF && pBOM[1] == 0xFE && pBOM[2] == 0x00 && pBOM[3] == 0x00) // little endian UTF-32
	{
		// convert to utf-8 before loading....
		MFDebug_Assert(false, "UTF-32 not yet supported!");
		return NULL;
	}
	else if(pBOM[0] == 0x00 && pBOM[1] == 0x00 && pBOM[2] == 0xFe && pBOM[3] == 0xFF) // big endian UTF-32
	{
		// convert to utf-8 before loading....
		MFDebug_Assert(false, "UTF-32 not yet supported!");
		return NULL;
	}
	else if(pBOM[0] == 0xFF && pBOM[1] == 0xFE) // little endian UTF-16
	{
		// convert to utf-8 before loading....
		MFDebug_Assert(false, "UTF-16 not yet supported!");
		return NULL;
	}
	else if(pBOM[0] == 0xFE && pBOM[1] == 0xFF) // big endian UTF-16
	{
		// convert to utf-8 before loading....
		MFDebug_Assert(false, "UTF-16 not yet supported!");
		return NULL;
	}

	// the output from the string loaders
	MFArray<MFString> languages;
	struct Column
	{
		MFArray<MFString> strings;
	} columns[256];

	// load the strings
	switch(format)
	{
		case MFISTF_CSV:
		{
			const char *pText = (const char *)pMemory;
			size_t remaining = size;

			// read the header
			ReadCSVLine(pText, remaining, languages);

			// read strings...
			MFArray<MFString> row;
			while(ReadCSVLine(pText, remaining, row))
			{
				for(size_t a=0; a<languages.size(); ++a)
				{
					if(!languages[a].IsEmpty())
						columns[a].strings.push(row[a]);
				}
			}
			break;
		}
		default:
			break;
	}

	// produce the string table
	int enumList = -1;
	int englishList = -1;

	int numLanguages = 0;
	int numStrings = 0;
	for(size_t a=0; a<languages.size(); ++a)
	{
		if(languages[a].IsEmpty())
			continue;

		++numLanguages;
		numStrings = columns[a].strings.size();

		if(languages[a].EqualsInsensitive("enum"))
			enumList = a;
		else if(languages[a].EqualsInsensitive("english"))
			englishList = a;
	}

	MFIntStringTable *pStringTable = (MFIntStringTable*)MFHeap_Alloc(sizeof(MFIntStringTable) + sizeof(MFIntStringTable::Table)*numLanguages);
	pStringTable->pLanguages = (MFIntStringTable::Table*)&pStringTable[1];
	pStringTable->numLanguages = numLanguages;
	pStringTable->numStrings = numStrings;
	pStringTable->enumList = -1;
	pStringTable->englishList = -1;

	MFStringCache *pStringCache = MFStringCache_Create(1024*1024);

	for(size_t a=0, l=0; a<languages.size(); ++a)
	{
		if(languages[a].IsEmpty())
			continue;

		// check the language is supported
		MFLanguage language = MFTranslation_GetLanguageByName(languages[a].CStr());
		MFDebug_Assert(language != MFLang_Unknown || languages[a].EqualsInsensitive("enum"), "Unsupported language!");

		// clear the cache
		MFStringCache_Clear(pStringCache);

		// allocate the string table
		size_t headerSize = sizeof(MFStringTable) + sizeof(const char *)*(numStrings-1);
		MFStringTable *pTable = (MFStringTable*)MFHeap_Alloc(headerSize);
		pTable->magic = MFMAKEFOURCC('D','L','G','1');
		pTable->numStrings = numStrings;

		// populate the strings in the cache
		for(int s=0; s<numStrings; ++s)
		{
			MFString str = columns[a].strings[s].CStr();
			if(str.IsEmpty())
				str = columns[englishList].strings[s];
			if(str.IsEmpty())
				str = columns[enumList].strings[s];
			if(str.IsEmpty())
				str = "[UNDEFINED STRING]";

			pTable->pStrings[s] = MFStringCache_Add(pStringCache, str.CStr());
		}

		// resize the table to include the string cache
		size_t cacheSize = MFStringCache_GetSize(pStringCache);
		pTable = (MFStringTable*)MFHeap_Realloc(pTable, headerSize + cacheSize);

		// copy the string cache into the table
		const char *pOldBuffer = MFStringCache_GetCache(pStringCache);
		char *pNewBuffer = (char *)&pTable->pStrings[pTable->numStrings];
		MFCopyMemory(pNewBuffer, pOldBuffer, cacheSize);

		// re-point the string to the new buffer
		for(int s=0; s<numStrings; ++s)
		{
			pTable->pStrings[s] -= (size_t)pOldBuffer;
			pTable->pStrings[s] += (size_t)pNewBuffer;
		}

		// and add it to the final set
		MFIntStringTable::Table &table = pStringTable->pLanguages[l];
		table.pTemplate = pTable;
		table.size = headerSize + cacheSize;
		table.language = language;

		// set some useful details
		if(enumList == (int)a)
			pStringTable->enumList = l;
		if(englishList == (int)a)
			pStringTable->englishList = l;
		++l;
	}

	MFStringCache_Destroy(pStringCache);

	return pStringTable;
}

MF_API void MFIntStringTable_Destroy(MFIntStringTable *pStringTable)
{
	for(int a=0; a<pStringTable->numLanguages; ++a)
		MFHeap_Free(pStringTable->pLanguages[a].pTemplate);

	MFHeap_Free(pStringTable);
}

MF_API MFStringTable *MFIntStringTable_CreateRuntimeData(MFIntStringTable *pStringTable, MFLanguage language, MFPlatform platform, size_t *pSize)
{
	for(int a=0; a<pStringTable->numLanguages; ++a)
	{
		if(pStringTable->pLanguages[a].language == language)
		{
			MFIntStringTable::Table &l = pStringTable->pLanguages[a];

			// collapse the pointers for export
			for(int s=0; s<l.pTemplate->numStrings; ++s)
				l.pTemplate->pStrings[s] -= (size_t)l.pTemplate;

			MFStringTable *pTable = (MFStringTable*)MFHeap_Alloc(l.size);
			MFCopyMemory(pTable, l.pTemplate, l.size);
			if(pSize)
				*pSize = l.size;

			// fix them up again, just incase we want to use it again...
			for(int s=0; s<l.pTemplate->numStrings; ++s)
				l.pTemplate->pStrings[s] += (size_t)l.pTemplate;

			return pTable;
		}
	}
	return NULL;
}

MF_API void MFIntStringTable_WriteEnumsToHeaderFile(MFIntStringTable *pStringTable, const char *pFilename)
{

}
