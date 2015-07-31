/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 19 "parser.y"

#include <stdio.h>
#include "log.h"
#include "parse_tree.h"
#include "symbol.h"
#include "line.h"

#define YYSTYPE node *

extern char yytext[];
extern int column, line;
extern symbol_table *sym;
extern node *parse_tree_head;

extern int yylex (void);

void yyerror(s)
char *s;
{
	lerr(line, "[plpcc] syntax error\n"); 
}



/* Line 268 of yacc.c  */
#line 96 "parser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENTIFIER = 258,
     CONSTANT = 259,
     STRING_LITERAL = 260,
     SIZEOF = 261,
     PTR_OP = 262,
     INC_OP = 263,
     DEC_OP = 264,
     LEFT_OP = 265,
     RIGHT_OP = 266,
     LE_OP = 267,
     GE_OP = 268,
     EQ_OP = 269,
     NE_OP = 270,
     AND_OP = 271,
     OR_OP = 272,
     MUL_ASSIGN = 273,
     DIV_ASSIGN = 274,
     MOD_ASSIGN = 275,
     ADD_ASSIGN = 276,
     SUB_ASSIGN = 277,
     LEFT_ASSIGN = 278,
     RIGHT_ASSIGN = 279,
     AND_ASSIGN = 280,
     XOR_ASSIGN = 281,
     OR_ASSIGN = 282,
     TYPE_NAME = 283,
     TYPEDEF = 284,
     EXTERN = 285,
     STATIC = 286,
     AUTO = 287,
     REGISTER = 288,
     CHAR = 289,
     SHORT = 290,
     INT = 291,
     LONG = 292,
     SIGNED = 293,
     UNSIGNED = 294,
     FLOAT = 295,
     DOUBLE = 296,
     CONST = 297,
     VOLATILE = 298,
     VOID = 299,
     STRUCT = 300,
     UNION = 301,
     ENUM = 302,
     ELLIPSIS = 303,
     CASE = 304,
     DEFAULT = 305,
     IF = 306,
     ELSE = 307,
     SWITCH = 308,
     WHILE = 309,
     DO = 310,
     FOR = 311,
     GOTO = 312,
     CONTINUE = 313,
     BREAK = 314,
     RETURN = 315,
     ASM = 316,
     IFX = 317
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 200 "parser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  61
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1346

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  87
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  213
/* YYNRULES -- Number of states.  */
#define YYNSTATES  354

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   317

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    74,     2,     2,     2,    76,    69,     2,
      63,    64,    70,    71,    68,    72,    67,    75,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    82,    84,
      77,    83,    78,    81,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    65,     2,    66,    79,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    85,    80,    86,    73,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    13,    18,    20,    25,
      29,    34,    38,    42,    45,    48,    50,    54,    56,    59,
      62,    65,    68,    73,    75,    77,    79,    81,    83,    85,
      87,    92,    94,    98,   102,   106,   108,   112,   116,   118,
     122,   126,   128,   132,   136,   140,   144,   146,   150,   154,
     156,   160,   162,   166,   168,   172,   174,   178,   180,   184,
     186,   192,   194,   198,   200,   202,   204,   206,   208,   210,
     212,   214,   216,   218,   220,   222,   226,   228,   231,   235,
     237,   240,   242,   245,   247,   250,   252,   256,   258,   262,
     264,   266,   268,   270,   272,   274,   276,   278,   280,   282,
     284,   286,   288,   290,   292,   294,   296,   302,   307,   310,
     312,   314,   316,   319,   323,   326,   328,   331,   333,   335,
     339,   341,   344,   348,   353,   359,   362,   364,   368,   370,
     374,   376,   378,   381,   383,   385,   389,   394,   398,   403,
     408,   412,   414,   417,   420,   424,   426,   429,   431,   435,
     437,   441,   444,   447,   449,   451,   455,   457,   460,   462,
     464,   467,   471,   474,   478,   482,   487,   490,   494,   498,
     503,   505,   509,   514,   516,   520,   522,   524,   526,   528,
     530,   532,   536,   541,   545,   548,   552,   556,   561,   563,
     566,   568,   571,   573,   576,   582,   590,   596,   602,   610,
     617,   625,   629,   632,   635,   638,   642,   644,   647,   649,
     651,   656,   660,   664
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     148,     0,    -1,     3,    -1,     4,    -1,     5,    -1,    63,
     107,    64,    -1,    61,    63,     5,    64,    -1,    88,    -1,
      89,    65,   107,    66,    -1,    89,    63,    64,    -1,    89,
      63,    90,    64,    -1,    89,    67,     3,    -1,    89,     7,
       3,    -1,    89,     8,    -1,    89,     9,    -1,   105,    -1,
      90,    68,   105,    -1,    89,    -1,     8,    91,    -1,     9,
      91,    -1,    92,    93,    -1,     6,    91,    -1,     6,    63,
     134,    64,    -1,    69,    -1,    70,    -1,    71,    -1,    72,
      -1,    73,    -1,    74,    -1,    91,    -1,    63,   134,    64,
      93,    -1,    93,    -1,    94,    70,    93,    -1,    94,    75,
      93,    -1,    94,    76,    93,    -1,    94,    -1,    95,    71,
      94,    -1,    95,    72,    94,    -1,    95,    -1,    96,    10,
      95,    -1,    96,    11,    95,    -1,    96,    -1,    97,    77,
      96,    -1,    97,    78,    96,    -1,    97,    12,    96,    -1,
      97,    13,    96,    -1,    97,    -1,    98,    14,    97,    -1,
      98,    15,    97,    -1,    98,    -1,    99,    69,    98,    -1,
      99,    -1,   100,    79,    99,    -1,   100,    -1,   101,    80,
     100,    -1,   101,    -1,   102,    16,   101,    -1,   102,    -1,
     103,    17,   102,    -1,   103,    -1,   103,    81,   107,    82,
     104,    -1,   104,    -1,    91,   106,   105,    -1,    83,    -1,
      18,    -1,    19,    -1,    20,    -1,    21,    -1,    22,    -1,
      23,    -1,    24,    -1,    25,    -1,    26,    -1,    27,    -1,
     105,    -1,   107,    68,   105,    -1,   104,    -1,   110,    84,
      -1,   110,   111,    84,    -1,   113,    -1,   113,   110,    -1,
     114,    -1,   114,   110,    -1,   125,    -1,   125,   110,    -1,
     112,    -1,   111,    68,   112,    -1,   126,    -1,   126,    83,
     137,    -1,    29,    -1,    30,    -1,    31,    -1,    32,    -1,
      33,    -1,    44,    -1,    34,    -1,    35,    -1,    36,    -1,
      37,    -1,    40,    -1,    41,    -1,    38,    -1,    39,    -1,
     115,    -1,   122,    -1,    28,    -1,   116,     3,    85,   117,
      86,    -1,   116,    85,   117,    86,    -1,   116,     3,    -1,
      45,    -1,    46,    -1,   118,    -1,   117,   118,    -1,   119,
     120,    84,    -1,   114,   119,    -1,   114,    -1,   125,   119,
      -1,   125,    -1,   121,    -1,   120,    68,   121,    -1,   126,
      -1,    82,   108,    -1,   126,    82,   108,    -1,    47,    85,
     123,    86,    -1,    47,     3,    85,   123,    86,    -1,    47,
       3,    -1,   124,    -1,   123,    68,   124,    -1,     3,    -1,
       3,    83,   108,    -1,    42,    -1,    43,    -1,   128,   127,
      -1,   127,    -1,     3,    -1,    63,   126,    64,    -1,   127,
      65,   108,    66,    -1,   127,    65,    66,    -1,   127,    63,
     130,    64,    -1,   127,    63,   133,    64,    -1,   127,    63,
      64,    -1,    70,    -1,    70,   129,    -1,    70,   128,    -1,
      70,   129,   128,    -1,   125,    -1,   129,   125,    -1,   131,
      -1,   131,    68,    48,    -1,   132,    -1,   131,    68,   132,
      -1,   110,   126,    -1,   110,   135,    -1,   110,    -1,     3,
      -1,   133,    68,     3,    -1,   119,    -1,   119,   135,    -1,
     128,    -1,   136,    -1,   128,   136,    -1,    63,   135,    64,
      -1,    65,    66,    -1,    65,   108,    66,    -1,   136,    65,
      66,    -1,   136,    65,   108,    66,    -1,    63,    64,    -1,
      63,   130,    64,    -1,   136,    63,    64,    -1,   136,    63,
     130,    64,    -1,   105,    -1,    85,   138,    86,    -1,    85,
     138,    68,    86,    -1,   137,    -1,   138,    68,   137,    -1,
     140,    -1,   141,    -1,   144,    -1,   145,    -1,   146,    -1,
     147,    -1,     3,    82,   139,    -1,    49,   108,    82,   139,
      -1,    50,    82,   139,    -1,    85,    86,    -1,    85,   143,
      86,    -1,    85,   142,    86,    -1,    85,   142,   143,    86,
      -1,   109,    -1,   142,   109,    -1,   139,    -1,   143,   139,
      -1,    84,    -1,   107,    84,    -1,    51,    63,   107,    64,
     139,    -1,    51,    63,   107,    64,   139,    52,   139,    -1,
      53,    63,   107,    64,   139,    -1,    54,    63,   107,    64,
     139,    -1,    55,   139,    54,    63,   107,    64,    84,    -1,
      56,    63,   144,   144,    64,   139,    -1,    56,    63,   144,
     144,   107,    64,   139,    -1,    57,     3,    84,    -1,    58,
      84,    -1,    59,    84,    -1,    60,    84,    -1,    60,   107,
      84,    -1,   149,    -1,   148,   149,    -1,   150,    -1,   109,
      -1,   110,   126,   142,   141,    -1,   110,   126,   141,    -1,
     126,   142,   141,    -1,   126,   141,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    65,    65,    66,    67,    68,    69,    73,    74,    75,
      76,    77,    78,    79,    80,    84,    85,    89,    90,    91,
      92,    93,    94,    98,    99,   100,   101,   102,   103,   107,
     108,   112,   113,   114,   115,   119,   120,   121,   125,   126,
     127,   131,   132,   133,   134,   135,   139,   140,   141,   145,
     146,   150,   151,   155,   156,   160,   161,   165,   166,   170,
     171,   175,   176,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   194,   195,   199,   203,   204,   208,
     209,   210,   211,   212,   213,   217,   218,   222,   223,   227,
     228,   229,   230,   231,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   250,   251,   252,   256,
     257,   261,   262,   266,   270,   271,   272,   273,   277,   278,
     282,   283,   284,   288,   289,   290,   294,   295,   299,   300,
     304,   305,   309,   310,   314,   315,   316,   317,   318,   319,
     320,   324,   325,   326,   327,   331,   332,   337,   338,   342,
     343,   347,   348,   349,   353,   354,   358,   359,   363,   364,
     365,   369,   370,   371,   372,   373,   374,   375,   376,   377,
     381,   382,   383,   387,   388,   392,   393,   394,   395,   396,
     397,   401,   402,   403,   407,   408,   409,   410,   414,   415,
     419,   420,   424,   425,   429,   430,   431,   435,   436,   437,
     438,   442,   443,   444,   445,   446,   450,   451,   455,   456,
     460,   461,   462,   463
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "CONSTANT",
  "STRING_LITERAL", "SIZEOF", "PTR_OP", "INC_OP", "DEC_OP", "LEFT_OP",
  "RIGHT_OP", "LE_OP", "GE_OP", "EQ_OP", "NE_OP", "AND_OP", "OR_OP",
  "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN", "ADD_ASSIGN", "SUB_ASSIGN",
  "LEFT_ASSIGN", "RIGHT_ASSIGN", "AND_ASSIGN", "XOR_ASSIGN", "OR_ASSIGN",
  "TYPE_NAME", "TYPEDEF", "EXTERN", "STATIC", "AUTO", "REGISTER", "CHAR",
  "SHORT", "INT", "LONG", "SIGNED", "UNSIGNED", "FLOAT", "DOUBLE", "CONST",
  "VOLATILE", "VOID", "STRUCT", "UNION", "ENUM", "ELLIPSIS", "CASE",
  "DEFAULT", "IF", "ELSE", "SWITCH", "WHILE", "DO", "FOR", "GOTO",
  "CONTINUE", "BREAK", "RETURN", "ASM", "IFX", "'('", "')'", "'['", "']'",
  "'.'", "','", "'&'", "'*'", "'+'", "'-'", "'~'", "'!'", "'/'", "'%'",
  "'<'", "'>'", "'^'", "'|'", "'?'", "':'", "'='", "';'", "'{'", "'}'",
  "$accept", "primary_expression", "postfix_expression",
  "argument_expression_list", "unary_expression", "unary_operator",
  "cast_expression", "multiplicative_expression", "additive_expression",
  "shift_expression", "relational_expression", "equality_expression",
  "and_expression", "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "expression", "constant_expression", "declaration",
  "declaration_specifiers", "init_declarator_list", "init_declarator",
  "storage_class_specifier", "type_specifier", "struct_or_union_specifier",
  "struct_or_union", "struct_declaration_list", "struct_declaration",
  "specifier_qualifier_list", "struct_declarator_list",
  "struct_declarator", "enum_specifier", "enumerator_list", "enumerator",
  "type_qualifier", "declarator", "direct_declarator", "pointer",
  "type_qualifier_list", "parameter_type_list", "parameter_list",
  "parameter_declaration", "identifier_list", "type_name",
  "abstract_declarator", "direct_abstract_declarator", "initializer",
  "initializer_list", "statement", "labeled_statement",
  "compound_statement", "declaration_list", "statement_list",
  "expression_statement", "selection_statement", "iteration_statement",
  "jump_statement", "translation_unit", "external_declaration",
  "function_definition", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,    40,    41,    91,    93,    46,    44,    38,
      42,    43,    45,   126,    33,    47,    37,    60,    62,    94,
     124,    63,    58,    61,    59,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    87,    88,    88,    88,    88,    88,    89,    89,    89,
      89,    89,    89,    89,    89,    90,    90,    91,    91,    91,
      91,    91,    91,    92,    92,    92,    92,    92,    92,    93,
      93,    94,    94,    94,    94,    95,    95,    95,    96,    96,
      96,    97,    97,    97,    97,    97,    98,    98,    98,    99,
      99,   100,   100,   101,   101,   102,   102,   103,   103,   104,
     104,   105,   105,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   107,   107,   108,   109,   109,   110,
     110,   110,   110,   110,   110,   111,   111,   112,   112,   113,
     113,   113,   113,   113,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   115,   115,   115,   116,
     116,   117,   117,   118,   119,   119,   119,   119,   120,   120,
     121,   121,   121,   122,   122,   122,   123,   123,   124,   124,
     125,   125,   126,   126,   127,   127,   127,   127,   127,   127,
     127,   128,   128,   128,   128,   129,   129,   130,   130,   131,
     131,   132,   132,   132,   133,   133,   134,   134,   135,   135,
     135,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     137,   137,   137,   138,   138,   139,   139,   139,   139,   139,
     139,   140,   140,   140,   141,   141,   141,   141,   142,   142,
     143,   143,   144,   144,   145,   145,   145,   146,   146,   146,
     146,   147,   147,   147,   147,   147,   148,   148,   149,   149,
     150,   150,   150,   150
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     3,     4,     1,     4,     3,
       4,     3,     3,     2,     2,     1,     3,     1,     2,     2,
       2,     2,     4,     1,     1,     1,     1,     1,     1,     1,
       4,     1,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       5,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     2,     3,     1,
       2,     1,     2,     1,     2,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     5,     4,     2,     1,
       1,     1,     2,     3,     2,     1,     2,     1,     1,     3,
       1,     2,     3,     4,     5,     2,     1,     3,     1,     3,
       1,     1,     2,     1,     1,     3,     4,     3,     4,     4,
       3,     1,     2,     2,     3,     1,     2,     1,     3,     1,
       3,     2,     2,     1,     1,     3,     1,     2,     1,     1,
       2,     3,     2,     3,     3,     4,     2,     3,     3,     4,
       1,     3,     4,     1,     3,     1,     1,     1,     1,     1,
       1,     3,     4,     3,     2,     3,     3,     4,     1,     2,
       1,     2,     1,     2,     5,     7,     5,     5,     7,     6,
       7,     3,     2,     2,     2,     3,     1,     2,     1,     1,
       4,     3,     3,     2
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,   134,   105,    89,    90,    91,    92,    93,    95,    96,
      97,    98,   101,   102,    99,   100,   130,   131,    94,   109,
     110,     0,     0,   141,   209,     0,    79,    81,   103,     0,
     104,    83,     0,   133,     0,     0,   206,   208,   125,     0,
       0,   145,   143,   142,    77,     0,    85,    87,    80,    82,
     108,     0,    84,     0,   188,     0,   213,     0,     0,     0,
     132,     1,   207,     0,   128,     0,   126,   135,   146,   144,
       0,    78,     0,   211,     0,     0,   115,     0,   111,     0,
     117,     2,     3,     4,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,    24,    25,    26,    27,    28,   192,   184,     7,    17,
      29,     0,    31,    35,    38,    41,    46,    49,    51,    53,
      55,    57,    59,    61,    74,     0,   190,   175,   176,     0,
       0,   177,   178,   179,   180,    87,   189,   212,   154,   140,
     153,     0,   147,   149,     0,     2,   137,    29,    76,     0,
       0,     0,     0,   123,    86,     0,   170,    88,   210,     0,
     114,   107,   112,     0,     0,   118,   120,   116,     0,     0,
      21,     0,    18,    19,     0,     0,     0,     0,     0,     0,
       0,     0,   202,   203,   204,     0,     0,     0,   156,     0,
       0,    13,    14,     0,     0,     0,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    63,     0,    20,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   193,
     186,     0,   185,   191,     0,     0,   151,   158,   152,   159,
     138,     0,   139,     0,   136,   124,   129,   127,   173,     0,
     106,   121,     0,   113,     0,   181,     0,     0,   183,     0,
       0,     0,     0,     0,   201,   205,     0,     5,     0,   158,
     157,     0,    12,     9,     0,    15,     0,    11,    62,    32,
      33,    34,    36,    37,    39,    40,    44,    45,    42,    43,
      47,    48,    50,    52,    54,    56,    58,     0,    75,   187,
     166,     0,     0,   162,     0,   160,     0,     0,   148,   150,
     155,     0,   171,   119,   122,    22,   182,     0,     0,     0,
       0,     0,     6,    30,    10,     0,     8,     0,   167,   161,
     163,   168,     0,   164,     0,   172,   174,   194,   196,   197,
       0,     0,     0,    16,    60,   169,   165,     0,     0,   199,
       0,   195,   198,   200
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   108,   109,   274,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   207,
     125,   149,    54,    55,    45,    46,    26,    27,    28,    29,
      77,    78,    79,   164,   165,    30,    65,    66,    31,    32,
      33,    34,    43,   301,   142,   143,   144,   189,   302,   239,
     157,   249,   126,   127,   128,    57,   130,   131,   132,   133,
     134,    35,    36,    37
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -215
static const yytype_int16 yypact[] =
{
    1021,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,     4,    22,   -21,  -215,    13,  1299,  1299,  -215,     5,
    -215,  1299,  1175,   146,    58,   931,  -215,  -215,   -61,    61,
     -31,  -215,  -215,   -21,  -215,     3,  -215,  1117,  -215,  -215,
      14,  1150,  -215,   280,  -215,    13,  -215,  1175,  1066,   712,
     146,  -215,  -215,    61,    19,   -38,  -215,  -215,  -215,  -215,
      22,  -215,   568,  -215,  1175,  1150,  1150,   417,  -215,    11,
    1150,    51,  -215,  -215,   835,   856,   856,   880,    59,    63,
      83,    97,   534,   123,   111,   107,   118,   606,   147,   691,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,   127,
     351,   880,  -215,    74,     7,   222,    34,    54,   126,   145,
     137,   224,   -11,  -215,  -215,    23,  -215,  -215,  -215,   352,
     424,  -215,  -215,  -215,  -215,   155,  -215,  -215,  -215,  -215,
      12,   189,   187,  -215,    88,  -215,  -215,  -215,  -215,   191,
     -27,   880,    61,  -215,  -215,   568,  -215,  -215,  -215,  1097,
    -215,  -215,  -215,   880,    32,  -215,   176,  -215,   534,   691,
    -215,   880,  -215,  -215,   182,   534,   880,   880,   880,   212,
     640,   185,  -215,  -215,  -215,    45,   265,    90,    77,   207,
     274,  -215,  -215,   736,   880,   275,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,   880,  -215,   880,
     880,   880,   880,   880,   880,   880,   880,   880,   880,   880,
     880,   880,   880,   880,   880,   880,   880,   880,   880,  -215,
    -215,   462,  -215,  -215,   976,   763,  -215,    57,  -215,   162,
    -215,  1278,  -215,   277,  -215,  -215,  -215,  -215,  -215,   -24,
    -215,  -215,    11,  -215,   880,  -215,   217,   534,  -215,   129,
     132,   139,   219,   640,  -215,  -215,   223,  -215,  1198,   165,
    -215,   880,  -215,  -215,   144,  -215,    87,  -215,  -215,  -215,
    -215,  -215,    74,    74,     7,     7,   222,   222,   222,   222,
      34,    34,    54,   126,   145,   137,   224,   -39,  -215,  -215,
    -215,   226,   227,  -215,   228,   162,  1241,   784,  -215,  -215,
    -215,   496,  -215,  -215,  -215,  -215,  -215,   534,   534,   534,
     880,   808,  -215,  -215,  -215,   880,  -215,   880,  -215,  -215,
    -215,  -215,   229,  -215,   230,  -215,  -215,   240,  -215,  -215,
     152,   534,   158,  -215,  -215,  -215,  -215,   534,   211,  -215,
     534,  -215,  -215,  -215
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -215,  -215,  -215,  -215,   -48,  -215,   -92,    35,    21,    44,
      30,    76,    78,    75,    79,   102,  -215,   -55,   -69,  -215,
     -46,   -53,    10,     0,  -215,   232,  -215,   138,  -215,  -215,
     225,   -64,   -26,  -215,    53,  -215,   269,   151,    29,   -13,
     -32,    -3,  -215,   -57,  -215,   101,  -215,   175,  -123,  -214,
    -127,  -215,   -74,  -215,   172,    98,   216,  -175,  -215,  -215,
    -215,  -215,   311,  -215
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      25,   141,    60,   156,   148,   263,   226,    38,    50,    40,
      24,   147,    47,   162,     1,     1,     1,   238,   179,   208,
      42,    16,    17,   305,    63,     1,    48,    49,   248,   228,
     152,    52,   148,    67,   174,    25,   170,   172,   173,   147,
      69,   152,   135,   327,   311,    24,   216,   217,   153,    23,
     160,   185,    41,   187,   167,   305,   233,   135,   140,   245,
       1,     1,   312,   147,    64,   270,   166,   136,   220,   221,
     227,    70,    68,   188,    22,   234,    22,   235,   212,   213,
      80,    23,    23,    23,   136,    22,   156,    71,   321,    39,
      51,   228,    23,   163,   255,   162,   148,    44,   246,    75,
     252,   258,   151,   147,    80,    80,    80,   229,   148,    80,
     251,   218,   219,   228,   181,   147,   253,   279,   280,   281,
     234,    22,   235,   187,   275,   187,   176,   236,    80,   265,
     259,   260,   261,   168,   190,   191,   192,   237,   278,   136,
     268,   175,   235,   188,   209,    74,   177,    23,   276,   210,
     211,   129,   242,   326,   267,   228,   243,   233,   228,   298,
     178,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   323,
     148,   297,   304,   316,   336,   269,   180,   147,    80,    76,
     193,   182,   194,   317,   195,   222,   318,   228,    80,   148,
     228,   314,   183,   319,    56,    60,   147,   228,   324,    58,
     186,    59,   325,    76,    76,    76,   348,   224,    76,    73,
     228,    40,   350,   147,   223,   306,   228,   307,   268,   137,
     235,   237,   214,   215,   140,   284,   285,    76,    72,   166,
     225,   140,   156,   337,   338,   339,   158,   282,   283,   332,
     290,   291,   148,   240,   334,   241,   343,   244,   254,   147,
     286,   287,   288,   289,   257,   269,   262,   349,   140,   264,
     266,   271,   344,   351,   340,   342,   353,   272,   277,   147,
     310,   315,   320,    81,    82,    83,    84,   322,    85,    86,
     328,   329,   347,   345,   330,   352,   346,    76,   292,   294,
     159,   293,   154,   247,   295,   313,   140,    76,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,   296,    87,
      88,    89,   150,    90,    91,    92,    93,    94,    95,    96,
      97,    98,   309,    99,   256,   231,    62,     0,     0,   100,
     101,   102,   103,   104,   105,    81,    82,    83,    84,     0,
      85,    86,     0,     0,   106,    53,   107,     0,     0,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,     0,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
       0,    87,    88,    89,     0,    90,    91,    92,    93,    94,
      95,    96,    97,    98,     0,    99,     0,     0,     0,     0,
       0,   100,   101,   102,   103,   104,   105,    81,    82,    83,
      84,     0,    85,    86,   206,     0,   106,    53,   230,     0,
       0,     0,     0,     0,     0,     2,     0,     0,     0,     0,
       0,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    81,    82,    83,    84,     0,
      85,    86,     0,    87,    88,    89,     0,    90,    91,    92,
      93,    94,    95,    96,    97,    98,     0,    99,     0,     0,
       0,     0,     0,   100,   101,   102,   103,   104,   105,   145,
      82,    83,    84,   161,    85,    86,     0,     0,   106,    53,
     232,    87,    88,    89,     0,    90,    91,    92,    93,    94,
      95,    96,    97,    98,     0,    99,     0,     0,     0,     0,
       0,   100,   101,   102,   103,   104,   105,    81,    82,    83,
      84,     0,    85,    86,     0,     0,   106,    53,   299,     0,
       0,     0,     0,     0,     0,     0,     0,    98,     0,    99,
       0,     0,     0,     0,     0,   100,   101,   102,   103,   104,
     105,   145,    82,    83,    84,     0,    85,    86,     0,     0,
       0,   155,   335,    87,    88,    89,     0,    90,    91,    92,
      93,    94,    95,    96,    97,    98,     0,    99,     0,     0,
       0,     0,     0,   100,   101,   102,   103,   104,   105,   145,
      82,    83,    84,     0,    85,    86,     0,     0,   106,    53,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    98,
       0,    99,     0,     0,     0,     0,     0,   100,   101,   102,
     103,   104,   105,   145,    82,    83,    84,     0,    85,    86,
       0,     0,     0,   155,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    98,     0,    99,
       0,     0,     0,     0,     0,   100,   101,   102,   103,   104,
     105,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     184,     0,     0,     0,   145,    82,    83,    84,     0,    85,
      86,    98,     0,    99,     0,     0,     0,     0,     0,   100,
     101,   102,   103,   104,   105,   145,    82,    83,    84,     2,
      85,    86,     0,     0,   106,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,   145,
      82,    83,    84,     0,    85,    86,     0,     0,     0,     0,
       0,     0,    98,     0,    99,     0,     0,     0,     0,     0,
     100,   101,   102,   103,   104,   105,   145,    82,    83,    84,
       0,    85,    86,    98,     0,    99,     0,     0,   146,     0,
       0,   100,   101,   102,   103,   104,   105,   145,    82,    83,
      84,     0,    85,    86,     0,     0,     0,    98,     0,    99,
     273,     0,     0,     0,     0,   100,   101,   102,   103,   104,
     105,   145,    82,    83,    84,     0,    85,    86,     0,     0,
       0,     0,     0,     0,    98,     0,    99,     0,     0,   303,
       0,     0,   100,   101,   102,   103,   104,   105,   145,    82,
      83,    84,     0,    85,    86,    98,     0,    99,     0,     0,
     333,     0,     0,   100,   101,   102,   103,   104,   105,   145,
      82,    83,    84,     0,    85,    86,     0,     0,     0,    98,
       0,    99,   341,     0,     0,     0,     0,   100,   101,   102,
     103,   104,   105,   145,    82,    83,    84,     0,    85,    86,
       0,     0,     0,     0,     0,     0,    98,     0,   169,     0,
       0,     0,     0,     0,   100,   101,   102,   103,   104,   105,
       0,     0,     0,     0,     0,     0,     0,    98,     0,   171,
       0,     0,     0,     0,     0,   100,   101,   102,   103,   104,
     105,    61,     0,     0,     1,     0,     0,     0,     0,     0,
       0,    98,     0,    99,     0,     0,     0,     0,     0,   100,
     101,   102,   103,   104,   105,     0,     0,     0,     0,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,     1,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    22,     0,     0,     0,     0,     0,
       0,    23,     0,     0,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     1,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   234,
     300,   235,     0,     0,     0,     0,    23,     0,     0,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,   138,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    22,     0,     0,     0,     0,     0,
       0,    23,     0,     0,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     2,     0,     0,     0,     0,
     139,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     2,     0,
       0,     0,     0,   250,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,     0,     0,
      72,     0,    53,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,     0,     0,     0,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      53,   268,   300,   235,     0,     0,     0,     0,    23,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   331,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,   308,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-215))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       0,    58,    34,    72,    59,   180,    17,     3,     3,    22,
       0,    59,    25,    77,     3,     3,     3,   140,    92,   111,
      23,    42,    43,   237,    85,     3,    26,    27,   155,    68,
      68,    31,    87,    64,    87,    35,    84,    85,    86,    87,
      43,    68,    55,    82,    68,    35,    12,    13,    86,    70,
      76,    97,    23,    99,    80,   269,   130,    70,    58,    86,
       3,     3,    86,   111,     3,   188,    79,    57,    14,    15,
      81,    68,    43,    99,    63,    63,    63,    65,    71,    72,
      51,    70,    70,    70,    74,    63,   155,    84,   263,    85,
      85,    68,    70,    82,   168,   159,   151,    84,   151,    85,
      68,   175,    83,   151,    75,    76,    77,    84,   163,    80,
     163,    77,    78,    68,     3,   163,    84,   209,   210,   211,
      63,    63,    65,   169,   193,   171,    63,   140,    99,    84,
     176,   177,   178,    82,     7,     8,     9,   140,   207,   129,
      63,    82,    65,   169,    70,    47,    63,    70,   194,    75,
      76,    53,    64,    66,    64,    68,    68,   231,    68,   228,
      63,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   271,
     235,   227,   235,   257,   311,   188,    63,   235,   159,    51,
      63,    84,    65,    64,    67,    69,    64,    68,   169,   254,
      68,   254,    84,    64,    32,   237,   254,    68,    64,    63,
      63,    65,    68,    75,    76,    77,    64,    80,    80,    47,
      68,   234,    64,   271,    79,    63,    68,    65,    63,    57,
      65,   234,    10,    11,   234,   214,   215,    99,    83,   252,
      16,   241,   311,   317,   318,   319,    74,   212,   213,   306,
     220,   221,   307,    64,   307,    68,   325,    66,    82,   307,
     216,   217,   218,   219,    82,   268,    54,   341,   268,    84,
       5,    64,   327,   347,   320,   321,   350,     3,     3,   327,
       3,    64,    63,     3,     4,     5,     6,    64,     8,     9,
      64,    64,    52,    64,    66,    84,    66,   159,   222,   224,
      75,   223,    70,   152,   225,   252,   306,   169,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,   226,    49,
      50,    51,    63,    53,    54,    55,    56,    57,    58,    59,
      60,    61,   241,    63,   169,   129,    35,    -1,    -1,    69,
      70,    71,    72,    73,    74,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    84,    85,    86,    -1,    -1,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    -1,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      -1,    49,    50,    51,    -1,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    -1,    63,    -1,    -1,    -1,    -1,
      -1,    69,    70,    71,    72,    73,    74,     3,     4,     5,
       6,    -1,     8,     9,    83,    -1,    84,    85,    86,    -1,
      -1,    -1,    -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,
      -1,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,     3,     4,     5,     6,    -1,
       8,     9,    -1,    49,    50,    51,    -1,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    -1,    63,    -1,    -1,
      -1,    -1,    -1,    69,    70,    71,    72,    73,    74,     3,
       4,     5,     6,    86,     8,     9,    -1,    -1,    84,    85,
      86,    49,    50,    51,    -1,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    -1,    63,    -1,    -1,    -1,    -1,
      -1,    69,    70,    71,    72,    73,    74,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    84,    85,    86,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    63,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,
      74,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    85,    86,    49,    50,    51,    -1,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    -1,    63,    -1,    -1,
      -1,    -1,    -1,    69,    70,    71,    72,    73,    74,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      -1,    63,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,
      72,    73,    74,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    63,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,
      74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    -1,     3,     4,     5,     6,    -1,     8,
       9,    61,    -1,    63,    -1,    -1,    -1,    -1,    -1,    69,
      70,    71,    72,    73,    74,     3,     4,     5,     6,    28,
       8,     9,    -1,    -1,    84,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    -1,    63,    -1,    -1,    -1,    -1,    -1,
      69,    70,    71,    72,    73,    74,     3,     4,     5,     6,
      -1,     8,     9,    61,    -1,    63,    -1,    -1,    66,    -1,
      -1,    69,    70,    71,    72,    73,    74,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    -1,    61,    -1,    63,
      64,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,
      74,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    61,    -1,    63,    -1,    -1,    66,
      -1,    -1,    69,    70,    71,    72,    73,    74,     3,     4,
       5,     6,    -1,     8,     9,    61,    -1,    63,    -1,    -1,
      66,    -1,    -1,    69,    70,    71,    72,    73,    74,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    61,
      -1,    63,    64,    -1,    -1,    -1,    -1,    69,    70,    71,
      72,    73,    74,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    -1,
      -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,    74,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    63,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,
      74,     0,    -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    -1,    63,    -1,    -1,    -1,    -1,    -1,    69,
      70,    71,    72,    73,    74,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,     3,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    -1,    -1,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      64,    65,    -1,    -1,    -1,    -1,    70,    -1,    -1,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,     3,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    -1,    -1,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,
      64,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    -1,
      -1,    -1,    -1,    86,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    -1,    -1,
      83,    -1,    85,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    -1,    -1,    -1,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    63,    64,    65,    -1,    -1,    -1,    -1,    70,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    64,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    63,    70,   109,   110,   113,   114,   115,   116,
     122,   125,   126,   127,   128,   148,   149,   150,     3,    85,
     126,   125,   128,   129,    84,   111,   112,   126,   110,   110,
       3,    85,   110,    85,   109,   110,   141,   142,    63,    65,
     127,     0,   149,    85,     3,   123,   124,    64,   125,   128,
      68,    84,    83,   141,   142,    85,   114,   117,   118,   119,
     125,     3,     4,     5,     6,     8,     9,    49,    50,    51,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    63,
      69,    70,    71,    72,    73,    74,    84,    86,    88,    89,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   107,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   126,   109,   141,     3,    64,
     110,   130,   131,   132,   133,     3,    66,    91,   104,   108,
     123,    83,    68,    86,   112,    85,   105,   137,   141,   117,
     119,    86,   118,    82,   120,   121,   126,   119,    82,    63,
      91,    63,    91,    91,   108,    82,    63,    63,    63,   139,
      63,     3,    84,    84,    84,   107,    63,   107,   119,   134,
       7,     8,     9,    63,    65,    67,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    83,   106,    93,    70,
      75,    76,    71,    72,    10,    11,    12,    13,    77,    78,
      14,    15,    69,    79,    80,    16,    17,    81,    68,    84,
      86,   143,    86,   139,    63,    65,   126,   128,   135,   136,
      64,    68,    64,    68,    66,    86,   108,   124,   137,   138,
      86,   108,    68,    84,    82,   139,   134,    82,   139,   107,
     107,   107,    54,   144,    84,    84,     5,    64,    63,   128,
     135,    64,     3,    64,    90,   105,   107,     3,   105,    93,
      93,    93,    94,    94,    95,    95,    96,    96,    96,    96,
      97,    97,    98,    99,   100,   101,   102,   107,   105,    86,
      64,   130,   135,    66,   108,   136,    63,    65,    48,   132,
       3,    68,    86,   121,   108,    64,   139,    64,    64,    64,
      63,   144,    64,    93,    64,    68,    66,    82,    64,    64,
      66,    64,   130,    66,   108,    86,   137,   139,   139,   139,
     107,    64,   107,   105,   104,    64,    66,    52,    64,   139,
      64,   139,    84,   139
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1806 of yacc.c  */
#line 65 "parser.y"
    { vlog("[parser] IDENTIFIER: %s\n", (yyvsp[(1) - (1)])->id); /* the lexer already made this object a constant */ }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 66 "parser.y"
    { vlog("[parser] CONSTANT: %s\n", (yyvsp[(1) - (1)])->id); /* the lexer already made this object a constant */ }
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 67 "parser.y"
    { vlog("[parser] STRING_LITERAL: %s\n", (yyvsp[(1) - (1)])->id); /* the lexer already made this object a string */ }
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 68 "parser.y"
    { vlog("[parser] EXPRESSION\n"); (yyval) = (yyvsp[(2) - (3)]); }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 69 "parser.y"
    { vlog("[parser] INLINE ASSEMBLY\n"); (yyval) = op("asm", 1, (yyvsp[(3) - (4)])); }
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 74 "parser.y"
    { vlog("[parser] POSTFIX_BRACKET_EXPRESSION\n"); (yyval) = op("postfix_expr", 2, (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 75 "parser.y"
    { vlog("[parser] POSTFIX_PAREN_EXPRESSION\n"); (yyval) = op("postfix_expr", 2, (yyvsp[(1) - (3)]), id("paren")); }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 76 "parser.y"
    { vlog("[parser] POSTFIX_ARG_EXPRESSION_LIST\n"); (yyval) = op("postfix_expr", 2, (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 77 "parser.y"
    { vlog("[parser] POSTFIX_._IDENTIFIER\n"); (yyval) = op("postfix_expr", 3, (yyvsp[(1) - (3)]), id("dot"), (yyvsp[(3) - (3)])); }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 78 "parser.y"
    { vlog("[parser] POSTFIX_PTR_OP_IDENTIFIER\n"); (yyval) = op("postfix_expr", 3, (yyvsp[(1) - (3)]), id("ptr"), (yyvsp[(3) - (3)])); }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 79 "parser.y"
    { vlog("[parser] POSTFIX_INC_OP_EXPRESSION\n"); (yyval) = op("postfix_expr", 2, (yyvsp[(1) - (2)]), id("inc")); }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 80 "parser.y"
    { vlog("[parser] POSTFIX_DEC_OP_EXPRESSION\n"); (yyval) = op("postfix_expr", 2, (yyvsp[(1) - (2)]), id("dec")); }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 84 "parser.y"
    { vlog("[parser] ASSIGNMENT_EXPRESSION\n"); (yyval) = op("argument_expr_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 85 "parser.y"
    { vlog("[parser] ARG_EXPRESSION_LIST_,_ASSIGNMENT_EXPRESSION\n"); (yyval) = add_child((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 90 "parser.y"
    { vlog("[parser] INC_OP_UNARY_EXPRESSION\n"); (yyval) = op("unary_expr", 2, id("inc"), (yyvsp[(2) - (2)])); }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 91 "parser.y"
    { vlog("[parser] DEC_OP_UNARY_EXPRESSION\n"); (yyval) = op("unary_expr", 2, id("dec"), (yyvsp[(2) - (2)])); }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 92 "parser.y"
    { vlog("[parser] UNARY_OP_CAST_EXPRESSION\n"); (yyval) = op("unary_expr", 2, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 93 "parser.y"
    { vlog("[parser] SIZEOF_UNARY_EXP\n"); (yyval) = op("sizeof", 1, (yyvsp[(1) - (2)])); }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 94 "parser.y"
    { vlog("[parser] SIZEOF_TYPE_NAME\n"); (yyval) = op("sizeof", 1, (yyvsp[(3) - (4)])); }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 98 "parser.y"
    { vlog("[parser] &\n"); (yyval) = id("&"); }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 99 "parser.y"
    { vlog("[parser] *\n"); (yyval) = id("*"); }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 100 "parser.y"
    { vlog("[parser] +\n"); (yyval) = id("+"); }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 101 "parser.y"
    { vlog("[parser] -\n"); (yyval) = id("-"); }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 102 "parser.y"
    { vlog("[parser] ~\n"); (yyval) = id("~"); }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 103 "parser.y"
    { vlog("[parser] !\n"); (yyval) = id("!"); }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 108 "parser.y"
    { vlog("[parser] TYPE_NAME_CAST_EXPRESSION\n"); (yyval) = op("cast_expr", 2, (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 113 "parser.y"
    { vlog("[parser] MULTIPLICATIVE_EXPRESSION_*_CAST_EXPRESSION\n"); (yyval) = op("multiply", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 114 "parser.y"
    { vlog("[parser] MULTIPLICATIVE_EXPRESSION_/_CAST_EXPRESSION\n"); (yyval) = op("divide", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 115 "parser.y"
    { vlog("[parser] MULTIPLICATIVE_EXPRESSION_MOD_CAST_EXPRESSION\n"); (yyval) = op("mod", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 120 "parser.y"
    { vlog("[parser] ADDITIVE_EXPRESSION_+_MULTIPLICATIVE_EXPRESSION\n"); (yyval) = op("add", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 121 "parser.y"
    { vlog("[parser] ADDITIVE_EXPRESSION_-_MULTIPLICATIVE_EXPRESSION\n"); (yyval) = op("sub", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 126 "parser.y"
    { vlog("[parser] SHIFT_LEFT_ADDITIVE\n"); (yyval) = op("shift_left", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 127 "parser.y"
    { vlog("[parser] SHIFT_RIGHT_ADDITIVE\n"); (yyval) = op("shift_right", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 132 "parser.y"
    { vlog("[parser] RELATIONAL_<_SHIFT\n"); (yyval) = op("less_than", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 133 "parser.y"
    { vlog("[parser] RELATIONAL_>_SIHFT\n"); (yyval) = op("greater_than", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 134 "parser.y"
    { vlog("[parser] RELATIONAL_LE_SHIFT\n"); (yyval) = op("less_equal_than", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 135 "parser.y"
    { vlog("[parser] RELATIONAL_GE_SHIFT\n"); (yyval) = op("greater_equal_than", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 140 "parser.y"
    { vlog("[parser] EQUALITY_EQ_RELATIONAL\n"); (yyval) = op("equality", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 141 "parser.y"
    { vlog("[parser] EQUALITY_NE_RELATIONAL\n"); (yyval) = op("equality_not", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 146 "parser.y"
    { vlog("[parser] AND_EXPRESSION_&_EQUALITY\n"); (yyval) = op("bitwise_and", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 151 "parser.y"
    { vlog("[parser] XOR_^_AND_EXPRESSION\n"); (yyval) = op("bitwise_xor", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 156 "parser.y"
    { vlog("[parser] OR_|_XOR\n"); (yyval) = op("bitwise_or", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 161 "parser.y"
    { vlog("[parser] LOGICAL_AND_&&_OR\n"); (yyval) = op("logical_and", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 166 "parser.y"
    { vlog("[parser] LOGICAL_OR_||_LOGICAL_AND\n"); (yyval) = op("logical_or", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 171 "parser.y"
    { vlog("[parser] LOGICAL_OR_?\n"); (yyval) = op("conditional", 3, (yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)])); }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 176 "parser.y"
    { vlog("[parser] UNARY_ASSIGNMENT\n"); (yyval) = op("assignment", 3, (yyvsp[(1) - (3)]), (yyvsp[(2) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 180 "parser.y"
    { vlog("[parser] =\n"); (yyval) = op("assign", 0);}
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 181 "parser.y"
    { vlog("[parser] *=\n"); (yyval) = op("assign_mul", 0); }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 182 "parser.y"
    { vlog("[parser] /=\n"); (yyval) = op("assign_div", 0); }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 183 "parser.y"
    { vlog("[parser] MOD=\n"); (yyval) = op("assign_mod", 0); }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 184 "parser.y"
    { vlog("[parser] +=\n"); (yyval) = op("assign_add", 0); }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 185 "parser.y"
    { vlog("[parser] -=\n"); (yyval) = op("assign_sub", 0); }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 186 "parser.y"
    { vlog("[parser] <<=\n"); (yyval) = op("assign_sll", 0); }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 187 "parser.y"
    { vlog("[parser] >>=\n"); (yyval) = op("assign_srl", 0); }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 188 "parser.y"
    { vlog("[parser] &=\n"); (yyval) = op("assign_and", 0); }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 189 "parser.y"
    { vlog("[parser] ^=\n"); (yyval) = op("assign_xor", 0); }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 190 "parser.y"
    { vlog("[parser] |=\n"); (yyval) = op("assign_or", 0); }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 194 "parser.y"
    { vlog("[parser] ASSIGNMENT_EXPRESSION\n"); (yyval) = op("expression", 1, (yyvsp[(1) - (1)])); }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 195 "parser.y"
    { vlog("[parser] EXPRESSION_,_ASSIGNMENT_EXPRESSION\n"); (yyval) = add_child((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 203 "parser.y"
    { vlog("[parser] DECLARATION_SPECIFIERS_;\n"); (yyval) = op("declaration", 1, (yyvsp[(1) - (2)])); (yyval) = install_symbol(sym, (yyval)); }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 204 "parser.y"
    { vlog("[parser] DECLARATION_SPECIFIERS_INIT_DECLARATOR_LIST_;\n"); (yyval) = op("declaration", 2, (yyvsp[(1) - (3)]), (yyvsp[(2) - (3)])); (yyval) = install_symbol(sym, (yyval)); }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 208 "parser.y"
    { vlog("[parser] STORAGE_CLASS_SPECIFIER\n"); (yyval) = op("declaration_specifier", 1, (yyvsp[(1) - (1)])); }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 209 "parser.y"
    { vlog("[parser] STORAGE_CLASS_SPECIFIER_DECLARATION_SPECIFIERS\n"); (yyval) = add_child((yyvsp[(2) - (2)]), (yyvsp[(1) - (2)])); }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 210 "parser.y"
    { vlog("[parser] DECLARATION_SPECIFIER_TYPE_SPECIFIER\n"); (yyval) = op("declaration_specifier", 1, (yyvsp[(1) - (1)])); }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 211 "parser.y"
    { vlog("[parser] TYPE_SPECIFIER_DECLARATION_SPECIFIERS\n"); (yyval) = add_child((yyvsp[(2) - (2)]), (yyvsp[(1) - (2)])); }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 212 "parser.y"
    { vlog("[parser] TYPE_QUALIFIER\n"); (yyval) = op("declaration_specifier", 1, (yyvsp[(1) - (1)])); }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 213 "parser.y"
    { vlog("[parser] TYPE_QUALIFIER_DECLARATION_SPECIFIERS\n"); (yyval) = add_child((yyvsp[(2) - (2)]), (yyvsp[(1) - (2)])); }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 217 "parser.y"
    { vlog("[parser] INIT_DECLARATOR\n"); (yyval) = op("init_declarator_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 218 "parser.y"
    { vlog("[parser] INIT_DECLARATOR_LIST_,_INIT_DECLARATOR\n"); (yyval) = add_child((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 222 "parser.y"
    { vlog("[parser] DECLARATOR\n"); (yyval) = op("init_declarator", 1, (yyvsp[(1) - (1)])); }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 223 "parser.y"
    { vlog("[parser] DECLARATOR_=_INITIALIZER\n"); (yyval) = op("init_declarator", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 227 "parser.y"
    { vlog("[parser] TYPEDEF\n"); (yyval) = type("typedef"); }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 228 "parser.y"
    { vlog("[parser] EXTERN\n"); (yyval) = type("extern"); }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 229 "parser.y"
    { vlog("[parser] STATIC\n"); (yyval) = type("static"); }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 230 "parser.y"
    { vlog("[parser] AUTO\n"); (yyval) = type("auto"); }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 231 "parser.y"
    { vlog("[parser] REGISTER\n"); (yyval) = type("register"); }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 235 "parser.y"
    { vlog("[parser] VOID\n"); (yyval) = type("void"); }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 236 "parser.y"
    { vlog("[parser] CHAR\n"); (yyval) = type("char"); }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 237 "parser.y"
    { vlog("[parser] SHORT\n"); (yyval) = type("short"); }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 238 "parser.y"
    { vlog("[parser] INT\n"); (yyval) = type("int"); }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 239 "parser.y"
    { vlog("[parser] LONG\n"); (yyval) = type("long"); }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 240 "parser.y"
    { vlog("[parser] FLOAT\n"); (yyval) = type("float"); }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 241 "parser.y"
    { vlog("[parser] DOUBLE\n"); (yyval) = type("double"); }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 242 "parser.y"
    { vlog("[parser] SIGNED\n"); (yyval) = type("signed"); }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 243 "parser.y"
    { vlog("[parser] UNSIGNED\n"); (yyval) = type("unsigned"); }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 244 "parser.y"
    { vlog("[parser] STRUCT_UNION\n"); /* do nothing */ }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 245 "parser.y"
    { vlog("[parser] ENUM\n"); (yyval) = type("enum"); }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 246 "parser.y"
    { vlog("[parser] TYPE_NAME\n"); (yyval) = type((yyvsp[(1) - (1)])->id); }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 250 "parser.y"
    { vlog("[parser] STRUCT_IDENTIFIER_{}\n"); (yyval) = op("struct_union", 3, (yyvsp[(1) - (5)]), (yyvsp[(2) - (5)]), (yyvsp[(4) - (5)])); }
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 251 "parser.y"
    { vlog("[parser] STRUCT_{}\n"); (yyval) = op("struct_union", 2, (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 252 "parser.y"
    { vlog("[parser] STRUCT_IDENTIFIER\n"); (yyval) = op("struct_union", 2, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 256 "parser.y"
    { vlog("[parser] STRUCT\n"); (yyval) = type("struct"); }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 257 "parser.y"
    { vlog("[parser] UNION\n"); (yyval) = type("union"); }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 261 "parser.y"
    { vlog("[parser] STRUCT_DECLARATION\n"); (yyval) = op("struct_declaration_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 262 "parser.y"
    { vlog("[parser] STRUCT_DECLARATION_LIST\n"); (yyval) = add_child((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 266 "parser.y"
    { vlog("[parser] SPECIFIER_QUALIFIER_LIST_STRUCT_DECLARATOR_LIST\n"); (yyval) = op("struct_declaration", 2, (yyvsp[(1) - (3)]), (yyvsp[(2) - (3)])); }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 270 "parser.y"
    { vlog("[parser] TYPE_SPECIFIER_SPEC_QUALIFIER_LIST\n"); (yyval) = add_child((yyvsp[(2) - (2)]), (yyvsp[(1) - (2)])); }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 271 "parser.y"
    { vlog("[parser] SPECIFIER_QUALIFIER_LIST_TYPE_SPECIFIER\n"); (yyval) = op("specifier_qualifier_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 272 "parser.y"
    { vlog("[parser] TYPE_QUALIFIER_SPEC_QUALIFIER_LIST\n"); (yyval) = add_child((yyvsp[(2) - (2)]), (yyvsp[(1) - (2)])); }
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 273 "parser.y"
    { vlog("[parser] TYPE_QUALIFIER\n"); (yyval) = op("specifier_qualifier_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 118:

/* Line 1806 of yacc.c  */
#line 277 "parser.y"
    { vlog("[parser] STRUCT_DECLARATOR\n"); (yyval) = op("struct_declarator_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 278 "parser.y"
    { vlog("[parser] STRUCT_DECLARATOR_LIST_,_STRUCT_DECLARATOR\n"); (yyval) = add_child((yyvsp[(1) - (3)]), (yyvsp[(2) - (3)])); }
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 282 "parser.y"
    { vlog("[parser] DECLARATOR\n"); (yyval) = op("struct_declarator", 1, (yyvsp[(1) - (1)])); }
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 283 "parser.y"
    { vlog("[parser] :_CONSTANT\n"); (yyval) = op("struct_declarator", 1, (yyvsp[(2) - (2)])); }
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 284 "parser.y"
    { vlog("[parser] DECLARATOR_:_CONSTANT\n"); (yyval) = op("struct_declarator", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 288 "parser.y"
    { vlog("[parser] ENUM_{}\n"); (yyval) = op("enum_specifier", 1, (yyvsp[(3) - (4)])); }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 289 "parser.y"
    { vlog("[parser] ENUM_IDENTIFIER_{}\n"); (yyval) = op("enum_specifier", 2, (yyvsp[(2) - (5)]), (yyvsp[(4) - (5)])); }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 290 "parser.y"
    { vlog("[parser] ENUM_IDENTIFIER\n"); (yyval) = op("enum_specifier", 1, (yyvsp[(2) - (2)])); }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 294 "parser.y"
    { vlog("[parser] ENUM\n"); (yyval) = op("enum_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 295 "parser.y"
    { vlog("[parser] ENUM_LIST_,_ENUM\n"); (yyval) = add_child((yyvsp[(1) - (3)]), (yyvsp[(2) - (3)])); }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 299 "parser.y"
    { vlog("[parser] ENUMERATOR_IDENTIFIER: %s\n", (yyvsp[(1) - (1)])->id); (yyval) = op("enumerator", 1, (yyvsp[(1) - (1)])); }
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 300 "parser.y"
    { vlog("[parser] ENUMERATOR_IDENTIFIER_=_CONSTANT\n"); (yyval) = op("enumerator", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 304 "parser.y"
    { vlog("[parser] CONST\n"); (yyval) = type("const"); }
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 305 "parser.y"
    { vlog("[parser] VOLATILE\n"); (yyval) = type("volatile"); }
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 309 "parser.y"
    { vlog("[parser] POINTER_DIRECT_DECLARATOR\n"); (yyval) = op("declarator", 2, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 310 "parser.y"
    { vlog("[parser] DIRECT_DECLARATOR\n"); (yyval) = op("declarator", 1, (yyvsp[(1) - (1)])); }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 314 "parser.y"
    { vlog("[parser] DIRECT_DECLARATOR_IDENTIFIER: %s\n", (yyvsp[(1) - (1)])->id); (yyval) = op("direct_declarator", 1, (yyvsp[(1) - (1)])); }
    break;

  case 135:

/* Line 1806 of yacc.c  */
#line 315 "parser.y"
    { vlog("[parser] (_DECLARATOR_)\n"); (yyval) = op("direct_declarator", 1, (yyvsp[(2) - (3)])); }
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 316 "parser.y"
    { vlog("[parser] DIRECT_DECLARATOR_[_CONSTANT_]\n"); (yyval) = add_child((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 317 "parser.y"
    { vlog("[parser] DIRECT_DECLARATOR_[]\n"); }
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 318 "parser.y"
    { vlog("[parser] DIRECT_DECLARATOR_(_PARAM_TYPE_LIST_)\n"); (yyval) = add_child((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 139:

/* Line 1806 of yacc.c  */
#line 319 "parser.y"
    { vlog("[parser] DIRECT_DECLARATOR_(_IDENTIFIER_LIST_)\n"); (yyval) = add_child((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 320 "parser.y"
    { vlog("[parser] DIRECT_DECLARATOR_()\n"); }
    break;

  case 141:

/* Line 1806 of yacc.c  */
#line 324 "parser.y"
    { vlog("[parser] POINTER_*\n"); (yyval) = op("pointer", 0); }
    break;

  case 142:

/* Line 1806 of yacc.c  */
#line 325 "parser.y"
    { vlog("[parser] POINTER_*_TYPE_QUALIFIER_LIST\n"); (yyval) = op("pointer", 1, (yyvsp[(2) - (2)])); }
    break;

  case 143:

/* Line 1806 of yacc.c  */
#line 326 "parser.y"
    { vlog("[parser] POINTER_*_POINTER\n"); (yyval) = op("pointer", 1, (yyvsp[(2) - (2)])); }
    break;

  case 144:

/* Line 1806 of yacc.c  */
#line 327 "parser.y"
    { vlog("[parser] POINTER_*_TYPE_QUALIFIER_LIST_POINTER\n"); (yyval) = op("pointer", 2, (yyvsp[(2) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 145:

/* Line 1806 of yacc.c  */
#line 331 "parser.y"
    { vlog("[parser] TYPE_QUALIFIER\n"); (yyval) = op("type_qualifier_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 332 "parser.y"
    { vlog("[parser] TYPE_QUALIFIER_LIST_TYPE_QUALIFIER\n"); (yyval) = add_child((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 337 "parser.y"
    { vlog("[parser] PARAMETER_LIST\n"); (yyval) = op("parameter_type_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 338 "parser.y"
    { vlog("[parser] PARAMETER_LIST_,_ELLIPSIS\n"); (yyval) = add_child((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 342 "parser.y"
    { vlog("[parser] PARAMETER_DECLARATION\n"); (yyval) = op("parameter_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 150:

/* Line 1806 of yacc.c  */
#line 343 "parser.y"
    { vlog("[parser] PARAMETER_LIST_,_PARAMETER_DECLARATION\n"); (yyval) = add_child((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 151:

/* Line 1806 of yacc.c  */
#line 347 "parser.y"
    { vlog("[parser] DECLARATION_SPEC_DECLARATOR\n"); (yyval) = op("parameter_declaration", 2, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 152:

/* Line 1806 of yacc.c  */
#line 348 "parser.y"
    { vlog("[parser] DECLARATION_SPEC_ABSTRACT_DECLARATOR\n"); (yyval) = op("parameter_declaration", 2, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 153:

/* Line 1806 of yacc.c  */
#line 349 "parser.y"
    { vlog("[parser] DECLARATION_SPEC\n"); (yyval) = op("parameter_declaration", 1, (yyvsp[(1) - (1)])); }
    break;

  case 154:

/* Line 1806 of yacc.c  */
#line 353 "parser.y"
    { vlog("[parser] IDENTIFIER_LIST_IDENTIFIER: %s\n", (yyvsp[(1) - (1)])->id); (yyval) = op("identifier_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 155:

/* Line 1806 of yacc.c  */
#line 354 "parser.y"
    { vlog("[parser] IDENTIFIER_LIST_,_IDENTIFIER\n"); (yyval) = add_child((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 156:

/* Line 1806 of yacc.c  */
#line 358 "parser.y"
    { vlog("[parser] SPECIFIER_QUALIFIER_LIST\n"); (yyval) = op("type_name", 1, (yyvsp[(1) - (1)])); }
    break;

  case 157:

/* Line 1806 of yacc.c  */
#line 359 "parser.y"
    { vlog("[parser] SPECIFIER_QUALIFIER_LIST_ABSTRACT_DECLARATOR\n"); (yyval) = op("type_name", 2, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 363 "parser.y"
    { vlog("[parser] POINTER\n"); (yyval) = op("abstract_declarator", 1, (yyvsp[(1) - (1)])); }
    break;

  case 159:

/* Line 1806 of yacc.c  */
#line 364 "parser.y"
    { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR\n"); (yyval) = op("abstract_declarator", 1, (yyvsp[(1) - (1)])); }
    break;

  case 160:

/* Line 1806 of yacc.c  */
#line 365 "parser.y"
    { vlog("[parser] POINTER_DIRECT_ABSTRACT_DECLARATOR\n"); (yyval) = op("abstract_declarator", 2, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 161:

/* Line 1806 of yacc.c  */
#line 369 "parser.y"
    { vlog("[parser] (_ABSTRACT_DECLARATOR_)\n"); (yyval) = op("direct_abstract_declarator", 1, (yyvsp[(2) - (3)])); }
    break;

  case 162:

/* Line 1806 of yacc.c  */
#line 370 "parser.y"
    { vlog("[parser] []\n"); (yyval) = op("direct_abstract_declarator", 0); }
    break;

  case 163:

/* Line 1806 of yacc.c  */
#line 371 "parser.y"
    { vlog("[parser] [_CONSTANT_]\n"); (yyval) = op("direct_abstract_declarator", 1, (yyvsp[(2) - (3)])); }
    break;

  case 164:

/* Line 1806 of yacc.c  */
#line 372 "parser.y"
    { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_[]\n"); /* do nothing */}
    break;

  case 165:

/* Line 1806 of yacc.c  */
#line 373 "parser.y"
    { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_[_CONSTANT_]\n"); (yyval) = add_child((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 166:

/* Line 1806 of yacc.c  */
#line 374 "parser.y"
    { vlog("[parser] ()\n"); (yyval) = op("direct_abstract_declarator", 0); }
    break;

  case 167:

/* Line 1806 of yacc.c  */
#line 375 "parser.y"
    { vlog("[parser] (_PARAM_TYPE_LIST_)\n"); (yyval) = op("direct_abstract_declarator", 1, (yyvsp[(2) - (3)])); }
    break;

  case 168:

/* Line 1806 of yacc.c  */
#line 376 "parser.y"
    { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_()\n"); /* do nothing */}
    break;

  case 169:

/* Line 1806 of yacc.c  */
#line 377 "parser.y"
    { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_(_PARAM_TYPE_LIST_)\n"); (yyval) = add_child((yyvsp[(1) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 170:

/* Line 1806 of yacc.c  */
#line 381 "parser.y"
    { vlog("[parser] ASSIGNMENT_EXPRESSION\n"); (yyval) = op("initializer", 1, (yyvsp[(1) - (1)])); }
    break;

  case 171:

/* Line 1806 of yacc.c  */
#line 382 "parser.y"
    { vlog("[parser] {_INIT_LIST_}\n"); (yyval) = op("initializer", 1, (yyvsp[(2) - (3)])); }
    break;

  case 172:

/* Line 1806 of yacc.c  */
#line 383 "parser.y"
    { vlog("[parser] {_INIT_LIST_,_}\n"); (yyval) = op("initializer", 1, (yyvsp[(2) - (4)])); }
    break;

  case 173:

/* Line 1806 of yacc.c  */
#line 387 "parser.y"
    { vlog("[parser] INITIALIZER\n"); (yyval) = op("initializer_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 174:

/* Line 1806 of yacc.c  */
#line 388 "parser.y"
    { vlog("[parser] INITIALIZER_LIST_,_INITIALIZER\n"); (yyval) = add_child((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 175:

/* Line 1806 of yacc.c  */
#line 392 "parser.y"
    { vlog("[parser] LABELED_STATEMENT\n"); }
    break;

  case 176:

/* Line 1806 of yacc.c  */
#line 393 "parser.y"
    { vlog("[parser] COMPOUND_STATEMENT\n"); }
    break;

  case 177:

/* Line 1806 of yacc.c  */
#line 394 "parser.y"
    { vlog("[parser] EXPRESSION_STATEMENT\n"); }
    break;

  case 178:

/* Line 1806 of yacc.c  */
#line 395 "parser.y"
    { vlog("[parser] SELECTION_STATEMENT\n"); }
    break;

  case 179:

/* Line 1806 of yacc.c  */
#line 396 "parser.y"
    { vlog("[parser] ITERATION_STATEMENT\n"); }
    break;

  case 180:

/* Line 1806 of yacc.c  */
#line 397 "parser.y"
    { vlog("[parser] JUMP_STATEMENT\n"); }
    break;

  case 181:

/* Line 1806 of yacc.c  */
#line 401 "parser.y"
    { vlog("[parser] IDENTIFIER_:_STATEMENT\n"); (yyval) = op("labeled_statement", 2, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])); }
    break;

  case 182:

/* Line 1806 of yacc.c  */
#line 402 "parser.y"
    { vlog("[parser] CASE_CONSTANT_:_STATEMENT\n"); (yyval) = op("labeled_statement", 2, (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 183:

/* Line 1806 of yacc.c  */
#line 403 "parser.y"
    { vlog("[parser] DEFAULT_:_STATEMENT\n"); (yyval) = op("labeled_statement", 1, (yyvsp[(3) - (3)])); }
    break;

  case 184:

/* Line 1806 of yacc.c  */
#line 407 "parser.y"
    { vlog("[parser] {}\n"); (yyval) = op("compound_statement", 0); }
    break;

  case 185:

/* Line 1806 of yacc.c  */
#line 408 "parser.y"
    { vlog("[parser] {_STATEMENT_LIST_}\n"); (yyval) = op("compound_statement", 1, (yyvsp[(2) - (3)])); }
    break;

  case 186:

/* Line 1806 of yacc.c  */
#line 409 "parser.y"
    { vlog("[parser] {_DECLARATION_LIST_}\n"); (yyval) = op("compound_statement", 1, (yyvsp[(2) - (3)])); }
    break;

  case 187:

/* Line 1806 of yacc.c  */
#line 410 "parser.y"
    { vlog("[parser] {_DECLARATION_LIST_STATEMENT_LIST_}\n"); (yyval) = op("compound_statement", 2, (yyvsp[(2) - (4)]), (yyvsp[(3) - (4)])); }
    break;

  case 188:

/* Line 1806 of yacc.c  */
#line 414 "parser.y"
    { vlog("[parser] DECLARATION\n"); (yyval) = op("declaration_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 189:

/* Line 1806 of yacc.c  */
#line 415 "parser.y"
    { vlog("[parser] DECLARATION_LIST_DECLARATION\n"); (yyval) = add_child((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 190:

/* Line 1806 of yacc.c  */
#line 419 "parser.y"
    { vlog("[parser] STATEMENT\n"); (yyval) = op("statement_list", 1, (yyvsp[(1) - (1)])); }
    break;

  case 191:

/* Line 1806 of yacc.c  */
#line 420 "parser.y"
    { vlog("[parser] STATEMENT_LIST_STATEMENT\n"); (yyval) = add_child((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); }
    break;

  case 192:

/* Line 1806 of yacc.c  */
#line 424 "parser.y"
    { vlog("[parser] ;\n"); (yyval) = op("expression_statement", 0); }
    break;

  case 193:

/* Line 1806 of yacc.c  */
#line 425 "parser.y"
    { vlog("[parser] EXPRESSION_;\n"); (yyval) = op("expression_statement", 1, (yyvsp[(1) - (2)])); }
    break;

  case 194:

/* Line 1806 of yacc.c  */
#line 429 "parser.y"
    { vlog("[parser] IF_(_EXPRESSION_)_STATEMENT\n"); (yyval) = op("selection_statement", 2, (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)])); }
    break;

  case 195:

/* Line 1806 of yacc.c  */
#line 430 "parser.y"
    { vlog("[parser] IF_(_EXPRESSION_)_STATEMENT_ELSE_STATEMENT\n"); (yyval) = op("selection_statement", 3, (yyvsp[(3) - (7)]), (yyvsp[(5) - (7)]), (yyvsp[(7) - (7)])); }
    break;

  case 196:

/* Line 1806 of yacc.c  */
#line 431 "parser.y"
    { vlog("[parser] SWITCH_(_EXPRESSION_)_STATEMENT\n"); (yyval) = op("selection_statement", 2, (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)])); }
    break;

  case 197:

/* Line 1806 of yacc.c  */
#line 435 "parser.y"
    { vlog("[parser] WHILE_(_EXPRESSION_)_STATEMENT\n"); (yyval) = op("iteration_statement", 2, (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)])); }
    break;

  case 198:

/* Line 1806 of yacc.c  */
#line 436 "parser.y"
    { vlog("[parser] DO_STATEMENT_WHILE_(_EXPRESSION_)_;\n"); (yyval) = op("iteration_statement", 2, (yyvsp[(2) - (7)]), (yyvsp[(5) - (7)])); }
    break;

  case 199:

/* Line 1806 of yacc.c  */
#line 437 "parser.y"
    { vlog("[parser] FOR_(_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_)_STATEMENT\n"); (yyval) = op("iteration_statement", 3, (yyvsp[(3) - (6)]), (yyvsp[(4) - (6)]), (yyvsp[(6) - (6)])); }
    break;

  case 200:

/* Line 1806 of yacc.c  */
#line 438 "parser.y"
    { vlog("[parser] FOR_(_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_EXPRESSION_)_STATEMENT\n"); (yyval) = op("iteration_statement", 4, (yyvsp[(3) - (7)]), (yyvsp[(4) - (7)]), (yyvsp[(5) - (7)]), (yyvsp[(7) - (7)])); }
    break;

  case 201:

/* Line 1806 of yacc.c  */
#line 442 "parser.y"
    { vlog("[parser] GOTO_IDENTIFIER\n"); (yyval) = op("jump_statement", 2, id("goto"), (yyvsp[(2) - (3)])); }
    break;

  case 202:

/* Line 1806 of yacc.c  */
#line 443 "parser.y"
    { vlog("[parser] CONTINUE\n"); (yyval) = op("jump_statement", 1, id("continue")); }
    break;

  case 203:

/* Line 1806 of yacc.c  */
#line 444 "parser.y"
    { vlog("[parser] BREAK\n"); (yyval) = op("jump_statement", 1, id("break")); }
    break;

  case 204:

/* Line 1806 of yacc.c  */
#line 445 "parser.y"
    { vlog("[parser] RETURN\n"); (yyval) = op("jump_statement", 1, id("return")); }
    break;

  case 205:

/* Line 1806 of yacc.c  */
#line 446 "parser.y"
    { vlog("[parser] RETURN_EXPRESSION\n"); (yyval) = op("jump_statement", 2, id("return"), (yyvsp[(2) - (3)])); }
    break;

  case 206:

/* Line 1806 of yacc.c  */
#line 450 "parser.y"
    { vlog("[parser] EXTERNAL_DECLARATION\n"); (yyval) = op("translation_unit", 1, (yyvsp[(1) - (1)])); parse_tree_head = (yyval); }
    break;

  case 207:

/* Line 1806 of yacc.c  */
#line 451 "parser.y"
    { vlog("[parser] TRANSLATION_UNIT_EXTERNAL_DECLARATION\n"); (yyval) = add_child((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); parse_tree_head = (yyval); }
    break;

  case 208:

/* Line 1806 of yacc.c  */
#line 455 "parser.y"
    { vlog("[parser] FUNCTION_DEFINITION\n"); }
    break;

  case 209:

/* Line 1806 of yacc.c  */
#line 456 "parser.y"
    { vlog("[parser] DECLARATION\n"); }
    break;

  case 210:

/* Line 1806 of yacc.c  */
#line 460 "parser.y"
    { vlog("[parser] DECLARATION_SPECIFIERS_DECLARATOR_DECLARATION_LIST_COMPOUND_STATEMENT\n"); (yyval) = op("function_definition", 4, (yyvsp[(1) - (4)]), (yyvsp[(2) - (4)]), (yyvsp[(3) - (4)]), (yyvsp[(4) - (4)])); (yyval) = install_function(sym, (yyval)); }
    break;

  case 211:

/* Line 1806 of yacc.c  */
#line 461 "parser.y"
    { vlog("[parser] DECLARATION_SPECIFIERS_DECLARATOR_COMPOUND_STATEMENT\n"); (yyval) = op("function_definition", 3, (yyvsp[(1) - (3)]), (yyvsp[(2) - (3)]), (yyvsp[(3) - (3)])); (yyval) = install_function(sym, (yyval)); }
    break;

  case 212:

/* Line 1806 of yacc.c  */
#line 462 "parser.y"
    { vlog("[parser] DECLARATOR_DECLARATION_LIST_COMPOUND_STATEMENT\n"); (yyval) = op("function_definition", 3, (yyvsp[(1) - (3)]), (yyvsp[(2) - (3)]), (yyvsp[(3) - (3)])); (yyval) = install_function(sym, (yyval)); }
    break;

  case 213:

/* Line 1806 of yacc.c  */
#line 463 "parser.y"
    { vlog("[parser] DECLARATOR_COMPOUND_STATEMENT\n"); (yyval) = op("function_definition", 2, (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])); (yyval) = install_function(sym, (yyval)); }
    break;



/* Line 1806 of yacc.c  */
#line 3362 "parser.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 466 "parser.y"



