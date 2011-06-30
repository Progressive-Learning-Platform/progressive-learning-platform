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
 
   We are selling our new Verilog compiler that compiles to X86 Linux
   assembly language.  It is at least two times faster for accurate gate
   level designs and much faster for procedural designs.  The new
   commercial compiled Verilog product is called CVC.  For more information
   on CVC visit our website at www.pragmatic-c.com/cvc.htm or contact 
   Andrew at avanvick@pragmatic-c.com

 */


#define VERS "GPLCVER_2"

#define VERS2 ".12a"

#define OFDT "05/16/07"
#define OUTLINLEN 71          /* length of output file line */
#define DFLTIOWORDS 8         /* preallocated size for I/O and work values */
#define MUSTFREEWORDS 64      /* for stk values wider free when done */
/* ? for debugging */
/* #define DFLTIOWORDS 1 */   /* preallocated size for I/O and work values */
/* #define MUSTFREEWORDS 1 */ /* for stk values wider free when done */

#define MAXNUMBITS 1000000    /* allowing 1 million for now */
#define MAXNUMPORTS 1000000   /* index must fit in 20 bits so 1 million max */
#define MAXXNEST 256          /* initial exp. eval. stack depth - grows */
#define MAXFCNEST 256         /* initial fcall eval stack depth - grows */
#define XNESTFIXINC 0x4096    /* increase xpr stack by this */
#define IDCHARS 4090          /* max. ID length (need 7 for unique) */   
#define IDLEN 4097            /* size of string to hold ID */
#define RECLEN 4097           /* length for normal temp string (plus \0) */
#define MSGTRUNCLEN 1265      /* for msg len. to trunc to (... end) - id fit */
#define TRTRUNCLEN 66         /* for tracing truncate length (short) */
#define MAXFILS 500           /* start number of in and lib. files (grows) */
#define MAXFILNEST 1024       /* max. num. of `include macro nestings */
#define MAXLEVELS 800         /* max. num. nested param/task/block decls */
#define MAXGLBCOMPS 800       /* max. components in global name */
#define MAXLBEXTS 512         /* max. no. of -y library extensions */
#define MAXINCDIRS 2048       /* max. no. of +incidr search paths files */
#define MAXNOOPTMODS 2048     /* max. no of +optimize_off module names */
#define MAXUPRTS 11           /* max. no. of udp ports */
#define UALTREPIPNUM 7        /* lowest port number to use slower udp rep */
#define TWHINITSIZE 4000      /* size of fixed timing wheel */
#define MAXVERIUSERTFS 4095   /* maximum number of pli veriusertfs tfs */
#define SYSTFDATSIZE 256      /* starting size of vpi_ systf table (grows) */
#define BASE_VERIUSERTFS 1000 /* base for pli (veriuser) system tf_s */
#define MAXITDPTH 800         /* dpth of itstk */
#define MAXCIRDPTH 4096       /* maximum depth of a circuit */ 
#define IAHISTSIZ 256         /* start history list line and command size */
#define INFLOOP_MAX 1000      /* stages with time move for loop checking */
#define DFLT_SNAP_EVS 5       /* default number futuren events to print */
#define GDVAL 0x400           /* good value */
#define BDVAL 1000            /* bad value */
#define TOPVPIVAL 1023        /* biggest vpi define const value (MUST EXTND) */
#define PVH_MAGIC 0x625       /* 11 bit vpi handle magic number */

#define MAXPRPSTNEST 800      /* maximum statement nesting */
#define MAXBTDPTH 40          /* maximum depth of timing q (3**40)/2 or so */
#define BTREEMAXWID 8         /* max. b tree node width (usually .5 full) */

#define BIG_ALLOC_SIZE 0x3ff8 /* size of block to allocate at once */
#define VPI_OBJALLOCNUM 2048  /* number of vpi object to allocate at once */
#define RDBUFSIZ 0x4000       /* source list buffer size */
#define DVBUFSIZ 0x10000      /* 256k buffer for dumpvars output */
#define MAX_ERRORS 32         /* error limit in one run */
#define MAXEMSGNUM 4000       /* highest possible any type error num */

#define MY_FOPEN_MAX 1024     /* guess at OS process max open stdio.h wrong */

typedef unsigned long word32;
typedef long sword32;
typedef int int32;
typedef unsigned long long word64;
typedef long long sword64;
typedef unsigned char byte;
typedef unsigned short hword;
typedef short hsword;
typedef int32 i_tev_ndx;
/* needed to allow signals to use normal decls */
typedef int32 sighandler();

#ifndef __sparc
/* BEWARE - assuming all non sparc systems define BYTE_ORDER and endian.h */
/* to PORT change for your system */ 
# ifdef __APPLE__
#  include <sys/types.h>
# else
#  ifdef __CYGWIN32__
#   include <sys/param.h>
#  else
#   ifdef __FreeBSD__
#    include <sys/endian.h>
#   else
#    include <endian.h>
#   endif
#  endif
# endif

typedef union {
 word64 w64v; 
#if (BYTE_ORDER == BIG_ENDIAN)
 struct { word32 high; word32 low; } w_u;
#else
 struct { word32 low; word32 high; } w_u;
#endif
} w64_u;
#else
#include <sys/isa_defs.h>

typedef union {
 word64 w64v; 
#if defined(_BIG_ENDIAN)
 struct { word32 high; word32 low; } w_u;
#else
 struct { word32 low; word32 high; } w_u;
#endif
} w64_u;
#endif

#define TRUE 1
#define FALSE 0
#define WBITS 32
#define LWBITS 64
#define WRDBYTES 4
#define MAXWSWRDS ((MAXEMSGNUM + WBITS)/WBITS)
#define TIMEBITS 64
#define WORDMASK_ULL 0xffffffffULL
#define ALL1W 0xffffffff
#define ALL1HW 0xffff
#define SHORTBASE (1 << (WBITS/2))

/* these values are IEEE */
#define REALBITS 32           /* 64 bit double - but no no b part */
#define LG2_DIV_LG10 0.3010299956639812
#define EPSILON 2.2204460492503131E-16  /* float.h dbl == diff. amount */
#define _DEXPLEN 11           /* double exp. len (not incl. sign) */

/* number base constants */
#define BHEX 0
#define BBIN 1
#define BOCT 2
#define BDEC 3
#define BDBLE 4               /* special base for printing */
#define BNONE 5               /* indicator for no base override */
/* debugger only bases */
#define BCHAR 6               /* force treatment as char */
#define BSTR 7                /* froce treatment as string */

/* net representation forms */
#define NX_CT 0
#define NX_ARR 1
#define NX_DWIR 2
#define NX_WIR 3
#define NX_SCALWIR 4          /* for scalar empty range field */

/* net storage representation forms */
#define SR_VEC 0
#define SR_SVEC 1
#define SR_SCAL 2
#define SR_SSCAL 3
#define SR_ARRAY 4
#define SR_PVEC 5             /* now only used for gates */
#define SR_PXPR 6             /* for parameter src. normal rhs expr */ 
#define SR_PISXPR 7           /* for # and defparams, per inst. array of x */
#define SR_PNUM 8             /* after fix up simple parameter const. */
#define SR_PISNUM 9           /* after fix up IS parameter const. */


/* strength types */
#define LOW_STREN 1
#define HIGH_STREN 2
#define CAP_STREN 3

/* strength values - must fit in 3 bits */
/* notice semantics requires this exact strength assignment for 0 to 7 */
#define ST_HIGHZ 0
#define ST_SMALL 1
#define ST_MEDIUM 2
#define ST_WEAK 3
#define ST_LARGE 4
#define ST_PULL 5
#define ST_STRONG 6
#define ST_SUPPLY 7

/* capacitor sizes */
#define CAP_NONE 0
#define CAP_SMALL 1 
#define CAP_MED 2
#define CAP_LARGE 3

/* 6 bit stren only constants */
#define NO_STREN 8            /* never a strength - indicator only */
#define ST_STRVAL 0x36        /* (strong0,strong1) 110110 */

/* strength and value constants */
#define ST_STRONGX ((ST_STRVAL << 2) | 3)
#define ST_HIZ 0x02           /* <hiZ:hiZ>=z - 00000010 */
#define ST_PULL0 0xb4         /* <Pu:Pu>=0 - 10110100 */
#define ST_PULL1 0xb5         /* <Pu:Pu>=1 - 10110101 */
#define ST_SUPPLY0 0xfc       /* <Su:Su>=0 - 11111100 */ 
#define ST_SUPPLY1 0xfd       /* <Su:Su>=1 - 11111101 */ 

/* udp types */
#define U_COMB 0
#define U_LEVEL 1
#define U_EDGE 2
#define NO_VAL 0x7f           /* misc. empty for byte no edge, no val etc. */

/* udp edge values (? and b legal in edges) */
#define UV_0 0
#define UV_1 1
#define UV_X 3
#define UV_Q 4
#define UV_B 5

/* net pin connection types */
#define NP_GATE 0
#define NP_CONTA 1            /* connects to lhs or rhs of cont. assign */
#define NP_PB_ICONN 2         /* per bit form of iconn type - now ld/drv */
#define NP_ICONN 3            /* for load down assign, driver to up iconn */
#define NP_PB_MDPRT 4         /* per bit mdprt form both drv/ld now */
#define NP_MDPRT 5            /* for drvr down port assgn, load up to iconn */
/* notice last of in src npps must be mdprt */
#define NP_MIPD_NCHG 6        /* for MIPD/interconnect, conn net(s) nchg */
#define NP_TCHG 7             /* specify time chg record subtyp determines */
#define NP_TRANIF 8           /* third tranif port enable is normal rhs */
#define NP_PULL 9             /* constant strength pullup or pulldown drv. */
/* PLI only not seen by vpi_ routines */
#define NP_TFRWARG 10         /* driver is tf_ task/func is_rw driver */
#define NP_VPIPUTV 11         /* driver is vpi_ put value wire driver */
/* removed from net drvs after tran channels built - drivers only */
#define NP_TRAN 12            /* driver/load for special tran/tranif gate */ 
#define NP_BIDICONN 13 
#define NP_BIDMDPRT 14 

/* port direction change net driver states - pre added np list states */
#define DRVR_NONE 0           /* net not driven */
#define DRVR_NON_PORT 1       /* inout port or gate/conta */
#define DRVR_ICONN 2          /* one up iconn inst conn output driver */ 
#define DRVR_MDPRT 3          /* one down mod port input driver */ 

/* parmnplst net pin types - disjoint32 from sim time npps */
#define PNP_GATEDEL 0
#define PNP_CONTADEL 1
#define PNP_NETDEL 2
#define PNP_PROCDCTRL 3
#define PNP_PATHDEL 4
#define PNP_TCHKP1 5
#define PNP_TCHKP2 6

/* change record subtypes */
#define NPCHG_NONE 0
#define NPCHG_TCSTART 1
#define NPCHG_TCCHK 2
#define NPCHG_PTHSRC 3

/* kinds of net pin global processing - npgru interpretation */
#define XNP_LOC 0
#define XNP_DOWNXMR 1
#define XNP_RTXMR 2
#define XNP_UPXMR 3

/* kinds of net pin processing */
#define NP_PROC_INMOD 0
#define NP_PROC_GREF 1
#define NP_PROC_FILT 2

/* left hand expression types */
#define LHS_DECL 1
#define LHS_PROC 2

/* tokens types - must < 255 */
#define TOK_NONE 0
#define ID 1
#define BADOBJ 2
#define NUMBER 3
#define ISNUMBER 4            /* array of constants - cur_itp selects */
#define REALNUM 5
#define ISREALNUM 6
#define LITSTR 7              /* token returned for literal str by scanner */
#define OPEMPTY 8             /* unc. expr place holder in expr. list */ 
#define UNCONNPULL 9          /* unc. inst. input port `unconndrive val. */
#define RMADDR 10             /* special readmem address token */

/* punctuation */
#define SEMI 12
#define COMMA 13
#define COLON 14
#define SHARP 15
#define LPAR 16
#define RPAR 17
#define LSB 18                /* [ (also bit select operator) */
#define RSB 19
#define LCB 20                /* { */
#define RCB 21
#define DOT 22
#define AT 23

/* special unary */
#define CAUSE 24              /* -> not an expression operator */
#define EQ 25                 /* = (assignment but not an expr. lang.) */

/* unary only operators - real 0.0 is F for logical */
#define BITNOT 26             /* ~ */
#define NOT 27                /* ! (logical) */
/* real unary only (0.0 if F else T) */
#define REALNOT 28 

/* both unary and binary */
#define REDXNOR 29            /* must ^~ - ~^ is 2 unaries */
#define PLUS 30               /* + */
#define MINUS 31              /* - */
#define BITREDAND 32          /* bin & (bit) - un red. and */
#define BITREDOR 33           /* bin | (bit) - un red. or */
#define BITREDXOR 34          /* bin ^ (bit) - un red. xor */
/* both real - notice type must be in operator */
/* notice this should have at 5? number */
#define REALMINUS 35

/* binary operators */
#define TIMES 36              /* * */
#define DIV 37                /* / */
#define MOD 38                /* % */
#define RELGE 39              /* >= */
#define RELGT 40              /* > */
#define RELLE 41              /* <= (also non block cont. assign token) */
#define RELLT 42              /* < */
#define RELCEQ 43             /* === */
#define RELEQ 44              /* == */
#define RELCNEQ 45            /* !== */
#define RELNEQ 46             /* != */
#define BOOLAND 47            /* && */
#define BOOLOR 48             /* || */
#define SHIFTL 49             /* << */
#define ASHIFTL 50            /* <<< - differenet operator but same as << */
#define SHIFTR 51             /* >> */
#define ASHIFTR 52            /* >>> */
#define FPTHCON 53            /* *> (full path conn spfy. op.) */
#define PPTHCON 54            /* => (parallel path conn spfy. op.) */
#define TCHKEVAND 55          /* &&& (timing check conditional event op.) */
/* real binary */
#define REALPLUS 56
#define REALTIMES 57
#define REALDIV 58
#define REALRELGT 59
#define REALRELGE 60
#define REALRELLT 61
#define REALRELLE 62
#define REALRELEQ 63
#define REALRELNEQ 64
#define REALBOOLAND 65
#define REALBOOLOR 66
/* notice logical non bit-wise and non-reduction 0.0 is F */

/* special ops */
/* special expression operators that do not correspond to read tokens */
#define QUEST 67              /* binary ? part of ?: */
#define QCOL 68               /* : part of ?: */
#define PARTSEL 69            /* part select [ */
#define CATCOM 70             /* concatenate comma (really list indicator) */
#define CATREP 71             /* concatenate repeat form */
#define FCALL 72              /* function call */
#define FCCOM 73              /* func/task call , (really list indicator) */
#define OPEVOR 74             /* event control or */
#define OPEVCOMMAOR 75        /* alternative to event ctrl or - sims same */
#define OPPOSEDGE 76          /* posedge in delay ctrl expr. */
#define OPNEGEDGE 77          /* negedge in delay ctrl expr. */
/* real specials */
#define REALREALQUEST 78      /* binary ?  part of ?: - all reals */ 
#define REALREGQUEST 79       /* bin ? part of ?: - real cond : part reg */ 
#define REGREALQCOL 80        /* binary : part of ?: - normal cond : real */

/* global expression componnents */ 
#define GLBREF 81             /* this is sort of ID and sort of expr. */    
#define GLBPTH 82             /* this is global path as XMRID expr (list) */
#define XMRID 83              /* ID node that is part of xmr (no sy) */
#define XMRCOM 84

/* special indicators */
#define UNDEF 85              /* toktyp for no pushed back token */
#define TEOF 86

/* notice unused token no. gap here for addition operators ? */

/* these can all appear outside modules - some can appear inside */
/* back quote is ascii 96 (0x60) just before lower case (after upper case) */
#define CDIR_ACCEL 100
#define CDIR_AEXPVECNETS 101
#define CDIR_CELLDEF 102
#define CDIR_DEFINE 103
#define CDIR_DFLNTYP 104
#define CDIR_ECELLDEF 105
#define CDIR_ELSE 106
#define CDIR_ENDIF 107
#define CDIR_ENDPROTECT 108
#define CDIR_ENDPROTECTED 109
#define CDIR_XPNDVNETS 110
#define CDIR_IFDEF 111
#define CDIR_IFNDEF 112
#define CDIR_INCLUDE 113
#define CDIR_NOACCEL 114
#define CDIR_NOXPNDVNETS 115
#define CDIR_NOREMGATENAMES 116
#define CDIR_NOREMNETNAMES 117
#define CDIR_NOUNCONNDRIVE 118
#define CDIR_PROTECT 119
#define CDIR_PROTECTED 120
#define CDIR_REMGATESNAMES 121
#define CDIR_REMNETNAMES 122
#define CDIR_RESETALL 123
#define CDIR_TIMESCALE 124
#define CDIR_UNCONNDRIVE 125
#define CDIR_UNDEF 126
#define CDIR_DFLTDECAYTIME 127
#define CDIR_DFLTTRIREGSTREN 128
#define CDIR_DELMODEDIST 129
#define CDIR_DELMODEPATH 130
#define CDIR_DELMODEUNIT 131
#define CDIR_LANG 132 
#define CDIR_DELMODEZERO 134

#define CDIR_TOKEN_START CDIR_ACCEL
#define CDIR_TOKEN_END CDIR_DELMODEZERO

/* notice 19 unused token no. gap here ? */
#define ALWAYS 140
#define ASSIGN 141
#define Begin 142
#define CASE 143
#define CASEX 144
#define CASEZ 145
#define DEASSIGN 146
#define DEFAULT 147
#define DEFPARAM 148
#define DISABLE 149
#define EDGE 150
#define ELSE 151
#define END 152
#define ENDCASE 153
#define ENDFUNCTION 154
#define ENDGENERATE 155
#define ENDMODULE 156
#define ENDPRIMITIVE 157
#define ENDSPECIFY 158
#define ENDTABLE 159
#define ENDTASK 160
#define EVENT 161
#define FOR 162
#define FORCE 163
#define FOREVER 164
#define FORK 165
#define FUNCTION 166
#define GENERATE 167
#define HIGHZ0 168
#define HIGHZ1 169
#define IF 170
#define IFNONE 171
#define INITial 172
#define INOUT 173
#define INPUT 174
#define INTEGER 175
#define JOIN 176
#define LARGE 177
#define LOCALPARAM 178
#define MACROMODULE 179
#define MEDIUM 180
#define MODULE 181
#define NEGEDGE 182
#define OUTPUT 183
#define PARAMETER 184
#define POSEDGE 185
#define PRIMITIVE 186
#define PULL0 187
#define PULL1 188
#define REAL 189
#define REALTIME 190
#define REG 191
#define RELEASE 192
#define REPEAT 193
#define SCALARED 194
#define SIGNED 195
#define SPECIFY 196
#define SPECPARAM 197
#define SMALL 198
#define Strength 199
#define STRONG0 200
#define STRONG1 201
#define SUPPLY0 202
#define SUPPLY1 203
#define TABLE 204
#define TASK 205
#define TIME 206
#define TRI 207
#define TRI0 208
#define TRI1 209
#define TRIAND 210
#define TRIOR 211
#define TRIREG 212
#define VECTORED 213
#define WAIT 214
#define WAND 215
#define WEAK0 216
#define WEAK1 217
#define WHILE 218
#define WIRE 219
#define WOR 220

/* gate nums (not tokens) for sim - now separate gatid range */
/* now for debugging do not use low numbers */
#define G_ASSIGN 10 
#define G_BITREDAND 11
#define G_BUF 12
#define G_BUFIF0 13
#define G_BUFIF1 14
#define G_CMOS 15
#define G_NAND 16
#define G_NMOS 17
#define G_NOR 18
#define G_NOT 19
#define G_NOTIF0 20
#define G_NOTIF1 21
#define G_BITREDOR 22
#define G_PMOS 23
#define G_PULLDOWN 24
#define G_PULLUP 25
#define G_RCMOS 26
#define G_RNMOS 27
#define G_RPMOS 28
#define G_RTRAN 29
#define G_RTRANIF0 30
#define G_RTRANIF1 31
#define G_TRAN 32
#define G_TRANIF0 33
#define G_TRANIF1 34
#define G_BITREDXOR 35
#define G_REDXNOR 36

#define LAST_GSYM G_REDXNOR

/* gate classes */ 
#define GC_LOGIC 0
#define GC_BUFIF 1
#define GC_MOS 2
#define GC_CMOS 3
#define GC_TRAN 4
#define GC_TRANIF 5
#define GC_UDP 6
#define GC_PULL 7

/* region separation constants */
#define FIRSTKEY CDIR_ACCEL
#define BKEYS FIRSTKEY
/* this is last real op. part select's etc. come after */
#define LASTOP TEOF

/* unconnected pull directive OPEMPTY values */
#define NO_UNCPULL 0
#define UNCPULL0 1
#define UNCPULL1 2

