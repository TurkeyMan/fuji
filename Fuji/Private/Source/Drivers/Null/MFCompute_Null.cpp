
#include "Fuji.h"


#if MF_COMPUTE == MF_DRIVER_NULL

#include "MFCompute_Internal.h"

void MFCompute_InitModule()
{
}

void MFCompute_DeinitModule()
{
}

//---------------------------------------------------------------------------------------------------------------------
MFComputeBuffer *MFCompute_CreateBuffer(MFCompute_ScalarType dataType, int count, void *host_ptr, MFCompute_BufferType type)
{
	return NULL;
}

//---------------------------------------------------------------------------------------------------------------------
void MFCompute_DestroyBuffer(MFComputeBuffer *pBuffer)
{
}

//---------------------------------------------------------------------------------------------------------------------
MFComputeProgram *MFCompute_CreateProgram(const char* source, size_t size)
{
	return NULL;
}

//---------------------------------------------------------------------------------------------------------------------
void MFCompute_DestroyProgram(MFComputeProgram *pProgram)
{
}

//---------------------------------------------------------------------------------------------------------------------
MFComputeKernel *MFCompute_CreateKernel(MFComputeProgram *pProgram, const char *kernel_name)
{
	return NULL;
}

//---------------------------------------------------------------------------------------------------------------------
void MFCompute_DestroyKernel(MFComputeKernel *pKernel)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool MFCompute_SetKernelArg(MFComputeKernel *pKernel, uint32 arg_index, MFComputeBuffer *pBuffer)
{
	return false;
}

//---------------------------------------------------------------------------------------------------------------------
bool MFCompute_SetKernelArg(MFComputeKernel *pKernel, uint32 arg_index, uint32 arg_size, const void *arg_value)
{
	return false;
}

//---------------------------------------------------------------------------------------------------------------------
void MFCompute_Enqueue(MFComputeKernel *pKernel, uint32 workItems)
{
}

//---------------------------------------------------------------------------------------------------------------------
void MFCompute_ReadBuffer(MFComputeBuffer *pBuffer, uint32 size, void *ptr)
{
}

//---------------------------------------------------------------------------------------------------------------------

#endif