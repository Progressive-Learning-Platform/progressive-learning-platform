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
 * Verilog simulation routines
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <signal.h>

#ifdef __DBMALLOC__
#include "../malloc.h"
#endif

#include "v.h"
#include "cvmacros.h"


/* local prototypes */
static void process_pnd0s(void);
static void exec_slotend_dv(void);
static void process_all_netchgs(void);
static void free_chgedvars(void);
static void eval_netchg_lds(register struct net_t *, int32, int32, int32);
static void process_mipd_nchg_ev(register struct tev_t *);
static void emit_nchglds_trmsg(struct net_t *, struct net_pin_t *);
static int32 is2_chg_match(int32, int32, int32);
static void std_chg_logic_gate(register struct gate_t *, register word32);
static void acc_chg_bufnot(struct gate_t *, word32);
static void acc_stichg_bufnot(register struct gate_t *, word32);
static void acc_chg_4igate(register struct gate_t *, word32);
static void acc_stichg_4igate(register struct gate_t *, word32);
static void trace_gunchg(struct gate_t *, word32);
static void std_chg_udp_gate(register struct gate_t *, register word32);
static void std_chg_bufif_gate(register struct gate_t *, register word32);
static void std_chg_mos_gate(register struct gate_t *, register word32);
static void std_chg_cmos_gate(register struct gate_t *, register word32);
static void prop_gatechg(register struct gate_t *, register int32, int32);
static void evtr_prop_gatechg(register struct gate_t *, register word32,
 int32);
static word32 get_showcancele_val(struct gate_t *);
static i_tev_ndx schedule_1gev(register struct gate_t *, word64, word64, int32);
static i_tev_ndx reschedule_1gev(i_tev_ndx, word64, word64, word32, int32);
static void emit_pulsewarn(struct gate_t *, struct tev_t *, word64 *,
 word64 *, char *);
static void process_gatechg_ev(register struct tev_t *);
static void acc_evchg_gate_outwire(register struct gate_t *);
static void emit_gev_trace(struct gate_t *, struct tev_t *);
static void change_gate_outwire(register struct gate_t *);
static void trace_chg_gateout(struct gate_t *, struct expr_t *);
static void gate_st_bit(union pck_u, int32, int32, int32, register word32,
 register word32);
static void gate_st_scalval(register word32 *, register word32, register word32);
static int32 chg_mos_instate(register struct gate_t *, word32);
static int32 chg_cmos_instate(register struct gate_t *, word32);
static void eval_tranif_onoff(struct gate_t *);
static void evtr_eval_conta_rhs_ld(struct net_pin_t *);
static void schedule_1caev(struct conta_t *, word64, word64, struct xstk_t *);
static void reschedule_1caev(i_tev_ndx, word64, word64, struct xstk_t *);
static void process_conta_ev(register struct tev_t *tevp);
static void process_wire_ev(register struct tev_t *tevp);
static void process_nbpa_ev(struct tev_t *tevp);
static int32 force_inhibit_wireassign(struct net_t *, register int32,
 struct itree_t *);
static void process_trpthdst_ev(register struct tev_t *);
static int32 filter_edge_expr(register struct dce_expr_t *, register word32 *,
 register word32 *);
static void trigger_evctrl(struct delctrl_t *, register i_tev_ndx);
static int32 stfilter_dce_chg(register struct net_t *,
 register struct dcevnt_t *, word32 *, word32 *, int32);
static int32 vccb_vec_standval_filter(register struct net_t *,
 register struct dcevnt_t *, word32 *, word32 *, int32);
static int32 vccb_scal_standval_filter(register struct net_t *,
 register struct dcevnt_t *, word32 *, word32 *, int32);
static int32 filter_dce_chg(register struct net_t *, register struct dcevnt_t *,
 word32 *, word32 *, int32);
static int32 scal_stfilter_dce_chg(register struct net_t *,
 register struct dcevnt_t *, word32 *, word32 *, int32);
static void bld_xmrsrc_ref(char *, struct net_t *);
static void bld_srcfilter_ref(char *, word32, word32, struct expr_t *);
static int32 filter_bitchange(register word32, register byte *,
 register word32, struct expr_t *);
static void add_tchk_chged(struct chktchg_t *);
static void process_all_tchk_violations(void);
static void process_notify(struct net_t *);
static char *bld_tchk_srcdump(char *, struct tchk_t *, word64 *, word64 *,
 word64 *, word64 *);
static void init_stime(void);
static void reinit_stime(void);
static void init_wires(void);
static void init_itinsts(struct itree_t *);
static void init_sched_thd(struct mod_t *);
static void gate_initeval(struct gate_t *);
static void init_udp(struct gate_t *);
static void init_logic_gate(struct gate_t *);
static void init_bufif_gate(struct gate_t *);
static void init_tranif_gate(struct gate_t *);
static void conta_initeval(struct conta_t *, struct conta_t *);
static int32 move_to_time0(void);
static int32 move_time(void);
static void chk_event_consist(void);
static void add_ovfetim(word64, i_tev_ndx, struct tev_t *);
static struct bt_t *alloc_btnod(int32);
static struct bt_t *find_fringe(word64);
static struct bt_t *insert_fringe(struct bt_t *, word64, i_tev_ndx);
static void splitinsert_nonfringe(void);
static void ovflow_into_wheel(void);
static void divide_fringe_node(struct bt_t *);
static void divide_internal_node(struct bt_t *);
static void remove_empty_upwards(void);
static void mv_subtree_towheel(struct bt_t *);
static void mv_to_wheel(word64, struct telhdr_t *);
static struct telhdr_t *tfind_btnode_after(struct bt_t *, word64);
static void dmp_twheel(void);
static int32 dmp_events(register i_tev_ndx);
static void dmp_btree(struct bt_t *);
static void dmp2_btree(struct bt_t *, int32);
static void dmp_btnode(struct bt_t *, int32);
static char *to_evtronam(char *, char *, struct itree_t *, struct task_t *);
/* SJM UNUSED static void chk_schd_dces(void); */
static void chk_tev_list(register i_tev_ndx);

/* extern prototypes defined elsewhere */
extern void __pv_sim(void);
extern void __get_cor_range(register int32, union intptr_u, register int32 *,
 register int32 *);
extern void __set_gchg_func(struct gate_t *);
extern void __vpi_set_chg_proc(struct gate_t *);
extern int32 __gate_is_acc(struct gate_t *);
extern void __add_nchglst_el(register struct net_t *);
extern void __add_dmpv_chglst_el(struct net_t *);
extern void __eval_conta_rhs_ld(register struct net_pin_t *);
extern void __eval_tranif_ld(register struct gate_t *, register int32);
extern void __add_select_nchglst_el(register struct net_t *, register int32,
 register int32);
extern void __wakeup_delay_ctrls(register struct net_t *, register int32,
 register int32);
extern void __process_npp_timofchg(struct net_t *,
 register struct net_pin_t *);

extern char *__to_evtrcanam(char *, struct conta_t *, struct itree_t *);
extern void __evtr_resume_msg(void);
extern void __process_getpat(struct conta_t *);
extern char *__to_evtrwnam(char *, struct net_t *, int32, int32,
 struct itree_t *);
extern char *__to_evtrpnam(char *, struct mod_pin_t *, int32,
 struct itree_t *);
extern void __init_sim(void);
extern void __reinit_sim(void);
extern void __insert_event(register i_tev_ndx);
extern void __free_btree(struct bt_t *);
extern void __free_telhdr_tevs(register struct telhdr_t *);
extern void __free_1tev(i_tev_ndx);
extern void __free_xtree(struct expr_t *);
extern void __call_misctfs_simstart(void);
extern void __vpi_startsim_trycall(void);
extern void __call_misctfs_endreset(void);
extern void __vpi_endreset_trycall(void);
extern void __do_interactive_loop(void);
extern void __process_thrd_ev(register struct tev_t *);
extern char *__to_timstr(char *, word64 *);
extern void __setdel_call_misctf(i_tev_ndx);
extern void __process_putpdel_ev(i_tev_ndx);
extern void __process_vpidrv_ev(i_tev_ndx);
extern void __process_vpi_varputv_ev(i_tev_ndx);
extern void __delay_callback(i_tev_ndx);
extern void __sync_call_misctf(struct tev_t *);
extern void __exec_monit(struct dceauxlst_t *, int32);
extern void __exec_fmonits(void);
extern void __exec_strobes(void);
extern void __exec_rosync_misctf(void);
extern void __vpi_del_rosync_call(void);
extern void __vpi_del_nxtsimtim_trycall(void);
extern void __do_dmpvars_baseline(char *);
extern void __turnoff_all_dumpvars(void);
extern void __turnon_all_dumpvars(void);
extern void __do_dmpvars_chg(void);
extern int32 __tilde_creat(char *);
extern int32 __my_creat(char *);
extern void __setup_dmpvars(void);
extern int32 __move_to_npprefloc(struct net_pin_t *);
extern char *__to_wtnam(char *, struct net_t *);
extern char *__to_tetyp(char *, word32);
extern char *__var_tostr(char *, struct net_t *, int32, int32, int32);
extern char *__to_npptyp(char *, struct net_pin_t *);
extern int32 __eval_logic_gate(struct gate_t *, word32, int32 *);
extern void __ld_bit(register word32 *, register word32 *,
 register struct net_t *, int32);
extern int32 __correct_forced_newwireval(struct net_t *, word32 *, word32 *);
extern int32 __forced_inhibit_bitassign(struct net_t *, struct expr_t *,
 struct expr_t *);
extern void __chg_st_bit(struct net_t *, int32, register word32, register word32);
extern word32 __wrd_redxor(word32);
extern int32 __eval_udp(register struct gate_t *, word32, int32 *, int32);
extern int32 __eval_bufif_gate(register struct gate_t *, word32, int32 *);
extern void __eval_pmos_gate(register word32);
extern void __eval_rpmos_gate(register word32);
extern void __eval_nmos_gate(register word32);
extern void __eval_rnmos_gate(register word32);
extern void __eval_cmos_gate(struct gate_t *);
extern int32 __get_acc_class(struct gate_t *);
extern void __hizstrengate_getdel(word64 *, register struct gate_t *);
extern void __get_del(register word64 *, register union del_u, word32);
extern int32 __em_suppr(int32);
extern char *__to_ginam(char *, struct gate_t *, word32, int32);
extern char *__to_gonam(char *, struct gate_t *, word32);
extern char *__msg_blditree(char *, struct itree_t *, struct task_t *);
extern char *__schop(char *, char *);
extern char *__msg2_blditree(char *, struct itree_t *);
extern void __mdr_assign_or_sched(register struct expr_t *);
extern void __exec_conta_assign(struct expr_t *, register word32 *,
 register word32 *, int32);
extern char *__gstate_tostr(char *, struct gate_t *, int32);
extern void __immed_eval_trifchan(struct gate_t *);
extern char *__to_gassign_str(char *, struct expr_t *);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern void __lhsbsel(register word32 *, register int32, word32);
extern struct xstk_t *__ndst_eval_xpr(struct expr_t *);
extern struct xstk_t *__eval2_xpr(register struct expr_t *);
extern void __sgn_xtnd_widen(struct xstk_t *, int32);
extern void __sizchg_widen(register struct xstk_t *, int32);
extern void __narrow_sizchg(register struct xstk_t *, int32);
extern void __fix_widened_toxs(register struct xstk_t *, int32);
extern void __st_perinst_val(union pck_u, int32, register word32 *,
 register word32 *);
extern void __grow_xstk(void);
extern void __grow_tevtab(void);
extern void __chg_xstk_width(struct xstk_t *, int32);
extern void __st_standval(register byte *, register struct xstk_t *, byte);
extern void __stren_exec_ca_concat(struct expr_t *, byte *, int32);
extern void __exec_ca_concat(struct expr_t *, register word32 *,
 register word32 *, int32);
extern int32 __wide_vval_is0(register word32 *, int32);
extern int32 __vval_is1(register word32 *, int32);
extern char *__regab_tostr(char *, word32 *, word32 *, int32, int32, int32);
extern char *__st_regab_tostr(char *, byte *, int32);
extern char *__bld_lineloc(char *, word32, int32);
extern void __my_free(char *, int32);
extern char *__my_malloc(int32);
extern char *__my_realloc(char *, int32 , int32);
extern char *__to_vvnam(char *, word32);
extern char *__xregab_tostr(char *, word32 *, word32 *, int32, struct expr_t *);
extern void __exec2_proc_assign(struct expr_t *, register word32 *,
 register word32 *);
extern int32 __unnormalize_ndx(struct net_t *, int32);
extern char *__to_mpnam(char *, char *);
extern void __eval_tran_1bit(register struct net_t *, register int32);
extern int32 __match_push_targ_to_ref(word32, struct gref_t *);
extern void __assign_qcaf(struct dcevnt_t *);
extern void __pvc_call_misctf(struct dcevnt_t *);
extern void __cbvc_callback(struct dcevnt_t *, struct cbrec_t *, struct h_t *);
extern void __exec_vpi_gateoutcbs(int32);
extern void __add_ev_to_front(register i_tev_ndx);
extern int32 __get_dcewid(struct dcevnt_t *, struct net_t *);
extern void __ld_wire_sect(word32 *, word32 *, struct net_t *, register int32,
 register int32);
extern void __ld_wire_val(register word32 *, register word32 *, struct net_t *);
extern char *__to_tcnam(char *, word32);
extern void __vpi_tchkerr_trycall(struct tchk_t *, struct itree_t *);
extern void __chg_st_val(struct net_t *, register word32 *, register word32 *);
extern void __adds(char *);
extern void __chg_xprline_size(int32);
extern char *__to_edgenam(char *, word32);
extern void __trunc_exprline(int32, int32);
extern char *__pv_stralloc(char *);
extern void __init_interactive(void);
extern void __my_fclose(FILE *);
extern void __init_all_trchans(void);
extern void __init_instdownport_contas(struct itree_t *, struct itree_t *);
extern void __init_instupport_contas(struct itree_t *);
extern struct thread_t *__alloc_thrd(void);
extern void __ld_perinst_val(register word32 *, register word32 *, union pck_u,
 int32);
extern int32 __sim_sigint_handler(void);
extern void __dmp_event_tab(void);
extern void __dmp1_nplstel(struct mod_t *, struct net_t *, struct net_pin_t *);
extern void __push_wrkitstk(struct mod_t *, int32);
extern void __pop_wrkitstk(void);
extern void __sched_mipd_nchg(struct net_t *, int32, struct mipd_t *);
extern void __exec_var_decl_init_assigns(void);

extern void __cv_msg(char *, ...);
extern void __cvsim_msg(char *, ...);
extern void __tr_msg(char *, ...);
extern void __dbg_msg(char *, ...);
extern void __pv_err(int32, char *, ...);
extern void __pv_warn(int32, char *,...);
extern void __gfwarn(int32, word32, int32, char *, ...);
extern void __pv_terr(int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);
extern void __ia_warn(int32, char *, ...);
extern void __my_fprintf(FILE *, char *, ...);

extern word32 __masktab[];
extern byte __stren_map_tab[];
extern byte __hizstren_del_tab[];

/*
 * ROUTINES TO RUN SIMULATION
 */

/*
 * actually run the simulation
 * need better way to handle p0 (events that must go at end) queue
 *
 * LOOKATME - set up so never check entering interact unless one event
 * processed (except -s entry before start of sim)
 */
extern void __pv_sim(void)
{
 register struct tev_t *tevp;
 i_tev_ndx tevp2i;
 struct tev_t *tevp2;

 /* unless quiet mode, need blank line before sim writing */
 __cv_msg("\n");
 /* possible that no events scheduled at 0, must really move time to 0 */
 /* this assumes wrap works ? */
 __simtime = 0xffffffffffffffffULL;
 __cur_twi = -1;

 /* move time to 0 */
 move_to_time0();

 /* now have timing wheel - can run vpi sim controls that are like */
 /* system task execs */
 __can_exec = TRUE;

 /* do not call vpiStartOfSim routine if resetting */
 if (__now_resetting)
  {
   __now_resetting = FALSE;
   if (__tfrec_hdr != NULL) __call_misctfs_endreset();
   if (__have_vpi_actions) __vpi_endreset_trycall();

   /* if no events after reset - nothing to do so terminate */
   if (__num_twhevents == 0 && __btqroot == NULL && __cur_te_hdri == -1
     && __p0_te_hdri == -1)
    {
     __pv_warn(614,
     "no pending statements or events after reset to time 0 - nothing to do");
     return;
    }
  }
 else
  {
   /* no sim (variables) d.s. and time before here */
   /* notice these routines cannot cause inside entry of debugger */
   /* so can call from here - just scan and register */
   if (__tfrec_hdr != NULL) __call_misctfs_simstart();
   if (__have_vpi_actions) __vpi_startsim_trycall();

   /* if no events after initializationand PLI start of sim - nothing to do */
   if (__num_twhevents == 0 && __btqroot == NULL && __cur_te_hdri == -1
     && __p0_te_hdri == -1)
    {
     __pv_warn(614,
      "no pending statements or events after initialization - nothing to do");
     return;
    }
  }

 /* enter immediately if -s option, . here just starts sim */
 __cur_tevpi = -1;

 /* if this is at least 100 entering from debugger reset */
 if (__dbg_stop_before >= 100)
  {
   if (__dbg_stop_before != 101) { __dbg_stop_before = 0; goto no_stop; }
   __dbg_stop_before = 0;
   goto stop;
  }

 /* else use -s option to decide if stop before sim */
 if (__stop_before_sim)
  {
   /* if no interactive ignore stop before sim with warning */
   if (__no_iact)
    {
     __pv_warn(628,
      "-s option ignored - +nointeractive disabled interactive mode");
     goto no_stop;
    }
stop:
   /* interactive loop expects int32 (^c) signal to be ignored */
   __do_interactive_loop();
  }

no_stop:
 /* set up during simulation control c signal handler - can set flag only */
#if defined(INTSIGS)
 signal(SIGINT, __sim_sigint_handler);
#else
 signal(SIGINT, (void (*)()) __sim_sigint_handler);
#endif

 /* SJM 09/30/04 - execute all new Verilog 2001 variable decl assigns */
 /* as the first step in simulation - do not need any events */
 /* SJM 09/30/04 - LOOKATME - could build and schedule separate init block */
 __exec_var_decl_init_assigns();

 /* repeat this loop for every time */
 __processing_pnd0s = FALSE;
 for (;;)
  {
   /* execute events until current time event list empty */
   /* events never added here (maybe to pound 0) */
   __cur_tevpi = __cur_te_hdri;
   for (; __cur_tevpi != -1; __cur_tevpi = __tevtab[__cur_tevpi].tenxti)
    {
     tevp = &(__tevtab[__cur_tevpi]);
     /* canceled because of inertial delay reschedule */
     if (tevp->te_cancel)
      { __num_cancel_tevents++; __num_twhevents--; continue; }

     /* every event has associated itree element */
     __push_itstk(tevp->teitp);
     /* notice before event executed, cur. itp set from event */
     switch ((byte) tevp->tetyp) {
      case TE_THRD:
       __process_thrd_ev(tevp);
       break;
      /* for gates and 1 bit continous assigns */
      case TE_CA: process_conta_ev(tevp); break;
      case TE_G: process_gatechg_ev(tevp); break;
      case TE_WIRE: process_wire_ev(tevp); break;
      case TE_BIDPATH: process_trpthdst_ev(tevp); break;
      case TE_MIPD_NCHG: process_mipd_nchg_ev(tevp); break;
      case TE_NBPA:
       /* 10/27/00 SJM - this is rhs delay that has elapsed - never rep form */
       /* non blocking proc assign, jump to #0 queue to process */
       alloc_tev_(tevp2i, TE_NBPA, tevp->teitp, __simtime);
       /* this moves entire nb records - since not needed here */
       tevp2 = &(__tevtab[tevp2i]);
       /* if present ptr to constant index lhs expr. copy also copied */
       tevp2->tu.tenbpa = tevp->tu.tenbpa;
       tevp->tu.tenbpa = NULL;
       __num_proc_tevents--;
       /* notice tevp not counted and contents freed */
       /* AIV 06/28/05 - if option set add to the end of the nb #0 list */
       if (!__nb_sep_queue)
        {
         if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevp2i;
         else
          {
           __tevtab[__p0_te_endi].tenxti = tevp2i;
           __p0_te_endi = tevp2i;
          }
        }
       else
        {
         /* AIV 07/05/05 - to match XL need nb te list that only processed */
         /* when all pnd 0 done effectively adds another section to current */
         /* time event queue */
         if (__nb_te_hdri == -1) __nb_te_hdri = __nb_te_endi = tevp2i;
         else
          { __tevtab[__nb_te_endi].tenxti = tevp2i; __nb_te_endi = tevp2i; }
        }
       break;
      case TE_TFSETDEL:
       /* RELEASE remove ---
       if (__debug_flg && __ev_tracing)
        {
         __tr_msg("-- processing tf_ set delay misctf call at %s\n",
  	  __to_timstr(__xs, &__simtime));
        }
       --- */
       __setdel_call_misctf(__cur_tevpi);
       break;
      case TE_TFPUTPDEL: __process_putpdel_ev(__cur_tevpi); break;
      case TE_VPIPUTVDEL: __process_vpi_varputv_ev(__cur_tevpi); break;
      case TE_VPIDRVDEL: __process_vpidrv_ev(__cur_tevpi); break;
      case TE_VPICBDEL: __delay_callback(__cur_tevpi); break;
      /* sync event only pound 0 */
      default: __case_terr(__FILE__, __LINE__);
     }
     __num_proc_tevents++;
     __num_twhevents--;
     __pop_itstk();

     /* DBG remove - notice between events itree stack must be empty ---
     if (__itspi != -1) __misc_terr(__FILE__, __LINE__);
     --- */

     /* notice adding to front adds after current which is really done */
     /* see if pending control c, if add to front, next is right header */
     if (__pending_enter_iact) __do_interactive_loop();
    }

   /* done with normal events, free and remove from time wheel */
   if (__cur_te_hdri != -1)
    {
     __tevtab[__cur_te_endi].tenxti = __tefreelsti;
     __tefreelsti = __cur_te_hdri;
     __twheel[__cur_twi]->te_hdri = -1;
     __twheel[__cur_twi]->num_events = 0;
     __cur_te_hdri = __cur_te_endi = -1;
    }

   process_pnd0s();

   /* AIV 07/05/05 - added processing of separate after all pnd0's non */
   /* blocking events processing nb events can add new pnd0's that are */
   /* processed as added and new nb's that are saved and added to do */
   /* after all current level nbs and pnd0s done */
   if (__nb_te_hdri != -1)
    {
     for (;;)
      { 
       __p0_te_hdri = __nb_te_hdri; 
       __p0_te_endi = __nb_te_endi;
       __nb_te_hdri = __nb_te_endi = -1;

       /* notice the move to pnd 0 queue events add to free list in */
       /* process pnd0 routine */
       process_pnd0s();
       if (__nb_te_hdri == -1) break;
     }
    }
   /* --- DBG remove
   if (__debug_flg && __ev_tracing
    && (__nchg_futhdr != NULL || __p0_te_hdri != -1))
     __tr_msg("-- processing #0 end of slot events\n");
   --- */

   /* final step do slot end timing checks and monitoring */
   /* cannot schedule any events from here */
   if (__slotend_action != 0)
    {
     if ((__slotend_action & SE_TCHK_VIOLATION) != 0)
      process_all_tchk_violations();
     if ((__slotend_action & SE_DUMPVARS) != 0) exec_slotend_dv();
     if (__monit_active && ((__slotend_action
      & (SE_MONIT_TRIGGER | SE_MONIT_CHG)) != 0))
      __exec_monit(__monit_dcehdr,
       (int32) (__slotend_action & SE_MONIT_CHG) != 0);

     /* LOOKATME - should monitoroff (on) effect fmonitor (think no) */
     if ((__slotend_action & SE_FMONIT_TRIGGER) != 0) __exec_fmonits();
     if ((__slotend_action & SE_STROBE) != 0) __exec_strobes();
     if ((__slotend_action & SE_TFROSYNC) != 0) __exec_rosync_misctf();
     if ((__slotend_action & SE_VPIROSYNC) != 0) __vpi_del_rosync_call();
     __slotend_action = 0;
    }
   /* contrl c here serviced at beginning of next time slot */
   /* if no more events done */
   if (!move_time()) break;

   /* call backs from vpi cb NextSimTime (after debugger entered) */
   if (__have_vpi_actions) __vpi_del_nxtsimtim_trycall();
  }
}

/*
 *  process all net changes - this can only enter pnd0 events at now
 * next process net changes, if any new pnd0's process
 * may then add new net changes that in turn can add pnd0's
 * if no 0 delay loop (must catch) will eventually terminate
 * can add normal delays but will occur in future
 */
static void process_pnd0s(void)
{
 register struct tev_t *tevp;

 for (__processing_pnd0s = TRUE, __cur_tevpi = -1;;)
  {
   if (__nchg_futhdr != NULL) process_all_netchgs();

   /* needed in case PLI tf_dostop or vpi_control(vpiStop called */
   if (__pending_enter_iact) __do_interactive_loop();

   /* no pending net changes and no more pound 0 events, can move time */
   if (__p0_te_hdri == -1) break;

   /* every event has associated itree element */
   __cur_tevpi = __p0_te_hdri;
   for (; __cur_tevpi != -1; __cur_tevpi = __tevtab[__cur_tevpi].tenxti)
    {
     tevp = &(__tevtab[__cur_tevpi]);

     /* canceled because interactive thread disabled */
     /* but pound 0 events not counted as timing wheel events */
     if (tevp->te_cancel) { __num_cancel_tevents++; continue; }

     /* notice, pnd0 never canceled since can just replace guts */
     __push_itstk(tevp->teitp);

     /* notice before event executed, cur. itp set from event */
     switch ((byte) tevp->tetyp) {
      case TE_THRD:
       __process_thrd_ev(tevp);
       break;
       break;
      case TE_CA: process_conta_ev(tevp); break;
      /* for gates and 1 bit continous assigns */
      case TE_G: process_gatechg_ev(tevp); break;
      case TE_WIRE: process_wire_ev(tevp); break;
      case TE_BIDPATH: process_trpthdst_ev(tevp); break;
      /* #0 here is normal 0 delay - start as no delay */
      case TE_MIPD_NCHG: process_mipd_nchg_ev(tevp); break;
      case TE_NBPA: process_nbpa_ev(tevp); break;
      case TE_TFSETDEL: __setdel_call_misctf(__cur_tevpi); break;

      case TE_TFPUTPDEL: __process_putpdel_ev(__cur_tevpi); break;
      case TE_VPIPUTVDEL: __process_vpi_varputv_ev(__cur_tevpi); break;
      case TE_VPIDRVDEL: __process_vpidrv_ev(__cur_tevpi); break;
      case TE_VPICBDEL: __delay_callback(__cur_tevpi); break;
      case TE_SYNC: __sync_call_misctf(tevp); break;
      default: __case_terr(__FILE__, __LINE__);
      }
     __num_proc_tevents++;
     /* when put into pnd0 list, no inc. of number of twheel events */
     __pop_itstk();
     /* here cur_tevpi done so any add to front after it */
     if (__pending_enter_iact) __do_interactive_loop();
    }

   /* all #0 events for this time slot processed but may be new net chgs */
   if (__p0_te_hdri != -1)
    {
     __tevtab[__p0_te_endi].tenxti = __tefreelsti;
     __tefreelsti = __p0_te_hdri;
     __p0_te_hdri = __p0_te_endi = -1;
    }
   __cur_tevpi = -1;
  }
 __processing_pnd0s = FALSE;
}

/*
 * routine to exec dump vars functions
 *
 * notice only normal dumpvars will change to over file size limit state
 * from then on until flush or limit change will not do any dumping
 * or for that matter recording.
 */
