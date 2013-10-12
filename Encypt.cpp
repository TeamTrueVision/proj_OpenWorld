#include <string>

#include "Encypt.h"

#define SEED 0x0FEADEAC

char *DecyptString(const char *b)
{
	char a[1024];
	char *d = a;
	const char *c = b;

	while(c) {
		*d++ = *c++;
	}

	return a;
}