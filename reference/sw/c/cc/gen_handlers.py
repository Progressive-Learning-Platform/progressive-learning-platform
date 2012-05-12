#
#    Copyright 2011 the PLP authors
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#

#script to generate empty backend handlers

#!/usr/bin/env python
import sys
import re

infile  = open(sys.argv[1], 'r')
handlers = []
prev = ""

count = 0
for line in infile:
        m = re.search('(?<=op\(").+"', line)
	if m != None:
		if m.group(0) != prev:
			handlers.append(m.group(0)[:len(m.group(0))-1])
		prev = m.group(0)
        count = count + 1

for x in handlers:
	print "void handle_" + x + """(node *n) {
	err("[code_gen] handle_""" + x + """ not implemented\\n");
}
"""

#generate the handler declarations
for x in handlers:
	print "void handle_" + x + """(node *);"""

# now generate the handle function
print """void handle(node *n) {
"""
for x in handlers:
	if x != handlers[0]:
		print "else",
	else:
		print "\t",
	print '''if (strcmp(n->id,"''' + x + '''") == 0) {
		vlog("[code_gen] handle: ''' + x + ''' \\n");
		handle_''' + x + '''(n);
	}''',
print """
}
"""

infile.close()

