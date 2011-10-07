/* Copyright (c) 1995-2007 Pragmatic C Software Corp. */

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
 * first module to implement pli vpi_ routines
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <dlfcn.h>

#ifdef __DBMALLOC__
#include "../malloc.h"
#endif

#include "v.h"
#include "cvmacros.h"

#include "vpi_user.h"
#include "cv_vpi_user.h"

/* local prototypes */
static int32 lbnam_so_suffix(char *);
static int32 check_systf(p_vpi_systf_data, int32 *);
static int32 chk_idnam_systfs(char *);
static void exec_vpisysfunc_compiletf(struct vpisystf_t *);
static void exec_vpisystask_compiletf(struct vpisystf_t *);
static struct sy_t *task_add_vpi_systf(char *, int32);
static struct sy_t *func_add_vpi_systf(char *, int32);
static vpiHandle valchg_cb_register(p_cb_data);
static int32 chk_valchg_cb(p_cb_data, struct h_t *);
static vpiHandle bld_cbrec(p_cb_data, word32);
static void bld_cbvc_dces(struct expr_t *, struct cbrec_t *);
static void linkon_cb_dce(struct net_t *, int32, int32, struct gref_t *,
 struct cbrec_t *, int32);
static vpiHandle gateout_valchg_register(struct h_t *, struct t_cb_data *);
static void set_dce_strenchg_on(struct dceauxlst_t *);
static vpiHandle rf_cb_register(p_cb_data, int32);
static int32 chk_rf_cb(p_cb_data, struct h_t *, char *);
static vpiHandle rf_all_register_cb(p_cb_data, int32);
static vpiHandle delay_cb_register(register p_cb_data);
static int32 chk_delay_cb(p_cb_data, struct h_t *);
static void exec_vpi_delaycbs(int32);
static void free_cbrec(struct cbrec_t *);
static vpiHandle action_cb_register(register p_cb_data);
static void linkout_allcb(struct cbrec_t *, int32);
static void linkout_gateout_cb(struct cbrec_t *); 
static void linkout_action_cb(struct cbrec_t *);
static char *to_cbtypnam(char *, int32);
static vpiHandle get_cursystfcall(struct h_t *);
static vpiHandle get_inmod_itp(struct h_t *);
static vpiHandle get_obj_index(struct h_t *);
static vpiHandle get_obj_range(struct h_t *, int32);
static vpiHandle get_obj_parent(struct h_t *);
static void exprobj_to_itreeloc(struct itree_t **, struct task_t **, 
 struct expr_t *, struct itree_t *, struct task_t *);
static vpiHandle get_obj_side(struct h_t *, int32);
static vpiHandle bld_scope_par(struct h_t *, struct task_t *);
static vpiHandle getbit_lowconn(struct h_t *);
static vpiHandle getexpr_lowconn(struct h_t *);
static vpiHandle getbit_highconn(struct h_t *);
static struct expr_t *find_catxp_frombit(struct expr_t *, int32, int32 *);
static vpiHandle getexpr_highconn(struct h_t *);
static void no1_1to1h_err(word32, word32, struct h_t *);
static void no_1to1h_err(int32, struct h_t *);
static vpiHandle get_tchk_term(word32, struct h_t *);
static vpiHandle get_cond(struct h_t *);
static vpiHandle bld_1to1_exprclass_handle(struct h_t *);
static vpiHandle mk_pthterm_exprclass_handle(struct net_t *, int32, int32,
 struct itree_t *); 
static vpiHandle get_obj_scope(struct h_t *);
static vpiHandle get_disable_scope(struct h_t *);
static vpiHandle get_contained_stmt(struct h_t *);
static vpiHandle get_dctrl_stmt(struct h_t *, int32);
static vpiHandle get_udpdef_from_inobj(struct h_t *);
static vpiHandle get_contained_udp_init(struct h_t *);
static vpiHandle get_up_poundparam_expr(struct h_t *);
static vpiHandle bld_itree_iterator(struct h_t *);
static vpiHandle bld_type_iterator(struct h_t *);
static vpiHandle bld_inst_iterator(struct h_t *);
static vpiHandle bld_udpdef_iterator(struct h_t *);
static void mustbe_inmoditer_err(word32, struct h_t *);
static vpiHandle bld_scope_iterator(register struct h_t *);
static vpiHandle bld_symtabs_iterator(struct symtab_t *, struct itree_t *);
static void fill_scopehandle(struct hrec_t *, struct symtab_t *);
static vpiHandle bld_net_iterator(struct h_t *, word32);
static vpiHandle bld_listofnets_iter(struct net_t *, int32, struct itree_t *,
 word32, struct task_t *);
static int32 cnt_typnetnum(register struct net_t *, int32, word32);


static vpiHandle bld_initalw_iterator(struct h_t *);
static vpiHandle bld_conta_iterator(struct h_t *);
static vpiHandle bld_gate_iterator(struct h_t *);
static vpiHandle bld_modpth_iterator(struct h_t *);
static vpiHandle bld_tchk_iterator(struct h_t *);
static vpiHandle bld_param_iterator(struct h_t *, int32);
static vpiHandle bld_listofparams_iter(struct net_t *, int32, struct net_t *,
 int32, struct itree_t *, struct task_t *);
static vpiHandle bld_specparam_iterator(struct h_t *);
static vpiHandle bld_defparam_stmt_iterator(struct h_t *);

/* NEED? static void shrink_systfdata_tab(void); */
static void exec_vpi_actioncbs(int32);
static void exec_vpi_tchkerr(int32, struct tchk_t *, struct itree_t *);

/* extern prototypes (maybe defined in this module) */
extern void __call_vlog_startup_procs(void);
extern void __chkbld_vpi_systf_func(struct expr_t *);
extern void __chkbld_vpi_systf_task(struct st_t *);
extern void __vpi_sysf_calltf(struct expr_t *);
extern void __vpi_syst_calltf(struct st_t *);
extern char *__cb_reason_to_nam(char *, int32);
extern int32 __expr_is_vpiconst(struct expr_t *);
extern void __find_call_force_cbs(struct net_t *, int32);
extern void __find_call_rel_cbs(struct net_t *, int32);
extern void __cb_all_rfs(struct net_t *, int32, int32);
extern void __cbvc_callback(struct dcevnt_t *, struct cbrec_t *, struct h_t *);
extern void __delay_callback(i_tev_ndx);
extern void __vpi_startreset_trycall(void);
extern void __vpi_endreset_trycall(void);
extern void __vpi_endcomp_trycall(void);
extern void __vpi_startsim_trycall(void);
extern void __vpi_endsim_trycall(void);
extern void __vpi_enteriact_trycall(void);
extern void __vpi_exitiact_trycall(void);
extern void __vpi_iactscopechg_trycall(void);
extern void __vpi_del_nxtsimtim_trycall(void);
extern void __vpi_del_rosync_call(void);
extern void __vpi_tchkerr_trycall(struct tchk_t *, struct itree_t *);
extern void __vpi_error_trycall(void);
extern struct task_t *__find_qualnam_task(char *, struct mod_t *,
 struct task_t *);
extern vpiHandle __mk_exprclass_handle(struct expr_t *, struct itree_t *,
 struct task_t *);
extern vpiHandle __mk_handle(word32, void *, struct itree_t *,
 struct task_t *);
extern vpiHandle __mk_stmt_handle(word32, struct st_t *, struct itree_t *,
 struct task_t *);
extern word32 __to_vpi_tasktyp(word32);
extern word32 __gate_to_vpiprimtyp(struct gate_t *);
extern void __init_hrec(struct hrec_t *);
extern struct thread_t *__alloc_thrd(void);
extern vpiHandle __nil_iter_err(word32);
extern struct pviter_t *__alloc_iter(int32, vpiHandle *);
extern word32 __to_vpi_stmttyp(struct st_t **);
extern vpiHandle __bld_port_iterator(struct h_t *);
extern vpiHandle __bld_neticonn_iter(struct h_t *);
extern vpiHandle __bld_paramassign_stmt_iter(struct h_t *);
extern vpiHandle __bld_udpline_iter(struct h_t *);
extern vpiHandle __bld_primterm_iterator(struct h_t *);
extern vpiHandle __bld_loc_lds_iterator(struct h_t *, int32);
extern vpiHandle __bld_lds_iterator(struct h_t *, int32);
extern vpiHandle __bld_loc_drvs_iterator(struct h_t *, int32);
extern vpiHandle __bld_drvs_iterator(struct h_t *, int32);
extern vpiHandle __bld_arrwrd_iterator(struct h_t *);
extern vpiHandle __bld_bitof_iterator(struct h_t *);
extern vpiHandle __bld_systf_iterator(struct h_t *);
extern vpiHandle __bld_tfargexpr_iterator(struct h_t *);
extern vpiHandle __bld_pthterm_iterator(struct h_t *, word32);
extern vpiHandle __bld_stmt_iterator(struct h_t *);
extern vpiHandle __bld_netin_tchkterms(struct h_t *);
extern vpiHandle __bld_netin_pthterms(struct h_t *);
extern vpiHandle __bld_caseitems_iter(struct h_t *);
extern vpiHandle __bld_casi_exprs_iter(struct h_t *);
extern vpiHandle __bld_operands_iter(struct h_t *);
extern vpiHandle __bld_allcbs_iter(struct h_t *);
extern vpiHandle __bld_delay_expr_iter(struct h_t *);
extern vpiHandle __bld_dig_attr_iter(struct h_t *);
extern vpiHandle __bld_iodecl_stmt_iter(struct h_t *);
extern vpiHandle __get_digattr_parent(struct h_t *);
extern struct itree_t *__find_dfpbot_itp(struct dfparam_t *);

extern char *__my_realloc(char *, int32, int32);
extern struct systsk_t *__alloc_systsk(void);
extern struct tnode_t *__vtfind(char *, struct symtab_t *);
extern void __add_sym(char *, struct tnode_t *);
extern struct sysfunc_t *__alloc_sysfunc(void);
extern int32 __validate_handle(char *, struct h_t *);
extern int32 __validate_nonit_handle(char *, struct h_t *); 
extern void __chg_xstk_width(struct xstk_t *, int32);
extern void __grow_xstk(void);
extern int32 __wide_vval_is0(register word32 *, int32);
extern void __grow_tevtab(void);
extern void __sim_notbegun_err(char *);
extern int32 __validate_time_type(char *, int32);
extern int32 __validate_value_fmt(char *, int32);
extern char *__to_vpiopnam(char *, int32);
extern void __xmrpush_refgrp_to_targ(struct gref_t *);
extern struct dcevnt_t *__alloc_dcevnt(struct net_t *);
extern void __alloc_1intdce_prevval(struct dcevnt_t *);
extern void __init_1instdce_prevval(struct dcevnt_t *);
extern struct task_t *__getcur_scope_tsk(void);
extern void __set_vpi_time(struct t_vpi_time *, word64 *, int32, struct mod_t *);
extern struct xstk_t *__eval2_xpr(register struct expr_t *);
extern int32 __vpitime_to_ticks(word64 *, p_vpi_time, struct mod_t *);
extern void __bad_rosync_err(char *);
extern void __insert_event(register i_tev_ndx);
extern void __my_free(char *, int32);
extern char *__my_malloc(int32);
extern char *__pv_stralloc(char *);
extern void __free_dceauxlst(struct dceauxlst_t *, int32);
extern void __free_xtree(struct expr_t *);
extern void __free_hp(struct h_t *); 
extern void __still_comp_err(char *);
extern int32 __validate_accessm(char *, int32, char *);
extern int32 __unnormalize_ndx(struct net_t *, int32);
extern struct expr_t *__bld_rng_numxpr(word32, word32, int32);
extern int32 __exprtype_get(struct expr_t *);
extern char *__to_vpionam(char *, word32);
extern void __getwir_range(struct net_t *, int32 *, int32 *);
extern void __getarr_range(struct net_t *, int32 *, int32 *, int32 *);
extern char *__to_mpnam(char *, char *);
extern int32 __expr_optype_get(struct expr_t *);
extern struct sy_t *__get_sym(char *, struct symtab_t *);
extern int32 __is_scope_sym(struct sy_t *);
extern struct expr_t *__sim_alloc_newxnd(void);
extern int32 __comp_ndx(register struct net_t *, register struct expr_t *);
extern word32 __from_vpi_vartyp(word32);
extern word32 __ntyp_to_vpivarhtyp(struct net_t *);
extern word32 __to_vpinetbithtyp(word32);
extern struct expr_t *__glbnam_to_expr(char *);
extern struct expr_t *__sim_copy_expr(struct expr_t *);
extern char *__to_timstr(char *, word64 *);
extern void __logic_acc_off(struct gate_t *);
extern char *__msg2_blditree(char *, struct itree_t *);
extern void __dce_turn_chg_store_on(struct mod_t *, struct dcevnt_t *, int32);  
extern void __dcelst_off(struct dceauxlst_t *);
extern void __alloc_1instdce_prevval(struct dcevnt_t *);

extern void __cv_msg(char *, ...);
extern void __tr_msg(char *, ...);
extern void __pv_err(int32, char *, ...);
extern void __pv_warn(int32, char *, ...);
extern void __pv_vpi_terr(int32, char *, ...);
extern void __inform(int32, char *, ...);

extern void __vpi_err(int32, int32, char *, ...);
extern void __vpi_terr(char *, int32);

extern word32 __masktab[];

/* vpi only storage */
extern struct t_vpi_error_info *__last_eip;/* if err, ptr to wrk eifo or nil */

/*
 * ROUTINES TO REGISTER VPI STYLE SYS TASKS AND FUNCTIONS
 */


#ifdef __APPLE__
#define SO_SUFFIX ".dylib"
#else
#define SO_SUFFIX ".so"
#endif

/*
 * process either load pli1 or load vpi dynamic load option lists
 *
 * same dynamic loading for both since bootstrap routine not called here
 */
extern void __process_pli_dynamic_libs(struct loadpli_t *ldp_hd)
{
 register struct loadpli_t *ldp;
 register struct dynboot_t *dnbp;
 int32 slen;
 void *handle;
 void *boot_rout;
 char s1[RECLEN], onam[RECLEN];
    
 for (ldp = ldp_hd; ldp != NULL; ldp = ldp->load_plinxt)
  {
   /* no dynamic library - assume boot routines in other dyn libs */
   if (ldp->pli1_option) strcpy(onam, "+loadpli1=");
   else strcpy(onam, "+loadvpi=");

   /* dynamic lib must be defined or previous error */
   /* if name does not have .so suffix - try that first */
   if (!lbnam_so_suffix(ldp->libnam))
    {
     strcpy(s1, ldp->libnam);
     strcat(s1, SO_SUFFIX);
     if ((handle = dlopen(s1, RTLD_LAZY | RTLD_GLOBAL)) == NULL)
      {
       if ((handle = dlopen(ldp->libnam, RTLD_LAZY | RTLD_GLOBAL)) == NULL)
        {
         __pv_err(1803,
          "unable to load %s dynamic library (also tried adding %s suffix): %s",
          ldp->libnam, SO_SUFFIX, dlerror());
         continue;
        }
      }
     else
      {
       slen = strlen(ldp->libnam) + 1;
       __my_free(ldp->libnam, slen);
       ldp->libnam = __pv_stralloc(s1);
      }
    }
   else
    {
     /* SJM 11/25/02 - must be now not lazy */
     if ((handle = dlopen(ldp->libnam, RTLD_LAZY | RTLD_GLOBAL)) == NULL)
      {
       __pv_err(1803, "unable to load %s dynamic library: %s",
         onam, dlerror());
       continue;
      }
    }

   /* ok to have no boot routines - if so this does nothing */
   /* because dynblst empty */
   for (dnbp = ldp->dynblst; dnbp != NULL; dnbp = dnbp->dynbootnxt)
    {
#if defined(__APPLE_CC__) && (__APPLE_CC__ < 1495)
     /* for Mach (Mac OSX) need to prepend a '_' - old BSD convention */
     /* LOOKATME - what happens if name already has '_'? */
     strcpy(s1, "_");
     strcat(s1, dnbp->bootrout_nam);
#else
     strcpy(s1, dnbp->bootrout_nam);
#endif
     if ((boot_rout = dlsym(handle, s1)) == NULL)
      {
       __pv_err(1803, "unable to find %s bootstrap routine in %s: %s",
        s1, ldp->libnam, dlerror());
       continue;
      } 
     if (ldp->pli1_option) dnbp->dynu.tf_rout = boot_rout;
     else dnbp->dynu.vpi_rout = boot_rout;
    }
  }
}

/*
 * return T if name has .so suffix (may differ on some systems)
 */
static int32 lbnam_so_suffix(char *lbnam)
{
 int32 suflen;
 char *cp;

 if ((cp = rindex(lbnam, '.')) == NULL) return(FALSE);
 suflen = strlen(SO_SUFFIX);
 if (strncmp(cp, SO_SUFFIX, suflen) == 0 && strlen(cp) == suflen)
  return(TRUE);
 return(FALSE);
}

/*
 * if vpi used, this is called 
 */
extern void __call_vlog_startup_procs(void)
{
 register struct loadpli_t *ldp;
 register struct dynboot_t *dnbp;

 /* SJM 07/08/02 - unless user needs static PLI, this now always empty */
 __vpi_vlog_start_done = FALSE;
#ifdef __STATIC_PLI__
 {
  register int32 i; 

  for (i = 0;; i++) 
   {
    if (vlog_startup_routines[i] == NULL) break; 
    vlog_startup_routines[i]();
   }
 } 
#else
 /* SJM 07/08/02 - call each dynamic vpi bootstrap routine */ 
 /* although normally static cverobj.o not shipped, run after P1364 vpi_ */ 
 /* standardized vlog_startup_routines table */
 for (ldp = __vpi_dynlib_hd; ldp != NULL; ldp = ldp->load_plinxt)
 {
  /* vpi boostrap routines are assume to return void - if not ret ignored */
  for (dnbp = ldp->dynblst; dnbp != NULL;  dnbp = dnbp->dynbootnxt)
   {
    if (dnbp->dynu.vpi_rout != NULL) (dnbp->dynu.vpi_rout)();         
   }
 }
#endif
 __vpi_vlog_start_done = TRUE;
 /* shrink_systfdata_tab(); */
}

/*
 * register a vpi_ systf style user system task or function
 * this must be done from vlog start routines only or error
 *
 * emitting both normal source error and vpi error - needed for two reasons 
 * 1) experts need vpi error when handler registed, 2) novices will not
 * be able to get error call back or error check code written when initial
 * registering happens
 */
extern vpiHandle vpi_register_systf(p_vpi_systf_data systf_data_p)
{
 int32 stf_ind, systf_typ;
 p_vpi_systf_data in_systfdatp;
 struct systftab_t *stftabp;
 struct systsk_t *stbp;
 struct sysfunc_t *sfbp;
 struct sy_t *syp;
 struct h_t *hp;
 struct hrec_t *hrp;

 __last_eip = NULL;
 if (__vpi_vlog_start_done)
  {
#ifdef __STATIC_PLI__
   __pv_err(1802,
    "vpi_register_systf only callable from vlog_startup_routines at start of elaboration");  

   __vpi_err(1802, vpiError,
    "vpi_register_systf only callable from vlog_startup_routines at start of elaboration");  
#else
   __pv_err(1802,
    "vpi_register_systf from +loadvpi= option only callable at start of elaboration");

   __vpi_err(1802, vpiError,
    "vpi_register_systf from +loadvpi= option only callable at start of elaboration");  
#endif
   return(NULL);
  }
 if (!check_systf(systf_data_p, &systf_typ)) return(NULL);

 /* allocate initial registered vpi_ systf table if first time */
 /* numbering starts just after tf_ table */
 if (__last_systf == __last_veriusertf) 
  {
   __systftab = (struct systftab_t *)
    __my_malloc(SYSTFDATSIZE*sizeof(struct systftab_t));
   __size_systftab = SYSTFDATSIZE;
  }

 __last_systf++;
 /* grow the vpi_ systf table if at end */
 if ((__last_systf - __last_veriusertf) >= __size_systftab)
  {  
   int32 osize;

   osize = __size_systftab*sizeof(struct systftab_t);
   __size_systftab = 3*(__size_systftab/2);
   __systftab = (struct systftab_t *) __my_realloc((char *) __systftab, osize,
    __size_systftab*sizeof(struct systftab_t));
  } 
 stf_ind = __last_systf - (__last_veriusertf + 1);
 stftabp = &(__systftab[stf_ind]);
 /* must copy so caller can free passed data - never need to free since */
 /* no vpi remove systf */
 in_systfdatp = (p_vpi_systf_data) __my_malloc(sizeof(struct t_vpi_systf_data));

 *(in_systfdatp) = *(systf_data_p);
 /* only field that needs to be copied (reallocated is name) */
 in_systfdatp->tfname = __pv_stralloc(systf_data_p->tfname); 
 stftabp->vpi_sytfdat = in_systfdatp;
 
 if (systf_data_p->type == vpiSysTask)
  {
   syp = task_add_vpi_systf(systf_data_p->tfname, __last_systf);
   stbp = syp->el.esytbp; 
   stftabp->sfu.stbp = stbp;
  }
 else
  {
   syp = func_add_vpi_systf(systf_data_p->tfname, __last_systf);
   sfbp = syp->el.esyftbp;
   stftabp->sfu.sfbp = sfbp;
   sfbp->tftyp = systf_typ;  
  }
 stftabp->systf_chk = FALSE;
 /* systf handles do not go on list since cannot be freed */
 hp = (struct h_t *) __my_malloc(sizeof(struct h_t)); 
 hrp = (struct hrec_t *) __my_malloc(sizeof(struct hrec_t));
 __init_hrec(hrp);
 hrp->htyp = vpiUserSystf;
 /* handle is 1000 + value that is always correct for indexing stftab table */
 hrp->hi = __last_systf;
 hp->hrec = hrp;
 hp->hin_itp = NULL;
 return((vpiHandle) hp);
}

/*
 * check a user passed systf structure
 * LOOKATME - also maybe warning for real func that has size_t?
 *
 * here error message is not vpi_ error since impossible to se
 *
 */
static int32 check_systf(p_vpi_systf_data systfp, int32 *stftyp)
{
 *stftyp = SYSF_VPI;
 if (!chk_idnam_systfs(systfp->tfname)) return(FALSE);

 if (systfp->type == vpiSysTask)
  {
   if (systfp->sizetf != NULL)
    {
     __pv_warn(2001, 
      "vpi_register_systf of task %s systf data record has sizetf function - ignored",
      systfp->tfname);

     __vpi_err(2001, vpiWarning,
      "vpi_register_systf of task %s systf data record has sizetf function - ignored",
      systfp->tfname);
     systfp->sizetf = NULL;
    }
  }
 else if (systfp->type == vpiSysFunc)
  {
   switch (systfp->sysfunctype) {
    case vpiIntFunc: case vpiRealFunc: case vpiTimeFunc: case vpiSizedFunc:
     break;
    default:
     __vpi_err(1806, vpiError,
      "vpi_register_systf function %s sysfunctype %d illegal",
      systfp->tfname, systfp->sysfunctype);
     return(FALSE);
    }
   /* sizetf required for Func Size */
   if (systfp->sysfunctype == vpiSizedFunc)
    {
     if (systfp->sizetf == NULL)
      {
       __pv_warn(2028,
        "vpi_register_systf of function %s systf data record sizetf function missing - using integer size",
        systfp->tfname);

      __vpi_err(2028, vpiWarning,
       "vpi_register_systf of function %s systf data record sizetf function missing - using integer size",
       systfp->tfname);
      }
    }
   else
    {
     /* but ignored for any other */
     if (systfp->sizetf != NULL)
      {
       __pv_warn(2002,
        "vpi_register_systf of function %s non vpiSizedFunc has sizetf - ignored", 
        systfp->tfname);

       __vpi_err(2002, vpiWarning,
        "vpi_register_systf of function %s non vpiSizedFunc has sizetf - ignored", 
        systfp->tfname);
       systfp->sizetf = NULL;
      }
    }
  }
 else 
  {
   __pv_err(1811,
    "vpi_register_systf task/function %s type %d illegal",
    systfp->tfname, systfp->type);
   return(FALSE);
  }
 return(TRUE); 
}

/*
 * check for legal vpi systf task or function name
 * version supporting vpi style errors 
 */
static int32 chk_idnam_systfs(char *tfnam)
{
 register char *chp;
 int32 len;

 chp = tfnam;
 if (chp == NULL || *chp == '\0' || *chp != '$') 
  {
   __pv_err(1812,  
    "vpi_register_systf data record task or function name %s must begin with '$'",
    tfnam); 

   __vpi_err(1812, vpiError,
    "vpi_register_systf data record task or function name %s must begin with '$'",
    tfnam); 
   return(FALSE);
  }
 chp++;
 for (len = 1; *chp != '\0'; chp++)
  {
   /* notice no escaped names here */
   if (!isalnum(*chp) && *chp != '_' && *chp != '$')  
    {
     __pv_err(1813,
      "vpi_register_systf data record task or function name %s contains illegal char %c",
      tfnam, *chp);   

     __vpi_err(1813, vpiError,
      "vpi_register_systf data record task or function name %s contains illegal char %c",
      tfnam, *chp);   
     return(FALSE);
    }
   if (++len >= IDLEN)   
    {
     /* notice cannot truncate since in user memory */
     __pv_err(1814,
      "vpi_register_systf data record task or function name %s too long (%d)",
      tfnam, IDLEN); 

     __vpi_err(1814, vpiError,
      "vpi_register_systf data record task or function name %s too long (%d)",
      tfnam, IDLEN); 
     return(FALSE);
    }
  }
 return(TRUE);
}


/*
 * add a vpi_ systf pli system task - handles vpi style errors 
 */
static struct sy_t *task_add_vpi_systf(char *tnam, int32 tnum)
{
 struct systsk_t *stbp;
 struct tnode_t *tnp;
 struct sy_t *syp;

 /* allocate a systsk_t entry - needed because splicing pli on and */
 /* need to be able to compile version without */
 stbp = __alloc_systsk();
 /* notice cross linking here is right */
 stbp->stsknam = tnam; 
 stbp->stsknum = tnum;
 tnp = __vtfind(tnam, __syssyms);
 if (!__sym_is_new)
  {
   syp = tnp->ndp;

   __inform(2103,
    "PLI vpi systf task %s replaces predefined or veriusertf system task or function with same name",
    syp->synam);

   __vpi_err(2103, vpiNotice,
    "PLI vpi systf task %s replaces predefined or veriusertf system task or function with same name",
    syp->synam);
  }
 else
  {
   __add_sym(tnam, tnp);
   (__syssyms->numsyms)++;
   syp = tnp->ndp;
  }
 syp->sytyp = SYM_STSK;
 syp->sydecl = TRUE;
 syp->el.esytbp = stbp;
 return(syp);
}

/*
 * add a vpi systf task symbol table for registering new systf task
 */
static struct sy_t *func_add_vpi_systf(char *fnam, int32 fnum)
{
 struct tnode_t *tnp;
 struct sy_t *syp;
 struct sysfunc_t *sfbp;

 sfbp = __alloc_sysfunc();
 /* notice cross linking right here */
 sfbp->syfnam = fnam;
 sfbp->syfnum = fnum;
 /* caller will set this if different */
 sfbp->tftyp = SYSF_VPI;
 tnp = __vtfind(fnam, __syssyms);
 if (!__sym_is_new)
  {
   syp = tnp->ndp;
   __inform(2104,
    "PLI vpi systf task %s replaces predefined or veriusertf system task or function with same name",
    syp->synam);

   __vpi_err(2104, vpiNotice,
    "PLI vpi systf task %s replaces predefined or veriusertf system task or function with same name",
    syp->synam);
  }
 else
  {
   __add_sym(fnam, tnp);
   (__syssyms->numsyms)++;
   syp = tnp->ndp;
  }
 syp->sytyp = SYM_SF;
 syp->sydecl = TRUE;
 syp->el.esyftbp = sfbp;
 return(syp);
}

/*
 * get vpi registered systf info - either call or registered handle allowed
 * notice internal name string is used - user must not free or write into
 */
extern void vpi_get_systf_info(vpiHandle object,
 p_vpi_systf_data systf_data_p)
{
 int32 stf_ind;
 struct h_t *hp;
 struct hrec_t *hrp;
 p_vpi_systf_data tfdatp;
 struct systftab_t *stftabp;
 
 __last_eip = NULL;
 hp = (struct h_t *) object;
 if (!__validate_handle("vpi_get_systf_info", hp)) return;
 hrp = hp->hrec;
 if (hrp->htyp != vpiUserSystf && hrp->htyp != vpiSysFuncCall
  && hrp->htyp != vpiSysTaskCall)
  {
   __vpi_err(1821, vpiError,
    "vpi_get_systf_info requires vpiUserSystf or vpiSys[Func|Task]Call handle");
   return;
  }
 stf_ind = hrp->hi - (__last_veriusertf + 1);
 stftabp = &(__systftab[stf_ind]);
 tfdatp = (p_vpi_systf_data) stftabp->vpi_sytfdat;
 *(systf_data_p) = *(tfdatp);
}

/*
 * final step shrink table to right size and adjust last values 
 */
