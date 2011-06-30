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
 * run time execution routines - statements but not expression eval
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <setjmp.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>

#include <signal.h>

#ifdef __DBMALLOC__
#include "../malloc.h"
#endif

#include "v.h"
#include "cvmacros.h"

/* local prototypes */
static void tr_resume_msg(void);
static void exec_stmts(register struct st_t *);
static void thrd_done_cleanup(register struct thread_t *);
static void brktr_exec_stmts(register struct st_t *);
static void step_exec_stmt(register struct st_t *);
static int32 stepped_to_new_loc(struct st_t *);
static void eval_tskassign_rhsexpr(register struct xstk_t *, register int32,
 register int32, register int32, register int32);
static void tr_proc_assign(struct st_t *, struct xstk_t *);
static void tr_nbproc_assign(struct st_t *, struct xstk_t *);
static struct st_t *exec_rep_ectl_setup(struct st_t *stp);
static int32 exec_dctrl(struct st_t *);
static void sched_proc_delay(struct delctrl_t *, word32 *, int32);
static void sched_nbproc_delay(struct delctrl_t *, struct xstk_t *,
 struct st_t *);
static void arm_event_dctrl(struct delctrl_t *, register word32 *, int32);
static void arm_nbevent_dctrl(struct delctrl_t *, struct xstk_t *,
 struct st_t *);
static struct st_t *exec_case(struct st_t *);
static struct st_t *exec_real_case(struct st_t *);
static void tr_case_st(struct xstk_t *, int32);
static struct st_t *exec_casex(struct st_t *);
static struct st_t *exec_casez(struct st_t *);
static int32 exec_wait(register struct st_t *);
static int32 for_not_done(struct for_t *);
static void exec_namblk(struct st_t *);
static struct thread_t *sched_fj_subthread(struct st_t *stp);
static void init_thrd(register struct thread_t *);
static void tradd_tf_argval(int32, struct net_t *, struct xstk_t *);
static void store_tskcall_outs(struct st_t *);
static void grow_fcstk(void);
static void exec_count_drivers(struct expr_t *);
static void exec_testplusargs(struct expr_t *);
static void exec_scanplusargs(struct expr_t *);
static void exec_valueplusargs(struct expr_t *);
static void exec_1arg_transcendental(int32, struct expr_t *);
static void exec_transcendental_int(struct expr_t *);
static void exec_transcendental_sign(struct expr_t *);
static void exec_transcendental_powsign(int32, struct expr_t *);
static void exec_transcendental_minmax(int32, struct expr_t *);
static void exec_transcendental_atan2(struct expr_t *);
static void exec_transcendental_hypot(struct expr_t *);
static void exec_cause(struct st_t *);
static struct thread_t *find_hgh_sametskthrd(struct thread_t *);
static int32 thread_above_cur(struct thread_t *);
static void free_thd_stuff(struct thread_t *);
static void unlink_tskthd(struct thread_t *);
static int32 chk_strobe_infloop(struct st_t *, struct sy_t *);
static void mcd_do_fclose(struct expr_t *);
static word32 bld_open_mcd(void);
static word32 mc_do_fopen(struct expr_t *);
static void do_showvars_stask(struct expr_t *);
static void do_warn_supp_chg(char *, struct expr_t *, int32);
static void do_reset(struct expr_t *);
static void do_showscopes(struct expr_t *);
static void prt_1m_scopelist(struct itree_t *);
static void prt_1tsk_scopelist(struct task_t *, int32);
static void prt_1m_nestscopes(struct itree_t *);
static void prt_1tsk_nestscopes(struct symtab_t *);
static void exec_qfull(struct expr_t *);
static void do_q_init(struct expr_t *);
static struct q_hdr_t *find_q_from_id(int32);
static void init_q(struct q_hdr_t *);
static void do_q_add(struct expr_t *);
static void do_q_remove(struct expr_t *);
static void do_q_examine(struct expr_t *);
static void cmp_mean_interarriv_tim(word64 *, struct q_hdr_t *);
static void cmp_max_wait(word64 *, struct q_hdr_t *);
static void cmp_mean_wait_tim(word64 *, struct q_hdr_t *);
static void exec_prttimscale(struct expr_t *);
static void exec_timefmt(struct expr_t *);
static int32 get_opt_starg(struct expr_t *, int32);
static void exec_log_fnamchg(struct expr_t *);
static void exec_trace_fnamchg(struct expr_t *);
static void exec_expr_schg(struct expr_t *);
static void free_thd_subtree(struct thread_t *);
static void suspend_curthd(struct st_t *);

static word32 fio_do_fopen(struct expr_t *, struct expr_t *);
static word32 fio_fopen(char *, char *);
static int32 chk_cnvt_fd_modes(char *, char *);
static void fio_do_fclose(struct expr_t *);
static int32 chk_get_mcd_or_fd(struct expr_t *, int32 *);
static void fio_fflush(struct expr_t *);
static int32 fio_ungetc(struct expr_t *, struct expr_t *);
static int32 chk_get_ver_fd(struct expr_t *);
static int32 fio_fgets(struct expr_t *, struct expr_t *);
static int32 fio_rewind(struct expr_t *);
static int32 fio_fseek(struct expr_t *, struct expr_t *, struct expr_t *);
static int32 fio_ferror(struct expr_t *, struct expr_t *);
static void fio_swrite(struct expr_t *, int32);
static void fio_sformat(struct expr_t *);
static int32 fio_fscanf(struct expr_t *);
static int32 fio_sscanf(struct expr_t *);
static int32 fio_exec_scanf(FILE *, struct expr_t *);
static int32 fio_fread(struct expr_t *);
static void fread_onto_stk(struct xstk_t *, byte *, int32);
static int32 fio_arr_fread(struct expr_t *, int32, struct expr_t *,
 struct expr_t *);
static int32 scanf_getc(FILE *);
static void scanf_ungetc(int32, FILE *);
static int32 chk_scanf_fmt(char *);
static int32 collect_scanf_num(int32 *, FILE *, int32, int32, int32);
static int32 collect_scanf_realnum(double *, FILE *, int32, int32, int32);
static struct xstk_t *collect_ufmt_binval(FILE *, struct expr_t *, int32);
static struct xstk_t *collect_zfmt_binval(FILE *, struct expr_t *, int32 );
static int32 cnvt_scanf_stnam_to_val(char *); 
extern void __str_do_disp(struct expr_t *, int32);
extern word32 __inplace_lnegate(register word32 *, int32);

/* extern prototypes (maybe defined in this module) */
extern int32 __comp_sigint_handler(void);
extern char *__my_malloc(int32);
extern char *__pv_stralloc(char *);
extern char *__my_realloc(char *, int32, int32);
extern struct xstk_t *__eval_assign_rhsexpr(register struct expr_t *,
 register struct expr_t *);
extern struct thread_t *__setup_tsk_thread(struct task_t *);
extern void __sched_fork(struct st_t *);
extern i_tev_ndx __bld_nb_tev(struct st_t *, struct xstk_t *, word64);
extern int32 __lhsexpr_var_ndx(register struct expr_t *);
extern void __eval_lhsexpr_var_ndxes(register struct expr_t *);
extern struct st_t *__brktr_exec_1stmt(struct st_t *);
extern struct thread_t *__alloc_thrd(void);
extern struct st_t *__exec_tskcall(struct st_t *);
extern struct xstk_t *__eval2_xpr(struct expr_t *);
extern struct expr_t *__copy_expr(struct expr_t *);
extern struct expr_t *__sim_copy_expr(struct expr_t *);
extern int32 __comp_ndx(register struct net_t *, register struct expr_t *);
extern struct expr_t *__bld_rng_numxpr(word32, word32, int32);
extern void __free_xtree(struct expr_t *);
extern char *__regab_tostr(char *, word32 *, word32 *, int32, int32, int32);
extern char *__xregab_tostr(char *, word32 *, word32 *, int32, struct expr_t *);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern char *__to_idnam(struct expr_t *);
extern char *__msg_blditree(char *, struct itree_t *, struct task_t *);
extern char *__msg2_blditree(char *, struct itree_t *);
extern char *__bld_lineloc(char *, word32, int32);
extern char *__to_timunitnam(char *, word32);
extern char *__to_timstr(char *, word64 *);
extern char *__to_tetyp(char *, word32); 
extern char *__to_tsktyp(char *, word32);
extern double __unscale_realticks(word64 *, struct mod_t *);
extern FILE *__tilde_fopen(char *, char *);
extern struct xstk_t *__ld_wire_driver(register struct net_pin_t *);
extern struct xstk_t *__ld_stwire_driver(register struct net_pin_t *);
extern int32 __has_vpi_driver(struct net_t *, struct net_pin_t *);
extern char *__schop(char *, char *);
extern char *__to_dcenam(char *, word32);
extern struct xstk_t *__cstr_to_vval(char *);
extern struct task_t *__getcur_scope_tsk(void);
extern word32 __mc1_fopen(char *, int32, int32);
extern word32 __close_mcd(word32, int32);
extern void __wrap_puts(char *, FILE *);
extern void __wrap_putc(int32, FILE *);
extern void __evtr_resume_msg(void);
extern void __do_iact_disable(struct hctrl_t *, int32);
extern void __dmp_thrd_info(struct thread_t *);
extern void __dmp_tskthd(struct task_t *, struct mod_t *);
extern void __my_free(char *, int32);
extern void __exec2_proc_assign(struct expr_t *, register word32 *,
 register word32 *);
extern int32 __cvt_lngbool(word32 *, word32 *, int32);
extern int32 __wide_vval_is0(register word32 *, int32);
extern void __exec_qc_assign(struct st_t *, int32);
extern void __exec_qc_wireforce(struct st_t *);
extern void __exec_qc_deassign(struct st_t *, int32);
extern void __exec_qc_wirerelease(struct st_t *);
extern int32 __process_brkpt(struct st_t *);
extern void __prt_src_lines(int32, int32, int32);
extern void __cnv_stk_fromreg_toreal(struct xstk_t *, int32);
extern void __cnv_stk_fromreal_toreg32(struct xstk_t *);
extern void __sizchgxs(struct xstk_t *, int32);  
extern void __narrow_to1wrd(register struct xstk_t *);
extern void __narrow_sizchg(register struct xstk_t *, int32);
extern void __sizchg_widen(register struct xstk_t *, int32);
extern void __sgn_xtnd_wrd(register struct xstk_t *, int32);
extern void __sgn_xtnd_widen(struct xstk_t *, int32);
extern void __dmp_proc_assgn(FILE *, struct st_t *, struct delctrl_t *, int32);
extern void __trunc_exprline(int32, int32);
extern void __dmp_nbproc_assgn(FILE *, struct st_t *, struct delctrl_t *);
extern void __dmp_dctrl(FILE *, struct delctrl_t *);
extern void __chg_xprline_size(int32);
extern void __dmp_dcxpr(FILE *, union del_u, word32);
extern void __get_del(register word64 *, register union del_u, word32);
extern void __insert_event(register i_tev_ndx);
extern void __dmp_forhdr(FILE *, struct for_t *);
extern void __add_ev_to_front(register i_tev_ndx);
extern void __dmp_tskcall(FILE *, struct st_t *);
extern void __xmrpush_refgrp_to_targ(struct gref_t *);
extern void __adds(char *);
extern void __chg_st_val(struct net_t *, register word32 *, register word32 *);
extern void __st_val(struct net_t *, register word32 *, register word32 *);
extern void __grow_xstk(void);
extern void __chg_xstk_width(struct xstk_t *, int32);
extern void __grow_tevtab(void);
extern void __ld_wire_val(register word32 *, register word32 *, struct net_t *);
extern void __do_interactive_loop(void);
extern void __cnv_ticks_tonum64(word64 *, word64, struct mod_t *);
extern void __exec_sfrand(struct expr_t *);
extern void __exec_scale(struct expr_t *);
extern void __pli_func_calltf(struct expr_t *);
extern void __vpi_sysf_calltf(struct expr_t *);
extern void __get_bidnpp_sect(struct net_t *, struct net_pin_t *, int32 *,
 int32 *);
extern char *__get_eval_cstr(struct expr_t *, int32 *);
extern void __free_1thd(struct thread_t *);
extern struct st_t *__exec_stsk(struct st_t *, struct sy_t *,
 struct tskcall_t *);
extern void __free_thd_list(struct thread_t *);
extern int32 __exec_disable(struct expr_t *);
extern void __do_disp(register struct expr_t *, int32);
extern void __fio_do_disp(register struct expr_t *, int32, int32, char *); 
extern void __start_fmonitor(struct st_t *);
extern void __dmpmod_nplst(struct mod_t *, int32);
extern void __start_monitor(struct st_t *);
extern void __exec_readmem(struct expr_t *, int32);
extern void __exec_sreadmem(struct expr_t *, int32);
extern void __exec_dumpvars(struct expr_t *);
extern int32 __get_eval_word(struct expr_t *, word32 *);
extern void __exec_input_fnamchg(struct expr_t *);
extern void __exec_history_list(int32);
extern void __do_scope_list(void);
extern void __exec_sdf_annotate_systsk(struct expr_t *);
extern void __call_misctfs_finish(void);
extern void __vpi_endsim_trycall(void);
extern void __emit_stsk_endmsg(void);
extern void __maybe_open_trfile(void);
extern void __escape_to_shell(char *);
extern void __write_snapshot(int32);
extern void __prt2_mod_typetab(int32);
extern void __pli_task_calltf(struct st_t *);
extern void __vpi_syst_calltf(struct st_t *);
extern void __my_fclose(FILE *);
extern void __emit_1showvar(struct net_t *, struct gref_t *);
extern void __prt_top_mods(void);
extern void __disp_itree_path(struct itree_t *, struct task_t *);
extern void __set_scopchg_listline(void);
extern void __call_misctfs_scope(void);
extern void __vpi_iactscopechg_trycall(void);
extern void __my_ftime(time_t *, time_t *);
extern void __prt_end_msg(void);
extern void __exec_dist_uniform(struct expr_t *);
extern void __exec_dist_stdnorm(struct expr_t *);
extern void __exec_dist_exp(struct expr_t *);
extern void __exec_dist_poisson(struct expr_t *);
extern void __exec_chi_square(struct expr_t *);
extern void __exec_dist_t(struct expr_t *);
/* ??? extern void __dmp_event_tab(void); */
extern void __my_dv_flush(void);
extern void __add_nchglst_el(register struct net_t *);
extern void __add_dmpv_chglst_el(struct net_t *);
extern void __wakeup_delay_ctrls(register struct net_t *, register int32,
 register int32);
extern void __dmp_all_thrds(void);
extern double __cnvt_stk_to_real(struct xstk_t *, int32);
extern int32 __enum_is_suppressable(int32);
extern int32 __trim1_0val(word32 *, int32);
extern char *__vval_to_vstr(word32 *, int32, int32 *);
extern void __vstr_to_vval(word32 *, char *, int32);
extern int32 __is_vdigit(int32, int32);
extern void __to_dhboval(int32, int32);
extern double __my_strtod(char *, char **, int32 *);
extern void __add_pnd0_nonblk_list(i_tev_ndx);

extern struct expr_t *__disp_1fmt_to_exprline(char *, struct expr_t *);
extern void __getarr_range(struct net_t *, int32 *, int32 *, int32 *);
extern void __st_arr_val(union pck_u, int32, int32, int32, register word32 *,
 register word32 *);
extern void __chg_st_arr_val(union pck_u, int32, int32, int32,
 register word32 *, register word32 *);
extern int32 __fd_do_fclose(int32);
extern void __add_select_nchglst_el(register struct net_t *, register int32,
 register int32);

extern void __tr_msg(char *, ...);
extern void __cv_msg(char *, ...);
extern void __cvsim_msg(char *, ...);
extern void __sgfwarn(int32, char *, ...);
extern void __sgferr(int32, char *, ...);
extern void __dbg_msg(char *, ...);
extern void __sgfinform(int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);
extern void __misc_sgfterr(char *, int32);
extern void __my_exit(int32, int32);
extern void __my_fprintf(FILE *, char *, ...);

/* reset mechanism long jump buffer */
extern jmp_buf __reset_jmpbuf;

/* system stuff */
extern int32 errno;

/* some general evaluation tables */
word32 __masktab[] = {
 /* since 0 is the same as all used, mask must be entire word32 */
 0xffffffffL, 0x00000001L, 0x00000003L, 0x00000007L,
 0x0000000fL, 0x0000001fL, 0x0000003fL, 0x0000007fL,
 0x000000ffL, 0x000001ffL, 0x000003ffL, 0x000007ffL,
 0x00000fffL, 0x00001fffL, 0x00003fffL, 0x00007fffL,
 0x0000ffffL, 0x0001ffffL, 0x0003ffffL, 0x0007ffffL,
 0x000fffffL, 0x001fffffL, 0x003fffffL, 0x007fffffL,
 0x00ffffffL, 0x01ffffffL, 0x03ffffffL, 0x07ffffffL,
 0x0fffffffL, 0x1fffffffL, 0x3fffffffL, 0x7fffffffL,
 /* special for places where mask uses length i.e. 32 bits */
 0xffffffffL
};

extern double __dbl_toticks_tab[];

/*
 * ROUTINES TO PROCESS PROCEDURAL EVENTS AND EXECUTE BEHAVIORAL STATEMENTS
 */

/*
 * execute a control thread from one event suspension until next
 * need to handle rhs delay control and => proc. assignment
 *
 * when thread completes just removes and continues with other threads
 * know if this suspends or hits ctrl-c will always build and schedule new ev
 * possible for thread next statement to be nil to terminate thread 
 * and here must be left and terminated after suspend
 */
extern void __process_thrd_ev(register struct tev_t *tevp)
{
 register struct st_t *stp;
 struct st_t *stp2;
 struct thread_t *parthp;

 __proc_thrd_tevents++;
 __suspended_thd = NULL;
 __suspended_itp = NULL;
 /* set current thread and remove connection of thread to event */
 __cur_thd = tevp->tu.tethrd;
 __cur_thd->thdtevi = -1;

 /* if not func. must have change itree to right one for thread */
 /* NO - this will not be be true if invoked xmr task - inst ptr. diff */  
 /* but will be put back when xmr task done so ok */ 
 /* DBG remove -- 
 if (__fcspi == -1 && __cur_thd->th_itp != __inst_ptr)
  __misc_terr(__FILE__, __LINE__);
 --- */
 stp = __cur_thd->thnxtstp;

 /* possible to remove thread even though no more statements to exec */
 if (stp != NULL && (__st_tracing || __ev_tracing))
  {
   __slin_cnt = stp->stlin_cnt;
   __sfnam_ind = stp->stfnam_ind;

   if (__st_tracing) tr_resume_msg(); else __evtr_resume_msg();
   __tr_msg("-- resuming at statement %s\n",
    __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt));
  }

 /* for each completed thread continue in parent without schd */
 /* loop because continues until thread tree done or suspend */
 for (__stmt_suspend = FALSE;;)
  {
   /* keep executing behavioral stmts until done or hit timing control */
   if (stp != NULL) 
    {
     /* even if single stepping must not see iact threads */
     /* since this always either hits end of thread or suspends */
     if (__single_step && __cur_thd->th_hctrl == NULL)
      {
       step_exec_stmt(stp);
      }
     /* but batch tracing traces */
     else if (__st_tracing) brktr_exec_stmts(stp);
     else exec_stmts(stp);

     /* on suspend event itree location is right for exec */
     /* if no suspend but current ctrl thread (init/always/task) got to end */ 
     /* fall thru and try to immediately exec parent */
     if (__stmt_suspend) break;
    }

   /* DBG remove --- */
   if (__cur_thd->thdtevi != -1) __misc_terr(__FILE__, __LINE__);
   /* --- */
   /* this thread tree done if nil - can only be interactive or init/always */
   /* this handles all freeing because entire thread tree done */
   if ((parthp = __cur_thd->thpar) == NULL)
    {
     /* if interactive thread - free and set possible history disabled */
     if (__cur_thd->th_hctrl != NULL)
      __do_iact_disable(__cur_thd->th_hctrl, FALSE);
     __stmt_suspend = TRUE;
     break;
    }

   /* know if task has outs will always have parent */
   /* store parameters if needed */
   if (__cur_thd->tsk_stouts)
    { 
     /* DBG remove --- */ 
     if (!parthp->th_postamble) __misc_terr(__FILE__, __LINE__);
     /* --- */
     /* if disabled do not store parameters, but still adjust nxt stp */
     /* not parent must be set to continue at tsk call for storing outs */
     if (!__cur_thd->th_dsable) store_tskcall_outs(parthp->thnxtstp);
 
     /* SJM 08/18/02 - must fixup including skip of non loop end gotos */
     /* now that store of tsk outs finished */
     stp2 = parthp->thnxtstp;
     if (stp2 != NULL) stp2 = stp2->stnxt;
     if (stp2 == NULL) parthp->thnxtstp = NULL;
     else if (stp2->stmttyp != S_GOTO) parthp->thnxtstp = stp2;
     else if (stp2->lpend_goto) parthp->thnxtstp = stp2;
     else
      {
       for (;;)
        {
         /* know here stp2 is non loop end goto - moves to goto first */
         if ((stp2 = stp2->st.sgoto) == NULL || stp2->stmttyp != S_GOTO)
          { parthp->thnxtstp = stp2; break; }
         if (stp2->lpend_goto) { parthp->thnxtstp = stp2; break; }
        }
      }
     /* ??? REPLACED parthp->thnxtstp = parthp->thnxtstp->stnxt; */
     parthp->th_postamble = FALSE;
    }   

   /* DBG remove --- */
   if (__cur_thd->th_ialw) __misc_terr(__FILE__, __LINE__); 
   if (parthp->thofscnt == 0) __misc_terr(__FILE__, __LINE__);
   if (__debug_flg)
    { __dbg_msg("*** thread finished:\n"); __dmp_thrd_info(__cur_thd); }
   /* --- */

   /* this thread finished - remove it from control thread d.s. */
   thrd_done_cleanup(parthp);

   /* more fork-join subthreads to complete */
   if (parthp->thofscnt > 0) { __stmt_suspend = TRUE; break; }

   /* all subthreads finished, continue with parent */
   /* for enabled task (not named block), know out arg. store phase done */
   parthp->thofs = NULL;
   /* continue with parent by executing next statement */
   /* no suspend here */
   __cur_thd = parthp;
   __pop_itstk();
   __push_itstk(__cur_thd->th_itp);
   stp = __cur_thd->thnxtstp;
  }
 /* DBG remove
 if (!__stmt_suspend) __misc_terr(__FILE__, __LINE__);
 --- */
 /* only have current thread when evaling thread event */
 __cur_thd = NULL;
}


/*
 * routine to clean up linked thread control structure after thread done
 *
 * thread finished - clean up and try to continue in parent
 * this removes various connected stuff but leave thread fields
 *
 * when done no current thread caller must set if needed
 */
static void thrd_done_cleanup(register struct thread_t *parthp)
{
 free_thd_stuff(__cur_thd);

 /* move up and continue in parent */
 parthp->thofscnt -= 1;
 /* one thread of fork/join done - link it out after redundant cnt dec */
 if (__cur_thd->thleft != NULL)
  __cur_thd->thleft->thright = __cur_thd->thright;
 /* adjust parent's thread ofset if removing first in list */
 else parthp->thofs = __cur_thd->thright;

 if (__cur_thd->thright != NULL)
  __cur_thd->thright->thleft = __cur_thd->thleft;
 /* free stuff already removed and events canceled so just free */
 __my_free((char *) __cur_thd, sizeof(struct thread_t));
 __cur_thd = NULL; 

 /* RELEASE remove ---
 if (parthp->thofscnt == 1)
  {
   if (parthp->thofs->thright != NULL
    || parthp->thofs->thleft != NULL) __misc_terr(__FILE__, __LINE__);
  }
 --- */
}

/*
 * print out trace location and time states
 *
 * no leading new line may need to have separate trace file if user output
 * leaves unfinished lines.
 *
 * for statement tracing only change file name when module changes
 * so line number will be in same * file
 */
static void tr_resume_msg(void)
{
 char s1[RECLEN], s2[RECLEN];

 if (__inst_ptr != __last_tritp)
  {
   __tr_msg("==> tracing in %s (%s) line %s\n",
    __msg2_blditree(s1, __inst_ptr), __inst_ptr->itip->imsym->synam,
    __bld_lineloc(s2, (word32) __sfnam_ind, __slin_cnt));
   __last_tritp = __inst_ptr;
  }
 if (__last_trtime != __simtime)
  {
   /* this should go through time format ? */
   __tr_msg("<<< tracing at time %s\n", __to_timstr(s1, &__simtime));
   __last_trtime = __simtime;
  }
}

/*
 * execute statement list
 * called from thrd event processing routine and return when blocked or done
 * execute until fall off end (thread done) or schedule wake up event
 */
static void exec_stmts(register struct st_t *stp)
{
 register word32 val;
 register struct xstk_t *xsp;
 int32 tmp, wlen;
 struct st_t *stp2;
 struct for_t *forp;
 struct expr_t *cntx;

 /* notice one pass through loop executes exactly 1 statement */
 for (;;)
  {
   __slin_cnt = stp->stlin_cnt;
   __sfnam_ind = stp->stfnam_ind;
   __num_execstmts++;
   /* DBG remove --
   if (__cur_thd == NULL || __cur_thd->th_itp != __inst_ptr)
    __misc_terr(__FILE__, __LINE__);
   --- */

   switch ((byte) stp->stmttyp) {
    /* SJM - 02/08/02 - should not count empties as exec stmts */
    case S_NULL: case S_STNONE: __num_execstmts--; break;
    case S_FORASSGN:
     __num_addedexec++; 
     /* FALLTHRU */
    case S_PROCA:
     xsp = __eval_assign_rhsexpr(stp->st.spra.rhsx, stp->st.spra.lhsx);
     __exec2_proc_assign(stp->st.spra.lhsx, xsp->ap, xsp->bp);
     __pop_xstk();
     break;
    case S_NBPROCA:
     /* only non delay form non blocking assign exec here - implied #0 */
     xsp = __eval_assign_rhsexpr(stp->st.spra.rhsx, stp->st.spra.lhsx);
     sched_nbproc_delay((struct delctrl_t *) NULL, xsp, stp);
     __pop_xstk();
     break;
    case S_RHSDEPROCA:
     /* notice this statement never executed directly - delctrl execed */ 
     /* then after block - results execed here */
     wlen = wlen_(stp->st.spra.lhsx->szu.xclen);
     /* know rhs width here same as lhs width */
     __exec2_proc_assign(stp->st.spra.lhsx, __cur_thd->th_rhswp,
      &(__cur_thd->th_rhswp[wlen]));
     /* must reset and free pending saved rhs over schedule */
     __my_free((char *) __cur_thd->th_rhswp, 2*wlen*WRDBYTES);
     __cur_thd->th_rhswp = NULL;
     __cur_thd->th_rhswlen = -1;
     __cur_thd->th_rhsform = FALSE;
     break;
    case S_IF:
     xsp = __eval_xpr(stp->st.sif.condx);
     /* condition T (non zero) only if at least 1, 1 */
     if (xsp->xslen <= WBITS)
      {
       /* SJM 07/20/00 - must convert to real if real */
       if (stp->st.sif.condx->is_real)
        {
         double d1;
 
         memcpy(&d1, xsp->ap, sizeof(double));
         tmp = (d1 != 0.0);
        }
       else tmp = ((xsp->ap[0] & ~xsp->bp[0]) != 0L);
      }
     else tmp = (__cvt_lngbool(xsp->ap, xsp->bp, wlen_(xsp->xslen)) == 1);
     __pop_xstk();
     if (tmp) stp = stp->st.sif.thenst;
     else if (stp->st.sif.elsest != NULL) stp = stp->st.sif.elsest;
     else stp = stp->stnxt;
     goto nxt_stmt;
    case S_CASE:
     /* notice Verilog cases cannot fall thru */
     if ((stp2 = exec_case(stp)) == NULL) break;
     stp = stp2;
     goto nxt_stmt;
    case S_FOREVER: stp = stp->st.swh.lpst; goto nxt_stmt;
    case S_REPSETUP:
     /* know repeat stmt follows rep setup */
     __num_addedexec++; 
     cntx = stp->stnxt->st.srpt.repx;
     xsp = __eval_xpr(cntx);
     /* SJM 04/02/02 - real count must be converted to word/int32 */
     if (cntx->is_real) __cnv_stk_fromreal_toreg32(xsp);
     /* SJM 12/05/04 - ### ??? FIXME - what if wide and low word good? */
     if (xsp->xslen > WBITS) __narrow_to1wrd(xsp);
     if (xsp->ap[1] != 0L)
      {
       __sgfwarn(645,
        "repeat loop in %s count has x/z expression value - loop skipped",
        __msg_blditree(__xs, __cur_thd->th_itp, __cur_thd->assoc_tsk));
       val = 0;
      }
     else
      {
       /* SJM 04/02/02 - if repeat count signed and negative, never exec */ 
       if (cntx->has_sign && ((int32) xsp->ap[0]) <= 0) val = 0;
       else val = xsp->ap[0];
      }
     __pop_xstk();
     /* notice count must be converted to word32 with neg set to 0 */
     /* set to 0 so after inced here and initial repeat exec dec, */
     stp->stnxt->st.srpt.reptemp[__inum] = ++val;
     break;
    case S_REPEAT:
     if ((val = --(stp->st.srpt.reptemp[__inum])) == 0L) break; 
     stp = stp->st.srpt.repst;
     goto nxt_stmt;
    case S_WHILE:
     xsp = __eval_xpr(stp->st.swh.lpx);
     if (xsp->xslen <= WBITS)
      {
       /* SJM 07/20/00 - must convert to real if real */
       if (stp->st.swh.lpx->is_real)
        {
         double d1;
  
         memcpy(&d1, xsp->ap, sizeof(double));
         __pop_xstk();
         /* must not emit informs from val if real */
         if (d1 != 0.0) { stp = stp->st.swh.lpst; goto nxt_stmt; }
         break;
        }
       val = xsp->bp[0];
       if ((xsp->ap[0] & ~val) != 0L)
        {
         if (val != 0) 
          {
           __sgfinform(403, "while in %s condition true but some bits x/z",
            __msg_blditree(__xs, __cur_thd->th_itp, __cur_thd->assoc_tsk));
          }
         __pop_xstk();
         stp = stp->st.swh.lpst;
         goto nxt_stmt;
        }
       /* notice any 1 implies true so will not get here */
       if (val != 0)
        {
         __sgfinform(402,
          "while loop in %s terminating false condition value has x/z bits",
          __msg_blditree(__xs, __cur_thd->th_itp, __cur_thd->assoc_tsk));
        }
       __pop_xstk();
       break;
      } 
     if ((tmp = __cvt_lngbool(xsp->ap, xsp->bp, wlen_(xsp->xslen))) == 1)
      {
       if (!vval_is0_(xsp->bp, xsp->xslen))
        {
         __sgfinform(403, "while condition in %s true but some bits x/z",
          __msg_blditree(__xs, __cur_thd->th_itp, __cur_thd->assoc_tsk));
        }
       __pop_xstk();
       stp = stp->st.swh.lpst;
       goto nxt_stmt;
      }
     __pop_xstk();
     /* notice any 1 implies true so will not get here */
     if (tmp == 3)
      {
       __sgfinform(402,
        "while loop terminating false condition in %s value has x/z bits",
        __msg_blditree(__xs, __cur_thd->th_itp, __cur_thd->assoc_tsk));
     }
     break;
    case S_WAIT:
     /* on true expression, returns true */
     if (exec_wait(stp)) { stp = stp->st.swait.lpst; goto nxt_stmt; }
     /* is this unnecessary since action stmt points back to wait */
     __cur_thd->thnxtstp = stp;
     __stmt_suspend = TRUE;
     return;
    case S_FOR:
     /* when loop done, for returns NULL as next stmt else 1st body st. */
     forp = stp->st.sfor;
     /* F when done */
     if (!for_not_done(forp))
      {
       break;
      }
     stp = forp->forbody;
     goto nxt_stmt;
    case S_REPDCSETUP:
     stp = exec_rep_ectl_setup(stp);
     goto nxt_stmt;
    case S_DELCTRL:
     /* this returns F, for suspend, non blocking returns T */
     /* 10/27/00 SJM - for repeat rhs ectrl count x/z <= 0 assign */
     /* immediate so also return T */
     if (exec_dctrl(stp)) { stp = __cur_thd->thnxtstp; goto nxt_stmt; }
     __stmt_suspend = TRUE;
     return;
    case S_NAMBLK:
     /* for function only, just continue in named block */
     if (__fcspi >= 0) { stp = stp->st.snbtsk->tskst; goto nxt_stmt; }
     exec_namblk(stp);
     stp = __cur_thd->thnxtstp;
     goto nxt_stmt;
    case S_UNBLK:
     stp = stp->st.sbsts;
     goto nxt_stmt;
    case S_UNFJ:
     /* this is unnamed fork-join only */ 
     __sched_fork(stp);
     __cur_thd->thnxtstp = stp->stnxt;
     __stmt_suspend = TRUE;
     return;
    case S_TSKCALL:
     /* if system task, NULL will suspend, else continue in down thread */
     if ((stp2 = __exec_tskcall(stp)) == NULL) return;
     stp = stp2;
     goto nxt_stmt;
    case S_QCONTA:
     if (stp->st.sqca->qcatyp == ASSIGN) __exec_qc_assign(stp, FALSE);
     else
      {
       /* force of reg, is like assign except overrides assign */
       if (stp->st.sqca->regform) __exec_qc_assign(stp, TRUE);
       else __exec_qc_wireforce(stp);
      }
     break;
    case S_QCONTDEA:
     if (stp->st.sqcdea.qcdatyp == DEASSIGN) __exec_qc_deassign(stp, FALSE);
     else
      {
       if (stp->st.sqcdea.regform) __exec_qc_deassign(stp, TRUE);
       else __exec_qc_wirerelease(stp);
      } 
     break;
    case S_CAUSE:
     exec_cause(stp);
     break;
    case S_DSABLE:
     /* if function, disable means continue with statement after block */ 
     /* if disable of func. next statement is nil, so done with func. */
     if (__fcspi >= 0) { stp = stp->st.sdsable.func_nxtstp; goto nxt_stmt; }

     if (__exec_disable(stp->st.sdsable.dsablx)) goto thread_done;
     /* disable elsewhere in control tree means just continue here */
     break;
    case S_GOTO:
     stp = stp->st.sgoto;
     /* notice goto of nil, ok just means done */ 
     __num_addedexec++; 
     goto nxt_stmt;
    case S_BRKPT:
     /* returns T on need to break */
     if (__process_brkpt(stp)) goto nxt_stmt;

     /* not a break for some reason - restore stmt type and exec 1 stmt */
     /* if bp halt off 2nd time through after break, this execs */
     stp->stmttyp = stp->rl_stmttyp;
     /* execute the broken on stmt */
     stp2 = __brktr_exec_1stmt(stp);
     /* put back break pt. and make returned next stp as stp */
     stp->stmttyp = S_BRKPT;
     /* if nil will check to see if suspend or end of thread */
     stp = stp2;
     goto nxt_stmt;
    default: __case_terr(__FILE__, __LINE__);
   }
   stp = stp->stnxt;
nxt_stmt:
   if (stp == NULL) break;
   /* entry from exec of interactive command only if ctrl c hit */
   if (__pending_enter_iact)  
    { __stmt_suspend = TRUE; suspend_curthd(stp); return; }
  }
 /* when done with current function just return */
 if (__stmt_suspend || __fcspi >= 0) return;
thread_done:
 __stmt_suspend = FALSE;
 __cur_thd->thnxtstp = NULL;
}

/*
 * tracing and break point processing version of exec statements
 * called from thrd event processing routine and return when blocked or done
 * execute until fall off end (thread done) or schedule wake up event
 */
static void brktr_exec_stmts(register struct st_t *stp)
{
 /* notice one pass through loop executes exactly 1 statement */
 for (;;)
  {
   /* here if nil returned force suspend - exec set thread next statement */
   stp = __brktr_exec_1stmt(stp);
   if (stp == NULL) break;
   /* if done with thread, will detect enter iact flag in higher routine */ 
   if (__pending_enter_iact)  
    { __stmt_suspend = TRUE; suspend_curthd(stp); return; }
  }
 if (__stmt_suspend) return;
 __cur_thd->thnxtstp = NULL;
}

/*
 * exec statements while stepping
 * special case if break hit in here
 * called from thrd event processing routine and return when blocked or done
 * execute until fall off end (thread done) or schedule wake up event
 */
static void step_exec_stmt(register struct st_t *stp)
{
 /* notice one pass through loop executes exactly 1 statement */
 for (;;)
  {
   /* if step command when iact entered from iact thread or ^c step to 1st */
   /* of new thread not one more statement */
   if (__step_from_thread) stp = __brktr_exec_1stmt(stp);
   else __step_from_thread = TRUE;

   /* if hit break point while stepping, disable stepping and return */
   /* suspend already done */
   if (__pending_enter_iact && __iact_reason == IAER_BRKPT)
    {
     __single_step = FALSE;
     __step_rep_cnt = 0;  
     __step_match_itp = NULL; 
     __verbose_step = FALSE;
     /* since rexec stmt, must have current thread */
     /* DBG remove --- */
     if (stp == NULL || __cur_thd == NULL) __misc_terr(__FILE__, __LINE__);
     /* --- */
     __last_stepitp = __cur_thd->th_itp;
     __last_steptskp = __cur_thd->assoc_tsk;
     __last_stepifi = (int32) stp->stfnam_ind;
     __step_lini = stp->stlin_cnt;
     __step_ifi = (int32) stp->stfnam_ind;
     /* must suspend */
     __stmt_suspend = TRUE;
     suspend_curthd(stp);
     return;
    }
   /* must exit loop since done with this thread */
   if (stp == NULL)
    {
     if (!__stmt_suspend)
      { __step_lini = -1; __step_ifi = -1; __cur_thd->thnxtstp = NULL; }
     return;
    }

   /* if istep (within cur. itree inst. only) continue if different */
   if (__step_match_itp != NULL && __inst_ptr != __step_match_itp)
    continue;
   /* in same instance, make sure move to next line - keep exec ing */
   if (stp->stlin_cnt == __step_lini && (int32) stp->stfnam_ind == __step_ifi)
    continue;
   /* hit step point, need to enter iact */
   break;
  }
 /* hit step stop know step non nil, suspend and return */
 /* set current step line in case in loop - most move to next line */
 __step_lini = stp->stlin_cnt;
 __step_ifi = (int32) stp->stfnam_ind;
 /* stepped to something to stop at */
 if (stepped_to_new_loc(stp))
  {
   __last_stepitp = __inst_ptr;
   __last_steptskp = __cur_thd->assoc_tsk;
   __last_stepifi = (int32) stp->stfnam_ind; 

    /* FIXME - is this __tr_s tracing ??? */
   __cvsim_msg("%s (%s line %d)", __msg_blditree(__xs, __last_stepitp,
    __last_steptskp), __in_fils[__last_stepifi], stp->stlin_cnt);
   if (__last_brktime != __simtime)
    {
     __cvsim_msg(" time %s\n", __to_timstr(__xs, &__simtime));
     __last_brktime = __simtime;
    }
   else __cvsim_msg("\n");
  }
 /* notice only change list location if print */
 if (__verbose_step)
  __prt_src_lines((int32) stp->stfnam_ind, stp->stlin_cnt, stp->stlin_cnt);
 __single_step = FALSE;
 /* if more stepping, continue using istep itp matching if needed */
 if (__step_rep_cnt <= 1) __step_match_itp = NULL; 
 __verbose_step = FALSE;
 suspend_curthd(stp);
 /* even if interrupt (^c) received, doing again does not hurt */
 signal(SIGINT, SIG_IGN);
 /* when execing interactive command, never single stepped */
 __pending_enter_iact = TRUE;
 __iact_reason = IAER_STEP;  
}

