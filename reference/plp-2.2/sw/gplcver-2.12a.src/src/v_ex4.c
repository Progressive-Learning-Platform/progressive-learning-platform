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
 * run time execution routines - assigns, strength and gate evaluation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <unistd.h>

#ifdef __DBMALLOC__
#include "../malloc.h"
#endif

#include "v.h"
#include "cvmacros.h"

/* sytem stuff */
extern int32 errno;

/* local prototypes */
static void set_from_mpp_unopts(struct mod_t *, struct mod_pin_t *, int32);
static void set_from_hconn_unopts(struct mod_t *, struct inst_t *, int32,
 struct expr_t *, struct mod_pin_t *, int32);
static void dbg_unopt_msg(struct mod_t *, struct mod_pin_t *, int32, char *);
static void dbg_unopt2_msg(struct mod_t *, struct inst_t *, int32,
 struct mod_pin_t *, int32, char *);
static void std_downtomdport(register struct expr_t *,
 register struct expr_t *, struct itree_t *);
static void prt_assignedto_val(struct expr_t *, char *);
static void multfi_acc_downtomdport( register struct expr_t *,
 struct expr_t *, struct itree_t *);
static void stacc_downtomdport(register struct expr_t *,
 register struct expr_t *, struct itree_t *);
static void stbsel_acc_downtomdport(register struct expr_t *,
 register struct expr_t *, struct itree_t *);
static void acc_downtomdport(register struct expr_t *,
 register struct expr_t *, struct itree_t *);
static void bsel_acc_downtomdport(register struct expr_t *,
 register struct expr_t *, struct itree_t *);
static void std_uptoiconn(register struct expr_t *, register struct expr_t *,
 struct itree_t *);
static void multfi_acc_uptoiconn(register struct expr_t *, struct expr_t *,
 struct itree_t *);
static void stacc_uptoiconn(register struct expr_t *, register struct expr_t *,
 struct itree_t *);
static void stbsel_acc_uptoiconn(register struct expr_t *,
 register struct expr_t *, struct itree_t *);
static void acc_uptoiconn(register struct expr_t *, register struct expr_t *,
 struct itree_t *);
static void bsel_acc_uptoiconn(register struct expr_t *,
 register struct expr_t *, struct itree_t *);
static void ldcomb_driver(struct xstk_t *, struct net_t *,
 register struct net_pin_t *);
static word32 widegate_ld_bit(word32 *, int32, int32);
static void ldcomb_stdriver(register byte *, struct net_t *,
 register struct net_pin_t *);
static struct xstk_t *init_stwire_accum(struct net_t *);
static struct xstk_t *ld_stgate_driver(struct net_pin_t *);
static struct xstk_t *ld_stconta_driver(struct net_pin_t *);
static struct xstk_t *ld_stvpiputv_driver(struct net_pin_t *);
static struct xstk_t *ld_sticonn_up_driver(register struct net_pin_t *);
static struct xstk_t *ld_pb_sticonn_up_driver(register struct net_pin_t *);
static struct xstk_t *ld_stmodport_down_driver(register struct net_pin_t *);
static struct xstk_t *ld_pb_stmodport_down_driver(
register struct net_pin_t *);
static struct xstk_t *ld_stpull_driver(struct net_pin_t *);
static void ndst_eval2_xpr(register byte *, register struct expr_t *);
static void access_stpsel(register byte *, register struct expr_t *);
static void rhs_stconcat(register byte *, struct expr_t *);
static void eval_stwire(word32, register byte *, int32, int32,
 register byte *);
static void adds_evgate_ins(word32 *, word32 *, int32);
static void show2_allvars(struct itree_t *);
static void emit1_driver(struct net_t *, struct net_pin_t *, int32);
static char *drive_tostr(char *, word32 *, word32 *, struct net_pin_t *,
 int32, int32);
static char *stdrive_tostr(char *, byte *, struct net_pin_t *, int32, int32);
static char *bld_wire_telltale(char *, struct net_t *);
static void emit1_load(struct net_t *, struct net_pin_t *);
static struct mdvmast_t *alloc_mdvmast(void);
static void setup_all_dvars(void);
static void setup_1argdvars(struct mdvmast_t *);
static void setup_1subtree_allvars(struct itree_t *, int32);
static void setup_1installvars(struct mod_t *, struct itree_t *);
static void turnon_1net_dmpv(struct net_t *, struct itree_t *,
 struct task_t *, struct mod_t *, int32);
static char *to_dvcode(register char *, register int32);
static void wr_1argdvhdr(struct mdvmast_t *);
static void wr_1subtree_allvars(struct itree_t *, int32);
static void wr_1inst_dvhdrs(struct itree_t *);
static void wr_tasks_dvhdrs(struct itree_t *, register struct symtab_t *);
static char *to_dvtsktyp(char *, word32);
static void wr_fromtop_iscopeto(struct itree_t *);
static void wr_totop_iscopeback(struct itree_t *);
static void wr_tskscopeto(struct symtab_t *);
static void wr_tskscopeback(struct symtab_t *);
static void wr_1vectored_dvdef(struct net_t *, char *, struct itree_t *);
static void dump_allvars_vals(void);
static void dmp_insts_ofwire(struct mod_t *, struct net_t *);
static void bld1_scal_dvval(struct net_t *, char *, struct itree_t *);
static void bld1_vec_dvval(struct net_t *, char *, struct itree_t *);
static void bld1_xdvval(register struct net_t *, char *);
static void dv_wr(int32);
static char *to_dvtimstr(char *, register word64);
static void access_stbsel(register byte *, register struct expr_t *);

/* extern prototypes (maybe defined in this module) */
extern void __set_mpp_assign_routines(void);
extern void __set_pb_mpp_assign_routines(void);
extern void __set_mpp_aoff_routines(void);
extern void __vpi_set_downtomdport_proc(struct mod_pin_t *, struct net_t *);
extern void __vpi_set_upiconnport_proc(struct mod_pin_t *);
extern void __init_instdownport_contas(struct itree_t *, struct itree_t *);
extern void __init_instupport_contas(struct itree_t *);
extern void __mdr_assign_or_sched(register struct expr_t *);
extern void __assign_1mdrwire(register struct net_t *);
extern void __sched_1mdrwire(register struct net_t *);
extern struct xstk_t *__load_mdrwire(register struct net_t *);
extern int32 __move_to_npprefloc(struct net_pin_t *);
extern struct xstk_t *__ld_wire_driver(register struct net_pin_t *);
extern struct xstk_t *__ld_tfrwarg_driver(struct net_pin_t *);
extern struct xstk_t *__ld_conta_driver(struct net_pin_t *);
extern struct xstk_t *__ld_vpiputv_driver(struct net_pin_t *);
extern struct xstk_t *__ld_gate_driver(struct net_pin_t *);
extern struct xstk_t *__ld_iconn_up_driver(register struct net_pin_t *);
extern struct xstk_t *__ld_pb_iconn_up_driver(register struct net_pin_t *);
extern struct xstk_t *__ld_modport_down_driver(register struct net_pin_t *);
extern struct xstk_t *__ld_pb_modport_down_driver(register struct net_pin_t *);


extern word32 __ld_gate_out(register struct gate_t *, int32 *);
extern void __eval_wire(word32 *, word32 *, struct net_t *,
 struct net_pin_t *); 
extern void __eval_wide_wire(word32 *, word32 *, word32 *, word32 *, int32,
 word32);
extern void __eval_1w_nonstren(register word32 *, register word32 *,
 register word32, register word32, word32);
extern struct xstk_t *__stload_mdrwire(struct net_t *);
extern struct xstk_t *__ld_stwire_driver(register struct net_pin_t *);
extern struct xstk_t *__ld_sttfrwarg_driver(struct net_pin_t *);
extern struct xstk_t *__ndst_eval_xpr(struct expr_t *);
extern int32 __get_const_bselndx(register struct expr_t *);
extern void __st_standval(register byte *, register struct xstk_t *, byte);
extern word32 __comb_1bitsts(word32, register word32, register word32);
extern char *__gstate_tostr(char *, struct gate_t *, int32);
extern void __show_allvars(void);
extern void __emit_1showvar(struct net_t *, struct gref_t *);
extern char *__bld_valofsched(char *, struct tev_t *);
extern char *__bld_showvars_prefix(char *, struct net_t *, struct gref_t *);
extern void __exec_dumpvars(struct expr_t *);
extern void __setup_dmpvars(void);
extern void __do_dmpvars_baseline(char *);
extern void __do_dmpvars_chg(void);
extern int32 __cnt_dcelstels(register struct dcevnt_t *);

extern char *__to_ptnam(char *, word32);
extern char *__to_mpnam(char *, char *);
extern char *__bld_lineloc(char *, word32, int32);
extern void __strenwiden_sizchg(struct xstk_t *, int32);
extern struct xstk_t *__eval2_xpr(register struct expr_t *);
extern void __sizchgxs(register struct xstk_t *, int32);
extern void __sgn_xtnd_widen(struct xstk_t *, int32);
extern void __sizchg_widen(register struct xstk_t *, int32);
extern void __narrow_sizchg(register struct xstk_t *, int32);
extern void __fix_widened_toxs(register struct xstk_t *, int32);
extern void __stren_exec_ca_concat(struct expr_t *, byte *, int32);
extern void __exec_conta_assign(struct expr_t *, register word32 *,
 register word32 *, int32);
extern void __exec_ca_concat(struct expr_t *, register word32 *,
 register word32 *, int32);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern char *__to_gassign_str(char *, struct expr_t *);
extern void __ld_perinst_val(register word32 *, register word32 *,
 union pck_u, int32);
extern void __grow_xstk(void);
extern void __chg_xstk_width(struct xstk_t *, int32);
extern void __lhsbsel(register word32 *, register int32, word32);
extern void __my_free(char *, int32);
extern char *__my_malloc(int32);
extern char *__to_vvstnam(char *, word32);
extern char *__to_vvnam(char *, word32);
extern char *__to_timstr(char *, word64 *);
extern int32 __correct_forced_newwireval(struct net_t *, word32 *, word32 *);
extern void __chg_st_val(struct net_t *, register word32 *, register word32 *);
extern void __ld_wire_val(register word32 *, register word32 *,
 struct net_t *);
extern int32 __comp_ndx(register struct net_t *, register struct expr_t *);
extern void __ld_bit(register word32 *, register word32 *,
 register struct net_t *, int32);
extern int32 __forced_inhibit_bitassign(struct net_t *, struct expr_t *,
 struct expr_t *);
extern void __assign_to_bit(struct net_t *, struct expr_t *, struct expr_t *,
 register word32 *, register word32 *);
extern void __xmrpush_refgrp_to_targ(struct gref_t *);
extern int32 __has_vpi_driver(struct net_t *np, struct net_pin_t *npp);
extern int32 __update_tran_harddrvs(struct net_t *);
extern void __eval_tran_bits(register struct net_t *);
extern void __ld_addr(word32 **, word32 **, register struct net_t *);
extern void __st_val(struct net_t *, register word32 *, register word32 *);
extern char *__st_regab_tostr(char *, byte *, int32);
extern char *__regab_tostr(char *, word32 *, word32 *, int32, int32, int32);
extern void __pth_stren_schd_allofwire(struct net_t *, register byte *, int32);
extern void __wdel_stren_schd_allofwire(struct net_t *, register byte *,
 int32);
extern void __pth_schd_allofwire(struct net_t *, register word32 *,
 register word32 *, int32);
extern void __wdel_schd_allofwire(struct net_t *, register word32 *,
 register word32 *, int32);
extern void __rhspsel(register word32 *, register word32 *, register int32,
 register int32);
extern void __lhspsel(register word32 *, register int32, register word32 *,
 register int32);
extern char *__to_wtnam(char *, struct net_t *);
extern char *__to_wtnam2(char *, word32);
extern void __adds(char *);
extern void __chg_xprline_size(int32);
extern void __disp_itree_path(register struct itree_t *, struct task_t *);
extern void __ld_gate_wide_val(word32 *, word32 *, word32 *, int32);
extern void __trunc_cstr(char *, int32, int32);
extern char *__msg2_blditree(char *, struct itree_t *);
extern int32 __get_arrwide(struct net_t *);
extern char *__var_tostr(char *, struct net_t *, int32, int32, int32);
extern void __disp_var(struct net_t *, int32, int32, int32, char);
extern int32 __unnormalize_ndx(struct net_t *, int32);
extern void __get_cor_range(register int32, union intptr_u, register int32 *,
 register int32 *);
extern void __getarr_range(struct net_t *, int32 *, int32 *, int32 *);
extern void __getwir_range(struct net_t *, int32 *, int32 *);
extern int32 __unmap_ndx(int32, int32, int32);
extern char *__schop(char *, char *);
extern char *__get_tfcellnam(struct tfrec_t *);
extern int32 __vval_isallzs(word32 *, word32 *, int32);
extern int32 __st_vval_isallzs(byte *, int32);
extern char *__to_wrange(char *, struct net_t *);
extern char *__to_arr_range(char *, struct net_t *);
extern char *__to1_stren_nam(char *, int32, int32);
extern int32 __fr_cap_size(int32);
extern void __bld_forcedbits_mask(word32 *, struct net_t *);
extern int32 __wide_vval_is0(register word32 *, int32);
extern int32 __match_push_targ_to_ref(word32, struct gref_t *);
extern char *__to_tcnam(char *, word32);
extern int32 __get_eval_word(struct expr_t *, word32 *);
extern int32 __ip_indsrch(char *);
extern char *__to_timunitnam(char *, word32);
extern char *__msg_blditree(char *, struct itree_t *, struct task_t *);
extern void __sdispb(register word32 *, register word32 *, int32, int32);
extern int32 __trim1_0val(word32 *, int32);
extern void __declcnv_tostr(char *, word32 *, int32, int32);
extern struct task_t *__getcur_scope_tsk(void);
extern void __add_nchglst_el(register struct net_t *);
extern void __add_dmpv_chglst_el(struct net_t *);
extern void __wakeup_delay_ctrls(register struct net_t *, register int32,
 register int32);
extern char *__to_npptyp(char *, struct net_pin_t *);

extern void __cv_msg(char *, ...);
extern void __cvsim_msg(char *, ...);
extern void __tr_msg(char *, ...);
extern void __sgfwarn(int32, char *, ...);
extern void __pv_warn(int32, char *, ...);
extern void __sgferr(int32, char *, ...);
extern void __dbg_msg(char *, ...);
extern void __sgfinform(int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);

extern word32 __masktab[];
extern byte __stren_map_tab[];
extern word32 __cap_to_stren[];

/*
 * ROUTINES TO SET PORT ASSIGN
 */

/*
 * set module port assign routines (type of acceleration)
 */ 
extern void __set_mpp_assign_routines(void)
{
 register int32 pi, ii;
 register struct mod_pin_t *mpp;
 int32 pnum;
 struct mod_t *mdp, *imdp;
 struct inst_t *ip;
 struct expr_t *xp;

 /* step 1: set the - down from iconn to mod port from module ports */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if ((pnum = mdp->mpnum) == 0) continue;

   /* SJM - PORT REMOVE */
   /* for top module set assign routine to nil - will force early core dump */
   /* if error */
   if (mdp->minstnum == 0)
    {
     /* leave port assign routines as nil for top level */
     for (pi = 0; pi < mdp->mpnum; pi++) 
      { mpp = &(mdp->mpins[pi]); mpp->assgnfunc_set = TRUE; }
     continue;
    }

   for (pi = 0; pi < mdp->mpnum; pi++) 
    {
     mpp = &(mdp->mpins[pi]);

     /* never have NP ICONN or NP MDPRT for inout */
     if (mpp->mptyp == IO_BID) { mpp->assgnfunc_set = TRUE; continue; }

     /* need iconn lhs info for setting output assign routines */
     if (mpp->mptyp == IO_OUT) continue;


     set_from_mpp_unopts(mdp, mpp, -1);
    }
  }
 /* step 2: look at all up instance connections to see if can optimize */ 
 /* can only be down after all low conn mod ports processed in step 1 */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   /* first process all instances in module */
   for (ii = 0; ii < mdp->minum; ii++)
    {
     ip = &(mdp->minsts[ii]);
     imdp = ip->imsym->el.emdp;
     if ((pnum = imdp->mpnum) == 0) continue;

     for (pi = 0; pi < pnum; pi++)
      {
       mpp = &(imdp->mpins[pi]);
       if (mpp->assgnfunc_set)
        {
         /* if assgn func set turned on, must not try to optimize more */
         continue;
        }

       /* port (either in or out) down must be simple ID for any accelerate */
       if (mpp->mpref->optyp != ID)  
        {
         if (__debug_flg)
          dbg_unopt2_msg(imdp, ip, pi, mpp, -1, "port is expression");
         if (mpp->mptyp == IO_IN)
          mpp->mpaf.mpp_downassgnfunc = std_downtomdport;
         else mpp->mpaf.mpp_upassgnfunc = std_uptoiconn;
         mpp->assgnfunc_set = TRUE;
         continue;
        }

       /* if assign not know, down must be simple wire */
       xp = ip->ipins[pi];

       /* set the best possible optimization routine from highconn */
       set_from_hconn_unopts(imdp, ip, pi, xp, mpp, -1);
      }
    }
  } 
}

/*
 * set decomposed from hconn concat per bit mod port assign routines
 * i.e. set type of acceleration for interpreter
 *
 * -O does much better job of optimizing these
 */ 
extern void __set_pb_mpp_assign_routines(void)
{
 register int32 pi, ii, pbi;
 register struct mod_pin_t *mpp;
 int32 pnum;
 struct mod_pin_t *mast_mpp;
 struct mod_t *mdp, *imdp;
 struct inst_t *ip;
 struct expr_t *xp;

 /* step 1: set the - down from iconn to mod port from module ports */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if ((pnum = mdp->mpnum) == 0) continue;

   /* no top level ports do not have any highconn - so never decomposed */
   if (mdp->minstnum == 0) continue;

   for (pi = 0; pi < mdp->mpnum; pi++) 
    {
     mast_mpp = &(mdp->mpins[pi]);
     if (!mast_mpp->has_scalar_mpps) continue; 

     for (pbi = 0; pbi < mast_mpp->mpwide; pbi++)
      {
       mpp = &(mast_mpp->pbmpps[pbi]);
       
       /* need iconn lhs info for setting output assign routines */
       if (mpp->mptyp == IO_OUT) continue;
       set_from_mpp_unopts(mdp, mpp, pbi);
      }
    }
  }
 /* step 2: look at all up instance connections to see if can optimize */ 
 /* can only be down after all low conn mod ports processed in step 1 */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   /* first process all instances in module */
   for (ii = 0; ii < mdp->minum; ii++)
    {
     ip = &(mdp->minsts[ii]);
     imdp = ip->imsym->el.emdp;
     if ((pnum = imdp->mpnum) == 0) continue;

     for (pi = 0; pi < pnum; pi++)
      {
       mast_mpp = &(imdp->mpins[pi]);
       /* if low conn of this high conn not dcomposed into per bit, done */
       if (!mast_mpp->has_scalar_mpps) continue; 

       /* if this one's up iconn not separable - ignore for setting accel */
       if (ip->pb_ipins_tab == NULL || ip->pb_ipins_tab[pi] == NULL)
        continue;

       for (pbi = 0; pbi < mast_mpp->mpwide; pbi++)
        {
         mpp = &(mast_mpp->pbmpps[pbi]);
         if (mpp->assgnfunc_set) continue;

         /* LOOKATME ??? - down will almost always be bsel for these */ 
         /* port (in or out) down must be simple ID for any accelerate */
         if (mpp->mpref->optyp != ID)  
          {
           if (__debug_flg)
            dbg_unopt2_msg(imdp, ip, pi, mpp, pbi,
             "per bit port is expression");

           if (mpp->mptyp == IO_IN)
            mpp->mpaf.mpp_downassgnfunc = std_downtomdport;
           else mpp->mpaf.mpp_upassgnfunc = std_uptoiconn;
           mpp->assgnfunc_set = TRUE;

           continue;
          }

         /* if assign not know, down must be simple wire */
         xp = ip->pb_ipins_tab[pi][pbi];
         /* set the best possible optimization routine from highconn */
         set_from_hconn_unopts(imdp, ip, pi, xp, mpp, pbi);
        }
      }
    }
  } 
}

/*
 * set ports that can't be optimized determinable only from down mpp
 * also sets input port fi>1 down assign since independent of iconn 
 *
 * if pbi not -1 then this is bit of per bit decomposed input port
 */
static void set_from_mpp_unopts(struct mod_t *mdp, struct mod_pin_t *mpp,
 int32 pbi)
{
 struct net_t *down_np;

 if (mpp->mpref->optyp != ID)
  {  
   if (__debug_flg) dbg_unopt_msg(mdp, mpp, pbi, "port is expression");
   mpp->mpaf.mpp_downassgnfunc = std_downtomdport;
   mpp->assgnfunc_set = TRUE;
   return;
  }
 down_np = mpp->mpref->lu.sy->el.enp;
 /* path destinations are stored as delay wires */
 if (down_np->ntraux != NULL || down_np->nrngrep == NX_DWIR)
  {
   if (__debug_flg)
    dbg_unopt_msg(mdp, mpp, pbi, "port in tran chan., delay or path dest.");
   mpp->mpaf.mpp_downassgnfunc = std_downtomdport;
   mpp->assgnfunc_set = TRUE;
   return;
  }
 /* if port wire multfi - all will be so now know fits fi>1 acc case */
 if (down_np->n_multfi)
  {
   if (__debug_flg) dbg_unopt_msg(mdp, mpp, pbi,
    "port fi>1 - optimized some");
   mpp->mpaf.mpp_downassgnfunc = multfi_acc_downtomdport;
   mpp->assgnfunc_set = TRUE;
  }
}

/*
 * set optimized routines from high conn iconn info
 *
 * uses up iconn expr info and optimize info set in step 1 mpp processing 
 * know down is simple ID or not called 
 *
 * works for both normal ports and per bit decomposed input ports
 * from concat high conns where pbi is bit, and xp/mpp are per bit decomposed
 */
static void set_from_hconn_unopts(struct mod_t *imdp, struct inst_t *ip,
 int32 pi, struct expr_t *xp, struct mod_pin_t *mpp, int32 pbi)
{
 struct net_t *up_np, *down_np;

 down_np = mpp->mpref->lu.sy->el.enp;
 /* always non acc if not same width */
 if (mpp->mpref->szu.xclen != xp->szu.xclen)
  {
   if (__debug_flg) dbg_unopt2_msg(imdp, ip, pi, mpp, pbi, "widths differ");
set_unoptim:
   if (mpp->mptyp == IO_IN) mpp->mpaf.mpp_downassgnfunc = std_downtomdport;
   else mpp->mpaf.mpp_upassgnfunc = std_uptoiconn;
   mpp->assgnfunc_set = TRUE;
   return;
  }
 if (mpp->mptyp == IO_IN)
  {
   /* down input port assign, know port lhs is optimizable */
   /* make sure rhs up iconn optimizable */
   if (xp->optyp == ID)
    {
     up_np = xp->lu.sy->el.enp;
     /* both up and down must be have or not have stren */
     if ((up_np->n_stren && !down_np->n_stren)
      || (!up_np->n_stren && down_np->n_stren))
      { 
       if (__debug_flg) dbg_unopt2_msg(imdp, ip, pi, mpp, pbi,
        "strength needed differs");
       goto set_unoptim;
      }  
     if (down_np->n_stren) 
      {
       if (xp->szu.xclen == 1) 
        mpp->mpaf.mpp_downassgnfunc = stbsel_acc_downtomdport;
       else mpp->mpaf.mpp_downassgnfunc = stacc_downtomdport;
       return;
      }
     /* non strength ID case */  
     if (xp->szu.xclen == 1)
      mpp->mpaf.mpp_downassgnfunc = bsel_acc_downtomdport;
     else mpp->mpaf.mpp_downassgnfunc = acc_downtomdport;
     return;
    }
   /* bit select for this up iconn case */
   /* SJM 07/24/03 - bit select from XMR high conn must not be optimized */
   if (xp->optyp != LSB || xp->lu.x->optyp != ID
    || mpp->mpref->szu.xclen != 1)
    {   
     if (__debug_flg)
      dbg_unopt2_msg(imdp, ip, pi, mpp, pbi,
       "vector port but not bit select from ID highconn");
     goto set_unoptim;
    } 
   /* maybe up bit select or scalar - must be constant index */
   if (xp->ru.x->optyp != NUMBER)
    {
     if (__debug_flg)
      dbg_unopt2_msg(imdp, ip, pi, mpp, pbi, "bit select highconn expr.");
     goto set_unoptim;
    }
   up_np = xp->lu.x->lu.sy->el.enp;
   /* both must be same strength */
   if ((up_np->n_stren && !down_np->n_stren)
    || (!up_np->n_stren && down_np->n_stren))
    {  
     if (__debug_flg)
      dbg_unopt2_msg(imdp, ip, pi, mpp, pbi, "bsel strength needed differs");
     goto set_unoptim;
    }
   /* may reduce optimlzation level to bsel form */ 
   if (down_np->n_stren)
    mpp->mpaf.mpp_downassgnfunc = stbsel_acc_downtomdport;
   else mpp->mpaf.mpp_downassgnfunc = bsel_acc_downtomdport;
   return;
  }

 /* hard output port case - assign to varying up iconns */ 
 /* know mod port is ID and widths same or will not get here */
 if (xp->optyp == ID) up_np = xp->lu.sy->el.enp; 
 else if (xp->optyp == LSB) up_np = xp->lu.x->lu.sy->el.enp;
 /* no debug message needed here because assign never happens */
 else if (xp->optyp == OPEMPTY) goto set_unoptim;
 else
  {
   if (__debug_flg)
    dbg_unopt2_msg(imdp, ip, pi, mpp, pbi, "iconn not bsel or wire");
   goto set_unoptim;
  }

 /* cannot have delay (also path dest.) or be in tran channel */
 if (up_np->nrngrep == NX_DWIR || up_np->ntraux != NULL)
  {
   if (__debug_flg)
    dbg_unopt2_msg(imdp, ip, pi, mpp, pbi,
    "iconn in tran chan or delay/path dest.");
   goto set_unoptim;
  }
 if (up_np->n_multfi)
  {
   if (mpp->mpaf.mpp_upassgnfunc == NULL)
    { mpp->mpaf.mpp_upassgnfunc = multfi_acc_uptoiconn; return; }
   /* if some instance connections, fi>1 and other not, not acc */ 
   if (mpp->mpaf.mpp_upassgnfunc != multfi_acc_uptoiconn)
    goto mixed_multfi;
   return;
  }
 if (mpp->mpaf.mpp_upassgnfunc == multfi_acc_uptoiconn)
  {
mixed_multfi:
   if (__debug_flg)
    dbg_unopt2_msg(imdp, ip, pi, mpp, pbi, 
     "mixed fi>1 and non fi>1 highconns");
   goto set_unoptim;
  }  

 /* both strengths must be same */
 if ((up_np->n_stren && !down_np->n_stren)
  || (!up_np->n_stren && down_np->n_stren)) goto set_unoptim;
 if (xp->optyp == ID)
  {
   if (up_np->n_stren) 
    {
     /* must be acc. bsel */
     if (xp->szu.xclen == 1) mpp->mpaf.mpp_upassgnfunc = stbsel_acc_uptoiconn;
     else mpp->mpaf.mpp_upassgnfunc = stacc_uptoiconn;
     return;
    }
   if (xp->szu.xclen == 1) mpp->mpaf.mpp_upassgnfunc = bsel_acc_uptoiconn;
   else mpp->mpaf.mpp_upassgnfunc = acc_uptoiconn;
   return;
  }
 /* bit select case */
 if (down_np->n_stren) mpp->mpaf.mpp_upassgnfunc = stbsel_acc_uptoiconn;
 else mpp->mpaf.mpp_upassgnfunc = bsel_acc_uptoiconn;
}

