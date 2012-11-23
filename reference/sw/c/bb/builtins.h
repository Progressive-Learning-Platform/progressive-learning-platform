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
.org 0x10000000\n\
\n\
_entrypoint:\n\
# enable the stack\n\
li $sp, 0x10fffffc\n\
\n\
# call main\n\
call main\n\
nop\n\
\n\
# if we get here just halt\n\
_halt:\n\
j _halt\n\
nop\n\
\n\
# end of entrypoint\n";

char *builtin_metafile = 
"\
PLP-5.0\n\
DIRTY=1\n\
ARCH=0\n\
\n";

#endif

