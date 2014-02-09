module fuji.sound;

public import fuji.fuji;

struct MFShader
{
	@disable this();
	@disable this(this);
}

alias extern (C) void function() MFShader_ConfigureCallback;
alias extern (C) void function(void* pWorkload) MFShader_ExecuteCallback;

enum MFShaderType
{
	VertexShader,
	PixelShader,
	GeometryShader,
	DomainShader,
	HullShader,
	ComputeShader
}

enum MFShaderInputType
{
	Unknown = -1,

	Vector = 0,
	Matrix,
	Float,
	Int,
	Bool,
	Sampler
}

struct MFShaderMacro
{
	const char* pDefine;
	const char* pValue;
}

struct MFShaderInput
{
	const char* pName;
	MFShaderInputType type;
	// TODO: fix me!
//	uint32 numRows			: 3;
//	uint32 columnMajor		: 1;
//	uint32 numElements		: 12;
//	uint32 constantRegister	: 8;
//	uint32 numRegisters		: 8;
}

extern (C) MFShader* MFShader_CreateFromFile(MFShaderType type, const(char)* pFilename, MFShaderMacro* pMacros = null);
extern (C) MFShader* MFShader_CreateFromString(MFShaderType type, const(char)* pShader, MFShaderMacro* pMacros = null, const(char)* pName = null, const(char)* pFilename = null, int startingLine = 0);
extern (C) MFShader* MFShader_CreateFromCallbacks(MFShaderType type, MFShader_ConfigureCallback configureFunc, MFShader_ExecuteCallback executeFunc, const(char)* pName = null);
extern (C) int MFShader_Release(MFShader* pShader);
