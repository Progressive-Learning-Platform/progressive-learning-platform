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
