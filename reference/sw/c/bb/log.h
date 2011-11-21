#ifndef LOG
#define LOG
#include <stdlib.h>
#include <stdio.h>

extern int LOG_LEVEL;
extern int STOP_ERROR;

#define log(...) if(LOG_LEVEL>=1){printf(__VA_ARGS__);}
#define err(...) { fprintf(stderr, "ERROR: "); fprintf(stderr,__VA_ARGS__); exit(-1); }
#define vlog(...) if(LOG_LEVEL>=2){printf(__VA_ARGS__);}

#endif 