/* --- LOOKATME - think can not use this
static void shrink_systfdata_tab(void)
{
 int32 num_systfs, osize, nsize;

 -* maybe none *-
 if (__last_systf == -1) return;

 num_systfs = __last_systf - __last_veriusertf;
 if (num_systfs > 0 && __verbose)
  {
   __cv_msg(
    "  %d vpi_register_systf user system tasks and functions registered.\n",
    num_systfs);
  }
 if (num_systfs < __size_systftab)
  {
   osize = __size_systftab*sizeof(struct systftab_t);
   nsize = num_systfs*sizeof(struct systftab_t);
   __systftab = (struct systftab_t *) __my_realloc((char *) __systftab, osize,
    nsize);
  }
}
--- */

/*
 * ROUTINES TO EXECUTE VPI COMPILETF AND CHECKTF IF FIRST TIME SEEN
 */

/*
 * check a systfs user function and call sizetf and compiletf if present
 * only called first time function seen (i.e. once per source location)
 */
extern void __chkbld_vpi_systf_func(struct expr_t *fcallx)
{
 int32 stf_ind;
 struct sy_t *syp;
 struct sysfunc_t *syfp;
 struct systftab_t *stftabp;
 p_vpi_systf_data stfdp;
 struct vpisystf_t *vstfp;
 int32 (*sizetf_func)();

 syp = fcallx->lu.x->lu.sy;
 syfp = syp->el.esyftbp;
 stf_ind = syfp->syfnum - (__last_veriusertf + 1); 
 stftabp = &(__systftab[stf_ind]);
 stfdp = (p_vpi_systf_data) stftabp->vpi_sytfdat;

 /* SJM 04/14/04 - following PLI 1.0, can use the sysf lu.x ptr to sy */
 /* field for the per call in source user data settable/readable fld */  
 fcallx->lu.x->szu.vpi_sysf_datap = NULL; 

 /* if has vpi registered systf compiletf, must put on list so can call */
 /* just before end of compile (when vpi_ d.s. built) point */  
 if (stfdp->compiletf != NULL)
  {
   vstfp = (struct vpisystf_t *) __my_malloc(sizeof(struct vpisystf_t));
   vstfp->is_sysfunc = TRUE;
   vstfp->vstffnam_ind = __sfnam_ind;
   vstfp->vstflin_cnt = __slin_cnt;
   vstfp->curmdp = __inst_mod;
   vstfp->curtskp = __cur_tsk;
   vstfp->vsystfu.sysfcallx = fcallx;

   /* link onto front */
   vstfp->vpistfnxt = __vpi_sysf_hdr;
   __vpi_sysf_hdr = vstfp;
  }

 /* do not need to set expr. node width - if called before*/
 if (stftabp->systf_chk) return;

 /* must run sizetf with no environment since still compiling */
 __cur_sysf_expr = NULL;
 switch (stfdp->sysfunctype) {
  case vpiIntFunc:
   syfp->retntyp = N_INT;
   syfp->retsigned = TRUE;
   syfp->retwid = WBITS;
   fcallx->has_sign = TRUE;
   break;
  case vpiRealFunc:
   syfp->retntyp = N_REAL;
   syfp->retsigned = TRUE;
   /* special width or real since no x/z */
   syfp->retwid = WBITS;  
   fcallx->is_real = TRUE;  
   fcallx->has_sign = TRUE;
   break;
  case vpiTimeFunc:
   syfp->retntyp = N_TIME;
   syfp->retsigned = FALSE;
   syfp->retwid = TIMEBITS;
   break;
  case vpiSizedFunc:
   syfp->retntyp = N_REG;
   syfp->retsigned = FALSE;

   /* if sizetf not set warning was emited and use 32 (WBITS) */
   if ((sizetf_func = stfdp->sizetf) == NULL) syfp->retwid = WBITS;
   else syfp->retwid = (*sizetf_func)(stfdp->user_data);

   break;
  default: __vpi_terr(__FILE__, __LINE__);
 }
 fcallx->szu.xclen = syfp->retwid;
 stftabp->systf_chk = TRUE;
}

/*
 * check a systfs user task and link on to compiletf list if needed
 * only called first time task seen at each source loc. (not per inst)
 *
 * never see sizetf here
 */
extern void __chkbld_vpi_systf_task(struct st_t *stp)
{
 int32 stf_ind;
 struct systsk_t *stbp;
 struct systftab_t *stftabp;
 p_vpi_systf_data stfdp;
 struct vpisystf_t *vstfp;
 struct tskcall_t *tkcp;

 tkcp = &(stp->st.stkc);
 stbp = tkcp->tsksyx->lu.sy->el.esytbp;

 stf_ind = stbp->stsknum - (__last_veriusertf + 1); 
 stftabp = &(__systftab[stf_ind]);
 stfdp = (p_vpi_systf_data) stftabp->vpi_sytfdat;

 /* SJM 04/14/04 - following PLI 1.0, can use the syst task_t tkcaux ptr */
 /* field for the per call in source user data settable/readable fld */  
 tkcp->tkcaux.vpi_syst_datap = NULL;

 /* if has vpi registered systf compiletf, must pu on list so can call */
 /* just before end of compile (when vpi_ d.s. built) point */  
 if (stfdp->compiletf != NULL)
  {
   vstfp = (struct vpisystf_t *) __my_malloc(sizeof(struct vpisystf_t));
   vstfp->is_sysfunc = FALSE;
   vstfp->vstffnam_ind = __sfnam_ind;
   vstfp->vstflin_cnt = __slin_cnt;
   vstfp->curmdp = __inst_mod;
   vstfp->curtskp = __cur_tsk;
   vstfp->vsystfu.syststp = stp;
   /* link onto front of sys task list */
   vstfp->vpistfnxt = __vpi_syst_hdr;
   __vpi_syst_hdr = vstfp;
  }
 stftabp->systf_chk = TRUE;
}

/*
 * ROUTINES TO EXECUTE VPI SYSTF COMPILETF 
 */

/*
 * execute every vpi registered systf compiletf routine
 *
 * notice because of xform for code gen - these can't be used after here
 * LOOKATME - memory leak because could be freed and only accessible through
 * header and list
 */
extern void __exec_all_compiletf_routines(void)
{
 register struct vpisystf_t *vstfp;

 for (vstfp = __vpi_sysf_hdr; vstfp != NULL; vstfp = vstfp->vpistfnxt)
  exec_vpisysfunc_compiletf(vstfp);

 for (vstfp = __vpi_syst_hdr; vstfp != NULL; vstfp = vstfp->vpistfnxt)
  exec_vpisystask_compiletf(vstfp);
}

/*
 * execute the vpi systf system function compiletf routine
 *
 * like calltf but does not return anything and must build own context since
 * nothing executing at this time so need thd and itree (first) env. 
 * also need task on stack if inside task or function
 *
 * LOOKATME is there ever a problem calling with first inst?
 */
static void exec_vpisysfunc_compiletf(struct vpisystf_t *vstfp)
{
 int32 stf_ind;
 struct sysfunc_t *sfbp;
 struct systftab_t *stftabp;
 p_vpi_systf_data stfdp;
 struct thread_t *thp, *sav_cur_thd;
 int32 (*vpicomptf_func)();
      
 sfbp = vstfp->vsystfu.sysfcallx->lu.x->lu.sy->el.esyftbp;
 stf_ind = sfbp->syfnum - (__last_veriusertf + 1); 
 stftabp = &(__systftab[stf_ind]);
 stfdp = (p_vpi_systf_data) stftabp->vpi_sytfdat;
 __cur_sysf_expr = vstfp->vsystfu.sysfcallx;

 vpicomptf_func = stfdp->compiletf;
 __vpifnam_ind = vstfp->vstffnam_ind;
 __vpilin_cnt = vstfp->vstflin_cnt;
 __push_itstk(vstfp->curmdp->moditps[0]);

 /* need dummy thread - still if user tried to use tf_stop think will crash */
 thp = __alloc_thrd();
 thp->thenbl_sfnam_ind = 0;
 thp->thenbl_slin_cnt = 0;
 thp->thnxtstp = NULL; 
 thp->thpar = NULL;
 /* think should never have a current thread here */
 sav_cur_thd = __cur_thd;
 __cur_thd = thp;
 
 if (vstfp->curtskp != NULL)
  {
   /* DBG remove --- */  
   if (__fcspi != -1) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   __fcstk[++__fcspi] = vstfp->curtskp;
  }

 /* notice global __sf location values are rigth for call loc. */
 (*vpicomptf_func)(stfdp->user_data);

 if (vstfp->curtskp != NULL) __fcspi--;
 __cur_thd = sav_cur_thd;
 __my_free((char *) thp, sizeof(struct thread_t)); 
 __pop_itstk();
 __cur_sysf_expr = NULL;
 __vpifnam_ind = 0;
 __vpilin_cnt = 0;
} 

/*
 * execute the vpi systf system task compiletf routine
 *
 * like calltf but does not return anything and must build own context since
 * nothing executing at this time so need thd and itree (first) env. 
 * also need task on stack if inside task or function
 *
 * LOOKATME is there ever a problem calling with first inst?
 */
static void exec_vpisystask_compiletf(struct vpisystf_t *vstfp)
{
 int32 stf_ind;
 struct systsk_t *stbp;
 struct systftab_t *stftabp;
 p_vpi_systf_data stfdp;
 struct thread_t *thp, *sav_cur_thd;
 struct st_t *stp;
 int32 (*vpicomptf_tsk)();

 stp = vstfp->vsystfu.syststp;
 stbp = stp->st.stkc.tsksyx->lu.sy->el.esytbp;
 stf_ind = stbp->stsknum - (__last_veriusertf + 1); 
 stftabp = &(__systftab[stf_ind]);
 stfdp = (p_vpi_systf_data) stftabp->vpi_sytfdat;
 __cur_syst_stp = stp;

 vpicomptf_tsk = stfdp->compiletf;
 __vpifnam_ind = __sfnam_ind; 
 __vpilin_cnt = __slin_cnt;

 __push_itstk(vstfp->curmdp->moditps[0]);

 /* need dummy thread - still if user tried to use tf_stop think will crash */
 thp = __alloc_thrd();
 thp->thenbl_sfnam_ind = 0;
 thp->thenbl_slin_cnt = 0;
 thp->thnxtstp = NULL; 
 thp->thpar = NULL;
 /* think should never have a current thread here */
 sav_cur_thd = __cur_thd;
 __cur_thd = thp;
 
 if (vstfp->curtskp != NULL)
  {
   /* DBG remove --- */  
   if (__fcspi != -1) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   __fcstk[++__fcspi] = vstfp->curtskp;
  }

 /* set systf for get handle and nil handle user must call vpi put value */
 (*vpicomptf_tsk)(stfdp->user_data);

 if (vstfp->curtskp != NULL) __fcspi--;
 __cur_thd = sav_cur_thd;
 __my_free((char *) thp, sizeof(struct thread_t)); 
 __pop_itstk();
 __cur_syst_stp = NULL;
 __vpifnam_ind = 0;
 __vpilin_cnt = 0;
} 

/*
 * ROUTINES TO EXECUTE VPI SYSTF CALLTF 
 */

/*
 * execute the vpi systf system function calltf routine
 * this pushes return value on top of expr. stack but does not return it
 * uses the instance tree loc. called from
 *
 * notice required replacement of built-in names happens because
 * symbol table symbol gets replaced
 */
extern void __vpi_sysf_calltf(struct expr_t *fcallx)
{
 int32 stf_ind;
 struct sysfunc_t *sfbp;
 struct xstk_t *xsp;
 struct systftab_t *stftabp;
 p_vpi_systf_data stfdp;
 int32 (*vpicalltf_func)();
      
 sfbp = fcallx->lu.x->lu.sy->el.esyftbp;
 stf_ind = sfbp->syfnum - (__last_veriusertf + 1); 
 stftabp = &(__systftab[stf_ind]);
 stfdp = (p_vpi_systf_data) stftabp->vpi_sytfdat;
 __cur_sysf_expr = fcallx;

 /* set systf for get handle and nil handle user must call vpi_put_value */
 push_xstk_(xsp, stftabp->sfu.sfbp->retwid);
 one_allbits_(xsp->ap, xsp->xslen);
 one_allbits_(xsp->bp, xsp->xslen);
 __cur_sysf_xsp = xsp;

 /* LOOKATME - maybe need warning if nil */
 if ((vpicalltf_func = stfdp->calltf) == NULL) { return; }
 __vpifnam_ind = __sfnam_ind; 
 __vpilin_cnt = __slin_cnt;

 /* notice global __sf location values are rigth for call loc. */
 (*vpicalltf_func)(stfdp->user_data);

 __cur_sysf_xsp = NULL;
 __cur_sysf_expr = NULL;
 __vpifnam_ind = 0;
 __vpilin_cnt = 0;
} 

/*
 * call the pli tf system task calltf routine
 */
extern void __vpi_syst_calltf(struct st_t *stp)
{
 int32 stf_ind;
 struct systsk_t *stbp;
 struct systftab_t *stftabp;
 p_vpi_systf_data stfdp;
 int32 (*vpicalltf_tsk)();

 stbp = stp->st.stkc.tsksyx->lu.sy->el.esytbp;
 stf_ind = stbp->stsknum - (__last_veriusertf + 1); 
 stftabp = &(__systftab[stf_ind]);
 stfdp = (p_vpi_systf_data) stftabp->vpi_sytfdat;
 __cur_syst_stp = stp;

 /* LOOKATME - maybe need warning if nil */
 if ((vpicalltf_tsk = stfdp->calltf) == NULL) return;

 __vpifnam_ind = __sfnam_ind; 
 __vpilin_cnt = __slin_cnt;
 /* set systf for get handle and nil handle user must call vpi put value */
 (*vpicalltf_tsk)(stfdp->user_data);

 __cur_syst_stp = NULL;
 __vpifnam_ind = 0;
 __vpilin_cnt = 0;
}

/*
 * ROUTINES TO IMPLEMENT VPI CALLBACKS
 */

/*
 * register a callback
 *
 * this can add non delay or val change call backs before sim begins
 */
extern vpiHandle vpi_register_cb(p_cb_data cb_data_p)
{
 __last_eip = NULL;

 if (cb_data_p->cb_rtn == NULL) 
  {
   __vpi_err(1810, vpiError,
    "vpi_register_cb no effect - no call back routine (cb_rtn field NULL)");
   return(NULL);
  }

 /* needs built net list */ 
 /* cannot check handle until call back type */
 switch (cb_data_p->reason) {
  /* value change cbs */
  case cbValueChange:
   if (__run_state != SS_SIM)
    {
no_sim:
      __sim_notbegun_err("vpi_register_cb");
      return(NULL);
    } 
   return(valchg_cb_register(cb_data_p));
  case cbForce: case cbRelease:
   if (__run_state != SS_SIM) goto no_sim;
   return(rf_cb_register(cb_data_p, cb_data_p->reason));
  /* delayed wake up cbs */
  case cbReadWriteSynch: case cbReadOnlySynch:
  case cbNextSimTime: case cbAfterDelay:
   if (__run_state != SS_SIM) goto no_sim;
   /* schedule a delay callback - reason determines how scheduled */
   return(delay_cb_register(cb_data_p));
  /* fixed routine call cbs - can be register before start of sim */ 
  case cbEndOfCompile: case cbStartOfSimulation: case cbEndOfSimulation:
  case cbError: case cbPLIError:
  case cbTchkViolation: case cbStartOfReset:
  case cbEndOfReset: case cbEnterInteractive: case cbExitInteractive:
  case cbInteractiveScopeChange:
  case cbLanguageLine: 
    return(action_cb_register(cb_data_p));  
  case cbAtStartOfSimTime: return(delay_cb_register(cb_data_p));

  /* currently unimplemented */
  case cbStartOfSave: case cbEndOfSave: case cbStartOfRestart:
  case cbEndOfRestart: case cbUnresolvedSystf:
   __vpi_err(1823, vpiError, "vpi_register_cb reason %s unsupported",
    __cb_reason_to_nam(__wrks1, cb_data_p->reason));
   return(NULL);
  case cbStmt:
   /* after move time, execute event from special list */ 
   __vpi_err(1823, vpiError,
    "vpi_register_cb reason %s unsupported - not yet implemented",
    __cb_reason_to_nam(__wrks1, cb_data_p->reason));
   return(NULL);
  default:
   __vpi_err(1824, vpiError,
    "vpi_register_cb illegal reason value %d - no callback registered",
     cb_data_p->reason); 
 }
 return(NULL);
}

/*
 * convert a callback reason to a name
 */
extern char *__cb_reason_to_nam(char *s, int32 reason)
{
 switch (reason) {
  case cbValueChange: strcpy(s, "cbValueChange"); break;
  case cbStmt: strcpy(s, "cbStmt"); break;
  case cbForce: strcpy(s, "cbForce"); break;
  case cbRelease: strcpy(s, "cbRelease"); break;
  case cbAtStartOfSimTime: strcpy(s, "cbAtStartOfSimTime"); break;
  case cbReadWriteSynch: strcpy(s, "cbReadWriteSynch"); break;
  case cbReadOnlySynch: strcpy(s, "cbReadOnlySynch"); break;
  case cbNextSimTime: strcpy(s, "cbNextSimTime"); break;
  case cbAfterDelay: strcpy(s, "cbAfterDelay"); break;
  case cbEndOfCompile: strcpy(s, "cbEndOfCompile"); break;
  case cbStartOfSimulation: strcpy(s, "cbStartOfSimulation"); break;
  case cbEndOfSimulation: strcpy(s, "cbEndOfSimulation"); break;
  case cbError: strcpy(s, "cbError"); break;
  case cbPLIError: strcpy(s, "cbPLIError"); break;
  case cbTchkViolation: strcpy(s, "cbTchkViolation"); break;
  case cbStartOfReset: strcpy(s, "cbStartOfReset"); break;
  case cbEndOfReset: strcpy(s, "cbEndOfReset"); break;
  case cbEnterInteractive: strcpy(s, "cbEnterInteractive"); break;
  case cbExitInteractive: strcpy(s, "cbExitInteractive"); break;
  case cbInteractiveScopeChange:
   strcpy(s, "cbInteractiveScopeChange");
   break;
  case cbUnresolvedSystf: strcpy(s, "cbUnresolvedSystf"); break;
  case cbStartOfSave: strcpy(s, "cbStartOfSave"); break;
  case cbEndOfSave: strcpy(s, "cbEndOfSave"); break;
  case cbStartOfRestart: strcpy(s, "cbStartOfRestart"); break;
  case cbEndOfRestart: strcpy(s, "cbEndOfRestart"); break;
  case cbLanguageLine: strcpy(s, "cbLanguageLine"); break;
  default:
   /* checking must be elsewhere */
   strcpy(s, "**illegal**");
 }
 return(s);
}

/*
 * setup (register) the value change callback 
 *
 * LOOKATME - think call backs for events not possible
 *
 * interpreting LRM to only allow value change call backs on members of
 * expression class (can come from to <expr. class> or contents of mod/task)
 *
 * LRM says term but interpreting term as "expr. of term" where must then go
 * through loads to find needed term - alternative could actually pass term
 * such as prim (gate), tchk, path, and more? but not for now
 */
static vpiHandle valchg_cb_register(p_cb_data cb_data_p) 
{
 int32 biti;
 vpiHandle cbref, ihref;
 struct h_t *hp, *hp2;
 struct hrec_t *hrp;
 struct net_t *np;
 struct expr_t *xp;
 struct gate_t *gp;
 
 hp = (struct h_t *) cb_data_p->obj;
 if (!chk_valchg_cb(cb_data_p, hp)) return(NULL);
 hrp = hp->hrec;
 __cbvc_dcehdr = NULL;
 __push_itstk(hp->hin_itp);
 switch (hrp->htyp) {
  case vpiNet: case vpiReg: case vpiIntegerVar: case vpiTimeVar:
   np = hrp->hu.hnp;
   cbref = bld_cbrec(cb_data_p, CB_VALCHG);
   hp2 = (struct h_t *) cbref;
   linkon_cb_dce(np, -1, -1, NULL, hp2->hrec->hu.hcbp, DCE_CBVC);
   break;
  case vpiNamedEvent:
   /* FIXME - think need to support change call back on events */
   np = hrp->hu.hnp;
   /* DBG remove -- */
   if (np->ntyp != N_EVENT) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   /* can not place change call back on event since no value? */
   __vpi_err(2024, vpiWarning,
    "vpi_register_cb value change form for vpiNamedEvent %s object unsupported",
    np->nsym->synam);
   goto bad_end;
  case vpiNetBit: case vpiRegBit: case vpiVarSelect: case vpiMemoryWord:
   /* know parent object is a vector or will not get here */
   if (!hrp->bith_ndx) { xp = hrp->hu.hxp; goto bit_expr_form; }

   np = hrp->hu.hnp;
   cbref = bld_cbrec(cb_data_p, CB_VALCHG);
   hp2 = (struct h_t *) cbref;
   /* always need to set this so callback cb_data field has change index */
   hp2->hrec->hu.hcbp->cb_ndxobj = TRUE;

   /* for vectored vectors or array - no individual bit/array cell identity */
   /* LOOKATME - can callbacks be placed on individual cells of arrays? */
   if (!np->vec_scalared || hrp->htyp == vpiMemoryWord) biti = -1;
   else biti = hrp->hi;

   /* index stored as normalized h:0 */ 
   /* must filter bit or array index */
   linkon_cb_dce(np, biti, biti, NULL, hp2->hrec->hu.hcbp, DCE_CBVC);
   break;
  case vpiRealVar: case vpiConstant: 
   /* no call back allowed on real variable or constant */
   __vpi_err(1836, vpiError,
    "vpi_register_cb real value change object %s illegal",
    __to_vpionam(__wrks1, hrp->htyp));
bad_end: 
   __pop_itstk();
   return(NULL);
  case vpiPartSelect: case vpiFuncCall: case vpiSysFuncCall: 
  case vpiOperation: 
   xp = hrp->hu.hxp;
bit_expr_form:
   /* normal rhs expression for which value change monitoring possible */
   cbref = bld_cbrec(cb_data_p, CB_VALCHG);
   hp2 = (struct h_t *) cbref;
   /* LSB for both bit select and array index */
   hp2->hrec->hu.hcbp->cb_ndxobj = (xp->optyp == LSB) ? TRUE : FALSE;
   bld_cbvc_dces(xp, hp2->hrec->hu.hcbp);
   break;
  case vpiPrimTerm:
   /* SJM 11/22/00 - add ability to put call backs on non tran/pull */
   /* gate outputs */
   gp = hrp->hu.hgp;
   /* for tran/pull put call back on expr - can go on either */
   if (gp->g_class == GC_PULL || gp->g_class == GC_TRAN
    || gp->g_class == GC_TRANIF)
    {
     if (gp->g_class == GC_TRANIF && hrp->hi == 2)
      {
       __vpi_err(1912, vpiError,
        "vpi_register_cb of value change for vpiPrimTerm (pos. %d) of %s.%s illegal input - only output or inout allowed",
        hrp->hi + 1, __msg2_blditree(__xs, hp->hin_itp), gp->gsym->synam);
       goto bad_end;
      }
     xp = hrp->hu.hgp->gpins[hrp->hi];
     /* SJM 11/28/00 - also must change handle to terminal index handle */
     ihref = __mk_exprclass_handle(xp, hp->hin_itp, hrp->hin_tskp);
     /* LOOKATME - must replace cb object passed by user to make this work */ 
     cb_data_p->obj = ihref;
     hp = (struct h_t *) ihref;
     hrp = hp->hrec;
     goto bit_expr_form;
    }
   /* if value monitoring used, must be strength - call backs always */
   /* monitor strengths for mos/cmos gates and never for others */
   if (gp->g_class == GC_LOGIC || gp->g_class == GC_UDP)
    {
     if (cb_data_p->value != NULL
      && cb_data_p->value->format == vpiStrengthVal)
      {
       __vpi_err(2055, vpiWarning,
       "vpi_register_cb primtive output value change form for logic/udp gate %s.%s strength never changes but format value vpiStrengthVal",
        __msg2_blditree(__xs, hp->hin_itp), gp->gsym->synam);
      }
    }
   else 
    {
     if (cb_data_p->value != NULL
      && cb_data_p->value->format != vpiStrengthVal)
      {
       __vpi_err(2055, vpiWarning,
       "vpi_register_cb primtive output value change form for mos/bufif gate %s.%s always called for strength changes but format value non strength",
        __msg2_blditree(__xs, hp->hin_itp), gp->gsym->synam);
      }
    }
   cbref = gateout_valchg_register(hp, cb_data_p);
   break;
  default:
   __vpi_err(1839, vpiError,
    "vpi_register_cb object %s illegal for cbValueChange reason",
    __to_vpionam(__wrks1, hrp->htyp));
   goto bad_end;
 }
 __pop_itstk();

 /* succeeded link dce's onto list associated with this cb for removing */
 hp2 = (struct h_t *) cbref;
 /* link on list because may be multiple for different nets from expr */
 hp2->hrec->hu.hcbp->cbdcep = __cbvc_dcehdr;
 /* SJM - 05/23/00 - if stren format, for each stren net off no mon stren */
 if (hp2->hrec->hu.hcbp->cb_retvalfmt == vpiStrengthVal)
  { 
   set_dce_strenchg_on(__cbvc_dcehdr);
  }

 __cbvc_dcehdr = NULL;
 return(cbref);
}

/*
 * check value change callback passed cb_data record
 */
static int32 chk_valchg_cb(p_cb_data cb_data_p, struct h_t *hp)
{
 if (!__validate_nonit_handle("vpi_register_cb (value change obj)", hp))
  return(FALSE);
 if (cb_data_p->time == NULL) 
  {
   __vpi_err(1850, vpiError,
    "vpi_register_cb cbValueChange p_cb_data record required time field empty"); 
   return(FALSE);
  }
 if (cb_data_p->value == NULL) 
  {
   __vpi_err(1931, vpiError,
    "vpi_register_cb cbValueChange p_cb_data record required value field empty"); 
   return(FALSE);
  }

 /* validate time format and value format */
 if (!__validate_time_type("vpi_register_cb cbValueChange",
  cb_data_p->time->type)) return(FALSE);
 if (!__validate_value_fmt("vpi_register_cb cbValueChange",
  cb_data_p->value->format)) return(FALSE);
 return(TRUE);
}

/*
 * build a call back record and build associated handle
 *
 * notice need handle for cb remove and dce points to cbrec
 * if call back handle is freed, no way to remove call back
 *
 * know value and time fields point to records or will not get here
 */
static vpiHandle bld_cbrec(p_cb_data cb_data_p, word32 cbclass)
{
 vpiHandle cbref;
 struct cbrec_t *cbp;
 struct h_t *hp, *hp2;
 struct hrec_t *hrp2;
 struct gate_t *gp;

 /* no need to init because user always creates and passes */
 cbp = (struct cbrec_t *) __my_malloc(sizeof(struct cbrec_t));
 cbp->cb_reason = cb_data_p->reason; 
 cbp->cb_class = cbclass;
 cbp->cb_ndxobj = FALSE;
 /* SJM 07/24/00 - call backs can be set off by user using sim control */
 cbp->cb_user_off = FALSE;
 cbp->cb_gateout = FALSE;
 /* if object passed must copy handle so user can free or reuse */
 if (cb_data_p->obj != NULL)
  {
   /* user can now free or reuse object */ 
   hp = (struct h_t *) cb_data_p->obj;
   /* this is same as copy object */
   hp2 = (struct h_t *) __mk_handle(hp->hrec->htyp, NULL, NULL, NULL);
   hp2->hin_itp = hp->hin_itp; 
   hrp2 = hp2->hrec;
   *(hrp2) = *(hp->hrec);
   /* if copy from iterator, no longer in iterator */
   hrp2->in_iter = FALSE; 
   if (hrp2->free_xpr) hrp2->hu.hxp = __sim_copy_expr(hp->hrec->hu.hxp);
   cbp->cb_hp = hp2;
  }
 else cbp->cb_hp = NULL;

 cbp->cb_rtn = cb_data_p->cb_rtn;
 cbp->cb_user_data = cb_data_p->user_data;

 if (cb_data_p->time != NULL)
  cbp->cb_rettimtyp = (word32) cb_data_p->time->type; 
 else cbp->cb_rettimtyp = vpiSuppressTime;
 
 if (cb_data_p->value != NULL)
  cbp->cb_retvalfmt = (word32) cb_data_p->value->format; 
 else cbp->cb_retvalfmt = vpiSuppressVal;
 cbp->cbdcep = NULL;
 cbp->cbtevpi = -1;

 /* link on to front design wide list */
 if (__vpi_cbrec_hdr == NULL)
  { __vpi_cbrec_hdr = cbp; cbp->cbnxt = cbp->cbprev = NULL; }
 else
  {
   cbp->cbnxt = __vpi_cbrec_hdr;
   cbp->cbnxt->cbprev = cbp;
   cbp->cbprev = NULL;
   __vpi_cbrec_hdr = cbp;
  }
 /* callback handles never have itree loc, but obj might */
 cbref = __mk_handle(vpiCallback, (void *) cbp, NULL, NULL);
 cbp->cb_cbhp = (struct h_t *) cbref;

 /* 11/28/00 - special processing for gate out term cb */
 /* BEWARE - must turn off optimization on fly when gate out */
 /* terminal call back is acclerated output of logic gate */
 /* only need to turn off for logic gates */ 
 /* problem here is that this is only value change call back */
 /* that can't use net scheduling mechanism but must be checked for */ 
 if (cbp->cb_gateout)
  { 
   gp = cbp->cb_hp->hrec->hu.hgp;
   if (gp->g_class == GC_LOGIC) __logic_acc_off(gp);
  }

 return(cbref);
}

