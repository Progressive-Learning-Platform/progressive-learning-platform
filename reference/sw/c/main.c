#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
#include "log.h"

/* for getopts */
#include <ctype.h>
#include <unistd.h>

void handle_opts(int argc, char *argv[]) {
	char *dvalue = NULL;
	int c;

	opterr = 0;
	
	while ((c = getopt(argc, argv, "d:")) != -1)
		switch (c) {
			case 'd':
				dvalue = optarg;
				break;
			case '?':
				if (optopt == 'd')
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
}

int main(int argc, char *argv[]) {

	/* process arguments */
	handle_opts(argc, argv);	

	log("starting parser\n");
	yyparse();

	log("done\n");
	return 0;
}
