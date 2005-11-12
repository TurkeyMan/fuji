#if !defined(_OUTPUTFILTER_H)
#define _OUTPUTFILTER_H

#include "Synth.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Mmreg.h>
#include <dsound.h>

class OutputFilter : public Filter
{
public:
	void Create(int bufferLength, int updateLength);

	void Update();
	void Draw();

	void Play();
	void Stop();
	virtual void Reset();

	void SetSourceFilter(int channel, Filter *pFilter);

protected:
	int bufferLength;
	int updateLength;
	int numSamples;

	float *pSourceBuffer;

	Filter *pLSource;
	Filter *pRSource;

	IDirectSoundBuffer *pAudioBuffer;
	uint32 bufferFed;

	bool playing;
};

#endif
