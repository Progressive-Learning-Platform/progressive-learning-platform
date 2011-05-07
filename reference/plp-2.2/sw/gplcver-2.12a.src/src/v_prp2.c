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
 * Verilog simulation preparation routines
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
/* --- SJM REMOVED static void setchk_trchan_wire(struct net_t *); */
static void setchk_1w_fifo(struct net_t *);
static int32 cnt_scalar_fo(struct net_t *);
static int32 has_npp_isform(register struct net_pin_t *);
static void chkset_vec_fifo(struct net_t *, int32 *, int32 *, int32, int32,
 int32);
static int32 has_rng_npp(struct net_t *);
static struct itree_t *cnvt_to_itp(struct mod_t *, int32);
static struct itree_t *cnvt_todown_itp(struct itree_t *, struct mod_t *,
 int32);
static void chk_trifctrl_insame_chan(struct gate_t *, struct expr_t *);
static void chk_samechan_trifctrl_simple(struct mod_t *, struct gate_t *,
 int32, struct expr_t *, struct expr_t *);
static int32 net_in_expr(struct expr_t *, struct net_t *);
static void prep_tf_rwexprs(void);
static int32 lhs_has_figt1(struct expr_t *);
static void getpat_lhs_figt1(struct mod_t *, struct expr_t *,
struct conta_t *);
static void chk_decl_siderep(struct expr_t *, struct expr_t *, char *,
int32, word32, int32);
static int32 find_var_in_xpr(struct expr_t *, struct net_t *, int32 *);
static void add_portbit_map(struct tenp_t *, struct expr_t *, int32);
static void cmp_nchgbtabsize(void);
static void set_1net_srep(struct net_t *);
static void cmp_tabsizes(void);
static void cmpadd_1var_storsiz(struct net_t *);
static void alloc_var(struct net_t *);
static void alloc_real_var(struct net_t *, int32);
static void alloc_scal_var(struct net_t *, int32);
static int32 all_drvrs_bidirect(struct net_t *);
static void alloc_sscal_var(struct net_t *, int32);
static void alloc_svec_var(struct net_t *, int32);
static void reinit_1wirereg(struct net_t *, struct mod_t *);
static void alloc_dce_prevval(struct dcevnt_t *, struct mod_t *);
static void init_dce_exprval(struct dcevnt_t *);
static struct mod_t *dcep_ref_mod(struct dcevnt_t *);
static void init_dce_prevval(struct dcevnt_t *, struct mod_t *);
static void prep_stskcalls(struct st_t *);
static struct st_t *add_loopend_goto(struct st_t *, struct st_t *);
static void push_prpstmt(struct st_t *);
static void pop_prpstmt(void);
static void prep_case(struct st_t *);
static void prep_dctrl(struct st_t *);
static void cnv_cmpdctl_todu(struct st_t *, struct delctrl_t *);
static void prep_event_dctrl(struct delctrl_t *);
static void bld_ev_dces(struct expr_t *, struct delctrl_t *);
static void bld_evxpr_dces(struct expr_t *, struct delctrl_t *, int32);
static void linkon_dce(struct net_t *, int32, int32, struct delctrl_t *,
 int32, struct gref_t *);
static void init_iact_dce(struct dcevnt_t *, struct delctrl_t *,
 struct gref_t *);
static void xmr_linkon_dce(struct net_t *, int32, int32, struct delctrl_t *,
 int32, struct gref_t *);
static struct dcevnt_t *linkon2_dce(struct net_t *, int32, int32,
 struct delctrl_t *, int32, int32, struct mod_t *, struct mod_t *);
static void prep_func_dsable(struct st_t *);
static void bld_init_qcaf_dce_lstlst(struct st_t *);
static void prep_qc_assign(struct st_t *, int32);
static void prep_qc_deassign(struct st_t *);
static struct dceauxlst_t *prep_noncat_qc_assign(struct st_t *,
 struct expr_t *);
static struct dceauxlst_t *prep_noncat_qc_regforce(struct st_t *,
 struct expr_t *);
static void init_qcval(struct qcval_t *);
static void prep_qc_wireforce(struct st_t *);
static void prep_qc_wirerelease(struct st_t *);
static void prep_noncat_qc_wireforce(struct st_t *, struct expr_t *,
 struct dceauxlstlst_t *);
static void prep_noncat_qc_wirerelease(struct expr_t *);
static void bld_qcaf_dces(struct expr_t *, struct qcval_t *);
static void linkon_qcaf_dce(struct net_t *, int32, int32, struct gref_t *,
 struct qcval_t *);
static void process_upwards_grp(struct gref_t *);
static void chk_downrel_inst_sels(struct gref_t *);
static void prep_tchks(void);
static struct tchg_t *bld_start_tchk_npp(struct tchk_t *, struct net_t *,
 int32);
static struct chktchg_t *bld_check_tchk_npp(struct net_t *, int32);
static byte *bld_npp_oldval(struct net_t *, struct mod_t *);
static void reinit_npp_oldval(byte *, struct net_t *, struct mod_t *);
static void prep_pths(void);
static int32 chk_pthels(struct spcpth_t *);
static int32 bldchk_pb_pthdsts(struct spcpth_t *);
static struct tchg_t *try_add_npp_dpthsrc(struct spcpth_t *, struct net_t *,
 int32);
static struct net_pin_t *find_1timchg_psnpp(struct net_t *, int32, int32);
static int32 bldchk_1bit_pthdst(struct spcpth_t *, struct net_t *, int32,
 struct net_t *, int32, int32, struct tchg_t *);
static void get_pthbitwidths(struct spcpth_t *, int32 *, int32 *);
static char *bld_bitref(char *, struct net_t *, int32);
static int32 chk_biti_pthdst_driver(struct spcpth_t *, struct net_t *, int32);
static void emit_pthdst_bit_informs(struct mod_t *);
static void free_dctrl(struct delctrl_t *, int32);
static void free_csitemlst(register struct csitem_t *);

/* extern prototypes defined elsewhere */
extern void __setchk_all_fifo(void);
extern void __bld_pb_fifo(struct net_t *, int32 *, int32 *, int32 *, int32);
extern void __prep_exprs_and_ports(void);
extern void __alloc_tfdrv_wp(struct tfarg_t *, struct expr_t *,
 struct mod_t *);
extern void __init_tfdrv(struct tfarg_t *, struct expr_t *, struct mod_t *);
extern void __prep_contas(void);
extern void __allocinit_perival(union pck_u *, int32, int32, int32);
extern void __allocinit_stperival(union pck_u *, int32, struct net_t *,
 int32);
extern void __alloc_nchgaction_storage(void);
extern void __alloc_sim_storage(void);
extern void __allocinit_arr_var(struct net_t *, int32, int32);
extern void __init_vec_var(register word32 *, int32, int32, int32, word32,
 word32);
extern int32 __get_initval(struct net_t *, int32 *);
extern void __allocinit_vec_var(struct net_t *, int32, int32);
extern void __reinitialize_vars(struct mod_t *);
extern void __initialize_dces(struct mod_t *);
extern void __prep_stmts(void);
extern struct st_t *__prep_lstofsts(struct st_t *, int32, int32);
extern void __push_nbstk(struct st_t *);
extern void __pop_nbstk(void);
extern void __dce_turn_chg_store_on(struct mod_t *, struct dcevnt_t *, int32);  
extern struct dcevnt_t *__alloc_dcevnt(struct net_t *);
extern int32 __is_upward_dsable_syp(struct sy_t *, struct symtab_t *,
 int32 *);
extern void __prep_xmrs(void);
extern void __fill_grp_targu_fld(struct gref_t *);
extern void __prep_specify(void);
extern void __xtract_wirng(struct expr_t *, struct net_t **, int32 *,
 int32 *);
extern void __free_1stmt(struct st_t *);
extern void __free_xprlst(struct exprlst_t *);

extern void __my_free(char *, int32);
extern char *__msg2_blditree(char *, struct itree_t *);
extern char *__to_wtnam(char *, struct net_t *);
extern char *__to_tsktyp(char *, word32);
extern char *__my_malloc(int32);
extern struct st_t *__alloc2_stmt(int32, int32, int32);
extern void __getwir_range(struct net_t *, int32 *, int32 *);
extern void __set_gchg_func(struct gate_t *);
extern int32 __isleaf(struct expr_t *);
extern struct net_t *__find_tran_conn_np(struct expr_t *);
extern int32 __get_pcku_chars(int32, int32);
extern void __grow_xstk(void);
extern void __chg_xstk_width(struct xstk_t *, int32);
extern void __st_perinst_val(union pck_u, int32, register word32 *,
 register word32 *);
extern char *__to_idnam(struct expr_t *);
extern int32 __get_arrwide(struct net_t *);
extern void __reinit_regwir_putvrec(struct net_t *, int32);
extern void __reinit_netdrvr_putvrec(struct net_t *, struct mod_t *);
extern void __init_1net_dces(struct net_t *, struct mod_t *);
extern void __alloc_1instdce_prevval(struct dcevnt_t *);
extern void __init_1instdce_prevval(struct dcevnt_t *);
extern int32 __get_dcewid(struct dcevnt_t *, struct net_t *);
extern void __ld_wire_sect(word32 *, word32 *, struct net_t *, register int32,
 register int32);
extern struct xstk_t *__eval2_xpr(register struct expr_t *);
extern char *__to_sttyp(char *, word32);
extern char *__bld_lineloc(char *, word32, int32);
extern void __add_dctldel_pnp(struct st_t *);
extern void __prep_delay(struct gate_t *, struct paramlst_t *, int32, int32,
 char *, int32, struct sy_t *, int32);
extern void __xmrpush_refgrp_to_targ(struct gref_t *);
extern int32 __ip_indsrch(char *);
extern struct itree_t *__find_unrt_targitp(struct gref_t *,
 register struct itree_t *, int32);
extern void __add_tchkdel_pnp(struct tchk_t *, int32);
extern void __conn_npin(struct net_t *, int32, int32, int32, int32,
 struct gref_t *, int32, char *);
extern void __add_pathdel_pnp(struct spcpth_t *);
extern char *__to_deltypnam(char *, word32);
extern void __free_xtree(struct expr_t *);
extern void __free_del(union del_u, word32, int32);
extern int32 __chk_0del(word32, union del_u, struct mod_t *);
extern void __push_wrkitstk(struct mod_t *, int32);
extern void __pop_wrkitstk(void);
extern void __dmp_exprtab(struct mod_t *, int32);
extern void __dmp_msttab(struct mod_t *, int32);
extern void __dmp_stmt(FILE *, struct st_t *, int32);
extern char *__regab_tostr(char *, word32 *, word32 *, int32, int32, int32);
extern char *__xregab_tostr(char *, word32 *, word32 *, int32,
 struct expr_t *);
extern char *__to_opname(word32);
extern struct expr_t *__sim_copy_expr(struct expr_t *);
extern struct expr_t *__copy_expr(struct expr_t *);
extern struct mod_t *__get_mast_mdp(struct mod_t *);
extern struct net_t *__tranx_to_netbit(register struct expr_t *, int32,
 int32 *, struct itree_t *oside_itp);
extern struct mipd_t *__get_mipd_from_port(struct mod_pin_t *, int32);
extern struct tenp_t *__bld_portbit_netbit_map(struct mod_pin_t *);
extern struct net_pin_t *__alloc_npin(int32, int32, int32);
extern void __alloc_qcval(struct net_t *);
extern void __get_qc_wirrng(struct expr_t *, struct net_t **, int32 *,
 int32 *, struct itree_t **);
extern void __prep_insrc_monit(struct st_t *, int32);
extern int32 __cnt_dcelstels(register struct dcevnt_t *);
extern void __dcelst_off(struct dceauxlst_t *);

extern void __gfwarn(int32, word32, int32, char *, ...);
extern void __sgfwarn(int32, char *, ...);
extern void __gfinform(int32, word32, int32, char *, ...);
extern void __gferr(int32, word32, int32, char *, ...);
extern void __sgferr(int32, char *, ...);
extern void __dbg_msg(char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);
extern void __misc_sgfterr(char *, int32);
extern void __misc_gfterr(char *, int32, word32, int32);
extern void __sgfterr(int32, char *, ...);
extern void __my_fprintf(FILE *, char *, ...);
extern void __free_stlst(register struct st_t *);
static void cmp_xform_delay(int32, union del_u);
static void cmp_xform_ports(void);
static void cmp_xform_ialst(void);
static struct st_t *cmp_xform_lstofsts(register struct st_t *);
static struct st_t *cmp_xform1_stmt(register struct st_t *, struct st_t *);
static void xform_tf_syst_enable(struct st_t *);
static struct expr_t *mv1_expr_totab(struct expr_t *);
static void xform_tf_sysf_call(struct expr_t *);
static void cmp_xform_csitemlst(register struct csitem_t *);
static void cxf_fixup_loopend_goto(struct st_t *, struct st_t *);
static void cxf_fixup_lstofsts_gotos(struct st_t *, int32);
static void cxf_fixup_case_gotos(struct st_t *);
static void cxf_fixup_func_dsabl_gotos(struct st_t *);
static void cmp_xform_inst_conns(void);
static void cmp_xform_gates(void);
static void cmp_xform_contas(void);
static void cmp_xform_tasks(void);
static void cmp_xform_specify(void);
static char *bld_opname(char *, struct expr_t *);

extern word32 __masktab[];

int32 __prep_numsts;

/*
 * FAN OUT AND PORT COLLAPSING ROUTINES
 */

/*
 * set and check all wire fi and fo
 * here must ignore any added for unc. bid. wires
 * cannot check fifo for wires in tran channels 
 *
 * LOOKATME - maybe allocate different formats depending on fi/fo
 * to keep lists short 
 * 
 * SJM - 06/25/00 - difference from 2018c since change chg state algorithm
 */
extern void __setchk_all_fifo(void)
{
 register int32 ni;
 register struct net_t *np;
 struct mod_t *mdp;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mnnum == 0) continue;

   __push_wrkitstk(mdp, 0);
   for (ni = 0, np = &(__inst_mod->mnets[0]); ni < __inst_mod->mnnum;
    ni++, np++)
    {
     /* --- SJM 11/20/01 - removed since useless and slow 
     if (np->ntraux != NULL) { setchk_trchan_wire(np); continue; }
     --- */
     if (np->ntyp >= NONWIRE_ST) continue;
     setchk_1w_fifo(np);
    }
   __pop_wrkitstk();
  }
} 

/*
 * check for tran channels with some bits in and some not
 * for non tran channel wires (no edges) remove vibp
 * notice all tran connected nets have fi>1 set 
 */
/* --- SJM 11/20/01 - removed since warning useless and this takes too
      long to make freeing rare small amount of memory worth it 
 
static void setchk_trchan_wire(struct net_t *np) 
{
 register int32 bi, ii;
 struct traux_t *trap;
 struct itree_t *itp;
 struct vbinfo_t *vbip;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 trap = np->ntraux;
 for (bi = 0; bi < np->nwid; bi++)
  {
   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    {
     vbip = trap->vbitchans[ii*np->nwid + bi];
     if (vbip != NULL) 
      {
       if (vbip->vivxp->vedges != NULL) continue;
       __my_free((char *) vbip, sizeof(struct vbinfo_t));
       trap->vbitchans[ii*np->nwid + bi] = NULL; 
      } 
     -* SJM 01/26/99 - new connect modules not in tran channels *- 
     if ((itp = cnvt_to_itp(__inst_mod, ii)) == NULL) continue;

     sprintf(s1, "in %s(%s):", __inst_mod->msym->synam,
      __msg2_blditree(__xs, itp));
     if (np->n_isavec) sprintf(s3, "%s[%d]", np->nsym->synam, bi);
     else strcpy(s3, np->nsym->synam); 
     __gfinform(450, np->nsym->syfnam_ind, np->nsym->sylin_cnt, 
      "%s %s %s not in any transistor channel but other bits are",
       s1, __to_wtnam(s2, np), s3);
    }
  }
}
--- */

/*
 * set and check for 1 wire
 * but no warns or informs
 *
 * sets n_multfi bit, but not set for tran channel's which need fi>1 eval
 * because instead x_multfi is set, n_multfi used for other checking
 */
static void setchk_1w_fifo(struct net_t *np)
{
 register int32 ii;
 int32 scalfo, nd_fi_chk;
 int32 *pbfi, *pbfo, *pbtcfo;
 char s1[RECLEN];
 
 /* no matter what tri0/tri1 and supply0/supply1 must be multi-fi */
 switch ((byte) np->ntyp) {
  case N_TRI0: case N_TRI1: case N_TRIREG: case N_SUPPLY0: case N_SUPPLY1:
   np->n_multfi = TRUE;
 }

 /* special case 1: pullup - must be constant - no fi */
 nd_fi_chk = TRUE;
 /* special case 1: supply should not have fan-in - no effect */
 if (np->ntyp == N_SUPPLY0 || np->ntyp == N_SUPPLY1)
  {
   if (np->ndrvs != NULL)
    {
     __gfwarn(607, np->nsym->syfnam_ind, np->nsym->sylin_cnt, 
      "in %s: %s %s has driver(s) that have no effect",
      __inst_mod->msym->synam, __to_wtnam(s1, np), np->nsym->synam);
    }
   /* pwr/gnd should not have drivers */
   nd_fi_chk = FALSE;
  }
 /* case 2: scalar */
 if (!np->n_isavec)
  {
   if (nd_fi_chk)
    {
     if (np->ndrvs == NULL)
      {
       if (np->nsym->sy_impldecl) 
        __gfinform(420, np->nsym->syfnam_ind, np->nsym->sylin_cnt, 
         "in %s: implicitly declared scalar wire %s has no drivers",
         __inst_mod->msym->synam, np->nsym->synam);
       else __gfinform(427, np->nsym->syfnam_ind, np->nsym->sylin_cnt, 
        "in %s: scalar wire %s has no drivers", __inst_mod->msym->synam,
        np->nsym->synam);
      } 
     else if (np->ndrvs->npnxt != NULL) np->n_multfi = TRUE;
    }
   /* scalar case multi fo set */  
   scalfo = cnt_scalar_fo(np);
   if (scalfo == 0)
    {
     if (np->nsym->sy_impldecl) 
      __gfinform(429, np->nsym->syfnam_ind, np->nsym->sylin_cnt, 
      "in %s: implicitly declared scalar wire %s drives no declarative fan-out",
       __inst_mod->msym->synam, np->nsym->synam);
     else __gfinform(426, np->nsym->syfnam_ind, np->nsym->sylin_cnt, 
      "in %s: scalar wire %s drives no declarative fan-out",
      __inst_mod->msym->synam, np->nsym->synam);
    }
   return;
  }
 /* case 4: wire that is not scalar */
 pbfi = (int32 *) __my_malloc(sizeof(int32)*np->nwid);
 pbfo = (int32 *) __my_malloc(sizeof(int32)*np->nwid);
 pbtcfo = (int32 *) __my_malloc(sizeof(int32)*np->nwid);
 if (!has_npp_isform(np->ndrvs))
  {
   /* notice not using t chg counts for now */
   __bld_pb_fifo(np, pbfi, pbfo, pbtcfo, 0);
   chkset_vec_fifo(np, pbfi, pbfo, 0, FALSE, nd_fi_chk);
  }
 else
  {
   /* tricky has IS form case - must do for every inst */
   /* if any inst. has fi > 1, then wire must be all multi fan in */
   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    chkset_vec_fifo(np, pbfi, pbfo, ii, TRUE, nd_fi_chk);
  }
  /* notice task/func/lb variables always regs - never fi > 1 */
  /* done free the tables */
 __my_free((char *) pbfi, sizeof(int32)*np->nwid);
 __my_free((char *) pbfo, sizeof(int32)*np->nwid);
 __my_free((char *) pbtcfo, sizeof(int32)*np->nwid);
}

/*
 * return number of non TCHG fan out for scalar
 * MIPD never seen here - only added by PLI or SDF after elaboration
 */
static int32 cnt_scalar_fo(struct net_t *np)
{
 register struct net_pin_t *npp;
 int32 nfi;

 for (nfi = 0, npp = np->nlds; npp != NULL; npp = npp->npnxt)
  {
   if (npp->npntyp != NP_TCHG) nfi++; 
  }
 return(nfi);
}

/*
 * return T if has IS (-2) form net pin list entry  
 */
static int32 has_npp_isform(register struct net_pin_t *npp)
{
 struct npaux_t *npauxp;

 for (; npp != NULL; npp = npp->npnxt)
  { if ((npauxp = npp->npaux) != NULL && npauxp->nbi1 == -2) return(TRUE); }
 return(FALSE);
}

/*
 * build per bit fi and fo tables - caller must pass wide enough tables
 */
extern void __bld_pb_fifo(struct net_t *np, int32 *pbfi, int32 *pbfo,
 int32 *pbtcfo, int32 ii)
{
 register struct net_pin_t *npp;
 register int32 bi;
 register struct npaux_t *npauxp;
 word32 *wp;

 memset(pbfi, 0, sizeof(int32)*np->nwid);
 memset(pbfo, 0, sizeof(int32)*np->nwid);
 memset(pbtcfo, 0, sizeof(int32)*np->nwid);

 for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
  {
   if ((npauxp = npp->npaux) == NULL || npauxp->nbi1 == -1) 
    {
     for (bi = 0; bi < np->nwid; bi++) (pbfi[bi])++;
     continue;
    }
   if (npauxp->nbi1 == -2)
    { 
     /* SJM 10/12/04 - because contab realloced, must be ndx base of IS */
     wp = &(__contab[npauxp->nbi2.xvi]);
     /* DBG remove - here should never has x in index */
     if (wp[2*ii + 1] != 0L) __arg_terr(__FILE__, __LINE__);
     bi = (int32) wp[2*ii];
     (pbfi[bi])++;
     continue;
    }
   for (bi = npauxp->nbi1; bi >= npauxp->nbi2.i; bi--) (pbfi[bi])++;
  }
 for (npp = np->nlds; npp != NULL; npp = npp->npnxt)
  {
   if ((npauxp = npp->npaux) == NULL || npauxp->nbi1 == -1) 
    {
     for (bi = 0; bi < np->nwid; bi++)
      {
       /* MIPD never seen here - only added by PLI or SDF after elaboration */
       if (npp->npntyp == NP_TCHG) (pbtcfo[bi])++; else (pbfo[bi])++; 
      }
     continue;
    }
   if (npauxp->nbi1 == -2)
    { 
     /* SJM 10/12/04 - because contab realloced, must be ndx base of IS */
     wp = &(__contab[npauxp->nbi2.xvi]);
    
     /* DBG remove - here should never has x in index */
     if (wp[2*ii + 1] != 0L) __arg_terr(__FILE__, __LINE__);
     bi = (int32) wp[2*ii];
     if (npp->npntyp == NP_TCHG) (pbtcfo[bi])++; else (pbfo[bi])++; 
     continue;
    }
   for (bi = npauxp->nbi1; bi >= npauxp->nbi2.i; bi--)
    { if (npp->npntyp == NP_TCHG) (pbtcfo[bi])++; else (pbfo[bi])++; } 
  }
}

/*
 * check fan-in and fan-out and set multiple driver net bit for vector
 * know vector or will not be called
 * could call lds and drivers reorganization routine from in here
 */
static void chkset_vec_fifo(struct net_t *np, int32 *pbfi, int32 *pbfo,
 int32 ii, int32 isform, int32 nd_fi_chk)
{
 register int32 bi;
 int32 r1, r2;
 int32 someno_fanin, someno_fanout, allno_fanin, allno_fanout;
 struct itree_t *itp;
 char s1[IDLEN], s2[RECLEN], s3[RECLEN];

 /* DBG remove */
 if (!np->vec_scalared && (has_rng_npp(np) || np->nsym->sy_impldecl))
  __misc_terr(__FILE__, __LINE__);

 someno_fanin = someno_fanout = FALSE;
 allno_fanin = allno_fanout = TRUE;
 /* first check to see if all of wire has no fan-in */ 
 for (bi = 0; bi < np->nwid; bi++)
  {
   if (pbfi[bi] == 0) someno_fanin = TRUE;
   /* notice for is form, any > 1 will set for entire wire */ 
   else { allno_fanin = FALSE; if (pbfi[bi] >= 2) np->n_multfi = TRUE; }
   if (pbfo[bi] == 0) someno_fanout = TRUE; else allno_fanout = FALSE;
  }
 if (!allno_fanin && !someno_fanin && !allno_fanout && !someno_fanout)
  return;
 /* if added net, no messages multi-fo set */ 
 /* mark for cases where cannot determine fi/fo from load and driver list */
 if (isform)
  {
   if ((itp = cnvt_to_itp(__inst_mod, ii)) != NULL)
    {
     sprintf(s1, "in %s(%s):", __inst_mod->msym->synam, __msg2_blditree(__xs,
      itp));
    }
   else sprintf(s1, "in %s(CONNECT?):", __inst_mod->msym->synam);
  }
 else sprintf(s1, "in %s:", __inst_mod->msym->synam);
 if (np->vec_scalared) strcpy(s2, ""); else strcpy(s2, " vectored");
 __getwir_range(np, &r1, &r2);
 if (allno_fanin && nd_fi_chk)
  {
   __gfinform(418, np->nsym->syfnam_ind, np->nsym->sylin_cnt, 
    "%s %s%s [%d:%d] %s has no drivers (no bit has fan-in)", s1,
    __to_wtnam(s3, np), s2, r1, r2, np->nsym->synam);
  }
 if (allno_fanout)
  {
   __gfinform(428, np->nsym->syfnam_ind, np->nsym->sylin_cnt, 
    "%s %s%s [%d:%d] %s drives nothing (no bit has fan-out)", s1,
    __to_wtnam(s3, np), s2, r1, r2, np->nsym->synam);
  }
 /* next emit bit by bit errors - know at least one error */ 
 if (nd_fi_chk)
  {
   if (someno_fanin && !allno_fanin)    
    {
     for (bi = 0; bi < np->nwid; bi++)
      {
       if (pbfi[bi] == 0)
        {
         __gfinform(418, np->nsym->syfnam_ind, np->nsym->sylin_cnt, 
          "%s %s%s [%d:%d] %s bit %d has no declarative drivers (no fan-in)",
          s1, __to_wtnam(s3, np), s2, r1, r2, np->nsym->synam, bi);
        }
     }
    } 
  }
 if (someno_fanout && !allno_fanout)
  {
   for (bi = 0; bi < np->nwid; bi++)
    {
     if (pbfo[bi] == 0)
      {
       __gfinform(428, np->nsym->syfnam_ind, np->nsym->sylin_cnt, 
        "%s %s%s [%d:%d] %s bit %d drives nothing (no declartive fan-out)",
        s1, __to_wtnam(s3, np), s2, r1, r2, np->nsym->synam, bi);
      }
    }
  }
}

/*
 * return T if any net pin is non -1 form
 */
static int32 has_rng_npp(struct net_t *np)
{
 register struct net_pin_t *npp;
 struct npaux_t *npauxp;

 for (npp = np->nlds; npp != NULL; npp = npp->npnxt)
  {
   if ((npauxp = npp->npaux) != NULL && npauxp->nbi1 != -1) return(TRUE);
  }
 return(FALSE);
}

/*
 * convert a module and an itinum to the corresponding itp location
 * this searches 
 */
static struct itree_t *cnvt_to_itp(struct mod_t *mdp, int32 itino)
{
 register int32 ii;
 struct itree_t *itp;

 for (ii = 0; ii < __numtopm; ii++)
  {
   if ((itp = cnvt_todown_itp(__it_roots[ii], mdp, itino)) != NULL)
    return(itp);
  }
 __arg_terr(__FILE__, __LINE__);
 return(NULL);
}

/*
 * dump a down level of a tree
 */
static struct itree_t *cnvt_todown_itp(struct itree_t *itp,
 struct mod_t *mdp, int32 itino)
{
 register int32 ii; 
 int32 ofsnum;
 struct itree_t *itp2;

 if (itp->itip->imsym->el.emdp == mdp && itp->itinum == itino) return(itp);
 ofsnum = itp->itip->imsym->el.emdp->minum;
 for (ii = 0; ii < ofsnum; ii++)
  {
   if ((itp2 = cnvt_todown_itp(&(itp->in_its[ii]), mdp, itino)) != NULL)
    return(itp2);
  } 
 return(NULL);
}

/* 
 * ROUTINE TO PREPARE EXPRESSIONS 
 */

/*
 * check all expr. things that cannot be checked until most of prep done
 * 1) set expr. >1 fi bit
 * check for port and inst psel direction mismatch (warning)  
 * inout ports multi-fi and set here
 */
extern void __prep_exprs_and_ports(void)
{
 register int32 pi, ii, gi, cai;
 register struct mod_pin_t *mpp;
 int32 pnum, derrtyp;
 struct inst_t *ip;
 struct mod_t *mdp, *imdp;
 struct gate_t *gp;
 struct conta_t *cap;
 struct expr_t *xp;
 struct conta_t *pbcap;
 char s1[RECLEN];

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   /* port lhs expr. that drive fi > 1 must be set for each inst. in mod */
   for (ii = 0; ii < mdp->minum; ii++)
    {
     ip = &(mdp->minsts[ii]);
     imdp = ip->imsym->el.emdp;
     if ((pnum = imdp->mpnum) == 0) continue;

     for (pi = 0; pi < pnum; pi++)
      {
       mpp = &(imdp->mpins[pi]);
       /* any up iconn connection to inout is fi>1 here */
       xp = ip->ipins[pi];

       /* input port never lhs */
       if (mpp->mptyp == IO_IN) continue;

       /* will never see inouts, if in tran chan. no drivers */
       if (lhs_has_figt1(xp)) xp->x_multfi = TRUE;
      }
    }
   /* gate outputs may drive fi > 1 and be not strength */
   for (gi = 0; gi < mdp->mgnum; gi++)
    {
     gp = &(mdp->mgates[gi]);

     switch ((byte) gp->g_class) {
      case GC_LOGIC: case GC_UDP: case GC_BUFIF: case GC_MOS: case GC_CMOS:
       if (lhs_has_figt1(gp->gpins[0])) gp->gpins[0]->x_multfi = TRUE;
       for (pi = 1; pi < (int32) gp->gpnum; pi++)
        {
         if (gp->g_class != GC_UDP)
          sprintf(s1, "%s gate input %d", gp->gmsym->synam, pi + 1);
         else sprintf(s1, "udp \"%s\" input %d", gp->gmsym->synam, pi + 1);
         xp = gp->gpins[pi];

         /* check for lhs and rhs same wire, delay type determines error */
         derrtyp =  __chk_0del(gp->g_delrep, gp->g_du, mdp);
         chk_decl_siderep(gp->gpins[0], xp, s1, derrtyp,
          gp->gsym->syfnam_ind, gp->gsym->sylin_cnt);
        }
       /* set the input change eval routine for the gate */ 
       __set_gchg_func(gp);
       break;
      /* no processing for trans - in separate tran switch channel */
      /* tran. channel expressions never seen */
      /* hard (channel input) driver expressions will be marked as fi>1 */
      case GC_TRAN:
       break;
      case GC_TRANIF:
       chk_trifctrl_insame_chan(gp, gp->gpins[2]);
       /* if tranif ctrl expr non leaf, warn if in same channel as term */
       if (!__isleaf(gp->gpins[2]))
        {
         chk_samechan_trifctrl_simple(mdp, gp, 0, gp->gpins[0], gp->gpins[2]);
         chk_samechan_trifctrl_simple(mdp, gp, 1, gp->gpins[1], gp->gpins[2]);
        }
       break;
      /* pull really source on wire not lhs */
      case GC_PULL: break;
      default: __case_terr(__FILE__, __LINE__); 
     }
    }

   for (cap = &(mdp->mcas[0]), cai = 0; cai < mdp->mcanum; cai++, cap++)
    {
     if (lhs_has_figt1(cap->lhsx))
      {
       cap->lhsx->x_multfi = TRUE;
       if (cap->ca_pb_sim)
        {
         for (pi = 0; pi < cap->lhsx->szu.xclen; pi++)
          {
           pbcap = &(cap->pbcau.pbcaps[pi]);
           pbcap->lhsx->x_multfi = TRUE;
          }
        }
      }
    }

   /* module in or inout ports can be non strength but drive fi > 1 */
   pnum = mdp->mpnum;
   for (pi = 0; pi < pnum; pi++)
    {
     mpp = &(mdp->mpins[pi]);
     if (mpp->mptyp == IO_OUT) continue;
     xp = mpp->mpref;
     if (mpp->mptyp == IO_BID) { xp->x_multfi = TRUE; continue; }
     if (lhs_has_figt1(xp)) xp->x_multfi = TRUE;
    }
  }
 prep_tf_rwexprs();
}

/*
 * check tranif 3rd ctrl input in same channel 
 * LOOKATME - possible for xmr to cause this to be ok
 */
