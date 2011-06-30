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
 * module that fixes and checks net list after all source read
 * all defparam and global processing in v_grf
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __DBMALLOC__
#include "../malloc.h"
#endif

#include "v.h"
#include "cvmacros.h"

/* local prototypes */
static void try_cnvt_parmsel_toconst(struct expr_t *);
static void cnvt_paramsel_toconst(struct expr_t *, struct net_t *);
static void cnvt_nonis_selparm(struct net_t *, struct expr_t *,
 struct expr_t *);
static void cnvt_is_bselparm(struct net_t *, struct expr_t *, struct expr_t *);
static void cnvt_is_arrselparm(struct net_t *, struct expr_t *,
 struct expr_t *);
static int32 get_nonis_param_ndx(struct net_t *, struct expr_t *);
static int32 get_is_param_ndx(struct net_t *, struct expr_t *, int32);
static void cnvt_nonis_pselparm(struct net_t *, struct expr_t *,
 struct expr_t *, struct expr_t *);
static void cnvt_is_pselparm(struct net_t *, struct expr_t *, struct expr_t *,
 struct expr_t *);
static void chk_struct_rhsexpr(struct expr_t *, int32);
static int32 chk_rhs_id(struct expr_t *);
static void unwind_rhsconcats(struct expr_t *);
static void remove_0width_catrep_els(struct expr_t *);
static void set_rhswidth(struct expr_t *, int32);
static void set2_rhswidth(struct expr_t *, int32);
static void set_fcall_widths(struct expr_t *, struct task_t *);
static void set_sysfcall_widths(struct expr_t *);
static void set_rhs_signed(struct expr_t *);
static int32 find_xbase(struct expr_t *);
static void setchk_real_expr(struct expr_t *);
static void real_setchk_quest(struct expr_t *);
static void chk_specialop_rhsexpr(struct expr_t *);
static int32 chk_srcbsel(struct expr_t *, int32);
static int32 chk_srcpsel(struct expr_t *, int32);
static void chkspecop_fcall(struct expr_t *);
static int32 chk_mixedsign_relops(struct expr_t *);
static void fold_subexpr(struct expr_t *);
static int32 mark_constnd(register struct expr_t *, int32 *);
static void fold_const(struct expr_t *);
static int32 is_signed_decimal(struct expr_t *);
static void chk_ndfolded_specops(struct expr_t *);
static int32 chk_inrng_bsel(struct expr_t *);
static int32 chknorm_range(struct expr_t *, int32, int32, char *, int32);
static int32 in_range(int32, int32, int32, int32 *);
static int32 chk_inrng_psel(struct expr_t *);
static void chk_folded_fargs(struct expr_t *);
static int32 nd_ndxisnum(struct expr_t *, char *, int32);
static void chk_sysfargs_syntax(struct expr_t *);
static void chkbld_pli_func(struct expr_t *, int32);
static void chk_pli_arglist(register struct expr_t *, int32);
static int32 tfexpr_isrw(struct expr_t *);
static int32 lhs_is_decl(struct expr_t *);
static struct tfrec_t *chkalloc_tfrec(struct expr_t *, int32);
static struct tfarg_t *alloc_tfarg(register struct expr_t *, int32, int32);
static void chksf_count_drivers(struct expr_t *, int32);
static int32 is_1bwire(struct expr_t *);
static void chksf_getpattern(struct sy_t *, struct expr_t *, int32);
static void sf_errifn(struct sy_t *, int32);
static void chksf_q_full(struct expr_t *, int32);
static void chksf_rand(struct expr_t *, int32, int32);
static int32 chksyn_lhsexpr(struct expr_t *, int32, int32);
static void unwind_lhsconcats(struct expr_t *);
static int32 idnd_var(struct sy_t *);
static int32 nd_reg(struct expr_t *);
static int32 xpr_hasfcall(struct expr_t *);
static int32 xpr_has_nonsys_fcall(struct expr_t *);
static void chk_edge_expr(struct expr_t *);
static void chk_systskenable(struct st_t *, struct tskcall_t *);
static void mark_monit_in_src_nets(struct expr_t *);
static void chkbld_pli_task(struct st_t *, int32);
static void st_errif_rng(struct sy_t *, int32, int32, int32);
static void st_errifn(struct sy_t *, int32, int32);
static void chkst_dumpvars_enable(struct tskcall_t *, int32);
static void mark_mod_dvars_under(struct mod_t *, int32);
static void set_iact_dmpv_all_nd_nchgstore(void);
static void set_iact_dmpvmod_nd_nchgstore(struct mod_t *);
static void chkst_readmem(struct sy_t *, int32, struct tskcall_t *);
static int32 nd_unind_arr(struct expr_t *);
static void chkst_sreadmem(struct sy_t *, int32, struct tskcall_t *);
static void chkst_sdfannotate_enable(struct tskcall_t *, int32);
static int32 chk_undef_specparams(struct symtab_t *);
static int32 chk_1spcpth(struct spcpth_t *);
static int32 expr_has_nonpth(struct expr_t *, word32 *);
static void chk_rep_in_fullpth(struct spcpth_t *);
static char *pth_tostr(char *, struct spcpth_t *, struct pathel_t *,
 struct pathel_t *);
static char *pthel_tostr(char *, struct pathel_t *);
static int32 pth_overlap(struct pathel_t *, struct pathel_t *);
static void chk_rep_sdpds(struct spfy_t *);
static struct xpnd_pthel_t *xpnd_pths(struct spfy_t *, int32 *);
static void chk_spterm(struct expr_t *, char *, char *, int32);
static int32 chk_1tchk(struct tchk_t *);
static void chk_tccond(struct expr_t *, char *, char *);
static void chk_notifier(struct tchk_t *, char *);
static struct tchk_t *bld_sup_of_suphld(struct tchk_t *);
static struct tchk_t *bld_rec_of_recrem(struct tchk_t *);
static void free_spcpths(struct spcpth_t *);
static void free_frozen_symtab(struct symtab_t *);
static void free_spcparms(struct net_t *, int32);

/* extern prototypes (maybe defined in this module) */
extern char *__my_malloc(int32);
extern void __my_free(char *, int32);
extern char *__to_idnam(struct expr_t *);
extern char *__to_wtnam(char *, struct net_t *);
extern char *__to_sytyp(char *, word32);
extern char *__to_tcnam(char *, word32);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern char *__to_opname(word32);
extern struct expr_t *__copy_expr(struct expr_t *);
extern struct xstk_t *__eval2_xpr(struct expr_t *);
extern void __grow_xstk(void);
extern void __chg_xstk_width(struct xstk_t *, int32);
extern char *__msgnumexpr_tostr(char *, struct expr_t *, int32);
extern char *__msg2_blditree(char *, struct itree_t *);
extern struct paramlst_t *__copy_dellst(struct paramlst_t *);
extern char *__bld_lineloc(char *, word32, int32);
extern int32 __isleaf(struct expr_t *);
extern struct expr_t *__dup_concat(int32, struct expr_t *);
extern struct expr_t *__find_catend(struct expr_t *);
extern int32 __nd_ndxnum(struct expr_t *, char *, int32);
extern void __free2_xtree(struct expr_t *);
extern void __init_xnd(struct expr_t *);
extern void __set_numval(struct expr_t *, word32, word32, int32);
extern int32 __get_netwide(struct net_t *);
extern int32 __wide_vval_is0(register word32 *, int32);
extern int32 __is_paramconstxpr(struct expr_t *, int32);
extern void __rhspsel(register word32 *, register word32 *, register int32,
 register int32);
extern int32 __get_rhswidth(struct expr_t *);
extern int32 __get_widthclass(struct expr_t *);
extern void __narrow_sizchg(register struct xstk_t *, int32);
extern void __sizchg_widen(register struct xstk_t *, int32);
extern void __sgn_xtnd_widen(struct xstk_t *, int32);
extern void __getarr_range(struct net_t *, int32 *, int32 *, int32 *);
extern void __getwir_range(struct net_t *, int32 *, int32 *);
extern int32 __cnt_tfargs(register struct expr_t *);
extern int32 __chk_lhsexpr(struct expr_t *, int32);
extern void __chkbld_vpi_systf_func(struct expr_t *);
extern void __pli_func_sizetf(struct expr_t *);
extern void __set_lhswidth(struct expr_t *);
extern int32 __nd_wire(struct expr_t *);
extern int32 __chk_lhsdecl_scalared(struct expr_t *);
extern void __set_expr_onrhs(struct expr_t *);  
extern int32 __is_scope_sym(struct sy_t *); 
extern void __chk_fmt(register struct expr_t *, byte *); 
extern void __chkbld_vpi_systf_task(struct st_t *);  
extern void __set_xtab_errval(void);
extern void __bld_xtree(int32);
extern void __xtract_wirng(struct expr_t *, struct net_t **, int32 *, int32 *);
extern void __free_xtree(struct expr_t *);
extern void __chk_spec_delay(struct expr_t *, char *);
extern void __free_dellst(struct paramlst_t *);
extern int32 __chk_numdelay(struct expr_t *, char *);
extern void __init_tchk(struct tchk_t *, word32);
extern void __free_tchks(struct tchk_t *);
extern void __free_xprlst(struct exprlst_t *);
extern int32 __expr_has_glb(struct expr_t *);
extern int32 __alloc_is_cval(int32);
extern int32 __allocfill_cval_new(word32 *, word32 *, int32);
extern int32 __alloc_shareable_cval(word32, word32, int32);
extern int32 __alloc_shareable_rlcval(double);
extern struct expr_t *__widen_unsiz_rhs_assign(struct expr_t *, int);

extern void __pv_warn(int32, char *,...);
extern void __sgfwarn(int32, char *, ...);
extern void __gfinform(int32, word32, int32, char *, ...);
extern void __gferr(int32, word32, int32, char *, ...);
extern void __sgferr(int32, char *, ...);
extern void __ia_err(int32, char *, ...);
extern void __dbg_msg(char *, ...);
extern void __sgfinform(int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);
extern void __misc_sgfterr(char *, int32);

extern struct opinfo_t __opinfo[];
extern word32 __masktab[];

/*
 * ROUTINES TO PROCESS AND CHECK RHS EXPRESSIONS
 */

/*
 * routine to check a rhs expr.
 * order must be structural, fold all constant (folds params), and
 * finally check all embedded selects
 *
 * all there is to rhs expression checking
 */
extern int32 __chk_rhsexpr(struct expr_t *ndp, int32 csiz)
{
 int32 saverr_cnt;

 saverr_cnt = __pv_err_cnt;
 /* intercept and convert any selects from paramters to num or is num */ 
 /* this finds all in expr. - returns F if error */ 
 if (!__isleaf(ndp))
  {
   try_cnvt_parmsel_toconst(ndp);
   if (saverr_cnt != __pv_err_cnt) return(FALSE);
  }

 /* this can also be called before parameter values (fixed) */
 /* substitution to real operator done here */
 chk_struct_rhsexpr(ndp, csiz);
 /* in case expr. contains declared non wire symbol cannot try to fold */
 if (saverr_cnt != __pv_err_cnt) return(FALSE);

 /* emit warning for word32 relations comparisons - needed before folding */
 chk_mixedsign_relops(ndp);

 /* LOOKATME - is here a problem folding analog expressions */
 /* notice that width know everywhere - so if correct all folding right */
 /* if error still to be caught, will make reasonable guess for width */
 fold_subexpr(ndp);
 /* after folding need checking that requires fold constants */
 /* for selects and function call arguments */
 chk_ndfolded_specops(ndp);
 if (ndp->is_real) ndp->ibase = BDBLE;

 if (saverr_cnt != __pv_err_cnt) return(FALSE);
 return(TRUE);
}

/*
 * convert all selects from parameters into constants
 */
static void try_cnvt_parmsel_toconst(struct expr_t *ndp)
{
 struct net_t *np;

 if (__isleaf(ndp)) return;
 if (ndp->optyp == LSB)
  {
   np = ndp->lu.x->lu.sy->el.enp;
   if (np->n_isaparam && !np->nu.ct->p_specparam)
    cnvt_paramsel_toconst(ndp, np);
   else try_cnvt_parmsel_toconst(ndp->ru.x);
   return;
  }
 if (ndp->optyp == PARTSEL)
  {
   np = ndp->lu.x->lu.sy->el.enp;
   if (np->n_isaparam && !np->nu.ct->p_specparam)
    cnvt_paramsel_toconst(ndp, np);
   else
    {
     try_cnvt_parmsel_toconst(ndp->ru.x->lu.x);
     try_cnvt_parmsel_toconst(ndp->ru.x->ru.x);
    }
   return;
  }

 if (ndp->lu.x != NULL) 
  {
   if (!__isleaf(ndp->lu.x)) try_cnvt_parmsel_toconst(ndp->lu.x); 
  }
 if (ndp->ru.x != NULL) 
  {
   if (!__isleaf(ndp->ru.x)) try_cnvt_parmsel_toconst(ndp->ru.x); 
  }
}

/*
 * intercept before any checking and convert to number or IS number
 *
 * returns F on error - must set width and handle all checking
 * since no more processing for this rhs expr.
 * works because nothing under here - changes guts and under of ndp to num.
 * know current module in cur mod
 *
 * LOOKATME - now handle constant selects from parameter arrays
 */
static void cnvt_paramsel_toconst(struct expr_t *ndp, struct net_t *np)
{
 struct expr_t *ndx1, *ndx2;
 char selnam[RECLEN], s1[RECLEN];

 /* parameter array index ok for reals, anything else is error */
 if (np->ntyp == N_REAL && !np->n_isarr)
  {
   __sgferr(896, "bit or part select from real parameter %s illegal",
    np->nsym->synam);
   return;
  }
 if (np->n_isarr && ndp->optyp == PARTSEL)
  {
   __sgferr(896, "part select from parameter array %s illegal",
    np->nsym->synam);
   return;
  }

 if (np->n_isarr) strcpy(selnam, "array");
 else if (np->n_isavec) strcpy(selnam, "bit");
 else
  {
   __sgferr(896, "select from scalar parameter %s illegal", np->nsym->synam);
   return;
  }
 /* step 1: check and convert indices to 32 bit values */
 if (ndp->optyp == LSB)
  {
   /* index must be correct rhs expr. */
   ndx1 = ndp->ru.x;
   ndx2 = NULL;
   if (!__chk_rhsexpr(ndx1, 0)) return;
   if (ndx1->optyp != NUMBER && ndx1->optyp != ISNUMBER)
    {
     /* SJM 11/14/03 - must allow variable bsel from param during sim */
     if (!np->n_isarr)
      {
       return;
      }
     else 
      {
       __sgferr(926,
        "parameter %s select %s[%s] index must be constant expression",
        selnam, np->nsym->synam, __msgexpr_tostr(__xs, ndx1));
      } 
     return;
    }
   /* convert to either number or is number, 32 bits - maybe x */
   sprintf(s1, "parameter %s select index", selnam);
   __nd_ndxnum(ndx1, s1, FALSE);
  }
 else if (ndp->optyp == PARTSEL)
  {
   ndx1 = ndp->ru.x->lu.x;
   if (!__chk_rhsexpr(ndx1, 0)) return;
   __nd_ndxnum(ndx1, "parmeter part select first index",
    FALSE);
   ndx2 = ndp->ru.x->ru.x;
   if (!__chk_rhsexpr(ndx2, 0)) return;
   __nd_ndxnum(ndx2, "parameter part select second index",
    FALSE);
   if (ndx1->optyp != NUMBER && ndx1->optyp != ISNUMBER
    && ndx2->optyp != NUMBER && ndx2->optyp != ISNUMBER)
    {
     __sgferr(926,
      "parameter %s part select %s both indices must be constant expressions",
      np->nsym->synam, __msgexpr_tostr(__xs, ndp));
     return;
    }
  }
 else
  {
   __sgferr(822,
    "bit select from parameter %s index expression non constant",
    np->nsym->synam);
   return;
  }
 if (ndp->optyp == LSB)
  {
   if (ndx1->optyp == NUMBER && np->srep == SR_PNUM)
    { cnvt_nonis_selparm(np, ndp, ndx1); return; }
   if (np->n_isarr) cnvt_is_arrselparm(np, ndp, ndx1);
   else cnvt_is_bselparm(np, ndp, ndx1);
   return;
  }
 /* these return F if out of range or x for either index */
 if (ndx1->optyp == NUMBER && ndx2->optyp == NUMBER && np->srep == SR_PNUM)
  cnvt_nonis_pselparm(np, ndp, ndx1, ndx2);
 else cnvt_is_pselparm(np, ndp, ndx1, ndx2);
}

/*
 * convert a parameter non IS bit select to 1 bit constant
 * or convert parameter array select to value
 */
static void cnvt_nonis_selparm(struct net_t *np, struct expr_t *ndp,
 struct expr_t *ndx)
{
 int32 bi, wlen;
 word32 *wp, av, bv;
 struct xstk_t *xsp;
 
 bi = get_nonis_param_ndx(np, ndx);
 if (!np->n_isarr)
  {
   if (bi == -1) av = bv = 1L;
   else
    {
     wlen = wlen_(np->nwid);
     wp = np->nva.wp;
     av = rhsbsel_(wp, bi);
     bv = rhsbsel_(&(wp[wlen]), bi); 
    }
   __free2_xtree(ndp);
   __init_xnd(ndp);
   __set_numval(ndp, av, bv, 1);
  }
 else
  {
   wlen = wlen_(np->nwid);
   __free2_xtree(ndp);
   __init_xnd(ndp);
   ndp->optyp = NUMBER;
   ndp->szu.xclen = np->nwid;

   /* parameter arrays never packed */
   if (bi == -1)
    {
     if (np->nwid == WBITS)
      {
       ndp->ru.xvi = __alloc_shareable_cval(ALL1W, ALL1W, np->nwid);
      }
     else
      { 
       push_xstk_(xsp, np->nwid);
       one_allbits_(xsp->ap, np->nwid);
       one_allbits_(xsp->bp, np->nwid);
       ndp->ru.xvi = __allocfill_cval_new(xsp->ap, xsp->bp, wlen);
       __pop_xstk();
      }
    }
   else
    {
     wp = &(np->nva.wp[2*wlen*bi]);
     if (np->nwid <= WBITS)
      {
       ndp->ru.xvi = __alloc_shareable_cval(wp[0], wp[1], np->nwid);
      }
     else
      {
       ndp->ru.xvi = __allocfill_cval_new(wp, &(wp[wlen]), wlen);
      }
    }
  }
 ndp->consubxpr = TRUE;
 ndp->folded = TRUE;
}

/*
 * convert a parameter IS bit select to 1 bit constant
 * know result is an IS constant
 */
static void cnvt_is_bselparm(struct net_t *np, struct expr_t *ndp,
 struct expr_t *ndx)
{
 register int32 ii;
 int32 bi, nbytes, wlen, tmpxvi;
 word32 *wp, *wp2, av, bv;

 /* bit select will be 1 bit IS constant */
 nbytes = __inst_mod->flatinum*2*WRDBYTES;  

 /* must not change ndp since still need to access ndx values */
 /* passes number of words in a part alloc multiplies by 2 for b part too */
 tmpxvi = __alloc_is_cval(__inst_mod->flatinum);
 wp = &(__contab[tmpxvi]);
 memset(wp, 0, nbytes); 
 
 wlen = wlen_(np->nwid);
 for (ii = 0; ii < __inst_mod->flatinum; ii++) 
  {  
   if (ndx->optyp == NUMBER) bi = get_nonis_param_ndx(np, ndx);
   else if (ndx->optyp == ISNUMBER) bi = get_is_param_ndx(np, ndx, ii); 
   else { __case_terr(__FILE__, __LINE__); bi = 0; }
   if (bi == -1) av = bv = 1L;
   else
    {
     wp2 = np->nva.wp;
     if (np->srep == SR_PISNUM) wp2 = &(wp2[2*wlen*ii]);
     av = rhsbsel_(wp2, bi);
     bv = rhsbsel_(&(wp2[wlen]), bi); 
    }
   wp[2*ii] = av;
   wp[2*ii + 1] = bv;
  }

 /* notice can't free ndp because need to acc ndx (that is contained in it) */
 __free2_xtree(ndp);
 __init_xnd(ndp);
 /* create IS number expr node and then fill it */
 ndp->optyp = ISNUMBER;
 ndp->szu.xclen = 1;
 ndp->ru.xvi = tmpxvi;
 ndp->consubxpr = TRUE;
 ndp->consub_is = TRUE;
 ndp->folded = TRUE;
}

/*
 * convert a parameter IS array select to constant
 * know result is an IS constant
 */
static void cnvt_is_arrselparm(struct net_t *np, struct expr_t *ndp,
 struct expr_t *ndx)
{
 register int32 ii;
 register word32 *wp, *wp2;
 int32 bi, wsiz, wlen, tmpxvi;

 wlen = wlen_(np->nwid);
 wsiz = __inst_mod->flatinum*wlen;  
 /* must not change ndp since still need to access ndx values */
 tmpxvi = __alloc_is_cval(wsiz);
 wp = &(__contab[tmpxvi]);
 memset(wp, 0, 2*WRDBYTES*wsiz); 

 for (ii = 0; ii < __inst_mod->flatinum; ii++) 
  {  
   if (ndx->optyp == NUMBER) bi = get_nonis_param_ndx(np, ndx);
   else if (ndx->optyp == ISNUMBER) bi = get_is_param_ndx(np, ndx, ii); 
   else { __case_terr(__FILE__, __LINE__); bi = 0; }

   /* array select is 2*wlen value */
   wp2 = np->nva.wp;
   if (np->srep == SR_PISNUM) wp2 = &(wp2[2*wlen*ii]);
   
   if (bi == -1) 
    {
     one_allbits_(&(wp[2*wlen*ii]), np->nwid);
     one_allbits_(&(wp[2*wlen*ii + wlen]), np->nwid);
    }
   else memcpy(&(wp[2*wlen*ii]), wp2, 2*WRDBYTES*wlen);
  }

 __free2_xtree(ndp);
 __init_xnd(ndp);
 ndp->optyp = ISNUMBER;
 ndp->szu.xclen = np->nwid;
 ndp->ru.xvi = tmpxvi;
 ndp->consubxpr = TRUE;
 ndp->consub_is = TRUE;
 ndp->folded = TRUE;
}

/*
 * access a non IS parameter index expr.
 * SJM - 08/23/00 - allow rng on params - rng h:0 if no declared range
 */
static int32 get_nonis_param_ndx(struct net_t *np, struct expr_t *ndx)
{
 register word32 *wp;
 register int32 bi;
 int32 ri1, ri2, awid, bi1;

 wp = &(__contab[ndx->ru.xvi]);
 /* if x/z, warning already emitted but must return -1 */
 if (wp[1] != 0L) { bi = -1; return(bi); }
 
 /* SJM 09/11/00 - SJM - must check for unnormalized range */
 if (np->n_isarr) __getarr_range(np, &ri1, &ri2, &awid);
 else __getwir_range(np, &ri1, &ri2);
 if (ri1 >= ri2)
  {
   if (wp[0] > ri1 || wp[0] < ri2) 
    {
bad_rng:
     __sgfwarn(586,
      "parameter %s select index %d out of range [%d:%d]",
      np->nsym->synam, (int32) wp[0], ri1, ri2);
     bi = -1;
     return(bi);
    }
  }
 else { if (wp[0] < ri1 || wp[0] > ri2) goto bad_rng; }
 bi1 = (int32) wp[0];
 bi = normalize_ndx_(bi1, ri1, ri2);
 return(bi);
}

/*
 * access an IS parameter index expr.
 * need special routine since no range for parameters
 * know since index converted to 32 bit IS form before here
 */
static int32 get_is_param_ndx(struct net_t *np, struct expr_t *ndx, int32 ii)
{
 register int32 bi;
 register word32 *wp;
 int32 bi1, ri1, ri2, awid;
 word32 wval;
 
 wp = &(__contab[ndx->ru.xvi]);
 /* if x/z, warning already emitted */
 if (wp[2*ii + 1] != 0L) { bi = -1; return(bi); }

 /* SJM 09/11/00 - SJM - must check for unnormalized range */
 if (np->n_isarr) __getarr_range(np, &ri1, &ri2, &awid);
 else __getwir_range(np, &ri1, &ri2);
 wval = wp[2*ii];
 if (ri1 >= ri2)
  {
   if (wval > ri1 || wval < ri2) 
    {
bad_rng:
     __sgfwarn(586,
      "parameter %s (instance %s) select index %d out of range [%d:%d]",
       np->nsym->synam, __msg2_blditree(__xs, __inst_mod->moditps[ii]),
       wval, ri1, ri2);
     bi = -1;
     return(bi);
    }
  }
 else { if (wval < ri1 || wval > ri2) goto bad_rng; }
 /* good range */
 bi1 = (int32) wval;
 bi = normalize_ndx_(bi1, ri1, ri2);
 return(bi);
}

/*
 * convert a parameter non IS part select to psel width constant
 * constants are stored in minimum 8 byte values
 */
static void cnvt_nonis_pselparm(struct net_t *np, struct expr_t *ndp,
 struct expr_t *ndx1, struct expr_t *ndx2)
{
 int32 bi1, bi2, pselwid, wlen;
 word32 *wp2;
 struct xstk_t *xsp;
 
 /* parameters always range [(width - 1) to 0] */
 bi1 = get_nonis_param_ndx(np, ndx1);
 bi2 = get_nonis_param_ndx(np, ndx2);
 if (bi1 == -1 || bi2 == -1)
  {
   __sgferr(1045,
    "parameter part select %s (width %d) index(s) x/z, out of range",
    __msgexpr_tostr(__xs, ndp), np->nwid);
   return;
  }
 if (bi1 < bi2) pselwid = bi2 - bi1 + 1;
 else pselwid = bi1 - bi2 + 1;

 __free2_xtree(ndp);
 __init_xnd(ndp);
 ndp->optyp = NUMBER;
 ndp->szu.xclen = pselwid;
 ndp->consubxpr = TRUE;
 ndp->folded = TRUE;

 /* can't use shareable here becuase need to psel to set value */
 push_xstk_(xsp, pselwid);
 wlen = wlen_(np->nwid);
 wp2 = np->nva.wp; 
 __rhspsel(xsp->ap, wp2, bi2, pselwid);
 __rhspsel(xsp->bp, &(wp2[wlen]), bi2, pselwid);
 ndp->ru.xvi = __allocfill_cval_new(xsp->ap, xsp->bp, wlen_(pselwid));
 __pop_xstk();
 return;
}

/*
 * convert a parameter IS bit select to 1 bit constant
 * know result is an IS constant
 */
static void cnvt_is_pselparm(struct net_t *np, struct expr_t *ndp,
 struct expr_t *ndx1, struct expr_t *ndx2)
{
 register int32 ii;
 int32 bi1, bi2, xwlen, wlen, wlen2, err, pselwid, tmp, tmpxvi;
 word32 *wp, *wp2;

 pselwid = 0;
 for (err = FALSE, ii =  0; ii < __inst_mod->flatinum; ii++) 
  {
   /* parameters always range [(width - 1) to 0] */
   if (ndx1->optyp == NUMBER) bi1 = get_nonis_param_ndx(np, ndx1);
   else bi1 = get_is_param_ndx(np, ndx1, ii); 
   if (ndx2->optyp == NUMBER) bi2 = get_nonis_param_ndx(np, ndx2);
   else bi2 = get_is_param_ndx(np, ndx2, ii); 
   if (bi1 == -1 || bi2 == -1)
    {
     __sgferr(1045,
      "parameter part select %s (width %d) (instance %s) index(s) x/z, out of range",
      __msgexpr_tostr(__xs, ndp), np->nwid, __msg2_blditree(__xs,
     __inst_mod->moditps[ii]));
     err = TRUE;
    }
   if (bi1 < bi2) tmp = bi2 - bi1 + 1;
   else tmp = bi1 - bi2 + 1;
   /* constant is widest in case IS form indices */
   if (tmp > pselwid) pselwid = tmp;
  }
 if (err) return;

 xwlen = __inst_mod->flatinum*wlen_(pselwid);  
 /* must not change ndp since still need to access ndx values */
 tmpxvi = __alloc_is_cval(xwlen);
 wp = &(__contab[tmpxvi]);
 memset(wp, 0, 2*xwlen*WRDBYTES);
 
 wlen = wlen_(pselwid);
 wlen2 = wlen_(np->nwid);
 for (ii =  0; ii < __inst_mod->flatinum; ii++) 
  {  
   if (ndx1->optyp == NUMBER) bi1 = get_nonis_param_ndx(np, ndx1);
   else bi1 = get_is_param_ndx(np, ndx1, ii); 
   if (ndx2->optyp == NUMBER) bi2 = get_nonis_param_ndx(np, ndx2);
   else bi2 = get_is_param_ndx(np, ndx2, ii); 

   wp2 = np->nva.wp;
   if (np->srep == SR_PISNUM) wp2 = &(wp2[2*wlen2*ii]);

   tmp = bi1 - bi2 + 1;
   /* can part select into section of constant providing starts at 0 */
   /* tmp can be narrower than psel wid */
   __rhspsel(&(wp[2*wlen*ii]), wp2, bi2, tmp);
   __rhspsel(&(wp[2*wlen*ii + wlen]), &(wp2[wlen2]), bi2, tmp);
  }

 __free2_xtree(ndp);
 __init_xnd(ndp);
 ndp->optyp = ISNUMBER;
 ndp->szu.xclen = pselwid;
 ndp->ru.xvi = tmpxvi;
 ndp->consubxpr = TRUE;
 ndp->consub_is = TRUE;
 ndp->folded = TRUE;
 return;
}

/*
 * SOURCE (STRUCTURAL) RHS EXPRESSION CHECKING
 */

/*
 * check gate/inst array index parameter expression
 * return F on error (i.e. no constant)
 *
 * special case because needed before parameters set to final values
 * if wider than 32 bits or has x/zs caught after expr. eval
 */
extern int32 __chk_giarr_ndx_expr(struct expr_t *ndp)
{
 chk_struct_rhsexpr(ndp, 0);
 /* emit warning for word32 relations comparisons */
 chk_mixedsign_relops(ndp);

 /* real not allowed */
 if (__is_paramconstxpr(ndp, FALSE)) return(TRUE);
 return(TRUE);
}

/*
 * check either defparam or specparam rhs expr.
 * cannot go through chk rhs expr here since some rhs stuff not allowed
 */
extern int32 __chk_paramexpr(struct expr_t *ndp, int32 xwid)
{
 /* FIXME - need to allow const user function calls in param rhs exprs */
 /* SJM 03/13/00 - now built-in sys funcs ok - chk later for non const args */
 if (xpr_has_nonsys_fcall(ndp)) return(FALSE);

 chk_struct_rhsexpr(ndp, xwid);
 if (ndp->is_real) ndp->ibase = BDBLE;
 /* emit warning for word32 relations comparisons */
 chk_mixedsign_relops(ndp);

 if (__is_paramconstxpr(ndp, TRUE)) return(TRUE);
 return(FALSE);
}

