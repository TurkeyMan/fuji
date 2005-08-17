#include "Common.h"

#include "IntImage.h"

void DestroyImage(SourceImage *pImage)
{
	for(int a=0; a<pImage->mipLevels; a++)
	{
		if(pImage->pLevels[a].pData)
			free(pImage->pLevels[a].pData);
	}

	if(pImage->pLevels)
		free(pImage->pLevels);

	if(pImage)
		free(pImage);
}

void FilterMipMaps(int numMipLevels, uint32 mipFilterOptions)
{

}

void ScanImage(SourceImage *pImage)
{
	pImage->opaque = true;
	pImage->oneBitAlpha = true;

	for(int a=0; a<pImage->mipLevels; a++)
	{
		for(int b=0; b<pImage->pLevels[a].width; b++)
		{
			for(int c=0; c<pImage->pLevels[a].height; c++)
			{
				float alpha = pImage->pLevels[a].pData[c*pImage->pLevels[a].width + b].a;

				if(alpha != 1.0f)
				{
					pImage->opaque = false;

					if(alpha != 0.0f)
						pImage->oneBitAlpha = false;
				}
			}
		}
	}
}

void FlipImage(SourceImage *pImage)
{
	for(int a=0; a<pImage->mipLevels; a++)
	{
		int halfHeight = pImage->pLevels[a].height / 2;
		int stride = pImage->pLevels[a].width * sizeof(Pixel);
		int height = pImage->pLevels[a].height;

		char *pBuffer = (char*)malloc(stride);
		char *pData = (char*)pImage->pLevels[a].pData;

		for(int b=0; b<halfHeight; b++)
		{
			// swap lines
			memcpy(pBuffer, &pData[b*stride], stride);
			memcpy(&pData[b*stride], &pData[(height-b-1)*stride], stride);
			memcpy(&pData[(height-b-1)*stride], pBuffer, stride);
		}
	}
}