/*
 * build and link on special cb val chg change dce for one callback
 * xp is expr. - if simple expr. (reg/wire handle not expr) 
 *
 * here because variable index becomes entire range all indices are
 * already normalized h:0 constants
 */
static void bld_cbvc_dces(struct expr_t *xp, struct cbrec_t *cbp)
{
 struct net_t *np;
 int32 biti, bitj;
 word32 *wp;
 struct expr_t *idndp, *ndx;
 struct expr_t *fax;
 
 switch ((byte) xp->optyp) {
  case GLBREF:
   idndp = xp; 
   biti = bitj = -1;
glb_dce:
   np = idndp->lu.sy->el.enp;
   linkon_cb_dce(np, biti, bitj, idndp->ru.grp, cbp, DCE_CBVC);
   break;
  case ID:
   idndp = xp; 
   np = xp->lu.sy->el.enp;
   linkon_cb_dce(np, -1, -1, NULL, cbp, DCE_CBVC);
   break;
  /* SJM 05/18/00 - must do nothing for reals */ 
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM: case OPEMPTY:
   return;
  case LSB:
   idndp = xp->lu.x; 
   np = idndp->lu.sy->el.enp;
   ndx = xp->ru.x;
   /* any reg or non scalaraed wire must trigger on any chg */
   if (ndx->optyp == NUMBER)
    {
     wp = &(__contab[ndx->ru.xvi]);
     if (wp[1] != 0L) biti = -1; else biti = (int32) wp[0];
    }
   else if (ndx->optyp == ISNUMBER)
    {
     /* because cb dces are per instance, for IS form just need to get */
     /* right index and use for per. inst. dces - no need for -2 form */
     wp = &(__contab[ndx->ru.xvi]);
     wp = &(wp[2*__inum]);
     /* need length for IS number because can be wider - but get low */
     if (wp[1] != 0L) biti = -1; else biti = (int32) wp[0];
    }
   else
    {
     /* notice for monitor and dctrl event change, variable here is legal */
     /* and implies change for index and trigger on all bits of variable */
     bld_cbvc_dces(ndx, cbp);
     biti = -1;
    }
   if (biti != -1 && !np->vec_scalared) biti = -1;
   /* know non -1 value already normalized because constant normalized to */
   /* h:0 when source read */
   if (idndp->optyp == GLBREF) { bitj = biti; goto glb_dce; }
   linkon_cb_dce(np, biti, biti, NULL, cbp, DCE_CBVC);
   break;
  case PARTSEL:
   idndp = xp->lu.x; 
   np = idndp->lu.sy->el.enp;
   ndx = xp->ru.x;
   /* know part select never IS and normalized during fixup */
   biti = (int32) __contab[ndx->lu.x->ru.xvi];
   bitj = (int32) __contab[ndx->ru.x->ru.xvi];
   if (!np->vec_scalared) biti = bitj = -1;
   if (idndp->optyp == GLBREF) goto glb_dce;
   linkon_cb_dce(np, biti, bitj, NULL, cbp, DCE_CBVC);
   break;
  case FCALL:
   /* if any arguments of system or user functions change, monitor triggers */
   /* notice $time function do not have arguments */
   for (fax = xp->ru.x; fax != NULL; fax = fax->ru.x)
    bld_cbvc_dces(fax->lu.x, cbp);
   break;
  case LCB:
   for (fax = xp->ru.x; fax != NULL; fax = fax->ru.x)
    bld_cbvc_dces(fax->lu.x, cbp);
   break;
  default:
   if (xp->lu.x != NULL) bld_cbvc_dces(xp->lu.x, cbp);
   if (xp->ru.x != NULL) bld_cbvc_dces(xp->ru.x, cbp);
   break;
 } 
}

/*
 * link on a special (one per inst) callback valchg dce
 *
 * -2 IS form impossible any one inst. IS form made constant before here
 * need old value for ranges since need exact change processing
 *
 * passed biti and bitj must be normalized to internal h:0
 */
static void linkon_cb_dce(struct net_t *np, int32 biti, int32 bitj,
 struct gref_t *grp, struct cbrec_t *cbp, int32 dcetyp)
{
 register struct dcevnt_t *dcep;
 int32 nd_itpop;
 struct itree_t *ref_itp;
 struct dceauxlst_t *dclp;

 ref_itp = __inst_ptr;
 nd_itpop = FALSE;
 if (grp != NULL) { __xmrpush_refgrp_to_targ(grp); nd_itpop = TRUE; }
 /* allocate, init, and fill the fields */
 dcep = __alloc_dcevnt(np);
   
 if (biti == -1) dcep->dce_typ = dcetyp;
 else
  {
   if (dcetyp == DCE_CBVC) dcep->dce_typ = DCE_RNG_CBVC;
   else if (dcetyp == DCE_CBF) dcep->dce_typ = DCE_RNG_CBF;
   else dcep->dce_typ = DCE_RNG_CBR;
   dcep->dci1 = biti;
   dcep->dci2.i = bitj;
  }

 /* since no dce, no loads, and no dmpvars must always turn chg store on */
 if (!np->nchg_nd_chgstore)
  {
   /* this also regen net's decl iops from dce if -O on */
   __dce_turn_chg_store_on(__inst_mod, dcep, FALSE);  
  }
 /* SJM 02/06/03 - may have npps but not dces so must turn this on */
 /* since nchg nd chgstore on, know nchg action right */

 /* SJM 04/14/04 - if wire also need to turn on when dumpvars execed later */
 /* this was wrongly only turning on for regs - also needed for nets */
 np->nchg_has_dces = TRUE;

 /* link this on front */
 /* notice goes in front of pvc dces? */
 dcep->dcenxt = np->dcelst;
 np->dcelst = dcep;

 dcep->dce_1inst = TRUE;
 /* this is itree dcep put on */
 dcep->dce_matchitp = __inst_ptr;
 /* this is placed referenced, unless xmr same as match */
 dcep->dce_refitp = ref_itp;
 /* know in right itree place and need prevous value for selects */
 __alloc_1instdce_prevval(dcep);
 __init_1instdce_prevval(dcep);
 dcep->dceu.dce_cbp = cbp;

 /* then link on cb undo/chg list - need if remove called */
 dclp = (struct dceauxlst_t *) __my_malloc(sizeof(struct dceauxlst_t));
 dclp->ldcep = dcep; 
 dclp->dclnxt = __cbvc_dcehdr;
 __cbvc_dcehdr = dclp;

 if (nd_itpop) __pop_itstk();
}

/*
 * for val change callback with vpi stren value - turn off no stren mon
 * if wire is stren model
 *
 * need to go through list since one for each net
 * only call if vpi_ value change callback stren format is stren change 
 */
static void set_dce_strenchg_on(struct dceauxlst_t *dceaux_hd)
{
 register struct dceauxlst_t *dceauxlp;

 for (dceauxlp = dceaux_hd; dceauxlp != NULL; dceauxlp = dceauxlp->dclnxt)
  {
   if (!dceauxlp->ldcep->dce_np->n_stren) continue;

   dceauxlp->ldcep->dce_nomonstren = FALSE;
  }
}

/*
 * allocate and add value change call back to gate output per inst list
 */
static vpiHandle gateout_valchg_register(struct h_t *hp,
 struct t_cb_data *datp)
{
 register int32 i, last_i;
 register struct mod_t *mdp;
 int32 gi, tevpi;
 word64 timval;
 struct gate_t *gp;
 vpiHandle cbref;
 struct cbrec_t *cbp;
 struct h_t *hp2;

 gp = hp->hrec->hu.hgp;
 mdp = __inst_mod; 
 /* if first primitive out cb for this module allocate list headers */
 if (mdp->mgateout_cbs == NULL)
  {
   mdp->mgateout_cbs = (i_tev_ndx **)
    __my_malloc(mdp->mgnum*sizeof(i_tev_ndx *));
   for (i = 0; i < mdp->mgnum; i++) mdp->mgateout_cbs[i] = NULL;
  }
 gi = gp - mdp->mgates; 
 if (mdp->mgateout_cbs[gi] == NULL)
  {
   mdp->mgateout_cbs[gi] = (i_tev_ndx *)
    __my_malloc(mdp->flatinum*sizeof(int32));
   for (i = 0; i < mdp->flatinum; i++) mdp->mgateout_cbs[gi][i] = -1;
  }
 __have_vpi_gateout_cbs = TRUE;

 /* need a cbref to hold passsed cb data record */
 cbref = bld_cbrec(datp, CB_VALCHG);
 hp2 = (struct h_t *) cbref;
 cbp = hp2->hrec->hu.hcbp;
 cbp->cb_gateout = TRUE;

 /* need an event so cb can be removed */
 timval = 0ULL;
 alloc_tev_(tevpi, 0, NULL, timval);
 __tevtab[tevpi].tu.tehp = hp2;
 /* needed for remove because must match event from non delay sched list */
 cbp->cbtevpi = tevpi;

 /* using a list of events so can free and reuse while linking out */
 /* from all cb list */ 
 /* LOOKATME could store extra field to speed search up but lists short */
 /* link on end of list */
 if ((i = mdp->mgateout_cbs[gi][__inum]) == -1)
  {
   mdp->mgateout_cbs[gi][__inum] = tevpi;
  }
 else 
  {
   last_i = -1;
   for (; i != -1; i = __tevtab[i].tenxti) last_i = i;
   __tevtab[last_i].tenxti = tevpi; 
  }
 return(cbref);
}

/*
 * setup (register) the force or release callback (specific net/reg form)
 *
 * scheme: the lhs forced or released wire has a dce placed on that
 * is checked for force or release statement (or vpi routine) is execed
 * cb remove just unlinks the dce
 *
 * force and release can only be entire reg (but not real or array)
 * or wire or bit select of wire but not non lvalue expression
 */
static vpiHandle rf_cb_register(p_cb_data cb_data_p, int32 qctyp)
{
 int32 rftyp, biti;
 vpiHandle cbref;
 struct h_t *hp, *hp2;
 struct net_t *np;
 struct hrec_t *hrp;
 struct expr_t *ndx, *xp;
 char s1[RECLEN];

 if (qctyp == cbForce) { strcpy(s1, "cbForce"); rftyp = DCE_CBF; }
 else { strcpy(s1, "cbRelease"); rftyp = DCE_CBR; }
 hp = (struct h_t *) cb_data_p->obj;
 if (!chk_rf_cb(cb_data_p, hp, s1)) return(NULL);
 
 /* handle nil obj (cb for all rel or force separately - no dces needed */
 if (cb_data_p->obj == NULL) return(rf_all_register_cb(cb_data_p, qctyp));

 hrp = hp->hrec; 
 __cbvc_dcehdr = NULL;
 __push_itstk(hp->hin_itp);
 switch (hrp->htyp) {
  case vpiNet: case vpiReg: case vpiIntegerVar: case vpiTimeVar:
   np = hrp->hu.hnp;
   cbref = bld_cbrec(cb_data_p, CB_VALCHG);
   hp2 = (struct h_t *) cbref;
   linkon_cb_dce(np, -1, -1, NULL, hp2->hrec->hu.hcbp, rftyp);
   break;
  case vpiNetBit:
   /* this can be either form */
   if (!hrp->bith_ndx)
    {
     /* DBG remove --- */
     if (hrp->hu.hxp->optyp != LSB) __vpi_terr(__FILE__, __LINE__);
     /* --- */ 
     ndx = hrp->hu.hxp->ru.x;
     /* know index is 32 bits */
     if (!__expr_is_vpiconst(ndx))
      {
bad_bsel:
       __vpi_err(1834, vpiError,
       "vpi_register_cb %s non constant or 'bx bit select of vpiNetBit illegal",
        s1);
       __pop_itstk();
       return(NULL);
      }

     xp = hrp->hu.hxp->lu.x;
     np = xp->lu.sy->el.enp;
     /* know correct itree loc. on top of stack - this may access IS number */
     /* but never variable - and know already normalized to h:0 */
     biti = __comp_ndx(np, ndx);
     if (biti == -1) goto bad_bsel;
    }
   /* stored in normalized h:0 range */
   else { np = hrp->hu.hnp; biti = hrp->hi; }

   cbref = bld_cbrec(cb_data_p, CB_VALCHG);
   hp2 = (struct h_t *) cbref;
   /* must filter bit or array index */
   linkon_cb_dce(np, biti, biti, NULL, hp2->hrec->hu.hcbp, rftyp);
   break;
  default:
   __vpi_err(1839, vpiError, "vpi_register_cb %s object %s illegal", s1,
    __to_vpionam(__wrks1, hrp->htyp));
   __pop_itstk();
   return(NULL);
 }
 __pop_itstk();
 if (qctyp == cbForce) __num_vpi_force_cbs++; else __num_vpi_rel_cbs++; 
 return(cbref);
}

/*
 * check force/release callback passed cb_data record
 */
static int32 chk_rf_cb(p_cb_data cb_data_p, struct h_t *hp, char *rfnam)
{
 /* if obj nil, legal means all forces/release form */
 if (hp != NULL)
  {
   if (!__validate_nonit_handle("vpi_register_cb", hp)) return(FALSE);
  }
 if (cb_data_p->time == NULL) 
  {
   __vpi_err(1932, vpiError,
    "vpi_register_cb %s p_cb_data record required time field empty", rfnam); 
   return(FALSE);
  }
 if (cb_data_p->value == NULL) 
  {
   __vpi_err(1933, vpiError,
    "vpi_register_cb %s p_cb_data record required value field empty", rfnam); 
   return(FALSE);
  }

 /* validate time format and value format */
 if (!__validate_time_type("vpi_register_cb", cb_data_p->time->type))
  return(FALSE);
 if (!__validate_value_fmt("vpi_register_cb", cb_data_p->value->format))
  return(FALSE);
 return(TRUE);
}

/*
 * return T if expression will be converted to vpiConst
 */
extern int32 __expr_is_vpiconst(struct expr_t *xp)
{
 switch ((byte) xp->optyp) {
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM: case OPEMPTY:
   return(TRUE);
 }
 return(FALSE);
}

/*
 * setup (register) every force or release callback
 *
 * caller must make right itree loc.
 *
 * here on any change check flag and if set go through list of
 * all registered all changes cbs (can be many) and exec routine
 * before cb, build and sets handle (handle can must be copied by user)
 */
static vpiHandle rf_all_register_cb(p_cb_data cb_data_p, int32 qctyp)
{
 vpiHandle cbref;
 struct h_t *hp;
 struct rfcblst_t *rfp;

 /* build the cbrec and handle */
 cbref = bld_cbrec(cb_data_p, CB_VALCHG);
 hp = (struct h_t *) cbref;

 rfp = (struct rfcblst_t *) __my_malloc(sizeof(struct rfcblst_t)); 
 rfp->rfcbp = hp->hrec->hu.hcbp;
 rfp->rfcbnxt = NULL;
 
 /* link on to right list and update flags */
 if (qctyp == cbForce) 
  {
   if (!__vpi_force_cb_always)
    {
     __vpi_force_cb_always = TRUE;
     __force_allcb_hdr = __force_allcb_end = rfp;
    }
   else { __force_allcb_end->rfcbnxt = rfp; __force_allcb_end = rfp; }
  }
 else
  {
   if (!__vpi_rel_cb_always)
    { __vpi_rel_cb_always = TRUE; __rel_allcb_hdr = __rel_allcb_end = rfp; }
   else { __rel_allcb_end->rfcbnxt = rfp; __rel_allcb_end = rfp; }
  }
 return(cbref);
}

/*
 * from force - go thru net dces looking for force cb 
 */
extern void __find_call_force_cbs(struct net_t *np, int32 i1)
{
 register struct dcevnt_t *dcep;
 register struct cbrec_t *cbp;

 for (dcep = np->dcelst; dcep != NULL; dcep = dcep->dcenxt) 
  {
   if (dcep->dce_typ == DCE_RNG_CBF)
    {
     if (i1 != -1 && (i1 < dcep->dci2.i || i1 > dcep->dci1))
      { if (__num_vpi_force_cbs == 1) return; else continue; }
     goto try_non_rng;
    }
   if (dcep->dce_typ != DCE_CBF) continue;

try_non_rng:
   /* SJM 07/24/00 - must turn off PLI 1.0 PV dces from inside self */
   /* LOOKATME - on/off of cbs may break use of counter to stop loop */
   /* think only does small amount of extra work */
   if (dcep->dce_off) continue;

   /* only entire reg/wire or 1 bit of wire allowed here */
   if (dcep->dce_matchitp != __inst_ptr) continue;
   /* have matching force */  
   cbp = dcep->dceu.dce_cbp; 

   /* SJM 07/24/00 - must run with this callback off in case rf in user code */
   dcep->dce_off = TRUE;

   /* SJM 10/06/06  - must run with this cb off - if user did not turn off */ 
   /* or remove call back, cbvc routine turns back on */
   __cbvc_callback(dcep, cbp, cbp->cb_hp);


   /* SJM 10/06/06 - call back routine turns back on if not turned off */
   /* by user */

   /* if only one force cb (common case) can stop now */
   if (__num_vpi_force_cbs == 1) return; 
  }
}

/*
 * from release - go thru net dces looking for release cb 
 */
extern void __find_call_rel_cbs(struct net_t *np, int32 i1)
{
 register struct dcevnt_t *dcep;
 register struct cbrec_t *cbp;

 for (dcep = np->dcelst; dcep != NULL; dcep = dcep->dcenxt) 
  {
   if (dcep->dce_typ == DCE_RNG_CBR)
    {
     if (i1 != -1 && (i1 < dcep->dci2.i || i1 > dcep->dci1))
      { if (__num_vpi_rel_cbs == 1) return; else continue; }
     goto try_non_rng;
    }
   if (dcep->dce_typ != DCE_CBR) continue;
try_non_rng:
   /* only entire reg/wire or 1 bit of wire allowed here */
   if (dcep->dce_matchitp != __inst_ptr) continue;
   /* have matching force */  
   cbp = dcep->dceu.dce_cbp;

   /* SJM 07/24/00 - must run with this callback off in case rf in user code */
   dcep->dce_off = TRUE;

   /* SJM 10/06/06  - must run with this cb off - if user did not turn off */ 
   /* or remove call back, cbvc routine turns back on */
   __cbvc_callback(dcep, cbp, cbp->cb_hp);

   /* SJM 10/06/06 - call back routine turns back on if not turned off */
   /* by user */

   /* if only one force cb (common case) can stop now */
   if (__num_vpi_rel_cbs == 1) return; 
  }
}

/*
 * call all forces and release form registered call backs
 *
 * registered routine passed nil 
 * no dce here - user can register multiple (handle built from changed)
 *
 * this builds a forced/released (after) changed reg or bit handle
 * and does normal vc change call back after that
 * fills dat obj field then frees handle and sets to nil upon return
 *
 * this works for all case because no instance specific filtering 
 * so just use itree loc for one changed
 */
extern void __cb_all_rfs(struct net_t *np, int32 bi, int32 is_force)
{
 register struct rfcblst_t *rfp; 
 word32 sav_cb_ndxobj;
 struct cbrec_t *cbp;
 struct h_t *hp;
 struct task_t *tskp;

 /* SJM 07/24/00 - if user executes force/release from within all rf */
 /* type call back must not see rf callback again */ 
 if ( __allrel_cbs_off) return;

 __vpifnam_ind = __sfnam_ind;
 __vpilin_cnt = __slin_cnt;
 tskp = __getcur_scope_tsk();
 /* SJM 07/24/00 - if user executes force/release from within all rf */
 /* type call back must not see rf callback again */ 
 if (is_force)
  {
   if (__allforce_cbs_off) return;
   rfp = __force_allcb_hdr;
  }
 else
  {
   if (__allrel_cbs_off) return;
   rfp = __rel_allcb_hdr; 
  } 

 for (; rfp != NULL; rfp = rfp->rfcbnxt)
  {
   cbp = rfp->rfcbp;
   sav_cb_ndxobj = cbp->cb_ndxobj;
   if (bi == -1)
    {  
     cbp->cb_ndxobj = FALSE;
     hp = (struct h_t *) __mk_handle(vpiNet, (void *) np, __inst_ptr, tskp);
    }
   else 
    {
     cbp->cb_ndxobj = TRUE;
     hp = (struct h_t *) __mk_handle(vpiNetBit, (void *) np, __inst_ptr, tskp);
     hp->hrec->hi = bi;
    }
   /* SJM 07/24/00 - all force/release form cbs require no force/release */ 
   /* call backs allows from within user callback routine in case user code */
   /* does some forcing and releasing */
   if (is_force) __allforce_cbs_off = TRUE; else __allrel_cbs_off = TRUE;

   /* since know cbp dce pointer also nil, will not access dce field */
   __cbvc_callback(NULL, cbp, hp);

   if (is_force) __allforce_cbs_off = FALSE; else __allrel_cbs_off = FALSE;

   /* this just frees temp handle allocated for call back */  
   cbp->cb_ndxobj = sav_cb_ndxobj; 
   /* free the temp object - because can be indexed need vpi free */
   vpi_free_object((vpiHandle) hp);
  }
 __sfnam_ind = __vpifnam_ind;
 __slin_cnt = __vpilin_cnt;
}

/*
 * call the cb value change call back routine (process the change)
 * also for force/release - net specific type - only cbrec ptr used
 *
 * for all form of force/release need to pass hp which is one that
 * changed - changes each time called for all (nil) case
 *
 * dcep's always 1 instance - called for any change of expr. 
 *
 * must allocate new cb_data record and set time and and value fields
 * (unless field type suppress) following new LRM rules for val chg cbs 
 *
 * this routine runs with no vpi_ location because event usually changes
 * but does have an itree loc.
 */
extern void __cbvc_callback(struct dcevnt_t *dcep, struct cbrec_t *cbp,
 struct h_t *hp)
{
 int32 biti, ndx;
 struct t_cb_data wrk_cbdata, *datp;
 struct t_vpi_time wrk_time;
 struct t_vpi_value wrk_val;
 struct xstk_t *xsp;
 struct net_t *np; 
 struct h_t *hp2;
 struct hrec_t *hrp2;

 /* suppress call backs during reset */
 if (__run_state == SS_RESET) return; 

 datp = &wrk_cbdata;
 datp->reason = cbp->cb_reason;
 datp->cb_rtn = cbp->cb_rtn;
 /* value change return call back requires all next 3 fields be present */
 /* must be passed for all force/release case - usually just from cbp fld */
 datp->obj = (vpiHandle) hp;
 datp->time = &wrk_time;
 datp->value = &wrk_val;
 datp->user_data = cbp->cb_user_data;

 if (cbp->cb_retvalfmt != vpiSuppressVal)
  {
   datp->value->format = (int32) cbp->cb_retvalfmt;
   vpi_get_value(datp->obj, datp->value);  
  }
 else datp->value->format = vpiSuppressVal;
 if (cbp->cb_rettimtyp != vpiSuppressTime)
  {
   datp->time->type = (int32) cbp->cb_rettimtyp;
   __set_vpi_time(datp->time, &__simtime, datp->time->type, __inst_mod); 
  }
 else datp->time->type = vpiSuppressTime;

 /* if bit handle or bit select or array word32 select handle set index field */
 if (cbp->cb_ndxobj)
  {  
   /* cb object is object not index */
   hp2 = (struct h_t *) datp->obj;
   hrp2 = hp2->hrec;
   /* alt form for expr. handle is index rather than expr. */
   if (!hrp2->bith_ndx)
    {
     /* DBG remove --- */
     if (hrp2->htyp != vpiNetBit && hrp2->htyp != vpiRegBit  
      && hrp2->htyp != vpiVarSelect && hrp2->htyp != vpiMemoryWord) 
      __vpi_terr(__FILE__, __LINE__);

     if (hrp2->hu.hxp->optyp != LSB) __vpi_terr(__FILE__, __LINE__);
     /* --- */

     /* evaluate index expr. even if constant because of IS case */
     __push_itstk(hp2->hin_itp);
     xsp = __eval_xpr(hrp2->hu.hxp->ru.x);
     __pop_itstk();
     if (xsp->bp[0] != 0) datp->index = 0;
     else
      {
       ndx = (int32) xsp->ap[0];
       /* constants must be unnormalized - non constant not normalized so */
       /* no need to unnormalize */
       if (__expr_is_vpiconst(hp2->hrec->hu.hxp->ru.x))
        {
         np = hrp2->hu.hxp->lu.x->lu.sy->el.enp; 
         biti = __unnormalize_ndx(np, ndx);
        }
       else biti = ndx;
       __pop_xstk();
       datp->index = biti;
      }
    }
   /* for non expr bit obj type, stored as internal h:0 need unnormalize */
   else datp->index = __unnormalize_ndx(hrp2->hu.hnp, hrp2->hi);
  }

 /* DBG remove --
 if (__debug_flg && __ev_tracing)
  {
   -* SJM 08/16/01 - BEWARE - not sure if can call vpi routine in here *-
   __tr_msg("<> processing vpi_ %s callback on %s now %s\n", 
    to_cbtypnam(__xs2, datp->reason), vpi_get_str(vpiFullName, datp->obj),
    __to_timstr(__xs, &__simtime));
  }
 --- */

 /* 10/06/06 - this will be set to nil if user code removes the call back */
 /* that is now being processed */ 
 __cbvc_causing_dcep = dcep;

 (*(datp->cb_rtn))(datp);

 /* SJM 07/24/00 - unless user turned off with vpi control turn back on */
 /* user may turn off in value change call back routine */

 /* SJM 10/06/06 - also if user removed this call back in value change */
 /* routine, must not turn back on since dcep will have been freed */

 /* SJM 10/06/06 - the cbvc causing dcep will be nil if user code */
 /* call cb remove for the currently active call back cbp (and dcep) */
 
 if (__cbvc_causing_dcep != NULL && !dcep->dceu.dce_cbp->cb_user_off)
   dcep->dce_off = FALSE;
 __cbvc_causing_dcep = NULL; 

 /* t_cb_data automatic so no need to free */
}

/*
 * execute a special case primitive output terminal value change call back
 *
 * these can only be freed by cb remove
 */
extern void __exec_vpi_gateoutcbs(register int32 tevpi)
{
 struct cbrec_t *cbp;

 for (; tevpi != -1; tevpi = __tevtab[tevpi].tenxti)
  {
   /* if already cancelled (i.e. callback removed), nothing to do */ 
   /* LOOKATME - think canceling not possible - will have been removed */
   if (__tevtab[tevpi].te_cancel) continue;

   cbp = __tevtab[tevpi].tu.tehp->hrec->hu.hcbp;
   /* SJM 07/24/00 - user can turn off/on with sim control */ 
   if (cbp->cb_user_off) continue;

   /* SJM 10/06/06 - this does not need to run with cb off since no assign */
   __cbvc_callback(NULL, cbp, cbp->cb_hp);
  }
 /* nothing to free since cb_data record in automatic storage */ 
}

/*
 * ROUTINES TO IMPLEMENT DELAY STYLE CALL BACKS
 */

/*
 * routine that registers all delay (time) callbacks
 * removal when done automatic - can be removed before happen
 *
 * for: sim start, wr sync, ro sync, next sim time, normal delay
 *
 * SJM 07/24/00 - delay call backs can't be turned off by user
 */