/* net types (ntyp field) - meaningful when iotyp is NON_IO */
#define N_WIRE 0
#define N_TRI 1
#define N_TRI0 2
#define N_TRI1 3
#define N_TRIAND 4
#define N_TRIOR 5
#define N_TRIREG 6
#define N_WA 7
#define N_WO 8
#define N_SUPPLY0 9
#define N_SUPPLY1 10
#define N_REG 11
#define N_INT 12
#define N_TIME 13
#define N_REAL 14
#define N_EVENT 15            /* needs to be event so -d decls in order */

/* beware - number ranges used so all reg types must come after all wires */ 
#define NONWIRE_ST N_REG

/* the i/o net and port types - separate from net type */
#define IO_IN 0
#define IO_OUT 1
#define IO_BID 2
#define IO_UNKN 3
#define NON_IO 4

/* sync to location classes */
#define SYNC_FLEVEL 0
#define SYNC_MODLEVEL 1
#define SYNC_STMT 2
#define SYNC_SPECITEM 3
#define SYNC_UDPLEVEL 4
#define SYNC_TARG 5

/* scalared/vectored splitting states */
#define SPLT_DFLT 0
#define SPLT_SCAL 1
#define SPLT_VECT 2

/* min/typ/max delay selection values */
#define DEL_MIN 1
#define DEL_TYP 2
#define DEL_MAX 3

/* symbol types - determines net list element */
/* front end only symbols */
#define SYM_UNKN 0
#define SYM_M 1               /* module type name (not in pli) */
#define SYM_STSK 2            /* built in system task name */
#define SYM_SF 3              /* built in system function name */
#define SYM_DEF 4             /* preprocessor `define sym, separate table */

/* in module (real pli) objects */
#define SYM_I 5               /* inst. name in module */
#define SYM_TSK 6             /* task name */
#define SYM_F 7               /* function name */
#define SYM_LB 8              /* labeled block */
#define SYM_PRIM 9            /* built in primitive */
#define SYM_UDP 10            /* udp definition name */
#define SYM_N 11              /* wire, regs, events and I/O ports (nets) */
#define SYM_CA 12             /* symbol for conta */
#define SYM_TCHK 13           /* symbol for time check */
#define SYM_PTH 14            /* symbol for delay path */

/* pli only object (handle) types - no front end symbol */
#define SYM_IMPATH 19         /* inter module (inst.) path not in ovi ? */
#define SYM_MPATH 20          /* any intra module path */
#define SYM_PRIMPATH 21       /* path between primitives - not in ovi ? */
#define SYM_WIREPATH 22       /* another kind of non ovi path ? */
#define SYM_TERM 23           /* primitive terminal dummy */
#define SYM_MPTERM 24         /* terminal of mod path - only acc handle */
#define SYM_PORT 25           /* module port dummy */
#define SYM_BIT 26            /* bit of expanded net dummy */
#define SYM_PBIT 27           /* bit of expanded port dummy */

/* tree balance constants */
#define BLEFT 0
#define BEVEN 1
#define BRIGHT 2

/* btree (timing queue) constants */
#define BTNORM 0
#define BTFRNGE 1

/* statement types */
#define S_NULL 0              /* ; by itself */
#define S_PROCA 1
#define S_NBPROCA 2
#define S_RHSDEPROCA 3
#define S_IF 4
#define S_CASE 5
#define S_FOR 6
#define S_FOREVER 7
#define S_REPEAT 8
#define S_WHILE 9             /* execept forever only sim time loop */
#define S_WAIT 10
#define S_DELCTRL 11
#define S_NAMBLK 12
#define S_UNBLK 13
#define S_UNFJ 14             /* this is for unnamed block fork-join only */
#define S_TSKCALL 15
#define S_QCONTA 16
#define S_QCONTDEA 17
#define S_CAUSE 18            /* CAUSE (->) <event> */
#define S_DSABLE 19           /* disable <task or block> */
#define S_STNONE 20           /* thing inside empty block */
/* special invisible simulation control statements */
#define S_REPSETUP 21         /* set up repeat count - st_u empty */
#define S_REPDCSETUP 22       /* event repeat form dctrl setup */ 
#define S_GOTO 23             /* goto statement for loops */
#define S_FORASSGN 24         /* special added (before) for assign */
#define S_BRKPT 25            /* for setting break point at statement */


/* system function types */
#define SYSF_UNKNOWN 0
#define SYSF_BUILTIN 1
#define SYSF_TF 2
#define SYSF_VPI 3

/* break point types */
#define BP_UNKN 0
#define BP_INST 1 
#define BP_TYPE 2

/* interactive entry reasons */
#define IAER_UNKN 0
#define IAER_BRKPT 1
#define IAER_STEP 2
#define IAER_STOP 3
#define IAER_CTRLC 4

/* event control types */
#define DC_NONE 0
#define DC_EVENT 1
#define DC_DELAY 2
#define DC_RHSEVENT 3
#define DC_RHSDELAY 4
#define DC_WAITEVENT 5

/* statement dump new line control constants */
#define NONL 1
#define NL 2

/* Verilog operator types */
#define NOTANOP 0
#define UNOP 1
#define BINOP 2
#define BOTHOP 3
#define SPECOP 4
#define RUNOP 5
#define RBINOP 6
#define RBOTHOP 7

/* indicator if operand legal for reals */
#define REALOP TRUE
#define NOTREALOP FALSE
#define PTHOP TRUE
#define NOTPTHOP FALSE

/* operater width result pattern */
#define WIDNONE 0             /* no width result */
#define WIDONE 1              /* result width 1 (for logicals) */
#define WIDENONE 2            /* result one, but widen opands to wides */
#define WIDLEFT 3             /* left operand width of binary */
#define WIDMAX 4              /* max. of 2 operands */
#define WIDSUM 5              /* sum of operand width for concats */
#define WIDSELF 6             /* width self determining (like [] or un ~/-) */

/* specify section constants */
#define PTH_PAR 0             /* => */
#define PTH_FULL 1            /* *> */
#define PTH_NONE 2

#define POLAR_NONE 0
#define POLAR_PLUS 1
#define POLAR_MINUS 2

#define TCHK_SETUP 0
#define TCHK_HOLD 1
#define TCHK_WIDTH 2
#define TCHK_PERIOD 3
#define TCHK_SKEW 4
#define TCHK_RECOVERY 5
#define TCHK_NOCHANGE 6
#define TCHK_SETUPHOLD 7
#define TCHK_FULLSKEW 8
#define TCHK_RECREM 9
#define TCHK_REMOVAL 10
#define TCHK_TIMESKEW 11

/* for [edge ...] form allows list so multiple bits possible */
/* values must match acc_ pli values */
#define NOEDGE 0
#define EDGE01 0x1
#define EDGE10 0x2
#define EDGE0X 0x4
#define EDGEX1 0x8
#define EDGE1X 0x10
#define EDGEX0 0x20
/* these are or of relevants */
#define E_POSEDGE 0xd
#define E_NEGEDGE 0x32

/* delay union form for all delays after simulation preparation */
/* notice for non IS form no byte and halfword packed forms */
#define DT_NONE 0
#define DT_1V 1
#define DT_IS1V  2            /* all is forms have 1,2 and 8 byte forms */
#define DT_IS1V1 3
#define DT_IS1V2 4
#define DT_4V 5               /* 4v forms only for primtives not paths */ 
#define DT_IS4V 6
#define DT_IS4V1 7
#define DT_IS4V2 8
#define DT_16V 9              /* except for 1v all spec. delays need 15v */
#define DT_IS16V 10
#define DT_IS16V1 11
#define DT_IS16V2 12
#define DT_1X 13
#define DT_4X 14
#define DT_PTHDST 15
#define DT_CMPLST 16

/* for checking 0 and path delays, kinds of bad delays */
#define DGOOD 1
#define DBAD_NONE 2
#define DBAD_EXPR 3
#define DBAD_0 4
#define DBAD_MAYBE0 5

/* timing event types - what tested by not starting at 0 ? */
#define TE_THRD 2
#define TE_G 3
#define TE_CA 4
#define TE_WIRE 5
#define TE_BIDPATH 6
#define TE_MIPD_NCHG 7
#define TE_NBPA 8
#define TE_TFSETDEL 9
#define TE_SYNC 10
#define TE_TFPUTPDEL 11
#define TE_VPIPUTVDEL 12
#define TE_VPIDRVDEL 13
#define TE_VPICBDEL 14
#define TE_UNKN 15

#define DFLT_LOGFNAM "verilog.log"
#define DFLTDVFNAM "verilog.dump"
#define DFLTKEYFNAM "verilog.key"

/* dumpvars action state */
#define DVST_NOTSETUP 0
#define DVST_DUMPING 1
#define DVST_NOTDUMPING 2
#define DVST_OVERLIMIT 3

/* baseline dump types for dumpvars */
#define DMPV_CHGONLY 0
#define DMPV_DMPALL 1
#define DMPV_DUMPX 2

/* delay control event filter processing types */
#define DCE_RNG_INST 0
#define DCE_INST 1
#define DCE_RNG_MONIT 2
#define DCE_MONIT 3
#define DCE_NONE 4
#define DCE_QCAF 5
#define DCE_RNG_QCAF 6
#define DCE_CBF 7
#define DCE_RNG_CBF 8
#define DCE_CBR 9
#define DCE_RNG_CBR 10
/* BEWARE these always require prevval - must be higher number than above */
#define DCE_PVC 11
#define DCE_RNG_PVC 12
#define DCE_CBVC 13
#define DCE_RNG_CBVC 14

#define ST_ND_PREVVAL DCE_PVC

/* slotend action masks - multiple can be on at time */
#define SE_TCHK_VIOLATION 1
#define SE_MONIT_CHG 0x2      /* definite chg including first time */
#define SE_MONIT_TRIGGER 0x4  /* monit chg where val saved (chk at end) */
/* FIXME - why is this never referenced */ 
#define SE_FMONIT_TRIGGER 0x8
#define SE_STROBE 0x10
#define SE_DUMPVARS 0x20      /* ON => some dumpvars action required */ 
#define SE_DUMPALL 0x40       /* need slot end dump all */
#define SE_DUMPON  0x800      /* need to turn dumping on at slot end */
#define SE_DUMPOFF 0x100      /* need to turn dumping off at slot end */
#define SE_TFROSYNC 0x200     /* have list of rosync events to process */
#define SE_VPIROSYNC 0x400    /* have list of vpi rw sync to process */

/* selectors for type of tran channel routine to use (low 3 bits) */ 
#define TRPROC_UNKN 0
#define TRPROC_BID 1
#define TRPROC_STBID 2
#define TRPROC_STWTYPBID 3
#define TRPROC_TRAN 4

/* type of gate acceleration - for selecting routine */
#define ACC_NONE 0
#define ACC_STD 1
#define ACC_BUFNOT 2
#define ACC_STIBUFNOT 3
#define ACC_4IGATE 4
#define ACC_ST4IGATE 5

/* internal vpi_ call back type classes */
#define CB_NONE 0
#define CB_VALCHG 1 
#define CB_DELAY 2
#define CB_ACTION 3

/* simulation (run) progress states */
#define SS_COMP 1
#define SS_LOAD 2
#define SS_RESET 3
#define SS_SIM 4

/* Cver supported error severity levels */
#define INFORM 0
#define WARN 1
#define ERROR 2
#define FATAL 3

/* per inst. masks for net nchg action byte array */
#define NCHG_DMPVARNOW 0x1    /* dumpvaring of this var inst on/off */
#define NCHG_DMPVNOTCHGED 0x2 /* var inst. not chged during current time */
#define NCHG_DMPVARED 0x4     /* var inst. dumpvared (setup) */
#define NCHG_ALL_CHGED 0x08   /* all var inst. bits chged (also if no lds) */

/* AIV 09/05/03 consts for new fileio OS file descriptor streams support */
#define FIO_MSB 0x80000000
#define FIO_FD 0x7fffffff
#define FIO_STREAM_ST 3       /* first usable ver fio fd open file number */
#define SE_VPIROSYNC 0x400    /* have list of vpi rw sync to process */

/* AIV 12/02/03 constants for cfg get token */
/* FIXME ??? - needed here because cfg get tok in v ms but used in cver? */
#define CFG_UNKNOWN 0
#define CFG_ID 1
#define CFG_COMMA 2
#define CFG_SEMI 3
#define CFG_EOF 4
#define CFG_LIBRARY 5
#define CFG_CFG 6
#define CFG_INCLUDE 7
#define CFG_DESIGN 8
#define CFG_LIBLIST 9
#define CFG_INSTANCE 10
#define CFG_CELL 11
#define CFG_USE 12
#define CFG_ENDCFG 13
#define CFG_DEFAULT 14

/* SJM 07/31/01 - need system type def. include files for 64 bit types */
#include <time.h>

/* various forward references */
struct sy_t;
struct st_t;
struct mod_pin_t;
struct thread_t;
struct tchk_t;
struct tchg_t;
struct fmonlst_t;

/* data structure for keywords and command line/file options */
struct namlst_t {
 int32 namid;
 char *lnam;
};

/* SJM 06/03/01 - to prepare for 64 bits union to avoid int32 to ptr cast */
/* SJM 10/12/04 - can't use p because need index into contab not addr */
union intptr_u {
 int32 i;
 int32 xvi;
};

/* SJM 08/22/01 - must be at top for new expr node union */
union pck_u {
 word32 *wp;
 hword *hwp;
 byte *bp;
 double *dp;
};

/* data struct for help message line tables - hnum is namlst_t namid */
struct hlplst_t {
 /* LOOKATME - why is this unused */
 int32 hnamid;
 char **hmsgtab;
};

struct sdfnamlst_t { 
 char *fnam;
 char *scopnam;
 char *optfnam;
 int32 opt_slcnt;
 struct sdfnamlst_t *sdfnamnxt;
};

struct optlst_t {
 unsigned optfnam_ind : 16;
 unsigned is_bmark : 1;       /* special -f markers for vpi_ */         
 unsigned is_emark : 1;
 unsigned is_argv : 1;
 unsigned argv_done : 1;
 int32 optlin_cnt;
 int32 optnum;
 int32 optlev;
 char *opt;
 char **dargv;
 struct optlst_t *optlnxt;
};

/* data structure for input file/`define/`include stack */
/* could be union but not too many ? */
struct vinstk_t {
 int32 vilin_cnt;
 word32 vifnam_ind;
 FILE *vi_s;
 char *vichp_beg;             /* beginning of macro string */  
 int32 vichplen;              /* if not -1, len + 1 must be freed */
 char *vichp;                 /* macro string current char */
};

/* P1364 2001 cfg include structs */
/* list of map files - uses map.lib in CWD if none from options */
struct mapfiles_t {
 char *mapfnam;
 struct mapfiles_t *mapfnxt;
};

/* order list of config element names */
struct cfgnamlst_t {
 char *nam;
 struct cfgnamlst_t *cnlnxt;
};

/* describes component of path during wild card expansion */
struct xpndfile_t {
 char *fpat;                  /* the original pattern per dir, split by '/' */
 int32 nmatch;                /* the number of matched files */
 int32 level;                 /* current directory level */
 unsigned wildcard: 2;        /* T=> if it contains a wild char, *, ? or ... */
 unsigned incall : 1;         /* include the entire dir, ends in '/' */
 struct xpndfile_t *xpfnxt;   /* next part of original pattern */
};

/* record for library element (after expand, all wild cards removed) */
struct libel_t {
 unsigned lbelsrc_rd : 1;     /* T => expanded lib file name src all read */
 unsigned expanded : 1;       /* T => file has been expanded  */ 
 char *lbefnam; 
 struct libcellndx_t **lbcelndx; /* byte offset of cell names for seeking */
 struct symtab_t *lbel_sytab; /* symbol table of cells in file */
 struct libel_t *lbenxt;
};

/* cfg library record - sort of corresponds to -v/y data record */ 
struct cfglib_t {
 unsigned lbsrc_rd : 1;       /* T => lib src files already read */
 unsigned sym_added : 1;      /* T => sym table has been read */
 char *lbname;                /* library name */ 
 struct libel_t *lbels;       /* list of library file path spec elements */
 struct cfglib_t *lbnxt;      /* pointer to the next library */
 char *cfglb_fnam;            /* location of library in src needed */
 int32 cfglb_lno;
 struct cfg_t *cfgnxt;
};

/* config body rule record */
struct cfgrule_t {
 unsigned rultyp : 8;         /* type - cfg symbol number */
 unsigned use_rule_cfg : 1;   /* T => use clause config form */ 
 unsigned matched : 1;        /* T => rule was bound at least once */ 
 unsigned is_use : 1;         /* T => 'use' else 'liblist' */ 
 char *objnam;                /* inst clause: [XMR inst path] */ 
 char *libnam;                /* cell clause: <lib name>.[cell name] */
 char *rul_use_libnam;        /* use <lib name.>[cell name][:config] */
 char *rul_use_celnam;
 char **inam_comptab;         /* table of inst clause XMR components */
 int32 inam_comp_lasti;       /* number of components in XMR inst path */

 struct cfgnamlst_t *rul_libs;/* liblist clause - just list of libraries */
 int32 rul_lno;
 struct cfgrule_t *rulnxt;
};

/* record for list of config design cell identifiers */
struct cfgdes_t {
 char *deslbnam;          /* name of config design library */
 struct cfglib_t *deslbp; /* ptr to config design library */
 char *topmodnam;         /* name of top level cell (type) */
 struct cfgdes_t *desnxt;  
};

/* record for each cfg block */
struct cfg_t {
 char *cfgnam;                /* name of the cfg */
 struct cfgdes_t *cfgdeslist; /* list of config design [lib].[cell]s */

 /* SJM 12/11/03 - notice preserving exact order of rules critical */
 struct cfgrule_t *cfgrules;  /* ordered list of config rules */
 struct cfgrule_t *cfgdflt;   /* default lib list if rules find no matches */
 char *cfg_fnam;
 int32 cfg_lno;
 struct cfg_t *cfgnxt;
};

/* for debugger include file and last included place */
struct incloc_t {
 int32 inc_fnind;             /* in_fils index of included file */
 int32 incfrom_fnind;         /* in_fils index file included from */
 int32 incfrom_lcnt;          /* lin cnt where included */ 
 struct incloc_t *inclocnxt;
};

/* doubly linked list for undefined modules */
struct undef_t {
 struct sy_t *msyp;
 int32 dfi;
 char *modnam;
 struct undef_t *undefprev;
 struct undef_t *undefnxt;
};

/* struct for files in ylib - to avoid opening if not needed */
struct mydir_t {
 unsigned ydirfnam_ind : 16;
 unsigned ylbxi : 16;         /* ndx in lbexts (-1 if none), -2 no match */
 char *dirfnam;
};

/* -y and -v file list */
struct vylib_t {
 unsigned vyfnam_ind : 16;    /* for rescanning file ind or num dir files */
 unsigned vytyp : 8;          /* 'v' for -v andd 'y' for -y */
 union {
  char *vyfnam;               /* file name */ 
  char *vydirpth;             /* for -y directory, directory path */
 } vyu;
 struct mydir_t *yfiles;      /* for -y header of file list */
 struct vylib_t *vynxt;
};

/* struct for multi-channel descriptors - need to save file name */ 
struct mcchan_t { 
 FILE *mc_s; 
 char *mc_fnam;
};

/* ver fd number record for new 2001 P1364 file io package */
struct fiofd_t {
 unsigned fd_error : 1;      /* error indicator */
 char *fd_name;              /* name of stdio file */
 FILE *fd_s;                 /* corresponding OS FILE * descriptor */ 
};

/* debugger source file cache line start location rec */
struct filpos_t {
 int32 nlines;
 int32 *lpostab;
};

/* formal macro argument list */
struct macarg_t {
 char *macargnam;
 struct macarg_t *macargnxt;
};

/* macro expansion templace list */
struct macexp_t {
 char *leading_str;
 int32 leadlen;
 int32 ins_argno;
 struct macexp_t *macexpnxt;
};

/* arg macro record */ 
struct amac_t { 
 int32 num_formal_args;
 struct macexp_t *amxp;
};

/* operator info struct for predefined table (index by op. symbol number) */
/* Verilog ops always associate left */
struct opinfo_t {
 unsigned opclass : 8;
 unsigned realop : 8;
 unsigned pthexpop: 8;        /* T => op legal in SDPD condional expr. */
 unsigned reswid : 8;
 char *opnam;
};

/* value in q for $q_ system tasks */
struct q_val_t {
 int32 job_id;
 int32 inform_id;
 word64 enter_tim;
};

/* q header record - linked list of all queues */
struct q_hdr_t {
 unsigned q_fifo : 1;       /* T => fifo, F => lifo */
 int32 q_id;                /* user passed ID */
 int32 q_hdr;               /* index of current head */
 int32 q_tail;              /* index of current tail */
 int32 q_maxlen;            /* maximum length of queue */
 int32 q_size;              /* need to store size for q_exam */
 struct q_val_t *qarr;      /* table contain queue */
 word64 q_minwait;          /* shortest wait (min) of all els. ever in Q */
 int32 q_maxsize;           /* for entire run, max size of q */
 struct q_hdr_t *qhdrnxt; 
};

