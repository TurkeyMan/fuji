#include <stdio.h>
#include <fstream>

int main(int argc, char *argv[])
{
	std::ifstream infile;
	std::ofstream outfile;

	char *pBuffer;

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

	int dwordCount = size + ((size%4) ? 4-(size%4) : 0);

	pBuffer = (char*)malloc(dwordCount);
	memset(&pBuffer[size], 0, dwordCount-size);
	dwordCount /= 4;

	infile.read(pBuffer, size);
	infile.close();

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

	outfile << "// " << outputFilename << ".h\n// Generated using BinToHeader by Manu Evans\n\n";
	outfile << "const int " << imageName << "_bufferBytes = " << size << ";\n";
	outfile << "unsigned int " << imageName << "_data[" << dwordCount << "] =\n{\n\t";

	// write data
	for(int i=0; i<dwordCount; )
	{
		sprintf(outputFilename, "0x%08X", ((unsigned int*)pBuffer)[i]);
		outfile << outputFilename;

		++i;

		if(i < dwordCount)
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
