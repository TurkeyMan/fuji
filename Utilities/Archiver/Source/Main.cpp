#include "FS.h"

void Traverse(const char *dir, unsigned int indent);

int main(int argc, char **argv)
{
	char workingDir[256];
	
	if(argc < 2) {
		if(!GetCurrentDir(workingDir, 255)) {
			fprintf(stderr, "Unable to retrieve current working directory\r\n");
			return(1);
		}
	
		Traverse(workingDir, 0);
	} else {
		Traverse(argv[1], 0);
	}

	return(0);
}

void Traverse(const char *dir, unsigned int indent)
{
	std::vector<char *> entries;
	
	if(GetDirectoryEntries(dir, entries) > 0) {
		for(unsigned int i = 0; i < entries.size(); i++) {
			for(unsigned int j = 0; j < indent; j++) {
				printf("\t");
			}
			
			char *tmpDir = new char[strlen(dir) + strlen(entries[i]) + 2];
			strcpy(tmpDir, dir);
			strcat(tmpDir, "\\");
			strcat(tmpDir, entries[i]);
			if(IsDirectory(tmpDir)) {
				printf("%s/\r\n", entries[i]);
				
				Traverse(tmpDir, indent + 1);
				delete[] tmpDir;
			} else {
				printf("%s\r\n", entries[i]);
				delete[] tmpDir;
			}
		}
	
		FreeDirectoryEntries(entries);
	}
}