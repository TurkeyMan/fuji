#if !defined(_CONVERTTEX_H)
#define _CONVERTTEX_H

enum TargetPlatform
{
	TP_Unknown = -1,

	TP_PC = 0,
	TP_XBox,
	TP_Linux,
	TP_PSP,

	TP_Max
};

enum ImageFormats
{
	TexFmt_Unknown = -1,

	TexFmt_A8R8G8B8,	// PC/XBox
	TexFmt_A8B8G8R8,	// PSP

	TexFmt_A2R10G10B10,	// DX9
	TexFmt_A2B10G10R10,	// DX9
	TexFmt_A16B16G16R16,// DX9

	TexFmt_R5G6B5,		// PC
	TexFmt_A1R5G5B5,	// PC

	TexFmt_A4R4G4B4,	// PC
	TexFmt_A4B4G4R4,	// PSP

	TexFmt_ABGR_F16,	// DX9  s1-e5-m10 - seeeeemmmmmmmmmm
	TexFmt_ABGR_F32,	// DX9

	TexFmt_I8,			// PSP
	TexFmt_I4,			// PSP

	TexFmt_DXT1,		// PSP/PC/XBox
	TexFmt_DXT2,
	TexFmt_DXT3,		// PSP/PC/XBox
	TexFmt_DXT4,
	TexFmt_DXT5,		// PSP/PC/XBox

	TexFmt_Max
};

template<class T>
inline void FixUp(T* &pPointer, void *pBase, int fix)
{
	if(pPointer)
	{
		int offset = (int&)pBase;

		if(!fix)
			offset = -offset;

		pPointer = (T*)((char*)pPointer + offset);
	}
}

#endif