static vpiHandle delay_cb_register(register p_cb_data datp) 
{
 word64 ticksdel, schtim;
 vpiHandle cbref;
 i_tev_ndx tevpi;
 struct h_t *hp;
 struct mod_t *mdp;

 /* need delay value in internal ticks */
 ticksdel = 0ULL;
 hp = (struct h_t *) datp->obj;
 if (!chk_delay_cb(datp, hp)) return(NULL);

 if (datp->reason != cbNextSimTime)
  {
   /* notice the object does not need to be scope object */
   if (hp != NULL && hp->hin_itp != NULL)
    mdp = hp->hin_itp->itip->imsym->el.emdp;
   else mdp = NULL;  
   if (!__vpitime_to_ticks(&ticksdel, datp->time, mdp)) return(NULL);
  }

 /* #0 cases - add to event queue - at current time */
 if (datp->reason == cbNextSimTime || ticksdel == 0ULL)
  {
   /* can't schedule any #0 in ro sync region */
   if (__rosync_slot && datp->reason != cbNextSimTime)
    { __bad_rosync_err("vpi_register_cb"); return(NULL); }

   cbref = bld_cbrec(datp, CB_DELAY);
   /* notice reusing hp */
   hp = (struct h_t *) cbref;
   /* alloc as #0 */ 
   alloc_tev_(tevpi, TE_VPICBDEL, __it_roots[0], __simtime);
   __tevtab[tevpi].tu.tehp = hp;
   /* back link from cbrec to event */
   hp->hrec->hu.hcbp->cbtevpi = tevpi;

   switch (datp->reason) {
    case cbAtStartOfSimTime:
     __vpi_err(1852, vpiError,
      "vpi_register_cb cbAtStartOfSimTime #0 delay illegal - already past");

     /* on error free actioncb psuedo event, free cbrec, and free callback */ 
     __tevtab[tevpi].tenxti = __tefreelsti;
     __tefreelsti = tevpi;
     free_cbrec(hp->hrec->hu.hcbp);
     return(NULL);
    case cbReadOnlySynch:
     /* link on end of rosync list - exec all cbs in register order */
     if (__vpicb_tehdri[cbReadOnlySynch] == -1)
      __vpicb_tehdri[cbReadOnlySynch] = __vpicb_teendi[cbReadOnlySynch]
       = tevpi;
     else
      {
       __tevtab[__vpicb_teendi[cbReadOnlySynch]].tenxti = tevpi;
       __vpicb_teendi[cbReadOnlySynch] = tevpi;
      }
     __slotend_action |= SE_VPIROSYNC;
     break;
    case cbNextSimTime:
     /* link on front of nxtim list */
     if (__vpicb_tehdri[cbNextSimTime] == -1)
      __vpicb_tehdri[cbNextSimTime] = __vpicb_teendi[cbNextSimTime] = tevpi;
     else
      {
       __tevtab[__vpicb_teendi[cbNextSimTime]].tenxti = tevpi;
       __vpicb_teendi[cbNextSimTime] = tevpi;
      }
     __have_vpi_actions = TRUE;
     break;
    case cbReadWriteSynch:
     /* MAYBE FIXME  - for now same as #0 except future becomes #0 */
    case cbAfterDelay:
     if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevpi;
     else { __tevtab[__p0_te_endi].tenxti = tevpi; __p0_te_endi = tevpi; }
     break;
    default: __vpi_terr(__FILE__, __LINE__);
   }
   return(cbref);
  }
 /* case 2: for later time */ 
 schtim = __simtime + ticksdel;
 cbref = bld_cbrec(datp, CB_DELAY);
 alloc_tev_(tevpi, TE_VPICBDEL, __it_roots[0], schtim);
 hp = (struct h_t *) cbref;
 __tevtab[tevpi].tu.tehp = hp; 
 /* back link from cbrec to event */
 hp->hrec->hu.hcbp->cbtevpi = tevpi;

 /* some later slot */
 switch (datp->reason) {
  case cbAtStartOfSimTime:
   /* add to front instead of end of queue */
   __tevtab[tevpi].vpi_onfront = TRUE;
   __insert_event(tevpi);
   break;
  case cbReadWriteSynch: case cbReadOnlySynch:
   /* schedule ev for future normal event queue, flag frees, alloc, schds #0 */
   __tevtab[tevpi].vpi_reschd = TRUE;
   __insert_event(tevpi);
   break;
  case cbAfterDelay:
   __insert_event(tevpi);
   break;
  default: __vpi_terr(__FILE__, __LINE__);
 }
 return(cbref);
}

/*
 * check force/release callback passed cb_data record
 */
static int32 chk_delay_cb(p_cb_data datp, struct h_t *hp)
{
 /* if optional obj present, it must be valid and a scope object */
 if (hp != NULL)
  {
   if (!__validate_nonit_handle("delay vpi_register_cb", hp)) return(FALSE);
  }
 /* unless next sim time, time field required and must not be suppress */
 if (datp->reason != cbNextSimTime)
  {
   if (datp->time == NULL) 
    {
     __vpi_err(1934, vpiError,
      "delay vpi_register_cb p_cb_data record required time field missing"); 
     return(FALSE);
    }
   /* validate time format */
   if (!__validate_time_type("delay call back", datp->time->type))
    return(FALSE);
   if (datp->time->type == vpiSuppressTime) 
    {
     __vpi_err(1935, vpiError,
      "delay vpi_register_cb p_cb_data record time field type vpiSuppress field illegal"); 
      return(FALSE);
    }
  }
 /* value field ignored - so may be nil - no need to check */
 return(TRUE);
}

/*
 * execute a delay related call back list
 *
 * only for ro sync and nxt sim time (after move time)
 * after event is processed, the cbrec is freed 
 * tevs freed when done en masse here
 * list may be empty if all removed - no way to detect until here
 *
 * SJM 07/24/00 - delay call backs can't be turned off by user
 */
static void exec_vpi_delaycbs(int32 cbtyp)
{
 register i_tev_ndx tevpi;

 /* all may have been cancelled */
 if ((tevpi = __vpicb_tehdri[cbtyp]) == -1) return;

 if (cbtyp == cbReadOnlySynch) __rosync_slot = TRUE; 
 /* if cancelled cbrec will be freed */
 for (; tevpi != -1; tevpi = __tevtab[tevpi].tenxti)
  {
   /* since not scheduled - never can be cancelled */
   /* DBG remove ---
   if (__tevtab[tevpi].te_cancel) __vpi_terr(__FILE__, __LINE__);
   if (__tevtab[tevpi].vpi_reschd) __vpi_terr(__FILE__, __LINE__);
   --- */
   /* when called from here reschd never on */
   __delay_callback(tevpi);
  }

 /* slot end action turned off all at once later */
 /* add entire list to tev free list - know no guts */
 __tevtab[__vpicb_teendi[cbtyp]].tenxti = __tefreelsti;
 __tefreelsti = __vpicb_tehdri[cbtyp];
 __vpicb_tehdri[cbtyp] = __vpicb_teendi[cbtyp] = -1;
 if (cbtyp == cbReadOnlySynch) __rosync_slot = FALSE; 
}

/*
 * call the cb delay routine - event exec is calling cb routine
 * used for all time related cbs
 *
 * when processed (not just moved to other part of slot) will never happen
 * again and handle is freed
 *
 * 2 ways through here - 1) move event to other part of slot list, 2) do
 * callback and free event since done and can never happen again
 *
 * if call back cancelled, event cancelled so will never get here
 */
extern void __delay_callback(i_tev_ndx tevpi)
{
 int32 tevp2i;
 struct t_cb_data wrk_cbdata, *datp;
 struct t_vpi_time wrk_time;
 struct cbrec_t *cbp;
 struct h_t *cbhp;
 struct mod_t *mdp;

 /* DBG remove -- */
 if (__debug_flg && __ev_tracing)
  {
   __tr_msg("-- processing vpi_ delay call back at %s\n", 
   __to_timstr(__xs, &__simtime));
  }
 /* --- */

 cbhp = __tevtab[tevpi].tu.tehp;
 cbp = cbhp->hrec->hu.hcbp; 
 /* move to right sync location */
 if (__tevtab[tevpi].vpi_reschd)
  {
   /* SJM 08/07/00 - must schedule new event since carrier event free */
   /* enmass with all other regular events in now being processed queue */ 
   alloc_tev_(tevp2i, TE_VPICBDEL, __tevtab[tevpi].teitp, __simtime);   
   /* move cb hp ptr to new event */
   __tevtab[tevp2i].tu.tehp = cbhp;
   /* nil field in normal event queue event since it is soon to be freed */
   __tevtab[tevpi].tu.tehp = NULL;
   __num_proc_tevents--;
   /* now all fields same except vpi reschd not on in new - as needed */

   if (cbp->cb_reason == cbReadWriteSynch)
    {
     if (__p0_te_hdri == -1) __p0_te_hdri = __p0_te_endi = tevp2i;
     else { __tevtab[__p0_te_endi].tenxti = tevp2i; __p0_te_endi = tevp2i; }
    }
   else if (cbp->cb_reason == cbReadOnlySynch)
    {
     /* link on end of rosync list since exec cbs in register order */
     if (__vpicb_tehdri[cbReadOnlySynch] == -1)
      __vpicb_tehdri[cbReadOnlySynch] = __vpicb_teendi[cbReadOnlySynch]
       = tevp2i;
     else
      { 
       __tevtab[__vpicb_teendi[cbReadOnlySynch]].tenxti = tevp2i; 
       __vpicb_teendi[cbReadOnlySynch] = tevp2i;
      }
     __slotend_action |= SE_VPIROSYNC;
    }
   else __vpi_terr(__FILE__, __LINE__);
   return;
  }

 datp = &wrk_cbdata;
 datp->reason = cbp->cb_reason;
 datp->cb_rtn = cbp->cb_rtn;
 /* if user passed object (for scope) must return else nil */
 if (cbp->cb_hp != NULL) datp->obj = (vpiHandle) cbp->cb_hp;
 else datp->obj = NULL;

 wrk_time.type = vpiSuppressTime;
 datp->time = &wrk_time;
 /* value always nil */
 datp->value = NULL;
 datp->user_data = cbp->cb_user_data;
 
 /* assign time */  
 if (cbp->cb_rettimtyp != vpiSuppressTime)
  {
   datp->time->type = (int32) cbp->cb_rettimtyp;
   if (cbp->cb_hp != NULL) mdp = cbp->cb_hp->hin_itp->itip->imsym->el.emdp;
   else mdp = NULL;  
   __set_vpi_time(datp->time, &__simtime, datp->time->type, mdp); 
  }

 (*(cbp->cb_rtn))(datp);

 /* datp freed since automatic var */
 /* remove the call back since can't happen again */
 free_cbrec(cbp);
 /* event will be collected by normal mechanism or when all of list done */
 __tevtab[tevpi].tu.tehp = NULL;  
}

/*
 * remove a callback - must unline from global list
 * only field to free is cb_hp copied object - cbtevp not freeable from here
 */
static void free_cbrec(struct cbrec_t *cbp)
{
 struct h_t *cbhp;

 /* link out of list */ 
 /* case 1: at front */
 if (cbp->cbprev == NULL)
  {
   __vpi_cbrec_hdr = cbp->cbnxt;
   if (cbp->cbnxt != NULL) cbp->cbnxt->cbprev = NULL;
  }
 /* case 2: at end but not at front */
 else if (cbp->cbnxt == NULL) cbp->cbprev->cbnxt = NULL;
 /* case 3: inside */
 else { cbp->cbprev->cbnxt = cbp->cbnxt; cbp->cbnxt->cbprev = cbp->cbprev; }

 /* free the copied object handle */
 if (cbp->cb_hp != NULL)
  {
   vpi_free_object((vpiHandle) cbp->cb_hp);
  }
 cbhp = cbp->cb_cbhp;
 __free_hp(cbhp);
 
 __my_free((char *) cbp, sizeof(struct cbrec_t));
}

/*
 * add a call back to a action (or feature) (fixed) reason
 *
 * cbs remain unless explicitly removed
 * value and time fields completely ignored
 */
static vpiHandle action_cb_register(register p_cb_data datp) 
{
 word64 timval;
 vpiHandle cbref;
 struct cbrec_t *cbp; 
 struct h_t *cbhp;
 i_tev_ndx tevpi;

 if (datp->reason == cbCustomer1)
  {
   if (__vpicb_tehdri[datp->reason] != -1)
    {
     __vpi_err(1830, vpiError, "cbCustomer1 action callback unsupported");
     return(NULL);
    }
  }

 /* already checked cb_rtn and reason, rest need not be set at all */
 /* because they are ignored */

 /* need a cbref to hold passsed cb data record */
 cbref = bld_cbrec(datp, CB_ACTION);
 cbhp = (struct h_t *) cbref;
 cbp = cbhp->hrec->hu.hcbp;
 /* need an event so cb can be removed */
 timval = 0ULL;
 alloc_tev_(tevpi, 0, NULL, timval);
 __tevtab[tevpi].tu.tehp = cbhp;
 /* needed for remove because must match event from non delay sched list */
 cbp->cbtevpi = tevpi;

 /* link on to end of list */
 if (__vpicb_tehdri[datp->reason] == -1)
  __vpicb_tehdri[datp->reason] = __vpicb_teendi[datp->reason] = tevpi;
 else
  {
   __tevtab[__vpicb_teendi[datp->reason]].tenxti = tevpi;
   __vpicb_teendi[datp->reason] = tevpi;
  }
 /* need special flag for at least one cbError cb active */
 if (datp->reason == cbError) __vpierr_cb_active = TRUE;
 __have_vpi_actions = TRUE;
 return(cbref);
}

/*
 * try to call start of reset routine
 *
 * wrapper around call since vpi_ include only in vpi_ code
 */
extern void __vpi_startreset_trycall(void)
{
 if (__vpicb_tehdri[cbStartOfReset] != -1)
  exec_vpi_actioncbs(cbStartOfReset);
}

/*
 * execute end of reset callback wrapper
 */
extern void __vpi_endreset_trycall(void)
{
 if (__vpicb_tehdri[cbEndOfReset] != -1)
  exec_vpi_actioncbs(cbEndOfReset);
}

/*
 * execute end of compile callback wrapper
 */
extern void __vpi_endcomp_trycall(void)
{
 if (__vpicb_tehdri[cbEndOfCompile] != -1)
  exec_vpi_actioncbs(cbEndOfCompile);
}

/*
 * execute start of sim callback wrapper
 */
extern void __vpi_startsim_trycall(void)
{
 if (__vpicb_tehdri[cbStartOfSimulation] != -1)
  exec_vpi_actioncbs(cbStartOfSimulation);
}

/*
 * execute end of sim callback wrapper
 */
extern void __vpi_endsim_trycall(void)
{
 if (__vpicb_tehdri[cbEndOfSimulation] != -1)
  exec_vpi_actioncbs(cbEndOfSimulation);
}

/*
 * try to call enter iact reason wrapper
 */
extern void __vpi_enteriact_trycall(void)
{
 __vpifnam_ind = __sfnam_ind;
 __vpilin_cnt = __slin_cnt;
 
 if (__vpicb_tehdri[cbEnterInteractive] != -1)
  exec_vpi_actioncbs(cbEnterInteractive);

 __vpifnam_ind = 0;
 __vpilin_cnt = 0;
}

/*
 * try to call exit iact reason wrapper
 */
extern void __vpi_exitiact_trycall(void)
{
 __vpifnam_ind = __sfnam_ind;
 __vpilin_cnt = __slin_cnt;

 if (__vpicb_tehdri[cbExitInteractive] != -1)
  exec_vpi_actioncbs(cbExitInteractive);

 __vpifnam_ind = 0;
 __vpilin_cnt = 0;
}

/*
 * try to call interactive scope change reason wrapper
 * this sets cb data handle using alloced and freed after call handle obj
 */
extern void __vpi_iactscopechg_trycall(void)
{
 word32 parhtyp;
 vpiHandle scopobj;
 struct t_cb_data wrk_cbdata, *datp;
 struct cbrec_t *cbp;
 i_tev_ndx tevpi;
 struct mod_t *mdp;
 struct task_t *up_tskp;
 struct symtab_t *sytp; 

 if ((tevpi = __vpicb_tehdri[cbInteractiveScopeChange]) == -1) return;

 __vpifnam_ind = __sfnam_ind;
 __vpilin_cnt = __slin_cnt;

 /* interactive scope change call back sets obj field to new scope */ 
 if (__scope_tskp == NULL)
  {
   mdp = __scope_ptr->itip->imsym->el.emdp;
   scopobj = __mk_handle(vpiModule, (void *) mdp, __scope_ptr, NULL);
  }
 else
  {
   sytp = __scope_tskp->tsksymtab;
   if (sytp->sytpar == NULL || sytp->sytpar->sypofsyt->sytyp == SYM_M)
    up_tskp = NULL;
   else up_tskp = sytp->sytpar->sypofsyt->el.etskp;

   parhtyp = __to_vpi_tasktyp(__scope_tskp->tsktyp);
   scopobj = __mk_handle(parhtyp, (void *) __scope_tskp, __scope_ptr, up_tskp);
  }

 datp = &wrk_cbdata;
 /* for action callback next 2 must be nil */
 datp->time = NULL;
 datp->value = NULL;
 /* scope object always return as obj - required */
 datp->obj = scopobj;

 for (; tevpi != -1; tevpi = __tevtab[tevpi].tenxti)
  {
   /* DBG remove --- */
   if (__tevtab[tevpi].te_cancel) __vpi_terr(__FILE__, __LINE__);
   /* --- */

   cbp = __tevtab[tevpi].tu.tehp->hrec->hu.hcbp;

   /* SJM 07/24/00 - user can turn off/on with sim control */ 
   if (cbp->cb_user_off) continue;

   datp->reason = cbp->cb_reason;
   datp->cb_rtn = cbp->cb_rtn;
   datp->user_data = cbp->cb_user_data;

   (*(datp->cb_rtn))(datp);
  }
 /* datp automatic so no need to free */
 /* must leave callback cbrec until released */

 /* can't free created scope object - because user may save and use */ 

 __vpifnam_ind = 0;
 __vpilin_cnt = 0;
}

/*
 * try to call next sim time (next move time time) reason wrapper
 * this is delay cb not action
 */
extern void __vpi_del_nxtsimtim_trycall(void)
{
 if (__vpicb_tehdri[cbNextSimTime] != -1) exec_vpi_delaycbs(cbNextSimTime);
}

/*
 * call the rosync (know slot bit set)
 * this is del cb not action
 */
extern void __vpi_del_rosync_call(void)
{
 exec_vpi_delaycbs(cbReadOnlySynch);
}

/*
 * try to call tchk violation reason wrapper
 */
extern void __vpi_tchkerr_trycall(struct tchk_t *tcp, struct itree_t *itp)
{
 __vpifnam_ind = tcp->tcsym->syfnam_ind;
 __vpilin_cnt = tcp->tcsym->sylin_cnt;

 if (__vpicb_tehdri[cbTchkViolation] != -1)
  exec_vpi_tchkerr(cbTchkViolation, tcp, itp);

 __vpifnam_ind = 0;
 __vpilin_cnt = 0;
}

/*
 * try to call vpi_ PLI error (only for __vpi errors) reason wrapper
 *
 * here error location maybe set by caller of error
 *
 * if error call back generated by routine called from error cb
 * end with fatal error since serious user code bug (infinite loop)
 */
extern void __vpi_plierror_trycall(void)
{
 if (__acc_vpi_erroff) return;

 if (__vpicb_tehdri[cbPLIError] != -1)
  {
   if (__in_vpi_errorcb) 
    {
     __pv_vpi_terr(303,
     "cbPLIError callback caused recursive error callback - giving up"); 
    }
   __in_vpi_errorcb = TRUE;
   exec_vpi_actioncbs(cbPLIError);
   __in_vpi_errorcb = FALSE;
  }
}

/*
 * try to call vpi_ non PLI error reason wrapper
 *
 * all Cver errors except interactive try to call these callbacks
 * vpi_chk_error used inside these call backs to get Cver einfo
 */
extern void __vpi_error_trycall(void)
{
 if (__acc_vpi_erroff) return;

 if (__vpicb_tehdri[cbError] == -1) return;

 if (__in_vpi_errorcb) 
  {
   __pv_vpi_terr(303,
    "cbError callback caused recursive error callback - giving up"); 
  }
 __in_vpi_errorcb = TRUE;
 exec_vpi_actioncbs(cbError);
 __in_vpi_errorcb = FALSE;

 /* PLI error status on until next routine, but other Cver errors */
 /* only pending in callback - if no callback no error ever pending */
 /* and when all callbacks done, always turned off */
 /* needed because no obvious place to reset non PLI errors */
 __last_eip = NULL;
}


/*
 * execute a fixed action or feature call back list
 *
 * these can only be freed by cb remove
 */
static void exec_vpi_actioncbs(int32 cbtyp)
{
 register i_tev_ndx tevpi;
 struct cbrec_t *cbp;
 struct t_cb_data wrk_cbdata, *datp;

 if ((tevpi = __vpicb_tehdri[cbtyp]) == -1) return;

 datp = &wrk_cbdata;
 /* for action cbs, next 3 nil */
 datp->time = NULL;
 datp->value = NULL;
 datp->obj = NULL;

 for (; tevpi != -1; tevpi = __tevtab[tevpi].tenxti)
  {
   /* if already cancelled (i.e. callback removed), nothing to do */ 
   if (__tevtab[tevpi].te_cancel) continue;

   cbp = __tevtab[tevpi].tu.tehp->hrec->hu.hcbp;
   /* SJM 07/24/00 - user can turn off/on with sim control */ 
   if (cbp->cb_user_off) continue;

   datp->reason = cbp->cb_reason;
   datp->cb_rtn = cbp->cb_rtn;
   datp->user_data = cbp->cb_user_data;

   (*(datp->cb_rtn))(datp);
  }
 /* nothing to free since cb_data record in automatic storage */ 
}

/*
 * execute a timing check feature call back list
 * these can only be freed by cb remove (or reset)
 */
static void exec_vpi_tchkerr(int32 cbtyp, struct tchk_t *tcp,
 struct itree_t *itp)
{
 register i_tev_ndx tevpi;
 struct cbrec_t *cbp;
 struct t_cb_data wrk_cbdata, *datp;

 if ((tevpi = __vpicb_tehdri[cbtyp]) == -1) return;

 datp = &wrk_cbdata;
 /* for action cbs, next 2 nil */
 datp->time = NULL;
 datp->value = NULL;
 /* free on return when all call backs done */
 datp->obj = __mk_handle(vpiTchk, (void *) tcp, itp, NULL);

 for (; tevpi != -1; tevpi = __tevtab[tevpi].tenxti)
  {
   /* if cancelled, cbrec will have been freed */ 
   if (__tevtab[tevpi].te_cancel) continue;

   cbp = __tevtab[tevpi].tu.tehp->hrec->hu.hcbp;

   /* SJM 07/24/00 - user can turn off/on with sim control */ 
   if (cbp->cb_user_off) continue;

   datp->reason = cbp->cb_reason;
   datp->cb_rtn = cbp->cb_rtn;

   (*(datp->cb_rtn))(datp);
  }
 /* can't free timing check object - caller may use it */
}

/*
 * execute a Cver extended foreign language line cb 
 * call back is cbLanguageLine
 * these can only be freed by cb remove
 *
 * this fills value record misc field with ptr to line record
 */
extern void __exec_vpi_langlinecbs(char *lp, char *fnam, int32 lno)
{
 register i_tev_ndx tevpi;
 struct t_vpi_value wrk_val;
 struct t_vpi_languageline wrk_langlin;
 struct cbrec_t *cbp;
 struct t_cb_data wrk_cbdata, *datp;

 if ((tevpi = __vpicb_tehdri[cbLanguageLine]) == -1) return;

 datp = &wrk_cbdata;
 /* for action cbs, next 3 nil */
 datp->time = NULL;
 datp->value = &wrk_val;
 datp->value->format = vpiStringVal;
 datp->value->value.misc = (char *) &wrk_langlin;
 wrk_langlin.linep = lp;
 wrk_langlin.file = fnam;
 wrk_langlin.lineno = lno;
 datp->obj = NULL;

 for (; tevpi != -1; tevpi = __tevtab[tevpi].tenxti)
  {
   /* if already cancelled (i.e. callback removed), nothing to do */ 
   if (__tevtab[tevpi].te_cancel) continue;

   cbp = __tevtab[tevpi].tu.tehp->hrec->hu.hcbp;
   datp->reason = cbp->cb_reason;
   datp->cb_rtn = cbp->cb_rtn;
   datp->user_data = cbp->cb_user_data;

   (*(datp->cb_rtn))(datp);
  }
 /* nothing to free since cb_data record in automatic storage */ 
}

/*
 * CALLBACK REMOVAL ROUTINES
 */

/*
 * remove a call back - find and free the cbrec
 *
 * handle is also freed because if used again will cause error
 * can access handle from event and cbrec from handle and back link from 
 * cbrec to event
 *
 * notice not freeing p_cb_data since user allocated
 * if user references after remove cb, error
 * BEWARE - action cbs usually do not have any itree loc (hin_itp nil) 
 *
 * SJM 02/08/03 - removed need for module since now just turn off dces
 */
extern PLI_INT32 vpi_remove_cb(vpiHandle cb_obj)
{
 i_tev_ndx tevpi;
 struct h_t *hp;
 struct hrec_t *hrp;
 struct cbrec_t *cbp;
 struct tev_t *tevp;

 __last_eip = NULL;
 hp = (struct h_t *) cb_obj;
 if (!__validate_handle("vpi_remove_cb", hp)) return(FALSE);
 hrp = hp->hrec;
 if (hrp->htyp != vpiCallback)
  {
   __vpi_err(1832, vpiError,
    "vpi_remove_cb requires vpiCallback handle - %s illegal",
    __to_vpionam(__wrks1, hrp->htyp));
   return(FALSE);
  }
 cbp = hrp->hu.hcbp;

 switch (cbp->cb_reason) {
  /* value change call backs - removal after some occurred */ 
  case cbValueChange:
   if (__run_state != SS_SIM)
    {
no_sim:
      __sim_notbegun_err("vpi_remove_cb");
      return(FALSE);
    } 
   /* no event here - once dce records gone, can't occur */
   /* this can only run once simulation starts run state is SIM STATE */ 
   if (cbp->cb_gateout) linkout_gateout_cb(cbp); 
   else
    {
     /* SJM 10/06/06 - now vpi looks at head of dce list so */
     /* can always free */
     __free_dceauxlst(cbp->cbdcep, 1);
    }
   free_cbrec(cbp);
   break;
  case cbForce:
   if (__run_state != SS_SIM) goto no_sim;
   if (cbp->cb_hp != NULL) 
    {
     /* SJM 10/06/06 - now vpi looks at head of dce list so */
     /* can always free */
     __free_dceauxlst(cbp->cbdcep, 1);

     free_cbrec(cbp);
     __num_vpi_force_cbs--;
     if (__num_vpi_force_cbs < 0) __vpi_terr(__FILE__, __LINE__);
    }
   else linkout_allcb(cbp, TRUE);
   break;
  case cbRelease:
   if (__run_state != SS_SIM) goto no_sim;
   if (cbp->cb_hp != NULL) 
    {
     /* SJM 10/06/06 - now vpi looks at head of dce list so */
     /* can always free */
     __free_dceauxlst(cbp->cbdcep, 1);

     free_cbrec(cbp);
     __num_vpi_rel_cbs--;
     if (__num_vpi_rel_cbs < 0) __vpi_terr(__FILE__, __LINE__);
    }
   else linkout_allcb(cbp, FALSE);
   break;

  /* call backs only removed before they happen */ 
  /* case 1: always scheduled - part of all events removed at end of slot */ 
  /* RW sync moves from scheduled with vpi reschd on to #0 list so */ 
  case cbAfterDelay: case cbAtStartOfSimTime: case cbReadWriteSynch:
   if (__run_state != SS_SIM) goto no_sim;
   /* when cancelled handle zero and removed from iterator list so must */
   /* never be seen again */
   tevpi = cbp->cbtevpi; 
   tevp = &(__tevtab[tevpi]);
   /* DBG remove --- */  
   if (tevp->te_cancel) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   free_cbrec(cbp);
   tevp->te_cancel = TRUE;
   tevp->tu.tehp = NULL;  
   break;  

  /* if vpi reschd on, scheduled so cancel event */
  /* else remove from action list */
  case cbReadOnlySynch:
   if (__run_state != SS_SIM) goto no_sim;
   tevpi = cbp->cbtevpi; 
   tevp = &(__tevtab[tevpi]);
   /* DBG remove --- */
   if (tevp->te_cancel) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   if (!tevp->vpi_reschd) goto in_nonschd_list;

   free_cbrec(cbp);
   tevp->te_cancel = TRUE;
   tevp->tu.tehp = NULL;  
   break;  
  /* this is treated as an action cb for removal - delay when happens */
  case cbNextSimTime:
   if (__run_state != SS_SIM) goto no_sim;
   goto in_nonschd_list;

  /* action cbs */
  case cbEndOfCompile: case cbStartOfSimulation: case cbEndOfSimulation:
  case cbError: case cbPLIError: case cbTchkViolation:
  case cbStartOfReset: case cbEndOfReset:
  case cbEnterInteractive: case cbExitInteractive:
  case cbInteractiveScopeChange:
  case cbLanguageLine: 
in_nonschd_list:
   /* permanent lists - link out */
   linkout_action_cb(cbp);
   free_cbrec(cbp);
   break;
  /* currently unimplemented */
  case cbStartOfSave: case cbEndOfSave: case cbStartOfRestart:
  case cbEndOfRestart:
  case cbUnresolvedSystf:
   __vpi_err(1823, vpiError, "vpi_remove_cb reason %s unsupported", 
    __cb_reason_to_nam(__wrks1, cbp->cb_reason));
   return(FALSE);
  case cbStmt:
   __vpi_err(1823, vpiError,
    "vpi_remove_cb reason %s unsupported - statement callbacks not yet implemented",
    __cb_reason_to_nam(__wrks1, cbp->cb_reason));
   return(FALSE);
  default:
   __vpi_err(1824, vpiError,
    "vpi_remove_cb illegal reason value %d - no callback removed",
     cbp->cb_reason); 
   return(FALSE);
 }
 return(TRUE);
}

/*
 * find and link out all forces or release handle (cbrec in list)
 *
 * this also frees cbrec and allocated copy of cb_data record 
 */
