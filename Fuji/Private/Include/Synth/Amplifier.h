#if !defined(_AMPLIFIER_H)
#define _AMPLIFIER_H

#include "Synth.h"

class Amplifier : public Filter
{
public:
	Amplifier(float _amplification) { amplification = _amplification; sampleCount = 0; pSource = NULL; pAmplificationParameter = NULL; }

	void SetSource(Filter *_pSource) { pSource = _pSource; }

	void SetAmplification(float _amplification) { amplification = _amplification; }
	void SetAmplification(Parameter *_pAmplificationParameter) { pAmplificationParameter = _pAmplificationParameter; }

	virtual void Reset();
	virtual void GetSamples(float *pOutputBuffer, int numSamples);
	virtual void SetBufferSize(int numSamples);

protected:
	Filter *pSource;

	float amplification;
	Parameter *pAmplificationParameter;

	uint32 sampleCount;
};

#endif
