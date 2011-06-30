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
#include <ctype.h>

#include <sys/types.h>

#if defined(__SVR4) || defined(__hpux) || defined(__CYGWIN32__)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif

#ifdef __DBMALLOC__
#include "../malloc.h"
#endif

#include "v.h"
#include "cvmacros.h"

/* local prototypes */
static void resolve_undef_mods(void);
static void process_lib(void);
static void resolve_from_ydir(struct vylib_t *);
static struct undef_t *add_undef_el(struct sy_t *, struct undef_t **,
 struct undef_t **);
static void bld_ylb_dirfiles(struct vylib_t *);
static int32 srch_yfiles(char *, struct mydir_t *, word32);
static int32 fn_cmp(const void *, const void *);
static void rd_vlibfil(struct undef_t *);
static void rescan_process_lib(void);
static void rescan_resolve_from_ydir(struct vylib_t *, struct undef_t *);
static void free_tnblks(void);
static void sep_mdgates(void);
static int32 count_minum_and_mgnum(int32 *, struct mod_t *);
static void cellrep_to_gate(struct cell_t *, struct gate_t *);
static void add_mod_attrs_toinst(struct inst_t *);
static int32 pndparams_explicit(struct namparam_t *, int32 *);
static struct paramlst_t *bld_gate_paramlst(struct namparam_t *);
static void bld_giarr(struct mod_t *, int32, struct cell_t *, int32);
static void init_giarr(struct giarr_t *);
static int32 cnt_gateprts(struct cell_t *);
static void cellrep_to_inst(struct cell_t *, struct inst_t *);
static void init_inst(struct inst_t *);
static struct expr_t **inst_nparms_to_xtab(struct namparam_t *,
 struct inst_t *);
static struct expr_t **match_namparam_exprtab(struct namparam_t *,
 struct inst_t *);
static int32 chk1_pndparam(char *, struct inst_t *, struct namparam_t *,
 int32, int32);
static int32 prmtab_cmp(const void *, const void *);
static struct expr_t **match_implprm_exprtab(struct namparam_t *,
 struct inst_t *);
static void bld_miarr(struct mod_t *, int32, struct cell_t *, int32);
static int32 fix_modcell_nl(void);
static int32 count_static_instances(struct mod_t *);
static int32 chk_dag(void);
static void fix_port_conns(void);
static void setchk_modports(void);
static int32 chk_prtref(struct expr_t *, struct mod_pin_t *, int32);
static int32 chk_prtntyp(struct mod_pin_t *, struct net_t *);
static void conn_mod_insts(void);
static void conn_impl_mports(struct inst_t *, struct cell_pin_t *,
 struct mod_t *, struct expr_t **, struct srcloc_t *);
static void conn_expl_mports(struct inst_t *, struct cell_pin_t *,
 struct mod_t *, struct expr_t **, struct srcloc_t *);
static int32 chk_mdports_named(struct inst_t *, struct mod_t *, struct expr_t **);
static void bld_srted_mdpins(struct mod_t *);
static int32 smp_cmp(const void *, const void *);
static int32 bld_srted_ipins(struct inst_t *, register struct cell_pin_t *,
 int32, struct cell_pin_t **);
static int32 cpn_cmp(const void *, const void *);
static void free_cpblks(void);
static void free_cppblks(void);
static void count_flat_insts(void);
static void count2_flat_insts(struct mod_t *);
static void chg_params_to_tab(void);
static int32 cnt_prms(struct net_t *);
static void free_param_listform(struct net_t *);
static void do_giarr_splitting(void);
static void mark_gia_rng_params(struct mod_t *);
static int32 in_giarng_markparam(struct giarr_t *, struct expr_t *);
static int32 down_hasgiarngdet_param(struct mod_t *);
static void bld_top_virtinsts(void);
static int32 topip_cmp(const void *, const void *);
static void save_all_param_vals(void);
static void free_all_param_vals(void);
static void set_giarr_ranges(void);
static void eval1_arr_of_gates_rng(struct giarr_t *, struct mod_t *,
 struct itree_t *, int32);
static int32 gi_ndxexpr_chk(struct expr_t *, int32, int32, char *); 
static int32 gi_ndxval_chk(struct xstk_t *, int32, int32, char *);
static void eval1_arr_of_insts_rng(struct giarr_t *, struct mod_t *,
 struct itree_t *, int32);
static void set_pnd_gi_rnges(void);
static void set2_pnd_gi_rnges(struct mod_t *, struct itree_t *);
static void set_gia_expr_pndparms(struct expr_t *, struct itree_t *);
static void set1_giarr_pndparm(struct net_t *, struct itree_t *); 
static void unsave_gia_expr_pndparms(struct expr_t *, struct itree_t *);
static void rebld_mod_giarrs(void);
static int32 gia_sym_cmp(const void *, const void *);
static void add_new_gsym(struct gate_t *, int32);  
static void add_new_isym(struct inst_t *, int32); 
static void chk_defparams(void);
static int32 lhs_chk1dfparam(struct dfparam_t *);
static int32 gref_has_giarr_ndxes(struct gref_t *);
static void set_1defparam_iis(struct dfparam_t *, struct gref_t *);
static void resolve_xmrs(void);
static int32 resolve_local_path(struct gref_t *, struct expr_t *);
static int32 chk_xmr_tail_wire(struct gref_t *, struct sy_t *, int32);
static int32 fill_dfp_gsymhead(struct gref_t *, struct expr_t *);
static int32 fill_gsymhead(struct gref_t *, struct expr_t *);
static int32 chk_all_uprels_same(struct gref_t *, struct sy_t *, char *,
 struct expr_t *);
static struct sy_t *fnd_uprel_inst(char *, struct itree_t *);
static int32 fnd_uprel_mod(struct sy_t *, struct itree_t *);
static struct mod_t *fnd_uprel_tskfunc(struct sy_t **, char *,
 struct itree_t *);
static struct sy_t *fnd_tskfunc_inscope(char *, struct symtab_t *);
static int32 fill_grestsyms(struct gref_t *, struct expr_t *);
static struct sy_t *find_inmod_sym(struct gref_t *, struct expr_t *,
 struct sy_t *, struct symtab_t **);
static int32 chk_glb_inst_sels(struct gref_t *);
static void free_gone_glbs(void);
static void mark_poundparam_splitinsts(void);
static int32 indir_widthdet_markparam(struct expr_t *);
static void do_poundparam_splitting(void);
static void split_upd_mod(struct mod_t *, struct inst_t *, int32); 
static void copy_mod(struct mod_t *, char *);
static void copy_modsymtabs(void);
static void copy_lowsymtab(register struct symtab_t *, struct symtab_t *);
static struct symtab_t *copy_1symtab(struct symtab_t *);
static struct sy_t **copy_stsyms(struct sy_t **, word32);
static void copy_modports(void);
static void copy_wires(struct symtab_t *);
static struct net_t *copy_params(struct net_t *, int32, int32);
static void copy_defparams(void);
static void copy_insts(void);
static struct attr_t *copy_attrs(struct attr_t *);
static struct varinitlst_t *copy_varinits(struct varinitlst_t *);
static void copy_1inst(struct inst_t *, struct inst_t *, int32);
static struct expr_t **copy_pndxtab(struct inst_t *);
static void copy_iports(struct inst_t *, struct inst_t *);
static void copy_miarr(void);
static void copy_gates(void);
static void copy_1gate(struct gate_t *, struct gate_t *, int32);
static void copy_mgarr(void);
static void copy_contas(void);
static void copy_mdtasks(void);
static struct task_pin_t *copy_tskargs(struct task_t *);
static struct st_t *copy_stmt(struct st_t *);
static struct st_t *copy_lstofsts(register struct st_t *);
static struct ialst_t *copy_ialst(register struct ialst_t *);
static struct csitem_t *copy_csitemlst(register struct csitem_t *);
static struct exprlst_t *copy_xprlst(struct exprlst_t *);
static void copy_mgrefs(void);
static void copy_1gref_flds(struct gref_t *, struct gref_t *);
static void copy_specify(void);
static void copy_spcpths(struct spfy_t *, struct spfy_t *);
static void copy_timchks(struct spfy_t *, struct spfy_t *);
static struct tchk_t *copy1_tchk(struct tchk_t *);
static void bld2_flat_itree(struct itree_t *);
static int32 dmp_down_itree(struct itree_t *, int32, int32);
static void do_dmp(struct itree_t *, int32);
static void free2_flat_itree(struct itree_t *);
static void bld_moditps(void);
static void bld2_itnum_to_itp(struct itree_t *);


/* extern prototypes (maybe defined in this module) */
extern char *__my_malloc(int32);
extern char *__my_realloc(char *, int32, int32);
extern char *__pv_stralloc(char *);
extern struct ncomp_t *__alloc_arrncomp(void);
extern char *__prt_vtok(void);
extern struct xstk_t *__eval2_xpr(struct expr_t *);
extern int32 __isleaf(struct expr_t *);
extern int32 __wide_vval_is0(register word32 *, int32);
extern struct expr_t *__alloc_newxnd(void);
extern struct exprlst_t *__alloc_xprlst(void);
extern char *__to_ptnam(char *, word32);
extern char *__to_wtnam(char *, struct net_t *);
extern char *__to_sytyp(char *, word32);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern struct expr_t *__copy_expr(struct expr_t *);
extern void __bld_flat_itree(void);
extern void __free_flat_itree(void);
/* DBG */
extern void __dmp_itree(struct itree_t *); 
/* --- */
extern struct sy_t *__zget_sym(char *, struct sy_t **, word32);
extern struct sy_t *__add_modsym(char *);
extern struct sy_t *__get_sym(char *, struct symtab_t *);
extern struct symtab_t *__alloc_symtab(int32);
extern struct paramlst_t *__alloc_pval(void);
extern struct delctrl_t *__alloc_dctrl(void);
extern struct csitem_t *__alloc_csitem(void);
extern struct paramlst_t *__copy_dellst(struct paramlst_t *);
extern char *__bld_lineloc(char *, word32, int32);
extern void __process_defparams(void);
extern void __set_poundparams(void);
extern void __process_timescales(void);
extern void __recalc_param_vals(void);
extern void __chk_1mdecls(void);
extern void __mark_st_wires(void);
extern void __setchk_mpwidths(void);
extern void __chk_shorted_bids(void);
extern void __reconn_gia_pins(void);
extern void __reset_modport_strens(void);
extern void __prt_top_mods(void);
extern void __chk_mod(void);
extern void __free_icptab(void);
extern void __chkfix_spfy(void);
extern void __free_specify(struct mod_t *);
extern void __free_tchks(struct tchk_t *);
extern void __emit_param_informs(void);
extern void __grow_infils(int32);
extern void __my_free(char *, int32);
extern void __get_vtok(void);
extern void __process_cdir(void);
extern int32 __vskipto_modend(int32);
extern int32 __vskipto2_modend(int32, int32);
extern int32 __rd_moddef(struct symtab_t *, int32);
extern int32 __rd_udpdef(struct symtab_t *);
extern void __my_rewind(FILE *);
extern void __my_fclose(FILE *);
extern FILE *__tilde_fopen(char *, char *);
extern void __free_xtree(struct expr_t *);
extern void __free2_xtree(struct expr_t *);
extern void __bld_unc_expr(void);
extern void __in_xpr_markparam(struct expr_t *);
extern int32 __chk_paramexpr(struct expr_t *, int32);
extern void __set_numval(struct expr_t *, word32, word32, int32);
extern void __free_1dfparam(struct dfparam_t *);
extern struct gref_t *__alloc_grtab(struct gref_t *, int32);
extern void __resolve_glbnam(struct gref_t *);
extern int32 __ip_indsrch(char *);
extern void __free_1glb_flds(struct gref_t *);
extern void __bld_mlevel_lists(void);
extern void __mark_widdet_params(struct mod_t *);
extern void __do_mdsplit(struct mod_t *);
extern void __init_itree_node(struct itree_t *);
extern int32 __chk_giarr_ndx_expr(struct expr_t *);
extern int32 __get_giarr_wide(struct giarr_t *);
extern void __dmp_dsgn_minst(char *);
extern void __dmp_1minst(struct inst_t *);
extern int32 __expr_has_glb(struct expr_t *);
extern char *__regab_tostr(char *, word32 *, word32 *, int32, int32, int32);
extern void __free_namedparams(struct namparam_t *);
extern int32 __chkndx_expr(struct expr_t *, char *);
extern void __cnvt_param_stkval(struct xstk_t *, struct expr_t *,
 struct net_t *, char *);
extern void __assgn_nonis_param(struct net_t *, struct expr_t *,
 struct xstk_t *);
extern char *__msg2_blditree(char *, struct itree_t *);
extern int32 __get_arrwide(struct net_t *);
extern void __push_wrkitstk(struct mod_t *, int32);
extern void __pop_wrkitstk(void);
extern struct mod_t *__get_mast_mdp(struct mod_t *);
extern void __set_drvr_bits(void);
extern void __chk_chg_port_dir(int32);
extern int32 __open_lbfil(int32);

extern void __cv_msg(char *, ...);
extern void __crit_msg(char *, ...);
extern void __pv_ferr(int32, char *, ...);
extern void __pv_err(int32, char *, ...);
extern void __pv_warn(int32, char *,...);
extern void __gfwarn(int32, word32, int32, char *, ...);
extern void __sgfwarn(int32, char *, ...);
extern void __pv_fwarn(int32, char *, ...);
extern void __gfinform(int32, word32, int32, char *, ...);
extern void __gferr(int32, word32, int32, char *, ...);
extern void __sgferr(int32, char *, ...);
extern void __finform(int32, char *, ...);
extern void __dbg_msg(char *, ...);
extern void __sgfinform(int32, char *, ...);
extern void __pv_terr(int32, char *, ...);
extern void __gfterr(int32, word32, int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);
extern void __misc_gfterr(char *, int32, word32, int32);

/*
 * DESIGN WIDE SYNTAX/SEMANTICS CHECKING ROUTINES
 */

/*
 * fixup the net list after all modules read
 * this is second pass on internal data structure
 * returns T to get cannot continue message
 */
extern int32 __fixup_nl(void)
{
 register struct mod_t *mdp;

 /* find and resolve unsatisfied module/udp refs - process lib. in here */
 /* SJM 05/19/04 - following new P1364 LRM, config can't be in src */
 if (__map_files_hd == NULL && __undef_mods != 0)
  {
   resolve_undef_mods();
   if (__undef_mods != 0)
    {
     __crit_msg(
      "  %d modules or udps unresolved after library processing - cannot continue.\n",
      __undef_mods);
     return(FALSE);
    }
  }

 /* at this point all module and task symbol tables frozen so can free */
 /* the tree nodes (can be large) and change formats */
 free_tnblks();
 chg_params_to_tab();

 /* separate into gates and module instances */
 /* also converts cell term expr. list to array of exprs (never explicit) */
 sep_mdgates();

 /* build static instance information */ 
 if (!fix_modcell_nl()) return(TRUE);

 /* check module ports and connect inst ports (for iarrs conns unexpanded) */
 fix_port_conns();

 /* AIV 06/01/04 FIXME ### ??? bug if config used and free these blocks */
 /* now finished with cell pins and cells */
 if (__map_files_hd != NULL)
  {
   free_cpblks();
   /* this also frees cell pins (optional explicit port name) */
   free_cppblks();
  }

 count_flat_insts();

 /* need well formed static instance tree to continue */
 if (__pv_err_cnt != 0) return(TRUE);

 /* build the levelized module table - rebuilt later again after splitting */
 __bld_mlevel_lists();

 /* do split - needed because pound param used in array of gate/inst ranges */
 if (__design_gi_arrays && __num_inst_pndparams != 0) do_giarr_splitting();

 /* build top of itree table now so can search tops when build dfpii */
 bld_top_virtinsts();

 /* after here, all g/i array ranges known */
 /* current value of pound params used then value thrown away because */
 /* defparam setting and splitting may change again */
 if (__design_gi_arrays)
  {
   /* save all module (not task/func) parameter values */
   save_all_param_vals();

   set_giarr_ranges();
   /* rebuild array and gate tables in module with g/i arrays */
   rebld_mod_giarrs();

   /* final step free saved param value records */
   free_all_param_vals();
  }

 /* check all defparams lhs and move globals to defparam struct */
 chk_defparams();

 /* do all splitting for all pound params */
 /* must set pound params after all splitting since need itree inst num. */
 if (__num_inst_pndparams != 0)
  {
   mark_poundparam_splitinsts();
   do_poundparam_splitting();
  }

 /* build the as if flattened instance tree and assign instance numbers */
 __bld_flat_itree();

 /* -- v_fx2.c routines start here -- */

 /* handle defparam setting and associated splitting and param expr array */
 /* building - finally reassigns itree place numbers */
 /* after here all parameters have a numeric value (but maybe IS form) */
 /* therefore can fold and evaluate constant expressions */
 
 /* notice if have defparams must process pound params at same time */
 /* else do just pound params separately */
 if (__num_dfps != 0) __process_defparams();
 else if (__num_inst_pndparams != 0) __set_poundparams();

 /* final itree instance numbers now assigned, build mod by num to inst tab */
 bld_moditps();

 /* resolve all global variables */
 /* this reuses much of freed defparam storage if needed */
 resolve_xmrs();

 /* must stop here if errors since will not have value for xmrs */
 if (__pv_err_cnt != 0) return(TRUE);

 /* fixup - 2nd pass done - connectivity now known and no errors */

 /* set timescale info before module checking that converts delays to ticks */
 __process_timescales();

 /* re-calculate parameter values, for params with other rhs params */
 /* may have been changed by pount or defparam setting */ 
 /* SJM 02/28/02 - need so parameter values use post pound/def val */
 __recalc_param_vals();

 /* now all parameter values known */
 /* must check decl. and eval. wire params before setting port widths */
 /* must always have at least dummy itree context set for these */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);
 
   /* LOOKATME maybe could change to scheme where one mod net table */
   /* contains all mod nets and regs decled in all tasks since one set */
   /* per task - SJM 02/29/00 - yes moved chk taskvars to chk 1 mdecls */
   __chk_1mdecls();
   
   __pop_wrkitstk();
  }

 /* SJM 05/23/01 - can't guess port types until ranges known as exprs */
 /* set net per bit drvrs class (type) and change port dir if option on, */
 /* else warns */
 /* must do this after xmr's resolved and know ranges so can access xmr net */ 
 __set_drvr_bits();
 __chk_chg_port_dir(__chg_portdir);

 /* at this point all wire widths known */

 /* if no arrays of gate or instances in design, can set strengths now */ 
 if (!__design_gi_arrays) __mark_st_wires();

 /* mod port widths set here, widths now known - inst. width set in chk mod */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);
   __setchk_mpwidths();
   __chk_shorted_bids();

   /* must make sure module level symbols are all marked as top level */
   mdp->msym->spltsy = NULL;
   __pop_wrkitstk();
  }

 if (__design_gi_arrays)
  {
   __reconn_gia_pins();
   /* mark wire that have strength storage format */
   __mark_st_wires();
   /* now set strength in port expr. bits if lhs with strength variable */
   /* only check exprs (because need size) at this point are ports */
   __reset_modport_strens();
  }

 /* if decl. errors cannot continue - do not know port widths */
 /* and probably have missing expr. node fields */
 if (__pv_err_cnt != 0) return(TRUE);

 if (!__quiet_msgs) __prt_top_mods();

 /* check mainly element usage semantics */
 /* expression node widths and constant optimizations here */
 /* statement checking here */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);
   __chk_mod();
   __pop_wrkitstk();
  }

 /* free iploctab (line no. per contained mod port) shared among split */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);
   __free_icptab();
   __pop_wrkitstk();
  }

 /* -- specify fixup (pass 2) routines in v fx3 but also expr checking */

 /* check and fix up expressions from specify section */
 /* also emit all unused parameter (both types warnings */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);
   if (mdp->mspfy != NULL)
    {
     __chkfix_spfy();
     if (__no_specify)
      { __free_specify(mdp); mdp->mspfy = NULL; } 
     else if (mdp->mspfy->tchks != NULL && __no_tchks)
      {
       __free_tchks(mdp->mspfy->tchks);
       mdp->mspfy->tchks = NULL;
      }
    }
   __emit_param_informs();
   __pop_wrkitstk();
  }
 return(TRUE);
}

/*
 * routine to print top level modules - on one line
 * always printed unless -q on
 */
extern void __prt_top_mods(void)
{
 register int32 tpii;
 struct inst_t *ip;

 __cv_msg("Highest level modules:\n");
 for (tpii = 0; tpii < __numtopm; tpii++)
  {
   ip = __top_itab[tpii];
   __cv_msg("%s\n", ip->imsym->synam);
  }
}

/*
 * MODULE DECLARATION CHECKING AND LIBRARY RESOLUTION ROUTINES
 */

/*
 * resolve all undefined modules and udps
 */
static void resolve_undef_mods(void)
{
 register struct undef_t *undefp;
 struct sy_t *syp;

 /* try to satisfy unresolved modules/udps from -y, -v or - lib. */
 /* check for remaining unresolveds, cannot continue if any */
 if (__vyhdr != NULL)
  {
   if (__lib_rescan) rescan_process_lib();
   else process_lib();
  }
 if (__undef_mods > 0)
  {
   /* notice this must be printed no matter what */
   __crit_msg("Unresolved modules or udps:\n");
   for (undefp = __undefhd; undefp != NULL; undefp = undefp->undefnxt)
    {
     syp = undefp->msyp;
     if(syp->sydecl || !syp->syundefmod) __misc_terr(__FILE__, __LINE__);
     __crit_msg("  %s\n", syp->synam);
    }
   __crit_msg("\n");
  }
 /* if libverbose messages need to separate */
 else if (__lib_verbose) __cv_msg("\n");
}

/*
 * process library to attempt to resolve all unresolved libraries
 * only called if unresolved modules/udps remain
 */
static void process_lib(void)
{
 register struct undef_t *undefp;
 register struct vylib_t *vyp;
 int32 num_passes, sav_last_lbf;
 struct sy_t *syp;

 /* go thru libs in order */
 for (num_passes = 1;; num_passes++)
  {
   if (num_passes > 1) __rescanning_lib = TRUE;

   __cur_passres = 0;
   if (__lib_verbose)
    {
     /* notice only unresolved after 1st pass are interesting */
     if (num_passes > 1) 
      {
       __cv_msg("\n  Library scan pass %d with the following %d undefined:\n",
        num_passes, __undef_mods);
       for (undefp = __undefhd; undefp != NULL; undefp = undefp->undefnxt)
        {
         syp = undefp->msyp;
         __cv_msg("    %s (first reference %s)\n", syp->synam,
          __bld_lineloc(__xs, syp->syfnam_ind, syp->sylin_cnt));
        }
      }
     else __cv_msg("\n  Begin library scan pass %d (%d undefined).\n",
      num_passes, __undef_mods);
    }

   for (vyp = __vyhdr; vyp != NULL; vyp = vyp->vynxt)
    {
     /* AIV 11/11/03 - if -y dir is empty changed to 'e' so do nothing */
     /* this handles multiple passes empty directories */
     if (vyp->vytyp == 'e') continue;

     if (vyp->vytyp == 'y')
      {
       if (vyp->yfiles == NULL) bld_ylb_dirfiles(vyp);

       /* AIV 11/11/03 - if -y dir is empty made 'e' and must do nothing */
       if (vyp->vytyp == 'e') continue;

       resolve_from_ydir(vyp);
       if (__undef_mods <= 0) return;
       continue;
      }

     /* add or reuse -v library file to in_fils */
     if (vyp->vyfnam_ind == 0)
      {
       /* this dies if >64k */
       if (++__last_lbf >= __siz_in_fils) __grow_infils(__last_lbf);
       vyp->vyfnam_ind = __last_lbf;
       __in_fils[__last_lbf] = __pv_stralloc(vyp->vyu.vyfnam);
      }

     sav_last_lbf = __last_lbf;
     __last_lbf = vyp->vyfnam_ind;
     /* know this will leave undef_mods with correct value */
     if (__open_lbfil(FALSE)) rd_vlibfil(NULL);
     __last_lbf = sav_last_lbf;
     if (__undef_mods <= 0) return;
    }
   if (__cur_passres == 0)
    {
     __pv_err(706,
      "%d modules or primitives unresolved after pass %d with no progress",
      __undef_mods, num_passes);
     return;
    }
  }
}

/*
 * resolve unresolved references from a library directory
 * tricky because resolve as go along effecting __undefhd list
 *
 * algorithm is:
 * 1) go through all unresolved and find all files that match any suffix
 *    and add to list
 * 2) then read and scan all files for any (not just names that produced
 *    file by adding +lbext+ suffix) unresolved - quits in case all resolved
 */
static void resolve_from_ydir(struct vylib_t *vyp)
{
 register int32 lbxi;
 register struct undef_t *undefp;
 int32 dfi, sav_last_lbf, got_err;
 struct undef_t *tmphd, *tmptail, *tmpundefp, *undefp2;
 char fnam[IDLEN], stem[IDLEN];

 tmphd = tmptail = NULL;
 /* build stem that gives current path to directory */
 sprintf(stem, "%s/", vyp->vyu.vydirpth);
 /* case 1: no lbext - module name must exactly match file name */
 if (__last_lbx == -1)
  {
   for (undefp = __undefhd; undefp != NULL; undefp = undefp->undefnxt)
    {
     if ((dfi = srch_yfiles(undefp->msyp->synam, vyp->yfiles, vyp->vyfnam_ind))
      == -1 ) continue;
     /* add to tmp list since parsing changes __undefhd list */ 
     tmpundefp = add_undef_el(undefp->msyp, &tmphd, &tmptail); 
     tmpundefp->dfi = dfi;
     vyp->yfiles[dfi].ylbxi = 0;
    }
  }
 else
  {
   /* case 2: must add lbext to module name and see if matches */
   /* build temporary list of all */
   for (undefp = __undefhd; undefp != NULL; undefp = undefp->undefnxt)
    {
     for (lbxi = 0; lbxi <= __last_lbx; lbxi++)
      {
       sprintf(fnam, "%s%s", undefp->msyp->synam, __lbexts[lbxi]);
       if ((dfi = srch_yfiles(fnam, vyp->yfiles, vyp->vyfnam_ind)) == -1)
        continue;

       /* add to tmp list since parsing changes __undefhd list */ 
       tmpundefp = add_undef_el(undefp->msyp, &tmphd, &tmptail); 
       tmpundefp->dfi = dfi;
       vyp->yfiles[dfi].ylbxi = lbxi;
       /* when found, from precedence rules must stop looking */
       break;
      }
    }
  }
 /* go through temp list reading file */
 for (undefp = tmphd; undefp != NULL; undefp = undefp->undefnxt)
  {
   dfi = undefp->dfi;
   if (vyp->yfiles[dfi].ydirfnam_ind == 0)
    {
     /* this dies if >64k */
     if (++__last_lbf >= __siz_in_fils) __grow_infils(__last_lbf);
     vyp->yfiles[dfi].ydirfnam_ind = __last_lbf;
     if (__last_lbx == -1) sprintf(fnam, "%s%s", stem, undefp->msyp->synam);
     else
      {
       lbxi = vyp->yfiles[dfi].ylbxi;
       sprintf(fnam, "%s%s%s", stem, undefp->msyp->synam, __lbexts[lbxi]);
      }
     __in_fils[__last_lbf] = __pv_stralloc(fnam);
    }
   /* notice each lib. file increments this by 1 */
   sav_last_lbf = __last_lbf;
   __last_lbf = vyp->yfiles[dfi].ydirfnam_ind;
   got_err = FALSE;

   if (__open_lbfil(FALSE)) rd_vlibfil(NULL); else got_err = TRUE;
   __last_lbf = sav_last_lbf;
   if (got_err) continue;
   /* if scanned because of a module and still unresolved after scanning */
   /* need warning - i.e. it is legal to not resolve in name matching */
   /* module but unrecommended */
   if (!undefp->msyp->sydecl)
    {
     __pv_warn(510,
      "module %s still unresolved after processing library directory file %s",
      undefp->msyp->synam, __in_fils[vyp->yfiles[dfi].ydirfnam_ind]);
    }
   else break;
  }
 /* final step is to free temp undef list */
 for (undefp = tmphd; undefp != NULL;) 
  {
   undefp2 = undefp->undefnxt;
   __my_free((char *) undefp, sizeof(struct undef_t)); 
   undefp = undefp2;
  }
}

/*
 * add to tail of undef list that is only singly linked
 */
static struct undef_t *add_undef_el(struct sy_t *syp,
 struct undef_t **hd, struct undef_t **tail)
{
 struct undef_t *undefp;

 undefp = (struct undef_t *) __my_malloc(sizeof(struct undef_t));
 undefp->msyp = syp;
 undefp->dfi = -1; 
 undefp->modnam = NULL; 
 undefp->undefprev = undefp->undefnxt = NULL;
 if (*tail == NULL) *hd = *tail = undefp;
 else { (*tail)->undefnxt = undefp; *tail = undefp; }
 return(undefp);
}

/*
 * preprocess 1 -y library directory - build entry for each file
 * this builds sorted yfiles array
 *
 * notice done only once and saved
 */
static void bld_ylb_dirfiles(struct vylib_t *vyp)
{
 register int32 last_fi;
 register int32 numdfils, siz_mydir, bytlen, obytlen;
 DIR *dirp;
#if defined(__SVR4) || defined(__hpux) || defined(__CYGWIN32__)
 struct dirent *dp;
#else
 /* all BSD cases and special case interface for non unices */
 struct direct *dp;
#endif
 struct mydir_t *mdtab; 

 if ((dirp = opendir(vyp->vyu.vyfnam)) == NULL)
  {
   __pv_warn(511, "-y directory name %s not found - ignored",
    vyp->vyu.vydirpth);
   return;
  }
 siz_mydir = 256;
 bytlen = siz_mydir*sizeof(struct mydir_t);
 mdtab = (struct mydir_t *) __my_malloc(bytlen);
 for (last_fi = -1;;)
  {
   /* should check error number here for other than end of file */
   if ((dp = readdir(dirp)) == NULL) break;
   if (*dp->d_name == '.' && (strcmp(dp->d_name, ".") == 0
    || strcmp(dp->d_name, "..") == 0)) continue;

   if (++last_fi >= siz_mydir)
    {
     obytlen = bytlen;
     siz_mydir = 2*siz_mydir; 
     bytlen = siz_mydir*sizeof(struct mydir_t); 
     /* know initial section copied */
     mdtab = (struct mydir_t *) __my_realloc((char *) mdtab, obytlen, bytlen); 
    }  
   mdtab[last_fi].ydirfnam_ind = 0;
   mdtab[last_fi].ylbxi = 0;
   mdtab[last_fi].dirfnam = __pv_stralloc(dp->d_name);
  }
 closedir(dirp);
 numdfils = last_fi + 1;

 /* AIV 11/11/03 could be an empty directory for -y, free, return */
 if (numdfils == 0)
  {
   __my_free((char *) mdtab, bytlen);
   __pv_warn(3114, "-y directory %s empty - ignored", vyp->vyu.vydirpth);
   /* set vy type to 'e' to prevent any further attmpts to rebuild */
   vyp->vytyp = 'e';
   return;
  }

 /* final step, shrink my directory table back to exact size */
 if (numdfils != siz_mydir) 
  {
   obytlen = bytlen;
   bytlen = numdfils*sizeof(struct mydir_t); 
   mdtab = (struct mydir_t *) __my_realloc((char *) mdtab, obytlen, bytlen); 
  } 
 qsort((char *) mdtab, (word32) numdfils, sizeof(struct mydir_t), fn_cmp); 
 vyp->vyfnam_ind = numdfils;
 vyp->yfiles = mdtab;
 __num_ys++;
}

/*
 * find index in sorted -y directory yfiles table
 */
static int32 srch_yfiles(char *nam, struct mydir_t *mdtab, 
 word32 numdfiles)
{
 int32 l, h;
 register int32 m, cv;

 if (numdfiles == 0) return(-1);
 l = 0; h = numdfiles - 1;
 for (;;)
  {
   m = (l + h)/2;
   if ((cv = strcmp(mdtab[m].dirfnam, nam)) == 0) return(m);
   if (cv < 0) l = m + 1; else h = m - 1;
   if (h < l) break;
  }
 return(-1);
}

/*
 * module port name comparison routine
 */
static int32 fn_cmp(const void *dp1, const void *dp2)
{
 return(strcmp(((struct mydir_t *) dp1)->dirfnam,
  ((struct mydir_t *) dp2)->dirfnam));
}

/*
 * process a -v or in -y directory file - must try to resolve all modules
 * know file open and no token read and normal source input env. set
 * caller must open and close file
 *
 *
 * if lib rescan only resolve one passed undefp and stop when resolved
 *
 * LOOKATME _ better here to save byte location from ftell and seek to byte
 * for defined before used definitions but for now cannot do because do
 * not have mechanism for saving and restoring compiler directive state?
 */
