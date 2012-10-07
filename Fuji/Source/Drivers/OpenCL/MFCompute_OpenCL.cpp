
#include "Fuji.h"


#if MF_COMPUTE == MF_DRIVER_OPENCL

#include "MFCompute.h"
#include "MFHeap.h"

#include <CL/cl.h>
//#include <CL/cl_gl.h>
//#include <CL/cl_gl_ext.h>
#include <CL/cl_ext.h>

//---------------------------------------------------------------------------------------------------------------------
struct MFComputeBuffer
{
	cl_mem mem;
	void *host_ptr;
	MFCompute_ScalarType dataType;
	uint32 count;
};
//---------------------------------------------------------------------------------------------------------------------
struct MFComputeProgram
{
	cl_program program;
};
//---------------------------------------------------------------------------------------------------------------------
struct MFComputeKernel
{
	MFComputeProgram* pProgram;
	cl_kernel kernel;
};
//---------------------------------------------------------------------------------------------------------------------
cl_platform_id			g_OpenCL_platform = NULL;
cl_context				g_OpenCL_context = NULL;
cl_command_queue		g_OpenCL_queue = NULL;
cl_device_id			g_OpenCL_device = NULL;

//---------------------------------------------------------------------------------------------------------------------
static cl_mem_flags bufferCreateFlags[MFCBT_Max] =
{
	CL_MEM_READ_ONLY,						//MFCBT_ReadOnly,
	CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,	//MFCBT_ReadOnlyCopy,
	CL_MEM_WRITE_ONLY,						//MFCBT_WriteOnly,
};
//---------------------------------------------------------------------------------------------------------------------
static uint32 scalarDataSize[MFCST_Max] =
{
	sizeof(bool), // MFCST_Bool,
	1, // MFCST_Int8,
	1, // MFCST_UInt8,
	2, // MFCST_Int16,
	2, // MFCST_UInt16,
	4, // MFCST_Int32,
	4, // MFCST_UInt32,
	8, // MFCST_Int64,
	8, // MFCST_Uint64,
	4, // MFCST_Float,
	2, // MFCST_Half,
	sizeof(size_t), // MFCST_Size,
	sizeof(ptrdiff_t), // MFCST_PtrDiff,
	sizeof(intptr_t), // MFCST_IntPtr,
	sizeof(uintptr_t), // MFCST_UIntPtr,
	0, // MFCST_Void,
};
//---------------------------------------------------------------------------------------------------------------------
static size_t shrRoundUp(size_t local_work_size, size_t size)
{
	size_t out = local_work_size;
	while (out < size)
	{
		out += local_work_size;
	}
	return out;
}
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
MFInitStatus MFCompute_InitModule()
{
	const int MAX_PLATFORMS = 32;
	cl_platform_id platforms[MAX_PLATFORMS];

	cl_int error = 0;   // Used to handle error codes

	cl_uint platformCount = 0;

	error = clGetPlatformIDs(MAX_PLATFORMS, platforms, &platformCount);
	if (error != CL_SUCCESS)
	{
		MFDebug_Error("Couldn't get OpenCL platforms");
	}

	for (uint32 i = 0; i < platformCount; ++i)
	{
		char profile[32];
		char version[32];
		char name[32];
		char vendor[32];
		char extentions[512];
		
		error = clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, sizeof(profile), profile, NULL);
		error = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(version), version, NULL);
		error = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(name), name, NULL);
		error = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(vendor), vendor, NULL);
		error = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, sizeof(extentions), extentions, NULL);
		error = error;
	}

	//TODO find prefered platform
	g_OpenCL_platform = platforms[0];


	error = clGetDeviceIDs(g_OpenCL_platform, CL_DEVICE_TYPE_GPU, 1, &g_OpenCL_device, NULL);
	if (error != CL_SUCCESS)
	{
		MFDebug_Error("Couldn't get OpenCL devices");
	}

	g_OpenCL_context = clCreateContext(0, 1, &g_OpenCL_device, NULL, NULL, &error);
	if (error != CL_SUCCESS)
	{
		MFDebug_Error("Couldn't create OpenCL context");
	}

	g_OpenCL_queue = clCreateCommandQueue(g_OpenCL_context, g_OpenCL_device, 0, &error);
	if (error != CL_SUCCESS)
	{
		MFDebug_Error("Couldn't create OpenCL queue");
	}
}
//---------------------------------------------------------------------------------------------------------------------
void MFCompute_DeinitModule()
{
	cl_int error = 0;   // Used to handle error codes
	
	error = clReleaseCommandQueue(g_OpenCL_queue);
	if (error != CL_SUCCESS)
	{
		MFDebug_Error("Error releasing OpenCL queue");
	}
	g_OpenCL_queue = 0;

	error = clReleaseContext(g_OpenCL_context);
	if (error != CL_SUCCESS)
	{
		MFDebug_Error("Error releasing OpenCL context");
	}
	g_OpenCL_context = 0;
}
//---------------------------------------------------------------------------------------------------------------------
MF_API MFComputeBuffer *MFCompute_CreateBuffer(MFCompute_ScalarType dataType, int count, void *host_ptr, MFCompute_BufferType type)
{
	MFComputeBuffer *pBuffer = NULL;
	cl_int error = 0;
	const uint32 mem_size = scalarDataSize[dataType] * count;
	cl_mem mem = clCreateBuffer(g_OpenCL_context, bufferCreateFlags[type], mem_size, host_ptr, &error);
	if (error == CL_SUCCESS)
	{
		pBuffer = reinterpret_cast<MFComputeBuffer *>(MFHeap_AllocAndZero(sizeof(MFComputeBuffer)));
		pBuffer->mem = mem;
		pBuffer->host_ptr = host_ptr;
		pBuffer->dataType = dataType;
		pBuffer->count = count;
	}
	return pBuffer;
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFCompute_DestroyBuffer(MFComputeBuffer *pBuffer)
{
	if (pBuffer)
	{
		clReleaseMemObject(pBuffer->mem);
		// SJS TODO
		// don't leak pBuffer->host_ptr
		MFHeap_Free(pBuffer);
	}
}
//---------------------------------------------------------------------------------------------------------------------
MF_API MFComputeProgram *MFCompute_CreateProgram(const char* source, size_t size)
{
	MFComputeProgram *pProgram = NULL;
	cl_int error = 0;
	const char **strings = &source;
	size_t *lengths = &size;
	cl_program program = clCreateProgramWithSource(g_OpenCL_context, 1, strings, lengths, &error);
	if (error == CL_SUCCESS)
	{
		error = clBuildProgram(program, 1, &g_OpenCL_device, NULL, NULL, NULL);

		if (error == CL_SUCCESS)
		{
			pProgram = reinterpret_cast<MFComputeProgram *>(MFHeap_AllocAndZero(sizeof(MFComputeProgram)));
			pProgram->program = program;

			{
				char* build_log;
				size_t log_size;
				// First call to know the proper size
				clGetProgramBuildInfo(program, g_OpenCL_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
				build_log = new char[log_size+1];
				// Second call to get the log
				clGetProgramBuildInfo(program, g_OpenCL_device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
				build_log[log_size] = '\0';
				MFDebug_Message(build_log);
				delete [] build_log;
			}
		}
	}
	return pProgram;
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFCompute_DestroyProgram(MFComputeProgram *pProgram)
{
	if (pProgram)
	{
		clReleaseProgram(pProgram->program);
		MFHeap_Free(pProgram);
	}
}
//---------------------------------------------------------------------------------------------------------------------
MF_API MFComputeKernel *MFCompute_CreateKernel(MFComputeProgram *pProgram, const char *kernel_name)
{
	MFComputeKernel *pKernel = NULL;
	cl_int error = 0;
	cl_kernel kernel = clCreateKernel(pProgram->program, kernel_name, &error);
	if (error == CL_SUCCESS)
	{
		pKernel = reinterpret_cast<MFComputeKernel *>(MFHeap_AllocAndZero(sizeof(MFComputeBuffer)));
		pKernel->pProgram = pProgram;
		pKernel->kernel = kernel;
	}
	return pKernel;
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFCompute_DestroyKernel(MFComputeKernel *pKernel)
{
	if (pKernel)
	{
		clReleaseKernel(pKernel->kernel);
		MFHeap_Free(pKernel);
	}
}
MF_API bool MFCompute_SetKernelArgBuffer(MFComputeKernel *pKernel, uint32 arg_index, MFComputeBuffer *pBuffer)
{
	cl_int error = 0;
	error = clSetKernelArg(pKernel->kernel, arg_index, sizeof(cl_mem), &pBuffer->mem);
	return (error == CL_SUCCESS);
}
//---------------------------------------------------------------------------------------------------------------------
MF_API bool MFCompute_SetKernelArg(MFComputeKernel *pKernel, uint32 arg_index, uint32 arg_size, const void *arg_value)
{
	cl_int error = 0;
	error = clSetKernelArg(pKernel->kernel, arg_index, arg_size, arg_value);
	return (error == CL_SUCCESS);
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFCompute_Enqueue(MFComputeKernel *pKernel, uint32 workItems)
{
	const size_t local_work_size = 512;
	const size_t global_work_size = shrRoundUp(local_work_size, workItems);

	cl_int error = 0;
	error = clEnqueueNDRangeKernel(g_OpenCL_queue, pKernel->kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
	error = error;
}
//---------------------------------------------------------------------------------------------------------------------
MF_API void MFCompute_ReadBuffer(MFComputeBuffer *pBuffer, uint32 size, void *ptr)
{
	cl_int error = 0;
	error = clEnqueueReadBuffer(g_OpenCL_queue, pBuffer->mem, CL_TRUE, 0, size, ptr, 0, NULL, NULL);
	error = error;
}
//---------------------------------------------------------------------------------------------------------------------
#endif