/*
 * return T if parameter constant expr. - else F (for ID)
 * if expr. contains anything but numbers and correct type of param return F
 *
 * this is for both def params and spec params 
 * notice for spec param can never be IS form
 */
extern int32 __is_paramconstxpr(struct expr_t *ndp, int32 realok)
{
 int32 rv1, rv2;
 struct sy_t *syp;
 struct net_t *np;
 struct expr_t *fandp;

 switch ((byte) ndp->optyp) { 
  case NUMBER: return(TRUE);
  case REALNUM:
   if (!realok) return(FALSE);
   return(TRUE);

  /* can only make ISNUMBER after def/# param assignment */ 
  case ISNUMBER: case ISREALNUM:
   __arg_terr(__FILE__, __LINE__);
   break;
  case ID:
   /* previously defined parameter ok */ 
   syp = ndp->lu.sy;
   if (!syp->sydecl) return(FALSE);
   np = syp->el.enp; 
   if (np->n_isaparam)
    {
     /* SJM 02/04/00 - error if parameter used on RHS of specparam */
     if (__cur_declobj == SPECIFY && !np->nu.ct->p_specparam) return(FALSE);
     return(TRUE);
    }
   return(FALSE);
  case GLBREF: return(FALSE);
  /* SJM - 08/22/00 - selects from bit and part selects need to work */ 
  /* case LSB: case PARTSEL: */
  /* return(FALSE); */
  case FCALL:
   /* getting output width, so have right context elsewhere */
   /* do not need to get arg. width since call shields */
   syp = ndp->lu.x->lu.sy;
   if (syp->sytyp != SYM_SF) return(FALSE);
   if (syp->el.esyftbp->tftyp != SYSF_BUILTIN) return(FALSE);
   if (!realok)
    { if (syp->el.esyftbp->retntyp == N_REAL) return(FALSE); }
   /* SJM 09/04/01 - for special constant sys funcs must check arg num here */ 
   /* args must also be constant - first in expr list is return expr */
   for (fandp = ndp->ru.x; fandp != NULL; fandp = fandp->ru.x)
    {
     /* LOOKATME - even if real param not allowed arg to const systf */
     /* can be real */
     if (!__is_paramconstxpr(fandp->lu.x, TRUE)) return(FALSE);
    }
   return(TRUE);
 } 
 rv1 = rv2 = TRUE;
 if (ndp->lu.x != NULL) rv1 = __is_paramconstxpr(ndp->lu.x, realok);
 if (ndp->ru.x != NULL) rv2 = __is_paramconstxpr(ndp->ru.x, realok);
 if (rv1 && rv2) return(TRUE); 
 return(FALSE);
}

/*
 * check a rhs expression for structural correctness
 *
 * only needs info available during pass 1
 * all rhs expressions must be processed through here
 * needs __sfnam_ind to be set before called
 *
 * this checks and changes structural things but does not
 * do any evaluating or substituting or folding since needed during
 * source input to determine parameters
 *
 * when done all node widths sets, all real operators substituted
 * and all concatenates 1 level with no repeat forms  
 * if error that will prevent simulation, emitted here
 *
 * not done: part select and bit select normalization and range checking
 * constant folding, conversion to WBITS values for indices and ranges
 */
static void chk_struct_rhsexpr(struct expr_t *ndp, int32 xwid)
{
 /* handle all leaves (special fast cases) */
 switch ((byte) ndp->optyp) {
  case OPEMPTY: return;
  case NUMBER: case ISNUMBER: return;
  case REALNUM: case ISREALNUM:
   ndp->is_real = TRUE;
   /* reals always signed */
   ndp->has_sign = TRUE;
   __expr_has_real = TRUE;
   return;
  case ID: case GLBREF:
   /* here must set width if possible */
   if (chk_rhs_id(ndp)) ndp->szu.xclen = __get_netwide(ndp->lu.sy->el.enp);
   else ndp->szu.xclen = 1;
   return;
 }
 /* must next unwind all concatenates in expr. bottom up */
 /* result is 1 level only simple concats */
 unwind_rhsconcats(ndp);

 /* SJM 09/11/03 - because XL/NC support 0 width concat elements, must */
 /* remove any 0 width one from concat here - if now empty syntax error */ 
 if (__nd_0width_catel_remove)
  {
   remove_0width_catrep_els(ndp);
   __nd_0width_catel_remove = FALSE;
  }

 __expr_has_real = FALSE;
 /* set widths and turn on flag if any reals in expr. */
 /* also sets is_real for real constants and variables */
 set_rhswidth(ndp, xwid);

 if (__expr_has_real) setchk_real_expr(ndp);

 /* 10/02/03 - use new 2001 LRM rules to set expr has sign bit */
 set_rhs_signed(ndp);

 /* check any special (bsel, psel, fcall, etc.) in rhs expr. */
 /* this leave bit select and part select expressions as is */
 /* must normalize and convert to WBITS number some where is const. */
 chk_specialop_rhsexpr(ndp);
}

/*
 * check a rhs identifier 
 * for global checks target
 */
static int32 chk_rhs_id(struct expr_t *ndp)
{
 struct net_t *np;
 struct sy_t *syp;

 syp = ndp->lu.sy;
 /* cross module reference not yet resolved - caller will emit error */
 if (syp == NULL) return(FALSE);

 if (!syp->sydecl) return(FALSE);

 if (syp->sytyp != SYM_N)
  {
  __sgferr(1040, "%s %s illegal in right side expression - must be variable",   
   __to_sytyp(__xs, syp->sytyp), __to_idnam(ndp));
   return(FALSE);
  }

 /* this check for illegal expr. unindexed array and event */
 /* params and real marking handled here also */
 np = syp->el.enp;
 if (np->ntyp == N_EVENT)
  {
   __sgferr(893, "event %s illegal in right hand side expression",
    __to_idnam(ndp));
   return(FALSE);
  }
 if (np->n_isarr)
  {
   __sgferr(894,
    "illegal unindexed array reference of %s in right hand side expression",
    __to_idnam(ndp));
   return(FALSE);
  }
 /* any rhs wire expr. node must be signed if wire is */
 if (np->n_signed) ndp->has_sign = TRUE;
 if (np->n_stren) ndp->x_stren = TRUE;
 if (np->ntyp == N_REAL)
  { ndp->is_real = TRUE; ndp->has_sign = TRUE; __expr_has_real = TRUE; }
 if (np->nrngrep == NX_CT) np->nu.ct->n_onrhs = TRUE;
 if (!__expr_rhs_decl) np->n_onprocrhs = TRUE;
 return(TRUE);
}

/*
 * check and unwind all rhs concatenates
 *
 * this will create any repeat form concatenates it needs to
 * when done concatenate is one level with catreps removed
 * concatenate constant folding if implemented must be done after here 
 *
 * notice this will unwind concatenates in fcall arguments
 */
static void unwind_rhsconcats(struct expr_t *ndp)
{
 if (__isleaf(ndp)) return;

 /* DBG ---
 if (__debug_flg)
  __dbg_msg("## unwinding concat %s\n", __msgexpr_tostr(__xs, ndp));
 --- */

 /* concatenate unwinding must be done bottom up */
 if (ndp->lu.x != NULL)
  {
   /* DBG ---
   if (__debug_flg)
   __dbg_msg("## left side %s\n", __msgexpr_tostr(__xs, ndp->lu.x));
   --- */ 
   unwind_rhsconcats(ndp->lu.x);
   /* DBG ---
   if (__debug_flg) __dbg_msg("^^ left up.\n");
   --- */
  }
 if (ndp->ru.x != NULL)
  {
   /* DBG ---
   if (__debug_flg)
   __dbg_msg("## right side %s\n", __msgexpr_tostr(__xs, ndp->ru.x));
   --- */
   unwind_rhsconcats(ndp->ru.x);
   /* DBG ---
   if (__debug_flg) __dbg_msg("^^ right up.\n");
   --- */ 
  }

 /* node is top of concatenate with all sub concatenates simplified */
 if (ndp->optyp == LCB)
  {
   register struct expr_t *ndp2;
   struct expr_t *last_ndp2, *end_ndp;

   last_ndp2 = ndp;
   for (ndp2 = ndp->ru.x; ndp2 != NULL; ndp2 = ndp2->ru.x)
    {
     struct expr_t *lop;

     lop = ndp2->lu.x;
     /* notice ,, form illegal in concatentate */
     switch ((byte) lop->optyp) {
      case NUMBER: case ISNUMBER:
       if (lop->unsiznum)
        __sgferr(897, "unsized number illegal in concatenate");
       break;
      case REALNUM: case ISREALNUM:
       __sgferr(813, "real number illegal in concatenate");
       break;
      case LCB:
       {
        /* nested concatenate - splice up one level */
        last_ndp2->ru.x = lop->ru.x;
        /* find rightmost element - know always there */
        end_ndp = __find_catend(lop);
        /* if rightmost up one node will make null */
        end_ndp->ru.x = ndp2->ru.x;
        /* end of new chain is new last */
        last_ndp2 = end_ndp;
       }
       continue;
      case CATREP:
       {
        int32 repval;
        struct expr_t *dupndp;
        struct xstk_t *xsp;

        if (__chk_paramexpr(lop->lu.x, 0))
         {
          xsp = __eval_xpr(lop->lu.x);
          if (xsp->xslen > WBITS)
           {
            if (!vval_is0_(&(xsp->ap[1]), xsp->xslen - WBITS) 
             || !vval_is0_(&(xsp->bp[1]), xsp->xslen - WBITS)
             || xsp->bp[0] != 0L) goto bad_rep;
           }
          else if (xsp->bp[0] != 0) goto bad_rep;
          
          repval = (int32) xsp->ap[0];
          /* SJM 09/11/03 - also need error for negative */
          if (repval < 0) goto bad_rep;
          else if (repval == 0)
           {
            __sgfwarn(3109,
             "concatenate repeat value of 0 causes removal of concatenate");
           }
          else if (repval == 1) 
           __sgfinform(442, "concatenate repeat value of 1 has no effect");
         }
        else
         {
bad_rep:
          __sgferr(814,
           "concatenate repeat value %s not a non negative constant expression",
           __msgexpr_tostr(__xs, lop->lu.x));
          repval = 1;
         }

        __pop_xstk();
        if (repval == 0)
         {
          /* SJM 09/11/03 - contradicts 2001 LRM but 0 width removed */
          /* from concatenate, but because of possibility of recursive */
          /* upward removal must insert size 1 and remove later */
          /* and if remove creates empty concat, then emit syntax error */ 
          dupndp = __dup_concat(1, lop->ru.x->ru.x);
          dupndp->folded = TRUE;
          __nd_0width_catel_remove = TRUE;
         }
        else
         {
          /* know the rhs thing must be a concatenate */
          dupndp = __dup_concat(repval, lop->ru.x->ru.x);
         }

        /* nested concatenate - splice up one level */
        last_ndp2->ru.x = dupndp;
        /* find rightmost element - know always there */
        end_ndp = __find_catend(dupndp);

        /* if rightmost up one node will make null */
        end_ndp->ru.x = ndp2->ru.x;
        /* end of new chain is new last */
        last_ndp2 = end_ndp;
       }
       continue;
      }
     /* NUMBER or other means move last down tree one */
     last_ndp2 = ndp2;
    }
  }
}

/*
 * return TRUE if node is leaf node (id, glbid, number)
 */
extern int32 __isleaf(struct expr_t *ndp)
{
 switch ((byte) ndp->optyp) {
  case ID: case GLBREF: case NUMBER: case ISNUMBER:
  case REALNUM: case ISREALNUM: case OPEMPTY: 
   return(TRUE);
 }
 return(FALSE);
}

/*
 * duplicate a single level no embedded CATREP concatenate
 */
extern struct expr_t *__dup_concat(int32 repcnt, struct expr_t *catndp)
{
 register int32 w;
 struct expr_t *dupndp, *newndp, *end_ndp;

 /* now build longer chain repeated repval - 1 times */
 for (newndp = NULL, w = 0; w < repcnt; w++)
  {
   /* make one copy */
   if (w < repcnt - 1) dupndp = __copy_expr(catndp); else dupndp = catndp;
   if (newndp == NULL) newndp = dupndp;
   else
    {
     end_ndp = __find_catend(dupndp);
     end_ndp->ru.x = newndp;
     newndp = dupndp;
    }
  }
 return(newndp);
}

/*
 * find the end (rightmost CATCOM) of a one level concatenate chain
 * expects to be passed LCB concatenate chain header
 */
extern struct expr_t *__find_catend(struct expr_t *catndp)
{
 register struct expr_t *ndp;
 struct expr_t *last_ndp;

 for (ndp = catndp, last_ndp = NULL; ndp != NULL; ndp = ndp->ru.x)
  last_ndp = ndp;
 return(last_ndp);
}

/*
 * go through linearized concat and remove all marked 0 width elements
 *
 * if all removed, must emit syntax error and replace now empty concat 
 * with a 1 bit x concat
 *
 * BEWARE - using folded to mark removal so must not fold before here
 */
static void remove_0width_catrep_els(struct expr_t *catndp)
{ 
 register struct expr_t *com_xp, *com_xp2, *last_com_xp;
 struct expr_t *rem_com_xp;

 last_com_xp = NULL;
 rem_com_xp = NULL;
 com_xp2 = catndp->ru.x; 
 for (com_xp = catndp; com_xp != NULL;)
  {
   if (com_xp->folded)
    {
     com_xp2 = com_xp->ru.x;

     if (last_com_xp == NULL) catndp->ru.x = com_xp->ru.x;
     else { last_com_xp->ru.x = com_xp->ru.x; }
     if (rem_com_xp == NULL) rem_com_xp = com_xp;

     com_xp = com_xp2;
    }
   else
    {
     last_com_xp = com_xp;
     com_xp = com_xp->ru.x;
    } 
  }
 if (catndp->ru.x == NULL)
  {
   /* DBG remove -- */
   if (rem_com_xp == NULL || rem_com_xp->lu.x == NULL)
    __misc_terr(__FILE__, __LINE__);
   /* -- */

   __sgferr(3110,
    "concatenate 0 width repeat removal created illegal empty expression");

   /* SJM 09/11/03 - notice minor memory leak but can't free exprs here */
   __set_numval(rem_com_xp->lu.x, 1, 1, 1);
   catndp->ru.x = rem_com_xp;
  }
}

/*  
 * WIDTH SETTING ROUTINES
 */

/*
 * set all width values of an expression in a given width context
 */
static void set_rhswidth(struct expr_t *rhsx, int32 cwid)
{
 int32 cwid2, ibas;

 cwid2 = __get_rhswidth(rhsx);
 if (cwid > cwid2) cwid2 = cwid;
 /* if expression wider than context, need expr. max. as context */
 set2_rhswidth(rhsx, cwid2);

 ibas = find_xbase(rhsx);
 if (ibas == '?') ibas = BHEX;
 rhsx->ibase = ibas;
}

/*
 * calculate the width of a rhs expression
 * know width never IS NUMBER form
 */
extern int32 __get_rhswidth(struct expr_t *rhsx)
{
 int32 wclass, xwid, xwid2, r1, r2, sav_errcnt;
 struct net_t *np;
 struct expr_t *psx1, *psx2;

 xwid = 1;
 switch ((byte) rhsx->optyp) {
  case ID: case GLBREF:
   xwid = __get_netwide(rhsx->lu.sy->el.enp);
   break;
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM: case OPEMPTY:
   xwid = rhsx->szu.xclen;
   break;
  case LSB:
   /* bit select or array reference */
   np = rhsx->lu.x->lu.sy->el.enp;
   /* know this is net or will not get here */
   if (np->n_isarr) return(__get_netwide(np));
   return(1);
  case PARTSEL:
   /* this is tricky since constants not yet folded but since params */
   /* value known, check (and fold) this sub expression before continuing */
   psx1 = rhsx->ru.x->lu.x;
   psx2 = rhsx->ru.x->ru.x;
   sav_errcnt = __pv_err_cnt;
   __chk_rhsexpr(psx1, 0);
   __chk_rhsexpr(psx2, 0);
   np = rhsx->lu.x->lu.sy->el.enp;
   /* error caught later, set to 1 for now */
   if (sav_errcnt != __pv_err_cnt || psx1->optyp != NUMBER
    || psx2->optyp != NUMBER) xwid = 1;
   else
    {
     r1 = (int32) __contab[psx1->ru.xvi];
     r2 = (int32) __contab[psx2->ru.xvi];
     xwid = (r1 > r2) ? (r1 - r2 + 1) : (r2 - r1 + 1);
    }
   break;
  case QUEST:
   xwid = __get_rhswidth(rhsx->ru.x->ru.x);
   xwid2 = __get_rhswidth(rhsx->ru.x->lu.x);
   if (xwid2 > xwid) xwid = xwid2;
   break;
  case FCALL:
   {
    struct task_t *tskp;
    struct task_pin_t *tpp;
    struct sy_t *syp;

    /* getting output width, so have right context elsewhere */
    /* do not need to get arg. width since call shields */
    syp = rhsx->lu.x->lu.sy;
    if (syp->sytyp == SYM_SF)
     {
      /* SJM 05/28/04 - $signed and $word32 are special case that return */
      /* arg width - but need to recursively call get width to determine */
      if (syp->el.esyftbp->syfnum == STN_SIGNED
       || syp->el.esyftbp->syfnum == STN_UNSIGNED)
       {
        xwid = __get_rhswidth(rhsx->ru.x->lu.x);
       }
      else xwid = syp->el.esyftbp->retwid;
     } 
    /* error caught later */
    else if (syp->sytyp == SYM_F)
     {
      tskp = syp->el.etskp;
      tskp->t_used = TRUE;
      tpp = tskp->tskpins;
      np = tpp->tpsy->el.enp;
      xwid = __get_netwide(np);
     }
    /* if non fcall, leave as 1 bit */
   }
   return(xwid);
  case LCB:
   {
    register struct expr_t *ndp2;

    /* know cat repeats removed by here - component widths set later */
    for (xwid = 0, ndp2 = rhsx->ru.x; ndp2 != NULL; ndp2 = ndp2->ru.x)
     {
      xwid2 = __get_rhswidth(ndp2->lu.x);
      xwid += xwid2;
     }
   }
   break;
  default:
   /* this is unary or binary op */
   wclass = __get_widthclass(rhsx);
   switch ((byte) wclass) {
    case WIDONE: case WIDENONE: return(1);
    case WIDMAX:
     if (rhsx->ru.x == NULL) xwid = 0; else xwid = __get_rhswidth(rhsx->ru.x);
     if (rhsx->lu.x == NULL) xwid2 = 0; else xwid2 = __get_rhswidth(rhsx->lu.x);
     if (xwid2 > xwid) xwid = xwid2;
    break;
    case WIDLEFT: return(__get_rhswidth(rhsx->lu.x));
   case WIDSELF: return(__get_rhswidth(rhsx->lu.x));
    default: __case_terr(__FILE__, __LINE__);
   }
 }
 return(xwid);
}

/*
 * get an operator's width class
 */
extern int32 __get_widthclass(struct expr_t *rhsx)
{
 struct opinfo_t *opip;

 opip = &(__opinfo[rhsx->optyp]);
 /* unary form of both always reduction => width one */
 if (opip->opclass == BOTHOP && rhsx->ru.x == NULL)
  {
   /* AIV 03/09/05 - minus is only bothop with lhs cnxt size */
   if (rhsx->optyp != MINUS) return(WIDONE);
  }
 return(opip->reswid);
}

/*
 * set rhs width - also mark expr. that are real or stren
 *
 * SJM 07/16/01 - algorithm here is that operator has correct width that
 * may be width of assignment lhs context because in expr eval after
 * pushing operands onto xstk, size changes of xstk are made where needed
 * according to LRM rules for propagating expression width to interim results
 */
static void set2_rhswidth(struct expr_t *rhsx, int32 cwid)
{
 int32 wclass, cat_stren, xwid, xwid2, r1, r2;
 struct expr_t *tndp, *ndx1, *ndx2;
 struct sy_t *syp;
 struct net_t *np;

 switch ((byte) rhsx->optyp) {
  case ID: case GLBREF:
   np = rhsx->lu.sy->el.enp;
   /* for real id exprs, expr. is_real bit set in chk id */ 
   if (np->ntyp == N_REAL)
    { __expr_has_real = TRUE; rhsx->is_real = TRUE; rhsx->has_sign = TRUE; }
   if (np->n_stren) rhsx->x_stren = TRUE;
   rhsx->szu.xclen = __get_netwide(np);
   return;
  case NUMBER: case ISNUMBER: case OPEMPTY: return;
  case REALNUM: case ISREALNUM:
   /* for real numbers is real bit set in chk id */ 
   __expr_has_real = TRUE;
   rhsx->is_real = TRUE;
   rhsx->has_sign = TRUE;
   return;
  case LSB:
   /* bit select or array reference */
   syp = rhsx->lu.x->lu.sy;
   rhsx->lu.x->szu.xclen = __get_netwide(syp->el.enp);
   set_rhswidth(rhsx->ru.x, 0);
   np = syp->el.enp;
   if (np->n_isarr && np->n_isavec) xwid = __get_netwide(np); else xwid = 1;
   if (np->n_isarr && np->ntyp == N_REAL)
    {
     __expr_has_real = TRUE;
     rhsx->is_real = TRUE;
     rhsx->has_sign = TRUE;
    }
   if (np->n_stren) rhsx->x_stren = TRUE;
   rhsx->szu.xclen = xwid;
   return;
  case PARTSEL:
   syp = rhsx->lu.x->lu.sy;
   np = syp->el.enp;
   if (np->n_stren) rhsx->x_stren = TRUE;
   rhsx->lu.x->szu.xclen = __get_netwide(np);
   ndx1 = rhsx->ru.x->lu.x;
   ndx2 = rhsx->ru.x->ru.x;
   set_rhswidth(ndx1, 0);
   set_rhswidth(ndx2, 0);
   /* SJM 05/22/00 - error for non illegal non numeric psel caught later */
   /* get rhs width always folds psel indices so if not numeric error */
   if ((ndx1->optyp == NUMBER || ndx1->optyp == ISNUMBER)
    && (ndx2->optyp == NUMBER || ndx2->optyp == ISNUMBER))
    {
     r1 = (int32) __contab[ndx1->ru.xvi];
     r2 = (int32) __contab[ndx2->ru.xvi];
     rhsx->szu.xclen = (r1 >= r2) ? (r1 - r2 + 1) : (r2 - r1 + 1);
    }
   /* using 32 bits - error caught later */
   else rhsx->szu.xclen = WBITS;
   return;
  case QUEST:
   set_rhswidth(rhsx->lu.x, 0);
   set_rhswidth(rhsx->ru.x->ru.x, cwid);
   set_rhswidth(rhsx->ru.x->lu.x, cwid);

   tndp = rhsx->ru.x;
   /* assume right wider */
   xwid = tndp->lu.x->szu.xclen;
   if (xwid < tndp->ru.x->szu.xclen) xwid = tndp->ru.x->szu.xclen;
   rhsx->szu.xclen = xwid;
   return;
  case FCALL:
   {
    int32 frntyp;
    struct task_t *tskp;
    struct task_pin_t *tpp;

    /* notice func. name does not have width but top (rhsx) has ret. width */
    syp = rhsx->lu.x->lu.sy;
    tskp = syp->el.etskp;
    if (syp->sytyp == SYM_SF)
     {
      struct sysfunc_t *syfp;

      syfp = syp->el.esyftbp;
      /* SJM 05/28/04 - $signed and $word32 are special case that return */
      /* arg width - but need to recursively call get width to determine */
      if (syp->el.esyftbp->syfnum == STN_SIGNED
       || syp->el.esyftbp->syfnum == STN_UNSIGNED)
       {
        rhsx->szu.xclen = __get_rhswidth(rhsx->ru.x->lu.x);
       }
      else
       {
        /* for real this must be WBITS */
        rhsx->szu.xclen = syfp->retwid;
       }
      /* notice also sets signed for function */
      if (syfp->retsigned) rhsx->has_sign = TRUE;
      frntyp = syfp->retntyp;
     }
    else if (syp->sytyp == SYM_F)
     {
      tpp = tskp->tskpins;
      np = tpp->tpsy->el.enp;
      /* functions cannot return arrays */
      rhsx->szu.xclen = __get_netwide(np);
      if (np->n_signed) rhsx->has_sign = TRUE;
      frntyp = np->ntyp;
     }
    /* if call of non function error (caught already) - do nothing */
    else return;

    if (frntyp == N_REAL)
     { rhsx->is_real = TRUE; rhsx->has_sign = TRUE; __expr_has_real = TRUE; }

    /* this sets argument widths */
    /* this is needed and uses right decl. var context for non sys functions */
    /* because width setting is structural before recursive processing */
    if (syp->sytyp == SYM_SF) set_sysfcall_widths(rhsx);
    else set_fcall_widths(rhsx, tskp);
   }
   return;
  case LCB:
   {
    register struct expr_t *ndp2;
    struct expr_t *catxp;

    /* know concatenates never nested by here */
    /* first set cat self determined component widths */
    xwid = 0;
    for (cat_stren = FALSE, ndp2 = rhsx->ru.x; ndp2 != NULL; ndp2 = ndp2->ru.x)
     {
      catxp = ndp2->lu.x;
      set_rhswidth(catxp, 0);
      xwid += catxp->szu.xclen;
      if (catxp->x_stren) { ndp2->x_stren = TRUE; cat_stren = TRUE; }
     }
    rhsx->szu.xclen = xwid;
    if (cat_stren) rhsx->x_stren = TRUE;
    /* CAT COM op width is dist from high bit of this to low (right) end */
    for (ndp2 = rhsx->ru.x; ndp2 != NULL; ndp2 = ndp2->ru.x)
     {
      ndp2->szu.xclen = xwid;
      xwid -= ndp2->lu.x->szu.xclen;
     }
    if (xwid != 0) __misc_terr(__FILE__, __LINE__);
   }
   return;
  default:
   /* this is unary or binary op */
   wclass = __get_widthclass(rhsx);
   switch ((byte) wclass) {
    case WIDONE:
     /* this is for unaries only except for || and && */
     rhsx->szu.xclen = 1;
     /* under width 1 shield new context is max. width of subexpr. */
     /* case is something like &(a + b + c) where c widest */
     /* this spreads max. width because of context to all subexpressions */
     xwid = __get_rhswidth(rhsx->lu.x);
     set_rhswidth(rhsx->lu.x, xwid);
     /* notice binaries can be wide one also */
     if (rhsx->ru.x != NULL)
      {
       /* same here */
       xwid = __get_rhswidth(rhsx->ru.x);
       set_rhswidth(rhsx->ru.x, xwid);
      }
     return;
    case WIDENONE:
     /* for binaries that produce one bit result */
     rhsx->szu.xclen = 1;
     /* notice not using cwid context under here */
     xwid = __get_rhswidth(rhsx->lu.x);
     xwid2 = __get_rhswidth(rhsx->ru.x);
     if (xwid2 > xwid) xwid = xwid2;
     if (rhsx->lu.x != NULL) set_rhswidth(rhsx->lu.x, xwid);
     if (rhsx->ru.x != NULL) set_rhswidth(rhsx->ru.x, xwid);
     return;
    case WIDMAX:
     /* need context here since non shielded binary */
     r1 = r2 = 0;
     if (rhsx->ru.x != NULL)
      {
       set_rhswidth(rhsx->ru.x, cwid);
       r1 = rhsx->ru.x->szu.xclen;
      }
     if (rhsx->lu.x != NULL)
      {
       set_rhswidth(rhsx->lu.x, cwid);
       r2 = rhsx->lu.x->szu.xclen;
      }
     xwid = (r1 < r2) ? r2 : r1;
select_wid:
     if (xwid < cwid) xwid = cwid;
     rhsx->szu.xclen = xwid;
     return;
    case WIDLEFT:
     /* set the non context right width (i.e. shift amount) */
     set_rhswidth(rhsx->ru.x, 0);
     set_rhswidth(rhsx->lu.x, cwid);
     xwid = rhsx->szu.xclen;
     goto select_wid;
    case WIDSELF:
     /* for unary - and ~ - (+ too but does nothing - optimized away) */
     /* LOOKATME - why getting width and using as context - would 0 work? */
     xwid = __get_rhswidth(rhsx->lu.x);
     set_rhswidth(rhsx->lu.x, xwid);
     /* SJM 07/16/01 - this is wrong - context width need for operator node */
     /* ??? rhsx->szu.xclen = xwid; */
     goto select_wid;
    default: __case_terr(__FILE__, __LINE__);
   }
 }
}

/*
 * set width for user fcall arguments - context here is argument width
 * user functions require all arguments
 * wrong number of arguments caught elsewhere - use what is there
 */
static void set_fcall_widths(struct expr_t *fcrhsx,
 struct task_t *tskp)
{
 struct expr_t *fandp;
 struct task_pin_t *tpp;
 int32 pwid;

 /* point 1 past extra first argument - not part of source call */
 tpp = tskp->tskpins->tpnxt;
 for (fandp = fcrhsx->ru.x; fandp != NULL; fandp = fandp->ru.x)
  {
   /* move to next declared argument */
   if (tpp != NULL) tpp = tpp->tpnxt;

   if (tpp == NULL || tpp->tpsy == NULL) pwid = 0;
   else pwid = __get_netwide(tpp->tpsy->el.enp);

   /* notice for numbers this can't change width */
   set_rhswidth(fandp->lu.x, pwid);
  }
}

/*
 * set width for system fcall arguments, no context
 * at this point since checking done just use expr. that is there
 */
static void set_sysfcall_widths(struct expr_t *fcrhsx)
{
 struct expr_t *fandp;

 /* 0 argument system functions common */
 /* notice expressions structure does not have extra 1st return value */
 for (fandp = fcrhsx->ru.x; fandp != NULL; fandp = fandp->ru.x)
  set_rhswidth(fandp->lu.x, 0);
}


/*
 * ROUTINE TO SET EXPR SIGNED USING NEW 2001 RULES
 */

/*
 * set all expression nodes has signed flag 
 */
