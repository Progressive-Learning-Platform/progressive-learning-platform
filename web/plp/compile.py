#!/usr/bin/env python

import cgi
import sys
import random
import subprocess
import re
import os
import string
import cgitb
cgitb.enable(display=0, logdir='/tmp')

def run_program(prog,filename):
	f = open("/tmp/" + filename + ".asm",'w')
	f.write(prog)
	f.close()
	command = ["java", "-jar", "PLPToolStatic.jar", "-a", "/tmp/%(filename)s.asm" % {'filename': filename}, "/tmp/%(filename)s.plp" % {'filename': filename}]
	data = subprocess.Popen(command, stdout=subprocess.PIPE).communicate()[0]
	command = ["java", "-jar", "PLPToolStatic.jar", "-s", "/tmp/%(filename)s.plp" % {'filename': filename}]
	p = subprocess.Popen(command, stdout=subprocess.PIPE, stdin=subprocess.PIPE)

	#100 cycles
	s = ""
	for i in range(100):
		s = s + "s\npreg\n"
	s = s + "q\n"	

	data = p.communicate(input=s)[0]

	#clean up
	os.remove("/tmp/" + filename + ".asm")
	os.remove("/tmp/" + filename + ".plp")
	return data

def parse_data(data):
	run = 0
	cwb  = []
	cmem = []
	cex  = []
	cid  = []
	cif  = []
	cpc  = []
	regs = []
	for i in xrange(32):
		regs.append([])
	for line in string.split(data,"\n"):
		#our matches
		if re.match("00000000 0 sim", line):
			run = 1
		if run:
			if re.match("wb:", line):
				cwb.append(line + "<br>")
			elif re.match("mem:", line):
				cmem.append(line + "<br>")
			elif re.match("ex:", line):
				cex.append(line + "<br>")
			elif re.match("id:", line):
				cid.append(line + "<br>")
			elif re.match("if:", line):
				cif.append(line + "<br>")
			elif re.match("i_pc:", line):
				cpc.append(line + "<br>")
			elif re.match("1\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r' \1 \2',line)
				regs[1].append(line + "  ")
			elif re.match("2\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r' \1 \2',line)
				regs[2].append(line + "  ")
			elif re.match("3\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r' \1 \2',line)
				regs[3].append(line + "<br>")
			elif re.match("4\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r' \1 \2',line)
				regs[4].append(line + "  ")
			elif re.match("5\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r' \1 \2',line)
				regs[5].append(line + "  ")
			elif re.match("6\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r' \1 \2',line)
				regs[6].append(line + "  ")
			elif re.match("7\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r' \1 \2',line)
				regs[7].append(line + "<br>")
			elif re.match("8\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r' \1 \2',line)
				regs[8].append(line + "  ")
			elif re.match("9\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r' \1 \2',line)
				regs[9].append(line + "  ")
			elif re.match("10\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[10].append(line + "  ")
			elif re.match("11\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[11].append(line + "<br>")
			elif re.match("12\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[12].append(line + "  ")
			elif re.match("13\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[13].append(line + "  ")
			elif re.match("14\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[14].append(line + "  ")
			elif re.match("15\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[15].append(line + "<br>")
			elif re.match("16\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[16].append(line + "  ")
			elif re.match("17\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[17].append(line + "  ")
			elif re.match("18\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[18].append(line + "  ")
			elif re.match("19\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[19].append(line + "<br>")
			elif re.match("20\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[20].append(line + "  ")
			elif re.match("21\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[21].append(line + "  ")
			elif re.match("22\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[22].append(line + "  ")
			elif re.match("23\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[23].append(line + "<br>")
			elif re.match("24\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[24].append(line + "  ")
			elif re.match("25\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[25].append(line + "  ")
			elif re.match("26\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[26].append(line + "  ")
			elif re.match("27\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[27].append(line + "<br>")
			elif re.match("28\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[28].append(line + "  ")
			elif re.match("29\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[29].append(line + "  ")
			elif re.match("30\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[30].append(line + "  ")
			elif re.match("31\t([0-9a-f]*)", line):
				line = re.sub(r'([0-9]*)\t([0-9a-f]*)(.*)',r'\1 \2',line)
				regs[31].append(line + "<br>")

	#ok, now generate the json output
	json_output = """{ "output": ["""
	for i in range(len(cpc)):
		json_output = json_output + "\"" + cpc[i] + "<br>" + cif[i] + cid[i] + cex[i] + cmem[i] + cwb[i] + "<br>" + " 0 00000000  " + regs[1][i] + regs[2][i] + regs[3][i] + regs[4][i] + regs[5][i] + regs[6][i] + regs[7][i] + regs[8][i] + regs[9][i] + regs[10][i] + regs[11][i] + regs[12][i] + regs[13][i] + regs[14][i] + regs[15][i] + regs[16][i] + regs[17][i] + regs[18][i] + regs[19][i] + regs[20][i] + regs[21][i] + regs[22][i] + regs[23][i] + regs[24][i] + regs[25][i] + regs[26][i] + regs[27][i] + regs[28][i] + regs[29][i] + regs[30][i] + regs[31][i] + "\","
	json_output = json_output + """ "done"], "compile_errors": "" } """

	return json_output

#main code

print "Content-Type: text/plain\n\n"

form = cgi.FieldStorage()
prog = form.getvalue("p", "(no p)")

if prog == "(no p)":
	print """{ "output": "",
		    "compile_errors": "input error" } """
else:
	random.seed()
	hash = random.getrandbits(128)
	filename = "%016x" % hash
	data = run_program(prog,filename)
	json_output = parse_data(data)
	print json_output

	#print "{ \"output\": [\"" + data + "\"], \"compile_errors\": \"\"}"
