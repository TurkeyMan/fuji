module fuji.compute;

enum MFCompute_BufferType
{
	ReadOnly,
	ReadOnlyCopy,
	WriteOnly
}

enum MFCompute_ScalarType
{
	Bool,
	Int8,
	UInt8,
	Int16,
	UInt16,
	Int32,
	UInt32,
	Int64,
	Uint64,
	Float,
	Half,
	Size,
	PtrDiff,
	IntPtr,
	UIntPtr,
	Void
}

struct MFComputeBuffer;

struct MFComputeProgram;

struct MFComputeKernel;

extern (C) MFComputeBuffer* MFCompute_CreateBuffer(MFCompute_ScalarType dataType, int count, void* host_ptr, MFCompute_BufferType type);

extern (C) void MFCompute_DestroyBuffer(MFComputeBuffer* pBuffer);

extern (C) MFComputeProgram* MFCompute_CreateProgram(const(char*) source, size_t size);

extern (C) void MFCompute_DestroyProgram(MFComputeProgram* pProgram);

extern (C) MFComputeKernel* MFCompute_CreateKernel(MFComputeProgram* pProgram, const(char*) kernel_name);

extern (C) void MFCompute_DestroyKernel(MFComputeKernel* pKernel);

extern (C) bool MFCompute_SetKernelArgBuffer(MFComputeKernel* pKernel, uint arg_index, MFComputeBuffer* pBuffer);

extern (C) bool MFCompute_SetKernelArg(MFComputeKernel* pKernel, uint arg_index, uint arg_size, const(void*) arg_value);

extern (C) void MFCompute_Enqueue(MFComputeKernel* pKernel, uint workItems);

extern (C) void MFCompute_ReadBuffer(MFComputeBuffer* pBuffer, uint size, void* ptr);

