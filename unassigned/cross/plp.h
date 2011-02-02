#ifndef PLP
#define PLP

/* log levels */
#define LOG_ALL   9
#define LOG_DEBUG 8
#define LOG_JUNK  3
#define LOG_INST  2
#define LOG_INFO  1

static int glevel = 0;
static char *inFile = NULL;
static char *outFile = NULL;
static FILE *iFile = NULL;
static FILE *oFile = NULL;

/* handlers */
void plp_handle_rtype(char *a, char *b, char *c, char *d);
void plp_handle_r2li(char *a, char *b, char *c);
void plp_handle_itype(char *a, char *b, char *c, char *d);
void plp_handle_lwsw(char *a, char *b, char *c);
void plp_handle_branch(char *a, char *b, char *c, char *d);
void plp_handle_directive(char *a);
void plp_handle_label(char *a);
void plp_hanlde_jump(char *a, char *b);



#endif
