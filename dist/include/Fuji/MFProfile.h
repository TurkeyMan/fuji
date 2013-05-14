/**
 * @file MFProfile.h
 * @brief Fuji profiling.
 * @author Manu Evans
 * @defgroup MFProfile Profiling
 * @{
 */

#pragma once
#if !defined(_MFPROFILE_H)
#define _MFPROFILE_H

enum MFProfileCounterFlags
{
	MFPCF_ResetEachFrame = MFBIT(0),
	MFPCF_GroupTotal = MFBIT(1)
};

MFProfileGroup* MFProfile_CreateProfileGroup(const char *pName);
MFProfileCounter* MFProfile_CreateProfileCounter(MFProfileGroup *pGroup, const char *pName, uint32 flags);

#endif // _MFPROFILE_H

/** @} */
