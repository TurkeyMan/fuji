#if !defined(_MFANIMSCRIPT_INTERNAL_H)
#define _MFANIMSCRIPT_INTERNAL_H

#include "MFAnimScript.h"
#include "MFIni.h"

MFInitStatus MFAnimScript_InitModule(int moduleId, bool bPerformInitialisation);
void MFAnimScript_DeinitModule();

struct MFAnimScriptSequence
{
	const char *pName;

	float startFrame, endFrame;
	float frameRate;
};

struct MFAnimScriptTemplate
{
	MFIni *pAnimScript;

	MFAnimScriptSequence *pSequences;
	int numSequences;

	int refCount;
};

struct MFAnimScript
{
	MFAnimScriptTemplate *pTemplate;

	MFModel *pModel;

	int currentSequence;
	float currentFrame;
};

MFAnimScriptTemplate *pMFAnimScriptInternal_Find(const char *pScript);

#endif
