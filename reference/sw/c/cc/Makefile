all: clean build nolog

CC = gcc
CFLAGS = -O2 -Wall -g -fPIC

LDFLAGS = -static

FILES_STANDALONE=\
	main\

FILES_JNI=\
	plpcc-jni\

FILES=\
	symbol\
	parse_tree\
	code_gen\
	line\
	handlers\
	lex.yy\
	parser.tab\
	struct
	
OBJECTS = $(addsuffix .o, $(FILES))
OBJECTS_STANDALONE = $(addsuffix .o, $(FILES_STANDALONE))
OBJECTS_JNI = $(addsuffix .o, $(FILES_JNI))

#for making a java so
JNI_INCLUDE = -I/usr/lib/jvm/java-6-openjdk/include/
JNI_OPTS = -shared -Wl,-soname,libplpcclib.so

clean.files: $(addsuffix .clean, $(FILES))
build.files: $(addsuffix .build, $(FILES))
clean.files_standalone: $(addsuffix .clean, $(FILES_STANDALONE))
clean.files_jni: $(addsuffix .clean, $(FILES_JNI))
build.files_standalone: $(addsuffix .build, $(FILES_STANDALONE))
build_jni.files_jni: $(addsuffix .build_jni, $(FILES_JNI))




nolog:
	+@rm -f flex.build.out bison.build.out build.out link.build.out build_jni.out

log: clean build.flex log.flex build.bison log.bison build.files build.files_standalone log.files build.link log.link

log.flex:
	+@sed  's/^/     /'  flex.build.out

log.bison:
	+@sed  's/^/     /'  bison.build.out

log.files:
	+@sed  's/^/     /'  build.out

log.link:
	+@sed  's/^/     /'  link.build.out

log.jni:
	+@sed  's/^/	/'	 build_jni.out

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

clean.files_jni:
	+@echo clean jni
	+@rm -f libplpcclib.so

%.build:
	+@echo build $*
	+@$(CC) $(CFLAGS) -c $*.c >>build.out 2>&1 || (echo BUILD FAIL $*; cat build.out; exit 1)

build.link:
	+@echo link
	+@$(CC) $(LDFLAGS) -o plpcc $(OBJECTS) $(OBJECTS_STANDALONE) >>link.build.out 2>&1 || (echo LINK FAIL $*; cat link.build.out; exit 1)

clean: clean.flex clean.bison clean.files clean.files_standalone clean.files_jni
	+@rm -f plpcc

%.build_jni:
	+@echo jni $*
	+@$(CC) $(CFLAGS) $(JNI_OPTS) $(JNI_INCLUDE) $(OBJECTS) $*.c -o libplpcclib.so >> build_jni.out 2>&1 || (echo JNI FAIL; cat build_jni.out; exit 1)

build: build.flex build.bison build.files build.files_standalone build.link

jni: clean build.flex build.bison build.files build_jni.files_jni nolog

jni_log: clean build.flex log.flex build.bison log.bison build.files log.files build_jni.files_jni log.jni
