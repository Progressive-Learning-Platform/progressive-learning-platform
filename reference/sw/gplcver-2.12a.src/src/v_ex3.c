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
 * run time execution routines - lhs stores and gate evalution routines
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
static void do_qc_assign(struct st_t *, struct expr_t *, int32,
 struct dceauxlstlst_t *);
static void do_qc_deassign(struct expr_t *);
static void do_qc_regforce(struct st_t *, struct expr_t *, int32,
 struct dceauxlstlst_t *);
static void do_qc_regrelease(struct expr_t *);
static void do_qc_wireforce(struct st_t *, struct expr_t *, int32,
 struct dceauxlstlst_t *);
static void do_1bit_wireforce(struct st_t *, struct net_t *, int32, int32,
 int32, struct itree_t *itp, struct dceauxlst_t *);
static void do_qc_wirerelease(struct expr_t *);
static void assign_alllhs_bits(struct expr_t *, struct xstk_t *);
static void do_qc2_regstore(struct net_t *, struct qcval_t *,
 struct xstk_t *);
static void do_qc2_wirestore(struct net_t *, struct qcval_t *,
 struct xstk_t *);
static void trace_conta_assign(struct expr_t *, word32 *, word32 *);
static void sched_conta_assign(struct expr_t *, register word32 *,
 register word32 *);
static void evtr_wdel_schd_1wirebit(register struct net_t *, register int32,
 register word32, register word32, int32);
static void schd_1pthwirebit(register struct net_t *, register int32,
 register word32, register word32);
static void evtr_schd_1pthwirebit(register struct net_t *, register int32,
 register word32, register word32);
static void prt_dbgpthtrmsg(struct spcpth_t *, word64);
static void get_impth_del(word64 *, struct net_t *, int32, struct mipd_t *);
static void prt_dbgimpthtrmsg(struct net_t *, int32, word64, word64);
static void evtr_sched_mipd_nchg(struct net_t *, int32, struct mipd_t *);
static void cancel_1mipdev(struct tev_t *);
static i_tev_ndx reschedule_1mipd(struct net_t *, int32, i_tev_ndx,
 word64, word64);
static void st_vecval(word32 *, int32, register word32 *, register word32 *);
static void chg_st_vecval(register word32 *, int32, register word32 *,
 register word32 *);
static void schedassign_to_bit(struct net_t *, struct expr_t *,
 struct expr_t *, register word32 *, register word32 *);
static void get_unknown_biti_val(struct net_t *, word32 *, word32 *, word32 *,
 word32 *, word32);
static void setx_ifnotval(word32 *, word32 *, word32);
static void chg_lhsbsel(register word32 *, int32, word32);
static int32 forced_assign_to_psel(struct expr_t *, int32, int32,
 struct net_t *, register word32 *, register word32 *);
static void schedassign_to_psel(struct expr_t *, register word32 *,
 register word32 *);
static void ins_walign(register word32 *, register word32 *, register int32);
static void cp_dofs_wval(register word32 *, register word32 *, int32, int32);
static void chg_st_unpckpsel(word32 *, int32, int32, int32, register word32 *,
 register word32 *);
static void chg_ins_wval(register word32 *, register int32, register word32 *,
 register int32);
static int32 chg_ofs_cmp(register word32 *, register word32 *, int32, int32);
static void eval_wide_gate(struct gate_t *, struct xstk_t *);
static void st_psel(struct net_t *, int32, int32, register word32 *,
 register word32 *);
static void chg_st_psel(struct net_t *, int32, int32, register word32 *,
 register word32 *);

/* extern prototypes (maybe defined in this module) */
extern void __exec2_proc_assign(struct expr_t *, register word32 *,
 register word32 *);
extern void __exec2_proc_concat_assign(struct expr_t *, word32 *, word32 *);
extern void __exec_qc_assign(struct st_t *, int32);
extern void __exec_qc_deassign(struct st_t *, int32);
extern void __exec_qc_wireforce(struct st_t *);
extern void __exec_qc_wirerelease(struct st_t *);
extern void __assign_qcaf(struct dcevnt_t *);
extern void __do_qc_store(struct net_t *, struct qcval_t *, int32);
extern void __xmrpush_refgrp_to_targ(struct gref_t *);
extern struct itree_t *__find_unrt_targitp(struct gref_t *,
 register struct itree_t *, int32);
extern struct inst_t *__get_gref_giarr_ip(struct gref_t *, int32,
 struct itree_t *); 
extern int32 __match_push_targ_to_ref(word32, struct gref_t *);
extern void __exec_ca_concat(struct expr_t *, register word32 *,
 register word32 *, int32);
extern void __stren_exec_ca_concat(struct expr_t *, byte *, int32);
extern void __exec_conta_assign(struct expr_t *, register word32 *,
 register word32 *, int32);
extern int32 __correct_forced_newwireval(struct net_t *, word32 *, word32 *);
extern void __bld_forcedbits_mask(word32 *, struct net_t *);
extern void __pth_schd_allofwire(struct net_t *, register word32 *,
 register word32 *, int32);
extern void __wdel_schd_allofwire(struct net_t *, register word32 *,
 register word32 *, int32);
extern void __pth_stren_schd_allofwire(struct net_t *, register byte *, int32);
extern void __wdel_schd_1wirebit(register struct net_t *, register int32,
 register word32, register word32, int32);
extern void __wdel_stren_schd_allofwire(struct net_t *, register byte *,
 int32);
extern void __emit_path_distinform(struct net_t *, struct pthdst_t *,
 word64 *);
extern void __emit_path_samewarn(struct net_t *, int32, struct tev_t *,
 word64 *, char *, word32);
extern void __emit_path_pulsewarn(struct pthdst_t *, struct tev_t *,
 word64 *, word64 *, char *, word32);
extern struct pthdst_t *__get_path_del(struct rngdwir_t *, int32, word64 *);
extern void __schedule_1wev(struct net_t *, int32, int32, word64, word64,
 word32, i_tev_ndx *, int32);
extern void __reschedule_1wev(i_tev_ndx, word32, word64, word64, i_tev_ndx *);
extern void __cancel_1wev(struct tev_t *);
extern void __st_val(struct net_t *, register word32 *, register word32 *);
extern void __st_perinst_val(union pck_u, int32, register word32 *,
 register word32 *);
extern void __chg_st_val(register struct net_t *, word32 *, word32 *);
extern void __assign_to_bit(struct net_t *, struct expr_t *, struct expr_t *,
 register word32 *, register word32 *);
extern void __assign_to_arr(struct net_t *, struct expr_t *, struct expr_t *,
 register word32 *, register word32 *);
extern int32 __forced_inhibit_bitassign(struct net_t *, struct expr_t *,
 struct expr_t *);
extern void __stren_schedorassign_unknown_bit(struct net_t *, word32, int32);
extern void __schedorassign_unknown_bit(struct net_t *np, word32 av,
 word32 bv, int32 schd_wire);
extern void __lhsbsel(register word32 *, register int32, word32);
extern void __chg_st_bit(struct net_t *, int32, register word32,
 register word32);
extern void __st_bit(struct net_t *, int32, register word32, register word32);
extern void __st_arr_val(union pck_u, int32, int32, int32, register word32 *,
 register word32 *);
extern void __chg_st_arr_val(union pck_u, int32, int32, int32,
 register word32 *, register word32 *);
extern void __assign_to_psel(struct expr_t *, int32, int32, struct net_t *,
 register word32 *, register word32 *);
extern void __lhspsel(register word32 *, register int32, register word32 *,
 register int32);
extern void __cp_sofs_wval(register word32 *, register word32 *,
 register int32, register int32);
extern void __chg_lhspsel(register word32 *, register int32,
 register word32 *, register int32);
extern void __sizchgxs(register struct xstk_t *, int32);
extern void __narrow_to1bit(register struct xstk_t *);
extern void __narrow_to1wrd(register struct xstk_t *);
extern void __fix_widened_tozs(struct xstk_t *, int32);
extern void __fix_widened_toxs(register struct xstk_t *, int32);
extern void __strenwiden_sizchg(struct xstk_t *, int32);
extern int32 __eval_logic_gate(struct gate_t *, word32, int32 *);
extern void __ld_gate_wide_val(word32 *, word32 *, word32 *, int32);
extern int32 __eval_bufif_gate(register struct gate_t *, word32, int32 *);
extern void __eval_nmos_gate(register word32);
extern void __eval_rnmos_gate(register word32);
extern void __eval_pmos_gate(register word32);
extern void __eval_rpmos_gate(register word32);
extern void __eval_cmos_gate(struct gate_t *);
extern char *__to_gassign_str(char *, struct expr_t *);
extern int32 __eval_udp(register struct gate_t *, word32, int32 *, int32);
extern void __grow_xstk(void);
extern void __chg_xstk_width(struct xstk_t *, int32);
extern void __rhspsel(register word32 *, register word32 *, register int32,
 register int32);
extern char *__to_idnam(struct expr_t *);
extern char *__to_wtnam(char *, struct net_t *);
extern char *__bld_lineloc(char *, word32, int32);
extern char *__msg2_blditree(char *, struct itree_t *);
extern char *__to_timstr(char *, word64 *);
extern char *__my_malloc(int32);
extern void __my_free(char *, int32);
extern void __find_call_force_cbs(struct net_t *, int32);
extern void __cb_all_rfs(struct net_t *, int32, int32);
extern void __find_call_rel_cbs(struct net_t *, int32);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern int32 __get_const_bselndx(register struct expr_t *);
extern void __assign_1mdrwire(register struct net_t *);
extern struct xstk_t *__eval_assign_rhsexpr(register struct expr_t *,
 register struct expr_t *);
extern char *__xregab_tostr(char *, word32 *, word32 *, int32,
 struct expr_t *);
extern char *__regab_tostr(char *, word32 *, word32 *, int32, int32, int32);
extern void __st_standval(register byte *, register struct xstk_t *, byte);
extern char *__st_regab_tostr(char *, byte *, int32);
extern int32 __vval_is1(register word32 *, int32);
extern int32 __wide_vval_is0(register word32 *, int32);
extern void __ld_wire_val(register word32 *, register word32 *,
 struct net_t *);
extern void __ld_bit(register word32 *, register word32 *,
 register struct net_t *, int32);
extern void __add_nchglst_el(register struct net_t *);
extern void __add_select_nchglst_el(register struct net_t *, register int32,
 register int32);
extern void __wakeup_delay_ctrls(register struct net_t *, register int32,
 register int32);
extern void __get_del(register word64 *, register union del_u, word32);
extern char *__to_evtrwnam(char *, struct net_t *, int32, int32,
 struct itree_t *);
extern char *__to_vnam(char *, word32, word32);
extern int32 __em_suppr(int32);
extern void __insert_event(register i_tev_ndx);
extern int32 __comp_ndx(register struct net_t *, register struct expr_t *);
extern int32 __get_arrwide(struct net_t *);
extern void __ld_psel(register word32 *, register word32 *,
 register struct net_t *, int32, int32);
extern struct xstk_t *__eval2_xpr(register struct expr_t *);
extern word32 __wrd_redxor(word32);
extern void __lunredand(int32 *, int32 *, word32 *, word32 *, int32);
extern void __lunredor(int32 *, int32 *, word32 *, word32 *, int32);
extern void __lunredxor(int32 *, int32 *, word32 *, word32 *, int32);
extern char *__gstate_tostr(char *, struct gate_t *, int32);
extern word32 __comb_1bitsts(word32, register word32, register word32);
extern struct xstk_t *__ndst_eval_xpr(struct expr_t *);
extern void __add_dmpv_chglst_el(struct net_t *);
extern void __qc_tran_wireforce(struct net_t *, int32, int32, int32,
 struct itree_t *, struct st_t *);
extern void __qc_tran_wirerelease(struct net_t *, int32, int32,
 struct itree_t *, struct expr_t *lhsx);
extern void __eval_tran_1bit(register struct net_t *, register int32);
extern char *__to_ptnam(char *, word32);
extern int32 __eval_1wide_gate(struct gate_t *, int32);
extern void __ins_wval(register word32 *, register int32, register word32 *,
 int32);
extern void __rem_stren(word32 *ap, word32 *bp, byte *, int32);
extern void __get_qc_wirrng(struct expr_t *, struct net_t **, int32 *,
 int32 *, struct itree_t **);
extern void __dcelst_on(struct dceauxlst_t *);
extern void __dcelst_off(struct dceauxlst_t *);

extern void __tr_msg(char *, ...);
extern void __gfwarn(int32, word32, int32, char *, ...);
extern void __sgfwarn(int32, char *, ...);
extern void __pv_warn(int32, char *,...);
extern void __gfinform(int32, word32, int32, char *, ...);
extern void __dbg_msg(char *, ...);
extern void __sgfinform(int32, char *, ...);
extern void __gferr(int32, word32, int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);

extern word32 __masktab[];

/*
 * LHS ASSIGN ROUTINES
 */

/*
 * PROCEDURAL ASSIGNMENT ROUTINES
 */

/*
 * immediate procedural assign from stacked value to lhs expr. xlhs
 * if assign changes, sets lhs change to T
 * know rhs side width same as lhs destination
 *
 * notice every path through here must add the net change element - called
 * this routine must be only possible way lhs procedural reg can be changed
 */