static void chk_trifctrl_insame_chan(struct gate_t *gp, struct expr_t *ndp)
{
 struct net_t *np0, *np1, *np2;
 int32 pi;

 if (__isleaf(ndp))
  {
   if (ndp->optyp == ID || ndp->optyp == GLBREF)
    {
     np2 = ndp->lu.sy->el.enp;
     if (np2->ntraux == NULL) return;
     np0 = __find_tran_conn_np(gp->gpins[0]);
     np1 = __find_tran_conn_np(gp->gpins[1]);
     pi = -1;
     if (np2 == np0) pi = 0;
     if (pi != -1 && (np2->ntyp != N_SUPPLY0 && np2->ntyp != N_SUPPLY1))
      {
       __gfinform(3011, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
        "%s %s non supply net %s (port %d) appears in tran channel and control input - possible infinite loop oscillation",
        gp->gmsym->synam, gp->gsym->synam, np2->nsym->synam, pi);
      }   
     pi = -1;
     if (np2 == np1) pi = 1;
     if (pi != -1 && (np2->ntyp != N_SUPPLY0 && np2->ntyp != N_SUPPLY1))
      {
       __gfinform(3011, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
        "%s %s non supply net %s (port %d) appears in tran channel and control input - possible infinite loop oscillation",
        gp->gmsym->synam, gp->gsym->synam, np2->nsym->synam, pi);
      }   
    }
   return;
  }
 if (ndp->lu.x != NULL) chk_trifctrl_insame_chan(gp, ndp->lu.x);
 if (ndp->ru.x != NULL) chk_trifctrl_insame_chan(gp, ndp->ru.x);
}

/*
 * check to see if tranif enable in same channel as the bidirect terminals
 * and expr non simple (if simple will use tranif node vertex value)
 * if complex will not update the enable during tran switch channel relax 
 */
static void chk_samechan_trifctrl_simple(struct mod_t *mdp, struct gate_t *gp,
 int32 pi, struct expr_t *termxp, struct expr_t *ctrlxp)
{
 register int32 ii;
 int32 bi, bi2, chanid0, chanid2, inum2;
 struct net_t *np0, *np2;
 struct vbinfo_t *vbip;
 struct gref_t *grp;

 np0 = __find_tran_conn_np(termxp);
 /* if terminal net is not in enable expr no problem possible */
 if (!net_in_expr(ctrlxp, np0)) return;

 /* DBG remove */
 if (np0->ntraux == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */
 __push_itstk(mdp->moditps[0]);
 /* SJM 08/02/01 - only called for tranif so other side itp same */
 /* get terminal net/bit */
 np0 = __tranx_to_netbit(termxp, 0, &bi, __inst_ptr);
 /* DBG remove */
 if (np0->ntraux == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 bi2 = (bi == -1) ? 0 : bi;
 vbip = np0->ntraux->vbitchans[np0->nwid*__inum + bi2];
 /* SJM 08/07/01 - bit may not be in channel */
 if (vbip == NULL) goto done;
 chanid0 = vbip->chan_id;

 if (ctrlxp->optyp == LSB)
  {
   /* BEWARE - this assumes all constant folded */
   if (termxp->ru.x->optyp != NUMBER)
    {
     __gfwarn(3112, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
      "%s %s third enable input net %s probably in same switch channel as terminal %d but bit select index non constant - updated node value not used when solving channel",
      gp->gmsym->synam, gp->gsym->synam, np0->nsym->synam, pi);
    }
   goto done;
  }
 if (termxp->optyp == PARTSEL)
  {
   chanid2 = vbip->chan_id;
   np2 = ctrlxp->lu.x->lu.sy->el.enp;
   if (ctrlxp->lu.x->optyp == GLBREF)
    {
     grp = ctrlxp->lu.x->ru.grp;
     for (ii = 0; ii < grp->targmdp->flatinum; ii++)
      {
       /* part select here uses low bit */

       inum2 = grp->targmdp->moditps[ii]->itinum;
       vbip = np0->ntraux->vbitchans[np0->nwid*inum2];
       /* SJM 08/07/01 - low bit may not be in channel */
       if (vbip == NULL) goto done;
       chanid2 = vbip->chan_id;
       if (chanid0 == chanid2)
        {
         __gfwarn(3115, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
          "%s %s third enable input heirarchical reference part select of net %s in same switch channel as terminal %d - updated node value not used when solving channel",
          gp->gmsym->synam, gp->gsym->synam, np0->nsym->synam, pi);
         goto done;
        }
      }
     goto done;
    }
   /* non xmr part select case */  
   bi = __contab[ctrlxp->ru.x->ru.xvi];
   /* DBG remove */
   if (np2->ntraux == NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */
   vbip = np2->ntraux->vbitchans[np0->nwid*__inum + bi];
   /* SJM 08/07/01 - low bit may not be in channel */
   if (vbip == NULL) goto done;
   chanid2 = vbip->chan_id;
   if (chanid0 == chanid2)
    {
     __gfwarn(3116, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
      "%s %s third enable input part select of net %s in same switch channel as terminal %d - updated node value not used when solving channel",
      gp->gmsym->synam, gp->gsym->synam, np0->nsym->synam, pi);
    }
   goto done; 
  }
 /* complex expression - can't tell if really in */
 __gfwarn(3118, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
  "%s %s third enable input complex expression contains net %s possibly in same switch channel as terminal %d - updated node value not used when solving channel",
  gp->gmsym->synam, gp->gsym->synam, np0->nsym->synam, pi);

done:
 __pop_itstk();
}

/*
 * return T if net in expr
 */
static int32 net_in_expr(struct expr_t *ndp, struct net_t *np)
{
 struct net_t *np1;

 if (__isleaf(ndp))
  {
   if (ndp->optyp == ID || ndp->optyp == GLBREF)
    {
     /* T even if different instances */
     np1 = ndp->lu.sy->el.enp;
     if (np == np1) return(TRUE);
    }
   return(FALSE);
 }
 if (ndp->lu.x != NULL)
  { if (net_in_expr(ndp->lu.x, np)) return(TRUE); }
 if (ndp->ru.x != NULL)
  { if (net_in_expr(ndp->ru.x, np)) return(TRUE); }
 return(FALSE);
}

/*
 * prepare tf rw expressions 
 * set multfi bits for tf_ rw expressions involving wires 
 * concat never tf_ rw 
 */
static void prep_tf_rwexprs(void)
{
 register int32 pi;
 register struct tfrec_t *tfrp; 
 register struct tfarg_t *tfap;
 struct expr_t *xp;

 for (tfrp = __tfrec_hdr; tfrp != NULL; tfrp = tfrp->tfrnxt)
  {
   for (pi = 1; pi < tfrp->tfanump1; pi++)
    {
     tfap = &(tfrp->tfargs[pi]);
     xp = tfap->arg.axp; 
     if (!xp->tf_isrw) continue;
     /* only wires have multiple fan in */
     if (tfap->anp->ntyp >= NONWIRE_ST) continue;

     /* no context module or inst neede in here */
     if (lhs_has_figt1(xp))
      { xp->x_multfi = TRUE; __alloc_tfdrv_wp(tfap, xp, tfrp->tf_inmdp); }
    }
  }
}

/*
 * allocate the tfdrv wp
 * notice this can never be array 
 */ 
extern void __alloc_tfdrv_wp(struct tfarg_t *tfap, struct expr_t *xp,
 struct mod_t *mdp)
{
 int32 totchars;
 
 if (xp->x_stren)
  tfap->tfdrv_wp.bp = (byte *) __my_malloc(xp->szu.xclen*mdp->flatinum);
 else
  {
   totchars = __get_pcku_chars(xp->szu.xclen, mdp->flatinum);
   tfap->tfdrv_wp.wp = (word32 *) __my_malloc(totchars);
  }
 __init_tfdrv(tfap, xp, mdp);
}

/*
 * initialize tf arg value to z 
 * may be strength
 */
extern void __init_tfdrv(struct tfarg_t *tfap, struct expr_t *xp,
 struct mod_t *mdp)
{
 register int32 i;
 byte *sbp;
 struct xstk_t *xsp;

 if (xp->x_stren)
  {
   sbp = (byte *) tfap->tfdrv_wp.bp;
   set_byteval_(sbp, mdp->flatinum*xp->szu.xclen, ST_HIZ);
  }
 else 
  {
   push_xstk_(xsp, xp->szu.xclen);
   zero_allbits_(xsp->ap, xp->szu.xclen); 
   one_allbits_(xsp->bp, xp->szu.xclen); 
   /* this does not access mod con tab */

   __push_wrkitstk(mdp, 0);
   for (i = 0; i < mdp->flatinum; i++) 
    {
     __inst_ptr->itinum = i;
     __inum = i;
     /* no need to access mod con table here */
     __st_perinst_val(tfap->tfdrv_wp, xp->szu.xclen, xsp->ap, xsp->bp);
    }
   __pop_xstk();
   __pop_wrkitstk();
  }
}

/*
 * return T if lhs has at least 1 fi > 1 net  
 * this is needed because expr. bit also set for any strength but for
 * fi == 1 strength do not need 
 * any wire in a tran/inout channel must be fi>1  
 */
static int32 lhs_has_figt1(struct expr_t *lhsx)
{
 struct expr_t *xp;
 struct net_t *np;

 switch ((byte) lhsx->optyp ) {
  case OPEMPTY: break;
  case ID:
  case GLBREF:
   np = lhsx->lu.sy->el.enp;
chk_net_bit:
   /* any wire in tran channel is fi>1 */ 
   if (np->ntraux != NULL) return(TRUE);
   if (np->n_multfi) return(TRUE);
   break;
  case LSB:
  case PARTSEL:
   np = lhsx->lu.x->lu.sy->el.enp;
   goto chk_net_bit;
  case LCB:
   /* know for lhs at most 1 level of concatenate */
   for (xp = lhsx->ru.x; xp != NULL; xp = xp->ru.x)
    { if (lhs_has_figt1(xp->lu.x)) return(TRUE); }
   break;
  default: __case_terr(__FILE__, __LINE__); 
 }
 return(FALSE);
}

/*
 * additional prep and checking of wide continuous assigns after fi known
 *
 * rules for delay and fi combinations of drivers and rhs save expr.:
 * fi==1, delay =0 => driver access by loading net, do not need rhs val wp
 *                    because no sched. value to re-eval  
 *                    (eval and store)
 * fi==1, delay >0 => driver access by loading net, has rhs val wp
 *                    because need to re-eval expr. after delay for assign
 *                    and so expr. no changes can be killed off early
 * fi>1,  delay =0 => driver access just by loading rhs, may need to save
 *                    rhs val save 
 * fi>1,  delay >0 => need to save driver wp and need rhs val wp for sched.
 *
 * notice delays prepared before here
 */
extern void __prep_contas(void)
{
 register int32 i, bi;
 register struct conta_t *cap, *pbcap;
 int32 cai, insts, derrtyp;
 struct mod_t *mdp; 
 char s1[RECLEN];

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   for (cap = &(mdp->mcas[0]), cai = 0; cai < mdp->mcanum; cai++, cap++)
    {
     insts = mdp->flatinum;

     /* first check and eliminate getpat conta form */
     /* has neither rhsval wp or driver wp and no delay or error */
     /* SJM 09/28/02 - get pat lhs never PB decomposed rhs concat form */
     if (cap->lhsx->getpatlhs)
      { getpat_lhs_figt1(mdp, cap->lhsx, cap); continue; }

     /* need to eval rhs because of delay bit */
     /* know 6 and 12 not possible here but is 4x possible here ? */
     switch ((byte) cap->ca_delrep) {
      case DT_4V: case DT_IS4V: case DT_IS4V1: case DT_IS4V2: case DT_4X:
       cap->ca_4vdel = TRUE;
       break;
      default: cap->ca_4vdel = FALSE;
     }

     if (!cap->ca_pb_sim)
      {
       /* need to save rhs driver for any fi > 1 case either because driver */
       /* different than rhs (delay > 0) or to avoid possible re-evaluation */
       /* of rhs func. call also need to save if has delay for accurate */
       /* inertial delay algorithm */
       if (cap->lhsx->x_multfi || cap->ca_delrep != DT_NONE)
        __allocinit_perival(&cap->ca_drv_wp, insts, cap->lhsx->szu.xclen,
         TRUE);

       /* if conta has delay, need scheduled event array */
       /* fi >1 but no delay does not need schedule event table */
       if (cap->ca_delrep != DT_NONE)
        {
         __allocinit_perival(&(cap->schd_drv_wp), insts,
          cap->lhsx->szu.xclen, TRUE);
         cap->caschd_tevs = (i_tev_ndx *)
          __my_malloc(insts*sizeof(i_tev_ndx));
         for (i = 0; i < insts; i++) cap->caschd_tevs[i] = -1;
        }
       /* need to check for variable on both sides, even if 0 delay */ 
       derrtyp =  __chk_0del(cap->ca_delrep, cap->ca_du, mdp);
       chk_decl_siderep(cap->lhsx, cap->rhsx, "continuous assign", derrtyp,
        cap->casym->syfnam_ind, cap->casym->sylin_cnt);
      }
     else
      {
       derrtyp =  __chk_0del(cap->ca_delrep, cap->ca_du, mdp);
       for (bi = 0; bi < cap->lhsx->szu.xclen; bi++)
        {
         pbcap = &(cap->pbcau.pbcaps[bi]);
         /* DBG remove -- */
         if (pbcap->lhsx->szu.xclen != 1) __misc_terr(__FILE__, __LINE__);
         /* --- */
         /* if conta lhs expr fi>1, then all bit must be and need drv wp */
         if (cap->lhsx->x_multfi || cap->ca_delrep != DT_NONE)
          __allocinit_perival(&(pbcap->ca_drv_wp), insts, 1, TRUE);
         if (cap->ca_delrep != DT_NONE)
          {
           __allocinit_perival(&(pbcap->schd_drv_wp), insts, 1, TRUE);
           pbcap->caschd_tevs = (i_tev_ndx *)
            __my_malloc(insts*sizeof(i_tev_ndx));
           for (i = 0; i < insts; i++) pbcap->caschd_tevs[i] = -1;
          }
         /* need to check for variable on both sides, even if 0 delay */ 
         sprintf(s1, "per bit %d continuous assign", bi);
         chk_decl_siderep(pbcap->lhsx, pbcap->rhsx, s1, derrtyp,
          cap->casym->syfnam_ind, cap->casym->sylin_cnt);
        }
      }
    }
  }
}

/*
 * initialize and maybe allocate the continuous assign sched. and drive tabs
 *
 * this is always used for mod port input and output half drivers using
 * dummy conta for union place holder 
 *
 * initial value is z since needed for inout port in and out drivers 
 * think also better for conta but for conta's always overwritten?
 * this is only for non strength case - stren must be initialized
 * to strength wire type init value
 */
extern void __allocinit_perival(union pck_u *nvap, int32 insts, int32 blen,
 int32 nd_alloc)
{
 int32 totchars, wlen;

 if (blen == 1)
  {
   totchars = insts;
   if (nd_alloc) nvap->bp = (byte *) __my_malloc(totchars);
   set_byteval_(nvap->bp, insts, 2);
  }
 else
  {
   wlen = wlen_(blen); 
   totchars = 2*insts*wlen*WRDBYTES;
   if (nd_alloc) nvap->wp = (word32 *) __my_malloc(totchars);
   /* initialize to z - do not know driver - sim init will set */
   __init_vec_var(nvap->wp, insts, wlen, blen, 0L, 0xffffffffL); 
  }
}

/*
 * allocate (if flag T) and initialize strength perinst value
 * this is for tran channels
 */
extern void __allocinit_stperival(union pck_u *nvap, int32 insts,
 struct net_t *np, int32 nd_alloc)
{
 int32 totbits, stval;
 byte sval;

 totbits = insts*np->nwid;
 if (nd_alloc) nvap->bp = (byte *) __my_malloc(totbits);
 /* for trireg in tran channel, hard driver must be z - it will set value */
 if (np->ntraux != NULL && np->ntyp == N_TRIREG) stval = ST_HIZ;
 else __get_initval(np, &stval);
 sval = (byte) stval;
 set_byteval_(nvap->bp, totbits, sval);
}

/*
 * check a lhs expr. and emit error for every wire that has fi > 1
 * message for getpat only
 * LOOKATME - should bit select of scalared wire be legal for getpattern
 */
static void getpat_lhs_figt1(struct mod_t *mdp, struct expr_t *lhsx,
 struct conta_t *cap)
{
 struct net_t *np;

 switch ((byte) lhsx->optyp) {
  case OPEMPTY: break;
  case ID:
  case GLBREF:
   np = lhsx->lu.sy->el.enp;
   if (np->n_multfi)
    {
     /* wire for getpat will be scalar or will not get this far */
     __gferr(858, cap->casym->syfnam_ind, cap->casym->sylin_cnt,
     "$getpattern lvalue wire %s more than one driver illegal - no way to removebus contention",
     __to_idnam(lhsx)); 
    }
   if (np->ntraux != NULL)
    {
     __gferr(858, cap->casym->syfnam_ind, cap->casym->sylin_cnt,
      "$getpattern lvalue wire %s inout port or tran connection illegal",
      __to_idnam(lhsx)); 
    }
   if (np->nrngrep == NX_DWIR)
    {
     __gferr(938, cap->casym->syfnam_ind, cap->casym->sylin_cnt,
      "$getpattern lvalue wire %s delay or path destination illegal",
      __to_idnam(lhsx)); 
    }
   break;
  case LCB:
   /* know for lhs at most 1 level of concatenate */
   {
    struct expr_t *xp2;
    for (xp2 = lhsx->ru.x; xp2 != NULL; xp2 = xp2->ru.x)
     getpat_lhs_figt1(mdp, xp2->lu.x, cap);
   }
   break;
  default: __case_terr(__FILE__, __LINE__); 
 }
}

/*
 * check continous assign or gate for same variable on both sides
 * if delay then inform, if no delay warn - probable inf. loop
 *
 * possible for 2 same wire globals to be on both sides but not caught here
 */
static void chk_decl_siderep(struct expr_t *lhsx, struct expr_t *rhsx,
 char *objnam, int32 deltyp, word32 fnind, int32 lcnt)
{
 int32 nd_inform, wire_issel;
 struct expr_t *ndp;
 struct net_t *np;
 char s1[RECLEN];

 /* expect rhs to be wider */
 nd_inform = TRUE;
 switch (rhsx->optyp) {
  case ID: 
   nd_inform = FALSE;
   np = rhsx->lu.sy->el.enp; 
cmp_wire:
   if (!find_var_in_xpr(lhsx, np, &wire_issel)) break;

   if (deltyp == DBAD_NONE) { strcpy(s1, "no delay"); nd_inform = FALSE; }
   else if (deltyp == DBAD_EXPR || deltyp == DBAD_MAYBE0) 
    { strcpy(s1, "possible 0 delay"); nd_inform = FALSE; }
   else if (deltyp == DBAD_0)
    { strcpy(s1, "all 0 delay"); nd_inform = FALSE; }
   else strcpy(s1, "delay");

   if (nd_inform || wire_issel)
    __gfinform(444, fnind, lcnt,
     "wire %s repeated on both sides of %s - has %s", np->nsym->synam,
     objnam, s1);
   else __gfwarn(624, fnind, lcnt,
    "wire %s repeated on both sides of %s - has %s", np->nsym->synam, objnam,
    s1);
   break;
  case LSB: case PARTSEL:
   np = rhsx->lu.x->lu.sy->el.enp;
   goto cmp_wire;
  case LCB:
   for (ndp = rhsx->ru.x; ndp != NULL; ndp = ndp->ru.x)
    chk_decl_siderep(lhsx, ndp->lu.x, objnam, deltyp, fnind, lcnt);
   break;
  case FCALL: 
   for (ndp = rhsx->ru.x; ndp != NULL; ndp = ndp->ru.x)
    chk_decl_siderep(lhsx, ndp->lu.x, objnam, deltyp, fnind, lcnt);
 }
}

/*
 * find a variable in an expr.
 * if same variable but global xmr, not a match
 */
static int32 find_var_in_xpr(struct expr_t *xp, struct net_t *np, 
 int32 *wire_sel)
{
 register struct expr_t *ndp;
 struct net_t *npx;

 if (np->ntyp >= NONWIRE_ST) return(FALSE);

 *wire_sel = TRUE;
 switch (xp->optyp) {
  case ID: 
   *wire_sel = FALSE;
   npx = xp->lu.sy->el.enp;
comp_net:
   return(np == npx);
  case LSB: case PARTSEL:
   npx = xp->lu.x->lu.sy->el.enp;
   goto comp_net;
  case LCB:
   for (ndp = xp->ru.x; ndp != NULL; ndp = ndp->ru.x)
    { if (find_var_in_xpr(ndp->lu.x, np, wire_sel)) return(TRUE); } 
   break;
  case FCALL: 
   for (ndp = xp->ru.x; ndp != NULL; ndp = ndp->ru.x)
    { if (find_var_in_xpr(ndp->lu.x, np, wire_sel)) return(TRUE); } 
 }
 return(FALSE);
}

/*
 * ROUTINES TO DYNAMICALLY SET UP MIPDS
 */
   
/*
 * allocate, initialize, and link in NP MIPD load delay npp for a net 
 *
 * reinit does not turn off SDF annotated delays - if task called
 * with replace form works, increment adds
 */
extern void __add_alloc_mipd_npp(struct net_t *np, struct mod_t *mdp)
{
 register int32 bi;
 int32 ii;
 struct net_pin_t *npp;
 struct mipd_t *mipdp;

 /* DBG remove --- */
 if (np->nlds != NULL && np->nlds->npntyp == NP_MIPD_NCHG)
  __misc_terr(__FILE__, __LINE__);
 /* --- */

 __cur_npnp = np;
 __cur_npnum = 0;
 /* always for entire net - table nil if no MIPD on bit for any inst */
 /* this also inserts on front of list */
 npp = __alloc_npin(NP_MIPD_NCHG, -1, -1);

 if (np->nlds == NULL)
  {
   /* DBG remove -- */
   if (__optimized_sim && !__sdf_from_cmdarg)
    {
     __misc_terr(__FILE__, __LINE__);
    }
   /* -- */

   /* LOOKATME - think since input port will always have load */
   /* add the one new mipd net pin - know np never a reg becaus in/inout */
   np->nlds = npp;

   /* SJM 07/25/01 - was not setting all needed bits right */
   /* need to set the various bits to indicate has load so net changes */
   /* put on nchg list */ 
   np->nchg_has_lds = TRUE;

   /* when add mipd load, must turn off all chged */
   /* even if dce list was not empty, if match itp dces some action bits */
   /* will be wrongly off */
   /* SJM 01/06/03 - is is possible to only turn on current inst? */
   for (ii = 0; ii < mdp->flatinum; ii++)
    { np->nchgaction[ii] &= ~(NCHG_ALL_CHGED); }
  }
 else { npp->npnxt = np->nlds; np->nlds = npp; }

 /* SJM 07/26/01 - alloc was wrong size - needs to be one per bit */
 npp->elnpp.emipdbits = (struct mipd_t *)
  __my_malloc(np->nwid*sizeof(struct mipd_t));

 /* need basic setup especially turning on no mipd bit for each */
 for (bi = 0; bi < np->nwid; bi++)
  {
   mipdp = &(npp->elnpp.emipdbits[bi]);
   /* BEWARE - this is crucial T bit indicating no path for this bit */
   mipdp->no_mipd = TRUE;
   mipdp->pth_mipd = FALSE;
   mipdp->impthtab = NULL;
   /* rest of fields set if path ends on bit */
  } 
 /* SJM 02/06/03 - may have npps but not dces so must turn this on */
 /* SJM 06/23/04 - ### ??? LOOKATME - is this needed without regen? */
 /* since nchg nd chgstore on, know nchg action right */
 if (np->ntyp >= NONWIRE_ST) np->nchg_has_dces = TRUE;
}

/*
 * initialize a (PORT form path - first step in annotating either path delay
 * value is the (PORT form destination for one bit
 */
extern void __setup_mipd(struct mipd_t *mipdp, struct net_t *np, int32 ninsts)
{
 register int32 ii;
 int32 stval;
 byte bv;

 mipdp->no_mipd = FALSE;
 mipdp->pb_mipd_delrep = DT_1V;
 mipdp->pb_mipd_du.d1v = (word64 *) __my_malloc(sizeof(word64));
 /* SJM 07/22/01 - nee to start value at 0 so unset stay as 0 */
 mipdp->pb_mipd_du.d1v[0] = 0ULL;

 mipdp->oldvals = (byte *) __my_malloc(ninsts); 

 if (!np->n_stren) bv = (byte) __get_initval(np, &stval);
 else { __get_initval(np, &stval); bv = (byte) stval; }
 for (ii = 0; ii < ninsts; ii++) mipdp->oldvals[ii] = bv;
 
 mipdp->mipdschd_tevs = (i_tev_ndx *) __my_malloc(ninsts*sizeof(i_tev_ndx));
 for (ii = 0; ii < ninsts; ii++) mipdp->mipdschd_tevs[ii] = -1;
 if (mipdp->pth_mipd)
  {
   mipdp->impthtab = (struct impth_t **)
    __my_malloc(ninsts*sizeof(struct impth_t *));
   for (ii = 0; ii < ninsts; ii++) mipdp->impthtab[ii] = NULL;
  }
}

/*
 * re-init all mipds for a port - only called if port has mipds
 * this just reset old value
 */
extern void __reinit_mipd(struct mod_pin_t *mpp, struct mod_t *mdp)
{
 register int32 ndx, bi, ii;
 register struct impth_t *impthp;
 int32 stval;
 byte bv;
 struct tenp_t *prtnetmap;
 struct mipd_t *mipdp;
 struct net_t *np;

 prtnetmap = __bld_portbit_netbit_map(mpp);
 /* for every port bit - just reinit connected bits */
 /* other ports will reinit other bits if used */ 
 for (ndx = 0; ndx < mpp->mpwide; ndx++)
  { 
   np = prtnetmap[ndx].tenu.np;
   /* notice ndx is port bit index but bi is connecting net bit index */
   bi = prtnetmap[ndx].nbi;
   /* DBG remove -- */
   if (np->nlds == NULL || np->nlds->npntyp != NP_MIPD_NCHG)
    __misc_terr(__FILE__, __LINE__);
   /* -- */
   mipdp = &(np->nlds->elnpp.emipdbits[bi]);

   if (!np->n_stren) bv = (byte) __get_initval(np, &stval);
   else { __get_initval(np, &stval); bv = (byte) stval; }
   for (ii = 0; ii < mdp->flatinum; ii++) mipdp->oldvals[ii] = bv;
 
   for (ii = 0; ii < mdp->flatinum; ii++) mipdp->mipdschd_tevs[ii] = -1;

   if (mipdp->pth_mipd)
    {
     /* only need to re-init change times */ 
     for (ii = 0; ii < mdp->flatinum; ii++) 
      {
       impthp = mipdp->impthtab[ii];
       for (; impthp != NULL; impthp = impthp->impthnxt)
        { impthp->lastchg = 0ULL; }
      }
    }
  } 
 __my_free((char *) prtnetmap, mpp->mpwide*sizeof(struct tenp_t));
}

/*
 * access mipd from port and port index
 *
 * for scalar ndx passed as 0 not -1 here
 * only called if port has mipd
 */
extern struct mipd_t *__get_mipd_from_port(struct mod_pin_t *mpp, int32 ndx)
{
 int32 bi;
 struct mipd_t *mipdp;
 struct tenp_t *prtnetmap;
 struct net_t *np;

 prtnetmap = __bld_portbit_netbit_map(mpp);
 np = prtnetmap[ndx].tenu.np;
 bi = prtnetmap[ndx].nbi;
 mipdp = &(np->nlds->elnpp.emipdbits[bi]);
 __my_free((char *) prtnetmap, mpp->mpwide*sizeof(struct tenp_t));
 return(mipdp);
}

/*
 * for a lhs port, malloc and build tenp map from port bit to net bit 
 * know port always lhs here
 */
extern struct tenp_t *__bld_portbit_netbit_map(struct mod_pin_t *mpp)
{
 register struct expr_t *catxp;
 int32 pi;
 struct tenp_t *prtnetmap;

 prtnetmap = (struct tenp_t *) __my_malloc(mpp->mpwide*sizeof(struct tenp_t));

 /* concatenate - add each component */
 if (mpp->mpref->optyp == LCB)
  {
   pi = mpp->mpwide - 1;
   for (catxp = mpp->mpref->ru.x; catxp != NULL; catxp = catxp->ru.x)
    {
     pi -= catxp->lu.x->szu.xclen; 
     add_portbit_map(prtnetmap, catxp->lu.x, pi);
    }
   return(prtnetmap);
  } 
 /* add simple port to map */
 add_portbit_map(prtnetmap, mpp->mpref, 0);
 return(prtnetmap);
}

/*
 * for a lhs port, add non concat lhs port to port to bit map
 * notice nbi index is 0 for scalar
 */
static void add_portbit_map(struct tenp_t *prtnetmap, struct expr_t *xp,
 int32 base_pi)
{
 register int32 pi, bi; 
 int32 psi1, psi2;
 word32 *wp;
 struct net_t *np;
 struct expr_t *ndx;
 
 switch ((byte) xp->optyp) {
  case ID:
   np = xp->lu.sy->el.enp;
   if (!np->n_isavec)
    {
     prtnetmap[base_pi].tenu.np = np;
     prtnetmap[base_pi].nbi = 0;
    }
   else 
    {
     for (pi = base_pi; pi < base_pi + xp->szu.xclen; pi++)
      { prtnetmap[pi].tenu.np = np; prtnetmap[pi].nbi = pi - base_pi; }
    }
   break;
  case OPEMPTY:
   for (pi = base_pi; pi < base_pi + xp->szu.xclen; pi++)
    {
     /* LOOKATME - index illegal -1 here since think won't occur */
     prtnetmap[pi].tenu.np = NULL;
     prtnetmap[pi].nbi = -1;
    }
   break;
  case PARTSEL:
   np = xp->lu.x->lu.sy->el.enp;
   ndx = xp->ru.x;
   wp = &(__contab[ndx->lu.x->ru.xvi]);
   psi1 = (int32) wp[0];
   wp = &(__contab[ndx->ru.x->ru.xvi]);
   psi2 = (int32) wp[0];
   /* part select always constant */
   for (pi = base_pi, bi = psi2; pi < base_pi + xp->szu.xclen; pi++, bi++)
    {
     prtnetmap[pi].tenu.np = np;
     prtnetmap[pi].nbi = bi;
    }
   break;
  case LSB:
   /* LOOKATME - think IS bit selects always split before here */
   /* DBG remove */
   if (xp->ru.x->optyp != NUMBER) __misc_terr(__FILE__, __LINE__);
   /* --- */
   np = xp->lu.x->lu.sy->el.enp;
   ndx = xp->ru.x;
   wp = &(__contab[ndx->ru.xvi]);
   psi1 = (int32) wp[0];
   prtnetmap[base_pi].tenu.np = np;
   prtnetmap[base_pi].nbi = psi1;
   break;
  /* since know only 1 level, removed before here */
  case LCB: __case_terr(__FILE__, __LINE__);
  /* xmr can't connect to port */
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * ROUTINES TO ALLOCATE NCHG ACTION STORAGE
 */

/*
 * allocate nchg byte table and set nchgaction net ptrs
 * even for cver-cc compiler, nchg storage malloc since access through net
 *
 * 08/22/02 - always need nchg store for dmpvars
 * FIXME - should align ptrs at least on 4 byte boundaries
 */
extern void __alloc_nchgaction_storage(void)
{
 register int32 ni;
 register struct net_t *np;
 struct mod_t *mdp;
 struct task_t *tskp;

 __nchgbtabbsiz = 0;
 cmp_nchgbtabsize();
 if (__nchgbtabbsiz > 0)
  {
   __nchgbtab = (byte *) __my_malloc(__nchgbtabbsiz);
  }
 __nchgbtabbi = 0;
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mnnum != 0)
    {
     for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
      {
       /* always need the changed byte array even for event */
       np->nchgaction = (byte *) &(__nchgbtab[__nchgbtabbi]);
       /* set to zero for now - initialize after lds/dces added */
       memset(np->nchgaction, 0, mdp->flatinum);
       __nchgbtabbi += mdp->flatinum; 
      }
    }
   for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     if (tskp->trnum == 0) continue;

     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       /* always need the changed byte array even for event */
       np->nchgaction = (byte *) &(__nchgbtab[__nchgbtabbi]);
       /* set to zero for now - initialize after dces/lds built */
       memset(np->nchgaction, 0, mdp->flatinum);
       __nchgbtabbi += mdp->flatinum;
      }
    }
  }
}

/*
 * compute size of needed nchg byte table in bytes
 * also sets srep since always called
 */
