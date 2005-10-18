#include "Fuji.h"
#include "FujiMath.h"
#include "Synth/PhaseInverter.h"

void PhaseInverter::SetSource(Filter *_pSource)
{
	pSource = _pSource;
}

void PhaseInverter::GetSamples(float *pOutputBuffer, int numSamples)
{
	pSource->GetSamples(pOutputBuffer, numSamples);

	for(int a=0; a<numSamples; a++)
	{
		pOutputBuffer[a] = -pOutputBuffer[a];
	}
}

void PhaseInverter::Reset()
{
	pSource->Reset();
}

void PhaseInverter::SetBufferSize(int numSamples)
{
	pSource->SetBufferSize(numSamples);
}

