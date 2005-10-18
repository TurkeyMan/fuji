#include "Fuji.h"
#include "F3D.h"

#include "xmlparse.h"

extern F3DFile *pModel;

void startElement(void *userData, const char *name, const char **atts)
{
  int i;
  int *depthPtr = (int*)userData;
  for (i = 0; i < *depthPtr; i++)
    putchar('\t');
  puts(name);
  *depthPtr += 1;
}

void endElement(void *userData, const char *name)
{
  int *depthPtr = (int*)userData;
  *depthPtr -= 1;
}

void charHandler(void *userData, const XML_Char *s, int len)
{
	int x=0;
}

void ParseDAEFile(char *pFile, uint32 length)
{
	XML_Parser parser = XML_ParserCreate(NULL);

	int x=0;
	XML_SetUserData(parser, &x);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, charHandler);
//	XML_SetProcessingInstructionHandler(parser, charHandler);
	XML_SetDefaultHandler(parser, charHandler);

	if(!XML_Parse(parser, pFile, length, 1))
	{
		printf("%s at line %d\n", XML_ErrorString(XML_GetErrorCode(parser)), XML_GetCurrentLineNumber(parser));
	}

	XML_ParserFree(parser);
}

int F3DFile::ReadDAE(char *pFilename)
{
	FILE *infile;
	char *file = NULL;
	uint32 filesize = 0;

	infile = fopen(pFilename, "r");

	if(!infile)
	{
		printf("Failed to open ASE file %s", pFilename);
		return 1;
	}

	fseek(infile, 0, SEEK_END);
	filesize = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	file = (char*)malloc(filesize+1);
	fread(file, 1, filesize, infile);
	file[filesize] = NULL;

	fclose(infile);

	ParseDAEFile(file, filesize);

	free(file);

	return 0;
}
