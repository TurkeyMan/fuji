#include "Common.h"
#include "FujiMath.h"
#include "Synth/Mixer.h"

Mixer::Mixer()
{
	pBuffer = NULL;

	pSource1 = NULL;
	pSource2 = NULL;

	level[0] = 1.0f;
	level[1] = 1.0f;
}

void Mixer::SetSourceFilters(Filter *_pSource1, Filter *_pSource2)
{
	pSource1 = _pSource1;
	pSource2 = _pSource2;
}

void Mixer::Reset()
{
	if(pSource1)
		pSource1->Reset();
	if(pSource2)
		pSource2->Reset();
}

void Mixer::GetSamples(float *pOutputBuffer, int numSamples)
{
	pSource1->GetSamples(pOutputBuffer, numSamples);
	pSource2->GetSamples(pBuffer, numSamples);

	for(int i=0; i<numSamples; i++)
	{
		float a = pOutputBuffer[i] * level[0];
		float b = pBuffer[i] * level[1];
		float x;

//		if(a<0.0f && b < 0.0f)
//			x = a+b+a*b;
//		else if(a>0.0f && b > 0.0f)
//			x = a+b-a*b;
//		else
			x = a+b;

		pOutputBuffer[i] = x;
	}
}

void Mixer::SetBufferSize(int numSamples)
{
	if(pBuffer)
		Heap_Free(pBuffer);

	pBuffer = (float*)Heap_Alloc(sizeof(float)*numSamples);

	pSource1->SetBufferSize(numSamples);
	pSource2->SetBufferSize(numSamples);
}

void Mixer::SetLevel(int channel, float _level)
{
	level[channel] = _level;
}
