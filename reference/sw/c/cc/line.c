#include <stdio.h>
#include <stdlib.h>
#include "line.h"
#include "log.h"

char **source_lines;
int *visited;
int num_lines = 0;

int is_visited(int l) {
	return visited[l];
}

void visit(int l) {
	visited[l] = 1;
}

char* get_line(int i) {
	return source_lines[i];
}

void build_lines(char *filename) {
	source_lines = NULL;
	char *line = NULL;
	ssize_t len = 0;
	
	/* open the file */
	FILE *fp = fopen(filename, "r");

	while (getline(&line, &len, fp) != -1) {
		/* realloc the source_lines */
		num_lines++;
		source_lines = realloc(source_lines, sizeof(char*)*num_lines);
		source_lines[num_lines-1] = line;
		line = NULL; /* don't let getline call realloc */
	}
	
	visited = calloc(num_lines, sizeof(int));
	
	vlog("[line] read %d lines\n", num_lines);
	fclose(fp);
}
