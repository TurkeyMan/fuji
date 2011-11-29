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

MFIntStringTable *MFIntStringTable_CreateFromFile(const char *pFilename);
MFIntStringTable *MFIntStringTable_CreateFromFileInMemory(const void *pMemory, size_t size, MFIntStringTableFormat format);

void MFIntStringTable_Destroy(MFIntStringTable *pStringTable);

MFStringTable *MFIntStringTable_CreateRuntimeData(MFIntStringTable *pStringTable, MFLanguage language, MFPlatform platform, size_t *pSize);

void MFIntStringTable_WriteEnumsToHeaderFile(MFIntStringTable *pStringTable, const char *pFilename);

#endif