static void linkout_allcb(struct cbrec_t *cbp, int32 is_force)
{
 register struct rfcblst_t *rfp, *last_rfp; 
 
 last_rfp = NULL;
 if (is_force)
  {
   rfp = __force_allcb_hdr;
   /* DBG remove --- */
   if (__vpi_force_cb_always && rfp == NULL) __vpi_terr(__FILE__, __LINE__);
   if (!__vpi_rel_cb_always && rfp != NULL) __vpi_terr(__FILE__, __LINE__);
   /* --- */  
   for (; rfp != NULL; rfp = rfp->rfcbnxt)
    { if (rfp->rfcbp == cbp) goto got_force_cbp; last_rfp = rfp; }
   __vpi_terr(__FILE__, __LINE__);
got_force_cbp:
   /* case 1: removing from front */
   if (last_rfp == NULL) 
    {
     /* case 1a: only one */ 
     if (rfp->rfcbnxt == NULL)
      {
       __vpi_force_cb_always = FALSE;
       __force_allcb_hdr = __force_allcb_end = NULL;
rem_done:
       free_cbrec(rfp->rfcbp);
       __my_free((char *) rfp, sizeof(struct rfcblst_t)); 
       return;
     }
     /* case 1b: more than one */
     __force_allcb_hdr = rfp->rfcbnxt;
     goto rem_done;
    }
   /* case 2: removing from inside */
   if (rfp->rfcbnxt == NULL)
    {  
     /* case 2a: removing last */
     __force_allcb_end = last_rfp;
     last_rfp->rfcbnxt = NULL; 
     goto rem_done;
    }
   /* case 2a: removing interior */
   last_rfp->rfcbnxt = rfp->rfcbnxt;
   goto rem_done;
  }
 rfp = __rel_allcb_hdr;
 /* DBG remove --- */
 if (__vpi_rel_cb_always && rfp == NULL) __vpi_terr(__FILE__, __LINE__);
 if (!__vpi_rel_cb_always && rfp != NULL) __vpi_terr(__FILE__, __LINE__);
 /* --- */  
 for (; rfp != NULL; rfp = rfp->rfcbnxt)
  { if (rfp->rfcbp == cbp) goto got_rel_cbp; last_rfp = rfp;   }
 __vpi_terr(__FILE__, __LINE__);
got_rel_cbp:
 /* case 1: removing from front */
 if (last_rfp == NULL) 
  {
   /* case 1a: only one */ 
   if (rfp->rfcbnxt == NULL)
    {
     __vpi_rel_cb_always = FALSE;
     __rel_allcb_hdr = __rel_allcb_end = NULL;
     goto rem_done;
    }
   /* case 1b: more than one */
   __rel_allcb_hdr = rfp->rfcbnxt;
   goto rem_done;
  }
 /* case 2: removing from inside */
 if (rfp->rfcbnxt == NULL)
  {  
   /* case 2a: removing last */
   __rel_allcb_end = last_rfp;
   last_rfp->rfcbnxt = NULL; 
   goto rem_done;
  }
 /* case 2a: removing interior */
 last_rfp->rfcbnxt = rfp->rfcbnxt;
 goto rem_done;
}

/*
 * link out prim output term 
 *
 * although using tev, not part of scheduling mechanism just in list
 */
static void linkout_gateout_cb(struct cbrec_t *cbp) 
{
 register i_tev_ndx tevpi, last_tevpi;
 int32 gi, ii;
 struct h_t *ghp;
 struct gate_t *gp;
 struct mod_t *mdp;

 ghp = cbp->cb_hp;
 mdp = ghp->hin_itp->itip->imsym->el.emdp;
 ii = ghp->hin_itp->itinum;
 gp = ghp->hrec->hu.hgp;
 gi = gp - mdp->mgates;
 if (mdp->mgateout_cbs == NULL || mdp->mgateout_cbs[gi] == NULL
  || mdp->mgateout_cbs[gi][ii] == -1)
  {  
   __vpi_err(1826, vpiError,
    "vpi_remove_cb of primitive terminal output value change type failed - cb handle already removed?");
   return;
  }  
 last_tevpi = -1;
 tevpi = mdp->mgateout_cbs[gi][ii];
 for (; tevpi != -1; tevpi = __tevtab[tevpi].tenxti)  
  {
   /* find right one to remove and one before it if not front */
   if (tevpi != cbp->cbtevpi) { last_tevpi = tevpi; continue; } 

   /* one to remove on front */
   if (last_tevpi == -1) 
    {
     /* just one to remove */
     if (__tevtab[tevpi].tenxti == -1) mdp->mgateout_cbs[gi][ii] = -1;
     /* link out first te end remains same */
     else mdp->mgateout_cbs[gi][ii] = __tevtab[tevpi].tenxti;
    }
   else
    {
     /* at end */
     if (__tevtab[tevpi].tenxti == -1) __tevtab[last_tevpi].tenxti = -1;
     /* te end remains same */
     else __tevtab[last_tevpi].tenxti = __tevtab[tevpi].tenxti;
    }
   /* link on front of free list - pseudo event so nothing else to do */
   __tevtab[tevpi].tenxti = __tefreelsti;
   __tefreelsti = tevpi;
   return;
  }
 __vpi_terr(__FILE__, __LINE__);
}

/*
 * link out action call back tevp from an action list
 *
 * although using tev, not part of scheduling mechanism just in list
 */
static void linkout_action_cb(struct cbrec_t *cbp) 
{
 register i_tev_ndx tevpi, last_tevpi;

 if ((tevpi = __vpicb_tehdri[cbp->cb_reason]) == -1)
  {  
   __vpi_err(1826, vpiError,
    "vpi_remove_cb of action type %s failed - cb handle already removed?",
    to_cbtypnam(__wrks1, cbp->cb_reason));
   return;
  }  
 last_tevpi = -1;
 for (; tevpi != -1; tevpi = __tevtab[tevpi].tenxti)  
  {
   /* find right one to remove and one before it if not front */
   if (tevpi != cbp->cbtevpi) { last_tevpi = tevpi; continue; } 

   /* one to remove on front */
   if (last_tevpi == -1) 
    {
     /* just one to remove */
     if (__tevtab[tevpi].tenxti == -1)
      {
       /* turn off special compile error callback registered flag */
       if (__vpicb_tehdri[cbp->cb_reason] == cbError)
        __vpierr_cb_active = FALSE;
     
       /* then remove the action call back */
       __vpicb_tehdri[cbp->cb_reason] = __vpicb_teendi[cbp->cb_reason] = -1; 
      }
     /* link out first te end remains same */
     else __vpicb_tehdri[cbp->cb_reason] = __tevtab[tevpi].tenxti;
    }
   else
    {
     /* at end */
     if (__tevtab[tevpi].tenxti == -1)
      {
       __tevtab[last_tevpi].tenxti = -1;
       __vpicb_teendi[cbp->cb_reason] = last_tevpi;
      }
     /* te end remains same */
     else __tevtab[last_tevpi].tenxti = __tevtab[tevpi].tenxti;
    }
   /* link on front of free list - pseudo event so nothing else to do */
   __tevtab[tevpi].tenxti = __tefreelsti;
   __tefreelsti = tevpi;
   return;
  }
 __vpi_terr(__FILE__, __LINE__);
}

/*
 * convert a call back value to its name
 */
static char *to_cbtypnam(char *s, int32 reason)
{
 switch (reason) {
  case cbValueChange: strcpy(s, "cbValueChange"); break;
  case cbStmt: strcpy(s, "cbStmt"); break;
  case cbForce: strcpy(s, "cbForce"); break;
  case cbRelease: strcpy(s, "cbRelease"); break;
  case cbAtStartOfSimTime: strcpy(s, "cbAtStartOfSimTime"); break;
  case cbReadWriteSynch: strcpy(s, "cbReadWriteSynch"); break;
  case cbReadOnlySynch: strcpy(s, "cbReadOnlySynch"); break;
  case cbNextSimTime: strcpy(s, "cbNextSimTime"); break;
  case cbAfterDelay: strcpy(s, "cbAfterDelay"); break;
  case cbEndOfCompile: strcpy(s, "cbEndOfCompile"); break;
  case cbStartOfSimulation: strcpy(s, "cbStartOfSimulation"); break;
  case cbEndOfSimulation: strcpy(s, "cbEndOfSimulation"); break;
  case cbError: strcpy(s, "cbError"); break;
  case cbTchkViolation: strcpy(s, "cbTchkViolation"); break;
  case cbStartOfSave: strcpy(s, "cbStartOfSave"); break;
  case cbEndOfSave: strcpy(s, "cbEndOfSave"); break;
  case cbStartOfRestart: strcpy(s, "cbStartOfRestart"); break;
  case cbEndOfRestart: strcpy(s, "cbEndOfRestart"); break;
  case cbStartOfReset: strcpy(s, "cbStartOfReset"); break;
  case cbEndOfReset: strcpy(s, "cbEndOfReset"); break;
  case cbEnterInteractive: strcpy(s, "cbEnterInteractive"); break;
  case cbExitInteractive: strcpy(s, "cbExitInteractive"); break;
  case cbInteractiveScopeChange: strcpy(s, "cbInteractiveScopeChange"); break;
  case cbUnresolvedSystf: strcpy(s, "cbUnresolvedSystf"); break;
  default: strcpy(s, "**none**");
 }
 return(s);
}

/*
 * get information about a call back
 *
 * LOOKATME - standard should return ptr not copy - copy guts for now
 */
extern void vpi_get_cb_info(vpiHandle object, p_cb_data cb_data_p)
{  
 struct h_t *hp;
 struct hrec_t *hrp;
 struct cbrec_t *cbp;

 hp = (struct h_t *) object;
 if (!__validate_handle("vpi_get_cb_info", hp)) return;
 hrp = hp->hrec;
 if (hrp->htyp != vpiCallback)
  {
   __vpi_err(1825, vpiError,
    "vpiget_cb_info requires vpiCallback object argument - %s illegal",
    __to_vpionam(__wrks1, hrp->htyp));
   return;
  }
 cbp = hrp->hu.hcbp;
 cb_data_p->reason = cbp->cb_reason;
 cb_data_p->cb_rtn = cbp->cb_rtn;
 cb_data_p->obj = (vpiHandle) cbp->cb_hp; 
 if (cb_data_p->time != NULL) cb_data_p->time->type = (int32) cbp->cb_rettimtyp;
 if (cb_data_p->value != NULL)
  cb_data_p->value->format = (int32) cbp->cb_retvalfmt;
 cb_data_p->index = 0;
 cb_data_p->user_data = cbp->cb_user_data;
}

/*
 * ROUTINES FOR TRAVERSING OBJECTS
 */

/*
 * get a 1-to-1 object handle given a reference handle and 1-to-1 access type
 *
 * for Cver only declarative structure and variables accessible and know
 * handle will be of supported object type
 * 
 * ALGORITHM:  use destination property (or tag) as first level index
 * then for each destination use reference handle (object) to
 * as starting point for which to select the destination property from 
 */
extern vpiHandle vpi_handle(PLI_INT32 type, vpiHandle referenceHandle)
{
 register struct h_t *rhp;
 register struct hrec_t *rhrp;
 int32 sf_ind;
 word32 sttyp, primtyp;
 struct sy_t *syp;
 struct st_t *stp;
 struct expr_t *xp;
 struct itree_t *itp;
 struct net_t *np;

 /* must reset at start of call to no error */
 __last_eip = NULL;
 if (__run_state == SS_COMP)
  { __still_comp_err("vpi_handle"); return(NULL); } 
 if (!__validate_accessm("vpi_handle", type, "1-to-1")) return(NULL);
 rhp = (struct h_t *) referenceHandle;
 /* handle must be nil can not validate */ 
 if (type == vpiSysTfCall) return(get_cursystfcall(rhp));
 if (!__validate_handle("vpi_handle", rhp)) return(NULL);
 rhrp = rhp->hrec;

 switch (type) { 
  /* get an object of type module (itree instance) that contains handle */ 
  case vpiModule:
   return(get_inmod_itp(rhp));
  case vpiIndex: return(get_obj_index(rhp));
  case vpiLeftRange: return(get_obj_range(rhp, type));
  case vpiRightRange: return(get_obj_range(rhp, type));
  /* only selects have parents - vpiScope or vpiModule for containing scope */
  case vpiParent: return(get_obj_parent(rhp));
  case vpiRhs: return(get_obj_side(rhp, type));
  case vpiLhs: return(get_obj_side(rhp, type));
  case vpiLowConn:
   /* port bit high conn is variable or var bit not expr */
   if (rhrp->htyp == vpiPortBit) return(getbit_lowconn(rhp));
   /* lowconn of port is module port expression */
   if (rhrp->htyp == vpiPort) return(getexpr_lowconn(rhp));
   goto bad_args;
  case vpiHighConn:
   /* port bit high conn is variable or var bit not expr */
   if (rhrp->htyp == vpiPortBit) return(getbit_highconn(rhp));
   /* highconn of port is expression */
   if (rhrp->htyp == vpiPort) return(getexpr_highconn(rhp));
   goto bad_args;
  case vpiTask:
   if (rhrp->htyp != vpiTaskCall) goto bad_args;
   syp = rhrp->hu.hstp->st.stkc.tsksyx->lu.x->lu.sy;
   /* this is task definition handle - tsk in not set */
   /* know task def. at top level */
   return(__mk_handle(vpiTask, (void *) syp->el.etskp, rhp->hin_itp, NULL));
  case vpiFunction:
   if (rhrp->htyp != vpiFuncCall) goto bad_args;
   syp = rhrp->hu.hxp->lu.x->lu.sy;
   return(__mk_handle(vpiFunction, (void *) syp->el.etskp, rhp->hin_itp, 
    NULL));
  case vpiUserSystf:
   if (rhrp->htyp == vpiSysFuncCall)
    {
     syp = rhrp->hu.hxp->lu.x->lu.sy;  
do_usersystf:
     sf_ind = syp->el.esyftbp->syfnum;
     if (sf_ind <= __last_veriusertf || sf_ind > __last_systf)
      {
       __vpi_err(1829, vpiError,
        "%s (of %s) handle no vpiUserSystf handle - not registered vpi_",
        __to_vpionam(__wrks1, rhp->hrec->htyp), syp->synam);
       return(NULL);
      }
     return(__mk_handle(vpiUserSystf, (void *) sf_ind, NULL, NULL));
    }
   if (rhrp->htyp == vpiSysTaskCall)
    {
     syp = rhrp->hu.hstp->st.stkc.tsksyx->lu.x->lu.sy;
     goto do_usersystf;
    }
   goto bad_args;
  /* notifier returns reg handle, others tchk term */
  case vpiTchkRefTerm: case vpiTchkDataTerm: case vpiTchkNotifier: 
   return(get_tchk_term((word32) type, rhp));
  case vpiTchk:
   /* LOOKATME - LRM says no connection back from notifier */
   if (rhrp->htyp != vpiTchkRefTerm && rhrp->htyp != vpiTchkDataTerm
    && rhrp->htyp != vpiTchkNotifier) goto bad_args;
   return(__mk_handle(vpiTchk, (void *) rhrp->hu.htcp, rhp->hin_itp, NULL));
  case vpiModPath:
   if (rhrp->htyp != vpiPathTerm) goto bad_args;
   return(__mk_handle(vpiModPath, (void *) rhrp->hu.hpthp, rhp->hin_itp,
    NULL));
  case vpiPrimitive:
   /* handle from primitive term to primitive it is part of */
   if (rhrp->htyp != vpiPrimTerm) goto bad_args;
   primtyp = __gate_to_vpiprimtyp(rhrp->hu.hgp);
   return(__mk_handle(primtyp, (void *) rhrp->hu.hgp, rhp->hin_itp, NULL));
  case vpiCondition: return(get_cond(rhp));
  case vpiElseStmt:
   if (rhrp->htyp != vpiIfElse) goto bad_args;
   stp = rhrp->hu.hstp->st.sif.elsest;
   if (stp->st_unbhead) sttyp = vpiBegin;
   else sttyp = __to_vpi_stmttyp(&stp);
   return(__mk_stmt_handle(sttyp, stp, rhp->hin_itp, rhrp->hin_tskp));
  case vpiExpr:
   return(bld_1to1_exprclass_handle(rhp));
  case vpiNamedEvent:
   if (rhrp->htyp != vpiEventStmt) goto bad_args;
   stp = rhrp->hu.hstp;
   xp = stp->st.scausx;
   if (xp->optyp == GLBREF)
    {
     __push_itstk(rhp->hin_itp);
     __xmrpush_refgrp_to_targ(xp->ru.grp);
     itp = __inst_ptr;
     __pop_itstk();
     __pop_itstk();
    }
   else if (xp->optyp == ID) itp = rhp->hin_itp;
   else { __vpi_terr(__FILE__, __LINE__); return(NULL); }
   np = xp->lu.sy->el.enp;
   return(__mk_handle(vpiNamedEvent, (void *) np, itp, rhrp->hin_tskp));
  /* scope handle offrom disable statement */
  case vpiScope: return(get_obj_scope(rhp));
  case vpiStmt: return(get_contained_stmt(rhp));
  case vpiDelayControl: case vpiRepeatControl:
   return(get_dctrl_stmt(rhp, type));
  case vpiEventControl:
   /* repeat dctrl form does not have contained stmt only contained ev ctrl */
   if (rhrp->htyp == vpiRepeatControl)
    {
     /* actual h_u object is the S DELCTRL - only difference between */
     /* repeat ev cntrl and normal ev cntrl is type */
     return(__mk_handle(vpiEventControl, (void *) rhrp->hu.hstp, rhp->hin_itp,
      rhrp->hin_tskp));
    }
   return(get_dctrl_stmt(rhp, type));
  case vpiForInitStmt:  
   if (rhrp->htyp != vpiFor) goto bad_args;
   stp = rhrp->hu.hstp->st.sfor->forassgn;
   /* this must really return for init part (special prefix now to for) */
   /* so cannot call to vpi stmttyp */
   /* DBG remove -- */
   if (stp->stmttyp != S_FORASSGN) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   return(__mk_handle(vpiAssignment, (void *) stp, rhp->hin_itp,
    rhrp->hin_tskp));
  case vpiForIncStmt:
   if (rhrp->htyp != vpiFor) goto bad_args;
   stp = rhrp->hu.hstp->st.sfor->forinc;
   sttyp = __to_vpi_stmttyp(&stp);
   return(__mk_stmt_handle(sttyp, stp, rhp->hin_itp, rhrp->hin_tskp));
  case vpiUdpDefn:
   return(get_udpdef_from_inobj(rhp));
  case vpiInitial:
   /* udp initial statement - containing is udp define */
   if (rhrp->htyp != vpiUdpDefn) goto bad_args;
   return(get_contained_udp_init(rhp));
  case vpiPoundParam:
   return(get_up_poundparam_expr(rhp));
  default:
bad_args:
   __vpi_err(1827, vpiError,
    "there is no 1-to-1 relationships of type %s from reference handle %s", 
    __to_vpionam(__wrks1, (word32) type), __to_vpionam(__wrks2, rhrp->htyp));
 }
 return(NULL);
}

/*
 * access handle for multi-dimensional array - mult dim arrays not supported
 */
extern vpiHandle vpi_handle_multi(PLI_INT32 type, vpiHandle refHandle1,
 vpiHandle refHandle2, ...)
{
 __vpi_err(1801, vpiError, "vpi_handle_multi routine currently unsupported"); 
 return(NULL);
}

/*
 * make a statement handle
 *
 * handles name block (fj or begin) except where contents is task not stp
 */
extern vpiHandle __mk_stmt_handle(word32 sttyp, struct st_t *stp,
 struct itree_t *itp, struct task_t *tskp)
{
 if (sttyp == vpiNamedBegin || sttyp == vpiNamedFork)
  {
   return(__mk_handle(sttyp, (void *) stp->st.snbtsk, itp, tskp));
  }
 return(__mk_handle(sttyp, (void *) stp, itp, tskp));
} 

/*
 * get current user systf call handle that can be passed to get systf info
 * 
 * this must be sys tf call with itree and task loc. but LRM unclear
 * this can only be called from inside called vpi sys task or func.
 */
static vpiHandle get_cursystfcall(struct h_t *hp)
{
 vpiHandle ihref;
 struct task_t *tskp;

 if (hp != NULL)
  {
   __vpi_err(1833, vpiError,
    "vpi_handle with 1-to-1 vpiSysTfCall NULL handle required - %s illegal",
    __to_vpionam(__wrks1, hp->hrec->htyp));
   return(NULL);
  }
 /* DBG remove --- */  
 if (__cur_sysf_expr == NULL && __cur_syst_stp == NULL)
  {
   __vpi_err(1840, vpiError,
    "vpi_handle with vpiSysTfCall not called from inside vpi_ systf callback");
   return(NULL);
  }
 /* --- */
 /* DBG remove --- */
 if (__cur_sysf_expr != NULL && __cur_syst_stp != NULL)
  __vpi_terr(__FILE__, __LINE__);
 /* --- */
 tskp = __getcur_scope_tsk();
 if (__cur_sysf_expr != NULL)
  ihref = __mk_handle(vpiSysFuncCall, (void *) __cur_sysf_expr, __inst_ptr,
   tskp);
 else ihref = __mk_handle(vpiSysTaskCall, (void *) __cur_syst_stp, __inst_ptr,
  tskp);
 return(ihref);
}

/*
 * for object in module, get enclosing module (actually itree inst)
 *
 * non declarative object types will never be handle types
 */
static vpiHandle get_inmod_itp(struct h_t *hp)
{
 struct itree_t *itp;

 switch (hp->hrec->htyp) {
  case vpiModule:
   if ((itp = hp->hin_itp->up_it) == NULL) return(NULL);  
   return(__mk_handle(vpiModule, (void *) itp->itip->imsym->el.emdp, itp,
    NULL));
   /* even if inner scope (in nested named blocks?), still itree inst. */
   /* scope is symbol table so top mod scope goes to itree inst (mod) */
   /* SJM 11/13/00 - forgot vpiReg - now added and checked new LRM */ 
  case vpiPort: case vpiNet: case vpiReg: case vpiIntegerVar: case vpiTimeVar:
  case vpiRealVar: case vpiMemory: case vpiNamedEvent: case vpiContAssign:
  case vpiGate: case vpiSwitch: case vpiUdp: case vpiModPath: case vpiTchk:
  case vpiSpecParam: case vpiDefParam:
  case vpiIODecl:
   break;
  case vpiParameter: case vpiParamAssign:
   break;

  /* can get containing module (inst) for any of 4 scope objects */ 
  case vpiTask: case vpiFunction: case vpiNamedBegin: case vpiNamedFork:
   break;
  case vpiInitial: case vpiAlways:
   break;
  default: no_1to1h_err(vpiModule, hp); return(NULL);
 }
 /* down handle will always have itree loc. */
 return(__mk_handle(vpiModule, (void *) hp->hin_itp->itip->imsym->el.emdp,
  hp->hin_itp, NULL));
}

/*
 * build a handle from malloced array - user must handle freeing
 * this should probably be macro
 */
extern vpiHandle __mk_handle(word32 typ, void *h_unp, struct itree_t *itp,
 struct task_t *tskp) 
{
 register int32 hi;
 register struct h_t *hp, *hparr;
 register struct hrec_t *hrp, *hrarr;

 /* if free list empty - add a few thousand */
 /* separate malloc calls waste 16 bytes per call for small record */
 if (__vpi_hfree_hdr == NULL)
  {
   hparr = (struct h_t *) __my_malloc(VPI_OBJALLOCNUM*sizeof(struct h_t));
   for (hi = 0; hi < VPI_OBJALLOCNUM - 1; hi++)
    {
     hparr[hi].hin_itp = (struct itree_t *) &(hparr[hi + 1]); 
    }
   hparr[VPI_OBJALLOCNUM - 1].hin_itp = NULL;
   __vpi_hfree_hdr = &(hparr[0]);
  }
 if (__vpi_hrecfree_hdr == NULL)
  {
   hrarr = (struct hrec_t *)
    __my_malloc(VPI_OBJALLOCNUM*sizeof(struct hrec_t));
   for (hi = 0; hi < VPI_OBJALLOCNUM - 1; hi++)
    {
     hrarr[hi].hu.hfreenxt = &(hrarr[hi + 1]); 
    }
   hrarr[VPI_OBJALLOCNUM - 1].hu.hfreenxt = NULL;
   __vpi_hrecfree_hdr = &(hrarr[0]);
  }

 /* know at least one always on each free list by here */
 hrp = __vpi_hrecfree_hdr;
 __vpi_hrecfree_hdr = hrp->hu.hfreenxt;

 hp = __vpi_hfree_hdr;
 __vpi_hfree_hdr = (struct h_t *) hp->hin_itp;

 __init_hrec(hrp);
 hrp->htyp = typ;
 hrp->hu.hanyp = h_unp;
 hrp->hin_tskp = tskp;

 hp->hin_itp = itp;
 hp->hrec = hrp;

 return((vpiHandle) hp);
}

/*
 * initialize a cver internal handle - zeroing handle turns off bits
 *
 * BEWARE - because zeroing must always define set bits to init to F (0)
 */
extern void __init_hrec(struct hrec_t *hrp)
{
 memset(hrp, 0, sizeof(struct hrec_t));
 hrp->h_magic = PVH_MAGIC;
 hrp->hi = -1;
}

/*
 * build an index expression handle for an object 
 *
 * this always blds vpiConst expr. handle - i.e. evaluates expression 
 *
 * for bith ndx forms (from per bit iterator) must convert to const and norm
 */
static vpiHandle get_obj_index(struct h_t *hp)
{
 register word32 av;
 register struct hrec_t *hrp;
 vpiHandle ihref;
 struct h_t *rhp;

 hrp = hp->hrec;
 switch (hrp->htyp) {
  case vpiMemoryWord: case vpiNetBit: case vpiRegBit: case vpiVarSelect:
   /* case 1, alternate form for expr. handle is fixed index */
   if (hrp->bith_ndx)
    {
     /* index hi is fixed index - know in range */
     av = (word32) __unnormalize_ndx(hrp->hu.hnp, hrp->hi);
     /* this is expr. separate from bsel so must be unnormalized value */
     /* because not connected to bsel constant can be unnormalized */
bld_const_obj:
     ihref = __mk_handle(vpiConstant, NULL, hp->hin_itp, hrp->hin_tskp);
     rhp = (struct h_t *) ihref;
     rhp->hrec->hu.hxp = __bld_rng_numxpr(av, 0L, WBITS);
     rhp->hrec->free_xpr = TRUE;
     break;
    }
   /* expr form case */
   /* DBG remove --- */
   if (hrp->hu.hxp->optyp != LSB) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   ihref = __mk_exprclass_handle(hrp->hu.hxp->ru.x, hp->hin_itp,
    hrp->hin_tskp);
   break;
  case vpiPortBit:
   /* no ned to normalize since port bits virtual range always h:0 */
   av = (word32) hrp->hi;
   goto bld_const_obj;
  default: no_1to1h_err(vpiIndex, hp); return(NULL);
 }
 return(ihref);
}

/*
 * build an index expression handle for an object 
 */
static vpiHandle get_obj_range(struct h_t *hp, int32 lrtyp)
{
 register struct hrec_t *hrp;
 int32 r1, r2, wid;
 vpiHandle ihref;
 word32 rngv;
 struct h_t *rhp;
 struct expr_t *xp;
 struct task_t *tskp;
 struct net_t *np;

 rngv = 0;
 hrp = hp->hrec;
 switch (hrp->htyp) {
  case vpiNet: case vpiReg:
   if (!hrp->hu.hnp->n_isavec) return(NULL);
   goto get_rng;
  case vpiIntegerVar: case vpiTimeVar:
get_rng:
   __getwir_range(hrp->hu.hnp, &r1, &r2);
   if (lrtyp == vpiLeftRange) rngv = (word32) r1; else rngv = (word32) r2;
set_val:
   ihref = __mk_handle(vpiConstant, NULL, hp->hin_itp, hrp->hin_tskp);
   rhp = (struct h_t *) ihref;
   rhp->hrec->hu.hxp = __bld_rng_numxpr(rngv, 0L, WBITS);
   rhp->hrec->free_xpr = TRUE;
   break;
  case vpiMemoryWord:
   if (hrp->bith_ndx) np = hrp->hu.hnp;
   else
    {
     xp = hrp->hu.hxp; 
     /* DBG remove -- */
     if (xp->optyp != LSB) __vpi_terr(__FILE__, __LINE__);
     /* --- */
     np = xp->lu.x->lu.sy->el.enp; 
    }
   if (!np->n_isavec || np->ntyp == N_REAL) return(NULL);
   __getwir_range(np, &r1, &r2);
   if (lrtyp == vpiLeftRange) rngv = (word32) r1; else rngv = (word32) r2;
   goto set_val;
  case vpiFunction:
   /* this if function declaration - only reg ret. funcs have range */
   tskp = hrp->hu.htskp;
   np = tskp->tskpins->tpsy->el.enp;
   if (!np->n_isavec) return(NULL);
   __getwir_range(np, &r1, &r2);
   if (lrtyp == vpiLeftRange) rngv = (word32) r1; else rngv = (word32) r2;
   goto set_val;
  case vpiMemory: 
   __getarr_range(hrp->hu.hnp, &r1, &r2, &wid);
   if (lrtyp == vpiLeftRange) rngv = (word32) r1; else rngv = (word32) r2;
   goto set_val;
  /* expr that is part select has ranges */
  case vpiPartSelect:
   xp = hrp->hu.hxp;
   /* know range can never be x or will not get here */

   if (lrtyp == vpiLeftRange) rngv = __contab[xp->ru.x->lu.x->ru.xvi];
   else rngv = __contab[xp->ru.x->ru.x->ru.xvi];
   goto set_val;
  case vpiIODecl:
   if (hrp->htyp2 == vpiUdpDefn)
    {
     /* udp io decls always scalars so no ranges */
     return(NULL);
    } 
   np = hrp->hu.hnp; 
   if (!np->n_isavec) return(NULL);
   goto get_rng;
  default:
   no_1to1h_err(lrtyp, hp);
   return(NULL);
 }
 return(ihref);
}

