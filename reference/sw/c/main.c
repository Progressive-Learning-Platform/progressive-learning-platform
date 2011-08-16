#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
#include "log.h"

/* for getopts */
#include <ctype.h>
#include <unistd.h>

static char *S_FILE_INPUT = NULL;
static char *S_FILE_OUTPUT = NULL;
static FILE *FILE_INPUT = NULL;
static FILE *FILE_OUTPUT = NULL;

void handle_opts(int argc, char *argv[]) {
	char *dvalue = NULL;
	char *ivalue = NULL;
	char *ovalue = NULL;

	int c;

	opterr = 0;
	
	while ((c = getopt(argc, argv, "d:i:o:")) != -1)
		switch (c) {
			case 'd':
				dvalue = optarg;
				break;
			case 'i':
				ivalue = optarg;
				break;
			case 'o':
				ovalue = optarg;
				break;
			case '?':
				if (optopt == 'd' ||
				    optopt == 'i' ||
				    optopt == 'o' )
					printf("[e] option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					printf("[e] unknown option '-%c'.\n", optopt);
				else
					printf("[e] unknown option character '\\x%x'.\n", optopt);
					exit(1);
				default:
					exit(1);
		}

	if (dvalue != NULL) {
		LOG_LEVEL = atoi(dvalue);
		vlog("setting log level\n");
	}

	if (ivalue != NULL) {
		S_FILE_INPUT = ivalue;
		log("input file: ");
		log(S_FILE_INPUT);
		log("\n");
	} else {
		err("no input file specified, use -i <input file>\n");
		exit(1);
	}

	if (ovalue != NULL) {
		S_FILE_OUTPUT = ovalue;
		log("output file: ");
		log(S_FILE_OUTPUT);
		log("\n");
	} else {
		err("no output file specified, use -o <output file>\n");
		exit(1);
	}
}

int main(int argc, char *argv[]) {

	/* process arguments */
	handle_opts(argc, argv);	

	/* open files */
	vlog("opening files\n");
	FILE_INPUT = fopen(S_FILE_INPUT,"r");
	if (FILE_INPUT == NULL) {
		err("cannot open input file!\n");
		exit(1);
	}
	FILE_OUTPUT = fopen(S_FILE_OUTPUT,"w");
	if (FILE_OUTPUT == NULL) {
		err("cannot open output file!\n");
		exit(1);
	}
	yyset_in(FILE_INPUT);

	log("starting parser\n");
	yyparse();

	vlog("closing files\n");
	fclose(FILE_INPUT);
	fclose(FILE_OUTPUT);

	log("done\n");
	return 0;
}
