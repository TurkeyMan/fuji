#include <stdio.h>
#include <fstream.h>

#include "Convert3DS.h"

struct Chunk
{
	uint16 id;
	uint16 nextOffset;
	char buffer[1];
}

void ParseChunk(Chunk *pChunk)
{
	switch(pChunk->id)
	{
		case 


	}
}

int main(int argc, char *argv[])
{
	ifstream file;
	file.open(argv[1], ios::nocreate | ios::binary | ios::in);

	file.seekg(0, ios::end);
	int filesize = file.tellg();
	file.seekg(0, ios::beg);

	char *pFile = new char[filesize];

	file.read(pFile, filesize);

	ParseChunk(pFile);

	return 0;
}