/*
 * assign std port assign routines if accelerate (port assign optimize off)
 */
extern void __set_mpp_aoff_routines(void)
{
 register int32 pi, pbi;
 register struct mod_t *mdp;
 register struct mod_pin_t *mpp, *mpp2;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->minstnum == 0)
    {
     for (pi = 0; pi < mdp->mpnum; pi++)
      {
       mpp = &(mdp->mpins[pi]);
       mpp->assgnfunc_set = TRUE;
      }
     continue;
    }

   for (pi = 0; pi < mdp->mpnum; pi++) 
    {
     mpp = &(mdp->mpins[pi]);
     mpp->assgnfunc_set = TRUE;

     /* lhs destination from port type determines routine */
     if (mpp->mptyp == IO_OUT) mpp->mpaf.mpp_upassgnfunc = std_uptoiconn;
     else if (mpp->mptyp == IO_IN)
      mpp->mpaf.mpp_downassgnfunc = std_downtomdport;
     else continue;

     /* SJM 09/18/06 - if not simulated as decomposed bits, avoid this code */
     if (!mpp->has_scalar_mpps) continue;

     for (pbi = 0; pbi < mpp->mpwide; pbi++)    
      {
       mpp2 = &(mpp->pbmpps[pbi]);
       /* only per bit for input ports */
       mpp2->mpaf.mpp_downassgnfunc = std_downtomdport;
       mpp2->assgnfunc_set = TRUE;
      }
    }
  }
}

/*
 * set new routine when mod port wire has added vpi put value driver added 
 *
 * SJM 09/20/02 - this may be separated per bit mod port 
 */
extern void __vpi_set_downtomdport_proc(struct mod_pin_t *mpp,
 struct net_t *np)
{
 /* if not accelerated when fi==1, then cannot be when fi>1 */
 if (mpp->mpaf.mpp_downassgnfunc == std_downtomdport) return;

 /* driver of wire np in module is module port from up inst. */
 if (mpp->mpref->optyp == ID && np->ntraux == NULL && np->nrngrep != NX_DWIR)
   mpp->mpaf.mpp_downassgnfunc = multfi_acc_downtomdport;
 else mpp->mpaf.mpp_downassgnfunc = std_downtomdport;
}

/*
 * for up to iconn that is now multfi, cannot optimize
 * because do not know other instances  
 */
extern void __vpi_set_upiconnport_proc(struct mod_pin_t *mpp)
{
 mpp->mpaf.mpp_upassgnfunc = std_uptoiconn;
}

/*
 * write a debug reason message for why port not optimized 
 * this is port side message for input ports
 */
static void dbg_unopt_msg(struct mod_t *mdp, struct mod_pin_t *mpp, int32 pbi,
 char *reason)
{
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 if (pbi == -1) __to_mpnam(s2, mpp->mpsnam);
 else sprintf(s2, "%s bit %d", __to_mpnam(s3, mpp->mpsnam), pbi);
 __dbg_msg("module %s %s port %s unoptimized: %s\n", mdp->msym->synam,
  __to_ptnam(s1, mpp->mptyp), s2, reason);  
}

/*
 * write a debug reason message for why port not optimized 
 * this is inst conn. side message for output ports
 */
static void dbg_unopt2_msg(struct mod_t *mdp, struct inst_t *ip, int32 pi,
 struct mod_pin_t *mpp, int32 pbi, char *reason)
{
 char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[RECLEN];

 if (pbi == -1) __to_mpnam(s3, mpp->mpsnam);
 else sprintf(s3, "%s bit %d", __to_mpnam(s4, mpp->mpsnam), pbi);

 __dbg_msg("inst. %s(%s) at %s %s port %s (pos. %d) unoptimized: %s\n",
  ip->isym->synam, mdp->msym->synam, __bld_lineloc(s1, ip->isym->syfnam_ind,
  ip->isym->sylin_cnt), __to_ptnam(s2, mpp->mptyp), s3, pi, reason);  
}

/*
 * PORT UP AND DOWN ASSIGN ROUTINES
 */

/*
 * assign all input ports downward into lower itree instance
 * called from up_itp where itp (down) is under itree place
 *
 * assumes called with nothing pushed on itree stack
 * never called for inout in tran channel - initialized elsewhere
 */
extern void __init_instdownport_contas(struct itree_t *up_itp,
 struct itree_t *down_itp)
{
 register int32 pi;
 register struct mod_pin_t *mpp;
 struct expr_t *xp;
 struct inst_t *ip;
 struct mod_t *down_mdp;
 int32 pnum;

 ip = down_itp->itip;
 down_mdp = ip->imsym->el.emdp;
 if ((pnum = down_mdp->mpnum) == 0) return;

 /* this must work from up itree loc */
 __push_itstk(up_itp);
 for (pi = 0; pi < pnum; pi++) 
  {
   xp = ip->ipins[pi];
   mpp = &(down_mdp->mpins[pi]);
   if (mpp->mptyp != IO_IN) continue;
   
   __immed_assigns++;
   /* must be call with current itree location up (rhs) but passed down */
   /* exec inst. input expr. downward to port changed assign */
   /* notice down always take only 4 args, down do not have first mpp */
   (*mpp->mpaf.mpp_downassgnfunc)(mpp->mpref, xp, down_itp);
  }
 __pop_itstk();
}

/*
 * initialize cross module out instance ports continuous assigns 
 *
 * can get to up itree loc. but following up ptr
 * never called for inout in tran channel - initialized elsewhere
 */
extern void __init_instupport_contas(struct itree_t *down_itp)
{
 register int32 pi;
 register struct mod_pin_t *mpp;
 struct expr_t *xp;
 struct inst_t *ip;
 struct mod_t *down_mdp;
 struct itree_t *up_itp;
 int32 pnum;

 ip = down_itp->itip;
 down_mdp = ip->imsym->el.emdp;
 if ((pnum = down_mdp->mpnum) == 0) return;

 /* cannot assume current itree location - need down starting */
 __push_itstk(down_itp);
 for (pi = 0; pi < pnum; pi++) 
  {
   xp = ip->ipins[pi];
   mpp = &(down_mdp->mpins[pi]);
   
   if (mpp->mptyp != IO_OUT) continue;

   __immed_assigns++;
   /* called from down module itree location */ 
   /* SJM - PORT REMOVE - no assign if top level module - ports remain */
   if ((up_itp = __inst_ptr->up_it) == NULL) continue;

   /* assign from rhs down mpp ref. to up lhs iconn */
   /* notice up always take only 3 args, down have extra 1st arg mpp */
   (*mpp->mpaf.mpp_upassgnfunc)(xp, mpp->mpref, up_itp);
  }
 __pop_itstk();
}

/*
 * ROUTINES FOR VARIOUS CASES OF DOWN INPUT PORT FROM ICONN TO MDPRT ASSIGN
 */

/*
 * down from iconn to port assign - for all non special cases
 *
 * called from up iconn rhs itree loc.
 * this is standard (general) function that must handle all cases 
 *
 * if any inst. size change, in tran channel, lhs has delay, some but not
 * all fi>1, any iconn is xmr, any iconn is concat, down port is expr.,
 * up is not wire or reg expr, this routine used - must be some more reasons?
 * called from up rhs iconn itree loc.
 */
static void std_downtomdport(register struct expr_t *lhsx,
 register struct expr_t *rhsx, struct itree_t *down_itp)
{
 register struct xstk_t *xsp;
 int32 schd_wire, orhslen;

 if (lhsx->x_multfi)
  {
   /* always must evaluate drivers with itstk of lhs (i.e. down) */
   __push_itstk(down_itp);
   /* all lhs expr. containging tran chan wire mult fi and handled here */
   __mdr_assign_or_sched(lhsx);

   /* assigned to down (destination) wire loads of this new assigned to */
   /* wire will be added to net chg list */
   /* DBG remove ---
   if (__debug_flg && __ev_tracing)
    prt_assignedto_val(lhsx, "fi>1 down port assign");
   --- */
   __pop_itstk();
   return;
  }

 /* if inst. input expr. is empty, nothing to do */
 if (rhsx->optyp == OPEMPTY) return;
 if (lhsx->x_stren)
  {
   /* handle lhs stren assign - pass strength through */
   /* other side always marked strength and if rhs reg, will add in strong */ 
   xsp = __ndst_eval_xpr(rhsx);
   /* widen to lhs width with z's - if too narrow, high part just unused */
   /* SJM 05/10/04 - no sign extension because widening to z'x */
   /* AIV 03/15/07 - strength for xsp->xslen was wrong for same reaston */
   /* as one fixed on 11/21/06 */
   /* SJM 11/21/06 - for stren's xsp is 4 times width for byte per bit */
   /* problem is xslen for stren is not the expr bit width */
   if (rhsx->szu.xclen < lhsx->szu.xclen)
    __strenwiden_sizchg(xsp, lhsx->szu.xclen);
  }
 else
  {
   xsp = __eval_xpr(rhsx);
   if (lhsx->szu.xclen != xsp->xslen)
    {
     orhslen = xsp->xslen;

     /* SJM 09/29/03 - change to handle sign extension and separate types */
     if (xsp->xslen > lhsx->szu.xclen)
      __narrow_sizchg(xsp, lhsx->szu.xclen);
     else if (xsp->xslen < lhsx->szu.xclen)
      {
       if (rhsx->has_sign) __sgn_xtnd_widen(xsp, lhsx->szu.xclen);
       else __sizchg_widen(xsp, lhsx->szu.xclen);
      }

     /* widened, set bits higher than orhslen to z */ 
     /* LOOKATME - only strength continuous assignments widen to z */
     /* all others widen to 0 */
     /* ??? if (orhslen < xsp->xslen) __fix_widened_tozs(xsp, orhslen); */
     /* SJM 05/10/04 - widening to x's eliminates need for sign difference */
     if (__wire_init) __fix_widened_toxs(xsp, orhslen);
    }
  }
 /* eval. in up itree side but assign on to lhs in down */ 
 __push_itstk(down_itp);
 if (lhsx->lhsx_ndel && !__wire_init) schd_wire = TRUE;
 else schd_wire = FALSE;
 if (lhsx->x_stren)
  {
   /* port conta just assigns strengths */ 
   if (lhsx->optyp == LCB)
    __stren_exec_ca_concat(lhsx, (byte *) xsp->ap, schd_wire);
   else __exec_conta_assign(lhsx, xsp->ap, xsp->bp, schd_wire);
  }
 else
  {
   if (lhsx->optyp == LCB)
    __exec_ca_concat(lhsx, xsp->ap, xsp->bp, schd_wire);
   else __exec_conta_assign(lhsx, xsp->ap, xsp->bp, schd_wire);
  }
 __pop_xstk();
 /* DBG remove --
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "port down assign");
 --- */  
 /* notice cannot pop until here since lhs is down */
 __pop_itstk();
}

/*
 * print value of assign to output wire 
 * only called when debug flag and some kind of tracing on
 */
static void prt_assignedto_val(struct expr_t *xp, char *nppnam)
{
 char s1[RECLEN], s2[RECLEN];

 if (xp->x_stren) strcpy(s2, "strength "); else strcpy(s2, "");
 __tr_msg("== %s lvalue %swire %s set or scheduled to %s\n", nppnam, s2,
  __msgexpr_tostr(__xs, xp), __to_gassign_str(s1, xp));
}

/*
 * mulfi lhs upward instance port assign from down module port assign 
 * for output port
 *
 * only for down port multi-fi wire (could handle selects but rare)
 * since port expressions rare
 * since down lhs is module port all will be same 
 * if port expr. or inout in tran channel, cannot use this routine
 * can be either strength or non strength
 * called from up rhs iconn itree location
 */
static void multfi_acc_downtomdport(register struct expr_t *lhsx,
 struct expr_t *rhsx, struct itree_t *down_itp)
{
 /* for mdport fi>1, eval all drivers */
 /* must evaluate drivers with itstk of lhs (i.e. down here) */
 __push_itstk(down_itp);
 __assign_1mdrwire(lhsx->lu.sy->el.enp);
 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "fi>1 down port assign");
 --- */
 __pop_itstk();
} 

/*
 * stren both wire/reg up iconn down to mod port assign special case routine 
 * only called if all instances both same width IDs
 * never for scalar - use bit select version for that 
 *
 * called from up rhs iconn itree location
 */
static void stacc_downtomdport(register struct expr_t *lhsx,
 register struct expr_t *rhsx, struct itree_t *down_itp)
{
 byte *bp, *sbp;
 struct net_t *lhsnp;
 struct xstk_t *xsp;

 push_xstk_(xsp, 4*rhsx->szu.xclen);
 sbp = (byte *) xsp->ap;
 get_stwire_addr_(bp, rhsx->lu.sy->el.enp);
 memcpy(sbp, bp, rhsx->szu.xclen); 

 /* assign in down */
 __push_itstk(down_itp);
 lhsnp = lhsx->lu.sy->el.enp;
 /* this adds the changed wire to nchglst if needed */
 /* return F if all of wire forced, nothing to do */ 
 if (lhsnp->frc_assgn_allocated)
  {
   if (!__correct_forced_newwireval(lhsnp, (word32 *) sbp, (word32 *) NULL))
    { __pop_itstk(); __pop_xstk(); return; }
  }
 __chg_st_val(lhsnp, (word32 *) sbp, (word32 *) NULL);
 __pop_xstk();
 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "strength mod. port down assign");
 --- */
 __pop_itstk();
}

/*
 * stren both wire/reg up iconn down to mod port assign special case routine 
 * this is also called for all scalars on up inst. 
 *
 * only called if all instances both scalar IDs or up bsel and down
 * and nothing special such as delay wire
 * only strength scalars handled here
 */
static void stbsel_acc_downtomdport(register struct expr_t *lhsx,
 register struct expr_t *rhsx, struct itree_t *down_itp)
{
 struct net_t *lhsnp;
 byte sb2, *bp, *sbp;

 /* eval rhs up iconn in current itree up loc. */
 if (rhsx->optyp == LSB) access_stbsel(&sb2, rhsx);
 else { get_stwire_addr_(bp, rhsx->lu.sy->el.enp); sb2 = bp[0]; }

 /* assign in down */
 __push_itstk(down_itp);
 lhsnp = lhsx->lu.sy->el.enp;
 /* this is needed so for endian differences - cast of ptr does nothing */
 sbp = &sb2;
 /* this adds the changed wire to nchglst if needed */
 /* return F if all of wire forced, nothing to do */ 
 if (lhsnp->frc_assgn_allocated)
  {
   if (!__correct_forced_newwireval(lhsnp, (word32 *) sbp, (word32 *) NULL))
    { __pop_itstk(); return; }
  }
 __chg_st_val(lhsnp, (word32 *) sbp, (word32 *) NULL);
 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "strength scalar mod. port assign");
 --- */
 __pop_itstk();
}

/*
 * non stren both wire upward to instance port assign special case routine 
 * only called if all instances both same width IDs
 * acc bsel routine called for scalars 
 */
static void acc_downtomdport(register struct expr_t *lhsx,
 register struct expr_t *rhsx, struct itree_t *down_itp)
{
 register struct xstk_t *xsp;
 struct net_t *lhsnp;

 /* up rhs always wire/reg or not fast routine */
 /* eval rhs up iconn in current itree up loc. */
 push_xstk_(xsp, rhsx->szu.xclen);
 __ld_wire_val(xsp->ap, xsp->bp, rhsx->lu.sy->el.enp);

 /* assign in down */
 __push_itstk(down_itp);
 lhsnp = lhsx->lu.sy->el.enp;
 /* this adds the changed wire to nchglst if needed */
 /* return F if all of wire forced, nothing to do */ 
 if (lhsnp->frc_assgn_allocated)
  {
   if (!__correct_forced_newwireval(lhsnp, xsp->ap, xsp->bp))
    { __pop_itstk(); __pop_xstk(); return; }
  }
 __chg_st_val(lhsnp, xsp->ap, xsp->bp);
 __pop_xstk();
 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "mod. port assign");
 --- */
 __pop_itstk();
}

/*
 * up wire bsel to instance port scalar assign special case routine 
 *
 * only called if up rhs is scalar or bit select and down port
 * is scalar wire - all insts must also be simple and not in tran channel
 * this is only for scalars and at least one bsel
 */
static void bsel_acc_downtomdport(register struct expr_t *lhsx,
 register struct expr_t *rhsx, struct itree_t *down_itp)
{
 int32 biti;
 word32 av, bv;
 struct net_t *lhsnp, *rhsnp;

 /* down rhs always wire/reg or not accelerated routine */
 if (rhsx->optyp == LSB)
  {
   rhsnp = rhsx->lu.x->lu.sy->el.enp;
   /* know biti never -1 since only fixed non -1 for this routine */
   biti = __comp_ndx(rhsnp, rhsx->ru.x);
   __ld_bit(&av, &bv, rhsnp, biti);
  }
 else __ld_wire_val(&av, &bv, rhsx->lu.sy->el.enp);

 /* assign in down - never an expr. */
 __push_itstk(down_itp);
 lhsnp = lhsx->lu.sy->el.enp;
 /* this adds the changed wire to nchglst if needed */
 /* return F if all of wire forced, nothing to do */ 
 if (lhsnp->frc_assgn_allocated)
  {
   if (!__correct_forced_newwireval(lhsnp, &av, &bv))
    { __pop_itstk(); return; }
  }
 __chg_st_val(lhsnp, &av, &bv);
 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "mod. port scalar assign");
 --- */
 __pop_itstk();
}

/*
 * ROUTINES FOR THE VARIOUS CASES OF FROM DOWN MDPRT TO UP ICONN ASSIGN
 */

/*
 * upward to instance port assign - for all non special cases
 * this is standard (general) function that must handle all cases 
 *
 * if any inst. size change, in tran channel, lhs has delay, some but not
 * all fi>1, any iconn is xmr, any iconn is concat, down port is expr.,
 * up is not wire or reg expr, this routine used - must be some more reasons?
 */
static void std_uptoiconn(register struct expr_t *lhsx,
 register struct expr_t *rhsx, struct itree_t *up_itp)
{
 register struct xstk_t *xsp;
 int32 schd_wire, orhslen;

 /* if destination driven by module output is empty, nothing to do */
 if (lhsx->optyp == OPEMPTY) return;

 /* for iconn part of fi>1, eval all drivers code */
 if (lhsx->x_multfi)
  {
   /* always must evaluate drivers with itstk of lhs (i.e. up here) */
   __push_itstk(up_itp);
   __mdr_assign_or_sched(lhsx);

   /* DBG remove ---
   if (__debug_flg && __ev_tracing)
    prt_assignedto_val(lhsx, "fi>1 upto highconn assign");
   --- */
   __pop_itstk();
   return;
  }
 /* load rhs according to lhs expr. type and make size equal to lhs */
 /* if port has no connections - LOOKATME maybe illegal, nothing to do */
 if (rhsx->optyp == OPEMPTY) return;

 /* eval rhs on current itree down loc. */
 if (lhsx->x_stren)
  {
   /* handle lhs stren assign - pass strength through */
   /* if reg, will add strong strength */
   xsp = __ndst_eval_xpr(rhsx);
   /* widen to lhs width with z's - if too narrow, high part just unused */
   /* SJM 05/10/04 - no sign extension because widening to z'x */
   /* SJM 11/21/06 - for stren's xsp is 4 times width for byte per bit */
   /* problem is xslen for stren is not the expr bit width */
   if (rhsx->szu.xclen < lhsx->szu.xclen)
    __strenwiden_sizchg(xsp, lhsx->szu.xclen);
  }
 else
  {
   xsp = __eval_xpr(rhsx);
   if (lhsx->szu.xclen != xsp->xslen)
    {
     orhslen = xsp->xslen;

     /* SJM 09/29/03 - change to handle sign extension and separate types */
     if (xsp->xslen > lhsx->szu.xclen)
      __narrow_sizchg(xsp, lhsx->szu.xclen);
     else if (xsp->xslen < lhsx->szu.xclen)
      {
       if (rhsx->has_sign) __sgn_xtnd_widen(xsp, lhsx->szu.xclen);
       else __sizchg_widen(xsp, lhsx->szu.xclen);
      }

     /* widened, set bits higher than orhslen to z */ 
     /* LOOKATME - only strength continuous assignments widen to z */
     /* all others widen to 0 except x during initialization */
     /* ??? if (orhslen < xsp->xslen) __fix_widened_tozs(xsp, orhslen); */
     /* SJM 05/10/04 - widening to x's eliminates need for sign difference */
     if (__wire_init) __fix_widened_toxs(xsp, orhslen);
    }
  }
 /* but assign on lhs up iconn itree location */
 __push_itstk(up_itp);
 if (lhsx->lhsx_ndel && !__wire_init) schd_wire = TRUE;
 else schd_wire = FALSE;
 /* notice here is lhs is gref, assign handles finding right itree loc. */
 if (lhsx->x_stren)
  {
   /* port conta just assigns strengths */ 
   if (lhsx->optyp == LCB)
    __stren_exec_ca_concat(lhsx, (byte *) xsp->ap, schd_wire);
   else __exec_conta_assign(lhsx, xsp->ap, xsp->bp, schd_wire);
  }
 else
  {
   if (lhsx->optyp == LCB)
    __exec_ca_concat(lhsx, xsp->ap, xsp->bp, schd_wire);
   else __exec_conta_assign(lhsx, xsp->ap, xsp->bp, schd_wire);
  }
 __pop_xstk();
 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "inst. conn. up assign");
 --- */
 __pop_itstk();
}

/*
 * mulfi lhs upward instance port assign from down module port assign 
 *
 * only for down wire and up any multi-fi non concat simple and non tran
 * if one multfi and any other not, std all cases routine must be used
 * can be either strength or non strength
 */
static void multfi_acc_uptoiconn(register struct expr_t *lhsx,
 struct expr_t *rhsx, struct itree_t *up_itp)
{
 /* for iconn part of fi>1, eval all drivers code */
 /* must evaluate drivers with itstk of lhs (i.e. up here) */
 __push_itstk(up_itp);
 if (lhsx->optyp == ID) __assign_1mdrwire(lhsx->lu.sy->el.enp);
 else __assign_1mdrwire(lhsx->lu.x->lu.sy->el.enp);
 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "fi>1 up to highconn assign");
 --- */
 __pop_itstk();
} 

/*
 * stren both wire upward to instance port assign special case routine 
 * only called if all instances both same width IDs
 */
static void stacc_uptoiconn(register struct expr_t *lhsx,
 register struct expr_t *rhsx, struct itree_t *up_itp)
{
 byte *bp, *sbp;
 struct net_t *lhsnp;
 struct xstk_t *xsp;

 /* eval rhs mod port in current itree down loc. */
 /* for accelerated know down port rhs is not an expression */
 push_xstk_(xsp, 4*rhsx->szu.xclen);
 sbp = (byte *) xsp->ap;
 get_stwire_addr_(bp, rhsx->lu.sy->el.enp);
 memcpy(sbp, bp, rhsx->szu.xclen); 

 /* assign in up */
 __push_itstk(up_itp);
 lhsnp = lhsx->lu.sy->el.enp;
 /* this adds the changed wire to nchglst if needed */
 /* return F if all of wire forced, nothing to do */ 
 if (lhsnp->frc_assgn_allocated)
  {
   if (!__correct_forced_newwireval(lhsnp, (word32 *) sbp, (word32 *) NULL))
    { __pop_itstk(); __pop_xstk(); return; }
  }
 __chg_st_val(lhsnp, (word32 *) sbp, (word32 *) NULL);
 __pop_xstk();
 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "strength inst. conn. up assign");
 --- */
 __pop_itstk();
}

/*
 * stren up wire bsel to instance port scalar assign special case routine 
 *
 * only called if up lhs is strength bit select or scalar and down
 * is scalar wire, all insts must also be simple and not in tran channel
 */