/*
 * return T if stepped to new scope or new file
 */
static int32 stepped_to_new_loc(struct st_t *stp)
{
 if (__last_stepitp != __inst_ptr
  || __last_steptskp != __cur_thd->assoc_tsk
  || __last_stepifi != (int32) stp->stfnam_ind
  || __last_brktime != __simtime) return(TRUE); 
 return(FALSE);
}

/*
 * break point and tracing version of execute one statement
 * also for executing non delay interactive statements
 */
extern struct st_t *__brktr_exec_1stmt(struct st_t *stp)
{
 register word32 val;
 int32 tmp, wlen;
 struct st_t *stp2;
 struct xstk_t *xsp;
 struct for_t *forp;
 struct if_t *ifinfo;
 struct expr_t *cntx;

again:
 /* notice must set location here - few cases where more than 1 stmt here */
 __slin_cnt = stp->stlin_cnt;
 __sfnam_ind = stp->stfnam_ind;
 __num_execstmts++;
 switch ((byte) stp->stmttyp) {
  case S_NULL: case S_STNONE: break;
  case S_FORASSGN:
   xsp = __eval_assign_rhsexpr(stp->st.spra.rhsx, stp->st.spra.lhsx);
   __exec2_proc_assign(stp->st.spra.lhsx, xsp->ap, xsp->bp);
   if (__st_tracing) tr_proc_assign(stp, xsp);
   __pop_xstk();
   stp = stp->stnxt;
   __num_addedexec++; 
   __num_execstmts++;
   goto again;
  case S_PROCA:
   xsp = __eval_assign_rhsexpr(stp->st.spra.rhsx, stp->st.spra.lhsx);
   __exec2_proc_assign(stp->st.spra.lhsx, xsp->ap, xsp->bp);
   if (__st_tracing) tr_proc_assign(stp, xsp);
   __pop_xstk();
   break;
  case S_NBPROCA:
   /* only non delay form non blocking assign exec here - implied #0 */
   xsp = __eval_assign_rhsexpr(stp->st.spra.rhsx, stp->st.spra.lhsx);
   if (__st_tracing) tr_nbproc_assign(stp, xsp);
   sched_nbproc_delay((struct delctrl_t *) NULL, xsp, stp);
   __pop_xstk();
   break;
  case S_RHSDEPROCA:
   /* this is continuation point for rhs form after block */
   wlen = wlen_(stp->st.spra.lhsx->szu.xclen);
   __exec2_proc_assign(stp->st.spra.lhsx, __cur_thd->th_rhswp,
    &(__cur_thd->th_rhswp[wlen]));
   if (__st_tracing)
    {
     /* here delay statement already displayed */
     __tr_msg("trace: %-7d %s = [%s] (saved rhs assign)\n", __slin_cnt,
      __msgexpr_tostr(__xs, stp->st.spra.lhsx),
      __xregab_tostr(__xs2, __cur_thd->th_rhswp, &(__cur_thd->th_rhswp[wlen]),
      stp->st.spra.lhsx->szu.xclen, stp->st.spra.rhsx));
    }
   /* must reset and free pending saved rhs over schedule */
   __my_free((char *) __cur_thd->th_rhswp, 2*wlen*WRDBYTES);
   __cur_thd->th_rhswp = NULL;
   __cur_thd->th_rhswlen = -1;
   __cur_thd->th_rhsform = FALSE;
   break;
  case S_IF:
   ifinfo = &(stp->st.sif);
   xsp = __eval_xpr(ifinfo->condx);
   /* condition T (1) only if at least 1, 1 */
   if (xsp->xslen <= WBITS)
    {
     /* SJM 07/20/00 - must convert to real if real */
     if (ifinfo->condx->is_real)
      {
       double d1;

       memcpy(&d1, xsp->ap, sizeof(double));
       tmp = (d1 != 0.0);
      }
     else tmp = ((xsp->ap[0] & ~xsp->bp[0]) != 0L);
    }
   else tmp = (__cvt_lngbool(xsp->ap, xsp->bp, wlen_(xsp->xslen)) == 1);
   __pop_xstk();
   if (__st_tracing)
    __tr_msg("trace: %-7d if (%s) [cond %d]\n", __slin_cnt,
     __msgexpr_tostr(__xs, ifinfo->condx), tmp);
   if (tmp) stp = ifinfo->thenst;
   else if (ifinfo->elsest != NULL) stp = ifinfo->elsest;
   else stp = stp->stnxt;
   return(stp);
  case S_CASE:
   /* notice Verilog cases cannot fall thru */
   if ((stp2 = exec_case(stp)) == NULL) break;
   return(stp2);
  case S_FOREVER:
   if (__st_tracing) __tr_msg("trace: %-7d forever\n", __slin_cnt);
   return(stp->st.swh.lpst);
  case S_REPSETUP:
   /* know repeat stmt follows rep setup */
   __num_addedexec++; 
   cntx = stp->stnxt->st.srpt.repx;
   xsp = __eval_xpr(cntx);
   /* SJM 04/02/02 - real count must be converted to word/int32 */
   if (cntx->is_real) __cnv_stk_fromreal_toreg32(xsp);
   if (xsp->xslen > WBITS) __narrow_to1wrd(xsp);

   if (xsp->ap[1] != 0L)
    {
     __last_stepitp = __cur_thd->th_itp;
     __last_steptskp = __cur_thd->assoc_tsk;
     __sgfwarn(645,
      "repeat loop in %s count has x/z expression value - loop skipped",
      __msg_blditree(__xs, __cur_thd->th_itp, __cur_thd->assoc_tsk));
     val = 0;
    }
   else
    {
     /* SJM 04/02/02 - if repeat count signed and negative, never exec */ 
     if (cntx->has_sign && ((int32) xsp->ap[0]) <= 0) val = 0;
     else val = xsp->ap[0];
    }
   __pop_xstk();
   /* notice count must be converted to word32 with neg set to 0 */
   stp->stnxt->st.srpt.reptemp[__inum] = ++val;
   break;
  case S_REPEAT:
   val = --(stp->st.srpt.reptemp[__inum]); 
   if (__st_tracing)
    {
     __tr_msg("trace: %-7d repeat (%s) [count %u]\n", __slin_cnt,
      __msgexpr_tostr(__xs, stp->st.srpt.repx), val);
    }
   if (val == 0L) break;
   return(stp->st.srpt.repst);
  case S_WHILE:
   xsp = __eval_xpr(stp->st.swh.lpx);
   if (xsp->xslen <= WBITS)
    {
     if (stp->st.swh.lpx->is_real)
      { 
       double d1;

       memcpy(&d1, xsp->ap, sizeof(double));
       if (d1 != 0.0) tmp = 1; else tmp = 0;
       goto while_end;
      }
     val = xsp->bp[0];
     if ((xsp->ap[0] & ~val) != 0L)
      {
       if (val != 0) 
        {
         __sgfinform(403, "while in %s condition true but some bits x/z",
          __msg_blditree(__xs, __cur_thd->th_itp, __cur_thd->assoc_tsk));
        }
       tmp = 1;
       goto while_end;
      }
     /* notice any 1 implies true so will not get here */
     if (val != 0)
      {
        __sgfinform(402,
        "while loop in %s terminating false condition value has x/z bits",
        __msg_blditree(__xs, __cur_thd->th_itp, __cur_thd->assoc_tsk));
      } 
     tmp = 0;
     goto while_end;
    } 
   if ((tmp = __cvt_lngbool(xsp->ap, xsp->bp, wlen_(xsp->xslen))) == 1)
    {
     if (!vval_is0_(xsp->bp, xsp->xslen))
      {
       __sgfinform(403, "while in %s condition true but some bits x/z",
        __msg_blditree(__xs, __cur_thd->th_itp, __cur_thd->assoc_tsk));
      }
     goto while_end;
    }
   /* notice any 1 implies true so will not get here */
   if (tmp == 3)
    {
     __sgfinform(402,
      "while loop in %s terminating false condition value has x/z bits",
      __msg_blditree(__xs, __cur_thd->th_itp, __cur_thd->assoc_tsk));
    }
   
while_end:
   __pop_xstk();
   if (__st_tracing)
    __tr_msg("trace: %-7d while (%s) [cond: %d]\n", __slin_cnt,
     __msgexpr_tostr(__xs, stp->st.swh.lpx), tmp);
   if (tmp == 1) return(stp->st.swh.lpst);
   return(stp->stnxt);
  case S_WAIT:
   /* on true expression, returns true */
   if (exec_wait(stp)) return(stp->st.swait.lpst);
   /* is this unnecessary since action stmt points back to wait */
   __cur_thd->thnxtstp = stp;
   __stmt_suspend = TRUE;
   return(NULL);
  case S_FOR:
   /* when loop done for returns NULL as next statement else 1st body st. */
   forp = stp->st.sfor;
   if (!for_not_done(forp))
    break;
   return(forp->forbody);
  case S_REPDCSETUP:
   /* 10/27/00 SJM - added repeat form rhs ectl and nb proca ectl setup */
   /* next statment is s delctrl or one after if repeat cnt x/z or <= 0 */ 
   return(exec_rep_ectl_setup(stp));
  case S_DELCTRL:
   /* this returns F, for suspend, non blocking returns T */
   /* 10/27/00 SJM - for repeat rhs ectrl count x/z <= 0 assign */
   /* immediate so also return T */
   if (exec_dctrl(stp)) return(__cur_thd->thnxtstp);
   __stmt_suspend = TRUE;
   return(NULL);
  case S_NAMBLK:
   /* for function only, just continue in named block */
   if (__fcspi >= 0)
    {
     if (__st_tracing)
      {
       __tr_msg("trace: %-7d begin : %s\n", __slin_cnt,
        stp->st.snbtsk->tsksyp->synam);
      }
     return(stp->st.snbtsk->tskst);
    }
   /* know this is new down thread - know at least 1 statement */
   exec_namblk(stp);
   return(__cur_thd->thnxtstp);
  case S_UNBLK:
   if (__st_tracing) __tr_msg("trace: %-7d begin\n", __slin_cnt);
   return(stp->st.sbsts);
  case S_UNFJ:
   /* this is unnamed fork-join only */ 
   if (__st_tracing) __tr_msg("trace: %-7d fork\n", __slin_cnt);
   __sched_fork(stp);
   __cur_thd->thnxtstp = stp->stnxt;
   __stmt_suspend = TRUE;
   return(NULL);
  case S_TSKCALL:
   /* if system task, NULL will suspend, else continue in down thread */
   if ((stp2 = __exec_tskcall(stp)) == NULL) return(NULL);
   return(stp2);
  case S_QCONTA:
   if (stp->st.sqca->qcatyp == ASSIGN) __exec_qc_assign(stp, FALSE);
   else
    {
     /* force of reg, is like assign except overrides assign */
     if (stp->st.sqca->regform) __exec_qc_assign(stp, TRUE);
     else __exec_qc_wireforce(stp);
    }
   if (__st_tracing)
    {
     char s1[RECLEN], s2[RECLEN];

     strcpy(s2, "");  
     if (stp->st.sqca->qcatyp == ASSIGN)
      {
       strcpy(s1, "assign");
       if (__force_active) strcpy(s2, " [active force effect hidden]"); 
      }
     else
      {
       strcpy(s1, "force");
       if (__assign_active) strcpy(s2, " [assign value saved]");
      }
     __tr_msg("trace: %-7d %s %s = %s%s\n", __slin_cnt, s1, 
      __msgexpr_tostr(__xs, stp->st.sqca->qclhsx),
      __msgexpr_tostr(__xs2, stp->st.sqca->qcrhsx), s2);
    }
   /* --- DBG remove 
   __dmpmod_nplst(__inst_mod, TRUE); 
   --- */
   __force_active = FALSE;
   __assign_active = FALSE;
   break;
  case S_QCONTDEA:
   if (stp->st.sqcdea.qcdatyp == DEASSIGN) __exec_qc_deassign(stp, FALSE);
   else
    {
     if (stp->st.sqcdea.regform) __exec_qc_deassign(stp, TRUE);
     else __exec_qc_wirerelease(stp);
    } 

   if (__st_tracing)
    {
     char s1[RECLEN], s2[RECLEN];

     strcpy(s2, "");  
     if (stp->st.sqcdea.qcdatyp == DEASSIGN)
      {
       strcpy(s1, "deassign");
       if (__force_active) strcpy(s2, " [active force effect hidden]"); 
      }
     else
      {
       strcpy(s1, "release");
       if (__assign_active) strcpy(s2, " [assign value restored]");
      }
     __tr_msg("trace: %-7d %s %s%s\n", __slin_cnt, s1, 
      __msgexpr_tostr(__xs, stp->st.sqcdea.qcdalhs), s2);
    }
   __force_active = FALSE;
   __assign_active = FALSE;
   break;
  case S_CAUSE:
   exec_cause(stp);
   break;
  case S_DSABLE:
   if (__st_tracing)
    {
     __tr_msg("trace: %-7d disable %s;\n", __slin_cnt,
      __msgexpr_tostr(__xs, stp->st.sdsable.dsablx));
    }
   /* if function, disable means continue with statement after block */ 
   if (__fcspi >= 0) return(stp->st.sdsable.func_nxtstp);

   /* here - done - suspend off so nil will end thread */
   /* xmr disable will mark some thread and return F */ 
   if (__exec_disable(stp->st.sdsable.dsablx)) return(NULL);
   break;
  case S_GOTO:
   stp = stp->st.sgoto;
   /* debug to nil ok, just end of list */
   __num_addedexec++; 
   if (__st_tracing)
    __tr_msg("trace: %-7d --continue %s\n", __slin_cnt, 
     __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt));
   __num_execstmts++;
   return(stp);
  /* do not trace break during break - may trace when execed */ 
  case S_BRKPT:
   /* returns T on need to break */
   if (__process_brkpt(stp)) return(stp); 

   /* not a break for some reason - restore stmt type and exec 1 stmt */
   /* if bp halt off 2nd time through after break, this execs */
   stp->stmttyp = stp->rl_stmttyp;
   stp2 = __brktr_exec_1stmt(stp);
   stp->stmttyp = S_BRKPT;
   return(stp2);
  default: __case_terr(__FILE__, __LINE__);
 }
 return(stp->stnxt);
}

/*
 * evaluate an procedural assign rhs expression and convert to form
 * needed for assignment
 * handles real conversion and size changes - never z widening
 * know returned stack width always exactly matches lhs width
 */
extern struct xstk_t *__eval_assign_rhsexpr(register struct expr_t *xrhs,
 register struct expr_t *xlhs)
{
 register struct xstk_t *xsp;

 xsp = __eval_xpr(xrhs);
 if (xlhs->is_real)
  { 
   /* needed: think passing packed bit does not work on all compilers ? */ 
   if (!xrhs->is_real) __cnv_stk_fromreg_toreal(xsp, (xrhs->has_sign == 1));
  }
 else
  { 
   /* handle rhs preparation of reals - then assign is just copy for reals */
   if (xrhs->is_real) __cnv_stk_fromreal_toreg32(xsp);

   /* SJM 09/29/03 - change to handle sign extension and separate types */
   if (xsp->xslen > xlhs->szu.xclen) __narrow_sizchg(xsp, xlhs->szu.xclen);
   else if (xsp->xslen < xlhs->szu.xclen)
    {
     if (xrhs->has_sign) __sgn_xtnd_widen(xsp, xlhs->szu.xclen);
     else __sizchg_widen(xsp, xlhs->szu.xclen);
    }
  }
 return(xsp);
}

/*
 * evaulate task assign - only different if form of various flags 
 */
static void eval_tskassign_rhsexpr(register struct xstk_t *xsp,
 register int32 lhsreal, register int32 lhswid, register int32 rhsreal,
 register int32 rhssign)
{
 if (lhsreal)
  { 
   /* think passing packed bit does not work on all compilers ? */ 
   if (!rhsreal) __cnv_stk_fromreg_toreal(xsp, rhssign);
  }
 else
  { 
   /* handle rhs preparation of reals - then assign is just copy for reals */
   if (rhsreal) __cnv_stk_fromreal_toreg32(xsp);

   if (xsp->xslen > lhswid) __narrow_sizchg(xsp, lhswid);
   else if (xsp->xslen < lhswid)
    {
     if (rhssign) __sgn_xtnd_widen(xsp, lhswid);
     else __sizchg_widen(xsp, lhswid);
    }
  }
}

/*
 * trace an assignment statement
 * notice this expects rhs value to be on top of stack (caller pops)
 *
 * ok to use rgab_tostr here since know __exprline can not be in use before
 * statement execution begins
 */
static void tr_proc_assign(struct st_t *stp, struct xstk_t *xsp)
{
 struct expr_t *xrhs;

 __cur_sofs = 0;
 __dmp_proc_assgn((FILE *) NULL, stp, (struct delctrl_t *) NULL, FALSE);
 __exprline[__cur_sofs] = 0; 
 __trunc_exprline(TRTRUNCLEN, FALSE);
 __tr_msg("trace: %-7d %s", __slin_cnt, __exprline);
 xrhs = stp->st.spra.rhsx;
 /* if rhs is number value is obvious, else print value that was assigned */
 /* any conversion to lhs already made so expr. info from lhs */ 
 if (xrhs->optyp != NUMBER && xrhs->optyp != REALNUM)
  {
   __tr_msg(" [%s]\n", __xregab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen,
    stp->st.spra.lhsx));
  }
 else __tr_msg("\n");
 __cur_sofs = 0;
}

/*
 * trace an non blocking assignment statement
 * notice this expects rhs value to be on top of stack (caller pops)
 *
 * ok to use rgab_tostr here since know __exprline can not be in use before
 * statement execution begins
 */
static void tr_nbproc_assign(struct st_t *stp, struct xstk_t *xsp)
{
 struct expr_t *xrhs;

 __cur_sofs = 0;
 __dmp_nbproc_assgn((FILE *) NULL, stp, (struct delctrl_t *) NULL);
 __exprline[__cur_sofs] = 0; 
 __trunc_exprline(TRTRUNCLEN, FALSE);
 __tr_msg("trace: %-7d %s", __slin_cnt, __exprline);
 xrhs = stp->st.spra.rhsx;
 if (xrhs->optyp != NUMBER && xrhs->optyp != REALNUM)
  {
   __tr_msg(" [%s]\n", __xregab_tostr(__xs, xsp->ap, xsp->bp,
    xsp->xslen, xrhs));
  }
 else __tr_msg("\n");
 __cur_sofs = 0;
}

/*
 * execute repeat event setup (rhs nb ectl proca or rhs ectl delay  
 * know rhs DEL CTRL stmt with repeat form rhs ev control follows
 * only can be rhs ev control or rhs non blocking assign ev ctrl
 */
static struct st_t *exec_rep_ectl_setup(struct st_t *stp)
{
 register struct xstk_t *xsp;
 register struct st_t *nxtstp, *astp;
 struct delctrl_t *rdctp;
 word32 val;

 __num_addedexec++; 
 nxtstp = stp->stnxt;
 /* DBG RELEASE remove --- */
 if (nxtstp->stmttyp != S_DELCTRL) __misc_terr(__FILE__, __LINE__);
 /* --- */
 rdctp = nxtstp->st.sdc;
 xsp = __eval_xpr(rdctp->repcntx);
 /* SJM 04/02/02 - real count must be converted to word/int32 */
 if (rdctp->repcntx->is_real) __cnv_stk_fromreal_toreg32(xsp);

 /* FIXME ??? - although know WBITS wide, should use bp here */
 if (xsp->ap[1] != 0L)
  {
   __sgfwarn(645,
    "repeat event control in %s count has x/z value - no wait for event",
    __msg_blditree(__xs, __cur_thd->th_itp, __cur_thd->assoc_tsk));
   __pop_xstk();

immed_ectl_exec:     
   astp = nxtstp->st.sdc->actionst;
   xsp = __eval_assign_rhsexpr(astp->st.spra.rhsx, astp->st.spra.lhsx);
   if (astp->stmttyp == S_NBPROCA)
    {
     /* case 1: NB assign - becomes no delay NB assign form */ 
     sched_nbproc_delay(NULL, xsp, astp);
    }
   else
    {
     /* case 2: rhs repeat event control - treat as simple proca */ 
     __exec2_proc_assign(astp->st.spra.lhsx, xsp->ap, xsp->bp);
    }
   __pop_xstk();
   /* continuation statment is one after S DELCTRL since no ev ctrl */
   /* in this case */
   return(nxtstp->stnxt);
  }
 /* if signed and <= 0, or word32 equal to 0, becomes immediate assign */
 /* SJM 04/02/02 - need to use word32 counter and convert neg to 0 */
 if (rdctp->repcntx->has_sign && (int32) xsp->ap[0] <= 0) val = 0;
 else val = xsp->ap[0];
 __pop_xstk();
 if (val == 0) goto immed_ectl_exec;

 /* val now number of edges (if 1 same as normal rhs ectrl */
 /* notice, here never exec unless at least one so do not need inc */
 rdctp->dce_repcnts[__inum] = val;
 return(nxtstp);
}

/*
 * execute a delay control indicator
 * notice this arms or schedules something - caller suspends thread
 * this return T if non blocking assign needs to not schedule
 */
