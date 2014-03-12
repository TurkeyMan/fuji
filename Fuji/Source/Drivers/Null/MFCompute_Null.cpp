
#include "Fuji_Internal.h"


#if MF_COMPUTE == MF_DRIVER_NULL

#include "MFCompute_Internal.h"

MFInitStatus MFCompute_InitModule(int moduleId, bool bPerformInitialisation)
{
	return MFIS_Succeeded;
}

void MFCompute_DeinitModule()
{
}

//---------------------------------------------------------------------------------------------------------------------
MF_API MFComputeBuffer *MFCompute_CreateBuffer(MFCompute_ScalarType dataType, int count, void *host_ptr, MFCompute_BufferType type)
{
	return NULL;
}

//---------------------------------------------------------------------------------------------------------------------
MF_API void MFCompute_DestroyBuffer(MFComputeBuffer *pBuffer)
{
}

//---------------------------------------------------------------------------------------------------------------------
MF_API MFComputeProgram *MFCompute_CreateProgram(const char* source, size_t size)
{
	return NULL;
}

//---------------------------------------------------------------------------------------------------------------------
MF_API void MFCompute_DestroyProgram(MFComputeProgram *pProgram)
{
}

//---------------------------------------------------------------------------------------------------------------------
MF_API MFComputeKernel *MFCompute_CreateKernel(MFComputeProgram *pProgram, const char *kernel_name)
{
	return NULL;
}

//---------------------------------------------------------------------------------------------------------------------
MF_API void MFCompute_DestroyKernel(MFComputeKernel *pKernel)
{
}

//---------------------------------------------------------------------------------------------------------------------
MF_API bool MFCompute_SetKernelArgBuffer(MFComputeKernel *pKernel, uint32 arg_index, MFComputeBuffer *pBuffer)
{
	return false;
}

//---------------------------------------------------------------------------------------------------------------------
MF_API bool MFCompute_SetKernelArg(MFComputeKernel *pKernel, uint32 arg_index, uint32 arg_size, const void *arg_value)
{
	return false;
}

//---------------------------------------------------------------------------------------------------------------------
MF_API void MFCompute_Enqueue(MFComputeKernel *pKernel, uint32 workItems)
{
}

//---------------------------------------------------------------------------------------------------------------------
MF_API void MFCompute_ReadBuffer(MFComputeBuffer *pBuffer, uint32 size, void *ptr)
{
}

//---------------------------------------------------------------------------------------------------------------------

#endif