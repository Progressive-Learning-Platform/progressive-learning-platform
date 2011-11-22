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

#ifndef DEFINES
#define DEFINES

#include <stdio.h>
#include <stdlib.h>

typedef struct define_t {
	char *macro;
	char *expansion;
	struct define_t *next;
} define;

define* install_define(define *d, char* m, char *e);
char* find_define(define *d, char* m);
void print_defines(FILE *f, define *d);

#endif