static void cmp_nchgbtabsize(void)
{
 register int32 ni;
 register struct net_t *np;
 struct mod_t *mdp;
 struct task_t *tskp;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mnnum != 0)
    {
     for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
      {
       /* storage for byte per inst. all changed table */
       /* change state bytes go into scalar (byte) storage table */
       __nchgbtabbsiz += mdp->flatinum;

       set_1net_srep(np);
      } 
    }
   for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     if (tskp->trnum == 0) continue;
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       /* always need the changed byte array even for event */
       /* change state bytes go into scalar (byte) storage table */
       __nchgbtabbsiz += mdp->flatinum;

       set_1net_srep(np);
      }
    }
  }
}

/*
 * set the storage rep type for one net
 *
 * SJM 05/04/05 - since not calling alloc storage routine for cver-cc, 
 * must set sreps in separate routine called from nchg routines that
 * are always called
 */
static void set_1net_srep(struct net_t *np)
{
 if (np->ntyp == N_EVENT) return;

 if (np->n_isarr) { np->srep = SR_ARRAY; return; }
 if (np->ntyp == N_REAL) { np->srep = SR_VEC; return; } 
 if (!np->n_isavec)
  {
   if (np->n_stren) np->srep = SR_SSCAL; else np->srep = SR_SCAL;
   return;
  }
 if (!np->n_stren) np->srep = SR_VEC; else np->srep = SR_SVEC;
}

/*
 * initialize nchg action byte table for all modules
 *
 * always assume dumpvars off also after reset (re-initialize)
 * this must be called after dces reinited
 */
extern void __set_nchgaction_bits(void)
{
 register int32 ii, ni;
 register struct net_t *np;
 struct mod_t *mdp;
 struct task_t *tskp;
 
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
    {
     for (ii = 0; ii < mdp->flatinum; ii++)
      {
       /* start with all dumpvars off - now independent of var chg */
       /* all all other fields 0 off */
       np->nchgaction[ii] = NCHG_DMPVNOTCHGED; 

       if (np->ntyp >= NONWIRE_ST)
        {
         /* SJM - 07/01/00 - for regs, if no lds, all var insts stay */
         /* all chged and never record */
         if (np->nlds == NULL) np->nchgaction[ii] |= NCHG_ALL_CHGED;
        }
       else
        { 
         /* SJM 07/24/00 - for wires, if has dces not all changed */
         if (np->nlds == NULL && (np->dcelst == NULL
          || __cnt_dcelstels(np->dcelst) == 0))
          np->nchgaction[ii] |= NCHG_ALL_CHGED;
        }
      }
     /* SJM - 07/01/00 - set various per variable bits in 0th element */
     /* SJM - 03/15/01 - change to fields in net record */
     if (np->nlds != NULL) np->nchg_has_lds = TRUE;

     /* SJM 07/24/00 - only nchg has dces on for regs immediate prop/wakeup */
     if (np->ntyp >= NONWIRE_ST && np->dcelst != NULL)
      np->nchg_has_dces = TRUE;

     /* SJM REMOVEME */
     /* --- 
     if (np->nlds == NULL && np->dcelst == NULL && np->dmpv_in_src)
      __misc_terr(__FILE__, __LINE__); 
     -- */

     /* if any lds, dces or dmpvs, need chg store  */
     if (np->nlds != NULL || np->dcelst != NULL || np->dmpv_in_src)
      np->nchg_nd_chgstore = TRUE;
    }
  
   for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       for (ii = 0; ii < mdp->flatinum; ii++)
        {
         /* start with all dumpvars off - now independent of var chg */
         /* all all other fields 0 off */
         np->nchgaction[ii] = NCHG_DMPVNOTCHGED; 

         /* SJM - 07/01/00 - if no lds, all var insts stay all chged */
         /* now all chged only for loads */
         /* SJM 07/24/00 - here since know reg, all chged if no lds */
         if (np->nlds == NULL) np->nchgaction[ii] |= NCHG_ALL_CHGED;
        }
       /* SJM - 07/01/00 - set various per var bits in 0th element */
       /* SJM 03/15/01 - change to fields in net record */
       if (np->nlds != NULL) np->nchg_has_lds = TRUE;

       /* SJM 07/24/00 - only nchg has dces on for regs but task vars regs */
       if (np->dcelst != NULL) np->nchg_has_dces = TRUE;

       /* if any lds, dces or dmpvs, need chg store  */
       if (np->nlds != NULL || np->dcelst != NULL || np->dmpv_in_src)
        np->nchg_nd_chgstore = TRUE;
      }
    }
  }
}

/*
 * set computed optimtab bits for all vars in entire design
 */
extern void __set_optimtab_bits(void)
{
 register int32 ni;
 register struct net_t *np;
 struct mod_t *mdp;
 struct task_t *tskp;
 
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
    {
     /* assume need chg store */
     if (__dv_allform_insrc || mdp->mod_dvars_in_src)
      {
       np->dmpv_in_src = TRUE;
       np->nchg_nd_chgstore = TRUE;
      }
    }
  
   for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       if (__dv_allform_insrc || mdp->mod_dvars_in_src)
        {
         np->dmpv_in_src = TRUE;
         np->nchg_nd_chgstore = TRUE;
        }
      }
    }
  }
}

/*
 * ROUTINES TO ALLOCATE VARIABLE STORAGE
 */

/*
 * allocate storage for all simulation variables
 */
extern void __alloc_sim_storage(void)
{
 register int32 ni;
 register struct net_t *np;
 struct mod_t *mdp;
 struct task_t *tskp;

 /* always calculate var storage size - but only emit for interpreter */
 __wtabwsiz = 0;
 __btabbsiz = 0; 
 cmp_tabsizes();

 /* for cver-cc, gen .comm lable in bss section */
 /* LOOKATME - is 0 storage size possible - think yes */
 if (__btabbsiz > 0)
  {
   __btab = (byte *) __my_malloc(__btabbsiz);
  }
 if (__wtabwsiz > 0)
  {
   __wtab = (word32 *) __my_malloc(__wtabwsiz*sizeof(word32));
  }
 __wtabwi = 0; 
 __btabbi = 0; 
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   if (__inst_mod->mnnum != 0)
    {
     for (ni = 0, np = &(__inst_mod->mnets[0]); ni < __inst_mod->mnnum;
      ni++, np++) 
      {
       /* no allocated storage here for parameters - in different list */
       /* also none for events (if dce will be allocated when used) */
       if (np->ntyp == N_EVENT) continue;

       /* for now always zeroing variables - when x/z */
       /* could free later if no fan-in and no fan-out */
       alloc_var(np);
      } 
    }
   for (tskp = __inst_mod->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     if (tskp->trnum == 0) continue;
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       if (np->ntyp == N_EVENT) continue;

       /* for now always zeroing variables - when x/z */
       alloc_var(np);
      }
    }
   __pop_wrkitstk();
  }
}

/*
 * compute size of needed startage tables
 *
 * PORTABILITY FIXME - maybe need 8 byte alignment for ptr too?
 * SJM 05/02/05 - now setting net srep here
 */
static void cmp_tabsizes(void)
{
 register int32 ni;
 register struct net_t *np;
 struct mod_t *mdp;
 struct task_t *tskp;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   if (__inst_mod->mnnum != 0)
    {
     for (ni = 0, np = &(__inst_mod->mnets[0]); ni < __inst_mod->mnnum;
      ni++, np++) 
      {
       /* no allocated storage here for parameters - in different list */

       /* also none for events (if dce will be allocated when used) */
       if (np->ntyp == N_EVENT) continue;

       /* because arrays may be large, must really alloc - so not counted */ 
       if (np->n_isarr) continue;

       cmpadd_1var_storsiz(np);
      } 
    }
   for (tskp = __inst_mod->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     if (tskp->trnum == 0) continue;
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       if (np->ntyp == N_EVENT) continue;

       /* no allocated storage here for parameters - in different list */
       /* SJM 05/02/05 - was previously counting task arrays as part of */
       /* the tab storage but still mallocing for interpreter */
       if (np->n_isarr) continue;

       /* for now always zeroing variables - when x/z */
       cmpadd_1var_storsiz(np);
      }
    }
   __pop_wrkitstk();
  }
}

/*
 * computer size for one variable
 *
 * never called for memories
 */
static void cmpadd_1var_storsiz(struct net_t *np)
{
 register int32 insts;

 /* allocate array of srep structs for each inst. */
 insts = __inst_mod->flatinum;

 /* 05/03/05 - reals now just put in wtab (take 2 words) */
 if (np->ntyp == N_REAL) { __wtabwsiz += 2*insts; return; }

 /* compute needed size in bits */
 /* non vectors */
 if (!np->n_isavec)
  {
   /* SJM 10/16/99 - now scalars always 1 byte even non strength */
   __btabbsiz += insts;
  }
 else
  {
   if (!np->n_stren)
    {
     /* hard non strength packed vector case - bits later converted to wrds */
     /* SJM 12/16/99 - now packed vector packs from 2 to 16 bits into 1 word */
     /* SJM 07/15/00 - now for vars only bits packed into bytes */
     __wtabwsiz += 2*wlen_(np->nwid)*insts;
    }
   else
    { 
     /* strength vector 1 byte per bit case */
     __btabbsiz += insts*np->nwid;
    }
  }
}

/*
 * allocate storage for a variable
 * know at this point storage form is compile (ct) union member
 * allocation here is module specific - all inst here indistinguishable
 * but at this point types determine initial values
 */
static void alloc_var(struct net_t *np)
{
 int32 insts;

 /* allocate array of srep structs for each inst. */
 insts = __inst_mod->flatinum;

 /* need to handle REAL as special case - has special representation */
 /* and now real can be array */
 if (np->ntyp == N_REAL) { alloc_real_var(np, insts); return; }

 if (np->n_isarr) __allocinit_arr_var(np, insts, TRUE);
 else if (!np->n_isavec)
  {
   if (!np->n_stren) alloc_scal_var(np, insts);
   else alloc_sscal_var(np, insts);
  }
 else
  {
   if (!np->n_stren) __allocinit_vec_var(np, insts, TRUE);
   else alloc_svec_var(np, insts);
  }
}

/*
 * initialize a real variable
 */
static void alloc_real_var(struct net_t *np, int32 insts)
{
 register int32 i;
 int32 arrw, totchars;
 double *dp;

 /* case 1: new real array - must be malloced */
 /* must malloc arrays because they can be large */
 if (np->n_isarr)
  {
   arrw = __get_arrwide(np);
   totchars = arrw*(2*WRDBYTES*insts*wlen_(REALBITS));
   np->nva.wp = (word32 *) __my_malloc(totchars);
   /* reals arrays contiguous a/b 8 bytes with no x/z */
   dp = np->nva.dp;
   for (i = 0; i < arrw*insts; i++)
    {
     *dp++ = 0.0;
    }
   __arrvmem_use += totchars;
   return;
  }

 /* case 2 non array */
 np->nva.wp = (word32 *) &(__wtab[__wtabwi]);
 dp = np->nva.dp;
 for (i = 0; i < insts; i++) *dp++ = 0.0; 
 __wtabwi += 2*insts;
}

/*
 * allocate all instances for a array var
 *
 * notice for now to access must call get packbits - store somewhere
 * never need to access nva through stu strength union because array cannot
 * have strength 
 *
 * initialization easy since know arrays are registers that are always
 * initialized to x's
 * notice this routine is somewhat dependent on 32 bit words
 */
extern void __allocinit_arr_var(struct net_t *np, int32 insts,
 int32 nd_alloc)
{
 register int32 i;
 int32 arrw, wlen, totchars, elwlen, totcells;
 word32 *rap, mask;

 totchars = 0;
 /* arrw is number of cells in memory */
 arrw = __get_arrwide(np);
 /* case 1, each cell is a scalar */
 if (!np->n_isavec)
  {
   wlen = wlen_(2*arrw*insts);
   if (nd_alloc)
    {
     totchars = WRDBYTES*wlen;
     np->nva.wp = (word32 *) __my_malloc(totchars);
     __arrvmem_use += totchars;
    }
   /* notice packed densly, index by cell array of 2 bit cells */
   for (i = 0; i < wlen; i++) np->nva.wp[i] = ALL1W;
   /* must mask off unused bits in last word32 */
   np->nva.wp[wlen - 1] &= __masktab[ubits_(2*arrw*insts)];
   goto done;
  }

 /* case 2: each cell cannot be packed */
 if (np->nwid > WBITS/2)
  {
   if (nd_alloc)
    {
     wlen = arrw*wlen_(np->nwid);
     totchars = 2*WRDBYTES*insts*wlen;
     np->nva.wp = (word32 *) __my_malloc(totchars);
     __arrvmem_use += totchars;
    }

   /* array is linear array of arrw*insts elements */
   /* each element has 2 elwlen x (1w) regions */
   elwlen = wlen_(np->nwid);
   rap = np->nva.wp;
   totcells = arrw*insts;
   for (i = 0;;) 
    {
     one_allbits_(rap, np->nwid);
     rap = &(rap[elwlen]);
     one_allbits_(rap, np->nwid); 
     if (++i >= totcells) break; 
     rap = &(rap[elwlen]);
    }
   goto done;
  }
 /* case 3a: packs into byte */
 if (np->nwid <= 4)
  {
   /* each cell has 1 byte */
   if (nd_alloc)
    {
     totchars = arrw*insts;
     np->nva.bp = (byte *) __my_malloc(totchars);
     __arrvmem_use += totchars;
    }
   /* pack into 2 contiguous low bit side sections of byte */
   mask = __masktab[2*np->nwid];
   for (i = 0; i < arrw*insts; i++) np->nva.bp[i] = (byte) mask;
   goto done;
  }
 /* case 3b: packs into half word32 */
 if (np->nwid <= 8)
  {
   if (nd_alloc)
    {
     totchars = 2*arrw*insts;
     np->nva.hwp = (hword *) __my_malloc(totchars);
     __arrvmem_use += totchars;
    }
   mask = __masktab[2*np->nwid];
   for (i = 0; i < arrw*insts; i++) np->nva.hwp[i] = (hword) mask;
   goto done;
  }
 /* case 3c: pcks in word32 */
 if (nd_alloc)
  {
   totchars = 4*arrw*insts;
   np->nva.wp = (word32 *) __my_malloc(totchars);
   __arrvmem_use += totchars;
  }
 mask = __masktab[2*np->nwid];
 for (i = 0; i < arrw*insts; i++) np->nva.wp[i] = mask;

done:
 if (__debug_flg && nd_alloc)
  {
   __dbg_msg(
    "==> array %s: %d insts of %d, %d bit per inst. cells uses %d bytes\n",
    np->nsym->synam, insts, arrw, np->nwid, totchars);
  }
}

/*
 * allocate all instances for a scalar var
 * storage here is 2 contiguous bits for a and b value of scalar
 * needed because eliminates need for unavailable total insts value
 *
 * notice important optimization here 1 bit per non strenth scalar,
 * but selection now takes array access, shift, and and
 */
static void alloc_scal_var(struct net_t *np, int32 insts)
{
 int32 ival, stval;

 /* variables accessed as section of design wide storage table */
 np->nva.bp = &(__btab[__btabbi]);
 __btabbi += insts;

 ival = __get_initval(np, &stval);
 if (ival == 0L) memset(np->nva.bp, 0, insts);
 /* initialize net storage */
 else set_byteval_(np->nva.bp, insts, ival);
}

/*
 * determine the initial value and strength for any wire type
 * stval is entire 8 bit value - ival returned is low 2 bits only
 * strength format is (st0 (7-5), st1 (4-2), val (1-0) 
 */
extern int32 __get_initval(struct net_t *np, int32 *stval)
{
 int32 ival, sval;

 switch ((byte) np->ntyp) { 
  case N_WIRE: case N_TRI: case N_TRIAND: case N_WA: case N_TRIOR: case N_WO:
   /* normal wires are z (hiz(0),hiz(0), z) */
   /* SJM 02/16/07 - initialize to x if net has drivers otherwise z */

   /* SJM 03/16/07 - if wire is an inout special case - may have npp bid */
   /* mod port drvrs here, but since inout are in tran channels removed */
   /* later - if all drivers are bid ports, really has no drivers */
   if (np->ndrvs == NULL || all_drvrs_bidirect(np))
    {
     ival = 2; 
     sval = ST_HIZ;
    }
   else
    {
     ival = 3; 
     sval = 0xdb;
    }
   break;
  /* these are normal wires, that have pull0 or pull driver added */  
  case N_TRI0: ival = 0; sval = ST_PULL0; break; /* <5:5>=0 Pu0 10110100 */
  case N_TRI1: ival = 1; sval = ST_PULL1; break; /* <5:5>=1 pu1 10110101 */
  case N_TRIREG:
   /* even if delay do not schedule decay to z at initialize */
   ival = 3; 
   if (np->n_capsiz == CAP_LARGE) sval = 0x93; /* <4:4>=x LaX 10010011 */
   else if (np->n_capsiz == CAP_SMALL) sval = 0x27; /* <1:1>=x SmX 00100111 */
   else sval = 0x4b; /* <2:2>=x MeX 01001011 */
   break;
  /* <7:7>=1 Su1 ll111101 */
  case N_SUPPLY1: ival = 1; sval = ST_SUPPLY1; break;
 /* <7:7>=0 Su0 11111100 */
  case N_SUPPLY0: ival = 0; sval = ST_SUPPLY0; break;
  /* register initialized to 0 */
  case N_REG: case N_INT: case N_TIME: case N_EVENT:
   ival = 3; sval = 0xdb; /* strength meaningless but <6:6>=X Stx 11011011 */
   break;
  default: __case_terr(__FILE__, __LINE__); return(0);
 } 
 *stval = sval;
 return(ival); 
}

/*
 * return T if all drivers are bid mdprt drivers because they are all 
 * removed later when inout are converted to switch channels
 */
static int32 all_drvrs_bidirect(struct net_t *np)
{
 register struct net_pin_t *npp;  

 for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
  {
   if (npp->npntyp != NP_BIDMDPRT && npp->npntyp != NP_BIDICONN)
    return(FALSE);
  }
 return(TRUE);
}

/*
 * allocate all instances for a strength scalar var
 * 1 byte per value (low 2 bits value), next 3 bits 1 stren, high 3 0 stren 
 */
static void alloc_sscal_var(struct net_t *np, int32 insts)
{
 byte *sbp;
 byte sval;
 int32 stval;

 /* first allocate the normal 2 bits packed per inst values */
 /* for sscal 1 byte per instance for strength and value (s000,s111,vv) */
 __get_initval(np, &stval);
 /* here byte array value allocated in some words */
 sbp = &(__btab[__btabbi]);
 __btabbi += insts;
 sval = (byte) stval;
 set_byteval_(sbp, insts, sval);
 np->nva.bp = sbp;
}

/*
 * allocate all instances for a vector variable
 */
extern void __allocinit_vec_var(struct net_t *np, int32 insts,
 int32 nd_alloc)
{
 int32 ival, stval;
 int32 wlen, totchars;
 word32 maska, maskb;

 ival = __get_initval(np, &stval);
 maska = maskb = 0L;
 /* SJM 07/15/00 - no longer pack <16 bit vecs - still pack scalar in byte */ 
 wlen = wlen_(np->nwid);
 totchars = 2*WRDBYTES*insts*wlen;
 if (nd_alloc)
  {
   np->nva.wp = &(__wtab[__wtabwi]);
   __wtabwi += 2*insts*wlen;
  }
 switch ((byte) ival) {
  case 0: memset(np->nva.wp, 0, totchars); return;
  case 1: maska = 0xffffffffL; break;
  case 2: maskb = 0xffffffffL; break;
  case 3: maska = 0xffffffffL; maskb = 0xffffffffL; break;
  default: __case_terr(__FILE__, __LINE__);
 }
 /* initialize net storage */
 __init_vec_var(np->nva.wp, insts, wlen, np->nwid, maska, maskb);
}

/*
 * build a packed mask depending on ival 
 */

/*
 * initialize a non strength vector (non packed) to z 
 */
extern void __init_vec_var(register word32 *wp, int32 insts, int32 wlen,
 int32 vecw, word32 maska, word32 maskb)
{ 
 register int32 ii, wi;
 word32 *iwp;
 int32 ubits;

 /* insts number of <= 1 word32 vectors that each occuppy part of 1 word32 */
 /* use normal full word32 initialization masks but make sure high bits 0 */
 if (vecw <= WBITS)
  {
   maska &= __masktab[vecw];
   maskb &= __masktab[vecw];
   /* for vectors <= WBITS, alternate high 0 masked init values per inst. */
   for (ii = 0; ii < insts; ii++) { *wp++ = maska; *wp++ = maskb; }
   return;
  }
 ubits = ubits_(vecw);
 iwp = wp;
 /* insts number of multiword per vector elements */
 for (ii = 0; ii < insts; ii++) 
  {
   /* for 1 insts's vector value, initial a part */
   for (wi = 0; wi < wlen; wi++) iwp[wi] = maska;
   iwp[wlen - 1] &= __masktab[ubits]; 

   /* then b part */
   for (wi = wlen; wi < 2*wlen; wi++) iwp[wi] = maskb;
   iwp[2*wlen - 1] &= __masktab[ubits]; 
   /* finally move iwp to vector location for next inst. */
   iwp = &(iwp[2*wlen]);
  }
}

/*
 * allocate all instances for a strength vector variable
 * using byte vector here so total if no. of bits in vec times no. of insts
 */
static void alloc_svec_var(struct net_t *np, int32 insts)
{
 int32 stval, totbits;
 byte sval;
 byte *bp;

 totbits = insts*np->nwid;
 bp = &(__btab[__btabbi]);
 __btabbi += totbits;

 __get_initval(np, &stval);
 sval = (byte) stval;
 set_byteval_((char *) bp, totbits, sval);
 np->nva.bp = bp;
}

/*
 * routine to re-initialize variables for 1 module
 *
 * this just reinits wire/reg value - after dces reinited sets chg action bits
 */
extern void __reinitialize_vars(struct mod_t *mdp)
{
 register int32 ni;
 register struct net_t *np;
 register struct task_t *tskp;

 if (mdp->mnnum == 0) goto do_tasks; 
 for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
  {
   /* if any instances any bits in tran channel, need to re-init */
   /* all hard drivers */ 
   if (np->ntraux != NULL)
    {
     if (np->n_stren)
      __allocinit_stperival(&np->ntraux->trnva, mdp->flatinum, np, FALSE);
     else
      __allocinit_perival(&np->ntraux->trnva, mdp->flatinum, np->nwid, FALSE);
    }
   /* need all changed even for event */
   if (np->ntyp == N_EVENT) continue;
   reinit_1wirereg(np, mdp);
  }

do_tasks:
 for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt) 
  {
   if (tskp->trnum == 0) continue;
   for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
    {
     if (np->ntyp == N_EVENT) continue;
     reinit_1wirereg(np, mdp);
    }
  }
}

/*
 * reinitialize 1 wire or reg
 */
static void reinit_1wirereg(struct net_t *np, struct mod_t *mdp)
{
 register int32 i;
 register struct qcval_t *qcvalp;
 int32 stval, arrw, ival;
 byte *sbp, sval;
 double d1;
 i_tev_ndx *itevpp;

 d1 = 0.0; 
 /* initialize per bit per inst scheduled event tab if needed */
 if (np->nrngrep == NX_DWIR)
  {
   itevpp = np->nu.rngdwir->wschd_pbtevs;
   for (i = 0; i < mdp->flatinum*np->nwid; i++) itevpp[i] = -1;
  }
 if (np->frc_assgn_allocated)
  {
   if (np->ntyp >= NONWIRE_ST)
    {
     qcvalp = np->nu2.qcval;
     /* reset all force/assigns to off and turn off any dces for active */
     for (i = 0; i < 2*mdp->flatinum; i++, qcvalp++)
      {
       qcvalp = &(np->nu2.qcval[i]);
       if (qcvalp->qc_active)
        { 
         /* turn off dces then empty fields by re-initializing record */
         if (qcvalp->qcdcep != NULL) __dcelst_off(qcvalp->qcdcep); 
         init_qcval(qcvalp); 
        }
       else if (qcvalp->qc_overridden)
        {
         /* here fields filled so need to re-init but dces off */
         init_qcval(qcvalp); 
        }
      }
    }
   else
    {
     /* reset all force/assigns on to off and reinit - no assign of wire */
     /* forces per bit */
     for (i = 0; i < np->nwid*mdp->flatinum; i++, qcvalp++)
      {
       qcvalp = &(np->nu2.qcval[i]);
       if (qcvalp->qc_active)
        { 
         if (qcvalp->qcdcep != NULL) __dcelst_off(qcvalp->qcdcep); 
         init_qcval(qcvalp); 
        }
      }
    }
  }
 /* reinitialize any pending vpi_put_value records */
 /* leave the record but change driver to nil and cancel any scheduled tevs */ 
 if (np->ntyp < NONWIRE_ST)
  { 
   if (np->vpi_ndrvs != NULL) __reinit_netdrvr_putvrec(np, mdp);
   if (np->regwir_putv_tedlst != NULL)
    __reinit_regwir_putvrec(np, mdp->flatinum);
  }
 else 
  {
   if (np->regwir_putv_tedlst != NULL)
    __reinit_regwir_putvrec(np, mdp->flatinum);
  }

 if (np->ntyp == N_REAL)
  {
   if (np->n_isarr)
    {
     arrw = __get_arrwide(np);
     /* LOOKATME - assumes real fits in 8 bytes and WBITS is 32 */
     for (i = 0; i < arrw*mdp->flatinum; i++)
      {
       memcpy(&(np->nva.wp[2*i]), &d1, sizeof(double));
      }
     return;
    }
   for (i = 0; i < mdp->flatinum; i++)
    {
     memcpy(&(np->nva.wp[2*i]), &d1, sizeof(double));
    } 
   return;
  }
 /* not for real arrays */
 if (np->n_isarr) { __allocinit_arr_var(np, mdp->flatinum, FALSE); return; }
 if (!np->n_isavec)
  {
   if (!np->n_stren)
    {
     ival = __get_initval(np, &stval);
     if (ival == 0) memset(np->nva.bp, 0, mdp->flatinum);
     else set_byteval_(np->nva.bp, mdp->flatinum, ival);
     return;
    }
   __get_initval(np, &stval);
   sbp = np->nva.bp;
   sval = (byte) stval;
   set_byteval_(sbp, mdp->flatinum, sval);
   return;
  }
 if (!np->n_stren) { __allocinit_vec_var(np, mdp->flatinum, FALSE); return; }
 __get_initval(np, &stval);
 sval = (byte) stval;
 set_byteval_(np->nva.bp, mdp->flatinum*np->nwid, sval);
}

/*
 * initialize all dces (and tchk old vals) in design
 */
extern void __initialize_dsgn_dces(void)
{
 register struct mod_t *mdp;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);
   __initialize_dces(__inst_mod);
   __pop_wrkitstk();
  }
}

/*
 * routine to re-initialize dces (and tchk npp old vals) for 1 module
 *
 * do not need to (re)initialize npps except path and timing check which have
 * internal state - normal npp's need processing on any kind of change
 */
extern void __initialize_dces(struct mod_t *mdp)
{
 register int32 i, ni;
 register struct net_t *np;
 register struct net_pin_t *npp;
 int32 insts;
 word64 tim0;
 struct tchg_t *tchgp;
 struct chktchg_t *chktcp;
 struct task_t *tskp;

 insts = mdp->flatinum;
 for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
  {
   if (np->ntyp == N_EVENT) goto skip_spec;

   /* reinitialize timing check state values - only accessible thru npps */
   /* if no specify section do not need to go through npp loads list */
   if (mdp->mspfy != NULL)
    {
     tim0 = 0ULL;
     for (npp = np->nlds; npp != NULL; npp = npp->npnxt) 
      {
       if (npp->npntyp != NP_TCHG) continue; 

       switch ((byte) npp->chgsubtyp) {
        case NPCHG_TCSTART: case NPCHG_PTHSRC:
         tchgp = npp->elnpp.etchgp;
         reinit_npp_oldval(tchgp->oldval, np, mdp);
         for (i = 0; i < insts; i++) tchgp->lastchg[i] = tim0;
         break;
        case NPCHG_TCCHK:
         chktcp = npp->elnpp.echktchgp;
         reinit_npp_oldval(chktcp->chkoldval, np, mdp);
         for (i = 0; i < insts; i++) chktcp->chklastchg[i] = tim0;
         break;
        default: __case_terr(__FILE__, __LINE__);
       }
      }
    }
skip_spec:
   __init_1net_dces(np, mdp);
  }
 for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt) 
  {
   if (tskp->trnum == 0) continue;
   for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
    {
     /* LOOKATME - think need to do this for events too */
     __init_1net_dces(np, mdp);
    }
  }
}

/*
 * initialize one net's dces (called faster cver-cc compile and $reset)
 *
 * for XMR mdp is the define (target) itree loc since dce on define var
 * FIXME (OR FINDOUT) - why is algorithm to remove PLI 1.0 vcls
 * but leave PLI 2.0 val chg cbs
 */
extern void __init_1net_dces(struct net_t *np, struct mod_t *mdp)
{
 int32 insts;
 struct dcevnt_t *dcep; 
 struct mod_t *ref_mdp;

 insts = mdp->flatinum;
 /* must go through setting all dce schedule per inst tables to nil */
 for (dcep = np->dcelst; dcep != NULL;)
  {
   switch (dcep->dce_typ) {
    case DCE_RNG_INST: case DCE_INST:
     /* set per inst. schedule table to nil but leave this type of dce */
     /* not per bit since filter applied to range */
     /* also for dce forms accessed from ref. not target itree loc. */

     /* DBG remove --- */
     if (dcep->st_dctrl->dceschd_tevs == NULL)
       __misc_terr(__FILE__, __LINE__);
     /* --- */

     /* SJM 10/07/06 - NOTICE that dce previous values are indexed by */ 
     /* declare in (target) instance number and have that size but */
     /* because dce scheduled tevs are accessed while arming from ref */
     /* point, the dce schd tev table is the size of and index by ref mod */

     /* SJM 10/07/06 - since init called from declared in (targ) itree loc */
     /* need to use the defined in mdp number of insts for XMRs */
     /* AIV 03/03/07 - made a routine to get the ref mod */
     ref_mdp = dcep_ref_mod(dcep);

     /* SJM 10/07/06 - for XMR dces (@(i1.reg) say), the schd tev table */
     /* has the size of the referenced in (in mdp) module and is accessed */
     /* by the referencing (used in) inum - XMR dce prevals are accessed */
     /* and have the size of the declare in (target) module */
     /* AIV 03/03/07 - these are init when alloced for the intep  */
     /* they are init when linked in for the compiler as well */
     /*
     for (i = 0; i < ref_mdp->flatinum; i++)
      {
       dcep->st_dctrl->dceschd_tevs[i] = -1;
      }
     */

     /* AIV 01/04/07 - init dce expr was using the wrong inst for */
     /* dce with more than one inst and was also skipping init for the */
     /* dce_expr for the one instance case */
     /* set dce previous values to initial wire value */
     if (dcep->dce_expr != NULL) init_dce_exprval(dcep);
     else
      {
       if (dcep->prevval.wp != NULL)
        {
         /* 05/18/03 - for XMR there is one for each decl in inst */
         init_dce_prevval(dcep, ref_mdp);
        }
      }
     break;
    case DCE_RNG_MONIT: case DCE_MONIT:
     /* DBG remove -- */
     if (!dcep->dce_1inst) __misc_terr(__FILE__, __LINE__);
     /* --- */
   
     /* SJM 12/30/02 - since monits can't be removed - only turned off */
     /* on reset must turn off and initialize to start value */
     /* previous fix was wrong */
     dcep->dce_off = TRUE;
     /* initialize old value as if this was first time - can never be XMR */
     __push_itstk(dcep->dce_matchitp);
     __init_1instdce_prevval(dcep);
     __pop_itstk();
     break;
    case DCE_RNG_QCAF: case DCE_QCAF:
     /* always 1 qca dce load per statement exec */
     /* never a previous value since better to repeat assign of forced */
     /* DBG remove --- */
     if (dcep->prevval.bp != NULL) __misc_terr(__FILE__, __LINE__);
     /* --- */
     /* 11/22/02 AIV - no longer freeing QCAF dces - turned off instead */
     /* also no previous value to re-initialize */
     dcep->dce_off = TRUE;
     break;
    case DCE_RNG_PVC: case DCE_PVC:
     /* DBG remove -- */
     if (!dcep->dce_1inst) __misc_terr(__FILE__, __LINE__);
     /* --- */

     /* SJM 01/02/03 - for the dce - always inst specific - just reinit in */
     /* case has previous value */
     __push_itstk(dcep->dce_matchitp);
     __init_1instdce_prevval(dcep);
     __pop_itstk();
     break;
    case DCE_CBVC: case DCE_RNG_CBVC:
    case DCE_CBF: case DCE_RNG_CBF: case DCE_CBR: case DCE_RNG_CBR:
     /* DBG remove -- */
     if (!dcep->dce_1inst) __misc_terr(__FILE__, __LINE__);
     /* --- */

     /* this handles re-init of PLI 2.0 dce that must be left on */
     /* know will always exist */  
     /* LOOKATME - why left on? or why not consistent with PLI 1 */
     __push_itstk(dcep->dce_matchitp);
     __alloc_1instdce_prevval(dcep);
     __pop_itstk();
     break;
    /* notice iact never seen here since only enabled from iact stmt */
    default: __case_terr(__FILE__, __LINE__);
   }
   dcep = dcep->dcenxt;
  }
}