/* history suspended statement control record */
struct hctrl_t {
 struct st_t *hc_stp;     /* initial style statement list */
 struct itree_t *hc_itp;  /* itree loc. runs in if reenabled */
 struct thread_t *hc_thp; /* associated thread */ 
 int32 hc_lini;           /* line loc. in history of statement */ 
 int32 hc_ifi; 
 struct hctrl_t *hc_nxt;  /* list next for non history traversing */
 struct hctrl_t *hc_prev; /* and previous */
 struct iahist_t *hc_iahp;/* ptr to history element if history on */
 struct dceauxlst_t *hc_dcelst; /* aux. list of per inst iact dces to free */ 
 struct gref_t *hc_grtab; /* table of any grefs used in this statement */ 
 int32 hc_numglbs;        /* size of table */
};

/* interactive history command record */
struct iahist_t { 
 char *iah_lp;            /* history line (may have embedded new lines) */
 struct hctrl_t *iah_hcp; /* parsed suspended stmt control record */
 struct itree_t *iah_itp; /* for non immed. save original itree loc. */
};

/* parallel to expr table during expr collect id name info struct */  
struct expridtab_t {
 char *idnam;
 int32 idfldwid;
 int32 idfnam_ind;
 int32 idlin_cnt;
};

/* structs for expressions */
union l_u {
 struct expr_t *x;
 int32 xi; /* for compile/save changed to indices */
 struct sy_t *sy;
 int32 si; /* for compile/save changed to indices */
};

union r_u {
 struct expr_t *x;
 int32 xvi;                   /* wrd index into constant tab */
 union pck_u iop_ptr;         /* for special malloc iopt node ptr to area */
 struct gref_t *grp;          /* during compile/exec - ptr to gref */
 int32 gri;                   /* for compile/save index */
 char *qnchp;                 /* for local qual. name, pointer to name */ 
};

/* expression size or tf rec union */
union sz_u {
 int32 xclen;
 struct tfrec_t *xfrec;
 void *vpi_sysf_datap;        /* for vpi_ sysf get/put data routine */
};

/* expression tree storage - eventually will fit in 12 bytes */
struct expr_t {
 /* operator token type - leaf node if var or number */
 unsigned optyp : 8;          /* token type of node */
 unsigned has_sign : 1;       /* T => result must be signed */
 unsigned rel_ndssign : 1;    /* T => 1 bit result relational needs sign */
 unsigned is_string : 1;      /* T => constant in src as string (quoted) */
 unsigned unsiznum : 1;       /* T => constant is unsized */
 unsigned ibase : 3;          /* for sized no., base char. */
 unsigned sizdflt : 1;        /* T => '[base] form but no width */
 unsigned is_real : 1;        /* T => expr. value is real number */
 unsigned cnvt_to_real : 1;   /* T => non real operand of real expr */
 unsigned unsgn_widen : 1;    /* T => for cases unsigned widen if needed */
 unsigned consubxpr : 1;      /* T => node will evaluate to number */
 unsigned consub_is : 1;      /* T => node will eval to IS number */
 unsigned folded : 1;         /* T => const. folded (also empty cat rep rem) */
 unsigned getpatlhs : 1;      /* T => expressions is lhs of get pattern */
 unsigned ind_noth0 : 1;      /* T => constant index has been normalized */
 unsigned x_multfi : 1;       /* T => expr has multi fan-in or stren wire */
 unsigned tf_isrw : 1;        /* T => for user tf arg. - assignable (rw) */
 unsigned x_islhs : 1;        /* T => expr. is lhs */
 unsigned locqualnam : 1;     /* T => if printing name is local qualified */
 unsigned lhsx_ndel : 1;      /* T => wire in lhs expr. has net delay */
 unsigned x_stren : 1;        /* T => expr. is strength wire/bs/ps */
 unsigned unc_pull : 2;       /* unc. pull directive value for OPEMPTY */
 union sz_u szu;              /* size of func tf rec. union */ 
 union l_u lu;                /* left sub expr. or symbol union */
 union r_u ru;                /* right sub expr. or ptr. to value union */
};

struct xstk_t {
 int32 xslen;                 /* number of bits required to store */
 int32 xsawlen;               /* word32 length of a/b half of alloced size */
 word32 *ap;                  /* a part of value but must be contigous */
 word32 *bp;                  /* b part of value */
};

struct exprlst_t {
 struct expr_t *xp;
 struct exprlst_t *xpnxt;
};

/* for new ver 2001 var (not wire) decl initialize assign list */
struct varinitlst_t {
 struct sy_t *init_syp;
 struct expr_t *init_xp;
 struct varinitlst_t *varinitnxt;
};

/* struct for storing # delays but not instance pound params */
struct paramlst_t {
 struct expr_t *plxndp;
 struct paramlst_t *pmlnxt;
};

/* struct for storing defparams - list is design wide */
struct dfparam_t {
 unsigned dfp_local : 1;      /* T => defparam is local or in top mod */
 unsigned dfp_rooted : 1;     /* T => rooted lhs gref */
 unsigned dfp_mustsplit : 1;  /* T = mst split rng cat lhs dest. param */
 unsigned dfp_done : 1;       /* during splitting, done with this one */
 unsigned dfp_has_idents : 1; /* T => identical to other rooted defparam */
 unsigned dfp_checked : 1;    /* T => after fix-up check done */
 unsigned dfpfnam_ind : 16;
 int32 dfplin_cnt;
 int32 last_dfpi;             /* last element of path */

 struct expr_t *dfpxlhs;
 struct expr_t *dfpxrhs;
 struct mod_t *in_mdp;        /* module appeared in for non rooted */
 struct dfparam_t *dfpnxt;
 int32 *dfpiis;               /* array of in its indices for path of defp */
 char *gdfpnam;               /* lhs name from gref */
 struct sy_t *targsyp;        /* target symbol from gref */
 struct dfparam_t *idntmastdfp;/* ptr to master if >1 with identical path */
 struct dfparam_t *idntnxt;   /* list from master of identicals path */
 struct dfparam_t *rooted_dfps;/* work for converting downward to rooted */
 struct itree_t *indfp_itp;   /* for rooted, itree place rhs evaled in */ 
 struct task_t *dfptskp;      /* if non NULL, task target declared in */
};

/* per type delay storage - 1 array acess during scheduling */
union del_u {
 word64 *d1v;                 /* 1 constant (non 0) delay value */
 word64 *dis1v;               /* 1 constant (IS form maybe some 0) delay */
 byte *dis1v1;                /* 1 constant IS fits in one byte */
 hword *dis1v2;               /* 1 constant IS fits in two byte */
 word64 *d4v;                 /* 4 value constant delay table */
 word64 *dis4v;               /* 4 constant (IS form maybe some 0) delay */
 byte *dis4v1;                /* 4 constant IS fits in one byte */
 hword *dis4v2;               /* 4 constant IS fits in two byte */
 word64 *d16v;                /* 16 constant IS form */
 word64 *dis16v;              /* 16 constant (IS form) delay */
 byte *dis16v1;               /* 16 constant IS fits in one byte */
 hword *dis16v2;              /* 16 constant IS fits in two bytes */
 struct expr_t *d1x;          /* 1 delay expr. */
 struct expr_t **d4x;         /* array of 4 expression ptrs */
 struct pthdst_t **pb_pthdst; /* per bit path dest. ptr to same dst. src */
 struct paramlst_t *pdels;    /* original compile time # delay form */
};

/* input instance structure before known whether gate or cell */
struct cell_pin_t {
 struct expr_t *cpxnd;        /* connecting expression root node */
 word32 cpfnam_ind;           /* file symbol defined in */
 int32 cplin_cnt;             /* def. line no. */
 char *pnam;                  /* probably no symbol table when read */
 struct cell_pin_t *cpnxt;
};

struct namparam_t {
 struct expr_t *pxndp;        /* parameter value as expr. */
 word32 prmfnam_ind;          /* file pound parameter appears in */
 int32 prmlin_cnt;            /* line pound parameter defined on */
 char *pnam;                  /* name as string - no sym table when read */
 struct namparam_t *nprmnxt;
};

/* record to store attributes - table of each from attribute inst */
struct attr_t {
 unsigned attr_tok : 8;       /* token attribute on */
 unsigned attr_seen : 1;      /* turn on when see attr of attr tok type */
                              /* off when added to object */
 unsigned attr_fnind : 16;    /* file attribute instance defined at */
 char *attrnam;               /* name but before parsed entire attr inst str */ 
 struct expr_t *attr_xp;      /* expr - nil if no value expr */
 int32 attrlin_cnt;           /* and line no. */
 struct attr_t *attrnxt;
};

/* source location struct for saving cell port line numbers */
struct srcloc_t {
 word32 sl_fnam_ind;
 int32 sl_lin_cnt;
};

/* compile time cell or gate contents */
struct cell_t {
 unsigned c_hasst : 1;        /* T => strength appears in source */
 unsigned c_stval : 6;        /* (3 bit 0 st. and 3 bit 1 st. */
 unsigned cp_explicit : 1;    /* T => all explicit connection list */
 unsigned c_named : 1;        /* T => instance is named */
 unsigned c_iscell : 1;       /* T => this is a timing lib. cell */  
 struct sy_t *csym;
 struct sy_t *cmsym;          /* module type symbol */
 struct cell_t *cnxt;
 struct expr_t *cx1, *cx2;    /* for arrays of gates/insts, the range */
 struct namparam_t *c_nparms;
 struct attr_t *cattrs;       /* attrs for cell - moved later */
 struct cell_pin_t *cpins;
};

/* list for blocks of allocated cells for fast block freeing */
struct cpblk_t {
 struct cell_t *cpblks;
 struct cpblk_t *cpblknxt; 
};

/* list for blocks of allocated cell pins for fast block freeing */
struct cppblk_t {
 struct cell_pin_t *cppblks;
 struct cppblk_t *cppblknxt;
};

/* list of tree node blocks */
struct tnblk_t {
 struct tnode_t *tnblks;
 struct tnblk_t *tnblknxt;
};

struct cpnblk_t {
 char *cpnblks;
 char *cpn_start_sp, *cpn_end_sp;
 struct cpnblk_t *cpnblknxt;
}; 

union gia_u {
 struct inst_t *gia_ip;
 struct gate_t *gia_gp;
};

/* index record for mod gate or inst arrays - pted to by insts/gates */ 
/* only need one of these per array of gate/insts */
struct giarr_t {
 unsigned gia_xpnd : 1;       /* T => inst/gate arr xpnd to per bit */
 unsigned gia_rng_has_pnd : 1;/* T => range has passed pound param */
 struct expr_t *giax1, *giax2;/* src constant rng exprs from cell */
 int32 gia1, gia2;            /* evaluated range - expr may chg */
 int32 gia_bi;                /* base mgates or minsts index */
 struct expr_t **giapins;     /* tab of original g/i pin expressions */
 struct sy_t *gia_base_syp;   /* original source base symbol name */
};

/* run time module instance contents */
struct inst_t {
 unsigned ip_explicit : 1;
 unsigned pprm_explicit : 1;  /* has explicit form pound params */
 unsigned i_iscell : 1;       /* instance is a cell */
 unsigned i_pndsplit : 1;     /* T => must pound param split */
 struct sy_t *isym;
 struct sy_t *imsym;          /* module type symbol */
 struct expr_t **ipxprtab;    /* inst. pound param expr. table, nil for none */
 struct attr_t *iattrs;       /* attribute table for inst. */
 struct expr_t **ipins;       /* ptr to array of port expr ptrs */
 struct expr_t ***pb_ipins_tab; /* if non nil, ptr to pb sep pin expr tab */ 
};

/* as if flattened instance structure - should use inum arrays instead ? */
/* think this can be smaller but per mod. inst. array better ? */
struct itree_t {
 int32 itinum;
 struct itree_t *up_it;       /* containing itree inst. */
 struct itree_t *in_its;      /* array of low itree elements */
 struct inst_t *itip;         /* corresponding inst_t info */
};

/* gate contents after fixup separation */
struct gate_t {
 unsigned gpnum : 14;         /* number of gate ports (limit 16k) */
 unsigned g_class : 3;        /* gate class for processing case */
 unsigned g_hasst : 1;        /* T => stren in src or tran(if) cycle break */
 unsigned g_stval : 6;        /* (3 bit each 0 and 1 st.) <ST:ST> if none */
 unsigned g_delrep : 5;
 unsigned g_unam : 1;         /* primitive has no instance name */
 unsigned g_gone : 1;         /* gate removed, no state */ 
 unsigned g_pdst : 1;         /* T => gate drives del/pth */
 union del_u g_du;            /* per type delay union */
 struct sy_t *gsym;
 struct sy_t *gmsym;          /* gate type symbol */
 union pck_u gstate;          /* per inst. storage of state vector */
 i_tev_ndx *schd_tevs;        /* per inst. array of scheduled events ndxes */
 struct attr_t *gattrs;       /* optional gate attribute */
 struct expr_t **gpins;       /* ptr to table of input port expr ptrs */

 /* routine to eval gate on input change */
 void (*gchg_func)(register struct gate_t *, register word32);
};

union pbca_u {
 struct conta_t *pbcaps;      /* for rhs cat per bit conta tab for sim */
 struct conta_t *mast_cap;    /* for per bit element, ptr to master */
 struct conta_t *canxt;       /* once mod's 1 bit conta rem, use table */
};

/* notice 1 bit ca's are simulated as gates */
struct conta_t {
 struct sy_t *casym;
 unsigned ca_hasst : 1;
 unsigned ca_stval : 6;       /* (3 bit 0 st. and 3 bit 1 st. */
 unsigned ca_delrep : 5;
 unsigned ca_4vdel : 1;       /* T => delay 4v so need to eval new_gateval */
 unsigned ca_gone : 1;        /* continuous assign could be removed */
 unsigned ca_pb_sim : 1;      /* T => in src rhs concat simulates as PB */
 unsigned ca_pb_el : 1;       /* T => this is a per bit el pb fld is master */
 union del_u ca_du;           /* per type delay table (only in master) */
 struct expr_t *lhsx;
 struct expr_t *rhsx;
 i_tev_ndx *caschd_tevs;      /* per inst. scheduled event ndx array */
 union pck_u ca_drv_wp;       /* ptr to rhs drive a/b values if needed */
 union pck_u schd_drv_wp;     /* pck per inst. scheduled value */
 union pbca_u pbcau;          /* up or down tab rhs cat decomposed PB val */ 
 struct conta_t *canxt;       /* for non rhs cat decomposed PB, nxt field */  
};

/* net storage */
/* compilation specific net auxiliary fields */
struct ncomp_t {
 /* compile time only flags */
 unsigned n_iotypknown : 1;
 unsigned n_wirtypknown : 1;
 unsigned n_rngknown : 1;     /* needed to separte range in i/o/wire decls */
 unsigned n_impldecl : 1;     /* declared implicitly */
 unsigned n_in_giarr_rng : 1; /* in array of g/i range expression */
 unsigned n_spltstate : 2;    /* scalared/vectored splitting of wire */
 unsigned n_onrhs : 1;        /* appears on some rhs (also declarative) */ 
 unsigned n_onlhs : 1;        /* appears on some lhs */
 unsigned n_2ndonlhs : 1;     /* T => more than 1 on lhs (for in to inout) */
 unsigned num_prtconns : 2;   /* count to 2 of no. of ports wire conns to */
 unsigned n_widthdet : 1;     /* T => parm is width determing - no IS form */
 unsigned n_indir_widthdet : 1; /* T => parm passed down to width det */
 unsigned p_setby_defpnd : 1; /* T => can't recalc since set by def/pnd */
 unsigned p_specparam : 1;    /* T => specify parameter */
 unsigned p_rhs_has_param : 1;/* T => param def rhs contains param */
 unsigned p_locparam : 1;     /* T => local parameter (never # or def) */
 unsigned prngdecl : 1;       /* T => range declared in source */
 unsigned ptypdecl : 1;       /* T => type declared in source */
 unsigned psigndecl : 1;      /* T => signed keyword declared in source */
 unsigned parm_srep : 4;      /* for parameter n_dels_u original expr rep */ 
 unsigned pbase : 3;          /* base of original rhs */
 unsigned pstring : 1;        /* T => if string appeared on original rhs */
 unsigned frc_assgn_in_src : 1; /* T => force or assign appears in src */
 unsigned monit_in_src : 1;   /* T => has mon/fmon in source for var */
 unsigned dmpv_in_src : 1;    /* if not all dummpvar => has mon/fmon in src */
 unsigned n_iscompleted : 1;  /* type def completed in earlier source */ 
 
 struct expr_t *nx1, *nx2;
 struct expr_t *ax1, *ax2;
 union del_u n_dels_u;
 byte *n_pb_drvtyp;           /* per bit drvr state tab for port dir chg */ 
 byte n_drvtyp;               /* for scalar simple stat tab for dir chg */
};

/* list for blocks of allocated ncomp for fast block freeing */
struct ncablk_t {
 struct ncomp_t *ancmps;
 struct ncablk_t *ncablknxt; 
};

/* array - LOOKATME - one dimensional only so far */
struct rngarr_t {
 int32 ni1, ni2;
 int32 ai1, ai2;
};

/* normal wire with delay or path destination wire */
struct rngdwir_t {
 int32 ni1, ni2;
 unsigned n_delrep : 5;
 union del_u n_du;            /* every bit has same delay for nets */ 
 i_tev_ndx *wschd_pbtevs;     /* any schd. event ndxes - indexed nwid*inst */
};

struct rngwir_t {
 int32 ni1, ni2;
};

/* various extra storage information for various stages for nets */
union nx_u {
 struct ncomp_t *ct;          /* compile time value */
 struct rngarr_t *rngarr;
 struct rngdwir_t *rngdwir;
 struct rngwir_t *rngwir;
};

union np_u {
 struct gref_t *npgrp;        /* xmr glb that needs normal downward proc. */
 struct itree_t *filtitp;     /* for root or uprel xmr filt to 1 inst */
 struct h_t *vpihp;           /* for vpi_ added, the assoc. handle */
};

/* auxiliary list struct of lists of per inst dce lsts for qcaf lhs concats */
struct dceauxlstlst_t {
 /* SJM 06/23/02 - for wire dce list for 1 lhs bit but ndx is per inst/bit */
 struct dceauxlst_t **dcelsttab; /* peri(/bit) tab of ptrs to dce lsts */
 struct dceauxlstlst_t *dcelstlstnxt;
};

/* auxiliary list struct for removing dce's of various types */ 
struct dceauxlst_t {
 struct dcevnt_t *ldcep;
 struct dceauxlst_t *dclnxt;
};

struct dvchgnets_t {
 struct net_t *dvchg_np; 
 struct itree_t *dvchg_itp;
 struct dvchgnets_t *dvchgnxt;
};

union dce_u {
 struct gref_t *dcegrp;       /* xmr glb that needs normal downward proc. */
 int32 pnum;                  /* for pvc dce tf routine port number */     
 struct task_t *tskp;         /* for dmpv dce, task variable in */
 struct cbrec_t *dce_cbp;     /* cbrec for vpi_ val chg callback */
};

/* SJM 06/12/02 - need 2nd union to separate different types of data */
union dce2_u {
 struct fmonlst_t *dce_fmon;  /* nil if monit else fmon record for monitors */
 struct expr_t *dce_pvc_fcallx; /* pvc misctf fcall expr */
 struct st_t *dce_pvc_stp;    /* pvc misctf statment */
 struct qcval_t *dce_qcvalp;  /* for qcaf dce, stmts qcval record */ 
};

/* for edge where value expr., need general information for filtering */
/* all decomposed dce's from expr, pt. to one of these */
struct dce_expr_t {
 struct expr_t *edgxp;
 byte *bp;
 struct dcevnt_t *mast_dcep;  /* one master (first) for alloc and init */ 
};

/* fixed (except $monitor chges) list off wire of things to trigger */
struct dcevnt_t {
 unsigned dce_typ : 8;        /* itree location and action match type */ 
 unsigned dce_xmrtyp : 2;     /* indicates if xmr and type */ 
 unsigned dce_1inst : 1;      /* one instance form must match match itp */
 unsigned dce_edge : 1;       /* T => need edge filtering  */
 unsigned dce_edgval : 8;     /* edge signature for filter (only pos/neg) */
 unsigned dce_nomonstren : 1; /* T => ignore stren for monit of stren */
 unsigned dce_tfunc : 1;      /* T => for pvc dce func. not task */
 unsigned dce_off : 1;        /* for byte code PLI/monitor need dce on/off */ 
 unsigned is_fmon : 1;        /* monitor is fmon */
 union pck_u prevval;         /* storage for wire or range for non dmpv */
 int32 dci1;                  /* for range dci2 union for IS param */ 
 union intptr_u dci2;         /* for IS param union rng ptr (dci1 = -2) */
 struct net_t *dce_np;        /* net this dce is triggerd by */
 struct delctrl_t *st_dctrl;  /* statement's delay control record */
 union dce_u dceu;            /* for xmr - propagation gref info */
 union dce2_u dceu2;          /* nil if monit else fmon record */
 struct itree_t *dce_matchitp;/* for 1 inst. must match this itree target */
 struct itree_t *dce_refitp;  /* for 1 inst. this is reference itree loc */
 struct itree_t *iact_itp;    /* for per inst. iact need inst. to match */
 struct dce_expr_t *dce_expr; /* for edge where need expr old val and expr */
 struct dcevnt_t *dcenxt;     /* next on wire dcelst */
};

