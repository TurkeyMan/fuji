#if !defined(_FS_H)
#define _FS_H

#include <vector>

bool GetCurrentDir(char *dir, int maxlen);
int GetDirectoryEntries(const char *directory, std::vector<char *> &entries);
void FreeDirectoryEntries(std::vector<char *> &entries);
bool IsDirectory(const char *entry);

#endif