/*
 * allocate normal multiple instance case dce prevval
 * need value for every instance (itree loc. not used)
 *
 * 05/07/03 - now separate alloc and initialize
 */
static void alloc_dce_prevval(struct dcevnt_t *dcep, struct mod_t *mdp)
{
 int32 dcewid, totchars;
 struct net_t *np;

 np = dcep->dce_np;
 /* PLI change values always need previous value because >1 change during */ 
 /* one time slot possible */
 if (dcep->dce_typ < ST_ND_PREVVAL)
  {
   /* no previous value for arrays or non edge entire wire regs */
   if (np->n_isarr || (np->ntyp >= NONWIRE_ST && dcep->dci1 == -1
    && !dcep->dce_edge))
    return;
  }


 /* build old value for wire range change detection */ 
 dcewid = __get_dcewid(dcep, np);
 if (np->n_stren)
  {
   /* notice this can never be array */
   dcep->prevval.bp = (byte *) __my_malloc(dcewid*mdp->flatinum);
  }
 else
  {
   totchars = __get_pcku_chars(dcewid, mdp->flatinum);
   dcep->prevval.wp = (word32 *) __my_malloc(totchars);
  }
}

/*
 * initialize by setting to current value of wire the dce preval
 * need value for every instance (itree loc. not used)
 *
 * SJM 05/07/03 - now separate alloc and initialize
 * only called if dce prev val non nil
 */
static void init_dce_prevval(struct dcevnt_t *dcep, struct mod_t *decl_mdp)
{
 register int32 ii;
 int32 i1;
 word32 *wp;
 byte *sbp, *sbp2, *sbp3;
 int32 dcewid;
 struct net_t *np;
 struct xstk_t *xsp;

 np = dcep->dce_np;
 /* build old value for wire range change detection */ 
 dcewid = __get_dcewid(dcep, np);
 if (np->n_stren)
  {
   /* notice this can never be array */
   sbp = dcep->prevval.bp;
   if (dcep->dci1 == -2)
    {
     /* SJM 10/12/04 - because contab realloced, must be ndx base of IS */
     wp = &(__contab[dcep->dci2.xvi]);
     for (ii = 0; ii < decl_mdp->flatinum; ii++)
      {
       /* know i1 not -1 since must be 1 bit */
       i1 =  (int32) wp[2*ii];
       sbp2 = &(np->nva.bp[np->nwid*ii + i1]);
       sbp3 = &(sbp[ii*dcewid]);
       memcpy(sbp3, sbp2, dcewid); 
      }
    }
   else
    {
     /* here set every instance */
     for (ii = 0; ii < decl_mdp->flatinum; ii++)
      {
       /* notice start addr. must be low (2nd) index */
       i1 = (dcep->dci1 == -1) ? 0 : dcep->dci2.i;
       sbp2 = &(np->nva.bp[np->nwid*ii + i1]);
       sbp3 = &(sbp[ii*dcewid]);
       memcpy(sbp3, sbp2, dcewid); 
      }
    }
   return;
  }
 push_xstk_(xsp, dcewid);
 /* dummy itree loc needed so can change inum */
 __push_wrkitstk(decl_mdp, 0);
 /* this is impossible for monit form */ 
 if (dcep->dci1 == -2)
  {
   /* SJM 10/12/04 - because contab realloced, must be ndx base of IS */
   wp = &(__contab[dcep->dci2.xvi]);
   /* know this is all inst. since monit/iact cannot be -2 form */
   for (ii = 0; ii < decl_mdp->flatinum; ii++)
    {
     /* no need to access mod con table here */
     __inst_ptr->itinum = ii;
     __inum = ii;
     i1 =  (int32) wp[2*ii];
     __ld_wire_sect(xsp->ap, xsp->bp, np, i1, i1);
     /* minus 2 form only possible for bit select */
     st_scalval_(dcep->prevval.bp, xsp->ap[0], xsp->bp[0]);
    }
  }
 else
  {
   for (ii = 0; ii < decl_mdp->flatinum; ii++)
    {
     /* no need to access mod con table here */
     __inst_ptr->itinum = ii;
     __inum = ii;
     /* notice dci1 will be -1 for entire wire and this handles */ 
     __ld_wire_sect(xsp->ap, xsp->bp, np, dcep->dci1, dcep->dci2.i);
     __st_perinst_val(dcep->prevval, dcewid, xsp->ap, xsp->bp);
    }  
  }
 __pop_wrkitstk();
 __pop_xstk();
}

/*
 * initialize dce expr old (expr. not variable) value 
 *
 * only called if dce has expr. and this is master (1st) for XMR 
 *
 * since expr evaluated in ref XMR loc, module context is referenced module
 * or XMR case which is set by caller
 *
 * SJM 05/06/03 - need to eval and save in ref mod loc XMR dce case
 * SJM 05/04/05 - notice this 
 */
static void init_dce_exprval(struct dcevnt_t *dcep)
{
 register int32 ii;
 struct xstk_t *xsp;
 struct mod_t *ref_mdp;
 struct gref_t *grp;

 if (dcep->dce_xmrtyp != XNP_LOC)
  {
   if (dcep->dce_xmrtyp == XNP_RTXMR)
    {
     ref_mdp = dcep->dce_refitp->itip->imsym->el.emdp;
    }
   else
    {
     grp = dcep->dceu.dcegrp;
     ref_mdp = grp->gin_mdp;
    }
  }
 else ref_mdp = __inst_mod;

 /* edges always 1 bit (maybe low of vector) */

 /* LOOKATME - can expr. be evaluated here? probably since can load wire */ 
 /* need to eval. from initialized wires */
 /* SJM 05/06/03 - must eval expr in ref loc itree context not define */
 for (ii = 0; ii < ref_mdp->flatinum; ii++)
  {
   __push_itstk(ref_mdp->moditps[ii]);
   xsp = __eval_xpr(dcep->dce_expr->edgxp);
   st_scalval_(dcep->dce_expr->bp,
    (xsp->ap[0] & 1L), (xsp->bp[0] & 1L));
   __pop_xstk();
   __pop_itstk();
  }
}

/*
 * return the reference dcep mod 
 */
static struct mod_t *dcep_ref_mod(struct dcevnt_t *dcep)
{
 struct mod_t *ref_mdp;
 struct itree_t *itp;

 if (dcep->dce_xmrtyp == XNP_UPXMR || dcep->dce_xmrtyp == XNP_DOWNXMR)
  {
   ref_mdp = dcep->dceu.dcegrp->targmdp;
  }
 /* AIV 03/01/07 - rooted must be linked with its containing mod */
 else if (dcep->dce_xmrtyp == XNP_RTXMR)
  {
   /* AIV 03/03/07 - should be match itp not ref */
   itp = dcep->dce_matchitp;
   ref_mdp = itp->itip->imsym->el.emdp;
  }
 else ref_mdp = __inst_mod;
 return(ref_mdp);
}

/*
 * allocate one inst form dce
 *
 * since called before dce filled, can only allocate - can't initialize
 * variant for one inst forms - monit and XMR
 * this also figures out if previous value needed for 1i case
 */
extern void __alloc_1instdce_prevval(struct dcevnt_t *dcep)
{
 int32 dcewid, totchars;
 struct net_t *np;
 struct mod_t *ref_mdp;

 /* SJM 05/08/03 - dce expr can never be 1 inst - always var and never XMR */
 /* DBG remove -- */
 if (dcep->dce_expr != NULL) __misc_terr(__FILE__, __LINE__); 
 /* --- */

 np = dcep->dce_np;

 /* always need prevval for PLI, multiple change at same time possible */ 
 if (dcep->dce_typ < ST_ND_PREVVAL)
  { 
   /* no previous value for arrays or non edge entire wire regs */
   /* but needed for all others and always build if nd prev val T */ 
   if (np->n_isarr || (np->ntyp >= NONWIRE_ST && dcep->dci1 == -1
    && !dcep->dce_edge)) return;
  }

 dcewid = __get_dcewid(dcep, np);
 if (np->n_stren) dcep->prevval.bp = (byte *) __my_malloc(dcewid);
 else
  {
   ref_mdp = dcep_ref_mod(dcep);
   totchars = __get_pcku_chars(dcewid, ref_mdp->flatinum);
   dcep->prevval.wp = (word32 *) __my_malloc(totchars);
  }
}

/*
 * initialize and set to current value into dce preval
 * variant for one inst forms - monit and XMR
 * SJM 05/07/03 - now only can be called after all of dce filled 
 *
 * this routine must be passed declare (target) itree context on top of
 * inst stack and ref loc 1 under
 */
extern void __init_1instdce_prevval(struct dcevnt_t *dcep)
{
 byte *sbp, *sbp2;
 int32 dcewid;
 struct net_t *np;
 struct xstk_t *xsp;

 np = dcep->dce_np;
 /* DBG remove -- */
 if (dcep->dce_expr != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 if (dcep->prevval.bp == NULL) return;

 dcewid = __get_dcewid(dcep, np);
 /* SJM 05/08/03 - eval in match context but store as 0 since only 1 inst */
 __push_itstk(dcep->dce_matchitp);
 if (np->n_stren)
  {
   /* notice this can never be array */
   sbp = dcep->prevval.bp;
   /* must load for initialize from right inst. */
   get_stwire_addr_(sbp2, np);
   if (dcep->dci1 != -1) sbp2 = &(sbp2[dcep->dci2.i]);
   memcpy(sbp, sbp2, dcewid);
  }
 else
  {
   push_xstk_(xsp, dcewid);
   /* must load value from correct (match_itp instance) on it stack */
   __ld_wire_sect(xsp->ap, xsp->bp, np, dcep->dci1, dcep->dci2.i);

   /* need dummy place for itree inst num since know only 1 inst. */
   /* this is match loc but only 1 inst - so any would work */
   __push_wrkitstk(__inst_mod, 0);
   __st_perinst_val(dcep->prevval, dcewid, xsp->ap, xsp->bp);
   __pop_wrkitstk();
   __pop_xstk();
  }
 __pop_itstk();
}

/*
 * STATEMENT PREPARATION ROUTINES
 */

/*
 * for every always, add surrounding forever statement
 *
 * all variable storage must have been allocated by here
 * all id (and glbid) expression nodes made to point to variable
 */
extern void __prep_stmts(void)
{
 register int32 i; 
 register struct ialst_t *ialp; 
 register struct task_t *tskp;
 struct mod_t *mdp;
 int32 sav_declobj;

 /* bottom of if/case/delay control continuation stack must be null */
 /* for cases where continuation is really NULL - no goto */
 __prpsti = 0;
 __nbsti = -1;
 __prpstk[0] = NULL;
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   __prep_numsts = 0;
   __processing_func = FALSE;
   for (ialp = __inst_mod->ialst; ialp != NULL; ialp = ialp->ialnxt)
    {
     /* for always, must be first - tails of last must point to 1st stmt */
     /* here may be list and will be put on end */
     if (ialp->iatyp == ALWAYS)
      add_loopend_goto(ialp->iastp, ialp->iastp);
     ialp->iastp = __prep_lstofsts(ialp->iastp, TRUE, FALSE);
     /* DBG remove --- */
     if (__prpsti != 0) __misc_terr(__FILE__, __LINE__); 
     /* --- */
    }
   /* notice, there is one set of task variables per instance */
   /* but a task in one itree inst. can be enabled multiple times */
   /* tthrds for tasks (not functions) is per inst. list of active thrds */
   /* so if disable can disable all below */
   for (tskp = __inst_mod->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     /* never need task threads for function */ 
     if (tskp->tsktyp == FUNCTION) continue; 
     tskp->tthrds = (struct tskthrd_t **)
      __my_malloc(__inst_mod->flatinum*sizeof(struct tskthrd_t *));
     for (i = 0; i < __inst_mod->flatinum; i++) tskp->tthrds[i] = NULL;
    }
  
   sav_declobj = __cur_declobj;
   __cur_declobj = TASK;
   for (tskp = __inst_mod->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     /* named blocks handled as statement where they occur */
     if (tskp->tsktyp == FUNCTION) __processing_func = TRUE;
     else if (tskp->tsktyp == TASK) __processing_func = FALSE; 
     else continue;

     tskp->tskst = __prep_lstofsts(tskp->tskst, FALSE, FALSE);
     /* no branch continue here because must schedule/disable thread */
     /* need inform if function never enable */
     if (!tskp->t_used)
      {
       char s1[RECLEN], s2[RECLEN];

       if (__processing_func) strcpy(s2, "called");
       else strcpy(s2, "enabled"); 
       __gfinform(439, tskp->tsksyp->syfnam_ind, tskp->tsksyp->sylin_cnt, 
        "%s %s never %s", __to_tsktyp(s1, tskp->tsktyp), tskp->tsksyp->synam,
        s2);
      }
     /* DBG remove --- */ 
     if (__prpsti != 0) __misc_terr(__FILE__, __LINE__); 
     /* --- */
    }
   __processing_func = FALSE;
   __cur_declobj = sav_declobj;

   /* DBG remove --
   {
    extern void __dmp_mod(FILE *, struct mod_t *mdp);

    if (__debug_flg) __dmp_mod(stdout, mdp);
   }
   --- */   
   /* DBG remove ---
   if (__prep_numsts != __inst_mod->mstnum) __misc_terr(__FILE__, __LINE__);
   --- */

   __pop_wrkitstk();
  }
}

/*
 * routine to prepare (optimize) list of statements for simulation
 * returns front (may be new statement for for)
 *
 * know begin ends already turned into statements list wherever possible
 * know if containing statement is loop, goto added at end before here
 */
extern struct st_t *__prep_lstofsts(struct st_t *hdrstp, int32 nd_endgoto,
 int32 is_dctrl_chain)
{
 register struct st_t *stp;
 register int32 ii;
 int32 tei, fji;
 struct for_t *forp;
 struct st_t *astp, *astp2, *last_stp, *fjstp;
 struct delctrl_t *dctp;

 for (stp = hdrstp, last_stp = NULL; stp != NULL; stp = stp->stnxt)
  {
   __sfnam_ind = stp->stfnam_ind;
   __slin_cnt = stp->stlin_cnt;
   /* DBG remove ---
   if (__debug_flg)
    {
     __dbg_msg("%04d: AT %s %s - STMT PREP (%s)\n", stp->stalloc_ndx, 
      __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt),
      __inst_mod->msym->synam, __to_sttyp(__xs2, stp->stmttyp));
    }
   --- */
   /* --- ALTERNATE DBG remove ---
     __dbg_msg("AT %s %s - STMT PREP %04d (%s)\n",
      __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt),
      __inst_mod->msym->synam, __prep_numsts - 1,
      __to_sttyp(__xs2, stp->stmttyp));
    }
   --- */ 

   switch ((byte) stp->stmttyp) {
    case S_PROCA: case S_FORASSGN: case S_RHSDEPROCA: case S_NBPROCA: break;
    case S_IF:
     if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);
     stp->st.sif.thenst = __prep_lstofsts(stp->st.sif.thenst, TRUE, FALSE);
     if (stp->st.sif.elsest != NULL)
      stp->st.sif.elsest = __prep_lstofsts(stp->st.sif.elsest, TRUE, FALSE);
     if (stp->stnxt != NULL) pop_prpstmt();
     break;
    case S_CASE:
     prep_case(stp);
     break;
    case S_FOR:
     /* must link so old for assign is replaced by for inc. that points */
     /* to for body (added) - key is that for inc next is not used */ 
     /* first add goto to for statement itself at end of body */
     /* notice initial assign already precedes for */
     forp = stp->st.sfor;
     astp2 = add_loopend_goto(forp->forbody, forp->forbody);
     /* change to point to for itself */
     astp2->stnxt->st.sgoto = stp;
     astp2->stnxt->st.sgoto->lpend_goto_dest = TRUE;

     /* insert inc stmt before goto */
     forp->forinc->stnxt = astp2->stnxt;
     astp2->stnxt = forp->forinc;
     /* inc will be fixed up in body of loop */
     forp->forbody = __prep_lstofsts(forp->forbody, FALSE, FALSE);
     break;
    case S_FOREVER:
    case S_WHILE:
     /* scheme here if evaluation of while means if non NULL xpr see if T */
     /* if not do not exec statement else exec - loops to same statement */
     /* something like while (x) begin : y ... end becomes: */
     /* L: while () begin ... end; goto L; */
     add_loopend_goto(stp->st.swh.lpst, stp);
     stp->st.swh.lpst = __prep_lstofsts(stp->st.swh.lpst, FALSE, FALSE);
     break;
    case S_REPEAT:
     /* first allocate special repeat setup statement and link on front */
     astp = __alloc2_stmt(S_REPSETUP, stp->stfnam_ind, stp->stlin_cnt);
     /* fill guts of new statement with repeat guts - astp2 pnts to nxt st */
     *astp = *stp;       

     stp->stmttyp = S_REPSETUP;
     stp->rl_stmttyp = stp->stmttyp;
     stp->st_unbhead = FALSE;
     stp->st.scausx = NULL;

     /* then exchange stp and astp pointers (since prev nxt is stp) */
     astp2 = astp;
     astp = stp;
     stp = astp2;
     astp->stnxt = stp;

     /* stp now points to rep setup */
     /* DBG remove --
     if (__debug_flg)
      {
       __dbg_msg("AT %s %s - STMT PREP (%s)\n",
        __bld_lineloc(__xs, astp->stfnam_ind, astp->stlin_cnt),
        __inst_mod->msym->synam, __to_sttyp(__xs2, astp->stmttyp));
      }
     --- */
     /* DBG remove --
     __prep_numsts++;
     if (__debug_flg)
      {
       __dbg_msg("%04d: AT %s %s - STMT PREP (%s)\n", astp->stalloc_ndx,
        __bld_lineloc(__xs, astp->stfnam_ind, astp->stlin_cnt),
        __inst_mod->msym->synam, __to_sttyp(__xs2, astp->stmttyp));
      }
     --- */
     /* ALTERNATE DBG remove ---
     __prep_numsts++;
     if (__debug_flg)
      {
       __dbg_msg("AT %s %s - STMT PREP %04d (%s)\n",
        __bld_lineloc(__xs, astp->stfnam_ind, astp->stlin_cnt),
        __inst_mod->msym->synam, __prep_numsts - 1,
        __to_sttyp(__xs2, astp->stmttyp));
      }
     --- */
     /* allocate per inst. count storage */ 
     /* add loop back to repeat header */
     add_loopend_goto(stp->st.srpt.repst, stp);
     /* 32 bit word32 width built in here */
     stp->st.srpt.reptemp = (word32 *)
      __my_malloc(WRDBYTES*__inst_mod->flatinum);
     memset(stp->st.srpt.reptemp, 0, WRDBYTES*__inst_mod->flatinum);
     /* end must loop back to actual repeat not setup */
     stp->st.srpt.repst = __prep_lstofsts(stp->st.srpt.repst, FALSE, FALSE);
     break;
    case S_WAIT:
     /* build and adding dc events is just change expr. case here */
     dctp = stp->st.swait.wait_dctp;
     /* must turn on iact bit so linkon dce adds to free list when done */ 
     if (__iact_state) dctp->dc_iact = TRUE;
     /* wait is simple expression - EV OR illegal - edge illegal state dep. */
     bld_evxpr_dces(stp->st.swait.lpx, dctp, FALSE);
     /* wait @ event triggers on loop exp and executes wait to evaluate */
     /* the loop expression */
     dctp->actionst = stp;
     /* fill the delay/event scheduled action rec that is needed by wait */
     /* for arming and triggering */
     dctp->dctyp = DC_WAITEVENT;
     /* need to alloc and init scheduled tevs table */
     dctp->dceschd_tevs = (i_tev_ndx *)
      __my_malloc(__inst_mod->flatinum*sizeof(i_tev_ndx));
     for (tei = 0; tei < __inst_mod->flatinum; tei++)
      dctp->dceschd_tevs[tei] = -1;

     /* prepare the statements */ 
     /* wait needs end link to next statement after wait not beginning */
     if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);
     /* last statement (normally only 1, will have goto to continuation */
     stp->st.swait.lpst = __prep_lstofsts(stp->st.swait.lpst, TRUE, FALSE);
     if (stp->stnxt != NULL) pop_prpstmt();
     break;
    case S_DELCTRL:
     dctp = stp->st.sdc;
     /* 10/28/00 - if repeat form, insert repeat dc setup in front */
     if (dctp->repcntx != NULL)
      {
       /* first allocate special repeat setup statement and link on front */
       /* this is needed so can insert after to get lists nexts right */
       /* but still have setup first */
       astp = __alloc2_stmt(S_REPDCSETUP, stp->stfnam_ind, stp->stlin_cnt);
       /* fill guts of new statement with  delctrl statment guts */
       /* astp2 points to next stmt */
       *astp = *stp;       

       stp->stmttyp = S_REPDCSETUP;
       stp->rl_stmttyp = stp->stmttyp;
       stp->st_unbhead = FALSE;
       stp->st.scausx = NULL;

       /* then exchange stp and astp pointers (since prev nxt is stp) */
       /* so stp will be at original (after inserted setup) so next works */
       /* right in loop */
       astp2 = astp;
       astp = stp;
       stp = astp2;
       astp->stnxt = stp;
       /* DBG remove */
       if (stp->st.sdc != dctp) __misc_terr(__FILE__, __LINE__);
       /* --- */
       /* finally, alloc and initialize to 0 per inst repeat down counter */ 
       /* SJM 04/02/01 - inter ectl rep counter now word32 */
       dctp->dce_repcnts = (word32 *)
        __my_malloc(sizeof(word32)*__inst_mod->flatinum);
       memset(dctp->dce_repcnts, 0, sizeof(word32)*__inst_mod->flatinum);
      }
     /* 10/28/00 SJM - always still prepare dctrl as usual */
     prep_dctrl(stp);
     break;
    case S_NAMBLK:
     /* for named block, no continuation - must be subthread except in func */
     __push_nbstk(stp);
     if (__processing_func)
      { 
       if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);
       stp->st.snbtsk->tskst = __prep_lstofsts(stp->st.snbtsk->tskst,
        TRUE, FALSE);
       if (stp->stnxt != NULL) pop_prpstmt();
      }
     else
      {
       push_prpstmt((struct st_t *) NULL);
       stp->st.snbtsk->tskst = __prep_lstofsts(stp->st.snbtsk->tskst, FALSE,
        FALSE);
       pop_prpstmt();
      }
     __pop_nbstk();
     break;
    case S_UNBLK:
     if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);
     /* need continuation for simple block */
     stp->st.sbsts = __prep_lstofsts(stp->st.sbsts, TRUE, FALSE);
     if (stp->stnxt != NULL) pop_prpstmt();
     break;
    case S_UNFJ:
     push_prpstmt((struct st_t *) NULL);
     /* continuation inside these must be NULL, not stacked val */
     for (fji = 0;; fji++)
      {
       if ((fjstp = stp->st.fj.fjstps[fji]) == NULL) break;
       stp->st.fj.fjstps[fji] = __prep_lstofsts(fjstp, FALSE, FALSE);
      }
     pop_prpstmt();
     break;
    case S_QCONTA:
     /* first build the one for each lhs cat element list of per inst */
     /* qcaf dce lists and initialize - then during build just fill */
     bld_init_qcaf_dce_lstlst(stp);

     /* separate prepare because different qcval record for each inst */
     /* qcaf dces per inst because only one active on a reg at a time */
     for (ii = 0; ii < __inst_mod->flatinum; ii++)
      {
       __push_itstk(__inst_mod->moditps[ii]);

       if (stp->st.sqca->qcatyp == ASSIGN) prep_qc_assign(stp, FALSE);
       else
        {
         /* force of reg, is like assign except overrides assign */
         if (stp->st.sqca->regform) prep_qc_assign(stp, TRUE);
         else prep_qc_wireforce(stp);
        }
       __pop_itstk();
      }
     break;
    case S_QCONTDEA:
     for (ii = 0; ii < __inst_mod->flatinum; ii++)
      {
       __push_itstk(__inst_mod->moditps[ii]);

       if (stp->st.sqcdea.qcdatyp == DEASSIGN) prep_qc_deassign(stp);
       else
        {
         /* SJM 06/21/02 - since deassign/release just allocs qcvals */
         /* no diff between release/deassign */
         if (stp->st.sqcdea.regform) prep_qc_deassign(stp);
         else prep_qc_wirerelease(stp);
        } 
       __pop_itstk();
      } 
     break;
    case S_DSABLE:
     /* inside function disables are gotos to next statement in up block */
     if (__processing_func) prep_func_dsable(stp);

     /* since for any other name block or task cannot optimize since can */
     /* be disabled from interactive command */ 
     break;
    case S_TSKCALL:
     /* identify and build dces only for monit/fmonit here */
     prep_stskcalls(stp);
     break;
   }
   last_stp = stp;
  }
 /* now at end of list */
 /* fix up by working up stack to point where statement has next */
 if (nd_endgoto && __prpstk[__prpsti] != NULL) 
  {
   /* DBG remove ---*/
   if (last_stp == NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */
   /* need to save cur location so allocate get right stmt loc */ 
   astp = __alloc2_stmt(S_GOTO, last_stp->stfnam_ind, last_stp->stlin_cnt);
   astp->st.sgoto = __prpstk[__prpsti];

   if (is_dctrl_chain) astp->dctrl_goto = TRUE;
   else astp->lstend_goto = TRUE;

   /* in this rare case will not have line number - can it happen */
   /* LOOKATME - can this happen */ 
   if (last_stp == NULL) hdrstp = astp;
   else
    {
     last_stp->stnxt = astp;
    }
   /* DBG remove --
   __prep_numsts++;
   if (__debug_flg)
    {
     __dbg_msg("AT %s %s - STMT PREP %04d (s)\n",
      __bld_lineloc(__xs, astp->stfnam_ind, astp->stlin_cnt),
      __inst_mod->msym->synam, __prep_numsts - 1,
      __to_sttyp(__xs2, astp->stmttyp));
    }
   --- */
   /* ALTERNATE DBG remove --
   __prep_numsts++;
   if (__debug_flg)
    { 
     __dbg_msg("AT %s %s - STMT PREP %04d (s)\n",
      __bld_lineloc(__xs, astp->stfnam_ind, astp->stlin_cnt),
      __inst_mod->msym->synam, __prep_numsts - 1,
      __to_sttyp(__xs2, astp->stmttyp));
    }
   -- */
  }
 return(hdrstp);
}

/*
 * prepare sys task enable - only for monit/fmonit and builds dces for those
 *
 * SJM 06/21/02 - new algorithm builds monit/fmonit dces during prep
 */
static void prep_stskcalls(struct st_t *stp)
{
 struct expr_t *tkxp;
 struct tskcall_t *tkcp;
 struct sy_t *syp;
 struct systsk_t *stbp;

 tkcp = &(stp->st.stkc);
 tkxp = tkcp->tsksyx;

 /* nothing to do for non system task calls */
 if (tkxp->optyp == ID && *(tkxp->lu.sy->synam) == '$')
  {
   syp = tkxp->lu.sy;
   stbp = syp->el.esytbp;

   switch (stbp->stsknum) {
    /* system task args do not have type or width - take what is there */
    case STN_MONITOR: case STN_MONITORB: case STN_MONITORH: case STN_MONITORO:
     __prep_insrc_monit(stp, FALSE);
     break;
    /* tasks that take a multichannel descriptor followed by anything */
    case STN_FMONITOR: case STN_FMONITORB: case STN_FMONITORH:
    case STN_FMONITORO:
     __prep_insrc_monit(stp, TRUE);
     break;
    default: break;
   }
  }
}

/*
 * for list of statements that is loop body, add goto that links
 * back to front of loop
 * returns previous last statement - error to be called withh begstp nul
 */
static struct st_t *add_loopend_goto(struct st_t *begstp,
 struct st_t *targstp)
{
 register struct st_t *stp; 
 struct st_t *last_stp, *gtstp;

 /* DBG remove --- */
 if (begstp == NULL) __arg_terr(__FILE__, __LINE__);
 /* --- */

 /* find last statement in loop - know has at least one */
 for (stp = begstp, last_stp = NULL; stp != NULL; stp = stp->stnxt)
  last_stp = stp;
 if (last_stp == NULL) __arg_terr(__FILE__, __LINE__);
 gtstp = __alloc2_stmt(S_GOTO, last_stp->stfnam_ind, last_stp->stlin_cnt);
 gtstp->lpend_goto = TRUE;
 gtstp->st.sgoto = targstp;
 targstp->lpend_goto_dest = TRUE;

 gtstp->stfnam_ind = last_stp->stfnam_ind;
 gtstp->stlin_cnt = last_stp->stlin_cnt;
 last_stp->stnxt = gtstp;
 if (__debug_flg)
  {
   char s1[RECLEN], s2[RECLEN], s3[RECLEN];

   __dbg_msg("++ loop: adding goto after %s at %s back to stmt %s at %s\n",
    __to_sttyp(s1, last_stp->stmttyp), __bld_lineloc(__xs, 
    last_stp->stfnam_ind, last_stp->stlin_cnt), __to_sttyp(s2,
    begstp->stmttyp), __bld_lineloc(s3, targstp->stfnam_ind,
    targstp->stlin_cnt));
  }
 /* --- */
 return(last_stp);
}

/*
 * push a nested preparation statement
 * this is for control flow so many not pushed
 */
static void push_prpstmt(struct st_t *stp)
{
 if (++__prpsti >= MAXPRPSTNEST)
  __sgfterr(317, "statements nested too deeply (%d)", MAXPRPSTNEST);
 __prpstk[__prpsti] = stp;
 /* DBG remove --
 if (__debug_flg)
  {
   if (stp != NULL)
    {
     __dbg_msg(".. push nested stmt stack to %d at %s\n", __prpsti,
      __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt));
    }
   else
    {  
     __dbg_msg(".. push NULL nested stmt stack to %d\n", __prpsti);
    }
  }
 --- */
}

/*
 * pop a nested preparation statement
 */
static void pop_prpstmt(void)
{ 
 /* should never undeflow */
 if (__prpsti < 0) __misc_sgfterr(__FILE__, __LINE__);
 __prpsti--;
 /* DBG remove --
 if (__debug_flg)
  {
   struct st_t *stp;

    stp = __prpstk[__prpsti];
    if (stp != NULL)
     {
      __dbg_msg(".. pop nested stmt stack to %d at %s\n", __prpsti,
       __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt));
     }
    else
     {  
      __dbg_msg(".. pop NULL nested stmt stack to %d\n", __prpsti);
     }
  }
 --- */
}

/*
 * push a nested named block statement - this is for disable processing
 * and only used during preparation
 */
extern void __push_nbstk(struct st_t *stp)
{
 if (++__nbsti >= MAXPRPSTNEST)
  __sgfterr(318, "named blocks nested too deeply (%d)", MAXPRPSTNEST);
 __nbstk[__nbsti] = stp;
}

/*
 * pop a nested named block statement
 */
extern void __pop_nbstk(void)
{ 
 /* named blocks during prep. also should not undeflow */
 if (__nbsti < 0) __misc_sgfterr(__FILE__, __LINE__);
 __nbsti--;
}

/*
 * prepare case statement for simulation
 */
static void prep_case(struct st_t *stp)
{
 register struct csitem_t *csip;
 struct csitem_t *dflt_csip;

 if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);

 dflt_csip = stp->st.scs.csitems;
 /* this will move up stack to add goto after ending stp */
 for (csip = dflt_csip->csinxt; csip != NULL; csip = csip->csinxt)
  csip->csist = __prep_lstofsts(csip->csist, TRUE, FALSE);

 /* this will move up stack to connect ending stnxt to next exec. place */
 if (dflt_csip->csist != NULL)
  dflt_csip->csist = __prep_lstofsts(dflt_csip->csist, TRUE, FALSE);
 if (stp->stnxt != NULL) pop_prpstmt();
}

/*
 * prepare a declare control
 * tricky because #<> #<> #<> etc [stmt] legal
 */
