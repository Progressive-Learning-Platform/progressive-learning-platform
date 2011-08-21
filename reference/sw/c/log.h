#ifndef LOG
#define LOG
#include <stdlib.h>
#include <stdio.h>

extern int LOG_LEVEL;

#define log(...) if(LOG_LEVEL>=1){printf(__VA_ARGS__);}
#define err(...) fprintf(stderr,__VA_ARGS__)
#define vlog(...) if(LOG_LEVEL>=2){printf(__VA_ARGS__);}

#endif 
