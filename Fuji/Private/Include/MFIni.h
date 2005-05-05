// Mount Fuji - INI file reader
//
//================================================
// USAGE
//
// MFIni ini("global.model");
// MFIniIterator iterate(ini);
// while (iterate.Curr())
// {
//   if (iterate.Curr().IsString(0, "Group")
//   {
//     ProcessGroup(iterate);
//   }
//   iterate.Next();
// }
//================================================

class MFIniIterator
{
public:
	// create an iterator - starting from first line in INI file
	MFIniIterator(const MFIni &ini);
	
	// create an iterator - will continue from current line in another iterator
	MFiniIterator(const MFIniIterator &iterator);

	// init from an existing iterator
	void Init(const MFIniIterator &iterator);

	// line accessors
	const MFIniLine &Curr()			{ return pLine; };
	const MFIniLine &Next();
	const MFIniLine &Prev();
	const MFIniLine &Sub();

private:
	MFIni *pIni;
	MFIniLine *pLine;
};

class MFIniLine
{
public:
	int GetStringCount();
	bool IsString(int index, const char *pString);
	const char *GetString(int index);
	float GetFloat(int index);
	int GetInt(int index);
	MFVector3 GetVector(int index);

	int GetSubLineCount();
	MFIniLine *GetSubLine();

private:
	int dataCount;							// number of data strings for this line
	const char **pDataStrings;	// pointer to array of data strings
	int subLineCount;						// how many sublines are we pointing to?
	MFIniLine *pSubLines;				// pointer to start of sublines (or NULL if no sub section)
};

class MFIni
{
public:
	MFIni(const char *pFilename);
	~MFini();

protected:
	char name[128];
	int nmbrOfLines;
	MFIniLine *pLines;
	const char **ppStrings;
	MFStringCache *pCache;
};