static void set_rhs_signed(struct expr_t *rhsx)
{
 register struct expr_t *ndp2;
 int32 wclass;
 struct net_t *np;
 struct sy_t *syp;
 struct task_t *tskp;
 struct sysfunc_t *syfp;
 struct task_pin_t *tpp;

 switch ((byte) rhsx->optyp) {
  case ID: case GLBREF:
   np = rhsx->lu.sy->el.enp;
   /* SJM 10/02/03 - for real has sign already set by width setting */
   /* but doing it here again doesn't hurt */
   if (np->n_signed) rhsx->has_sign = TRUE;
   break;
  case NUMBER: case ISNUMBER: case OPEMPTY:
  case REALNUM: case ISREALNUM:
   /* for numbers the signed flag set during source input */
   break;
  case LSB:
   /* bit select or array reference */
   /* set lhs ID node signedness */ 
   set_rhs_signed(rhsx->lu.x);

   /* then set select node signedness */
   syp = rhsx->lu.x->lu.sy;
   np = syp->el.enp;
   if (np->n_isarr)
    {
     if (np->n_signed) rhsx->has_sign = TRUE; 
    }
   else rhsx->has_sign = FALSE;

   /* finally set select range signed */
   set_rhs_signed(rhsx->ru.x);
   break;
  case PARTSEL:
   /* set lhs ID node signedness */ 
   set_rhs_signed(rhsx->lu.x);
   rhsx->has_sign = FALSE;
   break;
  case QUEST:
   set_rhs_signed(rhsx->lu.x);

   set_rhs_signed(rhsx->ru.x->lu.x);
   set_rhs_signed(rhsx->ru.x->ru.x);
   if (rhsx->ru.x->lu.x->has_sign && rhsx->ru.x->ru.x->has_sign)
    {
     rhsx->has_sign = TRUE;
    }
   else
    {
     /* SJM 05/21/04 - not both signed - easiest to just make both word32 */
     rhsx->has_sign = FALSE;
     rhsx->ru.x->lu.x->has_sign = FALSE;
     rhsx->ru.x->ru.x->has_sign = FALSE;
    }
   break;
  case FCALL:
   syp = rhsx->lu.x->lu.sy;
   tskp = syp->el.etskp;
   if (syp->sytyp == SYM_SF)
    {
     syfp = syp->el.esyftbp;
     if (syfp->retsigned) rhsx->has_sign = TRUE;
    }
   else if (syp->sytyp == SYM_F)
    {
     /* SJM 10/02/03 - LOOKATME - old rule was put info in first dummy arg */
     /* check to see for 2001 LRM is still true */

     tpp = tskp->tskpins;
     np = tpp->tpsy->el.enp;
     if (np->n_signed) rhsx->has_sign = TRUE;
    }
   /* if call of non function error (caught already) - do nothing */
   else return;

   /* final step sets signed flag for each argument of function call */
   for (ndp2 = rhsx->ru.x; ndp2 != NULL; ndp2 = ndp2->ru.x)
    {
     set_rhs_signed(ndp2->lu.x);
    }
   break;
  case LCB:
   /* know concatenate result never signed */
   rhsx->has_sign = FALSE;
   /* but components can have signed sub-expressions */
   for (ndp2 = rhsx->ru.x; ndp2 != NULL; ndp2 = ndp2->ru.x)
    {
     set_rhs_signed(ndp2->lu.x);
    }
   break; 
  default:
   wclass = __get_widthclass(rhsx);
   /* this is unary or binary op */
   switch ((byte) wclass) {
    case WIDONE:
     set_rhs_signed(rhsx->lu.x);
     if (rhsx->ru.x != NULL) set_rhs_signed(rhsx->ru.x);
     rhsx->has_sign = FALSE;
     break;
    case WIDENONE:
     /* this is for unaries only except for || and && */
     /* DBG remove */
     if (rhsx->ru.x == NULL) __misc_terr(__FILE__, __LINE__);
     /* --- */
     set_rhs_signed(rhsx->lu.x);
     set_rhs_signed(rhsx->ru.x);
     rhsx->has_sign = FALSE;
     break;
    case WIDMAX:
     /* SJM 04/26/04 - unary minus can be be WID MAX */
     /* rest of binaries */
     set_rhs_signed(rhsx->lu.x);
     if (rhsx->ru.x != NULL)
      {
       /* binary case */
       set_rhs_signed(rhsx->ru.x);
       if (rhsx->lu.x->has_sign && rhsx->ru.x->has_sign)
        rhsx->has_sign = TRUE;
       else rhsx->has_sign = FALSE;
      }
     else
      {
       /* unary case */
       if (rhsx->lu.x->has_sign) rhsx->has_sign = TRUE;
       else rhsx->has_sign = FALSE;
      }
     return;
    case WIDLEFT:
     /* SHIFT */
     set_rhs_signed(rhsx->ru.x);
     /* SJM 10/02/03 - LRM says shift count always word32 */
     rhsx->ru.x->has_sign = FALSE;
     set_rhs_signed(rhsx->lu.x);
     rhsx->has_sign = rhsx->lu.x->has_sign;
     break;
    case WIDSELF:
     /* for unary - and ~ - (+ too but does nothing - optimized away) */
     set_rhs_signed(rhsx->lu.x);
     rhsx->has_sign = rhsx->lu.x->has_sign;
     break;
    default: __case_terr(__FILE__, __LINE__);
   }
 }
}

/*
 * find a number in an expression with a base and return 
 * means folded expr. will have base of first component
 */
static int32 find_xbase(struct expr_t *ndp)
{
 int32 ibas;

 if (ndp->optyp == NUMBER) { ibas = ndp->ibase; return(ibas); }
 if (__isleaf(ndp)) return((int32) '?');
 if (ndp->lu.x != NULL)
  {
   ibas = find_xbase(ndp->lu.x);
   if (ibas != '?') return(ibas);
  }
 if (ndp->ru.x != NULL)
  {
   ibas = find_xbase(ndp->ru.x);
   if (ibas != '?') return(ibas);
  }
 return((int32) '?');
}

/*
 * set and check expr is real and check for all real rhs expression 
 *
 * only called if know somewhere in expression there is a real 
 * expressions must be all real but some operators take real args and return
 * reg (1 bit) - i.e. 1 bit logicals
 *
 * this sets IS real and expects IS real bit to already be set for leaf nodes
 * works bottom up and assumes leaf nodes have IS real set
 * sets upper nodes and replaces operators
 */
static void setchk_real_expr(struct expr_t *xrhs)
{
 int32 i, oandnum, wclass;
 struct opinfo_t *opip;
 struct expr_t *lx, *rx;

 /* is real bit already set */
 if (__isleaf(xrhs)) return;

 if (xrhs->optyp == QUEST)
  {
   real_setchk_quest(xrhs);
   return;
  }
 if (xrhs->optyp == FCALL)
  {
   /* must chk real for all arguments - know fcall is real already set */
   /* this is a structural entire expr. tree routine so must check reals */
   /* in arguments even though unrelated and arguments probably contain */ 
   /* no reals - using check real on these works - does some extra work */
   /* but subtley needed because probably function is real returning */

   /* if part of relational or boolean subexpression can actually be non */ 
   /* real returning function */
   for (rx = xrhs->ru.x; rx != NULL; rx = rx->ru.x) setchk_real_expr(rx->lu.x); 
   return; 
  }

 /* special handling of concatenates - by here know 1 level */
 if (xrhs->optyp == LCB)
  {
   for (rx = xrhs->ru.x, i = 1; rx != NULL; rx = rx->ru.x, i++)
    {
     setchk_real_expr(rx->lu.x);
     /* error if any component real - concatenate never real but can be */
     /* operator to real conditional */
     if (rx->lu.x->is_real)
      __sgferr(817, "real expression illegal in concatenate (pos. %d)", i);
    }
   return;
  } 

 oandnum = 0;
 if ((lx = xrhs->lu.x) != NULL) { setchk_real_expr(lx); oandnum++; }
 if ((rx = xrhs->ru.x) != NULL) { setchk_real_expr(rx); oandnum++; }

 if (oandnum > 1)
  {
   /* binary case */
   /* case 1: neither, operand is real - node not marked as real */
   /* this is legal - operator is real not set so if not part of relational */
   /* or logical error will be caught later */
   /* example of ok of this: (i < j) || (d1 < d2) */ 
   if (!lx->is_real && !rx->is_real) return;

   /* case 2: at least one operand is real */
   opip = &(__opinfo[xrhs->optyp]);
   if (!opip->realop)
    {
     __sgferr(815, "binary operator %s cannot have real operand(s)",
      __to_opname(xrhs->optyp));
     return;
    }
   /* know legal real binary op */
   /* notice dependent on real represented with len WBITS using b part */
   wclass = __get_widthclass(xrhs);
   if (wclass != WIDENONE && wclass != WIDONE)
    {
     /* normal real (allowed subset) operators result is real */
     xrhs->is_real = TRUE;
     xrhs->has_sign = TRUE;
     xrhs->szu.xclen = REALBITS;
     switch ((byte) xrhs->optyp) {
      case PLUS: xrhs->optyp = REALPLUS; break;
      case MINUS: xrhs->optyp = REALMINUS; break;
      case TIMES: xrhs->optyp = REALTIMES; break;
      case DIV: xrhs->optyp = REALDIV; break;
      /* for nest real function calls may already have been converted */ 
      case REALPLUS: break;
      case REALMINUS: break;
      case REALTIMES: break;
      case REALDIV: break;
      default: __case_terr(__FILE__, __LINE__);
     }
    }
   else
    {
     /* if 1 bit result (comparison or &&/|| result is not real) */
     switch ((byte) xrhs->optyp) {
      case RELGT: xrhs->optyp = REALRELGT; break;
      case RELGE: xrhs->optyp = REALRELGE; break;
      case RELLT: xrhs->optyp = REALRELLT; break;
      case RELLE: xrhs->optyp = REALRELLE; break;
      case RELEQ: xrhs->optyp = REALRELEQ; break;
      case RELNEQ: xrhs->optyp = REALRELNEQ; break;
      case BOOLAND: xrhs->optyp = REALBOOLAND; break;
      case BOOLOR: xrhs->optyp = REALBOOLOR; break;
      /* for nest real function calls may already have been converted */ 
      case REALRELGT: break;
      case REALRELGE: break;
      case REALRELLT: break;
      case REALRELLE: break;
      case REALRELEQ: break;
      case REALRELNEQ: break;
      case REALBOOLAND: break;
      case REALBOOLOR: break;
      default: __case_terr(__FILE__, __LINE__);
     }
    }
   /* mark either non real for conversion to real */
   if (!lx->is_real) lx->cnvt_to_real = TRUE; 
   else if (!rx->is_real) rx->cnvt_to_real = TRUE; 
   return;
  }

 /* SJM 10/16/00 - other part of expr. can be real if unary arg not real */
 /* nothing to do here */

 /* if non real ok, unless under relational err caught higher in expr tree */
 /* here operand must be real since only one and result real */
 if (!lx->is_real) return;

 /* unary real case (only unary - and ! legal) */
 if (xrhs->optyp != MINUS && xrhs->optyp != NOT)
  {
   __sgferr(878, "unary operator %s cannot have real operand",
    __to_opname(xrhs->optyp));
   return;
  }

 if (xrhs->optyp == MINUS)
  {
   xrhs->optyp = REALMINUS;
   xrhs->is_real = TRUE;
   xrhs->has_sign = TRUE; 
   xrhs->szu.xclen = REALBITS;
  }
 /* result is 1 bit but not real - if arg. not real do not change */
 else if (xrhs->optyp == NOT) xrhs->optyp = REALNOT;
}

/*
 * set is real and check real ?:
 *
 * LOOKATME - converting : part of ?: to real if one real
 */
static void real_setchk_quest(struct expr_t *xrhs)
{
 struct expr_t *lx, *rx;

 /* according to lrm - selector can be real (0.0 F else T) */
 setchk_real_expr(xrhs->lu.x);
 lx = xrhs->ru.x->lu.x;
 setchk_real_expr(lx);
 rx = xrhs->ru.x->ru.x;
 setchk_real_expr(rx);

 /* must change operators - this is most complicated case */
 /* know expr. correct and either both or no operators real */ 
 if (!xrhs->lu.x->is_real)
  {
   /* if either : expression real, convert other to real */
   if (lx->is_real || rx->is_real)
    {
     /* notice QUEST top (not QCOL) is changed */
     xrhs->optyp = REGREALQCOL;
     xrhs->is_real = TRUE;
     xrhs->has_sign = TRUE;
     /* real is WBITS since no x value */ 
     xrhs->szu.xclen = WBITS;
     xrhs->ru.x->is_real = TRUE;
     xrhs->ru.x->has_sign = TRUE;
     /* QCOL ? is real and has real width */ 
     xrhs->ru.x->szu.xclen = WBITS;
     /* set bit so if either non real will get converted */
     if (!lx->is_real) lx->cnvt_to_real = TRUE; 
     else if (!rx->is_real) rx->cnvt_to_real = TRUE; 
    }
   /* can be non real subexpression of real */   
   return;
  }
 /* know cond. real */
 if (lx->is_real || rx->is_real)
  {
   xrhs->optyp = REALREALQUEST;
   xrhs->ru.x->is_real = TRUE;
   xrhs->ru.x->has_sign = TRUE;
   xrhs->ru.x->szu.xclen = WBITS;
   xrhs->szu.xclen = WBITS;
   xrhs->is_real = TRUE;
   xrhs->has_sign = TRUE;
   /* set bit so if either non real will get converted */
   if (!lx->is_real) lx->cnvt_to_real = TRUE; 
   else if (!rx->is_real) rx->cnvt_to_real = TRUE; 
   return;
  }
 /* notice here nothing marked as real except cond. sub expr */
 xrhs->optyp = REALREGQUEST;  
}

/*
 * recursively check expression special operators - still structural
 * cannot do anything that requires any folding
 * also empty illegal here - will have been caught
 */
static void chk_specialop_rhsexpr(struct expr_t *ndp)
{
 switch ((byte) ndp->optyp) {
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM: case OPEMPTY:
   return;
  case ID: case GLBREF: chk_rhs_id(ndp); return;
  case LSB:
   chk_specialop_rhsexpr(ndp->ru.x);
   chk_srcbsel(ndp, TRUE);
   return;
  case PARTSEL:
   chk_specialop_rhsexpr(ndp->ru.x->lu.x);
   chk_specialop_rhsexpr(ndp->ru.x->ru.x);
   chk_srcpsel(ndp, TRUE);
   return;
  case LCB:
   {
    register struct expr_t *catndp;

    for (catndp = ndp->ru.x; catndp != NULL; catndp = catndp->ru.x)
     {
      chk_specialop_rhsexpr(catndp->lu.x);
      catndp->has_sign = FALSE;
     }
    ndp->has_sign = FALSE;
   }
   return;
  case FCALL: 
   chkspecop_fcall(ndp);
   return;
  case QUEST:
   chk_specialop_rhsexpr(ndp->lu.x);
   /* SJM 05/21/04 - these just sets the nd rel sign eval bit */
   /* signed set elsewhere */
   chk_specialop_rhsexpr(ndp->ru.x->lu.x);
   chk_specialop_rhsexpr(ndp->ru.x->ru.x);
   return;   
  case REALREGQUEST: case REALREALQUEST: case REGREALQCOL:
   /* SJM 10/17/99 - now real's allowed in expressions - need to check */
   chk_specialop_rhsexpr(ndp->lu.x);
   chk_specialop_rhsexpr(ndp->ru.x->lu.x);
   chk_specialop_rhsexpr(ndp->ru.x->ru.x);
   return;
  case OPEVOR: case OPEVCOMMAOR: case OPPOSEDGE: case OPNEGEDGE:
   __sgferr(818,
    "event expression operator %s illegal in right hand side expression",
    __to_opname(ndp->optyp));
   return;
 }
 /* if real already checked */
 /* ??? if (ndp->is_real) return; */ 

 /* know ndp some kind of arith operator to fall thru to here */
 /* know parsing made sure binaries have 2 ops and unaries 1 */
 if (ndp->lu.x != NULL) chk_specialop_rhsexpr(ndp->lu.x);
 if (ndp->ru.x != NULL) chk_specialop_rhsexpr(ndp->ru.x);
 if (ndp->ru.x == NULL && ndp->lu.x == NULL) __arg_terr(__FILE__, __LINE__);

 /* SJM 10/06/03 - now have separate proc to set sign but set rel sign eval */
 if (ndp->szu.xclen == 1)
  {
   /* SJM 05/13/04 - relationals always binary */
   if (ndp->lu.x != NULL && ndp->ru.x != NULL) 
    {
     if (ndp->lu.x->has_sign && ndp->ru.x->has_sign)
      {
       /* for reals, always signed and this flag not used */
       switch ((byte) ndp->optyp) { 
        case RELGE: case RELGT: case RELLE: case RELLT:
        case RELNEQ: case RELEQ: case RELCEQ: case RELCNEQ:
         ndp->rel_ndssign = TRUE;  
       }
      }
    } 
  }
}

/*
 * check a known constant bit select for in range and non-z/x
 * no lhs/rhs distinction here
 *
 * this can be called during source reading - 2nd part checks range
 * that requires parameters to have fixed values
 *
 * return F on error or x/z number - caller decides if error needed
 * will never get here for event since illegal in rhs or lhs expressions
 */
static int32 chk_srcbsel(struct expr_t *ndp, int32 is_rhs)
{
 struct expr_t *idndp, *rconx;
 struct sy_t *syp;
 struct net_t *np;

 idndp = ndp->lu.x;
 syp = idndp->lu.sy;
 rconx = ndp->ru.x;
 if (syp == NULL) __arg_terr(__FILE__, __LINE__);
 if (syp->sytyp != SYM_N)
  {
   __sgferr(819,
    "bit select or array index from non wire/reg \"%s\" type %s",
    __to_idnam(idndp), __to_sytyp(__xs, syp->sytyp));
   return(FALSE);
  }
 np = syp->el.enp;
 if (!np->n_isavec && !np->n_isarr)
  {
   __sgferr(820, "bit select or array index of non memory and non vector %s",
    __to_idnam(idndp));
   return(FALSE);
  }
 /* selects from real params already removed - LOOKATME ?? */
 if (np->ntyp == N_REAL && !np->n_isarr)
  {
   __sgferr(821, "bit select from real %s illegal", __to_idnam(idndp));
   return(FALSE);
  }
 /* bit and part selects from parameters in normal rhs exprs supported */
 if (np->n_isarr && (np->ntyp < NONWIRE_ST || np->ntyp == N_EVENT))
  {
   __sgferr(823, "array index of %s illegal for %s", __to_idnam(idndp),
    __to_wtnam(__xs, np));
   return(FALSE);
  }
 if (np->nrngrep == NX_CT)
  {
   if (is_rhs)
    {
     np->nu.ct->n_onrhs = TRUE;
     if (!__expr_rhs_decl) np->n_onprocrhs = TRUE;
    }
   else
    {
     if (np->nu.ct->n_onlhs) np->nu.ct->n_2ndonlhs = TRUE;
     else np->nu.ct->n_onlhs = TRUE;
    }   
  }

 /* can also do real checking now */
 switch ((byte) rconx->optyp) {
  case NUMBER: break;
  case ISNUMBER:
   if (rconx->is_real) goto bs_real;
   break;
  case ISREALNUM:
bs_real:
   __sgferr(824,
    "bit select or array index of %s cannot be real", __to_idnam(idndp));
   return(FALSE);
  default:
   /* for procedural expr. checking done and simplied index expr by here */
   if (rconx->is_real) goto bs_real; 
   if (rconx->szu.xclen > WBITS)
    {
     __sgfinform(455,
      "select index variable expression %s wider than %d - high bits ignored",
      __msgexpr_tostr(__xs, rconx), WBITS);
    }
   return(TRUE);
  }
 /* but must fold and normalize later */
 return(TRUE);
}

/*
 * check a part select for in range and non-x/z
 * no lhs/rhs distinction here
 * part select values non x/z numbers and cannot be variable
 * returns F on error
 */
static int32 chk_srcpsel(struct expr_t *ndp, int32 is_rhs)
{
 struct net_t *np;
 struct expr_t *idndp;
 struct sy_t *syp;

 idndp = ndp->lu.x; 
 syp = idndp->lu.sy;
 if (syp == NULL)
  {
   __sgferr(830, "part select from parameter %s illegal", __to_idnam(idndp));
   return(FALSE);
  }
 if (syp->sytyp != SYM_N)
  {
   __sgferr(831, "part select from non net or register \"%s\" type %s illegal",
    __to_idnam(idndp), __to_sytyp(__xs, syp->sytyp));
   return(FALSE);
  }
 np = syp->el.enp;
 if (!np->n_isavec)
  {
   __sgferr(832, "part select of non vector %s illegal", __to_idnam(idndp));
   return(FALSE);
  }
 /* ??? REAL ARRAYS LEGAL
 if (np->n_isarr)
  {
   __sgferr(833, "part select of array %s illegal", __to_idnam(idndp));
   return(FALSE);
  }
 --- */
 if (np->ntyp == N_REAL)
  {
   __sgferr(834, "part select of real %s illegal", __to_idnam(idndp));
   return(FALSE);
  }
 if (np->nrngrep == NX_CT)
  {
   if (is_rhs)
    {
     np->nu.ct->n_onrhs = TRUE;
     if (!__expr_rhs_decl) np->n_onprocrhs = TRUE;
    }
   else
    {
     if (np->nu.ct->n_onlhs) np->nu.ct->n_2ndonlhs = TRUE;
     else np->nu.ct->n_onlhs = TRUE;
    }
  }
 return(TRUE);
} 

/*
 * source (before expr. widths known) structural check of 1 function call
 * must be call of function with right number of args
 * argument checking elsewhere
 */
static void chkspecop_fcall(struct expr_t *ndp)
{
 register struct expr_t *fandp, *andp;
 struct sy_t *syp;
 struct expr_t *idndp;

 /* when translating iact stmt, func. call requires scheduled thread */
 /* since may have break in it */
 idndp = ndp->lu.x;
 syp = idndp->lu.sy;
 if (syp->sytyp != SYM_F && syp->sytyp != SYM_SF)
  {
   __sgferr(835, "call of non function %s", __to_idnam(idndp));
   return;
  }
 /* SJM 09/28/01 - set flag so can indicate func has user fcall */
 if (syp->sytyp == SYM_F)
  { __iact_must_sched = TRUE; __func_has_fcall = TRUE; }
 else
  {
   /* for system function (maybe PLI) - unindex array legal */
   for (fandp = ndp->ru.x; fandp != NULL; fandp = fandp->ru.x)
    {
     andp = fandp->lu.x;
     /* only for PLI 2.0 system functions, unindex array legal */ 
     if (syp->el.esyftbp->syfnum > __last_veriusertf
      && (andp->optyp == ID || andp->optyp == GLBREF))
      {
       if (andp->lu.sy->el.enp->n_isarr) continue;
      }
     chk_specialop_rhsexpr(andp);
    }
   return;
  }

 for (fandp = ndp->ru.x; fandp != NULL; fandp = fandp->ru.x)
  chk_specialop_rhsexpr(fandp->lu.x);
}

/*
 * emit warning for word32 relationals
 * return T if fold word32 so only 1 warning emitted
 */
static int32 chk_mixedsign_relops(struct expr_t *ndp)
{
 register struct expr_t *xp;

 switch ((byte) ndp->optyp) {
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM:
  case ID: case GLBREF:
   break;
  case LSB:
   if (chk_mixedsign_relops(ndp->ru.x)) return(TRUE);
   break;
  case PARTSEL:
   if (chk_mixedsign_relops(ndp->ru.x->lu.x)) return(TRUE);
   if (chk_mixedsign_relops(ndp->ru.x->ru.x)) return(TRUE);
   break;
  case LCB:
   {
    for (xp = ndp->ru.x; xp != NULL; xp = xp->ru.x)
     if (chk_mixedsign_relops(xp->lu.x)) return(TRUE);
   }
   break;
  case FCALL: 
   {
    /* even if consts in concatenate, entire thing is not */
    for (xp = ndp->ru.x; xp != NULL; xp = xp->ru.x)
     if (chk_mixedsign_relops(xp->lu.x)) return(TRUE);
   }
   break;
  default:
   if (ndp->lu.x != NULL) if (chk_mixedsign_relops(ndp->lu.x)) return(TRUE);
   if (ndp->ru.x != NULL) if (chk_mixedsign_relops(ndp->ru.x)) return(TRUE);
   switch ((byte) ndp->optyp) {
    case RELGE: case RELGT: case RELLE: case RELLT:
     /* if either but not both or neither signed, emit warning */
     if (ndp->lu.x->has_sign ^ ndp->ru.x->has_sign)
      {
       __sgfinform(440,
        "relational operator has mixed signed and word32 operands"); 
       return(TRUE);
      }
   }
 }
 return(FALSE);
}

/*
 * COMPILE TIME CONSTANT FOLDING ROUTINES
 */

/*
 * fold any normal rhs expression including specify section
 * where know only specparams used
 */
static void fold_subexpr(struct expr_t *ndp)
{
 int32 isform;

 /* notice any opempty will be marked as folded */
 if (!ndp->folded)
  {
   mark_constnd(ndp, &isform);
   fold_const(ndp);
   ndp->folded = TRUE;
  }
}

/*
 * mark all constant nodes
 * if evaluates to constant, root will have consubxpr turned on
 * all node width's must be set before calling this
 * any expr. that is ID but really param changed here to constant
 *
 * notice ISNUMBER and ISREAL are not foldable and places that need number
 * must be split ranges or module port header selects, or must be inst by
 * by inst. (creation of dependent param substituted exprs.) or are
 * illegal (concatenate repeat counts)
 *
 * all selects from parameters (legal in normal but not param define rhs)
 * converted to number or IS number before here because must set width
 *
 * for unary if operand is IS so is result
 * for binary if either is IS, so is result, no reduction operators
 */
static int32 mark_constnd(register struct expr_t *ndp, int32 *has_isform)
{
 register struct net_t *np;
 int32 const_subtree, cat_isform;
 int32 is1, is2, is3, wlen, nwid, wsiz;
 double d1;
 word32 *wp;
 double *dp;
 struct xstk_t *xsp;

 *has_isform = FALSE;
 switch ((byte) ndp->optyp) {
  case NUMBER: case REALNUM:
   ndp->consubxpr = TRUE;
   return(TRUE);
  case ISNUMBER: case ISREALNUM:
   ndp->consubxpr = TRUE;
   ndp->consub_is = TRUE;
   *has_isform = TRUE;
   return(TRUE);
  /* do not mark empty as constant */
  case OPEMPTY: break;
  case ID:
   /* LOOKATME - can this be fcall that is not const */  
   if (ndp->lu.sy->sytyp != SYM_N) break;

   np = ndp->lu.sy->el.enp;
cnvt_gref_param:
   if (!np->n_isaparam) return(FALSE); 
   else
    {
     /* must convert to ID's that are really params to number type  */
     /* wire converted - must convert expr that points to wire here */
     /* since parameter can be used more than once, must copy value */

     /* DBG - remove */
     nwid = __get_netwide(np);
     if (nwid != ndp->szu.xclen) __misc_terr(__FILE__, __LINE__);
     /* --- */
     wlen = wlen_(nwid);
     wsiz = 0;
     if (np->ntyp != N_REAL)
      {
       if (np->srep == SR_PISNUM)
        {
         ndp->optyp = ISNUMBER;
         ndp->consub_is = TRUE;
         *has_isform = TRUE;
         wsiz = wlen*__inst_mod->flatinum;
        }
       else if (np->srep == SR_PNUM)
        {
         ndp->optyp = NUMBER;
         ndp->consubxpr = TRUE;
         wsiz = wlen;
        }
       else __case_terr(__FILE__, __LINE__);

       if (np->n_signed) ndp->has_sign = TRUE;
       if (np->nu.ct->pstring) ndp->is_string = TRUE;
       ndp->ibase = np->nu.ct->pbase;

       if (ndp->optyp == NUMBER && nwid <= WBITS)
        {
         wp = np->nva.wp;
         ndp->ru.xvi = __alloc_shareable_cval(wp[0], wp[1], nwid);
        }
       else
        {
         /* IS forms can't be shared because are changed after added */
         if (*has_isform)
          {
           ndp->ru.xvi = __alloc_is_cval(wsiz);
           /* SJM 02/23/05 - IS const alloc no longer also sets the values */ 
           memcpy(&(__contab[ndp->ru.xvi]), np->nva.wp, 2*wsiz*WRDBYTES);
          }
         else
          {
           ndp->ru.xvi = __allocfill_cval_new(np->nva.wp, &(np->nva.wp[wsiz]),
            wsiz);
          }
        }
      }
     else
      {
       /* for real xclen is 32 which produces 8 bytes object (no b part) */
       /* but need to add to real con tab */ 
       if (np->srep == SR_PISNUM)
        {
         ndp->optyp = ISREALNUM;
         ndp->consub_is = TRUE;
         *has_isform = TRUE;
         /* word size is 1 since reals take 2 words but no b part */
         ndp->ru.xvi = __allocfill_cval_new(np->nva.wp, &(np->nva.wp[wsiz]),
          __inst_mod->flatinum);

         /* LOOKATME - does this work ??? */
         dp = (double *) &(__contab[ndp->ru.xvi]);
         /* copy from param (aka net) form to constant value table */

         /* copy from param (aka net) form to constant value table */
         /* SJM 02/20/04 - copy every inst with real size */
         memcpy(dp, np->nva.dp, sizeof(double)*__inst_mod->flatinum);
        }
       else if (np->srep == SR_PNUM)
        {
         ndp->optyp = REALNUM;
         ndp->consubxpr = TRUE;
         memcpy(&d1, np->nva.wp, sizeof(double));
         ndp->ru.xvi = __alloc_shareable_rlcval(d1);
         /* notice because of different byte ordering must cast word32 form */ 
        }
       else __case_terr(__FILE__, __LINE__);
       ndp->is_real = TRUE;
       ndp->has_sign = TRUE;
      }
    } 
   return(TRUE);
  case GLBREF: 
   /* SJM - 04/21/00 - allowing simple form of xmr param ref. */
   /* can't be IS - non loc - i.e. param that is assigned to */
   if (ndp->lu.sy->sytyp != SYM_N) break;

   np = ndp->lu.sy->el.enp;
   if (!np->n_isaparam) break; 

   if (np->srep != SR_PNUM)
    {
     __sgferr(3411,
      "heirarchical reference %s of non local parameter set by defparam or pound param illegal",
      ndp->ru.grp->gnam);

     ndp->optyp = NUMBER;
     ndp->consubxpr = TRUE;
     /* value is same width x */
     if (np->nwid <= WBITS)
      {
       ndp->ru.xvi = __alloc_shareable_cval(ALL1W, ALL1W, WBITS);
      }
     else
      {
       push_xstk_(xsp, np->nwid); 
       one_allbits_(xsp->ap, np->nwid);
       one_allbits_(xsp->bp, np->nwid);
       ndp->ru.xvi = __allocfill_cval_new(xsp->ap, xsp->bp, wlen_(np->nwid));
       __pop_xstk();
      }
     return(TRUE);
    }
   goto cnvt_gref_param;
  case LSB:
   /* must fold select expressions separately - but node never a const. */
   mark_constnd(ndp->ru.x, has_isform);
   break;
  case PARTSEL:
   mark_constnd(ndp->ru.x->lu.x, has_isform);
   mark_constnd(ndp->ru.x->ru.x, has_isform);
   break;
  case FCALL:
   /* must mark const. args as foldable but not fcall node */
   {
    register struct expr_t *fandp;

    /* even if consts in concatenate, entire thing is not */
    for (fandp = ndp->ru.x; fandp != NULL; fandp = fandp->ru.x)
     mark_constnd(fandp->lu.x, &is1);
   }
   break;
  case LCB:
   /* top concatentate left field unused */
   /* mark top for folding if all components constants */
   {
    register struct expr_t *ctndp;
    int32 canfold;

    cat_isform = FALSE;
    canfold = TRUE;
    for (ctndp = ndp->ru.x; ctndp != NULL;)
     {
      if (ctndp->optyp == CATCOM)
       {
        if (!mark_constnd(ctndp->lu.x, &is1)) canfold = FALSE;
        else { if (is1) cat_isform = TRUE; }
        ctndp = ctndp->ru.x;
       }
      else
       {
        if (!mark_constnd(ctndp, &is1)) canfold = FALSE;
        else { if (is1) cat_isform = TRUE; }
        break;
       }
     }
    if (canfold)
     {
      ndp->consubxpr = TRUE;
      if (cat_isform) { ndp->consub_is = TRUE; *has_isform = TRUE; } 
      return(TRUE);
     }
   }
   break;
  case QUEST:
   {
    int32 m1, m2, m3;

    /* only mark if all 3 constants - but really only need selector const */
    m1 = mark_constnd(ndp->lu.x, &is1);
    m2 = mark_constnd(ndp->ru.x->ru.x, &is2);
    m3 = mark_constnd(ndp->ru.x->lu.x, &is3);
    /* for now only folding if all 3 expressions constants */
    if (m1 && m2 && m3)
     {
      ndp->consubxpr = TRUE;
      /* if any IS then entire expression must eval to IS because selects */
      /* can differ - SJM 10/17/99 */
      if (is1 || is2 || is3 ) { ndp->consub_is = TRUE; *has_isform = TRUE; }
      return(TRUE);
     }
   }
   break;
  case CATREP:
   /* know left must be a constant */
   mark_constnd(ndp->ru.x, &is1);
   /* cat repeat value not simple number */
   if (is1) __misc_terr(__FILE__, __LINE__);
   break;
  default:
   /* know op. node to get here */
   is2 = FALSE;
   const_subtree = mark_constnd(ndp->lu.x, &is1);
   if (ndp->ru.x != NULL && !mark_constnd(ndp->ru.x, &is2))
    const_subtree = FALSE;
   ndp->consubxpr = (const_subtree) ? TRUE : FALSE;
   /* SJM 10/17/99 - must set has isform to propagate up tree */
   if (is1 || is2) { ndp->consub_is = TRUE; *has_isform = TRUE; } 
   return(const_subtree);
  }
 /* return for normal non constant case */
 ndp->consubxpr = FALSE;
 return(FALSE);
}

