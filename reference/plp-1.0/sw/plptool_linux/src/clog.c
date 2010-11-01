#include <stdio.h>
#include <stdlib.h>
#include "clog.h"

void log_info(const char* s) {
	printf("[i] %s\n",s);
}

void log_error(const char* s) {
	printf("[e] %s\n",s);
	exit(-1);
}

void log_warn(const char* s) {
	printf("[w] %s\n",s);
}

