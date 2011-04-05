#if !defined(_MFCOMPUTE_H)
#define _MFCOMPUTE_H

//---------------------------------------------------------------------------------------------------------------------
enum MFCompute_BufferType
{
	MFCBT_ReadOnly,
	MFCBT_ReadOnlyCopy,
	MFCBT_WriteOnly,
	
	MFCBT_Max,
};
//---------------------------------------------------------------------------------------------------------------------
enum MFCompute_ScalarType
{
	MFCST_Bool,
	MFCST_Int8,
	MFCST_UInt8,
	MFCST_Int16,
	MFCST_UInt16,
	MFCST_Int32,
	MFCST_UInt32,
	MFCST_Int64,
	MFCST_Uint64,
	MFCST_Float,
	MFCST_Half,
	MFCST_Size,
	MFCST_PtrDiff,
	MFCST_IntPtr,
	MFCST_UIntPtr,
	MFCST_Void,

	MFCST_Max,
};

//---------------------------------------------------------------------------------------------------------------------
struct MFComputeBuffer;

//---------------------------------------------------------------------------------------------------------------------
struct MFComputeProgram;

//---------------------------------------------------------------------------------------------------------------------
struct MFComputeKernel;

//---------------------------------------------------------------------------------------------------------------------
MFComputeBuffer *MFCompute_CreateBuffer(MFCompute_ScalarType dataType, int count, void *host_ptr, MFCompute_BufferType type);

//---------------------------------------------------------------------------------------------------------------------
void MFCompute_DestroyBuffer(MFComputeBuffer *pBuffer);

//---------------------------------------------------------------------------------------------------------------------
MFComputeProgram *MFCompute_CreateProgram(const char* source, size_t size);

//---------------------------------------------------------------------------------------------------------------------
void MFCompute_DestroyProgram(MFComputeProgram *pProgram);

//---------------------------------------------------------------------------------------------------------------------
MFComputeKernel *MFCompute_CreateKernel(MFComputeProgram *pProgram, const char *kernel_name);

//---------------------------------------------------------------------------------------------------------------------
void MFCompute_DestroyKernel(MFComputeKernel *pKernel);

//---------------------------------------------------------------------------------------------------------------------
bool MFCompute_SetKernelArg(MFComputeKernel *pKernel, uint32 arg_index, MFComputeBuffer *pBuffer);

//---------------------------------------------------------------------------------------------------------------------
bool MFCompute_SetKernelArg(MFComputeKernel *pKernel, uint32 arg_index, uint32 arg_size, const void *arg_value);

//---------------------------------------------------------------------------------------------------------------------
void MFCompute_Enqueue(MFComputeKernel *pKernel, uint32 workItems);

//---------------------------------------------------------------------------------------------------------------------
void MFCompute_ReadBuffer(MFComputeBuffer *pBuffer, uint32 size, void *ptr);

//---------------------------------------------------------------------------------------------------------------------
#endif
