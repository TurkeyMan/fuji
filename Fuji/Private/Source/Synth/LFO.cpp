#include "Common.h"
#include "FujiMath.h"
#include "Synth/LFO.h"

Oscillator::Oscillator(float _frequency, float _amplitude, WaveType _waveType)
{
	waveType = _waveType;
	frequency = _frequency;
	amplitude = _amplitude;

	lastTime = 0.0f;
	phase = 0.0f;

	base = 0.0f;
}

float Oscillator::GetValue()
{
	if(time != lastTime)
	{
		if(pAmplitudeParamater) pAmplitudeParamater->SetTime(time);
		if(pFrequencyParamater) pFrequencyParamater->SetTime(time);

		float period = (float)(time-lastTime) * SYNTH_SAMPLEPERIOD;

		phase += period*(pFrequencyParamater ? pFrequencyParamater->GetValue() : frequency)*PI*2.0f;

		lastTime = time;

		if(phase > PI*2.0f)
			phase -= PI*2.0f;
	}

	float a = pAmplitudeParamater ? pAmplitudeParamater->GetValue() : amplitude;
	return base + GetWave(waveType, phase)*a;
}

void Oscillator::SetTime(uint32 _time)
{
	Parameter::SetTime(_time);

	if(pFrequencyParamater)
		pFrequencyParamater->SetTime(_time);

	if(pAmplitudeParamater)
		pAmplitudeParamater->SetTime(_time);
}

void ParameterSource::GetSamples(float *pOutputBuffer, int numSamples)
{
	float *pBuffer = pOutputBuffer;

	for(int a=0; a<numSamples; a++)
	{
		pParameter->SetTime(sampleCount++);

		*pBuffer = pParameter->GetValue();
		++pBuffer;
	}
}

void ParameterSource::Reset()
{
	sampleCount = 0;
	pParameter->SetTime(0);
}
