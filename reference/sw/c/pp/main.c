#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.tab.h"
#include "log.h"

/* for getopts */
#include <ctype.h>
#include <unistd.h>

/* lexer/parser externs */
extern void yyset_in (FILE * in_str);
extern int yyparse(void);

int LOG_LEVEL = 0;
int STOP_ERROR = 1;

static char *S_FILE_INPUT = NULL;
static char *S_FILE_OUTPUT = NULL;
static FILE *FILE_INPUT = NULL;
static FILE *FILE_OUTPUT = NULL;

char *program = NULL;

char* emit(char* p, char *s) {
        if (p == NULL) {
                p = strdup(s); /* first emit from the compiler will ALWAYS be a label */
                if (p == NULL) {
                        err("emit strdup failed\n");
                }
        } else {
                p = realloc(p, (strlen(p)+strlen(s)+1)*sizeof(char));
                if (p == NULL) {
                        err("emit realloc failed\n");
                }
                p = strcat(p, s);
        }
        return p;
}

void print_usage(void) {
	printf("plppp - plp preprocessor\n\n");
	printf("usage: plppp <options> <input file>\n");
	printf("options:\n");
	printf("-o <filename>	output filename\n");
	printf("-d [0,1,2]	debug level (0=off (default), 1=on, 2=verbose)\n");
	printf("-e		do not stop preprocessing on errors\n");
}

void handle_opts(int argc, char *argv[]) {
	char *dvalue = NULL;
	char *ivalue = NULL;
	char *ovalue = NULL;

	int c;

	opterr = 0;
	
	while ((c = getopt(argc, argv, "d:o:spefa")) != -1)
		switch (c) {
			case 'd':
				dvalue = optarg;
				break;
			case 'o':
				ovalue = optarg;
				break;
			case 'e':
				STOP_ERROR = 0;
				break;
			default:
				print_usage();
				exit(-1);
		}

	if (dvalue != NULL) {
		LOG_LEVEL = atoi(dvalue);
		vlog("[plppp] setting log level: %d\n", LOG_LEVEL);
	}

	/* grab remaining values as inputs */
	if (optind < argc) {
		while (optind < argc) {
			ivalue = argv[optind];
			log("[plppp] input file: %s\n", argv[optind]);
			optind++;
		}
	}

	if (ivalue != NULL) {
		S_FILE_INPUT = ivalue;
	} else {
		print_usage();
		exit(-1);
	}

	if (ovalue != NULL) {
		S_FILE_OUTPUT = ovalue;
	} else {
		S_FILE_OUTPUT = malloc(11);
		sprintf(S_FILE_OUTPUT,"plppp.out");
	}
	log("[plppp] output file: %s\n", S_FILE_OUTPUT);
}

int main(int argc, char *argv[]) {

	/* process arguments */
	handle_opts(argc, argv);	

	/* open files */
	vlog("[plppp] opening files\n");
	FILE_INPUT = fopen(S_FILE_INPUT,"r");
	if (FILE_INPUT == NULL) {
		err("[plppp] cannot open input file: %s\n", S_FILE_INPUT);
	}
	FILE_OUTPUT = fopen(S_FILE_OUTPUT,"w");
	if (FILE_OUTPUT == NULL) {
		err("[plppp] cannot open output file: %s\n", S_FILE_OUTPUT);
	}
	yyset_in(FILE_INPUT);

	log("[plppp] starting preprocessor\n");
	yyparse();

	fprintf(FILE_OUTPUT, "%s", program);

	vlog("[plppp] closing files\n");
	fclose(FILE_INPUT);
	fclose(FILE_OUTPUT);

	log("[plppp] done\n");
	return 0;
}
