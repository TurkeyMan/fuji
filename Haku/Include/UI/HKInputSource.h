#if !defined(_HKINPUTSOURCE_H)
#define _HKINPUTSOURCE_H

class HKInputSource
{
public:
	static void Init();
	static void Deinit();

	static void UpdateSources();

	static int GetNumSources();
	static HKInputSource *GetSource(int source);

protected:
	int device;
	int deviceID;
};

#endif