static void rd_vlibfil(struct undef_t *res_undefp)
{
 int32 nd_repeat, len, rewind_pass;
 struct undef_t *hd, *tail, *undefp, *undefp2;
 struct sy_t *syp;
 char *chp, savtoken[IDLEN];

 rewind_pass = 0;
again:
 rewind_pass++;
 hd = tail = NULL; 
 __get_vtok();
 if (__toktyp == TEOF)
  { __pv_fwarn(609, "library file contains no tokens"); return; }

 for (;;)
  {
   /* may be a compiler directive */
   if (__toktyp >= CDIR_TOKEN_START && __toktyp <= CDIR_TOKEN_END)
    {
     __process_cdir();
     goto nxt_tok;
    }
   switch ((byte) __toktyp) {
    case MACROMODULE:
     __get_vtok();
     __finform(423,
      "macromodules in library not expanded - %s translated as module",
      __token);
     goto chk_name;
    case MODULE:
     __get_vtok();
chk_name:
     if (__toktyp != ID)
      {
       __pv_ferr(707, "in library file, module name expected - %s read",
        __prt_vtok());
       /* since error, just try to resynchronize */
       __vskipto_modend(ENDMODULE);
       goto nxt_tok;
      }

     /* if rescan specific one to resolve skip all but that one */
     if ((syp = __get_sym(__token, __modsyms)) == NULL || syp->sydecl
      || (res_undefp != NULL && syp != res_undefp->msyp))
      { 
       /* must save module name - skip overwrites */
       strcpy(savtoken, __token);
       if (!__vskipto_modend(ENDMODULE))
        { 
         __pv_ferr(767, 
         "syntax error in skipped (defined or not referenced) library module %s",
          __token);
         goto nxt_tok;
        }
try_add_unref:
       /* if symbol never referenced up to here may be define before use */
       /* so need to add to list in case reference nearer end */
       /* for rescan mode, this inhibits any rescanning */
       if (syp == NULL && !__lib_rescan)
        {
         undefp = add_undef_el((struct sy_t *) NULL, &hd, &tail); 
         chp = __pv_stralloc(savtoken);
         undefp->modnam = chp;
        }
       goto nxt_tok;
      }   

     /* notice name token read - also undef_mods not changed */
     if (__lib_verbose)
      __cv_msg("  Compiling library module (%s).\n", __token);
     /* know error here will cause skipping to file level thing */
     if (!__rd_moddef(NULL, FALSE)) goto nxt_tok;
     /* when reading source this was set only if in cell define region */
     /* now turn on (maybe again) if all library modules are cells */
     /* dummy itstk empty here but if good module read mark as cell */
     if (__lib_are_cells && __last_libmdp != NULL)
      {
       __last_libmdp->m_iscell = TRUE;
       __design_has_cells = TRUE;
      }
     __last_libmdp = NULL;
     /* know to get here 1 more resolved */
     __cur_passres++;
     if (__undef_mods <= 0 || __lib_rescan) return;
     break;

    case PRIMITIVE:
     __get_vtok();
     if (__toktyp != ID)
      {
       __pv_ferr(708, "library file udp primitive name expected - %s read",
	__prt_vtok());
       /* since err, just try to skip to end primitive */
       __vskipto_modend(ENDPRIMITIVE);
       goto nxt_tok;
      }
     /* if rescan specific one to resolve skip all but that one */
     if ((syp = __get_sym(__token, __modsyms)) == NULL || syp->sydecl
      || (res_undefp != NULL && syp != res_undefp->msyp))
      { 
       /* must save module name - skip overwrites */
       strcpy(savtoken, __token);
       if (!__vskipto_modend(ENDPRIMITIVE))
        {
         __pv_ferr(702,
	  "syntax error in skipped (defined or not referenced) library primitive %s",
          __token);
         goto nxt_tok;
        }
       goto try_add_unref;
      }
     /* notice name token read */
     if (__lib_verbose)
      __cv_msg("  Compiling library udp primitive (%s).\n", __token);
     if (!__rd_udpdef(NULL)) goto nxt_tok;
     /* if all modules resolved, nothing more to do */
     __cur_passres++;
     if (__undef_mods <= 0 || __lib_rescan) return;
     break;
    default:
     __pv_ferr(709,
     "library file module, primitive or directive expected - %s read",
      __prt_vtok());
     /* here just ignores extra semicolons */
     if (__toktyp != SEMI) __vskipto2_modend(ENDMODULE, ENDPRIMITIVE);
   }
nxt_tok:
   /* why checking this twice */
   if (__toktyp == TEOF)
    {
chk_ifdef:
     if (__in_ifdef_level != 0)
      {
       __pv_err(924, "last `ifdef unterminated in libary file %s",
        __cur_fnam);
      }
     break;
    }
   __get_vtok();
   if (__toktyp == TEOF) goto chk_ifdef;
 }
 /* need to determine if any of the skipped modules or udp's now undefined */
 nd_repeat = FALSE;
 for (undefp = hd; undefp != NULL; undefp = undefp->undefnxt) 
  {
   /* SJM 06/03/02 - change so undef list no longer shares undefprev */
   chp = (char *) undefp->modnam;
   /* DBG remove --- */
   if (chp == NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */
   
   if ((syp = __get_sym(chp, __modsyms)) != NULL && !syp->sydecl)
    {
     nd_repeat = TRUE; 
     if (!__lib_verbose) break;
     __cv_msg(
      "  Rereading %s library element %s defined before use (pass %d).\n",
      __cur_fnam, syp->synam, rewind_pass);
    }
  } 

 /* go through freeing list */
 for (undefp = hd; undefp != NULL;) 
  {
   undefp2 = undefp->undefnxt;
   /* LOOKATME - think will always be set here */ 
   if (undefp->modnam != NULL)
    {
     chp = undefp->modnam;
     len = strlen(chp);
     __my_free(chp, len + 1);
    }
   __my_free((char *) undefp, sizeof(struct undef_t)); 
   undefp = undefp2;
  }
 if (nd_repeat)
  {
   __my_rewind(__in_s);
   /* in case hit eof with push back make sure reads 1st token */
   __lasttoktyp = UNDEF;
   __lin_cnt = 1;
   __file_just_op = TRUE;
   goto again;
  }
}

/*
 * try to open the next library file and repl. top of stack with its info
 * file must be openable and have contents
 * return F on no success
 * in fils of last lbf must be filled with file name
 * since know last_lbf > last_inf - on EOF get_vtok will resturn to caller
 */
extern int32 __open_lbfil(int32 is_dir)
{
 /* if not first time, close previous file */
 if (__visp->vi_s != NULL) { __my_fclose(__visp->vi_s); __visp->vi_s = NULL; }
 /* know called with last_lbf index of file to process */
 __cur_fnam = __in_fils[__last_lbf];
 if ((__in_s = __tilde_fopen(__cur_fnam, "r")) == NULL)
  {
   if (is_dir) strcpy(__xs, " directory"); else strcpy(__xs, "");
   __pv_err(710, "cannot open Verilog library%s file %s - skipped", __xs,
    __cur_fnam);
   return(FALSE);
  }
 if (feof(__in_s))
  {
   __pv_warn(512, "Verilog library file %s empty", __cur_fnam);
   return(FALSE);
  }
 /* whenever open new file must discard pushed back */
 __lasttoktyp = UNDEF;
 __visp->vi_s = __in_s;
 __visp->vifnam_ind = __last_lbf;
 __cur_fnam_ind = __last_lbf;
 __cur_fnam = __in_fils[__cur_fnam_ind];
 __lin_cnt = 1;
 __file_just_op = TRUE;
 if (__lib_verbose)
  {
   if (is_dir)
    __cv_msg("  Opening library directory file \"%s\".\n", __cur_fnam);
   else __cv_msg("  Scanning library file \"%s\".\n", __cur_fnam);
  }
 return(TRUE);
}

/*
 * RESCAN LIBRARY RESOLUTION ROUTINES 
 */
 
/*
 * process library to attempt to resolve all unresolved libraries
 * only called if unresolved modules/udps remain
 *
 * BEWARE - must not free memory of linked out resolved undefp for
 * 2 reasons: 1) need it for error msgs, 2) need next field to move in list
 */
static void rescan_process_lib(void)
{
 register struct undef_t *undefp;
 register struct vylib_t *vyp;
 struct undef_t *sav_undefhd;
 struct sy_t *syp;
 int32 sav_last_lbf, passi;

 if (__lib_verbose)
  {
   __cv_msg("\n  Begin rescan mode library resolution (%d undefined).\n",
    __undef_mods);
  }

 /* goto through unresolved resolving one per pass */
 /* resolution of one may add other to end, but next field will */
 /* be right for new */
 passi = 1; 
 for (undefp = __undefhd; undefp != NULL; passi++)
  {
   /* AIV 10/10/03 - undefhd may be removed and then new undefhd added as */
   /* unresolved so must save and replace if changed */
   sav_undefhd = __undefhd;

   if (passi > 1) __rescanning_lib = TRUE;

   if (__lib_verbose)
    {
     syp = undefp->msyp;
     __cv_msg("\n  Resolving %s (first reference %s)(pass %d)\n",
      syp->synam, __bld_lineloc(__xs, syp->syfnam_ind, syp->sylin_cnt), passi);
    }
   __cur_passres = 0;
   for (vyp = __vyhdr; vyp != NULL; vyp = vyp->vynxt)
    {
     /* AIV 11/11/03 - if -y dir is empty changed to 'e' so do nothing */
     /* this handles empty directories for rescanning */
     if (vyp->vytyp == 'e') continue;

     if (vyp->vytyp == 'y')
      {
       if (vyp->yfiles == NULL) bld_ylb_dirfiles(vyp);

       /* AIV 11/11/03 - if -y dir is empty made 'e' and must do nothing */
       if (vyp->vytyp == 'e') continue;

       rescan_resolve_from_ydir(vyp, undefp);
       if (__cur_passres == 0) continue;

       /* DBG remove -- */
       if (__cur_passres != 1) __misc_terr(__FILE__, __LINE__);
       /* --- */
       goto resolve_nxt;
      }

     /* add or reuse -v library file to in_fils */
     if (vyp->vyfnam_ind == 0)
      {
       /* this dies if >64k */
       if (++__last_lbf >= __siz_in_fils) __grow_infils(__last_lbf);
       vyp->vyfnam_ind = __last_lbf;
       __in_fils[__last_lbf] = __pv_stralloc(vyp->vyu.vyfnam);
      }

     sav_last_lbf = __last_lbf;
     __last_lbf = vyp->vyfnam_ind;
     /* know this will leave undef_mods with correct value */
     if (__open_lbfil(FALSE)) rd_vlibfil(undefp);
     __last_lbf = sav_last_lbf;
     if (__cur_passres == 0) continue;
     /* DBG remove -- */
     if (__cur_passres != 1) __misc_terr(__FILE__, __LINE__);
     /* --- */
     goto resolve_nxt;
    }
   __pv_err(706,
    "lib rescan module %s unresolved after all libaries searched - skipping to next",
      undefp->msyp->synam);

resolve_nxt:;
   /* AIV 10/07/03 - if new head it is "next" else move one down list */ 
   if (sav_undefhd != __undefhd) undefp = __undefhd;
   else undefp = undefp->undefnxt;
  }
}

/*
 * rescan resolve one current unresolved references from a library dir
 * simpler than non rescan cases because at most one resolved 
 *
 * algorithm is:
 * 1) go through one current unresolved and find all files that match suffix
 *    and add to list
 * 2) then read and scan all files for the one unresolved now working on
 */
static void rescan_resolve_from_ydir(struct vylib_t *vyp,
 struct undef_t *undefp)
{
 register int32 lbxi;
 int32 dfi, sav_last_lbf, got_err;
 struct undef_t *tmpundefp, *undefp2, *tmphd, *tmptail;
 char fnam[IDLEN], stem[IDLEN];

 /* build stem that gives current path to directory */
 sprintf(stem, "%s/", vyp->vyu.vydirpth);
 /* case 1: no lbext - module name must exactly match file name */
 tmphd = tmptail = NULL;
 if (__last_lbx == -1)
  {
   if ((dfi = srch_yfiles(undefp->msyp->synam, vyp->yfiles, vyp->vyfnam_ind))
    != -1)
    {
     /* add to tmp list since parsing changes __undefhd list */ 
     tmpundefp = add_undef_el(undefp->msyp, &tmphd, &tmptail); 
     tmpundefp->dfi = dfi;
     vyp->yfiles[dfi].ylbxi = 0;
    }
  }
 else
  {
   /* case 2: must add lbext to module name and see if matches */
   /* build temporary list of all */
   for (lbxi = 0; lbxi <= __last_lbx; lbxi++)
    {
     sprintf(fnam, "%s%s", undefp->msyp->synam, __lbexts[lbxi]);
     if ((dfi = srch_yfiles(fnam, vyp->yfiles, vyp->vyfnam_ind)) == -1)
      continue;

     /* add to tmp list since parsing changes __undefhd list */ 
     tmpundefp = add_undef_el(undefp->msyp, &tmphd, &tmptail); 
     tmpundefp->dfi = dfi;
     vyp->yfiles[dfi].ylbxi = lbxi;
     /* when found, from precedence rules must stop looking */
     break;
    }
  }
 /* go through temp (will always be exactly one long) list reading file */
 /* DBG remove -- */
 if (tmphd != NULL && tmphd->undefnxt != NULL)
  __misc_terr(__FILE__, __LINE__);
 /* --- */
 for (undefp = tmphd; undefp != NULL; undefp = undefp->undefnxt)
  {
   dfi = undefp->dfi;
   if (vyp->yfiles[dfi].ydirfnam_ind == 0)
    {
     /* this dies if >64k */
     if (++__last_lbf >= __siz_in_fils) __grow_infils(__last_lbf);
     vyp->yfiles[dfi].ydirfnam_ind = __last_lbf;
     if (__last_lbx == -1) sprintf(fnam, "%s%s", stem, undefp->msyp->synam);
     else
      {
       lbxi = vyp->yfiles[dfi].ylbxi;
       sprintf(fnam, "%s%s%s", stem, undefp->msyp->synam, __lbexts[lbxi]);
      }
     __in_fils[__last_lbf] = __pv_stralloc(fnam);
    }
   /* notice each lib. file increments this by 1 */
   sav_last_lbf = __last_lbf;
   __last_lbf = vyp->yfiles[dfi].ydirfnam_ind;
   got_err = FALSE;

   if (__open_lbfil(FALSE)) rd_vlibfil(undefp); else got_err = TRUE;
   __last_lbf = sav_last_lbf;
   if (got_err) continue;
   /* if scanned because of a module and still unresolved after scanning */
   /* need warning - i.e. it is legal to not resolve in name matching */
   /* module but unrecommended */
   if (!undefp->msyp->sydecl)
    {
     __pv_warn(510,
      "module %s still unresolved after processing library directory file %s",
      undefp->msyp->synam, __in_fils[vyp->yfiles[dfi].ydirfnam_ind]);
    }
   else break;
  }
 /* final step is to free temp undef list */
 for (undefp = tmphd; undefp != NULL;) 
  {
   undefp2 = undefp->undefnxt;
   __my_free((char *) undefp, sizeof(struct undef_t)); 
   undefp = undefp2;
  }
}

/*
 * PRELIMINARY FIXUP ROUTINES USUALLY FORMAT CHANGES 
 */

/*
 * routine to free all allocated tnode blks from unfrozen part of symtabs
 */
static void free_tnblks(void)
{
 register struct tnblk_t *tnbp, *tnbp2;

 /* free all cell pin blks since ncomp form now gone */
 for (tnbp = __hdr_tnblks; tnbp != NULL;)
  {
   tnbp2 = tnbp->tnblknxt;
   __my_free((char *) tnbp->tnblks, BIG_ALLOC_SIZE);
   __my_free((char *) tnbp, sizeof(struct tnblk_t));
   tnbp = tnbp2;
  }
 __hdr_tnblks = NULL;
}

/*
 * change all module and task parameter lists to table from list
 *
 * also sets mprmnum and tprmnum so know size of tables
 * after completion, param nu2 field nnxt field no longed needed
 */
static void chg_params_to_tab(void)
{
 register int32 pi;
 register struct net_t *pnp, *pnp2;
 register struct mod_t *mdp;
 int32 pnum;
 struct net_t *nptab;
 struct task_t *tskp;
 struct spfy_t *spfyp;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   /* first module params */
   pnum = cnt_prms(mdp->mprms);
   if (pnum != 0)
    {
     nptab = (struct net_t *) __my_malloc(pnum*sizeof(struct net_t));
     pi = 0;
     for (pnp = mdp->mprms; pnp != NULL; pnp = pnp->nu2.nnxt, pi++)
      {
       pnp2 = &(nptab[pi]);
       *pnp2 = *pnp;
       /* symbol points back to net - because copied to table change ptr */
       pnp2->nsym->el.enp = pnp2;
       /* ptr filds in pnp unique so will just get moved to new */
       /* except must nil out new table pnp2 nu2 because needed for saving */
       pnp2->nu2.wp = NULL;
      }
     mdp->mprmnum = pnum;
     free_param_listform(mdp->mprms);
     mdp->mprms = nptab;
    }

   /* first module LOCAL params */
   pnum = cnt_prms(mdp->mlocprms);
   if (pnum != 0)
    {
     nptab = (struct net_t *) __my_malloc(pnum*sizeof(struct net_t));
     pi = 0;
     for (pnp = mdp->mlocprms; pnp != NULL; pnp = pnp->nu2.nnxt, pi++)
      {
       pnp2 = &(nptab[pi]);
       *pnp2 = *pnp;
       pnp2->nsym->el.enp = pnp2;
       pnp2->nu2.wp = NULL;
      }
     mdp->mlocprmnum = pnum;
     free_param_listform(mdp->mlocprms);
     mdp->mlocprms = nptab;
    }

   /* next parameters in each task */
   for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     if ((pnum = cnt_prms(tskp->tsk_prms)) != 0) 
      {
       nptab = (struct net_t *) __my_malloc(pnum*sizeof(struct net_t));
       pi = 0;
       for (pnp = tskp->tsk_prms; pnp != NULL; pnp = pnp->nu2.nnxt, pi++)
        {
         pnp2 = &(nptab[pi]);
         *pnp2 = *pnp;
         /* symbol points back to net - because copied to table change ptr */
         pnp2->nsym->el.enp = pnp2;
         /* ptr filds in pnp uniue so will just get moved to new */
         /* except must nil out new table pnp2 nu2 because needed for saving */
         pnp2->nu2.wp = NULL;
        }
       tskp->tprmnum = pnum;
       free_param_listform(tskp->tsk_prms);
       tskp->tsk_prms = nptab;
      }
     /* do the task LOCAL parameters as well */
     if ((pnum = cnt_prms(tskp->tsk_locprms)) != 0) 
      {
       nptab = (struct net_t *) __my_malloc(pnum*sizeof(struct net_t));
       pi = 0;
       for (pnp = tskp->tsk_locprms; pnp != NULL; pnp = pnp->nu2.nnxt, pi++)
        {
         pnp2 = &(nptab[pi]);
         *pnp2 = *pnp;
         /* symbol points back to net - because copied to table change ptr */
         pnp2->nsym->el.enp = pnp2;
         /* ptr filds in pnp uniue so will just get moved to new */
         /* except must nil out new table pnp2 nu2 because needed for saving */
         pnp2->nu2.wp = NULL;
        }
       tskp->tlocprmnum = pnum;
       free_param_listform(tskp->tsk_locprms);
       tskp->tsk_locprms = nptab;
      }
    }
   /* finally specparams */
   if ((spfyp = mdp->mspfy) == NULL) goto nxt_mod;
   if ((pnum = cnt_prms(spfyp->msprms)) == 0) goto nxt_mod;

   nptab = (struct net_t *) __my_malloc(pnum*sizeof(struct net_t));
   pi = 0;
   for (pnp = spfyp->msprms; pnp != NULL; pnp = pnp->nu2.nnxt, pi++)
    {
     pnp2 = &(nptab[pi]);
     *pnp2 = *pnp;
     /* symbol points back to net - because copied to table change ptr */
     pnp2->nsym->el.enp = pnp2;
     /* ptr filds in pnp unique so will just get moved to new */
     /* except must nil out new table pnp2 nu2 because needed for saving */
     pnp2->nu2.wp = NULL;
    }
   spfyp->sprmnum = pnum;
   free_param_listform(spfyp->msprms);
   spfyp->msprms = nptab;
nxt_mod:
   __pop_wrkitstk();
  }

}

/*
 * count params in list form
 */
static int32 cnt_prms(struct net_t *np)
{
 register struct net_t *parm_np;
 int32 pnum;

 for (pnum = 0, parm_np = np; parm_np != NULL; parm_np = parm_np->nu2.nnxt)
  { pnum++; }
 return(pnum);
}

/*
 * free parameter list form
 *
 * no freeing of ncomp or expressions pointed to from inside since moved
 * to table form element
 */
static void free_param_listform(struct net_t *parm_np)
{
 register struct net_t *pnp, *pnp2;

 for (pnp = parm_np; pnp != NULL;)
  {
   pnp2 = pnp->nu2.nnxt;
   __my_free((char *) pnp, sizeof(struct net_t)); 
   pnp = pnp2;
  }
}

/*
 * ROUTINES TO SEPARATE CELLS INTO GATES AND INSTANCES
 */

/*
 * separate module gate and insts into separate lists
 * contas already separated by here 
 * relocate, free cell_t, and build array form port lists
 *
 * also checks for unnamed module instances and number cells with type range
 * could save some space by make insts and gates array of thing rather
 * than array of ptrs to things
 */
static void sep_mdgates(void)
{
 register struct cell_t *cp;
 int32 modinum, modgnum, ii, gi, nbytes;
 struct mod_t *mdp;
 struct inst_t *iptab;
 struct gate_t *gptab;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   /* first allocate array of pointers to instances */
   modinum = count_minum_and_mgnum(&modgnum, mdp);
   if (modinum != 0)
    {
     nbytes = modinum*sizeof(struct inst_t);
     iptab = (struct inst_t *) __my_malloc(nbytes);
    }
   else iptab = NULL;
   if (modgnum != 0)
    {
     nbytes = modgnum*sizeof(struct gate_t);
     gptab = (struct gate_t *) __my_malloc(nbytes);
    }
   else gptab = NULL;

   ii = gi = -1;
   for (cp = mdp->mcells; cp != NULL; cp = cp->cnxt)
    {
     /* if not declared error above can still get here */
     /* but undeclared udp symbol with always be actually undcl mod */
     /* case 1: udp */ 
     if (cp->cmsym->sytyp == SYM_UDP)
      {
       cellrep_to_gate(cp, &(gptab[++gi]));
       if (cp->cx1 != NULL) bld_giarr(mdp, gi, cp, modgnum);
       continue;
      }
     /* case 2: built in primitive */ 
     if (cp->cmsym->sytyp == SYM_PRIM)
      {
       /* notice pullup becomes gate but can have only 1 (or more) ports */
       cellrep_to_gate(cp, &(gptab[++gi]));
       if (cp->cx1 != NULL) bld_giarr(mdp, gi, cp, modgnum);
       continue;
      }
     /* case 3: instance */
     if (!cp->c_named)
      {
       __gferr(731, cp->csym->syfnam_ind, cp->csym->sylin_cnt,
        "required module instance name missing (type is %s)",
        cp->cmsym->synam);
       cp->c_named = FALSE;
      }
     if (cp->cmsym != NULL && cp->cmsym->sydecl)
      {
       cellrep_to_inst(cp, &(iptab[++ii]));
       if (cp->cx1 != NULL) bld_miarr(mdp, ii, cp, modinum);
      }
    }
   mdp->minum = modinum; 
   mdp->minsts = iptab; 
   mdp->mgnum = modgnum;
   mdp->mgates = gptab; 
   mdp->mcells = NULL;

   __pop_wrkitstk();
  }
}

/*
 * count number of instances in module
 * need array that is parallel to itree array for instances so must
 * count in order to allocate before building array
 */
static int32 count_minum_and_mgnum(int32 *gnum, struct mod_t *mdp)
{
 register struct cell_t *cp;
 int32 inum;
 struct sy_t *syp;

 for (inum = *gnum = 0, cp = mdp->mcells; cp != NULL; cp = cp->cnxt)
  {
   syp = cp->cmsym;
   if (syp == NULL || !syp->sydecl) continue;

   if (syp->sytyp == SYM_UDP || syp->sytyp == SYM_PRIM) (*gnum)++;
   else inum++;
  }
 return(inum);
}

/*
 * convert a syntax analysis format general cell struct to a sim gate struct
 * cell pin list changed to expression list later
 */
static void cellrep_to_gate(struct cell_t *cp, struct gate_t *gp)
{
 register int32 pi;
 register struct cell_pin_t *cpp;
 int32 pnum, nbytes, all_named;
 char s1[RECLEN];

 /* how is a port connect lost */
 if ((pnum = cnt_gateprts(cp)) == 0L)
  {
   __gferr(732, cp->csym->syfnam_ind, cp->csym->sylin_cnt,
    "%s %s illegal - at least one gate required", __to_sytyp(__xs,
    cp->cmsym->sytyp), cp->cmsym->synam);
  }
 /* must fit in 16 bits */
 if (pnum >= 0x3fffL)
  {
   __gferr(732, cp->csym->syfnam_ind, cp->csym->sylin_cnt,
    "%s %s has too many terminals (%d)", __to_sytyp(__xs, cp->cmsym->sytyp),
    cp->cmsym->synam, 0x3fffL);
   pnum = 0x3fffL;
  }

 /* notice symbol type is still SYM_I here */
 gp->gsym = cp->csym;
 gp->gsym->el.egp = gp;
 gp->gmsym = cp->cmsym;
 gp->gpnum = pnum;
 gp->g_hasst = (cp->c_hasst) ? TRUE : FALSE;
 gp->g_stval = cp->c_stval;
 gp->g_delrep = DT_CMPLST;
 gp->g_gone = FALSE;
 gp->g_pdst = FALSE;
 if (gp->gmsym->sytyp == SYM_UDP)
  { gp->g_class = GC_UDP; gp->gsym->sytyp = SYM_PRIM; }
 else
  {
   gp->g_class = gp->gmsym->el.eprimp->gclass;
   gp->gsym->sytyp = SYM_PRIM;
  }

 gp->schd_tevs = NULL;
 /* since before cells numbered non zero means unnamed */
 if (!cp->c_named) gp->g_unam = TRUE; else gp->g_unam = FALSE;

 gp->g_du.pdels = NULL;
 if (cp->c_nparms != NULL)
  {
   /* explicit parameter name form illegal for udps or gates */
   /* T returned if some named */
   if (pndparams_explicit(cp->c_nparms, &all_named))
    {
     if (all_named) strcpy(s1, "all"); else strcpy(s1, "some");
     __gferr(809, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
      "%s explicit .[parameter name]([expr]) # delay forms illegal for %s %s",
      s1, __to_sytyp(__xs, gp->gmsym->sytyp), gp->gmsym->synam);
     __free_namedparams(cp->c_nparms);
     gp->g_du.pdels = NULL;
    }
   else
    {
     gp->g_du.pdels = bld_gate_paramlst(cp->c_nparms);
     __free_namedparams(cp->c_nparms);
     cp->c_nparms = NULL;
    }
  }
 gp->gstate.wp = NULL;

 /* just move ptr since not shared */
 gp->gattrs = cp->cattrs;

 nbytes = pnum*sizeof(struct expr_t *);
 /* SJM 05/05/03 - from previous syntax errors where can't recover */ 
 /* this may be nil, but since errors still works */
 if (pnum > 0) gp->gpins = (struct expr_t **) __my_malloc(nbytes);
 else gp->gpins = NULL;
 gp->gchg_func = NULL;
 for (cpp = cp->cpins, pi = 0; cpp != NULL; pi++, cpp = cpp->cpnxt)
  {
   /* this should always be at least 'bx by here */
   if (cpp->pnam != NULL)
    {
     __gferr(733, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
      "explicit port connection illegal for %s %s",
      __to_sytyp(__xs, gp->gmsym->sytyp), gp->gmsym->synam);
     /* all freeing later */
    }
   /* this copies expr. (not freed) from cpins so when entire */
   /* cpin blocks freed later, no interlinking */ 
   if (pi < pnum) gp->gpins[pi] = cpp->cpxnd;
  }
}

/*
 * return T if param name list has any explicit named forms
 * if returns T, also sets flag if all are explicit form
 */
static int32 pndparams_explicit(struct namparam_t *npmphdr, int32 *all_named)
{
 register struct namparam_t *npmp;
 int32 has_named, all2_named;

 *all_named = FALSE;
 has_named = FALSE;
 all2_named = TRUE;
 for (npmp = npmphdr; npmp != NULL; npmp = npmp->nprmnxt)
  {
   if (npmp->pnam != NULL) has_named = TRUE;
   else all2_named = FALSE;
  }
 if (has_named) *all_named = all2_named; 
 return(has_named);
}

/*
 * build a delay parameter list from a named parameter list for gates
 *
 * know all unnamed comma separated form list or will not be called
 * also know delay list with at least one element present or not called
 */
static struct paramlst_t *bld_gate_paramlst(struct namparam_t *npmphdr)
{
 register struct namparam_t *npmp;
 int32 pi;
 struct paramlst_t *nplphdr, *nplp, *last_nplp;

 nplphdr = last_nplp = NULL;
 for (pi = 0, npmp = npmphdr; npmp != NULL; npmp = npmp->nprmnxt, pi++)
  {
   if (npmp->pxndp->optyp == OPEMPTY)
    {
     __pv_ferr(1190,
      "empty ,, form primitive pound delay value illegal (pos. %d)", pi + 1);
     continue;
    }
   nplp = __alloc_pval();
   nplp->plxndp = npmp->pxndp; 
   npmp->pxndp = NULL;
   if (last_nplp == NULL) nplphdr = nplp; else last_nplp->pmlnxt = nplp;
   nplp->pmlnxt = NULL;
   last_nplp = nplp;
  }
 if (nplphdr == NULL)
  {
   __pv_ferr(1095, "empty primitive #() delay expression illegal");
  }
 return(nplphdr);
}

/*
 * build parallel array to mgates index element 
 * only called if array form has range expr
 */
static void bld_giarr(struct mod_t *mdp, int32 gi, struct cell_t *cp, int32 mgnum)
{
 int32 i;
 struct giarr_t *giap;

 if (mdp->mgarr == NULL)
  {
   mdp->mgarr = (struct giarr_t **) __my_malloc(mgnum*sizeof(struct giarr_t *));
   for (i = 0; i < mgnum; i++) mdp->mgarr[i] = NULL;
  }
 mdp->mgarr[gi] = (struct giarr_t *)__my_malloc(sizeof(struct giarr_t)); 
 giap = mdp->mgarr[gi];
 init_giarr(giap);
 giap->giax1 = cp->cx1;  
 giap->giax2 = cp->cx2;
 __design_gi_arrays = TRUE;
}

/*
 * initialize a giarr
 */
static void init_giarr(struct giarr_t *giap)
{
 giap->gia_xpnd = FALSE;
 giap->gia_rng_has_pnd = FALSE;
 giap->giax1 = giap->giax2 = NULL;
 giap->gia1 = giap->gia2 = -1; 
 giap->gia_bi = -1;
 giap->giapins = NULL;
 giap->gia_base_syp = NULL;
}

/*
 * build parallel array to marrs index element 
 * only called if module has arrays of insts
 */
static void bld_miarr(struct mod_t *mdp, int32 ii, struct cell_t *cp, int32 minum)
{
 int32 i;
 struct giarr_t *giap;

 if (mdp->miarr == NULL)
  {
   mdp->miarr = (struct giarr_t **) __my_malloc(minum*sizeof(struct giarr_t *));
   for (i = 0; i < minum; i++) mdp->miarr[i] = NULL;
  }
 mdp->miarr[ii] = (struct giarr_t *) __my_malloc(sizeof(struct giarr_t)); 
 giap = mdp->miarr[ii];
 init_giarr(giap);
 giap->giax1 = cp->cx1;  
 giap->giax2 = cp->cx2;
 __design_gi_arrays = TRUE;
}

/*
 * count the number of ports a gate has
 */
static int32 cnt_gateprts(struct cell_t *cp)
{
 register struct cell_pin_t *cpp;
 int32 pnum;

 for (pnum = 0, cpp = cp->cpins; cpp != NULL; cpp = cpp->cpnxt) pnum++;
 return(pnum);
}

/*
 * convert a syntax analysis format general cell struct to a sim gate struct
 *
 * by here know module declared params converted to table from list
 * this also checked passed instance # params constant expressions
 */
static void cellrep_to_inst(struct cell_t *cp, struct inst_t *ip)
{
 /* if module not defined, nothing to do */
 if (cp->c_hasst)
  {
   __gferr(734, cp->csym->syfnam_ind, cp->csym->sylin_cnt,
    "module instance %s type %s strength specification illegal",
    cp->csym->synam, cp->cmsym->synam);
  }

 init_inst(ip);

 ip->isym = cp->csym;
 ip->isym->el.eip = ip;
 ip->imsym = cp->cmsym;
 ip->pprm_explicit = FALSE;
 /* instance numbers assigned later because splitting can change */
 /* convert named param list to parallel table of pound param exprs */

 /* must set instance type flags before processing pound params */
 ip->i_iscell = (cp->c_iscell) ? TRUE : FALSE;
 ip->i_pndsplit = FALSE;

 /* this also checks pound paremeter's since know each params loc. */
 if (cp->c_nparms == NULL) ip->ipxprtab = NULL;
 else ip->ipxprtab = inst_nparms_to_xtab(cp->c_nparms, ip);

 ip->ip_explicit = (cp->cp_explicit) ? TRUE : FALSE;

 /* count instances with pound params */
 if (ip->ipxprtab != NULL) __num_inst_pndparams++;

 /* just move ptr since not shared */
 ip->iattrs = cp->cattrs;
 /* if has instance attributes combine mod attributes into inst attrs */
 if (ip->iattrs != NULL && ip->imsym->el.emdp->mattrs != NULL)
  add_mod_attrs_toinst(ip);

 /* for now must leave inst-pin list form */
 ip->ipins = (struct expr_t **) cp->cpins;
}

/*
 * routine to initialize inst_t record
 *
 * SJM 10/16/00 - needed to avoid errors and for top_ip
 */
static void init_inst(struct inst_t *ip)
{
 ip->ip_explicit = FALSE;
 ip->pprm_explicit = FALSE;
 ip->i_iscell = FALSE;
 ip->i_pndsplit = FALSE;
 ip->isym = NULL;
 ip->imsym = NULL;
 ip->ipxprtab = NULL;
 ip->iattrs = NULL;
 ip->ipins = NULL;
 ip->pb_ipins_tab = NULL;
}

/*
 * merge attributes from mod into inst
 *
 * only called if both inst and mod have at least one attr
 * if same attr name in both use inst value
 * if name only in mod add to inst list so can use list to make iterator  
 */
static void add_mod_attrs_toinst(struct inst_t *ip)
{
 register struct attr_t *iattrp, *mattrp, *iattr_end;
 struct attr_t *new_iattrp, *new_iattrhd, *new_iattrend;
 struct mod_t *mdp;

 new_iattrhd = new_iattrend = NULL;
 iattr_end = NULL;
 for (iattrp = ip->iattrs; iattrp != NULL; iattrp = iattrp->attrnxt) 
  { iattr_end = iattrp; }
 mdp = ip->imsym->el.emdp;

 for (mattrp = mdp->mattrs; mattrp != NULL; mattrp = mattrp->attrnxt) 
  {
   for (iattrp = ip->iattrs; iattrp != NULL; iattrp = iattrp->attrnxt)
    {
     /* attribute attached to both inst and mod - use inst value */
     if (strcmp(mattrp->attrnam, iattrp->attrnam) == 0) goto nxt_mod_attr;
    }
   /* attr attached to mod but not inst and inst has at least one attr */ 
   /* add mod attr to inst */
   new_iattrp = (struct attr_t *) __my_malloc(sizeof(struct attr_t));
   *new_iattrp = *mattrp;
   new_iattrp->attrnam = __pv_stralloc(mattrp->attrnam);
   new_iattrp->attr_xp = __copy_expr(mattrp->attr_xp);
   new_iattrp->attrnxt = NULL;
   if (new_iattrhd == NULL) new_iattrhd = new_iattrp;
   else new_iattrend->attrnxt = new_iattrp;
   new_iattrend = new_iattrp;
nxt_mod_attr:;
  }
 /* possible for all to be duplicated so none to add */
 if (iattr_end != NULL && new_iattrhd != NULL)
  iattr_end->attrnxt = new_iattrhd;
}