/*
 * eval all constant subnodes in tree - constant reduces to number node
 *
 * requires expr. node widths to be set before here
 * also __sfnam_ind must be set
 *
 * bit or part selects from parameters in rhs expressions are legal consts
 * but such selects cannot be used in param rhs defining expressions
 */
static void fold_const(struct expr_t *ndp)
{
 register int32 iti;
 int32 wlen, xreal, base, xwi, sav_xclen, xsign;
 word32 *wp; 
 double *dp, d1;
 struct xstk_t *xsp;

 wp = NULL;
 /* leaves already folded unless parameter */
 if (__isleaf(ndp))
  {
   if (ndp->optyp != ID) return;
   if (idnd_var(ndp->lu.sy)) return;
  }
 xsp = NULL;
 ndp->folded = TRUE;
 /* know sub expr. evaluates to constant */
 if (ndp->consubxpr)
  {
   /* --- DBG */
   if (__debug_flg)
    __dbg_msg("## folding constant expression %s\n",
     __msgexpr_tostr(__xs, ndp));
   /* --- */
   wlen = wlen_(ndp->szu.xclen);
   if (ndp->is_real) { xreal = TRUE; base = BDBLE; }
   else
    {
     xreal = FALSE;
     if (is_signed_decimal(ndp)) base = BDEC; else base = BHEX; 
    }
 

   /* case: foldable expression that does not contain or result in IS form */
   /* notice do not need current itree place here - will crash if problem */
   if (!ndp->consub_is)
    {
     xsp = __eval_xpr(ndp);

is_1inst:
     /* for constants at least need to change width of expression */ 
     /* SJM 09/29/03 - change to handle sign extension and separate types */
     if (xsp->xslen > ndp->szu.xclen) __narrow_sizchg(xsp, ndp->szu.xclen);
     else if (xsp->xslen < ndp->szu.xclen)
      {
       if (ndp->has_sign) __sgn_xtnd_widen(xsp, ndp->szu.xclen);
       else __sizchg_widen(xsp, ndp->szu.xclen);
      }

     /* AIV 09/29/04 - need to save the sign for a folded expr */
     xsign = ndp->has_sign; 
     __free2_xtree(ndp);
     __init_xnd(ndp);
     ndp->szu.xclen = xsp->xslen;
     ndp->has_sign = xsign; 

     /* if real know value will be real - no need for conversion */
     if (xreal)
      {
       memcpy(&d1, xsp->ap, sizeof(double)); 
       ndp->ru.xvi = __alloc_shareable_rlcval(d1);
       ndp->optyp = REALNUM;
       ndp->is_real = TRUE;
       ndp->has_sign = TRUE;
       ndp->ibase = BDBLE;
      }
     else
      {
       /* anything value set here must be (becomes) sized number */
       if (ndp->szu.xclen <= WBITS)
        {
         ndp->ru.xvi = __alloc_shareable_cval(xsp->ap[0], xsp->bp[0], 
          ndp->szu.xclen);
        } 
       else
        {
         ndp->ru.xvi = __allocfill_cval_new(xsp->ap, xsp->bp, wlen);
        }
       /* must set standard after folded expr. node flags */
       ndp->optyp = NUMBER;
       if (base == BDEC) { ndp->ibase = BDEC; ndp->has_sign = TRUE; }
      }
     __pop_xstk();
     ndp->consubxpr = TRUE;
     ndp->folded = TRUE;
    
     /* --- DBG --
     if (__debug_flg)
      __dbg_msg("## result is %s\n", __msgnumexpr_tostr(__xs, ndp, 0));
     --- */
     return;
    }
   /* for IS form only indication of real is is_real flag - and expr. and */
   /* replaced constant will have same value */ 
   if (xreal) xwi = __alloc_is_cval(__inst_mod->flatinum);
   else
    {
     xwi = __alloc_is_cval(wlen*__inst_mod->flatinum);
     wp = &(__contab[xwi]);
    }
   sav_xclen = ndp->szu.xclen;

   for (iti = 0; iti < __inst_mod->flatinum; iti++)
    {
     __inst_ptr->itinum = iti; 
     __inum = iti;
     /* 1 value per instance number */
     xsp = __eval_xpr(ndp);

     /* know if expr real all instances will eval to real */
     if (xreal)
      {
       memcpy(&d1, xsp->ap, sizeof(double)); 
       /* AIV 05/30/07 - if expression is real need to 2*__inum reals use */
       /* two words not one */
       dp = (double *) &(__contab[xwi + (2*__inum)]);
       *dp = d1;
      }
     else
      {
       /* SJM 09/29/03 - change to handle sign extension and separate types */
       if (xsp->xslen > ndp->szu.xclen) __narrow_sizchg(xsp, ndp->szu.xclen);
       else if (xsp->xslen < ndp->szu.xclen)
        {
         if (ndp->has_sign) __sgn_xtnd_widen(xsp, ndp->szu.xclen);
         else __sizchg_widen(xsp, ndp->szu.xclen);
        }

       memcpy(&(wp[2*wlen*__inum]), xsp->ap, 2*WRDBYTES*wlen);
      }

     /* this may no longer be IS form if only 1 inst - xstk popped at 1 inst */
     /* FIXME is this case possible - if so, memory leak */
     if (__inst_mod->flatinum == 1) { __pop_itstk(); goto is_1inst; }
     __pop_xstk();
    }

   __free2_xtree(ndp);
   __init_xnd(ndp);
   /* notice this eliminates width - so must be set from saved */
   ndp->szu.xclen = sav_xclen;
   ndp->ru.xvi = xwi;
   if (xreal) 
    {
     ndp->optyp = ISREALNUM; 
     /* AIV 05/30/07 - need to mark expression real if folded to real value */
     ndp->is_real = TRUE;
    }
   else ndp->optyp = ISNUMBER;
   ndp->consubxpr = TRUE;
   ndp->consub_is = TRUE;
   ndp->folded = TRUE;

   if (__debug_flg)
    {
     for (iti = 0; iti < __inst_mod->flatinum; iti++)
      {
       __force_base = BDEC;
       __dbg_msg("## IS form result for inst. no. %d is %s (width %d)\n", iti,
        __msgnumexpr_tostr(__xs, ndp, iti), ndp->szu.xclen);
       __force_base = BNONE;
      }
    }
   __inst_ptr->itinum = 0;
   __inum = 0;
   return;
  }

 /* must fold select expressions separately */
 /* but work is done in above part of this routine */ 
 switch ((byte) ndp->optyp) {
  case LSB:
   fold_const(ndp->ru.x);
   return;
  case PARTSEL:
   fold_const(ndp->ru.x->lu.x);
   fold_const(ndp->ru.x->ru.x);
   return;
  case QUEST:
  /* SJM 04/26/04 - must also catch real ?: cases and check leaves only */ 
  case REALREALQUEST:
  case REALREGQUEST:
  case REGREALQCOL:
   /* will not get here if all 3 constants - could fold if cond. const */
   /* if all 3 constants will not get here */
   fold_const(ndp->lu.x);
   fold_const(ndp->ru.x->lu.x);
   fold_const(ndp->ru.x->ru.x);
   return;
  case FCALL:
   {
    register struct expr_t *fandp;

    /* notice width already set for these arguments */
    for (fandp = ndp->ru.x; fandp != NULL; fandp = fandp->ru.x)
     {
      if (__debug_flg)
       __dbg_msg("## folding fcall argument expression %s\n",
        __msgexpr_tostr(__xs, fandp->lu.x));
      fold_const(fandp->lu.x);
     }
    }
   return;
  case LCB:
   /* for folding elements of concatenate when not all constants */
   {
    register struct expr_t *ctndp;

    for (ctndp = ndp->ru.x; ctndp != NULL; ctndp = ctndp->ru.x)
     {
      /* --- DBG
      if (__debug_flg && ctndp->lu.x->optyp != ID)
       {
        __dbg_msg("## folding non all constant concatenate expression %s\n",
  __msgexpr_tostr(__xs, ctndp->lu.x));
       }
      --- */
      fold_const(ctndp->lu.x);
     }
    /* now know all components are numbers */
   }
   break;
  case CATREP:
   /* concatenate repeat form expansion error */
   __misc_sgfterr(__FILE__, __LINE__);
   break;
  default:
   /* may be subexpressions of normal bin. or un. that can be folded */
   /* pass global context down - operators may change */
   /* know this is not leaf node */
   if (ndp->ru.x != NULL) fold_const(ndp->ru.x);
   if (ndp->lu.x != NULL) fold_const(ndp->lu.x);
 }
}

/*
 * check if constant signed decimal expression
 */
static int32 is_signed_decimal(struct expr_t *xp)
{
 switch ((byte) xp->optyp) { 
  case MINUS: case PLUS: case TIMES: case DIV: case MOD:
   if (xp->lu.x != NULL) if (!is_signed_decimal(xp->lu.x)) return(FALSE);
   if (xp->ru.x != NULL) return(is_signed_decimal(xp->ru.x));
   return(TRUE);
  case NUMBER: case ISNUMBER: 
   if (xp->szu.xclen == 1 || xp->szu.xclen > WBITS || xp->ibase != BDEC
    || !xp->has_sign) return(FALSE);
   break;
  default: return(FALSE); 
 }
 return(TRUE);
}

/*
 * ROUTINES TO CHECK AND NORMALIZE SELECT CONSTANT EXPRESIONS
 */

/*
 * check any psel or bsel constant index for in range and also
 * normalize
 */
static void chk_ndfolded_specops(struct expr_t *ndp)
{
 switch ((byte) ndp->optyp) {
  /* error for these already caught */
  case ID: case GLBREF: case NUMBER: case ISNUMBER:
  case REALNUM: case ISREALNUM: case OPEMPTY: 
   break;
  case LSB:
   /* know real number error already caught */
   /* but only call if constant - caller must check for required constant */
   if (ndp->ru.x->optyp == NUMBER || ndp->ru.x->optyp == ISNUMBER)
    chk_inrng_bsel(ndp);
   /* can have embedded selects */
   else chk_ndfolded_specops(ndp->ru.x);
   break;
  case PARTSEL:
   /* here - know selects must be numbers by here */
   chk_inrng_psel(ndp);
   break;
  case FCALL:
   /* this handles embedded special things like selects */
   chk_folded_fargs(ndp);
   break;
  default:
   if (ndp->lu.x != NULL) chk_ndfolded_specops(ndp->lu.x);
   if (ndp->ru.x != NULL) chk_ndfolded_specops(ndp->ru.x);
 }
}

/*
 * check a constant bit select for in range - if non constant must
 * check at run time and this routine not called
 * know index expression already checked and folded
 * return F on error
 *
 * 2nd part of bit select checking - requires pass 2 all parameter
 * substitution and splitting done
 */
static int32 chk_inrng_bsel(struct expr_t *ndp)
{
 int32 err;
 int32 ri1, ri2, obwid;
 struct expr_t *idndp, *rconx;
 struct net_t *np;
 char s1[RECLEN];
 
 idndp = ndp->lu.x;
 np = idndp->lu.sy->el.enp;
 rconx = ndp->ru.x;

 /* even if not number will be converted to 32 bit x */
 /* register bit select of x ok, means make it x on rhs and all x's on lhs */
 err = FALSE;

 ri1 = ri2 = -1;
 if (np->n_isarr) __getarr_range(np, &ri1, &ri2, &obwid);
 else if (np->n_isavec) __getwir_range(np, &ri1, &ri2);
 else __arg_terr(__FILE__, __LINE__);

 /* case 1: reg/real that can be array */
 if (np->ntyp >= NONWIRE_ST)
  {
   /* case 1b: array */ 
   if (np->n_isarr)
    {
     sprintf(s1, "array index of %s", np->nsym->synam);     
     if (!chknorm_range(rconx, ri1, ri2, s1, FALSE)) err = TRUE;
    }
   /* 2b: reg */
   else
    {
     sprintf(s1, "register bit select of %s", np->nsym->synam);     
     if (!__nd_ndxnum(rconx, s1, FALSE)) err = TRUE;
     else if (!chknorm_range(rconx, ri1, ri2, s1, FALSE))
      err = TRUE;
    }
  }
 /* case 2: wire */
 else
  {
   sprintf(s1, "wire bit select of %s", np->nsym->synam);     
   /* declarative either side select index must be non x/z */ 
   if (!__nd_ndxnum(rconx, s1, TRUE)) err = TRUE;
   /* if out of range, changed to end */
   else if (!chknorm_range(rconx, ri1, ri2, s1, FALSE)) err = TRUE;
  }
 return(!err);
}

/*
 * check and emit error for out of range condition
 * also normalize if no error
 *
 * for all selects - string of type passed
 * know value number or is number or will not be passed
 */
static int32 chknorm_range(struct expr_t *rconx, int32 ri1, int32 ri2,
 char *emsg, int32 emit_err)
{
 register int32 iti, iti2;
 int32 err, indval, newindval, errindval, alloc_new_con;
 word32 *wp, *wp2;

 if (rconx->optyp == NUMBER)
  {
   /* err/warn already emitted for this case */
   if (__contab[rconx->ru.xvi + 1] != 0) return(FALSE);
  
   indval = (int32) __contab[rconx->ru.xvi];
   /* ---
   if (__debug_flg) 
    __dbg_msg("$$$ in range check of %d in [%d:%d] - line %d\n",
     indval, ri1, ri2, __slin_cnt);
   --- */
   if (!in_range(indval, ri1, ri2, &errindval))
    {
     if (emit_err)
      __sgferr(836, "%s %d out of range [%d:%d] - end used", emsg, indval,
       ri1, ri2);
     else __sgfwarn(547, "%s %d out of range [%d:%d] - end used", emsg,
      indval, ri1, ri2);
     
     indval = errindval;
     rconx->ru.xvi = __alloc_shareable_cval((word32) indval, 0, WBITS);
    }
   /* normalize */
   newindval = normalize_ndx_(indval, ri1, ri2);
   if (newindval != indval)
    {
     rconx->ru.xvi = __alloc_shareable_cval((word32) newindval, 0, WBITS);
     rconx->ind_noth0 = TRUE;
    }
   return(TRUE);
  }
 err = FALSE;
 alloc_new_con = FALSE;
 wp = &(__contab[rconx->ru.xvi]);
 wp2 = NULL;
 for (iti = 0; iti < __inst_mod->flatinum; iti++)
  {
   /* must normal any that are good */
   if (wp[2*iti + 1] != 0L) continue; 
   indval = (int32) wp[2*iti];
   if (!in_range(indval, ri1, ri2, &errindval))
    {
     if (emit_err)
      __sgferr(837, "%s %d out of range [%d:%d] (inst. %d)", emsg,
       indval, ri1, ri2, iti);
     else __sgfwarn(548, "%s %d out of range [%d:%d] (inst %d)", emsg,
      indval, ri1, ri2, iti);
     if (!alloc_new_con)
      { 
       /* allocate new and copy up to current */
       rconx->ru.xvi = __alloc_is_cval(__inst_mod->flatinum);
       wp2 = &(__contab[rconx->ru.xvi]);
       alloc_new_con = TRUE;
       for (iti2 = 0; iti2 < iti; iti2++)
        {
         wp2[2*iti2] = wp[2*iti2];
         wp2[2*iti2 + 1] = wp[iti2 + 1]; 
        }
      }

     wp2[2*iti] = indval = errindval;
     /* SJM 12/22/03 - on error use end of range but b part must be set to 0 */
     wp2[2*iti + 1] = 0;
     err = TRUE;
    }
   /* know range are always non is form numbers - will have split if needed */
   /* normalize */
   newindval = normalize_ndx_(indval, ri1, ri2);
   /* notice if any not h:0 form all will be */
   if (alloc_new_con)
    {
     /* once any changed all must be copied */
     wp2[2*iti] = newindval;
     wp2[2*iti + 1] = 0;
     /* SJM 12/22/03 - if out of rng caused alloc new this may chg h:0 */
     if (newindval != indval) rconx->ind_noth0 = TRUE;
    }
   else if (newindval != indval)
    {
     /* allocate new and copy up to current */
     rconx->ru.xvi = __alloc_is_cval(__inst_mod->flatinum);
     wp2 = &(__contab[rconx->ru.xvi]);
     alloc_new_con = TRUE;
     for (iti2 = 0; iti2 < iti; iti2++)
      {
       wp2[2*iti2] = wp[2*iti2];
       wp2[2*iti2 + 1] = wp[iti2 + 1]; 
      }
     /* SJM 12/22/03 - must fill and set only when allocating new */ 
     /* this was wrongly out of loop so if same, wp2 was nil */
     wp2[2*iti] = newindval;
     wp2[2*iti + 1] = 0;
     rconx->ind_noth0 = TRUE;
    }
  }
 return(!err);
}

/*
 * return T if value av between v1a and v2a
 * if F, sets newv1a and newv2a to new extrema
 */
static int32 in_range(int32 ai, int32 v1a, int32 v2a, int32 *newai)
{
 /* case 1 low to high */
 if (v1a < v2a)
  {
   if (ai < v1a) { *newai = v1a; return(FALSE); }
   if (ai > v2a) { *newai = v2a; return(FALSE); }
   return(TRUE);
  }
 /* case 2 - normal high to low */
 if (ai > v1a) { *newai = v1a; return(FALSE); }
 if (ai < v2a) { *newai = v2a; return(FALSE); }
 return(TRUE);
}

/*
 * check a part select for in range - must always be constant
 * know index expression already checked and folded
 * but need to convert to WBITS 
 *
 * 2nd part of bit select checking - requires pass 2 all parameter
 * substitution and splitting done
 */
static int32 chk_inrng_psel(struct expr_t *ndp)
{
 int32 err, ri1, ri2, pseli1, pseli2;
 struct expr_t *idndp, *rcon1, *rcon2;
 struct net_t *np;
 char s1[RECLEN];
 
 idndp = ndp->lu.x;
 np = idndp->lu.sy->el.enp;
 rcon1 = ndp->ru.x->lu.x;
 rcon2 = ndp->ru.x->ru.x;

 /* even if not number will be converted to 32 bit x */
 /* always error for part select range x/z - what does x in range mean */
 err = FALSE;
 sprintf(s1, "1st part select index of %s", np->nsym->synam);
 if (!__nd_ndxnum(rcon1, s1, TRUE)) err = TRUE;
 sprintf(s1, "2nd part select index of %s", np->nsym->synam);
 if (!__nd_ndxnum(rcon2, s1, TRUE)) err = TRUE;

 /* parts selects of arrays not in language */
 __getwir_range(np, &ri1, &ri2);
 /* check psel direction and wire direction same */
 /* if errror (x/z for example) in range, cannot check direction */
 if (!err)
  {
   pseli1 = (int32) __contab[rcon1->ru.xvi];
   pseli2 = (int32) __contab[rcon2->ru.xvi];
   if ((ri1 < ri2 && pseli1 > pseli2) || (ri1 > ri2 && pseli1 < pseli2))
    {
      __sgferr(879,
       "part select %s range direction conflicts with net %s[%d:%d]",
       __msgexpr_tostr(__xs, ndp), __to_idnam(ndp->lu.x), ri1, ri2);
      err = TRUE;
    }
  }
 /* even if error - try to check and normalize - ignores any x's */
 sprintf(s1, "1st part select index of %s", np->nsym->synam);
 if (!chknorm_range(rcon1, ri1, ri2, s1, FALSE))
  err = TRUE;
 sprintf(s1, "2nd part select index of %s", np->nsym->synam);
 if (!chknorm_range(rcon2, ri1, ri2, s1, FALSE))
  err = TRUE;
 /* since will change out of range to top - need to reset widths */
 /* if no change will be same */
 pseli1 = (int32) __contab[rcon1->ru.xvi];
 pseli2 = (int32) __contab[rcon2->ru.xvi];
 ndp->szu.xclen = pseli1 - pseli2 + 1;  
 return(!err);
}

/*
 * check function call arguments after folding
 * mainly for checking the various special system function arguments 
 * but also check actual and formal argument number
 */
static void chk_folded_fargs(struct expr_t *ndp)
{
 register struct expr_t *fandp;
 register struct task_pin_t *tpp;
 int32 pwid;
 struct sy_t *syp;
 struct expr_t *idndp;
 struct task_t *tskp;

 idndp = ndp->lu.x;
 syp = idndp->lu.sy;

 /* SJM 10/08/04 - LOOKATME - think never need new ver 2001 x/z widening? */
 /* this checks system function arguments */
 if (syp->sytyp == SYM_SF) { chk_sysfargs_syntax(ndp); return; }

 /* can have embedded fcalls and selects - check before arg no. match */
 /* notice checking these even for undeclared function */
 for (fandp = ndp->ru.x; fandp != NULL; fandp = fandp->ru.x) 
  chk_ndfolded_specops(fandp->lu.x);

 /* if funcdef error, will not be declard and also cannot check params */
 /* but undeclared error already emitted */
 if (!syp->sydecl) return;

 /* final check is formal and actual number must match for user functions */ 
 /* for now allowing conversions of any arg real or reg to any other */
 /* in apl sense - yes, semantics allows assignments in both directions */

 tskp = syp->el.etskp;
 /* first pin is dummy return type */
 tpp = tskp->tskpins->tpnxt;
 /* point 1 past extra first argument - not part of source call */
 fandp = ndp->ru.x;
 for (; tpp != NULL; tpp = tpp->tpnxt, fandp = fandp->ru.x)
  {
   /* move to next actual argument */
   if (fandp == NULL)
    {
     __sgferr(840,
      "function %s called with too few arguments", __to_idnam(idndp));
     return;
    }

   /* notice x/z widening does not effect num arg checking */


   /* nwid field not set yet */
   pwid = __get_netwide(tpp->tpsy->el.enp);
   /* SJM 10/08/04 - for Ver 2001 because WBITS can be 64 - must widen */
   /* unsized constant (unsiznum bit in expr rec on) to lhs width */
   /* folded bit not set for these but will be folded to number */

   if (fandp->lu.x->optyp == NUMBER && fandp->lu.x->unsiznum
    && fandp->lu.x->szu.xclen < pwid)
    {
     fandp->lu.x = __widen_unsiz_rhs_assign(fandp->lu.x, pwid);
    }
  }
 if (fandp != NULL)
  __sgferr(841, "function %s called with too many arguments",
   __to_idnam(idndp));
}

/*
 * ROUTINE TO CHECK AND CONVERT NUMBERS THAT MUST BE INDEXES
 */

/*
 * convert a number (non real) to 32 bit non x/z constant
 * if possible - else error or warn - return F on error
 *
 * called after structural checking and constant folding
 * always converts but returns FALSE if losing something
 * and make value WBIT wide x and emits error 
 * for indices and ranges - can never be real and no scaling
 *
 * must be called with itree loc (inst and mod set)
 */
extern int32 __nd_ndxnum(struct expr_t *ndp, char *emsg, int32 emit_err)
{
 int32 wlen, err;
 word32 *ap, *bp;
 word32 av, bv;

 switch ((byte) ndp->optyp) {
  case ISNUMBER: return(nd_ndxisnum(ndp, emsg, emit_err));
  case NUMBER:
   wlen = wlen_(ndp->szu.xclen);
   ap = &(__contab[ndp->ru.xvi]);
   bp = &(ap[wlen]);
   if (ndp->szu.xclen > WBITS)
    {
     /* here always free-alloc to work constant */
     __free2_xtree(ndp);
     __init_xnd(ndp);
     ndp->optyp = NUMBER;
     ndp->szu.xclen = WBITS;


     if (!vval_is0_(&(ap[1]), ndp->szu.xclen - WBITS)
      || !vval_is0_(bp, ndp->szu.xclen))
      { 
       av = bv = ALL1W; 
       __force_base = BDEC;
       __msgexpr_tostr(__xs, ndp);
       __force_base = BNONE;
       err = TRUE;
      } 
     else { av = ap[0]; bv = 0L; err = FALSE; }
     /* know this is WBITS */
     ndp->ru.xvi = __alloc_shareable_cval(av, bv, WBITS);

     if (err) goto wr_err;
     return(TRUE);
    }
   ndp->szu.xclen = WBITS;
   if (bp[0] == 0L) return(TRUE);
   __force_base = BDEC;
   __msgexpr_tostr(__xs, ndp);
   __force_base = BNONE;
   ndp->ru.xvi = __alloc_shareable_cval(ALL1W, ALL1W, WBITS);
   goto wr_err;
 } 
 /* SJM 05/22/00 - need to save expr for error message before cnv to x */
 __force_base = BDEC;
 __msgexpr_tostr(__xs, ndp);
 __force_base = BNONE;

 /* normal expr - must convert to x */
 __free2_xtree(ndp);
 __init_xnd(ndp);
 ndp->optyp = NUMBER;
 ndp->szu.xclen = WBITS;
 ndp->ru.xvi = __alloc_shareable_cval(ALL1W, ALL1W, WBITS);

wr_err:
 if (emit_err) 
  __sgferr(902, "%s value %s not required %d bit non x/z number",
   emsg, __xs, WBITS);
 else __sgfwarn(582, "%s value %s not required %d bit non x/z number",
  emsg, __xs, WBITS);
 return(FALSE);
}

/*
 * need an index form number that fits in 32 or 64 for delays bits
 * know type is ISNUMBER or will not be called
 * return F on error (not good)
 *
 * always converts but returns FALSE if losing something
 * and make value WBIT x
 * assumes itree loc pushed
 */
static int32 nd_ndxisnum(struct expr_t *ndp, char *emsg, int32 emit_err)
{
 register int32 iti;
 int32 good, wlen;
 word32 *wp, *wp2, *ap, *bp;
 
 /* first do the checking */
 good = TRUE;
 wlen = wlen_(ndp->szu.xclen);
 wp = &(__contab[ndp->ru.xvi]);

 for (iti = 0; iti < __inst_mod->flatinum; iti++)
  {
   ap = &(wp[2*wlen*iti]);
   bp = &(ap[wlen]);
   if (ndp->szu.xclen > WBITS)
    {
     if (!vval_is0_(&(ap[1]), ndp->szu.xclen - WBITS) 
      || !vval_is0_(bp, ndp->szu.xclen)) goto x_err;
     continue;
    }
   if (bp[0] == 0L) continue;

x_err:
   __cur_sofs = 0;
   __force_base = BDEC;
   __msgnumexpr_tostr(__xs, ndp, iti);
   __force_base = BNONE;
   if (emit_err) 
    __sgferr(902, "%s value %s not required %d bit non x/z number (inst. %d)",
     emsg, __xs, WBITS, iti);
   else
    __sgfwarn(582, "%s value %s not required %d bit non x/z number (inst. %d)",
     emsg, __xs, WBITS, iti);
   good = FALSE;
   ndp->ru.xvi = __alloc_shareable_cval(ALL1W, ALL1W, WBITS);
  }
 /* next convert if needed */
 if (ndp->szu.xclen > WBITS)
  {
   /* must access old expression constant value before freeing */
   wp = &(__contab[ndp->ru.xvi]);

   __free2_xtree(ndp);
   __init_xnd(ndp);
   ndp->optyp = ISNUMBER;
   ndp->szu.xclen = WBITS;
   /* arg is words */
   ndp->ru.xvi = __alloc_is_cval(__inst_mod->flatinum);
   wp2 = &(__contab[ndp->ru.xvi]);

   for (iti = 0; iti < __inst_mod->flatinum; iti++)
    {
     ap = &(wp[2*wlen*iti]);
     bp = &(ap[wlen]);
     wp2[2*iti] = ap[0];
     wp2[2*iti + 1] = bp[0];
    }
  }
 else ndp->szu.xclen = WBITS;
 return(good);
}

/*
 * SYSTEM FUNCTION ARGUMENT CHECKING ROUTINES
 */

/*
 * check system functions call according to specific task argument
 * requirements not needed for user funcs, type mismatches just fixed
 * with copy there
 *
 * this routine must call chk rhsexpr for every argument
 * except know global and param substitution done
 * ndp is expr. node (i.e. left node is func name and right is args)  
 *
 * here few special sys func. arg lists cause net to not be deleteable
 * but not called for all non special
 */
