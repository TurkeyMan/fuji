#include "Fuji.h"
#include "Primitive.h"
#include "Font.h"
#include "Synth/OutputFilter.h"

extern IDirectSound8 *pDirectSound;

void OutputFilter::Create(int _bufferLength, int _updateLength)
{
	CALLSTACK;

	DBGASSERT(_bufferLength%_updateLength == 0, "bufferLength is not a multiple of updateLength.");

	bufferLength = _bufferLength;
	updateLength = _updateLength;
	numSamples = updateLength/sizeof(float);

	pLSource = NULL;
	pRSource = NULL;

	pSourceBuffer = (float*)Heap_Alloc(sizeof(float) * updateLength);

	// fill out DSBuffer creation data
	DSBUFFERDESC desc;
	WAVEFORMATEX wfx;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample/8);
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
	wfx.cbSize = 0;

	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN;
	desc.dwBufferBytes = bufferLength;
	desc.lpwfxFormat = &wfx;
	desc.dwReserved = 0; 
	desc.guid3DAlgorithm = DS3DALG_DEFAULT; 

	// create the DSBuffer
	pDirectSound->CreateSoundBuffer(&desc, &pAudioBuffer, NULL);

	void *pData1, *pData2;
	DWORD bytes1, bytes2;

	bufferFed = 0;

	// fill buffer
	pAudioBuffer->Lock(0, bufferLength, &pData1, &bytes1, &pData2, &bytes2, DSBLOCK_ENTIREBUFFER);
	memset(pData1, 0, bufferLength);
	pAudioBuffer->Unlock(pData1, bytes1, pData2, bytes2);

	playing = false;
}

void OutputFilter::SetSourceFilter(int channel, Filter *pFilter)
{
	if(channel == 0)
	{
		pLSource = pFilter;
		pFilter->SetBufferSize(numSamples);
	}
	else if(channel == 1)
	{
		pRSource = pFilter;
		pFilter->SetBufferSize(numSamples);
	}
}

void OutputFilter::Update()
{
	if(pLSource && playing)
	{
		DWORD playCursor, writeCursor;

		pAudioBuffer->GetCurrentPosition(&playCursor, &writeCursor);

		if(playCursor < bufferFed || bufferFed+updateLength < playCursor)
		{
			void *pData1, *pData2;
			DWORD bytes1, bytes2;

			struct sample
			{
				int16 left;
				int16 right;
			};

			// fill buffer
			pAudioBuffer->Lock(bufferFed, updateLength, &pData1, &bytes1, &pData2, &bytes2, DSBLOCK_ENTIREBUFFER);

			sample *pSamples = (sample*)pData1;

			while(playCursor < bufferFed || bufferFed+updateLength < playCursor)
			{
				if(pRSource)
				{
					pLSource->GetSamples(pSourceBuffer, numSamples);

					for(int a=0; a<numSamples; a++)
					{
						pSamples[a].left = (int16)(MFClamp(0.0f, pSourceBuffer[a], 1.0f) * 32767.0f);
					}

					pRSource->GetSamples(pSourceBuffer, numSamples);

					for(int a=0; a<numSamples; a++)
					{
						pSamples[a].right = (int16)(MFClamp(0.0f, pSourceBuffer[a], 1.0f) * 32767.0f);
					}
				}
				else
				{
					pLSource->GetSamples(pSourceBuffer, numSamples);

					for(int a=0; a<numSamples; a++)
					{
						pSamples[a].right = pSamples[a].left = (int16)(MFClamp(0.0f, pSourceBuffer[a], 1.0f) * 32767.0f);
					}
				}

				bufferFed = (bufferFed + updateLength) % bufferLength;
			}

			pAudioBuffer->Unlock(pData1, bytes1, pData2, bytes2);
		}
	}
}

void OutputFilter::Play()
{
	if(pAudioBuffer && playing == false)
	{
		pAudioBuffer->Play(0, 0, DSBPLAY_LOOPING);
		playing = true;
	}
}

void OutputFilter::Stop()
{
	if(pAudioBuffer && playing == true)
	{
		pAudioBuffer->Stop();
		playing = false;
	}
}

void OutputFilter::Reset()
{
	if(pLSource)
	{
		Stop();

		if(pLSource)
			pLSource->Reset();

		if(pRSource)
			pRSource->Reset();

		void *pData1, *pData2;
		DWORD bytes1, bytes2;

		struct sample
		{
			int16 left;
			int16 right;
		};

		bufferFed = 0;

		// fill buffer
		pAudioBuffer->Lock(0, bufferLength, &pData1, &bytes1, &pData2, &bytes2, DSBLOCK_ENTIREBUFFER);

		sample *pSamples = (sample*)pData1;

		int numBuffers = bufferLength/updateLength;

		for(int a=0; a<numBuffers; a++)
		{
			if(pRSource)
			{
				pLSource->GetSamples(pSourceBuffer, numSamples);

				for(int b=0; b<numSamples; b++)
				{
					pSamples[b].left = (int16)(MFClamp(0.0f, pSourceBuffer[b], 1.0f) * 32767.0f);
				}

				pRSource->GetSamples(pSourceBuffer, numSamples);

				for(int b=0; b<numSamples; b++)
				{
					pSamples[b].right = (int16)(MFClamp(0.0f, pSourceBuffer[b], 1.0f) * 32767.0f);
				}
			}
			else
			{
				pLSource->GetSamples(pSourceBuffer, numSamples);

				for(int b=0; b<numSamples; b++)
				{
					pSamples[b].right = pSamples[b].left = (int16)(MFClamp(0.0f, pSourceBuffer[b], 1.0f) * 32767.0f);
				}
			}

			pSamples += numSamples;
		}

		pAudioBuffer->Unlock(pData1, bytes1, pData2, bytes2);
	}
}