/*
 * get parent (containing) handle for bit of object
 *
 * also gets object part select is select of
 * here for non index form of these must map to itree loc of variable
 * also know never see bith index form for xmr
 *
 * tricky because for index handles must map to dest. for xmrs
 */
static vpiHandle get_obj_parent(struct h_t *hp)
{
 register struct hrec_t *hrp;
 word32 hotyp;
 vpiHandle ihref;
 struct expr_t *idndp;
 struct net_t *np;
 struct itree_t *nitp;
 struct task_t *ntskp;
 struct h_t *hp2;

 ihref = NULL;
 hrp = hp->hrec;
 switch (hrp->htyp) {
  case vpiMemoryWord:
   if (hrp->bith_ndx)
    {
     ihref = __mk_handle(vpiMemory, (void *) hrp->hu.hnp, hp->hin_itp,
      hrp->hin_tskp);
     break;
    }
   hotyp = vpiMemory; 
bld_par_handle:
   idndp = hrp->hu.hxp->lu.x; 
   /* expr. form - for xmr must map to right itree and task loc. */
   exprobj_to_itreeloc(&nitp, &ntskp, idndp, hp->hin_itp, hrp->hin_tskp);
   ihref = __mk_handle(hotyp, (void *) idndp->lu.sy->el.enp, nitp, ntskp);
   break;
  case vpiNetBit:
   if (hrp->bith_ndx)
    {
     ihref = __mk_handle(vpiNet, (void *) hrp->hu.hnp, hp->hin_itp,
      hrp->hin_tskp);
     break;
    }
   hotyp = vpiNet;
   goto bld_par_handle;
  case vpiRegBit: case vpiVarSelect:
   if (hrp->bith_ndx)
    {
     hotyp = __ntyp_to_vpivarhtyp(hrp->hu.hnp);
     ihref = __mk_handle(hotyp, (void *) hrp->hu.hnp, hp->hin_itp,
      hrp->hin_tskp);
     break;
    }
   hotyp = __ntyp_to_vpivarhtyp(hrp->hu.hxp->lu.x->lu.sy->el.enp);
   goto bld_par_handle;
  case vpiPortBit:
   /* know in_tskp will be nil */
   ihref = __mk_handle(vpiPort, (void *) hrp->hu.hpi, hp->hin_itp,
    hrp->hin_tskp);
   break;
  case vpiPartSelect:
   np = hrp->hu.hxp->lu.x->lu.sy->el.enp;
   hotyp = __ntyp_to_vpivarhtyp(np);
   goto bld_par_handle; 
  case vpiNetDriver:
   np = hrp->hu.hnpp->elnpp.enp;  
   ihref = __mk_handle(vpiNet, (void *) np, hp->hin_itp, hrp->hin_tskp); 
   break;
  case vpiPrimTerm:
   /* parent of primitive (gate) terminal is primitive */
   hotyp = __gate_to_vpiprimtyp(hrp->hu.hgp);
   ihref = __mk_handle(hotyp, (void *) hrp->hu.hgp, hp->hin_itp, NULL);
   break;
  case vpiNetBitDriver:
   np = hrp->hu.hnpp->elnpp.enp;
   ihref = __mk_handle(vpiNetBit, (void *) np, hp->hin_itp, hrp->hin_tskp); 
   hp2 = (struct h_t *) ihref; 
   /* DBG remove --- */
   if (hrp->hu.hnpp->npaux == NULL) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   /* need internal h:0 bit form here */
   hp2->hrec->hi = hrp->hu.hnpp->npaux->nbi1;
   break;
  case vpiAttribute:
   return(__get_digattr_parent(hp));
   
  default: no_1to1h_err(vpiParent, hp); return(NULL);
 }
 return(ihref);
}

/*
 * get expr. handle variable itree and task location
 *
 * for selects passed the left variable expr. node
 */
static void exprobj_to_itreeloc(struct itree_t **itpp, struct task_t **tskpp, 
 struct expr_t *idndp, struct itree_t *itp, struct task_t *tskp)
{
 struct mod_t *mdp;
 struct gref_t *grp;

 mdp = itp->itip->imsym->el.emdp;
 if (idndp->optyp == ID) 
  {
   if (idndp->locqualnam)
    *tskpp = __find_qualnam_task(idndp->ru.qnchp, mdp, tskp);
   else *tskpp = tskp;
   *itpp = itp;
  }
 else if (idndp->optyp == GLBREF)
  {
   grp = idndp->ru.grp;
   __push_itstk(itp);
   __xmrpush_refgrp_to_targ(grp);
   itp = __inst_ptr;
   __pop_itstk();
   __pop_itstk();
   tskp = grp->targtskp;
  }
 else __vpi_terr(__FILE__, __LINE__);
}

/*
 * get all 1-to-1 relationships that are on a vpi_ rhs or lhs
 */
static vpiHandle get_obj_side(struct h_t *rhp, int32 type)
{
 register struct hrec_t *rhrp;
 int32 hotyp;
 vpiHandle ihref;
 struct expr_t *xp;
 struct st_t *stp;
 struct task_t *tskp;
 struct dfparam_t *dfpp;
 struct itree_t *itp;
 struct net_t *np;

 tskp = NULL;
 rhrp = rhp->hrec;
 switch (rhrp->htyp) { 
  case vpiContAssign:
   if (rhrp->htyp2 == vpiGate)
    {
     if (type == vpiRhs) xp = rhrp->hu.hgp->gpins[1]; 
     else xp = rhrp->hu.hgp->gpins[0];
    }
   else
    {
     if (type == vpiRhs) xp = rhrp->hu.hcap->rhsx; 
     else xp = rhrp->hu.hcap->lhsx;
    }
   break;
  case vpiAssignment:
   stp = rhrp->hu.hstp;
   /* rhs delay or event control has RHS del/evnt control as stmt */
   if (stp->stmttyp == S_DELCTRL) stp = stp->st.sdc->actionst; 
   if (type == vpiRhs) xp = stp->st.spra.rhsx; 
   else xp = stp->st.spra.lhsx; 
   tskp = rhrp->hin_tskp;
   break;
  /* qc assign or force - do not need to distinguish here */
  case vpiAssignStmt: case vpiForce:
   stp = rhrp->hu.hstp;
   if (type == vpiRhs) xp = stp->st.sqca->qcrhsx; 
   else xp = stp->st.sqca->qclhsx; 
   tskp = rhrp->hin_tskp;
   break;
 case vpiDeassign: case vpiRelease:
   stp = rhrp->hu.hstp;
   if (type == vpiLhs) xp = stp->st.sqcdea.qcdalhs; 
   else { no_1to1h_err(type, rhp); return(NULL); }
   tskp = rhrp->hin_tskp;
   break;
  case vpiDefParam:
   dfpp = rhrp->hu.hdfp;
   /* already converted to rooted in exactly one itree */ 
   if (type == vpiLhs) 
    {
     /* left hand side is parameter (i.e. net) and can be task param */
     np = dfpp->targsyp->el.enp;   
     /* SJM - 05/26/05 - must search for bottom - splitting changes */
     itp = __find_dfpbot_itp(dfpp);
     ihref = __mk_handle(vpiParameter, (void *) np, itp, dfpp->dfptskp);
     return(ihref);  
    }
   /* SJM 01/27/04 - this no longer needs to be const for dependent dfps */ 
   xp = dfpp->dfpxrhs;
   /* rhs is constant that must appear in source at module level */
   break;
  case vpiParamAssign:
   /* because know assign (decl.) address - rally same as vpi param object */
   np = rhrp->hu.hnp;
   if (type == vpiLhs)
    {
     hotyp = vpiParameter;
     /* i.e. same thing except type different */
     ihref = __mk_handle(hotyp, (void *) np, rhp->hin_itp, rhrp->hin_tskp);
     return(ihref);
    }
   /* rhs is param expr. - to get value need to get value of lhs */
   /* DBG remove -- */
   if (np->nrngrep != NX_CT) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   if (np->nu.ct->parm_srep == SR_PXPR) xp = np->nu.ct->n_dels_u.d1x;
   else if (np->nu.ct->parm_srep == SR_PISXPR)
    {
     /* d4x used but in fact allocated size is number of insts */
     xp = np->nu.ct->n_dels_u.d4x[rhp->hin_itp->itinum];
    }
   else { __vpi_terr(__FILE__, __LINE__); return(NULL); }
   ihref = __mk_exprclass_handle(xp, rhp->hin_itp, rhrp->hin_tskp);
   return(ihref);
  default: no_1to1h_err(type, rhp); return(NULL);
 }
 ihref = __mk_exprclass_handle(xp, rhp->hin_itp, tskp);
 return(ihref);
}

/*
 * build a scope (mod or task) parent handle (up one symbol table)
 *
 * for named blocks and name fork-join, hu field is statement for
 * real tasks (task and function definitions hu field is tskp
 * but in head case other handle information right
 */
static vpiHandle bld_scope_par(struct h_t *hp, struct task_t *tskp)
{
 word32 parhtyp;
 vpiHandle ihref;
 struct task_t *up_tskp;
 struct symtab_t *sytp, *sytp2;

 sytp = tskp->tsksymtab;
 if ((sytp2 = sytp->sytpar) == NULL) __vpi_terr(__FILE__, __LINE__);
 if (sytp2->sypofsyt->sytyp == SYM_M)
  {
   ihref = __mk_handle(vpiModule, (void *) sytp->sytpar->sypofsyt->el.emdp,
    hp->hin_itp, tskp);
   return(ihref); 
  }
 if (sytp2->sytpar == NULL || sytp2->sytpar->sypofsyt->sytyp == SYM_M)
  up_tskp = NULL;
 else up_tskp = sytp2->sytpar->sypofsyt->el.etskp;

 parhtyp = __to_vpi_tasktyp(tskp->tsktyp);
 ihref = __mk_handle(parhtyp, (void *) sytp2->sypofsyt->el.etskp,
  hp->hin_itp, up_tskp);
 return(ihref);
}      

/*
 * convert from v.h task type to handle constant
 */
extern word32 __to_vpi_tasktyp(word32 tsktyp)
{
 switch (tsktyp) {
  case TASK: return(vpiTask);
  case FUNCTION: return(vpiFunction);
  case Begin: return(vpiNamedBegin);
  case FORK: return(vpiNamedFork);
  default: __vpi_terr(__FILE__, __LINE__);
 }
 return(0);
}

/*
 * build an net/reg bit handle from a port for the low connection (mod port)
 *
 * this build a reg/net bit (or reg/net if scalar) handle from expression
 * use with vpiPort to get entire expression (then can convert if needed) 
 * this is easy case because no xmrs or qualified names
 *
 * hp is port bit handle that must be lvalue
 *
 * here unc. impossible because connections determine width
 */
static vpiHandle getbit_lowconn(struct h_t *hp)
{
 int32 ndx, new_ndx;
 word32 hotyp;
 vpiHandle href;
 struct mod_t *mdp;
 struct mod_pin_t *mpp;
 struct expr_t *xp, *idndp;
 struct net_t *np;
 struct h_t *hp2;
 struct hrec_t *hrp;

 href = NULL;
 mdp = hp->hin_itp->itip->imsym->el.emdp;
 hrp = hp->hrec;
 mpp = &(mdp->mpins[hrp->hu.hpi]);
 /* this is index of port bit - always h:0 */
 ndx = hrp->hi;
 xp = mpp->mpref;
catcmp_again:
 /* know this is lvalue expr - using cver expressions */
 switch (xp->optyp) {
  case ID:
   idndp = xp;
make_handle:
   np = idndp->lu.sy->el.enp;
   /* notice need special correction for Net (wire) vector as bit */
   if (np->ntyp < NONWIRE_ST)
    {
     hotyp = (np->n_isavec) ? vpiNetBit : vpiNet;
    }
   /* reg cnnections never bits */
   else
    {
     if (np->n_isavec) hotyp = __to_vpinetbithtyp(np->ntyp);
     else hotyp = __ntyp_to_vpivarhtyp(np);
    }
   href = __mk_handle(hotyp, (void *) np, hp->hin_itp, NULL); 
   hp2 = (struct h_t *) href;
   /* for scalar will be -1 - ndx is internal h:0 that for ID is same bit */
   hp2->hrec->hi = ndx;
   if (hotyp == vpiNetBit || hotyp == vpiRegBit || hotyp == vpiVarSelect)
    hp2->hrec->bith_ndx = TRUE;
   break;
  case LSB:
   /* if constant, convert to bith_ndx form */ 
   if (__expr_is_vpiconst(xp->ru.x))
    {
     idndp = xp->lu.x;
     /* evaluate index since constant know will be non x but maybe IS form */
     __push_itstk(hp->hin_itp);
     ndx = __comp_ndx(idndp->lu.sy->el.enp, xp->ru.x);
     __pop_itstk();
     /* DBG remove --- */ 
     if (ndx == -1) __vpi_terr(__FILE__, __LINE__);
     /* --- */
     goto make_handle;
    }
   else
    {
     /* variable case - this is variable bit or array select */
     href = __mk_exprclass_handle(xp, hp->hin_itp, hrp->hin_tskp);
    }
   break;
  case PARTSEL:
   idndp = xp->lu.x;
   /* because always constant - normalized */
   ndx = __contab[xp->ru.x->ru.x->ru.xvi] + ndx;
   goto make_handle;
  case LCB:
   xp = find_catxp_frombit(xp, ndx, &new_ndx);
   ndx = new_ndx;
   goto catcmp_again;
  default: __vpi_terr(__FILE__, __LINE__);
 }  
 return(href);
}

/*
 * build an expr handle from a port for the low connection (mod port)
 */
static vpiHandle getexpr_lowconn(struct h_t *hp)
{
 vpiHandle ihref;
 struct mod_t *mdp;
 struct mod_pin_t *mpp;

 if (hp->hrec->htyp != vpiPort)
  { no1_1to1h_err(vpiLowConn, vpiPort, hp); return(NULL); }
 mdp = hp->hin_itp->itip->imsym->el.emdp;
 mpp = &(mdp->mpins[hp->hrec->hu.hpi]);
 ihref = __mk_exprclass_handle(mpp->mpref, hp->hin_itp, NULL);
 return(ihref);
}

/*
 * build an variable or var bit handle form vpi Port bit
 *
 * know handle is vpiPortBit
 * for port bit, get iconn expr then take apart to component, then to 
 * bit in expression, but if vpiOperation returns nil since no handle 
 *
 * this is hard case because converting up iconn expr to net/bit handle
 * and can be xmr or in module qualified name
 *
 * same as acc_, only variables, bit and port selects or concatenates thereof
 * are legal (not bits of constants, for example)
 *
 * hp is vpi port bit
 */
static vpiHandle getbit_highconn(struct h_t *hp)
{
 int32 ndx, new_ndx;
 word32 xtyp, otyp, hotyp;
 vpiHandle href;
 struct inst_t *ip;
 struct itree_t *up_itp, *itp;
 struct net_t *np;
 struct expr_t *xp, *idndp;
 struct task_t *tskp;
 struct h_t *hp2;
 struct hrec_t *hrp;
 struct gref_t *grp;
 struct mod_t *mdp;

 href = NULL;
 hrp = hp->hrec;
 ip = hp->hin_itp->itip;
 mdp = ip->imsym->el.emdp;
 /* no highconn for ports of top level module */
 if ((up_itp = hp->hin_itp->up_it) == NULL) return(NULL);
 ndx = hrp->hi; 
 xp = ip->ipins[hrp->hu.hpi];
new_expr:
 /* possibly unconnected */  
 if (ndx >= xp->szu.xclen)
 /* possibly unconnected */  
 /* SJM 08/24/99 - silently return nil if unc. - only way to tell if */
 /* unc - no warning because otherwise no way to tell unc. */
 if (ndx >= xp->szu.xclen) return(NULL);

 xtyp = (word32) __exprtype_get(xp);
 if (xtyp == vpiOperation)
  {
   /* know concatenates reduced to one level always */
   if ((otyp = (word32) __expr_optype_get(xp)) == vpiConcatOp) 
    {
     xp = find_catxp_frombit(xp, ndx, &new_ndx);
     ndx = new_ndx;
     goto new_expr;
    }
   __to_vpiopnam(__wrks2, (int32) otyp);
no_iconnbit:
   /* xtyp is an object */
   __vpi_err(2014, vpiWarning,
    "vpi_handle: no vpiHighConn of vpiPortBit for %s expression type %s",
    __to_vpionam(__wrks1, xtyp), __wrks2);
   return(NULL);
  }  
 if (xtyp == vpiConstant || xtyp == vpiSysFuncCall || xtyp == vpiFuncCall)
  {
   strcpy(__wrks2, "*none*");
   goto no_iconnbit;
  }

 switch (xp->optyp) {
  case ID:
   /* if vector, will be bit handle if scalar will be net/reg handle only */
   idndp = xp;
make_var:
   if (idndp->locqualnam)
    tskp = __find_qualnam_task(idndp->ru.qnchp, mdp, hrp->hin_tskp);
   else tskp = NULL;
   itp = up_itp;
bld_handle:
   /* this always builds either variable or variable bit */
   np = idndp->lu.sy->el.enp;
   /* notice need special correction for Net (wire) vector as bit */
   if (np->ntyp < NONWIRE_ST)
    {
     hotyp = (np->n_isavec) ? vpiNetBit : vpiNet;
    }
   else
    {
     if (np->n_isavec) hotyp = __to_vpinetbithtyp(np->ntyp);
     else hotyp = __ntyp_to_vpivarhtyp(np);
    }
   href = __mk_handle(hotyp, (void *) np, itp, tskp); 
   hp2 = (struct h_t *) href;
   /* for scalar will be 0 */
   hp2->hrec->hi = ndx;
   hp2->hrec->bith_ndx = TRUE;
   break;
  case GLBREF:
   idndp = xp;
make_grp_var:
   grp = idndp->ru.grp;
   __push_itstk(up_itp);
   __xmrpush_refgrp_to_targ(grp);
   itp = __inst_ptr;
   __pop_itstk();
   __pop_itstk();
   tskp = grp->targtskp;
   goto bld_handle;
  case LSB:
   /* here handle becomes index of bit select */
   idndp = xp->lu.x;
   /* no way in vpi_ routines to represent array bit (undivisable) */ 
   if (idndp->lu.sy->el.enp->n_isarr)
    {
     __vpi_err(2026, vpiWarning,
      "vpi_handle: no vpiHighConn of vpiPortBit for array %s",
      idndp->lu.sy->el.enp->nsym->synam);
     return(NULL);
    }
   /* also if var select, no highconn because no fixed bit */  
   if (xp->ru.x->optyp != NUMBER && xp->ru.x->optyp != ISNUMBER)
    { strcpy(__wrks2, "variable vpiVarSelect"); goto no_iconnbit; }

   /* rule for bit conn, is that it must be constant bit select */ 
   /* or error before here - but routine must return vpiNetbit */
   /* need to evaluate for constant since may be IS form */
   if (__expr_is_vpiconst(xp->ru.x))
    {
     __push_itstk(up_itp);
     /* evaluate index since constant know will be non x */
     ndx = __comp_ndx(idndp->lu.sy->el.enp, xp->ru.x);
     if (ndx == -1) 
      { strcpy(__wrks2, "x/z vpiVarSelect"); goto no_iconnbit; }
     __pop_itstk();  
     if (idndp->optyp == GLBREF) goto make_grp_var;
     goto make_var;
    }
   else
    {
     /* variable case - this is variable bit or array select */
     /* up xmr to task if present will be in XMR ref - se just need up */
     href = __mk_exprclass_handle(xp, up_itp, NULL);
    }
   break;
  case PARTSEL:
   /* this becomes vpiNetBit or vpiRegBit with index correct for part sel */
   idndp = xp->lu.x;
   ndx = __contab[xp->ru.x->ru.x->ru.xvi] + ndx;
   if (idndp->optyp == GLBREF) goto make_grp_var;
   goto make_var;
  default: __vpi_terr(__FILE__, __LINE__);
 }  
 return(href);
}

/*
 * given a concatenate and a bit index, return the concat component expr
 *
 * this works on Cver expr_t not handles
 * know in range
 */
static struct expr_t *find_catxp_frombit(struct expr_t *catxp, int32 bi,
 int32 *newbi)
{
 register struct expr_t *xp;
 register int32 catbi;
 
 xp = catxp->ru.x;
 for (catbi = catxp->szu.xclen; xp != NULL; xp = xp->ru.x)
  {
   catbi -= xp->lu.x->szu.xclen;
   if (bi >= catbi) { *newbi = bi - catbi; return(xp->lu.x); }
  }
 __vpi_terr(__FILE__, __LINE__);
 return(NULL);
}

/*
 * find loal qualified name task
 *
 * because this can only be local qualified (named blocks) name - instance
 * selects impossible
 *
 * never need to store except for vpi since have right symbol
 * but for vpi need to know task in
 */
extern struct task_t *__find_qualnam_task(char *qualnam, struct mod_t *mdp,
 struct task_t *reftskp)
{
 register struct expr_t *gcmp_ndp;
 struct sy_t *syp;
 struct expr_t *qn_ndp;
 struct symtab_t *sytp;

 /* know will have at least 2 components including ending variable */
 /* this can never fail because processing already check qualified name */
 if ((qn_ndp = __glbnam_to_expr(qualnam)) == NULL)
  __vpi_terr(__FILE__, __LINE__);
 /* handle already in some task - work upward */
 gcmp_ndp = qn_ndp->ru.x;
 /* DBG remove --- */ 
 if (gcmp_ndp->lu.x->optyp != XMRID) __vpi_terr(__FILE__, __LINE__); 
 /* --- */
 if (reftskp != NULL) 
  {
   for (sytp = reftskp->tsksymtab;;) 
    {
     if ((syp = __get_sym(gcmp_ndp->lu.x->ru.qnchp, sytp)) != NULL) 
      {
       if (!__is_scope_sym(syp)) __vpi_terr(__FILE__, __LINE__);
       if (syp->sytyp == SYM_M) sytp = syp->el.emdp->msymtab;
       else sytp = syp->el.etskp->tsksymtab;
       gcmp_ndp = gcmp_ndp->ru.x;
       break;
      }
     if ((sytp = sytp->sytpar) == NULL) __vpi_terr(__FILE__, __LINE__);
    }
  }
 else { sytp = mdp->msymtab;; }
 /* have start of path symbol table - need to stop 1 from tail end */
 /* DBG remove --- */ 
 if (gcmp_ndp == NULL) __vpi_terr(__FILE__, __LINE__);
 /* -- */ 
 /* know at least one more component by here */
 for (;;)
  {
   /* DBG remove ---*/
   if (gcmp_ndp->lu.x->optyp != XMRID) __vpi_terr(__FILE__, __LINE__);
   /* ---*/
   if ((syp = __get_sym(gcmp_ndp->lu.x->ru.qnchp, sytp)) == NULL
    || !__is_scope_sym(syp)) __vpi_terr(__FILE__, __LINE__);
   sytp = syp->el.etskp->tsksymtab;
   if ((gcmp_ndp = gcmp_ndp->ru.x) == NULL) break; 
   /* if next is tail wire, also done */
   if (gcmp_ndp->ru.x == NULL) break;
  }
 __free_xtree(qn_ndp); 
 return(sytp->sypofsyt->el.etskp);
}

/*
 * build an expr handle from a port for the high connection (iconn)
 */
static vpiHandle getexpr_highconn(struct h_t *hp)
{
 vpiHandle ihref;
 struct inst_t *ip;
 struct itree_t *up_itp;
 struct hrec_t *hrp;

 hrp = hp->hrec;
 if (hrp->htyp != vpiPort && hrp->htyp != vpiPortBit)
  { no1_1to1h_err(vpiHighConn, vpiPort, hp); return(NULL); }
 ip = hp->hin_itp->itip;
 if ((up_itp = hp->hin_itp->up_it) == NULL) return(NULL);
 /* for port, high conn is iconn expression */
 ihref = __mk_exprclass_handle(ip->ipins[hrp->hu.hpi], up_itp, NULL);
 return(ihref);
}

/*
 * no 1-to-1 connection error with 1 expected handle
 */
static void no1_1to1h_err(word32 typ1to1, word32 exptyp, struct h_t *hp)
{
 char s1[RECLEN];

 __vpi_err(1887, vpiError,
  "vpi_handle: handle %s (expected %s) does not have a 1-to-1 connection for type %s", 
  __to_vpionam(__wrks2, hp->hrec->htyp), __to_vpionam(s1, exptyp),
  __to_vpionam(__wrks1, typ1to1));
}

/*
 * no 1-to-1 connection error with no one expected handle type
 */
static void no_1to1h_err(int32 typ1to1, struct h_t *hp)
{
 __vpi_err(1887, vpiError,
  "vpi_handle: handle %s does not have a 1-to-1 connection for type %s", 
  __to_vpionam(__wrks2, hp->hrec->htyp), __to_vpionam(__wrks1,
  (word32) typ1to1));
}

/*
 * build an tchk term handle from a tchk - 2 different no iterator 
 */
static vpiHandle get_tchk_term(word32 termtyp, struct h_t *hp)
{
 vpiHandle ihref;
 struct tchk_t *tcp;
 struct h_t *hp2; 
 struct hrec_t *hrp;

 hrp = hp->hrec;
 if (hrp->htyp != vpiTchk)
  { no1_1to1h_err(termtyp, vpiTchk, hp); return(NULL); }
 tcp = hrp->hu.htcp;
 if (termtyp == vpiTchkNotifier)
  {
   if (tcp->ntfy_np == NULL) return(NULL);
   ihref = __mk_handle(vpiReg, (void *) tcp->ntfy_np, hp->hin_itp, NULL);
  }
 else
  {
   ihref = __mk_handle(vpiTchkTerm, (void *) tcp, hp->hin_itp, NULL);
   hp2 = (struct h_t *) ihref;
   /* term type determines since each terminal of tchk different */
   hp2->hrec->htyp2 = (word32) termtyp;
  }
 return(ihref);
}

/*
 * get condition 1-to-1 handle from hp
 */
static vpiHandle get_cond(struct h_t *hp)
{
 register struct hrec_t *hrp;
 vpiHandle ihref;
 struct tchk_t *tcp;
 struct spcpth_t *pthp;
 struct delctrl_t *dctp;
 struct expr_t *xp;

 hrp = hp->hrec;
 switch (hrp->htyp) {
  case vpiTchkTerm:
   tcp = hrp->hu.htcp;
   if (hrp->htyp2 == vpiTchkRefTerm)
    {
     if (tcp->startcondx == NULL) return(NULL);
     ihref = __mk_exprclass_handle(tcp->startcondx, hp->hin_itp, NULL);
    }
   else if (hrp->htyp2 == vpiTchkDataTerm)
    {
     if (tcp->chkcondx == NULL) return(NULL);
     ihref = __mk_exprclass_handle(tcp->chkcondx, hp->hin_itp, NULL);
    }
   else { __vpi_terr(__FILE__, __LINE__); return(NULL); }
   break;
  case vpiModPath:
   pthp = hrp->hu.hpthp;
   if (pthp->pthcondx == NULL) return(NULL);
   ihref = __mk_exprclass_handle(pthp->pthcondx, hp->hin_itp, NULL);
   break;
  /* conditon is if conditon */
  case vpiIf: case vpiIfElse:
   ihref = __mk_exprclass_handle(hrp->hu.hstp->st.sif.condx,
    hp->hin_itp, hrp->hin_tskp);
   break;
  case vpiCase:
   ihref = __mk_exprclass_handle(hrp->hu.hstp->st.scs.csx,
    hp->hin_itp, hrp->hin_tskp);
   break;
  /* notice to get condition for rhs proca event control first */
  /* use stmt handle of event control then handle of event control stmt */
  /* this is expression that is special - but checking already done */
  case vpiEventControl:
   dctp = hrp->hu.hstp->st.sdc;
   /* DBG remove --- */
   if (dctp->dc_delrep != DT_1X) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   xp = dctp->dc_du.d1x;  
   ihref = __mk_exprclass_handle(xp, hp->hin_itp, hrp->hin_tskp);
   break;
  case vpiWhile:
   ihref = __mk_exprclass_handle(hrp->hu.hstp->st.swh.lpx, hp->hin_itp,
    hrp->hin_tskp);
   break;
  case vpiRepeat:
   ihref = __mk_exprclass_handle(hrp->hu.hstp->st.srpt.repx, hp->hin_itp,
    hrp->hin_tskp);
   break;
  case vpiWait:
   ihref = __mk_exprclass_handle(hrp->hu.hstp->st.swait.lpx, hp->hin_itp,
    hrp->hin_tskp);
   break;  
  case vpiFor:
   ihref = __mk_exprclass_handle(hrp->hu.hstp->st.sfor->fortermx,
    hp->hin_itp, hrp->hin_tskp);
   break;  
  default: no_1to1h_err(vpiCondition, hp); return(NULL);
 }
 return(ihref);
}

