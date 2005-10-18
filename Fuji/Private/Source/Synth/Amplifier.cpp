#include "Fuji.h"
#include "FujiMath.h"
#include "Synth/Amplifier.h"


void Amplifier::Reset()
{
	sampleCount = 0;
	pSource->Reset();
	pAmplificationParameter->SetTime(0);
}

void Amplifier::GetSamples(float *pOutputBuffer, int numSamples)
{
	pSource->GetSamples(pOutputBuffer, numSamples);

	if(pAmplificationParameter || amplification != 1.0f)
	{
		for(int a=0; a<numSamples; a++)
		{
			float amp;

			if(pAmplificationParameter)
			{
				pAmplificationParameter->SetTime(sampleCount++);
				amp = pAmplificationParameter->GetValue();
			}
			else
				amp = amplification;

			pOutputBuffer[a] *= amp;
		}
	}
}

void Amplifier::SetBufferSize(int numSamples)
{
	pSource->SetBufferSize(numSamples);
}
