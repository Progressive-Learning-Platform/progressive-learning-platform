all: clean build test

DIRS=\
	pp\
	cc\
	bb\
	plpc\

clean.dirs:	$(addsuffix .clean, $(DIRS))
build.dirs:	$(addsuffix .build, $(DIRS))

%.clean:
	+@echo clean $*
	+@$(MAKE) -C $* clean
	+@rm -f $*/clean.out

%.build:
	+@echo build $*
	+@$(MAKE) -C $* build
	+@rm -f $*/clean.out

clean_bin:
	+@echo clean binaries
	-@rm -f bin/plpcc bin/plppp bin/plpbb bin/plpc
	-@rmdir bin

copy_bin:
	+@echo copy
	-@mkdir bin
	+@cp pp/plppp bin
	+@cp cc/plpcc bin
	+@cp bb/plpbb bin
	+@cp plpc/plpc bin

test:
	+@echo test
	+@$(MAKE) -C tests

clean_test:
	+@echo clean test
	+@$(MAKE) -C tests clean

clean: clean.dirs clean_test clean_bin
build: build.dirs copy_bin
