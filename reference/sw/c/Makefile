all: build

CC = gcc
CFLAGS = -O2 -Wall
LDFLAGS = 

FILES=\
	main\
	lex.yy\
	parser.tab\
	
OBJECTS = $(addsuffix .o, $(FILES))

clean.files: $(addsuffix .clean, $(FILES))
build.files: $(addsuffix .build, $(FILES))

build.flex:
	+@echo build lexer
	+@flex -s lexer.lex >flex.build.out 2>&1 || (echo BUILD FAIL lexer; cat flex.build.out; exit 1)
	+@rm -f flex.build.out

clean.flex:
	+@echo clean lexer
	+@rm -f lex.yy.c

build.bison:
	+@echo build bison
	+@bison -d parser.y >bison.build.out 2>&1 || (echo BUILD FAIL bison; cat bison.build.out; exit 1)
	+@rm -f bison.build.out

clean.bison:
	+@echo clean bison
	+@rm -f parser.tab.c parser.tab.h

%.clean:
	+@echo clean $*
	+@rm -f $*.o

%.build:
	+@echo build $*
	+@$(CC) $(CFLAGS) -c $*.c >>build.out 2>&1 || (echo BUILD FAIL $*; cat build.out; exit 1)
	+@rm -f build.out

build.link:
	+@echo link
	+@$(CC) $(LDFLAGS) -o pcc $(OBJECTS)

clean: clean.flex clean.bison clean.files
	+@rm -f pcc

build: build.flex build.bison build.files build.link

