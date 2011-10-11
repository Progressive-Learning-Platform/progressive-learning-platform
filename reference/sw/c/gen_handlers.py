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
	print "void handler_" + x + """(node *n) {
	err("[code_gen] handler_""" + x + """ not implemented\\n");
}
"""

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

