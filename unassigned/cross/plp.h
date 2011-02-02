#ifndef PLP
#define PLP

/* log levels */
#define LOG_ALL   9
#define LOG_DEBUG 8
#define LOG_JUNK  3
#define LOG_INST  2
#define LOG_INFO  1

void log_i(int level, char *str, int opt);
void log_s(int level, char *str, char *str2);

static int glevel = 0;
static char *inFile = NULL;
static char *outFile = NULL;
static FILE *iFile = NULL;
static FILE *oFile = NULL;
static char buf[1024];

void craft(int n_args, ...);

#endif
