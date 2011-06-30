/* Copyright (c) 1991-2007 Pragmatic C Software Corp. */

/*
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2 of the License, or (at your
   option) any later version.
 
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
 
   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   59 Temple Place, Suite 330, Boston, MA, 02111-1307.
 
   There is also a commerically supported faster new version of Cver that is
   not released under the GPL.   See file commerical-cver.txt, or web site
   www.pragmatic-c.com/commercial-cver or contact sales@pragmatic-c.com to
   learn more about commerical Cver.
   
 */


/*
 * system function id numbers
 * notice no patters other than pli tf_ start above these (at 1000 for now)
 */

/* functions for converting reals */
#define STN_BITSTOREAL 1
#define STN_ITOR 2
#define STN_REALTOBITS 3
#define STN_RTOI 4

/* time related system functions */
#define STN_REALTIME 5
#define STN_STIME 6
#define STN_TIME 7
#define STN_SCALE 8

/* random number generators */
#define STN_RANDOM 9
#define STN_DIST_UNIFORM 10
#define STN_DIST_NORMAL 11
#define STN_DIST_EXPONENTIAL 12
#define STN_DIST_POISSON 13
#define STN_DIST_CHI_SQUARE 14
#define STN_DIST_T 15
#define STN_DIST_ERLANG 16

/* misc. system functions */
#define STN_TESTPLUSARGS 17
#define STN_FOPEN 18
#define STN_COUNT_DRIVERS 19
#define STN_RESET_COUNT 21
#define STN_RESET_VALUE 22
#define STN_GETPATTERN 20
#define STN_Q_FULL 23

/* AIV 09/08/03 - new P1364 2001 fileio sys funcs */
#define STN_FGETC 24
#define STN_UNGETC 25
#define STN_FGETS 26
#define STN_FTELL 27
#define STN_REWIND 28
#define STN_FSEEK 29
#define STN_FERROR 30 
#define STN_FREAD 31
#define STN_FSCANF 32 
#define STN_SSCANF 33

/* system task id numbers */

/* display type tasks */
#define STN_FCLOSE 100
/* formated procedural display - implied cr */
#define STN_DISPLAY 101
#define STN_DISPLAYB 102
#define STN_DISPLAYH 103
#define STN_DISPLAYO 104
#define STN_FDISPLAY 105
#define STN_FDISPLAYH 106
#define STN_FDISPLAYB 107
#define STN_FDISPLAYO 108

/* formated procedural display - no implied cr */
#define STN_WRITE 109
#define STN_WRITEH 110
#define STN_WRITEB 111
#define STN_WRITEO 112
#define STN_FWRITE 113
#define STN_FWRITEH 114
#define STN_FWRITEB 115
#define STN_FWRITEO 116

/* like display except write at end of current time */
#define STN_STROBE 117
#define STN_STROBEH 118
#define STN_STROBEB 119
#define STN_STROBEO 120
#define STN_FSTROBE 121
#define STN_FSTROBEH 122
#define STN_FSTROBEB 123
#define STN_FSTROBEO 124

#define STN_MONITOR 125
#define STN_MONITOROFF 126
#define STN_MONITORON 127
#define STN_FMONITOR 128
#define STN_MONITORH 129
#define STN_MONITORB 130
#define STN_MONITORO 131
#define STN_FMONITORH 132
#define STN_FMONITORB 133
#define STN_FMONITORO 134

/* time releated system tasks */
#define STN_PRINTTIMESCALE 135
#define STN_TIMEFORMAT 136

/* notice specify section timing checks not here - no exec - built in tevs */
#define STN_READMEMB 137
#define STN_READMEMH 138
#define STN_SREADMEMB 139
#define STN_SREADMEMH 140

/* dump variables tasks */
#define STN_DUMPVARS 141
#define STN_DUMPALL 142
#define STN_DUMPFILE 143
#define STN_DUMPFLUSH 144
#define STN_DUMPLIMIT 145
#define STN_DUMPOFF 146
#define STN_DUMPON 147

/* mainly interactive system tasks */
#define STN_KEY 148
#define STN_NOKEY 149
#define STN_LIST 150
#define STN_LOG 151
#define STN_NOLOG 152
#define STN_HISTORY 153
#define STN_SAVE 154
#define STN_INCSAVE 155
#define STN_INPUT 156
#define STN_RESTART 157
#define STN_SETTRACE 158
#define STN_CLEARTRACE 159
#define STN_RESET 160
#define STN_FINISH 161
#define STN_STOP 162
#define STN_KEEPCMDS 163
#define STN_NOKEEPCMDS 164

/* internal simulation state printng tasks */
#define STN_SCOPE 165
#define STN_SHOWALLINSTANCES 166
#define STN_SHOWEXPANDEDNETS 167
#define STN_SHOWSCOPES 168
#define STN_SHOWVARIABLES 169
#define STN_SHOWVARS 170

/* q manipulation tasks - also q_full function */
#define STN_Q_INITIALIZE 171
#define STN_Q_ADD 172
#define STN_Q_REMOVE 173
#define STN_Q_EXAM 174

/* sdf annotate system task */
#define STN_SDF_ANNOTATE 175

/* new P1364 2001 special signed/word32 conversion system functions */
#define STN_SIGNED 176
#define STN_UNSIGNED 177

/* graphical output tasks */
#define STN_GRREMOTE 180
#define STN_PSWAVES 181
#define STN_GRSYNCHON 182
#define STN_GRREGS 183
#define STN_GRWAVES 184
#define STN_FREEZEWAVES 185
#define STN_DEFINEGROUPWAVES 186

/* Cver extension (system tasks or functions) */
#define STN_SETDEBUG 190 
#define STN_CLEARDEBUG 191
#define STN_SETEVTRACE 192
#define STN_CLEAREVTRACE 193
#define STN_TRACEFILE 194
#define STN_STICKSTIME 195
#define STN_TICKSTIME 196
#define STN_SNAPSHOT 197
#define STN_SYSTEM 198
#define STN_SUPWARNS 199
#define STN_ALLOWWARNS 200
#define STN_MEMUSE 201
#define STN_FLUSHLOG 202
#define STN_SCANPLUSARGS 203
#define STN_VALUEPLUSARGS 204

/* new ams transcendental function extension but works in digital */
/* SJM 10/01/03 - leaving in for now */
#define STN_COS 217
#define STN_SIN 218
#define STN_TAN 219
#define STN_ACOS 220
#define STN_ASIN 221
#define STN_ATAN 222
#define STN_ATAN2 223
#define STN_COSH 224
#define STN_SINH 225
#define STN_TANH 226
#define STN_ACOSH 227
#define STN_ASINH 228
#define STN_ATANH 229

#define STN_INT 230
#define STN_SGN 231
#define STN_LN 232
#define STN_LOG10 233
#define STN_ABS 234
#define STN_POW 235
#define STN_SQRT 236
#define STN_EXP 237
#define STN_MIN 238
#define STN_MAX 239

#define STN_HSQRT 240
#define STN_HPOW 241
#define STN_HPWR 242
#define STN_HLOG 243
#define STN_HLOG10 244
#define STN_HDB 245
#define STN_HSIGN 246
#define STN_HYPOT 247

/* AIV 09/08/03 - new P1364 2001 fileio sys tasks */
#define STN_FFLUSH 248
#define STN_SWRITE 249
#define STN_SWRITEB 250
#define STN_SWRITEH 251
#define STN_SWRITEO 252
#define STN_SFORMAT 253

