#include "Fuji_Internal.h"
#include "MFMidi_Internal.h"
#include "MFSystem.h"

/**** Forward Declarations ****/


/**** Structures ****/


/**** Globals ****/


/**** Functions ****/

MFInitStatus MFMidi_InitModule(int moduleId, bool bPerformInitialisation)
{
	if(!gDefaults.midi.useMidi)
		return MFIS_Succeeded;

	MFMidi_InitModulePlatformSpecific();

	return MFIS_Succeeded;
}

void MFMidi_DeinitModule()
{
	if(!gDefaults.midi.useMidi)
		return;

	MFMidi_DeinitModulePlatformSpecific();
}

void MFMidi_Update()
{
	MFMidi_UpdateInternal();
}