static void exec_slotend_dv(void)
{
 /* must only emit time once in all processing */
 __dv_time_emitted = FALSE;
 /* first execute any dumpall */
 if ((__slotend_action & SE_DUMPALL) != 0)
  {
   /* if over limit silently do nothing */
   /* dump all is indpendent of normal dumpvars processing */
   if (__dv_state != DVST_OVERLIMIT)
    __do_dmpvars_baseline("$dumpall");
  }

 switch ((byte) __dv_state) {
  case DVST_DUMPING:
   /* if encountered dumpoff, handle here */
   if ((__slotend_action & SE_DUMPOFF) != 0)
    {
     /* remove any pending changes */
     if (__dv_chgnethdr != NULL) free_chgedvars();
     __do_dmpvars_baseline("$dumpoff");
     __dv_state = DVST_NOTDUMPING;
     __turnoff_all_dumpvars();
     break;
    }
   /* if no changes this time slot, nothing to do */
   /* on file over dump limit, will return */
   if (__dv_chgnethdr != NULL) __do_dmpvars_chg();
   break;
  case DVST_NOTDUMPING:
   /* if not dumpon and no dump on action do nothing here */
   if ((__slotend_action & SE_DUMPON) != 0)
    {
     /* start with baseline dump */
     __do_dmpvars_baseline("$dumpon");
     __dv_state = DVST_DUMPING;
     /* turn on dump change recording and dumping for next time slot */
     __turnon_all_dumpvars();
    }
   break;
  /* if not set up only way for pending to be on if need to setup */
  case DVST_NOTSETUP:

   /* try to open the file */
   if ((__dv_fd = __tilde_creat(__dv_fnam)) == -1)
    {
     if (strcmp(__dv_fnam, DFLTDVFNAM) == 0)
      {
bad_dvfnam:
       __pv_err(759,
	"cannot open $dumpvars output file at %s - $dumpvars not executed",
	 __to_timstr(__xs, &__simtime));
       /* not setup but dv seen stops any future setup */
       __dv_seen = TRUE;
       __dv_state = DVST_NOTSETUP;
       return;
      }
     else
      {
       __pv_warn(589, "cannot open $dumpvars output file %s trying %s",
	__dv_fnam, DFLTDVFNAM);
       strcpy(__dv_fnam, DFLTDVFNAM);
       if ((__dv_fd = __my_creat(__dv_fnam)) == -1) goto bad_dvfnam;
      }
    }
   /* write the file reference header and setup dv "events" on wires */
   __setup_dmpvars();
   __do_dmpvars_baseline("$dumpvars");
   if (__verbose)
    {
     __cv_msg(
      "  $dumpvars setup complete at %s - variables dumped to file %s.\n",
      __to_timstr(__xs, &__simtime), __dv_fnam);
    }
   if (__dv_state != DVST_OVERLIMIT)
    {
     __dv_state = DVST_DUMPING;
     /* enable dmpv change recording */
    }
   /* more setup now disabled */
   __dv_seen = TRUE;
   break;
  case DVST_OVERLIMIT:
   if (__dv_chgnethdr != NULL) __misc_terr(__FILE__, __LINE__);
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * reset and free all changed vars when dump all needed
 *
 * in case of dump all - from dv state change may be some changed
 * vars that need to be reset
 */
static void free_chgedvars(void)
{
 register struct dvchgnets_t *dvchgnp;
 struct dvchgnets_t *dvchg_last;

 dvchg_last = NULL;
 /* need to find end to free because putting on front */
 for (dvchgnp = __dv_chgnethdr; dvchgnp != NULL; dvchgnp = dvchgnp->dvchgnxt)
  dvchg_last = dvchgnp;
 /* nothing was on list if last nil */
 if (dvchg_last != NULL)
  {
   dvchg_last->dvchgnxt = __dv_netfreelst;
   __dv_netfreelst = __dv_chgnethdr;
   __dv_chgnethdr = NULL;
  }
}

/*
 * AFTER CHANGE PROPOGATION FROM RHS ROUTINES
 */

/*
 * go through processing all nets (maybe a select) that changed
 * a pass may create more net changes that are in turn processed
 * until list empty
 * this is heuristic to try to cause breath first processing
 * when done future net change list empty
 *
 * if net (probably reg) has no load and no dcelst els not added to chg list
 *
 * SJM - 06/19/00 - now only save up and process structural net changes
 * event controls must be checked (and for force/release) done immediately
 */
static void process_all_netchgs(void)
{
 register struct net_t *np;
 register struct nchglst_t *nchglp, *last_nchglp;
 struct nchglst_t *sav_nchglp;
 int32 num_this_pass, num_passes, total_num;

 num_passes = 0;
 total_num = 0;
 for (;;)
  {
   if (__nchg_futhdr == NULL) break;

   /* point nchglp to all pending net chg elements for processing */
   nchglp = __nchg_futhdr;
   /* save head so can free at end of pass */
   sav_nchglp = nchglp;
   /* empty future so all net change elements added here will be put on */
   /* end of list and processed when all these done - breadth first */
   __nchg_futend = __nchg_futhdr = NULL;
   last_nchglp = NULL;
   for (num_this_pass = 0; nchglp != NULL; nchglp = nchglp->nchglnxt)
    {
     np = nchglp->chgnp;
     /* must eval. in definition itree loc. */
     __push_itstk(nchglp->nchg_itp);
     /* SJM 04/19/01 - must turn off all changed to allow load propagation */
     /* switch channel changes to be added to next pass change list */
     /* turn off all changed - if get here know has n lds */
     np->nchgaction[__inum] &= (~NCHG_ALL_CHGED);

     /* DEBUG remove ---
     {
      struct net_pin_t *npp;

      __dbg_msg("*** dumping loads for net %s (itp=%s)***\n",
       np->nsym->synam, __msg2_blditree(__xs, __inst_ptr));
      for (npp = np->nlds; npp != NULL; npp = npp->npnxt)
       {
        __dmp1_nplstel(__inst_mod, np, npp);
       }
      __dbg_msg("*** end of loads ***\n");
     }
    --- */

     /* SJM 07/24/00 - propagate changes to dces for wires at end of queue */
     /* new algorithm - for regs immediate propagate, for wires end of queue */
     /* LOOKATME - think event controls should be before lds */
     if (np->ntyp < NONWIRE_ST && np->dcelst != NULL)
      __wakeup_delay_ctrls(np, nchglp->bi1, nchglp->bi2);

     /* SJM 07/24/00 - for wires with no lds but only dces still need */
     /* to record nothing to do here */
     if (np->nlds != NULL)
      eval_netchg_lds(np, nchglp->bi1, nchglp->bi2, nchglp->delayed_mipd);

     last_nchglp = nchglp;
     num_this_pass++;
     __pop_itstk();
    }
   total_num += num_this_pass;
   if (++num_passes > 1000 && (num_passes % 1000) == 0)
    {
     if (__pending_enter_iact)
      {
       __ia_warn(1604,
         "interactive mode probably entered from zero delay oscillation - no scheduling");
       __do_interactive_loop();
      }
    }

   /* know last nchg lp set since routine not called if at least one */
   /* SJM 08/02/01 - add if to keep lint happy */
   if (last_nchglp != NULL) last_nchglp->nchglnxt = __nchgfreelst;
   __nchgfreelst = sav_nchglp;
   /* LINUX DBG - add me */
   /* chk_nchgnlst(__nchgfreelst); */
   /* --- */
  }
 if (__debug_flg && __ev_tracing)
  {
   __tr_msg("-- net change event scheduling %d processed in %d passes\n",
    total_num, num_passes);
  }
 __num_netchges += total_num;
}

/*
 * check nchg free list
 */
/* UNUSED LINUX DEBUG ---
void chk_nchgnlst(struct nchglst_t *hdr)
{
 register struct nchglst_t *nchglp;
 int32 ndx;

 ndx = 0;
 for (nchglp = hdr; nchglp != NULL; nchglp = nchglp->nchglnxt)
  {
   if ((void *) nchglp > (void *) 0x13257400)
    {
     __tr_msg("problem at index %d\n", ndx);
     __misc_terr(__FILE__, __LINE__);
    }
   if (nchglp->nchglnxt > (void *) 0x13257400)
    {
     __tr_msg("problem at index %d\n", ndx);
     __misc_terr(__FILE__, __LINE__);
    }
   ndx++;
  }
}
--- */

/*
 * after changed net (wire or reg) go through loads evaluating the
 * load net's drivers assigning to the load net a new value
 *
 * bit range passed and used to eliminate fan-out for other bit here
 * all ranges here normalized high to low form
 * notice will neve get to event trigger through this path (through cause)
 * this is called with current itstk set to wire targ. (maybe target of xmr)
 *
 * the driver evaluations caused by this cause any changed wires to be
 * added to a list which is then used to provide the next pass of wire
 * loads
 */
static void eval_netchg_lds(register struct net_t *np, int32 chgi1, int32 chgi2,
 int32 is_delayed_mipd)
{
 register struct net_pin_t *npp;
 register struct npaux_t *npauxp;
 register int32 bi;
 int32 nd_itpop;
 struct mod_t *downmdp;
 struct mod_pin_t *mpp;
 struct itree_t *itp;
 struct inst_t *ip;
 struct gate_t *gp;
 struct mipd_t *mipdp;

 if (__ev_tracing)
  {
   __evtr_resume_msg();
   __tr_msg("-- evaluating loads of reg/wire %s\n",
    __to_evtrwnam(__xs, np, chgi1, chgi2, __inst_ptr));
  }

 /* must process all loads on net */
 for (npp = np->nlds; npp != NULL; npp = npp->npnxt)
  {
   /* immediately filter out - npp's that require particular inst. */
   if (npp->npproctyp == NP_PROC_FILT
    && npp->npaux->npu.filtitp != __inst_ptr)
    continue;

   /* first need non empty union with 2 ranges */
   /* case 1: all bits of changed or driven bits unknown */
   if ((npauxp = npp->npaux) == NULL || npauxp->nbi1 == -1 || chgi1 == -1)
    goto got_match;
   /* case 2: range of npp is IS form */
   if (npauxp->nbi1 == -2)
    {
     if (is2_chg_match(npauxp->nbi2.xvi, chgi1, chgi2))
      goto got_match;
     continue;
    }
   /* case 3: must check to see if net chg in range */
   if (chgi1 < npauxp->nbi2.i || chgi2 > npauxp->nbi1) continue;

got_match:
   /* process various xmr special cases */
   /* know any instance filtering done before here */
   /* move from definition target xmr loc. back to ref. loc */
   /* for vpi_ just pushed same inst. on to stack again since no ref. loc. */
   if (npp->npproctyp != NP_PROC_INMOD)
    {
     /* SJM 04/17/03 - if XMR path does not match, do not eval */
     if (!__move_to_npprefloc(npp)) continue;
     nd_itpop = TRUE;
    }
   else nd_itpop = FALSE;

   /* maybe some tracing info */
   if (__ev_tracing) emit_nchglds_trmsg(np, npp);

   switch ((byte) npp->npntyp) {
    case NP_ICONN:
     /* notice iconn load (rhs) causes assign to down lhs mdprt */
     /* but iconn driver assigns from down rhs to iconn lhs for out port */
     __immed_assigns++;
     /* SJM 09/08/01 - can now remove this consistency check */
     /* DBG remove ---
     if (npp->elnpp.eii >= __inst_ptr->itip->imsym->el.emdp->minum)
      __misc_terr(__FILE__, __LINE__);
     --- */

     itp = &(__inst_ptr->in_its[npp->elnpp.eii]);
     ip = itp->itip;
     downmdp = ip->imsym->el.emdp;
     /* SJM 09/08/01 - can now remove this consistency check */
     /* DBG remove ---
     if (npp->obnum >= downmdp->mpnum) __misc_terr(__FILE__, __LINE__);
     --- */
     mpp = &(downmdp->mpins[npp->obnum]);
     /* assign from rhs up rhs iconn to lhs down mpp ref. for input port */
     /* notice down always take only 4 args, down do not have first mpp */
     (*mpp->mpaf.mpp_downassgnfunc)(mpp->mpref, ip->ipins[npp->obnum], itp);
     break;
    case NP_PB_ICONN:
     /* notice iconn load (rhs) causes assign to down lhs mdprt */
     /* but iconn driver assigns from down rhs to iconn lhs for out port */
     __immed_assigns++;
     /* SJM 09/08/01 - can now remove this consistency check */
     /* DBG remove ---
     if (npp->elnpp.eii >= __inst_ptr->itip->imsym->el.emdp->minum)
      __misc_terr(__FILE__, __LINE__);
     --- */
     itp = &(__inst_ptr->in_its[npp->elnpp.eii]);
     ip = itp->itip;
     downmdp = ip->imsym->el.emdp;
     /* SJM 09/08/01 - can now remove this consistency check */
     /* DBG remove ---
     if (npp->obnum >= downmdp->mpnum) __misc_terr(__FILE__, __LINE__);
     --- */
     mpp = &(downmdp->mpins[npp->obnum]);
     mpp = &(mpp->pbmpps[npp->pbi]);
     /* assign from rhs up rhs iconn to lhs down mpp ref. for input port */
     /* notice down always take only 4 args, down do not have first mpp */
     (*mpp->mpaf.mpp_downassgnfunc)(mpp->mpref,
      ip->pb_ipins_tab[npp->obnum][npp->pbi], itp);
     break;
    case NP_MDPRT:
     /* top of itstk determines which module inst this is and */
     /* which up instance port to assign to for output port */
     __immed_assigns++;
     downmdp = npp->elnpp.emdp;
     /* DBG remove --- */
     if (npp->obnum >= downmdp->mpnum) __misc_terr(__FILE__, __LINE__);
     /* --- */
     mpp = &(downmdp->mpins[npp->obnum]);
     itp = __inst_ptr->up_it;
     /* DBG remove - bug if trying to assign output of top module --- */
     if (itp == NULL) __misc_terr(__FILE__, __LINE__);
     /* --- */
     /* assign from rhs down mpp ref. to up lhs iconn for output port */
     /* for input port, assign from rhs up iconn to down mod port */
     /* notice up always take only 3 args, down have extra 1st arg mpp */
     (*mpp->mpaf.mpp_upassgnfunc)(__inst_ptr->itip->ipins[npp->obnum],
      mpp->mpref, itp);
     break;
    case NP_PB_MDPRT:
     /* top of itstk determines which module inst this is and */
     /* which up instance port to assign to for output port */
     __immed_assigns++;
     downmdp = npp->elnpp.emdp;
     mpp = &(downmdp->mpins[npp->obnum]);
     mpp = &(mpp->pbmpps[npp->pbi]);
     itp = __inst_ptr->up_it;
     (*mpp->mpaf.mpp_upassgnfunc)(
      __inst_ptr->itip->pb_ipins_tab[npp->obnum][npp->pbi],
      mpp->mpref, itp);
     break;
    case NP_GATE:
     /* evaluate load that is gate input and probably schedule gate chg */
     gp = npp->elnpp.egp;
     (*gp->gchg_func)(gp, npp->obnum);
     break;
    case NP_CONTA:
     /* know input that changed is always port 0 (only input) */
     /* SJM - 09/18/02 - for per bit rhs concat form same net pin type */
     __eval_conta_rhs_ld(npp);
     break;
    case NP_TRANIF:
     __eval_tranif_ld(npp->elnpp.egp, (int32) npp->obnum);
     break;
    case NP_TCHG:
     __process_npp_timofchg(np, npp);
     break;
    case NP_MIPD_NCHG:
     /* SJM 07/09/01 - for MIPD inserted between net(s) that connect to port */
     /* and loads, net pin t that causes schedule before processing rest */

     /* DBG remove --- */
     if (np->nlds != npp) __misc_terr(__FILE__, __LINE__);
     /* --- */

     /* SJM 07/13/01 if mipd net load processing already delayed, skip sched */
     /* and stop processing, algorithm is to store port conn net val and */
     /* sched ev, then ev processing routine puts on nchg list as if store */
     /* had happened after del - can't propagate MIPD nchges until normal */
     /* nchg loop */

     if (is_delayed_mipd) break;

     /* scalar is special case */
     if (!np->n_isavec)
      {
       mipdp = &(npp->elnpp.emipdbits[0]);
       if (mipdp->no_mipd) break;
       __sched_mipd_nchg(np, -1, mipdp);
      }
     else if (chgi1 == -1)
      {
       for (bi = np->nwid - 1; bi >= 0; bi--)
        {
         mipdp = &(npp->elnpp.emipdbits[bi]);
         /* SJM 07/24/05 - must process all bits even if middle no mipd */
         if (mipdp->no_mipd) continue;
         __sched_mipd_nchg(np, bi, mipdp);
        }
      }
     else
      {
       for (bi = chgi1; bi >= chgi2; bi--)
        {
         mipdp = &(npp->elnpp.emipdbits[bi]);
         /* SJM 07/24/05 - must process all bits even if middle no mipd */
         if (mipdp->no_mipd) continue;
         __sched_mipd_nchg(np, bi, mipdp);
        }
      }
     if (nd_itpop) __pop_itstk();
     /* notice must return since because of MIPD wasn't really changed */
     /* works because mipd npp always first on list */
     return;
     /* pull driver only illlegal here */
    default: __case_terr(__FILE__, __LINE__);
   }
   if (nd_itpop) __pop_itstk();
  }
}

/*
 * process MIPD event
 *
 * simple proc that just processes all but first MIPD delay schedule NPP
 * routine here since same as eval nchgs
 *
 * all MIPD events do is delay propagation from changed net(s) connected
 * to input or inout ports to its loads by delay amount - tricky part is
 * path-src delay value calculation
 */
static void process_mipd_nchg_ev(struct tev_t *tevp)
{
 register struct net_t *np;
 register struct mipd_t *mipdp;
 int32 bi, bi2;

 np = tevp->tu.tenp->tenu.np;
 bi = tevp->tu.tenp->nbi;

 if (__ev_tracing)
  {
   __evtr_resume_msg();
   __tr_msg("-- tracing MIPD event for %s\n",
    __to_evtrwnam(__xs, np, bi, bi, __inst_ptr));
  }

 /* turn off mipd sheduled event */
 bi2 = (bi == -1) ? 0 : bi;
 mipdp = &(np->nlds->elnpp.emipdbits[bi2]);
 mipdp->mipdschd_tevs[__inum] = -1;

 /* add to nchg list if net not already all changed */
 /* dmpvars bits chg not turned on since dumpvar happens when net changes */
 /* also can't use normal macro, because never need reg dce wakeup */
 if (!np->n_isavec)
  {
   if ((np->nchgaction[__inum] & NCHG_ALL_CHGED) == 0)
    {
     __add_nchglst_el(np);
     /* SJM 19/01/02 - T because this is 2nd delayed event one so must */
     /* not schedule */
     /* BEWARE - this assumes last element added to end of list */
     __nchg_futend->delayed_mipd = TRUE;
    }
  }
 else
  {
   if ((np->nchgaction[__inum] & NCHG_ALL_CHGED) == 0)
    {
     __add_select_nchglst_el(np, bi, bi);
     /* SJM 19/01/02 - T because this is 2nd delayed event one so must */
     /* not schedule */
     /* BEWARE - this assumes last element added to end of list */
     __nchg_futend->delayed_mipd = TRUE;
    }
  }

 /* free mipd event auxialiary field here since bit and wire extracted */
 __my_free((char *) tevp->tu.tenp, sizeof(struct tenp_t));
 tevp->tu.tenp = NULL;
}

/*
 * emit eval netchg lds trace message
 */
static void emit_nchglds_trmsg(struct net_t *np, struct net_pin_t *npp)
{
 int32 i1, i2;
 struct npaux_t *npauxp;
 char s1[RECLEN], s2[RECLEN];

 __evtr_resume_msg();
 if (__debug_flg)
  {
   if ((npauxp = npp->npaux) == NULL) i1 = i2 = -1;
   else __get_cor_range(npauxp->nbi1, npauxp->nbi2, &i1, &i2);
   __tr_msg("-- after %s %s changed to %s processing %s\n",
    __to_wtnam(s1, np), __to_evtrwnam(__xs, np, i1, i2, __inst_ptr),
   __var_tostr(__xs2, np, i1, i2, BHEX), __to_npptyp(s2, npp));
  }
}

/*
 * return T if is IS2 form bit range (depends on itree place) matches
 * T if bit inside npi1..npi2
 * will overlap unless either high changed below low of range
 * or low changed above high of range
 *
 * SJM 10/12/04 - changed to pass contab ndx instead of ptr since contab
 * realloced
 */
static int32 is2_chg_match(int32 nbi2_xvi, int32 npi1, int32 npi2)
{
 int32 i1;
 word32 *wp;

 wp = &(__contab[nbi2_xvi]);
 wp = &(wp[2*__inum]);
 /* if value x - force match since unknown - only can happen for procedural */
 if (wp[1] != 0L) return(TRUE);
 i1 = (int32) wp[0];
 /* know form here h:0 */
 return(npi1 >= i1 && npi2 <= i1);
}

/*
 * get an possibly correctd for is2 form bit index (part select always split)
 */
extern void __get_cor_range(register int32 oi1, union intptr_u oi2,
 register int32 *i1, register int32 *i2)
{
 register word32 *wp;

 if (oi1 != -2) { *i1 = oi1; *i2 = oi2.i; }
 else
  {
   /* SJM 10/12/04 - because contab realloced, must be ndx base of IS */
   wp = &(__contab[oi2.xvi]);
   *i1 = *i2 = (int32) wp[2*__inum];
  }
}

/*
 * DECLARATIVE EVENT SCHEDULING/PROCESSING ROUTINES
 */

/*
 * GATE EVENT ROUTINES
 */

/*
 * GENERAL COMMENTS FOR ALL ROUTINES:
 *
 * evaluate a gate because gate input changed and the gate input is
 * a load of the changed wire
 *
 * when gate gp input i of inst. cur. itp changes, eval. gate and maybe
 * schedule output change if has delay and output changed
 * after changing input in gate/inst. state vector
 *
 * notice 1 bit conta's are transformed to gates during fix up
 * so changes handled here
 *
 * after these if inputs differ all of old gate value, new gate val,
 * old gate strength and new gate strength set
 */

/* --- unused non proc call gate eval routine
static void eval_gatein_ld(struct gate_t *gp, int32 i)
{
 switch ((byte) gp->g_class) {
  case GC_LOGIC: std_chg_logic_gate(gp, i); break;
  case GC_UDP: std_chg_udp_gate(gp, i); break;
  case GC_BUFIF: std_chg_bufif_gate(gp, i); break;
  case GC_MOS: std_chg_mos_gate(gp, i); break;
  case GC_CMOS: std_chg_cmos_gate(gp, i); break;
  default: __case_terr(__FILE__, __LINE__);
 }
}
--- */

/*
 * evaluate a logic - std not optimized version
 */
static void std_chg_logic_gate(register struct gate_t *gp, register word32 i)
{
 int32 out_chg;

 if (!__eval_logic_gate(gp, i, &out_chg))
  { if (__ev_tracing) trace_gunchg(gp, i); return; }

 if (__ev_tracing) evtr_prop_gatechg(gp, i, out_chg);
 else
  {
   if (gp->g_delrep == DT_NONE) { if (out_chg) change_gate_outwire(gp); }
   else prop_gatechg(gp, out_chg, FALSE);
  }
}

/*
 * accelerated 2 input gate (all XL style simple expressions)
 *
 * if no delay and not ev trace does all inline, if delay call normal prop
 * both ports must be constant bit select or scalar
 * inputs can not be strength for this most optimized routine
 */
static void acc_chg_bufnot(struct gate_t *gp, word32 i)
{
 register word32 ouwrd, uwrd, ngav, ngbv;
 int32 out_chg, gatid, biti;
 word32 igav, igbv;
 struct expr_t *xp;
 struct net_t *np;

 xp = gp->gpins[1];
 if (xp->optyp == ID)
  ld_scalval_(&igav, &igbv, xp->lu.sy->el.enp->nva.bp);
 else
  {
   __ld_bit(&igav, &igbv, xp->lu.x->lu.sy->el.enp,
    (int32) __contab[xp->ru.x->ru.xvi]);
  }

 __new_inputval = igav | (igbv << 1);
 /* eval changed input and store in gstate if needed */
 /* 12/19/99 SJM - notice buf or not still packed into 1 byte but vars */
 /* only packed into word32 as smallest */
 ouwrd = (word32) gp->gstate.bp[__inum];

 /* input for not is bits 0 and 2 */
 uwrd = ouwrd & ~(0x5L);
 uwrd |= (igav | (igbv << 2));
 /* input change did not change gate */
 if (uwrd == ouwrd) { if (__ev_tracing) trace_gunchg(gp, i); return; }
 gp->gstate.bp[__inum] = (byte) uwrd;

 /* value for not is bits 1 and 3 */
 __old_gateval = ((uwrd >> 1) & 1L) | ((uwrd >> 2) & 2L);

 /* evaluate - not and buf always convert z to x */
 gatid = gp->gmsym->el.eprimp->gateid;
 ngbv = (uwrd >> 2) & 1L;
 if (gatid == G_NOT) ngav = !(uwrd & 1L) | ngbv;
 else if (gatid == G_BUF) ngav = (uwrd & 1L) | ngbv;
 /* but cont. ASSIGN passes z */
 else ngav = (uwrd & 1L);

 __new_gateval = ngav | (ngbv << 1);
 /* set to T (non 0) if not equal if changed (different) */
 out_chg = (__old_gateval != __new_gateval);
 /* if tracing must use std trace store-propagate routine */
 if (__ev_tracing) { evtr_prop_gatechg(gp, i, out_chg); return; }
 /* handle delay case using normal gate chg */
 /* third param means acc possible because called from acc routine */
 if (gp->g_delrep != DT_NONE) { prop_gatechg(gp, out_chg, TRUE); return; }

 /* immediate fast assign for accelerated */
 /* inline steps in store gate output value */
 if (!out_chg) return;

 /* g pdst on if wire driven by gate is path dest. or has delay */
 if (gp->g_pdst) { change_gate_outwire(gp); return; }

 /* non delay acc immediate assign code */
 xp = gp->gpins[0];
 /* update state with computed output value is bits 1 and 3 */
 uwrd = uwrd & ~(0x2L) & ~(0x8L);
 uwrd |= ((ngav << 1) | (ngbv << 3));
 gp->gstate.bp[__inum] = (byte) uwrd;

 /* accelerated assign to pin 0 (output) */
 if (xp->optyp == ID)
  {
   np = xp->lu.sy->el.enp;
   if (np->frc_assgn_allocated)
    {
     igav = ngav; igbv = ngbv;
     if (!__correct_forced_newwireval(np, &igav, &igbv))
      goto try_trace;
    }
   /* here since avoiding value store, need to add net change el. */
   chg_st_scalval_(np->nva.bp, ngav, ngbv);
   /* not 0 for mask ands is T */
   /* if lhs chged and no lds/dces and not entire inst changed, record it */
   if (__lhs_changed) record_nchg_(np);
  }
 else
  {
   np = xp->lu.x->lu.sy->el.enp;
   biti = (int32) __contab[xp->ru.x->ru.xvi];

   /* if the 1 bit is forced nothing to do else normal assign */
   if (np->frc_assgn_allocated
    && __forced_inhibit_bitassign(np, xp->lu.x, xp->ru.x)) goto try_trace;
   /* notice this adds the net chg element if needed */
   __chg_st_bit(np, biti, ngav, ngbv);
  }
try_trace:
 if (__ev_tracing) trace_chg_gateout(gp, xp);
}

/*
 * accelerated 1 input (maybe stren) gate (all XL style simple expressions)
 *
 * if no delay and not ev trace does all inline, if delay call normal prop
 * both ports must be constant bit select or scalar
 * here inputs may be strength (removed), but cannot drive stren
 *
 * only difference is slow if for accessing value from strength
 */
static void acc_stichg_bufnot(register struct gate_t *gp, word32 i)
{
 register struct expr_t *xp;
 register word32 ouwrd, uwrd, ngav, ngbv;
 register struct net_t *np;
 int32 out_chg, gatid, biti;
 word32 igav, igbv;

 xp = gp->gpins[1];
 if (xp->optyp == ID)
  {
   np = xp->lu.sy->el.enp;
   if (np->n_stren)
    {
     uwrd = (word32) np->nva.bp[__inum];
     igav = uwrd & 1L;
     igbv = (uwrd >> 1) & 1L;
    }
   else ld_scalval_(&igav, &igbv, np->nva.bp);
  }
 else __ld_bit(&igav, &igbv, xp->lu.x->lu.sy->el.enp,
  (int32) __contab[xp->ru.x->ru.xvi]);

 __new_inputval = igav | (igbv << 1);
 /* eval changed input and store in gstate if needed */
 ouwrd = (word32) gp->gstate.bp[__inum];
 /* input for not is bits 0 and 2 */
 uwrd = ouwrd & ~(0x5L);
 uwrd |= (igav | (igbv << 2));
 /* input change did not change gate */
 if (uwrd == ouwrd) { if (__ev_tracing) trace_gunchg(gp, i); return; }
 gp->gstate.bp[__inum] = (byte) uwrd;

 /* value for not is bits 1 and 3 */
 __old_gateval = ((uwrd >> 1) & 1L) | ((uwrd >> 2) & 2L);

 /* evaluate - not and buf always convert z to x */
 gatid = gp->gmsym->el.eprimp->gateid;
 ngbv = (uwrd >> 2) & 1L;
 if (gatid == G_NOT) ngav = !(uwrd & 1L) | ngbv;
 else if (gatid == G_BUF) ngav = (uwrd & 1L) | ngbv;
 /* but cont. ASSIGN passes z */
 else ngav = (uwrd & 1L);

 __new_gateval = ngav | (ngbv << 1);
 /* set to T (non 0) if not equal if changed (different) */
 out_chg = (__old_gateval != __new_gateval);
 /* if tracing must use std trace store-propagate routine */
 if (__ev_tracing) { evtr_prop_gatechg(gp, i, out_chg); return; }
 /* handle delay case using normal gate chg */
 /* thrd param T because being called from acc routine */
 if (gp->g_delrep != DT_NONE) { prop_gatechg(gp, out_chg, TRUE); return; }

 /* immediate fast assign for accelerated */
 /* inline steps in store gate output value */
 if (!out_chg) return;

 /* g pdst on if wire driven by gate is path dest. or has delay */
 if (gp->g_pdst) { change_gate_outwire(gp); return; }

 /* non delay acc assign code */
 xp = gp->gpins[0];
 /* update state with computed output value is bits 1 and 3 */
 uwrd = uwrd & ~(0x2L) & ~(0x8L);
 uwrd |= ((ngav << 1) | (ngbv << 3));
 gp->gstate.bp[__inum] = (byte) uwrd;
 /* accelerated assign to pin 0 (output) */
 if (xp->optyp == ID)
  {
   np = xp->lu.sy->el.enp;
   if (np->frc_assgn_allocated)
    {
     igav = ngav; igbv = ngbv;
     if (!__correct_forced_newwireval(np, &igav, &igbv))
      goto try_trace;
    }
   /* here since avoiding value store, need to add net change el. */
   chg_st_scalval_(np->nva.bp, ngav, ngbv);

   /* not 0 for mask ands is T */
   /* if lhs chged and no lds/dces and not entire inst changed, record it */
   if (__lhs_changed) record_nchg_(np);
  }
 else
  {
   np = xp->lu.x->lu.sy->el.enp;
   biti = (int32) __contab[xp->ru.x->ru.xvi];
   /* if the 1 bit is forced nothing to do else normal assign */
   if (np->frc_assgn_allocated
    && __forced_inhibit_bitassign(np, xp->lu.x, xp->ru.x)) goto try_trace;
   /* notice this adds the net chg element if needed */
   __chg_st_bit(np, biti, ngav, ngbv);
  }
try_trace:
 if (__ev_tracing) trace_chg_gateout(gp, xp);
}

/*
 * acceleated up to 3 input (4 pints) gate with no delay
 *
 * could unwind to separate for each gate type
 * if no delay and not ev trace does all inline, if delay call normal prop
 * both ports must be constant bit select or scalar
 * inputs must not be stren wires
 * degenerate 2 input gate not accelerated
 */
static void acc_chg_4igate(register struct gate_t *gp, word32 i)
{
 register struct expr_t *xp;
 register word32 ouwrd, uwrd, ngav, ngbv, gwid;
 struct net_t *np;
 int32 out_chg, biti, bi;
 word32 gav, gbv, mask;

 xp = gp->gpins[i];
 if (xp->optyp == ID) ld_scalval_(&gav, &gbv, xp->lu.sy->el.enp->nva.bp);
 else __ld_bit(&gav, &gbv, xp->lu.x->lu.sy->el.enp,
  (int32) __contab[xp->ru.x->ru.xvi]);
 bi = i - 1;
 gwid = gp->gpnum;
 __new_inputval = gav | (gbv << 1);

 /* eval changed input and store in gstate if needed */
 ouwrd = (word32) gp->gstate.bp[__inum];
 uwrd = ouwrd & ~(1L << bi) & ~(1L << (gwid + bi));
 uwrd |= ((gav << bi) | (gbv << (gwid + bi)));
 /* input change did not change gate */
 if (uwrd == ouwrd) { if (__ev_tracing) trace_gunchg(gp, i); return; }
 gp->gstate.bp[__inum] = (byte) uwrd;

 /* mask off a/b output bit - now gav/gbv all inputs */
 mask = __masktab[gwid - 1];
 gav = uwrd & mask;
 gbv = (uwrd >> gwid) & mask;
 /* works since know n ins at least 1 - b shifts 1 less, goes b bit */
 __old_gateval = ((uwrd >> (gwid - 1)) & 1L) | ((uwrd >> (2*gwid - 2)) & 2L);

 /* evaluate gate */
 /* LOOKATME - could split and copy for each pin/gate combination */
 ngav = ngbv = 1L;
 switch ((byte) gp->gmsym->el.eprimp->gateid) {
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
 __new_gateval = ngav | (ngbv << 1);
 /* set to T (non 0) if not equal if changed (different) */
 out_chg = (__old_gateval != __new_gateval);
 /* if tracing must use std trace store-propagate routine */
 if (__ev_tracing) { evtr_prop_gatechg(gp, i, out_chg); return; }
 /* handle delay case using normal gate chg */
 /* thrd param T because being called from acc routine */
 if (gp->g_delrep != DT_NONE) { prop_gatechg(gp, out_chg, TRUE); return; }

 /* immediate fast assign for accelerated */
 /* inline steps in store gate output value */
 if (!out_chg) return;

 /* g pdst on if wire driven by gate is path dest. or has delay */
 if (gp->g_pdst) { change_gate_outwire(gp); return; }

 /* non delay acc immediate assign code */
 xp = gp->gpins[0];
 /* mask off separated value bits to update output value in uwrd */
 uwrd = uwrd & ~(1L << (gwid - 1)) & ~(1L << (2*gwid - 1));
 /* works because ngav and ngbv exactly 1 low bit */
 uwrd |= ((ngav << (gwid - 1)) | (ngbv << (2*gwid - 1)));
 /* must store twice because update of input may not change output */
 gp->gstate.bp[__inum] = (byte) uwrd;
 /* accelerated assign to pin 0 (output) */
 if (xp->optyp == ID)
  {
   np = xp->lu.sy->el.enp;
   if (np->frc_assgn_allocated)
    {
     gav = ngav; gbv = ngbv;
     if (!__correct_forced_newwireval(np, &gav, &gbv))
      goto try_trace;
    }
   /* here since avoiding value store, need to add net change el. */
   chg_st_scalval_(np->nva.bp, ngav, ngbv);
   if (__lhs_changed) record_nchg_(np);
  }
 else
  {
   np = xp->lu.x->lu.sy->el.enp;
   biti = (int32) __contab[xp->ru.x->ru.xvi];
   /* if the 1 bit is forced nothing to do else normal assign */
   if (np->frc_assgn_allocated
    && __forced_inhibit_bitassign(np, xp->lu.x, xp->ru.x)) goto try_trace;
   /* this adds the nchg el if needed */
   __chg_st_bit(np, biti, ngav, ngbv);
  }
try_trace:
 if (__ev_tracing) trace_chg_gateout(gp, xp);
}

/*
 * accelerated up to 4 input gate
 *
 * could unwind to separate for each gate type
 * if no delay and not ev trace does all inline, if delay call normal prop
 * both ports must be constant bit select or scalar
 *
 * inputs can be strength wires (removed) but cannot driver stren
 */
static void acc_stichg_4igate(register struct gate_t *gp, word32 i)
{
 register struct expr_t *xp;
 register word32 ouwrd, uwrd, ngav, ngbv, gwid;
 struct net_t *np;
 int32 out_chg, biti, bi;
 word32 gav, gbv, mask;

 xp = gp->gpins[i];
 if (xp->optyp == ID)
  {
   np = xp->lu.sy->el.enp;
   if (np->n_stren)
    {
     uwrd = (word32) np->nva.bp[__inum];
     gav = uwrd & 1L;
     gbv = (uwrd >> 1) & 1L;
    }
   else ld_scalval_(&gav, &gbv, np->nva.bp);
  }
 else __ld_bit(&gav, &gbv, xp->lu.x->lu.sy->el.enp,
   (int32) __contab[xp->ru.x->ru.xvi]);
 bi = i - 1;
 gwid = gp->gpnum;
 __new_inputval = gav | (gbv << 1);

 /* eval changed input and store in gstate if needed */
 ouwrd = (word32) gp->gstate.bp[__inum];
 uwrd = ouwrd & ~(1L << bi) & ~(1L << (gwid + bi));
 uwrd |= ((gav << bi) | (gbv << (gwid + bi)));
 /* input change did not change gate */
 if (uwrd == ouwrd) { if (__ev_tracing) trace_gunchg(gp, i); return; }
 gp->gstate.bp[__inum] = (byte) uwrd;

 /* mask off a/b output bit - now gav/gbv all inputs */
 mask = __masktab[gwid - 1];
 gav = uwrd & mask;
 gbv = (uwrd >> gwid) & mask;
 /* works since know n ins at least 1 - b shifts 1 less, goes b bit */
 __old_gateval = ((uwrd >> (gwid - 1)) & 1L) | ((uwrd >> (2*gwid - 2)) & 2L);

 /* evaluate gate */
 /* LOOKATME - could split and copy for each pin/gate combination */
 ngav = ngbv = 1L;
 switch ((byte) gp->gmsym->el.eprimp->gateid) {
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
 __new_gateval = ngav | (ngbv << 1);
 /* set to T (non 0) if not equal if changed (different) */
 out_chg = (__old_gateval != __new_gateval);
 /* if tracing must use std trace store-propagate routine */
 if (__ev_tracing) { evtr_prop_gatechg(gp, i, out_chg); return; }
 /* handle delay case using normal gate chg */
 /* thrd param T because being called from acc routine */
 if (gp->g_delrep != DT_NONE) { prop_gatechg(gp, out_chg, TRUE); return; }

 /* immediate fast assign for accelerated */
 /* inline steps in store gate output value */
 if (!out_chg) return;

 /* g pdst on if wire driven by gate is path dest. or has delay */
 if (gp->g_pdst) { change_gate_outwire(gp); return; }

 /* non delay acc immediate assign code */
 xp = gp->gpins[0];
 /* mask off separated value bits to update output value in uwrd */
 uwrd = uwrd & ~(1L << (gwid - 1)) & ~(1L << (2*gwid - 1));
 /* works because ngav and ngbv exactly 1 low bit */
 uwrd |= ((ngav << (gwid - 1)) | (ngbv << (2*gwid - 1)));
 /* must store twice because update of input may not change output */
 gp->gstate.bp[__inum] = (byte) uwrd;
 /* accelerated assign to pin 0 (output) */
 if (xp->optyp == ID)
  {
   np = xp->lu.sy->el.enp;
   if (np->frc_assgn_allocated)
    {
     gav = ngav; gbv = ngbv;
     if (!__correct_forced_newwireval(np, &gav, &gbv))
      goto try_trace;
    }
   /* here since avoiding value store, need to add net change el. */
   chg_st_scalval_(np->nva.bp, ngav, ngbv);
   if (__lhs_changed) record_nchg_(np);
  }
 else
  {
   np = xp->lu.x->lu.sy->el.enp;
   biti = (int32) __contab[xp->ru.x->ru.xvi];
   /* if the 1 bit is forced nothing to do else normal assign */
   if (np->frc_assgn_allocated
    && __forced_inhibit_bitassign(np, xp->lu.x, xp->ru.x)) goto try_trace;
   /* this adds the nchg el if needed */
   __chg_st_bit(np, biti, ngav, ngbv);
  }
try_trace:
 if (__ev_tracing) trace_chg_gateout(gp, xp);
}

/*
 * write gate value unchanged when input changes trace msg
 */
static void trace_gunchg(struct gate_t *gp, word32 i)
{
 char s1[RECLEN];

 __tr_msg("-- %s %s %s input %u value unchanged\n",
  gp->gmsym->synam, (gp->g_class == GC_UDP) ? "udp" : "gate",
  to_evtronam(s1, gp->gsym->synam, __inst_ptr, (struct task_t *) NULL), i);
}

/*
 * evaluate a udp - std not optimized version
 */
static void std_chg_udp_gate(register struct gate_t *gp, register word32 i)
{
 register int32 is_edge;
 int32 out_chg;

 __cur_udp = gp->gmsym->el.eudpp;
 is_edge = (__cur_udp->utyp == U_EDGE) ? TRUE : FALSE;
 if (!__eval_udp(gp, i, &out_chg, is_edge))
  { if (__ev_tracing) trace_gunchg(gp, i); return; }

 if (__ev_tracing) evtr_prop_gatechg(gp, i, out_chg);
 else
  {
   if (gp->g_delrep == DT_NONE)
    { if (out_chg) change_gate_outwire(gp); return; }
   prop_gatechg(gp, out_chg, FALSE);
  }
}

/*
 * evaluate a bufif gate - std not optimized version
 */
static void std_chg_bufif_gate(register struct gate_t *gp, register word32 i)
{
 int32 out_chg;

 /* this sets __new_gateval to strength if out changed T */
 if (!__eval_bufif_gate(gp, i, &out_chg))
  { if (__ev_tracing) trace_gunchg(gp, i); return; }

 if (__ev_tracing) evtr_prop_gatechg(gp, i, out_chg);
 else
  {
   if (gp->g_delrep == DT_NONE)
    { if (out_chg) change_gate_outwire(gp); return; }
   prop_gatechg(gp, out_chg, FALSE);
  }
}

/*
 * evaluate a mos gate - std not optimized version
 *
 * g resist here is for real resistive gate not flag for acc
 */
static void std_chg_mos_gate(register struct gate_t *gp, register word32 i)
{
 register int32 out_chg, gid;

 /* this sets __new_gateval to strength if out changed T */
 if (!chg_mos_instate(gp, i))
  { if (__ev_tracing) trace_gunchg(gp, i); return; }

 out_chg = TRUE;
 gid = gp->gmsym->el.eprimp->gateid;
 switch (gid) {
  case G_NMOS: __eval_nmos_gate(gp->gstate.wp[__inum]); break;
  case G_RNMOS: __eval_rnmos_gate(gp->gstate.wp[__inum]); break;
  case G_PMOS: __eval_pmos_gate(gp->gstate.wp[__inum]); break;
  case G_RPMOS: __eval_rpmos_gate(gp->gstate.wp[__inum]); break;
  default: __case_terr(__FILE__, __LINE__);
 }
 if (__new_gateval == __old_gateval) out_chg = FALSE;

 if (__ev_tracing) evtr_prop_gatechg(gp, i, out_chg);
 else
  {
   if (gp->g_delrep == DT_NONE)
    { if (out_chg) change_gate_outwire(gp); return; }
   prop_gatechg(gp, out_chg, FALSE);
  }
}

/*
 * evaluate a cmos gate - std not optimized version
 */
static void std_chg_cmos_gate(register struct gate_t *gp, register word32 i)
{
 register int32 out_chg;

 if (!chg_cmos_instate(gp, i))
  { if (__ev_tracing) trace_gunchg(gp, i); return; }

 /* this sets __new_gateval to strength if out changed T */
 __eval_cmos_gate(gp);
 if (__new_gateval == __old_gateval) out_chg = FALSE; else out_chg = TRUE;

 if (__ev_tracing) evtr_prop_gatechg(gp, i, out_chg);
 else
  {
   if (gp->g_delrep == DT_NONE)
    { if (out_chg) change_gate_outwire(gp); return; }
   prop_gatechg(gp, out_chg, FALSE);
  }
}

/*
 * routine used during prep to determine and set gate in change routine
 * called for all including udp but not trans
 */
extern void __set_gchg_func(struct gate_t *gp)
{
 int32 acc_class;

 switch ((byte) gp->g_class) {
  case GC_LOGIC:
   /* accelerate class is 2 for buf/not and 3 for any up to 4 logic gate */
   /* 0 for cannot accelerate */
   if (!__accelerate) gp->gchg_func = std_chg_logic_gate;
   else
    {
     acc_class = __get_acc_class(gp);
     switch ((byte) acc_class) {
      case ACC_STD: gp->gchg_func = std_chg_logic_gate; break;
      case ACC_BUFNOT: gp->gchg_func = acc_chg_bufnot; break;
      case ACC_STIBUFNOT: gp->gchg_func = acc_stichg_bufnot; break;
      case ACC_4IGATE: gp->gchg_func = acc_chg_4igate; break;
      case ACC_ST4IGATE: gp->gchg_func = acc_stichg_4igate; break;
      default: __case_terr(__FILE__, __LINE__);
     }
    }
   break;
  case GC_UDP: gp->gchg_func = std_chg_udp_gate; break;
  case GC_BUFIF: gp->gchg_func = std_chg_bufif_gate; break;
  case GC_MOS: gp->gchg_func = std_chg_mos_gate; break;
  case GC_CMOS: gp->gchg_func = std_chg_cmos_gate; break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * routine to turn off acceleration for logic gates when gate out terminal
 * value change call back registered
 */
extern void __logic_acc_off(struct gate_t *gp)
{
 if (gp->g_class == GC_LOGIC && gp->gchg_func != std_chg_logic_gate)
  gp->gchg_func = std_chg_logic_gate;
}

/*
 * for gate that because of added vpi driver needs to be chaned to fi>1
 * set the standard unoptimized gate assign routine
 */
extern void __vpi_set_chg_proc(struct gate_t *gp)
{
 /* if non logic gate, never optimized so can just use the std */
 if (gp->g_class == GC_LOGIC)
  {
   if (gp->gchg_func != std_chg_logic_gate)
    {
     gp->gchg_func = std_chg_logic_gate;
    }
  }
}

/*
 * return T if gate has accelerated action routine
 */
extern int32 __gate_is_acc(struct gate_t *gp)
{
 if (gp->gchg_func == acc_chg_bufnot || gp->gchg_func == acc_stichg_bufnot
  || gp->gchg_func == acc_chg_4igate || gp->gchg_func == acc_stichg_4igate)
  return(TRUE);
 return(FALSE);
}

/*
 * propagate the gate change - normal version called when event tracing off
 * complicated because of spike analysis
 * this works for strength
 * this assumed old and new gateval globals set before here
 *
 * if gate or udp drives highz[01] strength used to access delay but gate
 * must drive actual value which is converted when assigned to wire
 * outchg for highz[01] not changed since same hiz will be same 0 or 1
 *
 * show cancel e analysis uses gate output not possible hiz wire since 0/1
 * glitch will just map to hiz spikes
 *
 * only called if know has delay and know old gateval and new gateval
 * globals set
 */
static void prop_gatechg(register struct gate_t *gp, register int32 outchg,
 int32 is_acc)
{
 i_tev_ndx tevpi;
 word64 gdel, schtim;
 struct tev_t *tevp;

 /* no pending scheduled event */
 if ((tevpi = gp->schd_tevs[__inum]) == -1)
  {
   /* case 1a: output changed */
   if (outchg)
    {
     /* if 0 or 1 and hiz strength gate need to use to hiz delay */
     if (gp->g_hasst && (__new_gateval & 2) == 0
      && __hizstren_del_tab[gp->g_stval] == 1)
      __hizstrengate_getdel(&gdel, gp);
     else __get_del(&gdel, gp->g_du, gp->g_delrep);

     schtim = __simtime + gdel;
     schedule_1gev(gp, gdel, schtim, is_acc);
    }
   /* if output did not change, nothing to do */
   return;
  }

 /* need time of new value scheduled change for this analysis */
 if (gp->g_hasst && (__new_gateval & 2) == 0
  && __hizstren_del_tab[gp->g_stval] == 1) __hizstrengate_getdel(&gdel, gp);
 else __get_del(&gdel, gp->g_du, gp->g_delrep);

 schtim = __simtime + gdel;

 /* pending event */
 tevp = &(__tevtab[tevpi]);
 /* new and old same - scheduled different - real pulse/glitch */
 if (!outchg)
  {
   if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
    emit_pulsewarn(gp, tevp, &(tevp->etime), &schtim, "drives glitch");

   /* if spike, suppress future but schedule to x at currently scheduled */
   if (__show_cancel_e)
    {
     if (__showe_onevent)
      { tevp->outv = get_showcancele_val(gp); return; }

     /* immediate assign then cancel */
on_detect_show_x:
     __new_gateval = get_showcancele_val(gp);
     if (tevp->gev_acc) acc_evchg_gate_outwire(gp);
     else change_gate_outwire(gp);
     /* newly scheduled to same so no event */
     tevp->te_cancel = TRUE;
     __inertial_cancels++;
     gp->schd_tevs[__inum] = -1;
     return;
    }
   /* newly scheduled to same so no event */
   tevp->te_cancel = TRUE;
   __inertial_cancels++;
   gp->schd_tevs[__inum] = -1;
   return;
  }
 /* new schedule to same value case */
 /* know that delay same and later so just discard new event */
 /* done silently here - trace message only */
 if (tevp->outv == (byte) __new_gateval) return;

 /* normal inertial reschedule */
 if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
  emit_pulsewarn(gp, tevp, &(tevp->etime), &schtim, "unstable");

 if (__show_cancel_e)
  {
   if (__showe_onevent) { tevp->outv = get_showcancele_val(gp); return; }
   goto on_detect_show_x;
  }
 /* reschedule - handles cancel */
 tevpi = reschedule_1gev(tevpi, gdel, schtim, __new_gateval, is_acc);
}

/*
 * propagate the gate change
 * complicated because of show cancel e analysis
 *
 * notice new gateval and old gateval set and has strength value if present
 * see prop gatechg routines for more comments
 * unlike prop_gatechg, this must be called for DT_NONE assigns
 * all events scheduled from here must not set event accelerate bit
 *
 * SJM 11/27/00 - this always calls or schedules change gate outwire where
 * the gate terminal call back is checked so do not need separate code
 */
static void evtr_prop_gatechg(register struct gate_t *gp, register word32 i,
 int32 outchg)
{
 i_tev_ndx tevpi;
 word64 gdel, schtim;
 struct tev_t *tevp;
 char vs1[10], vs2[10], vs3[10];
 char s1[RECLEN], s2[RECLEN];

 __tr_msg("-- %s gate %s input %d changed to %s:\n",
  gp->gmsym->synam, to_evtronam(s1, gp->gsym->synam, __inst_ptr,
  (struct task_t *) NULL), i, __to_ginam(vs1, gp, __new_inputval, i));

 /* case 0: gate has no delay - not even #0 */
 if (gp->g_delrep == DT_NONE)
  {
   if (!outchg) { __tr_msg(" NODEL, NOCHG\n"); return; }

   /* this assigns or schedules the 1 bit net change */
   __tr_msg(" NODEL <OV=%s, NV=%s>\n",
    __to_gonam(vs1, gp, __old_gateval), __to_gonam(vs2, gp, __new_gateval));
   change_gate_outwire(gp);
   return;
  }

 /* need time of new value scheduled change for this analysis */
 if (gp->g_hasst && (__new_gateval & 2) == 0
  && __hizstren_del_tab[gp->g_stval] == 1) __hizstrengate_getdel(&gdel, gp);
 else __get_del(&gdel, gp->g_du, gp->g_delrep);
 schtim = __simtime + gdel;

 /* case 1: no pending scheduled event */
 if ((tevpi = gp->schd_tevs[__inum]) == -1)
  {
   /* output did not change */
   if (!outchg)
    {
     /* no net change and must cancel any already scheduled event */
     __tr_msg(" DEL, NOCHG <OV=%s>\n",
      __to_gonam(vs1, gp, __old_gateval));
     return;
    }
   __tr_msg(" DEL, SCHD AT %s <OV=%s, NSV=%s>\n",
    __to_timstr(s1, &schtim), __to_gonam(vs1, gp, __old_gateval),
    __to_gonam(vs2, gp, __new_gateval));
   /* schedule */
   schedule_1gev(gp, gdel, schtim, FALSE);
   return;
  }
 /* pending event */
 tevp = &(__tevtab[tevpi]);
 /* new and old same - scheduled different - real pluse/glitch */
 if (!outchg)
  {
   if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
    emit_pulsewarn(gp, tevp, &(tevp->etime), &schtim, "drives glitch");

   /* if spike on set to x at time of previous change not inertial */
   if (__show_cancel_e)
    {
     if (__showe_onevent) sprintf(s2, "%s (on event)", __to_timstr(__xs,
      &(tevp->etime)));
     else sprintf(s2, "%s (on detect)", __to_timstr(__xs, &__simtime));
     __tr_msg(
      " DEL, PEND AT %s PULSE <OV=NSV=%s, OSV=%s SHOWING X AT %s MAYBE SWITCHED>\n",
      __to_timstr(s1, &(tevp->etime)), __to_gonam(vs1, gp, __old_gateval),
      __to_gonam(vs2, gp, tevp->outv), s2);

     if (__showe_onevent)
      { tevp->outv = get_showcancele_val(gp); return; }

     /* immediate assign then cancel */
on_detect_show_x:
     __new_gateval = get_showcancele_val(gp);
     if (tevp->gev_acc) acc_evchg_gate_outwire(gp);
     else change_gate_outwire(gp);
     /* newly scheduled to same so no event */
     tevp->te_cancel = TRUE;
     __inertial_cancels++;
     gp->schd_tevs[__inum] = -1;
     return;
    }
   /* newly scheduled to same (pulse) so no event */
   tevp->te_cancel = TRUE;
   __inertial_cancels++;
   gp->schd_tevs[__inum] = -1;
   /* SJM 01/21/02 - msg unclear since new sched value was missing */
   __tr_msg(" DEL, PEND, PULSE, INERTIAL CANCEL AT %s <OV=%s, OSV=%s NSV=%s>\n",
    __to_timstr(s1, &(tevp->etime)), __to_gonam(vs1, gp, __old_gateval),
    __to_gonam(vs2, gp, tevp->outv), __to_gonam(vs3, gp, __new_gateval));
   return;
  }

 /* new schedule to same value case */
 /* know that delay same and later so just discard new event */
 /* done silently here - trace message only */
 if (tevp->outv == (byte) __new_gateval)
  {
   __tr_msg(
    " DEL, MODEL ANOMALLY IGNORE SCHED TO SAME <OSV=NSV=%s> OLD AT %s NEW %s\n",
    __to_gonam(vs1, gp, __new_gateval), __to_timstr(s1, &(tevp->etime)),
    __to_timstr(s2, &schtim));
   return;
  }

 /* normal inertial reschedule */
 if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
  emit_pulsewarn(gp, tevp, &(tevp->etime), &schtim, "unstable");

 if (__show_cancel_e)
  {
   if (__showe_onevent) __to_timstr(s2, &schtim);
   else __to_timstr(s2, &__simtime);
   if (__showe_onevent) sprintf(s1, "%s (on event)", __to_timstr(__xs,
    &(tevp->etime)));
   else sprintf(s1, "%s (on detect)", __to_timstr(s1, &__simtime));
   __tr_msg(
    " DEL, PEND AT %s, UNSTABLE <OV=%s, OSV=%s, NSV=%s SHOWING X AT %s MAYBE SWITCHED>\n",
    __to_timstr(s1, &(tevp->etime)), __to_gonam(vs1, gp, __old_gateval),
    __to_gonam(vs2, gp, tevp->outv), __to_gonam(vs3, gp, __new_gateval), s2);

   if (__showe_onevent) { tevp->outv = get_showcancele_val(gp); return; }
   goto on_detect_show_x;
  }

 __tr_msg(" DEL, PEND, UNSTABLE RESCHD <OV=%s, OSV=%s AT %s, NSV=%s AT %s>\n",
  __to_gonam(vs1, gp, __old_gateval),
  __to_gonam(vs2, gp, tevp->outv), __to_timstr(s1, &(tevp->etime)),
  __to_gonam(vs3, gp, __new_gateval), __to_timstr(s2, &schtim));
 tevpi = reschedule_1gev(tevpi, gdel, schtim, __new_gateval, FALSE);
}

/*
 * compute show cancel x value depending on gate class and strength
 *
 * idea is to change event change to value but time still start of
 * region after place where gate may or may not have switched
 */
static word32 get_showcancele_val(struct gate_t *gp)
{
 int32 nd_stren = FALSE;

 switch ((byte) gp->g_class) {
  /* these never have strength value */
  case GC_LOGIC: case GC_UDP:
   if (gp->g_hasst) nd_stren = TRUE;
   break;
  case GC_BUFIF: nd_stren = TRUE; break;
  case GC_MOS: case GC_CMOS:
   /* LOOKATME - since mos gates pass strength for now driving strong x */
   /* maybe could take strength from input? and leave z */
   return(ST_STRONGX);
  /* for tranif input spike sched. only, x is turned off (0) */
  case GC_TRANIF: return(0);
  default: __case_terr(__FILE__, __LINE__);
 }
 /* notice no need to correct for possible highz strength since value x */
 /* SJM 08/07/01 - this works because with val x need both 0 and 1 strens */
 if (nd_stren) return((gp->g_stval << 2) | 3);
 return(3);
}

/*
 * emit an object name for tracing with path
 * cannot use __xs in here
 */
static char *to_evtronam(char *s, char *onam, struct itree_t *teitp,
 struct task_t *tskp)
{
 char s1[RECLEN], s2[RECLEN];

 sprintf(s, "%s.%s", __msg_blditree(s2, teitp, tskp), __schop(s1, onam));
 return(s);
}

/*
 * schedule 1 gate event
 * expects __new_gateval to contain value to schedule to
 */
static i_tev_ndx schedule_1gev(register struct gate_t *gp, word64 gdel,
 word64 schtim, int32 is_acc)
{
 register i_tev_ndx tevpi;
 register struct tev_t *tevp;

 alloc_tev_(tevpi, TE_G, __inst_ptr, schtim);
 if (gdel == 0ULL)
  {
   /* this is #0, but must still build tev */
   if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevpi;
   else { __tevtab[__p0_te_endi].tenxti = tevpi; __p0_te_endi = tevpi; }
  }
 else __insert_event(tevpi);

 gp->schd_tevs[__inum] = tevpi;
 tevp = &(__tevtab[tevpi]);
 tevp->tu.tegp = gp;
 /* if logic or udp, no strength, event processing assign will handle */
 /* if bufif, mos or cmos, know has strength, if tranif conducting state */
 tevp->outv = (byte) __new_gateval;
 /* acc. store only if gate is acc. and no conn. wire delay (resist off) */
 /* is acc T only if called from acc routine because acc routine never */
 /* possible for real resistive mos or tran */
 /* for mos with delay, can get here with g pdst on but is acc off */
 tevp->gev_acc = (is_acc && !gp->g_pdst);
 return(tevpi);
}

/*
 * take event and new value and either update if time same or cancel and
 * create new event if later
 */
static i_tev_ndx reschedule_1gev(i_tev_ndx tevpi, word64 gdel, word64 newtim,
 word32 newoutv, int32 is_acc)
{
 struct tev_t *tevp;

 tevp = &(__tevtab[tevpi]);
 /* if del == 0 (pnd0), will always be same time reschedule */
 if (gdel == 0ULL)
  {
   /* new value replaces old - must also be in event */
   __newval_rescheds++;
   tevp->outv = (byte) newoutv;
   return(tevpi);
  }

 /* case 3c-3 - more in future, cancel and reschedule */
 tevp->te_cancel = TRUE;
 __inertial_cancels++;
 tevpi = schedule_1gev(tevp->tu.tegp, gdel, newtim, is_acc);
 tevp->outv = (byte) newoutv;
 return(tevpi);
}

/*
 * emit the pulse (inertial reschedule) warning if not turned off
 * types are drives for normal spike and unstable for change that does not
 * do anything but interfere with transition
 */
static void emit_pulsewarn(struct gate_t *gp, struct tev_t *tevp,
 word64 *etim, word64 *newetim, char *sptnam)
{
 char s1[RECLEN], s2[RECLEN], s3[10], s4[10], s5[10];

 /* must turn on spike analysis */
 if (__show_cancel_e)
  {
   if (__showe_onevent) strcpy(s1, " - edge event to x");
   else strcpy(s1, " - now detect to x");
  }
 else strcpy(s1, "");

 sprintf(s2, "old %s, scheduled %s, new %s%s",
  __to_gonam(s3, gp, __old_gateval), __to_gonam(s4, gp, tevp->outv),
  __to_gonam(s5, gp, __new_gateval), s1);
 /* notice spike means new and old the same */
 __gfwarn(592, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
  "%s gate %s.%s %s (edge at %s replaced by new at %s) - %s",
  gp->gmsym->synam, __msg2_blditree(s1, tevp->teitp), gp->gsym->synam,
   sptnam, __to_timstr(__xs, etim), __to_timstr(__xs2, newetim), s2);
}

word32 __pow3tab[] = { 1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049 };

/*
 * process a gate change event - new level has reached output
 * know thing connected to gate output is 1 bit (bit select if needed)
 * but maybe has strength
 */
static void process_gatechg_ev(register struct tev_t *tevp)
{
 register struct gate_t *gp;

 /* notice event here emitted in change gate outwire */
 gp = tevp->tu.tegp;
 __new_gateval = tevp->outv;
 if (__ev_tracing) emit_gev_trace(gp, tevp);
 gp->schd_tevs[__inum] = -1;
 /* this handle added the 1 bit net change */
 if (tevp->gev_acc) acc_evchg_gate_outwire(gp);
 else change_gate_outwire(gp);
}

/*
 * accelerated routine for assigning a xl type simple gate event
 *
 * if assigned to net fi>1 (maybe from run time vpi_put_value) or has
 * delay can not use accelerated event processing
 * this requires new value in new gateval global
 */
static void acc_evchg_gate_outwire(register struct gate_t *gp)
{
 register struct expr_t *xp;
 register word32 uwrd, ngav, ngbv;
 register struct net_t *lhsnp;
 int32 biti, gwid;
 word32 igav, igbv;

 __immed_assigns++;
 ngav = __new_gateval & 1L;
 ngbv = __new_gateval >> 1;
 xp = gp->gpins[0];
 uwrd = (word32) gp->gstate.bp[__inum];
 if ((gwid = gp->gpnum) < 3)
  {
   /* update state with computed output value is bits 1 and 3 */
   uwrd = uwrd & ~(0xaL);
   uwrd |= ((ngav << 1) | (ngbv << 3));
  }
 else
  {
   /* mask off separated value bits to update output value in uwrd */
   uwrd = uwrd & ~(1L << (gwid - 1)) & ~(1L << (2*gwid - 1));
   /* works because ngav and ngbv exactly 1 low bit */
   uwrd |= ((ngav << (gwid - 1)) | (ngbv << (2*gwid - 1)));
  }
 gp->gstate.bp[__inum] = (byte) uwrd;

 /* accelerated assign to pin 0 (output) */
 if (xp->optyp == ID)
  {
   lhsnp = xp->lu.sy->el.enp;
   if (lhsnp->frc_assgn_allocated)
    {
     igav = ngav; igbv = ngbv;
     if (!__correct_forced_newwireval(lhsnp, &igav, &igbv))
      goto try_trace;
    }
   /* here since avoiding value store, need to add net change el. */
   chg_st_scalval_(lhsnp->nva.bp, ngav, ngbv);
   if (__lhs_changed) record_nchg_(lhsnp);
  }
 else
  {
   lhsnp = xp->lu.x->lu.sy->el.enp;
   biti = (int32) __contab[xp->ru.x->ru.xvi];
   /* if the 1 bit is forced nothing to do else normal assign */
   if (lhsnp->frc_assgn_allocated
    && __forced_inhibit_bitassign(lhsnp, xp->lu.x, xp->ru.x)) goto try_trace;
   /* notice this adds the net chg element if needed */
   __chg_st_bit(lhsnp, biti, ngav, ngbv);
  }
try_trace:
 if (__ev_tracing) trace_chg_gateout(gp, xp);
}

/*
 * emit gate event process trace message
 */
static void emit_gev_trace(struct gate_t *gp, struct tev_t *tevp)
{
 char s1[RECLEN], s2[RECLEN], vs1[10];

 __evtr_resume_msg();
 if (gp->gpins[0]->x_multfi) strcpy(s2, "this driver of multiple:");
 else strcpy(s2, "the fi=1 driver:");
 __tr_msg("-- %s gate %s processing store event to output, %s %s\n",
  gp->gmsym->synam, to_evtronam(s1, gp->gsym->synam, tevp->teitp,
  (struct task_t *) NULL), s2, __to_gonam(vs1, gp, __new_gateval));
}

/*
 * add a net change record when entire net changes(usually scalar)
 *
 * add to end of next pass, netchg list elements
 * notice this needs itstk of target wire for xmr
 */
extern void __add_nchglst_el(register struct net_t *np)
{
 register struct nchglst_t *nchglp;

 if (__nchgfreelst == NULL)
  nchglp = (struct nchglst_t *) __my_malloc(sizeof(struct nchglst_t));
 else
  {
   nchglp = __nchgfreelst;
   __nchgfreelst = __nchgfreelst->nchglnxt;

   /* DBG LINUX ADDME ??? */
   /* chk_nchgnlst(__nchgfreelst); */
   /* --- */
  }

 /* only turn on bit if all changed, each subrange goes on by itself */
 nchglp->chgnp = np;
 nchglp->nchg_itp = __inst_ptr;
 nchglp->bi1 = -1;
 nchglp->delayed_mipd = FALSE;

 /* all needed change info for this time slot now records */
 np->nchgaction[__inum] |= NCHG_ALL_CHGED;

 nchglp->nchglnxt = NULL;
 /* LOOKATME - maybe add dummy list element on front to avoid comparison */
 if (__nchg_futend != NULL)
  { __nchg_futend->nchglnxt = nchglp; __nchg_futend = nchglp; }
 else __nchg_futhdr = __nchg_futend = nchglp;

 /* DBG remove --- */
 if (__debug_flg && __ev_tracing)
  {
   __tr_msg("-- added net change element %s\n",
    __to_evtrwnam(__xs, np, -1, -1, __inst_ptr));
  }
 /* --- */
}

/*
 * add a net change record select range (usually bit) changed
 *
 * add to end of next pass, netchg list elements
 * notice this needs inst. loc of target wire for xmr
 */
extern void __add_select_nchglst_el(register struct net_t *np, register int32 i1,
 register int32 i2)
{
 register struct nchglst_t *nchglp;

 if (__nchgfreelst == NULL)
  nchglp = (struct nchglst_t *) __my_malloc(sizeof(struct nchglst_t));
 else
  {
   nchglp = __nchgfreelst;
   __nchgfreelst = __nchgfreelst->nchglnxt;
   /* DBG LINUX ADDME */
   /* chk_nchgnlst(__nchgfreelst); */
   /* --- */
  }

 /* only turn on bit if all changed, each subrange goes on by itself */
 nchglp->chgnp = np;
 nchglp->nchg_itp = __inst_ptr;
 nchglp->bi1 = i1;
 nchglp->bi2 = i2;
 /* AIV 04/30/07 - was not init the delay_mipd flag */
 nchglp->delayed_mipd = FALSE;

 /* here since range not marked as all changed so will match ranges */

 /* link on end since good heuristic to process in change order */
 nchglp->nchglnxt = NULL;
 /* LOOKATME - maybe add dummy list element on front to avoid comparison */
 if (__nchg_futend != NULL)
  { __nchg_futend->nchglnxt = nchglp; __nchg_futend = nchglp; }
 else __nchg_futhdr = __nchg_futend = nchglp;

 /* DBG remove ---
 if (__debug_flg && __ev_tracing)
  {
   __tr_msg("-- added net change element %s\n",
    __to_evtrwnam(__xs, np, i1, i2, __inst_ptr));
  }
 --- */
}

/*
 * add a dumpvars change element
 * only called first time for entire net in time slot
 */
extern void __add_dmpv_chglst_el(struct net_t *np)
{
 register struct dvchgnets_t *dvchgp;

 if (__dv_netfreelst == NULL)
   dvchgp = (struct dvchgnets_t *) __my_malloc(sizeof(struct dvchgnets_t));
 else
  {
   dvchgp = __dv_netfreelst;
   __dv_netfreelst = __dv_netfreelst->dvchgnxt;
  }
 /* indicate for this time slot inst of var already changed once */
 np->nchgaction[__inum] &= ~(NCHG_DMPVNOTCHGED);
 /* set the net and link the change on the front */
 dvchgp->dvchg_np = np;
 dvchgp->dvchg_itp = __inst_ptr;

 /* link on front since order does not matter */
 dvchgp->dvchgnxt = __dv_chgnethdr;
 __dv_chgnethdr = dvchgp;
 /* must indicate need for end of slot dv processing */
 __slotend_action |= SE_DUMPVARS;
}

/*
 * assign gate output to driven wire
 * this assumes __new_gateval previously set
 * works for both 8 bit strength and 2 bit non strength values
 *
 * notice simultaneously when changing wire (or scheduling if wire has delay)
 * must store gate state output - needed since when evaluating driver for
 * gate with delay need previous until actual store
 */
static void change_gate_outwire(register struct gate_t *gp)
{
 register word32 *wp;
 register struct expr_t *xp;
 register int32 bi, wi;
 int32 schd_wire, nins, srep;
 hword *hwp;
 word32 av, bv;
 byte sb2, *sbp;
 word32 uwrd;
 struct xstk_t *xsp;

 __immed_assigns++;
 xp = gp->gpins[0];
 switch ((byte) gp->g_class) {
  case GC_LOGIC:
   /* SJM 02/07/01 - remove st gstate out routine since only for logic */
   /* this removes inner loop case stmt */

   /* must store new gate value into state here - value does not have stren */
   nins = gp->gpnum - 1;
   if (nins > 15) srep = SR_VEC; else srep = SR_PVEC;
   /* FIXME - why are these not wrd? */
   av = ((word32) __new_gateval) & 1;
   bv = ((word32) __new_gateval) >> 1;
   gate_st_bit(gp->gstate, nins + 1, nins, srep, av, bv);
   break;
  case GC_UDP:
   /* new gateval for udp does not have strength - maybe added in store */
   /* tricky part for wide udp's - must update running signature if present */
   __cur_udp = gp->gmsym->el.eudpp;
   nins = __cur_udp->numins;
   bi = 2*nins;
   /* comb means no state - edge always has state */
   if (__cur_udp->u_wide)
    {
     wp = &(gp->gstate.wp[2*__inum]);
     /* update running 2nd state signature word32 */
     if (__cur_udp->utyp != U_COMB)
      {
       /* need to access old gate value (out about to change) */
       /* since for event schedule will not be stored */
       /* during initialize this will be meaningless 0 */
       __old_gateval = (wp[0] >> (2*nins)) & 3L;

       /* correct running index of output since part of state if not comb. */
       /* num ins is index of state */
       /* subtract off old contribution of state output */
       wp[1] -= ((__old_gateval == 3) ? 2 : __old_gateval)*__pow3tab[nins];
       /* add in new contribution of state output */
       wp[1] += ((__new_gateval == 3) ? 2 : __new_gateval)*__pow3tab[nins];
      }
     /* update first value word32 */
     wp[0] &= ~(3L << bi);
     wp[0] |= (__new_gateval << bi);
    }
   /* do the the gate state output store */
   /* udp state stored as nins 2 bit vals + out/state 2 bits */
   else
    {
     hwp = &(gp->gstate.hwp[__inum]);
     /* -- RELASE remove
     if (__debug_flg && __ev_tracing)
      __tr_msg("-- st udp out old %x\n", *hwp);
     -- */
     *hwp &= ~(3 << bi);
     *hwp |= (hword) (__new_gateval << bi);
     /* -- RELEASE remove ---
     if (__debug_flg && __ev_tracing)
      __tr_msg(" new %x\n", *hwp);
     -- */
    }
   break;
  case GC_BUFIF:
   /* store new gate val into bufif state - here must merge in stren */
   hwp = &(gp->gstate.hwp[__inum]);
   hwp[0] &= ~(0xff << 4);
   hwp[0] |= ((hword) (__new_gateval << 4));
   goto do_hasstren_assign;
  case GC_MOS:
   /* store new gate val into mos state */
   wp = &(gp->gstate.wp[__inum]);
   wp[0] &= ~(0xff << 16);
   wp[0] |= (__new_gateval << 16);
   goto do_hasstren_assign;
  case GC_CMOS:
   /* store new gate val into cmos state */
   wp = &(gp->gstate.wp[__inum]);
   wp[0] &= ~(0xff << 24);
   wp[0] |= (__new_gateval << 24);

do_hasstren_assign:
   /* then assign - here strength variable and new gateval has strength */
   if (xp->x_multfi) __mdr_assign_or_sched(xp);
   else
    {
     if (xp->lhsx_ndel && !__wire_init) schd_wire = TRUE;
     else schd_wire = FALSE;

     /* 07/08/00 - if gate (always 1 bit) drives wider vec must initialize */
     /* other bits to z since only has 1 driver */
     push_xstk_(xsp, 4*xp->szu.xclen);
     sbp = (byte *) xsp->ap;
     set_byteval_(sbp, xp->szu.xclen, ST_HIZ);
     /* set the low bit */
     sbp[0] = (byte) __new_gateval;
     __exec_conta_assign(xp, (word32 *) sbp, (word32 *) NULL, schd_wire);
     __pop_xstk();
    }
   goto done;
  case GC_TRANIF:
   /* out wire here is conducting state from 3rd input */
   /* SJM 12/13/00 - serious malloc bug was using bit ofset */
   wi = get_wofs_(2*__inum);
   bi = get_bofs_(2*__inum);
   /* 2 bits give conducting state */
   if (__new_gateval == 2) __new_gateval = 3;
   gp->gstate.wp[wi] &= ~(3L << bi);
   gp->gstate.wp[wi] |= (__new_gateval << bi);
   if (__ev_tracing)
    {
     __tr_msg("-- relaxing %s in switch channel\n",
      __gstate_tostr(__xs, gp, TRUE));
    }
   /* tranif enable changed must evaluate channel */
   /* LOOKATME think this only needs to be called if from/to 0, x same as 1 */
   /* SJM 04/11/00 - put back so immediately perturb both terminal vertices */
   __immed_eval_trifchan(gp);
   return;
  /* tran can never get here */
  default: __case_terr(__FILE__, __LINE__);
 }

 /* store gate or udp where value maybe needs constant strength added */
 /* gate state now updated, must assign to wire or schedule assign */
 if (xp->x_multfi) __mdr_assign_or_sched(xp);
 else
  {
   /* even though decl. no need for z extend since 1 bit max. wide */
   if (xp->lhsx_ndel && !__wire_init) schd_wire = TRUE;
   else schd_wire = FALSE;
   /* notice lhs cannot be concat here and know source and dest 1 bit */
   if (xp->x_stren)
    {
     /* here any strength constant */
     /* notice always need to add strength logic gate cannot drive z */
     if (__new_gateval == 2) sb2 = 2;
     else
      {
       /* notice tran that uses g st val for mark never goes here */
       uwrd = __new_gateval | (gp->g_stval << 2);
       uwrd = (word32) __stren_map_tab[uwrd];
       sb2 = (byte) uwrd;
      }
     /* 07/08/00 - if gate (always 1 bit) drives wider vec must initialize */
     /* other bits to z since only has 1 driver */
     push_xstk_(xsp, 4*xp->szu.xclen);
     sbp = (byte *) xsp->ap;
     set_byteval_(sbp, xp->szu.xclen, ST_HIZ);
     /* set the low bit */
     sbp[0] = sb2;
     __exec_conta_assign(xp, (word32 *) sbp, (word32 *) NULL, schd_wire);
     __pop_xstk();
    }
   else
    {
     av = __new_gateval & 1L;
     bv = __new_gateval >> 1;
     /* assign needed although only bit select or 1 bit wire, can be xmr */
     __exec_conta_assign(xp, &av, &bv, schd_wire);
    }
  }

done:
 /* SJM 11/27/00 - know out changed, state has been updated and strength */
 /* competition done to set new wire value - this call back monitors the */
 /* gate state so it does ont matter if after wire changed */
 if (__have_vpi_gateout_cbs)
  {
   int32 gi, tevpi;

   gi = gp - __inst_mod->mgates;
   if (__inst_mod->mgateout_cbs != NULL && __inst_mod->mgateout_cbs[gi] != NULL
    && (tevpi = __inst_mod->mgateout_cbs[gi][__inum]) != -1)
    {
     __exec_vpi_gateoutcbs(tevpi);
    }
  }

 if (__ev_tracing) trace_chg_gateout(gp, xp);
}

/*
 * trace message after change gate outwire
 */
static void trace_chg_gateout(struct gate_t *gp, struct expr_t *xp)
{
 char s1[RECLEN], s2[RECLEN], vs1[10], vs2[10];

 if (xp->lhsx_ndel && !__wire_init) strcpy(vs1, "schedule");
 else strcpy(vs1, "assign");
 if (gp->g_class == GC_UDP) strcpy(vs2, "udp"); else strcpy(vs2, "gate");
 __tr_msg("-- %s %s event output %s, %s to %s\n", vs1,
  __gstate_tostr(__xs, gp, TRUE), vs2, __to_gassign_str(s1, xp),
  __msgexpr_tostr(s2, xp));
}

/* SJM 02/07/01 - removed st gstate out routine - moved only used logic */

/*
 * store into coded wp of length blen at biti for current instance
 * that is stored according to srep format from low 2 bits of rgap
 * notice bits are separated into a and b parts in gate representation
 *
 * this differs from lhs bit select in accessing value for current instance
 * and adjusting place to select from according to storage representation
 * cannot be used to access array or strength value and called
 * with know good index (not -1)
 * this is for logic gates only
 */
static void gate_st_bit(union pck_u pckv, int32 blen, int32 biti, int32 srep,
 register word32 av, register word32 bv)
{
 register word32 uwrd, ouwrd;
 word32 *rap;
 int32 wlen;

 /* this is same as full value store - biti 0 or will not get here */
 switch ((byte) srep) {
  case SR_SCAL: gate_st_scalval(pckv.wp, av, bv); return;
  case SR_VEC:
   wlen = wlen_(blen);
   /* rap is base of vector for current inst */
   rap = &(pckv.wp[2*wlen*__inum]);
   __lhsbsel(rap, biti, av);
   __lhsbsel(&(rap[wlen]), biti, bv);
   return;
  case SR_PVEC:
   /* SJM 12/19/99 - notice gates still packed into bp, hwp, wp not just word32 */
   ouwrd = get_packintowrd_(pckv, __inum, blen);
   uwrd = ouwrd & ~(1L << biti) & ~(ouwrd & (1L << (blen + biti)));
   uwrd |= ((av & 1L) << biti) | ((bv & 1L) << (blen + biti));
   if (uwrd != ouwrd)
    {
     st_packintowrd_(pckv, __inum, uwrd, blen);
    }
   return;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * store a gate packed into 2 bits scalar
 * coded as 2 contiguous bits per instance
 * assuming shift by 0 legal and gets right answer in C
 */
static void gate_st_scalval(register word32 *wp, register word32 av,
 register word32 bv)
{
 register int32 bi;
 int32 dbi, dwi;

 bi = 2*__inum;
 dwi = get_wofs_(bi);
 dbi = get_bofs_(bi);
 wp[dwi] &= ~(3L << dbi);
 wp[dwi] |= ((av | (bv << 1)) << dbi);
}

/*
 * change input i part of gstate vector for mos style gate
 * strens passed thru so must load with stren even if driver no stren
 * returns false if new input value is same as old
 * this requires correct cur. itp
 */
static int32 chg_mos_instate(register struct gate_t *gp, word32 i)
{
 register word32 uwrd;
 register struct expr_t *ndp;
 register byte *sbp;
 struct xstk_t *xsp;

 uwrd = gp->gstate.wp[__inum];
 /* split because change scheduled or assigned if only strength changed */
 if (i == 1)
  {
   ndp = gp->gpins[1];
   /* if port a reg cannot have strength, this will add strong */
   xsp = __ndst_eval_xpr(ndp);
   sbp = (byte *) xsp->ap;
   __new_inputval = sbp[0];
   __pop_xstk();
   __old_inputval = uwrd & 0xffL;
   if (__new_inputval == __old_inputval) return(FALSE);
   uwrd &= ~0xffL;
   uwrd |= __new_inputval;
  }
 else
  {
   /* control input should not have strength but if does ignored */
   xsp = __eval_xpr(gp->gpins[2]);
   __new_inputval = (xsp->ap[0] & 1L) | ((xsp->bp[0] & 1L) << 1);
   __pop_xstk();
   __old_inputval = (uwrd >> 8) & 3L;
   if (__new_inputval == __old_inputval) return(FALSE);
   uwrd &= ~(3L << 8);
   uwrd |= (__new_inputval << 8);
  }
 gp->gstate.wp[__inum] = uwrd;
 return(TRUE);
}

/*
 * change input i part of gstate vector for cmos 2 ctrl input style gate
 * strens passed thru so must load with stren even if driver no stren
 * returns false if new input value is same as old
 * this requires correct cur. itp
 *
 * format is 3 8 bit values (0th input data, 1 nmos in, 2 pmos in, 3 output)
 * but only 2 bits of 1st and 2nd control inputs used
 * notice input starts at 1 because output is pos. 0
 */
static int32 chg_cmos_instate(register struct gate_t *gp, word32 i)
{
 register word32 uwrd;
 register byte *sbp;
 register struct expr_t *ndp;
 register struct xstk_t *xsp;

 uwrd = gp->gstate.wp[__inum];
 /* split because change scheduled or assigned if only strength changed */
 if (i == 1)
  {
   ndp = gp->gpins[1];
   xsp = __ndst_eval_xpr(ndp);
   sbp = (byte *) xsp->ap;
   __new_inputval = sbp[0];
   __pop_xstk();
   __old_inputval = uwrd & 0xffL;
   if (__new_inputval == __old_inputval) return(FALSE);
   uwrd &= ~0xffL;
   uwrd |= __new_inputval;
  }
 else
  {
   /* control inputs should not have strength but if does removed */
   xsp = __eval_xpr(gp->gpins[i]);
   __new_inputval = (xsp->ap[0] & 1L) | ((xsp->bp[0] & 1L) << 1);
   __pop_xstk();
   if (i == 2)
    {
     /* n ctrl bits 15-8 */
     __old_inputval = (uwrd >> 8) & 3L;
     if (__new_inputval == __old_inputval) return(FALSE);
     uwrd &= ~(3L << 8);
     uwrd |= (__new_inputval << 8);
    }
   else
    {
     /* p ctrl bits 23-16 */
     __old_inputval = (uwrd >> 16) & 3L;
     if (__new_inputval == __old_inputval) return(FALSE);
     uwrd &= ~(3L << 16);
     uwrd |= (__new_inputval << 16);
    }
  }
 gp->gstate.wp[__inum] = uwrd;
 return(TRUE);
}

/*
 * TRANIF EVALUATION ROUTINES
 */

/*
 * evaluate tranif when third enable port changes
 */
extern void __eval_tranif_ld(register struct gate_t *gp, register int32 i)
{
 int32 out_chg;

 /* DBG remove */
 if (i != 2) __arg_terr(__FILE__, __LINE__);
 /* --- */
 /* third in chged - schedule (if needed) conducting state chg */
 /* must always go through scheduling code for spike analysis */
 eval_tranif_onoff(gp);
 /* out_chg T if conducting state changed */
 out_chg = (__new_gateval != __old_gateval);
 /* must evaluate both sides */
 if (gp->g_delrep == DT_NONE)
  { if (out_chg) change_gate_outwire(gp); return; }
 prop_gatechg(gp, out_chg, FALSE);
}

/*
 * handle on (conducting) state checking and change for input state
 *
 * here for delay case gate value is pending conducting state (1 on, 0 off)
 * actual current conducting state is same as stored output wire value
 *
 * stored conducting state corrected for tranif1 and tranif0 (value
 * computed then reversed depnding on if1 or if0)
 */
static void eval_tranif_onoff(struct gate_t *gp)
{
 register int32 wi, bi;
 register word32 cval;
 register struct xstk_t *xsp;
 int32 gateid;

 wi = get_wofs_(2*__inum);
 bi = get_bofs_(2*__inum);
 /* step 1: access old value */
 cval = gp->gstate.wp[wi];
 /* this is conducting state independent of if0 or if1 */
 __old_gateval = (cval >> bi) & 3L;
 /* step 2: compute new conducting value */
 xsp = __eval_xpr(gp->gpins[2]);
 __new_inputval = (xsp->ap[0] & 1L) | ((xsp->bp[0] & 1L) << 1);
 if (__new_inputval == 2) __new_inputval = 3;
 __pop_xstk();
 gateid = gp->gmsym->el.eprimp->gateid;
 /* exactly 4 types of tranif gates */
 if (gateid == G_TRANIF1 || gateid == G_RTRANIF1)
  __new_gateval = __new_inputval;
 else
  { __new_gateval = (__new_inputval == 0) ? 1
   : ((__new_inputval == 1) ? 0 : __new_inputval);
  }
}

/*
 * CONTINOUS ASSIGN EVENT ROUTINES
 */

/*
 * load of changed wire is >1 bit conta rhs needs to be evaluated and
 * if delay scheduled else assigned and lhs added to net changes
 *
 * current itree element never changes in here
 * for now evaluating and scheduling - no optimization
 *
 * know rhs real illegal here since cannot assign to wire
 *
 * all computations done with non strength values since strength
 * added from conta type if needed when assigning to strength wire
 *
 * no pulse/glitch analysis here just inertial algorithm because
 * continuous assigns do not correspond to silicon rather modeling convention
 */
extern void __eval_conta_rhs_ld(register struct net_pin_t *npp)
{
 register struct xstk_t *xsp, *xsp2;
 register struct conta_t *cap;
 int32 cv, cv2, schd_wire;
 int32 lhswlen, orhslen, lhslen;
 byte *sbp;
 word64 cadel, schtim;
 i_tev_ndx tevpi;
 struct expr_t *lhsxp;
 struct xstk_t *xsp3;
 struct conta_t *mast_cap;

 if (__ev_tracing) { evtr_eval_conta_rhs_ld(npp); return; }

 /* SJM 09/18/02 - no separate per bit NP type, just check for pb sim on */
 mast_cap = npp->elnpp.ecap;
 /* get pattern never decomposd per bit */
 if (mast_cap->lhsx->getpatlhs) { __process_getpat(mast_cap); return; }

 if (mast_cap->ca_pb_sim) cap = &(mast_cap->pbcau.pbcaps[npp->pbi]);
 else cap = mast_cap;
 /* know getpat never has delay */
 lhsxp = cap->lhsx;
 lhslen = cap->lhsx->szu.xclen;
 /* this pushes rhs new maybe to be scheduled value onto expression stack */
 xsp = __eval_xpr(cap->rhsx);

 /* here rhs must be z with fixed conta strength merged in if present */
 if (xsp->xslen != lhslen)
  {
   orhslen = xsp->xslen;

   /* SJM 09/29/03 - change to handle sign extension and separate types */
   if (xsp->xslen > lhslen) __narrow_sizchg(xsp, lhslen);
   else if (xsp->xslen < lhslen)
    {
     if (cap->rhsx->has_sign) __sgn_xtnd_widen(xsp, lhslen);
     else __sizchg_widen(xsp, lhslen);
    }

   /* SJM 07/09/03 - now understand XL algorithm to mimic - for conta */
   /* semantics requires rhs non stren eval with 0 widening then add stren */
   /* SJM 05/10/04 init x widending not related to new signed widening */
   if (__wire_init) __fix_widened_toxs(xsp, orhslen);
  }

 /* case 1: no delay assign */
 /* since 1 bit handled as gate, not checking for no change - just assign */
 /* also rhs eval. is driver (i.e. not stored) */
 if (mast_cap->ca_delrep == DT_NONE)
  {
   __immed_assigns++;
   /* in here deals with saved driver - if any lhs bits fi>1 all must be */
   /* SJM 09/28/02 - know if master fi>1 all per bit will be */
   if (lhsxp->x_multfi)
    {
     /* this packs if possible */
     __st_perinst_val(cap->ca_drv_wp, lhslen, xsp->ap, xsp->bp);
     __mdr_assign_or_sched(lhsxp);
    }
   else
    {
     /* here do not need drv and do not need schd driver, rhs is driver */
     if (lhsxp->lhsx_ndel && !__wire_init) schd_wire = TRUE;
     else schd_wire = FALSE;
     if (lhsxp->x_stren)
      {
       /* convert to strength bytes forms - add in driven from ca */
       push_xstk_(xsp2, 4*lhslen);
       sbp = (byte *) xsp2->ap;
       __st_standval(sbp, xsp, cap->ca_stval);
       if (lhsxp->optyp == LCB) __stren_exec_ca_concat(lhsxp, sbp, schd_wire);
       else __exec_conta_assign(lhsxp, xsp2->ap, xsp2->bp, schd_wire);
       __pop_xstk();
      }
     else
      {
       if (lhsxp->optyp == LCB)
        __exec_ca_concat(lhsxp, xsp->ap, xsp->bp, schd_wire);
       else __exec_conta_assign(lhsxp, xsp->ap, xsp->bp, schd_wire);
      }
    }
   __pop_xstk();
   return;
  }

 /* case 2: has delay */
 lhswlen = wlen_(lhslen);
 /* xsp2 is currently driving (old) value and must exist */
 /* DBG remove ---
 if (cap->ca_drv_wp.wp == NULL) __arg_terr(__FILE__, __LINE__);
 -- */

 push_xstk_(xsp2, lhslen);
 __ld_perinst_val(xsp2->ap, xsp2->bp, cap->ca_drv_wp, lhslen);
 tevpi = cap->caschd_tevs[__inum];
 cv = memcmp(xsp2->ap, xsp->ap, 2*lhswlen*WRDBYTES);
 __pop_xstk();
 /* case 2a: short circuit case no event and new and old same */
 if (tevpi == -1 && cv == 0) { __pop_xstk(); return; }

 /* compute delay - know at least one bit changed */
 __new_gateval = 1L;
 /* if 4v delay, must set new_gateval for use in delay selection */
 /* notice modified LRM if left hand side all x's, minimum delay is used */
 if (mast_cap->ca_4vdel)
  {
   if (mast_cap->ca_pb_sim)
    {
     struct xstk_t *mast_xsp;

     /* if 4v delay, must always eval entire conta rhs to select delay */
     mast_xsp = __eval_xpr(mast_cap->rhsx);
     if (vval_is0_(mast_xsp->ap, lhslen))
      {
       if (vval_is0_(mast_xsp->bp, lhslen)) __new_gateval = 0L;
       else if (__vval_is1(mast_xsp->bp, lhslen)) __new_gateval = 2L;
      }
     else if (__vval_is1(mast_xsp->ap, lhslen)
      && __vval_is1(mast_xsp->bp, lhslen)) { __new_gateval = 3L; }
     __pop_xstk();
    }
   else
    {
     if (vval_is0_(xsp->ap, lhslen))
      {
       if (vval_is0_(xsp->bp, lhslen)) __new_gateval = 0L;
       else if (__vval_is1(xsp->bp, lhslen)) __new_gateval = 2L;
      }
     else if (__vval_is1(xsp->ap, lhslen) && __vval_is1(xsp->bp, lhslen))
      { __new_gateval = 3L; }
    }
  }
 /* this may use new gateval global to select delay */
 /* SJM 09/28/02 - delay same for all so stored in master */
 __get_del(&cadel, mast_cap->ca_du, mast_cap->ca_delrep);
 schtim = __simtime + cadel;

 /* case 2b: no pending event and different */
 if (tevpi == -1)
  {
   /* case 1b: new value to schedule */
   /* know xsp is lhs width */
   schedule_1caev(cap, cadel, schtim, xsp);
   __pop_xstk();
   return;
  }

 push_xstk_(xsp3, lhslen);
 __ld_perinst_val(xsp3->ap, xsp3->bp, cap->schd_drv_wp, lhslen);
 /* compare currently scheduled to to new to be scheduled */
 /* if same do nothing since already schedule to right value and know */
 /* value will be later */
 cv2 = memcmp(xsp3->ap, xsp->ap, 2*lhswlen*WRDBYTES);
 __pop_xstk();
 if (cv2 == 0) { __pop_xstk(); return; }

 /* case 2c: pending event - no spike analysis for >1 bit contas */
 /* case 2c-1 - new and old the same - cancel */
 if (cv == 0)
  {
   __tevtab[tevpi].te_cancel = TRUE;
    __inertial_cancels++;
   cap->caschd_tevs[__inum] = -1;
  }
 /* case 2c-2 - new and old differ - reschedule latest input change */
 /* notice even for modeling anomally where latest input change leads to */
 /* earlier output event, use latest input change */
 else reschedule_1caev(tevpi, cadel, schtim, xsp);

 __pop_xstk();
}

/*
 * event tracing version of eval conta rhs
 */
static void evtr_eval_conta_rhs_ld(struct net_pin_t *npp)
{
 int32 cv, schd_wire;
 int32 lhswlen, orhslen, lhslen;
 byte *sbp;
 word64 cadel, schtim;
 i_tev_ndx tevpi;
 struct tev_t *tevp;
 struct conta_t *cap, *mast_cap;
 struct xstk_t *xsp, *xsp2, *xsp3;
 struct expr_t *lhsxp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[RECLEN], s5[RECLEN];

 /* SJM 09/18/02 - no separate per bit NP type, just check for pb tab */
 mast_cap = npp->elnpp.ecap;
 /* know getpat never has delay and never decomposed per bit */
 if (mast_cap->lhsx->getpatlhs) { __process_getpat(mast_cap); return; }

 if (mast_cap->ca_pb_sim) cap = &(mast_cap->pbcau.pbcaps[npp->pbi]);
 else cap = mast_cap;
 lhsxp = cap->lhsx;

 if (mast_cap->ca_pb_sim)
  {
   __tr_msg("-- %s RHS bit %d changed:\n",
    __to_evtrcanam(__xs2, mast_cap, __inst_ptr), npp->pbi);
  }
 else
  {
   __tr_msg("-- %s RHS changed:\n",
    __to_evtrcanam(__xs2, mast_cap, __inst_ptr));
  }
 lhslen = cap->lhsx->szu.xclen;
 xsp = __eval_xpr(cap->rhsx);

 /* here rhs must be z with fixed conta strength merged in if present */
 if (xsp->xslen != lhslen)
  {
   orhslen = xsp->xslen;

   /* SJM 09/29/03 - change to handle sign extension and separate types */
   if (xsp->xslen > lhslen) __narrow_sizchg(xsp, lhslen);
   else if (xsp->xslen < lhslen)
    {
     if (cap->rhsx->has_sign) __sgn_xtnd_widen(xsp, lhslen);
     else __sizchg_widen(xsp, lhslen);
    }
   /* SJM 07/09/03 - now understand XL algorithm to mimic - if rhs is reg */
   /* or reg type expr, must widen with 0's (automatic) if net with zs */
   /* wire init is special case */
   /* SJM 05/10/04 init x widending not related to new signed widening */
   if (__wire_init) __fix_widened_toxs(xsp, orhslen);
  }

 /* case 1: no delay assign */
 /* since 1 bit handled as gate, not checking for no change - just assign */
 if (mast_cap->ca_delrep == DT_NONE)
  {
   __immed_assigns++;
   __tr_msg(" NODEL <NV=%s>\n",
    __regab_tostr(s1, xsp->ap, xsp->bp, lhslen, BHEX, FALSE));
   /* in here deals with saved driver */

   /* SJM 09/28/0-2 - know if master fi>1 all per bit will be */
   if (lhsxp->x_multfi)
    {
     __st_perinst_val(cap->ca_drv_wp, lhslen, xsp->ap, xsp->bp);
     __mdr_assign_or_sched(lhsxp);
    }
   else
    {
     /* here do not need drv and do not need schd driver, rhs is driver */
     if (lhsxp->lhsx_ndel && !__wire_init) schd_wire = TRUE;
     else schd_wire = FALSE;
     if (lhsxp->x_stren)
      {
       /* convert to strength bytes forms - add in driven from ca */
       push_xstk_(xsp2, 4*lhslen);
       sbp = (byte *) xsp2->ap;
       /* stren val also in PB */
       __st_standval(sbp, xsp, cap->ca_stval);
       if (lhsxp->optyp == LCB) __stren_exec_ca_concat(lhsxp, sbp, schd_wire);
       /* SJM 03/30/99 - was storing value without strength added */
       else __exec_conta_assign(lhsxp, xsp2->ap, xsp2->bp, schd_wire);
       __pop_xstk();
      }
     else
      {
       if (lhsxp->optyp == LCB)
        __exec_ca_concat(lhsxp, xsp->ap, xsp->bp, schd_wire);
       else __exec_conta_assign(lhsxp, xsp->ap, xsp->bp, schd_wire);
      }
    }
   __pop_xstk();
   return;
  }

 /* case 2: has delay */
 /* this is current (old) driving value */
 lhswlen = wlen_(lhslen);
 push_xstk_(xsp2, lhslen);
 __ld_perinst_val(xsp2->ap, xsp2->bp, cap->ca_drv_wp, lhslen);
 tevpi = cap->caschd_tevs[__inum];
 cv = memcmp(xsp2->ap, xsp->ap, 2*lhswlen*WRDBYTES);
 /* case 2a: short circuit case no event and new and old same */
 if (tevpi == -1 && cv == 0)
  {
   __tr_msg(" DEL, NOCHG <OV=%s>\n",
    __regab_tostr(s1, xsp2->ap, xsp2->bp, lhslen, BHEX, FALSE));
   __pop_xstk();
   __pop_xstk();
   return;
  }

 /* compute delay */
 __new_gateval = 1L;
 /* if 4v delay, must set new gateval for use in delay selection */
 /* SJM 09/28/02 - now match non evtr 4v case */
 if (mast_cap->ca_4vdel)
  {
   if (mast_cap->ca_pb_sim)
    {
     struct xstk_t *mast_xsp;

     /* if 4v delay, must always eval entire conta rhs to select delay */
     mast_xsp = __eval_xpr(mast_cap->rhsx);
     if (vval_is0_(mast_xsp->ap, lhslen))
      {
       if (vval_is0_(mast_xsp->bp, lhslen)) __new_gateval = 0L;
       else if (__vval_is1(mast_xsp->bp, lhslen)) __new_gateval = 2L;
      }
     else if (__vval_is1(mast_xsp->ap, lhslen)
      && __vval_is1(mast_xsp->bp, lhslen)) { __new_gateval = 3L; }
     __pop_xstk();
    }
   else
    {
     if (vval_is0_(xsp->ap, lhslen))
      {
       if (vval_is0_(xsp->bp, lhslen)) __new_gateval = 0L;
       else if (__vval_is1(xsp->bp, lhslen)) __new_gateval = 2L;
      }
     else if (__vval_is1(xsp->ap, lhslen) && __vval_is1(xsp->bp, lhslen))
      { __new_gateval = 3L; }
    }
  }
 /* this may use new gateval global to select delay */
 __get_del(&cadel, mast_cap->ca_du, mast_cap->ca_delrep);
 schtim = __simtime + cadel;

 /* case 2b: no pending event and different */
 if (tevpi == -1)
  {
   /* case 1b: new value to schedule */
   __tr_msg(" DEL, SCHD AT %s <OV=%s, NSV=%s>\n",
    __to_timstr(s1, &schtim),
    __regab_tostr(s2, xsp2->ap, xsp2->bp, lhslen, BHEX, FALSE),
    __regab_tostr(s3, xsp->ap, xsp->bp, lhslen, BHEX, FALSE));

   /* know xsp is lhs width */
   schedule_1caev(cap, cadel, schtim, xsp);
   __pop_xstk();
   __pop_xstk();
   return;
  }

 /* case 2c: pending event - no spike analysis for >1 bit contas */
 tevp = &(__tevtab[tevpi]);
 push_xstk_(xsp3, lhslen);
 __ld_perinst_val(xsp3->ap, xsp3->bp, cap->schd_drv_wp, lhslen);

 /* compare currently scheduled to to new to be scheduled */
 /* if same do nothing since already schedule to right value and know */
 /* value will be later */
 if (memcmp(xsp3->ap, xsp->ap, 2*lhswlen*WRDBYTES) == 0)
  {
   __tr_msg(
    " DEL, MODEL ANOMALLY IGNORE SCHED TO SAME <OSV=NSV=%s> OLD AT %s NEW %s\n",
    __regab_tostr(s1, xsp->ap, xsp->bp, lhslen, BHEX, FALSE),
    __to_timstr(s2, &(tevp->etime)), __to_timstr(s3, &schtim));
   goto done;
  }

 /* case 2c-1-a - new and old the same - remove inertial pulse */
 if (cv == 0)
  {
   /* cancel pending and return */
   __tr_msg(" DEL, PEND, SAME <OV=NSV=%s, OSV=%s AT %s INERTIAL CANCEL>\n",
    __regab_tostr(s1, xsp->ap, xsp->bp, lhslen, BHEX, FALSE),
    __regab_tostr(s2, xsp3->ap, xsp3->bp, lhslen, BHEX, FALSE),
    __to_timstr(s3, &(tevp->etime)));
   /* cancel */
   tevp->te_cancel = TRUE;
   __inertial_cancels++;
   cap->caschd_tevs[__inum] = -1;
   goto done;
  }
 /* case 2c-1-b - new and old differ */
 __tr_msg(
  " DEL, PEND, RESCHD <OV=%s, OSV=%s AT %s, NSV=%s AT %s REPLACES>\n",
  __regab_tostr(s1, xsp2->ap, xsp2->bp, lhslen, BHEX, FALSE),
  __regab_tostr(s2, xsp3->ap, xsp3->bp, lhslen, BHEX, FALSE),
  __to_timstr(s5, &(tevp->etime)),
  __regab_tostr(s3, xsp->ap, xsp->bp, lhslen, BHEX, FALSE),
  __to_timstr(s4, &schtim));

 /* reschedule by replacing (if same time) or cancelling */
 reschedule_1caev(tevpi, cadel, schtim, xsp);

done:
 __pop_xstk();
 __pop_xstk();
 __pop_xstk();
}

/*
 * emit an continous assign locator
 * must make sure s at least IDLEN + MSG TRUNC LEN
 */
extern char *__to_evtrcanam(char *s, struct conta_t *cap,
 struct itree_t *teitp)
{
 char s1[RECLEN], s2[RECLEN];

 sprintf(s, "continuous assign in %s %s", __msg2_blditree(s1, teitp),
  __bld_lineloc(s2, cap->casym->syfnam_ind, cap->casym->sylin_cnt));
 return(s);
}

/*
 * schedule 1 conta event
 * know schd_xsp width is exactly lhs width
 */
static void schedule_1caev(struct conta_t *cap, word64 cadel,
 word64 schtim, struct xstk_t *schd_xsp)
{
 register i_tev_ndx tevpi;

 alloc_tev_(tevpi, TE_CA, __inst_ptr, schtim);
 if (cadel == 0ULL)
  {
   /* this is #0, but must still build tev */
   if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevpi;
   else { __tevtab[__p0_te_endi].tenxti = tevpi; __p0_te_endi = tevpi; }
  }
 else __insert_event(tevpi);

 cap->caschd_tevs[__inum] = tevpi;
 __tevtab[tevpi].tu.tecap = cap;
 __st_perinst_val(cap->schd_drv_wp, schd_xsp->xslen, schd_xsp->ap,
  schd_xsp->bp);
}

/*
 * take ca event and new value and either update if time same or cancel and
 * create new event if later
 */
static void reschedule_1caev(i_tev_ndx tevpi, word64 cadel,
 word64 newtim, struct xstk_t *schd_xsp)
{
 struct tev_t *tevp;
 struct conta_t *cap;

 tevp = &(__tevtab[tevpi]);
 /* if del == 0 (pnd0), will always be same time reschedule */
 cap = tevp->tu.tecap;
 if (cadel == 0ULL)
  {
   /* new scheduled value replaces old */
   __newval_rescheds++;
   /* know length must be the same */
   __st_perinst_val(cap->schd_drv_wp, schd_xsp->xslen, schd_xsp->ap,
    schd_xsp->bp);
   return;
  }
 /* cancel */
 tevp->te_cancel = TRUE;
 __inertial_cancels++;
 /* this will change the scheduled field so no need to set to nil */
 schedule_1caev(cap, cadel, newtim, schd_xsp);
}

/*
 * process a continous assign actual assignment (end of delay ev triggered)
 * 1 bit continuous assign are processed as gates per Verilog semantics
 * and not seen here
 *
 * will only get here if delay >= 0 (maybe #0)
 * SJM 09/28/02 - for rhs concat decomposed into PB, event ptr is PB
 */
static void process_conta_ev(register struct tev_t *tevp)
{
 register struct xstk_t *xsp, *xsp2;
 register struct conta_t *cap;
 int32 schd_wire, lhslen;
 byte *sbp;
 struct expr_t *lhsxp;

 cap = tevp->tu.tecap;
 lhsxp = cap->lhsx;
 lhslen = cap->lhsx->szu.xclen;

 push_xstk_(xsp, lhslen);
 __ld_perinst_val(xsp->ap, xsp->bp, cap->schd_drv_wp, lhslen);
 if (__ev_tracing)
  {
   struct conta_t *cap2;

   __evtr_resume_msg();
   if (cap->ca_pb_el) cap2 = cap->pbcau.mast_cap; else cap2 = cap;
   __to_evtrcanam(__xs, cap2, tevp->teitp);
   __regab_tostr(__xs2, xsp->ap, xsp->bp, lhslen, BHEX, FALSE);
   if (lhsxp->x_multfi)
    __tr_msg("-- %s event this driver of multiple:\n   %s\n", __xs, __xs2);
   else __tr_msg("-- %s event the fi=1 driver: %s\n", __xs, __xs2);
  }
 /* move value from scheduled to driver wp - never store constant strength */
 /* has delay so will always have ca drv wp */
 /* store scheduled value into driving value */
 __st_perinst_val(cap->ca_drv_wp, lhslen, xsp->ap, xsp->bp);

 if (lhsxp->x_multfi) __mdr_assign_or_sched(lhsxp);
 else
  {
   /* here do not need drv and do not need schd driver, rhs is driver */
   if (lhsxp->lhsx_ndel && !__wire_init) schd_wire = TRUE;
   else schd_wire = FALSE;

   if (lhsxp->x_stren)
    {
     /* convert to strength bytes forms - add in driven from ca */
     /* know all widths exactly  required lhs width */
     push_xstk_(xsp2, 4*lhslen);
     sbp = (byte *) xsp2->ap;
     /* notice stren value also in each per bit, just not delay */
     __st_standval(sbp, xsp, cap->ca_stval);
     if (lhsxp->optyp == LCB) __stren_exec_ca_concat(lhsxp, sbp, schd_wire);
     else __exec_conta_assign(lhsxp, xsp2->ap, xsp2->bp, schd_wire);
     __pop_xstk();
    }
   else
    {
     if (lhsxp->optyp == LCB)
      __exec_ca_concat(lhsxp, xsp->ap, xsp->bp, schd_wire);
     else __exec_conta_assign(lhsxp, xsp->ap, xsp->bp, schd_wire);
    }
  }
 __pop_xstk();
 cap->caschd_tevs[__inum] = -1;
 /* can just leave scheduled wire value - nothing to free */
}

/*
 * process a wire delay event - know this is always 1 bit
 * non inout path dest. scheduled wire changes processed here also
 *
 * since r,f or path delays will have different delays and inertial
 * conditions for every bit
 * know for scalar bi 0 not -1
 *
 * could possibly optimize one delay form
 * will only get here if wire has delay > 0 (or #0)
 * also know path source or destination can never have wire delay
 *
 * notice when wire changes must see if really changes by doing
 * fi>1 competition of right type using scheduled plus current
 * then know scheduled value really changed
 */
static void process_wire_ev(register struct tev_t *tevp)
{
 register int32 bi;
 register struct net_t *np;
 register byte *sbp;
 word32 nval;
 struct rngdwir_t *dwirp;

 np = tevp->tu.tenp->tenu.np;
 bi = tevp->tu.tenp->nbi;
 /* DBG remove ---
 if (bi < 0) __misc_terr(__FILE__, __LINE__);
 --- */

 /* free wire event auxialiary field here since bit and wire extracted */
 __my_free((char *) tevp->tu.tenp, sizeof(struct tenp_t));
 tevp->tu.tenp = NULL;

 nval = tevp->outv;
 if (__ev_tracing)
  {
   char s1[RECLEN], s2[RECLEN];

   __evtr_resume_msg();
   if (np->n_isapthdst) strcpy(s2, " (path destination)");
   else strcpy(s2, "");
   __tr_msg("-- processing delay wire %s%s store event, value %s\n",
    __to_evtrwnam(__xs, np, bi, bi, tevp->teitp), s2,
     __to_vvnam(s1, (word32) nval));
  }
 dwirp = np->nu.rngdwir;
 dwirp->wschd_pbtevs[np->nwid*tevp->teitp->itinum + bi] = -1;

 /* inhibit if active force */
 if (np->frc_assgn_allocated && force_inhibit_wireassign(np, bi, tevp->teitp))
  return;

 __push_itstk(tevp->teitp);

 /* store bit into wire - value is after any multi-fi competition */
 /* this add net chg element if needed */
 if (np->n_stren)
  {
   if (tevp->te_trdecay)
    __gfwarn(649, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
     "charge on node %s.%s has decayed", __msg2_blditree(__xs, __inst_ptr),
     np->nsym->synam);

   /* get strength wire address */
   get_stwire_addr_(sbp, np);
   if (sbp[bi] != nval)
    {
     sbp[bi] = nval;
     /* know change, record if needed */
     record_sel_nchg_(np, bi, bi);
    }
  }
 else __chg_st_bit(np, bi, nval & 1L, (nval >> 1) & 1L);
 __pop_itstk();
}

/*
 * process a a non blocking procedural assign event - do the assign
 * here just assign - no inertial - each just overwrites
 *
 * SJM 08/08/99 - fixed so lhs indices evaluated at schedule time not
 * event proces time to match LRM and XL
 */
static void process_nbpa_ev(struct tev_t *tevp)
{
 register word32 *wp;
 register struct expr_t *con_lhsxp;
 register struct st_t *stp;
 int32 wlen;
 struct tenbpa_t *tenbp;

 __push_itstk(tevp->teitp);
 tenbp = tevp->tu.tenbpa;
 wp = tenbp->nbawp;
 stp = tenbp->nbastp;

 /* SJM 08/08/99 - need to assign to copied lhs expr with select indices */
 /* (possibly many if lhs concatenate) replaced by constants */
 /* SJM PUTMEBACK */
 con_lhsxp = tenbp->nblhsxp;
 if (con_lhsxp == NULL) con_lhsxp = stp->st.spra.lhsx;

 wlen = wlen_(con_lhsxp->szu.xclen);
 if (__ev_tracing)
  {
   char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[RECLEN];

   __tr_msg("-- nb event assign in %s %s of %s to %s at %s\n",
    __msg2_blditree(s1, tevp->teitp), __bld_lineloc(s2,
     tenbp->nbastp->stfnam_ind, tenbp->nbastp->stlin_cnt),
    __xregab_tostr(s3, wp, &wp[wlen], con_lhsxp->szu.xclen, stp->st.spra.rhsx),
    __msgexpr_tostr(s4, con_lhsxp), __to_timstr(__xs, &__simtime));
  }

 /* SJM 05/19/04 - complex procedural assign needs stmt file/line context */
 /* for error messages such as out of range - do not need to save since */
 /* no statement context in event processing - this is exception */
 __slin_cnt = stp->stlin_cnt;
 __sfnam_ind = stp->stfnam_ind;

 /* do assign - know if lhs expr copied, widith still same */
 __exec2_proc_assign(con_lhsxp, wp, &(wp[wlen]));

 /* final step is freeing contents */
 __my_free((char *) wp, 2*wlen*WRDBYTES);
 /* if needed to copy lhs expr., now free */
 if (tenbp->nblhsxp != NULL) __free_xtree(tenbp->nblhsxp);

 __my_free((char *) tevp->tu.tenbpa, sizeof(struct tenbpa_t));
 tevp->tu.tenbpa = NULL;
 __pop_itstk();
}

/*
 * print out evnet trace time - not in event trace message
 */
extern void __evtr_resume_msg(void)
{
 char s1[RECLEN];

 if (__last_evtrtime != __simtime)
  {
   /* this should go through time format ? */
   __tr_msg("\n<<< event tracing at time %s\n", __to_timstr(s1, &__simtime));
   __last_evtrtime = __simtime;
  }
}

/*
 * process a special getpattern form continous assigment
 * know left of rhsx is $getpattern symbol and right is var. array index
 *
 * notice this is a special immediate overriding assign and does not handle
 * multi-fi wire properties or wire delays or strengths
 *
 * should probably try to optimize special 64 bit case too
 * point of this is to optimized for known fixed form
 *
 * could keep old getpattern value and build index of change and bit
 * select to change those
 * cannot be xmr
 */
extern void __process_getpat(struct conta_t *cap)
{
 register int32 bi;
 register struct expr_t *catx;
 register word32 cbita, cbitb;
 int32 i, wlen, ubits;
 word32 tmpa, tmpb;
 struct expr_t *idndp, *lhsxp, *rhsxp;
 struct xstk_t *xsp;
 struct net_t *np;

 /* know rhs is variable array index */
 /* rhs is get pattern function call */
 lhsxp = cap->lhsx;
 rhsxp = cap->rhsx;
 xsp = __eval_xpr(rhsxp->ru.x->lu.x);
 /* this is except to convert to lhs width - extra array bits ignored or */
 /* lhs just not filled */

 /* if out of range or x, value will be changed to x */

 if (__ev_tracing)
  {
   __evtr_resume_msg();
   __tr_msg("-- $getpattern %s processed\n",
    __to_evtrcanam(__xs, cap, __inst_ptr));
  }

 /* wider than 1 word32 case */
 wlen = wlen_(lhsxp->szu.xclen);
 ubits = ubits_(lhsxp->szu.xclen);
 catx = lhsxp->ru.x;
 bi = (ubits == 0) ? WBITS - 1: ubits - 1;
 for (i = wlen - 1; i >= 0; i--)
  {
   tmpa = xsp->ap[i];
   tmpb = xsp->bp[i];
   /* know need prop. turned off after last propagation (off here) */
   for (; bi >= 0; catx = catx->ru.x, bi--)
    {
     if (catx == NULL) goto done;

     idndp = catx->lu.x;
     np = idndp->lu.sy->el.enp;

     cbita = (tmpa >> bi) & 1L;
     cbitb = (tmpb >> bi) & 1L;
     chg_st_scalval_(np->nva.bp, cbita, cbitb);

     /* know lhs get pat concat elements are scalars */
     if (__lhs_changed) record_nchg_(np);
    }
   bi = WBITS - 1;
   if (catx == NULL) break;
  }
done:
 __immed_assigns++;
 __pop_xstk();
}

/*
 * emit an netname for tracing with path if needed
 * for ev. know never task/func. part of xmr reference
 */
extern char *__to_evtrwnam(char *s, struct net_t *np, int32 bi1, int32 bi2,
 struct itree_t *teitp)
{
 char s1[RECLEN], s2[RECLEN];

 __msg2_blditree(s1, teitp);
 strcat(s1, ".");
 strcat(s1, __schop(s2, np->nsym->synam));

 if (bi1 == -1 || !np->n_isavec) strcpy(s, s1);
 else if (bi1 == bi2) sprintf(s, "%s[%d]", s1, __unnormalize_ndx(np, bi1));
 else sprintf(s, "%s[%d:%d]", s1, __unnormalize_ndx(np, bi1),
  __unnormalize_ndx(np, bi2));
 return(s);
}

/*
 * emit an MIPD port name for tracing with path if needed
 *
 * port can be only 1 bit always number from hight to low so no normalize
 */
extern char *__to_evtrpnam(char *s, struct mod_pin_t *mpp, int32 bi,
 struct itree_t *teitp)
{
 char s1[RECLEN], s2[RECLEN];

 __msg2_blditree(s1, teitp);
 strcat(s1, ".");
 __schop(s2, __to_mpnam(__xs, mpp->mpsnam));
 strcat(s1, s2);

 if (bi == -1 || mpp->mpwide == 1) strcpy(s, s1);
 else sprintf(s, "%s[%d]", s1, bi);
 return(s);
}

/*
 * for one bit, know some bits forced inhibit assign if this bit forced
 * here do not need to worry about some bits only forced from range
 * this is only for wire where 1 bit per bit*inst product
 */
static int32 force_inhibit_wireassign(struct net_t *np, register int32 biti,
 struct itree_t *itp)
{
 register struct qcval_t *frc_qcp;
 int32 nd_itpop, rv;

 if (itp != NULL) { __push_itstk(itp); nd_itpop = TRUE; }
 else nd_itpop = FALSE;
 frc_qcp =  &(np->nu2.qcval[__inum*np->nwid + biti]);
 if (frc_qcp->qc_active) rv = TRUE; else rv = FALSE;
 if (nd_itpop) __pop_itstk();
 return(rv);
}

/*
 * process a path dest. tran (inout) wire delay internal hard driver
 * change event
 *
 * for inout path. know the one driver changed previously and path delayed
 * update of hard driver internal tran channel value for the wire
 * any other change of path will cause event cancel and new schedule
 * so fact that the drivers if evaled will be new value still works
 *
 * since r,f or path delays will have different delays and inertial
 * conditions for every bit, know for scalar bi 0 not -1
 */
static void process_trpthdst_ev(register struct tev_t *tevp)
{
 register int32 bi;
 register struct net_t *np;
 register byte *sbp;
 struct traux_t *trap;
 word32 nval, av, bv;
 struct rngdwir_t *dwirp;
 struct xstk_t *xsp;

 /* notice event here emitted in change gate outwire */
 np = tevp->tu.tenp->tenu.np;
 bi = tevp->tu.tenp->nbi;
 /* DBG remove ---
 if (bi < 0) __misc_terr(__FILE__, __LINE__);
 --- */

 /* free wire event auxialiary field here since bit and wire extracted */
 __my_free((char *) tevp->tu.tenp, sizeof(struct tenp_t));
 tevp->tu.tenp = NULL;

 nval = tevp->outv;
 if (__ev_tracing)
  {
   char s1[RECLEN];

   __evtr_resume_msg();
   __tr_msg(
    "-- processing inout path dest. %s driven value update event, value %s\n",
    __to_evtrwnam(__xs, np, bi, bi, tevp->teitp),
     __to_vvnam(s1, (word32) nval));
  }
 dwirp = np->nu.rngdwir;
 dwirp->wschd_pbtevs[np->nwid*tevp->teitp->itinum + bi] = -1;

 trap = np->ntraux;
 __push_itstk(tevp->teitp);
 /* update hard driver stored value and re-eval tran channel if needed */
 if (np->n_stren)
  {
   /* get strength wire address */
   sbp = &(trap->trnva.bp[__inum*np->nwid]);
   if (sbp[bi] == nval) goto done;
   sbp[bi] = nval;
  }
 else
  {
   if (!np->n_isavec)
    {
     ld_scalval_(&av, &bv, trap->trnva.bp);
     if (nval == (av | (bv << 1))) goto done;
     /* SJM 07/16/01 - typo was storing old val so tr chan value never chgs */
     /* need to store new non stren value not old */
     /* ??? wrong - st_scalval_(trap->trnva.bp, av, bv); */
     st2_scalval_(trap->trnva.bp, nval);
    }
   else
    {
     push_xstk_(xsp, np->nwid);
     __ld_perinst_val(xsp->ap, xsp->bp, trap->trnva, np->nwid);
     av = rhsbsel_(xsp->ap, bi);
     bv = rhsbsel_(xsp->bp, bi);
     if (nval == (av | (bv << 1))) { __pop_xstk(); goto done; }
     __lhsbsel(xsp->ap, bi, (nval & 1L));
     __lhsbsel(xsp->bp, bi, ((nval >> 1) & 1L));
     __st_perinst_val(trap->trnva, np->nwid, xsp->ap, xsp->bp);
     __pop_xstk();
    }
  }
 /* if some but not this bit in tran channel, just assign */
 /* SJM - 03/15/01 - know bit not -1 since schedules as 0 for scalar */
 __eval_tran_1bit(np, bi);
done:
 __pop_itstk();
}

/*
 * ROUTINES TO PROCESS BEHAVIORAL EVENTS
 */

/* table for converting 4 bit (oonn) edge pair to edge value byte */
/* table treats edge with z as x here */
byte __epair_tab[] =
 { NOEDGE, EDGE01, EDGE0X, EDGE0X, EDGE10, NOEDGE, EDGE1X, EDGE1X,
   EDGEX0, EDGEX1, NOEDGE, NOEDGE, EDGEX0, EDGEX1, NOEDGE, NOEDGE };

/*
 * after net changed net (wire or reg) progagate to all dces wire drives
 * bit range passed and used to eliminate fan-out for other bits here
 * all ranges here normalized high to low form
 * notice will never get to event trigger through this path (through cause)
 *
 * inst. ptr here is place np changed (i.e. for XMR define itree loc)
 * know npi1 >= npi2 since normalized internally
 */
extern void __wakeup_delay_ctrls(register struct net_t *np, register int32 npi1,
 register int32 npi2)
{
 register struct dcevnt_t *dcep, *dcep2;
 register word32 *wp;
 int32 nd_itpop, oneinst, tevpi, i1;
 word32 oval, nval;
 byte emask;
 struct delctrl_t *dctp;
 struct fmonlst_t *fmonp;
 struct fmselst_t *fmsep;
 struct dce_expr_t *dcexp;

 for (dcep = np->dcelst; dcep != NULL; )
  {
   /* --- DBG remove ---
   if (__inst_ptr == NULL) __misc_terr(__FILE__, __LINE__);
   --- */

   /* filter one instance forms before case */
   if (dcep->dce_1inst && dcep->dce_matchitp != __inst_ptr) 
    { dcep = dcep->dcenxt; continue; } 

   /* SJM 10/06/06 - for vpi vc call back, may free the dcep so must save */
   dcep2 = dcep->dcenxt; 
   switch ((byte) dcep->dce_typ) {
    case DCE_RNG_INST:
     /* SJM 11/25/02 - notice can't be turned off/on */
     dctp = dcep->st_dctrl;
     /* all of wire changed match */
     if (npi1 == -1) goto do_event_ctrl;
     /* dce is range DCE range, know dci1 cannot be -1 */
     if (dcep->dci1 == -2)
      {
       /* SJM 10/12/04 - because contab realloced, must be ndx base of IS */
       wp = &(__contab[dcep->dci2.xvi]);
       i1 = (int32) wp[2*__inum];
       /* change must be inside range to match */
       if (i1 > npi1 || i1 < npi2) break;
      }
     else
      {
       /* SJM 06/26/04 - FIXME ??? ### isn't else needed here ??? */
       /* eliminate if changed bit do not overlap range */
       /* if low chged above high or high chged below low, eliminate */
       if (npi2 > dcep->dci1 || npi1 < dcep->dci2.i) break;
      }
     goto do_event_ctrl;
    case DCE_INST:
     /* notice dce that is entire wire always matches changed range */
     dctp = dcep->st_dctrl;

do_event_ctrl:
     nd_itpop = FALSE;
     /* first see if variable really changed (plus edge filtering) */
     oval = nval = 3;
     /* if no chg record, then array or reg entire wire so know changed */
     /* know for any wire even scalar, will exist */
     /* LOOKATME - filtering even for DOWN XMR insts that do not match? */
     if (dcep->prevval.wp != NULL)
      {
       /* also eliminate if event control range or wire did not change */
       /* for xmr and/or collpase - look in target itree where dce attached */
       /* for non dce expr form, sets old and new values for edge detection */
       oneinst = (dcep->dce_1inst) ? TRUE : FALSE;
       if (!np->n_isavec)
        {
         /* SJM 06/29/04 - simplified - always use stren version for scalar */
         if (!scal_stfilter_dce_chg(np, dcep, &oval, &nval, oneinst))
          goto dce_done;
        }
       else
        {
         if (!np->n_stren)
          {
           if (!filter_dce_chg(np, dcep, &oval, &nval, oneinst))
            goto dce_done;
          }
         else
          {
           if (!stfilter_dce_chg(np, dcep, &oval, &nval, oneinst))
            goto dce_done;
          }
        }
      }
     /*
      * idea for normal event control dce:
      * during prep: first for xmr trace from ref to target (place where wire)
      *
      * here when triggering (i.e. wakeup and schedule thread)
      * if xmr to on target place to wake up, for multiply
      * instantiated down will be one dce for each down inst. - when up wire
      * changes need to match one right dce itree loc which is done by
      * comparing against when match move down to xmr move from target back
      * to xmr ref.
      */
     /* if one inst form (know matches) move to reference itree loc. */
     if (dcep->dce_1inst)
      { __push_itstk(dcep->dce_refitp); nd_itpop = TRUE; }
     /* for xmr know target wire changed @(i1.i2.i3.i4.w)  w in dest. */
     else if (dcep->dce_xmrtyp != XNP_LOC)
      {
       /* SJM 04/17/03 - if not right instance do not process */
       if (!__match_push_targ_to_ref(dcep->dce_xmrtyp, dcep->dceu.dcegrp))
         break;
       nd_itpop = TRUE;
      }

     /* if armed (i.e. evctrl active) normal processing */
     /* notice current thread (init/always) may differ from dctp thread */
     /* so current thread must not be used here */
     if ((tevpi = dctp->dceschd_tevs[__inum]) != -1)
      {
       /* RELEASE remove ---
       {
        struct tev_t *tevp;

        tevp = &(__tevtab[tevpi]);
        if (tevp->tetyp < 1 || tevp->tetyp > 14)
         __misc_terr(__FILE__, __LINE__);
       }
       --- */
       /* filter out if wrong edge - know if has edge prev val set */
       /* unless dce expr also set (non nul) */
       if (dcep->dce_edge)
        {
         /* eval. expr - know at ref. (not var. chg) itree loc. */
         if ((dcexp = dcep->dce_expr) != NULL)
          {
           /* if XMR dce, already at ref itree loc*/
           if (!filter_edge_expr(dcexp, &oval, &nval)) goto dce_done;
           /* nval and oval set for edge checking below */
          }

         /* even though only pos and neg legal here use general signature */
         /* dce edgval is 1 bit per edge type table - epair tab maps to bit */
         emask = __epair_tab[nval | (oval << 2)];
         /* if no bits in common, no match */
         if (((byte) dcep->dce_edgval & emask) == 0) goto dce_done;
        }
       /* last: after move to ref inst, need to match itree loc for iact */
       /* need edge check before here because update old eval */
       if (dctp->dc_iact && dcep->iact_itp != __inst_ptr) goto dce_done;

       /* 10/27/00 SJM - if repeat form check and decrement repeat count */
       /* and if not counted down to 0 yet, do nothing (filter out) */
       if (dctp->repcntx != NULL)
        {
         /* REMOVEME AGAIN 04/01/02 ??? */
         /* DBG remove --- */
         if (__debug_flg)
          {
           __dbg_msg(
            "+++ now %s repeated edge for net %s (inst %s) count %d\n",
            __to_timstr(__xs2, &__simtime), np->nsym->synam,
            __msg2_blditree(__xs, __inst_ptr),
            (int32) dctp->dce_repcnts[__inum] - 1);
          }
         /* --- */
         /* SJM 04/02/02 since word32, any positive still do not trigger */
         if (--dctp->dce_repcnts[__inum] != 0) goto dce_done;
        }
       trigger_evctrl(dctp, tevpi);
      }

dce_done:
     if (nd_itpop) __pop_itstk();
     break;
    case DCE_RNG_MONIT:
     /* no -2 IS form since 1 active monit from 1 itree place only */
     /* if enire wire changed, always match */
     if (npi1 != -1 && (npi1 < dcep->dci2.i || npi2 > dcep->dci1)) break;
     /*FALLTHRU */
    case DCE_MONIT:
     /* SJM 11/25/02 - only check off for ones that can be off */
     if (dcep->dce_off) break;

     /* notice these work by storing old and new values */
     if (dcep->dce_matchitp != __inst_ptr) break;
     /* fmon nil for the one monitor in design */
     if (dcep->dceu2.dce_fmon == NULL) __slotend_action |= SE_MONIT_TRIGGER;
     else
      {
       fmonp = dcep->dceu2.dce_fmon;
       /* if already activated, nothing to do */
       if (fmonp->fmse_trig == NULL)
        {
         /* allocate new se fmon */
         if (__fmse_freelst == NULL)
          fmsep = (struct fmselst_t *) __my_malloc(sizeof(struct fmselst_t));
         else
          {
           fmsep = __fmse_freelst;
           __fmse_freelst = __fmse_freelst->fmsenxt;
          }
         fmsep->fmsenxt = NULL;
         fmsep->fmon = fmonp;

         /* link it on list */
         if (__fmonse_hdr == NULL) __fmonse_hdr = fmsep;
         else __fmonse_end->fmsenxt = fmsep;
         __fmonse_end = fmsep;
         /* mark triggered */
         fmonp->fmse_trig = fmsep;
         __slotend_action |= SE_FMONIT_TRIGGER;
        }
      }
     break;
    case DCE_RNG_QCAF:
     if (dcep->dce_off) break;
     /* no -2 IS form since 1 active from 1 itree place only */
     /* if enire wire changed, always match */
     if (npi1 != -1 && (npi1 < dcep->dci2.i || npi2 > dcep->dci1)) break;
     /*FALLTHRU */
    case DCE_QCAF:
     if (dcep->dce_off) break;
     if (dcep->dce_matchitp != __inst_ptr) break;
     /* do not care which rhs wire changed must eval and assign all */
     __assign_qcaf(dcep);
     break;
    case DCE_RNG_PVC:
     /* SJM 07/24/00 - must turn off PLI 1.0 PV dces from inside self */
     if (dcep->dce_off) break;

     /* no -2 IS form since 1 active from 1 itree place only */
     /* if enire wire changed, always match */
     if (npi1 != -1 && (npi1 < dcep->dci2.i || npi2 > dcep->dci1)) break;
     /*FALLTHRU */
    case DCE_PVC:
     /* SJM 07/24/00 - must turn off PLI 1.0 PV dces from inside self */
     if (dcep->dce_off) break;

     /* notice tf PV change always per instance */
     if (dcep->dce_matchitp != __inst_ptr) break;

     /* must check to make sure psel assign changed bits in actual range */
     oval = nval = 3;
     /* if no chg record, then array or reg entire wire so know changed */
     /* one dce for each different inst and location of _tf call */
     if (dcep->prevval.wp != NULL)
      {
       if (np->n_stren)
        { if (!stfilter_dce_chg(np, dcep, &oval, &nval, TRUE)) break; }
       else
        { if (!filter_dce_chg(np, dcep, &oval, &nval, TRUE)) break; }
      }
     /* do not care which rhs wire changed must eval and assign all */
     __pvc_call_misctf(dcep);
     break;
    case DCE_RNG_CBVC:
     /* SJM 07/24/00 - must turn off PLI 1.0 PV dces from inside self */
     if (dcep->dce_off) break;

     /* callback value change but dce contents differ */
     /* no -2 IS form since 1 active from 1 itree place only */
     /* if enire wire changed, always match */
     if (npi1 != -1 && (npi1 < dcep->dci2.i || npi2 > dcep->dci1)) break;
     /*FALLTHRU */
    case DCE_CBVC:
     /* SJM 07/24/00 - must turn off PLI 1.0 PV dces from inside self */
     if (dcep->dce_off) break;

     if (dcep->dce_matchitp != __inst_ptr) break;
     /* DBG remove ---
     if (__debug_flg && np->n_stren)
      {
       int32 dwid;
       byte *sbp;
       char s1[RECLEN];

       get_stwire_addr_(sbp, np);
       -* SJM 06/03/02 - was wrongly checking dci2 *-
       if (dcep->dci1 == -2) __misc_terr(__FILE__, __LINE__);

       if (dcep->dci1 != -1)
        {
         sbp = &(sbp[dcep->dci2.i]);
         dwid = (dcep->dci1 - dcep->dci2.i) + 1;
         sprintf(s1, "%s[%d:%d]", np->nsym->synam, dcep->dci1, dcep->dci2.i);
        }
       else { dwid = np->nwid; strcpy(s1, np->nsym->synam); }
       __dbg_msg("CBVC: %s strength value %s (old %s)\n", s1,
        __st_regab_tostr(__xs, sbp, dwid),
        __st_regab_tostr(__xs2, dcep->prevval.bp, dwid));
      }
     else
      {
       struct xstk_t *xsp, *xsp2;

       push_xstk_(xsp, np->nwid);
       __ld_wire_val(xsp->ap, xsp->bp, np);

       if (dcep->prevval.wp != NULL)
        {
         -* know have current instance here *-
         push_xstk_(xsp2, np->nwid);
         __ld_perinst_val(xsp2->ap, xsp2->bp, dcep->prevval, np->nwid);
         __regab_tostr(__xs2, xsp2->ap, xsp2->bp, xsp2->xslen, BHEX, FALSE);
         __pop_xstk();
        }
       else strcpy(__xs2, "**none**");

       __dbg_msg("CBVC: value %s (old %s)\n",
        __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE), __xs2);
       __pop_xstk();
      }
     --- */

     oval = nval = 3;
     /* if no chg record, then array or reg entire wire so know changed */
     /* one dce for each different inst and location of _tf call */
     if (dcep->prevval.wp != NULL)
      {
       if (!np->n_isavec)
        {
         /* 05/20/00 - SJM - following LRM vi vpi stren report st chg */
         /* user passed non stren val request to vpi_ cb call back */
         if (!np->n_stren || dcep->dce_nomonstren)
          {
           /* SJM 06/29/04 - simplified - always use stren version for scal */
           if (!scal_stfilter_dce_chg(np, dcep, &oval, &nval, TRUE))
            break;
          }
         else
          {
           /* need strength changes too */
            if (!vccb_scal_standval_filter(np, dcep, &oval, &nval, TRUE))
             break;
          }
        }
       else
        {
         if (!np->n_stren)
          { if (!filter_dce_chg(np, dcep, &oval, &nval, TRUE)) break; }
         else
          {
           /* 05/20/00 - SJM - following LRM vi vpi stren report st chg */
           /* user passed non stren val request to vpi_ cb call back */
           if (dcep->dce_nomonstren)
            {
             if (!stfilter_dce_chg(np, dcep, &oval, &nval, TRUE)) break;
            }
           else
            {
             /* need strength changes too */
             if (!vccb_vec_standval_filter(np, dcep, &oval, &nval, TRUE))
              break;
            }
          }
        }
      }
     /* need one call back for every change */

     /* SJM 07/24/00 - must run with this call back turned off in case */
     /* call back c code does put value to reg because change propagation */
     /* for regs must be immediate */
     /* notice will never get here unless dce on */
     dcep->dce_off = TRUE;
     /* SJM 10/06/06 - must pass the dce since dce cbp has list of dces */ 
     __cbvc_callback(dcep, dcep->dceu.dce_cbp, dcep->dceu.dce_cbp->cb_hp);

     /* SJM 10/06/06 - dcep may be free in the user call back so cbvc */
     /* call back processing code handles turning back on if user did */
     /* not turn off in the cb routine - also loop must handle freed case */
     break;

    /* these are used only in vpi_ for force/release call backs */
    case DCE_CBF: case DCE_RNG_CBF: case DCE_CBR: case DCE_RNG_CBR:
     break;
    default: __case_terr(__FILE__, __LINE__);
   }
   dcep = dcep2;
  }
}

/*
 * evaluate, set edge new and old and filter for dce change - non xmr case
 */
static int32 filter_edge_expr(register struct dce_expr_t *dcexp, word32 *oval,
 word32 *nval)
{
 register word32 nav, nbv;
 register struct xstk_t *xsp;
 word32 av, bv;

 /* evaluate expr. to get current edge in ref. context */
 xsp = __eval_xpr(dcexp->edgxp);
 /* extract low bit in case wide */
 nav = xsp->ap[0] & 1L;
 nbv = xsp->bp[0] & 1L;
 /* SJM 08/07/00 - now done with pushed expr value */
 __pop_xstk();

 *nval = nav | (nbv << 1);
 ld_scalval_(&av, &bv, dcexp->bp);
 *oval = av | (bv << 1);
 /* if variable does not effect expr. value, no edge */
 if (nval == oval) return(FALSE);
 st_scalval_(dcexp->bp, nav, nbv);
 return(TRUE);
}

/*
 * trigger an armed event control for current itree loc.
 * changes pending delay control event and thread resume event, and links in
 *
 * itree loc. must match and for xmr/col. is ref. itree loc.
 * know will not see if event delay control not active (armed)
 */
static void trigger_evctrl(struct delctrl_t *dctp, register i_tev_ndx tevpi)
{
 register struct tev_t *tevp;

 tevp = &(__tevtab[tevpi]);
 /* getting here means dctrl event triggered */
 /* DBG remove --- */
 if (__debug_flg && __st_tracing)
  {
   struct st_t *stp;

   if (tevp->tetyp == TE_NBPA)
    {
     stp = tevp->tu.tenbpa->nbastp;
     __tr_msg(
      "-- scheduling NB event control assign for now line %s (itree=%s)\n",
      __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt),
      __inst_ptr->itip->isym->synam);
    }
   else
    {
     stp = tevp->tu.tethrd->thnxtstp;
     __tr_msg(
      "-- scheduling event control resume for now line %s (chg in thd=%s, itree=%s)\n",
      __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt),
       tevp->tu.tethrd->th_itp->itip->isym->synam,
      __inst_ptr->itip->isym->synam);
    }
  }
 /* --- */

 /* must schedule wakeup since no way to interupt current context */
 /* which may not be procedural threads */
 tevp->etime = __simtime;
 /* armed event and now resume event already associated with thread */
 /* restart thread already set - must add to front for interactive */
 if (tevp->tetyp != TE_NBPA)
  {
   __add_ev_to_front(tevpi);
   dctp->dceschd_tevs[__inum] = -1;
   /* in case disable, indicate suspended on ev thrd no suspend to disable */
   tevp->tu.tethrd->th_dctp = NULL;
  }
 /* else add to #0 for non blocking assign */
 else
  {
   /* LOOKATME - is this right */
   /* here ok to have list of events (non inertial) */
   /* know at least 1 event, remove head schedule for now after trigger */
   dctp->dceschd_tevs[__inum] = tevp->tenxti;
   if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevpi;
   else { __tevtab[__p0_te_endi].tenxti = tevpi; __p0_te_endi = tevpi; }

   /* works because no new tevs that could cause realloc called */
   __tevtab[tevpi].tenxti = -1;
   /* this now looks like normal delay control nb */
   tevp->tu.tenbpa->nbdctp = NULL;
  }
}

/*
 * stren filter non monit dce for real change - return F if not changed
 *
 * needed because may assign to range but monitor bit in range did not chged
 * also if edge operator (not chaange) sets the oval and nval
 * dce put on target
 *
 * oneinst for cases where must load right inst of wire but only one inst
 * for dce
 *
 * this is for change operator not %v so value not strength change used
 */
static int32 stfilter_dce_chg(register struct net_t *np,
 register struct dcevnt_t *dcep, word32 *oval, word32 *nval, int32 oneinst)
{
 register int32 bi;
 register byte *dcesbp, *nsbp;
 int32 dcewid, i1, i2;
 byte dcev, nv;

 /* SJM 05/08/03 - stren can't be expr since eval of expr removes stren */
 /* DBG remove -- */
 if (dcep->dce_expr != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 dcewid = __get_dcewid(dcep, np);
 /* get dce value ptr (for iact per instance so from 0th) */
 if (oneinst) dcesbp = dcep->prevval.bp;
 else dcesbp = &(dcep->prevval.bp[__inum*dcewid]);

 __get_cor_range(dcep->dci1, dcep->dci2, &i1, &i2);
 /* point to wire value */
 get_stwire_addr_(nsbp, np);
 /* since LRM allows vector - this automatically accesses low bit */
 if (i1 != -1) nsbp = &(nsbp[i1]);

 /* only need to set values for edge if complicated need expr form */
 if (dcep->dce_edge)
  {
   if ((dcev = (dcesbp[0] & 3)) == (nv = (nsbp[0] & 3))) return(FALSE);
   *oval = dcev;
   *nval = nv;
   /* update the prevval for next wire change */
   dcesbp[0] = nsbp[0];
   return(TRUE);
  }

 /* all change operators here (%v handled elsewhere) are value only */
 for (bi = 0; bi < dcewid; bi++)
  { if ((dcesbp[bi] & 3) != (nsbp[bi] & 3)) goto not_same_val; }
 return(FALSE);

not_same_val:
 /* old value comes from internally stored prev. val, new is value of wire */
 /* copy from nbsp to dcesbp */
 memcpy(dcesbp, nsbp, dcewid);
 return(TRUE);
}

/*
 * vector stren and value filter for vpi_ stren only val chg call back
 * return F if not changed
 *
 * needed because may assign to range but monitor bit in range did not chged
 * also if edge operator (not chaange) sets the oval and nval
 * dce put on target
 *
 * 06/06/00 - SJM - new routine to filter vpi vec stren only chg
 */
static int32 vccb_vec_standval_filter(register struct net_t *np,
 register struct dcevnt_t *dcep, word32 *oval, word32 *nval, int32 oneinst)
{
 register byte *dcesbp, *nsbp;
 int32 dcewid, i1, i2;
 byte dcev, nv;

 /* SJM 05/08/03 - stren can't be expr since eval of expr removes stren */
 /* DBG remove -- */
 if (dcep->dce_expr != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 dcewid = __get_dcewid(dcep, np);
 /* get dce value ptr (for iact per instance so from 0th) */
 if (oneinst) dcesbp = dcep->prevval.bp;
 else dcesbp = &(dcep->prevval.bp[__inum*dcewid]);

 __get_cor_range(dcep->dci1, dcep->dci2, &i1, &i2);
 /* point to wire value */
 get_stwire_addr_(nsbp, np);
 /* since LRM allows vector - this automatically accesses low bit */
 if (i1 != -1) nsbp = &(nsbp[i1]);

 /* only need to set values for edge if complicated need expr form */
 /* for edges only low bit */
 /* LOOKATME - think edges can't happen here */
 if (dcep->dce_edge)
  {
   if ((dcev = dcesbp[0]) == (nv = nsbp[0])) return(FALSE);
   *oval = dcev & 3;
   *nval = nv & 3;
   /* update the prevval for next wire change */
   dcesbp[0] = nsbp[0];
   return(TRUE);
  }

 /* call back happens if only strength changes */
 if (memcmp((char *) nsbp, (char *) dcesbp, dcewid) == 0) return(FALSE);
 memcpy(dcesbp, nsbp, dcewid);
 return(TRUE);
}

/*
 * vpi_ scalar val chg stren and value filter (report stren only chges)
 * return F if not changed
 *
 * 06/06/00 - SJM - new routine to filter vpi scalar stren only chg
 */
static int32 vccb_scal_standval_filter(register struct net_t *np,
 register struct dcevnt_t *dcep, word32 *oval, word32 *nval, int32 oneinst)
{
 register byte *dcesbp, *nsbp;
 byte dcev, nv;

 /* SJM 05/08/03 - stren can't be expr since eval of expr removes stren */
 /* DBG remove -- */
 if (dcep->dce_expr != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 /* get dce value ptr (for iact per instance so from 0th) */
 if (oneinst) dcesbp = dcep->prevval.bp;
 else dcesbp = &(dcep->prevval.bp[__inum]);

 /* point to wire value */
 get_stwire_addr_(nsbp, np);
 /* if values same, no edge or no change */
 if ((dcev = dcesbp[0]) == (nv = nsbp[0])) return(FALSE);
 dcesbp[0] = nsbp[0];

 /* only need to set values for edge if complicated need expr form */
 /* but need value without strength for edge */
 if (dcep->dce_edge) { *oval = dcev & 3; *nval = nv & 3; }

 return(TRUE);
}

/*
 * filter non monit dce for actual change - return F if not changed
 * called from decl itree contextfor XMR
 *
 * needed because may assign to range but monitor bit in range did not chged
 * also if edge operator (not change) sets the oval and nval
 * know wire changed so "new" prevval is wire value
 */
static int32 filter_dce_chg(register struct net_t *np,
 register struct dcevnt_t *dcep, word32 *oval, word32 *nval, int32 oneinst)
{
 int32 dcewid, i1, i2, rv;
 register struct xstk_t *dcexsp, *nxsp;

 dcewid = __get_dcewid(dcep, np);
 /* load dce current value */
 push_xstk_(dcexsp, dcewid);
 /* if only one inst prev. val. is from 0th inst but wire from rigth inst */
 if (oneinst)
  {
   /* need some model so use current itree's although need inst 1 for eval */
   /* first move from inst about to be pushed on top of */
   __push_wrkitstk(__inst_mod, 0);
   __ld_perinst_val(dcexsp->ap, dcexsp->bp, dcep->prevval, dcewid);
   __pop_wrkitstk();
  }
 else __ld_perinst_val(dcexsp->ap, dcexsp->bp, dcep->prevval, dcewid);

 /* load wire value */
 push_xstk_(nxsp, dcewid);
 __get_cor_range(dcep->dci1, dcep->dci2, &i1, &i2);
 __ld_wire_sect(nxsp->ap, nxsp->bp, np, i1, i2);
 if (cmp_vval_(dcexsp->ap, nxsp->ap, dcewid) == 0 &&
  (cmp_vval_(dcexsp->bp, nxsp->bp, dcewid) == 0)) { rv = FALSE; goto done; }

 /* only need to set values for edge if complicated need expr form */
 if (dcep->dce_edge)
  {
   /* old value comes from internally stored preval, new is value of wire */
   *oval = (dcexsp->ap[0] & 1L) | ((dcexsp->bp[0] << 1) & 2L);
   *nval = (nxsp->ap[0] & 1L) | ((nxsp->bp[0] << 1) & 2L);
  }
 /* if one instance store into that 0th inst (only) loc. */
 if (oneinst)
  {
   /* need itree loc. - use current mod - save before pushing on top */
   __push_wrkitstk(__inst_mod, 0);
   __st_perinst_val(dcep->prevval, dcewid, nxsp->ap, nxsp->bp);
   __pop_wrkitstk();
  }
 else __st_perinst_val(dcep->prevval, dcewid, nxsp->ap, nxsp->bp);
 rv = TRUE;
done:
 __pop_xstk();
 __pop_xstk();
 return(rv);
}

/*
 * scalar stren filter non monit dce for real change
 * return F if not changed
 *
 * changes are always value only - monit %v strength handles in monit
 * as special case
 */
static int32 scal_stfilter_dce_chg(register struct net_t *np,
 register struct dcevnt_t *dcep, word32 *oval, word32 *nval, int32 oneinst)
{
 register byte *dcesbp, *nsbp;
 byte dcev, nv;

 /* get dce value ptr (for iact per instance so from 0th) */
 if (oneinst) dcesbp = dcep->prevval.bp;
 else dcesbp = &(dcep->prevval.bp[__inum]);

 /* point to wire value */
 get_stwire_addr_(nsbp, np);
 /* if values same, no edge or no change */
 if ((dcev = (dcesbp[0] & 3)) == (nv = (nsbp[0] & 3))) return(FALSE);
 dcesbp[0] = nsbp[0];

 /* only need to set values for edge if complicated need expr form */
 if (dcep->dce_edge) { *oval = dcev; *nval = nv; }

 /* LOOKATME - could fix %v strength only change on value here */
 /* if knew which dce was not %v and dcewid is number of bytes since stren */

 return(TRUE);
}

/*
 * TIMING CHECK SIMULATION ROUTINES
 */

/*
 * process some time change record net pin record
 * action depends on npp subtype
 * these are always 1 bit only (bsel, scalar or can be low bit of vector)
 */
extern void __process_npp_timofchg(struct net_t *np,
 register struct net_pin_t *npp)
{
 register int32 ii;
 register struct tchg_t *tchgp;
 int32 i1;
 word32 new_eval, old_eval, av, bv;
 word64 chgtim, reftim;
 struct tchk_t *tcp;
 struct chktchg_t *chktcp;
 struct itree_t *downitp;
 struct npaux_t *npauxp;
 struct spcpth_t *newpthp;

 /* notice because load bit of scalar works for entire wire get bit 0 */
 if ((npauxp = npp->npaux) == NULL) i1 = 0; else i1 = npauxp->nbi1;
 /* all but in module need this correction */
 downitp = NULL;
 if (npp->npproctyp != NP_PROC_INMOD)
  {
   /* know np is wire npp is on and one under itree stack is wire inst */
   /* current itree loc. here is loc. of ref. */
   downitp = __inst_ptr;
   __pop_itstk();
  }
 __ld_bit(&av, &bv, np, i1);
 new_eval = av | (bv << 1);
 if (downitp != NULL) __push_itstk(downitp);

 /* know inst ptr does not change in here */
 ii = __inum;
 switch (npp->chgsubtyp) {
  case NPCHG_TCSTART:
   /* notice reference event always recorded */
   /* $period does not have reference event net pin change element */
   tchgp = npp->elnpp.etchgp;
   tcp = tchgp->chgu.chgtcp;
   if (!filter_bitchange(new_eval, tchgp->oldval, tcp->startedge,
    tcp->startcondx)) break;
   tchgp->lastchg[ii] = __simtime;
   if (__debug_flg && __ev_tracing)
    {
     char s1[RECLEN], s2[RECLEN];

     if (npp->npproctyp != NP_PROC_INMOD) bld_xmrsrc_ref(s1, np);
     else sprintf(s1, "%s.%s", __msg2_blditree(__xs, __inst_ptr),
      np->nsym->synam);

     __tr_msg("## wire %s recording %s (line %s) reference event at %s\n", s1,
      __to_tcnam(__xs, tcp->tchktyp),
      __bld_lineloc(s2, tcp->tcsym->syfnam_ind, tcp->tcsym->sylin_cnt),
      __to_timstr(__xs2, &__simtime));

      bld_srcfilter_ref(s2, FALSE, tcp->startedge, tcp->startcondx);
      if (strcmp(s2, "") != 0) __tr_msg("   %s\n", s2);
    }
   break;
  case NPCHG_TCCHK:
   chktcp = npp->elnpp.echktchgp;
   tchgp = chktcp->startchgp;
   tcp = tchgp->chgu.chgtcp;
   /* this returns F if condition or edge does not match */
   if (!filter_bitchange(new_eval, chktcp->chkoldval, tcp->chkedge,
    tcp->chkcondx)) break;

   reftim = tchgp->lastchg[ii];
   /* filter out initialize changes - need real change for timing check */
   if (reftim == 0ULL)
    {
     /* first during run change for period recorded but no violation */
     if (tcp->tchktyp == TCHK_PERIOD) tchgp->lastchg[ii] = __simtime;
     break;
    }
   /* also if already on list for now, do not add again */
   chgtim = chktcp->chklastchg[ii];
   if (chgtim != __simtime)
    {
     add_tchk_chged(chktcp);
     /* possibly better to only record for "record before check" case */
     chktcp->chklastchg[ii] = __simtime;

     if (__debug_flg && __ev_tracing)
      {
       char s1[RECLEN], s2[RECLEN];

       if (npp->npproctyp != NP_PROC_INMOD) bld_xmrsrc_ref(s1, np);
       else sprintf(s1, "%s.%s", __msg2_blditree(__xs, __inst_ptr),
        np->nsym->synam);

       __tr_msg("## wire %s recording %s (line %s) data event at %s\n", s1,
        __to_tcnam(__xs, tcp->tchktyp),
        __bld_lineloc(s2, tcp->tcsym->syfnam_ind, tcp->tcsym->sylin_cnt),
        __to_timstr(__xs2, &__simtime));

        bld_srcfilter_ref(s2, FALSE, tcp->chkedge, tcp->chkcondx);
        if (strcmp(s2, "") != 0) __tr_msg("   %s\n", s2);
      }
     /* if repeated edge during same time - use 1st of this time as ref. */
    }
   /* SJM 10/10/04 - was wrongly setting ref even change time when repeated */
   /* check event changes during same time */
   break;
  case NPCHG_PTHSRC:
   tchgp = npp->elnpp.etchgp;
   newpthp = tchgp->chgu.chgpthp;
   /* special case code if no path edge or cond - also always record ifnone */
   /* because must be simple path */
   if (newpthp->pth_ifnone || (newpthp->pthedge == NOEDGE
    && newpthp->pthcondx == NULL))
    {
     ld_scalval_(&av, &bv, tchgp->oldval);
     old_eval = (av | (bv << 1));
     if (new_eval == old_eval) break;
     /* always save new value to old because value changed */
     st2_scalval_(tchgp->oldval, new_eval);
    }
   else
    {
     if (!filter_bitchange(new_eval, tchgp->oldval, newpthp->pthedge,
      newpthp->pthcondx)) break;
    }
   tchgp->lastchg[ii] = __simtime;
   if ((__debug_flg && __ev_tracing) || __pth_tracing)
    {
     char s1[RECLEN], s2[RECLEN];

     if (npp->npproctyp != NP_PROC_INMOD) bld_xmrsrc_ref(s1, np);
     else sprintf(s1, "%s.%s", __msg2_blditree(__xs, __inst_ptr),
      np->nsym->synam);

     /* notice cannot identify by delay since do not know old/new value */
     __tr_msg("## wire %s recording path (line %s) source change at %s\n", s1,
      __bld_lineloc(s2, newpthp->pthsym->syfnam_ind,
      newpthp->pthsym->sylin_cnt), __to_timstr(__xs, &__simtime));

     bld_srcfilter_ref(s2, newpthp->pth_ifnone, newpthp->pthedge,
      newpthp->pthcondx);
     if (strcmp(s2, "") != 0) __tr_msg("   %s\n", s2);
    }
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * build xmr source net instance reference
 */
static void bld_xmrsrc_ref(char *s, struct net_t *np)
{
 sprintf(s, "%s.%s (xmr from %s)",
  __msg2_blditree(__xs, __itstk[__itspi - 1]), np->nsym->synam,
  __msg2_blditree(__xs2, __inst_ptr));
}

/*
 * build path source filter (ifnone, or edge and/or condition)
 */
static void bld_srcfilter_ref(char *s, word32 pthifnone, word32 cedge,
 struct expr_t *cxp)
{
 int32 has_edge;
 struct xstk_t *xsp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 has_edge = FALSE;
 strcpy(s, "");
 if (pthifnone) strcpy(s, "CONDITION: ifnone)");
 else
  {
   if (cedge != NOEDGE)
    {
     sprintf(s1, "EDGE: %s", __to_edgenam(s2, cedge));
     strcpy(s, s1);
     has_edge = TRUE;
    }
   if (cxp != NULL)
    {
     xsp = __eval_xpr(cxp);
     sprintf(s1, "CONDITION: %s TRUE value %s", __msgexpr_tostr(s2, cxp),
      __regab_tostr(s3, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
     /* SJM 08/30/99 - for edge trace was not popping stack */
     __pop_xstk();
     if (has_edge) strcat(s, ", and ");
     strcat(s, s1);
    }
  }
}

/*
 * return T if bit changed (must pass all filters to change
 * because new edge value already computed (in up), need down itree here
 */
static int32 filter_bitchange(register word32 new_eval, register byte *oldbp,
 register word32 signat, struct expr_t *condx)
{
 register word32 old_eval;
 word32 av, bv;
 int32 epair;
 struct xstk_t *xsp;

 ld_scalval_(&av, &bv, oldbp);
 old_eval = (av | (bv << 1));

 /* first filter: this bit did not change */
 if (new_eval == old_eval) return(FALSE);
 /* always save new value to old */
 st2_scalval_(oldbp, new_eval);

 /* second filter if has edge - only change if matching edge */
 if (signat != 0)
  {
   /* build edge 4 bit index */
   epair = __epair_tab[new_eval | (old_eval << 2)];
   /* if any edge table bit is 1, then found edge */
   if ((signat & epair) == 0) return(FALSE);
  }
 /* third filter &&& cond or sdpd or edge sensitive path */
 if (condx != NULL)
  {
   /* only eliminate if evaluates to 0 - x/z condition match */
   xsp = __eval2_xpr(condx);
   /* timing check condition expressions must be 1 bit - use low */
   /* fastest to just always mask */
   /* LRM requires anything but explicit false (0) is T */
   /* for === operators never x/z (only 1 or 0) possible so always works */
   /* === illegal in SDPDs so never a problem */
   /* for nondeterministic x or z is always T on paths or tchks */

   if ((xsp->ap[0] & 1L) == 0L && (xsp->bp[0] & 1L) == 0L)
    { __pop_xstk(); return(FALSE); }
   __pop_xstk();
  }
 return(TRUE);
}

/*
 * add a timing check to end of now data change routines
 * for processing at end of time slot (required by semantics)
 */
static void add_tchk_chged(struct chktchg_t *chkchgp)
{
 struct tc_pendlst_t *tcpendp;

 /* get a list element from somewhere */
 if (__tcpendfreelst == NULL)
  tcpendp = (struct tc_pendlst_t *) __my_malloc(sizeof(struct tc_pendlst_t));
 else
  {
   tcpendp = __tcpendfreelst;
   __tcpendfreelst = __tcpendfreelst->tc_plnxt;
  }
 tcpendp->tc_chkchgp = chkchgp;
 tcpendp->tc_itp = __inst_ptr;
 tcpendp->tc_plnxt = NULL;

 /* link on end since need batch movement of all to free list */
 if (__tcpendlst_end != NULL)
  { __tcpendlst_end->tc_plnxt = tcpendp; __tcpendlst_end = tcpendp; }
 else
  {
   __tcpendlst_hdr = __tcpendlst_end = tcpendp;
   __slotend_action |= SE_TCHK_VIOLATION;
  }
}

/*
 * routine to process all timing check violations at end of time slot
 * LOOKATME - this is maybe wrong because of recording of data event order?
 */
static void process_all_tchk_violations(void)
{
 register struct tc_pendlst_t *tcpendp;
 word64 diff, lim1, lim2, reftim;
 struct tchk_t *tcp, *tcp2;
 struct chktchg_t *chktcp;
 struct tchg_t *chgp;
 char s1[RECLEN], s2[RECLEN];

 for (tcpendp = __tcpendlst_hdr; tcpendp != NULL; tcpendp = tcpendp->tc_plnxt)
  {
   __push_itstk(tcpendp->tc_itp);
   chktcp = tcpendp->tc_chkchgp;
   chgp = chktcp->startchgp;
   tcp = chgp->chgu.chgtcp;
   reftim = chgp->lastchg[__inum];
   diff = __simtime - reftim;
   /* notice no edge globals set but error before here if not simple delay */
   if (!tcp->tc_supofsuphld && !tcp->tc_recofrecrem)
    __get_del(&lim1, tcp->tclim_du, tcp->tc_delrep);
   lim2 = 0ULL;

   switch ((byte) tcp->tchktyp) {
    case TCHK_SETUP:
     if (tcp->tc_supofsuphld)
      {
       /* added setup of setup hold needs limit from 1st lim of setuphold */
       /* because must be able to change both delays during sim */
       tcp2 = (struct tchk_t *) tcp->tclim_du.pdels;
       __get_del(&lim1, tcp2->tclim_du, tcp2->tc_delrep);
      }
     /* for setup if simultaneous change no violation */
     /* setup of setuphold also here with reversed for setup conns/lim1 */
     if (diff >= lim1 || diff == 0ULL) break;

emit_msg:
     bld_tchk_srcdump(__xs, tcp, &reftim, &__simtime, &lim1, &lim2);
     __gfwarn(566, tcp->tcsym->syfnam_ind, tcp->tcsym->sylin_cnt,
      "timing violation in %s (diff. %s)\n %s",
      __msg2_blditree(s1, __inst_ptr), __to_timstr(s2, &diff), __xs);
     /* toggle notify reg if present */
     if (tcp->ntfy_np != NULL) process_notify(tcp->ntfy_np);
     if (__have_vpi_actions) __vpi_tchkerr_trycall(tcp, __inst_ptr);
     break;
    case TCHK_SETUPHOLD:
     /* this is hold part of setup hold */
     __get_del(&lim2, tcp->tclim2_du, tcp->tc_delrep2);
     /* 2nd limit is hold part */
     lim1 = lim2;
     /* AIV 09/15/04 - difference of 0 must not cause check - see hold */
     if (diff < lim1 && diff != 0ULL) goto emit_msg;
     break;
    case TCHK_HOLD:
     /* AIV 09/15/04 - difference of 0 must not cause check */
     if (diff < lim1 && diff != 0ULL) goto emit_msg;
     break;
    case TCHK_WIDTH:
     /* opposite edge data event less than limit after 1st edge ref., err */
     /* if time less than threshold, ignore very narrow pulses */
     /* if no threshold, will be set to 0, if same as threshold still err */
     if (tcp->tc_haslim2) __get_del(&lim2, tcp->tclim2_du, tcp->tc_delrep2);
     else lim2 = 0ULL;

     /* ignore pulse width (ok) if less than lim2 (threshold) */
     /* ? think LRM says if occur simultaneously no change (threshhold 0) */
     if (diff < lim1 && diff >= lim2) goto emit_msg;
     break;
    case TCHK_PERIOD:
     /* same edge data event less than limit after 1st edge ref. */
     /* period error if clock period too narrow, no threshold */
     /* no separate reference event */
     /* must set reference event to now even if no violation */
     chgp->lastchg[__inum] = __simtime;

     /* notice if edge repeated in time slot, it is timing violation */
     if (diff < lim1) goto emit_msg;
     break;
    case TCHK_SKEW:
     /* if data event (2nd) more than limit after ref. 1st => err */
     /* skew error if date event too long after reference event */
     /* i.e. too much skew (lateness) of arriving signal */
     /* SJM 04/13/04 - if backward should be opposite of PERIOD above */
     if (diff > lim1) goto emit_msg;
     break;
    case TCHK_RECREM:
     /* this is removal part of recrem */
     __get_del(&lim2, tcp->tclim2_du, tcp->tc_delrep2);
     /* 2nd limit is hold part */
     lim1 = lim2;
     if (diff < lim1 && diff != 0ULL) goto emit_msg;
     break;
    case TCHK_RECOVERY:
     /* SJM 01/16/04 - terminals reversed for rec part of recrem */
     if (tcp->tc_recofrecrem)
      {
       /* added setup of setup hold needs limit from 1st lim of setuphold */
       /* because must be able to change both delays during sim */
       tcp2 = (struct tchk_t *) tcp->tclim_du.pdels;
       __get_del(&lim1, tcp2->tclim_du, tcp2->tc_delrep);
      }

     /* if data event (2nd clock) occurs too soon after 1st (clear edge) */
     /* recovery like hold but error if reference event not edge */
     /* recovery err if clock happens too soon after clear or preset edge */
     if (diff < lim1 && diff != 0ULL) goto emit_msg;
     break;
    case TCHK_REMOVAL:
     /* if data event (2nd clock) does not occur soon enough after 1st */
     /* (clear edge) - removal like setup but error if reference event not */
     /* edge - removal err if clock happens too soon after clear or preset */
     /* edge */
     /* AIV 07/09/04 - removal test was reversed - was > but needs to be < */
     if (diff < lim1 && diff != 0ULL) goto emit_msg;
     break;
    default: __case_terr(__FILE__, __LINE__);
   }
   __pop_itstk();
  }
 /* must move all processed to front of free list */
 __tcpendlst_end->tc_plnxt = __tcpendfreelst;
 __tcpendfreelst = __tcpendlst_hdr;
 __tcpendlst_hdr = __tcpendlst_end = NULL;
}

/* LOOKATME - to match OVISIM x goes to 1 not 0 */
static word32 ntfy_toggle_tab[] = {1, 0, 2, 1};

/*
 * process notify
 */
static void process_notify(struct net_t *np)
{
 struct xstk_t *xsp;
 word32 val;

 push_xstk_(xsp, np->nwid);
 __ld_wire_val(xsp->ap, xsp->bp, np);
 /* DBG remove */
 if (xsp->xslen != 1) __misc_terr(__FILE__, __LINE__);
 /* --- */
 val = xsp->ap[0] | (xsp->bp[0] << 1);
 val = ntfy_toggle_tab[val];
 xsp->ap[0] = val & 1L;
 xsp->bp[0] = (val >> 1) & 1L;
 __chg_st_val(np, xsp->ap, xsp->bp);
 __pop_xstk();
}

/*
 * build a timing check source dump string with constants
 *
 * notice for setuphold (hold part) lim1 is correct lim2
 */
static char *bld_tchk_srcdump(char *s, struct tchk_t *tcp, word64 *tim1,
 word64 *tim2, word64 *lim1, word64 *lim2)
{
 int32 nd_rpar;
 char s1[RECLEN];

 __cur_sofs = 0;
 /* indicate whether setup or hold */
 if (tcp->tchktyp == TCHK_SETUPHOLD) __adds("hold(of setuphold)");
 else if (tcp->tc_supofsuphld) __adds("setup(of setuphold)");
 else if (tcp->tchktyp == TCHK_RECREM) __adds("removal(of recrem)");
 else if (tcp->tc_recofrecrem) __adds("recovery(of recrem)");
 else __adds(__to_tcnam(s1, tcp->tchktyp));

 __adds("(");
 if (tcp->startedge != NOEDGE || tcp->startcondx != NULL)
  { addch_('('); nd_rpar = TRUE; }
 else nd_rpar = FALSE;
 if (tcp->startedge != NOEDGE)
  { __adds(__to_edgenam(s1, tcp->startedge)); addch_(' '); }
 __adds(__msgexpr_tostr(s1, tcp->startxp));
 if (tcp->startcondx != NULL)
  { __adds(" &&& "); __adds(__msgexpr_tostr(s1, tcp->startcondx));  }
 if (nd_rpar) addch_(')');
 addch_(':');
 __adds(__to_timstr(s1, tim1));

 __adds(", ");
 if (tcp->chkedge != NOEDGE || tcp->chkcondx != NULL)
  { addch_('('); nd_rpar = TRUE; }
 else nd_rpar = FALSE;
 if (tcp->chkedge != NOEDGE)
  { __adds(__to_edgenam(s1, tcp->chkedge)); addch_(' '); }
 __adds(__msgexpr_tostr(s1, tcp->chkxp));
 if (tcp->chkcondx != NULL)
  { __adds(" &&& "); __adds(__msgexpr_tostr(s1, tcp->chkcondx));  }
 if (nd_rpar) addch_(')');
 addch_(':');
 __adds(__to_timstr(s1, tim2));

 __adds(", ");
 __adds(__to_timstr(s1, lim1));

 if ((tcp->tchktyp == TCHK_WIDTH || tcp->tchktyp == TCHK_PERIOD)
  && *lim2 != 0ULL)
  { __adds(", "); __adds(__to_timstr(s1, lim2)); }
 __adds(");");
 __trunc_exprline(MSGTRUNCLEN, FALSE);
 strcpy(s, __exprline);
 __cur_sofs = 0;
 return(s);
}

/*
 * ROUTINES TO INITIALIZE BEFORE START OF SIMULATION
 * TRICKY BEGINNING OF TIME 0 PROPOGATION HERE
 */

/*
 * initialization just before simulation start
 * need a dummy thread for functions on rhs of contas
 *
 * notice can execute statements from called conta rhs functions in here
 */
extern void __init_sim(void)
{
 char *sav_fnam;

 /* this is never called for resets, so initialize to no resets */
 __reset_count = 0;
 /* just set this to some value - task exec. always sets again */
 __reset_value = 0;

 init_stime();
 sav_fnam = __in_fils[0];
 __in_fils[0] = __pv_stralloc("**initialize none**");

 /* do not emit new message at time 0 since initialize time */
 __last_trtime = 0ULL;
 __last_evtrtime = 0ULL;

 __suspended_thd = NULL;
 __suspended_itp = NULL;
 __cur_thd = NULL;
 /* current inst. stack needs nil on bottom for debugging and must be empty */
 /* DBG remove -- */
 if (__itspi != -1) __misc_terr(__FILE__, __LINE__);
 if (__inst_ptr != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 /* must leave instance stack exactly as is - cannot initialize */
 init_wires();
 __pv_stlevel = 0;

 /* initialize dumpvars state */
 __dv_calltime = 0ULL;
 __dv_seen = FALSE;
 __dv_state = DVST_NOTSETUP;
 __dv_dumplimit_size = 0;
 __dv_chgnethdr = NULL;
 __dv_netfreelst = NULL;
 __dv_hdr = __dv_end = NULL;
 __dv_isall_form = FALSE;

 __in_fils[0] = sav_fnam;
 /* debugger source files go through last library file */
 __last_srcf = __last_lbf;
 /* putting any $input files on end since last_inf only needed for $input */
 /* from now on */
 /* resetting does not effect this */
 __last_inf = __last_lbf;

 /* last step is to setup interactive environment */
 /* needed since interactive setup stuff can be in source */
 __init_interactive();
 if (__slotend_action != 0) __misc_terr(__FILE__, __LINE__);
 __slotend_action = 0;
 __run_state = SS_SIM;
}

/*
 * initial simulation time and variables
 */
static void init_stime(void)
{
 register int32 i;
 struct telhdr_t *telp;

 /* initialize the simulation realloced event table */
 /* because fibronacci growth start with small value */

 /* notice for now fixed size timing wheel */
 /* initialize timing wheel headers to no events but need place holder */
 /* allocate timing wheel - allow variable sizing - should adjust from ts */
 __twhsize = TWHINITSIZE;

 /* need to allocate extra fence */
 __twheel = (struct telhdr_t **)
  __my_malloc((__twhsize + 1) *sizeof(struct telhdr_t *));

 /* include extra fence in loop */
 for (i = 0; i <= __twhsize; i++)
  {
   __twheel[i] = telp = (struct telhdr_t *)
    __my_malloc(sizeof(struct telhdr_t));
   telp->te_hdri = telp->te_endi = -1;
   telp->num_events = 0;
  }
 /* this is fence */
 __twheel[__twhsize]->num_events = -1;
 /* just need some good value here */
 __twheel[__twhsize]->te_hdri = -1;

 __simtime = 0ULL;
 __cur_te_hdri = __cur_te_endi = -1;
 __p0_te_hdri = __p0_te_endi = -1;
 /* SJM 07/05/05 - also initialize non block current time after pnd0 queue */
 __nb_te_hdri = __nb_te_endi = -1;

 __tedpfreelst = NULL;
 __teputvfreelst = NULL;
 /* init overflow q */
 __topi = 0;
 __btqroot = NULL;

 /* whetime is end of wheel time */
 /* works because __twsize never bigger than 2*31 */
 __whetime = (word64) (__twhsize - 1);
 __num_twhevents = 0;
 __num_ovflqevents = 0;
 __num_proc_tevents = __num_cancel_tevents = 0;
 __inertial_cancels = __newval_rescheds = 0;
 __proc_thrd_tevents = 0;
 __num_netchges = 0;
 __num_switch_vtxs_processed = 0;
 __immed_assigns = 0;
 __strobe_hdr = __strobe_end = __strb_freelst = NULL;
 __monit_active = TRUE;
 __monit_dcehdr = NULL;
 __fmon_hdr = __fmon_end = NULL;
 __fmonse_hdr = __fmonse_end = __fmse_freelst = NULL;
 __nchg_futend = __nchg_futhdr = __nchgfreelst = NULL;
 /* SJM 08/16/03 - now need to start with lhs changed off */
 __lhs_changed = FALSE;
 __tcpendlst_end = __tcpendlst_hdr = __tcpendfreelst = NULL;
 __dltevfreelst = NULL;
 __cur_thd = NULL;
 /* tf one way pending event free list */
 __ltevfreelst = NULL;
 __wrkevtab = NULL;
 __last_wevti = -1;
 __size_wrkevtab = 0;
}

/*
 * after debugger :rerun command - do initialization
 */
extern void __reinit_sim(void)
{
 char *sav_fnam;

 /* another reset - needed first in case init code uses $reset_count */
 __reset_count++;

 /* for catching problems with incorrectly reset dces */
 /* DBG remove ---
 chk_schd_dces();
 --- */

 reinit_stime();

 sav_fnam = __in_fils[0];
 __in_fils[0] = __pv_stralloc("**initialize none**");

 /* no time 0 messages */
 __last_trtime = 0ULL;
 __last_evtrtime = 0ULL;

 /* current inst. stack needs nil on bottom for debugging */
 __cur_thd = NULL;
 __suspended_thd = NULL;
 __suspended_itp = NULL;
 /* must empty stack since may have been called from running code */
 __itspi = -1;
 __itstk[0] = NULL;
 __inst_ptr = NULL;
 __inst_mod = NULL;

 /* must leave instance stack exactly as is - cannot initialize */
 init_wires();
 __pv_stlevel = 0;

 /* reinitialize dumpvars state */
 __dv_calltime = 0ULL;
 __dv_seen = FALSE;
 __dv_state = DVST_NOTSETUP;
 __dv_dumplimit_size = 0;
 __dv_chgnethdr = NULL;
 __dv_hdr = __dv_end = NULL;
 __dv_isall_form = FALSE;

 /* free and maybe close open command file - for reset will use start cmd_s */
 if (__cmd_s != NULL)
  {
   __my_fclose(__cmd_s);
   __cmd_s = NULL;
   __my_free(__cmd_fnam, strlen(__cmd_fnam) + 1);
   __cmd_fnam = NULL;
  }

 __in_fils[0] = sav_fnam;

 /* interactive environment must be left as is */
 __slotend_action = 0;

 /* things initialized in pv that need to be re-initialized */
 __next_dvnum = 0;
 /* reset interactive run state */
 __pending_enter_iact = FALSE;
 __iact_reason = IAER_UNKN;
 /* this will cause any -i file to be read on first iact entry */
 __ia_entered = FALSE;
 __cur_sofs = 0;
 __xspi = __itspi = __fcspi = -1;
 __inst_ptr = NULL;
 /* reinit $scope must start at first top level module */
 __scope_ptr = __it_roots[0];
 __scope_tskp = NULL;
 __run_state = SS_SIM;
}

/*
 * reinitial simulation time and variables
 */
static void reinit_stime(void)
{
 __simtime = 0ULL;
 __cur_te_hdri = __cur_te_endi = -1;
 __p0_te_hdri = __p0_te_endi = -1;
 /* SJM 07/05/05 - also initialize non block current time after pnd0 queue */
 __nb_te_hdri = __nb_te_endi = -1;

 /* works because __twsize never bigger than 2*31 */
 __whetime = (word64) (__twhsize - 1);
 __num_twhevents = 0;
 __num_ovflqevents = 0;
 __num_proc_tevents = __num_cancel_tevents = 0;
 __inertial_cancels = __newval_rescheds = 0;
 __proc_thrd_tevents = 0;
 __num_netchges = 0;
 __immed_assigns = 0;
 __strobe_hdr = __strobe_end = NULL;
 __monit_active = TRUE;
 __monit_dcehdr = NULL;
 __fmon_hdr = __fmon_end = NULL;
 /* here leave the free list to reuse storage from there */
 __fmonse_hdr = __fmonse_end = NULL;

 /* SJM 08/16/03 - now need to start with lhs changed off */
 __lhs_changed = FALSE;

 /* notice must leave free lists - will begin by allocating from there */
 __nchg_futend = __nchg_futhdr = NULL;
 __tcpendlst_end = __tcpendlst_hdr = NULL;
 __cur_thd = NULL;

 /* must leave tevtab timing queue - free added o free list */
 /* num used stays same */
}

/*
 * initialize by scheduling a wire change for every wire driver
 * needs empty timing wheel header for delay 0 scheduling
 *
 * this needs to be run with all wire delays disabled
 * no PLI interaction here and run in SS_RESET or SS_LOAD run state
 */
static void init_wires(void)
{
 int32 ii;

 __wire_init = TRUE;
 /* go through list of 1 inst. corresponding to each top level module */
 /* ignore all delays in propagating from lhs's to rhs's */
 __nchg_futend = __nchg_futhdr = NULL;
 __initalw_thrd_hdr = NULL;

 /* SJM - 05/24/00 - must not process var changes until 0 normal #0 pt. */
 /* if (__nchg_futhdr != NULL) process_all_netchgs(); */

 for (ii = 0; ii < __numtopm; ii++)
  {
   init_itinsts(__it_roots[ii]);
   /* even though top modules can be linked by xmrs, do here can at worst */
   /* cause a few extra events to be processed from xmrs */
   /* SJM - 05/24/00 - must not process var changes until 0 normal #0 pt. */
   /* if (__nchg_futhdr != NULL) process_all_netchgs(); */
  }

 /* SJM 04/11/01 - initializing tran channels after drivers propagated */
 /* hard drivers as possible have changed */
 __init_all_trchans();

 __wire_init = FALSE;
 if (__ev_tracing)
  __tr_msg("\n>>>> wire initialization complete <<<<<\n");
}

/*
 * initialize all wires and threads in one itree instance
 * know that when storage for all wires allocated, also initialized
 *
 * algorithm is to evaluate every cont. assign (including cross module ports)
 * and gate/udp and schedule wire change at some point
 * this adds all no delay to netchg list and all delays are scheduledo
 * with pnd0's going on pnd0 list
 *
 * also allocate and schedules all behavioral initial/always threads
 * notice behavior force/assign can only occur during time 0 or later
 */
static void init_itinsts(struct itree_t *up_itp)
{
 register int32 gi, pbi;
 int32 cai, ii;
 struct conta_t *cap;
 struct mod_t *mdp;
 struct itree_t *itp;

 if (__debug_flg)
  {
   __dbg_msg("== initializing wires in %s\n", __msg2_blditree(__xs, up_itp));
  }
 /* on entry know unprocessed net change list empty */
 mdp = up_itp->itip->imsym->el.emdp;
 __push_itstk(up_itp);
 /* schedule all behavioral threads for this instance */
 init_sched_thd(mdp);

 /* evaluate and schedule all gates in instance */
 for (gi = 0; gi < mdp->mgnum; gi++) gate_initeval(&(mdp->mgates[gi]));

 /* and all contas */
 for (cap = mdp->mcas, cai = 0; cai < mdp->mcanum; cai++, cap++)
  {
   /* SJM 09/28/02 - need to initialize the PB separated contas */
   if (cap->ca_pb_sim)
    {
     /* SJM 08/08/03 - for per bit sim form, need 2nd arg master conta */
     for (pbi = 0; pbi < cap->lhsx->szu.xclen; pbi++)
      { conta_initeval(&(cap->pbcau.pbcaps[pbi]), cap); }
    }
   else conta_initeval(cap, cap);
  }
 __pop_itstk();

 for (ii = 0; ii < mdp->minum; ii++)
  {
   itp = &(up_itp->in_its[ii]);
   /* force downward cross port continuous assign for inputs and inouts */
   __init_instdownport_contas(up_itp, itp);

   /* process depth first down one instance */
   init_itinsts(itp);

   /* force upward from down (after its processed) output/inout contas */
   __init_instupport_contas(itp);
  }
}

/*
 * build thread and schedule time 0 event for each initial always block
 * just goes into time 0 current time event list as if time move from -1
 *
 * if initial/always statement is named block will have unnamed block
 * put around it by here
 *
 * this requires cur. itp to be set to current place in itree
 */
static void init_sched_thd(struct mod_t *mdp)
{
 register struct ialst_t *ialp;
 struct thread_t *thp;
 i_tev_ndx tevpi;
 struct telhdr_t *tw0;
 struct st_t *stp, *stp2;

 /* each element in ia sts list is a possibly added unnamed begin block */
 /* each separate intial/always must be its own thread */
 /* because one blocking does not block others */
 for (ialp = mdp->ialst; ialp != NULL; ialp = ialp->ialnxt)
  {
   /* build the initial/always thread */
   /* build the event and allocate assoc. thread */
   alloc_tev_(tevpi, TE_THRD, __inst_ptr, __tim_zero);
   stp = ialp->iastp;

   thp = __alloc_thrd();
   thp->th_ialw = TRUE;
   __cur_thd = thp;
   thp->thenbl_sfnam_ind = ialp->ia_first_ifi;
   thp->thenbl_slin_cnt = ialp->ia_first_lini;
   /* caller must alloc any event type specific fields, cannot assume NULL */
   __tevtab[tevpi].tu.tethrd = thp;
   thp->thnxtstp = stp;
   thp->thpar = NULL;
   thp->th_itp = __inst_ptr;
   /* link on sequential list for rerun freeing */
   if (__initalw_thrd_hdr == NULL) __initalw_thrd_hdr = thp;
   else
    {
     thp->thright = __initalw_thrd_hdr;
     __initalw_thrd_hdr->thleft = thp;
     __initalw_thrd_hdr = thp;
    }

   /* this just causes all initial and always 1st statements to happen */
   /* at time 0 - know thnxtstp is just list of statements */
   if (__ev_tracing)
    {
     stp2 = __tevtab[tevpi].tu.tethrd->thnxtstp;
     if (stp2 == NULL)
      {
       __tr_msg(
        "-- adding initial machine code thread for init/always at %s\n",
        __bld_lineloc(__xs, ialp->ia_first_ifi, ialp->ia_first_lini));
      }
     else
      {
       __tr_msg("-- adding initial procedural start at statement %s\n",
        __bld_lineloc(__xs, stp2->stfnam_ind, stp2->stlin_cnt));
      }
    }

   tw0 = __twheel[0];
   if (tw0->te_hdri == -1) tw0->te_hdri = tw0->te_endi = tevpi;
   else { __tevtab[tw0->te_endi].tenxti = tevpi; tw0->te_endi = tevpi; }
   tw0->num_events += 1;
   __num_twhevents++;
   __cur_thd = NULL;
  }
}

/*
 * initialize gate by evaluating all inputs, changing wire if needed,
 * and if wire changed, propagate changes
 * called once for every gate in itree
 */
static void gate_initeval(struct gate_t *gp)
{
 int32 i, gid;

 /* evaluate gate - even if no change assign (this stores state) */
 /* if input value same, nothing to do */
 switch ((byte) gp->g_class) {
  case GC_LOGIC: init_logic_gate(gp); break;
  case GC_UDP: init_udp(gp); break;
  case GC_BUFIF: init_bufif_gate(gp); break;
  case GC_MOS:
   chg_mos_instate(gp, 1);
   chg_mos_instate(gp, 2);

   gid = gp->gmsym->el.eprimp->gateid;
   /* note here input change routine and eval separate */
   /* eval always evals even if new and old input are the same */
   if (gid == G_NMOS) __eval_nmos_gate(gp->gstate.wp[__inum]);
   else if (gid == G_RNMOS) __eval_rnmos_gate(gp->gstate.wp[__inum]);
   else if (gid == G_PMOS) __eval_pmos_gate(gp->gstate.wp[__inum]);
   else if (gid == G_RPMOS) __eval_rpmos_gate(gp->gstate.wp[__inum]);
   else __case_terr(__FILE__, __LINE__);
   break;
  case GC_CMOS:
   chg_mos_instate(gp, 1);
   chg_mos_instate(gp, 2);
   chg_mos_instate(gp, 3);
   /* note here input change routine and eval separate */
   /* eval always evals even if new and old input are the same */
   __eval_cmos_gate(gp);
   break;
  case GC_PULL:
   /* each port is pull wire */
   /* this is needed since probably all drivers tristate at time 0 */
   /* but pull must start at time 0 */
   /* notice no output here so starts at 0 not 1 (normally 0 output) */
   for (i = 0; i < (int32) gp->gpnum; i++)
    __mdr_assign_or_sched(gp->gpins[i]);
   if (__debug_flg)
    {
     __dbg_msg("-- all connections of pull %s evalutated for initialization\n",
     to_evtronam(__xs, gp->gsym->synam, __inst_ptr,
      (struct task_t *) NULL));
    }
   return;
  case GC_TRANIF: init_tranif_gate(gp); return;
  /* nothing to do for trans */
  case GC_TRAN: return;
  default: __case_terr(__FILE__, __LINE__);
 }

 if (__debug_flg && __ev_tracing)
  {
   char s1[RECLEN];

   if (gp->g_class == GC_UDP) strcpy(s1, "udp"); else strcpy(s1, "gate");
   /* notice during wire init all wire delays off */
   __tr_msg("-- %s %s %s assign initialized to state:\n",
    gp->gmsym->synam, s1, to_evtronam(__xs, gp->gsym->synam, __inst_ptr,
     (struct task_t *) NULL));
   __tr_msg("   %s\n", __gstate_tostr(__xs, gp, FALSE));
  }
 /* must always immediately assign to wire */
 change_gate_outwire(gp);
}

/*
 * initialize udps
 * 1) evaluate all udp input exprs in case constant (update wide signature)
 * 2) if has initial value, set new gate value to initial and do not eval
 * 3) if no initial value, force evaluate to get new gate value
 * for sequential use combinatorial table
 *
 * for wide this updates signature to x that is then adjusted by
 * storing gate output that always happens during init.
 *
 * notice that for gpins, 0 is output and first input is 1
 * but values in upd state word32 are input 0 in low 2 bits, and high (maybe
 * output if sequential) in high 2 bits
 */
static void init_udp(struct gate_t *gp)
{
 register int32 i;
 int32 nins;
 hword *hwp;
 word32 wide_ival, new_inputval, *wp;
 int32 out_chg;
 struct xstk_t *xsp;
 extern word32 __to_noztab[];
 extern word32 __to_uvaltab[];

 __cur_udp = gp->gmsym->el.eudpp;
 /* for level, this includes state */
 nins = __cur_udp->numins;
 if (!__cur_udp->u_wide)
  {
   /* here can ingore old state (if present) since just over-written */
   hwp = &(gp->gstate.hwp[__inum]);
   /* eval and store all inputs in case constant expr. */
   for (i = 0; i < nins; i++)
    {
     xsp = __eval_xpr(gp->gpins[i + 1]);
     new_inputval = __to_noztab[(xsp->ap[0] & 1L) | ((xsp->bp[0] & 1L) << 1)];
     __pop_xstk();
     /* know z will always cause new input value */
     /* change the input */
     hwp[0] &= ~(3L << (2*i));
     hwp[0] |= ((hword) new_inputval << (2*i));
     /* -- RELEASE remove ---
     if (__debug_flg)
      __dbg_msg("-- udp init after %dth input hwp=%lx\n", i, hwp[0]);
     -- */
    }
   /* -- RELEASE remove --
   if (__debug_flg)
    __dbg_msg("-- narrow before init eval: hwp=%lx\n", hwp[0]);
   -- */
  }
 else
  {
   /* in wide, case need 2nd running value index word32 */
   wp = &(gp->gstate.wp[2*__inum]);
   for (i = 0; i < nins; i++)
    {
     /* remove signature contribution from initialized value */
     wide_ival = __to_uvaltab[((wp[0] >> (2*i)) & 3L)];
     wp[1] -= wide_ival*__pow3tab[i];

     xsp = __eval_xpr(gp->gpins[i + 1]);
     /* think evaluate can be wide thing that must be truncated */
     new_inputval = __to_noztab[(xsp->ap[0] & 1L) | ((xsp->bp[0] & 1L) << 1)];
     __pop_xstk();
     wp[0] &= ~(3L << (2*i));
     wp[0] |= (new_inputval << (2*i));

     /* add in new input signature value */
     wide_ival = __to_uvaltab[new_inputval];
     wp[1] += wide_ival*__pow3tab[i];
    }
   /* -- RELEASE remove ---
   if (__debug_flg)
     __dbg_msg("-- wide before init eval: w0=%lx, w1=%lu\n", wp[0], wp[1]);
   -- */
  }
 /* know combinatorial will never have initial value */
 if (__cur_udp->ival == NO_VAL)
  {
   /* change old input 0 to force evaluation - new replace set wrong */
   if (!__cur_udp->u_wide)
    {
     hwp = &(gp->gstate.hwp[__inum]);
     new_inputval = (word32) (hwp[0] & 3L);
     if (new_inputval == 0) new_inputval = 3; else new_inputval = 0;
     hwp[0] &= ~(3L);
     hwp[0] |= (hword) new_inputval;
    }
   else
    {
     wp = &(gp->gstate.wp[2*__inum]);
     new_inputval = wp[0] & 3L;
     /* subtract out old signature contribution */
     wide_ival = __to_uvaltab[new_inputval];
     wp[1] -= wide_ival*__pow3tab[0];

     if (new_inputval == 0) new_inputval = 2; else new_inputval = 0;
     wp[0] &= ~(3L);
     wp[0] |= new_inputval;

     /* add in new input signature value */
     wide_ival = __to_uvaltab[new_inputval];
     wp[1] += wide_ival*__pow3tab[0];
    }
   /* this sets new gate value */
   __eval_udp(gp, 1, &out_chg, FALSE);
  }
 else __new_gateval = __cur_udp->ival;
 /* caller will store or schedule store into output connection */
}

/*
 * initialize logic gate by evaluating all inputs then forcing eval of
 * logic gate - not for buf and mos types gates
 */
static void init_logic_gate(struct gate_t *gp)
{
 register int32 i;
 int32 srep, nins;
 int32 out_chg;
 struct xstk_t *xsp;

 if (gp->gpnum > 16) srep = SR_VEC; else srep = SR_PVEC;

 /* tricky part must - make sure input 0 does not eval the same */
 /* simply invert b bit of input 0 result */
 nins = gp->gpnum - 1;
 for (i = 0; i < nins; i++)
  {
   xsp = __eval_xpr(gp->gpins[i + 1]);
   xsp->ap[0] &= 1L;
   xsp->bp[0] &= 1L;
   if (i == 0) xsp->bp[0] = (~xsp->bp[0]) & 1L;
   gate_st_bit(gp->gstate, (int32) gp->gpnum, i, srep, xsp->ap[0], xsp->bp[0]);
   __pop_xstk();
  }
 /* know reevaluating 1st input will result in new value so will eval. */
 __eval_logic_gate(gp, 1, &out_chg);
 /* wire assign in caller */
}

/*
 * initialize a bufif style gate
 * evaluate both inputs and change gate state for data to opposite
 *
 * storage: low 2 bits data in, next 2 control in - next 8 out strength
 * stored as half word
 */
static void init_bufif_gate(struct gate_t *gp)
{
 int32 out_chg;
 hword *hwp;
 struct xstk_t *xsp;

 hwp = &(gp->gstate.hwp[__inum]);
 xsp = __eval_xpr(gp->gpins[1]);
 xsp->ap[0] &= 1L;
 xsp->bp[0] &= 1L;
 /* invert to force eval */
 xsp->bp[0] = (~xsp->bp[0]) & 1L;
 hwp[0] &= ~3L;
 hwp[0] |= ((hword) (xsp->ap[0] | (xsp->bp[0] << 1)));
 __pop_xstk();

 xsp = __eval_xpr(gp->gpins[2]);
 xsp->ap[0] &= 1L;
 xsp->bp[0] &= 1L;
 hwp[0] &= ~(3L << 2);
 hwp[0] |= ((hword) ((xsp->ap[0] | (xsp->bp[0] << 1)) << 2));
 __pop_xstk();
 /* eval. 1st input in gpins - index 1 */
 __eval_bufif_gate(gp, 1, &out_chg);
}

/*
 * initialize the state of a tranif gate by evaluating control input
 * here must evaluate input and store into state
 * for tran, do not need any initialization
 */
static void init_tranif_gate(struct gate_t *gp)
{
 register word32 cval;
 int32 conducting, gateid, bi, wi;
 struct xstk_t *xsp;

 /* first initialize conducting state */
 xsp = __eval_xpr(gp->gpins[2]);
 conducting = (xsp->ap[0] & 1L) | ((xsp->bp[0] & 1L) << 1);
 if (conducting == 2) conducting = 3;
 __pop_xstk();
 gateid = gp->gmsym->el.eprimp->gateid;
 /* for if0s, 0 turns on (1), 1 of (0) */
 if (gateid == G_TRANIF0 || gateid == G_RTRANIF0)
  {
   if (conducting == 0) conducting = 1;
   else if (conducting == 1) conducting = 0;
  }

 /* immediate change to conducting state during initialization */
 bi = get_bofs_(2*__inum);
 wi = get_wofs_(2*__inum);
 cval = gp->gstate.wp[wi];
 cval &= ~(3L << bi);
 cval |= (conducting << bi);
 gp->gstate.wp[wi] = cval;

 if (__debug_flg && __ev_tracing)
  {
   __tr_msg("-- %s %s 3rd input evaluated - initial conducting: %s\n",
    gp->gmsym->synam, to_evtronam(__xs, gp->gsym->synam, __inst_ptr,
    (struct task_t *) NULL), (conducting == 1) ? "*ON*"
     : ((conducting == 0) ? "*OFF*" : "*UNKNOWN*"));
  }
}

/*
 * evaluate continous assign to initialize lhs
 * notice ignore conta delay here but use wire delay
 *
 * SJM 09/28/02 - for per bit rhs concat contas, caller passed decomposed PB
 */
static void conta_initeval(struct conta_t *cap, struct conta_t *mast_cap)
{
 int32 lhslen, orhslen;
 byte *sbp;
 struct xstk_t *xsp, *xsp2;
 struct expr_t *xp, *lhsxp;
 struct sy_t *syp;

 /* do not need to set lhs here before schedule changed */
 xp = cap->rhsx;
 lhsxp = cap->lhsx;
 /* if rhs normal function must call it in case of constant args */
 /* but $getpattern must just assign right width x's since index probably */
 /* out of range at this point */
 if (xp->optyp == FCALL)
  {
   syp = xp->lu.x->lu.sy;
   /* know getpat conta form never has rhsval wp or driver wp */
   /* rule is that $getpattern with unknown index is x's */
   if (syp->sytyp == SYM_SF && syp->el.esyftbp->syfnum == STN_GETPATTERN)
    {
     lhslen = lhsxp->szu.xclen;
     push_xstk_(xsp, lhslen);
     /* IN - getpattern with unknown index (like at init) return x value */
     one_allbits_(xsp->ap, lhslen);
     one_allbits_(xsp->bp, lhslen);

     /* know all getpat lhs wires fi == 1 and no wire delay and no stren */
     __exec_ca_concat(lhsxp, xsp->ap, xsp->bp, FALSE);
     __pop_xstk();
     return;
    }
  }

 lhslen = lhsxp->szu.xclen;
 xsp = __eval_xpr(xp);
 if (xsp->xslen != lhslen)
  {
   orhslen = xsp->xslen;

   /* SJM 09/29/03 - change to handle sign extension and separate types */
   if (xsp->xslen > lhslen) __narrow_sizchg(xsp, lhslen);
   else if (xsp->xslen < lhslen)
    {
     if (xp->has_sign) __sgn_xtnd_widen(xsp, lhslen);
     else __sizchg_widen(xsp, lhslen);
    }
   /* during initialization widen to x not 0 */

   /* SJM 05/19/04 - remove see below must widen rhs expr using 0's */
   /* --
   if (orhslen < xsp->xslen)
    {
     if (__wire_init) __fix_widened_toxs(xsp, orhslen);
    }
   -- */
  }

 /* fi == 1 and no delay contas have no driver state stored */
 if (cap->ca_drv_wp.wp != NULL)
  {
   /* SJM - 02/18/03 - remove - since above does same change so never exec */
   /* --- REMOVED
   if (xsp->xslen != lhslen)
    {
     orhslen = xsp->xslen;

     -* SJM 09/29/03 - change to handle sign extension and separate types *-
     if (xsp->xslen > lhslen) __narrow_sizchg(xsp, lhslen);
     else if (xsp->xslen < lhslen)
      {
       if (xp->has_sign) __sgn_xtnd_widen(xsp, lhslen);
       else __sizchg_widen(xsp, lhslen);
      }

     __fix_widened_toxs(xsp, orhslen);
    }
   -- */
   __st_perinst_val(cap->ca_drv_wp, lhslen, xsp->ap, xsp->bp);
  }
 if (__debug_flg && __ev_tracing)
  {
   char s1[RECLEN];

   /* notice even if delay wire, off during wire initialization */
   strcpy(s1, "assigned");
   __tr_msg("-- %s %s initial value %s\n", s1,
    __to_evtrcanam(__xs, mast_cap, __inst_ptr),
    __regab_tostr(__xs2, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
  }

 /* notice multi-fi case assume new driving value already assigned */
 /* this will add any conta driving strength if needed from ca drv wp */
 if (lhsxp->x_multfi) __mdr_assign_or_sched(lhsxp);
 else
  {
   /* notice wire delays including specify paths off during wire init */
   if (lhsxp->x_stren)
    {
     push_xstk_(xsp2, 4*lhslen);
     sbp = (byte *) xsp2->ap;
     __st_standval(sbp, xsp, cap->ca_stval);
     if (lhsxp->optyp == LCB) __stren_exec_ca_concat(lhsxp, sbp, FALSE);
     /* SJM 03/30/99 - was storing value without strength added */
     else __exec_conta_assign(lhsxp, xsp2->ap, xsp2->bp, FALSE);
     __pop_xstk();
    }
   else
    {
     if (lhsxp->optyp == LCB)
      __exec_ca_concat(lhsxp, xsp->ap, xsp->bp, FALSE);
     else __exec_conta_assign(lhsxp, xsp->ap, xsp->bp, FALSE);
    }
  }
 __pop_xstk();
}

/*
 * MEDIUM LEVEL SCHEDULING MECHANISM ROUTINES
 */

/*
 * insert event in timing wheel or overflow q (know event after now)
 */
extern void __insert_event(register i_tev_ndx tevpi)
{
 register int32 schtwi;
 register struct tev_t *tevp;
 register struct telhdr_t *telp;
 word64 schtim, t;

 tevp = &(__tevtab[tevpi]);
 schtim = tevp->etime;
 /* because of wrap around event up to __twhsize - 1 fit in timing wheel */
 /* notice borrow possible here - but index always fit in 1 word32 */
 t = schtim - __simtime;
 /* schtwi here is number of timing wheel slots to scheduled event */
 schtwi = (int32) (t & WORDMASK_ULL);

 /* if event would go in timing wheel if it were at 0, then goes in */
 /* but may need to wrap around */
 if (schtwi < __twhsize && t < 0x7fffffffULL)
  {
   /* wrap around table if needed */
   if ((schtwi += __cur_twi) >= __twhsize) schtwi %= __twhsize;
   /* ??? DBG add ---
   if (__debug_flg && __ev_tracing)
    {
     word64 t2;
     char s1[RECLEN], s2[RECLEN];

     t = (word64) (__twhsize - 1);
     t2 = __whetime - t;
     __tr_msg(
      ".. adding %s event to timing wheel based at %s for time %s (schtwi=%d, cur_twi=%d)\n",
      __to_tetyp(__xs, tevp->tetyp), __to_timstr(s1, &t2),
      __to_timstr(s2, &schtim), schtwi, __cur_twi);
    }
   --- */

   telp = __twheel[schtwi];
    /* know tevp next field is nil */
   if (telp->te_hdri == -1) telp->te_hdri = telp->te_endi = tevpi;
   else
    {
     if (tevp->vpi_onfront)
      { tevp->tenxti = telp->te_hdri; telp->te_hdri = tevpi; }
     else
      { __tevtab[telp->te_endi].tenxti = tevpi; telp->te_endi = tevpi; }
    }
   telp->num_events += 1;
   __num_twhevents++;
  }
 else add_ovfetim(schtim, tevpi, tevp);
}

/*
 * routine to allocate event - non macro for debugging
 * the b zero initializes all flags to off
 */
/* DBG ??? remove --- */
extern i_tev_ndx __alloc_tev(int32 etyp, struct itree_t *itp, word64 absetime)
{
  register struct tev_t *tevp__;
  register i_tev_ndx tevpi;

  if (__tefreelsti != -1)
   { tevpi = __tefreelsti; __tefreelsti = __tevtab[__tefreelsti].tenxti; }
  else
   {
    if (++__numused_tevtab >= __size_tevtab) __grow_tevtab();
    tevpi = __numused_tevtab;
   }
  tevp__ = &(__tevtab[tevpi]);
  /* LOOKATME - maybe zeroing to init bit fields unportable */
  memset(tevp__, 0, sizeof(struct tev_t));
  tevp__->tetyp = etyp;
  tevp__->teitp = itp;
  tevp__->etime = absetime;
  tevp__->tenxti = -1;
  return(tevpi);
}
/*  --- */

/*
 * grow the tev table by reallocating
 *
 * BEWARE - because of growth by reallocating tevp ptrs can only
 * be used as tmps between calls to alloc tev macro
 *
 * notice this increases table size when no free and next to used at
 * end of table, after grow caller increases num used value
 */
extern void __grow_tevtab(void)
{
 int32 osize, nsize;

 osize = __size_tevtab*sizeof(struct tev_t);
 __size_tevtab += (__size_tevtab/2);
 nsize = __size_tevtab*sizeof(struct tev_t);
 __tevtab = (struct tev_t *) __my_realloc((char *) __tevtab, osize, nsize);

 /* DBG remove --- UNDO */
 if (__debug_flg)
  __dbg_msg("+++ fixed event table grew from %d bytes to %d\n", osize, nsize);
 /* --- */
}

/*
 * normally unused droutine for debugging
 */
static void chk_tev_list(register i_tev_ndx tevpi)
{
 struct tev_t *tevp;

 for (; tevpi != -1; tevpi = __tevtab[tevpi].tenxti)
  {
   tevp = &(__tevtab[tevpi]);
   if (tevp->tetyp < 1 || tevp->tetyp > 14) __misc_terr(__FILE__, __LINE__);
  }
}

/*
 * normally unused routine for checking pending scheduled dce events
 * UNUSED
 */
/* ---
static void chk_schd_dces(void)
{
 register int32 ni;
 register struct mod_t *mdp;
 register struct task_t *tskp;
 struct net_t *np;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++)
    {
     if (np->dcelst == NULL) continue;
     chk_1nschd_dce(np, mdp);
    }
   for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       if (np->dcelst == NULL) continue;
       chk_1nschd_dce(np, mdp);
      }
    }
  }
}
-- */

/*
 * check one static scheduled dce
 * UNUSED
 */
/* ---
static void chk_1nschd_dce(struct net_t *np, struct mod_t *mdp)
{
 register struct dcevnt_t *dcep;
 register int32 ii;
 i_tev_ndx tevpi;
 struct delctrl_t *dctp;

 for (dcep = np->dcelst; dcep != NULL; dcep = dcep->dcenxt)
  {
   if (dcep->dce_typ != DCE_RNG_INST && dcep->dce_typ != DCE_INST)
     continue;
   dctp = dcep->st_dctrl;
   if (dctp->dceschd_tevs == NULL)
    __cvsim_msg("*** net %s dcep no dceschd_tevs\n", np->nsym->synam);
   for (ii = 0; ii < mdp->flatinum; ii++)
    {
     if ((tevpi = dctp->dceschd_tevs[__inum]) != -1)
      __cvsim_msg("*** net %s inst num. %s dceschd_tevs index %d set\n",
       np->nsym->synam, ii, tevpi);
     if (__tevtab[tevpi].tetyp < 1 || __tevtab[tevpi].tetyp > 14)
      __misc_terr(__FILE__, __LINE__);
    }
  }
}
--- */

/*
 * ROUTINES TO IMPLEMENT TIME FLOW
 */

static int32 move_to_time0(void)
{
 register struct telhdr_t *twp;

 __simtime++;
 twp = __twheel[++__cur_twi];
 __cur_te_hdri = twp->te_hdri;
 __cur_te_endi = twp->te_endi;
 return(TRUE);
}

/*
 * move to next time slot
 * return FALSE if no events pending (i.e. all done)
 * know both normal and pound 0 event lists empty from last time
 *
 * when done, events to process at current time ready to be processed
 * and __cur_te_hdr and __cur_te_end point to the now event queue
 */
static int32 move_time(void)
{
 register struct telhdr_t *twp;
 word64 tmp;

 /* -- DBG remove
 if (__btqroot != NULL) dmp_btree(__btqroot);
 dmp_twheel();
 --- */

 __simtime++;
 /* normal case, find event in timing wheel */
 /* DBG remove ---
 if (__num_twhevents < 0) __misc_terr(__FILE__, __LINE__);
 --- */
 if (__num_twhevents == 0) goto move_gap;
again:
 /* --- DBG remove chk_event_consist(); */

 twp = __twheel[++__cur_twi];
 /* hit sentinel */
 if (twp->num_events == -1) goto at_twend;
 if (twp->te_hdri == -1) { __simtime++; goto again; }
 __cur_te_hdri = twp->te_hdri;
 __cur_te_endi = twp->te_endi;
 goto got_event;

 /* move all events whose time is < new sim time + twh size to time wheel */
 /* copy header of timing q to timing wheel */
 /* if no events in overflow q nothing to do here */
 /* this reduces number of overflow events and increases __twheel events */
at_twend:
 tmp = (word64) (__twhsize - 1);
 __whetime = __simtime + tmp;
 __cur_twi = -1;
 /* events still in timing wheel, if ovflow empty, must advance wheel */
 if (__btqroot != NULL) ovflow_into_wheel();
 /* DBG remove -- */
 if (__debug_flg && __ev_tracing)
  {
   word64 t;

   tmp = (word64) __twhsize;
   t = __whetime - tmp + 1;
   /* whe time is end of timing wheel time */
   __tr_msg(".. timing wheel base moved to %s\n", __to_timstr(__xs, &t));
  }
 /* --- */
 /* know timing wheel not empty or will not get to at twend */
 goto again;

 /* handle gap in timing wheel - know wheel currently empty */
move_gap:
 if (__btqroot == NULL) return(FALSE);
 __simtime = __btqroot->btltim;
 tmp = (word64) (__twhsize - 1);
 __whetime = __simtime + tmp;
 /* DBG remove */
 if (__whetime <= __simtime)
  {
   __pv_terr(338,
    "scheduled event causes 64 bit time overflow - contact vendor");
  }
 /* -- */
 ovflow_into_wheel();
 /* DBG remove -- */
 if (__debug_flg && __ev_tracing)
  {
   __tr_msg(".. event gap - jumping to %s\n", __to_timstr(__xs, &__simtime));
  }
 /* -- */
 /* know at least one event in timing wheel */
 __cur_twi = -1;
 goto again;
got_event:
 /* getting to here means have event - better always happen */
 /* --- DBG remove --
 if (__debug_flg && __ev_tracing)
  {
   __tr_msg(
    ".. dumping current pending event list for time %s (%ld events):\n",
    __to_timstr(__xs, &__simtime), twp->num_events);
   dmp_events(twp->te_hdri);
   __dmp_event_tab();
  }
 -- */
 return(TRUE);
}

/*
 * check a timing wheel to make sure number of events consistent
 */
static void chk_event_consist(void)
{
 register int32 twi;
 int32 num_whevents;

 for (num_whevents = 0, twi = 0;; twi++)
  {
   /* use sentinel for end */
   if (__twheel[twi]->num_events == -1) break;
   num_whevents += __twheel[twi]->num_events;
  }

 /* DBG remove --- */
 if (__num_twhevents != num_whevents) __misc_terr(__FILE__, __LINE__);
 if (__btqroot == NULL)
  { if (__num_ovflqevents != 0) __misc_terr(__FILE__, __LINE__); }
 else { if (__num_ovflqevents == 0) __misc_terr(__FILE__, __LINE__); }
 /* --- */
}

/*
 * adding event tevp to overflow tree at time etim
 * will never see #0 form here and always add exactly 1 event
 */
static void add_ovfetim(word64 etim, i_tev_ndx tevpi, struct tev_t *tevp)
{
 struct bt_t *btp, *splthdr, *new_splthdr;
 struct telhdr_t *telpp;

 __num_ovflqevents++;
 /* ---
 if (__debug_flg && __ev_tracing)
  {
   word64 tmp, t;
   char s1[RECLEN];

   tmp = (word64) __twhsize;
   t = __whetime - tmp + 1;
   __tr_msg(
    ".. adding event after timing wheel based at %s for time %s (ovfl. num. %d)\n",
    __to_timstr(s1, &t), __to_timstr(__xs, &(tevp->etime)), __num_ovflqevents);
  }
 --- */
 /* empty tree */
 if (__btqroot == NULL)
  {
   __btqroot = alloc_btnod(BTNORM);
   __btqroot->btnfill = 1;
   btp = alloc_btnod(BTFRNGE);
   __max_level = 1;
   __btqroot->ofsu.btofs = btp;
   __btqroot->btltim = etim;

   btp->ofsu.telp = telpp = (struct telhdr_t *)
    __my_malloc(sizeof(struct telhdr_t));
   telpp->te_hdri = telpp->te_endi = tevpi;
   telpp->num_events = 1;
   btp->btltim = etim;
   btp->btnxt = NULL;
   btp->btnfill = 1;
   return;
  }

 /* search down tree to find fringe node that new time gets found or */
 /* insert in */
 splthdr = find_fringe(etim);

 /* insert somewhere in fringe - know goes within range or on ends */
 /* if found and did not need to insert, done */
 if ((new_splthdr = insert_fringe(splthdr, etim, tevpi)) == NULL)
  return;

 /* if inserted at front must update 1 up from fringe path */
 if (splthdr != new_splthdr)
  {
   btp = __btndhdrstk[__topi];
   btp->ofsu.btofs = new_splthdr;
  }

 /* if not did not grow to be too wide, done */
 if (new_splthdr->btnfill < BTREEMAXWID) return;
 /* this uses path set in find_fringe to split and grow tree upwards */
 splitinsert_nonfringe();
}

/*
 * allocate a btree node of type bntyp
 */
static struct bt_t *alloc_btnod(int32 btyp)
{
 struct bt_t *btp;

 btp = (struct bt_t *) __my_malloc(sizeof(struct bt_t));
 btp->bttyp = btyp;
 btp->btnfill = 0;
 btp->btltim = 0ULL;
 btp->ofsu.btofs = NULL;
 btp->btnxt = NULL;
 return(btp);
}

/*
 * find fringe multiple element node that etim goes in or divides or will be
 * found in
 * return header node of fringe node as list and set __topi to 1 less than
 * fringe - stack does not contain fringe node
 *
 * notice just stops when hits fringe - does not stack fringe node because
 * maybe be on front of list that is multikey nod or after end
 */
static struct bt_t *find_fringe(word64 etim)
{
 register struct bt_t *btp;
 struct bt_t *hdrbtp, *last_btp;

 /* stack special size 1 root node */
 __btndhdrstk[0] = __btndstk[0] = __btqroot;
 hdrbtp = __btqroot->ofsu.btofs;
 /* must handle case of fringe immediately under root */
 __topi = 0;
 if (hdrbtp->bttyp == BTFRNGE) return(hdrbtp);

 for (__topi = 0;;)
  {
   /* stack 1 down header of linked node */
   __btndhdrstk[++__topi] = last_btp = hdrbtp;
   for (btp = hdrbtp->btnxt; btp != NULL; btp = btp->btnxt)
    {
     /* true here means path selects last btp node */
     if (etim < btp->btltim) break;
     last_btp = btp;
    }
   __btndstk[__topi] = last_btp;
   hdrbtp = last_btp->ofsu.btofs;
   /* notice top of stack is one above fringe node */
   if (hdrbtp->bttyp == BTFRNGE) break;
  }
 return(hdrbtp);
}

/*
 * insert a fringe node into a fringe node list
 * this is simple linear linked list insert
 *
 * if needs to allocate new node puts event into new telhdr node
 * else adds to end of right list
 * return new fringe header node ptrif added else NULL if found
 * even if found still adds time event to found time event header
 *
 * notice fringe node (with pointer to telhdr) never stacked on path list
 */
static struct bt_t *insert_fringe(struct bt_t *frnghdr, word64 etim,
 i_tev_ndx tevpi)
{
 register int32 i;
 register struct bt_t *btp;
 int32 goes_onend;
 struct bt_t *last_btp, *btpnew;
 struct telhdr_t *telp;

 goes_onend = FALSE;
 for (last_btp = NULL, btp = frnghdr; btp != NULL; btp = btp->btnxt)
  {
   if (etim > btp->btltim) { last_btp = btp; continue; }

   /* found place */
   if (etim == btp->btltim)
    {
     telp = btp->ofsu.telp;
     if (telp->te_hdri == -1) telp->te_hdri = telp->te_endi = tevpi;
     else
      {
       if (__tevtab[tevpi].vpi_onfront)
        { __tevtab[tevpi].tenxti = telp->te_hdri; telp->te_hdri = tevpi; }
       else { __tevtab[telp->te_endi].tenxti = tevpi; telp->te_endi = tevpi; }
      }
     telp->num_events += 1;
     return(NULL);
    }

do_add:
   /* allocate new fringe node */
   telp = (struct telhdr_t *) __my_malloc(sizeof(struct telhdr_t));
   telp->te_hdri = telp->te_endi = tevpi;
   telp->num_events = 1;

   btpnew = alloc_btnod(BTFRNGE);
   btpnew->btltim = etim;
   btpnew->ofsu.telp = telp;

   /* goes past end - insert after last_btp */
   if (goes_onend)
    { last_btp->btnxt = btpnew; btpnew->btnxt = NULL; }
   else
    {
     /* insert before btp */
     if (last_btp == NULL)
      {
       btpnew->btnxt = btp;
       btpnew->btnfill = frnghdr->btnfill;
       frnghdr->btnfill = 0;
       frnghdr = btpnew;

       /* this is tricky case since btpnew time less than all header nodes */
       /* in tree - fix using header node path */
       for (i = __topi; i >= 0; i--) __btndhdrstk[i]->btltim = etim;
      }
     else { btpnew->btnxt = btp; last_btp->btnxt = btpnew; }
    }
   frnghdr->btnfill = frnghdr->btnfill + 1;
   return(frnghdr);
  }
 goes_onend = TRUE;
 goto do_add;
}

/*
 * insert fringe node that gets split into non fringe upward parent node
 * keep propagating wide nodes up to root
 * know must split fringe node or will not get here
 *
 * know __topi stack of path that got to this fringe node
 * but top is one up from fringe node
 */
static void splitinsert_nonfringe(void)
{
 register int32 i;
 register struct bt_t *btp;
 int32 stki;
 struct bt_t *parbtp, *parhdr, *splt1, *splt2, *last_btp, *splt2par;

 /* notice fringe node not stacked, top of stack is parent of fringe */
 splt1 = __btndstk[__topi]->ofsu.btofs;
 last_btp = NULL;
 for (stki = __topi;;)
  {
   /* split too wide node into 2 - max must be divisible by 2 */
   /* know at least 1 node here */
   for (btp = splt1, i = 0; i < BTREEMAXWID/2; i++)
    { last_btp = btp; btp = btp->btnxt; }
   last_btp->btnxt = NULL;
   splt2 = btp;
   splt1->btnfill = BTREEMAXWID/2;
   splt2->btnfill = BTREEMAXWID/2;

   /* construct non fringe node to link splt2 node list onto */
   splt2par = alloc_btnod(BTNORM);
   splt2par->btltim = splt2->btltim;
   splt2par->ofsu.btofs = splt2;

   /* parent nodes of path used to descend to fringe */
   parbtp = __btndstk[stki];
   parhdr = __btndhdrstk[stki];
   /* up 1 level is special root node - must increase tree height */
   if (stki == 0)
    {
     /* allocate new added level parbtp (same as header for root) */
     parbtp = alloc_btnod(BTNORM);
     parbtp->btltim = splt1->btltim;
     parbtp->ofsu.btofs = splt1;
     parbtp->btnfill = 2;
     parbtp->btnxt = splt2par;
     parhdr->ofsu.btofs = parbtp;
     splt2par->btnxt = NULL;
     /* this is only way max. tree level can increase */
     __max_level++;
     return;
    }
   /* on sun += does not work for bit fields */
   parhdr->btnfill = parhdr->btnfill + 1;

   splt2par->btnxt = parbtp->btnxt;
   parbtp->btnxt = splt2par;
   if (parhdr->btnfill < BTREEMAXWID) break;
   stki--;
   splt1 = __btndstk[stki]->ofsu.btofs;
  }
}

/*
 * LOW LEVEL OVERFLOW QUEUE TO TIMING WHEEL ROUTINES
 */

/*
 * depth first move of nodes to timing wheel
 */
static void ovflow_into_wheel(void)
{
 int32 stki;
 struct bt_t *btphdr;

 /* if leftmost time in overflow tree past wheel end time, nothing to do */
 if (__btqroot->btltim > __whetime) return;

 __btndhdrstk[0] = __btndstk[0] = __btqroot;
 btphdr = __btqroot->ofsu.btofs;
 for (__topi = 0;;)
  {
   __btndhdrstk[++__topi] = btphdr;
   /* DBG remove ---
   if (__debug_flg && __ev_tracing)
    {
     word64 t, tmp;

     tmp = (word64) __twhsize;
     t = (__whetime - tmp) + 1;
     __tr_msg(
      ".. time queue move to wheel based at %s descending to level %d\n",
      __to_timstr(__xs, &t), __topi);
    }
   -- */
   /* case 1 - descended down to fringe node */
   if (btphdr->bttyp == BTFRNGE)
    {
     divide_fringe_node(btphdr);
     break;
    }
   divide_internal_node(btphdr);
   /* move down one level from new divide node */
   btphdr = __btndstk[__topi]->ofsu.btofs;
  }
 /* must set min times of list hdr nodes */
 /* since do not know new and left times until hit fringe */
 if (__btqroot == NULL) return;

 if (__btqroot != NULL)
  {
   for (stki = __topi; stki > 0; stki--)
    __btndstk[stki - 1]->btltim = __btndstk[stki]->btltim;
  }
 /* finally remove any size one nodes at top */
 for (;;)
  {
   btphdr = __btqroot->ofsu.btofs;
   if (btphdr->bttyp == BTFRNGE || btphdr->btnfill != 1) return;
   if (__debug_flg && __ev_tracing)
    {
     __tr_msg(".. removing redundant size 1 node under root\n");
    }
   __btqroot->ofsu.btofs = btphdr->ofsu.btofs;
   __my_free((char *) btphdr, sizeof(struct bt_t));
   btphdr = NULL;
  }
}

/*
 * divide a fringe node
 */
static void divide_fringe_node(struct bt_t *btphdr)
{
 register struct bt_t *btp, *btp2;
 int32 cnum;
 struct bt_t *btptmp;

 /* fringe node low time cannot be larger than wheel end */
 if (btphdr->btltim > __whetime) __misc_terr(__FILE__, __LINE__);

 /* case 1 (illegal): all of fringe node remains in tree */
 cnum = btphdr->btnfill - 1;
 for (btp = btphdr->btnxt; btp != NULL; btp = btp->btnxt)
  {
   if (btp->btltim > __whetime)
    {
     /* case 2: from 2nd to nth is first node of new tree after removal */
     /* remove all nodes up to btp */
     for (btp2 = btphdr; btp2 != btp;)
      {
       btptmp = btp2->btnxt;
       mv_to_wheel(btp2->btltim, btp2->ofsu.telp);
       /* SJM 03/07/01 - always fringe, must free telp too */
       __my_free((char *) btp2->ofsu.telp, sizeof(struct telhdr_t));
       __my_free((char *) btp2, sizeof(struct bt_t));
       btp2 = btptmp;
      }
     /* fixup tree */
     __btndstk[__topi] = __btndhdrstk[__topi] = btp;
     btp->btnfill = cnum;
     __btndhdrstk[__topi - 1]->ofsu.btofs = btp;
     __btndhdrstk[__topi - 1]->btltim = btp->btltim;
     return;
    }
   cnum--;
  }
 /* case 3 - all fringe nodes go in timing wheel */
 /* remove all nodes */
 for (btp = btphdr; btp != NULL;)
  {
   btptmp = btp->btnxt;
   mv_to_wheel(btp->btltim, btp->ofsu.telp);
   /* AIV 05/21/04 - miss one here, must free telp too */
   __my_free((char *) btp->ofsu.telp, sizeof(struct telhdr_t));
   __my_free((char *) btp, sizeof(struct bt_t));
   btp = btptmp;
  }
 /* remove know 1 up empty and propagate empties up */
 remove_empty_upwards();
}

/*
 * divide internal node
 */
static void divide_internal_node(struct bt_t *btphdr)
{
 register struct bt_t *btp;
 int32 cnum;
 struct bt_t *last_btp, *btp2, *btptmp;

 /* case 1 (impossible) - all of tree past timing wheel */
 /* internal node low time cannot be larger than wheel end */
 if (btphdr->btltim > __whetime) __misc_terr(__FILE__, __LINE__);

 cnum = btphdr->btnfill - 1;
 last_btp = btphdr;
 for (btp = btphdr->btnxt; btp != NULL; btp = btp->btnxt)
  {
   if (btp->btltim > __whetime)
    {
got_divide:
     /* case 2: from 2nd to nth is first node of new tree after removal */
     /* remove all subtrees up to last_btp (divide node) */
     for (btp2 = btphdr; btp2 != last_btp;)
      {
       btptmp = btp2->btnxt;
       mv_subtree_towheel(btp2->ofsu.btofs);
       __my_free((char *) btp2, sizeof(struct bt_t));
       btp2 = btptmp;
      }
     /* fixup tree - last_btp is divide node */
     last_btp->btnfill = cnum + 1;
     __btndstk[__topi] = last_btp;
     __btndhdrstk[__topi] = last_btp;
     __btndstk[__topi - 1]->ofsu.btofs = last_btp;
     __btndstk[__topi - 1]->btltim = last_btp->btltim;
     return;
    }
   cnum--;
   last_btp = btp;
  }
 /* case 3 - divide not is last node in tree */
 goto got_divide;
}

/*
 * when leaf empty (size == 0) must remove upward
 */
static void remove_empty_upwards(void)
{
 register struct bt_t *btp;
 struct bt_t *last_btp, *rembtp;
 int32 stki, stki2;

 /* case 1, if only root is above now empty fringe - remove all of tree */
 if (__topi == 1)
  {
empty_tree:
   __my_free((char *) __btqroot, sizeof(struct bt_t));

   /* SJM 05/26/04 - notice root node is special case no telp multi */
   /* element not, instead just a pointer to a btp */
   __btqroot = NULL;
   return;
  }
 /* case 2, need to remove at least fringe */
 /* first - chain upwards of size 1 nodes that get removed */
 for (stki = __topi - 1; stki > 0; stki--)
  {
   /* this can never be fringe since fringe not in btndstk */
   if (__btndhdrstk[stki]->btnfill != 1) goto got_nonremove;
   __my_free((char *) __btndstk[stki], sizeof(struct bt_t));
   __btndstk[stki] = NULL;
  }
 goto empty_tree;

got_nonremove:
 /* know that node at level stki stays */
 /* step 1: link out node */
 /* step 1a: find predecessor of linked out node if exists */
 rembtp = __btndstk[stki];
 /* case 2a: header node is removed */
 if (rembtp == __btndhdrstk[stki])
  {
   /* know node following rembtp exists */
   __btndhdrstk[stki - 1]->ofsu.btofs = rembtp->btnxt;
   __btndstk[stki] = rembtp->btnxt;
   __btndstk[stki]->btnfill = __btndhdrstk[stki]->btnfill - 1;
   __btndhdrstk[stki] = __btndstk[stki];
  }
 /* case 2b: non header removed */
 else
  {
   /* find predecessor of removed node since it is now node stack */
   last_btp = NULL;
   for (btp = __btndhdrstk[stki]; btp != rembtp; btp = btp->btnxt)
    last_btp = btp;
   __btndhdrstk[stki]->btnfill -= 1;

   /* case 2b1: last node removed */
   if (rembtp->btnxt == NULL)
    {
     /* last_btp is one before removed node */
     __btndstk[stki] = last_btp;
 
     /* SJM 08/02/01 - add if to keep lint happy */
     if (last_btp != NULL) last_btp->btnxt = NULL;
    }
   /* case 2b2: internal node removed */
   else
    {
     __btndstk[stki] = rembtp->btnxt;
     /* SJM 08/02/01 - add if to keep lint happy */
     if (last_btp != NULL) last_btp->btnxt = rembtp->btnxt;
    }
  }
 /* notice __btndstk that points to rembtp - has new value (moved down) */
 __my_free((char *) rembtp, sizeof(struct bt_t));
 /* finally, work upwards to fringe updating __btndstk */
 /* notice even if only fringe removed, this will change ndstk for fringe */
 for (stki2 = stki + 1; stki2 <= __topi; stki2++)
  __btndstk[stki2] = __btndstk[stki2 - 1]->ofsu.btofs;
}

/*
 * move an entire subtree to timing wheel and free subtree
 * not called for fringe nodes
 */
static void mv_subtree_towheel(struct bt_t *btphdr)
{
 register struct bt_t *btp;
 struct bt_t *btp2;

 for (btp = btphdr; btp != NULL;)
  {
   btp2 = btp->btnxt;

   if (btp->bttyp == BTFRNGE)
    {
     mv_to_wheel(btp->btltim, btp->ofsu.telp);
     /* SJM 03/07/01 - only fringe node has telp than needs to be freed */
     __my_free((char *) btp->ofsu.telp, sizeof(struct telhdr_t));
    }
   else mv_subtree_towheel(btp->ofsu.btofs);

   /* always free the node */
   __my_free((char *) btp, sizeof(struct bt_t));
   btp = btp2;
  }
}

/*
 * move an event header te lst to timing wheel
 */
static void mv_to_wheel(word64 etim, struct telhdr_t *telp)
{
 int32 twslot;
 word64 tmp;
 struct telhdr_t *twlp;

 /* removing fringe node that should stay at time */
 if (etim > __whetime) __misc_terr(__FILE__, __LINE__);
 /* ---
 if (__debug_flg && __ev_tracing)
  {
   __tr_msg(".. moving time queue to wheel based at time %s\n",
    __to_timstr(__xs, &etim));
  }
 --- */

 /* add overflow q to correct wheel slot - must go on front */
 /* because of wrapping later events already on wheel element */
 /* sim time + 1 is 0th timing wheel position */
 tmp = etim - __simtime;
 twslot = (int32) (tmp & WORDMASK_ULL);

 /* notice, there will always be at least one entry on overflow q */
 /* list or will not get here - cancelled events just marked */
 /* add to end if timing wheel slot already has events */
 /* DBG remove  --- */
 if (twslot < 0 || twslot > __twhsize) __misc_terr(__FILE__, __LINE__);
 /* --- */
 twlp = __twheel[twslot];
 /* DBG remove  --- */
 if (twlp == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 /* twlp points to current wheel events - telp to list to move on end */
 if (twlp->te_hdri == -1)
  {
   twlp->te_hdri = telp->te_hdri;
   twlp->te_endi = telp->te_endi;
   twlp->num_events = telp->num_events;
  }
 else
  {
   /* splice onto end and set new end - leave front as is */
   __tevtab[twlp->te_endi].tenxti = telp->te_hdri;
   twlp->te_endi = telp->te_endi;
   twlp->num_events += telp->num_events;
  }
 __num_twhevents += telp->num_events;
 __num_ovflqevents -= telp->num_events;
 if (__num_ovflqevents < 0) __misc_terr(__FILE__, __LINE__);
}

/*
 * find btree node after or same as tim
 *
 * LOOKATME - why is this not called
 */
static struct telhdr_t *tfind_btnode_after(struct bt_t *btphdr, word64 tim)
{
 register struct bt_t *btp;
 struct telhdr_t *telp;

 if (btphdr->bttyp == BTFRNGE)
  {
   for (btp = btphdr; btp != NULL; btp = btp->btnxt)
    { if (btp->btltim >= tim) return(btp->ofsu.telp); }
   return(NULL);
  }
 for (btp = btphdr; btp != NULL; btp = btp->btnxt)
  {
   if ((telp = tfind_btnode_after(btp->ofsu.btofs, tim)) != NULL)
    return(telp);
  }
 return(NULL);
}

/*
 * Q DEBUGGING ROUTINES
 */

/*
 * dump all events in timing wheel
 * only called if debug flag on
 */
static void dmp_twheel(void)
{
 register int32 twi;
 int32 e_num, totenum;

 __dbg_msg("<< timing wheel that ends at %s\n", __to_timstr(__xs, &__whetime));
 for (twi = 0, totenum = 0;; twi++)
  {
   /* use sentinel for end */
   if (__twheel[twi]->num_events == -1) break;
   e_num = dmp_events(__twheel[twi]->te_hdri);
   totenum += e_num;
   /* DBG remove ---
   __dbg_msg("--index %d %d counted events and %d stored--\n", twi,
    e_num, __twheel[twi]->num_events);
   ---*/
  }
 __dbg_msg("<< total counted wheel events %d, overflow %d, wheel stored %d\n",
  totenum, __num_ovflqevents, __num_twhevents);
}

/*
 * dump event list
 */
static int32 dmp_events(register i_tev_ndx tevpi)
{
 int32 e_num;
 /* char s1[20], s2[RECLEN]; */

 for (e_num = 0; tevpi != -1; tevpi = __tevtab[tevpi].tenxti, e_num++)
  {
   /* --
   struct tev_t *tevp;

   tevp = &(__tevtab[tevpi]);
   __dbg_msg("^^%s event index %d in inst. %s at %s cancel=%d\n",
    __to_tetyp(s1, tevp->tetyp), tevpi, __msg2_blditree(s2, tevp->teitp),
    __to_timstr(__xs, &__simtime), tevp->te_cancel);
   -- */
  }
 return(e_num);
}

/*
 * dump event table and free list
 */
extern void __dmp_event_tab(void)
{
 register int32 ei;
 struct tev_t *tevp;
 char s1[RECLEN];

 /* dump all allocated events */
 __dbg_msg("*** DUMPING EVENT TABLE *** (high used %d)\n",
  __numused_tevtab);
 for (ei = 0; ei <= __numused_tevtab; ei++)
  {
   tevp = &(__tevtab[ei]);
   __dbg_msg("^^%s (%d) event index %d next %d\n",
    __to_tetyp(s1, tevp->tetyp), tevp->tetyp, ei, tevp->tenxti);
  }
 if (__tefreelsti != -1)
  {
   __dbg_msg("*** DUMPING FREE LIST ***\n");
   for (ei = __tefreelsti; ei != -1; ei = __tevtab[ei].tenxti)
    {
     tevp = &(__tevtab[ei]);
     __dbg_msg("^^%s (%d) free event index %d next %d\n",
      __to_tetyp(s1, tevp->tetyp), tevp->tetyp, ei, tevp->tenxti);
    }
  }
}

/*
 * dump a levelized tree
 */
static void dmp_btree(struct bt_t *btphdr)
{
 register int32 i;

 if (__btqroot == NULL)
  {
   if (__debug_flg && __ev_tracing)
    __dbg_msg("--empty tree--\n");
   return;
  }
 for (i = 0; i <= __max_level; i++)
  {
   __nd_level = i;
   dmp2_btree(btphdr, 0);
  }
}

/*
 * dump a btree to standard output depth first using large linked b nodes
 */
static void dmp2_btree(struct bt_t *btphdr, int32 level)
{
 register struct bt_t *btp;

 if (level > __nd_level) return;
 dmp_btnode(btphdr, level);
 if (btphdr->bttyp == BTFRNGE) return;
 for (btp = btphdr; btp != NULL; btp = btp->btnxt)
  dmp2_btree(btp->ofsu.btofs, level + 1);
}

/*
 * dump a btree node
 * for now assume fits on one line
 */
static void dmp_btnode(struct bt_t *btp, int32 level)
{
 struct bt_t *btp1;
 int32 first_time;
 char s1[RECLEN];

 if (__nd_level != level) return;
 if (btp->bttyp == BTFRNGE) strcpy(s1, "fringe");
 else strcpy(s1, "internal");

 __outlinpos = 0;
 __dbg_msg("level %d %s node size %u:", level, s1, btp->btnfill);
 first_time = TRUE;
 for (btp1 = btp; btp1 != NULL; btp1 = btp1->btnxt)
  {
   if (btp1->bttyp == BTFRNGE)
    {
     if (first_time) first_time = FALSE; else __dbg_msg(", ");
     __dbg_msg("time %s(events %d)", __to_timstr(s1, &(btp1->btltim)),
      btp1->ofsu.telp->num_events);
    }
   else
    {
     if (first_time) first_time = FALSE; else __dbg_msg(", ");
     __dbg_msg("time %s", __to_timstr(s1, &(btp1->btltim)));
    }
  }
 __dbg_msg("\n");
 __outlinpos = 0;
}

/*
 * free subtree of overflow event queue btree
 * know always passed leftmost of multiple node node
 * and btphdr never nil, caller must check for empty tree
 */
extern void __free_btree(struct bt_t *btphdr)
{
 register struct bt_t *btp, *btp2;

 /* at bottom of tree this nodes and all right siblings fringe nodes */
 if (btphdr->bttyp == BTFRNGE)
  {
    for (btp = btphdr; btp != NULL;)
     {
      btp2 = btp->btnxt;
      /* because freeing telp record, know contents freed */
      /* events freed by just marking all of tev tab unused */
      __free_telhdr_tevs(btp->ofsu.telp);
      __my_free((char *) btp->ofsu.telp, sizeof(struct telhdr_t));
      __my_free((char *) btp, sizeof(struct bt_t));
     btp = btp2;
    }
   return;
  }
 /* if one node non fringe, all non fringe */
 for (btp = btphdr; btp != NULL;)
  {
   btp2 = btp->btnxt;
   __free_btree(btp->ofsu.btofs);
   __my_free((char *) btp, sizeof(struct bt_t));
   btp = btp2;
  }
}


/*
 * free list of tevs - either btree node or timing wheel list
 *
 * normally add entire period's events to free list - this is for reset only
 * because when an event is processed guts (if any) freed so can link on
 * free list but here need to free guts too
 */
extern void __free_telhdr_tevs(register struct telhdr_t *telp)
{
 register i_tev_ndx tevpi, tevp2i;

 for (tevpi = telp->te_hdri; tevpi != -1;)
  {
   tevp2i = __tevtab[tevpi].tenxti;
   __free_1tev(tevpi);
   tevpi = tevp2i;
  }
 /* this is needed for timing wheel since, telp not freed */
 telp->te_hdri = telp->te_endi = -1;
 telp->num_events = 0;
}

/*
 * free 1 event - may need to free auxiliary since will never be processed
 * freeing just puts on front of ev free list
 */
extern void __free_1tev(i_tev_ndx tevpi)
{
 int32 wlen;
 word32 *wp;
 struct tev_t *tevp;
 struct tenbpa_t *tenbp;
 struct tedputp_t *tedp;
 struct teputv_t *tepvp;

 tevp = &(__tevtab[tevpi]);
 switch ((byte) tevp->tetyp) {
  case TE_WIRE: case TE_BIDPATH: case TE_MIPD_NCHG:
   if (tevp->tu.tenp != NULL)
    __my_free((char *) tevp->tu.tenp, sizeof(struct tenp_t));
   break;
  case TE_NBPA:
   /* for non #0 original freed here but no tenbpa - moved to new */
   if ((tenbp = tevp->tu.tenbpa) == NULL) break;

   wp = tenbp->nbawp;
   wlen = wlen_(tenbp->nbastp->st.spra.lhsx->szu.xclen);
   __my_free((char *) wp, 2*wlen*WRDBYTES);
   /* if needed to copy lhs expr., now free */
   if (tenbp->nblhsxp != NULL) __free_xtree(tenbp->nblhsxp);
   __my_free((char *) tevp->tu.tenbpa, sizeof(struct tenbpa_t));
   break;
  case TE_TFPUTPDEL:
   if ((tedp = tevp->tu.tedputp) == NULL) break;
   tevp->tu.tedputp = NULL;
   tedp->tedtfrp =  (struct tfrec_t *) __tedpfreelst;
   __tedpfreelst = tedp;
   break;
  case TE_VPIPUTVDEL: case TE_VPIDRVDEL:
   if ((tepvp = tevp->tu.teputvp) == NULL) break;
   tevp->tu.teputvp = NULL;
   tepvp->np =  (struct net_t *) __teputvfreelst;
   __teputvfreelst = tepvp;
   break;
  /* for these either no auxiliary rec., or must stay, or free with handle */
  case TE_THRD: case TE_G: case TE_CA: case TE_TFSETDEL: case TE_SYNC:
  case TE_VPICBDEL:
   break;
  default: __case_terr(__FILE__, __LINE__);
 }

 /* ** DBG remove --
 memset(tevp, 0, sizeof(struct tev_t));
 __dbg_msg("--- free tev at %x\n", tevp);
 --- */
 __tevtab[tevpi].tenxti = __tefreelsti;
 __tefreelsti = tevpi;
}

