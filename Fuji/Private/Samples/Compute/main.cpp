#include "Fuji.h"
#include "MFSystem.h"
#include "MFHeap.h"
#include "MFCompute.h"
#include "MFRenderer.h"

//---------------------------------------------------------------------------------------------------------------------
/**** Globals ****/
//---------------------------------------------------------------------------------------------------------------------

static const int BUFFER_SIZE = 1024*1024;

static float* pBufferMemA = NULL;
static float* pBufferMemB = NULL;
static float* pBufferMemR = NULL;
static float* pBufferMemX = NULL;

static MFComputeBuffer *pBufferA = NULL;
static MFComputeBuffer *pBufferB = NULL;
static MFComputeBuffer *pBufferR = NULL;

static MFComputeProgram *pProgram = NULL;

static MFComputeKernel *pKernel = NULL;

//---------------------------------------------------------------------------------------------------------------------
static const char* pSource = ""
	"// this is the source of the program\n"
	"__kernel void vector_add_gpu (__global const float* src_a, __global const float* src_b, __global float* res, const int num)\n"
	"{\n"
	"   /* get_global_id(0) returns the ID of the thread in execution.\n"
	"   As many threads are launched at the same time, executing the same kernel,\n"
	"   each one will receive a different ID, and consequently perform a different computation.*/\n"
	"   const int idx = get_global_id(0);\n"
	"\n"
	"   /* Now each work-item asks itself: \"is my ID inside the vector's range?\"\n"
	"   If the answer is YES, the work-item performs the corresponding computation*/\n"
	"   if (idx < num)\n"
	"      res[idx] = src_a[idx] + src_b[idx];\n"
	"}\n"
	;

//---------------------------------------------------------------------------------------------------------------------
/**** Functions ****/
//---------------------------------------------------------------------------------------------------------------------
void Game_Init()
{
	MFCALLSTACK;

	pBufferMemA = reinterpret_cast<float*>(MFHeap_AllocAndZero(sizeof(float) * BUFFER_SIZE));
	pBufferMemB = reinterpret_cast<float*>(MFHeap_AllocAndZero(sizeof(float) * BUFFER_SIZE));
	pBufferMemR = reinterpret_cast<float*>(MFHeap_AllocAndZero(sizeof(float) * BUFFER_SIZE));
	pBufferMemX = reinterpret_cast<float*>(MFHeap_AllocAndZero(sizeof(float) * BUFFER_SIZE));

	for (int i = 0; i < BUFFER_SIZE; ++i)
	{
		pBufferMemA[i] = pBufferMemB[i] = static_cast<float>(i);
	}

	pBufferA = MFCompute_CreateBuffer(MFCST_Float, BUFFER_SIZE, pBufferMemA, MFCBT_ReadOnlyCopy);
	pBufferB = MFCompute_CreateBuffer(MFCST_Float, BUFFER_SIZE, pBufferMemB, MFCBT_ReadOnlyCopy);
	pBufferR = MFCompute_CreateBuffer(MFCST_Float, BUFFER_SIZE, pBufferMemR, MFCBT_WriteOnly);

	pProgram = MFCompute_CreateProgram(pSource, MFString_Length(pSource));

	pKernel = MFCompute_CreateKernel(pProgram, "vector_add_gpu");

	bool set = false;

	size_t size = BUFFER_SIZE;
	
	MFCompute_SetKernelArg(pKernel, 0, pBufferA);
	MFCompute_SetKernelArg(pKernel, 1, pBufferB);
	MFCompute_SetKernelArg(pKernel, 2, pBufferR);
	MFCompute_SetKernelArg(pKernel, 3, sizeof(size_t), &size);

	MFCompute_Enqueue(pKernel, BUFFER_SIZE);

	MFCompute_ReadBuffer(pBufferR, sizeof(float) * BUFFER_SIZE, pBufferMemX);
}
//---------------------------------------------------------------------------------------------------------------------
void Game_Deinit()
{
	MFCALLSTACK;

	MFCompute_DestroyKernel(pKernel); pKernel = NULL;
	
	MFCompute_DestroyProgram(pProgram); pProgram = NULL;
	
	MFCompute_DestroyBuffer(pBufferR); pBufferR = NULL;
	MFCompute_DestroyBuffer(pBufferB); pBufferB = NULL;
	MFCompute_DestroyBuffer(pBufferA); pBufferA = NULL;

	MFHeap_Free(pBufferMemA); pBufferMemA = NULL;
	MFHeap_Free(pBufferMemB); pBufferMemB = NULL;
	MFHeap_Free(pBufferMemR); pBufferMemR = NULL;
}
//---------------------------------------------------------------------------------------------------------------------
void Game_Update()
{
	MFCALLSTACK;
}
//---------------------------------------------------------------------------------------------------------------------
void Game_Draw()
{
	MFCALLSTACK;

	MFRenderer_SetClearColour(0.5f, 0.0f, 0.2f, 1.0f);
	MFRenderer_ClearScreen();

}
//---------------------------------------------------------------------------------------------------------------------
int GameMain(MFInitParams *pInitParams)
{
	MFRand_Seed((uint32)MFSystem_ReadRTC());

	MFSystem_RegisterSystemCallback(MFCB_InitDone, Game_Init);
	MFSystem_RegisterSystemCallback(MFCB_Update, Game_Update);
	MFSystem_RegisterSystemCallback(MFCB_Draw, Game_Draw);
	MFSystem_RegisterSystemCallback(MFCB_Deinit, Game_Deinit);

	return MFMain(pInitParams);
}
//---------------------------------------------------------------------------------------------------------------------


#if defined(MF_WINDOWS) || defined(_WINDOWS)
#include <windows.h>

//---------------------------------------------------------------------------------------------------------------------
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
	MFInitParams initParams;
	MFZeroMemory(&initParams, sizeof(MFInitParams));
	initParams.hInstance = hInstance;
	initParams.pCommandLine = lpCmdLine;

	return GameMain(&initParams);
}
//---------------------------------------------------------------------------------------------------------------------

#elif defined(MF_PSP) || defined(_PSP)
#include <pspkernel.h>

//---------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	MFInitParams initParams;
	MFZeroMemory(&initParams, sizeof(MFInitParams));
	initParams.argc = argc;
	initParams.argv = argv;

	int r = GameMain(&initParams);

	sceKernelExitGame();
	return r;
}
//---------------------------------------------------------------------------------------------------------------------

#else

//---------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	MFInitParams initParams;
	MFZeroMemory(&initParams, sizeof(MFInitParams));
	initParams.argc = argc;
	initParams.argv = argv;

	return GameMain(&initParams);
}
//---------------------------------------------------------------------------------------------------------------------

#endif
