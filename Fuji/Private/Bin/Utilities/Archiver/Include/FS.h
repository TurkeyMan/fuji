#if !defined(_FS_H)
#define _FS_H

#include <vector>
#include <string>
#include <map>

bool GetCurrentDir(char *dir, int maxlen);
int GetDirectoryEntries(const char *directory, std::vector<std::string> &entries);
void FreeDirectoryEntries(std::vector<std::string> &entries);
bool IsDirectory(const char *entry);

#endif
