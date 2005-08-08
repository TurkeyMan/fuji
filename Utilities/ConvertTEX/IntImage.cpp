#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