/* built in table for every built in primitive (gate) */
struct primtab_t {
 unsigned gateid : 8;
 unsigned gclass : 8;         /* class of gate */
 struct sy_t *gnsym;
 char *gatnam;
};

/* table for system task info */
struct systsk_t {
 word32 stsknum;              /* systask ind or 1000+index veriusertfs[] */
 char *stsknam;               /* name of system task or pli task */
};

/* list for +loadvpi or +loadpli1 dynamic lib and bootstrap function pairs */
union dynlb_u {
 void (*vpi_rout)(void);
 void *(*tf_rout)(void);
};

/* need list of boot strap routines because option allows more than one */
struct dynboot_t {
 char *bootrout_nam;
 union dynlb_u dynu;
 struct t_tfcell *ret_veriusertf;
 struct dynboot_t *dynbootnxt;
}; 

struct loadpli_t {
 unsigned pli1_option : 1;    /* T => +loadpli1= option */
 char *libnam;
 struct dynboot_t *dynblst;   /* list of boot routines (can be empty) */ 
 /* for pli1, if non nil ptr to veriusertf tab */
 struct loadpli_t *load_plinxt;
};

struct tfinst_t { 
 struct expr_t *callx;
 struct st_t *tfstp;
 struct itree_t *tfitp;
 struct task_t *tftskp;
};

/* table for system function info */
struct sysfunc_t {
 word32 syfnum;               /* sysfunc ind. or 1000+ind veriusertfs[] */
 unsigned retntyp : 8;
 unsigned retsigned : 1;      /* need for signed wire */
 unsigned tftyp : 8;          /* type */
 int32 retwid;                /* for veriuser and vpi systf sizetf call sets */
 char *syfnam;                /* name of system function */
};

union vsystf_u {
 struct expr_t *sysfcallx;    /* if systf fcall, vpi_ systf call expr. */
 struct st_t *syststp;        /* stmt vpi_ systf call or task enable in */
};

/* list element for each location vpi_ systf appears in source */
/* cross linked through sz_u from calling expr node */
struct vpisystf_t {
 unsigned is_sysfunc : 1;     /* t +> vpi_ systf func */
 unsigned vstffnam_ind : 16;
 int32 vstflin_cnt;
 struct mod_t *curmdp;        /* ptr. to current mod source line of call in */
 struct task_t *curtskp;      /* ptr. to current task source line of call in */
 union vsystf_u vsystfu;      /* ptr to callx or sys task stmt */ 
 struct vpisystf_t *vpistfnxt; 
};

union mpp_afunc_u {
 /* routine for input cross port assign */
 void (*mpp_downassgnfunc)(register struct expr_t *,
  register struct expr_t *, struct itree_t *);
 /* routine for output cross port assign */
 void (*mpp_upassgnfunc)(register struct expr_t *,
  register struct expr_t *, struct itree_t *);
};

/* work record for decomposed into per bit declarative assign */
struct pbexpr_t {
 unsigned ndx_outofrng : 1;
 unsigned rhs_const : 1;
 unsigned no_rhs_expr : 1;     /* lhs widers bits need 0/z assign */
 struct expr_t *xp;
 int32 bi;                     /* for bsel/psel offset in var object */
 word32 aval;                  /* aways 1 bit constant value (for rhs only) */
 word32 bval;
};

/* module port element */
struct mod_pin_t {
 unsigned mptyp : 3;          /* I/O type - maybe changed with warning */
 unsigned mp_explicit : 1;    /* T => source explicit port reference */
 unsigned mp_jmpered : 1;     /* T => port has net also in other port */
 unsigned inout_unc : 1;      /* T => this inout unc. somewhere */
 unsigned assgnfunc_set : 1;  /* T => know the port assign func */
 unsigned has_mipd : 1;       /* T => input port has mipd delay */
 unsigned has_scalar_mpps : 1;/* T => simulated per bit */
 unsigned mpfnam_ind : 16;    /* file symbol defined in */
 int32 mplin_cnt;             /* def. line no. */
 int32 mpwide;                /* width of port */
 
 char *mpsnam;                /* name not symbol of port (can be NULL) */
 struct expr_t *mpref;        /* expression form port list (const.) */
 struct attr_t *mpattrs;      /* all I/O decls attrs for conn nets */

 /* union of proc ptrs  for input/output cross port assign */
 union mpp_afunc_u mpaf;
 struct mod_pin_t *pbmpps;    /* for rhs hconn input port, per bit mpp */
 struct mod_pin_t *mpnxt;     /* elaborate time next link */
};

/* table build per line for wild card processing */
struct wcard_t {
 int32 wcinum;                /* in port (also state) numb for wild card */
 char wcchar;                 /* wild card ? or b */
};

/* udp table storage */
struct utline_t {
 /* these must be char pairs even for combinatorial */
 unsigned ulhas_wcard : 1;
 unsigned ullen : 8;          /* length of original tline in bytes */
 unsigned utabsel : 8;        /* first edge char table select (maybe wc) */
 unsigned uledinum : 8;       /* input position number of one allowed edge */
 unsigned utlfnam_ind : 16;   /* file no. of udp line */
 int32 utlin_cnt;             /* line no. of udp tble line */
 char *tline;                 /* table line - for edge 2nd char in line */
 struct utline_t *utlnxt;
};

/* special struct for udp tables */
struct udptab_t {
 word32 *ludptab;
 /* alloced according to required size (max 27 pointers to tabs of words */
 word32 **eudptabs;
};

struct udp_t {
 unsigned utyp : 4;
 unsigned numins : 8;         /* number of inputs */
 unsigned numstates : 8;      /* for combinatorial same else one more */
 unsigned ival : 8;           /* initial value (0,1,3 - none) */
 unsigned u_used : 1;         /* T => need table, instanciated in some mod */
 unsigned u_wide : 1;         /* T => uses alt. wide representation */
 int32 uidnum;                /* tmp. id number for counting */
 struct sy_t *usym;
 struct symtab_t *usymtab;    /* symbol table just for terminals */
 struct mod_pin_t *upins;     /* defined ports for order */
 struct utline_t *utlines;
 struct udp_t *udpnxt;
 struct udptab_t *utab;
};

/* task port (like procedural variable - all info in reg(wire)) */
struct task_pin_t {
 unsigned trtyp : 8;          /* i/o type (same as sym np iotyp) */
 struct sy_t *tpsy;           /* port name sym. points to wire/reg */
 struct task_pin_t *tpnxt;
};

/* procedureal assignment statement */
struct proca_t {
 struct expr_t *lhsx;         /* need to make sure this is an lvalue */
 struct expr_t *rhsx;
};

/* quasi continous assigns */
struct qconta_t {
 unsigned qcatyp : 8;
 unsigned regform : 8;
 struct expr_t *qclhsx;
 struct expr_t *qcrhsx;
 struct dceauxlstlst_t *rhs_qcdlstlst;
};

/* quasi cont. deassigns - no continuous deassign */
struct qcontdea_t {
 unsigned qcdatyp : 8;
 unsigned regform : 8;
 struct expr_t *qcdalhs;
};

struct if_t {
 struct expr_t *condx;
 struct st_t *thenst;
 struct st_t *elsest;
};

struct csitem_t {
 struct exprlst_t *csixlst;
 struct st_t *csist;          /* for unnamed block can be list */
 struct csitem_t *csinxt;
};

struct case_t {
 unsigned castyp : 8;
 unsigned maxselwid : 24;
 struct expr_t *csx;
 struct csitem_t *csitems;    /* first item always default: place holder */  
};

struct wait_t { 
 struct expr_t *lpx;
 struct st_t *lpst;
 struct delctrl_t *wait_dctp;
};

/* for forever and while */
struct while_t {
 struct expr_t *lpx;
 struct st_t *lpst;
};

struct repeat_t {
 struct expr_t *repx;
 word32 *reptemp;             /* per inst. word32 current value tmp */
 struct st_t *repst;
};

struct for_t {
 struct st_t *forassgn;
 struct expr_t *fortermx;
 struct st_t *forinc;
 struct st_t *forbody;
};

/* for Ver 2001 - implicit delay control var list in action stmt */
struct evctlst_t {
 struct net_t *evnp; 
 int32 evi1, evi2;
};

/* normal separate delay control - but in case of rhs control contains st. */
/* event expression allow events edges of events and oring of ev. exprs */
struct delctrl_t {
 unsigned dctyp : 8;          /* event or delay */
 unsigned dc_iact : 1;        /* iact delay control */ 
 /* SJM 06/10/02 - distinguish non blocking from blocking event ctrls */  
 unsigned dc_nblking : 1;     /* T => delay control is non blocking form */
 unsigned implicit_evxlst : 1;/* special all rhs's @(*) ev var-ndx list */ 
 unsigned dc_delrep : 5;      /* normal gca delay type */
 union del_u dc_du;           /* normal delay union (also used for ev ctrl) */
 struct expr_t *repcntx;      /* for rhs ev ctrl repeat form, rep cnt expr */
 i_tev_ndx *dceschd_tevs;     /* per inst. scheduled event ndxes */ 
 /* SJM 04/02/01 - need word32 down repeat counter with end when becomes 0 */ 
 word32 *dce_repcnts;         /* per inst. current ev rep down count val */
 struct st_t *actionst;
};

/* header list for each initial always block */
struct ialst_t {
 unsigned iatyp : 8;
 struct st_t *iastp; 
 
 int32 ia_first_lini;         /* line no. of initial/always */
 int32 ia_first_ifi;          /* and file index */
 int32 ia_last_lini;          /* line no. of end (mabye in next file) */
 int32 ia_last_ifi;           /* end file in case spans multiple files */
 struct ialst_t *ialnxt;   
};

union tf_u {
 struct expr_t *callx;        /* for tf_ func ptr back to call expr else nil */
 struct st_t *tfstp;          /* for tf_ task ptr back to call stmt else nil */
};

union tfx_u {
 struct expr_t *axp;          /* for arg the expr. */
 word32 *awp;                 /* for 0th return value just the value */
};

/* per parameter tfrec record */
struct tfarg_t {
 union tfx_u arg;
 struct net_t *anp;           /* for is rw, the wire */
 byte *old_pvc_flgs;          /* old and sav pvc flags */
 byte *sav_pvc_flgs;
 struct dltevlst_t **dputp_tedlst;/* per inst ptr to list of delputps */
 char **sav_xinfos;           /* per inst evalp saved expr info rec */
 union pck_u tfdrv_wp;        /* for rw, pointer to param driver value */ 
};

/* extra task call side data for pli tasks/funcs (each call loc. differs) */
struct tfrec_t {
 unsigned tf_func : 1;        /* is record for tf_ func */
 unsigned fretreal : 1;       /* func. returns real */
 unsigned tffnam_ind : 16;
 int32 tflin_cnt;
 int32 tfanump1;              /* no. of args plus 1 (included 0th func ret) */
 int32 fretsiz;               /* for function return size (sizetf ret. val) */  
 union tf_u tfu;              /* ptr back to tf_ task or function */
 struct mod_t *tf_inmdp;      /* module in (for inst. number) */
 struct task_t *tf_intskp;    /* task call in (nil in in init/always */
 struct tfarg_t *tfargs;      /* per parameter param record table */
 char **savwrkarea;           /* per inst saved work areas */
 i_tev_ndx *sync_tevp;        /* per inst ndx to sync event */
 i_tev_ndx *rosync_tevp;      /* per inst ndx to ro sync event in list */ 
 byte *asynchon;              /* per inst. async on flag */
 struct tevlst_t **setd_telst; /* per inst ptr to list of set dels */  
 struct dceauxlst_t **pvcdcep;/* per inst dce list but all parameters */
 struct tfrec_t *tfrnxt;      /* linked list */
};

/* vpi handle union of internal d.s. elements */
union h_u {
 struct gate_t *hgp;
 struct net_t *hnp;
 struct task_t *htskp;        /* also named block and function */
 struct mod_t *hmdp;          /* for mods in modsyms table */
 struct ialst_t *hialstp;     /* process - init or always */
 struct mod_pin_t *hmpp;      /* udp port or net-pin list mod. I/O port */
 struct conta_t *hcap;        /* pointer to conta */
 struct spcpth_t *hpthp;      /* point back to path for special id symbol */    
 struct dfparam_t *hdfp;      /* for defparam source reconstruction handles */
 struct tchk_t *htcp;         /* and back to time check */      
 struct expr_t *hxp;          /* representation as ptr to expr. */
 struct st_t *hstp;           /* statement handle */
 struct csitem_t *hcsip;      /* case item */
 int32 hpi;                   /* for port bit handle the port index */
 /* this is only contents field that is freed */
 struct pviter_t *hiterp;     /* iterator indirect element with alloc list */
 struct cbrec_t *hcbp;        /* callback handle guts record */
 i_tev_ndx htevpi;            /* event ndx for sched. vpi put value only */
 struct net_pin_t *hnpp;      /* for driver the drvr net pin record */
 struct udp_t *hudpp;         /* udp handle object record */ 
 struct utline_t *hutlp;      /* udp line for table entry handle */
 struct schdev_t *hevrec;     /* pending vpi_ driver all net sched. evs tab */
 void *hanyp;                 /* any ptr so do not need to cast to it */
 struct hrec_t *hfreenxt;     /* ptr to next hrec on free list */
};

/* non-blocking event list */
struct nb_event_t { 
 i_tev_ndx nb_tevpi; 
 struct nb_event_t *nbnxt;
};

/* handle object event table record for scheduled vector events */ 
struct schdev_t {
 struct net_t *evnp;
 i_tev_ndx *evndxtab; 
};

/* actual handle */
struct h_t {
 struct hrec_t *hrec;         /* hadnle content record */
 struct itree_t *hin_itp;     /* itree inst object in (if relevant) */
};

/* handle contents */
struct hrec_t {
 unsigned htyp : 16;          /* vpi object type of handle */
 unsigned htyp2 : 16;         /* extra type */
 unsigned h_magic : 11;       /* magic number for internal checking */
 unsigned in_iter : 1;        /* part of iterr, free when iterator freed */
 unsigned free_xpr : 1;       /* free expr. when free handle */
 unsigned evnt_done : 1;      /* for vpi schd event, event happened */
 unsigned bith_ndx : 1;       /* bit handle index form (also any tab form) */ 
 union h_u hu;                /* ptr to object (could be void *) */ 
 int32 hi;                    /* for bit of object handle the index */
 struct task_t *hin_tskp;     /* task (scope) handle in (or ialst) */
};

struct pviter_t {
 struct h_t *scanhtab;        /* table of handles built by vpi iterator */
 struct hrec_t *ihrectab;     /* table of handle content records */ 
 int32 nxthi;                 /* next handle to return */
 int32 numhs;                 /* total number of handles for iterator */
};

struct onamvpi_t {
 char *vpiobjnam;
 word32 vpiotyp;              /* redundant value same as obj constant ind */
};

struct pnamvpi_t { 
 char *vpipropnam;
 int32 vpiproptyp;            /* redundant value same as prop constant ind */
};

struct opnamvpi_t { 
 char *vpiopnam;
 int32 vpioptyp;              /* redundant value same as op constant ind */
 char *vpiopchar;             /* operator character (such as +) */
};

union systf_u { 
 struct systsk_t *stbp;
 struct sysfunc_t *sfbp;
};

struct systftab_t {
 unsigned systf_chk : 1;      /* T => already seen in source and checked */ 
 void *vpi_sytfdat;
 union systf_u sfu;
};

/* guts of callback handle - separate handle for each instance */
/* info from passed user cb_data record copied into here */
struct cbrec_t {
 unsigned cb_reason : 12;     /* reason field from passed cb_data rec. */
 unsigned cb_rettimtyp : 6;   /* time record field type to return */
 unsigned cb_retvalfmt : 7;   /* value record field format to return */
 unsigned cb_class : 3;       /* internal scheduling class */  
 unsigned cb_ndxobj : 1;      /* T => value chg handle is indexed */  
 unsigned cb_user_off : 1;    /* T => user turned off with sim control */
 unsigned cb_gateout : 1;     /* T => gate outut val chg type cb */ 
 struct h_t *cb_hp;           /* object */ 
 int32 (*cb_rtn)();           /* cb_data call back routine */
 char *cb_user_data;          /* user data field from passed cb_data rec. */
 struct dceauxlst_t *cbdcep;  /* list of dces for cb removal */ 
 i_tev_ndx cbtevpi;           /* ndx (ptr) back to event */
 struct h_t *cb_cbhp;         /* associated handle of call back */
 struct cbrec_t *cbnxt;       /* next in chain of all callbacks */
 struct cbrec_t *cbprev;      /* previous in chain of all callbacks */
};

/* list for release and force cb all forms - many cbs possible */
struct rfcblst_t {
 struct cbrec_t *rfcbp; 
 struct rfcblst_t *rfcbnxt;
};

/* SJM 06/20/02 - for monit now bld dces where can during prep, need dce ptr */
struct monaux_t {
 unsigned dces_blt : 1;       /* T => dces already built for this monit */
 byte *argisvtab;             /* tab of v format flags for monitor */
 struct dceauxlst_t **mon_dcehdr; /* lst of per inst dces bld during prep */ 
};

/* SJM 06/20/02 - change sys task aux field to union instead of casts */
/* sys task call union for PLI systf calls and monitor/fmonitor aux info */
union tkcaux_u {
 struct tfrec_t *trec;        /* ptr to aux pli tf_ data */
 struct monaux_t *mauxp;      /* ptr to mon aux v format flag and dce ptr */
 void *vpi_syst_datap;
};

struct tskcall_t {
 struct expr_t *tsksyx;       /* task symbol or global table entry */
 struct expr_t *targs;        /* FCCOM expr of args */
 union tkcaux_u tkcaux;       /* ptr to aux pli tf_ data or monit rec */
};

struct dsable_t {
 struct expr_t *dsablx;       /* expr. (ID/GLB) of disable task/func/blk */
 struct st_t *func_nxtstp;    /* for functions disable is goto */
};

struct fj_t {
 struct st_t **fjstps;        /* ptr to tab (nil ended) fork-join stmts */
 int32 *fjlabs;               /* ptr to tab of fj start lab nums */
};

/* slot end stmt list for strobes with itree where execed strobe appeared */
struct strblst_t {
 struct st_t *strbstp;
 struct itree_t *strb_itp;
 struct strblst_t *strbnxt;
};

/* slot end triggered fmonitor list */
struct fmselst_t {
 struct fmonlst_t *fmon;
 struct fmselst_t *fmsenxt;
};

/* acctivated fmonitor list with itree location */
struct fmonlst_t {
 word32 fmon_forcewrite;
 struct st_t *fmon_stp;
 struct itree_t *fmon_itp;
 struct fmselst_t *fmse_trig; /* set when monitor triggered 1st time */
 struct dceauxlst_t *fmon_dcehdr;
 struct fmonlst_t *fmonnxt;
}; 

/* union either 4 byte ptr or 12 byte union in statment */
/* save space since previously needed 4 byte ptr plus record */
union st_u {
 struct proca_t spra;
 struct if_t sif;
 struct case_t scs;
 struct while_t swh;
 struct wait_t swait;
 struct repeat_t srpt;
 struct for_t *sfor;
 struct delctrl_t *sdc;
 struct task_t *snbtsk;       /* named (like task) block */
 struct st_t *sbsts;          /* unnamed block statement list */
 struct fj_t fj;              /* fj 2 tabs (1 stps and 1 fj c code labs */
 struct tskcall_t stkc;
 struct qconta_t *sqca;       /* assign and force */
 struct qcontdea_t sqcdea;    /* deassign and release */
 struct expr_t *scausx;       /* cause expr. node symbor or global id. */
 struct dsable_t sdsable;     /* disable struct (also place for goto dest) */
 struct st_t *sgoto;          /* destination of added goto */
};

/* statement structure */
struct st_t {
 unsigned stmttyp : 5;
 unsigned rl_stmttyp : 5;     /* for break set at statment, real type */
 unsigned st_unbhead : 1;     /* if unnamed block removed, indicated here */  
 unsigned strb_seen_now : 1;  /* T => for checking strobe on list for now */
 unsigned lpend_goto : 1;     /* T => this is loopend goto */
 unsigned dctrl_goto : 1;     /* T => this is next after dctrl chain goto */ 
 unsigned lstend_goto : 1;    /* T => list end continue goto */
 unsigned st_schd_ent : 1;    /* T => can be first stmt entered from schd */
 unsigned lpend_goto_dest : 1; /* dest. of loop end back goto for compiler */
 unsigned stfnam_ind : 16;
 int32 stlin_cnt;
 union st_u st;
 struct st_t *stnxt;
};