static void prep_dctrl(struct st_t *stp)
{
 register struct st_t *stp2;
 struct delctrl_t *dctp;
 struct st_t *last_stp;

 dctp = stp->st.sdc;
 if (__iact_state) dctp->dc_iact = TRUE;
 cnv_cmpdctl_todu(stp, dctp);
 /* if no statement just prepare expr. - stnxt correct */
 if (dctp->actionst == NULL) return;

 /* for #[d1] #[d2] #[d3] ... <stmt> chain, add goto to end only */ 
 last_stp = NULL;
 for (stp2 = dctp->actionst;; stp2 = stp2->st.sdc->actionst)
  {
   /* keep going until delay control has no action statement or */
   /* a non delay control action statement */
   /* case "#10 begin #20 ..." - is not delay control chain */
   if (stp2 == NULL || stp2->stmttyp != S_DELCTRL || stp2->st_unbhead)
    break;
   dctp = stp2->st.sdc;
   cnv_cmpdctl_todu(stp2, dctp);

/* DBG remove --
   if (__debug_flg)
    {
     __dbg_msg("AT %s %s - STMT PREP %04d (%s)\n",
      __bld_lineloc(__xs, stp2->stfnam_ind, stp2->stlin_cnt),
      __inst_mod->msym->synam, __to_sttyp(__xs2, stp2->stmttyp));
    }
--- */

   last_stp = stp2;
  }
 if (stp2 == NULL)
  {
   __sgfwarn(562, "INTERNAL - delay control chain does not end with stmt.");
   /* unrecognized delay control chain */
   if (last_stp == NULL) __misc_terr(__FILE__, __LINE__);
   stp2 = last_stp;
  }
 /* finally, just add goto from stp2 to original statement next */
 if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);
 stp2 = __prep_lstofsts(stp2, TRUE, TRUE);
 if (stp->stnxt != NULL) pop_prpstmt();
}

/*
 * convert a delay control CMP LST form to run tim delay 
 */
static void cnv_cmpdctl_todu(struct st_t *stp, struct delctrl_t *dctp)
{
 register int32 tei;
 struct gate_t gwrk;
 struct sy_t tmpsym;

 /* notice for delay controls schd event tevs field not used */
 if (dctp->dctyp == DC_DELAY || dctp->dctyp == DC_RHSDELAY)
  {
   __add_dctldel_pnp(stp); 
   tmpsym.syfnam_ind = (word32) __sfnam_ind;
   tmpsym.sylin_cnt = __slin_cnt;
   __prep_delay(&gwrk, dctp->dc_du.pdels, FALSE, FALSE,
    "procedural delay control", FALSE, &tmpsym, FALSE);
   if (__nd_neg_del_warn)
    {
     __sgferr(974, "delay control negative delay illegal (0 used)");
     __nd_neg_del_warn = FALSE;
    } 
   dctp->dc_delrep = gwrk.g_delrep;
   dctp->dc_du = gwrk.g_du;
  }
 else
  {
   prep_event_dctrl(dctp);
   /* need to alloc and init scheduled tevs table */
   dctp->dceschd_tevs = (i_tev_ndx *)
    __my_malloc(__inst_mod->flatinum*sizeof(i_tev_ndx));
   for (tei = 0; tei < __inst_mod->flatinum; tei++)
    dctp->dceschd_tevs[tei] = -1L;
  }
}

/*
 * for every value in expr. add the dc list to appropriate wire 
 */
static void prep_event_dctrl(struct delctrl_t *dctp)
{
 register struct expr_t *xp;
 struct expr_t *evx;
 struct paramlst_t *pmp;

 /* must change delay representation to DT_1X */
 pmp = dctp->dc_du.pdels;
 evx = pmp->plxndp;
 /* first free param list form */
 __my_free((char *) pmp, sizeof(struct paramlst_t));
 dctp->dc_du.d1x = evx;
 dctp->dc_delrep = DT_1X;

 /* SJM 06/28/05 - for degenerate form no dces to build but must catch */  
 /* nil event expr */
 if (evx == NULL)
  {
   __sgfwarn(3139,
    "implicit event control - no events in statement - will never trigger");
   return;
  }

 if (evx->optyp != OPEVOR && evx->optyp != OPEVCOMMAOR)
  { bld_ev_dces(evx, dctp); return; }
 /* notice evor tree must associate left to right - i.e. evor chain */
 /* extends down left links */
 for (xp = evx;;) 
  {
   bld_ev_dces(xp->ru.x, dctp);
   if (xp->lu.x->optyp != OPEVOR && xp->lu.x->optyp != OPEVCOMMAOR)
    {
     /* left is bottom of tree */
     bld_ev_dces(xp->lu.x, dctp);
     break;
    }
   xp = xp->lu.x;
  }
}

/*
 * build the event control dcevnt list element(s) for one event control 
 *
 * know xp is not evor - main expr of operand of evor or wait simple expr
 * this cannot assume any itree loc. 
 *
 * for constant bit select, ID, or global do not need expr.
 * otherwise use normal variable in expr. change mechanism but before 
 * triggering armed evaluate expression and see if correct edge
 * aux dce_expr record contains previous entire expr. value
 */
static void bld_ev_dces(struct expr_t *xp, struct delctrl_t *dctp)
{
 int32 biti, bitj, e_val;
 word32 *wp;
 struct net_t *np;
 struct expr_t *endp, *ndx, *idndp;
 struct gref_t *grp;

 __cur_dce_expr = NULL;
 /* know evor must be at top and associates right (right tree) */
 e_val = NOEDGE; 
 if (xp->optyp == OPNEGEDGE || xp->optyp == OPPOSEDGE)
  {
   if (xp->optyp == OPNEGEDGE) e_val = E_NEGEDGE;
   else if (xp->optyp == OPPOSEDGE) e_val = E_POSEDGE;
   else __case_terr(__FILE__, __LINE__);

   endp = xp->lu.x;
   biti = bitj = -1;
   if (endp->optyp == LSB)
    {
     idndp = endp->lu.x;
     np = idndp->lu.sy->el.enp; 
     /* array never scalared and if not scalared need dce expr eval */
     /* this will force expr eval for array index (not bsel) */
     if (!np->vec_scalared) goto expr_edge;

     ndx = endp->ru.x;
     if (ndx->optyp == NUMBER)
      {
       /* this can be reg constant out of range of x */
       /* becomes entire range */
       wp = &(__contab[ndx->ru.xvi]);
       if (wp[1] == 0L) biti = bitj = (int32) wp[0];
      }
     else if (ndx->optyp == ISNUMBER)
      {
       __isform_bi_xvi = ndx->ru.xvi;
       biti = -2;
       bitj = 0;
      }
     else goto expr_edge;
    }
   else if (endp->optyp == ID || endp->optyp == GLBREF)
    {
     idndp = endp;
     np = idndp->lu.sy->el.enp;
     /* since using low bit only vectored wire do not need expr. */
     /* unindexed array illegal in rhs expr. */
    }
   else goto expr_edge;

   grp = (idndp->optyp == GLBREF) ? idndp->ru.grp : NULL; 
   linkon_dce(np, biti, bitj, dctp, e_val, grp);
   return;

expr_edge:
   __cur_dce_expr = (struct dce_expr_t *)
    __my_malloc(sizeof(struct dce_expr_t));
   __cur_dce_expr->edgxp = endp; 
   __cur_dce_expr->bp = NULL;
   __cur_dce_expr->mast_dcep = NULL; 
   /* this will set global dce expr for each allocated in bld routine */
   bld_evxpr_dces(xp, dctp, e_val);
   __cur_dce_expr = NULL;
   return;
  }

 /* LOOKATME - slight bug here - trigger on variable change */
 /* instead of expression change - rare (eval. to 1 bit logical mostly) */
 /* cases where there is a difference (sending question to P1364 committee */
 /* allocate for every variable including indices in expr. */

 bld_evxpr_dces(xp, dctp, FALSE);
}

/*
 * build and link on dce vents for 1 normal not evor expression but can be
 * many dces since 1 per variable for non edge expr. that is handled here
 * need itree place since called while running
 */
static void bld_evxpr_dces(struct expr_t *xp, struct delctrl_t *dctp,
 int32 eval)
{
 struct net_t *np;
 int32 biti, bitj;
 word32 *wp;
 struct expr_t *idndp, *ndx;
 struct gref_t *grp;

 switch ((byte) xp->optyp) {
  case GLBREF:
   idndp = xp; 
   np = xp->lu.sy->el.enp;
   linkon_dce(np, -1, -1, dctp, eval, idndp->ru.grp);
   break;
  case ID:
   idndp = xp; 
   np = xp->lu.sy->el.enp;
   linkon_dce(np, -1, -1, dctp, eval, (struct gref_t *) NULL);
   break;
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM: case OPEMPTY:
   break;
  case LSB:
   /* SJM - 07/02/00 - arrays also here if constant index makes range form */
   idndp = xp->lu.x; 
   np = idndp->lu.sy->el.enp;
   ndx = xp->ru.x;
   biti = bitj = -1;
   if (ndx->optyp == NUMBER)
    {
     /* know if out of range or x/z - will be all x value */
     wp = &(__contab[ndx->ru.xvi]);
     if (wp[1] == 0L) biti = bitj = (int32) wp[0];
    }
   else if (ndx->optyp == ISNUMBER)
    {
     __isform_bi_xvi = ndx->ru.xvi;
     biti = -2;
     bitj = 0;
    }
   else
    {
     /* notice for monitor and dctrl event change, variable here is legal */
     /* and implies change for index and trigger on all bits of variable */
     bld_evxpr_dces(ndx, dctp, eval);
    }
   /* SJM - 07/03/00 - for arrays need index, i.e. value but not array */
   /* index scalared - works since no way to refer to both bit and index */
   if (biti != -1 && !np->n_isarr && !np->vec_scalared) biti = bitj = -1;
   grp = (idndp->optyp == GLBREF) ? idndp->ru.grp : NULL; 
   linkon_dce(np, biti, bitj, dctp, eval, grp);
   break;
  case PARTSEL:
   idndp = xp->lu.x; 
   np = idndp->lu.sy->el.enp;
   ndx = xp->ru.x;
   /* know part select never IS, will not get here if x/z or out of rng */
   biti = __contab[ndx->lu.x->ru.xvi];
   bitj = __contab[ndx->ru.x->ru.xvi];
   if (!np->vec_scalared) biti = bitj = -1;
   grp = (idndp->optyp == GLBREF) ? idndp->ru.grp : NULL; 
   linkon_dce(np, biti, bitj, dctp, eval, grp);
   break;
  case FCALL:
   {
    register struct expr_t *fax;

    /* if any args of system or user functions chg, monitor triggers */
    /* notice $time function do not have arguments */
    for (fax = xp->ru.x; fax != NULL; fax = fax->ru.x)
     bld_evxpr_dces(fax->lu.x, dctp, eval);
   }
   break;
  case LCB:
   {
    register struct expr_t *fax;

    for (fax = xp->ru.x; fax != NULL; fax = fax->ru.x)
     bld_evxpr_dces(fax->lu.x, dctp, eval);
   }
   break;
  default:
   if (xp->lu.x != NULL) bld_evxpr_dces(xp->lu.x, dctp, eval);
   if (xp->ru.x != NULL) bld_evxpr_dces(xp->ru.x, dctp, eval);
   break;
 } 
}

/*
 * for dces: to move from expr. reference to target where var. stored 
 *  -- if xmr, xmr ref. to target by calling xmrpush refgrp to targ(grp)
 *     [notice np is right but itree loc. wrong]
 * to move from target variable location back to expr. ref.
 *  -- if xmr, xmr target to ref cal xmrpush targ to ref(xmrtyp, npu1)
 */

/*
 * link the delay event control element for wire np
 * this builds the dcelst on np that is never removed  
 *
 * when called __inst_mod is module ref. in, (not declare in target)
 * and np is wire possibly xmr target but not itree context only mod here
 * if ref. expr is xmr, grp non nil 
 */
static void linkon_dce(struct net_t *np, int32 biti, int32 bitj,
 struct delctrl_t *dctp, int32 e_val, struct gref_t *grp)
{
 struct dcevnt_t *dcep;

 /* case 1: all xmr cases including xmr target */
 /* notice because of preprocessing never need 2 steps from target to ref */
 if (grp != NULL)
  {
   xmr_linkon_dce(np, biti, bitj, dctp, e_val, grp);
   return;
  }
 /* case 2: simple in module */
 dcep = linkon2_dce(np, biti, bitj, dctp, e_val, FALSE, __inst_mod,
  __inst_mod);
 dcep->dce_xmrtyp = XNP_LOC;
 if (dctp->dc_iact) init_iact_dce(dcep, dctp, NULL);
}

/*
 * initialize interactive only dces 
 * 
 * expects inst mod to be set to module where net declared in
 * SJM 01/14/03 - LOOKATME - think there is reason need to pass grp
 *
 * SJM 05/04/05 - because putting var storage (np.nva) in .bss section
 * for cver-cc, this is only for interactive init using interpreter
 * after linking in .bss .so lib var values, now initializing by net in mod
 */
static void init_iact_dce(struct dcevnt_t *dcep, struct delctrl_t *dctp,
 struct gref_t *grp)
{
 struct net_t *np;
 struct dceauxlst_t *dclp;
 struct mod_t *ref_mdp;

 np = dcep->dce_np;

 /* AIV 01/04/07 - init dce expr was using the wrong inst for */
 /* dce with more than one inst and was also skipping init for the */
 /* dce_expr for the one instance case */
 if (dcep->dce_expr != NULL) init_dce_exprval(dcep);
 else
  {
   if (dcep->prevval.wp != NULL)
    {
     ref_mdp = dcep_ref_mod(dcep);
     init_dce_prevval(dcep, ref_mdp);
    }
   }

 if (dctp->dc_iact)
  {
   /* add to iact list for this statement - will be linked to hctrl */  
   dclp = (struct dceauxlst_t *) __my_malloc(sizeof(struct dceauxlst_t));
   dclp->ldcep = dcep; 
   dclp->dclnxt = __iact_dcehdr;
   __iact_dcehdr = dclp;
   /* SJM 05/03/03 - LOOKATME - think iact XMR event controls converted */
   /* to per inst.  is that true? */
   dcep->iact_itp = __inst_ptr;

   /* since no dce, no loads, and no dmpvars must always turn chg store on */
   if (!np->nchg_nd_chgstore)
    {
     /* this also turn regen of net's decl iops from dce if -O on */
     __dce_turn_chg_store_on(__inst_mod, dcep, TRUE);  
    }
   /* SJM 04/14/04 - even if need chg store if dumpvars in future but that */
   /* can happen if need only has dce added from iact code and dumpvars */
   np->nchg_has_dces = TRUE;
  }
}

/*
 * routine to handle all cases where dce expr. is xmr (global ref. expr)
 * ref. in module __inst_mod - these all always one instance forms
 * notice all rooted dce xmrs are 1inst forms also npps
 */
static void xmr_linkon_dce(struct net_t *np, int32 biti, int32 bitj,
 struct delctrl_t *dctp, int32 e_val, struct gref_t *grp)
{
 register int32 ii;
 struct dcevnt_t *dcep;
 struct itree_t *itp;
 struct mod_t *ref_mdp;

 /* handle xmr */
 if (!grp->is_rooted)
  {
   /* SJM 05/04/03 - for non rooted where need prev. val, must set */
   /* module instance context for any instance such as inst 0 */
   /* (many instance and do not need to know here) */
   dcep = linkon2_dce(np, biti, bitj, dctp, e_val, FALSE, grp->gin_mdp,
    grp->targmdp);

   if (grp->upwards_rel)
    { dcep->dceu.dcegrp = grp; dcep->dce_xmrtyp = XNP_UPXMR; }
   /* downward case */
   else { dcep->dceu.dcegrp = grp; dcep->dce_xmrtyp = XNP_DOWNXMR; }
   /* SJM 05/08/03 - now initialize only after complete dce built */
   if (dctp->dc_iact) init_iact_dce(dcep, dctp, grp);
   return;
  }

 /* rooted xmr handled here */
 for (ii = 0; ii < __inst_mod->flatinum; ii++)
  {
   /* itp is itree loc. of xmr reference */
   itp = __inst_mod->moditps[ii]; 
   /* reference itree loc. */
   __push_itstk(itp);
   ref_mdp = __inst_ptr->itip->imsym->el.emdp;
   /* xmr target (declared in) */
   __xmrpush_refgrp_to_targ(grp);
   /* SJM 05/06/03 - BEWARE - TOS must be def (targ) and TOS-1 must be ref */ 
   dcep = linkon2_dce(np, biti, bitj, dctp, e_val, TRUE, ref_mdp, __inst_mod);
   /* if one inst form needs xmr target (where wire decled) itree loc */
   dcep->dce_1inst = TRUE; 
   /* match is target itree loc */
   dcep->dce_matchitp = __inst_ptr;
   __pop_itstk();
   dcep->dce_refitp = __inst_ptr;
   /* SJM 05/07/03 - must set as rooted dce XMR */ 
   __pop_itstk();
   dcep->dce_xmrtyp = XNP_RTXMR;

   /* SJM 05/08/03 - now initialize only after complete dce built */
   if (dctp->dc_iact) init_iact_dce(dcep, dctp, grp);
  }
}

/*
 * actually link on event dce - other routines for monitor and dumpvars 
 * created dce is returned
 *
 * assumes inst mod set to declared in module context 
 *
 * if oninst, know the itree context of the one inst targed (declared in) set
 * this allocates any old value storage but does not initialize it 
 *
 * SJM 05/07/03 - now since caller for XMR sets some dce fields must only 
 * set fields here, can't call routines that use dce fields
 * this was cause of most of the XMR event control dce bugs
 */
static struct dcevnt_t *linkon2_dce(struct net_t *np, int32 biti, int32 bitj,
 struct delctrl_t *dctp, int32 e_val, int32 oneinst, struct mod_t *ref_mdp,
 struct mod_t *decl_mdp)
{
 struct dcevnt_t *dcep;
 struct dceauxlst_t *dclp;

 /* allocate, init, and fill the fields */
 dcep = __alloc_dcevnt(np);

 /* if unused for non complicated edge expression will be nil */ 
 if (__cur_dce_expr != NULL)
  {
   dcep->dce_expr = __cur_dce_expr;
   /* DBG remove -- */
   if (dcep->dce_1inst) __misc_terr(__FILE__, __LINE__);
   /* --- */
   
   /* notice for dce expr, old value only for each ref inst since */
   /* must eval expr in ref inst */
   if (__cur_dce_expr->mast_dcep == NULL)
    {
     __cur_dce_expr->mast_dcep = dcep; 
     dcep->dce_expr->bp = (byte *) __my_malloc(ref_mdp->flatinum);
    }
  }

 if (biti == -1) dcep->dce_typ = DCE_INST;
 else
  {
   dcep->dce_typ = DCE_RNG_INST;
   dcep->dci1 = biti;
   if (biti == -2)
    {
     /* for one inst. form must access -2 form to actual index */
     /* because know itree context pushed for one inst - from now on not IS */
     if (oneinst)
      {
       /* SJM 10/12/04 - because contab is realloc must be index */
       dcep->dci1 = dcep->dci2.i = __contab[__isform_bi_xvi + 2*__inum];
      }
     /* since correct for direction here will be right bits */
     /* has normal user error of connecting opposite direction bus problem */
     else dcep->dci2.xvi = __isform_bi_xvi;
    }  
   else dcep->dci2.i = bitj;
  }
 /* 07/01/00 - just added a dce - also need change store */
 /* 07/24/00 - has dces only on if reg for immediate propagate/wakeup */
 /* but recording bits set after here */
 if (!dctp->dc_iact)
  {
   if (np->ntyp >= NONWIRE_ST) np->nchg_has_dces = TRUE;
   np->nchg_nd_chgstore = TRUE;
  }  

 /* link onto front of d ctrl list for np */
 dcep->dcenxt = np->dcelst;
 np->dcelst = dcep;
 dcep->st_dctrl = dctp;
 if (e_val != NOEDGE)
  {
   dcep->dce_edge = TRUE;
   dcep->dce_edgval = (word32) e_val;
  }
 /* if has edge needs per inst. old value table for last value */
 /* if oneinst know right itree loc. set */
 /* SJM 05/09/03 - if have 1 bit edge expr, previous value not needed */
 if (dcep->dce_expr == NULL)
  {
   if (oneinst)
    {
     /* notice this does need inst context */
     __alloc_1instdce_prevval(dcep);
    }
   else alloc_dce_prevval(dcep, decl_mdp);
  }

 /* DBG remove ---
 if (__debug_flg)
  {
   if (dctp->dc_iact) strcpy(__xs, " (interactive)");
   else strcpy(__xs, ""); 
   if (oneinst) strcat(__xs, " (xmr)");
   __dbg_msg(
    ".. ref. mod %s decl. %s adding net %s delay ctrl type %d%s ([%d:%d])\n",
    ref_mdp->msym->synam, decl_mod->msym->synam, np->nsym->synam,
    dcep->dce_typ, __xs, dcep->dci1, dcep->dci2.i);
  }
 --- */
 if (dctp->dc_iact)
  {
   /* add to iact list for this statement - will be linked to hctrl */  
   dclp = (struct dceauxlst_t *) __my_malloc(sizeof(struct dceauxlst_t));
   dclp->ldcep = dcep; 
   dclp->dclnxt = __iact_dcehdr;
   __iact_dcehdr = dclp;
   /* for after sim start iact dctrl add, know the iact itree loc set */  
   dcep->iact_itp = __inst_ptr;
  }
 return(dcep);
}

/*
 * routine to turn chg store on when new dce added but previously chg store
 * off because no dces, and no loads, and not dumpvars 
 *
 * SJM 02/08/03 - for -O this only regens any needed proc insns but
 * caller must regen the net with the new dces added - proc regen
 * is only for case where net was not compiled in proc code
 * case where
 */
extern void __dce_turn_chg_store_on(struct mod_t *in_mdp,
 struct dcevnt_t *dcep, int32 all_insts)  
{
 register int32 ii;
 struct mod_t *mdp;
 struct net_t *np;

 if (dcep->dce_1inst)
  {
   mdp = dcep->dce_matchitp->itip->imsym->el.emdp;
   /* DBG remove -- */
   if (in_mdp != mdp) __misc_terr(__FILE__, __LINE__); 
   /* --- */
  }
 else if (dcep->dce_xmrtyp != XNP_LOC) mdp = dcep->dceu.dcegrp->targmdp;
 else mdp = in_mdp;
 np = dcep->dce_np;

 /* SJM 01/06/03 - fix interpreter bug since need chg store on if */
 /* net had no dces, no nlds, and dumpvars was off */
 np->nchg_nd_chgstore = TRUE;

 /* SJM 04/14/04 - for iact added dce if dumpvars and unc. need this on */
 np->nchg_has_dces = TRUE;

 if (all_insts)
  {
   /* SJM 01/06/03 - LOOKATME - is is possible to only turn on this inst? */
   for (ii = 0; ii < __inst_mod->flatinum; ii++)
    { np->nchgaction[ii] &= ~(NCHG_ALL_CHGED); }
  }
 else np->nchgaction[__inst_ptr->itinum] &= ~(NCHG_ALL_CHGED);
}

/*
 * allocate a delay control event record
 *
 * this assumes non XMR and non 1inst - if not caller must set
 * SJM 05/07/03 - must set net at beginning since needed by following code
 */
extern struct dcevnt_t *__alloc_dcevnt(struct net_t *np)
{ 
 struct dcevnt_t *dcep;

 dcep = (struct dcevnt_t *) __my_malloc(sizeof(struct dcevnt_t));
 dcep->dce_np = NULL;

 dcep->dce_typ = DCE_NONE;
 dcep->dce_np = np;

 dcep->dce_xmrtyp = XNP_LOC; 
 dcep->dce_1inst = FALSE;
 dcep->dce_tfunc = FALSE;
 /* SJM 07/22/00 - for re-entrant problem and vpi control off - never off */
 /* unless set by vpi (sim) control or call back entry */
 /* SJM 06/13/02 - now also interpreter always filters dce for on/off */
 /* because (f)monitor and qcaf now added during design load (prep) */
 /* for these builder must explicitlyturn off */
 dcep->dce_off = FALSE;
 dcep->is_fmon = FALSE;
 dcep->dce_nomonstren = TRUE; 
 dcep->dci1 = -1;
 dcep->dci2.i = -1;
 dcep->prevval.wp = NULL;
 dcep->dce_edge = FALSE;
 dcep->dce_edgval = NOEDGE;
 dcep->st_dctrl = NULL;
 dcep->dceu.dcegrp = NULL;
 dcep->dceu2.dce_fmon = NULL;
 dcep->dce_matchitp = NULL;
 dcep->dce_refitp = NULL; 
 dcep->iact_itp = NULL;
 dcep->dcenxt = NULL;
 dcep->dce_expr = NULL;
 return(dcep);
}

/*
 * prepare a disable inside a function by setting to next statement to goto
 * inside function disable are like c continue and are just gotos
 */
static void prep_func_dsable(struct st_t *stp)
{
 register int32 i;
 struct expr_t *dsxp;
 struct sy_t *syp;
 struct task_t *dsatskp;

 dsxp = stp->st.sdsable.dsablx;
 syp = dsxp->lu.sy; 
 /* disable of func. indicated by nil next statmenet - use fcall stack */
 if (syp->sytyp == SYM_F)
  {
   stp->st.sdsable.func_nxtstp = NULL;
   return;
  }

 /* must be disabling named block */
 if (syp->sytyp != SYM_LB || syp->el.etskp->tsktyp == FORK
  || dsxp->optyp == GLBREF) __misc_sgfterr(__FILE__, __LINE__);
 dsatskp = syp->el.etskp;
 /* know every named block when entered in function name block is stacked */
 for (i = __nbsti; i >= 0; i--)
  {
   if (__nbstk[i]->st.snbtsk == dsatskp)
    {
     /* this can be nil */
     stp->st.sdsable.func_nxtstp = __nbstk[i]->stnxt;
     return;
    }
  }
 /* know always enclosing, or will not get here - earlier error */
 __case_terr(__FILE__, __LINE__);
}

/*
 * return T if disable targsyp above cursytp 
 * i.e. is upward break type disable
 * if any named block on path sets nbonpath to T
 *
 * because of xmr disabling of named begin-end blocks need thread so cannot
 * use goto except inside function
 */
extern int32 __is_upward_dsable_syp(struct sy_t *targsyp,
 struct symtab_t *cursytp, int32 *nbonpath)
{
 register struct symtab_t *sytp;
 struct sy_t *syp;

 *nbonpath = FALSE;
 if (targsyp->sytyp != SYM_LB) return(FALSE); 
 /* notice top of upward chain will be module, but disable of module */
 /* illegal so will return match top of chain module */
 for (sytp = cursytp; sytp != NULL; sytp = sytp->sytpar) 
  {
   syp = sytp->sypofsyt;
   if (targsyp == syp) return(TRUE);
   if (syp->sytyp == SYM_LB) *nbonpath = TRUE;
  }
 return(FALSE);
}

/*
 * FORCE/ASSIGN/MONITOR PREPARATION ADD DCE ROUTINES
 */

/*
 * for qcaf stmts, build the per lhs cat element 
 * for reg per cat component dce lists are per inst and for wire
 * they are per bit of cat expr per inst
 */
static void bld_init_qcaf_dce_lstlst(struct st_t *stp)
{
 register int32 ii;
 int32 ibase;
 struct expr_t *lhsx;
 struct expr_t *catndp, *catlhsx;
 struct dceauxlstlst_t *dcllp, *end_dcllp;
 
 lhsx = stp->st.sqca->qclhsx;
 if (lhsx->optyp != LCB)
  {
   /* one list of peri lists */
   dcllp = (struct dceauxlstlst_t *)
    __my_malloc(sizeof(struct dceauxlstlst_t)); 
   stp->st.sqca->rhs_qcdlstlst = dcllp;
   if (stp->st.sqca->regform)
    {
     /* for reg list field is peri table of dce lists - starting at empty */
     dcllp->dcelsttab = (struct dceauxlst_t **)
      __my_malloc(__inst_mod->flatinum*sizeof(struct dceauxlst_t *));
     
     for (ii = 0; ii < __inst_mod->flatinum; ii++) 
      dcllp->dcelsttab[ii] = NULL; 
    }
   else
    {
     /* for wire list field is peri/bit tab of dce lists - starts at empty */
     ibase = __inst_mod->flatinum*lhsx->szu.xclen;
     dcllp->dcelsttab = (struct dceauxlst_t **)
      __my_malloc(ibase*sizeof(struct dceauxlst_t *));
     for (ii = 0; ii < ibase; ii++) dcllp->dcelsttab[ii] = NULL; 
    }
   /* since non concat, only one element */
   dcllp->dcelstlstnxt = NULL;
  } 
 else
  {
   end_dcllp = NULL;
   for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x)
    {
     catlhsx = catndp->lu.x;

     /* allocate list of peri lists element */
     dcllp = (struct dceauxlstlst_t *)
      __my_malloc(sizeof(struct dceauxlstlst_t)); 
     if (end_dcllp == NULL) stp->st.sqca->rhs_qcdlstlst = dcllp;
     else end_dcllp->dcelstlstnxt = dcllp;
     end_dcllp = dcllp;

     if (stp->st.sqca->regform)
      {
       /* reg list field is peri table of dce lists - starting at empty */
       dcllp->dcelsttab = (struct dceauxlst_t **)
        __my_malloc(__inst_mod->flatinum*sizeof(struct dceauxlst_t *));
       for (ii = 0; ii < __inst_mod->flatinum; ii++) 
        dcllp->dcelsttab[ii] = NULL; 
      }
     else
      {
       ibase = __inst_mod->flatinum*catlhsx->szu.xclen;
       /* if not list end, next pass will set */
       dcllp->dcelsttab = (struct dceauxlst_t **)
        __my_malloc(ibase*sizeof(struct dceauxlst_t *));
       for (ii = 0; ii < ibase; ii++) dcllp->dcelsttab[ii] = NULL; 
      }
     dcllp->dcelstlstnxt = NULL;
    }
  }
}

/*
 * prepare a quasi-continuous assign or force of reg (same as qc assign)
 * this is for both reg force and reg assign
 *
 * SJM 06/14/02 - same as old exec qc assign for each inst but at prep time
 * to build and fill d.s
 */
static void prep_qc_assign(struct st_t *stp, int32 is_force)
{
 register struct expr_t *catndp;
 register struct dceauxlstlst_t *dcllp;
 struct expr_t *lhsx, *catlhsx;
 struct dceauxlst_t *qcdep;

 /* first evaluate rhs */
 lhsx = stp->st.sqca->qclhsx;
 /* only possibilities are concat and ID */
 /* this builds and initializes the reg assign/frc qcval records */ 
 if (lhsx->optyp != LCB)
  {
   if (is_force) qcdep = prep_noncat_qc_regforce(stp, lhsx);
   else qcdep = prep_noncat_qc_assign(stp, lhsx);
   /* only one list of lists element since not lhs concat */
   stp->st.sqca->rhs_qcdlstlst->dcelsttab[__inum] = qcdep;
  }
 else
  {
   /* concatenate case know lhs entire var */
   dcllp = stp->st.sqca->rhs_qcdlstlst;
   for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x,
    dcllp = dcllp->dcelstlstnxt)
    {
     catlhsx = catndp->lu.x;
     if (is_force) qcdep = prep_noncat_qc_regforce(stp, catlhsx);
     else qcdep = prep_noncat_qc_assign(stp, catlhsx);
     dcllp->dcelsttab[__inum] = qcdep;
    }
  }
}

/*
 * prepare a quasi-continuous deassign (same as old exec but at per/inst now)
 * inverse of assign and reg only defined for regs
 */
static void prep_qc_deassign(struct st_t *stp)
{
 register struct expr_t *catndp;
 int32 nd_itpop;
 struct expr_t *lhsx, *catlhsx;
 struct net_t *np;
 struct gref_t *grp;

 /* SJM 07/19/02 - was wrongly accessing qconta not qcontdea record */
 lhsx = stp->st.sqcdea.qcdalhs;
 /* only possibilities are concat and ID */
 if (lhsx->optyp != LCB)
  {
   /* just need to alloc qcval records here - dce list built from assgn */
   np = lhsx->lu.sy->el.enp;
   if (np->nu2.qcval == NULL)
    {
     /* SJM 05/23/03 - must alloc in context of XMR */
     if (lhsx->optyp == GLBREF)
      { grp = lhsx->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
     else nd_itpop = FALSE;

     __alloc_qcval(np);

     if (nd_itpop) __pop_itstk();
    }
  }
 else
  {
   /* concatenate case know lhs full wire - tricky extractions of rhs */
   for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x)
    {
     catlhsx = catndp->lu.x;
     np = catlhsx->lu.sy->el.enp;
     if (np->nu2.qcval == NULL)
      {
       /* SJM 05/23/03 - must alloc in context of XMR */
       if (lhsx->optyp == GLBREF)
        {
         grp = lhsx->ru.grp;
         __xmrpush_refgrp_to_targ(grp);
         nd_itpop = TRUE;
        }
       else nd_itpop = FALSE;

       __alloc_qcval(np);

       if (nd_itpop) __pop_itstk();
      }
    }
  }
}

/*
 * prep quasi continuous assign for one expr in one inst.
 *
 * know lhs always entire register - no assign for wires - lhs can be xmr 
 * this is called for every inst of module tha contains stmt
 * LOOKATME - do not need stmt since can get from qcval already built
 */
