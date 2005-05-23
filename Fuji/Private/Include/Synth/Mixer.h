#if !defined(_MIXER_H)
#define _MIXER_H

#include "Synth.h"

class Mixer : public Filter
{
public:
	Mixer();

	void SetSourceFilters(Filter *pSource1, Filter *pSource2);
	void SetLevel(int channel, float level);

	virtual void Reset();

	virtual void GetSamples(float *pOutputBuffer, int numSamples);
	virtual void SetBufferSize(int numSamples);

protected:
	float *pBuffer;

	Filter *pSource1;
	Filter *pSource2;

	float level[2];
};

#endif