static void chk_sysfargs_syntax(struct expr_t *ndp)
{
 register struct expr_t *fandp;
 int32 anum, special_syntax;
 struct sy_t *syp;
 struct sysfunc_t *syfp;
 struct expr_t *fax;
 struct net_t *np;

 syp = ndp->lu.x->lu.sy;
 syfp = syp->el.esyftbp;
 anum = __cnt_tfargs(ndp->ru.x);

 /* special check for 1 arg. */ 
 if (anum == 1 && ndp->ru.x->lu.x->optyp == OPEMPTY)
  {
   __sgfwarn(633,
    "system function call: %s(); has one empty argument - for no arguments omit the ()",
    syp->synam);
  }

 special_syntax = FALSE;
 switch (syfp->syfnum) {
  /* functions that require special handling */
  case STN_ITOR:
   if (anum != 1) { sf_errifn(syp, 1); break; }
   fax = ndp->ru.x->lu.x;
   /* 32 bit width dependendent */
   /* this should probably allow 64 bit integers */
   __chk_rhsexpr(fax, WBITS);
   if (fax->szu.xclen > WBITS || fax->is_real)
    __sgferr(859,
     "%s system function call argument %s not required width %d integer or reg",
     syp->synam, __msgexpr_tostr(__xs, fax), WBITS);
   special_syntax = TRUE;
   break;
  case STN_BITSTOREAL:
   if (anum != 1) { sf_errifn(syp, 1); break; }
   fax = ndp->ru.x->lu.x;
   /* must set arg. rhs expression width using 64 bit context */
   /* just overwrites one that is there */
   __chk_rhsexpr(fax, REALBITS);
   /* 64 bit width dependendent */
   if (fax->szu.xclen != 64 || fax->is_real)
    __sgfwarn(634,
     "%s system function call argument %s should be 64 bit wide non real",
     syp->synam, __msgexpr_tostr(__xs, fax));
   special_syntax = TRUE;
   break;
  case STN_RTOI:
  case STN_REALTOBITS:
   if (anum != 1) { sf_errifn(syp, 1); break; } 
   fax = ndp->ru.x->lu.x;
   __chk_rhsexpr(fax, 0);
   if (!fax->is_real)
    __sgferr(843,
     "%s system function call argument %s is not required real expression",
     syp->synam, __msgexpr_tostr(__xs, fax));
   special_syntax = TRUE;
   break;
  case STN_SIGNED:
   if (anum != 1) { sf_errifn(syp, 1); break; } 
   fax = ndp->ru.x->lu.x;
   /* no context for this special case sys func that does nothing */
   /* and whose return value width is same as arg width */
   __chk_rhsexpr(fax, 0);
   if (fax->has_sign)
    __sgfinform(3007,
     "argment %s to new 2001 $signed system task already signed",
     __msgexpr_tostr(__xs, fax));
   special_syntax = TRUE;
   break;
  case STN_UNSIGNED:
   if (anum != 1) { sf_errifn(syp, 1); break; } 
   fax = ndp->ru.x->lu.x;
   /* no context for this special case sys func that does nothing */
   /* and whose return value width is same as arg width */
   __chk_rhsexpr(fax, 0);
   if (!fax->has_sign)
    __sgfinform(3007,
     "argment %s to new 2001 $word32 system task already unsigned",
     __msgexpr_tostr(__xs, fax));
   special_syntax = TRUE;
   break;
  case STN_COUNT_DRIVERS:
   chksf_count_drivers(ndp, anum);
   special_syntax = TRUE;
   break;
  case STN_GETPATTERN:
   __rhs_isgetpat = TRUE;
   chksf_getpattern(syp, ndp, anum);
   special_syntax = TRUE;
   break;
  case STN_SCALE:
   /* this takes one module value (cannot be expr.) that should be time */
   special_syntax = TRUE;
   if (anum != 1) { sf_errifn(syp, 1); break; }
   fax = ndp->ru.x->lu.x;
   if (fax->optyp != GLBREF)
    {
     __sgferr(844, 
      "$scale system function argument %s must be cross module hierarchical reference",
      __msgexpr_tostr(__xs, fax));
     break;
    }
   syp = fax->lu.sy;
   if (syp->sytyp == SYM_M || syp->sytyp == SYM_I)
    __arg_terr(__FILE__, __LINE__);
   np = syp->el.enp;
   if (__get_netwide(np) > TIMEBITS)  
    __sgfwarn(637,
     "$scale argument %s value wider than %d bits - will be truncated",
     fax->ru.grp->gnam, TIMEBITS); 
   break;
  case STN_Q_FULL:
   chksf_q_full(ndp, anum);
   special_syntax = TRUE;
   break;
  case STN_RANDOM:
   /* various dist. of random number generators - all params ins, ret. int32 */
   chksf_rand(ndp, 1, anum);
   special_syntax = TRUE;
   break;
  case STN_DIST_CHI_SQUARE:
  case STN_DIST_EXPONENTIAL:
  case STN_DIST_POISSON:
  case STN_DIST_T:
   chksf_rand(ndp, 2, anum);
   special_syntax = TRUE;
   break;
  case STN_DIST_UNIFORM:
  case STN_DIST_NORMAL:
  case STN_DIST_ERLANG:
   chksf_rand(ndp, 3, anum);
   special_syntax = TRUE;
   break;

  /* functions that must not have any arguments */
  case STN_REALTIME: case STN_STIME: case STN_TIME:
  case STN_STICKSTIME: case STN_TICKSTIME:
  case STN_RESET_COUNT: case STN_RESET_VALUE:
   if (anum != 0)
    {
     /* needs to be warning and throw away arguments */
     __sgferr(846,
      "%s system function has %d arguments but no arguments allowed",
      syp->synam, anum);
     ndp->ru.x = NULL;
    }
   break;

  /* functions that take exactly one argument */
  case STN_FOPEN:
   /* AIV 09/08/03 - changed now can take one or 2 args */
   /* $fopen([filename]) (mcd form) or $fopen([filename], [open type str]) */
   if (anum !=1 && anum !=2)        
    {
     __sgferr(851,
     "%s system function illegal number of arguments (%d) - 1 or 2 legal",
     syp->synam, anum);
    }
   break; 
  /* AIV 09/08/03 - new fileio system functions */
  case STN_FGETC: case STN_FTELL: case STN_REWIND:
   /* code = $fgetc([fd]), code = $ftell([fd]), code = $rewind([fd]) */
   if (anum != 1) sf_errifn(syp, 1);
   break;
  case STN_UNGETC:
   /* code = $ungetc([char], [fd]) - [fd] has high bit on */ 
   if (anum != 2) sf_errifn(syp, 2);
   break;
  case STN_FSEEK:
   /* code = $fseek([fd], [offset], [operation]) - [fd] has high bit on */ 
   /* $fseek([fd], [offset], [operation]) */
   if (anum != 3) sf_errifn(syp, 3);
   break; 
  case STN_FGETS:
   /* code = $fgets([proc lhs], [fd]) */ 
   /* this inhibits normal rhs checking of args */ 
   special_syntax = TRUE;
   if (anum != 2) { sf_errifn(syp, 2); break; }
   fax = ndp->ru.x;
   __chk_lhsexpr(fax->lu.x, LHS_PROC); 
   fax = fax->ru.x;
   __chk_rhsexpr(fax->lu.x, 32);
   break;
  case STN_FERROR:
   /* [user errno] = $ferror([fd], [proc lhs]) */ 
   /* this inhibits normal rhs checking of args */ 
   if (anum != 2) { sf_errifn(syp, 3); return; }
   fax = ndp->ru.x;
   __chk_rhsexpr(fax->lu.x, 32);
   fax = fax->ru.x;
   __chk_lhsexpr(fax->lu.x, LHS_PROC); 
   special_syntax = TRUE;
   break;
  case STN_FSCANF: case STN_SSCANF:
   /* $fscanf([fd], ...) or $sscanf([proc lhs], ...) */
   if (anum < 3)
    {
     __sgferr(851, "%s system function does not have required three arguments",
      syp->synam);
     return;
    }
   fax = ndp->ru.x;
   /* width self determined */
   __chk_rhsexpr(fax->lu.x, 0);
   fax = fax->ru.x;
   fax = fax->ru.x;
   /* width self determined */
   __chk_rhsexpr(fax->lu.x, 0);
   for (fandp = fax->ru.x; fandp != NULL; fandp = fandp->ru.x)
    {
     __chk_lhsexpr(fandp->lu.x, LHS_PROC);
    }
   special_syntax = TRUE;
   break;
  case STN_FREAD:
   /* for reg: code = $fgets([proc lhs], [fd]) - size from [proc lhs] size */ 
   /* for mem $fgets(mem, [fd], {[start], [count] optional}) */
   if (anum < 2 || anum > 4)        
    {
     __sgferr(851,
      "%s system function illegal number of arguments (%d) - 2, 3, or 4 legal",
      syp->synam, anum);
    }
   fax = ndp->ru.x;
   /* if first arg is memory and not indexed ok */
   if (fax->lu.x->optyp == ID || fax->lu.x->optyp == GLBREF)
    {
     if (fax->lu.x->lu.sy->el.enp->n_isarr) goto first_arg_ok;
    }
   __chk_lhsexpr(fax->lu.x, LHS_PROC);
first_arg_ok:
   /* check rest of args as normal rhs */
   for (fandp = fax->ru.x; fandp != NULL; fandp = fandp->ru.x)
    {
     fax = fandp->lu.x;
     /* next 3 are 32 bits numbers and optional */ 
     __chk_rhsexpr(fax, WBITS);
    }
   special_syntax = TRUE;
   break;
  case STN_TESTPLUSARGS:
   if (anum != 1) sf_errifn(syp, 1);
   break;
  case STN_SCANPLUSARGS:  
  case STN_VALUEPLUSARGS:  
   if (anum != 2) sf_errifn(syp, 2);
   fandp = ndp->ru.x; 
   if (anum >= 1) { fax = fandp->lu.x; __chk_rhsexpr(fax, 0); }
   if (anum >= 2)
    { fandp = fandp->ru.x; fax = fandp->lu.x; __chk_lhsexpr(fax, LHS_PROC); }
   special_syntax = TRUE;
   break;
  /* SJM 03/09/00 - added built-in math functins and can be used with */
  /* constant args in parameter expressions */
  case STN_COS: case STN_SIN: case STN_TAN: case STN_ACOS: case STN_ASIN:
  case STN_ATAN: case STN_COSH: case STN_SINH: case STN_TANH:
  case STN_INT: case STN_SGN:
  case STN_LN: case STN_LOG10: case STN_ABS: case STN_SQRT: case STN_EXP:
  case STN_HSQRT: case STN_HLOG: case STN_HLOG10:
  case STN_ATANH: case STN_ACOSH: case STN_ASINH:
   if (anum != 1) sf_errifn(syp, 1);
   break;
  case STN_POW: case STN_MIN: case STN_MAX:
  case STN_HPOW: case STN_HPWR: case STN_HSIGN: case STN_HDB:
  case STN_HYPOT: case STN_ATAN2:
   if (anum != 2) sf_errifn(syp, 2);
   break;
  default: 
   if (syfp->syfnum >= BASE_VERIUSERTFS && (int32) syfp->syfnum <= __last_systf)
    {
     /* chk and bld user tf_ or systf vpi_ pli function or real function */
     chkbld_pli_func(ndp, (int32) syfp->syfnum);
     return;
    }
   /* fall through on built-in system function */
  }
 if (special_syntax) return;

 /* if this is not executed width not check for wrong array/event usage */
 /* or bit/part select problems */
 /* this checks all of argument as rhs expr. - prev. chk rhsexpr stopped */
 /* when hit system function */
 for (fandp = ndp->ru.x; fandp != NULL; fandp = fandp->ru.x)
  {
   fax = fandp->lu.x;
   __chk_rhsexpr(fax, 0);
  }
}

/*
 * check build tf rec for pli function
 *
 * this is passed top of fcall expr. node
 * know this is called with __cur task and __cur mod set
 * do everything but call user checktf even if errors
 * will probably crash here if user pointers bad
 */
static void chkbld_pli_func(struct expr_t *fcallx, int32 sfnum)
{
 int32 sav_errcnt, anum;
 struct tfrec_t *tfrp;
 struct expr_t *cpfcallx;

 sav_errcnt = __pv_err_cnt;

 /* also check syntax of argument list */
 /* this sets tf_rw flags */
 if (fcallx->ru.x != NULL)
  { 
   chk_pli_arglist(fcallx->ru.x, sfnum); 
   anum = __cnt_tfargs(fcallx->ru.x); 
  }
 else anum = 0;

 /* need separate routine for vpi_ systfs compiletf and maybe sizetf */
 if (sfnum > __last_veriusertf)
  {
   __chkbld_vpi_systf_func(fcallx);
   return;
  }

 /* need both direction links */
 /* SJM 04/06/07 since free del list expr calls after delay must make */
 /* copy here to handle case of tf sysfunc that is delay expr */
 cpfcallx = __copy_expr(fcallx);

 /* allocate the tf aux d.s. and link on to func. name expr unused len fld */
 /* every call of tf_ system function needs d.s. chges (pvc flags etc) */
 /* AIV 05/17/07 - need to pass the new copied expression - not the actual */
 tfrp = chkalloc_tfrec(cpfcallx->ru.x, anum);
 tfrp->tf_func = TRUE;
 tfrp->tfu.callx = cpfcallx;
 /* need to set the xfrec for the copied expression as well */
 cpfcallx->lu.x->szu.xfrec = tfrp;

 fcallx->lu.x->szu.xfrec = tfrp;
 if (__tfrec_hdr == NULL) __tfrec_hdr = __tfrec_end = tfrp;
 else { __tfrec_end->tfrnxt = tfrp; __tfrec_end = tfrp; }

 /* cannot call tf routines if errors */
 if (__pv_err_cnt != sav_errcnt) return;

 /* call the sizetf - this sets the func. return width in tf rec */
 /* must he called or cannot check rhs exprs */
 __pli_func_sizetf(fcallx);
 /* AIV 05/17/07 - need to set size of copied function as well */
 __pli_func_sizetf(cpfcallx);
}

/*
 * check PLI _tf and vpi_ systf routine argument syntax  
 * notice build aux. d.s. and call checktf routine during prep for tf_
 */
static void chk_pli_arglist(register struct expr_t *argxp, int32 stfnum)
{
 int32 lhstyp;
 struct expr_t *xp;
 struct sy_t *syp;
 
 /* right offspring is first argument */
 for (; argxp != NULL; argxp = argxp->ru.x)
  {
   xp = argxp->lu.x;
   /* XMR task and inst scope arguments legal and are not checked */
   /* PLI deals with argument */
   /* if expr. can't contain scope object - i.e. this is special form */
   if (xp->optyp == GLBREF)
    {
     syp = xp->lu.sy; 
     /* LOOKATME - why are top level modules SYM M not SYM I? */
     if (syp->sytyp == SYM_M) continue; 

     if (syp->sytyp == SYM_I || syp->sytyp == SYM_TSK
      || syp->sytyp == SYM_LB) continue;  

     /* function scope object illegal - will be converted to fcall */
     /* DBG remove --- */
     if (syp->sytyp == SYM_F) __misc_terr(__FILE__, __LINE__);
     /* --- */
     /* fall through normal XMR */
    }

   /* for vpi_ system tasks or functions, entire array legal */
   if (stfnum > __last_veriusertf)
    {
     if ((xp->optyp == ID || xp->optyp == GLBREF) && xp->lu.sy->sytyp == SYM_N
      && xp->lu.sy->el.enp->n_isarr) continue;
    }
   /* can be assigned to must be treated as lhs expr */
   if (tfexpr_isrw(xp))
    {
     if (lhs_is_decl(xp)) lhstyp = LHS_DECL; else lhstyp = LHS_PROC;
     __chk_lhsexpr(xp, lhstyp);
     xp->tf_isrw = TRUE;
    }
   else __chk_rhsexpr(xp, 0);
  }
}

/*
 * for user tf (task or func.) args - set tf_isrw if expr can be assigned to
 * concatenates are legal Verilog but not PLI tf_ lvalues
 */
static int32 tfexpr_isrw(struct expr_t *ndp)
{
 struct net_t *np;

 /* only simplify/check bit and part select expressions (not lvals) */
 switch ((byte) ndp->optyp) {
  case ID: case GLBREF:
   /* notice even for global lu.x actual symbol set */
   if (ndp->lu.sy->sytyp != SYM_N) return(FALSE);
   np = ndp->lu.sy->el.enp;
   /* possible for unexpanded parameters here */ 
   if (np->n_isaparam) return(FALSE);
   return(TRUE);
  /* this include array index */
  case LSB: case PARTSEL: return(TRUE);
 }
 return(FALSE);
}

/*
 * return T if lhs rw expr. is declarative else F is procedural 
 */
static int32 lhs_is_decl(struct expr_t *xp)
{
 struct net_t *np;

 if (xp->optyp == LSB || xp->optyp == PARTSEL)
  np = xp->lu.x->lu.sy->el.enp; 
 else np = xp->lu.sy->el.enp;
 if (np->ntyp >= NONWIRE_ST) return(FALSE);
 return(TRUE);
}

/*
 * alloc and initialize a tf_ auxialiary record
 * always set 0th arg. (for tf_ func only) to nil here
 */
static struct tfrec_t *chkalloc_tfrec(struct expr_t *argxp,
 int32 anum)
{ 
 register int32 i;
 int32 insts;
 struct tfrec_t *tfrp;

 if (__inst_mod == NULL) insts = 1;
 else insts = __inst_mod->flatinum;
 tfrp = (struct tfrec_t *) __my_malloc(sizeof(struct tfrec_t)); 
 tfrp->tf_func = FALSE;
 tfrp->fretreal = FALSE;
 tfrp->tffnam_ind = __sfnam_ind;
 tfrp->tflin_cnt = __slin_cnt;
 tfrp->tfu.callx = NULL;
 /* this is number - 1 since 0th is unused for task ret. val for func. */ 
 tfrp->tfanump1 = anum + 1;
 tfrp->fretsiz = -1;
 tfrp->tf_inmdp = __inst_mod;
 tfrp->tf_intskp = __cur_tsk;
 tfrp->tfargs = alloc_tfarg(argxp, anum + 1, insts);

 tfrp->asynchon = (byte *) __my_malloc(insts); 
 memset(tfrp->asynchon, 0, insts);
 /* some save area for user between calls but rather pointless */
 tfrp->savwrkarea = (char **) __my_malloc(insts*sizeof(char *));
 tfrp->pvcdcep = (struct dceauxlst_t **)
  __my_malloc(insts*sizeof(struct dceauxlst_t *));
 tfrp->sync_tevp = (i_tev_ndx *) __my_malloc(insts*sizeof(i_tev_ndx)); 
 tfrp->rosync_tevp = (i_tev_ndx *) __my_malloc(insts*sizeof(i_tev_ndx)); 
 tfrp->setd_telst = (struct tevlst_t **) 
  __my_malloc(insts*sizeof(struct tevlst_t *));
 for (i = 0; i < insts; i++)
  {
   tfrp->savwrkarea[i] = NULL;
   tfrp->pvcdcep[i] = NULL;
   tfrp->sync_tevp[i] = -1;
   tfrp->rosync_tevp[i] = -1;
   tfrp->setd_telst[i] = NULL;
  }
 tfrp->tfrnxt = NULL;
 return(tfrp);
}

/*
 * allocate and initialize the tf argument record array
 * notice this is an array not an array of ptrs to the records
 */
static struct tfarg_t *alloc_tfarg(register struct expr_t *argxp,
 int32 anump1, int32 insts)
{
 register int32 i, pi;
 register struct expr_t *xp;
 struct tfarg_t *tfatab, *tfap;
 
 /* notice if no args still will allocate 1 even if tf_ task */
 tfatab = (struct tfarg_t *) __my_malloc(anump1*sizeof(struct tfarg_t));
 tfap = &(tfatab[0]); 
 tfap->arg.awp = NULL; 
 tfap->old_pvc_flgs = NULL;
 tfap->sav_pvc_flgs = NULL;
 tfap->sav_xinfos = NULL;
 tfap->tfdrv_wp.wp = NULL;

 for (pi = 1; argxp != NULL; argxp = argxp->ru.x, pi++)
  {
   tfap = &(tfatab[pi]);
   xp = argxp->lu.x;
   tfap->arg.axp = xp;
   tfap->old_pvc_flgs = (byte *) __my_malloc(insts); 
   memset(tfap->old_pvc_flgs, 0, insts);
   tfap->sav_pvc_flgs = (byte *) __my_malloc(insts); 
   memset(tfap->sav_pvc_flgs, 0, insts);
   tfap->dputp_tedlst = (struct dltevlst_t **)
    __my_malloc(insts*sizeof(struct dltevlst_t *));
   for (i = 0; i < insts; i++) tfap->dputp_tedlst[i] = NULL;
   tfap->sav_xinfos = (char **) __my_malloc(insts*sizeof(char *));
   for (i = 0; i < insts; i++) tfap->sav_xinfos[i] = NULL;

   if (xp->tf_isrw)
    {
     if (xp->optyp == ID || xp->optyp == GLBREF) 
      tfap->anp = xp->lu.sy->el.enp;
     else if (xp->optyp == LSB || xp->optyp == PARTSEL)
      tfap->anp = xp->lu.x->lu.sy->el.enp;
     else __case_terr(__FILE__, __LINE__);
     tfap->tfdrv_wp.bp = NULL;
     /* only have node infos if expr. rw */
    }
   /* SJM 04/20/00 - must allo set drive to nil if not rw */
   else { tfap->anp = NULL; tfap->tfdrv_wp.bp = NULL; }
  }
 return(tfatab);
}

/*
 * count no. of function arguments
 */
extern int32 __cnt_tfargs(register struct expr_t *argndp)
{
 int32 anum;

 for (anum = 0; argndp != NULL; argndp = argndp->ru.x) anum++;
 return(anum);
}

/*
 * check count drivers system function arguments (lhs)
 */
static void chksf_count_drivers(struct expr_t *ndp, int32 anum)
{
 register struct expr_t *fandp;

 if (anum > 6 || anum < 1)
  {
   __sgferr(847,
    "$countdrivers system function call must have from 1 to 6 arguments - has %d",
    anum);
   return;
  }

 /* first argument must be good declarative lhs and 1 bit form */ 
 fandp = ndp->ru.x;
 __chk_lhsexpr(fandp->lu.x, LHS_DECL);
 if (!is_1bwire(fandp->lu.x))
  {
   __sgferr(903,
    "$count_drivers system function first argument %s not scalar or constant bit select",
     __msgexpr_tostr(__xs, fandp->lu.x));
  }

 /* rest of arguments must be procedural lhs since output params */
 /* ,, common and legal lhs - these do not effect gate eater must be reg */
 for (fandp = fandp->ru.x; fandp != NULL; fandp = fandp->ru.x)
  __chk_lhsexpr(fandp->lu.x, LHS_PROC);
}

/*
 * return T if expression is a scalar wire or a bit select of scalared wire
 * also if bit select index x 
 */
static int32 is_1bwire(struct expr_t *ndp)
{
 int32 rv;
 struct net_t *np;
 struct expr_t *idndp;
 struct xstk_t *xsp;

 if (ndp->optyp == LSB)
  {
   idndp = ndp->lu.x; 
   if (ndp->ru.x->optyp != NUMBER && ndp->ru.x->optyp != ISNUMBER)
    return(FALSE); 
   np = idndp->lu.sy->el.enp;
   if (np->nrngrep == NX_CT)
    {
     if ((__no_expand && np->nu.ct->n_spltstate != SPLT_SCAL)
      || np->nu.ct->n_spltstate == SPLT_VECT) return(FALSE);
    }
   else if (!np->vec_scalared) return(FALSE);

   xsp = __eval_xpr(ndp->ru.x);
   /* if out of range or constant x index still error here */
   if (!vval_is0_(xsp->bp, xsp->xslen)) rv = FALSE; else rv = TRUE; 
   __pop_xstk();
   return(rv);
  }
 else
  {
   np = ndp->lu.sy->el.enp;
   if (ndp->optyp != ID && ndp->optyp != GLBREF) return(FALSE);
   np = ndp->lu.sy->el.enp;
  }
 if (np->ntyp >= NONWIRE_ST) return(FALSE);
 return(TRUE);
}

/*
 * check getpattern system function arguments - checks very limited usage
 */
static void chksf_getpattern(struct sy_t *syp, struct expr_t *ndp,
 int32 anum)
{
 struct net_t *np;
 struct expr_t *fax;

 if (anum != 1)
  {
   sf_errifn(syp, 1);
   if (anum > 1) ndp->ru.x->ru.x = NULL;
   else return;
  }
 if (!__chking_conta)
  __sgferr(849,
   "$getpattern system function call not on right hand side of continuous assignment");
 fax = ndp->ru.x->lu.x;
 if (fax->optyp == LSB)
  {
   np = fax->lu.x->lu.sy->el.enp;
   if (np->n_isarr)
    {
     /* getpattern result is same width as arg. */
     /* must make sure constant folding done */
     __chk_rhsexpr(fax, 0);
     if (fax->ru.x->optyp != ID) goto gpat_err;
     ndp->szu.xclen = fax->lu.x->szu.xclen;
     return;
    }
  }
gpat_err:
 __sgferr(850,
  "argument to $getpattern system function not variable indexed array");
}

/*
 * system function wrong number of arguments error
 */
static void sf_errifn(struct sy_t *syp, int32 expn)
{
 __sgferr(851, "system function %s call does not have required %d arguments",
  syp->synam, expn);
}

/*
 * check the $q_full system function
 * if final argument not needed can be left out
 */
static void chksf_q_full( struct expr_t *ndp, int32 anum)
{
 struct expr_t *fax;

 /* but for now making it an error */
 if (anum > 2 || anum < 1)
  __sgferr(847,
   "$q_full system function must have either 1 or 2 arguments not %d", anum);

 fax = ndp->ru.x;
 /* context here must be WBITS and also must change width during exec */
 __chk_rhsexpr(fax->lu.x, WBITS);
 if (anum == 2) { fax = fax->ru.x; __chk_lhsexpr(fax->lu.x, LHS_PROC); }
}

/*
 * check a random number generator type system function
 *
 * know first argument if present is always seed lvalue
 */
static void chksf_rand(struct expr_t *ndp, int32 maxanum, int32 anum)
{
 struct sy_t *syp;
 struct expr_t *fax;

 syp = ndp->lu.x->lu.sy;
 if (anum > maxanum)
  {
   __sgferr(847,
    "%s system function must have less than %d arguments - has %d",
    syp->synam, maxanum + 1, anum);
   return;
  }
 if (anum == 0) return;
 fax = ndp->ru.x;
 if (fax->lu.x->optyp != OPEMPTY)
  {
   /* seed argument can be any procedural lhs including ,, - OPEMPTY */ 
   __chk_lhsexpr(fax->lu.x, LHS_PROC);
   if (fax->lu.x->szu.xclen != WBITS) 
    __sgferr(877, "%s system function first argument %s must be 32 bits",
    syp->synam, __msgexpr_tostr(__xs, fax->lu.x));
  }
 /* check from 2nd on - must be rvalue not lvalue */
 for (fax = fax->ru.x; fax != NULL; fax = fax->ru.x)
  __chk_rhsexpr(fax->lu.x, 0);
}

/*
 * PASS 2 LHS EXPRESSION CHECKING ROUTINES
 */

/*
 * check a lhs expression
 * needs __sfnam_ind to be set before called
 * convention is to mark all ID and GLBIDs that are lhs expr and top
 * expr. as lhs expr.
 */
extern int32 __chk_lhsexpr(struct expr_t *ndp, int32 lhstyp)
{
 if (ndp->optyp == LCB)
  {
   unwind_lhsconcats(ndp);
  }
 if (!chksyn_lhsexpr(ndp, TRUE, lhstyp)) return(FALSE);
 /* even if error try to set */
 __set_lhswidth(ndp);
 ndp->x_islhs = TRUE; 
 return(TRUE);  
}

/*
 * check syntax of lhs lvalue expression
 * also marks lhs expression nodes and wires
 * needs __sfnam_ind to be set before called
 * return FALSE on error
 */
static int32 chksyn_lhsexpr(struct expr_t *ndp, int32 is_top, int32 lhstyp)
{
 struct expr_t *erndp;
 struct net_t *np;
 struct sy_t *syp;

 /* only simplify/check bit and part select expressions (not lvals) */
 switch ((byte) ndp->optyp) {
  case ID: case GLBREF:
   syp = ndp->lu.sy;
   if (syp->sytyp != SYM_N)
    { 
     __sgferr(1046, "symbol %s of type %s illegal left hand side value",
      syp->synam, __to_sytyp(__xs, syp->sytyp));
     return(FALSE);
    }

   ndp->szu.xclen = __get_netwide(syp->el.enp);
   ndp->x_islhs = TRUE; 
   erndp = ndp;

   /* unindexed array illegal on lhs */
   np = syp->el.enp;
   if (np->n_isarr) 
    {
     __sgferr(1046, "entire (unindexed) array %s illegal left hand side value",
      np->nsym->synam);
     return(FALSE);
    }

chk_id:
   np = syp->el.enp;
   if (np->n_isaparam)
    {
     __sgferr(854, "assignment to parameter illegal");
     return(FALSE);
    }
   if (np->nrngrep == NX_CT)
    {
     if (np->nu.ct->n_onlhs) np->nu.ct->n_2ndonlhs = TRUE; 
     else np->nu.ct->n_onlhs = TRUE;
    }

   if (np->ntyp == N_REAL) { ndp->is_real = TRUE; ndp->has_sign = TRUE; }
   if (lhstyp == LHS_DECL)
    {
     if (np->n_stren) ndp->x_stren = TRUE;
     return(__nd_wire(erndp));
    }
   else return(nd_reg(erndp));
  case LSB:
   erndp = ndp->lu.x;
   syp = erndp->lu.sy;
   /* this does not need to be a constant */
   /* lhs decl. bit selects must be constants see lrm 5-1, 12-16 (rule 2) */
   if (lhstyp == LHS_DECL)
    {
     /* this must convert to bit index constant (maybe IS form) */
     if (!chk_srcbsel(ndp, FALSE)) return(FALSE);
     if (!__chk_rhsexpr(ndp->ru.x, 0)) return(FALSE);
     chk_inrng_bsel(ndp);
     /* even if error, check decl. scalared */
     if (!__chk_lhsdecl_scalared(erndp)) return(FALSE);
    }
   /* procedural lhs can be non constant */
   else
    {
     if (!chk_srcbsel(ndp, FALSE)) return(FALSE);
     if (!__chk_rhsexpr(ndp->ru.x, 0)) return(FALSE);
     /* here either constant or expr. ok */
     if (ndp->ru.x->optyp == NUMBER || ndp->ru.x->optyp == ISNUMBER)
      {
       /* for procedural number lhs warn if out of range but gets assigned */
       if (!chk_inrng_bsel(ndp)) return(FALSE);
      }
    }
   goto chk_id;
  case PARTSEL:
   syp = ndp->lu.x->lu.sy;
   erndp = ndp->lu.x;
   if (!chk_srcpsel(ndp, FALSE)) return(FALSE);
   if (!__chk_rhsexpr(ndp->ru.x->lu.x, 0)) return(FALSE);
   if (!__chk_rhsexpr(ndp->ru.x->ru.x, 0)) return(FALSE);
   if (!chk_inrng_psel(ndp)) return(FALSE);
   if (lhstyp == LHS_DECL)
    {
     if (!__chk_lhsdecl_scalared(erndp)) return(FALSE);
    }
   goto chk_id;
  case LCB:
   /* DBG remove */
   if (!is_top) __misc_terr(__FILE__, __LINE__);
   /* --- */
   /* notice legal lhs concatenates element width self determined */
   {
    register struct expr_t *ndp2;
    int32 errind, has_stren, i;

    errind = TRUE;
    has_stren = FALSE;
    for (ndp2 = ndp->ru.x, i = 1; ndp2 != NULL; ndp2 = ndp2->ru.x, i++)
     {
      /* widths get set after checking */
      if (!chksyn_lhsexpr(ndp2->lu.x, FALSE, lhstyp)) errind = FALSE;
      /* notice, setting lhs st for all components and also top */
      /* if has stren know will be wire */
      if (ndp2->lu.x->x_stren) { ndp2->x_stren = TRUE; has_stren = TRUE; }
      if (ndp2->lu.x->is_real)
       {
        __sgferr(900,
         "real variable illegal in lvalue concatenate (pos. %d)", i);
       }
     }
    if (has_stren) ndp->x_stren = TRUE;
    return(errind);
   }
  case CATREP:
   __sgferr(856, "lvalue concatenate repeat form illegal");
   break;
  /* if empty form already checked to have appeared in legal place */
  case OPEMPTY: return(TRUE);
  case NUMBER:
    /* number is wrong so needs to fall through */
  default:
   __sgferr(857, "expression %s is illegal lvalue element",
    __msgexpr_tostr(__xs, ndp));
 }
 return(FALSE);
}