/*
 * build expr table from named param list
 *
 * know instance has pound param(s) or routine not called 
 * list needed because must be able to index pound parameters when building
 * arrays of instance 
 *
 * this assumes module pound params converted to table from list
 * happens just before cell rep to inst for each module
 *
 * when routine done no longer need nqc_u union (i.e. done with npi)
 */
static struct expr_t **inst_nparms_to_xtab(struct namparam_t *npmphdr,
 struct inst_t *ip)
{
 int32 all_named;
 struct expr_t **npxtab;

 /* new explicitly named form */
 if (pndparams_explicit(npmphdr, &all_named))
  {
   if (!all_named)
    {
     __gferr(829, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
      "mixed implicit and explicit # parameters illegal for instance %s",
      ip->isym->synam);
     __free_namedparams(npmphdr);
     return(NULL);
    }
   npxtab = match_namparam_exprtab(npmphdr, ip);
   if (npxtab != NULL) ip->pprm_explicit = TRUE;
  }
 else
  {
   /* old unnamed list - but ,, allowed and becomes nil (no param) */
   npxtab = match_implprm_exprtab(npmphdr, ip);
  }
 __free_namedparams(npmphdr);
 return(npxtab);
} 

/*
 * routine to build inst param expr table from named param list
 *
 * by here module definition pound parameters converted to table
 * param nu2 field union member npi used here as temp, can reuse after here 
 */
static struct expr_t **match_namparam_exprtab(struct namparam_t *npmphdr,
 struct inst_t *ip)
{
 register int32 pi;
 register struct namparam_t *npmp;
 struct mod_t *mdp;
 struct net_t *np, **prmtab;
 struct expr_t **npxtab;
 struct sy_t *syp;
 char s1[2*IDLEN];

 sprintf(s1, "instance %s", ip->isym->synam); 

 mdp = ip->imsym->el.emdp;
 if (mdp->mprmnum == 0)
  {
   __gferr(865, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
    "%s has explicit pound parameters but none declared in module %s",
    s1, mdp->msym->synam);
   return(NULL);
  }

 /* build sorted by name param declaration list */
 prmtab = (struct net_t **) __my_malloc(mdp->mprmnum*sizeof(struct net_t *));
 for (pi = 0; pi < mdp->mprmnum; pi++)
  {
   prmtab[pi] = &(mdp->mprms[pi]);
   /* need original ndx of param (net) in table so can set after sorting */
   prmtab[pi]->nu2.npi = pi;
  }
 qsort((char *) prmtab, (word32) mdp->mprmnum, sizeof(struct net_t *),
  prmtab_cmp); 
 /* allocate inst named param expr ptr table and set to unuused (ni) */
 npxtab = (struct expr_t **) __my_malloc(mdp->mprmnum*sizeof(struct expr_t *));
 for (pi = 0; pi < mdp->mprmnum; pi++) npxtab[pi] = NULL;

 for (npmp = npmphdr, pi = 0; npmp != NULL; npmp = npmp->nprmnxt, pi++)
  {
   /* DBG remove --- */
   if (npmp->pnam == NULL) __misc_terr(__FILE__, __LINE__); 
   /* --- */
   
   syp = __zget_sym(npmp->pnam, mdp->msymtab->stsyms, mdp->msymtab->numsyms);
   if (syp == NULL || syp->sytyp != SYM_N)
    {
undcl_param:
     __gferr(865, npmp->prmfnam_ind, npmp->prmlin_cnt,
      "%s explicit pound parameter %s (pos. %d) not a declared parameter",
      s1, npmp->pnam, pi);
     /* if no declared parameters, always continue here */
     continue; 
    }
   np = syp->el.enp;
   if (!np->n_isaparam || np->nu.ct->p_specparam) goto undcl_param;

   /* AIV 09/27/06 - net cannot be a local param value cannot be overridden */
   if (np->nu.ct->p_locparam)
    {
     __gferr(3431, npmp->prmfnam_ind, npmp->prmlin_cnt,
      "%s explicit pound parameter %s (pos. %d) cannot be a localparam (declared pos. %d)",
      s1, npmp->pnam, pi, np->nu2.npi);
     continue;
    }

   if (npxtab[np->nu2.npi] != NULL)
    {
     __gferr(935, npmp->prmfnam_ind, npmp->prmlin_cnt,
      "%s explicit pound parameter %s (pos. %d) repeated (declared pos. %d)",
      s1, npmp->pnam, pi, np->nu2.npi);
     continue;
    }
   /* empty .[pnam]() form legal and becomes nil with warning */
   if (npmp->pxndp->optyp == OPEMPTY)
    {
     __gfwarn(545, npmp->prmfnam_ind, npmp->prmlin_cnt,
      "%s explicit pound parameter %s expression empty (pos. %d) - no pound override",
      s1, npmp->pnam, pi);
     continue;
    }
   /* on error (returns F), leave location in npx tab nil */
   if (chk1_pndparam(s1, ip, npmp, pi, np->nu2.npi))
    {
     npxtab[np->nu2.npi] = npmp->pxndp;
     /* this needed to prevent freeing when namparams freed */
     npmp->pxndp = NULL;
    }
  }
 /* if all are errors - i.e. table or none decled still empty return nil */
 for (pi = 0; pi < mdp->mprmnum; pi++)
  { if (npxtab[pi] != NULL) goto some_good; }
 return(NULL);

some_good:
 return(npxtab);
}

/*
 * compare for sort of table of ptrs order module parameter table
 */
static int32 prmtab_cmp(const void *np1, const void *np2)
{
 return(strcmp((*((struct net_t **) np1))->nsym->synam,
 ((*(struct net_t **) np2))->nsym->synam));
}

/*
 * check one pound param to make sure parameter constant expression
 *
 * if error, return -1 so caller does not add to ipx table
 * folding done later
 */
static int32 chk1_pndparam(char *s1, struct inst_t *ip, struct namparam_t *npmp,
 int32 pi, int32 npi)
{
 int32 rv, sav_sfnam_ind, sav_slin_cnt;
 struct mod_t *imdp;
 struct net_t *parm_np;

 /* SJM 10/01/99 - improve error location for param checking */
 /* chk param expr needs sim locations set - set temporary guess here */
 sav_sfnam_ind = __sfnam_ind;
 sav_slin_cnt = __slin_cnt;
 __sfnam_ind = npmp->prmfnam_ind;
 __slin_cnt = npmp->prmlin_cnt;
 
 imdp = ip->imsym->el.emdp;
 parm_np = &(imdp->mprms[npi]);

 /* notice must check #(parms) rhs here since can be any parameter*/
 /* even one defined later in source according to LRM */
 if (!__chk_paramexpr(npmp->pxndp, 0))
  {
   __sgferr(753,
    "%s pound parameter %s (pos. %d) expression %s only parameters and constants - is parameter from other module?",
    s1, parm_np->nsym->synam, pi, __msgexpr_tostr(__xs, npmp->pxndp));
   rv = FALSE;
  }
 else rv = TRUE;

 __sfnam_ind = sav_sfnam_ind;
 __slin_cnt = sav_slin_cnt;
 return(rv);
}

/*
 * routine to build inst param expr table from implicit unnamed # param list
 *
 * no error possible here
 */
static struct expr_t **match_implprm_exprtab(struct namparam_t *npmphdr,
 struct inst_t *ip)
{
 register int32 pi;
 register struct namparam_t *npmp;
 struct mod_t *mdp;
 struct expr_t **npxtab;
 char s1[2*IDLEN];

 /* can never see connector parameters here */
 sprintf(s1, "instance %s", ip->isym->synam); 

 mdp = ip->imsym->el.emdp;
 /* FIXME - SJM 06/01/99 - if syntax error and no mod params */
 /* was wrongly allocating 0 size array here */
 /* notice 1 bit enough since once malloced exits pound param loop */
 if (mdp->mprmnum == 0)
  {
   npxtab = (struct expr_t **) __my_malloc(1*sizeof(struct expr_t *));
   npxtab[0] = NULL;
  }
 else
  {
   /* allocate inst named param expr ptr table and set to unuused (ni) */
   npxtab = (struct expr_t **)
    __my_malloc(mdp->mprmnum*sizeof(struct expr_t *));
  }
 for (pi = 0; pi < mdp->mprmnum; pi++) npxtab[pi] = NULL;

 /* unnamed so must match by position */
 for (npmp = npmphdr, pi = 0; npmp != NULL; npmp = npmp->nprmnxt, pi++)
  {
   /* too few ok, but too many is error */
   if (pi >= mdp->mprmnum) 
    {
     /* need to count all in list */
     for (; npmp != NULL; npmp = npmp->nprmnxt) pi++;

     __gferr(752, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
      "%s # parameter list too long (%d) - only %d parameters declared",
      s1, pi, mdp->mprmnum); 
     break;
    }
   /* because now translating like ports allowing ,, that become nil */
   if (npmp->pxndp->optyp == OPEMPTY) continue;
   if (chk1_pndparam(s1, ip, npmp, pi, pi))
    {
     npxtab[pi] = npmp->pxndp;
     npmp->pxndp = NULL;
    }
  }
 /* if list short, tail will be correctly nil but need inform */
 if (pi < mdp->mprmnum)
  {
   __gfinform(408, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
    "%s only first %d of %d pound parameters set", s1, pi + 1, mdp->mprmnum);
  }
 return(npxtab); 
}

/*
 * ROUTINES TO ANALYZE INSTANTIATION CONNECTIVITY STRUCTURE
 */

/*
 * rearrange and build module and inst. connections for further processing
 * also set dag levels and connect inst. pins to module ports
 *
 * parameter values still unknown but this builds global strength anal. and
 * splitting info
 */
static int32 fix_modcell_nl(void)
{
 register struct mod_t *mdp;

 /* first need to know static instantiations */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   /* F for self instantiation and cannot continue */
   if (!count_static_instances(mdp)) return(TRUE);

   __pop_wrkitstk();
  }

 /* set dag levels */
 if (!chk_dag()) return(FALSE);
 return(TRUE);
}

/*
 * distinguish number of times module statically instantiated
 * 0 means top, 1 exactly once, 2 more than once
 * value here is static unflattened number of instantiations
 */
static int32 count_static_instances(struct mod_t *mdp)
{
 register int32 ii; 
 register struct inst_t *ip;
 struct sy_t *syp;
 struct mod_t *imdp;

 for (ii = 0; ii < mdp->minum; ii++)
  {
   ip = &(mdp->minsts[ii]);
   syp = ip->imsym;
   imdp = syp->el.emdp;
   if (mdp == imdp)
    __gfterr(312, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
     "INTERNAL - module instantiates itself");
   /* only need counts of 0 (never), 1 (exactly once), 2 (more than once) */
   if (imdp->minstnum == 0) imdp->minstnum = 1;
   else if (imdp->minstnum == 1) imdp->minstnum = 2;
  }
 return(TRUE);
}

/*
 * check to see if any instantiation loops and set levelized level
 * for multiple instantiations level is level of lowest one
 */
static int32 chk_dag(void)
{
 register int32 ii;
 register struct mod_t *mdp;
 int32 i, change, allmark;
 struct inst_t *ip;
 struct mod_t *imdp;

 __dagmaxdist = 0;
 /* previously initialized to -1 (unmarked) */
 for (allmark = TRUE, mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->minsts == NULL) mdp->mlpcnt = 0;
   else allmark = FALSE;
  }
 if (allmark) return(TRUE);
 /* level 0 means no contained instances, mark all level i modules */
 for (i = 1; i < MAXCIRDPTH; i++)
  {
   change = FALSE;
   /* assume all marked */
   allmark = TRUE;
   for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
    {
     /* if marked nothing to do */
     if (mdp->mlpcnt != -1) continue;

     for (ii = 0; ii < mdp->minum; ii++)
      {
       ip = &(mdp->minsts[ii]);
       imdp = ip->imsym->el.emdp;
       /* cannot mark if unmarked or mark this time == i */
       if (imdp->mlpcnt == -1 || imdp->mlpcnt == i)
	{
         /* ---
	 if (__debug_flg)
          {
  	   __dbg_msg("++at level %d cannot mark %s because of %s\n", i,
	     mdp->msym->synam, imdp->msym->synam);
          }
         ---- */
 	 goto cannot_mark;
	}
      }
     mdp->mlpcnt = i;
     /* ---
     if (__debug_flg)
      __dbg_msg("++at level %d marking %s\n", i, mdp->msym->synam);
     --- */
     change = TRUE;
     continue;

cannot_mark:
     allmark = FALSE;
    }
   if (allmark) { __dagmaxdist = i; return(TRUE); }
   if (!change) break;
  }
 __pv_err(735,
  "design contains indirect (length > 1) self instantiation loop");
 return(FALSE);
}

/*
 * ROUTINES TO CONNECT PORTS
 */

/*
 * check module ports and make instance connections
 * handles implicit and explicit form port matching
 */
static void fix_port_conns(void)
{
 register struct mod_t *mdp;

 /* set implicit module port form names and I/O directions */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);
   setchk_modports();
   __pop_wrkitstk();
  }

 /* connect module ports and change to instance port expr. list */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);
   conn_mod_insts();
   __pop_wrkitstk();
  }

 /* finally can free any sorted work mod port pointer arrays */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->smpins == NULL) continue;
   __my_free((char *) mdp->smpins, mdp->mpnum*sizeof(struct srtmp_t));
   mdp->smpins = NULL;
  }
}

/*
 * set and check module header port connections
 * needed because .[port]([expr]), legal, unnamed ports legal, and
 * repeated nets legal
 *
 * since wire width not known yet, structural checking here only, widths
 * set and checked later
 */
static void setchk_modports(void)
{
 register int32 pi;
 register struct mod_pin_t *mpp;
 int32 pnum, port_gd;

 if ((pnum = __inst_mod->mpnum) == 0) return;
 for (pi = 0; pi < pnum; pi++)
  {
   mpp = &(__inst_mod->mpins[pi]);
   __pr_iodir = IO_UNKN;
   port_gd = chk_prtref(mpp->mpref, mpp, TRUE);
   mpp->mptyp = __pr_iodir;

   /* notice empty and concatenate port lists remain unnamed */
   if (port_gd)
    {
     /* SJM 08/23/00 - now OPEMPTY ports also legal and unnamed */
     if (mpp->mpsnam == NULL && mpp->mpref->optyp != LCB
      && mpp->mpref->optyp != OPEMPTY)
      {
       if (mpp->mpref->optyp == ID) mpp->mpsnam = mpp->mpref->lu.sy->synam;
       else mpp->mpsnam = mpp->mpref->lu.x->lu.sy->synam;
      }
    }
   else
    {
     __free_xtree(mpp->mpref);
     __bld_unc_expr();
     mpp->mpref = __root_ndp;
    }
  }
}

/*
 * check a module port def. expression to make sure it is a prtref
 * return F on error else T
 * prtref is lhs value or concatenate (including nested) of port refs.
 * also checks and sets I/O direction
 * this is called before wire and port ranges known
 */
static int32 chk_prtref(struct expr_t *ndp, struct mod_pin_t *mpp,
 int32 is_top)
{
 struct net_t *np;
 struct expr_t *idndp;
 char s1[RECLEN], s2[RECLEN];

 switch ((byte) ndp->optyp) {
  case GLBREF:
bad_prtglb:
   __gferr(736, mpp->mpfnam_ind, mpp->mplin_cnt,
    "module header list of ports hierarchical path reference %s illegal",
     ndp->ru.grp->gnam);
   return(FALSE);
  case ID:
   /* when ID appears in mod. def. port header, converted to undecl net */ 
   np = ndp->lu.sy->el.enp;
   idndp = ndp;
chk_dir:
   if (!idndp->lu.sy->sydecl)
    {
     __gferr(737, mpp->mpfnam_ind, mpp->mplin_cnt,
      "I/O port %s required direction declaration missing", np->nsym->synam);
     return(FALSE);
    }
   if (np->iotyp == NON_IO)
    {
     __gferr(738, mpp->mpfnam_ind, mpp->mplin_cnt,
      "module header list of ports %s %s must be I/O port",
       __to_wtnam(s1, np), np->nsym->synam);
     return(FALSE);
    }
   if (__pr_iodir == IO_UNKN) __pr_iodir = np->iotyp;
   else if (__pr_iodir != np->iotyp)
    {
     __gferr(739, mpp->mpfnam_ind, mpp->mplin_cnt,
     "module header list of ports %s wire %s has conflicting %s port type",
      __to_ptnam(s1, np->iotyp), np->nsym->synam, __to_ptnam(s2, __pr_iodir));
     return(FALSE);
    }
   /* count times wire repeated in port list */
   if (np->nu.ct->num_prtconns != 2 && is_top) np->nu.ct->num_prtconns += 1;

   /* notice in and inout ports must be wires but out can be reg */
   if (!chk_prtntyp(mpp, np)) return(FALSE);
   break;
  case PARTSEL:
   /* since check module port syntax before splitting, mark params that */
   /* can effect expr. width here to later possibly cause split not IS form */
   __in_xpr_markparam(ndp->ru.x->lu.x);
   __in_xpr_markparam(ndp->ru.x->ru.x);
   /* FALLTHRU */
  case LSB:
   if (ndp->lu.x->optyp == GLBREF) goto bad_prtglb;
   idndp = ndp->lu.x;
   np = idndp->lu.sy->el.enp;
   /* inout port expr. may need to go in tran channel so can't be IS form */
   if (np->iotyp == IO_BID) __in_xpr_markparam(ndp->ru.x);
   goto chk_dir;
  case LCB:
   if (!is_top)
    {
     __gferr(740, mpp->mpfnam_ind, mpp->mplin_cnt,
      "module header list of ports nested concatenate illegal");
     return(FALSE);
    }
   {
    register struct expr_t *ndp2;

    /* ndp2 left can still be nested concatenate by here */
    /* but can find rep. number */
    for (ndp2 = ndp->ru.x; ndp2 != NULL; ndp2 = ndp2->ru.x)
     {
      /* check I/O direction from subcomponents one down */
      /* also check for incorrect reg one down */
      if (!chk_prtref(ndp2->lu.x, mpp, FALSE)) return(FALSE);
     }
   }
   break;
  case CATREP:
   __gferr(741, mpp->mpfnam_ind, mpp->mplin_cnt,
    "module header list of ports port ref. concatenate repeat form illegal");
   return(FALSE);
  case OPEMPTY:
   break;
  default:
   __gferr(742, mpp->mpfnam_ind, mpp->mplin_cnt,
    "%s illegal module header list of ports port reference",
    __msgexpr_tostr(__xs, ndp));
   return(FALSE);
 }
 return(TRUE);
}

/*
 * check a port wire to make sure net type legal
 */
static int32 chk_prtntyp(struct mod_pin_t *mpp, struct net_t *np)
{
 char s1[RECLEN], s2[RECLEN];

 if (np->iotyp == IO_OUT)
  {
   if (np->ntyp == N_REAL)
    {
     __gferr(743, mpp->mpfnam_ind, mpp->mplin_cnt,
      "%s port %s type real illegal", __to_ptnam(__xs, np->iotyp),
      np->nsym->synam);
     return(FALSE);
    }
  }
 else
  {
   if (np->ntyp >= NONWIRE_ST)
    {
     __gferr(744, mpp->mpfnam_ind, mpp->mplin_cnt,
      "%s port %s of net type %s illegal - must be a wire",
       __to_ptnam(s1, np->iotyp), np->nsym->synam, __to_wtnam(s2, np));
     return(FALSE);
    }
  }
 return(TRUE);
}

/*
 * check/connect cell pins to defined down one module
 * no processing of gates here
 */
static void conn_mod_insts(void)
{
 register int32 ii;
 register struct cell_pin_t *cpp;
 int32 nbytes;
 struct inst_t *ip;
 struct sy_t *syp;
 struct mod_t *imdp;
 struct expr_t **xphdr;
 struct srcloc_t *srcloc;

 if (__inst_mod->minum == 0) return;
 
 /* allocate the || to minsts per contained inst pin src loc table */  
 nbytes = __inst_mod->minum*sizeof(struct srcloc_t *);
 __inst_mod->iploctab = (struct srcloc_t **) __my_malloc(nbytes);

 for (ii = 0; ii < __inst_mod->minum; ii++)
  {
   ip = &(__inst_mod->minsts[ii]);
   syp = ip->imsym;
   /* module instance declaration info lost */
   if (!syp->sydecl)
    __misc_gfterr(__FILE__, __LINE__, ip->isym->syfnam_ind,
     ip->isym->sylin_cnt);
   imdp = ip->imsym->el.emdp;

   /* instance must contain at least () which will cause 1 unc. pin */
   cpp = (struct cell_pin_t *) ip->ipins;
   /* DBG remove --- */
   /* know there will be at least one pin if no error */
   if (cpp == NULL && imdp->mpnum != 0)
    {
     if (__pv_err_cnt == 0) __misc_terr(__FILE__, __LINE__); else continue;
    }
   /* --- */

   /* allocate new expression table */
   /* special case of module with no ports instantiated as () */
   /* it will have one unc. port - legal for 1 port case */
   /* but since no module ports this special port must be thrown away */
   if (imdp->mpnum == 0)
    {
     ip->ipins = NULL;
     srcloc = NULL;
     /* only set to NULL if has 1 unc. port - else will be null */
     /* LOOKATME - not freeing case of one unc. port */  
     if (cpp == NULL || (cpp->cpnxt == NULL && cpp->cpxnd->optyp == OPEMPTY))
      goto nxt_cell;

     __gferr(746, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
      "instance %s module type %s has ports but module does not",
       ip->isym->synam, imdp->msym->synam);
     goto nxt_cell;
    }
   nbytes = imdp->mpnum*sizeof(struct expr_t *);
   xphdr = (struct expr_t **) __my_malloc(nbytes);
   nbytes = imdp->mpnum*sizeof(struct srcloc_t);
   srcloc = (struct srcloc_t *) __my_malloc(nbytes);

   /* allocate cell-pin expression table */
   /* all implicit connection case */
   if (cpp->pnam == NULL) conn_impl_mports(ip, cpp, imdp, xphdr, srcloc);
   /* all explicit connection case */
   else conn_expl_mports(ip, cpp, imdp, xphdr, srcloc);
   ip->ipins = xphdr;

nxt_cell:

   /* must connect to current mod since need port locs for expr check */
   /* since parallel to insts, for insts. with no cell pin list will be nil */ 
   __inst_mod->iploctab[ii] = srcloc;
  }
}

/*
 * connect implicit instance ports
 * mdp is instance module type
 * various checking goes here
 * ,, form here already made appropriate unc. 
 * 
 * know module port num at least one or not called 
 */
static void conn_impl_mports(struct inst_t *ip, struct cell_pin_t *cpp,
 struct mod_t *mdp, struct expr_t **xphdr, struct srcloc_t *srclocp)
{
 register int32 pi;
 struct cell_pin_t *last_cpp;
 int32 pnum, num_unc_ports;

 ip->ip_explicit = FALSE;
 pnum = mdp->mpnum;
 num_unc_ports = 0;
 for (pi = 0; pi < pnum;)
  {
   if (cpp->pnam != NULL)
    {
     __gferr(747, cpp->cpfnam_ind, cpp->cplin_cnt,
     "%s(%s) explicit port %s (pos. %d) mixed in implicit connection list",
      ip->isym->synam, ip->imsym->synam, cpp->pnam, pi + 1);
    }
   /* SJM 11/13/00 - now also emit warning if ,, unc. form in implicit list */
   if (cpp->cpxnd->optyp == OPEMPTY) num_unc_ports++;
   xphdr[pi] = cpp->cpxnd;
   /* here implicit means port in order def. i and inst i same */ 
   srclocp[pi].sl_fnam_ind = cpp->cpfnam_ind;
   srclocp[pi].sl_lin_cnt = cpp->cplin_cnt;
   last_cpp = cpp;
   if ((cpp = cpp->cpnxt) == NULL && pi < pnum - 1) goto too_few;
   pi++;
  }
 if (cpp != NULL)
  {
   __gferr(748, cpp->cpfnam_ind, cpp->cplin_cnt,
    "%s(%s) implicit connection list more ports than type with %d",
    ip->isym->synam, ip->imsym->synam, pnum);
  }
 if (num_unc_ports > 0)
  {
   __gfwarn(531, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
    "%s(%s) implicit connection list fewer ports %d connected than type's %d",
    ip->isym->synam, ip->imsym->synam, pnum - num_unc_ports, pnum);
  }
 return;

too_few:
 pi++;
 /* warn because ,,s should be included */
  {
   __gfwarn(531, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
    "%s(%s) implicit connection list fewer ports %d in list than type's %d",
    ip->isym->synam, ip->imsym->synam, pi, pnum);
  }
 for (; pi < pnum; pi++)
  {
   __bld_unc_expr(); 
   xphdr[pi] = __root_ndp;
   /* for missing use last or if no conn use isym loc. */
   if (last_cpp == NULL)
    {
     srclocp[pi].sl_fnam_ind = ip->isym->syfnam_ind;
     srclocp[pi].sl_lin_cnt = ip->isym->sylin_cnt;
    }
   else
    {
     srclocp[pi].sl_fnam_ind = last_cpp->cpfnam_ind;
     srclocp[pi].sl_lin_cnt = last_cpp->cplin_cnt;
    }
  }
}

/*
 * connect explicit instance connection form ports
 *
 * builds ipins table (of prts to exprs) size is always num mod ports
 * even if some unconnected (will be unc. expr)
 *
 * know module port num at least one or not called 
 */
static void conn_expl_mports(struct inst_t *ip, struct cell_pin_t *cpp,
 struct mod_t *imdp, struct expr_t **xphdr, struct srcloc_t *srclocp)
{
 register int32 pi, mpi;
 register int32 cpnum, pnum;
 register int32 cv;
 int32 num_unc_ports;
 struct cell_pin_t **srtcptab;
 struct srtmp_t *srtmptab;
 struct cell_pin_t *scpp;
 struct srtmp_t *smpp;

 ip->ip_explicit = TRUE;
 /* if module has unnamed ports, cannot continue */
 if (!chk_mdports_named(ip, imdp, xphdr)) return;

 /* if no sorted port list for this module type, build one */
 if (imdp->smpins == NULL) bld_srted_mdpins(imdp);
 srtmptab = imdp->smpins;

 /* build the sorted cell pin table for instance */
 pnum = imdp->mpnum; 
 for (scpp = cpp, cpnum = 0; scpp != NULL; scpp = scpp->cpnxt) cpnum++;
 srtcptab = (struct cell_pin_t **)
  __my_malloc(cpnum*sizeof(struct cell_pin_t *));
 if (!bld_srted_ipins(ip, cpp, cpnum, srtcptab)) goto free_pin_tab;

 pi = 0; scpp = srtcptab[0];
 mpi = 0; smpp = &(srtmptab[0]);
 /* know both lists sorted so move through */
 /* if past end of either done - if no match, left as nil, made unc. */ 
 for (; pi < cpnum && mpi < pnum;)
  {
   /* scpp is cell (inst.) port and smpp is module port */
   if ((cv = strcmp(scpp->pnam, smpp->smp->mpsnam)) == 0)
    {
     xphdr[smpp->mppos] = scpp->cpxnd;
     /* index here is type def. order, def. i line no. if for i conn. */ 
     srclocp[smpp->mppos].sl_fnam_ind = scpp->cpfnam_ind; 
     srclocp[smpp->mppos].sl_lin_cnt = scpp->cplin_cnt;
     /* if at end of instance ports, done */
     pi++; if (pi < cpnum) scpp = srtcptab[pi];
     mpi++; if (mpi < pnum) smpp = &(srtmptab[mpi]);
     continue;
    }
   /* module instance port extra or repeated - fits between type ports */
   /* cell port alphabetically less than module port - is non mod port */
   /* becasue both sorted - if repeated, moved to next mod port so same */
   /* cell port now less than */
   if (cv < 0)
    {
     if (pi > 0 && strcmp(srtcptab[pi - 1]->pnam, scpp->pnam) == 0) 
      {
       __gferr(745, scpp->cpfnam_ind, scpp->cplin_cnt,
        "%s(%s) explicit connection port %s repeated",
        ip->isym->synam, imdp->msym->synam, scpp->pnam);
      } 
     else __gferr(749, scpp->cpfnam_ind, scpp->cplin_cnt,
      "%s(%s) explicit connection port %s is not a module port",
      ip->isym->synam, imdp->msym->synam, scpp->pnam);
     pi++;
     if (pi < cpnum) scpp = srtcptab[pi];
     continue;
    }
   /* cell port alphabetically greater than module port */
   /* module type port is extra (unc.) - fits between module inst ports */
   mpi++; if (mpi < pnum) smpp = &(srtmptab[mpi]);
  }
 /* if more cell ports, know are non port - emit error */
 for (; pi < cpnum; pi++)
  {
   scpp = srtcptab[pi];
   __gferr(749, scpp->cpfnam_ind, scpp->cplin_cnt,
    "%s(%s) explicit connection port %s is not a module port",
    ip->isym->synam, imdp->msym->synam, scpp->pnam);
  }  
free_pin_tab:
 __my_free((char *) srtcptab, cpnum*sizeof(struct cell_pin_t *));
 srtcptab = NULL;

 /* for all unc. (NULL) expressions convert to real unconnected */
 num_unc_ports = 0;
 for (pi = 0; pi < pnum; pi++)
  {
   if (xphdr[pi] == NULL)
    {
     __bld_unc_expr();
     xphdr[pi] = __root_ndp;
     /* use instance location for these */
     srclocp[pi].sl_fnam_ind = ip->isym->syfnam_ind; 
     srclocp[pi].sl_lin_cnt = ip->isym->sylin_cnt;
     num_unc_ports++;
    }
  }
 if (num_unc_ports > 0)
  {
   __gfwarn(531, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
    "%s(%s) explicit connection list fewer ports %d connected than type's %d",
    ip->isym->synam, ip->imsym->synam, pnum - num_unc_ports, pnum);
  }
}

/*
 * check to make sure for explicit instance form all module ports named 
 * also initialized instance port expression table to NULL
 * only called for explicit conn. instances
 */
static int32 chk_mdports_named(struct inst_t *ip,
 struct mod_t *mdp, struct expr_t **xphdr)
{
 register int32 pi;
 register struct mod_pin_t *mpp;
 int32 pnum, __err_seen;

 if ((pnum = mdp->mpnum) == 0) return(FALSE);

 __err_seen = FALSE;
 for (pi = 0; pi < pnum; pi++)
  {
   mpp = &(mdp->mpins[pi]);
   if (mpp->mpsnam == NULL)
    {
     __gferr(750, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
      "%s(%s) explicit connection form illegal - port %d unnamed",
     ip->isym->synam, mdp->msym->synam, pi + 1);
     __err_seen = TRUE;
    }
   xphdr[pi] = NULL;
  }
 return(!__err_seen);
}

/*
 * build sorted table of module ports - once built saved so not rebuilt
 * each time - freed when port connection phase completed
 * only called when know all module ports named and at least 1 port
 */
static void bld_srted_mdpins(struct mod_t *mdp)
{
 register int32 pi; 
 register struct srtmp_t *smpp;
 register struct mod_pin_t *mpp;
 struct srtmp_t *smptab;  
 int32 pnum;

 pnum = mdp->mpnum;
 smptab = (struct srtmp_t *) __my_malloc(pnum*sizeof(struct srtmp_t)); 
 mdp->smpins = smptab;
 mpp = &(mdp->mpins[0]);
 for (pi = 0, smpp = &(smptab[0]); pi < pnum; pi++, smpp++, mpp++)
  {
   smpp->smp = mpp;
   smpp->mppos = pi;
  }
 qsort((char *) smptab, (word32) pnum, sizeof(struct srtmp_t), smp_cmp); 
}

/*
 * module port name comparison routine
 */
static int32 smp_cmp(const void *srp1, const void *srp2)
{
 return(strcmp(((struct srtmp_t *) srp1)->smp->mpsnam,
 ((struct srtmp_t *) srp2)->smp->mpsnam));
}

/*
 * build sorted table of instance ports
 * built for each explicitly connected instance
 * free when instance connections made
 */
static int32 bld_srted_ipins(struct inst_t *ip,
 register struct cell_pin_t *cpp, int32 pnum, struct cell_pin_t **scptab)
{
 register int32 pi; 
 register struct cell_pin_t *cpp2;
 char *chp;

 for (pi = 0; pi < pnum; pi++, cpp = cpp->cpnxt)
  {
   if (cpp->pnam == NULL)
    {
     __gferr(751, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
      "%s(%s) explicit connection list but port connection %d is implicit",
      ip->isym->synam, ip->imsym->synam, pi + 1);
     return(FALSE);
    }
   scptab[pi] = cpp;
  }
 qsort((char *) scptab, (word32) pnum, sizeof(struct cell_pin_t *), cpn_cmp); 

 /* must check for duplicate of same name since illegal */
 /* LOOKATME - think none impossible but doing nothing is right if happens */ 
 if (pnum <= 0) return(TRUE);

 chp = scptab[0]->pnam; 
 for (pi = 1; pi < pnum; pi++)
  {
   cpp2 = scptab[pi];
   if (strcmp(chp, cpp2->pnam) == 0)
    {
     __gferr(1038, cpp2->cpfnam_ind, cpp2->cplin_cnt,
      "%s(%s) explicit connection list named port %s repeated",
      ip->isym->synam, ip->imsym->synam, chp);
    }
   else chp = scptab[pi]->pnam;
  }
 return(TRUE);
}

/*
 * module port name comparison routine
 */
static int32 cpn_cmp(const void *cpp1, const void *cpp2)
{
 return(strcmp((*((struct cell_pin_t **) cpp1))->pnam,
  (*(struct cell_pin_t **) cpp2)->pnam));
}

/*
 * routine to free all allocated ncmp blks when no longed used at all
 */
static void free_cpblks(void)
{
 register struct cpblk_t *cpbp, *cpbp2;

 /* free all cell pin blks since ncomp form now gone */
 for (cpbp = __hdr_cpblks; cpbp != NULL;)
  {
   cpbp2 = cpbp->cpblknxt;
   __my_free((char *) cpbp->cpblks, BIG_ALLOC_SIZE);
   __my_free((char *) cpbp, sizeof(struct cpblk_t));
   cpbp = cpbp2;
  }
}

/*
 * routine to free all allocated ncmp blks when no longed used at all
 */
static void free_cppblks(void)
{
 register struct cppblk_t *cppbp, *cppbp2;
 register struct cpnblk_t *cpnbp, *cpnbp2;

 /* free all cell pin blks since ncomp form now gone */
 for (cppbp = __hdr_cppblks; cppbp != NULL;)
  {
   cppbp2 = cppbp->cppblknxt;
   __my_free((char *) cppbp->cppblks, BIG_ALLOC_SIZE);
   __my_free((char *) cppbp, sizeof(struct cppblk_t));
   cppbp = cppbp2;
  }
 for (cpnbp = __hdr_cpnblks; cpnbp != NULL;)
  {
   cpnbp2 = cpnbp->cpnblknxt;
   __my_free((char *) cpnbp->cpnblks, BIG_ALLOC_SIZE);
   __my_free((char *) cpnbp, sizeof(struct cpnblk_t));
   cpnbp = cpnbp2;
  }
}

