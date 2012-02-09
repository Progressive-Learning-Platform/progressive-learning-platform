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
 * run time execution routines - rhs evaluation and readmem
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>

#ifdef __DBMALLOC__
#include "../malloc.h"
#endif

/* REMOVEME - no longer supporting SunOS - maybe needed for hpux? */
#if defined(__sparc) && !defined(__SVR4)  
extern int32 tolower(int32);
extern ungetc(int32 c, FILE *);
#endif

#include "v.h"
#include "cvmacros.h"

/* local prototypes */
static void prep_bld_monit_dces(struct expr_t *, int32);
static void linkon_monit_dce(struct net_t *, int32, int32, int32,
 struct itree_t *);
static int32 chk_monits_chged(register struct dceauxlst_t *);
static int32 chk_rm_rng_legal(int32, int32, int32, char *);
static void push_bsel(struct expr_t *);
static void push_psel(register struct expr_t *);
static int32 mdata_gettok(FILE *, int32);
static int32 rmrd_comment(FILE *);
static int32 mdata_rdhex(FILE *, int32);
static int32 is_mdataxdigit(int32);
static int32 mdata_rdbin(FILE *, int32);
static int32 rm_getc(FILE *);
static void rm_ungetc(int32, FILE *);
static int32 is_mdatabit(int32);
static void do_srm_xtrct(struct expr_t *, int32, struct net_t *, int32, int32,
 int32, int32, int32);
static double stdnorm_dev(int32 *);
static double gamma_dev(double, int32 *);
static int32 poisson_dev(int32, int32 *);
static double log_gamma(double);
static void lxqcol(register struct xstk_t *, register struct xstk_t *,
 register struct xstk_t *, int32, int32, int32);
static void eval_unary(struct expr_t *);
static void eval_wide_unary(register struct expr_t *,
 register struct xstk_t *);
static void eval_binary(struct expr_t *);
static void eval_wide_binary(struct expr_t *, register struct xstk_t *,
 register struct xstk_t *);
static void bitmwrshift(register word32 *, register int32, register int32);
static void bitmwlshift(register word32 *, register int32, register int32);
static void wrdmwrshift(register word32 *, register int32, register int32);
static void wrdmwlshift(register word32 *, register int32, register int32);
static int32 sgn_linc(register word32 *, int32);
static int32 accmuladd32(word32 *, word32 *, word32, word32 *, int32);
static void mpexpr_zdiv(word32 *, word32 *, word32 *, int32, word32 *, int32);
static int32 ztrim(word32 *, int32);
static void dadd(word32 *, word32 *, int32, int32);
static int32 dsub(word32 *, int32, word32 *, int32, int32, int32);
static void dmul(word32 *, int32, word32 *, int32, word64);
static int32 ldiv_cmp(register word32 *, register word32 *, int32);
static void xchg_stk(int32, int32);
static double uniform(int32 *, sword32, sword32);
static sword32 rtl_dist_uniform(int32 *, sword32, sword32);

/* extern prototypes (maybe defined in this module) */
extern char *__my_realloc(char *, int32, int32);
extern char *__my_malloc(int32);
extern void __my_free(char *, int32);
extern void __xmrpush_refgrp_to_targ(struct gref_t *);
extern struct xstk_t *__eval2_xpr(struct expr_t *);
extern char *__regab_tostr(char *, word32 *, word32 *, int32, int32, int32);
extern void __ld_stval(register word32 *, register word32 *, register byte *, int32);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern char __to_baselet(int32);
extern struct dcevnt_t *__alloc_dcevnt(struct net_t *);
extern FILE *__tilde_fopen(char *, char *);
extern word32 __wrd_redxor(word32);
extern double __cnvt_stk_to_real(struct xstk_t *, int32);
extern char *__bld_lineloc(char *, word32, int32);
extern char *__to_timstr(char *, word64 *);
extern char *__vval_to_vstr(word32 *, int32, int32 *);
extern char * __get_eval_cstr(struct expr_t *, int32 *);
extern word32 __lsub(register word32 *, register word32 *, register word32 *,
 int32);
extern void __st_perinst_val(union pck_u pckv, int32, register word32 *,
 register word32 *);

extern void __start_monitor(struct st_t *);
extern void __start_fmonitor(struct st_t *);
extern void __alloc_1instdce_prevval(struct dcevnt_t *);
extern void __init_1instdce_prevval(struct dcevnt_t *);
extern int32 __get_dcewid(struct dcevnt_t *, struct net_t *);
extern int32 __get_pcku_chars(int32, int32);
extern void __exec_strobes(void);
extern void __exec_fmonits(void);
extern void __exec_monit(struct dceauxlst_t *, int32);
extern void __ld_wire_sect(word32 *, word32 *, struct net_t *, register int32,
 register int32);
extern void __grow_tevtab(void);
extern void __grow_xstk(void);
extern void __chg_xstk_width(struct xstk_t *, int32);
extern void __alloc_xsval(struct xstk_t *, int32);
extern void __ld_wire_val(register word32 *, register word32 *,
 struct net_t *);
extern void __ld_perinst_val(register word32 *, register word32 *,
 union pck_u, int32);
extern int32 __comp_ndx(register struct net_t *, register struct expr_t *);
extern void __ld_bit(register word32 *, register word32 *,
 register struct net_t *, int32);
extern void __ld_arr_val(register word32 *, register word32 *, union pck_u,
 int32, int32, int32);
extern void __ld_psel(register word32 *, register word32 *,
 register struct net_t *, int32, int32);
extern void __rhspsel(register word32 *, register word32 *, register int32,
 register int32);
extern void __exec_readmem(struct expr_t *, int32);
extern void __exec_sreadmem(struct expr_t *, int32);
extern void __exec_sfrand(struct expr_t *);
extern void __ld_addr(word32 **, word32 **, register struct net_t *);
extern void __luminus(word32 *, word32 *, int32);
extern int32 __is_lnegative(word32 *, int32); 
extern word32 __cp_lnegate(word32 *, register word32 *, int32);
extern word32 __inplace_lnegate(register word32 *, int32);
extern void __lunredand(int32 *, int32 *, word32 *, word32 *, int32);
extern void __lunredor(int32 *, int32 *, word32 *, word32 *, int32);
extern void __lunredxor(int32 *, int32 *, word32 *, word32 *, int32);
extern void __mwrshift(word32 *, word32, int32);
extern void __arith_mwrshift(word32 *, word32, int32);
extern void __mwlshift(word32 *, word32, int32);
extern int32 __cvt_lngbool(word32 *, word32 *, int32); 
extern int32 __do_widecmp(int32 *, register word32 *, register word32 *,
 register word32 *, register word32 *, int32);
extern int32 __do_sign_widecmp(int32 *, register word32 *, register word32 *,
 register word32 *, register word32 *, int32);
extern int32 __do_xzwidecmp(register word32 *, register word32 *,
 register word32 *, register word32 *, int32);
extern void __ladd(word32 *, word32 *, word32 *, int32);
extern void __lmult(register word32 *, register word32 *, register word32 *,
 int32);
extern void __sgn_lmult(register word32 *, register word32 *,
 register word32 *, int32);
extern void __ldivmod(word32 *, word32 *, word32 *, int32, int32);
extern void __sgn_ldivmod(register word32 *, register word32 *,
 register word32 *, int32, int32);
extern void __ldivmod2(word32 *, word32 *, word32 *, word32 *, int32);
extern void __by16_ldivmod(word32 *, word32 *, word32 *, word32, int32);

extern void __fio_do_disp(register struct expr_t *, int32, int32, char *);
extern void __do_disp(register struct expr_t *, int32);
extern void __get_cor_range(register int32, union intptr_u, register int32 *,
 register int32 *);
extern int32 __get_arrwide(struct net_t *);
extern void __exec_sysfunc(register struct expr_t *);
extern void __exec_func(register struct expr_t *);
extern void __lhsbsel(register word32 *, register int32, word32);
extern void __cp_sofs_wval(register word32 *, register word32 *,
 register int32, register int32);
extern void __getarr_range(struct net_t *, int32 *, int32 *, int32 *);
extern void __my_fclose(FILE *);
extern void __to_dhboval(int32, int32);
extern void __sizchgxs(register struct xstk_t *, int32);
extern void __sgn_xtnd_widen(struct xstk_t *, int32);
extern void __sgn_xtnd_wrd(struct xstk_t *, int32);
extern void __sizchg_widen(register struct xstk_t *, int32);
extern void __narrow_sizchg(register struct xstk_t *, int32);
extern void __narrow_to1bit(register struct xstk_t *);

extern int32 __wide_vval_is0(register word32 *, int32);
extern int32 __trim1_0val(word32 *, int32);
extern void __lhspsel(register word32 *, register int32, register word32 *,
 register int32);
extern int32 __vval_is1(register word32 *, int32);
extern int32 __get_eval_word(struct expr_t *, word32 *);
extern void __exec2_proc_assign(struct expr_t *, register word32 *,
 register word32 *);
extern void __rhs_concat(struct expr_t *);
extern void __eval_qcol(register struct expr_t *);
extern void __eval_realrealqcol(register struct expr_t *);
extern void __eval_realregqcol(register struct expr_t *);
extern void __eval_regrealqcol(register struct expr_t *);
extern void __lunbitnot(word32 *, word32 *, int32);
extern int32 __set_binxresult(word32 *, word32 *, word32 *, word32 *, int32);
extern void __lbitand(word32 *, word32 *, word32 *, word32 *, int32);
extern void __lbitor(word32 *, word32 *, word32 *, word32 *, int32);
extern void __lbitxor(word32 *, word32 *, word32 *, word32 *, int32);
extern void __lbitxnor(word32 *, word32 *, word32 *, word32 *, int32);
extern void __push_wrkitstk(struct mod_t *, int32);
extern void __pop_wrkitstk(void);
extern int32 __omitxz_widenoteq(register word32 *, register word32 *,
 register word32 *, register word32 *, int32);
extern void __dcelst_on(struct dceauxlst_t *);
extern void __dcelst_off(struct dceauxlst_t *);
extern void init_dcelst(struct dceauxlst_t *); 
extern void __dce_turn_chg_store_on(struct mod_t *, struct dcevnt_t *, int32);  
extern void __do_rm_reading(FILE *, int32, struct net_t *, int32, int32,
 int32, int32, int32);
extern void __wakeup_delay_ctrls(register struct net_t *, register int32,
 register int32);
extern void __add_select_nchglst_el(register struct net_t *, register int32,
 register int32);
extern void __add_dmpv_chglst_el(struct net_t *);
extern void __st_arr_val(union pck_u, int32, int32, int32, register word32 *,
 register word32 *);
extern void __chg_st_arr_val(union pck_u, int32, int32, int32,
 register word32 *, register word32 *);

extern void __cvsim_msg(char *, ...);
/* SJM - not used -extern void __pv_err(int32, char *, ...); */
extern void __pv_warn(int32, char *,...);
extern void __sgfwarn(int32, char *, ...);
extern void __pv_fwarn(int32, char *, ...);
extern void __sgferr(int32, char *, ...);
extern void __dbg_msg(char *, ...);
extern void __sgfinform(int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);
extern void __inform(int32, char *, ...);

extern word32 __masktab[];
extern char __pv_ctab[];

/*
 * MONITOR/STROBE SETUP AND EXECUTION ROUTINES
 */

/*
 * set up a new monitor argument list
 * every expression in $monitor has event trigger added
 *
 * SJM 06/20/02 - change so this exec routine assumes dce's built if in
 * source - only builds if from interactive
 * this runs with itree context set
 */
extern void __start_monitor(struct st_t *stp)
{
 register struct expr_t *alxp;
 int32 argi;
 byte *argisvtab;
 struct tskcall_t *tkcp;
 struct mod_t *imdp;
 struct monaux_t *mauxp;

 tkcp = &(stp->st.stkc);

 /* turn off all monitor events associated with previous monitor (1 inst.) */
 /* LOOKATME - possible minor memory leak here */
 if (__monit_dcehdr != NULL)
  {
   __dcelst_off(__monit_dcehdr);
   __monit_dcehdr = NULL;
  }

 /* turn off monitor by $monitor() - empty arg list */ 
 if (tkcp->targs == NULL)
  { __monit_stp = NULL; __monit_itp = NULL; return; }

 /* DBG remove -- */
 if (tkcp->tkcaux.mauxp == NULL || tkcp->tkcaux.mauxp->argisvtab == NULL)
  __arg_terr(__FILE__, __LINE__);
 /* --- */
 mauxp = tkcp->tkcaux.mauxp;
 argisvtab = (byte *) mauxp->argisvtab;

 __monit_stp = stp;
 /* if $monitor in multiply instantiated module, last one executed is */
 /* current instance */
 __monit_itp = __inst_ptr;
 __cur_fmon = NULL;
 
 if (!mauxp->dces_blt)
  {
   register int32 ii;

   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    {
     __push_itstk(__inst_mod->moditps[ii]);

     __monit_dcehdr = NULL;
     alxp = tkcp->targs;
     for (argi = 0; alxp != NULL; alxp = alxp->ru.x, argi++)
      {
       prep_bld_monit_dces(alxp->lu.x, (int32) argisvtab[argi]);
      }
     mauxp->mon_dcehdr[ii] = __monit_dcehdr; 

     __pop_itstk();
    }
   mauxp->dces_blt = TRUE;
  }
 __monit_dcehdr = mauxp->mon_dcehdr[__inum];
 /* turn on (enable) all dces in list built during prep - off when built */ 
 __dcelst_on(__monit_dcehdr);

 /* SJM 01/02/03 - must re-initialize monit dces previous value if present */
 init_dcelst(__monit_dcehdr);

 /* changing $monitor (including 1st) always trigger 1 output */
 /* no warning since normal to turn off monitoring with $monitor() */
 __slotend_action |= SE_MONIT_CHG;
 imdp = __inst_mod;
 if (imdp->flatinum > 1)
  __sgfinform(441,
   "$monitor invoked in module %s that is instantiated multiple times",
   imdp->msym->synam);
}

/*
 * initialize dce list for monitor form that is always one inst
 */
extern void init_dcelst(struct dceauxlst_t *dcehdr) 
{
 register struct dceauxlst_t *dclp;

 for (dclp = dcehdr; dclp != NULL; dclp = dclp->dclnxt)
  {
   /* notice if no previous value, this detects it and does nothing */ 
   if (dclp->ldcep->prevval.bp != NULL) __init_1instdce_prevval(dclp->ldcep);
  }
}

/* 
 * enable all dces in dceaux list
 */
extern void __dcelst_on(struct dceauxlst_t *dcehdr)
{
 register struct dceauxlst_t *dclp;
 register struct net_t *np;

 for (dclp = dcehdr; dclp != NULL; dclp = dclp->dclnxt)
  {
   dclp->ldcep->dce_off = FALSE; 
   np = dclp->ldcep->dce_np;
   /* SJM 07/19/02 - turn on so now records chges - if nld nil won't record */
   /* until first time here */
   if (np->ntyp >= NONWIRE_ST) np->nchg_has_dces = TRUE;

   /* SJM 11/25/02 - never turn in src dces on/off */
   /* DBG remove -- */
   if (dclp->ldcep->dce_typ == DCE_INST
    || dclp->ldcep->dce_typ == DCE_RNG_INST) __misc_terr(__FILE__, __LINE__); 
   /* --- */
   /* DBG remove -- */
   if (!dclp->ldcep->dce_1inst) __misc_terr(__FILE__, __LINE__);
   /* --- */ 
   
   /* since adding dce, if wire and no lds, must turn off all chged */ 
   /* that prevents recording since now must record for wire dce wakeup */
   /* SJM 11/25/02 - since only 1 inst match forms can be turned on/off */
   /* just turn on the match itp here */
   if (np->nlds == NULL && np->ntyp < NONWIRE_ST) 
    {
     np->nchgaction[dclp->ldcep->dce_matchitp->itinum] &= ~(NCHG_ALL_CHGED);
    }
  }
}

/* 
 * disable all dces in dceaux list
 *
 * notice not stopping recording of changes since expect to be activated again
 */
extern void __dcelst_off(struct dceauxlst_t *dcehdr)
{
 register struct dceauxlst_t *dclp;

 for (dclp = dcehdr; dclp != NULL; dclp = dclp->dclnxt)
  {
   dclp->ldcep->dce_off = TRUE; 
  }
}

/*
 * set up a new fmonitor argument list
 *
 * every expression in $fmonitor has event trigger added
 * this runs with itree context set
 */
extern void __start_fmonitor(struct st_t *stp)
{
 register struct expr_t *alxp;
 register struct dceauxlst_t *dclp;
 int32 argi;
 byte *argisvtab;
 struct tskcall_t *tkcp;
 struct fmonlst_t *fmonp;
 struct fmselst_t *fmsep;
 struct dceauxlst_t *sav_dclp;
 struct dcevnt_t *dcep;
 struct monaux_t *mauxp;

 tkcp = &(stp->st.stkc);
 /* ignore first mc channel descripter since not involved in monitoring */
 if (tkcp->targs != NULL) alxp = tkcp->targs->ru.x; else alxp = NULL;

 /* $fmonitor with no argument list does nothing - ignore with warn */
 if (alxp == NULL)
  {
   __sgfwarn(639,
    "execution of $fmonitor with one argument has no effect - ignored"); 
   return;
  } 

 /* DBG remove -- */
 if (tkcp->tkcaux.mauxp == NULL || tkcp->tkcaux.mauxp->argisvtab == NULL)
  __arg_terr(__FILE__, __LINE__);
 /* --- */
 mauxp = tkcp->tkcaux.mauxp;
 argisvtab = (byte *) mauxp->argisvtab;
 
 /* allocate and link on fmonitor record for this fmonitor */
 fmonp = (struct fmonlst_t *) __my_malloc(sizeof(struct fmonlst_t));
 fmonp->fmon_stp = stp;
 fmonp->fmon_itp = __inst_ptr;
 fmonp->fmonnxt = NULL;
 if (__fmon_hdr == NULL) __fmon_hdr = fmonp; else __fmon_end->fmonnxt = fmonp; 
 __fmon_end = fmonp; 
 __cur_fmon = fmonp;

 /* SJM 06/20/02 - new algorithm only build if call from interactive mode */
 /* works because if any vm insn gen, no interactive */
 if (!mauxp->dces_blt)
  {
   register int32 ii;

   /* save $monit dce list */
   sav_dclp = __monit_dcehdr; 
   __monit_dcehdr = NULL; 

   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    {
     /* build the dces - notice build now starts each monit dce off*/
     __monit_dcehdr = NULL;
     for (argi = 1; alxp != NULL; alxp = alxp->ru.x, argi++)
       prep_bld_monit_dces(alxp->lu.x, (int32) argisvtab[argi]);

     /* SJM 08/26/02 - need to indicate monit is fmonit */ 
     for (dclp = __monit_dcehdr; dclp != NULL; dclp = dclp->dclnxt)
      {
       dcep = dclp->ldcep;
       dcep->is_fmon = TRUE;

      }

     mauxp->mon_dcehdr[ii] = __monit_dcehdr; 
    }
   mauxp->dces_blt = TRUE;
   __monit_dcehdr = sav_dclp;
  }
 fmonp->fmon_dcehdr = mauxp->mon_dcehdr[__inum];

 /* turn on - for fmonitor nothing to turn off */
 __dcelst_on(mauxp->mon_dcehdr[__inum]);

 /* SJM 01/02/03 - must re-initialize fmonit dces previous value if presetn */
 init_dcelst(mauxp->mon_dcehdr[__inum]); 

 dclp = mauxp->mon_dcehdr[__inum];
 for (; dclp != NULL; dclp = dclp->dclnxt)
  {
   dclp->ldcep->dceu2.dce_fmon = __cur_fmon;
  }

 /* add to triggered this time list since always write first time seen */ 
 if (__fmse_freelst == NULL) 
  fmsep = (struct fmselst_t *) __my_malloc(sizeof(struct fmselst_t)); 
 else { fmsep = __fmse_freelst; __fmse_freelst = __fmse_freelst->fmsenxt; } 
 fmsep->fmsenxt = NULL;

 fmsep->fmon = fmonp; 
 fmonp->fmse_trig = fmsep; 
 fmonp->fmon_forcewrite = TRUE;
 if (__fmonse_hdr == NULL) __fmonse_hdr = fmsep;
 else __fmonse_end->fmsenxt = fmsep; 
 __fmonse_end = fmsep;
 /* changing $fmonitor (including 1st) always trigger 1 output */
 __slotend_action |= SE_FMONIT_TRIGGER;
}

/*
 * build the dces for each monit/fmonit in source once 
 * then assign list when enable and turn off list when replaced
 *
 * SJM 06/21/02 - new algorithm build dce list for all in src (f)monit
 * during prep and activate/deactive from execution
 */
extern void __prep_insrc_monit(struct st_t *stp, int32 fmon_type)
{
 register int32 ii;
 register struct expr_t *alxp;
 register struct dceauxlst_t *dclp;
 int32 argi;
 byte *argisvtab;
 struct tskcall_t *tkcp;
 struct monaux_t *mauxp;
 char s1[RECLEN];

 tkcp = &(stp->st.stkc);
 /* ignore first mc channel descripter since not involved in monitoring */
 /* ignore first mc channel descripter since not involved in monitoring */
 /* AIV 06/25/05 - must check if fmonit if not first arg is not decriptor */
 /* was skipping first arg to $monitor */ 
 if (fmon_type) 
  {
   if (tkcp->targs != NULL) alxp = tkcp->targs->ru.x; else alxp = NULL;
  }
 else
  {
   if (tkcp->targs != NULL) alxp = tkcp->targs->lu.x; else alxp = NULL;
  }
 
 if (fmon_type) strcpy(s1, "$fmonitor"); else strcpy(s1, "$monitor");

 /* $monitor/$fmonitor with no args - does nothing - ignore with warn */
 if (alxp == NULL)
  {
   /* SJM - 05/14/04 - monitor with no args turns off monitor - no warn */
   return;
  } 
 /* DBG remove -- */
 if (tkcp->tkcaux.mauxp == NULL || tkcp->tkcaux.mauxp->argisvtab == NULL)
  __arg_terr(__FILE__, __LINE__);
 /* --- */
 mauxp = tkcp->tkcaux.mauxp;
 argisvtab = (byte *) mauxp->argisvtab;

 for (ii = 0; ii < __inst_mod->flatinum; ii++)
  {
   __push_itstk(__inst_mod->moditps[ii]);

   __monit_dcehdr = NULL;
   argi = 0;
   for (alxp = tkcp->targs; alxp != NULL; alxp = alxp->ru.x, argi++)
    {
     prep_bld_monit_dces(alxp->lu.x, (int32) argisvtab[argi]);
    }
   if (fmon_type)
    {
     /* SJM 08/26/02 - need to indicate monit is fmonit */ 
     for (dclp = __monit_dcehdr; dclp != NULL; dclp = dclp->dclnxt)
      {
       dclp->ldcep->is_fmon = TRUE;
      }
    }
   mauxp->mon_dcehdr[ii] = __monit_dcehdr; 

   __pop_itstk(); 
  } 
 mauxp->dces_blt = TRUE;
 __monit_dcehdr = NULL;
}

/*
 * during design preparation build monitor dces for every possible
 * instance (also non prep version for when monit added from iact mode)
 */
static void prep_bld_monit_dces(struct expr_t *xp, int32 argisvfmt)  
{
 struct net_t *np;
 int32 biti, bitj;
 word32 *wp;
 struct expr_t *idndp, *ndx;
 struct expr_t *fax;
 struct itree_t *ref_itp;
 
 ref_itp = __inst_ptr;
 switch ((byte) xp->optyp) {
  case GLBREF:
   idndp = xp; 
   /* for global - do not need ref. point - just link on 1 (because only 1 */
   /* monit call from 1 inst.) target wire */
   biti = bitj = -1;
glb_dce:
   __xmrpush_refgrp_to_targ(idndp->ru.grp);
   np = idndp->lu.sy->el.enp;
   linkon_monit_dce(np, biti, bitj, argisvfmt, ref_itp);
   __pop_itstk();
   break;
  case ID:
   idndp = xp; 
   np = xp->lu.sy->el.enp;
   linkon_monit_dce(np, -1, -1, argisvfmt, ref_itp);
   break;
  /* SJM 05/18/00 - must do nothing for reals */ 
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM: case OPEMPTY:
   return;
  case LSB:
   idndp = xp->lu.x; 
   np = idndp->lu.sy->el.enp;
   ndx = xp->ru.x;
   /* for monits, any reg or non scalared wire must trigger on any chg */
   if (ndx->optyp == NUMBER)
    {
     wp = &(__contab[ndx->ru.xvi]);
     if (wp[1] != 0L) biti = -1; else biti = (int32) wp[0];
    }
   else if (ndx->optyp == ISNUMBER)
    {
     wp = &(__contab[ndx->ru.xvi]);
     wp = &(wp[2*__inum]);
     /* need length for IS number because can be wider - but get low */
     if (wp[1] != 0L) biti = -1; else biti = (int32) wp[0];
    }
   else
    {
     /* notice for monitor and dctrl event change, variable here is legal */
     /* and implies change for index and trigger on all bits of variable */
     prep_bld_monit_dces(ndx, argisvfmt);
     biti = -1;
    }
   bitj = biti;
   if (biti != -1 && !np->vec_scalared) biti = bitj = -1;
   if (idndp->optyp == GLBREF) goto glb_dce;
   linkon_monit_dce(np, biti, biti, argisvfmt, ref_itp);
   break;
  case PARTSEL:
   idndp = xp->lu.x; 
   np = idndp->lu.sy->el.enp;
   ndx = xp->ru.x;
   /* know part select never IS */
   wp = &(__contab[ndx->lu.x->ru.xvi]);
   biti = (int32) wp[0];
   wp = &(__contab[ndx->ru.x->ru.xvi]);
   bitj = (int32) wp[0];

   if (!np->vec_scalared) biti = bitj = -1;
   if (idndp->optyp == GLBREF) goto glb_dce;
   linkon_monit_dce(np, biti, bitj, argisvfmt, ref_itp);
   break;
  case FCALL:
   /* if any arguments of system or user functions change, monitor triggers */
   /* notice $time function do not have arguments */
   for (fax = xp->ru.x; fax != NULL; fax = fax->ru.x)
    {
     prep_bld_monit_dces(fax->lu.x, argisvfmt);
    }
   break;
  case LCB:
   for (fax = xp->ru.x; fax != NULL; fax = fax->ru.x)
    {
     prep_bld_monit_dces(fax->lu.x, argisvfmt);
    }
   break;
  default:
   if (xp->lu.x != NULL) prep_bld_monit_dces(xp->lu.x, argisvfmt);
   if (xp->ru.x != NULL) prep_bld_monit_dces(xp->ru.x, argisvfmt);
   break;
 } 
}

/*
 * link on a special (simplified) monitor dce
 * IS form never possible here and always local, have moved to dest.
 *
 * if xmr know np is already dest. and itree place is dest.
 *
 * monitor dce must go on dest. since when value changed that wire
 * is traced, final eval. is separate unrelated code
 * this is never dce_itp since just put on one right xmr targ.
 * and moved to xmr target above
 * goes on front but after andy dmpvar dces
 * also for fmonitor - different end of slot list
 *
 * SJM 01/06/03 - only callable during prep since all monit dces in src
 * build here or from interactive mode
 */
static void linkon_monit_dce(struct net_t *np, int32 biti, int32 bitj,
 int32 argisvfmt, struct itree_t *ref_itp)
{
 struct dcevnt_t *dcep;
 struct dceauxlst_t *dclp;

 /* allocate, init, and fill the fields */
 dcep = __alloc_dcevnt(np);
 if (biti == -1) dcep->dce_typ = DCE_MONIT;
 else
  {
   dcep->dce_typ = DCE_RNG_MONIT;
   dcep->dci1 = biti;
   dcep->dci2.i = bitj;
  }
 /* dce's assume on but here only turned on when activated by exec */
 dcep->dce_off = TRUE;

 /* non v format strengths for monitor only output if value part changes */ 
 if (np->n_stren && argisvfmt) dcep->dce_nomonstren = FALSE;

 /* this is instance where dce trigger must occur for xmr different from */
 /* itree location in which (f)monitor execed */
 dcep->dce_matchitp = __inst_ptr;
 dcep->dce_refitp = ref_itp;
 dcep->dce_1inst = TRUE;
 dcep->dceu2.dce_fmon = __cur_fmon;
 
 /* link this on front */
 dcep->dcenxt = np->dcelst;
 np->dcelst = dcep;
 
 /* allocate the prev val fields and set to cur. value of wire for inst. */
 /* for monits always need prev. val. field because set flag if any */
 /* change and change may get filtered out later */
 /* except for entire reg, do not need */
 __alloc_1instdce_prevval(dcep);
 /* SJM 05/04/05 - can initialize since for cver-cc, not linked to .bss yet */

 /* then link on undo/chg list - fmon's never undone except for :reset */
 dclp = (struct dceauxlst_t *) __my_malloc(sizeof(struct dceauxlst_t));
 dclp->ldcep = dcep; 
 dclp->dclnxt = __monit_dcehdr;
 __monit_dcehdr = dclp;

 if (__iact_state)
  {
   /* since no dce, no loads, and no dmpvars must always turn chg store on */
   if (!dcep->dce_np->nchg_nd_chgstore)
    {
     /* this also causes regen of all mod entire procedural iops since net */
     /* need to be compiled with change form on everywhere */
     __dce_turn_chg_store_on(__inst_mod, dcep, TRUE);  
    }
   /* SJM 02/06/03 - may have npps but not dces so must turn this on */
   /* since nchg nd chgstore on, know nchg action right */
   if (np->ntyp >= NONWIRE_ST) np->nchg_has_dces = TRUE;
  } 

 /* -- DBG REMOVE
 {
  struct dceauxlst_t *dclp2, *dclp3; 

  for (dclp2 = __monit_dcehdr; dclp2 != NULL; dclp2 = dclp2->dclnxt)
   {
    for (dclp3 = dclp2->dclnxt; dclp3 != NULL; dclp3 = dclp3->dclnxt)
     if (dclp2 == dclp3)
      {
       __dbg_msg("^^^ monitor dclp duplicate addr %lx\n", dclp2);
       __misc_terr(__FILE__, __LINE__);
      }
   }
 }
 --- */
}

/*
 * get width in bits of a dcep range or wire
 */
extern int32 __get_dcewid(struct dcevnt_t *dcep, struct net_t *np)
{
 if (dcep->dci1 == -2) return(1);
 if (dcep->dci1 == -1) return(np->nwid);
 return(dcep->dci1 - dcep->dci2.i + 1);  
}

/*
 * get number of packed bytes for ld peri and st peri access 
 */
extern int32 __get_pcku_chars(int32 blen, int32 insts)
{
 /* SJM 10/14/99 - now storing all scalars as one byte */
 if (blen == 1) return(insts);
 /* SJM 07/15/00 - no longer packing 2 to 16 bits */
 return(2*insts*wlen_(blen)*WRDBYTES);
}