static struct dceauxlst_t *prep_noncat_qc_assign(struct st_t *qcstp,
 struct expr_t *lhsx)
{
 int32 nd_itpop;
 struct net_t *np;
 struct qcval_t *assgn_qcp;
 struct gref_t *grp;

 /* SJM 05/29/03 - must allocate and find qcval rec from lhs itree loc */
 nd_itpop = FALSE;
 if (lhsx->optyp == GLBREF)
  {
   grp = lhsx->ru.grp;
   __xmrpush_refgrp_to_targ(grp);
   nd_itpop = TRUE;
  }
 else if (lhsx->optyp != ID) __case_terr(__FILE__, __LINE__);

 np = lhsx->lu.sy->el.enp;
 if (np->nu2.qcval == NULL) __alloc_qcval(np);
 assgn_qcp = &(np->nu2.qcval[2*__inum + 1]);

 /* add qcaf from soruce location of statment */
 if (nd_itpop) __pop_itstk();

 /* SJM 05/29/03 - must build the qcaf dce in reference not declare context */ 
 __qcaf_dcehdr = NULL;
 /* for constant rhs this can be nil */
 bld_qcaf_dces(qcstp->st.sqca->qcrhsx, assgn_qcp);

 return(__qcaf_dcehdr);
}

/*
 * prepare the quasi continuous force for reg variables - just alloc qcval rec
 *
 * know lhs always entire register
 * lhs here can be xmr 
 * force of entire reg only overrides possible active reg assign
 */
static struct dceauxlst_t *prep_noncat_qc_regforce(struct st_t *qcastp,
 struct expr_t *lhsx)
{
 int32 nd_itpop;
 struct net_t *np;
 struct qcval_t *frc_qcp; 
 struct gref_t *grp;

 nd_itpop = FALSE;
 /* 05/28/03 - must get fource qc record ptr from lhs decl itree loc */
 if (lhsx->optyp == GLBREF)
  {
   grp = lhsx->ru.grp;
   __xmrpush_refgrp_to_targ(grp);
   nd_itpop = TRUE;
  }
 else if (lhsx->optyp != ID) __case_terr(__FILE__, __LINE__);

 np = lhsx->lu.sy->el.enp;
 /* DBG remove -- */
 if (!np->frc_assgn_allocated) __misc_terr(__FILE__, __LINE__);
 /* --- */
 if (np->nu2.qcval == NULL) __alloc_qcval(np);
 frc_qcp = &(np->nu2.qcval[2*__inum]);

 /* 05/28/03 - but must bld qcaf dces in ref itree context */
 if (nd_itpop) __pop_itstk();

 __qcaf_dcehdr = NULL;
 /* for constant rhs thsi can be nil */
 bld_qcaf_dces(qcastp->st.sqca->qcrhsx, frc_qcp);

 return(__qcaf_dcehdr);
}

/*
 * allocate a new qc assign value aux. record
 * itree location must be set before calling here since needs to know mod in
 *
 * SJM 12/21/02 - this is per inst of mod net declared in when called as XMR
 */
extern void __alloc_qcval(struct net_t *np)
{
 register int32 i;
 register struct qcval_t *qcvalp;

 /* AIV 03/09/05 - if force from vpi bit needs to be set */
 np->frc_assgn_allocated = TRUE;
 if (np->ntyp >= NONWIRE_ST)
  {
   /* here need 1 qcval per inst. but need 1 for assign and 1 for force */
   np->nu2.qcval = (struct qcval_t *)
    __my_malloc(2*__inst_mod->flatinum*sizeof(struct qcval_t));
  
   qcvalp = np->nu2.qcval;
   for (i = 0; i < 2*__inst_mod->flatinum; i++, qcvalp++) init_qcval(qcvalp);
  }
 else
  {
   /* here need 1 per inst bit product */
   /* LOOKATME - could have 1 bit per inst. for vectored wires */ 
   np->nu2.qcval = (struct qcval_t *)
    __my_malloc(__inst_mod->flatinum*np->nwid*sizeof(struct qcval_t));
   qcvalp = np->nu2.qcval;
   for (i = 0; i < __inst_mod->flatinum*np->nwid; i++, qcvalp++)
    init_qcval(qcvalp);
  }
}

/*
 * allocate a qcval record
 */
static void init_qcval(struct qcval_t *qcvalp) 
{
 qcvalp->qc_active = FALSE;
 qcvalp->qc_overridden = FALSE;
 qcvalp->qcstp = NULL;
 qcvalp->qcrhsbi = -1;
 qcvalp->qclhsbi = -1;
 qcvalp->lhsitp = NULL;
 qcvalp->qcdcep = NULL;
}

/*
 * QUASI CONTINUOUS WIRE FORCE/RELEASE PREP ROUTINES 
 */

/*
 * prepare quasi-continuous force on a wire
 * possibilities here are wire, constant bit select, part select
 * also concat of above
 * wire must be scalared and everything decomposed to bits
 */
static void prep_qc_wireforce(struct st_t *stp)
{
 register struct expr_t *catndp;
 register struct dceauxlstlst_t *dcllp;
 struct expr_t *lhsx, *catlhsx;

 /* first evaluate rhs */
 lhsx = stp->st.sqca->qclhsx;
 /* only possibilities are concat and ID */
 /* this builds and initializes the reg assign/frc qcval records */ 
 if (lhsx->optyp != LCB)
  {
   prep_noncat_qc_wireforce(stp, lhsx, stp->st.sqca->rhs_qcdlstlst);
  }
 else
  {
   /* concatenate case know lhs full wire - tricky extractions of rhs */
   dcllp = stp->st.sqca->rhs_qcdlstlst;
   for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x,
    dcllp = dcllp->dcelstlstnxt)
    {
     catlhsx = catndp->lu.x;
     prep_noncat_qc_wireforce(stp, catlhsx, dcllp);
    }
  }
}

/*
 * prepare a quasi-continuous release - only decomposes into lhs exprs
 * only scalared wires or selects or cats not regs
 * wire force/release is one level only
 * called in itree context of release stmt 
 * 
 * 06/24/02 - wire release just alloc qcvals for all nets so it must
 * decompose into cat elements to get net - for wire all bits need
 * qcval even if only some can be forced/released 
 */
static void prep_qc_wirerelease(struct st_t *stp)
{
 register struct expr_t *catndp;
 struct expr_t *lhsx, *catlhsx;

 lhsx = stp->st.sqcdea.qcdalhs; 
 /* only possibilities are concat and ID */
 if (lhsx->optyp != LCB) prep_noncat_qc_wirerelease(lhsx);
 else
  {
   /* concatenate case know lhs full wire - tricky extractions of rhs */
   for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x)
    { catlhsx = catndp->lu.x; prep_noncat_qc_wirerelease(catlhsx); }
  }
}
/*
 * after possible concat unwinding, prepare wire force
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
 *
 * sjm 12/21/02 - attached to statement so dcllp element must use stmt
 * itree inst loc even for XMR lhs expr
 */
static void prep_noncat_qc_wireforce(struct st_t *qcfstp, struct expr_t *lhsx,
 struct dceauxlstlst_t *dcllp)
{
 register int32 bi, xbi, ibase;
 int32 biti, bitj;
 struct qcval_t *frc_qcp;
 struct net_t *np;
 struct itree_t *itp;

 /* step 1: get the wire range */
 /* for psel or vector, range is biti down to bitj - for scalar 0,0 */
 /* this computes any xmr new itp but does not push it - nil if not XMR */
 __get_qc_wirrng(lhsx, &np, &biti, &bitj, &itp); 

 /* for tran channel wire, no dces - forcing wire forces channel */
 /* LOOKATME - think no need also no need for qcvals */

 /* SJM 07/19/02 - for lhs xmr need target itree loc */
 /* rest needs possible lhs xmr itree context including qcval alloc */
 if (itp != NULL) __push_itstk(itp);

 /* allocate the qcval record for wire if not yet alloced */
 /* also needed for tran channel force */
 /* for XMR attached net per inst values are from pushed defined in mod */
 if (np->nu2.qcval == NULL) __alloc_qcval(np);

 /* 05/28/03 - need to get per bit frc qc record from lhs decl itree cntxt */ 
 ibase = __inum*np->nwid;
 if (itp != NULL) __pop_itstk();

 if (np->ntraux != NULL) return;

 for (bi = bitj, xbi = 0; bi <= biti; bi++, xbi++)
  {
   /* for normal add dces for every bit */
   frc_qcp = &(np->nu2.qcval[ibase + bi]);

   __qcaf_dcehdr = NULL;
   /* for constant rhs thsi can be nil */
   /* LOOKATME-could decompose lhs-rhs bit but rare and speed non-critical */
   bld_qcaf_dces(qcfstp->st.sqca->qcrhsx, frc_qcp);

   /* SJM 12/22/02 - per inst here is stmt when lhs XMR */ 
   dcllp->dcelsttab[__inum*lhsx->szu.xclen + xbi] = __qcaf_dcehdr;
  }
}

/*
 * after possible concat unwinding, prepare lhs expr release
 * just allocs qcval if needed in case seen before force
 */
static void prep_noncat_qc_wirerelease(struct expr_t *lhsx)
{
 int32 biti, bitj;
 struct net_t *np;
 struct itree_t *itp;
 
 /* get the wire range - using common routine but only need net here */
 __get_qc_wirrng(lhsx, &np, &biti, &bitj, &itp); 

 /* SJM 07/19/02 - for lhs xmr need target itree loc */
 /* rest needs possible lhs xmr itree context including qcval alloc */
 if (itp != NULL) __push_itstk(itp);

 /* allocate the qcval record for wire in case see release before force */
 if (np->nu2.qcval == NULL) __alloc_qcval(np);

 if (itp != NULL) __pop_itstk();
}

/*
 * ROUTINES TO SETUP QUASI-CONTINOUS ASSIGN STORE AND DCE LIST
 */

/*
 * build and link on special qc assign/force rhs simple dce
 * xp is rhs expr and called from itree loc. where exec qc assign
 */
static void bld_qcaf_dces(struct expr_t *xp, struct qcval_t *qcvalp)
{
 register word32 *wp;
 struct net_t *np;
 int32 biti, bitj;
 struct expr_t *idndp, *ndx;
 struct expr_t *fax;
 
 switch ((byte) xp->optyp) {
  case GLBREF:
   idndp = xp; 
   /* for global - do not need ref. point - just link on 1 (because only 1 */
   /* monit call from 1 inst.) target wire */
   biti = bitj = -1;
glb_dce:
   np = idndp->lu.sy->el.enp;
   linkon_qcaf_dce(np, biti, bitj, idndp->ru.grp, qcvalp);
   break;
  case ID:
   idndp = xp; 
   np = xp->lu.sy->el.enp;
   linkon_qcaf_dce(np, -1, -1, (struct gref_t *) NULL, qcvalp);
   break;
  /* SJM 05/18/00 - must do nothing for reals */ 
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM: case OPEMPTY:
   return;
  case LSB:
   idndp = xp->lu.x; 
   np = idndp->lu.sy->el.enp;
   ndx = xp->ru.x;
   /* for monits, any reg or non scalaraed wire must trigger on any chg */
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
     bld_qcaf_dces(ndx, qcvalp);
     biti = -1;
    }
   bitj = biti;
   if (biti != -1 && !np->vec_scalared) biti = bitj = -1;
   if (idndp->optyp == GLBREF) goto glb_dce;
   linkon_qcaf_dce(np, biti, biti, (struct gref_t *) NULL, qcvalp);
   break;
  case PARTSEL:
   idndp = xp->lu.x; 
   np = idndp->lu.sy->el.enp;
   ndx = xp->ru.x;
   /* know part select never IS */
   biti = __contab[ndx->lu.x->ru.xvi];
   bitj = __contab[ndx->ru.x->ru.xvi];
   if (!np->vec_scalared) biti = bitj = -1;
   if (idndp->optyp == GLBREF) goto glb_dce;
   linkon_qcaf_dce(np, biti, bitj, (struct gref_t *) NULL, qcvalp);
   break;
  case FCALL:
   /* if any arguments of system or user functions change, monitor triggers */
   /* notice $time function do not have arguments */
   for (fax = xp->ru.x; fax != NULL; fax = fax->ru.x)
    bld_qcaf_dces(fax->lu.x, qcvalp);
   break;
  case LCB:
   for (fax = xp->ru.x; fax != NULL; fax = fax->ru.x)
    bld_qcaf_dces(fax->lu.x, qcvalp);
   break;
  default:
   if (xp->lu.x != NULL) bld_qcaf_dces(xp->lu.x, qcvalp);
   if (xp->ru.x != NULL) bld_qcaf_dces(xp->ru.x, qcvalp);
   break;
 } 
}

/*
 * link on a special (simplified) qc assign/force dce rhs load
 * caller must decompose any scalared wire part selects into bit selects
 * before here
 * -2 IS form impossible since any one inst. IS form converted to constant
 * before here
 *
 * this must be called from source reference location of the qc stmt 
 *
 * notice never a need for an old value since better to just re-eval assign
 * this goes on front but after any DMPV 
 */
static void linkon_qcaf_dce(struct net_t *np, int32 biti, int32 bitj,
 struct gref_t *grp, struct qcval_t *qcvalp)
{
 int32 nd_itpop;
 struct itree_t *ref_itp;
 struct dcevnt_t *dcep;
 struct dceauxlst_t *dclp;

 ref_itp = __inst_ptr;
 nd_itpop = FALSE;
 if (grp != NULL) { __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
 /* allocate, init, and fill the fields */
 dcep = __alloc_dcevnt(np);
 if (biti == -1) dcep->dce_typ = DCE_QCAF;
 else
  {
   dcep->dce_typ = DCE_RNG_QCAF;
   dcep->dci1 = biti;
   dcep->dci2.i = bitj;
  }
 dcep->dce_np = np;
 /* dce's start out on so must explicitly turn prep time built qc off here */
 dcep->dce_off = TRUE;

 /* link this on front */
 dcep->dcenxt = np->dcelst;
 np->dcelst = dcep;

 /* set ref. itree location - since dcep on target */
 dcep->dce_1inst = TRUE;
 dcep->dce_matchitp = __inst_ptr;
 dcep->dce_refitp = ref_itp;
 /* SJM 07/19/02 needed to make sure chg form iops get gened */ 
 np->nchg_nd_chgstore = TRUE;

 /* also set unused fmon field to qcval for bit or wire if reg */
 dcep->dceu2.dce_qcvalp = qcvalp;

 /* then link on undo/chg list */
 dclp = (struct dceauxlst_t *) __my_malloc(sizeof(struct dceauxlst_t));
 dclp->ldcep = dcep; 
 dclp->dclnxt = __qcaf_dcehdr;
 __qcaf_dcehdr = dclp;

 /* SJM 06/23/04 ### ??? - without regen is this needed? */
 /* SJM 02/06/03 - may have npps but not dces so must turn this on */
 /* since nchg nd chgstore on, know nchg action right */
 if (np->ntyp >= NONWIRE_ST) np->nchg_has_dces = TRUE;

 if (nd_itpop) __pop_itstk();
}

/*
 * XMR VARIABLE PREPARATION ROUTINES
 */

/*
 * prepare xmrs - allocate per inst. storage and set itree pointers
 *
 * no itree context here - must use explicit itree locs
 */
extern void __prep_xmrs(void)
{
 register int32 gri; 
 register struct mod_t *mdp;
 register struct gref_t *grp;
 int32 ii;
 struct itree_t *itp;

 /* first process rooted and count upward rel. in static tree */
 __num_uprel_glbs = 0;
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   for (gri = 0, grp = &(mdp->mgrtab[0]); gri < mdp->mgrnum; gri++, grp++)
    {
     /* DBG remove - if any gr_err should not get here */
     if (grp->gr_err) __misc_terr(__FILE__, __LINE__);
     if (grp->gr_gone) continue;

     /* if guessed wrong xmr from used in instance argument loc., undo */
     /* if rooted set root target itp */ 
     if (grp->is_rooted)
      {
       /* find itree root corresponding to root path staring module name */
       /* never constant inst array select */
       if ((ii = __ip_indsrch(grp->grcmps[0]->synam)) == -1)
	__misc_gfterr(__FILE__, __LINE__, grp->grfnam_ind, grp->grflin_cnt);
       itp = __it_roots[ii];

       if (grp->last_gri != 0)
        grp->targu.targitp = __find_unrt_targitp(grp, itp, 1);
       /* one component rooted, so already have itp */
       else grp->targu.targitp = itp;

       /* must mark module as containing rooted gref */
       /* and first rooted gref - allocate flatinum style itree table */
       continue;
      }
     /* notice one uprel xmr has flatinum number in itree */
     if (grp->upwards_rel)
      {
       /* setting of inst. selects in upward rels, handled elsewhere */
       __num_uprel_glbs += mdp->flatinum; 
       process_upwards_grp(grp);
       continue;
      }
     /* for downward relative with array of inst. selects must check ranges */
     if (grp->path_has_isel) chk_downrel_inst_sels(grp);
    }
  }
} 

/*
 * fill grp targ field for rooted grp and uprel itps tabl for upward rel
 * called for interactive and delay setting run time xmr eval only
 */
extern void __fill_grp_targu_fld(struct gref_t *grp)
{
 int32 ii;
 struct itree_t *itp2;

 if (grp->gr_err || grp->gr_gone) return;

 /* if rooted set root target itp */ 
 if (grp->is_rooted)
  {
   /* find itree root corresponding to root path staring module name */
   if ((ii = __ip_indsrch(grp->grcmps[0]->synam)) == -1)
    __misc_gfterr(__FILE__, __LINE__, grp->grfnam_ind, grp->grflin_cnt);
   itp2 = __it_roots[ii];
   /* may be rooted and in top module */
   if (grp->last_gri != 0)
     grp->targu.targitp = __find_unrt_targitp(grp, itp2, 1);
   else grp->targu.targitp = itp2;
   return;
  }
 /* since never more than 1 inst. of interactive scope, up from current */
 /* scope - this implies must always reparse interactive statements */
 /* SJM 09/15/00 - this fills the per ref. indexed targ itps table */
 if (grp->upwards_rel) process_upwards_grp(grp);
}

/*
 * process upward relative gref by building downward reverse of upward
 * path to first above module of matching type
 *
 * rule is go upward until find matching 0th component type, then down
 * this is needed because for inst. upward distance may be different
 * because inst array pound param range types copied before defparam
 * splitting, first upward rel. first component symbol will be right split off
 */
static void process_upwards_grp(struct gref_t *grp)
{
 register int32 ii;
 struct mod_t *imdp, *up_mdp, *mast_imdp, *up_mast_mdp;
 struct itree_t *in_itp, *up_itp, *titp;
 struct inst_t *ip;
 struct sy_t *syp;
 struct itree_t **uprelitps;

 /* notice for upward relative, first component is module type not inst */  
 syp = grp->grcmps[0];
 /* DBG remove - upward relative xmr head not module type */
 if (syp->sytyp != SYM_M)
  __misc_gfterr(__FILE__, __LINE__, grp->grfnam_ind, grp->grflin_cnt);
 /* -- */

 uprelitps = (struct itree_t **)
  __my_malloc(grp->gin_mdp->flatinum*sizeof(struct itree_t *));
 grp->targu.uprel_itps = uprelitps;

 /* for upward distance is per inst. variable - up to find target mod */
 up_mdp = syp->el.emdp;
 up_mast_mdp = __get_mast_mdp(up_mdp);
 for (ii = 0; ii < grp->gin_mdp->flatinum; ii++)
  {
   in_itp = grp->gin_mdp->moditps[ii];

   for (up_itp = in_itp;;)
    {
     if (up_itp == NULL)
      {
       __gferr(981, grp->grfnam_ind, grp->grflin_cnt,
        "unqualified use of upward relative task/func %s illegal - matched upward relative task/func undefined above instance %s",
        grp->gnam, __msg2_blditree(__xs, in_itp)); 
       return;
      }

     /* upward xmr reference no matching type above */
     ip = up_itp->itip;
     imdp = ip->imsym->el.emdp;
     mast_imdp = __get_mast_mdp(imdp);
     /* DBG remove --
     __dbg_msg("upwards rel: %s: first comp=%s, current mdp: %s(%s)=%d\n",
      grp->gnam, up_mast_mdp->msym->synam, mast_imdp->msym->synam,
      ip->isym->synam, up_itp->itinum);
     -- */
     if (mast_imdp == up_mast_mdp) break;
     up_itp = up_itp->up_it;
    }
   /* next trace down from upward module type match (using it inst) */
   /* 0th is module that determines upward distance */
   if (grp->last_gri != 0) titp = __find_unrt_targitp(grp, up_itp, 1);
   /* one component upward relative module name */
   else titp = up_itp;

   uprelitps[ii] = titp;
  }
}     

/*
 * given possible split off module type get the mast type
 *
 * pound params may be 1 level split off and defparams one more
 * i.e. all pound parameters split off from one master (maybe >1 inst)
 * then defparam split off from either normal or pound split off
 */
extern struct mod_t *__get_mast_mdp(struct mod_t *mdp)
{
 struct mod_t *mast_mdp;

 if (mdp->msplit) 
  {
   mast_mdp = mdp->mspltmst;
   if (mast_mdp->mpndsplit) mast_mdp = mast_mdp->mpndspltmst;
  }
 else if (mdp->mpndsplit) mast_mdp = mdp->mpndspltmst;
 else mast_mdp = mdp;
 return(mast_mdp);
}

/*
 * check all instances of downward relative instance selects
 *
 * only called for downward relative that have inst array selects
 */