/* contents of break point table */
struct brkpt_t {
 unsigned bp_type : 4;        /* type of breakpoint */ 
 unsigned bp_can_halt : 1;    /* T => really break, F start and reset */
 unsigned bp_enable : 1;      /* T => break not disabled */
 unsigned bp_prttyp : 2;      /* type of printing when hit */
 unsigned bp_dup : 1;         /* T => multiple break points at statement */
 unsigned bp_rm_when_hit : 1; /* T => remove when hit */
 int32 bp_num;                /* identifying number */
 int32 bp_ignore_cnt;
 int32 bp_hit_cnt;            /* number of times hit */
 struct st_t *bp_stp;         /* statement this break point breaks at */
 struct itree_t *bp_itp;      /* itree loc. - any (first?) for all of type */ 
 struct task_t *bp_tskp;      /* task break point in else NULL if not */
 struct expr_t *bp_condx;     /* :cond cmd filter expr. */ 
 /* expressions and watch point values go here */
 struct brkpt_t *bpnxt;
};

struct dispx_t {
 unsigned dsp_enable : 1;
 unsigned dsp_frc_unsign : 1;
 int32 dsp_num;
 int32 dsp_prtfmt;
 int32 dsp_prtwidth;
 struct expr_t *dsp_xp;
 struct itree_t *dsp_itp;
 struct task_t *dsp_tskp;
 struct dispx_t *dsp_nxt;
};

/* horizontal doubly linked list of task threads */
struct tskthrd_t {
 struct tskthrd_t *tthd_l, *tthd_r;
 struct thread_t *tthrd;
};

/* task definition - type of task name symbol determines task type */
struct task_t {
 unsigned tsktyp : 8;         /* symbol number giving task type */
 unsigned t_used : 1;         /* T => task/func. invoked some where */
 unsigned thas_outs : 1;      /* T => task has outs that must be stored */ 
 unsigned thas_tskcall : 1;   /* T => task contains other task call */
 unsigned fhas_fcall : 1;     /* T => func contains non sys fcall */
 unsigned tf_lofp_decl : 1;   /* T => t/f hdr list of port decl form */ 
 struct sy_t *tsksyp;         /* name symbol in module level table */
 int32 tsk_last_lini;         /* line no. of end (mabye in next file) */
 int32 tsk_last_ifi;          /* end file in case spans multiple files */
 struct symtab_t *tsksymtab;  /* symbol table for objects in task */
 struct st_t *st_namblkin;    /* stmt named block in in */
 struct task_pin_t *tskpins;  /* task ports - procedural not wires */
 struct net_t *tsk_prms;      /* parameters defined in task */
 struct net_t *tsk_locprms;   /* local parameters defined in task */
 int32 tprmnum;               /* number of task parameters */
 int32 tlocprmnum;            /* number of task local parameters */
 struct net_t *tsk_regs;      /* list then array of nets in task */
 int32 trnum;                 /* number of task regs */   
 struct st_t *tskst;          /* one task statement (usually a block) */
 struct task_t *tsknxt;       /* next defined task in current module */
 struct tskthrd_t **tthrds;   /* per inst. list of active threads for task */
};

/* symbol table tree nodes */
struct tnode_t {
 unsigned bal : 8;
 unsigned via_dir : 8;
 struct sy_t *ndp;
 struct tnode_t *lp, *rp;
};

/* symbol table element (type) pointer union */
union el_u {
 struct cell_t *ecp;
 struct inst_t *eip;
 struct gate_t *egp;
 struct net_t *enp;
 struct st_t *estp;
 struct delctrl_t *edctp;     /* delay ctrl for re prep pnps */
 struct task_t *etskp;        /* also named block and function */
 struct systsk_t *esytbp;     /* pointer to system task table entry */
 struct sysfunc_t *esyftbp;   /* pointer to system func. table entry */
 struct mod_t *emdp;          /* for mods in modsyms table */
 struct primtab_t *eprimp;    /* for prims (gates) in modsyms table */
 struct udp_t *eudpp;         /* for udps in modsysm table */
 struct mod_pin_t *empp;      /* udp port or net-pin list mod. I/O port */
 char *edfchp;                /* value of `define macros in defsyms table */
 struct amac_t *eamacp;       /* value for macro with arguments */
 struct conta_t *ecap;        /* pointer to conta */
 struct spcpth_t *epthp;      /* point back to path for special id symbol */    
 struct tchk_t *etcp;         /* and back to time check */      
 int32 eii;                   /* index of inst. in mod inst list or ni */
 struct tchg_t *etchgp;       /* time change record */ 
 struct chktchg_t *echktchgp; /* check time change record */
 struct undef_t *eundefp;     /* for undefined temp. link to list el. */
 struct tfrec_t *etfrp;       /* ptr to tf_ func/task record */
 void *eanyp;                 /* castable void * for special cases */
 struct mipd_t *emipdbits;    /* ptr to per bit mipd record for MIPD npp */
};


/* net pin aux struct for special cases */
struct npaux_t {
 int32 nbi1;                  /* range of connection (-1 for all) */
 /* convention if nbi1 is -2, nbi2 is word32 * union ptr to 2 ISNUM tables */
 union intptr_u nbi2;         /* for IS param union rng ptr (dci1 = -2) */
 int32 lcbi1, lcbi2;          /* if fi is lhs concat, the rhs val subrange */
 union np_u npu;
 /* for src down itree loc tab for root, must match targ. itree */
 /* SJM 05/14/03 - since only for rooted XMR npp, this is addr not peri tab */
 struct itree_t *npdownitp;
};

struct net_pin_t {
 unsigned npntyp : 4;         /* type of net pin connection */
 unsigned npproctyp : 2;      /* type of npp processing needed */ 
 unsigned chgsubtyp : 3;      /* for change the change subtype */
 unsigned np_xmrtyp : 2;      /* xmr type */
 unsigned pullval : 1;        /* for pull driver 0 or 1 */
 unsigned obnum : 20;         /* port index number */
 int32 pbi;                   /* for per bit bit index */

 union el_u elnpp;            /* net pin connecting objects */
 struct npaux_t *npaux;       /* aux. for net pins needing special fields */
 struct net_pin_t *npnxt;
};

/* parm nplist for delay update on param change delay anotation */
struct parmnet_pin_t {
 unsigned pnptyp : 8;         /* delay expr (or list) type param ref. in */
 unsigned pnp_free : 1;       /* T => master ptr to delay need to free */
 union el_u elpnp;            /* in mod element with delay */
 struct paramlst_t *pnplp;    /* param list (all dels) - copy or orig */
 struct parmnet_pin_t *pnpnxt;
};

/* PLI list of mod port or iconn npp's not yet processed for XL style iters */
struct xldlnpp_t {
 struct net_pin_t *xlnpp;
 struct itree_t *xlitp;
 struct xldlnpp_t *xlnxt;
};

/* record accessed through sorted index of net/bits with xl style drv/lds */
struct xldlvtx_t {
 struct net_t *dlnp;
 int32 dlbi;
 struct itree_t *dlitp;
};

/* edge list - since both vertices point to this */
struct edge_t { 
 struct vtx_t *ev2;           /* other side edge, this side is cur vtx */
 struct itree_t *edgoside_itp; /* edge other side itree loc (nil if same) */
 /* npnxt has value for traux list for vpi - tran chan must not touch */ 
 struct net_pin_t *enpp;      /* npp of connecting edge (npp) */
 struct edge_t *enxt;
};

/* vertex of tran graph - only traversed thru edges - need npp itp chg */ 
struct vtx_t {
 unsigned new_vtxval : 8;     /* for tran chan, new value */
 unsigned old_vtxval : 8;     /* value at start of tran relax */
 unsigned vtx_chged : 1;      /* T => vertex changes - when done store */
 unsigned vtx_in_vicinity : 1;/* T => vertex already in vincinity */
 unsigned vtx_forced : 1;     /* T => vertex is forced */
 unsigned vtx_supply : 1;     /* T => vertex hard drivers/decl supply */ 
 unsigned vtx_mark : 1;       /* mark bit used for cutting cycles */
 unsigned vtx_mark2 : 1;      /* mark bit used for turning off marks */
 struct net_t *vnp;           /* net in channel */
 int32 vi1;                   /* bit index for vector wire */
 struct edge_t *vedges;       /* ptr to adjacency list for this node */
};

/* per bit vtx and chan. id table for this static inst. loc. */
struct vbinfo_t {
 int32 chan_id;               /* ID for channel */
 struct vtx_t *vivxp;         /* vertex */
};

/* net tran aux record on - exists if net on some bid or tran channel */
struct traux_t {
 union pck_u trnva;           /* per inst hard driver values for all of net */
 struct vbinfo_t **vbitchans; /* (chan_id, vtx) per bit or bit-inst */
 /* notice this uses npp npnxt field because unused by edges */
 struct net_pin_t *tran_npps; /* ptr to linked tran chan npps */
};

/* list element for tran channel relaxation */
struct vtxlst_t { 
 struct vtx_t *vtxp;
 struct itree_t *vtx_itp;
 struct vtxlst_t *vtxnxt;
};

/* list of net/bits in switch channel for init and change processing */
struct bidvtxlst_t {
 struct net_t *vnp;           /* net in channel */
 int32 vi1;                   /* bit index for vector wire */
 struct itree_t *bidvtx_itp;
 struct bidvtxlst_t *bidvtxnxt;
};

/* per channel info - some needed for future rtran(if) cutting */
struct chanrec_t {
 unsigned chtyp : 3;          /* channel processing type */
 unsigned chan_diffwtyps : 1; /* T => different wire types in channel */
 unsigned chan_no_vtxs : 1;   /* T => for pure bid after cnvt to list */
 struct vtx_t *chvxp;         /* for now one random vertex */ 
 struct itree_t *chvitp;      /* and its itree loc. */
 struct bidvtxlst_t *bid_vtxlp;  /* for bid only, list with itree loc */
 word32 numvtxs;              /* number of vertices (net-bits) in channel */
};

/* master dumpvar header record for module argument */
struct mdvmast_t { 
 struct itree_t *mdv_itprt;   /* itree root of subtree or var itree loc */
 struct task_t *mdv_tskp;     /* task variable or scope in */
 struct net_t *mdv_np;        /* for var. form, pointer to wire */
 struct mdvmast_t *mdvnxt; 
 int32 mdv_levels;            /* level down from $dumpvars call */
};

struct qcval_t {
 unsigned qc_active : 1;      /* T => this net(/bit) force or assign active */
 unsigned qc_overridden : 1;  /* T => reg assgn active but force overrides */
 struct st_t *qcstp;
 int32 qcrhsbi;               /* if concat, rhs low bit index else -1 */
 int32 qclhsbi;               /* for wire, select lhs low bit index */
 struct itree_t *lhsitp;
 struct dceauxlst_t *qcdcep;  /* rhs dce tab of lists */
};

/* qcval union during prep and exec but other uses earlier and for params */
union nqc_u {
 word32 *wp;                  /* for params work saved param val ptr */ 
 int32 npi;                   /* for params fixup sorting/matching val */
 struct net_t *nnxt;          /* during src rd and fixup, nets not yet tab */ 
 struct qcval_t *qcval;       /* per inst. (maybe per bit) qc aux records */
};

/* table of driver and scheduled values for each added vpi_ net driver */
/* know if any at least two, last has both fields nil */
struct vpi_drv_t {
 union pck_u vpi_hasdrv;      /* per inst. net wide driver added table */
 union pck_u vpi_drvwp;       /* per inst. net wide driver values */
 struct dltevlst_t **putv_drv_tedlst;/* per inst per bit pending ev. lists */
 struct dltevlst_t **softforce_putv_tedlst; /* per inst/bit pend ev lists */
};

struct net_t {
 unsigned ntyp : 4;
 unsigned iotyp : 3;          /* I/O direction */
 unsigned n_isaparam : 1;     /* T => net is a parameter */
 unsigned srep : 4;           /* storage representation of value - nva fld */
 unsigned nrngrep : 3;        /* representation for net ranges - nx_u fld */
 unsigned n_isavec : 1;       /* this is a vector */
 unsigned vec_scalared : 1;   /* wire is scalared (bit by bit, T if scalar) */
 unsigned n_isarr : 1;
 unsigned n_signed : 1;
 unsigned n_stren : 1;        /* T => needs strength model */
 unsigned n_capsiz : 2;       /* special capacitor size constant */
 unsigned n_mark : 1;         /* misc. marking flag for var. uses */ 
 unsigned n_multfi : 1;       /* at least one bit fi > 1 */
 unsigned n_isapthsrc : 1;    /* T => port net a pth src (or tc ref ev.) */
 unsigned n_isapthdst : 1;    /* T => port net is a path dest */
 unsigned n_hasdvars : 1;     /* T => at least one inst. dumpvared */
 unsigned n_onprocrhs : 1;    /* appears on some procedural rhs */
 unsigned n_gone : 1;         /* T => net deleted by gate eater */
 /* 03/21/01 - these are fields from removed separate optim table */ 
 unsigned nchg_nd_chgstore : 1;/* need chg store for var */
 unsigned nchg_has_dces : 1;  /* T => var currently has delay ctrls */
 unsigned nchg_has_lds : 1;   /* var has lds (structural from src */
 unsigned frc_assgn_allocated : 1; /* T => force/assign in src or PLI added */
 unsigned dmpv_in_src : 1;    /* T => dumpvars for net in src */
 unsigned monit_in_src : 1;   /* monit for var somewhere in src */
 unsigned n_onrhs : 1;        /* appears on some rhs (also declarative) */ 
 unsigned n_onlhs : 1;        /* appears on some lhs */
 unsigned n_drvtyp : 3;       /* for scalar, pre elab p type chg drvr state */
 int32 nwid;                  /* net width */
 union pck_u nva;             /* storage for net */
 struct sy_t *nsym;
 union nx_u nu;               /* ptr to auxiliary range and delay values */
 struct dcevnt_t *dcelst;     /* fixed list of dce/monit events */
 struct net_pin_t *ndrvs;     /* list of net drivers */
 struct net_pin_t *nlds;      /* list of loads driven by net */
 struct traux_t *ntraux;      /* aux tran channel record */  
 byte *nchgaction;            /* per inst. (or 0 only) assign action bits */
 /* SJM 07/24/00 - need both add driver and soft force for wires */
 struct dltevlst_t **regwir_putv_tedlst; /*  per inst (all bits) ev list */
 struct vpi_drv_t **vpi_ndrvs;/* ptr to table of ptrs to drivers */
 union nqc_u nu2;             /* union - comp. nnxt, and qcval */  
 struct attr_t *nattrs;       /* table of net's attributes */
};

/* symbol table contents and avl tree link info */
struct sy_t {
 unsigned sytyp : 8;
 unsigned sydecl : 1;         /* symbol has been declared */
 unsigned syundefmod : 1;     /* undefined mod or udp */ 
 unsigned cfg_needed : 1;     /* T => symbol is used in config */
 unsigned sy_impldecl : 1;    /* for wire, declared implicitly */
 unsigned sy_argsmac : 1;     /* macro symbol with arguments */
 unsigned sy_giabase : 1;     /* T => symbol is g/i array base */
 unsigned syfnam_ind : 16;    /* file symbol defined in */
 int32 sylin_cnt;             /* def. line no. */
 char *synam;
 union el_u el;
 struct sy_t *spltsy;
};

/* separate work array storage of global component names */
struct glbncmp_t {
 int32 gncsiz;                /* size of largest alloc component (reused) */
 char *gncmp;
};

union targ_u {
 struct itree_t *targitp;     /* only for rooted - one target itree place */
 struct itree_t **uprel_itps; /* parallel to mod itps dest itree table */
}; 

/* global ref. info - master global rec. also used during simulation */
struct gref_t {
 unsigned upwards_rel : 1;    /* T => upwards relative form path */
 unsigned is_upreltsk : 1;    /* T => upward rel and special just task name */
 unsigned is_rooted : 1;      /* T => path starts at top module root */
 unsigned path_has_isel : 1;  /* T => down or root path has inst arr select */
 unsigned pathisel_done : 1;  /* T => for upward rel., grcmps fixed */
 unsigned gr_inst_ok : 1;     /* T => xmr can be inst. maybe just 1 comp. */
 unsigned gr_defparam : 1;    /* T => allow parameter as net tail */
 unsigned gr_gone : 1;        /* T => for defparam processing - now gone */
 unsigned gr_err : 1;         /* T => global is incorrect */
 unsigned grfnam_ind : 16;    /* file location of ref. */
 int32 grflin_cnt;            /* line location of ref. */
 int32 last_gri;              /* last - 1 (1 before target) symbol in path */

 char *gnam;                  /* name */
 struct expr_t *gxndp;        /* back link to expr. node containing xmr */
 struct expr_t *glbref;       /* original gref expr for inst. array eval. */
 struct symtab_t *grsytp;     /* symtab where ref. appears */
 struct mod_t *targmdp;       /* module target declared in */
 struct task_t *targtskp;     /* if non NULL, task target declared in */
 struct mod_t *gin_mdp;       /* place where gref used, for netchg prop. */
 struct sy_t **grcmps;        /* table of path symbols (prep fixes ia sels) */
 struct expr_t **grxcmps;     /* table of arr of inst path symbol sel exprs */
 union targ_u targu;          /* up rel. itp list or rooted 1 target itp */
 struct sy_t *targsyp;        /* actual end path symbol in target module */
 struct gref_t *spltgrp;      /* ptr from split off grp back to original */
 /* SJM 08/18/99 - now using gref tables no next */
};

/* symbol table - separate for each module */
/* needs union here ? */
struct symtab_t {
 unsigned freezes : 1;
 unsigned numsyms : 31;
 struct tnode_t *n_head;
 struct sy_t **stsyms;        /* non null means frozed form */
 struct symtab_t *sytpar;     /* parent is enclosing scope */
 struct symtab_t *sytsib;     /* sibling is disjoint32 same level scope */
 struct symtab_t *sytofs;     /* offspring is list hdr of enclosed scopes */
 struct sy_t *sypofsyt;       /* symbol this table is symbol table of */
};

/* list for saving timing check chk event changes for end of time slot */
struct tc_pendlst_t {
 struct chktchg_t *tc_chkchgp;
 struct itree_t *tc_itp;
 struct tc_pendlst_t *tc_plnxt;
};

union tchg_u {
 struct tchk_t *chgtcp;
 struct spcpth_t *chgpthp;
};

/* time of last chg net pin record - np from context, bit from npp rec. */
struct tchg_t {
 union tchg_u chgu;           /* union pointer to associated master */
 byte *oldval;                /* per inst. old value of source bit */
 word64 *lastchg;             /* per inst. last chg time for this 1 bit */
};

/* time check change record - access tchk record thru start change */
struct chktchg_t {
 struct tchg_t *startchgp;    /* corresponding start wire time change */
 byte *chkoldval;
 word64 *chklastchg;          /* per inst. last chg time for check event */
};

/* simulation per bit path delay element */
struct pthdst_t {
 struct tchg_t *pstchgp;      /* path src time change and indir. to path */ 
 struct pthdst_t *pdnxt;      /* destination bit can have >1 sources */
};

/* one path elment */
struct pathel_t {
 int32 pthi1, pthi2;          /* specparam then index number value */
 struct net_t *penp;          /* path input/out wire - NULL if removed */
}; 

/* work data struct for checking multiple same sdpds */
struct xpnd_pthel_t {
 unsigned in_equiv_set : 1;
 struct spcpth_t *pthp;
 int32 peii, peoi; 
 struct xpnd_pthel_t *xpthnxt;
};

struct spcpth_t {
 unsigned pthtyp : 2;         /* type of path (full or *>) */ 
 unsigned pth_gone : 1;       /* must ignore because of serious error */
 unsigned pth_as_xprs : 1;    /* T => fixup not done path els still exprs */
 unsigned pth_delrep : 5;     /* delay representation for path */
 unsigned pthpolar : 2;       /* simple path polarity (no sim meaning) */
 unsigned pthedge : 8;        /* if edge, edge sensitive path */
 unsigned dsrc_polar : 2;     /* if edge sensitive polarity */
 unsigned pth_ifnone : 1;     /* T => condition is ifnone and other sdpds */
 unsigned pth_0del_rem : 1;   /* T => removed from 0 delay */ 
 struct sy_t *pthsym;         /* built symbol for identifying path */
 int32 last_pein;
 int32 last_peout;
 struct pathel_t *peins;      /* array of path input elements */
 struct pathel_t *peouts;     /* array of path output elements */
 union del_u pth_du;          /* delay value */
 struct expr_t *datasrcx;     /* non sim polarity expr, can be FCCOM list */
 struct expr_t *pthcondx;     /* for if (cond) sdpd, the cond expr. */
 struct spcpth_t *spcpthnxt;
};

/* timing check system task master record */
struct tchk_t {
 unsigned tchktyp : 8;        /* timing check type */
 unsigned tc_delrep : 5;      /* limit delay type (will be same for both) */
 unsigned tc_delrep2 : 5;     /* type of 2nd limit if present */ 
 unsigned tc_gone : 1;        /* must ignore because of error */
 unsigned tc_supofsuphld : 1; /* setup (added) part of setup hold */
 unsigned tc_recofrecrem : 1; /* recovery (added) part of recrem */
 unsigned tc_haslim2 : 1;     /* lim2 value present */
 unsigned startedge : 8;      /* edge byte (pli 1 bit per rf coding) */
 unsigned chkedge : 8;        /* edge byte (pli 1 bit per rf coding) */
 struct sy_t *tcsym;          /* symbol with constructed name for del set */
 struct expr_t *startxp;      /* event (reference) expr */
 struct expr_t *startcondx;   /* &&& ref. event or edge path delay sel. */
 struct expr_t *chkxp;        /* event (data - check) expr */
 struct expr_t *chkcondx;     /* &&& data event or edge path delay sel. */
 union del_u tclim_du;        /* 1st limit as delay */
 union del_u tclim2_du;       /* 2nd optional limit as delay */
 struct net_t *ntfy_np;       /* notify wire (known during fix up) */
 struct tchk_t *tchknxt;      /* next in current specify block */
};

