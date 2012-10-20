module fuji.asset.intstringtable;

import fuji.fuji;
import fuji.translation;

enum MFIntStringTableFormat
{
	Unknown = -1,

	CSV = 0
}

struct MFIntStringTable;

extern (C) MFIntStringTable* MFIntStringTable_CreateFromFile(const(char*) pFilename);
extern (C) MFIntStringTable* MFIntStringTable_CreateFromFileInMemory(const(void*) pMemory, size_t size, MFIntStringTableFormat format);

extern (C) void MFIntStringTable_Destroy(MFIntStringTable* pStringTable);

extern (C) MFStringTable* MFIntStringTable_CreateRuntimeData(MFIntStringTable* pStringTable, MFLanguage language, MFPlatform platform, size_t* pSize);

extern (C) void MFIntStringTable_WriteEnumsToHeaderFile(MFIntStringTable* pStringTable, const(char*) pFilename);

