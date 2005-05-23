#if !defined(_PHASEINVERTER_H)
#define _PHASEINVERTER_H

#include "Synth.h"

class PhaseInverter : public Filter
{
public:
	PhaseInverter() { pSource = NULL; }
	void SetSource(Filter *pSource);

	virtual void Reset();
	virtual void GetSamples(float *pOutputBuffer, int numSamples);
	virtual void SetBufferSize(int numSamples);

protected:
	Filter *pSource;
};

#endif
