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
#include <time.h>
#include "log.h"

/* for getopts */
#include <ctype.h>
#include <unistd.h>

/* libarchive */
#include <fcntl.h>
#include <archive.h>
#include <archive_entry.h>

#include "builtins.h"
#define BUFFER 8192

int LOG_LEVEL = 0;

static char *S_FILE_OUTPUT = NULL;

char **files = NULL;
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
	char **filenames = files;
	if (f == NULL) {
		ret = strdup(builtin_metafile);
		ret = realloc(ret, strlen(ret) + strlen("entrypoint.asm") + 2);
		ret = strcat(ret, "entrypoint.asm");
		ret = strcat(ret, "\n");
		while (*filenames) {
			ret = realloc(ret, strlen(ret) + strlen(*filenames) + 2);
			ret = strcat(ret, *filenames);
			ret = strcat(ret, "\n");
			filenames++;
		}
		return ret;
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
		int index = 0;
		files = malloc(sizeof(char*) * (argc-optind));
		while (optind < argc) {
			files[index] = argv[optind];
			log("[plpbb] input file: %s\n", argv[optind]);
			optind++;
			index++;
		}
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

void write_archive(char *o, char **files) {
	struct archive *a;
	struct archive_entry *entry;
	struct stat st;
	char buff[BUFFER];
	int len;
	int fd;
	a = archive_write_new();
	archive_write_set_format_pax_restricted(a);
	archive_write_open_filename(a, o);

	/* put the metafile and entrypoint in there */
	entry = archive_entry_new();
	archive_entry_set_pathname(entry, "plp.metafile");
	archive_entry_set_size(entry, strlen(metafile));
	archive_entry_set_filetype(entry, AE_IFREG);
	archive_entry_set_perm(entry, 0644);
	archive_write_header(a, entry);
	archive_write_data(a, metafile, strlen(metafile));
	archive_entry_free(entry);
	
	entry = archive_entry_new();
	archive_entry_set_pathname(entry, "entrypoint.asm");
	archive_entry_set_size(entry, strlen(entrypoint));
	archive_entry_set_filetype(entry, AE_IFREG);
	archive_entry_set_perm(entry, 0644);
	archive_write_header(a, entry);
	archive_write_data(a, entrypoint, strlen(entrypoint));
	archive_entry_free(entry);

	if (files != NULL) {
		while (*files) {
			stat(*files, &st);
			entry = archive_entry_new();
			archive_entry_set_pathname(entry, *files);
			archive_entry_set_size(entry, st.st_size);
			archive_entry_set_filetype(entry, AE_IFREG);
			archive_entry_set_perm(entry, 0644);
			archive_write_header(a, entry);
			fd = open(*files, O_RDONLY);
			len = read(fd, buff, sizeof(buff));
			while (len > 0) {
				archive_write_data(a, buff, len);
				len = read(fd, buff, sizeof(buff));
			}
			close(fd);
			archive_entry_free(entry);
			files++;
		}
	}
	archive_write_close(a);
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

	log("[plpbb] building archive: %s\n", S_FILE_OUTPUT);
	write_archive(S_FILE_OUTPUT, files);

	log("[plpbb] done\n");
	return 0;
}
