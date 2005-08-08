#include "Chomp.h"

#include <string.h>

bool Chomp(char *str, char toChomp)
{
	size_t len = strlen(str);

	if(str[len - 1] == toChomp) {
		str[len - 1] = '\0';
		return true;
	}

	return false;
}