/*
 * set the as if flattended instance counts for all modules
 * know flat insts initialized to 0 and m insts already counted
 * this also links every module inst. upward (parent) instance
 *
 * count here counts only 1 for each cell array - fixed up later
 */
static void count_flat_insts(void)
{
 register int32 ii;
 register struct mod_t *mdp;
 struct inst_t *ip;
 struct mod_t *imdp;

 /* must always reinitialize flat count because maybe called multiple times */ 
 /* i.e. after arrays of instances expanded */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  mdp->flatinum = 0;

 /* inc. count of all instances in each top level module */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   /* descend to count only from top level modulss */
   if (mdp->minstnum != 0) continue;
   mdp->flatinum = 1;

   /* count depth first */
   for (ii = 0; ii < mdp->minum; ii++)
    {
     ip = &(mdp->minsts[ii]);
     imdp = ip->imsym->el.emdp;
     imdp->flatinum++;

     /* DBG remove ---
     if (__debug_flg)
      {
       __dbg_msg(
        "+++ incing count (%d) of inst. %s type %s in top level module %s\n",
        imdp->flatinum, ip->isym->synam, imdp->msym->synam,
        __inst_mod->msym->synam);
      }
     ---- */ 
     count2_flat_insts(imdp);
    }
  }
}

/*
 * traverse 1 down flattened inst. count
 */
static void count2_flat_insts(struct mod_t *mdp)
{
 register int32 ii;
 register struct inst_t *ip;
 struct mod_t *imdp;

 for (ii = 0; ii < mdp->minum; ii++)
  {
   ip = &(mdp->minsts[ii]); 
   imdp = ip->imsym->el.emdp;
   imdp->flatinum++;
   /* DBG remove ---
   if (__debug_flg)
    { 
     __dbg_msg("+++ traverse inc count (%d) inst %s type %s in mod %s\n",
      imdp->flatinum, ip->isym->synam, imdp->msym->synam, mdp->msym->synam);
    }
   --- */
   if (imdp->mlpcnt > 0) count2_flat_insts(imdp);
  }
}

/*
 * ROUTINES TO SPLIT INSTS, REBUILD AND EXPAND FOR GATE/INST ARRAYS 
 */

/*
 * split (copy) module types for which pound params used in arrays of
 * gates or instances declaration width ranges
 */
static void do_giarr_splitting(void)
{
 register int32 ii, mlevel;
 register struct mod_t *mdp; 
 struct inst_t *ip;
 struct mod_t *imdp;

 /* start will all modules that contain instances only of modules */
 /* which themselves contain no instances and work upward */ 
 for (mlevel = 1; mlevel <= __dagmaxdist; mlevel++) 
  {
   /* SJM 03/16/04 - notice processing one up and splitting (if needed) */
   /* contained instances - therefore mlevel of md lev hdr list not chged */
   /* inside this loop */
   for (mdp = __mdlevhdr[mlevel]; mdp != NULL; mdp = mdp->mlevnxt)
    {
     __push_wrkitstk(mdp, 0);
     
     /* go through instances in current module */
     for (ii = 0; ii < __inst_mod->minum; ii++)
      {
       ip = &(__inst_mod->minsts[ii]);
       if (ip->ipxprtab == NULL) continue;

       /* imdp is module type of instance that has pound params */
       imdp = ip->imsym->el.emdp;
       __sfnam_ind = ip->isym->syfnam_ind;
       __slin_cnt = ip->isym->sylin_cnt;

       /* because static src inst loc. has pnd params, if any gate must */
       /* try to set any gate range params */
       /* AIV 07/12/04 - old checking and was wrong nad mark gia rng checks */
       if (!imdp->mgiarngdone)
        {
         mark_gia_rng_params(imdp);
         imdp->mgiarngdone = TRUE;
        }

       /* eliminate if instance not arrayed at this src loc. */ 
       if (__inst_mod->miarr == NULL || __inst_mod->miarr[ii] == NULL)
        continue;

       /* mark parameters used in gi array ranges exprs once for each mod */
       if (!imdp->mgiarngdone)
        { mark_gia_rng_params(imdp); imdp->mgiarngdone = TRUE; }

       if (!down_hasgiarngdet_param(imdp)) continue;

       /* even if no split (one place in source instantiated) need to eval */ 
       /* mark that some modules have width determining pnd params and mark */
       /* that this one does */
       ip->imsym->el.emdp->mpndprm_ingirng = TRUE;

       /* if only one place in source instantiated, must not split */
       /* because would create never instantiated instance */
       if (imdp->flatinum == 1 || imdp->flatinum == __inst_mod->flatinum)
        continue; 

       /* this does not need to update miarr since parallel minsts updated */
       split_upd_mod(imdp, ip, mlevel); 
      }
     __pop_wrkitstk();
    }
  }
}

/*
 * mark all parameters that are used in arrays of gate/inst ranges
 * inside module pmdp
 *
 * splitting later only if these parameters are passed as pound params
 * caller will mark module 
 */
static void mark_gia_rng_params(struct mod_t *pmdp)
{
 register int32 gi, ii;
 struct giarr_t *giap;

 if (pmdp->mgarr != NULL)
  {
   for (gi = 0; gi < pmdp->mgnum; gi++)
    {
     if ((giap = pmdp->mgarr[gi]) == NULL) continue;

     if (in_giarng_markparam(giap, giap->giax1))
      pmdp->mpndprm_ingirng = TRUE;
     if (in_giarng_markparam(giap, giap->giax2))
      pmdp->mpndprm_ingirng = TRUE;
    }
  }
  
 if (pmdp->miarr != NULL)
  {
   for (ii = 0; ii < pmdp->minum; ii++)
    {
     if ((giap = pmdp->miarr[ii]) == NULL) continue;

     if (in_giarng_markparam(giap, giap->giax1))
      pmdp->mpndprm_ingirng = TRUE;
     if (in_giarng_markparam(giap, giap->giax2))
      pmdp->mpndprm_ingirng = TRUE;
    }
  }
}

/*
 * mark parmeters (nets) in arrays of gate/inst range parameters
 *
 * only called for expressions in gate/inst range indices
 */
static int32 in_giarng_markparam(struct giarr_t *giap, struct expr_t *xp)
{
 int32 rv1, rv2;
 struct net_t *np;

 rv1 = rv2 = FALSE;
 if (__isleaf(xp))
  {
   if (xp->optyp == ID && xp->lu.sy->sytyp == SYM_N)
    {
     np = xp->lu.sy->el.enp; 
     if (np->n_isaparam)
      {
       np->nu.ct->n_in_giarr_rng = TRUE;
       giap->gia_rng_has_pnd = TRUE;
       __design_gia_pndparams = TRUE;
       rv1 = TRUE; 
      }
    }
   return(rv1);
  }
 if (xp->lu.x != NULL) rv1 = in_giarng_markparam(giap, xp->lu.x);
 if (xp->ru.x != NULL) rv2 = in_giarng_markparam(giap, xp->ru.x);
 return(rv1 || rv2);
}

/*
 * return T if any down instance (module type ipmdp) pound parameter
 * determines array of gates or instances range
 */
static int32 down_hasgiarngdet_param(struct mod_t *ipmdp)
{
 register int32 pi;
 struct net_t *parm_np;
 
 /* if # param list short ok, do not bother changing missing at end */
 /* only check params actually set if short list */
 for (pi = 0; pi < ipmdp->mprmnum; pi++)
  {
   parm_np = &(ipmdp->mprms[pi]);

   if (parm_np->nu.ct->n_in_giarr_rng)
    {
     if (__debug_flg)
      {
       __dbg_msg(
        "+++ split module %s, # param %s in array of inst/gate range expr.\n",
        ipmdp->msym->synam, parm_np->nsym->synam);
      }
     return(TRUE);
    }
   /* move to next target module pound param */
  }
 if (__debug_flg)
  __dbg_msg(
   "+++ no need to split module %s because of pound param in gate/inst range",
   ipmdp->msym->synam);
 return(FALSE);
} 

/*
 * ROUTINES TO BUILD TOP VIRTUAL INSTANCES
 */

/*
 * build the list of instances for uninstantiated top level modules
 * this builds inst_t (static inst) for each top level modules
 * and makes top itab of index point to it
 *
 * notice top module may be split, so this new elements can be added later
 *
 * top_iptab points to created instances since for top module and instance
 * have same name - top_iptab and it_roots are parallel arrays, top_ipind 
 * ins sorted index into both top_iptab and it_roots since they are ||
 *
 */
static void bld_top_virtinsts(void)
{
 register int32 tpii;
 register struct mod_t *mdp;
 int32 ii;
 struct inst_t *ip;
 struct sy_t *syp;
 struct itree_t *itp;

 /* count number of top level modules */ 
 __numtopm = 0;
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   /* descend to count only from top level modules */
   if (mdp->minstnum != 0) continue;

   /* top modules will have is cell bit on if configs used */
   if (mdp->m_iscell && __map_files_hd == NULL)
    {
     __gfwarn(597, mdp->msym->syfnam_ind, mdp->msym->sylin_cnt,
      "top level module %s in `celldefine region but cannot be a cell",
       mdp->msym->synam);
     mdp->m_iscell = FALSE;
    }
   __numtopm++;
  }
 if (__numtopm == 0) return;
 __top_itab = (struct inst_t **)
  __my_malloc(__numtopm*sizeof(struct inst_t *));
 for (tpii = -1, mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->minstnum != 0) continue;
   /* need 1 itree inst for each top level module because itp->itip */
   /* accessed during simulation */
   ip = (struct inst_t *) __my_malloc(sizeof(struct inst_t));
   __top_itab[++tpii] = ip;
   /* initialize for unused by top fields */
   init_inst(ip);

   /* need to allocate symbol here - because may need name */
   /* but this symbol is in no symbol table - cannot be looked up */
   /* because there is no virtual top level module */
   syp = (struct sy_t *) __my_malloc(sizeof(struct sy_t));
   /* copying here in case implementing name removal comp. directives */
   syp->synam = __pv_stralloc(mdp->msym->synam);
   syp->sytyp = SYM_I;
   /* notice all symbols add during input phase */
   syp->sydecl = TRUE;
   syp->el.eip = ip;
   syp->syfnam_ind = mdp->msym->syfnam_ind;
   syp->sylin_cnt = mdp->msym->sylin_cnt;
   syp->spltsy = NULL;

   ip->isym = syp; 
   ip->imsym = mdp->msym;
   /* top level modules have exactly 1 instance */
   mdp->flatinum = 1;
   ip->ipxprtab = NULL;
   ip->pprm_explicit = FALSE;
   ip->ip_explicit = FALSE;
   ip->ipins = NULL;
  }
 __top_ipind = (int32 *) __my_malloc(__numtopm*sizeof(int32)); 
 for (ii = 0; ii < __numtopm; ii++) __top_ipind[ii] = ii;
 qsort((char *) __top_ipind, (word32) __numtopm, sizeof(int32), topip_cmp);

 /* table of pointers to root entries */
 __it_roots = (struct itree_t **)
  __my_malloc(__numtopm*sizeof(struct itree_t *));

 /* allocate root itp entries for each top level module */
 for (ii = 0; ii < __numtopm; ii++)
  {
   /* alloc sets itree value */
   itp = (struct itree_t *) __my_malloc(sizeof(struct itree_t));
   __it_roots[ii] = itp;
   __init_itree_node(itp);
   itp->itip = __top_itab[ii];
   itp->up_it = NULL;
   /* since top instance number must be 0 */
   itp->itinum = 0;
  }
}

/*
 * comparison routines for sorting top modules index
 */
static int32 topip_cmp(const void *ii1, const void *ii2)
{
 struct inst_t *ip1, *ip2;

 ip1 = __top_itab[*((int32 *) ii1)];
 ip2 = __top_itab[*((int32 *) ii2)];
 return(strcmp(ip1->imsym->synam, ip2->imsym->synam));
}

extern void __init_itree_node(struct itree_t *itp)
{
 itp->up_it = NULL;
 itp->in_its = NULL;
 itp->itip = NULL;
 itp->itinum = 0;
}

/*
 * ROUTINES TO DETERMINE ARRAYS OF GATES/INSTANCES RANGES 
 */

/*
 *
 * save all design instance (not task/func) parameters values
 *
 * only called if need to set gia ranges from params 
 *
 * this is point where gia ranges set so parameters evaluated but must
 * be put back because later defparams may change final values
 *
 * AIV 09/27/06 - since can't be changed do not need to save local params
 */
static void save_all_param_vals(void)
{
 register int32 pi;
 register struct mod_t *mdp;
 register struct net_t *parm_np;
 int32 nbytes;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mprmnum == 0) continue;

   for (pi = 0, parm_np = &(mdp->mprms[0]); pi < mdp->mprmnum; pi++, parm_np++)
    {
     nbytes = 2*WRDBYTES*wlen_(parm_np->nwid);
     parm_np->nu2.wp = (word32 *) __my_malloc(nbytes);
     memcpy(parm_np->nu2.wp, parm_np->nva.wp, nbytes);
    }
  }
}

/*
 * free all param values - must do this so can reuse nu2 field 
 */
static void free_all_param_vals()
{
 register int32 pi;
 register struct mod_t *mdp;
 register struct net_t *parm_np;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mprmnum == 0) continue;

   for (pi = 0, parm_np = &(mdp->mprms[0]); pi < mdp->mprmnum; pi++, parm_np++)
    {
     /* DBG remove --- */
     if (parm_np->nu2.wp == NULL) __misc_terr(__FILE__, __LINE__);
     /* --- */
     __my_free((char *) parm_np->nu2.wp, 2*WRDBYTES*wlen_(parm_np->nwid));
     parm_np->nu2.wp = NULL;
    }
  }
}

/*
 * evaluate arrays of gates and instances ranges to number
 *
 * depends on convention that arrays of gate/inst range indices can use
 * pound params (parmeter values changed by # parameters) but evaluation
 * happens before defparams set (and splitting occurs) so values converted
 * to number because component parameters may ultimately have different value
 *
 * here because no defparams values are static instance tree dependent, i.e. 
 * do not need to build and use itree
 *
 * trick is that because by here all modules with array of gate or instance
 * ranges determined by pound params, can set range as number
 */
static void set_giarr_ranges(void)
{
 register int32 gi, ii2;
 register struct mod_t *mdp;
 struct giarr_t *giap;

 /* assume some ranges do not have pound params because not much work */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   /* if module has arrays of gates expand first */
   if (mdp->mgarr != NULL)
    {
     for (gi = 0; gi < mdp->mgnum; gi++)
      {
       /* skip non array style gates */ 
       if ((giap = mdp->mgarr[gi]) == NULL) continue;
       /* if has pound param need to handle in special code below */
       if (giap->gia_rng_has_pnd) continue;

       eval1_arr_of_gates_rng(giap, mdp, (struct itree_t *) NULL, gi);
      }
    }
   if (mdp->miarr != NULL)
    {
     for (ii2 = 0; ii2 < mdp->minum; ii2++)
      {
       /* skip non array style instances */ 
       if ((giap = mdp->miarr[ii2]) == NULL) continue;
       /* if has pound param need to handle in special code below */
       if (giap->gia_rng_has_pnd) continue;

       eval1_arr_of_insts_rng(giap, mdp, (struct itree_t *) NULL, ii2);
      }
    }
   __pop_wrkitstk();
  }

 /* if design has no pound params in arrays of g/i ranges, nothing to do */
 if (__design_gia_pndparams) set_pnd_gi_rnges();
}

/*
 * evaluate array of gates ranges in module mdp 
 *
 * for non pound param case just works on raw expr
 * for pound param case, code called to save and set elaborated (fixed)
 * param values from propagating pound params down then afterwards unsaving
 * elaborated values because defparams may change
 */
static void eval1_arr_of_gates_rng(struct giarr_t *giap, struct mod_t *mdp,
 struct itree_t *itp, int32 gi)
{
 int32 bad_rng;
 struct gate_t *gp;
 struct xstk_t *xsp1, *xsp2;

 /* if error, since do not know range, convert to non array of gates */
 bad_rng = FALSE;
 /* know only first (master) auxiliary extra array non nil */
 gp = &(mdp->mgates[gi]);

 if (!gi_ndxexpr_chk(giap->giax1, gp->gsym->syfnam_ind,
  gp->gsym->sylin_cnt, "of gates first")) bad_rng = TRUE;

 /* save and set pound params by propagating up initial vals down */
 if (!bad_rng && itp != NULL) set_gia_expr_pndparms(giap->giax1, itp);

 /* although impossible to have any inst. specific values here */
 /* need something on it stk */
 if (itp == NULL)
  {
   /* if no passed itp - use wrk dummy inst (right mod) */
   xsp1 = __eval_xpr(giap->giax1);
  }
 else
  {
   __push_itstk(itp);
   xsp1 = __eval_xpr(giap->giax1);
   __pop_itstk();
  }

 /* unsave if pound case */
 if (!bad_rng && itp != NULL) unsave_gia_expr_pndparms(giap->giax1, itp);

 if (!gi_ndxval_chk(xsp1, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
  "of gates first")) bad_rng = TRUE;

 if (!gi_ndxexpr_chk(giap->giax2, gp->gsym->syfnam_ind,
  gp->gsym->sylin_cnt, "of gates second")) bad_rng = TRUE;

 /* save and set pound params by propagating up initial vals down */
 if (!bad_rng && itp != NULL) set_gia_expr_pndparms(giap->giax2, itp);

 if (itp == NULL)
  {
   /* if no passed itp - use wrk dummy inst (right mod) */
   xsp2 = __eval_xpr(giap->giax2);
  }
 else
  {
   __push_itstk(itp);
   xsp2 = __eval_xpr(giap->giax2);
   __pop_itstk();
  }

 if (!bad_rng && itp != NULL) unsave_gia_expr_pndparms(giap->giax2, itp);

 if (!gi_ndxval_chk(xsp2, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
  "of gates second")) bad_rng = TRUE;
 /* if error do not know range - convert back to one gate */
 if (!bad_rng) 
  {
   giap->gia1 = (int32) xsp1->ap[0];
   giap->gia2 = (int32) xsp2->ap[0];
  }
 __pop_xstk();
 __pop_xstk();
}

/*
 * check a gate or instance array index constant expression 
 * returns F on error 
 */
static int32 gi_ndxexpr_chk(struct expr_t *xp, int32 lfnind, int32 lcnt,
 char *emsg) 
{
 if (__expr_has_glb(xp) || !__chk_giarr_ndx_expr(xp))
  {
   __gferr(1161, lfnind, lcnt, 
    "array %s declaration index expression %s error - pound parameters and constants only",
    emsg, __msgexpr_tostr(__xs, xp));
   return(FALSE);
  }
 return(TRUE);
}

/*
 * check a gate or instance array value for 32 bits non x/z
 * emits error and returns F on error
 *
 * LOOKATME - maybe need warn or inform if >32 bits but higher 0's
 */
static int32 gi_ndxval_chk(struct xstk_t *xsp, int32 lfnind, int32 lcnt, char *emsg)
{
 if (xsp->xslen > WBITS)
  {
   if (!vval_is0_(&(xsp->ap[1]), xsp->xslen - 64)
    || !vval_is0_(xsp->bp, xsp->xslen))
    {
     __gferr(1189, lfnind, lcnt,
      "array %s declaration index value %s not required %d bit non x/z number",
      emsg, __regab_tostr(__xs, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE),
      WBITS);
     return(FALSE);
    }
  }
 return(TRUE);
}

/*
 * evaluate array of instances ranges in mdp
 *
 * for non pound param case just works on raw expr
 * for pound param case, code called to save and set elaborated (fixed)
 * param values from propagating pound params down then afterwards unsaving
 * elaborated values because defparams may change
 */
static void eval1_arr_of_insts_rng(struct giarr_t *giap, struct mod_t *mdp,
 struct itree_t *itp, int32 ii)
{
 int32 bad_rng;
 struct inst_t *ip;
 struct xstk_t *xsp1, *xsp2;

 /* if error, since do not know range, convert to non array of gates */
 bad_rng = FALSE;
 /* know only first (master) auxiliary extra array non nil */
 ip = &(mdp->minsts[ii]);

 if (!gi_ndxexpr_chk(giap->giax1, ip->isym->syfnam_ind,
  ip->isym->sylin_cnt, "of instances first")) bad_rng = TRUE;

 /* save and set pound params by propagating up initial vals down */
 if (!bad_rng && itp != NULL) set_gia_expr_pndparms(giap->giax1, itp);

 /* although impossible to have any inst. specific values here */
 /* need something on it stk */
 if (itp == NULL)
  {
   /* if no passed itp - use wrk dummy inst (right mod) */
   xsp1 = __eval_xpr(giap->giax1);
  }
 else
  {
   __push_itstk(itp);
   xsp1 = __eval_xpr(giap->giax1);
   __pop_itstk();
  }

 /* unsave if pound case */
 if (!bad_rng && itp != NULL) unsave_gia_expr_pndparms(giap->giax1, itp);

 if (!gi_ndxval_chk(xsp1, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
  "of instances first")) bad_rng = TRUE;

 if (!gi_ndxexpr_chk(giap->giax2, ip->isym->syfnam_ind,
  ip->isym->sylin_cnt, "of gates second")) bad_rng = TRUE;

 /* save and set pound params by propagating up initial vals down */
 if (!bad_rng && itp != NULL) set_gia_expr_pndparms(giap->giax2, itp);

 if (itp == NULL)
  {
   /* if no passed itp - use wrk dummy inst (right mod) */
   xsp2 = __eval_xpr(giap->giax2);
  }
 else
  {
   __push_itstk(itp);
   xsp2 = __eval_xpr(giap->giax2);
   __pop_itstk();
  }

 /* unsave if pound case */
 if (!bad_rng && itp != NULL) unsave_gia_expr_pndparms(giap->giax2, itp);

 if (!gi_ndxval_chk(xsp2, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
  "of instances second")) bad_rng = TRUE;
 /* if error do not know range - convert back to one gate */
 if (!bad_rng)
  {
   giap->gia1 = (int32) xsp1->ap[0];
   giap->gia2 = (int32) xsp2->ap[0];
  }
 __pop_xstk();
 __pop_xstk();
}

/*
 * set all module instance gi ranges with pound params
 *
 * only need to do once per module because modules with pound params
 * in gi ranges always split off to one instance
 *
 * final hard case where need itree that is discarded after this step
 */
static void set_pnd_gi_rnges(void)
{
 register int32 ii;
 struct itree_t *itp;
 struct mod_t *mdp;

 __bld_flat_itree();

 /* know top modules never have pound params */
 for (ii = 0; ii < __numtopm; ii++)
  {
   itp = __it_roots[ii];
   mdp = itp->itip->imsym->el.emdp;
   set2_pnd_gi_rnges(mdp, itp);
  }

 /* last step free itree - need to rebuild after defparam splitting */
 __free_flat_itree();
}

/*
 * descend down in itree setting pound param gia ranges
 * 
 * when see first instance of module set ranges for all instances
 * and mark - after that just ignore instances of module
 */
static void set2_pnd_gi_rnges(struct mod_t *up_mdp, struct itree_t *up_itp)
{
 register int32 gi, ii, ii2;
 struct mod_t *imdp;
 struct itree_t *itp;
 struct giarr_t *giap;

 /* first descend */
 for (ii = 0; ii < up_mdp->minum; ii++)
  {
   itp = &(up_itp->in_its[ii]);
   /* once module done, know all under also done - no need to descend */
   imdp = itp->itip->imsym->el.emdp;
   if (imdp->mpnd_girng_done) continue;

   /* need to descend even if does not have any in gia range pnd params */
   set2_pnd_gi_rnges(imdp, itp);
  }
 /* when this returns will be done even if none in range */
 up_mdp->mpnd_girng_done = TRUE;
 if (!up_mdp->mpndprm_ingirng) return;

 /* eval arrays of g/i ranges containing pnd params - others already done */
 if (up_mdp->mgarr != NULL)
  {
   for (gi = 0; gi < up_mdp->mgnum; gi++)
    {
     /* skip non array style gates */ 
     if ((giap = up_mdp->mgarr[gi]) == NULL) continue;

     /* if does not contain pound param already done */
     if (!giap->gia_rng_has_pnd) continue;

     __push_wrkitstk(up_mdp, 0);
     eval1_arr_of_gates_rng(giap, up_mdp, up_itp, gi);
     __pop_wrkitstk();
    }
  }
 if (up_mdp->miarr != NULL)
  {
   for (ii2 = 0; ii2 < up_mdp->minum; ii2++)
    {
     /* skip non array style instances */ 
     if ((giap = up_mdp->miarr[ii2]) == NULL) continue;
     /* if does not contain pound param already done */
     if (!giap->gia_rng_has_pnd) continue;

     __push_wrkitstk(up_mdp, 0);
     eval1_arr_of_insts_rng(giap, up_mdp, up_itp, ii2);
     __pop_wrkitstk();
    }
  }
}
    
/*
 * set all pound params in one g/i array range expression
 *
 * when done know all params in g/i array range expr. values elaborated (fixed)
 */
static void set_gia_expr_pndparms(struct expr_t *xp, struct itree_t *itp)
{
 struct net_t *np;

 if (__isleaf(xp))
  {
   if (xp->optyp == ID && xp->lu.sy->sytyp == SYM_N)
    {
     np = xp->lu.sy->el.enp; 
     if (np->n_isaparam) set1_giarr_pndparm(np, itp); 
    }
   return;
  }
 if (xp->lu.x != NULL) set_gia_expr_pndparms(xp->lu.x, itp);
 if (xp->ru.x != NULL) set_gia_expr_pndparms(xp->ru.x, itp);
}

/*
 * for parameter determine if pound and if so set value by setting
 * values in up instance pound expr and evaluating it
 * new up value is then set as value of down parameter 
 * may need to work up to top of itree
 */
static void set1_giarr_pndparm(struct net_t *np, struct itree_t *itp) 
{
 int32 pndpi;
 struct itree_t *up_itp;
 struct mod_t *up_mdp;
 struct inst_t *up_ip;
 struct expr_t *up_pndxp;
 struct xstk_t *xsp;
 char s1[RECLEN];

 /* may need to access parameter (not pound) in top level module */
 up_itp = itp->up_it;
 /* if top or no pound params or no pound expr. passed down for this, done */
 up_ip = itp->itip;
 if (up_ip->ipxprtab == NULL) return;
 up_mdp = up_ip->imsym->el.emdp;
 pndpi = ((byte *) np - (byte *) up_mdp->mprms)/sizeof(struct net_t);
 if (up_ip->ipxprtab[pndpi] == NULL) return;

 /* know this is pound param */
 up_pndxp = up_ip->ipxprtab[pndpi];

 /* work up evaluating parmeters changing pound params to passed down val */
 /* this sets values in pound passed (up) expr - if it contains pound */
 /* params recursively repeats up as far as needed (maybe to top) */
 set_gia_expr_pndparms(up_pndxp, up_itp);
 /* now all pound param in expr. replaced by passed down pound value */
   
 /* although pound params never instance specific must access constants */
 /* from up module con tab */
 __push_itstk(up_itp);
 xsp = __eval_xpr(up_pndxp);
 __pop_itstk();

 /* change value - IS form impossible here */
 sprintf(s1, "%s (pound param)", __msg2_blditree(__xs, itp));
 __cnvt_param_stkval(xsp, up_pndxp, np, s1);
 __assgn_nonis_param(np, up_pndxp, xsp);
 __pop_xstk();

 /* free and put back temporary "as of now" parameter values of params */
 /* in up pound expr */
 unsave_gia_expr_pndparms(up_pndxp, up_itp);
}

/*
 * free and unsave pound params in expression 
 *
 * non up pound params just ignored initial in source value was not changed
 * only needed for this expr. because if pnds worked up, evaled and unsaved
 */
static void unsave_gia_expr_pndparms(struct expr_t *xp, struct itree_t *itp)
{
 int32 pndpi;
 struct net_t *np;
 struct itree_t *up_itp;
 struct inst_t *ip;
 struct mod_t *mdp;

 if (__isleaf(xp))
  {
   if (xp->optyp == ID && xp->lu.sy->sytyp == SYM_N)
    {
     np = xp->lu.sy->el.enp; 
     if (np->n_isaparam)
      {
       if ((up_itp = itp->up_it) == NULL) return;
       ip = up_itp->itip;
       if (ip->ipxprtab == NULL) return;
       mdp = itp->itip->imsym->el.emdp;
       pndpi = ((byte *) np - (byte *) mdp->mprms)/sizeof(struct net_t);
       if (ip->ipxprtab[pndpi] == NULL) return;
 
       /* DBG remove --- */
       if (np->srep != SR_PNUM) __misc_terr(__FILE__, __LINE__);
       if (np->nu2.wp == NULL) __misc_terr(__FILE__, __LINE__);
       /* --- */

       /* need to unsave param value by copying back */
       /* this puts back intial value that is always SR_PNUM */
       memcpy(np->nva.wp, np->nu2.wp, 2*WRDBYTES*wlen_(np->nwid));
      }
    }
   return;
  }
 if (xp->lu.x != NULL) unsave_gia_expr_pndparms(xp->lu.x, itp);
 if (xp->ru.x != NULL) unsave_gia_expr_pndparms(xp->ru.x, itp);
}

/*
 * ROUTINES TO SPLIT ARRAY OF GATES AND INSTANCES  
 */

/*
 * rebuild mgates and miarr tables for each module with arrays of g/i
 *
 * for g/i arrays original symbol and pins store in giap
 * BEWARE - can not access symbol table in here until sorted and reconnected
 *          at end of gate and inst expansion
 */
static void rebld_mod_giarrs(void)
{
 register int32 i, i2, i3;
 register struct mod_t *mdp, *imdp;
 int32 has_iarrs, newgnum, newinum, giawid, j;
 int32 arrsynum, bi, gia_dir, new_stsiz;
 struct gate_t *gptab, *gp;
 struct inst_t *iptab, *ip;
 struct giarr_t *giap, **giatab;
 struct srcloc_t **sloctabp;

 has_iarrs = FALSE;
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mgarr == NULL && mdp->miarr == NULL) continue;

   __push_wrkitstk(mdp, 0);
   arrsynum = 0;
   /* first count number of new */ 
   if (mdp->mgarr != NULL)
    {
     /* compute new size of mgates table */
     for (i = 0, newgnum = 0; i < mdp->mgnum; i++) 
      {
       if ((giap = mdp->mgarr[i]) == NULL) { newgnum++; continue; }
       giap->gia_bi = newgnum;
       newgnum += __get_giarr_wide(giap);
       /* one extra symbol for every element of array but original gate */
       /* counted as one */
       arrsynum++;
      }
    } 
   else newgnum = mdp->mgnum;

   if (mdp->miarr != NULL)
    {
     has_iarrs = TRUE;
     /* compute new size of minsts table */
     for (i = 0, newinum = 0; i < mdp->minum; i++) 
      {
       if ((giap = mdp->miarr[i]) == NULL) { newinum++; continue; }
       giap->gia_bi = newinum;
       newinum += __get_giarr_wide(giap);
       arrsynum++;
      }
    }
   else newinum = mdp->minum;

   /* allocate room for new symbols now that know how many */
   /* notice every gate name symbol including first added */ 
   /* original moved to giap */
   new_stsiz = mdp->msymtab->numsyms + (newgnum - mdp->mgnum)
    + (newinum - mdp->minum) + arrsynum;
   /* SJM 07/02/05 - can't use realloc because need old stsyms during */
   /* build of new minst iptab - old worked only if realloc didn't move */
   /* can't free stsyms yet */
   __wrkstab = (struct sy_t **) __my_malloc(new_stsiz*sizeof(struct sy_t *));
   memcpy(__wrkstab, mdp->msymtab->stsyms,
    mdp->msymtab->numsyms*sizeof(struct sy_t *));

   __last_sy = mdp->msymtab->numsyms - 1;
   /* DBG remove --- */
   if (!mdp->msymtab->freezes) __misc_terr(__FILE__, __LINE__);
   /* --- */

   if (mdp->mgarr != NULL)
    {
     giatab = (struct giarr_t **)
      __my_malloc(newgnum*sizeof(struct giarr_t *));
     gptab = (struct gate_t *) __my_malloc(newgnum*sizeof(struct gate_t));
     /* copy from old to new gia and gate tables */
     /* i moves through old mgates, i2 through new */ 
     for (i = 0, i2 = 0; i < mdp->mgnum; i++) 
      {
       if ((giap = mdp->mgarr[i]) == NULL)
        {
         giatab[i2] = NULL;
         /* copy body (insides) */
         gp = &(gptab[i2]);
         *gp = mdp->mgates[i];
         /* SJM 07/02/05 - for non arrayed insts in mod with inst arrays, */ 
         /* need to connect the sym to new iptab entry - needed because */
         /* ip tab is array of inst_t records not array of ptrs */
         gp->gsym->el.egp = gp;
         i2++;
         continue;
        }
       /* move original gpins to giap and point all to it */
       /* for now all gpins lists same and >1 ptrs to it */
       giap->giapins = mdp->mgates[i].gpins;
       giap->gia_base_syp = mdp->mgates[i].gsym;
       giap->gia_base_syp->el.egp = &(gptab[i2]);
       giap->gia_base_syp->sy_giabase = TRUE;
       giap->gia_xpnd = TRUE;
       giawid = __get_giarr_wide(giap);
       bi = giap->gia1;
       gia_dir = (giap->gia1 > giap->gia2) ? -1 : 1;

       /* stored in gia1 to gia2 order - not normalized to h:l as for nets */
       for (i3 = i2; i3 < i2 + giawid; i3++, bi += gia_dir)
        {
         /* point all expanded to same index record - can always find first */
         giatab[i3] = giap;

         /* copy body of declared array of gates */
         gp = &(gptab[i3]);
         *gp = mdp->mgates[i];
         gp->g_du.pdels = __copy_dellst(mdp->mgates[i].g_du.pdels);
         add_new_gsym(gp, bi);  
        }
       i2 += giawid;
      }
     /* finally free old gate tables and allocate new */
     __my_free((char *) mdp->mgates, mdp->mgnum*sizeof(struct gate_t));
     mdp->mgates = gptab;
     mdp->mgarr = giatab;
     mdp->mgnum = newgnum;
    }
   if (mdp->miarr != NULL)
    {
     giatab = (struct giarr_t **)
      __my_malloc(newinum*sizeof(struct giarr_t *));
     iptab = (struct inst_t *) __my_malloc(newinum*sizeof(struct inst_t));
     sloctabp = (struct srcloc_t **)
      __my_malloc(newinum*sizeof(struct srcloc_t *));

     /* copy from old to new gia and gate tables */
     /* i moves through old minsts, i2 through new */ 
     for (i = 0, i2 = 0; i < mdp->minum; i++) 
      {
       if ((giap = mdp->miarr[i]) == NULL)
        {
         /* non array inst - just copy */
         giatab[i2] = NULL;

         ip = &(iptab[i2]);
         *ip = mdp->minsts[i];
         /* SJM 07/02/05 - for non arrayed insts in mod with inst arrays, */ 
         /* need to connect the sym to new iptab entry - needed because */
         /* ip tab is array of inst_t records not array of ptrs */
         ip->isym->el.eip = ip; 

         /* here just move ptr */
         sloctabp[i2] = mdp->iploctab[i];
         mdp->iploctab[i] = NULL;
         i2++;
         continue;
        }

       giap->giapins = mdp->minsts[i].ipins;
       giap->gia_base_syp = mdp->minsts[i].isym;
       giap->gia_base_syp->el.eip = &(iptab[i2]);
       giap->gia_base_syp->sy_giabase = TRUE;
       giap->gia_xpnd = TRUE;

       /* stored in gia1 to gia2 order - not normalized to h:l as for nets */
       giawid = __get_giarr_wide(giap);
       bi = giap->gia1;
       gia_dir = (giap->gia1 > giap->gia2) ? -1 : 1;
       /* stored in gia1 to gia2 order - not normalized to h:l as for nets */
       for (i3 = i2; i3 < i2 + giawid; i3++, bi += gia_dir)
        {
         /* point all expanded to same index record - can always find first */
         giatab[i3] = giap;
         ip = &(iptab[i3]);
         /* copy body of declared array of insts - no need to initialize */
         *ip = mdp->minsts[i];
         imdp = ip->imsym->el.emdp;

         /* symbol from base array of insts must point to first */
         if (i3 == i2)
          {
           /* first time move */
           sloctabp[i3] = mdp->iploctab[i];
           mdp->iploctab[i] = NULL;
          }
         else
          {
           /* thereafter alloc and copy */
           if (imdp->mpnum == 0) sloctabp[i3] = NULL; 
           else
            { 
             /* array of ptr to array of src locs each size different */
             /* because each inst. has different number of pins */
             sloctabp[i3] = (struct srcloc_t *)
              __my_malloc(imdp->mpnum*sizeof(struct srcloc_t));  
             /* copy each source loc record */
             for (j = 0; j < imdp->mpnum; j++)
              sloctabp[i3][j] = sloctabp[i2][j];
            }
          }
         ip->ipxprtab = copy_pndxtab(&(mdp->minsts[i]));
         add_new_isym(ip, bi);  
        }
       i2 += giawid;
       if (mdp->minstnum == 1 && giawid > 1) mdp->minstnum = 2;
      }
     /* finally free old insts tables and allocate new */
     __my_free((char *) mdp->minsts, mdp->minum*sizeof(struct inst_t));
     __my_free((char *) mdp->miarr, mdp->minum*sizeof(struct giarr_t *));
     __my_free((char *) mdp->iploctab, mdp->minum*sizeof(struct srcloc_t *));
     mdp->minsts = iptab;
     mdp->miarr = giatab;
     mdp->iploctab = sloctabp; 
     mdp->minum = newinum;
    }

   /* must re-sort symbol table - because table of ptr el union right */
   qsort((char *) __wrkstab, (word32) new_stsiz, sizeof(struct sy_t *),
    gia_sym_cmp);
   /* SJM 07/02/05 - free the mod's symbtab stsyms to fix memory leak */
   /* symtab is ptr to syms - for non i/g array's sym record still used */
   __my_free((char *) mdp->msymtab->stsyms,
    mdp->msymtab->numsyms*sizeof(struct sy_t *)); 
   mdp->msymtab->numsyms = new_stsiz;
   mdp->msymtab->stsyms = __wrkstab;
   __pop_wrkitstk();
  }
 /* if any any arrays of instances, must recount */  
 if (has_iarrs) count_flat_insts();
 /* mod level table remains same since more instance but no more types */
}