static void stbsel_acc_uptoiconn(register struct expr_t *lhsx,
 register struct expr_t *rhsx, struct itree_t *up_itp)
{
 byte *bp, sb2, *sbp;
 struct net_t *lhsnp;
 struct expr_t *idndp, *ndx1;

 /* eval rhs mod port in current itree down loc. */
 /* for accelerated know down port rhs is not an expression */
 get_stwire_addr_(bp, rhsx->lu.sy->el.enp);
 sb2 = bp[0];
 sbp = &sb2;

 __push_itstk(up_itp);
 /* here can be either stren scalar or bit select */
 if (lhsx->optyp == ID)
  {
   lhsnp = lhsx->lu.sy->el.enp;
   if (lhsnp->frc_assgn_allocated)
    {
     if (!__correct_forced_newwireval(lhsnp, (word32 *) sbp, (word32 *) NULL))
      { __pop_itstk(); return; }
    }
   __chg_st_val(lhsnp, (word32 *) sbp, (word32 *) NULL);
  }
 else
  {
   idndp = lhsx->lu.x;
   ndx1 = lhsx->ru.x;
   lhsnp = idndp->lu.sy->el.enp;
   /* the 1 bit is forced nothing to do else normal assign */
   if (lhsnp->frc_assgn_allocated
    && __forced_inhibit_bitassign(lhsnp, idndp, ndx1))
    { __pop_itstk(); return; }
   __assign_to_bit(lhsnp, idndp, ndx1, (word32 *) sbp, (word32 *) NULL);
  }  
 /* DBG remove --
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "strength scalar inst. conn. up assign");
 --- */
 __pop_itstk();
}

/*
 * non stren both wire upward to instance port assign special case routine 
 * only called if all instances both same width IDs
 */
static void acc_uptoiconn(register struct expr_t *lhsx,
 register struct expr_t *rhsx, struct itree_t *up_itp)
{
 register struct xstk_t *xsp;
 struct net_t *lhsnp;

 /* down rhs always wire/reg or not accerated routine */
 push_xstk_(xsp, rhsx->szu.xclen);
 __ld_wire_val(xsp->ap, xsp->bp, rhsx->lu.sy->el.enp);

 /* assign in up */
 __push_itstk(up_itp);
 lhsnp = lhsx->lu.sy->el.enp;
 /* this adds the changed wire to nchglst if needed */
 /* return F if all of wire forced, nothing to do */ 
 if (lhsnp->frc_assgn_allocated)
  {
   if (!__correct_forced_newwireval(lhsnp, xsp->ap, xsp->bp))
    { __pop_itstk(); __pop_xstk(); return; }
  }
 __chg_st_val(lhsnp, xsp->ap, xsp->bp);
 __pop_xstk();
 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "inst. conn. up assign");
 --- */
 __pop_itstk();
}

/*
 * up wire bsel to instance port scalar assign special case routine 
 *
 * only called if up lhs is strength bit select and down is scalar wire
 * all insts must also be simple and not in tran channel
 */
static void bsel_acc_uptoiconn(register struct expr_t *lhsx,
 register struct expr_t *rhsx, struct itree_t *up_itp)
{
 word32 av, bv;
 struct net_t *lhsnp;
 struct expr_t *idndp, *ndx1;

 /* down rhs always wire/reg or not accerated routine */
 __ld_wire_val(&av, &bv, rhsx->lu.sy->el.enp);

 /* assign in up */
 __push_itstk(up_itp);
 /* here can be either stren scalar or bit select */
 if (lhsx->optyp == ID)
  {
   lhsnp = lhsx->lu.sy->el.enp;
   if (lhsnp->frc_assgn_allocated)
    {
     if (!__correct_forced_newwireval(lhsnp, &av, &bv))
      { __pop_itstk(); return; }
    }
   __chg_st_val(lhsnp, &av, &bv);
  }
 else if (lhsx->optyp == LSB)
  {
   idndp = lhsx->lu.x;
   ndx1 = lhsx->ru.x;
   lhsnp = idndp->lu.sy->el.enp;
   /* the 1 bit is forced nothing to do else normal assign */
   if (lhsnp->frc_assgn_allocated
    && __forced_inhibit_bitassign(lhsnp, idndp, ndx1))
    { __pop_itstk(); return; }
   __assign_to_bit(lhsnp, idndp, ndx1, &av, &bv);
  }  
 /* DBG remove (also above bsel if) -- */
 else __case_terr(__FILE__, __LINE__);
 /* --- */

 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  prt_assignedto_val(lhsx, "strength scalar inst. conn. up assign");
 --- */
 __pop_itstk();
}

/*
 * MULTI FAN IN NON STRENGTH WIRE ASSIGN ROUTINES
 * ASSIGN BY EVALUATING ALL DRIVERS
 */

/*
 * continuous assign for multi-fi and stren (even if fi=1) wires
 * conta, mod and inst. ports
 *
 * this is called from itree loc. of lhsx, each driver eval may need
 * to move to driving itree loc. load driver routine handles moving
 *
 * notice even if only a few bits are changed - must reevaluate all bits
 * idea is that strengths are almost always scalars and multi-fi
 */
extern void __mdr_assign_or_sched(register struct expr_t *lhsx)
{
 register struct net_t *np;
 int32 nd_itpop;
 struct expr_t *idndp, *catndp;
 struct gref_t *grp;

 nd_itpop = FALSE;
 switch ((byte) lhsx->optyp) {
  case GLBREF:
   idndp = lhsx;
mdr_glb:
   grp = idndp->ru.grp;
   /* move from xmr ref. itree location to wire definition location */
   __xmrpush_refgrp_to_targ(grp);
   nd_itpop = TRUE;
   goto mdr_id;
  case ID:
   idndp = lhsx;
mdr_id:
   np = idndp->lu.sy->el.enp;
   /* handle fi>1 eval and store of bits not in any tran channel */
   if (np->ntraux != NULL)
    {
     /* if hard drivers of np do not change, channel cannot change */
     /* SJM 12/18/00 - for tran/tranif switch channels may just add to list */
     if (__update_tran_harddrvs(np))
      {
       /* can't eval (relax) tran channels until all hard drivers known */
       /* so first relax can be done */
       __eval_tran_bits(np);
      }
     break;
    }
   if (np->nrngrep == NX_DWIR) __sched_1mdrwire(np);
   else __assign_1mdrwire(np);
   break;
  case LSB:
  case PARTSEL:
   idndp = lhsx->lu.x;
   if (idndp->optyp == GLBREF) goto mdr_glb; else goto mdr_id;
  case LCB:
   /* must treat all wires in lhs multi-fan-in concat as special */ 
   /* this means every lhs element needs re-eval of rhs */
   for (catndp = lhsx->ru.x; catndp != NULL; catndp = catndp->ru.x)
    __mdr_assign_or_sched(catndp->lu.x);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 if (nd_itpop) __pop_itstk();
}

/*
 * assign to one entire wire with multiple fan-in
 *
 * it can have strength (or not) but delayed wire handled elsewhere
 * cannot be either inout iconn or inout port wire
 * called in lhs itree loc. of np (wire definition)
 */
extern void __assign_1mdrwire(register struct net_t *np)
{
 register struct xstk_t *xsp;
 register byte *sbp;
 int32 sbi;
 byte *abp;
 word32 *app, *bpp;

 /* separate routines for rare multi-fi wires with delay */
 /* here since schedule lhs changed always off */
 sbp = NULL;
 /* first most common strength case since know muliple drive */
 if (np->n_stren)
  {
   /* get new value of wire from combination of all drivers */
   xsp = __stload_mdrwire(np);
   sbp = (byte *) xsp->ap;

   /* notice any force overrides even tri reg */
   if (np->frc_assgn_allocated
    && !__correct_forced_newwireval(np, xsp->ap, xsp->bp))
    { __pop_xstk(); return; }
   /* short circuit chg store into strength wire */
   get_stwire_addr_(abp, np);

   /* no delay form trireg wire, correct for same value cap. strength */
   if (np->ntyp == N_TRIREG)
    {
     for (sbi = 0; sbi < np->nwid; sbi++)
      {
       if (sbp[sbi] == ST_HIZ)
        sbp[sbi] = (byte) ((abp[sbi] & 3) | __cap_to_stren[np->n_capsiz]);
      }
    }
   if (memcmp(abp, sbp, np->nwid) != 0)
    {
     memcpy(abp, sbp, np->nwid);
     __lhs_changed = TRUE;
    }
  }
 else
  {
   /* multiple driver non strength no delay case */
   /* xsp is new wire value */
   /* load address and store value use current (np lhs) itree place */
   xsp = __load_mdrwire(np);

   if (np->frc_assgn_allocated
    && !__correct_forced_newwireval(np, xsp->ap, xsp->bp))
    { __pop_xstk(); return; }

   /* notice this address because of packing cannot be stored into */
   __ld_addr(&app, &bpp, np);
   if (np->nwid <= WBITS)
    { if (app[0] == xsp->ap[0] && bpp[0] == xsp->bp[0]) goto done; }
   else
    {
     if (cmp_vval_(app, xsp->ap, np->nwid) == 0
      && cmp_vval_(bpp, xsp->bp, np->nwid) == 0) goto done;
    }
   __st_val(np, xsp->ap, xsp->bp); 
   __lhs_changed = TRUE;
  }

done:
 if (__ev_tracing)
  {
   char s1[RECLEN];

   if (np->n_stren) strcpy(s1, " strength"); else strcpy(s1, "");
   __tr_msg(" drivers of%s wire %s combined", s1, np->nsym->synam);
   if (__lhs_changed)
    {
     if (np->n_stren) __st_regab_tostr(__xs, sbp, np->nwid); 
     else __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE);
     __tr_msg(" and assigned value %s\n", __xs);
    }
   else __tr_msg(" value unchanged\n"); 
  }
 if (__lhs_changed) record_nchg_(np);
 __pop_xstk();
}

/*
 * sched assign for one entire wire with multiple fan-in
 * that has delay and be strength (or not)
 * but cannot be either inout iconn or inout port wire
 *
 * if forced, still must schedule because by assign force may be off later 
 */
extern void __sched_1mdrwire(register struct net_t *np)
{
 register struct xstk_t *xsp;
 register byte *sbp;

 /* separate routines for rare multi-fi wires with delay */
 /* here since schedule lhs changed always off */
 sbp = NULL;
 if (np->n_stren)
  {
   /* this must run in location of ref. not dest. target */
   xsp = __stload_mdrwire(np);
   sbp = (byte *) xsp->ap;
   /* but rest for xmr must store into dest. target */
   /* if np is trireg this will handle (decays known from sbp z's) */ 
   if (np->nu.rngdwir->n_delrep == DT_PTHDST)
    __pth_stren_schd_allofwire(np, sbp, np->nwid);
   else __wdel_stren_schd_allofwire(np, sbp, np->nwid);
  } 
 else
  {
   xsp = __load_mdrwire(np);
   if (np->nu.rngdwir->n_delrep == DT_PTHDST)
    __pth_schd_allofwire(np, xsp->ap, xsp->bp, np->nwid);
   else __wdel_schd_allofwire(np, xsp->ap, xsp->bp, np->nwid);
  }
 if (__ev_tracing)
  {
   char s1[RECLEN];

   if (np->n_stren) strcpy(s1, " strength"); else strcpy(s1, "");
   __tr_msg(" drivers of%s wire %s combined", s1, 
    np->nsym->synam);
   if (np->n_stren) __st_regab_tostr(__xs, sbp, np->nwid); 
   else __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE);
   __tr_msg(" and scheduled to value %s\n", __xs);
  }
 __pop_xstk();
}

/*
 * ROUTINES TO LOAD AND COMBINE NON STRENGTH MULTI DRIVER WIRE
 */

/* 
 * load entire wire with at least some bits of multiple fan-in
 * onto stack - leaves value on top of stack that caller must pop
 *
 * this is for non strength case 
 * for xmr the drivers are drivers for wire target wire
 * loads only hard drivers - tran channel code handles tran components
 */
extern struct xstk_t *__load_mdrwire(register struct net_t *np)
{
 register struct net_pin_t *npp;
 register struct xstk_t *xsp;

 /* allocate accumulator with undriven bits set to z */
 push_xstk_(xsp, np->nwid);
 /* initialize to high z - only possibility in non strength case */
 zero_allbits_(xsp->ap, np->nwid);
 one_allbits_(xsp->bp, np->nwid);

 /* must reevaluate every driver since no algorithm for undoing */
 /* 1 driver to add new one even if could store old value somewhere */
 for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
  {
   /* filter new rooted and up. rel col. forms */ 
   if (npp->npproctyp == NP_PROC_FILT
    && npp->npaux->npu.filtitp != __inst_ptr) continue;
   ldcomb_driver(xsp, np, npp);
  }
 return(xsp);
}

/*
 * load and combine in to passed z initial acumulator one npp driver
 *
 * filters for downward relative xmrs if npp does not match this inst.
 * know definition itree loc on itstk
 */
static void ldcomb_driver(struct xstk_t *acc_xsp, struct net_t *np,
 register struct net_pin_t *npp)
{
 register struct xstk_t *xsp, *tmpxsp;
 int32 lhswid, nd_itpop;
 struct npaux_t *npauxp;

 /* --- DBG remove
 if (__debug_flg && __ev_tracing)
  {
   __tr_msg("## loading %s driver of wire %s\n",
    __to_npptyp(__xs, npp), np->nsym->synam);
  }
 --- */
 if (npp->npproctyp != NP_PROC_INMOD)
  {
   /* SJM 04/17/03 - if XMR does not match - do not combine in */
   if (!__move_to_npprefloc(npp)) return;
   nd_itpop = TRUE;
  }
 else nd_itpop = FALSE;

 /* this puts the driving value (normal rhs with rhs width) on stack */
 /* notice tran strength only so error if appears here */
 switch ((byte) npp->npntyp) {
  case NP_CONTA: xsp = __ld_conta_driver(npp); break;
  case NP_TFRWARG: xsp = __ld_tfrwarg_driver(npp); break;
  case NP_VPIPUTV: xsp = __ld_vpiputv_driver(npp); break;
  case NP_GATE: xsp = __ld_gate_driver(npp); break;
  /* these are up to highconn output port drivers */
  case NP_ICONN: xsp = __ld_iconn_up_driver(npp); break;
  case NP_PB_ICONN: xsp = __ld_pb_iconn_up_driver(npp); break;
  /* these are down to lowconn input port drivers */
  case NP_MDPRT: xsp = __ld_modport_down_driver(npp); break;
  case NP_PB_MDPRT: xsp = __ld_pb_modport_down_driver(npp); break;
  default: __case_terr(__FILE__, __LINE__); xsp = NULL; 
 }
 if (nd_itpop) __pop_itstk();

 /* if lhs of concatenate, must select section out of rhs value */
 if ((npauxp = npp->npaux) != NULL && npauxp->lcbi1 != -1)
  {
   /* loaded value always matches lhs width exactly with z extension */
   /* if too wide just will not rhs psel from section */
   lhswid = npauxp->lcbi1 - npauxp->lcbi2 + 1;
   push_xstk_(tmpxsp, lhswid);
   __rhspsel(tmpxsp->ap, xsp->ap, npauxp->lcbi2, lhswid); 
   __rhspsel(tmpxsp->bp, xsp->bp, npauxp->lcbi2, lhswid); 

   __eval_wire(acc_xsp->ap, tmpxsp->ap, np, npp);
   __pop_xstk();
  }
 else __eval_wire(acc_xsp->ap, xsp->ap, np, npp);
 __pop_xstk();
}

/*
 * routine to move from xmr definition location of wire npp driver
 * or load net on back to reference location so can get gate/conta
 * driving or conducting value
 *
 * push onto itstk and leave on
 */
extern int32 __move_to_npprefloc(struct net_pin_t *npp)
{
 if (npp->npproctyp == NP_PROC_GREF)
  {
   if (!__match_push_targ_to_ref(npp->np_xmrtyp, npp->npaux->npu.npgrp))
    return(FALSE);
  }
 /* all rooted xmrs here */
 else __push_itstk(npp->npaux->npdownitp);
 return(TRUE);
}

/*
 * load a wire driver for use by count drivers
 * puts the driving value (normal rhs with rhs width) on stack 
 *
 * this is not normal routine - for case where just need one with fi>1 comb.
 * must set returned NULL to all z's
 */
extern struct xstk_t *__ld_wire_driver(register struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 int32 nd_itpop;

 /* for new col or xmr npp forms that need to match inst */
 /* instance filter already done */  
 if (npp->npproctyp != NP_PROC_INMOD)
  {
   /* SJM 04/17/03 - if XMR does not match - do not combine in */
   if (!__move_to_npprefloc(npp)) return(NULL);
   nd_itpop = TRUE;
  }
 else nd_itpop = FALSE;

 /* iconn and mod port may return xsp of nil - caller does not pop */
 switch ((byte) npp->npntyp) {
  case NP_CONTA: xsp = __ld_conta_driver(npp); break;
  case NP_TFRWARG: xsp = __ld_tfrwarg_driver(npp); break;
  case NP_VPIPUTV: xsp = __ld_vpiputv_driver(npp); break;
  case NP_GATE: xsp = __ld_gate_driver(npp); break;
  /* these are up to highconn output port drivers */
  case NP_ICONN: xsp = __ld_iconn_up_driver(npp); break;
  case NP_PB_ICONN: xsp = __ld_pb_modport_down_driver(npp); break;
  /* these are down to lowconn input port drivers */
  case NP_MDPRT: xsp = __ld_modport_down_driver(npp); break;
  case NP_PB_MDPRT: xsp = __ld_pb_modport_down_driver(npp); break;
  /* tran strength only */
  default: __case_terr(__FILE__, __LINE__); return(NULL);
 }
 if (nd_itpop) __pop_itstk();
 return(xsp);
}

/*
 * load a continuous assignment driver npp
 * caller must have moved to right itree loc. for xmr form
 *
 * if rhs conta concat causes sep into bits, this is PB conta
 */
extern struct xstk_t *__ld_conta_driver(struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 register int32 blen;
 register struct conta_t *cap;
 int32 orhslen;

 /* SJM 09/18/02 - no separate per bit NP type, check for pb sim on */
 cap = npp->elnpp.ecap; 
 if (cap->ca_pb_sim)
  {
   cap = &(cap->pbcau.pbcaps[npp->pbi]);
  }

 blen = cap->lhsx->szu.xclen;
 /* case 1: fi=1 - for showvars driver is wire section [nbi1:nbi2] itself */
 /* for fi == 1 and no delay need this for wire display */
 /* will never get here normally since just assign */
 /* accessing rhs for consistency check but lhs should always be same */ 
 if (cap->ca_drv_wp.wp == NULL)
  {
   xsp = __eval2_xpr(cap->rhsx);
   if (blen != xsp->xslen)
    {
     orhslen = xsp->xslen;

     /* SJM 09/29/03 - change to handle sign extension and separate types */
     if (xsp->xslen > blen) __narrow_sizchg(xsp, blen);
     else if (xsp->xslen < blen)
      {
       if (cap->rhsx->has_sign) __sgn_xtnd_widen(xsp, blen);
       else __sizchg_widen(xsp, blen);
      }

     /* LOOKATME - only strength continuous assignments widen to z */
     /* all others widen to 0 unless during initialization when x */
     /* ??? if (orhslen < xsp->xslen) __fix_widened_tozs(xsp, orhslen); */
     /* SJM 05/10/04 - widening to x's eliminates need for sign difference */
     if (__wire_init) __fix_widened_toxs(xsp, orhslen);
    }
   return(xsp);
  }
 /* case 2: multi-fi or delay - know driver rhs exists */
 /* but if delay 0 no scheduled (since same) */
 push_xstk_(xsp, blen);
 /* know this is exactly lhs width */
 __ld_perinst_val(xsp->ap, xsp->bp, cap->ca_drv_wp, blen); 
 return(xsp);
}

/*
 * load a tf_ task/func argument rw parameter driver
 *
 * caller must have moved to right itree loc. for xmr form
 * never strength
 */
extern struct xstk_t *__ld_tfrwarg_driver(struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 register int32 blen;
 register struct tfrec_t *tfrp;
 struct tfarg_t *tfap;
 struct expr_t *xp;

 tfrp = npp->elnpp.etfrp;
 tfap = &(tfrp->tfargs[npp->obnum]);
 xp = tfap->arg.axp;
 blen = xp->szu.xclen;
 if (tfap->tfdrv_wp.wp == NULL)
  {
   xsp = __eval2_xpr(xp);
   /* here because lhs is width self determing should never differ */
   /* DBG remove */
   if (blen != xsp->xslen) __misc_terr(__FILE__, __LINE__);
   /* -- */ 
   return(xsp);
  }
 /* case 2: multi-fi */
 push_xstk_(xsp, blen);
 /* know this is exactly lhs r/w arg width */
 __ld_perinst_val(xsp->ap, xsp->bp, tfap->tfdrv_wp, blen); 
 return(xsp);
}

/*
 * load a vpi put value wire driver (non strength case)
 *
 * always entire wire stored (bits with no drivers will be z)
 * obnum is index into possibly multiple drivers
 *
 * for the net bit case must load entire (allowing sharing of driver storage
 * for per bit) then select out the needed bit which is later combined in
 *
 * if any bits unused z's will never be seen
 */
extern struct xstk_t *__ld_vpiputv_driver(struct net_pin_t *npp)
{
 register struct net_t *np;
 register struct xstk_t *xsp;
 register struct vpi_drv_t *drvp;
 int32 bi;

 np = npp->elnpp.enp;
 drvp = np->vpi_ndrvs[npp->obnum];
 push_xstk_(xsp, np->nwid);
 __ld_perinst_val(xsp->ap, xsp->bp, drvp->vpi_drvwp, np->nwid); 
 if (npp->npaux != NULL && (bi = npp->npaux->nbi1) != -1)
  {
   word32 av, bv;
    
   av = rhsbsel_(xsp->ap, bi);
   bv = rhsbsel_(xsp->bp, bi);
   __pop_xstk();
   push_xstk_(xsp, 1);
   xsp->ap[0] = av;
   xsp->bp[0] = bv;
  }
 return(xsp);
}

/*
 * load a gate driver npp - also for strength case
 * caller must have moved to right itree loc. for xmr form
 */
extern struct xstk_t *__ld_gate_driver(struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 register word32 uwrd;
 int32 has_stren;

 /* load output port - must remove strengths since wire is non strength */
 push_xstk_(xsp, 1);
 uwrd = __ld_gate_out(npp->elnpp.egp, &has_stren);
 /* this must load value and remove strength since drives constant */
 /* notice this is non stren case - stren passing gates not proc. here */
 xsp->ap[0] = uwrd & 1L;
 xsp->bp[0] = (uwrd >> 1) & 1L;
 return(xsp);
}

/*
 * load a gate or udp output - value determined from gate type
 * and set has_stren and return stren byte value if gate drives st.
 * some gates always drive varying stren or some have constant non
 * (st0,st1) strength
 *
 * this is passed net pin since need pin number for trans
 */
extern word32 __ld_gate_out(register struct gate_t *gp, int32 *has_stren)
{
 register word32 wrd, uwrd;
 register int32 nins;
 struct udp_t *udpp;

 /* here just using a part of stack */ 
 *has_stren = FALSE;
 nins = gp->gpnum - 1;
 switch ((byte) gp->g_class) {
  case GC_UDP:
   udpp = gp->gmsym->el.eudpp;
   if (!udpp->u_wide)
    wrd = (((word32) (gp->gstate.hwp[__inum])) >> (2*nins)) & 3L;
   else wrd = (gp->gstate.wp[2*__inum] >> (2*nins)) & 3L;
   /* must or in driving strength - if (st0,st1) already removed */
   /* key here is that know wire driven is n_stren */
   /* state here does not have strength */
adjust_stren:
   if (gp->g_hasst)
    {
     /* z value does not have strength */
     if (wrd != 2) wrd |= (gp->g_stval << 2L);
     wrd = (word32) __stren_map_tab[wrd];
     *has_stren = TRUE;
    }
   break;
  case GC_LOGIC:
   if (nins > 15) wrd = widegate_ld_bit(gp->gstate.wp, nins + 1, nins);
   else
    {
     wrd = get_packintowrd_(gp->gstate, __inum, nins + 1);
     wrd = ((wrd >> nins) & 1L) | (((wrd >> (2*nins + 1)) & 1L) << 1);
    }
   if (gp->g_hasst) goto adjust_stren;
   break;
  case GC_BUFIF:
   uwrd = (word32) gp->gstate.hwp[__inum];
   wrd = (uwrd >> 4) & 0xff;
   *has_stren = TRUE;
  break;
  case GC_MOS: 
   /* state here has strength */
   wrd = (gp->gstate.wp[__inum] >> 16) & 0xffL;
   *has_stren = TRUE;
   break;
  case GC_CMOS:
   wrd = (gp->gstate.wp[__inum] >> 24) & 0xffL;
   *has_stren = TRUE;
   break;
  default: __case_terr(__FILE__, __LINE__); wrd = 0;
 }
 return(wrd);
}

/*
 * load a gate or udp input - value determined from gate type
 * and set has_stren and return stren byte value if input has strength
 *
 * pin number pi starts from 1 for first input since output is 0
 */
extern word32 __ld_gate_in(struct gate_t *gp, int32 pi, int32 *has_stren)
{
 register word32 wrd, uwrd, tmp;
 register int32 nins;
 struct udp_t *udpp;

 /* here just using a part of stack */ 
 *has_stren = FALSE;
 nins = gp->gpnum - 1;
 wrd = 0;
 switch ((byte) gp->g_class) {
  case GC_UDP:
   udpp = gp->gmsym->el.eudpp;
   if (!udpp->u_wide) uwrd = (word32) (gp->gstate.hwp[__inum]);
   else uwrd = gp->gstate.wp[2*__inum];
   wrd = (uwrd >> (pi - 1)) & 1;
   tmp = (uwrd >> (nins + pi - 1)) & 1;
   wrd |= (tmp << 1);
   break;
  case GC_LOGIC:
   if (nins > 15) wrd = widegate_ld_bit(gp->gstate.wp, nins + 1, pi - 1);
   else
    {
     wrd = get_packintowrd_(gp->gstate, __inum, nins + 1);
     wrd = ((wrd >> (pi - 1)) & 1L) | (((wrd >> (nins + pi - 1)) & 1L) << 1);
    }
   break;
  case GC_BUFIF:
   uwrd = (word32) gp->gstate.hwp[__inum];
   if (pi == 1) wrd = (uwrd & 3);
   else if (pi == 2) wrd = (uwrd >> 2) & 3;
   else __case_terr(__FILE__, __LINE__);
   break;
  case GC_MOS: 
   /* state here has strength */
   uwrd = gp->gstate.wp[__inum];
   if (pi == 1) { *has_stren = TRUE; wrd = (uwrd & 0xff); }
   else if (pi == 2) wrd = ((uwrd >> 8) & 3);
   else __case_terr(__FILE__, __LINE__);
   break;
  case GC_CMOS:
   /* this has one stren data input and 2 control inputs */
   uwrd = gp->gstate.wp[__inum];
   if (pi == 1) { *has_stren = TRUE; wrd = uwrd & 0xff; }
   else if (pi == 2) wrd = (wrd >> 8) & 3;
   else if (pi == 3) wrd = (wrd >> 16) & 3;
   else __case_terr(__FILE__, __LINE__);
   break;
  default: __case_terr(__FILE__, __LINE__); wrd = 0;
 }
 return(wrd);
}