static void chk_downrel_inst_sels(struct gref_t *grp)
{
 register int32 ii; 
 struct mod_t *mdp; 
 struct itree_t *itp;
 int32 sav_ecnt;

 sav_ecnt = __pv_err_cnt;
 mdp = grp->gin_mdp;
 for (ii = 0; ii < mdp->flatinum; ii++)
  {
   itp = mdp->moditps[ii];
   /* DBG remove --- */
   if (itp == NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */
   /* this does checking for each and emits error */  
   __find_unrt_targitp(grp, itp, 0);
   /* once error is found stop */
   if (__pv_err_cnt > sav_ecnt) break;
  }
}

/*
 * SPECIFY SECTION PREPARATION ROUTINES
 */

extern void __prep_specify(void)
{
 prep_tchks();
 prep_pths();
}

/*
 * TIMING CHECK REPRESENTATION CHANGE ROUTINES
 */

/*
 * routine to prepare timing checks 
 * for vectors in timing check event slots must split into 1 bit wide
 * checks since each bit timing reference and data events separate
 *
 * notice timing checks cannot be xmrs (must be I/O ports) so no xmr
 * processing net pin elements
 */
static void prep_tchks(void)
{
 register struct tchk_t *tcp;
 register int32 i1, i2;
 int32 starti1, starti2, chki1, chki2;
 struct mod_t *mdp;
 struct net_t *startnp, *chknp;
 struct tchg_t *start_tchgp;
 struct chktchg_t *chk_tchgp;
 struct gate_t gwrk;
 
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {

   /* multiple blocks each with different symbol table legal */
   /* if option to ignore, specify will be remove before here */
   if (mdp->mspfy == NULL) continue;

   __push_wrkitstk(mdp, 0);
   for (tcp = mdp->mspfy->tchks; tcp != NULL; tcp = tcp->tchknxt)
    {
     /* do not convert since always will not get initialized - invisible */
     if (tcp->tc_gone) continue;

     /* convert ref. and data events to wire and range */
     /* know events is non concatente lhs (wire, const. bit/part select) */
     __xtract_wirng(tcp->startxp, &startnp, &starti1, &starti2);
     if (tcp->tchktyp == TCHK_PERIOD || tcp->tchktyp == TCHK_WIDTH) 
      {
       chknp = startnp; chki1 = starti1; chki2 = starti2;
       /* need to copy to make timing check violation check code work */
       tcp->chkxp = __copy_expr(tcp->startxp);
       tcp->chkcondx = __copy_expr(tcp->startcondx);
       if (tcp->tchktyp == TCHK_WIDTH)
        {
         /* opposite edge */
         if (tcp->startedge == E_NEGEDGE) tcp->chkedge = E_POSEDGE;
         else if (tcp->startedge == E_POSEDGE) tcp->chkedge = E_NEGEDGE;
         else __case_terr(__FILE__, __LINE__);
        }
       /* same edge */
       else tcp->chkedge = tcp->startedge;
      }
     else __xtract_wirng(tcp->chkxp, &chknp, &chki1, &chki2);

     switch ((byte) tcp->tchktyp) {
      /* SJM 01/16/04 - added $removal - reversed terms of $recovery */
      case TCHK_SETUP: case TCHK_HOLD: case TCHK_SKEW: case TCHK_RECOVERY:
      case TCHK_SETUPHOLD: case TCHK_REMOVAL: case TCHK_RECREM:
       /* timing check range all full - every bit time checked against */
       for (i1 = starti1; i1 >= starti2; i1--)
        {
         start_tchgp = bld_start_tchk_npp(tcp, startnp, i1); 
         for (i2 = chki1; i2 >= chki1; i2--)
         {
          chk_tchgp = bld_check_tchk_npp(chknp, i2); 
          /* check event needs access to start - also path to tchk master */
          chk_tchgp->startchgp = start_tchgp;
         }
        } 
       break;
      case TCHK_WIDTH: case TCHK_PERIOD:
       /* after above fixup, like normal except ==> not *> form */  
       /* maybe wrong and should be ? */
       for (i1 = starti1; i1 >= starti2; i1--)
        {
         /* for $period no reference event, data and reference the same */
         /* so this build ref. but no npp */
         start_tchgp = bld_start_tchk_npp(tcp, startnp, i1); 
         chk_tchgp = bld_check_tchk_npp(chknp, i1); 
         /* check event needs access to start - also path to tchk master */
         chk_tchgp->startchgp = start_tchgp;
        } 
       break;
      default: __case_terr(__FILE__, __LINE__);
     }
    /* no delay preparation for added hold of setuphold */
    if (tcp->tc_supofsuphld || tcp->tc_recofrecrem) continue;

    /* first set &&& conditional fields net still acessed from t event */
    /* width irrelevant know will always only be 1 delay */
    /* know every timing check has 1 limit field */
    __add_tchkdel_pnp(tcp, TRUE);
    __prep_delay(&gwrk, tcp->tclim_du.pdels, FALSE, FALSE,
     "first timing check limit", TRUE, tcp->tcsym, TRUE);
    if (__nd_neg_del_warn)
     {
      __gfwarn(601, tcp->tcsym->syfnam_ind, tcp->tcsym->sylin_cnt,
       "timing check negative delay changed to 0 (ok for timing verifier)");
       __nd_neg_del_warn = FALSE;
     }
    tcp->tc_delrep = gwrk.g_delrep;
    tcp->tclim_du = gwrk.g_du;

    /* notice for setuphold (actually hold part) this always on */
    if (tcp->tc_haslim2)
     {
      /* width irrelevant know will always only be 1 delay */
      __add_tchkdel_pnp(tcp, FALSE);
      __prep_delay(&gwrk, tcp->tclim2_du.pdels, FALSE, FALSE,
       "2nd timing check limit", TRUE, tcp->tcsym, TRUE);
      if (__nd_neg_del_warn)
       {
        __gfwarn(601, tcp->tcsym->syfnam_ind, tcp->tcsym->sylin_cnt,
         "timing check negative delay changed to 0 (ok for timing verifier)");
        __nd_neg_del_warn = FALSE;
       } 
      tcp->tc_delrep2 = gwrk.g_delrep;
      tcp->tclim2_du = gwrk.g_du;
     }
    }
   __pop_wrkitstk();
  }
}

/*
 * extract wire and range
 * know ranges always constants
 */
extern void __xtract_wirng(struct expr_t *xp, struct net_t **np,
 int32 *i1, int32 *i2)
{
 word32 *wp;

 switch ((byte) xp->optyp) {
  case ID:
   *np = xp->lu.sy->el.enp;
   *i1 = *i2 = -1;
   break;
  case LSB:
   *np = xp->lu.x->lu.sy->el.enp;
   wp = &(__contab[xp->ru.x->ru.xvi]);
   *i1 = *i2 = (int32) wp[0]; 
   break;
  case PARTSEL:
   *np = xp->lu.x->lu.sy->el.enp;
   *i1 = (int32) __contab[xp->ru.x->lu.x->ru.xvi];
   *i2 = (int32) __contab[xp->ru.x->ru.x->ru.xvi];
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * build the start (earliest reference) event
 * for period need the start change (ref.) event but no npp
 */
static struct tchg_t *bld_start_tchk_npp(struct tchk_t *tcp,
 struct net_t *startnp, int32 bi1)
{
 register int32 ii;
 word64 t;
 struct tchg_t *start_tchgp;

 start_tchgp = (struct tchg_t *) __my_malloc(sizeof(struct tchg_t));
 start_tchgp->chgu.chgtcp = tcp;
 start_tchgp->oldval = bld_npp_oldval(startnp, __inst_mod);
 start_tchgp->lastchg = (word64 *)
  __my_malloc(__inst_mod->flatinum*sizeof(word64));
 /* FIXME - why are 2nd later change time and t chg rec unused here */
 t = 0ULL;
 for (ii = 0; ii < __inst_mod->flatinum; ii++) start_tchgp->lastchg[ii] = t;

 /* for $period, no npp just placer holder set when data event occurs */ 
 if (tcp->tchktyp != TCHK_PERIOD)
  {
   __cur_npnum = 0;
   if (!startnp->n_isavec) bi1 = -1;
   __conn_npin(startnp, bi1, bi1, FALSE, NP_TCHG, (struct gref_t *) NULL,
     NPCHG_TCSTART, (char *) start_tchgp);

   /* SJM - 04/04/02 need to turn on chg processing if only load */
   /* DBG remove -- */
   if (startnp->nlds == NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */
   if (startnp->nlds->npnxt == NULL)
    {
     startnp->nchg_has_lds = TRUE;
     startnp->nchg_nd_chgstore = TRUE;

     /* if also no dces, now when add tchk load, must turn off all chged */
     if (startnp->dcelst == NULL)
      {
       for (ii = 0; ii < __inst_mod->flatinum; ii++)
        { startnp->nchgaction[ii] &= ~(NCHG_ALL_CHGED); }
      }
    }
  }
 return(start_tchgp);
}

/*
 * build the check net pin event (later data event)
 */
static struct chktchg_t *bld_check_tchk_npp(struct net_t *chknp, int32 bi1)
{
 register int32 ii; 
 word64 t;
 struct chktchg_t *chk_tchgp;

 chk_tchgp = (struct chktchg_t *) __my_malloc(sizeof(struct chktchg_t));
 /* link to start and tchk master accessed through start but caller sets */
 /* tchk master accessed through union in start tim chg */
 /* FIXME - why are 2nd later change time and t chg rec unused here */
 chk_tchgp->chklastchg = (word64 *)
  __my_malloc(__inst_mod->flatinum*sizeof(word64));
 t = 0ULL;
 for (ii = 0; ii < __inst_mod->flatinum; ii++) chk_tchgp->chklastchg[ii] = t;
 chk_tchgp->chkoldval = bld_npp_oldval(chknp, __inst_mod);
 __cur_npnum = 0;
 if (!chknp->n_isavec) bi1 = -1;
 __conn_npin(chknp, bi1, bi1, FALSE, NP_TCHG, (struct gref_t *) NULL,
  NPCHG_TCCHK, (char *) chk_tchgp);

 /* SJM - 04/04/02 need to turn on chg processing if only load */
 /* DBG remove -- */
 if (chknp->nlds == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */
 if (chknp->nlds->npnxt == NULL)
  {
   chknp->nchg_has_lds = TRUE;
   chknp->nchg_nd_chgstore = TRUE;

   /* if also no dces, now when add tchk load, must turn off all chged */
   if (chknp->dcelst == NULL)
    {
     for (ii = 0; ii < __inst_mod->flatinum; ii++)
      { chknp->nchgaction[ii] &= ~(NCHG_ALL_CHGED); }
    }
  }
 return(chk_tchgp);
}

/*
 * build net pin old value for detecting path source and tc event bit chgs
 */
static byte *bld_npp_oldval(struct net_t *np, struct mod_t *mdp)
{
 int32 stval, ival, insts;
 byte sval, *bp;

 insts = mdp->flatinum;
 if (np->n_stren)
  { 
   bp = (byte *) __my_malloc(insts);
   __get_initval(np, &stval);
   sval = (byte) stval;
   set_byteval_(bp, insts, sval);
   return(bp);
  }
 bp = (byte *) __my_malloc(insts);
 ival = __get_initval(np, &stval);
 set_byteval_(bp, insts, ival);
 return(bp);
}

/*
 * build net pin old value for detecting path source and tc event bit chgs
 */
static void reinit_npp_oldval(byte *bp, struct net_t *np, struct mod_t *mdp)
{
 int32 stval;
 int32 insts, ival;
 byte sval;

 insts = mdp->flatinum;
 if (np->n_stren)
  { 
   __get_initval(np, &stval);
   sval = (byte) stval;
   set_byteval_(bp, insts, sval);
  }
 else 
  {
   ival = __get_initval(np, &stval);
   set_byteval_(bp, insts, ival);
  }
}

/*
 * prepare path elements and delays
 * 1) convert path expr. lists to path elements
 * 2) convert delays - know delay expression is 1, 2, 3, or 6 numbers
 *    here delay expr. must still be scaled
 * 3) check inout ports that are path srcs to be sure has input driver
 *    pattern and path and path dest. has output driver pattern only
 *
 * LOOKATME - also somewhere back annotation can create IS forms ?
 */
static void prep_pths(void)
{
 register struct spcpth_t *pthp;
 int32 rv, mod_has_path;
 struct mod_t *mdp;
 struct gate_t gwrk;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mspfy == NULL) continue;

   __push_wrkitstk(mdp, 0);

   mod_has_path = FALSE;
   /* know each delay is NUMBER or REALNUM */ 
   for (pthp = __inst_mod->mspfy->spcpths; pthp != NULL;
    pthp = pthp->spcpthnxt)
    {
     /* do not convert since always will not get initialized - invisible */
     if (pthp->pth_gone) continue;

     if (!chk_pthels(pthp)) { pthp->pth_gone = TRUE; continue; }
     if (!bldchk_pb_pthdsts(pthp)) { pthp->pth_gone = TRUE; continue; }

     /* prepare the delay - notice this uses __inst_mod */
     __add_pathdel_pnp(pthp);
     __prep_delay(&gwrk, pthp->pth_du.pdels, TRUE, FALSE, "path delay",
      TRUE, pthp->pthsym, TRUE);
     if (__nd_neg_del_warn)
      {
       __gferr(981, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt, 
        "path delay negative (used 0)");
       __nd_neg_del_warn = FALSE;
      } 
     pthp->pth_delrep = gwrk.g_delrep;
     pthp->pth_du = gwrk.g_du;
     if (pthp->datasrcx != NULL)
      {
       __gfinform(481, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt, 
        "path polarity operator ignored meaningless in simulator"); 
      } 
     /* and check for any illegal 0 delays */
     if ((rv = __chk_0del(pthp->pth_delrep, pthp->pth_du, __inst_mod))
      != DGOOD)
      {
       if (rv != DBAD_MAYBE0 && rv != DBAD_0)
        {
         __gferr(964, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
          "path delay illegal (%s delay form)", __to_deltypnam(__xs,
          pthp->pth_delrep));
        }
       else
        {
         /* for IO PAD cells some 0 delays common */
         if (rv == DBAD_MAYBE0)
          {
           __gfinform(483, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
           "path delay (%s form) contains some zero delays - primitive delay may be better",
           __to_deltypnam(__xs, pthp->pth_delrep));
          }
         else
          {
           __gfinform(484, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
            "path delay (%s form) all zeros - no effect unless annotated to",
            __to_deltypnam(__xs, pthp->pth_delrep));
          }
        }
      }
     mod_has_path = TRUE;
    }
   if (mod_has_path) emit_pthdst_bit_informs(__inst_mod);
   __pop_wrkitstk();
  }
}

/*
 * check all path components - check things that apply to path elements
 * check for bit by bit things when building sim path d.s.
 */
static int32 chk_pthels(struct spcpth_t *pthp)
{
 register int32 pei;
 struct pathel_t *pep;
 struct net_t *np;
 int32 gd_path;

 gd_path = TRUE;
 for (pei = 0; pei <= pthp->last_pein; pei++)
  {
   pep = &(pthp->peins[pei]);
   np = pep->penp; 
   /* DBG remove */
   if (!np->n_isapthsrc) __misc_terr(__FILE__, __LINE__);
   /* --- */ 
   /* path source can be vectored or scalared (can be reg) */
   /* since source just timing check ref. event - record time of any chg */
  }
 for (pei = 0; pei <= pthp->last_peout; pei++)
  {
   pep = &(pthp->peouts[pei]);
   np = pep->penp; 
   if (np->n_isavec && !np->vec_scalared)
    {
     __gferr(825, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
      "vectored path destination wire %s illegal - must be scalared",
      np->nsym->synam);
     gd_path = FALSE;
    }
   /* if both has wire delay and dest., pth dst bit was turned off */
   /* LOOKATME - not sure what to check here */
   if (np->ntyp == N_TRIREG)
    {
     __gferr(826, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
      "path destination wire %s illegal trireg type", np->nsym->synam);
     gd_path = FALSE;
    }
   else if (!np->n_isapthdst
    || (np->nrngrep == NX_DWIR && np->nu.rngdwir->n_delrep != DT_PTHDST))
    {
     __gferr(826, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
      "path destination wire %s illegal - has wire delay", np->nsym->synam);
     gd_path = FALSE;
    }
  }
 return(gd_path);
}

/*
 * build and check the per bit but not per inst path dest table lists
 * all non bit specific path properties already checked
 */
static int32 bldchk_pb_pthdsts(struct spcpth_t *pthp)
{
 register int32 spi, dpi;
 register struct pathel_t *spep, *dpep;
 int32 pbiwid, pbowid;
 int32 sbi, sbi2, dbi, dbi2, pb_gd, nxt_spep, nxt_dpep;
 struct net_t *snp, *dnp;
 struct tchg_t *src_tchg;

 pb_gd = TRUE; 
 /* needed since loop initialization indirect and lint cannot detect */
 sbi = sbi2 = dbi = dbi2 = -1;
 snp = dnp = NULL;
 src_tchg = NULL;
 if (pthp->pthtyp == PTH_PAR)
  { 
   get_pthbitwidths(pthp, &pbiwid, &pbowid);
   if (pbiwid != pbowid)
    {
     __gferr(839, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
      "parallel path input bit length %d and output %d differ",
      pbiwid, pbowid);
     return(FALSE);
    }
   /* know bit lengths the same and if vectors or selects scalared */
   for (spi = dpi = -1, nxt_spep = nxt_dpep = TRUE;;)   
    {
     if (nxt_spep)
      {
       /* before moving to next bit always add net pin element */
       if (++spi > pthp->last_pein)
        {
         if (!nxt_dpep || (dpi + 1) <= pthp->last_peout)
          __misc_terr(__FILE__, __LINE__);
         break;
        }
       spep = &(pthp->peins[spi]);
       snp = spep->penp;
       if (!snp->n_isavec) sbi = sbi2 = 0; 
       else
        {
         if (spep->pthi1 == -1) { sbi = snp->nwid - 1; sbi2 = 0; }  
         else { sbi = spep->pthi1; sbi2 = spep->pthi2; }
        }
       /* build and add (if non already added) path dest. NP_DPTHSRC */
       src_tchg = try_add_npp_dpthsrc(pthp, snp, sbi);
       nxt_spep = FALSE; 
      }
     if (nxt_dpep)
      {
       /* if done, will always exit on src over run of end */ 
       if (++dpi > pthp->last_peout) __misc_terr(__FILE__, __LINE__);
       dpep = &(pthp->peouts[dpi]);
       dnp = dpep->penp;
       if (!dnp->n_isavec) dbi = dbi2 = 0; 
       else
        {
         if (dpep->pthi1 == -1) { dbi = dnp->nwid - 1; dbi2 = 0; }  
         else { dbi = dpep->pthi1; dbi2 = dpep->pthi2; }
        }
       nxt_dpep = FALSE; 
      }
     if (!bldchk_1bit_pthdst(pthp, snp, sbi, dnp, dbi, dnp->nwid, src_tchg))
      pb_gd = FALSE;
     if (--sbi < sbi2) nxt_spep = TRUE;
     if (--dbi < dbi2) nxt_dpep = TRUE;
    }
   return(pb_gd);
  }
 /* handle full path case - for every source bit */
 for (spi = 0; spi <= pthp->last_pein; spi++)
  {
   spep = &(pthp->peins[spi]);
   snp = spep->penp;
   if (!snp->n_isavec) sbi = sbi2 = 0; 
   else
    {
     if (spep->pthi1 == -1) { sbi = snp->nwid - 1; sbi2 = 0; }  
     else { sbi = spep->pthi1; sbi2 = spep->pthi2; }
    }
   for (; sbi >= sbi2; sbi--)
    {
     src_tchg = try_add_npp_dpthsrc(pthp, snp, sbi);
     /* for given bit of input path - for every output path */ 
     for (dpi = 0; dpi <= pthp->last_peout; dpi++)
      {
       dpep = &(pthp->peouts[dpi]);
       dnp = dpep->penp;
       if (!dnp->n_isavec) dbi = dbi2 = 0; 
       else
        {
         if (dpep->pthi1 == -1) { dbi = dnp->nwid - 1; dbi2 = 0; }  
         else { dbi = dpep->pthi1; dbi2 = dpep->pthi2; }
        }
       /* for every bit of destination path element */
       for (; dbi >= dbi2; dbi--)
        {
         /* notice here, must check every sbits by dbits combination */
         if (!bldchk_1bit_pthdst(pthp, snp, sbi, dnp, dbi, dnp->nwid,
          src_tchg)) pb_gd = FALSE;
        }
      }
    }
  }
 return(pb_gd);
}

/*
 * build and add the path source net pin entry - like tc ref. event
 * just records latest change
 *
 * for cases with mulitple source for 1 path only add first time
 * here special indirect npp that allows indexing by bit and comparison
 *
 * if non empty or separate bit lists make sense
 * best is to put in pass that checks to see if needed and adds after here
 */
static struct tchg_t *try_add_npp_dpthsrc(struct spcpth_t *pthp,
 struct net_t *snp, int32 sbi)
{
 register int32 ii;
 struct net_pin_t *npp;
 word64 t;
 struct tchg_t *start_tchgp;

 if ((npp = find_1timchg_psnpp(snp, sbi, NPCHG_PTHSRC)) != NULL)
  return(npp->elnpp.etchgp);

 start_tchgp = (struct tchg_t *) __my_malloc(sizeof(struct tchg_t));
 start_tchgp->chgu.chgpthp = pthp;
 start_tchgp->oldval = bld_npp_oldval(snp, __inst_mod);
 start_tchgp->lastchg = (word64 *)
  __my_malloc(__inst_mod->flatinum*sizeof(word64));
 
 t = 0ULL;
 for (ii = 0; ii < __inst_mod->flatinum; ii++) start_tchgp->lastchg[ii] = t;
 __cur_npnum = 0;
 /* sbi access inst. table so must be 0 but must be -1 for npp */
 if (!snp->n_isavec) sbi = -1;

 /* with table for moving down to col. from inst. */
 __conn_npin(snp, sbi, sbi, FALSE, NP_TCHG, (struct gref_t *) NULL,
  NPCHG_PTHSRC, (char *) start_tchgp);
 return(start_tchgp);
}

/*
 * find a path source time change npp 
 * know __inst_mod set 
 */
static struct net_pin_t *find_1timchg_psnpp(struct net_t *snp, int32 bi,
 int32 subtyp)
{
 register struct net_pin_t *npp;
 struct npaux_t *npauxp;

 for (npp = snp->nlds; npp != NULL; npp = npp->npnxt) 
  {
   if (npp->npntyp == NP_TCHG && npp->chgsubtyp == subtyp
    && ((npauxp = npp->npaux) != NULL && npauxp->nbi1 == bi)) return(npp);
  }
 return(NULL);
}

/*
 * build 1 bit path dest. pair simulation path and do 1 bit path checking
 * if error path dest. not built
 *
 * for scalar dbi will be 0
 */
static int32 bldchk_1bit_pthdst(struct spcpth_t *pthp, struct net_t *s_np,
 int32 sbi, struct net_t *d_np, int32 dbi, int32 dnwid,
 struct tchg_t *src_tchg)
{
 register int32 i;
 int32 pb_gd;
 struct pthdst_t *pdp;
 char s1[RECLEN], s2[RECLEN];

 /* for inouts - source and dest. cannot be the same */
 pb_gd = TRUE;
 if (d_np->nsym == s_np->nsym && dbi == sbi)
  {
   __gfwarn(617, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
    "ignoring path from %s%s to %s%s because source and destination same wire - simulators that split inout ports results may differ",
    s_np->nsym->synam, bld_bitref(s1, s_np, sbi), d_np->nsym->synam,
    bld_bitref(s2, d_np, dbi));
   return(FALSE);
  }
 /* if previous error, net marked as gone here */
 if (d_np->n_gone || s_np->n_gone) return(FALSE);

 /* check for no more than one driver per bit */ 
 /* LOOKATME - now driver for 1364 can be anything and */ 
 /* tran or inouts are not drivers (not hard), does this work? */
 if (!chk_biti_pthdst_driver(pthp, d_np, dbi)) return(FALSE);

 /* know for dest. rng dwir already allocated */
 /* if first bit of this wire, allocate table and mark all unused */
 if (d_np->nu.rngdwir->n_du.pb_pthdst == NULL)
  {
   d_np->nu.rngdwir->n_du.pb_pthdst = (struct pthdst_t **)
    __my_malloc(dnwid*sizeof(struct pthdst_t *));
   for (i = 0; i < dnwid; i++) d_np->nu.rngdwir->n_du.pb_pthdst[i] = NULL;
  }
 /* if this is path with different source but same destination, allocate */
 pdp = (struct pthdst_t *) __my_malloc(sizeof(struct pthdst_t));
 pdp->pstchgp = src_tchg;
 pdp->pdnxt = d_np->nu.rngdwir->n_du.pb_pthdst[dbi];
 d_np->nu.rngdwir->n_du.pb_pthdst[dbi] = pdp;
 return(pb_gd);
}

/*
 * compute a path's input and output bit widths
 */
static void get_pthbitwidths(struct spcpth_t *pthp, int32 *pinwid,
 int32 *poutwid)
{
 register int32 pei;
 int32 pbwid;
 struct pathel_t *pep;

 for (pei = 0, pbwid = 0; pei <= pthp->last_pein; pei++)
  {
   pep = &(pthp->peins[pei]);
   /* notice all ranges normalized to h:l here */
   if (pep->pthi1 == -1) pbwid += pep->penp->nwid; 
   else pbwid += (pep->pthi1 - pep->pthi2 + 1); 
  }
 *pinwid = pbwid;
  for (pei = 0, pbwid = 0; pei <= pthp->last_peout; pei++)
   {
    pep = &(pthp->peouts[pei]);
    if (pep->pthi1 == -1) pbwid += pep->penp->nwid;
    else pbwid += (pep->pthi1 - pep->pthi2 + 1); 
   }
 *poutwid = pbwid;
} 

/*
 * build a bit reference of form [<bit>] or ""
 */
static char *bld_bitref(char *s, struct net_t *np, int32 bi)
{
 if (!np->n_isavec || bi == -1) strcpy(s, "");
 else sprintf(s, "[%d]", bi);
 return(s);
}

/*
 * check driver of bit to make sure bit has exactly one driver
 */
static int32 chk_biti_pthdst_driver(struct spcpth_t *pthp, struct net_t *np,
 int32 biti)
{
 register struct net_pin_t *npp;
 register struct npaux_t *npauxp;
 int32 num_drvs;
 char s1[RECLEN];

 if (!np->n_isavec)
  {
   if (np->ndrvs == NULL) goto no_drv;
   for (num_drvs = 0, npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
    {
     if (npp->npntyp > NP_MDPRT) continue;
     if (npp->npntyp <= NP_MDPRT && npp->np_xmrtyp != XNP_LOC)
      goto pthdst_xmr;
     if (++num_drvs > 1)
      {
       __gferr(827, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
        "path destination %s illegal - scalar wire has more than one driver",
        np->nsym->synam);
       return(FALSE);
      }  
    }
   if (num_drvs == 0) goto no_drv;
  }

 /* here only check drivers of this bit */
 for (num_drvs = 0, npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
  {
   if ((npauxp = npp->npaux) == NULL || npauxp->nbi1 == -1) goto got_match;
   /* must never see IS form driver here - must cause copying earlier */ 
   /* DBG remove --- */
   if (npauxp->nbi1 == -2) __misc_terr(__FILE__, __LINE__);   
   /* --- */

   if (biti > npauxp->nbi1 || biti < npauxp->nbi2.i) continue;

got_match:
   /* found driver of bit - know only 1 */
   if (npp->npntyp <= NP_MDPRT && npp->np_xmrtyp != XNP_LOC)
    {
pthdst_xmr:
     __gferr(961, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
      "path destination %s%s cross module reference driver illegal",
      np->nsym->synam, bld_bitref(s1, np, biti));
     return(FALSE);
    }
   if (++num_drvs > 1)
    {
     __gferr(827, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
      "path destination %s%s illegal - more than one driver",
      np->nsym->synam, bld_bitref(s1, np, biti));
    }  
  }
 if (num_drvs != 0) return(TRUE);

no_drv:
 __gfwarn(616, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt,
  "path destination %s%s does not have a driver - path delay no effect",
  np->nsym->synam, bld_bitref(s1, np, biti));
 return(TRUE);
}

/*
 * emit path dest bit informs where bits not in any path for vectors
 * processes path dests in current module.
 * know module has specify section and at least one path
 *
 * SJM 06/06/00 - also emitting inform for inout as both path src/dest
 */
static void emit_pthdst_bit_informs(struct mod_t *mdp)
{
 register int32 ni, bi;
 register struct net_t *np;
 
 if (mdp->mnnum == 0) return;
 for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
  {
   /* SJM 06/06/00 - emit inform if inout used as path src and dst */   
   /* SJM 07/16/01 - removed warning, it was wrong - inout paths now work */

   if (!np->n_isapthdst || !np->n_isavec) continue;
   /* DBG remove --- */
   if (np->nu.rngdwir->n_delrep != DT_PTHDST)
    __misc_terr(__FILE__, __LINE__);
   /* --- */
   
   for (bi = 0; bi < np->nwid; bi++)   
    {
     if (np->nu.rngdwir->n_du.pb_pthdst[bi] == NULL)
      {
       __gfinform(474, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
        "for delay path destination %s, bit %d is not destionation of any path",
        np->nsym->synam, bi); 
      }
    }
  }
}

/*
 * check path delay non zero and non expression
 */
extern int32 __chk_0del(word32 drep, union del_u du, struct mod_t *mdp)
{
 register int32 i;
 int32 some_0, all_0;

 some_0 = FALSE;
 all_0 = TRUE;
 switch ((byte) drep) {
  /* think ,, form will work right here for timing checks */
  case DT_NONE: return(DBAD_NONE);
  case DT_1X: case DT_4X: return(DBAD_EXPR);
  case DT_1V:
   if (*du.d1v == 0ULL) return(DBAD_0);
   return(DGOOD);
  case DT_IS1V:
   for (i = 0; i < mdp->flatinum; i++)
    { if (du.dis1v[i] == 0ULL) some_0 = TRUE; else all_0 = FALSE; }
   break;
  case DT_IS1V1:
   for (i = 0; i < mdp->flatinum; i++)
    { if (du.dis1v1[i] == 0) some_0 = TRUE; else all_0 = FALSE; }
   break;
  case DT_IS1V2:
   for (i = 0; i < mdp->flatinum; i++)
    { if (du.dis1v2[i] == 0) some_0 = TRUE; else all_0 = FALSE; }
   break;
  case DT_4V:
   for (i = 0; i < 4; i++)
    { if (du.d4v[i] == 0ULL) some_0 = TRUE; else all_0 = FALSE; }
   break;
  case DT_IS4V:
   for (i = 0; i < 4*mdp->flatinum; i++)
    { if (du.dis4v[i] == 0ULL) some_0 = TRUE; else all_0 = FALSE; }
   break; 
  case DT_IS4V1:
   some_0 = FALSE;
   all_0 = TRUE;
   for (i = 0; i < 4*mdp->flatinum; i++)
    { if (du.dis4v1[i] == 0) some_0 = TRUE; else all_0 = FALSE; }
   break;
  case DT_IS4V2:
   some_0 = FALSE;
   all_0 = TRUE;
   for (i = 0; i < 4*mdp->flatinum; i++)
    { if (du.dis4v2[i] == 0) some_0 = TRUE; else all_0 = FALSE; }
   break;
  case DT_16V:
   for (i = 1; i < 16; i++)
    {
     /* must skip unused that will be 0 */
     if (i == 5 || i == 10 || i == 15) continue;
     if (du.d16v[i] == 0ULL) some_0 = TRUE; else all_0 = FALSE;
    }
   break;
  case DT_IS16V:
   for (i = 1; i < 16*mdp->flatinum; i++)
    { 
     if ((i % 16) == 5 || (i % 16) == 10 || (i % 16) == 15) continue; 
     if (du.dis16v[i] == 0ULL) some_0 = TRUE; else all_0 = FALSE;
    }
   break;
  case DT_IS16V1:
   for (i = 1; i < 16*mdp->flatinum; i++)
    {
     if ((i % 16) == 5 || (i % 16) == 10 || (i % 16) == 15) continue; 
     if (du.dis16v1[i] == 0) some_0 = TRUE; else all_0 = FALSE;
    }
   break;
  case DT_IS16V2:
   for (i = 0; i < 16*mdp->flatinum; i++)
    {
     if ((i % 16) == 5 || (i % 16) == 10 || (i % 16) == 15) continue; 
     if (du.dis16v2[i] == 0) some_0 = TRUE; else all_0 = FALSE;
    }
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 if (all_0) return(DBAD_0); 
 if (some_0) return(DBAD_MAYBE0);
 return(DGOOD);
}

/*
 * DESIGN PART FREE ROUTINES 
 */

/*
 * free one statement
 * this requires set __inst_mod  
 * LOOKATME - how come only called from interactive and always 1 flat inst?
 */
extern void __free_1stmt(struct st_t *stp)
{
 int32 fji;
 struct for_t *frp;
 struct qconta_t *qcafs;
 struct st_t *fjstp;

 if (stp == NULL) return;

 switch ((byte) stp->stmttyp) {
  case S_NULL: case S_STNONE: break;
  case S_PROCA: case S_FORASSGN: case S_RHSDEPROCA: case S_NBPROCA:
   __free_xtree(stp->st.spra.lhsx);
   __free_xtree(stp->st.spra.rhsx);
   break;
  case S_IF:
   __free_xtree(stp->st.sif.condx);
   __free_stlst(stp->st.sif.thenst);
   __free_stlst(stp->st.sif.elsest);
   break;
  case S_CASE:
   __free_xtree(stp->st.scs.csx);
   /* this also frees default: (maybe just place holder) and list els */
   free_csitemlst(stp->st.scs.csitems);
   break;
  case S_REPEAT:
   __free_xtree(stp->st.srpt.repx);
   /* if after preparation need to free temp. repeat count array */
   /* LOOKATME - why is this only per. inst. stor. place in stmts? */ 
   if (stp->st.srpt.reptemp != NULL)
    __my_free((char *) stp->st.srpt.reptemp,
     __inst_mod->flatinum*sizeof(word32 *));
   __free_stlst(stp->st.srpt.repst);
   break;
  case S_FOREVER:
  case S_WHILE:
   __free_xtree(stp->st.swh.lpx);
   __free_stlst(stp->st.swh.lpst);
   break;
  case S_WAIT:
   __free_xtree(stp->st.swait.lpx);
   /* free statement list since del. ctrl. points to wait itself */
   __free_stlst(stp->st.swait.lpst);
   /* do not free action statement which is wait itself */
   free_dctrl(stp->st.swait.wait_dctp, FALSE);
   break;
  case S_FOR:
   frp = stp->st.sfor;
   /* notice for assign already freed */
   __free_xtree(frp->fortermx);
   __free_1stmt(frp->forinc);
   __free_stlst(frp->forbody);
   __my_free((char *) frp, sizeof(struct for_t));
   break;
  case S_DELCTRL: free_dctrl(stp->st.sdc, TRUE); break;
  case S_UNBLK:
   __free_stlst(stp->st.sbsts);
   break;
  case S_UNFJ:
   for (fji = 0;; fji++)
    {
     if ((fjstp = stp->st.fj.fjstps[fji]) == NULL) break;
     __free_stlst(fjstp); 
    }
   __my_free((char *) stp->st.fj.fjstps, (fji + 1)*sizeof(struct st_t *)); 
   __my_free((char *) stp->st.fj.fjlabs, (fji + 1)*sizeof(int32)); 
   break;
  case S_TSKCALL:
   __free_xtree(stp->st.stkc.targs);
   /* cannot free symbol */
   /* only interactive can be freed so cannot have tfrec */
   if (stp->st.stkc.tkcaux.trec != NULL) __misc_terr(__FILE__, __LINE__);
   break;
  case S_QCONTA:
   qcafs = stp->st.sqca;
   /* notice for assign already freed */
   __free_xtree(qcafs->qclhsx);
   __free_xtree(qcafs->qcrhsx);
   __my_free((char *) qcafs, sizeof(struct qconta_t));
   break;
  case S_QCONTDEA:
   __free_xtree(stp->st.sqcdea.qcdalhs);
   break;
  case S_CAUSE: break;
  case S_DSABLE:
   __free_xtree(stp->st.sdsable.dsablx);
   break;
  /* statement added for execution */
  case S_REPSETUP:
   /* union field unused */
   stp->st.scausx = NULL;
   break;
  case S_REPDCSETUP:
   /* union field unused */
   stp->st.scausx = NULL;
   break;
  case S_GOTO: break;
  /* notice named block non freeable (at least for now) */
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * free a delay control record
 */
static void free_dctrl(struct delctrl_t *dctp, int32 free_action)
{
 /* free the delay value or expr. for event controls */
 __free_del(dctp->dc_du, dctp->dc_delrep, __inst_mod->flatinum);

 /* SJM - 08/03/01 - also free repeat cout expr if present */
 if (dctp->repcntx != NULL) __free_xtree(dctp->repcntx);

 /* for delay not event control, will not have scheduled tevs */
 if (dctp->dceschd_tevs != NULL)
  __my_free((char *) dctp->dceschd_tevs,
   __inst_mod->flatinum*sizeof(struct tev_t *));
 dctp->dceschd_tevs = NULL;
 if (free_action) __free_stlst(dctp->actionst);
 __my_free((char *) dctp, sizeof(struct delctrl_t));
}

/*
 * free case item list
 */
static void free_csitemlst(register struct csitem_t *csip)
{
 struct csitem_t *csip2;

 for (;csip != NULL;)
  {
   csip2 = csip->csinxt;
   /* nil expr list always nil for default */
   if (csip->csixlst != NULL) __free_xprlst(csip->csixlst);
   /* if no default, stmt of first nil */
   if (csip->csist != NULL) __free_stlst(csip->csist);
   __my_free((char *) csip, sizeof(struct csitem_t));
   csip = csip2;
  }
}

/*
 * free a list of statements - i.e. next fields connect to make block list
 */
extern void __free_stlst(register struct st_t *stp)
{
 register struct st_t *stp2;

 for (; stp != NULL;) { stp2 = stp->stnxt; __free_1stmt(stp); stp = stp2; }
}

/*
 * free an expression list
 */
extern void __free_xprlst(struct exprlst_t *xplp)
{
 register struct exprlst_t *xplp2;

 for (; xplp != NULL;)
  {
   xplp2 = xplp->xpnxt;
   __free_xtree(xplp->xp);
   __my_free((char *) xplp, sizeof(struct exprlst_t));
   xplp = xplp2;
  }
}

/*
 * ROUTINES TO TRANFORM INTERNAL NET LIST FOR COMPILATION 
 */

/*
 * process all net list data structures tranforming for compilation
 */
extern void __xform_nl_to_modtabs(void)
{
 struct mod_t *mdp;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   /* allocate table filled with all exprs in module */
   if (__inst_mod->mexprnum == 0) __inst_mod->mexprtab = NULL;
   else
    {
     __inst_mod->mexprtab = (struct expr_t *)
      __my_malloc(__inst_mod->mexprnum*sizeof(struct expr_t));
    }
   __last_modxi = -1;

   if (__inst_mod->mstnum == 0) __inst_mod->msttab = NULL;
   else
    {
     __inst_mod->msttab = (struct st_t *)
      __my_malloc(__inst_mod->mstnum*sizeof(struct st_t));
    }
   __last_modsti = -1;
   
   cmp_xform_ports();
   cmp_xform_ialst();
   cmp_xform_inst_conns();
   cmp_xform_gates();
   cmp_xform_contas();
   cmp_xform_tasks();
 
   if (__inst_mod->mspfy != NULL) cmp_xform_specify();

   /* LOOKATME - do analog block statement and expressions need to be */
   /* xformed since not accessed in digital sim, think not */

   /* many expressions (say from xmrs) not copied since not needed during */ 
   /* exec - check and adjust mod expr no. here */
   if (__last_modxi + 1 > __inst_mod->mexprnum)
    __misc_terr(__FILE__, __LINE__);
   __inst_mod->mexprnum = __last_modxi + 1;

   /* DBG remove --
   if (__last_modsti + 1 != __inst_mod->mstnum)
    __misc_terr(__FILE__, __LINE__);
   --- */
   __pop_wrkitstk();

/* DBG remove ---
   if (__debug_flg)
    {
     __dmp_exprtab(mdp, mdp->mexprnum);
     __dmp_msttab(mdp, mdp->mstnum);
    }
--- */
  }
}

/*
 * compile transform all port lists for one module 
 */
static void cmp_xform_ports(void)
{
 register int32 pi, pnum;
 register struct mod_pin_t *mpp;

 mpp = &(__inst_mod->mpins[0]);  
 pnum = __inst_mod->mpnum;
 for (pi = 0; pi < pnum; pi++, mpp++)
  {
   mpp->mpref = mv1_expr_totab(mpp->mpref);
  }
}

/*
 * copy a statement list (linked list of statements)
 */
static struct st_t *cmp_xform_lstofsts(register struct st_t *ostp)
{
 register struct st_t *nstp_hdr, *nstp, *last_nstp, *ostp2;

 nstp_hdr = NULL;
 for (last_nstp = NULL; ostp != NULL;)
  {
   nstp = cmp_xform1_stmt(ostp, last_nstp);
   /* must not free internal parts of old statement */

   if (last_nstp == NULL) nstp_hdr = nstp; else last_nstp->stnxt = nstp;
   nstp->stnxt = NULL;
   last_nstp = nstp;
   ostp2 = ostp->stnxt;
   __my_free((char *) ostp, sizeof(struct st_t));
   ostp = ostp2;
  }
 return(nstp_hdr);
}

/*
 * compile transform one statement
 */
static struct st_t *cmp_xform1_stmt(register struct st_t *ostp,
 struct st_t *last_stp)
{
 register struct st_t *nstp;
 int32 fji;
 struct sy_t *syp; 
 struct systsk_t *stbp;
 struct st_t *fjstp;

/* DBG remove --
 if (__debug_flg)
  {
-* --
   __dbg_msg("%04d: AT %s %s - STMT XFORM (%s)\n", ostp->stalloc_ndx,
    __bld_lineloc(__xs, ostp->stfnam_ind, ostp->stlin_cnt),
    __inst_mod->msym->synam, __to_sttyp(__xs2, ostp->stmttyp));
-- *-
-* -- *-
   __dbg_msg("AT %s %s - STMT XFORM %04d (%s)\n",
    __bld_lineloc(__xs, ostp->stfnam_ind, ostp->stlin_cnt),
    __inst_mod->msym->synam, __last_modsti + 1,
    __to_sttyp(__xs2, ostp->stmttyp));
-* -- *-
  }
--- */

 if (ostp == NULL) return(NULL);

 /* DBG remove -- */
 if (__last_modsti + 2 > __inst_mod->mstnum) __misc_terr(__FILE__, __LINE__);
 /* ---*/

 nstp = &(__inst_mod->msttab[++__last_modsti]);

 /* because union for common case now in statement, gets copied here */
 *nstp = *ostp;
 nstp->stnxt = NULL;

 switch ((byte) ostp->stmttyp) {
  /* null just has type value and NULL pointer (i.e. ; by itself) */
  case S_NULL: case S_STNONE: break;
  /* SJM 09/24/01 - for fj, must not make for assign separate stmt */
  case S_PROCA: case S_RHSDEPROCA: case S_NBPROCA: case S_FORASSGN:
   nstp->st.spra.lhsx = mv1_expr_totab(ostp->st.spra.lhsx);
   nstp->st.spra.rhsx = mv1_expr_totab(ostp->st.spra.rhsx);
   break;
  case S_IF:
   nstp->st.sif.condx = mv1_expr_totab(ostp->st.sif.condx);
   nstp->st.sif.thenst = cmp_xform_lstofsts(ostp->st.sif.thenst);
   nstp->st.sif.elsest = cmp_xform_lstofsts(ostp->st.sif.elsest);
   break;
  case S_CASE:
   nstp->st.scs.castyp = ostp->st.scs.castyp;
   nstp->st.scs.maxselwid = ostp->st.scs.maxselwid;
   nstp->st.scs.csx = mv1_expr_totab(ostp->st.scs.csx);

   /* if case has no default, st and expr list fields nil */ 
   /* no copy but must transform st lists and expr inside */
   nstp->st.scs.csitems = ostp->st.scs.csitems;
   cmp_xform_csitemlst(nstp->st.scs.csitems);
   break;
  case S_REPEAT:
   nstp->st.srpt.repx = mv1_expr_totab(ostp->st.srpt.repx);
   /* move ptr to new, old freed so no wrong cross links */
   nstp->st.srpt.reptemp = ostp->st.srpt.reptemp;
   nstp->st.srpt.repst = cmp_xform_lstofsts(ostp->st.srpt.repst);
   break;
  case S_REPSETUP:
   /* no contents just indicator for setting up next actual rep stmt */
   break;
 
  case S_FOREVER: case S_WHILE:
   nstp->st.swh.lpx = mv1_expr_totab(ostp->st.swh.lpx);
   nstp->st.swh.lpst = cmp_xform_lstofsts(ostp->st.swh.lpst);
   break;
  case S_WAIT:
   nstp->st.swait.lpx = mv1_expr_totab(ostp->st.swait.lpx);
   nstp->st.swait.lpst = cmp_xform_lstofsts(ostp->st.swait.lpst);
   /* here just move pointer since needed by new statement in table */
   /* LOOKATME - is this already copied */
   nstp->st.swait.wait_dctp = ostp->st.swait.wait_dctp;
   /* also point dctp statement to new one */
   nstp->st.swait.wait_dctp->actionst = nstp->st.swait.lpst;
   break;
  case S_FOR:
   /* DBG remove --- */
   if (last_stp == NULL) __misc_terr(__FILE__, __LINE__);
   if (last_stp->stmttyp != S_FORASSGN) __misc_terr(__FILE__, __LINE__);
   /* --- */
   /* notice using already allocated storage - assumes dynamic dlopen so */
   /* no need to save and copy storage to runtime */
   nstp->st.sfor = ostp->st.sfor;
   nstp->st.sfor->forassgn = last_stp;
   /* still need to xform expressions */
   nstp->st.sfor->fortermx = mv1_expr_totab(ostp->st.sfor->fortermx);

   /* for inc xformed in here */
   nstp->st.sfor->forbody = cmp_xform_lstofsts(ostp->st.sfor->forbody);
   
   {
    struct st_t *stp2, *last_stp2, *last_stp3;

    stp2 = nstp->st.sfor->forbody;
    for (last_stp2 = last_stp3 = NULL; stp2 != NULL; stp2 = stp2->stnxt)
     {
      last_stp3 = last_stp2;
      last_stp2 = stp2;
     }
    /* DBG remove --- */
    if (last_stp3->stmttyp != S_PROCA) __misc_terr(__FILE__, __LINE__);
    /* --- */
    nstp->st.sfor->forinc = last_stp3;
   }
   break;
  case S_REPDCSETUP:
   /* no contents just indicator for setting up next actual rep stmt */
   break;
  case S_DELCTRL:
   /* stmt contents do not fit in record so link old ptr to new */ 
   nstp->st.sdc = ostp->st.sdc;
   /* here fix but no need to copy dellst */
   cmp_xform_delay(nstp->st.sdc->dc_delrep, nstp->st.sdc->dc_du);
   nstp->st.sdc->actionst = cmp_xform_lstofsts(ostp->st.sdc->actionst);
   break;
  case S_NAMBLK:
   nstp->st.snbtsk->tsksyp->el.etskp->st_namblkin = nstp;
   /* for named block, no continuation - must be subthread except in func */
   nstp->st.snbtsk->tskst = cmp_xform_lstofsts(ostp->st.snbtsk->tskst);
   break;
  case S_UNBLK:
   nstp->st.sbsts = cmp_xform_lstofsts(ostp->st.sbsts);
   break;
  case S_UNFJ:
   for (fji = 0;; fji++)
    {
     if ((fjstp = ostp->st.fj.fjstps[fji]) == NULL) break;
     nstp->st.fj.fjstps[fji] = cmp_xform_lstofsts(fjstp);
    }
   break;
  case S_TSKCALL:
   /* find new task through old to new symbol */
   /* for system tasks since points to itself gets same (right) sym */
   nstp->st.stkc.tsksyx = mv1_expr_totab(ostp->st.stkc.tsksyx);
   nstp->st.stkc.targs = mv1_expr_totab(ostp->st.stkc.targs);
   /* task itself xforms in xform task routine */

   /* need to fixup PLI tf systf cross linked tf recs only - not vpi */
   syp = nstp->st.stkc.tsksyx->lu.sy;
   if (syp->sytyp == SYM_STSK)
    {
     stbp = syp->el.esytbp;
     /* only way to tell if tf_ is by range of tsk veriusertf index */
     if (stbp->stsknum >= BASE_VERIUSERTFS
      && (int32) stbp->stsknum <= __last_veriusertf)
      {
       xform_tf_syst_enable(nstp);
      }
    }
   /* for vpi registered and built-in system tasks nothing to do */  
   /* for vpi systasks accessing vpi_ systf has index and values */
   /* needed in vpi_ t vpi systf data record */
   break;
  case S_QCONTA:
   /* SJM 02/23/02 - now malloc aux info, but can still use - no new malloc */
   nstp->st.sqca->qcatyp = ostp->st.sqca->qcatyp;
   nstp->st.sqca->regform = ostp->st.sqca->regform;
   nstp->st.sqca->qclhsx = mv1_expr_totab(ostp->st.sqca->qclhsx);
   nstp->st.sqca->qcrhsx = mv1_expr_totab(ostp->st.sqca->qcrhsx);
   nstp->st.sqca->rhs_qcdlstlst = ostp->st.sqca->rhs_qcdlstlst;
   break;
  case S_QCONTDEA:
   nstp->st.sqcdea.qcdatyp = ostp->st.sqcdea.qcdatyp;
   nstp->st.sqcdea.regform = ostp->st.sqcdea.regform;
   nstp->st.sqcdea.qcdalhs = mv1_expr_totab(ostp->st.sqcdea.qcdalhs);
   break;
  case S_CAUSE:
   /* must copy expr. even though know just event name */
   nstp->st.scausx = mv1_expr_totab(ostp->st.scausx);
   break;
  case S_DSABLE:
   nstp->st.sdsable.dsablx = mv1_expr_totab(ostp->st.sdsable.dsablx);
   break;
  case S_GOTO:
   /* must 0 out dest. since fixed later with right target */
   nstp->st.sgoto = NULL;
   break;
  /* LOOKATME - need name resolving statement type no? */
  default: __case_terr(__FILE__, __LINE__);
 }
 return(nstp);
}

/*
 * xform tf_ style sys task enable by updating tf rec 
 *
 * not needed for vpi style syst enables because compiletf already called
 *
 * LOOKATME - could pass systf ptrs since already computed
 */
static void xform_tf_syst_enable(struct st_t *nstp)
{
 register int32 ai;
 struct tskcall_t *tkcp;
 struct tfrec_t *tfrp;
 struct expr_t *argxp;
 struct tfarg_t *tfap;

 tkcp = &(nstp->st.stkc);
 /* DBG remove --- */
 if (tkcp->tkcaux.trec == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */
 /* tsk call record contains tfrec for PLI 1.0 registered */
 tfrp = tkcp->tkcaux.trec;

 /* back link tfrec for systask enable to new moved stmt */
 tfrp->tfu.tfstp = nstp;
 /* notice for tsk, targs 1st left has actual argument - no need for next */
 argxp = tkcp->targs;
 /* update all the tfarg expr ptrs to moved expr */
 for (ai = 1; ai < tfrp->tfanump1; ai++, argxp = argxp->ru.x)
  {
   /* DBG remove -- */
   if (argxp == NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */

   tfap = &(tfrp->tfargs[ai]);
   tfap->arg.axp = argxp->lu.x;
  }
}

/*
 * ROUTINES TO XFORM (MOVE TO INDEXABLE TABLE) EXPRESSIONS
 */
/*
 * routine to move one leaf or non leaf expression into table
 */
static struct expr_t *mv1_expr_totab(struct expr_t *oxp)
{ 
 struct expr_t *nxp;
 struct sy_t *syp;
 struct sysfunc_t *sfbp;

 if (oxp == NULL) return(NULL);

 /* DBG remove -- */
 if (__last_modxi + 2 > __inst_mod->mexprnum) __misc_terr(__FILE__, __LINE__);
 /* ---*/

 nxp = &(__inst_mod->mexprtab[++__last_modxi]);
 switch ((byte) oxp->optyp) { 
  case ID: case OPEMPTY:
   /* op empty works since both lu and ru are nil */
   *nxp = *oxp;
   /* notice symbol name in symbol table so ptr can be subtracted from */
   /* symbol table base */
   break;
  case GLBREF:
   /* for global ru is gref and lu is symbol in target mod symbol table */
   /* know ru.grp always in module and already built as table */
   *nxp = *oxp;
   nxp->ru.grp->gxndp = nxp;
   break;
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM:
   /* know constant table already built so can just copy */ 
   *nxp = *oxp;
   break;
  case FCALL:
   syp = oxp->lu.x->lu.sy;
   *nxp = *oxp;
   /* start by copying call expr. */
   if (oxp->lu.x != NULL) nxp->lu.x = mv1_expr_totab(oxp->lu.x);
   if (oxp->ru.x != NULL) nxp->ru.x = mv1_expr_totab(oxp->ru.x);

   /* need to fixup PLI systf cross linked records when change repl expr */
   if (syp->sytyp == SYM_SF)
    {
     sfbp = syp->el.esyftbp;
     if (sfbp->tftyp == SYSF_TF) xform_tf_sysf_call(nxp);
    }
   /* for vpi registered and built-in system function nothing to do */  
   /* for vpi sysfuncs accessing vpi_ sysfunc t has index and values */
   /* needed in vpi_ t vpi systf data record */
   break;
  case TOK_NONE: case BADOBJ: case LITSTR:
   __misc_terr(__FILE__, __LINE__); break;
  default:
   *nxp = *oxp;
   if (oxp->lu.x != NULL) nxp->lu.x = mv1_expr_totab(oxp->lu.x);
   if (oxp->ru.x != NULL) nxp->ru.x = mv1_expr_totab(oxp->ru.x);
 }
 /* last step - contained in expr. link in parents right dir ptr */
 __my_free((char *) oxp, sizeof(struct expr_t)); 
 return(nxp);
}

/*
 * xform tf_ style sysf call by updating sysf call record
 *
 * not needed for vpi style sysf calls because compiletf already called
 */
static void xform_tf_sysf_call(struct expr_t *nxp)
{
 register int32 ai;
 struct sy_t *syp;
 struct sysfunc_t *sfbp;
 struct tfrec_t *tfrp;
 struct expr_t *argxp;
 struct tfarg_t *tfap;

 syp = nxp->lu.x->lu.sy;
 sfbp = syp->el.esyftbp;

 /* DBG remove --- */
 if (sfbp->syfnum < BASE_VERIUSERTFS
  || sfbp->syfnum > __last_veriusertf) __misc_terr(__FILE__, __LINE__);
 /* --- */
 /* szu field used for ptr to the tfrec - nothing to set in new expr*/
 tfrp = nxp->lu.x->szu.xfrec;
 /* back link tfrec for sysf call to new moved expr */
 tfrp->tfu.callx = nxp;
 /* update all the tfarg expr ptrs to moved expr */
 argxp = nxp->ru.x;
 for (ai = 1; ai < tfrp->tfanump1; ai++, argxp = argxp->ru.x)
  {
   /* DBG remove -- */
   if (argxp == NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */

   tfap = &(tfrp->tfargs[ai]);
   tfap->arg.axp = argxp->lu.x;
  }
}

/*
 * compile transform list of case items
 *
 * no need to copy but must replace statement lists and exprs inside
 */
static void cmp_xform_csitemlst(register struct csitem_t *csip)
{
 register struct exprlst_t *xplp;

 for (; csip != NULL; csip = csip->csinxt)
  {
   if (csip->csist != NULL) csip->csist = cmp_xform_lstofsts(csip->csist);
   /* know for first default, selector x list nil */
   for (xplp = csip->csixlst; xplp != NULL; xplp = xplp->xpnxt)
    {
     xplp->xp = mv1_expr_totab(xplp->xp);
    }
  }
}

/*
 * must replace expressions in case dellst one of the expr forms
 *
 * by here delays already converted to one of 16 forms
 */
static void cmp_xform_delay(int32 drep, union del_u du)
{
 struct expr_t *tmpxp;

 switch ((byte) drep) {
  /* nothing to do for the delay value table forms */
  case DT_NONE:
  case DT_1V: case DT_IS1V: case DT_IS1V1: case DT_IS1V2: case DT_4V:
  case DT_IS4V: case DT_IS4V1: case DT_IS4V2: case DT_16V: case DT_IS16V:
  case DT_IS16V1: case DT_IS16V2:
   break;
  case DT_1X:
   /* since must copy from something that gets freed, must first */
   /* copy expr to tmp that is then immediately freed by expr mv */
   tmpxp = __sim_copy_expr(du.d1x);
   du.d1x = mv1_expr_totab(tmpxp);
   break;
  case DT_4X:
   if (du.d4x[0] != NULL)
    { tmpxp = __sim_copy_expr(du.d4x[0]); du.d4x[0] = mv1_expr_totab(tmpxp); }
   if (du.d4x[1] != NULL)
    { tmpxp = __sim_copy_expr(du.d4x[1]); du.d4x[1] = mv1_expr_totab(tmpxp); }
   if (du.d4x[2] != NULL)
    { tmpxp = __sim_copy_expr(du.d4x[2]); du.d4x[2] = mv1_expr_totab(tmpxp); }
   if (du.d4x[3] != NULL)
    { tmpxp = __sim_copy_expr(du.d4x[3]); du.d4x[3] = mv1_expr_totab(tmpxp); }
   break;
  case DT_CMPLST:
   /* LOOKATME - is this legal */
   __misc_terr(__FILE__, __LINE__);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * GOTO FIXUP ROUTINES
 */

/*
 * compile transform all initial/always statement lists
 */
static void cmp_xform_ialst(void)
{
 register struct ialst_t *ialp;

 __processing_func = FALSE;
 for (ialp = __inst_mod->ialst; ialp != NULL; ialp = ialp->ialnxt)
  {
   ialp->iastp = cmp_xform_lstofsts(ialp->iastp);
  } 
 __prpsti = 0;
 __nbsti = -1;
 __prpstk[0] = NULL;
 for (ialp = __inst_mod->ialst; ialp != NULL; ialp = ialp->ialnxt)
  {
   if (ialp->iatyp == ALWAYS)
    {
     cxf_fixup_loopend_goto(ialp->iastp, ialp->iastp);
    }
   cxf_fixup_lstofsts_gotos(ialp->iastp, TRUE);

   /* DBG remove --- */ 
   if (__prpsti != 0) __misc_terr(__FILE__, __LINE__); 
   /* --- */
  } 
}

/*
 * fixup loop end gotos
 */
static void cxf_fixup_loopend_goto(struct st_t *begstp, struct st_t *targstp)
{
 register struct st_t *stp; 
 struct st_t *last_stp;

 /* DBG remove --- */
 if (begstp == NULL) __arg_terr(__FILE__, __LINE__);
 /* --- */

 /* find last statement in loop - know has at least one */
 for (stp = begstp, last_stp = NULL; stp != NULL; stp = stp->stnxt)
  last_stp = stp;
 /* DBG remove --- */
 if (last_stp == NULL) __arg_terr(__FILE__, __LINE__);
 if (last_stp->stmttyp != S_GOTO) __arg_terr(__FILE__, __LINE__);
 if (!last_stp->lpend_goto) __arg_terr(__FILE__, __LINE__);
 if (targstp == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */
 last_stp->st.sgoto = targstp;

 if (__debug_flg)
  {
   char s1[RECLEN], s2[RECLEN], s3[RECLEN];

   __dbg_msg("++ loop: xform goto after %s at %s back to stmt %s at %s\n",
    __to_sttyp(s1, last_stp->stmttyp), __bld_lineloc(__xs, 
    last_stp->stfnam_ind, last_stp->stlin_cnt), __to_sttyp(s2,
    begstp->stmttyp), __bld_lineloc(s3, targstp->stfnam_ind,
    targstp->stlin_cnt));
  }
}

/*
 * fixup all gotos in list of stmts
 *
 * know all statement transformed - sets correct new goto dest.
 */
static void cxf_fixup_lstofsts_gotos(struct st_t *hdrstp, int32 has_endgoto)
{
 register struct st_t *stp;
 int32 fji;
 struct for_t *forp;
 struct st_t *stp2, *fjstp;

 for (stp = hdrstp; stp != NULL; stp = stp->stnxt)
  {
   __sfnam_ind = stp->stfnam_ind;
   __slin_cnt = stp->stlin_cnt;

   switch ((byte) stp->stmttyp) {
    case S_PROCA: case S_FORASSGN: case S_RHSDEPROCA: case S_NBPROCA: break;
    case S_IF:
     if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);
     cxf_fixup_lstofsts_gotos(stp->st.sif.thenst, TRUE);
     if (stp->st.sif.elsest != NULL)
      cxf_fixup_lstofsts_gotos(stp->st.sif.elsest, TRUE);
     if (stp->stnxt != NULL) pop_prpstmt();
     break;
    case S_CASE:
     cxf_fixup_case_gotos(stp);
     break;
    case S_FOR:
     forp = stp->st.sfor;
     /* LOOKATME - is prep change needed */
     /* goto to for itself */
     cxf_fixup_loopend_goto(forp->forbody, stp);
     cxf_fixup_lstofsts_gotos(forp->forbody, FALSE);
     break;
    case S_FOREVER: case S_WHILE:
     cxf_fixup_loopend_goto(stp->st.swh.lpst, stp);
     cxf_fixup_lstofsts_gotos(stp->st.swh.lpst, FALSE);
     break;
    case S_REPEAT:
     /* allocate per inst. count storage */ 
     /* add loop back to repeat header */
     cxf_fixup_loopend_goto(stp->st.srpt.repst, stp);
     cxf_fixup_lstofsts_gotos(stp->st.srpt.repst, FALSE);
     break;
    case S_WAIT:
     if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);
     cxf_fixup_lstofsts_gotos(stp->st.swait.lpst, TRUE);
     if (stp->stnxt != NULL) pop_prpstmt();
     break;
    case S_DELCTRL:
     /* first find end of decltrl chain */
     /* if no actoin statement - nothing to do */
     if (stp->st.sdc->actionst == NULL) break; 
     for (stp2 = stp->st.sdc->actionst;; stp2 = stp2->st.sdc->actionst)
      {
       /* keep going until delay control has no action statement or */
       /* a non delay control action statement */
       /* case "#10 begin #20 ..." - is not delay control chain */
       if (stp2 == NULL || stp2->stmttyp != S_DELCTRL || stp2->st_unbhead)
        break;
      }
     /* ??? 11/10/99 -  if (stp2 == NULL || stp2->stmttyp != S_GOTO) break; */
     if (stp2 == NULL) break;
     /* do the goto fix up */
     if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);
     cxf_fixup_lstofsts_gotos(stp2, TRUE);
     /* only end of chain can have "real" statement */
     if (stp->stnxt != NULL) pop_prpstmt();
     break;
    case S_NAMBLK:
     /* for named block, no continuation - must be subthread except in func */
     __push_nbstk(stp);
     if (__processing_func)
      { 
       /* for function no task scheduled - run inline to need goto to cont */
       /* at end */
       if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);
       cxf_fixup_lstofsts_gotos(stp->st.snbtsk->tskst, TRUE);
       if (stp->stnxt != NULL) pop_prpstmt();
      }
     else
      {
       push_prpstmt((struct st_t *) NULL);
       cxf_fixup_lstofsts_gotos(stp->st.snbtsk->tskst, FALSE);
       pop_prpstmt();
      }
     __pop_nbstk();
     break;
    case S_UNBLK:
     if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);
     /* need continuation for simple block */
     cxf_fixup_lstofsts_gotos(stp->st.sbsts, TRUE);
     if (stp->stnxt != NULL) pop_prpstmt();
     break;
    case S_UNFJ:
     push_prpstmt((struct st_t *) NULL);
     /* continuation inside these must be NULL, not stacked val */
     for (fji = 0;; fji++)
      {
       if ((fjstp = stp->st.fj.fjstps[fji]) == NULL) break;
       cxf_fixup_lstofsts_gotos(fjstp, FALSE);
      }
     pop_prpstmt();
     break;
    case S_DSABLE:
     /* inside function disables are gotos to next statement in up block */
     if (__processing_func) cxf_fixup_func_dsabl_gotos(stp);
     /* any other disable is task scheduled */
     break;
    case S_GOTO:
     /* goto can only be last */
     /* DBG remove --- */
     if (stp->stnxt != NULL) __misc_terr(__FILE__, __LINE__);
     /* --- */
     /* if loopend dest. set and nothing to do here */
     if (stp->lpend_goto) return;
     if (has_endgoto && __prpstk[__prpsti] != NULL) 
      {
       stp->st.sgoto = __prpstk[__prpsti];
       /* in this rare case will not have line number - can it happen */
      }
     /* DBG remove -- */
     else __case_terr(__FILE__, __LINE__); 
     /* -- */
     return;
    default:
     /* statements with no processing ignored thru here */
     break;
   }
  }
 /* block with no loop end goto returns thru here */ 
}

/*
 * fixup all gotos in case stmt
 */
static void cxf_fixup_case_gotos(struct st_t *stp)
{
 register struct csitem_t *csip;
 struct csitem_t *dflt_csip;

 if (stp->stnxt != NULL) push_prpstmt(stp->stnxt);

 dflt_csip = stp->st.scs.csitems;
 /* this will move up stack to add goto after ending stp */
 for (csip = dflt_csip->csinxt; csip != NULL; csip = csip->csinxt)
  {
   cxf_fixup_lstofsts_gotos(csip->csist, TRUE);
  }

 /* this will move up stack to connect ending stnxt to next exec. place */
 if (dflt_csip->csist != NULL)
  cxf_fixup_lstofsts_gotos(dflt_csip->csist, TRUE);
 if (stp->stnxt != NULL) pop_prpstmt();
}

/*
 *
 * inside function disable are like c continue and are just gotos
 */
static void cxf_fixup_func_dsabl_gotos(struct st_t *stp)
{
 register int32 i;
 struct expr_t *dsxp;
 struct sy_t *syp;
 struct task_t *dsatskp;

 dsxp = stp->st.sdsable.dsablx;
 syp = dsxp->lu.sy; 
 /* disable of func. indicated by nil next statmenet - use fcall stack */
 /* system function disable no next */
 if (syp->sytyp == SYM_F) return;

 dsatskp = syp->el.etskp;
 /* know every named block when entered in function name block is stacked */
 for (i = __nbsti; i >= 0; i--)
  {
   if (__nbstk[i]->st.snbtsk == dsatskp)
    {
     /* this can be nil */
     stp->st.sdsable.func_nxtstp = __nbstk[i]->stnxt;
     return;
    }
  }
 /* know always enclosing, or will not get here - earlier error */
 __case_terr(__FILE__, __LINE__);
}

/*
 * compile transform instance connections
 */
static void cmp_xform_inst_conns(void)
{
 register int32 ii, pi;
 register struct inst_t *ip;
 int32 pnum;

 if (__inst_mod->minum == 0) return;

 for (ii = 0; ii < __inst_mod->minum; ii++)
  {
   ip = &(__inst_mod->minsts[ii]);
   pnum = ip->imsym->el.emdp->mpnum;
   for (pi = 0; pi < pnum; pi++)
    ip->ipins[pi] = mv1_expr_totab(ip->ipins[pi]);
  }
}

/*
 * compile transform gates
 */
static void cmp_xform_gates(void)
{
 register int32 gi, pi;
 register struct gate_t *gp;
 int32 pnum;
 
 for (gi = 0; gi < __inst_mod->mgnum; gi++)
  {
   gp = &(__inst_mod->mgates[gi]);
   /* fixex (moves expr)  only delay expression forms */
   cmp_xform_delay(gp->g_delrep, gp->g_du);
   pnum = gp->gpnum;
   for (pi = 0; pi < pnum; pi++)
    gp->gpins[pi] = mv1_expr_totab(gp->gpins[pi]);
  }
}

/*
 * compile transform cont assigns
 */
static void cmp_xform_contas(void)
{
 register int32 bi;
 register struct conta_t *cap, *pbcap; 
 int32 cai;

 for (cap = &(__inst_mod->mcas[0]), cai = 0; cai < __inst_mod->mcanum;
  cai++, cap++)
  {
   /* fixex (moves expr)  only delay expression forms */
   cmp_xform_delay(cap->ca_delrep, cap->ca_du);

   cap->lhsx = mv1_expr_totab(cap->lhsx);
   cap->rhsx = mv1_expr_totab(cap->rhsx);
   if (cap->ca_pb_sim) 
    {
     for (bi = 0; bi < cap->lhsx->szu.xclen; bi++)
      {
       pbcap = &(cap->pbcau.pbcaps[bi]);
       pbcap->lhsx = mv1_expr_totab(pbcap->lhsx);
       pbcap->rhsx = mv1_expr_totab(pbcap->rhsx);
      }
    }
  }
}

/*
 * compile transform tasks
 *
 * this also handle tasks - task specific task field (one for now)
 * set when statemetns tranformed
 *
 * task args do not have expr. forms
 */
static void cmp_xform_tasks(void)
{
 register struct task_t *tskp;

 /* finally check user tasks and function statements */
 for (tskp = __inst_mod->mtasks; tskp != NULL; tskp = tskp->tsknxt)
  {
   /* named blocks handled as statement where they occur */
   if (tskp->tsktyp == FUNCTION) __processing_func = TRUE;
   else if (tskp->tsktyp == TASK) __processing_func = FALSE; 
   else continue;
   /* DBG remove -- */
   if (__debug_flg)
    {
     __dbg_msg("+++ xforming task %s\n", tskp->tsksyp->synam); 
    }
   /* --- */

   tskp->tskst = cmp_xform_lstofsts(tskp->tskst);
  }
 __prpsti = 0;
 __nbsti = -1;
 __prpstk[0] = NULL;
 for (tskp = __inst_mod->mtasks; tskp != NULL; tskp = tskp->tsknxt)
  {
   /* named blocks handled as statement where they occur */
   if (tskp->tsktyp == FUNCTION) __processing_func = TRUE;
   else if (tskp->tsktyp == TASK) __processing_func = FALSE; 
   else continue;

   cxf_fixup_lstofsts_gotos(tskp->tskst, FALSE);
  } 
 /* DBG remove --- */ 
 if (__prpsti != 0) __misc_terr(__FILE__, __LINE__); 
 /* --- */
 /* AIV 10/20/05 - must reset __processing_func to FALSE */
 __processing_func = FALSE;
}

/*
 * compile transform specify sections expressions
 *
 * only called if module has specify section
 * maybe do not need to copy these expressions
 *
 * notice by here pathel's no longer expressions and expr. delay forms
 * illegal in paths (error before here)
 */
static void cmp_xform_specify(void)
{
 register struct spcpth_t *pthp;
 register struct tchk_t *tcp;

 /* prepare paths */
 for (pthp = __inst_mod->mspfy->spcpths; pthp != NULL; pthp = pthp->spcpthnxt)
  {
   /* data source not really used in simulator but moved still */ 
   pthp->datasrcx = mv1_expr_totab(pthp->datasrcx);
   /* need to move this since condition eval can eliminates path */
   pthp->pthcondx = mv1_expr_totab(pthp->pthcondx);
  }
 /* copy these because needed at run time for timing check error msgs */
 for (tcp = __inst_mod->mspfy->tchks; tcp != NULL; tcp = tcp->tchknxt)
  {
   if (tcp->startxp != NULL) tcp->startxp = mv1_expr_totab(tcp->startxp);

   if (tcp->startcondx != NULL)
    tcp->startcondx = mv1_expr_totab(tcp->startcondx);

   if (tcp->chkxp != NULL) tcp->chkxp = mv1_expr_totab(tcp->chkxp);

   if (tcp->chkcondx != NULL) tcp->chkcondx = mv1_expr_totab(tcp->chkcondx);

   /* both sides of setuphold determined from hold half and both sides of*/
   /* recrem determined removal half */
   if (tcp->tc_supofsuphld || tcp->tc_recofrecrem) continue;

   /* LOOKATME - are these needed since think spec delays can't be exprs */
   cmp_xform_delay(tcp->tc_delrep, tcp->tclim_du);
   if (tcp->tc_haslim2) cmp_xform_delay(tcp->tc_delrep2, tcp->tclim2_du);
  }
}

/*
 * dump a module's statement table
 *
 * number to dump is arg so can use on partially built table
 */
extern void __dmp_msttab(struct mod_t *mdp, int32 snum)
{
 register int32 si;
 register struct st_t *stp;
 int32 si2;

 if (mdp->mstnum == 0) 
  {
   __dbg_msg("+++ module %s has no statements +++\n", mdp->msym->synam);
   return;
  }
 /* DBG remove --- */
 if (mdp->msttab == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 __dbg_msg("+++ module %s has %d statements +++\n", mdp->msym->synam,
  mdp->mstnum);
 __push_wrkitstk(mdp, 0); 
 for (si = 0; si < snum; si++)
  {
   stp = &(mdp->msttab[si]);
   if (stp->stmttyp == S_GOTO)
    {
     si2 = stp->st.sgoto - &(mdp->msttab[0]);
     __dbg_msg("GOTO STMT %d (goto %d):\n", si, si2);
    }
   else __dbg_msg("STMT %d:\n", si);
   __dmp_stmt(stdout, stp, FALSE);
   __dbg_msg("\n");
  }
 __pop_wrkitstk();
}

/*
 * dump a module's expr table
 *
 * number to dump is arg so can use on partially built table
 */
extern void __dmp_exprtab(struct mod_t *mdp, int32 xnum)
{
 register int32 xi;
 register struct expr_t *xp;
 int32 lxi, rxi;
 char s1[RECLEN];

 if (mdp->mexprnum == 0) 
  {
   __dbg_msg("+++ module %s has no expressions +++\n", mdp->msym->synam);
   return;
  }
 /* DBG remove --- */
 if (mdp->mexprtab == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 __dbg_msg("+++ module %s has %d expression nodes +++\n", mdp->msym->synam,
  mdp->mexprnum);
 __push_wrkitstk(mdp, 0); 
 for (xi = 0; xi < xnum; xi++)
  {
   xp = &(mdp->mexprtab[xi]);

   if (__isleaf(xp))
    {
     __dbg_msg("EXPR %d:  leaf %s\n", xi, bld_opname(s1, xp));
    }
   else
    {
     lxi = rxi = -1;
     if (xp->lu.x != NULL) lxi = xp->lu.x - &(mdp->mexprtab[0]);
     if (xp->ru.x != NULL) rxi = xp->ru.x - &(mdp->mexprtab[0]); 
     __dbg_msg("EXPR %d:  op %s left [%d] right [%d]\n", xi,
      bld_opname(s1, xp), lxi, rxi);
    }
  }
 __pop_wrkitstk();
}

/*
 * build name of an operator in a string
 *
 * LOOKATME - this is almost same as to_xndnam but here names in symbol
 * table and expridtab not filled
 */
static char *bld_opname(char *s, struct expr_t *ndp)
{
 int32 wlen;
 word32 *ap, *bp;
 char s1[2*IDLEN], s2[RECLEN];

 switch ((byte) ndp->optyp) {
  case ID:
   sprintf(s1, "IDENTIFIER: %s", ndp->lu.sy->synam);
   break;
   /* LOOKATME - can GLBREF occur here */
  case GLBREF:
   sprintf(s1, "XMR: %s", ndp->ru.grp->gnam);
   break;
  case NUMBER:
  case ISNUMBER:
   ap = &(__contab[ndp->ru.xvi]);
   wlen = wlen_(ndp->szu.xclen);
   bp = &ap[wlen];
   sprintf(s1, "NUMBER: %s", __xregab_tostr(s2, ap, bp, ndp->szu.xclen, ndp));
   break;
  case REALNUM: case ISREALNUM:
   /* just pass a for both here */ 
   /* LOOKATME - should just format as double */ 
   ap = &(__contab[ndp->ru.xvi]);
   sprintf(s1, "REAL: %s", __regab_tostr(s2, ap, ap, ndp->szu.xclen, BDBLE,
    FALSE));
   break;
  case OPEMPTY:
   strcpy(s, "<EMPTY>");
   return(s);
  case UNDEF:
   strcpy(s, "<EXPR END>");
   return(s);
  default:
   strcpy(s, __to_opname(ndp->optyp)); return(s);
 }
 if ((int32) strlen(s1) >= RECLEN - 1) s1[RECLEN - 1] = '\0';
 strcpy(s, s1);
 return(s);
}
