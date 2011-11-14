#ifndef LINE
#define LINE
#include "log.h"

#define llog(x, ...) if(LOG_LEVEL>=1){printf("AT LINE %d: %s\t", x, get_line(x)); printf(__VA_ARGS__);}
#define lerr(x, ...) { fprintf(stderr, "AT LINE %d: %s\t", x, get_line(x)); fprintf(stderr, "ERROR: "); fprintf(stderr,__VA_ARGS__); if (STOP_ERROR) exit(-1); }
#define lvlog(x, ...) if(LOG_LEVEL>=2){printf("AT LINE %d: %s\t", x, get_line(x)); printf(__VA_ARGS__);}

void build_lines(char*);
char* get_line(int);
int is_visited(int);
void visit(int);

#endif
