#include <stdio.h>
#include <fstream>

#pragma pack(1)

typedef struct tagTARGAFILEHEADER
{
unsigned char imageIDLength;
unsigned char colorMapType;
unsigned char imageTypeCode;
short int colorMapOrigin;
short int colorMapLength;
unsigned char colorMapEntrySize;
short int imageXOrigin;
short int imageYOrigin;
short int imageWidth;
short int imageHeight;
unsigned char bitCount;
unsigned char imageDescriptor;
} TARGAFILEHEADER;

#pragma pack()

int main(int argc, char *argv[])
{
	std::ifstream infile;
	std::ofstream outfile;

	char *pBuffer;
	TARGAFILEHEADER *pTGAHeader;
	unsigned int *pImage;

	if(argc<2)
	{
		printf("No source file specified...\n\nTargaToHeader v1.0 - by Manu Evans\n\nUsage:\n  TargaToHeader.exe <sourceFile>\n\n");
		return 1;
	}

	infile.open(argv[1], std::ios::binary|std::ios::in);

	if(!infile.is_open())
	{
		printf("Error opening source file...\n\n");
		return 2;
	}

	infile.seekg(0, std::ios_base::end);
	unsigned int size = infile.tellg();
	infile.seekg(0, std::ios_base::beg);

	if(size <= 0)
	{
		printf("File is empty or corrupt...\n\n");
		return 2;
	}

	pBuffer = (char*)malloc(size);

	infile.read(pBuffer, size);
	infile.close();

	pTGAHeader = (TARGAFILEHEADER*)pBuffer;
	pImage = (unsigned int*)(pBuffer + sizeof(TARGAFILEHEADER));

	char imageName[256];
	char outputFilename[256];

	size_t a;
	for(a = strlen(argv[1]); a>0 && argv[1][a] != '.'; a--) {}
	if(a > 0) argv[1][a] = 0;

	for(a = strlen(argv[1]); a>0 && argv[1][a-1] != '/' && argv[1][a-1] != '\\'; a--) {}

	// output filename
	if(argc > 2)
	{
		strcpy(outputFilename, argv[1]);
		sprintf(&outputFilename[a], "%s.h", argv[2]);

		strcpy(imageName, argv[2]);
	}
	else
	{
		sprintf(outputFilename, "%s.h", argv[1]);

		strcpy(imageName, &argv[1][a]);
	}

	outfile.open(outputFilename, std::ios::out);

	outfile << "// " << outputFilename << ".h\n// Generated using TargaToHeader by Manu Evans\n\n";
	outfile << "const int " << imageName << "_width = " << pTGAHeader->imageWidth << ";\n";
	outfile << "const int " << imageName << "_height = " << pTGAHeader->imageHeight << ";\n\n";
	outfile << "unsigned int " << imageName << "_data[" << pTGAHeader->imageWidth*pTGAHeader->imageHeight << "] =\n{\n\t";

	// write data

	int pixelCount = pTGAHeader->imageWidth*pTGAHeader->imageHeight;

	for(int i=0; i<pixelCount; )
	{
		sprintf(outputFilename, "0x%08X", pImage[i]);
		outfile << outputFilename;

		++i;

		if(i < pixelCount)
		{
			outfile << ", ";

			if(i%16 == 0)
			{
				outfile << "\n\t";
			}
		}
	}

	outfile << "\n};\n\n";

	outfile.close();

	return 0;
}