extern void __exec2_proc_assign(struct expr_t *xlhs, register word32 *ap,
 register word32 *bp)
{
 register struct net_t *np;
 int32 nd_itpop, ri1, ri2;
 struct expr_t *idndp, *ndx1;
 struct gref_t *grp;

 nd_itpop = FALSE;
 switch ((byte) xlhs->optyp) {
  case GLBREF:
   grp = xlhs->ru.grp;    
   __xmrpush_refgrp_to_targ(grp);
   nd_itpop = TRUE;
   /* FALLTHRU */
  case ID:
   np = xlhs->lu.sy->el.enp;
   if (np->frc_assgn_allocated && reg_fr_inhibit_(np)) goto chk_itpop;

   /* this add the changed wire to nchglst if needed */
   /* SJM 03/15/01 - change to fields in net record */
   if (np->nchg_nd_chgstore) __chg_st_val(np, ap, bp);
   else __st_val(np, ap, bp);

chk_itpop:
   if (nd_itpop) __pop_itstk();
   break;
  case LSB:
   /* for now first determine if array index */
   idndp = xlhs->lu.x;
   ndx1 = xlhs->ru.x;
   np = idndp->lu.sy->el.enp;
   /* notice can never assign or force arrays */
   if (np->n_isarr) __assign_to_arr(np, idndp, ndx1, ap, bp);
   else
    {
     if (np->frc_assgn_allocated)
      {
       if (idndp->optyp == GLBREF)
        {
         grp = idndp->ru.grp;
         __xmrpush_refgrp_to_targ(grp);
         nd_itpop = TRUE;
        }
       if (reg_fr_inhibit_(np)) goto chk_itpop;
       /* assign to bit handles own grep itree stack pushing */ 
       if (nd_itpop) __pop_itstk();
      }
     __assign_to_bit(np, idndp, ndx1, ap, bp);
    }
   break;
  case PARTSEL:
   idndp = xlhs->lu.x;
   np = idndp->lu.sy->el.enp;
   if (np->frc_assgn_allocated)
    {
     if (idndp->optyp == GLBREF)
      { grp = idndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
     /* do not assign if assign or force pending on inst. of wire */
     if (reg_fr_inhibit_(np)) goto chk_itpop;
     /* assign to psel handles own grep itree stack pushing */ 
     if (nd_itpop) __pop_itstk();
    }
   ri1 = (int32) __contab[xlhs->ru.x->lu.x->ru.xvi];
   ri2 = (int32) __contab[xlhs->ru.x->ru.x->ru.xvi];
   __assign_to_psel(idndp, ri1, ri2, np, ap, bp);
   break;
  case LCB:
   /* know evaluated rhs (maybe concatenate) in top of stack reg. */
   __exec2_proc_concat_assign(xlhs, ap, bp);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * execute an assignment or schedule to a concatentate (know non strength)
 * rhs value on stack apportioned into parts of concatenate
 * know xsp width same as lhs destination
 * caller must pop stack on return
 */
extern void __exec2_proc_concat_assign(struct expr_t *xlhs, word32 *ap, word32 *bp)
{
 register struct expr_t *catndp, *catlhsx;
 register int32 catxlen;
 int32 bi1;
 struct xstk_t *catxsp;

 /* do lhs concatenate assigns from left to right */
 for (catndp = xlhs->ru.x; catndp != NULL; catndp = catndp->ru.x)
  {
   catlhsx = catndp->lu.x;
   catxlen = catlhsx->szu.xclen;
   /* bi1 is low bit of rhs part select */
   /* length for catndp is distance from high bit of section to right end */ 
   bi1 = catndp->szu.xclen - catxlen;

   /* --- DBG remove 
   if (__debug_flg)
    __dbg_msg(
     "+++lhs proc: total cat wid=%u, low index=%d, wid=%u, remaining wid=%u\n",
     xlhs->szu.xclen, bi1, catxlen, catndp->szu.xclen);
  --- */

   /* select current pos. right width piece from rhs and put on tos reg */
   /* notice assignment always same width */
   push_xstk_(catxsp, catxlen);
   if (catxlen == 1)
    { catxsp->ap[0] = rhsbsel_(ap, bi1); catxsp->bp[0] = rhsbsel_(bp, bi1); }
   else
    {
     __rhspsel(catxsp->ap, ap, bi1, catxlen);
     __rhspsel(catxsp->bp, bp, bi1, catxlen);
    }

   /* know reals illegal in concatenates (rhs/lhs components of) */
   /* also nested lhs concatenates illegal - will never appear */
   /* notice this is part of immediate assign must not inc assign counter */
   /* and know no nested lhs concatenates */ 
   __exec2_proc_assign(catlhsx, catxsp->ap, catxsp->bp);
   __pop_xstk();
  }
}

/*
 * QUASI CONTINUOUS REG ASSIGN/DEASSIGN/FORCE/RELEASE ROUTINES 
 */

/*
 * exec a quasi-continuous assign or force of register (same as qc assign)
 * this is for both reg force and reg assign
 */
extern void __exec_qc_assign(struct st_t *stp, int32 is_force)
{
 register struct expr_t *catndp;
 register struct dceauxlstlst_t *dcllp;
 int32 catxlen, bi1; 
 struct expr_t *lhsx, *catlhsx;

 /* first evaluate rhs */
 lhsx = stp->st.sqca->qclhsx;
 /* only possibilities are concat and ID - list of ptrs to peri/bit lists */
 dcllp = stp->st.sqca->rhs_qcdlstlst;
 if (lhsx->optyp != LCB)
  {
   if (is_force) do_qc_regforce(stp, lhsx, -1, dcllp);
   else do_qc_assign(stp, lhsx, -1, dcllp);
  }
 else
  {
   /* concatenate case know lhs full wire - tricky extractions of rhs */
   for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x,
    dcllp = dcllp->dcelstlstnxt)
    {
     catlhsx = catndp->lu.x;
     catxlen = catlhsx->szu.xclen;
     bi1 = catndp->szu.xclen - catxlen;
     if (is_force) do_qc_regforce(stp, catlhsx, bi1, dcllp);
     else do_qc_assign(stp, catlhsx, bi1, dcllp);
    }
  }
}

/*
 * execute a quasi-continuous deassign
 * inverse of assign
 */
extern void __exec_qc_deassign(struct st_t *stp, int32 is_force)
{
 register struct expr_t *catndp;
 struct expr_t *lhsx, *catlhsx;

 /* SJM 07/19/02 - was wrongly accessing qconta not qcontdea record */
 lhsx = stp->st.sqcdea.qcdalhs;
 /* only possibilities are concat and ID */
 if (lhsx->optyp != LCB)
  {
   if (is_force) do_qc_regrelease(lhsx);
   else do_qc_deassign(lhsx);
  }
 else
  {
   /* concatenate case know lhs full wire - tricky extractions of rhs */
   for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x)
    {
     catlhsx = catndp->lu.x;
     if (is_force) do_qc_regrelease(catlhsx);
     else do_qc_deassign(catlhsx);
    }
  }
}

/*
 * exec quasi continuous assign for one expr in one inst.
 *
 * know lhs always entire register - no assign for wires - lhs can be xmr 
 *
 * if active force do nothing but save assign rhs expr. so if force released
 * assign expr. evaluated and activated
 *
 * SJM 06/23/02 - new qcaf algorithm build qcaf lists during prep and moves
 * to and from stmt sqca fields and turns on/off when needed
 */
static void do_qc_assign(struct st_t *qcastp, struct expr_t *lhsx, int32 rhsbi,
 struct dceauxlstlst_t *dcllp)
{
 int32 nd_itpop, stmt_inum;
 struct net_t *np;
 struct gref_t *grp;
 struct qcval_t *frc_qcp, *assgn_qcp; 

 /* assign to lhs itree loc. */
 nd_itpop = FALSE;
 /* SJM 05/23/03 - need to access stmt info from original inum if XMR */
 stmt_inum = __inum;
 if (lhsx->optyp == GLBREF)
  { grp = lhsx->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
 else if (lhsx->optyp != ID) __case_terr(__FILE__, __LINE__);
 np = lhsx->lu.sy->el.enp;

 /* for reg var 2 records always allocated, first is force and 2nd assign */
 frc_qcp = &(np->nu2.qcval[2*__inum]);
 assgn_qcp = &(np->nu2.qcval[2*__inum + 1]);
 /* if active force of wire just fill assign qcval so when force removed */
 /* assign becomes active - but do not make qc assign active */
 if (frc_qcp->qc_active)
  {
   /* notice this can replace other over-ridden */
   assgn_qcp->qc_overridden = TRUE;
   assgn_qcp->qcstp = qcastp;
   assgn_qcp->qcrhsbi = rhsbi; 
   assgn_qcp->qclhsbi = -1; 
   /* need lhs target inst. loc including non xmr same for change assign */
   assgn_qcp->lhsitp = __inst_ptr;
   /* assign group of dces (usually 1) - if lhs concat this lhs expr's one */
   /* but don't turn on yet */
   /* SJM 05/23/03 - dcllp is linked off stmt and has stmt instances */
   assgn_qcp->qcdcep = dcllp->dcelsttab[stmt_inum];

   if (nd_itpop) __pop_itstk();

   if (__debug_flg && __ev_tracing)
    {
     char s1[RECLEN];

     /* context for trace message is stmt not possible xmr */
     __tr_msg(
      ":: quasi-continuous assign to reg %s at %s in %s now %s - no effect active force\n",
      __to_idnam(lhsx), __bld_lineloc(__xs, (word32) __sfnam_ind, __slin_cnt),
      __msg2_blditree(__xs2, __inst_ptr), __to_timstr(s1, &__simtime));
    }
   return;
  }
 /* if active assign, deactivate before setting new - know fields replaced */
 if (assgn_qcp->qc_active)
  {
   assgn_qcp->qc_active = FALSE;
   /* turn on dces after doing store if rhs dces */
   /* SJM 08/18/02 - bug - this was turning on but must turn off */
   if (assgn_qcp->qcdcep != NULL) __dcelst_off(assgn_qcp->qcdcep);
  }

 /* SJM 07/19/02 - was not making assign active */
 assgn_qcp->qc_active = TRUE;
 /* DBG remove - can't be over-ridden by force if get here */
 if (assgn_qcp->qc_overridden) __misc_terr(__FILE__, __LINE__);
 /* --- */
 /* but still save in case reg var force removed */
 assgn_qcp->qcstp = qcastp;
 assgn_qcp->qcrhsbi = rhsbi; 
 assgn_qcp->qclhsbi = -1;
 /* do store and build dces in ref. itree loc. */ 
 assgn_qcp->lhsitp = __inst_ptr;
 /* assign group of dces (usually 1) - if lhs concat this lhs expr's one */
 assgn_qcp->qcdcep = dcllp->dcelsttab[stmt_inum];

 if (nd_itpop) __pop_itstk();

 if (__debug_flg && __ev_tracing)
  {
   char s1[RECLEN];

   __tr_msg(":: quasi-continuous assign to reg %s at %s in %s now %s\n",
    __to_idnam(lhsx), __bld_lineloc(__xs, (word32) __sfnam_ind, __slin_cnt),
    __msg2_blditree(__xs2, __inst_ptr), __to_timstr(s1, &__simtime));
  }

 /* these routines need ref. itree loc - they set right context for xmr */
 __do_qc_store(np, assgn_qcp, TRUE);

 /* turn on dces after doing store if rhs dces */
 if (assgn_qcp->qcdcep != NULL) __dcelst_on(assgn_qcp->qcdcep);

 /* FIXME ??? - assign callbacks go here */
}

/*
 * do the quasi continuous deassign for one lhs expr.
 *
 * know lhs always entire register and cannot be wire
 * notice lhs here can be xmr
 * sematics is deassign leaves val until next assgn which now happens execs
 * value of wire not changed here  
 * for xmr (reg only possible), must exec in target itree loc.
 */
static void do_qc_deassign(struct expr_t *lhsx)
{
 int32 nd_itpop;
 struct net_t *np;
 struct gref_t *grp;
 struct qcval_t *frc_qcp, *assgn_qcp;
 char s1[RECLEN];

 strcpy(s1, "");
 /* must work in itree loc. of lhs if xmr */
 nd_itpop = FALSE;
 if (lhsx->optyp == GLBREF)
  { grp = lhsx->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
 else if (lhsx->optyp != ID) __case_terr(__FILE__, __LINE__);
 np = lhsx->lu.sy->el.enp;
 
 /* for reg var 2 records always allocated, first is force and 2nd assign */
 frc_qcp = &(np->nu2.qcval[2*__inum]);
 assgn_qcp = &(np->nu2.qcval[2*__inum + 1]);
 if (!assgn_qcp->qc_active && !assgn_qcp->qc_overridden)
  {
   __sgfinform(462,
    "attempted deassign of reg %s in instance %s failed - not assigned",
    __to_idnam(lhsx), __msg2_blditree(__xs, __inst_ptr));
   goto done;
  }
 assgn_qcp->qc_active = FALSE; 
 /* turn off dces but do not empty qcval rec - will get refilled if needed */
 /* if over-ridden were not turned so do not need to turn off */
 if (!assgn_qcp->qc_overridden)
  {
   if (assgn_qcp->qcdcep != NULL) __dcelst_off(assgn_qcp->qcdcep);
  }
 assgn_qcp->qcdcep = NULL; 
 assgn_qcp->qc_overridden = FALSE;
 __assign_active = FALSE; 
 /* LOOKATME - extra work to set force flag only used for tracing */
 if (frc_qcp->qc_active)
  {
   /* dce list can be empty if forced to constant */
   __force_active = TRUE;
   strcpy(s1, " force active");
  }
 if (__debug_flg && __ev_tracing)
  {
   char s2[RECLEN];

   /* messages needs itree context of stmt */
   if (nd_itpop) __pop_itstk();

   __tr_msg(":: quasi-continuous deassign of reg %s%s at %s in %s now %s\n",
    __to_idnam(lhsx), s1, __bld_lineloc(__xs, (word32) __sfnam_ind,
    __slin_cnt), __msg2_blditree(__xs2, __inst_ptr),
    __to_timstr(s2, &__simtime));

   return;
  }

done:
 if (nd_itpop) __pop_itstk();
}

/*
 * do the quasi continuous force for reg variables
 *
 * know lhs always entire register
 * lhs here can be xmr 
 * force of entire reg only overrides possible active reg assign
 *
 * SJM 06/15/02 - new algorithm leaves dce list always linked on, turns on/off
 * when active and keeps one different qc dce lists for each lhs concat el
 * so can reg release each reg listed in lhs concats separately
 */
static void do_qc_regforce(struct st_t *qcastp, struct expr_t *lhsx,
  int32 rhsbi, struct dceauxlstlst_t *dcllp)
{
 int32 nd_itpop, stmt_inum;
 struct net_t *np;
 struct gref_t *grp;
 struct qcval_t *assgn_qcp, *frc_qcp; 
 struct itree_t *itp;
 char s1[RECLEN];

 strcpy(s1, "");
 /* if lhs xmr, change to target since forcing in target instance */
 nd_itpop = FALSE;
 /* for XMR need stmt context inum for getting dcellst linked on stmt */
 stmt_inum = __inum;
 itp = NULL;
 if (lhsx->optyp == GLBREF)
  {
   grp = lhsx->ru.grp;
   __xmrpush_refgrp_to_targ(grp);
   nd_itpop = TRUE;
  }
 else if (lhsx->optyp != ID) __case_terr(__FILE__, __LINE__);
 np = lhsx->lu.sy->el.enp;

 /* for reg var 2 records always allocated, first is force and 2nd assign */
 frc_qcp = &(np->nu2.qcval[2*__inum]);
 assgn_qcp = &(np->nu2.qcval[2*__inum + 1]);

 /* case 1, force pending */
 if (frc_qcp->qc_active) 
  { 
   strcat(s1, " replace force");
   /* turn off current (if lhs concat many) list of rhs dces */
   if (frc_qcp->qcdcep != NULL) __dcelst_off(frc_qcp->qcdcep);
   frc_qcp->qcdcep = NULL;
   frc_qcp->qc_active = FALSE;
   goto setup_force;
  }

 /* if qc assign pending, inactivate but leave ptrs and set bit */
 if (assgn_qcp->qc_active)
  {
   /* turn off the assign list - will be turned on if reg force released */
   if (assgn_qcp->qcdcep != NULL) __dcelst_off(assgn_qcp->qcdcep);
   assgn_qcp->qc_active = FALSE;
   assgn_qcp->qc_overridden = TRUE;
   strcat(s1, " override assign");
  }
setup_force:
 /* setup the new force */
 frc_qcp->qc_active = TRUE;
 frc_qcp->qcstp = qcastp; 
 frc_qcp->qcrhsbi = rhsbi; 
 frc_qcp->qclhsbi = -1;
 frc_qcp->lhsitp = __inst_ptr;
 /* SJM 06/23/02 - add qc dcep list (right one if lhs cat) to qcval rec */
 /* one needed for each lhs element because reg release can do separately */
 frc_qcp->qcdcep = dcllp->dcelsttab[stmt_inum];

 if (nd_itpop) { itp = __inst_ptr; __pop_itstk(); }
 if (__debug_flg && __ev_tracing)
  {
   char s2[RECLEN];

   /* message needs itree context of stmt not lhs if xmr */ 
   __tr_msg(":: quasi-continuous force of reg %s%s at %s in %s now %s\n",
    __to_idnam(lhsx), s1, __bld_lineloc(__xs, (word32) __sfnam_ind,
    __slin_cnt), __msg2_blditree(__xs2, __inst_ptr),
    __to_timstr(s2, &__simtime));
  }

 /* start force by storing rhs of force - dces will cause dce chges */
 /* these routines need ref itree loc not lhs xmr */
 __do_qc_store(np, frc_qcp, TRUE);

 /* SJM 07/19/02 - must not turn on any rhs dces until store done */
 /* turn on reg force for this set of dces if non constant rhs */ 
 if (frc_qcp->qcdcep != NULL) __dcelst_on(frc_qcp->qcdcep);

 /* but these need to run in itree context of lhs */
 if (nd_itpop) __push_itstk(itp);

 /* notice can have both many wire specific and many all cbs */
 if (__num_vpi_force_cbs > 0) __find_call_force_cbs(np, -1);
 if (__vpi_force_cb_always) __cb_all_rfs(np, -1, TRUE);

 if (nd_itpop) __pop_itstk();
}

/*
 * do a quasi continuous release for a reg lhs (lvalue)
 *
 * know lhs always entire register
 * lhs here can be xmr
 * releasing reg with pending active assign re-establishes assign
 *
 * SJM 06/23/02 - new qcaf algorithm build qcaf lists during prep and moves
 * to and from stmt sqca fields and turns on/off when needed
 */
static void do_qc_regrelease(struct expr_t *lhsx)
{
 int32 nd_itpop;
 struct net_t *np;
 struct gref_t *grp;
 struct qcval_t *assgn_qcp, *frc_qcp; 
 struct itree_t *itp;
 char s1[RECLEN];

 strcpy(s1, "");
 /* must release reg in itree loc. of lhs if xmr */
 nd_itpop = FALSE;
 itp = NULL;
 if (lhsx->optyp == GLBREF)
  { grp = lhsx->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
 else if (lhsx->optyp != ID) __case_terr(__FILE__, __LINE__);
 np = lhsx->lu.sy->el.enp;

 /* for reg var 2 records always allocated, first is force and 2nd assign */
 frc_qcp = &(np->nu2.qcval[2*__inum]);
 assgn_qcp = &(np->nu2.qcval[2*__inum + 1]);

 /* if no force, nothing to do */
 if (!frc_qcp->qc_active)
  {
   /* message here needs lhs xmr context */
   __sgfinform(465,
    "attempted release of reg %s in instance %s failed - never forced",
    __to_idnam(lhsx), __msg2_blditree(__xs, __inst_ptr));
   if (nd_itpop) __pop_itstk();
   return;
  }
 frc_qcp->qc_active = FALSE;
 /* turn off active force dces */
 if (frc_qcp->qcdcep != NULL) __dcelst_off(frc_qcp->qcdcep);
 frc_qcp->qcdcep = NULL;
 __force_active = FALSE;

 /* if pending but inactive assign - must reactivate it */
 if (assgn_qcp->qc_overridden)
  {
   /* these need to run in itree context of stmt not lhs if xmr */
   if (nd_itpop) { itp = __inst_ptr; __pop_itstk(); }

   __do_qc_store(np, assgn_qcp, TRUE);
   /* build the QCAF dcelst - this must be build in rhs ref. itree loc. */

   assgn_qcp->qc_active = TRUE; 
   assgn_qcp->qc_overridden = FALSE;
   /* turn stored last dce list on */
   /* FIXME - this is never first time */
   if (assgn_qcp->qcdcep != NULL) __dcelst_on(assgn_qcp->qcdcep);
   __assign_active = TRUE;
   strcpy(s1, " reactivating assign");
  }
 else { if (nd_itpop) { itp = __inst_ptr; __pop_itstk(); } }

 /* message need stmt itree context */
 if (__debug_flg && __ev_tracing)
  {
   char s2[RECLEN];

   __tr_msg(":: quasi-continuous release of reg %s%s at %s in %s now %s\n",
    __to_idnam(lhsx), s1, __bld_lineloc(__xs, (word32) __sfnam_ind,
    __slin_cnt), __msg2_blditree(__xs2, __inst_ptr),
    __to_timstr(s2, &__simtime));
  }

 /* these must run in lhs itree context for xmr */ 
 if (nd_itpop) __push_itstk(itp);

 /* notice can have both many wire specific and many all cbs */
 if (__num_vpi_rel_cbs > 0) __find_call_rel_cbs(np, -1);
 if (__vpi_rel_cb_always) __cb_all_rfs(np, -1, FALSE);

 if (nd_itpop) __pop_itstk();
}

/*
 * QUASI CONTINUOUS WIRE FORCE/RELEASE ROUTINES 
 */

/*
 * execute a quasi-continuous force on a wire
 * possibilities here are wire, constant bit select, part select
 * also concat of above
 * wire must be scalared and everything decomposed to bits
 */
extern void __exec_qc_wireforce(struct st_t *stp)
{
 register struct expr_t *catndp;
 register struct dceauxlstlst_t *dcllp;
 int32 catxlen, bi1; 
 struct expr_t *lhsx, *catlhsx;

 /* DBG remove --
 struct itree_t *sav_itp = __inst_ptr;
 ---*/

 lhsx = stp->st.sqca->qclhsx;
 dcllp = stp->st.sqca->rhs_qcdlstlst;
 /* only possibilities are concat and ID */
 if (lhsx->optyp != LCB) do_qc_wireforce(stp, lhsx, -1, dcllp);
 else
  {
   /* concatenate case know lhs full wire - tricky extractions of rhs */
   for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x,
    dcllp = dcllp->dcelstlstnxt)
    {
     catlhsx = catndp->lu.x;
     catxlen = catlhsx->szu.xclen;
     bi1 = catndp->szu.xclen - catxlen;
     do_qc_wireforce(stp, catlhsx, bi1, dcllp);
    }
  }
 /* DBG remove --
 if (sav_itp != __inst_ptr) __misc_terr(__FILE__, __LINE__); 
 ---*/
}

/*
 * execute a quasi-continuous release
 * only scalared wires or selects or cats not regs
 * wire force/release is one level only
 * called in itree context of release stmt 
 */
extern void __exec_qc_wirerelease(struct st_t *stp)
{
 register struct expr_t *catndp;
 struct expr_t *lhsx, *catlhsx;

 /* DBG remove --
 struct itree_t *sav_itp = __inst_ptr;
 ---*/

 lhsx = stp->st.sqcdea.qcdalhs; 
 /* only possibilities are concat and ID */
 if (lhsx->optyp != LCB) do_qc_wirerelease(lhsx);
 else
  {
   /* concatenate case know lhs full wire - tricky extractions of rhs */
   for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x)
    { catlhsx = catndp->lu.x; do_qc_wirerelease(catlhsx); }
  }
 /* DBG remove --
 if (sav_itp != __inst_ptr) __misc_terr(__FILE__, __LINE__); 
 ---*/
}

/*
 * after possible concat unwinding, exec the wire force
 *
 * wire force is bit by bit unless vectored wire (when only entire wire)
 *
 * force which is for debugging overrides any wire delay assign
 * when wire change happens (wire event process) if force active, no assign
 * rhsbi is low bit of possible rhs section select (0 for not concat)
 * this is called with stmt itree loc even if lhs xmr and handled push/pop
 *
 * SJM 11/14/00 - tran channel (inout port) force now is separate routine
 * LOOKATME - think could simplify since for wire force always one bit
 */
static void do_qc_wireforce(struct st_t *qcfstp, struct expr_t *lhsx,
 int32 rhsbi, struct dceauxlstlst_t *dcllp)
{
 register int32 bi, ibase;
 int32 biti, bitj, rhsbi2, lhsbi2, ndx;
 struct net_t *np;
 struct itree_t *itp;
 struct dceauxlst_t *qcdcep;
 
 /* step 1: get the wire range */
 /* for psel or vector, range is biti down to bitj - for scalar 0,0 */
 /* this computes any xmr new itp but does not push it */
 __get_qc_wirrng(lhsx, &np, &biti, &bitj, &itp); 

 /* SJM 11/14/00 - if wire in tran chan, force all wires in it*/
 if (np->ntraux != NULL)
  {
   /* this pushes and pops lhs xmr itree context itp if needed */ 
   /* routine also handles any PLI force callbacks */
   /* SJM 02/26/02 - no dces so new pre-build qcaf dces does not chg this */
   __qc_tran_wireforce(np, biti, bitj, rhsbi, itp, qcfstp);

   /* SJM - 04/15/01 - must eval tran chan in lhs xmr itree context */
   if (itp != NULL) __push_itstk(itp);
   /* SJM - 03/15/01 - must re-eval all bits if this is vector range */
   /* new tran force algorithm - force wire in tran channel and then */ 
   /* re-eval channel */ 
   for (bi = biti; bi >= bitj; bi--) { __eval_tran_1bit(np, bi); }
   if (itp != NULL) __pop_itstk();

   /* but trace message must use stmt context */
   if (__debug_flg && __ev_tracing)
    {
     char s1[RECLEN], s2[RECLEN];

     __tr_msg(":: quasi-continuous force of wire %s at %s in %s now %s\n",
      __msgexpr_tostr(__xs, lhsx), __bld_lineloc(__xs2,
      (word32) __sfnam_ind, __slin_cnt), __msg2_blditree(s1, __inst_ptr),
     __to_timstr(s2, &__simtime));
    }
   return;
  }

 /* SJM 07/22/02 - need to access dce list form stmt inum not lhs if xmr */
 /* access dce list from stmt not lhs itree context */
 qcdcep = dcllp->dcelsttab[__inum];
 /* wire force must run in lhs itree context */ 
 if (itp != NULL) __push_itstk(itp);
 /* ibase is lhs xmr qcval base */ 
 ibase = __inum*np->nwid;
 if (!np->n_isavec)
  {
   /* DBG remove */
   if (biti != 0 || bitj != 0) __misc_terr(__FILE__, __LINE__);
   /* --- */

   /* this pops itstk if needed */
   do_1bit_wireforce(qcfstp, np, ibase, -1, rhsbi, itp, qcdcep);
   ndx = -1;    
   goto done;
  }

 /* force every bit in range using same rhs dce list for each */
 for (bi = bitj; bi <= biti; bi++)
  {
   /* rhsbi is low bit of possible lhs concat caused rhs select */
   if (rhsbi == -1) rhsbi2 = bi - bitj;
   else rhsbi2 = rhsbi + (bi - bitj);
   lhsbi2 = bi;

   do_1bit_wireforce(qcfstp, np, ibase, lhsbi2, rhsbi2, itp, qcdcep);
  }

 if (biti != bitj) ndx = -1; else ndx = biti;

done:
 if (__debug_flg && __ev_tracing)
  {
   char s1[RECLEN], s2[RECLEN];

   /* this must run in itree context of stmt not possible lhs xmr */
   if (itp != NULL) __pop_itstk(); 
   __tr_msg(":: quasi-continuous force of wire %s at %s in %s now %s\n",
    __msgexpr_tostr(__xs, lhsx), __bld_lineloc(__xs2, (word32) __sfnam_ind,
     __slin_cnt), __msg2_blditree(s1, __inst_ptr),
    __to_timstr(s2, &__simtime));
   if (itp != NULL) __push_itstk(itp); 
  }

 /* these need to run in itree context of possible lhs xmr */
 /* notice can have both many wire specific and many all cbs */
 if (__num_vpi_force_cbs > 0) __find_call_force_cbs(np, ndx);
 if (__vpi_force_cb_always) __cb_all_rfs(np, ndx, TRUE);

 /* on return, itree context of force stmt needed */ 
 if (itp != NULL) __pop_itstk(); 
}

/*
 * do 1 bit wire force 
 *
 * this is called with itree context of lhs that is passed in itp if lhs xmr
 * it handles it own popping and pushing and leave itree same as called 
 */
static void do_1bit_wireforce(struct st_t *qcstp, struct net_t *np,
 int32 ibase, int32 lhsbi, int32 rhsbi, struct itree_t *itp,
 struct dceauxlst_t *qcdcep)
{
 int32 biti;
 struct qcval_t *frc_qcp;

 if (lhsbi == -1) biti = 0; else biti = lhsbi;

 /* for reg var 2 records always allocated, first is force and 2nd assign */
 frc_qcp = &(np->nu2.qcval[ibase + biti]);
 /* forcing to different expr */
 if (frc_qcp->qc_active)
  {
   if (frc_qcp->qcdcep != NULL) __dcelst_off(frc_qcp->qcdcep);
   frc_qcp->qcdcep = NULL;
  }

 /* setup the new force */
 frc_qcp->qcstp = qcstp; 
 /* rhsbi is low bit of rhs range in case lhs concatenate */ 
 frc_qcp->qcrhsbi = rhsbi;
 frc_qcp->qclhsbi = lhsbi;
 /* store and build dces in lhs ref. itree location */ 
 frc_qcp->lhsitp = __inst_ptr;
 frc_qcp->qcdcep = qcdcep;

 /* store and setup dces needs to run in stmt itree context */ 
 if (itp != NULL) __pop_itstk();

 /* start force by storing rhs of force - dces will cause dce chges */
 __do_qc_store(np, frc_qcp, FALSE);
 frc_qcp->qc_active = TRUE;

 /* turn on dces after doing store */
 if (frc_qcp->qcdcep != NULL) __dcelst_on(frc_qcp->qcdcep);

 if (itp != NULL) __push_itstk(itp);
}

/*
 * get qc wire element (after lhs concat separation) 
 * this sets needed itree loc. to itpp, also sets wire, and range 
 */
extern void __get_qc_wirrng(struct expr_t *lhsx, struct net_t **nnp,
 int32 *biti, int32 *bitj, struct itree_t **itpp)
{
 int32 nd_itpop;
 word32 *wp;
 struct gref_t *grp;
 struct net_t *np;
 struct expr_t *idndp, *ndx;

 *itpp = NULL; 
 np = NULL;
 nd_itpop = FALSE;
 switch ((byte) lhsx->optyp) {
  case GLBREF:
   grp = lhsx->ru.grp;
   __xmrpush_refgrp_to_targ(grp);
   nd_itpop = TRUE;
   /* FALLTHRU */
  case ID:
   np = lhsx->lu.sy->el.enp;
   *biti = np->nwid - 1;
   *bitj = 0;
   break;
  case LSB: case PARTSEL:
   idndp = lhsx->lu.x;
   if (idndp->optyp == GLBREF)
    {
     grp = idndp->ru.grp;
     __xmrpush_refgrp_to_targ(grp);
     nd_itpop = TRUE;
    }
   np = idndp->lu.sy->el.enp;
   /* know error before here if non in range constant value */ 
   if (lhsx->optyp == LSB) *bitj = *biti = __get_const_bselndx(lhsx);
   else
    {
     ndx = lhsx->ru.x->lu.x;
     __inst_mod = __inst_mod;
     wp = &(__contab[ndx->ru.xvi]);
     *biti = wp[0];
     ndx = lhsx->ru.x->ru.x;
     wp = &(__contab[ndx->ru.xvi]);
     *bitj = wp[0];
    }
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 if (nd_itpop) { *itpp = __inst_ptr;  __pop_itstk(); }
 *nnp = np;
}

/*
 * after possible concat unwinding, exec the wire section release
 *
 * tricky part is need to force evaluation and store of all drivers
 * LOOKATME - is there any reason cannot just call multi driver eval
 * even for 1 or no driver case 
 */
static void do_qc_wirerelease(struct expr_t *lhsx)
{
 register int32 bi, ibase;
 int32 biti, bitj, all_forced, ndx;
 struct net_t *np;
 struct itree_t *itp;
 struct qcval_t *frc_qcp;
 char s1[RECLEN];
 
 /* step 1: get the wire range */
 /* for psel or vector, range is biti down to bitj - for scalar 0,0 */
 __get_qc_wirrng(lhsx, &np, &biti, &bitj, &itp); 

 /* SJM 11/14/00 - if wire in tran chan, force all wires in it*/
 if (np->ntraux != NULL)
  {
   __qc_tran_wirerelease(np, biti, bitj, itp, lhsx);

   /* SJM 04/15/01 - need to eval tran in lhs itree context */
   if (itp != NULL) __push_itstk(itp);

   /* SJM - 03/15/01 - must re-eval all bits if this is vector range */
   /* new tran force algorithm - force wire in tran channel and then */ 
   /* re-eval channel */ 
   for (bi = biti; bi >= bitj; bi--)
    {
     __eval_tran_1bit(np, bi);
    }
   if (itp != NULL) __pop_itstk();

   /* but messages needs stmt itree context */
   if (__debug_flg && __ev_tracing)
    {
     char s2[RECLEN];

     __tr_msg(":: quasi-continuous force of wire %s at %s in %s now %s\n",
      __msgexpr_tostr(__xs, lhsx), __bld_lineloc(__xs2,
      (word32) __sfnam_ind, __slin_cnt), __msg2_blditree(s1, __inst_ptr),
     __to_timstr(s2, &__simtime));
    }
   return;
  }

 /* SJM 07/23/02 - this needs lhs expr context */
 if (itp != NULL) __push_itstk(itp);

 ibase = __inum*np->nwid;
 for (bi = 0; bi < np->nwid; bi++)
  {
   frc_qcp = &(np->nu2.qcval[ibase + bi]);
   if (frc_qcp->qc_active) goto some_bit_forced;
  }
 strcpy(s1, " - no bits forced");
 __sgfinform(465,
  "attempted release of %s in instance %s failed%s",
  __msgexpr_tostr(__xs2, lhsx), __msg2_blditree(__xs, __inst_ptr), s1);
 /* SJM 04/15/01 - if no bits forced, do not try to exec call backs */
 if (itp != NULL) __pop_itstk();
 return;

some_bit_forced:
 if (__debug_flg && __ev_tracing)
  {
   char s2[RECLEN];

   __tr_msg(":: quasi-continuous release of wire %s at %s in %s now %s\n",
    __msgexpr_tostr(__xs, lhsx), __bld_lineloc(__xs2, (word32) __sfnam_ind,
     __slin_cnt), __msg2_blditree(s1, __inst_ptr),
    __to_timstr(s2, &__simtime));
  }

 /* know some forced or will not get here - release all in range */
 /* notice wire force is per bit but no second qc assigns for wires */  
 all_forced = TRUE;
 for (bi = biti; bi >= bitj; bi--)
  {
   frc_qcp = &(np->nu2.qcval[ibase + bi]);
   if (!frc_qcp->qc_active) { all_forced = FALSE; continue; }
   frc_qcp->qc_active = FALSE;

   /* turn off dces after doing store if rhs non constant */ 
   if (frc_qcp->qcdcep != NULL) __dcelst_off(frc_qcp->qcdcep);
   frc_qcp->qcdcep = NULL;
  }

 /* assign expected value now that force removed by evaling all drivers */
 /* must re-eval entire wire since other drivers may overlap forced range */
 /* notice this must be called from target of xmr and/or col. to */
 /* it handles moving back to references */
 __assign_1mdrwire(np);

 if (!all_forced)
  {
   if (itp != NULL) __pop_itstk();
   strcpy(s1, " - some bits forced");
   __sgfinform(465, "attempted release of %s in instance %s failed%s",
    __msgexpr_tostr(__xs2, lhsx), __msg2_blditree(__xs, __inst_ptr), s1);
   if (itp != NULL) __push_itstk(itp);

   /* here still need to try to exec PLI callbacks */
  } 

 /* must exec call backs in possible lhs xmr itree context */ 
 /* FIXME - why not calling for every bit in range - only 1 bit possible? */
 if (biti != bitj) ndx = -1; else ndx = biti;
 /* notice can have both many wire specific and many all cbs */
 if (__num_vpi_rel_cbs > 0) __find_call_rel_cbs(np, ndx);
 if (__vpi_rel_cb_always) __cb_all_rfs(np, ndx, FALSE);

 if (itp != NULL) __pop_itstk();
}

/*
 * ROUTINES TO EXEC QUASI-CONTINOUS ASSIGN OPERATIONS
 */

/*
 * exec the quasi-continuous assign or force under control of qcval 
 * change may be in rhs xmr target, but must exec this is ref.
 * rhs wire np changed
 *
 * SJM 06/16/02 - new algorithm that builds qcval and qc dces once
 * works because dce points to corresponding qcval record
 */
extern void __assign_qcaf(struct dcevnt_t *dcep)
{
 register struct expr_t *catndp;
 int32 nd_itpop, nd_itpop2;
 struct qcval_t *qcvalp;
 struct st_t *stp;
 struct expr_t *lhsx, *lhsx2, *rhsx; 
 struct xstk_t *xsp;
 struct net_t *np;

 nd_itpop = FALSE;
 /* first must move itree loc. back to ref. (where lhs and rhs are) */ 
 if (dcep->dce_1inst) { nd_itpop = TRUE; __push_itstk(dcep->dce_refitp); }  

 /* know some bit in rhs changed, get one qcval */
 /* here using fmon field really as union since unused in qca case */
 qcvalp = dcep->dceu2.dce_qcvalp;

 /* get the qc statement */
 stp = qcvalp->qcstp; 
 /* evaluate the rhs */
 rhsx = stp->st.sqca->qcrhsx; 
 lhsx = stp->st.sqca->qclhsx;
 /* this converts rhs if needed and makes right lhs width */
 xsp = __eval_assign_rhsexpr(rhsx, lhsx);

 if (__debug_flg && __ev_tracing)
  {
   char s1[RECLEN], s2[RECLEN];

   __tr_msg(
    ":: quasi-continuous rhs at %s changed in %s now %s - assign/force to %s\n",
    __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt),
    __msg2_blditree(__xs2, __inst_ptr), __to_timstr(s1, &__simtime),
    __msgexpr_tostr(s2, stp->st.sqca->qclhsx));
  }
 /* if reg form and not concatenate, easy just use changed qcval */
 if (stp->st.sqca->regform)
  {
   if (lhsx->optyp != LCB)
    {
     np = lhsx->lu.sy->el.enp;
     do_qc2_regstore(np, qcvalp, xsp);
    }
   else
    {
     /* concatenate required finding qcval for each */
     for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x)
      {
       lhsx2 = catndp->lu.x;
       /* each lhs concatenate component may be xmr */
       if (lhsx2->optyp == GLBREF)
        { __xmrpush_refgrp_to_targ(lhsx2->ru.grp); nd_itpop2 = TRUE; }
       else nd_itpop2 = FALSE;

       np = lhsx2->lu.sy->el.enp;
       if (stp->st.sqca->qcatyp == ASSIGN)
        qcvalp = &(np->nu2.qcval[2*__inum + 1]);
       else qcvalp = &(np->nu2.qcval[2*__inum]);
       do_qc2_regstore(np, qcvalp, xsp);
       if (nd_itpop2) __pop_itstk();
      }
    }
  } 
 else
  {
   /* for every lhs concat part and bit do the assign */
   if (lhsx->optyp != LCB) assign_alllhs_bits(lhsx, xsp);
   else
    {
     for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x)
      assign_alllhs_bits(catndp->lu.x, xsp);
    }
  }
 __pop_xstk();
 if (nd_itpop) __pop_itstk();
}

/*
 * assign to all qcval bits of passed lhs expr (know not concat)
 */
static void assign_alllhs_bits(struct expr_t *lhsx, struct xstk_t *xsp)
{
 register int32 bi, ibase;
 int32 biti, bitj, nd_itpop;
 struct net_t *np;
 struct itree_t *lhsitp;
 struct qcval_t *frc_qcp;

 nd_itpop = FALSE;
 __get_qc_wirrng(lhsx, &np, &biti, &bitj, &lhsitp); 
 if (lhsitp != NULL) { nd_itpop = TRUE; __push_itstk(lhsitp); }
 /* must run this in lhs itree loc. */
 ibase = np->nwid*__inum;
 for (bi = biti; bi >= bitj; bi--, frc_qcp--)
  {
   frc_qcp = &(np->nu2.qcval[ibase + bi]);
   do_qc2_wirestore(np, frc_qcp, xsp);
  }
 if (nd_itpop) __pop_itstk();
}

/*
 * execute a qc assign under control of qc val record into net np
 *
 * this must be called from ref. stmt (not xmr lhs) itree location
 * notice assign is either entire reg or bit of wire 
 */
extern void __do_qc_store(struct net_t *np, struct qcval_t *qcvalp, int32 is_reg)
{ 
 struct xstk_t *xsp;
 struct expr_t *rhsx;

 rhsx = qcvalp->qcstp->st.sqca->qcrhsx; 
 /* this converts rhs if needed and makes lhs right width */
 xsp = __eval_assign_rhsexpr(rhsx, qcvalp->qcstp->st.sqca->qclhsx);
 if (is_reg) do_qc2_regstore(np, qcvalp, xsp);
 else do_qc2_wirestore(np, qcvalp, xsp);
 __pop_xstk();
}

/*
 * store q qc value with rhs in xsp for reg entire wire only
 * ths must run in assign/force stmt context since get rhs
 */
static void do_qc2_regstore(struct net_t *np, struct qcval_t *qcvalp,
 struct xstk_t *xsp)
{
 int32 nd_itpop, nd_xpop;
 struct xstk_t *xsp2;

 /* know lhs always entire reg but rhs may need select out */ 
 /* rhsbi field is low bit of section from lhs concatenate if needed */
 if (qcvalp->qcrhsbi != -1)
  {
   push_xstk_(xsp2, np->nwid);
   __rhspsel(xsp2->ap, xsp->ap, qcvalp->qcrhsbi, np->nwid);
   __rhspsel(xsp2->bp, xsp->bp, qcvalp->qcrhsbi, np->nwid);
   nd_xpop = TRUE;; 
  }
 else { xsp2 = xsp; nd_xpop = FALSE; }

 if (qcvalp->lhsitp != NULL)
  { nd_itpop = TRUE; __push_itstk(qcvalp->lhsitp); }
 else nd_itpop = FALSE;

 /* emit debug tracing message if needed */
 if (__debug_flg && __ev_tracing)
  {
   __tr_msg("   QC immediate store of %s into reg %s\n",
    __xregab_tostr(__xs2, xsp2->ap, xsp2->bp, xsp2->xslen,
    qcvalp->qcstp->st.sqca->qcrhsx), np->nsym->synam);
  }
 __chg_st_val(np, xsp2->ap, xsp2->bp);

 if (nd_xpop) __pop_xstk();
 if (nd_itpop) __pop_itstk();
}

/*
 * store qc value with rhs in xsp for wire either scalar or 1 bit select only
 */
static void do_qc2_wirestore(struct net_t *np, struct qcval_t *qcvalp,
 struct xstk_t *xsp)
{
 int32 ind, nd_itpop, nd_xpop;
 byte *sbp;
 struct xstk_t *xsp2, *xsp3;

 /* DBG remove ---
 __dbg_msg("   +++ before QC cat rhs value %s rhsbi=%d, lhsbi=%d\n",
  __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE),
  qcvalp->qcrhsbi, qcvalp->qclhsbi);
 --- */

 /* 2 cases that require selecting a bit from evaled rhs xsp */
 /* rhs bi or lhs bi not -1 because lhs concatenate */
 if (qcvalp->qcrhsbi != -1 || qcvalp->qclhsbi != -1)
  {
   ind = ((qcvalp->qcrhsbi == -1) ? 0 : qcvalp->qcrhsbi);
   push_xstk_(xsp2, 1);
   xsp2->ap[0] = rhsbsel_(xsp->ap, ind);
   xsp2->bp[0] = rhsbsel_(xsp->bp, ind);
   nd_xpop = TRUE;
  }
 else { xsp2 = xsp; nd_xpop = FALSE; }

 /* now know lot bit of xsp is value to assign */
 /* DBG remove ---
 if (__debug_flg)
  {
   __dbg_msg("   +++ after QC cat rhs value %s rhsbi=%d, lhsbi=%d\n",
    __regab_tostr(__xs, xsp2->ap, xsp2->bp, xsp2->xslen, BHEX, FALSE),
    qcvalp->qcrhsbi, qcvalp->qclhsbi);
  } 
 --- */ 

 if (qcvalp->lhsitp != NULL)
  { nd_itpop = TRUE; __push_itstk(qcvalp->lhsitp); }
 else nd_itpop = FALSE;

 /* emit debug tracing message if needed */
 if (__debug_flg && __ev_tracing)
  {
   if (qcvalp->qclhsbi == -1) strcpy(__xs, "");
   else sprintf(__xs, "[%d]", qcvalp->qclhsbi);
   __tr_msg("   QC immediate store of %s into wire %s%s\n",
    __xregab_tostr(__xs2, xsp2->ap, xsp2->bp, xsp->xslen,
    qcvalp->qcstp->st.sqca->qcrhsx), np->nsym->synam, __xs);
  }

 /* quasi-continuous assign to strength wire always strong */
 if (np->n_stren)
  {
   push_xstk_(xsp3, 4);
   sbp = (byte *) xsp3->ap;
   /* LOOKATME could simpify since know only 1 bit */
   __st_standval(sbp, xsp2, ST_STRVAL);
   /* notice if vector for wire know lhs bi always non zero */

   /* SJM 11/14/00 slightly better to just pass a part as sbp */
   /* notice if vector for wire know lhs bi always non zero */
   if (np->n_isavec) __chg_st_bit(np, qcvalp->qclhsbi, (word32) sbp[0], 0L); 

   /* AIV 07/09/04 - was calling macro but macro never checked record nchg */
   /* notice for non stren case was calling chg st as needed */ 
   else __chg_st_val(np, (word32 *) sbp, 0L); 
   __pop_xstk();
  }
 else 
  {
   if (np->n_isavec)
    __chg_st_bit(np, qcvalp->qclhsbi, xsp2->ap[0], xsp2->bp[0]); 
   else __chg_st_val(np, xsp2->ap, xsp2->bp);
  }
 if (nd_xpop) __pop_xstk();
 if (nd_itpop) __pop_itstk();
}

/*
 * ROUTINES TO MOVE ITREE LOCATION (PUSH/POP ITREE STACK) 
 */

/*
 * routine to push something onto itstk when there is no itree context
 */
extern void __push_wrkitstk(struct mod_t *mdp, int32 winum)
{
 struct itree_t *tmpitp;
 struct inst_t *tmpip;

 /* DBG REMOVE --- 
 if (__inst_mod != NULL) 
  __dbg_msg("### pushing inst mod (before) = %s\n", __inst_mod->msym->synam);
 --- */

 if (__tmpitp_freelst == NULL)
  {
   tmpitp = (struct itree_t *) __my_malloc(sizeof(struct itree_t)); 
   tmpip = (struct inst_t *) __my_malloc(sizeof(struct inst_t));
  }
 else
  {
   tmpitp = __tmpitp_freelst;
   tmpip = __tmpip_freelst;
   __tmpitp_freelst = __tmpitp_freelst->up_it;
   __tmpip_freelst = (struct inst_t *) __tmpip_freelst->imsym;
  }

 tmpitp->itinum = winum;
 tmpitp->up_it = NULL;
 tmpitp->in_its = NULL;
 tmpitp->itip = tmpip;
 tmpip->imsym = mdp->msym;
 /* indicates dummy work itp */
 tmpip->isym = NULL;
 __push_itstk(tmpitp);
 /* DBG REMOVE ---
 __dbg_msg("### pushing inst mod (after) = %s\n", __inst_mod->msym->synam);
 --- */
}

/*
 * routine to pop something from itstk when there is no itree context
 * must pair with push_wrkistk
 */
extern void __pop_wrkitstk(void)
{
 struct itree_t *tmpitp;
 struct inst_t *tmpip;

 tmpitp = __inst_ptr;
 /* REMOVE ---
 __dbg_msg("### popping inst mod (before) = %s\n", __inst_mod->msym->synam);
 --- */
 tmpip = tmpitp->itip;
 __pop_itstk();
 /* DBG REMOVE ---
 if (__inst_mod != NULL)
  __dbg_msg("### popping inst mod (after) = %s\n", __inst_mod->msym->synam);
 -- */
 tmpip = tmpitp->itip;
 tmpitp->up_it = __tmpitp_freelst;
 __tmpitp_freelst = tmpitp;
 tmpip->imsym = (struct sy_t *) __tmpitp_freelst;
 __tmpip_freelst = tmpip;
}

/*
 * push new current itp and set current inst num
 * LOOKATME - maybe these could be macros (see cvmacros.h)
 * SJM 04/20/01 - this now used instead of macros since do not slow down much
 */
/* ---- */
extern void __push_itstk(struct itree_t *itp)
{
 /* DBG remove --
 if (itp == NULL) __misc_terr(__FILE__, __LINE__);
 if (__itspi + 1 >= MAXITDPTH) __misc_terr(__FILE__, __LINE__);
 --- */
 __itstk[++__itspi] = __inst_ptr = itp;
 /* DBG remove --
 if (__inst_ptr == NULL) __misc_terr(__FILE__, __LINE__);
 --- */
 __inst_mod = __inst_ptr->itip->imsym->el.emdp;
 __inum = __inst_ptr->itinum;
 /* DBG REMOVE ---
 if (__debug_flg)
  {
   if (__inst_ptr->itip == NULL) strcpy(__xs, "**no itip (dummy)**");
   else if (__inst_ptr->itip->isym == NULL) strcpy(__xs, "*no ip (wrkitp?)");
   else __msg2_blditree(__xs, __inst_ptr);
   __dbg_msg(
    "+++ pushing itree stack to height %d - inum %d, inst %s mod %s\n",
    __itspi + 1, __inum, __xs, __inst_mod->msym->synam);
  }
 -- */
 /* DBG remove - this can happen but need to study example */
 /* if (__itspi > 6) __misc_terr(__FILE__, __LINE__); */
}
/* --- */

/*
 * pop cur. itp - module itree place
 *
 * this is for debugging normally use this macro
 */
