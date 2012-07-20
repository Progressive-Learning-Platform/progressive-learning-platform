all: build test clean

TESTS=\
	for\
	func\
	array\
	string\
	define\
	recursion\
	struct\

build.tests:	$(addsuffix .build, $(TESTS))
clean.tests:	$(addsuffix .clean, $(TESTS))
test.tests:	$(addsuffix .test, $(TESTS))

%.clean:
	+@echo clean $*
	+@rm -f $*.plp $*.test $*.build

%.build:
	+@echo build $*
	+@plpc -o $*.plp $*.c >$*.build 2>&1 || (echo BUILD FAIL $*; cat $*.build;)

%.test:
	+@echo -n test $*...
	+@$(plptool) -r $*.plp $*.script >$*.test 2>&1 || (echo PLPTOOL FAIL $*; cat $*.test;)
	+@grep -q True $*.test && echo " PASS" || (echo " FAIL"; grep -A 1 False $*.test)
	 
clean: clean.tests
test: test.tests
build: build.tests
