#pragma once
#if !defined(_MFTRANSLATION_INTERNAL_H)
#define _MFTRANSLATION_INTERNAL_H

#include "MFTranslation.h"

struct MFStringTable
{
	uint32 magic;
	int numStrings;
	const char *pStrings[1];
};

#endif
