all: clean build nolog

CC = gcc
CFLAGS = -O2 -Wall -g -fPIC

LDFLAGS = -larchive -static

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
	+@$(CC) $(CFLAGS) $(OBJECTS) -o plpbb $(LDFLAGS) >>link.build.out 2>&1 || (echo LINK FAIL $*; cat link.build.out; exit 1)

clean: clean.files 
	+@rm -f plpbb

build: build.files build.link