/*
 * execute the strobe statements at end of time slot
 * only called if at least one and by end of time slot functionality
 * of $display
 *
 * need fstrobes here too
 */
extern void __exec_strobes(void)
{
 register struct strblst_t *strblp;
 int32 base, sav_slin_cnt, sav_sfnam_ind;
 struct st_t *stp;
 struct tskcall_t *tkcp;
 struct expr_t *tkxp;
 struct systsk_t *stbp;

 sav_slin_cnt = __slin_cnt;
 sav_sfnam_ind = __sfnam_ind;
 for (strblp = __strobe_hdr; strblp != NULL; strblp = strblp->strbnxt)
  {
   stp = strblp->strbstp;
   __slin_cnt = stp->stlin_cnt;
   __sfnam_ind = stp->stfnam_ind;

   /* notice here cur. itp does not need to be preserved */
   __push_itstk(strblp->strb_itp);
   tkcp = &(stp->st.stkc);
   tkxp = tkcp->tsksyx;
   stbp = tkxp->lu.sy->el.esytbp;

   switch (stbp->stsknum) {
    case STN_STROBE: base = BDEC; goto nonf_write;
    case STN_STROBEH: base = BHEX; goto nonf_write;
    case STN_STROBEB: base = BBIN; goto nonf_write;
    case STN_STROBEO: base = BOCT;
nonf_write:
     __do_disp(tkcp->targs, base);
     __cvsim_msg("\n");
     break;
    case STN_FSTROBE: base = BDEC; goto f_disp;
    case STN_FSTROBEB: base = BBIN; goto f_disp;
    case STN_FSTROBEH: base = BHEX; goto f_disp;
    case STN_FSTROBEO:
     base = BOCT;
f_disp:
     __fio_do_disp(tkcp->targs, base, TRUE, tkxp->lu.sy->synam);
     break;
    default: __case_terr(__FILE__, __LINE__);
   }
   stp->strb_seen_now = FALSE;
   __pop_itstk();
  }
 /* free strobes all at once */
 if (__strobe_hdr != NULL)
  { __strobe_end->strbnxt = __strb_freelst; __strb_freelst = __strobe_hdr; }
 __strobe_hdr = __strobe_end = NULL; 
 __slin_cnt = sav_slin_cnt;
 __sfnam_ind = sav_sfnam_ind;
}

/*
 * execute all triggered during this time slot fmonitors
 * LOOKATME - for now $monitoroff (on) does not effect fmonitor and iact -[num]
 * also impossible
 */
extern void __exec_fmonits(void)
{
 register struct fmselst_t *fmsep;
 struct fmonlst_t *fmonp;
 struct st_t *sav_monit_stp;
 struct itree_t *sav_monit_itp;

 if (__fmonse_hdr == NULL) __arg_terr(__FILE__, __LINE__);
 sav_monit_stp = __monit_stp;
 sav_monit_itp = __monit_itp;
 for (fmsep = __fmonse_hdr; fmsep != NULL; fmsep = fmsep->fmsenxt)
  {
   fmonp = fmsep->fmon;
   __monit_stp = fmonp->fmon_stp; 
   __monit_itp = fmonp->fmon_itp; 
   __exec_monit(fmonp->fmon_dcehdr, (int32) (fmonp->fmon_forcewrite == 1));
   /* turn off triggered since this time slot end change processed */
   fmonp->fmse_trig = NULL;
   fmonp->fmon_forcewrite = FALSE;
   /* DBG remove --
   if (__debug_flg)
    {
     __dbg_msg("+++ time %s: executing fmonitor at %s\n", __to_timstr(__xs,
      &__simtime), __bld_lineloc(__xs2, __monit_stp->stfnam_ind,
      __monit_stp->stlin_cnt));
    }
   --- */
  } 
 __monit_stp = sav_monit_stp;
 __monit_itp = sav_monit_itp;
 /* add entire list to se free list */
 __fmonse_end->fmsenxt = __fmse_freelst;
 __fmse_freelst = __fmonse_hdr; 
 __fmonse_hdr = __fmonse_end = NULL;
}

/*
 * execute the monitor statements at end of time slot
 * only called if at least one and like dispay not write
 * fmonp nil implies it is $monitor
 */
