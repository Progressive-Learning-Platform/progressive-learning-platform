#ifndef BUILTIN
#define BUILTIN

char *entrypoint_header = 
"\
#\n\
# plp standard entrypoint\n\
#\n\
# created on: ";

char *entrypoint_body = 
"\
#\n\
\n\
_entrypoint:\n\
# enable the stack\n\
li $sp, 0x10fffffc\n\
\n\
# call main\n\
jal main\n\
nop\n\
\n\
# if we get here just halt\n\
_halt:\n\
j halt\n\
nop\n\
\n\
# end of entrypoint\n";

char *builtin_metafile = 
"\
PLP-4.0\n\
DIRTY=1\n\
ARCH=plpmips\n\
\n";

#endif

