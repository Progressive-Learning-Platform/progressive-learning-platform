all: clean build nolog

CC = gcc
CFLAGS = -O2 -Wall
LDFLAGS = 

FILES=\
	main\
	handlers\
	stack\
	symbol\
	lex.yy\
	parser.tab\
	
OBJECTS = $(addsuffix .o, $(FILES))

clean.files: $(addsuffix .clean, $(FILES))
build.files: $(addsuffix .build, $(FILES))

nolog:
	+@rm -f flex.build.out bison.build.out build.out link.build.out

log: clean build.flex log.flex build.bison log.bison build.files log.files build.link log.link

log.flex:
	+@sed  's/^/     /'  flex.build.out

log.bison:
	+@sed  's/^/     /'  bison.build.out

log.files:
	+@sed  's/^/     /'  build.out

log.link:
	+@sed  's/^/     /'  link.build.out

build.flex:
	+@echo build lexer
	+@flex -s lexer.lex >flex.build.out 2>&1 || (echo BUILD FAIL lexer; cat flex.build.out; exit 1)

clean.flex:
	+@echo clean lexer
	+@rm -f lex.yy.c

build.bison:
	+@echo build bison
	+@bison -d parser.y >bison.build.out 2>&1 || (echo BUILD FAIL bison; cat bison.build.out; exit 1)

clean.bison:
	+@echo clean bison
	+@rm -f parser.tab.c parser.tab.h

%.clean: nolog
	+@echo clean $*
	+@rm -f $*.o

%.build:
	+@echo build $*
	+@$(CC) $(CFLAGS) -c $*.c >>build.out 2>&1 || (echo BUILD FAIL $*; cat build.out; exit 1)

build.link:
	+@echo link
	+@$(CC) $(LDFLAGS) -o pcc $(OBJECTS) >>link.build.out 2>&1 || (echo LINK FAIL $*; cat link.build.out; exit 1)

clean: clean.flex clean.bison clean.files
	+@rm -f pcc

build: build.flex build.bison build.files build.link

