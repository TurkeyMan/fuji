#pragma once
#if !defined(_MFINTSTRINGTABLE_H)
#define _MFINTSTRINGTABLE_H

#include "MFTranslation.h"

enum MFIntStringTableFormat
{
	MFISTF_Unknown = -1,

	MFISTF_CSV = 0,

	MFISTF_Max,
	MFISTF_ForceInt = 0x7FFFFFFF
};

struct MFIntStringTable;

MF_API MFIntStringTable *MFIntStringTable_CreateFromFile(const char *pFilename);
MF_API MFIntStringTable *MFIntStringTable_CreateFromFileInMemory(const void *pMemory, size_t size, MFIntStringTableFormat format);

MF_API void MFIntStringTable_Destroy(MFIntStringTable *pStringTable);

MF_API MFStringTable *MFIntStringTable_CreateRuntimeData(MFIntStringTable *pStringTable, MFLanguage language, MFPlatform platform, size_t *pSize);

MF_API void MFIntStringTable_WriteEnumsToHeaderFile(MFIntStringTable *pStringTable, const char *pFilename);

#endif
