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

#ifndef LOG
#define LOG
#include <stdlib.h>
#include <stdio.h>

extern int LOG_LEVEL;
extern int STOP_ERROR;

#define log(...) if(LOG_LEVEL>=1){printf(__VA_ARGS__);}
#define err(...) { fprintf(stderr, "ERROR: "); fprintf(stderr,__VA_ARGS__); exit(-1); }
#define vlog(...) if(LOG_LEVEL>=2){printf(__VA_ARGS__);}

#endif 
