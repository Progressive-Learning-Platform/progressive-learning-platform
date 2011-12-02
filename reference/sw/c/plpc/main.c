/*
    Copyright 2011 the PLP authors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"

/* for getopt long */
#include <getopt.h>

int LOG_LEVEL = 0;
char *S_FILE_OUTPUT = NULL;
char **files = NULL;
int STOP_ERRORS = 1;
int PSYMBOL = 0;
int PDEFINE = 0;
int PPARSE = 0;
int FRONT_ONLY = 0;
int NOANNOTATE = 0;
int PBUILTIN = 0;

void print_usage(void) {
	printf("plpc - plp c toolchain\n\n");
	printf("usage: plpc <options> <input files>\n");
	printf("global options:\n");
	printf("-o <filename>		output filename\n");
	printf("-d [0,1,2]		debug level (0=off (default), 1=on, 2=verbose)\n");
	printf("-e 			do not stop on errors, even if crazy things happen\n");
	printf("\n");
	printf("preprocessor options:\n");
	printf("--define		print defines\n");
	printf("\n");
	printf("compiler options:\n");
	printf("--symbol		print symbol table to <output name>.symbol\n");
	printf("--parse			print parse tree to <output name>.parse\n");
	printf("--front			run the front end only, do not call handle() on the parse tree\n");
	printf("--noannotate		do not annotate output with original source\n");
	printf("\n");
	printf("binary blob options:\n");
	printf("--entry <filename>	use alternate entry point\n");
	printf("--metafile <filename>	use alternate metafile\n");
	printf("--builtin		print builtin entry point and metafile and exit\n");
}

void handle_opts(int argc, char *argv[]) {
	char *dvalue = NULL;
	char *ovalue = NULL;
	char *evalue = NULL;
	char *mvalue = NULL;
	int c;

	while (1) {
		static struct option long_options[] = {
			{"define", no_argument, 0, 'a'},
			{"symbol", no_argument, 0, 'b'},
			{"parse", no_argument, 0, 'c'},
			{"front", no_argument, 0, 'f'},
			{"noannotate", no_argument, 0, 'g'},
			{"entry", required_argument, 0, 'h'},
			{"metafile", required_argument, 0, 'i'}, 
			{"builtin", no_argument, 0, 'j'},
			{0,0,0,0}
		};

		int option_index = 0;
		c = getopt_long(argc, argv, "o:d:eabcfgh:i:j", long_options, &option_index);
		if (c == -1)
			break;
	
		switch(c) {
			case 'a':
				PDEFINE = 1;
				break;
			case 'b':
				PSYMBOL = 1;
				break;
			case 'c':
				PPARSE = 1;;
				break;
			case 'd':
				dvalue = optarg;
				break;
			case 'e':
				STOP_ERRORS = 0;
				break;
			case 'f':
				FRONT_ONLY = 1;
				break;
			case 'g':
				NOANNOTATE = 1;
				break;
			case 'h':
				evalue = optarg;
				break;
			case 'i':
				mvalue = optarg;
				break;
			case 'j':
				PBUILTIN = 1;
				break;
			case '?':
				/* getopt_long printed the error message for us */
				print_usage();
				exit(-1);
				break;
			default:
				print_usage();
				exit(-1);
				break;
		}
	}

	if (dvalue != NULL) {
		LOG_LEVEL = atoi(dvalue);
		vlog("[plpc] setting log level    : %d\n", LOG_LEVEL);
	}

	if (ovalue != NULL) {
		S_FILE_OUTPUT = ovalue;
	} else {
		S_FILE_OUTPUT = malloc(11);
		sprintf(S_FILE_OUTPUT,"plpc.plp");
	}
	log("[plpc] output file          : %s\n", S_FILE_OUTPUT);

	vlog("[plpc] stop on errors       : %s\n", STOP_ERRORS ? "true" : "false");
	vlog("[plpc] print defines        : %s\n", PDEFINE ? "true" : "false");
	vlog("[plpc] print symbol table   : %s\n", PSYMBOL ? "true" : "false");
	vlog("[plpc] print parse tree     : %s\n", PPARSE ? "true" : "false");
	vlog("[plpc] run front end only   : %s\n", FRONT_ONLY ? "true" : "false");
	vlog("[plpc] no source annotation : %s\n", NOANNOTATE ? "true" : "false");
	vlog("[plpc] print builtins       : %s\n", PBUILTIN ? "true" : "false");
	
	/* grab remaining values as inputs */
	if (optind < argc) {
		int index = 0;
		files = malloc(sizeof(char*) * (argc-optind));
		while (optind < argc) {
			files[index] = argv[optind];
			log("[plpc] input file           : %s\n", argv[optind]);
			optind++;
			index++;
		}
	} else {
		err("[plpc] no input files specified\n");
		exit(-1);
	}
}

int main(int argc, char *argv[]) {

	/* process arguments */
	handle_opts(argc, argv);	

	log("[plpc] preprocessor\n");
	log("[plpc] c compiler\n");
	log("[plpc] binary blob\n");

	log("[plpc] done\n");
	return 0;
}
