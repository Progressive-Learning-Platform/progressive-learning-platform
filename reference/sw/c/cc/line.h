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

#ifndef LINE
#define LINE
#include "log.h"

#define llog(x, ...) if(LOG_LEVEL>=1){printf("AT LINE %d: %s\t", x, get_line(x)); printf(__VA_ARGS__);}
#define lerr(x, ...) { fprintf(stderr, "AT LINE %d: %s\t", x, get_line(x)); fprintf(stderr, "ERROR: "); fprintf(stderr,__VA_ARGS__); if (STOP_ERROR) exit(-1); }
#define lvlog(x, ...) if(LOG_LEVEL>=2){printf("AT LINE %d: %s\t", x, get_line(x)); printf(__VA_ARGS__);}

void build_lines(char*);
char* get_line(int);
int is_visited(int);
void visit(int);

#endif
