#if !defined(_LFO_H)
#define _LFO_H

#include "Synth.h"

class Oscillator : public Parameter
{
public:
	Oscillator(float frequency = 0.0f, float amplitude = 1.0f, WaveType waveType = WT_Sine);

	virtual float GetValue();
	virtual void SetTime(uint32 _time);

	void SetFrequency(float _frequency) { frequency = _frequency; }
	void SetAmplitude(float _amplitude) { amplitude = _amplitude; }
	void SetBase(float _base) { base = _base; }
	void SetFrequency(Parameter *pFrequencyParam) { pFrequencyParamater = pFrequencyParam; }
	void SetAmplitude(Parameter *pAmplitudeParam) { pAmplitudeParamater = pAmplitudeParam; }

	void SetWaveType(WaveType _waveType) { waveType = _waveType; }

protected:
	float frequency;
	float amplitude;
	WaveType waveType;

	Parameter *pFrequencyParamater;
	Parameter *pAmplitudeParamater;

	uint32 lastTime;
	float phase;

	float base;
};

class ParameterSource : public Filter
{
public:
	ParameterSource(Parameter *_pParameter = NULL) { pParameter = _pParameter; sampleCount = 0; }

	void SetParameter(Parameter *_pParameter = NULL) { pParameter = _pParameter; sampleCount = 0; }

	virtual void Reset();
	virtual void GetSamples(float *pOutputBuffer, int numSamples);

protected:
	Parameter *pParameter;
	uint32 sampleCount;
};

#endif
