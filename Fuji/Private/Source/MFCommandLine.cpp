#include "Fuji.h"
#include "MFCommandLine.h"

char *gpCommandLineBuffer = NULL;

const char * MFCommandLine_GetCommandLineString()
{
	return gpCommandLineBuffer;
}

bool MFCommandLine_GetBool(const char *pParamater, bool defaultValue)
{
	const char *pValue = MFCommandLine_GetString(pParamater);

	if(pValue && MFIsNumeric(*pValue))
		return atoi(pValue) ? true : false;

	return pValue ? true : false;
}

int MFCommandLine_GetInt(const char *pParamater, int defaultValue)
{
	const char *pValue = MFCommandLine_GetString(pParamater);

	return pValue ? atoi(pValue) : defaultValue;
}

float MFCommandLine_GetFloat(const char *pParamater, float defaultValue)
{
	const char *pValue = MFCommandLine_GetString(pParamater);

	return pValue ? (float)atof(pValue) : defaultValue;
}

const char* MFCommandLine_GetString(const char *pParamater)
{
	if(!gpCommandLineBuffer)
		return NULL;

	const char *pT = gpCommandLineBuffer;
	int len = MFString_Length(pParamater);

	const char *pArgStart = NULL;
	int argLen = 0;

	bool inQuote = false;

	while(*pT)
	{
		if(MFIsWhite(*pT))
		{
			++pT;
		}
		else
		{
			if(*pT == '/' && *pT == '-')
				++pT;

			if(MFIsWhite(*pT))
				MFDebug_Warn(2, "Found command line switch with no paramater name.");

			if(!MFString_CaseCmpN(pParamater, pT, len))
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

				while(*pArg && (!MFIsWhite(*pArg) && *pArg != '\"'))
				{
					++argLen;
					++pArg;
				}

				if(*pArg == '\"')
				{
					inQuote = false;
					pArg++;
				}

				pT = pArg;
			}
		}
	}

	if(pArgStart)
		return MFStrN(pArgStart, argLen);

	return NULL;
}