/* --- */
extern void __pop_itstk(void)
{
 /* DBG remove ---
 if (__itspi < 0) __misc_terr(__FILE__, __LINE__);
  -- */
 if (--__itspi < 0)
  {
   __inst_ptr = NULL;
   __inst_mod = NULL;
   __inum = 0xffffffff;
  }
 else 
  {
   __inst_ptr = __itstk[__itspi];
 
   /* DBG remove ---
   if (__inst_ptr == NULL || __inum == 0xffffffff)
    __misc_terr(__FILE__, __LINE__);
   -- */
   __inst_mod = __inst_ptr->itip->imsym->el.emdp;
   __inum = __inst_ptr->itinum;
  }

 /* DBG remove ---
 if (__debug_flg)
  __dbg_msg("+++ popping itree stack to height %d\n", __itspi + 1);
 --- */
}
/* --- */

/*
 * push an itree stack entry that is the global inst. place
 * current itp place is place xmr appears - push target after tracing
 *
 * this pushes target given reference xmr location
 * by here all selects of instance arrays have been resolved to right symbol
 */
extern void __xmrpush_refgrp_to_targ(struct gref_t *grp)
{
 struct itree_t *itp;

 /* rooted case */
 if (grp->is_rooted)
  {
   __push_itstk(grp->targu.targitp);
   /* DBG remove ---
   if (__debug_flg)
    __dbg_msg("== pushing rooted global %s target (itree=%s)\n",
     grp->gnam, __msg2_blditree(__xs, __inst_ptr));
   --- */  
   return;
  }
 /* upward relative case - requires linear list search */
 if (grp->upwards_rel)
  {
   __push_itstk(grp->targu.uprel_itps[__inum]);
   /* --- DBG remove */
   if (__debug_flg)
    __dbg_msg("== pushing upward relative global %s target (itree=%s)\n",
     grp->gnam, __msg2_blditree(__xs, __inst_ptr));
   /* -- */
   return;
  }
 /* normal downward instance path case */
 itp = __find_unrt_targitp(grp, __inst_ptr, 0);
 __push_itstk(itp);

 /* DBG remove --- 
 if (__debug_flg)
  __dbg_msg("== pushing downward global %s target (itree=%s) \n",
    grp->gnam, __msg2_blditree(__xs, __inst_ptr));
 --- */
} 

/*
 * return target itp given current itp and downward xmr (gref)
 *
 * by here all selects of instance arrays resolved to right inst symbol
 */
extern struct itree_t *__find_unrt_targitp(struct gref_t *grp,
 register struct itree_t *itp, int32 startii)
{
 register int32 gri;
 int32 ii; 
 byte *bp1, *bp2;
 struct inst_t *ip;
 struct mod_t *imdp;

 /* notice first is inst. in module gref appears in */
 imdp = itp->itip->imsym->el.emdp;
 for (gri = startii;;)
  {
   if (grp->grxcmps[gri] != NULL) ip = __get_gref_giarr_ip(grp, gri, itp); 
   else ip = grp->grcmps[gri]->el.eip;

   /* DBG remove RELEASE REMOVEME -- */
   if (imdp->minsts == NULL) __misc_terr(__FILE__, __LINE__);
   /* -- */

   /* making use of c pointer subtraction correction object size here */
   /* changing to byte ptr because not sure of c ptr size object rules */
   bp1 = (byte *) ip;
   bp2 = (byte *) imdp->minsts;
   /* DBG remove ---
   if (bp2 > bp1)
    {
     __dbg_msg("== global %s comp=%d mod=%s inst=%s itp=%s(%s).\n",
      grp->gnam, gri, imdp->msym->synam, ip->isym->synam, 
      itp->itip->isym->synam, itp->itip->imsym->synam);
     __arg_terr(__FILE__, __LINE__);
    }
   --- */
   ii = (bp1 - bp2)/sizeof(struct inst_t);
   itp = &(itp->in_its[ii]);
   /* DBG remove ---
   if (__debug_flg)
    {
     __dbg_msg("== global %s comp=%d mod=%s inst=%s num=%d, itp=%s(%s).\n",
      grp->gnam, gri, imdp->msym->synam, ip->isym->synam, ii, 
      itp->itip->isym->synam, itp->itip->imsym->synam);
    }
   --- */
   if (++gri > grp->last_gri) break;
   imdp = itp->itip->imsym->el.emdp;
  }
 return(itp);
}

/*
 * access a gref inst array index and return the right expanded instance
 *
 * assumes grxcmps folded to 32 bit non x/z constants (maybe IS)
 * LOOKATME - this always checks ranges - maybe should have separate routine
 */
extern struct inst_t *__get_gref_giarr_ip(struct gref_t *grp, int32 gri,
 struct itree_t *itp) 
{
 int32 indx, ii2, ii3;      
 byte *bp1, *bp2;
 struct sy_t *syp;
 struct xstk_t *xsp;
 struct giarr_t *giap;
 struct inst_t *ip;
 struct mod_t *imdp;

 syp = grp->grcmps[gri];
 /* DBG remove --- */
 if (!syp->sy_giabase) __arg_terr(__FILE__, __LINE__);
 /* --- */
 __push_itstk(itp);
 /* evaluate - this just loads constant but may be IS form constant */
 /* know checked so will be non x/z 32 bit value or will not get here */
 xsp = __eval_xpr(grp->grxcmps[gri]);
 indx = (int32) xsp->ap[0];
 __pop_xstk();
 __pop_itstk();
 imdp = itp->itip->imsym->el.emdp;
 /* syp points to first instance of expanded */
 ip = syp->el.eip;
 bp1 = (byte *) ip;
 bp2 = (byte *) imdp->minsts;
 ii2 = (bp1 - bp2)/sizeof(struct inst_t);
 giap = imdp->miarr[ii2]; 
 if (giap->gia1 > giap->gia2)
  {
   if (indx > giap->gia1 || indx < giap->gia2) goto bad_ref; 
   else ii3 = giap->gia_bi + (giap->gia1 - indx);
  }
 else
  {
   if (indx < giap->gia1 || indx > giap->gia2) goto bad_ref; 
   ii3 = giap->gia_bi + (indx - giap->gia1);
  }
done:
 ip = &(imdp->minsts[ii3]);
 return(ip);

bad_ref:
 __gferr(680, grp->grfnam_ind, grp->grflin_cnt,
  "hierarchical reference %s of %s index %d (comp. %d) out of range [%d:%d]",
  grp->gnam, syp->synam, indx, gri + 1, giap->gia1, giap->gia2); 
 ii3 = giap->gia_bi;
 goto done;
}

/*
 * routine to push ref. itstk place onto inst. stack when at define place
 * propagation caller will pop back to target place - only called if xmr 
 * notice if this is called, caller will need to pop
 */
extern int32 __match_push_targ_to_ref(word32 xmrtyp, struct gref_t *grp)
{
 register int32 i, gri;
 struct itree_t *itp;
 struct inst_t *ip;

 switch ((byte) xmrtyp) {
  case XNP_DOWNXMR:
   /* since current itp is target - back up to reference itp */
   itp = __inst_ptr;
   /* SJM 04/17/03 - must move up and check that each one on way up */
   /* matches inst above since down can be multiply instanciated */
   for (gri = grp->last_gri; gri >= 0; gri--)
    {
     /* need to use inst array select expr which is itree loc. specific */
     if (grp->grxcmps[gri] != NULL) ip = __get_gref_giarr_ip(grp, gri, itp); 
     else ip = grp->grcmps[gri]->el.eip;

     if (ip != itp->itip)
      {
       /* DBG remove --- */ 
       if (__debug_flg)
        {
         __dbg_msg(
          "== down glb drive/load of %s (in %s) mismatch instance %s (comp %s != %s)\n",
          grp->gnam, grp->gin_mdp->msym->synam, __msg2_blditree(__xs,
          __inst_ptr), ip->isym->synam, itp->itip->isym->synam);
        }
       /* --- */
       return(FALSE);
      }
     /* DBG remove --- */
     if (itp->up_it == NULL) __misc_terr(__FILE__, __LINE__);
     /* --- */
     /* DBG remove --- */ 
     if (__debug_flg)
      {
       __dbg_msg(
        "== down glb %s drive/load move from decl in targ %s to ref in %s\n",
        grp->gnam, __msg2_blditree(__xs, itp),
        __msg2_blditree(__xs2, itp->up_it)); 
      }
     /* --- */
     itp = itp->up_it;
    }
   break;
  case XNP_RTXMR:
   /* rooted not part of np union field - never called uses filter fld */ 
   __case_terr(__FILE__, __LINE__);
   return(FALSE);
  case XNP_UPXMR:
   /* SJM 09/14/00 - must search for current target place to move back */
   /* to location referenced in */
   /* SJM 07/01/03 - index was one too many */
   for (i = 0; i < grp->gin_mdp->flatinum; i++)
    {
     if (__inst_ptr == grp->targu.uprel_itps[i])
      {
       /* DBG remove --- */ 
       if (__debug_flg)
        {
         __dbg_msg(
          "== up rel glb %s drive/load move from decl in targ %s to ref in %s\n",
          grp->gnam, __msg2_blditree(__xs, __inst_ptr),
          __msg2_blditree(__xs2, grp->gin_mdp->moditps[i]));
        }
       /* --- */
       goto got_itp;
      }
    } 
   /* SJM 05/23/03 - possible for down declared in upwards rel to all go */ 
   /* to only some instances so this declared in may not exist */
   if (__debug_flg)
    {
     __dbg_msg(
      "== uprel glb %s drive/load of %s (in %s) no matching uprel inst\n",
       grp->gnam, grp->gin_mdp->msym->synam, __msg2_blditree(__xs,
       __inst_ptr));
    }
   return(FALSE);
got_itp:
   itp = grp->gin_mdp->moditps[i];
   break;
  default: __case_terr(__FILE__, __LINE__); return(TRUE);
 } 
 __push_itstk(itp);
 return(TRUE);
}

/*
 * CONTINUOUS ASSIGNMENT ROUTINES
 */

/*
 * execute concat assign part of non strength lhs wire continuous assign
 *
 * know rhs non strength and no component of lhs needs strength
 * i.e. continuous assign does not drive strength (strong0, strong1)
 * caller must pop stack on return
 */
extern void __exec_ca_concat(struct expr_t *xlhs, register word32 *ap, 
 register word32 *bp, int32 must_schedule)
{
 register struct expr_t *catndp;
 register int32 catxlen;
 register struct xstk_t *catxsp;
 int32 bi1;
 struct expr_t *catlhsx;

 /* do lhs concatenate assigns from left to right */
 for (catndp = xlhs->ru.x; catndp != NULL; catndp = catndp->ru.x)
  {
   catlhsx = catndp->lu.x;
   catxlen = catlhsx->szu.xclen;
   /* catndp comma node is dist. to low bit, bi1 is low bit of rhs psel */
   bi1 = catndp->szu.xclen - catxlen;

   /* DBG remove ---
   if (__debug_flg)
    __dbg_msg(
     "+++lhs: total concat wid=%u, low index=%d, wid=%u, remaining wid=%u\n",
     xlhs->szu.xclen, bi1, catxlen, catndp->szu.xclen);
   --- */

   /* select current pos. right width piece from rhs and put on tos reg */
   /* notice assignment always same width */
   push_xstk_(catxsp, catxlen);
   if (catxlen == 1)
    { catxsp->ap[0] = rhsbsel_(ap, bi1); catxsp->bp[0] = rhsbsel_(bp, bi1); } 
   else
    {
     __rhspsel(catxsp->ap, ap, bi1, catxlen);
     __rhspsel(catxsp->bp, bp, bi1, catxlen);
    }
   /* also nested lhs concatenates illegal - will never appear */
   /* notice this is part of immediate assign must not inc assign counter */
   /* here know every catlhsx wire must be strength */
   __exec_conta_assign(catlhsx, catxsp->ap, catxsp->bp, must_schedule);
   __pop_xstk();
  }
}

/*
 * execute assign part of strength lhs wire continuous assign concat
 *
 * know rhs has strength - maybe only (strong0, strong1) because at least
 * one lhs wire needs strength - if any lhs concat component non stren
 * that section converted back to value here
 * caller must pop stack on return
 */
extern void __stren_exec_ca_concat(struct expr_t *xlhs, byte *sbp,
 int32 must_schedule)
{
 register int32 catxlen;
 register struct expr_t *catndp;
 register int32 sbi, sbi2;
 int32 bi1;
 byte *sbp2;
 struct expr_t *catlhsx;
 struct xstk_t *catxsp, *xsp;

 /* do lhs concatenate assigns from left to right */
 for (catndp = xlhs->ru.x; catndp != NULL; catndp = catndp->ru.x)
  {
   catlhsx = catndp->lu.x;
   catxlen = catlhsx->szu.xclen;
   /* bi1 is low bit of rhs evaluted value part select */
   bi1 = catndp->szu.xclen - catxlen; 

   /* --- DBG remove
   if (__debug_flg)
    __dbg_msg(
     "+++lhs proc: total cat wid=%u, low index=%d, wid=%u, remaining wid=%u\n",
     xlhs->szu.xclen, bi1, catxlen, catndp->szu.xclen);
   --- */

   /* select current pos. right width piece from rhs and put on tos reg */
   /* notice assignment always same width */
   push_xstk_(catxsp, 4*catxlen);
   sbp2 = (byte *) catxsp->ap;
   if (catxlen == 1) sbp2[0] = sbp[bi1];
   else
    {
     /* sbp2 and sbi2 is section selected from concatenate rhs */
     for (sbi = bi1, sbi2 = 0; sbi2 < catxlen; sbi++, sbi2++)
      sbp2[sbi2] = sbp[sbi];
    }
   if (!catlhsx->x_stren)
    {
     push_xstk_(xsp, catxlen);
     __rem_stren(xsp->ap, xsp->bp, sbp2, catxlen); 
     /* DBG remove */
     if (__debug_flg && __ev_tracing)
      __tr_msg("+++ strength concat assign - needed to remove strength?");
     /* --- */
     __exec_conta_assign(catlhsx, xsp->ap, xsp->bp, must_schedule);
     __pop_xstk();
    }
   else
    {
     /* also nested lhs concatenates illegal - will never appear */
     /* this is part of immediate assign must not inc assign counter */
     __exec_conta_assign(catlhsx, catxsp->ap, catxsp->bp, must_schedule);
    }
   __pop_xstk();
  }
}

/*
 * actually execute the continuous assign store 
 *
 * concatenates removed before here
 * the various store routines assume xsp stren consistent with wire type
 * i.e. this routine handles both stren and non stren
 * know xlhs is same width as ap/bp that is new rhs value
 *
 * key here is that mutiple driver or supply0/1 or tri0/1 or tran chan nets
 * never just assigned here without fi>1 eval. all drivers evaluated 
 *
 * notice that if this is called from force mechanism caller must
 * turn off any bits before or will not really change value
 */