/*
 * get expr. (1-to-1) from handle
 *
 * this traverses to expr_t then converts to <expr object class> handle
 * except for disable statement vpi_handle
 */
static vpiHandle bld_1to1_exprclass_handle(struct h_t *hp)
{
 register int32 pi;
 register struct hrec_t *hrp;
 word32 hotyp;
 struct spcpth_t *pthp;
 struct tchk_t *tcp;
 struct gate_t *gp;
 struct net_t *np;
 struct pathel_t *pep;
 struct expr_t *xp;
 struct delctrl_t *dctp;
 vpiHandle ihref;

 ihref = NULL;
 hrp = hp->hrec;
 switch (hrp->htyp) {
  case vpiDisable:
   /* even though disable is a statement scope here is expr. disabling scope */
   /* odd case because vpiScope 1-to-1 method gets containing scope */ 
   /* not scope to disable - therefore using vpiExpr 1-to-1 method */
   return(get_disable_scope(hp));
  case vpiTchkTerm:
   tcp = hrp->hu.htcp;
   /* get tchk term of notifier is expr. already not tchk terminal obj */
   if (hrp->htyp2 == vpiTchkRefTerm)
    {
     if (tcp->startxp == NULL) return(NULL);
     ihref = __mk_exprclass_handle(tcp->startxp, hp->hin_itp, NULL);
     break;
    }
   if (hrp->htyp2 == vpiTchkDataTerm)
    {
     if (tcp->chkxp == NULL) return(NULL);
     ihref = __mk_exprclass_handle(tcp->chkxp, hp->hin_itp, NULL);
     break;
    }
   __vpi_terr(__FILE__, __LINE__);
   return(NULL);
  case vpiPathTerm:
   pthp = hrp->hu.hpthp;
   if (hrp->htyp2 == vpiModPathIn || hrp->htyp2 == vpiModPathOut)
    { 
     if (hrp->htyp2 == vpiModPathIn) pep = &(pthp->peins[hrp->hi]);
     else pep = &(pthp->peouts[hrp->hi]);    
     ihref = mk_pthterm_exprclass_handle(pep->penp, pep->pthi1, pep->pthi2,
      hp->hin_itp);
     break;
    }  
   if (hrp->htyp2 != vpiModDataPathIn) __vpi_terr(__FILE__, __LINE__);
   if ((xp = pthp->datasrcx) == NULL) return(NULL);

   /* notice indices start at 0 */
   if (xp->optyp != FCCOM)
    {
     if (hrp->hi > 0) __vpi_terr(__FILE__, __LINE__);
     ihref = __mk_exprclass_handle(xp, hp->hin_itp, NULL);
     break;
    }
   /* FCCOM list */
   for (pi = 0; xp != NULL; xp = xp->ru.x, pi++) 
    {
     if (xp->optyp != FCCOM || pi > hrp->hi) __vpi_terr(__FILE__, __LINE__); 
     if (pi == hrp->hi) 
      {
       ihref = __mk_exprclass_handle(xp->lu.x, hp->hin_itp, NULL);
       break;  
      }
    }
   break;
  case vpiPrimTerm:
   gp = hrp->hu.hgp;
   ihref = __mk_exprclass_handle(gp->gpins[hrp->hi], hp->hin_itp, NULL);
   break;
  case vpiParameter: case vpiSpecParam:
   /* parameters stored as nets but this must return expr. not final val. */
   np = hrp->hu.hnp;
   if (np->nu.ct->parm_srep == SR_PXPR) xp = np->nu.ct->n_dels_u.d1x;
   else if (np->nu.ct->parm_srep == SR_PISXPR)
    {
     /* d4x used but in fact allocated size is number of insts */
     xp = np->nu.ct->n_dels_u.d4x[hp->hin_itp->itinum];
    }
   else { __vpi_terr(__FILE__, __LINE__); return(NULL); }
   ihref = __mk_exprclass_handle(xp, hp->hin_itp, hrp->hin_tskp);
   break;
  case vpiIODecl:
   if (hrp->htyp2 == vpiUdpDefn) return(NULL);

   /* since can not be bit of - just get wire or reg type */
   hotyp = __ntyp_to_vpivarhtyp(hrp->hu.hnp);
   /* since can only be var. do not need expr. class object */
   /* can be io decl in task so need task from io decl handle */
   ihref = __mk_handle(hotyp, (void *) hrp->hu.hnp, hp->hin_itp,
    hrp->hin_tskp);
   return(ihref);
  case vpiRepeatControl:
   /* 10/28/00 SJM - added vpi Expr 1-to-1 access method from repeat ev ctrl */
   /* returns the repeat count expr */
   dctp = hrp->hu.hstp->st.sdc;
   xp = hrp->hu.hstp->st.sdc->repcntx;
   ihref = __mk_exprclass_handle(xp, hp->hin_itp, hrp->hin_tskp);
   return(ihref);
  default: no_1to1h_err(vpiExpr, hp); return(NULL);
 }
 return(ihref);
}

/*
 * build a path terminal expression class handle
 * 
 * only need to allocate expr_t and waste storage for part select
 * never a task for specify section object
 *
 * builds vpiNetBit index form for bit select
 */
static vpiHandle mk_pthterm_exprclass_handle(struct net_t *np,
 int32 i1, int32 i2, struct itree_t *in_itp) 
{
 word32 hotyp;
 vpiHandle href;
 struct expr_t *xp, *xpcol, *xpid, *xp1, *xp2;
 struct h_t *hp;

 if (i1 == -1 && i2 == -1)
  {
   hotyp = __ntyp_to_vpivarhtyp(np);
   href = __mk_handle(hotyp, (void *) np, in_itp, NULL); 
   return(href);
  }

 /* bit select */
 if (i1 == i2)
  {
   /* this can be bit index form because can not be xmr */
   href = __mk_handle(vpiNetBit, (void *) np, in_itp, NULL);
   hp = (struct h_t *) href;
   hp->hrec->bith_ndx = TRUE;
   hp->hrec->hi = i1;
   return(href);
  }
 /* part select - allocate new expr and build expr vpiPartSelect handle */
 xpid = __sim_alloc_newxnd();
 xpid->optyp = ID; 
 xpid->lu.sy = np->nsym;
 xp1 = __bld_rng_numxpr((word32) i1, 0L, WBITS);
 xp2 = __bld_rng_numxpr((word32) i2, 0L, WBITS);
 /* root of part select */
 xp = __sim_alloc_newxnd();
 xp->optyp = PARTSEL;
 xp->lu.x = xpid;
 xpcol = __sim_alloc_newxnd();
 xpcol->optyp = COLON;
 xp->ru.x = xpcol;
 xpcol->lu.x = xp1;
 xpcol->ru.x = xp2;
 href = __mk_exprclass_handle(xp, in_itp, NULL);
 hp = (struct h_t *) href;
 hp->hrec->free_xpr = TRUE;
 return(href);
}

/*
 * get object containing scope handle
 *
 * LOOKATME - allowing use of vpiScope for things in module even though
 * LRM says handle should be vpiModule
 */
static vpiHandle get_obj_scope(struct h_t *hp)
{
 int32 ttyp;
 vpiHandle ihref;
 struct mod_t *mdp;
 struct symtab_t *sytp;
 struct task_t *up_tskp;
 struct hrec_t *hrp;

 ihref = NULL;
 mdp = hp->hin_itp->itip->imsym->el.emdp;
 hrp = hp->hrec;
 switch (hrp->htyp) {
  /* also process class containing scope (vpiModule) accessible */
  /* using vpiModule 1-to-1 method */
  case vpiAlways: case vpiInitial:
  /* simple cases - parent is module in */ 
  case vpiPort: case vpiContAssign:
  case vpiModPath: case vpiTchk: case vpiSpecParam:
  case vpiGate: case vpiUdp: case vpiSwitch: 
   ihref = __mk_handle(vpiModule, (void *) mdp, hp->hin_itp, NULL);
   break;
   /* this is object in handle - all variables and stmts */
  case vpiNet: case vpiReg: case vpiNamedEvent: case vpiMemory:
  case vpiIntegerVar: case vpiTimeVar: case vpiRealVar:
  case vpiModule:
   /* all statements - except those with symbol tables */
  case vpiAssignStmt: case vpiAssignment: case vpiBegin:
  case vpiCase: case vpiDeassign: case vpiDelayControl:
  case vpiEventStmt: case vpiFor: case vpiForce: case vpiForever:
  case vpiFork: case vpiFuncCall: case vpiIf: case vpiIfElse:
  case vpiNullStmt: case vpiRelease: case vpiRepeat: case vpiSysFuncCall:
  case vpiSysTaskCall: case vpiTaskCall: case vpiWait: case vpiWhile:
  /* for expression forms - in module is where expr. appears not variable */
  case vpiMemoryWord: case vpiNetBit: case vpiRegBit: case vpiVarSelect:
  case vpiNetDriver: case vpiNetBitDriver:
  /* 10/28/00 - added missing things with scope */ 
  case vpiEventControl: case vpiRepeatControl:
   /* if index is expression form, context is where expr appears */
   /* to get variable def. loc. need to get parent object */
   /* this can either be in module or task */
   if (hp->hrec->hin_tskp == NULL)
    ihref = __mk_handle(vpiModule, (void *) mdp, hp->hin_itp, NULL);
   else
    {
     sytp = hrp->hin_tskp->tsksymtab;
     if (sytp->sytpar == NULL || sytp->sytpar->sypofsyt->sytyp == SYM_M)
      up_tskp = NULL;
     else up_tskp = sytp->sytpar->sypofsyt->el.etskp;

     ttyp = __to_vpi_tasktyp(hrp->hin_tskp->tsktyp);
     ihref = __mk_handle(ttyp, (void *) hrp->hin_tskp, hp->hin_itp, up_tskp);
    }
   break;
  /* these are definitions in */
  case vpiTask: case vpiFunction:
   return(bld_scope_par(hp, hrp->hu.htskp));
  /* LOOKATME - think can reduce to one case for all 4 */
  case vpiNamedBegin: case vpiNamedFork:
   /* for named block handle is stmt not task so know loc. - can go from */
   /* task to enclosing stmt */
   return(bld_scope_par(hp, hrp->hu.htskp));
  default: no_1to1h_err(vpiScope, hp); return(NULL);
 }
 return(ihref);
}

/*
 * get scope (task/func/named fork or join) of disable
 *
 * LOOKATME - since statement vpiScope should be containing 
 * but LRM says this is exception where it is disable scope
 */
static vpiHandle get_disable_scope(struct h_t *rhp)
{
 int32 ttyp;
 struct st_t *stp;
 struct expr_t *xp;
 struct itree_t *itp;
 struct task_t *tskp;
 struct symtab_t *sytp;
 struct task_t *up_tskp;

 stp = rhp->hrec->hu.hstp;
 xp = stp->st.sdsable.dsablx;

 if (xp->optyp == GLBREF)
  {
   __push_itstk(rhp->hin_itp);
   __xmrpush_refgrp_to_targ(xp->ru.grp);
   itp = __inst_ptr;
   __pop_itstk();
   __pop_itstk();
  }
 else if (xp->optyp == ID) itp = rhp->hin_itp;
 else { __vpi_terr(__FILE__, __LINE__); return(NULL); }

 /* this is scope symbol */
 tskp = xp->lu.sy->el.etskp;
 sytp = tskp->tsksymtab;
 if (sytp->sytpar == NULL || sytp->sytpar->sypofsyt->sytyp == SYM_M)
  up_tskp = NULL;
 else up_tskp = sytp->sytpar->sypofsyt->el.etskp;

 /* return task-function-named block handle */
 ttyp = __to_vpi_tasktyp(tskp->tsktyp);
 return(__mk_handle(ttyp, (void *) tskp, itp, up_tskp));
} 

/*
 * get a contained in stmt handle
 */
static vpiHandle get_contained_stmt(struct h_t *rhp)
{
 register struct hrec_t *rhrp;
 word32 sttyp;
 vpiHandle ihref;
 struct st_t *ifstp, *thenstp, *dcstp, *stp;
 struct csitem_t *csip;
 struct delctrl_t *dctp;
 struct task_t *tskp;

 rhrp = rhp->hrec;
 switch (rhrp->htyp) {
   case vpiInitial: case vpiAlways:
    stp = rhrp->hu.hialstp->iastp;
    if (stp->st_unbhead) sttyp = vpiBegin;
    else sttyp = __to_vpi_stmttyp(&stp);
    ihref = __mk_stmt_handle(sttyp, stp, rhp->hin_itp, NULL);
    break;
   case vpiTask: case vpiFunction:
    /* SJM 07/30/01 - misisng access from function or task object to stmt */
    tskp = rhrp->hu.htskp;
    stp = tskp->tskst;
    if (stp->st_unbhead) sttyp = vpiBegin;
    else sttyp = __to_vpi_stmttyp(&stp);
    ihref = __mk_stmt_handle(sttyp, tskp->tskst, rhp->hin_itp, tskp);
    break; 
   case vpiIf: case vpiIfElse:
    ifstp = rhrp->hu.hstp;    
    thenstp = ifstp->st.sif.thenst;
    if (thenstp->st_unbhead) sttyp = vpiBegin;
    else sttyp = __to_vpi_stmttyp(&thenstp);
    ihref = __mk_stmt_handle(sttyp, thenstp, rhp->hin_itp, rhrp->hin_tskp);
    break;
   case vpiCaseItem:
    /* for multiple expressions, know points to first (stp non nil) */
    csip = rhrp->hu.hcsip;
    stp = csip->csist; 
    if (stp->st_unbhead) sttyp = vpiBegin;
    else sttyp = __to_vpi_stmttyp(&stp);
    ihref = __mk_stmt_handle(sttyp, stp, rhp->hin_itp, rhrp->hin_tskp);
    break;
   case vpiDelayControl:
    dctp = rhrp->hu.hstp->st.sdc;
    /* really assignment with rhs delay control so LRM requires nil */
    if (dctp->dctyp == DC_RHSDELAY || dctp->actionst == NULL) return(NULL);
    dcstp = dctp->actionst;
    if (dcstp->st_unbhead) sttyp = vpiBegin;
    else sttyp = __to_vpi_stmttyp(&dcstp);
    ihref = __mk_stmt_handle(sttyp, dcstp, rhp->hin_itp, rhrp->hin_tskp);
    break;
   case vpiEventControl:
    dctp = rhrp->hu.hstp->st.sdc;
    /* really assignment with rhs delay control so LRM requires nil */
    if (dctp->dctyp == DC_RHSEVENT || dctp->actionst == NULL) return(NULL);
    dcstp = dctp->actionst;
    /* since after prep for some loops need to return one stmt after setup */
    if (dcstp->st_unbhead) sttyp = vpiBegin;
    else sttyp = __to_vpi_stmttyp(&dcstp);
    ihref = __mk_stmt_handle(sttyp, dcstp, rhp->hin_itp, rhrp->hin_tskp);
    break;
   /* forever condition is nil and never accessed */ 
   case vpiWhile: case vpiForever:
    stp = rhrp->hu.hstp->st.swh.lpst;
loop_sthandle:
    if (stp->st_unbhead) sttyp = vpiBegin;
    else sttyp = __to_vpi_stmttyp(&stp);
    ihref = __mk_stmt_handle(sttyp, stp, rhp->hin_itp, rhrp->hin_tskp);
    break;
   case vpiRepeat: stp = rhrp->hu.hstp->st.srpt.repst; goto loop_sthandle;
   case vpiWait: stp = rhrp->hu.hstp->st.swait.lpst; goto loop_sthandle;
   case vpiFor: stp = rhrp->hu.hstp->st.sfor->forbody; goto loop_sthandle;
    
  default: no_1to1h_err(vpiStmt, rhp); return(NULL);
 }
 return(ihref);
}

/*
 * get a vpi delay control statement from an vpi assignment statement
 * this is tricky because for rhs dctrls DELCTRL is stmt not actionst assign
 */
static vpiHandle get_dctrl_stmt(struct h_t *rhp, int32 dctype)
{
 struct st_t *stp;
 struct delctrl_t *dctp;
 struct hrec_t *rhrp;

 rhrp = rhp->hrec;
 if (rhrp->htyp != vpiAssignment)
  { no_1to1h_err(dctype, rhp); return(NULL); }
 stp = rhrp->hu.hstp;
 if (stp->stmttyp != S_DELCTRL) return(NULL);
 /* same code for both delay control and event control */
 dctp = stp->st.sdc;
 if (dctp->dctyp != DC_RHSEVENT && dctp->dctyp != DC_RHSDELAY) 
  __vpi_terr(__FILE__, __LINE__);
 if (dctype == vpiDelayControl && dctp->dctyp == DC_RHSDELAY)
  {
   return(__mk_handle(vpiDelayControl, (void *) stp, rhp->hin_itp,
    rhrp->hin_tskp));
  }
 if (dctype == vpiEventControl && dctp->dctyp == DC_RHSEVENT
  && dctp->repcntx == NULL)
  {
   return(__mk_handle(vpiEventControl, (void *) stp, rhp->hin_itp,
    rhrp->hin_tskp));
  }
 /* 10/26/00 SJM - also access repeat control */
 if (dctype == vpiRepeatControl && dctp->dctyp == DC_RHSEVENT
  && dctp->repcntx != NULL) 
  {
   return(__mk_handle(vpiRepeatControl, (void *) stp, rhp->hin_itp,
    rhrp->hin_tskp));
  }
 return(NULL);
}

/*
 * get a udp define handle from either a udp primitive
 * or object inside udp Def
 */
static vpiHandle get_udpdef_from_inobj(struct h_t *rhp)
{
 vpiHandle href;
 struct udp_t *udpp;
 struct hrec_t *rhrp;

 rhrp = rhp->hrec;
 switch (rhrp->htyp) {
  case vpiIODecl:
   /* DBG remove --- */
   if (rhrp->htyp2 != vpiUdpDefn) __vpi_terr(__FILE__, __LINE__); 
   /* --- */
   udpp = (struct udp_t *) rhp->hin_itp;
   break;
  case vpiTableEntry: case vpiInitial:
   udpp = (struct udp_t *) rhp->hin_itp;
   break;
  case vpiUdp:
   udpp = rhrp->hu.hgp->gmsym->el.eudpp;
   break;
  default: no_1to1h_err(vpiUdpDefn, rhp); return(NULL);
 }
 href = __mk_handle(vpiUdpDefn, (void *) udpp, NULL, NULL);
 return(href);
}

/*
 * get initial value - handle here is just udp with initial type 
 */
static vpiHandle get_contained_udp_init(struct h_t *rhp)
{
 vpiHandle href;
 struct udp_t *udpp;
 struct h_t *hp;

 udpp = rhp->hrec->hu.hudpp;
 if (udpp->ival == NO_VAL)  
  {
   __vpi_err(2111, vpiNotice,
    "vpi_handle of vpiInitial for vpiUdpDefn handle failed - no initial value");
   return(NULL);
  }
 href = __mk_handle(vpiInitial, (void *) udpp, NULL, NULL);
 hp = (struct h_t *) href;
 hp->hrec->htyp2 = vpiUdpDefn;
 return(href);
}

/*
 * one to one access method for getting up instance pound param override 
 * expression
 */
static vpiHandle get_up_poundparam_expr(struct h_t *rhp)
{
 int32 pi;
 vpiHandle href;
 struct inst_t *ip;
 struct mod_t *mdp;
 struct itree_t *up_itp;
 struct expr_t *xp;

 if (rhp->hrec->htyp != vpiParameter)
  {
   no_1to1h_err(vpiPoundParam, rhp);
   return(NULL);
  }

 ip = rhp->hin_itp->itip;
 if (ip->ipxprtab == NULL) return(NULL);

 mdp = rhp->hin_itp->itip->imsym->el.emdp;
 pi = rhp->hrec->hu.hnp - mdp->mprms;
 if ((xp = ip->ipxprtab[pi]) == NULL) return(NULL);

 up_itp = rhp->hin_itp->up_it;
 /* DBG remove --- */
 if (up_itp == NULL) __vpi_terr(__FILE__, __LINE__);
 /* --- */

 href = __mk_exprclass_handle(xp, up_itp, NULL);
 return(href);
}

/*
 * ROUTINES TO BUILD ITERATOR HANDLES (FIRST GROUP) 
 */

/*
 * allocate and return an iterator handle
 *
 * build the entire table of iterators that are returned in order and
 * freed by scan
 *
 * ALGORITHM: use type as first level index that filters which of multiple
 *            objects to build into scan table.  Then for given type
 *            use handle (object) as object from which to traverse and build
 *            type list (i.e. type is target of ->->[type])
 *
 * a nil reference handle means use all top level modules
 * FIXME - what if reference handle is nil for top level modules?
 */
extern vpiHandle vpi_iterate(PLI_INT32 itype, vpiHandle referenceHandle)
{
 register struct h_t *hp;

 __last_eip = NULL;
 if (__run_state == SS_COMP)
  { __still_comp_err("vpi_iterate"); return(NULL); } 
 if (!__validate_accessm("vpi_iterate", itype, "1-to-many iterator"))
  return(NULL);
 hp = (struct h_t *) referenceHandle;
 /* some iterators have nil (top level?) and value cases so must check */
 /* for validitly of nil in the routine */
 if (hp != NULL && !__validate_handle("vpi_iterate", hp)) return(NULL);

 /* separate processing for each 1 to many connection type */ 
 /* given 1 - many conn. type handle determines object to traverse from */
 /* type of thing that can appear as multiple list in some handle */
 switch (itype) {
  case vpiModule:
   return(bld_itree_iterator(hp));
  case vpiOneOfEachMod:
   return(bld_type_iterator(hp));
  case vpiUdpDefn:
   return(bld_udpdef_iterator(hp));
  /* this is 1-to-many acess method selector - inside mod or task */
  case vpiInternalScope:
   return(bld_scope_iterator(hp));
  case vpiPort:
   /* for ports in module and net/reg and bit port hiconn connections */
   return(__bld_port_iterator(hp));
  case vpiPortInst: return(__bld_neticonn_iter(hp));
  case vpiNet: case vpiReg: case vpiNamedEvent: case vpiMemory:
  case vpiVariables:
   /* vpiVariables for all of integer, time, real - not separated */  
   return(bld_net_iterator(hp, (word32) itype));
  case vpiProcess:
   /* list of initial - always statements in module - only access from mod */
   return(bld_initalw_iterator(hp));
  case vpiContAssign:
   return(bld_conta_iterator(hp));
  /* here primitive is right since it is the access method */
  case vpiPrimitive:
   /* gate, switch, or udp, handle set to actual type */
   return(bld_gate_iterator(hp));
  case vpiModPath: return(bld_modpth_iterator(hp));    
  case vpiTchk: return(bld_tchk_iterator(hp));    
  case vpiParameter: return(bld_param_iterator(hp, itype));
  case vpiSpecParam: return(bld_specparam_iterator(hp));    
  case vpiDefParam: return(bld_defparam_stmt_iterator(hp));
  /* AIV 09/27/06 - this includes localparams too */ 
  case vpiParamAssign: return(__bld_paramassign_stmt_iter(hp));
  case vpiIODecl: return(__bld_iodecl_stmt_iter(hp));
  case vpiTableEntry: return(__bld_udpline_iter(hp));
  case vpiPrimTerm: return(__bld_primterm_iterator(hp));

  case vpiLocalLoad: return(__bld_loc_lds_iterator(hp, itype));
  case vpiLoad: return(__bld_lds_iterator(hp, itype));
  case vpiLocalDriver: return(__bld_loc_drvs_iterator(hp, itype)); 
  case vpiDriver: return(__bld_drvs_iterator(hp, itype)); 

  case vpiMemoryWord: return(__bld_arrwrd_iterator(hp));
  case vpiBit: return(__bld_bitof_iterator(hp));

  case vpiUserSystf: return(__bld_systf_iterator(hp));
  case vpiArgument: return(__bld_tfargexpr_iterator(hp));
  case vpiModPathIn: case vpiModPathOut: case vpiModDataPathIn:
   return(__bld_pthterm_iterator(hp, (word32) itype));
  case vpiStmt: return(__bld_stmt_iterator(hp));
  case vpiTchkTerm: return(__bld_netin_tchkterms(hp));
  case vpiPathTerm: return(__bld_netin_pthterms(hp));
  case vpiCaseItem: return(__bld_caseitems_iter(hp));
  case vpiExpr: return(__bld_casi_exprs_iter(hp));
  /* notice only concatenate accessible */
  case vpiOperand: return(__bld_operands_iter(hp));  
  /* error if hp non nil - routine checks */
  case vpiCallback: return(__bld_allcbs_iter(hp));
  case vpiDelay: return(__bld_delay_expr_iter(hp));
  case vpiAttribute: return(__bld_dig_attr_iter(hp));
  default:
   if (hp == NULL) strcpy(__wrks2, "**NULL**"); 
   else __to_vpionam(__wrks2, hp->hrec->htyp);
   __vpi_err(1838, vpiError,
    "method %s not a 1-to-many (iterator) object for %s handle",
    __to_vpionam(__wrks1, (word32) itype), __wrks2);
 }
 return(NULL);
}

/*
 * build the top level module (itree inst.) iterator 
 */
static vpiHandle bld_itree_iterator(struct h_t *hp)
{
 register int32 ii;
 register struct h_t *hp2;
 register struct hrec_t *hrp2;
 vpiHandle ihref;
 struct itree_t *itp;
 struct pviter_t *iterp;

 /* case 1: handle non nil, get instances (vpi Module) inside */
 if (hp != NULL) return(bld_inst_iterator(hp));

 /* case 2: nil so get top level modules */ 
 /* LOOKATME - how can there be no top level modules */
 if (__numtopm <= 0) return(NULL);

 /* get all of it roots */
 iterp = __alloc_iter(__numtopm, &ihref);
 for (ii = 0; ii < __numtopm; ii++)
  {
   hp2 = &(iterp->scanhtab[ii]);
   hrp2 = hp2->hrec; 
   hrp2->htyp = vpiModule;
   itp = __it_roots[ii];
   hrp2->hu.hmdp = itp->itip->imsym->el.emdp;
   hp2->hin_itp = itp;
  }
 return(ihref);
}

/*
 * build one instance of each type iterator 
 *
 * error if handle non nil
 * LOOKATME - for now returning connect modules (even if only used in
 * connect or maybe by accident connect from others) - is this right?
 */
static vpiHandle bld_type_iterator(struct h_t *hp)
{
 register int32 ti;
 register struct mod_t *mdp;
 register struct h_t *hp2;
 register struct hrec_t *hrp2;
 int32 numtypes;
 vpiHandle ihref;
 struct itree_t *itp;
 struct pviter_t *iterp;

 /* case 1: handle non nil, get instances (vpi Module) inside */
 if (hp != NULL)
  {
   __vpi_err(1860, vpiError,
    "vpiOneOfEachMod 1-to-many iterator 2nd argument must NULL - %s object passed",
    __to_vpionam(__wrks1, hp->hrec->htyp));
   return(NULL); 
  }
 /* count number of types */
 for (mdp = __modhdr, numtypes = 0; mdp != NULL; mdp = mdp->mnxt)
  {
   numtypes++;
  }

 if (numtypes <= 0) return(NULL);
 /* get all of it roots */
 iterp = __alloc_iter(numtypes, &ihref);
 for (ti = 0, mdp = __modhdr; ti < numtypes; ti++, mdp = mdp->mnxt)
  {
   hp2 = &(iterp->scanhtab[ti]);
   hrp2 = hp2->hrec; 
   hrp2->htyp = vpiModule;
   itp = mdp->moditps[0]; 
   hrp2->hu.hmdp = mdp;
   hp2->hin_itp = itp;
  }
 return(ihref);
}

/*
 * allocate an iterator 
 */
extern struct pviter_t *__alloc_iter(int32 nels, vpiHandle *ihrefp)
{
 register int32 iti;
 register struct h_t *hp;
 register struct hrec_t *hrarr;
 struct pviter_t *iterp;

 iterp = (struct pviter_t *) __my_malloc(sizeof(struct pviter_t));    
 iterp->numhs = nels;
 iterp->nxthi = 0;   
 *ihrefp = __mk_handle(vpiIterator, (void *) iterp,  NULL, NULL);
 /* this is table of handle guts not ptrs to handles */
 iterp->scanhtab = (struct h_t *) __my_malloc(nels*sizeof(struct h_t));
 hrarr = (struct hrec_t *) __my_malloc(nels*sizeof(struct hrec_t)); 
 iterp->ihrectab = hrarr;

 /* do the non type specific initialization */
 for (iti = 0; iti < nels; iti++)
  {
   hp = &(iterp->scanhtab[iti]);
   hp->hin_itp = NULL;
   hp->hrec = &(hrarr[iti]);
   __init_hrec(hp->hrec);
   hp->hrec->in_iter = TRUE;
  }
 return(iterp);
}

/*
 * build the iterator scan table for module instances in handle instance
 * 1-to-many Vpi Module to Vpi Module inside
 * only called if at least one contained instance (hp not nil)
 */
