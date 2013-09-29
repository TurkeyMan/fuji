#include "Fuji.h"
#include "MFCommandLine.h"

extern char *gpCommandLineBuffer;

MF_API const char * MFCommandLine_GetCommandLineString()
{
	return gpCommandLineBuffer;
}

MF_API bool MFCommandLine_GetBool(const char *pParameter, bool defaultValue)
{
	const char *pValue = MFCommandLine_GetString(pParameter);

	if(pValue && MFIsNumeric(*pValue))
		return atoi(pValue) ? true : false;

	return pValue ? true : false;
}

MF_API int MFCommandLine_GetInt(const char *pParameter, int defaultValue)
{
	const char *pValue = MFCommandLine_GetString(pParameter);

	return pValue ? atoi(pValue) : defaultValue;
}

MF_API float MFCommandLine_GetFloat(const char *pParameter, float defaultValue)
{
	const char *pValue = MFCommandLine_GetString(pParameter);

	return pValue ? (float)atof(pValue) : defaultValue;
}

MF_API const char* MFCommandLine_GetString(const char *pParameter)
{
	if(!gpCommandLineBuffer)
		return NULL;

	const char *pT = gpCommandLineBuffer;
	size_t len = MFString_Length(pParameter);

	const char *pArgStart = NULL;
	int argLen = 0;

	bool inQuote = false;

	while(*pT)
	{
		if(!MFIsWhite(*pT))
		{
			if(*pT == '/' && *pT == '-')
				++pT;

			if(MFIsWhite(*pT))
				MFDebug_Warn(2, "Found command line switch with no parameter name.");

			if(!MFString_CaseCmpN(pParameter, pT, len))
			{
				// find arg
				const char *pArg = pT+len;
				while(*pArg && (MFIsWhite(*pArg) || *pArg == '='))
					pArg++;

				if(*pArg == '\"')
				{
					inQuote = true;
					pArg++;
				}

				pArgStart = pArg;
				argLen = 0;

				while(*pArg && (!inQuote && !MFIsWhite(*pArg)) && *pArg != '\"')
				{
					++argLen;
					++pArg;
				}

				if(*pArg == '\"')
				{
					inQuote = false;
					pArg++;
				}

				return MFStrN(pArgStart, pArg - pArgStart);
			}
		}

		++pT;
	}

	return NULL;
}

