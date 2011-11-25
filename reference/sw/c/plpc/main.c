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

/* for getopts */
#include <ctype.h>
#include <unistd.h>

int LOG_LEVEL = 0;

void print_usage(void) {
	printf("plpc - plp c toolchain\n\n");
	printf("usage: plpc <options> <input files>\n");
	printf("global options:\n");
	printf("-o <filename>		output filename\n");
	printf("-d [0,1,2]		debug level (0=off (default), 1=on, 2=verbose)\n");
	printf("-e 			do not stop on errors, even if crazy things happen\n");
	printf("\n");
	printf("preprocessor options:\n");
	printf("-define			print defines\n");
	printf("\n");
	printf("compiler options:\n");
	printf("-symbol			print symbol table to <output name>.symbol\n");
	printf("-parse			print parse tree to <output name>.parse\n");
	printf("-front			run the front end only, do not call handle() on the parse tree\n");
	printf("-noannotate		do not annotate output with original source\n");
	printf("\n");
	printf("binary blob options:\n");
	printf("-entry <filename>	use alternate entry point\n");
	printf("-metafile <filename>	use alternate metafile\n");
	printf("-builtin		print builtin entry point and metafile and exit\n");
}

void handle_opts(int argc, char *argv[]) {
	print_usage();
//	char *dvalue = NULL;
//	char *ovalue = NULL;
//
//	int c;
//
//	opterr = 0;
//	
//	while ((c = getopt(argc, argv, "d:o:e:m:p")) != -1)
//		switch (c) {
//			case 'd':
//				dvalue = optarg;
//				break;
//			case 'o':
//				ovalue = optarg;
//				break;
//			case 'e':
//				entrypoint = optarg;
//				break;
//			case 'm':
//				metafile = optarg;
//				break;
//			case 'p':
//				print_builtin();
//				exit(0);
//			default:
//				print_usage();
//				exit(-1);
//		}
//
//	if (dvalue != NULL) {
//		LOG_LEVEL = atoi(dvalue);
//		vlog("[plpbb] setting log level: %d\n", LOG_LEVEL);
//	}
//
//	/* grab remaining values as inputs */
//	if (optind < argc) {
//		int index = 0;
//		files = malloc(sizeof(char*) * (argc-optind));
//		while (optind < argc) {
//			files[index] = argv[optind];
//			log("[plpbb] input file: %s\n", argv[optind]);
//			optind++;
//			index++;
//		}
//	} else {
//		print_usage();
//		exit(-1);
//	}
//
//	if (ovalue != NULL) {
//		S_FILE_OUTPUT = ovalue;
//	} else {
//		S_FILE_OUTPUT = malloc(11);
//		sprintf(S_FILE_OUTPUT,"plpbb.plp");
//	}
//	log("[plpbb] output file: %s\n", S_FILE_OUTPUT);
//
}

int main(int argc, char *argv[]) {

	/* process arguments */
	handle_opts(argc, argv);	

	log("[plpc] options:\n");
	log("[plpc] preprocessor\n");
	log("[plpc] c compiler\n");
	log("[plpc] binary blob\n");

	log("[plpc] done\n");
	return 0;
}
