#include "Common.h"
#include "MFModel_Internal.h"

PtrListDL<MFModel> gModelBank;

void MFModel_InitModule()
{
	gModelBank.Init("Model Bank", gDefaults.model.maxModels);
}

void MFModel_DeinitModule()
{
	gModelBank.Deinit();
}