/*
 * check and unwind lhs concatenates
 *
 * repeat form concats and numbers illegal on lhs
 * when done concatenate is one level and checked
 *
 * notice this will unwind concatenates in fcall arguments
 */
static void unwind_lhsconcats(struct expr_t *ndp)
{
 if (__isleaf(ndp)) return;

 /* DBG ---
 if (__debug_flg)
  __dbg_msg("## unwinding lhs concat %s\n", __msgexpr_tostr(__xs, ndp));
 --- */

 /* concatenate unwinding must be done bottom up */
 if (ndp->lu.x != NULL)
  {
   /* DBG ---
   if (__debug_flg)
   __dbg_msg("## left side %s\n", __msgexpr_tostr(__xs, ndp->lu.x));
   --- */ 
   unwind_lhsconcats(ndp->lu.x);
   /* DBG ---
   if (__debug_flg) __dbg_msg("^^ left up.\n");
   --- */
  }
 if (ndp->ru.x != NULL)
  {
   /* DBG ---
   if (__debug_flg)
   __dbg_msg("## right side %s\n", __msgexpr_tostr(__xs, ndp->ru.x));
   --- */
   unwind_lhsconcats(ndp->ru.x);
   /* DBG ---
   if (__debug_flg) __dbg_msg("^^ right up.\n");
   --- */ 
  }

 /* node is top of concatenate with all sub concatenates simplified */
 if (ndp->optyp == LCB)
  {
   register struct expr_t *ndp2;
   struct expr_t *last_ndp2, *end_ndp;

   last_ndp2 = ndp;
   for (ndp2 = ndp->ru.x; ndp2 != NULL; ndp2 = ndp2->ru.x)
    {
     struct expr_t *lop;

     lop = ndp2->lu.x;
     /* notice ,, form illegal in concatentate */
     switch ((byte) lop->optyp) {
      /* constants illegal but caught later */
      case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM:
       break;
      case LCB:
       {
        /* nested concatenate - splice up one level */
        last_ndp2->ru.x = lop->ru.x;
        /* find rightmost element - know always there */
        end_ndp = __find_catend(lop);

        /* DBG LINT remove -- */
        if (end_ndp == NULL) __misc_terr(__FILE__, __LINE__);
        /* -- */

        /* if rightmost up one node will make null */
        end_ndp->ru.x = ndp2->ru.x;
        /* end of new chain is new last */
        last_ndp2 = end_ndp;
       }
       continue;
      case CATREP:
       /* error caught later */
       continue;
      }
     /* NUMBER or other means move last down tree one */
     last_ndp2 = ndp2;
    }
  }
}

/*
 * check a declarative lhs bit or part select to make sure scalared
 *
 * notice wire name on conta lhs does not require scalared vector
 * unless bit or part select
 *
 * if this is a reg (error caught later) surpress this error
 */
extern int32 __chk_lhsdecl_scalared(struct expr_t *idndp)
{
 struct net_t *np;

 np = idndp->lu.sy->el.enp;
 if (np->ntyp >= NONWIRE_ST) return(TRUE);

 /* if no autoexpand, must explicitly declare as scalared or error */
 /* if autoexpand (the default),  error if explicitly declared vectored */ 
 /* check only possible for non interact - should never happen in interact */
 if (np->nrngrep == NX_CT)
  {
   if ((__no_expand && np->nu.ct->n_spltstate != SPLT_SCAL)
    || np->nu.ct->n_spltstate == SPLT_VECT)
    {
     __sgferr(860, "vectored %s %s illegal in declarative lvalue",
      __to_wtnam(__xs, np), __to_idnam(idndp));
     return(FALSE);
   }
  }
 return(TRUE);
}

/*
 * return T if is a variable - i.e. any SYM_N that is not a parameter
 * know object is an id
 */
static int32 idnd_var(struct sy_t *syp)
{
 struct net_t *np;

 if (syp->sytyp != SYM_N) return(FALSE);
 np = syp->el.enp;
 if (!np->n_isaparam) return(TRUE);
 return(FALSE);
}

/*
 * for declarative code, lhs must be a wire
 * this catches declarative lhs reals
 * needs __sfnam_ind to be set before called
 */
extern int32 __nd_wire(struct expr_t *ndp)
{
 struct net_t *np;

 np = ndp->lu.sy->el.enp;
 if (np->ntyp >= NONWIRE_ST)
  {
   __sgferr(861, "declarative lvalue or port sink %s %s must be a wire",
    __to_wtnam(__xs, np), __to_idnam(ndp));
   return(FALSE);
  }
 return(TRUE);
}

/*
 * return T if expression contains any register
 *
 * needed for force/release since if has reg, must be quasi-continuous assign
 * reg form else normal lhs
 * notice this must handle un converted globals since chk expressions
 * routine that replaces the global not yet called at this point
 */
extern int32 __xhas_reg(struct expr_t *ndp)
{
 struct net_t *np;

 switch ((byte) ndp->optyp) {
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM: case OPEMPTY:
   break;
  case ID: case GLBREF:
   np = ndp->lu.sy->el.enp;
   if (np->ntyp >= NONWIRE_ST) return(TRUE);
   break;
  default:
   if (ndp->lu.x != NULL) if (__xhas_reg(ndp->lu.x)) return(TRUE);
   if (ndp->ru.x != NULL) if (__xhas_reg(ndp->ru.x)) return(TRUE);
   break;
 }
 return(FALSE);
}

/*
 * for procedural code, lhs must be a reg
 * know node is an id
 * needs __sfnam_ind to be set before called
 */
static int32 nd_reg(struct expr_t *ndp)
{
 struct net_t *np;

 np = ndp->lu.sy->el.enp;
 if (np->ntyp < NONWIRE_ST || np->ntyp == N_EVENT)
  {
   __sgferr(862, "procedural lvalue %s %s is not a reg",
    __to_wtnam(__xs, np), __to_idnam(ndp));
   return(FALSE);
  }
 if (np->n_isaparam)
  {
   __sgferr(863,
    "defparam %s use as register variable not supported", __to_idnam(ndp));
   return(FALSE);
  }
 return(TRUE);
}

/*
 * compute the lhs width
 * know called in only 1 place after global substituted
 */