/* 
 * load the output bit for wide gate
 * format for wide gate is a part in one word32 group, b in other  
 * bit 0 is 0 (inputs [0:nins - 1]), output is bit nins
 */
static word32 widegate_ld_bit(word32 *gsp, int32 gwid, int32 biti)
{
 int32 wlen;
 register word32 av, bv, *rap;

 /* rap is start of instance coded vector a b groups */
 wlen = wlen_(gwid);
 rap = &(gsp[2*wlen*__inum]);
 av = rhsbsel_(rap, biti);
 bv = rhsbsel_(&(rap[wlen]), biti);
 return(av | (bv << 1));
}

/*
 * load a iconn (down module port rhs to up iconn lhs) driver npp
 * caller must have moved to right itree loc. for xmr form
 *
 * driver is down module output port (inout handled in switch channel) 
 * called from up iconn itree location
 */
extern struct xstk_t *__ld_iconn_up_driver(register struct net_pin_t *npp)
{
 register struct mod_pin_t *mpp;
 register struct expr_t *xlhs;
 register struct itree_t *itp;
 int32 orhslen;
 struct xstk_t *xsp;
 struct mod_t *downmdp;

 itp = &(__inst_ptr->in_its[npp->elnpp.eii]);
 downmdp = itp->itip->imsym->el.emdp;
 mpp = &(downmdp->mpins[npp->obnum]);
 /* notice this can never be xmr */
 __push_itstk(itp);
 xsp = __eval2_xpr(mpp->mpref);

 /* lvalue is iconn pos. port number */ 
 xlhs = itp->itip->ipins[npp->obnum];
 /* needed iconn connection width may differ from port width */
 if (xlhs->szu.xclen != xsp->xslen)
  {
   orhslen = xsp->xslen;

   /* SJM 09/29/03 - change to handle sign extension and separate types */
   if (xsp->xslen > xlhs->szu.xclen)
    __narrow_sizchg(xsp, xlhs->szu.xclen);
   else if (xsp->xslen < xlhs->szu.xclen)
    {
     if (mpp->mpref->has_sign) __sgn_xtnd_widen(xsp, xlhs->szu.xclen);
     else __sizchg_widen(xsp, xlhs->szu.xclen);
    }

   /* LOOKATME - only strength continuous assignments widen to z */
   /* all others widen to 0 */
   /* ??? if (orhslen < xlhs->szu.xclen) __fix_widened_tozs(xsp, orhslen); */
   /* SJM 05/10/04 - widening to x's eliminates need for sign difference */
   if (__wire_init) __fix_widened_toxs(xsp, orhslen);
  }
 __pop_itstk();
 return(xsp);
}

/*
 * load per bit iconn (down module port rhs to up iconn lhs) driver npp
 * caller must have moved to right itree loc. if xmr form
 *
 * driver is down module output port (inouts handled in tran channels)
 * called from up iconn itree location
 */
extern struct xstk_t *__ld_pb_iconn_up_driver(register struct net_pin_t *npp)
{
 register struct mod_pin_t *mpp;
 register struct itree_t *itp;
 struct xstk_t *xsp;
 struct mod_t *downmdp;

 itp = &(__inst_ptr->in_its[npp->elnpp.eii]);
 downmdp = itp->itip->imsym->el.emdp;
 mpp = &(downmdp->mpins[npp->obnum]);
 mpp = &(mpp->pbmpps[npp->pbi]);
 /* notice this can never be xmr */
 __push_itstk(itp);
 xsp = __eval2_xpr(mpp->mpref);
 __pop_itstk();

 /* lvalue is iconn pos. port number */ 
 /* since per bit, never need size convert */
 /* DBG remove --
 {
  struct expr_t *xlhs;

  xlhs = itp->itip->pb_ipins_tab[npp->obnum][npp->pbi];
  if (mpp->mpref->szu.xclen != xsp->xslen) __misc_terr(__FILE__, __LINE__);
 }
 --- */

 return(xsp);
}

/*
 * load a mod. port (up iconn rhs to down mod port lhs) driver npp
 *
 * xmr form impossible here
 * driver is up iconn rhs connection for input port
 * called from down module itree location
 */
extern struct xstk_t *__ld_modport_down_driver(register struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 register struct mod_pin_t *mpp;
 int32 orhslen;
 struct itree_t *itp;
 struct mod_t *downmdp;
 struct expr_t *xlhs, *up_rhsx;

 itp = __inst_ptr; 
 /* --- DBG remove
 if (itp->up_it == NULL) __misc_terr(__FILE__, __LINE__);
 --- */
 downmdp = itp->itip->imsym->el.emdp;

 /* instance of lhs has module type that provides port lhs expr. */
 up_rhsx = itp->itip->ipins[npp->obnum];
 /* but variables in ndp come from inside up instance's module */
 __push_itstk(itp->up_it);
 /* this may access from different itree place if iconn xmr */
 xsp = __eval2_xpr(up_rhsx);

 /* up iconn width may differ from port width */
 mpp = &(downmdp->mpins[npp->obnum]);
 xlhs = mpp->mpref;
 if (xlhs->szu.xclen != xsp->xslen)
  {
   orhslen = xsp->xslen;
   /* SJM 09/29/03 - change to handle sign extension and separate types */
   if (xsp->xslen > xlhs->szu.xclen)
    __narrow_sizchg(xsp, xlhs->szu.xclen);
   else if (xsp->xslen < xlhs->szu.xclen)
    {
     if (up_rhsx->has_sign) __sgn_xtnd_widen(xsp, xlhs->szu.xclen);
     else __sizchg_widen(xsp, xlhs->szu.xclen);
    }

   /* LOOKATME - only strength continuous assignments widen to z */
   /* all others widen to 0 */
   /* ?? if (orhslen < xlhs->szu.xclen) __fix_widened_tozs(xsp, orhslen); */
   /* SJM 05/10/04 - widening to x's eliminates need for sign difference */
   if (__wire_init) __fix_widened_toxs(xsp, orhslen);
  }
 __pop_itstk();
 return(xsp); 
}

/*
 * load per bit mod. port (up iconn rhs to down mod port lhs) driver npp
 * for input ports where high conn is concat
 *
 * xmr form impossible here
 * driver of this down iput port is up highconn iconn rhs expr
 * called from down module itree location
 */
extern struct xstk_t *__ld_pb_modport_down_driver(
 register struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 register struct itree_t *itp;
 struct expr_t *up_rhsx;

 itp = __inst_ptr; 
 /* --- DBG remove
 if (itp->up_it == NULL) __misc_terr(__FILE__, __LINE__);
 --- */

 /* instance of lhs has module type that provides port lhs expr. */
 up_rhsx = itp->itip->pb_ipins_tab[npp->obnum][npp->pbi];
 /* but variables in ndp come from inside up instance's module */

 __push_itstk(itp->up_it);
 /* this may access from different itree place if iconn xmr */
 /* FIXME ??? - since up rhs decomposes into 1 bit, this can be faster */
 xsp = __eval2_xpr(up_rhsx);
 __pop_itstk();

 /* since per bit, never need size convert */
 /* DBG remove --
 {
  struct mod_pin_t *mpp;
  struct mod_t *downmdp;

  downmdp = itp->itip->imsym->el.emdp;
  mpp = &(downmdp->mpins[npp->obnum]);
  mpp = &(mpp->pbmpps[npp->pbi]);
  if (mpp->mpref->szu.xclen != xsp->xslen) __misc_terr(__FILE__, __LINE__);
 }
 --- */

 return(xsp); 
}

/*
 * ROUTINES TO COMBINE DRIVERS USING NON-STRENGTH RULES
 */

/*
 * apply values from npp npcxsp to a/b accumulator for net pin npp
 *
 * this is for non strength case
 * wire here is used for type - does not need itree place
 * can pass entire wire since section evaled determined by passed npp
 *
 * SJM 11/13/02 - not handling IS -2 per inst param range select right
 * SJM 11/15/02 - need to pass the a/b word32 ptr so can call from compiled
 *
 * FIXME - think this can never happen
 */
extern void __eval_wire(word32 *acc_wp, word32 *drv_wp, struct net_t *np,
 struct net_pin_t *npp)
{
 register struct npaux_t *npauxp;
 word32 resa, resb, *wp;
 int32 wlen, pselwid, i1, i2;
 struct xstk_t *tmpxsp;

 wlen = wlen_(np->nwid);
 /* entire wire */
 if ((npauxp = npp->npaux) == NULL || npauxp->nbi1 == -1)
  {
   if (np->nwid > WBITS)
    {
     __eval_wide_wire(acc_wp, &(acc_wp[wlen]), drv_wp, &(drv_wp[wlen]),
      np->nwid, np->ntyp);
    } 
   else __eval_1w_nonstren(acc_wp, &(acc_wp[1]), drv_wp[0], drv_wp[1],
    np->ntyp);
   return;
  }

 /* know that driver is section of lhs wire - i.e. lhs select decl assign */
 /* know npaux field exists */
 if (npauxp->nbi1 == -2)
  {
   /* SJM 10/12/04 - because contab realloced, must be ndx base of IS */
   wp = &(__contab[npauxp->nbi2.xvi]);
   wp = &(wp[2*__inum]);
   i1 = i2 = (int32) wp[0];
  }
 else { i1 = npauxp->nbi1; i2 = npauxp->nbi2.i; }
 if (i1 == i2) 
  {
   resa = rhsbsel_(acc_wp, i1);
   resb = rhsbsel_(&(acc_wp[wlen]), i1);
   /* know thing driving wire is 1 bit - will eval. to 1 bit */
   __eval_1w_nonstren(&resa, &resb, drv_wp[0], drv_wp[1], np->ntyp);
   /* assign back to bit */
   __lhsbsel(acc_wp, i1, resa);
   __lhsbsel(&(acc_wp[wlen]), i1, resb);
   return;
  }

 /* part select inefficient but think still better than if done bit by bit */
 /* part select - net pin range is normalized */
 pselwid = i1 - i2 + 1; 
 /* load accumulator region into tmp xsp */ 
 push_xstk_(tmpxsp, pselwid);
 __rhspsel(tmpxsp->ap, acc_wp, i2, pselwid); 
 __rhspsel(tmpxsp->bp, &(acc_wp[wlen]), i2, pselwid); 

 /* eval. - notice if fits in 1 word32 do not need width */
 if (pselwid > WBITS)
  __eval_wide_wire(tmpxsp->ap, tmpxsp->bp, drv_wp, &(drv_wp[wlen_(pselwid)]),
   pselwid, np->ntyp);
 /*11/21/2002 AIV was nwid for the last arg should be type*/
 else __eval_1w_nonstren(tmpxsp->ap, tmpxsp->bp, drv_wp[0], drv_wp[1],
  np->ntyp);

 /* store back into accumulator */
 __lhspsel(acc_wp, i2, tmpxsp->ap, pselwid); 
 __lhspsel(&(acc_wp[wlen]), i2, tmpxsp->bp, pselwid); 
 __pop_xstk();
}

/*
 * evaluate stack elements into depending on wire type
 * any selection handled above here rxsp and xsp1 can be same
 * since all have same width here, no need to remove unused bits
 *
 * SJM 11/15/02 - change so pass a/b word32 ptrs so compiled code can call
 */
extern void __eval_wide_wire(word32 *acc_ap, word32 *acc_bp,
 word32 *drv_ap, word32 *drv_bp, int32 opbits, word32 wtyp)
{
 register int32 wi;

 /* DBG remove - only these can have multi-fi and no strength */
 /* tri0, tri1 and trireg always strength -- 
 switch ((byte) wtyp) {
  case N_WIRE: case N_TRI: case N_TRIAND: case N_WA: case N_TRIOR: case N_WO:
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 --- */
 for (wi = 0; wi < wlen_(opbits); wi++)
  {
   __eval_1w_nonstren(&(acc_ap[wi]), &(acc_bp[wi]), drv_ap[wi],
    drv_bp[wi], wtyp);
  }
}

/*
 * evaluate a 1 word32 normal tri wire
 * notice res and op can be same object since op by value
 * for non strength case if one z, use other, if both z leave as z 
 * since fits in one bit do not need width - high bits will just be 0
 *
 * eval is word32 by word32 then after stren competition, used section
 * extracted with select if needed
 *
 * LOOKATME - think this can never happen
 */