extern void __exec_conta_assign(struct expr_t *xlhs, register word32 *ap,
 register word32 *bp, int32 must_schedule)
{
 int32 nd_itpop, ri1, ri2;
 struct expr_t *idndp, *ndx1;
 struct net_t *np;
 struct gref_t *grp;

 /* notice if forced still must schedule since force maybe off later */
 if (must_schedule) { sched_conta_assign(xlhs, ap, bp); return; }

 nd_itpop = FALSE;
 switch ((byte) xlhs->optyp) {
  case GLBREF:
   grp = xlhs->ru.grp;    
   __xmrpush_refgrp_to_targ(grp);
   nd_itpop = TRUE;
   /* FALLTHRU */
  case ID:
   np = xlhs->lu.sy->el.enp;
   /* this add the changed wire to nchglst if needed */
   if (np->frc_assgn_allocated)
    {
     /* return F if all of wire forced, nothing to do */ 
     if (!__correct_forced_newwireval(np, ap, bp))
      { if (nd_itpop) __pop_itstk(); return; }
    }
   /* SJM 03/15/01 - change to fields in net record */
   if (np->nchg_nd_chgstore) __chg_st_val(np, ap, bp);
   else __st_val(np, ap, bp);

   if (nd_itpop) __pop_itstk();
   break;
  case LSB:
   /* for now first determine if array index */
   idndp = xlhs->lu.x;
   ndx1 = xlhs->ru.x;
   np = idndp->lu.sy->el.enp;
   /* the 1 bit is forced nothing to do else normal assign */
   if (np->frc_assgn_allocated
    && __forced_inhibit_bitassign(np, idndp, ndx1)) return;
   __assign_to_bit(np, idndp, ndx1, ap, bp);
   break;
  case PARTSEL:
   idndp = xlhs->lu.x;
   np = idndp->lu.sy->el.enp;
   ri1 = (int32) __contab[xlhs->ru.x->lu.x->ru.xvi];
   ri2 = (int32) __contab[xlhs->ru.x->ru.x->ru.xvi];

   /* if all bits of lhs part select range forced, do not do assign */
   /* this also update ap and bp to include forced values */
   if (np->frc_assgn_allocated
    && !forced_assign_to_psel(idndp, ri1, ri2, np, ap, bp)) return;

   /* know part select here in range and converted to h:0 index form */ 
   __assign_to_psel(idndp, ri1, ri2, np, ap, bp);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 /* DBG remove --- */
 if (__debug_flg && __ev_tracing) trace_conta_assign(xlhs, ap, bp);
 /* --- */
}

/*
 * trace conta assign
 */
static void trace_conta_assign(struct expr_t *xlhs, word32 *ap, word32 *bp)
{
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 if (xlhs->x_stren) __st_regab_tostr(s1, (byte *) ap, xlhs->szu.xclen);
 else __regab_tostr(s1, ap, bp, xlhs->szu.xclen, BHEX, FALSE);
 /* SJM 08/24/03 - printing chg state wrong since this is not chg */
 __tr_msg("## declarative assign to %s (itree=%s) value %s\n",
  __msgexpr_tostr(s2, xlhs), __msg2_blditree(s3, __inst_ptr), s1);
}

/*
 * correct an entire wire about to be assigned value for forced bits
 * if no assign needed return F else T
 * must be called from correct itree location
 * know has qcval or will not get here
 * this does not leave anything on stack 
 */
extern int32 __correct_forced_newwireval(struct net_t *np, word32 *ap, word32 *bp) 
{
 register int32 bi, ibase, wi;
 int32 nd_assign, wlen;
 byte *sbp, *sbp2;
 struct xstk_t *xsp, *xsp2;
  
 /* if scalar or 1 bit only, no correction but maybe skip assign */ 
 if (!np->n_isavec)
  {
   if (np->nu2.qcval[__inum].qc_active) return(FALSE);
   return(TRUE);
  }
 ibase = __inum*np->nwid;
 if (np->n_stren)
  {
   sbp = (byte *) ap;
   get_stwire_addr_(sbp2, np);
   /* trick is to replace forced bits so new assign is same as forced val */
   for (nd_assign = FALSE, bi = 0; bi < np->nwid; bi++)
    {
     /* some bits not forced - so need assign */
     if (np->nu2.qcval[ibase + bi].qc_active) sbp[bi] = sbp2[bi];
     else nd_assign = TRUE;
    }
   return(nd_assign);
  }

 push_xstk_(xsp, np->nwid);
 __bld_forcedbits_mask(xsp->ap, np);
 zero_allbits_(xsp->bp, np->nwid); 
 
 /* if all bits forced nothing to do */
 if (__vval_is1(xsp->ap, np->nwid)) { __pop_xstk(); return(FALSE); }
 /* if no bits forced, just assign ap */
 if (vval_is0_(xsp->ap, np->nwid)) { __pop_xstk(); return(TRUE); }

 /* only load wire if some bits forced */
 push_xstk_(xsp2, np->nwid);
 __ld_wire_val(xsp2->ap, xsp2->bp, np);

 /* take new value and merge in some forced bits */
 wlen = wlen_(np->nwid);    
 for (wi = 0; wi < wlen; wi++)
  {
   /* remove forced bits from new value */
   ap[wi] &= ~(xsp->ap[wi]);
   bp[wi] &= ~(xsp->ap[wi]);
   /* remove non forced bits from new wire */
   xsp2->ap[wi] &= (xsp->ap[wi]);
   xsp2->bp[wi] &= (xsp->ap[wi]);
   /* combine old maybe forced bits into new value - so will be same */  
   ap[wi] |= (xsp2->ap[wi]);
   bp[wi] |= (xsp2->ap[wi]);
  }
 __pop_xstk();
 __pop_xstk();
 return(TRUE);
}

/*
 * convert the per bit forced table to a per bit forced vector 
 */
extern void __bld_forcedbits_mask(word32 *ap, struct net_t *np)
{
 register int32 bi;
 int32 ibase;

 zero_allbits_(ap, np->nwid); 
 /* build mask in ap part with 1 for every forced bit */ 
 ibase = __inum*np->nwid;
 for (bi = 0; bi < np->nwid; bi++)
  {
   if (np->nu2.qcval[ibase + bi].qc_active) __lhsbsel(ap, bi, 1L);
  }
}

/*
 * schedule assignment of all bits from wire that is lhs of conta
 * processing conta assign event - this delays wire value change
 */
static void sched_conta_assign(struct expr_t *xlhs, register word32 *ap, 
 register word32 *bp)
{
 int32 nd_itpop;
 byte *sbp;
 struct gref_t *grp;
 struct net_t *np;

 nd_itpop = FALSE;
 switch ((byte) xlhs->optyp) {
  case GLBREF:
   grp = xlhs->ru.grp;    
   __xmrpush_refgrp_to_targ(grp);
   nd_itpop = TRUE;
   /* FALLTHRU */
  case ID:
   np = xlhs->lu.sy->el.enp;
   if (np->nu.rngdwir->n_delrep == DT_PTHDST)
    {
     if (np->n_stren)
      {
       sbp = (byte *) ap;
       __pth_stren_schd_allofwire(np, sbp, xlhs->szu.xclen);
      }
     else __pth_schd_allofwire(np, ap, bp, xlhs->szu.xclen);
    }
   else
    {
     if (np->n_stren)
      {
       sbp = (byte *) ap;
       __wdel_stren_schd_allofwire(np, sbp, xlhs->szu.xclen);
      }
     else __wdel_schd_allofwire(np, ap, bp, xlhs->szu.xclen);
    }
   if (nd_itpop) __pop_itstk();
   break;
  case LSB:
   np = xlhs->lu.x->lu.sy->el.enp;
   schedassign_to_bit(np, xlhs->lu.x, xlhs->ru.x, ap, bp);
   break;
  case PARTSEL:
   schedassign_to_psel(xlhs, ap, bp);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * convert conta where lhs is strength wire to strength form
 * trick here is that conta always eats strength and maybe generates its own
 */
extern void __rem_stren(word32 *ap, word32 *bp, byte *sbp, int32 blen)
{
 register int32 bi, bi2, wi;
 int32 wlen, ubits;   
 word32 aw, bw;

 wlen = wlen_(blen);
 if ((ubits = ubits_(blen)) == 0) ubits = WBITS;
 for (bi2 = blen - 1, wi = wlen - 1; wi >= 0; wi--)
  { 
   aw = bw = 0L;
   for (bi = ubits - 1; bi >= 0; bi--, bi2--)   
    {
     aw |= ((sbp[bi2] & 1L) << bi);
     bw |= (((sbp[bi2] & 2L) >> 1) << bi);
    }
   ap[wi] = aw;
   bp[wi] = bw;
   ubits = WBITS;
  }
} 

/*
 * INTERMEDIATE WIRE SCHEDULING ROUTINES
 */

/*
 * schedule an entire non strength wire with delay
 * this always requires z extension
 *
 * blen here is real not 4x too big
 */
extern void __pth_schd_allofwire(struct net_t *np, register word32 *ap,
 register word32 *bp, int32 blen)
{
 register int32 bi;
 register word32 aval, bval;
 word32 av, bv;

 if (!np->n_isavec)
  {
   aval = ap[0] | (bp[0] << 1);
   /* must load wire value here in case packed */
   __ld_wire_val(&av, &bv, np);
   av |= (bv << 1);
   /* must pass index of 0, since only bit is 0th here */
   schd_1pthwirebit(np, 0, aval, av);
   return;
  }
 
 /* case 1: same or truncate */
 if (blen >= np->nwid)
  {
   for (bi = 0; bi < blen; bi++)
    {
     aval = rhsbsel_(ap, bi);
     bval = rhsbsel_(bp, bi);
     aval |= (bval << 1);
     __ld_bit(&av, &bv, np, bi); 
     schd_1pthwirebit(np, bi, aval, (av | (bv << 1)));
    }
   return;
  }
 /* case 2: widen rhs */
 for (bi = 0; bi < blen; bi++)
  {
   aval = rhsbsel_(ap, bi);
   bval = rhsbsel_(bp, bi);
   aval |= (bval << 1);
   __ld_bit(&av, &bv, np, bi); 
   schd_1pthwirebit(np, bi, aval, (av | (bv << 1)));
  }
 for (bi = blen; bi < np->nwid; bi++)
  {
   __ld_bit(&av, &bv, np, bi); 
   schd_1pthwirebit(np, bi, (word32) 2, (av | (bv << 1)));
  }
}

/*
 * schedule an entire non strength delay with wire
 * this always requires z extension
 * ap/bp is new value to assign
 *
 * blen here is real not 4x too big
 */
extern void __wdel_schd_allofwire(struct net_t *np, register word32 *ap,
 register word32 *bp, int32 blen)
{
 register int32 bi;
 register word32 aval, bval;
 word32 av, bv;

 if (!np->n_isavec)
  {
   aval = ap[0] | (bp[0] << 1);
   /* must load wire value here in case packed */
   __ld_wire_val(&av, &bv, np);
   av |= (bv << 1);
   /* must pass index of 0, since only bit is 0th here */
   __wdel_schd_1wirebit(np, 0, aval, av, FALSE);
   return;
  }
 
 /* case 1: same or truncate */
 if (blen >= np->nwid)
  {
   for (bi = 0; bi < blen; bi++)
    {
     aval = rhsbsel_(ap, bi);
     bval = rhsbsel_(bp, bi);
     aval |= (bval << 1);
     __ld_bit(&av, &bv, np, bi); 
     __wdel_schd_1wirebit(np, bi, aval, (av | (bv << 1)), FALSE);
    }
   return;
  }
 /* case 2: widen rhs */
 for (bi = 0; bi < blen; bi++)
  {
   aval = rhsbsel_(ap, bi);
   bval = rhsbsel_(bp, bi);
   aval |= (bval << 1);
   __ld_bit(&av, &bv, np, bi); 
   __wdel_schd_1wirebit(np, bi, aval, (av | (bv << 1)), FALSE);
  }
 for (bi = blen; bi < np->nwid; bi++)
  {
   __ld_bit(&av, &bv, np, bi); 
   __wdel_schd_1wirebit(np, bi, (word32) 2, (av | (bv << 1)), FALSE);
  }
}

/* table (copied in other places) to convert from cap to 6 bit stren */
/* 0 is impossible any error caught before here */
word32 __cap_to_stren[] = { 0, 0x24, 0x48, 0x90 }; 

/*
 * schedule an entire strength path delay wire
 * this always requires z extension
 *
 * blen here is real not 4x too big
 * this handles change from z to previous value plus cap. size
 * never see trireg here since cannot be path destination
 */
extern void __pth_stren_schd_allofwire(struct net_t *np, register byte *sbp,
 int32 sblen)
{
 register int32 bi;
 register byte *sbp2;

 /* DBG remove --- */
 if (np->ntyp == N_TRIREG) __misc_terr(__FILE__, __LINE__);
 /* --- */

 /* get strength wire address */
 get_stwire_addr_(sbp2, np);
 if (!np->n_isavec)
  {
   schd_1pthwirebit(np, 0, (word32) sbp[0], (word32) sbp2[0]);
   return;
  }

 /* case 1: same or truncate */
 if (sblen >= np->nwid)
  {
   for (bi = 0; bi < np->nwid; bi++)
    schd_1pthwirebit(np, bi, (word32) sbp[bi], (word32) sbp2[bi]);
   return;
  }
 /* case 2: widen rhs */
 for (bi = 0; bi < sblen; bi++)
  schd_1pthwirebit(np, bi, (word32) sbp[bi], (word32) sbp2[bi]);
 for (bi = sblen; bi < np->nwid; bi++)
  schd_1pthwirebit(np, bi, (word32) 2, (word32) sbp2[bi]); 
}

/*
 * schedule an entire strength wire with delay
 * this always requires z extension
 *
 * blen here is real not 4x too big
 * this handles change from z to previous value plus cap. size
 *
 * this is only place need to schedule trireg decay since trireg always fi>1
 * non fi>1 lhs select assigns only for non fi>1
 */
extern void __wdel_stren_schd_allofwire(struct net_t *np, register byte *sbp,
 int32 sblen)
{
 register int32 bi;
 register byte *sbp2;

 /* get strength wire address */
 get_stwire_addr_(sbp2, np);

 if (np->ntyp == N_TRIREG) 
  {
   int32 tr_decay;
   byte ntrival;
   word64 ndel;

   /* DBG remove ---
   if (np->nwid != sblen) __misc_terr(__FILE__, __LINE__);
   --- */
   for (bi = 0; bi < np->nwid; bi++)
    {
     if (sbp[bi] == ST_HIZ)
      {
       /* immediately assign the cap. size strength (same old value) */ 
       ntrival = (sbp2[bi] & 3) | __cap_to_stren[np->n_capsiz];
       if (ntrival != sbp2[bi])
        {
         sbp2[bi] = ntrival;
         /* since otherwise scheduling must indicate immed. net changed */
         /* also schedule decay */
         record_sel_nchg_(np, bi, bi);
        }
       /* schedule decay to cap. size x value change if z delay not 0 */
       /* trireg charge decay time is third to-z delay - 0 means never */
       /* decays to z */
       __new_gateval = 2;
       __old_gateval = sbp2[bi];
       /* index in get_del removes any indexing */
       __get_del(&ndel, np->nu.rngdwir->n_du, np->nu.rngdwir->n_delrep);
       if (ndel == 0ULL) continue;
       sbp[bi] = 3 | __cap_to_stren[np->n_capsiz];
       tr_decay = TRUE;
      }
     /* use normal to 0, 1, or x delay if not all z drivers of bit */ 
     else tr_decay = FALSE;
     __wdel_schd_1wirebit(np, bi, (word32) sbp[bi], (word32) sbp2[bi], tr_decay);
    }
   return; 
  }

 if (!np->n_isavec)
  {
   __wdel_schd_1wirebit(np, 0, (word32) sbp[0], (word32) sbp2[0], FALSE);
   return;
  }

 /* case 1: same or truncate */
 if (sblen >= np->nwid)
  {
   for (bi = 0; bi < np->nwid; bi++)
    __wdel_schd_1wirebit(np, bi, (word32) sbp[bi], (word32) sbp2[bi], FALSE);
   return;
  }
 /* case 2: widen rhs */
 for (bi = 0; bi < sblen; bi++)
  __wdel_schd_1wirebit(np, bi, (word32) sbp[bi], (word32) sbp2[bi], FALSE);
 for (bi = sblen; bi < np->nwid; bi++)
  __wdel_schd_1wirebit(np, bi, (word32) 2, (word32) sbp2[bi], FALSE); 
}

/*
 * ROUTINES TO SCHEDULE AND PROCESS WIRE EVENTS - ALWAYS BIT BY BIT
 */

/*
 * schedule a 1 bit wire change - know wire has delay to get here 
 *
 * no spike analysis because even though inertial modeling here 
 * wires do not switch
 *
 * this works for both strength 8 bit nval and oval and non strength
 * if scalar biti must be 0 (i.e. biti can not be -1)
 * nval is new value to schedule change to, old value is current wire value
 *
 * for trireg - if nval is weak previous, ndselval is 2 and nval for 
 * all z drivers is built cap. stren plus current value
 */
extern void __wdel_schd_1wirebit(register struct net_t *np, register int32 biti,
 register word32 nval, register word32 oval, int32 tr_decay)
{
 word64 ndel, schtim;
 i_tev_ndx tevpi, *itevpi;
 struct tev_t *tevp;
 struct rngdwir_t *dwirp; 

 if (__ev_tracing)
  { evtr_wdel_schd_1wirebit(np, biti, nval, oval, tr_decay); return; }

 dwirp = np->nu.rngdwir;
 itevpi = &(dwirp->wschd_pbtevs[np->nwid*__inum]); 
 tevpi = itevpi[biti];

 /* since always use last changed value, if last same as current */
 /* because gate style spike nothing to do since already right value */   
 if (tevpi == -1 && nval == oval) return;

 /* get delay and if path delay immediate (distributed longer) store */
 /* globals must be set for get del routine */
 __old_gateval = oval;
 /* notice new gateval is not value set but to-z needed for get del */
 __new_gateval = (tr_decay) ? 2 : nval;

 /* normal wire delay */
 /* notice old and new gate values always set before here */
 __get_del(&ndel, dwirp->n_du, dwirp->n_delrep);
 schtim = __simtime + ndel;

 if (tevpi == -1)
  {
   /* if nothing pending can just schedule */
   __schedule_1wev(np, biti, TE_WIRE, ndel, schtim, nval, itevpi, tr_decay);
   return;
  }
 tevp = &(__tevtab[tevpi]);
 /* DBG remove -- */
 if (tevp->tetyp != TE_WIRE) __misc_terr(__FILE__, __LINE__);
 /* --- */

 /* there is a pending unmatured event */
 /* case 1: real pulse (aka spike or glitch) just cancel */ 
 if (nval == oval)
  {
   /* cancel */
   __cancel_1wev(tevp);
   itevpi[biti] = -1;
  }
 /* this handles cancel too */
 else __reschedule_1wev(tevpi, nval, ndel, schtim, itevpi);
} 

/*
 * trace version sched 1 bit wire change - know wire has delay to get here 
 *
 * no spike analysis but normal inertial rescheduling
 * this works for both strength 8 bit nval and oval and non strength
 *
 * for trireg - if nval is weak previous, ndselval is 2 and nval for 
 * all z drivers is built cap. stren plus current value
 */
static void evtr_wdel_schd_1wirebit(register struct net_t *np,
 register int32 biti, register word32 nval, register word32 oval, int32 tr_decay)
{
 word32 is_stren;
 i_tev_ndx tevpi, *itevpi;
 word64 ndel, schtim;
 struct tev_t *tevp;
 struct rngdwir_t *dwirp; 
 char s1[RECLEN], vs1[10], vs2[10], vs3[10];

 dwirp = np->nu.rngdwir;
 itevpi = &(dwirp->wschd_pbtevs[np->nwid*__inum]); 
 tevpi = itevpi[biti];
 is_stren = np->n_stren;

 /* if no change and do not need schedule time for cancel, done */
 __tr_msg("-- delayed wire %s changed:\n",
  __to_evtrwnam(__xs, np, biti, biti, __inst_ptr));

 /* since always use last changed value, if last same as current */
 /* because gate style spike nothing to do since already right value */   
 if (tevpi == -1 && nval == oval)
  {
   __tr_msg(" NOPEND, NOCHG <OV=%s>\n", __to_vnam(vs1, is_stren, nval));
   return;
  }

 /* get delay and if path delay immediate (distributed longer) store */
 /* these globals must be set for get del routine */
 __new_gateval = (tr_decay) ? 2 : nval;
 __old_gateval = oval;
 /* normal wire delay */
 /* notice old and new gate values always set before here */
 __get_del(&ndel, dwirp->n_du, dwirp->n_delrep);
 schtim = __simtime + ndel; 

 if (tevpi == -1)
  {
   __tr_msg(" SCHD AT %s <OV=%s, NSV=%s>\n",
    __to_timstr(s1, &schtim), __to_vnam(vs1, is_stren, oval),
    __to_vnam(vs2, is_stren, nval));
   /* if nothing pending can just schedule */
   __schedule_1wev(np, biti, TE_WIRE, ndel, schtim, nval, itevpi,
    tr_decay);
   return;
  }

 tevp = &(__tevtab[tevpi]);
 /* DBG remove -- */
 if (tevp->tetyp != TE_WIRE) __misc_terr(__FILE__, __LINE__);
 /* --- */

 /* pending event - no spike analysis but inertial reschedule */
 /* current driving and schedule same causes scheduled to be removed since */
 /* output value correct */
 if (nval == oval)
  {
   __tr_msg(" PENDING EVENT, SAME <OV=NSV=%s, OSV=%s AT %s CANCEL>\n",
    __to_vnam(vs1, is_stren, nval), __to_vnam(vs2, is_stren, oval), 
    __to_timstr(s1, &schtim));

   /* cancel */
   __cancel_1wev(tevp);
   itevpi[biti] = -1;
  }
 else
  {
   __tr_msg(
    " PENDING EVENT, RESCHD <OV=%s, OSV=%s AT %s, NSV=%s AT %s REPLACES>\n",
    __to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren,
    (word32) tevp->outv), __to_timstr(s1, &(tevp->etime)), __to_vnam(vs3,
    is_stren, nval), __to_timstr(__xs, &schtim));

   __reschedule_1wev(tevpi, nval, ndel, schtim, itevpi);
  }
} 

/*
 * schedule a 1 bit path delay 
 *
 * implements show cancel e analysis including non path distributed delay
 *
 * this works for both strength 8 bit nval and oval and non strength
 * if scalar biti must be 0 (i.e. biti can not be -1)
 * nval is new value to schedule change to, old value is current wire value
 */
static void schd_1pthwirebit(register struct net_t *np, register int32 biti,
 register word32 nval, register word32 oval) 
{
 word32 is_stren;
 word64 schtim;
 i_tev_ndx tevpi, *itevpi;
 struct tev_t *tevp;
 struct rngdwir_t *dwirp; 
 struct pthdst_t *pdp;
 
 if (__ev_tracing)
  {
   evtr_schd_1pthwirebit(np, biti, nval, oval);
   return;
  }
 dwirp = np->nu.rngdwir;
 itevpi = &(dwirp->wschd_pbtevs[np->nwid*__inum]); 
 tevpi = itevpi[biti];

 /* since always use last changed value, if last same as current */
 /* because gate style glitch nothing to do since already right value */   
 if (tevpi == -1 && nval == oval)
  return;

 /* DBG remove --
 if (tevpi != -1 && __tevtab[tevpi].tetyp != TE_WIRE)
  __misc_terr(__FILE__, __LINE__);
 --- */

 /* these globals must be set for get del routine */
 is_stren = (word32) np->n_stren;
 __new_gateval = nval;
 __old_gateval = oval;
 /* possible for some bits to not be path destinations - just immed assign */
 /* SJM 11/24/00 - if values same won't find path or delay since need */
 /* transition */
 if (nval != oval) 
  {
   if ((pdp = __get_path_del(dwirp, biti, &schtim)) == NULL)   
    {
     if (is_stren) __chg_st_bit(np, biti, nval, 0L);
     else __chg_st_bit(np, biti, nval & 1L, (nval >> 1) & 1L);
     return;
    }
  }
 else { pdp = NULL; schtim = 0ULL; }

 /* special case 0 - distributed delay longer - immediate assign */
 /* normal cause is path (probably from multiple input final driving gate) */
 /* that has not path delay on it - this may be ok */
 if (pdp != NULL && schtim <= __simtime)
  {
   /* problem with modeling - distributed delay longer than path */
   if (!__no_informs) __emit_path_distinform(np, pdp, &__pdmindel);

   /* modeling anomally style spike possible - know immed. assign earlier */ 
   if (tevpi != -1)
    {
     tevp = &(__tevtab[tevpi]);

     if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
      __emit_path_pulsewarn(pdp, tevp, &(tevp->etime), NULL,
       "distributed longer or path destination driver unstable",
       is_stren);
     /* always cancel pending */
     __cancel_1wev(tevp);
     itevpi[biti] = -1;

     /* this is same for on detect and on event */
     if (__show_cancel_e)
      {
       /* this is special case where immediate assign must be to x */ 
       /* and cancel future event that can be scheduled for now */
set_on_detect_x:
       if (is_stren) __chg_st_bit(np, biti, (word32) ST_STRONGX, 0L);
       else __chg_st_bit(np, biti, 1L, 1L);
       return;
      }
     /* if no show canceled e, just assign */  
    }

   if (is_stren) __chg_st_bit(np, biti, nval, 0L);
   else __chg_st_bit(np, biti, nval & 1L, (nval >> 1) & 1L);
   return;
  }

 /* no pending event - know nval not = oval or will not get here */
 if (tevpi == -1)
  {
   /* because no pending event must be different */
   __schedule_1wev(np, biti, TE_WIRE, __pdmindel, schtim, nval, itevpi,
    FALSE);
   return;
  }    

 /* pending event */
 /* new and old same but know scheduled different - classic pulse/glitch */
 tevp = &(__tevtab[tevpi]);
 if (nval == oval)
  {
   /* spike analysis, know scheduled different - tell user */
   /* this is classical spike analysis */
   /* do not have delay to use to select pa0th */  
   if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
    {
     __emit_path_samewarn(np, biti, tevp, &(tevp->etime), "pulse", is_stren);
    }

   /* if spike, suppress future but schedule to x at currently scheduled */
   if (__show_cancel_e)
    {
     if (__showe_onevent) { tevp->outv = (is_stren) ? ST_STRONGX : 3; return; }
     __cancel_1wev(tevp);
     itevpi[biti] = -1;
     goto set_on_detect_x;
    }
   /* remove pulse */
   __cancel_1wev(tevp);
   itevpi[biti] = -1;
   return;
  }
 /* SJM 11/24/00 - now know has pdp delay since old and new not same */

 /* new schedule to same value case */
 /* here delay can be different because different path selected */
 /* and maybe other reasons */
 /* done silently here - trace message only below */
 if (tevp->outv == (byte) nval) return;

 /* inertial reschedule */
 if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
  __emit_path_pulsewarn(pdp, tevp, &(tevp->etime), &schtim, "unstable",
   is_stren);   

 /* easy show cancel (set to x case) - no new event may or may not switch */
 if (__show_cancel_e)
  {
   /* LOOKATME - maybe need to check old tevp and new schd time and if 2nd */
   /* input change results in earlier edge cancel and schedule earlier */
   if (__showe_onevent)
    { tevp->outv = (is_stren) ? ST_STRONGX : 3; return; }

   /* remove pulse */
   __cancel_1wev(tevp);
   itevpi[biti] = -1;
   goto set_on_detect_x;
  }
 /* inertial reschedule, this handles cancel if needed */
 __reschedule_1wev(tevpi, nval, __pdmindel, schtim, itevpi);
}

/*
 * trace version - schedule a 1 bit path delay 
 *
 * schedule a 1 bit path delay change 
 *
 * show cancel e analysis including non path distributed delay
 *
 * this works for both strength 8 bit nval and oval and non strength
 * if scalar biti must be 0 (i.e. biti can not be -1)
 * nval is new value to schedule change to, old value is current wire value
 */
static void evtr_schd_1pthwirebit(register struct net_t *np, register int32 biti,
 register word32 nval, register word32 oval) 
{
 word32 is_stren;
 word32 outval;
 word64 schtim, distdel, tevptim;
 i_tev_ndx tevpi, *itevpi;
 struct tev_t *tevp;
 struct rngdwir_t *dwirp; 
 struct pthdst_t *pdp;
 struct spcpth_t *pthp;
 char s1[RECLEN], s2[RECLEN], vs1[10], vs2[10], vs3[10];

 is_stren = np->n_stren;
 dwirp = np->nu.rngdwir;
 itevpi = &(dwirp->wschd_pbtevs[np->nwid*__inum]); 
 tevpi = itevpi[biti];

 if (tevpi != -1)
  {
   /* DBG remove -- */
   if (__tevtab[tevpi].tetyp != TE_WIRE) __misc_terr(__FILE__, __LINE__);
   /* --- */
   strcpy(s1, " (pending event)");
  }
 else strcpy(s1, "");

 /* if no change and do not need schedule time for cancel, done */
 __tr_msg("-- path delay destination %s driver change%s now %s:\n",
  __to_evtrwnam(__xs, np, biti, biti, __inst_ptr), s1,
  __to_timstr(__xs2, &__simtime));

 /* since always use last changed value, if last same as current */
 /* because gate style glitch nothing to do since already right value */   
 if (tevpi == -1 && nval == oval)
  {
   __tr_msg(" PATHDEL, NOCHG <OV=%s> now %s\n",
    __to_vnam(vs1, is_stren, nval), __to_timstr(__xs, &__simtime));
   return;
  }

 /* these globals must be set for get del routine */
 __new_gateval = nval;
 __old_gateval = oval;
 
 if (nval != oval)
  {
   /* possible for some bits to not be path desitinations - just immed assign */
   if ((pdp = __get_path_del(dwirp, biti, &schtim)) == NULL)   
    {
     __tr_msg(" BIT %d NOT PATH DEST: IMMED ASSIGN <OV=%s, NV=%s>\n",
      biti, __to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren, nval));

     if (is_stren) __chg_st_bit(np, biti, nval, 0L);
     else __chg_st_bit(np, biti, nval & 1L, (nval >> 1) & 1L);
     return;
    } 
   pthp = pdp->pstchgp->chgu.chgpthp;
   __tr_msg(" PATH (at line %s) SRC CHG TIME %s\n",
    __bld_lineloc(s1, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt),
    __to_timstr(__xs, &__pdlatechgtim));
  }
 else { pdp = NULL; schtim = 0ULL; }


 /* special case 0 - distributed delay longer - immediate assign */
 if (pdp != NULL && schtim <= __simtime)
  {
   /* problem with modeling - distributed delay longer than path */
   /* or changed path has no path delay */
   if (!__no_informs) __emit_path_distinform(np, pdp, &__pdmindel);

   /* modeling anomally style spike possible - know immed. assign earlier */ 
   if (tevpi != -1)
    {
     tevp = &(__tevtab[tevpi]);
     if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
      __emit_path_pulsewarn(pdp, tevp, &(tevp->etime), NULL,
       "distributed longer or path destination driver unstable",
       is_stren);

     outval = (word32) tevp->outv;
     tevptim = tevp->etime;
     /* always cancel pending */
     __cancel_1wev(tevp);
     itevpi[biti] = -1;

     /* this is same for on detect and on event since immed. assign */
     if (__show_cancel_e)
      {
       /* this is special case where immediate assign must be to x */ 
       /* and cancel future event that can be scheduled for now */
       __tr_msg(
        " PATH, DIST DELAY PULSE <OV=%s, OSV=%s at %s NV=%s SHOWING X FROM NOW>\n",
        __to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren, outval),
        __to_timstr(s1, &tevptim), __to_vnam(vs3, is_stren, nval));

set_on_detect_x:
       if (is_stren) __chg_st_bit(np, biti, (word32) ST_STRONGX, 0L);
       else __chg_st_bit(np, biti, 1L, 1L);
       return;
      }
     __tr_msg(
      " PATH, DIST DELAY PULSE <OV=%s, OSV=%s at %s - NV=%s ASSIGN AND CANCEL>\n",
      __to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren, outval),
      __to_timstr(s1, &tevptim), __to_vnam(vs3, is_stren, nval));
     if (is_stren) __chg_st_bit(np, biti, nval, 0L);
     else __chg_st_bit(np, biti, nval & 1L, (nval >> 1) & 1L);
     return; 
    }
   /* no pending event store - know must be different */
   distdel = __simtime - __pdlatechgtim;
   __tr_msg(
    " DIST DELAY %s LONGER THAN PATH %s: IMMED ASSIGN <OV=%s, NV=%s>\n",
    __to_timstr(__xs2, &distdel), __to_timstr(s1, &__pdmindel), 
    __to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren, nval));
   if (is_stren) __chg_st_bit(np, biti, nval, 0L);
   else __chg_st_bit(np, biti, nval & 1L, (nval >> 1) & 1L);
   return;
  }

 /* real path delay */
 /* case 1: no pending event - know have different new value */
 if (tevpi == -1)
  {
   /* because no pending event must be different */
   __tr_msg(" PATH DEL, SCHD AT %s <OV=%s, NSV=%s>\n",
    __to_timstr(s1, &schtim), __to_vnam(vs1, is_stren, oval),
    __to_vnam(vs2, is_stren, nval));
   __schedule_1wev(np, biti, TE_WIRE, __pdmindel, schtim, nval,
    itevpi, FALSE);
   return;
  }    

 /* pending event */
 tevp = &(__tevtab[tevpi]);
 /* new and old same but know scheduled different - classic pulse/glitch */
 if (nval == oval)
  {
   /* show cancel e analysis, know scheduled different - tell user */
   /* this is classical spike analysis */
   if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
    {
     __emit_path_samewarn(np, biti, tevp, &(tevp->etime), "pulse", is_stren);
    }

   /* if spike, suppress future but schedule to x at currently scheduled */
   if (__show_cancel_e)
    {
     if (__showe_onevent) sprintf(s1, "%s (on event)", __to_timstr(__xs,
      &(tevp->etime)));
     else sprintf(s1, "%s (on detect)", __to_timstr(__xs, &__simtime));

     /* LOOKATME - think on event pulse should use schedule if earlier? */
     __tr_msg(
      " PATH DEL, PEND AT %s, PULSE <OV=NSV=%s, OSV=%s SHOWING X FROM %s>\n",
      __to_timstr(__xs, &(tevp->etime)), __to_vnam(vs1, is_stren, oval),
      __to_vnam(vs2, is_stren, (word32) tevp->outv), s1);

     if (__showe_onevent)
      { tevp->outv = (is_stren) ? ST_STRONGX : 3; return; }

     /* cancel pending */
     __cancel_1wev(tevp);
     itevpi[biti] = -1;
     goto set_on_detect_x;
    }
   /* remove pulse */
   __tr_msg(" PATH DEL, PEND, PULSE, INERTIAL CANCEL AT %s <OV=%s, OSV=%s>\n",
    __to_timstr(s1, &(tevp->etime)), __to_vnam(vs1, is_stren, oval),
    __to_vnam(vs2, is_stren, (word32) tevp->outv));
   /* no spike, but newly scheduled to same so no event - cancel */
   __cancel_1wev(tevp);
   itevpi[biti] = -1;
   return;
  }

 /* new schedule to same value case - know have pdp and delay */
 /* know that delay same and later so just discard new event */
 /* done silently here - trace message only */
 if (tevp->outv == (byte) __new_gateval)
  {
   __tr_msg(
    " PATH DEL, MODEL ANOMALLY IGNORE SCHED TO SAME <OSV=NSV=%s> OLD AT %s NEW %s\n",
    __to_vnam(vs1, is_stren, nval), __to_timstr(s1, &(tevp->etime)),
    __to_timstr(s2, &schtim));
   return;
  }

 /* inertial reschedule */
 if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
  __emit_path_pulsewarn(pdp, tevp, &(tevp->etime), &schtim, "unstable",
   is_stren);

 /* easy show cancel (set to x case) - no new event may or may not switch */
 if (__show_cancel_e)
  {
   if (__showe_onevent) sprintf(s2, "%s (on event)", __to_timstr(__xs,
    &(tevp->etime)));
   else sprintf(s2, "%s (on detect)", __to_timstr(__xs, &__simtime));

   __tr_msg(
    " PATH DEL, PEND AT %s, UNSTABLE <OV=%s, OSV=%s, NSV=%s SHOWING X FROM %s>\n",
     __to_timstr(s1, &(tevp->etime)), __to_vnam(vs1, is_stren, oval),
     __to_vnam(vs2, is_stren, (word32) tevp->outv), __to_vnam(vs3, is_stren,
     nval), s2);
   if (__showe_onevent)
    { tevp->outv = (is_stren) ? ST_STRONGX : 3; return; }

   __cancel_1wev(tevp);
   itevpi[biti] = -1;
   goto set_on_detect_x;
  }

 /* inertial reschedule, this handles cancel if needed */ 
 __tr_msg(
  " PATH DEL, PEND, UNSTABLE, INERTIAL RESCHD <OV=%s, OSV=%s AT %s, NSV=%s AT %s>\n",
  __to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren, (word32) tevp->outv),
  __to_timstr(s1, &(tevp->etime)), __to_vnam(vs3, is_stren, nval),
  __to_timstr(s2, &schtim)); 

 __reschedule_1wev(tevpi, nval, __pdmindel, schtim, itevpi);
}

/*
 * print a distributed delay longer than path warning
 * normally caused by path not having path delay which is maybe ok
 *
 * algorithm for path delays is: 1) record path source changes, 2) only when
 * destination changes (attempt to assign value to wire) schedule wire delay
 * at src change time plus path delay, 3) if dest. assign to wire after
 * time when path would have changed immediate assign with warning
 * 
 * this is questionable inform because happens when source change
 * does not propagate to output
 */
extern void __emit_path_distinform(struct net_t *np, struct pthdst_t *pdp,
 word64 *pdmindel)
{
 word64 distdel;
 struct spcpth_t *pthp;
 char s1[RECLEN], s2[RECLEN];

 distdel = __simtime - __pdlatechgtim;
 pthp = pdp->pstchgp->chgu.chgpthp;

 __gfinform(470, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
  "path (in %s) distributed delay %s longer than path %s (path without path delay? or destination driver unstable) - storing %s",
  __msg2_blditree(s1, __inst_ptr), __to_timstr(__xs, &distdel),  
  __to_timstr(__xs2, pdmindel), __to_vnam(s2,
  (unsigned) ((np->n_stren) ? TRUE : FALSE), (word32) __new_gateval));
}

/*
 * emit path pulse warning if not turned off
 */
extern void __emit_path_pulsewarn(struct pthdst_t *pdp, struct tev_t *tevp,
 word64 *etim, word64 *newetim, char *sptnam, word32 is_stren)
{
 struct spcpth_t *pthp;
 char s1[RECLEN], s2[RECLEN], s3[10], s4[10], s5[10], s6[RECLEN];

 /* must turn on spike analysis */
 if (__show_cancel_e)
  {
   if (__showe_onevent) strcpy(s1, " - edge event to x");
   else strcpy(s1, " - now detect to x");
  }
 else strcpy(s1, "");

 sprintf(s2, "old %s, scheduled %s, new %s%s",
  __to_vnam(s3, is_stren, (word32) __old_gateval), __to_vnam(s4, is_stren,
  (word32) tevp->outv), __to_vnam(s5, is_stren, (word32) __new_gateval), s1);   

 if (newetim == NULL) 
  { sprintf(s6, "(edge at %s removed)", __to_timstr(__xs2, etim)); }
 else 
  {
   sprintf(s6, "(edge at %s replaced by new at %s)",
    __to_timstr(__xs, etim), __to_timstr(__xs2, newetim));
  }
 pthp = pdp->pstchgp->chgu.chgpthp;
 /* notice spike means new and old the same */
 __gfwarn(592, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
  "path (in %s) %s %s - %s", __msg2_blditree(s1, tevp->teitp), sptnam,
 s6, s2);
}

/*
 * emit path pulse warning for same value unstable (no new path)
 */
extern void __emit_path_samewarn(struct net_t *np, int32 biti,
 struct tev_t *tevp, word64 *etim, char *sptnam, word32 is_stren)
{
 char s1[RECLEN], s2[RECLEN], s3[10], s4[10], s5[10], s6[RECLEN];

 /* must turn on spike analysis */
 if (__show_cancel_e)
  {
   if (__showe_onevent) strcpy(s1, " - edge event to x");
   else strcpy(s1, " - now detect to x");
  }
 else strcpy(s1, "");

 sprintf(s2, "old %s, scheduled %s, new %s%s",
  __to_vnam(s3, is_stren, (word32) __old_gateval), __to_vnam(s4, is_stren,
  (word32) tevp->outv), __to_vnam(s5, is_stren, (word32) __new_gateval), s1);   

 sprintf(s1, "(edge at %s removed)", __to_timstr(__xs2, etim));

 if (np->n_isavec) sprintf(s6, "%s %s[%d]", __to_ptnam(__xs, np->iotyp),
  np->nsym->synam, biti);
 else sprintf(s6, "%s %s", __to_ptnam(__xs, np->iotyp), np->nsym->synam);

 /* notice spike means new and old the same */
 __pv_warn(592, "path destination %s: %s %s - %s", s6, sptnam, s1, s2);
}

/*
 * compute path delay and imputed schedule time (sets delay in global)
 *
 * caller determines action if immediate assign needed
 * return nil for bit not path dest. (other bits are), pschtim not set
 *
 * rules for multiple paths with this destination
 *  1) select latest change (inertial pattern - schedule with latest)
 *  2) if 2 sources changed at same time use shortest path - open path end
 *     spigot as soon possible
 *
 * this assumes globals __new_gateval and __old_gateval have out transition 
 * do not need delay because here know never pnd0
 * notice transition that selects delay here is output change 
 *
 * this finds last change path even though maybe no input changed and
 * this is just path dest. change from non path delay cause
 *
 * LOOKATME - contrary to P1364 LRM, ifnone paths just like other paths
 * in selecting path delays to use.  filtering for sdps done on input
 * and because input changes may not propagate to output, if ifnone selected
 * because latest change (and shortest delay if tie) must use - ifnone only
 * used to distinguish exact ties in last change and delay time
 */
extern struct pthdst_t *__get_path_del(struct rngdwir_t *dwirp, int32 biti,
 word64 *pschtim) 
{
 register struct pthdst_t *pdp, *latepdp;
 register struct spcpth_t *latepthp, *newpthp;
 word64 chgtim, newdel;

 if ((pdp = dwirp->n_du.pb_pthdst[biti]) == NULL) return(NULL);

 /* list of paths terminating on wire np is same for all insts */
 __pdlatechgtim = pdp->pstchgp->lastchg[__inum];
 latepdp = pdp;
 latepthp = latepdp->pstchgp->chgu.chgpthp;
 if (__pth_tracing || (__debug_flg && __ev_tracing))
  prt_dbgpthtrmsg(latepthp, __pdlatechgtim);
 __get_del(&__pdmindel, latepthp->pth_du, latepthp->pth_delrep);
 /* common only 1 path ends on net case */ 
 if ((pdp = pdp->pdnxt) == NULL)
  {
   *pschtim = __pdlatechgtim + __pdmindel;
   return(latepdp);
  }

 /* complicated case where more than one path end on this dest. wire np */
 for (;pdp != NULL; pdp = pdp->pdnxt)
  {
   /* get source change time */
   chgtim = pdp->pstchgp->lastchg[__inum];
   if (__pth_tracing || (__debug_flg && __ev_tracing))
    prt_dbgpthtrmsg(pdp->pstchgp->chgu.chgpthp, chgtim);

   /* case 1: change time earlier, always select latest */
   if (chgtim < __pdlatechgtim) continue;
    
   newpthp = pdp->pstchgp->chgu.chgpthp;
   __get_del(&newdel, newpthp->pth_du, newpthp->pth_delrep);

   /* if change times are the same, use the shortest path */
   if (chgtim == __pdlatechgtim)
    {
     if (newpthp->pth_ifnone && newdel == __pdmindel) continue;

     /* newdel larger implies not shorter path, do not change */
     /* if same try to replace since for ties must replace ifnone */ 
     if (newdel > __pdmindel) continue;
    }
   /* this path's change time later(< handled above), just use */
   else __pdlatechgtim = chgtim;

   __pdmindel = newdel;
   latepdp = pdp;
   latepthp = newpthp;
  }
 *pschtim = __pdlatechgtim + __pdmindel;
 return(latepdp);
}

/*
 * print a debugging or path trace message
 */
static void prt_dbgpthtrmsg(struct spcpth_t *newpthp, word64 chgtim)
{
 word64 newdel;
 char s1[RECLEN];

 __get_del(&newdel, newpthp->pth_du, newpthp->pth_delrep);
 __tr_msg("## path (line %s) last change %s delay %s.\n",
  __bld_lineloc(__xs, newpthp->pthsym->syfnam_ind, newpthp->pthsym->sylin_cnt),
  __to_timstr(__xs2, &chgtim), __to_timstr(s1, &newdel)); 
}

/*
 * compute intermodule path delay and imputed schedule time 
 * only called if at least one inter module interconnect path
 *
 * this assumes new and old gate vals glbs set
 * using same algorithm as used for normal specify path delays 
 * each inst/bit for mipd src-dst delays different (outside inst struct)
 * LOOKATME - is this algorithm right for intra-module paths
 */
static void get_impth_del(word64 *pschtim, struct net_t *np, int32 bi,
 struct mipd_t *mipdp)
{
 register struct impth_t *impdp;
 word64 chgtim, newdel;
  
 impdp = mipdp->impthtab[__inum];
 __pdlatechgtim = impdp->lastchg;
 /* know delay is non IS since src-dst delays outside inst tree */
 __get_del(&__pdmindel, impdp->impth_du, impdp->impth_delrep);

 if (__pth_tracing || (__debug_flg && __ev_tracing))
  { prt_dbgimpthtrmsg(np, bi, __pdlatechgtim, __pdmindel); } 

 /* common only 1 path ends on net/bit case */ 
 if ((impdp = impdp->impthnxt) == NULL)
  {
   *pschtim = __pdlatechgtim + __pdmindel;
   return;
  }

 /* complicated case where more than one path end on this dest. wire np */
 for (;impdp != NULL; impdp = impdp->impthnxt)
  {
   /* get source change time */
   chgtim = impdp->lastchg;

   if (__pth_tracing || (__debug_flg && __ev_tracing))
    { 
     /* know this will be non IS delay */
     __get_del(&newdel, impdp->impth_du, impdp->impth_delrep);
     prt_dbgimpthtrmsg(np, bi, chgtim, newdel);
    }

   /* case 1: change time earlier, always select latest */
   if (chgtim < __pdlatechgtim) continue;
    
   /* know this will be non IS delay */
   __get_del(&newdel, impdp->impth_du, impdp->impth_delrep);

   /* if change times are the same, use the shortest path */
   if (chgtim == __pdlatechgtim)
    {
     /* newdel larger implies not shorter path, do not change */
     /* if same try to replace */
     if (newdel > __pdmindel) continue;
    }
   /* this path's change time later(< handled above), just use */
   else __pdlatechgtim = chgtim;

   __pdmindel = newdel;
  }
 *pschtim = __pdlatechgtim + __pdmindel;
}

/*
 * print a debugging or path trace message
 * passed bi as 0 for scalar and corrects in here
 */
static void prt_dbgimpthtrmsg(struct net_t *np, int32 bi, word64 chgtim,
 word64 newdel)
{
 int32 bi2;
 char s1[RECLEN];

 bi2 = (!np->n_isavec) ? -1 : bi;
 __tr_msg("## intermodule src-dest path end on %s last change %s delay %s.\n",
  __to_evtrwnam(__xs, np, bi2, bi2, __inst_ptr), __to_timstr(__xs2, &chgtim),
  __to_timstr(s1, &newdel)); 
}

/*
 * schedule 1 wire event
 *
 * passed in np but may be mpp (no cast needed) user must access right union
 */
extern void __schedule_1wev(struct net_t *np, int32 bi, int32 etyp, word64 ndel,
 word64 schtim, word32 newval, i_tev_ndx *itevpi, int32 tr_decay)
{
 register i_tev_ndx tevpi; 
 register struct tev_t *tevp;
 register struct tenp_t *tenp; 

 alloc_tev_(tevpi, etyp, __inst_ptr, schtim);
 if (ndel == 0ULL)
  {
   /* this is #0, but must still build tev */
   if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevpi;
   else { __tevtab[__p0_te_endi].tenxti = tevpi; __p0_te_endi = tevpi; }
  }
 else __insert_event(tevpi);
 tevp = &(__tevtab[tevpi]);
 tevp->outv = (byte) newval;
 tevp->te_trdecay = tr_decay;
 itevpi[bi] = tevpi;

 tenp = (struct tenp_t *) __my_malloc(sizeof(struct tenp_t));
 tevp->tu.tenp = tenp;
 tenp->tenu.np = np;
 tenp->nbi = bi;
}

/*
 * take wire del event and new val and update if time same or cancel and
 * create new event if later
 */
