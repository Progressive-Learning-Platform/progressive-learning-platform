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
 * Verilog delay preparation and changing routines
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __DBMALLOC__
#include "../malloc.h"
#endif

#include "v.h"
#include "cvmacros.h"

/* local prototypes */
static void delx_to_deltim(word64 *, struct expr_t *, struct xstk_t *);
static void emit_reprepdev_verbmsg(struct gate_t *, struct gate_t *,
 struct mod_t *, struct itree_t *, int32);
static void emit_reprepconta_verbmsg(struct conta_t *, struct gate_t *,
 struct mod_t *, struct itree_t *, int32);
static void emit_reprepnet_verbmsg(struct net_t *, struct gate_t *,
 struct mod_t *, struct itree_t *, int32);
static void emit_reprepstmt_verbmsg(struct st_t *, struct gate_t *,
 struct mod_t *, struct itree_t *, int32);
static void emit_reprepiopath_verbmsg(struct spcpth_t *, struct gate_t *,
 struct mod_t *, struct itree_t *, int32);
static void emit_repreptclim_verbmsg(struct tchk_t *, struct gate_t *,
 struct mod_t *, struct itree_t *, word32, int32);
static void prep2_delay(struct gate_t *, struct paramlst_t *, int32, int32);
static void nonis_to_delval(word64 *, struct expr_t *);
static void prep_1delform(struct gate_t *, struct expr_t *);
static void convert_trireg_1vto4v(struct gate_t *);
static void prep_prim_is_dels(struct gate_t *, struct expr_t **, int32, int32);
static void prep_path_is_dels(struct gate_t *, struct expr_t **, int32);
static void fill_1col_isdel(word64 *, int32, struct expr_t *, int32);
static void chg1vform_1instdel(struct gate_t *, struct itree_t *,
 struct gate_t *);
static void set_1is1val(struct gate_t *, int32, int32, word64 *, int32);
static void chg4vform_1instdel(struct gate_t *, struct itree_t *,
 struct gate_t *);
static void set_1is416val(struct gate_t *, int32, int32, word64 *, int32);
static void chg16vform_1instdel(struct gate_t *, struct itree_t *,
 struct gate_t *);
static void cnv_1is_to_4is(struct gate_t *, int32, int32, int32);
static void cnv_1is_to_16is(struct gate_t *, int32, int32, int32);
static void create_disv(struct gate_t *, int32, int32, word64 *, int32);
static int32 get_ispack(word64);
static void unpack_isv1_to_isv2(struct gate_t *, int32, int32);
static void unpack_isv1_to_isv(struct gate_t *, int32, int32);
static void unpack_isv2_to_isv(struct gate_t *, int32, int32);
static void zero_unused_16v(word64 *);
static void real_to_ticks(word64 *, double, struct mod_t *);

/* extern prototypes (maybe defined in this module) */
extern void __hizstrengate_getdel(word64 *, register struct gate_t *);
extern void __get_del(register word64 *, register union del_u, word32);
extern int32 __real_to_v64tim(word64 *, double);
extern void __cnv_ticks_tonum64(word64 *, word64, struct mod_t *);
extern void __re_prep_dels(struct net_t *, struct itree_t *, struct mod_t *,
 int32);
extern void __prep_delay(struct gate_t *, struct paramlst_t *, int32, int32, char *, int32, struct sy_t *, int32);
extern void __free_del(union del_u, word32, int32);
extern void __free_dellst(struct paramlst_t *);
extern void __fill_4vconst(word64 *, word64 *, word64 *, word64 *, int32, int32);
extern void __fill_16vconst(word64 *, word64 *, int32);
extern void __chg_1inst_del(struct gate_t *, struct itree_t *, struct gate_t *);
extern char *__bld_delay_str(char *, union del_u, word32);
extern void __extract_delval(word64 *, int32 *, union del_u, word32);
extern void __map_16v_to_12vform(word64 *, word64 *);
extern void __try_reduce_16vtab(word64 *, int32 *);
extern void __do_showdel_stask(struct expr_t *, char *);
extern struct xstk_t *__eval2_xpr(register struct expr_t *);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern char *__to_timstr(char *, word64 *);
extern int32 __v64_to_real(double *, word64 *);
extern char *__msg2_blditree(char *, struct itree_t *);
extern char *__bld_lineloc(char *, word32, int32);
extern char *__to_tcnam(char *, word32);
extern struct paramlst_t *__copy_dellst(struct paramlst_t *);
extern int32 __chk_delparams(struct paramlst_t *, char *, int32);
extern void __chk_spec_delay(struct expr_t *, char *);
extern char *__my_malloc(int32);
extern void __my_free(char *, int32);
extern struct expr_t *__copy_expr(struct expr_t *);
extern void __free_xtree(struct expr_t *);
extern void __push_wrkitstk(struct mod_t *, int32);
extern void __pop_wrkitstk(void);
extern int32 __is_lnegative(word32 *, int32); 

extern void __cv_msg(char *, ...);
extern void __gfwarn(int32, word32, int32, char *, ...);
extern void __sgfwarn(int32, char *, ...);
extern void __gferr(int32, word32, int32, char *, ...);
extern void __sgferr(int32, char *, ...);
extern void __dbg_msg(char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);

extern int32 errno;
extern word32 __masktab[];

#if defined(__SVR4) || defined(__hpux)
#define drem fmod
#endif

double __dbl_toticks_tab[] = { 1.0, 1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5,
 1.0e6, 1.0e7, 1.0e8, 1.0e9, 1.0e10, 1.0e11, 1.0e12, 1.0e13, 1.0e14, 1.0e15 };

/*
 * DELAY ACCESS ROUTINES
 */

/*
 * get a primitive delay for a gate that drives highz[01]
 * this is only called if g_hasst T
 *
 * primitive gate delays always determined solely by new value
 * and since wire (not gate) driven to z because of strength use wire for
 * delay
 */
extern void __hizstrengate_getdel(word64 *gdel, register struct gate_t *gp)
{ 
 register word32 save_val, stren0;
   
 save_val = __new_gateval;
 /* know one or other must be 0 strength */
 stren0 = (gp->g_stval >> 3) & 7;
 if (stren0 == 0) { if (save_val == 0) __new_gateval = 2; }
 else { if (save_val == 1) __new_gateval = 2; } 
 __get_del(gdel, gp->g_du, gp->g_delrep);
 __new_gateval = save_val;
}

/*
 * get the delay value needed for transition to global __new_gateval
 * know no delay case already handled
 *
 * know 4 array already set with right values if gate or conta
 * global new gateval must be set to new value (0-3) (0,1,z,x) and
 * for continuous assign (know > 1) must be corrected (0 if all 0's,
 * (2 if all z's, else 1 (rise)
 *
 * notice this can return a 0 delay that needs to be on pound 0 queue
 * also these have all been scaled
 *
 * need to remove all but low 3 bits because can be used for strenght vals
 */