static vpiHandle bld_inst_iterator(struct h_t *hp)
{
 register int32 ii;
 register struct hrec_t *hrp2;
 register struct h_t *hp2;
 struct mod_t *mdp;
 struct itree_t *itp, *itp2;
 vpiHandle ihref;
 struct pviter_t *iterp;

 if (hp->hrec->htyp != vpiModule)
  { mustbe_inmoditer_err(vpiModule, hp); return(NULL); }
 itp = hp->hin_itp;
 mdp = itp->itip->imsym->el.emdp; 
 if (mdp->minum <= 0) return(NULL);
 iterp = __alloc_iter(mdp->minum, &ihref);
 for (ii = 0; ii < mdp->minum; ii++)
  {
   hp2 = &(iterp->scanhtab[ii]);
   hrp2 = hp2->hrec;
   hrp2->htyp = vpiModule;
   itp2 = &(itp->in_its[ii]);  
   hrp2->hu.hmdp = itp2->itip->imsym->el.emdp;
   hp2->hin_itp = itp2;
  }
 return(ihref);
}

/*
 * error for iterator that only exists in module but handle is other
 */
static void mustbe_inmoditer_err(word32 ityp, struct h_t *hp)
{
 __vpi_err(1841, vpiError,
  "%s 1-to-many iterator from object %s illegal - only allowed for vpiModule",
  __to_vpionam(__wrks1, ityp), __to_vpionam(__wrks2, hp->hrec->htyp));
}

/*
 * build the iterator for all defined in design iterators 
 *
 * in Cver all non instantiated udps removed
 */
static vpiHandle bld_udpdef_iterator(struct h_t *rhp)
{
 register int32 ui;
 register struct udp_t *udpp;
 register struct hrec_t *hrp;
 register struct h_t *hp; 
 int32 nudps;
 vpiHandle ihref;
 struct pviter_t *iterp;

 if (rhp != NULL)
  {
   __vpi_err(1837, vpiError,
    "vpi_iterate of vpiUdpDefn handle must be NULL (%s illegal) - udp definitions design wide",
    __to_vpionam(__wrks1, rhp->hrec->htyp));
   return(NULL);
  }
 for (udpp = __udphead, nudps = 0; udpp != NULL; udpp = udpp->udpnxt) nudps++;
 if (nudps <= 0) return(NULL);

 /* build the design wide iterator of all udps */
 iterp = __alloc_iter(nudps, &ihref);
 for (ui = 0, udpp = __udphead; ui < nudps; udpp = udpp->udpnxt, ui++)
  {
   hp = &(iterp->scanhtab[ui]);
   hrp = hp->hrec;
   hrp->htyp = vpiUdpDefn;
   hrp->hu.hudpp = udpp;
   /* there is no hin itp for design wide udps */
  }
 return(ihref);
}

/*
 * given an object handle - build iterator of all scopes in object
 *
 * nil handle is top level scopes
 * scope is class but actual handler is one of module, taskfunc, named begin,
 * and named fork
 */
static vpiHandle bld_scope_iterator(register struct h_t *hp) 
{
 struct mod_t *mdp;
 struct symtab_t *sytp;
 
 if (hp == NULL) return(__nil_iter_err(vpiInternalScope));
 switch (hp->hrec->htyp) {
  case vpiModule:
   /* scope list in module */
   mdp = hp->hin_itp->itip->imsym->el.emdp;
   return(bld_symtabs_iterator(mdp->msymtab->sytofs, hp->hin_itp));
  case vpiTask: case vpiFunction:
   /* real task, hu is task ptr */
   sytp = hp->hrec->hu.htskp->tsksymtab;
   return(bld_symtabs_iterator(sytp->sytofs, hp->hin_itp));
  case vpiNamedBegin: case vpiNamedFork:
   /* pseudo task, hu is stmt */
   sytp = hp->hrec->hu.htskp->tsksymtab;
   return(bld_symtabs_iterator(sytp->sytofs, hp->hin_itp));
  default:
   __vpi_err(1843, vpiError,
    "unable to construct iterator of contained scopes for %s object",
   __to_vpionam(__wrks1, hp->hrec->htyp));
 }
 return(NULL);
}

/*
 * emit error for iterator type that can not be nil
 *
 * needed because many design lists accessed from nil handle
 */
extern vpiHandle __nil_iter_err(word32 otyp)
{
 __vpi_err(1854, vpiError, "vpi_iterate type %s passed illegal NULL handle",
  __to_vpionam(__wrks1, otyp));
 return(NULL);
}

/*
 * build an iterator for a list of scopes - works from symbol table
 * know passed left most symbol table for multiple disjoint32 scopes
 */
static vpiHandle bld_symtabs_iterator(struct symtab_t *sytp,
 struct itree_t *itp)
{
 register int32 ii;
 register struct symtab_t *sytp2;
 register struct h_t *hp;
 register struct hrec_t *hrp;
 int32 numtabs;
 vpiHandle ihref;
 struct pviter_t *iterp;
 struct task_t *up_tskp;

 if (sytp == NULL) return(NULL);
 for (numtabs = 0, sytp2 = sytp; sytp2 != NULL; sytp2 = sytp2->sytsib)
  numtabs++;
 if (numtabs <= 0) return(NULL);
 iterp = __alloc_iter(numtabs, &ihref);
 for (sytp2 = sytp, ii = 0; ii < numtabs; sytp2 = sytp2->sytsib, ii++)
  {
   hp = &(iterp->scanhtab[ii]);
   hrp = hp->hrec;
   fill_scopehandle(hrp, sytp2);
   hp->hin_itp = itp;
   if (sytp->sytpar == NULL || sytp->sytpar->sypofsyt->sytyp == SYM_M)
    up_tskp = NULL;
   else up_tskp = sytp->sytpar->sypofsyt->el.etskp;
   /* this task handle is maybe in one up task (for nested named blocks) */  
   hrp->hin_tskp = up_tskp;
  }
 return(ihref);
}

/*
 * get vpi_ object associated with a symbol table - always a task_t
 *
 * only one of tskp or mdp set
 */
static void fill_scopehandle(struct hrec_t *hrp, struct symtab_t *sytp)
{
 struct task_t *tskp; 
 struct sy_t *syp;

 syp = sytp->sypofsyt;
 switch ((byte) syp->sytyp) {
  case SYM_M:
   hrp->htyp = vpiModule;
   hrp->hu.hmdp = syp->el.emdp;
   return;
  case SYM_TSK: hrp->htyp = vpiTask; break;
  case SYM_F: hrp->htyp = vpiFunction; break;
  case SYM_LB:
   tskp = syp->el.etskp;
   if (tskp->tsktyp == Begin) hrp->htyp = vpiNamedBegin;
   else if (tskp->tsktyp == FORK) hrp->htyp = vpiNamedFork;
   else __vpi_terr(__FILE__, __LINE__);
   /* this is tricky part of named block as task code because scope is tskp */
   /* but need the enclosing statement (can only be one) */
   hrp->hu.htskp = syp->el.etskp->st_namblkin->st.snbtsk;
   return;
  default:;
 }
 hrp->hu.htskp = syp->el.etskp;
}

/*
 * given an object containing nets handle - build iterator of all nets
 *
 * otype is vpi_ type of net 
 */
static vpiHandle bld_net_iterator(struct h_t *hp, word32 otype)
{
 register struct hrec_t *hrp;
 struct mod_t *mdp;
 struct task_t *tskp;
 struct itree_t *itp; 
 struct net_t *np;
 word32 ntyp;

 if (hp == NULL) return(__nil_iter_err(otype));
 itp = hp->hin_itp;
 hrp = hp->hrec;
 switch (hrp->htyp) {
  case vpiModule: 
   mdp = itp->itip->imsym->el.emdp;
   if (mdp->mnets == NULL) return(NULL);
   np = &(mdp->mnets[0]);
   return(bld_listofnets_iter(np, mdp->mnnum, itp, otype, NULL));
  case vpiTask: case vpiFunction: case vpiNamedBegin: case vpiNamedFork:
   /* variables class does not map to ntyp */  
   if (otype != vpiVariables)
    {
     /* cannot be wire type for task */
     ntyp = __from_vpi_vartyp(otype);
     if (ntyp < NONWIRE_ST)
      {
       __vpi_err(1845, vpiError,
        "unable to construct %s iterator for non module scope %s",
        __to_vpionam(__wrks1, otype), __to_vpionam(__wrks2, hrp->htyp));
       return(NULL);
      }
    }
   tskp = hrp->hu.htskp;
   if (tskp->tsk_regs == NULL) return(NULL);

   np = &(tskp->tsk_regs[0]);
   return(bld_listofnets_iter(np, tskp->trnum, itp, otype, tskp));
  default:
   __vpi_err(1847, vpiError,
    "unable to construct iterator of contained %s for %s object",
    __to_vpionam(__wrks1, otype), __to_vpionam(__wrks2, hrp->htyp));
 }
 return(NULL);
}

/*
 * build an iterator for a list of module nets of type type
 *
 * know at least one net or will not be called but maybe none of type
 */
static vpiHandle bld_listofnets_iter(struct net_t *np, int32 onnum,
 struct itree_t *itp, word32 otype, struct task_t *tskp)
{
 register int32 ni, ni2;
 register struct net_t *np2;
 register struct h_t *hp;
 register struct hrec_t *hrp;
 int32 nnum;
 word32 ntyp, vpityp;
 vpiHandle ihref;
 struct pviter_t *iterp;

 if ((nnum = cnt_typnetnum(np, onnum, otype)) <= 0) return(NULL);
 iterp = __alloc_iter(nnum, &ihref);
 if (otype == vpiMemory)
  {
   for (ni = ni2 = 0, np2 = np; ni < onnum; ni++, np2++)
    {
     if (np2->n_isarr)
      {
       /* notice number of nets (ni) maybe larger than iterator size ni2 */
       hp = &(iterp->scanhtab[ni2++]);
       hrp = hp->hrec;
       hrp->htyp = vpiMemory;
       hrp->hu.hnp = np2;
       hp->hin_itp = itp;
       hrp->hin_tskp = tskp;
      }
    }
   return(ihref);
  }
 /* passed vpiNet for all wire types (all non regs) */
 if (otype == vpiNet)
  {
   for (ni = ni2 = 0, np2 = np; ni < onnum; ni++, np2++)
    {
     if (np2->ntyp < NONWIRE_ST)
      {
       hp = &(iterp->scanhtab[ni2++]);
       hrp = hp->hrec;
       hrp->htyp = vpiNet; 
       hrp->hu.hnp = np2;
       hp->hin_itp = itp;
       hrp->hin_tskp = tskp;
      }
    }
   return(ihref);
  }
 /* passed vpiVariables for all reg types (all non wires) */
 if (otype == vpiVariables)
  {
   for (ni = ni2 = 0, np2 = np; ni < onnum; ni++, np2++)
    {
     if (np2->n_isarr) continue;

     /* 1 to many iterator selector must be variable but handle is separate */
     /* variable type */
     if (np2->ntyp == N_TIME) vpityp = vpiTimeVar;
     else if (np2->ntyp == N_INT) vpityp = vpiIntegerVar;
     else if (np2->ntyp == N_REAL) vpityp = vpiRealVar;
     else continue;
      
     hp = &(iterp->scanhtab[ni2++]);
     hrp = hp->hrec;
     hrp->htyp = vpityp;
     hrp->hu.hnp = np2;
     hp->hin_itp = itp;
     hrp->hin_tskp = tskp;
    }
   return(ihref);
  }
 /* this is used to convert for comparing know type already checked */
 ntyp = __from_vpi_vartyp(otype);
 for (ni2 = ni = 0, np2 = np; ni < onnum; ni++, np2++)
  {
   if (np2->n_isarr) continue;
   if (np2->ntyp == ntyp)
    {
     hp = &(iterp->scanhtab[ni2++]);
     hrp = hp->hrec;
     hrp->htyp = otype;
     hrp->hu.hnp = np2;
     hp->hin_itp = itp;
     hrp->hin_tskp = tskp;
    }
  }
 return(ihref);
}

/*
 * count number of nets of object type
 * needed because all wires stored in one list in Cver
 * possibilities are array, net (and wire) or match N_ internal type
 */
static int32 cnt_typnetnum(register struct net_t *np, int32 onnum,
 word32 typ)
{
 register int32 ni;
 word32 nnum;
 word32 ntyp;

 nnum = 0;
 if (typ == vpiMemory)
  {
   for (ni = 0; ni < onnum; ni++, np++) { if (np->n_isarr) nnum++; }
   return(nnum);
  }
 /* passed vpiNet for all wire types (all non regs) */
 if (typ == vpiNet)
  {
   for (ni = 0; ni < onnum; ni++, np++)
    { if (np->ntyp < NONWIRE_ST) nnum++; }
   return(nnum);
  }
 /* vpiVarible for real, integer, and time that are accessed together */
 if (typ == vpiVariables)
  {
   for (ni = 0; ni < onnum; ni++, np++)
    {
     if (np->n_isarr) continue;
     if (np->ntyp == N_REAL || np->ntyp == N_INT || np->ntyp == N_TIME)
      nnum++;
    }
   return(nnum);
  }
 /* for reg - can not fail */
 ntyp = __from_vpi_vartyp(typ);
 for (ni = 0; ni < onnum; ni++, np++)
  { 
   if (np->n_isarr) continue;
   if (np->ntyp == ntyp) nnum++;
  }
 return(nnum);
}


/*
 * build the iterator for initial-always - 1-to-many selector is vpi process
 * but handle has type of object itself
 *
 * the object here is the ialst not the one statement
 */
static vpiHandle bld_initalw_iterator(struct h_t *hp)
{
 register struct ialst_t *ialp;
 register int32 iai;
 register struct hrec_t *hrp2;
 int32 ianum;
 vpiHandle ihref;
 struct h_t *hp2;
 struct mod_t *mdp;
 struct itree_t *itp;
 struct pviter_t *iterp;

 if (hp == NULL) return(__nil_iter_err(vpiProcess));
 if (hp->hrec->htyp != vpiModule)
  { mustbe_inmoditer_err(vpiProcess, hp); return(NULL); }
 itp = hp->hin_itp;
 mdp = itp->itip->imsym->el.emdp;
 if (mdp->ialst == NULL) return(NULL); 
 for (ianum = 0, ialp = mdp->ialst; ialp != NULL; ialp = ialp->ialnxt)
  ianum++;
 /* DBG remove - checking again to maybe catch corrupted memory */
 if (ianum <= 0) return(NULL); 

 iterp = __alloc_iter(ianum, &ihref);
 for (iai = 0, ialp = mdp->ialst; iai < ianum; iai++, ialp = ialp->ialnxt)
  {
   hp2 = &(iterp->scanhtab[iai]);
   hrp2 = hp2->hrec;
   if (ialp->iatyp == ALWAYS) hrp2->htyp = vpiAlways;
   else hrp2->htyp = vpiInitial;
   hrp2->hu.hialstp = ialp;
   hp2->hin_itp = itp;
  }
 return(ihref);
}

/*
 * build the iterator scan table for module contas
 * only called if at least one conta and no other 1-to-many access path
 */
static vpiHandle bld_conta_iterator(struct h_t *hp)
{
 register int32 cai, gi;
 register struct h_t *hp2;
 register struct hrec_t *hrp2;
 int32 numcas, num1bcas;
 vpiHandle ihref;
 struct conta_t *cap;
 struct gate_t *gp;
 struct mod_t *mdp;
 struct itree_t *itp;
 struct pviter_t *iterp;

 if (hp == NULL) return(__nil_iter_err(vpiContAssign));
 if (hp->hrec->htyp != vpiModule)
  { mustbe_inmoditer_err(vpiContAssign, hp); return(NULL); }
 mdp = hp->hin_itp->itip->imsym->el.emdp;

 itp = hp->hin_itp;
 /* contas in table - does not include 1 bit cas treated as gate */
 numcas = mdp->mcanum;
 num1bcas = 0;
 if (mdp->mod_1bcas)
  {
   for (gi = 0; gi < mdp->mgnum; gi++)
    {
     gp = &(mdp->mgates[gi]);
     if (gp->gmsym->el.eprimp->gateid == G_ASSIGN) num1bcas++;
    }
  }
 if (numcas + num1bcas <= 0) return(NULL);
 iterp = __alloc_iter(numcas + num1bcas, &ihref);
 for (cai = 0, cap = &(mdp->mcas[0]); cai < numcas; cai++, cap++)
  {
   hp2 = &(iterp->scanhtab[cai]);
   hrp2 = hp2->hrec;
   hrp2->htyp = vpiContAssign;
   hrp2->hu.hcap = cap;
   hp2->hin_itp = itp;
  }
 if (mdp->mod_1bcas)
  {
   for (gi = 0; gi < mdp->mgnum; gi++)
    {
     gp = &(mdp->mgates[gi]);
   
     if (gp->gmsym->el.eprimp->gateid != G_ASSIGN) continue;
     hp2 = &(iterp->scanhtab[cai]);
     hrp2 = hp2->hrec;
     hrp2->htyp = vpiContAssign;
     hrp2->htyp2 = vpiGate;
     hrp2->hu.hgp = gp;
     hp2->hin_itp = itp;
     cai++;
    }
  }
 return(ihref);
}

/*
 * build the iterator scan table for gates one scan table mixed the 3
 * types with htyp giving the type
 *
 * notice primitives are all gates, switches, and udps
 */
static vpiHandle bld_gate_iterator(struct h_t *hp)
{
 register struct gate_t *gp;
 register struct h_t *hp2;
 register struct hrec_t *hrp2;
 int32 numgs, gi, hi;
 vpiHandle ihref;
 struct pviter_t *iterp;
 struct mod_t *mdp;

 if (hp == NULL) return(__nil_iter_err(vpiPrimitive));
 if (hp->hrec->htyp != vpiModule)
  { mustbe_inmoditer_err(vpiPrimitive, hp); return(NULL); }

 mdp = hp->hin_itp->itip->imsym->el.emdp;
 if (mdp->mgnum == 0) return(NULL);
 for (numgs = 0, gi = 0; gi < mdp->mgnum; gi++)
  {
   gp = &(mdp->mgates[gi]);
   if (gp->gmsym->el.eprimp->gateid != G_ASSIGN) numgs++;
  }
 /* DBG remove - checking again to maybe catch corrupted memory */
 if (numgs <= 0) return(NULL);
 iterp = __alloc_iter(numgs, &ihref);
 for (gi = 0, hi = 0; gi < mdp->mgnum; gi++)
  { 
   gp = &(mdp->mgates[gi]);
   if (gp->gmsym->el.eprimp->gateid == G_ASSIGN) continue;

   hp2 = &(iterp->scanhtab[hi++]);
   hrp2 = hp2->hrec;
   hrp2->hu.hgp = gp;
   hp2->hin_itp = hp->hin_itp;
   hrp2->htyp = __gate_to_vpiprimtyp(gp);
  }
 return(ihref);
}

/*
 * map from a gate_t to a vpi primitive type (i.e. vpi Switch)
 */
extern word32 __gate_to_vpiprimtyp(struct gate_t *gp)
{
 int32 ptyp;

 switch ((byte) gp->g_class) {
  /* LOOKATME - is pull possible here (think so) */  
  case GC_LOGIC: case GC_BUFIF: case GC_MOS: case GC_CMOS: case GC_PULL:
   ptyp = vpiGate;
   break;
  case GC_UDP: ptyp = vpiUdp; break;
  case GC_TRAN: case GC_TRANIF: ptyp = vpiSwitch; break;
  default: __vpi_terr(__FILE__, __LINE__); return(0);
 }
 return(ptyp);
}

/*
 * build the iterator scan table for mod paths
 * only called if module has at least one path
 */
static vpiHandle bld_modpth_iterator(struct h_t *hp)
{
 register int32 pi;
 register struct spcpth_t *pthp;
 register struct hrec_t *hrp2;
 int32 mpths; 
 vpiHandle ihref;
 struct h_t *hp2;
 struct mod_t *mdp;
 struct pviter_t *iterp;
 
 if (hp == NULL) return(__nil_iter_err(vpiModPath));
 if (hp->hrec->htyp != vpiModule)
  { mustbe_inmoditer_err(vpiModPath, hp); return(NULL); }

 mdp = hp->hin_itp->itip->imsym->el.emdp;
 if (mdp->mspfy == NULL || mdp->mspfy->spcpths == NULL) return(NULL);
 mpths = 0;
 for (pthp = mdp->mspfy->spcpths; pthp != NULL; pthp = pthp->spcpthnxt)
  mpths++;
 /* DBG remove - checking again to maybe catch corrupted memory */
 if (mpths <= 0) return(NULL);

 iterp = __alloc_iter(mpths, &ihref);
 for (pi = 0, pthp = mdp->mspfy->spcpths; pi < mpths; pthp = pthp->spcpthnxt,
  pi++)
  {
   hp2 = &(iterp->scanhtab[pi]);
   hrp2 = hp2->hrec;
   hrp2->htyp = vpiModPath;
   hrp2->hu.hpthp = pthp;
   hp2->hin_itp = hp->hin_itp;
  }
 return(ihref);
}

/*
 * build the iterator scan table for tchks
 *
 * added complication must remove setup with tc setup of setup hold flag on
 * hold has pointer to setup value when needed
 */
static vpiHandle bld_tchk_iterator(struct h_t *hp)
{
 register int32 tci;
 register struct h_t *hp2;
 register struct tchk_t *tcp;
 register struct hrec_t *hrp2;
 int32 ntchks; 
 vpiHandle ihref;
 struct mod_t *mdp;
 struct pviter_t *iterp;

 if (hp == NULL) return(__nil_iter_err(vpiTchk));
 if (hp->hrec->htyp != vpiModule)
  { mustbe_inmoditer_err(vpiTchk, hp); return(NULL); }
 mdp = hp->hin_itp->itip->imsym->el.emdp;
 if (mdp->mspfy == NULL || mdp->mspfy->tchks == NULL) return(NULL);
 for (ntchks = 0, tcp = mdp->mspfy->tchks; tcp != NULL; tcp = tcp->tchknxt)
  {
   if (tcp->tchktyp == TCHK_SETUP && tcp->tc_supofsuphld) continue;
   if (tcp->tchktyp == TCHK_RECOVERY && tcp->tc_recofrecrem) continue;
   ntchks++;
  }
 /* DBG remove - checking again to maybe catch corrupted memory */
 if (ntchks <= 0) return(NULL);

 iterp = __alloc_iter(ntchks, &ihref);
 for (tci = 0, tcp = mdp->mspfy->tchks; tcp != NULL; tcp = tcp->tchknxt)
  {
   /* must remove setup of setuphold since can access from hold side */
   if (tcp->tchktyp == TCHK_SETUP && tcp->tc_supofsuphld) continue;
   if (tcp->tchktyp == TCHK_RECOVERY && tcp->tc_recofrecrem) continue;

   hp2 = &(iterp->scanhtab[tci++]);
   hrp2 = hp2->hrec;
   hrp2->htyp = vpiTchk;
   hrp2->hu.htcp = tcp;
   hp2->hin_itp = hp->hin_itp;
  }
 return(ihref);
}

/*
 * given an object containing params handle - build iterator of all params
 *
 * otype is vpi_ type of net 
 */
static vpiHandle bld_param_iterator(struct h_t *hp, int32 otype)
{
 register struct hrec_t *hrp;
 struct mod_t *mdp;
 struct task_t *tskp;
 struct itree_t *itp; 

 if (hp == NULL)
  {
   return(__nil_iter_err(vpiParameter));
  }

 itp = hp->hin_itp;
 hrp = hp->hrec;
 switch (hrp->htyp) {
  case vpiModule: 
   mdp = hp->hin_itp->itip->imsym->el.emdp;
   if (mdp->mprms == NULL) return(NULL);
   return(bld_listofparams_iter(mdp->mprms, mdp->mprmnum,
    mdp->mlocprms, mdp->mlocprmnum, itp, NULL));
  case vpiTask: case vpiFunction:
   tskp = hrp->hu.htskp;
bld_tskprms:
   if (tskp->tsk_prms == NULL) return(NULL);
   return(bld_listofparams_iter(tskp->tsk_prms, tskp->tprmnum,
    tskp->tsk_locprms, tskp->tlocprmnum, itp, tskp));
  case vpiNamedBegin: case vpiNamedFork:
   tskp = hrp->hu.htskp;
   goto bld_tskprms;
  default:
   __vpi_err(1851, vpiError,
    "unable to construct iterator of contained %s for %s object",
    __to_vpionam(__wrks1, (word32) otype), __to_vpionam(__wrks2, hrp->htyp));
 }
 return(NULL);
}

/*
 * build the iterator scan table for parameters (all nets of mprms)
 *
 * LOOKATME - why was mprms not freed? - needed for here
 * SJM 09/28/06 - simplified since no more param arrays
 */
static vpiHandle bld_listofparams_iter(struct net_t *nptab, int32 nparams,
 struct net_t *loc_nptab, int32 loc_nparams, struct itree_t *itp, 
 struct task_t *tskp)
{
 register int32 pi, iti;
 register struct hrec_t *hrp;
 int32 tot_nparams;
 vpiHandle ihref;
 struct h_t *hp;
 struct net_t *np;
 struct pviter_t *iterp;

 tot_nparams = nparams + loc_nparams;
 if (tot_nparams == 0) return(NULL);
 /* because iter by array class may have params but 0 iterator size */
 if (tot_nparams <= 0) return(NULL);

 iterp = __alloc_iter(tot_nparams, &ihref);
 for (pi = 0, iti = -1; pi < nparams; pi++)
  {
   np = &(nptab[pi]);
   hp = &(iterp->scanhtab[++iti]);
   hrp = hp->hrec;
   hrp->htyp = vpiParameter;
   hrp->hu.hnp = &(nptab[pi]);
   hp->hin_itp = itp;
   hrp->hin_tskp = tskp;
  }
 /* SJM 09/27/06 - local params go into iterator after normal */
 for (pi = 0; pi < loc_nparams; pi++)
  {
   np = &(loc_nptab[pi]);
   hp = &(iterp->scanhtab[++iti]);
   hrp = hp->hrec;
   hrp->htyp = vpiParameter;
   hrp->hu.hnp = &(loc_nptab[pi]);
   hp->hin_itp = itp;
   hrp->hin_tskp = tskp;
  }

 return(ihref);
}

/*
 * build the iterator scan table for specparam (all nets)
 *
 * only called if module has at least one specparam
 * this does not need parent since one in module - can get from itp  
 */
static vpiHandle bld_specparam_iterator(struct h_t *hp)
{
 register int32 pi;
 register struct h_t *hp2;
 register struct hrec_t *hrp2;
 int32 nsparams; 
 vpiHandle ihref;
 struct pviter_t *iterp;
 struct mod_t *mdp;
 struct spfy_t *spfyp;
 struct hrec_t *hrp;

 if (hp == NULL) return(__nil_iter_err(vpiSpecParam));
 hrp = hp->hrec;
 if (hrp->htyp != vpiModule)
  { mustbe_inmoditer_err(vpiSpecParam, hp); return(NULL); }

 mdp = hp->hin_itp->itip->imsym->el.emdp;
 if ((spfyp = mdp->mspfy) == NULL) return(NULL);
 if (spfyp->msprms == NULL) return(NULL);

 nsparams = spfyp->sprmnum;
 if (nsparams <= 0) return(NULL); 
 iterp = __alloc_iter(nsparams, &ihref);
 for (pi = 0; pi < spfyp->sprmnum; pi++)
  {
   hp2 = &(iterp->scanhtab[pi]);
   hrp2 = hp2->hrec;
   hrp2->htyp = vpiSpecParam;
   hrp2->hu.hnp = &(spfyp->msprms[pi]);
   hp2->hin_itp = hp->hin_itp;
  }
 return(ihref);
}

/*
 * build a defparam statement iterator
 *
 * no one master list - must match itree location of module handle
 */
static vpiHandle bld_defparam_stmt_iterator(struct h_t *hp)
{
 register int32 dfi;
 register struct h_t *hp2;
 register struct hrec_t *hrp2;
 register struct dfparam_t *dfp;
 int32 ndfps; 
 vpiHandle ihref;
 struct pviter_t *iterp;
 struct itree_t *itp;

 if (hp == NULL) return(__nil_iter_err(vpiDefParam));
 if (hp->hrec->htyp != vpiModule)
  { mustbe_inmoditer_err(vpiDefParam, hp); return(NULL); }

 if (__dfphdr == NULL) return(NULL);

 for (ndfps = 0, dfp = __dfphdr; dfp != NULL; dfp = dfp->dfpnxt)
  {
   itp = __find_dfpbot_itp(dfp);
   if (itp == hp->hin_itp) ndfps++;
  }
 if (ndfps <= 0) return(NULL);
 iterp = __alloc_iter(ndfps, &ihref);
 for (dfi = 0, dfp = __dfphdr; dfp != NULL; dfp = dfp->dfpnxt)
  {
   itp = __find_dfpbot_itp(dfp);
   if (itp != hp->hin_itp) continue;

   hp2 = &(iterp->scanhtab[dfi]);
   hrp2 = hp2->hrec;
   hrp2->htyp = vpiDefParam;
   hrp2->hu.hdfp = dfp;
   hp2->hin_itp = hp->hin_itp;
   dfi++;
  }
 return(ihref);
}