/* concatenation of all module specify items */
struct spfy_t {
 struct symtab_t *spfsyms;    /* specify symbol table for specparams */
 struct spcpth_t *spcpths;    /* delay paths */
 struct tchk_t *tchks;        /* timing checks */
 struct net_t *msprms;        /* module's specify parameters */
 int32 sprmnum;               /* number of specify parameters in table */
};

/* SJM 07/10/01 - d.s. for new delay net load propagation algorithm */
/* for inter module paths, each src-dest pair has one of these */
struct impth_t {
 unsigned impth_delrep : 8;   /* delrep */
 union del_u impth_du;        /* for inter mod non IS del for src/dst pair */
 struct net_t *snp;           /* source net */
 int32 sbi;                   /* and bit */
 struct itree_t *sitp;        /* and itree loc */
 word64 lastchg;              /* last chg time for this 1 pth bit of inst */
 struct impth_t *impthnxt;    /* for || paths dst has >1 src with diff del */
};

/* mipd npp elnpp field points to per bit array mipd records */
struct mipd_t {
 unsigned no_mipd : 1;        /* T => no mipd delay for this bit */
 unsigned pth_mipd : 1;       /* T => do not treat as src-dest inter mod pth */
 unsigned pb_mipd_delrep : 5; /* bit's delay type */ 
 union del_u pb_mipd_du;      /* bit's delay - max if src-dst impth */
 byte *oldvals;               /* per inst array of old values */
 i_tev_ndx *mipdschd_tevs;    /* per inst sched. event ndxes (-1 if none) */
 struct impth_t **impthtab;   /* per inst inter mod src-dst pth ptr list */
};

struct srtmp_t {
 int32 mppos;
 struct mod_pin_t *smp;
};

/* top level n.l. entry points - this is a real user defined module */
struct mod_t {
 unsigned minstnum : 2;       /* static inst no. 0, 1, 2 (more than once) */
 unsigned mhassts : 1;        /* T => module has strength wire(s) */
 unsigned msplit : 1;         /* T => module created from defparam splitting */
 unsigned mpndsplit : 1;      /* T => split off from # params */
 unsigned mhassplit : 1;      /* T => modules split off from this one */
 unsigned mgone : 1;          /* T => split from defparam but same params */
 unsigned msplitpth : 1;      /* T => module comp. of split off chain */
 unsigned mwiddetdone : 1;    /* T => mod width determing param marking done */
 unsigned mhas_widthdet : 1;  /* T => module has width determin param */
 unsigned mhas_indir_widdet : 1; /* T => module has indr width det param */
 unsigned mgiarngdone : 1;    /* T => mod in array of g/i rng marking done */
 unsigned mpndprm_ingirng : 1;/* T => module has pound param in gi range */ 
 unsigned mpnd_girng_done : 1;/* T => gi rng pound param eval finished */
 unsigned mhasisform : 1;     /* T => at least oe IS form constant in mod */
 unsigned mtime_units : 4;    /* no. of significant units of time rounded */
 unsigned mtime_prec : 4;     /* prec. - exp. 0 (1 sec.) - 15 (1 fs) */
 unsigned mno_unitcnv : 1;    /* F => need for delay unit conv. */
 unsigned m_iscell : 1;       /* T => module is asic style cell */
 unsigned m_hascells : 1;     /* T => module contains asic style cells */
 unsigned mod_hasdvars : 1;   /* T => for at least 1 inst. all vars dumped */
 unsigned mod_dvars_in_src : 1;/* T => all vars dumpved in src */
 unsigned mod_dumiact : 1;    /* T => dummy iact. info mod. */
 unsigned mod_rhs_param : 1;  /* T => mod has param def with rhs param */ 
 unsigned mod_hasbidtran : 1; /* T => has inout mod. port - tran chan */
 unsigned mod_hastran : 1;    /* T => has tran gate - tran chan */
 unsigned mod_gatetran : 1;   /* T => module has tran gate in it */
 unsigned mod_has_mipds : 1;  /* T => mod has SDF or vpi_ only set mipds */  
 unsigned mod_parms_gd : 1;   /* T => all dependent and pound params right */
 unsigned mod_1bcas : 1;      /* T => contains 1 bit ca - special for vpi */
 unsigned mod_lofp_decl : 1;  /* T => port decl uses list of ports form */ 
 unsigned mod_dfltntyp : 4;   /* T => for vpi, from directive net dflt type */
 unsigned mod_uncdrv : 8;     /* for vpi, mod val of unc drive directive */ 
 unsigned m_inconfig : 1;     /* T => module is from a config file */
 unsigned cfg_scanned : 1;    /* T => module has been scanned by config */
 unsigned mhas_frcassgn : 1;  /* T => mod contains force/assign(s) */

 int32 flatinum;              /* num. of flattened instances of module */
 int32 mpnum;                 /* number of module ports */
 int32 minum;                 /* number of instances in module */
 int32 mgnum;                 /* number of gates in module */
 int32 mcanum;                /* number of non 1 bit cas in module */ 
 int32 mnnum;                 /* number of nets in module */  
 int32 mtotvarnum;            /* total number of variables mod+task */
 int32 mprmnum;               /* number of pound params defined */ 
 int32 mlocprmnum;            /* number of local params defined */ 
 int32 mlpcnt;                /* mod. inst. loop count (static level) */
 struct sy_t *msym;           /* symbol from separate module name space */
 int32 mod_last_lini;         /* line no. of end (mabye in next file) */
 int32 mod_last_ifi;          /* and file in case spans multiple */
 struct symtab_t *msymtab;    /* symbol table for objects in module */
 struct cfglib_t *mod_cfglbp; /* config lib this mod's cells bound using */
 struct mod_pin_t *mpins;     /* defined ports first list then arr. */
 struct gate_t *mgates;       /* array of gates, udps and assigns */
 struct giarr_t **mgarr;      /* parallel ptr array to mgates for gate arr. */
 struct conta_t *mcas;        /* module cont. assigns */
 struct inst_t *minsts;       /* array of module instance records */
 struct giarr_t **miarr;      /* parallel ptr array to minst for inst arr. */
 struct net_t *mlocprms;      /* local param definitions - list then tab */
 struct net_t *mprms;         /* local param definitions - list then tab */
 struct net_t *mnets;         /* list then array of nets in module */
 /* SJM 12/19/04 - only fixed ones from conn npin - vpi added malloced */
 struct net_pin_t *mnpins;    /* table of all npins in module */
 struct itree_t **moditps;    /* itnum to itree struct mapping table */
 struct mod_t *mnxt;          /* next defined module */

 /* new Verilog 2000 fields */
 struct attr_t *mattrs;       /* mod attrs - used only if inst has no attr */
 struct varinitlst_t *mvarinits; /* list of var (not net) init decls */
 i_tev_ndx **mgateout_cbs;    /* per gate ptr to per inst list of vc cbs */

 /* LOOKATME - these are compile only and can maybe be freed after fixup */
 int32 mversno;               /* version number for split modules */
 int32 lastinum;              /* while assigning inums - last used */
 struct mod_t *mlevnxt;       /* link to next same level module */
 struct mod_t *mspltmst;      /* pointer to defparam master if split off */
 struct mod_t *mpndspltmst;   /* pointer to pound master if split off */
 struct cell_t *mcells;       /* unordered list of module objects */
 struct srtmp_t *smpins;      /* sorted table of module pins */
 struct srcloc_t **iploctab;  /* || to insts ptr tab to tab of port locs */

 struct dfparam_t *mdfps;     /* modules defparams */
 struct task_t *mtasks;       /* tasks declared in module */
 struct ialst_t *ialst;       /* initial/always list */
 struct spfy_t *mspfy;        /* module's specify elements */
 char *mndvcodtab;            /* table of dmpvar codes for all nets */

 /* per module record tables - for optimized code and save/restart */
 struct expr_t *mexprtab;     /* per mod table of expr. records */    
 int32 mexprnum;              /* size of mod's expr tab */

 struct st_t *msttab;         /* per mod table of mod stmts */
 int32 mstnum;                /* size of mod's stmt tab */

 struct gref_t *mgrtab;       /* list of xmr's appearing in this mod. */
 int32 mgrnum;                /* number of grefs in module */
};

/*
 * run time only structures
 */
/* doubly linked list of events for cancelling some */ 
struct dltevlst_t {
 i_tev_ndx tevpi;
 struct dltevlst_t *telp, *terp;
};

/* singly linked list of events for cancel (short search from front) */ 
struct tevlst_t {
 i_tev_ndx tevpi;
 struct tevlst_t *telnxt;
};

/* threads - because associated with an event, inst. specific */
struct thread_t {
 unsigned tsk_stouts : 1;     /* T => when task returns has outs to store */
 unsigned th_dsable : 1;      /* thread done because disabled */
 unsigned th_rhsform : 1;     /* T => rhs form so value in thread record */
 unsigned th_fj : 1;          /* T => thread is fj - assoc. task is upward */
 unsigned th_postamble : 1;   /* T => pending task needs end store/trace */
 unsigned th_ialw : 1;        /* T => thread in initial/always list */
 unsigned thofscnt : 24;      /* num of still active fork-join offspring */
 word32 thenbl_sfnam_ind;     /* file index of place thread enabled from */
 int32 thenbl_slin_cnt;       /* and line number */

 struct st_t *thnxtstp;       /* next st. in thread to execute (resume pc) */
 /* tree of 2 way linked sibling subthread lists */
 struct thread_t *thpar;      /* thread that invoked this one */
 struct thread_t *thright;    /* double linked same level right ptr */
 struct thread_t *thleft;     /* double linked same level right ptr */
 struct thread_t *thofs;      /* list of threads, enabled by this one */
 struct tskthrd_t *tthlst;    /* ptr to this task assoc. thrd list el. */
 i_tev_ndx thdtevi;           /* if thread has scheduled event, ndx of it */
 struct task_t *assoc_tsk;    /* if created from task, connection to it */

 struct delctrl_t *th_dctp;   /* if armed but not trig. ev ctrl, ptr to it */
 word32 *th_rhswp;            /* for rhs delay/event controls, rhs value */  
 int32 th_rhswlen;            /* word32 length of rhs value for disable */
 struct itree_t *th_itp;      /* itree loc. thread runs in */
 struct hctrl_t *th_hctrl;    /* for iact, pointer to control rec. */
};

union ten_u {
 struct net_t *np;
 struct mod_pin_t *mpp;
};

/* net bit or MIPD event storage - must be alloced and freed as used */
struct tenp_t {
 int32 nbi;
 union ten_u tenu;
};

/* storage for scheduled but not matured non blocking assign event */ 
struct tenbpa_t {
 word32 *nbawp;
 struct st_t *nbastp;
 struct expr_t *nblhsxp;      /* if var ndx expr (maybe cat) const ndx copy */
 struct delctrl_t *nbdctp;
};

/* tf strdelputp event storage - one schedule change (tev has instance) */
struct tedputp_t {
 struct tfrec_t *tedtfrp;
 int32 tedpnum;
 word32 *tedwp;               /* point to both half - know exact lhs word32 len */
};

/* vpi put value event storage (for both reg net driver assign */
struct teputv_t { 
 int32 nbi;
 int32 di;                    /* for mult. drivers the ndx no. in table */
 struct net_t *np;
 word32 *putv_wp;
 struct h_t *evnt_hp;         /* ptr back to event for cancelling */
};

union te_u {
 struct thread_t *tethrd;
 struct gate_t *tegp;
 struct conta_t *tecap;
 struct tenp_t *tenp;
 struct tenbpa_t *tenbpa;
 struct tfrec_t *tetfrec;
 struct tedputp_t *tedputp;
 struct teputv_t *teputvp; 
 struct h_t *tehp;
};

struct tev_t {
 unsigned tetyp : 5;
 unsigned te_cancel : 1;      /* this event canceled */
 unsigned nb_evctrl : 1;      /* for non blocking event is event control */
 unsigned te_trdecay : 1;     /* event is trireg decay */
 unsigned gev_acc : 1;        /* accelerated gate value assign */
 unsigned vpi_reschd : 1;     /* T => vpi_ time cb moves to other slot part */
 unsigned vpi_onfront : 1;    /* T => cb sim time start, add on fut. front */
 unsigned vpi_regwir_putv : 1;/* T => putv to reg/wire soft force */
 unsigned outv : 8;           /* for gate output value */
 word64 etime;                /* absolute time not delay */
 struct itree_t *teitp;       /* instance tree inst event executes in */
 union te_u tu;
 i_tev_ndx tenxti;
};

struct telhdr_t {
 i_tev_ndx te_hdri, te_endi;
 int32 num_events;
};

/* list element for pending net changes to process after event done */
struct nchglst_t {
 int32 delayed_mipd;          /* T - 2nd after schedule from ev process */
 int32 bi1, bi2;
 struct net_t *chgnp;
 struct itree_t *nchg_itp;
 struct nchglst_t *nchglnxt;
};

/* b tree (timing queue) structs */
union bofs_u {
 struct bt_t *btofs;
 struct telhdr_t *telp;
};

struct bt_t {
 unsigned bttyp : 8;
 unsigned btnfill : 8;        /* no. of filled nodes in btree node */
 word64 btltim;               /* time of highest left subtree */
 union bofs_u ofsu;
 struct bt_t *btnxt;
};

struct fbel_t {
 struct fbel_t *fbnxt;
};

/* table of separate c funcs - usually only one unless too large */
/* SJM ### ??? why is this not referenced? */
struct cproc_t {
 int32 cpnum;                 /* number c proc name suffix */
 void (*funcptr)();           /* ptr to dlsym found c routine */ 
 void *restabptr;             /* ptr to extrn in c resume label table */
 int32 last_u32var;           /* highest num u32 var to decl in c proc */
 struct cproctab_t *cptabnxt; 
};

struct contab_info_t {
 int32 cwid;
 int32 xvi;
 struct contab_info_t *cnxt;
};

/* various simulation counting variables */
extern char *__vers;          /* actual product info */
extern char *__vers2;
extern char *__ofdt;
extern char *__platform;
extern char *__start_sp, *__end_sp;/* pointer for "big" memory allocator */
extern char *__pvdate;        /* date in unix form */
extern char *__pv_timestamp;  /* timestamp for design */
extern time_t __start_time;   /* start wall clock times in secs. */
extern time_t __start_mstime; /* start millisecond part */
extern time_t __end_comp_time;/* end of compilation time */
extern time_t __end_comp_mstime;
extern time_t __end_prep_time;/* end of preparation time */
extern time_t __end_prep_mstime;
extern time_t __end_time;     /* start and end wall clock times */
extern time_t __end_mstime;   /* start and end wall clock times */

/* various file variables and global data base flags (i/o vars) */
extern char **__in_fils;      /* malloced input file list from args */
extern int32 __siz_in_fils;   /* current size of input files */
extern int32 __last_inf;      /* last input file in list */
extern int32 __last_optf;     /* last option file */
extern int32 __last_lbf;      /* last lib/inc file (starts last_inf + 1) */
extern int32 __last_srcf;     /* last src containing file for debugger */
extern struct incloc_t *__inclst_hdr; /* header of included files list */
extern struct incloc_t *__inclst_end; /* end of included files list */
extern struct filpos_t *__filpostab; /* in fils size tab of file line pos. */
extern FILE *__save_log_s;    /* if $nolog executed, value to use if log */
extern int32 __echo_iactcmds_tolog; /* T => echo interactive cmds to log file */
extern FILE *__save_key_s;    /* if $nolog executed, value to use if log */
extern int32 __nokey_seen;    /* $nokey executed and no key */
extern FILE *__in_s;
extern FILE *__log_s;
extern FILE *__cmd_s;         /* command file source or null for tty */ 
extern FILE *__key_s;         /* if key_s nil but key_fnam not, must open */
extern struct optlst_t *__opt_hdr;   /* header of expanded option list */
extern struct optlst_t *__opt_end;   /* wrk end of expanded option list */
extern word32 *__wsupptab;    /* tab (1 bit/msg) for warn and iact suppress */
extern char *__blnkline;      /* work blank line */
extern char __pv_homedir[RECLEN]; /* home dir - . if HOME env. not set */
extern struct mcchan_t __mulchan_tab[32];/* mc desc. tab (32 built in Ver) */ 
extern struct fiofd_t **__fio_fdtab; /* array of ptrs to file io stream */
extern char *__fiolp;         /* fio file input work string ptr */
extern char *__fiofp;         /* fio file input work fmt string ptr */
extern long __scanf_pos;      /* byte offset position of scanf in file */
extern sighandler *__old_int_sig;  /* value of quit (^c) signal on entry */
extern int32 __force_base;    /* for output force base if not BASENONE */
extern struct vinstk_t **__vinstk;/* open file/macro list in stack form */
extern struct vinstk_t *__visp;/* pointer to top of open input stack */
extern int32 __vin_top;       /* index of top of current file stack */
extern char *__log_fnam;      /* log file for all terminal output */
extern char *__sdf_opt_log_fnam; /* sdf log file if set by cmd arg */
extern FILE *__sdf_opt_log_s; /* and open file ptr */
extern int32 __sdf_no_warns;  /* T => don't print any SDF warning msgs */
extern int32 __sdf_no_errs;   /* T => don't print any SDF error msgs */
extern int32 __sdf_from_cmdarg; /* T => SDF annotation call from cmd option */
extern char *__cmd_fnam;      /* command interact. input file name */
extern char *__cmd_start_fnam;/* -i startup interactive input file name */
extern char *__key_fnam;      /* key file name and stream */
extern FILE *__tr_s;          /* trace output file - can be stdout */
extern char *__tr_fnam;
extern int32 __cmd_ifi;       /* constant command in_fils index */
extern char *__lic_path;      /* +licpath [path] if option used */
extern FILE *__sdf_s;         /* current SDF back annotate file/stream */
extern struct sdfnamlst_t *__sdflst; /* list of sdf annotate option files */
extern int32 __sdf_sav_enum;  /* saved error num. for annotate inhibit */
extern int32 __sdf_sav_maxerrs; /* saved max errors so won't stop */
extern int32 __has_sdfann_calls;/* T => no sdf annotate systsk calls in src */ 
extern int32 __sdf_active;    /* T => annotating SDF - for PLI erro code  */
extern struct mod_t *__sdf_mdp; /* special sdf context mod */

/* cfg variables */
extern char *__cmdl_library;  /* library name to file off the command line */
extern struct mapfiles_t *__map_files_hd; /* hdr of map files from cmd args */
extern struct mapfiles_t *__map_files_tail; /* end of map file list */
extern struct cfglib_t *__cfglib_hd; /* head of list of libs for cfg */
extern struct cfglib_t *__cfglib_tail; /* and tail */
extern struct cfg_t *__cfg_hd;/* head of list of cfgs */
extern struct cfg_t *__cur_cfg;/* current cfg */
extern struct mod_t *__cfg_mdp;/* SJM - remove me - why global */
extern char **__bind_inam_comptab;/* during cfg binding, comp descent comps */ 
extern int32 __siz_bind_comps;/* current malloc size of table */
extern int32 __last_bind_comp_ndx;/* last currently used comp end index */ 
extern int32 __cfg_verbose;   /* T => emit cfg reading verbose messages */ 

/* file variables */
extern int32 __cur_infi;      /* index in in_fils of current file */
extern struct optlst_t *__new_opt_hdr;/* header of expanded option list */
extern struct optlst_t *__new_opt_end;/* wrk end of expanded option list */
extern struct optlst_t *__log_olp;   /* log option, nil if none */
extern struct optlst_t *__help_olp;  /* help option, nil if none */
extern struct optlst_t *__quiet_olp; /* quiet option, nil if none */
extern struct optlst_t *__verb_olp;  /* verbose option, nil if none */  
extern int32 __vpi_argc;      /* global arg count for vpi */
extern char **__vpi_argv;     /* global arg array for vpi */
extern char *__vpi_argv0;     /* argv execed program name */
extern char *__cur_fnam;      /* being read file name for errors */
extern int32 __cur_fnam_ind;  /* index in in_fils of cur_fnam */
extern int32 __sfnam_ind;     /* global file index for current stmt. */
extern int32 __slin_cnt;      /* global line no. for currently check stmt */
extern int32 __vpifnam_ind;   /* vpi_ global current file index */
extern int32 __vpilin_cnt;    /* vpi_ global current line no. */
extern struct expr_t *__srm_xp; /* current string 'file' for sreadmem */
extern char *__srm_strp;      /* char. pos. in sreadmem string */
extern char *__srm_strp_beg;  /* work alloced location for sreadmem string */
extern int32 __srm_strp_len;  /* alloced length */
extern int32 __srm_stargi;    /* current string position number */
extern int32 __in_ifdef_level;/* current processing `ifdef level */ 
extern int32 __ifdef_skipping;/* T = skipping not included ifdef section */
extern char *__langstr;       /* work string for `language */
extern int32 __doing_langdir; /* T => processing `language directive */ 
extern int32 __rding_top_level; /* T => reading outside top level construct */