/*
 * comparison routine for sorting symbol table after expand g/i added 
 */
static int32 gia_sym_cmp(const void *sy1, const void *sy2)
{
 return(strcmp((*((struct sy_t **) sy1))->synam,
  (*(struct sy_t **) sy2)->synam));
}

/*
 * get width of array of inst/gate
 */
extern int32 __get_giarr_wide(struct giarr_t *giap)
{
 int32 r1, r2;

 r1 = giap->gia1;
 r2 = giap->gia2;
 /* DBG remove -- */
 if (r1 == -1 || r2 == -1) __arg_terr(__FILE__, __LINE__); 
 /* --- */
 return((r1 >= r2) ? (r1 - r2 + 1) : (r2 - r1 + 1));
}

/*
 * routine to add new (still unusuable and unsorted) symbol table
 * because stored in range order never need to normalize here
 */
static void add_new_gsym(struct gate_t *gp, int32 bi)  
{
 struct sy_t *gsyp;
 char nsynam[2*IDLEN];

 gsyp = (struct sy_t *) __my_malloc(sizeof(struct sy_t)); 
 *gsyp = *(gp->gsym);
 sprintf(nsynam, "%s[%d]", gp->gsym->synam, bi); 
 gsyp->synam = __pv_stralloc(nsynam);
 __wrkstab[++__last_sy] = gsyp;
 gp->gsym = gsyp;
 gsyp->el.egp = gp;
}

/*
 * routine to update new (still unusuable and unsorted) symbol table
 * because stored in range order never need to normalize here
 */
static void add_new_isym(struct inst_t *ip, int32 bi)  
{
 struct sy_t *isyp;
 char nsynam[2*IDLEN];

 isyp = (struct sy_t *) __my_malloc(sizeof(struct sy_t)); 
 *isyp = *(ip->isym);
 sprintf(nsynam, "%s[%d]", ip->isym->synam, bi); 
 isyp->synam = __pv_stralloc(nsynam);
 __wrkstab[++__last_sy] = isyp;
 ip->isym = isyp;
 isyp->el.eip = ip;
}

/*
 * routine to dump minsts for every module in the design
 */
extern void __dmp_dsgn_minst(char *hdr_s)
{
 register struct mod_t *mdp;
 int32 ii;
 struct inst_t *ip;

 __dbg_msg("%s ===>\n", hdr_s);
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __dbg_msg("+++ dumping instances in module %s:\n", mdp->msym->synam);
   for (ii = 0; ii < mdp->minum; ii++)
    {
     ip = &(mdp->minsts[ii]);
     __dmp_1minst(ip);
    }
  }
}

/*
 * dmp one minst element 
 */
extern void __dmp_1minst(struct inst_t *ip)
{
 char s1[RECLEN];

 if (ip->ipxprtab != NULL)
  {
   sprintf(s1, "ipxprtab[0]=%s", __msgexpr_tostr(__xs, ip->ipxprtab[0]));
  }
 else strcpy(s1, "[no ipxprtab]");
 __dbg_msg("     inst %s type %s %s\n", ip->isym->synam, ip->imsym->synam,
  s1);
 if (ip->isym->el.eip != ip)
  {
   __dbg_msg("inst %s type %s - other inst %s type %s\n",
     ip->isym->el.eip->isym->synam, ip->isym->el.eip->imsym->synam, 
     ip->isym->synam, ip->imsym->synam); 
   __misc_terr(__FILE__, __LINE__);
  }
}

/*
 * ROUTINES TO CHECK DEFPARAMS
 */

/*
 * check defparam format parameter statements 
 *
 * must be done before pound splitting because pound splitting copy
 * of defparam grefs must copy glb ref components
 *
 * if error, undef symbol - must ignore undef. symbols later
 */
static void chk_defparams(void)
{
 register struct dfparam_t *dfpp;
 register struct mod_t *mdp;
 struct dfparam_t *dfpp2, *last_dfpp;

 __num_dfps = 0;
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   for (last_dfpp = NULL, dfpp = __inst_mod->mdfps; dfpp != NULL;)
    {
     dfpp2 = dfpp->dfpnxt;
     __sfnam_ind = dfpp->dfpfnam_ind;
     __slin_cnt = dfpp->dfplin_cnt;

     /* must check and set widths here since can contain parameters defined */ 
     /* later in source according to LRM */
     if (!__chk_paramexpr(dfpp->dfpxrhs, 0))
      {
       __sgferr(1035,
        "defparam %s right hand side error - parameters and constants only",
        __msgexpr_tostr(__xs, dfpp->dfpxlhs));
       __free2_xtree(dfpp->dfpxrhs);
       __set_numval(dfpp->dfpxrhs, ALL1W, ALL1W, WBITS);
      }
     /* check and set value during src read since only previously defined */
     /* params on rhs and must use initial values */

     /* SJM 01/27/04 - old code was wrongly evaling defparam rhs to number */
     /* must be left as expr since dfps may need changed dependend param */

     if (!lhs_chk1dfparam(dfpp))
      {
       /* if error, remove from list on error */
       if (last_dfpp == NULL) __inst_mod->mdfps = dfpp2;
       else last_dfpp->dfpnxt = dfpp2;
       /* free the defparam - compeletly removed from circuit */
       __free_1dfparam(dfpp);
      }
     else { last_dfpp = dfpp; __num_dfps++; }
     dfpp = dfpp2;
    }
   __pop_wrkitstk();
  }
 /* final step is freeing all defparams in gref table, no longer needed */
 free_gone_glbs();
}

/*
 * check parameter lhs expression and return
 * resolution of defparam lhs global in here
 *
 * array of instance selects illegal in defparams  
 */
static int32 lhs_chk1dfparam(struct dfparam_t *dfpp)
{
 struct gref_t *grp;
 struct expr_t *lhsndp;
 struct sy_t *syp;
 struct net_t *np;

 lhsndp = dfpp->dfpxlhs;
 grp = NULL;
 if (lhsndp->optyp == GLBREF)
  { 
   grp = lhsndp->ru.grp;
   grp->gr_defparam = TRUE;
   /* know here none of the special scope or dumpvars bits set */
   __resolve_glbnam(grp);
   if (lhsndp->optyp == ID) goto is_local;
   __num_glbdfps++;
   /* if error in resolving global cannot check here - syp not set */
   if (grp->gr_err) return(FALSE);
   /* upward relative defparams illegal to match Cadence */
   if (grp->upwards_rel)
    {
     __sgferr(760,
      "upward relative defparam lvalue %s illegal - must be rooted or downward relative",
      grp->gnam);
     grp->upwards_rel = FALSE;      
     grp->gr_err = TRUE;
     return(FALSE);
    }

   syp = lhsndp->lu.sy;
   np = syp->el.enp;
   if (syp->sytyp != SYM_N || !np->n_isaparam)
    {
     __sgferr(755, "defparam hierarchical name lvalue %s is not a parameter",
      grp->gnam);
     grp->gr_err = TRUE;
     return(FALSE);
    }
  
   /* AIV 09/27/06 - lhs of a defparam cannot be a localparam */
   if (np->nu.ct->p_locparam)
    {
     __sgferr(3430, "defparam hierarchical name lvalue %s cannot be a localparam",
      grp->gnam);
     grp->gr_err = TRUE;
     return(FALSE);
    }

   /* for any array of inst selects in defparams are illegal */ 
   if (gref_has_giarr_ndxes(grp))
    {
     __sgferr(691, 
      "defparam lvalue %s instance array select illegal - use pound param",
      grp->gnam);
     grp->gr_err = TRUE;
     return(FALSE);
    }

   /* now have defparam global lhs path - so done with global move to defp */
   /* move guts of gref to defparam nil fields and mark global as gone */
   set_1defparam_iis(dfpp, grp);
   dfpp->gdfpnam = grp->gnam;
   grp->gnam = NULL;

   grp->last_gri = -1;
   dfpp->targsyp = grp->targsyp;
   dfpp->dfptskp = grp->targtskp;
   grp->targsyp = NULL;
   /* must remove gref since contents in defparam */
   grp->gr_gone = TRUE;
   lhsndp->ru.grp = NULL;
   return(TRUE); 
  }

is_local:
 /* this is local - if in module gref - converted to simple by here */
 syp = lhsndp->lu.sy;
 np = syp->el.enp;
 if (syp->sytyp != SYM_N || !np->n_isaparam)
  {
   __sgferr(756, "defparam local lvalue variable %s not a parameter",
    syp->synam);
   if (grp != NULL) grp->gr_err = TRUE;
   return(FALSE);
  }
 /* AIV 09/27/06 - lhs of a defparam cannot be a localparam */
 if (np->nu.ct->p_locparam)
  {
   __sgferr(3430, "defparam local lvalue variable %s cannot be a localparam",
    syp->synam);
   if (grp != NULL) grp->gr_err = TRUE;
   return(FALSE);
  }
 dfpp->dfp_local = TRUE;
 dfpp->gdfpnam = __pv_stralloc(syp->synam);
 dfpp->in_mdp = __inst_mod;
 dfpp->targsyp = syp;
 __num_locdfps++;
 return(TRUE);
}

/*
 * return T if resolved global name has instance array index component
 */
static int32 gref_has_giarr_ndxes(struct gref_t *grp)
{
 register int32 gri;

 for (gri = 0; gri <= grp->last_gri; gri++) 
  {
   if (grp->grxcmps[gri] != NULL) return(TRUE);
  }
 return(FALSE);
}

/*
 * set defparam itree ii paths 
 *
 * needed now because copying changes instance symbols
 * later will convert downward relative dfpiis to rooted and 1 design list
 */
static void set_1defparam_iis(struct dfparam_t *dfpp,
 struct gref_t *grp)
{
 register int32 gi;
 int32 ii;
 byte *bp1, *bp2;
 struct sy_t *syp;
 struct inst_t *ip;
 struct mod_t *up_mdp;

 dfpp->last_dfpi = grp->last_gri;
 dfpp->dfpiis = (int32 *) __my_malloc((dfpp->last_dfpi + 1)*sizeof(int32));
 dfpp->dfp_rooted = (grp->is_rooted) ? TRUE : FALSE;
 if (dfpp->dfp_rooted)
  {
   /* defparam path root wrong */
   syp = grp->grcmps[0];
   if ((ii = __ip_indsrch(syp->synam)) == -1) __misc_terr(__FILE__, __LINE__);
   dfpp->dfpiis[0] = ii;
   gi = 1;
   up_mdp = __top_itab[ii]->imsym->el.emdp;
  }
 else { gi = 0; up_mdp = __inst_mod; }  

 for (; gi <= grp->last_gri; gi++)
  {
   /* grcmps components parallel here */
   syp = grp->grcmps[gi];
   ip = syp->el.eip;
   /* making use of c pointer subtraction correction object size here */
   /* changing to byte ptr because not sure of c ptr size object rules */
   bp1 = (byte *) ip;
   bp2 = (byte *) up_mdp->minsts;
   ii = (bp1 - bp2)/sizeof(struct inst_t);

   /* DBG remove --- */
   if (__debug_flg)
    {
     __dbg_msg(
      "^^ defparam %s component %s index %d instance %s type %s in module %s\n",
      grp->gnam, syp->synam, ii, ip->isym->synam, ip->imsym->synam,
      up_mdp->msym->synam);
    }
   /* --- */
   dfpp->dfpiis[gi] = ii;
   up_mdp = syp->el.eip->imsym->el.emdp;
  }
}

/*
 * resolve all non defparam xmrs in all modules
 *
 * since this is done after all splitting and fix up of itree and static
 * inst tree - all grefs will have correct syms
 * all defparams set by here 
 */
static void resolve_xmrs(void)
{
 register int32 gri;
 register struct gref_t *grp; 
 register struct mod_t *mdp;

 /* notice must include any newly split off modules here */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);
   grp = &(__inst_mod->mgrtab[0]);
   for (gri = 0; gri < __inst_mod->mgrnum; gri++, grp++)
    {
     /* FIXME - for now leaving defparams but marked as gone here */
     /* why not remove? */
     if (grp->gr_err) continue;
     __resolve_glbnam(grp);
     /* must count here because must resolve globals from pli and debugger */
     if (!grp->gr_err) __num_glbs++;
    }
   __pop_wrkitstk();
  }
 /* must free because can be many PLI systf args that are converted back */  
 free_gone_glbs();
}

/*
 * build the global path name component symbol list for 1 global
 *
 * SJM 01/14/00 
 * this requires that inst mod be set usually by push wrk itstk routine
 * it just decompoes xmr into path components so does not need itree context
 */
extern void __resolve_glbnam(struct gref_t *grp)
{
 register int32 gi;
 struct sy_t *syp;
 struct sy_t **syarr;
 struct expr_t *gcmp_ndp, **syxarr;

 __sfnam_ind = grp->grfnam_ind;
 __slin_cnt = grp->grflin_cnt;

 /* DBG remove --- */
 if (grp->gxndp->optyp != GLBREF) __arg_terr(__FILE__, __LINE__);
 if (grp->glbref->optyp != GLBPTH) __arg_terr(__FILE__, __LINE__);
 if (grp->glbref->ru.x == NULL || grp->glbref->ru.x->optyp != XMRCOM)
  __arg_terr(__FILE__, __LINE__);
 /* --- */
 gcmp_ndp = grp->glbref->ru.x;

 /* separate handling for intra-module (really local) hierarchical paths */
 /* does all processing and then returns */
 /* defparam lhs may be hierarchical local - changed to def param later */
 /* all 1 component special instance/prim globals caught here */
 /* also any illegal selects caught because know non-inst scope objects */
 if (resolve_local_path(grp, gcmp_ndp)) return;

 /* SJM 09/15/00 - because of inst tree dependent upward rel forms */
 /* reauired by LRM must use old routine for defparams to fill head */
 if (grp->gr_defparam)
  {
   /* fill the defparam lhs head - notice after first component found */
   /* and filled rest is same as normal XMR */
   /* different needed because itree needed for full XMR processing */
   if (!fill_dfp_gsymhead(grp, gcmp_ndp)) goto bad_end;
  }
 else
  {
   /* fill root of non defparam xmr path */
   if (!fill_gsymhead(grp, gcmp_ndp)) goto bad_end;
  }

 /* handle special scope form 1 component global */
 /* only scope place [top mod] or [inst] as simple name can get here */ 
 /* LOOKATME think this case handled in resolve local path */
 if (gcmp_ndp->ru.x == NULL)
  {
   /* fake last component index - since only access one back from end */
   syp = NULL;
   if (grp->gr_inst_ok)
    {
     /* this needs to be 1 component global where targsyp and path same */
     /* here both target and last inst. scope symbol are the same */ 
     syp = __glbsycmps[0]; 
     grp->targsyp = syp;
    }
   else if (grp->is_upreltsk)
    {
     /* upward rel converts to [mod name].[func name] */
     syp = __glbsycmps[0]; 
     grp->targsyp = __glbsycmps[1]; 
    }
   else __case_terr(__FILE__, __LINE__);
   goto fill_glb;
  }
 /* this is a normal xmr - fill rest of xmr path */
 /* this removes the intra-module path (except last) from glb sy cmps */
 if (!fill_grestsyms(grp, gcmp_ndp->ru.x)) goto bad_end;

 /* fill various gref fields - target symbol always last even if inst. */
 grp->targsyp = __glbsycmps[__last_gsc];

 /* need to set syp as ending inst. not var for setting targmdp */
 if (grp->targsyp->sytyp == SYM_I || grp->targsyp->sytyp == SYM_M)
  syp = grp->targsyp;
 /* notice syp is 1 before target end - usually will be inst. */
 else syp = __glbsycmps[__last_gsc - 1]; 

fill_glb:
 /* may be dumpvars or scope style instance reference */
 if (syp->sytyp == SYM_I) grp->targmdp = syp->el.eip->imsym->el.emdp;
 else if (syp->sytyp == SYM_M) grp->targmdp = syp->el.emdp;
 else 
  {
   __sgferr(1107,
    "hierarchical path reference %s target symbol %s type %s illegal",  
    grp->gnam, syp->synam, __to_sytyp(__xs, syp->sytyp)); 
   goto bad_end;   
  }

 /* handle special scope globals */
 /* can be module for top only or upward relative one component */
 if (grp->targsyp->sytyp == SYM_I || grp->targsyp->sytyp == SYM_M) 
  {
   /* notice task scope form looks like normal here and checked elsewhere */
   if (!grp->gr_inst_ok)
    {
     __sgferr(1030,
      "special system task instance form argument %s not allowed here",
      grp->gnam);
     goto bad_end;
    }
   /* adjust last component so it stays as part of path here */
   /* targsyp and last are same symbol */
   grp->last_gri = (short int) __last_gsc;
  }
 else grp->last_gri = (short int) __last_gsc - 1;
 if (!chk_glb_inst_sels(grp)) goto bad_end;

 /* allocate the array of symbols in malloced storage */
 syarr = (struct sy_t **)
   __my_malloc((__last_gsc + 1)*sizeof(struct sy_t *));
 grp->grcmps = syarr;

 syxarr = (struct expr_t **)
   __my_malloc((__last_gsc + 1)*sizeof(struct expr_t *));
 grp->grxcmps = syxarr;

 /* tail pointed to by targsyp end of syarr is last instance symbol */
 /* for scope object targsyp and last syarr same */ 
 for (gi = 0; gi <= __last_gsc; gi++)
  {
   syarr[gi] = __glbsycmps[gi];
   /* will be nil for non inst. select */
   syxarr[gi] = __glbxcmps[gi];
  }

 /* check and fold gref instance array select constant select expressions */
 if (grp->path_has_isel)
  {
   char s1[RECLEN];

   for (gi = 0; gi <= grp->last_gri; gi++) 
    {
     if (grp->grxcmps[gi] == NULL) continue;

     sprintf(s1, "hierarchical reference instance array index (pos. %d)",
      gi + 1);
     if (!__chkndx_expr(grp->grxcmps[gi], s1)) goto bad_end;
    }
  }

 /* even though downward really rooted, must not treat as rooted */
 /* until variable xmr converted to location - for defparam need to */
 /* add module name (1 level) prefix */

 /* fill symbol grp expr and grp ru expr field already cross linked */
 /* for disable will point to disable expr. */
 /* LOOKATME - although XMRCOM expr contents no longer accessible */
 /* can not free because grp points to grcmps - slight memory leak here */
 grp->gxndp->ru.grp = grp;
 grp->gxndp->lu.sy = grp->targsyp;
 return;

bad_end:
 grp->gr_err = TRUE;
}

/*
 * resolve a local name that is a qualified path name
 * if really local path, converted and checked as id later
 *
 * here caller handled instance arrays - know in module scope symbols
 * are never arrayed (error emitted if select appears)
 */
static int32 resolve_local_path(struct gref_t *grp, struct expr_t *ghd_ndp)
{
 struct symtab_t *sytp;
 struct sy_t *syphd, *syptail;
 struct expr_t *ndp, *locndp;
 char *ncmp;

 if (ghd_ndp->lu.x->optyp == XMRID) ndp = ghd_ndp->lu.x;
 else if (ghd_ndp->lu.x->optyp == LSB) ndp = ghd_ndp->lu.x->lu.x;
 else { __case_terr(__FILE__, __LINE__); return(FALSE); }

 ncmp = ndp->ru.qnchp;
 /* work up task chains searching for local */
 for (syphd = NULL, sytp = grp->grsytp; sytp != NULL; sytp = sytp->sytpar)
  {
   if ((syphd = __zget_sym(ncmp, sytp->stsyms, sytp->numsyms)) != NULL) break;
  }
 if (syphd == NULL) return(FALSE);

 /* handle xmr's that were guessed to be xmrs but are really either errors */
 /* or guessed wrong and just simple wires - if this is place wire cannot */
 /* go, error will be caught in item (such as task enable) checking */
 if (syphd->sytyp == SYM_N)
  {
   /* if this is hierarchical global whose head is wire - error */
   /* i.e. if only 1 component ok */
   if (ghd_ndp->ru.x != NULL || !grp->gr_inst_ok)
    {
     __sgferr(754, "hierarchical reference %s first component %s %s illegal",
      grp->gnam, __to_wtnam(__xs, syphd->el.enp), syphd->synam);
     grp->gr_err = TRUE;
     return(TRUE);
    }
   /* must be wire not parameter here at end - param use xmr's illegal */
   if (!chk_xmr_tail_wire(grp, syphd, TRUE))
    { grp->gr_err = TRUE; return(TRUE); }

local_1cmp:
   /* if this is xmr that is 1 component - change back to simple wire */
   grp->gr_gone = TRUE;
   /* LOOKATME - why is this needed since removing glb */
   if (syphd->sytyp == SYM_TSK || syphd->sytyp == SYM_F ||
    syphd->sytyp == SYM_LB) grp->targtskp = syphd->el.etskp;
   /* this can be local task call too - not just wire */
   if (ghd_ndp->lu.x->optyp == XMRID)
    {
     /* first free the XMRCOM grp expr. part */  
     locndp = grp->gxndp;
     __free2_xtree(locndp);
     locndp->optyp = ID;
     locndp->ru.grp = NULL;
     locndp->lu.sy = syphd;
    }
   else 
    {
     /* here gxndp is GLB REF with ru ptr to XMR COM (list) */     
     /* here rearranging so can not free */
     locndp = grp->gxndp;
     locndp->optyp = LSB;
     locndp->ru.grp = NULL;
     locndp->lu.x = ghd_ndp->lu.x->lu.x;
     locndp->lu.x->optyp = ID;
     locndp->lu.x->lu.sy = syphd;
     locndp->ru.x = ghd_ndp->lu.x->ru.x; 
    }
   /* global is now removed and do not need to save name since only one comp */
   return(TRUE);
  }
 /* know only way for this to happen is gr loc is scope object */  
 /* if this is 1 component task enable or function call will never be */
 /* seen as global in source reading */
 /* handle in normal global code since it cannot be local */

 /* not a wire - handle case of 1 component scope object */
 /* this is global caused by appearing in scope legal place */
 if (ghd_ndp->ru.x == NULL)
  {
   /* can be task/func/named block in which case convert back to local */
   /* also can be udp or gate primitive */
   /* and no need to check for possibility of wrong param will check in */
   /* sys task enable code */
   if (syphd->sytyp == SYM_TSK || syphd->sytyp == SYM_F ||
    syphd->sytyp == SYM_LB)
    {
     /* any scope array that is not array of insts is illegal */
     if (ghd_ndp->lu.x->optyp == LSB)
      {
local_sel_err:
       __sgferr(690,
        "hierarchical reference %s illegal - array of %s illegal",
        grp->gnam, __to_sytyp(__xs, syphd->sytyp));
       grp->gr_err = TRUE;
       return(TRUE);
      }
     goto local_1cmp; 
    }

   /* normal 1 component inst. or top mod. global - handled elsewhere */
   return(FALSE);
  }
 /* if in module symbol head instance, then not local */
 if (syphd->sytyp != SYM_TSK && syphd->sytyp != SYM_F &&
  syphd->sytyp != SYM_LB) return(FALSE); 

 /* know this local name expressed with qualified path */
 /* 1st check head to see if also a module name */
 if (__get_sym(ncmp, __modsyms) != NULL)
  {
   __sgfwarn(532,
    "local hierarchical name %s path head is also module name - local used",  
    grp->gnam);
  }
 /* NULL means undecl. - error but getting here still means local */
 /* checks for parameter at end of path in here */
 /* getting here means first symbol is is declared in module */  
 if ((syptail = find_inmod_sym(grp, ghd_ndp, syphd, &sytp)) == NULL)
  {
   grp->gr_err = TRUE;
   return(TRUE);
  }
 /* after end of instances, local scope object can not be array ref. */
 if (ghd_ndp->lu.x->optyp == LSB) goto local_sel_err;

 /* know task/func/mod */
 /* notice ...lb.lb as scope is ok - just like local - and >2 elements */
 if (syphd->sytyp == SYM_LB && ghd_ndp->ru.x != NULL
  && ghd_ndp->ru.x->ru.x != NULL)
  {
   __sgfinform(401, 
    "local hierarchical path %s first component is named block", grp->gnam);
  }
 /* finally convert to ID but save name for printing */
 grp->gr_gone = TRUE;
 /* LOOKATME - again why is this needed since gone turned on */
 grp->targtskp = syphd->el.etskp;

 locndp = grp->gxndp;
 __free2_xtree(locndp);
 /* must set non xmr node tail */
 locndp->lu.sy = syptail;
 locndp->optyp = ID;
 /* re-allocate this so can eventually if needed free global */
 /* notice must not free gnam field in case free globals (shouldn't) */  
 /* FIXME - should for inst. array references need to convert to numbers */
 locndp->ru.qnchp = grp->gnam;
 grp->gnam = NULL;
 locndp->locqualnam = TRUE;
 /* global is now removed */
 return(TRUE);
}

/*
 * check a wire tail to make sure not illegal parameter wire form
 * know symbol is wire to be called 
 */
static int32 chk_xmr_tail_wire(struct gref_t *grp, struct sy_t *tailsyp,
 int32 is_loc1cmp)
{
 struct net_t *np;

 switch ((byte) tailsyp->sytyp) {
  /* these are legal in some places - various kinds of xmrs checked for in */
  /* place of use */
  case SYM_I: case SYM_M: case SYM_LB: case SYM_F: case SYM_TSK: break;
  case SYM_N:
   /* this if fixup of guessed wrong because sys. task special arg xmr */
   /* that is really net */
   np = tailsyp->el.enp;
   /* if resolving defparam lhs xmr, parameter ok and required but */
   /* if not error caught later */
   if (grp->gr_defparam) break;

   if (!is_loc1cmp && np->n_isaparam)
    {
     __sgferr(769,
      "hierarchical reference %s tail variable %s cannot be a parameter",
      grp->gnam, np->nsym->synam);
     return(FALSE);
    }
   break;
  default:
   __sgferr(766, "hierarchical reference %s tail %s cannot be a %s",
    grp->gnam, tailsyp->synam, __to_sytyp(__xs, tailsyp->sytyp));
  }
 return(TRUE);
}

/*
 * fill defparam component symbol array first element 
 * this forces symbol location of all remaining components 
 *
 * only legal possibilites are 1) instance in current module 2) top level
 * module, and 3) upward module type reference (this is error caught by
 * caller)
 *
 * return F on error else T - error message emitted here
 */
static int32 fill_dfp_gsymhead(struct gref_t *grp, struct expr_t *ghd_ndp)
{
 int32 comp_isel; 
 struct sy_t *syp, *modsyp;
 struct symtab_t *sytp;
 struct expr_t *ndp;
 char *ncmp;

 comp_isel = FALSE;
 if (ghd_ndp->lu.x->optyp == XMRID) ndp = ghd_ndp->lu.x;
 else if (ghd_ndp->lu.x->optyp == LSB)
   { ndp = ghd_ndp->lu.x->lu.x; comp_isel = TRUE; }
 else { __case_terr(__FILE__, __LINE__); return(FALSE); }

 ncmp = ndp->ru.qnchp;
 /* if can possibly be rooted from top module or above module, save symbol */
 modsyp = __get_sym(ncmp, __modsyms);

 /* 1st priority is instance (must exist) in mod where glb ref. appeared */
 /* but maybe accessed from contained task or named block scope */
 for (sytp = grp->grsytp; sytp != NULL; sytp = sytp->sytpar)
  { if (sytp->sypofsyt->sytyp == SYM_M) goto got_inst; } 
 __misc_terr(__FILE__, __LINE__);
  
 /* look up in inst. symbol table only */
got_inst:
 if ((syp = __zget_sym(ncmp, sytp->stsyms, sytp->numsyms)) != NULL)
  {
   if (syp->sytyp != SYM_I)
    {
     __sgfwarn(533,
      "defparam left hand side %s head %s %s in %s not instance - assuming rooted",
      grp->gnam, __to_sytyp(__xs, syp->sytyp), ncmp, sytp->sypofsyt->synam);
     syp = NULL;
     goto try_mod1st;
    }
   /* emit warning if also could be rooted but use inst. */
   if (modsyp != NULL)
    {
     __sgfwarn(534,
      "defparam left hand side %s head %s both upward module and downward instance - instance used",
      grp->gnam, ncmp);
    }
   __glbsycmps[0] = syp;
   if (comp_isel)
    {
     __glbxcmps[0] = ghd_ndp->lu.x->ru.x;
     grp->path_has_isel = TRUE;
    }
   else __glbxcmps[0] = NULL;
   __last_gsc = 0;
   return(TRUE);
  }

 /* did not find instance name - see if matches module name */
try_mod1st:
 if (modsyp == NULL)
  {
   __sgferr(757,
    "rooted or upward relative defparam left hand side %s first component %s illegal or undeclared",
    grp->gnam, ncmp);
   return(FALSE);
  }
 /* since upward module not instance, error if select */
 if (comp_isel) 
  {
   __sgferr(692,
    "rooted or upward relative path %s head %s not instance array - select illegal",
    grp->gnam, ncmp);
   return(FALSE);
  }

 /* know this is module and if non rooted for now assume above */
 __glbsycmps[0] = modsyp;
 __glbxcmps[0] = NULL;
 __last_gsc = 0;
 /* notice cannot check if upward really above in itree until prep. time */
 if (modsyp->el.emdp->minstnum != 0) grp->upwards_rel = TRUE; 
 else grp->is_rooted = TRUE;
 return(TRUE);
}

/*
 * fill global component symbol array first element 
 * this forces symbol location of all remaining components 
 *
 * 09/14/00 - SJM now allow insts above this inst as first to check
 * LOOKATME - for now module type found above must be same for all insts
 *            but can come from inst or mod (happens when same name)
 *
 * only legal possibilites are 1) instance in current module 2) instance
 * above 3) top level module, and 4) upward module
 *
 * return F on error else T - error message emitted here
 */