extern void __set_lhswidth(struct expr_t *lhsx)
{
 int32 r1, r2, xwid;
 struct net_t *np;
 struct sy_t *syp;

 switch ((byte) lhsx->optyp) {
  case ID: case GLBREF:
   lhsx->szu.xclen = __get_netwide(lhsx->lu.sy->el.enp);
   break;
  /* width of empty is 0 that matches anything */
  case OPEMPTY: break;
  case LSB:
   syp = lhsx->lu.x->lu.sy;
   lhsx->lu.x->szu.xclen = __get_netwide(syp->el.enp);
   /* this is bit select or array index */
   np = syp->el.enp;
   if (np->n_isarr && np->n_isavec) xwid = __get_netwide(np); else xwid = 1;
   lhsx->szu.xclen = xwid;
   break;
  case PARTSEL:
   syp = lhsx->lu.x->lu.sy;
   lhsx->lu.x->szu.xclen = __get_netwide(syp->el.enp);
   np = syp->el.enp;
   /* know these values always fit in 32 bits and never IS form */
   r1 = (int32) __contab[lhsx->ru.x->lu.x->ru.xvi];
   r2 = (int32) __contab[lhsx->ru.x->ru.x->ru.xvi];
   lhsx->szu.xclen = (r1 > r2) ? r1 - r2 + 1 : r2 - r1 + 1;
   break;
  case LCB:
   {
    register struct expr_t *ndp2;

    /* know lhs concatenates never nested */
    for (xwid = 0, ndp2 = lhsx->ru.x; ndp2 != NULL; ndp2 = ndp2->ru.x)
     {
      __set_lhswidth(ndp2->lu.x);
      xwid += ndp2->lu.x->szu.xclen;
     }
    lhsx->szu.xclen = xwid;
    /* CAT COM op width is dist from high bit of this to low (right) end */
    for (ndp2 = lhsx->ru.x; ndp2 != NULL; ndp2 = ndp2->ru.x)
     {
      ndp2->szu.xclen = xwid;
      xwid -= ndp2->lu.x->szu.xclen;
     }
    if (xwid != 0) __misc_terr(__FILE__, __LINE__);
    break;
   }
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * check an event expression
 * real numbers not allowed in event expressions
 */
extern void __chk_evxpr(struct expr_t *ndp)
{
 struct net_t *np;

 switch ((byte) ndp->optyp) {
  /* any simple ID good - event versus variable change known from var type */
  case ID: case GLBREF:
   np = ndp->lu.sy->el.enp;
   if (np->ntyp == N_EVENT)
    {
     ndp->szu.xclen = 1;
     if (np->nrngrep == NX_CT) np->nu.ct->n_onrhs = TRUE;
     break;
    }
   /* any wire used in an event control is accessed as on rhs */
   if (np->nrngrep == NX_CT) np->nu.ct->n_onrhs = TRUE;
   goto chk_expr;
  case OPEVOR: case OPEVCOMMAOR:
   __chk_evxpr(ndp->lu.x);
   __chk_evxpr(ndp->ru.x);
   break;
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM:
   /* SJM 05/22/00 - per XL, real ev exprs (non edge) legal - just chg op */
is_num:
    __sgfwarn(551, "event expression constant - event cannot occur"); 
   break;
  case OPPOSEDGE: case OPNEGEDGE:
   chk_edge_expr(ndp); 
   break;
  default: 
   /* change of any rhs expressions */
chk_expr:
   __chk_rhsexpr(ndp, 0);
   if (ndp->optyp == NUMBER) goto is_num;
   /* SJM 05/22/00 - per XL, real ev exprs (non edge) legal - just chg op */
   if (xpr_hasfcall(ndp))
    __sgfinform(414, "event expression %s has function call that can have side effects",
     __msgexpr_tostr(__xs, ndp));
 }
}

/*
 * return T if expression has a function call
 */
static int32 xpr_hasfcall(struct expr_t *ndp)
{
 if (ndp->optyp == FCALL) return(TRUE);
 if (__isleaf(ndp)) return(FALSE);
 if (ndp->lu.x != NULL) { if (xpr_hasfcall(ndp->lu.x)) return(TRUE); }
 if (ndp->ru.x != NULL) { if (xpr_hasfcall(ndp->ru.x)) return(TRUE); }
 return(FALSE);
}

/*
 * return T if expression has a user function call
 * built-in $ starting system functions legal
 *
 */
static int32 xpr_has_nonsys_fcall(struct expr_t *ndp)
{
 int32 anum;
 struct sy_t *syp; 
 struct sysfunc_t *syfp;

 if (ndp->optyp == FCALL)
  {
   syp = ndp->lu.x->lu.sy;
   if (syp->sytyp != SYM_SF) return(TRUE);

   syfp = syp->el.esyftbp;
   if (syfp->tftyp != SYSF_BUILTIN) return(TRUE);
   anum = __cnt_tfargs(ndp->ru.x);

   /* SJM 09/04/01 - must check arg num here since can't fold - also */
   /* exec assumes right no. of args without check */
   /* only some built-in system functions allowed in const exprs */
   switch (syfp->syfnum) {
    case STN_ITOR: case STN_BITSTOREAL: case STN_RTOI: case STN_REALTOBITS:
    case STN_SIGNED: case STN_UNSIGNED:
    case STN_COS: case STN_SIN: case STN_TAN: case STN_ACOS: case STN_ASIN:
    case STN_ATAN: case STN_COSH: case STN_SINH: case STN_TANH:
    case STN_ATANH: case STN_ACOSH: case STN_ASINH:
    case STN_INT: case STN_SGN:
    case STN_LN: case STN_LOG10: case STN_ABS: case STN_SQRT: case STN_EXP:
    case STN_HSQRT: case STN_HLOG: case STN_HLOG10:
     if (anum != 1) { sf_errifn(syp, 1); return(TRUE); }
     break;

    case STN_POW: case STN_MIN: case STN_MAX:
    /* hspice special too */
    case STN_HPOW: case STN_HPWR: case STN_HSIGN: case STN_HDB:
    case STN_HYPOT: case STN_ATAN2:
     if (anum != 2) { sf_errifn(syp, 2); return(TRUE); }
     break;
    default:
     /* illegal built-in system function in constant expression */ 
     return(TRUE);
   }
   /* fall thru - because must check args */
  }
 if (__isleaf(ndp)) return(FALSE);

 if (ndp->lu.x != NULL) { if (xpr_has_nonsys_fcall(ndp->lu.x)) return(TRUE); }
 if (ndp->ru.x != NULL) { if (xpr_has_nonsys_fcall(ndp->ru.x)) return(TRUE); }
 return(FALSE);
}

/*
 * any expression legal here but warning if wider than 1 bit (use low bit)
 * LRM requires using low bit for any wider than 1 bit
 *
 * allowing any expression including array index - have separate expr
 * old value and eval to filter for anything but scalared variable and
 * constant bit select of scalared
 */
static void chk_edge_expr(struct expr_t *endp)
{
 struct expr_t *ndp;

 ndp = endp->lu.x;
 /* this will catch any event variable */
 __chk_rhsexpr(ndp, 0);
 /* SJM 05/22/00 - real edge expressions still illegal */
 if (ndp->is_real)
  {
   __sgferr(864, "edge event control expression %s cannot be real",
    __msgexpr_tostr(__xs, endp));
   return;
  }
 if (ndp->szu.xclen != 1)
  {
   __sgfwarn(677,
    "edge control %s expression width %d wider than 1 bit - low bit used",
    __msgexpr_tostr(__xs, endp), ndp->szu.xclen);
  }
}

/*
 * PASS 2 (AFTER ALL SOURCE READ) FUNCTION AND TASK CHECKING ROUTINES
 */

/*
 * check a task enable statement
 */
extern void __chk_tskenable(struct st_t *stp)
{
 register struct expr_t *xp;
 register struct task_pin_t *tpp;
 int32 pwid, pi;
 struct task_t *tskp;
 struct sy_t *syp;
 struct expr_t *tkxp;
 struct tskcall_t *tkcp;

 tkcp = &(stp->st.stkc);
 /* cannot check number of widths (assuming 0 context of system tasks) */
 tkxp = tkcp->tsksyx;
 if (tkxp->optyp == ID && *(tkxp->lu.sy->synam) == '$')
  {
   /* system task args do not have type or width - take what is there */
   chk_systskenable(stp, tkcp);
   return;
  }

 /* task call symbol missing or not declared as task */
 if (tkxp == NULL || (tkxp->optyp != GLBREF && tkxp->optyp != ID)
  || (syp = tkxp->lu.sy) == NULL) __misc_sgfterr(__FILE__, __LINE__);

 /* if error will be left as global ID */
 syp = tkcp->tsksyx->lu.sy;
 tskp = syp->el.etskp;
 tpp = tskp->tskpins;
 pi = 1;
 for (xp = tkcp->targs; xp != NULL; xp = xp->ru.x, tpp = tpp->tpnxt, pi++)
  {
   if (tpp == NULL)
    {
     __sgferr(871, "task %s enable has more arguments (%d) than declaration",
      __to_idnam(tkcp->tsksyx), pi);
     return;
    }
   /* decl. argumen cannot be global - declared symbol */
   pwid = __get_netwide(tpp->tpsy->el.enp);
   /* but expression is normal either right or left sides */
   if (tpp->trtyp == IO_IN)
    {
     __chk_rhsexpr(xp->lu.x, pwid);

     /* SJM 10/08/04 - for Ver 2001 because WBITS can be 64 - must widen */
     /* unsized constant (unsiznum bit in expr rec on) to lhs width */
     if (xp->lu.x->optyp == NUMBER && xp->lu.x->unsiznum
      && xp->lu.x->szu.xclen < pwid)
      {
       xp->lu.x = __widen_unsiz_rhs_assign(xp->lu.x, pwid);
      }
    }
   else
    {
     __chk_lhsexpr(xp->lu.x, LHS_PROC);
     if (tpp->trtyp == IO_BID) __set_expr_onrhs(xp->lu.x);
    }
   /* no inform for bit width changes (they are common) */
   /* but inform for arg. real and value non real or opposite */
   if (tpp->tpsy->el.enp->ntyp == N_REAL && !(xp->lu.x->is_real))
    {
     __sgfinform(485,
      "task %s argument pos. %d declared real - non real expression %s converted",
      __to_idnam(tkcp->tsksyx), pi,  __msgexpr_tostr(__xs, xp->lu.x));
    }
   else if (tpp->tpsy->el.enp->ntyp != N_REAL && xp->lu.x->is_real)
    {
     __sgfinform(485,
      "task %s argument pos. %d declared non real - real expression %s converted",
      __to_idnam(tkcp->tsksyx), pi,  __msgexpr_tostr(__xs, xp->lu.x));
    }
  }
 if (tpp != NULL)
  __sgferr(872, "task %s enable has fewer arguments than declaration",
   __to_idnam(tkcp->tsksyx));
 tskp->t_used = TRUE;
}

/*
 * check a system task enable
 *
 * notice there is no task structure and symbol in special symbol table
 * any number with any type argument list just passed on
 */
static void chk_systskenable(struct st_t *stp, struct tskcall_t *tkcp)
{
 register struct expr_t *xp;
 int32 anum, special_syntax, is_disp_typ, is_monit_typ, nbytes, ii;
 struct sy_t *syp;
 struct expr_t *tkxp, *sav_xp;
 struct systsk_t *stbp;
 struct expr_t *ndp;
 struct sy_t *tsyp;
 byte *monit_argtyps;

 tkxp = tkcp->tsksyx;
 syp = tkxp->lu.sy;
 stbp = syp->el.esytbp;

 /* system task number inconsistent */
 if (stbp->stsknum == 0) __misc_sgfterr(__FILE__, __LINE__);

 special_syntax = FALSE;
 /* notice for system tasks no special first arg. return type */
 /* assume start checking expressions with 1st arg. */
 xp = tkcp->targs;
 anum = __cnt_tfargs(tkcp->targs);
 is_disp_typ = FALSE;
 is_monit_typ = FALSE;
 if (anum == 1 && xp->lu.x->optyp == OPEMPTY)
  {
   __sgfwarn(633,
    "system task enable: %s(); has one empty argument - for no arguments omit the ()",
    syp->synam);
  }

 /* dumpvars is special case because xmr's may not resolve to wire */
 /* if pli system task supercedes normal, will never see its task number */ 
 switch (stbp->stsknum) {
  case STN_FFLUSH:
   /* $fflush([optional op]) - op can be missing, then all, [mcd] or [fd] */
   if (anum != 0 && anum !=1)
    {
     __sgferr(851,
      "%s system task illegal number of arguments (%d) - 0 or 1 legal",
      syp->synam, anum);
    } 
   break;
  case STN_SWRITE: case STN_SWRITEB: case STN_SWRITEH: case STN_SWRITEO:
   is_disp_typ = TRUE;
   if (anum < 2)
    {
     __sgferr(882,
      "%s system task required output reg and one argument missing",
      syp->synam);
     return;
    }
   ndp = xp->lu.x;
   /* SJM 05/17/04 - LOOKATME this can be any width string - why 32? */
   __chk_lhsexpr(ndp, 32);
   /* move to 2nd argument */
   xp = xp->ru.x;
   break;
  case STN_SFORMAT:
   is_disp_typ = TRUE;
   if (anum < 2)
    {
     __sgferr(882,
      "%s system task required output reg and format argument(s) missing",
      syp->synam);
     special_syntax = TRUE;
     break;
    }
   ndp = xp->lu.x;
   /* width self determined for lhs string written into */
   __chk_lhsexpr(ndp, 0);
   /* move to 2nd format argument */
   xp = xp->ru.x;
   /* again width self determined */
   __chk_rhsexpr(xp->lu.x, 0);

   /* if format arg is literal string - can check format */
   if (xp->lu.x->is_string) is_disp_typ = TRUE;
   else special_syntax = TRUE;

   xp = xp->ru.x;
   /* if variable fmt, can only check the rhs fmt args */ 
   if (special_syntax)
    {
     for (sav_xp = xp; xp != NULL; xp = xp->ru.x)
      {
       __chk_rhsexpr(xp->lu.x, 0);
      }
    }
   break;
  /* task requires special argument processing */
  case STN_DUMPVARS:
   chkst_dumpvars_enable(tkcp, anum);

   /* SJM 01/27/03 - error if compiled sim and try to call dumpvars */
   /* from interactive mode because need source preprocessing */ 
   if (__iact_state && __optimized_sim)
    {
     __sgferr(876, 
      "%s system task call from interactive debugger illegal - move to module ssource or run without -O",
      syp->synam);
     /* error will inhibit interactive execution but need rest of checking */
    }
   special_syntax = TRUE;
   break;
  case STN_READMEMB: case STN_READMEMH:
   chkst_readmem(syp, anum, tkcp);
   special_syntax = TRUE;
   break;
  case STN_SREADMEMB: case STN_SREADMEMH:
   chkst_sreadmem(syp, anum, tkcp);
   special_syntax = TRUE;
   break;
  case STN_TIMEFORMAT:
   /* set how %t displayed and sets interactive time unit interpretation */
   /* (units, precision, suffix, minimum width) */
   if (anum < 1 || anum > 4) st_errif_rng(syp, 1, 4, anum);
   if (!__des_has_timescales)
    {
     __sgfwarn(626,
      "%s system task ignored - design does not use timescales", syp->synam);
    }
   /* args are just string or numeric expressions - from interp. in Ver */
   break;
  case STN_PRINTTIMESCALE:
   /* this takes hierarchical xmr as name */
   if (anum > 1) st_errif_rng(syp, 0, 1, anum);
   /* 0 args - means use $scope() set module */
   if (anum == 1)
    {
     /* notice must not check as rhs expr since requires variables */
     ndp = xp->lu.x;
     /* this must always be xmr - already resolved */ 
     if (ndp->optyp != GLBREF || (ndp->lu.sy->sytyp != SYM_I
      && ndp->lu.sy->sytyp != SYM_M))
      {
       __sgferr(876, 
        "%s system task argument %s is not required module instance reference",
        syp->synam, __msgexpr_tostr(__xs, ndp));
      }
     special_syntax = TRUE;
    }
   break;
  case STN_SCOPE:
   /* list can have 0 arguments but scope must have exactly one */  
   if (anum != 1) { st_errifn(syp, 1, anum); break; } 
   goto args_ok;
  case STN_LIST:
   /* this takes hierarchical xmr as name */
   if (anum > 1) st_errif_rng(syp, 0, 1, anum);
   /* no argument means use current scope for list */
   if (anum == 0) break;
args_ok:
   ndp = xp->lu.x;
   /* any 1 component scope reference will still be global here */
   if (ndp->optyp != GLBREF)
    {
     if (ndp->optyp != ID) goto bad_scope;
     tsyp = ndp->lu.sy;
     /* local task/func/lb scope is legal - will not be seen as global */
     if (tsyp->sytyp == SYM_F || tsyp->sytyp == SYM_LB
     || tsyp->sytyp == SYM_TSK) { special_syntax = TRUE; break; }
     goto bad_scope;
    }
   else tsyp = ndp->lu.sy;
   /* know this is global - maybe 1 component */
   /* need error if try to $scope into udp or built in primitive */
   if (tsyp->sytyp == SYM_UDP || tsyp->sytyp == SYM_PRIM)
    {
     __sgferr(880, "%s system task argument %s %s illegal", syp->synam,
      __to_sytyp(__xs, syp->sytyp), syp->synam);
     special_syntax = TRUE;
     break;
    }
   /* only scope required here */
   if (!__is_scope_sym(tsyp))
    {
bad_scope:
     __sgferr(881,
      "%s system task argument %s is not required scope identifier",
      syp->synam, __msgexpr_tostr(__xs, ndp));
     }
   special_syntax = TRUE;
   break;
  case STN_SHOWVARIABLES:
   /* for now just treating same as showvars - ignoring control argument */
   if (anum < 1)
    {
     __sgferr(883, "%s system task required first control argument missing",
      syp->synam);
     break;
    }
   /* move to 2nd argument */
   xp = xp->ru.x;
   /*FALLTHRU */
  case STN_SHOWVARS:
   /* any number of args ok - but can be var,glb,bsel or psel only */ 
   for (; xp != NULL; xp = xp->ru.x)
    {
     ndp = xp->lu.x;
     __chk_rhsexpr(ndp, 0);
     switch ((byte) ndp->optyp) {
      case ID: case GLBREF: case LSB: case PARTSEL: break;
      default:
       __sgferr(917,
        "%s system task argument %s: variable, global or select required",
        syp->synam, __msgexpr_tostr(__xs, ndp));
     }
    }
   special_syntax = TRUE;
   break;
  /* tasks that cannot have any arguments */
  case STN_MONITORON: case STN_MONITOROFF: case STN_DUMPON: case STN_DUMPOFF:
  case STN_DUMPALL: case STN_DUMPFLUSH:
  case STN_NOKEY: case STN_NOLOG: case STN_CLEARTRACE:
  case STN_SHOWALLINSTANCES: case STN_SETTRACE: case STN_SHOWEXPANDEDNETS:
  case STN_HISTORY: case STN_MEMUSE: case STN_FLUSHLOG:
  case STN_SETEVTRACE: case STN_CLEAREVTRACE:
  case STN_SETDEBUG: case STN_CLEARDEBUG:
  case STN_RESET:
   if (anum > 3) st_errif_rng(syp, 0, 3, anum);
   /* any arguments just evaluated for 32 bit values - sign bit may have */
   /* significance */ 
   break;

  /* tasks that can have 0 or 1 arguments */
  /* notice these can appear in source - since just change files */
  case STN_KEY: case STN_LOG: case STN_INPUT:
  case STN_FINISH: case STN_STOP: case STN_SHOWSCOPES:
  case STN_SNAPSHOT: case STN_TRACEFILE: case STN_SYSTEM:
  case STN_DUMPFILE:
   if (anum > 1) st_errif_rng(syp, 0, 1, anum);
   break;
  /* tasks that require exactly 1 expr. type arg. (file name?) */
  case STN_DUMPLIMIT: case STN_SAVE: case STN_RESTART: case STN_INCSAVE:
  case STN_FCLOSE:
   if (anum != 1) st_errifn(syp, 1, anum);
   break;
  /* tasks that take a multichannel descriptor followed by anything */
  case STN_FMONITOR: case STN_FMONITORB: case STN_FMONITORH:
  case STN_FMONITORO:
   is_monit_typ = TRUE;
   /*FALLTHRU*/ 
  case STN_FDISPLAY: case STN_FDISPLAYB: case STN_FDISPLAYH:
  case STN_FDISPLAYO:
  case STN_FWRITE: case STN_FWRITEB: case STN_FWRITEH: case STN_FWRITEO:
  case STN_FSTROBE: case STN_FSTROBEB: case STN_FSTROBEH: case STN_FSTROBEO:
   is_disp_typ = TRUE;
   if (anum < 1)
    {
     __sgferr(882,
      "%s system task required file descriptor or MCD first argument missing",
      syp->synam);
     return;
    }
   ndp = xp->lu.x;
   __chk_rhsexpr(ndp, 32);
   if (ndp->szu.xclen != 32)
    {
     __sgfwarn(612,
      "file descriptor or MCD first argument has size %d should be 32",
      ndp->szu.xclen);
    }
   /* move to 2nd argument */
   xp = xp->ru.x;
   break;
  /* tasks that take any number of numeric arguments - checked later */
  case STN_SUPWARNS:
  case STN_ALLOWWARNS:
   /* could check constant arguments here */
   break;
  /* task that take any number of unchecked arguments */
  case STN_MONITOR: case STN_MONITORB: case STN_MONITORH: case STN_MONITORO:
   is_monit_typ = TRUE;
   /*FALLTHRU*/ 
  case STN_DISPLAY: case STN_DISPLAYB: case STN_DISPLAYH: case STN_DISPLAYO:
  case STN_WRITE: case STN_WRITEB: case STN_WRITEH: case STN_WRITEO:
  case STN_STROBE: case STN_STROBEB: case STN_STROBEH: case STN_STROBEO:
   is_disp_typ = TRUE;
   break;
  /* any number of reg/wire variables */
  /* all have four required arguments, last inout */
  case STN_Q_ADD:
  case STN_Q_INITIALIZE:
   /* LOOKATME - LRM says 4th arg. is required - but now old Gateway doc. */
   /* but maybe operator empty is arg (i.e. , required? */
   if (anum != 4) st_errifn(syp, 4, anum);
   /* first arg. is input int32 identifying the Q */
   if (anum >= 1) __chk_rhsexpr(xp->lu.x, 0);
   /* 2nd arg. is q_type for initialize and job_id (user num.) for add */
   if (anum >= 2) { xp = xp->ru.x; __chk_rhsexpr(xp->lu.x, 0); }
   /* 3rd arg is max q len for initialize and 2nd user num for add */
   if (anum >= 3) { xp = xp->ru.x; __chk_rhsexpr(xp->lu.x, 0); }
   /* 4th arg is output operation status value */
   if (anum >= 4) { xp = xp->ru.x; __chk_lhsexpr(xp->lu.x, LHS_PROC); }
   special_syntax = TRUE;
   break;
  case STN_Q_REMOVE:
   if (anum != 4) st_errifn(syp, 4, anum);
   /* first arg. is input int32 identifying the Q */
   if (anum >= 1) __chk_rhsexpr(xp->lu.x, 0);
   /* 2nd arg. is output job_id (user num. put in by add call) */
   if (anum >= 2) { xp = xp->ru.x; __chk_lhsexpr(xp->lu.x, LHS_PROC); }
   /* 3rd arg. is output inform_id (2nd user num. put in by add call) */
   if (anum >= 3) { xp = xp->ru.x; __chk_lhsexpr(xp->lu.x, LHS_PROC); }
   /* 4th arg is output operation status (completion value) value */
   if (anum >= 4) { xp = xp->ru.x; __chk_lhsexpr(xp->lu.x, LHS_PROC); }
   special_syntax = TRUE;
   break;
  case STN_Q_EXAM:
   if (anum != 4) st_errifn(syp, 4, anum);
   /* first arg. is input int32 identifying the Q */
   if (anum >= 1) __chk_rhsexpr(xp->lu.x, 0);
   /* 2nd arg. is input q_stat_code that selects information to return */
   if (anum >= 2) { xp = xp->ru.x; __chk_rhsexpr(xp->lu.x, 0); }
   /* 3rd arg. is output status value */
   if (anum >= 3) { xp = xp->ru.x; __chk_lhsexpr(xp->lu.x, LHS_PROC); }
   /* 4th arg is output operation status (completion code) value */
   if (anum >= 4) { xp = xp->ru.x; __chk_lhsexpr(xp->lu.x, LHS_PROC); }
   special_syntax = TRUE;
   break;
  case STN_SDF_ANNOTATE:
   chkst_sdfannotate_enable(tkcp, anum);
   special_syntax = TRUE;
   break;

  case STN_GRREMOTE:
  case STN_PSWAVES:
  case STN_GRSYNCHON:
  case STN_GRREGS:
  case STN_GRWAVES:
  case STN_FREEZEWAVES:
  case STN_DEFINEGROUPWAVES:
   __sgfwarn(553,
    "group waves task %s not supported- executed as null statement",
    syp->synam);
   return; 

  default:
   /* check pli tf user task enable and build tf rec */
   if (stbp->stsknum >= BASE_VERIUSERTFS
    && (int32) stbp->stsknum <= __last_systf)
    {
     /* tf_ checktf called during prep after aux record built */
     /* for both tf_ and vpi_ systf */
     chkbld_pli_task(stp, (int32) stbp->stsknum);
     return;
    }
 }
 if (special_syntax) return;

 /* fall through to normal anything legal - notice 0 (none) expr. context */
 for (sav_xp = xp; xp != NULL; xp = xp->ru.x)
  {
   ndp = xp->lu.x;
   __chk_rhsexpr(ndp, 0);
   /* if special syntax called if needed in that routine */
   /* width in bytes where bit sizes rounded up to next word32 */
   /* SJM 07/24/00 - no nu.ct when elaborated from iact state */
   if (is_monit_typ && !__iact_state)
    {
     mark_monit_in_src_nets(ndp);
    }
  }
 if (is_monit_typ)
  {
   /* must allocate 1 byte even if no args */
   nbytes = (anum != 0) ? anum : 1; 
   /* monit type implies display type */
   monit_argtyps = (byte *) __my_malloc(nbytes);
   memset(monit_argtyps, 0, nbytes);
   __chk_fmt(sav_xp, monit_argtyps);
   /* will never get here for PLI task func that shares trec field */ 
   if (tkcp->tkcaux.mauxp == NULL)
    {
     tkcp->tkcaux.mauxp = (struct monaux_t *)
      __my_malloc(sizeof(struct monaux_t));
     tkcp->tkcaux.mauxp->dces_blt = FALSE;
     tkcp->tkcaux.mauxp->argisvtab = NULL;
     tkcp->tkcaux.mauxp->mon_dcehdr = (struct dceauxlst_t **)
      __my_malloc(__inst_mod->flatinum*sizeof(struct dceauxlst_t *));
     for (ii = 0; ii < __inst_mod->flatinum; ii++) 
      tkcp->tkcaux.mauxp->mon_dcehdr[ii] = NULL;
    }
   tkcp->tkcaux.mauxp->argisvtab = monit_argtyps;
   return;
  }
 if (is_disp_typ) __chk_fmt(sav_xp, (byte *) NULL);
}

/*
 * for optimizer, mark monit arg nets as having monit in src 
 *
 * BEWARE - must not call this during iact elaboration but called
 * even for interpreter now
 */
static void mark_monit_in_src_nets(struct expr_t *xp)
{
 struct net_t *np;

 if (__isleaf(xp))
  {
   /* SJM 03/22/02 - XMR monits in source must also be marked */
   if ((xp->optyp == ID || xp->optyp == GLBREF) && xp->lu.sy->sytyp == SYM_N)
    {
     np = xp->lu.sy->el.enp; 
     if (!np->n_isaparam) np->nu.ct->monit_in_src = TRUE;
    }
   return;
  }
 if (xp->lu.x != NULL) mark_monit_in_src_nets(xp->lu.x);
 if (xp->ru.x != NULL) mark_monit_in_src_nets(xp->ru.x);
}

/*
 * return T if symbol can be scope 
 */
extern int32 __is_scope_sym(struct sy_t *syp) 
{
 switch ((byte) syp->sytyp) { 
  case SYM_I: case SYM_M: case SYM_LB: case SYM_TSK: case SYM_F: return(TRUE);
 }
 return(FALSE);
}

/*
 * check and build vpi_ tf rec for pli task
 */
static void chkbld_pli_task(struct st_t *stp, int32 tfnum)
{
 int32 anum;
 struct tskcall_t *tkcp;
 struct tfrec_t *tfrp;

 tkcp = &(stp->st.stkc);
 /* this sets tf_rw flags for args */
 if (tkcp->targs != NULL)
  { chk_pli_arglist(tkcp->targs, tfnum); anum = __cnt_tfargs(tkcp->targs); }
 else anum = 0;

 /* need separate routine for vpi_ systfs compiletf and checking */ 
 if (tfnum > __last_veriusertf)
  {
   __chkbld_vpi_systf_task(stp);
   return;
  }

 /* allocate the tf rec and link on to func. name expr unused len fld */
 tfrp = chkalloc_tfrec(tkcp->targs, anum);
 tfrp->tf_func = FALSE;
 /* link together both directions */
 tfrp->tfu.tfstp = stp;
 tkcp->tkcaux.trec = tfrp;

 if (__tfrec_hdr == NULL) __tfrec_hdr = __tfrec_end = tfrp;
 else { __tfrec_end->tfrnxt = tfrp; __tfrec_end = tfrp; }
}

/*
 * system task number of arguments outside of legal range error
 */
static void st_errif_rng(struct sy_t *syp, int32 expl, int32 exph, int32 anum)
{
 __sgferr(884, "%s system task must have from %d to %d arguments - has %d",
  syp->synam, expl, exph, anum);
}

/*
 * system task number of arguments not required number
 */
static void st_errifn(struct sy_t *syp, int32 expn, int32 anum)
{
 if (expn == 0)
  __sgferr(885, "%s system task cannot have any arguments - has %d",
   syp->synam, anum);
 else __sgferr(886, "%s system task %d argument(s) required - has %d",
  syp->synam, expn, anum);
}

/*
 * check the $dumpvars system task enable - needs special processing
 * 1st arg. if present must be num. rhs expr. and rest must be vars.
 * does the following:
 *  1. make sure 1st expr. evaluates to rhs value
 *  2. must be simple scope or wire 
 * case of wire and top mod. conflicts handled in global substitution  
 *
 * SJM 07/15/00 - now 3. mark all all mod and one net dumpvars in src
 */
static void chkst_dumpvars_enable(struct tskcall_t *tkcp, int32 anum)
{
 register struct expr_t *alxp;
 int32 levels;
 word32 *wp;
 double *dp;
 struct expr_t *dpthndp, *xp;
 struct gref_t *grp;
 struct net_t *np;
 struct sy_t *syp;
 struct mod_t *mdp;

 alxp = tkcp->targs;
 /* special case of dump all variables in design */
 if (anum == 0)
  {
   /* SJM 07/15/00 - dumpvars with no args is dumpv all in src */
   __dv_allform_insrc = TRUE;
   /* must adjust so all net stores use chg form to trigger dumpvaring */
   if (__iact_state) set_iact_dmpv_all_nd_nchgstore();
   return;
  }

 dpthndp = alxp->lu.x;
 __chk_rhsexpr(dpthndp, 0);
 if (dpthndp->optyp == OPEMPTY)
  __sgferr(918, "$dumpvars first argument () or (, empty form illegal");

 /* SJM 08/08/03 - was not handling first level argument right */
 /* also must be constant expression or can't mark underneath levels */
 /* argument list form, know first is level */
 /* must be number by here since will be folded if possible */ 

 /* NOTICE - works for -O vm gen even for non numeric expr since */
 /* just causes dmpv change forms to be emitted for all under */
 if (dpthndp->optyp == NUMBER)
  {
   wp = &(__contab[dpthndp->ru.xvi]);
   if (!vval_is0_(wp, dpthndp->szu.xclen)) goto non_const_expr;
   levels = (int32) wp[0];
  }
 else if (dpthndp->optyp == REALNUM)
  {
   dp = (double *) __contab[dpthndp->ru.xvi];
   levels = (int32) *dp;
   if (levels < 0.0) goto non_const_expr;
  }
 else
  {
non_const_expr:
   /* if non constant expr set levels to 0 and do all underneath */
   levels = 0;
  }

 for (alxp = alxp->ru.x; alxp != NULL; alxp = alxp->ru.x)
  {
   /* case 1 - identifier - must be wire */
   xp = alxp->lu.x;
   switch ((byte) xp->optyp) {
    case ID:
     np = xp->lu.sy->el.enp;
     if (np->n_isarr)
      {
is_arr:
       __sgfwarn(541,
        "$dumpvars argument variable %s cannot be array - ignored",
        __to_idnam(xp)); 
       break;
      }
     np->nu.ct->dmpv_in_src = TRUE;

     /* SJM 08/08/03 - for dumpvars sys task called from iact code */
     /* must set nd chg store form because only this net changed */
     if (__iact_state)
      {
       np->dmpv_in_src = TRUE;
       np->nchg_nd_chgstore = TRUE;
      }
     break;
    case GLBREF:
     grp = xp->ru.grp;
     syp = grp->targsyp; 
     if (syp->sytyp == SYM_N)
      {
       np = syp->el.enp;
       if (np->n_isarr) goto is_arr;
       np->nu.ct->dmpv_in_src = TRUE;

       /* SJM 08/08/03 - for dumpvars sys task called from iact code */
       /* must set nd chg store form because only this net changed */
       if (__iact_state)
        {
         np->dmpv_in_src = TRUE;
         np->nchg_nd_chgstore = TRUE;
        }
       break;
      }
     /* also cannot be udp or primitive - have no vars */
     /* here module is for top level modules (inst. name the same here) */
     if (syp->sytyp != SYM_I && syp->sytyp != SYM_M)
      {
       __sgferr(887,
        "$dumpvars scope form argument %s %s illegal - instance required",
 __to_sytyp(__xs, syp->sytyp), syp->synam);
       goto make_op_err;
      }
     /* distance under 1 is nothing under and 0 and is all under */
     /* SJM 08/08/03 - notice must mark wires for all insts of mod */ 
     /* even though only some under depending on where $dumpvars is */
     /* called from during sim */
     if (syp->sytyp == SYM_I) mdp = syp->el.eip->imsym->el.emdp;
     else mdp = syp->el.emdp;

     if (levels == 1)
      {
       if (!mdp->mod_dvars_in_src)
        {
         mdp->mod_dvars_in_src = TRUE;
         /* can't be called from iact if vm compiler on */
         if (__iact_state) set_iact_dmpvmod_nd_nchgstore(mdp);
        }
      } 
     else mark_mod_dvars_under(mdp, levels);
     break;
    case OPEMPTY: 
     __sgferr(889, "$dumpvars argument cannot be empty (,,) form");
     goto make_op_err;
    default:
     __sgferr(888, "$dumpvars argument %s must be variable or scope path",
      __msgexpr_tostr(__xs, xp));
make_op_err:
     __set_xtab_errval();
     __bld_xtree(0);
     alxp->lu.x = __root_ndp;
   }
   /* notice by here, if parameter will be substituted to number */
  }
}

/*
 * mark all module instances up to level under as dumpvars
 * levels of 0 implies all
 */
static void mark_mod_dvars_under(struct mod_t *mdp, int32 levels)
{
 register int32 ii; 
 register struct inst_t *down_ip;
 if (!mdp->mod_dvars_in_src)
  {
   mdp->mod_dvars_in_src = TRUE;
   /* SJM 08/08/03 - must turn on assign chg processing for nets in mod */
   if (__iact_state) set_iact_dmpvmod_nd_nchgstore(mdp);
  }
 if (levels == 1) return; 

 for (ii = 0; ii < mdp->minum; ii++)
  { 
   down_ip = &(mdp->minsts[ii]);
   mark_mod_dvars_under(down_ip->imsym->el.emdp,
    (levels != 0) ? levels - 1 : 0);
  }
}
  

/*
 * for all dumpvars from called from interactive need to turn on all
 * nd chg store bits in all modules or else dumpvars records will not happen
 *
 * SJM 08/08/03 - this fixes bug that caused iact dumpvars not to work
 */
static void set_iact_dmpv_all_nd_nchgstore(void)
{
 register struct mod_t *mdp;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   set_iact_dmpvmod_nd_nchgstore(mdp);
  }
}

/*
 * set all nchg need chg store bits if dumpvars called from interactive
 * with module form
 *
 * this is same as nd nchg nchg nd chgstore adjustment when iact dces added
 * but the nchgaction bits will be set up right when iact dumpv stsk execed
 *
 * SJM - 08/08/03 - this is needed to make dumpvars from interactive code work
 * LOOKATME - maybe should not allow $dumpvars calls from iact mode
 */
static void set_iact_dmpvmod_nd_nchgstore(struct mod_t *mdp)
{
 register int32 ni;
 register struct net_t *np;
 register struct task_t *tskp;

 for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
  {
   /* can never dumpvar arrays */
   if (!np->n_isarr)
    {
     np->dmpv_in_src = TRUE;
     np->nchg_nd_chgstore = TRUE;
    }
  }
 for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
  {
   for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
    {
     if (!np->n_isarr)
      {
       np->dmpv_in_src = TRUE;
       np->nchg_nd_chgstore = TRUE;
      }
    }
  }
}

/*
 * check $readmemb or $readmemh arguments
 */
static void chkst_readmem(struct sy_t *syp, int32 anum,
 struct tskcall_t *tkcp)
{
 struct expr_t *alxp, *ndp;

 /* allow any expr that evaluates to string (i.e. treated as string) here */
 if (anum > 4 || anum < 2)
  {
   __sgferr(884,
    "%s system task has %d arguments not from 2 to 4", syp->synam, anum);
   /* leave since correct number will not be needed */
   return;
  }
 alxp = tkcp->targs;
 ndp = alxp->lu.x;
 __chk_rhsexpr(ndp, 0);

 alxp = alxp->ru.x;
 ndp = alxp->lu.x;
 if (!nd_unind_arr(ndp))
  {
   __sgferr(891,
    "%s system task second argument %s must be an unindexed memory name",
    syp->synam, __msgexpr_tostr(__xs, ndp));
   return;
  }
 if (anum > 2) { alxp = alxp->ru.x; __chk_rhsexpr(alxp->lu.x, 0); }
 if (anum > 3) { alxp = alxp->ru.x; __chk_rhsexpr(alxp->lu.x, 0); }
}

/*
 * return if expression is an unindexed array
 */
static int32 nd_unind_arr(struct expr_t *ndp)
{
 struct net_t *np;

 if (ndp->optyp != ID && ndp->optyp != GLBREF) return(FALSE);
 np = ndp->lu.sy->el.enp;
 /* notice (s)readmem assigns to memory so it is lhs indirectly here */
 if (np->nrngrep == NX_CT) np->nu.ct->n_onlhs = TRUE;
 if (np->ntyp < NONWIRE_ST || np->ntyp == N_EVENT) return(FALSE);
 if (!np->n_isarr) return(FALSE);
 return(TRUE);
}

/*
 * check $sreadmemb or $sreadmemh arguments
 */
static void chkst_sreadmem(struct sy_t *syp, int32 anum,
 struct tskcall_t *tkcp)
{
 register struct expr_t *alxp;

 if (anum < 4)
  {
   __sgferr(884, "%s system task has %d arguments but at least 4 required",
    syp->synam, anum);
   /* leave since correct number will not be needed */
   return;
  }
 alxp = tkcp->targs;
 if (!nd_unind_arr(alxp->lu.x))
  {
   __sgferr(895,
    "%s system task first argument %s must be an unindexed memory name",
    syp->synam, __msgexpr_tostr(__xs, alxp->lu.x));
   return;
  }
 alxp = alxp->ru.x;
 __chk_rhsexpr(alxp->lu.x, 0);
 alxp = alxp->ru.x;
 __chk_rhsexpr(alxp->lu.x, 0);
 /* all remaiing only needs to eval (forced) to string need not be literal */
 for (alxp = alxp->ru.x; alxp != NULL; alxp = alxp->ru.x)
  __chk_rhsexpr(alxp->lu.x, 0);
} 
 
/*
 * check $sdf_annotate system tsk arguments 
 */
static void chkst_sdfannotate_enable(struct tskcall_t *tkcp, int32 anum)
{
 register struct expr_t *alxp;
 register int32 argi;
 struct expr_t *ndp;

 if (anum < 1 || anum > 7)
  {
   __sgferr(884,
    "$sdf_annotate system task must have from 1 to 7 arguments - has %d",
    anum);
   return;
  }
 alxp = tkcp->targs;

 ndp = alxp->lu.x;
 /* allowing string expressions following other places in language */
 __chk_rhsexpr(ndp, 0);
 if (ndp->optyp == OPEMPTY)
  {
   __sgferr(918,
    "$sdf_annotate required first file name argument empty (, form illegal");
   return;
  }
 if (__iact_state)
  {
   __ia_err(1495,
    "$sdf_annotate system task call illegal as interactive command - must be added to source");
   return;
  }
 __has_sdfann_calls = TRUE;
 /* only first file name arg required */
 if (anum <= 1) return;

 /* this must always be xmr instance scope reference - already resolved */ 
 alxp = alxp->ru.x;
 ndp = alxp->lu.x;
 /* because this must be scope - can not use chk rhs expr for this arg */
 /* scope reference can be empty ,, form */
 if (ndp->optyp != OPEMPTY)
  {
   if (ndp->optyp != GLBREF || (ndp->lu.sy->sytyp != SYM_I
    && ndp->lu.sy->sytyp != SYM_M))
    {
     __sgferr(876, 
     "$sdf_annotate second argument %s illegal - must be module instance scope",
      __msgexpr_tostr(__xs, ndp));
     return;
    }
  }
 for (alxp = alxp->ru.x, argi = 3; alxp != NULL; alxp = alxp->ru.x, argi++)
  {
   ndp = alxp->lu.x;
   __chk_rhsexpr(ndp, 0);
   if (ndp->optyp == OPEMPTY) continue;

   switch (argi) {
    case 3:
     __sgfwarn(664, 
      "$sdf_annotate third config_file argument %s ignored - not in SDF standard",
      __msgexpr_tostr(__xs, ndp));
     break;
    /* SJM 07/08/01 - now supporting separate SDF log file */
    case 4: break;
    /* MTM to over-ride command line option supported */
    case 5: break;
    case 6:
      __sgfwarn(664, 
       "$sdf_annotate sixth scale factors argument %s ignored - not in SDF standard",
       __msgexpr_tostr(__xs, ndp));
      break;
     case 7:
      __sgfwarn(664, 
       "$sdf_annotate seventh scale type argument %s ignored - not in SDF standard",
       __msgexpr_tostr(__xs, ndp));
      break;
     default: __case_terr(__FILE__, __LINE__);
   }
  }
}

/*
 * ROUTINES TO FIX UP AND CHECK SPECIFY SECTION
 */

/*
 * check specparams
 */
extern void __chkfix_spfy(void)
{ 
 register struct spcpth_t *pthp;
 register struct tchk_t *tcp;
 int32 spec_empty, saveobj;
 struct spfy_t *spfp;
 struct tchk_t *suptcp, *rectcp;

 saveobj = __cur_declobj;
 __cur_declobj = SPECIFY;
 spec_empty = TRUE;
 spfp = __inst_mod->mspfy;
 /* make sure all specparams that were used are defined */
 /* unless all specparam values defined cannot check specify section */
 /* or determine specparam values */
 if (spfp->spfsyms->numsyms != 0)
  {
   spec_empty = FALSE;
   if (!chk_undef_specparams(spfp->spfsyms)) goto done;
  } 

 /* if top level module has paths - emit warning and ignore */
 if (__inst_mod->minstnum == 0)
  { 
   if (spfp->spcpths != NULL)  
    {
     __pv_warn(679, 
      "specify paths in top level module %s ignored - should be in library",
      __inst_mod->msym->synam);

     spec_empty = FALSE;
     free_spcpths(spfp->spcpths);
     spfp->spcpths = NULL;
    }
  }
 else 
  {
   /* check pin to pin delays */
   for (pthp = spfp->spcpths; pthp != NULL; pthp = pthp->spcpthnxt)
    {
     spec_empty = FALSE;
     if (!chk_1spcpth(pthp)) { pthp->pth_gone = TRUE; continue; }
     if (pthp->pthtyp == PTH_FULL) chk_rep_in_fullpth(pthp);
    }
   /* use all module paths to check for sdpds multiple same paths withs conds */
   chk_rep_sdpds(spfp);
  }

 /* check timing checks */
 for (tcp = spfp->tchks; tcp != NULL; tcp = tcp->tchknxt)
  {
   /* if marked gone because of error, do not split setuphold */
   spec_empty = FALSE;
   if (!chk_1tchk(tcp)) tcp->tc_gone = TRUE;
   else
    {
     if (tcp->tchktyp == TCHK_SETUPHOLD)
      {
       suptcp = bld_sup_of_suphld(tcp); 
       /* insert after current and move pointer so not checked */
       suptcp->tchknxt = tcp->tchknxt;
       tcp->tchknxt = suptcp;
       tcp = tcp->tchknxt;
      }
     else if (tcp->tchktyp == TCHK_RECREM)
      {
       rectcp = bld_rec_of_recrem(tcp); 
       /* insert after current and move pointer so not checked */
       rectcp->tchknxt = tcp->tchknxt;
       tcp->tchknxt = rectcp;
       tcp = tcp->tchknxt;
      } 
    } 
  }
done:
 if (spec_empty)
  __pv_warn(559, 
   "module %s specify section has no delay paths or timing checks",
   __inst_mod->msym->synam);
 __cur_declobj = saveobj;
}

/*
 * check for undefined but used specparams
 *
 * if a variable is used in a delay expr. or tchk limit or specparam rhs
 * it is assumed to be an undeclared specparam - this makes sure it appears
 * in a specparam statement 
 */
static int32 chk_undef_specparams(struct symtab_t *sytp)
{
 register int32 syi;
 int32 good;
 struct sy_t *syp;

 __wrkstab = sytp->stsyms;
 for (good = TRUE, syi = 0; syi < (int32) sytp->numsyms; syi++)
  {
   syp = __wrkstab[syi];
   if (!syp->sydecl)
    {
     __gferr(777, syp->syfnam_ind, syp->sylin_cnt,
      "specparam or path element %s not defined or not wire", syp->synam); 
     good = FALSE;
    }
  }
 return(good);
}

/*
 * check 1 specify path
 * converts from expression to path else simple wire or select 
 * will return F if error and path marked as gone
 *
 * any edge check at source input - will be correct by here or previous error
 * check cond. expr. here than can be any rhs expression
 * eval code handles selecting low bit if needed
 */
static int32 chk_1spcpth(struct spcpth_t *pthp) 
{
 register struct exprlst_t *pxlp;
 register int32 pei;
 int32 num_peinels, num_peoutels, pi1, pi2, dnum, sav_errcnt;
 word32 badoptyp;
 struct exprlst_t *pxins, *pxouts, *pxlp2;
 struct net_t *np;
 struct pathel_t *pep;
 struct expr_t *cndx;
 struct paramlst_t *pmp, *dhdr;
 char s1[RECLEN];

 sav_errcnt = __pv_err_cnt;
 __sfnam_ind = pthp->pthsym->syfnam_ind;
 __slin_cnt = pthp->pthsym->sylin_cnt;

 /* first check input and output path expr. lists */
 pxlp = (struct exprlst_t *) pthp->peins;
 /* do not bother free error stuff here - if non I/O will need to support */
 for (num_peinels = 0; pxlp != NULL; pxlp = pxlp->xpnxt, num_peinels++)
  {
   if (num_peinels == 0 && pxlp->xpnxt == NULL) strcpy(s1, "");
   else sprintf(s1, " terminal %d", num_peinels + 1); 
   chk_spterm(pxlp->xp, s1, "path input", IO_IN);
  }
 pxlp = (struct exprlst_t *) pthp->peouts;
 for (num_peoutels = 0; pxlp != NULL; pxlp = pxlp->xpnxt, num_peoutels++)
  {
   if (num_peoutels == 0 && pxlp->xpnxt == NULL) strcpy(s1, "");
   else sprintf(s1, " terminal %d", num_peoutels + 1); 
   chk_spterm(pxlp->xp, s1, "path output", IO_OUT);
  }
 if (sav_errcnt != __pv_err_cnt) goto chk_pdels;
 /* paths must always have 1 element to get here */
 if (num_peinels < 1 || num_peoutels < 1) __misc_terr(__FILE__, __LINE__);

 /* error for || paths to have lists - but continue checking */
 if (pthp->pthtyp == PTH_PAR && (num_peinels > 1 || num_peoutels > 1))
  {
   __sgferr(1085,
    "parallel (=>) path illegally has multiple sources (%d) and/or destinations (%d)",
    num_peinels, num_peoutels);
  }

 /* convert to pathel form and mark wires that are specify srcs and dsts */
 pxins = (struct exprlst_t *) pthp->peins;
 pxouts = (struct exprlst_t *) pthp->peouts;
 /* must build the path tables and mark path source and dest. wires */
 pthp->peins = (struct pathel_t *)
  __my_malloc(num_peinels*sizeof(struct pathel_t));
 pthp->last_pein = num_peinels - 1;
 for (pei = 0, pxlp = pxins; pei < num_peinels; pei++)
  {
   pxlp2 = pxlp->xpnxt;
   __xtract_wirng(pxlp->xp, &np, &pi1, &pi2);
   pep = &(pthp->peins[pei]);
   pep->penp = np;
   pep->pthi1 = pi1;
   pep->pthi2 = pi2;
   np->n_isapthsrc = TRUE;
   __free_xtree(pxlp->xp); 
   __my_free((char *) pxlp, sizeof(struct exprlst_t)); 
   pxlp = pxlp2;
  }
 pthp->peouts = (struct pathel_t *)
  __my_malloc(num_peoutels*sizeof(struct pathel_t));
 pthp->last_peout = num_peoutels - 1;
 for (pei = 0, pxlp = pxouts; pei < num_peoutels; pei++)
  {
   pxlp2 = pxlp->xpnxt;
   __xtract_wirng(pxlp->xp, &np, &pi1, &pi2);
   np->n_isapthdst = TRUE;
   pep = &(pthp->peouts[pei]);
   pep->penp = np;
   pep->pthi1 = pi1;
   pep->pthi2 = pi2;
   __free_xtree(pxlp->xp); 
   __my_free((char *) pxlp, sizeof(struct exprlst_t)); 
   pxlp = pxlp2;
  }
 /* mark form of path expressions changed to range form */
 pthp->pth_as_xprs = FALSE;

chk_pdels:
 /* first check cond. expr if present */
 if ((cndx = pthp->pthcondx) != NULL)
  {
   __chk_rhsexpr(cndx, 0);
   /* normal expression except no xmr's and only path condition operators */
   if (__expr_has_glb(cndx))
    {
     __sgferr(1022,
      "global hierarchical reference illegal in state dependent path condition %s",
      __msgexpr_tostr(__xs, cndx));
    }
   if (expr_has_nonpth(cndx, &badoptyp))
    {
     __sgferr(1022,
      "state dependent path condition (%s) illegal operator(s) [one bad: %s]",
      __msgexpr_tostr(__xs, cndx), __to_opname(badoptyp));
    }
  }
 /* if ifnone, must be simple module path */
 if (pthp->pth_ifnone)
  {
   if (pthp->pthcondx != NULL || pthp->pthedge != NOEDGE)
    {
     __sgferr(1012, "ifnone path illegal - has edge or is state dependent");
    }
  }

 /* next check and substitute numeric values for expressions */
 /* changed to sim form during prep */
 dhdr = __copy_dellst(pthp->pth_du.pdels);
 for (dnum = 0, pmp = dhdr; pmp != NULL; pmp = pmp->pmlnxt)
  {
   sprintf(s1, "path delay (element %d)", dnum + 1);
   __chk_spec_delay(pmp->plxndp, s1);
   dnum++;
  }
 __free_dellst(dhdr);
 if (dnum == 1 || dnum == 2 || dnum == 3 || dnum == 6 || dnum == 12) 
  {
   if (sav_errcnt != __pv_err_cnt) return(FALSE);
   return(TRUE);
  }
 __sgferr(791, "path delay illegal number of delays %d", dnum);
 return(FALSE);
}

/*
 * return T if expression has any illegal path cond. expr. operators
 * uses value from op info table
 */
static int32 expr_has_nonpth(struct expr_t *cndx, word32 *badop)
{
 struct opinfo_t *opip;

 *badop = UNDEF;
 if (__isleaf(cndx)) return(FALSE);
 opip = &(__opinfo[cndx->optyp]);
 if (!opip->pthexpop)
  {
   *badop = cndx->optyp;
   return(TRUE);
  }

 if (cndx->lu.x != NULL)
  {
   if (expr_has_nonpth(cndx->lu.x, badop)) return(TRUE);
  }  
 if (cndx->ru.x != NULL)
  {
   if (expr_has_nonpth(cndx->ru.x, badop)) return(TRUE);
  }
 return(FALSE);
}

/*
 * check all paths to make sure no same paths withing full (*>) list
 *
 * because duplicate entries for same sdpds need different conditions
 * error for repeated path in one statement
 *
 * only called for full *> paths
 */
static void chk_rep_in_fullpth(struct spcpth_t *pthp)
{
 register int32 pei, pei2;
 struct pathel_t *pep1, *pep2;
 char s1[RECLEN], s2[RECLEN];

 /* first check for repeated ins - because full will be repeated */
 for (pei = 0; pei <= pthp->last_pein; pei++)
  {
   pep1 = &(pthp->peins[pei]);
   for (pei2 = pei + 1; pei2 <= pthp->last_pein; pei2++)
    {
     pep2 = &(pthp->peins[pei2]);
     if (pep1->penp != pep2->penp) continue; 
     /* exactly same path repeated */
     if (pep1->pthi1 == pep2->pthi1 && pep1->pthi2 == pep2->pthi2)
      {
       __gferr(1059,  pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
        "full path input element %s repeated in one path", 
        pthel_tostr(s1, pep1));
       continue;
      }
     /* overlapped path repeated */ 
     if (pth_overlap(pep1, pep2))
      {
       __gferr(1079,  pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
        "path path input element %s overlaps %s in one path",
        pthel_tostr(s1, pep1), pthel_tostr(s2, pep2));
      } 
     }
   }
 /* next check for repeated outs - because full will be repeated */
 for (pei = 0; pei <= pthp->last_peout; pei++)
  {
   pep1 = &(pthp->peins[pei]);
   for (pei2 = pei + 1; pei2 <= pthp->last_pein; pei2++)
    {
     pep2 = &(pthp->peouts[pei2]);
     if (pep1->penp != pep2->penp) continue; 
     /* exactly same path repeated */
     if (pep1->pthi1 == pep2->pthi1 && pep1->pthi2 == pep2->pthi2)
      {
       __gferr(1059, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
        "full path output element %s repeated in one path", 
        pthel_tostr(s1, pep1));
       pthp->pth_gone = TRUE;
       continue;
      }
     /* overlapped path elements error */
     if (pth_overlap(pep1, pep2))
      {
       __gferr(1079,  pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
        "path output element %s overlaps %s in one path",
        pthel_tostr(s1, pep1), pthel_tostr(s2, pep2));
       pthp->pth_gone = TRUE;
      }
    }
  }
}

/*
 * convert a simple (non list form) path to a string
 */
static char *pth_tostr(char *s, struct spcpth_t *pthp,
 struct pathel_t *pep1, struct pathel_t *pep2)
{
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 if (pthp->pthtyp == PTH_FULL) strcpy(s1, "*>"); else strcpy(s1, "=>");
 sprintf(s, "(%s %s %s)", pthel_tostr(s2, pep1), s1, pthel_tostr(s3, pep2));
 return(s);
}

/*
 * convert path element to string
 */
static char *pthel_tostr(char *s, struct pathel_t *pep)
{
 if (pep->pthi1 == -1) strcpy(s, pep->penp->nsym->synam);
 else if (pep->pthi1 == pep->pthi2) sprintf(s, "%s[%d]",
  pep->penp->nsym->synam, pep->pthi1);
 else sprintf(s, "%s[%d:%d]", pep->penp->nsym->synam, pep->pthi1, pep->pthi2);
 return(s);
}

/*
 * return T if 2 path elements overlap - know not called if identical
 */
static int32 pth_overlap( struct pathel_t *pep1, struct pathel_t *pep2)
{
 int32 i1, i2, o1, o2;

 if (pep1->pthi1 >= pep1->pthi2) { i1 = pep1->pthi1; i2 = pep1->pthi2; }
 else { i1 = pep1->pthi2; i2 = pep1->pthi1; }
 if (pep2->pthi1 >= pep2->pthi2) { o1 = pep2->pthi1; o2 = pep2->pthi2; }
 else { o1 = pep2->pthi2; o2 = pep2->pthi1; }
 if (i2 > o1 || o2 > i1) return(FALSE);
 return(TRUE);
}

/*
 * check repeated sdpd paths
 */
static void chk_rep_sdpds(struct spfy_t *spfp)
{
 register struct xpnd_pthel_t *xpthp, *xpthp2;
 int32 numxpths, last_xpi, xpi, has_ifnone;
 struct xpnd_pthel_t *xpth_hdr, **xpth_equivs;
 struct spcpth_t *pthp, *pthp2;
 struct pathel_t *pep1s, *pep1e, *pep2s, *pep2e;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];
 
 /* for paths that are gone, expanded elements not added */
 /* if none expanded, done */
 if ((xpth_hdr = xpnd_pths(spfp, &numxpths)) == NULL) return;

 /* since as big as entire list know big enough */
 xpth_equivs = (struct xpnd_pthel_t **)
  __my_malloc(numxpths*sizeof(struct xpnd_pthel_t *));

 for (xpthp = xpth_hdr; xpthp != NULL; xpthp = xpthp->xpthnxt)
  {
   if (xpthp->in_equiv_set) continue;
   xpth_equivs[0] = xpthp; 
   /* since processing done, makr as in equiv. class (maybe size 1) */ 
   xpthp->in_equiv_set = TRUE;
   last_xpi = 0;
   pthp = xpthp->pthp;
   pep1s = &(pthp->peins[xpthp->peii]);
   pep1e = &(pthp->peouts[xpthp->peoi]);
   for (xpthp2 = xpthp->xpthnxt; xpthp2 != NULL; xpthp2 = xpthp2->xpthnxt)
    {
     if (xpthp2->in_equiv_set) continue;

     pthp2 = xpthp2->pthp;
     pep2s = &(pthp2->peins[xpthp2->peii]);
     pep2e = &(pthp2->peouts[xpthp2->peoi]);

     /* if both src or both dest nets differ, eliminate */
     if (pep1s->penp != pep2s->penp || pep1e->penp != pep2e->penp) continue;
     
     /* range must be exact match */
     if (pep1s->pthi1 == pep2s->pthi1 && pep1s->pthi2 == pep2s->pthi2
      && pep1e->pthi1 == pep2e->pthi1 && pep1e->pthi2 == pep2e->pthi2)
      { 
       xpth_equivs[++last_xpi] = xpthp2; 
       xpthp2->in_equiv_set = TRUE;
       continue;
      }

     /* probably same paths but not coded the same way - error */ 
     /* notice some source bit(s) to two different dest bit(s) ok */
     if (pth_overlap(pep1s, pep2s) && pth_overlap(pep1e, pep2e))
      {
       __gferr(1118, pthp2->pthsym->syfnam_ind, pthp2->pthsym->sylin_cnt,
        "path %s and path %s at %s overlap - multiple sdpds must have identical ranges",
        pth_tostr(s1, pthp2, pep2s, pep2e), pth_tostr(s2, pthp, pep1s, pep1e), 
        __bld_lineloc(s3, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt));
       /* mark so will test for other overlap equivalences */ 
       xpthp2->in_equiv_set = TRUE;
      }
    }
   /* check equivalence class (even if only one element) */
   if (last_xpi == 0)
    {
     pthp2 = xpth_equivs[0]->pthp;
     if (pthp2->pth_ifnone)
      {
       pthp2->pth_ifnone = FALSE;
       __gfinform(482, pthp2->pthsym->syfnam_ind, pthp2->pthsym->sylin_cnt,
        "path has ifnone condition but no other sdpds for path - made simple");
      }
     continue;
    }
   /* real multiple path - only check makes sure either has cond or edge */
   /* LOOKATME - will not work if same expressions but can not check this */
   for (has_ifnone = FALSE, xpi = 0; xpi <= last_xpi; xpi++)
    {
     xpthp2 = xpth_equivs[xpi];
     pthp2 = xpthp2->pthp;
     /* if none must be simple - already check */
     if (pthp2->pth_ifnone)
      {
       if (has_ifnone)
        {
         __gferr(1125, pthp2->pthsym->syfnam_ind, pthp2->pthsym->sylin_cnt,
          "more than one ifnone path in sdpd same path group illegal");
        }
       else has_ifnone = TRUE;
       continue;
      }
     /* other in same path sdpd group must be non simple */
     if (pthp2->pthcondx == NULL && pthp2->pthedge == NOEDGE)
      {
       __gferr(1126, pthp2->pthsym->syfnam_ind, pthp2->pthsym->sylin_cnt,
        "path in sdpd same path group illegally simple - needs edge or conditon");
      }
    }
  }

 /* free the expanded path list */
 __my_free((char *) xpth_equivs, numxpths*sizeof(struct xpnd_pthel_t *));
 for (xpthp = xpth_hdr; xpthp != NULL;)
  {
   xpthp2 = xpthp->xpthnxt;
   __my_free((char *) xpthp, sizeof(struct xpnd_pthel_t));
   xpthp = xpthp2;
  } 
}