extern void __get_del(register word64 *gdel, register union del_u du,
 word32 drep)
{
 register int32 ind;
 word64 dv1, dv0;
 struct xstk_t *xsp;
 struct expr_t *dxp;
 struct thread_t *sav_curthp;

 /* -- DBG remove
  extern void __dbg_dump_del(union del_u, word32);

 if (__debug_flg) __dbg_dump_del(du, drep);
 -- */
 
 switch ((byte) drep) {
  case DT_1V: *gdel = du.d1v[0]; break;
  case DT_IS1V: *gdel = du.dis1v[__inum]; break;
  case DT_IS1V1:
   *gdel = (word64) du.dis1v1[__inum];
   break;
  case DT_IS1V2:
   *gdel = (word64) du.dis1v2[__inum];
   break;
  case DT_4V:
   /* notice here the array has precomputed the delay for the new value */
   /* including min comp. for x/z */
   *gdel = du.d4v[__new_gateval & 3];
   break;
  case DT_IS4V:
   /* this is time width 32 bit dependent */
   *gdel = du.d4v[4*__inum + (__new_gateval & 3)];
   break;
  case DT_IS4V1:
   *gdel = (word64) du.dis4v1[4*__inum + (__new_gateval & 3)];
   break;
  case DT_IS4V2:
   *gdel = (word64) du.dis4v2[4*__inum + (__new_gateval & 3)];
   break;
  case DT_16V:
   ind = ((__new_gateval & 3) << 2 | (__old_gateval & 3)) & 0xf;
   *gdel = du.d16v[ind];
   break;
  case DT_IS16V:
   /* notice size is 16 since x->x transition needed for strengths only chg */
   ind = 16*__inum
    + ((((__new_gateval & 3) << 2) | (__old_gateval & 3)) &0xf);
   *gdel = du.dis16v[ind];
   break;
  case DT_IS16V1:
   ind = 16*__inum
    + ((((__new_gateval & 3) << 2) | (__old_gateval & 3)) & 0xf);
   *gdel = (word64) du.dis16v1[ind];
   break;
  case DT_IS16V2:
   ind = 16*__inum
    + ((((__new_gateval & 3) << 2) | (__old_gateval & 3)) & 0xf);
   *gdel = (word64) du.dis16v2[ind];
   break;
  case DT_1X:
   /* must run delay expr. eval without context thread */
   sav_curthp = __cur_thd;
   __cur_thd = NULL;

   xsp = __eval_xpr(du.d1x);
   /* this handles required scaling */
   delx_to_deltim(gdel, du.d1x, xsp);
   __pop_xstk();
   __cur_thd = sav_curthp;
   break;
  case DT_4X:
   /* know at least 2 delays */
   /* trick here is do not know expr. ordering so can't precompute to z */ 
   /* must run delay expr. eval without context thread */
   sav_curthp = __cur_thd;
   __cur_thd = NULL;
   dxp = du.d4x[__new_gateval];
   if (dxp == NULL)
    {
     dxp = du.d4x[0];
     xsp = __eval_xpr(dxp);
     delx_to_deltim(&dv0, dxp, xsp);
     __pop_xstk();
     dxp = du.d4x[1];
     xsp = __eval_xpr(dxp);
     
     delx_to_deltim(&dv1, dxp, xsp);
     __pop_xstk();
     *gdel = (dv1 <= dv0) ? dv1 : dv0;
     __cur_thd = sav_curthp;
     return;
    }
   xsp = __eval_xpr(dxp);
   delx_to_deltim(gdel, dxp, xsp);
   __pop_xstk();
   __cur_thd = sav_curthp;
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * DELAY CONVERSION ROUTINES
 */

/*
 * given a delay expression - scale and convert to word64 delay value
 * no checking - 64 bits 
 */
static void delx_to_deltim(word64 *dval, struct expr_t *dxp,
 struct xstk_t *xsp)
{
 word64 t;
 double d1;
 struct mod_t *mdp;

 mdp = __inst_mod;
 if (dxp->is_real)
  {
   memcpy(&d1, xsp->ap, sizeof(double));
   real_to_ticks(dval, d1, mdp);
   return;
  }
 if (xsp->bp[0] != 0L)
  {
   __sgfwarn(565, "delay expression #(%s) evaluates to x/z changed to #0",
    __msgexpr_tostr(__xs, dxp));
   t = 0ULL; 
   *dval = t;
   return;
  }
 t = (word64) xsp->ap[0];
 if (xsp->xslen > WBITS) t |= (((word64) xsp->ap[1]) << 32);

 if (!mdp->mno_unitcnv) cnv_num64to_ticks_(*dval, t, mdp);
 else *dval = t;
}

/*
 * convert a real to ticks - here must scale (multiply before) conversion
 * rule is: multiply to get ticks, round, then convert to time 
 *
 * notice this is only routine that is called even for mods that do not
 * need scaling
 */ 
static void real_to_ticks(word64 *tim, double d1, struct mod_t *mdp)
{
 int32 unit;

 if (!mdp->mno_unitcnv)
  {
    unit = __des_timeprec - mdp->mtime_units;
    d1 *= __dbl_toticks_tab[unit];
  }
 if (!__real_to_v64tim(tim, d1))
  __sgfwarn(563, "error in conversion of real %g to 64 bit time - set to %s",
   d1, __to_timstr(__xs, tim));
} 

/*
 * convert a real value to a 64 bit word32 time
 * uses normal Verilog rounding from 0.5
 * return F if error in conversion but still set some time value
 * illegal if called with negative real
 *
 * this now assumes that that double to word32 conversion works
 * used to not work on Machten 
 */
extern int32 __real_to_v64tim(word64 *tim, double dv)
{
 *tim = (word64) (dv + 0.500000);
 return(TRUE);

 /* ---
 double d1, d2, d3, dbase;
 int32 expbits, good;

 good = TRUE;
 tim->th = tim->tl = 0L;
 if (dv < 0.0) return(FALSE);
 -* see if value fits in 31 bits *-
 if (dv <= (double) 0x7fffffff)
  {
   -* cannot use c compiler rounding since verilog is to higher magnitude *-
   tim->tl = ver_rword(dv);
   return(TRUE);
  }

 -* first catch overflow *-
 d1 = frexp(dv, &expbits);
 if (errno != 0) good = FALSE;
 -* know since 0.5 <= d1 < 1.0 will be no bigger than 2*62 - 1 *-  
 if (expbits > 64) return(FALSE);
 dv += 0.500000000;
 -* base is 2**32 *-
 dbase = ((double) ((word32) 0xffffffff)) + 1.0; 
 d1 = floor(dv/dbase);
 d2 = drem(dv, dbase);
 tim->th = (word32) d1; 
 tim->tl = (word32) d2; 
 -* DBG remove ---
 __v64_to_real(&d3, tim);
 if (fabs(dv - d3) >= EPSILON) __misc_terr(__FILE__, __LINE__);
 --- *-
 return(good);
 -- */
}

/*
 * convert from ticks to a module's user time - divide by units pow 10 diff
 * only called for modules where time needs unscaling
 * know this will always succeed
 */
extern void __cnv_ticks_tonum64(word64 *usertim, word64 tickstim,
 struct mod_t *mdp)
{ 
 word64 quot, rem, tscale;
 int32 unit;

 /* this subtract backwards here */
 unit = __des_timeprec - mdp->mtime_units;

 tscale = __itoticks_tab[unit];
 rem = tickstim % tscale;  
 quot = tickstim/tscale;  
 if (rem != 0ULL) 
  {
   /* divide scale period by 2 */
   tscale = tscale/2;
   if (rem >= tscale) quot++;
  }
 *usertim = quot;
}

/*
 * DELAY PREPARATION ROUTINES
 */

/*
 * after parameter change during SDF annotation re-prepare delays
 *
 * for each parameter parm npp list contains each RHS delay expr.
 * reference to parameter - afer change re-prep each delay again
 */
extern void __re_prep_dels(struct net_t *parmnp, struct itree_t *itp,
 struct mod_t *mdp, int32 from_vpi)
{
 register struct parmnet_pin_t *pnpp;  
 int32 sav_declobj;
 struct gate_t gwrk;
 struct net_t *np;
 struct gate_t *gp;
 struct conta_t *cap;
 struct st_t *stp;
 struct delctrl_t *dctp;
 struct sy_t tmpsym;
 struct spcpth_t *pthp;
 struct tchk_t *tcp;
 char s1[RECLEN];

 __push_wrkitstk(mdp, 0);
 for (pnpp = (struct parmnet_pin_t *) parmnp->nlds; pnpp != NULL;
  pnpp = pnpp->pnpnxt)
  { 
   switch ((byte) pnpp->pnptyp) {
    case PNP_GATEDEL:
     gp = pnpp->elpnp.egp;
     __prep_delay(&gwrk, pnpp->pnplp, FALSE, FALSE, "prim delay annotate",
      FALSE, gp->gsym, FALSE);
     if (__nd_neg_del_warn)
      {
       __gferr(972, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
        "gate %s annotated delay negative (0 used)", gp->gsym->synam);
        __nd_neg_del_warn = FALSE;
      } 
     /* free old */
     __free_del(gp->g_du, gp->g_delrep, mdp->flatinum);
     gp->g_du = gwrk.g_du;
     gp->g_delrep = gwrk.g_delrep;
     if (__sdf_verbose && (!from_vpi || __debug_flg))
      emit_reprepdev_verbmsg(gp, &gwrk, mdp, itp, from_vpi);
     break;
    case PNP_CONTADEL:
     /* SJM 09/28/02 - know reprep list always master even if per bit */
     cap = pnpp->elpnp.ecap;
      
     __prep_delay(&gwrk, pnpp->pnplp, FALSE, FALSE,
      "continuous assignment delay", FALSE, cap->casym, FALSE);
     if (__nd_neg_del_warn)
      {
       __gferr(973, cap->casym->syfnam_ind, cap->casym->sylin_cnt,
        "annotated continuous assign delay negative (0 used)");
        __nd_neg_del_warn = FALSE;
      } 
     __free_del(cap->ca_du, cap->ca_delrep, mdp->flatinum);
     cap->ca_du = gwrk.g_du;
     cap->ca_delrep = gwrk.g_delrep;
     if (__sdf_verbose && (!from_vpi || __debug_flg))
      emit_reprepconta_verbmsg(cap, &gwrk, mdp, itp, from_vpi);
     break;
    case PNP_NETDEL:
     np = pnpp->elpnp.enp;
     __prep_delay(&gwrk, pnpp->pnplp, FALSE,
      (np->ntyp == N_TRIREG), "annotated path delay", TRUE, np->nsym, FALSE); 
 
     if (__nd_neg_del_warn)
      {
       __gferr(971, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
        "annotated wire %s delay negative (0 used)", np->nsym->synam);
       __nd_neg_del_warn = FALSE;
      } 
     __free_del(np->nu.rngdwir->n_du, np->nu.rngdwir->n_delrep, mdp->flatinum);
     np->nu.rngdwir->n_delrep = gwrk.g_delrep;
     np->nu.rngdwir->n_du = gwrk.g_du;
     if (__sdf_verbose && (!from_vpi || __debug_flg))
      emit_reprepnet_verbmsg(np, &gwrk, mdp, itp, from_vpi);
     break;
    case PNP_PROCDCTRL:
     dctp = pnpp->elpnp.edctp;
     /* 01/25/00 SJM - fixed s now d not need stmt (maybe not around) */
     /* but pointing to dctrl and using actionst if present */
     if (dctp->actionst != NULL) stp = dctp->actionst; else stp = NULL;
     if (stp != NULL)
      {
       tmpsym.syfnam_ind = stp->stfnam_ind;
       tmpsym.sylin_cnt = stp->stlin_cnt;
      }
     else { tmpsym.syfnam_ind = 0; tmpsym.sylin_cnt = 0; }
     
     __prep_delay(&gwrk, pnpp->pnplp, FALSE, FALSE,
      "defparam annotate to procedural delay control", FALSE, &tmpsym, FALSE);
     if (__nd_neg_del_warn)
      {
       __sgferr(974,
        "defparam delay control annotate negative delay illegal (0 used)");
       __nd_neg_del_warn = FALSE;
      } 
     __free_del(dctp->dc_du, dctp->dc_delrep, mdp->flatinum);
     dctp->dc_delrep = gwrk.g_delrep;
     dctp->dc_du = gwrk.g_du;
     if (__sdf_verbose && (!from_vpi || __debug_flg))
      emit_reprepstmt_verbmsg(stp, &gwrk, mdp, itp, from_vpi);
     break;
    case PNP_PATHDEL:
     sav_declobj = __cur_declobj;
     __cur_declobj = SPECIFY;
     pthp = pnpp->elpnp.epthp;
     /* prepare the delay - notice this uses inst mod */
     __prep_delay(&gwrk, pnpp->pnplp, TRUE, FALSE, "path delay annotate",
      TRUE, pthp->pthsym, TRUE);
     if (__nd_neg_del_warn)
      {
      __gferr(981, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt, 
        "annotate path delay negative (used 0)");
       __nd_neg_del_warn = FALSE;
      } 
     __free_del(pthp->pth_du, pthp->pth_delrep, mdp->flatinum);
     pthp->pth_delrep = gwrk.g_delrep;
     pthp->pth_du = gwrk.g_du;
     if (__sdf_verbose && (!from_vpi || __debug_flg))
      emit_reprepiopath_verbmsg(pthp, &gwrk, mdp, itp, from_vpi);
     __cur_declobj = sav_declobj;
     break;
    case PNP_TCHKP1:
     strcpy(s1, "annotate first timing check limit");
     goto ano_tchk;
    case PNP_TCHKP2:
     strcpy(s1, "annotate second timing check limit");
ano_tchk:
     sav_declobj = __cur_declobj;
     __cur_declobj = SPECIFY;
     tcp = pnpp->elpnp.etcp;
     __prep_delay(&gwrk, pnpp->pnplp, FALSE, FALSE, s1, TRUE, tcp->tcsym,
      TRUE);
     if (__nd_neg_del_warn)
      {
       __gfwarn(601, tcp->tcsym->syfnam_ind, tcp->tcsym->sylin_cnt,
        "%s negative delay changed to 0 (ok for timing verifier)", s1);
       __nd_neg_del_warn = FALSE;
      }
     if (pnpp->pnptyp == PNP_TCHKP1)
      {
       __free_del(tcp->tclim_du, tcp->tc_delrep, mdp->flatinum);
       tcp->tc_delrep = gwrk.g_delrep;
       tcp->tclim_du = gwrk.g_du;
      }
     else
      {
       __free_del(tcp->tclim2_du, tcp->tc_delrep2, mdp->flatinum);
       tcp->tc_delrep2 = gwrk.g_delrep;
       tcp->tclim2_du = gwrk.g_du;
      }
     if (__sdf_verbose && (!from_vpi || __debug_flg))
      emit_repreptclim_verbmsg(tcp, &gwrk, mdp, itp, pnpp->pnptyp, from_vpi);
     __cur_declobj = sav_declobj;
     break;
    default: __case_terr(__FILE__, __LINE__);
   }
  }
 __pop_wrkitstk();
}
       
/*
 * emit changed param new primitive delay
 */
static void emit_reprepdev_verbmsg(struct gate_t *gp, struct gate_t *ogp,
 struct mod_t *mdp, struct itree_t *itp, int32 from_vpi)
{
 struct itree_t *itp2;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[10];

 if (itp == NULL) { itp2 = mdp->moditps[0]; sprintf(s1, "'*' first"); }
 else { __msg2_blditree(s1, itp); itp2 = itp; }
 if (from_vpi) strcpy(s4, "VPI_ PLI"); else strcpy(s4, "SDF"); 
 __push_itstk(itp2);
 __cv_msg(
  "  %s changed defparam: delay set to (%s) prim %s %s at %s in %s\n", s4,
  __bld_delay_str(s2, ogp->g_du, ogp->g_delrep), gp->gmsym->synam,
  gp->gsym->synam, __bld_lineloc(s3, gp->gsym->syfnam_ind,
  gp->gsym->sylin_cnt), s1); 
 __pop_itstk();
}

/*
 * emit changed param new continuous assign delay
 */
static void emit_reprepconta_verbmsg(struct conta_t *cap, struct gate_t *ogp,
 struct mod_t *mdp, struct itree_t *itp, int32 from_vpi)
{
 struct itree_t *itp2;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[10];

 if (itp == NULL) { itp2 = mdp->moditps[0]; sprintf(s1, "'*' first"); }
 else { __msg2_blditree(s1, itp); itp2 = itp; }
 if (from_vpi) strcpy(s4, "VPI_ PLI"); else strcpy(s4, "SDF"); 

 __push_itstk(itp2);
 __cv_msg(
  "  %s changed defparam: delay set to (%s) cont. assign at %s in %s\n", s4,
  __bld_delay_str(s2, ogp->g_du, ogp->g_delrep), __bld_lineloc(s3,
  cap->casym->syfnam_ind, cap->casym->sylin_cnt), s1);
 __pop_itstk();
}

/*
 * emit changed param new net delay
 */
static void emit_reprepnet_verbmsg(struct net_t *np, struct gate_t *ogp,
 struct mod_t *mdp, struct itree_t *itp, int32 from_vpi)
{
 struct itree_t *itp2;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[10];

 if (itp == NULL) { itp2 = mdp->moditps[0]; sprintf(s1, "'*' first"); }
 else { __msg2_blditree(s1, itp); itp2 = itp; }
 if (from_vpi) strcpy(s4, "VPI_ PLI"); else strcpy(s4, "SDF"); 

 __push_itstk(itp2);
 __cv_msg(
  "  %s changed defparam: delay set to (%s) net %s in %s at %s\n", s4,
  __bld_delay_str(s2, ogp->g_du, ogp->g_delrep), np->nsym->synam, s1,
  __bld_lineloc(s3, np->nsym->syfnam_ind, np->nsym->sylin_cnt));
 __pop_itstk();
}

/*
 * emit changed param new statement procedural delay control
 */
static void emit_reprepstmt_verbmsg(struct st_t *stp, struct gate_t *ogp,
 struct mod_t *mdp, struct itree_t *itp, int32 from_vpi)
{
 struct itree_t *itp2;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[10];

 if (itp == NULL) { itp2 = mdp->moditps[0]; sprintf(s1, "'*' first"); }
 else { __msg2_blditree(s1, itp); itp2 = itp; }
 if (from_vpi) strcpy(s4, "VPI_ PLI"); else strcpy(s4, "SDF"); 
 __push_itstk(itp2);
 if (stp != NULL)
  {
   __cv_msg(
    "  %s changed defparam: delay set to (%s) statement at %s in %s\n", s4,
    __bld_delay_str(s3, ogp->g_du, ogp->g_delrep),
    __bld_lineloc(s2, stp->stfnam_ind, stp->stlin_cnt), s1);
  }
 else
  {
   __cv_msg(
    "  %s changed defparam: delay set to (%s) statement in %s\n", s4,
    __bld_delay_str(s3, ogp->g_du, ogp->g_delrep), s1);
  }
 __pop_itstk();
}

/*
 * emit changed param new iopath delay
 */
static void emit_reprepiopath_verbmsg(struct spcpth_t *pthp,
 struct gate_t *ogp, struct mod_t *mdp, struct itree_t *itp, int32 from_vpi)
{
 struct itree_t *itp2;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[10];

 if (itp == NULL) { itp2 = mdp->moditps[0]; sprintf(s1, "'*' first"); }
 else { __msg2_blditree(s1, itp); itp2 = itp; }
 if (from_vpi) strcpy(s4, "VPI_ PLI"); else strcpy(s4, "SDF"); 

 __push_itstk(itp2);
 __cv_msg("  %s changed specparam: delay set to (%s) path at %s in %s\n", s4,
  __bld_delay_str(s3, ogp->g_du, ogp->g_delrep), __bld_lineloc(s2,
  pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt), s1);
 __pop_itstk();
}

/*
 * emit changed param new timing check (either first or 2nd) 
 */
static void emit_repreptclim_verbmsg(struct tchk_t *tcp, struct gate_t *ogp,
 struct mod_t *mdp, struct itree_t *itp, word32 pnptyp, int32 from_vpi)
{
 struct itree_t *itp2;
 char s1[RECLEN], s2[10], s3[RECLEN], s4[RECLEN], s5[10];

 if (itp == NULL) { itp2 = mdp->moditps[0]; sprintf(s1, "'*' first"); }
 else { __msg2_blditree(s1, itp); itp2 = itp; }
 if (pnptyp == PNP_TCHKP1) strcpy(s2, ""); else strcpy(s2, " 2nd");
 if (from_vpi) strcpy(s5, "VPI_ PLI"); else strcpy(s5, "SDF"); 

 __push_itstk(itp2);
 __cv_msg(
  "  %s changed specparam:%s limit set to %s for %s timing check at %s in %s\n",
  s5, s2, __bld_delay_str(s4, ogp->g_du, ogp->g_delrep), __to_tcnam(s3,
  tcp->tchktyp), __bld_lineloc(s4, tcp->tcsym->syfnam_ind,
  tcp->tcsym->sylin_cnt), s1);
 __pop_itstk();
}

/*
 * ROUTINES TO PREP DELAYS
 */

/*
 * wrapper around prep delay that copies, checks again, and frees
 *
 * needed because can change parameters in delay expressions multiple
 * times so much copy each time fold and check copy (maybe newly wrong) and
 * use copy for delay expr. but then can change later so need to do again
 */
extern void __prep_delay(struct gate_t *gp, struct paramlst_t *dhdr,
 int32 ispath, int32 is_trireg, char *emsg, int32 mustbeconst, struct sy_t *dsym, 
 int32 is_spec)
{
 register struct paramlst_t *pmp;
 int32 sav_nowarns, sav_noinforms, sav_lcnt, sav_fnind, sav_ecnt;
 struct paramlst_t *dhdr2;

 /* any warns or informs already emitted */ 
 sav_nowarns = __no_warns;
 sav_noinforms = __no_informs;
 __no_warns = TRUE;
 __no_informs = TRUE;
 sav_lcnt = __slin_cnt;
 sav_fnind = __sfnam_ind;
 __sfnam_ind = (int32) dsym->syfnam_ind;
 __slin_cnt = dsym->sylin_cnt;
 sav_ecnt = __pv_err_cnt;
 
 dhdr2 = __copy_dellst(dhdr);
 if (!is_spec) __chk_delparams(dhdr2, emsg, mustbeconst);
 else
  {
   for (pmp = dhdr2; pmp != NULL; pmp = pmp->pmlnxt)
    { __chk_spec_delay(pmp->plxndp, emsg); }
  }
 __no_warns = sav_nowarns;
 __no_informs = sav_noinforms;
 __slin_cnt = sav_lcnt;
 __sfnam_ind = sav_fnind;
 if (__pv_err_cnt == sav_ecnt) prep2_delay(gp, dhdr2, ispath, is_trireg);
 __free_dellst(dhdr2);
}

/*
 * prepare gate and continuous assignment preprocessed delay arrays
 * this changes representation for gate del_u field
 * also used for wire delays
 *
 * this routine assumes at least dummy module context set
 *
 * know delay list already checked and if error will not get here
 * also no scaling or conversion from reals performed by fix_nl 
 *
 * if is trireg, then know will be exactly 3 values in list 
 * that are constants
 *
 * for path delays only DT_1V and DT_16V forms possible because
 * of table look up algorithm (user may code 1,2,3,6,12) 
 */
static void prep2_delay(struct gate_t *gp, struct paramlst_t *dhdr,
 int32 ispath, int32 is_trireg)
{
 register int32 i;
 register struct paramlst_t *pmp;
 int32 nparams, has_xpr, has_is;
 word64 rise, fall, toz;
 word64 *dtab, xt[16];
 struct expr_t *xa[12], *xp1;

 /* DBG remove --
 extern void __dbg_dump_del(union del_u, word32);
 --- */

 /* no delays */
 if (dhdr == NULL) { gp->g_du.d1v = NULL; gp->g_delrep = DT_NONE; return; }

 /* assume no negative delay constant expr. */
 __nd_neg_del_warn = FALSE;

 /* case 1: one delay special case */
 /* also 1x form handled here */
 if (dhdr->pmlnxt == NULL)
  {
   xp1 = dhdr->plxndp;
   prep_1delform(gp, xp1);
   /* trireg 1 value must be converted to 4v since need special toz decay */
   if (is_trireg) convert_trireg_1vto4v(gp); 
   goto done;
  }

 /* figure out representation needed */
 has_xpr = FALSE;
 has_is = FALSE;
 for (pmp = dhdr, nparams = 0; pmp != NULL; pmp = pmp->pmlnxt, nparams++)
  {
   xa[nparams] = pmp->plxndp;
   switch ((byte) xa[nparams]->optyp) {
    case NUMBER: case REALNUM: break;
    case ISNUMBER: case ISREALNUM: has_is = TRUE; break;
    default: has_xpr = TRUE;
   }  
  }

 /* case 2: has expression - cannot be path case */
 if (has_xpr)
  {
   /* DBG remove --- */
   if (ispath) __arg_terr(__FILE__, __LINE__);
   /* -- */

   /* if at least one expr, all must evaluate at run time - leave xpr */
   gp->g_du.d4x = (struct expr_t **) __my_malloc(4*sizeof(struct expr_t *));
   /* since indexing by new 2 bit value order is f, r, z, x */
   gp->g_du.d4x[0] = __copy_expr(xa[1]);
   /* must fold all of these this is point where copy is fixed (elaborated) */
   gp->g_du.d4x[1] = __copy_expr(xa[0]);
   if (nparams == 2) gp->g_du.d4x[2] = NULL;
   else 
    {
     gp->g_du.d4x[2] = __copy_expr(xa[2]);
    }
   gp->g_du.d4x[3] = NULL;
   gp->g_delrep = DT_4X;
   goto done;
  }
 
 /* case 3: IS form more than 1 val (if any IS all must be) */
 if (has_is)
  {
   if (ispath) prep_path_is_dels(gp, xa, nparams);
   else prep_prim_is_dels(gp, xa, nparams, is_trireg); 
   goto done;
  }

 /* case 4a: non IS path form and more than 1 value */
 /* here must be 16V form */
 if (ispath)
  {
   /* first evaluate expr., prep. constant delay form */
   for (i = 0; i < nparams; i++) nonis_to_delval(&(xt[i]), xa[i]);

   dtab = (word64 *) __my_malloc(16*sizeof(word64));
   /* know not 1 value but can be no convert to number 2,3,6, or 12 */
   __fill_16vconst(dtab, xt, nparams);
   gp->g_du.d16v = dtab;
   gp->g_delrep = DT_16V;
   goto done;
  }
 /* case 4b: non IS primitive form */
 /* DBG remove ---
 if (nparams > 3) __arg_terr(__FILE__, __LINE__);  
 --- */
 dtab = (word64 *) __my_malloc(4*sizeof(word64));
 /* fill the 3 values with scaled ticks */
 nonis_to_delval(&rise, xa[0]);
 nonis_to_delval(&fall, xa[1]);
 if (nparams == 2) toz = 0ULL; else nonis_to_delval(&toz, xa[2]);
 /* this handles special processing for tri reg if needed */
 __fill_4vconst(dtab, &rise, &fall, &toz, nparams, is_trireg);
 gp->g_du.d4v = dtab;
 gp->g_delrep = DT_4V;

done:;
 /* --- DBG remove
 if (__debug_flg) __dbg_dump_del(gp->g_du, gp->g_delrep);
 --- */ 
}

/*
 * convert 1 non is form to a delay value
 * this takes a num expression checked else so is only for delay prep.
 */
static void nonis_to_delval(word64 *ticksval, struct expr_t *xp)
{
 double *dp, d1;
 word32 *wp;
 word64 delval;

 if (xp->optyp == REALNUM)
  {
   dp = (double *) &(__contab[xp->ru.xvi]);
   d1 = *dp;
   if (d1 < 0.0)
    {
neg_del:
     *ticksval = 0ULL;
     __nd_neg_del_warn = TRUE;
     return;
    }
   real_to_ticks(ticksval, d1, __inst_mod);
   return;
  }
 wp = &(__contab[xp->ru.xvi]);
 delval = (word64) wp[0];
 /* * SJM 08/17/04 - now any value can be signed - need warning */
 if (xp->szu.xclen > WBITS)
  {
   if (__is_lnegative(wp, xp->szu.xclen)) goto neg_del; 
   delval |= (((word64) wp[1]) << 32);
  }
 else
  {
   if (xp->has_sign && (wp[0] & (1 << (xp->szu.xclen - 1))) != 0)
    goto neg_del;
  }
 if (!__inst_mod->mno_unitcnv)
  cnv_num64to_ticks_(*ticksval, delval, __inst_mod);
 else *ticksval = delval;
}

/*
 * free one delay - lots of forms
 * union is 4 bytes in record, so do not need to free union
 * this is for after preparation freeing
 */
extern void __free_del(union del_u du, word32 drep, int32 numinsts)
{
 register int32 i;

 switch ((byte) drep) {
  case DT_1V:
   __my_free((char *) du.d1v, sizeof(word64));
   break;
  case DT_IS1V:
   __my_free((char *) du.dis1v, numinsts*sizeof(word64));
   break;
  case DT_IS1V1:
   __my_free((char *) du.dis1v1, numinsts);
   break;
  case DT_IS1V2:
   __my_free((char *) du.dis1v2, 2*numinsts);
   break;
  case DT_4V:
   __my_free((char *) du.d4v, 4*sizeof(word64));
   break;
  case DT_IS4V:
   __my_free((char *) du.dis4v, 4*numinsts*sizeof(word64));
   break;
  case DT_IS4V1:
   __my_free((char *) du.dis4v1, 4*numinsts);
   break;
  case DT_IS4V2:
   __my_free((char *) du.dis4v2, 4*2*numinsts);
   break;
  case DT_16V: __my_free((char *) du.d16v, 16*sizeof(word64)); break;
  case DT_IS16V:
   __my_free((char *) du.dis16v, 16*numinsts*sizeof(word64));
   break;
  case DT_IS16V1:
   __my_free((char *) du.dis16v1, 16*numinsts);
   break;
  case DT_IS16V2:
   __my_free((char *) du.dis16v2, 16*2*numinsts);
   break;
  case DT_1X:
   __free_xtree(du.d1x);
   break;
  case DT_4X:
   /* notice this is array of 4 expression pointers (ptr to ptrs) */
   for (i = 0; i < 3; i++) __free_xtree(du.d4x[i]);
   __my_free((char *) du.d4x, 4*sizeof(struct expr_t *));
   break;
  case DT_CMPLST:
   __free_dellst(du.pdels);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * free a delay parameter list
 */
extern void __free_dellst(struct paramlst_t *dhdr)
{
 register struct paramlst_t *pmp;
 struct paramlst_t *pmp2;

 for (pmp = dhdr; pmp != NULL;)
  {
   pmp2 = pmp;
   pmp = pmp2->pmlnxt;
   __free_xtree(pmp2->plxndp);
   __my_free((char *) pmp2, sizeof(struct paramlst_t));
  }
}

/*
 * prepare all 1 delay value cases
 */
static void prep_1delform(struct gate_t *gp, struct expr_t *xp1)
{
 register int32 ii;
 int32 minsiz, nbytes;
 word32 *wp, *wp2, w1;
 double d1, *dp;
 word64 *dtab, *dtab2, ticksval, delval;

 dtab = NULL;
 /* know both double and 64 bit time take 8 bytes dependent */
 nbytes = sizeof(word64)*__inst_mod->flatinum;     
 switch ((byte) xp1->optyp) {
  case NUMBER:
  case REALNUM:
   nonis_to_delval(&ticksval, xp1);
   gp->g_du.d1v = (word64 *) __my_malloc(sizeof(word64));
   *(gp->g_du.d1v) = ticksval;
   gp->g_delrep = DT_1V;
   break;
  case ISNUMBER:
   /* this is common because of back annotation for gate arrays */
   /* step 1 build the 8 byte case */
   /* LOOKATME - can dtab not be nil? */
   if (dtab == NULL) dtab = (word64 *) __my_malloc(nbytes);
   /* notice never x/z delays by here no matter what */
   if (xp1->szu.xclen > WBITS)
    {
     wp = &(__contab[xp1->ru.xvi]); 
     for (ii = 0; ii < __inst_mod->flatinum; ii++)
      {
       /* SJM 08/17/04 - now any value can be signed - need warning */
       if (__is_lnegative(wp, xp1->szu.xclen)) 
        {
         __nd_neg_del_warn = TRUE;
         dtab[ii] = 0ULL;
         continue;
        }
       dtab[ii] = ((word64) wp[2*ii]) | (((word64) wp[2*ii + 1]) << 32);
       if (!__inst_mod->mno_unitcnv)
	{
         delval = dtab[ii];
         cnv_num64to_ticks_(dtab[ii], delval, __inst_mod);
        }
      }
    }
   else
    {
     wp = &(__contab[xp1->ru.xvi]); 
     for (ii = 0; ii < __inst_mod->flatinum; ii++)
      {
       wp2 = &(wp[2*ii]);

       /* SJM 08/17/04 - now any value can be signed - need warning */
       if (xp1->has_sign && (wp2[0] & (1 << (xp1->szu.xclen - 1))) != 0)
        {
         __nd_neg_del_warn = TRUE;
         dtab[ii] = 0ULL;
         continue;
        }

       dtab[ii] = (word64) wp2[0];
       if (!__inst_mod->mno_unitcnv)
	{
         delval = dtab[ii];
         cnv_num64to_ticks_(dtab[ii], delval, __inst_mod);
        }
      }
    }
try_narrow:
   /* next determine if can be smaller - assume all fit in 1 byte */
   for (minsiz = 1, ii = 0; ii < __inst_mod->flatinum; ii++)
    {
     if (dtab[ii] > WORDMASK_ULL) { minsiz = 8; break; }
     w1 = (word32) (dtab[ii] & WORDMASK_ULL);
     if ((w1 & 0xffffff00L) == 0L) continue;
     if ((w1 & 0xffff0000L) == 0L) { minsiz = 2; continue; }
    }
   if (minsiz > 2)
    {
     dtab2 = (word64 *) __my_malloc(nbytes);
     memcpy(dtab2, dtab, nbytes);

     gp->g_du.dis1v = dtab2;
     gp->g_delrep = DT_IS1V;
     break;
    }
   if (minsiz == 1)
    {
     gp->g_du.dis1v1 = (byte *) __my_malloc(__inst_mod->flatinum);
     for (ii = 0; ii < __inst_mod->flatinum; ii++)
      gp->g_du.dis1v1[ii] = (byte) dtab[ii]; 
     gp->g_delrep = DT_IS1V1;
    }
   else 
    {
     gp->g_du.dis1v2 = (hword *) __my_malloc(2*__inst_mod->flatinum);
     for (ii = 0; ii < __inst_mod->flatinum; ii++)
      gp->g_du.dis1v2[ii] = (hword) dtab[ii]; 
     gp->g_delrep = DT_IS1V2;
    }
   break;
  case ISREALNUM: 
   /* here if value too big - silently truncates to mod 64 bits */
   if (dtab == NULL) dtab = (word64 *) __my_malloc(nbytes);
   dp = (double *) &(__contab[xp1->ru.xvi]);
   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    {
     /* for reals know fits and non x/z but must scale before conversion */
     d1 = dp[ii];
     if (d1 < 0.0) { dtab[ii] = 0ULL; __nd_neg_del_warn = TRUE; }
     else
      { 
       real_to_ticks(&ticksval, d1, __inst_mod);
       dtab[ii] = ticksval;
      } 
    }
   goto try_narrow;
  default:
   /* expr. that must be evaluated each time */
   /* must copy since delay parm list including expr. will be freed */
   gp->g_du.d1x = __copy_expr(xp1); 
   gp->g_delrep = DT_1X;
  }
 if (dtab != NULL) __my_free((char *) dtab, nbytes);
}

/*
 * for trireg convert 1v or is1v forms to 4v or is4v 
 */
static void convert_trireg_1vto4v(struct gate_t *gp)
{
 register int32 ii;
 int32 ninsts;
 word64 *dtab;
 byte *btab;
 hword *htab;

 ninsts = __inst_mod->flatinum;
 switch ((byte) gp->g_delrep) {
  case DT_1V:
   dtab = gp->g_du.d4v;
   gp->g_du.d4v = (word64 *) __my_malloc(4*sizeof(word64));
   gp->g_du.d4v[0] = dtab[0];
   gp->g_du.d4v[1] = dtab[0];
   gp->g_du.d4v[3] = dtab[0];
   gp->g_du.d4v[2] = 0ULL;
   __my_free((char *) dtab, sizeof(word64));
   break;
  case DT_IS1V:
   dtab = gp->g_du.dis1v;
   gp->g_du.dis4v = (word64 *) __my_malloc(4*ninsts*sizeof(word64));
   for (ii = 0; ii < ninsts; ii++) 
    {
     gp->g_du.dis4v[4*ii] = dtab[ii];
     gp->g_du.dis4v[4*ii + 1] = dtab[ii]; 
     gp->g_du.dis4v[4*ii + 3] = dtab[ii]; 
     gp->g_du.dis4v[4*ii + 2] = 0ULL;
    }
   __my_free((char *) dtab, ninsts*sizeof(word64));
   break;
  case DT_IS1V1:
   btab = gp->g_du.dis1v1;
   gp->g_du.dis4v1 = (byte *) __my_malloc(4*ninsts);
   for (ii = 0; ii < ninsts; ii++) 
    {
     gp->g_du.dis4v1[4*ii] = btab[ii];
     gp->g_du.dis4v1[4*ii + 1] = btab[ii]; 
     gp->g_du.dis4v1[4*ii + 3] = btab[ii]; 
     gp->g_du.dis4v1[4*ii + 2] = 0;
    }
   __my_free((char *) btab, 1*ninsts);
   break;
  case DT_IS1V2:
   htab = gp->g_du.dis1v2;
   gp->g_du.dis4v2 = (hword *) __my_malloc(4*2*ninsts);
   for (ii = 0; ii < ninsts; ii++) 
    {
     gp->g_du.dis4v2[4*ii] = htab[ii];
     gp->g_du.dis4v2[4*ii + 1] = htab[ii]; 
     gp->g_du.dis4v2[4*ii + 3] = htab[ii]; 
     gp->g_du.dis4v2[4*ii + 2] = 0;
    }
   __my_free((char *) htab, 2*ninsts);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * fill 4v constant where some values require 64 bit amount
 * idea is to fill d4v array with delays according to algorithm
 *
 * 4v only for primitive and wire delays
 */
extern void __fill_4vconst(word64 *dv4, word64 *rise, word64 *fall,
 word64 *toz, int32 nparams, int32 is_trireg)
{
 word64 ltmp;

 /* 4 value array first is fall (destination state as index) */
 dv4[0] = *fall;
 dv4[1] = *rise;
 /* 2 delay case, use minimum for both to-x and to-z */
 if (nparams == 2)
  {
   if (is_trireg)
    {
     if (dv4[0] <= dv4[1]) dv4[3] = dv4[0]; else dv4[3] = dv4[1];
     dv4[2] = 0ULL; 
    }
   else
    {
     if (dv4[0] <= dv4[1]) dv4[3] = dv4[2] = dv4[0];
     else dv4[3] = dv4[2] = dv4[1];
    }
  }
 else
  {
   dv4[2] = *toz;
   /* for trireg to x is smallest of rise and fall */ 
   if (is_trireg)
    { if (dv4[0] <= dv4[1]) ltmp = dv4[0]; else ltmp = dv4[1]; }
   else
    {
     /* index 3 (to-x) is minimum of all 3 */
     if (dv4[0] <= dv4[1] && dv4[0] <= dv4[2]) ltmp = dv4[0];
     else
      { if (dv4[1] < dv4[2]) ltmp = dv4[1]; else ltmp = dv4[2]; }
    }
   dv4[3] = ltmp;
  }
 /* LOOKATME */
 /* form LRM for wider than 1 cont. assign, v[0] is to-0, v[1] is other */
 /* and v[3] is to-z */
 /* according to LRM this should be overriden to 1, but OVIsim uses fast */
 /* as possible to x (wrong just low bit but still uses it */ 
 /* *** if (lhswid > 1) dv4[3] = dv4[1]; */
}

/* ---
12 form: (0->1, 1->0, 0->z, z->1, 1->z, z->0,
          0->x, x->1, 1->x, x->0, x->z, z->x)  
0000 --
0001 1->0 - 1 
0010 z->0 - 5
0011 x->0 - 9
0100 0->1 - 0
0101 --
0110 z->1 - 3
0111 x->1 - 7
1000 0->z - 2
1001 1->z - 4
1010 ---
1011 x->z - 10
1100 0->x - 6
1101 1->x - 8
1110 z->x - 11
--- */

/*
 * fill a 16v table from table of expressions  
 * know need 16v because specify and nparams 2,3,6, or 12
 */
extern void __fill_16vconst(word64 *dtab, word64 *xt, int32 nparams)
{
 /* have all values just rearrange into nnoo form */
 if (nparams == 12)
  {
move_to_dtab:
   /* FIXME - why needed - same value can happen? */
   dtab[0] = 0ULL; 
   dtab[5] = dtab[10] = dtab[15] = dtab[0];
   dtab[1] = xt[1]; dtab[2] = xt[5]; dtab[3] = xt[9]; dtab[4] = xt[0];
   dtab[6] = xt[3]; dtab[7] = xt[7]; dtab[8] = xt[2]; dtab[9] = xt[4];
   dtab[11] = xt[10]; dtab[12] = xt[6]; dtab[13] = xt[8];
   dtab[14] = xt[11];
   return;
  }

 /* must convert separately from building nnoo dtab */
 if (nparams == 2) { xt[2] = xt[3] = xt[0]; xt[4] = xt[5] = xt[1]; }
 else if (nparams == 3)
  { 
   /* 0 thru 2 (rise, fall, toz) good */
   /* z->1 is rising */
   xt[3] = xt[0];
   /* 1->z is toz */
   xt[4] = xt[2];
   /* z->0 is falling */
   xt[5] = xt[1];
  }
 /* 6 value case is (0->1, 1->0, 0->z, z->1, 1->z, z->0) */

 /* here know 0-5 values of xt filled - next fill 6-11 x transitions */ 
 /* 0->x min(0->1, 0->z) */ 
 xt[6] = (xt[0] < xt[2]) ? xt[0] : xt[2]; 
 /* 1->x min(1->0, 1->z) */ 
 xt[8] = (xt[1] < xt[4]) ? xt[1] : xt[4]; 
 /* z->x min(z->0, z->1) */ 
 xt[11] = (xt[5] < xt[3]) ? xt[5] : xt[3]; 
 /* x->0 max(1->0, z->0) */ 
 xt[9] = (xt[1] > xt[5]) ? xt[1] : xt[5]; 
 /* x->1 max(z->1, 0->1) */ 
 xt[7] = (xt[3] > xt[0]) ? xt[3] : xt[0]; 
 /* x->z max(1->z, 0->z) */ 
 xt[10] = (xt[4] > xt[2]) ? xt[4] : xt[2]; 
 goto move_to_dtab;
}

/*
 * handle the primitive non path IS 2v and 3v cases
 * this always builds an IS4V table
 */
static void prep_prim_is_dels(struct gate_t *gp, struct expr_t **xa,
 int32 nparams, int32 is_trireg)
{ 
 register int32 ii;
 int32 minsiz, nbytes;
 word32 w1;
 byte *btab;
 hword *htab;
 word64 ticksval;
 word64 *dtab, *dtab2;

 if (nparams > 3) __arg_terr(__FILE__, __LINE__);

 /* if some NUBMER and some IS then all must be IS */
 nbytes = 4*sizeof(word64)*__inst_mod->flatinum;
 dtab = (word64 *) __my_malloc(nbytes);
 fill_1col_isdel(dtab, 0, xa[0], 4);
 fill_1col_isdel(dtab, 1, xa[1], 4);
 /* if 3 fill column, if 2 zero toz column */
 if (nparams == 3) fill_1col_isdel(dtab, 2, xa[2], 4);
 else
  {
   ticksval = 0ULL;
   for (ii = 0; ii < __inst_mod->flatinum; ii++) dtab[4*ii + 2] = ticksval;
  } 
 gp->g_delrep = DT_IS4V;
 /* arrange 4 values table for look up - applies missing values rules */  
 /* to each row of IS form column */ 
 for (ii = 0; ii < __inst_mod->flatinum; ii++)
  {
   __fill_4vconst(&(dtab[4*ii]), &(dtab[4*ii]), &(dtab[4*ii + 1]),
    &(dtab[4*ii + 2]), nparams, is_trireg); 
  }

 /* now know inum by 4 table filled */
 /* attempt to change form */
 for (minsiz = 1, ii = 0; ii < 4*__inst_mod->flatinum; ii++) 
  {
   if (dtab[ii] > WORDMASK_ULL) { minsiz = 8; break; }
   w1 = (word32) (dtab[ii] & WORDMASK_ULL);
   if ((w1 & 0xffffff00) == 0L) continue;
   if ((w1 & 0xffff0000) == 0L) { minsiz = 2; continue; }
  }
 if (minsiz > 2)
  {
   dtab2 = (word64 *) __my_malloc(nbytes);
   memcpy(dtab2, dtab, nbytes);
   gp->g_du.dis4v = dtab2;
   gp->g_delrep = DT_IS4V;
   goto done;
  }

 if (minsiz == 1)
  {
   btab = (byte *) __my_malloc(4*__inst_mod->flatinum);
   for (ii = 0; ii < 4*__inst_mod->flatinum; ii++)
    btab[ii] = (byte) dtab[ii];
   gp->g_du.dis4v1 = btab;
   gp->g_delrep = DT_IS4V1;
  }
 else 
  {
   htab = (hword *) __my_malloc(4*2*__inst_mod->flatinum);
   for (ii = 0; ii < 4*__inst_mod->flatinum; ii++)
    htab[ii] = (hword) dtab[ii];
   gp->g_du.dis4v2 = htab;
   gp->g_delrep = DT_IS4V2;
  }
done:
 __my_free((char *) dtab, nbytes);
}

/*
 * prep the IS 16 value (4 bit change table look up) forms
 * know 1 delay case handled elsewhere (do not need 16V) 
 *
 *
 * possibilities are 2, 3, 6, 12
 * dtab is 16 by flatinum table (need 0 delays for strength only change)
 */
static void prep_path_is_dels(struct gate_t *gp, struct expr_t **xa,
 int32 nparams)
{
 register int32 ii;
 int32 minsiz, nbytes;
 byte *btab;
 hword *htab;
 word32 w1;
 word64 tim1, tim2;
 word64 *dtab, *dtab2;

 /* build big enough table */
 nbytes = 16*sizeof(word64)*__inst_mod->flatinum;
 dtab = (word64 *) __my_malloc(nbytes);
 /* have 16 columns to fill */

 /* step 1: zero entire table */
 memset((char *) dtab, 0, nbytes);
 /* case 1: 12 (all except for 3 unused coded) */
 if (nparams == 12)
  {
   fill_1col_isdel(dtab, 1, xa[1], 16);
   fill_1col_isdel(dtab, 2, xa[5], 16);
   fill_1col_isdel(dtab, 3, xa[9], 16);
   fill_1col_isdel(dtab, 4, xa[0], 16);
   fill_1col_isdel(dtab, 6, xa[3], 16);
   fill_1col_isdel(dtab, 7, xa[7], 16);
   fill_1col_isdel(dtab, 8, xa[2], 16);
   fill_1col_isdel(dtab, 9, xa[4], 16);
   fill_1col_isdel(dtab, 11, xa[10], 16);
   fill_1col_isdel(dtab, 12, xa[6], 16);
   fill_1col_isdel(dtab, 13, xa[8], 16);
   fill_1col_isdel(dtab, 14, xa[11], 16);
   goto minimize;
  }

 /* case 2: 6 coded delays */
 if (nparams == 6)
  {
   fill_1col_isdel(dtab, 4, xa[0], 16);
   fill_1col_isdel(dtab, 1, xa[1], 16);
   fill_1col_isdel(dtab, 8, xa[2], 16);
   fill_1col_isdel(dtab, 6, xa[3], 16);
   fill_1col_isdel(dtab, 9, xa[4], 16);
   fill_1col_isdel(dtab, 2, xa[5], 16);
  }
 /* case 3: handle similar 2 and 3 value case */
 /* dtab is 16 position nnoo index form */ 
 /* for 2,3 and 6 cases first fill dtab 6 transitions not involving x */
 else if (nparams == 2)
  {
   /* first of 2 is all rising transitions */
   /* fill dtab[4] - xa[0] (0->1) */
   fill_1col_isdel(dtab, 4, xa[0], 16);
   /* fill dtab[8] - xa[2] (0->z) */
   /* fill dtab[6] - xa[3] (z->1) */
   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    dtab[16*ii + 8] = dtab[16*ii + 6] = dtab[16*ii + 4];

   /* 2nd of 2 is all falling transitions */
   /* fill dtab[1] - xa[1] (1->0) */
   fill_1col_isdel(dtab, 1, xa[1], 16);
   /* fill dtab[9] - xa[4] (1->z) */
   /* fill dtab[2] - xa[5] (z->0) */
   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    dtab[16*ii + 9] = dtab[16*ii + 2] = dtab[16*ii + 1];
  }
 else if (nparams == 3)
  {
   /* rising of 3 */
   /* fill dtab[4] - xa[0] (0->1) */
   fill_1col_isdel(dtab, 4, xa[0], 16);
   /* fill dtab[6] - xa[3] (0->1) */
   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    dtab[16*ii + 6] = dtab[16*ii + 4];

   /* falling of 3 */
   /* fill dtab[1] - xa[1] (1->0) */
   fill_1col_isdel(dtab, 1, xa[1], 16);
   /* fill dtab[2] - xa[5] (z->0) */
   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    dtab[16*ii + 2] = dtab[16*ii + 1];

   /* to z */
   /* fill dtab[8] - xa[2] (0->z) */
   fill_1col_isdel(dtab, 8, xa[2], 16);
   /* fill dtab[9] - xa[4] (1->z) */
   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    dtab[16*ii + 9] = dtab[16*ii + 8];
  }
 /* next row by row (per inst.) */
 for (ii = 0; ii < __inst_mod->flatinum; ii++)
  {
   /* dtab columns for low 6 filled by here */
   /* 0->x min(0->1, 0->z) */ 
   tim1 = dtab[16*ii + 4];
   tim2 = dtab[16*ii + 8];
   dtab[16*ii + 12] = (tim1 < tim2) ? tim1 : tim2;
   /* 1->x min(1->0, 1->z) */ 
   tim1 = dtab[16*ii + 1];
   tim2 = dtab[16*ii + 9];
   dtab[16*ii + 13] = (tim1 < tim2) ? tim1 : tim2;
   /* z->x min(z->0, z->1) */ 
   tim1 = dtab[16*ii + 2];
   tim2 = dtab[16*ii + 6];
   dtab[16*ii + 14] = (tim1 < tim2) ? tim1 : tim2;
   /* x->0 max(1->0, z->0) */ 
   tim1 = dtab[16*ii + 1];
   tim2 = dtab[16*ii + 2];
   dtab[16*ii + 3] = (tim1 > tim2) ? tim1 : tim2; 
   /* x->1 max(z->1, 0->1) */ 
   tim1 = dtab[16*ii + 6];
   tim2 = dtab[16*ii + 4];
   dtab[16*ii + 7] = (tim1 > tim2) ? tim1 : tim2; 
   /* x->z max(1->z, 0->z) */ 
   tim1 = dtab[16*ii + 9];
   tim2 = dtab[16*ii + 8];
   dtab[16*ii + 11] = (tim1 > tim2) ? tim1 : tim2; 
  }

 /* now know inum by 16 table filled */
 /* attempt to change form */
minimize:
 for (minsiz = 1, ii = 0; ii < 16*__inst_mod->flatinum; ii++) 
  {
   if (dtab[ii] > WORDMASK_ULL) { minsiz = 8; break; }
   w1 = (word32) (dtab[ii] & WORDMASK_ULL);
   if ((w1 & 0xffffff00) == 0L) continue;
   if ((w1 & 0xffff0000) == 0L) { minsiz = 2; continue; }
  }
 if (minsiz > 2)
  {
   dtab2 = (word64 *) __my_malloc(nbytes);
   memcpy(dtab2, dtab, nbytes);
   gp->g_du.dis16v = dtab2;
   gp->g_delrep = DT_IS16V;
   goto done;
  }

 if (minsiz == 1)
  {
   btab = (byte *) __my_malloc(16*__inst_mod->flatinum);
   for (ii = 0; ii < 16*__inst_mod->flatinum; ii++)
    btab[ii] = (byte) dtab[ii];
   gp->g_du.dis16v1 = btab;
   gp->g_delrep = DT_IS16V1;
  }
 else 
  {
   htab = (hword *) __my_malloc(16*2*__inst_mod->flatinum);
   for (ii = 0; ii < 16*__inst_mod->flatinum; ii++)
    htab[ii] = (hword) dtab[ii];
   gp->g_du.dis16v2 = htab;
   gp->g_delrep = DT_IS16V2;
  }
done: 
 __my_free((char *) dtab, nbytes);
}

/*
 * fill one column of the flatinum deep delay table
 */
static void fill_1col_isdel(word64 *dtab, int32 coli, struct expr_t *xp,
 int32 stride)
{
 register int32 ii;
 word32 *wp;
 word64 ticksval, delval;
 double d1, *dp;

 switch ((byte) xp->optyp) {
  case REALNUM:
  case NUMBER:
   nonis_to_delval(&ticksval, xp);
   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    dtab[stride*ii + coli] = ticksval;
   break;
  case ISREALNUM:
   dp = (double *) &(__contab[xp->ru.xvi]);
   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    {
     d1 = dp[ii];
     if (d1 < 0.0)
      {
       ticksval = 0ULL;
       __nd_neg_del_warn = TRUE;
      }
     else real_to_ticks(&ticksval, d1, __inst_mod);
     dtab[stride*ii + coli] = ticksval;
    }
   break;
  case ISNUMBER:
   if (xp->szu.xclen > WBITS)
    {
     wp = &(__contab[xp->ru.xvi]);
     for (ii = 0; ii < __inst_mod->flatinum; ii++)
      {
       /* SJM 08/17/04 - now for ver 2001 any width can be signed */
       if (__is_lnegative(wp, xp->szu.xclen))
        {
         __nd_neg_del_warn = TRUE;
         ticksval = 0ULL;
        }
       else
        {
         ticksval = ((word64) wp[2*ii]) | (((word64) wp[2*ii + 1]) << 32);
         if (!__inst_mod->mno_unitcnv)
	  {
           delval = ticksval;
           cnv_num64to_ticks_(ticksval, delval, __inst_mod);
          }
        }
       dtab[stride*ii + coli] = ticksval;
      }
    }
   else
    {
     wp = &(__contab[xp->ru.xvi]);
     for (ii = 0; ii < __inst_mod->flatinum; ii++)
      {
       ticksval = (word64) wp[2*ii];

       /* SJM 08/17/04 - now for ver 2001 any width can be signed */
       if (xp->has_sign && (wp[0] & (1 << (xp->szu.xclen - 1))) != 0)
        {
         ticksval = 0ULL;
         __nd_neg_del_warn = TRUE;
        }
       else if (!__inst_mod->mno_unitcnv)
	{
         delval = ticksval;
         cnv_num64to_ticks_(ticksval, delval, __inst_mod);
        }
       dtab[stride*ii + coli] = ticksval;
      }
    }
   break;
   default: __case_terr(__FILE__, __LINE__);
  }
}

/*
 * DELAY REPRESENTATION CHANGE ROUTINES 
 */

/*
 * change one instance delay form
 *
 * this merges the 1 inst into IS form
 * know new delay is not IS form since for 1 instance
 * this does not need itree loc. on itree stack uses passed value
 */
extern void __chg_1inst_del(struct gate_t *mgp, struct itree_t *mastitp,
 struct gate_t *ngp)
{
 switch ((byte) ngp->g_delrep) {
  case DT_1V: chg1vform_1instdel(mgp, mastitp, ngp); break;
  case DT_4V: chg4vform_1instdel(mgp, mastitp, ngp); break;
  case DT_16V: chg16vform_1instdel(mgp, mastitp, ngp); break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * change one instance delay form when new is DT_1V form
 * know mgp always is form after here
 * can be called from any form either prim or path delay
 */
static void chg1vform_1instdel(struct gate_t *mgp, struct itree_t *mastitp,
 struct gate_t *ngp)
{
 register int32 i;
 int32 numinsts, tmp, mwid;
 word64 ntim[16], mtim[16];

 numinsts = mastitp->itip->imsym->el.emdp->flatinum;
 ntim[0] = ngp->g_du.d1v[0];
 switch ((byte) mgp->g_delrep) {
  case DT_1V:
   /* if values same, nothing to do */
   mtim[0] = mgp->g_du.d1v[0];
   if (mtim[0] == ntim[0]) break;

   __my_free((char *) mgp->g_du.d1v, sizeof(word64));
   /* attempt to store in packed form */
   mwid = get_ispack(ntim[0]);
   tmp = get_ispack(mtim[0]);
   if (tmp > mwid) mwid = tmp;
   create_disv(mgp, numinsts, 1, mtim, mwid);
   /* set the new value */
   set_1is1val(mgp, mastitp->itinum, 1, ntim, mwid);
   break;
  case DT_4V:
   for (i = 0; i < 4; i++) mtim[i] = mgp->g_du.d4v[i];
   for (i = 0; i < 4; i++) { if (mtim[i] != ntim[0]) goto nd_is4v; }
   break;
nd_is4v:  
   __my_free((char *) mgp->g_du.d4v, 4*sizeof(word64));
   mwid = get_ispack(ntim[0]);
   for (i = 0; i < 4; i++) 
    { tmp = get_ispack(mtim[i]); if (tmp > mwid) mwid = tmp; }
   create_disv(mgp, numinsts, 4, mtim, mwid);
   set_1is1val(mgp, mastitp->itinum, 4, ntim, mwid);
   break;
  case DT_16V: 
   for (i = 0; i < 16; i++) mtim[i] = mgp->g_du.d16v[i];
   for (i = 1; i < 16; i++)
    {
     if (i == 5 || i == 10 || i == 15) continue;  
     if (mtim[i] != ntim[0]) goto nd_is16v;
    }
   break;
nd_is16v:  
   __my_free((char *) mgp->g_du.d16v, 16*sizeof(word64));
   mwid = get_ispack(ntim[0]);
   for (i = 1; i < 16; i++) 
    {
     if (i == 5 || i == 10 || i == 15) continue;
     tmp = get_ispack(mtim[i]);
     if (tmp > mwid) mwid = tmp;
    }
   create_disv(mgp, numinsts, 16, mtim, mwid);
   set_1is1val(mgp, mastitp->itinum, 16, ntim, mwid);
   break;
  case DT_IS1V: 
   mgp->g_du.dis1v[mastitp->itinum] = ntim[0];
   /* slight possibility that could now narrow but rare */
   break;
  case DT_IS1V1: 
   mwid = get_ispack(ntim[0]); 
   if (mwid == 1)
    { mgp->g_du.dis1v1[mastitp->itinum] = (byte) ntim[0]; break; }
   if (mwid == 2) 
    {
     unpack_isv1_to_isv2(mgp, numinsts, 1);
     mgp->g_du.dis1v2[mastitp->itinum] = (hword) ntim[0];
     break;
    }
   unpack_isv1_to_isv(mgp, numinsts, 1);
   mgp->g_du.dis1v[mastitp->itinum] = ntim[0];
   break;
  case DT_IS1V2:
   mwid = get_ispack(ntim[0]); 
   if (mwid < 8) mgp->g_du.dis1v2[mastitp->itinum] = (hword) ntim[0];
   else
    {
     unpack_isv2_to_isv(mgp, numinsts, 1);
     mgp->g_du.dis1v[mastitp->itinum] = ntim[0];
    }
   break;
  case DT_IS4V:
   for (i = 0; i < 4; i++) mgp->g_du.dis4v[4*mastitp->itinum + i] = ntim[0];
   /* slight possibility that could now narrow but rare */
   break;
  case DT_IS4V1: 
   mwid = get_ispack(ntim[0]); 
   if (mwid == 1)
    {
     for (i = 0; i < 4; i++)
      mgp->g_du.dis4v1[4*mastitp->itinum + i] = (byte) ntim[0];
     break;
    }
   if (mwid == 2) 
    {
     unpack_isv1_to_isv2(mgp, numinsts, 4);
     for (i = 0; i < 4; i++)
      mgp->g_du.dis4v2[4*mastitp->itinum + i] = (hword) ntim[0];
     break;
    }
   unpack_isv1_to_isv(mgp, numinsts, 4);
   for (i = 0; i < 4; i++)
    mgp->g_du.dis4v[4*mastitp->itinum + i] = ntim[0];
   break;
  case DT_IS4V2:
   mwid = get_ispack(ntim[0]); 
   if (mwid < 8)
    {
     for (i = 0; i < 4; i++)
      mgp->g_du.dis4v2[4*mastitp->itinum + i] = (hword) ntim[0];
    }
   else
    {
     unpack_isv2_to_isv(mgp, numinsts, 4);
     for (i = 0; i < 4; i++)
      mgp->g_du.dis4v[4*mastitp->itinum + i] = ntim[0];
    }
   break;
  case DT_IS16V:
   for (i = 0; i < 16; i++)
    mgp->g_du.dis16v[16*mastitp->itinum + i] = ntim[0];
   zero_unused_16v(&(mgp->g_du.dis16v[16*mastitp->itinum]));
   /* slight possibility that could now narrow but rare */
   break;
  case DT_IS16V1: 
   mwid = get_ispack(ntim[0]); 
   if (mwid == 1)
    {
     for (i = 0; i < 16; i++)
      {
       if (i == 0 || i == 5 || i == 10 || i == 15)  
        { mgp->g_du.dis16v1[16*mastitp->itinum + i] = 0; continue; } 
       mgp->g_du.dis16v1[16*mastitp->itinum + i] = (byte) ntim[0];
      }
     break;
    }
   if (mwid == 2) 
    {
     unpack_isv1_to_isv2(mgp, numinsts, 16);
     for (i = 0; i < 16; i++)
      {
       if (i == 0 || i == 5 || i == 10 || i == 15)  
        { mgp->g_du.dis16v2[16*mastitp->itinum + i] = 0; continue; } 
       mgp->g_du.dis16v2[16*mastitp->itinum + i] = (hword) ntim[0];
      }
     break;
    }
   unpack_isv1_to_isv(mgp, numinsts, 16);
   for (i = 0; i < 16; i++)
    mgp->g_du.dis16v[16*mastitp->itinum + i] = ntim[0];
   zero_unused_16v(&(mgp->g_du.dis16v[16*mastitp->itinum]));
   break;
  case DT_IS16V2:
   mwid = get_ispack(ntim[0]); 
   if (mwid < 8)
    {
     for (i = 0; i < 16; i++)
      {
       if (i == 0 || i == 5 || i == 10 || i == 15)  
        { mgp->g_du.dis16v2[16*mastitp->itinum + i] = 0; continue; } 
       mgp->g_du.dis16v2[16*mastitp->itinum + i] = (hword) ntim[0];
      }
    }
   else
    {
     unpack_isv2_to_isv(mgp, numinsts, 16);
     for (i = 0; i < 16; i++)
      mgp->g_du.dis16v[16*mastitp->itinum + i] = ntim[0];
     zero_unused_16v(&(mgp->g_du.dis16v[16*mastitp->itinum]));
    }
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * routine to set 1 instance of IS form value
 * here know only 0th element of ntim used since previous known 1 value
 */
static void set_1is1val(struct gate_t *gp, int32 iti, int32 nvals, word64 *ntim, 
 int32 mwid)
{
 register int32 i, ivalnum;

 /* set the new value */
 ivalnum = nvals*iti;
 if (mwid == 1)
  {
   for (i = 0; i < nvals; i++)
    {
     if (nvals == 16)
      {
       if (i == 0 || i == 5 || i == 10 || i == 15) 
        gp->g_du.dis16v1[ivalnum + i] = 0;
       else gp->g_du.dis16v1[ivalnum + i] = (byte) ntim[0];
      }
     else gp->g_du.dis4v1[ivalnum + i] = (byte) ntim[0];
    }
  }
 else if (mwid == 8)
  {
   for (i = 0; i < nvals; i++) gp->g_du.dis16v[ivalnum + i] = ntim[0];
   if (nvals == 16) zero_unused_16v(&(gp->g_du.dis16v[ivalnum]));
  }
 else
  {
   for (i = 0; i < nvals; i++)
    {
     if (nvals == 16)
      {
       if (i == 0 || i == 5 || i == 10 || i == 15)
        gp->g_du.dis16v2[ivalnum + i] = 0;
       else gp->g_du.dis16v2[ivalnum + i] = (hword) ntim[0];
      }
     /* since all ptrs to array of hwords use 4v2 form, could use any */
     else gp->g_du.dis4v2[ivalnum + i] = (hword) ntim[0];
    }
  }
}

/*
 * change one instance delay form when new is DT_4V form
 * know old can only be 1v or 4v form since 16v impossible for prim delay
 */
static void chg4vform_1instdel(struct gate_t *mgp, struct itree_t *mastitp,
 struct gate_t *ngp)
{
 register int32 i;
 int32 numinsts, tmp, mwid;
 word64 ntim[16], mtim[16];

 numinsts = mastitp->itip->imsym->el.emdp->flatinum;
 for (i = 0; i < 4; i++) ntim[i] = ngp->g_du.d4v[i];
 switch ((byte) mgp->g_delrep) {
  case DT_1V:
   /* new is 4 value, master is 1 value */
   /* since new is 4v always need at least 4v */
   mtim[0] = mgp->g_du.d1v[0];
   /* if all 4 new same as same as 1v, leave 1 v */ 
   for (i = 0; i < 4; i++) { if (mtim[0] != ntim[i]) goto nd_is1v4v; }
   break;
nd_is1v4v:
   /* duplicate 1 to all of master times */
   mtim[1] = mtim[2] = mtim[3] = mtim[0];
   __my_free((char *) mgp->g_du.d1v, sizeof(word64));
   mwid = get_ispack(mtim[0]);
   for (i = 0; i < 4; i++)
    { tmp = get_ispack(ntim[i]); if (tmp > mwid) mwid = tmp; }
   create_disv(mgp, numinsts, 4, mtim, mwid);
   set_1is416val(mgp, mastitp->itinum, 4, ntim, mwid);
   break;
  case DT_4V:
   /* master is 4 and new is 4 */
   for (i = 0; i < 4; i++) mtim[i] = mgp->g_du.d4v[i];
   for (i = 0; i < 4; i++) { if (mtim[i] != ntim[i]) goto nd_is4v; }
   break;
nd_is4v:  
   __my_free((char *) mgp->g_du.d1v, 4*sizeof(word64));
   mwid = get_ispack(ntim[0]);
   for (i = 1; i < 4; i++) 
    { tmp = get_ispack(ntim[i]); if (tmp > mwid) mwid = tmp; }
   for (i = 0; i < 4; i++) 
    { tmp = get_ispack(mtim[i]); if (tmp > mwid) mwid = tmp; }
   /* notice only difference from 1v here is mtim has 4 different vals */
   create_disv(mgp, numinsts, 4, mtim, mwid);
   set_1is416val(mgp, mastitp->itinum, 4, ntim, mwid);
   break;
  case DT_IS1V: 
   /* new is 4 value, master is 1 IS form */
   /* convert 1 IS to 4 IS */
   cnv_1is_to_4is(mgp, numinsts, 8, 8); 
   /* SJM 08/14/01 - was wrongly filling is1v case - other 3 not filled */
   for (i = 0; i < 4; i++) 
    mgp->g_du.dis1v[4*mastitp->itinum + i] = ntim[i];
   /* slight possibility that could now narrow but rare */
   break;
  case DT_IS1V1: 
   /* master is 1 byte 1 value IS and new time is 4v */
   mwid = get_ispack(ntim[0]); 
   for (i = 1; i < 4; i++) 
    { tmp = get_ispack(ntim[i]); if (tmp > mwid) mwid = tmp; }
   if (mwid == 1)
    {
     cnv_1is_to_4is(mgp, numinsts, 1, 1);  
     for (i = 0; i < 4; i++) 
      mgp->g_du.dis4v1[4*mastitp->itinum + i] = (byte) ntim[i];
     break;
    }
   if (mwid == 2) 
    {
     cnv_1is_to_4is(mgp, numinsts, 1, 2);  
     for (i = 0; i < 4; i++) 
      mgp->g_du.dis4v2[4*mastitp->itinum + i] = (hword) ntim[i];
     break;
    }
   cnv_1is_to_4is(mgp, numinsts, 1, 8);  
   for (i = 0; i < 4; i++) 
    mgp->g_du.dis4v[4*mastitp->itinum + i] = ntim[i];
   break;
  case DT_IS1V2:
   mwid = get_ispack(ntim[0]); 
   for (i = 1; i < 4; i++) 
    { tmp = get_ispack(ntim[i]); if (tmp > mwid) mwid = tmp; }
   if (mwid < 8)
    {
     cnv_1is_to_4is(mgp, numinsts, 2, 2);  
     for (i = 0; i < 4; i++) 
      mgp->g_du.dis4v2[4*mastitp->itinum + i] = (hword) ntim[i];
     break;
    }
   cnv_1is_to_4is(mgp, numinsts, 2, 8);  
   for (i = 0; i < 4; i++) 
    mgp->g_du.dis4v2[4*mastitp->itinum + i] = (hword) ntim[i];
   break;
  case DT_IS4V:
   /*  new 4v and master 4v is */
   for (i = 0; i < 4; i++) mgp->g_du.dis4v[4*mastitp->itinum + i] = ntim[i];
   /* slight possibility that could now narrow but rare */
   break;
  case DT_IS4V1: 
   mwid = get_ispack(ntim[0]); 
   for (i = 1; i < 4; i++) 
    { tmp = get_ispack(ntim[i]); if (tmp > mwid) mwid = tmp; }
   if (mwid == 1)
    {
     for (i = 0; i < 4; i++)
      mgp->g_du.dis4v1[4*mastitp->itinum + i] = (byte) ntim[i];
     break;
    }
   if (mwid == 2) 
    {
     unpack_isv1_to_isv2(mgp, numinsts, 4);
     for (i = 0; i < 4; i++)
      mgp->g_du.dis4v2[4*mastitp->itinum + i] = (hword) ntim[i];
     break;
    }
   unpack_isv1_to_isv(mgp, numinsts, 4);
   for (i = 0; i < 4; i++)
    mgp->g_du.dis4v[4*mastitp->itinum + i] = ntim[i];
   break;
  case DT_IS4V2:
   mwid = get_ispack(ntim[0]); 
   for (i = 1; i < 4; i++) 
    { tmp = get_ispack(ntim[i]); if (tmp > mwid) mwid = tmp; }
   if (mwid < 8)
    {
     for (i = 0; i < 4; i++)
      mgp->g_du.dis4v2[4*mastitp->itinum + i] = (hword) ntim[i];
    }
   else
    {
     unpack_isv2_to_isv(mgp, numinsts, 4);
     for (i = 0; i < 4; i++)
      mgp->g_du.dis4v[4*mastitp->itinum + i] = ntim[i];
    }
   break;
  /* all 16 v forms here are impossible */
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * routine to set 1 instance of IS form value where ntim has right
 * width and nvals assigned for 1 changed instance
 */
static void set_1is416val(struct gate_t *gp, int32 iti, int32 nvals, word64 *ntim, 
 int32 mwid)
{
 register int32 i, ivalnum;

 /* set the new value */
 ivalnum = nvals*iti;
 if (mwid == 1)
  {
   for (i = 0; i < nvals; i++)
    {
     if (nvals == 16)
      {
       if (i == 0 || i == 5 || i == 10 || i == 15)
        gp->g_du.dis16v1[ivalnum + i] = 0;
       else gp->g_du.dis16v1[ivalnum + i] = (byte) ntim[i];
      }
     else gp->g_du.dis4v1[ivalnum + i] = (byte) ntim[i];
    }
  }
 else if (mwid == 8)
  {
   for (i = 0; i < nvals; i++)
    gp->g_du.dis16v[ivalnum + i] = ntim[i];
   if (nvals == 16) zero_unused_16v(&(gp->g_du.dis16v[ivalnum]));
  }
 else
  {
   for (i = 0; i < nvals; i++)
    {
     if (nvals == 16)
      {
       if (i == 0 || i == 5 || i == 10 || i == 15)
        gp->g_du.dis16v2[ivalnum + i] = 0;
       else gp->g_du.dis16v2[ivalnum + i] = (hword) ntim[i];
      }
     else gp->g_du.dis4v2[ivalnum + i] = (hword) ntim[i];
    }
  }
}

/*
 * change one instance of IS delay form when new is DT_16V form
 * here old 4v form impossible since must be path delay
 */
static void chg16vform_1instdel(struct gate_t *mgp, struct itree_t *mastitp,
 struct gate_t *ngp)
{
 register int32 i;
 int32 numinsts, tmp, mwid;
 word64 tmptim, ntim[16], mtim[16];

 numinsts = mastitp->itip->imsym->el.emdp->flatinum;
 for (i = 0; i < 16; i++) ntim[i] = ngp->g_du.d16v[i];
 switch ((byte) mgp->g_delrep) {
  case DT_1V:
   /* new is 16 value, master is 1 value */
   mtim[0] = mgp->g_du.d1v[0];
   for (i = 1; i < 16; i++)
    {
     if (i == 5 || i == 10 || i == 15) continue;
     if (mtim[0] != ntim[i]) goto nd_is1v16v;
    }
   break;
nd_is1v16v:
   /* duplicate 1 to all of master times */
   tmptim = mtim[0];
   for (i = 1; i < 16; i++) mtim[i] = tmptim; 
   zero_unused_16v(mtim);
   __my_free((char *) mgp->g_du.d1v, sizeof(word64));
   mwid = get_ispack(mtim[0]);
   for (i = 1; i < 16; i++)
    {
     if (i == 5 || i == 10 || i == 15) continue;
     tmp = get_ispack(ntim[i]);
     if (tmp > mwid) mwid = tmp;
    }
   create_disv(mgp, numinsts, 16, mtim, mwid);
   set_1is416val(mgp, mastitp->itinum, 16, ntim, mwid);
   break;
  case DT_16V: 
   /* new 16 values, master 16 */
   for (i = 0; i < 16; i++) mtim[i] = mgp->g_du.d16v[i];
   for (i = 0; i < 16; i++) { if (mtim[i] != ntim[i]) goto nd_is16v; }
   break;
nd_is16v:  
   /* must look at 12 master's and 4 news */
   mwid = get_ispack(ntim[1]);
   for (i = 2; i < 16; i++) 
    { 
     if (i == 5 || i == 10 || i == 15) continue;
     tmp = get_ispack(ntim[i]); if (tmp > mwid) mwid = tmp;
    }
   for (i = 1; i < 16; i++) 
    {
     if (i == 5 || i == 10 || i == 15) continue;
     tmp = get_ispack(mtim[i]); if (tmp > mwid) mwid = tmp;
    }
   /* set the new value */
   create_disv(mgp, numinsts, 16, mtim, mwid);
   set_1is416val(mgp, mastitp->itinum, 16, ntim, mwid);
   break;
  case DT_IS1V: 
   /* new is 16 value, master is 1 IS form */
   /* convert 1 is to 16 IS and set to dummy value */
   cnv_1is_to_16is(mgp, numinsts, 8, 8);  
   /* SJM 08/14/01 - was wrongly filling is1v case - other 15 not filled */
   for (i = 0; i < 16; i++) 
    mgp->g_du.dis16v[16*mastitp->itinum + i] = ntim[i];
   /* slight possibility that could now narrow but rare */
   break;
  case DT_IS1V1: 
   /* master is 1 byte 1 value IS and new time is 16v */
   mwid = get_ispack(ntim[1]); 
   for (i = 2; i < 16; i++) 
    {
     if (i == 5 || i == 10 || i == 15) continue;
     tmp = get_ispack(ntim[i]);
     if (tmp > mwid) mwid = tmp;
    }
   if (mwid == 1)
    {
     cnv_1is_to_16is(mgp, numinsts, 1, 1);  
     for (i = 0; i < 16; i++) 
      mgp->g_du.dis16v1[16*mastitp->itinum + i] = (byte) ntim[i];
     break;
    }
   if (mwid == 2) 
    {
     cnv_1is_to_16is(mgp, numinsts, 1, 2);  
     for (i = 0; i < 16; i++) 
      mgp->g_du.dis16v2[16*mastitp->itinum + i] = (hword) ntim[i];
     break;
    }
   cnv_1is_to_16is(mgp, numinsts, 1, 8);  
   for (i = 0; i < 16; i++) 
    mgp->g_du.dis16v[16*mastitp->itinum + i] = ntim[i];
   break;
  case DT_IS1V2:
   mwid = get_ispack(ntim[1]); 
   for (i = 2; i < 16; i++) 
    {
     if (i == 5 || i == 10 || i == 15) continue;
     tmp = get_ispack(ntim[i]); if (tmp > mwid) mwid = tmp;
    }
   if (mwid < 8)
    {
     cnv_1is_to_16is(mgp, numinsts, 2, 2);  
     for (i = 0; i < 16; i++) 
      mgp->g_du.dis16v2[16*mastitp->itinum + i] = (hword) ntim[i];
     break;
    }
   cnv_1is_to_16is(mgp, numinsts, 2, 8);  
   for (i = 0; i < 16; i++) 
    mgp->g_du.dis16v[16*mastitp->itinum + i] = ntim[i];
   break;
  case DT_IS16V:
   /*  new 16v and master 16v is */
   for (i = 0; i < 16; i++)
    mgp->g_du.dis16v[16*mastitp->itinum + i] = ntim[i];
   /* slight possibility that could now narrow but rare */
   break;
  case DT_IS16V1: 
   mwid = get_ispack(ntim[1]); 
   for (i = 2; i < 16; i++) 
    {
     if (i == 5 || i == 10 || i == 15) continue;
     tmp = get_ispack(ntim[i]); if (tmp > mwid) mwid = tmp;
    }
   if (mwid == 1)
    {
     for (i = 0; i < 16; i++)
      mgp->g_du.dis16v1[16*mastitp->itinum + i] = (byte) ntim[i];
     break;
    }
   if (mwid == 2) 
    {
     unpack_isv1_to_isv2(mgp, numinsts, 16);
     for (i = 0; i < 16; i++)
      mgp->g_du.dis16v2[16*mastitp->itinum + i] = (hword) ntim[i];
     break;
    }
   unpack_isv1_to_isv(mgp, numinsts, 16);
   for (i = 0; i < 16; i++)
    mgp->g_du.dis16v[16*mastitp->itinum + i] = ntim[i];
   break;
  case DT_IS16V2:
   mwid = get_ispack(ntim[1]); 
   for (i = 2; i < 16; i++) 
    {
     if (i == 5 || i == 15) continue;
     tmp = get_ispack(ntim[i]);
     if (tmp > mwid) mwid = tmp;
    }
   if (mwid < 8)
    {
     for (i = 0; i < 16; i++)
      mgp->g_du.dis16v2[16*mastitp->itinum + i] = (hword) ntim[i];
    }
   else
    {
     unpack_isv2_to_isv(mgp, numinsts, 16);
     for (i = 0; i < 16; i++)
      mgp->g_du.dis16v[16*mastitp->itinum + i] = ntim[i];
    }
   break;
  /* 4v forms illegal here */
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * convert 1 IS packed opck size to 4 IS form packed npck
 */
static void cnv_1is_to_4is(struct gate_t *gp, int32 numinsts, int32 opck, int32 npck)
{
 register int32 ii, i;
 union del_u sav_du;
 word64 ntim[16];

 for (i = 0; i < 4; i++) ntim[i] = 0ULL; 
 sav_du = gp->g_du;
 /* build nvals value packed acording to new value */
 create_disv(gp, numinsts, 4, ntim, npck);

 for (ii = 0; ii < numinsts; ii++)
  {
   for (i = 0; i < 4; i++)
    {
     if (opck == 1) ntim[i] = (word64) sav_du.dis1v1[ii];
     else if (opck == 2) ntim[i] = (word64) sav_du.dis1v2[ii];
     else ntim[i] = sav_du.dis1v[ii];  
    }
   if (npck == 1)
    {
     for (i = 0; i < 4; i++)
      gp->g_du.dis4v1[4*ii + i] = (byte) ntim[i];
    }
   else if (npck == 2)
    {
     for (i = 0; i < 4; i++)
      gp->g_du.dis4v2[4*ii + i] = (hword) ntim[i];
    }
   else
    { for (i = 0; i < 4; i++) gp->g_du.dis4v[4*ii + i] = ntim[i]; }
  }
 if (opck == 1) __my_free((char *) sav_du.dis1v1, numinsts);
 else if (opck == 2) __my_free((char *) sav_du.dis1v2, 2*numinsts);
 else __my_free((char *) sav_du.dis1v, sizeof(word64)*numinsts);
}

/*
 * convert 1 IS packed opck size to 6 IS form packed npck
 * separate routine becase 0, 5, 10 must be 0
 */
static void cnv_1is_to_16is(struct gate_t *gp, int32 numinsts, int32 opck, int32 npck)
{
 register int32 ii, i;
 union del_u sav_du;
 word64 ntim[16];

 for (i = 0; i < 4; i++) ntim[i] = 0ULL;
 sav_du = gp->g_du;
 /* build nvals value packed according to npck */
 create_disv(gp, numinsts, 16, ntim, npck);

 for (ii = 0; ii < numinsts; ii++)
  {
   for (i = 1; i < 16; i++)
    {
     if (i == 5 || i == 10) continue;

     if (opck == 1) ntim[i] = (word64) sav_du.dis1v1[ii];
     else if (opck == 2) ntim[i] = (word64) sav_du.dis1v2[ii];
     else ntim[i] = sav_du.dis1v[ii];  
    }
   if (npck == 1)
    {
     for (i = 0; i < 16; i++)
      gp->g_du.dis16v1[16*ii + i] = (byte) ntim[i];
    }
   else if (npck == 2)
    {
     for (i = 0; i < 16; i++)
      gp->g_du.dis16v2[16*ii + i] = (hword) ntim[i];
    }
   else
    { for (i = 0; i < 16; i++) gp->g_du.dis16v[16*ii + i] = ntim[i]; }
  }
 if (opck == 1) __my_free((char *) sav_du.dis1v1, numinsts);
 else if (opck == 2) __my_free((char *) sav_du.dis1v2, 2*numinsts);
 else __my_free((char *) sav_du.dis1v, sizeof(word64)*numinsts);
}

/*
 * change from dv? to dis?v
 * nvals of tim must be filled
 * this always uses 16v form since just pointer to alloced array
 */
static void create_disv(struct gate_t *gp, int32 numinsts, int32 nvals,
 word64 *tim, int32 wid)
{
 register int32 ii, i;

 if (wid == 1)
  {
   gp->g_du.dis16v1 = (byte *) __my_malloc(nvals*numinsts);
   for (ii = 0; ii < numinsts; ii++)
    {
     for (i = 0; i < nvals; i++)
      gp->g_du.dis16v1[nvals*ii + i] = (byte) tim[i];
    }
   gp->g_delrep = (nvals == 1) ? DT_IS1V1
    : ((nvals == 4) ? DT_IS4V1 : DT_IS16V1);
   return;
  }
 if (wid == 8)
  {
   gp->g_du.dis16v = (word64 *) __my_malloc(nvals*numinsts*sizeof(word64));
   for (ii = 0; ii < numinsts; ii++)
    { for (i = 0; i < nvals; i++) gp->g_du.dis16v[nvals*ii + i] = tim[i]; }
   gp->g_delrep = (nvals == 1) ? DT_IS1V : ((nvals == 4) ? DT_IS4V : DT_IS16V);
   return;
  }
 /* final rare fits in 2 bytes case */
 gp->g_du.dis16v2 = (hword *) __my_malloc(2*nvals*numinsts);
 for (ii = 0; ii < numinsts; ii++)
  {
   for (i = 0; i < nvals; i++)
    gp->g_du.dis16v2[nvals*ii + i] = (hword) tim[i];
  }
 gp->g_delrep = (nvals == 1) ? DT_IS1V2
  : ((nvals == 4) ? DT_IS4V2 : DT_IS16V2);
}

/*
 * get width
 */
static int32 get_ispack(word64 tim)
{
 register word32 w1;

 w1 = (word32) (tim & WORDMASK_ULL);
 if (tim > WORDMASK_ULL || (w1 & 0xffff0000) != 0) return(8);
 if ((w1 & 0xffffff00) != 0) return(2);
 return(1);
}

/*
 * unpack from isv1 to isv2, nvals determines if 1, 4, or 16 value form 
 * caller must insure unpack will fit 
 * uses 16v form since just pointer to array 
 */
static void unpack_isv1_to_isv2(struct gate_t *gp, int32 numinsts, int32 nvals)
{
 register int32 ii, i;
 union del_u sav_du;

 sav_du = gp->g_du;
 gp->g_du.dis16v2 = (hword *) __my_malloc(2*numinsts*nvals);
 for (ii = 0; ii < numinsts; ii++)
  {
   for (i = 0; i < nvals; i++) 
    gp->g_du.dis16v2[nvals*ii + i] = (hword) sav_du.dis16v1[nvals*ii + i];
  }
 __my_free((char *) sav_du.dis16v1, numinsts*nvals);
 gp->g_delrep = (nvals == 1) ? DT_IS1V2
  : ((nvals == 4) ? DT_IS4V2 : DT_IS16V2);
}

/*
 * unpack from isv1 to isv, nvals determines if 1, 4, or 16 value form 
 * caller must insure unpack will fit 
 */
static void unpack_isv1_to_isv(struct gate_t *gp, int32 numinsts, int32 nvals)
{
 register int32 ii, i;
 union del_u sav_du;

 sav_du = gp->g_du;
 gp->g_du.dis16v = (word64 *) __my_malloc(sizeof(word64)*numinsts*nvals);
 for (ii = 0; ii < numinsts; ii++)
  {
   for (i = 0; i < nvals; i++) 
    {
     gp->g_du.dis16v[nvals*ii + i] = (word64) sav_du.dis16v1[nvals*ii + i];
    }
  }
 __my_free((char *) sav_du.dis16v1, numinsts*nvals);
 gp->g_delrep = (nvals == 1) ? DT_IS1V : ((nvals == 4) ? DT_IS4V : DT_IS16V);
}

/*
 * unpack from isv2 to isv, nvals determines if 1, 4, or 16 value form 
 * caller must insure unpack will fit 
 * uses 16v form since just ptr to array
 */
static void unpack_isv2_to_isv(struct gate_t *gp, int32 numinsts, int32 nvals)
{
 register int32 ii, i;
 union del_u sav_du;

 sav_du = gp->g_du;
 gp->g_du.dis16v = (word64 *) __my_malloc(sizeof(word64)*numinsts*nvals);
 for (ii = 0; ii < numinsts; ii++)
  {
   for (i = 0; i < nvals; i++) 
    {
     gp->g_du.dis16v[nvals*ii + i] = (word64) sav_du.dis16v2[nvals*ii + i];
    }
  }
 __my_free((char *) sav_du.dis16v2, 2*numinsts*nvals);
 gp->g_delrep = (nvals == 1) ? DT_IS1V : ((nvals == 4) ? DT_IS4V : DT_IS16V);
}

/*
 * zero unused 16v values
 */
static void zero_unused_16v(word64 *tim)
{
 tim[0] = 0ULL;
 tim[5] = 0ULL;
 tim[10] = 0ULL;
}

/*
 * build a delay using current itree loc.
 *
 * can not be called until delays prepared
 * uses current itree loc for instance if delay per inst. 
 */
extern char *__bld_delay_str(char *s, union del_u du, word32 drep)
{
 register int32 i;
 int32 ndels, sav_nd_timstr_suf;
 word64 tim[12], timval;
 struct mod_t *mdp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 sav_nd_timstr_suf = __nd_timstr_suf;
 __nd_timstr_suf = FALSE;
 __extract_delval(tim, &ndels, du, drep);
 mdp =__inst_mod;
 if (ndels > 0 && !mdp->mno_unitcnv)
  {
   for (i = 0; i < ndels; i++)
    { __cnv_ticks_tonum64(&timval, tim[i], mdp); tim[i] = timval; }
  }
 switch ((byte) drep) {
  case DT_1V: case DT_IS1V: case DT_IS1V1: case DT_IS1V2:
   sprintf(s, "%s", __to_timstr(s1, &(tim[0])));
   break;
  case DT_4V: case DT_IS4V: case DT_IS4V1: case DT_IS4V2:
   sprintf(s, "%s, %s, %s", __to_timstr(s1, &(tim[0])),
    __to_timstr(s2, &(tim[1])), __to_timstr(s3, &(tim[2])));
   break;
  case DT_16V: case DT_IS16V: case DT_IS16V1: case DT_IS16V2:
   for (i = 0; i < ndels; i++)
    {
     if (i == 0) { sprintf(s, "%s", __to_timstr(s1, &(tim[0]))); continue; } 
     sprintf(s1, ", %s", __to_timstr(s2, &(tim[i])));
     strcat(s, s1);
    } 
   break;
  case DT_1X:
   sprintf(s, "**EXPR: %s", __to_timstr(s1, &(tim[0]))); break;
  case DT_4X:
   if (ndels == 3)
    {
     sprintf(s, "**EXPR: %s, %s, %s", __to_timstr(s1, &(tim[0])),
     __to_timstr(s2, &(tim[1])), __to_timstr(s3, &(tim[2])));
    }
   else
    {
     sprintf(s, "**EXPR: %s, %s", __to_timstr(s1, &(tim[0])),
     __to_timstr(s2, &(tim[1])));
    }
   break;
  case DT_CMPLST: strcpy(s, "**WRONG SOURCE FORM LIST**"); break;
  case DT_PTHDST: strcpy(s, "**PATH DESTIONATION PLACE HOLDER**"); break;
  case DT_NONE: strcpy(s, "**NO DELAY**"); break; 
  default: __case_terr(__FILE__, __LINE__);
 }
 __nd_timstr_suf = sav_nd_timstr_suf;
 return(s);
}

/*
 * extract delays and fill passed tim array
 *
 * caller must push itree loc.
 * errors odd delays, and scaling processed by caller above
 * only for delays during sim
 * can return 1,3,6, or 12 delays
 * (or 0 for DT NONE - used for SDF or vpi_ annotate adding new)
 *
 * SJM 02/03/00 - cast to word64 word32 because packed form always unsigned
 */
extern void __extract_delval(word64 *tim, int32 *ndels, union del_u du,
 word32 drep)
{
 struct xstk_t *xsp;

 switch ((byte) drep) {
  case DT_NONE: *ndels = 0; break;
  case DT_1V: tim[0] = du.d1v[0]; *ndels = 1; break;
  case DT_IS1V:
   tim[0] = du.dis1v[__inum];
   *ndels = 1;
   break;
  case DT_IS1V1:
   tim[0] = (word64) du.dis1v1[__inum];
   *ndels = 1;
   break;
  case DT_IS1V2:
   tim[0] = (word64) du.dis1v2[__inum];
   *ndels = 1;
   break;
  case DT_4V:
   /* (rise, fall, toz) */
   tim[0] = du.d4v[1]; tim[1] = du.d4v[0]; tim[2] = du.d4v[2];
   *ndels = 3;
   break;
  case DT_IS4V:
   /* (rise, fall, toz) */
   tim[0] = du.dis4v[4*__inum + 1];
   tim[1] = du.dis4v[4*__inum + 0];
   tim[2] = du.dis4v[4*__inum + 2];
   *ndels = 3;
   break;
  case DT_IS4V1:
   /* (rise, fall, toz) */
   tim[0] = (word64) du.dis4v1[4*__inum + 1];
   tim[1] = (word64) du.dis4v1[4*__inum + 0];
   tim[2] = (word64) du.dis4v1[4*__inum + 2];
   *ndels = 3;
   break;
  case DT_IS4V2:
   /* (rise, fall, toz) */
   tim[0] = (word64) du.dis4v2[4*__inum + 1];
   tim[1] = (word64) du.dis4v2[4*__inum + 0];
   tim[2] = (word64) du.dis4v2[4*__inum + 2];
   *ndels = 3;
   break;
  /* notice all the 6 forms are really size 16 tables */
  case DT_16V: 
   /* map from 16v table to 12 input list form */
   __map_16v_to_12vform(tim, du.d16v);
   __try_reduce_16vtab(tim, ndels); 
   break;
  case DT_IS16V:
   tim[0] = du.dis16v[16*__inum + 4];
   tim[1] = du.dis16v[16*__inum + 1];
   tim[2] = du.dis16v[16*__inum + 8];
   tim[3] = du.dis16v[16*__inum + 6];
   tim[4] = du.dis16v[16*__inum + 9];
   tim[5] = du.dis16v[16*__inum + 2];
   tim[6] = du.dis16v[16*__inum + 12];
   tim[7] = du.dis16v[16*__inum + 7];
   tim[8] = du.dis16v[16*__inum + 13];
   tim[9] = du.dis16v[16*__inum + 3];
   tim[10] = du.dis16v[16*__inum + 11];
   tim[11] = du.dis16v[16*__inum + 14];
   __try_reduce_16vtab(tim, ndels); 
   break;
  case DT_IS16V1:
   tim[0] = (word64) du.dis16v1[16*__inum + 4];
   tim[1] = (word64) du.dis16v1[16*__inum + 1];
   tim[2] = (word64) du.dis16v1[16*__inum + 8];
   tim[3] = (word64) du.dis16v1[16*__inum + 6];
   tim[4] = (word64) du.dis16v1[16*__inum + 9];
   tim[5] = (word64) du.dis16v1[16*__inum + 2];
   tim[6] = (word64) du.dis16v1[16*__inum + 12];
   tim[7] = (word64) du.dis16v1[16*__inum + 7];
   tim[8] = (word64) du.dis16v1[16*__inum + 13];
   tim[9] = (word64) du.dis16v1[16*__inum + 3];
   tim[10] = (word64) du.dis16v1[16*__inum + 11];
   tim[11] = (word64) du.dis16v1[16*__inum + 14];
   __try_reduce_16vtab(tim, ndels); 
   break;
  case DT_IS16V2:
   tim[0] = (word64) du.dis16v2[16*__inum + 4];
   tim[1] = (word64) du.dis16v2[16*__inum + 1];
   tim[2] = (word64) du.dis16v2[16*__inum + 8];
   tim[3] = (word64) du.dis16v2[16*__inum + 6];
   tim[4] = (word64) du.dis16v2[16*__inum + 9];
   tim[5] = (word64) du.dis16v2[16*__inum + 2];
   tim[6] = (word64) du.dis16v2[16*__inum + 12];
   tim[7] = (word64) du.dis16v2[16*__inum + 7];
   tim[8] = (word64) du.dis16v2[16*__inum + 13];
   tim[9] = (word64) du.dis16v2[16*__inum + 3];
   tim[10] = (word64) du.dis16v2[16*__inum + 11];
   tim[11] = (word64) du.dis16v2[16*__inum + 14];
   __try_reduce_16vtab(tim, ndels); 
   break;
  case DT_1X:
   xsp = __eval_xpr(du.d1x);
   tim[0] = (word64) xsp->ap[0];
   if (xsp->xslen > WBITS) tim[0] |= (((word64) xsp->ap[1]) << 32);
   *ndels = 1;
   __pop_xstk();
   break;
  case DT_4X:
   xsp = __eval_xpr(du.d4x[1]);
   tim[0] = (word64) xsp->ap[0]; 
   if (xsp->xslen > WBITS) tim[0] |= (((word64) xsp->ap[1]) << 32);
   __pop_xstk(); 
   xsp = __eval_xpr(du.d4x[0]);
   tim[1] = (word64) xsp->ap[0]; 
   if (xsp->xslen > WBITS) tim[1] |= (((word64) xsp->ap[1]) << 32);
   __pop_xstk(); 
   if (du.d4x[2] != NULL)
    {
     xsp = __eval_xpr(du.d4x[2]);
     tim[2] = (word64) xsp->ap[0]; 
     if (xsp->xslen > WBITS) tim[2] |= (((word64) xsp->ap[1]) << 32);
     __pop_xstk(); 
     *ndels = 3;
    }
   else
    {
     /* LOOKATME - is this right */
     if (tim[0] < tim[1]) tim[2] = tim[0]; else tim[2] = tim[1];
     *ndels = 2;
    }
   break;
  default: __case_terr(__FILE__, __LINE__); *ndels = 0;
 }
}

/*
 * map from 16 delay internal table to 12v input form list
 */
extern void __map_16v_to_12vform(word64 *ntim, word64 *tim)
{
 ntim[0] = tim[4]; ntim[1] = tim[1]; ntim[2] = tim[8];
 ntim[3] = tim[6]; ntim[4] = tim[9]; ntim[5] = tim[2];
 ntim[6] = tim[12]; ntim[7] = tim[7]; ntim[8] = tim[13];
 ntim[9] = tim[3]; ntim[10] = tim[11]; ntim[11] = tim[14];
}

/*
 * try to reduce a 16 value tim form to 2, 3, 6 if possible
 * this sets nnvals to number of values after reductions
 * for tim narrow just ignore higher values
 * reducible to 1 value will never be 16v in first place
 */
extern void __try_reduce_16vtab(word64 *tim, int32 *nvals)
{
 word64 tmin, tmax;

 /* know already mapped to 12v form */ 
 /* see if can reduce to 6 form */
 *nvals = 12;
 /* 0->x min(0->1, 0->z) */ 
 tmin = tim[0];
 if (tim[2] < tmin) tmin = tim[2];  
 if (tmin != tim[6]) return;

 /* 1->x min(1->0, 1->z) */ 
 tmin = tim[1];
 if (tim[4] < tmin) tmin = tim[4];  
 if (tmin != tim[8]) return;

 /* z->x min(z->0, z->1) */ 
 tmin = tim[5];
 if (tim[3] < tmin) tmin = tim[3];  
 if (tmin != tim[11]) return;

 /* x->0 max(1->0, z->0) */ 
 tmax = tim[1];
 if (tim[5] > tmax) tmax = tim[5];  
 if (tmax != tim[9]) return;

 /* x->1 max(z->1, 0->1) */ 
 tmax = tim[3];
 if (tim[0] > tmax) tmax = tim[0];  
 if (tmax != tim[7]) return;

 /* x->z max(1->z, 0->z) */ 
 tmax = tim[4];
 if (tim[2] > tmax) tmax = tim[2];  
 if (tmax != tim[10]) return;

 /* see if 6 can reduce to 2 */ 
 *nvals = 6; 
 if (tim[0] == tim[2] && tim[2] == tim[3] && tim[1] == tim[4]
  && tim[4] == tim[5])
  { *nvals = 2; return; }
 /* see if 6 can reduce to 3 */
 if ((tim[0] != tim[3]) || (tim[1] != tim[5]) || (tim[2] != tim[4])) return; 
 *nvals = 3;
}


/*
 * DELAY DEBUGGING ROUTINES
 */

/*
 * dump all 16 (really 12) delay transitions
 * DBG only routine
 */
/* ---
extern void __dbg_dump_del(union del_u du, word32 drep)
{
 register int32 i;
 struct xstk_t *xsp;
 struct expr_t *dxp;
 word64 dtab[16], tdel, dv0, dv1, dvx, dvz;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 switch ((byte) drep) {
  case DT_1V:
   for (i = 0; i < 16; i++) dtab[i] = du.d1v[0];
   break;
  case DT_IS1V:
   for (i = 0; i < 16; i++) dtab[i] = du.dis1v[__inum];
   break;
  case DT_IS1V1:
   for (i = 0; i < 16; i++)
    { dtab[i] = (word64) du.dis1v1[__inum]; }
   break;
  case DT_IS1V2:
   for (i = 0; i < 16; i++)
    { dtab[i] = (word64) du.dis1v2[__inum]; }
   break;
  case DT_4V:
   dtab[0x1] = du.d4v[0]; dtab[0x2] = du.d4v[0]; dtab[0x3] = du.d4v[0];
   dtab[0x4] = du.d4v[1]; dtab[0x6] = du.d4v[1]; dtab[0x7] = du.d4v[1];
   dtab[0x8] = du.d4v[2]; dtab[0x9] = du.d4v[2]; dtab[0xb] = du.d4v[2];
   dtab[0xc] = du.d4v[3]; dtab[0xe] = du.d4v[3]; dtab[0xd] = du.d4v[3];
   break;
  case DT_IS4V:
   tdel = du.d4v[4*__inum + 0];
   dtab[0x1] = tdel; dtab[0x2] = tdel; dtab[0x3] = tdel;
   tdel = du.d4v[4*__inum + 1];
   dtab[0x4] = tdel; dtab[0x6] = tdel; dtab[0x7] = tdel;
   tdel = du.d4v[4*__inum + 2];
   dtab[0x8] = tdel; dtab[0x9] = tdel; dtab[0xb] = tdel;
   tdel = du.d4v[4*__inum + 3];
   dtab[0xc] = tdel; dtab[0xe] = tdel; dtab[0xd] = tdel;
   break;
  case DT_IS4V1:
   tdel = (word64) du.dis4v1[4*__inum + 0];
   dtab[0x1] = tdel; dtab[0x2] = tdel; dtab[0x3] = tdel;
   tdel = (word64) du.dis4v1[4*__inum + 1];
   dtab[0x4] = tdel; dtab[0x6] = tdel; dtab[0x7] = tdel;
   tdel = (word64) du.dis4v1[4*__inum + 2];
   dtab[0x8] = tdel; dtab[0x9] = tdel; dtab[0xb] = tdel;
   tdel = (word64) du.dis4v1[4*__inum + 3];
   dtab[0xc] = tdel; dtab[0xe] = tdel; dtab[0xd] = tdel;
   break;
  case DT_IS4V2:
   tdel = (word64) du.dis4v2[4*__inum + 0];
   dtab[0x1] = tdel; dtab[0x2] = tdel; dtab[0x3] = tdel;
   tdel = (word64) du.dis4v1[4*__inum + 1];
   dtab[0x4] = tdel; dtab[0x6] = tdel; dtab[0x7] = tdel;
   tdel = (word64) du.dis4v1[4*__inum + 2];
   dtab[0x8] = tdel; dtab[0x9] = tdel; dtab[0xb] = tdel;
   tdel = (word64) du.dis4v1[4*__inum + 3];
   dtab[0xc] = tdel; dtab[0xe] = tdel; dtab[0xd] = tdel;
   break;
  case DT_16V:
   for (i = 0; i < 16; i++) dtab[i] = du.d16v[i];
   break;
  case DT_IS16V:
   for (i = 0; i < 16; i++) dtab[i] = du.dis16v[16*__inum + i];
   break;
  case DT_IS16V1:
   for (i = 0; i < 16; i++)
    {
     dtab[i] = (word64) du.dis16v1[16*__inum + i];
    }
   break;
  case DT_IS16V2:
   for (i = 0; i < 16; i++)
    {
     dtab[i] = (word64) du.dis16v2[16*__inum + i];
    }
   break;
  case DT_1X:
   xsp = __eval_xpr(du.d1x);
   delx_to_deltim(&tdel, du.d1x, xsp);
   for (i = 0; i < 16; i++) dtab[i] = tdel;
   __pop_xstk();
   break;
  case DT_4X:
   dxp = du.d4x[0];
   xsp = __eval_xpr(dxp);
   delx_to_deltim(&dv0, dxp, xsp);
   __pop_xstk();
   dxp = du.d4x[1];
   xsp = __eval_xpr(dxp);
   delx_to_deltim(&dv1, dxp, xsp);
   __pop_xstk();
   if (du.d4x[2] == NULL) dvz = (dv1 < dv0) ? dv1 : dv0;
   else
    {
     dxp = du.d4x[2];
     xsp = __eval_xpr(dxp);
     delx_to_deltim(&dvz, dxp, xsp);
     __pop_xstk();
    }
   if (du.d4x[2] == NULL) dvx = (dv1 < dv0) ? dv1 : dv0;
   else
    {
     if (dv0 < dv1) { dvx = (dv0 < dvz) ? dv0 : dvz; }
     else { dvx = (dv1 < dvz) ? dv1 : dvz; }
    }
   dtab[0x1] = dv0; dtab[0x2] = dv0; dtab[0x3] = dv0;
   dtab[0x4] = dv1; dtab[0x6] = dv1; dtab[0x7] = dv1;
   dtab[0x8] = dvz; dtab[0x9] = dvz; dtab[0xb] = dvz;
   dtab[0xc] = dvx; dtab[0xe] = dvx; dtab[0xd] = dvx;
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 __dbg_msg(":: to 0 transitions: 1->0 = %s, z->0 = %s , x->0 = %s\n",
  __to_timstr(s1, &(dtab[0x1])), __to_timstr(s2, &(dtab[0x2])),
  __to_timstr(s3, &(dtab[0x3])));
 __dbg_msg(":: to 1 transitions: 0->1 = %s, z->1 = %s , x->1 = %s\n",
  __to_timstr(s1, &(dtab[0x4])), __to_timstr(s2, &(dtab[0x6])),
  __to_timstr(s3, &(dtab[0x7])));
 __dbg_msg(":: to z transitions: 0->z = %s, 1->z = %s , x->z = %s\n",
  __to_timstr(s1, &(dtab[0x8])), __to_timstr(s2, &(dtab[0x9])),
  __to_timstr(s3, &(dtab[0xb])));
 __dbg_msg(":: to x transitions: 0->x = %s, 1->x = %s , z->x = %s\n",
  __to_timstr(s1, &(dtab[0xc])), __to_timstr(s2, &(dtab[0xd])),
  __to_timstr(s3, &(dtab[0xe])));
}
--- */