static int32 fill_gsymhead(struct gref_t *grp, struct expr_t *ghd_ndp)
{
 int32 comp_isel; 
 struct sy_t *syp, *syp2, *modsyp, *scope_syp;
 struct symtab_t *sytp;
 struct expr_t *ndp;
 struct mod_t *imdp;
 struct itree_t *in_itp;
 char *ncmp;

 comp_isel = FALSE;
 if (ghd_ndp->lu.x->optyp == XMRID) ndp = ghd_ndp->lu.x;
 else if (ghd_ndp->lu.x->optyp == LSB)
   { ndp = ghd_ndp->lu.x->lu.x; comp_isel = TRUE; }
 else { __case_terr(__FILE__, __LINE__); return(FALSE); }

 ncmp = ndp->ru.qnchp;
 /* if can possibly be rooted from top module or above module, save symbol */
 /* but first priority is instance in module and then instance above */
 modsyp = __get_sym(ncmp, __modsyms);

 /* if ref in task/named block, first move up to inst sym tab */
 /* will always find it and usually already there */
 /* know if is it not XMR but really local path ref, won't get here */
 for (sytp = grp->grsytp; sytp != NULL; sytp = sytp->sytpar)
  { if (sytp->sypofsyt->sytyp == SYM_M) goto inst_sytab; } 
 __misc_terr(__FILE__, __LINE__);
  
inst_sytab:
 /* case 1: first priority downward relative inst name */
 syp = __zget_sym(ncmp, sytp->stsyms, sytp->numsyms);
 if (syp == NULL) goto try_rooted; 
 if (syp->sytyp != SYM_I)
  {
   /* LOOKATME - think this can only happen for top level named blocks */
   __sgfwarn(533,
    "hierarchical path %s head defined as %s %s in %s not instance - assuming upward name or rooted",
    grp->gnam, __to_sytyp(__xs, syp->sytyp), ncmp, sytp->sypofsyt->synam);
   syp = NULL;
   goto try_rooted;
  }
 /* emit inform if can also be rooted */
 if (modsyp != NULL && modsyp->el.emdp->minstnum == 0)
  {
   __sgfinform(464,
    "hierarchical path %s head %s downward relative but also top level module - could be rooted path",
    grp->gnam, ncmp);
  }
 /* emit inform if can be upward relative instance - just check one 0th inst */
 if (fnd_uprel_inst(ncmp, grp->gin_mdp->moditps[0]) != NULL)
  {
   __sgfinform(464,
    "hierarchical path %s head %s downward relative but also upward instance - could be upward relative path",
    grp->gnam, ncmp);
  }
 /* emit inform if can be upward relative instance - just check 0th inst */
 if (modsyp != NULL && fnd_uprel_mod(modsyp, grp->gin_mdp->moditps[0]))
  {
   __sgfinform(464,
    "hierarchical path %s head %s downward relative but also upward module type name - could be upward relative path",
    grp->gnam, ncmp);
  }
 __glbsycmps[0] = syp;
 if (comp_isel)
  {
   __glbxcmps[0] = ghd_ndp->lu.x->ru.x;
   grp->path_has_isel = TRUE;
  }
 else __glbxcmps[0] = NULL;
 __last_gsc = 0;
 return(TRUE);

try_rooted: 
 /* SJM 09/18/00 - LRM says upward relative inst before rooted-think wrong */
 if (modsyp != NULL && modsyp->el.emdp->minstnum == 0)
  {
   imdp = modsyp->el.emdp;
   /* first inform if upward instance but not top */
   if ((syp2 = fnd_uprel_inst(ncmp, grp->gin_mdp->moditps[0])) != NULL
    && syp2->el.emdp->minstnum != 0)
    {
     __sgfinform(464,
      "hierarchical path %s rooted but head %s also other and lower upward instance - rooted path used",
      grp->gnam, ncmp);
   }
   /* notice can't be upward module since module is top or not get here */
   goto found_ref;
  }
 /* see if upward inst that is not rooted - use 0th instance for find */
 /* works because all upward relative paths must find same module type */
 in_itp = grp->gin_mdp->moditps[0];
 if ((syp = fnd_uprel_inst(ncmp, in_itp)) != NULL)
  {
   /* found an upward relative inst, if more than one in src, error */
   /* if do not find same module for all */
   if (grp->gin_mdp->minstnum > 1) 
    {
     /* if instantiated in many places must always find exactly same */
     /* uprel module type */
     if (!chk_all_uprels_same(grp, modsyp, ncmp, ghd_ndp)) return(FALSE);
    }

   /* inform if found inst and head also upward mod name - know not rooted */
   if (modsyp != NULL && fnd_uprel_mod(modsyp, in_itp))
    {
     __sgfinform(464,
      "hierarchical path %s head %s upward relative instance but %s also upward module - instance used",
      grp->gnam, ncmp, ncmp);
    }
   /* if found, uprel instance it becomes mod sym */
   modsyp = syp; 
   if (grp->gin_mdp->minstnum > 1) 
    {
     /* if instantiated in many places must always find exactly same */
     /* uprel module type */
     if (!chk_all_uprels_same(grp, modsyp, ncmp, ghd_ndp)) return(FALSE);
    }
   goto found_ref;
  }
 /* see if upward relative module - inst array select impossible */
 if (comp_isel) 
  {
   __sgferr(692,
    "rooted or upward relative path %s head instance array select %s not found",
    grp->gnam, ncmp);
   return(FALSE);
  }
 /* if head is type name, see if upward relative module name */
 if (modsyp != NULL)
  {
   if (!fnd_uprel_mod(modsyp, in_itp)) goto try_tskscope;

   /* here no conflict possible but check for all same module containing */ 
   /* reference instantiated in source in different places */

   if (grp->gin_mdp->minstnum > 1) 
    {
     /* if instantiated in many places must always find exactly same */
     /* uprel module type */
     if (!chk_all_uprels_same(grp, modsyp, ncmp, ghd_ndp)) return(FALSE);
    }
   goto found_ref;
  } 

try_tskscope:
 /* FIXME - will this be seen as XMR? - think not and test */
 /* finally try to find name as task/func/named block (other scope) */
 /* anywhere in upward module - must be one component non select form */
 if (ghd_ndp->ru.x == NULL)
  {
   if ((imdp = fnd_uprel_tskfunc(&scope_syp, ncmp, in_itp)) == NULL)
    goto ref_err;

   __last_gsc = 1;
   __glbsycmps[0] = imdp->msym;
   __glbxcmps[0] = NULL;
   __glbsycmps[__last_gsc] = scope_syp;
   __glbxcmps[__last_gsc] = NULL; 

   /* this must always be upward relative even if in top module */
   grp->upwards_rel = TRUE; 
   grp->is_upreltsk = TRUE;
   grp->targtskp = scope_syp->el.etskp;
   /* here can't call fill grest syms */
   return(TRUE);
  }

 /* finally, not found */
ref_err:
 /* SJM 03/29/02 - for 1 component - assume undeclared */
 if (ghd_ndp->ru.x == NULL)
  {
   /* DBG remove */
   if (strcmp(ncmp, grp->gnam) != 0) __misc_terr(__FILE__, __LINE__);
   /* --- */
   __sgferr(757,
    "%s undeclared - possibly undeclared one component cross module reference",
    grp->gnam);
  }
 else
  {
   __sgferr(757,
    "rooted, downward relative or upward relative hierarchical reference %s first component %s not found - illegal path head or undeclared",
    grp->gnam, ncmp);
  }
 return(FALSE);
 
found_ref:
 /* know this is module and if non rooted for now assume above */
 __glbsycmps[0] = modsyp;
 __glbxcmps[0] = NULL;
 __last_gsc = 0;
 /* notice cannot check if upward really above in itree until prep. time */
 if (modsyp->el.emdp->minstnum != 0) grp->upwards_rel = TRUE; 
 else grp->is_rooted = TRUE;
 return(TRUE);
}

/*
 * for every instance of gin module type, find uprel dest and check to
 * make sure destination the same
 *
 * also checks each instance's uprel
 */
static int32 chk_all_uprels_same(struct gref_t *grp, struct sy_t *modsyp,
 char *ncmp, struct expr_t *ghd_ndp)
{
 register int32 ii;
 register struct itree_t *in_itp;
 int32 uprel_typ, got_one, fnd_ii, sav_ecnt; 
 struct sy_t *uprel_syp, *scope_syp, *syp;
 struct mod_t *imdp;

 uprel_typ = -1;
 uprel_syp = NULL;
 fnd_ii = -1;
 got_one = FALSE;
 sav_ecnt = __pv_err_cnt;
 for (ii = 0; ii < grp->gin_mdp->flatinum; ii++)
  {
   in_itp = grp->gin_mdp->moditps[ii];
   /* upward relative instance name - symbol is module not inst */
   if ((syp = fnd_uprel_inst(ncmp, in_itp)) != NULL)
    {
     if (!got_one)
      { uprel_typ = SYM_I; uprel_syp = syp; got_one = TRUE; fnd_ii = ii; }
     else
      {
       if (uprel_typ != SYM_I || syp != uprel_syp) 
        {
        __sgferr(939,
         "rooted or upward relative instance reference %s for instance %s differs from upward reference for instance %s",
         grp->gnam, __msg2_blditree(__xs, in_itp),
         __msg2_blditree(__xs2, grp->gin_mdp->moditps[fnd_ii]));
        }
      }
     continue; 
    }

   /* upward relative type name */
   if (modsyp != NULL)
    { 
     /* this returns T on success */
     if (fnd_uprel_mod(modsyp, in_itp))
      {
       if (!got_one)
        { uprel_typ = SYM_M; uprel_syp = modsyp; got_one = TRUE; fnd_ii = ii; }
       else
        {
         if (uprel_typ != SYM_M || modsyp != uprel_syp) 
          {
           __sgferr(3402,
           "upward relative module type reference %s for instance %s differs from upward reference for instance %s",
           grp->gnam, __msg2_blditree(__xs, in_itp),
           __msg2_blditree(__xs2, grp->gin_mdp->moditps[fnd_ii]));
          }
        }
       continue; 
      }
    }

   /* if only one component, look for upward somwhere task/func */
   if (ghd_ndp->ru.x == NULL)
    {
     if ((imdp = fnd_uprel_tskfunc(&scope_syp, ncmp, in_itp)) != NULL)
      { 
       if (!got_one)
        {
         uprel_typ = SYM_TSK;
         uprel_syp = scope_syp;
         got_one = TRUE;
         fnd_ii = ii;
        }
       else
        {
         if (uprel_typ != SYM_TSK || scope_syp != uprel_syp) 
          {
           __sgferr(3402,
           "upward relative task/func/named block reference %s for instance %s differs from upward reference for instance %s",
           grp->gnam, __msg2_blditree(__xs, in_itp),
           __msg2_blditree(__xs2, grp->gin_mdp->moditps[fnd_ii]));
          }
        }
       continue; 
      }
    } 
   __sgferr(757,
    "rooted or upward relative hierarchical reference %s first component %s not found for instance %s - illegal symbol or undeclared",
    grp->gnam, ncmp, __msg2_blditree(__xs, in_itp));
  }
 if (__pv_err_cnt > sav_ecnt) return(FALSE);
 return(TRUE);
}

/*
 * match upward relative instance - if found returns mod symbol
 */
static struct sy_t *fnd_uprel_inst(char *ncmp, struct itree_t *in_itp)
{
 register struct itree_t *up_itp;
 struct inst_t *ip;

 for (up_itp = in_itp; up_itp != NULL;)
  { 
   ip = up_itp->itip;
   /* know insts in split off module types have unchanged inst name */
   /* FIXME - should compare inst exprs not just iarr element name */
   if (strcmp(ncmp, ip->isym->synam) == 0) return(ip->imsym);
   up_itp = up_itp->up_it;
  }
 return(NULL);
}

/*
 * match upward relative instance - if found returns T else FALSE
 */
static int32 fnd_uprel_mod(struct sy_t *modsyp, struct itree_t *in_itp)
{
 register struct itree_t *up_itp;
 struct mod_t *up_mdp, *up_mast_mdp, *imdp, *mast_imdp;
 struct inst_t *ip;

 up_mdp = modsyp->el.emdp;
 up_mast_mdp = __get_mast_mdp(up_mdp);
 for (up_itp = in_itp; up_itp != NULL;)
  {
   /* must match master mod type */
   ip = up_itp->itip;
   imdp = ip->imsym->el.emdp;
   mast_imdp = __get_mast_mdp(imdp);
   if (mast_imdp == up_mast_mdp) return(TRUE);
   up_itp = up_itp->up_it;
  }
 return(FALSE);
}

/*
 * find an upward relative task/func/named block anywhere in uprel inst
 * if found return module type and set scope syp 
 */
static struct mod_t *fnd_uprel_tskfunc(struct sy_t **scope_syp, char *ncmp,
 struct itree_t *in_itp)
{
 register struct itree_t *up_itp;
 struct sy_t *tskfunc_syp; 
 struct mod_t *imdp;

 for (up_itp = in_itp; up_itp != NULL;)
  { 
   imdp = up_itp->itip->imsym->el.emdp;
   if ((tskfunc_syp = fnd_tskfunc_inscope(ncmp, imdp->msymtab)) != NULL)
    {
     *scope_syp = tskfunc_syp;
     return(imdp);
    }
   up_itp = up_itp->up_it;
  }
 return(NULL);
}

/*
 * find a scope symbol inside one symbol table scope level    
 */
static struct sy_t *fnd_tskfunc_inscope(char *ncmp,
 struct symtab_t *scope_sytp)
{
 struct symtab_t *sytp;
 struct sy_t *match_syp;

 for (sytp = scope_sytp->sytofs; sytp != NULL; sytp = sytp->sytsib)  
  {
   if (strcmp(sytp->sypofsyt->synam, ncmp) == 0) return(sytp->sypofsyt);
   if ((match_syp = fnd_tskfunc_inscope(ncmp, sytp)) != NULL)
    return(match_syp);
  }
 return(NULL);
}

/*
 * set hierarchical global reference symbols for rest of path
 * know head (pos. 0) filled
 */
static int32 fill_grestsyms(struct gref_t *grp, struct expr_t *gcmp_ndp)
{
 register int32 gi;
 int32 comp_isel;
 struct sy_t *syp;
 struct symtab_t *sytp;
 struct expr_t *ndp;
 char *ncmp;

 syp = __glbsycmps[0];
 if (syp->sytyp == SYM_I) sytp = syp->el.eip->imsym->el.emdp->msymtab;
 else sytp = syp->el.emdp->msymtab;

 for (gi = 1;; gi++)
  {
   comp_isel = FALSE;
   if (gcmp_ndp->lu.x->optyp == XMRID) ndp = gcmp_ndp->lu.x;
   else if (gcmp_ndp->lu.x->optyp == LSB)
    { ndp = gcmp_ndp->lu.x->lu.x; comp_isel = TRUE; }
   /* SJM 02/02/05 - part select legal but same as ID here */
   else if (gcmp_ndp->lu.x->optyp == PARTSEL) ndp = gcmp_ndp->lu.x->lu.x;
   else { __case_terr(__FILE__, __LINE__); return(FALSE); }

   ncmp = ndp->ru.qnchp;
   if ((syp = __zget_sym(ncmp, sytp->stsyms, sytp->numsyms)) == NULL)
    {
     __sgferr(758, "hierarchical path %s component %s undeclared in %s",
      grp->gnam, ncmp, sytp->sypofsyt->synam);
     return(FALSE);
    }
   if (++__last_gsc >= MAXGLBCOMPS - 1)
    {
     __pv_terr(310, "hierarchical path %s has too many components (%d)",
      grp->gnam, MAXGLBCOMPS);
    }
   __glbsycmps[__last_gsc] = syp;
   if (comp_isel)
    {
     __glbxcmps[__last_gsc] = gcmp_ndp->lu.x->ru.x;
     grp->path_has_isel = TRUE;
    }
   else __glbxcmps[__last_gsc] = NULL;
  
   /* here global ending with instance name is ok - non scope caught later */
   if (syp->sytyp == SYM_I)
    {
     /* if instance, move to it module type symbol table */
     sytp = syp->el.eip->imsym->el.emdp->msymtab;
     if ((gcmp_ndp = gcmp_ndp->ru.x) == NULL) return(TRUE);
     continue;
    }
   /* if not legal in module hierarchical name type, error */
   if (syp->sytyp == SYM_N)
    {
     if (gcmp_ndp->ru.x != NULL)
      {
       __sgferr(761,
        "hierarchical path %s net %s not rightmost path element",
        grp->gnam, ncmp); 
       return(FALSE);
      }
     /* SJM 05/02/00 - hierarchical params legal following XL - add if not */
     /* --
     if (syp->el.enp->n_isaparam)
      {
       __sgferr(762, "path %s component %s illegal hierachical reference to parameter",
        grp->gnam, ncmp, __to_sytyp(__xs, syp->sytyp)); 
       return(FALSE);
      }
     --- */
     break;
    }
   if (syp->sytyp != SYM_TSK && syp->sytyp != SYM_F && syp->sytyp != SYM_LB)
    {
     __sgferr(762, "hierarchical path %s component %s illegal %s symbol type",
      grp->gnam, ncmp, __to_sytyp(__xs, syp->sytyp)); 
     return(FALSE);
    }
   /* if last symbol, end with task not wire type symbol which is ok */
   if (gcmp_ndp->ru.x == NULL)
    {
     if (gcmp_ndp->lu.x->optyp == LSB)
      {
       __sgferr(693,
        "hierarchical reference %s select of last symbol task %s illegal",
        grp->gnam, __to_sytyp(__xs, syp->sytyp));
       return(FALSE);
      }
     grp->targtskp = syp->el.etskp;
     break;
    }

   /* must be some kind of task or will not get here */
   sytp = syp->el.etskp->tsksymtab; 
   /* section of path containing in module symbols removed since only need */
   /* pointer to symbol and inst. number for var. access same */
   /* will check for parameter at end of path or wrong thing in here */
   gcmp_ndp = gcmp_ndp->ru.x;
   if ((syp = find_inmod_sym(grp, gcmp_ndp, syp, &sytp)) != NULL) 
    {
     /* by here skipped over all in module symbols, syp is the end */
     __last_gsc = gi;
     __glbsycmps[__last_gsc] = syp;
     /* since in mod, can never be select - error already caught */
     __glbxcmps[__last_gsc] = NULL; 
     grp->targtskp = sytp->sypofsyt->el.etskp;
     break;
    }
   /* if found match but rest is not in module global - keep looking */
   /* and will emit error */
  }
 return(TRUE);
}

/*
 * find in module symbol - know previous symbol selected module  
 * or local task/block/function hierarchical name root
 *
 * end of path here can be scope named block/func/task/mod/inst etc.
 * this must also return (set) symbol table where last symbol declared
 */
static struct sy_t *find_inmod_sym(struct gref_t *grp, struct expr_t *gcmp_ndp,
 struct sy_t *syp, struct symtab_t **endsytp)
{
 struct expr_t *ndp;
 struct symtab_t *sytp;

 sytp = *endsytp;
 for (;;)
  {
   if (gcmp_ndp->lu.x->optyp == XMRID) ndp = gcmp_ndp->lu.x;
   else if (gcmp_ndp->lu.x->optyp == LSB)
    {
     __sgferr(694,
     "hierarchical path %s select of %s component illegal - not instance array",
      grp->gnam, __to_sytyp(__xs, syp->sytyp));
     return(NULL);
    }
   else { __case_terr(__FILE__, __LINE__); return(FALSE); }

   syp = __zget_sym(ndp->ru.qnchp, sytp->stsyms, sytp->numsyms);
   if (gcmp_ndp->ru.x == NULL) break;

   if (syp == NULL)
    {
     /* did not find next path component */
     __sgferr(763, "hierarchical path %s component %s undeclared", grp->gnam,
      ndp->ru.qnchp);
     return(NULL);
    } 
   /* SJM 10/07/06 - also legal for local XMR reference nets in tasks and */
   /* functions declared within the current module (i.e. t.r) */
   if (syp->sytyp != SYM_LB && syp->sytyp != SYM_TSK && syp->sytyp != SYM_F)
    {
     __sgferr(764, 
      "hierarchical path %s internal component %s type %s instead of expected named block",
      grp->gnam, ndp->ru.qnchp, __to_sytyp(__xs, syp->sytyp));
     return(NULL);
    }
   sytp = syp->el.etskp->tsksymtab; 
   gcmp_ndp = gcmp_ndp->ru.x;
  }
 /* syp is tail (actual variable), if declared replace - checking later */
 if (syp == NULL)
  {
   __sgferr(765,
    "internal hierarchical path %s part last component %s undeclared",
    grp->gnam, ndp->ru.qnchp);
   return(NULL);
  }
 if (!chk_xmr_tail_wire(grp, syp, FALSE)) return(NULL);
 *endsytp = sytp;
 return(syp);
}

/*
 * check global selects 
 *
 * error if select of non inst array and non select of inst. array
 * this is called during global input, can not check select exprs
 */
static int32 chk_glb_inst_sels(struct gref_t *grp)
{
 register int32 gi;
 int32 good, ii;
 struct sy_t *syp, *up_syp;
 struct inst_t *ip;
 struct mod_t *mdp, *up_mdp;

 for (good = TRUE, up_syp = NULL, gi = 0; gi <= __last_gsc; gi++)
  {
   syp = __glbsycmps[gi];
   if (syp->sytyp != SYM_I)
    {
     if (__glbxcmps[gi] != NULL)
      {
       __sgferr(695,
        "hierarchical path %s illegal select of %s symbol - never arrayed",
        grp->gnam, __to_sytyp(__xs, syp->sytyp));
       good = FALSE;
      }
     continue;
    }
   /* know symbol is instance */
   /* if inst select but not array of instances, select illegal */
   ip = syp->el.eip;
   mdp = ip->imsym->el.emdp;
   /* know this component inst, need to access up module */
   if (gi == 0)
    {
     /* first component instance mean down relative */
     /* DBG remove -- */
     if (grp->is_rooted) __misc_terr(__FILE__, __LINE__);
     /* --- */
     up_mdp = grp->gin_mdp;
    }
   else
    {
     up_syp = __glbsycmps[gi - 1];
     if (up_syp->sytyp == SYM_I) up_mdp = up_syp->el.eip->imsym->el.emdp;
     else if (up_syp->sytyp == SYM_M) up_mdp = up_syp->el.emdp;
     else { up_mdp = NULL;__case_terr(__FILE__, __LINE__); }
    }
   ii = ((byte *) ip - (byte *) up_mdp->minsts)/sizeof(struct inst_t);

   if (up_mdp->miarr == NULL || up_mdp->miarr[ii] == NULL)
    {
     if (__glbxcmps[gi] == NULL) continue;
     __sgferr(696,
      "hierarchical path %s select illegal for non vectored instance %s (type %s)",
      grp->gnam, ip->isym->synam, mdp->msym->synam);
     good = FALSE;
     continue;
    }
   /* final case vectored instance - select required */ 
   if (__glbxcmps[gi] != NULL) continue;
   __sgferr(697,
    "hierarchical path %s missing required select of vectored instance %s (type %s)",
    grp->gnam, ip->isym->synam, mdp->msym->synam);
   good = FALSE;
  }
 return(good);
}

/*
 * ROUTINES TO ACCESS AND BUILD GLOBALS USING STRINGS
 */

/*
 * convert global reference as string into global path expression
 *
 * no white space allowed except to end escaped IDs
 * handles escaped IDs (\...[white space] and ending [<number>] for inst arrays 
 * source xmr's can have spaces but not allowed here - make sure documented
 *
 * this can return one component xmr
 */
extern struct expr_t *__glbnam_to_expr(char *irefs)
{
 int32 slen, ind, esc_name;
 struct expr_t *glbndp, *gcmp_ndp, *sel_ndp;
 char *chp, *chp2, *chp3, *sepchp;
 char s1[IDLEN], s2[IDLEN], s3[IDLEN];

 /* allocate top node */
 glbndp = __alloc_newxnd();
 glbndp->optyp = GLBREF;
 gcmp_ndp = glbndp;
 for (chp = irefs;;)
  {
   /* separate out one component */
   /* case 1: escaped name */
   esc_name = FALSE;
   if (*chp == '\\') 
    {
     if (chp[1] == ' ') 
      {
       __sgferr(1138, 
        "global reference %s illegal - empty escaped component illegal",
        irefs); 
bad_end:
       __free_xtree(glbndp);
       return(NULL);
      }
     if ((chp2 = strchr(chp, ' ')) == NULL)
      {
       __sgferr(1138, 
        "global reference %s illegal - escaped component ending space missing",
        irefs); 
       goto bad_end;
      }
     /* include one space in escaped ID */
     chp2++;
     strncpy(s1, chp, chp2 - chp);
     /* escaped ID now extracted into s1 */
     s1[chp2 - chp] = '\0';
     while (*chp2 == ' ') chp2++;
     if (*chp2 == '[')
      {
       if ((chp3 = strchr(chp2, ']')) == NULL)
        {
         __sgferr(1139, 
          "global reference %s illegal - instance array select missing ']'",
          irefs);
         goto bad_end; 
        }
       chp3++;
       strncpy(s2, chp2, chp3 - chp2); 
       s2[chp3 - chp2] = '\0';
       strcat(s1, s2); 
       /* one past ending ']' */ 
       sepchp = chp3;
      }
     else sepchp = chp2;
     esc_name = TRUE;
    }
   else
    { 
     /* case 2: non escaped maybe instance array select */ 
     if ((chp2 = strchr(chp, '.')) != NULL)
      {
       strncpy(s1, chp, chp2 - chp);
       s1[chp2 - chp] = '\0';
       /* point to '.' separator */
       sepchp = chp2;
      }
     else
      {
       strcpy(s1, chp);
       slen = strlen(chp);
       /* point to '\0' */
       sepchp = &(chp[slen]);
      }
    }
   /* s1 contains component maybe with select */
   if (esc_name || (chp = strchr(s1, '[')) == NULL) 
    {
     /* case 1: non instance select form */
     gcmp_ndp->ru.x = __alloc_newxnd();
     gcmp_ndp->ru.x->optyp = XMRCOM;
     gcmp_ndp = gcmp_ndp->ru.x;
     gcmp_ndp->lu.x = __alloc_newxnd();
     gcmp_ndp->lu.x->optyp = XMRID;
     gcmp_ndp->lu.x->ru.qnchp = __pv_stralloc(s1);
    }
   else 
    {
     /* case 2: instance select form */
     /* chp points into [ in s1 */
     strncpy(s2, s1, chp - s1); 
     s2[chp - s1] = '\0';
     if ((chp2 = strchr(chp, ']')) == NULL)
      {
       __sgferr(1139, 
        "global reference %s illegal - instance array select missing ']'",
        irefs);
       goto bad_end; 
      }
     chp++;
     /* chp point to first digit that gets put into s3 */
     strncpy(s3, chp, chp2 - chp); 
     s3[chp2 - chp] = '\0';
     for (chp = s3; *chp != '\0'; chp++)
      {
       if (!isdigit(*chp))
        {
bad_sel_index:
          __sgferr(1139, 
          "global reference %s illegal - instance select must be number",
          irefs);
         goto bad_end;
        }
      }
     if (sscanf(s3, "%d", &ind) != 1 || ind < 0) goto bad_sel_index;
     sel_ndp = __alloc_newxnd();
     sel_ndp->optyp = LSB;
     sel_ndp->lu.x = __alloc_newxnd();
     sel_ndp->lu.x->optyp = XMRID;
     sel_ndp->lu.x->ru.qnchp = __pv_stralloc(s2);
     sel_ndp->ru.x = __alloc_newxnd();
     __set_numval(sel_ndp->ru.x, (word32) ind, 0L, WBITS);

     gcmp_ndp->ru.x = __alloc_newxnd();
     gcmp_ndp->ru.x->optyp = XMRCOM;
     gcmp_ndp = gcmp_ndp->ru.x;
     gcmp_ndp->lu.x = sel_ndp;
    } 
   /* sepchp points into separae iref string */
   if (*sepchp  == '\0') break;
   else if (*sepchp != '.') 
    {
     __sgferr(1139, 
      "global reference %s internal component not followed by '.' separator",
      irefs);
     goto bad_end; 
    } 
   chp = sepchp;
   chp++;
  }
 return(glbndp);
}

/*
 * return pointer to component name of current component of global expr 
 *
 * FIXME - this returns instance name but does not deal with number index
 */
extern char *__to_glbcmp_nam(struct expr_t *gcmp_ndp)
{
 char *chp;

 if (gcmp_ndp->lu.x->optyp == XMRID) chp = gcmp_ndp->lu.x->ru.qnchp;
 else if (gcmp_ndp->lu.x->optyp == LSB || gcmp_ndp->lu.x->optyp == PARTSEL)
  chp = gcmp_ndp->lu.x->lu.x->ru.qnchp;
 else { __case_terr(__FILE__, __LINE__); return(NULL); }
 return(chp);
}

/*
 * ROUTINES TO FREE GLOBAL DATA STRUCTURES
 */

/*
 * free a defparam
 * may want to keep defparams and global for debugging
 * but latest value of parameter is what is needed not defparam
 *
 * global marked as gone so will be freed later
 */
extern void __free_1dfparam(struct dfparam_t *dfpp)
{
 if (dfpp->last_dfpi > -1 && dfpp->dfpiis != NULL)
  __my_free((char *) dfpp->dfpiis, (dfpp->last_dfpi + 1)*sizeof(int32));
 __free_xtree(dfpp->dfpxlhs);
 /* notice cannot free rhs since moved to parameter value rhs */
 /* if error, will stop so no need to free */
 __my_free((char *) dfpp, sizeof(struct dfparam_t));
}

/*
 * free globals marked to be freed - mostly for defparam tmp globals
 *
 * must remove gone and err from grtab and fix up gxndp ptrs or
 * will process with empty guts and can't skip steps if only defparams
 */
static void free_gone_glbs(void)
{ 
 register int32 gri, ngri;
 register struct gref_t *grp;
 register struct mod_t *mdp;
 struct gref_t *ngrtab;
 int32 ngrnum;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mgrnum == 0) continue;
   __push_wrkitstk(mdp, 0);

   /* first count remaining */
   grp = &(mdp->mgrtab[0]);
   for (ngrnum = gri = 0; gri < __inst_mod->mgrnum; grp++, gri++) 
    {
     /* if global gone or error will not get counted at all */
     if (grp->gr_err || grp->gr_gone) continue; 
     ngrnum++;
    }
   /* nothing to remove */
   if (ngrnum == __inst_mod->mgrnum) goto nxt_mod;
   /* all removed */
   if (ngrnum == 0)
    {
     __my_free((char *) __inst_mod->mgrtab,
      __inst_mod->mgrnum*sizeof(struct gref_t));
     __inst_mod->mgrtab = NULL;
     __inst_mod->mgrnum = 0;
     goto nxt_mod;
    }

   ngrtab = (struct gref_t *) __my_malloc(ngrnum*sizeof(struct gref_t));
   grp = &(mdp->mgrtab[0]);
   for (ngri = gri = 0; gri < __inst_mod->mgrnum; grp++, gri++) 
    {
     /* this frees contents of global but left in table */
     if (grp->gr_err || grp->gr_gone) { __free_1glb_flds(grp); continue; }

     ngrtab[ngri] = *grp; 
     ngrtab[ngri].gxndp->ru.grp = &(ngrtab[ngri]);
     ngri++;
    }
   __my_free((char *) __inst_mod->mgrtab,
    __inst_mod->mgrnum*sizeof(struct gref_t));
   __inst_mod->mgrtab = ngrtab;
   __inst_mod->mgrnum = ngrnum;

nxt_mod:
   __pop_wrkitstk();
  }
}

/*
 * allocate and fill new gref table 
 *
 * also always fixes up r_u ptrs from expr. to to newly alloced global
 */
extern struct gref_t *__alloc_grtab(struct gref_t *oldgrtab, int32 grnum)
{
 register int32 gri;
 register struct gref_t *grp; 
 struct gref_t *ngrtab;

 /* DBG remove -- */
 if (grnum <= 0) __arg_terr(__FILE__, __LINE__);
 /* --- */
 ngrtab = (struct gref_t *) __my_malloc(grnum*sizeof(struct gref_t));
 memcpy(ngrtab, oldgrtab, grnum*sizeof(struct gref_t));

 /* pointer from expr nodes need to be fixed to point to new addr */
 grp = &(ngrtab[0]);
 for (gri = 0; gri < grnum; grp++, gri++) grp->gxndp->ru.grp = grp; 
 return(ngrtab);
}

/*
 * free 1 global's fields 
 *
 * caller must free table containing this global if needed
 * LOOKATME - what if gnam in name table (think never calling this then)? 
 */
extern void __free_1glb_flds(struct gref_t *grp)
{
 /* if using this for local qualifed name will be set to nil */
 if (grp->gnam != NULL) __my_free((char *) grp->gnam, strlen(grp->gnam + 1));
 grp->gnam = NULL;
 /* also free glb ref exprssion - always in malloced memory here */
 __free_xtree(grp->glbref);

 /* only sized targu is upwards rel list - may not have been alloced yet */
 if (grp->upwards_rel && grp->targu.uprel_itps != NULL)
  {
   __my_free((char *) grp->targu.uprel_itps,
    grp->gin_mdp->flatinum*sizeof(struct itree_t *));
  } 
 if (grp->grcmps != NULL)
  {
   __my_free((char *) grp->grcmps, (grp->last_gri + 1)*sizeof(struct sy_t *));
   grp->grcmps = NULL;
  }
 if (grp->grxcmps != NULL)
  {
   __my_free((char *) grp->grxcmps,
    (grp->last_gri + 1)*sizeof(struct expr_t *));
   grp->grxcmps = NULL;
  }
}

/*
 * ROUTINES TO DO POUND PARAM SPLITTING
 */

/*
 * work form bottom up marking all instances that need to be split
 * and all params that are width determining or indirect width determining 
 */
