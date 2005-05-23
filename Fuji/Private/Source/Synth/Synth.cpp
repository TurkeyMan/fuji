#include "Common.h"
#include "FujiMath.h"
#include "Synth/Synth.h"

float GetWave(WaveType waveType, float phase)
{
	if(phase >= PI*2.0f)
		phase = fmodf(phase, PI*2.0f);

	switch(waveType)
	{
		case WT_Sine:
			return MFSin(phase);
		case WT_CoSine:
			return MFCos(phase);
		case WT_Triangle:
			return phase >= PI ? -((phase-PI)*((1.0f/PI) * 2.0f) - 1.0f) : phase*((1.0f/PI) * 2.0f) - 1.0f;
		case WT_Square:
			return phase >= PI ? 1.0f : -1.0f;
		case WT_UpSaw:
			return -1.0f + phase*(1.0f/PI);
		case WT_DownSaw:
			return 1.0f - phase*(1.0f/PI);
	}

	return 0.0f;
}

float GetNoteFrequency(float noteOffset, float baseFrequency)
{
	float freq = baseFrequency;

	if(noteOffset)
	{
		freq = baseFrequency * MFPow(2.0f, 1.0f/(12.0f/noteOffset));
	}

	return freq;
}

void Filter::GetSamples(float *pOutputBuffer, int numSamples)
{
	memset(pOutputBuffer, 0, numSamples*sizeof(float));
}

void Filter::Draw()
{
}

void Filter::SetBufferSize(int numSamples)
{
}
