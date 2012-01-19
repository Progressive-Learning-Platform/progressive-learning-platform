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


/*
 * input/output conversion and bit part selection mechanism routines
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>

#ifdef __DBMALLOC__
#include "../malloc.h"
#endif
#include "v.h"
#include "cvmacros.h"

/* local prototypes */
static void disp_toexprline(register struct expr_t *, int32);
static char *sep_real_fmt(char *, char *, int32);
static void sdisph(word32 *, word32 *, int32, int32);
static char bitsto_char(word32, word32, int32);
static void sdispd(word32 *, word32 *, int32, int32, int32);
static char get_decxz(word32 *, word32 *, int32);
static int32 bld_tfmt_val(char *, struct expr_t *, word32 *, word32 *, int32,
 int32, int32);
static void sdispo(word32 *, word32 *, int32, int32);
static void vstr_to_cstr(char *, int32, int32, int32, int32);
static void disp_ufmt_binval(word32 *, word32 *, int32);
static void disp_zfmt_binval(word32 *, word32 *, int32);
static void numexpr_disp(struct expr_t *, int32);
static void st_regab_disp(byte *, int32);
static void disp_stvar(struct net_t *, int32, int32);
static int32 get_ovrsign(struct net_t *, char);
static void dmp_arr_insts(struct net_t *, int32, int32);
static void dmp_arr(struct net_t *, int32, int32, int32);
static void dmp1n_nplst(struct mod_t *, struct net_t *, int32);
static int32 cnt_nplstels(register struct net_pin_t *);
static word32 get_dce_edgeval(struct mod_t *, struct dcevnt_t *);
static int32 bin_trim_abval(word32 *, word32 *, int32);
static int32 trim_abval(word32 *, word32 *, int32);
static int32 bithi_is0(word32, int32);
static int32 trim1_1val(word32 *, int32);
static int32 vval_isall_xs(word32 *, word32 *, int32);
static int32 vval_hasx(word32 *, word32 *, int32);
static long my_strtol(char *, char **, int32, int32 *);
static void dmp_dig_attr_list(FILE *, struct attr_t *, int32);
static void dmp_modports(FILE *, struct mod_t *);
static void dmp_mod_lofp_hdr(FILE *, struct mod_t *);
static void dmp_decls(FILE *, struct mod_t *);
static void dmp_1portdecl(FILE *, struct expr_t *);
static void dmp_1netdecl(FILE *, struct net_t *);
static int32 nd_iowirdecl(struct net_t *);
static void dmp_paramdecls(FILE *, struct net_t *, int32, char *);
static void dmp_defparams(FILE *, struct mod_t *);
static void dmp_mdtasks(FILE *, struct mod_t *);
static void dmp_insts(FILE *, struct mod_t *);
/* DBG */ static void dbg_dmp_insts(FILE *, struct mod_t *);
static void dmp_1inst(FILE *, struct inst_t *, struct giarr_t *);
/* DBG */ static void dbg_dmp_1inst(FILE *f, struct inst_t *ip, char *inam);
static void dmp_pnd_params(FILE *, struct inst_t *, struct mod_t *);
static int32 impl_pndparams(struct inst_t *, struct mod_t *);
static void dmp_iports(FILE *, struct inst_t *, struct expr_t **);
static void dmp_1gate(FILE *, struct gate_t *, struct giarr_t *);
/* DBG */ static void dbg_dmp_1gate(FILE *f, struct gate_t *gp, char *gnam);
static void dmp_1conta(FILE *, struct conta_t *);
static void dmp_1bitconta(FILE *, struct gate_t *);
static void dmp_ialst(FILE *, struct mod_t *);
static void dmp_case(FILE *, struct st_t *);
static void dmp_case_dflt(FILE *, struct csitem_t *);
static void dmp_fj_stlst(FILE *, struct st_t *);
static void dmp_task(FILE *, struct task_t *);
static void dmp_func_decl(FILE *, struct task_t *);
static void dmp_nblock(FILE *, struct task_t *, char *);
static void dmp_tfdecls(FILE *, struct task_t *);
static void dmp_tf_lofp_hdr(FILE *, struct task_t *);
static void dmp_mdspfy(FILE *, struct mod_t *);
static void dmp_specpths(FILE *, register struct spcpth_t *);
static void dmp_pthlst(FILE *, struct spcpth_t *, int32);
static void dmp_pthel(FILE *, struct pathel_t *);
static void dmp_tchks(FILE *, register struct tchk_t *);
static void dmp_tchk_selector(FILE *, word32, struct expr_t *,
 struct expr_t *);
static void dmp_mod_grefs(FILE *, struct mod_t *);
static char *to_glbinfo(char *, struct gref_t *);
static void dmp_casesel(FILE *, struct st_t *);
static void dmp_delay(FILE *, union del_u, word32, char *);
static void dmp_dellst(FILE *, register struct paramlst_t *);
static void dmp_lstofsts(FILE *, struct st_t *);
static void tox_wrange(FILE *, struct expr_t *, struct expr_t *);
static void dmp_expr(FILE *, struct expr_t *);
static int32 is_simplex(struct expr_t *);
static void dmp_catexpr(FILE *, struct expr_t *);
static void dmp_catel(FILE *, struct expr_t *);
static void dmp_fcallx(FILE *, struct expr_t *);
static void dmp_evor_chain(FILE *, struct expr_t *);
static void sdisp_st(struct expr_t *);
static int32 find_deepest_level(struct optlst_t *);
static int32 cnt_beg_to_endmark(struct optlst_t *, int32);
static int32 cnt_level0(struct optlst_t *);
static void dump_vpi_argv(int32, char **);
static void dump_nest_vpi_argv(int32, char **);
static void dmp1_optlst(struct optlst_t *, char *);


/* extern prototypes defined elsewhere */
extern char *__my_realloc(char *, int32, int32);
extern char *__my_malloc(int32);
extern char *__mytf_malloc(int32);
extern char *__pv_stralloc(char *);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern char *__msgtox_wrange(char *, struct expr_t *, struct expr_t *);
extern char *__to_wtnam(char *, struct net_t *);
extern char *__to_wtnam2(char *, word32);
extern char *__to_ptnam(char *, word32);
extern char *__to_wrange(char *, struct net_t *);
extern char *__to_stren_nam(char *, int32, int32);
extern char *__to_stval_nam(char *, word32);
extern char *__to_opname(word32);
extern char *__to_vvstnam(char *, word32);
extern char *__to_sytyp(char *, word32);
extern char *__to_tcnam(char *, word32);
extern char *__to_edgenam(char *, word32);
extern char *__to_timunitnam(char *, word32);
extern char *__msg_blditree(char *, struct itree_t *, struct task_t *);
extern char *__msg2_blditree(char *, struct itree_t *);
extern char *__regab_tostr(char *, word32 *, word32 *, int32, int32, int32);
extern char *__xregab_tostr(char *, word32 *, word32 *, int32, struct expr_t *);
extern char *__strab_tostr(char *, word32 *, int32, int32, int32);
extern char *__vval_to_vstr(word32 *, int32, int32 *);
extern char *__to_arr_range(char *, struct net_t *);
extern char *__to_mpnam(char *, char *);
extern struct xstk_t *__eval2_xpr(struct expr_t *);
extern struct xstk_t *__ndst_eval_xpr(struct expr_t *);
extern char *__to_timstr(char *, word64 *);
extern char *__to_vvnam(char *, word32);
extern char *__bld_lineloc(char *, word32, int32);
extern struct task_t *__find_thrdtsk(struct thread_t *);
extern char *__alloc_vval_to_cstr(word32 *, int32, int32, int32);
extern char *__to_sttyp(char *, word32);

extern struct expr_t *__disp_1fmt_to_exprline(char *, struct expr_t *);
extern struct task_t *__getcur_scope_tsk(void);
extern void __wrap_puts(char *, FILE *);
extern void __wrap_putc(int32, FILE *);
extern void __nl_wrap_puts(char *, FILE *);
extern void __chg_xprline_size(int32);
extern void __disp_itree_path(register struct itree_t *, struct task_t *);
extern void __declcnv_tostr(char *, word32 *, int32, int32);
extern void __adds(char *);
extern void __sdispb(register word32 *, register word32 *, int32, int32);
extern int32 __trim1_0val(word32 *, int32);
extern void __xline_vval_to_cstr(word32 *, int32, int32, int32, int32);
extern void __my_free(char *, int32);
extern void __regab_disp(word32 *, word32 *, int32, int32, int32, int32);
extern void __trunc_cstr(char *, int32, int32);
extern int32 __vval_isallzs(word32 *, word32 *, int32);
extern void __grow_xstk(void);
extern void __chg_xstk_width(struct xstk_t *, int32);
extern void __by16_ldivmod(word32 *, word32 *, word32 *, word32, int32);
extern int32 __wide_vval_is0(register word32 *, int32);
extern int32 __v64_to_real(double *, word64 *);
extern void __cnv_stk_fromreal_toreg32(struct xstk_t *);
extern void __cnv_stk_fromreg_toreal(struct xstk_t *, int32);
extern void __rhspsel(register word32 *, register word32 *, register int32,
 register int32);
extern void __disp_var(struct net_t *, int32, int32, int32, char);
extern int32 __cnt_dcelstels(register struct dcevnt_t *);
extern int32 __get_arrwide(struct net_t *);
extern void __ld_arr_val(register word32 *, register word32 *, union pck_u,
 int32, int32, int32);
extern void __ld_wire_val(register word32 *, register word32 *, struct net_t *);
extern void __ld_bit(register word32 *, register word32 *, register struct net_t *,
 int32);
extern void __ld_psel(register word32 *, register word32 *,
 register struct net_t *, int32, int32);
extern void __getarr_range(struct net_t *, int32 *, int32 *, int32 *);
extern void __dmp1_nplstel(struct mod_t *, struct net_t *, struct net_pin_t *);
extern void __dmp1_dcelstel(struct mod_t *, struct dcevnt_t *);
extern int32 __get_dcewid(struct dcevnt_t *, struct net_t *);
extern void __ld_perinst_val(register word32 *, register word32 *, union pck_u,
 int32);
extern int32 __vval_is1(register word32 *, int32);
extern void __sizchgxs(struct xstk_t *, int32);  
extern int32 __fr_cap_size(int32);
extern int32 __get_netwide(struct net_t *);
extern void __dmp_proc_assgn(FILE *, struct st_t *, struct delctrl_t *, int32);
extern void __dmp_stmt(FILE *, struct st_t *, int32);

extern void __dmp_nbproc_assgn(FILE *, struct st_t *, struct delctrl_t *);
extern void __dmp_forhdr(FILE *, struct for_t *);
extern void __dmp_dctrl(FILE *, struct delctrl_t *);
extern void __dmp_tskcall(FILE *, struct st_t *);
extern void __map_16v_to_12vform(word64 *, word64 *);
extern void __try_reduce_16vtab(word64 *, int32 *);
extern void __dmp_dcxpr(FILE *, union del_u, word32);
extern int32 __isleaf(struct expr_t *);
extern void __getwir_range(struct net_t *, int32 *, int32 *);
extern int32 __unnormalize_ndx(struct net_t *, int32);
extern int32 __get_giarr_wide(struct giarr_t *);
extern char *__get_vkeynam(char *, int32);
extern void __push_wrkitstk(struct mod_t *, int32);
extern void __pop_wrkitstk(void);
extern int32 __is_lnegative(word32 *, int32); 
extern word32 __cp_lnegate(word32 *, register word32 *, int32); 

extern void __cvsim_msg(char *, ...);
extern void __dbg_msg(char *, ...);
extern void __pv_err(int32, char *, ...);
extern void __pv_warn(int32, char *,...);
extern void __sgfwarn(int32, char *, ...);
extern void __sgfinform(int32, char *, ...);
extern void __sgferr(int32, char *, ...);
extern void __gfterr(int32, word32, int32, char *, ...);
extern void __pv_terr(int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);

extern word32 __masktab[];
extern int32 errno;
extern double __dbl_toticks_tab[];

/* LOOKATME - on mach ten and sunos fmod is drem - but maybe not same */
/* think ok since only used for know positive and arg1>arg2 case */
#if defined(__SVR4) || defined(__hpux)
#define drem fmod
#endif

/*
 * VERILOG EXACT DISPLAY ROUTINES - NO LINE HERE
 */

/*
 * write $fdisplay output to all files selected by multi-channel desc. 
 * or 
 *
 * important to call write only once per file here
 * for now this shares line wrap line output code so must save and restore
 *
 * axp must be function call comma operator even if only 1 argument
 */