extern void __reschedule_1wev(i_tev_ndx tevpi, word32 newval, word64 ndel,
 word64 newtim, i_tev_ndx *itevpi)
{
 struct tenp_t *tenp; 
 struct tev_t *tevp;

 tevp = &(__tevtab[tevpi]);
 /* if del == 0 (pnd0), will always be same time reschedule */
 if (ndel == 0ULL)
  {
   /* new scheduled value replaces old - itevp remains correct */
   __newval_rescheds++;
   tevp->outv = (byte) newval;
   return;
  }
 tenp = tevp->tu.tenp;
 /* notice this will replace pending event in itevp */
 __schedule_1wev(tenp->tenu.np, tenp->nbi, (int32) tevp->tetyp, ndel,
  newtim, newval, itevpi, (int32) tevp->te_trdecay);
 __cancel_1wev(tevp);
}

/*
 * cancel 1 wev - after process or really cancel to free storage
 * know itevp new event already adjusted
 *
 * this must be event not index since need old here
 */
extern void __cancel_1wev(struct tev_t *tevp)
{
 tevp->te_cancel = TRUE;
 __inertial_cancels++;
 /* DBG remove --- */
 if (tevp->tu.tenp == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */
 __my_free((char *) tevp->tu.tenp, sizeof(struct tenp_t)); 
 tevp->tu.tenp = NULL;
}

/*
 * ROUTINES TO PROCESS MIPD LOAD SCHEDULING AND EV PROCESSING
 */

/*
 * schedule one simple MIPD delay nchg propagate event
 * this runs with itree instance location pushed
 *
 * works by stopping normal net change load propagation and scheduling it
 * after MIPD delay has elaspsed
 */
extern void __sched_mipd_nchg(struct net_t *np, int32 bi, struct mipd_t *mipdp)
{
 register i_tev_ndx tevpi;
 register word32 nval, oval;
 word32 nav, nbv;
 word64 ndel, schtim;
 struct tev_t *tevp;
 struct tenp_t *tenp; 

 if (__ev_tracing)
  { evtr_sched_mipd_nchg(np, bi, mipdp); return; }

 /* load new wire value - need this call in case packed */
 if (!np->n_isavec)
  {
   /* BEWARE - this depends on all scalars stored as byte array */
   /* need to preserve strens for change check */
   nval = (word32) np->nva.bp[__inum];
  }
 else
  {
   if (np->srep == SR_SVEC)
    {
     /* BEWARE - this depends on stren vec stored as byte array */
     nval = (word32) np->nva.bp[__inum*np->nwid + bi];
    }
   else
    {
     __ld_bit(&nav, &nbv, np, bi); 
     nval = (nav & 1) | ((nbv & 1) << 1);
    }
  }

 tevpi = mipdp->mipdschd_tevs[__inum];
 oval = mipdp->oldvals[__inum];

 /* if no pending event and this bit unchanged nothing to schedule */
 /* know at least one bit chged or will not get here but maybe not this one */
 if (tevpi == -1 && nval == oval) return;

 mipdp->oldvals[__inum] = nval;
 /* delay only uses logic not stren part of value */
 __new_gateval = nval & 3;
 __old_gateval = oval & 3;

 /* notice old and new gate values must be set before here */
 if (!mipdp->pth_mipd || mipdp->impthtab == NULL
  || mipdp->impthtab[__inum] == NULL)
  {
   /* non src-dst path delay for this simple MIPD case */
   __get_del(&ndel, mipdp->pb_mipd_du, mipdp->pb_mipd_delrep);
   schtim = __simtime + ndel;
  }
 else
  {
   /* use same algorithm as specify path delay algorithm to get last chged */
   get_impth_del(&schtim, np, bi, mipdp); 
   ndel = schtim - __simtime;
  }

 if (tevpi == -1)
  {
   /* if nothing pending can just schedule */
   alloc_tev_(tevpi, TE_MIPD_NCHG, __inst_ptr, schtim);
   if (ndel == 0ULL)
    {
     /* this is #0, but must still build tev */
     if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevpi;
     else { __tevtab[__p0_te_endi].tenxti = tevpi; __p0_te_endi = tevpi; }
    }
   else __insert_event(tevpi);
   mipdp->mipdschd_tevs[__inum] = tevpi;
   tevp = &(__tevtab[tevpi]);
   tenp = (struct tenp_t *) __my_malloc(sizeof(struct tenp_t));
   tevp->tu.tenp = tenp;
   tenp->tenu.np = np;
   tenp->nbi = bi;
   return;
  }

 tevp = &(__tevtab[tevpi]);
 /* DBG remove --- */
 if (tevp->tetyp != TE_MIPD_NCHG)
  {
   char s1[RECLEN], s2[RECLEN];
   extern char *__to_tetyp(char *, word32);

   __dbg_msg("^^%s event index %d in inst. %s at %s cancel=%d\n",
    __to_tetyp(s1, tevp->tetyp), tevpi, __msg2_blditree(s2, tevp->teitp),
    __to_timstr(__xs, &__simtime), tevp->te_cancel);
   __misc_terr(__FILE__, __LINE__);
  }
    
 /* --- */

 /* pending event - no spike analysis but inertial reschedule */
 /* case 1a: pending event earlier than newly scheduled */
 if (tevp->etime <= schtim)
  {
   /* current driving and schedule same, new later inertial value just */
   /* causes scheduled to be removed since output at right value */
   if (nval == oval)
    {
     /* cancel */
     cancel_1mipdev(tevp);
     mipdp->mipdschd_tevs[__inum] = -1;
    }
   else
    {
     /* reschedule - cancel and sched new or replace if pound 0 */
     mipdp->mipdschd_tevs[__inum] = reschedule_1mipd(np, bi, tevpi, ndel,
      schtim);
    }
   /* fall through since next case does nothing */
  }
 /* case 1b: pending event later (rare modeling anomally?) */ 
 /* since inertial just ignore new change */
}

/*
 * cancel 1 mipd ev - after process or really cancel to free storage
 * this must be event not index since need old here
 *
 * sinc wev also uses tenp malloced field this is same as cance 1 wev
 */
static void cancel_1mipdev(struct tev_t *tevp)
{
 tevp->te_cancel = TRUE;
 __inertial_cancels++;
 /* DBG remove --- */
 if (tevp->tu.tenp == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */
 __my_free((char *) tevp->tu.tenp, sizeof(struct tenp_t)); 
 tevp->tu.tenp = NULL;
}

/*
 * take wire del event and new val and update if time same or cancel and
 * create new event if later
 */
static i_tev_ndx reschedule_1mipd(struct net_t *np, int32 bi, i_tev_ndx tevpi,
 word64 ndel, word64 newtim)
{
 register struct tev_t *tevp, *tevp2;
 register struct tenp_t *tenp; 
 i_tev_ndx tevpi2;

 tevp = &(__tevtab[tevpi]);
 /* if del == 0 (pnd0), will always be same time reschedule */
 if (ndel == 0ULL)
  {
   /* new scheduled value replaces old - same pending event */
   __newval_rescheds++;
   return(tevpi);
  }

 /* if nothing pending can just schedule */
 alloc_tev_(tevpi2, TE_MIPD_NCHG, __inst_ptr, newtim);
 __insert_event(tevpi2);
 tevp2 = &(__tevtab[tevpi2]);
 tenp = (struct tenp_t *) __my_malloc(sizeof(struct tenp_t));
 tevp2->tu.tenp = tenp;
 tenp->tenu.np = np;
 tenp->nbi = bi;

 cancel_1mipdev(tevp);
 return(tevpi2);
}

/*
 * tracing version schedule one simple MIPD delay nchg propagate event
 *
 * no spike analysis but normal inertial rescheduling
 * this works for both strength 8 bit nval and oval and non strength
 */
static void evtr_sched_mipd_nchg(struct net_t *np, int32 bi,
 struct mipd_t *mipdp)
{
 register i_tev_ndx tevpi;
 register word32 nval, oval;
 word32 nav, nbv;
 word64 ndel, schtim;
 word32 is_stren;
 struct tev_t *tevp;
 struct tenp_t *tenp; 
 char s1[RECLEN], vs1[10], vs2[10], vs3[10];

 is_stren = np->n_stren;
 tevpi = mipdp->mipdschd_tevs[__inum];

 /* if no change and do not need schedule time for cancel, done */
 __tr_msg("-- scheduling MIPD for %s:\n",
  __to_evtrwnam(__xs, np, bi, bi, __inst_ptr));

 /* load new wire value - need this call in case packed */
 if (!np->n_isavec)
  {
   /* BEWARE - this depends on all scalars stored as byte array */
   /* need to preserve strens for change check */
   nval = (word32) np->nva.bp[__inum];
  }
 else
  {
   if (np->srep == SR_SVEC)
    {
     /* BEWARE - this depends on stren vec stored as byte array */
     nval = (word32) np->nva.bp[__inum*np->nwid + bi];
    }
   else
    {
     __ld_bit(&nav, &nbv, np, bi); 
     nval = (nav & 1) | ((nbv & 1) << 1);
    }
  }
 oval = mipdp->oldvals[__inum];

 /* since always use last changed value, if last same as current */
 /* because gate style spike nothing to do since already right value */   
 if (tevpi == -1 && nval == oval)
  {
   __tr_msg(" NOPEND, NOCHG <OV=%s>\n", __to_vnam(vs1, is_stren, nval));
   return;
  }

 mipdp->oldvals[__inum] = nval;
 /* delay only uses logic not stren part of value */
 __new_gateval = nval & 3;
 __old_gateval = oval & 3;
 /* notice old and new gate values must be set before here */
 if (!mipdp->pth_mipd || mipdp->impthtab == NULL
  || mipdp->impthtab[__inum] == NULL)
  {
   /* non src-dst path delay for this simple MIPD case */
   __get_del(&ndel, mipdp->pb_mipd_du, mipdp->pb_mipd_delrep);
   schtim = __simtime + ndel;
  }
 else
  {
   /* use same algorithm as specify path delay algorithm to get last chged */
   get_impth_del(&schtim, np, bi, mipdp); 
   ndel = schtim - __simtime;
  }

 if (tevpi == -1)
  {
   __tr_msg(" SCHD AT %s <OV=%s, NSV=%s>\n", __to_timstr(s1, &schtim),
    __to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren, nval));

   /* if nothing pending can just schedule */
   alloc_tev_(tevpi, TE_MIPD_NCHG, __inst_ptr, schtim);
   if (ndel == 0ULL)
    {
     /* this is #0, but must still build tev */
     if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevpi;
     else { __tevtab[__p0_te_endi].tenxti = tevpi; __p0_te_endi = tevpi; }
    }
   else __insert_event(tevpi);

   mipdp->mipdschd_tevs[__inum] = tevpi;
   tevp = &(__tevtab[tevpi]);
   tenp = (struct tenp_t *) __my_malloc(sizeof(struct tenp_t));
   tevp->tu.tenp = tenp;
   tenp->tenu.np = np;
   tenp->nbi = bi;
   return;
  }

 tevp = &(__tevtab[tevpi]);
 /* DBG remove --- */
 if (tevp->tetyp != TE_MIPD_NCHG)
  {
   char s2[RECLEN];
   extern char *__to_tetyp(char *, word32);

   tevp = &(__tevtab[tevpi]);
   __dbg_msg("^^%s event index %d in inst. %s at %s cancel=%d\n",
    __to_tetyp(s1, tevp->tetyp), tevpi, __msg2_blditree(s2, tevp->teitp),
    __to_timstr(__xs, &__simtime), tevp->te_cancel);
   __misc_terr(__FILE__, __LINE__);
  }
 /* --- */

 /* pending event - no spike analysis but inertial reschedule */
 /* case 1a: pending event earlier than newly scheduled */
 if (tevp->etime <= schtim)
  {
   /* current driving and schedule same, new later inertial value just */
   /* causes scheduled to be removed since output at right value */
   if (nval == oval)
    {
     __tr_msg(" PENDING NCHG EVENT, SAME <NV==%s, OV=%s AT %s CANCEL>\n",
      __to_vnam(vs1, is_stren, nval), __to_vnam(vs2, is_stren, oval), 
      __to_timstr(s1, &schtim));
     /* cancel */
     cancel_1mipdev(tevp);
     mipdp->mipdschd_tevs[__inum] = -1;
    }
   else
    {
     __tr_msg(
      " PENDING NCHG EVENT, RESCHD <OV=%s, AT %s, NV=%s AT %s REPLACES>\n",
      __to_vnam(vs1, is_stren, oval), __to_timstr(s1, &(tevp->etime)),
       __to_vnam(vs3, is_stren, nval), __to_timstr(__xs, &schtim));

     /* reschedule - cancel and sched new or replace if pound 0 */
     mipdp->mipdschd_tevs[__inum] = reschedule_1mipd(np, bi, tevpi, ndel,
      schtim);
    }
   /* although next case does nothing, must not emit the message */
   return;
  }
 /* case 1b: pending event later (rare modeling anomally?) */ 
 /* since inertial just ignore new change */
 __tr_msg(
  " PENDING NCHG EVENT, NEW EARLY <OV=%s AT %s, INERTIAL IGNORE NV=%s AT %s>\n",
  __to_vnam(vs1, is_stren, oval), __to_timstr(s1, &(tevp->etime)),
  __to_vnam(vs3, is_stren, nval), __to_timstr(__xs, &schtim));
} 

/*
 * VALUE STORE ROUTINES FOR ASSIGNS
 */

/*
 * store (copy) an entire value from rgap and rgbp into wp of length blen
 * from current instance 
 * stored according to representation srep from from rgap and rgbp
 * but cannot be used to store strength values
 *
 * stored using representation srep
 * separate rgap and rgbp into code contigous wp since stack size changes
 * cause non contigous storage form
 * know rgap and rgbp exact np nwid width
 */
extern void __st_val(struct net_t *np, register word32 *rgap,
 register word32 *rgbp)
{
 register word32 *dwp;
 register int32 wlen;
 byte *newsbp;

 switch ((byte) np->srep) {
  case SR_SCAL:
   st_scalval_(np->nva.bp, rgap[0], rgbp[0]);
   break;
  case SR_VEC:
   wlen = wlen_(np->nwid);
   dwp = &(np->nva.wp[2*wlen*__inum]);
   st_vecval(dwp, np->nwid, rgap, rgbp);
   break;
  case SR_SVEC:
   /* this must be strong */
   memcpy(&(np->nva.bp[np->nwid*__inum]), rgap, np->nwid);
   break;
  case SR_SSCAL:
   newsbp = (byte *) rgap;
   np->nva.bp[__inum] = *newsbp;
   break;
  default: __case_terr(__FILE__, __LINE__); return;
 }
}

/*
 * store a per instance value into a word32 location
 * mostly for storing cont. assign driver values
 * bit width determines form
 *
 * caller must make sure source blen and vblen same
 */
extern void __st_perinst_val(union pck_u pckv, int32 vblen, register word32 *rgap,
 register word32 *rgbp)
{
 register word32 *dwp;
 int32 wlen;

 /* know rgab always scalar here if 1 bit case */
 if (vblen == 1) { st_scalval_(pckv.bp, rgap[0], rgbp[0]); return; }
 /* SJM - 07/15/00 - all vectors now not packed - min 2 words */
 wlen = wlen_(vblen);
 dwp = &(pckv.wp[2*wlen*__inum]);
 st_vecval(dwp, vblen, rgap, rgbp);
}

/*
 * store into value at dwp of length destination length from ap and bp of
 * srcblen
 *
 * any high bits of destination must be zeroed - not for lhs selects
 * know vectors always occupy number of words (no packing to less then word32)
 * routine called after array or instance decoding completed to get dwp
 */
static void st_vecval(word32 *dwp, int32 blen, register word32 *ap,
 register word32 *bp)
{
 int32 wlen;

 /* truncating wide source into narrower dest. */
 wlen = wlen_(blen);
 /* know copy will 0 any high unused bits of high word32 */
 cp_walign_(dwp, ap, blen);
 cp_walign_(&(dwp[wlen]), bp, blen);
}

/*
 * store if changed (and set flag) know rgap and rgbp are adjusted and
 * z extended (if needed) to exact wire width
 * also any 1 bit cases must already be adjusted to a part only form  
 */
extern void __chg_st_val(register struct net_t *np, word32 *rgap, word32 *rgbp)
{
 register word32 *dwp;
 int32 wlen;
 byte *netsbp, *newsbp;

 switch ((byte) np->srep) {
  case SR_SCAL:
   chg_st_scalval_(np->nva.bp, rgap[0], rgbp[0]);
   break;
  case SR_VEC:
   wlen = wlen_(np->nwid);
   dwp = &(np->nva.wp[2*wlen*__inum]);
   chg_st_vecval(dwp, np->nwid, rgap, rgbp);
   break;
  case SR_SVEC:
   /* this must be strong */
   netsbp = &(np->nva.bp[np->nwid*__inum]); 
   newsbp = (byte *) rgap;
   if (memcmp(netsbp, newsbp, np->nwid) != 0)
    {
     /* bcopy 2nd argument is destination */
     memcpy(netsbp, newsbp, np->nwid);
     __lhs_changed = TRUE;
    }
   break;
  case SR_SSCAL:
   netsbp = &(np->nva.bp[__inum]); 
   newsbp = (byte *) rgap;
   if (*netsbp != *newsbp) { *netsbp = *newsbp; __lhs_changed = TRUE; } 
   break;
  default: __case_terr(__FILE__, __LINE__); return;
 }
 if (__lhs_changed) record_nchg_(np);
}

/*
 * routine to record net change - non macro for debugging
 */
/* DBG ??? add ---
extern void __record_nchg(struct net_t *np)
{
 -* SJM 08/08/03 - can't assume caller turns off chged flag any more *-
 -* but one record called, it must be off for dctrl processing - not needed *- 
 __lhs_changed = FALSE;

 -* --- DBG remove
 if (__debug_flg)
  {
   strcpy(__xs2, "");
   if (np->nlds == NULL) strcat(__xs2, "[no lds, ");
   else strcat(__xs2, "[ld, ");
   if (np->dcelst == NULL) strcat(__xs2, "no dces, ");
   else strcat(__xs2, "dces, ");
   if (np->ndrvs == NULL) strcat(__xs2, "no drvs]");
   else strcat(__xs2, "drvs]");
   
   -* emit for inst 0 (all should be same) *-
   __dbg_msg("record nchg for net %s type %s nchgaction=%x conn=%s\n",
    np->nsym->synam, __to_wtnam(__xs, np), np->nchgaction[__inum], __xs2);
  }
 --- *-

 -* SJM 07/24/00 - has dces only on for regs *-
 -* SJM 03/15/01 - change to fields in net record *-
 if (np->nchg_has_dces) __wakeup_delay_ctrls(np, -1, -1);

 if ((np->nchgaction[__inum] & NCHG_ALL_CHGED) == 0)
   __add_nchglst_el(np);

 if ((np->nchgaction[__inum] & (NCHG_DMPVARNOW | NCHG_DMPVNOTCHGED)) 
  == (NCHG_DMPVARNOW | NCHG_DMPVNOTCHGED))
  {
   np->nchgaction[__inum] &= ~(NCHG_DMPVNOTCHGED);
   __add_dmpv_chglst_el(np);
  }
}
--- */

/*
 * routine to record net select change - non macro for debugging
 */
/* DBG ??? add ---
extern void __record_sel_nchg(struct net_t *np, int32 i1, int32 i2)
{
 -* SJM 08/08/03 - can't assume caller turns off chged flag any more *-
 -* but one record called, it must be off for dctrl processing - not needed *- 
 __lhs_changed = FALSE;

 -* --- DBG remove *-
 if (__debug_flg)
  {
   strcpy(__xs2, "");
   if (np->nlds == NULL) strcat(__xs2, "[no lds, ");
   else strcat(__xs2, "[ld, ");
   if (np->dcelst == NULL) strcat(__xs2, "no dces, ");
   else strcat(__xs2, "dces, ");
   if (np->ndrvs == NULL) strcat(__xs2, "no drvs]");
   else strcat(__xs2, "drvs]");
   
   __dbg_msg("record nchg for net %s[%d:%d] type %s nchgaction=%x conn=%s\n",
    np->nsym->synam, i1, i2, __to_wtnam(__xs, np), np->nchgaction[__inum],
    __xs2);
  }
 --- *-

 -* --- *- 
 -* SJM 07/24/00 - has dces only on for regs *-
 if (np->nchg_has_dces) __wakeup_delay_ctrls(np, i1, i2); 

 if ((np->nchgaction[__inum] & NCHG_ALL_CHGED) == 0)
  __add_select_nchglst_el(np, i1, i2); 

 if ((np->nchgaction[__inum] & (NCHG_DMPVARNOW | NCHG_DMPVNOTCHGED)) 
  == (NCHG_DMPVARNOW | NCHG_DMPVNOTCHGED))
  {
   np->nchgaction[__inum] &= ~(NCHG_DMPVNOTCHGED);
   __add_dmpv_chglst_el(np);
  }
}
--- */

/*
 * change form of store vector value - know stacked ap and bp width
 * same as destination
 * know blen at least 16 bits or wider
 *
 * caller must adjust
 * notice this does not add changed net to change list
 */
static void chg_st_vecval(register word32 *dwp, int32 blen,
 register word32 *ap, register word32 *bp)
{
 int32 wlen;
 word32 *dwp2;

 if (blen <= WBITS)
  { 
   if (dwp[0] != ap[0])
    {
     dwp[0] = ap[0];
     /* AIV 09/19/06 - if a part is not the same assign bpart regardless */
     /* this is better than doing another compare */
     dwp[1] = bp[0];
     __lhs_changed = TRUE;
     return;
    }
   /* AIV 09/19/06 - was assuming contiguous words (using ap[1] for bpart) */
   /* which isn't always true local words av, bv can be passed */
   if (dwp[1] != bp[0])
    {
     dwp[1] = bp[0];
     __lhs_changed = TRUE;
    }
   return;
  }
 wlen = wlen_(blen);
 if (cmp_wvval_(dwp, ap, wlen) != 0)
  { cp_walign_(dwp, ap, blen); __lhs_changed = TRUE; }
 dwp2 = &(dwp[wlen]); 
 if (cmp_wvval_(dwp2, bp, wlen) != 0)
  { cp_walign_(dwp2, bp, blen); __lhs_changed = TRUE; }
}

/*
 * assign to a bit
 * if np is stren ap will point to array with one byte
 * know ap/bp either strength 8 bits in ap part or exactly 1 bit
 */
extern void __assign_to_bit(struct net_t *np, struct expr_t *idndp,
 struct expr_t *ndx1, register word32 *ap, register word32 *bp)
{
 int32 biti, nd_itpop;
 byte *sbp;
 struct gref_t *grp;

 biti = __comp_ndx(np, ndx1);
 nd_itpop = FALSE;
 if (idndp->optyp == GLBREF)
  { grp = idndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
 if (biti == -1)
  {
   if (np->n_stren)
    {
     sbp = (byte *) ap;
     __stren_schedorassign_unknown_bit(np, (word32) sbp[0], FALSE); 
    }
   else __schedorassign_unknown_bit(np, ap[0], bp[0], FALSE); 
  }
 else
  {
   /* notice best to use change form since it has fast macro checking */
   /* need to record change - non change bit store not much better */
   /* SJM 03/15/01 - change to fields in net record */
   if (np->nchg_nd_chgstore)
    {
     if (np->n_stren)
      {
       sbp = (byte *) ap;
       __chg_st_bit(np, biti, (word32) sbp[0], (word32) 0);
      }
     else __chg_st_bit(np, biti, ap[0], bp[0]);
    }
   else
    {
     if (np->n_stren)
      {
       sbp = (byte *) ap;
       __st_bit(np, biti, (word32) sbp[0], (word32) 0);
      }
     else __st_bit(np, biti, ap[0], bp[0]);
    }
  }
 if (nd_itpop) __pop_itstk();
} 

/*
 * if 1 bit is forced return T (nothing to do) if need assign returns F
 */
extern int32 __forced_inhibit_bitassign(struct net_t *np, struct expr_t *idndp,
 struct expr_t *ndx1)
{
 int32 biti, nd_itpop, rv;
 struct gref_t *grp;

 /* SJM 10/11/02 - should compute index in assign itree cntxt not xmr */
 biti = __comp_ndx(np, ndx1);

 nd_itpop = FALSE;
 if (idndp->optyp == GLBREF)
  { grp = idndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }

 if (biti == -1) __arg_terr(__FILE__, __LINE__);
 /* if the one bit is forced, no need to do assign */
 if (np->nu2.qcval[np->nwid*__inum + biti].qc_active) rv = TRUE;
 else rv = FALSE;
 if (nd_itpop) __pop_itstk();
 return(rv);
} 

/*
 * schedule assignment to a bit
 * ap and bp may be wider than 1 bit 
 */
static void schedassign_to_bit(struct net_t *np, struct expr_t *idndp,
 struct expr_t *ndx1, register word32 *ap, register word32 *bp)
{
 int32 biti, nd_itpop;
 byte *sbp, *sbp2;
 word32 av, bv, nval;
 struct gref_t *grp;

 biti = __comp_ndx(np, ndx1);
 nd_itpop = FALSE;
 if (idndp->optyp == GLBREF)
  { grp = idndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
 if (biti == -1)
  {
   /* here same routine for both */
   if (np->n_stren)
    {
     sbp = (byte *) ap;
     __stren_schedorassign_unknown_bit(np, (word32) sbp[0], TRUE); 
    }
   else __schedorassign_unknown_bit(np, ap[0], bp[0], TRUE); 
   if (nd_itpop) __pop_itstk();
   return;
  }

 if (np->n_stren)
  {
   /* get strength wire address */
   get_stwire_addr_(sbp, np);
   sbp2 = (byte *) ap;
   if (np->nu.rngdwir->n_delrep == DT_PTHDST)
    schd_1pthwirebit(np, biti, (word32) sbp2[0], (word32) sbp[biti]);
   else
    __wdel_schd_1wirebit(np, biti, (word32) sbp2[0], (word32) sbp[biti], FALSE);
  }
 else
  {
   __ld_bit(&av, &bv, np, biti); 
   nval = (ap[0] & 1L) | ((bp[0] << 1) & 2L);
   if (np->nu.rngdwir->n_delrep == DT_PTHDST)
    schd_1pthwirebit(np, biti, nval, (av | (bv << 1)));
   else __wdel_schd_1wirebit(np, biti, nval, (av | (bv << 1)), FALSE);
  }
 if (nd_itpop) __pop_itstk();
}

/*
 * assign or schedule to an unknown - bit - all x that differs from value
 *
 * strength version
 */
extern void __stren_schedorassign_unknown_bit(struct net_t *np, word32 bval,
 int32 schd_wire)
{
 register int32 i;
 byte *sbp;
 word32 newval;

 /* get strength wire address */
 get_stwire_addr_(sbp, np);
 /* even if value the same - strength here always strong */
 if (schd_wire)
  {
   /* case 1a: schedule for delay wire - know not a path source */
   for (i = 0; i < np->nwid; i++)
    {
     if (sbp[i] != (byte) bval) newval = (word32) ST_STRONGX;
     else newval = bval;
     if (newval != (word32) sbp[i])
      {
       if (np->nu.rngdwir->n_delrep == DT_PTHDST)
        schd_1pthwirebit(np, i, newval, (word32) sbp[i]);
       else __wdel_schd_1wirebit(np, i, newval, (word32) sbp[i], FALSE);
      }
    }
   return;
  }
 /* case 1b : immediate assign */
 for (i = 0; i < np->nwid; i++)
  {
   /* LOOKATME - think this can be simplified - but if chg to x and was x*/
   /* still need to do nothing - also need cast to word32 */
   /* SJM 01/18/01 - was always setting lhs changed even if same */
   if (sbp[i] != (byte) bval) newval = (word32) ST_STRONGX;
   else newval = bval;
   if (newval != (word32) sbp[i])
    { sbp[i] = (byte) newval; __lhs_changed = TRUE; }
  }
 /* this could be lots of 1 bit schedules */
 if (__lhs_changed) record_nchg_(np);
}

/*
 * assign or schedule to an unknown - bit - all x that differs from value
 *
 * non strength version
 */
extern void __schedorassign_unknown_bit(struct net_t *np, word32 av, word32 bv,
 int32 schd_wire)
{
 register int32 i;
 word32 bval, newval, oval, w1, w2;
 struct xstk_t *oxsp, *nxsp;

 bval = av | (bv << 1);
 push_xstk_(oxsp, np->nwid);
 /* know net width > 1 */
 __ld_wire_val(oxsp->ap, oxsp->bp, np);
 push_xstk_(nxsp, np->nwid);
 get_unknown_biti_val(np, nxsp->ap, nxsp->bp, oxsp->ap, oxsp->bp, bval); 

 /* bit by bit select comparison if needed for source else just store */
 /* case 2a: schdule delay wire - know cannot be path source */
 if (schd_wire)
  {
   for (i = 0; i < np->nwid; i++)
    {
     w1 = rhsbsel_(nxsp->ap, i);
     w2 = rhsbsel_(nxsp->bp, i);
     newval = w1 | (w2 << 1);
     w1 = rhsbsel_(oxsp->ap, i);
     w2 = rhsbsel_(oxsp->bp, i);
     oval = w1 | (w2 << 1);
     if (newval != oval)
      {
       if (np->nu.rngdwir->n_delrep == DT_PTHDST)
        schd_1pthwirebit(np, i, newval, oval);
       else __wdel_schd_1wirebit(np, i, newval, oval, FALSE);
      }
    }
  }
 /* normal store entire value - faster than bit by bit */
 /* know store and both are wire width */
 else __chg_st_val(np, nxsp->ap, nxsp->bp);
 __pop_xstk();
 __pop_xstk();
}

/*
 * build the new unknown biti value on stack where new bit value is newval
 * this is not needed for strength case where stored as bytes 
 */
static void get_unknown_biti_val(struct net_t *np, word32 *nap, word32 *nbp,
 word32 *oap, word32 *obp, word32 newval)
{
 register int32 i;
 int32 wlen, ubits;

 /* if new value x, then all bits of var. changed to x */
 if (newval == 3)
  { one_allbits_(nap, np->nwid); one_allbits_(nbp, np->nwid); return; }
 /* otherwide make all bits that differ from value x */
 wlen = wlen_(np->nwid);
 for (i = 0; i < wlen; i++)
  {
   nap[i] = oap[i];
   nbp[i] = obp[i];
   setx_ifnotval(&(nap[i]), &(nbp[i]), newval);
  }
 ubits = ubits_(np->nwid);
 nap[wlen - 1] &= __masktab[ubits];
 nbp[wlen - 1] &= __masktab[ubits];
} 

/*
 * set new a word32 and b word32 so that if value leave as is else x
 */
static void setx_ifnotval(word32 *ap, word32 *bp, word32 val)
{
 word32 mask;

 switch ((byte) val) {
  case 0: mask = ap[0] | bp[0]; ap[0] = bp[0] = mask; break;
  case 1: ap[0] = ALL1W; bp[0] = ~(ap[0] | bp[0]) | bp[0]; break;
  case 2: ap[0] = ~(ap[0] | bp[0]) | ap[0]; bp[0] = ALL1W; break;
  case 3: ap[0]= bp[0] = ALL1W; break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * set bit dbit of dwp from low bit of word32 sw 
 * know dbit in range 
 * this does not assume dbit in 1st word32 of dwp
 * also sw may have high bits beside low bit on
 * dwp must be set to right place using current instance
 *
 * these should be macros in asm
 */
extern void __lhsbsel(register word32 *dwp, register int32 dbit, word32 sw)
{
 register int32 bi, wi;

 /* dbit in word32 0 is 0-31, word32 1 is 32-63, etc */
 wi = get_wofs_(dbit);
 /* bi is index with 0 rightmost bit and 31 high bit in select word32 */
 bi = get_bofs_(dbit);
 dwp[wi] &= ~(1L << bi);
 dwp[wi] |= ((sw & 1L) << bi);
}

/*
 * change versions of store bit
 * know biti in range
 */
extern void __chg_st_bit(struct net_t *np, int32 biti, register word32 av,
 register word32 bv)
{
 register word32 *rap;
 byte *netsbp;
 int32 wlen;

 switch ((byte) np->srep) {
 /* this is same as full value store - biti 0 or will not get here */
  case SR_SCAL:
   /* DBG remove --
   if (biti != 0) __arg_terr(__FILE__, __LINE__); 
   --- */
   chg_st_scalval_(np->nva.bp, av, bv);
   break;
  case SR_VEC:
   /* rap is base of vector for current inst */
   wlen = wlen_(np->nwid);
   rap = &(np->nva.wp[2*wlen*__inum]);
   chg_lhsbsel(rap, biti, av);
   chg_lhsbsel(&(rap[wlen]), biti, bv);
   break;
  case SR_SVEC:
   /* rap is base of vector for current inst */
   netsbp =  &(np->nva.bp[np->nwid*__inum]);
   if (netsbp[biti] != (byte) av)
    { netsbp[biti] = (byte) av; __lhs_changed = TRUE; }
   break;
  case SR_SSCAL:
   /* DBG remove ---
   if (biti != 0) __arg_terr(__FILE__, __LINE__); 
   -- */
   netsbp = &(np->nva.bp[__inum]);
   if (netsbp[0] != (byte) av)
    { *netsbp = (byte) av; __lhs_changed = TRUE; }
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 if (__lhs_changed) record_sel_nchg_(np, biti, biti);
}

/*
 * immediate versions of store bit
 * know biti in range
 */
extern void __st_bit(struct net_t *np, int32 biti, register word32 av,
 register word32 bv)
{
 register word32 *rap;
 int32 wlen;

 switch ((byte) np->srep) {
 /* this is same as full value store - biti 0 or will not get here */
  case SR_SCAL:
   /* DBG remove --
   if (biti != 0) __arg_terr(__FILE__, __LINE__); 
   --- */
   st_scalval_(np->nva.bp, av, bv);
   break;
  case SR_VEC:
   /* rap is base of vector for current inst */
   wlen = wlen_(np->nwid);
   rap = &(np->nva.wp[2*wlen*__inum]);
   __lhsbsel(rap, biti, av);
   __lhsbsel(&(rap[wlen]), biti, bv);
   break;
  case SR_SVEC:
   /* rap is base of vector for current inst */
   np->nva.bp[np->nwid*__inum + biti] = (byte) av;
   break;
  case SR_SSCAL:
   /* DBG remove ---
   if (biti != 0) __arg_terr(__FILE__, __LINE__); 
   -- */
   np->nva.bp[__inum] = (byte) av;
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * lhs bit select but do not store if same
 * sets global lhs changed to F if the same
 * LOOKATME - is it true that sw must be masked off - faster if not needed
 */
static void chg_lhsbsel(register word32 *dwp, int32 dbit, word32 sw)
{
 register word32 mask, sw2;
 register int32 bi, wi;

 /* dbit in word32 0: 0-31, word32 1: 32-63, etc (32 left high - 0 right low) */
 wi = get_wofs_(dbit);
 bi = get_bofs_(dbit);
 mask = 1L << bi;
 sw2 = (sw & 1L) << bi;
 if (((dwp[wi] & mask) ^ sw2) != 0L)
  { __lhs_changed = TRUE; dwp[wi] &= ~mask; dwp[wi] |= sw2; }
}

/*
 * assign to an indexed array location (procedural only)
 */
extern void __assign_to_arr(struct net_t *np, struct expr_t *idndp,
 struct expr_t *ndx1, register word32 *ap, register word32 *bp)
{
 int32 arri, nd_itpop, arrwid;
 struct gref_t *grp;

 /* arrwid is number of cells in array */
 arri = __comp_ndx(np, ndx1);
 /* for array - if index out of range - do not change array at all */ 
 if (arri == -1)
  {
   __sgfwarn(530,
   "left hand side array index %s of %s unknown or out of range - unchanged",
    __regab_tostr(__xs, &__badind_a, &__badind_b, __badind_wid, BHEX, FALSE),
    __to_idnam(idndp));
   return;
  }
 /* SJM DBG REMOVEME --- */
 if (arri == -2) __misc_terr(__FILE__, __LINE__);
 /* --- */
 arrwid = __get_arrwide(np);
 nd_itpop = FALSE;
 /* notice for xmr - symbol points to right wire - trick is to make */
 /* sure target itree place right */
 if (idndp->optyp == GLBREF)
  { grp = idndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }

 /* SJM 03/15/01 - change to fields in net record */
 if (np->nchg_nd_chgstore)
  {
   __chg_st_arr_val(np->nva, arrwid, np->nwid, arri, ap, bp);

   /* SJM - 06/25/00 - lhs changed possible from change store */
   /* and must only trigger change for right array index */
   if (__lhs_changed) record_sel_nchg_(np, arri, arri);
  }
 else __st_arr_val(np->nva, arrwid, np->nwid, arri, ap, bp);

 if (nd_itpop) __pop_itstk();
}

/*
 * store into array map of len mlen with element length blen using index
 * arri value from current instance from rgap and rgbp
 *
 * notice arrays stored and normalized to h:0 just like vectors
 * this handles source (to be stored) value of wrong size
 */
extern void __st_arr_val(union pck_u pckv, int32 mlen, int32 blen, int32 arri,
 register word32 *rgap, register word32 *rgbp)
{
 register word32 uwrd;
 int32 indi, wlen, wi, bi;
 register word32 *vap, *rap;

 /* compute number of words used to store 1 array element */
 /* 17 or more bits cannot be packed with multiple elements per word32 */
 /* new real arrays fit here */ 
 if (blen > WBITS/2)
  {
   /* case 1: each vector element of array takes multiple words */

   wlen = wlen_(blen);
   /* find array for inst i with each vector wlen words wide */
   vap = &(pckv.wp[2*wlen*mlen*__inum]);
   /* find element arri that may be a vector */
   rap = &(vap[arri*2*wlen]);

   /* instance and array index used to decode into vector addr rap */
   st_vecval(rap, blen, rgap, rgbp);
   return;
  }

 /* case 2: array of 1 bit elements */
 if (blen == 1)
  {
   indi = 2*(__inum*mlen + arri);
   wi = get_wofs_(indi);
   bi = get_bofs_(indi);
   uwrd = pckv.wp[wi];
   uwrd &= ~(3L << bi); 
   uwrd |= (((rgap[0] & 1L) | ((rgbp[0] & 1L) << 1)) << bi);
   pckv.wp[wi] = uwrd;
   return;
  } 
 /* case 3: array cells packed */
 uwrd = (rgap[0] & __masktab[blen]) | ((rgbp[0] & __masktab[blen]) << blen);
 indi = __inum*mlen + arri;
 st_packintowrd_(pckv, indi, uwrd, blen);
}

/*
 * store a array value if changed only - reset lhs change if not changed
 *
 * notice this does not record change (caller must)
 */
extern void __chg_st_arr_val(union pck_u pckv, int32 mlen, int32 blen, int32 arri,
 register word32 *ap, register word32 *bp)
{
 register word32 *rap, uwrd, ouwrd;
 int32 wlen, wi, bi, indi;
 word32 *vap;

 /* compute number of words used to store 1 array element */
 /* 17 or more bits cannot be packed with multiple elements per word32 */
 if (blen > WBITS/2)
  {
   /* case 1: each vector element of array takes multiple words */
   /* new real arrays fit here */ 

   wlen = wlen_(blen);
   /* find array for inst i with each vector wlen words wide */
   vap = &(pckv.wp[2*wlen*mlen*__inum]);
   /* find element arri that may be a vector */
   rap = &(vap[arri*2*wlen]);
   /* instance and array indexed used to decode into vector addr rap */
   /* SJM 08/24/03 - caller check for lhs changed maybe set by this */
   chg_st_vecval(rap, blen, ap, bp);
   return;
  }

 /* case 2: array of 1 bit elements */
 if (blen == 1)
  {
   indi = 2*(__inum*mlen + arri);
   wi = get_wofs_(indi);
   bi = get_bofs_(indi);
   ouwrd = pckv.wp[wi];
   uwrd = ouwrd & ~(3L << bi); 
   uwrd |= (((ap[0] & 1L) | ((bp[0] & 1L) << 1)) << bi);
   if (ouwrd != uwrd) { pckv.wp[wi] = uwrd; __lhs_changed = TRUE; }
   return;
  } 
 /* case 3: array cells packed */
 indi = __inum*mlen + arri;
 /* SJM 02/08/00 - since memory still need to get pack into word32 */
 ouwrd = get_packintowrd_(pckv, indi, blen);
 uwrd = (ap[0] & __masktab[blen]) | ((bp[0] & __masktab[blen]) << blen);
 if (uwrd != ouwrd)
  {
   st_packintowrd_(pckv, indi, uwrd, blen);
   __lhs_changed = TRUE;
  }
}

/*
 * assign to a part select
 * know xsp width exactly matches part select range
 */
extern void __assign_to_psel(struct expr_t *idndp, int32 ri1, int32 ri2,
 struct net_t *np, register word32 *ap, register word32 *bp)
{
 struct gref_t *grp;
 int32 nd_itpop;

 nd_itpop = FALSE;
 if (idndp->optyp == GLBREF)
  { grp = idndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
 /* if strength, know ap points to st bytes and array rhswid 4x to big */

 /* SJM - 12/14/05 - must not call chg store of psel unless needed */
 /* otherwise - next assign that needs chg store but doesn't chg */  
 /* incorrectly looks like it changed */
 if (np->nchg_nd_chgstore)
  {
   /* if strength, know ap points to st bytes and array rhswid 4x to big */
   chg_st_psel(np, ri1, ri2, ap, bp);
  }
 else st_psel(np, ri1, ri2, ap, bp);

 if (nd_itpop) __pop_itstk();
}

/*
 * assign to a part select
 * know ap/bp width exactly matches part select range
 * if returns F, caller does not do lhs assign, if T must do it
 */
static int32 forced_assign_to_psel(struct expr_t *idndp, int32 ri1, int32 ri2,
 struct net_t *np, register word32 *ap, register word32 *bp)
{
 register int32 bi, bi2;
 int32 wi, pswid, nd_itpop, wlen, nd_assign, ibase;
 byte *sbp, *sbp2;
 struct gref_t *grp;
 struct xstk_t *xsp, *xsp2;

 pswid = ri1 - ri2 + 1; 
 nd_itpop = FALSE;
 if (idndp->optyp == GLBREF)
  { grp = idndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }

 if (np->n_stren)
  {
   sbp = (byte *) ap;
   get_stwire_addr_(sbp2, np);

   /* trick is to replace forced bits so new assign is same as forced val */
   /* if all bits forced, do not need assign */
   ibase = __inum*np->nwid;
   for (nd_assign = FALSE, bi = ri2, bi2 = 0; bi2 < pswid; bi++, bi2++)
    {
     /* some bits not forced - so need assign */
     if (np->nu2.qcval[ibase + bi].qc_active) sbp[bi] = sbp2[bi];
     else nd_assign = TRUE;
    }
   if (nd_itpop) __pop_itstk();
   return(nd_assign);
  }

 push_xstk_(xsp, pswid);
 push_xstk_(xsp2, np->nwid);
 __bld_forcedbits_mask(xsp2->ap, np);
 
 /* xsp has part select range forced bits */
 __rhspsel(xsp->ap, xsp2->ap, ri2, pswid);
 __pop_xstk();

 /* if all bits forced nothing to do */
 if (__vval_is1(xsp->ap, pswid))
  { __pop_xstk(); if (nd_itpop) __pop_itstk(); return(FALSE); }
 /* if no bits in range forced, just do lhs part select */
 if (vval_is0_(xsp->ap, pswid))
  { __pop_xstk(); if (nd_itpop) __pop_itstk(); return(TRUE); }

 push_xstk_(xsp2, pswid);
 /* xsp2 has value of old wire part selected range */
 __ld_psel(xsp2->ap, xsp2->bp, np, ri1, ri2); 
 wlen = wlen_(pswid);

 /* this changes new value so lhs part select will set right value */
 for (wi = 0; wi < wlen; wi++) 
  {
   /* remove forced bits from new value */
   ap[wi] &= ~(xsp->ap[wi]);   
   bp[wi] &= ~(xsp->ap[wi]);
   /* remove non forced bits from old (current value) */
   xsp2->ap[wi] &= xsp->ap[wi];
   xsp2->bp[wi] &= xsp->ap[wi];
   /* or old value forced into new value */
   ap[wi] |= xsp2->ap[wi]; 
   bp[wi] |= xsp2->ap[wi]; 
  }
 __pop_xstk();
 __pop_xstk();
 if (nd_itpop) __pop_itstk();
 return(TRUE);
}

/*
 * schedule assignment to a part select
 * know xsp width exactly matches part select range
 */
static void schedassign_to_psel(struct expr_t *xlhs, register word32 *ap,
 register word32 *bp)
{
 register int32 bi, bi2;
 int32 ri1, ri2, pslen, nd_itpop;
 byte *sbp, *sbp2;
 word32 oav, obv, aval, bval; 
 struct net_t *np;
 struct expr_t *idndp, *ndx1, *ndx2;
 struct gref_t *grp;

 idndp = xlhs->lu.x;
 np = idndp->lu.sy->el.enp;

 ndx1 = xlhs->ru.x->lu.x;
 ri1 = (int32) __contab[ndx1->ru.xvi];
 ndx2 = xlhs->ru.x->ru.x;
 ri2 = (int32) __contab[ndx2->ru.xvi];

 nd_itpop = FALSE;
 if (idndp->optyp == GLBREF)
  { grp = idndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
 pslen = ri1 - ri2 + 1;
 if (np->n_stren)
  {
   sbp = (byte *) ap;
   /* get strength wire address */
   get_stwire_addr_(sbp2, np);
   for (bi = ri2, bi2 = 0; bi < ri2 + pslen; bi++, bi2++)
    {
     if (np->nu.rngdwir->n_delrep == DT_PTHDST)
      schd_1pthwirebit(np, bi, (word32) sbp[bi], (word32) sbp2[bi2]);
     else __wdel_schd_1wirebit(np, bi, (word32) sbp[bi], (word32) sbp2[bi2],
      FALSE);
    }
  }
 else
  {
   /* schedule for each bit */
   for (bi = ri2; bi < ri2 + pslen; bi++)
    {
     /* isolate rhs bit */
     aval = rhsbsel_(ap, bi);
     bval = rhsbsel_(bp, bi);
     aval |= (bval << 1);
     /* load old bit from wire */
     __ld_bit(&oav, &obv, np, bi); 
     if (np->nu.rngdwir->n_delrep == DT_PTHDST)
      schd_1pthwirebit(np, bi, aval, (oav | (obv << 1)));
     else __wdel_schd_1wirebit(np, bi, aval, (oav | (obv << 1)), FALSE);
    }
  }
 if (nd_itpop) __pop_itstk();
}

/*
 * part select numbits from swp (starting at 0) into dwp starting at dbi
 * assume swp part starts at bit 0
 * preserves high unused bits of new high word32 of dwp
 */
extern void __lhspsel(register word32 *dwp, register int32 dbi,
 register word32 *swp, register int32 numbits)
{
 register int32 wi;

 /* correct so part select goes into 1st word32 */
 if (dbi >= WBITS)
  { wi = get_wofs_(dbi); dwp = &(dwp[wi]); dbi = get_bofs_(dbi); }
 /* if swp too short must correct */
 if (dbi == 0) ins_walign(dwp, swp, numbits);
 else __ins_wval(dwp, dbi, swp, numbits);
}

/*
 * insert aligned on word32 boudary dest. dwp numbits from swp 
 * preserves any unused high bits of high destination word
 * and ignored any used high bits of swp (wider than numbits)
 * if swp too narror correction made before here
 */
static void ins_walign(register word32 *dwp, register word32 *swp,
 register int32 numbits)
{
 register int32 ubits, wlen;
 word32 save_val;

 if (numbits <= WBITS)
  {
   /* preserve high bits and zero low */
   *dwp &= ~__masktab[numbits];
   /* or in new low bits - aligned to low (right) bits of word32 */
   /* if high bits of swp word32 used - will be masked off here */
   *dwp |= (*swp & __masktab[numbits]);
  }
 else
  {
   ubits = ubits_(numbits);
   /* need set brackets here since macro is multiple statements */
   if (ubits == 0) { cp_walign_(dwp, swp, numbits); }
   else
    {
     wlen = wlen_(numbits);
     save_val = dwp[wlen - 1] & ~__masktab[ubits];
     /* in case high word32 of swp has 0 bits (too wide), */
     /* copy will mask those bits off - high word32 ignored */
     cp_walign_(dwp, swp, numbits);
     dwp[wlen - 1] |= save_val;
    }
  }
}

/*
 * insert value into dwp at index dbi from swp with length sblen
 * assume swp part starts at bit 0 but dwp and dbi corrected so dbi < WBITS
 * preserves high unused bits of new high word32 of dwp
 */
extern void __ins_wval(register word32 *dwp, register int32 dbi,
 register word32 *swp, int32 numbits)
{
 register word32 save_val, mask;
 int32 wlen, w2bits;

 /* case 1a - fits in 1st actual word32 */
 if (dbi + numbits <= WBITS)
  {
   mask = __masktab[numbits] << dbi;
   *dwp = (*dwp & ~mask) | ((*swp << dbi) & mask);
   return;
  }
 /* case 2 - less than 32 bits but crosses word32 boundary */
 if (numbits <= WBITS)
  {
   /* preserve low bits */
   *dwp = (*dwp & __masktab[dbi]);
   *dwp |= (*swp << dbi);

   w2bits = numbits - (WBITS - dbi);
   dwp[1] = (dwp[1] & ~__masktab[w2bits])
    | ((*swp >> (WBITS - dbi)) & __masktab[w2bits]);
   return;
  }
 /* case 3 - general multiword case */
 w2bits = numbits + dbi;
 /* w2bits is length in bits starting from start of dest word32 */
 wlen = wlen_(w2bits);
 save_val = dwp[wlen - 1] & ~__masktab[ubits_(w2bits)];
 cp_dofs_wval(dwp, swp, dbi, numbits);
 dwp[wlen - 1] |= save_val;
}

/*
 * routine to copy one value to another assuming dest. 1st bit is non 0
 * but copies from source bit 0
 * notice preserves bits to right of dbit1 but does not preserve high
 * bits of high word
 * dbit1 is offset in 1st word32 of dest. (>0 and <WBITS)
 *
 * copy from non 0 bit of source to 0 bitb of destionation use cp_sofs_wval
 *
 * chgs high unused bits of dest. word32 to 0's - caller must save if needed
 * this should probably be macro
 */
static void cp_dofs_wval(register word32 *dwp, register word32 *swp,
 int32 dbit1, int32 numbits)
{
 int32 dbit2;

 /* do 1st word32 as special case */
 dwp[0] &= __masktab[dbit1];
 dwp[0] |= (swp[0] << dbit1);
 dbit2 = WBITS - dbit1;
 if (dbit2 >= numbits) return;
 numbits -= dbit2;
 __cp_sofs_wval(&(dwp[1]), swp, dbit2, numbits);
}

/*
 * copy one value to another assuming source 1st bit is non 0
 * but copies into destination starting at bit 0
 * sbit1 is offset in 1st word32 (>0 and <WBITS)
 *
 * to copy into dwp non bit 0 copy use cp_dofs_wval
 * and then use this routine with WBITS - dbit1 for rest
 *
 * chgs high unused bits of dest. word32 to 0's - caller must save and restore
 * this should probably be macro
 */
extern void __cp_sofs_wval(register word32 *dwp, register word32 *swp,
 register int32 sbit1, register int32 numbits)
{
 register int32 wi, bi, sbit2;

 sbit2 = WBITS - sbit1;
 for (bi = 0, wi = 0; ; wi++)
  {
   dwp[wi] = (swp[wi] >> sbit1);
   if ((bi += sbit2) >= numbits) break;
   dwp[wi] |= (swp[wi + 1] << sbit2);
   if ((bi += sbit1) >= numbits) break;
  }
 /* bits in high source word32 will probably be on but must not be selected */
 dwp[wi] &= __masktab[ubits_(numbits)];
}

/*
 * non change store assign to psel
 *
 * SJM 12/14/05 - can't use chg form for non chg since may wrongly leave
 * lhs changed on so next one wrongly appears to be changed
 */
static void st_psel(struct net_t *np, int32 ri1, int32 ri2,
 register word32 *ap, register word32 *bp)
{
 int32 numbits, wlen;
 word32 *rap;
 byte *netsbp, *newsbp;

 numbits = ri1 - ri2 + 1;
 if (np->srep == SR_VEC)
  {
   /* SJM 07/15/00 - all non mem vecs in at least 2 wrds - scalars in byte */
   /* while this needs words since always some number of words */
   wlen = wlen_(np->nwid);
   rap = &(np->nva.wp[2*wlen*__inum]);
   __lhspsel(rap, ri2, ap, numbits);
   rap = &(rap[wlen]);
   __lhspsel(rap, ri2, bp, numbits);
  }
 else
  {
   netsbp = &(np->nva.bp[np->nwid*__inum + ri2]);
   newsbp = (byte *) ap;
   memcpy(netsbp, newsbp, numbits);
  }
}

/*
 * change store a part select - only for non strength vector
 *
 * LOOKATME - notice no non chg form store into psel
 */ 
static void chg_st_psel(struct net_t *np, int32 ri1, int32 ri2,
 register word32 *ap, register word32 *bp)
{
 register int32 bi, bi2;
 int32 pslen;
 byte *netsbp, *newsbp;

 /* SJM 07/15/00 - now all non mem vecs in at least 2 wrds - scalars in byte */
 /* while this needs words since always some number of words */
 if (np->srep == SR_VEC)
  chg_st_unpckpsel(np->nva.wp, np->nwid, ri1, ri2, ap, bp);
 else
  {
   netsbp = &(np->nva.bp[np->nwid*__inum]);
   newsbp = (byte *) ap;
   pslen = ri1 - ri2 + 1;
   /* case 1: part select size same or narrower than rhs - truncation */
   for (bi = ri2, bi2 = 0; bi2 < pslen; bi++, bi2++)
    {
     if (netsbp[bi] != newsbp[bi2])
      { netsbp[bi] = newsbp[bi2]; __lhs_changed = TRUE; } 
    }
  }
 /* notice since know dce and npps never ranges unless possible */
 /* do not need to correct vectored or reg subrange to entire wire */
 /* SJM 07/24/00 - because only ch store psel - no record if no dces/lds */ 
 /* SJM 03/15/01 - change to fields in net record */
 if (__lhs_changed && np->nchg_nd_chgstore)
  {
   record_sel_nchg_(np, ri1, ri2);
  }
}

/*
 * change version of store psel for unpacked vector
 */
static void chg_st_unpckpsel(word32 *wp, int32 blen, int32 bith, int32 bitl,
 register word32 *ap, register word32 *bp)
{
 register word32 *rap;
 int32 wlen, numbits;

 /* this is rightmost in word32, leftmost among words bit */
 numbits = bith - bitl + 1;
 wlen = wlen_(blen);
 rap = &(wp[2*wlen*__inum]);
 __chg_lhspsel(rap, bitl, ap, numbits);
 rap = &(rap[wlen]);
 __chg_lhspsel(rap, bitl, bp, numbits);
}

/*
 * only store if change form of lhs psel
 */
extern void __chg_lhspsel(register word32 *dwp, register int32 dbi,
 register word32 *swp, register int32 numbits)
{
 register int32 wi;
 int32 wlen, ubits;

 /* correct so part select goes into 1st word32 */
 if (dbi >= WBITS)
  { wi = get_wofs_(dbi); dwp = &(dwp[wi]); dbi = ubits_(dbi); }
 /* if swp too short must correct */
 if (dbi == 0)
  {
   if (numbits <= WBITS)
    {
     if ((dwp[0] & __masktab[numbits]) != (swp[0] & __masktab[numbits])) 
      {
       ins_walign(dwp, swp, numbits);
       __lhs_changed = TRUE;
      }
    }
   else
    {
     wlen = wlen_(numbits);
     ubits = ubits_(numbits);

     /* if any differences copy all */
     /* when comparing high word32 - dest. high bits are masked off */
     if ((dwp[wlen - 1] & __masktab[ubits])
      != (swp[wlen - 1] & __masktab[ubits]) 
      || memcmp(dwp, swp, WRDBYTES*(wlen - 1)) != 0) 
      {
       ins_walign(dwp, swp, numbits);
       __lhs_changed = TRUE;
      }
    }
   return;
  }
 /* unaligned case */
 chg_ins_wval(dwp, dbi, swp, numbits);
}

/*
 * only assign if changed form of unaligned ins_wval 
 */
static void chg_ins_wval(register word32 *dwp, register int32 dbi,
 register word32 *swp, register int32 numbits)
{
 register word32 mask;
 word32 save_val;
 int32 wlen, w2bits;

 /* case 1a - fits in 1st actual word32 */
 if (dbi + numbits <= WBITS)
  {
   mask = __masktab[numbits] << dbi;
   save_val = (swp[0] << dbi) & mask;
   if ((dwp[0] & mask) != save_val)
    { dwp[0] = (dwp[0] & ~mask) | save_val; __lhs_changed = TRUE; }
   return;
  }
 /* case 2 - less than 32 bits but crosses word32 boundary */
 if (numbits <= WBITS)
  {
   w2bits = numbits - (WBITS - dbi);

   /* compare 1st word32 dbi to 31 versus 32 - dbi low bits of source */ 
   /* compare 2nd word32 numbits - 32 - dbi  to dbi to numbits of source */
   mask = __masktab[w2bits];
   if ((dwp[0] & ~__masktab[dbi]) != (swp[0] << dbi) ||
    ((dwp[1] & mask) != ((swp[0] >> (WBITS - dbi)) & mask)))
    {
     /* remove high (WBITS - dbi) bits (0s) */
     dwp[0] &= (dwp[0] & __masktab[dbi]);
     /* or into 0's low (WBITS - dbi) into range [31:dbi] */
     dwp[0] |= (swp[0] << dbi);
     /* remove low w2bits and or in all but low (WBITS - dbi) from new */
     dwp[1] = (dwp[1] & ~mask) | ((swp[0] >> (WBITS - dbi)) & mask);
     __lhs_changed = TRUE;
    }
   return;
  }

 /* case 3 - general multiword case */
 if (chg_ofs_cmp(dwp, swp, dbi, numbits) == 0) return;

 /* SJM 03/29/02 - handles restoring high big problem by saving and putting */
 /* back - these copies always zero high bits when done so or works */
 w2bits = numbits + dbi;
 /* w2bits is length in bits starting from start of dest word32 */
 wlen = wlen_(w2bits);
 save_val = dwp[wlen - 1] & ~__masktab[ubits_(w2bits)];
 cp_dofs_wval(dwp, swp, dbi, numbits);
 dwp[wlen - 1] |= save_val;
 __lhs_changed = TRUE;
}

/*
 * compare source versus destination ofset by dbi bits
 * returns 0 for equal 1 for not
 * know source (new) always aligned to bit 0
 *
 * 03/17/02 - changed so saves and zeros high bits so compe works  
 */
static int32 chg_ofs_cmp(register word32 *dwp, register word32 *swp,
 int32 dbi, int32 numbits)
{
 register int32 bi; 
 register word32 mask1, mask2;
 int32 wi, sbit2, w2bits, w2len, chged;
 word32 save_val;

 sbit2 = WBITS - dbi;

 mask1 = __masktab[dbi];
 mask2 = __masktab[sbit2];

 w2bits = numbits + dbi;
 w2len = wlen_(w2bits);
 /* trick here is to save and set high bits to 0's so do not need */
 /* high bit special case and can use saved val if copy needed */
 save_val = dwp[w2len - 1] & ~__masktab[ubits_(w2bits)];
 dwp[w2len - 1] &= __masktab[ubits_(w2bits)];

 /* assume chged */
 chged = 1;

 /* 1st word32 is special case */
 if ((dwp[0] & ~__masktab[dbi]) != (swp[0] << dbi)) goto done_putback;
 numbits -= sbit2;

 for (bi = 0, wi = 1; ; wi++)
  {
   if ((dwp[wi] & mask1) != ((swp[wi - 1] >> sbit2) & mask1))
    goto done_putback;
   if ((bi += dbi) >= numbits) break;

   if (((dwp[wi] >> dbi) & mask2) != (swp[wi] & mask2))
    goto done_putback;
   if ((bi += sbit2) >= numbits) break;
  }
 chged = 0;

done_putback:
 /* finally put back high bits of dest word32 */
 dwp[w2len - 1] |= save_val;
 return(chged); 
}

/* SJM 07/15/00 - now 2 to 16 bit vectors not packed - in 2 words */

/*
 * SIZE CHANGE AND Z EXTENSION ROUTINES
 */

/*
 * widen or narrow a stack value (only for rhs exprs)
 * know bit widths differ or will not be called
 * this may need to widen stack value width (alloc-free) 
 * also if widens zeros all bits
 *
 * this is not for z filling but for normal operators where no z filling
 * allowed - may z fill in assignment after this size change done
 * also does not work for strengths
 */
extern void __sizchgxs(register struct xstk_t *xsp, int32 nblen)
{
 register int32 wi;
 register word32 *wpna, *wpnb, *wpob;
 int32 nwlen, nubits, stkwlen, xtrabits;

 /* case 1: widening */
 if (xsp->xslen < nblen)
  {
   /* case 1a: stays in one word32 */ 
   if (nblen <= WBITS) goto done;
   nwlen = wlen_(nblen);
   /* case 1b: multiword but into same number of words - does nothing */
   stkwlen = wlen_(xsp->xslen);
   if (nwlen == stkwlen) goto done;
   nubits = ubits_(nblen);
   /* case 1c: wider needs bigger area */
   if (nwlen > xsp->xsawlen)
    {
     /* SJM 05/16/04 sign change was wrong - need to copy low parts */ 
     /* of a and b separately */
     wpna = (word32 *) __my_malloc(2*WRDBYTES*nwlen);
     memcpy(wpna, xsp->ap, WRDBYTES*xsp->xsawlen);
     /* SJM 09/29/04 widening b part is new wlen offset from a part */
     /* SJM 10/02/04 wasn't fixed right was using wrong old xsp ap part */
     wpnb = &(wpna[nwlen]);
     memcpy(wpnb, xsp->bp, WRDBYTES*xsp->xsawlen);

     __my_free((char *) xsp->ap, 2*WRDBYTES*xsp->xsawlen);
     xsp->ap = wpna;
     xsp->bp = wpnb;
     xsp->xsawlen = nwlen;
    }
   else
    { 
     /* case 1d: wider by adjusting loc in region of b part and copying */
     wpob = xsp->bp;
     /* SJM 09/29/04 widening b part should be new wlen offset from a part */
     wpnb = &(xsp->ap[nwlen]);

     /* must copy high to low to preserve high old */ 
     for (wi = stkwlen - 1; wi >= 0; wi--) wpnb[wi] = wpob[wi]; 
     xsp->bp = wpnb; 
    }
   /* 0 wide new high bits */
   xtrabits = (nblen - WBITS*stkwlen);
   zero_allbits_(&(xsp->ap[stkwlen]), xtrabits);
   zero_allbits_(&(xsp->bp[stkwlen]), xtrabits);
   goto done;
  }
 /* case 2 narrowing case - know cannot be 1 bit to start */
 /* case 2a: narrow to 1 bit */
 nwlen = wlen_(nblen);
 nubits = ubits_(nblen);
 stkwlen = wlen_(xsp->xslen);
 /* case 2b: narrowing where narrower same number of words */
 if (stkwlen == nwlen)
  {
   xsp->ap[nwlen - 1] &= __masktab[nubits];
   xsp->bp[nwlen - 1] &= __masktab[nubits];
   goto done;
  }
 /* case 2c: general narrowing */
 /* SJM 09/29/04 widening b part should be new wlen offset from a part */
 wpnb = &(xsp->ap[nwlen]);
 wpob = xsp->bp;
 /* need loop because must make sure copy low first */
 for (wi = 0; wi < nwlen; wi++) wpnb[wi] = wpob[wi];
 xsp->bp = wpnb;
 xsp->ap[nwlen - 1] &= __masktab[nubits];
 xsp->bp[nwlen - 1] &= __masktab[nubits];
done:
 xsp->xslen = nblen;
}

/*
 * zero widen a stack value (only for rhs exprs) - not for sign extend
 * know need to widen or will not be called
 * this may need to widen stack value width (alloc-free) 
 * also if widens zeros all bits (not for sign extend widening)
 */
extern void __sizchg_widen(register struct xstk_t *xsp, int32 nblen)
{
 register int32 wi;
 register word32 *wpna, *wpnb, *wpob;
 int32 nwlen, nubits, stkwlen, xtrabits;

 /* case 1: widening */
 if (nblen <= WBITS) { xsp->xslen = nblen; return; }
 nwlen = wlen_(nblen);
 /* case 2: multiword but into same number of words - does nothing */
 stkwlen = wlen_(xsp->xslen);
 if (nwlen == stkwlen) { xsp->xslen = nblen; return; }
 nubits = ubits_(nblen);
 /* case 1c: wider needs bigger area */
 if (nwlen > xsp->xsawlen)
  {
   /* SJM 05/16/04 - 9-29 sign change was wrong - need to copy low parts */ 
   /* of a and b separately */
   wpna = (word32 *) __my_malloc(2*WRDBYTES*nwlen);
   memcpy(wpna, xsp->ap, WRDBYTES*xsp->xsawlen);
   /* SJM 09/29/04 widening b part should be new wlen offset from a part */
   /* SJM 10/02/04 wasn't fixed right was using wrong old xsp ap part */
   wpnb = &(wpna[nwlen]);
   memcpy(wpnb, xsp->bp, WRDBYTES*xsp->xsawlen);

   __my_free((char *) xsp->ap, 2*WRDBYTES*xsp->xsawlen);
   xsp->ap = wpna;
   xsp->bp = wpnb;
   xsp->xsawlen = nwlen;
  }
 else
  { 
   /* case 1d: wider by adjusting loc in region of b part and copying */
   wpob = xsp->bp;
   /* SJM 09/29/04 widening b part should be new wlen offset from a part */
   wpnb = &(xsp->ap[nwlen]);
   /* must copy high to low to preserve high old */ 
   for (wi = stkwlen - 1; wi >= 0; wi--) wpnb[wi] = wpob[wi]; 
   xsp->bp = wpnb; 
  }
 /* 0 wide new high bits */
 xtrabits = (nblen - WBITS*stkwlen);
 zero_allbits_(&(xsp->ap[stkwlen]), xtrabits);
 zero_allbits_(&(xsp->bp[stkwlen]), xtrabits);
 xsp->xslen = nblen;
}

/*
 * widen a stack using signed extension (narrow case handled in other routine)
 *
 * know need widening (maybe within one word32) - checks and extends sign
 * if needed
 *
 * this may need to widen stack value width (alloc-free) but never pops/pushes
 * also routine does not work for strens
 */
extern void __sgn_xtnd_widen(struct xstk_t *xsp, int32 nblen)
{
 register int32 wi, osgn_bofs;
 register word32 mask;
 word32 *wpna, *wpnb, *wpob;
 int32 nwlen, stkwlen, widen_amt, xtra_wbits, ival;

 /* case 1: stays in one word32 */ 
 if (nblen <= WBITS)
  {
   osgn_bofs = xsp->xslen - 1;
   /* if signed, sign extend, otherwise nothing to do */
   if ((xsp->ap[0] & (1 << (osgn_bofs))) != 0)
    {
     /* AIV 01/18/06 - added parenthesis around minus amount */
     mask = __masktab[nblen - (osgn_bofs + 1)] << (osgn_bofs + 1);
     xsp->ap[0] |= mask;
     /* if x/z x/z extend */
     if ((xsp->bp[0] & (1 << (osgn_bofs))) != 0) xsp->bp[0] |= mask;
    }
   else
    {
     if ((xsp->bp[0] & (1 << (osgn_bofs))) != 0)
      {
       /* since sign bit off, 0 extend a part but if z, z extend b part */
       mask = __masktab[nblen - (osgn_bofs + 1)] << (osgn_bofs + 1);
       if ((xsp->bp[0] & (1 << (osgn_bofs))) != 0) xsp->bp[0] |= mask;
      }
    }
   xsp->xslen = nblen;
   return;
  }
 nwlen = wlen_(nblen);
 /* case 2: multiword but into same number of words */
 stkwlen = wlen_(xsp->xslen);

 if (nwlen == stkwlen)
  {
   osgn_bofs = get_bofs_(xsp->xslen - 1);

   /* if signed, sign extend, otherwise nothing to do */
   /* notice nwlen and stkwlen same */
   if ((xsp->ap[nwlen - 1] & (1 << osgn_bofs)) != 0)
    {
     /* AIV 12/22/06 - masktab index was wrong */
     mask = ((__masktab[WBITS - (osgn_bofs + 1)]) << (osgn_bofs + 1));
     xsp->ap[nwlen - 1] |= mask;
     /* SJM 09/29/04 - was checking word32 after high end not high word32 */
     /* if x/z, x/z extend */
     if ((xsp->bp[nwlen - 1] & (1 << (osgn_bofs))) != 0)
      xsp->bp[nwlen - 1] |= mask;
    }
   else
    {
     /* AIV 01/10/07 - was using bp[0] - wrong for wide case */
     if ((xsp->bp[nwlen - 1] & (1 << (osgn_bofs))) != 0)
      {
       /* AIV 12/22/06 - masktab index was wrong */
       mask = ((__masktab[WBITS - (osgn_bofs + 1)]) << (osgn_bofs + 1));
       /* SJM 09/29/04 - was masking word32 after high end not high word32 */
       xsp->bp[nwlen - 1] |= mask;
      }
    }
   xsp->xslen = nblen;
  }

 /* case 3: wider - first create the larger area */
 if (nwlen > xsp->xsawlen)
  {
   /* SJM 05/16/04 - 9-29 sign change was wrong - need to copy low parts */ 
   /* of a and b separately */
   wpna = (word32 *) __my_malloc(2*WRDBYTES*nwlen);
   memcpy(wpna, xsp->ap, WRDBYTES*xsp->xsawlen);
   /* SJM 09/29/04 widening b part should be new wlen offset from a part */
   wpnb = &(wpna[nwlen]);
   memcpy(wpnb, xsp->bp, WRDBYTES*xsp->xsawlen);

   __my_free((char *) xsp->ap, 2*WRDBYTES*xsp->xsawlen);
   xsp->ap = wpna;
   xsp->bp = wpnb;
   xsp->xsawlen = nwlen;
  }
 else
  { 
   /* case 1d: wider by adjusting loc in region of b part and copying */
   wpob = xsp->bp;
   /* SJM 09/29/04 widening b part should be new wlen offset from a part */
   wpnb = &(xsp->ap[nwlen]);
   /* must copy high to low to preserve high old */ 
   for (wi = stkwlen - 1; wi >= 0; wi--) wpnb[wi] = wpob[wi]; 
   xsp->bp = wpnb; 
  }

 /* widen amount is number of bits to set to 1 (sign extend into) */
 widen_amt = nblen - xsp->xslen; 
 /* this is position in old narrower value */
 osgn_bofs = get_bofs_(xsp->xslen - 1);
 /* xtra bits is bit num bits with high bits of sign words subtracted */
 xtra_wbits = widen_amt - (WBITS - osgn_bofs - 1);

 /* AIV 06/23/05 - special case don't check for sign if 32 bits */
 /* just cast to int and copy high part */
 if (xsp->xslen == WBITS)
  {
   ival = (int32) xsp->ap[0];
   if (ival < 0) one_allbits_(&(xsp->ap[stkwlen]), xtra_wbits);
   else zero_allbits_(&(xsp->ap[stkwlen]), xtra_wbits);
   ival = (int32) xsp->bp[0];
   if (ival < 0) one_allbits_(&(xsp->bp[stkwlen]), xtra_wbits);
   else zero_allbits_(&(xsp->bp[stkwlen]), xtra_wbits);
   xsp->xslen = nblen;
   return;
  }

 /* now can set new widened size */
 xsp->xslen = nblen;

 /* sign extend if sign bit on, x/z extend if sign bit x/z, else 0 extend */
 if ((xsp->ap[stkwlen - 1] & (1 << osgn_bofs)) != 0)
  {
   mask = __masktab[WBITS - (osgn_bofs + 1)] << (osgn_bofs + 1);
   /* one high bits of this word32 */
   xsp->ap[stkwlen - 1] |= mask;
   /* then all bits of rest */
   one_allbits_(&(xsp->ap[stkwlen]), xtra_wbits);

   /* if x/z x/z extend */
   if ((xsp->bp[stkwlen - 1] & (1 << osgn_bofs)) != 0)
    {
     xsp->bp[stkwlen - 1] |= mask;
     one_allbits_(&(xsp->bp[stkwlen]), xtra_wbits);
    } 
   /* know high bits of high old size word32 0, but 0 all new words */
   else zero_allbits_(&(xsp->bp[stkwlen]), xtra_wbits);
   return;
  }
 /* a part sign bit off, 0 all high a part words */
 zero_allbits_(&(xsp->ap[stkwlen]), xtra_wbits);
 if ((xsp->bp[stkwlen - 1] & (1 << osgn_bofs)) != 0)
  {
   mask = __masktab[WBITS - (osgn_bofs +1)] << (osgn_bofs + 1);
   xsp->bp[stkwlen - 1] |= mask;
   one_allbits_(&(xsp->bp[stkwlen]), xtra_wbits);
   return;
  }
 /* 0 wide new high bits */
 zero_allbits_(&(xsp->bp[stkwlen]), xtra_wbits);
}

/*
 * sign extend widen within one word
 */
extern void __sgn_xtnd_wrd(register struct xstk_t *xsp, int32 nblen)
{
 register int32 oubits;
 register word32 mask;

 oubits = xsp->xslen;
 /* if signed, sign extend, otherwise nothing to do */
 if ((xsp->ap[0] & (1 << (oubits - 1))) != 0)
  {
   mask = (__masktab[WBITS - oubits]) << oubits;
   xsp->ap[0] |= mask;
   /* if x/z x/z extend */
   /* AIV - 10/10/05 - b part was & should be | */
   if ((xsp->bp[0] & (1 << (oubits - 1))) != 0) xsp->bp[0] |= mask;
  }
 else
  {
   mask = (__masktab[WBITS - oubits]) << oubits;
   if ((xsp->bp[0] & (1 << (oubits - 1))) != 0)
    xsp->bp[0] |= mask;
  }
 xsp->xslen = nblen;
}

/*
 * special case narrow to 1 routine 
 */
extern void __narrow_to1bit(register struct xstk_t *xsp)
{
 register int32 stkwlen;

 stkwlen = wlen_(xsp->xslen);
 /* case 1: narrowing within one word32 */
 if (stkwlen == 1) { xsp->ap[0] &= 1; xsp->bp[0] &= 1; }
 else
  {
   /* case 2: wide to 1 bit narrow */
   xsp->ap[0] &= 1; 
   xsp->ap[1] = xsp->bp[0] & 1;
   xsp->bp = &(xsp->ap[1]); 
  }
 xsp->xslen = 1;
}

/*
 * special case narrow to WBITS routine 
 */
extern void __narrow_to1wrd(register struct xstk_t *xsp)
{
 register int32 stkwlen;

 stkwlen = wlen_(xsp->xslen);
 /* DBG remove -- */
 if (stkwlen == 1) __misc_terr(__FILE__, __LINE__);
 /* --- */
 xsp->ap[1] = xsp->bp[0];
 xsp->bp = &(xsp->ap[1]); 
 xsp->xslen = WBITS;
}

/*
 * narrow a stack value (only for rhs exprs)
 * know need to narrow or will not be called
 */
extern void __narrow_sizchg(register struct xstk_t *xsp, int32 nblen)
{
 register int32 wi;
 register word32 *wpnb, *wpob;
 int32 nwlen, nubits, stkwlen;

 /* know cannot be 1 bit to start */
 nwlen = wlen_(nblen);
 nubits = ubits_(nblen);
 stkwlen = wlen_(xsp->xslen);
 /* case 2b: narrowing where narrower same number of words */
 if (stkwlen == nwlen)
  {
   xsp->ap[nwlen - 1] &= __masktab[nubits];
   xsp->bp[nwlen - 1] &= __masktab[nubits];
   xsp->xslen = nblen;
   return;
  }
 /* case 2c: general narrowing */
 /* SJM 09/29/04 widening b part should be new wlen offset from a part */
 wpnb = &(xsp->ap[nwlen]);
 wpob = xsp->bp;
 /* need loop because must make sure copy low first */
 /* this insures a/b parts contiguous */ 
 for (wi = 0; wi < nwlen; wi++) wpnb[wi] = wpob[wi];
 xsp->bp = wpnb;
 xsp->ap[nwlen - 1] &= __masktab[nubits];
 xsp->bp[nwlen - 1] &= __masktab[nubits];
 xsp->xslen = nblen;
}

/*
 * after widening stack, z extend new high bits if needed
 * know nblen greater than oblen
 *
 * this is only for non stren case since stren never assigned only added
 * from something that drives strength (gates are exception and handled as
 * special case elsewhere)
 */
extern void __fix_widened_tozs(struct xstk_t *xsp, int32 oblen)
{
 register int32 wi;
 int32 nblen, owlen, nwlen, oubits, nubits;

 /* case 1: same number of words */
 nblen = xsp->xslen;
 nubits = ubits_(nblen);
 oubits = ubits_(oblen);
 owlen = wlen_(oblen);
 nwlen = wlen_(nblen);
 if (owlen == nwlen)
  {
   xsp->bp[owlen - 1] |= (__masktab[nubits] & ~__masktab[oubits]); 
   return;
  }
 /* case 2 widen to more words */
 /* set b part high bits of high old word32 to 1s */ 
 xsp->bp[owlen - 1] |= ~__masktab[oubits];
 /* set b part of all high words */
 for (wi = owlen; wi < nwlen; wi++) xsp->bp[wi] = ALL1W;
 /* except unused high bit of new high word32 */
 xsp->bp[nwlen - 1] &= __masktab[nubits];
}

/*
 * after widening stack, x extend new high bits if needed
 * know nblen greater than oblen
 * this is only for non strength initialization case
 */
extern void __fix_widened_toxs(register struct xstk_t *xsp, int32 oblen)
{
 register int32 wi;
 int32 nblen, owlen, nwlen, oubits, nubits;

 /* case 1: same number of words */
 nblen = xsp->xslen;
 nubits = ubits_(nblen);
 oubits = ubits_(oblen);
 owlen = wlen_(oblen);
 nwlen = wlen_(nblen);
 if (owlen == nwlen)
  {
   xsp->ap[owlen - 1] |= (__masktab[nubits] & ~__masktab[oubits]); 
   xsp->bp[owlen - 1] |= (__masktab[nubits] & ~__masktab[oubits]); 
   return;
  }
 /* case 2 widen to more words */
 /* set b part high bits of high old word32 to 1s */ 
 /* SJM 02/18/03 WRONG - also need to mask in 1's for a part */
 /* code came from widen to z's so a part was missing */ 
 xsp->ap[owlen - 1] |= ~__masktab[oubits];
 xsp->bp[owlen - 1] |= ~__masktab[oubits];
 /* set all high words to x */
 for (wi = owlen; wi < nwlen; wi++) xsp->ap[wi] = ALL1W;
 for (wi = owlen; wi < nwlen; wi++) xsp->bp[wi] = ALL1W;
 /* except unused high bit of new high word32 */
 xsp->ap[nwlen - 1] &= __masktab[nubits];
 xsp->bp[nwlen - 1] &= __masktab[nubits];
}

/*
 * widen a stacked strength byte value (only for rhs exprs)
 * know bit widths differ or will not be called
 * this may need to widen stack value width (alloc-free) 
 * also if widens z's 00z all bits
 *
 * 07/08/00 SJM - also used to widen fi>1 strength driver competition results
 *
 * for strength all extension must be z
 * for narrow just adjust xslen 
 */
extern void __strenwiden_sizchg(struct xstk_t *xsp, int32 nblen)
{
 register int32 bi;
 byte *sbp, *sbp2;
 int32 oblen, numavailbytes, wlen;

 /* DBG remove -- */
 if ((xsp->xslen % 4) != 0) __arg_terr(__FILE__, __LINE__);
 /* --- */
 oblen = xsp->xslen/4;
 sbp = (byte *) xsp->ap;
 /* case 1: widening */
 if (oblen < nblen)
  {
   numavailbytes = 2*WRDBYTES*xsp->xsawlen; 
   if (numavailbytes < nblen)
    {
     /* just widen to enough room plus 1 word32 */
     wlen = (nblen + WRDBYTES - 1)/WRDBYTES + 1;
     /* SJM 05/23/03 - freed and realloced with size 2 times - was not */
     /* allocing both a and b parts but was freeing */
     xsp->ap = (word32 *) __my_malloc(2*wlen*WRDBYTES);
     xsp->xsawlen = wlen;
     sbp2 = (byte *) xsp->ap;
     /* LOOKATME - think overlap possible here */
     memmove(sbp2, sbp, oblen); 
     __my_free((char *) sbp, numavailbytes);
     sbp = sbp2;
    }
   for (bi = oblen; bi < nblen; bi++) sbp[bi] = ST_HIZ;
   xsp->xslen = 4*nblen;
  }
 /* case 2: narrow */
 else xsp->xslen = 4*nblen;
}

/*
 * BUILT-IN GATE AND UDP EVALUATION ROUTINES
 */

/*
 * evaluate logic gate - know input changed and changes recorded in gstate
 * set new value in new gateval and if needs strength in new gate stren 
 * and if return T sets old gateval
 * returns F if no change (new value and old the same)
 * gate values here never have strength - maybe added when stored in wire
 * 
 * i is position of gate input expr. starting at 1, bi bit starting at 0
 */
extern int32 __eval_logic_gate(struct gate_t *gp, word32 i, int32 *out_chg)
{
 register word32 *rap, uwrd, ouwrd, ngav, ngbv;
 register word32 gav, gbv, mask;
 int32 gwid, wlen, bi, gatid;
 struct xstk_t *xsp;

 gwid = gp->gpnum;
 xsp = __eval_xpr(gp->gpins[i]);
 bi = i - 1; 
 ngav = xsp->ap[0] & 1L;
 ngbv = xsp->bp[0] & 1L;
 __new_inputval = ngav | (ngbv << 1);
 __pop_xstk();

 /* always set strength even if not needed - always constant here */
 /* rare wide case */
 if (gwid > 16)
  {
   wlen = wlen_(gwid);
   /* rap is base of vector for current inst */
   rap = &(gp->gstate.wp[2*wlen*__inum]);
   chg_lhsbsel(rap, bi, ngav);
   chg_lhsbsel(&(rap[wlen]), bi, ngbv);
   if (!__lhs_changed) return(FALSE);
   /* this set global new and old gate values */
   *out_chg = __eval_1wide_gate(gp, gwid);
   return(TRUE);
  }

 /* eval changed input expr. and store in gstate if needed */
 /* know packed both a and b sections in same word32 */
 /* SJM 12/16/99 still packing gate state as usual */
 ouwrd = get_packintowrd_(gp->gstate, __inum, gwid);
 uwrd = ouwrd & ~(1L << bi) & ~(1L << (gwid + bi));
 uwrd |= ((ngav << bi) | (ngbv << (gwid + bi))); 
 if (uwrd == ouwrd) return(FALSE);
 st_packintowrd_(gp->gstate, __inum, uwrd, gwid);

 /* now need gate id */
 gatid = gp->gmsym->el.eprimp->gateid;
 /* buf (assign buf) or not short circuit */
 /* now reusing ngav/ngbv as new output value no longer new input value */
 /* FIXME not allowing multiple output nots and bufs yet */
 if (gwid == 2)
  {
   /* old gate value b part in 3 and a part in bit 1 */
   __old_gateval = ((uwrd >> 1) & 1L) | ((uwrd >> 2) & 2L); 
   ngbv = (uwrd >> 2) & 1L;
   /* buf and not always convert z to x */ 
   if (gatid == G_NOT) ngav = !(uwrd & 1L) | ngbv;
   else if (gatid == G_BUF) ngav = (uwrd & 1L) | ngbv;
   /* but cont. ASSIGN passes z */
   else if (gatid == G_ASSIGN) ngav = (uwrd & 1L);
   else 
    {
     switch ((byte) gatid) {
      case G_BITREDAND: case G_BITREDOR: case G_BITREDXOR:
       ngav = (uwrd & 1L) | ngbv;
       break;
      case G_NAND: case G_NOR: case G_REDXNOR:
       ngav = !(uwrd & 1L) | ngbv;
       break;
     }
    }
   goto done;
  }

 /* need to handle 2 input gates as partial special case */
 if (gwid == 3)
  {
   mask = __masktab[2];
   gav = uwrd & 3L;
   gbv = (uwrd >> 3) & 3L;
   __old_gateval = ((uwrd >> 2) & 1L) | ((uwrd >> 4) & 2L);
  }
 else
  {
   /* gav and gbv are inputs only */
   mask = __masktab[gwid - 1];
   /* this masks off a/b output bit - but gav/gbv all inputs */
   gav = uwrd & mask;
   gbv = (uwrd >> gwid) & mask;
   /* works since know n ins at least 1 - b shifts 1 less, goes b bit */
   __old_gateval = ((uwrd >> (gwid - 1)) & 1L) | ((uwrd >> (2*gwid - 2)) & 2L); 
  }
 ngav = ngbv = 1L;
 switch ((byte) gatid) {
  case G_BITREDAND:
   /* if even 1 0 value in any used bit, result is 0 */
   if (gbv == 0L) { ngav = (gav != mask) ? 0L : 1L; ngbv = 0L; }
   else if ((gav | gbv) != mask) ngav = ngbv = 0L;
   break;
  case G_NAND:
   /* if even 1 0 value in any used bit, result is 1 */
   if (gbv == 0L) { ngav = (gav != mask) ? 1L : 0L; ngbv = 0L; }
   else if ((gav | gbv) != mask) ngbv = 0L;
   break;
  case G_BITREDOR:
   /* if even 1 1 value in any used bit, result is 1 */
   if (gbv == 0L) { ngav = (gav != 0L) ? 1L : 0L; ngbv = 0L; }
   else if ((gav & ~gbv) != 0L) ngbv = 0L;
   break;
  case G_NOR:
   /* if even 1 1 value in any used bit, result is 0 */
   if (gbv == 0L) { ngav = (gav != 0L) ? 0L : 1L; ngbv = 0L; }
   else if ((gav & ~gbv) != 0L) ngav = ngbv = 0L;
   break;
  case G_BITREDXOR:
   if (gbv == 0L) { ngbv = 0L; ngav = __wrd_redxor(gav); }
   break;
  case G_REDXNOR:
   if (gbv == 0L) { ngbv = 0L; ngav = !__wrd_redxor(gav); }
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 /* these gates can drive constant strengths on wire - handled at assign */
 /* need gate as 2 bit value for delay selection */
done:
 __new_gateval = ngav | (ngbv << 1);
 /* set to T (non 0) if not equal if changed (different) */
 *out_chg = (__old_gateval != __new_gateval);
 return(TRUE);
} 

/*
 * evaluate a gate that has at least 15 inputs
 * could do 15 and 16 input gates slightly better since gate inputs fit
 * in 1 word32 but stored as 2 words per gate here
 */
extern int32 __eval_1wide_gate(struct gate_t *gp, int32 gwid)
{
 struct xstk_t *xsp;
 int32 bi, wi;

 push_xstk_(xsp, gwid);
 /* notice this includes output */
 __ld_gate_wide_val(xsp->ap, xsp->bp, gp->gstate.wp, gwid); 

 /* notice width it total bits while bit is index */
 /* must acess output value but in load must mask off high output bit */
 wi = get_wofs_(gwid - 1);
 bi = get_bofs_(gwid - 1);
 /* 2 shifts for b part because bit can be low bit in both halves */
 /* extract output value before masking off output */
 __old_gateval = ((xsp->ap[wi] >> bi) & 1L)
  | (((xsp->bp[wi] >> bi) & 1L) << 1);  

 /* input state minus output for gate eval */
 xsp->ap[wi] &= __masktab[bi];
 xsp->bp[wi] &= __masktab[bi];
 /* this sets new gateval */
 eval_wide_gate(gp, xsp); 
 __pop_xstk();
 if (__new_gateval == __old_gateval) return(FALSE);
 return(TRUE);
}

/*
 * load a wide form gate value from wp into ap and bp
 */
extern void __ld_gate_wide_val(word32 *ap, word32 *bp, word32 *gsp, int32 gwid)
{
 int32 wlen;
 word32 *rap;

 wlen = wlen_(gwid);
 rap = &(gsp[2*wlen*__inum]);
 memcpy(ap, rap, WRDBYTES*wlen);
 memcpy(bp, &(rap[wlen]), WRDBYTES*wlen);
}

/*
 * evaluate a wide gate (> 15 inputs)
 * know xsp a and b parts have high output bit masked off
 * and operations here in place so state replaced by 1 bit value
 */
static void eval_wide_gate(struct gate_t *gp, struct xstk_t *xsp)
{
 int32 rta, rtb;
 int32 nins;

 nins = gp->gpnum - 1;
 switch (gp->gmsym->el.eprimp->gateid) {
  case G_BITREDAND: /* and */
   __lunredand(&rta, &rtb, xsp->ap, xsp->bp, nins);
   __narrow_to1bit(xsp);
   xsp->ap[0] = (word32) rta;
   xsp->bp[0] = (word32) rtb;
   break;
  case G_NAND: /* nand */
   __lunredand(&rta, &rtb, xsp->ap, xsp->bp, nins);
   __narrow_to1bit(xsp);
   xsp->ap[0] = (word32) rta;
   xsp->bp[0] = (word32) rtb;
invert:
   __new_gateval = ((~xsp->ap[0] | xsp->bp[0]) & 1L) | (xsp->bp[0] << 1);
   return;
  case G_BITREDOR: /* or */
   __lunredor(&rta, &rtb, xsp->ap, xsp->bp, nins);
   __narrow_to1bit(xsp);
   xsp->ap[0] = (word32) rta;
   xsp->bp[0] = (word32) rtb;
   break;
  case G_NOR: /* nor */
   __lunredor(&rta, &rtb, xsp->ap, xsp->bp, nins);
   __narrow_to1bit(xsp);
   xsp->ap[0] = (word32) rta;
   xsp->bp[0] = (word32) rtb;
   goto invert;
  case G_BITREDXOR: /* xor */
   __lunredxor(&rta, &rtb, xsp->ap, xsp->bp, nins);
   __narrow_to1bit(xsp);
   xsp->ap[0] = (word32) rta;
   xsp->bp[0] = (word32) rtb;
   break;
  case G_REDXNOR: /* xnor */
   __lunredxor(&rta, &rtb, xsp->ap, xsp->bp, nins);
   __narrow_to1bit(xsp);
   xsp->ap[0] = (word32) rta;
   xsp->bp[0] = (word32) rtb;
   goto invert;
  default: __case_terr(__FILE__, __LINE__);
 } 
 /* know stack value replaced to 1 bit result by here */
 __new_gateval = xsp->ap[0] | (xsp->bp[0] << 1);
}

/* --
-- value is 000111vv table is 6 bits lllhhh - 1 if has 0 strength --
-- i.e. st0 is val shifted 5 let and st1 is val shifted 2 --
0 - 15
000 000 00 - 0
000 000 01 - 0
000 000 10 - 0x02
000 000 11 - 0
000 001 00 - 0x02
000 001 01 - 001001 01 = 0x25
000 001 10 - 0
000 001 11 - 0x07

000 010 00   0x02
000 010 01   01001001 = 0x49 
000 010 10   0
000 010 11   0x0b
000 011 00   0x02
000 011 01   01101101 = 0x6d
000 011 10   0
000 011 11   0x0f

16-31
000 100 00 - 0x02
000 100 01 - 10010001 = 0x91
000 100 10 - 0
000 100 11 - 0x13
000 101 00 - 0x02
000 101 01 - 101101 01 = 0xb5
000 101 10 - 0
000 101 11 - 0x17

000 110 00   0x02
000 110 01   11011001 = 0xd9
000 110 10   0
000 110 11   0x1b 
000 111 00   0x02
000 111 01   11111101 = 0xfd
000 111 10   0
000 111 11   0x1f

32-47
001 000 00 - 00100100 = 0x24
001 000 01 - 0x02
001 000 10 - 0
001 000 11 - 0x23
001 001 00 - 0x24
001 001 01 - 0x25
001 001 10 - 0
001 001 11 - 0x27

*001 010 00  00100100 = 0x24 not 0x28
* 001 010 01 01001001 = 0x49 not 0x29
001 010 10   0
001 010 11   0x2b
*001 011 00  00100100 = 0x24 not 0x2c
*001 011 01  01101101 = 0x6d not 0x2d
001 011 10   0
001 011 11   0x2f

48-63
*001 100 00 - 00100100 = 0x24 not 0x30
*001 100 01 - 10010001 = 0x91 not 0x31
001 100 10 - 0
001 100 11 - 0x33
*001 101 00 - 00100100 = 0x24 not 0x34
*001 101 01 - 10110101 = 0xb5 not 0x35
001 101 10 - 0
001 101 11 - 0x37

*001 110 00 - 00100100 = 0x24 not 0x38
*001 110 01 - 11011001 = 0xd9 not 0x39
001 110 10   0
001 110 11   0x3b
*001 111 00 - 00100100 = 0x24 not 0x3c
*001 111 01 - 11111101 = 0xfd not 0x3d
001 111 10   0
001 111 11   0x3f

64-79
010 000 00 - 01001000 = 0x48
010 000 01 - 0x02
010 000 10 - 0
010 000 11 - 0x43
*010 001 00 - 01001000 = 0x48 not 0x44
*010 001 01 - 00100101 = 0x25 not 0x45
010 001 10 - 0
010 001 11 - 0x47

010 010 00   0x48
010 010 01   0x49
010 010 10   0
010 010 11   0x4b
*010 011 00 - 01001000 = 0x48 not 0x4c
*010 011 01 - 01101101 = 0x6d not 0x4d
010 011 10   0
010 011 11   0x4f

80-95
*010 100 00 - 01001000 = 0x48 not 0x50
*010 100 01 - 10010001 = 0x91 not 0x51
010 100 10 - 0
010 100 11 - 0x53
*010 101 00 - 01001000 = 0x48 not 0x54
*010 101 01 - 10110101 = 0xb5 not 0x55
010 101 10 - 0
010 101 11 - 0x57

*010 110 00 - 01001000 = 0x48 not 0x58
*010 110 01 - 11011001 = 0xd9 not 0x59
010 110 10   0
010 110 11   0x5b
*010 111 00 - 01001000 = 0x49 not 0x5c
*010 111 01 - 11111101 = 0xfd not 0x5d
010 111 10   0
010 111 11   0x5f

96-111
011 000 00 - 01101100 = 0x6c
011 000 01 - 0x02
011 000 10 - 0
011 000 11 - 0x63
*011 001 00 - 01101100 = 0x6c not 0x64
*011 001 01 - 00100101 = 0x25 not 0x65
011 001 10 - 0
011 001 11 - 0x67

*011 010 00 - 01101100 = 0x6c not 0x68
*011 010 01 - 01001001 = 0x49 not 0x69
011 010 10   0
011 010 11   0x6b
011 011 00   0x6c
011 011 01   0x6d
011 011 10   0
011 011 11   0x6f

112-127
*011 100 00 - 01101100 = 0x6c not 0x70
*011 100 01 - 10010001 = 0x91 not 0x71
011 100 10 - 0
011 100 11 - 0x73
*011 101 00 - 01101100 = 0x6c not 0x74
*011 101 01 - 10110101 = 0xb5 not 0x75
011 101 10 - 0
011 101 11 - 0x77

*011 110 00 - 01101100 = 0x6c not 0x78
*011 110 01 - 11011001 = 0xd9 not 0x79
011 110 10   0
011 110 11   0x7b
*011 111 00 - 01101100 = 0x6c not 0x7c
*011 111 01 - 11111101 = 0xfd not 0x7d
011 111 10   0
011 111 11   0x7f

128-143
100 000 00 - 10010000 = 0x90
100 000 01 - 0x02
100 000 10 - 0
100 000 11 - 0x83
*100 001 00 - 10010000 = 0x90 not 0x84
*100 001 01 - 00100101 = 0x25 not 0x85
100 001 10 - 0
100 001 11 - 0x87

*100 010 00 - 10010000 = 0x90 not 0x88
*100 010 01 - 01001001 = 0x49 not 0x89
100 010 10   0
100 010 11   0x8b
*100 011 00 - 10010000 = 0x90 not 0x8c
*100 011 01 - 01101101 = 0x6d not 0x8d
100 011 10   0
100 011 11   0x8f

144-159
100 100 00 - 0x90
100 100 01 - 0x91
100 100 10 - 0
100 100 11 - 0x93
*100 101 00 - 10010000 = 0x90 not 0x94
*100 101 01 - 10110101 = 0xb5 not 0x95
100 101 10 - 0
100 101 11 - 0x97

*100 110 00 - 10010000 = 0x90 not 0x98
*100 110 01 - 11011001 = 0xd9 not 0x99
100 110 10   0
100 110 11   0x9b
*100 111 00 - 10010000 = 0x90 not 0x9c
*100 111 01 - 11111101 = 0xfd not 0x9d
100 111 10   0
100 111 11   0x9f

160-175
101 000 00 - 10110100 = 0xb4
101 000 01 - 0x02
101 000 10 - 0
101 000 11 - 0xa3
*101 001 00 - 10110100 = 0xb4 not 0xa4
*101 001 01 - 00100101 = 0x25 not 0xa5
101 001 10 - 0
101 001 11 - 0xa7

*101 010 00 - 10110100 = 0xb4 not 0xa8
*101 010 01 - 01001001 = 0x49 not 0xa9
101 010 10   0
101 010 11   0xab
*101 011 00 - 10110100 = 0xb4 not 0xac
*101 011 01 - 01101101 = 0x6d not 0xad
101 011 10   0
101 011 11   0xaf

176-191
*101 100 00 - 10110100 = 0xb4 not 0xb0
*101 100 01 - 10010001 = 0x91 not 0xb1
101 100 10 - 0
101 100 11 - 0xb3
101 101 00 - 0xb4
101 101 01 - 0xb5
101 101 10 - 0
101 101 11 - 0xb7

*101 110 00 - 10110100 = 0xb4 not 0xb8
*101 110 01 - 11011001 = 0xb9 not 0xb9
101 110 10   0
101 110 11   0xbb
*101 111 00 - 10110100 = 0xb4 not 0xbc
*101 111 01 - 11111101 = 0xfd not 0xbd
101 111 10   0
101 111 11   0xbf

192-207
110 000 00 - 11011000 = 0xd8
110 000 01 - 0x02
110 000 10 - 0
110 000 11 - 0xc3
*110 001 00 - 11011000 = 0xd8 not 0xc4
*110 001 01 - 00100101 = 0x25 not 0xc5
110 001 10 - 0
110 001 11 - 0xc7

*110 010 00 - 11011000 = 0xd8 not 0xc8
*110 010 01 - 01001001 = 0x49 not 0xc9
110 010 10   0
110 010 11   0xcb
*110 011 00 - 11011000 = 0xd8 not 0xcc
*110 011 01 - 01101101 = 0x6d not 0xcd
110 011 10   0
110 011 11   0xcf

208-223
*110 100 00 - 11011000 = 0xd8 not 0xd0
*110 100 01 - 10010001 = 0x91 not 0xd1
110 100 10 - 0
110 100 11 - 0xd3
*110 101 00 - 11011000 = 0xd8 not 0xd4
*110 101 01 - 10110101 = 0xb5 not 0xd5
110 101 10 - 0
110 101 11 - 0xd7

110 110 00   0xd8
110 110 01   0xd9
110 110 10   0
110 110 11   0xdb
*110 111 00 - 11011000 = 0xd8 not 0xdc
*110 111 01 - 11111101 = 0xfd not 0xdd
110 111 10   0
110 111 11   0xdf

224-239
111 000 00 - 11111100 = 0xfc
111 000 01 - 0x02
111 000 10 - 0
111 000 11 - 0xe3
*111 001 00 - 11111100 = 0xfc not 0xe4
*111 001 01 - 00100101 = 0x25 not 0xe5
111 001 10 - 0
111 001 11 - 0xe7

*111 010 00 - 11111100 = 0xfc not 0xe8
*111 010 01 - 01001001 = 0x49 not 0xe9
111 010 10   0
111 010 11   0xeb
*111 011 00 - 11111100 = 0xfc not 0xec
*111 011 01 - 01101101 = 0x6d not 0xed
111 011 10   0
111 011 11   0xef

240-255
*111 100 00 - 11111100 = 0xfc not 0xf0
*111 100 01 - 10010001 = 91 not 0xf1
111 100 10 - 0
111 100 11 - 0xf3
*111 101 00 - 11111100 = 0xfc not 0xf4
*111 101 01 - 10110101 = 0xb5 not 0xf5
111 101 10 - 0
111 101 11 - 0xf7

*111 110 00 - 11111100 = 0xfc not 0xf8
*111 110 01 - 11011001 = 0xd9 not 0xf9
111 110 10   0
111 110 11   0xfb
111 111 00   0xfc
111 111 01   0xfd
111 111 10   0
111 111 11   0xff

--- */

/* table to determine if special strength delay calculation needed */
/* index is 6 stren bits - value removed */
byte __hizstren_del_tab[] = {
 1, 1, 1, 1, 1, 1, 1, 1,  
 1, 0, 0, 0, 0, 0, 0, 0, 
 1, 0, 0, 0, 0, 0, 0, 0,
 1, 0, 0, 0, 0, 0, 0, 0,
 1, 0, 0, 0, 0, 0, 0, 0,
 1, 0, 0, 0, 0, 0, 0, 0,
 1, 0, 0, 0, 0, 0, 0, 0,
 1, 0, 0, 0, 0, 0, 0, 0 
};

/* SJM 08/07/01 - table always needed to map from gate/conta logic val */
/* to driving stren (or connected net stren if only one driver) */
/* old version just mapped the hiz strens to hiz instead of 0 or 1 */
/* now also maps 0 and 1 and uses identity map for x */

/* table to map gate or conta driving stren to driving stren val */
/* if stren component hiz, value replaced by hiz else if 0 or 1 selects */
/* that component from the 0 and 1 components, if x uses both components */
/* if 0, both stren components have stren0, i 1, both stren1, if x both */
/* as coded in source if either component hiz logic value replaced by z */

/* for open collector nor gate, the 1 stren is highz1 so gate output val */
/* of 1 causes gate to output hiz */ 
byte __stren_map_tab[] = {

 0, 0, 0x02, 0, 0x02, 0x25, 0, 0x07,
 0x02, 0x49, 0, 0x0b, 0x02, 0x6d, 0, 0x0f,

 0x02, 0x91, 0, 0x13, 0x02, 0xb5, 0, 0x17,
 0x02, 0xd9, 0, 0x1b, 0x02, 0xfd, 0, 0x1f,

 0x24, 0x02, 0, 0x23, 0x24, 0x25, 0, 0x27,
 0x24, 0x49, 0, 0x2b, 0x24, 0x6d, 0, 0x2f,

 0x24, 0x91, 0, 0x33, 0x24, 0xb5, 0, 0x37,
 0x24, 0xd9, 0, 0x3b, 0x24, 0xfd, 0, 0x3f,

 0x48, 0x02, 0, 0x43, 0x48, 0x25, 0, 0x47,
 0x48, 0x49, 0, 0x4b, 0x48, 0x6d, 0, 0x4f,

 0x48, 0x91, 0, 0x53, 0x48, 0xb5, 0, 0x57,
 0x48, 0xd9, 0, 0x5b, 0x49, 0xfd, 0, 0x5f,

 0x6c, 0x02, 0, 0x63, 0x6c, 0x25, 0, 0x67,
 0x6c, 0x49, 0, 0x6b, 0x6c, 0x6d, 0, 0x6f,

 0x6c, 0x91, 0, 0x73, 0x6c, 0xb5, 0, 0x77,
 0x6c, 0xd9, 0, 0x7b, 0x6c, 0xfd, 0, 0x7f,

 0x90, 0x02, 0, 0x83, 0x90, 0x25, 0, 0x87,
 0x90, 0x49, 0, 0x8b, 0x90, 0x6d, 0, 0x8f,

 0x90, 0x91, 0, 0x93, 0x90, 0xb5, 0, 0x97,
 0x90, 0xd9, 0, 0x9b, 0x90, 0xfd, 0, 0x9f,

 0xb4, 0x02, 0, 0xa3, 0xb4, 0x25, 0, 0xa7,
 0xb4, 0x49, 0, 0xab, 0xb4, 0x6d, 0, 0xaf,

 0xb4, 0x91, 0, 0xb3, 0xb4, 0xb5, 0, 0xb7,
 0xb4, 0xb9, 0, 0xbb, 0xb4, 0xfd, 0, 0xbf,

 0xd8, 0x02, 0, 0xc3, 0xd8, 0x25, 0, 0xc7,
 0xd8, 0x49, 0, 0xcb, 0xd8, 0x6d, 0, 0xcf,

 0xd8, 0x91, 0, 0xd3, 0xd8, 0xb5, 0, 0xd7,
 0xd8, 0xd9, 0, 0xdb, 0xd8, 0xfd, 0, 0xdf,

 0xfc, 0x02, 0, 0xe3, 0xfc, 0x25, 0, 0xe7,
 0xfc, 0x49, 0, 0xeb, 0xfc, 0x6d, 0, 0xef,

 0xfc, 0x91, 0, 0xf3, 0xfc, 0xb5, 0, 0xf7,
 0xfc, 0xd9, 0, 0xfb, 0xfc, 0xfd, 0, 0xff
};

/* bufif table */
 /* ---
  format is (cb)(ca)(db)(da) 
  0   0   0   0  (d0,c0) 0x0
  0   0   0   1  (d1,c0) 0x1
  0   0   1   0  (dz,c0) 0x2
  0   0   1   1  (dx,c0) 0x3
  0   1   0   0  (d0,c1) 0x4
  0   1   0   1  (d1,c1) 0x5
  0   1   1   0  (dz,c1) 0x6
  0   1   1   1  (dx,c1) 0x7
  1   0   0   0  (d0,cz) 0x8
  1   0   0   1  (d1,cz) 0x9
  1   0   1   0  (dz,cz) 0xa
  1   0   1   1  (dx,cz) 0xb
  1   1   0   0  (d0,cx) 0xc
  1   1   0   1  (d1,cx) 0xd
  1   1   1   0  (dz,cx) 0xe
  1   1   1   1  (dx,cx) 0xf
  --- */

/* assume value is strength and in and tab and or in or tab */
/* and tab no value is ff, or tab no value is 0 */
byte __bufif_and_tab[] = {  
 /* 0-15 bufif0 */
 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0,
 0xe0, 0x1c, 0xff, 0xff, 0xe0, 0x1c, 0xff, 0xff,  
 /* 16-31 bufif1 */
 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
 0xe0, 0x1c, 0xff, 0xff, 0xe0, 0x1c, 0xff, 0xff, 
 /* 32-47 notif0 */
 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0,
 0x1c, 0xe0, 0xff, 0xff, 0x1c, 0xe0, 0xff, 0xff,  
 /* 48-63 notif1 */
 0, 0,  0, 0, 0xff, 0xff, 0xff, 0xff,
 0x1c, 0xe0, 0xff, 0xff, 0x1c, 0xe0, 0xff, 0xff,
};

byte __bufif_or_tab[] = {  
 /* 0-15 bufif0 */
 0, 1, 3, 3, 2, 2, 2, 2,
 3, 3, 3, 3, 3, 3, 3, 3,  
 /* 16-31 bufif1 */
 2, 2, 2, 2, 0, 1, 3, 3,
 3, 3, 3, 3, 3, 3, 3, 3,
 /* 32-47 notif0 */
 1, 0, 3, 3, 2, 2, 2, 2,
 3, 3, 3, 3, 3, 3, 3, 3,
 /* 48-63 notif1 */
 2, 2, 2, 2, 1, 0, 3, 3,
 3, 3, 3, 3, 3, 3, 3, 3  
};

/* interpreter bufif gate type base table */
/* BEWARE - assumes LAST_GSYM is 36 - if changes intialize at start */
/* and G_BUFIF0 is 13, G_BUFIF1 is 14, G_NOTIF0 is 20, G_NOTIF1 is 21 */
int32 __bufif_base_tab[] = {
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, 0, 16, -1, -1, -1, -1, -1,
 32, 48, -1, -1, -1, -1, -1, -1, -1, -1,
 -1, -1, -1, -1, -1, -1, -1 };

/*
 * evaluate bufif gate style gate where state stores output strength
 * this routine does not evaluate if input same
 *
 * input strength not passed thru but driven strength non constant
 * storage is data port 2 bits, control 2 bit and output 8 bits and
 * stored as half word32 that is access as such
 *
 * wires drives by bufifs must be marked as strength
 * i is gate expr. index (0th is out)
 * and since gate state stores strength need to correct for hiz strength
 *
 * if inputs differ and evals, it must set __new_gateval to value
 * with added in or driven strength
 *
 */
extern int32 __eval_bufif_gate(register struct gate_t *gp, word32 i,
 int32 *out_chg)
{
 register word32 gwrd;
 register struct xstk_t *xsp;
 int32 base;

 if (__debug_flg && __ev_tracing)
  __tr_msg("--changing bufif - old: %s\n",
   __gstate_tostr(__xs, gp, FALSE));

 /* this loads value part if driver has strength */
 xsp = __eval_xpr(gp->gpins[i]);
 /* gate inputs can be wider than 1 bit */
 xsp->ap[0] &= 1L;
 xsp->bp[0] &= 1L;

 gwrd = (word32) gp->gstate.hwp[__inum];
 __old_inputval = (i == 1) ? (gwrd & 3L) : ((gwrd >> 2) & 3L); 
 __new_inputval = xsp->ap[0] | (xsp->bp[0] << 1);
 __pop_xstk();
 if (__new_inputval == __old_inputval) return(FALSE);

 /* update the changed input state */
 if (i == 1) { gwrd &= ~3L; gwrd |= __new_inputval; }
 else { gwrd &= ~(3L << 2); gwrd |= (__new_inputval << 2); }
 gp->gstate.hwp[__inum] = (hword) gwrd;
 /* buf always has strength (maybe strong if no explicit) */ 
 __old_gateval = (gwrd >> 4);

 /* use input 4 bits as case access - output not just simple 8 bit value */
 gwrd &= 0xf;
 
 /* assume 0 value with driven strength (strong if none) */
 __new_gateval = (gp->g_stval << 2);

 base = __bufif_base_tab[gp->gmsym->el.eprimp->gateid];
 /* DEBUG remove ---
 if (base == -1) __misc_terr(__FILE__, __LINE__);
 --- */
 __new_gateval &= __bufif_and_tab[gwrd + base];
 __new_gateval |= __bufif_or_tab[gwrd + base];

 /* strength is lllhhhvv table is 6 bits lllhhh - 1 if has 0 strength*/  
 /* must correct for special case where 1 strength is hiz (no drive) */
 __new_gateval = __stren_map_tab[__new_gateval];

 /* if no change, nothing to do */
 /* set to T (non 0) if not equal if changed (different) */
 *out_chg = (__new_gateval != __old_gateval);
 return(TRUE);
}

/* normal mos mapping only supply changed to strong */
word32 __mos_stmap[] = { 0, 1, 2, 3, 4, 5, 6, 6 };
/* resistive devices reduce strengths */
word32 __rmos_stmap[] = { 0, 1, 1, 2, 2, 3, 5, 5 }; 

/* ----
0 => d=0,c=0 4 => d=0,c=1 8 => d=0,c=z 0c=> d=0,c=x -> 0,4,8,0xc
1 => d=1,c=0 5 => d=1,c=1 9 => d=1,c=z 0d=> d=1,c=x -> 1,5,9,0xd
3 => d=x,c=0 7 => d=x,c=1 0b=> d=x,c=z 0f=> d=x,c=x -> 3,7,0xb,0xf 
2 => d=z,c=0 6 => d=z,c=1 0a=> d=z,c=z 0e=> d=z,c=x -> 2,6,0xa,0xe
--- */

/*
 * evaluate nmos gate 
 * special format for all mos gates is 3 8 bit values (0-7) data
 * (1st input stren val, * 8-9 control value (2nd input) (10-15 unused stren
 * ignored), * 16-23 output with strength
 *
 * this is passed state word32 for instance and set globals __old gateval
 * and __new gateval
 *
 * for r style reduce strength according to table for non resistive only
 * changes supply to strong (also uses table)
 */
extern void __eval_nmos_gate(word32 gwrd)
{
 register word32 ivec, st0, st1;

 /* state here is 2 8 bit inputs and 1 8 bit strength format output */
 /* 1 word32 per gate */
 __old_gateval = (gwrd >> 16) & 0xffL; 
 /* 4 bit value is index (bits 0-1 data value, 2-3 control value */
 ivec = ((gwrd >> 6) & 0x0c) | (gwrd & 3L);
 st0 = (gwrd >> 5) & 7;
 st1 = (gwrd >> 2) & 7;

 switch ((byte) ivec) {
  /* control 0 or data z - driven to z */
  case 0: case 1: case 2: case 3: case 6: case 10: case 14:
   __new_gateval = 2; 
   break;
  /* control 1 - non x/z data passed thru */
  case 4:
   __new_gateval = 0 | (__mos_stmap[st0] << 5) | (__mos_stmap[st1] << 2); 
   break;
  case 5:
   __new_gateval = 1 | (__mos_stmap[st0] << 5) | (__mos_stmap[st1] << 2); 
   break;
  case 7: case 11: case 15:
   __new_gateval = 3 | (__mos_stmap[st0] << 5) | (__mos_stmap[st1] << 2); 
   break;
  case 8: case 12:
    /* control x/z - data 0 - L */
    /* high 3 bits are 0 strength */
   if (st0 == 0) __new_gateval = 2;
   else __new_gateval = (__mos_stmap[st0] << 5) | 3;
   break;
  case 9: case 13:
   if (st1 == 0) __new_gateval = 2;
   else __new_gateval = (__mos_stmap[st1] << 2) | 3;
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

extern void __eval_rnmos_gate(word32 gwrd)
{
 register word32 ivec, st0, st1;

 /* state here is 2 8 bit inputs and 1 8 bit strength format output */
 /* 1 word32 per gate */
 __old_gateval = (gwrd >> 16) & 0xffL; 
 /* 4 bit value is index (bits 0-1 data value, 2-3 control value */
 ivec = ((gwrd >> 6) & 0x0c) | (gwrd & 3L);
 st0 = (gwrd >> 5) & 7;
 st1 = (gwrd >> 2) & 7;

 switch ((byte) ivec) {
  /* control 0 or data z - driven to z */
  case 0: case 1: case 2: case 3: case 6: case 10: case 14:
   __new_gateval = 2; 
   break;
  /* control 1 - non x/z data passed thru */
  case 4:
   __new_gateval = 0 | (__rmos_stmap[st0] << 5) | (__rmos_stmap[st1] << 2); 
   break;
  case 5:
   __new_gateval = 1 | (__rmos_stmap[st0] << 5) | (__rmos_stmap[st1] << 2); 
   break;
  case 7: case 11: case 15:
   __new_gateval = 3 | (__rmos_stmap[st0] << 5) | (__rmos_stmap[st1] << 2); 
   break;
  case 8: case 12:
    /* control x/z - data 0 - L */
    /* high 3 bits are 0 strength */
   if (st0 == 0) __new_gateval = 2;
   else __new_gateval = (__rmos_stmap[st0] << 5) | 3;
   break;
  case 9: case 13:
   if (st1 == 0) __new_gateval = 2;
   else __new_gateval = (__rmos_stmap[st1] << 2) | 3;
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

extern void __eval_pmos_gate(word32 gwrd)
{
 register word32 ivec, st0, st1;

 /* state here is 2 8 bit inputs and 1 8 bit strength format output */
 /* 1 word32 per gate */
 __old_gateval = (gwrd >> 16) & 0xffL; 
 /* 4 bit value is index (bits 0-1 data value, 2-3 control value */
 ivec = ((gwrd >> 6) & 0x0c) | (gwrd & 3L);
 st0 = (gwrd >> 5) & 7;
 st1 = (gwrd >> 2) & 7;

 switch ((byte) ivec) {
  /* control 0 - non x/z data passed thru */
  case 0:
   __new_gateval = 0 | (__mos_stmap[st0] << 5) | (__mos_stmap[st1] << 2); 
   break;
  case 1:
   __new_gateval = 1 | (__mos_stmap[st0] << 5) | (__mos_stmap[st1] << 2); 
   break;
  case 3: case 11: case 15:
    /* data x, ctrl 0, data x, ctrl z, data x cntrl x become strength x */  
    /* 3, 11, 15 */
   __new_gateval = 3 | (__mos_stmap[st0] << 5) | (__mos_stmap[st1] << 2); 
   break;
  /* control 1 or data z - driven to z */
  case 2: case 4: case 5: case 6: case 7: case 10: case 14:
   __new_gateval = 2; 
    break;
  case 8: case 12: 
   /* if H becomes hiz */
   if (st0 == 0) __new_gateval = 2;
   else __new_gateval = (__mos_stmap[st0] << 5) | 3;
   break;
  case 9: case 13:
   /* if L goes to Hiz not H */
   if (st1 == 0) __new_gateval = 2;
   else __new_gateval = (__mos_stmap[st1] << 2) | 3;
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

extern void __eval_rpmos_gate(word32 gwrd)
{
 register word32 ivec, st0, st1;

 /* state here is 2 8 bit inputs and 1 8 bit strength format output */
 /* 1 word32 per gate */
 __old_gateval = (gwrd >> 16) & 0xffL; 
 /* 4 bit value is index (bits 0-1 data value, 2-3 control value */
 ivec = ((gwrd >> 6) & 0x0c) | (gwrd & 3L);
 st0 = (gwrd >> 5) & 7;
 st1 = (gwrd >> 2) & 7;

 switch ((byte) ivec) {
  /* control 0 - non x/z data passed thru */
  case 0:
   __new_gateval = 0 | (__rmos_stmap[st0] << 5) | (__rmos_stmap[st1] << 2); 
   break;
  case 1:
   __new_gateval = 1 | (__rmos_stmap[st0] << 5) | (__rmos_stmap[st1] << 2); 
   break;
  case 3: case 11: case 15:
   /* data x, ctrl 0, data x, ctrl z, data x cntrl x become strength x */  
   /* 3, 11, 15 */
   __new_gateval = 3 | (__rmos_stmap[st0] << 5) | (__rmos_stmap[st1] << 2); 
   break;
  /* control 1 or data z - driven to z */
  case 2: case 4: case 5: case 6: case 7: case 10: case 14:
   __new_gateval = 2; 
   break;
  case 8: case 12: 
   /* if H becomes hiz */
   if (st0 == 0) __new_gateval = 2;
   else __new_gateval = (__rmos_stmap[st0] << 5) | 3;
   break;
  case 9: case 13:
   /* if L goes to Hiz not H */
   if (st1 == 0) __new_gateval = 2;
   else __new_gateval = (__rmos_stmap[st1] << 2) | 3;
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * evaluate cmos style gate that passes thru input strength
 *
 * special format is 4 8 bit values (0-7) data (1st input stren val,
 * 8-9 nmos control value (2nd input) (10-15 unused stren ignored),
 * 16-17 pmos control value (3rd input) (18-23 unused stren ignored),
 * 24-31 output with strength
 *
 * this is passed gate record and sets globals __old gateval
 * and __new gateval 
 *
 * values only changed if inputs differ
 *
 * for r style reduce strength according to table for non resistive only
 * changes supply to strong
 * this uses the output port net for strength competition
 * scheme builds tree so 2 halves of cmos converted to one drive val here
 */
extern void __eval_cmos_gate(struct gate_t *gp)
{
 register word32 gwrd, tmpwrd;
 int32 nchged, pchged;
 word32 wtyp, gid;
 word32 new_nval, new_pval;
 struct expr_t *xp;

 gwrd = gp->gstate.wp[__inum];
 /* ---
 stren combined gate state [31-24] => always bits 23:16
 3nd pmos control [23-16] => removed 1st, to bits 15-8 2nd
 2rd nmos control [15-8] => same place 1st, removed 2nd
 1st input state [7-0] = always same place
 -- */

 /* LOOKATME - maybe should add bit or use g gone also for this */
 gid = gp->gmsym->el.eprimp->gateid;

 /* controls: 1st nmos 8-15, 2nd pmos 16-23 */
 tmpwrd = (gwrd & 0xffffL) | ((gwrd >> 8) & 0x00ff0000L);
 nchged = TRUE;
 if (gid == G_RCMOS) __eval_rnmos_gate(tmpwrd); else __eval_nmos_gate(tmpwrd);
 if (__new_gateval == __old_gateval) nchged = FALSE; 
 new_nval = __new_gateval;

 tmpwrd = (gwrd & 0xffL) | ((gwrd >> 8) & 0x00ffff00L);
 pchged = TRUE;
 if (gid == G_RCMOS) __eval_rpmos_gate(tmpwrd);
 else __eval_pmos_gate(tmpwrd);
 if (__new_gateval == __old_gateval) pchged = FALSE;
 new_pval = __new_gateval;

 /* now can set old values */
 __old_gateval = gwrd >> 24;

 /* since here old value always old value of cmos, no change means */
 /* no change since if some sort of wired or/and effect will be different */
 if (!nchged && !pchged) return;
 
 /* know at least one different from old, need tournament */
 /* hard part is need wire type of output */
 xp = gp->gpins[0];
 wtyp = (word32) N_REG;
 switch ((byte) xp->optyp) {
  case ID: case GLBREF: wtyp = xp->lu.sy->el.enp->ntyp; break;
  case LSB: wtyp = xp->lu.x->lu.sy->el.enp->ntyp; break;
  default: __case_terr(__FILE__, __LINE__);
 }
 __new_gateval = __comb_1bitsts(wtyp, new_nval, new_pval); 
}

/*
 * convert assigned to expression to value
 */
extern char *__to_gassign_str(char *s, struct expr_t *xp) 
{
 struct xstk_t *xsp;
 byte *sbp;

 if (!xp->x_stren)
  {
   xsp = __eval_xpr(xp);
   __regab_tostr(s, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE);
  }
 else
  {
   xsp = __ndst_eval_xpr(xp);
   sbp = (byte *) xsp->ap;
   __st_regab_tostr(s, sbp, xp->szu.xclen); 
  }
 __pop_xstk();
 return(s);
}

word32 __to_uvaltab[] = {0, 1, 2, 2 };
word32 __to_noztab[] = {0, 1, 3, 3 };

/*
 * change input and if different evaluate udp
 *
 * notice input stored as 2 bit scalars not a and b sections
 * sets 2 bit value byte and restores updated gstate
 * notice extra input does not exist - include output in index if not comb.
 *
 * must pass is edge because initialize treats edge as seq.
 * expects __cur_udp to point to current udp record
 * pi is gstate bit index (starting at 0), i is gate expr. index (from 1)
 * gate values never have strength here maybe added when stored
 *
 * output always stored in high 2 bits of state but 0th gpins pin
 */
extern int32 __eval_udp(register struct gate_t *gp, word32 i, int32 *out_chg,
 int32 is_edge)
{
 register int32 pi;
 register word32 uwrd;
 register struct xstk_t *xsp;
 int32 ndx, bi, wi, outbi, tabi;
 word32 *wp;
 extern word32 __pow3tab[];

 /* combinatorial can be sequential or not */
 /* DBG remove --
 if (__debug_flg && __ev_tracing)
  __tr_msg("-- changing udp - old: %s\n",
   __gstate_tostr(__xs, gp, FALSE));
 --- */

 xsp = __eval_xpr(gp->gpins[i]);
 /* normal udp - just using scalar form as index - i.e. x/z must be 3 */
 __new_inputval = __to_noztab[(xsp->ap[0] & 1L) | ((xsp->bp[0] & 1L) << 1)];
 __pop_xstk();

 outbi = 2*__cur_udp->numins;
 pi = i - 1;
 if (!__cur_udp->u_wide)
  {
   uwrd = (word32) gp->gstate.hwp[__inum];
   __old_inputval = (uwrd >> (2*pi)) & 3L;
   /* DBG remove ---
   if (__old_inputval == 2) __misc_terr(__FILE__, __LINE__);
   --- */

   if (__new_inputval == __old_inputval) return(FALSE);
   /* update the state */
   uwrd &= ~(3L << (2*pi));
   uwrd |= ((hword) __new_inputval << (2*pi));
   gp->gstate.hwp[__inum] = (hword) uwrd;
   /* finally compute the index - will include output if sequential */ 
   /* index since table look up always 3 for x or z */ 
   ndx = (int32) 2*(uwrd & __masktab[2*__cur_udp->numstates]);
   __old_gateval = (uwrd >> outbi) & 3L;
  }
 /* in wide, case need 2nd running value index word32 */
 else 
  {
   wp = &(gp->gstate.wp[2*__inum]);
   /* know all input 0,1, x (2) only */
   __old_inputval = (wp[0] >> (2*pi)) & 3L;
   /* DBG remove --- 
   if (__old_inputval == 2) __misc_terr(__FILE__, __LINE__);
   --- */
   if (__new_inputval == __old_inputval) return(FALSE);

   /* change the input - here x/z is 3 */
   wp[0] &= ~(3L << (2*pi));
   wp[0] |= (__new_inputval << (2*pi));

   /* must correct running index - subtract off contribution of port i */
   /* here x must be 2 not 3 */
   wp[1] -= ((__old_inputval == 3) ? 2 : __old_inputval)*__pow3tab[pi];
   /* add in new contribution of port i */
   wp[1] += ((__new_inputval == 3) ? 2 : __new_inputval)*__pow3tab[pi];  
   /* --- RELEASE remove --
   if (__debug_flg && __ev_tracing)
    __tr_msg("## wide udp word0=%x, word1=%x(%d)\n", wp[0], wp[1], wp[1]); 
   --- */
   /* notice word32 1 index is bit - times 2 to get 2 bit output val. ind */
   ndx = (int32) 2*wp[1];
   __old_gateval = (wp[0] >> outbi) & 3L;
  }

 /* notice ndx already multiplied by 2 for 2 bit table values */
 wi = get_wofs_(ndx);
 bi = get_bofs_(ndx); 
 __new_gateval = (__cur_udp->utab->ludptab[wi] >> bi) & 3L;
 /* RELEASE remove ---
 if (__debug_flg && __ev_tracing)
  __tr_msg(
   "## in=%d, old in=%d, gval=%d, old gval=%d, bi=%d, wi=%d, twrd=%lx\n",
   __new_inputval, __old_inputval, __new_gateval, __old_gateval, bi, wi, 
   __cur_udp->utab->ludptab[wi]);
 --- */

 if (__new_gateval == 3 && is_edge)
  {
   /* level sensitive has state but no edge table */
   tabi =3*pi + ((__old_inputval == 3) ? 2 : __old_inputval);
   wp = __cur_udp->utab->eudptabs[tabi];
   __new_gateval = (wp[wi] >> bi) & 3L;
   /* --- RELEASE remove
   if (__debug_flg && __ev_tracing)
    __tr_msg("## eval edge - new gval=%d, tabi=%d, twrd=%lx\n",
     __new_gateval, tabi, wp[wi]);
   --- */
  }
 /* set to T (non 0) if not equal if changed (different) */
 /* know strengths will always be same */
 *out_chg = (__old_gateval != __new_gateval);
 return(TRUE);
}
