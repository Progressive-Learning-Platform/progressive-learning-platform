/* fritz - logging functionality */

#include <stdio.h>
#include <stdlib.h>
#include "log.h"

int LOG_LEVEL = 0;

void _log(int l, FILE *f, char *s) {
	if (LOG_LEVEL >= l)
		fprintf(f, "%s\n", s);
}
