#ifndef LOG
#define LOG
#include <stdlib.h>
#include <stdio.h>

extern LOG_LEVEL;

#define log(x) _log(1,stdout,x)
#define err(x) _log(0,stderr,x)
#define vlog(x) _log(2,stdout,x)

void _log(int l, FILE *f, char* s);

#endif 