extern void __eval_1w_nonstren(register word32 *resa, register word32 *resb,
 register word32 op2a, register word32 op2b, word32 wtyp)
{
 register word32 zmask, donemask;
 register word32 op1a, op1b;

 op1a = resa[0];
 op1b = resb[0];
 /* first all bits that are the same - common case */
 /* notice wand and wor same bits always same */
 /* unused bits handled here - since will both be 0 */
 /* mask has 1 if same else 0 */
 zmask = ~((op1a ^ op2a) | (op1b ^ op2b));
 resa[0] = op1a & zmask;
 resb[0] = op1b & zmask;
 donemask = zmask;
 if (donemask == __masktab[0]) goto done;

 /* next op1 z bits, use op2 bits */
 zmask = (op1a ^ op1b) & op1b; 
 /* if z in op1, value is op2 */
 resa[0] |= (op2a & zmask);
 resb[0] |= (op2b & zmask);
 donemask |= zmask;
 if (donemask == __masktab[0]) goto done;

 /* next op2 zbits, use op1 bits */
 zmask = (op2a ^ op2b) & op2b; 
 /* if z in op2, value is op2 */
 resa[0] |= op1a & zmask;
 resb[0] |= op1b & zmask;
 donemask |= zmask; 
 if (donemask == __masktab[0]) goto done;

 zmask = ~donemask;
 /* finally net type determines algorithm - know b bit 0 for these bits */
 switch ((byte) wtyp) {
  case N_WIRE: case N_TRI:
   /* know remaining bits must be x, neither and not the same */
   resa[0] |= zmask; resb[0] |= zmask;
   break;
  case N_TRIAND: case N_WA:
   /* if either 0, result 0, else x, since 1 1 done */ 
   resa[0] |= ((op1a | op1b) & (op2a | op2b)) & zmask;
   resb[0] |= (resa[0] & (op1b | op2b)) & zmask;
   break;  
  case N_TRIOR: case N_WO:
   resb[0]
    |= ((op2b ^ op1b ^ ((op1a | op1b) & (op2b | (op2a & op1b)))) & zmask);
   resa[0] |= ((resb[0] | op2a | op1a) & zmask);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
done:
 /* DBG --- */
 if (__debug_flg && __ev_tracing) 
  {
   char s1[RECLEN];

   __tr_msg(
    "+> fi>1 nonstren: %s op1a=%lx,op1b=%lx,op2a=%lx,op2b=%lx,resa=%lx,resb=%lx,zmask=%lx\n", 
    __to_wtnam2(s1, wtyp), op1a, op1b, op2a, op2b, resa[0], resb[0], zmask);
  }
 /* --- */
}

/*
 * MULTI FAN IN STRENGTH WIRE ASSIGN ROUTINES
 * ALL STRENGTH ASSIGN THROUGH HERE
 */

/*
 * load an entire strength wire value onto top of stack
 * by evaluating all drivers
 *
 * for normal wire itstk must be lhs wire place - for mod. in this is down
 * (rhs up) and for inst. output this is up (rhs down) 
 * for xmr wire current itree place is target (define) inst of wire
 *
 * the driver evaluation may change to xmr itree place but always restores
 *
 * loads only hard drivers - tran channel code handles tran components
 */
extern struct xstk_t *__stload_mdrwire(struct net_t *np)
{
 register struct net_pin_t *npp;
 register struct xstk_t *xsp;
 register byte *sbp;

 /* allocate accumulator - initialize all bits to z in case not driven */
 /* notice built in 8 bits per byte but also no b part so need half size */
 xsp = init_stwire_accum(np);
 sbp = (byte *) xsp->ap;

 /* evaluation of every net and channel driver against current accum. value */
 for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
  {
   /* filter new rooted and up. rel col. forms */ 
   if (npp->npproctyp == NP_PROC_FILT && npp->npaux->npu.filtitp != __inst_ptr)
    continue;

   ldcomb_stdriver(sbp, np, npp);
  }
 return(xsp);
}

/*
 * load and combine in to passed z initial acumulator the driver for 1 val
 * may not add if npp does not apply to this inst. or driver off
 */
static void ldcomb_stdriver(register byte *acc_sbp, struct net_t *np,
 register struct net_pin_t *npp)
{
 register byte *sbp2;
 register int32 i1;
 register struct xstk_t *xsp;
 int32 nd_itpop;
 word32 *wp;
 byte *sbp;
 struct npaux_t *npauxp;

 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  {
   __tr_msg("## loading %s strength driver of wire %s\n",
    __to_npptyp(__xs, npp), np->nsym->synam);
  }
 --- */

 /* trace from target (definition location) back to ref. inst. itree loc */
 if (npp->npproctyp != NP_PROC_INMOD)
  {
   /* SJM 04/17/03 - if XMR does not match - do not combine in */
   if (!__move_to_npprefloc(npp)) return;
   nd_itpop = TRUE;
  }
 else nd_itpop = FALSE;

 /* load driver onto new top of stack (this always pushes) */
 /* know rhs driver width will be changed to match lhs width exactly */

 switch ((byte) npp->npntyp) {
  case NP_CONTA: xsp = ld_stconta_driver(npp); break;
  case NP_TFRWARG: xsp = __ld_sttfrwarg_driver(npp); break;
  case NP_VPIPUTV: xsp = ld_stvpiputv_driver(npp); break;
  case NP_GATE: xsp = ld_stgate_driver(npp); break;
  /* these are up to highconn strength output port drivers */
  case NP_ICONN: xsp = ld_sticonn_up_driver(npp); break;
  case NP_PB_ICONN: xsp = ld_pb_sticonn_up_driver(npp); break;
  /* these are down to lowconn strength input port drivers */
  case NP_MDPRT: xsp = ld_stmodport_down_driver(npp); break;
  case NP_PB_MDPRT: xsp = ld_pb_stmodport_down_driver(npp); break;
  case NP_PULL: xsp = ld_stpull_driver(npp); break;
  /* TRAN impossible here */
  default: __case_terr(__FILE__, __LINE__); xsp = NULL;
 }
 if (nd_itpop) __pop_itstk();

 /* SJM 07/08/00 - need high z's if gate driver or narrow conta */
 /* need at least when stren gate output drives vector */ 
 /* SJM 11/11/02 - slightly wrong - works because only needed for gate */
 /* wich never has lhs concat sink */

 /* SJM 05/10/04 - no sign extension because widening to z'x */
 if (xsp->xslen/4 < np->nwid) __strenwiden_sizchg(xsp, np->nwid);

 sbp = (byte *) xsp->ap;

 /* first if this is lhs concat, must isolate relevant from rhs (sbp2) */
 /* all indices normalized here to h:0 */
 if ((npauxp = npp->npaux) != NULL && npauxp->lcbi1 != -1)
  sbp2 = &(sbp[npauxp->lcbi2]);
 else sbp2 = sbp;
 /* first entire wire case */
 if (npauxp == NULL || npauxp->nbi1 == -1)
  {
   eval_stwire(np->ntyp, acc_sbp, np->nwid - 1, 0, sbp2);
   goto done;
  }
 /* IS bit select cases */
 if (npauxp->nbi1 == -2)
  {
   /* SJM 10/12/04 - because contab realloced, must be ndx base of IS */
   wp = &(__contab[npauxp->nbi2.xvi]);
   wp = &(wp[2*__inum]);
   i1 = (int32) wp[0];
   /* here strength competition of sbp2[0] against sbp[i1] accum. */
   eval_stwire(np->ntyp, acc_sbp, i1, i1, sbp2);
   goto done;
  }
 /* --- DBG
 if (__debug_flg) __st_regab_tostr(s1, acc_sbp, np->nwid);
 --- */ 

 /* finally competition of accum sbp[nbi1:nbi2] to low of sbp2 */
 eval_stwire(np->ntyp, acc_sbp, npauxp->nbi1, npauxp->nbi2.i, sbp2);

 /* --- DBG 
 if (__debug_flg)
  {
   int32 ti1, ti2, lci1, lci2;
   char s2[RECLEN], s3[RECLEN];

   if ((npauxp = npp->npaux) != NULL) 
    {
     ti1 = npauxp->nbi1;
     ti2 = npauxp->nbi2.i;
     lci1 = npauxp->lcbi1;
     lci2 = npauxp->lcbi2;
    }
   else ti1 = ti2 = lci1 = lci2 = -1;

   __dbg_msg(
    "## stren driver before %s, after %s,\n  value %s, rhs [%d:%d] lhs [%d:%d]\n",
    s1, __st_regab_tostr(s2, acc_sbp, np->nwid), __st_regab_tostr(s3, sbp2,
    ti1 - ti2 + 1), lci1, lci2, ti1, ti2);
  }
 --- */
done: 
 __pop_xstk();
}

/*
 * initialize a wire for multi-fi combination (depends on wire type)
 * this push value on to expr. stack
 */
static struct xstk_t *init_stwire_accum(struct net_t *np)
{
 register byte *sbp;
 struct xstk_t *xsp;
 byte stval;

 push_xstk_(xsp, 4*np->nwid);
 sbp = (byte *) xsp->ap;

 /* 0,0,2 is high z */
 /* initialize in case unc., */
 switch ((byte) np->ntyp) {
  case N_TRI0: stval = ST_PULL0; goto set_stren;
  case N_TRI1: stval = ST_PULL1; goto set_stren;
  case N_SUPPLY0:
   set_byteval_(sbp, np->nwid, ST_SUPPLY0);
   return(xsp);
  case N_SUPPLY1:
   set_byteval_(sbp, np->nwid, ST_SUPPLY1);
   return(xsp);
  default:
   stval = ST_HIZ;
set_stren:
  set_byteval_(sbp, np->nwid, stval);
 }
 return(xsp);
}

/*
 * load a wire's driven value (maybe not driven z) on to top of stack
 * routines called from here must do stack pushing themselves
 *
 * this is only called by count drivers
 */
extern struct xstk_t *__ld_stwire_driver(register struct net_pin_t *npp)
{
 register struct xstk_t *xsp; 
 int32 nd_itpop;

 /* move from target back to itree loc of ref. */ 
 if (npp->npproctyp != NP_PROC_INMOD)
  {
   /* SJM 04/17/03 - if XMR does not match - do not combine in */
   if (!__move_to_npprefloc(npp)) return(NULL);
   nd_itpop = TRUE;
  }
 else nd_itpop = FALSE;

 /* this puts the driving value (normal rhs with rhs width) on stack */
 /* notice md port or tran or iconn may return nil, caller does not pop */
 switch ((byte) npp->npntyp) {
  case NP_CONTA: xsp = ld_stconta_driver(npp); break;
  case NP_TFRWARG: xsp = __ld_sttfrwarg_driver(npp); break;
  case NP_VPIPUTV: xsp = ld_stvpiputv_driver(npp); break;
  case NP_GATE: xsp = ld_stgate_driver(npp); break;
  /* these are up to highconn strength output port drivers */
  case NP_ICONN: xsp = ld_sticonn_up_driver(npp); break;
  case NP_PB_ICONN: xsp = ld_pb_sticonn_up_driver(npp); break;
  /* these are down to lowconn strength input port drivers */
  case NP_MDPRT: xsp = ld_stmodport_down_driver(npp); break;
  case NP_PB_MDPRT: xsp = ld_pb_stmodport_down_driver(npp); break;
  default: __case_terr(__FILE__, __LINE__); return(NULL);
 }
 if (nd_itpop) __pop_itstk();
 return(xsp);
}

/*
 * load a strength gate driver npp
 * caller must have moved to right itree loc. for xmr form
 */
static struct xstk_t *ld_stgate_driver(struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 register byte *sbp;
 int32 has_stren;

 /* here must add (st0,st0) if no strength */
 push_xstk_(xsp, 4);
 sbp = (byte *) xsp->ap;
 sbp[0] = (byte) __ld_gate_out(npp->elnpp.egp, &has_stren);
 if (!has_stren && sbp[0] != 2) sbp[0] |= (ST_STRVAL << 2); 
 return(xsp);
}

/*
 * load a strength continuous assignment (not port conta) driver npp
 * caller must have moved to right itree loc. for xmr form
 *
 * for rhs concat separated into per bit, this is PB conta el
 */
static struct xstk_t *ld_stconta_driver(struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 register int32 blen;
 register struct conta_t *cap;
 int32 orhslen;
 byte *sbp; 
 struct xstk_t *xsp2;

 /* case 1: fi of 1 - driver if npp range of net itself */
 /* fi == 1 subcase only for driver (show vars) display */
 /* SJM 09/18/02 - no separate per bit NP type, checkfor pb sim on */
 cap = npp->elnpp.ecap;
 if (cap->ca_pb_sim) cap = &(cap->pbcau.pbcaps[npp->pbi]);

 blen = cap->lhsx->szu.xclen;
 /* if fi == 1 and no delay, no driver field driver is size changed rhs */
 /* could access lhs since should be same but this is consistency check */
 /* notice display whole wire with range value printed if cat */
 push_xstk_(xsp, 4*blen); 
 sbp = (byte *) xsp->ap;
 if (cap->ca_drv_wp.wp == NULL)
  {
   xsp2 = __eval2_xpr(cap->rhsx);
   if (blen != xsp2->xslen)
    {
     orhslen = xsp2->xslen;

     /* SJM 09/29/03 - change to handle sign extension and separate types */
     /* SJM 06/20/05 - rare case but needs signed widen (stren maybe added?) */
     if (xsp2->xslen > blen) __narrow_sizchg(xsp2, blen);
     else if (xsp2->xslen < blen)
      {
       if (cap->rhsx->has_sign) __sgn_xtnd_widen(xsp2, blen);
       else __sizchg_widen(xsp2, blen);
      }

     if (__wire_init) __fix_widened_toxs(xsp2, orhslen);
    }
  }
 else
  {
   /* complicated has delay or multi-fi case, driver is saved value */
   /* know width will always be correct lhs */
   /* for multi-fi, must save driver else rhs may re-eval too many times */
   push_xstk_(xsp2, blen);
   __ld_perinst_val(xsp2->ap, xsp2->bp, cap->ca_drv_wp, blen);
  }
 /* add strength */
 __st_standval(sbp, xsp2, cap->ca_stval);
 __pop_xstk();
 return(xsp);
}

/*
 * load a tf_ task/func argument rw parameter strength driver
 * caller must have moved to right itree loc. for xmr form
 */
extern struct xstk_t *__ld_sttfrwarg_driver(struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 register int32 blen;
 register struct tfrec_t *tfrp;
 byte *sbp, *sbp2;
 struct tfarg_t *tfap;
 struct expr_t *xp;

 tfrp = npp->elnpp.etfrp;
 tfap = &(tfrp->tfargs[npp->obnum]);
 xp = tfap->arg.axp;
 blen = xp->szu.xclen;
 /* tf arg fi == 1 case */  
 if (tfap->tfdrv_wp.wp == NULL)
  {
   xsp = __ndst_eval_xpr(xp);
   /* know this is lhs so width loaded must be arg width */
   /* DBG remove */
   if (blen != xsp->xslen/4) __misc_terr(__FILE__, __LINE__);
   /* --- */
  }
 else
  {
   /* tf arg r/w lhs wire has multiple drivers */ 
   push_xstk_(xsp, 4*blen); 
   sbp = (byte *) xsp->ap;
   sbp2 = &(tfap->tfdrv_wp.bp[__inum*blen]);
   memcpy(sbp, sbp2, blen); 
  }
 return(xsp);
}

/*
 * load a putv added terminal strength driver
 *
 * inst. or bit may not have added driver if so will just load z's
 * z's do not hurt and faster to load entire wire
 */
static struct xstk_t *ld_stvpiputv_driver(struct net_pin_t *npp)
{
 register struct net_t *np;
 register struct xstk_t *xsp;
 register struct vpi_drv_t *drvp;
 register byte *sbp, *sbp2;
 int32 bi;

 np = npp->elnpp.enp;
 drvp = np->vpi_ndrvs[npp->obnum];
 if (npp->npaux != NULL && (bi = npp->npaux->nbi1) != -1)
  {
   push_xstk_(xsp, 4);
   sbp = (byte *) xsp->ap;
   sbp[0] = drvp->vpi_drvwp.bp[np->nwid*__inum + bi];
   return(xsp);
  }
 /* SJM 08/11/00 - was wrongly not 4 times width so widen over-wrote hiz */
 push_xstk_(xsp, 4*np->nwid);
 sbp = (byte *) xsp->ap;
 sbp2 = &(drvp->vpi_drvwp.bp[np->nwid*__inum]);
 memcpy(sbp, sbp2, np->nwid);
 return(xsp);
}

/*
 * load a strength iconn (down module port rhs to up iconn lhs) driver npp
 * caller must have moved to right itree loc. for xmr form
 *
 * called from up iconn itree location
 */
static struct xstk_t *ld_sticonn_up_driver(register struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 register struct mod_pin_t *mpp;
 register struct expr_t *xlhs;
 struct itree_t *itp;
 struct mod_t *downmdp;

 /* assign from down module port rhs into up iconn lhs expr. */
 /* driver is down module port - called with itree location of up */
 itp = &(__inst_ptr->in_its[npp->elnpp.eii]);
 downmdp = itp->itip->imsym->el.emdp;
 mpp = &(downmdp->mpins[npp->obnum]);
 xlhs = itp->itip->ipins[npp->obnum];

 /* this must load preserving strengths - here know mod. port has stren */
 /* inout port output half driver is always port value */
 __push_itstk(itp);
 /* if down rhsx is reg, will add stron strength */ 
 xsp = __ndst_eval_xpr(mpp->mpref);

 /* only if rhs too narrow, need to add in HIZ and maybe widen alloc area */
 /* if too wide just ignores high bytes */
 if (xlhs->szu.xclen > mpp->mpref->szu.xclen)
  {
   /* SJM 05/10/04 - no sign extension because widening to z'x */
   __strenwiden_sizchg(xsp, xlhs->szu.xclen);
  }
 __pop_itstk();
 return(xsp);
}

/*
 * load per bit stren iconn (down module port rhs to up iconn lhs) driver
 *
 * driver is down module output port
 * called from up iconn itree location
 * caller must have moved to right itree loc. for xmr form
 */
static struct xstk_t *ld_pb_sticonn_up_driver(register struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 register struct mod_pin_t *mpp;
 struct itree_t *itp;
 struct mod_t *downmdp;

 /* assign from down module port rhs into up iconn lhs expr. */
 /* driver is down module port - called with itree location of up */
 itp = &(__inst_ptr->in_its[npp->elnpp.eii]);
 downmdp = itp->itip->imsym->el.emdp;
 mpp = &(downmdp->mpins[npp->obnum]);
 mpp = &(mpp->pbmpps[npp->pbi]);

 /* this must load preserving strengths - here know mod. port has stren */
 /* inout port output half driver is always port value */
 __push_itstk(itp);
 /* if down rhsx is reg, will add stron strength */ 
 xsp = __ndst_eval_xpr(mpp->mpref);
 __pop_itstk();

 /* since per bit, never need size convert */
 /* DBG remove --
 {
  struct expr_t *lhsx;

  lhsx = itp->itip->pb_ipins_tab[npp->obnum][npp->pbi];
  if (lhsx->szu.xclen != xsp->xslen) __misc_terr(__FILE__, __LINE__);
 }
 --- */
 return(xsp);
}

/*
 * load a mod. port (up iconn rhs to down mod port lhs) driver npp
 *
 * xmr form impossible here
 * driver is down module input or inout port (strength model)
 * called from down module itree location
 */
static struct xstk_t *ld_stmodport_down_driver(register struct net_pin_t *npp)
{
 register struct mod_pin_t *mpp;
 register struct itree_t *itp;
 register struct xstk_t *xsp;
 struct mod_t *downmdp;
 struct expr_t *xlhs, *up_rhsx;

 itp = __inst_ptr; 
 /* --- DBG remove
 if (itp->up_it == NULL) __misc_terr(__FILE__, __LINE__);
 --- */

 up_rhsx = itp->itip->ipins[npp->obnum];
 downmdp = itp->itip->imsym->el.emdp;
 mpp = &(downmdp->mpins[npp->obnum]);
 xlhs = mpp->mpref;
 __push_itstk(itp->up_it);
 /* if down rhsx is reg, will add strong */ 
 xsp = __ndst_eval_xpr(up_rhsx);
 /* only if rhs too narrow, need to add in HIZ and maybe widen alloc area */
 /* if too wide just ignores high bytes */
 /* SJM 05/10/04 - no sign extension because widening to z'x */
 if (xlhs->szu.xclen > up_rhsx->szu.xclen)
  __strenwiden_sizchg(xsp, xlhs->szu.xclen);
 __pop_itstk();
 return(xsp);
}

/*
 * load a mod. port (up iconn rhs to down mod port lhs) driver npp
 *
 * xmr form impossible here
 * driver is down module input or inout port (strength model)
 * called from down module itree location
 */
static struct xstk_t *ld_pb_stmodport_down_driver(
 register struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 register struct itree_t *itp;
 struct expr_t *up_rhsx;

 itp = __inst_ptr; 
 /* --- DBG remove
 if (itp->up_it == NULL) __misc_terr(__FILE__, __LINE__);
 --- */

 __push_itstk(itp->up_it);
 up_rhsx = itp->itip->pb_ipins_tab[npp->obnum][npp->pbi];
 /* if down rhsx is reg, will add strong */ 
 xsp = __ndst_eval_xpr(up_rhsx);

 /* since per bit never need size change */
 /* DBG remove --
 {
  struct mod_pin_t *mpp;
  struct mod_t *downmdp;

  downmdp = itp->itip->imsym->el.emdp;
  mpp = &(downmdp->mpins[npp->obnum]);
  mpp = &(mpp->pbmpps[npp->pbi]);
  if (mpp->mpref->szu.xclen > up_rhsx->szu.xclen)
   __misc_terr(__FILE__, __LINE__);
 }
 --- */

 __pop_itstk();
 return(xsp);
}

/*
 * load a strength pull driver (only strength possible) 
 */
static struct xstk_t *ld_stpull_driver(struct net_pin_t *npp)
{
 register struct xstk_t *xsp;
 byte *sbp;
 struct gate_t *gp;
 struct expr_t *ndp;

 /* get pull out lhs wire or select */
 gp = npp->elnpp.egp;
 ndp = gp->gpins[npp->obnum]; 
 push_xstk_(xsp, 4*ndp->szu.xclen);
 sbp = (byte *) xsp->ap;
 set_byteval_(sbp, ndp->szu.xclen, (byte) ((gp->g_stval << 2) | npp->pullval));
 return(xsp);
}

/*
 * STRENGTH EXPRESSION EVALUATION ROUTINES 
 */

/*
 * evaluate a rhs expression producing a strength format result
 * even if needs to add (st0,st1) for non strength expr.
 * this is equivalent of __eval_xpr when strength required
 *
 * pushes and pops temps and leaves result on top of reg stack
 * caller must pop value from stack
 * no stack checks underflow checks - maybe would help debugging
 */
extern struct xstk_t *__ndst_eval_xpr(struct expr_t *ndp)
{
 byte *sbp;
 struct xstk_t *stxsp;

 /* strength uses only a part of stack register */
 push_xstk_(stxsp, 4*ndp->szu.xclen);
 sbp = (byte *) stxsp->ap;
 ndst_eval2_xpr(sbp, ndp);
 return(stxsp);
}

/*
 * in place evaluate at least top node strength expression
 * this is for port feed thru assignments that pass strength thru
 * this does not leave value on top of stack
 */
static void ndst_eval2_xpr(register byte *sbp, register struct expr_t *ndp)
{
 register byte *bp; 
 int32 nd_itpop;
 struct net_t *np;
 struct expr_t *idndp;
 struct gref_t *grp;
 struct xstk_t *xsp;

 /* possible for this to be non stength reg. where strong added */
 if (!ndp->x_stren)
  {
   xsp = __eval2_xpr(ndp);
   __st_standval(sbp, xsp, ST_STRVAL);
   __pop_xstk();
   return;
  }

 /* in this case, must put value on tos */
 nd_itpop = FALSE;
 switch ((byte) ndp->optyp) {
  case UNCONNPULL:
   /* connection is unconnected but directives causes unc. to be pulled */
   /* know width here always exactly port width and stren */
   set_byteval_(sbp, ndp->szu.xclen, (ndp->unc_pull == UNCPULL0) ? ST_PULL0
    : ST_PULL1);
   return;
  case GLBREF:
   grp = ndp->ru.grp;
   __xmrpush_refgrp_to_targ(grp);
   nd_itpop = TRUE;
   /*FALLTHRU */
  case ID:
   np = ndp->lu.sy->el.enp;
   /* RELEASE remove ---
   if (!np->n_stren) __misc_terr(__FILE__, __LINE__);
   --- */
   /* get strength wire address */
   get_stwire_addr_(bp, np);
   memcpy(sbp, bp, ndp->szu.xclen); 
   break;
  case LSB:
   /* can never be array */
   idndp = ndp->lu.x;
   np = idndp->lu.sy->el.enp;
   /* SJM - 03/26/00 - was setting grp to bsel node not lhs id */
   if (idndp->optyp == GLBREF)
    { grp = idndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
   access_stbsel(sbp, ndp);
   break;
  case PARTSEL:
   idndp = ndp->lu.x;
   np = idndp->lu.sy->el.enp;
   /* SJM - 03/26/00 - was setting grp to bsel node not lhs id */
   if (idndp->optyp == GLBREF)
    { grp = idndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
   access_stpsel(sbp, ndp);
   break;
  case LCB: rhs_stconcat(sbp, ndp); break;
  default: __case_terr(__FILE__, __LINE__);
 }
 if (nd_itpop) __pop_itstk();
}

/*
 * access a selected bit to sbp (width 1 for bit)
 *
 * this if for cases where accessing strength and know it is needed and
 * know value has strength
 */
static void access_stbsel(register byte *sbp, register struct expr_t *ndp)
{
 register int32 biti;
 register byte *bp;
 register struct net_t *np;

 np = ndp->lu.x->lu.sy->el.enp;
 biti = __get_const_bselndx(ndp);
 /* unknown index is strong x - cannot emit warning happens too often */
 /* strong x 6,6,3 - 11011011 */
 if (biti == -1) { *sbp = 0xdb; return; }
 /* get strength wire address */
 get_stwire_addr_(bp, np);
 *sbp = bp[biti]; 
}

/*
 * get a bit select index from an expr. node
 * need to be in correct current itree loc.
 * only for constant index cases
 */
extern int32 __get_const_bselndx(register struct expr_t *ndp)
{
 register int32 biti;
 register word32 *wp;

 /* know will either be constant or expr. here */
 if (ndp->ru.x->optyp == NUMBER)
  {
   wp = &(__contab[ndp->ru.x->ru.xvi]);
   if (wp[1] != 0) biti = -1; else biti = (int32) wp[0];
  }
 else if (ndp->ru.x->optyp == ISNUMBER)
  {
   wp = &(__contab[ndp->ru.x->ru.xvi]);
   wp = &(wp[2*__inum]);
   if (wp[1] != 0) biti = -1; else biti = (int32) wp[0];
  }
 else { __case_terr(__FILE__, __LINE__); biti = -1; }
 return(biti);
}

/*
 * push (access) a selected range into sbp
 * one bit ok but cannot part select from scalar
 */
static void access_stpsel(register byte *sbp, register struct expr_t *ndp)
{
 register int32 bi2;
 register byte *abp;
 struct expr_t *idndp, *ndx2;
 struct net_t *np;

 /* know these are both constant nodes, in range, and are h:0 normalized */
 /* and non IS form */
 idndp = ndp->lu.x;
 np = idndp->lu.sy->el.enp;
 /* notice 2nd range is low */
 ndx2 = ndp->ru.x->ru.x;

 /* bi2 cannot be -1 (out of range) or will not get here */
 bi2 = (int32) __contab[ndx2->ru.xvi];

 /* get strength wire address */
 get_stwire_addr_(abp, np);

 memcpy(sbp, &(abp[bi2]), ndp->szu.xclen);
}

/*
 * evaluate a known strength expr node rhs concatenate
 * key is that { op. node width is same as starting high bit of value
 *
 * notice that subexpressions of strength concat can be non strength
 * strength stored low bit (0) to high bit (n)
 */
static void rhs_stconcat(register byte *sbp, struct expr_t *lcbndp)
{
 register struct expr_t *ndp; 
 register byte *sbp2;
 register int32 i;
 register int32 bi2;
 struct expr_t *catndp;

 for (ndp = lcbndp->ru.x; ndp != NULL; ndp = ndp->ru.x)
  {
   catndp = ndp->lu.x;

   /* bi2 is low bit, LCB node xclen start high bit, catndp is id/num width */
   bi2 = ndp->szu.xclen - catndp->szu.xclen;
   sbp2 = &(sbp[bi2]);
   /* here know cat width must match rhs width */
   ndst_eval2_xpr(sbp2, catndp);
  }
 if (__debug_flg && __ev_tracing)
  {
   __cur_sofs = 0;
   for (i = lcbndp->szu.xclen - 1; i >= 0; i--)
    {
     __adds(__to_vvstnam(__xs, (word32) sbp[i]));
     addch_('|');
     __exprline[__cur_sofs] = '\0';
    }
   __tr_msg("++ strength concatenate result: %s\n", __exprline);
   __cur_sofs = 0;
  }
}

/*
 * store a/b value into strength byte array - set value or in new stregth
 * byte order for strength bytes [h:l] just like word32 bits
 * notice this needs to be called with blen less that actual stacked blen
 * also must and off possibly unused parts
 *
 * notice z must not have strong added for non strength case
 * and need to map to right 0 val/1 val strengths
 */
extern void __st_standval(register byte *sbp, register struct xstk_t *xsp,
 byte new_st)
{
 register int32 bi, aw, bw;

 /* short circuit for 1 bit case */
 if (xsp->xslen == 1)
  {
   aw = (xsp->ap[0] & 1L) | ((xsp->bp[0] << 1) & 2L);
   /* if z, no strength */
   if (aw == 2) sbp[0] = (byte) aw;
   else
     {
      /* SJM 08/07/01 - need to use stren map table so val 0 has 0 stren etc */
      aw |= (new_st << 2);
      sbp[0] = __stren_map_tab[aw];
     }
   return;
  }

 for (bi = 0; bi < xsp->xslen; bi++)
  {
   aw = rhsbsel_(xsp->ap, bi);
   bw = rhsbsel_(xsp->bp, bi);
   aw |= (bw << 1);
   /* if z no strength */
   if (aw == 2) sbp[bi] = (byte) aw;
   else
    {
     /* SJM 08/07/01 - need to use stren map table so val 0 has 0 stren etc */
     aw |= (new_st << 2);
     /* SJM 08/15/01 - this is vector - from typo was only set 0 */
     sbp[bi] = __stren_map_tab[aw];
    }
  }
}

/*
 * pairwise combine driving value into section (or all) of wire
 * know abi1 (high) and abi2 (low) already corrected for h:0 form
 */
static void eval_stwire(word32 wtyp, register byte *accsbp,
 int32 abi1, int32 abi2, register byte *sbp)
{
 register int32 bi, bi2;
 
 for (bi = abi2, bi2 = 0; bi <= abi1; bi++, bi2++)
  accsbp[bi] = (byte) __comb_1bitsts(wtyp, (word32) accsbp[bi], (word32) sbp[bi2]);
}

/*
 * combine into first 2nd 1 bit (byte form) strength value that is not
 * wired logic
 *
 * format [s000,s111,vv] - 3 high bits st 0, 3 middle bits st 1, 2 low val 
 * convention for non x/z value s0 is highest and s1 is lowest
 *
 * routine works for everything except trireg - need special "array" for it
 * because for tri0/tri1 initialized to state if no drivers
 * supply0 nets cannot be effected by driver so just init and return above
 *
 * notice storing H and L as x value with 1 or other strength 0 - since
 * no different from real way where strengths same but value z (2)
 *
 * LOOKATME - myabe this should use word32 variables
 */
extern word32 __comb_1bitsts(word32 wtyp, register word32 acc, register word32 op)
{
 word32 str0acc, str1acc, vacc, str0op, str1op, vop;
 word32 s0hop, s0lop, s1hop, s1lop, s0hacc, s0lacc, s1hacc, s1lacc;
 word32 s0h, s0l, s1h, s1l, s0, s1;

 /* eliminate either HiZ because HiZ loses to anything */
 /* notice H and L cannot be eliminated here */
 if (acc == 2)
  {
   /* DBG remove -- */
   if (__debug_flg && __ev_tracing) { vop = op; goto do_outmsg; }
   /* --- */
   return(op);
  }
 if (op == 2)
  {
   /* DBG remove - */
   if (__debug_flg && __ev_tracing) { vop = acc; goto do_outmsg; }
   /* --- */
   return(acc);
  }

 /* extract values, */
 vacc = acc & 3; 
 vop = op & 3; 
 str0acc = (acc >> 5) & 7;  
 str1acc = (acc >> 2) & 7;
 str0op = (op >> 5) & 7;  
 str1op = (op >> 2) & 7;

 /* handle special cases that are independent of wired logic type */
 /* both 1 or both 0 - strength always largest */
 if ((vacc == 1 && vop == 1) || (vacc == 0 && vop == 0))
  {
   /* in case of both strength in 0 or 1 region, 0 st higher than 1 st */ 
   s0 = (str0acc >= str0op) ? str0acc : str0op;
   s1 = (str1acc >= str1op) ? str1acc : str1op;
   goto done;
  }
 /* both have fixed strength (<s:s>=? and <t:t>=?) */ 
 /* notice for H and L, strength never same */
 if (str0acc == str1acc && str0op == str1op) 
  {
   /* case 1: strengths same - may need wired logic compare */
   s0 = s1 = str0acc;
   if (str0acc == str0op)
    {
     switch ((byte) wtyp) {
      /* since know differ and know stren same, non wired must be x */
      case N_TRI: case N_WIRE: case N_TRI0: case N_TRIREG: case N_TRI1:
      /* SJM 11/16/00 - supplies possible here, but how */
      case N_SUPPLY0: case N_SUPPLY1:
       vacc = 3;
       break;
      case N_WA: case N_TRIAND: 
       /* if either 0, result 0, else result X */
       if (vacc == 0 || vop == 0) vacc = 0; else vacc = 3;
       break;
      case N_WO: case N_TRIOR:
       /* if either 1, result 1, else result X */
       if (vacc == 1 || vop == 1) vacc = 1; else vacc = 3;
       break;
      default: __case_terr(__FILE__, __LINE__);
     }
     goto done; 
    }
   /* case 1: strengths differ - value of stronger stren wins */
   if (str0acc <= str0op) { s0 = s1 = str0op; vacc = vop; }
   goto done;
  }

 /* separate into low-high for both 0 and 1 for both operands */
 s0hacc = s0lacc = s1hacc = s1lacc = 0;
 switch ((byte) vacc) {
  case 0: s0hacc = str0acc; s0lacc = str1acc; break; 
  case 1: s1hacc = str0acc; s1lacc = str1acc; break; 
  case 3: s0hacc = str0acc; s1hacc = str1acc; break;
  default: __case_terr(__FILE__, __LINE__);
 }
 s0hop = s0lop = s1hop = s1lop = 0;
 switch ((byte) vop) {
  case 0: s0hop = str0op; s0lop = str1op; break; 
  case 1: s1hop = str0op; s1lop = str1op; break; 
  case 3: s0hop = str0op; s1hop = str1op; break;
  default: __case_terr(__FILE__, __LINE__);
 }
 /* tournament 1 - op and acc 0 strengths and 1 strengths */
 s0h = (s0hacc >= s0hop) ? s0hacc : s0hop; 
 s0l = (s0lacc >= s0lop) ? s0lacc : s0lop;
 s1h = (s1hacc >= s1hop) ? s1hacc : s1hop; 
 s1l = (s1lacc >= s1lop) ? s1lacc : s1lop;

 /* tournament 2 - 0 and 1 strengths */
 /* if lowest 0 higher than highest 1, remove 1 strengths */
 if (s0l > s1h) { s0 = s0h; s1 = s0l; vacc = 0; }
 /* if lowest 1 higher than highest 0, remove 0 strengths */  
 else if (s1l > s0h) { s0 = s1h; s1 = s1l; vacc = 1; } 
 /* x - strengths in both regions (fill in smaller gap) */
 else { s0 = s0h; s1 = s1h; vacc = 3; }

 /* ? believe that for wired logic, if 1 strength range */
 /* then value is outer range x - no wired logic here - is it true */

done:
 /* --- DBG remove */
 if (__debug_flg && __ev_tracing)
  {
   char vs1[10], vs2[10], vs3[10];

   vop = (s0 << 5) | (s1 << 2) | vacc;
do_outmsg:
   __tr_msg("+> fi>1 strength: %s acc=%s,op=%s,res=%s\n", 
    __to_wtnam2(__xs, (word32) wtyp), __to_vvstnam(vs1, (word32) acc),   
    __to_vvstnam(vs2, (word32) op), __to_vvstnam(vs3, (word32) vop));
   return(vop);
  }
/* --- */
 return((s0 << 5) | (s1 << 2) | vacc); 
}

/*
 * EXECUTION OUTPUT ROUTINES INCLUDING SHOWVARS (NEEDS EXEC VALUES)
 */

/*
 * build the state of a gate into string
 * can only be called during exec when current inst. on itstk
 * caller must do any needed truncating
 *
 * handles strengths and bufif and mos style gates
 * all values extracted from gate state
 */
extern char *__gstate_tostr(char *s, struct gate_t *gp, int32 fullpath)
{
 int32 srep, pi, nins, sav_sofs, conducting;
 word32 tmp, tmp2, uwrd, av, bv;
 i_tev_ndx tevpi;
 struct xstk_t *xsp;
 struct udp_t *udpp;
 char s1[RECLEN], s2[10], s3[10], s4[10];

 sav_sofs = __cur_sofs;
 __adds(gp->gmsym->synam);

 addch_(' ');
 if (fullpath)
  {
   __disp_itree_path(__inst_ptr, (struct task_t *) NULL);
   addch_('.');
  }
 __adds(gp->gsym->synam);
 addch_('(');

 nins = gp->gpnum - 1;
 switch ((byte) gp->g_class) {
  case GC_LOGIC:
   if (gp->gpnum > 16) srep = SR_VEC; else srep = SR_PVEC;
   push_xstk_(xsp, nins + 1);
   if (srep == SR_VEC)
    {
     __ld_gate_wide_val(xsp->ap, xsp->bp, gp->gstate.wp, nins + 1); 
     /* SJM 11/26/00 - need to select from multi-word32 xsp if wider than 16 */
     /* put output value in tmp - since low bit is 0 right index is nins **/
     tmp = rhsbsel_(xsp->ap, nins);
     tmp2 = rhsbsel_(xsp->bp, nins);
     tmp |= (tmp2 << 1);
     __adds(__to_vvnam(s1, tmp));
     adds_evgate_ins(xsp->ap, xsp->bp, nins);
    }
   else
    {
     /* extract current output value */
     uwrd = get_packintowrd_(gp->gstate, __inum, nins + 1);
     tmp = ((uwrd >> nins) & 1L) | ((uwrd >> (2*nins)) & 2L);
     __adds(__to_vvnam(s1, tmp));
     av = uwrd & __masktab[nins + 1];
     bv = uwrd >> (nins + 1);
     adds_evgate_ins(&av, &bv, nins);
    }
   addch_(')');
   __pop_xstk();
   break;
  case GC_UDP:
   udpp = gp->gmsym->el.eudpp;
   nins = udpp->numins;
   if (udpp->u_wide) uwrd = gp->gstate.wp[2*__inum];
   else uwrd = (word32) gp->gstate.hwp[__inum];
   tmp = (uwrd >> (2*nins)) & 3L;
   /* DBG remove ---
   if (tmp == 2) __misc_terr(__FILE__, __LINE__);
   --- */
   __adds(__to_vvnam(s1, tmp));
   for (pi = 0; pi < nins; pi++)
    {
     __adds(", ");
     tmp = (uwrd >> (2*pi)) & 3L;
     /* DBG remove ---
     if (tmp == 2) __misc_terr(__FILE__, __LINE__);
     --- */
     __adds( __to_vvnam(s1, tmp));
    }
   addch_(')');
   /* --- RELEASE REMOVE --- 
   if (udpp->u_wide)
    {
     tmp = gp->gstate.wp[2*__inum + 1];
     sprintf(s1, "<%lu>", tmp);
     __adds(s1);
    }
   --- */
   break;
  case GC_BUFIF:
   uwrd = (word32) gp->gstate.hwp[__inum];
   __adds(__to_vvstnam(s1, (uwrd >> 4) & 0xffL));
   __adds(", ");
   __adds(__to_vvnam(s2, (uwrd & 0x3L)));
   __adds(", ");
   __adds(__to_vvnam(s3, (uwrd >> 2) & 3L));
   addch_(')');
   break;
  case GC_MOS:
   /* notice packing bit ranges for mos and bufif different */
   uwrd = gp->gstate.wp[__inum];
   __adds(__to_vvstnam(s1, ((uwrd >> 16) & 0xffL)));
   __adds(", ");
   __adds(__to_vvstnam(s2, (uwrd & 0xffL)));
   __adds(", ");
   __adds(__to_vvnam(s3, ((uwrd >> 8) & 0x3L)));
   addch_(')');
   break;
  case GC_CMOS:
   uwrd = gp->gstate.wp[__inum];
   __adds(__to_vvstnam(s1, ((uwrd >> 24) & 0xffL)));
   __adds(", ");
   __adds(__to_vvstnam(s2, (uwrd & 0xffL)));
   __adds(", ");
   __adds(__to_vvnam(s3, ((uwrd >> 8) & 0x3L)));
   __adds(", ");
   __adds(__to_vvnam(s4, ((uwrd >> 16) & 0x3L)));
   addch_(')');
   break;
  case GC_TRANIF:
   /* only called for tranif input gate */
   conducting = get_tranif_onoff_(gp);
   if (conducting == 1) __adds("**ON**)");
   else if (conducting == 0) __adds("**OFF**)");
   else __adds("**UNKNOWN**)");
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 /* if pending event, write it */
 if (gp->schd_tevs != NULL
  && (tevpi = gp->schd_tevs[__inum]) != -1)
  { __bld_valofsched(s1, &(__tevtab[tevpi])); __adds(s1); }
 __exprline[__cur_sofs] = '\0';
 
 /* finally truncate suffix */
 __trunc_cstr(&(__exprline[sav_sofs]), MSGTRUNCLEN, FALSE);
 strcpy(s, &(__exprline[sav_sofs]));
 __cur_sofs = sav_sofs;
 __exprline[__cur_sofs] = '\0'; 
 return(s);
}

/*
 * write gate inputs - only for normal logic gates
 * notice ports are numbered 0 (output) to gpnum (rightmost input)
 * but storage is inputs bit 0 to gpnum and output in gpnum (sep. a and b) 
 */
static void adds_evgate_ins(word32 *gatap, word32 *gatbp, int32 nins)
{
 register int32 bi;
 word32 tmp, tmp2;
 char s1[RECLEN];
 
 for (bi = 0; bi < nins; bi++)
  {
   tmp = rhsbsel_(gatap, bi);
   tmp2 = rhsbsel_(gatbp, bi);
   tmp |= (tmp2 << 1);
   __adds(", ");
   __adds(__to_vvnam(s1, tmp));
  }
}

/*
 * SIMULATION TIME DUMP (SHOWVARS) ROUTINES
 */

/*
 * dump all variables in a design
 * this can only be called after prep done
 */
extern void __show_allvars(void)
{
 register int32 ii;
 struct itree_t *itp; 

 for (ii = 0; ii < __numtopm; ii++)
  { itp = __it_roots[ii]; show2_allvars(itp); }
 __cvsim_msg("\n");
}

/*
 * convert global for module whose type corresponds to itree itp
 */
static void show2_allvars(struct itree_t *itp)
{
 register int32 ii; 
 register struct net_t *np;
 struct mod_t *imdp;
 struct itree_t *itp2;

 imdp = itp->itip->imsym->el.emdp;
 __cvsim_msg("==> showing all variables in %s type %s.\n",
  __msg2_blditree(__xs, itp), imdp->msym->synam);

 /* notice using ii as ni here */
 if (imdp->mnnum != 0)
  {
   for (ii = 0, np = &(imdp->mnets[0]); ii < imdp->mnnum; ii++, np++) 
    { __push_itstk(itp); __emit_1showvar(np, NULL); __pop_itstk(); }
  }
 for (ii = 0; ii < imdp->minum; ii++)
  { itp2 = &(itp->in_its[ii]); show2_allvars(itp2); }
}

/*
 * show variable prefix from current itp instance
 * assumes scope location already emitted
 *
 * expects __stlevel to be set to 0 here
 * this must happen after all processing within one time unit
 * itree must be on top of stack to call this
 */
extern void __emit_1showvar(struct net_t *np, struct gref_t *grp)
{
 register int32 i, bi;
 register struct net_pin_t *npp;
 int32 arrwid, obwid;
 i_tev_ndx tevpi, *teviarr;
 char s1[RECLEN], s2[RECLEN];

 /* for parameters (still in symb. table but replace) no showvars */ 
 if (np->n_isaparam) return;

 __cur_sofs = 0;
 /* emit the variable information prefix */
 __bld_showvars_prefix(s1, np, grp);

 if (np->ntyp == N_EVENT)
  { __cvsim_msg("%s **event**\n", s1); goto disp_drvs; }

 /* emit the current value of entire variable */
 /* for array print the first 4 */
 if (np->n_isarr)
  {
   __adds(" [");
   arrwid = __get_arrwide(np);
   /* this should be setable in the debugger by the user */
   if (arrwid > 4) obwid = 4; else obwid = arrwid;
   for (i = 0; i < obwid; i++)
    {
     if (i != 0) __adds(", ");
     __var_tostr(s2, np, i, i, BBIN);
     __adds(s2);
    }
   if (arrwid > obwid)
    { sprintf(__xs, ", ... <%d more>", arrwid - obwid); __adds(__xs); }
   __adds("]");
  }
 /* emit variable current value - also handles strength variables */ 
 /* LOOKATME - this should use base from net or at least give base */
 else __disp_var(np, -1, -1, BHEX, '?');
 __cvsim_msg("%s = %s\n", s1, __exprline);
 __cur_sofs = 0;

 /* if wire has delay or path dest., there may be a scheduled value */
 if (np->nrngrep == NX_DWIR)
  {
   /* access per bit array of possibly scheduled values for wire */
   /* pointer to array element that is first bit of current inst. */
   teviarr = &(np->nu.rngdwir->wschd_pbtevs[np->nwid*__inum]);
   if (np->n_isavec)
    {
     __cvsim_msg("   Per bit wire with delay scheduled values:\n"); 
     for (bi = np->nwid - 1; bi >= 0; bi--)
      {
       if ((tevpi = teviarr[bi]) != -1) 
        {
         /* form: [i] (schedule %s at %s) */
         __cvsim_msg("    [%d] %s\n", __unnormalize_ndx(np, bi),
          __bld_valofsched(s1, &(__tevtab[tevpi])));
        }
      }
    }
   else if ((tevpi = teviarr[0]) != -1)
    {
    __cvsim_msg("scalar wire scheduled %s\n",
      __bld_valofsched(s1, &(__tevtab[tevpi])));
    }
  }

 /* emit drivers - regs never have drivers but can have loads */
disp_drvs:
 /* here no filter for rooted xmr since one inst is driver */
 for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
  emit1_driver(np, npp, FALSE);
 /* emit loads - no inst. filter for rooted xmr npp */
 for (npp = np->nlds; npp != NULL; npp = npp->npnxt) emit1_load(np, npp);
 __cur_sofs = 0;
}

/*
 * emit a driver string - if nonz_only only emit non floating drivers
 *
 * only for use at end of time unit
 * when called itree location is wire from which npp driver connected to
 * notice np tchg can never be a driver
 */
static void emit1_driver(struct net_t *np, struct net_pin_t *npp,
 int32 nonz_only)
{
 int32 i1, i2, obwid, ri1, ri2, nd_itpop;
 i_tev_ndx tevpi;
 word32 wrd;
 byte *sbp;
 struct gate_t *gp;
 struct conta_t *cap, *cap2;
 struct xstk_t *xsp;
 struct itree_t *dritp;
 struct inst_t *ip;
 struct mod_t *mdp;
 struct mod_pin_t *mpp;
 struct npaux_t *npauxp;
 struct tfrec_t *tfrp;
 struct tfarg_t *tfap;
 char ndxs[RECLEN], s1[RECLEN], s2[RECLEN], s3[RECLEN];

 /* think sbp and xsp always set here - lint warning from  if separation */
 sbp = NULL;
 xsp = NULL;
 ri1 = ri2 = -1;
 /* this is needed to get this instances pattern for IS form */ 
 
 if ((npauxp = npp->npaux) == NULL) i1 = i2 = -1;
 else __get_cor_range(npauxp->nbi1, npauxp->nbi2, &i1, &i2);
 /* also need to print lhs select range but leave rhs width as is */
 /* assignment will widen or truncate if needed but driver is rhs */
 if (i1 != -1)
  {
   /* need range of array for umapping index for message */
   if (np->n_isarr) __getarr_range(np, &ri1, &ri2, &obwid);
   else if (np->n_isavec) __getwir_range(np, &ri1, &ri2);
   else __arg_terr(__FILE__, __LINE__);
   if (i1 == i2) sprintf(ndxs, " [%d]", __unmap_ndx(i1, ri1, ri2));
   else sprintf(ndxs, " [%d:%d]", __unmap_ndx(i1, ri1, ri2),
   __unmap_ndx(i2, ri1, ri2));
  }
 else strcpy(ndxs, "");

 /* this traces from target (where var. is) back to ref. itree loc. */
 if (npp->npproctyp != NP_PROC_INMOD)
  {
   /* SJM 04/17/03 - if XMR does not match - do not combine in */
   if (!__move_to_npprefloc(npp)) return;
   nd_itpop = TRUE;
  }
 else nd_itpop = FALSE;

 /* special handling for getpattern on rhs */
 if (npp->npntyp == NP_CONTA)
  { 
   cap = npp->elnpp.ecap;
   if (cap->lhsx->getpatlhs)
    { 
     __cvsim_msg("%s   driver: continuous assign of $getpattern to %s\n",
      ndxs, __msgexpr_tostr(__xs, cap->lhsx)); 
     if (nd_itpop) __pop_itstk();
     return;
    }
  }

 /* here for inouts (up iconn and down mdprt) and trans need driver value */ 
 /* notice this insure that returned xsp never nil */
 switch ((byte) npp->npntyp) {
  case NP_GATE:
   /* current value of gate is state output - next value is event state */
   /* if driver (gate output) wide just accesses low bit here */
   gp = npp->elnpp.egp;
   /* get driving value */ 
   if (np->n_stren)
    {
     xsp = ld_stgate_driver(npp);
     sbp = (byte *) xsp->ap;
     /* high impedance z will always have z value and 0 strengths */
     if (nonz_only && sbp[0] == 2) break;
     __to_vvstnam(s1, (word32) sbp[0]);
    }
   else
    {
     xsp = __ld_gate_driver(npp);
     wrd = xsp->ap[0] | (xsp->bp[0] << 1);
     if (nonz_only && wrd == 2L) break;
     __to_vvnam(s1, wrd);
    }
   if (gp->g_class == GC_UDP)
    sprintf(s2, "%s udp %s", __schop(__xs, gp->gmsym->synam),
    __schop(__xs2, gp->gsym->synam));
   else
    {
     if (gp->gmsym->el.eprimp->gateid == G_ASSIGN)
      strcpy(s2,  "1 bit continuous assign"); 
     else sprintf(s2, "%s gate %s", __schop(__xs, gp->gmsym->synam),
      __schop(__xs2, gp->gsym->synam));
    }
   if (gp->schd_tevs != NULL && 
    (tevpi = gp->schd_tevs[__inum]) != -1)
    __bld_valofsched(s3, &(__tevtab[tevpi]));
   else strcpy(s3, ""); 

   __cvsim_msg("%s   driver: %s (port %d) at %s = %s%s\n", ndxs, s2,
    npp->obnum + 1, __bld_lineloc(__xs, gp->gsym->syfnam_ind,
    gp->gsym->sylin_cnt), s1, s3);
   break;
  case NP_CONTA:
   /* value for driver that is cont. assign is: 1) ca driver wp per inst if */
   /* set, 2) ca_rhsval if for 0 delay case, else evaled lhs if fi == 1 */
   cap = npp->elnpp.ecap;
   if (cap->ca_pb_sim) cap2 = &(cap->pbcau.pbcaps[npp->pbi]);
   else cap2 = cap;
   if (np->n_stren)
    {
     xsp = ld_stconta_driver(npp);
     sbp = (byte *) xsp->ap;  
     /* this returns NULL on all drivers z if nonz only on */
     /* SJM 11/13/00 - must use lhs width here in rhs wider truncated */
     if (stdrive_tostr(s1, sbp, npp, cap2->lhsx->szu.xclen, nonz_only)
      == NULL) break;
    }
   else
    {
     xsp = __ld_conta_driver(npp);
     if (drive_tostr(s1, xsp->ap, xsp->bp, npp, cap2->rhsx->szu.xclen,
      nonz_only) == NULL) break;
    }
   if (cap2->caschd_tevs != NULL && (tevpi = cap2->caschd_tevs[__inum]) != -1)
     __bld_valofsched(s2, &(__tevtab[tevpi]));
   else strcpy(s2, ""); 

   if (cap->ca_pb_sim)
    {
     __cvsim_msg("%s   driver: continuous assign to %s per bit %d at %s = %s%s\n",
      ndxs, __msgexpr_tostr(__xs, cap2->lhsx), npp->pbi, 
      __bld_lineloc(__xs2, cap->casym->syfnam_ind, cap->casym->sylin_cnt),
      s1, s2);
    }
   else
    {
     __cvsim_msg("%s   driver: continuous assign to %s at %s = %s%s\n",
      ndxs, __msgexpr_tostr(__xs, cap->lhsx), 
      __bld_lineloc(__xs2, cap->casym->syfnam_ind, cap->casym->sylin_cnt),
      s1, s2);
    }
   break;
  case NP_TFRWARG:
   tfrp = npp->elnpp.etfrp;
   tfap = &(tfrp->tfargs[npp->obnum]);
   if (np->n_stren)
    {
     xsp = __ld_sttfrwarg_driver(npp);
     sbp = (byte *) xsp->ap;  
     /* this returns NULL on all drivers z if nonz only on */
     if (stdrive_tostr(s1, sbp, npp, tfap->arg.axp->szu.xclen, nonz_only)
      == NULL) break;
    }
   else
    {
     xsp = __ld_tfrwarg_driver(npp);
     if (drive_tostr(s1, xsp->ap, xsp->bp, npp, tfap->arg.axp->szu.xclen,
      nonz_only) == NULL) break;
    }
   __cvsim_msg("%s   driver: tf_ call of %s arg %s (pos. %d) at %s = %s\n",
    ndxs, __get_tfcellnam(tfrp), __msgexpr_tostr(__xs, tfap->arg.axp), npp->obnum,
    __bld_lineloc(__xs2, tfrp->tffnam_ind, tfrp->tflin_cnt), s1);
   break;
  case NP_VPIPUTV:
   /* DBG remove -- */
   if (npp->elnpp.enp != np) __arg_terr(__FILE__, __LINE__);
   /* --- */
   /* if this instance does not have driver, do not emit */
   if (!__has_vpi_driver(np, npp))
    {
     if (nd_itpop) __pop_itstk();
     return;
    }

   if (np->n_stren)
    {
     xsp = ld_stvpiputv_driver(npp); 
     sbp = (byte *) xsp->ap;  
     /* this returns NULL on all drivers z if nonz only on */
     if (stdrive_tostr(s1, sbp, npp, np->nwid, nonz_only) == NULL) break;
    }
   else
    {
     xsp = __ld_vpiputv_driver(npp); 
     if (drive_tostr(s1, xsp->ap, xsp->bp, npp, np->nwid, nonz_only) == NULL)
      break;
    }
   __cvsim_msg("%s   driver: vpi_put_value driving %s %s = %s\n", ndxs,
    np->nsym->synam, __to_wtnam(__xs2, np), s1);
   break;
  case NP_ICONN:
   /* called from up itree loc. */
   dritp = &(__inst_ptr->in_its[npp->elnpp.eii]);
   ip = dritp->itip;
   /* need to make sure driver is loaded - no concept of changing one here */
   if (np->n_stren)
    {
     xsp = ld_sticonn_up_driver(npp);
     sbp = (byte *) xsp->ap;
     /* this returns NULL on all drivers z if nonz only on */
     if (stdrive_tostr(s1, sbp, npp, xsp->xslen/4, nonz_only) == NULL) break;
    }
   else
    {
     xsp = __ld_iconn_up_driver(npp);
     if (drive_tostr(s1, xsp->ap, xsp->bp, npp, xsp->xslen, nonz_only)
      == NULL) break;
    }
   __cvsim_msg("%s   driver: instance %s port %s at %s = %s\n",
    ndxs, ip->isym->synam, np->nsym->synam, __bld_lineloc(__xs,
    ip->isym->syfnam_ind, ip->isym->sylin_cnt), s1);
   break;
  case NP_PB_ICONN:
   /* called from up itree loc. */
   dritp = &(__inst_ptr->in_its[npp->elnpp.eii]);
   ip = dritp->itip;
   /* need to make sure driver is loaded - no concept of changing one here */
   if (np->n_stren)
    {
     xsp = ld_pb_sticonn_up_driver(npp);
     sbp = (byte *) xsp->ap;
     /* this returns NULL on all drivers z if nonz only on */
     if (stdrive_tostr(s1, sbp, npp, xsp->xslen/4, nonz_only) == NULL) break;
    }
   else
    {
     xsp = __ld_pb_iconn_up_driver(npp);
     if (drive_tostr(s1, xsp->ap, xsp->bp, npp, xsp->xslen, nonz_only)
      == NULL) break;
    }
   __cvsim_msg("%s   driver: instance %s port %s bit %d at %s = %s\n",
    ndxs, ip->isym->synam, np->nsym->synam, npp->pbi, __bld_lineloc(__xs,
    ip->isym->syfnam_ind, ip->isym->sylin_cnt), s1);
   break;
  case NP_MDPRT:
   /* called from down mod port itree loc. */
   mdp = npp->elnpp.emdp;
   mpp = &(mdp->mpins[npp->obnum]);
   if (np->n_stren)
    {
     xsp = ld_stmodport_down_driver(npp);
     sbp = (byte *) xsp->ap;      
     /* this returns NULL on all drivers z if nonz only on */
     if (stdrive_tostr(s1, sbp, npp, xsp->xslen/4, nonz_only) == NULL)
      break;
    }
   else
    {
     xsp = __ld_modport_down_driver(npp);
     if (drive_tostr(s1, xsp->ap, xsp->bp, npp, xsp->xslen, nonz_only)
      == NULL) break;
    }
   __cvsim_msg("%s   driver: module %s port %s at %s = %s\n",
    ndxs, mdp->msym->synam, np->nsym->synam, __bld_lineloc(__xs,
    mpp->mpfnam_ind, mpp->mplin_cnt), s1);
   break;
  case NP_PB_MDPRT:
   /* called from down mod port itree loc. */
   mdp = npp->elnpp.emdp;
   mpp = &(mdp->mpins[npp->obnum]);
   mpp = &(mpp->pbmpps[npp->pbi]);
   if (np->n_stren)
    {
     xsp = ld_pb_stmodport_down_driver(npp);
     sbp = (byte *) xsp->ap;      
     /* this returns NULL on all drivers z if nonz only on */
     if (stdrive_tostr(s1, sbp, npp, xsp->xslen/4, nonz_only) == NULL)
      break;
    }
   else
    {
     xsp = __ld_pb_modport_down_driver(npp);
     if (drive_tostr(s1, xsp->ap, xsp->bp, npp, xsp->xslen, nonz_only)
      == NULL) break;
    }
   __cvsim_msg("%s   driver: module %s port %s bit %d at %s = %s\n",
    ndxs, mdp->msym->synam, np->nsym->synam, npp->pbi, __bld_lineloc(__xs,
    mpp->mpfnam_ind, mpp->mplin_cnt), s1);
   break;
  case NP_PULL:
   /* notice do not need to call ld_pull stren - fixed so get from gstate */
   gp = npp->elnpp.egp;
   __cvsim_msg("%s   driver: pull to %s at %s of %s\n",
    ndxs, __to_vvstnam(s1, (word32) ((gp->g_stval << 2) | npp->pullval)),
    __bld_lineloc(__xs, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt),
    __msgexpr_tostr(__xs2, gp->gpins[npp->obnum]));
   if (nd_itpop) __pop_itstk();
   return;
  default: __case_terr(__FILE__, __LINE__); return;
 }
 __pop_xstk();
 if (nd_itpop) __pop_itstk();
}

/*
 * convert driver to string with non z only processing
 * ap and bp rhs conta expr. 
 * rhs is driver of entire conta which is right unless lhs is
 * concat, if so lcb i1 used to select part driving this net
 *
 * notice truncating this then truncating entire drive message
 */
static char *drive_tostr(char *s, word32 *ap, word32 *bp, struct net_pin_t *npp, 
 int32 rhswid, int32 nonz_only)
{
 int32 nd_xpop;
 int32 wlen, ubits;
 struct xstk_t *tmpxsp;
 struct npaux_t *npauxp;

 nd_xpop = FALSE;
 /* if lhs is concatenate, must isolate part dirving this npp wire section */ 
 if ((npauxp = npp->npaux) != NULL && npauxp->lcbi1 != -1)
  {
   rhswid = npauxp->lcbi1 - npauxp->lcbi2 + 1;
   if (npauxp->lcbi2 != 0)
    {
     push_xstk_(tmpxsp, rhswid);
     __rhspsel(tmpxsp->ap, ap, npauxp->lcbi2, rhswid); 
     __rhspsel(tmpxsp->bp, bp, npauxp->lcbi2, rhswid); 
     nd_xpop = TRUE;
     ap = tmpxsp->ap; 
     bp = tmpxsp->bp;
    }
   else
    {
     /* conta lhs concatenate section starts at low bit */
     wlen = wlen_(rhswid);
     if ((ubits = ubits_(rhswid)) != 0)
      { ap[wlen - 1] &= __masktab[ubits]; bp[wlen - 1] &= __masktab[ubits]; }
    }
  }
 /* notice all z test after selection of actual bits that drive this net */
 if (nonz_only && !__vval_isallzs(ap, bp, rhswid)) return(FALSE);

 __regab_tostr(s, ap, bp, rhswid, BBIN, FALSE);
 if (nd_xpop) __pop_xstk();
 return(s);
}

/*
 * copy a strength driving expr. value to a string with select if lhs concat
 */
static char *stdrive_tostr(char *s, byte *sbp, struct net_pin_t *npp,
 int32 rhswid, int32 nonz_only)
{
 struct npaux_t *npauxp;

 /* if conta has lhs concat, only emit bits that drive this wire */
 /* know form here always internal h:l */
 if ((npauxp = npp->npaux) != NULL && npauxp->lcbi1 != -1)
  {
   sbp = &(sbp[npauxp->lcbi2]);
   rhswid = npauxp->lcbi1 - npauxp->lcbi2 + 1;
  }
 /* notice all z test after selection of actual bits that drive this net */
 if (nonz_only && !__st_vval_isallzs(sbp, rhswid)) return(NULL);
 __st_regab_tostr(s, sbp, rhswid);
 return(s);
}

/*
 * for message build event value and time in passed string
 *
 * notice cannot use __xs strings here since caller will pass as s 
 * this must only be called if there is a scheduled value
 */
extern char *__bld_valofsched(char *s, struct tev_t *tevp)
{
 word32 outv;
 int32 blen;
 byte *sbp;
 struct gate_t *gp;
 struct conta_t *cap;
 struct tenp_t *tenp;
 struct xstk_t *xsp, *xsp2;
 char s1[RECLEN], s2[RECLEN];

 switch ((byte) tevp->tetyp) {
  case TE_G:
   gp = tevp->tu.tegp;
   outv = tevp->outv;
   /* only input TRANIF separate gate can have delay */
   if (gp->g_class == GC_TRANIF)
    {
     if (gp->gpnum != 1) __case_terr(__FILE__, __LINE__);
     if (tevp->outv == 0) strcpy(s1, "**OFF**");
     else if (tevp->outv == 1) strcpy(s1, "**ON**");
     else strcpy(s1, "**UNKNOWN**");  
     break;
    }
   if (gp->g_hasst) __to_vvstnam(s1, outv);
   else __to_vvnam(s1, outv);
   break;
  case TE_CA: 
   /* SJM 09/28/02 - this is passed the per bit conta indexed above */
   cap = tevp->tu.tecap;
   /* first access the rhs value - that is thing scheduled to change to */
   /* if rhs changed, previous scheduled replaced with new */
   /* if delay and multi-fi know schedule will exist */
   blen = cap->lhsx->szu.xclen;
   push_xstk_(xsp, blen);
   /* FIXME - think this should not be called from lhs itree loc. for xmr */
   __ld_perinst_val(xsp->ap, xsp->bp, cap->schd_drv_wp, blen);
   if (cap->ca_hasst)
    {
     push_xstk_(xsp2, 4*blen);
     sbp = (byte *) xsp2->ap;
     /* since rhs eval or saved, never stored with strength - must add here */
     /* conta may drive strength but any input strengths removed */ 
     __st_standval(sbp, xsp, cap->ca_stval);
     __st_regab_tostr(s1, sbp, blen);
     __pop_xstk();
    }
   else __regab_tostr(s1, xsp->ap, xsp->bp, blen, BBIN, FALSE);
   __pop_xstk();
   break;
  case TE_WIRE: case TE_BIDPATH:
   tenp = tevp->tu.tenp;
   outv = tevp->outv;
   if (tenp->tenu.np->n_stren) __to_vvstnam(s1, outv);
   else __to_vvnam(s1, outv);
   break;
  case TE_MIPD_NCHG:
   strcpy(s1, "*NONE*");
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 sprintf(s, "(scheduled %s at %s)", s1, __to_timstr(s2, &(tevp->etime))); 
 return(s);
}

/*
 * emit the showvars variable information prefix
 */
extern char *__bld_showvars_prefix(char *s, struct net_t *np,
 struct gref_t *grp)
{
 char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[RECLEN], s5[RECLEN];

 /* always emit prefix info */
 if (np->iotyp != NON_IO) sprintf(s1, " %s", __to_ptnam(s3, np->iotyp));
 else strcpy(s1, "");
 if (np->n_isavec && np->ntyp != N_INT && np->ntyp != N_TIME
  && np->ntyp != N_REAL) sprintf(s2, " %s", __to_wrange(s3, np));
 else strcpy(s2, "");
 if (np->n_isarr) sprintf(s4, __to_arr_range(s3, np)); else strcpy(s4, "");

 /* if xmr form must emit path given as arg */
 if (grp == NULL) __schop(s3, np->nsym->synam); else __schop(s3, grp->gnam); 
 sprintf(s, "%s%s %s%s %s", s1, s2, s3, s4, bld_wire_telltale(s5, np));
 return(s);
}

/*
 * build special flag tell-tale
 * know NX_CT gone after prep before this called
 */
static char *bld_wire_telltale(char *s, struct net_t *np)
{
 char s1[RECLEN], s2[RECLEN];
 struct xstk_t *xsp;

 sprintf(s, "<%s", __to_wtnam(s1, np));
 if (np->n_signed) strcat(s, " signed");
 if (np->n_isavec && !np->vec_scalared) strcat(s, " vectored");
 if (np->nrngrep == NX_DWIR)
  {
   if (np->nu.rngdwir->n_delrep == DT_PTHDST) strcat(s, " path dest.");
   else strcat(s, " delay");
  }
 if (np->n_isapthsrc) strcat(s, " path src.");
 if (np->n_stren) strcat(s, " strength");
 if (np->n_capsiz != CAP_NONE)
  {
   sprintf(s1, " %s capacitor", __to1_stren_nam(s2,
    __fr_cap_size((int32) np->n_capsiz), 0));
   strcat(s, s1); 
  }
 if (np->n_multfi) strcat(s, " multi-fi");
 if (np->ntraux != NULL) strcat(s, " in tran channel");
 if (np->nlds != NULL) strcat(s, " fo"); 
 if (np->n_hasdvars) strcat(s, " dumpvar"); 
 /* SJM 07/19/02 - if all off no events */
 if (np->dcelst != NULL && __cnt_dcelstels(np->dcelst) > 0)
  strcat(s, " evnts");
 if (np->n_gone) strcat(s, " disconnected");
 if (np->frc_assgn_allocated)
  {
   if (np->ntyp >= NONWIRE_ST)
    {
     if (np->nu2.qcval[2*__inum].qc_active) strcat(s, " forced");
     else if (np->nu2.qcval[2*__inum + 1].qc_active) strcat(s, " assigned");
    }
   else
    {
     push_xstk_(xsp, np->nwid);
     __bld_forcedbits_mask(xsp->ap, np);
     if (!vval_is0_(xsp->ap, np->nwid)) strcat(s, " forced");
     __pop_xstk();
    }
  }
 strcat(s, ">");
 return(s);
}

/*
 * emit a load string - static information - should be emitted only once 
 * load on xmr target 
 */
static void emit1_load(struct net_t *np, struct net_pin_t *npp)
{
 int32 i1, i2, nd_itpop, obwid, ri1, ri2;
 struct inst_t *ip;
 struct itree_t *dritp;
 struct mod_t *mdp;
 struct mod_pin_t *mpp;
 struct gate_t *gp;
 struct conta_t *cap, *cap2;
 struct tchk_t *tcp;
 struct spcpth_t *pthp;
 struct npaux_t *npauxp;
 char ndxs[RECLEN];

 /* SJM 06/03/02 - only call get cor range if npaux exists */
 if ((npauxp = npp->npaux) == NULL) i1 = i2 = -1;
 else __get_cor_range(npauxp->nbi1, npauxp->nbi2, &i1, &i2);
 if (i1 != -1)
  {
   if (np->n_isarr) __getarr_range(np, &ri1, &ri2, &obwid);
   else if (np->n_isavec) __getwir_range(np, &ri1, &ri2);
   else { __arg_terr(__FILE__, __LINE__); ri1 = ri2 = 0; }

   if (i1 == i2) sprintf(ndxs, " [%d]", __unmap_ndx(i1, ri1, ri2));
   else sprintf(ndxs, " [%d:%d]", __unmap_ndx(i1, ri1, ri2),
   __unmap_ndx(i2, ri1, ri2));
  }
 else strcpy(ndxs, "");

 nd_itpop = TRUE;
 switch ((byte) npp->npproctyp) {
  case NP_PROC_INMOD: nd_itpop = FALSE; break; 
  case NP_PROC_GREF:
   /* SJM 04/17/03 - if not end instance of matching downrel path do not */ 
   /* print */
   if (!__match_push_targ_to_ref(npp->np_xmrtyp, npp->npaux->npu.npgrp))
    return;
   break;
  case NP_PROC_FILT:
   /* all rooted xmrs here */
   __push_itstk(npp->npaux->npdownitp);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }

 switch ((byte) npp->npntyp) {
  case NP_ICONN:
   dritp = &(__inst_ptr->in_its[npp->elnpp.eii]);
   ip = dritp->itip;
   mdp = ip->imsym->el.emdp;
   mpp = &(mdp->mpins[npp->obnum]);
   __cvsim_msg("%s   load: instance %s port %s at %s\n",
    ndxs, ip->imsym->el.emdp->msym->synam, __to_mpnam(__xs2, mpp->mpsnam),
    __bld_lineloc(__xs, ip->isym->syfnam_ind, ip->isym->sylin_cnt));
   break;
  case NP_PB_ICONN:
   /* per bit load same except know scalar/bsel */
   dritp = &(__inst_ptr->in_its[npp->elnpp.eii]);
   ip = dritp->itip;
   mdp = ip->imsym->el.emdp;
   /* notic since not emitting decomposed per bit exprs, need master port */
   mpp = &(mdp->mpins[npp->obnum]);
   __cvsim_msg("%s   load: instance %s port %s bit %d at %s\n",
    ndxs, ip->imsym->el.emdp->msym->synam, __to_mpnam(__xs2, mpp->mpsnam),
    npp->pbi, __bld_lineloc(__xs, ip->isym->syfnam_ind, ip->isym->sylin_cnt));
   break;
  case NP_GATE:
   /* current value of gate is state output - next value is event state */
   gp = npp->elnpp.egp;
   if (gp->g_class == GC_UDP) strcpy(__xs2, "udp");
   else strcpy(__xs2, "gate");
   __cvsim_msg("%s   load: %s %s %s (port %d) at %s\n" , ndxs, __xs2,
    gp->gmsym->synam, gp->gsym->synam, npp->obnum + 1, __bld_lineloc(__xs,
    gp->gsym->syfnam_ind, gp->gsym->sylin_cnt));
   break;
  case NP_TRANIF:
   /* this is only for third port of tranif */
   gp = npp->elnpp.egp;
   __cvsim_msg("%s   load: %s %s (port %d - enable) at %s\n" , ndxs,
    gp->gmsym->synam, gp->gsym->synam, npp->obnum + 1, __bld_lineloc(__xs,
    gp->gsym->syfnam_ind, gp->gsym->sylin_cnt));
   break;
  case NP_CONTA:
   cap = npp->elnpp.ecap;
   if (cap->ca_pb_sim)
    {
     cap2 = &(cap->pbcau.pbcaps[npp->pbi]);
     __cvsim_msg("%s   load: continuous assign to %s per bit %d at %s\n",
      ndxs, __msgexpr_tostr(__xs, cap2->lhsx), npp->pbi, 
      __bld_lineloc(__xs2, cap->casym->syfnam_ind, cap->casym->sylin_cnt));
    }
   else
    {
     __cvsim_msg("%s   load: continuous assign to %s at %s\n", ndxs,
      __msgexpr_tostr(__xs, cap->lhsx), 
      __bld_lineloc(__xs2, cap->casym->syfnam_ind, cap->casym->sylin_cnt));
    } 
   break;
  case NP_MDPRT:
   /* notice mipd module port is only driver not load */
   mdp = npp->elnpp.emdp;
   mpp = &(mdp->mpins[npp->obnum]);
   __cvsim_msg("%s   load: module %s port %s at %s\n", ndxs,
    mdp->msym->synam, __to_mpnam(__xs2, mpp->mpsnam),
    __bld_lineloc(__xs, mpp->mpfnam_ind, mpp->mplin_cnt));
   break;
  case NP_PB_MDPRT:
   /* notice mipd module port is only driver not load */
   mdp = npp->elnpp.emdp;
   mpp = &(mdp->mpins[npp->obnum]);
   mpp = &(mpp->pbmpps[npp->pbi]);
   /* here since not emitting exprs, need master mod port */
   __cvsim_msg("%s   load: module %s port %s bit %d at %s\n", ndxs,
    mdp->msym->synam, __to_mpnam(__xs2, mpp->mpsnam), npp->pbi,
    __bld_lineloc(__xs, mpp->mpfnam_ind, mpp->mplin_cnt));
   break;
  case NP_TCHG:
   switch ((byte) npp->chgsubtyp) {
    case NPCHG_TCSTART:
     tcp = npp->elnpp.etchgp->chgu.chgtcp;
     __cvsim_msg(
     "%s   change: %s timing check line %s reference change event\n",
      ndxs, __to_tcnam(__xs, tcp->tchktyp), __bld_lineloc(__xs2,
      tcp->tcsym->syfnam_ind, tcp->tcsym->sylin_cnt));
     break; 
    case NPCHG_TCCHK:
     tcp = npp->elnpp.echktchgp->startchgp->chgu.chgtcp;
     __cvsim_msg("%s   change: %s timing check line %s data check event\n",
      ndxs, __to_tcnam(__xs, tcp->tchktyp),__bld_lineloc(__xs2,
      tcp->tcsym->syfnam_ind, tcp->tcsym->sylin_cnt));
     break; 
    case NPCHG_PTHSRC:
     pthp = npp->elnpp.etchgp->chgu.chgpthp;
     __cvsim_msg("%s   change: path source and %s source port bit change\n",
      ndxs, __bld_lineloc(__xs, pthp->pthsym->syfnam_ind,
      pthp->pthsym->sylin_cnt));
     break; 
    default: __case_terr(__FILE__, __LINE__);
   }  
   break;
  case NP_MIPD_NCHG:
   /* LOOKATME - should maybe add more details to data structure */
   __cvsim_msg("%s   load: MIPD delay device\n", ndxs);
   break;
  /* pull or tfrw arg can only be driver */
  default: __case_terr(__FILE__, __LINE__);
 }
 if (nd_itpop) __pop_itstk();
}

/*
 * ROUTINES TO IMPLEMENT $DUMPVARS SYSTEM TASK
 */

/*
 * ROUTINES TO COLLECT DUMPVARS INFO DURING ONE TIME
 */

/*
 * execute a dumpvars system task - just collect information - written 
 * at end of this time unit __dv_calltime triggers end of slot dump var
 * file output - if off must still always write header 
 *
 * this must be called with nil if none or func. call. operator (hd of list) 
 */
extern void __exec_dumpvars(struct expr_t *argx)
{
 register struct mdvmast_t *mdvp;
 int32 dpth, anum, ii;
 word32 tmp;
 struct mdvmast_t *mdvp2;
 struct itree_t *itp;
 struct gref_t *grp;
 struct sy_t *syp;

 /* multiple dumpvars legal during first time where dumpvars seen */
 /* dv seen means previous dumpvars call failed */
 if (__dv_seen)
  {
   __sgferr(729,
    "all $dumpvars calls must be at same time - previously called at %s",
    __to_timstr(__xs, &__dv_calltime));
   return;
  }
 __dv_calltime = __simtime;
 
 if (argx == NULL)
  {
   if (__dv_hdr != NULL)
    {
     __sgfwarn(516,
      "$dumpvars no argument form replaces previous $dumpvars call");
     /* free master records - wire records will be reused if present */
     for (mdvp = __dv_hdr; mdvp->mdvnxt != NULL;)
      {
       mdvp2 = mdvp->mdvnxt;
       __my_free((char *) mdvp, sizeof(struct mdvmast_t));
       mdvp = mdvp2;
      }
     __dv_hdr = __dv_end = NULL;
    }
   /* mdv_iprt NULL means entire design - common case */
   mdvp = alloc_mdvmast();
   __dv_hdr = __dv_end = mdvp;
   __dv_isall_form = TRUE; 
   goto done;
  }
 /* argument list form, know first is level */
 if (!__get_eval_word(argx->lu.x, &tmp))
  {
   __sgferr(715,
    "$dumpvar depth argument illegal numeric expression %s - 1 used",
    __msgexpr_tostr(__xs, argx->lu.x));
   tmp = 1;
  }
 dpth = (int32) tmp; 
 argx = argx->ru.x;
 for (anum = 0; argx != NULL; argx = argx->ru.x, anum++)
  {
   if (__dv_isall_form)
    {
     __sgfwarn(500,
      "$dumpvars argument %s (number %d) ignored - follows all of design form",
      __msgexpr_tostr(__xs, argx->lu.x), anum);
     continue;
    }

   /* do not need checking since checked at fixup time */
   if (argx->lu.x->optyp == GLBREF)
    {
     grp = argx->lu.x->ru.grp;
     syp =  grp->targsyp; 
     mdvp = alloc_mdvmast();
     /* xmr wire */
     switch ((byte) syp->sytyp) {
      case SYM_N:
       __xmrpush_refgrp_to_targ(grp);
       mdvp->mdv_itprt = __inst_ptr;
       __pop_itstk();
       mdvp->mdv_tskp = grp->targtskp;
       mdvp->mdv_np = syp->el.enp;
       break;
      case SYM_I:
       __xmrpush_refgrp_to_targ(grp);
       itp = __inst_ptr;
       __pop_itstk();
       goto inst_form;
      case SYM_M:
       if ((ii = __ip_indsrch(syp->synam)) == -1)
        __case_terr(__FILE__, __LINE__);
       itp = __it_roots[ii];
inst_form:
       mdvp->mdv_itprt = itp;
       mdvp->mdv_levels = dpth;
       break;
      default: __case_terr(__FILE__, __LINE__);
     }
    }
   /* know this is local wire form */
   else
    {
     mdvp = alloc_mdvmast();
     mdvp->mdv_itprt = __inst_ptr;
     mdvp->mdv_np = argx->lu.x->lu.sy->el.enp;
     if (__cur_thd == NULL) __misc_terr(__FILE__, __LINE__);  

     /* SJM 02/29/00 - need to also get var's containing func task */
     /* LOOKATME - shoudl $dumpvars work in functions - think yes? */
     /* SJM 07/05/01 - if var form dumpvars called not task no no task cntxt */
     if (__cur_thd->assoc_tsk != NULL) mdvp->mdv_tskp = __getcur_scope_tsk();
     syp = mdvp->mdv_np->nsym; 
    }
   if (__dv_end == NULL) __dv_hdr = mdvp; else __dv_end->mdvnxt = mdvp; 
   __dv_end = mdvp;
  }
done:
 __slotend_action |= SE_DUMPVARS;
 __dv_seen = FALSE;
}

/*
 * allocate a new dumpvars argument master record
 * notice these can never be freed - needed for dumping all vars
 */
static struct mdvmast_t *alloc_mdvmast(void)
{
 struct mdvmast_t *mdvp;

 mdvp = (struct mdvmast_t *) __my_malloc(sizeof(struct mdvmast_t));
 mdvp->mdv_levels = 0;
 mdvp->mdv_itprt = NULL;
 mdvp->mdv_tskp = NULL;
 mdvp->mdv_np = NULL;
 mdvp->mdvnxt = NULL;
 return(mdvp);
}

/*
 * ROUTINES TO SETUP DMPV EVENTS AND DUMP DUMPVARS HEADER AT SETUP SLOT END
 */

/*
 * setup the dumpvars header
 * called at end of time when first $dumpvars executed
 */
extern void __setup_dmpvars()
{
 register struct mdvmast_t *mdvp;

 /* SJM 04/20/00 - in case of reset, must not reallocate */ 
 /* but possible for after reset to invoke dumpvars but not original */
 if (__dv_buffer == NULL) __dv_buffer = __my_malloc(DVBUFSIZ);
 __dv_nxti = 0;
 /* case 1 - all form - know only one mdv master */
 if (__dv_isall_form)
  {
   if (__dv_hdr == NULL || __dv_hdr->mdvnxt != NULL
    || __dv_hdr->mdv_itprt != NULL) __misc_terr(__FILE__, __LINE__);

   setup_all_dvars();
  }
 else
  {
   for (mdvp = __dv_hdr; mdvp != NULL; mdvp = mdvp->mdvnxt)
    setup_1argdvars(mdvp);
  }
 sprintf(__xs2, "$date\n    %s\n$end\n", __pv_timestamp);
 __adds(__xs2);
 dv_wr(FALSE);

 sprintf(__xs2, "$version\n    %s%s of %s\n$end\n", __vers, __vers2, __ofdt);
 __adds(__xs2);
 dv_wr(FALSE);

 /* need to use actual time scale - is default right */
 sprintf(__xs2, "$timescale\n    %s\n$end\n", __to_timunitnam(__xs,
  __des_timeprec));
 __adds(__xs2);
 dv_wr(FALSE);

 /* first write the header info for variables form */
 for (mdvp = __dv_hdr; mdvp != NULL; mdvp = mdvp->mdvnxt)
  wr_1argdvhdr(mdvp);
 sprintf(__xs2, "$enddefinitions $end\n");
 __adds(__xs2);
 dv_wr(FALSE);
}

/*
 * set up dumpvars for all instances of all variables of all modules
 * called at end of time slot, $dumpvars called in
 */
static void setup_all_dvars(void)
{
 register int32 ni;
 register struct net_t *np;
 register struct mod_t *mdp;
 register struct task_t *tskp; 

 /* each module */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  { 
   /* add to each variable */
   if (mdp->mnnum != 0)
    {
     for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++)
      {
       if (np->n_isarr) continue;
       turnon_1net_dmpv(np, (struct itree_t *) NULL, (struct task_t *) NULL,
        mdp, FALSE);
      }
    }
   for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt) 
    {

     if (tskp->trnum != 0) 
      {
       for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
        {
         if (np->n_isarr) continue;
         turnon_1net_dmpv(np, (struct itree_t *) NULL, tskp, mdp, FALSE);
        }
      }
    }
  }
}

/*
 * setup the dumpvar delays controls for 1 master dumpvars arg record
 *
 * separate routine writes the headers 
 * know if dump entire design will not get here
 */
static void setup_1argdvars(struct mdvmast_t *mdvp)
{
 struct net_t *np;
 struct mod_t *mdp;
 struct itree_t *itp;

 /* case 1 simple variable */
 if ((np = mdvp->mdv_np) != NULL)
  {
   /* think flatinum not needed here since 1 itree loc. place */
   if (np->n_isarr) return;


   itp = mdvp->mdv_itprt;
   mdp = itp->itip->imsym->el.emdp;
   turnon_1net_dmpv(np, itp, mdvp->mdv_tskp, mdp, FALSE);
  }
 else
  {
   /* case 2 subtree */
   /* descend number of levels - 0 is all, 1 is just current */
   setup_1subtree_allvars(mdvp->mdv_itprt, mdvp->mdv_levels);
  }
}

/*
 * set all varaibles in an instance and under for dumping
 */
static void setup_1subtree_allvars(struct itree_t *itp, int32 level)
{
 register int32 ii;
 struct mod_t *mdp; 
 struct itree_t *down_itp;

 /* must always try to do current */
 mdp = itp->itip->imsym->el.emdp;
 /* notice cannot stop if one mod dv setup since level may have been less */
 /* than current level (i.e. this ones descends more) */ 
 setup_1installvars(mdp, itp);
 mdp->mod_hasdvars = TRUE;

 if (level == 1) return; 
 for (ii = 0; ii < mdp->minum; ii++)
  { 
   down_itp = &(itp->in_its[ii]);
   setup_1subtree_allvars(down_itp, (level != 0) ? level - 1 : 0);
  }
}

/*
 * set up dumpvars for all variables of module at one itree location 
 */
static void setup_1installvars(struct mod_t *mdp, struct itree_t *itp)
{
 register int32 ni;
 register struct net_t *np;
 struct task_t *tskp; 

 /* add to each variable */
 if (mdp->mnnum != 0)
  {
   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++)
    {
     if (np->n_isarr) continue;
     turnon_1net_dmpv(np, itp, NULL, mdp, FALSE);
    }
  }
 for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt) 
  {

   if (tskp->trnum != 0)
    {
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       if (np->n_isarr) continue;

       turnon_1net_dmpv(np, itp, tskp, mdp, FALSE);
      }
    } 
  }
}