/* variables for batch tracing */
extern word64 __last_trtime;  /* last trace statement time */
extern word64 __last_evtrtime;/* last event trace time */
extern struct itree_t *__last_tritp;/* last event traced inst. itree loc. */ 

/* command processing variables and temps */
extern int32 __pv_err_cnt, __pv_warn_cnt; /* error counts */
extern int32 __inform_cnt;    /* number of informs */
extern int32 __outlinpos;     /* current trunc. output line pos. */
extern long __mem_use;        /* counts allocated mem for debugging */
extern long __mem_allocated;  /* bytes allocated */
extern long __mem_freed;      /* bytes freed */
extern long __memstr_use;     /* counts allocated string mem for debugging */
extern long __arrvmem_use;    /* allocated bytes for Verilog arrays */
extern long __mem_udpuse;     /* number of bytes used by udp tables */
extern word64 __tim_zero;     /* place for time of constant 0 */
extern int32 __num_glbs;      /* total no. of globals in design */
extern int32 __num_inmodglbs; /* glbs thar resolve to intra module refs. */
extern int32 __num_uprel_glbs;/* number of upward relative globals */
extern int32 __nets_removable;/* flat no. of deletable nets */
extern int32 __flnets_removable;/* removable static nets */
extern int32 __gates_removable; /* removable static gates */
extern int32 __flgates_removable; /* flat no. of deletable gates */
extern int32 __contas_removable; /* removabale static cont. assigns */
extern int32 __flcontas_removable; /* flat no. of deletable cont. assigns */

/* special allocate free variables */
extern struct ncablk_t *__hdr_ncablks; /* blocks used for ncomp recs */
extern int32 __ncablk_nxti;   /* index of next free pos. */
extern struct cpblk_t *__hdr_cpblks; /* blocks used for cell recs*/
extern int32 __cpblk_nxti;    /* index of next free pos. */
extern struct cppblk_t *__hdr_cppblks; /* blocks used for cell pin recs*/
extern int32 __cppblk_nxti;   /* index of next free pos. */
extern struct tnblk_t *__hdr_tnblks;  /* blocks of symb table tree nodes */
extern int32 __tnblk_nxti;    /* index of next free pos. */ 
extern struct cpnblk_t *__hdr_cpnblks;  /* blocks of explicit cell pnames */

/* source processing variables */
extern int32 __lin_cnt;       /* line number while reading a file */
extern int32 __saverr_cnt;    /* counter to inhibit more than a err in xpr */
extern int32 __max_errors;    /* maximum errors before stopping */
extern int32 __rding_comment; /* flag so comment non printable chars ok */
extern int32 __total_rd_lines;/* total number of lines read */
extern int32 __total_lang_dirs; /* total num `language directives read */

/* booleans for program options (flags) */
extern int32 __verbose;       /* T => emit various extra messages */
extern int32 __quiet_msgs;    /* T => do not emit msgs just errors */
extern int32 __no_warns;      /* T => don't print warning msgs */
extern int32 __no_errs;       /* T => don't print error msgs */
extern int32 __no_informs;    /* T => don't print inform msgs (dflt) */
extern int32 __debug_flg;     /* T => turn on debugging output */
extern int32 __opt_debug_flg; /* T => turn on vm compiler debugging output */
extern int32 __st_tracing;    /* T => trace statement execution */
extern int32 __ev_tracing;    /* T => trace event schedules and processes */
extern int32 __pth_tracing;   /* T => trace path delays in detail */
extern int32 __prt_stats;     /* T => print design statics tables */
extern int32 __prt_allstats;  /* T => print design and mod content tabs */
extern int32 __show_cancel_e; /* T => chg val to x on pulse cancelled event */
extern int32 __showe_onevent; /* T => if showing cancel e, drive x on event */
extern int32 __warn_cancel_e; /* T => emit warn cancel_e (indep. of cancel) */
extern int32 __rm_gate_pnd0s; /* T => remove #0 from all gates */
extern int32 __rm_path_pnd0s; /* T => (default) remove all 0 delay paths */
extern int32 __dmpvars_all;   /* T => dumpvars all variables */

/* command option booleans */
extern int32 __lib_verbose;   /* T => emit src.file/lib source messages */
extern int32 __sdf_verbose;   /* T => emit msgs for SDF annotated delays */
extern int32 __switch_verbose;/* T => emit msgs for switch/tran chan build */
extern int32 __chg_portdir;   /* T => chg port dir to bid. for XL compat */
extern int32 __nb_sep_queue;  /* F => old un-seperated nb in pnd0 queue */
extern int32 __decompile;     /* T => decompile and print Verilog source */
extern int32 __compile_only;  /* T => check syntax (inc. lib.) no quads */
extern int32 __parse_only;    /* T => first pass parse only to chk sep mod */
extern int32 __dflt_ntyp;     /* Verilog wire type for normal nets */
extern int32 __mintypmax_sel; /* for (e:e:e) expressions value to use */
extern int32 __sdf_mintypmax_sel; /* min:nom_max over-ride for $sdf_annotate */
extern int32 __gateeater_on;  /* T => attempt to remove (disconnect) gates */ 
extern int32 __no_expand;     /* T => make all wire vectors vectored */
extern int32 __in_cell_region;/* T => turn on cell bit for every module */
extern int32 __unconn_drive;  /* if none TOK_NONE else PULL 0 or PULL 1 */
extern int32 __no_specify;    /* T => check but no simulate with specify */
extern int32 __no_tchks;      /* T => check but no simulate with tim chks */
extern int32 __lib_are_cells; /* T => if in lib, the mark as cell (dflt.) */
extern int32 __design_has_cells;/* T => cells somewhere in design */
extern int32 __accelerate;    /* T => use short circuits g/prt code if can */
extern int32 __pli_keep_src;  /* T => keep more source stmt info for pli */
extern int32 __use_impthdels; /* T => use src-dst im path dels */

/* source input variables and temps */
extern char __lasttoken[IDLEN];/* current last pushed back symbol name */
extern char __token[IDLEN];   /* current symbol (in canonical (lc) form) */
extern int32 __toktyp;        /* place for current toktyp value */
extern int32 __lasttoktyp;    /* push back token type (= UNDEF if none) */
extern int32 __last_attr_prefix;/* push back pending attr prefix state */
extern int32 __itoklen;       /* current number token bit length */
extern int32 __itoksized;     /* T => token is sized */
extern int32 __itokbase;      /* base constant for number token */
extern int32 __itoksizdflt;   /* '[base] form with width (uses dflt.) */
extern int32 __itok_signed;   /* T => token is signed number */
extern double __itok_realval; /* actual scannoer double val */
extern char *__strtoken;      /* growable token to hold string */
extern int32 __strtok_wid;    /* current size of string token */    
extern char *__numtoken;      /* growable token to hold numbers */
extern int32 __numtok_wid;    /* current size of number token */    
extern int32 __letendnum_state; /* T => letter can end unsized num. */
extern int32 __macro_sep_width; /* T => possible beginning of macro 2 tok num */
extern int32 __maybe_2tok_sized_num; /* T => seeing number after macro num */
extern int32 __macro_sav_nwid;/* value of saved first tok width */
extern int32 __first_linetok; /* T => token first on line */
extern int32 __file_just_op;  /* T => new file and no token yet returned */
extern int32 __first_num_eol; /* T => first tok because number ended line */
extern char *__macwrkstr;     /* work string for macros */ 
extern int32 __mac_line_len;  /* actual length of macro line in wrk str */   
extern int32 __macwrklen;     /* allocated len of mac. work string */
extern struct macarg_t *__macarg_hdr; /* hdr of list of format mac. args */
extern int32 __macbs_flag;    /* T=> 8'h`DEFINE catch multiple bases errors */
extern char *__attrwrkstr;    /* work string for attributes */
extern int32 __attr_line_len; /* actual length of attribute string */ 
extern int32 __attrwrklen;    /* alloced len of attr work string - grows */
extern char *__attrparsestr;  /* string to parse attr out of */
extern int32 __attrparsestrlen; /* string to parse attr out of */
extern int32 __attr_prefix;   /* T => token has attribute prefix */
extern int32 __attr_fnam_ind; /* location of attr inst. */
extern int32 __attr_lin_cnt;  /* location of attr inst. */
extern struct attr_t __wrk_attr; /* latest read attribute */
extern char *__xs, *__xs2;    /* places to put expr to str trunc messages */
extern int32 __pv_ctv;        /* tmp for white space skipping macros */
extern int32 __syncto_class;  /* token class sync skipping halted at */
extern char *__exprline;      /* expr. output line work string */
extern int32 __exprlinelen;   /* expr. line length */
extern int32 __cur_sofs;      /* ndx of next ofs (position) in expr line */
extern word32 *__acwrk;       /* a value work string for scanning number */
extern word32 *__bcwrk;       /* b value work string for scanning number */
extern word32 __addrtmp[2];   /* up to 32 bit temp with addr. */
extern int32 __abwrkwlen;     /* current acwrk a half length in words */
extern char __portnam[IDLEN];
extern char __pv_varnam[IDLEN]; /* variable name */
extern int32 __expr_is_lval;  /* T => parsing proc. assign lhs */
extern int32 __allow_scope_var; /* T => process systask arg can be scope */

/* vars needed for pushing back numbers (see var. comment) */
extern int32 __lastitokbase;
extern int32 __lastitoksized;
extern int32 __lastitoksizdflt;
extern int32 __lastitok_signed;
extern int32 __lastitoklen;
extern word32 *__lastacwrk;   /* special malloced push back num value */
extern word32 *__lastbcwrk; 
extern double __lastitok_realval;

/* the module and module subtask specific work variables */
extern struct mod_t *__oinst_mod;/* ptr. to old current module for copying */
extern struct mod_t *__end_mdp; /* end of module def. list */
extern struct cell_t *__end_cp; /* end of module inst. list */
extern int32 __cp_num;          /* counter for unnamed gate/inst pos. */ 
extern struct conta_t *__end_ca; /* end of module conta list */
extern int32 __conta_num;     /* counter for building symbol for conta */
extern struct varinitlst_t *__end_mod_varinitlst; /* end of mod var inits */
extern struct dfparam_t *__end_dfp;/* module current end of defparam list */
extern struct task_pin_t *__end_tpp; /* end of task port list */
extern struct task_t *__end_tbp;/* end of top level task/functions/blocks */
extern struct task_t *__cur_tsk;/* ptr. to current task */
extern struct net_t *__end_paramnp; /* end of ordered parm decl. list */
extern struct net_t *__end_loc_paramnp; /* end of ordered parm loc decl. list */
extern struct net_t *__end_impparamnp; /* end of ordered imprt parm decl lst */
extern struct net_t *__end_glbparamnp; /* end of ordered glb parm decl. lst */
extern struct net_t *__end_tskparamnp; /* end of task param decl. list */
extern struct net_t *__end_tsk_loc_paramnp; /* end of task param decl. list */
extern struct ialst_t *__end_ialst; /* end of module initial/always list */
extern struct gref_t *__grwrktab;  /* work table for building mod glbs */
extern int32 __grwrktabsiz;        /* its size */
extern int32 __grwrknum;      /* current number of glbs in work table */ 
extern int32 __cur_declobj;   /* token type of declared mod or task */
extern int32 __pv_stlevel;    /* tmp. for current stmt nesting level */
extern int32 __design_no_strens;/* T => no strengths used in design */
extern int32 __strenprop_chg; /* during propagate pass at least one chged */
extern int32 __splitting;     /* T => in process of splitting module */
extern int32 __processing_pnd0s;/* T => in time unit, in end #0 region */
extern struct dce_expr_t *__cur_dce_expr; /* glb for edge events eval expr */
extern int32 __lofp_port_decls; /* T => exclusive hdr port decls appeared */ 
extern struct exprlst_t *__impl_evlst_hd; /* hdr of impl @(*) ev expr list */
extern struct exprlst_t *__impl_evlst_tail; /* and its tail */
extern int32 __canbe_impl_evctrl; /* glb switch to allow @(*) as ev ctrl */

/* variables for dumpvars */
extern int32 __dv_seen;       /* dumpvars seen but not yet setup */
extern int32 __dv_state;      /* processing state of dumpvars */
extern word64 __dv_calltime;  /* time dump var. first (and only) called */
extern int32 __dv_dumplimit_size; /* user set limit of dv file size (0 none) */
extern int32 __dv_file_size;  /* current size of dumpvars file */
extern int32 __dv_time_emitted; /* flag to stop repeated same #[time] */
extern char *__dv_fnam;       /* name of dumpvars output file */
extern int32 __dv_func;       /* global set with type of dumpvar dumping */
extern struct mdvmast_t *__dv_hdr; /* hdr of mast dumpvar rec. list */
extern struct mdvmast_t *__dv_end; /* end of dumpvar rec. list */
extern struct dvchgnets_t *__dv_netfreelst; /* free list of time var chges */
extern int32 __dv_fd;         /* file number of dmpvars fd */
extern char *__dv_buffer;     /* buffer to speed up dumpvars output */
extern int32 __dv_nxti;       /* next free location */
extern int32 __dv_outlinpos;  /* line postion in dump vars file */ 
extern int32 __next_dvnum;    /* highest so far used dumpvars number */
extern struct dvchgnets_t *__dv_chgnethdr;  /* curr. time var chg list hdr */
extern int32 __dv_isall_form; /* T doing all of design dumpvar setup */
extern int32 __dv_allform_insrc;/* T dumpvars all form in source */

/* time scale - precision variables */
extern word32 __cur_units;    /* current units (0 (1s) - 15 (1ft) */ 
extern word32 __cur_prec;     /* current digits of precision (0-15) */
extern word32 __des_timeprec; /* assume -, 0-15 design sim. tick prec. */
extern word32 __tfmt_units;   /* %t output units (also interact. units) */
extern word32 __tfmt_precunits;/* %t number of prec. digits */
extern int32 __des_has_timescales;/* T => design has at least one timescale */
extern char *__tfmt_suf;      /* suffix for %t */
extern int32 __tfmt_minfwid;  /* minimum field width for %t */
extern word64 __itoticks_tab[16];/* table of scales amount from prec. */
extern char __timstr_unitsuf[4];/* to_timstr units suffix if needed */
extern word64 __timstr_mult;  /* multiplier if needed */
extern int32  __nd_timstr_suf;/* T => need to_timstr units */

/* veriusertfs pli user function and task work variables */ 
/* SJM 07/16/02 - need internal veriuser tfs for new +loadpli1 option */
extern struct t_tfcell *__shadow_veriusertfs; /* internal copy of table */
extern int32 __last_veriusertf; /* last user veriusertfs tf number */
extern struct tfinst_t *__tfinst;/* current tf_ inst loc. */
extern struct tfrec_t *__tfrec;/* current tf_ record */ 
extern struct dceauxlst_t *__pvc_dcehdr; /* header of current pvc dces */
extern struct tfrec_t *__tfrec_hdr; /* header of design wide tfrec list */ 
extern struct tfrec_t *__tfrec_end; /* last el of design wide tfrec list */ 
extern i_tev_ndx __tehdr_rosynci; /* hdr ndx of slot end ro sync ev lst */ 
extern i_tev_ndx __teend_rosynci; /* end of slot end ro sync ev lst */ 
extern int32 __now_resetting; /* reset in progress - for cbs and misctf */  
extern int32 __rosync_slot;   /* T => processing tf or vpi  ro synch events */
extern struct loadpli_t *__pli1_dynlib_hd; /* hd of ld pli1 dynamic lb list */
extern struct loadpli_t *__pli1_dynlib_end; /* and its end */

/* vpi_ work variables */
extern int32 __last_systf;    /* last vpi_ registered sytfs number */
extern int32 __num_vpi_force_cbs; /* number of registered vpi force cbs */
extern int32 __vpi_force_cb_always; /* T => always call back on force */
extern int32 __num_vpi_rel_cbs; /* number of registered vpi rel cbs */
extern int32 __vpi_rel_cb_always; /* T => always call back on release */
extern int32 __allforce_cbs_off; /* T => can't reenter any of all force cbs */
extern int32 __allrel_cbs_off;/* T => can't reenter any of all release cbs */
extern char *__wrks1;         /* work string - can not use xs if func */
extern char *__wrks2;
extern char __wrk_vpiemsg[IDLEN];/* error msg. work string */
extern char __wrk_vpiget_str[IDLEN];/* standard required vpi get str string */
extern char __wrk_vpi_product[256];/* product version */
extern char __wrk_vpi_errcode[256];/* error codes are Cver err num as str */
extern double __wrk_vpi_timedbl;/* time double for vpi error rec */
extern char *__wrkvalbufp;    /* buf for vpi get value value_p contents */ 
extern int32 __wrkval_buflen; /* and current length */
extern int32 __vpi_vlog_start_done;/* T => startup done, no systf registering */
extern struct systftab_t *__systftab; /* table of vpi_ systf records */
extern int32 __size_systftab; /* current size of systf data rec. table */
extern struct xstk_t *__cur_sysf_xsp; /* tmp stk_t for vpi sysf ret val */
extern struct expr_t *__cur_sysf_expr;/* tmp calling expr. for vpi sysf*/
extern struct st_t *__cur_syst_stp; /* tmp stmt for vpi syst*/
extern struct dceauxlst_t *__cbvc_dcehdr; /* header of current vc cb dces */
extern struct rfcblst_t *__rel_allcb_hdr;
extern struct rfcblst_t *__rel_allcb_end;
extern struct rfcblst_t *__force_allcb_hdr;
extern struct rfcblst_t *__force_allcb_end;
extern i_tev_ndx *__vpicb_tehdri; /* hdr of fixed cb tev list - 1 per class */
extern i_tev_ndx *__vpicb_teendi; /* end of fixed cb tev list - 1 per class */
extern int32 __have_vpi_actions;/* some use of __vpi actions */
extern int32 __have_vpi_gateout_cbs;/* some use of gate out term cbs */
extern struct h_t *__vpi_hfree_hdr;  /* handle free list hdr */ 
extern struct hrec_t *__vpi_hrecfree_hdr;  /* handle record free list hdr */ 
extern struct cbrec_t *__vpi_cbrec_hdr; /* all cbs list header */
extern int32 __ithtsiz;       /* size of global work ld/drv handle table */
extern struct h_t *__ithtab;  /* and the work ld/drv handle table */
extern struct hrec_t *__ithrectab; /* and hrec contents of it */
extern int32 __ithtsiz2;      /* size of global work ld/drv handle table */
extern struct h_t *__ithtab2; /* 2nd work for in subtree handles */
extern struct hrec_t *__ithrectab2; /* and hrec contents of it */
extern struct vpisystf_t *__vpi_sysf_hdr; /* hdr sys func call src locs */
extern struct vpisystf_t *__vpi_syst_hdr; /* hdr sys task enable src locs */
extern int32 __in_vpi_errorcb;/* T => if sim ctrl, suppress error msg error */
extern int32 __vpierr_cb_active; /* T => at least one cbError reged */
extern int32 __acc_vpi_erroff;/* acc_ flag to stop internal acc_ error cbs */
extern int32 __errorcb_suppress_msg; /* T => sim control suppress error msg */
extern struct h_t *__cur_vpi_inst;
extern struct hrec_t *__cur_vpi_obj;
extern struct loadpli_t *__vpi_dynlib_hd; /* hd of ld vpi dynamic lib list */
extern struct loadpli_t *__vpi_dynlib_end; /* and its end */
extern struct dcevnt_t *__cbvc_causing_dcep; /* glb for vc cb if it is remed */

/* specify work variables */
extern struct spfy_t *__cur_spfy;/* current specify block */
extern struct spcpth_t *__end_spcpths; /* end of specify path st. list */
extern int32 __path_num;      /* counter for unnamed paths */
extern struct tchk_t *__end_tchks;/* end of specify time check st. list */
extern struct net_t *__end_msprms;/* end of specify specparam net list */
extern struct tchk_t *__cur_tchk;
extern int32 __tchk_num;      /* counter for unnamed paths */
extern struct symtab_t *__sav_spsytp;/* save loc. of sym tab in spfy sect. */

