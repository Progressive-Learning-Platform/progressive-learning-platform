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

#define BUFFER_LEN 8192

int LOG_LEVEL = 0;
char *S_FILE_OUTPUT = NULL;
char **files = NULL;
char *entrypoint = NULL;
char *metafile = NULL;
int STOP_ERRORS = 1;
int PSYMBOL = 0;
int PDEFINE = 0;
int PPARSE = 0;
int FRONT_ONLY = 0;
int NOANNOTATE = 0;
int PBUILTIN = 0;
int file_index = 0;
int CLEANUP = 1;

void print_usage(void) {
	printf("plpc - plp c toolchain\n\n");
	printf("usage: plpc <options> <input files>\n");
	printf("global options:\n");
	printf("-o <filename>		output filename\n");
	printf("-d [0,1,2]		debug level (0=off (default), 1=on, 2=verbose)\n");
	printf("-e 			do not stop on errors, even if crazy things happen\n");
	printf("--nocleanup		leave intermediate files\n");
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
	printf("binary builder options:\n");
	printf("--entry <filename>	use alternate entry point\n");
	printf("--metafile <filename>	use alternate metafile\n");
	printf("--builtin		print builtin entry point and metafile and exit\n");
}

void handle_opts(int argc, char *argv[]) {
	char *dvalue = NULL;
	char *ovalue = NULL;
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
			{"nocleanup", no_argument, 0, 'k'},
			{0,0,0,0}
		};

		int option_index = 0;
		c = getopt_long(argc, argv, "o:d:eabcfgh:i:jk", long_options, &option_index);
		if (c == -1)
			break;
	
		switch(c) {
			case 'o':
				ovalue = optarg;
				break;
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
				entrypoint = optarg;
				break;
			case 'i':
				metafile = optarg;
				break;
			case 'j':
				PBUILTIN = 1;
				break;
			case 'k':
				CLEANUP = 0;
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
		files = malloc(sizeof(char*) * (argc-optind));
		while (optind < argc) {
			files[file_index] = argv[optind];
			log("[plpc] input file           : %s\n", argv[optind]);
			optind++;
			file_index++;
		}
	} else {
		err("[plpc] no input files specified\n");
		exit(-1);
	}
}

int main(int argc, char *argv[]) {
	char command[BUFFER_LEN];
	int ret = 0;
	int i;

	/* process arguments */
	handle_opts(argc, argv);	


	/* PREPROCESSOR */

	log("[plpc] preprocessor\n");
	/* build preprocessor arguments */
	sprintf(command, "plppp -o %s.pp -d %d %s %s ",
		S_FILE_OUTPUT, 			/* output file name */
		LOG_LEVEL, 			/* debug level */
		STOP_ERRORS ? "" : "-e", 	/* stop on errors */
		PDEFINE ? "-p" : "" 		/* print defines and exit */
	);
	/* now add the files */
	for (i=0; i<file_index; i++)
		sprintf(command, "%s %s", command, files[i]);

	vlog("[plpc] preprocessor arguments: %s\n", command);

	/* run it! */
	ret = system(command);
	if (ret != 0) {
		err("[plpc] preprocessor returned error: %d\n", ret);
	}

	/* COMPILER */

	log("[plpc] c compiler\n");
	
	/* build compiler options */
	sprintf(command, "plpcc -o %s.asm -d %d	%s %s %s %s %s %s.pp",
		S_FILE_OUTPUT,			/* output file name */
		LOG_LEVEL,			/* debug level */
		PSYMBOL ? "-s" : "",		/* print symbol table */
		PPARSE ? "-p" : "", 		/* print parse tree */
		STOP_ERRORS ? "" : "-e",	/* stop on errors */
		FRONT_ONLY ? "-f" : "", 	/* run front end only */
		NOANNOTATE ? "-a" : "", 	/* no c annotation */
		S_FILE_OUTPUT			/* input file */
	);
	vlog("[plpc] compiler arguments: %s\n", command);

	/* run it! */
	ret = system(command);
	if (ret != 0) {
		err("[plpc] compiler returned error: %d\n", ret);
	}

	/* BINARY BLOB */

	log("[plpc] binary builder\n");

	/* build binary builder options */
	sprintf(command, "plpbb -o %s -d %d %s %s %s %s %s %s.asm", 
		S_FILE_OUTPUT,				/* output file name */
		LOG_LEVEL,				/* debug level */
		entrypoint == NULL ? "" : "-e", 	/* entrypoint */
		entrypoint == NULL ? "" : entrypoint, 
		metafile == NULL ? "" : "-m",		/* metafile */
		metafile == NULL ? "" : metafile,	
		PBUILTIN ? "-p" : "",			/* print builtins */
		S_FILE_OUTPUT
	);	
	vlog("[plpc] binary builder arguments: %s\n", command);
	
	/* run it! */
	ret = system(command);
	if (ret != 0) {
		err("[plpc] binary builder returned error: %d\n", ret);
	}

	/* unlink the files we created, maybe. */
	if (CLEANUP) {	
		log("[plpc] removing intermediate files\n");
		sprintf(command, "%s.pp", S_FILE_OUTPUT);
		remove(command);
		sprintf(command, "%s.asm", S_FILE_OUTPUT);
		remove(command);
	}

	log("[plpc] done\n");
	return 0;
}