/*
 * turn on dumpvars for one net - called only at end of time slot
 *
 * if already turned on for for other instance, just set bit
 * if itp nil, set all bits
 *
 * only get here if at least one net in mod
 */
static void turnon_1net_dmpv(struct net_t *np, struct itree_t *itp,
 struct task_t *tskp, struct mod_t *mdp, int32 repeat_ok)
{ 
 register int32 ni, ii, jj;
 char s1[RECLEN];

 mdp->mod_hasdvars = TRUE;

 /* assign dump vars id numbers for every inst. even if only some used */
 /* LOOKATME - could eliminate bad (hard to read) codes, but for now no */
 /* reason to bother - also problem with continuous regions */
 /* would need to find good contiguous region */
 /* __next_dvnum = skip_bad_codes(np, __next_dvnum); */ 

 /* if this net already has some instance dumpvared do not set up codes */
 if (!np->n_hasdvars)
  {
   np->n_hasdvars = TRUE;
   /* if net has any dumpvars must always use change stores */
   /* but recording now unrelated to dumpvars */ 
   /* SJM 10/10/06 - if dmpv in src legal to turn */
   /* the nchg nd chgstore bit on because needed for dumpvars and starts */
   /* using nchg processing from here on when know no dces */
   if (!np->nchg_nd_chgstore)
    {
     /* DBG remove -- */
     if (!np->dmpv_in_src) __misc_terr(__FILE__, __LINE__);
     /* -- */
     np->nchg_nd_chgstore = TRUE;
    }

   /* if first time any net of module has dumpvars - alloc table */
   /* for all nets in module */ 
   if (mdp->mndvcodtab == NULL)
    {
     mdp->mndvcodtab = __my_malloc(5*mdp->mtotvarnum*mdp->flatinum);
     memset(mdp->mndvcodtab, 0, 5*mdp->mtotvarnum*mdp->flatinum);
    }
   /* 07/02/00 - SJM - fill module net dvcod table for this net */
   /* FIXME - think size should be 8 for alignment */
   ni = np - mdp->mnets;
   for (ii = 0, jj = 5*ni*mdp->flatinum; ii < mdp->flatinum; ii++, jj += 5)
    {
     strcpy(&(mdp->mndvcodtab[jj]), to_dvcode(s1, __next_dvnum + ii));
    }
   /* RELEASE remove ---
   if (__debug_flg)
    {
    __dbg_msg("$$$ setting dvnum base %d for net %s (%d) in %s\n",
     __next_dvnum, np->nsym->synam, ni, mdp->msym->synam); 
    }
   --- */
   __next_dvnum += mdp->flatinum;
  }

