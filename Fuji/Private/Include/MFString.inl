
// some useful string parsing functions
inline bool MFIsWhite(char c)
{
	return c==' ' || c=='\t';
}

inline bool MFIsAlpha(char c)
{
	return (c>='a' && c<='z') || (c>='A' && c<='Z');
}

inline bool MFIsNumeric(char c)
{
	return c>='0' && c<='9';
}

inline bool MFIsAlphaNumeric(char c)
{
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || (c=='_');
}

inline bool MFIsNewline(char c)
{
	return c=='\n' || c=='\r';
}

inline char* MFSeekNewline(char *pC)
{
	while(!MFIsNewline(*pC) && *pC != 0) pC++;
	while(MFIsNewline(*pC)) pC++;
	return pC;
}

inline char* MFSkipWhite(char *pC)
{
	while(MFIsWhite(*pC)) pC++;
	return pC;
}