extern void __fio_do_disp(register struct expr_t *axp, int32 dflt_fmt, int32 nd_nl,
 char *namstsk) 
{
 register int32 i;
 word32 mcd; 
 struct xstk_t *xsp;
 char s1[RECLEN];

 xsp = __eval_xpr(axp->lu.x);
 if (xsp->bp[0] != 0L) 
  {
   __sgfwarn(611, 
    "%s file or multi-channel descriptor %s not 32 bit non x/z value - no action",
    namstsk, __regab_tostr(s1, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
   __pop_xstk();
   return;
  }
 mcd = xsp->ap[0]; 
 __pop_xstk();

 axp = axp->ru.x; 

 __cur_sofs = 0;
 disp_toexprline(axp, dflt_fmt);

 /* SJM 09/09/03 - fd case easy because only one stream to write to */
 if ((mcd & FIO_MSB) == FIO_MSB)
  {
   int32 fd;

   fd = (int32) (mcd & ~FIO_MSB);
   /* if fd does not correspond to open file, just set error indicator */
   /* AIV 06/27/05 - fd cannot be greater than max file size */
   if (fd >= MY_FOPEN_MAX || __fio_fdtab[fd] == NULL) 
    { 
     errno = EBADF; 
     __cur_sofs = 0; 
     return; 
    }
   fputs(__exprline, __fio_fdtab[fd]->fd_s);
   if (nd_nl) fputc('\n', __fio_fdtab[fd]->fd_s);
   __cur_sofs = 0;
   return;
  }

 /* SJM 03/26/00 - mcd 1 now both stdout and std log and vendor 1 must */
 /* go through call back so can be intercepted */
 /* bit 0 - mcd 1 is both stdout and stdlog */
 if ((mcd & 1) != 0)
  {
   __cvsim_msg("%s", __exprline);
   if (nd_nl) __cvsim_msg("\n");
  }

 for (i = 1; i < 31; i++) 
  {
   if (((mcd >> i) & 1L) != 0L)
    { 
     if (__mulchan_tab[i].mc_s == NULL)
      {
       __sgfwarn(583,
        "%s multi-channel descriptor bit %d on, but file not open",
        namstsk, i);  
      }
     else
      {
       /* FIXME - how are these handled? */
       fputs(__exprline, __mulchan_tab[i].mc_s);
       if (nd_nl) fputc('\n', __mulchan_tab[i].mc_s);
      }
    }
  }
 if (((mcd >> 31) & 1) != 0)
  {
   __sgfwarn(583,
    "%s multi-channel descriptor bit 31 on but file not open - unusable because reserved for new Verilog 2000 file I/O",
     namstsk);  
  }
 __cur_sofs = 0;
}

/*
 * $swrite[hdob] version of formatted write into strings
 * this writes into __exprline - caller handles assign verilog reg
 *
 * notice this is write not display so no new line at end
 */
extern void __str_do_disp(struct expr_t *axp, int32 dflt_fmt)
{
 __cur_sofs = 0;
 disp_toexprline(axp, dflt_fmt);
}

/*
 * write $display output to file f (must be stream)  
 *
 * for now this shares line wrap line output code so must save and restore
 * axp must be function call comma operator even if only 1 argument
 */
extern void __do_disp(register struct expr_t *axp, int32 dflt_fmt)
{
 __cur_sofs = 0;
 disp_toexprline(axp, dflt_fmt);
 __cvsim_msg("%s", __exprline);
 __cur_sofs = 0;
}

/*
 * routine to implement $display type displaying into a expr line string
 * macros will grow string if needed
 * starts at place called set __cur_sofs to
 *
 * notice this run time routine and any called routines should
 * not emit messages - exception is round to time if has bits 52-64 on
 */
static void disp_toexprline(register struct expr_t *axp, int32 dflt_fmt)
{
 register char *chp;
 int32 base, chlen;
 char *start_fp;
 struct expr_t *xp;
 struct xstk_t *xsp;

 for (;axp != NULL;)
  {
   xp = axp->lu.x;
   /* empty parameter adds 1 space */
   if (xp->optyp == OPEMPTY) { addch_(' '); axp = axp->ru.x; continue; }

   /* literal format string (i.e 1st byte is 1st char of string not \0 */
   /* cannot be defparam since rhs literal string removed */
   if (xp->is_string)
    {
     /* since just encoding literal, know no leading 0's */
     chp = __vval_to_vstr(&(__contab[xp->ru.xvi]), xp->szu.xclen, &chlen);

     /* notice since must be literal source text string escapes removed */
     /* also this string can contain embedded 0's that are printed */
     /* know literal string has no bval */
     start_fp = chp;

     /* assuming 8 bit bytes */
     axp = __disp_1fmt_to_exprline(chp, axp);
     if (axp != NULL) xp = axp->lu.x; else xp = NULL;

     /* already pointing one past last format string used */
     __my_free(start_fp, chlen);
     continue;
    }
   /* handle argument that is just printed using default format */
   /* notice if literal string will not get here and if string var */
   /* here printed as number - need format */
   /* here default is base, but display real as real */
   xsp = __eval_xpr(xp);
   base = (xp->is_real) ? BDBLE : dflt_fmt;
   /* always trim here */
   __regab_disp(xsp->ap, xsp->bp, xsp->xslen, base, FALSE,
    (xp->has_sign == 1));
   __pop_xstk();
   axp = axp->ru.x;
  }
 __exprline[__cur_sofs] = '\0';
}

/*
 * print into string according to specifications in one format string
 *
 * this is called multiple times $swrite and exactly onece for $sformat
 * chp points to value converted to string
 */
extern struct expr_t *__disp_1fmt_to_exprline(char *chp, struct expr_t *axp)
{
 int32 trim, fmt_pos, fmt_non_real, blen;
 word32 *ap, *bp;
 double d1;
 char *new_chp;
 struct expr_t *xp;
 struct xstk_t *xsp;
 struct task_t *tskp;
 struct mod_t *mdp;
 char rfmtstr[RECLEN], s1[RECLEN], s2[RECLEN];

 /* point to 1st format value argument */
 axp = axp->ru.x;
 if (axp == NULL) xp = NULL; else xp = axp->lu.x;

 fmt_pos = 0;
 for (; *chp != '\0'; chp++)
  {
   /* notice is escaped by user char in format gets echoed as is */
   if (*chp != '%') { addch_(*chp); continue; }
   chp++;
   fmt_pos++; 
   /* if non format character just continue */
   /* needed since xp may be for %[non format] form */
   trim = FALSE;
try_fmt_again:
   fmt_non_real = TRUE;
   switch (*chp) {
    case '%':
     /* interesting but since output always goes through printf */
     /* this no longer goes through printf so only one needed */
     addch_('%');
     continue;
    case '0':
     chp++;
     if (trim) goto c_style_case;
     trim = TRUE;
     goto try_fmt_again;
    case 'm': case 'M':
     /* if no thread, know in interactive and have scope */
     if (__cur_thd == NULL) tskp = __scope_tskp; 
     /* %m defined as "scope" which may be task/func/block/inst */
     else tskp = __getcur_scope_tsk();
     __disp_itree_path(__inst_ptr, tskp);
     /* must not consume an argument */
     continue;
    case 'l': case 'L':
     /* SJM 05/17/04 - FIXME ??? ### need ptr from mod to cfg lib rec def in */
     mdp = __inst_ptr->itip->imsym->el.emdp;
     if (mdp->mod_cfglbp != NULL)
      {
       sprintf(s1, "%s.%s", mdp->mod_cfglbp->lbname, mdp->msym->synam); 
      }
     else
      {
       sprintf(s1, "[NO-CONFIG].%s", mdp->msym->synam); 
      } 
     __adds(s1);
     continue;

    case 'h': case 'H': case 'x': case 'X': case 'd': case 'D':
    case 'o': case 'O': case 'b': case 'B': case 'c': case 'C':
    case 's': case 'S': case 'v': case 'V':
    /* SJM 05/17/04 - binary formats added for P1364 2001 */
    case 'u': case 'U': case 'z': case 'Z':
     break;

    case 't': case 'T':
     fmt_non_real = FALSE;
     break;
    case 'e': case 'E': case 'f': case 'F': case 'g': case 'G':
     fmt_non_real = FALSE;
     /* F not in C standard and indentical to 'f' */
     if (*chp == 'F') *chp = 'f';
     /* trim ignored here and any c style %10.3g (z.b.) example */    
     strcpy(rfmtstr, "");
     break;
    /* for %f/%g only need to parse C style format */
    /* notice for 0. enter normally, for c legal 00 get here from '0' */
    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9': case '.': case '-':
    case '+': case '#': case ' ':
c_style_case:
     /* one of these after percent probably real format */
     /* if new_chp not 0 points to end format char on return */
     if ((new_chp = sep_real_fmt(rfmtstr, chp, trim)) == NULL)
      {
       addch_('%');
       if (trim) { addch_('0'); chp--; } else addch_(*chp);
       continue;
      }
     /* got real format */
     chp = new_chp;
     fmt_non_real = FALSE;
     break;
    default:
     /* copy %[non format char] case */
     addch_('%'); 
     addch_(*chp);
     /* notice for loop has chp++ inc */
     continue;
   }

  /* format needs expr. - check it */
  /* if nil, treat as ,, */
  if (xp == NULL)
   {
    addch_(' ');
    continue;
   }

  if (xp->optyp == OPEMPTY) { addch_(' '); goto nxt_arg; }
  /* ok for literal string to be printed as value */ 
  /* need special evaluation for with strength formats */ 
  if (*chp == 'v' || *chp == 'V') { sdisp_st(xp); goto nxt_arg; }

  /* eval. here uses type of expression to get real or non real val */
  xsp = __eval_xpr(xp);
  if (xp->is_real && fmt_non_real) __cnv_stk_fromreal_toreg32(xsp);
  ap = xsp->ap;
  bp = xsp->bp;
  switch (*chp) {
   case 'h': case 'H': case 'x': case'X':
    /* for variable accesses correct one */
    sdisph(ap, bp, xsp->xslen, trim);
    break;
   case 'd': case 'D':
do_dec:
    sdispd(ap, bp, xp->szu.xclen, trim, (xp->has_sign == 1));
    break;
   case 't': case 'T':
    {
     char tfmtstr[IDLEN];
          
     if (!bld_tfmt_val(tfmtstr, xp, ap, bp, xsp->xslen, trim, fmt_pos))
      goto do_dec;
     __adds(tfmtstr);
    }
    break;
   case 'o': case 'O':
    sdispo(ap, bp, xsp->xslen, trim);
    break;
   case 'b': case 'B':
    __sdispb(ap, bp, xsp->xslen, trim);
    break;
   case 's': case 'S':
    /* %s ignores b part and if non literal str leading 0's trimmed */
    /* do not need %0s to cause leading (high) 0's to not print */
    blen = __trim1_0val(ap, xsp->xslen);
    __xline_vval_to_cstr(ap, blen, FALSE, FALSE, FALSE);
    break;
   case 'c': case 'C':
    /* OVIsim only prints if printable */
    /* SJM 09/19/03 - must also print new line and tab - isprint does */
    /* not include new line and tab etc. */
    if (isprint(ap[0] & 0xff) || (ap[0] & 0xff) == '\n'
     || (ap[0] & 0xff) == '\r' || (ap[0] & 0xff) == '\t')
     addch_((char) (ap[0] & 0xff));
    else addch_(' ');
    break;
   case 'u': case 'U':
    disp_ufmt_binval(ap, bp, xsp->xslen);
    break;
   case 'z': case 'Z':
    disp_zfmt_binval(ap, bp, xsp->xslen);
    break;

   /* this can only be bit select or scalar */
   /* notice this reevaluates expression since needs to access st too */
   case 'e': case 'E': case 'f': case 'F': case 'g': case 'G':
    /* tricky case of real that needs c printf - fmt str in rfmtstr */
    if (!xp->is_real) 
     {
      /* this may produce warning if does not fit */
      __cnv_stk_fromreg_toreal(xsp, (xp->has_sign == 1));
      ap = xsp->ap;
      bp = xsp->bp;
     }
    /* first build the format string */
    if (*chp == 'F') *chp = 'f'; 
    sprintf(s1, "%%%s%c", rfmtstr, *chp);
    memcpy(&d1, ap, sizeof(double));

    /* know max size of real cannot be more than 30 or so */
    sprintf(s2, s1, d1);
    __adds(s2);
    /* LOOKATME - fall thru under what conditions? */
   }
   __pop_xstk();
nxt_arg:
   /* know 1 format arguemnt used up, so must move to next */
   axp = axp->ru.x;
   if (axp == NULL) xp = NULL; else xp = axp->lu.x;
  }
 return(axp);
}

/*
 * get the task from the current execing scope 
 * return NULL if none
 */
extern struct task_t *__getcur_scope_tsk(void)
{
 struct task_t *tskp;

 if (__fcspi >= 0) tskp = __fcstk[__fcspi];
 else if (__cur_thd->th_fj) tskp = __find_thrdtsk(__cur_thd);
 else tskp = __cur_thd->assoc_tsk;
 return(tskp);
}

/*
 * routine to check a display format system task (called from v_fx) 
 *
 * this is version of display routine above with all output and evaluation
 * turned off 
 */
extern void __chk_fmt(register struct expr_t *axp, byte *argnonvtab) 
{
 register char *chp;
 int32 trim, fmt_pos, fmt_non_real, chlen, argi;
 char *new_chp, *start_fp;
 struct expr_t *xp;
 char rfmtstr[RECLEN], s1[RECLEN];

 for (fmt_pos = 0, argi = 0; axp != NULL;)
  {
   xp = axp->lu.x;
   /* empty parameter, skip */
   if (xp->optyp == OPEMPTY) { axp = axp->ru.x; argi++; continue; }

   /* literal format string (i.e 1st byte is 1st char of string no \0 */
   /* cannot be defparam */
   if (xp->is_string)
    {
     /* since just en-ecoding literal, know no leading 0's */
     chp = __vval_to_vstr(&(__contab[xp->ru.xvi]), xp->szu.xclen, &chlen);
     start_fp = chp;
     /* know literal string has no bval */
     /* point to 1st format value argument */
     axp = axp->ru.x;
     if (axp == NULL) xp = NULL; else { xp = axp->lu.x; argi++; }

     /* notice since must be liternal source text string escapes removed */
     /* also this string can contain embedded 0's that are printed */
     for (; *chp != '\0'; chp++)
      {
       if (*chp != '%')
        {
         if (isprint(*chp) || *chp == '\n' || *chp == '\r' || *chp == '\t'
          || *chp == '\f') continue;
         __sgfwarn(561,
          "format contains non printable character \\%o (%d) (next pos. %d)",
          *chp, *chp, fmt_pos + 1);
        }
       chp++;
       fmt_pos++; 
       /* if non format character just continue */
       /* needed since xp can be %[non format] form */
       trim = FALSE;
try_fmt_again:
       fmt_non_real = TRUE;
       switch (*chp) {
        case '%': continue;
        case '0':
         chp++;
         if (trim) goto c_style_case;
         trim = TRUE;
         goto try_fmt_again;
        case 'm': case 'M': continue;
        case 'l': case 'L': continue;

        case 'v': case 'V':
         if (argnonvtab != NULL) argnonvtab[argi] = 1;
         break;
        case 'h': case 'H': case 'x': case 'X': case 'd': case 'D':
        case 'o': case 'O': case 'b': case 'B': case 'c': case 'C':
        case 's': case 'S': case 'u': case 'U': case 'z': case 'Z':
         break;
        case 't': case 'T':
         fmt_non_real = FALSE;
         break;
        case 'e': case 'E': case 'f': case 'F': case 'g': case 'G':
         fmt_non_real = FALSE;
         /* F not in C standard and indentical to 'f' */
         if (*chp == 'F') *chp = 'f';
         /* trim ignored here and any c style %10.3g (z.b.) example */    
         break;
        /* for %f/%g only need to parse C style format */
        /* notice for 0. enter normally, for c legal 00 get here from '0' */
        case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': case '.': case '-':
        case '+': case '#': case ' ':
c_style_case:
         /* one of these after percent probably real format */
         /* if new_chp not 0 points to end format char on return */
         if ((new_chp = sep_real_fmt(rfmtstr, chp, trim)) == NULL)
          { if (trim) chp--; continue; }
         /* got real format */
         chp = new_chp;
         fmt_non_real = FALSE;
         break;
        default: continue;
         /* notice for loop has chp++ inc */
       }

       /* format needs expr. - check it */
       if (xp == NULL)
        {
         __sgferr(719, "argument list exhausted for %%%c format (pos. %d).",
          *chp, fmt_pos);
         continue;
        }
       if (xp->optyp == OPEMPTY)
        {
         __sgfwarn(549,
          "argument list ,, value - format %%%c (pos. %d) - value is space",
          *chp, fmt_pos);
         goto nxt_arg;
        }
       if (xp->is_string && *chp != 's' && *chp != 'S')
        {
         __sgfwarn(554,
          "string constant probably incompatible with %%%c format (pos. %d)",
          *chp, fmt_pos);
         goto nxt_arg;
        }
       /* strengths can be any 1 bit in OVIsim but anything in Cver */

       /* eval. here uses type of expression to get real or non real val */
       if (xp->is_real && fmt_non_real)
        {
         __sgfwarn(552,
          "%c format but value expression (pos. %d) type real - converted to 32 bit reg",
          *chp, fmt_pos);
        }
       /* only look at things that need checking */
       switch (*chp) {
        case 't': case 'T':
         if (xp->szu.xclen > TIMEBITS)
          {
           if (fmt_pos == -1) strcpy(s1, "");
           else sprintf(s1, " (pos. %d)", fmt_pos);
           __sgfwarn(520,
            "%%t or $timeformat%s expression wider than %d bits - high ignored",
            s1, TIMEBITS);
          }
         break;

        /* this can only be bit select or scalar */
        /* notice this reevaluates expression since needs to access st too */
        case 'e': case 'E': case 'f': case 'F': case 'g': case 'G':
        /* tricky case of real that needs c printf - fmt str in rfmtstr */
        if (!xp->is_real)
         {
          __sgfinform(480,
            "non real value output with %c format (pos. %d) - conversion needed",
           *chp, fmt_pos);
         }
        }
nxt_arg:
       /* know 1 format arguemnt used up, so must move to next */
       axp = axp->ru.x;
       if (axp == NULL) xp = NULL; else { argi++; xp = axp->lu.x; }
      }
     /* already pointing one past last format string used */
     __my_free(start_fp, chlen);
     continue;
    }
   axp = axp->ru.x;
   argi++;
  }
}

/*
 * search currently executing task and up to top of nested created
 * subthreads for a thread that is a task
 *
 * must search up since fork-join threads do not have tasks but
 * thread that fork-join is subthread of may
 * this is needed by %m
 */
extern struct task_t *__find_thrdtsk(struct thread_t *cur_thp)
{
 struct thread_t *thdp;

 for (thdp = cur_thp; thdp != NULL; thdp = thdp->thpar)
  {
   if (thdp->assoc_tsk != NULL) return(thdp->assoc_tsk);
  } 
 return(NULL);
}

/*
 * version os message build itree when know there is no task
 */
extern char *__msg2_blditree(char *s, struct itree_t *itp)
{
 return(__msg_blditree(s, itp, (struct task_t *) NULL));
}

/*
 * version of instance name that builds work string
 * does start at cur spos over-write anything ? 
 */
extern char *__msg_blditree(char *s, struct itree_t *itp, struct task_t *tskp)
{
 int32 sav_sofs = __cur_sofs;

 __disp_itree_path(itp, tskp);
 __trunc_cstr(&(__exprline[sav_sofs]), MSGTRUNCLEN, FALSE);
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = '\0'; 
 return(s);
}

/*
 * add itree path to __exprline - gets path from value of cur. itp
 * use glbsycmp work array as work stack
 * this just writes scope part - if need object name caller must write
 * allow any length path here
 */
extern void __disp_itree_path(register struct itree_t *itp, struct task_t *tskp)
{
 register int32 gi;
 struct symtab_t *sytp;
 char *chp;

 /* if in task (probably nested block), build from bottom */
 if (tskp != NULL)
  {
   for (sytp = tskp->tsksymtab, gi = 0;;)
    {
     /* notice do not need glb x cmps here since symbol is right inst */
     __glbsycmps[gi] = sytp->sypofsyt;
     sytp = sytp->sytpar;
     if (sytp == NULL || sytp->sypofsyt->sytyp == SYM_M) break;
     if (++gi >= MAXGLBCOMPS)
      __pv_terr(310,
       "cannot print instance path name with too many components (%d)",
       MAXGLBCOMPS);
    }
   gi++;
  }
 else gi = 0;

 /* fill from front to end - know at least one component */
 for (;;)
  {
   __glbsycmps[gi] = itp->itip->isym;
   /* virtual tops modules have no up entry */
   itp = itp->up_it;
   if (itp == NULL) break;
   if (++gi >= MAXGLBCOMPS)
    __pv_terr(310,
     "cannot print instance path name with too many components (%d)",
     MAXGLBCOMPS);
  }
 /* then fill top end to front - know string nil terminated by last __adds */
 for (; gi >= 0; gi--)
  { chp = __glbsycmps[gi]->synam; __adds(chp); if (gi > 0) addch_('.'); }
}

/*
 * check for legal c printf style real format string
 * this is format preprocessing routine
 * think ascii standard says '-' can go anywhere
 */
static char *sep_real_fmt(char *fmtstr, char *chp, int32 trim)
{
 int32 i;
 char *fchp;

 /* first see if can find real format end char within reasonable distance */
 /* this eliminates literal % followed by real fonmat pun case */
 for (fchp = chp, i = 1; ; fchp++, i++)
  {
   switch (*fchp) {
    case 'e': case 'E': case 'f': case 'F': case 'g': case 'G':
     /* -0dd.ddf is max but allowing 2 extra 0s */
     if (i > 10) return(NULL);
     goto got_fmt;
    case '\0': return(NULL);
   }
  }
 /* know format end fence in string to get here */
got_fmt:
 fchp = fmtstr;
 for (;;)
  {
   switch (*chp) {
    case '-': case '+': case ' ': case '#': break;
    /* 0 from trim followed by prefix 0 illegal */
    case '0': if (trim) return(NULL); break;
    default: goto col_digs;
   }
   *fchp++ = *chp++;
  }

col_digs:
 /* collect possible digit string */
 for (;;) { if (!isdigit(*chp)) break; *fchp++ = *chp++; }
 if (*chp == '.')
  {
   *fchp++ = *chp++;
   for (;;) { if (!isdigit(*chp)) break; *fchp++ = *chp++; }
  }
 switch (*chp) {
  case 'e': case 'E': case 'g': case 'G': case 'f': case 'F': break;
  default: return(NULL);
 }
 *fchp = '\0';
 /* must return pointer to format char */
 return(chp);
}

/*
 * convert a Verilog value to a printable hex string
 * if trim false must pad with leading spaces
 */
static void sdisph(word32 *ap, word32 *bp, int32 blen, int32 trim)
{
 register int32 bi, wi;
 int32 swlen, bi2, trimmed_blen, highused;
 word32 tmpa, tmpb;
 char ch;

 /* short circuit common 1 bit case */
 if (blen == 1)
  {
   tmpa = ap[0] | (bp[0] << 1);
   if (tmpa < 2) ch = '0' + ((char) tmpa);
   else if (tmpa == 2) ch = 'z';
   else ch = 'x';
   addch_(ch);
   goto done;
  }
 if (trim)
  {
   trimmed_blen = trim_abval(ap, bp, blen);
   if (trimmed_blen == 0) { addch_('0'); goto done; }
   if (vval_isall_xs(ap, bp, blen)) { addch_('x'); goto done; }
   if (__vval_isallzs(ap, bp, blen)) { addch_('z'); goto done; }
  }
 else trimmed_blen = blen;

 /* think high bits non zero (but same as new high bit will select right) */
 /* may need to adjust so have 1 entire hex digit */
 swlen = wlen_(trimmed_blen);
 bi2 = trimmed_blen & 0x1fL;
 if (bi2 == 0) bi = WBITS;
 else bi = WRDBYTES*((bi2 + WRDBYTES - 1)/WRDBYTES);
 bi -= 4;
 if ((highused = (bi2 % 4)) == 0) highused = 4;
 for (wi = swlen - 1; wi >= 0; wi--)
  {
   tmpa = ap[wi]; tmpb = bp[wi];
   for (;bi > 0; bi -= 4)
    {
     tmpa = (ap[wi] >> bi) & 0xfL;
     tmpb = (bp[wi] >> bi) & 0xfL;
     addch_(bitsto_char(tmpa, tmpb, highused));
     highused = 4;
    }
   /* notice bi will always == 0 here */
   addch_(bitsto_char((ap[wi] & 0xfL), (bp[wi] & 0xfL), highused));
   bi = WBITS - 4;
  }
 /* need explicit terminator */
done:
 __exprline[__cur_sofs] = '\0';
}

/*
 * convert hex/oct/bin bit pattern to ascii character
 * all bits z = lower case else upper case (same for x)
 */
static char bitsto_char(word32 a, word32 b, int32 bwid)
{
 char ch;
 word32 mask;

 if (!b) { ch = (char) valtoch_(a); return(ch); }
 mask = __masktab[bwid];
 if ((b & mask) == mask)
  {
   /* know all control bits on */
   if (a == 0L) return('z');
   else if ((a & mask) == mask) return('x');
   else return('X');
  }
 /* some control bits on - if no x's Z else X */
 if ((a & b) == 0L) return('Z');
 return('X');
}

/*
 * convert a Verilog value to a printable decimal string using c printf
 * starts fill __exprline at sofs and update sofs
 * dsigned false for signed value - like %u in c
 * if caller added [size]'d will never be signed
 */
static void sdispd(word32 *ap, word32 *bp, int32 blen, int32 trim, int32 dsigned)
{
 register int32 i;
 int32 ochnum, trimblen, widnumlen;
 sword32 sval;
 struct xstk_t *xsp;
 char ch, *chp, *chp2, s1[RECLEN];

 if (!trim) 
  {
   /* need ceil here */
   ochnum = (int32) (blen*LG2_DIV_LG10 + 0.999999);
   if (dsigned) ochnum++;
  }
 else ochnum = 0;

 /* handle various x/z forms */
 if ((ch = get_decxz(ap, bp, blen)) != ' ')
  {
   /* ? PORTABILITY sprintf(s1, "%*c", ochnum, ch); */
   for (i = 0; i < ochnum - 1; i++) addch_(' ');
   addch_(ch);
   __exprline[__cur_sofs] = '\0';
   return;
  }
 if (blen <= WBITS)
  {
   /* for decimal leading untrimmed must be spaces */
   if (dsigned)
    {
     if (blen == WBITS) sprintf(s1, "%ld", (sword32) ap[0]);
     else
      {
       if ((ap[0] & (1 << (blen - 1))) != 0)
        {
         /* SJM 10/20/03 - 1 bit width vars can't be signed so works */ 
         if (blen == 1)
          {
           /* AIV 09/15/04 - LOOKATME - should this be -1? */
           strcpy(s1, "0");
          }
         else
          {
           sval = ~((int32) ap[0]);
           sval++;
           sval &= __masktab[blen - 1];
           /* AIV 09/15/04 - was wrongly printing - 0 for most negative */
           if (sval == 0) sprintf(s1, "-%ld", ap[0]);
           else sprintf(s1, "-%ld", sval);
          }
        }
       else sprintf(s1, "%lu", ap[0]);
      }
    }
   else sprintf(s1, "%lu", ap[0]);
   for (i = 0; i < ochnum - (int32) strlen(s1); i++) addch_(' ');
   __adds(s1);
   return;
  }
 /* SJM 05/27/04 - for negative can't trim until negated */
 if (dsigned && __is_lnegative(ap, blen))
  {
   /* compute number of chars required by converted number */
   /* need ceil here */
   /* SJM 10/20/03 - since wide can now be signed must use 2's complement */
   /* this works because know x/z case removed above */

   /* number is negative - compute bit wise not then add 1 */
   push_xstk_(xsp, blen);
   __cp_lnegate(xsp->ap, ap, blen);

   trimblen = __trim1_0val(xsp->ap, blen);
   widnumlen = trimblen*LG2_DIV_LG10 + 0.999999;
   chp = (char *) __my_malloc(widnumlen + 2);
   __declcnv_tostr(chp, xsp->ap, trimblen, widnumlen);
   widnumlen++;
   for (i = 0; i < ochnum - widnumlen; i++) addch_(' ');
   addch_('-');

   /* SJM 05/28/04 - widnumlen estimate can be 1 too large must trim front */
   /* can't just inc chp since must free beginning of area */
   chp2 = chp;
   while (*chp2 == ' ') chp2++;
   __adds(chp2);

   __my_free(chp, widnumlen + 2);
   __pop_xstk(); 
   return;
  }
 /* case 2: either not signed or positive */
 trimblen = __trim1_0val(ap, blen);
 /* handle wide 0 as special case - these need trim on to happen */
 if (trimblen == 0)
  {
   for (i = 0; i < ochnum - 1; i++) addch_(' ');
   addch_('0');
   __exprline[__cur_sofs] = '\0';
   return;
  }
 /* next wide that trims to 1 word32 */
 if (trimblen <= WBITS)
  {
   sprintf(s1, "%lu", ap[0]);
   for (i = 0; i < ochnum - (int32) strlen(s1); i++) addch_(' ');
   __adds(s1);
   return;
  }
 widnumlen = trimblen*LG2_DIV_LG10 + 0.999999;
 chp = (char *) __my_malloc(widnumlen + 2);
 __declcnv_tostr(chp, ap, trimblen, widnumlen);
 for (i = 0; i < ochnum - widnumlen; i++) addch_(' ');
 __adds(chp);
 __my_free(chp, widnumlen + 2);
}

/*
 * convert an array of words (word32 contiguous Verilog multiword value)
 * into a string s known to be wide enough
 * know blen > WBITS for will not be called
 * this does not use exprline or a part
 */
extern void __declcnv_tostr(char *s, word32 *wp, int32 trimblen, int32 widnumlen)
{
 register int32 chi;
 word32 *quot, *u, r0;
 int32 wlen;
 struct xstk_t *xsp;

 wlen = wlen_(trimblen);
 push_xstk_(xsp, wlen*WBITS/2);
 quot = xsp->ap;
 memset(quot, 0, wlen*WRDBYTES);
 push_xstk_(xsp, wlen*WBITS/2);
 u = xsp->ap;
 cp_walign_(u, wp, trimblen);
 s[widnumlen] = '\0'; 
 /* repeatedly divide by 10 filling from end to front */
 for (chi = widnumlen - 1;;)
  {
   __by16_ldivmod(quot, &r0, u, (word32) 10, trimblen);
   s[chi--] = (char) (r0 + '0'); 
   if (vval_is0_(quot, trimblen)) break;
   if (chi < 0) __case_terr(__FILE__, __LINE__);
   /* know value shorter so retrim */
   trimblen = __trim1_0val(quot, trimblen);
   cp_walign_(u, quot, trimblen);
   memset(quot, 0, wlen_(trimblen)*WRDBYTES); 
  }
 for (; chi >= 0; chi--) s[chi] = ' ';
 __pop_xstk();
 __pop_xstk();
}

/*
 * convert a 64 bit time to a decimal string
 * t must be ticks scaled to value for module if needed
 */
extern char *__to_timstr(char *s, word64 *t)
{
 /* UNUSED - int32 trimblen, widnumlen; */
 word64 t1;

 /* hard absolute unit case */
 if (__nd_timstr_suf && __timstr_mult != 1) t1 = (*t)*__timstr_mult;
 else t1 = *t;
 /* LOOKATME - is this portable? */
 /* LOOKATME - what is sparc and hp format letters for these? */ 
 sprintf(s, "%llu", t1); 

 /* LOOKATME - can this be eliminated ---
 if ((t1 >> 32) == 0ULL) sprintf(s, "%lu", (word32) (t1 & WORDMASK_ULL));
 else
  {
   word32 t1a[2];

   -* low at 0th address and high at next *-
   t1a[0] = (word32) (t1 & WORDMASK_ULL);
   t1a[1] = (word32) ((t1 >> 32) & WORDMASK_ULL);
   -* notice this case makes use of c require that fields are in order *-
   trimblen = __trim1_0val(t1a, TIMEBITS);
   -* need ceil here *-
   widnumlen = (int32) (trimblen*LG2_DIV_LG10 + 0.999999);
   __declcnv_tostr(s, t1a, trimblen, widnumlen);
  }
 --- */
 if (__nd_timstr_suf) strcat(s, __timstr_unitsuf);
 return(s);
}

/*
 * return decimal x/z/X/Z char ' ' for non x/z
 * could write routine that does not call routines but eliminates
 */
static char get_decxz(word32 *ap, word32 *bp, int32 blen)
{
 if (vval_is0_(bp, blen)) return(' ');
 if (vval_isall_xs(ap, bp, blen)) return('x');
 if (__vval_isallzs(ap, bp, blen)) return('z');
 /* finally find if any z's and any x's */
 if (vval_hasx(ap, bp, blen)) return('X');
 return('Z');
}

/*
 * build the time format (real) from $timeformat string
 * passed expr. is in scaled (mult. by min. units power of 10)
 * computation here use double so only 53 (52 ieee) bits of accuracy
 * (ieee) may cause loss of values
 *
 * notice value passed here is scaled to units of current module 
 * i.e. must come from $time (or $stime or $realtime (already double))
 *
 * could do some case in exact 64 bit unsigned?
 * this string never wider than RECLEN and does not use expr line
 */
static int32 bld_tfmt_val(char *s, struct expr_t *xp, word32 *ap, word32 *bp,
 int32 blen, int32 trim, int32 fmt_pos)
{
 int32 unit;
 word64 usertim;
 double d1;
 struct mod_t *mdp;
 char s1[RECLEN];

 if (fmt_pos == -1) strcpy(s1, ""); else sprintf(s1, " (pos. %d)", fmt_pos);
 /* fill time from expr. - value is already scaled - may unscale */
 if (xp->is_real) memcpy(&d1, ap, sizeof(double)); 
 else 
  { 
   if (blen <= WBITS)
    { if (bp[0] != 0L) return(FALSE); d1 = (double) ap[0]; }
   else
    {
     /* this causes decimal style x with no suffix string to be emitted */
     if (bp[1] != 0L || bp[0] != 0L) return(FALSE);
     /* SJM 02/03/00 - works because ap[0] word32 */
     usertim = ((word64) ap[0]) | (((word64) ap[1]) << 32);
     if (!__v64_to_real(&d1, &usertim))
      __sgfwarn(572,
       "%%t or $timeformat%s conversion to real lost precision",
      s1); 
    }
  }  
 /* know d1 is this module's time unit time */
 /* if time format units same as modules d1 has ticks */
 mdp = __inst_mod;
 if (mdp->mtime_units != __tfmt_units)
  {
   if (mdp->mtime_units > __tfmt_units)
    {
     /* here d1 module ticks higher exponent (more precision) - divide */  
     unit = mdp->mtime_units - __tfmt_units;
     d1 /= __dbl_toticks_tab[unit];
    }   
   else
    {
     /* here d1 module ticks lower (less precision) - multiply */
     unit = __tfmt_units - mdp->mtime_units;
     d1 *= __dbl_toticks_tab[unit];
    }
  }
 /* unscaled time (can have frac. part) in d1 */
 /* uses the sprintf rounding */ 
 /* this can never fail */
 if (!trim)
  {
   sprintf(s, "%*.*f%s", (int32) (__tfmt_minfwid - strlen(__tfmt_suf)),
    (int32) __tfmt_precunits, d1, __tfmt_suf);
  }
 else sprintf(s, "%.*f%s", (int32) __tfmt_precunits, d1, __tfmt_suf);
 return(TRUE);
}

/*
 * for tf_ strgetp routines, build value of expr. in number
 *
 * return string (nil on error)
 * this is in convert because calls display formatting routines 
 * added %v and %g/%f that are not part of standard
 * conversions to/from real are 32 bit following LRM $display
 *
 * tf_ routines malloc memory that user frees when done
 * know correct itree loc. set here
 */
extern char *__alloc_getasfmt(struct expr_t *xp, struct tfrec_t *tfrp,
 int32 fmtchar)
{
 int32 sav_sofs, slen, vsigned, valreal;
 word32 *wp;
 double d1;
 struct xstk_t *xsp;
 char *chp, s1[RECLEN];

 sav_sofs = __cur_sofs;
 /* handle argument 0 only for function as special case */
 if (tfrp != NULL)
  {
   if (fmtchar == 'v' || fmtchar == 'V') return(NULL);
   push_xstk_(xsp, tfrp->fretsiz);
   wp = (word32 *) tfrp->tfargs[0].arg.awp;
   /* SJM 12/12/01 - this was wrong was copying over stk ptr */
   memcpy(xsp->ap, wp, 2*wlen_(tfrp->fretsiz)*WRDBYTES);

   if (tfrp->fretreal) valreal = TRUE; else valreal = FALSE;
   vsigned = FALSE;
   goto xl_disp;
  }

 /* return 32 bit int32 as pointer to string */
 /* if really literal string format letter ignored */ 
 if (xp->optyp == NUMBER && xp->is_string)
  {
   /* notice even though in cnv uses tf malloc routine */ 
   chp = __alloc_vval_to_cstr(&(__contab[xp->ru.xvi]), xp->szu.xclen,
    FALSE, FALSE);
   /* notice no xstk push to get here */
   return(chp);
  }

 /* now fill exprline (on end saving prefix) */
 /* for strength allow vector but here display routine evals expr */
 /* if xp is real this will use bit pattern not converted value */ 
 if (fmtchar == 'v' || fmtchar == 'V') { sdisp_st(xp); goto xl_done; }

 xsp = __eval_xpr(xp);
 if (xp->is_real) valreal = TRUE; else valreal = FALSE;
 if (xp->has_sign) vsigned = TRUE; else vsigned = FALSE;

xl_disp:
 d1 = 0.0;
 /* do any to/from real conversion if needed */
 switch ((byte) fmtchar) {
  case 'g': case 'G': case 'f': case 'F':
   if (!valreal) __cnv_stk_fromreg_toreal(xsp, vsigned);
   memcpy(&d1, xsp->ap, sizeof(double));
   break;
  default:
   if (valreal) __cnv_stk_fromreal_toreg32(xsp);
 }

 /* SJM 08/03/04 - this code was wrongly trimmming leading 0s */
 /* LRM  requires keeping leading 0s */
 switch ((byte) fmtchar) {
  case 'h': case 'H': case 'x': case'X':
   sdisph(xsp->ap, xsp->bp, xsp->xslen, FALSE);
   break;
  case 'd': case 'D':
   sdispd(xsp->ap, xsp->bp, xsp->xslen, FALSE, vsigned);
   break;
  case 'o': case 'O':
   sdispo(xsp->ap, xsp->bp, xsp->xslen, FALSE);
   break;
  case 'b': case 'B':
   __sdispb(xsp->ap, xsp->bp, xsp->xslen, FALSE);
   break;
  case 'g': case 'G': case 'f': case 'F':
   sprintf(s1, "%g", d1);
   break;
  default: __pop_xstk(); return(NULL);
 }
 __pop_xstk();

xl_done:
 slen = __cur_sofs - sav_sofs;
 chp = __mytf_malloc(slen + 1);
 strcpy(chp, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = 0;
 return(chp); 
}

/*
 * convert a Verilog value to a printable oct string
 */
static void sdispo(word32 *ap, word32 *bp, int32 blen, int32 trim)
{
 register int32 bi;
 word32 psaval, psbval;
 int32 digs, trimmed_blen, highused, tmp;
 char ch;

 /* short circuit common 1 bit case */
 if (blen == 1)
  {
   tmp = (int32) (ap[0] | (bp[0] << 1)); 
   if (tmp < 2) ch = '0' + ((char) tmp);
   else if (tmp == 2) ch = 'z';
   else ch = 'x';
   addch_(ch);
   goto done;
  }
 if (trim)
  {
   trimmed_blen = trim_abval(ap, bp, blen);
   if (trimmed_blen == 0) { addch_('0'); goto done; }
   if (vval_isall_xs(ap, bp, blen)) { addch_('x'); goto done; }
   if (__vval_isallzs(ap, bp, blen)) { addch_('z'); goto done; }
  }
 else trimmed_blen = blen;

 /* notice -1 correction start at right part of 3 bit field */
 digs = ((trimmed_blen + 2)/3) - 1;

 /* notice selects required here because bits overlap word32 boundaries */
 if ((highused = (trimmed_blen % 3)) == 0) highused = 3;
 for (bi = 3*digs; bi >= 0; bi -= 3)
  {
   /* need part select here since word32 overlap */
   __rhspsel(&psaval, ap, bi, highused);
   __rhspsel(&psbval, bp, bi, highused);
   addch_(bitsto_char(psaval, psbval, highused));
   highused = 3;
  }
done:
 __exprline[__cur_sofs] = '\0';
}

/*
 * convert a Verilog value to a printable binary string
 * if any prefix that must be preserved - sofs points to 1st usable pos.
 * notice this must be efficient since used for value change dump
 */
extern void __sdispb(register word32 *ap, register word32 *bp, int32 blen, int32 trim)
{
 register int32 wi, bi;
 register word32 tmpa, tmpb;
 int32 swlen, trimmed_blen;
 char ch;

 /* short circuit common 1 bit case */
 if (blen == 1)
  {
   tmpa = ap[0] | (bp[0] << 1);
   if (tmpa < 2) ch = '0' + ((char) tmpa);
   else if (tmpa == 2) ch = 'z';
   else ch = 'x';
   addch_(ch);
   __exprline[__cur_sofs] = '\0';  
   return;
  }

 if (trim)
  {
   trimmed_blen = bin_trim_abval(ap, bp, blen);
   if (trimmed_blen == 0) { addch_('0'); goto done; }
   if (vval_isall_xs(ap, bp, blen)) { addch_('x'); goto done; }
   else if (__vval_isallzs(ap, bp, blen)) { addch_('z'); goto done; }
  }
 else trimmed_blen = blen;

 swlen = wlen_(trimmed_blen);
 bi = trimmed_blen & 0x1f;
 if (bi == 0) bi = WBITS - 1; else bi--;
 for (wi = swlen - 1; wi >= 0; wi--)
  {
   tmpa = ap[wi]; tmpb = bp[wi];
   for (;bi > 0; bi--)
    {
     tmpa = (ap[wi] >> bi) & 1L;
     tmpb = (bp[wi] >> bi) & 1L;
     if (tmpb != 0L) { if (tmpa != 0L) ch = 'x'; else ch = 'z'; }
     else ch = '0' + ((char) tmpa);
     addch_(ch);
    }
   /* notice bi always exactly 0 here */
   tmpa = ap[wi] & 1L;
   tmpb = bp[wi] & 1L;
   if (tmpb != 0L) { if (tmpa != 0L) ch = 'x'; else ch = 'z'; }
   else ch = '0' + ((char) tmpa);
   addch_(ch);
   bi = WBITS - 1;
  }
 /* need \0 but next char. must over write it */
done:
 __exprline[__cur_sofs] = '\0';  
}

/*
 * convert a value as Verilog string to printable string
 * truncating version
 */
extern char *__strab_tostr(char *s, word32 *ap, int32 blen, int32 nd_quotes,
 int32 space_0)
{
 int32 sav_sofs = __cur_sofs;

 __xline_vval_to_cstr(ap, blen, nd_quotes, space_0, FALSE);
 /* here keeps low part if trunc needed */
 __trunc_cstr(&(__exprline[sav_sofs]), MSGTRUNCLEN, TRUE);
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[sav_sofs] = '\0';
 return(s);
}

/*
 * convert a value as Verilog string to printable string
 * non truncating version
 *
 * caller must insure big enough 
 */
extern char *__strab2_tostr(char *s, word32 *ap, int32 blen, int32 nd_quotes,
 int32 space_0)
{
 int32 sav_sofs = __cur_sofs;

 __xline_vval_to_cstr(ap, blen, nd_quotes, space_0, FALSE);
 /* here keeps low part if trunc needed */
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[sav_sofs] = '\0';
 return(s);
}

/*
 * construct a Verilog string into line starting at __cur_sofs
 *
 * for values that are being printed in some type of display
 * dmp_expr uses this to add string to accumulating line
 */
extern void __xline_vval_to_cstr(word32 *ap, int32 blen, int32 nd_quotes,
 int32 space_0, int32 esc_esc)
{
 int32 chlen;
 char *chp;

 /* convert to pascal (no ending 0) string  - malloc storage */
 chp = __vval_to_vstr(ap, blen, &chlen);
 /* converts to c string with non printable expansion into __exprline */
 vstr_to_cstr(chp, blen, space_0, nd_quotes, esc_esc);
 /* free the version in malloced storage */
 __my_free(chp, chlen);
}

/*
 * convert a verilog string to a cstring - substitutes chars and adds \0
 * and put result in __exprline starting at current place 
 */
static void vstr_to_cstr(char *vstr, int32 blen, int32 space_0, int32 nd_quotes,
 int32 esc_esc)
{
 register int32 i; 
 register char *chp;
 int32 chlen;
 char s1[RECLEN];

 /* this will never extend higher than high word32, could have inform if */
 /* not even 8 bits but no way to locate for user */ 
 chlen = (blen + 7)/8;
 /* DBG remove --- */
 if (vstr[chlen] != '\0') __arg_terr(__FILE__, __LINE__);
 /* --- */ 
 if (nd_quotes) addch_('"');
 for (chp = vstr, i = 0; i < chlen; i++, chp++)
  {
   switch (*chp) {
    case '"': case '\\': addch_('\\'); addch_(*chp); break;
    case '\n':
     /* for source output must escape c escape so string contains embedded */
     if (esc_esc) { addch_('\\'); addch_('n'); } else addch_(*chp);
     break;
    case '\t':
     if (esc_esc) { addch_('\\'); addch_('t'); } else addch_(*chp);
     break;
    default:
     if (!isprint(*chp))
      {
       if (space_0 && *chp == '\0') { addch_(' '); break; }
       sprintf(s1, "%03o", *chp);
       addch_('\\');
       __adds(s1);
       break;
      }
     addch_(*chp);
   }
  }
 if (nd_quotes) addch_('"');
 __exprline[__cur_sofs] = '\0';
}

/* ---
INVERSE:

#if (BYTE_ORDER == BIG_ENDIAN)
   wrd = (b1 & 0xff) | ((b2 & 0xff) << 8) | ((b3 & 0xff) << 16)
    | ((b4 & 0xff) << 24);
#else
   wrd = (b4 & 0xff) | ((b3 & 0xff) << 8) | ((b2 & 0xff) << 16)
    | ((b1 & 0xff) << 24);
--- */
/*
 * write 'u' format binary 0/1 one word32 using machine's endianness
 * into __exprline
 *
 * for u format output is a words only but if b bit set value is 0
 */
static void disp_ufmt_binval(word32 *ap, word32 *bp, int32 blen)
{
 register int32 j;
 register word32 wrd;
 byte bval;
 int32 wi;
  
 for (wi = 0; wi < wlen_(blen); wi++)
  {
   wrd = ap[wi] & (bp[wi] ^ 0xffffffff);

#if (BYTE_ORDER != BIG_ENDIAN)
   for (j = 24; j >= 0; j -= 8)
    {
     bval = (wrd >> j) & 0xff;
     addch_(bval);
    }
#else
   for (j = 0; j <= 24; j += 8)
    {
     bval = (wrd >> j) & 0xff;
     addch_(bval);
    }
#endif
  }
}

/*
 * write 'z' format binary 0/1 two words using machine's endianness
 * into __exprline
 *
 * for z format output is a and b words interleaved
 */
static void disp_zfmt_binval(word32 *ap, word32 *bp, int32 blen)
{
 register int32 j;
 int32 wi;
 byte bval;

 for (wi = 0; wi < wlen_(blen); wi++)
  {
#if (BYTE_ORDER != BIG_ENDIAN)
   for (j = 24; j >= 0; j -= 8)
    {
     bval = (ap[wi] >> j) & 0xff;
     addch_(bval);
     bval = (bp[wi] >> j) & 0xff;
     addch_(bval);
    }
#else
   for (j = 0; j <= 24; j += 8)
    {
     bval = (ap[wi] >> j) & 0xff;
     addch_(bval);
     bval = (bp[wi] >> j) & 0xff;
     addch_(bval);
    }
#endif
  }
}

/*
 * get a string and allocate with malloc
 *
 * not for disp routines where string in Verilog must be literal
 * this will always build some kind of string
 * this must be called with actual string or number object
 */
extern char *__get_eval_cstr(struct expr_t *xp, int32 *chlen)
{
 register int32 i;
 register char *chp; 
 int32 sav_sofs, slen;
 struct xstk_t *xsp;

 xsp = __eval_xpr(xp); 

 /* entire string could be \ddd form and 1 char per 8 bits */
 /* notice here every char can become \ddd where digits are octal */
 /* puts result in expr line */
 sav_sofs = __cur_sofs;
 __xline_vval_to_cstr(xsp->ap, xsp->xslen, FALSE, FALSE, FALSE);
 __pop_xstk();

 /* final step is trim all leading \000 chars */
 /* notice if any non \000, must stop */
 slen = __cur_sofs;
 for (chp = __exprline; *chp != '\0'; chp += 4)
  { if (strncmp(chp, "\\000", 4) != 0) break; }
 /* remove leading 0's if needed */
 if (chp != __exprline)
  {
   slen -= (chp - __exprline); 
   /* tricky in place copy - strcpy not portable - also copy ending 0 */
   for (i = 0; i <= slen; i++) __exprline[i] = chp[i];
  }
 slen = strlen(&(__exprline[sav_sofs]));
 chp = __my_malloc(slen + 1);
 strcpy(chp, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = '\0';
 *chlen = slen;
 return(chp);
}

/*
 * convert constant string stored as Verilog value to Verilog style string
 * know b val always 0 and still must convert to C style string for print
 * copy little endian chars in words, bit endian word32 reg to big endian
 *
 * always puts a \0 at end of string in case needed (usually not)
 * also not Verilog value in sense that high chars are not zeroed
 *
 * cannot use to replace value 
 * all this stuff is not 8 bit clean
 * loop probably better than the inlining here
 * notice assuming 8 bit byte
 * this routines is 32 bit word32 dependent
 */
extern char *__vval_to_vstr(word32 *ap, int32 blen, int32 *slen)
{
 register int32 si, wi;
 char *s;
 int32 wlen, nchs;

 /* make sure one extra character at end, if need to convert to */
 /* binary style c string for sreadmem */
 nchs = (blen + 7)/8;
 wlen = wlen_(8*nchs);
 /* make this up to 7 bytes larger than strictly needed */
 *slen = WRDBYTES*(wlen + 1);
 s = __my_malloc(*slen);

 for (si = nchs - 1, wi = 0; wi < wlen - 1; si -= 4, wi++)
  {
   s[si] = (char) (ap[wi] & 0xffL);
   s[si - 1] = (char) ((ap[wi] >> 8) & 0xffL);
   s[si - 2] = (char) ((ap[wi] >> 16) & 0xffL);
   s[si - 3] = (char) ((ap[wi] >> 24) & 0xffL);
  }
 if (si < 0) goto done;
 s[si--] = (char) (ap[wi] & 0xffL);
 if (si < 0) goto done;
 s[si--] = (char) ((ap[wi] >> 8) & 0xffL);
 if (si < 0) goto done;
 s[si--] = (char) ((ap[wi] >> 16) & 0xffL);
 if (si < 0) goto done;
 s[si--] = (char) ((ap[wi] >> 24) & 0xffL);
 /* format of verilog value string wrong */
 if (si != -1) __misc_terr(__FILE__, __LINE__);
done:
 /* after filled form high end to low put in high ending \0 */
 s[nchs] = '\0';
 return(s);
}

/*
 * convert strength wire expr. (maybe range) to string 
 */
extern char *__strenexpr_tostr(char *s, struct expr_t *xp)
{
 int32 sav_sofs = __cur_sofs;

 sdisp_st(xp);
 /* here keeps low part if trunc needed */
 __trunc_cstr(&(__exprline[sav_sofs]), MSGTRUNCLEN, TRUE);
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = '\0';
 return(s);
}

/*
 * display one strength value - can only be scalar or bit select
 * checks for required simple expr.
 * this must not leave anything on stack
 * also fills exprline from current place
 *
 * notice for non strength this will add strong
 * also notice this has standard extension to allow printing vectors
 */
static void sdisp_st(struct expr_t *xp)
{
 register int32 bi;
 int32 first_time;
 byte *sbp; 
 struct xstk_t *xsp;
 char s1[10];

 /* eval. the expression - if non strength strong added */
 xsp = __ndst_eval_xpr(xp); 
 sbp = (byte *) xsp->ap;
 for (first_time = TRUE, bi = xsp->xslen/4 - 1; bi >= 0; bi--)
  {
   if (first_time) first_time = FALSE; else addch_(' '); 
   __adds(__to_vvstnam(s1, (word32) sbp[bi]));
  }
 __pop_xstk();  
}

/*
 * NUMERIC EXPRESSION DISPLAY WORK ROUTINES - EXPR DETERMINES FMT 
 * INTERNAL INTERFACE TO SAME MECHANISM AS DO_DISP
 */

/*
 * truncated string form of num expr to value
 * this uses high env 
 */
extern char *__msgnumexpr_tostr(char *s, struct expr_t *ndp, int32 inum)
{
 int32 sav_sofs = __cur_sofs;
 
 numexpr_disp(ndp, inum);
 __trunc_cstr(&(__exprline[sav_sofs]), MSGTRUNCLEN, FALSE);
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = '\0'; 
 return(s);
}

/*
 * build a numeric expr. value in exprline - grows if needed
 * use ndp node info to attempt to reconstruct input string
 *
 * does not set cur_sofs - can add into middle, but caller must set it
 * also if inum not known and IS form - must pass 0
 */
static void numexpr_disp(struct expr_t *ndp, int32 inum)
{
 word32 *ap, *bp, *wp;
 int32 wlen, base;
 double *dp, d1;
 char s1[RECLEN];

 if (ndp == NULL)
  {
   __pv_warn(515, "INTERNAL - trying to print NULL expr.");
   __adds("NULL expr.");
   return;
  }
 wlen = wlen_(ndp->szu.xclen);
 switch ((byte) ndp->optyp) {
  case ISNUMBER:
   wp = &(__contab[ndp->ru.xvi]);
   ap = &(wp[2*wlen*inum]);
   break;
  case NUMBER:
   ap = &(__contab[ndp->ru.xvi]);
   break;
  case ISREALNUM:
   /* SJM 05/05/05 - must multiply inum by 2 since real cons now use a/bs */
   dp = (double *) &(__contab[ndp->ru.xvi + 2*inum]); 
   d1 = *dp;
disp_real:
   sprintf(s1, "%g", d1);
   __adds(s1);
   return;
 case REALNUM:
   dp = (double *) &(__contab[ndp->ru.xvi]); 
   d1 = *dp;
   goto disp_real;
  default: __case_terr(__FILE__, __LINE__); return;
 }
 if (ndp->is_string)
  {
   /* if is string on, know will not contain x/z bits, input as ".." */
   /* know cur sofs saved and restored by caller */
   __xline_vval_to_cstr(ap, ndp->szu.xclen, TRUE, TRUE, TRUE);
   return;
  }
 bp = &(ap[wlen]);
 if (__force_base == BNONE) base = ndp->ibase; else base = __force_base;
 switch ((byte) base) {
  case BDEC:
   if ((ndp->unsiznum || ndp->szu.xclen == WBITS)
    && vval_is0_(bp, ndp->szu.xclen))
    {
     /* unsized (no 'd) decimal always signed */
     sdispd(ap, bp, ndp->szu.xclen, TRUE, TRUE);
     break;
    }
   if (ndp->sizdflt) __adds("'d");
   else { sprintf(s1, "%d'd", ndp->szu.xclen); __adds(s1); }
   /* if no. even decimal has 'd, always word32 and trimmed */
   sdispd(ap, bp, ndp->szu.xclen, TRUE, FALSE);
   break;
  case BHEX:
   /* know rest all word32 */
   /* [size]'h prefixed number always trimmed */
   if (ndp->sizdflt) __adds("'h");
   else { sprintf(s1, "%d'h", ndp->szu.xclen); __adds(s1); }
   sdisph(ap, bp, ndp->szu.xclen, TRUE);
   break;
  case BOCT:
   if (ndp->sizdflt) __adds("'o");
   else { sprintf(s1, "%d'o", ndp->szu.xclen); __adds(s1); }
   sdispo(ap, bp, ndp->szu.xclen, TRUE);
   break;
  case BBIN:
   if (ndp->sizdflt) __adds("'b");
   else { sprintf(s1, "%d'b", ndp->szu.xclen); __adds(s1); }
   __sdispb(ap, bp, ndp->szu.xclen, TRUE);
   break;
  default:
   __case_terr(__FILE__, __LINE__);
   return;
 }
}

/*
 * truncate __exprline to newsize - __cur_sofs points to end
 * know truncation must be less than RECLEN (512)
 */
extern void __trunc_exprline(int32 newsize, int32 from_front)
{
 if (__cur_sofs < newsize) return;

 if (from_front)
  {
   char s1[RECLEN];

   strcpy(s1, "...");
   strcat(s1, &(__exprline[__cur_sofs - newsize - 3]));
   strcpy(__exprline, s1);
   __cur_sofs = newsize;
  }
 else
  {
   __cur_sofs = newsize;
   __exprline[__cur_sofs] = '\0';
   __exprline[__cur_sofs - 3] = '.';
   __exprline[__cur_sofs - 2] = '.';
   __exprline[__cur_sofs - 1] = '.';
  }
}

/*
 * truncate a cstring know new size inside allocated size of string
 * this removes prefix when truncation needed 
 * know truncation must be less than RECLEN (512)
 */
extern void __trunc_cstr(char *s, int32 newsize, int32 from_front)
{
 int32 slen;

 if ((slen = strlen(s)) < newsize) return;
   
 if (from_front)
  {
   char s1[RECLEN];

   strcpy(s1, "...");
   /* since string starts at 0 to start at pos. k index is k - 1 */
   strcat(s1, &(s[slen - newsize - 3 - 1]));
   strcpy(s, s1);
  }
 else
  {
   s[slen - 3] = '.';
   s[slen - 2] = '.';
   s[slen - 1] = '.';
   s[slen] = '\0';
  }
}

/*
 * PRINTING COPY INTO STRING INTERFACE ROUTINES TO NORMAL PRINT 
 */

/*
 * version of regab to string that gets format from expression
 * this handles string constants in both forms
 * LOOKATME - how does this routine differ from num expr disp
 */
extern char *__xregab_tostr(char *s, word32 *ap, word32 *bp, int32 blen,
 struct expr_t *xp)
{
 int32 signv, base;

 /* if expr. is string, write as unquoted string */
 if (xp->is_string)
  {
   __strab_tostr(s, ap, xp->szu.xclen, FALSE, TRUE);
   return(s);
  }
 base = BHEX;
 signv = FALSE;
 if (xp->is_real) { base = BDBLE; signv = TRUE; }
 else
  {  
   /* can com close to constant number format duplication */
   if  (xp->optyp == NUMBER || xp->optyp == ISNUMBER)
    {
     if (xp->unsiznum) { base = BDEC; signv = TRUE; }   
     else base = xp->ibase; 
    }
   /* notice if simple varible signed, has sign on */
   if (xp->has_sign) { base = BDEC; signv = TRUE; }
  }
 __regab_tostr(s, ap, bp, blen, base, signv);
 return(s);
}

/*
 * version of regab to string that gets format from parameter ncomp record
 * this handles string constants in both forms
 */
extern char *__pregab_tostr(char *s, word32 *ap, word32 *bp, struct net_t *np)
{
 int32 signv, base;

 /* DBG remove --- */
 if (np->nrngrep != NX_CT || !np->n_isaparam)
  __misc_terr(__FILE__, __LINE__);
 /* --- */ 

 /* if original param rhs string, write as unquoted string */
 if (np->nu.ct->pstring)
  {
   __strab_tostr(s, ap, np->nwid, FALSE, TRUE);
   return(s);
  }
 base = BHEX;
 signv = FALSE;
 if (np->ntyp == N_REAL) { base = BDBLE; signv = TRUE; }
 else
  {  
   base = np->nu.ct->pbase;
   if (np->n_signed) signv = TRUE; else signv = FALSE;
  }
 __regab_tostr(s, ap, bp, np->nwid, base, signv);
 return(s);
}

/*
 * convert reg. ab style value to string - always trim
 * can always print value by loading to stk and calling this
 *
 * notice this prints passed string not chp of next pos.
 * string passed must be at least RECLEN 
 */
extern char *__regab_tostr(char *s, word32 *ap, word32 *bp, int32 blen, int32 base,
 int32 hassign)
{
 int32 sav_sofs = __cur_sofs;

 __regab_disp(ap, bp, blen, base, TRUE, hassign);
 /* here keeps low part if trunc needed */
 __trunc_cstr(&(__exprline[sav_sofs]), MSGTRUNCLEN, TRUE);
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = '\0';
 return(s);
}

/*
 * version of regab to string that allows turning trim off
 */
extern char *__regab2_tostr(char *s, word32 *ap, word32 *bp, int32 blen, int32 base,
 int32 hassign, int32 trim)
{
 int32 sav_sofs = __cur_sofs;

 __regab_disp(ap, bp, blen, base, trim, hassign);
 __trunc_cstr(&(__exprline[sav_sofs]), MSGTRUNCLEN, TRUE);
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = '\0';
 return(s);
}

/*
 * special display routine that takes base as argrument
 * but does not output sizes
 * to print value subrange caller must adjust passed ap and bp and blen
 *
 * notice if can be using exprline caller must save
 */
extern void __regab_disp(word32 *ap, word32 *bp, int32 blen, int32 base, int32 trim,
 int32 hassign)
{
 char s1[RECLEN];

 switch ((byte) base) {
  case BBIN: __sdispb(ap, bp, blen, trim); break;
  case BOCT: sdispo(ap, bp, blen, trim); break;
  case BDEC: sdispd(ap, bp, blen, trim, hassign); break;
  case BHEX: sdisph(ap, bp, blen, trim); break;
  case BDBLE:
   {
    double d1;

    memcpy(&d1, ap, sizeof(double));
    /* here always try to trim to match OVIsim */
    sprintf(s1, "%g", d1);
    __adds(s1);
   }
   break;
  default:
   __pv_err(768, "numeric display format illegal (value %d)", base); 
 }
}

/*
 * convert strength style value to string
 * can always print value by loading to stk and calling this
 * s must be at least RECLEN wide
 */
extern char *__st_regab_tostr(char *s, byte *sbp, int32 blen)
{
 int32 sav_sofs = __cur_sofs;

 st_regab_disp(sbp, blen);
 /* truncate from front if needed - keep low bits */
 __trunc_cstr(&(__exprline[sav_sofs]), MSGTRUNCLEN, TRUE);
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = '\0'; 
 return(s);
}

/*
 * display a strength value into expr line
 */
static void st_regab_disp(byte *sbp, int32 blen)
{
 register int32 i;
 char s1[RECLEN];

 /* maybe need some kind of separator here */
 for (i = blen - 1;; i--)
  {
   __adds(__to_vvstnam(s1, (word32) sbp[i]));
   if (i <= 0) break;
   addch_(' '); 
  }
 __exprline[__cur_sofs] = '\0';
}

/*
 * to an expression wire range - x1 must be != NULL
 * uses end of expr line in case in use 
 */
extern char *__msgtox_wrange(char *s, struct expr_t *x1, struct expr_t *x2)
{
 int32 sav_sofs = __cur_sofs;

 tox_wrange((FILE *) NULL, x1, x2);
 __trunc_cstr(&(__exprline[sav_sofs]), MSGTRUNCLEN, FALSE);
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = '\0'; 
 return(s);
}

/*
 * version of expr to string that truncates for messages
 * this dumps unevaluated expression not value
 */
extern char *__msgexpr_tostr(char *s, struct expr_t *ndp)
{
 int32 sav_sofs = __cur_sofs;

 dmp_expr((FILE *) NULL, ndp);
 __trunc_cstr(&(__exprline[sav_sofs]), MSGTRUNCLEN, FALSE);
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = '\0'; 
 return(s);
}

/*
 * FOR DEBUGGING VARIABLE DUMP ROUTINES
 */

/*
 * load a value
 */

/*
 * convert a variable to a string
 */
extern char *__var_tostr(char *s, struct net_t *np, int32 i1, int32 i2, int32 base)
{
 int32 sav_sofs = __cur_sofs;

 /* use sign from var. */
 __disp_var(np, i1, i2, base, '?');
 /* truncate from front if needed - keep low bits */
 __trunc_cstr(&(__exprline[sav_sofs]), MSGTRUNCLEN, TRUE);
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = '\0'; 
 return(s);
}

/*
 * display variable (possibly part or bit select range out of [i1,i2]) with
 * base base and force signed '-' or word32 ' ' or use variable sign
 * in other case
 *
 * for array - may need to be called multiple times but will index one
 * entire array if i1==i2 and in range
 *
 * expects __cur_sofs to be set and builds in expr line 
 */  
extern void __disp_var(struct net_t *np, int32 i1, int32 i2, int32 base,
 char vsign_ovride)
{
 int32 arrwid, hassign, netwid;
 double d1;
 struct xstk_t *xsp;
 char s1[RECLEN];

 /* need var. for net wid since may change width for printing below */
 netwid = np->nwid;
 /* case 1: strength */
 if (np->n_stren) { disp_stvar(np, i1, i2); return; } 

 /* case 2: real variable (no arrays of reals) */
 if (np->ntyp == N_REAL) 
  {
   memcpy(&d1, &(np->nva.wp[2*__inum]), sizeof(double));
   sprintf(s1, "%g", d1);
   __adds(s1);
   return;
  } 
 hassign = FALSE;
 /* case 3: array - must be 1 index */
 if (np->n_isarr)
  {
   /* must have exactly one index */
   arrwid = __get_arrwide(np);
   if (i1 == i2 && i1 == -1) 
    {
     strcpy(s1, "[first of entire array]: ");
     __adds(s1);
     i1 = 0;
    }
   else
    {
     if (i1 < 0 || i1 != i2 || i1 >= arrwid) __arg_terr(__FILE__, __LINE__);
    }
   push_xstk_(xsp, netwid);
   __ld_arr_val(xsp->ap, xsp->bp, np->nva, arrwid, netwid, i1);
   if (netwid > WBITS) goto do_print;
   hassign = get_ovrsign(np, vsign_ovride);
   goto do_print;
  }
 /* case 4: scalar or all of vector */
 /* case 4a: all of variable */
 if (i1 == -1)
  {
   push_xstk_(xsp, netwid); 
   __ld_wire_val(xsp->ap, xsp->bp, np);
   if (netwid > WBITS) goto do_print;
   hassign = get_ovrsign(np, vsign_ovride);
   goto do_print;
  } 
 /* 4b part of var. - must be vector do select */
 else 
  {
   if (!np->n_isavec) __case_terr(__FILE__, __LINE__);
   /* case 4b-a: bit select */
   if (i1 == i2)
    {
     if (i1 < 0 || i1 >= netwid) __case_terr(__FILE__, __LINE__);
     push_xstk_(xsp, 1); 
     __ld_bit(xsp->ap, xsp->bp, np, i1);
     netwid = 1;
     goto do_print;
    }
   /* case 4b-b: part select */
   if (i1 < 0 || i2 < 0 || i1 < i2 || i1 >= netwid)
    __case_terr(__FILE__, __LINE__);
   push_xstk_(xsp, netwid);
   /* notice here nwid must be width of net */ 
   __ld_psel(xsp->ap, xsp->bp, np, i1, i2);
   /* but display nwid as width of part select */
   netwid = i1 - i2 + 1;
  }
do_print:
 __regab_disp(xsp->ap, xsp->bp, netwid, base, TRUE, hassign);
 __pop_xstk();
} 

/*
 * display a possible section of a strength variable (from the wire)
 */
static void disp_stvar(struct net_t *np, int32 i1, int32 i2)
{
 register int32 i;
 char s1[RECLEN];

 if (i1 == -1) { i1 = np->nwid - 1; i2 = 0; }
 /* maybe need some kind of separator here */
 for (i = i1; i >= i2; i--)
  {
   __adds(__to_vvstnam(s1, (word32) np->nva.bp[i]));
   if (i > i2) addch_(' ');
  }
 __exprline[__cur_sofs] = '\0'; 
}

/*
 * return sign by testing over-ride variable - if not use wire sign
 */
static int32 get_ovrsign(struct net_t *np, char ovride)
{
 if (ovride == '-') return(TRUE);
 else if (ovride == ' ') return(FALSE);
 return(np->n_signed == 1);
}

/*
 * dump all of a array for inst ifr to ito
 * only called if debug on and inst ptr set
 *
 */
extern void __dmp_arr_all(struct net_t *np, int32 ifr, int32 ito)
{
 if (!np->n_isarr) __arg_terr(__FILE__, __LINE__);
 dmp_arr_insts(np, ifr, ito);
}

/*
 * dmp instance ifr to ito of arr var. np from mfr to mto
 * mfr and mto assume normalized h:0 form not actual range
 */
static void dmp_arr_insts(struct net_t *np, int32 ifr, int32 ito)
{
 register int32 i;
 int32 ri1, ri2, arrwid;

 if (!np->n_isarr) __arg_terr(__FILE__, __LINE__);
 __getarr_range(np, &ri1, &ri2, &arrwid);
 for (i = ifr; i <= ito; i++) dmp_arr(np, ri1, ri2, i);
}

/*
 * dump an array from index mifr to mito for instance inum
 */
static void dmp_arr(struct net_t *np, int32 mifr, int32 mito, int32 inum)
{
 register int32 mi;
 int32 arrwid, ri1, ri2, h0_arri;
 struct xstk_t *xsp;

 if (!np->n_isarr) __arg_terr(__FILE__, __LINE__);
 __getarr_range(np, &ri1, &ri2, &arrwid);

 __push_wrkitstk(__inst_mod, inum);
 push_xstk_(xsp, np->nwid);
 for (mi = mifr; mi <= mito; mi++)
  {
   /* FIXME - think this is wrong should unnormalize? */ 
   h0_arri = normalize_ndx_(mi, ri1, ri2);
   __ld_arr_val(xsp->ap, xsp->bp, np->nva, arrwid, np->nwid, h0_arri);
   __dbg_msg("array %s inst %d width %d index %d = %s\n", np->nsym->synam,
    inum, np->nwid, mi, __regab_tostr(__xs, xsp->ap, xsp->bp, np->nwid,
    BHEX, FALSE));
  }
 __pop_xstk();
 __pop_wrkitstk();
}

/*
 * FOR DEBUGGING ROUTINES TO DUMP NET PIN LISTS
 */

/*
 * dump the np and dce lists for one module 
 * only called if debug flag on
 */
extern void __dmpmod_nplst(struct mod_t *mdp, int32 dces_only)
{
 register int32 ni;
 register struct net_t *np;
 register struct task_t *tskp;

 /* DBG remove ---
 --   int32 ii;
 if (__debug_flg)
  {
   for (ii = 0; ii < __numtopm; ii++) __dmp_itree(__it_roots[ii]);
  }
 --- */

 if (dces_only)
  __dbg_msg("++> module %s event list\n", mdp->msym->synam);
 else __dbg_msg("++> module %s net pin and event lists\n", mdp->msym->synam);

 if (mdp->mnnum != 0)
  {
   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++)
    dmp1n_nplst(mdp, np, dces_only);
  }
 for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt) 
  {
   __dbg_msg("**> task %s net pin list\n", tskp->tsksyp->synam);
   if (tskp->trnum != 0)
    {
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      dmp1n_nplst(mdp, np, dces_only);
    }
  }
}

/*
 * dump the net pin list elements for 1 net
 */
static void dmp1n_nplst(struct mod_t *mdp, struct net_t *np, int32 dces_only)
{
 register struct net_pin_t *npp;
 register struct dcevnt_t *dcep;
 int32 first_time;
 char s1[RECLEN];

 if (np->ntyp == N_EVENT)
  {
   __dbg_msg("==> event %s - no net loads and no drivers\n",
    np->nsym->synam);
   __dbg_msg("==> %s %s size %d with %d event ctrls:\n",
    __to_wtnam(s1, np), np->nsym->synam, np->nwid,
    __cnt_dcelstels(np->dcelst));
   goto try_dces;
  }

 __dbg_msg("==> %s %s size %d with %d drivers, %d loads and %d event ctrls:\n",
  __to_wtnam(s1, np), np->nsym->synam, np->nwid,
  cnt_nplstels(np->ndrvs), cnt_nplstels(np->nlds),
  __cnt_dcelstels(np->dcelst));

 if (dces_only) goto try_dces;

 first_time = TRUE;
 for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
  {
   if (first_time) { __dbg_msg("  ++drivers ->\n"); first_time = FALSE; }
   __dmp1_nplstel(mdp, np, npp);
  }
 first_time = TRUE;
 for (npp = np->nlds; npp != NULL; npp = npp->npnxt)
  {
   if (first_time) { __dbg_msg("  ++loads ->\n"); first_time = FALSE; }
   __dmp1_nplstel(mdp, np, npp);
  }

try_dces:
 first_time = TRUE;
 /* notice #<num> delays are just scheduled no triggering */
 for (dcep = np->dcelst; dcep != NULL; dcep = dcep->dcenxt)
  {
   if (dcep->dce_off) continue;

   if (first_time)
    { __dbg_msg("  ++event controls ->\n"); first_time = FALSE; }
   __dmp1_dcelstel(mdp, dcep);
  }
}

/*
 * count drivers or loads (inst. specific counted for each inst.)
 * SJM 07/25/01 - although MIPD is a load, it is not counted
 */
static int32 cnt_nplstels(register struct net_pin_t *npp)
{ 
 int32 i;

 for (i = 0; npp != NULL; npp = npp->npnxt)
  {
   if (npp->npntyp != NP_MIPD_NCHG) i++;
  }
 return(i);
}

/*
 * count delay control/dump vars events
 */
extern int32 __cnt_dcelstels(register struct dcevnt_t *dcep)
{ 
 int32 i;

 for (i = 0; dcep != NULL; dcep = dcep->dcenxt)
  {
   if (dcep->dce_off) continue;
   i++;
  }
 return(i);
}

/*
 * dump 1 net pin list element
 *
 * includes the removed after tran chan build removed npps
 * mdp only need for mod port or instance conns npps
 */
extern void __dmp1_nplstel(struct mod_t *mdp, struct net_t *np,
 struct net_pin_t *npp)
{
 word32 stval;
 struct inst_t *ip;
 struct gate_t *gp;
 struct mod_t *imdp, *tmp_mdp;
 struct conta_t *cap;
 struct tfrec_t *tfrp;
 struct tchk_t *tcp;
 struct spcpth_t *pthp;
 struct npaux_t *npauxp;
 struct itree_t *itp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[RECLEN];

 if ((npauxp = npp->npaux) != NULL && npauxp->nbi1 != -1)
  {
   if (npauxp->lcbi1 == -1)
    __dbg_msg("    [%d:%d]: ", npauxp->nbi1, npauxp->nbi2.i);
   else __dbg_msg("    [%d:%d] (lhs cat rhs [%d:%d]): ",
    npauxp->nbi1, npauxp->nbi2.i, npauxp->lcbi1, npauxp->lcbi2);
  }
 else __dbg_msg("    ");

 /* handle proc type - need a module (can by any) */
 switch ((byte) npp->npproctyp) {
  case NP_PROC_INMOD:
   tmp_mdp = mdp;
   sprintf(s3, "(IN=%s)", tmp_mdp->msym->synam); break;
  case NP_PROC_GREF:
   tmp_mdp = npauxp->npu.npgrp->gin_mdp;
   sprintf(s3, "(XMR %s mod=%s)", npauxp->npu.npgrp->gnam,
    tmp_mdp->msym->synam);
   break;
  case NP_PROC_FILT:
   itp = npauxp->npdownitp;
   tmp_mdp = itp->itip->imsym->el.emdp;
   sprintf(s3, "(INST-LOC=%s(%s))", __msg2_blditree(__xs, itp),
    tmp_mdp->msym->synam);
   break;
  default: __case_terr(__FILE__, __LINE__); return;
 }
 switch ((byte) npp->np_xmrtyp) {
  case XNP_LOC: strcpy(s4, "-LOCAL-"); break;
  case XNP_DOWNXMR: strcpy(s4, "-DOWNXMR-"); break;
  case XNP_RTXMR: strcpy(s4, "-RTXMR-"); break;
  case XNP_UPXMR: strcpy(s4, "-UPXMR-"); break;
  default: __case_terr(__FILE__, __LINE__); return;
 }
 switch ((byte) npp->npntyp) {
  case NP_ICONN: case NP_BIDICONN:
   /* current mod is target but need ref module (of first inst?) */
   /* DBG remove -- */
   if (npp->elnpp.eii >= tmp_mdp->minum || tmp_mdp->minum == 0)
    __arg_terr(__FILE__, __LINE__);
   /* --- */
   /* notice normally actual instance relative to itree place */
   ip = &(tmp_mdp->minsts[npp->elnpp.eii]); 
   __dbg_msg("inst. %s type %s %s %s port %d at %s\n", ip->isym->synam,
    ip->imsym->synam, s3, s4, npp->obnum + 1,
    __bld_lineloc(__xs, ip->isym->syfnam_ind, ip->isym->sylin_cnt));
   break;
  case NP_PB_ICONN:
   ip = &(tmp_mdp->minsts[npp->elnpp.eii]); 
   __dbg_msg("inst. %s type %s %s %s port %d bit %d at %s\n",
    ip->isym->synam, ip->imsym->synam, s3, s4, npp->obnum + 1,
    npp->pbi, __bld_lineloc(__xs, ip->isym->syfnam_ind, ip->isym->sylin_cnt));
   break;
  case NP_GATE: case NP_TRANIF: case NP_TRAN:
   gp = npp->elnpp.egp;
   __dbg_msg("gate %s type %s %s %s port %d at %s\n", gp->gsym->synam,
    gp->gmsym->synam, s3, s4, npp->obnum + 1, __bld_lineloc(__xs,
    gp->gsym->syfnam_ind, gp->gsym->sylin_cnt));
   break;
  case NP_CONTA:
   cap = npp->elnpp.ecap;
   if (cap->ca_pb_sim)
    {
     __dbg_msg("per bit continuous assign bit %d at %s %s %s\n", npp->pbi,
      __bld_lineloc(__xs, cap->casym->syfnam_ind, cap->casym->sylin_cnt),
      s3, s4);
    }
   else
    {
     __dbg_msg("continuous assign at %s %s %s\n", __bld_lineloc(__xs,
      cap->casym->syfnam_ind, cap->casym->sylin_cnt), s3, s4);
    }
   break;
  case NP_TFRWARG:
   tfrp = npp->elnpp.etfrp;
   if (tfrp->tf_func) strcpy(s1, "function"); else strcpy(s1, "task");
   __dbg_msg("tf_ %s %s read/write argument (pos. %d) at %s\n", s3, s4,
    npp->obnum, __bld_lineloc(__xs, tfrp->tffnam_ind, tfrp->tflin_cnt));
   break;
  case NP_VPIPUTV:
   if (npp->npaux == NULL || npp->npaux->nbi1 == -1)
    strcpy(__xs, "vpiWireDriver");
   else strcpy(__xs, "vpiWireBitDriver");
   __dbg_msg("vpi_ added %s for net %s %s %s\n", __xs, np->nsym->synam, s3,
    s4);
   break;
  case NP_MDPRT: case NP_BIDMDPRT:
   imdp = npp->elnpp.emdp;
   __dbg_msg("module %s %s %s %s port %s (pos. %d) at %s\n", imdp->msym->synam,
    s3, s4, __to_ptnam(s1, np->iotyp),
    __to_mpnam(s2, imdp->mpins[npp->obnum].mpsnam), npp->obnum + 1,
    __bld_lineloc(__xs, mdp->msym->syfnam_ind, mdp->msym->sylin_cnt));
   break;
  case NP_PB_MDPRT: 
   imdp = npp->elnpp.emdp;
   __dbg_msg("module %s %s %s %s port %s (pos. %d) bit %d at %s\n",
    imdp->msym->synam, s3, s4, __to_ptnam(s1, np->iotyp),
    __to_mpnam(s2, imdp->mpins[npp->obnum].mpsnam), npp->obnum + 1,
    npp->pbi, __bld_lineloc(__xs, mdp->msym->syfnam_ind,
    mdp->msym->sylin_cnt));
   break;
  case NP_MIPD_NCHG:
   __dbg_msg("MIPD delay device in module %s for %s %s\n",
    mdp->msym->synam, __to_ptnam(s1, np->iotyp), np->nsym->synam);
   break;
  case NP_PULL:
   gp = npp->elnpp.egp;
   stval = gp->g_stval << 2;
   if (npp->pullval == 1) stval |= 1; 
   __dbg_msg("%s to %s on %s at %s\n",
    gp->gmsym->synam, __to_vvstnam(s1, (word32) stval),  
    __msgexpr_tostr(__xs, gp->gpins[npp->obnum]), __bld_lineloc(__xs2,
    gp->gsym->syfnam_ind, gp->gsym->sylin_cnt));
   break;
  case NP_TCHG:
   switch ((byte) npp->chgsubtyp) {
    case NPCHG_TCSTART: 
     tcp = npp->elnpp.etchgp->chgu.chgtcp;
     __dbg_msg("%s timing check first (reference) change line %s\n",
      __to_tcnam(__xs, tcp->tchktyp), __bld_lineloc(__xs2,
      tcp->tcsym->syfnam_ind, tcp->tcsym->sylin_cnt));
     break;
    case NPCHG_TCCHK:
     tcp = npp->elnpp.echktchgp->startchgp->chgu.chgtcp;
     __dbg_msg("%s timing check 2nd (check) change line %s\n",
      __to_tcnam(__xs, tcp->tchktyp), __bld_lineloc(__xs2,
      tcp->tcsym->syfnam_ind, tcp->tcsym->sylin_cnt));
     break;
    case NPCHG_PTHSRC:
     pthp = npp->elnpp.etchgp->chgu.chgpthp;
     __dbg_msg("delay path source change line %s\n",
      __bld_lineloc(__xs, pthp->pthsym->syfnam_ind,
      pthp->pthsym->sylin_cnt));
     break;
    default: __case_terr(__FILE__, __LINE__);
   }
   break;
  default:
   __gfterr(302, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
    "unknown net pin value %d", npp->npntyp);
 }
}

/*
 * dump 1 net pin list element
 *
 * this does not dump base dvcodes because caller does not know if net
 * in task or module
 */
extern void __dmp1_dcelstel(struct mod_t *mdp, struct dcevnt_t *dcep)
{
 struct st_t *stp; 
 struct delctrl_t *dctp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 if (dcep->dce_edge)
  {
   /* access value */
   sprintf(s1, " %s (first instance old value %s)",
    __to_edgenam(s2, dcep->dce_edgval), __to_vvnam(s3,
    get_dce_edgeval(mdp, dcep)));
  }
 else strcpy(s1, "");
 if (dcep->dce_off) __dbg_msg("*OFF*");
 switch ((byte) dcep->dce_typ) {
  case DCE_RNG_INST: case DCE_INST:
   __dbg_msg("    [%d:%d]: event control net %s trigger: %s",
    dcep->dci1, dcep->dci2.i, dcep->dce_np->nsym->synam, s1);
   dctp = dcep->st_dctrl;
   /* DBG remove --- */
   if (dctp->dctyp < DC_EVENT || dctp->dctyp > DC_WAITEVENT)
    __misc_terr(__FILE__, __LINE__);
   /* --- */
   stp = dcep->st_dctrl->actionst;
   if (stp == NULL) __dbg_msg("\n");
   else __dbg_msg(" at %s\n", __bld_lineloc(s2, stp->stfnam_ind,
    stp->stlin_cnt));
   break;
  case DCE_RNG_MONIT:
  case DCE_MONIT:
   /* maybe need more info here */
   __dbg_msg("    [%d:%d]: monitor net %s trigger\n", dcep->dci1,
    dcep->dci2.i, dcep->dce_np->nsym->synam); 
   /* FIXME - only defined for monitor - should also dump for rng form */
   /* LOOKATME - can this happen since no more col. */
   if (dcep->dce_1inst)
    {
     __dbg_msg("   (xmr match: target %s - down src: %s)\n", 
      __msg2_blditree(s1, dcep->dce_matchitp), __msg2_blditree(s2,
      dcep->dce_refitp));
    }
   break;
  case DCE_RNG_QCAF: case DCE_QCAF:
   __dbg_msg("    [%d:%d]: quasi-continuous assign or force load net %s\n",
    dcep->dci1, dcep->dci2.i, dcep->dce_np->nsym->synam); 
   break;
  case DCE_RNG_PVC: case DCE_PVC:
   __dbg_msg("    [%d:%d]: tf_ parameter value change load net %s\n",
    dcep->dci1, dcep->dci2.i, dcep->dce_np->nsym->synam); 
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * load a dce edge value with strength removed if attached to stren wire
 * know 1 bit and does not use itree loc. - gets from 1st instance
 */
static word32 get_dce_edgeval(struct mod_t *mdp, struct dcevnt_t *dcep)
{
 struct net_t *np;
 int32 dcewid;
 byte *sbp;
 word32 val, aw, bw;
 struct xstk_t *xsp;
 
 /* here edge always 1 bit */
 if (dcep->dce_expr != NULL)
  {
   __push_wrkitstk(mdp, 0);
   ld_scalval_(&aw, &bw, dcep->dce_expr->bp);
   val = (aw & 1L) | ((bw << 1) & 2L);
   __pop_wrkitstk();
  }
 else
  {
   np = dcep->dce_np;
   /* this is low bit of 1st inst. */
   if (np->n_stren) { sbp = dcep->prevval.bp; val = (word32) (sbp[0] & 3); }
   else
    {
     /* since edge, know low bit used for range */
     dcewid = __get_dcewid(dcep, np);

     __push_wrkitstk(mdp, 0);
     /* special case load of perinst value */
     push_xstk_(xsp, dcewid);
     __ld_perinst_val(xsp->ap, xsp->bp, dcep->prevval, dcewid);
     val = (xsp->ap[0] & 1L) | ((xsp->bp[0] << 1) & 2L);
     __pop_xstk();
     __pop_wrkitstk();
    }
  }
 return(val);
}

/*
 * MISC. VALUE CONVERSION ROUTINES TOO COMPLICATED TO BE MACROS
 * USED FOR VALUE CONVERSION EVERYWHERE
 */

/*
 * trim binary value by changing length (for binary only)
 * so if all left bits 0, x, or z compute new length
 *
 * if all highs 0, trim to highest non 0, if 0 make 1
 * if all high x or z trim so include 1 high x or z bit
 * return new trimmed length
 *
 * this maybe would be better converting to blen and using that
 */
static int32 bin_trim_abval(word32 *ap, word32 *bp, int32 blen)
{
 int32 ahigh0, bhigh0;
 int32 wlen, ubits, trimalen, trimblen, nblen;

 /* this adjusts 0 to WBITS */
 wlen = wlen_(blen);
 ubits = ubits_(blen);
 if (ubits == 0) ubits = WBITS;
 /* notice range for bits is [32:1] */
 if (bithi_is0(ap[wlen - 1], ubits)) ahigh0 = TRUE; else ahigh0 = FALSE;
 if (bithi_is0(bp[wlen - 1], ubits)) bhigh0 = TRUE; else bhigh0 = FALSE;
 /* if high bit 1, cannot trim */
 if (bhigh0 && !ahigh0) return(blen);

 /* if high bit 0 - len is width where non 0 high a and non 0 highb */
 /* if high bit z - len is width where non 0 high a and non 1 highb */
 /* if high bit x - len is width where non 1 high a and non 1 highb */
 if (ahigh0) trimalen = __trim1_0val(ap, blen);
 else trimalen = trim1_1val(ap, blen);
 if (bhigh0) trimblen = __trim1_0val(bp, blen);
 else trimblen = trim1_1val(bp, blen);
 nblen = (trimalen >= trimblen) ? trimalen : trimblen;

 /* if all 0s, x's, or z's, make 1 bit */
 if (nblen == 0) return(1);

 /* if trimmed x or z always need high x or z */ 
 /* AIV 01/18/06 - if the high bit was one x or z was returning an extra bit */
 if (!bhigh0)
  {
   if (blen == nblen) return(nblen);
   return(nblen + 1);
  }

 /* trimmed 0's to something */
 /* if trim 0's to x or z - need the to inc for one extra high 0 */ 
 wlen = wlen_(nblen);
 ubits = ubits_(nblen);
 if (ubits == 0) ubits = WBITS;
 /* notice range for bits is [32:1] */
 /* if trimmed 0's to 1, do not need extra high 1 */
 if (!bithi_is0(bp[wlen - 1], ubits)) return(nblen + 1);

 /* if trimmed 0's to 1, then no extra high 0 */
 return(nblen);
}

/*
 * trim a value by changing length so if all left bits 0, x, or z compute
 * if all highs 0, trim to highest non 0, if 0 make 1
 * if all high x or z trim so include 1 high x or z bit
 * return new trimmed length
 *
 * this maybe would be better converting to blen and using that
 */
static int32 trim_abval(word32 *ap, word32 *bp, int32 blen)
{
 int32 ahigh0, bhigh0;
 int32 wlen, ubits, trimalen, trimblen, nblen;

 /* this adjusts 0 to WBITS */
 wlen = wlen_(blen);
 ubits = ubits_(blen);
 if (ubits == 0) ubits = WBITS;
 /* notice range for bits is [32:1] */
 if (bithi_is0(ap[wlen - 1], ubits)) ahigh0 = TRUE; else ahigh0 = FALSE;
 if (bithi_is0(bp[wlen - 1], ubits)) bhigh0 = TRUE; else bhigh0 = FALSE;
 /* if high bit 1, cannot trim */
 if (bhigh0 && !ahigh0) return(blen);

 if (ahigh0) trimalen = __trim1_0val(ap, blen);
 else trimalen = trim1_1val(ap, blen);
 if (bhigh0) trimblen = __trim1_0val(bp, blen);
 else trimblen = trim1_1val(bp, blen);
 nblen = (trimalen >= trimblen) ? trimalen : trimblen;
 /* if all 0s, make 1 bit */
 if (nblen == 0) return(1);

 /* if x or z extension, need 1 extra bit */
 /* AIV 01/18/06 - if the high bit was one x or z was returning an extra bit */
 if (!ahigh0 || !bhigh0)
  {
   if (blen == nblen) return(nblen);
   return(nblen + 1);
  }
 return(nblen);
}

/*
 * return T if bit i is 0 (32 >= i >= 1)
 */
static int32 bithi_is0(word32 val, int32 ubits)
{
 word32 mask;

 mask = 1L << (ubits - 1);
 if ((mask & val) == 0L) return(TRUE);
 return(FALSE);
}

/* table used by count leading zeros macro */
static byte clz_tab[] =
{
 0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4, 5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6, 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7,
 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
};

/* notice this routine in 32 bit word32 dependent */
extern int32 __count_leading_zeros(register word32 xr)
{
 register word32 a;
 register int32 count;

 a = xr < (1L << 16) ? (xr < (1L << 8) ? 0 : 8) : (xr < (1L << 24) ? 16: 24);
 count = 32 - (clz_tab[xr >> a] + a);
 return(count);
}

/*
 * trim a 0 value - returns new length that is 0 if all 0's
 * else new trimmed length - know all high unsued bits 0
 *
 * this works on either a or b part but not both at once - caller must
 * combine
 */
extern int32 __trim1_0val(word32 *wp, int32 blen)
{
 register int32 swi;
 int32 hbits, wlen;

 /* adjust so blen value as if all bits in high word32 used */
 wlen = wlen_(blen);
 /* notice this works from high word32 toward low */
 for (swi = wlen - 1; swi >= 0; swi--)
  {
   if (wp[swi] != 0L)
    {
     hbits = __count_leading_zeros(wp[swi]);
     return(WBITS*(swi + 1) - hbits);
    }
  }
 return(0);
}

/*
 * trim a 1 value - returns new length that is 0 if all 1's
 * know all high unsued bits 1 in value with new length
 */
static int32 trim1_1val(word32 *wp, int32 blen)
{
 register int32 swi;
 word32 tmp;
 int32 wlen, hbits, ubits;

 wlen = wlen_(blen);
 ubits = ubits_(blen);
 /* if some bits unused, use mask when inverting for zero count */
 if (ubits != 0)
  {
   wlen--;
   tmp = ~wp[wlen] & __masktab[ubits];
   if (tmp != 0L)
    { hbits = __count_leading_zeros(tmp); return(WBITS*(wlen + 1) - hbits); }
  }
 /* notice this works from high word32 toward low */
 for (swi = wlen - 1; swi >= 0; swi--)
  {
   if ((tmp = ~wp[swi]) != 0L)
    {
     hbits = __count_leading_zeros(tmp);
     return(WBITS*(swi + 1) - hbits);
    }
  }
 return(0);
}

/*
 * return T if value is all z's
 */
extern int32 __vval_isallzs(word32 *ap, word32 *bp, int32 blen)
{
 int32 v1, v2;

 /* v1, v2 here for debugging only */
 v1 = vval_is0_(ap, blen);
 if (!v1) return(FALSE);
 v2 = __vval_is1(bp, blen);
 return(v2);
}

/*
 * return T for strength value that is all z's
 * <0:0=z (i.e. 00000010) is only z value
 */
extern int32 __st_vval_isallzs(byte *sbp, int32 blen)
{
 register int32 i;

 for (i = 0; i < blen; i++) if (*sbp != 3) return(FALSE);
 return(TRUE);
}

/*
 * return T if value is all x's
 */
static int32 vval_isall_xs(word32 *ap, word32 *bp, int32 blen)
{
 if (!__vval_is1(ap, blen)) return(FALSE);
 if (!__vval_is1(bp, blen)) return(FALSE);
 return(TRUE);
}

/*
 * return T is has any x's, even 1 x implies X, else Z
 * notice half word32 here ok
 */
static int32 vval_hasx(word32 *ap, word32 *bp, int32 blen)
{
 register int32 wi;

 for (wi = 0; wi < wlen_(blen); wi++)
  {
   if ((ap[wi] & bp[wi]) != 0L) return(TRUE);
  }
 return(FALSE);
}

/*
 * return T if value is all 1's (processes either a or b value not both)
 * notice half word32 blen here ok
 */
extern int32 __vval_is1(register word32 *wp, int32 blen)
{
 register int32 i;
 register word32 mask;
 int32 wlen;

 wlen = wlen_(blen);
 for (i = 0; i < wlen - 1; i++) if (wp[i] != ALL1W) return(FALSE);
 mask = __masktab[ubits_(blen)];
 if ((wp[wlen - 1] & mask) != mask) return(FALSE);
 return(TRUE);
}

/*
 * return T if value is all 0's (processes either a or b value)
 * passed with 2 word32 length for both a and b values
 * only for values wider than WBITS
 * notice must be word32 since 2 word32 length possible
 *
 * this is invoked from a macro
 */
extern int32 __wide_vval_is0(register word32 *wp, int32 blen)
{
 register int32 i;

 for (i = 0; i < wlen_(blen); i++) if (*wp++ != 0L) return(FALSE);
 return(TRUE);
}

/*
 * REAL CONVERSION ROUTINES
 */

/*
 * convert a 64 bit ver. word32  to a ieee double real value
 * return F on error - caller can emit warning - d1 as good as possible
 *
 * FIXME - since tim now word32 64, maybe can just assign
 * PORTABLE - assumes IEEE floating point
 */
extern int32 __v64_to_real(double *d1, word64 *tim)
{
 int32 good;
 register word32 w0, w1, mask;
 double dbase;

 /* easy case, fit in 32 bits */
 w0 = (word32) ((*tim) & WORDMASK_ULL);
 w1 = (word32) (((*tim) >> 32) & WORDMASK_ULL);
 if (w1 == 0L) { *d1 = (double) w0; return(TRUE); }

 good = TRUE;
 /* must fit in mantissa or will lose precision */
 mask = __masktab[WBITS - (_DEXPLEN + 1)];
 if ((w1 & ~mask) != 0L) good = FALSE;
 dbase = ((double) (0xffffffff)) + 1.0; 
 /* cannot use base 2**32 because conversion to double assumes int32 */
 /* even if value is word32 */
 /* conversion from double to word32 works if double is positive */
 *d1 = (double) w0 + dbase*((double) w1);
 return(good); 
}

/*
 * convert a stack value to a real and 
 *
 * 09/30/06 SJM - thanks to Bryan Catanzaro from Tabula for find this
 *
 * there is a slight bug here because this must also change stack  
 * width to WBITS - needed because the stack value is used later
 * and changing the xslen field is always good - see narrow sizchg
 */
extern double __cnvt_stk_to_real(struct xstk_t *xsp, int32 is_signed)
{
 int32 i, blen;
 word32 mask_val;
 word32 u;
 word64 tim;
 double d1;
 
 if (!vval_is0_(xsp->bp, xsp->xslen))
  {
not_real:
   __sgfwarn(618,
    "expression value %s cannot be represented as real - 0.0 used", 
    __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, is_signed));
   return(0.0);
  }
 if (xsp->xslen > WBITS)
  {
   if (xsp->xslen > WBITS && !vval_is0_(&(xsp->ap[2]), xsp->xslen - 64))
    goto not_real;
   tim = ((word64) xsp->ap[0]) | (((word64) xsp->ap[1]) << 32);
   __v64_to_real(&d1, &tim);
  }
 else
  {
   /* AIV 09/29/06 - if sign bit on and < WBITS need mask prior to cast */
   if (is_signed) 
    { 
     blen = xsp->xslen;
     mask_val = xsp->ap[0]; 
     if (xsp->xslen != WBITS)
      {
       if ((mask_val & (1 << (blen - 1))) != 0)
        {
         mask_val |= ~(__masktab[blen]);
        }
       }
     i = (int32) mask_val; 
     d1 = (double) i; 
    }
   else { u = (word32) xsp->ap[0]; d1 = (double) u; } 
  }
 /* SJM 09/30/06 - can resue the strength arg */
 xsp->xslen = WBITS;
 return(d1);
}

/*
 * routine to unscale from ticks to user time as double
 * this routine changes value of real
 * only called if know need to scale
 */
extern double __unscale_realticks(word64 *ticksval, struct mod_t *mdp)
{
 word32 unit;
 double d1;

 if (!__v64_to_real(&d1, ticksval))
  {
   __sgfwarn(575,
    "conversion from internal time %s to time as real lost precision",
    __to_timstr(__xs, ticksval));
  }

 if (!mdp->mno_unitcnv)
  {
   unit = __des_timeprec - mdp->mtime_units;
   d1 /= __dbl_toticks_tab[unit];
  }
 return(d1);
}

/*
 * return result of scale function
 * this must return a double value
 * even though this does not return xstk, must leave on expr. stack
 */
extern void __exec_scale(struct expr_t *fax)
{
 int32 unit;
 double d1; 
 word64 tval;
 struct xstk_t *xsp;
 struct mod_t *from_mdp, *to_mdp;
   
 xsp = __eval_xpr(fax);
 if (xsp->xslen != TIMEBITS) __sizchgxs(xsp, TIMEBITS);  
 if (xsp->bp[0] != 0L || xsp->bp[1] != 0)
  {
   __sgfwarn(629, "$scale time argument bits x or z - scaled to 0.0");
   d1 = 0.0;
   goto done;
  }
 tval = ((word64) xsp->ap[0]) | (((word64) xsp->ap[1]) << 32);
 if (!__v64_to_real(&d1, &tval))
  {
   __sgfwarn(575,
    "conversion from $scale time %s to time as real lost precision",
    __to_timstr(__xs, &tval));
  }
 to_mdp = __inst_mod;
 from_mdp = fax->ru.grp->targmdp;
 if (from_mdp->mtime_units != to_mdp->mtime_units)
  {
   if (from_mdp->mtime_units > to_mdp->mtime_units)
    {
     /* here from module more precise (high exponent) - divide */  
     unit = from_mdp->mtime_units - to_mdp->mtime_units;
     d1 /= __dbl_toticks_tab[unit];
    }   
   else
    {
     /* here from module less precise (low exponent) - multiply */  
     unit = to_mdp->mtime_units - from_mdp->mtime_units;
     d1 *= __dbl_toticks_tab[unit];
    }
  }
done:
 memcpy(xsp->ap, &d1, sizeof(double));
}

/*
 * wrapper for cnv stk from real to reg32 when reading source
 * purpose to set up right source line
 */
extern void __src_rd_cnv_stk_fromreal_toreg32(struct xstk_t *xsp)
{
 int32 sav_slin_cnt, sav_sfnam_ind;

 /* can assign specparam value immediately */
 /* SJM 06/17/99 - needs to run in run/fixup mode - statement loc set */
 sav_slin_cnt = __slin_cnt;
 sav_sfnam_ind = __sfnam_ind;
 __sfnam_ind = __cur_fnam_ind;
 __slin_cnt = __lin_cnt;

 __cnv_stk_fromreal_toreg32(xsp);

 /* must put back in case reading iact source */
 __slin_cnt = sav_slin_cnt;
 __sfnam_ind = sav_sfnam_ind;
}

/*
 * convert an evaluated tos value from real to reg 32 (maybe signed) 
 * know real will always be WBITS with b part part of real
 * this routine is 32 bit dependent
 *
 * algorith converts real to int32 (32 bit) then move bit pattern to reg32
 * notice output can be signed
 */
extern void __cnv_stk_fromreal_toreg32(struct xstk_t *xsp)
{
 double d1; 
 int32 i;

 memcpy(&d1, xsp->ap, sizeof(double)); 
 i = ver_rint_(d1);
 xsp->ap[0] = (word32) i;
 xsp->bp[0] = 0L;
}

/*
 * wrapper for cnv stk from reg to realwhen reading source
 * purpose to set up right source line
 */
extern void __src_rd_cnv_stk_fromreg_toreal(struct xstk_t *xsp, int32 src_signed)
{
 int32 sav_slin_cnt, sav_sfnam_ind;

 /* can assign specparam value immediately */
 /* SJM 06/17/99 - needs to run in run/fixup mode - statement loc set */
 sav_slin_cnt = __slin_cnt;
 sav_sfnam_ind = __sfnam_ind;
 __sfnam_ind = __cur_fnam_ind;
 __slin_cnt = __lin_cnt;

 __cnv_stk_fromreg_toreal(xsp, src_signed);

 /* must put back in case reading iact source */
 __slin_cnt = sav_slin_cnt;
 __sfnam_ind = sav_sfnam_ind;
}

/*
 * convert a tos evaluated value from reg to real
 * need run time warning if does not fit - since if high bits 0 ok
 * this is real width and word32 width dependent
 */ 
extern void __cnv_stk_fromreg_toreal(struct xstk_t *xsp, int32 src_signed)
{
 int32 i;
 word32 w;
 double d1;

 if ((xsp->xslen > WBITS && !vval_is0_(&(xsp->ap[1]), xsp->xslen - WBITS)) 
  || !vval_is0_(xsp->bp, xsp->xslen))
  {
   __sgfwarn(518,
    "conversion of %s to real width %d too wide or x/z - converted to 0.0",
    __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE),
    xsp->xslen);
   d1 = 0.0;
   goto done;
  }
 if (src_signed) { i = (int32) xsp->ap[0]; d1 = (double) i;  }
 else { w = xsp->ap[0]; d1 = (double) w; }

 /* next must adjust stack - know will have enough room */
done:
 xsp->bp = &(xsp->ap[1]);
 xsp->xslen = WBITS;
 memcpy(xsp->ap, &d1, sizeof(double));
}

/*
 * ROUTINES TO IMPLEMENT VERILOG SPECIFIC OR UNPORTABLE MATH LIB
 */

#define MY_MAXDOUBLE 1.797693134862315708e+308
/* #define MY_MAXDOUBLE 1.79e+308 */
#define MY_MINDOUBLE 4.9406564584124654e-324
#define MY_LONG_MAX 0x7fffffff
#define MY_LONG_MIN 0x80000000

/*
 * convert nptr to a double - modified verision of lib. strtod
 * If endptr is not nil, endptr contains addr of 1 afer end
 * notice here error EINVAL set even no digits - standard just sets to 0
 */
extern double __my_strtod(char *nptr, char **endptr, int32 *errnum)
{
 register char *s;
 short int sign;
 long int tmpexp;
 int32 got_dot, got_digit, save;
 long int exponent;
 double num;
 char *end;

 if (nptr == NULL) goto noconv;
 s = nptr;
 if (*s == '\0') goto noconv;

 /* skip - know *s is a char from system getc type routinm */
 while (isspace(*s)) s++;
 /* get sign */
 sign = *s == '-' ? -1 : 1;
 if (*s == '-' || *s == '+') s++;

 num = 0.0;
 got_dot = 0;
 got_digit = 0;
 exponent = 0;
 for (;; s++)
  {
   if (isdigit(*s))
    {
     got_digit = 1;
     /* if too many digits for double rep., ignore but need for for exp */
     if (num > MY_MAXDOUBLE * 0.1) exponent++;
      else num = 10.0*num + (*s - '0');
     /* count digits after exponent */
     if (got_dot) exponent--;
    }
   /* indicate dot seen */
   else if (!got_dot && *s == '.') got_dot = 1;
   else break;
  }
 /* must be at least 1 digit */
 if (!got_digit) goto noconv;
 /* get the exp. */
 *errnum = 0;
 if (*s == 'e' || *s == 'E')
  {
   save = *errnum;
   s++;
   tmpexp = my_strtol(s, &end, TRUE, errnum);
   if (*errnum == ERANGE)
    {
     /* overflow certainly means double exp. limit also exceeded */
     if (endptr != NULL) *endptr = end;
     if (tmpexp < 0) goto underflow; else goto overflow;
    }
   /* no exponent (e was end of number) */
   else if (end == s) end = (char *) s - 1;
   errno = save;
   s = end;
   exponent += tmpexp;
  }
 if (endptr != NULL) *endptr = s;
 if (num == 0.0) return(0.0);

 /* multiply int32 part by 10 to exponent as power */ 
 if (exponent < 0)
  { if (num < MY_MINDOUBLE * pow(10.0, (double) -exponent)) goto underflow; }
 else if (exponent > 0)
  { if (num > MY_MAXDOUBLE * pow(10.0, (double) -exponent)) goto overflow; }
 num *= pow(10.0, (double) exponent);
 return(sign*num);

overflow:
 *errnum = ERANGE;
 return(sign*MY_MAXDOUBLE);

underflow:
 if (endptr != NULL) *endptr = nptr;
 *errnum = ERANGE;
 return(0.0);

noconv:
 if (endptr != NULL) *endptr = nptr;
 *errnum = EINVAL;
 return(0.0);
}

/*
 * convert string to word32 long
 */
extern word32 __my_strtoul(char *nptr, char **endptr, int *errnum)
{
 word32 ul;

 ul = (word32) my_strtol(nptr, endptr, FALSE, errnum);
 return(ul);
}

/*
 * convert string at nptr to long int and point endptr to 1 past end
 * notice here error EINVAL set even no digits - standard just sets to 0
 */
static long my_strtol(char *nptr, char **endptr, int32 sign, int32 *errnum)
{
 register char *s;
 register unsigned char c;
 register word32 i;
 int32 negative;
 word32 cutoff;
 word32 cutlim;
 char *save;
 int32 overflow;

 s = nptr;
 /* skip white space */
 while (isspace(*s)) ++s;
 if (*s == '\0') goto noconv;

 /* check for a sign */
 if (*s == '-')
  {
   /* word32 can not have sign prefix - maybe should allow and convert */
   if (!sign) goto noconv;
   negative = 1;
   s++;
  }
 else if (*s == '+') { negative = 0; s++; }
 else negative = 0;

 /* save pointer to test for no digits */
 save = s;
 /* this is word32 max */
 cutoff = ALL1W / (word32) 10;
 cutlim = ALL1W % (word32) 10;
 for (overflow = 0, i = 0, c = *s; c != '\0'; c = *++s)
  {
   if (isdigit(c)) c -= '0'; else break;
   /* check for overflow */
   if (i > cutoff || (i == cutoff && c > cutlim)) overflow = 1;
   else { i *= 10; i += c; }
  }
 /* if no progress, error */
 if (s == save) goto noconv;
 /* store endptr if user passed non nil */
 if (endptr != NULL) *endptr = (char *) s;

 /* check for in word32 but outside long int */
 /* FIXME - what is this supposed to do? */
 if (sign)
  {
   if (i > (negative ? - (word32) MY_LONG_MIN
    : (unsigned long int) MY_LONG_MAX)) overflow = 1;
   if (overflow)
    { *errnum = ERANGE; return(negative ? MY_LONG_MIN : MY_LONG_MAX); }
  }
 *errnum = 0;
 return (negative ? - i : i);

noconv:
 *errnum = EINVAL;
 if (endptr != NULL) *endptr = (char *) nptr;
 return(0L);
}

/*
 * ASCII RECONSTRUCTED SOURCE DUMP ROUTINES
 */

/*
 * not dumping cap charges
 * f must not be nil
 * this requires a current instance (call be in loop for all)
 */
extern void __dmp_mod(FILE *f, struct mod_t *mdp)
{
 char s1[RECLEN];

 if (f == NULL) __arg_terr(__FILE__, __LINE__);
 /* if precision changed dump new */
 if (__cur_units != mdp->mtime_units || __cur_prec != mdp->mtime_prec)
  {
   /* replace with new current and write */
   __cur_units = mdp->mtime_units;
   __cur_prec = mdp->mtime_prec;

   sprintf(s1, "\n`timescale %s / %s", __to_timunitnam(__xs, __cur_units),
    __to_timunitnam(__xs2, __cur_units + __cur_prec));
   __nl_wrap_puts(s1, f);
  }

 __push_wrkitstk(mdp, 0);

 __pv_stlevel = 0;
 __outlinpos = 0;
 __cur_sofs = 0;
 __wrap_putc('\n', f);
 /* 04/01/00 SJM - for Verilog 2000 need to dmp digital attributes */
 if (mdp->mattrs != NULL) { dmp_dig_attr_list(f, mdp->mattrs, TRUE); }
 __wrap_puts("module ", f);
 __wrap_puts(mdp->msym->synam, f);

 if (mdp->mod_lofp_decl) dmp_mod_lofp_hdr(f, mdp);
 else dmp_modports(f, mdp);

 /* know I/O port decls will come first */
 dmp_decls(f, mdp);

 dmp_paramdecls(f, mdp->mprms, mdp->mprmnum, "parameter");
 if (mdp->mprms != NULL) { __wrap_putc('\n', f); __outlinpos = 0; }

 /* AIV 09/27/06 - must dump local param declarations too */
 dmp_paramdecls(f, mdp->mlocprms, mdp->mlocprmnum, "localparam");
 if (mdp->mlocprms != NULL) { __wrap_putc('\n', f); __outlinpos = 0; }

 dmp_defparams(f, mdp);

 dmp_mdtasks(f, mdp);

 if (mdp->mtasks != NULL) { __wrap_putc('\n', f); __outlinpos = 0; }
 dbg_dmp_insts(f, mdp);

 if (mdp->mcells != NULL && __outlinpos != 0)
  { __wrap_putc('\n', f); __outlinpos = 0; }
 dmp_ialst(f, mdp);
 if (mdp->ialst != NULL) { __wrap_putc('\n', f); __outlinpos = 0; }


 if (mdp->mspfy != NULL)
  {
   dmp_mdspfy(f, mdp);
   if (__outlinpos != 0) { __wrap_putc('\n', f); __outlinpos = 0; }
  }
 __nl_wrap_puts("endmodule", f);

 __pv_stlevel = 0;
 if (__debug_flg)
  {
   /* if regression dumping first symbol tables and global tables */
   dmp_mod_grefs(f, mdp);
  }
 __cur_sofs = 0;
 __pop_wrkitstk();
}

/*
 * routine to dump an attribute_instance
 */
static void dmp_dig_attr_list(FILE *f, struct attr_t *attr_hd, int32 nd_nl)
{
 register struct attr_t *attrp;
 int32 first_time = TRUE;

 if (nd_nl && __outlinpos != 0) __nl_wrap_puts("", f);
 __wrap_puts("(* ", f);
 for (attrp = attr_hd; attrp != NULL; attrp = attrp->attrnxt)
  {
   if (first_time) first_time = FALSE; else __wrap_puts(", ", f);
   __wrap_puts(attrp->attrnam, f);
   if (attrp->attr_xp != NULL)
    {
     __wrap_puts(" = ", f);
     dmp_expr(f, attrp->attr_xp);
    }
  }
 __wrap_puts(" *)", f);
 if (nd_nl && __outlinpos != 0) __nl_wrap_puts("", f);
 else __wrap_puts(" ", f);
}

/*
 * dump module ports
 */
static void dmp_modports(FILE *f, struct mod_t *mdp)
{
 register int32 pi;
 register struct mod_pin_t *mpp;
 int32 first_time, pnum;

 first_time = TRUE;
 __pv_stlevel++;
 if ((pnum = mdp->mpnum) != 0)
  {
   for (pi = 0, mpp = &(mdp->mpins[0]); pi < pnum; pi++, mpp++)
    {
     if (first_time) { __wrap_putc('(', f); first_time = FALSE; }
     else __wrap_puts(", ", f);
     if (mpp->mp_explicit)
      {
       /* un-named but explicit module port stored wrong */
       /* DBG remove ---
       if (mpp->mpsnam == NULL)
        __misc_gfterr( __FILE__, __LINE__, mdp->msym->syfnam_ind,
         mdp->msym->sylin_cnt);
       --- */
       __wrap_putc('.', f);
       __wrap_puts(mpp->mpsnam, f);
       __wrap_putc('(', f);
       dmp_expr(f, mpp->mpref);
       /* this makes sure (..) always on 1 line */
      __wrap_putc(')', f);
      }
     else dmp_expr(f, mpp->mpref);
    }
  }
 if (!first_time) __wrap_putc(')', f);
 __nl_wrap_puts(";", f);
 __pv_stlevel--;
}

/*
 * dump new list of ports module ports header
 */
static void dmp_mod_lofp_hdr(FILE *f, struct mod_t *mdp)
{
 register int32 pi;
 register struct mod_pin_t *mpp;
 int32 first_time, pnum;
 struct net_t *np;
 char s1[RECLEN];

 first_time = TRUE;
 __pv_stlevel++;
 if ((pnum = mdp->mpnum) == 0)
  {
   if (!first_time) __wrap_putc(')', f);
   __nl_wrap_puts(";", f);
   __pv_stlevel--;
  }

 for (pi = 0, mpp = &(mdp->mpins[0]); pi < pnum; pi++, mpp++)
  {
   if (first_time) { __wrap_putc('(', f); first_time = FALSE; }
   else __wrap_puts(", ", f);

   /* DBG remove -- */
   if (mpp->mpref->optyp != ID) __misc_terr(__FILE__, __LINE__);
   /* --- */

   np = mpp->mpref->lu.sy->el.enp;

   if (np->nattrs != NULL) dmp_dig_attr_list(f, np->nattrs, FALSE);

   __wrap_puts(__to_ptnam(s1, np->iotyp), f);

   if (np->ntyp != N_WIRE)
    {
     __wrap_putc(' ', f);
     __wrap_puts(__to_wtnam(s1, np), f);
    }

   if (np->n_signed && (np->ntyp != N_INT && np->ntyp != N_REAL
    && np->ntyp != N_TIME))
    {
     __wrap_puts(" signed", f); 
    }

   /* for special types no range but will have internal range */
   if (!np->n_isavec || np->ntyp == N_INT || np->ntyp == N_TIME
    || np->ntyp == N_REAL) strcpy(s1, "");
   else { __to_wrange(s1, np); __wrap_putc(' ', f); __wrap_puts(s1, f); }

   __wrap_putc(' ', f);
   __wrap_puts(np->nsym->synam, f);
  }
 if (!first_time) __wrap_putc(')', f);
 __nl_wrap_puts(";", f);
 __pv_stlevel--;
}

/* 
 * dump all i/o and wire declarations for module
 */
static void dmp_decls(FILE *f, struct mod_t *mdp)
{
 register struct net_t *np;
 register int32 i;
 int32 pnum;
 struct mod_pin_t *mpp;

 __pv_stlevel++;
 /* first mark all wires not emitted */
 if (mdp->mnnum != 0)
  { 
   for (i = 0, np = &(mdp->mnets[0]); i < mdp->mnnum; np++, i++)
    np->n_mark = FALSE;
  }

 if ((pnum = mdp->mpnum) != 0 && !mdp->mod_lofp_decl)
  {
   for (i = 0, mpp = &(mdp->mpins[0]); i < pnum; i++, mpp++)
    dmp_1portdecl(f, mpp->mpref);
  }
 if (mdp->mnnum != 0)
  {
   /* if use ptr must be sure at least one */
   /* do not emit decl. for any added nets */
   for (i = 0, np = &(mdp->mnets[0]); i < mdp->mnnum; i++, np++)
    dmp_1netdecl(f, np);
  }
 __outlinpos = 0;
 __pv_stlevel--;
}

/*
 * dump the required I/O direction declaration for one port net
 * must declare every wire in expr. as I/O port
 */
static void dmp_1portdecl(FILE *f, struct expr_t *ndp)
{
 register struct expr_t *catndp;
 struct net_t *np;
 char s1[RECLEN];

 switch ((byte) ndp->optyp) {
  case ID:
   np = ndp->lu.sy->el.enp;
emit_decl:
   /* just a mark here */

   if (!np->n_mark)
    {
     __wrap_puts(__to_ptnam(s1, np->iotyp), f);
     __wrap_putc(' ', f);
     __wrap_puts(np->nsym->synam, f);
     __nl_wrap_puts(";", f);
    }
   np->n_mark = TRUE;
   break;
  case LSB:
  case PARTSEL:
   np = ndp->lu.x->lu.sy->el.enp;
   goto emit_decl;
  case LCB:
   {
    for (catndp = ndp->ru.x; catndp != NULL; catndp = catndp->ru.x)
     dmp_1portdecl(f, catndp->lu.x);
   }
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * dump 1 net's declarations - may need I/O and wire
 */
static void dmp_1netdecl(FILE *f, struct net_t *np)
{
 char wnam[RECLEN], s1[IDLEN], s2[RECLEN];

 __outlinpos = 0;
 strcpy(wnam, "");
 /* if I/O port is normal wire, do not need wire declaration */
 if (np->iotyp != NON_IO)
  {
   if (__inst_mod->mod_lofp_decl) return;
   if (!nd_iowirdecl(np)) return;
  }
 
 /* 04/01/00 SJM - for Verilog 2000 need to dmp digital attributes */
 if (np->nattrs != NULL) dmp_dig_attr_list(f, np->nattrs, FALSE);

 __wrap_puts(__to_wtnam(wnam, np), f);
 /* know wire type will be trireg for cap. strength here */
 if (np->n_capsiz != CAP_NONE)
  {
   __wrap_putc(' ', f);
   __wrap_puts(__to_stren_nam(s1, __fr_cap_size((int32) np->n_capsiz),
    ST_STRONG), f);
  }
 /* never emit scalared/vectored for reg */
 if (np->n_isavec && np->ntyp < NONWIRE_ST)
  {
   if (np->nrngrep == NX_CT)
    {
     if (np->nu.ct->n_spltstate == SPLT_SCAL) __wrap_puts(" scalared", f);
     else if (np->nu.ct->n_spltstate == SPLT_VECT)
      __wrap_puts(" vectored", f);
    }
   else { if (!np->vec_scalared) __wrap_puts(" vectored", f); }
  }

 /* SJM 10/06/03 - signed if present after vec/scalar */
 if (np->n_signed && (np->ntyp != N_INT && np->ntyp != N_REAL
  && np->ntyp != N_TIME))
  {
   __wrap_puts(" signed", f); 
  }

 /* for special types no range but will have internal range */
 if (!np->n_isavec || np->ntyp == N_INT || np->ntyp == N_TIME
  || np->ntyp == N_REAL) strcpy(s1, "");
 else { __to_wrange(s1, np); __wrap_putc(' ', f); __wrap_puts(s1, f); }

 /* these only write something if delay present */
 if (np->nrngrep == NX_CT) dmp_delay(f, np->nu.ct->n_dels_u,
  (word32) DT_CMPLST, "#");
 else if (np->nrngrep == NX_DWIR)
  dmp_delay(f, np->nu.rngdwir->n_du, np->nu.rngdwir->n_delrep, "#");

 __wrap_putc(' ', f);
 __wrap_puts(np->nsym->synam, f);
 if (np->n_isarr) __wrap_puts(__to_arr_range(s2, np), f);
 __nl_wrap_puts(";", f);

}

/*
 * return T if need wire decl. for I/O port
 * know wire only trireg wire type can have strength and need decl.
 *
 * default wire type in Verilog already handled because undeclared wires
 * will be assigned default wire type
 */
static int32 nd_iowirdecl(struct net_t *np)
{
 if (np->n_isavec || np->n_isarr || np->nattrs != NULL) return(TRUE);

 /* SJM 10/06/03 - if signed but not integer/real/time, need signed decl */ 
 if (np->n_signed && (np->ntyp != N_INT && np->ntyp != N_REAL
  && np->ntyp != N_TIME)) return(TRUE);

 if (np->nrngrep == NX_CT)
  {
   /* scalared is the default and impossible for regs */
   if (np->nu.ct->n_spltstate == SPLT_VECT) return(TRUE);
   if (np->nu.ct->n_dels_u.pdels != NULL) return(TRUE);
  }
 else
  {
   if (!np->vec_scalared) return(TRUE);
   if (np->nrngrep == NX_DWIR && np->nu.rngdwir->n_delrep != DT_PTHDST)
    return(TRUE);
  }
 if (np->ntyp != N_WIRE) return(TRUE);
 return(FALSE);
}

/*
 * dump locally declared param declarations
 * this does not work if f == NULL - does nothing
 */
static void dmp_paramdecls(FILE *f, struct net_t *parm_nptab, int32 pnum,
 char *pclassnam)
{
 register int32 pi;
 int32 varwid, base;
 struct net_t *parm_np;
 struct xstk_t *xsp;
 char s1[RECLEN], s2[RECLEN];

 __pv_stlevel++;
 for (pi = 0; pi < pnum; pi++)
  {
   parm_np = &(parm_nptab[pi]);

   __wrap_puts(pclassnam, f);
   /* FIXME - should emit according to exact source */
   if (parm_np->nu.ct->ptypdecl || parm_np->nu.ct->prngdecl)
    {
     /* always emit vector declaration even if not in source */
     if (parm_np->nu.ct->ptypdecl)
      {
       /* SJM 10/06/03 - know signed keyword can't have been used for these */
       if (parm_np->ntyp == N_REAL) strcpy(s1, " real");
       else if (parm_np->ntyp == N_INT) strcpy(s1, " integer");
       else if (parm_np->ntyp == N_TIME) strcpy(s1, " time");
       else __case_terr(__FILE__, __LINE__);
       __wrap_puts(s1, f);
      }
     else if (parm_np->nu.ct->prngdecl)
      {
       /* SJM 10/06/03 - signed and range can be declared together */ 
       if (parm_np->nu.ct->psigndecl) __wrap_puts(" signed", f);
       if (parm_np->n_isavec)
        {
         __wrap_putc(' ', f);
         __to_wrange(s1, parm_np);
         __wrap_puts(s1, f);
        }
      } 
    }
   /* SJM 10/06/03 - signed without range possible - rhs range used */
   else if (parm_np->nu.ct->psigndecl) __wrap_puts(" signed", f);

   __wrap_putc(' ', f);
   __wrap_puts(parm_np->nsym->synam, f);
   /* for specify never array and only array if decled */
   if (parm_np->n_isarr)
    {
     __wrap_puts(__to_arr_range(s2, parm_np), f);
     __wrap_puts(" = ", f);
     dmp_expr(f, parm_np->nu.ct->n_dels_u.d1x);
     __nl_wrap_puts(";", f);
     continue;
    }

   __wrap_puts(" = ", f);
   /* should probably dump wire range here */
   varwid = __get_netwide(parm_np);
   push_xstk_(xsp, varwid);
   __ld_wire_val(xsp->ap, xsp->bp, parm_np);
   
   /* since need to parse this if x/z, need [size]'b form */
   if (parm_np->nu.ct->pstring)
    {
     __xline_vval_to_cstr(xsp->ap, xsp->xslen, TRUE, TRUE, FALSE);
    }
   else
    {
     base = parm_np->nu.ct->pbase;
     __regab_disp(xsp->ap, xsp->bp, xsp->xslen, base, TRUE,
      (parm_np->n_signed == 1));
    }
   __pop_xstk();
   __wrap_puts(__exprline, f);

   __nl_wrap_puts(";", f);
   /* know f not nil or will not get here */
   __cur_sofs = 0;
  }
 __pv_stlevel--;
}


/*
 * dump defparam statements (module items)
 * if output time becomes a problem could build index since in order
 */
static void dmp_defparams(FILE *f, struct mod_t *mdp)
{
 register int32 i;
 struct dfparam_t *dfpp;
 struct itree_t *itp;

 __pv_stlevel++;
 for (dfpp = __dfphdr; dfpp != NULL; dfpp = dfpp->dfpnxt)
  {
   if (dfpp->in_mdp != mdp) continue;
   __wrap_puts("defparam ", f);
   if (dfpp->dfp_local) __wrap_puts(dfpp->gdfpnam, f);
   else
    {
     /* this prints the after splitting form */
     /* always printed rooted but source may have been downward */
     itp = __it_roots[dfpp->dfpiis[0]];
     for (i = 0;;)
      {
       if (i > 0) __wrap_putc('.', f);
       /* must be on one line no matter how long */
       __wrap_puts(itp->itip->isym->synam, f);
       if (++i > dfpp->last_dfpi) break;
       itp = &(itp->in_its[dfpp->dfpiis[i]]);
      }
     if (i > 0) __wrap_putc('.', f);
     __wrap_puts(dfpp->targsyp->synam, f);
     /* must be on one line no matter how long */
    }
   __wrap_puts(" = ", f);
   dmp_expr(f, dfpp->dfpxrhs);
   __nl_wrap_puts(";", f);
  }
 __pv_stlevel--;
}

/*
 * dump tasks and functions - named blocks dumped inline
 */
static void dmp_mdtasks(FILE *f, struct mod_t *mdp)
{
 register struct task_t *tskp;

 for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
  {
   if (tskp->tsktyp == FUNCTION || tskp->tsktyp == TASK) dmp_task(f, tskp);
  }
}

/*
 * dump module instances and gates
 * notice by here cells gone and split into insts and gates
 *
 * normal version that dumps g/i arrays as vectors
 */
static void dmp_insts(FILE *f, struct mod_t *mdp)
{
 register int32 ii, gi, cai;
 register struct giarr_t *giap;
 struct gate_t *gp;
 register struct conta_t *cap;
 struct inst_t *ip;

 __outlinpos = 0;
 __pv_stlevel++;
 for (ii = 0; ii < mdp->minum;)
  {
   if (mdp->miarr != NULL && (giap = mdp->miarr[ii]) != NULL)
    {
     ip = &(mdp->minsts[ii]);
     dmp_1inst(f, ip, giap);
     ii += __get_giarr_wide(giap);
    }
   else
    {
     dmp_1inst(f, &(mdp->minsts[ii]), NULL);
     ii++;
    }
  }
 for (gi = 0; gi < mdp->mgnum;)
  {
   if (mdp->mgarr != NULL && (giap = mdp->mgarr[gi]) != NULL)
    {
     gp = &(mdp->mgates[gi]);
     dmp_1gate(f, gp, giap);
     gi += __get_giarr_wide(giap);
    }
   else
    {
     dmp_1gate(f, &(mdp->mgates[gi]), NULL);
     gi++;
    } 
  }
 for (cap = &(mdp->mcas[0]), cai = 0; cai < mdp->mcanum; cai++, cap++)
  dmp_1conta(f, cap);
 __pv_stlevel--;
}


/*
 * dump 1 module instance
 */
static void dmp_1inst(FILE *f, struct inst_t *ip, struct giarr_t *giap)
{
 struct mod_t *imdp;
 char s1[RECLEN];

 __wrap_puts(ip->imsym->synam, f);
 /* this writes nothing if no # style passed set defparams */
 imdp = ip->imsym->el.emdp;
 if (ip->ipxprtab != NULL) dmp_pnd_params(f, ip, imdp);

 /* know modules always named */
 __wrap_putc(' ', f);
 __wrap_puts(ip->isym->synam, f);

 /* if giap non nil - emit range */
 if (giap != NULL)
  {
   if (__debug_flg)
    {
     sprintf(s1, " [%d:%d]", giap->gia1, giap->gia2);
     __wrap_puts(s1, f);
    }
   else
    {
     __wrap_puts(" [", f);
     dmp_expr(f, giap->giax1);
     __wrap_putc(':', f);
     dmp_expr(f, giap->giax2);
     __wrap_putc(']', f);
    }
   dmp_iports(f, ip, giap->giapins);
  }
 else dmp_iports(f, ip, ip->ipins);
}

/*
 * dump instance pound params
 */
static void dmp_pnd_params(FILE *f, struct inst_t *ip, struct mod_t *imdp)
{
 register int32 pi;
 int32 first_time;
 struct expr_t *pxp;
 struct net_t *modnp;

 __force_base = BDEC;
 __wrap_puts(" #(", f);
 if (impl_pndparams(ip, imdp))
  {
   for (first_time = TRUE, pi = 0; pi < imdp->mprmnum; pi++) 
    {
     pxp = ip->ipxprtab[pi];
     /* this handles missing tail params */
     if (pxp == NULL) break;
     modnp = &(imdp->mprms[pi]);

     if (first_time) first_time = FALSE; else __wrap_puts(", ", f);
     if (__debug_flg)
      {
       int32 wid, base;
       struct xstk_t *xsp;
       char s1[RECLEN];
   
       wid = __get_netwide(modnp);
       push_xstk_(xsp, wid);
       __ld_wire_val(xsp->ap, xsp->bp, modnp);

       /* since need to parse this if x/z, need [size]'b form */
       if (modnp->nu.ct->pstring)
        {
         __xline_vval_to_cstr(xsp->ap, xsp->xslen, TRUE, TRUE, FALSE);
        }
       else
        {
         base = modnp->nu.ct->pbase;
         if (base == BHEX)
          { sprintf(s1, "%d'h", xsp->xslen); __wrap_puts(s1, f); }
         __regab_disp(xsp->ap, xsp->bp, xsp->xslen, base, TRUE,
          (modnp->n_signed == 1));
        }
       __pop_xstk();
       __wrap_puts(__exprline, f);
       /* know f not nil or will not get here */
       __cur_sofs = 0;
      }
     else dmp_expr(f, pxp);
    }
  }
 else
  {
   for (first_time = TRUE, pi = 0; pi < imdp->mprmnum; pi++) 
    {
     pxp = ip->ipxprtab[pi];
     /* this handles missing embedded or tail */
     if (pxp == NULL) continue;
     modnp = &(imdp->mprms[pi]);

     if (first_time) first_time = FALSE; else __wrap_puts(", ", f);
     __wrap_putc('.', f);
     __wrap_puts(modnp->nsym->synam, f);
     __wrap_putc('(', f);
     if (__debug_flg)
      {
       int32 wid, base;
       struct xstk_t *xsp;
       char s1[RECLEN];
   
       wid = __get_netwide(modnp);
       push_xstk_(xsp, wid);
       __ld_wire_val(xsp->ap, xsp->bp, modnp);

       /* since need to parse this if x/z, need [size]'b form */
       if (modnp->nu.ct->pstring)
        {
         __xline_vval_to_cstr(xsp->ap, xsp->xslen, TRUE, TRUE, FALSE);
        }
       else
        {
         base = modnp->nu.ct->pbase;
         if (base == BHEX)
          { sprintf(s1, "%d'h", xsp->xslen); __wrap_puts(s1, f); }
         __regab_disp(xsp->ap, xsp->bp, xsp->xslen, base, TRUE,
          (modnp->n_signed == 1));
        }
       __pop_xstk();
       __wrap_puts(__exprline, f);
       /* know f not nil or will not get here */
       __cur_sofs = 0;
      }
     else dmp_expr(f, pxp);
     __wrap_putc(')', f);
    }
  }
 __wrap_putc(')', f);
 __force_base = BNONE;
}

/*
 * return T if can use implicit form for instance pound params 
 *
 * only called if at least 1 pound param
 * either all present in ip expr table or only tail missing
 */
static int32 impl_pndparams(struct inst_t *ip, struct mod_t *imdp)
{
 register int32 pi, pi2;
 struct expr_t *pxp;

 imdp = ip->imsym->el.emdp; 
 for (pi2 = imdp->mprmnum - 1; pi2 >= 0; pi2--)
  {
   pxp = ip->ipxprtab[pi2];
   if (pxp != NULL) break;
  }  
 /* DBG remove --- */
 if (pi2 < 0) __misc_terr(__FILE__, __LINE__);
 /* --- */
 for (pi = 0; pi < pi2; pi++)
  {
   pxp = ip->ipxprtab[pi];
   /* once missing tail sequence eliminated, if any missing need explicit */
   if (pxp == NULL) return(FALSE); 
  }
 return(TRUE);
}

/*
 * dump module instances and gates
 * unused DBG version that dump each bit
 */
/* DBG --- */ 
static void dbg_dmp_insts(FILE *f, struct mod_t *mdp)
{
 register int32 ii, gi, cai;
 register struct conta_t *cap;
 int32 slen;
 struct gate_t *gp;
 struct inst_t *ip;
 char *chp, s1[IDLEN];

 __outlinpos = 0;
 __pv_stlevel++;
 for (ii = 0; ii < mdp->minum; ii++)
  {
   ip = &(mdp->minsts[ii]);
   /* 04/01/00 SJM - for Verilog 2000 need to dmp digital attributes */
   if (ip->iattrs != NULL) dmp_dig_attr_list(f, ip->iattrs, FALSE);
   if (mdp->miarr != NULL && mdp->miarr[ii] != NULL)
    {
     strcpy(s1, ip->isym->synam);
     if ((chp = strrchr(s1, '[')) == NULL) __arg_terr(__FILE__, __LINE__);
     *chp = '$'; 
     slen = strlen(s1);
     s1[slen - 1] = '$';
    }
   else strcpy(s1, ip->isym->synam);
   dbg_dmp_1inst(f, ip, s1);
  }
 for (gi = 0; gi < mdp->mgnum; gi++)
  {
   gp = &(mdp->mgates[gi]);
   /* 04/01/00 SJM - for Verilog 2000 need to dmp digital attributes */
   if (gp->gattrs != NULL) dmp_dig_attr_list(f, gp->gattrs, FALSE);

   if (mdp->mgarr != NULL && mdp->mgarr[gi] != NULL)
    {
     strcpy(s1, gp->gsym->synam);
     if ((chp = strrchr(s1, '[')) == NULL) __arg_terr(__FILE__, __LINE__);
     *chp = '$'; 
     slen = strlen(s1);
     s1[slen - 1] = '$';
    }
   else strcpy(s1, gp->gsym->synam);

   dbg_dmp_1gate(f, gp, s1);
  }
 for (cap = &(mdp->mcas[0]), cai = 0; cai < mdp->mcanum; cai++, cap++)
  dmp_1conta(f, cap);
 __pv_stlevel--;
}

/* --- */

/*
 * dump 1 module instance
 */
/* DBG --- */ 
static void dbg_dmp_1inst(FILE *f, struct inst_t *ip, char *inam)
{
 struct mod_t *imdp;

 __wrap_puts(ip->imsym->synam, f);
 imdp = ip->imsym->el.emdp;
 if (ip->ipxprtab != NULL) dmp_pnd_params(f, ip, imdp);

 __wrap_putc(' ', f);
 __wrap_puts(inam, f);
 dmp_iports(f, ip, ip->ipins);
}
/* --- */

/*
 * dump an instance port connection list
 */
static void dmp_iports(FILE *f, struct inst_t *ip, struct expr_t **iptab)
{
 register int32 pi;
 register struct mod_pin_t *mpp;
 int32 first_time, pnum;
 struct expr_t *cxp;
 struct mod_t *mdp;

 __wrap_putc('(', f);
 first_time = TRUE;
 mdp = ip->imsym->el.emdp;
 if ((pnum = mdp->mpnum) == 0) { __nl_wrap_puts(");", f); return; }

 mpp = &(mdp->mpins[0]);
 if (ip->ip_explicit)
  {
   for (pi = 0; pi < pnum; pi++, mpp++)
    {
     if (first_time) first_time = FALSE; else __wrap_puts(", ", f);
     /* even if explicit, if error and no info make implicit */
     cxp = iptab[pi];
     if (mpp->mpsnam != NULL)
      {
       __wrap_putc('.', f); 
       __wrap_puts(mpp->mpsnam, f);
       __wrap_putc('(', f); 
       dmp_expr(f, cxp);
       __wrap_putc(')', f);
      }
     else dmp_expr(f, cxp);
    }
  }
 else
  {
   for (pi = 0; pi < pnum; pi++)
    {
     if (first_time) first_time = FALSE; else __wrap_puts(", ", f);
     /* even if explicit, if error and no info make implicit */
     cxp = iptab[pi];
     dmp_expr(f, cxp);
    }
  }
 __nl_wrap_puts(");", f);
}

/*
 * dump 1 gate - source dumping after output net/bit setting in v_fx
 */
static void dmp_1gate(FILE *f, struct gate_t *gp, struct giarr_t *giap)
{
 register int32 pi;
 int32 gid, first_time;
 char s1[RECLEN];

 /* 1 bit continous assign simulated as gate must be dumped as ca */
 if (gp->g_class == GC_LOGIC)
  {
   gid = gp->gmsym->el.eprimp->gateid;
   if (gid == G_ASSIGN) { dmp_1bitconta(f, gp); return; }
  }

 /* normal primitive */
 __wrap_puts(gp->gmsym->synam, f);
 /* know by here if strength on mod. inst. will have emitted error */
 if (gp->g_hasst)
  {
   __wrap_putc(' ', f);
   __wrap_puts(__to_stval_nam(s1, gp->g_stval), f);
  }
 dmp_delay(f, gp->g_du, gp->g_delrep, "#");

 /* if unnamed (system generated name) do not emit inst. name */
 if (!gp->g_unam)
  {
   __wrap_putc(' ', f);
   __wrap_puts(gp->gsym->synam, f);
  }

 if (giap != NULL)
  {
   if (__debug_flg)
    {
     sprintf(s1, " [%d:%d]", giap->gia1, giap->gia2);
     __wrap_puts(s1, f);
    }
   else
    {
     __wrap_puts(" [", f);
     dmp_expr(f, giap->giax1);
     __wrap_putc(':', f);
     dmp_expr(f, giap->giax2);
     __wrap_putc(']', f);
    }
  }
 __wrap_putc('(', f);
 for (first_time = TRUE, pi = 0; pi < (int32) gp->gpnum; pi++)
  {
   if (first_time) first_time = FALSE; else __wrap_puts(", ", f);
   if (giap != NULL) dmp_expr(f, giap->giapins[pi]);
   else dmp_expr(f, gp->gpins[pi]);
  }
 __nl_wrap_puts(");", f);
}

/*
 * dbg dump 1 gate - source dumping after output net/bit setting in v_fx
 */
/* DBG --- */
static void dbg_dmp_1gate(FILE *f, struct gate_t *gp, char *gnam)
{
 register int32 pi;
 int32 gid, first_time;
 char s1[RECLEN];

 if (gp->g_class == GC_LOGIC)
  {
   gid = gp->gmsym->el.eprimp->gateid;
   if (gid == G_ASSIGN) { dmp_1bitconta(f, gp); return; }
  }

 __wrap_puts(gp->gmsym->synam, f);
 if (gp->g_hasst)
  {
   __wrap_putc(' ', f);
   __wrap_puts(__to_stval_nam(s1, gp->g_stval), f);
  }
 dmp_delay(f, gp->g_du, gp->g_delrep, "#");

 if (!gp->g_unam)
  {
   __wrap_putc(' ', f);
   __wrap_puts(gnam, f);
  }
 __wrap_putc('(', f);
 for (first_time = TRUE, pi = 0; pi < (int32) gp->gpnum; pi++)
  {
   if (first_time) first_time = FALSE; else __wrap_puts(", ", f);
   /* this dumps post expanded pins */
   dmp_expr(f, gp->gpins[pi]);
  }
 __nl_wrap_puts(");", f);
}
/* --- */

/*
 * dump 1 cont. assign
 */
static void dmp_1conta(FILE *f, struct conta_t *cap)
{
 char s1[RECLEN];

 /* continous assign:  assign #(delay) lhs = rhs; */
 __wrap_puts("assign", f);
 if (cap->ca_hasst)
  {
   __wrap_putc(' ', f);
   __wrap_puts(__to_stval_nam(s1, cap->ca_stval), f);
  }
 dmp_delay(f, cap->ca_du, cap->ca_delrep, "#");
 __wrap_putc(' ', f);

 dmp_expr(f, cap->lhsx);
 __wrap_puts(" = ", f);
 dmp_expr(f, cap->rhsx);
 __nl_wrap_puts(";", f);
}

/*
 * dump a 1 bit continuous assign strored as a gate
 */
static void dmp_1bitconta(FILE *f, struct gate_t *gp)
{
 char s1[RECLEN];

 /* continous assign:  assign #(delay) lhs = rhs; */
 __wrap_puts("assign", f);
 if (gp->g_hasst)
  { __wrap_putc(' ', f); __wrap_puts(__to_stval_nam(s1, gp->g_stval), f); }
 dmp_delay(f, gp->g_du, gp->g_delrep, "#");
 __wrap_putc(' ', f);
 dmp_expr(f, gp->gpins[0]);
 __wrap_puts(" = ", f);
 dmp_expr(f, gp->gpins[1]);
 __nl_wrap_puts(";", f);
}

/*
 * routine to dump all initial and always statements
 */
static void dmp_ialst(FILE *f, struct mod_t *mdp)
{
 struct ialst_t *ialp;

 for (ialp = mdp->ialst; ialp != NULL; ialp = ialp->ialnxt)
  {
   if (__outlinpos != 0) __nl_wrap_puts("", f);
   __pv_stlevel++;
   if (ialp->iatyp == ALWAYS) __wrap_puts("always ", f);
   else __wrap_puts("initial ", f);
   dmp_lstofsts(f, ialp->iastp);
   __pv_stlevel--;
  }
}

/*
 * routine to reconstruct and write one statement
 * this routine can take nill to build entire statement in string
 *
 * notice this cannot be called with f== nil
 */
extern void __dmp_stmt(FILE *f, struct st_t *stp, int32 nd_nl)
{
 struct st_t *stp2;
 struct for_t *frs;
 struct task_t *tskp;
 struct st_t *gtstp;

 if (f == NULL) __arg_terr(__FILE__, __LINE__);
 if (nd_nl == NL && __outlinpos != 0) __nl_wrap_puts("", f);
 switch ((byte) stp->stmttyp) {
  case S_NULL:
   __wrap_putc(';', f);
   if (__debug_flg) __wrap_puts("** S_NULL **", f);
   break;
  case S_STNONE:
   /* none is place holder for empty block, emit nothing */
   if (__debug_flg) __wrap_puts("** S_STNONE **", f);
   break;
  case S_PROCA: case S_FORASSGN: case S_RHSDEPROCA:
   __dmp_proc_assgn(f, stp, (struct delctrl_t *) NULL, FALSE);
   break;
  case S_NBPROCA:
   __dmp_nbproc_assgn(f, stp, (struct delctrl_t *) NULL);
   break;
  case S_IF:
   __wrap_puts("if (", f);
   dmp_expr(f, stp->st.sif.condx);
   __wrap_puts(") ", f);
   dmp_lstofsts(f, stp->st.sif.thenst);
   if (stp->st.sif.elsest != NULL)
    {
     if (f != NULL && __outlinpos != 0) __nl_wrap_puts("", f); 
     __wrap_puts("else ", f);
     dmp_lstofsts(f, stp->st.sif.elsest);
    }
   break;
  case S_CASE:
   dmp_case(f, stp);
   break;
  case S_FOREVER:
   __wrap_puts("forever ", f);
   dmp_lstofsts(f, stp->st.swh.lpst);
   break;
  case S_REPEAT:
   __wrap_puts("repeat (", f);
   dmp_expr(f, stp->st.srpt.repx);
   __wrap_puts(") ", f);
   dmp_lstofsts(f, stp->st.srpt.repst);
   break;
  case S_WHILE:
   __wrap_puts("while (", f);
   dmp_expr(f, stp->st.swh.lpx);
   __wrap_puts(") ", f);
   dmp_lstofsts(f, stp->st.swh.lpst);
   break;
  case S_WAIT:
   __wrap_puts("wait (", f);
   dmp_expr(f, stp->st.swait.lpx);
   __wrap_puts(") ", f);
   dmp_lstofsts(f, stp->st.swait.lpst);
   break;
  case S_FOR:
   frs = stp->st.sfor;
   __dmp_forhdr(f, frs);
   dmp_lstofsts(f, frs->forbody);
   break;
  case S_DELCTRL:
   __dmp_dctrl(f, stp->st.sdc);
   break;
  case S_NAMBLK:
   /* indent block and statements within */
   __pv_stlevel++;
   tskp = stp->st.snbtsk;
   dmp_task(f, tskp);
   __pv_stlevel--;
   break;
  case S_UNBLK:
   if (f != NULL && nd_nl == NONL && __outlinpos != 0) __nl_wrap_puts("", f);
   __pv_stlevel++;
   if (f != NULL) __nl_wrap_puts("begin", f);
   __pv_stlevel++;
   for (stp2 = stp->st.sbsts; stp2 != NULL; stp2 = stp2->stnxt) 
    __dmp_stmt(f, stp2, NL);
   __pv_stlevel--;
   if (f != NULL && __outlinpos != 0) __nl_wrap_puts("", f);
   __nl_wrap_puts("end", f);
   __pv_stlevel--;
   break;
  case S_UNFJ:
   /* only place unnamed blocks (not statements lists) is fork-join */  
   if (f != NULL && nd_nl == NONL && __outlinpos != 0) __nl_wrap_puts("", f);
   __pv_stlevel++;
   if (f != NULL) __nl_wrap_puts("fork", f);
   __pv_stlevel++;
   dmp_fj_stlst(f, stp);
   __pv_stlevel--;
   if (f != NULL && __outlinpos != 0) __nl_wrap_puts("", f);
   __nl_wrap_puts("join", f);
   __pv_stlevel--;
   break;
  case S_TSKCALL:
   __dmp_tskcall(f, stp);
   break;
  case S_QCONTA:
   if (stp->st.sqca->qcatyp == ASSIGN) __wrap_puts("assign ", f);
   else __wrap_puts("force ", f);
   dmp_expr(f, stp->st.sqca->qclhsx);
   __wrap_puts(" = ", f);
   dmp_expr(f, stp->st.sqca->qcrhsx);
   __wrap_putc(';', f);
   break;
  case S_QCONTDEA:
   if (stp->st.sqcdea.qcdatyp == DEASSIGN) __wrap_puts("deassign ", f);
   else __wrap_puts("release ", f);
   dmp_expr(f, stp->st.sqcdea.qcdalhs);
   __wrap_putc(';', f);
   break;
  case S_CAUSE:
   __wrap_puts("->", f);
   /* this will print global ref. if needed */
   dmp_expr(f, stp->st.scausx);
   __wrap_putc(';', f);
   break;
  case S_DSABLE:
   __wrap_puts("disable ", f);
   dmp_expr(f, stp->st.sdsable.dsablx);
   __wrap_putc(';', f);
   break;
  case S_REPSETUP:
   if (__debug_flg) __wrap_puts("**added setup for repeat**", f);
   break;
  case S_REPDCSETUP:
   if (__debug_flg) __wrap_puts("**added repeat event control setup**", f);
   break;
  case S_GOTO:
   if (__debug_flg)
    {
     char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[RECLEN];

     gtstp = stp->st.sgoto;
     /* DBG remove -- */
     if (gtstp == NULL) __misc_terr(__FILE__, __LINE__);
     /* --- */
     sprintf(s1,
      "** add goto at %s [lpend=%0d,dc=%0d,lstend=%0d,dst=%0d] targ %s at %s [lpend=%0d,dc=%0d,lstend=%0d,dst=%0d]",
      __bld_lineloc(s2, stp->stfnam_ind, stp->stlin_cnt),
      stp->lpend_goto, stp->dctrl_goto, stp->lstend_goto, stp->lpend_goto_dest,
      __to_sttyp(s3, gtstp->stmttyp),
      __bld_lineloc(s4, gtstp->stfnam_ind, gtstp->stlin_cnt),
      gtstp->lpend_goto, gtstp->dctrl_goto, gtstp->lstend_goto,
      gtstp->lpend_goto_dest);
     __wrap_puts(s1, f);
    }
   break;
  default: __case_terr(__FILE__, __LINE__);
  }
}

/*
 * dump a case statement
 * notice that now only place for expr lst is case selector lists
 * notice this routine cannot take f == nil, not need and tricky formatting
 */
static void dmp_case(FILE *f, struct st_t *stp)
{
 register struct csitem_t *csip;
 register struct exprlst_t *xplp;
 int32 first_time;
 struct csitem_t *dflt_csip;

 /* dump the case selector */
 dmp_casesel(f, stp);
 dflt_csip = stp->st.scs.csitems;
 csip = dflt_csip->csinxt;
 __pv_stlevel++;
 for (; csip != NULL; csip = csip->csinxt)
  {
   if (__outlinpos != 0) { __wrap_putc('\n', f); __outlinpos = 0; }
   first_time = TRUE;
   for (xplp = csip->csixlst; xplp != NULL; xplp = xplp->xpnxt)
    {
     if (first_time) first_time = FALSE; else __wrap_puts(", ", f);
     dmp_expr(f, xplp->xp);
    }
   __wrap_putc(':', f);
   dmp_lstofsts(f, csip->csist);
  }
 if (__outlinpos != 0) { __wrap_putc('\n', f); __outlinpos = 0; }
 if (dflt_csip->csist != NULL) dmp_case_dflt(f, dflt_csip);
 __pv_stlevel--;
 if (__outlinpos != 0) __nl_wrap_puts("", f);
 __nl_wrap_puts("endcase", f);
}

/*
 * dump default
 */
static void dmp_case_dflt(FILE *f, struct csitem_t *dflt_csip)
{
 __outlinpos = 0;
 __wrap_puts("default:", f);
 if (dflt_csip->csist != NULL) dmp_lstofsts(f, dflt_csip->csist);
}

/*
 * dump a fj statement list - enclosing stuff done elsewhere
 */
static void dmp_fj_stlst(FILE *f, struct st_t *stp)
{
 register int32 fji;
 register struct st_t *fjstp;

 for (fji = 0;; fji++)
  {
   if ((fjstp = stp->st.fj.fjstps[fji]) == NULL) break;

   /* SJM 09/24/01 - can have 2 stmt if for assign and other inserted */
   if (fjstp->rl_stmttyp == S_REPSETUP || fjstp->rl_stmttyp == S_FORASSGN
    || fjstp->rl_stmttyp == S_REPDCSETUP) fjstp = fjstp->stnxt;

   __dmp_stmt(f, fjstp, NL);
  }
}

/*
 * dump a task or function declartion
 */
static void dmp_task(FILE *f, struct task_t *tskp)
{
 struct sy_t *syp;

 syp = tskp->tsksyp;
 switch ((byte) tskp->tsktyp) {
  case TASK:
   if (__outlinpos != 0) __nl_wrap_puts("", f);
   __nl_wrap_puts("", f);
   __wrap_puts("task ", f);
   __wrap_puts(syp->synam, f);

   if (tskp->tf_lofp_decl) dmp_tf_lofp_hdr(f, tskp);
   __wrap_putc(';', f);

   dmp_tfdecls(f, tskp);
   dmp_paramdecls(f, tskp->tsk_prms, tskp->tprmnum, "parameter");
   dmp_lstofsts(f, tskp->tskst);
   if (__outlinpos != 0) __nl_wrap_puts("", f);
   __nl_wrap_puts("endtask", f);
   break;
  case FUNCTION:
   dmp_func_decl(f, tskp);
   break;
  /* named task */
  case Begin:
  case FORK:
   dmp_nblock(f, tskp, syp->synam);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * dump a function declaration
 */
static void dmp_func_decl(FILE *f, struct task_t *tskp)
{
 struct net_t *np;
 char ftyp[RECLEN], s1[RECLEN];

 if (__outlinpos != 0) __nl_wrap_puts("", f);
 __nl_wrap_puts("", f);
 np = tskp->tskpins->tpsy->el.enp;
 /* set function type */
 if (np->ntyp == N_REAL) strcpy(ftyp, "real");
 else if (np->ntyp == N_INT) strcpy(ftyp, "integer");
 else if (np->ntyp == N_TIME) strcpy(ftyp, "time");
 else __to_wrange(ftyp, np);
 sprintf(s1, "function %s ", ftyp);
 __wrap_puts(s1, f);
 __wrap_puts(tskp->tsksyp->synam, f);

 /* for new Ver 2001, if function args declared in header, emit in hdr */
 if (tskp->tf_lofp_decl) dmp_tf_lofp_hdr(f, tskp);

 __nl_wrap_puts(";", f);

 dmp_tfdecls(f, tskp);
 dmp_paramdecls(f, tskp->tsk_prms, tskp->tprmnum, "parameter");
 dmp_lstofsts(f, tskp->tskst);
 if (__outlinpos != 0) __nl_wrap_puts("", f);
 __nl_wrap_puts("endfunction", f);
}

/*
 * for named block either begin : [n] - end or fork-join blocks
 * even if one statement need begin-join block
 */
static void dmp_nblock(FILE *f, struct task_t *tskp, char *bnam)
{
 struct st_t *stp2;

 __pv_stlevel++;
 if (tskp->tsktyp == FORK) __wrap_puts("fork : ", f);
 else __wrap_puts("begin : ", f);
 __wrap_puts(bnam, f);
 __nl_wrap_puts("", f);
 dmp_tfdecls(f, tskp);
 dmp_paramdecls(f, tskp->tsk_prms, tskp->tprmnum, "parameter");
 __pv_stlevel++;
 if (tskp->tsktyp == FORK) dmp_fj_stlst(f, tskp->tskst);
 else
  { 
   for (stp2 = tskp->tskst; stp2 != NULL; stp2 = stp2->stnxt)
    __dmp_stmt(f, stp2, NL);
  }
 __pv_stlevel--;
 if (__outlinpos != 0) __nl_wrap_puts("", f);
 if (tskp->tsktyp == FORK) __nl_wrap_puts("join", f); 
 else __nl_wrap_puts("end", f);
 __pv_stlevel--;
}

/*
 * dump tf declares
 */
static void dmp_tfdecls(FILE *f, struct task_t *tskp)
{
 register int32 i;
 register struct task_pin_t *tpp;
 register struct net_t *regp;
 struct sy_t *syp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 if (__outlinpos != 0) __nl_wrap_puts("", f);
 if (!tskp->tf_lofp_decl)
  {
   /* SJM 05/26/04 - only dump tf port if they were not declared in hdr */ 
   for (tpp = tskp->tskpins; tpp != NULL; tpp = tpp->tpnxt)
    {
     /* 1st parameter for function is required return value */
     if (tskp->tsktyp == FUNCTION && tpp == tskp->tskpins) continue;

     syp = tpp->tpsy;
     regp = syp->el.enp;
     if (!regp->n_isavec || regp->ntyp == N_TIME || regp->ntyp == N_INT
      || regp->ntyp == N_REAL) strcpy(s1, "");
     else __to_wrange(s1, regp);

     /* notice __to_wrange truncated to max of RECLEN - ok since only no.s */
     sprintf(s3, " %s%s ", __to_ptnam(s2, tpp->trtyp), s1);
     __wrap_puts(s3, f);
     __wrap_puts(syp->synam, f);
     __nl_wrap_puts(";", f);
    }
  }

 if (tskp->trnum == 0) return;
 for (i = 0, regp = &(tskp->tsk_regs[0]); i < tskp->trnum; i++, regp++)
  {
   if (regp->ntyp == N_REG && regp->iotyp != NON_IO) continue;

   if (!regp->n_isavec || regp->ntyp == N_TIME || regp->ntyp == N_INT
    || regp->ntyp == N_REAL) strcpy(s1, "");
   else __to_wrange(s1, regp);

   sprintf(s3, " %s%s ", __to_wtnam(s2, regp), s1);
   __wrap_puts(s3, f);
   __wrap_puts(regp->nsym->synam, f);
   __nl_wrap_puts(";", f);
  }
}

/*
 * dump tf list of ports header task port decls
 */
static void dmp_tf_lofp_hdr(FILE *f, struct task_t *tskp)
{
 register struct task_pin_t *tpp;
 int32 first_time;
 struct sy_t *syp;
 struct net_t *regp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 __wrap_putc('(', f);
 first_time = TRUE;
 for (tpp = tskp->tskpins; tpp != NULL; tpp = tpp->tpnxt)
  {
   /* 1st parameter for function is required return value */
   if (tskp->tsktyp == FUNCTION && tpp == tskp->tskpins) continue;

   if (!first_time) __wrap_puts(", ", f); else first_time = FALSE;

   syp = tpp->tpsy;
   regp = syp->el.enp;
   if (!regp->n_isavec || regp->ntyp == N_TIME || regp->ntyp == N_INT
    || regp->ntyp == N_REAL) strcpy(s1, "");
   else __to_wrange(s1, regp);

   /* notice __to_wrange truncated to max of RECLEN - ok since only no.s */
   sprintf(s3, "%s%s ", __to_ptnam(s2, tpp->trtyp), s1);
   __wrap_puts(s3, f);
   __wrap_puts(syp->synam, f);
  }
 __wrap_putc(')', f);
}

/*
 * SPECIFY SECTION SOURCE DUMP ROUTINES
 * NULL FILE ILLEGAL FOR FILL STRING ILLEGAL
 */

/*
 * dump module specify section - will not be called if not persent
 */
static void dmp_mdspfy(FILE *f, struct mod_t *mdp)
{ 
 struct spfy_t *spfp;

 __pv_stlevel = 1;
 if (__outlinpos != 0) __nl_wrap_puts("", f);
 __nl_wrap_puts("", f);
 __outlinpos = 0;
 __nl_wrap_puts("specify", f);

 spfp = mdp->mspfy;
 /* parameters - this indents by 1 level */
 dmp_paramdecls(f, spfp->msprms, spfp->sprmnum, "specparam");

 __pv_stlevel = 2;
 /* delay paths */
 dmp_specpths(f, spfp->spcpths);
 /* timing checks */
 dmp_tchks(f, spfp->tchks);
 __pv_stlevel = 1;
 __nl_wrap_puts("endspecify", f);
 __pv_stlevel = 0;
}

/*
 * dump specify section paths
 * (<pth lst> [=*]> <pth lst>) = (<delay list>); 
 * edge dependent paths will parse but not stored, simulated or dmped
 */
static void dmp_specpths(FILE *f, register struct spcpth_t *pthp)
{
 char s1[RECLEN];

 for (; pthp != NULL; pthp = pthp->spcpthnxt) 
  {
   if (pthp->pth_gone) continue;

   /* dump sdpd (state dependent path desc.) if present */
   if (pthp->pthcondx != NULL)
    { 
     __wrap_puts("if (", f);
     dmp_expr(f, pthp->pthcondx);
     __wrap_puts(") ", f);
    }
   else if (pthp->pth_ifnone) __wrap_puts("ifnone ", f);
   if (pthp->datasrcx == NULL)
    { 
     __wrap_putc('(', f);
     dmp_pthlst(f, pthp, TRUE);
     __wrap_putc(' ', f);
     if (pthp->pthpolar != POLAR_NONE)
      {
      if (pthp->pthpolar == POLAR_PLUS) __wrap_putc('+', f);
       else __wrap_putc('-', f);
      }
     if (pthp->pthtyp == PTH_FULL) __wrap_puts("*> ", f);
     else __wrap_puts("=> ", f); 
     dmp_pthlst(f, pthp, FALSE);
    }
   else
    {
     __wrap_putc('(', f);
     if (pthp->pthedge != NOEDGE) 
      {
       __wrap_puts(__to_edgenam(s1, pthp->pthedge), f);
       __wrap_putc(' ', f);
      }
     dmp_pthlst(f, pthp, TRUE);
     __wrap_putc(' ', f);
     if (pthp->pthpolar != POLAR_NONE)
      {
       if (pthp->pthpolar == POLAR_PLUS) __wrap_putc('+', f);
       else __wrap_putc('-', f);
      }
     if (pthp->pthtyp == PTH_FULL) __wrap_puts("*> ", f);
     else __wrap_puts("=> ", f); 
     /* edge sensitive path output form */
     __wrap_putc('(', f);
     dmp_pthlst(f, pthp, FALSE);
     __wrap_putc(' ', f);
     if (pthp->dsrc_polar != POLAR_NONE)
      {
       if (pthp->dsrc_polar == POLAR_PLUS) __wrap_putc('+', f);
       else __wrap_putc('-', f);
      }
     __wrap_puts(": ", f);
     /* this can be fcall comma form, does it work by itself ? */
     dmp_expr(f, pthp->datasrcx);
     __wrap_putc(')', f);
    }
   __wrap_puts(") = ", f);
   /* notice delay always start with ' ' and if only 1 value no parens */
   dmp_delay(f, pthp->pth_du, pthp->pth_delrep, "");
   __nl_wrap_puts(";", f);
  }
}

/*
 * dump a path list
 */
static void dmp_pthlst(FILE *f, struct spcpth_t *pthp, int32 is_pein)
{
 register int32 pthi;

 if (is_pein)
  {
   for (pthi = 0; pthi <= pthp->last_pein; pthi++)
    {
     if (pthi != 0) __wrap_puts(", ", f);
     dmp_pthel(f, &(pthp->peins[pthi]));
    }
  } 
 else
  {
   for (pthi = 0; pthi <= pthp->last_peout; pthi++)
    {
     if (pthi != 0) __wrap_puts(", ", f);
     dmp_pthel(f, &(pthp->peouts[pthi]));
    }
  }
}

/*
 * dump a preped path element form
 */
static void dmp_pthel(FILE *f, struct pathel_t *pelp)
{
 char s1[RECLEN];

 __wrap_puts(pelp->penp->nsym->synam, f);
 if (pelp->pthi1 == -1) return;

 if (pelp->pthi1 == pelp->pthi2) sprintf(s1, "[%d]", pelp->pthi1);
 else sprintf(s1, "[%d:%d]", pelp->pthi1, pelp->pthi2);
 __wrap_puts(s1, f);
}

/*
 * dump specify section timing checks 
 * $setup(<data event>, <reference event>, limit, <opt. notifier>)
 */
static void dmp_tchks(FILE *f, register struct tchk_t *tcp)
{
 char s1[RECLEN];

 for (; tcp != NULL; tcp = tcp->tchknxt)
  {
   /* when dumping only emit hold (same conn. order half with right name) */
   if (tcp->tc_gone || tcp->tc_supofsuphld || tcp->tc_recofrecrem) continue; 

   __wrap_puts(__to_tcnam(s1, tcp->tchktyp), f);
   __wrap_putc('(', f); 
   /* dump an event */
   dmp_tchk_selector(f, tcp->startedge, tcp->startxp, tcp->startcondx);
   if (tcp->tchktyp != TCHK_PERIOD && tcp->tchktyp != TCHK_WIDTH) 
    { 
     __wrap_puts(", ", f);
     dmp_tchk_selector(f, tcp->chkedge, tcp->chkxp, tcp->chkcondx);
    }
   /* dump a delay - know will never be list form */
   __wrap_puts(", ", f); 
   dmp_delay(f, tcp->tclim_du, tcp->tc_delrep, "");
   if (tcp->tc_haslim2)
    {
     __wrap_puts(", ", f); 
     dmp_delay(f, tcp->tclim2_du, tcp->tc_delrep2, "");
    }
   /* need , place holder if no limit 2 but notifier */
   if (tcp->ntfy_np != NULL)
    {
     __wrap_puts(", ", f);
     __wrap_puts(tcp->ntfy_np->nsym->synam, f); 
    } 
   __nl_wrap_puts(");", f);
  }
}

/*
 * dump a timing check selector expression form
 */
static void dmp_tchk_selector(FILE *f, word32 edgval, struct expr_t *xp,
 struct expr_t *condx)
{
 char s1[RECLEN];

 if (edgval != NOEDGE) 
  { __wrap_puts(__to_edgenam(s1, edgval), f); __wrap_putc(' ', f); }
 dmp_expr(f, xp);
 if (condx != NULL) { __wrap_puts(" &&& ", f); dmp_expr(f, condx); }
}

/*
 * dump one gref entry
 */
static void dmp_mod_grefs(FILE *f, struct mod_t *mdp)
{
 register int32 gri;
 register struct gref_t *grp;
 char s1[RECLEN];

 if (mdp->mgrnum == 0) return;

 __pv_stlevel = 0;
 if (__outlinpos != 0) __nl_wrap_puts("", f);
 __wrap_puts("/* ==> hierarchical path occurring in ", f);
 __wrap_puts(mdp->msym->synam, f);
 __nl_wrap_puts(":", f);
 for (gri = 0, grp = &(mdp->mgrtab[0]); gri < mdp->mgrnum; gri++, grp++)
  {
   if (grp->gr_gone) continue;
   
   __wrap_puts(grp->gnam, f);  
   __wrap_puts(to_glbinfo(s1, grp), f);
   __nl_wrap_puts("", f); 
   /* -- could dump target symbol contents */
   /* -- could dump expr. containing gref */
  }
 __nl_wrap_puts("=== end of globals. */", f);
}

/*
 * convert global reference entry to info line
 */
static char *to_glbinfo(char *s, struct gref_t *grp)
{
 struct sy_t *syp;
 char s1[RECLEN], s2[RECLEN];

 sprintf(s, "<target %s", grp->targmdp->msym->synam);
 /* this is symbol corresponding to symbol table target in */
 if (grp->targtskp != NULL)
  {
   syp = grp->targtskp->tsksyp;
   sprintf(s1, " in %s %s", __to_sytyp(s2, syp->sytyp), syp->synam);
   strcat(s, s1);
  }
 sprintf(s1, " %s)", __bld_lineloc(s2, grp->grfnam_ind, grp->grflin_cnt));
 strcat(s, s1);
 strcat(s, ">");
 return(s);
}

/*
 * ROUTINES THAT CAN BE CALLED WITH NULL F TO GO IN EXPRLINE
 */

/*
 * dump the case selector
 */
static void dmp_casesel(FILE *f, struct st_t *stp)
{
 char s1[RECLEN];

 if (stp->st.scs.castyp == CASEZ) strcpy(s1, "casez");
 else if (stp->st.scs.castyp == CASEX) strcpy(s1, "casex");
 else strcpy(s1, "case");
 __wrap_puts(s1, f);
 __wrap_puts(" (", f);
 dmp_expr(f, stp->st.scs.csx);
 __wrap_putc(')', f);
}

/*
 * for delays already converted to scaled sim. form
 * know by this representation if number will not be x/z
 *
 * handles no delay case (its a union) and if writes insert leading space 
 * notice dumping delays always just emits the 1st
 */
static void dmp_delay(FILE *f, union del_u du, word32 drep, char *sharps)
{
 register int32 i; 
 int32 ndels; 
 word32 t1a[2], t1b[2];
 word64 tarr[16], tlist[16], *timp; 
 char s1[RECLEN];

 /* think ,, form will work right here for timing checks */
 if (drep == DT_PTHDST || drep == DT_NONE || (drep == DT_CMPLST
  && du.pdels == NULL)) goto done;

 if (strcmp(sharps, "") != 0)
  { __wrap_putc(' ', f); __wrap_puts(sharps, f); }
 t1b[0] = t1b[1] = 0L;
 
 switch ((byte) drep) {
  case DT_1V:
   /* 1 v is a ptr to an 8 byte rec., is1v is ptr to array but use 1st */
   t1a[0] = (word32) ((*du.d1v) & WORDMASK_ULL);
   t1a[1] = (word32) (((*du.d1v) >> 32) & WORDMASK_ULL);
   __regab_disp(t1a, t1b, TIMEBITS, BDEC, TRUE, FALSE);
   if (f != NULL) { __wrap_puts(__exprline, f); __cur_sofs = 0; }
   break;
  case DT_IS1V:
   /* 1 v is a ptr to an 8 byte rec., is1v is ptr to array but use 1st */
   t1a[0] = (word32) (du.dis1v[0] & WORDMASK_ULL);
   t1a[1] = (word32) ((du.dis1v[0] >> 32) & WORDMASK_ULL);
   __regab_disp(t1a, t1b, TIMEBITS, BDEC, TRUE, FALSE);
   if (f != NULL) { __wrap_puts(__exprline, f); __cur_sofs = 0; }
   break;
  case DT_IS1V1:
   sprintf(s1, "%lu", (word32) du.dis1v1[0]);
   __wrap_puts(s1, f);
   break;
  case DT_IS1V2:
   sprintf(s1, "%lu", (word32) du.dis1v2[0]);
   __wrap_puts(s1, f);
   break;
  case DT_4V: timp = du.d4v; goto do4;
  case DT_IS4V:
   timp = du.dis4v;
do4:
   /* for IS 4 v linear array of groups of 4 so just use first */
   __wrap_putc('(', f);
   t1a[0] = (word32) (timp[1] & WORDMASK_ULL);
   t1a[1] = (word32) ((timp[1] >> 32) & WORDMASK_ULL);

   __regab_disp(t1a, t1b, TIMEBITS, BDEC, TRUE, FALSE);
   if (f != NULL) { __wrap_puts(__exprline, f); __cur_sofs = 0; }
   __wrap_puts(", ", f);
   t1a[0] = (word32) (timp[0] & WORDMASK_ULL);
   t1a[1] = (word32) ((timp[0] >> 32) & WORDMASK_ULL);
   __regab_disp(t1a, t1b, TIMEBITS, BDEC, TRUE, FALSE);
   if (f != NULL) { __wrap_puts(__exprline, f); __cur_sofs = 0; }
   __wrap_puts(", ", f);
   t1a[0] = (word32) (timp[2] & WORDMASK_ULL);
   t1a[1] = (word32) ((timp[2] >> 32) & WORDMASK_ULL);
   __regab_disp(t1a, t1b, TIMEBITS, BDEC, TRUE, FALSE);
   if (f != NULL) { __wrap_puts(__exprline, f); __cur_sofs = 0; }
   __wrap_putc(')', f);
   break;
  case DT_IS4V1:
   /* for IS 4 v linear array of groups of 4 so just use first */
   sprintf(s1, "(%lu, %lu, %lu)", (word32) du.dis4v1[1],
    (word32) du.dis4v1[0], (word32) du.dis4v1[2]);
   __wrap_puts(s1, f);
   break;
  case DT_IS4V2:
   /* for IS 4 v linear array of groups of 4 so just use first */
   sprintf(s1, "(%lu, %lu, %lu)", (word32) du.dis4v2[1],
    (word32) du.dis4v2[0], (word32) du.dis4v2[2]);
   __wrap_puts(s1, f);
   break;
  /* notice all the 6 forms are really size 16 tables */
  case DT_16V: timp = du.d16v; goto do16;
  case DT_IS16V:
   timp = du.dis16v;
do16:
   for (i = 0; i < 16; i++) tarr[i] = timp[i];
try_reduce:
   /* first step, reorder internal 16 table to 12 values */
   __map_16v_to_12vform(tlist, tarr);
   __try_reduce_16vtab(tlist, &ndels); 
   __wrap_putc('(', f);
   for (i = 0; i < ndels; i++) 
    {
     if (i != 0) __wrap_puts(", ", f);
     t1a[0] = (word32) (tlist[i] & WORDMASK_ULL);
     t1a[1] = (word32) ((tlist[i] >> 32) & WORDMASK_ULL);
     __regab_disp(t1a, t1b, TIMEBITS, BDEC, TRUE, FALSE);
     if (f != NULL) { __wrap_puts(__exprline, f); __cur_sofs = 0; }
    }
   __wrap_putc(')', f);
   break;
  case DT_IS16V1:
   for (i = 0; i < 16; i++) { tarr[i] = (word64) du.dis16v1[i]; }
   goto try_reduce;
  case DT_IS16V2:
   for (i = 0; i < 16; i++) { tarr[i] = (word64) du.dis16v2[i]; }
   goto try_reduce;
  case DT_1X:
   __wrap_putc('(', f);
   dmp_expr(f, du.d1x);
   __wrap_putc(')', f);
   break;
  case DT_4X:
   __wrap_putc('(', f);
   dmp_expr(f, du.d4x[1]);
   __wrap_puts(", ", f);
   dmp_expr(f, du.d4x[0]);
   if (du.d4x[2] != NULL) { __wrap_puts(", ", f); dmp_expr(f, du.d4x[2]); }
   __wrap_putc(')', f);
   break;
  case DT_CMPLST:
   dmp_dellst(f, du.pdels);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
done:
 if (f != NULL) __cur_sofs = 0; else __exprline[__cur_sofs] = '\0';
}

/*
 * dump a delay or module instance parameter list
 * this must handle no delay case
 */
static void dmp_dellst(FILE *f, register struct paramlst_t *pmp)
{
 int32 first_time;
 struct expr_t *xp;

 __force_base = BDEC;
 if (pmp->pmlnxt == NULL)
  {
   xp = pmp->plxndp;
   switch ((byte) xp->optyp) {
    case NUMBER: case REALNUM: case ISNUMBER: case ISREALNUM: case ID:
     dmp_expr(f, pmp->plxndp);
     __force_base = BNONE;
     return;
    }
  }
 /* for even 1 expr. need parentheses - also for global here */
 __wrap_putc('(', f);
 for (first_time = TRUE; pmp != NULL; pmp = pmp->pmlnxt)
  {
   if (first_time) first_time = FALSE; else __wrap_puts(", ", f);
   dmp_expr(f, pmp->plxndp);
  }
 __wrap_putc(')', f);
 __force_base = BNONE;
}

/*
 * dump the for header part
 */
extern void __dmp_forhdr(FILE *f, struct for_t *frs)
{
 int32 sav_debug_flg;

 sav_debug_flg = __debug_flg;
 __debug_flg = TRUE;
 __wrap_puts("for (", f);
 /* ending ; will be written */
 __dmp_proc_assgn(f, frs->forassgn, (struct delctrl_t *) NULL, TRUE);
 __wrap_putc(' ', f);
 dmp_expr(f, frs->fortermx);
 __wrap_puts("; ", f);
 /* cannot use dmp procedural assign because of trailing ; */
 dmp_expr(f, frs->forinc->st.spra.lhsx);
 __wrap_puts(" = ", f);
 dmp_expr(f, frs->forinc->st.spra.rhsx);
 __wrap_puts(") ", f);
 __debug_flg = sav_debug_flg;
}

/*
 * dump a procedural assignment statement
 * for rhs delay control statement is dctrl action statement
 */
extern void __dmp_proc_assgn(FILE *f, struct st_t *stp, struct delctrl_t *dctp,
 int32 force_for)
{
 /* for assign moved to before for, but must not emit */
 if (!force_for && stp->stmttyp == S_FORASSGN && stp->stnxt != NULL
  && stp == stp->stnxt->st.sfor->forassgn) return;

 dmp_expr(f, stp->st.spra.lhsx);
 __wrap_puts(" = ", f);

 if (dctp != NULL)
  {
   if (dctp->repcntx != NULL)
    {
     __wrap_puts("repeat (", f);
     dmp_expr(f, dctp->repcntx);
     __wrap_puts(") ", f);
    }
   __dmp_dctrl(f, dctp);
  }
 dmp_expr(f, stp->st.spra.rhsx);
 __wrap_puts(";", f);
 /* here may have added goto for rhs form */
 if (dctp != NULL && dctp->actionst != NULL)
  __dmp_stmt(f, dctp->actionst, FALSE);
}

/*
 * dump a non blocking procedural assignment statement
 * for rhs delay control statement is dctrl action statement
 */
extern void __dmp_nbproc_assgn(FILE *f, struct st_t *stp,
 struct delctrl_t *dctp)
{
 dmp_expr(f, stp->st.spra.lhsx);
 __wrap_puts(" <= ", f);
 /* know this dctrl will not have action statement */
 if (dctp != NULL)
  {
   if (dctp->repcntx != NULL)
    {
     __wrap_puts("repeat (", f);
     dmp_expr(f, dctp->repcntx);
     __wrap_puts(") ", f);
    }
   __dmp_dctrl(f, dctp);
  }
 dmp_expr(f, stp->st.spra.rhsx);
 __wrap_puts(";", f);
}

/*
 * dump a delay control
 */
extern void __dmp_dctrl(FILE *f, struct delctrl_t *dctp)
{
 struct delctrl_t tmpdctp;

 switch ((byte) dctp->dctyp) {
  case DC_EVENT:
   __wrap_putc('@', f);
non_rhs:
   /* continue on same line */
   __dmp_dcxpr(f, dctp->dc_du, dctp->dc_delrep);
   /* this can be list because of added goto if debug flag on */
   if (dctp->actionst != NULL) dmp_lstofsts(f, dctp->actionst);
   break;
  case DC_DELAY:
   __wrap_putc('#', f);
   goto non_rhs;
  case DC_RHSEVENT:
  case DC_RHSDELAY:
   if (dctp->dctyp == DC_RHSEVENT) tmpdctp.dctyp = DC_EVENT;
   else tmpdctp.dctyp = DC_DELAY;
   tmpdctp.actionst = NULL;
   /* notice since this is read only just sharing previous expr */
   tmpdctp.dc_delrep = dctp->dc_delrep;
   tmpdctp.dc_du = dctp->dc_du;
   tmpdctp.repcntx = dctp->repcntx;

   if (dctp->actionst->stmttyp == S_NBPROCA)
    __dmp_nbproc_assgn(f, dctp->actionst, &tmpdctp);
   else if (dctp->actionst->stmttyp == S_RHSDEPROCA)
    __dmp_proc_assgn(f, dctp->actionst, &tmpdctp, FALSE);
   else __case_terr(__FILE__, __LINE__);
   /* if dumping source - maybe goto as actionst next */ 
   if (__run_state != SS_SIM && dctp->actionst->stnxt != NULL)
    {
     /* notice set off during wire init and no statemnt exec there */
     if (dctp->actionst->stnxt->stmttyp != S_GOTO)
      __misc_terr(__FILE__, __LINE__);
     __dmp_stmt(f, dctp->actionst->stnxt, NONL);
    }
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 if (f == NULL) __exprline[__cur_sofs] = '\0';
}

/*
 * dump the actual delay
 * can be used during compilation or after prep. where ticks (scaled) dumped 
 */
extern void __dmp_dcxpr(FILE *f, union del_u du, word32 drep)
{
 int32 leaf;
 struct expr_t *dxp;

 __force_base = BDEC;
 if (drep != DT_CMPLST)
  {
   dmp_delay(f, du, drep, "");
   __wrap_putc(' ', f); 
  }
 else
  {
   dxp = du.pdels->plxndp;  
   leaf = __isleaf(dxp);
   if (!leaf) __wrap_putc('(', f);
   dmp_expr(f, dxp);
   if (!leaf) __wrap_putc(')', f);
   /* needed to make sure exprline for f nil terminated */ 
   __wrap_putc(' ', f);
  }
 if (f == NULL) __exprline[__cur_sofs] = '\0';
 __force_base = BNONE;
}

/*
 * dump a task call
 * should dump task call global name
 */
extern void __dmp_tskcall(FILE *f, struct st_t *stp)
{
 register struct expr_t *xp;
 int32 first_time;

 dmp_expr(f, stp->st.stkc.tsksyx);
 first_time = TRUE;
 for (xp = stp->st.stkc.targs; xp != NULL; xp = xp->ru.x)
  {
   if (first_time) { __wrap_putc('(', f); first_time = FALSE; }
   else __wrap_puts(", ", f);
   dmp_expr(f, xp->lu.x);
  }
 if (!first_time) __wrap_putc(')', f);
 /* need null terminated string */
 __wrap_puts(";", f);
}

/*
 * dump a statement list
 */
static void dmp_lstofsts(FILE *f, struct st_t *hdstp)
{
 register struct st_t *stp;

 if (hdstp->st_unbhead)
  {
   if (__outlinpos != 0) __nl_wrap_puts("", f);
   __pv_stlevel++;
   __wrap_puts("begin", f);
   __pv_stlevel++;
   for (stp = hdstp; stp != NULL; stp = stp->stnxt)
    __dmp_stmt(f, stp, NL);
   if (__outlinpos != 0) __nl_wrap_puts("", f);
   __pv_stlevel--;
   __nl_wrap_puts("end", f);
   __pv_stlevel--;
   return;
  }
 /* added control statements if being printed go on same line */
 for (stp = hdstp; stp != NULL; stp = stp->stnxt)
  __dmp_stmt(f, stp, NONL);
}

/*
 * build an array range expression from a net
 * assume comp. time representation
 *
 * notice this uses expr line so cannot be called from expr_tostr
 * but does call it
 */
extern char *__to_arr_range(char *s, struct net_t *np)
{
 int32 r1, r2, awid;

 if (np->n_isarr)
  {
   if (np->nrngrep == NX_CT)
    __msgtox_wrange(s, np->nu.ct->ax1, np->nu.ct->ax2);
   else
    {
     __getarr_range(np, &r1, &r2, &awid);
     sprintf(s, "[%d:%d]", r1, r2);
    }
  }
 else strcpy(s, "");
 return(s);
}

/*
 * build a wire range expression from a net
 * silently trucates if > RECLEN
 *
 * this is for ranges in messages - ok since will be numbers, if expr.
 * will use other routine
 */
extern char *__to_wrange(char *s, struct net_t *np)
{
 int32 r1, r2;

 if (np->n_isavec)
  {
   if (np->nrngrep == NX_CT)
    __msgtox_wrange(s, np->nu.ct->nx1, np->nu.ct->nx2);
   else
    {

     /* if this is a vector and not compile time rep, know will be range */
     __getwir_range(np, &r1, &r2);
     sprintf(s, "[%d:%d]", r1, r2);
    }
  }
 else strcpy(s, "");
 return(s);
}

/*
 * build a range expression in expr line
 * must terminate string in exprline if nil
 * this can be nil
 */
static void tox_wrange(FILE *f, struct expr_t *x1, struct expr_t *x2)
{
 __force_base = BDEC;
 if (x1 == NULL) return;
 __wrap_putc('[', f);
 dmp_expr(f, x1);
 __wrap_putc(':', f);
 dmp_expr(f, x2);
 __wrap_putc(']', f);
 if (f == NULL) __exprline[__cur_sofs] = '\0';
 __force_base = BNONE;
}


/*
 * ROUTINES TO DUMP VERILOG EXPRESSIONS TO SOURCE FORMAT
 */

/*
 * either write a string through dmp puts path (wraps lines) if f not nil 
 * or else collect in __exprline starting at __cur_sofs (make big enough) 
 *
 * must write immediately for dumping source to get line wrapping right
 * note this requires current itree place - will crash otherwise
 * if f is not nil, __cur_sofs must be set to 0 since used as work area
 */
static void dmp_expr(FILE *f, struct expr_t *ndp)
{
 int32 indv, ind1, ind2, nd_par, sav_spos;
 word32 *wp;
 struct net_t *np;
 char *chp;
 char s1[RECLEN];

 /* expr. pointer while converting to string bad */
 if (ndp == NULL || ndp->optyp == 0 || ndp->optyp == UNDEF)
  __misc_terr(__FILE__, __LINE__);

 switch ((byte) ndp->optyp) {
  case ID:
   /* if empty symbol - emit empty string here - for wire ranges */
   if (ndp->lu.sy == NULL) return; 
   if (ndp->locqualnam) chp = ndp->ru.qnchp; else chp = ndp->lu.sy->synam;
   __wrap_puts(chp, f);
   return;
  case GLBREF:
   /* know gref always have name - first expr. image during parsing */
   /* then image with instance array selectors folded to numbers */
   /* once parsing complete xmrs convert to gnam so if global */
   /* select is xmr that expr will resolved to gnam before here */
   chp = ndp->ru.grp->gnam;
   __wrap_puts(chp, f);
   return;
  case XMRID:
   /* special case during global resolution xmr name - no symbol */ 
   chp = ndp->ru.qnchp;
   __wrap_puts(chp, f);
   return;
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM:
   /* if empty do not emit anything, else fill with constant value  */
   /* this knows about all number forms but IS is 0th only */
   if (f == NULL) numexpr_disp(ndp, 0);
   else
    { 
     sav_spos = __cur_sofs;
     numexpr_disp(ndp, 0);
     __wrap_puts(&(__exprline[sav_spos]), f);
     __cur_sofs = sav_spos; 
     __exprline[__cur_sofs] = '\0'; 
    }
   return;
  case LSB:
   if (ndp->lu.x->optyp == XMRID) dmp_expr(f, ndp->lu.x);
   else if (ndp->lu.x->lu.sy != NULL) dmp_expr(f, ndp->lu.x);
   __wrap_putc('[', f);
   /* convert to original value (not h:0 new adjusted) if needed */
   if (ndp->ru.x->ind_noth0 && ndp->lu.x->lu.sy != NULL)
    {
     /* notice for dumping - convention is to use IS 0th */
     wp = &(__contab[ndp->ru.x->ru.xvi]);
     indv = __unnormalize_ndx(ndp->lu.x->lu.sy->el.enp, (int32) wp[0]);
     sprintf(s1, "%d", indv);
     __wrap_puts(s1, f);
     __wrap_putc(']', f);
    }
   else { dmp_expr(f, ndp->ru.x); __wrap_putc(']', f); }
   if (f == NULL) __exprline[__cur_sofs] = '\0'; 
   return;
  case PARTSEL:
   if (ndp->lu.x->lu.sy != NULL) dmp_expr(f, ndp->lu.x);
   __wrap_putc('[', f);
   /* convert to original value (not h:0 new adjusted) if needed */
   if (ndp->ru.x->lu.x->ind_noth0 && ndp->lu.x->lu.sy != NULL)
    {
     np = ndp->lu.x->lu.sy->el.enp;
     /* know part select range never IS form */
     wp = &(__contab[ndp->ru.x->lu.x->ru.xvi]);
     ind1 = __unnormalize_ndx(np, (int32) wp[0]);
     wp = &(__contab[ndp->ru.x->ru.x->ru.xvi]);
     ind2 = __unnormalize_ndx(np, (int32) wp[0]);
     sprintf(s1, "%d:%d]", ind1, ind2);
     __wrap_puts(s1, f);
    }
   else
    {
     dmp_expr(f, ndp->ru.x->lu.x);
     __wrap_putc(':', f);
     dmp_expr(f, ndp->ru.x->ru.x);
     __wrap_putc(']', f);
     if (f == NULL) __exprline[__cur_sofs] = '\0'; 
    }
   return;
  case QUEST:
   /* for now need parentheses around these or cannot parse */
   __wrap_putc('(', f);
   dmp_expr(f, ndp->lu.x);
   __wrap_putc(')', f);
   __wrap_puts(" ? ", f);
   dmp_expr(f, ndp->ru.x->lu.x);
   __wrap_puts(" : ", f);
   dmp_expr(f, ndp->ru.x->ru.x);
   return;
  case LCB:
   /* l of expr. node empty */
   dmp_catexpr(f, ndp);
   return;
  case FCALL:
   dmp_fcallx(f, ndp);
   return;
  /* dumping empty is write nothing */
  case OPEMPTY:
   return;
  /* event or cannot be parenthesisized or nested */ 
  case OPEVOR: case OPEVCOMMAOR:
   /* SJM 06/01/04 - current scheme is 2 ops that print different but same */
   dmp_evor_chain(f, ndp);
   return;
  /* FALLTHRU */
 }
 /* know this is operator - unary has right subtree nil */
 if (ndp->ru.x == NULL)
  {
   __wrap_puts(__to_opname(ndp->optyp), f);
   if (!is_simplex(ndp->lu.x) && ndp->lu.x->ru.x != NULL) nd_par = TRUE;
   else nd_par = FALSE;
   if (nd_par) __wrap_putc('(', f);
  
   dmp_expr(f, ndp->lu.x);
   if (nd_par) __wrap_putc(')', f);
   if (f == NULL) __exprline[__cur_sofs] = '\0'; 
   return;
  }
 /* DBG remove --- */
 if (ndp->lu.x == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */
 /* know this is binary */
 if (!is_simplex(ndp->lu.x)) nd_par = TRUE; else nd_par = FALSE; 
 if (nd_par) __wrap_putc('(', f);
 dmp_expr(f, ndp->lu.x);
 if (nd_par) __wrap_putc(')', f);
 __wrap_putc(' ', f);
 __wrap_puts(__to_opname(ndp->optyp), f);
 __wrap_putc(' ', f);
 if (!is_simplex(ndp->ru.x)) nd_par = TRUE; else nd_par = FALSE; 
 if (nd_par) __wrap_putc('(', f);
 dmp_expr(f, ndp->ru.x);
 if (nd_par) __wrap_putc(')', f); 
 if (f == NULL) __exprline[__cur_sofs] = '\0'; 
}

/*
 * return T if is simple object expr. leaf, concat, or fcall
 * that does not need parenthesis
 */
static int32 is_simplex(struct expr_t *xp)
{
 if (__isleaf(xp)) return(TRUE);
 switch (xp->optyp) {
  case LSB: case PARTSEL: case LCB: case FCALL: return(TRUE); 
 }
 return(FALSE);
}

/*
 * dump a concatenate
 * this really needs to have new lines and indentation added
 */
static void dmp_catexpr(FILE *f, struct expr_t *ndp)
{
 int32 first_time;

 __wrap_putc('{', f);
 /* know { operator left always unused */
 for (first_time = TRUE, ndp = ndp->ru.x; ndp != NULL; ndp = ndp->ru.x)
  {
   if (!first_time) __wrap_puts(", ", f); else first_time = FALSE;
   dmp_catel(f, ndp->lu.x);
  }
 __wrap_putc('}', f);
 if (f == NULL) __exprline[__cur_sofs] = '\0';
}

/*
 * dump a concatenate element
 */
static void dmp_catel(FILE *f, struct expr_t *ndp)
{
 if (ndp->optyp == CATREP)
  {
   __wrap_putc('(', f);
   dmp_expr(f, ndp->lu.x);
   __wrap_putc(')', f);
   dmp_catexpr(f, ndp->ru.x);
  }
 else dmp_expr(f, ndp);
}

/*
 * dump a function call
 */
static void dmp_fcallx(FILE *f, struct expr_t *ndp)
{
 int32 first_time;
 char paren;

 dmp_expr(f, ndp->lu.x);
 if (ndp->ru.x == NULL) return;

 paren = '(';

 __wrap_putc(paren, f);
 if (ndp->ru.x == NULL) return;
 /* know fcall operator left always unused */
 for (first_time = TRUE, ndp = ndp->ru.x; ndp != NULL; ndp = ndp->ru.x)
  {
   if (!first_time) __wrap_puts(", ", f); else first_time = FALSE;
   dmp_expr(f, ndp->lu.x);
  }
 if (paren == '<') __wrap_putc('>', f); else __wrap_putc(')', f);
 if (f == NULL) __exprline[__cur_sofs] = '\0';
}

/*
 * dump an event evor expression 
 * syntax allow ([edge] [expr] or ([normal expression]) and
 * ([event expr] or [event expr] or [] ) - stylized chain only 
 *
 * LOOKATME:
 * but associating to left for some reason - works since just list that
 * each of which gets added to dcelst as dcevnt
 * maybe needed because rhs section can be complicated expr. not just chain
 * as in concat case
 */
static void dmp_evor_chain(FILE *f, struct expr_t *ndp) 
{
 if (ndp->lu.x->optyp == OPEVOR || ndp->lu.x->optyp == OPEVCOMMAOR)
  {
   dmp_evor_chain(f, ndp->lu.x);
  }
 else dmp_expr(f, ndp->lu.x);

 if (ndp->lu.x->optyp == OPEVOR) __wrap_puts(" or ", f);
 else __wrap_puts(", ", f);

 dmp_expr(f, ndp->ru.x); 
}

/*
 * put a string for source statement dumping handles line wrap and
 * statement level indenting
 */
extern void __wrap_puts(char *s, FILE *f)
{
 int32 ll;

 ll = strlen(s);
 if (f == NULL)
  {
   if (__cur_sofs + ll >= __exprlinelen - 1) __chg_xprline_size(ll + 1);
   strcpy(&(__exprline[__cur_sofs]), s);
   __cur_sofs += ll;
   return;    
  }
 
 /* case 1: at beginning of line, just need statement indent */
 if (__outlinpos == 0)
  {
   if (__pv_stlevel > 0)
    {
     __blnkline[__pv_stlevel] = '\0'; 
     __cvsim_msg(__blnkline);
     __blnkline[__pv_stlevel] = ' '; 
    }
   /* always print s no matter how wide - cannot break inside objs */
   __outlinpos = __pv_stlevel + ll;
  
   __cvsim_msg("%s", s);
   return;
  }

 /* case 2: try continuation on current line */
 /* if string <= 2 make this line slightly too long (for ,[space]) */
 if ((__outlinpos += ll) > OUTLINLEN)
  {
   /* various ending punctuation (short fields) should go on same line */
   if ((ll > 3 || (!ispunct(s[0]) && s[0] != ' '))
    || __outlinpos > OUTLINLEN + 6)
    {
     __cvsim_msg("\n");

     __blnkline[__pv_stlevel + 1] = '\0'; 
     __cvsim_msg("%s", __blnkline);
     __blnkline[__pv_stlevel + 1] = ' '; 
     __outlinpos = __pv_stlevel + ll + 1;
    }
  }
 /* cannot use printf here since must emit format with % as is */
 __cvsim_msg("%s", s);
}

/*
 * source statement put char analog of put string above 
 */
extern void __wrap_putc(int32 c, FILE *f)
{
 if (f == NULL) { addch_(c); return; }

 if (__outlinpos == 0)
  {
   if (__pv_stlevel > 0)
    {
     __blnkline[__pv_stlevel] = '\0'; 
     __cvsim_msg("%s", __blnkline);
     __blnkline[__pv_stlevel] = ' '; 
    }
   __outlinpos = __pv_stlevel + 1;
   __cvsim_msg("%c", c);
   return;
  }

 /* if string <= 2 make this line slightly too long (for ,[space]) */
 if (++__outlinpos > OUTLINLEN)
  {
   /* various ending punctuation (short fields) should go on same line */
   if ((!ispunct(c) && c != ' ') || __outlinpos > OUTLINLEN + 6)
    {
     __cvsim_msg("\n");

     __blnkline[__pv_stlevel + 1] = '\0'; 
     __cvsim_msg("%s", __blnkline);
     __blnkline[__pv_stlevel + 1] = ' '; 
     __outlinpos = __pv_stlevel + 2;
    }
  }
 __cvsim_msg("%c", c);
}

/*
 * wrap form of puts that adds new line
 * notice macro does nothing if called with f == nil
 */
extern void __nl_wrap_puts(char *s, FILE *f)
{
 if (f != NULL)
  {
   if (*s != '\0') __wrap_puts(s, f);
   __wrap_putc('\n', f);
   __outlinpos = 0;
  }
 else __cur_sofs = 0;
}

/*
 * add string to current place in __exprline
 * notice this always leaves line null terminated but addch_ macros does not 
 */
extern void __adds(char *s)
{
 int32 slen;

 slen = strlen(s);
 if (__cur_sofs + slen >= __exprlinelen - 1) __chg_xprline_size(slen + 1);
 strcpy(&(__exprline[__cur_sofs]), s);
 __cur_sofs += slen; 
}

/*
 * change the length of a partially filled expr string line
 * if need to increase increase by needed amount plus large (1024) piece
 * requires that __exprline always be \0 terminated before calling this
 */
extern void __chg_xprline_size(int32 slen)
{
 int32 newlen;

 newlen = slen + ((__exprlinelen < 4*IDLEN) ? __exprlinelen + IDLEN
  : __exprlinelen + BIG_ALLOC_SIZE);
 __exprline = __my_realloc(__exprline, __exprlinelen, newlen);
 __exprlinelen = newlen;
}

/*
 * ROUTINES TO BUILD VPI_ ARGV ARRAY
 */

/*
 * convert opthdr from to vpi_ recursive argv
 *
 * only called once first time needed (vpi argv nil)
 * LOOKATME - this always copies and reallocates but maybe not needed
 */
extern void __bld_vpi_argv(void)
{
 register int32 lev, i;
 register struct optlst_t *olp;
 int32 maxlev, argnum, nbytes;
 struct optlst_t *bmark_olp;
 char **down_argv;

 maxlev = find_deepest_level(__opt_hdr);
 /* process bottom up replacing BMARK to EMARK with sub argv ** */
 for (lev = maxlev; lev >= 1; lev--) 
  {
   for (olp = __opt_hdr; olp != NULL; olp = olp->optlnxt)
    {
     if (olp->argv_done) continue;
     if (olp->optlev != lev || !olp->is_bmark) continue;

     /* found right level BMARK */
     argnum = cnt_beg_to_endmark(olp, olp->optlev);
     down_argv = (char **) __my_malloc(argnum*sizeof(char *));
     down_argv[0] = __pv_stralloc(__in_fils[olp->optfnam_ind]);
     /* starting one after bmark */
     bmark_olp = olp;
     for (olp = olp->optlnxt, i = 1;; olp = olp->optlnxt)
      {
       if (olp->argv_done) continue;
       if (olp->optlev != lev) __misc_terr(__FILE__, __LINE__);
       /* case 1: insert in down argv (first -f) */
       if (olp->is_argv)
        {
         if (!olp->is_bmark) __misc_terr(__FILE__, __LINE__);
         down_argv[i++] = __pv_stralloc("-f");
         down_argv[i++] = (char *) olp->dargv;
         olp->dargv = NULL;
         olp->is_argv = FALSE;
         olp->argv_done = TRUE;
         continue;
        }
       /* case 2: end mark - add nil terminator and done */
       if (olp->is_emark)
        {
         olp->argv_done = TRUE;
         down_argv[i++] = NULL;
         break;
        }
       /* case 3: normal option (must remove -f by itself) */
       if (strcmp(olp->opt, "-f") != 0)
        {
         down_argv[i++] = __pv_stralloc(olp->opt);
        }
       olp->argv_done = TRUE;
      }
     bmark_olp->dargv = down_argv;
     bmark_olp->is_argv = TRUE;
     bmark_olp->optlev--;
    }
  }
 /* top level is special case because of normal OS argc */
 /* special count that counts anything - no bmark-emark needed */
 argnum = cnt_level0(__opt_hdr);
 /* need 0th that is invoking Cver file name */
 __vpi_argc = argnum + 1;
 nbytes = __vpi_argc*(sizeof(char **));
 __vpi_argv = (char **) __my_malloc(nbytes);
 /* need to set simulator name from OS argv[0] */
 __vpi_argv[0] = __vpi_argv0;
 /* notice top uses argc - no ending nil */
 for (i = 1, olp = __opt_hdr; olp != NULL; olp = olp->optlnxt)
  {
   if (olp->argv_done) continue;
   /* case 1: down argv - need to add -f */
   if (olp->is_argv)
    {
     if (!olp->is_bmark) __misc_terr(__FILE__, __LINE__);
     __vpi_argv[i++] = __pv_stralloc("-f");
     __vpi_argv[i++] = (char *) olp->dargv;
     continue;
    }
   /* case 2: normal option (must reomve -f not followed by openable file) */ 
   if (strcmp(olp->opt, "-f") != 0)
    {
     __vpi_argv[i++] = __pv_stralloc(olp->opt);
    } 
  }
 /* DBG remove ---
 dump_vpi_argv(__vpi_argc, __vpi_argv);
 --- */
}

/*
 * find highest (deepest level) because convert bottom up to PLI **argv tabs
 */
static int32 find_deepest_level(struct optlst_t *olphd)
{
 register struct optlst_t *olp;
 int32 level;

 for (level = 0, olp = olphd; olp != NULL; olp = olp->optlnxt)
  { if (olp->optlev > level) level = olp->optlev; }
 return(level);
}

/*
 * count number of options from begin to end know olp points to beg
 * and ignores any argv_done elements
 *
 * level passed for checking - all counted must be at level lev or internal err
 * BMARK counted for file name and EMARK counted for ending NULL
 */
static int32 cnt_beg_to_endmark(struct optlst_t *olp, int32 lev)
{
 int32 onum;

 if (!olp->is_bmark) __arg_terr(__FILE__, __LINE__);
 olp = olp->optlnxt;
 for (onum = 1; olp != NULL; olp = olp->optlnxt)
  {
   if (olp->argv_done) continue;
   /* empty file will have count of 0 */
   if (olp->is_emark) return(onum + 1);
   if (olp->optlev != lev) __arg_terr(__FILE__, __LINE__);

   if (olp->is_argv) onum += 2;
   else
    {
     /* must not count -f not followed by openable file */
     if (strcmp(olp->opt, "-f") != 0) onum++;
    }
  }
 /* must always see EMARK */
 __arg_terr(__FILE__, __LINE__);
 return(-1);
}

/*
 * count top level (0) non argv done olps
 */
static int32 cnt_level0(struct optlst_t *olp)
{
 int32 onum;

 for (onum = 0; olp != NULL; olp = olp->optlnxt)
  {
   if (olp->argv_done) continue;
   /* only remaining must be level 0 */
   if (olp->optlev != 0) __arg_terr(__FILE__, __LINE__);
   if (olp->is_argv) onum += 2;
   else
    {
     /* must not count -f not followed by openable file */  
     if (strcmp(olp->opt, "-f") != 0) onum++;
    }
  }
 return(onum);
}

/*
 * dump a vpi argv/argc d.s.
 */
static void dump_vpi_argv(int32 argc, char **argv)
{
 register int32 i;

 __dbg_msg("TOP ARGC: %d\n", argc);
 for (i = 0; i < argc; i++) 
  {
   /* know if see -f, will be followed by sub argv */
   if (strcmp(argv[i], "-f") == 0)
    {
     __dbg_msg("LEVEL 0: arg %d: -f\n", i);
     __dbg_msg("LEVEL 0: arg %d: NESTED ARGV\n", i + 1);
     dump_nest_vpi_argv(1, (char **) argv[i + 1]);
     i++;
    }
   else __dbg_msg("LEVEL 0: arg %d: %s\n", i, argv[i]);
  }
}

/*
 * dump a contained nested file
 */
static void dump_nest_vpi_argv(int32 lev, char **argv)
{
 register int32 i;

 __dbg_msg("LEVEL %d: arg 0: file %s\n", lev, argv[0]);
 for (i = 1;; i++)
  {
   if (argv[i] == NULL) break;
   /* know if see -f, will be followed by sub argv */
   if (strcmp(argv[i], "-f") == 0)
    {
     __dbg_msg("LEVEL %d: arg %d: -f\n", lev, i);
     __dbg_msg("LEVEL %d: arg %d: NESTED ARGV\n", lev, i + 1);
     dump_nest_vpi_argv(lev + 1, (char **) argv[i + 1]);
     i++;
    }
   else __dbg_msg("LEVEL %d: arg %d: %s\n", lev, i, argv[i]);
  }
 __dbg_msg("LEVEL %d: **NULL**\n", lev);
}

/*
 * dump one oplst_t record
 */
static void dmp1_optlst(struct optlst_t *olp, char *emsg)
{
 char s1[RECLEN];

 if (olp->is_bmark && olp->is_emark) __misc_terr(__FILE__, __LINE__);
 if (olp->is_bmark) strcpy(s1, "BMARK");
 else if (olp->is_emark) strcpy(s1, "EMARK");
 else strcpy(s1, "NONE");
 __dbg_msg("**%s: file %s line %d: mark %s optnum %d optlev %d opt %s\n",
  emsg, __in_fils[olp->optfnam_ind], olp->optlin_cnt, s1, olp->optnum,
  olp->optlev, olp->opt);  
}
