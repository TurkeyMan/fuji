#define VERSION 100

#include "Fuji.h"
#include "MFIni.h"
#include "MFHeap.h"
#include "MFSystem.h"
#include "MFString.h"
#include "FS.h"

#if defined(_WIN32)
#include <direct.h>
#else
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "pcre.h"
#include "minizip/zip.h"

void Traverse(const char *dir);
int ProcessIniFile(const char *pIniFile, MFPlatform platform);
void Replace(std::string &string, std::string subString, std::string newString);
void CopyFile(const char *pSource, const char *pDest);
void AddToZip(zipFile zip, const char *pSourceFile, const char *pSourceFileName);

std::vector<std::string> excludePatterns;
std::vector<pcre*> reHandles;
std::vector<std::string> sources;

std::map<std::string, std::string> userVariables;
std::map<std::string, std::string> extensionHandlers;

std::vector<std::string> filesToProcess;

std::string output;

int main(int argc, char **argv)
{
	MFPlatform platform = FP_Unknown;
	char iniFileName[256] = "";
	char workingDir[256] = "";
	char outPath[256] = "";
	zipFile zip = 0;
	int a;

	bool outputRawFiles = false;

	// process command line
	for(a=1; a<argc; a++)
	{
		if(argv[a][0] == '-' || argv[a][0] == '/')
		{
			for(int b=0; b<FP_Max; b++)
			{
				if(!MFString_CaseCmp(&argv[a][1], MFSystem_GetPlatformString(b)))
				{
					platform = (MFPlatform)b;
					break;
				}
			}

			if(!MFString_CaseCmpN(&argv[a][1], "ini", 3))
			{
				const char *pIniString = &argv[a][4];

				while(pIniString[0] == ' ' || pIniString[0] == '\t' || pIniString[0] == '=')
				 ++pIniString;

				MFString_Copy(iniFileName, pIniString);
			}
			else if(!MFString_CaseCmp(&argv[a][1], "?") || !MFString_CaseCmp(&argv[a][1], "h") || !MFString_CaseCmp(&argv[a][1], "-help"))
			{
				// show help

				return 0;
			}
			else if(!MFString_CaseCmp(&argv[a][1], "v") || !MFString_CaseCmp(&argv[a][1], "version"))
			{
				printf("%.2f", (float)VERSION/100.0f);
				return VERSION;
			}
		}
		else
		{
			sources.push_back(argv[a]);
		}
	}

	if(platform == FP_Unknown)
	{
		printf("No platform specified...\n");
		return 1;
	}

	if(!GetCurrentDir(workingDir, 255))
	{
		fprintf(stderr, "Unable to retrieve current working directory\r\n");
	}

	MFString_Cat(workingDir, "/");

	// read ini file
	if(iniFileName[0])
	{
		ProcessIniFile(iniFileName, platform);

		Replace(output, "%platform%", MFSystem_GetPlatformString(platform));
		Replace(output, "$(platform)", MFSystem_GetPlatformString(platform));
	}

	if(!sources.size())
	{
		sources.push_back(workingDir);
	}

	if(output[output.size()-1] == '/' || output[output.size()-1] == '\\')
	{
		outputRawFiles = true;
		output.resize(output.size()-1);
		MFString_Copy(outPath, output.c_str());
	}
	else if(!MFString_CaseCmp(&(output.c_str()[output.size()-4]), ".zip"))
	{
		// output is zip file
/*
		std::string::size_type lastSlash = output.rfind('/');
		if(lastSlash == std::string::npos)
			lastSlash = output.rfind('\\');

		if(lastSlash != std::string::npos)
		{
			MFString_Copy(outPath, output.c_str());
			outPath[lastSlash] = 0;
		}
*/
		MFString_Copy(outPath, output.c_str());
		outPath[output.size()-4] = 0;

		zip = zipOpen(output.c_str(), 0);

		if(!zip)
		{
			printf("Failed to open zip file '%s'", output.c_str());
			return 1;
		}
	}
	else
	{
		printf("Invalid output filename '%s'", output.c_str());
		return 1;
	}

	// create output folder if it doesn't exist
	char testPath[256] = "";

	char *pPathPart = strtok(outPath, "/\\");

	while(pPathPart)
	{
		MFString_Cat(testPath, pPathPart);
		MFString_Cat(testPath, "/");

		if(!IsDirectory(testPath))
		{
#if defined(WIN32)
			mkdir(testPath);
#else
			mkdir(testPath, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
		}

		pPathPart = strtok(NULL, "/\\");
	}

	// build regular expression handles
	for(a=0; a<(int)excludePatterns.size(); a++)
	{
		const char *errorString;
		int errorOffset;

		pcre *pRE = pcre_compile(excludePatterns[a].c_str(), PCRE_CASELESS|PCRE_DOLLAR_ENDONLY, &errorString, &errorOffset, NULL);

		if(!pRE)
			printf(MFStr("%d: %s", errorOffset, errorString));
		else
			reHandles.push_back(pRE);
	}

	// do stuff
	for(a=0; a<(int)sources.size(); a++)
	{
		Traverse(sources[a].c_str());
	}

	// process all files
	for(a=0; a<(int)filesToProcess.size(); a++)
	{
		std::string commandLine = "";
		std::string fileName;

		const char *pExt = "";
		char filePart[256];
		char filePath[256];
		int x, y;

		bool customTool = false;

		std::string fullName = filesToProcess[a];

		// get all the file name related variables
		x = (int)fullName.rfind('/');
		y = (int)fullName.rfind('\\');

		MFString_Copy(filePath, fullName.c_str());

		if(x >= 0 || y >= 0)
		{
			fileName = &fullName.c_str()[MFMax(x,y)+1];
			filePath[MFMax(x,y)] = 0;
		}
		else
			fileName = fullName.c_str();

		MFString_Copy(filePart, fileName.c_str());

		x = (int)fileName.rfind('.');

		if(x >= 0)
		{
			pExt = &fileName.c_str()[x+1];
			filePart[x] = 0;
		}

		// search file extensions list
		std::map<std::string, std::string>::iterator i = extensionHandlers.find(MFString_ToLower(pExt));

		if(i != extensionHandlers.end())
		{
			commandLine = i->second;
			customTool = true;
		}

		printf("%s\n", fileName.c_str());

		// run custom tool
		if(customTool)
		{
			// fill command line environment variables
			Replace(commandLine, "%outpath%", outPath);
			Replace(commandLine, "$(outpath)", outPath);
			Replace(commandLine, "%fullname%", fullName);
			Replace(commandLine, "$(fullname)", fullName);
			Replace(commandLine, "%filepath%", filePath);
			Replace(commandLine, "$(filepath)", filePath);
			Replace(commandLine, "%filename%", fileName);
			Replace(commandLine, "$(filename)", fileName);
			Replace(commandLine, "%filepart%", filePart);
			Replace(commandLine, "$(filepart)", filePart);
			Replace(commandLine, "%ext%", pExt);
			Replace(commandLine, "$(ext)", pExt);
			Replace(commandLine, "%platform%", MFSystem_GetPlatformString(platform));
			Replace(commandLine, "$(platform)", MFSystem_GetPlatformString(platform));

			// execute tool
			system(commandLine.c_str());
		}
		else
		{
			// copy file to archive as is
			if(outputRawFiles)
			{
				CopyFile(fullName.c_str(), MFStr("%s/%s", outPath, fileName.c_str()));
			}
			else
			{
				AddToZip(zip, fullName.c_str(), fileName.c_str());
			}
		}
	}

	if(!outputRawFiles)
	{
		// add processed files to zip..
		FreeDirectoryEntries(filesToProcess);
		GetDirectoryEntries(MFStr("%s/", outPath), filesToProcess);

		for(a=0; a<(int)filesToProcess.size(); a++)
		{
			std::string fullName = std::string(outPath) + "/" + filesToProcess[a];
			std::string fileName = filesToProcess[a];

			AddToZip(zip, fullName.c_str(), fileName.c_str());
		}

		zipClose(zip, NULL);

		// remove processed files..
		for(a=0; a<(int)filesToProcess.size(); a++)
		{
			remove(MFStr("%s/%s", outPath, filesToProcess[a].c_str()));
		}

		// remove intermediate folder
		rmdir(outPath);
	}

	return 0;
}

void Replace(std::string &string, std::string subString, std::string newString)
{
	std::string::size_type x;

	while((x = string.find(subString)) != std::string::npos)
	{
		string.replace(x, subString.size(), newString);
	}
}

int ProcessIniFile(const char *pIniFile, MFPlatform platform)
{
	MFIni *pIni = MFIni::Create(pIniFile);

	if(!pIni)
	{
		printf("Couldnt load ini file '%s'.\n", pIniFile);
		return 1;
	}

	MFIniLine *pLine = pIni->GetFirstLine();

	while(pLine)
	{
		if(pLine->IsString(0, "section"))
		{
			if(pLine->IsString(1, "version"))
			{
				// check program versions
			}
			else if(pLine->IsString(1, "sources"))
			{
				MFIniLine *pSub = pLine->Sub();

				while(pSub)
				{
					std::string source = pSub->GetString(0);

					if(source[source.size()-1] != '/' && source[source.size()-1] != '\\')
						source.append("/");

					sources.push_back(source);

					pSub = pSub->Next();
				}
			}
			else if(pLine->IsString(1, "output"))
			{
				MFIniLine *pSub = pLine->Sub();

				while(pSub)
				{
					output = pSub->GetString(0);
					pSub = pSub->Next();
				}
			}
			else if(pLine->IsString(1, "extensions"))
			{
				MFIniLine *pSub = pLine->Sub();

				while(pSub)
				{
					const char *pString = MFString_ToLower(pSub->GetString(0));
					extensionHandlers[pString] = pSub->GetString(1);

					pSub = pSub->Next();
				}
			}
			else if(pLine->IsString(1, "exclude"))
			{
				MFIniLine *pSub = pLine->Sub();
				uint32 includePlatform = 0xFFFFFFFF;
				uint32 platformFlag = 1 << (int)platform;
				bool bitwiseNot = false;
				int operation = 0;

				while(pSub)
				{
					const char *pString = pSub->GetString(0);

					if(*pString == ':')
					{
						++pString;

						while(*pString && *pString != ':')
						{
							// check for bitwise 'not' character
							bitwiseNot = *pString == '~';
							if(bitwiseNot)
								pString++;

							// find platform
							uint32 platformFlag = 0;
							int platformID = 0;

							for(platformID = 0; platformID < FP_Max; platformID++)
							{
								int strLen = MFString_Length(MFSystem_GetPlatformString(platformID));
								if(!MFString_CaseCmpN(pString, MFSystem_GetPlatformString(platformID), strLen))
								{
									platformFlag = 1 << platformID;
									pString += strLen;
									break;
								}
							}

							if(!platformFlag)
							{
								if(!MFString_CaseCmpN(pString, "all", 3))
								{
									platformFlag = 0xFFFFFFFF;
									pString += 3;
								}
								else
								{
									const char *pUnknownPlatform = pString;

									while(MFIsAlphaNumeric(*pString))
										++pString;

									pUnknownPlatform = MFStrN(pUnknownPlatform, (uint32&)pString - (uint32&)pUnknownPlatform);

									printf("Unknown platform '%s' in ini file.\n", pUnknownPlatform);
								}
							}

							// apply bitwise modifiers
							if(bitwiseNot)
								platformFlag = ~platformFlag;

							if(operation == 0)
								includePlatform = platformFlag;
							else if(operation == 1)
								includePlatform |= platformFlag;
							else if(operation == 2)
								includePlatform &= platformFlag;

							// check for bitwise operator for next platform
							if(*pString == '|')
								operation = 1;
							else if(*pString == '&')
								operation = 2;
							else
								operation = 0;

							if(operation)
								++pString;
						}
					}
					else
					{
						if(includePlatform & platformFlag)
						{
							excludePatterns.push_back(pString);
						}
					}

					pSub = pSub->Next();
				}
			}
		}

		pLine = pLine->Next();
	}

	MFIni::Destroy(pIni);

	return 0;
}

void Traverse(const char *dir)
{
	std::vector<std::string> entries;
	char testEntry[256];

	if(GetDirectoryEntries(dir, entries) > 0)
	{
		for(unsigned int i = 0; i < entries.size(); i++)
		{
			MFString_CopyCat(testEntry, dir, entries[i].c_str());

			bool isDir = IsDirectory(testEntry);
			bool excluded = false;

			if(isDir)
				MFString_Cat(testEntry, "/");

			// test exclude patterns
			for(int j=0; j<(int)reHandles.size(); j++)
			{
				int x = pcre_exec(reHandles[j], NULL, testEntry, MFString_Length(testEntry), 0, 0, NULL, 0);

				if(x > -1)
				{
					excluded = true;
					break;
				}
			}

			if(!excluded)
			{
				if(isDir)
				{
					// if passes regular expressions, scan folder
					Traverse(testEntry);
				}
				else
				{
					// if passes regular expressions, add to list
					filesToProcess.push_back(testEntry);
				}
			}
		}

		FreeDirectoryEntries(entries);
	}
}

void CopyFile(const char *pSource, const char *pDest)
{
	// copy the file
	FILE *pRead = fopen(pSource, "rb");

	if(pRead)
	{
		FILE *pWrite = fopen(pDest, "wb");

		if(pWrite)
		{
			fseek(pRead, 0, SEEK_END);
			uint32 fileSize = ftell(pRead);
			fseek(pRead, 0, SEEK_SET);

			char *pBuffer = (char*)MFHeap_Alloc(fileSize);

			fread(pBuffer, 1, fileSize, pRead);
			fwrite(pBuffer, 1, fileSize, pWrite);

			MFHeap_Free(pBuffer);

			fclose(pWrite);
		}
		else
		{
			printf("  Couldnt open output file...\n");
		}

		fclose(pRead);
	}
	else
	{
		printf("  Couldnt open file...\n");
	}
}

void AddToZip(zipFile zip, const char *pSourceFile, const char *pSourceFileName)
{
	char *pBuffer;

	FILE *pFile = fopen(pSourceFile, "rb");

	if(!pFile)
	{
		printf("Error writing file '%s' to zip, Unable to open file..", pSourceFile);
		return;
	}

	fseek(pFile, 0, SEEK_END);
	int fileLen = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	pBuffer = (char*)MFHeap_Alloc(fileLen);
	fread(pBuffer, 1, fileLen, pFile);
	fclose(pFile);

	int z = zipOpenNewFileInZip(zip, pSourceFileName, NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
	zipWriteInFileInZip(zip, pBuffer, fileLen);
	zipCloseFileInZip(zip);

	MFHeap_Free(pBuffer);
}
