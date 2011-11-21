all: clean build

DIRS=\
	pp\
	cc\

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
	+@rm -f bin/plpcc bin/plppp
	+@rmdir bin

copy_bin:
	+@echo copy
	+@mkdir bin
	+@cp pp/plppp bin
	+@cp cc/plpcc bin

clean: clean.dirs clean_bin
build: build.dirs copy_bin