extern void __exec_monit(struct dceauxlst_t *monit_hd, int32 force_write)
{
 int32 base, sav_slin_cnt, sav_sfnam_ind;
 struct tskcall_t *tkcp;
 struct expr_t *tkxp;
 struct systsk_t *stbp;

 /* if execed "$monitor;" no monitoring but force write will be on */
 if (__monit_stp == NULL) return;

 /* first make sure at least one changed (or just starting so must write) */
 /* and update all dce values (not per inst.) to current value if chged */ 
 if (!chk_monits_chged(monit_hd) && !force_write) return;

 sav_slin_cnt = __slin_cnt;
 sav_sfnam_ind = __sfnam_ind;
 __slin_cnt = __monit_stp->stlin_cnt;
 __sfnam_ind = __monit_stp->stfnam_ind;

 tkcp = &(__monit_stp->st.stkc);
 tkxp = tkcp->tsksyx;
 stbp = tkxp->lu.sy->el.esytbp;
 /* current instance does not need to be preserved here */
 __push_itstk(__monit_itp);

 switch (stbp->stsknum) {
  case STN_MONITOR: base = BDEC; goto nonf_write;
  case STN_MONITORH: base = BHEX; goto nonf_write;
  case STN_MONITORB: base = BBIN; goto nonf_write;
  case STN_MONITORO: base = BOCT;
nonf_write:
   __do_disp(tkcp->targs, base);
   __cvsim_msg("\n");
   break;
  case STN_FMONITOR: base = BDEC; goto f_disp;
  case STN_FMONITORB: base = BBIN; goto f_disp;
  case STN_FMONITORH: base = BHEX; goto f_disp;
  case STN_FMONITORO: base = BOCT;
f_disp:
   __fio_do_disp(tkcp->targs, base, TRUE, tkxp->lu.sy->synam);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 __pop_itstk();
 __slin_cnt = sav_slin_cnt;
 __sfnam_ind = sav_sfnam_ind;
}

/*
 * routine to go through entire monit dce list and see if any really changed
 * if really changed, store new value 
 *
 * know itree loc set
 */
static int32 chk_monits_chged(register struct dceauxlst_t *dclp)
{
 register struct xstk_t *xsp, *xsp2; 
 byte *sbp, *sbp2;
 int32 i1, i2, i, chged, dcewid;
 struct dcevnt_t *dcep;
 struct net_t *np; 

 chged = FALSE;
 for (; dclp != NULL; dclp = dclp->dclnxt)
  {
   dcep = dclp->ldcep;

   /* for array index or entire non wire (not constant range), no change */
   /* previous value needed, if in list changed */ 
   /* but notice must go thru entire list to update changed values */
   if (dcep->prevval.wp == NULL) { chged = TRUE; continue; }

   np = dcep->dce_np;
   dcewid = __get_dcewid(dcep, np);
   __push_itstk(dcep->dce_matchitp);
   __get_cor_range(dcep->dci1, dcep->dci2, &i1, &i2);
   if (np->n_stren)
    {
     /* for monit know exactly one prevval */
     sbp = dcep->prevval.bp;
     /* this uses "real" iti num */
     get_stwire_addr_(sbp2, np);
     if (i1 != -1) sbp2 = &(sbp2[i2]);

     if (dcep->dce_nomonstren)
      {
       for (i = 0; i < dcewid; i++, sbp++, sbp2++) 
        {
         if (*sbp != *sbp2)
          {
           /* here only changed if value (low 2 bits) differ */
           if ((*sbp & 3) != (*sbp2 & 3)) chged = TRUE; 
           /* always copy even if values the same */
           *sbp = *sbp2;
          }
        }
      }
     else
      {
       if (memcmp(sbp, sbp2, dcewid) != 0)
        { chged = TRUE; memcpy(sbp, sbp2, dcewid); }
      }
    }
   else
    {
     push_xstk_(xsp, dcewid);

     /* this need to access 0th instance since one inst form */
     __push_wrkitstk(__inst_mod, 0);
     __ld_perinst_val(xsp->ap, xsp->bp, dcep->prevval, dcewid);
     __pop_wrkitstk();

     /* now must load from correct inst and store into one inst. */
     push_xstk_(xsp2, dcewid);
     __ld_wire_sect(xsp2->ap, xsp2->bp, np, i1, i2);
     if (cmp_vval_(xsp->ap, xsp2->ap, dcewid) != 0 || 
      cmp_vval_(xsp->bp, xsp2->bp, dcewid) != 0)
      {
       chged = TRUE;
   
       /* know tmpitp unchanged since load */
       __push_wrkitstk(__inst_mod, 0);
       __st_perinst_val(dcep->prevval, dcewid, xsp2->ap, xsp2->bp);
       __pop_wrkitstk();
      }
     __pop_xstk();
     __pop_xstk();
    }
   __pop_itstk();
  }
 return(chged);
}

/*
 * load section of wirereg for monitor and dce change determination
 * know itree location correct and i1 and i2 set (-2 IS form fixed by here)
 * if bit select of array load array cell
 * this is for non strength wire case
 */ 
extern void __ld_wire_sect(word32 *ap, word32 *bp, struct net_t *np,
 register int32 i1, register int32 i2)
{
 int32 arrwid;

 if (i1 == -1) { __ld_wire_val(ap, bp, np); return; }
 if (i1 == i2)
  {
   if (!np->n_isarr) __ld_bit(ap, bp, np, i1);
   else
    {
     arrwid = __get_arrwide(np);
     __ld_arr_val(ap, bp, np->nva, arrwid, np->nwid, i1);
    }
   return;
  }
 __ld_psel(ap, bp, np, i1, i2);
}

/*
 * ROUTINES TO EVALUATE RHS EXPRESSIONS
 */

/*
 * wrapper for eval_xpr that sets correct exec source location
 */
extern struct xstk_t *__src_rd_eval_xpr(struct expr_t *ndp)
{
 struct xstk_t *xsp;
 int32 sav_slin_cnt, sav_sfnam_ind;

 /* can assign specparam value immediately */
 /* SJM 06/17/99 - needs to run in run/fixup mode - statement loc set */
 sav_slin_cnt = __slin_cnt;
 sav_sfnam_ind = __sfnam_ind;
 __sfnam_ind = __cur_fnam_ind;
 __slin_cnt = __lin_cnt;

 xsp = __eval_xpr(ndp);

 /* must put back in case reading iact source */
 __slin_cnt = sav_slin_cnt;
 __sfnam_ind = sav_sfnam_ind;
 return(xsp);
}

/*
 * for debugging special interface to evaluate expression 
 * so stack depth can be checked
 * -- notice where strength needed (even if non stren) use ndst eval xpr
 * this is normally macro see pvmacros.h
 */
/* --- DBG add ---
extern struct xstk_t *__eval_xpr(struct expr_t *ndp)
{
 struct xstk_t *xsp;
  
 xsp = __eval2_xpr(ndp);

 -* ---
 if (__debug_flg)
  {
   if (__xspi != 0 && __fcspi == -1)
    {
     __sgfwarn(526, 
     "INTERNAL - %d extra values on stack after expression evaluation", __xspi);
    }   
  }
 --- *-
 return(xsp);
}
---- */

/*
 * evaluate a rhs expressions
 * pushes and pops temps and leaves result on top of reg stack
 * requires all expr. node widths to be set
 * both for constant folding and execution
 * caller must pop value from stack
 * know a and p parts always contiguous
 *
 * this is used for constant evaluation where already check for only values
 * parameters are converted to numbers by here and spec params are never in
 * expressions
 *
 * LOOKATME - since now have real cont table and real storage table
 *            could get rid of double copying even for interpreter
 */
extern struct xstk_t *__eval2_xpr(register struct expr_t *ndp)
{
 register struct xstk_t *xsp;
 int32 wlen;
 register word32 *wp;
 struct net_t *np;
 struct gref_t *grp;
 struct sy_t *syp;

 /* DBG remove --- 
 if (ndp == NULL) __arg_terr(__FILE__, __LINE__);
 --- */

 /* in this case, must put value on tos */
 switch ((byte) ndp->optyp) {
  case NUMBER:
   push_xstk_(xsp, ndp->szu.xclen);
   if (ndp->szu.xclen <= WBITS)
    {
     xsp->ap[0] = __contab[ndp->ru.xvi];
     xsp->bp[0] = __contab[ndp->ru.xvi + 1];
    }
   else memcpy(xsp->ap, &(__contab[ndp->ru.xvi]),
    2*WRDBYTES*wlen_(ndp->szu.xclen));

   return(xsp); 
  case REALNUM:
   push_xstk_(xsp, ndp->szu.xclen);
   /* know high bits of num value already zeroed */
   memcpy(xsp->ap, &(__contab[ndp->ru.xvi]),
    2*WRDBYTES*wlen_(ndp->szu.xclen));
   return(xsp);
  case OPEMPTY:
   /* this evaluates to one space - for procedural (stask) ,, connections */
   push_xstk_(xsp, 8);
   xsp->bp[0] = 0L;
   xsp->ap[0] = ' ';
   return(xsp);
  case UNCONNPULL:
   /* only for inst. input where down must be marked strength */
   __case_terr(__FILE__, __LINE__);
   break;
  case ISNUMBER:
   push_xstk_(xsp, ndp->szu.xclen);
   wlen = wlen_(ndp->szu.xclen);
   wp = (word32 *) &(__contab[ndp->ru.xvi]);
   memcpy(xsp->ap, &(wp[2*wlen*__inum]), 2*WRDBYTES*wlen);
   return(xsp);
  case ISREALNUM:
   push_xstk_(xsp, ndp->szu.xclen);
   wlen = wlen_(ndp->szu.xclen);
   wp = &(__contab[ndp->ru.xvi + 2*__inum]);
   memcpy(xsp->ap, wp, 2*WRDBYTES*wlen);
   return(xsp);
  case GLBREF:
   /* if local fall through - symbol and cur. itp right */
   grp = ndp->ru.grp;
   __xmrpush_refgrp_to_targ(grp);
   push_xstk_(xsp, ndp->szu.xclen);
   /* know this is global wire or will not get here */
   /* cannot use short circuit xva for globals */
   np = grp->targsyp->el.enp;
   __ld_wire_val(xsp->ap, xsp->bp, np);
   __pop_itstk();
   goto done;
  case ID:
   push_xstk_(xsp, ndp->szu.xclen);
   np = ndp->lu.sy->el.enp;
   __ld_wire_val(xsp->ap, xsp->bp, np);
   goto done;
  case LSB:
   push_bsel(ndp);
   goto done;
  case PARTSEL:
   push_psel(ndp);
   goto done;
  case LCB:
   __rhs_concat(ndp);
   goto done;
  case FCALL:
   syp = ndp->lu.x->lu.sy;
   /* notice these routines in v_ex - result left on top of stack */
   /* as usual caller must free */
   if (syp->sytyp == SYM_SF) __exec_sysfunc(ndp); else __exec_func(ndp);
   /* function return value now on top of stack */
   goto done; 
  case QUEST:
   /* notice that because of side effects, must evaluate in order */
   __eval_qcol(ndp);
   goto done;
  case REALREALQUEST:
   __eval_realrealqcol(ndp);
   goto done;
  case REALREGQUEST:
   __eval_realregqcol(ndp);
   goto done;
  case REGREALQCOL:
   __eval_regrealqcol(ndp);
   /* 1 value now on tos */
   goto done;
  }
 if (ndp->ru.x == NULL) eval_unary(ndp); else eval_binary(ndp);
done:
 return(__xstk[__xspi]);
}

/*
 * routine to grow xstk
 */
extern void __grow_xstk(void)
{
 register int32 i;
 int32 old_maxxnest;
 int32 osize, nsize;

 old_maxxnest = __maxxnest;
 osize = old_maxxnest*sizeof(struct xstk_t *);
 if (__maxxnest >= XNESTFIXINC) __maxxnest += XNESTFIXINC;
 else __maxxnest *= 2;
 nsize = __maxxnest*sizeof(struct xstk_t *);
 __xstk = (struct xstk_t **) __my_realloc((char *) __xstk, osize, nsize);
 /* assume stack hold 1 work case at initialization */
 for (i = old_maxxnest; i < __maxxnest; i++)
  {
   __xstk[i] = (struct xstk_t *) __my_malloc(sizeof(struct xstk_t));
   __alloc_xsval(__xstk[i], 1);
  }
 if (__debug_flg)
  __dbg_msg("+++ expr. stack grew from %d bytes to %d\n", osize, nsize); 
}

/* 
 * routine to widen xstk element if wider than default words - rare 
 */
extern void __chg_xstk_width(struct xstk_t *xsp, int32 wlen)
{
 /* freeing in case of need for very wide expr. */
 __my_free((char *) xsp->ap, 2*WRDBYTES*xsp->xsawlen);
 __alloc_xsval(xsp, wlen);
}

/*
 * allocate stack entry value word32 array
 * only called after need to widen determined 
 * this allocates wide enough stack value - caller must set width and value
 * this always makes a and b parts contiguous
 *
 * only allocate and free here 
 * FIXME - why not use re-alloc?
 */
extern void __alloc_xsval(struct xstk_t *xsp, int32 xstkwlen)
{
 word32 *ap;
 int32 awlen;

 awlen = (xstkwlen < DFLTIOWORDS) ? DFLTIOWORDS : xstkwlen;
 /* notice a and b parts must be allocated once contiguously */
 /* 4 words means 128 bits at 32 bits dependent per word32 */
 ap = (word32 *) __my_malloc(2*WRDBYTES*awlen);
 xsp->ap = ap;
 /* this makes 2 part contiguous */
 xsp->bp = &(ap[xstkwlen]);
 xsp->xsawlen = awlen;
}

/*
 * load (copy) an entire value into possibly separated rgap and rgbp from wp
 * of length blen instance cur. itp loaded with representation srep
 *
 * assumes cur. itp starts at 0
 * cannot be used to access array and removes any strengths (value only)
 * rgap and rgbp assumed to have enough room
 * also for params and specparams
 */
extern void __ld_wire_val(register word32 *rgap, register word32 *rgbp,
 struct net_t *np)
{
 register word32 uwrd, *rap, *wp;
 register int32 wlen;
 struct expr_t *xp;
 struct expr_t **pxparr;
 struct xstk_t *xsp;

 switch ((byte) np->srep) {
  case SR_VEC:
   /* rap is 2*wlen word32 section of vec array that stores cur. inst vec. */
   wlen = wlen_(np->nwid);
   /* DBG remove ---
   if (__inst_ptr == NULL) __arg_terr(__FILE__, __LINE__);
   --- */
   rap = &(np->nva.wp[2*wlen*__inum]);
   memcpy(rgap, rap, WRDBYTES*wlen);
   memcpy(rgbp, &(rap[wlen]), WRDBYTES*wlen);
   return;
  case SR_SVEC:
   __ld_stval(rgap, rgbp, &(np->nva.bp[np->nwid*__inum]), np->nwid);
   return;
  case SR_SCAL:
   ld_scalval_(rgap, rgbp, np->nva.bp);
   return;
  case SR_SSCAL:
   /* notice accessing byte value and assign so endian ok */
   uwrd = (word32) np->nva.bp[__inum];
   rgap[0] = uwrd & 1L;
   rgbp[0] = (uwrd >> 1) & 1L;
   return;

  /* PX representations are left for getting param value at run time */
  /* also for parameter forms never selects */
  case SR_PXPR:
   wlen = wlen_(np->nwid);
   /* this assumes for parameters expr. points to nva expr. field */
   xp = (struct expr_t *) np->nva.wp;
   xsp = __eval2_xpr(xp);
   memcpy(rgap, xsp->ap, WRDBYTES*wlen);
   memcpy(rgbp, xsp->bp, WRDBYTES*wlen);
   __pop_xstk();
   return;
  case SR_PISXPR:
   wlen = wlen_(np->nwid);
   /* this assumes for parameters expr. points to nva expr. field */
   /* caller sets iti num */
   pxparr = (struct expr_t **) np->nva.wp;  
   xsp = __eval2_xpr(pxparr[__inum]);
   /* if real just copy */
   memcpy(rgap, xsp->ap, WRDBYTES*wlen);
   memcpy(rgbp, xsp->bp, WRDBYTES*wlen);
   __pop_xstk();
   return;
  case SR_PNUM:
   wlen = wlen_(np->nwid);
   wp = np->nva.wp;
   memcpy(rgap, wp, WRDBYTES*wlen);
   memcpy(rgbp, &(wp[wlen]), WRDBYTES*wlen);
   return;
  case SR_PISNUM:
   wlen = wlen_(np->nwid);
   wp = &(np->nva.wp[2*wlen*__inum]);
   memcpy(rgap, wp, WRDBYTES*wlen);
   memcpy(rgbp, &(wp[wlen]), WRDBYTES*wlen);
   return;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * load per inst. value - allows packing etc.
 * mostly for continuous assign driver 
 * not for strengths and needs __inst_ptr to be set
 * caller needs to push big enough area pointed to by rgap/rgbp
 *
 * know size change never needed here 
 */
extern void __ld_perinst_val(register word32 *rgap, register word32 *rgbp,
 union pck_u pckv, int32 vblen)
{
 word32 *rap;
 int32 wlen;

 if (vblen == 1) { ld_scalval_(rgap, rgbp, pckv.bp); return; }
 /* SJM - 07/15/00 - all per-inst vecs in at least 2 words */
 wlen = wlen_(vblen);
 rap = &(pckv.wp[2*wlen*__inum]);
 memcpy(rgap, rap, WRDBYTES*wlen);
 memcpy(rgbp, &(rap[wlen]), WRDBYTES*wlen);
}

/*
 * load value part of a strength scalar bytes array into an a/b vector
 * strength bytes (bits) stored low to high just like normal words
 *
 * notice this uses lhs selects into rgap/rgbp so must 0 to start
 * and cannot assume contiguous
 */
extern void __ld_stval(register word32 *rgap, register word32 *rgbp,
 register byte *sbp, int32 blen)
{
 register int32 bi;
 int32 wlen;
 word32 tmpw;
 
 /* zero unused high bits in high word32 only - rest will be selected into */
 wlen = wlen_(blen);
 rgap[wlen - 1] = 0L;
 rgbp[wlen - 1] = 0L;
 for (bi = 0; bi < blen; bi++)
  {
   tmpw = (word32) sbp[bi];
   __lhsbsel(rgap, bi, tmpw & 1L);
   __lhsbsel(rgbp, bi, (tmpw >> 1) & 1L);
  }
}

/*
 * push (access) a selected bit or array locaton on top of reg. stack
 * know width will be 1 if bit or array vector width if array
 *
 * this can be improved by assigning free reg not just pushing
 * for now not separating at compile time (stupid) so separate is done here
 */
static void push_bsel(struct expr_t *ndp)
{
 register struct xstk_t *xsp;
 register struct net_t *np;
 register word32 *rap;
 int32 biti, arrwid, wlen;
 struct expr_t *idndp;
 
 idndp = ndp->lu.x;
 np = idndp->lu.sy->el.enp;

 /* can be either constant or expr. - both handled in comp. */
 biti = __comp_ndx(np, ndp->ru.x);
 push_xstk_(xsp, ndp->szu.xclen);
 if (biti == -1)
  {
   /* notice too many places where cannot emit warning - just change to x/z */
   set_regtox_(xsp->ap, xsp->bp, xsp->xslen);
   return;
  }

 /* notice load routines unwound into both paths */
 if (idndp->optyp != GLBREF)
  {
   /* if (!np->n_isarr) __ld_bit(xsp->ap, xsp->bp, np, biti); */
   if (!np->n_isarr)
    {
     if (np->srep == SR_VEC)
      {
       /* BEWARE - this is vectored rep. short circuit */
       wlen = wlen_(np->nwid);  
       rap = &(np->nva.wp[2*wlen*__inum]);
       xsp->ap[0] = rhsbsel_(rap, biti);
       xsp->bp[0] = rhsbsel_(&(rap[wlen]), biti);
      }
     else __ld_bit(xsp->ap, xsp->bp, np, biti);
    }
   else
    {
     arrwid = __get_arrwide(np);
     __ld_arr_val(xsp->ap, xsp->bp, np->nva, arrwid, np->nwid, biti);
    }
   return;
  }

 __xmrpush_refgrp_to_targ(idndp->ru.grp);
 /* if (!np->n_isarr) __ld_bit(xsp->ap, xsp->bp, np, biti); */
 if (!np->n_isarr)
  {
   if (np->srep == SR_VEC)
    {
     /* BEWARE - this is vectored rep. short circuit */
     wlen = wlen_(np->nwid);  
     rap = &(np->nva.wp[2*wlen*__inum]);
     xsp->ap[0] = rhsbsel_(rap, biti);
     xsp->bp[0] = rhsbsel_(&(rap[wlen]), biti);
    }
   else __ld_bit(xsp->ap, xsp->bp, np, biti);
  }
 else
  {
   arrwid = __get_arrwide(np);
   __ld_arr_val(xsp->ap, xsp->bp, np->nva, arrwid, np->nwid, biti);
  }
 __pop_itstk();
}

/*
 * compute a vector or array var or constant index value
 * returns -1 on x - caller must handle
 * if returns -1, globals __badind_a and __badind_b contain value
 *
 * if net is a array, array index else vector index 
 *
 * just like C implied truncating index to 32 bit value
 * constants are already normalized during compilation
 * reals illegal and caught before here
 */
extern int32 __comp_ndx(register struct net_t *np,
 register struct expr_t *ndx)
{
 register word32 *rap;
 register word32 *wp;
 int32 biti, ri1, ri2, biti2, obwid;
 struct net_t *xnp;
 struct xstk_t *xsp2;

 /* special case 0 - simple unpacked variable */
 /* SJM 05/21/04 - can only short circuit if fits in one word32 */
 if (ndx->optyp == ID && ndx->szu.xclen <= WBITS)
  {
   xnp = ndx->lu.sy->el.enp;
   if (xnp->srep != SR_VEC) goto nd_eval;

   /* BEWARE - this short circuit assumes particular SR_VEC d.s. */
   rap = &(xnp->nva.wp[2*__inum]);
   biti2 = rap[0];
   if (rap[1] == 0L) goto normalize; 
    
   __badind_a = rap[0];
   __badind_b = rap[1];
   __badind_wid = xnp->nwid;
   return(-1);
  }

 /* case 1: constant */
 if (ndx->optyp == NUMBER)  
  {
   wp = &(__contab[ndx->ru.xvi]); 
   if (wp[1] != 0L)
    {
     __badind_a = wp[0];
     __badind_b = wp[1];
     __badind_wid = ndx->szu.xclen;
     return(-1);
    }
   return((int32) wp[0]);
  }
 /* case 2 IS constant */
 /* notice IS NUMBER form must be normalized at compile time */
 if (ndx->optyp == ISNUMBER)
  {
   wp = &(__contab[ndx->ru.xvi]); 
   wp = &(wp[2*__inum]);
   if (wp[1] != 0L)
    {
     __badind_a = wp[0];
     __badind_b = wp[1];
     __badind_wid = ndx->szu.xclen;
     return(-1);
    }
   return((int32) wp[0]);
  }
 /* case 3 expression */
 /* case 3b - other expr. */
nd_eval:
 xsp2 = __eval2_xpr(ndx);

 if (xsp2->bp[0] != 0L)
  {
   __badind_a = xsp2->ap[0];
   __badind_b = xsp2->bp[0];
   __badind_wid = ndx->szu.xclen;
   __pop_xstk();
   return(-1);
  }
 biti2 = (int32) xsp2->ap[0];
 __pop_xstk();

normalize:
 /* convert index to h:0 normalized value */
 /* === SJM 11/16/03 - using inline code this is slower 
 if (np->n_isarr) __getarr_range(np, &ri1, &ri2, &obwid);
 else { __getwir_range(np, &ri1, &ri2); obwid = np->nwid; }
 === */ 

 /* SJM 11/14/03 - use original code - MAYBE PUTMEBACK */ 
 if (np->n_isarr)
  {
   ri1 = np->nu.rngarr->ai1;
   ri2 = np->nu.rngarr->ai2;
   obwid = (ri1 >= ri2) ? (ri1 - ri2 + 1) : (ri2 - ri1 + 1);
  }
 else
  {
   if (np->nrngrep == NX_CT)
    {
     ri1 = (int32) __contab[np->nu.ct->nx1->ru.xvi];
     ri2 = (int32) __contab[np->nu.ct->nx2->ru.xvi];
    }
   else
    {
     ri1 = np->nu.rngdwir->ni1;
     ri2 = np->nu.rngdwir->ni2;
    }
   obwid = np->nwid;
  }

 biti = normalize_ndx_(biti2, ri1, ri2);
 /* SJM 05/21/04 - for 0:h - if above - value will be negative */
 if (biti >= obwid || biti < 0)
  { __badind_a = biti2; __badind_b = 0L; __badind_wid = WBITS; return(-1); }
 return(biti);
}

/*
 * pop the expression stack
 * DBG macro: #define __pop_xstk() xsp = __xstk[__xspi--]
 */
/* -- ??? DBG ADD --
extern void __pop_xstk(void)
{
 struct xstk_t *xsp;

 if (__xspi < 0) __arg_terr(__FILE__, __LINE__);
 xsp = __xstk[__xspi];
 if (xsp->xsawlen > MUSTFREEWORDS) __chg_xstk_width(xsp, 1);
 __xspi--;
 -* ---
 if (__debug_flg)
  __dbg_msg("+++ popping stack to height %d, old bit length %d\n", __xspi + 1,
   xsp->xslen);
 ---*-
}
--- */

/* BEWARE - a and b parts must be contiguous because often refed as only a */ 
/* -- ??? DBG ADD --
extern struct xstk_t *__push_xstk(int32 blen)
{ 
 register struct xstk_t *xsp;

 if (++__xspi >= __maxxnest) __grow_xstk();
 xsp = __xstk[__xspi];
 if (wlen_(blen) > xsp->xsawlen) __chg_xstk_width(xsp, wlen_(blen));
 xsp->bp = &(xsp->ap[wlen_(blen)]);
 xsp->xslen = blen;

 -* --- *-
 if (__debug_flg)
  __dbg_msg("+++ pushing stack to height %d, bit length %d\n", __xspi + 1,
   xsp->xslen);
 -* ---*-

 return(xsp);
}
--- */

/*
 * load one bit into low bit position of registers rgpa and rgpb from
 * coded wp of length blen from biti current instance stored using
 * representation srep;
 * biti must be corrected to normalized h:0 value before here 
 * and biti of x also handled before here
 * know result will be isolated in low bit value (no need to mask)
 *
 * this differs from rhs bit select in accessing value for current inst and
 * adjusting place to select from according to storage representation
 * if value out of range sets result to x
 *
 * cannot be used to access array and ignores strength parts of stren values
 * at least for now this load makes a copy in normal ab vector form
 *
 * only run time SR forms possible here
 * BEWARE - this is sometimes used to load scalar so much leave scalar sreps 
 */
extern void __ld_bit(register word32 *rgap, register word32 *rgbp,
 register struct net_t *np, int32 biti)
{
 register word32 uwrd, *rap;
 register int32 wlen;

 /* out of range is x */
 if (biti > np->nwid) { rgap[0] = rgbp[0] = 1L; return; }
 /* this is number of words needed to hold a or b part not region */ 
 switch ((byte) np->srep) {
  case SR_VEC:
   wlen = wlen_(np->nwid);
   /* rap is start of instance coded vector a/b groups */
   rap = &(np->nva.wp[2*wlen*__inum]);
   rgap[0] = rhsbsel_(rap, biti);
   rgbp[0] = rhsbsel_(&(rap[wlen]), biti);
   break;
  case SR_SVEC:
   /* strength vectors normalized to h:0 which means v[0] is index 0 */
   /* since h:0 means low bit to left in radix style notation */
   uwrd = (word32) np->nva.bp[__inum*np->nwid + biti];
do_slb:
   rgap[0] = uwrd & 1L;
   rgbp[0] = (uwrd >> 1) & 1L;
   break;
  case SR_SCAL:
   /* LOOKATME - maybe load bit of scalar should be fatat err */
   /* this is same as full value load for 1 bit thing */
   /* notice biti will be 0 or will not get here */
   ld_scalval_(rgap, rgbp, np->nva.bp);
   break;
  case SR_SSCAL:
   uwrd = (word32) np->nva.bp[__inum];
   goto do_slb;
  case SR_PISNUM:
   wlen = wlen_(np->nwid);
   /* rap is start of instance coded vector a/b groups */
   rap = &(np->nva.wp[2*wlen*__inum]);
   rgap[0] = rhsbsel_(rap, biti);
   rgbp[0] = rhsbsel_(&(rap[wlen]), biti);
   break;
  /* SJM 08/22/00 - also allowing part and bit selects from parameters */
  case SR_PNUM:
   wlen = wlen_(np->nwid);
   /* rap is start of instance coded vector a/b groups */
   rap = np->nva.wp;
   rgap[0] = rhsbsel_(rap, biti);
   rgbp[0] = rhsbsel_(&(rap[wlen]), biti);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * load one indexed element of a array map, size mlen with each element
 * blen wide from index arri from current instance into rgap and rgbp
 *
 * arrays stored and normalized h:0 just like vectors
 *
 * vector (or scalar) are packed into 1 linear array of partial bit
 * elements - 4,8,16,32 with both a and b together - both array index
 * and instance index used to decode linear array
 *
 * arri here must be normalized to h:0 form
 * map is base of array area (np nva)
 */
extern void __ld_arr_val(register word32 *rgap, register word32 *rgbp,
 union pck_u map, int32 mlen, int32 blen, int32 arri)
{
 register int32 wlen;
 register word32 *rap;
 word32 tmpw;
 int32 indi;

 /* compute number of words used to store 1 array element */
 /* 17 or more bits cannot be packed with multiple elements per word32 */
 if (blen > WBITS/2)
  {
   /* case 1: each vector element of array needs multiple words */
   wlen = wlen_(blen);
   /* find array for inst i with each vector wlen words wide */
   rap = &(map.wp[2*wlen*mlen*__inum]);
   /* find element arri that may be a vector */
   rap = &(rap[arri*2*wlen]);
   cp_walign_(rgap, rap, blen);
   cp_walign_(rgbp, &(rap[wlen]), blen);
   return;
  }
 /* case 2: array of scalars */
 if (blen == 1)
  {
   /* here arri is real twice real index to get bit index */
   indi = 2*(mlen*__inum + arri);
   tmpw = map.wp[get_wofs_(indi)] >> (get_bofs_(indi));
   rgap[0] = tmpw & 1L;
   rgbp[0] = (tmpw >> 1) & 1L;
   return;
  }
 /* case 3: multiple elements packed per word32, half word32, or byte */
 indi = __inum*mlen + arri;
 /* SJM 12/16/99 - still need to really pack memories to bytes and hwords */
 tmpw = get_packintowrd_(map, indi, blen);
 rgap[0] = tmpw & __masktab[blen];
 /* know high unused bits of byte, hword, or word 0 */
 rgbp[0] = (tmpw >> blen);
}

/*
 * push (access) a selected part select range on top of reg. stack
 * know index values must be <= WBITS constants
 * notice for now only one representation for vector that can be part selected
 * from - one bit ok but cannot part select from scalar
 */
static void push_psel(register struct expr_t *ndp)
{
 register struct expr_t *ndx1, *ndx2;
 int32 bi1, bi2;
 struct expr_t *idndp;
 struct xstk_t *xsp;
 struct net_t *np;
 struct gref_t *grp;

 idndp = ndp->lu.x;
 /* know these are both constant nodes */
 ndx1 = ndp->ru.x->lu.x;
 ndx2 = ndp->ru.x->ru.x;
 push_xstk_(xsp, ndp->szu.xclen);

 /* here never need to execute a part select with x range - what is meaning */
 bi1 = __contab[ndx1->ru.xvi];
 bi2 = __contab[ndx2->ru.xvi];
 /* notice calling of ld psel unwound to 2 paths */
 if (idndp->optyp == GLBREF)
  {
   grp = idndp->ru.grp;    
   __xmrpush_refgrp_to_targ(grp);
   np = grp->targsyp->el.enp;
   __ld_psel(xsp->ap, xsp->bp, np, bi1, bi2);
   __pop_itstk();
   return;
  }
 /* part selects bit numbered from h:0 (i.e. 31..0) */
 np = idndp->lu.sy->el.enp;
 __ld_psel(xsp->ap, xsp->bp, np, bi1, bi2);
}

/*
 * load part select into low bit positions of registers rgap and rgbp from
 * coded wp of length blen from bit1 to bit2 current instance
 *
 * representation must be a vector 
 * bit1 > bit2 according to normalized h:0 form
 * low bit is 0 and high bit is 31 in word
 * part select in range or will not get here
 *
 * notice perfectly legal to declare wire [1:1] x and psel the 1 bit
 */
extern void __ld_psel(register word32 *rgap, register word32 *rgbp,
 register struct net_t *np, int32 bith, int32 bitl)
{
 register word32 *rap;
 byte *netsbp;
 int32 wlen, numbits;
 
 numbits = bith - bitl + 1;

 switch ((byte) np->srep) {
  case SR_VEC:
   /* rap is start of current instance coded vector a b groups */
   wlen = wlen_(np->nwid);
   rap = &(np->nva.wp[2*wlen*__inum]);
   /* this routine expects select to start from bit in 1st word32 */
   __rhspsel(rgap, rap, bitl, numbits);
   rap = &(rap[wlen]);
   __rhspsel(rgbp, rap, bitl, numbits);
   break;
  case SR_SVEC:
   netsbp = &(np->nva.bp[__inum*np->nwid + bitl]);
   /* copy from low to high in array according to radix notation */
   __ld_stval(rgap, rgbp, netsbp, numbits);
   break;
  /* SJM 08/22/00 - also allowing part and bit selects from parameters */
  case SR_PNUM:
   /* rap is start of current instance coded vector a b groups */
   wlen = wlen_(np->nwid);
   rap = np->nva.wp;
   /* this routine expects select to start from bit in 1st word32 */
   __rhspsel(rgap, rap, bitl, numbits);
   rap = &(rap[wlen]);
   __rhspsel(rgbp, rap, bitl, numbits);
   break;
  case SR_PISNUM:
   /* rap is start of current instance coded vector a b groups */
   wlen = wlen_(np->nwid);
   rap = &(np->nva.wp[2*wlen*__inum]);
   /* this routine expects select to start from bit in 1st word32 */
   __rhspsel(rgap, rap, bitl, numbits);
   rap = &(rap[wlen]);
   __rhspsel(rgbp, rap, bitl, numbits);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * right hand side part select from swp of len sblen at sbit1 into dwp
 *
 * selecting numbits long section
 * sbits must be in range 0 to [high bit]
 * notice for part select, range correction done at compile time
 * also notice that 1 bits things can go through here but not lhs psel
 */
extern void __rhspsel(register word32 *dwp, register word32 *swp,
 register int32 sbit1, register int32 numbits)
{
 register int32 wi, corsbit1;

 /* case 1 - select within 1st word32 */
 if (sbit1 + numbits <= WBITS)
  {
   corsbit1 = sbit1;
do_inword:
   if (corsbit1 == 0) *dwp = *swp & __masktab[numbits];
   else *dwp = (*swp >> corsbit1) & __masktab[numbits];
   return;
  }

 /* normalize so swp and corsbit1 start of src with corsbit1 in 1st word32 */
 wi = get_wofs_(sbit1);
 swp = &(swp[wi]);
 corsbit1 = ubits_(sbit1);

 /* case 2 - selection does not cross word32 boundary */
 if (corsbit1 + numbits <= WBITS) goto do_inword;

 /* case 3a - selection crosses word32 boundary but start on word32 boundary */
 if (corsbit1 == 0) { cp_walign_(dwp, swp, numbits); return; }

 /* case 3a - crosses 1 word32 boundary and <= WBITS long */
 if (numbits <= WBITS)
  {
   *dwp = (swp[0] >> corsbit1);
   *dwp |= swp[1] << (WBITS - corsbit1);
   *dwp &= __masktab[numbits];
   return;
  }
 __cp_sofs_wval(dwp, swp, corsbit1, numbits);
}

/*
 * ROUTINES TO IMPLEMENT READMEM
 */

/*
 * execute the readmem[bh] system task
 * know 2nd argument array and from 2 to 4 args or will not get here
 * LOOKATME maybe a memory leak with __cur_fnam?
 */
extern void __exec_readmem(struct expr_t *argxp, int32 base)
{
 int32 slen, ri1, ri2, arrwid, arr1, arr2;
 int32 tmpi, sav_lincnt, nd_itpop;
 FILE *f;
 struct expr_t *axp;
 struct net_t *np;
 char *chp, *savfnam;
 char s1[RECLEN];

 axp = argxp->lu.x;
 /* if contains non printable ok, since will just not be able to open file */
 chp = __get_eval_cstr(axp, &slen);
 savfnam = __cur_fnam;
 sav_lincnt = __lin_cnt;
 /* notice must copy out of __exprline since reused below */
 __cur_fnam = chp;
 if ((f = __tilde_fopen(__cur_fnam, "r")) == NULL)
  {
   __sgferr(716, "unable to open $readmem%c input file %s",
    __to_baselet(base), __cur_fnam);
   goto no_fil_done;
  }

 /* 2nd arg. is array destination */
 argxp = argxp->ru.x;
 axp = argxp->lu.x; 

 /* know will be array name ID or array name global reference */
 if (axp->optyp == GLBREF)
  {
   __xmrpush_refgrp_to_targ(axp->ru.grp);
   nd_itpop = TRUE;
  }
 else nd_itpop = FALSE;

 if (axp->is_real)
  {
   __sgferr(717, "$readmem%c of %s illegal no readmem of array of reals",
    __to_baselet(base), __msgexpr_tostr(__xs, axp));
   goto fil_done;
  }
 np = axp->lu.sy->el.enp;

 argxp = argxp->ru.x;
 /* know this is array so range is array range */
 __getarr_range(np, &ri1, &ri2, &arrwid);
 /* set up addresses */
 /* if no range - use array declaration range */ 
 arr1 = arr2 = -1;
 if (argxp != NULL) 
  {
   axp = argxp->lu.x;
   if (axp->optyp != OPEMPTY) 
    {
     if ((tmpi = __comp_ndx(np, axp)) == -1)
      {
       __sgferr(717,
        "$readmem%c start address expression %s unknown or out of range",
        __to_baselet(base), __msgexpr_tostr(__xs, axp));
       goto fil_done;
      }
     sprintf(s1, "$readmem%c start", __to_baselet(base));
     if (!chk_rm_rng_legal(tmpi, ri1, ri2, s1)) goto fil_done;
     arr1 = tmpi;
    }
   argxp = argxp->ru.x;
   /* if no 2nd ending range (4th arg), use array decl. 2nd range */
   if (argxp != NULL)
    {
     axp = argxp->lu.x;
     if (axp->optyp != OPEMPTY) 
      {
       if ((tmpi = __comp_ndx(np, axp)) == -1)
        {
         __sgferr(718, 
          "$readmem%c end address expression %s unknown or out of range",
          __to_baselet(base), __msgexpr_tostr(__xs, axp));
         goto fil_done;
        }
       sprintf(s1, "$readmem%c end", __to_baselet(base));
       if (!chk_rm_rng_legal(tmpi, ri1, ri2, s1)) goto fil_done;
       arr2 = tmpi;
       if (arr1 == -1)
        {
         __sgferr(718, 
          "$readmem%c end address value %d illegal - no start address",
          __to_baselet(base), arr2);
         goto fil_done;
        }
      }
    }
  }
 __do_rm_reading(f, base, np, arr1, arr2, ri1, ri2, arrwid);
 /* DBG remove ---
 if (__debug_flg) __dmp_arr_all(np, __inum, __inum);
 --- */ 

fil_done:
 if (nd_itpop) __pop_itstk();
 __my_fclose(f);
no_fil_done:
 __lin_cnt = sav_lincnt;
 __cur_fnam = savfnam;
}

/*
 * check a readmem range value to make sure in range
 * return F on error
 */
static int32 chk_rm_rng_legal(int32 tmpi, int32 ri1, int32 ri2, char *msg)
{
 if (ri1 <= ri2) { if (tmpi >= ri1 && tmpi <= ri2) return(TRUE); } 
 else { if (tmpi <= ri1 && tmpi >= ri2) return(TRUE); }
 __sgferr(712, "%s address value %d outside memory range [%d:%d]",
  msg, tmpi, ri1, ri2);
 return(FALSE);
}

/*
 * do the readmem reading and filling 
 *
 * SJM - 06/19/00 - LRM wrong fills from 0 to high unless
 * ranges arr1 and arr2 both given
 * -1 for arr1 and/or arr2 if not given if only arr1 given direction up
 * arr2 can only be set if arr1 is
 * only way array down (-1 direction) if arr1 > arr2
 *
 * this corrects each time for non h:0 form if needed
 * if readmem array is XMR itree loc changed before this call
 */
extern void __do_rm_reading(FILE *f, int32 base, struct net_t *np,
 int32 arr1, int32 arr2, int32 ri1, int32 ri2, int32 arrwid)
{
 register int32 arri, rmfr, rmto; 
 int32 dir, nbytes, ttyp, h0_arri, no_rngwarn;
 struct xstk_t *xsp;

 /* fill the array */
 no_rngwarn = FALSE;
 __lin_cnt = 1;
 /* if only one starting range, direction toward high */
 /* only way for downward range is arr2 not -1 and arr2 > arr1 */
 if (arr2 != -1) { if (arr1 <= arr2) dir = 1; else dir = -1; }
 else dir = 1;

 if (arr1 == -1)
  {
   if (ri1 <= ri2) { rmfr = ri1; rmto = ri2; }
   else { rmfr = ri2; rmto = ri1; }
  }
 else if (arr2 == -1)
  {
   rmfr = arr1;
   if (ri1 <= ri2) rmto = ri2; else rmto = ri1;
  }
 else { rmfr = arr1; rmto = arr2; }

 for (arri = rmfr;;)
  {
   /* separate get tok that does not malloc value */
   ttyp = mdata_gettok(f, base);
   if (ttyp == TEOF)
    {
     /* only range not filled warning if range passed */
     if (!no_rngwarn && arr2 != -1 && arri != rmto)
      __sgfwarn(529,
       "$readmem%c fewer data elements than range size at **%s(%d)",
       __to_baselet(base), __cur_fnam, __lin_cnt);
     return;
    }
   if (ttyp == BADOBJ)
    {
     __sgferr(720,
      "illegal $readmem%c file value at **%s(%d) - loading terminated",
      __to_baselet(base), __cur_fnam, __lin_cnt); 
     break;
    }
   if (ttyp == RMADDR) 
    {
     /* convert to hex number - in __acwrk value - check number */
     /* no error possible in here for hex since digits checked during */
     /* token input */
     /* notice this puts in [a/b]cwrk but does not allocate */
     __to_dhboval(BHEX, TRUE);
     if (__bcwrk[0] != 0L)
      {
       __sgferr(721,
       "illegal $readmem%c address value %s at **%s(%d) - loading terminated",
        __to_baselet(base), __regab_tostr(__xs, __acwrk, __bcwrk, __itoklen,
        BHEX, FALSE), __cur_fnam, __lin_cnt); 
       return;
      }
     /* check for within specified range */
     arri = (int32) __acwrk[0];
     if ((dir == 1 && (arri < rmfr || arri > rmto))
      || (dir == -1 && (arri > rmfr || arri < rmto)))
      {
       __sgferr(722,
       "$readmem%c address value %lu out of range at **%s(%d) - loading terminated",
        __to_baselet(base), __acwrk[0], __cur_fnam, __lin_cnt); 
       return;
      }
     no_rngwarn = TRUE;
     continue;
    }
   if ((dir == 1 && arri > rmto) || (dir == -1 && arri < rmto))
    {
     __sgfwarn(521, "$readmem%c extra data words at **%s(%d) ignored",
      __to_baselet(base), __cur_fnam, __lin_cnt);
     return;
    }
   __to_dhboval(base, TRUE);
   h0_arri = normalize_ndx_(arri, ri1, ri2); 
   /* SJM 09/22/03 - modern P1364 algorithm requires chg stores for each */ 
   /* index if memory elements appear on any assign rhs */
   /* correct size if needed */
   if (np->nwid != __itoklen)
    {
     push_xstk_(xsp, __itoklen);
     nbytes = WRDBYTES*wlen_(__itoklen); 
     memcpy(xsp->ap, __acwrk, nbytes);
     memcpy(xsp->bp, __bcwrk, nbytes);

     /* SJM 09/29/03 - change to handle sign extension and separate types */
     if (xsp->xslen > np->nwid) __narrow_sizchg(xsp, np->nwid);
     else if (xsp->xslen < np->nwid)
      {
       if (np->n_signed) __sgn_xtnd_widen(xsp, np->nwid);
       else __sizchg_widen(xsp, np->nwid);
      }

     if (np->nchg_nd_chgstore)
      {
       __chg_st_arr_val(np->nva, arrwid, np->nwid, h0_arri,
        xsp->ap, xsp->bp);

       /* must only trigger change for right array index */
       if (__lhs_changed) record_sel_nchg_(np, h0_arri, h0_arri);
      }
     else __st_arr_val(np->nva, arrwid, np->nwid, h0_arri, xsp->ap, xsp->bp);
     __pop_xstk();
    }
   else
    {
     if (np->nchg_nd_chgstore)
      {
       __chg_st_arr_val(np->nva, arrwid, np->nwid, h0_arri,
        __acwrk, __bcwrk);

       /* must only trigger change for right array index */
       if (__lhs_changed) record_sel_nchg_(np, h0_arri, h0_arri);
      }
     else __st_arr_val(np->nva, arrwid, np->nwid, h0_arri, __acwrk, __bcwrk);
    }

   /* finally increment index */
   if (dir > 0) { arri++; if (arri > rmto) break; }
   else { arri--; if (arri < rmto) break; }
  }
}

/*
 * read a readmem file style token
 * (modified from yylex in "The Unix Programming Environment" p. 337)
 * value in token of __itoklen bits
 * array width here limited to 1023 chars 
 */
static int32 mdata_gettok(FILE *f, int32 base)
{
 register int32 c;

again:
 while ((c = rm_getc(f)) == ' ' || c == '\t' || c == '\f' || c == '\r') ;
 if (c == '\n') { __lin_cnt++; goto again; }
 if (c == EOF) return(TEOF);

 if (c == '/')
  { if (rmrd_comment(f) == UNDEF) goto again; else return(BADOBJ); }
 if (c == '@') 
  {
   c = rm_getc(f);
   if (mdata_rdhex(f, c) == BADOBJ) return(BADOBJ);
   return(RMADDR);
  }
 if (base == BBIN)
  {
   if (mdata_rdbin(f, c) == BADOBJ) return(BADOBJ);
   return(NUMBER);
  }
 if (mdata_rdhex(f, c) == BADOBJ) return(BADOBJ);
 return(NUMBER);
}

/*
 * readmem form of get a comment
 */
static int32 rmrd_comment(FILE *f)
{
 register int32 c;
 int32 c2;

 /* // to EOL comment */
 if ((c2 = rm_getc(f)) == '/')
  {
   while ((c = rm_getc(f)) != '\n') if (c == EOF) return(TEOF);
   __lin_cnt++;
   return(UNDEF);
  }
 /* slash-star comments don't nest */
 if (c2 == '*')
  {
more_comment:
   while ((c = rm_getc(f)) != '*')
    {
     /* error if / * comments nested */
     if (c == '/')
      {
       if ((c2 = rm_getc(f)) == '*')
        { __inform(407, "/* style comment nested in readmem"); continue; }
       c = c2;
      }
     if (c == EOF) return(TEOF);
     if (c == '\n') __lin_cnt++;
    }
got_star:
   if ((c = rm_getc(f)) == '/') return(UNDEF);
   if (c == '*') goto got_star;
   if (c == '\n') __lin_cnt++;
   goto more_comment;
  }
 /* not a comment so treat as name token */
 rm_ungetc(c2, f);
 return(BADOBJ);
}

/*
 * routine to read readmem style hex number
 */
static int32 mdata_rdhex(FILE *f, int32 c)
{
 register char *chp;
 int32 len, nsize;
 int32 has_digit = FALSE;
 int32 toolong = FALSE;
 
 for (chp = __numtoken, len = 0;;)
  {
   if (c ==  '_') { c = rm_getc(f); continue; } 
   if (!is_mdataxdigit(c))
    {
     *chp = '\0';
     if (!has_digit) return(BADOBJ);
     /* if white space good end */
     if (vis_white_(c))
      {
       /* if new line must push back for correct line counts */
       if (c == '\n') rm_ungetc(c, f);
       break;
      }
     /* end of string only good is reading smem */ 
     if (f == NULL && (c == '\0' || c == -1)) break;
     return(BADOBJ);

     /* end of string only good is reading smem */ 
     /* LOOKATME - think this can never happen */
     if (f == NULL && c == '\0') break;
     /* SJM 09/13/99 - EOF char returned for both files and strings */
     if (f != NULL && c == EOF) { rm_ungetc(c, f); break; }
    }
   if (c == '?') c = 'X';
   else if (isalpha(c) && isupper(c)) c = tolower(c);

   if (++len >= __numtok_wid)
    {
     /* since hex each digit takes 4 bits */
     if (len >= (MAXNUMBITS + 1)/4) 
      {
       if (!toolong)
        {
         __pv_fwarn(522, "readmem value or address too wide (%d) - truncated",
          MAXNUMBITS);
         toolong = TRUE;
        }
       len--;
      }
     else
      {
       /* increase size and continue */
       /* LOOKATME - SJM 03/20/00 - doubling may be too fast growth */
       nsize = 2*__numtok_wid;
       __numtoken = __my_realloc(__numtoken, __numtok_wid, nsize);
       __numtok_wid = nsize;
       /* AIV 04/20/06 - need to reset chp pointer due to realloc */ 
       chp = &(__numtoken[len - 1]); 
       *chp++ = c;
      }
    }
   else *chp++ = c;

   c = rm_getc(f);
   has_digit = TRUE;
  }
 __itoklen = 4*len;
 return(NUMBER);
}

/*
 * return T if readmem style hex digit 
 */
static int32 is_mdataxdigit(int32 c)
{
 switch ((byte) c) {
  case 'z': case 'Z': case 'x': case 'X': case '?': break;
  default: if (isxdigit(c)) return(TRUE); else return(FALSE);
 }
 return(TRUE);
}

/*
 * routine to read readmem style binary number
 */
static int32 mdata_rdbin(FILE *f, int32 c)
{
 register char *chp;
 register int32 len;
 int32 nsize;
 int32 has_bit = FALSE;
 int32 toolong = FALSE;

 for (len = 0, chp = __numtoken;;)
  {
   if (c ==  '_') { c = rm_getc(f); continue; } 
   if (!is_mdatabit(c))
    {
     *chp = '\0';
     if (!has_bit) return(BADOBJ);
     /* if white space good end */
     if (vis_white_(c))
      {
       /* if new line must push back for correct line counts */
       /* impossible in string case */
       if (c == '\n') rm_ungetc(c, f);
       break;
      }
     /* SJM 09/13/99 - EOF char return for both files and strings */
     if (c == EOF) { rm_ungetc(c, f); break; }
     return(BADOBJ);
    }
   if (c == '?') c = 'X';
   else if (isalpha(c) && isupper(c)) c = tolower(c);

   if (++len >= __numtok_wid)
    {
     if (len >= MAXNUMBITS) 
      {
       if (!toolong)
        {
         __pv_fwarn(523, "readmemb value too wide (%d) - truncated",
          MAXNUMBITS);
         toolong = TRUE;
        }
       len--;
      }
     else
      {
       /* increase size and continue */
       /* LOOKATME - SJM 03/20/00 - doubling may be too fast growth */
       nsize = 2*__numtok_wid;
       __numtoken = __my_realloc(__numtoken, __numtok_wid, nsize);
       __numtok_wid = nsize;
       /* AIV 04/20/06 - need to reset chp pointer due to realloc */ 
       chp = &(__numtoken[len - 1]); 
       *chp++ = c;
      }
    }
   else *chp++ = c;

   c = rm_getc(f);
   has_bit = TRUE;
  }
 __itoklen = len;
 return(NUMBER);
}

/*
 * special get that allow reading from sreadmem c style string argument
 */
static int32 rm_getc(FILE *f)
{
 register int32 c;
 int32 blen;

 if (f == NULL)
  {
   c = *__srm_strp;
   /* end of string means white space */
   if (c == '\0')
    {
     struct xstk_t *xsp;

     if (__srm_strp_len != 0)
      __my_free(__srm_strp_beg, __srm_strp_len);
     __srm_strp_len = 0;

     /* need 2 cases for eof since, normally would push eof back first */
     /* but when reading strings cannot do that */
     if (__srm_xp == NULL) return(EOF);
     if ((__srm_xp = __srm_xp->ru.x) == NULL) return(EOF);

     /* eval to allow params and expressions */
     xsp = __eval_xpr(__srm_xp->lu.x);
     if (!vval_is0_(xsp->bp, xsp->xslen))
      {
       __pv_fwarn(579,
        "sreadmem required string is x/z number (pos. %d) - x/z's ignored",
        __srm_stargi);
      }         
     /* must trim away high 0's since will cause leading \0 at start */
     blen = __trim1_0val(xsp->ap, xsp->xslen);
     blen = ((blen + 7)/8)*8;

     /* SJM 09/29/03 - change to handle sign extension and separate types */
     if (xsp->xslen > blen) __narrow_sizchg(xsp, blen);
     else if (xsp->xslen < blen)
      {
       if (__srm_xp->has_sign) __sgn_xtnd_widen(xsp, blen);
       else __sizchg_widen(xsp, blen);
      }

     /* notice this is actual Pascal style string len, no ending \0 */
     __srm_strp_beg = __vval_to_vstr(xsp->ap, xsp->xslen, &__srm_strp_len);
     __srm_strp = __srm_strp_beg;
     __pop_xstk();
     if (__debug_flg) __dbg_msg("new sreadmem string: %s\n", __srm_strp);
     __srm_stargi++;
     return(' ');
    }
   __srm_strp++;
   return(c);
  }
 return(getc(f));
}

/*
 * special get that allow reading from sreadmem c style string argument
 * caller's responsible to not back over front 
 */
static void rm_ungetc(int32 c, FILE *f)
{
 if (f != NULL) ungetc(c, f); else __srm_strp--;
}

/*
 * return T if readmem style bin digit 
 */
static int32 is_mdatabit(int32 c)
{
 switch ((byte) c) {
  case 'z': case 'Z': case 'x': case 'X': case '?': case '0': case '1':
   break;
  default:
  return(FALSE);
 }
 return(TRUE);
}

/*
 * execute the sreadmem[bh] system task
 * know 1st argument array, 2nd and 3nd range, rest strings
 */
extern void __exec_sreadmem(struct expr_t *argxp, int32 base)
{
 int32 ri1, ri2, arrwid, arr1, arr2, tmpi, nd_itpop;
 struct expr_t *axp;
 struct net_t *np;
 char s1[RECLEN];

 /* 1st arg. is array destination to read into */
 axp = argxp->lu.x;
 /* know will be array name ID or array name global reference */
 if (axp->optyp == GLBREF)
  {
   __xmrpush_refgrp_to_targ(axp->ru.grp);
   nd_itpop = TRUE;
  }
 else nd_itpop = FALSE;

 np = axp->lu.sy->el.enp;
 if (axp->is_real)
  {
   __sgferr(717, "$sreadmem%c of %s illegal - no readmem of array of reals",
    __to_baselet(base), __msgexpr_tostr(__xs, axp));
   goto done;
  }
 /* know this is array so range is array range */
 __getarr_range(np, &ri1, &ri2, &arrwid);

 arr1 = arr2 = -1;
 argxp = argxp->ru.x;
 /* set up addresses - start and end range must be present or fixup error */
 axp = argxp->lu.x;
 /* for system task arguments optyp empty not used - know arg present */
 if (axp->optyp != OPEMPTY)
  {
   if ((tmpi = __comp_ndx(np, axp)) == -1)
    {
     __sgferr(723,
      "$sreadmem%c start address expression %s unknown or out of range",
      __to_baselet(base), __msgexpr_tostr(__xs, axp));
     goto done;
    }
   sprintf(s1, "$sreadmem%c start", __to_baselet(base));
   if (!chk_rm_rng_legal(tmpi, ri1, ri2, s1)) goto done;
   arr1 = tmpi;
  }
 argxp = argxp->ru.x;
 axp = argxp->lu.x;
 if (axp->optyp != OPEMPTY)
  {
   if ((tmpi = __comp_ndx(np, axp)) == -1)
    {
     __sgferr(724,
     "$sreadmem%c end address expression %s unknown or out of range",
      __to_baselet(base), __msgexpr_tostr(__xs, axp));
     goto done;
    }
   sprintf(s1, "$sreadmem%c end", __to_baselet(base));
   if (!chk_rm_rng_legal(tmpi, ri1, ri2, s1)) goto done;
   arr2 = tmpi;
   if (arr1 == -1)
    {
     __sgferr(724, 
      "$sreadmem%c end address value %d illegal - no start address",
      __to_baselet(base), arr2);
     goto done;
    }
  }
 do_srm_xtrct(argxp->ru.x, base, np, arr1, arr2, ri1, ri2, arrwid);
 if (__srm_strp_len != 0) __my_free(__srm_strp_beg, __srm_strp_len);
 __srm_strp_beg = NULL;
 __srm_strp_len = 0;
done:
 if (nd_itpop) __pop_itstk();
 /* DBG remove ---
 if (__debug_flg) __dmp_arr_all(np, __inum, __inum);
 --- */
}

/*
 * do the sreadmem string extraction and array filling
 *
 * this keeps arr1, arr2, and arri in original ranges and corrects each
 * time for non h:0 form - could use correct range and uncorrect for msgs
 *
 * string can be any expr. that evaluates to string
 */
static void do_srm_xtrct(struct expr_t *xp, int32 base, struct net_t *np,
 int32 arr1, int32 arr2, int32 ri1, int32 ri2, int32 arrwid)
{
 register int32 arri; 
 FILE *f;
 int32 dir, ttyp, h0_arri, nbytes, no_rngwarn, blen;
 int32 rmfr, rmto;
 struct xstk_t *xsp;

 no_rngwarn = FALSE;

 /* SJM - 06/19/00 - if only one starting range, direction toward high */
 /* only way for downward range is arr2 not -1 and arr2 > arr1 */
 if (arr2 != -1) { if (arr1 <= arr2) dir = 1; else dir = -1; }
 else dir = 1;

 if (arr1 == -1)
  {
   if (ri1 <= ri2) { rmfr = ri1; rmto = ri2; }
   else { rmfr = ri2; rmto = ri1; }
  }
 else if (arr2 == -1)
  {
   rmfr = arr1;
   if (ri1 <= ri2) rmto = ri2; else rmto = ri1;
  }
 else { rmfr = arr1; rmto = arr2; }

 __srm_stargi = 1;
 __srm_xp = xp;
 __srm_strp_len = 0;
 __srm_strp_beg = NULL;

 /* notice must leave non printing as is and add \0 - know there is room */
 xsp = __eval_xpr(__srm_xp->lu.x);
 /* check for x/z (not really string) */
 if (!vval_is0_(xsp->bp, xsp->xslen))
  {
   __pv_fwarn(579,
    "sreadmem required string value is x/z number (pos. 1) - x/z's ignored");
  }         
 /* must trim away high 0's since will cause leading \0 at start */
 blen = __trim1_0val(xsp->ap, xsp->xslen);
 blen = ((blen + 7)/8)*8;

 /* SJM 09/29/03 - change to handle sign extension and separate types */
 if (xsp->xslen > blen) __narrow_sizchg(xsp, blen);
 else if (xsp->xslen < blen)
  {
   if (__srm_xp->has_sign) __sgn_xtnd_widen(xsp, blen);
   else __sizchg_widen(xsp, blen);
  }

 __srm_strp_beg = __vval_to_vstr(xsp->ap, xsp->xslen, &__srm_strp_len);
 __srm_strp = __srm_strp_beg;
 __pop_xstk();
 if (__debug_flg) __dbg_msg("first sreadmem string: %s\n", __srm_strp);
 f = NULL;

 for (arri = rmfr;;)
  {
   /* also for sreadmem, rm_getc will have freed string ptr needed */
   if (__srm_xp == NULL) ttyp = TEOF;
   else ttyp = mdata_gettok(f, base);
   if (ttyp == TEOF)
    {
     if (!no_rngwarn && arr2 != -1 && arri != rmto)
      __sgfwarn(524,
       "$sreadmem%c number of data words fewer than range size (pos. %d)",
        __to_baselet(base), __srm_stargi);
     return;
    }
   if (ttyp == BADOBJ)
    {
     __sgferr(725,
      "illegal $sreadmem%c string value (pos. %d) - loading terminated",
      __to_baselet(base), __srm_stargi);
     break;
    }
   if (ttyp == RMADDR) 
    {
     /* convert to hex number - in __acwrk value - check number */
     /* no error possible in here for hex since digits checked during */
     /* token input */
     __to_dhboval(BHEX, TRUE);
     if (__bcwrk[0] != 0L)
      {
       __sgferr(726,
       "illegal $readmem%c address value %s (pos. %d) - loading terminated",
        __to_baselet(base), __regab_tostr(__xs, __acwrk, __bcwrk, __itoklen,
        BHEX, FALSE), __srm_stargi);
       return;
      }
     /* check for within specified range */
     arri = (int32) __acwrk[0];
     if ((dir == 1 && (arri < rmfr || arri > rmto))
      || (dir == -1 && (arri > rmfr || arri < rmto)))
      {
       __sgferr(727,
       "$sreadmem%c address value %lu out of range (pos. %d) - loading terminated",
        __to_baselet(base), __acwrk[0], __srm_stargi);
       return;
      }
     no_rngwarn = TRUE;
     continue;
    }
   if ((dir == 1 && arri > rmto) || (dir == -1 && arri < rmto))
    {
     __sgfwarn(525, "$sreadmem%c extra data words ignored (pos. %d)",
      __to_baselet(base), __srm_stargi);
     return;
    }
   __to_dhboval(base, TRUE);

   h0_arri = normalize_ndx_(arri, ri1, ri2); 

   /* SJM 09/22/03 - modern P1364 algorithm requires chg stores for each */ 
   /* index if memory elements appear on any assign rhs */
   /* correct size if needed */
   if (np->nwid != __itoklen)
    {
     push_xstk_(xsp, __itoklen);
     nbytes = WRDBYTES*wlen_(__itoklen); 
     memcpy(xsp->ap, __acwrk, nbytes);
     memcpy(xsp->bp, __bcwrk, nbytes);

     /* SJM 09/29/03 - change to handle sign extension and separate types */
     /* SJM 05/19/04 - notice read into memory can be signed - need net wid */ 
     if (xsp->xslen > np->nwid) __narrow_sizchg(xsp, np->nwid);
     else if (xsp->xslen < np->nwid)
      {
       if (np->n_signed) __sgn_xtnd_widen(xsp, np->nwid);
       else __sizchg_widen(xsp, np->nwid);
      }

     if (np->nchg_nd_chgstore)
      {
       __chg_st_arr_val(np->nva, arrwid, np->nwid, h0_arri,
        xsp->ap, xsp->bp);

       /* must only trigger change for right array index */
       if (__lhs_changed) record_sel_nchg_(np, h0_arri, h0_arri);
      }
     else __st_arr_val(np->nva, arrwid, np->nwid, h0_arri, xsp->ap, xsp->bp);
     __pop_xstk();
    }
   else
    {
     if (np->nchg_nd_chgstore)
      {
       __chg_st_arr_val(np->nva, arrwid, np->nwid, h0_arri,
        __acwrk, __bcwrk);

       /* must only trigger change for right array index */
       if (__lhs_changed) record_sel_nchg_(np, h0_arri, h0_arri);
      }
     else __st_arr_val(np->nva, arrwid, np->nwid, h0_arri, __acwrk, __bcwrk);
    }

   /* finally increment index */
   if (dir > 0) arri++; else arri--;
  }
}

/*
 * RANDOM NUMBER GENERATION ROUTINES 
 */

#define MY_LONG_MAX2 0xffffffff
/* SJM 11/19/03 - must use 32 bit range even for 64 bit systems */ 
/* because Verilog WBITS (for now?) must be set to 32 */
#define MY_LONG_MAX 2147483647L
#define MY_LONG_MIN (-MY_LONG_MAX - 1L)

/*
 * execute the random system function 
 * this leaves new value on tos and uses a copy of the good Berkeley
 * unix style random number generator
 *
 * notice pli code no longer shares random number generator
 * this is 32 bit long int dependent
 *
 * SJM 11/18/03 - rewritten to make seed arg inout instead of input if used
 * SJM 01/27/04 - seed must be C global and initialized to 0 (guess) in
 *                case user never passes the seed argument
 */
extern void __exec_sfrand(struct expr_t *ndp)
{
 int32 ranv;
 int32 localseed;
 struct xstk_t *xsp;
 struct expr_t *fax;

 /* case 1 random with seed set - arg is inout */
 /* AIV 03/06/07 - case should not modify global seed - thanks SAS@ Tharas */
 if (ndp->ru.x != NULL && ndp->ru.x->optyp != OPEMPTY)
  {
   fax = ndp->ru.x->lu.x;
   /* even though evaling, previous error if seed is not simple WBIT reg */
   xsp = __eval_xpr(fax);
   /* know here the argument must be a reg. that is updated */
   /* by setting reg., user can change seed */
   if (xsp->bp[0] != 0)
    {
     __sgfwarn(588,
      "$random seed register value %s has x/z bits - a part used",
      __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
     xsp->bp[0] = 0L;
    }
   localseed = (int32) xsp->ap[0]; 
   __pop_xstk();

   /* this sets the seed (acutally a state) - 1 to reset to defl. sequence */
   /* generator returns only 31 (signed +) bits so high bit always 0 */  
   ranv = rtl_dist_uniform(&localseed, MY_LONG_MIN, MY_LONG_MAX);

   push_xstk_(xsp, WBITS);
   xsp->bp[0] = 0L;
   xsp->ap[0] = ranv;

   /* SJM 11/19/03 - I misread LRM - if seed arg passed it is inout not in */ 
   /* temp use of top of stack - removed before return that needs tos */
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = (word32) localseed;
   xsp->bp[0] = 0L;

   __exec2_proc_assign(fax, xsp->ap, xsp->bp);
   __pop_xstk();
   return; 
  }

 /* case 2: no seed passed */
 /* SJM 01/27/04 - now if no seed passed uses last one */
 push_xstk_(xsp, WBITS);
 xsp->bp[0] = 0L;
 /* generator returns only 31 (signed +) bits so high bit always 0 */  
 ranv = rtl_dist_uniform(&__seed, LONG_MIN, LONG_MAX);
 xsp->ap[0] = (word32) ranv;
}

/*
 * SJM 11/19/03 - replacing $random with LRM 2001 random as suggested
 * by standard - although BSD algorithm better (longer period), it
 * requires 8 words of state which can't be returned in seed
 */

/*
 * uniform distribution low level routine from 2001 LRM
 *
 * this is wrapper handling edge cases and returns 32 bit signed not double
 * notice since uses double and maps back to word32 has good period 
 */
static sword32 rtl_dist_uniform(int32 *seed, sword32 start, sword32 end)
{
 double r;
 sword32 i;

 if (start >= end) return(start);
 if (end != LONG_MAX)
  {
   end++;
   r = uniform(seed, start, end);
   if (r >= 0)
    {
     i = (sword32) r;
    }
   else
    {
     i = (sword32) (r - 1);
    }
   if (i < start) i = start;
   if (i >= end) i = end-1;
  }
 else if (start != LONG_MIN)
  {
   start--;
   r = uniform(seed, start, end) + 1.0;
   if (r >= 0)
    {
     i = (sword32) r;
    }
   else
    {
     i = (sword32) (r - 1);
    }
   if (i <= start) i = start+1;
   if (i > end) i = end;
  }
 else
  {
   r = (uniform(seed,start,end) + 2147483648.0)/4294967295.0;
   r = r*4294967296.0 - 2147483648.0;
   if (r >= 0)
    {
     i = (sword32) r;
    }
   else
    {
     i = (sword32) (r-1);
    }
  }
 return(i);
}

/*
 * uniform distribution low level routine from 2001 LRM
 * this returns double random number
 *
 * BEWARE - IEEE floating point format dependent
 */
static double uniform(int32 *seed, sword32 start, sword32 end)
{
 union u_s
  {
   float s;
   word32 stemp;
  } u;
 double d = 0.00000011920928955078125;
 double a,b,c;

 if ((*seed) == 0) *seed = 259341593;
 if (start >= end)
  {
   a = 0.0;
   b = 2147483647.0;
  }
 else
  {
   a = (double) start;
   b = (double) end;
  }
 *seed = 69069 * (*seed) + 1;
 u.stemp = *seed;

 /* This relies on IEEE floating point format */
 u.stemp = (u.stemp >> 9) | 0x3f800000;
 c = (double) u.s;
 c = c + (c*d);
 c = ((b - a) * (c - 1.0)) + a;
 return(c);
}

/*
 * IMPLEMENT SPECIAL DISTRIBUTION ROUTINES
 */

/*
 * return a randomly distributed integer between start and end
 *
 * notice requiring all 3 arguments - ,, illegal
 *
 * idea for left hand side seed is that normal random is returned
 * so can be used to repeat next normal dist.
 *
 * algorithm maps uniformly distributed random real in 0 to 1 range to int
 * in start to end range (integers where start or end can be negative)
 */
extern void __exec_dist_uniform(struct expr_t *ndp)
{
 int32 start, end, u2;
 word32 val;
 double rng, x;
 struct expr_t *fax, *a1xp, *a2xp, *a3xp;
 struct xstk_t *xsp, *xsp2;

 /* for func passed func node not first arg */
 fax = ndp->ru.x;
 /* access the required 3 arguments */
 if (fax == NULL) __arg_terr(__FILE__, __LINE__);
 a1xp = fax->lu.x;
 if ((fax = fax->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a2xp = fax->lu.x;
 if ((fax = fax->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a3xp = fax->lu.x;
 if (fax->ru.x != NULL) __arg_terr(__FILE__, __LINE__);

 /* all 3 arguments required */
 if (a1xp->optyp == OPEMPTY || a2xp->optyp == OPEMPTY
  || a3xp->optyp == OPEMPTY)
  {
   __sgfwarn(588,
   "$dist_uniform arguments must not be missing (,, form) - returning 32'bx");
   goto ret_x;
  }

 /* even though evaling, previous error if seed is not simple WBIT reg */
 xsp = __eval_xpr(a1xp);
 if (xsp->bp[0] != 0)
  {
   __sgfwarn(588,
    "$dist_uniform seed value %s has x/z bits - returning 32'bx",
    __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
   xsp->bp[0] = 0L;
   __pop_xstk(); 
ret_x:
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = ALL1W;
   xsp->bp[0] = ALL1W;
   return;
  }
 __seed = (int32) xsp->ap[0]; 
 __pop_xstk();

 if (!__get_eval_word(a2xp, &val))
  {
   __sgfwarn(588,
    "$dist_uniform argument 2, start value %s x/z - returning 32'bx",
    __msgexpr_tostr(__xs, a2xp));
   goto ret_x;
  }
 start = (int32) val;
 if (!__get_eval_word(a3xp, &val))
  {
   __sgfwarn(588,
    "$dist_uniform argument 3, end value %s x/z - returning 32'bx",
    __msgexpr_tostr(__xs, a2xp));
   goto ret_x;
  }
 end = (int32) val;
 x = uniform(&__seed, 0, 1); 

 /* LOOKATME - maybe: rng = (double) (end - start); */
 rng = (double) (end - start + 1);
 /* FIXME - does this round right? */
 u2 = start + ((int32) (rng*x));

 push_xstk_(xsp, WBITS);
 xsp->bp[0] = 0L;
 /* notice generator returns only 31 (signed +) bits so high bit always 0 */  
 xsp->ap[0] = (word32) u2;

 push_xstk_(xsp2, WBITS);
 xsp2->ap[0] = (word32) __seed;
 xsp2->bp[0] = 0L;
 __exec2_proc_assign(a1xp, xsp2->ap, xsp2->bp);
 __pop_xstk();
}

/*
 * return randomly distributed int std. normal dist - std. dev. 'standard_dev'
 * and mean 'mean'
 *
 * notice requiring all 3 arguments - ,, illegal
 *
 * algorithm uses ratio method to map uniform random real in 0 to 1 range
 * into standard normal with mean 0.0 and standard deviation 1.0
 * then convert to integer with translated standard_Dev and mean mean
 *
 * i.e. if mean is same 0 and standard_dev is same 1.0 only will return ints
 * -4 to 4 nearly all the time (>4 standard devs rare)
 */
extern void __exec_dist_stdnorm(struct expr_t *ndp)
{
 int32 mean, std_dev, u2;
 word32 val;
 double x, u;
 struct expr_t *fax, *a1xp, *a2xp, *a3xp;
 struct xstk_t *xsp, *xsp2;

 /* for func passed func node not first arg */
 fax = ndp->ru.x;
 /* access the required 3 arguments */
 if (fax == NULL) __arg_terr(__FILE__, __LINE__);
 a1xp = fax->lu.x;
 if ((fax = fax->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a2xp = fax->lu.x;
 if ((fax = fax->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a3xp = fax->lu.x;
 if (fax->ru.x != NULL) __arg_terr(__FILE__, __LINE__);

 /* all 3 arguments required */
 if (a1xp->optyp == OPEMPTY || a2xp->optyp == OPEMPTY
  || a3xp->optyp == OPEMPTY)
  {
   __sgfwarn(588,
   "$dist_normal arguments must not be missing (,, form) - returning 32'bx");
   goto ret_x;
  }

 /* even though evaling, previous error if seed is not simple WBIT reg */
 xsp = __eval_xpr(a1xp);
 if (xsp->bp[0] != 0)
  {
   __sgfwarn(588,
    "$dist_normal seed value %s has x/z bits - returning 32'bx",
    __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
   xsp->bp[0] = 0L;
   __pop_xstk(); 
ret_x:
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = ALL1W;
   xsp->bp[0] = ALL1W;
   return;
  }
 __seed = (int32) xsp->ap[0]; 
 __pop_xstk();

 if (!__get_eval_word(a2xp, &val))
  {
   __sgfwarn(588,
    "$dist_normal argument 2, mean value %s x/z - returning 32'bx",
    __msgexpr_tostr(__xs, a2xp));
   goto ret_x;
  }
 mean = (int32) val;
 if (!__get_eval_word(a3xp, &val))
  {
   __sgfwarn(588,
    "$dist_normal argument 3, standard_dev value %s x/z - returning 32'bx",
    __msgexpr_tostr(__xs, a2xp));
   goto ret_x;
  }
 std_dev = (int32) val;

 /* get the standard normal deviate (stddev=1.0, mean = 0.0) */
 x = stdnorm_dev(&__seed);

 /* map to real with passed std_dev and mean */
 u = (double) mean + x*((double) std_dev);
 /* then to int32 using Verilog round rules */
 u2 = ver_rint_(u);

 push_xstk_(xsp, WBITS);
 xsp->bp[0] = 0L;
 /* notice generator returns only 31 (signed +) bits so high bit always 0 */  
 xsp->ap[0] = (word32) u2;

 push_xstk_(xsp2, WBITS);
 xsp2->ap[0] = (word32) __seed;
 xsp2->bp[0] = 0L;
 __exec2_proc_assign(a1xp, xsp2->ap, xsp2->bp);
 __pop_xstk();
}

/*
 * generate standard norm with median 0 and standard deviation 1
 *
 * from Knuth - Seminumerical algorithms - uses ratio method
 */
static double stdnorm_dev(int32 *seed)
{
 double u, v, x;

 /* generate two uniform deviates in 0.0 to 1.0 (u non 0) */
 for (;;)
  {
   do { u = uniform(seed, 0, 1); } while (u == 0.0); 
   v = uniform(seed, 0, 1); 

   x = (sqrt(8.0/M_E)*(v - 0.5))/u; 
   if (x*x <= -4.0/log(u)) return(x); 
  }
}

/*
 * return randomly distributed exponential with mean 'mean'
 * returns values with average mean 
 *
 * notice requiring both arguments - ,, illegal
 *
 * algorithm uses a=1 case of gamma deviate with mean 1.0
 *
 * i.e. if mean is same 1 - values from 0 to inf with mean 1
 */
extern void __exec_dist_exp(struct expr_t *ndp)
{
 int32 mean, u2;
 word32 val;
 double x, u;
 struct expr_t *fax, *a1xp, *a2xp;
 struct xstk_t *xsp, *xsp2;

 /* for func passed func node not first arg */
 fax = ndp->ru.x;
 /* access the required 2 arguments */
 if (fax == NULL) __arg_terr(__FILE__, __LINE__);
 a1xp = fax->lu.x;
 if ((fax = fax->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a2xp = fax->lu.x;

 /* both arguments required */
 if (a1xp->optyp == OPEMPTY || a2xp->optyp == OPEMPTY)
  {
   __sgfwarn(588,
   "$dist_exponential arguments must not be missing (,, form) - returning 32'bx");
   goto ret_x;
  }

 /* even though evaling, previous error if seed is not simple WBIT reg */
 xsp = __eval_xpr(a1xp);
 if (xsp->bp[0] != 0)
  {
   __sgfwarn(588,
    "$dist_exponential seed value %s has x/z bits - returning 32'bx",
    __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
   xsp->bp[0] = 0L;
   __pop_xstk(); 
ret_x:
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = ALL1W;
   xsp->bp[0] = ALL1W;
   return;
  }
 __seed = (int32) xsp->ap[0]; 
 __pop_xstk();

 if (!__get_eval_word(a2xp, &val))
  {
   __sgfwarn(588,
    "$dist_exponential argument 2, mean value %s x/z - returning 32'bx",
    __msgexpr_tostr(__xs, a2xp));
   goto ret_x;
  }
 mean = (int32) val;

 /* get the gamma deviate (mean = 1.0, ) */
 x = gamma_dev((double) 1, &__seed);

 /* map to real mean mean (expand by multiplying here */
 u = x*((double) mean);
 /* then to int32 (but will always be positive) using Verilog round rules */
 u2 = ver_rint_(u);

 push_xstk_(xsp, WBITS);
 xsp->bp[0] = 0L;
 /* notice generator returns only 31 (signed +) bits so high bit always 0 */  
 xsp->ap[0] = (word32) u2;

 push_xstk_(xsp2, WBITS);
 xsp2->ap[0] = (word32) __seed;
 xsp2->bp[0] = 0L;
 __exec2_proc_assign(a1xp, xsp2->ap, xsp2->bp);
 __pop_xstk();
}

/*
 * gamma deviate from normal deviate of order xia
 * returns real with mean around 1.0
 * 
 * using seed random seed and setting seed to last generated
 * again from Numerical Recipes
 * caller must have checked arguments
 */
static double gamma_dev(double xa, int32 *seed)
{
 double am, e, s, v1, v2, x, y;

 if (xa < 1.0) __arg_terr(__FILE__, __LINE__); 
 /* direct method for order 1 - exponential */
 if (xa == 1.0) { x = uniform(seed, 0, 1); x = -log(x); return(x); }

 /* rejection method for any higher order - works for non integal */
 do {
  do {  
   do {
    v1 = uniform(seed, 0, 1);
    v2 = 2.0*uniform(seed, 0, 1) - 1.0;
   } while (v1*v1 + v2*v2 > 1.0);
   y = v2/v1;
   am = xa - 1.0;
   s = sqrt(2.0*am + 1.0);
   x = s*y + am;   
  } while (x <= 0.0);
  e = (1.0 + y*y)*exp(am*log(x/am) - s*y);
 } while (uniform(seed, 0, 1) > e);
 return(x);
}

/*
 * return randomly distributed poisson distribution integer value
 *with mean 'mean'
 *
 * notice requiring both arguments - ,, illegal
 * almost same as gamm dist. but integral so computation different
 *
 * algorithm uses rejection method poisson routine from Numerical
 * Recipes this is integral distribution related to binomial
 */
extern void __exec_dist_poisson(struct expr_t *ndp)
{
 int32 mean, u2;
 word32 val;
 struct expr_t *fax, *a1xp, *a2xp;
 struct xstk_t *xsp, *xsp2;

 /* for func passed func node not first arg */
 fax = ndp->ru.x;
 /* access the required 2 arguments */
 if (fax == NULL) __arg_terr(__FILE__, __LINE__);
 a1xp = fax->lu.x;
 if ((fax = fax->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a2xp = fax->lu.x;

 /* both arguments required */
 if (a1xp->optyp == OPEMPTY || a2xp->optyp == OPEMPTY)
  {
   __sgfwarn(588,
   "$dist_poisson arguments must not be missing (,, form) - returning 32'bx");
   goto ret_x;
  }

 /* even though evaling, previous error if seed is not simple WBIT reg */
 xsp = __eval_xpr(a1xp);
 if (xsp->bp[0] != 0)
  {
   __sgfwarn(588,
    "$dist_poisson seed value %s has x/z bits - returning 32'bx",
    __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
   xsp->bp[0] = 0L;
   __pop_xstk(); 
ret_x:
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = ALL1W;
   xsp->bp[0] = ALL1W;
   return;
  }
 __seed = (int32) xsp->ap[0]; 
 __pop_xstk();

 if (!__get_eval_word(a2xp, &val))
  {
   __sgfwarn(588,
    "$dist_poisson argument 2, mean value %s x/z - returning 32'bx",
    __msgexpr_tostr(__xs, a2xp));
   goto ret_x;
  }
 mean = (int32) val;

 /* get the gamma deviate (mean = 1.0, ) */
 u2 = poisson_dev(mean, &__seed);

 push_xstk_(xsp, WBITS);
 xsp->bp[0] = 0L;
 xsp->ap[0] = (word32) u2;

 push_xstk_(xsp2, WBITS);
 xsp2->ap[0] = (word32) __seed;
 xsp2->bp[0] = 0L;
 __exec2_proc_assign(a1xp, xsp2->ap, xsp2->bp);
 __pop_xstk();
}

/*
 * poission deviate using rejection method (exp for small)
 * 
 * using seed random seed and setting seed to last generated
 * random number
 *
 * again from Numerical Recipes
 */
static int32 poisson_dev(int32 ixm, int32 *seed)
{
 int32 iem;
 double em, g, t, xm, sq, alxm, y; 

 /* direct method for small order */
 if (ixm < 12) 
  {
   g = exp((double) ixm);
   t = 1.0;
   iem = -1;
   do {
    ++iem;
    /* "multiplying uniform deviates same as adding exp"? */
    t *= uniform(seed, 0, 1);
   } while (t > g);
   return(iem);
  }

 /* otherwise use bounded region rejection method */
 xm = (double) ixm;
 sq = sqrt(2.0*ixm);  
 alxm = log (xm);
 g = xm*alxm - log_gamma(xm + 1.0);
 do {
  do {
   y = tan(M_PI*uniform(seed, 0, 1));
   em = sq*y + xm;
   /* reject if in region of 0 probability */
  } while (em < 0.0); 
  /* maybe follow Verilog convention should be rounded? */
  iem = (int32) floor(em);
  t = 0.9 + (1.0*y*y)*exp(em*alxm - log_gamma(em + 1.0) - g);
 /* rejection by probability preserving ratio step */
 } while (uniform(seed, 0, 1) > t);
 return(iem);
}

static double gamma_powser_coeff[6] = { 76.18009172947146,
 -86.50532032941677, 24.01409824083091, -1.231739572450155,
 0.1208650973866179E-2, -0.5395239384953E-5 };

/*
 * log gamma (gamma large so better to calculate with log)
 *
 * This follows method in Numerical Recipes in C
 */
static double log_gamma(double d1)
{
 register int32 j;
 double y, x, tmp, ser;
 
 y = x = d1;
 tmp = x + 5.5;
 tmp -= (x + 0.5)*log(tmp);
 ser = 1.000000000190015;
 for (j = 0; j <= 5; j++) ser += gamma_powser_coeff[j]/++y;
 return(-tmp + log(2.5066282746310005*ser/x));
}

/*
 * return randomly distributed chi-square with freedeg degrees
 * of freedom
 *
 * notice requiring both arguments - ,, illegal
 *
 * algorithm uses gamma dist. of order v/2 and mean 1
 */
extern void __exec_chi_square(struct expr_t *ndp)
{
 int32 u2;
 word32 val;
 double x, u;
 struct expr_t *fax, *a1xp, *a2xp;
 struct xstk_t *xsp, *xsp2;

 /* for func passed func node not first arg */
 fax = ndp->ru.x;
 /* access the required 2 arguments */
 if (fax == NULL) __arg_terr(__FILE__, __LINE__);
 a1xp = fax->lu.x;
 if ((fax = fax->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a2xp = fax->lu.x;

 /* both arguments required */
 if (a1xp->optyp == OPEMPTY || a2xp->optyp == OPEMPTY)
  {
   __sgfwarn(588,
   "$chi_square arguments must not be missing (,, form) - returning 32'bx");
   goto ret_x;
  }

 /* although evaling, previous error if seed is not simple WBIT reg */
 xsp = __eval_xpr(a1xp);
 if (xsp->bp[0] != 0)
  {
   __sgfwarn(588,
    "$dist_chi_square seed value %s has x/z bits - returning 32'bx",
    __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
   xsp->bp[0] = 0L;
   __pop_xstk(); 
ret_x:
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = ALL1W;
   xsp->bp[0] = ALL1W;
   return;
  }
 __seed = (int32) xsp->ap[0]; 
 __pop_xstk();

 if (!__get_eval_word(a2xp, &val))
  {
   __sgfwarn(588,
    "$dist_chi_square argument 2, degree_of_freedom value %s x/z - returning 32'bx",
    __msgexpr_tostr(__xs, a2xp));
   goto ret_x;
  }
 /* look at me - are odd case right?, i.e. does rounding work? */
 /* for <= 2 degress of freedom same as exponential */
 u = val;
 if (u <= 2.0) u = 1.0; else u = u/2.0;

 /* chi-square is 2 times gamma deviate of degree_of_freedom/2 */
 x = gamma_dev(u, &__seed);

 /* map to real mean for chi-square always 1 */
 /* then to int32 (will always be positive) using Verilog round rules */
 u2 = ver_rint_(x);

 push_xstk_(xsp, WBITS);
 xsp->bp[0] = 0L;
 /* notice generator returns only 31 (signed +) bits so high bit always 0 */  
 xsp->ap[0] = (word32) u2;

 push_xstk_(xsp2, WBITS);
 xsp2->ap[0] = (word32) __seed;
 xsp2->bp[0] = 0L;
 __exec2_proc_assign(a1xp, xsp2->ap, xsp2->bp);
 __pop_xstk();
}

/*
 * return randomly distributed t-distribution with freedeg degrees
 * of freedom
 *
 * notice requiring both arguments - ,, illegal
 *
 * t distribution is std normal (real) Y1 with mean 1 and var. 0
 * and chi-square Y2 with v degress of freed and mean 1 (see above)
 * t is then y1/sqrt(y2/v)
 */
extern void __exec_dist_t(struct expr_t *ndp)
{
 int32 u2;
 word32 val;
 double y1, y2, x, u, v;
 struct expr_t *fax, *a1xp, *a2xp;
 struct xstk_t *xsp, *xsp2;

 /* for func passed func node not first arg */
 fax = ndp->ru.x;
 /* access the required 2 arguments */
 if (fax == NULL) __arg_terr(__FILE__, __LINE__);
 a1xp = fax->lu.x;
 if ((fax = fax->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a2xp = fax->lu.x;

 /* both arguments required */
 if (a1xp->optyp == OPEMPTY || a2xp->optyp == OPEMPTY)
  {
   __sgfwarn(588,
   "$dist_t arguments must not be missing (,, form) - returning 32'bx");
   goto ret_x;
  }

 /* although evaling, previous error if seed is not simple WBIT reg */
 xsp = __eval_xpr(a1xp);
 if (xsp->bp[0] != 0)
  {
   __sgfwarn(588,
    "$dist_t seed value %s has x/z bits - returning 32'bx",
    __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
   xsp->bp[0] = 0L;
   __pop_xstk(); 
ret_x:
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = ALL1W;
   xsp->bp[0] = ALL1W;
   return;
  }
 __seed = (int32) xsp->ap[0]; 
 __pop_xstk();

 if (!__get_eval_word(a2xp, &val))
  {
   __sgfwarn(588,
    "$dist_t argument 2, degree_of_freedom value %s x/z - returning 32'bx",
    __msgexpr_tostr(__xs, a2xp));
   goto ret_x;
  }
 /* look at me - are odd case right?, i.e. does rounding work? */
 /* for <= 2 degress of freedom same as exponential */
 v = (double) val;
 if (v <= 2.0) u = 1.0; else u = v/2.0;


 /* Y1 is stdnorm with 0 mean and 1.0 variance */
 y1 = stdnorm_dev(&__seed);

 /* Y2 real chi-square is 2*gamma deviate of degree_of_freedom/2 */
 y2 = gamma_dev(u, &__seed);
 x = y1/sqrt(y2/v); 

 /* map to real mean for chi-square always 1 */
 /* then to int32 (will always be positive) using Verilog round rules */
 u2 = ver_rint_(x);

 push_xstk_(xsp, WBITS);
 xsp->bp[0] = 0L;
 /* generator returns only 31 (signed +) bits so high bit always 0 */  
 xsp->ap[0] = (word32) u2;

 push_xstk_(xsp2, WBITS);
 xsp2->ap[0] = (word32) __seed;
 xsp2->bp[0] = 0L;
 __exec2_proc_assign(a1xp, xsp2->ap, xsp2->bp);
 __pop_xstk();
}

/*
 * SPECIAL OPERATOR EVALUATION ROUTINES
 */

/*
 * evaluate a rhs concatenate on to top of reg stack
 * key is that { op. node width is same as starting high bit of value
 *
 * this requires inst ptr set
 */
extern void __rhs_concat(struct expr_t *lcbndp)
{
 register int32 catxlen;
 register struct expr_t *catndp, *catrhsx; 
 register word32 *ap, *bp;
 word32 *wp, *sawp, *sbwp, tmpa, tmpb;
 int32 wlen, bi1;
 struct xstk_t *catreg, *xsp;
 struct net_t *np;

 /* build concatenate from high (right) to low since faster */
 push_xstk_(catreg, lcbndp->szu.xclen);
 ap = catreg->ap;
 bp = catreg->bp;
 /* never z extend for rhs concat eval. - only assigns */
 zero_allbits_(ap, catreg->xslen);
 zero_allbits_(bp, catreg->xslen);

 /* now evaluate components and assign to section of ap/bp */
 /* first concat component is highest bit(s) so work from high bits to low */ 
 for (catndp = lcbndp->ru.x; catndp != NULL; catndp = catndp->ru.x)
  {
   /* catrhsx actual rhs concat component expr. with width catxlen */
   catrhsx = catndp->lu.x;
   catxlen = catrhsx->szu.xclen;
   /* catndp comma node is dist. to low bit, bi1 is low bit of rhs psel */
   bi1 = catndp->szu.xclen - catxlen;

   /* DBG remove ---
   if (__debug_flg)
    __dbg_msg(
     "+++rhs: total concat wid=%u, low index=%d, wid=%u, remaining wid=%u\n",
      lcbndp->szu.xclen, bi1, catxlen, catndp->szu.xclen);
   --- */ 
   switch ((byte) catrhsx->optyp) {
    /* notice IS number just falls through to expr load case */ 
    case NUMBER:
     wlen = wlen_(catxlen);
     sawp = &(__contab[catrhsx->ru.xvi]);
     sbwp = &(sawp[wlen]);
do_lhssel:
     /* notice lhs bsel takes value not ptr to value */
     if (catxlen == 1)
      { __lhsbsel(ap, bi1, sawp[0]); __lhsbsel(bp, bi1, sbwp[0]); }
     else
      {
       __lhspsel(ap, bi1, sawp, catxlen);
       __lhspsel(bp, bi1, sbwp, catxlen);
      }
     break;
    case ISNUMBER:
     wlen = wlen_(catxlen);
     wp = &(__contab[catrhsx->ru.xvi]);
     sawp = &(wp[2*wlen*__inum]);
     sbwp = &(sawp[wlen]);
     goto do_lhssel;
    case ID:
     /* optimize if fits in word32 case - else just use eval mechanism */
     if (catxlen <= WBITS)
      {
       np = catrhsx->lu.sy->el.enp;
       /* cannot optimize more because can be scalar or strength rep */ 
       __ld_wire_val(&tmpa, &tmpb, np);
       if (catxlen == 1)
	{
	 __lhsbsel(ap, bi1, tmpa & 1L);
	 __lhsbsel(bp, bi1, tmpb & 1L);
	}
       else
	{
         __lhspsel(ap, bi1, &tmpa, catxlen);
         __lhspsel(bp, bi1, &tmpb, catxlen);
	}
       break;
      }
     /* > WBITS so fall thru */
     /* cases PARTSEL, LSB, GLBREF, ... fall thru */
     /* just evaluate normally */
     /* and then assigned like a normal expression */
     /* so far do not have a way of doing these special cases faster */
    default:
     xsp = __eval2_xpr(catrhsx);
     /* get rid of this stuff when debugged */
     /* expression in concatenate has wrong width */
     if (xsp->xslen != catxlen) __misc_terr(__FILE__, __LINE__);
     if (catxlen == 1)
      { __lhsbsel(ap, bi1, xsp->ap[0]); __lhsbsel(bp, bi1, xsp->bp[0]); }
     else
      {
       __lhspsel(ap, bi1, xsp->ap, catxlen);
       __lhspsel(bp, bi1, xsp->bp, catxlen);
      }
     __pop_xstk();
   }
  }
}

/*
 * load rhs only address of a part of contiguous coded variable
 * returned value is read only - must use st_ routines to store
 *
 * this must not be used for strength values
 * for scalars and packed moves to tmp so not really load address
 * i.e. address returned cannot be indirectly stored into
 * 
 * notice xmr must be converted to net before here 
 */
extern void __ld_addr(word32 **aap, word32 **abp, register struct net_t *np)
{
 register word32 *ap, *bp;
 register int32 wlen;

 switch ((byte) np->srep) {
  case SR_VEC:
   wlen = wlen_(np->nwid);
   ap = &(np->nva.wp[2*wlen*__inum]);
   bp = &(ap[wlen]);
   break;
  case SR_SCAL:
   ap = __addrtmp;
   bp = &(__addrtmp[1]);
   ld_scalval_(ap, bp, np->nva.bp);
   break;
  default: __case_terr(__FILE__, __LINE__); return;
 }
 *aap = ap;
 *abp = bp;
}

/*
 * evaluate a question ?: expression
 *
 * SJM 01/14/99 - fixed bug - previously always evaluatged expr 1 and 2
 * that was wrong since only need to do that if selector has x/z bits
 */
extern void __eval_qcol(register struct expr_t *ndp)
{
 struct xstk_t *xspq, *xsp1, *xsp2;

 xspq = __eval_xpr(ndp->lu.x);
 /* case 1: some x bits in selector, need both to be result (widest) wide */
 if (!vval_is0_(xspq->bp, xspq->xslen))
  {
   xsp1 = __eval_xpr(ndp->ru.x->lu.x);
   xsp2 = __eval_xpr(ndp->ru.x->ru.x);
   /* xspq overwritten with x case (i.e. 2 down from top) */
   /* SJM 09/30/03 - select self determined so may or may not be signed */
   /* : operands either both signed or neither signed */
   /* AIV 01/04/07 - was passing the wrong expr parts for has_sign bit */
   lxqcol(xspq, xsp1, xsp2, ndp->szu.xclen, ndp->ru.x->lu.x->has_sign,
    ndp->ru.x->ru.x->has_sign);
   /* pop top 2 arguments leaving result that is always down 2 */
   __pop_xstk();
   __pop_xstk();
   return;
  }
 /* case 2: non x/z selector */
 /* case 2a: non 0 is true - only evaluate first for non x/z selector */
 if (!vval_is0_(xspq->ap, xspq->xslen))
  {
   /* pop selector */
   __pop_xstk();
   /* evaluate expression 1 */ 
   xsp1 = __eval_xpr(ndp->ru.x->lu.x);

   /* need to change width to widest if 1 and 2 differ */
   /* SJM 09/30/03 - widen only but can be sign extend */
   if (xsp1->xslen != ndp->szu.xclen)
    {
     if (ndp->has_sign) __sgn_xtnd_widen(xsp1, ndp->szu.xclen);
     else __sizchg_widen(xsp1, ndp->szu.xclen);
    }
   return;
  }
 /* case 2b: 0 is FALSE - only evaluate first for non x/z selector */
 /* pop selector */
 __pop_xstk();
 /* evaluate expression 2 */ 
 xsp1 = __eval_xpr(ndp->ru.x->ru.x);
 /* need to change width to widest if 1 and 2 differ */

 /* need to change width to widest if 1 and 2 differ */
 /* SJM 09/30/03 - widen only but can be sign extend */
 if (xsp1->xslen < ndp->szu.xclen)
  {
   if (ndp->has_sign) __sgn_xtnd_widen(xsp1, ndp->szu.xclen);
   else __sizchg_widen(xsp1, ndp->szu.xclen);
  }
}

/*
 * eval real ?: both condition and : operands reals
 *
 * SJM 01/14/99 - fixed bug - previously always evaluated expr 1 and 2
 * that was always wrong for reals 
 */
extern void __eval_realrealqcol(register struct expr_t *ndp)
{
 double d1;
 register struct xstk_t *xspq, *xsp;
 register struct expr_t *xp;

 /* know this is real to get here */
 xspq = __eval2_xpr(ndp->lu.x);
 memcpy(&d1, xspq->ap, sizeof(double));

 /* T case - evaluate first */
 if (d1 != 0.0)
  {
   __pop_xstk();
   xp = ndp->ru.x->lu.x;
   xsp = __eval_xpr(xp);
   if (xp->cnvt_to_real)
    {
     d1 = __cnvt_stk_to_real(xsp, (xp->has_sign == 1));
     /* this works because minimum stack every allocated is 8 bytes */
     /* PORTABILITY - stack must always be at least 8 bytes */
     memcpy(xsp->ap, &d1, sizeof(double));
    }
   return;
  }
 /* F case - evaluate 2nd */
 __pop_xstk();
 xp = ndp->ru.x->ru.x;
 xsp = __eval_xpr(xp);
 if (xp->cnvt_to_real)
  {
   d1 = __cnvt_stk_to_real(xsp, (xp->has_sign == 1));
   /* this works because minimum stack every allocated is 8 bytes */

   /* PORTABILITY - stack must always be at least 8 bytes */
   memcpy(xsp->ap, &d1, sizeof(double));
  }
}

/*
 * eval realreg ?: condition real but : operands regs
 *
 * SJM 01/14/99 - fixed bug - previously always evaluated expr 1 and 2
 * now since selector real only evaluate right one
 */
extern void __eval_realregqcol(struct expr_t *ndp)
{
 struct xstk_t *xspq, *xsp;
 double d1;

 xspq = __eval_xpr(ndp->lu.x);
 memcpy(&d1, xspq->ap, sizeof(double));
 /* case 2a: non 0 is true - only evaluate first for non x/z selector */
 if (d1 != 0.0)
  {
   /* pop selector */
   __pop_xstk();
   /* evaluate expression 1 */ 
   xsp = __eval_xpr(ndp->ru.x->lu.x);
   /* need to change width to widest if 1 and 2 differ */

   /* need to change width to widest if 1 and 2 differ */
   /* SJM 09/30/03 - widen only but can be sign extend */
   if (xsp->xslen < ndp->szu.xclen)
    {
     if (ndp->has_sign) __sgn_xtnd_widen(xsp, ndp->szu.xclen);
     else __sizchg_widen(xsp, ndp->szu.xclen);
    }
   return;
  }
 /* case 2b: 0 is FALSE - only evaluate first for non x/z selector */
 /* pop selector */
 __pop_xstk();
 /* evaluate expression 2 */ 
 xsp = __eval_xpr(ndp->ru.x->ru.x);
 /* need to change width to widest if 1 and 2 differ */
 /* SJM 09/30/03 - widen only but can be sign extend */
 if (xsp->xslen < ndp->szu.xclen)
  {
   if (ndp->has_sign) __sgn_xtnd_widen(xsp, ndp->szu.xclen);
   else __sizchg_widen(xsp, ndp->szu.xclen);
  }
}

/*
 * eval regreal ?: condition reg but : operands real
 *
 * SJM 01/14/99 - fixed bug - previously always evaluated expr 1 and 2
 * now only evaluate if selector has x/z bits 
 */
extern void __eval_regrealqcol(register struct expr_t *ndp)
{
 register struct xstk_t *xspq, *xsp;
 register struct expr_t *xp;
 double d1;

 xspq = __eval_xpr(ndp->lu.x);
 /* case 1: selector has x/z bits */
 if (!vval_is0_(xspq->bp, xspq->xslen))
  {
   __sgferr(730,
     "?: operator select condition x/z but values are real - 0.0 returned");
   d1 = 0.0;
   memcpy(xspq->ap, &d1, sizeof(double));
   return;
  }

 /* T (non zero) case */
 if (!vval_is0_(xspq->ap, xspq->xslen))
  {
   __pop_xstk();
   xp = ndp->ru.x->lu.x;
   xsp = __eval_xpr(xp);
   if (xp->cnvt_to_real)
    {
     d1 = __cnvt_stk_to_real(xsp, (xp->has_sign == 1));
     /* this works because minimum stack every allocated is 8 bytes */

     /* PORTABILITY - stack must always be at least 8 bytes */
     memcpy(xsp->ap, &d1, sizeof(double));
    }
   return;
  }
 /* F case */
 __pop_xstk();
 xp = ndp->ru.x->ru.x;
 xsp = __eval_xpr(xp);
 if (xp->cnvt_to_real)
  {
   d1 = __cnvt_stk_to_real(xsp, (xp->has_sign == 1));
   /* this works because minimum stack every allocated is 8 bytes */
   /* PORTABILITY - stack must always be at least 8 bytes */
   memcpy(xsp->ap, &d1, sizeof(double));
  }
}

/*
 * evaluate a qcol x form (at least one x bit) truth table
 *
 * notice this overwrites conditional but works because once know any x's
 * in conditional, then value obtained purely from combination of args.
 * LOOKATME - one word32 form could be more efficient but used for all
 *
 * SJM 09/30/03 - need different widen if signed - : operand either both
 * signed or both no signed
 */
static void lxqcol(register struct xstk_t *xspq, register struct xstk_t *xsp1,
 register struct xstk_t *xsp2, int32 opbits, int32 sel_sign, int32 col_sign)
{
 register int32 wi;
 word32 *resap, *resbp;
 struct xstk_t *tmpq;
 int32 wlen, ubits;

 /* LOOKATME - LRM 2.0 is wrong to match OVIsim any 1 in value is T */ 
 /* any one implies use T */
 /* notice must use qcol word32 width here */
 wlen = wlen_(xspq->xslen);
 for (wi = 0; wi < wlen; wi++)
  { if ((xspq->ap[wi] & ~xspq->bp[wi]) != 0) goto true_has_1bit; }

 wlen = wlen_(opbits);
 
 /* SJM 09/30/03 - widen only but can be sign extend - know : opands same */
 if (xsp1->xslen < opbits)
  {
   /* AIV 01/10/07 - was using col sign for sel part sign */
   if (sel_sign) __sgn_xtnd_widen(xsp1, opbits);
   else __sizchg_widen(xsp1, opbits);
  }
 if (xsp2->xslen < opbits)
  {
   if (col_sign) __sgn_xtnd_widen(xsp2, opbits);
   else __sizchg_widen(xsp2, opbits);
  }

 /* SJM 05/21/04 - if select is wider need to narrow it - the : arms */ 
 /* can only be widened because max width set in return val node */
 if (xspq->xslen > opbits) __narrow_sizchg(xspq, opbits);
 else if (xspq->xslen < opbits)
  {
   if (sel_sign) __sgn_xtnd_widen(xspq, opbits);
   else __sizchg_widen(xspq, opbits);
  }

 resap = xspq->ap; resbp = xspq->bp;
 for (wi = 0; wi < wlen; wi++)
  {
   /* widened already with 0's before using table */
   /* truth table is 0-0 = 0, 1-1 = 1, else x */
   resbp[wi] = xsp2->bp[wi] | xsp1->bp[wi] | (xsp2->ap[wi] ^ xsp1->ap[wi]);
   /* if resbp bit zero, know either both 1 or both 0 */
   resap[wi] = resbp[wi] | xsp1->ap[wi];
  }
 ubits = ubits_(opbits);
 resap[wlen - 1] &= __masktab[ubits];
 resbp[wlen - 1] &= __masktab[ubits];
 return;

true_has_1bit:
 /* T case because at least one bit 1 */
 /* just shuffle pointers to stack regs here */
 tmpq = __xstk[__xspi - 2];
 __xstk[__xspi - 2] = __xstk[__xspi - 1];
 __xstk[__xspi - 1] = tmpq; 
}

/*
 * ROUTINES TO EVALUATE NORMAL UNARY OPERATORS
 */

/*
 * evaluate a unary operator
 * replaces tos expr. value with result
 * for now routine always uses long form, need too routines
 *
 * never need stack conversion here to real because will not be real op
 * if operand not real
 */
static void eval_unary(struct expr_t *ndp)
{
 register word32 op1a, op1b;
 register struct xstk_t *xsp;
 word32 mask;
 int32 ida;
 double d1;

 xsp = __eval2_xpr(ndp->lu.x);
 /* notice double must not be > WBITS (i.e 64 bits no b part - width WBITS) */ 
 if (ndp->szu.xclen > WBITS || xsp->xslen > WBITS)
  {
   eval_wide_unary(ndp, xsp); 
   return;
  }
 op1a = xsp->ap[0];
 op1b = xsp->bp[0]; 

 /* SJM 03/11/02 - beware - this uses fact that op1b same as xsp b part */
 /* operand so if op1b 0, know xsp, bp part also 0 */
 switch ((byte) ndp->optyp) {
  /* both unary and binary but used as unary here */
  case /* + */ PLUS:
   /* plus removed (no op) before here */
   __misc_terr(__FILE__, __LINE__);
   return;
  case /* - */ MINUS:
   /* only if operand value too wide, need mask to narrow */
   if (op1b == 0L)
    {
     if (ndp->has_sign && ndp->lu.x->szu.xclen != WBITS)
      {
       /* SJM 06/01/04 - may need to sign extend operand */
       if ((op1a & (1 << (ndp->lu.x->szu.xclen - 1))) != 0) 
        op1a |= ~(__masktab[ndp->lu.x->szu.xclen]);
       /* AIV 09/25/06 - if the sign bit is on converting to size of expr */
       /* was wrong - since sign extended/mask here just set needed size */
       xsp->xslen = ndp->szu.xclen;
      } 
     /* convert to signed 32 bit then copy back to word32 */
     /* works because narrower than 32 signed extended already */
     ida = (int32) op1a;
     xsp->ap[0] = ((word32) -ida) & __masktab[ndp->szu.xclen];
    }
   else xsp->ap[0] = xsp->bp[0] = __masktab[ndp->szu.xclen];
   return;
  case /* real - */ REALMINUS:
   /* notice double may be on only 4 byte boundary so need to copy and op */
   /* also width of xsp is WBITS - 8 bytes for double - known real */ 
   /* FIXME ??? - this is stupid - should use endianness */
   memcpy(&d1, xsp->ap, sizeof(double));   
   d1 = -d1;
   memcpy(xsp->ap, &d1, sizeof(double));
   return;
  case /* ~ */ BITNOT:
   mask = __masktab[ndp->szu.xclen];
   xsp->ap[0] = (~op1a | op1b) & mask;
   xsp->bp[0] = op1b & mask; 
   return;
  /* notice for reduction and logicals - must set xsp stack width to 1 */ 
  case /* logical ! */ NOT:
   /* know a val unused bits will be 0 */
   if (op1b == 0L) xsp->ap[0] = (xsp->ap[0] == 0L) ? 1L : 0L;
   else xsp->ap[0] = xsp->bp[0] = 1L;
   break;
  case /* logical real ! */ REALNOT:
   /* real only uses x stack a part */
   /* width of xsp is WBITS - 8 bytes for double - known real */ 
   memcpy(&d1, xsp->ap, sizeof(double));
   /* notice overwriting place dp points */
   /* SJM 01/04/99 - had logic backward */
   if (d1 > -EPSILON && d1 < EPSILON) xsp->ap[0] = 1L; else xsp->ap[0] = 0L;
   xsp->bp[0] = 0L;
   break;
  case /* & */ BITREDAND:
   /* width here is just unary operand width since unary self determined */  
   /* must use operand width since result node width will be 1 probably */
   mask = __masktab[xsp->xslen];
   /* if even 1 0 value in any used bit, result is 0 */
   if (op1b == 0L) xsp->ap[0] = (op1a != mask) ? 0L : 1L;
   else if ((op1a | op1b) != mask) xsp->ap[0] = xsp->bp[0] = 0L; 
   else xsp->ap[0] = xsp->bp[0] = 1L;
   break;
  case /* | */ BITREDOR:
   /* here wide will be zero's so no need for width change */
   if (op1b == 0L) xsp->ap[0] = (op1a != 0L) ? 1L : 0L;
   else if ((op1a & ~op1b) != 0L) { xsp->ap[0] = 1L; xsp->bp[0] = 0L; }
   else xsp->ap[0] = xsp->bp[0] = 1L;
   break;
  case /* ^ */ BITREDXOR:
   if (op1b != 0L) { xsp->ap[0] = xsp->bp[0] = 1L; break; } 
   /* notice here any high unused 0's will not effect result */
   xsp->bp[0] = 0L;
   xsp->ap[0] = __wrd_redxor(op1a);
   break;
  case /* ^~ */ REDXNOR:
   /* exec as first the ^ then take opposite of 1 bit */
   /* truth table is: 00 = 1, 01 = 0,  10 = 0, 11 = 1 */   
   /* odd numer of 1 bits value 1, else value 0 */
   if (op1b != 0L) { xsp->ap[0] = xsp->bp[0] = 1L; break; } 
   /* use mask as tmp */
   xsp->ap[0] = !__wrd_redxor(op1a);
   xsp->bp[0] = 0L; 
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 xsp->xslen = 1;
}

/*
 * evaluate a wide unary 
 * notice xsp widened to same width as ndp above here for - and ~
 * but not for reduction and logical
 */
static void eval_wide_unary(register struct expr_t *ndp,
 register struct xstk_t *xsp)
{
 int32 rta, rtb;

 switch ((byte) ndp->optyp) {
  /* both unary and binary but used as unary here */
  case /* + */ PLUS:
   /* unary plus eliminated before here */
   __misc_terr(__FILE__, __LINE__);
   return;
  case /* - */ MINUS:
   /* SJM 05/10/04 FIXME - since sign extended, do not need signed l uminus */
   /* SJM 09/29/03 - change to handle sign extension and separate types */
   if (xsp->xslen > ndp->szu.xclen) __narrow_sizchg(xsp, ndp->szu.xclen);
   else if (xsp->xslen < ndp->szu.xclen)
    {
     if (ndp->has_sign) __sgn_xtnd_widen(xsp, ndp->szu.xclen);
     else __sizchg_widen(xsp, ndp->szu.xclen);
    }

   __luminus(xsp->ap, xsp->bp, ndp->szu.xclen);
   /* must fix since stack exchanged */
   xsp->xslen = ndp->szu.xclen;
   return;
  case /* ~ */ BITNOT:
   /* SJM 05/10/04 FIXME - since sign extended, do not need signed l bitnot */
   /* SJM 09/29/03 - change to handle sign extension and separate types */
   if (xsp->xslen > ndp->szu.xclen) __narrow_sizchg(xsp, ndp->szu.xclen);
   else if (xsp->xslen < ndp->szu.xclen)
    {
     if (ndp->has_sign) __sgn_xtnd_widen(xsp, ndp->szu.xclen);
     else __sizchg_widen(xsp, ndp->szu.xclen);
    }

   __lunbitnot(xsp->ap, xsp->bp, ndp->szu.xclen);
   /* know bit not in place and size changed already */
   return;
  /* notice for logicals and reductions must set width to 1 */
  case /* logical ! */ NOT:
   if (vval_is0_(xsp->bp, xsp->xslen))
    {
     rtb = 0L;
     if (vval_is0_(xsp->ap, xsp->xslen)) rta = 1L; else rta = 0L;
    }
   else rta = rtb = 1L;
   /* SJM 09/30/03 - can use simpler narrow to 1 bit */
   __narrow_to1bit(xsp);
   xsp->ap[0] = rta;
   xsp->bp[0] = rtb;
   break;
  case /* & */ BITREDAND:
   /* this changes tos to 1 bit value */
   __lunredand(&rta, &rtb, xsp->ap, xsp->bp, xsp->xslen);
   /* SJM 09/30/03 - can use simpler narrow to 1 bit */
   __narrow_to1bit(xsp);
   xsp->ap[0] = (word32) rta; 
   xsp->bp[0] = (word32) rtb;
   break;
  case /* | */ BITREDOR:
   __lunredor(&rta, &rtb, xsp->ap, xsp->bp, xsp->xslen);
   /* SJM 09/30/03 - can use simpler narrow to 1 bit */
   __narrow_to1bit(xsp);
   xsp->ap[0] = (word32) rta; 
   xsp->bp[0] = (word32) rtb;
   break;
  case /* ^ */ BITREDXOR:
   __lunredxor(&rta, &rtb, xsp->ap, xsp->bp, xsp->xslen);
   /* SJM 09/30/03 - can use simpler narrow to 1 bit */
   __narrow_to1bit(xsp);
   xsp->ap[0] = (word32) rta; 
   xsp->bp[0] = (word32) rtb;
   break;
  case /* ^~ */ REDXNOR:
   /* truth table is logical not of bit wire reducing */ 
   /* odd numer of 1 bits value 1, else value 0 */
   __lunredxor(&rta, &rtb, xsp->ap, xsp->bp, xsp->xslen);
   /* SJM 09/30/03 - can use simpler narrow to 1 bit */
   __narrow_to1bit(xsp);
   xsp->ap[0] = (word32) rta; 
   xsp->bp[0] = (word32) rtb;
   if (rtb == 0L) xsp->ap[0] = (word32) !rta;
   /* this produces the 1 bit result */
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 xsp->xslen = 1;
}

/*
 * unary bit not - notice this is bit for bit
 */
extern void __lunbitnot(word32 *op1ap, word32 *op1bp, int32 opwid)
{
 register int32 wi;
 int32 wlen;

 wlen = wlen_(opwid);
 for (wi = 0; wi < wlen; wi++)
  {
   op1ap[wi] = (~op1ap[wi] | op1bp[wi]);
   /* b value remains unchanged */
  }
 op1ap[wlen - 1] &= __masktab[ubits_(opwid)];
}

/*
 * unary minus (0 - value)
 * know op1ap and op1bp are just pointers to tos values
 *
 * SJM 09/30/03 - signed just works because 2's complement
 */
extern void __luminus(word32 *op1ap, word32 *op1bp, int32 opbits)
{
 struct xstk_t *xsp0, *xspr;

 if (!vval_is0_(op1bp, opbits))
  { one_allbits_(op1ap, opbits); one_allbits_(op1bp, opbits); }
 else
  {
   /* need real multi-bit subtract */
   push_xstk_(xsp0, opbits);
   zero_allbits_(xsp0->ap, opbits);
   zero_allbits_(xsp0->bp, opbits);

   push_xstk_(xspr, opbits);
   /* result on tos above 2 operands */
   __lsub(xspr->ap, xsp0->ap, op1ap, opbits);
   zero_allbits_(xspr->bp, opbits);
   /* now must adjust tos stack */
   xchg_stk(__xspi - 2, __xspi);
   __pop_xstk();
   __pop_xstk();
  }
}

/*
 * exchange 2 eval. stack locations
 */
static void xchg_stk(int32 xspi1, int32 xspi2)
{
 struct xstk_t *xstmp;

 xstmp = __xstk[xspi1]; __xstk[xspi1] = __xstk[xspi2]; __xstk[xspi2] = xstmp;
}

/*
 * compute reduction xor for 32 bit word32 (or part)
 * this returns 1 bit
 * notice this is 32 bit word32 dependent
 * 
 * FIXME - if processor has instruction for word32 reducing xor should use
 */
extern word32 __wrd_redxor(word32 opa)
{
 register word32 t, rta;

 t = opa;
 t = t ^ (t >> 16);
 t = t ^ (t >> 8);
 t = t ^ (t >> 4);
 t = t ^ (t >> 2);
 rta = (t ^ (t >> 1)) & 1L;
 return(rta);
}

/*
 * compute reduction xor for 64 bit lword (or word64) (or part of lword)
 * this returns 1 bit
 * notice this is 64 bit word32 dependent
 * 
 * FIXME - if processor has instruction for word32 reducing xor should use
 */
extern word64 __lwrd_redxor(word64 opa)
{
 register word64 t, rta;

 t = opa;
 t = t ^ (t >> 32);
 t = t ^ (t >> 16);
 t = t ^ (t >> 8);
 t = t ^ (t >> 4);
 t = t ^ (t >> 2);
 rta = (t ^ (t >> 1)) & 1ULL;
 return(rta);
}

/*
 * ROUTINES FOR WIDE UNARY OPERATORS
 */

/*
 * wide bit reducing and - set tos to 1 bit result
 * if not all 1's, reduction and will turn to 0
 */
extern void __lunredand(int32 *rta, int32 *rtb, word32 *op1ap, word32 *op1bp,
 int32 opwid)
{
 register int32 wi;
 int32 wlen, ubits;

 /* handle non x/z case */
 if (vval_is0_(op1bp, opwid))
  {
   *rtb = 0;
   if (!__vval_is1(op1ap, opwid)) *rta = 0; else *rta = 1;
   return;
  }

 /* if even one 0 => 0, else x, know high op1a and op1b bit 0 */
 wlen = wlen_(opwid);
 ubits = ubits_(opwid);
 if (ubits != 0) wlen--;

 for (wi = 0; wi < wlen; wi++)
  {
   /* if any 0 value bits in either, reducing and will be 0 */
   if ((op1ap[wi] | op1bp[wi]) != ALL1W) { *rtb = *rta = 0; return; }
  }
 if (ubits != 0)
  {
   if ((op1ap[wlen] | op1bp[wlen]) != __masktab[ubits])
    { *rtb = *rta = 0; return; }
  }
 /* did not find 0, must be x */
 *rtb = *rta = 1;
} 

/*
 * wide bit reducing or - set tos to 1 bit result
 * if not all 0's, reduction and will turn to 1
 */
extern void __lunredor(int32 *rta, int32 *rtb, word32 *op1ap, word32 *op1bp,
 int32 opwid)
{
 register int32 wi;
 register word32 rta2, rtb2;

 /* if even 1 1 and no x/z bits, reduction and will turn to 1 */
 if (vval_is0_(op1bp, opwid))
  {
   rtb2 = 0;
   if (!vval_is0_(op1ap, opwid)) rta2 = 1; else rta2 = 0;
   goto done;
  }
 /* if even one 1 => 1, else x, know high op1a and op1b bit 0 */
 /* since know high bits will be 0, no need to handle separately */
 for (wi = 0; wi < wlen_(opwid); wi++)
  {
   /* if a bit is 1 and b bit is 0, have the one needed 1 */
   if ((op1ap[wi] & ~op1bp[wi]) != 0L)
    { rtb2 = 0; rta2 = 1; goto done; }
  }
 /* did not find a 1, must be x */
 rtb2 = rta2 = 1;
done:
 *rta = rta2;
 *rtb = rtb2;
 return;
}

/*
 * wide bit reducing xor - set tos to 1 bit result
 * counts number of 1 bits
 */
extern void __lunredxor(int32 *rta, int32 *rtb, word32 *op1ap, word32 *op1bp,
 int32 opwid)
{
 register int32 wi;
 register word32 rtmp, rtmp2;

 /* if any x/zs, return is x */
 if (!vval_is0_(op1bp, opwid)) { *rta = *rtb = 1; return; }

 /* any unused 0's can be ignored - just produce 0 */
 for (rtmp = 0, wi = 0; wi < wlen_(opwid); wi++)
  {
   /* this returns 1 bit result */ 
   rtmp2 = __wrd_redxor(op1ap[wi]);
   rtmp ^= rtmp2;
  }
 *rta = rtmp; 
 *rtb = 0L;
 return;
}

/*
 * ROUTINES TO EVALUATE NORMAL BINARY OPERATORS
 */

/*
 * evaluate a binary operator
 * know all high (unused) bits set to zero - and left as zero
 * evaluates 2 operands and places result on tos (i.e. stack shrinks by 1)
 *
 * SJM 10/22/03 - the signed narrower than 32 bits consistently wrong
 * because sign bit not in bit 32 as was case when only 32 bit ints
 * could be signed - now either sign extend or use signed magnitude operation
 *
 * notice are tmps that can be changed during evaluation
 */
static void eval_binary(struct expr_t *ndp)
{
 register word32 rta, rtb;
 register word32 op1a, op1b, op2a, op2b, mask;
 int32 tmp1, tmp2, nd_signop, opwid, has_sign;
 double d1, d2;
 struct xstk_t *xsp1, *xsp2;
 struct expr_t *lx, *rx;

 xsp1 = __eval2_xpr(ndp->lu.x);
 xsp2 = __eval2_xpr(ndp->ru.x);

 /* need to separate off wide case */
 /* notice this code depends on real width == W BITS */
 if (ndp->szu.xclen > WBITS || xsp1->xslen > WBITS || xsp2->xslen > WBITS)
  {
   /* this replaces tos 2 values with 1 value */
   /* wide always word32 */
   eval_wide_binary(ndp, xsp1, xsp2);
   return;
  }
 opwid = ndp->szu.xclen;
 /* set during checking - result has sign if < WBITS and not 1 bit */
 /* and one or both operaads have sign */
 if (ndp->has_sign || ndp->rel_ndssign) nd_signop = TRUE;
 else nd_signop = FALSE;

 op1a = xsp1->ap[0]; op1b = xsp1->bp[0];
 op2a = xsp2->ap[0]; op2b = xsp2->bp[0];
 mask = __masktab[ubits_(opwid)];

 /* this is result operator not operands width */
 rta = rtb = 0L;
 switch ((byte) ndp->optyp) {
  case /* + */ PLUS:
   if (op1b == 0L && op2b == 0L)
    {
     rtb = 0;
     /* SJM 09/30/03 - need signed for c signed add (hardware sign xtnd) */
     if (!nd_signop) rta = (op1a + op2a) & mask;
     else
      {
       /* SJM 09/29/04 - but do need to mask if either operand not 32 bits */ 
       if (ndp->lu.x->szu.xclen != WBITS)
        {
         /* complex narrower than 32 bit signed case - sign extend to c int */ 
         if ((op1a & (1 << (xsp1->xslen - 1))) != 0) 
          op1a |= ~(__masktab[xsp1->xslen]);
        } 
       if (ndp->ru.x->szu.xclen != WBITS)
        {
         if ((op2a & (1 << (xsp2->xslen - 1))) != 0) 
          op2a |= ~(__masktab[xsp2->xslen]);
        }
       /* mask even if 32 bits */
       rta = (word32) ((((sword32) op1a) + ((sword32) op2a)) & mask);    
      }
    }
   else rta = rtb = mask;
   break;
  case /* + real */ REALPLUS:
   /* is it portable to pass 1 bit bit field? */
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));

   /* notice never a size change since both must be real */
   d1 += d2;
   /* this works because minimum stack every allocated is 8 bytes */
   /* PORTABILITY - stack must always be at least 8 bytes */
   memcpy(xsp1->ap, &d1, sizeof(double));
   __pop_xstk();
   return;
  case  /* - */ MINUS:
   if (op1b == 0L && op2b == 0L)
    {
     rtb = 0L;
     /* SJM 09/30/03 - need signed for c signed - (hardware sign xtnd) */
     if (!nd_signop)
      {
       rta = op1a + op2a;
       rta = (op1a - op2a) & mask;
      }
     /* since know 32 bits, no need to mask */
     else
      {
       /* SJM 09/29/04 - but do need to mask if either operand not 32 bits */ 
       if (ndp->lu.x->szu.xclen != WBITS)
        {
         /* complex narrower than 32 bit signed case - sign extend to c int */ 
         if ((op1a & (1 << (xsp1->xslen - 1))) != 0) 
          op1a |= ~(__masktab[xsp1->xslen]);
        }
       if (ndp->ru.x->szu.xclen != WBITS)
        {
         if ((op2a & (1 << (xsp2->xslen - 1))) != 0) 
          op2a |= ~(__masktab[xsp2->xslen]);
        }
       rta = (word32) ((((sword32) op1a) - ((sword32) op2a)) & mask);    
      }
    }
   else rta = rtb = mask;
   break;
  case /* - real */ REALMINUS:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   d1 -= d2;
   /* notice never a size change since both must be real */
   memcpy(xsp1->ap, &d1, sizeof(double));
   __pop_xstk();
   return;
  case /* * */ TIMES:
   if (op1b != 0L || op2b != 0L) rta = rtb = mask;
   else
    {
     /* SJM 09/30/03 - need ints for c signed op to work */
     if (!nd_signop) rta = (op1a * op2a) & mask;
     /* never need to mask for 32 bits */
     /* SJM 09/29/04 - but do need to mask if either operand not 32 bits */ 
     else if (ndp->lu.x->szu.xclen == WBITS && ndp->ru.x->szu.xclen == WBITS)
      rta = (word32) (((sword32) op1a) * ((sword32) op2a));  
     else
      {
       /* SJM 10/22/03 - LOOKATME - think this must use sign/magnitude */
       /* complex narrower than 32 bit signed case */ 
       has_sign = FALSE; 
       /* 2's complement makes positive if needed */
       if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
        {
         /* since c - of cast to int can only handle 32 bit ints, sign xtnd */
         op1a |= ~(__masktab[xsp1->xslen]);
         op1a = (word32) (-((sword32) op1a)); 
         has_sign = TRUE;
        }
       if ((op2a & (1 << (xsp2->xslen - 1))) != 0)
        {
         /* since c - of cast to int can only handle 32 bit ints, sign xtnd */
         op2a |= ~(__masktab[xsp2->xslen]);
         op2a = (word32) (-((sword32) op2a));
         has_sign = !has_sign;
        }

       /* know op1a and op2a positive */
       rta = op1a * op2a;
       if (has_sign) rta = (word32) (-((sword32) rta));
       /* must mask so product fits in widest operand size */  
       rta &= mask;
      }
     rtb = 0;
    } 
   break;
  case /* * real */ REALTIMES:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));

   /* notice never a size change since both must be real */
   d1 *= d2;
   memcpy(xsp1->ap, &d1, sizeof(double));
   __pop_xstk();
   return;
  case /* / */ DIV:
   if (op1b != 0L || op2b != 0L || op2a == 0L) rta = rtb = mask;
   /* case 1: unsigned */
   else if (!nd_signop) rta = op1a / op2a;
   /* SJM 09/29/04 - but do need to mask if either operand not 32 bits */ 
   else if (ndp->lu.x->szu.xclen == WBITS && ndp->ru.x->szu.xclen == WBITS)
    {
     /* case 2 signed but int32 so can use c casts */
     rta = (word32) ((sword32) op1a / (sword32) op2a);
     rtb = 0L;
     /* SJM 05/13/04 - no need to mask since know WBITS */
    }
   else 
    {
     /* SJM 10/22/03 - must extract signs (sign of result from 1st) */
     /* and do operation word32 and then put back sign */
     if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
      {
       /* SJM 05/13/04 - must sign extend to WBITS int32 size */
       op1a = op1a | ~(__masktab[xsp1->xslen]);
       op1a = (word32) (-((sword32) op1a)); 
       has_sign = TRUE;
      }
     else has_sign = FALSE; 

     /* for mod, first operand determines sign of result */
     if ((op2a & (1 <<( xsp2->xslen - 1))) != 0)
      {
       /* SJM 05/13/04 - must sign extend to WBITS int32 size */
       op2a = op2a | ~(__masktab[xsp2->xslen]);
       op2a = (word32) (-((sword32) op2a)); 
       /* sign combination rules for div same as mult */
       has_sign = !has_sign;
      }

     /* know op1a and op2a positive */
     rta = op1a / op2a;
     /* if result signed, first comp WBITS signed - */
     if (has_sign) rta = (word32) (-((sword32) rta));
     rta &= mask;
     rtb = 0L;
    }
   break;
  case /* * real */ REALDIV:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));

   /* notice never a size change since both must be real */
   d1 /= d2;
   memcpy(xsp1->ap, &d1, sizeof(double));
   __pop_xstk();
   return;
  case /* % */ MOD:
   if (op1b != 0L || op2b != 0L || op2a == 0L) rta = rtb = mask;
   /* case 1: unsigned */
   else if (!nd_signop) rta = op1a % op2a;
   /* SJM 09/29/04 - but do need to mask if either operand not 32 bits */ 
   else if (ndp->lu.x->szu.xclen == WBITS && ndp->ru.x->szu.xclen == WBITS)
    {
     /* case 2 signed but int32 so can use c casts */
     /* case 2 signed but int32 so can use c casts */
     rta = (word32) ((sword32) op1a % (sword32) op2a);
     rtb = 0L;
     /* think value narrower but needed for signed */
     rta &= mask;
    }
   else 
    {
     /* SJM 10/22/03 - must extract signs (sign of result from 1st) */
     /* and do operation word32 and then put back sign */
     if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
      {
       /* SJM 05/13/04 - must sign extend to WBITS int32 size */
       op1a = op1a | ~(__masktab[xsp1->xslen]);
       /* AIV 10/12/06 - forgot to do the sign cast */
       op1a = (word32) (-((sword32) op1a)); 
       has_sign = TRUE;
      }
     else has_sign = FALSE; 

     /* for mod, first operand determines sign of result */
     if ((op2a & (1 <<( xsp2->xslen - 1))) != 0)
      {
       /* SJM 05/13/04 - must sign extend to WBITS int32 size */
       op2a = op2a | ~(__masktab[xsp2->xslen]);
       op2a = (word32) (-((sword32) op2a)); 
      }

     /* know op1a and op2a positive */
     rta = op1a % op2a;
     if (has_sign) rta = (word32) (-((sword32) rta));
     rta &= mask;
     rtb = 0L;
    }
   break;
  case /* & */ BITREDAND:
   /* idea is if both op1b and op2b 1 (x), need rta to be 1 for x result */
   if ((op1b | op2b) == 0L) { rtb = 0L; rta = (op1a & op2a); }
   else
    {
     rta = (op1a | op1b) & (op2a | op2b);
     rtb = rta & (op2b | op1b);
    }
   break;
  case /* | */ BITREDOR:
   if ((op1b | op2b) == 0L) { rtb = 0L; rta = (op1a | op2a); }
   else
    {
     rtb = op2b ^ op1b ^ ((op1a | op1b) & (op2b | (op2a & op1b)));
     rta = rtb | op2a | op1a;
    }
   break;
  case /* ^ */ BITREDXOR:
   /* know same width so high non 0 never possible */
   if ((op1b | op2b) == 0L) { rtb = 0L; rta = (op1a ^ op2a); }
   else { rtb = op1b | op2b; rta = rtb | (op1a ^ op2a);  }
   break;
  case /* not ^ */ REDXNOR:
   /* since same length, xor 0 0 is 1, not of 1 is 0 so works */
   if ((op1b | op2b) == 0L) 
    { rtb = 0L; rta = ~(op1a ^ op2a) & mask; }
   else
    /* must mask here because 0 xnor 0 is 1 for high unused bits */
    { rtb = op1b | op2b; rta = (rtb | ~(op1a ^ op2a)) & mask; }
   break;
  case /* >= */ RELGE:
   if ((op1b | op2b) == 0L)
    {
     rtb = 0L;
     /* C result for true is always 1 */
     if (nd_signop)
      {
       /* SJM 10/22/03 - easiest here to just sign extend - does it work? */
       /* SJM 05/13/04 - width is opand not result width */
       if (xsp1->xslen < WBITS)
        {
         if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
          op1a |= ~(__masktab[xsp1->xslen]);
        }
       if (xsp2->xslen < WBITS)
        {
         if ((op2a & (1 << (xsp2->xslen - 1))) != 0)
          op2a |= ~(__masktab[xsp2->xslen]);
        }
       rta = (word32) (((sword32) op1a) >= ((sword32) op2a));
      }
     else rta = op1a >= op2a;
    }
   else rta = rtb = 1L;
   break;
  case /* >= real */ REALRELGE:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));

   rta = (d1 >= d2) ? 1L : 0L;
   rtb = 0L; 
   break;
  case /* > */ RELGT:
   if ((op1b | op2b)== 0L)
    {
     rtb = 0L;
     if (nd_signop)
      {
       /* SJM 10/22/03 - easiest here to just sign extend - does it work? */
       /* SJM 05/13/04 - width is opand not result width */
       if (xsp1->xslen < WBITS)
        {
         if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
          op1a |= ~(__masktab[xsp1->xslen]);
        }
       if (xsp2->xslen < WBITS)
        {
         if ((op2a & (1 << (xsp2->xslen - 1))) != 0)
          op2a |= ~(__masktab[xsp2->xslen]);
        }
       rta = (word32) (((sword32) op1a) > ((sword32) op2a));
      }
     else rta = op1a > op2a;
    }
   else rta = rtb = 1L;
   break;
  case /* > real */ REALRELGT:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));
   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   rta = (d1 > d2) ? 1L : 0L;
   rtb = 0L; 
   break;
  case /* <= */ RELLE:
   if ((op1b | op2b) == 0L)
    {
     rtb = 0L;
     if (nd_signop)
      { 
       /* SJM 10/22/03 - easiest here to just sign extend - does it work? */
       /* SJM 05/13/04 - width is opand not result width */
       if (xsp1->xslen < WBITS)
        {
         if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
          op1a |= ~(__masktab[xsp1->xslen]);
        }
       if (xsp2->xslen < WBITS)
        {
         if ((op2a & (1 << (xsp2->xslen - 1))) != 0)
          op2a |= ~(__masktab[xsp2->xslen]);
        }
       rta = (word32) (((sword32) op1a) <= ((sword32) op2a));
      }
     else rta = op1a <= op2a;
    }
   else rta = rtb = 1L;
   break;
  case /* <= real */ REALRELLE:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));
   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   rta = (d1 <= d2) ? 1L : 0L;
   rtb = 0L; 
   break;
  case /* < */ RELLT:
   if ((op1b | op2b) == 0L)
    {
     rtb = 0L;
     if (nd_signop)
      {
       /* SJM 10/22/03 - easiest here to just sign extend - does it work? */
       /* SJM 05/13/04 - width is opand not result width */
       if (xsp1->xslen < WBITS)
        {
         if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
          op1a |= ~(__masktab[xsp1->xslen]);
        }
       if (xsp2->xslen < WBITS)
        {
         if ((op2a & (1 << (xsp2->xslen - 1))) != 0)
          op2a |= ~(__masktab[xsp2->xslen]);
        }
       rta = (word32) (((sword32) op1a) < ((sword32) op2a));
      } 
     else rta = op1a < op2a;
    }
   else rta = rtb = 1L;
   break;
  case /* < real */ REALRELLT:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   /* notice never a size change since both must be real */
   rta = (d1 < d2) ? 1L : 0L;
   rtb = 0L; 
   break;
  case /* != */ RELNEQ:

   if ((op1b | op2b) == 0L)
    {
     if (nd_signop)
      {
       /* SJM 05/13/04 - width is opand not result width */
       if (xsp1->xslen < WBITS)
        {
         if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
          op1a |= ~(__masktab[xsp1->xslen]);
        }
       if (xsp2->xslen < WBITS)
        {
         if ((op2a & (1 << (xsp2->xslen - 1))) != 0)
          op2a |= ~(__masktab[xsp2->xslen]);
        }
      }
     rtb = 0L;
     rta = (op1a != op2a);
     break;
    }

   /* SJM 06/16/00 - was wrong for x cases because if non equal where both */
   /* operands non x/z, should be 0 not unknown */
   /* new algorithm - if compare with a parts all x/z bits set to 1 */
   /* not equal then x/z bits can not effect not equal result */
   /* know at least one bit x/z to get here */

   if (nd_signop)
    {
     /* SJM 05/13/04 - width is opand not result width */
     if (xsp1->xslen < WBITS)
      {
       if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
        op1a |= ~(__masktab[xsp1->xslen]);
       /* if any x/z bits must x/z extned if signed */
       if ((op1b & (1 << (xsp1->xslen - 1))) != 0)
        op1b |= ~(__masktab[xsp1->xslen]);
      }
     if (xsp2->xslen < WBITS)
      {
       if ((op2a & (1 << (xsp2->xslen - 1))) != 0)
        op2a |= ~(__masktab[xsp2->xslen]);
       if ((op2b & (1 << (xsp2->xslen - 1))) != 0)
        op2b |= ~(__masktab[xsp2->xslen]);
      }
    }
   if ((op1a | (op1b | op2b)) != (op2a | (op1b | op2b)))
    { rtb = 0L; rta = 1L; }
   else rta = rtb = 1L;
   break;
  case /* != real */ REALRELNEQ:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));

   /* notice never a size change since both must be real */
   /* this is ieee float point dependent */
   rta = (fabs(d1 - d2) >= EPSILON) ? 1L : 0L;
   rtb = 0L; 
   break;
  case /* == */ RELEQ:
   /* relation true in C is always 1 */
   if ((op1b | op2b) == 0L)
    {
     if (nd_signop)
      {
       /* SJM 05/13/04 - width is opand not result width */
       if (xsp1->xslen < WBITS)
        {
         if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
          op1a |= ~(__masktab[xsp1->xslen]);
        }
       if (xsp2->xslen < WBITS)
        {
         if ((op2a & (1 << (xsp2->xslen - 1))) != 0)
          op2a |= ~(__masktab[xsp2->xslen]);
        }
      }
     rtb = 0L;
     rta = (op1a == op2a);
     break;
    }

   /* SJM 06/16/00 - wrong for x cases because if non equal where both */
   /* operands non x/z, should be 0 not unknown */
   /* new algorithm - if compare with a parts all x/z bits set to 1 */
   /* not equal then x/z bits can not effect not equal result */
   /* know at least one bit x/z to get here */

   if (nd_signop)
    {
     /* SJM 05/13/04 - width is opand not result width */
     if (xsp1->xslen < WBITS)
      {
       if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
        op1a |= ~(__masktab[xsp1->xslen]);
       /* if any x/z bits must x/z extned if signed */
       if ((op1b & (1 << (xsp1->xslen - 1))) != 0)
        op1b |= ~(__masktab[xsp1->xslen]);
      }
     if (xsp2->xslen < WBITS)
      {
       if ((op2a & (1 << (xsp2->xslen - 1))) != 0)
        op2a |= ~(__masktab[xsp2->xslen]);
       if ((op2b & (1 << (xsp2->xslen - 1))) != 0)
        op2b |= ~(__masktab[xsp2->xslen]);
      }
    }

   if ((op1a | (op1b | op2b)) != (op2a | (op1b | op2b)))
    { rtb = 0L; rta = 0L; }
   else rta = rtb = 1L;
   break;
  case /* == real */ REALRELEQ:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));
   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   /* notice never a size change since both must be real */
   /* this is ieee float point dependent */
   rta = (fabs(d1 - d2) >= EPSILON) ? 0L : 1L;
   rtb = 0L; 
   break;
  case /* === */ RELCEQ:

   if (nd_signop)
    {
     /* SJM 05/13/04 - width is opand not result width */
     if (xsp1->xslen < WBITS)
      {
       if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
        op1a |= ~(__masktab[xsp1->xslen]);
       /* if any x/z bits must x/z extned if signed */
       if ((op1b & (1 << (xsp1->xslen - 1))) != 0)
        op1b |= ~(__masktab[xsp1->xslen]);
      }
     if (xsp2->xslen < WBITS)
      {
       if ((op2a & (1 << (xsp2->xslen - 1))) != 0)
        op2a |= ~(__masktab[xsp2->xslen]);
       if ((op2b & (1 << (xsp2->xslen - 1))) != 0)
        op2b |= ~(__masktab[xsp2->xslen]);
      }
    }
   rtb = 0L;
   /* works without masking since semantics requires 0 extending */
   rta = (0L == ((op1a ^ op2a) | (op1b ^ op2b)));
   break;
  case /* !== */ RELCNEQ:

   if (nd_signop) 
    {
     /* SJM 05/13/04 - width is opand not result width */
     if (xsp1->xslen < WBITS)
      {
       if ((op1a & (1 << (xsp1->xslen - 1))) != 0)
        op1a |= ~(__masktab[xsp1->xslen]);
       /* if any x/z bits must x/z extned if signed */
       if ((op1b & (1 << (xsp1->xslen - 1))) != 0)
        op1b |= ~(__masktab[xsp1->xslen]);
      }
     if (xsp2->xslen < WBITS)
      {
       if ((op2a & (1 << (xsp2->xslen - 1))) != 0)
        op2a |= ~(__masktab[xsp2->xslen]);
       if ((op2b & (1 << (xsp2->xslen - 1))) != 0)
        op2b |= ~(__masktab[xsp2->xslen]);
      }
    }
   rtb = 0L;
   /* works without masking since semantics requires 0 extending */
   rta = (0L != ((op1a ^ op2a) | (op1b ^ op2b)));
   break;
  case /* && */ BOOLAND:
   /* notice this complicated algorithm is needed because if a bit */
   /* is some position is 0 other bit does not matter - i.e. this is */
   /* really 32 bool ands */
   rtb = 0L;
   tmp1 = cvt_wrdbool_(op1a, op1b);
   if (tmp1 == 0) { rta = 0L; break; }
   tmp2 = cvt_wrdbool_(op2a, op2b);
   if (tmp2 == 0) { rta = 0L; break; }
   if (tmp1 == 1 && tmp2 == 1) rta = 1L; else rta = rtb = 1L;
   break;
  case /* && real */ REALBOOLAND:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));
   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   /* notice never a size change since both must be real */
   rta = (d1 != 0.0 && d2 != 0.0) ? 1L : 0L;   
   rtb = 0L; 
   break;
  case /* || */ BOOLOR:
   rtb = 0L;
   tmp1 = cvt_wrdbool_(op1a, op1b);
   if (tmp1 == 1) { rta = 1L; break; }
   tmp2 = cvt_wrdbool_(op2a, op2b);
   if (tmp2 == 1) { rta = 1L; break; }
   /* if not both 0, some bit x&x or x&1 */
   if (tmp1 == 0 && tmp2 == 0) rta = 0L; else rta = rtb = 1L;
   break;
  case /* && real */ REALBOOLOR:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));
   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));

   /* notice never a size change since both must be real */
   rta = (d1 != 0.0 || d2 != 0.0) ? 1L : 0L;   
   rtb = 0L; 
   break;
  case /* << */ SHIFTL:
  case /* <<< */ ASHIFTL:
   /* if shift amt x/z, result is 0 */ 
   if (op2b != 0L) rtb = rta = mask;
   /* if shift length wider than op1, result is 0 */
   /* 2nd shift width operand is interpreted as range index (word32) */
   /* AIV 03/14/06 - shift >= set to zero */
   else if (op2a >= (word32) opwid) rtb = rta = 0L;
   else
    {
     if (nd_signop && xsp1->xslen < ndp->szu.xclen)
      {
       __sgn_xtnd_wrd(xsp1, ndp->szu.xclen);
       op1a = xsp1->ap[0]; 
       op1b = xsp1->bp[0]; 
      }
     rtb = (op1b << op2a) & mask;
     rta = (op1a << op2a) & mask;
    }
   break;
  case /* >> */ SHIFTR:
   /* SJM 09/30/03 - logical shift right stays same even if sign bit 1 */
   /* if shift amt x/z, result is 0 */ 
   if (op2b != 0L) rtb = rta = mask;
   /* AIV 03/14/06 - shift >= set to zero */
   else if (op2a >= (word32) ndp->szu.xclen)
    {
     /* if shift length wider than op1, result is 0 */
     /* 2nd shift width operand is interpreted as range index (word32) */
     rtb = rta = 0L;
    }
   else
    {
     /* notice no need to mask since 0's injected into high bits */
     rtb = op1b >> op2a;
     rta = op1a >> op2a;
     }
   break;
  case /* >>> */ ASHIFTR:
   /* SJM 09/30/03 - arithmetic shift right inserts sign bit if on not 0 */
   /* if shift amt x/z, result is x */ 
   if (op2b != 0L)
    {
     rtb = rta = mask;
    }
   /* AIV 03/14/06 - shift >= set to zero */
   else if (op2a >= (word32) ndp->szu.xclen)
    { 
     /* 2nd shift width operand is interpreted as range index (word32) */
     /* notice if word32, no sign bit */
     if (nd_signop)
      {
       if ((op1a & (1 << (ndp->szu.xclen - 1))) != 0)
        {
         /* since shift amount wider than var, if sign bit on */ 
         /* 1's shifted into each bit position, else 0 */
         rta = mask;
         rtb = 0;
        }
       else rta = rtb = 0;
      }
     else rtb = rta = 0L;
    }
   else
    {
     /* notice no need to mask since 0's injected into high bits */
     if (nd_signop)
      {
       /* SJM 05/10/04 - could use c signed arithmetic shift for WBITS wide */
       /* SJM for word32 arithmetic right shift use c arithmetic shift */
       /* AIV 06/02/05 - if shift amt is wrong for 0 - don't mask */
       if (op2a != 0 && (op1a & (1 << (ndp->szu.xclen - 1))) != 0)
        {
         /* first shift as if 0 bits then or in the bits shifted in from */
         /* injected sign bits */
         rta = (word32) (op1a >> op2a);
         rtb = (word32) (op1b >> op2a);
         rta |= (__masktab[op2a] << (ndp->szu.xclen - op2a));
        }
       else
        {
         /* if sign bit off - same as logical right shift */
         rta = (word32) (op1a >> op2a);
         rtb = (word32) (op1b >> op2a);
        }
      }
     else
      {
       rtb = op1b >> op2a;
       rta = op1a >> op2a;
      }
    }
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 /* DB remove after debugged ---
 if ((rta & ~mask) != 0L || (rtb & ~mask) != 0L)
  {
   __pv_warn(528,
    "INTERNAL - binary op truncation wrong - width %d, av=%lx,bv=%lx",
    ndp->szu.xclen, rta, rtb);
  }
 --- */
 xsp1->ap[0] = rta;
 xsp1->bp[0] = rtb;
 xsp1->xslen = ndp->szu.xclen;
 __pop_xstk();
}

/*
 * ROUTINES FOR WIDE BINARY OPERATORS
 */

/*
 * evaluate binary operators where at least 1 operand wider than 32
 * what about 1 extension here and in <32 case
 * notice operation must be done in wider of operands and then copied
 * if needed
 *
 * this expects 2 operands on tos and leaves 1 result on tos
 * xsp2 is above xsp1 and arith routines called from here need 3 on stack
 * wider than 32 never signed - no checking
 */
static void eval_wide_binary(struct expr_t *ndp, register struct xstk_t *xsp1,
 register struct xstk_t *xsp2)
{
 register word32 rta, rtb;
 int32 isxz, cmpval, tmp1, tmp2, nd_signop;
 word32 shiftamt;
 struct xstk_t *xspr;
 struct expr_t *lx, *rx;
 double d1, d2;

 if (ndp->has_sign || ndp->rel_ndssign) nd_signop = TRUE;
 else nd_signop = FALSE;

 /* impossible for both operands <32 but result > 32 */
 switch ((byte) ndp->optyp) {
  case /* + */ PLUS:
  case /* - */ MINUS:
  case /* * */ TIMES:
  case /* / */ DIV:
  case /* % */ MOD:
   /* SJM 09/30/03 - change to handle sign extension and separate types */
   if (xsp1->xslen > ndp->szu.xclen) __narrow_sizchg(xsp1, ndp->szu.xclen);
   else if (xsp1->xslen < ndp->szu.xclen)
    {
     /* if any signed all will be */
     if (ndp->has_sign) __sgn_xtnd_widen(xsp1, ndp->szu.xclen);
     else __sizchg_widen(xsp1, ndp->szu.xclen);
    }
   if (xsp2->xslen > ndp->szu.xclen) __narrow_sizchg(xsp2, ndp->szu.xclen);
   else if (xsp2->xslen < ndp->szu.xclen)
    {
     /* if any signed all will be */
     if (ndp->has_sign) __sgn_xtnd_widen(xsp2, ndp->szu.xclen);
     else __sizchg_widen(xsp2, ndp->szu.xclen);
    }

   if (__set_binxresult(xsp1->ap, xsp1->bp, xsp1->bp, xsp2->bp,
     ndp->szu.xclen))
    {
     __pop_xstk();
     return;
    }

   /* for 0 divisor, result is x */
   if ((ndp->optyp == DIV || ndp->optyp == MOD)
    && vval_is0_(xsp2->ap, xsp2->xslen))
    {
     one_allbits_(xsp1->ap, xsp1->xslen);
     one_allbits_(xsp1->bp, xsp1->xslen);
     __pop_xstk();
     return;
    }
   /* in routine will fill all of xspr with value - no need to init */
   push_xstk_(xspr, ndp->szu.xclen);
   /* add/sub to accumulator */
   /* SJM 09/30/03 - since using sign mult and div others 2 complement work */
   switch ((byte) ndp->optyp) {
    case /* + */ PLUS:
     __ladd(xspr->ap, xsp1->ap, xsp2->ap, xspr->xslen);
     break;
    case /* - */ MINUS:
     __lsub(xspr->ap, xsp1->ap, xsp2->ap, xspr->xslen);
     break;
    case /* * */ TIMES:
     /* SJM 09/30/03 - need wapper for signed since wide needs positive */
     if (ndp->has_sign)
      __sgn_lmult(xspr->ap, xsp1->ap, xsp2->ap, xspr->xslen);
     else __lmult(xspr->ap, xsp1->ap, xsp2->ap, xspr->xslen);
     break;
    case /* / */ DIV:
     /* SJM 09/30/03 - need wapper for signed since wide needs positive */
     if (ndp->has_sign)
      __sgn_ldivmod(xspr->ap, xsp1->ap, xsp2->ap, xspr->xslen, TRUE);
     else __ldivmod(xspr->ap, xsp1->ap, xsp2->ap, xspr->xslen, TRUE);
     break;
    case /* % */ MOD:
     /* SJM 09/30/03 - need wapper for signed since wide needs positive */
     if (ndp->has_sign)
      __sgn_ldivmod(xspr->ap, xsp1->ap, xsp2->ap, xspr->xslen, FALSE);
      else __ldivmod(xspr->ap, xsp1->ap, xsp2->ap, xspr->xslen, FALSE);
     break;
    default: __case_terr(__FILE__, __LINE__);
   }
   /* zero result b value */
   zero_allbits_(xspr->bp, xspr->xslen);
   /* move result down 2 and then pop top 2 (old tmp result and op2) */
   xchg_stk(__xspi - 2, __xspi);
   __pop_xstk();
   __pop_xstk();
   break;
  case /* << */ SHIFTL:
  case /* >>> */ ASHIFTL:
  case /* >> */ SHIFTR:
   /* this replaces top 2 args with shifted result replacing 2nd down */
   /* if shift width has any x/z's, even if will be truncated, result x */
   /* need to widen from context before shift - need overflow bits */

   /* SJM 09/29/03 - change to handle sign extension but left arithmetic */
   /* shift same as left logical shift (only change is signed widening) */
   if (xsp1->xslen > ndp->szu.xclen) __narrow_sizchg(xsp1, ndp->szu.xclen);
   else if (xsp1->xslen < ndp->szu.xclen)
    {
     if (ndp->has_sign) __sgn_xtnd_widen(xsp1, ndp->szu.xclen);
     else __sizchg_widen(xsp1, ndp->szu.xclen);
    }

   if (!vval_is0_(xsp2->bp, xsp2->xslen))
    {
     one_allbits_(xsp1->ap, xsp1->xslen);
     one_allbits_(xsp1->bp, xsp1->xslen);
    }
   else
    {
     /* if op value is 0 or shift amount wider than op */
     /* SJM 12/28/98 - this was wrongly checking first word32 of long */  
     /* SJM 03/28/03 - for shift of case with only z's in op1 was wrong */
     /* because if a part 0 but b part 1 (z in val) wrongly setting to 0 */ 
     if ((vval_is0_(xsp1->ap, xsp1->xslen)
      && vval_is0_(xsp1->bp, xsp1->xslen)) 
      || (xsp2->xslen > WBITS && !vval_is0_(&(xsp2->ap[1]),
       xsp2->xslen - WBITS)) || xsp2->ap[0] >= (word32) xsp1->xslen) 
       memset(xsp1->ap, 0, 2*WRDBYTES*wlen_(xsp1->xslen)); 
     else
      {
       if ((shiftamt = xsp2->ap[0]) != 0) 
        { 
         if (vval_is0_(xsp1->bp, xsp1->xslen)) isxz = FALSE;
         else isxz = TRUE;
         if (ndp->optyp != SHIFTR)
          {
           __mwlshift(xsp1->ap, shiftamt, xsp1->xslen);
           if (isxz) __mwlshift(xsp1->bp, shiftamt, xsp1->xslen);
          }
         else
          {
           __mwrshift(xsp1->ap, shiftamt, xsp1->xslen);
           if (isxz) __mwrshift(xsp1->bp, shiftamt, xsp1->xslen);
          }
        }   
      }
    }
   __pop_xstk();
   break;
  case /* >>> */ ASHIFTR:
   /* SJM 05/11/04 - split arithmetic right shift off */ 
   /* main different is that if sign bit on, need to shift in 1's for both */
   /* a and b parts */
   /* SJM 05/11/04 - notice that shift amount always treated as word32, */
   /* i.e. no minus opposite direction shifts */
   if (!vval_is0_(xsp2->bp, xsp2->xslen))
    {
     one_allbits_(xsp1->ap, xsp1->xslen);
     one_allbits_(xsp1->bp, xsp1->xslen);
     goto ashift_pop; 
    }
   if (vval_is0_(xsp1->ap, xsp1->xslen) && vval_is0_(xsp1->bp, xsp1->xslen)) 
    {
     /* opand 1 value 0 - shift can't change */
     memset(xsp1->ap, 0, 2*WRDBYTES*wlen_(xsp1->xslen)); 
     goto ashift_pop; 
    }
   if ((xsp2->xslen > WBITS && !vval_is0_(&(xsp2->ap[1]),
    xsp2->xslen - WBITS)) || xsp2->ap[0] >= (word32) xsp1->xslen) 
    {
     int32 bi, wlen;

     /* shift amount wider than value */
     bi = get_bofs_(xsp1->xslen);
     wlen = wlen_(xsp1->xslen);
       
     /* SJM 06/20/04 - if right ashift opand word32 - no sign extend */
     if (ndp->has_sign && (xsp1->ap[wlen - 1] & (1 << bi)) != 0)
      {
       /* since shift amount wider than var, if sign bit on */ 
       /* 1's shifted into each bit position, i.e. set all bits to 1 */
       one_allbits_(xsp1->ap, xsp1->xslen);
      }
     else memset(xsp1->ap, 0, 2*WRDBYTES*wlen); 

     /* if b part high bit on, all bits become x/z */
     if (ndp->has_sign && (xsp1->bp[wlen - 1] & (1 << bi)) != 0)
      {
       one_allbits_(xsp1->bp, xsp1->xslen);
      }
     else memset(xsp1->ap, 0, 2*WRDBYTES*wlen); 
     goto ashift_pop; 
    }
   if ((shiftamt = xsp2->ap[0]) != 0) 
    { 
     if (vval_is0_(xsp1->bp, xsp1->xslen)) isxz = FALSE;
     else isxz = TRUE;

     if (nd_signop)
      {
       __arith_mwrshift(xsp1->ap, shiftamt, xsp1->xslen);
       if (isxz) __arith_mwrshift(xsp1->bp, shiftamt, xsp1->xslen);
      }
     else
      {
       /* arithmetic right shift for word32 same as logical */
       __mwrshift(xsp1->ap, shiftamt, xsp1->xslen);
       if (isxz) __mwrshift(xsp1->bp, shiftamt, xsp1->xslen);
      }
    }
ashift_pop:
   __pop_xstk();
   break;
  /* binary of these is bit by bit not reducing and ndp width is needed */
  case /* & */ BITREDAND:
   /* SJM 09/29/03 - change to handle sign extension and separate types */
   /* AIV 10/13/06 - need to ignore sign here */
   if (xsp1->xslen > ndp->szu.xclen) __narrow_sizchg(xsp1, ndp->szu.xclen);
   else if (xsp1->xslen < ndp->szu.xclen) __sizchg_widen(xsp1, ndp->szu.xclen);

   if (xsp2->xslen > ndp->szu.xclen) __narrow_sizchg(xsp2, ndp->szu.xclen);
   else if (xsp2->xslen < ndp->szu.xclen) __sizchg_widen(xsp2, ndp->szu.xclen);

   __lbitand(xsp1->ap, xsp1->bp, xsp2->ap, xsp2->bp, xsp1->xslen);
   __pop_xstk();
   break;
  case /* | */ BITREDOR:
   /* SJM 09/29/03 - change to handle sign extension and separate types */
   /* AIV 10/13/06 - need to ignore sign here */
   if (xsp1->xslen > ndp->szu.xclen) __narrow_sizchg(xsp1, ndp->szu.xclen);
   else if (xsp1->xslen < ndp->szu.xclen) __sizchg_widen(xsp1, ndp->szu.xclen);

   if (xsp2->xslen > ndp->szu.xclen) __narrow_sizchg(xsp2, ndp->szu.xclen);
   else if (xsp2->xslen < ndp->szu.xclen) __sizchg_widen(xsp2, ndp->szu.xclen);

   __lbitor(xsp1->ap, xsp1->bp, xsp2->ap, xsp2->bp, xsp1->xslen);
   __pop_xstk();
   break;
  case /* ^ */ BITREDXOR:
   /* SJM 09/29/03 - change to handle sign extension and separate types */
   /* AIV 10/13/06 - need to ignore sign here */
   if (xsp1->xslen > ndp->szu.xclen) __narrow_sizchg(xsp1, ndp->szu.xclen);
   else if (xsp1->xslen < ndp->szu.xclen) __sizchg_widen(xsp1, ndp->szu.xclen);

   if (xsp2->xslen > ndp->szu.xclen) __narrow_sizchg(xsp2, ndp->szu.xclen);
   else if (xsp2->xslen < ndp->szu.xclen) __sizchg_widen(xsp2, ndp->szu.xclen);

   __lbitxor(xsp1->ap, xsp1->bp, xsp2->ap, xsp2->bp, xsp1->xslen);
   __pop_xstk();
   break;
  case /* ^~ */ REDXNOR:
   /* SJM 09/29/03 - change to handle sign extension and separate types */
   /* AIV 10/13/06 - need to ignore sign here */
   if (xsp1->xslen > ndp->szu.xclen) __narrow_sizchg(xsp1, ndp->szu.xclen);
   else if (xsp1->xslen < ndp->szu.xclen) __sizchg_widen(xsp1, ndp->szu.xclen);

   if (xsp2->xslen > ndp->szu.xclen) __narrow_sizchg(xsp2, ndp->szu.xclen);
   else if (xsp2->xslen < ndp->szu.xclen) __sizchg_widen(xsp2, ndp->szu.xclen);

   __lbitxnor(xsp1->ap, xsp1->bp, xsp2->ap, xsp2->bp, xsp1->xslen);
   __pop_xstk();
   break;
  case /* != */ RELNEQ:
  case /* == */ RELEQ:
   /* SJM 10/16/00 - for non equal when x/z in either not counted must be */
   /* less pessimistic not equal */
   /* LOOKATME - this is complex - can it be simplified? */

   /* widen narrower to be same as wider - may need sign xtnd */
   /* SJM 05/13/04 - was wrongly using the 1 bit result not other opand */
   if (xsp1->xslen > xsp2->xslen)
    {
     /* SJM 05/13/04 - since result 1 bit word32 but operand cmp signed */
     if (ndp->rel_ndssign) __sgn_xtnd_widen(xsp2, xsp1->xslen);
     else __sizchg_widen(xsp2, xsp1->xslen);
    }
   else if (xsp2->xslen > xsp1->xslen)
    {
     if (ndp->rel_ndssign) __sgn_xtnd_widen(xsp1, xsp2->xslen);
     else __sizchg_widen(xsp1, xsp2->xslen);
    }

   /* result goes into 1 bit tos and know ndp xclen is 1 here */
   /* SJM 05/13/04 - compare can't be signed since eq */
   cmpval = __do_widecmp(&isxz, xsp1->ap, xsp1->bp, xsp2->ap, xsp2->bp,
     xsp1->xslen);

   rtb = 0;
   if (isxz)
    {
     if (!__omitxz_widenoteq(xsp1->ap, xsp1->bp, xsp2->ap, xsp2->bp,
      xsp1->xslen)) rtb = rta = 1L;
     else { if (ndp->optyp == RELEQ) rta = 0; else rta = 1; }
    }
   else
    {
     if (ndp->optyp == RELEQ) rta = (cmpval == 0); else rta = (cmpval != 0);
    }
   goto make_1bit;
  case /* >= */ RELGE:
  case /* > */ RELGT:
  case /* <= */ RELLE:
  case /* < */ RELLT:
   /* widen narrower to be same as wider - may need sign xtnd */
   /* SJM 05/13/04 - was wrongly using the 1 bit result not other opand */
   if (xsp1->xslen > xsp2->xslen)
    {
     /* SJM 05/13/04 - since result 1 bit word32 but operand cmp signed */
     if (ndp->rel_ndssign) __sgn_xtnd_widen(xsp2, xsp1->xslen);
     else __sizchg_widen(xsp2, xsp1->xslen);
    }
   else if (xsp2->xslen > xsp1->xslen)
    {
     if (ndp->rel_ndssign) __sgn_xtnd_widen(xsp1, xsp2->xslen);
     else __sizchg_widen(xsp1, xsp2->xslen);
    }

   /* result goes into 1 bit tos and know ndp xclen is 1 here */
   /* AIV 05/27/04 - must be nd sign not res node has sign since res 1 bit */ 
   if (ndp->rel_ndssign)
    {
     /* SJM 05/10/04 - wide sign compare casts to int32 on not == */
     cmpval = __do_sign_widecmp(&isxz, xsp1->ap, xsp1->bp, xsp2->ap,
      xsp2->bp, xsp1->xslen);
    }
   else
    {
     cmpval = __do_widecmp(&isxz, xsp1->ap, xsp1->bp, xsp2->ap,
      xsp2->bp, xsp1->xslen);
    }
   if (isxz) { rtb = rta = 1L; goto make_1bit; }
   rta = rtb = 0L;

   switch ((byte) ndp->optyp) {
    case RELGE: rta = (cmpval >= 0); break;
    case RELGT: rta = (cmpval > 0); break;
    case RELLE: rta = (cmpval <= 0); break;
    case RELLT: rta = (cmpval < 0); break;
   }
make_1bit:
   /* this is need because a and b parts must be kept contiguous */
   /* SJM 09/30/03 - can use simpler narrow to 1 bit */
   __narrow_to1bit(xsp1);
   xsp1->ap[0] = rta;
   xsp1->bp[0] = rtb;
   xsp1->xslen = 1;
   __pop_xstk();
   break;
  case /* === */ RELCEQ:
  case /* !== */ RELCNEQ:
   /* SJM 09/29/03 - only widen - can be signed */
   if (xsp1->xslen > xsp2->xslen)
    { 
     /* only signed if both signed */
     if (ndp->rel_ndssign) __sgn_xtnd_widen(xsp2, xsp1->xslen);
     else __sizchg_widen(xsp2, xsp1->xslen);
    }
   else if (xsp2->xslen > xsp1->xslen)
    {
     if (ndp->lu.x->has_sign) __sgn_xtnd_widen(xsp1, xsp2->xslen);
     else __sizchg_widen(xsp1, xsp2->xslen);
    }

   /* returns 1 if not equal, 0 if equal */
   cmpval = __do_xzwidecmp(xsp1->ap, xsp1->bp, xsp2->ap, xsp2->bp,
    xsp1->xslen);
   rtb = 0L;
   if (ndp->optyp == RELCEQ) rta = (cmpval == 0); else rta = (cmpval != 0);
   goto make_1bit;
  case /* && */ BOOLAND:
   rtb = 0L;
   tmp1 = __cvt_lngbool(xsp1->ap, xsp1->bp, wlen_(xsp1->xslen));
   if (tmp1 == 0) { rta = 0L; goto make_1bit; }
   tmp2 = __cvt_lngbool(xsp2->ap, xsp2->bp, wlen_(xsp2->xslen));
   if (tmp2 == 0) { rta = 0L; goto make_1bit; }
   if (tmp1 == 1 && tmp2 == 1) rta = 1L; else rta = rtb = 1L;
   goto make_1bit;
  case /* || */ BOOLOR:
   rtb = 0L;
   tmp1 = __cvt_lngbool(xsp1->ap, xsp1->bp, wlen_(xsp1->xslen));
   if (tmp1 == 1) { rta = 1L; goto make_1bit; }
   tmp2 = __cvt_lngbool(xsp2->ap, xsp2->bp, wlen_(xsp2->xslen));
   if (tmp2 == 1) { rta = 1L; goto make_1bit; }
   if (tmp1 == 0 && tmp2 == 0) rta = 0L; else rta = rtb = 1L;
   goto make_1bit;

  /* SJM 03/01/00 - all real binaries can be wide if one wide operand and */
  /* other real (usually 64 bit time) - need to be converted to real and */
  /* evaled as real - same as non wide case be duplicated here so no */
  /* need for extra test in eval inner loop - the non real arg converted */
  /* to real */
  case /* + real */ REALPLUS:
   /* is it portable to pass 1 bit bit field? */
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));

   /* notice never a size change since both must be real */
   d1 += d2;
   /* this works because minimum stack every allocated is 8 bytes */
   /* PORTABILITY - stack must always be at least 8 bytes */
   memcpy(xsp1->ap, &d1, sizeof(double));
   __pop_xstk();
   return;
  case /* - real */ REALMINUS:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   d1 -= d2;
   /* notice never a size change since both must be real */
   memcpy(xsp1->ap, &d1, sizeof(double));
   __pop_xstk();
   return;
  case /* * real */ REALTIMES:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   /* notice never a size change since both must be real */
   d1 *= d2;
   memcpy(xsp1->ap, &d1, sizeof(double));
   __pop_xstk();
   return;
  case /* * real */ REALDIV:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   /* notice never a size change since both must be real */
   d1 /= d2;
   memcpy(xsp1->ap, &d1, sizeof(double));
   __pop_xstk();
   return;
  case /* >= real */ REALRELGE:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));
   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   rta = (d1 >= d2) ? 1L : 0L;
   rtb = 0L; 
   goto make_1bit;
  case /* > real */ REALRELGT:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   rta = (d1 > d2) ? 1L : 0L;
   rtb = 0L; 
   goto make_1bit;
  case /* <= real */ REALRELLE:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   rta = (d1 <= d2) ? 1L : 0L;
   rtb = 0L; 
   goto make_1bit;
  case /* < real */ REALRELLT:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));

   /* notice never a size change since both must be real */
   rta = (d1 < d2) ? 1L : 0L;
   rtb = 0L; 
   goto make_1bit;
  case /* != real */ REALRELNEQ:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));

   /* notice never a size change since both must be real */
   /* this is ieee float point dependent */
   rta = (fabs(d1 - d2) >= EPSILON) ? 1L : 0L;
   rtb = 0L; 
   goto make_1bit;
  case /* == real */ REALRELEQ:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));
   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   /* notice never a size change since both must be real */
   /* this is ieee float point dependent */
   rta = (fabs(d1 - d2) >= EPSILON) ? 0L : 1L;
   rtb = 0L; 
   goto make_1bit;
  case /* && real */ REALBOOLAND:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   /* notice never a size change since both must be real */
   rta = (d1 != 0.0 && d2 != 0.0) ? 1L : 0L;   
   rtb = 0L; 
   goto make_1bit;
  case /* && real */ REALBOOLOR:
   lx = ndp->lu.x;
   rx = ndp->ru.x;
   if (lx->cnvt_to_real) d1 = __cnvt_stk_to_real(xsp1, (lx->has_sign == 1));
   else memcpy(&d1, xsp1->ap, sizeof(double));

   if (rx->cnvt_to_real) d2 = __cnvt_stk_to_real(xsp2, (rx->has_sign == 1));
   else memcpy(&d2, xsp2->ap, sizeof(double));
   /* notice never a size change since both must be real */
   rta = (d1 != 0.0 || d2 != 0.0) ? 1L : 0L;   
   rtb = 0L; 
   goto make_1bit;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * if any x in bin. operand nodes set accumulator to all x's and return T
 */
extern int32 __set_binxresult(word32 *resap, word32 *resbp, word32 *op1bp,
 word32 *op2bp, int32 opbits)
{
 if (!vval_is0_(op1bp, opbits)) goto not_zero;
 if (!vval_is0_(op2bp, opbits)) goto not_zero;
 return(FALSE);

not_zero:
 one_allbits_(resap, opbits);
 one_allbits_(resbp, opbits);
 return(TRUE);
}

/*
 * right shift multiword value into new valune - shift value <1m (1 word32)
 * know shiftval <= lwlen
 *
 * notice no need to mask off high bits here;
 */
extern void __mwrshift(word32 *valwp, word32 shiftval, int32 blen)
{
 int32 shwords, shbits, lwlen;

 lwlen = wlen_(blen);
 shwords = get_wofs_(shiftval);
 /* notice value here is 0-31 */
 shbits = ubits_(shiftval);

 /* DBG remove ---
 if (__debug_flg)
  __dbg_msg("---> mw right shift of %d was %lx", shiftval, valwp[0]);
 --- */
 if (shwords != 0) wrdmwrshift(valwp, shwords, lwlen);
 if (shbits != 0) bitmwrshift(valwp, shbits, lwlen);
 /* DBG remove --
 if (__debug_flg) __dbg_msg("after %lx\n", valwp[0]);
 */
}

/*
 * arithmetic right shift multiword value into new value
 * 
 * arithmetic (signed) version of multi-word right shift - if sign 1,
 * then set area to 1's not 0
 *
 * SJM 10/08/04 - this shifts both a and b part
 */
extern void __arith_mwrshift(word32 *valwp, word32 shiftval, int32 blen)
{
 register int32 wlen, wi;
 int32 sign_bi, bi, shwords, shbits, nblen, is_signed;

 wlen = wlen_(blen);
 sign_bi = get_bofs_(blen - 1);
  
 if ((valwp[wlen - 1] & (1 << sign_bi)) != 0) is_signed = TRUE;
 else is_signed = FALSE;

 shwords = get_wofs_(shiftval);
 /* notice ubits and get_bofs macros are the same */
 shbits = get_bofs_(shiftval);
 /* do normal shift */
 if (shwords != 0) wrdmwrshift(valwp, shwords, wlen);
 if (shbits != 0) bitmwrshift(valwp, shbits, wlen);

 if (!is_signed) return;

 /* tricky part is making sure sign/x/z bit gets shifted in (duplicated) */
 /* new sign bit is one less than new bit len */
 nblen = blen - shiftval;
 /* set 1 bits for wi+1 to end and high bits in wi word32 */
 bi = get_bofs_(nblen);
 wi = get_wofs_(nblen); 
 if (bi != 0)
  {
   valwp[wi] |= (__masktab[WBITS - bi] << bi);
   one_allbits_(&(valwp[wi + 1]), shiftval - (WBITS - bi));
  }
 else one_allbits_(&(valwp[wi]), shiftval);
}

/*
 * wide left shift - know value will not be wider than WBITS
 * for left shift must mask off high bits
 *
 * SJM 10/01/03 - for wide left shift arithmetic is same
 */
extern void __mwlshift(word32 *valwp, word32 shiftval, int32 blen)
{
 int32 shwords, shbits, lwlen, ubits;

 lwlen = wlen_(blen);
 /* this and 1f for 32 mask on number of bits */
 shwords = get_wofs_(shiftval);
 /* notice value here is 0-31 */
 shbits = ubits_(shiftval);

 /* DBG remove --
 if (__debug_flg)
  __dbg_msg("---> mw left shift of %d was %lx", shiftval, valwp[0]);
 -- */
 if (shwords != 0) wrdmwlshift(valwp, shwords, lwlen);
 if (shbits != 0) bitmwlshift(valwp, shbits, lwlen);
 ubits = ubits_(blen);
 valwp[lwlen - 1] &= __masktab[ubits];
 /* DBG remove ---
 if (__debug_flg) __dbg_msg("after %lx\n", valwp[0]);
 --- */
}

/*
 * partial word32 shift within multiword value
 * k < WBITS bits mw right shift (high bits toward low bits - divide)
 * this handles bit shifting - other part does word32 shifting
 */
static void bitmwrshift(register word32 *wp, register int32 k, register int32 lwlen)
{
 register int32 i;
 register word32 cy;

 wp[0] >>= k;
 for (i = 1; i < lwlen; i++)
  {
   cy = (wp[i] & __masktab[k]) << (WBITS - k);
   wp[i - 1] |= cy;
   /* C language right shift of word32 defined to shift in 0's */
   wp[i] >>= k;
  }
 /* since know wp right width with high zero's - know anwswer is right */
}

/*
 * partial word32 shift within multiword value
 * k < WBITS bits mw left shift (low bits toward high - mult.)
 * this handles bit shifting - other part does word32 shifting
 */
static void bitmwlshift(register word32 *wp, register int32 k, register int32 lwlen)
{
 register int32 i;
 register word32 cy;

 wp[lwlen - 1] <<= k;
 for (i = lwlen - 2; i >= 0; i--)
  {
   cy = ((wp[i] >> (WBITS - k)) & __masktab[k]);
   wp[i + 1] |= cy;
   /* C language left logical shift of word32 defined to shift in 0's */
   wp[i] <<= k;
  }
}

/*
 * whole word32 right shift within multiword value
 * kwrds is number of words
 * high words toward low - divide by 2**32 units
 * this handles word32 shifting - other part does bit shifting
 * know kwrds never 0
 */
static void wrdmwrshift(register word32 *wp, register int32 kwrds,
 register int32 lwlen)
{
 register int32 wi;

 for (wi = kwrds; wi < lwlen; wi++)  wp[wi - kwrds] = wp[wi];
 for (wi = lwlen - kwrds; wi < lwlen; wi++) wp[wi] = 0L;
}

/*
 * whole word32 left shift within multiword value
 * kwrds is number of words
 * low words toward high- multiply by 2**32 units
 * this handles word32 shifting - other part does bit shifting
 * know kwrds never 0
 */
static void wrdmwlshift(register word32 *wp, register int32 kwrds,
 register int32 lwlen)
{
 register int32 swi, wi;

 for (swi = lwlen - 1; swi >= kwrds; swi--) wp[swi] = wp[swi - kwrds];
 for (wi = 0; wi < kwrds; wi++) wp[wi] = 0L;
}

/*
 * long binary bit and - in place from top to 1 down
 * know both operands correct final width
 */
extern void __lbitand(word32 *op1ap, word32 *op1bp, word32 *op2ap, word32 *op2bp,
 int32 opbits)
{
 register int32 wi;

 for (wi = 0; wi < wlen_(opbits); wi++)
  {
   if ((op1bp[wi] | op2bp[wi]) == 0L)
    { op1bp[wi] = 0L; op1ap[wi] = op1ap[wi] & op2ap[wi]; }
   else
    {
     op1ap[wi] = (op1ap[wi] | op1bp[wi]) & (op2ap[wi] | op2bp[wi]);
     op1bp[wi] = op1ap[wi] & (op2bp[wi] | op1bp[wi]);
    }
  }
}

/*
 * long binary bit or - both operands on stack already widened to same size
 * no reason to mask off high since both xor and or of 0 and 0 are 0
 */
extern void __lbitor(word32 *op1ap, word32 *op1bp, word32 *op2ap, word32 *op2bp,
 int32 opbits)
{
 register int32 wi;

 for (wi = 0; wi < wlen_(opbits); wi++)
  {
   if ((op1bp[wi] | op2bp[wi]) == 0L)
    { op1bp[wi] = 0L; op1ap[wi] = op1ap[wi] | op2ap[wi]; }
   else
    {
     op1bp[wi] = op2bp[wi] ^ op1bp[wi] ^ ((op1ap[wi] | op1bp[wi])
      & (op2bp[wi] | (op2ap[wi] & op1bp[wi])));
     op1ap[wi] = op1bp[wi] | op2ap[wi] | op1ap[wi];
    }
  }
}

/*
 * long binary bit xor - both operands on stack already widened
 * to exactly same width 
 */
extern void __lbitxor(word32 *op1ap, word32 *op1bp, word32 *op2ap, word32 *op2bp,
 int32 opbits)
{
 register int32 wi;
 int32 wlen;

 wlen = wlen_(opbits);
 for (wi = 0; wi < wlen; wi++)
  {
   if ((op1bp[wi] | op2bp[wi]) == 0L)
    { op1bp[wi] = 0L; op1ap[wi] = op1ap[wi] ^ op2ap[wi]; }
   else
    {
     op1bp[wi] = op1bp[wi] | op2bp[wi];
     op1ap[wi] = op1bp[wi] | (op1ap[wi] ^ op2ap[wi]);
    }
  }
 /* know high bits are 0, since both op's high bits 0 */
}

/*
 * long binary bit xnor - both operands on stack already widened
 */
extern void __lbitxnor(word32 *op1ap, word32 *op1bp, word32 *op2ap, word32 *op2bp,
 int32 opbits)
{
 register int32 wi;
 int32 wlen;

 wlen = wlen_(opbits);
 for (wi = 0; wi < wlen; wi++)
  {
   if ((op1bp[wi] | op2bp[wi]) == 0L)
    { op1bp[wi] = 0L; op1ap[wi] = ~(op1ap[wi] ^ op2ap[wi]); }
   else
    {
     op1bp[wi] = op1bp[wi] | op2bp[wi];
     op1ap[wi] = op1bp[wi] | ~(op1ap[wi] ^ op2ap[wi]);
    }
  }
 op1ap[wlen - 1] &= __masktab[ubits_(opbits)];
 /* know high bits in b part are 0, since both op's high bits 0 */
}

/*
 * convert wide value on top of reg stack to boolean - any 1=1,0,x(3)
 * must be extern since invoked by macro
 */
extern int32 __cvt_lngbool(word32 *ap, word32 *bp, int32 wlen)
{
 register int32 wi;
 int32 hasxs;

 for (hasxs = FALSE, wi = 0; wi < wlen; wi++)
  {
   if ((ap[wi] & ~bp[wi]) != 0L) return(1);
   if (bp[wi] != 0L) hasxs = TRUE;
  }
 if (hasxs) return(3);
 return(0);
}

/*
 * compare word32 first with second - know widths the same
 *
 * set isx if either has x or z, else -1 <, 0 = , 1 >
 * know all wider than WBITS values in Verilog are unsigned
 * not for === or !== sincd non x even if x's or z's
 *
 * know size change made so both same no. words and high bits of narrow now 0 
 */
extern int32 __do_widecmp(int32 *isx, register word32 *op1ap, register word32 *op1bp,
 register word32 *op2ap, register word32 *op2bp, int32 opwid)
{
 register int32 i;

 *isx = TRUE;
 if (!vval_is0_(op1bp, opwid)) return(0);
 if (!vval_is0_(op2bp, opwid)) return(0);

 *isx = FALSE;
 /* know unused parts of high words will both be zero */
 for (i = wlen_(opwid) - 1; i >= 0; i--)
  {
   if (op1ap[i] != op2ap[i])
    { if (op1ap[i] < op2ap[i]) return(-1); else return(1); }
  }
 return(0);
}

/*
 * compare signed wide first with second - know widths the same
 *
 * set isx if either has x or z, else -1 <, 0 = , 1 >
 * not for === or !== sincd non x even if x's or z's
 *
 * know size change made so both same no. words and high bits of narrow now 0 
 */
extern int32 __do_sign_widecmp(int32 *isx, register word32 *op1ap,
 register word32 *op1bp, register word32 *op2ap, register word32 *op2bp, int32 opwid)
{
 register int32 i, i1, i2;
 int32 wlen;

 *isx = TRUE;
 if (!vval_is0_(op1bp, opwid)) return(0);
 if (!vval_is0_(op2bp, opwid)) return(0);

 *isx = FALSE;
 /* wi is index of high word32 */
 wlen = wlen_(opwid);
 
 /* if op1 is negative */
 if ((op1ap[wlen - 1] & (1 << ubits_(opwid - 1))) != 0)
  {
   /* if op1 is negative and op2 is positive */
   if ((op2ap[wlen - 1] & (1 << ubits_(opwid - 1))) == 0) return(-1);
  }
 /* op1 is positive and op2 is negative */
 else if ((op2ap[wlen - 1] & (1 << ubits_(opwid - 1))) != 0) return(1);

 /* here both will have the same sign (especially high word32) */
 /* know unused parts of high words will both be zero */
 for (i = wlen_(opwid) - 1; i >= 0; i--)
  {
   if (op1ap[i] != op2ap[i])
    {
     i1 = (sword32) op1ap[i];    
     i2 = (sword32) op2ap[i];
     if (i1 < i2) return(-1);
     else return(1);
    }
  }
 return(0);
}

/*
 * compare known x/z wide values and return T if not equal when x/z bits
 * ignored
 *
 * SJM 10/16/00 - routine for wide == or != return T if non x/z 
 * comparision is not equal (i.e. if for every word32 any x/z bits in either
 * 1st or 2nd operand are set to same 1 for comparison, then if value not
 * equal x/z bits do not effect outcome so result must be not equal)
 *
 * no need for high bit masking because high unused set to 0 and size
 * change made to make bits same width
 * know size change made before calling this so both same words with narrower's
 * high bits 0 (if one was narrower)
 */
extern int32 __omitxz_widenoteq(register word32 *op1ap, register word32 *op1bp,
 register word32 *op2ap, register word32 *op2bp, int32 opwid)
{
 register word32 xzmask;
 int32 i;
 
 /* know unused parts of high words will both be zero */
 /* when find first bit that makes not equal after masking all x/z in both */
 /* to same, done */
 for (i = wlen_(opwid) - 1; i >= 0; i--)
  {
   xzmask = op1bp[i] | op2bp[i];
   if ((op1ap[i] | xzmask) != (op2ap[i] | xzmask)) return(TRUE);
  }
 return(FALSE);
}

/*
 * compare word32 first with second - know widths the same
 * returns 1 if non equal 0 if equal
 * for === and !== compare cannot be used for greater or less 
 *
 * do not need to worry about high bits since sematics requires 0 extend
 * and 0 and 0 will match as 0 (never effect result)
 */
extern int32 __do_xzwidecmp(register word32 *op1ap, register word32 *op1bp,
 register word32 *op2ap, register word32 *op2bp, int32 opbits)
{
 int32 bytlen;

 bytlen = WRDBYTES*wlen_(opbits);
 if (memcmp(op1ap, op2ap, bytlen) != 0 || memcmp(op1bp, op2bp, bytlen) != 0)
  return(1);
 return(0);
}

/*
 * MULTIWORD ARITHMETIC ROUTINES
 */

/*
 * routines taken from BSD style license mpexpr package and modified
 * to match Verilog internal storage requirements and operation semantics
 * routines mostly stright forward implementations from Knuht Vol. 2
 *  
 * here is copyright notice in mpexpr package zmath.c file: 
 *
 * Copyright (c) 1994 David I. Bell
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * Extended precision integral arithmetic primitives
 *
 * I have re-written these routines to use Cver endian code and to use
 * result wrap around trick for determining add/sub carry from 32 bit
 * words without using 64 bit arithemtic - also not using packages n**1.6
 * multiply routine since more then 300 (or so) bit multiples rare in
 * verilog
 */

/*
 * wide word32 add
 * know u and v same width and resp wide enough and high zeroed
 * >WBITS always unsigned
 *
 * result and operands can't be same
 * LOOKATME - think not worth converting to word32 64 array
 *
 * SJM 09/30/03 - for signed just works because of 2's complement  
 */
extern void __ladd(word32 *res, word32 *u, word32 *v, int32 blen)
{
 register word32 a2;
 register word32 *u_end, cy;
 int32 ublen, vblen, trimblen, wlen, hzwlen, verwlen;
 extern void __my_fprintf(FILE *, char *, ...);
 
 ublen = __trim1_0val(u, blen);
 vblen = __trim1_0val(v, blen);
 trimblen = (ublen >= vblen) ? ublen : vblen;
 /* if trimmed max fits, need 1 more word32 for carry that is needed */ 
 if ((wlen = wlen_(trimblen)) < (verwlen = wlen_(blen))) wlen++;
 if ((hzwlen = verwlen - wlen) > 0)
  memset(&(res[wlen]), 0, WRDBYTES*hzwlen);

 u_end = &(u[wlen]);
 cy = 0;
 do {
   /* DBG remove --
   __dbg_msg("at top of loop: cy=%0x\n", cy); 
   -- */ 
   a2 = *v++;
   *res = *u++ + a2 + cy;
   /* use wrap around 32 bit test and auto incr instead of mpexpr cast */
   /* to word32 64 although current gcc does not handle auto inc/dec well */
 
   /* notice if cy on if res and v equal, must not turn off */
   /* also if cy off if res and v equal, do not turn on */
   if (cy == 0) { if (*res < a2) cy = 1; }
   else { if (*res > a2) cy = 0; }
   res++;

   /* DBG remove ---
   __dbg_msg("*u=%0x, *v=%0x, a2=%0x, *res=%0x, cy=%0x\n", u[-1], v[-1],
     a2, res[-1], cy);
   --- */
  } while (u < u_end);

 /* usually do not need this but faster to mask than test and then mask */
 res--;
 *res &= __masktab[ubits_(blen)];
}

/*
 * wide subtract
 * know u and v same width and resp wide enough and zeroed
 * also res can be same as u or v (needed for ldiv2)
 * can get by with 32 bit arithmetic here
 * since mask any unused high bits - can borrow from unused
 *
 * LOOKATME - think not worth converting to word32 64 array
 * SJM 09/28/03 - 2's complement means signed just interpretation
 * i.e. if sign bit on then negative
 */
extern word32 __lsub(word32 *res, word32 *u, word32 *v, int32 blen)
{
 register word32 *u_end, borrow, tmpres;
 int32 wlen;

 wlen = wlen_(blen);
 borrow = 0; 
 u_end = &(u[wlen]);
 do {
   /* modified to use only 32 bit arithmetic and ptr inc */
   if ((tmpres = *u++ - borrow) > (ALL1W - borrow)) tmpres = ALL1W - *v; 
   else if ((tmpres -= *v) > (ALL1W - *v)) borrow = 1;
   else borrow = 0;
   *res++ = tmpres; 
   v++;
 } while (u < u_end);
 /* notice in Verilog borrow always taken - even though nothing higher */
 res--; 
 *res &= __masktab[ubits_(blen)];
 return(borrow);
} 

/*
 * multiple 2 multi-word32 signed numbers
 *
 * wrapper that use normal word32 lmult on absolute values
 * since no x/z part (already handled) no x/z extension
 * BEWARE - this depends on fact that xstk ap/bp parts contiguous
 */
extern void __sgn_lmult(register word32 *res, register word32 *u,
 register word32 *v, int32 blen)
{
 int32 wlen, usign, vsign;
 word32 *wrku, *wrkv;
 struct xstk_t *uxsp, *vxsp;

 wlen = wlen_(blen);
 usign = vsign = 1;
 uxsp = vxsp = NULL;
 if (__is_lnegative(u, blen))
  {
   /* SJM 09/15/04 - lnegate need both a and b parts */
   push_xstk_(uxsp, blen);
   usign = -1;
   /* ignoring carry */
   __cp_lnegate(uxsp->ap, u, blen);
   wrku = uxsp->ap;
  }
 else wrku = u;
 if (__is_lnegative(v, blen))
  {
   /* SJM 09/15/04 - lnegate need both a and b parts */
   push_xstk_(vxsp, blen);
   vsign = -1;
   /* ignoring carry */
   __cp_lnegate(vxsp->ap, v, blen);
   wrkv = vxsp->ap;
  }
 else wrkv = v;

 __lmult(res, wrku, wrkv, blen);
 if ((usign*vsign) == -1)
  {
   __inplace_lnegate(res, blen);
  }
 if (uxsp != NULL) __pop_xstk();
 if (vxsp != NULL) __pop_xstk();
}

/*
 * routine to determine if signed val negative by checking sign bit
 * 
 * FIXME - this should be macro
 */
extern int32 __is_lnegative(word32 *u, int32 blen) 
{
 register int32 wi, bi;

 blen--;
 wi = get_wofs_(blen);
 bi = get_bofs_(blen);
 if ((u[wi] & (1 << bi)) != 0) return(TRUE);
 return(FALSE);
}

/*
 * in place routine to compute 2's complement negation of signed wide number
 * formula is ~(value) + 1
 * return carry if any but not used for now
 * in place
 *
 * LOOKATME - copy version - maybe in place better
 */
extern word32 __inplace_lnegate(register word32 *u, int32 blen)
{
 register int32 wi, ubits;
 int32 wlen;
 word32 cy;
 
 wlen = wlen_(blen);
 for (wi = 0; wi < wlen; wi++) u[wi] = ~(u[wi]);
 ubits = ubits_(blen); 
 u[wlen - 1] &= __masktab[ubits]; 
 /* SJM 09/15/04 - was wrongly passes ubits so was not incing high words */ 
 cy = sgn_linc(u, blen);
 return(cy);
}

/*
 * copy routine to compute 2's complement negation of signed wide number
 * formula is ~(value) + 1
 * return carry if any but not used for now
 * in place
 *
 * LOOKATME - copy version - maybe in place better
 */
extern word32 __cp_lnegate(word32 *u, register word32 *v, int32 blen)
{
 register int32 wi, ubits;
 word32 cy;
 int32 wlen;
 
 wlen = wlen_(blen);
 for (wi = 0; wi < wlen; wi++, v++) u[wi] = ~(*v);
 ubits = ubits_(blen); 
 u[wlen - 1] &= __masktab[ubits]; 

 cy = sgn_linc(u, blen);
 return(cy);
}

/*
 * inc (add 1) in place to wide signed value
 */
static int32 sgn_linc(register word32 *u, int32 blen)
{
 register int32 wi, ubits;
 register int32 wlen;

 wlen = wlen_(blen);
 /* done when no carry - special case speed up attmpt */ 
 if (++(u[0]) != 0) return(0); 

 /* enter loop with cy */
 for (wi = 1; wi < wlen; wi++)
  {
   /* add the carry from last one */
   if (++(u[wi]) != 0)
    {
     if (wi != wlen - 1) return(0);
     ubits = ubits_(blen);
     u[wi] &= __masktab[ubits];
     return(1);
    }
  }
 /* value was all 1's and fills high word32, no mask but return cy */
 /* 2's complement of 0 is 0 plus carry */
 return(1);
}

/*
 * multiply two multi-word32 numbers to obtain the double len product
 *
 * notice res must not be same addr as u or v
 * original idea for this routine came from Dr. Dobbs article
 *
 * this does not use mpexpr recursive 1.6 power multiply since Verilog
 * numbers rarely wider than 300 bits - algorithm is simple distributed
 * accumulate
 *
 * SJM 09/28/03 - must multply with absolute values so there is sign  
 * handling wrapper for signed wide multiply
 */
extern void __lmult(register word32 *res, register word32 *u, register word32 *v,
 int32 blen)
{
 register int32 i;
 int32 wlen, ublen, vblen, uwlen, vwlen, prodwlen;
 word32 *wp;
 w64_u w64res;
 struct xstk_t *xsp;
 
 /* set result to zero for special case - not using b part - left as 0 */
 wlen = wlen_(blen);
 memset(res, 0, wlen*WRDBYTES);

 /* normalize - by finding bit widths for u and v */
 ublen = __trim1_0val(u, blen);
 vblen = __trim1_0val(v, blen);
 if (ublen == 0 || vblen == 0) return;
 /* if trim so that both multipliers fit in 32 bits use 64 prod routine */
 /* know blen > WBITS or will not be called */
 if (ublen <= WBITS && vblen <= WBITS) 
  {
   /* notice if blen wider then 64 - values already 0ed and left 0ed */
   w64res.w64v = ((word64) *u)*((word64) *v);
   res[0] = w64res.w_u.low;
   /* SJM 12/07/01 - and out any bits wider than blen if blen < 64 */
   /* very wide blen can trim to here, if so no mask needed */
   res[1] = w64res.w_u.high;
   if (blen < 64) res[1] &= __masktab[ubits_(blen)];
   return;
  }
 /* at least one trim wider than 32 but no carry to high since blen <= 64 */
 if (blen <= 64) 
  {
   w64_u w64op1, w64op2;
   
   /* LOOKATME - could just use pointer for little endia X86 */
   w64op1.w_u.low = u[0];
   w64op1.w_u.high = u[1]; 
   w64op2.w_u.low = v[0];
   w64op2.w_u.high = v[1]; 
   w64res.w64v = w64op1.w64v * w64op2.w64v;
   res[0] = w64res.w_u.low;
   /* SJM 12/07/01 - and out any bits wider than blen - fastest always mask */
   res[1] = w64res.w_u.high & __masktab[ubits_(blen)];
   return;
  }
 uwlen = wlen_(ublen); 
 vwlen = wlen_(vblen);
 prodwlen = uwlen + vwlen;
 /* multiply into double trimmed width product - but use all no b part */
 push_xstk_(xsp, prodwlen*WBITS/2);
 wp = xsp->ap;
 memset(wp, 0, WRDBYTES*prodwlen);
 for (i = 0; i < uwlen; i++) 
  {
   wp[i + vwlen] += accmuladd32(&(wp[i]), &(wp[i]), u[i], v, vwlen);
  }
 
 memcpy(res, wp, ((wlen < prodwlen) ? wlen : prodwlen)*WRDBYTES); 
 /* SJM 04/07/03 - need to mask high bits in high word32 here */
 /* AIV 12/20/06 - was masking wrong pointer should be res not wp */
 res[wlen - 1] &= __masktab[ubits_(blen)];
 __pop_xstk();
}

/*
 * a[] = b[] + c*d[] - compute array per word32, word32 product sum
 * returns carry
 */
static int32 accmuladd32(word32 *a, word32 *b, word32 c, word32 *d, int32 wlen)
{
 register int32 i;
 register word32 t0, t1, cy;
 w64_u res;
 word64 c64;

 c64 = (word64) c;
 for (cy = 0, i = 0; i < wlen; i++)
  {
   /* know product of 2 32 bit values fits in 64 bits */
   res.w64v = c64*((word64) d[i]);
   t0 = res.w_u.low;
   t1 = res.w_u.high;

   if ((a[i] = b[i] + cy) < cy) cy = 1; else cy = 0;
   if ((a[i] += t0) < t0) cy++;
   cy += t1;
  }
 return(cy);
}

/*
 * interfact to signed long div and mod (keep rem) that select needed result
 *
 * wrapper that use normal word32 on absolute values then adjusts signs
 * since no x/z part (already handled) no x/z extension
 * BEWARE - this depends on fact that xstk ap/bp parts contiguous
 */
extern void __sgn_ldivmod(register word32 *res, register word32 *u,
 register word32 *v, int32 blen, int32 nd_quot)
{
 int32 wlen, usign, vsign;
 word32 *wrku, *wrkv;
 struct xstk_t *uxsp, *vxsp, *tmpxsp; 

 /* always need unused tmp area for unused of mod/div results */
 wlen = wlen_(blen);
 push_xstk_(tmpxsp, wlen*WBITS/2);

 wlen = wlen_(blen);
 usign = vsign = 1;
 uxsp = vxsp = NULL;
 /* div/mod routine assumes both operands positive */
 if (__is_lnegative(u, blen))
  {
   /* SJM 09/15/04 - lnegate need both a and b parts */
   push_xstk_(uxsp, blen);
   usign = -1;
   /* ignoring carry */
   __cp_lnegate(uxsp->ap, u, blen);
   wrku = uxsp->ap;
  }
 else wrku = u;
 if (__is_lnegative(v, blen))
  {
   /* SJM 09/15/04 - lnegate need both a and b parts */
   push_xstk_(vxsp, blen);
   vsign = -1;
   /* ignoring carry */
   __cp_lnegate(vxsp->ap, v, blen);
   wrkv = vxsp->ap;
  }
 else wrkv = v;

 /* separate into div/mod and adjust sign according to different rules */
 if (nd_quot)
  {
   __ldivmod2(res, tmpxsp->ap, wrku, wrkv, blen);
   /* for div sign negative if one but not both negative */
   if ((usign*vsign) == -1) __inplace_lnegate(res, blen);
  }
 else
  {
   __ldivmod2(tmpxsp->ap, res, wrku, wrkv, blen);
   /* for mod sign same as sign of first but must do word32 wide div/mod */
   if (usign == -1) __inplace_lnegate(res, blen);
  }
 if (uxsp != NULL) __pop_xstk();
 if (vxsp != NULL) __pop_xstk();
 __pop_xstk();
}

/*
 * interfact to long div and mod (keep rem) that select needed result
 */
extern void __ldivmod(word32 *res, word32 *u, word32 *v, int32 blen, int32 nd_quot)
{
 int32 wlen; 
 struct xstk_t *tmpxsp; 

 wlen = wlen_(blen);
 push_xstk_(tmpxsp, wlen*WBITS/2);

 if (nd_quot) __ldivmod2(res, tmpxsp->ap, u, v, blen);
 else __ldivmod2(tmpxsp->ap, res, u, v, blen);
 __pop_xstk();
}

/*
 * Divide two numbers to obtain a quotient and remainder.
 *
 * dividing u by v - know if v 0 already returned x value
 * res, quot, u and v must all be different addresses
 * u and v are preserved and no b parts are assumed to exist 
 *
 * blen is width of both u and z (one widened if needed from Ver semantics)
 * fills blen wide result
 *
 * SJM 09/30/03 - wrapper insures operands here are positive  
 */
extern void __ldivmod2(word32 *quot, word32 *rem, word32 *u, word32 *v, int32 blen)
{
 register word32 *uwp, *vwp;
 word32 r0;
 int32 ublen, vblen, uwlen, vwlen, wlen, normdist, ubits;
 struct xstk_t *xsp;

 /* set rem and quotient to zero */
 wlen = wlen_(blen);

 memset(quot, 0, wlen*WRDBYTES);
 memset(rem, 0, wlen*WRDBYTES);

 /* normalize - by finding bit widths for u and v */
 ublen = __trim1_0val(u, blen);
 vblen = __trim1_0val(v, blen);
 /* 0 over anything is quotient and remainder of 0 */
 if (ublen == 0) return;

 /* can use c division - know not signed */
 if (ublen <= WBITS && vblen <= WBITS) 
  { quot[0] = u[0] / v[0]; rem[0] = u[0] % v[0]; return; }

 /* if divisor fits in half word32, use fast linear algorithm */
 if (vblen <= WBITS/2)
  {
   /* special divide by 1 - rem is 0 (already initialized and quot is u) */
   if (v[0] == 1)
    {
     cp_walign_(quot, u, ublen);
     return;
    }
   __by16_ldivmod(quot, &r0, u, v[0], ublen);
   rem[0] = r0;
   return;
  }
 /* if u smaller in abs. value then v, answer immediate */
 uwlen = wlen_(ublen);
 vwlen = wlen_(vblen);
 /* if u smaller than v, then rem is u and quotient is 0 */
 /* think this is wrong - what about extra high part of 1 ? */
 if (ldiv_cmp(u, v, ((uwlen < vwlen) ? vwlen : uwlen)) < 0)
  { cp_walign_(rem, u, blen); return; }

 /* need long division */
 /* normalizing divisor (bottom)(v) first */ 
 /* high bit of divisor (v) must be 1 - compute number of leading 0s */
 /* AIV 06/25/04 - only nomalize if not multiple of WBITS */
 ubits = ubits_(vblen);
 normdist = (ubits == 0) ? 0 : WBITS - ubits;

 /* since must shift, need copy of if stacked v (divisor) can be changed */
 /* could just shift */
 push_xstk_(xsp, vwlen*WBITS/2);
 vwp = xsp->ap;
 memcpy(vwp, v, vwlen*WRDBYTES);

 if (normdist != 0) bitmwlshift(vwp, normdist, vwlen);

 /* normalize dividend next */
 /* need 1 extra 0 digit in work numerator (u) so shift fits */
 push_xstk_(xsp, (uwlen + 1)*WBITS/2);
 uwp = xsp->ap;
 memcpy(uwp, u, uwlen*WRDBYTES); 

 uwp[uwlen] = 0;
 if (normdist != 0) bitmwlshift(uwp, normdist, uwlen + 1);

 /* use basic algorithm of mpexpr long div routine */
 /* notice length of dividend (u) is plus 1 because divisor (v) gets */
 /* normalized probably causing shift of part of u into 1 higher word32 */
 mpexpr_zdiv(quot, rem, uwp, uwlen + 1, vwp, vwlen);
 if (normdist != 0) bitmwrshift(rem, normdist, vwlen);
 __pop_xstk();
 __pop_xstk();
}


/*
 * EXTENDED PRECISION DIVISION ROUTINE 
 */

#define BASE 0x100000000ULL
#define BASE1 BASE - 1ULL

/*
 * Divide ztmp1/ztmp2 and set quotient in quot and remainder in rem
 *
 * user must allocate large enough area for quot and rem and they must
 * be zeroed before call 
 * 
 * know ztmp1 and zmp2 normalized copies of u and v (u/v) so this can
 * overright values
 *
 * routine is taken exactly from mpexpr except changed to verilog number
 * representation and special cases handled by ldiv/lmod removed
 *
 * "digit" here is word32 32 bit word32, lengths are no. of words 
 * uwp is top and vwp is bottom
 *
 * changed endian word64/word32 access to follow Cver conventions and set
 * 64 long long 32 long that is only supported by modern c compilers
 * know both zu and v normalized and dividend > 0 when called
 * 
 * uwp and vwp are copies of u and v and both normalized before calling zdiv
 * 
 * Comments in mpexpr:
 *   This algorithm is taken from
 *   Knuth, The Art of Computer Programming, vol 2: Seminumerical Algorithms.
 *   Slight modifications were made to speed this mess up.
 *
 */
static void mpexpr_zdiv(word32 *quot, word32 *rem, word32 *ztmp1, int32 ztmp1_len,
 word32 *ztmp2, int32 ztmp2_len)  
{
 register word32 *q, *pp;
 int32 quot_len, y, ztmp3_len, k;
 word64 x;
 word32 *ztmp3, h2, v2;
 /* pair of word32 values to make word64 value - uses endianess */
 w64_u pair;
 struct xstk_t *ztmp3_xsp;

 /* know u/v both normalized and remainder only case remove before here */
 quot_len = ztmp1_len - ztmp2_len;
 q = &(quot[quot_len]);
 y = ztmp1_len - 1;
 h2 = ztmp2[ztmp2_len - 1];
 v2 = 0;
 k = ztmp1_len - ztmp2_len;

 /* need ztmp3 of width v_len (denominator size) + 1 & b part for dbg print */
 /* LOOKATME - except for debugging this does not need b part */
 push_xstk_(ztmp3_xsp, (ztmp2_len + 1)*WBITS);
 ztmp3 = ztmp3_xsp->ap;
 ztmp3_len = ztmp2_len + 1;
 /* zero b part for debugging */
 memset(&(ztmp3[ztmp3_len]), 0, ztmp3_len*WRDBYTES);
 if (ztmp2_len >= 2) v2 = ztmp2[ztmp2_len - 2];

 /* y starts at trimmed len of u - 1 in words and is deced each time thru */
 /* k is how many "digits" more in numerator than denominator */
 for (; k--; --y)
  {
   pp = &(ztmp1[y - 1]);
   pair.w_u.low = pp[0];
   pair.w_u.high = pp[1];

   if (ztmp1[y] == h2) x = BASE1; else x = pair.w64v/(word64) h2;
   if (x != 0ULL)
    {
     /* this computes one word32 ("digit") x */
     while ((pair.w64v - x*h2 < BASE) && (y > 1)
      && (v2*x > (pair.w64v - x*h2) * BASE + ztmp1[y-2]))
      {
       x--;
      }
     /* multiply high digit by numerator in preparation for subtract */
     /* carry may go into one digit wider than denominator w2tmp size */
     /* notice reversed order from mpexpr zdiv so result in 1st arg */
     dmul(ztmp3, ztmp3_len, ztmp2, ztmp2_len, x);

     /* DBG remove ---
     if (__debug_flg)
      {
       __dbg_msg("** zdiv: x = %ld\n", x);
       __dbg_msg("         ztmp1 = %s\n", __regab_tostr(__xs, ztmp1,
        &(ztmp1[ztmp1_len]), ztmp1_len*WBITS, BHEX, FALSE));
       __dbg_msg("         ztmp2 = %s\n", __regab_tostr(__xs, ztmp2,
        &(ztmp2[ztmp2_len]), ztmp2_len*WBITS, BHEX, FALSE));
       memset(&ztmp3[ztmp3_len], 0, ztmp3_len*WRDBYTES);
       __dbg_msg("         ztmp3 = %s\n", __regab_tostr(__xs, ztmp3,
        &(ztmp3[ztmp3_len]), ztmp3_len*WBITS, BHEX, FALSE));
      }
     --- */

     /* subtract new "high" digit ("word") from numerator */
     if (dsub(ztmp1, ztmp1_len, ztmp3, ztmp3_len, y, ztmp2_len))
      {
       --x;
       /* DBG remove --
       if (__debug_flg) __dbg_msg("** zdiv: adding back\n");
       -- */  
       dadd(ztmp1, ztmp2, y, ztmp2_len);
      }
    }
   ztmp1_len = ztrim(ztmp1, ztmp1_len);
   /* each time thru set one current high digit of quotient to value */
   *--q = (word32) x;
  }

 /* remainder is value now in ztmp1, caller unnormalizes rem */
 memcpy(rem, ztmp1, ztmp1_len*WRDBYTES);

 /* quot has right value - no need to unnormalize */
 __pop_xstk();
}

/*
 * 32 bit "digit" trim - unlike mpexpr ztrim returns trimmed width 
 */
static int32 ztrim(word32 *zp, int32 z_len)
{
 register word32 *h;
 register int32 len;
 
 h = &(zp[z_len - 1]);
 len = z_len;
 while (*h == 0 && len > 1) { --h; --len; }
 return (len);
}

/*
 * internal add in place z1 += z2
 *
 * what are y and n
 * need len to save extra work for high 0s
 */
static void dadd(word32 *z1p, word32 *z2p, int32 y, int32 n)
{
 word32 *s1p, *s2p;
 word32 carry;
 word64 sum;

 s1p = &(z1p[y - n]);
 s2p = z2p;
 carry = 0;
 while (n--)
  {
   sum = ((word64) *s1p) + ((word64) *s2p) + ((word64) carry);
   carry = 0;
   if (sum >= BASE) { sum -= BASE; carry = 1; }
   *s1p = (word32) sum;
   ++s1p;
   ++s2p;
  }
 sum = ((word64) *s1p) + ((word64) carry);
 *s1p = (word32) sum;
}

/*
 * subtract z2p from z1p with result in place into z1p for divide
 * returns T result goes negative.
 *
 * LOOKATME - what are y and n?
 * "digits" unsigned
 */
static int32 dsub(word32 *z1p, int32 z1_len, word32 *z2p, int32 z2_len, int32 y, int32 n)
{
 word32 *s1p, *s2p, *s3p;
 word64 i1;
 int32 neg;

 neg = FALSE;
 s1p = &(z1p[y - n]);
 s2p = z2p;
 if (++n > z2_len) n = z2_len;

 while (n--)
  {
   i1 = (word64) *s1p;
   if (i1 < (word64) *s2p)
    {
     s3p = &(s1p[1]);
     while (s3p < &(z1p[z1_len]) && !(*s3p))
      { *s3p = (word32) BASE1; ++s3p; }

     if (s3p >= &(z1p[z1_len])) neg = TRUE; else --(s3p[0]);
     i1 += BASE;
    }
  *s1p = (word32) (i1 - (word64) *s2p);
  ++s1p;
  ++s2p;
 }
 return neg;
}

/*
 * multiply into tmp dest zp times one word32 ("digit") in mul  
 *
 * mpexpr comments:
 * Multiply a number by a single 'digit'.
 * This is meant to be used only by the divide routine, and so the
 * destination area must already be allocated and be large enough.
 */
static void dmul(word32 *destp, int32 dest_len, word32 *z1p, int32 z1_len, word64 mul)
{
 register word32 *zp, *dp;
 w64_u pair;
 word64 carry;
 long len;

 memset(destp, 0, dest_len*WRDBYTES);
 /* multiple by 0 result is 0 */
 if (mul == 0ULL) return;

 len = z1_len;
 zp = &(z1p[len - 1]);
 dp = destp;
 /* trim each time to save work */
 while ((*zp == 0) && (len > 1)) { len--; zp--; }
 zp = z1p;

 carry = 0;
 /* compute 4 word("digits") sections with unrolled loop */ 
 while (len >= 4) {
  len -= 4;
  pair.w64v = (mul * ((word64) *zp++)) + carry;
  *dp++ = pair.w_u.low;
  pair.w64v = (mul * ((word64) *zp++)) + ((word64) pair.w_u.high);
  *dp++ = pair.w_u.low;
  pair.w64v = (mul * ((word64) *zp++)) + ((word64) pair.w_u.high);
  *dp++ = pair.w_u.low;
  pair.w64v = (mul * ((word64) *zp++)) + ((word64) pair.w_u.high);
  *dp++ = pair.w_u.low;
  carry = pair.w_u.high;
 }
 /* copmute final ending left over digits ("words") */ 
 while (--len >= 0) {
  pair.w64v = (mul * ((word64) *zp++)) + carry;
  *dp++ = pair.w_u.low;
  carry = pair.w_u.high;
 }
 /* LOOKATME - how make sure enough room in dest if there is a carry? */
 if (carry != 0) *dp = (word32) carry;
}

/*
 * compare only a parts - know wider than WBITS bits
 */
static int32 ldiv_cmp(register word32 *u, register word32 *v, int32 wlen) 
{
 register int32 i;

 /* know unused parts of high words will both be zero */
 for (i = wlen - 1; i >= 0; i--)
  {
   if (u[i] < v[i]) return(-1);
   else if (u[i] > v[i]) return(1);
  }
 return(0);
}

/*
 * divide a number by 1 half word32 (digit based 16 bits)
 * notice all values must be separate addresses
 *
 * also quot and rem assumed to be initialized to 0 before passing to here
 * also this works on words not half words
 */
extern void __by16_ldivmod(word32 *quot, word32 *r0, word32 *u, word32 v0, int32 ublen)
{
 register int32 i;
 int32 uwlen;
 word32 r, newn;

 uwlen = wlen_(ublen); 
 r = 0L;
 for (i = uwlen - 1; i >= 0; i--)
  {
   newn = r*SHORTBASE + (u[i] >> (WBITS/2));
   quot[i] = (newn / v0) << (WBITS/2);
   r = newn % v0;
   /* notice since r < d0, newn / v0 will aways fit in half word32 */
   newn = r*SHORTBASE + (u[i] & ALL1HW); 
   quot[i] |= (newn / v0);
   r = newn % v0;
  }
 *r0 = r;
}