/*
 * expand paths into list of simple (non list form) elements
 */
static struct xpnd_pthel_t *xpnd_pths(struct spfy_t *spfp,
 int32 *numxpths)
{
 register int32 pii, poi;
 int32 numxps;
 struct spcpth_t *pthp;
 struct xpnd_pthel_t *xpth_hdr, *xpth_end, *xpthp; 

 xpth_hdr = xpth_end = NULL;
 numxps = 0;
 for (pthp = spfp->spcpths; pthp != NULL; pthp = pthp->spcpthnxt)
  {
   if (pthp->pth_gone) continue;

   if (pthp->pthtyp == PTH_PAR)
    {
     xpthp = (struct xpnd_pthel_t *) __my_malloc(sizeof(struct xpnd_pthel_t));
     xpthp->in_equiv_set = FALSE;
     xpthp->pthp = pthp;
     xpthp->peii = xpthp->peoi = 0;
     xpthp->xpthnxt = NULL;
     if (xpth_hdr == NULL) xpth_hdr = xpth_end = xpthp;
     else { xpth_end->xpthnxt = xpthp; xpth_end = xpthp; } 
     numxps++; 
     continue;
    }
   /* harder full path case */
   for (pii = 0; pii <= pthp->last_pein; pii++)
    {
     for (poi = 0; poi <= pthp->last_peout; poi++)
      {
       xpthp = (struct xpnd_pthel_t *) __my_malloc(sizeof(struct xpnd_pthel_t));
       xpthp->in_equiv_set = FALSE;
       xpthp->pthp = pthp;
       xpthp->peii =pii; 
       xpthp->peoi = poi;
       xpthp->xpthnxt = NULL;
       if (xpth_hdr == NULL) xpth_hdr = xpth_end = xpthp;
       else { xpth_end->xpthnxt = xpthp; xpth_end = xpthp; } 
       numxps++; 
      }
    }
  }
 *numxpths = numxps;
 return(xpth_hdr);
}

/*
 * check a specify section delay value
 */
extern void __chk_spec_delay(struct expr_t *ndp, char *emsg)
{
 /* first must be expr. of numbers and parameters only */
 if (!__chk_paramexpr(ndp, 0))
  {
   __sgferr(898, "%s must be specify constant expression", emsg);
   return;
  }
 /* then must fold */
 fold_subexpr(ndp);
 /* finally check to make sure number delay - prep code scales */
 __chk_numdelay(ndp, emsg);
 ndp->ibase = BDEC;
}

/*
 * check specify terminal (path or timing check)
 * do not remove since will not get to prep. code
 */
static void chk_spterm(struct expr_t *spxp, char *spos, char *snam,
 int32 iodir)
{
 struct net_t *np;
 struct expr_t *xp, *xp2;

 /* if even path destination (could be rhs reg), here must be lhs wire */
 if (spxp->optyp == LCB)
  {
   __sgferr(914, "%s%s concatenate illegal", snam, spos);
   return;
  }
 if (spxp->optyp == GLBREF)
  {
   __sgferr(899, "%s%s hierarchical reference illegal", snam, spos); 
   return;
  }
 /* exactly 3 things legal, port wire, bsel of port wire, psel of port wire */ 
 switch (spxp->optyp) {
  case ID: 
   np = spxp->lu.sy->el.enp;

chk_iodir:
   /* timing check terminal can be any wire */
   if (iodir == NON_IO)
    {
     if (np->ntyp >= NONWIRE_ST)
      {
       __sgferr(901, "%s%s terminal %s %s must be a wire",
        snam, spos, __to_wtnam(__xs, np), np->nsym->synam);
      }
     return;
    }

   /* check to make sure terminal is port */
   if (np->iotyp == NON_IO)
    {
     __sgferr(905, "%s%s %s %s must be a module I/O port",
      snam, spos, __to_wtnam(__xs, np), np->nsym->synam);
     return;
    }
   if (iodir == IO_IN)
    {
     if (np->iotyp != IO_IN && np->iotyp != IO_BID)
      {
       __sgferr(906, "%s%s port %s must be input or inout", snam, spos,
        np->nsym->synam); 
       return;
      }
    }
   else
    {
     if (np->iotyp != IO_OUT && np->iotyp != IO_BID)
      {
       __sgferr(907, "%s%s port %s must be output or inout", snam, spos,
        np->nsym->synam); 
       return;
      }
    }
   break;
  case LSB:
   np = spxp->lu.x->lu.sy->el.enp;
   xp = spxp->ru.x;
   /* index must only contain specparams and numbers */
   if (!__chk_paramexpr(xp, 0)) return;
   /* next must fold it */
   fold_subexpr(xp);
   /* result must be number (no IS form) or error */
   if (xp->optyp != NUMBER)
    {
     __sgferr(908,
      "%s%s port bit select %s must contain only numbers and specparams",
      snam, spos, __msgexpr_tostr(__xs, spxp));
    }
   /* finally check and normalize bit select */
   chk_inrng_bsel(spxp);
   goto chk_iodir;
  case PARTSEL:
   np = spxp->lu.x->lu.sy->el.enp;
   xp = spxp->ru.x->lu.x;
   xp2 = spxp->ru.x->ru.x;
   /* index must only contain specparams and numbers */
   if (!__chk_paramexpr(xp, 0) || !__chk_paramexpr(xp2, 0)) return;
   /* next must fold it */
   fold_subexpr(xp);
   fold_subexpr(xp2);
   /* result must be number (no IS form) or error */
   if (xp->optyp != NUMBER || xp2->optyp != NUMBER)
    {
     __sgferr(904,
      "%s%s port part select %s must contain only numbers and specparams",
      snam, spos, __msgexpr_tostr(__xs, spxp));
    }
   /* finally check and normalize bit select */
   chk_inrng_psel(spxp);
   goto chk_iodir;
  default:
   __sgferr(915, "%s%s illegal expression %s", snam, spos,
     __msgexpr_tostr(__xs, spxp));
 } 
}

/*
 * check 1 specify timing check
 *
 * notice only checking here - width/period event duplicated in prep code
 * limit evaluation routines must be recalled if specparams changed
 * any edges read and checking during source input but cond checked here
 */
static int32 chk_1tchk(struct tchk_t *tcp)
{
 int32 sav_errcnt;
 struct paramlst_t *dhdr;
 char s1[RECLEN], s2[RECLEN];

 __sfnam_ind = tcp->tcsym->syfnam_ind;
 __slin_cnt = tcp->tcsym->sylin_cnt;

 sav_errcnt = __pv_err_cnt;
 sprintf(s1, " %s timing check", __to_tcnam(s2, tcp->tchktyp)); 
 switch ((byte) tcp->tchktyp) { 
  case TCHK_SETUP: case TCHK_HOLD: case TCHK_SKEW: case TCHK_SETUPHOLD:
  case TCHK_RECOVERY: case TCHK_REMOVAL: case TCHK_RECREM:
   /* SJM 01/16/04 - $removal has reversed terms to $recovery as setup/hold */
   chk_spterm(tcp->startxp, "first event", s1, NON_IO);
   if (tcp->startcondx != NULL) chk_tccond(tcp->startcondx, "first", s1);
   chk_spterm(tcp->chkxp, "second event", s1, NON_IO);
   if (tcp->chkcondx != NULL) chk_tccond(tcp->chkcondx, "second", s1);
   chk_notifier(tcp, s1);

   /* this will find errors and if errors convert to 0 */
   /* notice leave 1st (setup) limit even though not used for hold part */ 
   dhdr = __copy_dellst(tcp->tclim_du.pdels);
   __chk_spec_delay(dhdr->plxndp, "timing check limit");
   __free_dellst(dhdr);

   /* this is hold half limit but kept in original not added setup */
   if (tcp->tchktyp == TCHK_SETUPHOLD)
    {
     dhdr = __copy_dellst(tcp->tclim2_du.pdels);
     __chk_spec_delay(dhdr->plxndp, "setuphold second (hold) limit");
     __free_dellst(dhdr);
    }
   /* this is removal half limit but kept in original not added recovery */
   else if (tcp->tchktyp == TCHK_SETUPHOLD)
    {
     dhdr = __copy_dellst(tcp->tclim2_du.pdels);
     __chk_spec_delay(dhdr->plxndp, "recrem second (removal) limit");
     __free_dellst(dhdr);
    }
   break;
  case TCHK_WIDTH: case TCHK_PERIOD:
   /* one event, 2 limits for width but 2nd threshold is optional */  
   chk_spterm(tcp->startxp, "first edge", s1, NON_IO);
   if (tcp->startcondx != NULL) chk_tccond(tcp->startcondx, "first edge", s1);
   chk_notifier(tcp, s1);
   /* check delay expr. */
   dhdr = __copy_dellst(tcp->tclim_du.pdels);
   __chk_spec_delay(dhdr->plxndp, "timing check limit");
   __free_dellst(dhdr);

   if (tcp->tchktyp == TCHK_WIDTH)
    {
     dhdr = __copy_dellst(tcp->tclim2_du.pdels);
     __chk_spec_delay(dhdr->plxndp, "width second limit");
     __free_dellst(dhdr);
    }
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 /* if any errors, return fail (F) */ 
 if (sav_errcnt != __pv_err_cnt) return(FALSE); 
 return(TRUE);
}

/*
 * check the option condition
 * notice this is procedural rhs
 */
static void chk_tccond(struct expr_t *cndx, char *spos, char *snam)
{
 struct net_t *np;

 /* this expr. needs to be one bit but width self determined */
 __chk_rhsexpr(cndx, 0);
 switch ((byte) cndx->optyp) {
  case ID:
   np = cndx->lu.sy->el.enp;
chk_1bit:
   /* notice - wires ok here - just gets evaluated */
   if (np->n_isarr)
    {
     __sgferr(909, "%s %s &&& condition array %s illegal", spos, snam,
      np->nsym->synam); 
     return;
    }
   if (cndx->szu.xclen > 1)
    {
     __sgfwarn(613,
      "%s %s &&& condition expression %s wider than 1 bit - low bit used",
      spos, snam, __msgexpr_tostr(__xs, cndx));
    }
   return;
  case BITNOT: 
   if (cndx->lu.x->optyp != ID)
    __sgferr(910,
     "%s %s &&& condition ~ (bit not) operand too complicated (must be identifier)",
     spos, snam);
   np = cndx->lu.x->lu.sy->el.enp;
   goto chk_1bit;
  case RELCEQ:
  case RELCNEQ:
  case RELEQ: 
  case RELNEQ:
   if (cndx->lu.x->optyp != ID)
    {
     __sgferr(912,
      "%s %s &&& condition left operand too complicated (must be identifier)",
      spos, snam);
     return; 
    }
   np = cndx->lu.x->lu.sy->el.enp;
   if (cndx->ru.x->optyp != NUMBER || cndx->ru.x->szu.xclen > WBITS)
    {
bad_const:
     __sgferr(911,
      "%s %s &&& condition right operand must be a scalar constant",
      spos, snam);
     goto chk_1bit;
    }
   if (cndx->ru.x->szu.xclen != 1)
    {
     word32 av, bv;
     word32 *wp;

     wp = &(__contab[cndx->ru.x->ru.xvi]);
     av = wp[0];
     bv = wp[1];
     if ((av != 0L && av != 1L) || (bv != 0L && bv != 1L)) goto bad_const;  
     cndx->ru.x->szu.xclen = 1;
    }
   goto chk_1bit;
  default:
   __sgferr(913,
    "%s %s &&& condition expression must be one operator scalar expression",
    spos, snam);
 }
}

/*
 * check a notifier - must be register
 * this also changed coded symbol to wire or NULL
 */
static void chk_notifier(struct tchk_t *tcp, char *snam)
{
 struct sy_t *syp;

 if (tcp->ntfy_np == NULL) return;
 syp = (struct sy_t *) tcp->ntfy_np;
 if (syp->sytyp != SYM_N || syp->el.enp->ntyp != N_REG
  || syp->el.enp->n_isavec)
  {
   __sgferr(916,
    "%s notify symbol %s is not a scalar register", snam, syp->synam);
   tcp->ntfy_np = NULL;
   return;
  }
 tcp->ntfy_np = syp->el.enp;
}

/*
 * build setuphold new setup timing check
 *
 * setuphold contains hold half (ref. and data event orders match), 
 * setup has reversed but uses 1st delay from hold referenced thru lim
 */
static struct tchk_t *bld_sup_of_suphld(struct tchk_t *otcp)
{
 struct tchk_t *ntcp;

 ntcp = (struct tchk_t *) __my_malloc(sizeof(struct tchk_t));
 __init_tchk(ntcp, TCHK_SETUP);
 ntcp->tc_supofsuphld = TRUE;
 /* notice the 2 halves share symbol and handled specially in mod copy */
 ntcp->tcsym = otcp->tcsym;
 /* reverse edges */
 ntcp->startedge = otcp->chkedge;
 ntcp->chkedge = otcp->startedge;

 /* copy start reference event expr. from check */
 if (otcp->startxp != NULL) ntcp->chkxp = __copy_expr(otcp->startxp);
 if (otcp->startcondx != NULL)
  ntcp->chkcondx = __copy_expr(otcp->startcondx);

 /* copy start reference event expr. from check */
 if (otcp->chkxp != NULL) ntcp->startxp = __copy_expr(otcp->chkxp);
 if (otcp->chkcondx != NULL)
  ntcp->startcondx = __copy_expr(otcp->chkcondx);
 /* point first delay to master hold and get delay (1st) from there */
 ntcp->tclim_du.pdels = (struct paramlst_t *) otcp;

 /* notice this is intra module so can just copy - becomes net */
 ntcp->ntfy_np = otcp->ntfy_np;
 return(ntcp);
}

/*
 * build recrems new recovery timing check
 * SJM - 01/16/04 added to support new 2001 LRM $recrem
 *
 * recrem contains removal half (ref. and data event orders match), 
 * recovery has reversed but uses 1st delay from removal referenced thru lim
 */
static struct tchk_t *bld_rec_of_recrem(struct tchk_t *otcp)
{
 struct tchk_t *ntcp;

 ntcp = (struct tchk_t *) __my_malloc(sizeof(struct tchk_t));
 __init_tchk(ntcp, TCHK_RECOVERY);
 ntcp->tc_recofrecrem = TRUE;
 /* notice the 2 halves share symbol and handled specially in mod copy */
 ntcp->tcsym = otcp->tcsym;
 /* reverse edges */
 ntcp->startedge = otcp->chkedge;
 ntcp->chkedge = otcp->startedge;

 /* copy start reference event expr. from check */
 if (otcp->startxp != NULL) ntcp->chkxp = __copy_expr(otcp->startxp);
 if (otcp->startcondx != NULL)
  ntcp->chkcondx = __copy_expr(otcp->startcondx);

 /* copy start reference event expr. from check */
 if (otcp->chkxp != NULL) ntcp->startxp = __copy_expr(otcp->chkxp);
 if (otcp->chkcondx != NULL)
  ntcp->startcondx = __copy_expr(otcp->chkcondx);
 /* point first delay to master hold and get delay (1st) from there */
 ntcp->tclim_du.pdels = (struct paramlst_t *) otcp;

 /* notice this is intra module so can just copy - becomes net */
 ntcp->ntfy_np = otcp->ntfy_np;
 return(ntcp);
}

/*
 * emit unused informs for all parameters and specparams
 */
extern void __emit_param_informs(void)
{
 register int32 pi;
 register struct net_t *np;
 register struct task_t *tskp;
 
 for (pi = 0; pi < __inst_mod->mprmnum; pi++)
  {
   np = &(__inst_mod->mprms[pi]);
   if (!np->n_isaparam || np->nu.ct->n_onrhs) continue; 

   __gfinform(451, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
    "in %s: parameter %s unused", __inst_mod->msym->synam, np->nsym->synam);
  }

 /* AIV 09/27/06 - also need to check the local params */
 for (pi = 0; pi < __inst_mod->mlocprmnum; pi++)
  {
   np = &(__inst_mod->mlocprms[pi]);
   if (!np->n_isaparam || !np->nu.ct->p_locparam || np->nu.ct->n_onrhs)
    continue; 

   __gfinform(451, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
    "in %s: localparam %s unused", __inst_mod->msym->synam, np->nsym->synam);
  }

 for (tskp = __inst_mod->mtasks; tskp != NULL; tskp = tskp->tsknxt)
  { 
   for (pi = 0; pi < tskp->tprmnum; pi++)
    {
     np = &(tskp->tsk_prms[pi]);
     if (!np->n_isaparam || np->nu.ct->n_onrhs) continue; 

     __gfinform(451, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
      "in %s.%s: parameter %s unused", __inst_mod->msym->synam,
      tskp->tsksyp->synam, np->nsym->synam);
    }
   for (pi = 0; pi < tskp->tlocprmnum; pi++)
    {
     np = &(tskp->tsk_locprms[pi]);
     if (!np->n_isaparam || !np->nu.ct->p_locparam || np->nu.ct->n_onrhs)
      continue; 

     __gfinform(451, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
      "in %s.%s: localparam %s unused", __inst_mod->msym->synam,
      tskp->tsksyp->synam, np->nsym->synam);
    }
  }

 if (__no_specify || __inst_mod->mspfy == NULL) return; 
 for (pi = 0; pi < __inst_mod->mspfy->sprmnum; pi++)
  {
   np = &(__inst_mod->mspfy->msprms[pi]);
   /* DBG remove --- */
   if (!np->n_isaparam) __misc_terr(__FILE__, __LINE__);
   /* --- */
   if (np->nu.ct->n_onrhs) continue; 

   __gfinform(452, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
    "in %s: specparam %s unused", __inst_mod->msym->synam, np->nsym->synam);
  }
}

/*
 * free specify
 * this can only be called be prep of specify section called
 */
extern void __free_specify(struct mod_t *mdp)
{
 struct spfy_t *spfp;

 spfp = mdp->mspfy;
 if (spfp->spfsyms != NULL) free_frozen_symtab(spfp->spfsyms);
 if (spfp->spcpths != NULL) free_spcpths(spfp->spcpths);
 if (spfp->tchks != NULL) __free_tchks(spfp->tchks);
 if (spfp->msprms != NULL) free_spcparms(spfp->msprms, spfp->sprmnum);
}

/*
 * free specify section paths
 */
static void free_spcpths(struct spcpth_t *pthp)
{
 register int32 pi;
 struct pathel_t *pep;
 struct spcpth_t *pthp2;

 for (; pthp != NULL;)
  {
   pthp2 = pthp->spcpthnxt;
   /* normal case free as fixup to path elements */
   if (!pthp->pth_as_xprs)
    {
     /* first mark all path source and destination wires as non paths */ 
     for (pi = 0; pi <= pthp->last_pein; pi++)
      { pep = &(pthp->peins[pi]); pep->penp->n_isapthsrc = FALSE; }
     for (pi = 0; pi <= pthp->last_peout; pi++)
      { pep = &(pthp->peouts[pi]); pep->penp->n_isapthdst = FALSE; }

     /* next delete the path range arrays */
     if (pthp->last_pein >= 0) __my_free((char *) pthp->peins,
      (pthp->last_pein + 1)*sizeof(struct pathel_t)); 
     if (pthp->last_peout >= 0) __my_free((char *) pthp->peouts,
      (pthp->last_peout + 1)*sizeof(struct pathel_t)); 
    }
   else
    {
     /* freeing before fixup must free expr. list */
     __free_xprlst((struct exprlst_t *) pthp->peins);
     __free_xprlst((struct exprlst_t *) pthp->peouts);
    }
   /* free does nothing if nil */
   __free_dellst(pthp->pth_du.pdels);
   __free_xtree(pthp->datasrcx);
   __free_xtree(pthp->pthcondx);

   __my_free((char *) pthp, sizeof(struct spcpth_t)); 
   pthp = pthp2;
  }
}

/*
 * free timing checks
 */
extern void __free_tchks(struct tchk_t *tcp)
{
 struct tchk_t *tcp2;

 for (; tcp != NULL;) 
  {
   tcp2 = tcp->tchknxt;
   /* notice freeing null expr. is ok, does nothing */
   /* and for setup of setuphold and recovery of recrem these are copied */
   __free_xtree(tcp->startxp);
   __free_xtree(tcp->startcondx);
   __free_xtree(tcp->chkxp);
   __free_xtree(tcp->chkcondx);
   /* but delays shared (not copied) with hold of setup hold - can't free */
   if (!tcp->tc_supofsuphld && !tcp->tc_recofrecrem)
    {
     __free_dellst(tcp->tclim_du.pdels);
     __free_dellst(tcp->tclim2_du.pdels);
    }
   /* must not free symbol for ntfy_np */
   __my_free((char *) tcp, sizeof(struct tchk_t)); 
   tcp = tcp2;
  }
}

/*
 * free specify parameters - passing spec param (net) table
 */
static void free_spcparms(struct net_t *nptab, int32 pnum)
{
 register int32 pi;
 struct net_t *np;
 struct ncomp_t *ncomp;

 /* first free insides */ 
 for (pi = 0; pi < pnum; pi++) 
  {
   np = &(nptab[pi]);
   /* nothing inside net but comp union to free (at fixup time) */
   ncomp = np->nu.ct;
   /* just free the expressions since if arg. nil does nothing */
   __free_xtree(ncomp->nx1);
   __free_xtree(ncomp->nx2);
   __free_xtree(ncomp->ax1);
   __free_xtree(ncomp->ax2);
   /* will free the ncomp in blocks later */
   /* SJM 03/29/99 - no list - only one value (maybe min:nom:max) */
   __free_xtree(ncomp->n_dels_u.d1x);
  }
 /* then free entire block */
 __my_free((char *) nptab, pnum*sizeof(struct net_t));
}

/*
 * free 1 frozen symbol table
 */
static void free_frozen_symtab(struct symtab_t *sytp)
{
 register int32 syi;
 struct sy_t *syp;  

 /* may be dummy symbol table */
 if (sytp->numsyms == 0) return;

 if (sytp->stsyms == NULL) __misc_terr(__FILE__, __LINE__);
 for (syi = 0; syi < (int32) sytp->numsyms; syi++)
  {
   /* frozen form is array of ptrs to symbols */
   syp = sytp->stsyms[syi]; 
   /* notice must leave symbol names even though not accessible */
   __my_free((char *) syp, sizeof(struct sy_t)); 
  }
 __my_free((char *) sytp->stsyms,
  (int32) (sytp->numsyms*sizeof(struct sy_t *)));
 __my_free((char *) sytp, sizeof(struct symtab_t));
}
