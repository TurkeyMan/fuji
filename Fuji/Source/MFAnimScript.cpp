#include "Fuji.h"
#include "MFAnimScript_Internal.h"
#include "MFPtrList.h"
#include "MFHeap.h"

MFPtrList<MFAnimScriptTemplate> gAnimScripts;

MFInitStatus MFAnimScript_InitModule()
{
	gAnimScripts.Init("Anim Script Templates", 256);

	return MFIS_Succeeded;
}

void MFAnimScript_DeinitModule()
{
	gAnimScripts.Deinit();
}

MFAnimScriptTemplate *pMFAnimScriptInternal_Find(const char *pScript)
{
	MFAnimScriptTemplate **ppIterator = gAnimScripts.Begin();

	while(*ppIterator)
	{
		if(!MFString_CaseCmp(pScript, (char*)*ppIterator + sizeof(MFAnimScriptTemplate)))
			return *ppIterator;

		ppIterator++;
	}

	return NULL;
}

MF_API MFAnimScript* MFAnimScript_Create(const char *pFilename, MFModel *pModel)
{
	MFAnimScriptTemplate *pTemplate = pMFAnimScriptInternal_Find(pFilename);

	if(!pTemplate)
	{
		MFIni *pIni = MFIni::Create(pFilename);

		if(!pIni)
		{
			MFDebug_Assert(false, MFStr("Couldnt load anim script '%s'.", pFilename));
			return NULL;
		}

		pTemplate = (MFAnimScriptTemplate*)MFHeap_Alloc(sizeof(MFAnimScriptTemplate) + MFString_Length(pFilename) + 1);
		MFString_Copy((char*)pTemplate + sizeof(MFAnimScriptTemplate), pFilename);

		pTemplate->pAnimScript = pIni;
		pTemplate->refCount = 0;

		// count sequences
		pTemplate->numSequences = 0;

		MFIniLine *pLine = pIni->GetFirstLine();

		while(pLine)
		{
			if(pLine->IsString(0, "anim"))
				++pTemplate->numSequences;

			pLine = pLine->Next();
		}

		// load sequences
		pTemplate->pSequences = (MFAnimScriptSequence*)MFHeap_Alloc(sizeof(MFAnimScriptSequence) * pTemplate->numSequences);
		MFAnimScriptSequence *pSequence = pTemplate->pSequences;

		pLine = pIni->GetFirstLine();

		while(pLine)
		{
			if(pLine->IsString(0, "anim"))
			{
				pSequence->pName = pLine->GetString(1);

				MFIniLine *pAnim = pLine->Sub();

				if(pAnim)
				{
					// load animation settings...

					pAnim = pAnim->Next();
				}

				++pSequence;
			}

			pLine = pLine->Next();
		}
	}

	MFAnimScript *pScript = (MFAnimScript*)MFHeap_Alloc(sizeof(MFAnimScript));

	pScript->pTemplate = pTemplate;
	pScript->pModel = pModel;

	pScript->currentSequence = -1;
	pScript->currentFrame = 0.0f;

	++pTemplate->refCount;

	return pScript;
}

MF_API void MFAnimScript_Destroy(MFAnimScript *pAnimScript)
{
	--pAnimScript->pTemplate->refCount;

	if(!pAnimScript->pTemplate->refCount)
		MFHeap_Free(pAnimScript->pTemplate);

	MFHeap_Free(pAnimScript);
}

MF_API int MFAnimScript_GetNumSequences(MFAnimScript *pAnimScript)
{
	return pAnimScript->pTemplate->numSequences;
}

MF_API void MFAnimScript_PlaySequence(MFAnimScript *pAnimScript, int sequence, float tweenTime)
{
	pAnimScript->currentSequence = sequence;
	pAnimScript->currentFrame = pAnimScript->pTemplate->pSequences[sequence].startFrame;
}

MF_API const char* MFAnimScript_GetSequenceName(MFAnimScript *pAnimScript, int sequence)
{
	return pAnimScript->pTemplate->pSequences[sequence].pName;
}

MF_API int MFAnimScript_FindSequence(MFAnimScript *pAnimScript, const char *pSequenceName)
{
	for(int a=0; a<pAnimScript->pTemplate->numSequences; a++)
	{
		if(!MFString_CaseCmp(pSequenceName, pAnimScript->pTemplate->pSequences[a].pName))
			return a;
	}

	return -1;
}