/* work compile global variables accessed by routines */
extern int32 __v1stren;       /* wire/inst. Ver. 1 strength */
extern int32 __v0stren;       /* wire/inst. Ver. 0 strength */
extern word32 __pr_iodir;     /* glb. for port ref. expr. I/O direction */
extern int32 __pr_wid;        /* global for total port ref. expr. width */
extern int32 __mpref_explicit;/* T => mod def header port ref explicit */
extern int32 __sym_is_new;    /* set when new symbol added */
extern struct sy_t **__wrkstab;/* malloced work symbol table area */
extern int32 __last_sy;       /* last symbol in work area */
extern int32 __mod_specparams;/* number of declared specparams in mod */
extern int32 __name_assigned_to;/* glb set if func. def. name assigned to */
extern struct sy_t *__locfnamsyp; /* place for func. def. chk func. symbol */
extern int32 __processing_func; /* T => prep or exec of function occuring */
extern struct st_t **__nbstk; /* func. nest nblock stack (nxt for exec) */
extern int32 __nbsti;      
extern struct sy_t *__ca1bit_syp; /* gmsym for 1 bit conta converted gate */
extern int32 __chking_conta;  /* T => checking a continuous assignment */
extern int32 __rhs_isgetpat;  /* T => flag for checking stylized getpat */
extern int32 __lhs_changed;   /* T => assignment changed lhs */
extern word32 __badind_a;     /* place for a part of in error index value */
extern word32 __badind_b;     /* and for b part */
extern int32 __badind_wid;    /* width for bad ind (<32 expr can eval to x) */
extern int32 __expr_has_real; /* T => know some real in expr. */
extern int32 __isform_bi_xvi; /* glbl for IS net pin bit index in contab */
extern int32 __lhsxpr_has_ndel; /* T => component wire of lhs has wire del */
extern int32 __checking_only; /* T => no error msg, looking for something */ 
extern int32 __task_has_tskcall;/* T => task calls other task (not name blk) */
extern int32 __task_has_delay;/* T => task call has del. needs thread */
extern int32 __func_has_fcall;/* T => func contains has non sys fcall */
extern int32 __iact_must_sched; /* T => iact stmt(s) have $stop or loop */
extern int32 __expr_rhs_decl; /* T current expr. is decl. not proc. rhs */ 
extern int32 __chg_rng_direct;/* T => change rng dir. for implicitly decl */
extern int32 __has_top_mtm;   /* T => for parameter rhs non () m:t:m */
extern int32 __nd_0width_catel_remove; /* fx3 file 0 width concat glb */

/* current Verilog module/task/block symbol environment */
extern struct symtab_t **__venviron;
extern int32 __top_sti;
extern struct symtab_t *__modsyms;/* separate symbol table for type names */
extern struct symtab_t *__pv_defsyms;/* global table for `defines */
extern struct symtab_t *__syssyms;/* global tab for system tasks and funcs */
extern struct sy_t **__glbsycmps; /* work global name symbols */
extern struct expr_t **__glbxcmps;/* work glbal exprs */
extern int32 __last_gsc;

/* n.l. access headers and tables */
extern struct mod_t *__modhdr;/* header of top level module list */
extern struct udp_t *__udphead; /* header udps */
extern struct udp_t *__udp_last;/* end udp list */
extern struct inst_t **__top_itab; /* tab of virt inst ptrs of top mods */
extern int32 *__top_ipind;    /* binary searchable top insts index */
extern int32 __numtopm;       /* number of uninstanciated top modules */
extern struct itree_t **__it_roots; /* table of root itree entries */
extern int32 __ualtrepipnum;  /* udp rep. change threshold */
extern struct thread_t *__initalw_thrd_hdr; /* list hd of per inst in/al thds */
extern struct tev_t *__tevtab;/* reallocable tab of events and free evs */
extern int32 __numused_tevtab;/* num used at least once in tev tab */
extern int32 __size_tevtab;   /* num tev's allocated in tev tab */ 
extern word32 *__contab;      /* design wide constant table */
extern int32 __contabwsiz;    /* currrent size of const tab in words */
extern int32 __contabwi;      /* next free word32 slot in const tab */
extern int32 __opempty_contabi; /* special contab ndx for opempty expr leaf */
extern struct contab_info_t **__contab_hash; /* contab hash information */

/* n.l. access routines */  
extern struct dfparam_t *__dfphdr; /* design wide defparam list header */
extern int32 __num_dfps;      /* number of defparams in source */
extern int32 __num_glbdfps;   /* number of defparams in design */
extern int32 __num_locdfps;   /* number of local defparams */
extern int32 __num_inst_pndparams;/* static number of inst. pound params */
extern int32 __design_gia_pndparams;/* T => at least one gia range pnd params */
extern int32 __design_gi_arrays;  /* T => design has arrays of g/i */ 
extern int32 __pndparam_splits; /* T => at least one split from pound params */
extern int32 __defparam_splits; /* T => at least one split from def params */
extern int32 __dagmaxdist;    /* max. nested mod. inst. level */
extern struct mod_t **__mdlevhdr; /* array of ptrs to ith lev linked mods */ 
extern struct cell_pin_t *__cphdr;   /* header of temp. cell pin list */
extern struct cell_pin_t *__cpp_last;/* current last cell pin*/
extern struct tnode_t *__tmp_head;

extern struct xldlnpp_t *__xldl_hdr; /* other side unproc. xl drv/ld npps */
extern struct xldlnpp_t *__last_xldl;/* end of list - place to add after */
extern struct xldlvtx_t **__xldlvtxind; /* table of xl drv/ld net/bit vtx */
extern int32 __num_xldlvtxs;  /* number of lements in table */
extern int32 __siz_xldlvtxtab;/* current size of table */

/* udp table building variables */
extern struct wcard_t *__wcardtab; /* level wildcard table */
extern int32 __last_wci;      /* last wild card index for line */
extern word32 *__cur_utab;    /* current udp table */
extern struct utline_t *__cur_utlp; /* current line info struct */
extern word32 __cur_uoval;    /* current udp line output value */
extern int32 __cur_unochange; /* T => cur line has '-' no change output */
extern struct udp_t *__cur_udp; /* current udp struct */
extern word32 __cur_upstate;  /* current last input (state) for wide */
extern int32 __cur_ueipnum;   /* cur. input pos. num of edge (NO_VAL none) */
extern int32 __cur_utabsel;   /* current edge 1st char - 2nd in state line */

/* expression and function processing variables */
extern int32 __xndi;          /* next place in collected expression list */
extern struct expr_t **__exprtab;/* table to collect expressions into */
extern struct expridtab_t **__expr_idtab; /* expr parse id name info */ 
extern int32 __exprtabsiz;    /* current operator precedence expr tab siz */
extern int32 __last_xtk;
extern struct expr_t *__root_ndp;/* root of built and alloced expression */
extern struct xstk_t **__xstk;/* expr work vals */
extern int32 __xspi;          /* expr. pointer */
extern int32 __maxxnest;      /* current size of expr. stack - must grow */ 
extern int32 __maxfcnest;     /* size of func. call task stk - must grow */
extern struct task_t **__fcstk; /* function call nesting stack */
extern int32 __fcspi;           /* fcall tos index */

/* -y and -v library variables */
extern struct vylib_t *__vyhdr; /* header of lib. file list */
extern struct vylib_t *__end_vy;/* last entry on vy lib. list */
extern int32 __num_ylibs;     /* number of ylibs in options */
extern int32 __num_vlibs;     /* number of vlibs in options */

extern struct undef_t *__undefhd;/* head of undefined mod/udp list */   
extern struct undef_t *__undeftail; /* tail of undefined mod/udp list */
extern int32 __undef_mods;    /* count of undefined modules */

extern int32 __lib_rescan;    /* T => rescan from start after each */
extern int32 __cur_passres;   /* num mods resolved in current pass */
extern int32 __rescanning_lib;/* T => for `language exclude after 1st pass */ 
extern int32 __num_ys;        /* number of -y options in lib. */
extern char **__lbexts;       /* tab of -y library extension suffixes */
extern int32 __last_lbx;
extern char **__incdirs;      /* tab of +incdir paths (always / end) */
extern int32 __last_incdir;

/* simulation preparation variables */
extern int32 __cur_npii;      /* current index of inst. in cur. mod */
extern struct gate_t *__cur_npgp;/* current net-pin processing gate */
extern struct mod_t *__cur_npmdp;/* current net-pin processing module */
extern struct conta_t *__cur_npcap; /* current net pin proc. conta */
extern struct tfrec_t *__cur_nptfrp; /* current net pin tf arg drvr rec */
extern struct net_t *__cur_npnp; /* current net pin net for vpi putv driver */
extern int32 __cur_npnum;     /* current port number (from 0) */
extern int32 __cur_pbi;       /* current bit number for PB ICONN npp */
extern int32 num_optim_cats;  /* number of optimized concats */
extern int32 num_optim_catels;/* number of all elements in optim concats */
extern int32 __cur_lhscati1;  /* if lhs concat, high rhs psel index */
extern int32 __cur_lhscati2;  /* if lhs concat, low rhs psel index */
extern struct st_t **__prpstk;/* during prep., continue stp */
extern int32 __prpsti;        /* top of nested stmt. stack */
extern int32 __nd_parmpnp_free; /* T => after 1st parmnpp need copy not orig */
extern int32 __num_rem_gate_pnd0s; /* number of removed source #0 gates */
extern int32 __num_flat_rem_gate_pnd0s; /* and flat number */
extern int32 __num_rem_mipds; /* number of per bit flat MIPDs 0 delays rmed */
extern int32 __last_modxi;    /* global counter used by n.l expr xform code */
extern int32 __last_modsti;   /* and counter for statements */
extern int32 __optimized_sim; /* generate c code - compile and dl link */
extern int32 __dump_flowg;    /* dump flow graph for debugging */

/* timing queue scheduling variables */
extern word64 __whetime;      /* current timing wheel end time */
extern word64 __simtime;      /* current simulaton time (make 64 bits ?) */
extern word32 __num_execstmts;/* total number of executed statements */
extern word32 __num_addedexec;/* number of executed added statements */
extern word32 __num_proc_tevents;/* total num simulation events processed */
extern word32 __nxtstmt_freq_update; /* next ev count for xbig freq upd. */ 
extern word32 __num_cancel_tevents; /* total num sim events processed */
extern int32 __num_twhevents; /* num of evs currently in timing wheel */
extern int32 __num_ovflqevents; /* num of events currently in ovflow q */
extern word32 __inertial_cancels; /* num resched form later inertial del */
extern word32 __newval_rescheds; /* num rescheduled for same time */
extern word32 __num_netchges; /* num of processed net change records */
extern word32 __immed_assigns;/* num immed assign (not scheduled) */ 
extern word32 __proc_thrd_tevents;/* number of processed thread events */
extern struct q_hdr_t *__qlist_hdr; /* for $q_ system task q list header */
extern int32 __num_switch_vtxs_processed; /* total num tranif chan vtx done */
extern int32 __num_switch_chans; /* total num tranif channels in design */ 

/* storage tables variables */
extern byte *__btab;          /* design wide scalar (byte) storage table */
extern int32 __btabbsiz;      /* scalar storage byte table size in bytes */
extern int32 __btabbi;        /* during var init next index to use */ 
extern byte *__nchgbtab;      /* table for per inst nchg bytes */
extern int32 __nchgbtabbsiz;  /* size in btab of nchg action bits */
extern int32 __nchgbtabbi;    /* during init, next index to use */
extern word32 *__wtab;        /* design wide var but not mem storage area */
extern int32 __wtabwsiz;      /* precomputed size (need ptrs into) in words */
extern int32 __wtabwi;        /* during var init next index to use */ 

/* simulation control and state values */
extern int32 __stmt_suspend;  /* set when behavioral code suspends */
extern int32 __run_state;     /* state of current simulation run */
extern int32 __can_exec;      /* T => for vpi sim ctrl - can now exec */
extern int32 __wire_init;     /* T => initializing wires */
extern int32 __no_tmove_levels; /* T => infinite 0 delay loop warn path dist */
extern struct thread_t *__cur_thd;  /* currently executing thread addr. */
extern struct thread_t *__suspended_thd; /* cur thread before suspend */
extern struct itree_t *__suspended_itp; /* cur inst ptr before suspend */
extern struct itree_t *__inst_ptr; /* current if flattened itree place */ 
extern struct mod_t *__inst_mod;   /* module of current itree inst */
extern int32 __inum;          /* iti num  of current inst (always set) */
extern struct itree_t **__itstk; /* stack of saved itrees */
extern int32 __itspi;         /* top of itree stack */
extern i_tev_ndx __fsusp_tevpi;/* in func. step, event to undo(cancel) */
extern struct itree_t *__tmpitp_freelst; /* free list of wrk itps */
extern struct inst_t *__tmpip_freelst; /* free list of wrk ips */
extern struct mod_t *__last_libmdp; /* libary module just read */
extern int32 __seed;          /* SJM 01/27/04 - glb seed needed if no arg */

/* execution state variables */
extern word32 __new_gateval;  /* new gate out val (st. possible) */
extern word32 __old_gateval;  /* before gate change (st. possible) */
extern word32 __new_inputval; /* new input value for tracing message */
extern word32 __old_inputval; /* prev. value of input for wide udp eval */
extern word64 __pdlatechgtim; /* for path tracing latest path chg time */
extern word64 __pdmindel;     /* for path minimum path delay */
extern int32 __nd_neg_del_warn; /* T => must emit warn (or err) for <0 del */ 
extern int32 __force_active;  /* T => for trace deassign while force */ 
extern int32 __assign_active; /* T => for trace release activates assgn */
extern struct dceauxlst_t *__qcaf_dcehdr; /* header of current qcaf dces */
extern int32 __nxt_chan_id;   /* cnter and size for assigning chan ids */
extern int32 __chanallocsize; /* size of allocated chan tables */
extern struct chanrec_t *__chantab;/* tab of channel records (one per id) */
extern struct vtxlst_t *__stvtxtab[8]; /* per stren value vertex list */ 
extern struct vtxlst_t *__stvtxtabend[8]; /* and ptr to last el on each */
extern struct vtxlst_t *__chg_vtxlst_hdr; /* list of chged vertices to store */
extern struct vtxlst_t *__chg_vtxlst_end; /* and ptr to end */
extern struct vtxlst_t *__off_vtxlst_hdr; /* bid chan vtx list for marks off */
extern struct vtxlst_t *__off_vtxlst_end; /* and ptr to end */
extern struct vtxlst_t *__vtxlst_freelst; /* free list for vtx lists */
extern struct vtx_t *__vtx_freelst;  /* free list for re-using vtxs */   
extern struct edge_t *__edge_freelst; /* free list for re-using edges */

extern word32 __acum_sb;      /* accumulator for stren tran chan combined */
extern word32 __acum_a;       /* accumulator for tran chan non stren */
extern word32 __acum_b;
extern byte *__acum_sbp;      /* ptr to stacked strength byte */
extern struct xstk_t *__acum_xsp; /* ptr to stacked strength byte */

/* end of time slot variables, strobe, monitor, time check */
extern struct strblst_t *__strobe_hdr; /* list strobe display at slot end */
extern struct strblst_t *__strobe_end; /* end of strobe display list */
extern struct strblst_t *__strb_freelst; /* head of free strobe elements */
extern struct st_t *__monit_stp;/* monit if chg display at slot end stmt */
extern struct itree_t *__monit_itp; /* current monitor itree element */
extern word32 __slotend_action; /* word32 of 1 bit switches set for action */
extern int32 __monit_active;  /* T => monitor can trigger (default) */
extern struct dceauxlst_t *__monit_dcehdr; /* header of current dces */
extern struct fmonlst_t *__fmon_hdr; /* list of execed (enabled) fmonitors */
extern struct fmonlst_t *__fmon_end;
extern struct fmonlst_t *__cur_fmon; /* current fmon list entry */
extern struct fmselst_t *__fmonse_hdr; /* this slot end fmon eval list */
extern struct fmselst_t *__fmonse_end;
extern struct fmselst_t *__fmse_freelst; /* fmon slot end free list head */

/* interactive execution variables */
extern struct itree_t *__scope_ptr; /* from $scope itree place */
extern struct task_t *__scope_tskp; /* from $scope task if present */ 
extern struct symtab_t *__last_iasytp; /* last found symbol symbol table */
extern struct iahist_t *__iahtab;/* table of history commands */
extern int32 __iahsiz;        /* current size of history cmd table */
extern int32 __iah_lasti;     /* current (latest) command */
extern struct hctrl_t *__hctrl_hd; /* head of active iact stmts */
extern struct hctrl_t *__hctrl_end;/* and end */
extern int32 __history_on;    /* collecting and saving history is on */
extern int32 __hist_cur_listnum;/* number to list for :history command */
extern int32 __iasetup;       /* F until interactive entered */
extern int32 __ia_entered;    /* F (also for reset) until iact entered */
extern int32 __iact_state;    /* T => in interactive processing */
extern int32 __iact_can_free; /* T => non monitor/strobe, can free */
extern int32 __no_iact;       /* T => no interactive processing for run */
extern int32 __intsig_prt_snapshot; /* T => on no iact end, print shapshot */
extern int32 __reset_count;   /* count of the number of rests ($reset) */
extern int32 __reset_value;   /* 2nd $reset value preserved after reset */ 
extern int32 __list_cur_ifi;  /* index in in fils of current source file */
extern int32 __list_cur_fd;   /* current opened file no. (-1 if none) */
extern int32 __list_cur_lini; /* current line no. in current dbg file */
extern int32 __list_cur_listnum;/* number of lines to list at once */
extern int32 __list_arg_lini; /* for :b (:ib), user list argument */
extern int32 __iact_scope_chg;/* T => always move scope to cur on iact st. */
extern struct brkpt_t *__bphdr;/* header of breakpoint list */ 
extern int32 __nxt_bpnum;     /* next breakpoint number to use */
extern struct dispx_t *__dispxhdr;/* header of display list */ 
extern int32 __nxt_dispxnum;  /* next display number to use */
extern struct itree_t *__last_stepitp;/* last step inst. itree loc. */ 
extern struct task_t *__last_steptskp;/* last step task */
extern int32 __last_stepifi;  /* last step in fils index */
extern word64 __last_brktime; /* last break or step time */
extern int32 __dbg_dflt_base; /* :print debugger default base */ 
extern int32 __iact_stmt_err; /* T => syntax error for iact stmt */
extern struct mod_t *__iact_mdp; /* current iact dummy module */
extern int32 __sav_mtime_units; /* prep of iact statements needs tfmt units */

/* interactive variables */
extern char *__iahwrkline;    /* interactive command line work area */
extern int32 __iahwrklen;     /* allocated len of iah work string */
extern int32 __pending_enter_iact;/* T => enter iact as soon as can */
extern int32 __iact_reason;   /* reason for entering interactive state */
extern int32 __single_step;   /* T => need to single step */
extern int32 __step_rep_cnt;  /* number of times to repeat step */
extern int32 __step_from_thread;/* T step from non thread loc. (^c?) */
extern struct itree_t *__step_match_itp; /* for istep, exec itp must match */
extern int32 __step_lini;     /* line stepping from (must step to next) */  
extern int32 __step_ifi;      /* and file */ 
extern int32 __verbose_step;  /* T => emit location each step */
extern int32 __stop_before_sim; /* T => enter interactive before sim */
extern int32 __dbg_stop_before; /* if >100, T (-100) stop before sim */ 
extern struct st_t *__blklast_stp; /* stmt loc. saved last stmt in block */
extern struct dceauxlst_t *__iact_dcehdr; /* header of current iact dces */

/* event list variables */
extern struct telhdr_t **__twheel;
extern int32 __twhsize;       /* current size for timing wheel */   
extern int32 __cur_twi;
extern i_tev_ndx __p0_te_hdri;/* pound 0 event list header */
extern i_tev_ndx __p0_te_endi;/* pound 0 event list end */
extern i_tev_ndx __cur_te_hdri;
extern i_tev_ndx __cur_tevpi; /* ptr to event list for adding to front */
extern i_tev_ndx __cur_te_endi;
extern i_tev_ndx __tefreelsti;/* free list for events */
extern struct tedputp_t *__tedpfreelst; /* tf_ putp rec free list header */
extern struct teputv_t *__teputvfreelst; /* vpi_ put value free list hdr */ 
extern struct nchglst_t *__nchgfreelst; /* change element free list */
extern struct tc_pendlst_t *__tcpendfreelst; /* free slot end changed tchks */
extern struct dltevlst_t *__dltevfreelst; /* pend double event free list */
extern struct tevlst_t *__ltevfreelst; /* pend event free list */
extern i_tev_ndx __nb_te_hdri; /* non-blocking new end queue hd */ 
extern i_tev_ndx __nb_te_endi; /* and tail */

/* net change list variables */
extern struct nchglst_t *__nchg_futhdr; /* header of future net chg list */
extern struct nchglst_t *__nchg_futend; /* end (for add) of future net chgs */
extern struct tc_pendlst_t *__tcpendlst_hdr; /* header of pending */ 
extern struct tc_pendlst_t *__tcpendlst_end; /* end of pending */
extern i_tev_ndx *__wrkevtab; /* for exit, trace of pending events */
extern int32 __last_wevti;    /* last filled */
extern int32 __size_wrkevtab; /* and current allocated size */ 

/* b tree variables */
extern struct bt_t *__btqroot;/* root of timing overflow q */
/* for fringe node, node previous to place where inserted */
/* storage for path to fringe - node passed thru if not fringe */
extern struct bt_t **__btndstk; /* nodes with node list length */
extern struct bt_t **__btndhdrstk;
extern int32 __topi;
extern int32 __max_level;
extern int32 __nd_level;


#include "systsks.h"