static void mark_poundparam_splitinsts(void)
{
 register int32 ii, mlevel, pi;
 int32 split_inst;
 struct mod_t *mdp;
 int32 giawid;
 struct inst_t *ip;
 struct mod_t *imdp;
 struct giarr_t *giap;
 struct expr_t *pxp;
 struct net_t *modnp;

 /* bottom modules which do not instantiate anything special case */
 for (mdp = __mdlevhdr[0]; mdp != NULL; mdp = mdp->mlevnxt)
  {
   /* for bottom pound params, if only one flat instance - nothing to do */
   /* because if only one flat instance then know anything instantiating */
   /* also only one flat inst */
   if (mdp->flatinum == 1) continue;

   __push_wrkitstk(mdp, 0);
   /* this also set mod has width determining bit */
   if (!mdp->mwiddetdone)
    { __mark_widdet_params(mdp); mdp->mwiddetdone = TRUE; }
   __pop_wrkitstk();
  }

 /* work upward in levelized dag marking width determining params and */ 
 /* instance that need to be split */
 __pndparam_splits = FALSE;
 for (mlevel = 1; mlevel <= __dagmaxdist; mlevel++)
  {
   for (mdp = __mdlevhdr[mlevel]; mdp != NULL; mdp = mdp->mlevnxt)
    {
     __push_wrkitstk(mdp, 0);

     /* width determining needs module context - always set here */
     /* if has width determining, becomes probable if ever instantiated */
     /* if only 1 inst in design no need to mark width determining */
     if (mdp->flatinum > 1)
      {
       if (!mdp->mwiddetdone)
        { __mark_widdet_params(mdp); mdp->mwiddetdone = TRUE; }
      }

     /* even if only one inst of mod, must still try to split insts of mod in*/ 

     /* for every instance in module, see if need to split */
     for (ii = 0; ii < __inst_mod->minum; ii++)
      {
       /* if instance in mod is inst array (first) never split and do not */
       /* even check other expanded from in source instance array */
       /* pound param splitting in inst arrays illegal */
       /* LOOKATME - is error emitted somewhere? */
       /* for contained insts that came from mi arrays, no pnd param split */
       if (__inst_mod->miarr != NULL && (giap = __inst_mod->miarr[ii]) != NULL)
        {
         giawid = __get_giarr_wide(giap);
         /* always inc by one in for loop above */
         ii += (giawid - 1);
         continue;
        }

       ip = &(__inst_mod->minsts[ii]);
       if (ip->ipxprtab == NULL) continue;

       /* LOOKATME - think these not used */
       __sfnam_ind = ip->isym->syfnam_ind;
       __slin_cnt = ip->isym->sylin_cnt;

       /* imdp is module type of instance that has pound param */
       imdp = ip->imsym->el.emdp;

       /* if one inst never split - even if in later splitting by here */
       /* spliting would result in 0 insts, mark here but stop when split */
       if (imdp->flatinum == 1)
        {
         if (__debug_flg)
          {
           __dbg_msg("++ # form inst. %s(%s) not split - only one instance\n",
           ip->isym->synam, imdp->msym->synam);
          } 
         continue;
        }
       /* know contained module has all width determining params set */
       /* if no width determining and no indir width determining, done */
       if (!imdp->mhas_widthdet && !imdp->mhas_indir_widdet)
        {
         if (__debug_flg)
          {
           __dbg_msg(
            "++ # form inst. %s(%s) not split - no params in ranges\n",
            ip->isym->synam, imdp->msym->synam);
          } 
         continue;
        }

       split_inst = FALSE;
       /* go through defined params in one down mod */
       for (pi = 0; pi < imdp->mprmnum; pi++)
        {
         pxp = ip->ipxprtab[pi];
         /* explicit form this one unused or short list */
         if (pxp == NULL) continue;

         modnp = &(imdp->mprms[pi]);
         /* if down mod has width determining, but none of the width */
         /* determining passed down to this inst. as pound param, no split */
         if (modnp->nu.ct->n_widthdet || modnp->nu.ct->n_indir_widthdet)
          {
           /* set indirect width determining */

           /* if no up expr params set as indirect width determining, do not */
           /* need to split, but other source locs insts of type may split */
           /* returns T if any set */
           if (indir_widthdet_markparam(pxp)) split_inst = TRUE;
          }
        }
       /* if no width determining and no indir width determining, done */
       if (!split_inst)
        {
         if (__debug_flg)
          {
           __dbg_msg(
            "++ # form inst. %s(%s) not split - no range # params\n",
            ip->isym->synam, imdp->msym->synam);
          } 
        }
       ip->i_pndsplit = TRUE; 
      }
     __pop_wrkitstk();
    }
  }
}

/*
 * set the indirect width determining bit for any wire up pound param expr
 * return T if any params in this up expr else F
 */
static int32 indir_widthdet_markparam(struct expr_t *xp)
{
 int32 rv, rv2;
 struct net_t *np;

 rv = rv2 = FALSE;
 if (__isleaf(xp))
  {
   if (xp->optyp == ID && xp->lu.sy->sytyp == SYM_N)
    {
     np = xp->lu.sy->el.enp; 
     if (np->n_isaparam)
      {
       /* if already width determining, can't be indir width determining */
       if (!np->nu.ct->n_widthdet)
        {
         np->nu.ct->n_indir_widthdet = TRUE;
         __inst_mod->mhas_indir_widdet = TRUE;
         rv = TRUE;
        }
      }
    }
   return(rv);
  }
 if (xp->lu.x != NULL) rv = indir_widthdet_markparam(xp->lu.x);
 if (xp->ru.x != NULL) rv2 = indir_widthdet_markparam(xp->ru.x);
 if (rv || rv2) return(TRUE);
 return(FALSE);
}

/*
 * work from top down splitting off static instance locations for
 * pound parameters that are width determining or indirect width determining
 * such instances are already marked
 *
 * splitting here may produce modules with more than 1 flat instance
 */
static void do_poundparam_splitting(void)
{
 register int32 ii, mlevel;
 register struct mod_t *mdp;
 int32 giawid;
 struct inst_t *ip;
 struct mod_t *imdp;
 struct giarr_t *giap;

 __pndparam_splits = FALSE;
 /* work from top (mod) down - since need fixed no pound param start */
 /* LOOKATME - wonder if bottom up works - think not */
 for (mlevel = __dagmaxdist; mlevel >= 1; mlevel--)
  {
   /* SJM 03/16/04 - notice processing from one up and splitting (if needed) */
   /* contained insts - therefore mlevel of md lev hdr list not chged */
   /* inside this loop - mlevel one less may use update list */
   for (mdp = __mdlevhdr[mlevel]; mdp != NULL; mdp = mdp->mlevnxt)
    {
     __push_wrkitstk(mdp, 0);
 
     /* do all splitting from pound params - also check but cannot set */
     /* for every instance in module */
     for (ii = 0; ii < __inst_mod->minum; ii++)
      {
       /* if instance in mod is inst array (first) never split and do not */
       /* even check other expanded from in source instance array */
       /* for contained insts that came from mi arrays, no pnd param split */
       if (__inst_mod->miarr != NULL && (giap = __inst_mod->miarr[ii]) != NULL)
        {
         giawid = __get_giarr_wide(giap);
         /* always inc by one in for loop above */
         ii += (giawid - 1);
         continue;
        }

       ip = &(__inst_mod->minsts[ii]);
       if (!ip->i_pndsplit) continue;
       
       /* when splitting marked, marked for all but do not split last */
       /* when splitting would result in nonsense 0 insts */

       imdp = ip->imsym->el.emdp;
       if (imdp->flatinum == 1 || imdp->flatinum == __inst_mod->flatinum)
        {
         if (__debug_flg)
          {
           __dbg_msg(
            "++ # form inst. %s(%s) not split - last %d splittable insts\n",
            ip->isym->synam, ip->imsym->synam, __inst_mod->flatinum);
          } 
         continue;
        }

       /* DBG remove -- */
       if (__inst_mod->flatinum == 0) __misc_terr(__FILE__, __LINE__);
       /* --- */

       if (__debug_flg)
        {
         __dbg_msg(
          "++ # form inst. %s(%s) split (%d new instances)\n",
          ip->isym->synam, imdp->msym->synam, __inst_mod->flatinum);
        } 

       __sfnam_ind = ip->isym->syfnam_ind;
       __slin_cnt = ip->isym->sylin_cnt;

       /* imdp is module type of instance that has pound param */
       /* SJM 03/16/04 - notice mlevel is one above imdp being split off */
       split_upd_mod(imdp, ip, mlevel); 
      }
     __pop_wrkitstk();
    }
  }
}

/*
 * split (copy) module from pound params or arrays of gate/insts where
 * assumes tmp itree loc set and sets/uses various other globals
 * 
 * called after conditions for split determined to be true
 * does copy and update the static (in source text) instance d.s.
 */
static void split_upd_mod(struct mod_t *imdp, struct inst_t *ip, int32 mlevel) 
{
 struct mod_t *ipmdp, *sav_inst_mod;

 /* this is one up module containing instance for which new module split off */
 sav_inst_mod = __inst_mod;
 __do_mdsplit(imdp);
 __pndparam_splits = TRUE;

 /* after here new (in __inst_mod) set to new, and old set to old - 1 */ 
 /* ipmdp is newly split off, sav cur mod if old split from */
 ipmdp = __inst_mod;
 /* adjust type of this instance */
 ip->imsym = ipmdp->msym;
 /* adjust inst numbers (spilt set new number to 1 and old to old - 1) */
 ipmdp->flatinum = sav_inst_mod->flatinum; 
 if (ipmdp->flatinum > 1) ipmdp->minstnum = 2;

 /* imdp is old, reduce by number of times containing module instantiated */
 imdp->flatinum -= sav_inst_mod->flatinum;
 /* during splitting reduced count by one too many - adjust back */
 imdp->flatinum++;
 /* SJM 02/17/00 - must adjust new split off module to have right inst */
 /* num */
 if (imdp->flatinum > 1) imdp->minstnum = 2;

 /* now adjust so both are masters that can be split from */
 /* this may cause lots of $$[num] prefixes */
 /* DBG remove --- */
 if (imdp->msplit) __misc_terr(__FILE__, __LINE__);
 /* --- */
 ipmdp->msplit = FALSE;
 /* if split off others from new, add new prefix starting at 1 */
 ipmdp->mversno = 0;
 imdp->mhassplit = FALSE;
 /* this must remain a defparm master since master applies only to */
 /* pealing off 1 instance paths from defparams */
 ipmdp->mspltmst = NULL;
 ipmdp->mpndsplit = TRUE;
 /* but need to record master of pound split off */
 ipmdp->mpndspltmst = imdp;

 /* SJM 03/16/04 - passed mlevel is one higher than imdp being split */  
 /* must add new split to dag list but for one down that is processed later */
 /* but must incrementally update the md lev hdr list during splitting */
 ipmdp->mlevnxt = __mdlevhdr[mlevel - 1];
 __mdlevhdr[mlevel - 1] = ipmdp;
 /* put back current module to up module instances in */
 __inst_mod = sav_inst_mod;
}

/*
 * ROUTINES TO SPLIT (COPY) MODULES
 */

/*
 * copy the module and link in to list module list unless only 1 inst.
 * __modsyms updated when this is done so split module insts. can be split
 *
 * if from pound param splitting must also copy non rooted defparams
 * split off module, splitting sets __inst_mod to point to it
 * so caller must save and restore __inst_mod
 */
extern void __do_mdsplit(struct mod_t *smdp)
{
 char s1[IDLEN];
 struct sy_t *syp;

 /* for master, version no. is highest of split off, else actual no. */
 if (!smdp->msplit)
  {
   /* __splitting a module effects exactly one flattened inst. too */
   /* flat inst counts and version numbers only kept in original master */
   (smdp->mversno)++;
   sprintf(s1, "%s$$%d", smdp->msym->synam, smdp->mversno);
  }
 else
  {
   /* for module, should never split from already split */
   __misc_terr(__FILE__, __LINE__);
  }
 __splitting = TRUE;
 copy_mod(smdp, s1);
 /* new split off module now pointed to by __inst_mod */
 __splitting = FALSE;

 /* must link on end so gets processed later */
 if (__end_mdp == NULL) __modhdr = __inst_mod;
  else __end_mdp->mnxt = __inst_mod;
 __end_mdp = __inst_mod;
 syp = smdp->msym;
 __gfinform(409, syp->syfnam_ind, syp->sylin_cnt,
  "module %s converted to new type - defparam or # parameter changes width",
  syp->synam); 
}

/*
 * copy a module - point __inst_mod to new module
 *
 * this is passed the master to copy out of
 * expects splitting to be true and adjust old module counts
 * new module does not have any defparams since info from master's
 * if from pound must copy non rooted defparams also
 *
 * this copies internal module components but does not insert new module
 * type in itree or up module's inst_t list
 *
 * BEWARE - non standard use of inst mod required here no itstk value
 */
static void copy_mod(struct mod_t *omdp, char *newnam)
{
 struct sy_t *syp;

 __oinst_mod = omdp;
 __inst_mod = (struct mod_t *) __my_malloc(sizeof(struct mod_t));

 /* notice this copies the flags which always stay the same */
 *__inst_mod = *omdp;

 __inst_mod->msplit = TRUE;
 __oinst_mod->mhassplit = TRUE;
 /* must add new name to modsyms - copy here can not effect undef mods */
 /* because all references resolved or will not get here */
 if ((syp = __add_modsym(newnam)) == NULL)
  {
   /* unable to split module to version and name */
   __misc_gfterr(__FILE__, __LINE__, omdp->msym->syfnam_ind,
    omdp->msym->sylin_cnt);
  }

 /* must set new sym and new version */
 syp->sydecl = TRUE;
 syp->el.emdp = __inst_mod;
 syp->syfnam_ind = __oinst_mod->msym->syfnam_ind;
 syp->sylin_cnt = __oinst_mod->msym->sylin_cnt;
 /* never need to connect through old splt sym here */
 __inst_mod->msym = syp;

 /* __splitting off from already split impossible */
 if (__oinst_mod->mspltmst != NULL) __misc_terr(__FILE__, __LINE__);

 /* not decing non flattented module insts. = only used to determine tops */
 (__oinst_mod->flatinum)--;
 /* can never be 0 instances of old copied from */
 if (__oinst_mod->flatinum == 1) __oinst_mod->minstnum = 1;
 __inst_mod->minstnum = 1;
 __inst_mod->flatinum = 1;

 /* connect current module to module type that was its source */
 /* if split off already split, link to real master */
 /* once split off no way to resplit since params frozen at split point */
 if (__oinst_mod->mspltmst != NULL)
  __inst_mod->mspltmst = __oinst_mod->mspltmst;
 else __inst_mod->mspltmst = __oinst_mod;

 /* caller responsible for linking into list */
 __inst_mod->mnxt = NULL;
 /* copy symbol table including all lower such as tasks and named blocks */
 copy_modsymtabs();

 /* must copy global references before copy any expressions */
 copy_mgrefs();

 /* never copy module's defparams since already moved to design wide list */
 /* also any grefs related to defparams removed by here */
 __inst_mod->mdfps = NULL;

 /* notice since params and ports really wires, must copy nets first */
 /* but after symbol table copied */
 copy_wires(__oinst_mod->msymtab);
 copy_modports();
 __inst_mod->mprms = copy_params(__oinst_mod->mprms, __oinst_mod->mprmnum,
  MODULE);

 /* AIV 09/27/06 - need to copy the local params as well */
 __inst_mod->mlocprms = copy_params(__oinst_mod->mlocprms, 
  __oinst_mod->mlocprmnum, MODULE);

 if (__oinst_mod->mdfps != NULL) copy_defparams();
 if (__oinst_mod->mattrs != NULL)
  __inst_mod->mattrs = copy_attrs(__oinst_mod->mattrs);
 if (__oinst_mod->mvarinits != NULL)
  __inst_mod->mvarinits = copy_varinits(__oinst_mod->mvarinits);

 copy_insts();
 copy_miarr();
 
 copy_gates();
 copy_mgarr();

 copy_contas();
 copy_mdtasks();

 __inst_mod->ialst = copy_ialst(__oinst_mod->ialst);
 if (__oinst_mod->mspfy != NULL) copy_specify();
}


/*
 * copy module symbol table structure
 * expects __oinst_mod to be set to old inst mod, __inst_mod to newly alloced
 * still must set all symbol el. union values
 * this does not copy specify symbol table for specparams
 */
static void copy_modsymtabs(void)
{
 struct symtab_t *nsytp, *osytp;

 osytp = __oinst_mod->msymtab;
 nsytp = copy_1symtab(osytp);
 nsytp->sytpar = NULL;
 nsytp->sytsib = NULL;
 nsytp->sytofs = NULL;
 /* this copyeis and connects by linking in pointers */
 if (osytp->sytofs != NULL) copy_lowsymtab(osytp->sytofs, nsytp);
 __inst_mod->msymtab = nsytp;
 nsytp->sypofsyt = __inst_mod->msym;
}

/*
 * depth first symbol table tree traversal across offspring
 */
static void copy_lowsymtab(register struct symtab_t *osytp,
 struct symtab_t *nupsytp)
{
 struct symtab_t *nsytp, *last_nsytp;

 for (last_nsytp = NULL; osytp != NULL; osytp = osytp->sytsib)
  {
   nsytp = copy_1symtab(osytp);
   if (last_nsytp == NULL) nupsytp->sytofs = nsytp;
   else last_nsytp->sytsib = nsytp;

   /* link up */
   nsytp->sytpar = nupsytp;
   /* copy underneath level (know only frozen tables copied) */
   if (osytp->sytofs != NULL) copy_lowsymtab(osytp->sytofs, nsytp);
   nsytp->sytsib = NULL;
   last_nsytp = nsytp;
  }
}

/*
 * copy one symbol table
 * caller fills in up and down links
 */
static struct symtab_t *copy_1symtab(struct symtab_t *osytp)
{
 int32 ofreezes;
 struct symtab_t *nsytp;

 ofreezes = osytp->freezes;
 nsytp = __alloc_symtab(ofreezes);
 *nsytp = *osytp;
 /* must leave as NULL if empty table */
 if (osytp->numsyms == 0) nsytp->stsyms = NULL;
 else nsytp->stsyms = copy_stsyms(osytp->stsyms, osytp->numsyms);

 /* when above symbol table was copied, old symbol links to new */
 /* SJM 12/26/03 - specify symbol table has no associated symbol */
 if (osytp->sypofsyt != NULL) nsytp->sypofsyt = osytp->sypofsyt->spltsy;
 else nsytp->sypofsyt = NULL;

 /* must link old to point to new (n_head now unused) */
 osytp->n_head = (struct tnode_t *) nsytp;
 return(nsytp);
}

/*
 * copy stsyms - know at least one symbol in table
 */
static struct sy_t **copy_stsyms(struct sy_t **osytab,
 word32 nsyms)
{
 register int32 i;
 struct sy_t **sytbp;
 struct sy_t *nsyp;
 int32 bytes;

 bytes = nsyms*sizeof(struct sy_t *);
 __wrkstab = (struct sy_t **) __my_malloc(bytes);
 for (i = 0; i < (int32) nsyms; i++)
  {
   /* allocate the new symbol */
   nsyp = (struct sy_t *) __my_malloc(sizeof(struct sy_t));
   *nsyp = *(osytab[i]);
   /* set union to NULL, any member will do */
   nsyp->el.ecp = NULL;
   __wrkstab[i] = nsyp;
   /* tmp link from old to new */
   osytab[i]->spltsy = nsyp;
  }
 sytbp = __wrkstab;
 return(sytbp);
}

/*
 * copy module ports
 */
static void copy_modports(void)
{
 register int32 pi;
 register struct mod_pin_t *ompp, *nmpp;
 int32 pnum;

 if ((pnum = __oinst_mod->mpnum) == 0) return;

 __inst_mod->mpins = (struct mod_pin_t *)
  __my_malloc(pnum*sizeof(struct mod_pin_t)); 

 nmpp = &(__inst_mod->mpins[0]);  
 for (pi = 0, ompp = &(__oinst_mod->mpins[0]); pi < pnum; pi++, ompp++, nmpp++)
  {
   *nmpp = *ompp;
   nmpp->mpref = __copy_expr(ompp->mpref);
  }
}

/*
 * copy wires from __oinst_mod to __inst_mod
 * also link in nsym and symbol table el.enp
 * know decl. symbol tables for these
 *
 * at this point do not know if wire correct - must copy from symbol table
 */
static void copy_wires(struct symtab_t *sytp)
{
 register int32 syi;
 struct sy_t **syms;
 struct sy_t *syp;
 struct net_t *onp, *nnp;

 for (syms = sytp->stsyms, syi = 0; syi < (int32) sytp->numsyms; syi++)
  {
   syp = syms[syi];
   if (syp->sytyp != SYM_N) continue;
   onp = syp->el.enp;
   /* do not copy parameters or specparams here */
   if (onp->n_isaparam) continue;

   /* must copy depending or original storage method */
   nnp = (struct net_t *) __my_malloc(sizeof(struct net_t));
   /* copy body */
   *nnp = *onp;
   /* allocate extra storage area */
   nnp->nu.ct = __alloc_arrncomp(); 
   /* copy ncomp body */
   *(nnp->nu.ct) = *(onp->nu.ct); 
   /* DBG remove --- */
   if (nnp->nrngrep != NX_CT) __misc_terr(__FILE__, __LINE__);
   /* --- */
   if (nnp->n_isavec)
    {
     nnp->nu.ct->nx1 = __copy_expr(onp->nu.ct->nx1);
     nnp->nu.ct->nx2 = __copy_expr(onp->nu.ct->nx2);
    }
   else nnp->nu.ct->nx1 = nnp->nu.ct->nx2 = NULL;

   if (nnp->n_isarr)
    {
     nnp->nu.ct->ax1 = __copy_expr(onp->nu.ct->ax1);
     /* notice this copies initial value expr. for params */
     nnp->nu.ct->ax2 = __copy_expr(onp->nu.ct->ax2);
    }
   else nnp->nu.ct->ax1 = nnp->nu.ct->ax2 = NULL;

   if (onp->nattrs != NULL) nnp->nattrs = copy_attrs(onp->nattrs);
   else nnp->nattrs = NULL;

   nnp->nu.ct->n_dels_u.pdels = __copy_dellst(onp->nu.ct->n_dels_u.pdels);

   /* when symbol table copied old spltsy field pointed to new */
   /* copying symbol table copies symbols */
   syp = (struct sy_t *) onp->nsym->spltsy;
   nnp->nsym = syp;
   syp->el.enp = nnp;
   /* mutual links now separated */
   /* must not change splt sym since still needed by param type regs */
  }
}

/*
 * copy params (storage same as nets and also by here will be in tab form)
 * for both module and task/func params 
 *
 * also links in nsym and symbol table el.enp
 * know decl. symbol tables for these
 *
 * value of param as parm_var net is assumed to be in pxp format
 * and expr. copied.
 */
static struct net_t *copy_params(struct net_t *onptab, int32 oprmnum,
 int32 pclass)
{
 register int32 ni;
 int32 nbytes, awid;
 struct sy_t *syp;
 struct net_t *nnp, *onp, *nnptab;

 if (oprmnum == 0) return(NULL);
 nbytes = oprmnum*sizeof(struct net_t);
 /* allocate and copy new parameter (really nets) as a block */ 
 nnptab = (struct net_t *) __my_malloc(nbytes);
 memcpy(nnptab, onptab, nbytes);
 
 /* need to also allocate and copy ncomp union field for each */
 for (ni = 0; ni < oprmnum; ni++)
  {
   nnp = &(nnptab[ni]);
   onp = &(onptab[ni]);
   /* DBG remove --- */
   if (pclass == MODULE)
    { if (!onp->n_isaparam) __misc_terr(__FILE__, __LINE__); }
   else if (pclass == SPECIFY)
    { if (!onp->n_isaparam) __misc_terr(__FILE__, __LINE__); }
   else __case_terr(__FILE__, __LINE__);
   /* --- */

   /* get area from special block */
   nnp->nu.ct = __alloc_arrncomp(); 

   *(nnp->nu.ct) = *(onp->nu.ct); 
   if (nnp->n_isavec)
    {
     nnp->nu.ct->nx1 = __copy_expr(onp->nu.ct->nx1);
     nnp->nu.ct->nx2 = __copy_expr(onp->nu.ct->nx2);
    }
   else nnp->nu.ct->nx1 = nnp->nu.ct->nx2 = NULL;

   /* know rhs will still be one expr. (SR_PNUM) here pointed to by nva */
   /* DBG remove --- */
   if (onp->srep != SR_PNUM) __misc_terr(__FILE__, __LINE__);
   /* --- */
   
   /* must also handle parameter arrays */
   if (onp->n_isarr)
    {
     nnp->nu.ct->ax1 = __copy_expr(onp->nu.ct->ax1);
     nnp->nu.ct->ax2 = __copy_expr(onp->nu.ct->ax2);
     awid = __get_arrwide(onp); 
     /* arrays of parameters never packed always take at least 8 bytes */
     nbytes = 2*WRDBYTES*awid*wlen_(onp->nwid);
    }
   else
    {
     nnp->nu.ct->ax1 = nnp->nu.ct->ax2 = NULL;
     nbytes = 2*WRDBYTES*wlen_(onp->nwid); 
    }
   /* alloc and copy value */
   nnp->nva.wp = (word32 *) __my_malloc(nbytes);
   memcpy(nnp->nva.wp, onp->nva.wp, nbytes);

   /* and copy source expr for params kept in n_dels_u field */
   /* DBG remove --- */
   if (onp->nu.ct->parm_srep != SR_PXPR) __misc_terr(__FILE__, __LINE__);
   /* --- */
   nnp->nu.ct->n_dels_u.d1x = __copy_expr(onp->nu.ct->n_dels_u.d1x);

   /* when symbol table copied old splt sym field pointed to new */
   /* copying symbol table copies symbols */
   syp = (struct sy_t *) onp->nsym->spltsy;
   nnp->nsym = syp;
   syp->el.enp = nnp;
   /* mutual links now separated */
   /* must not change splt sym since still needed by param type regs */
  }
 return(nnptab);
}

/*
 * copy downward relative only defparams - know checking completed
 */
static void copy_defparams(void)
{
 register struct dfparam_t *odfpp, *ndfpp;
 register int32 dfi;
 struct dfparam_t *ndfphdr, *ndfpend;

 ndfphdr = ndfpend = NULL;
 for (odfpp = __oinst_mod->mdfps; odfpp != NULL; odfpp = odfpp->dfpnxt) 
  {
   /* for rooted defparam in copied has no effect */
   /* if copying during splitting from defparams, down relative gone */ 
   if (odfpp->dfp_rooted) continue; 

   /* always copy local defparams */
   ndfpp = (struct dfparam_t *) __my_malloc(sizeof(struct dfparam_t));
   *ndfpp = *odfpp;
   ndfpp->dfpxlhs = __copy_expr(odfpp->dfpxlhs);
   ndfpp->dfpxrhs = __copy_expr(odfpp->dfpxrhs);
   ndfpp->in_mdp = __inst_mod;
   ndfpp->gdfpnam = __pv_stralloc(odfpp->gdfpnam);
   /* since resetting do not need this code --
   if (odfpp->dfptskp != NULL)
    ndfpp->dfptskp = odfpp->dfptskp->tsksyp->spltsy->el.etskp;
   --- */
   ndfpp->dfpnxt = NULL;

   /* must look up target symbols for defparam at end, no copy here */ 
   ndfpp->dfpiis = (int32 *) __my_malloc((ndfpp->last_dfpi + 1)*sizeof(int32));
   /* SJM 06/03/05 - index was wrongly not starting at 0 */
   for (dfi = 0; dfi < ndfpp->last_dfpi + 1; dfi++)
    ndfpp->dfpiis[dfi] = odfpp->dfpiis[dfi];

   if (ndfphdr == NULL) ndfphdr = ndfpp;
   else ndfpend->dfpnxt = ndfpp;  
   ndfpend = ndfpp;
  }
 __inst_mod->mdfps = ndfphdr;
}

/*
 * copy Verilog 2000 attributes
 */
static struct attr_t *copy_attrs(struct attr_t *oattrp)
{
 register struct attr_t *nattrp, *nattr_hd, *last_nattrp; 

 last_nattrp = NULL;
 nattr_hd = NULL;
 for (; oattrp != NULL; oattrp = oattrp->attrnxt)
  {
   nattrp = (struct attr_t *) __my_malloc(sizeof(struct attr_t));
   *nattrp = *oattrp;
   nattrp->attrnam = __pv_stralloc(oattrp->attrnam);
   if (oattrp->attr_xp != NULL)
    nattrp->attr_xp = __copy_expr(oattrp->attr_xp);
   nattrp->attrnxt = NULL;
   if (last_nattrp == NULL) nattr_hd = nattrp;
   else last_nattrp->attrnxt = nattrp;  
   last_nattrp = nattrp;
  }
 return(nattr_hd);
}

/*
 * routine for copy module variable initializ assign var init records
 *
 * notice this must be copied after copying wires and parameters 
 */
static struct varinitlst_t *copy_varinits(struct varinitlst_t *oinitp)
{
 register struct varinitlst_t *ninitp, *nvarinit_hd, *last_initp; 

 last_initp = NULL;
 nvarinit_hd = NULL;
 ninitp = NULL;
 for (; oinitp != NULL; oinitp = oinitp->varinitnxt)
  {
   ninitp = (struct varinitlst_t *) __my_malloc(sizeof(struct varinitlst_t));
   *ninitp = *oinitp;
   ninitp->init_syp = oinitp->init_syp->spltsy;
   ninitp->init_xp = __copy_expr(oinitp->init_xp); 
   ninitp->varinitnxt = NULL;
   if (last_initp == NULL) nvarinit_hd = ninitp;
   else last_initp->varinitnxt = ninitp;
   last_initp = ninitp;
  }
 return(ninitp);
}

/*
 * routine for copying module instances
 */
static void copy_insts(void)
{
 register int32 ii, ii2;
 int32 giawid;
 struct inst_t *oip, *nip;
 struct giarr_t *giap;

 /* if no instances - original copy set fields to nil and 0 */
 if (__oinst_mod->minum == 0) return;

 __inst_mod->minsts = (struct inst_t *)
  __my_malloc(__oinst_mod->minum*sizeof(struct inst_t));

 for (ii = 0; ii < __oinst_mod->minum;)
  {
   oip = &(__oinst_mod->minsts[ii]);
   nip = &(__inst_mod->minsts[ii]);
   /* notice for earliest copying miarr always nil */
   /* giap here is old since new does not yet exist */
   if (__oinst_mod->miarr != NULL && (giap = __oinst_mod->miarr[ii]) != NULL)
    {
     copy_1inst(nip, oip, TRUE);
     giawid = __get_giarr_wide(giap);
     for (ii2 = ii + 1; ii2 < ii + giawid; ii2++) 
      {
       oip = &(__oinst_mod->minsts[ii2]);
       nip = &(__inst_mod->minsts[ii2]);
       copy_1inst(nip, oip, FALSE);
       nip->ipins = giap->giapins;
      }
     ii = ii2;
    }
   else { copy_1inst(nip, oip, TRUE); ii++; }
  }
}

/*
 * copy 1 instance
 */
static void copy_1inst(struct inst_t *nip, struct inst_t *oip, int32 nd_cp_ipins)
{
 struct sy_t *syp;

 /* this copies instance must split for later if needed (>1 unsplit) */
 *nip = *oip;
 /* when symbol copied old splt sym field pointed to new */
 syp = (struct sy_t *) oip->isym->spltsy;
 if (syp == NULL) __misc_terr(__FILE__, __LINE__);
 nip->isym = syp;
 syp->el.eip = nip;
 /* mutual links now separated */
 /* notice module symbol (in modsyms) the same until splitting completed */
 nip->ipxprtab = copy_pndxtab(oip);

 if (oip->iattrs != NULL) nip->iattrs = copy_attrs(oip->iattrs);
 else nip->iattrs = NULL;

 if (nd_cp_ipins) copy_iports(nip, oip); else nip->ipins = NULL;
}

/*
 * copy pound param expression table
 *
 * if pound param expr. table exists it has loc (maybe nil) for every mod param
 */
static struct expr_t **copy_pndxtab(struct inst_t *oip)
{
 register int32 pi;
 struct expr_t **nnpxtab, **onpxtab;
 struct mod_t *imdp;

 if ((onpxtab = oip->ipxprtab) == NULL) return(NULL);

 imdp = oip->imsym->el.emdp;
 nnpxtab = (struct expr_t **)
  __my_malloc(imdp->mprmnum*sizeof(struct expr_t *));
 for (pi = 0; pi < imdp->mprmnum; pi++)
  {
   if (onpxtab[pi] == NULL) nnpxtab[pi] = NULL;
   else nnpxtab[pi] = __copy_expr(onpxtab[pi]);
  }
 return(nnpxtab);
}

/*
 * copy module instance ports
 */
static void copy_iports(struct inst_t *nip, struct inst_t *oip)
{
 register int32 pi, pnum;

 if ((pnum = oip->imsym->el.emdp->mpnum) != 0)
  {
   nip->ipins = (struct expr_t **) __my_malloc(pnum*sizeof(struct expr_t *));
   for (pi = 0; pi < pnum; pi++) nip->ipins[pi] = __copy_expr(oip->ipins[pi]);
  }
 else nip->ipins = NULL;
}

/*
 * copy module array of instances (miarr) 
 *
 * this is copy of index into individual insts in normal insts tab 
 */
static void copy_miarr(void)
{
 register int32 ii, ii2;
 int32 giawid;
 struct giarr_t *ogiap, *ngiap;
 struct sy_t *syp;

 /* if no arrays of instances - original copy set fields to nil and 0 */
 if (__oinst_mod->miarr == NULL) return;

 /* this is pointer to elements not array of elements */
 __inst_mod->miarr = (struct giarr_t **)
  __my_malloc(__oinst_mod->minum*sizeof(struct giarr_t *));

 for (ii = 0; ii < __oinst_mod->minum;)
  {
   ogiap = __oinst_mod->miarr[ii];
   if (ogiap == NULL) { __inst_mod->miarr[ii] = NULL; continue; }

   ngiap = (struct giarr_t *) __my_malloc(sizeof(struct giarr_t));
   *ngiap = *ogiap;
   /* new needs to point to right (its) module's ipins list */
   ngiap->giapins = __inst_mod->minsts[ii].ipins;

   syp = (struct sy_t *) ogiap->gia_base_syp->spltsy;
   if (syp == NULL) __misc_terr(__FILE__, __LINE__);
   ngiap->gia_base_syp = syp;
   /* points back to first (base) */
   /* LOOKATME - is this needed */
   syp->el.eip = &(__inst_mod->minsts[ii]);
   syp->sy_giabase = TRUE;

   /* only other fields that need copying is expression */
   ngiap->giax1 = __copy_expr(ogiap->giax1);
   ngiap->giax2 = __copy_expr(ogiap->giax2);
   __inst_mod->miarr[ii] = ngiap;
   /* LOOKATME - for new generate maybe need to change this */
   /* notice all giap's for expanded instances point to same master */
   giawid = __get_giarr_wide(ogiap);
   for (ii2 = ii + 1; ii2 < ii + giawid; ii2++) __inst_mod->miarr[ii2] = ngiap;
   ii = ii2;
  }
}

/*
 * routine for copying module gates
 */
static void copy_gates(void)
{
 register int32 gi, gi2;
 int32 giawid;
 struct gate_t *ogp, *ngp;
 struct giarr_t *giap;

 /* inst mod is new */
 if (__oinst_mod->mgnum != 0)
  __inst_mod->mgates = (struct gate_t *)
   __my_malloc(__oinst_mod->mgnum*sizeof(struct gate_t));

 for (gi = 0; gi < __oinst_mod->mgnum;)
  {
   ogp = &(__oinst_mod->mgates[gi]);
   ngp = &(__inst_mod->mgates[gi]);

   /* notice for earliest copying miarr always nil */
   /* giap here is old since new does not yet exist */
   if (__oinst_mod->mgarr != NULL && (giap = __oinst_mod->mgarr[gi]) != NULL)
    {
     copy_1gate(ngp, ogp, TRUE);
     giawid = __get_giarr_wide(giap);
     for (gi2 = gi + 1; gi2 < gi + giawid; gi2++) 
      {
       ogp = &(__oinst_mod->mgates[gi2]);
       ngp = &(__inst_mod->mgates[gi2]);
       copy_1gate(ngp, ogp, FALSE);
       ngp->gpins = giap->giapins;
      }
     gi = gi2;
    }
   else { copy_1gate(ngp, ogp, TRUE); gi++; }
  }
}

/*
 * copy 1 gate (passes already allocated address in gate tables)
 */