static int32 exec_dctrl(struct st_t *stp)
{
 int32 bytes, wlen;
 word32 *wp;
 struct delctrl_t *dctp;
 struct xstk_t *xsp;
 struct st_t *astp;

 dctp = stp->st.sdc;
 if (__st_tracing)
  {
   __evtr_resume_msg();
   __cur_sofs = 0;
   if (dctp->actionst == NULL || dctp->dctyp == DC_RHSEVENT
    || dctp->dctyp == DC_RHSDELAY) __dmp_dctrl((FILE *) NULL, dctp);
   else
    {
     if (dctp->dctyp == DC_EVENT) addch_('@'); else addch_('#');
     __dmp_dcxpr((FILE *) NULL, dctp->dc_du, dctp->dc_delrep);
    }
   __trunc_exprline(TRTRUNCLEN, FALSE);
   __tr_msg("trace: %-7d %s\n", __slin_cnt, __exprline);
   __cur_sofs = 0;
  }
 /* for all but non blocking assign block - continue with action statement */
 if (dctp->actionst == NULL) __cur_thd->thnxtstp = stp->stnxt;
 else __cur_thd->thnxtstp = dctp->actionst;

 switch ((byte) dctp->dctyp) {
  case DC_DELAY:
   sched_proc_delay(dctp, (word32 *) NULL, -1);
   break;
  case DC_EVENT:
   arm_event_dctrl(dctp, (word32 *) NULL, -1);
   break; 
  case DC_RHSDELAY: case DC_RHSEVENT:
   /* 10/28/00 SJM - only rhs event either blocking or non blocking */
   /* can have repeat form */
   astp = dctp->actionst;
   /* rhs # delay or event ctrl */
   /* -- DBG remove
   if (astp == NULL || (astp->stmttyp != S_RHSDEPROCA
    && astp->stmttyp != S_NBPROCA)) __arg_terr(__FILE__, __LINE__);
   --- */ 

   /* evaluate rhs and schedule as usual */
   /* notice this depends on contiguous xsp a and b parts */
   xsp = __eval_assign_rhsexpr(astp->st.spra.rhsx, astp->st.spra.lhsx);

   if (astp->stmttyp == S_NBPROCA)
    {
     /* for non blocking assign - must not exec assign - event processing */
     /* routine does that, must continue after actionst if can else nxt */
     if (dctp->actionst != NULL && dctp->actionst->stnxt != NULL)
      __cur_thd->thnxtstp = dctp->actionst->stnxt;
     else __cur_thd->thnxtstp = stp->stnxt;
     
     if (dctp->dctyp == DC_RHSDELAY) sched_nbproc_delay(dctp, xsp, astp);
     else arm_nbevent_dctrl(dctp, xsp, astp);
     __pop_xstk();
     return(TRUE);
    }
   /* continuation point for rhs delay form is action statement if exists */
   /* that is same as normal delay control */

   /* if blocking allocate and store - no inertial problems for blocking */
   /* SJM - 01/12/00 - wlen_ omitted so here was large memory leak */
   /*                  was only freeing 4/32 percent of bytes */
   wlen = wlen_(astp->st.spra.lhsx->szu.xclen);
   bytes = 2*WRDBYTES*wlen;
   
   wp = (word32 *) __my_malloc(bytes);
   memcpy(wp, xsp->ap, bytes); 

   if (dctp->dctyp == DC_RHSDELAY) sched_proc_delay(dctp, wp, wlen);
   else arm_event_dctrl(dctp, wp, wlen);
   __pop_xstk();
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 return(FALSE);
}

/*
 * schedule procedural delay thread simple prefix timing delay
 *
 * must continue after wake up with same thread (contents?)
 * before call statement set to statement to exec after wake up
 * also handles rhs delay form
 *
 * notice on disable event canceled and any rhs value free but that is
 * all that is needed
 */
static void sched_proc_delay(struct delctrl_t *dctp, word32 *wp, int32 wlen)
{
 register i_tev_ndx tevpi;
 register struct tev_t *tevp;
 word64 t, schtim;
 struct st_t *stp;

 /* this can not be edge delay or syntax error before here */
 __get_del(&t, dctp->dc_du, dctp->dc_delrep);  
 schtim = __simtime + t;
 alloc_tev_(tevpi, TE_THRD, __inst_ptr, schtim);
 /* set the associate event - after return, __cur_thd will be new */
 __cur_thd->thdtevi = tevpi;
 /* restart current - will block after here and change threads */
 tevp = &(__tevtab[tevpi]);
 tevp->tu.tethrd = __cur_thd;
 /* if rhs delay form, set values */ 
 if (wp != NULL)
  {
   __cur_thd->th_rhsform = TRUE;
   __cur_thd->th_rhswp = wp;
   __cur_thd->th_rhswlen = wlen;  
  }

 if (__ev_tracing)
  {
   char s1[RECLEN], vs2[10];

   __evtr_resume_msg();
   if (wp == NULL) strcpy(vs2, ""); else strcpy(vs2, "(rhs)");
   stp = tevp->tu.tethrd->thnxtstp;
   __tr_msg("-- scheduling delay resume%s at %s for time %s\n",
    vs2, __bld_lineloc(s1, stp->stfnam_ind, stp->stlin_cnt),
    __to_timstr(__xs, &(tevp->etime)));
  }
 /* notice that procedural #0 (does not need to be rhs assign form) */
 /* done after all normal events */
 if (t == 0ULL)
  {
   if (__debug_flg && __ev_tracing)
    {
     __tr_msg("sched: adding #0 %s event to list end\n",
      __to_tetyp(__xs, tevp->tetyp));
    }
   /* notice pound 0 only added from current time events */
   if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevpi;
   else { __tevtab[__p0_te_endi].tenxti = tevpi; __p0_te_endi = tevpi; }
  }
 /* if non blocking procedural assign, insert in normal moved to #0 later */
 else __insert_event(tevpi);
}

/*
 * schedule non blocking procedural assign simple prefix timing delay
 *
 * this is simple because of strange non hardware related semantics
 * every time a non blocking delay assigned is executed, just compute
 * delay and schedule - can have >1 events per unit or per statement
 * but just schedule and forget
 */
static void sched_nbproc_delay(struct delctrl_t *dctp, struct xstk_t *xsp,
 struct st_t *stp)
{
 i_tev_ndx tevpi;
 word64 t, schtim;

 /* if no delay form, schedule at end of currnt time #0s */
 if (dctp == NULL) t = 0ULL;
 /* error before here if edge dependent delay */
 else __get_del(&t, dctp->dc_du, dctp->dc_delrep);  
 schtim = __simtime + t;

 if (__ev_tracing)
  {
   char s1[RECLEN], s2[RECLEN], s3[RECLEN];

   __evtr_resume_msg();
   __tr_msg(
    "-- scheduling delay form non blocking assign line %s now %s in %s:\n",
    __bld_lineloc(s1, stp->stfnam_ind, stp->stlin_cnt),
    __to_timstr(s2, &__simtime), __msg2_blditree(s3, __inst_ptr));
   __tr_msg(" NB SCHEDULE TO NEW VALUE %s AT TIME %s\n",
    __xregab_tostr(s1, xsp->ap, xsp->bp, stp->st.spra.rhsx->szu.xclen,
     stp->st.spra.rhsx), __to_timstr(s2, &schtim));
  }

 /* build the disable remove list for possibly multiple active nb forms */
 tevpi = __bld_nb_tev(stp, xsp, schtim); 
 /* final step is inserting event in list */
 /* no dleay form becomes #0 schedule for assign event here */
 if (t == 0ULL)
  {
   if (__debug_flg && __ev_tracing)
    {
     __tr_msg("sched: adding #0 %s event to list end\n",
      __to_tetyp(__xs, __tevtab[tevpi].tetyp));
    }
   /* notice pound 0 only added for current time events */
   /* AIV 06/28/05 - if option not set add to the end of the nb #0 list */
   if (!__nb_sep_queue)
    {
     if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevpi;
     else { __tevtab[__p0_te_endi].tenxti = tevpi; __p0_te_endi = tevpi; }
    }
   else
    {
     /* AIV 07/05/05 - to match XL need nb te list that only processed */
     /* when all pnd 0s done */  
     /* effectively adds another section to current time event queue */
     if (__nb_te_hdri == -1) __nb_te_hdri = __nb_te_endi = tevpi;
     else { __tevtab[__nb_te_endi].tenxti = tevpi; __nb_te_endi = tevpi; }
    }
  }
 /* if non blocking procedural assign, insert in normal moved to #0 later */
 else __insert_event(tevpi);
}

/*
 * build and emit trace message for non blocking schedule or trigger
 * notice these are not inertial - just keep scheduling
 * never cancel or re-schedule
 *
 * SJM 08/08/99 - change so if lhs expr (maybe concat) has non constant
 * bit selects copy and then evaluate variable indices to numbers
 * and change copied expr.
 *
 * BEWARE - code here and in many places assumes numeric expressions
 * folded to number or IS number by here
 */
extern i_tev_ndx __bld_nb_tev(struct st_t *stp, struct xstk_t *xsp,
 word64 schtim)
{
 register struct tenbpa_t *nbpap;
 register word32 *wp;
 i_tev_ndx tevpi;
 int32 wlen; 
 struct expr_t *lhsxp;

 alloc_tev_(tevpi, TE_NBPA, __inst_ptr, schtim);
 nbpap = (struct tenbpa_t *) __my_malloc(sizeof(struct tenbpa_t)); 
 __tevtab[tevpi].tu.tenbpa = nbpap;
 wlen = wlen_(stp->st.spra.lhsx->szu.xclen);
 wp = (word32 *) __my_malloc(2*wlen*WRDBYTES);

 memcpy(wp, xsp->ap, 2*wlen*WRDBYTES);

 nbpap->nbawp = wp; 
 nbpap->nbastp = stp;

 /* copy expr. if needed */
 /* BEWARE - code in many places assumes numeric expressions folded to */ 
 /* number or IS number by here */
 if (!__lhsexpr_var_ndx(stp->st.spra.lhsx)) nbpap->nblhsxp = NULL;
 else
  {
   /* notice - know will have same width as stp lhsx */
   lhsxp = __sim_copy_expr(stp->st.spra.lhsx);
   __eval_lhsexpr_var_ndxes(lhsxp);
   nbpap->nblhsxp = lhsxp; 
  }
 /* caller sets dctp if needed */
 nbpap->nbdctp = NULL;
 return(tevpi);
}

/*
 * return T if expression contains non constant bit select index
 *
 * this assume only one level concats but maybe should
 */
extern int32 __lhsexpr_var_ndx(register struct expr_t *xp)
{
 switch ((byte) xp->optyp) {
  case GLBREF: case ID:
   break;
  case PARTSEL:
   /* part select always constant */
   break;
  case LSB:
   if (xp->ru.x->optyp == NUMBER || xp->ru.x->optyp == ISNUMBER) break;
   return(TRUE);
  case LCB:
   {
    register struct expr_t *catxp;

    for (catxp = xp->ru.x; catxp != NULL; catxp = catxp->ru.x)
     {
      /* if var index must copy entire expr. */
      if (__lhsexpr_var_ndx(catxp->lu.x)) return(TRUE);
     }
   } 
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 return(FALSE);
}

/*
 * evaluate any variable indices to constants
 *
 * this is guts of LRM non-blocking assign algorithm - for any variable
 * bit index eval and convert to constant
 *
 * this mangles expr but since copied and free when non blocking assign
 * done still works
 *
 * assumes only one level concats but maybe should
 */
extern void __eval_lhsexpr_var_ndxes(register struct expr_t *xp)
{
 int32 biti;
 struct expr_t *idndp;
 struct net_t *np;
 struct expr_t *ndx;

 switch ((byte) xp->optyp) {
  case GLBREF: case ID: break;
  case PARTSEL:
   /* part select always constant */
   break;
  case LSB:
   if (xp->ru.x->optyp != NUMBER && xp->ru.x->optyp != ISNUMBER)
    {
     idndp = xp->lu.x;
     np = idndp->lu.sy->el.enp;
     /* can be either constant or expr. - both handled in comp. */
     biti = __comp_ndx(np, xp->ru.x);
     /* out of range is x as index */
     if (biti == -1) ndx = __bld_rng_numxpr(ALL1W, ALL1W, WBITS);
     else ndx = __bld_rng_numxpr((word32) biti, 0, WBITS);
     __free_xtree(xp->ru.x);
     xp->ru.x = ndx;
    }
   /* if constant (even IS) index, nothing to do */ 
   break;
  case LCB:
   {
    register struct expr_t *catxp;

    for (catxp = xp->ru.x; catxp != NULL; catxp = catxp->ru.x)
     {
      /* if var index must copy entire expr. */
      __eval_lhsexpr_var_ndxes(catxp->lu.x);
     }
   } 
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * arm an event delay control - know already set up
 * know current thread set to continuation point here
 * know current thread will be blocked waiting for this 1 event
 *
 * notice may be triggered from other thread (init/always) in same inst.
 * but continuation is here
 * here arming ref. instance even though only change of target wire will
 * trigger for xmr or col. case
 */
static void arm_event_dctrl(struct delctrl_t *dctp, register word32 *wp,
 int32 wlen)
{
 register i_tev_ndx tevpi;
 struct tev_t *tevp;
 struct st_t *stp;

 /* build after trigger fires, startup event */
 /* notice this event record is not linked onto any event list for now */
 alloc_tev_(tevpi, TE_THRD, __inst_ptr, __simtime);
 /* link event back to thread */
 __cur_thd->thdtevi = tevpi;
 tevp = &(__tevtab[tevpi]);
 tevp->tu.tethrd = __cur_thd;

 /* if rhs delay form, set values */ 
 if (wp != NULL)
  {
   __cur_thd->th_rhsform = TRUE;
   __cur_thd->th_rhswp = wp;
   __cur_thd->th_rhswlen = wlen;
  }

 if (__debug_flg && __st_tracing)
  {
   stp = tevp->tu.tethrd->thnxtstp;
   __tr_msg("-- arming event thread %s itree loc. %s statement at %s\n",
    __cur_thd->th_itp->itip->isym->synam, __inst_ptr->itip->isym->synam,
    __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt));
  }
 /* RELEASE remove --
 if (__debug_flg)
  __dmp_dcemsg(dctp, "setting dce to event");
 --- */

 /* if rexecuting task, algorithm is to cancel previous pending */
 /* delay control and emit warning */
 if (dctp->dceschd_tevs[__inum] != -1)
  {
   if (__cur_thd->assoc_tsk == NULL)
    {
     stp = tevp->tu.tethrd->thnxtstp;
     __sgfwarn(635, 
      "INTERNAL BUG? - in %s cancel and rearm of event control to resume at %s",
      __msg2_blditree(__xs, __inst_ptr), __bld_lineloc(__xs2, stp->stfnam_ind,
      stp->stlin_cnt));
    }
   else
    {
     stp = tevp->tu.tethrd->thnxtstp;
     __sgfwarn(635, 
      "when reexecuting task %s cancel and rearm of event control to resume at %s",
      __msg_blditree(__xs, __inst_ptr, __cur_thd->assoc_tsk),
      __bld_lineloc(__xs2, stp->stfnam_ind, stp->stlin_cnt));
    }
  }

 /* DBG remove -- */
 if (__cur_thd->th_dctp != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 /* notice simply linking event on the scheduled list enables the ev ctrl */
 dctp->dceschd_tevs[__inum] = tevpi;
 __cur_thd->th_dctp = dctp;
 /* handle any tracing */
 if (__ev_tracing)
  {
   char vs2[10];

   stp = tevp->tu.tethrd->thnxtstp;
   if (wp == NULL) strcpy(vs2, ""); else strcpy(vs2, "(rhs)");
   __tr_msg("-- event control suspend%s to resume line %s\n", 
    vs2, __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt));
  }
}

/*
 * print a dctp message for debugging
 */
extern void __dmp_dcemsg(struct delctrl_t *dctp, char *dcemsg)
{
 char s1[RECLEN];

 if (dctp->actionst != NULL)
  __bld_lineloc(s1, dctp->actionst->stfnam_ind, dctp->actionst->stlin_cnt);
 else strcpy(s1, "<none>");
 __dbg_msg("%s: at %p of type %s instance %d(%s) iact=%d stmt. %s\n", dcemsg, 
  dctp, __to_dcenam(__xs, dctp->dctyp), __inum,
  __msg2_blditree(__xs2, __inst_ptr), dctp->dc_iact, s1);
}

/*
 * arm a non blocking assign delay control
 * 
 * multiple allowed just add each new tev to end
 * LOOKATME - notice current scheme requires linear traversal to list end.
 */
static void arm_nbevent_dctrl(struct delctrl_t *dctp, struct xstk_t *xsp,
 struct st_t *stp)
{
 register i_tev_ndx tevp2i;
 i_tev_ndx tevpi;
 struct tev_t *tevp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN]; 

 /* DBG remove -- */
 if (__ev_tracing)
  {
   __evtr_resume_msg();
    __tr_msg(
     "-- arming event control non blocking assign line %s now %s in %s:\n",
     __bld_lineloc(s1, stp->stfnam_ind, stp->stlin_cnt),
     __to_timstr(s3, &__simtime), __msg2_blditree(s2, __inst_ptr));

   __tr_msg("   EVENT TRIGGER ARM NEW VALUE %s\n",
    __xregab_tostr(s1, xsp->ap, xsp->bp, stp->st.spra.lhsx->szu.xclen,
    stp->st.spra.rhsx));
  }
 /* --- */
 tevpi = __bld_nb_tev(stp, xsp, __simtime); 
 tevp = &(__tevtab[tevpi]);
 tevp->nb_evctrl = TRUE;
 /* for event control form need to set dctp field */
 tevp->tu.tenbpa->nbdctp = dctp;
 if ((tevp2i = dctp->dceschd_tevs[__inum]) != -1)
  {
   /* could save end pointer if too slow ? */
   for (; __tevtab[tevp2i].tenxti != -1; tevp2i = __tevtab[tevp2i].tenxti) ;
   __tevtab[tevp2i].tenxti = tevpi;
   /* ??? LOOKATME is this needed */
   __tevtab[tevpi].tenxti = -1;
  }
 else dctp->dceschd_tevs[__inum] = tevpi;
}

/*
 * execute a simple (not casex and casez) case statement
 */
static struct st_t *exec_case(struct st_t *stp)
{
 register word32 aw, bw;
 register struct xstk_t *itemxsp;
 register struct exprlst_t *xplp;
 register struct csitem_t *csip;
 int32 selxlen, selwlen, i; 
 struct xstk_t *selxsp;
 struct csitem_t *dflt_csip;

 /* SJM 12/12/03 - must treat all 3 case types as special case if any */
 /* of select or case item exprs real */
 if (stp->st.scs.csx->is_real || stp->st.scs.csx->cnvt_to_real)
  {
   return(exec_real_case(stp));
  }

 if (stp->st.scs.castyp == CASEX) return(exec_casex(stp));
 else if (stp->st.scs.castyp == CASEZ) return(exec_casez(stp));

 /* compute the case type - determines operator to use */
 selxsp = __eval_xpr(stp->st.scs.csx);

 /* if expression real, convert to 32 bit reg */
 if (stp->st.scs.csx->is_real) __cnv_stk_fromreal_toreg32(selxsp);

 /* if result of selector is not as wide as needed widen */
 /* case needs w bits width but selector is wire < w */
 selxlen = stp->st.scs.maxselwid;
 /* DBG remove -- */
 if (selxsp->xslen > selxlen) __misc_terr(__FILE__, __LINE__);
 /* --- */ 
 /* SJM 09/29/03 - change for new sized but widen only */
 if (selxsp->xslen < selxlen)
  {
   /* SJM 05/10/04 - LOOKATME - algorithm is that if any of the case match */
   /* exprs are word32 - case becomes word32 */
   if (stp->st.scs.csx->has_sign && !stp->st.scs.csx->unsgn_widen)
    __sgn_xtnd_widen(selxsp, selxlen);
   else __sizchg_widen(selxsp, selxlen);
  }

 selxlen = selxsp->xslen; 
 if (__st_tracing) tr_case_st(selxsp, (stp->st.scs.csx->has_sign == 1));
 dflt_csip = stp->st.scs.csitems;
 csip = dflt_csip->csinxt;

 /* case case 1: fits in one word32 */
 if (selxlen <= WBITS)
  {
   aw = selxsp->ap[0];
   bw = selxsp->bp[0];
   for (; csip != NULL; csip = csip->csinxt)
    {
     for (xplp = csip->csixlst; xplp != NULL; xplp = xplp->xpnxt)
      {
       itemxsp = __eval2_xpr(xplp->xp);

       /* SJM 12/12/03 - never can be real here using new all if any code */

       /* no conversion needed becaause know item may be too narrow only */
       if (((aw ^ itemxsp->ap[0]) | (bw ^ itemxsp->bp[0])) == 0)
        { __pop_xstk(); __pop_xstk(); return(csip->csist); }
       __pop_xstk();
      }
    }
   __pop_xstk();
   if (dflt_csip->csist != NULL) return(dflt_csip->csist);
   return(NULL);
  }
 /* case case 2: wider than 1 word32 */
 selwlen = wlen_(selxlen);
 for (; csip != NULL; csip = csip->csinxt)
  {
   for (xplp = csip->csixlst; xplp != NULL; xplp = xplp->xpnxt)
    {
     itemxsp = __eval2_xpr(xplp->xp);

     /* SJM 12/12/03 - never can be real here using new all if any code */

     /* SJM 09/29/03 handle sign extension and separate cases */
     if (itemxsp->xslen > selxlen) __narrow_sizchg(itemxsp, selxlen);
     else if (itemxsp->xslen < selxlen)
      {
       if (xplp->xp->has_sign && !xplp->xp->unsgn_widen)
        __sgn_xtnd_widen(itemxsp, selxlen);
       else __sizchg_widen(itemxsp, selxlen);
      }

     for (i = 0; i < selwlen; i++)
      {
       if (((selxsp->ap[i] ^ itemxsp->ap[i])
        | (selxsp->bp[i] ^ itemxsp->bp[i])) != 0) goto nxt_x;
      }
     __pop_xstk();
     __pop_xstk();
     return(csip->csist);

nxt_x:
     __pop_xstk();
    }
  }  
 __pop_xstk();
 if (dflt_csip->csist != NULL) return(dflt_csip->csist);
 return(NULL);
}

/*
 * special case routine to exec a case where any expr real
 *
 * SJM 12/12/03 - was converting real to word32 for cases with real but
 * think that is wrong (although 2001 LRM does not say exactly) so now
 * if any of case select or case item real, all compares real
 * (following same rule, if any of select or match expr word32 all)
 * (widening word32 - compares are for equal so only widening changes)
 */
static struct st_t *exec_real_case(struct st_t *stp)
{
 register struct xstk_t *itemxsp;
 register struct exprlst_t *xplp;
 register struct csitem_t *csip;
 double d1, d2;
 struct xstk_t *selxsp;
 struct csitem_t *dflt_csip;

 /* warning if casex or casez with real since no effect */
 if (stp->st.scs.castyp == CASEX || stp->st.scs.castyp == CASEZ)
  {
   __sgfwarn(3113,
    "select or case item expression real - casex/casez no effect");
  }

 /* compute the case type - determines operator to use */
 selxsp = __eval_xpr(stp->st.scs.csx);

 /* if select expr not real convert it */
 if (stp->st.scs.csx->cnvt_to_real)
  __cnv_stk_fromreg_toreal(selxsp, stp->st.scs.csx->has_sign);

 if (__st_tracing) tr_case_st(selxsp, (stp->st.scs.csx->has_sign == 1));

 dflt_csip = stp->st.scs.csitems;
 csip = dflt_csip->csinxt;
 memcpy(&d1, selxsp->ap, sizeof(double));

 for (; csip != NULL; csip = csip->csinxt)
  {
   for (xplp = csip->csixlst; xplp != NULL; xplp = xplp->xpnxt)
    {
     itemxsp = __eval2_xpr(xplp->xp);

     /* if case item expr not real convert it */
     if (xplp->xp->cnvt_to_real)
      {
       __cnv_stk_fromreg_toreal(itemxsp, xplp->xp->has_sign);
      } 
     memcpy(&d2, itemxsp->ap, sizeof(double));

     /* real == (near 0.0) */
     if ((d2 - d1) > -EPSILON && (d2 - d1) < EPSILON)
      { __pop_xstk(); __pop_xstk(); return(csip->csist); }
     __pop_xstk();
    }
  }
 __pop_xstk();
 if (dflt_csip->csist != NULL) return(dflt_csip->csist);
 return(NULL);
}

/*
 * trace a case (for any of case/casex/casez) 
 */
static void tr_case_st(struct xstk_t *selxsp, int32 cas_sign)
{
 __tr_msg("trace: %-7d -- [selector: %d'h%s]\n",
  __slin_cnt, selxsp->xslen, __regab_tostr(__xs, selxsp->ap, selxsp->bp,
  selxsp->xslen, BHEX, cas_sign));
}

/*
 * execute a casex case statement
 */
static struct st_t *exec_casex(struct st_t *stp)
{
 register word32 aw, bw;
 register struct xstk_t *itemxsp;
 register struct csitem_t *csip;
 register struct exprlst_t *xplp;
 int32 selxlen, selwlen, i;
 struct xstk_t *selxsp;
 struct csitem_t *dflt_csip;

 /* compute the case type - determines operator to use */
 selxsp = __eval_xpr(stp->st.scs.csx);

 /* if expression real, convert to 32 bit reg */
 if (stp->st.scs.csx->is_real) __cnv_stk_fromreal_toreg32(selxsp);

 /* if result of selector is not as wide as needed widen */
 /* case needs w bits width but selector is wire < w */

 selxlen = stp->st.scs.maxselwid;
 /* DBG remove -- */
 if (selxsp->xslen > selxlen) __misc_terr(__FILE__, __LINE__);
 /* --- */ 
 /* SJM 09/29/03 - change for new sized but widen only */
 if (selxsp->xslen < selxlen)
  {
   if (stp->st.scs.csx->has_sign && !stp->st.scs.csx->unsgn_widen)
    __sgn_xtnd_widen(selxsp, selxlen);
   else __sizchg_widen(selxsp, selxlen);
  }
 if (__st_tracing) tr_case_st(selxsp, (stp->st.scs.csx->has_sign == 1));

 dflt_csip = stp->st.scs.csitems;
 csip = dflt_csip->csinxt;

 /* case case 1: fits in one word32 */
 if (selxlen <= WBITS)
  {
   aw = selxsp->ap[0];
   bw = selxsp->bp[0];
   for (; csip != NULL; csip = csip->csinxt)
    {
     for (xplp = csip->csixlst; xplp != NULL; xplp = xplp->xpnxt)
      {
       itemxsp = __eval2_xpr(xplp->xp);

       /* SJM 12/12/03 - never can be real here using new all if any code */

       /* no conversion needed becaause know item may be too narrow only */
       /* must 0 any don't care bits with either x/z bit 0 mask */
       if ((((aw ^ itemxsp->ap[0]) | (bw ^ itemxsp->bp[0]))
        & ~(bw | itemxsp->bp[0])) == 0)
        { __pop_xstk(); __pop_xstk(); return(csip->csist); }
       __pop_xstk();
      }
    }
   __pop_xstk();
   if (dflt_csip->csist != NULL) return(dflt_csip->csist);
   return(NULL);
  }
 /* case case 2: wider than 1 word32 */
 selwlen = wlen_(selxlen);
 for (; csip != NULL; csip = csip->csinxt)
  {
   for (xplp = csip->csixlst; xplp != NULL; xplp = xplp->xpnxt)
    {
     itemxsp = __eval2_xpr(xplp->xp);

     /* SJM 12/12/03 - never can be real here using new all if any code */
 
     /* SJM 09/29/03 handle sign extension and separate cases */
     if (itemxsp->xslen > selxlen) __narrow_sizchg(itemxsp, selxlen);
     else if (itemxsp->xslen < selxlen)
      {
       if (xplp->xp->has_sign && !xplp->xp->unsgn_widen)
        __sgn_xtnd_widen(itemxsp, selxlen);
       else __sizchg_widen(itemxsp, selxlen);
      }

     for (i = 0; i < selwlen; i++)
      {
       /* SJM 01/08/99 - WAS WRONG - for wide if == 0 always matches first */
       if ((((selxsp->ap[i] ^ itemxsp->ap[i])
        | (selxsp->bp[i] ^ itemxsp->bp[i]))
        & ~(selxsp->bp[i] | itemxsp->bp[i])) != 0) goto nxt_x;
      }
     __pop_xstk();
     __pop_xstk();
     return(csip->csist);

nxt_x:
     __pop_xstk();
    }
  }
 __pop_xstk();
 if (dflt_csip->csist != NULL) return(dflt_csip->csist);
 return(NULL);
}

/*
 * execute a casez case statement
 */
static struct st_t *exec_casez(struct st_t *stp)
{
 register word32 aw, bw;
 register struct xstk_t *itemxsp;
 register struct csitem_t *csip;
 register struct exprlst_t *xplp;
 register word32 mask;
 int32 selxlen, selwlen, i;
 struct xstk_t *selxsp;
 struct csitem_t *dflt_csip;

 /* compute the case type - determines operator to use */
 selxsp = __eval_xpr(stp->st.scs.csx);

 /* if expression real, convert to 32 bit reg */
 if (stp->st.scs.csx->is_real) __cnv_stk_fromreal_toreg32(selxsp);

 /* if result of selector is not as wide as needed widen */
 /* case needs w bits width but selector is wire < w */
 selxlen = stp->st.scs.maxselwid;
 /* DBG remove -- */
 if (selxsp->xslen > selxlen) __misc_terr(__FILE__, __LINE__);
 /* --- */ 
 /* SJM 09/29/03 - change for new sized but widen only */
 if (selxsp->xslen < selxlen)
  {
   if (stp->st.scs.csx->has_sign && !stp->st.scs.csx->unsgn_widen)
    __sgn_xtnd_widen(selxsp, selxlen);
   else __sizchg_widen(selxsp, selxlen);
  }
 if (__st_tracing) tr_case_st(selxsp, (stp->st.scs.csx->has_sign == 1));

 dflt_csip = stp->st.scs.csitems;
 csip = dflt_csip->csinxt;

 /* case case 1: fits in one word32 */
 if (selxlen <= WBITS)
  {
   aw = selxsp->ap[0];
   bw = selxsp->bp[0];
   for (; csip != NULL; csip = csip->csinxt)
    {
     for (xplp = csip->csixlst; xplp != NULL; xplp = xplp->xpnxt)
      {
       itemxsp = __eval_xpr(xplp->xp);

       /* SJM 12/12/03 - never can be real here using new all if any code */

       /* no conversion needed becaause know item may be too narrow only */
       /* must 0 any don't care bits z bits in either */
       mask = (aw | ~bw) & (itemxsp->ap[0] | ~itemxsp->bp[0]);
       if ((((aw ^ itemxsp->ap[0]) | (bw ^ itemxsp->bp[0])) & mask) == 0)
        { __pop_xstk(); __pop_xstk(); return(csip->csist); }
       __pop_xstk();
      }
    }
   __pop_xstk();
   if (dflt_csip->csist != NULL) return(dflt_csip->csist);
   return(NULL);
  }
 /* case case 2: wider than 1 word32 */
 selwlen = wlen_(selxlen);
 for (; csip != NULL; csip = csip->csinxt)
  {
   for (xplp = csip->csixlst; xplp != NULL; xplp = xplp->xpnxt)
    {
     itemxsp = __eval_xpr(xplp->xp);

     /* SJM 12/12/03 - never can be real here using new all if any code */

     /* SJM 09/29/03 handle sign extension and separate cases */
     if (itemxsp->xslen > selxlen) __narrow_sizchg(itemxsp, selxlen);
     else if (itemxsp->xslen < selxlen)
      {
       if (xplp->xp->has_sign && !xplp->xp->unsgn_widen)
        __sgn_xtnd_widen(itemxsp, selxlen);
       else __sizchg_widen(itemxsp, selxlen);
      }

     for (i = 0; i < selwlen; i++)
      {
       mask = (selxsp->ap[i] | ~selxsp->bp[i]) & (itemxsp->ap[i]
        | ~itemxsp->bp[i]);
/* SJM 01/08/99 - WRONG - for wide if == 0 always matches first */
       if ((((selxsp->ap[i] ^ itemxsp->ap[i])
        | (selxsp->bp[i] ^ itemxsp->bp[i])) & mask) != 0) goto nxt_x;
      }
     __pop_xstk();
     __pop_xstk();
     return(csip->csist);

nxt_x:
     __pop_xstk();
    }
  }
 __pop_xstk();
 if (dflt_csip->csist != NULL) return(dflt_csip->csist);
 return(NULL);
}

/*
 * execute the wait statement (not really a loop)
 *
 * if expression T, execute immediately
 * else block until change of variable in expr.
 * set up special net pin list elements (like events) until change
 * evaluate and remove if T
 *
 */
static int32 exec_wait(register struct st_t *stp)
{
 int32 tmp, rv;
 i_tev_ndx tevpi;
 struct xstk_t *xsp;
 struct delctrl_t *dctp;
 
 xsp = __eval_xpr(stp->st.swait.lpx);
 dctp = stp->st.swait.wait_dctp;
 if (xsp->xslen <= WBITS) tmp = ((xsp->ap[0] & ~xsp->bp[0]) != 0L);

 if (xsp->xslen <= WBITS)
  {
   if (stp->st.swait.lpx->is_real)
    {
     double d1;
 
     memcpy(&d1, xsp->ap, sizeof(double));
     tmp = (d1 != 0.0);
     /* must not emit z bit warning for real */
     /* LOOKATME - changing part of stack since really done with it */
     xsp->bp[0] = 0; 
    }
   else tmp = ((xsp->ap[0] & ~xsp->bp[0]) != 0L);
  }
 else tmp = (__cvt_lngbool(xsp->ap, xsp->bp, wlen_(xsp->xslen)) == 1);

 if (tmp == 1)
  {
   if (!vval_is0_(xsp->bp, xsp->xslen))
    __sgfinform(404, "TRUE wait expression contains some x/z bits");
   __pop_xstk();

   /* RELEASE remove
   if (__debug_flg)
    __dmp_dcemsg(dctp, "setting wait dce to nil");
   --- */

   /* disarm for this instance - wait now past */
   /* first time thru will be nils but faster to just assign */
   dctp->dceschd_tevs[__inum] = -1;
   __cur_thd->th_dctp = NULL;

   if (__st_tracing)
    { strcpy(__xs2, "--continuing"); rv = TRUE; goto tr_done; }  
   return(TRUE);
  }

 __pop_xstk();
 /* because of fast tev reclaim scheme - allocate and assign new */
 /* tev each time through here */
 /* notice this does not link on list */
 alloc_tev_(tevpi, TE_THRD, __inst_ptr, __simtime);
 __cur_thd->thdtevi = tevpi;
 __tevtab[tevpi].tu.tethrd = __cur_thd;
 /* if rexecuting task, algorithm is to cancel previous pending */
 /* delay control and emit warning */
 if (dctp->dceschd_tevs[__inum] != -1)
  {
   if (__cur_thd->assoc_tsk == NULL)
    {
     __sgfwarn(635,
      "INTERNAL BUG? - when reexecuting in %s cancel and rearm of wait",
      __msg2_blditree(__xs, __inst_ptr));
    }  
   else
    {
     __sgfwarn(635, "when reexecuting task %s cancel and rearm of wait",
     __msg_blditree(__xs, __inst_ptr, __cur_thd->assoc_tsk));
    }
  }
 /* RELEASE remove ---
 if (__debug_flg)
  __dmp_dcemsg(dctp, "setting wait dce to event");
 --- */
 dctp->dceschd_tevs[__inum] = tevpi;
 __cur_thd->th_dctp = dctp;
 if (__st_tracing) { strcpy(__xs2, "--suspend"); rv = FALSE; goto tr_done; }
 /* this arms this instances delay control in case expr. changes */
 return(FALSE);

tr_done:
 if (__st_tracing)
  {
   __tr_msg("trace: %-7d wait (%s) [cond: %d] %s\n",
    __slin_cnt, __msgexpr_tostr(__xs, stp->st.swait.lpx), tmp, __xs2);
  }
 return(rv);
}

/*
 * execute a for statement header
 * for is [init. assign; while (cond. exp) { <stmt>; <inc. assign stmt>; }
 * notice unlike C both initial statement and inc. statement must be assigns
 *
 * know inc. executed before here and never seen
 */
static int32 for_not_done(struct for_t *frs)
{
 int32 tmp, has_xzs;
 word32 val;
 double d1;
 struct xstk_t *xsp;

 /* must move and execute for inc. at end not beginning of loop */
 has_xzs = FALSE;
 xsp = __eval_xpr(frs->fortermx);
 if (xsp->xslen <= WBITS)
  {
   /* SJM 07/20/00 - must convert to real if real */
   if (frs->fortermx->is_real)
    { memcpy(&d1, xsp->ap, sizeof(double)); tmp = (d1 != 0.0); }
   else
    {
     val = xsp->bp[0];
     tmp = ((xsp->ap[0] & ~val) != 0L);
     if (val != 0) { if (tmp == 0) tmp = 3; else has_xzs = TRUE; }
    }
  }
 else
  {
   tmp = __cvt_lngbool(xsp->ap, xsp->bp, wlen_(xsp->xslen));
   if (tmp == 1) { if (!vval_is0_(xsp->bp, xsp->xslen)) has_xzs = TRUE; }
  }

 if (__st_tracing)
  {
   __cur_sofs = 0; 
   __dmp_forhdr((FILE *) NULL, frs);
   __trunc_exprline(TRTRUNCLEN, FALSE);
   __tr_msg("trace: %-7d %s) [cond: %d]\n", 
    __slin_cnt, __exprline, tmp);
   __cur_sofs = 0;
  }
 __pop_xstk();
 if (tmp == 1)
  {
   if (has_xzs)
    {
     __sgfinform(405, "for condition true but has some x/z bits");
    }
   return(TRUE);
  }
 /* notice any 1 implies true so will not get here */
 if (tmp == 3)
  {
   __sgfinform(406,
    "for loop terminated by FALSE expressions containing x/z bits");
  }
 /* done with loop */
 return(FALSE);
}

/* notice non label begin block optimized away by here */

/*
 * USER TASK/FUNCTION EXECUTION ROUTINES
 */

/*
 * build named block thread structure and then execute the block
 */
static void exec_namblk(struct st_t *stp)
{
 struct task_t *tskp;
 struct thread_t *thp;

 tskp = stp->st.snbtsk;
 /* indent block and statements within */
 if (__st_tracing)
  {
   if (tskp->tsktyp == FORK) strcpy(__xs, "fork");
   else strcpy(__xs, "begin");  
   __tr_msg("trace: %-7d %s : %s\n", __slin_cnt, __xs,
    tskp->tsksyp->synam);
  }
 /* use sub thread scheduling routine but just build and exec immediately */
 __cur_thd->thofscnt = 1;
 __cur_thd->thnxtstp = stp->stnxt;

 /* create normal thread structure but exec immediately - no schedule */
 thp = __setup_tsk_thread(tskp);

 thp->thpar = __cur_thd;
 __cur_thd->thofs = thp;
 /* move down but notice never an xmr instance loc. change */
 __cur_thd = thp;
 __cur_thd->th_itp = __inst_ptr; 
 /* DBG remove -- */
 if (__cur_thd->thnxtstp == NULL) __misc_terr(__FILE__, __LINE__);
 /* __dmp_tskthd(tskp, __inst_mod); */
 /* --- */
 /* always continue with down 1 thread - need thread only for possible */
 /* disable of named block */ 
}

/*
 * exec a task or named block as subthread of __cur_thd
 * stp is place to begin after completion
 * returns pointer to new sub thread 
 *
 * not used for fork-join because all fork join sub threads must be scheduled
 * caller must set current thread fields
 * will never see simple unnamed begin-end blocks here
 */
extern struct thread_t *__setup_tsk_thread(struct task_t *tskp)
{
 register struct thread_t *thp;
 register struct tskthrd_t *ttp;

 /* allocate a new thread */
 thp = __alloc_thrd();
 thp->thenbl_sfnam_ind = __sfnam_ind;
 thp->thenbl_slin_cnt = __slin_cnt;

 /* DBG remove -- */
 if (tskp == NULL) __arg_terr(__FILE__, __LINE__);
 /* --- */

 /* if task, set next stmt to first of task and link thread on tasks list */
 /* for disable, task list of all threads and thread to task link  */  
 /* schedule of task conflicts with thread task */
 /* DBG remove --- */
 if (thp->assoc_tsk != NULL && thp->assoc_tsk != tskp)
  __misc_sgfterr(__FILE__, __LINE__);
 /* --- */

 thp->thnxtstp = tskp->tskst;

 /* put on front of task thread lists */
 ttp = (struct tskthrd_t *) __my_malloc(sizeof(struct tskthrd_t)); 
 ttp->tthd_l = NULL;
 ttp->tthd_r = tskp->tthrds[__inum];
 tskp->tthrds[__inum] = ttp;
 if (ttp->tthd_r != NULL) ttp->tthd_r->tthd_l = ttp;
 ttp->tthrd = thp;
 thp->assoc_tsk = tskp;
 /* set the one list element that this thread connects to */
 thp->tthlst = ttp;
 return(thp);
}

/*
 * allocate threads and schedule execution of a fork-join 
 * know __cur_thd will be named block thread for label fork join - else
 * current thread
 *
 * no assoc. task since disabling fork-join label block disable 1 up normal
 * label block not fork join
 */
extern void __sched_fork(register struct st_t *stp)
{
 register int32 fji;
 register struct thread_t *thp;
 int32 sav_slin_cnt, sav_sfnam_ind;
 struct thread_t *last_thp;
 struct st_t *fjstp;

 /* DBG remove */
 if (__cur_thd->thofscnt != 0) __misc_terr(__FILE__, __LINE__); 
 /* --- */ 
 /* convert current thread (one-up) to fork joint32 header */
 /* and build (link in) list of per statement threads */  
 last_thp = NULL;
 for (fji = 0;; fji++)
  {
   if ((fjstp = stp->st.fj.fjstps[fji]) == NULL) break;
   
   /* SJM 03/07/02 - for optimizer must always schedule 1st stmt of unblk */
   /* instead of unnamed blk as now */
   if (fjstp->stmttyp == S_UNBLK) fjstp = fjstp->st.sbsts;
   
   /* using location of fork-join statement as enable loc. not fork loc. */
   sav_sfnam_ind = __sfnam_ind;
   sav_slin_cnt = __slin_cnt;
   __sfnam_ind = fjstp->stfnam_ind;
   __slin_cnt = fjstp->stlin_cnt;
   /* schedule each subthread after building it */
   thp = sched_fj_subthread(fjstp);

   __sfnam_ind = sav_sfnam_ind;
   __slin_cnt = sav_slin_cnt;

   __cur_thd->thofscnt += 1;
   if (last_thp == NULL) __cur_thd->thofs = thp;
   else { thp->thleft = last_thp; last_thp->thright = thp; }
   thp->thpar = __cur_thd;
   thp->th_itp = __inst_ptr;
   /* flag on fork-join component to indicate must look for assoc tsk up */
   thp->th_fj = TRUE;
   last_thp = thp;
  }
}

/*
 * setup and schedule execution of one fork-join subthread of __cur_thd
 *
 * stp is place to begin when event processed
 * returns pointer to thread value in scheduled event
 * caller must set current thread fields
 */
static struct thread_t *sched_fj_subthread(struct st_t *stp)
{
 register struct thread_t *thp;
 i_tev_ndx tevpi;

 /* allocate a new thread */
 thp = __alloc_thrd();
 thp->thenbl_sfnam_ind = __sfnam_ind;
 thp->thenbl_slin_cnt = __slin_cnt;

 /* set the one fj statement (or list) as next stmt of subthread */
 thp->thnxtstp = stp;
 
 /* allocate an event for this fork-join component statement */
 /* at end of current time slot */
 alloc_tev_(tevpi, TE_THRD, __inst_ptr, __simtime);

 /* link thread back to event */
 thp->thdtevi = tevpi;
 __tevtab[tevpi].tu.tethrd = thp;

 if (__debug_flg && __st_tracing)
  {
   __tr_msg("trace: %-7d -- schedule new subthread at %s continue at %s\n",
    __slin_cnt, __bld_lineloc(__xs, thp->thenbl_sfnam_ind,
    thp->thenbl_slin_cnt), __bld_lineloc(__xs2, stp->stfnam_ind,
    stp->stlin_cnt));
  }

 /* this must go on front because interactive statement must complete */
 __add_ev_to_front(tevpi);
 return(thp);
}

/*
 * add an event to front of current queue
 * for various procedural control - must go front so interactive completes
 * before any other events processed
 *
 * scheme is to always execute procedural as soon as possible
 * but declarative as late as possible
 */
extern void __add_ev_to_front(register i_tev_ndx tevpi)
{
 if (!__processing_pnd0s)
  {
   /* adding to front is just after current since now processing current */
   if (__cur_tevpi == -1)
    {
     if (__cur_te_hdri == -1) __cur_te_hdri = __cur_te_endi = tevpi; 
     else { __tevtab[tevpi].tenxti = __cur_te_hdri; __cur_te_hdri = tevpi; }
    }
   else
    {
     if (__cur_tevpi != __cur_te_endi)
      __tevtab[tevpi].tenxti = __tevtab[__cur_tevpi].tenxti;
     else __cur_te_endi = tevpi;
     __tevtab[__cur_tevpi].tenxti = tevpi;
    }
   __num_twhevents++;
   /* need to make sure number of timing wheel events matches cur number */
   __twheel[__cur_twi]->num_events += 1;
  }
 else
  {
   /* also in pnd 0's front is just after current if set */
   if (__cur_tevpi == -1)
    {
     /* notice during add net chg elements cur tevp nil, so common to add */
     /* to end */ 
     if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevpi; 
     else { __tevtab[tevpi].tenxti = __p0_te_hdri; __p0_te_hdri = tevpi; }
    }
   else
    {
     if (__cur_tevpi != __p0_te_endi)
      __tevtab[tevpi].tenxti = __tevtab[__cur_tevpi].tenxti;
     else __p0_te_endi = tevpi;
     __tevtab[__cur_tevpi].tenxti = tevpi;
    }
   /* this does not go on timing wheel or get counted */
  }
}

/*
 * allocate a new thread
 */
extern struct thread_t *__alloc_thrd(void)
{
 register struct thread_t *thp;

 thp = (struct thread_t *) __my_malloc(sizeof(struct thread_t));
 init_thrd(thp);
 return(thp);
}

/*
 * initialize a new thread
 */
static void init_thrd(register struct thread_t *thp)
{
 thp->tsk_stouts = FALSE;
 thp->th_dsable = FALSE;
 thp->th_rhsform = FALSE;
 thp->th_fj = FALSE;
 thp->th_ialw = FALSE;
 thp->th_postamble = FALSE;
 /* off-spring count is 0 unless incremented when sub thread created */  
 thp->thofscnt = 0;
 thp->thnxtstp = NULL;
 thp->thpar = thp->thright = thp->thleft = thp->thofs = NULL;
 thp->tthlst = NULL;
 thp->assoc_tsk = NULL;
 thp->th_dctp = NULL;
 thp->thdtevi = -1;
 thp->thenbl_sfnam_ind = 0;
 thp->thenbl_slin_cnt = 0;
 thp->th_rhswp = NULL;
 thp->th_rhswlen = -1;
 thp->th_itp = NULL;
 thp->th_hctrl = NULL;
}

/*
 * execute a task call
 *
 * thread suspend mechanism set up but works by continuing with first
 * stmt in tsk body (can be execed as iop) - thread mechanism fixed 
 * up so suspend works right including handling of disable and tsk outs
 *
 * know for user tasks, argument list exactly matches definition list 
 * can improve this by preprocessing call/return evaluation
 */
extern struct st_t *__exec_tskcall(struct st_t *stp)
{
 register struct expr_t *xp;
 register struct task_pin_t *tpp;
 int32 argi;
 struct tskcall_t *tkcp;
 struct expr_t *tkxp, *rhsxp;
 struct sy_t *syp;
 struct task_t *tskp;
 struct xstk_t *xsp;
 struct net_t *np;
 struct thread_t *thp;
 struct itree_t *tsk_itp;
 struct st_t *stp2;

 tkcp = &(stp->st.stkc);
 tkxp = tkcp->tsksyx;
 syp = tkxp->lu.sy;

 if (syp->sytyp == SYM_STSK)
  {
   /* no time movement in system tasks */
   /* return NULL, to suspend thread - non null to continue as to next st */
   /* this does own tracing */
   if (__st_tracing)
    {
     __cur_sofs = 0;
     __dmp_tskcall((FILE *) NULL, stp);
     __trunc_exprline(TRTRUNCLEN, FALSE);
     __tr_msg("trace: %-7d %s\n", __slin_cnt,
       __exprline);
     __cur_sofs = 0;
    }
   return(__exec_stsk(stp, syp, tkcp));
  }
 tskp = syp->el.etskp;
 tpp = tskp->tskpins;

 if (tkxp->optyp == GLBREF)
  {
   /* must get tsk exec itree location but cannot change to yet */
   __xmrpush_refgrp_to_targ(tkxp->ru.grp);
   tsk_itp = __inst_ptr;
   /* notice need to print new location for xmr task */
   if (__st_tracing) tr_resume_msg();
   __pop_itstk();
  }
 else tsk_itp = NULL;
 if (__st_tracing)
  {
   __cur_sofs = 0;
   __adds("<** enabling task ");
   __adds(__to_idnam(tkxp));
   addch_('(');
  }

 /* must assign to task variables since values persist */
 /* user tasks are value-result */
 argi = 0;
 for (xp = tkcp->targs; xp != NULL; xp = xp->ru.x, tpp = tpp->tpnxt, argi++)
  {
   if (tpp->trtyp != IO_OUT)
    {
     /* assign rhs in or inout arg. expr. to task local variable */
     np = tpp->tpsy->el.enp;
     rhsxp = xp->lu.x;
     xsp = __eval_xpr(rhsxp);
     eval_tskassign_rhsexpr(xsp, (np->ntyp == N_REAL), np->nwid,
      (rhsxp->is_real == 1), (rhsxp->has_sign == 1));
     if (__st_tracing) tradd_tf_argval(argi, np, xsp);
     /* if xmr call, afer eval in cur. itree loc. must store in xmr dest */
     if (tsk_itp != NULL)
      {
       __push_itstk(tsk_itp);
       __chg_st_val(np, xsp->ap, xsp->bp);
       __pop_itstk();
      }
     else __chg_st_val(np, xsp->ap, xsp->bp);
     __pop_xstk();
    }
   else if (__st_tracing)
    {
     /* for tracing output value on entry, need caller's itree loc. */
     np = tpp->tpsy->el.enp;
     push_xstk_(xsp, np->nwid);
     __ld_wire_val(xsp->ap, xsp->bp, np);
     tradd_tf_argval(argi, np, xsp);
     __pop_xstk();
    }
  }
 if (__st_tracing)
  {
   __adds(")"); 
   __trunc_exprline(TRTRUNCLEN, FALSE);
   __tr_msg("trace: %-7d %s\n", __slin_cnt, __exprline);
   __cur_sofs = 0;
  }

 /* use sub thread scheduling routine but just build and exec immediately */
 __cur_thd->thofscnt = 1;

 /* if xmr task call replace top of instance stack here - cur_thd has up 1 */
 if (tsk_itp != NULL) { __pop_itstk(); __push_itstk(tsk_itp); }

 /* trick here is that must not advance statement since need to store */
 thp = __setup_tsk_thread(tskp);

 if (tskp->thas_outs || __st_tracing)
  {
   thp->tsk_stouts = TRUE;
   /* must set thrd nxt stmt to the task call so can find task after */ 
   /* task completed suspend so can find tsk to set out params in */
   /* fixup to skip of non loop end gotos after tsk outs stored */
   __cur_thd->thnxtstp = stp;
   __cur_thd->th_postamble = TRUE;
  }
 /* SJM 04/05/02 - skip over all non loop end gotos so can exec actual stmt */
 else
  {
   stp2 = stp->stnxt;
   if (stp2 == NULL) __cur_thd->thnxtstp = NULL;
   else if (stp2->stmttyp != S_GOTO) __cur_thd->thnxtstp = stp2;
   else if (stp2->lpend_goto) __cur_thd->thnxtstp = stp2;
   else
    {
     for (;;)
      {
       /* know on entry stp2 goto */
       stp2 = stp2->st.sgoto;
       if (stp2 == NULL || stp2->stmttyp != S_GOTO)
        { __cur_thd->thnxtstp = stp2; break; }
       if (stp2->lpend_goto) { __cur_thd->thnxtstp = stp2; break; }
      }
    }
  }

 thp->thpar = __cur_thd;
 __cur_thd->thofs = thp;
 /* make task thread current */
 __cur_thd = thp;
 __cur_thd->th_itp = __inst_ptr;
 /* DBG remove ---
 __dmp_tskthd(tskp, __inst_mod);
 --- */
 return(thp->thnxtstp);
}

/*
 * print a task or function argument value    
 */
static void tradd_tf_argval(int32 argi, struct net_t *np, struct xstk_t *xsp)
{
 char s1[RECLEN];
 int32 signv, base;

 if (argi != 0) __adds(", "); 
 if (!vval_is0_(xsp->bp, xsp->xslen) && np->ntyp != N_REAL)
  {
   sprintf(s1, "%d'h", xsp->xslen); 
   __adds(s1);
   __regab_tostr(s1, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE);
  }
 else
  {
   signv = FALSE; base = BHEX;
   if (np->ntyp == N_REAL) base = BDBLE; 
   else if (np->n_signed) { base = BDEC; signv = TRUE; } 
   __regab_tostr(s1, xsp->ap, xsp->bp, xsp->xslen, base, signv);
  }
 __adds(s1);
}

/*
 * store task return output parameters
 *
 * if disabled never get here
 * tricky because must eval using tos itree loc. but assign to 1 under
 * also get here even if no out args but statement tracing on
 *
 * this is called from task thread and itree loc. but assigns to one up
 * thread and itree location
 */
static void store_tskcall_outs(struct st_t *tskcall_stp) 
{
 register struct expr_t *xp;
 register struct task_pin_t *tpp;
 int32 first_time, base, signv;
 struct tskcall_t *tkcp;
 struct expr_t *tkxp, *lhsxp;
 struct task_t *tskp;
 struct xstk_t *xsp;
 struct net_t *np;
 char s1[RECLEN];

 tkcp = &(tskcall_stp->st.stkc);
 tkxp = tkcp->tsksyx;

 tskp = tkxp->lu.sy->el.etskp;
 if (__st_tracing)
  {
   __cur_sofs = 0;
   __adds("**> returning from task ");
   __adds(__to_idnam(tkxp));
   addch_('(');
  }
 tpp = tskp->tskpins;
 first_time = TRUE;
 for (xp = tkcp->targs; xp != NULL; xp = xp->ru.x, tpp = tpp->tpnxt)
  {
   if (tpp->trtyp == IO_IN) continue;

   /* assign task local param var. value to lhs call argument */
   /* xp->lu.x is rhs src., np is lhs dest. var. */
   np = tpp->tpsy->el.enp;
   push_xstk_(xsp, np->nwid);
   /* need load value here because, need to decode storage rep */
   __ld_wire_val(xsp->ap, xsp->bp, np);
   lhsxp = xp->lu.x;
   eval_tskassign_rhsexpr(xsp, (lhsxp->is_real == 1), lhsxp->szu.xclen,
    (np->ntyp == N_REAL), (np->n_signed == 1));
   /* np here is rhs */
   if (__st_tracing) 
    {
     if (first_time) first_time = FALSE; else __adds(", "); 
     if (np->ntyp != N_REAL && !vval_is0_(xsp->bp, xsp->xslen))
      {
       sprintf(s1, "%d'h", xsp->xslen); 
       __adds(s1);
       __regab_tostr(s1, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE);
      }
     else
      {
       signv = FALSE; base = BHEX;
       if (np->ntyp == N_REAL) base = BDBLE; 
       else if (np->n_signed) { base = BDEC; signv = TRUE; } 
       __regab_tostr(s1, xsp->ap, xsp->bp, xsp->xslen, base, signv); 
      }
     __adds(s1);
    } 
   /* notice for xmr task enable, must eval in task itree place */
   /* but store top of expr. stack in calling itree place */
   if (tkxp->optyp == GLBREF)
    {
     __push_itstk(__cur_thd->thpar->th_itp);
     __exec2_proc_assign(lhsxp, xsp->ap, xsp->bp);
     __pop_itstk();
    }
   else __exec2_proc_assign(lhsxp, xsp->ap, xsp->bp);
   __pop_xstk();
  }
 if (__st_tracing)
  {
   __adds(")"); 
   __trunc_exprline(TRTRUNCLEN, FALSE);
   __tr_msg("trace: %-7d %s\n", __slin_cnt, __exprline);
   __cur_sofs = 0;
  }
}

/*
 * execute a user function call operator in an expression
 * ndp is FCALL expression node - ru is operand list
 * user functions only take input args 
 * notice local variables presist and puts return value on top of expr stk 
 */
extern void __exec_func(register struct expr_t *ndp) 
{
 register struct expr_t *argxp;
 register struct task_pin_t *tpp;
 int32 savslin_cnt, savsfnam_ind, nd_thdfree;
 struct sy_t *fsyp;
 struct itree_t *func_itp, *xmr_savitp;
 struct st_t *stp;
 struct task_t *tskp;
 struct tev_t *tevp;
 int32 argi;
 struct gref_t *grp;
 struct xstk_t *xsp;
 struct net_t *np;
 struct expr_t *rhsxp;
 
 /* SJM 05/22/05 - no reason to pass func symbol - just get from expr node */  
 fsyp = ndp->lu.x->lu.sy; 

 /* for decl. rhs, maybe no thrd - bld for 1st call else take over cur. */ 
 nd_thdfree = FALSE;
 if (__cur_thd == NULL)
  {
   __cur_thd = __alloc_thrd();
   __cur_thd->th_itp = __inst_ptr;
   nd_thdfree = TRUE;
  }
 /* DBG remove --- */
 else if (__cur_thd->th_itp != __inst_ptr) __misc_terr(__FILE__, __LINE__);
 /* --- */ 

 /* function source will be dump later */ 
 if (__st_tracing)
  {
   __cur_sofs = 0;
   __adds("<** calling function ");
   __adds(__to_idnam(ndp->lu.x));
   addch_('(');
  }

 xmr_savitp = __inst_ptr;
 /* function call prep. block */

 argi = 0;
 tskp = fsyp->el.etskp;
 tpp = tskp->tskpins->tpnxt;
 /* if global, local variables accessed from target (defining mod) inst */
 if (ndp->lu.x->optyp == GLBREF)
  {
   grp = ndp->lu.x->ru.grp;
   __xmrpush_refgrp_to_targ(grp);
   if (__st_tracing) tr_resume_msg();
   func_itp = __inst_ptr;
   /* cannot change to func xmr place yet */
   __pop_itstk();
  }
 else func_itp = NULL;

 /* evaluate and store input params */
 /* 1st tpp is by convention is return value but 1st arg is real arg */
 /* know number matches exactly (no ,,) or will not get here */ 
 argi = 0;
 for (argxp = ndp->ru.x; argxp != NULL; argxp = argxp->ru.x,
  tpp = tpp->tpnxt, argi++)
  {
   /* if xmr call, must eval. these in current not func. */
   /* assign rhs in or inout arg. expr. to task local variable */
   np = tpp->tpsy->el.enp;
   rhsxp = argxp->lu.x;
   xsp = __eval2_xpr(rhsxp);
   eval_tskassign_rhsexpr(xsp, (np->ntyp == N_REAL), np->nwid,
    (rhsxp->is_real == 1), (rhsxp->has_sign == 1));

   if (__st_tracing) tradd_tf_argval(argi, np, xsp);

   /* notice can cause propagate reg xmr on rhs that is function arg */
   /* if xmr need to store in down itree location */
   if (func_itp != NULL)
    {
     __push_itstk(func_itp);
     __chg_st_val(np, xsp->ap, xsp->bp);
     __pop_itstk();
    }
   else __chg_st_val(np, xsp->ap, xsp->bp);
   __pop_xstk();
  }

 if (__st_tracing)
  {
   __adds(")"); 
   __trunc_exprline(TRTRUNCLEN, FALSE);
   __tr_msg("trace: %-7d %s\n", __slin_cnt, __exprline);
   __cur_sofs = 0;
  }
 /* this is dynamic call list */
 if (++__fcspi >= __maxfcnest) grow_fcstk();
 __fcstk[__fcspi] = tskp;
 savslin_cnt = __slin_cnt;
 savsfnam_ind = __sfnam_ind;
 /* if xmr function call replace top - relative xmr's not though itstk */ 
 if (func_itp != NULL)
  { __pop_itstk(); __push_itstk(func_itp); __cur_thd->th_itp = __inst_ptr; }

 /* cannot schedule and resume inside func. so suspend and schedule */
 /* then unsuspend and cancel event */
 stp = tskp->tskst;
 __cur_thd->thnxtstp = stp;
 /* if stepping, make sure first execed */
 if (__single_step) __step_from_thread = FALSE;

again:
 __stmt_suspend = FALSE;
 /* step returns after 1 statement (to new line) or end of func */
 if (__single_step && __cur_thd->th_hctrl == NULL) step_exec_stmt(stp);
 else if (__st_tracing || __single_step)
  {
    brktr_exec_stmts(stp);
  }
 else exec_stmts(stp);

 /* happens if hit break or step or ^c hit - suspend routine just execed */
 if (__stmt_suspend)
  {
   __do_interactive_loop();
   /* tricky code for func - restart and cancel scheduled resume event */
   /* DBG remove --- */  
   if (__fsusp_tevpi == -1) __misc_terr(__FILE__, __LINE__);

   tevp = &(__tevtab[__fsusp_tevpi]);
   if (__inst_ptr != tevp->teitp || __inst_ptr != __suspended_itp) 
    __misc_terr(__FILE__, __LINE__);
   /* --- */
   /* restore from func. suspended event and cancel event */
   __cur_thd = tevp->tu.tethrd;
   tevp->te_cancel = TRUE;
   __fsusp_tevpi = -1L;

   /* undo suspend */
   __cur_thd->thdtevi = -1;
   __suspended_thd = NULL;
   __suspended_itp = NULL;
   stp = __cur_thd->thnxtstp;
   goto again;
  }
 
 __slin_cnt = savslin_cnt;
 __sfnam_ind = savsfnam_ind;
 __fcspi--;

 /* SJM 05/12/03 - do not need block here */
 /* return block */
 /* put assign func return variable (func. name) value on tos */
 /* key here is task name local variable has declaration from func hdr */
 /* notice ok if not assigned, to will just return x */
 np = tskp->tskpins->tpsy->el.enp;
 push_xstk_(xsp, np->nwid);
 /* caller must intepret type of value on tos */
 /* for itp this must be loaded from dest. */
 __ld_wire_val(xsp->ap, xsp->bp, np);
 /* if xmr replace top with original and put back thread itp */ 
 if (func_itp != NULL)
 { __pop_itstk(); __push_itstk(xmr_savitp); __cur_thd->th_itp = __inst_ptr; }

 if (__st_tracing)
  {
   int32 signv, base;

   if (np->ntyp != N_REAL && !vval_is0_(xsp->bp, xsp->xslen))
    {
     sprintf(__xs2, "%d'h%s", xsp->xslen, __regab_tostr(__xs, xsp->ap,
      xsp->bp, xsp->xslen, BHEX, FALSE));
    }
   else 
    {
     signv = FALSE; base = BHEX;
     if (np->ntyp == N_REAL) base = BDBLE; 
     else if (np->n_signed) { base = BDEC; signv = TRUE; } 
     __regab_tostr(__xs2, xsp->ap, xsp->bp, xsp->xslen, base, signv);
    }
   __tr_msg("trace: %-7d **> [%s] returned by function %s\n",
    __slin_cnt, __xs2, __to_idnam(ndp->lu.x));
  }

 if (nd_thdfree)
  {
   __my_free((char *) __cur_thd, sizeof(struct thread_t));
   __cur_thd = NULL;
  } 
 return;
}


/*
 * routine to grow fcstk (function call no display local variables)
 */
static void grow_fcstk(void)
{
 register int32 i;
 int32 old_maxnest;
 int32 osize, nsize;

 old_maxnest = __maxfcnest;
 osize = old_maxnest*sizeof(struct task_t *);
 /* grow by 50% after certain point */
 if (__maxfcnest >= 2000) __maxfcnest += __maxfcnest/2;
 else __maxfcnest *= 2;
 nsize = __maxfcnest*sizeof(struct task_t *);
 __fcstk = (struct task_t **) __my_realloc((char *) __fcstk, osize, nsize);
 for (i = old_maxnest; i < __maxfcnest; i++) __fcstk[i] = NULL;
 if (__debug_flg)
  __dbg_msg("+++ fcall stack grew from %d bytes to %d\n", osize, nsize); 
}

/*
 * execute a system function call operator
 * ndp is actual FCALL node
 * leaves return value on top of expr. stack but does not return it
 */
extern void __exec_sysfunc(register struct expr_t *ndp) 
{
 register struct xstk_t *xsp, *xsp2;
 register struct expr_t *fax;
 int32 ival, fd, c;
 word32 uval;
 word64 timval;
 double d1;
 struct sy_t *fsyp;
 struct sysfunc_t *sfbp;

 /* SJM 05/22/05 - no reason to pass func symbol - just get from expr node */  
 fsyp = ndp->lu.x->lu.sy; 

 sfbp = fsyp->el.esyftbp;
 switch (sfbp->syfnum) {
  /* functions that take exactly one argument */
  case STN_FOPEN:
   /* AIV 09/08/03 - changed now can take one or 2 args for OS FILE * */
   /* fd = $fopen([filen name]) or fd = $fopen([file name], [I/O mode]) */
   fax = ndp->ru.x->lu.x;
   /* 2nd arg empty also must be interpreted as MCD open */
   if (ndp->ru.x->ru.x != NULL && ndp->ru.x->lu.x->optyp != OPEMPTY)
    {
     uval = fio_do_fopen(fax, ndp->ru.x->ru.x->lu.x);
    }
   else uval = mc_do_fopen(fax);

   push_xstk_(xsp, WBITS);
   xsp->ap[0] = (word32) uval;  
   xsp->bp[0] = 0L;
   break; 
  /* AIV 09/08/03 - new fileio system functions */
  case STN_FGETC:
   /* c = $fgetc([fd expr]) */
   /* on error, this sets errno OS state var */
   if ((fd = chk_get_ver_fd(ndp->ru.x->lu.x)) == -1) c = -1;
   else
    {
     /* on error, this return EOF (-1) and sets OS err number state var */
     c = fgetc(__fio_fdtab[fd]->fd_s);
    }
   push_xstk_(xsp, WBITS);
   /* -1 becomes correct all word32 all 1's until new signed added */
   xsp->ap[0] = (word32) c;  
   xsp->bp[0] = 0L;
   break;
  case STN_UNGETC:
   /* c = $ungetc([put back ch], [fd expr]) */
   /* know exactly 2 args or won't get here */
   ival = fio_ungetc(ndp->ru.x->lu.x, ndp->ru.x->ru.x->lu.x);
   push_xstk_(xsp, WBITS);
   /* -1 becomes correct all word32 all 1's until new signed added */
   xsp->ap[0] = (word32) ival;  
   xsp->bp[0] = 0L;
   break;
  case STN_FGETS:
   /* format: cnt = $fgets([lhs proc str expr], [fd expr]) */
   /* know exactly 2 args or won't get here - returns 0 or num chs read */
   ival= fio_fgets(ndp->ru.x->lu.x, ndp->ru.x->ru.x->lu.x);
   push_xstk_(xsp, WBITS);
   /* 0 on error esle number of chars read */
   xsp->ap[0] = (word32) ival;  
   xsp->bp[0] = 0L;
   break;
  case STN_FTELL:
   /* format: fpos = $ftell([fd expr]) */
   /* on error, this sets errno OS state var */
   if ((fd = chk_get_ver_fd(ndp->ru.x->lu.x)) == -1) ival = -1;
   else
    {
     /* on error, this return EOF (-1) and sets OS err number state var */
     ival = ftell(__fio_fdtab[fd]->fd_s);
    }
   push_xstk_(xsp, WBITS);
   /* -1 becomes correct all word32 all 1's until new signed added */
   xsp->ap[0] = (word32) ival;
   xsp->bp[0] = 0L;
   break;
  case STN_REWIND:
   /* format: fpos = $rewind([fd expr]) */
   ival = fio_rewind(ndp->ru.x->lu.x);
   push_xstk_(xsp, WBITS);
   /* -1 becomes correct all word32 all 1's until new signed added */
   xsp->ap[0] = (word32) ival;
   xsp->bp[0] = 0L;
   break;
  case STN_FSEEK:
   /* format: fpos = $fseek([fd expr], [ofs expr], [whence expr]) */
   /* syntax error does not have 3 args caught during check (in v_fx3.c) */
   ival = fio_fseek(ndp->ru.x->lu.x, ndp->ru.x->ru.x->lu.x,
    ndp->ru.x->ru.x->ru.x->lu.x);
   push_xstk_(xsp, WBITS);
   /* -1 becomes correct all word32 all 1's until new signed added */
   xsp->ap[0] = (word32) ival;
   xsp->bp[0] = 0L;
   break;
  case STN_FERROR:
   /* format: errnum = $ferror([fd expr], [lhs proc string]) */
   ival = fio_ferror(ndp->ru.x->lu.x, ndp->ru.x->ru.x->lu.x);
   push_xstk_(xsp, WBITS);
   /* -1 becomes correct all word32 all 1's until new signed added */
   xsp->ap[0] = (word32) ival;
   xsp->bp[0] = 0L;
   break;
  case STN_FREAD:
   /* [num chars read] = $fread([lhs proc reg expr], [fd expr]) */
   /* [num chars read] = $fread([mem name], [fd expr], [{starg}, {count}]) */
   ival = fio_fread(ndp->ru.x);
   push_xstk_(xsp, WBITS);
   /* -1 becomes correct all word32 all 1's until new signed added */
   xsp->ap[0] = (word32) ival;
   xsp->bp[0] = 0L;
   break;
  case STN_FSCANF:
   /* [num matched flds] = $fscanf([fd], [format], ...) */
   ival = fio_fscanf(ndp->ru.x);
   push_xstk_(xsp, WBITS);
   /* -1 becomes correct all word32 all 1's until new signed added */
   xsp->ap[0] = (word32) ival;
   xsp->bp[0] = 0L;
   break;
 case STN_SSCANF:
   /* [num matched flds] = $sscanf([string expr], [format], ...) */
   ival = fio_sscanf(ndp->ru.x);
   push_xstk_(xsp, WBITS);
   /* -1 becomes correct all word32 all 1's until new signed added */
   xsp->ap[0] = (word32) ival;
   xsp->bp[0] = 0;
   break;
  case STN_STIME: 
  case STN_TIME:
   /* convert ticks to user time (maybe smaller) and return WBIT form */
   /* with warn if does not fit */
   /* this can be 0 - know conversion to user time always succeeds */
   if (!__inst_mod->mno_unitcnv)
    __cnv_ticks_tonum64(&timval, __simtime, __inst_mod);
   else timval = __simtime;
   if (sfbp->syfnum == STN_STIME)
    { 
     push_xstk_(xsp, WBITS);
     if (timval > WORDMASK_ULL)
      {
       __sgfinform(411, "system function %s result does not fit in %d bits",
        fsyp->synam, WBITS);
      }
     xsp->ap[0] = (word32) (timval & WORDMASK_ULL);
     xsp->bp[0] = 0L;
    }
   else
    {
     push_xstk_(xsp, TIMEBITS);
     xsp->ap[0] = (word32) (timval & WORDMASK_ULL);
     xsp->ap[1] = (word32) ((timval >> 32) & WORDMASK_ULL);
     xsp->bp[0] = xsp->bp[1] = 0L;
    }
   break;
  case STN_REALTIME:
   /* for time as user world (unscaled) time, must convert to real first */
   d1 =__unscale_realticks(&__simtime, __inst_mod);
   push_xstk_(xsp, WBITS);
   /* copy from 1st to 2nd */
   memcpy(xsp->ap, &d1, sizeof(double));
   break;
  case STN_STICKSTIME:
   push_xstk_(xsp, WBITS);
   if (__simtime > WORDMASK_ULL)
    {
     __sgfinform(411, "system function %s result does not fit in %d bits",
      fsyp->synam, WBITS);
    }
   xsp->ap[0] = (word32) (__simtime & WORDMASK_ULL);
   xsp->bp[0] = 0L;
   break;
  case STN_TICKSTIME:
   push_xstk_(xsp, TIMEBITS);
   xsp->ap[0] = (word32) (__simtime & WORDMASK_ULL);
   xsp->ap[1] = (word32) ((__simtime >> 32) & WORDMASK_ULL);
   xsp->bp[0] = xsp->bp[1] = 0L;
   break;
  case STN_BITSTOREAL:
   /* this converts the a parts of a 64 bit reg to a wbit real */ 
   /* know this will be 64 bit or previous error */
   fax = ndp->ru.x->lu.x;
   xsp = __eval_xpr(fax);
   if (xsp->xslen != 64)
    {
     __sgfwarn(636, "$bitstoreal of %s value not 64 bits - set to 0",
      __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
conv_0:
     d1 = 0.0;
     memcpy(xsp->ap, &d1, sizeof(double));
     /* SJM 07/05/03 - need to also adjust b part for real */
     xsp->xslen = WBITS;
     xsp->bp = &(xsp->ap[1]);
     break;
    } 

   /* notice must silently convert x to 0.0, since port will start at x */
   if (!vval_is0_(xsp->bp, xsp->xslen)) goto conv_0;
   /* finally, convert to real - assuming bits good - should convert to */
   /* something and see what error code is set ? */
   /* this is stupid but allow looking at the number in debugger */  
   memcpy(&d1, xsp->ap, sizeof(double)); 
   /* DBG - LOOKATME - why here 
   if (finite(d1) == 0) __arg_terr(__FILE__, __LINE__);
   -- */
   memcpy(xsp->ap, &d1, sizeof(double)); 

   xsp->bp = &(xsp->ap[1]);
   xsp->xslen = WBITS;
   break;
  case STN_REALTOBITS:
   push_xstk_(xsp, 64);
   fax = ndp->ru.x->lu.x;
   xsp2 = __eval_xpr(fax);
   /* notice double stored with b (x/z) part as WBITS really 64 */
   xsp->ap[0] = xsp2->ap[0];
   xsp->ap[1] = xsp2->bp[0];
   xsp->bp[0] = xsp->bp[1] = 0L;
   __pop_xstk();
   break;
  case STN_ITOR:
   /* know arg must be 32 or narrower */
   fax = ndp->ru.x->lu.x;
   xsp = __eval_xpr(fax);
   if (xsp->bp[0] != 0L)
    {
     __sgfwarn(631,
      "system function %s argument %s x/z value converted to 0.0", 
       fsyp->synam, __msgexpr_tostr(__xs, fax));
     d1 = 0.0;
    }
   else
    {
     if (fax->has_sign) { ival = (int32) xsp->ap[0]; d1 = (double) ival; } 
     else d1 = (double) xsp->ap[0];
    }
   /* notice reusing xsp since know size of both is WBITS */
   memcpy(xsp->ap, &d1, sizeof(double));
   break;
  case STN_RTOI:
   /* think semantics is to convert keeping sign - number maybe 2's compl */
   fax = ndp->ru.x->lu.x;
   xsp = __eval_xpr(fax);
   memcpy(&d1, xsp->ap, sizeof(double));
   /* DBG - LOOKATME - why here 
   if (finite(d1) == 0) __arg_terr(__FILE__, __LINE__);
   -- */
   ival = (int32) d1;
   /* reuse expr. that know is WBITS */ 
   xsp->bp[0] = 0L;
   xsp->ap[0] = (word32) ival;
   break;
  case STN_SIGNED:
   /* this must eval its argument and then return its value */
   /* signed is just marking expr - no bit pattern change */
   fax = ndp->ru.x->lu.x;
   xsp = __eval_xpr(fax);
   /* 05/26/04 - may need size change here to mach fcall node size */
   if (xsp->xslen != ndp->szu.xclen)
    {
     if (xsp->xslen < ndp->szu.xclen) __sgn_xtnd_wrd(xsp, ndp->szu.xclen);
     else __narrow_sizchg(xsp, ndp->szu.xclen);
    }
   break;
  case STN_UNSIGNED:
   /* this must eval its argument and then return its value */
   /* word32 is just marking expr - no bit pattern change */
   fax = ndp->ru.x->lu.x;
   xsp = __eval_xpr(fax);

   /* 05/26/04 - may need size change here to mach fcall node size */
   /* but know result always word32 */ 
   if (xsp->xslen != ndp->szu.xclen) __sizchgxs(xsp, ndp->szu.xclen);
   
   /* because function return value has exactly same width as arg - never */
   /* need conversion */
   /* DBG remove -- */
   if (fax->szu.xclen != xsp->xslen) __misc_terr(__FILE__, __LINE__);
   /* --- */
   break;
  case STN_RANDOM:
   __exec_sfrand(ndp);
   break; 
  case STN_COUNT_DRIVERS:
   exec_count_drivers(ndp);
   break;
  case STN_DIST_UNIFORM:
   __exec_dist_uniform(ndp);
   break;
  case STN_DIST_EXPONENTIAL:
   __exec_dist_exp(ndp);
   break;
  case STN_DIST_NORMAL:
   __exec_dist_stdnorm(ndp);
   break;
  case STN_DIST_CHI_SQUARE:
   __exec_chi_square(ndp);
   break;
  case STN_DIST_POISSON:
   __exec_dist_poisson(ndp);
   break;
  case STN_DIST_T:
   __exec_dist_t(ndp);
   break;

  case STN_DIST_ERLANG:
   /* not yet implemented make 32 bit x */
   __sgfwarn(550, "system function %s not implemented - returning 32'bx",
    fsyp->synam);
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = ALL1W;
   xsp->bp[0] = ALL1W;
   break;
  case STN_Q_FULL:
   exec_qfull(ndp);  
   break;
  case STN_SCALE:
   fax = ndp->ru.x->lu.x;
   /* DBG remove */
   if (fax->optyp != GLBREF) __arg_terr(__FILE__, __LINE__);
   /* --- */
   /* this puts scale value on top of stack */
   __exec_scale(fax);
   break;
  case STN_TESTPLUSARGS:
   exec_testplusargs(ndp);      
   break;
  case STN_SCANPLUSARGS:
   exec_scanplusargs(ndp);      
   break;
  case STN_VALUEPLUSARGS:
   exec_valueplusargs(ndp);
   break;
  case STN_RESET_COUNT:
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = (word32) __reset_count;
   xsp->bp[0] = 0L;
   break;
  case STN_RESET_VALUE:
   push_xstk_(xsp, WBITS);
   /* value may be int32 (signed) - caller will interpret */
   xsp->ap[0] = (word32) __reset_value;
   xsp->bp[0] = 0L;
   break;
  case STN_GETPATTERN:
   /* should never see get pattern here */
   __arg_terr(__FILE__, __LINE__);
   break;
  case STN_COS: case STN_SIN: case STN_TAN:
  case STN_ACOS: case STN_ASIN: case STN_ATAN:
  case STN_COSH: case STN_SINH: case STN_TANH:
  case STN_ACOSH: case STN_ASINH: case STN_ATANH:
  case STN_LN: case STN_LOG10: case STN_ABS: case STN_SQRT: case STN_EXP:
  case STN_HSQRT: case STN_HLOG: case STN_HLOG10: case STN_HDB: 
   fax = ndp->ru.x->lu.x;
   exec_1arg_transcendental(sfbp->syfnum, fax);
   break;
  case STN_INT:
   exec_transcendental_int(ndp);
   break;
  case STN_SGN:
   exec_transcendental_sign(ndp);
   break;
  case STN_POW: case STN_HPOW: case STN_HPWR: case STN_HSIGN:
   exec_transcendental_powsign(sfbp->syfnum, ndp);
   break;
  case STN_MIN: case STN_MAX:
   exec_transcendental_minmax(sfbp->syfnum, ndp);
   break;
  case STN_ATAN2:
   exec_transcendental_atan2(ndp);
   break;
  case STN_HYPOT:
   exec_transcendental_hypot(ndp);
   break;
  default:
   /* DBG remove --- */
   if (sfbp->syfnum < BASE_VERIUSERTFS || (int32) sfbp->syfnum > __last_systf)
    __case_terr(__FILE__, __LINE__);
   /* --- */
   /* call pli system function calltf here - leave ret. value on stk */ 
   if (sfbp->syfnum <= __last_veriusertf) __pli_func_calltf(ndp);
   /* vpi_ systf after veriusertfs up to last systf */
   else __vpi_sysf_calltf(ndp);
 } 
}

/*
 * execute the count driver system function
 * notice these leave return (count) on expr. stack
 * change so count forced when force implemented
 */
static void exec_count_drivers(struct expr_t *ndp)
{
 register struct net_pin_t *npp;
 register struct expr_t *axp;
 register int32 i;
 int32 ri1, ri2, nd_itpop, biti, indi, is_forced, numdrvs, drvcnt[4];
 word32 val;
 byte *sbp;
 struct net_t *np;
 struct xstk_t *xsp;
 struct expr_t *idndp;
 struct gref_t *grp;

 nd_itpop = FALSE;
 for (i = 0; i < 4; i++) drvcnt[i] = 0; 
 is_forced = 0;
 numdrvs = 0;
 biti = -1;
 /* first get 1st argument value */
 axp = ndp->ru.x->lu.x; 
 /* know if bit select will be non x or earlier error to stop execution */
 if (axp->optyp == LSB) 
  {
   /* must eval. index expr. in ref. not target in case xmr */ 
   xsp = __eval_xpr(axp->ru.x);
   biti = xsp->ap[0];
   __pop_xstk(); 
   idndp = axp->lu.x;   
  }
 else idndp = axp;

 if (idndp->optyp == GLBREF)
  {
   grp = idndp->ru.grp;
   __xmrpush_refgrp_to_targ(grp);
   nd_itpop = TRUE;
  }
 np = idndp->lu.sy->el.enp;

 if (biti == -1) indi = 0; else indi = biti;
 if (np->frc_assgn_allocated)
  {
   if (np->ntyp >= NONWIRE_ST)
    {
     if (np->nu2.qcval[2*__inum].qc_active ||
      np->nu2.qcval[2*__inum + 1].qc_active) is_forced = TRUE;
     }
   else
    {
     if (np->nu2.qcval[np->nwid*__inum + indi].qc_active)
      is_forced = TRUE;
    }
  }

 /* since just evaluating read only must evaluate all drivers in here */
 /* inout itp and inout mpp NULL which is needed */ 
 for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
  {
   if (npp->npproctyp == NP_PROC_FILT
    && npp->npaux->npu.filtitp != __inst_ptr) continue;

   /* need to handle -2 IS specific bit select */ 
   __get_bidnpp_sect(np, npp, &ri1, &ri2);
   if (npp->npaux == NULL || ri1 == -1 || biti == -1) goto got_match;
   if (biti > ri1 || biti < ri2) continue;

got_match:
   /* need to make sure driver is loaded - no concept of changing here */
   switch (npp->npntyp) {
    case NP_VPIPUTV:
     /* for added vpi driver - this inst. or bit may not be added */
     /* if not added (used), do not count */
     if (!__has_vpi_driver(np, npp)) continue; 
     goto load_driver;

    case NP_GATE: case NP_CONTA: case NP_MDPRT: case NP_PB_MDPRT:
    case NP_ICONN: case NP_TFRWARG:
load_driver:
     /* load driver leaves value on expr. stack */
     if (np->n_stren)
      { 
       if ((xsp = __ld_stwire_driver(npp)) == NULL) break;
       sbp = (byte *) xsp->ap;
       val = sbp[indi] & 3;
       __pop_xstk();
      }
     else
      {
       xsp = __ld_wire_driver(npp);
       if (biti == -1) val = (xsp->ap[0] & 1L) | ((xsp->bp[0] & 1L) << 1); 
       else val = (rhsbsel_(xsp->ap, biti)) | (rhsbsel_(xsp->bp, biti) << 1);
       __pop_xstk();
      }
     (drvcnt[val])++;
     break;
    case NP_PULL: break; 
    default: __case_terr(__FILE__, __LINE__);
   } 
   /* other drivers such as pull just ingored in determing drivers */
  }
 if (nd_itpop) __pop_itstk();

 /* finally do the storing */
 numdrvs = drvcnt[0] + drvcnt[1] + drvcnt[3];
 /* know at least one argument */
 push_xstk_(xsp, WBITS);
 xsp->ap[0] = 0;
 xsp->bp[0] = 0;
 if ((axp = ndp->ru.x->ru.x) == NULL) goto just_ret; 
 for (i = 1; axp != NULL; axp = axp->ru.x, i++)
  {
   if (axp->lu.x->optyp == OPEMPTY) continue;
   switch ((byte) i) {
    case 1: xsp->ap[0] = (word32) is_forced; break;
    case 2: xsp->ap[0] = (word32) numdrvs; break;
    /* next 3 are 0, 1, and x drivers */
    case 3: xsp->ap[0] = (word32) drvcnt[0]; break;
    case 4: xsp->ap[0] = (word32) drvcnt[1]; break;
    case 5: xsp->ap[0] = (word32) drvcnt[3]; break;
    default: __case_terr(__FILE__, __LINE__);
   }
   __exec2_proc_assign(axp->lu.x, xsp->ap, xsp->bp);
  }
just_ret:
 xsp->ap[0] = (numdrvs > 1) ? 1 : 0;
}


/*
 * execute the test plus args system function
 * argument does not include the plus
 */
static void exec_testplusargs(struct expr_t *ndp)
{
 int32 slen;
 register struct optlst_t *olp;
 register char *chp, *argchp;
 int32 rv;
 struct xstk_t *xsp;

 argchp = __get_eval_cstr(ndp->ru.x->lu.x, &slen);
 for (rv = 0, olp = __opt_hdr; olp != NULL; olp = olp->optlnxt)
  {
   /* ignore markers added for building vpi argc/argv */
   if (olp->is_bmark || olp->is_emark) continue;

   chp = olp->opt;
   if (*chp != '+') continue;
   if (strcmp(&(chp[1]), argchp) == 0) { rv = 1; break; }
  }
 push_xstk_(xsp, WBITS);
 xsp->bp[0] = 0L;
 xsp->ap[0] = (word32) rv;
 __my_free(argchp, slen + 1);
}

/*
 * execute the scan plus args added system function 
 * same function as mc_scan_plusargs but assigns to 2nd parameter
 * almost same code as mc scan plus args pli system task
 * argument does not include the '+'
 */
static void exec_scanplusargs(struct expr_t *ndp)
{
 register struct optlst_t *olp;
 register char *chp;
 int32 arglen, rv;
 struct expr_t *fax;
 struct xstk_t *xsp;
 char *plusarg;
 
 fax = ndp->ru.x;
 /* this is the passed argment prefix */
 plusarg = __get_eval_cstr(fax->lu.x, &arglen);

 /* all options expanded and saved so this is easy */
 for (rv = 0, olp = __opt_hdr; olp != NULL; olp = olp->optlnxt)
  {
   /* ignore markers added for building vpi argc/argv */
   if (olp->is_bmark || olp->is_emark) continue;

   chp = olp->opt;
   if (*chp != '+') continue;

   /* option length if the length of the command line plus option string */
   /* option must be at least as long as passed arg or cannot match */
   if (strlen(chp) < arglen) continue; 
   /* match prefix - arg. is same or narrow that plus command line option */
   if (strncmp(&(chp[1]), plusarg, arglen) == 0)
    {
     rv = 1;
     xsp = __cstr_to_vval(&(chp[arglen + 1]));
     /* move to next - assign to arg */
     fax = fax->ru.x;

     /* SJM 05/10/04 - think this can be signed */
     if (xsp->xslen != fax->lu.x->szu.xclen)
      {
       if (xsp->xslen < fax->lu.x->szu.xclen && fax->lu.x->has_sign)
        __sgn_xtnd_wrd(xsp, fax->lu.x->szu.xclen);
       else __sizchgxs(xsp, fax->lu.x->szu.xclen);
      }

     __exec2_proc_assign(fax->lu.x, xsp->ap, xsp->bp);
     __pop_xstk();
     break;
    }
  }
 push_xstk_(xsp, WBITS);
 xsp->bp[0] = 0L;
 xsp->ap[0] = (word32) rv;
 __my_free(plusarg, arglen + 1);
}

/*
 * execute the $value$plusargs system function 
 * takes a string with a single format and places the value into a single
 * variable passed as the second argument
 *
 * works by dividing string from the variable name and the format
 * so "TEST=%d", becomes two strings "TEST=" and "%d" 
 * when it is passed to scanf routine
 * 
 * returns TRUE on success fails returns FALSE
 * $value$plusargs("TEST=%d", var)
 * takes +TEST=3 off command line and places value (format %) into var
 */
static void exec_valueplusargs(struct expr_t *ndp)
{
 register struct optlst_t *olp;
 register char *chp;
 int32 arglen, rv, i, j, namlen, saverrno;
 struct expr_t *fax;
 struct xstk_t *xsp;
 char *plusarg;
 char format[RECLEN];
 
 fax = ndp->ru.x;
 /* this is the passed argment prefix */
 plusarg = __get_eval_cstr(fax->lu.x, &arglen);

 rv = FALSE;
 chp = plusarg;
 /* namlen is the name of the +variable minus the format */
 namlen = -1; 
 /* get the +variable without the format */
 for (i = 0; i < arglen; i++, chp++)
  {
   /* first '%' found probably the format */
   if (*chp == '%')
    {
     /* check for escaped '%%' */
     if (i+1 < arglen && *(chp+1) == '%')
      {
       i++; chp++;
      }
     else break;
    }
  }

 /* the one format has to have at least '%d' */
 if (i > (arglen-2))
  {
   __sgferr(1300, "$value$plusargs string '%s' doesn't contain a format",
    plusarg); 
   goto done;
  }

  /* end of +variable name */
  namlen = i; 
  j = 0;
  /* the one format has to have at least '%d' */
  format[j++] = *chp++;
  /* add the number format */
  while (isdigit(*chp))  
   {
    format[j++] = *chp++;
   }
  /* check to make sure the format is valid */
  switch (*chp) {
   case 'd':
   case 'o':
   case 'h':
   case 'b':
   case 'e':
   case 'f':
   case 'g':
   case 's':
    format[j++] = *chp;
    /* the fomat has to be the end of the string */
    if (i+j == arglen)
     {
      format[j] = '\0';
      break;
     }
    /* FALLTHRU */
   default:
    __sgferr(1301, "$value$plusargs string '%s' contains illegal format",
     plusarg); 
    goto done; 
    break;
  }

 /* this part just the same as $scan$plusargs */
 for (olp = __opt_hdr; olp != NULL; olp = olp->optlnxt)
  {
   /* ignore markers added for building vpi argc/argv */
   if (olp->is_bmark || olp->is_emark) continue;

   chp = olp->opt;
   if (*chp != '+') continue;

   /* option length if the length of the command line plus option string */
   /* option must be at least as long as passed arg or cannot match */
   if (strlen(chp) < namlen) continue; 
   /* match prefix - arg. is same or narrow that plus command line option */
   if (strncmp(&(chp[1]), plusarg, namlen) == 0)
    {
     /* set the scanf format string */
     /* SJM 09/28/06 - can't pass local var pointed to by global */ 
     __fiofp = __pv_stralloc(format); 
     /* pass the string value */
     __fiolp = &(chp[namlen+1]); 
     /* move to next - assign to arg */
     fax = fax->ru.x;
     /* save and restore errno since it isn't an IO operation */
     saverrno = errno;
     /* get the format */
     rv = fio_exec_scanf(NULL, fax);
     __my_free(__fiofp, strlen(__fiofp) + 1);
     errno = saverrno;
     if (rv == -1) rv = FALSE;
     else rv = TRUE;
     break;
    }
  }
done:
 push_xstk_(xsp, WBITS);
 xsp->bp[0] = 0L;
 xsp->ap[0] = (word32) rv;
 __my_free(plusarg, arglen + 1);
}

/*
 * execute 1 real in returns real transcendental
 * places computed real on to expr stack
 */
static void exec_1arg_transcendental(int32 syfnum, struct expr_t *fax)
{
 double d1, d2;
 struct xstk_t *xsp;

 /* this pushes avaluated expressions onto stack - always real and replaced */
 xsp = __eval_xpr(fax);
 if (!fax->is_real) d1 = __cnvt_stk_to_real(xsp, (fax->has_sign == 1));
 else memcpy(&d1, xsp->ap, sizeof(double));

 /* DBG - LOOKATME - why here 
 if (finite(d1) == 0) __arg_terr(__FILE__, __LINE__);
 -- */
 switch (syfnum) {
  case STN_COS: d2 = cos(d1); break;
  case STN_SIN: d2 = sin(d1); break;
  case STN_TAN: d2 = tan(d1); break;
  case STN_ACOS:
   if (d1 < -1.0 || d1 > 1.0)
    {
     __sgfwarn(631,
      "$acos system function argument %s outside -1 to 1 legal range - returning 0.0", 
       __msgexpr_tostr(__xs, fax));
     d2 = 0.0;
    }
   else d2 = acos(d1);
   break;
  case STN_ACOSH:
   if (d1 < 1.0)
    {
     __sgfwarn(631,
      "$acosh system function argument %s outside 1 to inf legal range - returning 0.0", 
       __msgexpr_tostr(__xs, fax));
     d2 = 0.0;
    }
   else d2 = acosh(d1);
   break;
  case STN_ASIN:
   if (d1 < -1.0 || d1 > 1.0)
    {
     __sgfwarn(631,
      "$asin system function argument %s outside -1 to 1 legal range - returning 0.0", 
       __msgexpr_tostr(__xs, fax));
     d2 = 0.0;
    }
   else d2 = asin(d1);
   break;
  case STN_ASINH: d2 = asinh(d1); break;
  case STN_ATAN: d2 = atan(d1); break;
  case STN_COSH: d2 = cosh(d1); break;
  case STN_SINH: d2 = sinh(d1); break;
  case STN_TANH: d2 = tanh(d1); break;
  case STN_ATANH:
   if (d1 < -1.0 || d1 > 1.0)
    {
     __sgfwarn(631,
      "$atanh system function argument %s outside -1 to 1 legal range - returning 0.0", 
       __msgexpr_tostr(__xs, fax));
     d2 = 0.0;
    }
   else d2 = atanh(d1);
   break;
  case STN_LN:
   if (d1 <= 0.0)
    {
     __sgfwarn(631,
      "$ln system function argument %s illegal non positive - returning 0.0", 
       __msgexpr_tostr(__xs, fax));
     d2 = 0.0;
    }
   else d2 = log(d1);
   break;
  case STN_LOG10:
   if (d1 <= 0.0)
    {
     __sgfwarn(631,
      "$log10 system function argument %s illegal non positive - returning 0.0", 
       __msgexpr_tostr(__xs, fax));
     d2 = 0.0;
    }
   else d2 = log10(d1);
   break;
  case STN_ABS: d2 = fabs(d1); break;
  case STN_SQRT:
   if (d1 < 0.0)
    {
     __sgfwarn(631,
      "$sqrt system function argument %s illegal negative - returning 0.0", 
       __msgexpr_tostr(__xs, fax));
     d2 = 0.0;
    }
   else d2 = sqrt(d1);
   break;
  case STN_EXP: d2 = exp(d1); break;
  case STN_HSQRT:
   if (d1 >= 0.0) d2 = sqrt(d1); else d2 = -sqrt(-d1);
   break;
  case STN_HLOG:
   if (d1 > 0.0) d2 = log(d1);
   else if (d1 == 0.0) d2 = 0.0;
   else d2 = -log(-d1);
   break;
  case STN_HLOG10:
   if (d1 > 0.0) d2 = log10(d1);
   else if (d1 == 0.0) d2 = 0.0;
   else d2 = -log10(-d1);
   break;
  case STN_HDB: 
   if (d1 > 0.0) d2 = log(d1);
   else if (d1 == 0.0) d2 = 0.0;
   else d2 = -20.0*log(-d1);
   break;
  default: d2 = 0.0; __case_terr(__FILE__, __LINE__);
 }
 memcpy(xsp->ap, &d2, sizeof(double));
 xsp->bp = &(xsp->ap[1]);
 xsp->xslen = WBITS;
}

/*
 * execute transcendental int32 (convert to int32) routine
 */
static void exec_transcendental_int(struct expr_t *ndp)
{
 int32 ival;
 double d1;
 struct expr_t *fax;
 struct xstk_t *xsp;

 /* this returns 32 bit signed reg aka integer not real */
 fax = ndp->ru.x->lu.x;
 xsp = __eval_xpr(fax);
 if (!fax->is_real)
  {
   if (xsp->xslen > WBITS) __narrow_to1wrd(xsp);
   else
    {
     /* SJM 05/10/04 - old style convert to int32 - needs sign extension */
     if (xsp->xslen < WBITS) __sgn_xtnd_wrd(xsp, fax->szu.xclen);
    }

   if (xsp->bp[0] != 0) { xsp->ap[0] = ALL1W; xsp->bp[0] = ALL1W; }
   else 
    {
     /* LOOKATME - does this do anything? */ 
     ival = (int32) xsp->ap[0];
     xsp->ap[0] = (word32) ival;
    }
  }
 else
  {
   memcpy(&d1, xsp->ap, sizeof(double));
   ival = (int32) d1;
   xsp->bp[0] = 0L;
   xsp->ap[0] = (word32) ival;
  }
 /* reuse xstk that know is now WBITS */ 
}

/*
 * execute transcendental sign routine
 */
static void exec_transcendental_sign(struct expr_t *ndp)
{
 int32 ival;
 double d1;
 struct expr_t *fax;
 struct xstk_t *xsp;

 /* this returns 32 bit signed reg aka integer not real */
 fax = ndp->ru.x->lu.x;

 xsp = __eval_xpr(fax);
 if (!fax->is_real) d1 = __cnvt_stk_to_real(xsp, (fax->has_sign == 1));
 else memcpy(&d1, xsp->ap, sizeof(double));
 if (d1 < 0) ival = -1; else if (d1 > 0) ival = 1; else ival = 0;
 /* reuse xstk that know is WBITS */ 
 xsp->bp[0] = 0L;
 xsp->ap[0] = (word32) ival;
}

/*
 * exec transcendental pow - takes 2 args
 * also hspice sign with 2 args here 
 */
static void exec_transcendental_powsign(int32 sysfnum, struct expr_t *ndp)
{
 int32 ival;
 double d1, d2, d3;
 struct expr_t *fax, *fax2;
 struct xstk_t *xsp;

 ndp = ndp->ru.x;
 fax = ndp->lu.x;
 xsp = __eval_xpr(fax);
 if (!fax->is_real) d1 = __cnvt_stk_to_real(xsp, (fax->has_sign == 1));
 else memcpy(&d1, xsp->ap, sizeof(double));
 __pop_xstk();

 ndp = ndp->ru.x;
 fax2 = ndp->lu.x;
 xsp = __eval_xpr(fax2);
 if (!fax2->is_real) d2 = __cnvt_stk_to_real(xsp, (fax2->has_sign == 1));
 else memcpy(&d2, xsp->ap, sizeof(double));

 d3 = 0.0;
 if (sysfnum == STN_POW)
  {
   if (d1 < 0.0)
    {
     double d4;

     /* notice this uses hspice not Verilog conversion to int32 - matters not */
     ival = (int32) d2;
     d4 = ival;
     /* != real */
     if ((d4 - d2) <= -EPSILON && (d4 - d2) >= EPSILON)
      {
       __sgfwarn(631,
        "$pow system function argument first argument %s negative and second argument %s non integral - returning 0.0", 
       __msgexpr_tostr(__xs, fax), __msgexpr_tostr(__xs2, fax2));
       d2 = 0.0;
      }
     else d3 = pow(d1, d2);
    }
   else d3 = pow(d1, d2);
  }
 else if (sysfnum == STN_HPOW)
  {
   /* LOOKATME - notice this uses Hspice not Verilog conversion to int32 */
   ival = (int32) d2;
   d2 = ival;
   d3 = pow(d1, d2);
  }
 else if (sysfnum == STN_HPWR)
  {
   if (d1 > 0.0) d3 = pow(d1, d2);
   else if (d1 == 0.0) d3 = 0.0;
   else d3 = -pow(-d1, d2);
  }
 else if (sysfnum == STN_HSIGN)
  {
   /* notice $hsign returns double but $sign returns int32 */
   if (d2 > 0.0) d3 = fabs(d1);
   else if (d2 == 0.0) d3 = 0.0;
   else d3 = -fabs(d1);
  }
 else { d3 = 0.0; __case_terr(__FILE__, __LINE__); }

 memcpy(xsp->ap, &d3, sizeof(double));
 xsp->bp = &(xsp->ap[1]);
 xsp->xslen = WBITS;
}

/*
 * exec transcendental min/max - takes 2 args
 * LOOKATME - since can do with arg macro maybe unneeded
 */
static void exec_transcendental_minmax(int32 syfnum, struct expr_t *ndp)
{
 double d1, d2, d3;
 struct expr_t *fax;
 struct xstk_t *xsp;

 ndp = ndp->ru.x;
 fax = ndp->lu.x;
 xsp = __eval_xpr(fax);
 if (!fax->is_real) d1 = __cnvt_stk_to_real(xsp, (fax->has_sign == 1));
 else memcpy(&d1, xsp->ap, sizeof(double));
 __pop_xstk();

 ndp = ndp->ru.x;
 fax = ndp->lu.x;
 xsp = __eval_xpr(fax);
 if (!fax->is_real) d2 = __cnvt_stk_to_real(xsp, (fax->has_sign == 1));
 else memcpy(&d2, xsp->ap, sizeof(double));

 if (syfnum == STN_MIN) d3 = (d1 < d2) ? d1 : d2;
 else d3 = (d1 > d2) ? d1 : d2;

 memcpy(xsp->ap, &d3, sizeof(double));

 xsp->bp = &(xsp->ap[1]);
 xsp->xslen = WBITS;
}

/*
 * exec transcendental atan2 - takes 2 args
 */
static void exec_transcendental_atan2(struct expr_t *ndp)
{
 double d1, d2, d3;
 struct expr_t *fax;
 struct xstk_t *xsp;

 ndp = ndp->ru.x;
 fax = ndp->lu.x;
 xsp = __eval_xpr(fax);
 if (!fax->is_real) d1 = __cnvt_stk_to_real(xsp, (fax->has_sign == 1));
 else memcpy(&d1, xsp->ap, sizeof(double));
 __pop_xstk();

 ndp = ndp->ru.x;
 fax = ndp->lu.x;
 xsp = __eval_xpr(fax);
 if (!fax->is_real) d2 = __cnvt_stk_to_real(xsp, (fax->has_sign == 1));
 else memcpy(&d2, xsp->ap, sizeof(double));

 d3 = atan2(d1, d2);
 memcpy(xsp->ap, &d3, sizeof(double));

 xsp->bp = &(xsp->ap[1]);
 xsp->xslen = WBITS;
}

/*
 * exec transcendental hypot (dist func) - takes 2 args
 */
static void exec_transcendental_hypot(struct expr_t *ndp)
{
 double d1, d2, d3;
 struct expr_t *fax;
 struct xstk_t *xsp;

 ndp = ndp->ru.x;
 fax = ndp->lu.x;
 xsp = __eval_xpr(fax);
 if (!fax->is_real) d1 = __cnvt_stk_to_real(xsp, (fax->has_sign == 1));
 else memcpy(&d1, xsp->ap, sizeof(double));
 __pop_xstk();

 ndp = ndp->ru.x;
 fax = ndp->lu.x;
 xsp = __eval_xpr(fax);
 if (!fax->is_real) d2 = __cnvt_stk_to_real(xsp, (fax->has_sign == 1));
 else memcpy(&d2, xsp->ap, sizeof(double));

 d3 = hypot(d1, d2);
 memcpy(xsp->ap, &d3, sizeof(double));

 xsp->bp = &(xsp->ap[1]);
 xsp->xslen = WBITS;
}


/*
 * ROUTINES TO EXEC CAUSE
 */

/*
 * execute a cause statement
 * this is simply an assign to the event variable - gets added to net chg
 * as normal var change and during prep has normal dce npp's built
 */
static void exec_cause(struct st_t *stp)
{
 int32 nd_itpop;
 struct expr_t *xp;
 struct net_t *np;
 struct gref_t *grp;

 if (__st_tracing)
  __tr_msg("trace: %-7d -> %s\n", __slin_cnt,
   __to_idnam(stp->st.scausx));
 xp = stp->st.scausx;
 nd_itpop = FALSE;
 if (xp->optyp == GLBREF)
  {
   /* idea for xmr cause is to cause an event in some other part of the */
   /* itree - by changine current itree place will match waits only in */
   /* target of cause instance */ 
   grp = xp->ru.grp;    
   __xmrpush_refgrp_to_targ(grp);
   nd_itpop = TRUE;
  }
 else if (xp->optyp != ID) __case_terr(__FILE__, __LINE__);

 /* notice even if global ref. can still get net from symbol */ 
 np = xp->lu.sy->el.enp;
 /* notice cause does nothing - just schedules trigger for each waiting */
 /* armed ectrl - so any waiting event control blocks will be activated */

 /* record cause event var change as usual, if no pending do not record */
 /* must not record or will be worse event ordering dependency */
 /* know change see if need to record - also maybe dmpvars */
 record_nchg_(np);

 if (nd_itpop) __pop_itstk();
}

/*
 * DISABLE ROUTINES
 */

/*
 * ROUTINES TO IMPLEMENT TASK AND THREAD DISABLING
 */

/*
 * execute a disable statement - disable tskp
 * return T to cause disable of current thread (above on chain)
 * and F if this thread continues as usual
 * 
 * this code is not for functions there all disables converted to added gotos
 * disable argument is task name not ?? 
 */
extern int32 __exec_disable(struct expr_t *dsxndp)
{
 register struct tskthrd_t *ttp, *ttp_real_r;
 int32 thread_finished, nd_itpop; 
 struct sy_t *syp;
 struct task_t *tskp; 
 struct thread_t *dsathp, *thp, *thd_1up, *sav_thd;
 struct gref_t *grp;
 struct st_t *stp;

 /* this pointer to target symbol in some other module */
 nd_itpop = FALSE;
 syp = dsxndp->lu.sy;  
 if (dsxndp->optyp == GLBREF)
  { grp = dsxndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }

 /* disabling every thread associated with task of given instance required */
 /* first disable and free all underneath */ 
 tskp = syp->el.etskp;
 /* assume current thread not disabled */
 thread_finished = FALSE;
 /* task can be enabled from >1 place in inst. but share vars */
 ttp = tskp->tthrds[__inum];

 if (ttp == NULL)
  { 
   __sgfinform(469, "disable of %s %s no effect - not active",
    __to_tsktyp(__xs2, tskp->tsktyp), __msg_blditree(__xs, __inst_ptr, tskp));
   goto done;
  }
 for (; ttp != NULL;)
  {
   /* the task thread list that ttp points to will probably be freed */
   /* changing the right ptr so must get the next before freeing */
   ttp_real_r = ttp->tthd_r;
   dsathp = ttp->tthrd;

   /* better point back to self */
   /* -- DBG remove */
   if (dsathp->assoc_tsk != NULL && dsathp->assoc_tsk != tskp)
    __misc_sgfterr(__FILE__, __LINE__);
   /* --- */

   /* first if recurive task enables, disable top most */
   if ((thp = find_hgh_sametskthrd(dsathp)) != NULL) dsathp = thp;

   /* case 1: disabling current thread */ 
   if (dsathp == __cur_thd)
    {
     /* cannot unlink here since will be unlinked because thread done */
     __cur_thd->th_dsable = TRUE;
     thread_finished = TRUE;
    }
   /* case 2: disabling highest thread above currently active */
   /* this is case where above spawns subthread for task/blk/fj */
   /* case 1: and 2: mutually exclusive */
   else if (thread_above_cur(dsathp))
    {
     /* free under threads including current */
     free_thd_subtree(dsathp);
     /* ok to just set current thread since finished and all under gone */
     __cur_thd = dsathp;
     __cur_thd->th_dsable = TRUE;
     thread_finished = TRUE;
    }
   /* disabling thread with scheduled event elsewhere in thread tree */
   else
    {
     thd_1up = dsathp->thpar;
     /* anything (task-named block than can be disabled here parent */
     /* DBG remove --- */
     if (thd_1up == NULL) __misc_terr(__FILE__, __LINE__);
     if (thd_1up->thofscnt > 1 && !dsathp->th_fj)
      __misc_terr(__FILE__, __LINE__);  
     /* --- */
     /* disable a thread means remove and unlink from parent */
     thd_1up->thofscnt -= 1;
     if (dsathp->thleft != NULL) dsathp->thleft->thright = dsathp->thright;
     /* if first thread finished, make up thofs list point to its right */
     else thd_1up->thofs = dsathp->thright;
     if (dsathp->thright != NULL) dsathp->thright->thleft = dsathp->thleft;

     /* free all under and thread itself - mark schd. events canceled */ 
     free_thd_subtree(dsathp);
     __free_1thd(dsathp);
     dsathp = NULL;
     /* if more subthreads, nothing to do since will eval thread events */
     /* for other fj subthreads */
     if (thd_1up->thofscnt == 0)
      {
       /* know have parent or cannot get here */
       /* previously disable thread scheduled, after remove schedule 1 up */
       /* eliminating any waiting delay/event controls */
       sav_thd = __cur_thd;
       __cur_thd = thd_1up;
       /* if this was task with output parameters - do not store and */
       /* make sure up thread not in task returning state */
       if (thd_1up->th_postamble)
        { stp = thd_1up->thnxtstp->stnxt; thd_1up->th_postamble = FALSE; }
       else stp = thd_1up->thnxtstp;
       /* this will incorrectly turn on stmt suspend */ 
       /* DBG remove --- */
       if (__stmt_suspend) __misc_terr(__FILE__, __LINE__);
       /* --- */ 
       suspend_curthd(stp);
       __stmt_suspend = FALSE;
       __cur_thd = sav_thd;
      }
    }
   /* move to next - saved since previous probably freed */
   ttp = ttp_real_r;
  }

done:
 /* DBG remove ---
 __dmp_tskthd(tskp, __inst_mod);
 --- */
 if (nd_itpop) __pop_itstk();
 return(thread_finished);
}

/*
 * find highest thread associated with same task above
 * return NULL if thread to disable not above this one 
 */
static struct thread_t *find_hgh_sametskthrd(struct thread_t *dsthp)
{
 register struct thread_t *thp;
 struct thread_t *highthd;

 for (highthd = NULL, thp = __cur_thd; thp != NULL; thp = thp->thpar)
  {
   if (thp->assoc_tsk != NULL && thp->assoc_tsk == dsthp->assoc_tsk)
    highthd = thp;
  }
 return(highthd);
}

/*
 * return T if thread is above current thread
 */
static int32 thread_above_cur(struct thread_t *dsthp)
{
 register struct thread_t *thp;

 for (thp = __cur_thd; thp != NULL; thp = thp->thpar)
  {
   if (thp == dsthp) return(TRUE);
  }
 return(FALSE);
}

/*
 * free a thread subtree below thp (but not including thp)
 */
static void free_thd_subtree(struct thread_t *thp)
{
 if (thp->thofs != NULL) __free_thd_list(thp->thofs);
 thp->thofscnt = 0;
 thp->thofs = NULL;
 thp->th_fj = FALSE;
}

/*
 * free a thread list (passed head that is probably thofs)
 */
extern void __free_thd_list(struct thread_t *thp)
{
 register struct thread_t *thp2, *thp3;

 for (thp2 = thp; thp2 != NULL;)
  {
   thp3 = thp2->thright;
   if (thp2->thofs != NULL) __free_thd_list(thp2->thofs);
   __free_1thd(thp2);
   thp2 = thp3;
  }
}

/*
 * free one thread
 * called after any subthreads freed
 */
extern void __free_1thd(struct thread_t *thp)
{
 /* every thread with an associated task - unlabeled fork-join will not */
 /* DBG remove --- */
 if (thp->th_itp == NULL || thp->th_ialw)
   __misc_terr(__FILE__, __LINE__);
 /* --- */
 __push_itstk(thp->th_itp);
 free_thd_stuff(thp);
 __pop_itstk();
 __my_free((char *) thp, sizeof(struct thread_t)); 
}

/*
 * routine used by disable to force finish (free all but thread itself)
 */
static void free_thd_stuff(struct thread_t *thp)
{
 /* every thread with an associated task - unlabeled fork-join will not */
 if (thp->tthlst != NULL) unlink_tskthd(thp);
 if (thp->thdtevi != -1) __tevtab[thp->thdtevi].te_cancel = TRUE;
 /* notice if free statements (from iact) this will be set to nil */
 /* since when freeing dctrl and wait freeing the dctp */
 /* events freed later */
 if (thp->th_dctp != NULL)
  {
   if (thp->th_dctp->dceschd_tevs != NULL)
    thp->th_dctp->dceschd_tevs[thp->th_itp->itinum] = -1;
   thp->th_dctp = NULL;
  }
 if (thp->th_rhsform) 
  {
   __my_free((char *) thp->th_rhswp, 2*WRDBYTES*thp->th_rhswlen);
   thp->th_rhswp = NULL;
  }  
}

/*
 * unlink and free one task thead - remove from tasks list
 *
 * every thread that has assoc task has pointer to 1 tskthrd_t element
 * that is its entry on task's thread list for given instance 
 */
static void unlink_tskthd(struct thread_t *thp)
{
 struct tskthrd_t *ttp;
 struct task_t *tskp;

 ttp = thp->tthlst;
 /* DBG remove --- */
 if (__debug_flg && __st_tracing)
  {
   /* unlink of disabled thread to with no assoc. task */
   /* should emit the itree loc. here */
   if (thp->assoc_tsk == NULL) __misc_sgfterr(__FILE__, __LINE__);
   __tr_msg("++ unlink task %s for instance %s number %d\n",
    thp->assoc_tsk->tsksyp->synam, __inst_ptr->itip->isym->synam, __inum);
  }
 /* --- */

 /* lifo recursive enable freeing case */
 if (ttp->tthd_l == NULL)
  {
   tskp = thp->assoc_tsk;
   tskp->tthrds[__inum] = ttp->tthd_r;
   if (ttp->tthd_r != NULL) ttp->tthd_r->tthd_l = NULL;
  }
 else
  {
   /* any other order */
   ttp->tthd_l->tthd_r = ttp->tthd_r;
   if (ttp->tthd_r != NULL) ttp->tthd_r->tthd_l = ttp->tthd_l;
  }
 /* ttp has already been linked out */
 __my_free((char *) ttp, sizeof(struct tskthrd_t));
 thp->tthlst = NULL;   
 /* DBG remove ---
 __dmp_tskthd(tskp, __inst_ptr->itip->imsym->el.emdp);
 --- */
}

/*
 * SYSTEM TASK/FUNCTION EXECUTION ROUTINES
 */

/* 
 * execute the system tasks
 *
 * for monitor and strobe effect is to set up later action
 */
extern struct st_t *__exec_stsk(struct st_t *stp, struct sy_t *tsyp,
 struct tskcall_t *tkcp)
{
 int32 base, stav, oslen, slen;
 word32 wval;
 struct systsk_t *stbp;
 struct strblst_t *strblp;
 struct expr_t *argvx;
 char *chp;

 stbp = tsyp->el.esytbp;
 switch (stbp->stsknum) {
  /* file manipulation - most functions */
  case STN_FCLOSE:
   /* AIV 09/08/03 - for P1364 2001 must handle both MCD and FILE closes */
   fio_do_fclose(tkcp->targs);
   break;
  case STN_FFLUSH:
   /* AIV 09/08/03 - for P1364 2001 new sys task - not in 1995 std */
   /* SJM 09/09/03 - there is POSIX flush all that also flushes PLI */
   /* and other(?) open files - PORTABILITY? - works on Linux */
   if (tkcp->targs == NULL) fflush(NULL);
   else fio_fflush(tkcp->targs->lu.x);
   break;
  /* display write to terminal */
  case STN_DISPLAY: base = BDEC; goto nonf_disp;
  case STN_DISPLAYB: base = BBIN; goto nonf_disp;
  case STN_DISPLAYH: base = BHEX; goto nonf_disp;
  case STN_DISPLAYO:
   base = BOCT;
nonf_disp:
   __do_disp(tkcp->targs, base);
   __cvsim_msg("\n");
   break;

  /* write to terminal with no ending nl */
  case STN_WRITE: base = BDEC; goto nonf_write;
  case STN_WRITEH: base = BHEX; goto nonf_write;
  case STN_WRITEB: base = BBIN; goto nonf_write;
  case STN_WRITEO: base = BOCT;
nonf_write:
   __do_disp(tkcp->targs, base);
   /* if tracing to stdout need the new line just to stdout */
   /* LOOKATME - could change to separate verilog.trace file ?? */
   /* NOTICE - this is not __tr_msg */
   if (__st_tracing && __tr_s == stdout) __cv_msg("\n");
   break;

  /* multi-channel descriptor display to file */
  case STN_FDISPLAY: base = BDEC; goto f_disp;
  case STN_FDISPLAYB: base = BBIN; goto f_disp;
  case STN_FDISPLAYH: base = BHEX; goto f_disp;
  case STN_FDISPLAYO:
   base = BOCT;
f_disp:
   __fio_do_disp(tkcp->targs, base, TRUE, tsyp->synam);
   break;

  /* multi-channel descriptor write to file */
  case STN_FWRITE: base = BDEC; goto f_write;
  case STN_FWRITEH: base = BHEX; goto f_write;
  case STN_FWRITEB: base = BBIN; goto f_write;
  case STN_FWRITEO: base = BOCT;
f_write:
   /* if tracing need the new line */
   __fio_do_disp(tkcp->targs, base, __st_tracing, tsyp->synam);
   break;
  case STN_SWRITE: base = BDEC; goto s_write;
  case STN_SWRITEH: base = BHEX; goto s_write;
  case STN_SWRITEB: base = BBIN; goto s_write;
  case STN_SWRITEO: base = BOCT;
   /* $swrite([output reg], ...) */
s_write:
   fio_swrite(tkcp->targs, base);
   break;
  case STN_SFORMAT: 
   /* SJM 05/14/04 - LOOKATME - LRM is unclear and suggests that $sformat */
   /* returns a val but it is called a system task in LRM */
   fio_sformat(tkcp->targs);
   break; 
  /* like display except write at end of current time */
  case STN_FSTROBE: case STN_FSTROBEH: case STN_FSTROBEB: case STN_FSTROBEO:
  case STN_STROBE: case STN_STROBEH: case STN_STROBEB: case STN_STROBEO:
   /* if same strobe statement repeated in one time slot - warn/inform */
   if (stp->strb_seen_now)
    { 
     /* if dup. of same inst. and statement, do not re-add */
     if (!chk_strobe_infloop(stp, tsyp)) break;
    }
   if (__strb_freelst != NULL) 
    {
     strblp = __strb_freelst;
     __strb_freelst = __strb_freelst->strbnxt;
    }
   else strblp = (struct strblst_t *) __my_malloc(sizeof(struct strblst_t));
   strblp->strbstp = stp;
   stp->strb_seen_now = TRUE;
   strblp->strb_itp = __inst_ptr;
   strblp->strbnxt = NULL;
   __iact_can_free = FALSE;
   if (__strobe_hdr == NULL)
    {
     __strobe_hdr = __strobe_end = strblp; 
     __slotend_action |= SE_STROBE;
    }
   else { __strobe_end->strbnxt = strblp; __strobe_end = strblp; }
   break;
  /* monitor control sys tasks */ 
  case STN_MONITOROFF: __monit_active = FALSE; break;
  case STN_MONITORON:
   __monit_active = TRUE;
   __iact_can_free = FALSE;
   /* when monitor turned on (even if on), trigger even if no changes */
   /* and update save dce values to current */
   __slotend_action |= SE_MONIT_CHG;
   break;
  case STN_FMONITOR: case STN_FMONITORH: case STN_FMONITORB:
  case STN_FMONITORO:
   __start_fmonitor(stp); 
   __iact_can_free = FALSE;
   /* DBG remove ---
   if (__debug_flg)
    __dmpmod_nplst(__inst_mod, TRUE); 
   -- */
   break;
  /* change monitor write to terminal system tasks */
  case STN_MONITOR: case STN_MONITORH: case STN_MONITORB: case STN_MONITORO:
   __start_monitor(stp);
   __iact_can_free = FALSE;
   break;
  /* time releated system tasks */
  case STN_PRINTTIMESCALE:
   exec_prttimscale(tkcp->targs);
   break;
  case STN_TIMEFORMAT:
   /* if no `timescale in design, $timeformat is a noop */
   if (__des_has_timescales) exec_timefmt(tkcp->targs);
   break;
  case STN_READMEMB:
   __exec_readmem(tkcp->targs, BBIN);
   break;
  case STN_READMEMH:
   __exec_readmem(tkcp->targs, BHEX);
   break;
  case STN_SREADMEMB:
   __exec_sreadmem(tkcp->targs, BBIN);
   break;
  case STN_SREADMEMH:
   __exec_sreadmem(tkcp->targs, BHEX);
   break;

  /* dump variables tasks */
  case STN_DUMPVARS:
   __exec_dumpvars(tkcp->targs);
   break;
  case STN_DUMPALL:
   if (__dv_state == DVST_NOTSETUP)
    {
     __sgferr(703, "$dumpall ignored because: $dumpvars not set up");
     break;
    }
   /* dumpall is independent of other dumpvars except past file size limit */
   /* but still must dump at end of time slot */
   if ((__slotend_action & SE_DUMPALL) != 0)
    __sgfinform(445,
     "$dumpall ignored beause: $dumpall already executed at this time");
   /* turn on need dumpall plus need some dump vars action */
   __slotend_action |= (SE_DUMPALL | SE_DUMPVARS);
   break;
  case STN_DUMPFILE: 
   /* SJM 10/26/00 - fix to match XL */
   /* can set dumpvars file name before time of dumpvars setup */
   /* but not after dumpvars started */
   if (__dv_seen)
    {
     __sgferr(1066,
      "$dumpfile name set at time after $dmmpvars started - name not changed");
     break;
    }
   /* if no argument just leaves default - cannot be called more than once */
   if (tkcp->targs == NULL) break;

   /* set the file to dump too */
   argvx = tkcp->targs->lu.x;
   chp = __get_eval_cstr(argvx, &slen);
   /* cannot open yet but must save */
   oslen = strlen(__dv_fnam);
   if (__dv_fnam != NULL) __my_free((char *) __dv_fnam, oslen + 1);
   __dv_fnam = chp;
   break;
  case STN_DUMPFLUSH:
   /* flush the file now since need to flush before adding this times */
   /* dumpvars */
   if (__dv_state == DVST_NOTSETUP)
    {
     __sgferr(703,
      "$dumpflush ignored because: dumping of variables not begun");
     break;
    }
   if (__dv_fd == -1) __arg_terr(__FILE__, __LINE__);
   /* flush when called - i.e. flush is before this time's dumping */
   /* this call OS functions that may set errno */
   __my_dv_flush();
   break;
  case STN_DUMPLIMIT:
   /* notice can call even if not set up and can change if not over limit */
   argvx = tkcp->targs->lu.x;
   if (!__get_eval_word(argvx, &wval) || ((wval & (1 << (WBITS - 1))) != 0))
    {
     __sgferr(1036, "$dumplimit value %s illegal positive integer - not set",
      __msgexpr_tostr(__xs, tkcp->targs));
     break;
    }
    
   /* if already over limit cannot change */
   if (__dv_state == DVST_OVERLIMIT)
    {
     __sgferr(1069,
      "$dumplimit not set to %d - dump file already over previous limit %d",
      (int32) wval, __dv_dumplimit_size);
     break;
    }
   /* else inform if already set */
   if (__dv_dumplimit_size != 0)
    {
     __sgfinform(449, "$dumplimit changed from %d to %d",
      __dv_dumplimit_size, (int32) wval);
    }
   __dv_dumplimit_size = (int32) wval; 
   break;
  case STN_DUMPON: 
   switch ((byte) __dv_state) {
    case DVST_NOTSETUP:
     __sgferr(703, "$dumpon ignored because: $dumpvars not set up");
     break;
    /* if over limit silently ignore */
    case DVST_OVERLIMIT: break;
    case DVST_DUMPING:
     if ((__slotend_action & SE_DUMPOFF) != 0)
      {
       __sgfinform(453, "$dumpon overrides $dumpoff executed at this time");
       __slotend_action &= ~SE_DUMPOFF;
      }
     __sgfinform(446, "$dumpon ignored because: dumping already on");
     break;
    case DVST_NOTDUMPING:
     if ((__slotend_action & SE_DUMPON) != 0)
      __sgfinform(445,
       "$dumpon ignored because: $dumpon already executed at this time");
     /* also indicate need some dumpvars action */
     __slotend_action |= (SE_DUMPON | SE_DUMPVARS);
     break;
    default: __case_terr(__FILE__, __LINE__);
   }
   break;
  case STN_DUMPOFF: 
   switch ((byte) __dv_state) {
    case DVST_NOTSETUP:
     __sgferr(703, "$dumpoff ignored because: $dumpvars not set up");
     break;
    case DVST_OVERLIMIT: break;
    case DVST_NOTDUMPING:
     if ((__slotend_action & SE_DUMPON) != 0)
      {
       __sgfinform(453, "$dumpoff overrides $dumpon executed at this time");
       __slotend_action &= ~SE_DUMPON;
      }
     __sgfinform(446, "$dumpoff ignored because: dumping already off");
     break;
    case DVST_DUMPING:
     if ((__slotend_action & SE_DUMPON) != 0)
      __sgfinform(445,
       "$dumpoff ignored because: $dumpoff already executed at this time");
     /* also indicate need some dumpvars action even if no changes */
     __slotend_action |= (SE_DUMPOFF | SE_DUMPVARS);
     break;
    default: __case_terr(__FILE__, __LINE__); 
   }
   break;
  case STN_INPUT:
   __exec_input_fnamchg(tkcp->targs);
   break;
  case STN_HISTORY:
   /* LRM strictly requires all element here */
   __exec_history_list(__hist_cur_listnum);
   break;
  case STN_NOKEY:
no_key:
   __sgfwarn(560, "%s no effect - obsolete key file not supported",
    tsyp->synam);
   /* DBGER ---
   __save_key_s = __key_s;
   __nokey_seen = TRUE;
   __key_s = NULL;
   break;
   -- */
  case STN_KEY:
   goto no_key;
  case STN_KEEPCMDS: 
   if (__history_on)
    __sgfinform(458, "%s ignored - command history saving already on", 
     tsyp->synam);
   __history_on = TRUE; 
   break;
  case STN_NOKEEPCMDS: 
   if (!__history_on)
    __sgfinform(458, "%s ignored - command history saving already off", 
     tsyp->synam);
   __history_on = FALSE; 
   break;
  case STN_NOLOG:
   if (__log_s != NULL) fflush(__log_s);
   __save_log_s = __log_s;   
   
   /* notice cannot close log file */
   /* SJM 03/26/00 - 2 in Verilog 2000 not used for log file lumped with */
   /*  stdout (bit 0 value 1) */
   /* ---
   if (__mulchan_tab[2].mc_s != NULL) 
    {
     __mulchan_tab[2].mc_s = NULL;
     chp = __mulchan_tab[2].mc_fnam;
     __my_free(chp, strlen(chp) + 1);
     __mulchan_tab[2].mc_fnam = NULL;
    }
   --- */
   __log_s = NULL;
   break;
  case STN_LOG:
   exec_log_fnamchg(tkcp->targs); 
   break;
  case STN_TRACEFILE:
   argvx = tkcp->targs->lu.x;
   exec_trace_fnamchg(argvx); 
   break;
  case STN_SCOPE:
   exec_expr_schg(tkcp->targs->lu.x);
   break;
  /* listing off because no interactive environment */
  case STN_LIST:
   /* list current scope - */
   if (tkcp->targs != NULL)
    {
     struct itree_t *sav_scope_ptr = __scope_ptr;
     struct task_t *sav_tskp = __scope_tskp;

     exec_expr_schg(tkcp->targs->lu.x);
     __do_scope_list();
     __scope_ptr = sav_scope_ptr; 
     __scope_tskp = sav_tskp;
    }
   else __do_scope_list();
   break;

  /* unimplemented - need complicated state write file mechanism */
  case STN_SAVE: case STN_INCSAVE: case STN_RESTART:
   goto un_impl;

  case STN_FINISH:
   if (__tfrec_hdr != NULL) __call_misctfs_finish();
   if (__have_vpi_actions) __vpi_endsim_trycall();
   stav = get_opt_starg(tkcp->targs, 1); 
   if (stav >= 1 || __verbose)
    {
     /* LOOKATME - why needed - if (!__quiet_msgs) __cv msg("\n"); */
     __cv_msg("Halted at location %s time %s from call to $finish.\n",   
      __bld_lineloc(__xs2, (word32) __sfnam_ind, __slin_cnt),
      __to_timstr(__xs, &__simtime));
    }
   if (stav >= 2 || __verbose) __emit_stsk_endmsg();
   /* notice must always print error counts if any */
   if (__pv_err_cnt != 0 || __pv_warn_cnt != 0 || __inform_cnt != 0)
    __cv_msg("  There were %d error(s), %d warning(s), and %d inform(s).\n",
     __pv_err_cnt, __pv_warn_cnt, __inform_cnt);
   __my_exit(0, TRUE);
  case STN_STOP:
   if (__no_iact)
    {
     __sgfwarn(560, "%s no effect - interactive environment disabled",
      stbp->stsknam);
     break;
    }
   if (__iact_state)
    {
     __sgfwarn(587, "%s no effect - enabled from interactive debugger",
      stbp->stsknam);
     break;
    }
   stav = get_opt_starg(tkcp->targs, 1); 
   if (stav >= 1)
    {
     if (!__quiet_msgs) __cv_msg("\n");
     __cv_msg(
      "$stop executed at time %s from source location %s.\n",
      __to_timstr(__xs, &__simtime), __bld_lineloc(__xs2,
      (word32) __sfnam_ind, __slin_cnt));
    }
   if (stav >= 2) __emit_stsk_endmsg();
   __pending_enter_iact = TRUE;
   __iact_reason = IAER_STOP;
   signal(SIGINT, SIG_IGN);
   __stmt_suspend = TRUE;
   suspend_curthd(stp->stnxt);
   return(NULL);
  case STN_SETTRACE:
   /* statement tracing change requires suspend of thread */
   __st_tracing = TRUE;
   /* if enabled from interactive state, suspend will cause core dump */
   if (!__iact_state) suspend_curthd(stp->stnxt);
   /* if trace file name set from command option open now if not open */  
   __maybe_open_trfile();
   return(NULL);
  case STN_CLEARTRACE: 
   __st_tracing = FALSE;
   break;
  case STN_SETEVTRACE:
   __ev_tracing = TRUE;
   /* if trace file name set from command option open now if not open */  
   __maybe_open_trfile();
   break;
  case STN_CLEAREVTRACE:
   /* notice leave file open */
   __ev_tracing = FALSE;
   break;
  case STN_SETDEBUG:
   __debug_flg = TRUE;
   break;
  case STN_CLEARDEBUG:
   __debug_flg = FALSE;
   break;
  case STN_SHOWVARS:
   do_showvars_stask(tkcp->targs);
   break;
  case STN_SHOWVARIABLES:
   /* for now same as showvars - i.e. skip 1st control arg. */
   do_showvars_stask(tkcp->targs->ru.x);
   break;
  case STN_SYSTEM:
   /* $system with no args, means run interactive shell */ 
   if (tkcp->targs == NULL) chp = __pv_stralloc(" ");
   else
    {
     argvx = tkcp->targs->lu.x;
     if (argvx->optyp == OPEMPTY) { chp = __pv_stralloc(" "); slen = 1; }
     else chp = __get_eval_cstr(argvx, &slen);
    }
   __escape_to_shell(chp);
   slen = strlen(chp);
   __my_free(chp, slen + 1);
   break;
  case STN_SUPWARNS:
   do_warn_supp_chg(stbp->stsknam, tkcp->targs, TRUE);
   break;   
  case STN_ALLOWWARNS:
   do_warn_supp_chg(stbp->stsknam, tkcp->targs, FALSE);
   break;
  case STN_MEMUSE:
   /* this will force output */
   __cvsim_msg("Approximately %ld bytes allocated dynamic storage.\n",
     __mem_use);
   __cvsim_msg("Verilog arrays (memories) require %ld bytes.\n", __arrvmem_use);
   break;
  case STN_FLUSHLOG:
   /* LOOKATME - maybe should check for rare but possible error */
   if (__log_s != NULL) fflush(__log_s);
   if (__tr_s != NULL) fflush(__tr_s);
   break;
  case STN_RESET:
   /* do reset processing - final step is long jmp to top level */
   do_reset(tkcp->targs);
   break;
  case STN_SNAPSHOT:
   stav = get_opt_starg(tkcp->targs, DFLT_SNAP_EVS); 
   __write_snapshot(stav);
   break;
  case STN_SHOWALLINSTANCES:
   __prt2_mod_typetab(FALSE);
   break;
  case STN_SHOWSCOPES:
   do_showscopes(tkcp->targs);
   break;
  /* graphical output tasks - ignore with warn */
  case STN_GRREMOTE:
  case STN_PSWAVES:
  case STN_GRSYNCHON:
  case STN_GRREGS:
  case STN_GRWAVES:
  case STN_FREEZEWAVES:
  case STN_DEFINEGROUPWAVES:
   /* earlier warning - just ignore */
   break;
   
  /* internal simulation state printng tasks */
  case STN_SHOWEXPANDEDNETS: goto un_impl;

  /* q manipulation tasks - also q_full function */
  case STN_Q_INITIALIZE:
   do_q_init(tkcp->targs);
   break;
  case STN_Q_ADD:
   do_q_add(tkcp->targs);
   break;
  case STN_Q_REMOVE:
   do_q_remove(tkcp->targs);
   break;
  case STN_Q_EXAM:
   do_q_examine(tkcp->targs);
   break;
  case STN_SDF_ANNOTATE:
   __exec_sdf_annotate_systsk(tkcp->targs);
   break;
  default:
   /* DBG remove --- */
   if (stbp->stsknum < BASE_VERIUSERTFS || (int32) stbp->stsknum > __last_systf)
     __case_terr(__FILE__, __LINE__);
   /* --- */
   /* exec (call) pli user tf system function here */ 
   if (stbp->stsknum <= __last_veriusertf) __pli_task_calltf(stp);
   else __vpi_syst_calltf(stp);
 }
 return(stp->stnxt);

un_impl:
 __sgfwarn(550, "system task %s not implemented - ignored", stbp->stsknam);
 return(stp->stnxt);
}

/*
 * MISCELLANEOUS SYSTEM TASK EXEC ROUTINES
 */

/*
 * check strobe statement repeated in same strobe system task call
 *
 * notice this only called if know strobe task enable statement repeated
 * checking for strobe repeated but in different instance 
 */
static int32 chk_strobe_infloop(struct st_t *stp, struct sy_t *tsksyp)
{
 register struct strblst_t *strbp;
 int32 match;
 
 match = FALSE;
 for (strbp = __strobe_hdr; strbp != NULL; strbp = strbp->strbnxt)
  {
   if (strbp->strbstp == stp)
    {
     match = TRUE; 
     if (strbp->strb_itp == __inst_ptr)
      {
       __sgfwarn(527, "%s enable for instance %s repeated at this time",
        tsksyp->synam, __msg2_blditree(__xs, __inst_ptr));
       return(FALSE);
      }
     /* maybe should only inform one inform per call ? */
     __sgfinform(434, "%s enable in instance %s repeated in %s at this time", 
      tsksyp->synam, __msg2_blditree(__xs, __inst_ptr),
      __msg2_blditree(__xs2, strbp->strb_itp));
    }
  }
 if (!match) __case_terr(__FILE__, __LINE__);
 return(TRUE);
}

/*
 * setup event from suspend (^c or $stop or stmt. break) of cur. thread
 * stp is place to begin after wake up
 * links new event on front of current event list
 */
static void suspend_curthd(struct st_t *stp)
{
 i_tev_ndx tevpi;

 alloc_tev_(tevpi, TE_THRD, __inst_ptr, __simtime);
 __cur_thd->thdtevi = tevpi;
 __tevtab[tevpi].tu.tethrd = __cur_thd;
 __cur_thd->thnxtstp = stp;
 __stmt_suspend = TRUE;
 __suspended_thd = __cur_thd;
 __suspended_itp = __inst_ptr;
 /* must save suspended, but popping done in event processing loop */
 __cur_thd = NULL;

 /* if hit break or step in func. save event, must unde (extr. and cancel) */
 /* to continue in function without going through event processing loop */
 if (__fcspi >= 0) __fsusp_tevpi = tevpi; else __fsusp_tevpi = -1;

 /* since just suspending and want to continue from here, put on front */
 __add_ev_to_front(tevpi);
 /* maybe remove this since - will print interactive msg anyway ? */
 if (__debug_flg && __st_tracing)
  {
   if (stp != NULL)
    sprintf(__xs2,  "at %s", __bld_lineloc(__xs, stp->stfnam_ind,
     stp->stlin_cnt));
   else strcpy(__xs2, "**past end");
   __tr_msg("-- suspend of current thread, was enabled at %s, continue %s\n",
    __bld_lineloc(__xs, __suspended_thd->thenbl_sfnam_ind,
      __suspended_thd->thenbl_slin_cnt), __xs2);
  }
}

/*
 * routine to open trace output file when needed
 */
extern void __maybe_open_trfile(void)
{
 if (strcmp(__tr_fnam, "stdout") == 0 || __tr_s != NULL) return;
 if ((__tr_s = __tilde_fopen(__tr_fnam, "w")) == NULL)
  {
   __sgferr(1247, "cannot open trace output file %s - stdout used",
    __tr_fnam);
   __tr_s = stdout;
   __my_free(__tr_fnam, strlen(__tr_fnam) + 1);
   __tr_fnam = __my_malloc(7);
   strcpy(__tr_fnam, "stdout");
  }
}

/*
 * execute the multi-channel descriptor file close 
 * this is a system task not function  
 *
 * SJM 08/09/03 - FIXME ??? - need to disable pending f monit or strobe 
 */
static void mcd_do_fclose(struct expr_t *axp)
{
 word32 mcd;
 struct xstk_t *xsp;

 xsp = __eval_xpr(axp->lu.x);
 if (xsp->bp[0] != 0L) 
  {
   __sgfwarn(611, 
    "$fclose multi-channel descriptor %s contains x or z bits - no action",
    __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE));
   __pop_xstk();
   return;
  }
 /* system task does not return anything but vpi_ call does */
 mcd = xsp->ap[0]; 
 __pop_xstk();
 __close_mcd((word32) mcd, FALSE);
}

/*
 * close mcd
 */
extern word32 __close_mcd(word32 mcd, int32 from_vpi)
{
 register int32 i;
 int32 err;

 err = FALSE;
 if (mcd == 0L)
  {
   if (!from_vpi)
    {
     __sgfinform(431,
      "$fclose passed empty (no bits on) multi-channel descriptor");
    }
   return(bld_open_mcd());
  }
 if ((mcd & 1L) != 0)  
  {
   if (!from_vpi)
    {
     __sgfinform(432,
      "$fclose bit 1 (stdout) multi-channel descriptor cannot be closed");
    }
   err = TRUE;
  }
 if ((mcd & 2L) != 0)  
  {
   if (!from_vpi)
    {
     __sgfinform(432,
      "$fclose bit 2 (stderr) multi-channel descriptor cannot be closed");
    }
   err = TRUE;
  }
 for (i = 2; i < 31; i++)
  {
   if (((mcd >> i) & 1L) == 0L) continue;

   if (__mulchan_tab[i].mc_s == NULL)
    {
     if (!from_vpi)
      {
       __sgfwarn(611,
        "$fclose multi-channel descriptor bit %d on, but file already closed",
        i + 1);  
      }
     err = TRUE;
     continue;
    }  
   __my_fclose(__mulchan_tab[i].mc_s);
   __mulchan_tab[i].mc_s = NULL;
   __my_free(__mulchan_tab[i].mc_fnam, strlen(__mulchan_tab[i].mc_fnam) + 1); 
   __mulchan_tab[i].mc_fnam = NULL;
  }
 if (!from_vpi)
  {
   if (((mcd >> 31) & 1L) != 0L)
    {
     __sgfwarn(611,
      "$fclose multi-channel descriptor bit 31 on, but no open file - unusable because reserved for new Verilog 2000 file I/O");
     err = TRUE;
    }
  }  

 if (err) return(bld_open_mcd());
 return(0);
}

/*
 * build a mc descriptor for open channels 
 */
static word32 bld_open_mcd(void)
{ 
 word32 mcd; 
 register int32 i;

 /* SJM 03/26/00 - high bit 32 reserved for new Verilog 2000 file I/O */
 for (i = 0, mcd = 0; i < 31; i++)
  {
   if (__mulchan_tab[i].mc_s == NULL) continue;
   mcd |= (1 << i);   
  }
 return(mcd);
}

/*
 * execute the multi-channel descriptor file open
 * assigns to next free descriptor slot if one available
 *
 * this is system function that returns 0 on fail 
 * 1 (index 0) is stdout and log file, 2 (index 1) is stder
 */
static word32 mc_do_fopen(struct expr_t *axp)
{
 int32 slen;
 char *chp;

 chp = __get_eval_cstr(axp, &slen);
 return(__mc1_fopen(chp, strlen(chp), FALSE));
}

/*
 * do the mcd fopen if possible
 */
extern word32 __mc1_fopen(char *chp, int32 slen, int32 from_vpi)
{
 register int32 i; 
 FILE *tmp_s;

 /* SJM 03/26/00 - changed to match Verilog 2000 LRM */
 /* if name matches exactly return open - this is only heuristic */
 /* notice 2 is bit 3 (or value 4) that is first to use */
 /* bit 31 is rserved for new c style file open enhancement */
 for (i = 2; i < 31; i++) 
  {
   if (__mulchan_tab[i].mc_s == NULL) continue; 

   /* LOOKATME - not storing in tilde expanded form - same name can */
   /* mismatch but will just get open error from OS */
   if (strcmp(__mulchan_tab[i].mc_fnam, chp) == 0)
    {
     if (!from_vpi)
      {
       __sgfinform(433,
        "$fopen of %s failed: file already open and assigned to channel %d",
          chp, i + 1);
        __my_free(chp, slen + 1);
      }
     return((word32) (1L << i));
    }
  }

 for (i = 2; i < 31; i++) 
  { if (__mulchan_tab[i].mc_s == NULL) goto found_free; }
 if (!from_vpi)
  {
   __sgfinform(433,
    "$fopen of %s failed: no available multi-channel descriptor", chp);
  }
err_done:
 __my_free(chp, slen + 1);
 return(0);

found_free:
 if ((tmp_s = __tilde_fopen(chp, "w")) == NULL)
  {
   if (!from_vpi) 
    {
     __sgfinform(433, "$fopen of %s multi-channel bit %d failed: %s",
      chp, i, strerror(errno));
    }
   goto err_done;
  }
 __mulchan_tab[i].mc_s = tmp_s;
 /* know this is closed so no previous name to free */
 __mulchan_tab[i].mc_fnam = chp;
 /* notice first unused is 3 which is bit 4 on (if low bit is 1) */
 return((word32) (1L << i));
}


/*
 * execute showvars system task
 * this is called with fcall comma arg. list header not value 
 */
static void do_showvars_stask(struct expr_t *argxp)
{
 register int32 ni;
 register struct expr_t *xp;
 register struct net_t *np;
 int32 nd_itpop;
 struct gref_t *grp;
 struct task_t *tskp;
 struct expr_t *ndp;

 if (__iact_state) tskp = __scope_tskp;
 else tskp = __getcur_scope_tsk();

 /* notice here, calling itree location correct */
 if (argxp == NULL)
  {
   /* if no arguments - all variables in current scope */
   __cvsim_msg(">>> $showvars all local variables - scope %s type %s.\n",
    __msg_blditree(__xs, __inst_ptr, tskp), __inst_ptr->itip->imsym->synam);
   if (tskp != NULL)
    {
     if (tskp->trnum != 0)
      {
       np = &(tskp->tsk_regs[0]);
       for (ni = 0; ni < tskp->trnum; ni++, np++) __emit_1showvar(np, NULL);
      }
    }
   else
    {
     if (__inst_mod->mnnum != 0)
      {
       for (ni = 0, np = &(__inst_mod->mnets[0]); ni < __inst_mod->mnnum;
        ni++, np++)
        __emit_1showvar(np, NULL);
      }
    }
  }
 else
  {
   /* go through list of variables - may be xmr's */ 
   /* these can be only var, bit select or part select */
   __cvsim_msg(
    ">>> $showvars list of variables form - current scope %s type %s.\n",
    __msg_blditree(__xs, __inst_ptr, tskp), __inst_ptr->itip->imsym->synam);
   for (xp = argxp; xp != NULL; xp = xp->ru.x)
    {
     nd_itpop = FALSE;
     grp = NULL;
     ndp = xp->lu.x;
     if (ndp->optyp == LSB || ndp->optyp == PARTSEL)
      {
       np = ndp->lu.x->lu.sy->el.enp;
       if (ndp->lu.x->optyp == GLBREF)
        {
         grp = ndp->lu.x->ru.grp;
         __xmrpush_refgrp_to_targ(grp);
         nd_itpop = TRUE;
        }
      }
     else
      {
       if (ndp->optyp == GLBREF)
        { grp = ndp->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
       np = ndp->lu.sy->el.enp;
      }
     __emit_1showvar(np, grp);
     if (nd_itpop) __pop_itstk();
    }
  }
}

/*
 * set new suppressed warnings during simulation
 */
static void do_warn_supp_chg(char *stnam, struct expr_t *argxp, int32 supp)
{
 int32 argi;
 word32 ernum;
 struct expr_t *ndp;

 for (argi = 1; argxp != NULL; argxp = argxp->ru.x, argi++)
  {
   ndp = argxp->lu.x;
   if (!__get_eval_word(ndp, &ernum))
    {
bad_num:
     __sgferr(714,
      "%s argument %d value %s illegal or outside of inform/warning number range",
      stnam, argi, __msgexpr_tostr(__xs, ndp));
     continue;
    }
   if (!__enum_is_suppressable(ernum)) goto bad_num;

   if (supp) __wsupptab[ernum/WBITS] |= (1 << (ernum % WBITS));
   else __wsupptab[ernum/WBITS] &= ~(1 << (ernum % WBITS));
  }
}

/*
 * execute reset system task
 */
static void do_reset(struct expr_t *axp)
{
 int32 enter_iact, reset_val, diag_val;

 /* assume interactive entry in case reset value (2nd) arg missing */
 enter_iact = TRUE;
 reset_val = 0;
 diag_val = 0; 
 if (axp == NULL) goto do_it;
 if (get_opt_starg(axp, 0) != 0) enter_iact = FALSE;
 if ((axp = axp->ru.x) == NULL) goto do_it; 
 reset_val = get_opt_starg(axp, 0);
 if ((axp = axp->ru.x) == NULL) goto do_it; 
 diag_val = get_opt_starg(axp, 0);

do_it:
 if (diag_val >= 1)
  {
   if (!__quiet_msgs) __cv_msg("\n");
   __cv_msg("$reset to time 0 called from location %s at time %s.\n",
    __bld_lineloc(__xs2, (word32) __sfnam_ind, __slin_cnt),
    __to_timstr(__xs, &__simtime));
  }
 if (diag_val >= 2) __emit_stsk_endmsg();
 /* enter interactive unless reset value given and non zero */
 if (reset_val != 0) enter_iact = FALSE;

 if (enter_iact) __stop_before_sim = TRUE;
 else __stop_before_sim = FALSE;
 /* record state changes caused by arguments */
 __reset_value = reset_val;

 /* reenable the normal ^c signal handler - when variables reset */
 /* sim will replace with sim handler for entering interactive */
#if defined(INTSIGS)
 signal(SIGINT, __comp_sigint_handler);
#else
 signal(SIGINT, (void (*)()) __comp_sigint_handler);
#endif

 /* this does not return - uses lng jmp */
 longjmp(__reset_jmpbuf, 1);
}

/*
 * write the scope information
 * uses current scope not interactive - except $scope also changes current 
 * if in interactive mode
 */
static void do_showscopes(struct expr_t *axp)
{
 word32 flag;
 struct task_t *tskp;
 struct mod_t *imdp;
 struct sy_t *syp;

 if (axp == NULL) flag = 0;
 else if (!__get_eval_word(axp->lu.x, &flag))
  {
   __sgfwarn(646, "$showscopes argument value %s has x/z bits - made 0",
    __msgexpr_tostr(__xs, axp->lu.x));
   flag = 0;
  }
 /* use current thread to determine if in task */
 /* if no thread (sim. not started) cannot be active task */
 if (__cur_thd == NULL) tskp = NULL; else tskp = __getcur_scope_tsk();

 /* 0 means current level, other value all underneath */
 if (flag == 0)
  {
   if (tskp == NULL) prt_1m_scopelist(__inst_ptr);
   else prt_1tsk_scopelist(tskp, TRUE);
  }
 else
  { 
   __cvsim_msg("Nested scopes:\n");
   __outlinpos = 0;
   __pv_stlevel = 0;

   if (tskp == NULL) prt_1m_nestscopes(__inst_ptr);
   /* if current scope is task, must print out named blocks inside */
   else prt_1tsk_nestscopes(tskp->tsksymtab->sytofs);

   __pv_stlevel = 0;
   __outlinpos = 0;
  }

 /* final step is printing current scope and list of top level modules */
 imdp = __inst_mod;
 
 if (tskp == NULL) syp = imdp->msym; else syp = tskp->tsksyp;
 __cvsim_msg("Current scope: %s (file %s line %d)\n",
  __msg_blditree(__xs, __inst_ptr, tskp), __schop(__xs2,
  __in_fils[syp->syfnam_ind]), syp->sylin_cnt); 
 __prt_top_mods();
}

/*
 * show one module scope level given itree location 
 *
 * notice this is sort of static since once at itree location under same
 * 4 catagories of scopes: instances, tasks, functions, named blocks
 * everything here but named blockes in tasks/functions or named blocks
 */
static void prt_1m_scopelist(struct itree_t *itp)
{
 register int32 i;
 register struct task_t *tskp;
 int32 none, first_time;
 struct mod_t *imdp;
 struct inst_t *ip;

 if (__outlinpos != 0) __misc_terr(__FILE__, __LINE__);
 /* first instances */
 __pv_stlevel = 0;
 imdp = itp->itip->imsym->el.emdp;
 if (imdp->minum != 0) __wrap_puts("  Instances:", stdout);
 __pv_stlevel = 3;
 for (i = 0; i < imdp->minum; i++)
  {
   ip = &(imdp->minsts[i]);
   __wrap_putc(' ', stdout);
   __wrap_puts(ip->isym->synam, stdout);
   __wrap_putc('(', stdout);
   __wrap_puts(ip->imsym->synam, stdout);
   if (i < imdp->minum - 1) __wrap_puts("),", stdout);
   else __wrap_putc(')', stdout);
  }
 if (__outlinpos != 0) { __wrap_putc('\n', stdout); __outlinpos = 0; }
 __pv_stlevel = 0;

 /* next tasks */
 for (none = TRUE, tskp = imdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
  { if (tskp->tsktyp == TASK) { none = FALSE; break; } } 
 if (!none) 
  {
   __wrap_puts("  Tasks:", stdout);
   __pv_stlevel = 3;
   first_time = TRUE;
   for (tskp = imdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     if (tskp->tsktyp != TASK) continue;
     if (first_time) { __wrap_putc(' ', stdout); first_time = FALSE; }
     else __wrap_puts(", ", stdout);
     __wrap_puts(tskp->tsksyp->synam, stdout);
    }
   if (__outlinpos != 0) { __wrap_putc('\n', stdout); __outlinpos = 0; }
   __pv_stlevel = 0;
  }
 /* next functons */
 for (none = TRUE, tskp = imdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
  { if (tskp->tsktyp == FUNCTION) { none = FALSE; break; } } 
 if (!none) 
  {
   __wrap_puts("  Functions:", stdout);
   __pv_stlevel = 3;
   first_time = TRUE;
   for (tskp = imdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     if (tskp->tsktyp != FUNCTION) continue;
     if (first_time) { __wrap_putc(' ', stdout); first_time = FALSE; }
     else __wrap_puts(", ", stdout);
     __wrap_puts(tskp->tsksyp->synam, stdout);
    }
   if (__outlinpos != 0) { __wrap_putc('\n', stdout); __outlinpos = 0; }
   __pv_stlevel = 0;
  }
 /* finally named blocks */
 for (none = TRUE, tskp = imdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
  {

   if (tskp->tsktyp == FORK || tskp->tsktyp == Begin)
    { none = FALSE; break; }
  } 
 if (!none) 
  {
   __wrap_puts("  Named blocked:", stdout);
   __pv_stlevel = 3;
   first_time = TRUE;
   for (tskp = imdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {

     if (tskp->tsktyp == TASK || tskp->tsktyp == FUNCTION) continue;
     if (first_time) { __wrap_putc(' ', stdout); first_time = FALSE; }
     else __wrap_puts(", ", stdout);
     __wrap_puts(tskp->tsksyp->synam, stdout);
    }
   if (__outlinpos != 0) { __wrap_putc('\n', stdout); __outlinpos = 0; }
   __pv_stlevel = 0;
  }
}

/*
 * print the scopes in a task scope
 * here can only be named blocks located from symbol table
 * separate routine for recursive named block listing
 */
static void prt_1tsk_scopelist(struct task_t *tskp, int32 nd_msg)
{
 register struct symtab_t *sytp2;
 struct symtab_t *sytp;
 int32 first_time;
 
 sytp = tskp->tsksymtab;
 if (sytp->sytofs == NULL && nd_msg) return;
 __wrap_puts("  Named blocks:", stdout);
 __pv_stlevel = 3;
 first_time = FALSE;
 for (sytp2 = sytp->sytofs; sytp2 != NULL; sytp2 = sytp2->sytsib)
  {
   if (first_time) { first_time = FALSE; __wrap_putc(' ', stdout); }
   else __wrap_puts(", ", stdout);
   __wrap_puts(sytp2->sypofsyt->synam, stdout);
  }
 if (__outlinpos != 0) { __wrap_putc('\n', stdout); __outlinpos = 0; }
 __pv_stlevel = 0;
 if (__outlinpos != 0) __misc_terr(__FILE__, __LINE__);
 /* first instances */
 __pv_stlevel = 0;
}

/*
 * for module print nested scopes with indent - to show the scope structure
 */
static void prt_1m_nestscopes(struct itree_t *itp)
{
 register int32 i;
 register struct task_t *tskp;
 struct mod_t *mdp;
 struct itree_t *down_itp;
 struct inst_t *ip;

 __pv_stlevel++;
 mdp = itp->itip->imsym->el.emdp;
 for (i = 0; i < mdp->minum; i++)
  {
   down_itp = &(itp->in_its[i]);
   ip = down_itp->itip;
   __wrap_putc(' ', stdout);
   __wrap_puts(ip->isym->synam, stdout);
   __wrap_putc('(', stdout);
   __wrap_puts(ip->imsym->synam, stdout);
   __wrap_putc(')', stdout);
   if (__outlinpos != 0) { __wrap_putc('\n', stdout); __outlinpos = 0; }
   prt_1m_nestscopes(down_itp);
  }
 /* next tasks */
 __pv_stlevel++;
 for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
  {

   sprintf(__xs, "%s: ", __to_tsktyp(__xs2, tskp->tsktyp));
   __wrap_puts(__xs, stdout);
   __wrap_puts(tskp->tsksyp->synam, stdout);
   if (__outlinpos != 0) { __wrap_putc('\n', stdout); __outlinpos = 0; }
   if (tskp->tsksymtab->sytofs != NULL)
    {
     __pv_stlevel++;
     prt_1tsk_nestscopes(tskp->tsksymtab->sytofs);
     __pv_stlevel--;
    }
  }
 __pv_stlevel -= 2;
}

static void prt_1tsk_nestscopes(struct symtab_t *up_sytp)
{
 struct symtab_t *sytp; 
 struct task_t *tskp;

 for (sytp = up_sytp->sytofs; sytp != NULL; sytp = sytp->sytsib) 
  {
   tskp = sytp->sypofsyt->el.etskp;
   sprintf(__xs, "%s: ", __to_tsktyp(__xs2, tskp->tsktyp));
   __wrap_puts(__xs, stdout);
   __wrap_puts(tskp->tsksyp->synam, stdout);
   if (__outlinpos != 0) { __wrap_putc('\n', stdout); __outlinpos = 0; }
   if (tskp->tsksymtab->sytofs != NULL)
    {
     __pv_stlevel++;
     prt_1tsk_nestscopes(tskp->tsksymtab->sytofs);
     __pv_stlevel--;
    }
  }
}

/*
 * BUILT INTO VERILOG STOCHASTIC QUEUE SYSTEM TASKS
 */

/*
 * execute the qfull function - must push 1 (room), 0 no room onto xstk
 *
 * LOOKATME - is this a 1 bit 0/1?
 */
static void exec_qfull(struct expr_t *argxp)
{
 int32 q_id, rv;
 word32 val;
 struct q_hdr_t *q_p;
 struct expr_t *xp, *a1xp, *a2xp;
 struct xstk_t *xsp;

 rv = 0;
 /* access the required 4 arguments */
 if ((xp = argxp) == NULL) __arg_terr(__FILE__, __LINE__);
 /* first element in function arg. list is return variable */
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a1xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a2xp = xp->lu.x;

 if (!__get_eval_word(a1xp, &val))
  {
   __sgfwarn(596, "$q_full argument 1, q_id value %s x/z or out of range",
    __msgexpr_tostr(__xs, a1xp));
ret_x:
   push_xstk_(xsp, 1);
   xsp->ap[0] = 1;
   xsp->bp[0] = 1;
   rv = 2;
   goto done;
  }
 q_id = (int32) val;

 /* find q that matches passed q id */
 if ((q_p = find_q_from_id(q_id)) == NULL) goto ret_x;

 push_xstk_(xsp, 1);
 xsp->bp[0] = 0;
 if (q_p->q_size >= q_p->q_maxlen) xsp->ap[0] = 1;
 else xsp->ap[0] = 0;

done:
 if (a2xp->optyp == OPEMPTY) return;

 push_xstk_(xsp, WBITS);
 xsp->ap[0] = (word32) rv; 
 xsp->bp[0] = 0L;

 /* SJM 09/29/03 - change to handle sign extension and separate types */
 /* know xsp WBITS but can widen or narow */
 if (xsp->xslen > a2xp->szu.xclen) __narrow_sizchg(xsp, a2xp->szu.xclen);
 else if (xsp->xslen < a2xp->szu.xclen)
  {
   if (a2xp->has_sign) __sgn_xtnd_widen(xsp, a2xp->szu.xclen);
   else __sizchg_widen(xsp, a2xp->szu.xclen);
  }

 __exec2_proc_assign(a2xp, xsp->ap, xsp->bp);
 __pop_xstk();
}

/*
 * initialize a queue
 *
 * know exactly 4 args (possibly ,,) or will not get here
 */
static void do_q_init(struct expr_t *argxp)
{
 int32 q_id, q_type, q_maxlen, rv;
 word32 val;
 struct q_hdr_t *q_p;
 struct expr_t *xp, *a1xp, *a2xp, *a3xp, *a4xp;
 struct xstk_t *xsp;

 rv = 0;
 /* access the required 4 arguments */
 if ((xp = argxp) == NULL) __arg_terr(__FILE__, __LINE__);
 a1xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a2xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a3xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a4xp = xp->lu.x;
 if (xp->ru.x != NULL) __arg_terr(__FILE__, __LINE__);

 /* access the rhs arguments */
 if (!__get_eval_word(a1xp, &val))
  {
   __sgfwarn(596, "$q_initialize argument 1, q_id value %s x/z or out of range",
    __msgexpr_tostr(__xs, a1xp));
   rv = 2;
   q_id = 0;
  }
 else q_id = (int32) val;
 if (!__get_eval_word(a2xp, &val) || val < 1 || val > 2)
  {
   __sgfwarn(596,
    "$q_initialize argument 2, q_type value %s x/z or out of range",
    __msgexpr_tostr(__xs, a2xp));
   if (rv == 0) rv = 4;
   q_type = 0;
  }
 else q_type = (int32) val;
 if (!__get_eval_word(a3xp, &val)) 
  {
bad_qlen:
   __sgfwarn(596,
    "$q_initialize argument 3, max_length value %s x/z or negative",
    __msgexpr_tostr(__xs, a3xp));
   if (rv == 0) rv = 5;
   q_maxlen = 0;
  }
 else
  {
   q_maxlen = (int32) val;
   if (q_maxlen <= 0) goto bad_qlen;
  }
 if (rv != 0) goto done;

 /* make sure id is unqiue */
 if (find_q_from_id(q_id) != NULL) { rv = 6; goto done; }

 /* allocate the new q header and link into q list */
 q_p = (struct q_hdr_t *) __my_malloc(sizeof(struct q_hdr_t));
 init_q(q_p);
 if (__qlist_hdr == NULL) __qlist_hdr = q_p; 
 else { q_p->qhdrnxt = __qlist_hdr; __qlist_hdr = q_p; }
 if (q_type == 1) q_p->q_fifo = TRUE; else q_p->q_fifo = FALSE;
 q_p->qarr = (struct q_val_t *) __my_malloc(q_maxlen*sizeof(struct q_val_t)); 
 memset(q_p->qarr, 0, q_maxlen*sizeof(struct q_val_t));
 q_p->q_id = q_id;
 q_p->q_maxlen = q_maxlen;

done:
 if (a4xp->optyp == OPEMPTY) return;

 push_xstk_(xsp, WBITS);
 xsp->ap[0] = (word32) rv; 
 xsp->bp[0] = 0L;

 /* SJM 09/29/03 - change to handle sign extension and separate types */
 /* know xsp WBITS but can widen or narow */
 if (xsp->xslen > a4xp->szu.xclen) __narrow_sizchg(xsp, a4xp->szu.xclen);
 else if (xsp->xslen < a4xp->szu.xclen)
  {
   if (a2xp->has_sign) __sgn_xtnd_widen(xsp, a4xp->szu.xclen);
   else __sizchg_widen(xsp, a4xp->szu.xclen);
  }

 __exec2_proc_assign(a4xp, xsp->ap, xsp->bp);
 __pop_xstk();
}

/*
 * initialize a q
 */
static void init_q(struct q_hdr_t *q_p)
{
 q_p->q_fifo = FALSE;
 q_p->q_id = 0;
 q_p->q_hdr = -1;
 q_p->q_tail = -1;
 q_p->q_maxlen = 0;
 q_p->q_size = 0;
 q_p->q_minwait = 0xffffffffffffffffULL;
 q_p->qhdrnxt = NULL;
}

/*
 * find q header record from identifying q id number
 *
 * LOOKATME - could use binary search but think will not be many queues
 */
static struct q_hdr_t *find_q_from_id(int32 id)
{
 register struct q_hdr_t *qp;

 for (qp = __qlist_hdr; qp != NULL; qp = qp->qhdrnxt)
  {
   if (qp->q_id == id) return(qp);
  }
 return(NULL);
}

/*
 * add an element to a queue
 *
 * know exactly 4 args (possibly ,,) or will not get here
 */
static void do_q_add(struct expr_t *argxp)
{
 int32 q_id, qjob_id, qinform_id, rv;
 word32 val;
 struct q_hdr_t *q_p;
 struct q_val_t *qvp;
 struct expr_t *xp, *a1xp, *a2xp, *a3xp, *a4xp;
 struct xstk_t *xsp;

 rv = 0;
 /* access the required 4 arguments */
 if ((xp = argxp) == NULL) __arg_terr(__FILE__, __LINE__);
 a1xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a2xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a3xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a4xp = xp->lu.x;
 if (xp->ru.x != NULL) __arg_terr(__FILE__, __LINE__);

 /* access the rhs arguments */
 if (!__get_eval_word(a1xp, &val))
  {
   __sgfwarn(596, "$q_add argument 1, q_id value %s x/z or out of range",
    __msgexpr_tostr(__xs, a1xp));
   rv = 2;
   q_id = 0;
  }
 else q_id = (int32) val;
 if (a2xp->optyp == OPEMPTY) qjob_id = 0;
 else
  {
   if (!__get_eval_word(a2xp, &val))
    {
     __sgfwarn(596,
      "$q_add argument 2, job_id value %s x/z or too wide (0 used)",
      __msgexpr_tostr(__xs, a2xp));
     val = 0;
    }
   qjob_id = (int32) val;
  }
 if (a3xp->optyp == OPEMPTY) qinform_id = 0;
 else
  {
   if (!__get_eval_word(a3xp, &val)) 
    {
     __sgfwarn(596,
      "$q_add argument 3, inform_id value %s x/z or too wide (0 used)",
      __msgexpr_tostr(__xs, a3xp));
     val = 0;
    }
   qinform_id = (int32) val;
  }
 if (rv != 0) goto done;

 /* find q that matches passed q id */
 if ((q_p = find_q_from_id(q_id)) == NULL) { rv = 2; goto done; }

 /* add the element */
 if (q_p->q_fifo)
  {
   if (q_p->q_hdr == -1) q_p->q_hdr = q_p->q_tail = 0;
   else
    {
     if (q_p->q_size >= q_p->q_maxlen) { rv = 1; goto done; }
     (q_p->q_hdr)++;
     /* wrap queue around - since size not too big know empty */ 
     if (q_p->q_hdr >= q_p->q_maxlen) q_p->q_hdr = 0;
    }
  }
 else
  {
   /* easy stack lifo case - q tail not used */
   if (q_p->q_hdr == -1) q_p->q_hdr = 0;
   else
    {
     if (q_p->q_size >= q_p->q_maxlen) { rv = 1; goto done; }
     (q_p->q_hdr)++;
    }
  }
 qvp = &(q_p->qarr[q_p->q_hdr]);
 (q_p->q_size)++;
 if (q_p->q_size > q_p->q_maxsize) q_p->q_maxsize = q_p->q_size;
 qvp->job_id = qjob_id;
 qvp->inform_id = qinform_id;
 qvp->enter_tim = __simtime;

done:
 if (a4xp->optyp == OPEMPTY) return;

 push_xstk_(xsp, WBITS);
 xsp->ap[0] = (word32) rv; 
 xsp->bp[0] = 0L;

 /* SJM 09/29/03 - change to handle sign extension and separate types */
 /* know xsp WBITS but can widen or narow */
 if (xsp->xslen > a4xp->szu.xclen) __narrow_sizchg(xsp, a4xp->szu.xclen);
 else if (xsp->xslen < a4xp->szu.xclen)
  {
   if (a2xp->has_sign) __sgn_xtnd_widen(xsp, a4xp->szu.xclen);
   else __sizchg_widen(xsp, a4xp->szu.xclen);
  }

 __exec2_proc_assign(a4xp, xsp->ap, xsp->bp);
 __pop_xstk();
}

/*
 * delete an element from a queue
 *
 * know exactly 4 args (possibly ,,) or will not get here
 */
static void do_q_remove(struct expr_t *argxp)
{
 int32 q_id, qjob_id, qinform_id, rv;
 word32 val;
 word64 timval;
 struct q_hdr_t *q_p;
 struct q_val_t *qvp;
 struct expr_t *xp, *a1xp, *a2xp, *a3xp, *a4xp;
 struct xstk_t *xsp;

 rv = 0;
 /* access the required 4 arguments - last 3 outputs can be empty */
 if ((xp = argxp) == NULL) __arg_terr(__FILE__, __LINE__);
 a1xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a2xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a3xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a4xp = xp->lu.x;
 if (xp->ru.x != NULL) __arg_terr(__FILE__, __LINE__);

 /* access the rhs arguments */
 if (!__get_eval_word(a1xp, &val))
  {
   __sgfwarn(596, "$q_remove argument 1, q_id value %s x/z or out of range",
    __msgexpr_tostr(__xs, a1xp));
   rv = 2;
   goto done;
  }
 else q_id = (int32) val;

 /* find q that matches passed q id */
 if ((q_p = find_q_from_id(q_id)) == NULL) { rv = 2; goto done; }

 /* here no assignment to output values */
 if (q_p->q_size == 0) { rv = 3; goto done; }

 qvp = &(q_p->qarr[q_p->q_tail]); 
 /* delete the element - take off from tail */
 if (q_p->q_fifo)
  {
   if (q_p->q_size == 1) q_p->q_hdr = q_p->q_tail = -1;
   else
    {
     (q_p->q_tail)++;
     /* wrap queue around - since size not too big know empty */ 
     if (q_p->q_tail >= q_p->q_maxlen) q_p->q_tail = 0;
    }
  }
 /* easy stack lifo case - q tail not used */
 else (q_p->q_hdr)--;

 /* save minimum time in q (wait time) */
 timval = __simtime - qvp->enter_tim;
 if (timval < q_p->q_minwait) q_p->q_minwait = timval;

 (q_p->q_size)--;
 qjob_id = qvp->job_id;
 qinform_id = qvp->inform_id;

 if (a2xp->optyp != OPEMPTY)
  {
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = (word32) qjob_id; 
   xsp->bp[0] = 0L;

   /* SJM 09/29/03 - change to handle sign extension and separate types */
   /* know xsp WBITS but can widen or narow */
   if (xsp->xslen > a2xp->szu.xclen) __narrow_sizchg(xsp, a2xp->szu.xclen);
   else if (xsp->xslen < a2xp->szu.xclen)
    {
     if (a2xp->has_sign) __sgn_xtnd_widen(xsp, a2xp->szu.xclen);
     else __sizchg_widen(xsp, a2xp->szu.xclen);
    }

   __exec2_proc_assign(a2xp, xsp->ap, xsp->bp);
   __pop_xstk();
  }

 if (a3xp->optyp != OPEMPTY)
  {
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = (word32) qinform_id; 
   xsp->bp[0] = 0L;

   /* SJM 09/29/03 - change to handle sign extension and separate types */
   /* know xsp WBITS but can widen or narow */
   if (xsp->xslen > a3xp->szu.xclen) __narrow_sizchg(xsp, a3xp->szu.xclen);
   else if (xsp->xslen < a3xp->szu.xclen)
    {
     if (a3xp->has_sign) __sgn_xtnd_widen(xsp, a3xp->szu.xclen);
     else __sizchg_widen(xsp, a3xp->szu.xclen);
    }

   __exec2_proc_assign(a3xp, xsp->ap, xsp->bp);
   __pop_xstk();
  }

done:
 if (a4xp->optyp == OPEMPTY) return;

 push_xstk_(xsp, WBITS);
 xsp->ap[0] = (word32) rv; 
 xsp->bp[0] = 0L;

 /* SJM 09/29/03 - change to handle sign extension and separate types */
 /* know xsp WBITS but can widen or narow */
 if (xsp->xslen > a4xp->szu.xclen) __narrow_sizchg(xsp, a4xp->szu.xclen);
 else if (xsp->xslen < a4xp->szu.xclen)
  {
   if (a4xp->has_sign) __sgn_xtnd_widen(xsp, a4xp->szu.xclen);
   else __sizchg_widen(xsp, a4xp->szu.xclen);
  }

 __exec2_proc_assign(a4xp, xsp->ap, xsp->bp);
 __pop_xstk();
}

/*
 * examine a queue
 *
 * know exactly 4 args (possibly ,,) or will not get here
 */
static void do_q_examine(struct expr_t *argxp)
{
 int32 q_id, q_stat_code, rv;
 word32 val;
 word64 timval;
 struct q_hdr_t *q_p;
 struct expr_t *xp, *a1xp, *a2xp, *a3xp, *a4xp;
 struct xstk_t *xsp;

 rv = 0;
 /* access the required 4 arguments - last 3 outputs can be empty */
 if ((xp = argxp) == NULL) __arg_terr(__FILE__, __LINE__);
 a1xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a2xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a3xp = xp->lu.x;
 if ((xp = xp->ru.x) == NULL) __arg_terr(__FILE__, __LINE__);
 a4xp = xp->lu.x;
 if (xp->ru.x != NULL) __arg_terr(__FILE__, __LINE__);

 /* access the rhs arguments */
 if (!__get_eval_word(a1xp, &val))
  {
   __sgfwarn(596, "$q_examine argument 1, q_id value %s x/z or out of range",
    __msgexpr_tostr(__xs, a1xp));
   rv = 2;
   goto done;
  }
 q_id = (int32) val;

 if (!__get_eval_word(a2xp, &val) || val < 1 || val > 6)
  {
   __sgfwarn(596,
    "$q_examine argument 2, q_stat_code value %s x/z or out of range",
    __msgexpr_tostr(__xs, a2xp));
   /* LOOKATME - really no good value for this error */
   q_stat_code = 0;
   rv = 4;
  }
 else q_stat_code = (int32) val;

 if (rv != 0) goto done;

 /* find q that matches passed q id */
 if ((q_p = find_q_from_id(q_id)) == NULL) { rv = 2; goto done; }

 switch (q_stat_code) {
  case 1:
   /* current size of q */
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = (word32) q_p->q_size;
   xsp->bp[0] = 0L;
   break;
  case 2:
   /* mean inter arrival time for elements currently in the q */
   cmp_mean_interarriv_tim(&timval, q_p);
push_cmp_tim:
   push_xstk_(xsp, 64);
   xsp->ap[0] = (word32) (timval & WORDMASK_ULL);
   xsp->ap[1] = (word32) ((timval >> 32) & WORDMASK_ULL); 
   xsp->bp[0] = xsp->bp[1] = 0L;
   break;
  case 3:
   /* maximum size queue ever attained */
   push_xstk_(xsp, WBITS);
   xsp->ap[0] = (word32) q_p->q_maxsize;
   xsp->bp[0] = 0L;
   break;
  case 4:
   push_xstk_(xsp, 64);
   xsp->ap[0] = (word32) (q_p->q_minwait & WORDMASK_ULL);
   xsp->ap[1] = (word32) ((q_p->q_minwait >> 32) & WORDMASK_ULL);
   xsp->bp[0] = xsp->bp[1] = 0L;
   break;
  case 5:
   cmp_max_wait(&timval, q_p);
   goto push_cmp_tim;
  case 6:
   cmp_mean_wait_tim(&timval, q_p);
   goto push_cmp_tim;
  default: __case_terr(__FILE__, __LINE__); return;
 }

 /* only assign result, lhs arg. passed */
 if (a3xp->optyp != OPEMPTY)
  {
   /* SJM 09/29/03 - change to handle sign extension and separate types */
   /* know xsp WBITS but can widen or narow */
   if (xsp->xslen > a3xp->szu.xclen) __narrow_sizchg(xsp, a3xp->szu.xclen);
   else if (xsp->xslen < a3xp->szu.xclen)
    {
     if (a3xp->has_sign) __sgn_xtnd_widen(xsp, a3xp->szu.xclen);
     else __sizchg_widen(xsp, a3xp->szu.xclen);
    }

   __exec2_proc_assign(a3xp, xsp->ap, xsp->bp);
  }
 __pop_xstk();

done:
 if (a4xp->optyp == OPEMPTY) return;

 push_xstk_(xsp, WBITS);
 xsp->ap[0] = (word32) rv; 
 xsp->bp[0] = 0L;

 /* SJM 09/29/03 - change to handle sign extension and separate types */
 /* know xsp WBITS but can widen or narow */
 if (xsp->xslen > a4xp->szu.xclen) __narrow_sizchg(xsp, a4xp->szu.xclen);
 else if (xsp->xslen < a4xp->szu.xclen)
  {
   if (a4xp->has_sign) __sgn_xtnd_widen(xsp, a4xp->szu.xclen);
   else __sizchg_widen(xsp, a4xp->szu.xclen);
  }

 __exec2_proc_assign(a4xp, xsp->ap, xsp->bp);
 __pop_xstk();
}

/*
 * compute average inter (between 2) arrival time for qs currently in queue
 */
static void cmp_mean_interarriv_tim(word64 *timvalp, struct q_hdr_t *q_p)
{
 register int32 qi, i;
 word64 avgtim, arrdif, quot, rem;

 /* for one or less size q mean always 0 */
 if (q_p->q_size <= 1) { *timvalp = 0ULL; return; }
 avgtim = 0ULL;
 if (q_p->q_fifo)
  {
   for (qi = q_p->q_hdr, i = 0; i < q_p->q_size; i++)
    {
     if ((++qi) >= q_p->q_size) qi = 0;
     if (i == 0) continue;

     arrdif = q_p->qarr[qi].enter_tim - q_p->qarr[qi - 1].enter_tim;
     avgtim += arrdif;
    }
  }
 else
  {
   /* easy lifo stack case */
   for (qi = 0; qi < q_p->q_size; qi++)
    {
     if (qi == 0) continue;
     arrdif = q_p->qarr[qi].enter_tim - q_p->qarr[qi - 1].enter_tim;
     avgtim += arrdif;
    }
  }
 /* divide - round - know q at least 2 elements to get here */
 /* SJM 02/03/00 - cast of negative (>2**31) sign extends need word32 1st */
 quot = avgtim/((word64) (((word32) q_p->q_size) - 1)); 
 rem = avgtim % ((word64) (((word32) q_p->q_size) - 1));
 avgtim = quot;
 if (rem >= ((word64) (((word32) q_p->q_size)/2))) avgtim++;
 *timvalp = avgtim;
}

/*
 * compute longest wait (in queue) time for elements in queue
 */
static void cmp_max_wait(word64 *timvalp, struct q_hdr_t *q_p)
{
 register int32 qi, i;
 word64 inqtim;

 if (q_p->q_size <= 1) { *timvalp = 0ULL; return; }
 if (q_p->q_fifo)
  {
   for (qi = q_p->q_hdr, i = 0; i < q_p->q_size; i++)
    {
     if ((++qi) >= q_p->q_size) qi = 0;
     if (i == 0)
      {
       *timvalp = q_p->qarr[qi].enter_tim;
       continue;
      }
     inqtim = __simtime - q_p->qarr[qi].enter_tim;
     if (inqtim < *timvalp) *timvalp = inqtim;
    }
  }
 else
  {
   /* easy lifo stack case */
   for (qi = 0; qi < q_p->q_size; qi++)
    {
     if (qi == 0)
      {
       *timvalp = q_p->qarr[qi].enter_tim;
       continue;
      }
     inqtim = __simtime - q_p->qarr[qi].enter_tim;
     if (inqtim < *timvalp) *timvalp = inqtim;
    }
  }
}

/*
 * compute average (mean) time each element has spent in queue
 */
static void cmp_mean_wait_tim(word64 *timvalp, struct q_hdr_t *q_p)
{
 register int32 qi, i;
 word64 avgtim, waitdif, quot, rem;

 if (q_p->q_size <= 0) { *timvalp = 0ULL; return; }
 avgtim = 0ULL;
 if (q_p->q_fifo)
  {
   for (qi = q_p->q_hdr, i = 0; i < q_p->q_size; i++)
    {
     if ((++qi) >= q_p->q_size) qi = 0;
     waitdif = __simtime - q_p->qarr[qi].enter_tim; 
     avgtim += waitdif;
    }
  }
 else
  {
   /* easy lifo stack case */
   for (qi = 0; qi < q_p->q_size; qi++)
    {
     waitdif = __simtime - q_p->qarr[qi].enter_tim;
     avgtim += waitdif;
    }
  }
 /* divide - round - know q at least 1 element to get here */
 quot = avgtim/((word64) ((word32) q_p->q_size)); 
 rem = avgtim % ((word64) ((word32) q_p->q_size));
 avgtim = quot;
 if (rem >= ((word64) (((word32) q_p->q_size)/2))) avgtim++;
 *timvalp = avgtim;
}

/*
 * TIMESCALE TASK ROUTINES 
 */

/*
 * execute the print time scale
 * know the 1 arg. is xmr or no arg means current scope
 *
 * know arg. is xmr even if only has one component
 * axp is nil (for none) or function call comma operator (head of list)
 */
static void exec_prttimscale(struct expr_t *axp)
{
 struct mod_t *mdp;
 struct expr_t *ndp; 
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 __cur_sofs = 0;
 __adds("Time scale of (");
 if (axp == NULL)
  {
   mdp = __scope_ptr->itip->imsym->el.emdp;
   __disp_itree_path(__inst_ptr, (struct task_t *) NULL);
  }
 else 
  {
   ndp = axp->lu.x;
   if (ndp->optyp == GLBREF)
    { mdp = ndp->ru.grp->targmdp; __adds(ndp->ru.grp->gnam); }
   else if (ndp->lu.sy->sytyp == SYM_M)
    { mdp = ndp->lu.sy->el.emdp; __adds(mdp->msym->synam); }
   else { __case_terr(__FILE__, __LINE__);  return; }
  }
 sprintf(s1, ") is %s / %s\n",
  __to_timunitnam(s2, mdp->mtime_units),
  __to_timunitnam(s3, mdp->mtime_units + mdp->mtime_prec));
 __adds(s1);
 /* notice cannot truncate here */
 my_puts_(__exprline, stdout);
 __cur_sofs = 0;
}

/*
 * execute the time format system task - just sets some internal values 
 * know exactly four arguments or will not get here
 *
 * this allows ,, and missing arguments that mean use 0 - error
 */
static void exec_timefmt(struct expr_t *argxp) 
{
 int32 argi, i1, slen;
 word32 val, val1, val2, val3;
 struct expr_t *xp, *ndp;
 char *chp;

 /* set values for missing arguments */
 val1 = val2 = val3 = 0; 

 argi = 1;
 /* empty arg. list already checked for */
 if ((xp = argxp) == NULL) __arg_terr(__FILE__, __LINE__);
 ndp = xp->lu.x;
 if (ndp->optyp != OPEMPTY)
  {
   /* eval. here is just word32 stored in 2 complement */
   if (!__get_eval_word(ndp, &val)) goto bad_arg; 
   i1 = (int32) val;
   if (i1 > 0 || i1 < -15) goto bad_arg;
   i1 = -i1;
   val1 = (word32) i1;
   if (val1 > __des_timeprec)
    { 
     __sgferr(1240,
      "$timeformat units %s (%d) impossible - must be larger than %s (%d) tick",
       __to_timunitnam(__xs, (word32) val1), -((int32) val1),
       __to_timunitnam(__xs2, (word32) __des_timeprec),
       -((int32) __des_timeprec));
     /* change nothing if this does not change */
     return;
    }
  }
 if ((xp = xp->ru.x) == NULL)
  { chp = __my_malloc(1); *chp = '\0'; slen = 1; goto do_chg; }

 ndp = xp->lu.x; 
 argi++;
 if (ndp->optyp != OPEMPTY)
  {
   if (!__get_eval_word(ndp, &val)) goto bad_arg;
   i1 = (int32) val;
   if (i1 < 0 || i1 >= RECLEN) goto bad_arg;
   val2 = val;
  }

 if ((xp = xp->ru.x) == NULL)
  { chp = __my_malloc(1); *chp = '\0'; slen = 1; goto do_chg; }
 ndp = xp->lu.x;
 argi++;
 if (ndp->optyp != OPEMPTY)
  {
   chp = __get_eval_cstr(ndp, &slen);
   /* must fit in RECLEN style string - but maybe should be narrower */ 
   if (slen >= RECLEN) { __my_free(chp, slen + 1); goto bad_arg; }
  }
 else { chp = __my_malloc(1); *chp = '\0'; slen = 1; }
 
 if ((xp = xp->ru.x) == NULL) goto do_chg;
 ndp = xp->lu.x;
 argi++;
 /* must allow ,) form */
 if (ndp->optyp != OPEMPTY)
  {
   if (!__get_eval_word(ndp, &val) || val > 40)
    {
     __sgferr(1047,
      "$timeformat minimum field width must be between 0 and 40 - not changed");
     __my_free(chp, slen + 1);
     return;
    }
   val3 = val;
  }
 
do_chg:
 if (slen > (int32) (val3 + 1))  
  {
   __sgferr(1047,
    "$timeformat suffix length %d wider than minimum field width (%d) - not changed",
    slen, val3);
   __my_free(chp, slen + 1);
   return;
  }
 __tfmt_units = val1;
 __tfmt_precunits = val2;
 __my_free((char *) __tfmt_suf, strlen(__tfmt_suf) + 1);
 __tfmt_suf = chp;
 __tfmt_minfwid = val3;
 return;

bad_arg:
 __sgferr(713, "$timeformat argument %d value %s x/z or out of range",
  argi, __msgexpr_tostr(__xs, ndp));
}

/*
 * get an optional system task control argument
 * gets converted to machine int
 * this must be called with xp head of fcall list (comma operator)
 */
static int32 get_opt_starg(struct expr_t *xp, int32 dflt_val)
{
 int32 val;
 word32 rval;
 struct expr_t *axp;

 if (xp == NULL) return(dflt_val);
 axp = xp->lu.x;
 if (axp->optyp == OPEMPTY) return(dflt_val);

 if (!__get_eval_word(axp, &rval))
  {
   __sgfwarn(519,
    "optional system task numeric argument has x/z bits - default used");
   return(dflt_val);
  }
 val = (int32) rval;
 return(val); 
}

/*
 * evaluate a value to an int32 (return F if not a non x/z WBIT int32)
 * this must be called with actual argument expr. not fcall comma expr.
 */
extern int32 __get_eval_word(struct expr_t *xp, word32 *wval)
{
 int32 rval;
 struct xstk_t *xsp;

 *wval = 0;
 xsp = __eval_xpr(xp); 
 /* semantics says there is an implied conversion from real to int32 */
 /* but not across system task/func. arguments */
 /* however this routine is only called when int32 needed */
 if (xp->optyp == REALNUM || xp->optyp == ISREALNUM)
  {
   double d1;

   /* truncating since for getting 32 bit value */
   memcpy(&d1, xsp->ap, sizeof(double));
   *wval = (word32) d1; 
   rval = TRUE; 
   goto done;
  }
 if (xsp->xslen > WBITS)
  {
   if (!vval_is0_(&(xsp->ap[1]), xsp->xslen - WBITS)
    || !vval_is0_(&(xsp->bp[1]), xsp->xslen - WBITS))
    { rval = FALSE; goto done; }
  }
 if (xsp->bp[0] != 0L) { rval = FALSE; goto done; }
 *wval = xsp->ap[0];
 rval = TRUE;

done:
 __pop_xstk();
 return(rval);
}

/*
 * exec the $log file system task
 * this is called with fcall comma operator header
 */
static void exec_log_fnamchg(struct expr_t *axp)
{ 
 int32 slen;
 FILE *tmp_s;
 char *chp;

 if (axp == NULL) { __log_s = __save_log_s; return; }
 chp = __get_eval_cstr(axp->lu.x, &slen);
 if ((tmp_s = __tilde_fopen(chp, "w")) == NULL)
  {
   __sgferr(1243, 
    "cannot open new $log output transcript file %s - not changed",
    __exprline); 
   __my_free(chp, slen + 1);
   return;
  }
 if (__log_s != NULL && __log_s != stdout && __log_s != stderr)
  {
   __my_fclose(__log_s);
   __my_free(__log_fnam, strlen(__log_fnam) + 1);
   __log_fnam = NULL;
  }
 __log_fnam = chp; 
 __log_s = tmp_s;
 __save_log_s = NULL;
 /* SJM 03/26/00 - log file now not an mcd - lumped with 0 (stdout) */
 /* ---
 __mulchan_tab[2].mc_fnam = __pv_stralloc(__log_fnam);
 __mulchan_tab[2].mc_fnam = __pv_stralloc(__log_fnam);
 --- */ 
 if (__verbose)
  __cv_msg("  Now writing output log to file \"%s\".\n", __log_fnam);
}

/*
 * exec the $tracefile system task
 * always open - if no writing into then just empty file
 *
 * will not get here if name argument missing 
 * this is called with actual file name argument not fcall list header op
 * 
 */
static void exec_trace_fnamchg(struct expr_t *argvx)
{ 
 int32 slen;
 FILE *tmp_s;
 char *chp;

 chp = __get_eval_cstr(argvx, &slen);
 if (strcmp(chp, "STDOUT") == 0) strcpy(chp, "stdout");
 if (strcmp(__tr_fnam, chp) == 0) 
  {
   __sgfwarn(625, "$tracefile file name %s same as previous - task ignored",
    __tr_fnam);
   goto done;
  }
 /* if changing to stdout set it, but cannot open */
 if (strcmp(chp, "stdout") == 0)
  {
   if (__tr_s != NULL && __tr_s != stdout && __tr_s != stderr)
    __my_fclose(__tr_s);

   if (__tr_fnam != NULL) __my_free(__tr_fnam, strlen(__tr_fnam) + 1);
   __tr_fnam = chp;
   __tr_s = stdout;
   goto new_tr;
  }
 /* know new file not stdout - always open system task new trace file */
 if ((tmp_s = __tilde_fopen(chp, "w")) == NULL)
  {
   __sgferr(1247, "cannot open new trace output file %s - not changed",
    chp); 
   goto done;
  }
 if (__tr_s != NULL && __tr_s != stdout && __tr_s != stderr)
  __my_fclose(__tr_s);

 if (__tr_fnam != NULL) __my_free(__tr_fnam, strlen(__tr_fnam) + 1);
 __tr_fnam = chp;
 __tr_s = tmp_s;

new_tr:
 if (__verbose) 
  {
   __cv_msg(
   "  Now writing statement and/or event trace output to file \"%s\".\n",
   __tr_fnam);
  }
 return;

done:
 __my_free(chp, slen + 1);
}

/*
 * execute a $scope change
 * this can be used for scope changes into local task from instance
 */
static void exec_expr_schg(struct expr_t *xp)
{
 struct itree_t *itp;
 struct task_t *tskp; 
 struct sy_t *syp;

 /* will not get here if no argument */

 /* need to handle scope change into local task - inst. does not change */
 /* scope changes of local [lb].[lb].[lb] simple task target by here */
 if (xp->optyp == ID)
  { 
   syp = xp->lu.sy;
   /* DBG remove */
   if (syp->sytyp != SYM_TSK && syp->sytyp != SYM_F && syp->sytyp != SYM_LB)
    __arg_terr(__FILE__, __LINE__);
   /* --- */
   __scope_tskp = xp->lu.sy->el.etskp;
   if (__iact_state) __set_scopchg_listline();
   return;
  } 

 /* DBG remove --- */
 if (xp->optyp != GLBREF) __arg_terr(__FILE__, __LINE__);
 /* --- */ 

 /* this converts from gref to itree location */
 __xmrpush_refgrp_to_targ(xp->ru.grp);
 itp = __inst_ptr;
 __pop_itstk();
 if (xp->lu.sy->sytyp != SYM_I && xp->lu.sy->sytyp != SYM_M)
  tskp = xp->lu.sy->el.etskp;
 else tskp = NULL;
 __scope_ptr = itp; 
 __scope_tskp = tskp;
 /* if called from interactive must update list line to scope */
 if (__iact_state)
  {
   /* in iact, need top of inst. stack to be same as scope ptr */
   __pop_itstk();
   __push_itstk(__scope_ptr);
   __set_scopchg_listline();

   if (__tfrec_hdr != NULL) __call_misctfs_scope();
   if (__have_vpi_actions) __vpi_iactscopechg_trycall();
  }
}

/*
 * emit various systask time end message - task passed >= 2 arg 
 */
extern void __emit_stsk_endmsg(void)
{
 /* notice must know current end time */
 __my_ftime(&__end_time, &__end_mstime);
 __prt_end_msg();
}

/*
 * ROUTINES TO MAKE VARIABLE DECLARATION INITIALIZE ASSIGNMENTS
 */

/*
 * after init sim and any -i interactive running, set all var decl initials
 *
 * semantics is same as: reg [r1:r2] x; initial x = [expr]; 
 */
extern void __exec_var_decl_init_assigns(void)
{
 register struct varinitlst_t *initp;
 register struct net_t *np;
 register struct expr_t *xp; 
 struct xstk_t *xsp;
 struct mod_t *mdp;
 int32 ii;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mvarinits == NULL) continue; 

   for (ii = 0; ii < mdp->flatinum; ii++)
    {
     __push_itstk(mdp->moditps[ii]);
     
     for (initp = mdp->mvarinits; initp != NULL; initp = initp->varinitnxt)
      { 
       xp = initp->init_xp;
       np = initp->init_syp->el.enp;
       /* notice this code is almost same as eval assign rhsexpr except */ 
       /* do not have lhs expr but instead have net */
       xsp = __eval_xpr(initp->init_xp);
       if (np->ntyp == N_REAL)
        { 
         if (!xp->is_real) __cnv_stk_fromreg_toreal(xsp, (xp->has_sign == 1));
        }
       else
        { 
         if (xp->is_real) __cnv_stk_fromreal_toreg32(xsp);

         if (xsp->xslen > np->nwid) __narrow_sizchg(xsp, np->nwid);
         else if (xsp->xslen < np->nwid)
          {
           if (xp->has_sign) __sgn_xtnd_widen(xsp, np->nwid);
           else __sizchg_widen(xsp, np->nwid);
          }
        }

       /* notice may need change store here - works because netchg list hd */
       /* initialized in init stim so at end of first time 0 queue segment */
       /* the changes will be processed */
       if (np->nchg_nd_chgstore) __chg_st_val(np, xsp->ap, xsp->bp);
       else __st_val(np, xsp->ap, xsp->bp);
       __pop_xstk();
      }
     __pop_itstk();
    }
  }
}

/* 
 * ROUTINES TO IMPLEMENT FILE IO SYS TASKS AND FUNCS (INTERMIXED)
 */

/*
 * open a OS file system and return the 32 bit file descriptor
 * fd is OS file descriptor although using buffered read/write I/O
 */
static word32 fio_do_fopen(struct expr_t *axp, struct expr_t *mode_xp)
{
 int32 slen, slen2;
 word32 rv;
 char *chp, *chp2;

 /* these always return something as a string - can never fail */ 
 chp = __get_eval_cstr(axp, &slen);
 chp2 = __get_eval_cstr(mode_xp, &slen2);
 rv = fio_fopen(chp, chp2);

 /* get eval cstr puts string into malloc memory, must free */
 __my_free(chp, slen + 1);
 __my_free(chp2, slen2 + 1);
 return(rv);
}

/*
 * file descriptor fopen and update fio used file table 
 * returns 0 on error else file Verilog fd number (with high bit on)
 *
 * notice the verilog file I/O number may not match the OS one
 */
static word32 fio_fopen(char *chp, char *fmode)
{
 int32 fd;
 FILE *fd_s;
 struct fiofd_t *fdtp;
 char os_mode[RECLEN];

 /* check the fmode string */
 if (!chk_cnvt_fd_modes(os_mode, fmode)) { errno = EINVAL; return(0); }

 /* notice if too many open files (use PLI plus Verilog fd open) this fails */
 if ((fd_s = __tilde_fopen(chp, os_mode)) == NULL)
  {
   /* notice errno set by OS file open call */
   return(0);
  }
 fd = fileno(fd_s);
 /* SJM 09/08/03 - ??? can file name be "stdin" here - think not */
 if (fd == -1 || fd < FIO_STREAM_ST) { errno = EBADF; return(0); }

 /* SJM 08/09/03 - change so always uses OS file number as index */
 /* internal error if same returned twice for open file */
 if (__fio_fdtab[fd] != NULL)
  {
   /* not quite right since error really fd number in use */
   errno = EEXIST;
   __misc_terr(__FILE__, __LINE__);
   return(0);
  }

 /* notice index with high bit on is the Verilog side file descriptor no. */
 fdtp = (struct fiofd_t *) __my_malloc(sizeof(struct fiofd_t));
 fdtp->fd_error = FALSE;
 fdtp->fd_name = __pv_stralloc(chp);
 /* notice can always get fd from stream using fileno C lib func */
 fdtp->fd_s = fd_s;
 __fio_fdtab[fd] = fdtp; 
 
 return(fd | FIO_MSB);
}

/*
 * check and convert the file modes (types for file descriptions) strings
 * returns F on fail else T
 *
 * ending 'b' allowed but never used for unix  
 * SJM 09/08/03 - must fix for other OSes
 */
static int32 chk_cnvt_fd_modes(char *os_mode, char *ver_mode)
{
 /* assume ver mode string and OS mode string same */ 
 strcpy(os_mode, ver_mode); 

 if (strcmp(ver_mode, "r") == 0) return(TRUE);
 if (strcmp(ver_mode, "rb") == 0) { strcpy(os_mode, "r"); return(TRUE); }
 if (strcmp(ver_mode, "w") == 0) return(TRUE);
 if (strcmp(ver_mode, "wb") == 0) { strcpy(os_mode, "w"); return(TRUE); }
 if (strcmp(ver_mode, "a") == 0) return(TRUE);
 if (strcmp(ver_mode, "ab") == 0) { strcpy(os_mode, "a"); return(TRUE); }
 if (strcmp(ver_mode, "r+") == 0) return(TRUE);
 if (strcmp(ver_mode, "r+b") == 0) { strcpy(os_mode, "r+"); return(TRUE); }
 if (strcmp(ver_mode, "rb+") == 0) { strcpy(os_mode, "r+"); return(TRUE); }
 if (strcmp(ver_mode, "w+") == 0) return(TRUE);
 if (strcmp(ver_mode, "w+b") == 0) { strcpy(os_mode, "w+"); return(TRUE); }
 if (strcmp(ver_mode, "wb+") == 0) { strcpy(os_mode, "w+"); return(TRUE); }
 if (strcmp(ver_mode, "a+") == 0) return(TRUE);
 if (strcmp(ver_mode, "a+b") == 0) { strcpy(os_mode, "a+"); return(TRUE); }
 if (strcmp(ver_mode, "ab+") == 0) { strcpy(os_mode, "a+"); return(TRUE); }
 return(FALSE);
}

/*
 * close either an mcd (all files - bits) or one file if fd passed
 * this is sys task so just sets errno on error - no error return
 *
 * SJM 09/08/03 - FIXME ??? need to cancel pending f monits and strobes
 */
static void fio_do_fclose(struct expr_t *axp)
{
 int32 fd, is_mcd;

 /* this sets error nunber */
 if ((fd = chk_get_mcd_or_fd(axp->lu.x, &is_mcd)) == -1) return;

 /* case close mcd */
 if (is_mcd)
  {
   /* just have this re-eval mcd */
   mcd_do_fclose(axp);
   return;
  }
 
 /* notice $fclose does not return anything but vpi mcd fclose with fd does */
 __fd_do_fclose(fd);
}

/*
 * close a file descriptor and return 0 on success and 1 on error 
 */
extern int32 __fd_do_fclose(int32 fd)
{
 int32 slen;
 FILE *f;

 /* know fd in range but if not open error */ 
 if (__fio_fdtab[fd] == NULL)
  {
   errno = EBADF;
   /* SJM 09/23/03 - STRANGE but LRM says return open mcd numbers even here */
   return(bld_open_mcd());
  }
 
 /* must save fd stream before freeing */
 f = __fio_fdtab[fd]->fd_s;
 slen = strlen(__fio_fdtab[fd]->fd_name);
 __my_free(__fio_fdtab[fd]->fd_name, slen + 1); 
 __my_free((char *) __fio_fdtab[fd], sizeof(struct fiofd_t)); 
 __fio_fdtab[fd] = NULL;
 __my_fclose(f);

 return(0);
}
 
/*
 * check and then convert mcd or fd expressions to int
 * returns -1 on error else mcd or fd number (with high bit off)
 * sets is_mcd arg to 1 if mcd to 0 for Unix fd 
 *
 * there is implied truncation to 32 bits so if wider with x's ok
 */
static int32 chk_get_mcd_or_fd(struct expr_t *fdxp, int32 *is_mcd)
{
 word32 fd;
 struct xstk_t *xsp;

 /* assume new file descriptor passed */
 *is_mcd = FALSE;
 xsp = __eval_xpr(fdxp);
 if (xsp->bp[0] != 0L) { errno = EBADF; __pop_xstk(); return(-1); }

 fd = xsp->ap[0];
 __pop_xstk();

 /* if high bit 0, then know mcd */
 /* SJM 09/30/06 - using wrong mask - need only high bit on for test */
 /* was using fio fd wrongly */
 if ((fd & FIO_MSB) == 0)
  {
   *is_mcd = TRUE;
   return(fd);
  } 
 /* turn off high bit for file descriptor */
 fd &= ~(FIO_FD);
 if (fd >= FOPEN_MAX) { errno = EBADF; return(-1); }
 if (__fio_fdtab[fd] == NULL) { errno = EBADF; return(-1); } 
 return(fd);
}

/*
 * flush either an mcd (all bits on) or one fd file
 */
static void fio_fflush(struct expr_t *axp)
{
 register int32 i;
 int32 fd, is_mcd;
 word32 mcd;

 /* this sets error nunber */
 if ((fd = chk_get_mcd_or_fd(axp, &is_mcd)) == -1) return;

 /* case close mcd */
 if (is_mcd)
  {
   mcd = (word32) fd;
   /* SJM 09/09/03 - bit 31 now not used for mcds */   
   for (i = 1; i < 30; i++)
    {
     if (((mcd >> i) & 1L) != 0L)
      {
       if (__mulchan_tab[i].mc_s == NULL)
        {
         __sgfinform(583,
          "multi-channel descriptor bit %d on, but file not open",  i);
        }
       else fflush(__mulchan_tab[i].mc_s);
      }
    }
   return;
  }

 /* know fd in range but if not open error */ 
 if (__fio_fdtab[fd] == NULL) { errno = EBADF; return; }
 fflush(__fio_fdtab[fd]->fd_s);
}

/* 
 * get a character from stream with verilog file descripter expr fdxp
 *
 * SJM 08/09/03 - using literal -1 but maybe should be using EOF define?
 * SJM 08/09/03 - LOOKATME - assuming OS will catch seeking on std[in,out,err]
 * files
 */
static int32 fio_ungetc(struct expr_t *chxp, struct expr_t *fdxp)
{
 int32 c, fd, ival;
 struct xstk_t *xsp;

 /* implied assign to 8 bits - if b part non zero implied assign to 32 bits */
 xsp = __eval_xpr(chxp); 
 if (xsp->bp[0] != 0) { errno = EINVAL; __pop_xstk(); return(-1); }
 /* this insures good char */ 
 c = (int32) (xsp->ap[0] & 0xff);
 __pop_xstk();
 
 /* fd is OS file number with high bit anded off */
 if ((fd = chk_get_ver_fd(fdxp)) == -1) return(-1);

 /* returns c if success else -1 */
 ival = ungetc(c, __fio_fdtab[fd]->fd_s);
 return(ival);
}

/*
 * check and then convert fd expressions to int
 * return -1 on error else fd number with high bit off (know positive or 0)
 *
 * there is implied truncation to 32 bits so if wider with x's ok
 */
static int32 chk_get_ver_fd(struct expr_t *fdxp)
{
 int32 fd;
 struct xstk_t *xsp;

 xsp = __eval_xpr(fdxp);
 if (xsp->bp[0] != 0L) { errno = EBADF; __pop_xstk(); return(-1); }

 fd = xsp->ap[0] & FIO_FD;
 __pop_xstk();
 if (fd >= FOPEN_MAX) { errno = EBADF; return(-1); }
 /* AIV 06/27/05 - fd cannot be greater than max file size */
 if (fd >= MY_FOPEN_MAX || __fio_fdtab[fd] == NULL) 
  { errno = EBADF; return(-1); } 
 return(fd);
}

/* 
 * get a string from stream with verilog file descripter expr fdxp
 *
 * SJM 09/08/03 - assuming following C lib fgets new line included in string
 */
static int32 fio_fgets(struct expr_t *str_xp, struct expr_t *fdxp)
{
 int32 fd, slen, chlen;
 struct xstk_t *xsp;
 char *lp;
 
 /* result string can't be empty "(, fd)" */
 if (str_xp->optyp == OPEMPTY) { errno = EINVAL; return(0); }

 /* fd is OS file number with high bit anded off - on error OS err num set */
 if ((fd = chk_get_ver_fd(fdxp)) == -1) return(0);
 
 /* len rounds down if not div by 8 following LRM */
 slen = str_xp->szu.xclen/8;
 lp = __my_malloc(slen + 1);
 /* fgets returns ptr to lp or nil not number of read chars */
 if (fgets(lp, slen, __fio_fdtab[fd]->fd_s) == NULL)
  {
   __my_free(lp, slen + 1);
   return(0);
  }

 /* SJM 10/20/03 - think fgets should return nil if at eof but check */ 
 /* DBG remove -- */
 if (*lp == '\0') __misc_terr(__FILE__, __LINE__);
 /* --- */ 

 chlen = strlen(lp);

 xsp = __cstr_to_vval(lp);
 /* now done with read c string must free */
 __my_free(lp, slen + 1);

 /* following verilog convention if not enough chars (EOF) zero fill */ 

 /* 05/16/04 - Verilog strings can't be signed */
 if (xsp->xslen != str_xp->szu.xclen) __sizchgxs(xsp, str_xp->szu.xclen);

 __exec2_proc_assign(str_xp, xsp->ap, xsp->bp);
 __pop_xstk();
 /* notice num chars read my be differ than len of lhs assign to reg */
 return(chlen);
}

/* 
 * rewind within an OS stream - returns -1 on error 0 on success
 *
 * equivalent to C lib fseek(FILE *, 0, SEEK_SET) 
 */
static int32 fio_rewind(struct expr_t *fdxp)
{
 int32 fd;

 /* fd is OS file number with high bit anded off - on error OS err num set */
 if ((fd = chk_get_ver_fd(fdxp)) == -1) return(-1);

 fseek(__fio_fdtab[fd]->fd_s, 0L, SEEK_SET);
 /* returns 0 on success */
 return(0);
}

/* 
 * seek within an OS stream - returns -1 on error and 0 on success
 */
static int32 fio_fseek(struct expr_t *fdxp, struct expr_t *ofs_xp,
 struct expr_t *whence_xp)
{
 int32 fd, offset, whence, seek_typ;
 struct xstk_t *xsp;

 /* fd is OS file number with high bit anded off - on error OS err num set */
 if ((fd = chk_get_ver_fd(fdxp)) == -1) return(-1);

 /* there is an implied convert to 32 bits here */
 xsp = __eval_xpr(ofs_xp);
 if (xsp->bp[0] != 0) { errno = EINVAL; __pop_xstk(); return(-1); }
 /* offset can be negative */
 offset = (int32) xsp->ap[0];
 __pop_xstk();

 /* there is an implied convert to 32 bits here */
 xsp = __eval_xpr(whence_xp);
 if (xsp->bp[0] != 0) { errno = EINVAL; __pop_xstk(); return(-1); }
 /* only 3 possibilities */
 whence = (int32) xsp->ap[0];
 __pop_xstk();
 /* check for legal whence seek type */ 
 if (whence == 0) seek_typ = SEEK_SET;
 else if (whence == 1) seek_typ = SEEK_CUR;
 else if (whence == 2) seek_typ = SEEK_END;
 else { errno = EINVAL; return(-1); }
  
 if (__fio_fdtab[fd] == NULL) { errno = EBADF; return(-1); } 
 fseek(__fio_fdtab[fd]->fd_s, (long) offset, seek_typ);
 /* returns 0 on success */
 return(0);
}

/*
 * get error status - verilog equivalent of strerror function
 * return 0 on no error else set error number and copies err str to str xp
 * 
 * if user passes string narrower than 80 chars, silently truncates err str
 * notice if this has error it overwrites the pending errno error
 *
 * SJM 08/09/03 - although LRM does not say it, returns -1 on error here
 */
static int32 fio_ferror(struct expr_t *fdxp, struct expr_t *str_xp)
{
 int32 fd, rv, stream_err;
 char *cp;
 struct xstk_t *xsp;
  
 /* result string can't be empty "(, fd)" */
 if (str_xp->optyp == OPEMPTY) { errno = EINVAL; return(-1); }
  
 /* fd is OS file number with high bit anded off - on error OS err num set */
 /* notice if fails can change error number */
 if ((fd = chk_get_ver_fd(fdxp)) == -1) return(-1);
 
 if ((stream_err = ferror(__fio_fdtab[fd]->fd_s)) == 0)
  {
   rv = 0; 
err_ret:
   push_xstk_(xsp, str_xp->szu.xclen);
   zero_allbits_(xsp->ap, xsp->xslen);
   zero_allbits_(xsp->bp, xsp->xslen);	
   __exec2_proc_assign(str_xp, xsp->ap, xsp->bp);
   __pop_xstk();
   return(rv);
  }

 /* use the reentrant posix form of str error function */
 /* SJM 01/26/05 - no reason for reentrant version strerr here */
 if ((cp = strerror(stream_err)) == NULL) { rv = -1; goto err_ret; }

 /* know buf ends with '\0' */
 xsp = __cstr_to_vval(cp);
 /* 05/16/04 - Verilog strings can't be signed */
 if (xsp->xslen != str_xp->szu.xclen) __sizchgxs(xsp, str_xp->szu.xclen);

 __exec2_proc_assign(str_xp, xsp->ap, xsp->bp);
 __pop_xstk();
 return(errno);
}

/* 
 * fread data into reg or memory - only 0 or 1 can be read
 * on error 0 else number of 8 bit chars read
 *
 * SJM 09/20/03 - LRM wrong for memories fread can't read addresses
 */
static int32 fio_fread(struct expr_t *ndp)
{
 int32 fd, vlen, nbytes, bufi;
 byte *buf;
 struct expr_t *lhsx, *fdxp, *startxp, *cntxp;
 struct net_t *np;
 struct xstk_t *xsp;

 lhsx = ndp->lu.x;
 /* result string can't be empty "(, fd)" */
 if (lhsx->optyp == OPEMPTY) { errno = EINVAL; return(-1); }

 fdxp = ndp->ru.x->lu.x;
 /* fd is OS file number with high bit anded off - on error OS err num set */
 if ((fd = chk_get_ver_fd(fdxp)) == -1) return(0);

 startxp = cntxp = NULL; 
 if (lhsx->optyp == ID || lhsx->optyp == GLBREF)
  {
   /* for array element, won't be ID */
   np = lhsx->lu.sy->el.enp;
   if (np->n_isarr)
    {
     if ((ndp = ndp->ru.x->ru.x) != NULL)
      {
       startxp = ndp->lu.x;
       if ((ndp = ndp->ru.x) != NULL) cntxp = ndp->lu.x;
      }
     return(fio_arr_fread(lhsx, fd, startxp, cntxp));
    }
  }

 /* case 1: read into reg - start and end args ignored if present */
 /* len rounds down if not div by 8 following LRM */
 /* SJM 09/20/03 LRM says round down but that doesn't make sense */ 
 /* so round up following PLI implementation */
 vlen = (lhsx->szu.xclen + 7)/8;

 /* must store into stack value and then assign */ 
 buf = (byte *) __my_malloc(vlen);

 nbytes = fread(buf, 1, vlen, __fio_fdtab[fd]->fd_s);
 /* if unable to read entire reg, return error and do not assign */
 /* if part read and EOF, correct for reg to not be changed */
 if (nbytes != vlen)
  {
   __my_free((char *) buf, vlen);
   /* for short last section, reg not assigned and bytes in stream read */
   /* returned - user must call ferror or feof system task to find error */
   /* to mimic stdio lib beheavior - if error will probably be 0 */
   return(nbytes);
  }

 push_xstk_(xsp, lhsx->szu.xclen);
 /* 0 value so only need to turn 1 bits on */
 zero_allbits_(xsp->ap, lhsx->szu.xclen);
 zero_allbits_(xsp->bp, lhsx->szu.xclen);
 bufi = vlen - 1;

 fread_onto_stk(xsp, buf, bufi);

 __my_free((char *) buf, vlen);
 /* know xsp width exactly match lhs expr width */  
 __exec2_proc_assign(lhsx, xsp->ap, xsp->bp);
 __pop_xstk();
 return(0);
}

/*
 * read one reg (also used for cell of array)
 * can't fail
 *
 * know correct width location pushed onto x stack that is filled 
 * also know that f read buf value big enough and bufi starts at high end byte
 */
static void fread_onto_stk(struct xstk_t *xsp, byte *buf, int32 bufi)
{
 register int32 bi;
 int32 hbused, hbi, wi, bi2;  
 word32 bitval, bval;

 /* know have char 0/1 value for every bit */
 /* handle partially filled high byte as special case */
 hbused = xsp->xslen % 8;
 bi = xsp->xslen - 1;
 if (hbused != 0)
  {
   bval = (word32) buf[0];
   for (hbi = hbused - 1; hbi >= 0; hbi--, bi--)
    {
     wi = get_wofs_(bi);
     bi2 = get_bofs_(bi);
     bitval = ((bval >> hbi) & 1) << bi2;
     if (bitval != 0) xsp->ap[wi] |= bitval;  
    }
   bufi--;
  }
 /* handle simple all bits in all fread bytes used */
 /* bi correct next high bit to set from read byte */ 
 for (; bufi >= 0; bufi--)
  {
   bval = (word32) buf[bufi];
   for (hbi = 7; hbi >= 0; hbi--, bi--) 
    {
     /* DBG remove -- */
     if (bi < 0) __misc_terr(__FILE__, __LINE__);
     /* --- */
     wi = get_wofs_(bi);
     bi2 = get_bofs_(bi);
     bitval = ((buf[bufi] >> hbi) & 1) << bi2;
     if (bitval != 0) xsp->ap[wi] |= bitval;  
    }
  }
}

/*
 * fread into array (memory)
 *
 * fread of memory differs from read mem because no addresses in file   
 * and can only read non x/z values
 */
static int32 fio_arr_fread(struct expr_t *lhsx, int32 fd,
 struct expr_t *startxp, struct expr_t *cntxp)
{
 register int32 i, arri;
 int32 ri1, ri2, arrwid, starti, cnt, nbytes, tot_bytes, nd_itpop, vlen;
 byte *buf;
 struct net_t *np;
 struct xstk_t *xsp;
 struct gref_t *grp;

 np = lhsx->lu.sy->el.enp;
 __getarr_range(np, &ri1, &ri2, &arrwid);

 /* array elements stored h:0 normalized so index h to high */
 /* but loading is from low to high array words */
 starti = 0;
 cnt = arrwid - 1;

 /* ,,) form possible for start and count expressions */
 if (startxp != NULL) 
  {
   if (startxp->optyp != OPEMPTY) 
    {
     /* can't use comp ndx here because value is just normal expr */
     xsp = __eval_xpr(cntxp);
     if (!vval_is0_(xsp->bp, xsp->xslen) ||
      (xsp->xslen > WBITS && !vval_is0_(&(xsp->ap[1]), xsp->xslen - WBITS)))
      {
       __sgfwarn(588,
        "array $fread of %s start value has x/z bits or wide - low a part used",
        np->nsym->synam);
      }
     arri = (int32) xsp->ap[0];
     /* stsk arg. in Verilog source is actual index - must convert to h:0 */
     starti = normalize_ndx_(arri, ri1, ri2); 
     __pop_xstk();
     if (starti < 0 || starti >= arrwid) 
      {
       errno = EINVAL;
       return(0);
      }
    }
  }
 if (cntxp != NULL)
  {
   if (cntxp->optyp != OPEMPTY)
    {
     /* if count present but not start then use first addr in mem */
     xsp = __eval_xpr(cntxp);
     if (!vval_is0_(xsp->bp, xsp->xslen) ||
      (xsp->xslen > WBITS && !vval_is0_(&(xsp->ap[1]), xsp->xslen - WBITS)))
      {
       __sgfwarn(588,
        "array $fread of %s count value has x/z bits or wide - low a part used",
        np->nsym->synam);
      }
     cnt = (int32) xsp->ap[0];
     if (cnt < 0 || starti + cnt >= arrwid) 
      {
       errno = EINVAL;
       return(0);
      }
    }
  }

 nd_itpop = FALSE;
 if (lhsx->optyp == GLBREF)
  { grp = lhsx->ru.grp; __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
 push_xstk_(xsp, np->nwid);

 /* round up so 1 bit memory still requires 1 byte per cell */
 vlen = (arrwid + 7)/8;
 buf = (byte *) __my_malloc(vlen);
 tot_bytes = 0;
 for (arri = starti, i = 0; i < cnt; i++, arri++)  
  {
   nbytes = fread(buf, 1, vlen, __fio_fdtab[fd]->fd_s);
   tot_bytes += nbytes;
   /* if unable to read entire reg, return error and do not assign */
   /* if part read and EOF, correct for reg to not be changed */
   if (nbytes != vlen) goto done;

   /* 0 value so only need to turn 1 bits on */
   zero_allbits_(xsp->ap, lhsx->szu.xclen);
   zero_allbits_(xsp->bp, lhsx->szu.xclen);

   fread_onto_stk(xsp, buf, vlen - 1);

   /* SJM 03/15/01 - change to fields in net record */
   if (np->nchg_nd_chgstore)
    {
     __chg_st_arr_val(np->nva, arrwid, np->nwid, arri, xsp->ap, xsp->bp);

     /* SJM - 06/25/00 - lhs changed possible from change store */
     /* and must only trigger change for right array index */
     if (__lhs_changed) record_sel_nchg_(np, arri, arri);
    }
   else __st_arr_val(np->nva, arrwid, np->nwid, arri, xsp->ap, xsp->bp);
  }
done:
 __my_free((char *) buf, vlen);
 __pop_xstk();
 if (nd_itpop) __pop_itstk();
 return(tot_bytes);
}
 
/*
 * implement the swrite to string (instead of file) Verilog formatted 
 * print sys tasks
 *
 * easy since because of mcds formatting always goes into c string
 * using the _expr line and cur sofs mechanism
 */
static void fio_swrite(struct expr_t *axp, int32 dflt_fmt)
{
 struct expr_t *str_xp; 
 struct xstk_t *xsp;

 str_xp = axp->lu.x;
 axp = axp->ru.x;
 __str_do_disp(axp, dflt_fmt);
 xsp = __cstr_to_vval(__exprline);

 /* now done with expr line */
 __cur_sofs = 0;

 /* do the assign to string after formatting into __expr line */
 /* following verilog convention if not enough chars (EOF) zero fill */ 

 /* 05/16/04 - Verilog strings can't be signed */
 if (xsp->xslen != str_xp->szu.xclen) __sizchgxs(xsp, str_xp->szu.xclen);

 __exec2_proc_assign(str_xp, xsp->ap, xsp->bp);
 __pop_xstk();
}

/*
 * implement $sformat version of $swrite that has only 1 format but can
 * be variable unlike $swrite
 *
 * first arg is string to write into, 2nd arg is format that can be
 * var so needs to be evaled - rest are the format args - error if
 * too few format args and ignores extra (unlike $swrite that prints
 * them with format)
 */
static void fio_sformat(struct expr_t *axp)
{
 int32 blen, flen;
 struct expr_t *str_xp, *fmt_xp; 
 struct xstk_t *xsp;
 char *fmtstr;

 /* lhs expr to store formatted string into */
 str_xp = axp->lu.x;

 /* evaluate the format into a Verilog string */
 axp = axp->ru.x;
 fmt_xp = axp->lu.x;

 xsp = __eval_xpr(fmt_xp);
 if (!vval_is0_(xsp->bp, xsp->xslen))
  {
   errno = EINVAL;
   return;
  }

 /* trim high 0's of a part only */
 blen = __trim1_0val(xsp->ap, xsp->xslen); 
 if (blen == 0)
  {
   errno = EINVAL;
   return;
  }

 /* this mallocs the input string to scan from */
 fmtstr = __vval_to_vstr(xsp->ap, blen, &flen);
 __pop_xstk();

 /* assuming 8 bit bytes */
 axp = __disp_1fmt_to_exprline(fmtstr, axp);
 if (axp != NULL)
  {
   __sgfwarn(3133,
    "$sformat extra unused arguments after format exhausted");
  }

 __my_free(fmtstr, flen);

 xsp = __cstr_to_vval(__exprline);
 /* now done with expr line */
 __cur_sofs = 0;

 /* do the assign to string after formatting into __expr line */
 /* following verilog convention if not enough chars (EOF) zero fill */ 

 /* 05/16/04 - Verilog strings can't be signed */
 if (xsp->xslen != str_xp->szu.xclen) __sizchgxs(xsp, str_xp->szu.xclen);

 __exec2_proc_assign(str_xp, xsp->ap, xsp->bp);
 __pop_xstk();
}

/*
 * implement $fscanf for now using old scin_s scanf code
 */
static int32 fio_fscanf(struct expr_t *ndp)
{
 int32 fd, blen, flen, rv;
 char *fmtstr;
 struct expr_t *fmt_xp;
 struct xstk_t *xsp;

 /* fd is OS file number with high bit anded off - on error OS err num set */
 /* notice if fails can change error number */
 if ((fd = chk_get_ver_fd(ndp->lu.x)) == -1) return(-1);

 /* know fd in range but if not open error */ 
 if (__fio_fdtab[fd] == NULL) { errno = EBADF; return(-1); }

 ndp = ndp->ru.x;
 fmt_xp = ndp->lu.x;
 xsp = __eval_xpr(fmt_xp);
 if (!vval_is0_(xsp->bp, xsp->xslen))
  {
   errno = EINVAL;
   return(-1);
  }
 /* trim high 0's of a part only */
 blen = __trim1_0val(xsp->ap, xsp->xslen); 
 if (blen == 0)
  {
   errno = EINVAL;
   return(-1);
  }
 fmtstr = __vval_to_vstr(xsp->ap, blen, &flen);
 __fiofp = fmtstr; 

 /* ndp now ptr to comma operator of first arg past fmt */
 ndp = ndp->ru.x;
 rv = fio_exec_scanf(__fio_fdtab[fd]->fd_s, ndp);
 __my_free(fmtstr, flen);
 __pop_xstk();
 return(rv);
}

/*
 * implement $sscanf for now using old scin_s scanf code
 */
static int32 fio_sscanf(struct expr_t *ndp)
{
 int32 blen, slen, flen, rv;
 char *instr, *fmtstr;
 struct expr_t *str_xp, *fmt_xp;
 struct xstk_t *xsp;

 /* first arg is string to read from */
 str_xp = ndp->lu.x;
 xsp = __eval_xpr(str_xp);
 if (!vval_is0_(xsp->bp, xsp->xslen))
  {
   errno = EINVAL;
   return(-1);
  }
 /* trim high 0's of a part only */
 blen = __trim1_0val(xsp->ap, xsp->xslen); 
 if (blen == 0)
  {
   errno = EINVAL;
   return(-1);
  }

 /* this mallocs the input string to scan from */
 instr = __vval_to_vstr(xsp->ap, blen, &slen);
 /* implied global used for reading input char by char */ 
 __fiolp = instr; 
 __pop_xstk();

 ndp = ndp->ru.x;
 fmt_xp = ndp->lu.x;
 xsp = __eval_xpr(fmt_xp);
 if (!vval_is0_(xsp->bp, xsp->xslen))
  {
   errno = EINVAL;
   return(-1);
  }
 /* trim high 0's of a part only */
 blen = __trim1_0val(xsp->ap, xsp->xslen); 
 if (blen == 0)
  {
   errno = EINVAL;
   return(-1);
  }

 /* this mallocs the fmt string to scan from */
 /* notice for new routines (especially input), only one format string */
 fmtstr = __vval_to_vstr(xsp->ap, blen, &flen);
 __fiofp = fmtstr; 
 __pop_xstk();

 /* AIV 09/29/03 - forgot to assign ndp to first arg past format */
 ndp = ndp->ru.x;

 rv = fio_exec_scanf(NULL, ndp);
 /* free the version in malloced storage */
 __my_free(instr, slen);
 __my_free(fmtstr, flen);
 return(rv);
}

/* SJM 09/24/03 - why need defines here? */
/* SJM 09/24/03 - eliminated EOL since no significance of new line now */
#define infmt() ((*__fiofp == '\0') ? EOF : *__fiofp++)

/*
 * execute the scan input system task 
 * returns number of successfully read items 
 *
 * on entry axp points to arg list comma operator of first scan into arg
 * if f nil, gets char from global file io work char ptr else read char
 *
 * know globals __fiolp points to string (for sscanf) and __fiofp always
 * points to first char in format string on entry
 *
 * SJM 09/24/03 - seems that unlike c lib no \ escaping of % allowed - true?
 * now must check format syntax correctness here because format can be var 
 */
static int32 fio_exec_scanf(FILE *f, struct expr_t *axp)
{
 register char *wchp;
 register int32 c, fch, width;
 int32 len, num_matched, base, signc, ival;
 int32 retval, assgn_sup, stval, sav_sofs, lmatch; 
 double d1;
 struct expr_t *lhsx; 
 struct xstk_t *xsp;
 struct task_t *tskp;

 /* if F, illegal format so return EOF */ 
 if (!chk_scanf_fmt(__fiofp))
  {
   errno = EINVAL;
   return(-1);
  }

 /* start by readin first input char - may push back */
 /* axp always points to comma operator of next arg (maybe nil) */
 retval = -1;

 /* if EOF on input file before any matches return EOF */
 if ((c = scanf_getc(f)) == EOF) return(-1);
 for (lmatch = num_matched = 0;;)
  {
   if (f != NULL && lmatch != num_matched)
    {
     if ((__scanf_pos = ftell(f)) == -1) return(-1);
    }
   lmatch = num_matched;

   /* at beginning of loop c is next input line char to process */
   /* but fch is last processed format char */
   if ((fch = infmt()) == EOF) break; 
   if (fch != '%')
    {
     /* fmt white space matches optional any width input line white space */
     if (isspace(fch))
      {
       while(isspace(c))
        {
         if ((c = scanf_getc(f)) == EOF) break;
        }
       if (c == EOF) break;
       continue;
      }
     else if (c == fch)
      {
       c = scanf_getc(f);
       continue;
      }
     /* mismatched input char - finished ret count of assigned */
     retval = 0;  
     break;
    }
   if ((fch = infmt()) == EOF) break;
   /* check for format suppress char */
   if (fch == '*') 
    {
     assgn_sup = TRUE;
     if ((fch = infmt()) == EOF) break; 
    }
   else assgn_sup = FALSE;

   /* find maximum field width */
   width = 0;
   while (isdigit(fch))
    {
     width *= 10;
     width += fch - '0';
     if ((fch = infmt()) == EOF) goto done;
    }
   if (width == 0) width = -1;

   /* LOOKATME - possible portability problem since isspace of */ 
   /* special -2 may or may be space - checking both */
   /* consume input line white space unless special c format */
   if (fch != 'c')
    {
     while (isspace(c)) { if ((c = scanf_getc(f)) == EOF) break; }
    }

   retval = 0;
   switch (fch) {
    case '%':
     /* %% matches % - i.e. it is % escaping mechanism */
     if (c != '%') goto done; 
     break;
    case 'd': base = BDEC; goto do_num;
    case 'b': base = BBIN; goto do_num;
    case 'o': base = BOCT; goto do_num;
    case 'x': case 'h': base = BHEX;
do_num: 
     /* return F if no characters collected */
     if (!collect_scanf_num(&signc, f, c, base, width)) goto done;
     if (!assgn_sup)
      {
       num_matched++; 
       if (axp == NULL) goto done;
       lhsx = axp->lu.x;

       /* also convert into ac/bc wrk globals */
       /* conversion requires knowing arg expr width - can only do here */
       __itoklen = lhsx->szu.xclen;
       /* converted number converted to exactly lhs expr size */ 
       /* use expr. width as imputed [num]' form - always succeeds */
       __to_dhboval(base, FALSE);

       /* try to correct value for minus sign */
       if (signc == '-')
        {
         /* SJM 05/14/04 - must handle any width signed */
         if (vval_is0_(__bcwrk, __itoklen))
          {
           if (__itoklen == WBITS)
            { ival = (int32) __acwrk[0]; __acwrk[0] = (word32) -ival; }
           else __inplace_lnegate(__acwrk, __itoklen);
          }
        }

       /* know __acwrk and _bcwrk have right width number */
       if (lhsx->optyp != OPEMPTY)
        {
         __exec2_proc_assign(lhsx, __acwrk, __bcwrk);
        }
       /* know c has 1 char after number */
       axp = axp->ru.x;
      }
     break;
    case 'f': case 'e': case 'g': case 't':
     if (!collect_scanf_realnum(&(d1), f, c, width, fch)) goto done;
     if (!assgn_sup)
      {
       if (axp == NULL) goto done;
       lhsx = axp->lu.x;
       axp = axp->ru.x;
       num_matched++; 
       if (lhsx->optyp != OPEMPTY)
        {
         push_xstk_(xsp, WBITS);
         memcpy(xsp->ap, &d1, sizeof(double));
         if (!lhsx->is_real)
          {
           __cnv_stk_fromreal_toreg32(xsp);

           /* SJM 09/29/03 - chg to handle sign extend and separate types */
           if (xsp->xslen > lhsx->szu.xclen)
            __narrow_sizchg(xsp, lhsx->szu.xclen);
           else if (xsp->xslen < lhsx->szu.xclen)
            {
             /* know always signed */
             __sgn_xtnd_widen(xsp, lhsx->szu.xclen);
            }
          }

         __exec2_proc_assign(lhsx, xsp->ap, xsp->bp);
         __pop_xstk();
        }
      }
     /* know c has 1 char after number */
     break;
    case 'v':
     wchp = __numtoken; 
     /* know there is always look ahead char */ 
     wchp[0] = c;
     if ((c = scanf_getc(f)) == EOF) goto done;
     wchp[1] = c;
     if ((c = scanf_getc(f)) == EOF) goto done;
     wchp[2] = c;
     wchp[3] = '\0';
     if ((stval = cnvt_scanf_stnam_to_val(wchp)) == 0) goto done;
     if (!assgn_sup)
      {
       lhsx = axp->lu.x;
       if (lhsx->optyp != OPEMPTY)
        {
         push_xstk_(xsp, 1);
         /* SJM 09/25/03 - since can only assign to reg remove stren */ 
         xsp->ap[0] = stval & 1;
         xsp->bp[0] = (stval >> 1) & 1;

         /* SJM 09/29/03 - since 1 bit rhs never signed - can only widen */
         if (xsp->xslen != lhsx->szu.xclen)
          __sizchg_widen(xsp, lhsx->szu.xclen);

         __exec2_proc_assign(lhsx, xsp->ap, xsp->bp);
         __pop_xstk();
        }
      }
     break;
    case 'c':
     /* SJM 09/24/03 - old sscanf multiple chars removed - now 1 char only */
     /* SJM 03/20/00 - know never need to grow num token - f get used */
     /* notice unlike clib, width can't be used for "string" of chars */
     /* SJM 05/14/04 - know the char already read and in c */
     /* must be one char left on input stream */
     if (c == EOF) goto done;
     wchp = __numtoken;
     wchp[0] = (byte) c;
     wchp[1] = '\0';
     len = 1;
     /* if past end of formats, no more assignments to do */
     /* convert to pascal style string as Verilog value on stack */
do_str_assign:
     /* DBG remove ---
     if (__debug_flg)
      { __dbg_msg("read string [%s]\n", __numtoken); }
     --- */
     if (!assgn_sup)
      {
       num_matched++; 
       if (axp == NULL) goto done;
       lhsx = axp->lu.x;
       axp = axp->ru.x;
       push_xstk_(xsp, 8*len);
       zero_allbits_(xsp->bp, xsp->xslen);
       __vstr_to_vval(xsp->ap, __numtoken, 8*len);
       if (lhsx->optyp != OPEMPTY)
        {
         /* size chg never needs sign extend - can narrow or widen */ 
         if (xsp->xslen != lhsx->szu.xclen) __sizchgxs(xsp, lhsx->szu.xclen);

         __exec2_proc_assign(lhsx, xsp->ap, xsp->bp);
        }
       __pop_xstk();
       /* c has next char to process */ 
      }
     break;
    case 's': 
     /* s is for white space delimited strings */
     wchp = __numtoken;
     /* notice works since added \0 to end of input line */
     /* i.e. empty string ok */ 
     for (len = 0;;)
      {
       *wchp++ = c;
       len++;
       c = scanf_getc(f);
       if (width > 0 && --width == 0) break;
       if (c == EOF || isspace(c)) break;
      }
     *wchp = '\0'; 
     goto do_str_assign;
    case 'u':
     /* no way to detemine num words to read */
     if (assgn_sup)
      {
       __sgferr(3417,
        "scanf assignment suppression character illegal with %%u binary data format");
       errno = EINVAL;
       return(-1);
      }
     if (width != -1)
      {
       __sgfinform(3008,
        "scanf field width meaningless with %%u binary data format - width ignored");
      }      
     if (axp == NULL) goto done;
     lhsx = axp->lu.x;
     xsp = collect_ufmt_binval(f, lhsx, c);
     if (xsp == NULL) goto done;
     axp = axp->ru.x;
     /* since use scanf assign to reg arg for size - never need size chg */
     __exec2_proc_assign(lhsx, xsp->ap, xsp->bp);
     __pop_xstk();
     break;
    case 'z':
     /* no way to detemine num words to read */
     if (assgn_sup)
      {
       __sgferr(3417,
        "scanf assignment suppression character illegal with %%z binary data format");
       errno = EINVAL;
       return(-1);
      }
     if (width != -1)
      {
       __sgfinform(3008,
        "scanf field width meaningless with %%z binary data format - width ignored");
      }      
     if (axp == NULL) goto done;
     lhsx = axp->lu.x;
     axp = axp->ru.x;
     xsp = collect_zfmt_binval(f, lhsx, c);
     if (xsp == NULL) goto done;

     /* since use scanf assign to reg arg for size - never need size chg */
     __exec2_proc_assign(lhsx, xsp->ap, xsp->bp);
     __pop_xstk();
     break;
    case 'm':
     sav_sofs = __cur_sofs;
     if (__cur_thd == NULL) tskp = __scope_tskp; 
     else tskp = __getcur_scope_tsk();
     __disp_itree_path(__inst_ptr, tskp);
     /* use optional width field to truncate */
     if (width > 1 && (__cur_sofs - sav_sofs) > width)
      {
       __cur_sofs = sav_sofs + width;
       __exprline[__cur_sofs] = '\0';
      }
     len = __cur_sofs - sav_sofs;
     if (!assgn_sup)
      {
       num_matched++; 
       if (axp == NULL) { __cur_sofs = sav_sofs; goto done; }

       lhsx = axp->lu.x;
       axp = axp->ru.x;
       push_xstk_(xsp, 8*len);
       zero_allbits_(xsp->bp, xsp->xslen);
       __vstr_to_vval(xsp->ap, __exprline, 8*len);
       if (lhsx->optyp != OPEMPTY)
        {
         /* widening adds high 0 bits */
         /* again can narrow or widen and neve need sign extend */
         if (xsp->xslen != lhsx->szu.xclen) __sizchgxs(xsp, lhsx->szu.xclen);

         __exec2_proc_assign(lhsx, xsp->ap, xsp->bp);
        }
       __pop_xstk();
      }
     /* c has next char to process */ 
     continue;
    default:
     /* invalid format char after % */
     errno = EINVAL;
     goto done; 
   }
   /* SJM 05/14/04 - top of loop expects one ahead */
   c = scanf_getc(f);
  }
 /* good exit from scan processing of fmt - but still read one too far */  
 scanf_ungetc(c, f);

 /* this is nothing read case */
 if (retval == -1) return(-1);
 return(num_matched);

done:
 if (f != NULL && __scanf_pos > 0)
  {
   if (fseek(f, __scanf_pos - 1, SEEK_SET) == -1) return(-1);
  }
 /* this is nothing read case */
 if (retval == -1) return(-1);
 return(num_matched);
}

/*
 * version of getc that read from passed scanf input file or buf for sscanf
 */
static int32 scanf_getc(FILE *f)
{
 int32 c;

 if (f == NULL)
  {
   if (*__fiolp == '\0') return(EOF);
   return(*__fiolp++);
  }
 c = fgetc(f); 
 return(c); 
}

/*
 * version of ungetc that backup up buffer for string file io operations 
 * BEWARE - can't call ungetc unless something read
 */
static void scanf_ungetc(int32 c, FILE *f)
{
 if (f == NULL) __fiolp--; else ungetc(c, f);
}

/*
 * check new fileio $fscanf or $sscanf format string
 *
 * SJM 09/24/03 - now must check at run time each time called because
 * format can be variable - also only check fmt not arg matching
 *
 * LOOKATME - could check only once for constant fmt string
 */
static int32 chk_scanf_fmt(char *fmt)
{
 register char *fp;
 int32 fmt_pos, rv, has_width;

 rv = TRUE;
 fp = fmt;
 fmt_pos = 0;
 while (*fp != '\0')
  {
   /* just char in fmt to match */
   if (*fp++ != '%') continue;
  
   /* %% is way to match % in input */ 
   /* SJM 09/24/03 - assuming %[* and/or width digs]% illegal */
   if (*fp == '%') { fp++; continue; }

   /* assign suppress char legal */
   if (*fp == '*') fp++;

   /* possible %ddd[fmt letter] */
   has_width = FALSE;
   while (isdigit(*fp))
    {
     fp++;
     if (*fp == '\0')
      {
       __sgferr(1186,
         "end of format while reading maximum field width (pos. %d)",
        fmt_pos);
       rv = FALSE;
       goto done;
      }
     has_width = TRUE;
    }
   fmt_pos++;
   /* string formats must be multiple of 8 bits */
   switch (*fp) {
    case 'b': case 'o': case 'h': case 'x': case 'd':
     break;
    case 'f': case 'e': case 'g':
     break;
    case 'v':
     break;
    case 't':
     break;
    case 'c':
     if (has_width)
      {
       __sgfwarn(3104,
        "maximum field width used with %%c format (pos. %d) - width ignored",
        fmt_pos);
      }
     break;
    case 's':
     break;
    case 'u':
     break;
    case 'z':
     break;
    case 'm':
     break;
    default:
     __sgferr(1274,
      "$scanf %%%c (pos. %d) is not legal FILE IO format letter",
      *fp, fmt_pos); 
     rv = FALSE;
   }
   fp++;
  }
done:
 return(rv);
}

/*
 * collect a dhbo number from input into num token global 
 * returns F on error - if so num token invalid 
 */
static int32 collect_scanf_num(int32 *signc, FILE *f, int32 c, int32 base, int32 width)
{
 register char *wchp;

 wchp = __numtoken; 
 /* collect number */
 if (c == '-' || c == '+')
  {
   /* minus only legal for %d (and real) format(s) */
   if (base != BDEC)
    {
     __sgfinform(3008,
      "numeric non decimal (%%d) scanf format illegally begins with sign");
     return(FALSE);
    } 
   *signc = c;
   if (width > 0) --width;
   c = scanf_getc(f);
  }
 else *signc = ' ';

 for (;;)
  {
   if (isspace(c) || c == EOF) break;
   /* skip _ space holder */
   if (c != '_')
    {
     /* non number char ends number if not ended with white space */
     if ((c = __is_vdigit(c, base)) < 0) break;

     if (base == BDEC)
      {
       /* SJM 05/14/04 - decimal format ended by non beginning xz, i.e. */ 
       /* it is assumed to be start of a string */
       if ((c == 'x' || c == 'z') && wchp != __numtoken) break;  
      }
     *wchp++ = c;
    }
   c = scanf_getc(f);
   if (width > 0 && --width == 0) break;
  }
 *wchp = '\0';
 if (wchp == __numtoken) return(FALSE);
 /* SJM 05/14/04 - if ends with non white space, need to start reading with */ 
 /* ending char */
 if (!isspace(c) && c != EOF) scanf_ungetc(c, f);

 return(TRUE);
}

/*
 * collect a ral (f,g, e, and t) real number from input into num token global 
 * returns F on error - if so dret not changed
 */
static int32 collect_scanf_realnum(double *dret, FILE *f, int32 c, int32 width,
 int32 fch)
{
 register char *wchp;
 double d1;
 int32 got_dot, got_e, signc, unit, errnum;
 char *endp;

 /* collect the string */
 wchp = __numtoken; 
 if (c == '-' || c == '+')
  { signc = c; if (width > 0) --width; c = scanf_getc(f); }
 else signc = ' ';
 got_dot = got_e = 0;
 for (;;)
  {
   if (isdigit(c)) *wchp++ = c;
   else if (got_e && wchp[-1] == 'e' && (c == '-' || c == '+'))
    *wchp++ = c;
   else if (!got_e && (c == 'e' || c == 'E'))
    { *wchp++ = 'e'; got_e = got_dot = 1; }
   else if (c == '.' && !got_dot) { *wchp++ = c; got_dot = 1; }
   else break;

   if ((c = scanf_getc(f)) == EOF) break;
   if (width > 0 && --width == 0) break;
  }
 *wchp = '\0';
 /* terminate if no characters collected */
 if (wchp == __numtoken) return(FALSE);

 /* SJM 05/14/04 - if ends with non white space, need to start reading with */ 
 /* ending char */
 if (!isspace(c) && c != EOF) scanf_ungetc(c, f);

 d1 = __my_strtod(__numtoken, &endp, &errnum);   
 if (errnum != 0 || *endp != '\0') return(FALSE);
 if (signc == '-') d1 = -d1;

 /* SJM 09/24/03 - LOOKATME - maybe should only do if not suppresed */
 if (fch == 't')
  {
   /* t format same as real except need to scale time format */ 
   if (__inst_mod->mtime_units != __tfmt_units)
    {
     if (__inst_mod->mtime_units > __tfmt_units)
      {
       /* here d1 module ticks higher exp (more precision) - divide */  
       unit = __inst_mod->mtime_units - __tfmt_units;
       d1 /= __dbl_toticks_tab[unit];
      }   
     else
      {
       /* here d1 module ticks lower (less precision) - multiply */
       unit = __tfmt_units - __inst_mod->mtime_units;
       d1 *= __dbl_toticks_tab[unit];
      }
    }
  }
 *dret = d1;
 return(TRUE);
}

/*
 * collect 'u' format binary 0/1 one word32 values onto top of pushed xstk
 */
static struct xstk_t *collect_ufmt_binval(FILE *f, struct expr_t *lhsx, int32 c)
{
 register int32 wi;
 register word32 wrd;
 int32 b1, b2, b3, b4;
 struct xstk_t *xsp;

 push_xstk_(xsp, lhsx->szu.xclen);
 /* b part always 0 */
 zero_allbits_(xsp->bp, xsp->xslen);
 for (wi = 0; wi < wlen_(lhsx->szu.xclen); wi++)
  {
   b1 = (word32) c;
   if ((c = scanf_getc(f)) == EOF) return(NULL);
   b2 = (word32) c;
   if ((c = scanf_getc(f)) == EOF) return(NULL);
   b3 = (word32) c;
   if ((c = scanf_getc(f)) == EOF) return(NULL);
   b4 = (word32) c;
#if (BYTE_ORDER == BIG_ENDIAN)
   wrd = (b1 & 0xff) | ((b2 & 0xff) << 8) | ((b3 & 0xff) << 16)
    | ((b4 & 0xff) << 24);
#else
   wrd = (b4 & 0xff) | ((b3 & 0xff) << 8) | ((b2 & 0xff) << 16)
    | ((b1 & 0xff) << 24);
#endif
   xsp->ap[wi] = wrd;
  }
 return(xsp);
}

/*
 * collect 'z' format binary 4 value 2 word32 values onto top of pushed xstk
 */
static struct xstk_t *collect_zfmt_binval(FILE *f, struct expr_t *lhsx,
 int32 c2)
{
 register int32 wi;
 register word32 wrd, wrd2;
 int32 c, b1, b2, b3, b4;
 struct xstk_t *xsp;

 push_xstk_(xsp, lhsx->szu.xclen);
 for (wi = 0; wi < wlen_(lhsx->szu.xclen); wi++)
  {
   /* binary format by convention is a/b pairs following PLI t_vecval */
   b1 = (word32) c2;
   if ((c = scanf_getc(f)) == EOF) return(NULL);
   b2 = (word32) c;
   if ((c = scanf_getc(f)) == EOF) return(NULL);
   b3 = (word32) c;
   if ((c = scanf_getc(f)) == EOF) return(NULL);
   b4 = (word32) c;
#if (BYTE_ORDER == BIG_ENDIAN)
   wrd = (b1 & 0xff) | ((b2 & 0xff) << 8) | ((b3 & 0xff) << 16)
    | ((b4 & 0xff) << 24);
#else
   wrd = (b4 & 0xff) | ((b3 & 0xff) << 8) | ((b2 & 0xff) << 16)
    | ((b1 & 0xff) << 24);
#endif
   xsp->ap[wi] = wrd;

   if ((c = scanf_getc(f)) == EOF) return(NULL);
   b1 = (word32) c;
   if ((c = scanf_getc(f)) == EOF) return(NULL);
   b2 = (word32) c;
   if ((c = scanf_getc(f)) == EOF) return(NULL);
   b3 = (word32) c;
   if ((c = scanf_getc(f)) == EOF) return(NULL);
   b4 = (word32) c;
#if (BYTE_ORDER == BIG_ENDIAN)
   wrd2 = (b1 & 0xff) | ((b2 & 0xff) << 8) | ((b3 & 0xff) << 16)
    | ((b4 & 0xff) << 24);
#else
   wrd2 = (b4 & 0xff) | ((b3 & 0xff) << 8) | ((b2 & 0xff) << 16)
    | ((b1 & 0xff) << 24);
#endif
   xsp->bp[wi] = wrd2;
  }
 return(xsp);
}

/*
 * convert scanf stren name to one byte stren value
 * return byte value or 0 (impossible stren) on error
 *
 * since scanf can only assign to regs, stren has stren removed
 * but must check for legal and then remove stren
 */
static int32 cnvt_scanf_stnam_to_val(char *s) 
{
 int32 stval, st0, st1;
 char val;
 char stren[RECLEN];

 if (strcmp(s, "HiZ") == 0) return(2);

 val = s[2];
 strncpy(stren, s, 2);  
 stren[2] = '\0';
 if (strcmp(stren, "Su") == 0) st0 = st1 = ST_SUPPLY;
 else if (strcmp(stren, "St") == 0) st0 = st1 = ST_STRONG;
 else if (strcmp(stren, "Pu") == 0) st0 = st1 = ST_PULL;
 else if (strcmp(stren, "La") == 0) st0 = st1 = ST_LARGE;
 else if (strcmp(stren, "We") == 0) st0 = st1 = ST_WEAK;
 else if (strcmp(stren, "Me") == 0) st0 = st1 = ST_MEDIUM;
 else if (strcmp(stren, "Sm") == 0) st0 = st1 = ST_SMALL;
 else return(-1);

 /* only use of Z aready eliminated */
 /* notice st1 and st0 must be same here */
 switch(val) {
  case '0':
   stval = 0 | (st1 << 2) | (st0 << 5);
   break; 
  case '1':
   stval = 1 | (st1 << 2) | (st0 << 5);
   break;
  case 'X':
   stval = 3 | (st1 << 2) | (st0 << 5);
   break;
  case 'L':
   stval = 3 | (st0 << 5); 
   break;
  case 'H':
   stval = 3 | (st1 << 2); 
   break;
  default: return(0);
 } 
 return(stval);
}