 /* set bits in nets nchg action tab for this dumpvared net and maybe inst */
 if (itp == NULL)
  {
   for (ii = 0; ii < mdp->flatinum; ii++)
    {
     /* LOOKATME - here just turn on fact that dumpvared */
     /* otherwide would start too early - before next time step */
     /* i.e. mark not changed but set master dmpv copied to now flag */ 
     np->nchgaction[ii] |=
      (NCHG_DMPVARED | NCHG_DMPVARNOW | NCHG_DMPVNOTCHGED);
    }
   return;
  }
 if (!repeat_ok && ((np->nchgaction[itp->itinum] & NCHG_DMPVARED) != 0))
  {
   __sgfinform(435, "variable %s in instance %s repeated in $dumpvars list",
    np->nsym->synam, __msg_blditree(__xs, itp, tskp));
  }
 np->nchgaction[itp->itinum]
  |= (NCHG_DMPVARED | NCHG_DMPVARNOW | NCHG_DMPVNOTCHGED);
}

/*
 * convert a number to a special character base 93 char number
 * this should be moved to v_cnv
 * notice requires ascii
 * LOOKATME - code ! is possible - is it legal?
 */
static char *to_dvcode(register char *s, register int32 vnum)
{
 s[4] = '\0';
 s[3] = '!' + vnum % 93;
 if (vnum < 93) return(&(s[3]));
 vnum /= 93;
 s[2] = '!' + vnum % 93;
 if (vnum < 93) return(&(s[2]));
 vnum /= 93;
 s[1] = '!' + vnum % 93;
 if (vnum < 93) return(&(s[1]));
 vnum /= 93;
 s[0] = '!' + vnum;
 return(s);
}

/*
 * routine which skip number that are illegal codes
 * for now # (must be illegal) plus one digit and one letter x, z, X, Z 
 */
/* --
static int32 skip_bad_codes(struct net_t *np, int32 nextcod)
{
 register char *cp;
 int32 fr, to, bad;
 char s1[RECLEN];

 if (nextcod > 93) return(nextcod);

 -- notice this list must be in numerical order --
 strcpy(s1, "#0123456789XZxz");
 fr = nextcod;
 to = nextcod + np->nwid - 1; 

 for (cp = s1; *cp != '\0'; cp++) 
  {
   bad = *cp - '!';
   if (fr <= bad && bad <= to)
    {
     if (isdigit(*cp)) nextcod = '9' - '!' + 1;
     else nextcod = bad + 1;
     fr = nextcod;
     to = nextcod + np->nwid - 1;
    }
  }
 return(nextcod);
}
--- */

