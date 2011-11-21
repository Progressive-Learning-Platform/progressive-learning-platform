#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "log.h"

/* for getopts */
#include <ctype.h>
#include <unistd.h>

/* libarchive */
#include <archive.h>
#include <archive_entry.h>

#include "builtins.h"

#define BUFFER 8192

int LOG_LEVEL = 0;

static char *S_FILE_INPUT = NULL;
static char *S_FILE_OUTPUT = NULL;
static FILE *FILE_INPUT = NULL;
static FILE *FILE_OUTPUT = NULL;

char *entrypoint = NULL;
char *metafile = NULL;

char *build_entrypoint(char *f) {
	char *ret;
	if (f == NULL) {
		/* use the builtin */
		time_t rawtime;
		struct tm *timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		char *t = asctime(timeinfo);

		ret = strdup(entrypoint_header);
		ret = realloc(ret, strlen(ret) + strlen(t) + 1 + strlen(entrypoint_body));
		ret = strcat(ret, t);
		ret = strcat(ret, entrypoint_body);
	} else { 
		FILE *fd = fopen(f,"r");
		int total = BUFFER;
		int len;
		if (fd == NULL) {
			err("[plpbb] could not open entrypoint file: %s\n", f);
		}
		ret = malloc(total);
		len = fread(ret, BUFFER, 1, fd);
		while (len > 0) {
			ret = realloc(ret, total + BUFFER);
			len = fread(ret + total, BUFFER, 1, fd);
			total += BUFFER;
		}
	}
	return ret;
}

char *build_metafile(char *f) {
	char *ret;
	if (f == NULL) {
		return builtin_metafile;
	} else { 
		FILE *fd = fopen(f,"r");
                int total = BUFFER;
                int len;
		if (fd == NULL) {
			err("[plpbb] could not open metafile file: %s\n", f);
		}
                ret = malloc(total);
                len = fread(ret, BUFFER, 1, fd);
                while (len > 0) {
                        ret = realloc(ret, total + BUFFER);
                        len = fread(ret + total, BUFFER, 1, fd);
                        total += BUFFER;
                }
	}
	return ret;
}

void print_usage(void) {
	printf("plpbb - plp binary builder\n\n");
	printf("usage: plpbb <options> <input files>\n");
	printf("options:\n");
	printf("-o <filename>	output filename\n");
	printf("-d [0,1,2]	debug level (0=off (default), 1=on, 2=verbose)\n");
	printf("-e <filename>	use alternate entry point\n");
	printf("-m <filename>	use alternate metafile\n");
	printf("-p		print builtin entry point and metafile and exit\n");
}

void print_builtin(void) {
	char *e = build_entrypoint(NULL);
	char *m = build_metafile(NULL);
	printf("[plpbb] builtin entrypoint: \n\n%s\n\n", e);
	printf("[plpbb] builtin metafile: \n\n%s\n", m);
}

void handle_opts(int argc, char *argv[]) {
	char *dvalue = NULL;
	char *ivalue = NULL;
	char *ovalue = NULL;

	int c;

	opterr = 0;
	
	while ((c = getopt(argc, argv, "d:o:e:m:p")) != -1)
		switch (c) {
			case 'd':
				dvalue = optarg;
				break;
			case 'o':
				ovalue = optarg;
				break;
			case 'e':
				entrypoint = optarg;
				break;
			case 'm':
				metafile = optarg;
				break;
			case 'p':
				print_builtin();
				exit(0);
			default:
				print_usage();
				exit(-1);
		}

	if (dvalue != NULL) {
		LOG_LEVEL = atoi(dvalue);
		vlog("[plpbb] setting log level: %d\n", LOG_LEVEL);
	}

	/* grab remaining values as inputs */
	if (optind < argc) {
		while (optind < argc) {
			ivalue = argv[optind];
			log("[plpbb] input file: %s\n", argv[optind]);
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
		sprintf(S_FILE_OUTPUT,"plpbb.plp");
	}
	log("[plpbb] output file: %s\n", S_FILE_OUTPUT);

}

int main(int argc, char *argv[]) {

	/* process arguments */
	handle_opts(argc, argv);	

	/* get the entrypoint and metafile */
	vlog("[plpbb] building entrypoint\n");
	entrypoint = build_entrypoint(entrypoint);
	vlog("[plpbb] entrypoint is: \n%s\n", entrypoint);
	vlog("[plpbb] building metafile\n");
	metafile = build_metafile(metafile);	
	vlog("[plpbb] metafile is: \n%s\n", metafile);

	/* open files */
	vlog("[plpbb] opening files\n");
	FILE_INPUT = fopen(S_FILE_INPUT,"r");
	if (FILE_INPUT == NULL) {
		err("[plpbb] cannot open input file: %s\n", S_FILE_INPUT);
	}
	FILE_OUTPUT = fopen(S_FILE_OUTPUT,"w");
	if (FILE_OUTPUT == NULL) {
		err("[plpbb] cannot open output file: %s\n", S_FILE_OUTPUT);
	}

	vlog("[plpbb] closing files\n");
	fclose(FILE_INPUT);
	fclose(FILE_OUTPUT);

	log("[plpbb] done\n");
	return 0;
}
