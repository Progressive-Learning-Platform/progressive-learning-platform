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

all: clean build nolog

CC = gcc
CFLAGS = -O2 -Wall -g -fPIC

LDFLAGS = -larchive

FILES=\
	main\
	
OBJECTS = $(addsuffix .o, $(FILES))

clean.files: $(addsuffix .clean, $(FILES))
build.files: $(addsuffix .build, $(FILES))

nolog:
	+@rm -f build.out link.build.out

log: clean build.files log.files build.link log.link

log.files:
	+@sed  's/^/     /'  build.out

log.link:
	+@sed  's/^/     /'  link.build.out

%.clean: nolog
	+@echo clean $*
	+@rm -f $*.o

%.build:
	+@echo build $*
	+@$(CC) $(CFLAGS) -c $*.c >>build.out 2>&1 || (echo BUILD FAIL $*; cat build.out; exit 1)

build.link:
	+@echo link
	+@$(CC) $(LDFLAGS) -o plpbb $(OBJECTS) >>link.build.out 2>&1 || (echo LINK FAIL $*; cat link.build.out; exit 1)

clean: clean.files 
	+@rm -f plpbb

build: build.files build.link