/*
 * ROUTINES TO WRITE THE DUMPVAR HEADER REFERENCE INFO
 */

/*
 * write the header for 1 master dumpvars arg record
 * done at end of setup time slot 
 */
static void wr_1argdvhdr(struct mdvmast_t *mdvp)
{
 register int32 ii;
 int32 ni, jj;
 struct itree_t *itp, *itp2;
 struct mod_t *mdp;
 struct task_t *tskp;
 struct net_t *np;
 char *dvcodp;

 /* case 0: all of design */
 if (mdvp->mdv_itprt == NULL)
  {
   for (ii = 0; ii < __numtopm; ii++)
    {
     itp = __it_roots[ii];
     sprintf(__xs2, "$scope module %s $end\n", itp->itip->isym->synam); 
     __adds(__xs2);
     dv_wr(FALSE);

     wr_1subtree_allvars(itp, 0);
     sprintf(__xs2, "$upscope $end\n");
     __adds(__xs2);
     dv_wr(FALSE);
    }
   return;
  }

 /* case 1 simple variable */
 if ((np = mdvp->mdv_np) != NULL)
  {
   /* DBG remove --- */
   if (np->n_isarr) __misc_terr(__FILE__, __LINE__);
   /* --- */
   
   /* no need to check for array 1 wire form - illegal if array */
   wr_fromtop_iscopeto(mdvp->mdv_itprt);
   itp2 = mdvp->mdv_itprt;
   mdp = itp2->itip->imsym->el.emdp;
   if ((tskp = mdvp->mdv_tskp) != NULL)
    {
     wr_tskscopeto(tskp->tsksymtab);
     ni = np - mdp->mnets;
     /* 07/02/00 - SJM - find right dvcod table code */
     jj = 5*ni*mdp->flatinum + itp2->itinum; 
     dvcodp = &(mdp->mndvcodtab[jj]);
    }  
   else  
    {
     /* 07/02/00 - SJM - find right dvcod table code */
     ni = np - mdp->mnets;
     jj = 5*(ni*mdp->flatinum + itp2->itinum); 
     dvcodp = &(mdp->mndvcodtab[jj]);
    }
   wr_1vectored_dvdef(np, dvcodp, itp2);
   if (tskp != NULL) wr_tskscopeback(mdvp->mdv_tskp->tsksymtab);
   wr_totop_iscopeback(mdvp->mdv_itprt);
  }
 else
  {
   /* case 2 inst. scope */
   /* this handles all its own scope movements */
   /* case inst. with some levels underneath */
   /* descend number of levels - 0 is all, 1 is just current */
   wr_fromtop_iscopeto(mdvp->mdv_itprt);
   wr_1subtree_allvars(mdvp->mdv_itprt, mdvp->mdv_levels);
   wr_totop_iscopeback(mdvp->mdv_itprt);

   /* AIV 01/28/05 - removed an extra upscope printing */ 
  }
}

/*
 * write the variables for all instances in and under 1 itree module loc.
 * descends level numbers 0 - all
 * when called expects scope to be moved upon return leaves scope at itp
 */
static void wr_1subtree_allvars(struct itree_t *itp, int32 level)
{
 register int32 ii;
 struct mod_t *imdp;
 struct symtab_t *sytp;
 struct itree_t *down_itp;

 wr_1inst_dvhdrs(itp);
 imdp = itp->itip->imsym->el.emdp;
 /* when done scope left at instance same as called */
 if ((sytp = imdp->msymtab->sytofs) != NULL) wr_tasks_dvhdrs(itp, sytp);
 if (level != 1)
  { 
   for (ii = 0; ii < imdp->minum; ii++)
    { 
     down_itp = &(itp->in_its[ii]);
     sprintf(__xs2, "$scope module %s $end\n", down_itp->itip->isym->synam); 
     __adds(__xs2);
     dv_wr(FALSE);

     wr_1subtree_allvars(down_itp, (level != 0) ? level - 1 : 0);
     sprintf(__xs2, "$upscope $end\n");
     __adds(__xs2);
     dv_wr(FALSE);
    }
  }
}

/*
 * write the dv header variables in one module
 * scope set and does not descend or change scope
 */
static void wr_1inst_dvhdrs(struct itree_t *itp)
{
 register int32 ni;
 register struct net_t *np;
 int32 jj;
 struct mod_t *mdp;
 struct itree_t *itp2;
 char *dvcodp;

 mdp = itp->itip->imsym->el.emdp;
 /* write all wires */
 if (mdp->mnnum == 0) return;
 for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++)
  {
   if (np->n_isarr) continue;


   /* SJM - 07/02/00 - now dvcods pre-built as strings */
   jj = 5*(ni*mdp->flatinum + itp->itinum); 
   dvcodp = &(mdp->mndvcodtab[jj]);
   itp2 = itp;   
   wr_1vectored_dvdef(np, dvcodp, itp2);
  } 
}

/*
 * dump the header info for task and all contained tasks
 * handles scoping but assume all dmpvar dces already set up
 */
static void wr_tasks_dvhdrs(struct itree_t *itp,
 register struct symtab_t *sytp)
{
 register int32 ni, ni2;
 register struct net_t *np;
 int32 jj;
 struct task_t *tskp;
 struct mod_t *mdp;
 char *dvcodp;

 mdp = itp->itip->imsym->el.emdp;
 /* write all contained tasks/funcs/lbs - through symbol table list */
 for (; sytp != NULL; sytp = sytp->sytsib)
  {
   tskp = sytp->sypofsyt->el.etskp;
   /* notice must set up scope even if no variables */
   /* if (tskp->tsk_regs == NULL) continue; */
   sprintf(__xs2, "$scope %s %s $end\n", to_dvtsktyp(__xs, tskp->tsktyp),
    tskp->tsksyp->synam);
   __adds(__xs2);
   dv_wr(FALSE);

   if (tskp->trnum != 0)
    {
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       if (np->n_isarr) continue;
       /* 04/15/00 - net index for dmpvars is from mod first net */
       ni2 = np - mdp->mnets;

       /* 07/02/00 - SJM - dvcod values not pre-built */
       jj = 5*(ni2*mdp->flatinum + itp->itinum); 
       dvcodp = &(mdp->mndvcodtab[jj]);
       wr_1vectored_dvdef(np, dvcodp, itp);
      } 
    }
   if (sytp->sytofs != NULL) wr_tasks_dvhdrs(itp, sytp->sytofs);
   sprintf(__xs2, "$upscope $end\n");
   __adds(__xs2);
   dv_wr(FALSE);
  } 
}

/*
 * build string name of task for $dumpvars - all blocks labeled
 */
static char *to_dvtsktyp(char *s, word32 tskt) 
{
 switch ((byte) tskt) {
  case Begin: strcpy(s, "begin"); break;
  case FORK: strcpy(s, "fork"); break;
  case FUNCTION: strcpy(s, "function"); break;
  case TASK: strcpy(s, "task"); break;
 }
 return(s);
}

/*
 * write commands to scope to an itree place
 */
static void wr_fromtop_iscopeto(struct itree_t *itp)
{
 register int32 i;
 register struct itree_t *witp;
 int32 frtoplevs;

 for (witp = itp, frtoplevs = 1;; frtoplevs++)
  { 
   __push_itstk(witp);
   if ((witp = witp->up_it) == NULL) break;
  }
 for (i = 1; i <= frtoplevs; i++) 
  {
   sprintf(__xs2, "$scope module %s $end\n", __inst_ptr->itip->isym->synam); 
   __adds(__xs2);
   dv_wr(FALSE);

   __pop_itstk();
  }
}

static void wr_totop_iscopeback(struct itree_t *itp)
{
 register struct itree_t *witp;

 for (witp = itp;;)
  {
   sprintf(__xs2, "$upscope $end\n");
   __adds(__xs2);
   dv_wr(FALSE);

   if ((witp = witp->up_it) == NULL) break;  
  }
}

/*
 * write the scope into the task/lb/func - must pass tasks symbol
 */
static void wr_tskscopeto(struct symtab_t *sytp)
{
 struct task_t *tskp;

 if (sytp->sytpar != NULL && sytp->sytpar->sypofsyt->sytyp != SYM_M)
  wr_tskscopeto(sytp->sytpar);
 tskp = sytp->sypofsyt->el.etskp;
 sprintf(__xs2, "$scope %s %s $end\n", to_dvtsktyp(__xs, tskp->tsktyp),
  tskp->tsksyp->synam);
 __adds(__xs2);
 dv_wr(FALSE);
}

static void wr_tskscopeback(struct symtab_t *sytp)
{
 struct symtab_t *wsytp;

 for (wsytp = sytp->sytpar;;)
  {
   sprintf(__xs2, "$upscope $end\n");
   __adds(__xs2);
   dv_wr(FALSE);

   /* keep going until back to containing module scope */
   if (wsytp->sypofsyt->sytyp == SYM_M) break;
  }
}

/*
 * write dv define for 1 scalar or vectored wire 
 * never call for array
 */
static void wr_1vectored_dvdef(struct net_t *np, char *dvcod,
 struct itree_t *itp)
{
 int32 r1, r2;
 char s1[15];

 if ((np->nchgaction[itp->itinum] & NCHG_DMPVARED) == 0) return;

 if (np->n_isavec || np->ntyp == N_REAL)
  {
   __getwir_range(np, &r1, &r2);
   /* this must be wire name */ 
   sprintf(__xs2, "$var %s ", __to_wtnam2(s1, np->ntyp)); 
   __adds(__xs2);
   dv_wr(FALSE);

   /* SJM 06/18/01 - no range for dumpvar of reals and width 64 */
   if (np->ntyp == N_REAL)
    {
     sprintf(__xs2, "     64 %-4s %s $end\n", dvcod, np->nsym->synam);
    }
   else
    {
     /* SJM 09/13/99 - range but be MSB to LSB - was internal h:0 */
     sprintf(__xs2, "%7d %-4s %s [%d:%d] $end\n", np->nwid, dvcod,
      np->nsym->synam, r1, r2);
    }
   __adds(__xs2);
   dv_wr(FALSE);
  }
 else
  {
   sprintf(__xs2, "$var %s ", __to_wtnam2(s1, np->ntyp)); 
   __adds(__xs2);
   dv_wr(FALSE);
   sprintf(__xs2, "      1 %-4s %s $end\n", dvcod, np->nsym->synam);
   __adds(__xs2);
   dv_wr(FALSE);
  }
}

/* 07/02/00 - now only one dvcod for each net - never scalared */

/*
 * ROUTINES TO TURN ON/OFF DUMPVARS RECORDING
 */

/*
 * turn off all dumpvars - uses being dumpvared template
 */
extern void __turnoff_all_dumpvars(void)
{
 register int32 ii;
 register struct net_t *np;
 register struct task_t *tskp;
 register struct mod_t *mdp;
 int32 ni;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (!mdp->mod_hasdvars) continue;

   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++)
    {
     if (!np->n_hasdvars) continue;

     for (ii = 0; ii < mdp->flatinum; ii++)
      {
       /* if not dumpvared nothing to do */
       if ((np->nchgaction[ii] & NCHG_DMPVARED) == 0) continue;

       /* dumpv now off and not changed on (even if changed in slot) */
       np->nchgaction[ii] &= ~NCHG_DMPVARNOW;
       np->nchgaction[ii] |= NCHG_DMPVNOTCHGED;
      }
    }
   for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       if (!np->n_hasdvars) continue;

       for (ii = 0; ii < mdp->flatinum; ii++)
        {
         if ((np->nchgaction[ii] & NCHG_DMPVARED) == 0) continue;

         /* dumpv now off and not changed on (even if changed in slot) */
         np->nchgaction[ii] &= ~NCHG_DMPVARNOW;
         np->nchgaction[ii] |= NCHG_DMPVNOTCHGED;
        }
      }
    }
  }
}

/*
 * turn on all dumpvars (from previous turn-off) 
 */
extern void __turnon_all_dumpvars(void)
{
 register int32 ii;
 register struct net_t *np;
 register struct task_t *tskp;
 register struct mod_t *mdp;
 int32 ni;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (!mdp->mod_hasdvars) continue;

   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++)
    {
     if (!np->n_hasdvars) continue;

     for (ii = 0; ii < mdp->flatinum; ii++)
      {
       /* if not dumpvared nothing to do */
       if ((np->nchgaction[ii] & NCHG_DMPVARED) == 0) continue;

       /* dumpv now off and not changed on (even if changed in slot) */
       np->nchgaction[ii] |= NCHG_DMPVARNOW;
       /* not changed bit already on */
      }
    }
   for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       if (!np->n_hasdvars) continue;

       for (ii = 0; ii < mdp->flatinum; ii++)
        {
         if ((np->nchgaction[ii] & NCHG_DMPVARED) == 0) continue;

         /* dumpv now off and not changed on (even if changed in slot) */
         np->nchgaction[ii] |= NCHG_DMPVARNOW;
         /* not changed bit already on */
        }
      }
    }
  }
}

/*
 * ROUTINES TO ACTUALLY DUMP VARIABLES
 */

/*
 * dump all variables for baseline - for start ($dumpvars), dumpon,
 * dumpoff (need x's) and dumpall - keywrds determines which
 * set flags
 */
extern void __do_dmpvars_baseline(char *keyws)
{
 __cur_sofs = 0;
 if (__dv_outlinpos != 0)
  {
   addch_('\n');
   __exprline[__cur_sofs] = '\0'; 
   dv_wr(FALSE);
  }
 if (!__dv_time_emitted)
  {
   sprintf(__xs2, "#%s\n", to_dvtimstr(__xs, __simtime));
   __adds(__xs2);
   __dv_time_emitted = TRUE;
   dv_wr(FALSE);
  }
 
 __adds(keyws);
 dv_wr(TRUE);
 __dv_outlinpos = 0;
 if (strcmp(keyws, "$dumpoff") == 0) __dv_func = DMPV_DUMPX;
 else __dv_func = DMPV_DMPALL; 
 dump_allvars_vals();
 if (__dv_outlinpos != 0) addch_('\n');
 __adds("$end\n");
 dv_wr(FALSE);
 __dv_outlinpos = 0;
}

/*
 * dumpvars convert a 64 bit internal tick time to a decimal string
 *
 * SJM 11/21/03 - now all systems but OSX support word32 64 printf
 */
static char *to_dvtimstr(char *s, register word64 t)
{
#ifdef __APPLE__
 int32 trimblen, widnumlen;
 word32 t1a[2];
 char *cp;

 t1a[0] = (word32) (t & WORDMASK_ULL);
 if (t < WORDMASK_ULL) { sprintf(s, "%lu", t1a[0]); return(s); }

 t1a[1] = (word32) ((t >> 32) & WORDMASK_ULL);
 /* notice this case makes use of c require that fields are in order */
 trimblen = __trim1_0val(t1a, TIMEBITS);
 /* need ceil here */
 widnumlen = (int32) (trimblen*LG2_DIV_LG10 + 0.999999);
 __declcnv_tostr(s, t1a, trimblen, widnumlen);
 /* AIV 11/20/03 need to trim the spaces for #time in vcd files */
 if (*s == ' ')
  {
   for(cp = s; *cp == ' '; cp++);
  } 
 return(cp);
#else
 sprintf(s, "%llu", t);
 return(s);
#endif
}

/*
 * dump all variables for various types of check pointing
 * know when this called positioned at beginning of new line
 */
static void dump_allvars_vals(void)
{
 register int32 ni;
 register struct mod_t *mdp;
 register struct net_t *np;
 register struct task_t *tskp;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (!mdp->mod_hasdvars) continue; 
   if (mdp->mnnum != 0)
    {
     for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++)
      {
       /* if using default only dump vals for col. to col will never */
       /* have dumpvars so do not need special checking */
       /* if dump all even col. from, this is normal code */ 
       if (np->n_hasdvars)
        dmp_insts_ofwire(mdp, np);
      }
    }
   for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {

     if (tskp->trnum == 0) continue;
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      { if (np->n_hasdvars) dmp_insts_ofwire(mdp, np); }
    }
  }
}

/*
 * for a given net dump all instances of it - do not check change bit
 * this is for base lines and any change info left unchanged
 *
 * loop here could be unwound to make it faster
 * must never be called for arrays
 *
 * bld1 dvval uses global xs strings
 */
static void dmp_insts_ofwire(struct mod_t *mdp, struct net_t *np)
{
 register int32 ii; 
 register struct itree_t *itp;
 int32 jj;
 char *dvcodp;

 /* dump all dumpvared instances */
 /* LOOKATME - this is only step much slower in new approach - problem? */ 
 for (ii = 0; ii < mdp->flatinum; ii++) 
  {
   if ((np->nchgaction[ii] & NCHG_DMPVARED) == 0) continue;

   itp = mdp->moditps[ii];

   /* 07/02/00 - SJM - find right dvcod table code */
   jj = 5*((np - mdp->mnets)*mdp->flatinum + itp->itinum); 
   dvcodp = &(mdp->mndvcodtab[jj]);

   /* here for strength vector pass 0th (first-low) bit of vector */
   if (__dv_func == DMPV_DUMPX) bld1_xdvval(np, dvcodp);
   else
    {
     if (np->n_isavec) bld1_vec_dvval(np, dvcodp, itp);
     else bld1_scal_dvval(np, dvcodp, itp);
    }
   dv_wr(TRUE);
  }
} 

/*
 * do the dumpvars changes 
 * this routine requires at least one dv change entry
 */
extern void __do_dmpvars_chg()
{ 
 register struct dvchgnets_t *dvchgnp;
 register struct dvchgnets_t *dvchg_last;
 register struct net_t *np;
 register struct itree_t *itp;
 int32 jj;
 struct mod_t *mdp;
 char *dvcodp;

 if (__dv_outlinpos != 0)
  {
   addch_('\n'); 
   __exprline[__cur_sofs] = '\0'; 
   dv_wr(FALSE);
  }
 if (!__dv_time_emitted)
  {
   sprintf(__xs2, "#%s\n", to_dvtimstr(__xs, __simtime));
   __adds(__xs2);
   dv_wr(FALSE);
   __dv_time_emitted = TRUE;
   /* notice can only check for over limit when emitting new change time */
   if (__dv_dumplimit_size != 0)
    {
     if (__dv_file_size > __dv_dumplimit_size)
      {
       __dv_outlinpos = 0;
       sprintf(__xs2,
        "$comment - Note: $dumplimit %d limit exceeded at %s $end\n",
        __dv_dumplimit_size, __to_timstr(__xs, &__simtime));
       __adds(__xs2);
       dv_wr(FALSE);

       __dv_state = DVST_OVERLIMIT;
       __turnoff_all_dumpvars();

       __cv_msg(
        "  *** Dumping of variables stopped at %s because $dumplimit %d exceeded.\n",
        __to_timstr(__xs, &__simtime), __dv_dumplimit_size);


       /* need to move to end of change list so can free in chunk */
       for (dvchg_last = NULL, dvchgnp = __dv_chgnethdr; dvchgnp != NULL;
         dvchgnp = dvchgnp->dvchgnxt) dvchg_last = dvchgnp;
       goto free_chglist;
      }
    }
  }
 __dv_outlinpos = 0;
 __dv_func = DMPV_CHGONLY;

 dvchg_last = NULL;
 for (dvchgnp = __dv_chgnethdr; dvchgnp != NULL; dvchgnp = dvchgnp->dvchgnxt)
  {
   /* do the dumpvars for one var instance */
   np = dvchgnp->dvchg_np;
   itp = dvchgnp->dvchg_itp;
   mdp = itp->itip->imsym->el.emdp;

   jj = 5*((np - mdp->mnets)*mdp->flatinum + itp->itinum); 
   dvcodp = &(mdp->mndvcodtab[jj]);
   if (np->n_isavec) bld1_vec_dvval(np, dvcodp, itp);
   else bld1_scal_dvval(np, dvcodp, itp);
   dv_wr(TRUE);
   /* reset this inst. not changed flag - i.e. must be on (not changed) */
   /* for next time */ 
   np->nchgaction[itp->itinum] |= NCHG_DMPVNOTCHGED;
   dvchg_last = dvchgnp;
  }
 /* splice list on front of change free list all at once */
 /* if last nil, no changes */
free_chglist:
 if (dvchg_last != NULL)
  {
   dvchg_last->dvchgnxt = __dv_netfreelst;
   __dv_netfreelst = __dv_chgnethdr;
   __dv_chgnethdr = NULL;
  }
}

/*
 * special routine to emit time stamp at end of dumpvars file
 *
 * LOOKATME - does not write if over file size limit but maybe should 
 */
extern void __wr_dvtimstr(void)
{
 if (__dv_state != DVST_DUMPING) return;
 sprintf(__xs2, "#%s\n", to_dvtimstr(__xs, __simtime));
 __adds(__xs2);
 dv_wr(FALSE);
}

static char valtoch_tab[] = { '0', '1', 'z', 'x' };

/*
 * build 1 scalar variable's dumpvar value string into exprline
 * caller must make sure right inum on top of itstk
 * format for vector b[value] [up to 4 char id] - for scale [v][4 char id]
 * for non x/z vector h[value] [id] - for real r[value][id]
 * called only when change known or need to dump all vals
 * notice __exprline can never be in use here 
 */
static void bld1_scal_dvval(struct net_t *np, char *dvcodp,
 struct itree_t *itp)
{
 register word32 v;

 __cur_sofs = 0;
 /* when changed written with 1 - never have value 0 and always 1 bit */
 /* will only be on changed list if cause during this time slot */
 if (np->ntyp == N_EVENT) addch_('1');
 else
  {
   v = (word32) np->nva.bp[itp->itinum];
   addch_(valtoch_tab[v & 3]);
  }
 __adds(dvcodp);  
}

/*
 * build 1 vector variable's dumpvar value string into exprline
 * caller must make sure right inum on top of itstk
 * format for vector b[value] [up to 4 char id] - for scale [v][4 char id]
 * for non x/z vector h[value] [id] - for real r[value][id]
 * called only when change known or need to dump all vals
 * notice __exprline can never be in use here 
 */
static void bld1_vec_dvval(struct net_t *np, char *dvcodp, struct itree_t *itp)
{
 double d1; 
 register struct xstk_t *xsp;

 __cur_sofs = 0;
 /* normal case */
 if (np->ntyp == N_REAL)
  {
   /* double is always 8 bytes stored using b part as extra 4 bytes */
   /* net width is 32 */
   memcpy(&d1, &(np->nva.wp[2*itp->itinum]), sizeof(double));
   sprintf(__xs2, "r%.16g ", d1);   
   __adds(__xs2);
  }
 else
  {
   __push_itstk(itp);
   push_xstk_(xsp, np->nwid);
   __ld_wire_val(xsp->ap, xsp->bp, np);
   addch_('b');
   __sdispb(xsp->ap, xsp->bp, np->nwid, TRUE);
   addch_(' ');
   __pop_xstk();
   __pop_itstk();
  }
 __adds(dvcodp);  
}

/*
 * build 1 variable's dumpvar x value string into exprline
 * format is x[up to 4 char id name] for scalar or bx [id] for vector
 */
static void bld1_xdvval(register struct net_t *np, char *dvcodp)
{
 __cur_sofs = 0;
 if (!np->n_isavec)
  { if (np->ntyp == N_EVENT) addch_('1'); else addch_('x'); }
 /* real x is 0.0 */
 else if (np->ntyp == N_REAL) __adds("r0.0 ");
 else __adds("bx ");
 __adds(dvcodp);
}

/*
 * write the built dump var value
 * know by here line always has ending new line
 */
static void dv_wr(int32 nd_nl)
{
 register int32 new_fsiz;

 if (__cur_sofs == 0) return;
 if (__dv_dumplimit_size != 0)
  {
   if (__dv_state == DVST_OVERLIMIT) return;

   /* LOOKATME - +1 right since it points to \0? */
   /* SJM 11/13/99 - no adding one makes file too small (now matches 1862) */
   new_fsiz = __dv_file_size + __cur_sofs; 
   __dv_file_size = new_fsiz;
  }
 /* know __cur_sofs points one past end of buffer */
 /* 2 overflow cases current line bigger than buffer or does not fit */
 if (__dv_nxti + __cur_sofs >= DVBUFSIZ - 2) 
  {
   /* SJM 03/03/00 - no longer adding nil to end of dv buffer */
   if (__dv_nxti > 0)
    {
     /* DBG remove --
     __cv_msg("writing dumpvars buffer of size %d\n", __dv_nxti);
     --- */
     write(__dv_fd, __dv_buffer, __dv_nxti);
    }
   __dv_nxti = 0;
   /* immediate write if larger than buffer */
   if (__cur_sofs >= DVBUFSIZ - 2)
    {
     if (nd_nl)
      {
       __exprline[__cur_sofs++] = '\n';
       __dv_file_size++;
      }
     write(__dv_fd, __exprline, __cur_sofs); 
     /* DBG remove --
     __cv_msg("writing dumpvars buffer of size %d\n", __cur_sofs + 1);
     --- */
     goto done;
    }
  }
 memcpy(&(__dv_buffer[__dv_nxti]), __exprline, __cur_sofs);

 __dv_nxti += __cur_sofs;
 if (nd_nl)
  {
   __dv_file_size++;
   __dv_buffer[__dv_nxti++] = '\n';
  }

done:
 /* FIXME - what if wrote without new line to add or in string - then wrong? */
 __dv_outlinpos = 0;
 __cur_sofs = 0;
}

/*
 * flush a dumpvars - empty buffer then do OS flush 
 */
extern void __my_dv_flush(void)
{
 if (__dv_nxti > 0) write(__dv_fd, __dv_buffer, __dv_nxti);
 __dv_nxti = 0;

 /* SJM 07/02/00 - now avoiding fwrite buffering -- 
 if (fflush(__dv _ s) == EOF)
  {
   __sgferr(701, "OS failure when flushing $dumpvars file %s: %s",
    __dv_fnam, strerror(errno));
  }
 --- */
}