void OutputFilter::Draw()
{
	MFPrimitive(PT_TriStrip|PT_Untextured);

	float y = 100.0f;

	Font_DrawTextf(gpDebugFont, 30.0f, y, 20.0f, MFVector::one, "Buffer:");

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(28);
	MFSetColour(0xFFFFFFFF);
	MFSetPosition(98.0f, y-2.0f, 0.0f);
	MFSetPosition(502.0f, y-2.0f, 0.0f);
	MFSetPosition(98.0f, y+22.0f, 0.0f);
	MFSetPosition(502.0f, y+22.0f, 0.0f);

	MFSetPosition(502.0f, y+22.0f, 0.0f);
	MFSetPosition(100.0f, y, 0.0f);

	MFSetColour(0xFF404040);
	MFSetPosition(100.0f, y, 0.0f);
	MFSetPosition(500.0f, y, 0.0f);
	MFSetPosition(100.0f, y+20.0f, 0.0f);
	MFSetPosition(500.0f, y+20.0f, 0.0f);

	DWORD playCursor, writeCursor;
	pAudioBuffer->GetCurrentPosition(&playCursor, &writeCursor);

	float xPlayCursor = 100.0f + (500.0f-100.0f) * ((float)playCursor / (float)bufferLength);

	MFSetPosition(500.0f, y+20.0f, 0.0f);
	MFSetPosition(xPlayCursor-1.0f, y-1.0f, 0.0f);

	MFSetColour(0xFFFFFF00);
	MFSetPosition(xPlayCursor-1.0f, y-1.0f, 0.0f);
	MFSetPosition(xPlayCursor+1.0f, y-1.0f, 0.0f);
	MFSetPosition(xPlayCursor-1.0f, y+21.0f, 0.0f);
	MFSetPosition(xPlayCursor+1.0f, y+21.0f, 0.0f);

	float xWriteCursor = 100.0f + (500.0f-100.0f) * ((float)writeCursor / (float)bufferLength);

	MFSetPosition(xPlayCursor+1.0f, y+21.0f, 0.0f);
	MFSetPosition(xWriteCursor-1.0f, y-1.0f, 0.0f);

	MFSetColour(0xFF0000FF);
	MFSetPosition(xWriteCursor-1.0f, y-1.0f, 0.0f);
	MFSetPosition(xWriteCursor+1.0f, y-1.0f, 0.0f);
	MFSetPosition(xWriteCursor-1.0f, y+21.0f, 0.0f);
	MFSetPosition(xWriteCursor+1.0f, y+21.0f, 0.0f);

	float xBufferFilled = 100.0f + (500.0f-100.0f) * ((float)bufferFed/(float)bufferLength);

	MFSetPosition(xWriteCursor+1.0f, y+21.0f, 0.0f);
	MFSetPosition(xBufferFilled-1.0f, y-1.0f, 0.0f);

	MFSetColour(0xFFFF8000);
	MFSetPosition(xBufferFilled-1.0f, y-1.0f, 0.0f);
	MFSetPosition(xBufferFilled+1.0f, y-1.0f, 0.0f);
	MFSetPosition(xBufferFilled-1.0f, y+21.0f, 0.0f);
	MFSetPosition(xBufferFilled+1.0f, y+21.0f, 0.0f);
	MFEnd();

	y += 30.0f;


	float width = 40.0f;
	y += width + 10.0f;

	Font_DrawTextf(gpDebugFont, 30.0f, y-10.0f, 20.0f, MFVector::one, "Wave:");

	MFPrimitive(PT_TriStrip|PT_Untextured);

	MFBegin(10);
	MFSetColour(0xFFFFFFFF);
	MFSetPosition(98.0f, y-width-2.0f, 0.0f);
	MFSetPosition(502.0f, y-width-2.0f, 0.0f);
	MFSetPosition(98.0f, y+width+2.0f, 0.0f);
	MFSetPosition(502.0f, y+width+2.0f, 0.0f);

	MFSetPosition(502.0f, y+width+2.0f, 0.0f);
	MFSetPosition(100.0f, y-width, 0.0f);

	MFSetColour(0xFF404040);
	MFSetPosition(100.0f, y-width, 0.0f);
	MFSetPosition(500.0f, y-width, 0.0f);
	MFSetPosition(100.0f, y+width, 0.0f);
	MFSetPosition(500.0f, y+width, 0.0f);
	MFEnd();

	MFPrimitive(PT_LineStrip|PT_Untextured);
	MFBegin(401);
	MFSetColour(0xFFFFFFFF);
	for(int a=0; a<401; a++)
	{
		MFSetPosition(100.0f + (float)a, y+(-pSourceBuffer[a*4]*width), 0.0f);
	}
	MFEnd();

	y += width + 10.0f;
}