static void copy_1gate(struct gate_t *ngp, struct gate_t *ogp, int32 nd_cp_gpins)
{
 register int32 pi;
 int32 pnum;
 struct sy_t *syp;

 pnum = ogp->gpnum;
 /* copy everything but port connection expressions */
 *ngp = *ogp;

 /* when symbol copied old splt sym field pointed to new */
 syp = (struct sy_t *) ogp->gsym->spltsy;
 ngp->gsym = syp;
 syp->el.egp = ngp;
 /* mutual links now separated */
 /* notice module symbol (in modsyms) the same until splitting completed */

 /* copy output that is still expr.  - gets changed to wire later */
 ngp->g_du.pdels = __copy_dellst(ogp->g_du.pdels);

 if (ogp->gattrs != NULL) ngp->gattrs = copy_attrs(ogp->gattrs);
 else ngp->gattrs = NULL;

 ngp->gpins = (struct expr_t **) __my_malloc(pnum*sizeof(struct expr_t *));
 if (nd_cp_gpins)
  {
   /* copy the input port expressions */
   for (pi = 0; pi < pnum; pi++)
    ngp->gpins[pi] = __copy_expr(ogp->gpins[pi]);
  }
 else ngp->gpins = NULL;
}

/*
 * copy module array of gates (mgarr)
 */
static void copy_mgarr(void)
{
 register int32 gi, gi2;
 int32 giawid;
 struct giarr_t *ogiap, *ngiap;
 struct sy_t *syp;

 /* if no arrays of instances - original copy set fields to nil and 0 */
 if (__oinst_mod->mgarr == NULL) return;

 /* this is pointer to elements not array of elements */
 __inst_mod->mgarr = (struct giarr_t **)
  __my_malloc(__oinst_mod->mgnum*sizeof(struct giarr_t *));

 for (gi = 0; gi < __oinst_mod->mgnum; gi++)
  {
   ogiap = __oinst_mod->mgarr[gi];
   if (ogiap == NULL) { __inst_mod->mgarr[gi] = NULL; continue; }

   ngiap = (struct giarr_t *) __my_malloc(sizeof(struct giarr_t));
   *ngiap = *ogiap;

   /* new needs to point to right (its) module's ipins list */
   ngiap->giapins = __inst_mod->mgates[gi].gpins;

   syp = (struct sy_t *) ogiap->gia_base_syp->spltsy;
   if (syp == NULL) __misc_terr(__FILE__, __LINE__);
   ngiap->gia_base_syp = syp;
   /* points back to first (base) */
   /* LOOKATME - is this needed */
   syp->el.egp = &(__inst_mod->mgates[gi]);
   syp->sy_giabase = TRUE;

   /* only other fields that need copying is expression */
   ngiap->giax1 = __copy_expr(ogiap->giax1);
   ngiap->giax2 = __copy_expr(ogiap->giax2);
   __inst_mod->mgarr[gi] = ngiap;

   /* LOOKATME - for new generate maybe need to change this */
   /* notice all giap's for expanded instances point to same master */
   giawid = __get_giarr_wide(ogiap);
   for (gi2 = gi + 1; gi2 < gi + giawid; gi2++)
     __inst_mod->mgarr[gi2] = ngiap;
   /* AIV 06/08/06 - was skipping one because gi2 is + 1 */
   gi = gi2 - 1;
  }
}

/*
 * copy continuous assigns 
 */
static void copy_contas(void)
{
 register struct conta_t *ocap; 
 struct conta_t *ncap, *last_ncap;
 struct sy_t *syp;

 __inst_mod->mcas = NULL; 
 last_ncap = NULL;
 for (ocap = __oinst_mod->mcas; ocap != NULL; ocap = ocap->pbcau.canxt)
  {
   ncap = (struct conta_t *) __my_malloc(sizeof(struct conta_t));

   /* when symbol copied old splt sym field pointed to new */
   *ncap = *ocap;
   syp = (struct sy_t *) ocap->casym->spltsy;
   ncap->casym = syp;
   syp->el.ecap = ncap;

   ncap->ca_du.pdels = __copy_dellst(ocap->ca_du.pdels);
   ncap->lhsx = __copy_expr(ocap->lhsx);
   ncap->rhsx = __copy_expr(ocap->rhsx);
   if (last_ncap == NULL) __inst_mod->mcas = ncap;
   else last_ncap->pbcau.canxt = ncap;
   last_ncap = ncap;  
  }
 if (last_ncap != NULL) last_ncap->pbcau.canxt = NULL;
}

/*
 * copy all mod tasks
 */
static void copy_mdtasks(void)
{
 register struct task_t *otskp, *ntskp;
 struct task_t *last_ntskp;

 last_ntskp = NULL;
 __inst_mod->mtasks = NULL;
 for (otskp = __oinst_mod->mtasks; otskp != NULL; otskp = otskp->tsknxt)
  {
   ntskp = (struct task_t *) __my_malloc(sizeof(struct task_t));
   *ntskp = *otskp;
   /* fill link from task to symbol table and symbol */
   ntskp->tsksymtab = (struct symtab_t *) otskp->tsksymtab->n_head;
   /* notice must link both directions */
   ntskp->tsksyp = (struct sy_t *) otskp->tsksyp->spltsy;
   ntskp->tsksyp->el.etskp = ntskp;

   /* for tasks everything is a reg so must copy first */
   copy_wires(otskp->tsksymtab);

   /* for named blocks, connect from task to named block filled when named */
   /* block statement filled (st_namblkin field) */

   /* copy task pins */
   ntskp->tskpins = copy_tskargs(otskp);

   /* copy task params */
   ntskp->tsk_prms = copy_params(otskp->tsk_prms, otskp->tprmnum, MODULE);

   /* AIV 09/27/06 - need to copy the local params as well */
   ntskp->tsk_locprms = copy_params(otskp->tsk_locprms, otskp->tlocprmnum,
    MODULE); 

   /* copy the 1 statement */
   ntskp->tskst = copy_lstofsts(otskp->tskst);

   if (last_ntskp == NULL) __inst_mod->mtasks = ntskp;
   else last_ntskp->tsknxt = ntskp;
   last_ntskp = ntskp;
  }
}

/*
 * copy task ports - only regs can be ports
 */
static struct task_pin_t *copy_tskargs(struct task_t *otskp)
{
 register struct task_pin_t *otpp;
 struct task_pin_t *last_ntpp, *ntpp, *ntpp_hdr;

 last_ntpp = NULL;
 ntpp_hdr = NULL;
 for (otpp = otskp->tskpins; otpp != NULL; otpp = otpp->tpnxt)
  {
   ntpp = (struct task_pin_t *) __my_malloc(sizeof(struct task_pin_t));
   /* this copies trtyp field */
   *ntpp = *otpp;
   ntpp->tpsy = (struct sy_t *) otpp->tpsy->spltsy;
   ntpp->tpnxt = NULL;
   if (last_ntpp == NULL) ntpp_hdr = ntpp; else last_ntpp->tpnxt = ntpp;
   last_ntpp = ntpp;
  }
 return(ntpp_hdr);
}

/*
 * copy a statement entry
 */
static struct st_t *copy_stmt(struct st_t *ostp)
{
 struct st_t *nstp;
 struct sy_t *nsyp;

 if (ostp == NULL) return(NULL);

 nstp = (struct st_t *) __my_malloc(sizeof(struct st_t));
 *nstp = *ostp;
 nstp->stnxt = NULL;

 switch ((byte) ostp->stmttyp) {
  /* null just has type value and NULL pointer (i.e. ; by itself) */
  case S_NULL: case S_STNONE: break;
  case S_PROCA: case S_FORASSGN: case S_RHSDEPROCA: case S_NBPROCA:
   nstp->st.spra.lhsx = __copy_expr(ostp->st.spra.lhsx);
   nstp->st.spra.rhsx = __copy_expr(ostp->st.spra.rhsx);
   break;
  case S_IF:
   nstp->st.sif.condx = __copy_expr(ostp->st.sif.condx);
   nstp->st.sif.thenst = copy_lstofsts(ostp->st.sif.thenst);
   nstp->st.sif.elsest = copy_lstofsts(ostp->st.sif.elsest);
   break;
  case S_CASE:
   nstp->st.scs.castyp = ostp->st.scs.castyp;
   nstp->st.scs.maxselwid = ostp->st.scs.maxselwid;
   nstp->st.scs.csx = __copy_expr(ostp->st.scs.csx);

   /* this also copies default which is first (always present) */
   /* if case has no default, st field nil */ 
   nstp->st.scs.csitems = copy_csitemlst(ostp->st.scs.csitems);
   break;
  case S_REPEAT:
   nstp->st.srpt.repx = __copy_expr(ostp->st.srpt.repx);
   /* per inst. temporaries not allocated by here */
   nstp->st.srpt.reptemp = NULL;
   nstp->st.srpt.repst = copy_lstofsts(ostp->st.srpt.repst);
   break;
  case S_FOREVER:
  case S_WHILE:
   nstp->st.swh.lpx = __copy_expr(ostp->st.swh.lpx);
   nstp->st.swh.lpst = copy_lstofsts(ostp->st.swh.lpst);
   break;
  case S_WAIT:
   nstp->st.swait.lpx = __copy_expr(ostp->st.swait.lpx);
   nstp->st.swait.lpst = copy_lstofsts(ostp->st.swait.lpst);
   /* until prep, this dctp is just allocated unfilled version */
   nstp->st.swait.wait_dctp = __alloc_dctrl();
   *(nstp->st.swait.wait_dctp) = *(ostp->st.swait.wait_dctp);
   break;
  case S_FOR:
   {
    struct for_t *nfrp, *ofrp;

    nfrp = (struct for_t *) __my_malloc(sizeof(struct for_t));
    nstp->st.sfor = nfrp;
    ofrp = ostp->st.sfor;
    nfrp->forassgn = copy_stmt(ofrp->forassgn);
    nfrp->fortermx = __copy_expr(ofrp->fortermx);
    nfrp->forinc = copy_stmt(ofrp->forinc);
    nfrp->forbody = copy_lstofsts(ofrp->forbody);
   }
   break;
  case S_DELCTRL:
   {
    struct delctrl_t *ndcp, *odcp;

    ndcp = (struct delctrl_t *) __my_malloc(sizeof(struct delctrl_t));
    nstp->st.sdc = ndcp;
    odcp = ostp->st.sdc;
    ndcp->dctyp = odcp->dctyp;
    ndcp->dc_iact = odcp->dc_iact; 
    /* SJM 08/02/02 - need to also explicitly copy new non blking flag */ 
    ndcp->dc_nblking = odcp->dc_nblking;
    /* SJM 08/17/04 - f2016 was failing because this was not set */
    ndcp->implicit_evxlst = odcp->implicit_evxlst;
    ndcp->dc_delrep = odcp->dc_delrep;
    ndcp->dc_du.pdels = __copy_dellst(odcp->dc_du.pdels);
    ndcp->repcntx = __copy_expr(odcp->repcntx);

    /* SJM 10/07/06 - repcnts not yet set - set in v prp2 */
    /* DBG remove --- */
    if (odcp->dce_repcnts != NULL) __misc_terr(__FILE__, __LINE__);
    /* --- */

    ndcp->dce_repcnts = NULL;
    ndcp->dceschd_tevs = NULL;
    /* can be list because of #10 begin ... end */
    ndcp->actionst = copy_lstofsts(odcp->actionst);
   }
   break;
  case S_NAMBLK:
   {
    /* know task copied and old sy points to new - new el is new task */
    nsyp = (struct sy_t *) ostp->st.snbtsk->tsksyp->spltsy;
    nstp->st.snbtsk = nsyp->el.etskp;
    nsyp->el.etskp->st_namblkin = nstp;
   }
   break;
  case S_UNBLK:
   nstp->st.sbsts = copy_lstofsts(ostp->st.sbsts);
   break;
  case S_UNFJ:
   { 
    register int32 fji;
    int32 num_fji;
    struct st_t *ofjstp;

    /* first count number of statements in fj */
    for (num_fji = 0;; num_fji++)
     { if (ostp->st.fj.fjstps[num_fji] == NULL) break; }

    nstp->st.fj.fjstps = (struct st_t **)
     __my_malloc((num_fji + 1)*sizeof(struct st_t *));
    nstp->st.fj.fjlabs = (int32 *) __my_malloc((num_fji + 1)*sizeof(int32));

    /* know fork-join will always have at least 1 statement (maybe null) */
    /* also if labeled it will be surrounded by named block */
    for (fji = 0;; fji++)
     {
      if ((ofjstp = ostp->st.fj.fjstps[fji]) == NULL) break;
      nstp->st.fj.fjstps[fji] = copy_lstofsts(ofjstp);

      /* code gen label unused here but still copy */
      nstp->st.fj.fjlabs[fji] = ostp->st.fj.fjlabs[fji];
     }
    nstp->st.fj.fjstps[num_fji] = NULL;
    nstp->st.fj.fjlabs[num_fji] = -1;
   }
   break;
  case S_TSKCALL:
   /* find new task through old to new symbol */
   /* for system tasks since points to itself gets same (right) sym */
   nstp->st.stkc.tsksyx = __copy_expr(ostp->st.stkc.tsksyx);
   nstp->st.stkc.targs = __copy_expr(ostp->st.stkc.targs);

   /* field for pli tasks filled during prep */
   nstp->st.stkc.tkcaux.trec = NULL;
   break;
  case S_QCONTA:
   {
    /* SJM 06/23/02 - now aneed more room in qcont stmt specific area */
    struct qconta_t *nqcafs;

    nqcafs = (struct qconta_t *) __my_malloc(sizeof(struct qconta_t));

    nstp->st.sqca = nqcafs;
    nstp->st.sqca->qcatyp = ostp->st.sqca->qcatyp;
    nstp->st.sqca->regform = ostp->st.sqca->regform;
    nstp->st.sqca->qclhsx = __copy_expr(ostp->st.sqca->qclhsx);
    nstp->st.sqca->qcrhsx = __copy_expr(ostp->st.sqca->qcrhsx);
    /* lst of dce lists field nil until near end of prep */
    nstp->st.sqca->rhs_qcdlstlst = NULL;
   }
   break;
  case S_QCONTDEA:
   nstp->st.sqcdea.qcdatyp = ostp->st.sqcdea.qcdatyp;
   nstp->st.sqcdea.regform = ostp->st.sqcdea.regform;
   nstp->st.sqcdea.qcdalhs = __copy_expr(ostp->st.sqcdea.qcdalhs);
   break;
  case S_CAUSE:
   /* must copy expr. even though know just event name */
   nstp->st.scausx = __copy_expr(ostp->st.scausx);
   break;
  case S_DSABLE:
   nstp->st.sdsable.dsablx = __copy_expr(ostp->st.sdsable.dsablx);
   nstp->st.sdsable.func_nxtstp = NULL;
   break;
  /* name resolving statement type no. */
  default: __case_terr(__FILE__, __LINE__);
 }
 return(nstp);
}

/*
 * copy a statement list (linked list of statements)
 */
static struct st_t *copy_lstofsts(register struct st_t *ostp)
{
 struct st_t *nstp_hdr, *nstp, *last_nstp;

 nstp_hdr = NULL;
 for (last_nstp = NULL; ostp != NULL; ostp = ostp->stnxt)
  {
   nstp = copy_stmt(ostp);
   if (last_nstp == NULL) nstp_hdr = nstp; else last_nstp->stnxt = nstp;
   nstp->stnxt = NULL;
   last_nstp = nstp;
  }
 return(nstp_hdr);
}

/*
 * copy a list of ialst blocks
 */
static struct ialst_t *copy_ialst(register struct ialst_t *oialp)
{
 struct ialst_t *nialp, *last_nialp, *nialst_hdr;
 
 nialst_hdr = NULL;
 for (last_nialp = NULL; oialp != NULL; oialp = oialp->ialnxt)
  {
   nialp = (struct ialst_t *) __my_malloc(sizeof(struct ialst_t));
   *nialp = *oialp; 
   /* notice here initial/always exactly 1 statement, for always 2nd loop */
   /* back added during prep. */
   nialp->iastp = copy_lstofsts(oialp->iastp);
   nialp->ialnxt = NULL;

   if (last_nialp == NULL) nialst_hdr = nialp;
   else last_nialp->ialnxt = nialp;
   last_nialp = nialp;
  }
 return(nialst_hdr);
}

/*
 * copy the case item list
 * notice in common case of no default, pass nil - returns nil
 *
 * this copies default which is first too
 */
static struct csitem_t *copy_csitemlst( register struct csitem_t *ocsip)
{
 struct csitem_t *ncsip, *ncsip_hdr, *last_ncsip;

 ncsip_hdr = NULL;
 for (last_ncsip = NULL; ocsip != NULL; ocsip = ocsip->csinxt)
  {
   ncsip = __alloc_csitem();
   if (last_ncsip == NULL) ncsip_hdr = ncsip; else last_ncsip->csinxt = ncsip;
   ncsip->csixlst = copy_xprlst(ocsip->csixlst);
   if (ocsip->csist == NULL) ncsip->csist = NULL; 
   else ncsip->csist = copy_lstofsts(ocsip->csist);
   ncsip->csinxt = NULL;
   last_ncsip = ncsip;
  }
 return(ncsip_hdr);
}

/*
 * copy an expression list
 */
static struct exprlst_t *copy_xprlst(struct exprlst_t *oxplp)
{
 register struct exprlst_t *xplp;
 struct exprlst_t *nxplp_hdr, *nxplp, *last_nxplp;

 nxplp_hdr = NULL;
 for (last_nxplp = NULL, xplp = oxplp; xplp != NULL; xplp = xplp->xpnxt)
  {
   nxplp = __alloc_xprlst();
   if (last_nxplp == NULL) nxplp_hdr = nxplp; else last_nxplp->xpnxt = nxplp;
   nxplp->xp = __copy_expr(xplp->xp);
   last_nxplp = nxplp;
  }
 return(nxplp_hdr);
}


/*
 * copy an expression
 * this must deal with ID and symbol and globals (or need 2nd vers.)
 * update expression nodes to point to new modules symbols not old
 * if needed, caller must free src
 *
 */
extern struct expr_t *__copy_expr(struct expr_t *src)
{
 struct expr_t *dst;
 struct sy_t *syp;

 /* loops can have null expr. slots */
 if (src == NULL) return(NULL);

 dst = __alloc_newxnd();
 /* think long constant values can be shared but for now copying */
 /* copy of number node does node body including type copy */
 switch ((byte) src->optyp) {
  case NUMBER: case REALNUM:
   /* for non IS form constants can share (use same pointer) */
   *dst = *src;
   break;
  case ISNUMBER: case ISREALNUM:
   *dst = *src;
   /* 2 cases - if splitting - IS forms impossible else can't share con tab */
   if (__splitting)
    {
     __misc_terr(__FILE__, __LINE__);
    }
   break;
  case ID:
   /* must point new modules symbol - nothing points back */
   *dst = *src;
   /* if not splitting a module, copy has same symbol */
   if (__splitting)
    {
     syp = src->lu.sy;
     /* for system tasks/functions copied symbol is same as original */
     if (syp->sytyp != SYM_STSK && syp->sytyp != SYM_SF
      && syp->synam[0] != '$')
      dst->lu.sy = (struct sy_t *) syp->spltsy;
    }
   break;
  case XMRID:
   /* LOOKATME - copying pointers to make sure no shared storage */
   /* but since both read only maybe do not need to */
   *dst = *src;
   /* DBG remove ---*/
   if (src->ru.qnchp == NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */
   dst->ru.qnchp = __pv_stralloc(src->ru.qnchp);
   break;
  case GLBREF:
   /* need to copy only top level expr because global already copied */
   /* LOOKATME - if not splitting gref point to original not copied gxndp */
   /* since not really copying gref think this must work - i.e. not using */
   *dst = *src;
   /* know symbol points to old gref entry - this sets new copied gref */
   /* and expr. cross linking */
   if (__splitting)
    {
     struct gref_t *dgrp, *sgrp;

     /* trick is that src gref field points to dst gref */
     sgrp = src->ru.grp;
     /* SJM 06/03/05 - when copying checked defparam, grp gone */
     /* happens when defparam definition in mode split from pound param */
     if (sgrp == NULL) dst->ru.grp = NULL; 
     else
      {
       /* destionation global reference saved */
       dgrp = sgrp->spltgrp;
       /* dest. gref points to new gref entry */
       dst->ru.grp = dgrp;
       /* also finally link dest. group entry expr. ID node ptr to new node */
       dgrp->gxndp = dst;
       /* do not need to set lu.sy since later global name resolution sets */
       /* to target in some other part of design */
      }
    }
   break;
  default:
   /* know this is a operator node */
   /* must copy all contents - need optyp, rest mostly recomputed */
   *dst = *src;
   if (src->lu.x != NULL) dst->lu.x = __copy_expr(src->lu.x);
   if (src->ru.x != NULL) dst->ru.x = __copy_expr(src->ru.x);
 }
 return(dst);
}
 
/*
 * copy an expressions - special version for xform and vpi 
 * that does not count in size of expr table
 */
extern struct expr_t *__sim_copy_expr(struct expr_t *src)
{
 int32 sav_stnum, sav_exprnum;
 struct expr_t *dst;

 sav_stnum = 0;
 sav_exprnum = 0;
 if (__inst_mod != NULL)
  {
   sav_stnum = __inst_mod->mstnum;
   sav_exprnum = __inst_mod->mexprnum;
  }
   
 dst = __copy_expr(src);

 if (__inst_mod != NULL)
  {
   __inst_mod->mstnum = sav_stnum;
   __inst_mod->mexprnum = sav_exprnum;
  }
 return(dst);
}

/*
 * copy modules' global list
 *
 * must copy grefs before copying any expression so can link new expr
 * to new global
 */
static void copy_mgrefs(void)
{
 register int32 gri;
 register struct gref_t *ogrp, *ngrp;
 int32 num_grefs;

 if (__oinst_mod->mgrnum == 0) return;

 num_grefs = __oinst_mod->mgrnum;
 __inst_mod->mgrtab = (struct gref_t *)
  __my_malloc(num_grefs*sizeof(struct gref_t));

 ogrp = &(__oinst_mod->mgrtab[0]);
 ngrp = &(__inst_mod->mgrtab[0]);
 for (gri = 0; gri < __oinst_mod->mgrnum; gri++, ogrp++, ngrp++)
  {
   copy_1gref_flds(ngrp, ogrp);
  }
}

/*
 * copy 1 gref guts - operation depends on type (processing state) of gref
 * copies from 2nd ogrp to new ngrp 
 *
 * notice copying done before xmr resolution - so only need to copy
 * things that determine where appears 
 */
static void copy_1gref_flds(struct gref_t *ngrp, struct gref_t *ogrp)
{
 *ngrp = *ogrp; 
 /* this will be set when, expr. copied */
 ngrp->gxndp = NULL;
 ngrp->grsytp = (struct symtab_t *) ogrp->grsytp->n_head;
 ngrp->gin_mdp = __inst_mod; 
 /* need to copy global path because later folding inst/gate array selects */
 ngrp->glbref = __copy_expr(ogrp->glbref);

 /* this is needed so when expr. copied will point to gref in copied mod */
 ogrp->spltgrp = ngrp;
 /* no need to copy grcmps because all copying before resolution */
}

/*
 * copy specify section if old has specify
 */
static void copy_specify(void)
{
 struct spfy_t *ospfyp, *nspfyp;

 /* first allocate new module's specify section */
 ospfyp = __oinst_mod->mspfy;
 nspfyp = (struct spfy_t *) __my_malloc(sizeof(struct spfy_t));
 /* AIV 06/08/06 - need to do copy of old to new */
 *nspfyp = *ospfyp;
 __inst_mod->mspfy = nspfyp;
 /* if has symbol table for specparams copy */
 /* this will link old specparams to new */ 
 if (ospfyp->spfsyms != NULL) nspfyp->spfsyms = copy_1symtab(ospfyp->spfsyms);
 /* copy parameters */
 nspfyp->msprms = copy_params(ospfyp->msprms, ospfyp->sprmnum, SPECIFY);
 copy_spcpths(ospfyp, nspfyp);
 copy_timchks(ospfyp, nspfyp);
}

/*
 * copy list of specify paths (port to port delay paths)
 */
static void copy_spcpths(struct spfy_t *ospfyp, struct spfy_t *nspfyp)
{
 struct spcpth_t *npthp, *opthp, *last_npthp, *npthp_hdr;
 struct sy_t *syp;

 last_npthp = NULL;
 npthp_hdr = NULL;
 for (opthp = ospfyp->spcpths; opthp != NULL; opthp = opthp->spcpthnxt)
  {
   npthp = (struct spcpth_t *) __my_malloc(sizeof(struct spcpth_t));
   /* for path this copies most of fields (part of packed constants) */
   *npthp = *opthp;
   /* must link copied symbol */
   syp = (struct sy_t *) opthp->pthsym->spltsy;
   npthp->pthsym = syp;
   syp->el.epthp = npthp;

   /* copy the path input and and path output that is here a cast expr list */
   npthp->peins = (struct pathel_t *)
    copy_xprlst((struct exprlst_t *) opthp->peins);
   npthp->peouts = (struct pathel_t *)
    copy_xprlst((struct exprlst_t *) opthp->peouts);

   /* copy the optional conditional path fields */
   npthp->datasrcx = __copy_expr(opthp->datasrcx);
   npthp->pthcondx = __copy_expr(opthp->pthcondx);

   /* copy the delay list - always CMPLST and can be long (up to 12?) */
   npthp->pth_du.pdels = __copy_dellst(opthp->pth_du.pdels);

   /* finally, link on end */ 
   npthp->spcpthnxt = NULL;
   if (last_npthp == NULL) npthp_hdr = npthp;
   else last_npthp->spcpthnxt = npthp;
   last_npthp = npthp;
  }
 nspfyp->spcpths = npthp_hdr;
}

/*
 * copy timing checks 
 */
static void copy_timchks(struct spfy_t *ospfyp, struct spfy_t *nspfyp)
{
 struct tchk_t *ntcp, *otcp, *last_ntcp, *ntcp_hdr;

 last_ntcp = NULL;
 ntcp_hdr = NULL;
 for (otcp = ospfyp->tchks; otcp != NULL; otcp = otcp->tchknxt)
  {
   ntcp = copy1_tchk(otcp);
   /* finally, link on end */ 
   ntcp->tchknxt = NULL;
   if (last_ntcp == NULL) ntcp_hdr = ntcp; else last_ntcp->tchknxt = ntcp;
   last_ntcp = ntcp;
  }
 nspfyp->tchks = ntcp_hdr;
}

/*
 * copy 1 timing check 
 * notice here hold half of setuphold not yet copied
 */
static struct tchk_t *copy1_tchk(struct tchk_t *otcp)
{
 struct tchk_t *ntcp;
 struct sy_t *syp;

 ntcp = (struct tchk_t *) __my_malloc(sizeof(struct tchk_t));
 /* this copies flags */
 *ntcp = *otcp;
 syp = (struct sy_t *) otcp->tcsym->spltsy;
 ntcp->tcsym = syp;
 syp->el.etcp = ntcp;

 if (otcp->startxp != NULL) ntcp->startxp = __copy_expr(otcp->startxp);
 if (otcp->startcondx != NULL)
  ntcp->startcondx = __copy_expr(otcp->startcondx);
 if (otcp->chkxp != NULL) ntcp->chkxp = __copy_expr(otcp->chkxp);
 if (otcp->chkcondx != NULL) ntcp->chkcondx = __copy_expr(otcp->chkcondx);

 /* know 1st delay limit always required - always CMPLST here */
 ntcp->tclim_du.pdels = __copy_dellst(otcp->tclim_du.pdels);
 if (otcp->tc_haslim2) 
  ntcp->tclim2_du.pdels = __copy_dellst(otcp->tclim2_du.pdels);
 else ntcp->tclim2_du.pdels = NULL;

 /* at this point, if present, ntfy_np is a sy_t */ 
 if (otcp->ntfy_np == NULL) ntcp->ntfy_np = NULL;
 else
  {
   syp = (struct sy_t *) otcp->ntfy_np;
   /* old symbols always have splt sym link to new */
   syp = syp->spltsy;
   ntcp->ntfy_np = (struct net_t *) syp;  
  }
 return(ntcp);
}

/*
 * ROUTINES TO BUILD AS IF FLATTENED INSTANCE TREE
 */

/*
 * build a linked tree as if flattened instance structure
 * needed in simulation and for tracing xmrs
 * try to set fields at as high a place in tree as possible
 */
extern void __bld_flat_itree(void)
{
 register int32 ii;

 for (ii = 0; ii < __numtopm; ii++) bld2_flat_itree(__it_roots[ii]);
 /* DBG remove ---
 if (__debug_flg)
  { for (ii = 0; ii < __numtopm; ii++) __dmp_itree(__it_roots[ii]); }
 --- */
}

/*
 * non top level built itree for inst. of one module
 *
 * know up instance pointers point to allocated but not set itree nodes
 * for each inst. in module one up
 * try to make as breadth first as possible
 */
static void bld2_flat_itree(struct itree_t *new_itp)
{
 register int32 ii;
 struct inst_t *ip;
 struct itree_t *itp;
 struct mod_t *up_imdp, *imdp;

 up_imdp = new_itp->itip->imsym->el.emdp;
 /* if mod has no insts empty minsts ptr tab and empty up_itp ptr table */
 if (up_imdp->minum == 0) return;

 /* allocate pointer to contained itree instances */
 new_itp->in_its = (struct itree_t *)
  __my_malloc(up_imdp->minum*sizeof(struct itree_t));

 /* fill contained itree instance contents */ 
 for (ii = 0; ii < up_imdp->minum; ii++)
  {
   /* alloc sets inst_t value */
   itp = &(new_itp->in_its[ii]);
   __init_itree_node(itp);
   ip = &(up_imdp->minsts[ii]);
   imdp = ip->imsym->el.emdp;
   itp->itip = ip;
   itp->up_it = new_itp;
   /* assign instance number */
   itp->itinum = imdp->lastinum;
   imdp->lastinum += 1;
   /* DBG remove ---
   if (__debug_flg)
    {
     __dbg_msg("==> building flat itree for instance %s of type %s\n",
      ip->isym->synam, ip->imsym->synam);
    }
   --- */
  }
 /* finally down 1 level */
 for (ii = 0; ii < up_imdp->minum; ii++)
  bld2_flat_itree(&(new_itp->in_its[ii]));
}

/*
 * dump a instance tree
 */
extern void __dmp_itree(struct itree_t *itp) 
{
 register int32 i;

 __dbg_msg("--- dumping instance tree ---.\n");
 /* dump top */
 do_dmp(itp, 0);
 for (i = 1;; i++) { if (!dmp_down_itree(itp, i, i)) return; }
}

/*
 * dump a down level of a tree
 */
static int32 dmp_down_itree(struct itree_t *itp, int32 lev, int32 more_down)
{
 register int32 i; 
 int32 ofsnum, retval;

 if (more_down == 0) { do_dmp(itp, lev); return(TRUE); }
 if ((ofsnum = itp->itip->imsym->el.emdp->minum) == 0) return(FALSE);
 for (retval= FALSE, i = 0; i < ofsnum; i++)
  {
   if (dmp_down_itree(&(itp->in_its[i]), lev, more_down - 1))
    retval = TRUE;
  }
 return(retval);
}

/*
 * do actual dump for level of 1 itp at level lev
 */
static void do_dmp(struct itree_t *itp, int32 lev)
{
 int32 ofsnum;
 char *chp, s1[RECLEN];

 /* DBG remove --- */
 if (itp->itip == 0) __misc_terr(__FILE__, __LINE__);
 /* --- */
 if (itp->up_it == NULL) { strcpy(s1, "(top module)"); chp = s1; }
 else chp = itp->up_it->itip->isym->synam;
 ofsnum = itp->itip->imsym->el.emdp->minum;
 __dbg_msg("-- level %d: inst. %s(%s) itinum %d, up %s %d offspring\n",
  __dagmaxdist - lev, itp->itip->isym->synam, itp->itip->imsym->synam,
  itp->itinum, chp, ofsnum); 
}

/*
 * free under root (top level) itree
 */
extern void __free_flat_itree(void)
{
 register int32 ii;
 register struct mod_t *mdp;

 for (ii = 0; ii < __numtopm; ii++) free2_flat_itree(__it_roots[ii]);

 /* final step requires resetting last inum field in every module */ 
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt) mdp->lastinum = 0;
}

/*
 * free itree in_its tables bottom up
 */
static void free2_flat_itree(struct itree_t *cur_itp)
{
 register int32 ii;
 struct mod_t *cur_imdp;

 cur_imdp = cur_itp->itip->imsym->el.emdp;

 /* if mod has no insts, nothing to do, if not top up frees */ 
 if (cur_imdp->minum == 0) return;

 /* otherwise free all under first */
 for (ii = 0; ii < cur_imdp->minum; ii++)
  free2_flat_itree(&(cur_itp->in_its[ii]));

 /* all under freed but in its table not yet freed */
 __my_free ((char *) cur_itp->in_its, cur_imdp->minum*sizeof(struct itree_t)); 
 cur_itp->in_its = NULL;
}

/*
 * ROUTINES TO BUILD INSTANCE NUMBER TO ITREE TABLES
 */

/*
 * build mod itp tabs for all modules
 *
 * look to improve this - either build only when needed or remove unneeded
 * before run time
 */
static void bld_moditps(void)
{
 register struct mod_t *mdp;
 register int32 ii;
 struct itree_t *itp, *itp2;

 /* first allocate the inst num to itree location tables */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   mdp->moditps = (struct itree_t **)
   __my_malloc(mdp->flatinum*sizeof(struct itree_t *));
   memset(mdp->moditps, 0, mdp->flatinum*sizeof(struct itree_t *));
  }

 for (ii = 0; ii < __numtopm; ii++)
  {
   itp = __it_roots[ii];
   mdp = itp->itip->imsym->el.emdp;
   mdp->moditps[itp->itinum] = itp;
   bld2_itnum_to_itp(itp);
  }


 if (__debug_flg)
  {
   for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
    {
     __dbg_msg("\n--- module %s instances:\n", mdp->msym->synam);
     for (ii = 0; ii < mdp->flatinum; ii++)
      {
       itp2 = mdp->moditps[ii];
       __dbg_msg("  instance %s type %s\n", __msg2_blditree(__xs, itp2),
        itp2->itip->imsym->synam);
      }
    }
   __dbg_msg("\n");
  }
}

/*
 * recusively build the table
 */
static void bld2_itnum_to_itp(struct itree_t *itp)
{
 register int32 ii;
 struct mod_t *mdp;
 
 mdp = itp->itip->imsym->el.emdp;
 if (mdp->moditps != NULL) mdp->moditps[itp->itinum] = itp;
 for (ii = 0; ii < mdp->minum; ii++) bld2_itnum_to_itp(&(itp->in_its[ii]));
}

