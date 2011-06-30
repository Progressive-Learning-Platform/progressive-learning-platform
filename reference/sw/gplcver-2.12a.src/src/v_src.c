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
 * source reading of module items except for specify and udps
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#ifdef __DBMALLOC__
#include "../malloc.h"
#endif

#include "v.h"
#include "cvmacros.h"

/* local prototypes */
static void decl_instconns(struct mod_t *);
static void dcl_iconn_wires(struct cell_t *, struct expr_t *);
static void freeze_mod_syms(struct symtab_t *, struct symtab_t *);
static void travfreeze_syms(register struct tnode_t *);
static void travfreeze_lowsymtab(register struct symtab_t *);
static void bld_mdpin_table(struct mod_t *);
static int32 rd_modhdr(struct mod_t *);
static int32 rd_hdrpnd_parmdecls(void);
static int32 rd_portref(void);
static void set_ioprtnets(struct expr_t *);
static int32 rd_list_of_ports_decl(struct mod_t *);
static int32 rd_modbody(void);
static int32 rd_iodecl(word32);
static void add_net_attr(struct net_t *, int32);
static int32 rd_vardecl(word32);
static void chk_capwdecl_strens(word32);
static void chk_drvstren(word32);
static int32 rd_oparamdels(struct paramlst_t **);
static int32 do_wdecl_assgn(struct sy_t *, struct paramlst_t *, int32);
static int32 rdbld_mod_varinitlst(struct sy_t *);
static int32 is_decl_err(struct sy_t *, word32, word32);
static void set_reg_widths(word32, struct expr_t **, struct expr_t **);
static int32 chkset_wdrng(struct net_t *, struct expr_t *, struct expr_t *);
static int32 cmp_rng(struct expr_t *, struct expr_t *, struct expr_t *,
 struct expr_t *);
static int32 rd_verstrens(void);
static int32 rd_1verstren(int32 *);
static int32 is_tokstren(int32);
static int32 rd_opt_param_array_rng(struct expr_t **, struct expr_t **,
 int32);
static struct net_t *chkadd_array_param(char *, int32, int32, int32,
 struct expr_t *, struct expr_t *, struct expr_t *, struct expr_t *);
static void cnvt_to_pdecl(struct xstk_t *, struct expr_t *, struct net_t *,
 char *);
static void unwind_param_array_constructor(struct expr_t *);
static void chk1_arrinit_expr(struct expr_t *, char *, int32);
static int32 rd_contassign(void);
static struct conta_t *add_conta(struct expr_t *, struct expr_t *, int32,
 int32);
static int32 rd_eventdecl(int32);
static int32 rd_paramdecl(int32, int32);
static int32 rd_dfparam_stmt(void);
static struct dfparam_t *alloc_dfpval(void);
static int32 rd_task(void);
static word32 to_tasksytyp(int32);
static int32 rd_taskvardecl(word32, int32, char *);
static struct net_t *decl_taskvar(word32, struct expr_t *, struct expr_t *);
static struct task_pin_t *alloc_tskpin(void);
static int32 rd_func(void);
static void add_funcretdecl(char *, word32, struct expr_t *,
 struct expr_t *, int32);
static int32 rd_inst(char *);
static void add_cell_attr(struct cell_t *);
static int32 rd_pull_stren(char *, int32 *);
static struct namparam_t *rd_npndparams(void);
static struct namparam_t *rd1_namedparam(void);
static struct namparam_t *copy_namparamlst(struct namparam_t *);  
static int32 rd_iports(char *);
static int32 rd_cpin_conn(void);
static char *alloc_cpnam(char *);
static struct cell_pin_t *alloc_memcpins(void);
static struct cell_t *add_cell(char *);
static struct cell_t *alloc_memcell(void);
static void init_task(struct task_t *);
static int32 rd_tf_list_of_ports_decl(struct task_t *, char *);

/* extern prototypes (maybe defined in this module) */
extern char *__pv_stralloc(char *);
extern char *__my_malloc(int32);
extern struct mod_t *__alloc_mod(struct sy_t *);
extern void __init_mod(struct mod_t *, struct sy_t *);
extern int32 __xpr_has_param(struct expr_t *);
extern struct ncomp_t *__alloc_arrncomp(void);
extern char *__prt_kywrd_vtok(void);
extern char *__prt_vtok(void);
extern void __freeze_1symtab(struct symtab_t *);
extern struct sy_t *__get_sym_env(char *);
extern struct sy_t *__get_sym(char *, struct symtab_t *);
extern struct sy_t *__decl_sym(char *, struct symtab_t *);
extern struct sy_t *__bld_loc_symbol(int32, struct symtab_t *, char *,
 char *);
extern struct sy_t *__find_sym(char *);
extern struct net_t *__add_net(struct sy_t *);
extern struct net_t *__decl_wirereg(word32, struct expr_t *,
 struct expr_t *, struct sy_t *);
extern struct sy_t *__add_modsym(char *);
extern char *__to_ptnam(char *, word32);
extern char *__to_wtnam(char *, struct net_t *);
extern char *__to_wtnam2(char *, word32);
extern char *__to_wrange(char *, struct net_t *);
extern char *__get_vkeynam(char *, int32);
extern char *__to_sytyp(char *, word32);
extern char *__msgtox_wrange(char *, struct expr_t *, struct expr_t *);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern char *__pregab_tostr(char *, word32 *, word32 *, struct net_t *);
extern char *__to_stren_nam(char *, int32, int32);
extern word32 __fr_stren_nam(int32);
extern struct symtab_t *__alloc_symtab(int32);
extern struct mod_pin_t *__alloc_modpin(void);
extern struct tnode_t *__vtfind(char *, struct symtab_t *);
extern struct namparam_t *__alloc_namparam(void);
extern struct paramlst_t *__copy_dellst(struct paramlst_t *);
extern struct expr_t *__bld_rng_numxpr(word32, word32, int32);
extern struct st_t *__alloc_stmt(int32);
extern struct st_t *__alloc2_stmt(int32, int32, int32);
extern struct delctrl_t *__alloc_dctrl(void);
extern char *__to_splt_nam(char *, int32);
extern struct st_t *__rd_stmt(void);
extern struct paramlst_t *__alloc_pval(void);
extern struct task_t *__alloc_task(struct sy_t *);
extern struct expr_t *__gen_wireid_expr(struct sy_t *);
extern char *__bld_lineloc(char *, word32, int32);
extern struct xstk_t *__eval2_xpr(struct expr_t *);
extern struct xstk_t *__src_rd_eval_xpr(struct expr_t *);
extern word32 __to_cap_size(int32);
extern struct expr_t *__copy_expr(struct expr_t *);
extern struct expr_t *__alloc_newxnd(void);
extern void __get_vtok(void);
extern int32 __rd_moddef(struct symtab_t *, int32);
extern struct gref_t *__alloc_grtab(struct gref_t *, int32);
extern int32 __rd_udpdef(struct symtab_t *);
extern int32 __vskipto_modend(int32);
extern int32 __vskipto_lofp_end(void);
extern int32 __vskipto2_lofp_end(void);
extern void __add_sym(char *, struct tnode_t *);
extern int32 __rd_decl_rng(struct expr_t **, struct expr_t **);
extern int32 __chk_redef_err(char *, struct sy_t *, char *, word32);
extern void __remove_undef_mod(struct sy_t *);
extern void __my_free(char *, int32);
extern int32 __vskipto2_any(int32, int32);
extern int32 __vskipto3_any(int32, int32, int32);
extern int32 __vskipto4_any(int32, int32, int32, int32);
extern void __unget_vtok(void);
extern int32 __col_parenexpr(int32);
extern int32 __col_connexpr(int32);
extern void __bld_xtree(int32);
extern int32 __rd_spfy(struct mod_t *);
extern int32 __fr_wtnam(int32);
extern int32 __vskipto_any(int32);
extern int32 __col_rangeexpr(void);
extern int32 __is_capstren(int32);
extern int32 __col_comsemi(int32);
extern void __set_numval(struct expr_t *, word32, word32, int32);
extern int32 __col_lval(void);
extern int32 __col_newparamrhsexpr(void);
extern int32 __col_lofp_paramrhsexpr(void);
extern int32 __bld_tsk(char *, int32);
extern int32 __rd_tfdecls(char *);
extern int32 __bld_expnode(void);
extern void __set_xtab_errval(void);
extern int32 __col_delexpr(void);
extern int32 __vskipto3_modend(int32, int32, int32);
extern void __set_opempty(int32);
extern void __free_xtree(struct expr_t *);
extern void __free2_xtree(struct expr_t *);
extern int32 __src_rd_chk_paramexpr(struct expr_t *, int32);
extern int32 __rd_opt_param_vec_rng(struct expr_t **, struct expr_t **,
 int32);
extern int32 __chk_paramexpr(struct expr_t *, int32);
extern void __eval_param_rhs_tonum(struct expr_t *);
extern int32 __nd_ndxnum(struct expr_t *, char *, int32);
extern struct net_t *__add_param(char *, struct expr_t *, struct expr_t *,
 int32);
extern void __init_stmt(struct st_t *, int32);

extern int32 __expr_has_glb(struct expr_t *);
extern int32 __isleaf(struct expr_t *);
extern struct expr_t *__dup_concat(int32, struct expr_t *);
extern struct expr_t *__find_catend(struct expr_t *);
extern void __free_namedparams(struct namparam_t *);
extern struct cell_t *__alloc_cell(struct sy_t *);
extern struct cell_pin_t *__alloc_cpin(int32);
extern void __add_syp_to_undefs(struct sy_t *);
extern void __src_rd_cnv_stk_fromreg_toreal(struct xstk_t *, int32);
extern void __src_rd_cnv_stk_fromreal_toreg32(struct xstk_t *);
extern void __sizchgxs(struct xstk_t *, int32);  
extern void __narrow_sizchg(register struct xstk_t *, int32);
extern void __sizchg_widen(register struct xstk_t *, int32);
extern void __sgn_xtnd_widen(struct xstk_t *, int32);
extern int32 __wide_vval_is0(register word32 *, int32);
extern int32 __allocfill_cval_new(word32 *, word32 *, int32);
extern int32 __alloc_shareable_cval(word32, word32, int32);
extern int32 __alloc_shareable_rlcval(double);
extern char *__to1_stren_nam(char *, int32, int32);
extern void __push_wrkitstk(struct mod_t *, int32);
extern void __pop_wrkitstk(void);
extern int32 __cmp_xpr(struct expr_t *, struct expr_t *);
extern struct attr_t *__rd_parse_attribute(struct attr_t *);
extern void __cnv_stk_fromreg_toreal(struct xstk_t *, int32);
extern void __cnv_stk_fromreal_toreg32(struct xstk_t *);


extern void __cv_msg(char *, ...);
extern void __crit_msg(char *, ...);
extern void __pv_ferr(int32, char *, ...);
extern void __pv_fwarn(int32, char *, ...);
extern void __gfinform(int32, word32, int32, char *, ...);
extern void __finform(int32, char *, ...);
extern void __dbg_msg(char *, ...);
extern void __pv_terr(int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);
extern void __misc_fterr(char *, int32);

/*
 * read one top level module or udp definition
 * expects a keyword to have been read and reads end of module
 *
 * upon return current token must be endmodule/primitive or eof or
 * one before module/primitive
 */
extern void __rd_ver_mod(void)
{
 switch ((byte) __toktyp) {
  case TEOF: return;
  case MACROMODULE:
   __get_vtok();
   __finform(423, "macromodules not expanded - %s translated as module",
    __token);
   goto rd_def;
  case MODULE:
   /* know these will either die with eof or have read end mod/prim */
   /* or have skipped on error so next token read is mod/prim */
   __get_vtok();
rd_def:
   __rd_moddef(NULL, FALSE);
   break;
  case PRIMITIVE:
   /* because library must check for unresolved, get name before call */
   __get_vtok();
   __rd_udpdef(NULL);
   break;
  case GENERATE:
   __pv_ferr(3549, "generate feature not implemented in this version");
   __vskipto_modend(ENDGENERATE); 
   break;
  default:
   __pv_ferr(975, "module or primitive keyword expected - %s read",
    __prt_vtok());
   /* for common extra ;, will move to module and back up 1 */
   /* otherwise will skip to eof */  
   __vskipto_modend(ENDMODULE);
 }
}

/*
 * cfg form of read ver mod 
 *
 * SJM FIXME ??? - why is this different version needed 
 */
extern void __rd_cfg_ver_mod(void)
{
 switch ((byte) __toktyp) {
  case TEOF: return;
  case MACROMODULE:
   __get_vtok();
   __finform(423, "macromodules not expanded - %s translated as module",
    __token);
   __get_vtok();
   break;
  case MODULE:
   /* know these will either die with eof or have read end mod/prim */
   /* or have skipped on error so next token read is mod/prim */
   __get_vtok();
   break;
  case PRIMITIVE:
   /* because library must check for unresolved, get name before call */
   __get_vtok();
   break;
  default:
   __pv_ferr(975, "module or primitive keyword expected - %s read",
    __prt_vtok());
   /* for common extra ;, will move to module and back up 1 */
   /* otherwise will skip to eof */  
 }
}

/*
 * MODULE DEFINITION ROUTINES
 */

/*
 * read a module definition
 * 
 * if reading config, put module name in passed config lib el sym table 
 * upon return current token must be synced to file level token
 * return F if parse errors, T even if other errors
 */
extern int32 __rd_moddef(struct symtab_t *cfg_sytab, int32 isconfig)
{
 struct sy_t *syp;
 struct symtab_t *sp_sytp;
 struct mod_t *mdp;
 struct tnode_t *tnp;

 __lofp_port_decls = FALSE;
 /* DBG remove --- */
 if (__top_sti != 0) __misc_terr(__FILE__, __LINE__);
 if (__inst_mod != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 /* these are couner for use in building object names */
 __cp_num = __conta_num = 1;

 /* notice that Verilog keywords are reserved words */
 if (__toktyp != ID)
  {
no_read:
   __pv_ferr(976, "module name expected - %s read", __prt_kywrd_vtok());
   __vskipto_modend(ENDMODULE);
   return(FALSE);
  }
 
 /* SJM 01/07/04 - if module in config library file, put into its mod sy tab */
 if (cfg_sytab != NULL) 
  {
   tnp = __vtfind(__token, cfg_sytab);
   /* dups already checked for */
   /* DBG remove -- */
   if (!__sym_is_new) __misc_terr(__FILE__, __LINE__);
   /* --- */
   __add_sym(__token, tnp);
   (cfg_sytab->numsyms)++;
   syp = tnp->ndp;
  }
 else syp = __add_modsym(__token);

 if (syp == NULL) goto no_read;
 syp->cfg_needed = FALSE;
 syp->sytyp = SYM_M;

 mdp = __alloc_mod(syp);
 syp->el.emdp = mdp;
 /* AIV 05/24/04 - need to set flag to get rid of highest level mods */
 /* that are scanned but never used in configs */
 mdp->m_inconfig = isconfig;
 /* this is where module definition actually resolved */
 syp->sydecl = TRUE;
 /* need place where udp declared */
 syp->syfnam_ind = __cur_fnam_ind;
 syp->sylin_cnt = __lin_cnt;

 /* if saw attribute before reading latest module keyword save */
 /* as string - evaled during pass 2 fixup */ 
 if (__wrk_attr.attr_seen)
  {
   /* here only one string possible - on serious error returns nil */
   /* if returns non nil, at least some attr_specs good */
   mdp->mattrs = __rd_parse_attribute(&__wrk_attr);
   /* SJM 07/30/01 - this is work read value, but now done with it */
   __my_free(__wrk_attr.attrnam, __attr_line_len + 1);
   __wrk_attr.attr_seen = FALSE;
  } 
 __push_wrkitstk(mdp, 0);

 /* must also allocate the new symbol table */
 __inst_mod->msymtab = __alloc_symtab(TRUE);
 /* module symbol table always outermost */
 __inst_mod->msymtab->sytpar = NULL;
 /* link symbol table back to module symbol */
 __inst_mod->msymtab->sypofsyt = syp;

 /* set list ends for elements that must be kept in order */
 __end_cp = NULL; __end_tbp = NULL; 
 __end_paramnp = __end_loc_paramnp = NULL;
 __end_ca = NULL; __end_ialst = NULL; __end_dfp = NULL;
 __end_impparamnp = NULL;
 __end_mod_varinitlst = NULL; 
 __cur_declobj = MODULE;
 __mod_specparams = 0;
 /* initialize symbol table stack so module on bottom */
 /* replaces previous module symbol table so no need to pop here */
 /* system names now in separate symbol table and separated by $ prefix */
 __top_sti = 0;
 __venviron[0] = __inst_mod->msymtab;

 /* do not need to build type until entire module read */
 /* if these return F, know needed to skip to file level mod/prim */
 if (!rd_modhdr(__inst_mod)) goto bad_end;
 if (!rd_modbody()) goto bad_end;

 /* if error will not get linked in - this quarentees source order */
 if (__end_mdp == NULL) __modhdr = __inst_mod;
 else __end_mdp->mnxt = __inst_mod;
 __end_mdp = __inst_mod;

 /* current module now always one root of symbol table tree */
 /* declare wire names used in inst. conns. as 1 bit wires */
 decl_instconns(__inst_mod);
 /* here may have no specparams but symbol because of syntax error */
 /* in common attempt to use defparams as specparams error */
 if (__inst_mod->mspfy != NULL && __inst_mod->mspfy->spfsyms != NULL)
  sp_sytp = __inst_mod->mspfy->spfsyms;
 else sp_sytp = NULL;
 freeze_mod_syms(__inst_mod->msymtab, sp_sytp);
 bld_mdpin_table(__inst_mod);

 /* copy wrk gr table to module's gr table */ 
 if (__grwrknum > 0)
  {
   __inst_mod->mgrtab = __alloc_grtab(__grwrktab, __grwrknum);
   __inst_mod->mgrnum = __grwrknum;
   __grwrknum = 0; 
  }

 __last_libmdp = __inst_mod;
 __pop_wrkitstk();
 return(TRUE);

bad_end:
 /* need to free storage so no inst. will find this type */
 /* assuming could not stay synced while reading */
 if (__mod_specparams > 0) sp_sytp = __inst_mod->mspfy->spfsyms;
 else sp_sytp = NULL;
 freeze_mod_syms(__inst_mod->msymtab, sp_sytp);
 /* make undeclared */
 syp->sydecl = FALSE;
 __pop_wrkitstk();
 return(TRUE);
}

/*
 * add the module name symbol to symbol table
 */
extern struct sy_t *__add_modsym(char *nam)
{
 struct tnode_t *tnp;
 struct sy_t *syp;

 tnp = __vtfind(nam, __modsyms);
 /* this is define before use in source file case */
 if (__sym_is_new)
  {
   __add_sym(nam, tnp);
   (__modsyms->numsyms)++;
   syp = tnp->ndp;
  }
 else
  {
   /* path impossible for copy mod splitting */
   syp = tnp->ndp;
   if (!__chk_redef_err(nam, syp, "module", SYM_M)) return(NULL);
   /* chk fail means never in module undef list */
   __remove_undef_mod(syp);
  }
 syp->sytyp = SYM_M;
 return(syp);
}

/*
 * remove a module from the undef list and count
 * module now resolved
 */
extern void __remove_undef_mod(struct sy_t *syp)
{
 struct undef_t *undefp;

 /* repeated use of "module [name]" with no definition will cause */
 /* lots of syntax error but also will have no module to remove */
 if (!syp->syundefmod) return;

 /* DBG remove -- */
 if (syp->sydecl) __misc_terr(__FILE__, __LINE__);
 /* --- */
 undefp = syp->el.eundefp;
 __undef_mods--; 
 /* case 1, removing head */ 
 if (undefp == __undefhd)
  {
   if (undefp == __undeftail)
    { 
     __undefhd = __undeftail = NULL;
     if (__undef_mods != 0) __misc_terr(__FILE__, __LINE__);
    }
   else { __undefhd = undefp->undefnxt; __undefhd->undefprev = NULL; }
  }
 /* case 2, removing tail - know at least 2 elements */
 else if (undefp == __undeftail)
  {
   /* SJM 06/03/2002 - added extra fields for 64 bit clean - no sharing */
   __undeftail = undefp->undefprev;
   __undeftail->undefnxt = NULL;
  }
 /* case 3, removing internal */
 else
  {
   undefp->undefprev->undefnxt = undefp->undefnxt;
   undefp->undefnxt->undefprev = undefp->undefprev;
  }
 syp->syundefmod = FALSE;
 syp->el.eundefp = NULL;
}

/*
 * allocate a new module
 */
extern struct mod_t *__alloc_mod(struct sy_t *syp)
{
 struct mod_t *mdp;

 mdp = (struct mod_t *) __my_malloc(sizeof(struct mod_t));
 __init_mod(mdp, syp);
 return(mdp);
}

/*
 * allocate a new module
 */
extern void __init_mod(struct mod_t *mdp, struct sy_t *syp)
{
 mdp->msym = syp;
 mdp->mod_last_lini = -1;
 mdp->mod_last_ifi = -1;
 mdp->msymtab = NULL;
 mdp->mod_cfglbp = NULL; 
 mdp->minstnum = 0;
 mdp->mhassts = FALSE;
 mdp->msplit = FALSE;
 mdp->mpndsplit = FALSE;
 mdp->mhassplit = FALSE;
 mdp->mgone = FALSE;
 mdp->msplitpth = FALSE;
 mdp->mwiddetdone = FALSE;
 mdp->mhas_widthdet = FALSE;
 mdp->mhas_indir_widdet = FALSE;
 mdp->mgiarngdone = FALSE;
 mdp->mpndprm_ingirng = FALSE;
 mdp->mpnd_girng_done = FALSE;
 mdp->mhasisform = FALSE;
 /* default is 1 ns.(time unit) with 0 digits percision */
 mdp->mtime_units = __cur_units;
 /* normally just round (0 frac. digits of precision) */
 /* module precision is mtime units plus mtime units */
 mdp->mtime_prec = __cur_prec;
 mdp->mno_unitcnv = FALSE;
 if (__in_cell_region) { mdp->m_iscell = TRUE; __design_has_cells = TRUE; }
 else mdp->m_iscell = FALSE;
 mdp->m_hascells = FALSE;
 mdp->mod_hasdvars = FALSE;
 mdp->mod_dvars_in_src = FALSE;
 mdp->mod_dumiact = FALSE;
 mdp->mod_rhs_param = FALSE;
 mdp->mod_hasbidtran = FALSE;
 mdp->mod_hastran = FALSE;
 mdp->mod_gatetran = FALSE;
 mdp->mod_1bcas = FALSE;
 mdp->mod_has_mipds = FALSE;
 mdp->mod_parms_gd = FALSE;
 mdp->mod_lofp_decl = FALSE;
 /* values not used by cver but set so vpi_ routines can return */
 mdp->mod_dfltntyp = (word32) __dflt_ntyp;
 mdp->mod_uncdrv =  (word32) __unconn_drive;
 mdp->mhas_frcassgn = FALSE;
 mdp->flatinum = 0;
 mdp->mpnum = 0;
 mdp->miarr = NULL; 
 mdp->mpins = NULL;
 mdp->mgates = NULL;
 mdp->mgarr = NULL;
 mdp->minum = 0;
 mdp->mgnum = 0;
 mdp->mcas = NULL;
 mdp->mcanum = 0;
 mdp->minsts = NULL;
 mdp->miarr = NULL;
 mdp->mnets = NULL;
 mdp->mnnum = 0;
 mdp->mtotvarnum = 0;
 mdp->mprms = NULL;
 mdp->mlocprms = NULL;
 mdp->mprmnum = 0;
 mdp->mlocprmnum = 0;
 mdp->moditps = NULL;
 mdp->mnxt = NULL;

 mdp->mattrs = NULL;
 mdp->mvarinits = NULL;
 mdp->mgateout_cbs = NULL;

 mdp->ialst = NULL;
 mdp->mtasks = NULL;

 mdp->mexprtab = NULL;
 mdp->mexprnum = 0;

 mdp->msttab = NULL;
 mdp->mstnum = 0;

 mdp->mgrtab = NULL;
 mdp->mgrnum = 0;

 mdp->mspfy = NULL;
 mdp->mndvcodtab = NULL;
 /* LOOKATME - could convert to compile time only struct */
 mdp->mversno = 0;
 mdp->lastinum = 0;
 mdp->mlpcnt = -1;
 mdp->mlevnxt = NULL;
 mdp->mspltmst = NULL;
 mdp->mpndspltmst = NULL;
 mdp->mcells = NULL;
 mdp->smpins = NULL;
 mdp->iploctab = NULL;
 mdp->mdfps = NULL;
}

/*
 * NET LIST FIX UP ROUTINES CALLED DURING INPUT PROCESSING
 */

/*
 * declare all undeclared wires connected to module insts and gates
 */
static void decl_instconns(struct mod_t *mdp)
{
 register struct cell_t *cp;
 register struct cell_pin_t *cpp;

 /* at this point all mod. insts., and gates on cell list */
 /* SJM 03/25/99 - all gate ports including control must be declared imp. */ 
 for (cp = mdp->mcells; cp != NULL; cp = cp->cnxt)
  {
   if (cp->cmsym == NULL) continue;
   
   for (cpp = cp->cpins; cpp != NULL; cpp = cpp->cpnxt)
    {
     /* this should always be at least 'bx by here */
     /* change to special unc. indicator and check/fix here */
     /* cell port connections lost */
     if (cpp->cpxnd == NULL) __misc_terr(__FILE__, __LINE__);
    
     dcl_iconn_wires(cp, cpp->cpxnd);
    }
  }
}

/*
 * declare all undeclared wires mentioned in inst. conns implicitly
 * anything in inst. port expr. implictly declared if not declared
 * even index of bit select and everything in concatenate
 */
static void dcl_iconn_wires(struct cell_t *cp, struct expr_t *ndp)
{
 switch ((byte) ndp->optyp) {
  case NUMBER: case REALNUM: case OPEMPTY: break;
  /* global are not declared in this module */
  case GLBREF: break;
  case ID:
   {
    struct net_t *np;
    struct sy_t *syp;

    syp = ndp->lu.sy;
    if (syp->sydecl || syp->sytyp != SYM_N) break;

    np = syp->el.enp;
    /* must not implicitly declare I/O port */
    if (np->iotyp != NON_IO) break;
    syp->sydecl = TRUE;
    syp->sy_impldecl = TRUE;
    syp->syfnam_ind = cp->csym->syfnam_ind;
    syp->sylin_cnt = cp->csym->sylin_cnt;
    np->iotyp = NON_IO;
    /* type for undeclared is default net types */
    np->ntyp = __dflt_ntyp;
    np->nu.ct->n_iotypknown = TRUE;
    np->nu.ct->n_rngknown = FALSE;
    np->nu.ct->n_impldecl = TRUE;
    np->nu.ct->n_wirtypknown = TRUE;
    __gfinform(419, syp->syfnam_ind, syp->sylin_cnt,
     "%s %s implicitly declared here from use in instance or gate connection",
      __to_wtnam(__xs, np), np->nsym->synam);
   }
   break;
  default:
   /* know will not get here unless operator */
   if (ndp->lu.x != NULL) dcl_iconn_wires(cp, ndp->lu.x);
   if (ndp->ru.x != NULL) dcl_iconn_wires(cp, ndp->ru.x);
   break;
 }
}

/*
 * freeze module symbols and all enclosed symbol tables
 * treee form of tables free during fixup by freeing the tn blocks
 * notice sp_sytp can be nil but not sytp
 */
static void freeze_mod_syms(struct symtab_t *sytp,
 struct symtab_t *sp_sytp)
{
 /* since only system tasks and interpretive level symbols in level 0 */
 /* do not look for global there */
 sytp->sytpar = NULL;

 if (sytp->numsyms == 0) sytp->stsyms = NULL;
 else __freeze_1symtab(sytp);

 /* if needed also freeze specify specparam symbol table */
 if (sp_sytp != NULL)
  {
   /* symbol table empty if spec params all numbers - works since */
   /* back annotation is to slot not spec param */
   if (sp_sytp->numsyms == 0) sp_sytp->stsyms = NULL;
   else __freeze_1symtab(sp_sytp);
  }
 /* notice no top level symbol but contained symbols possible */
 if (sytp->sytofs != NULL) travfreeze_lowsymtab(sytp->sytofs);
}

/*
 * freeze one non empty symbol table
 */
extern void __freeze_1symtab(struct symtab_t *sytp)
{
 int32 bytes;

 /* DBG remove */
 if (!sytp->freezes) __misc_terr(__FILE__, __LINE__);
 /* --- */

 bytes = sytp->numsyms*sizeof(struct sy_t *);
 __wrkstab = (struct sy_t **) __my_malloc(bytes);
 __last_sy = -1;
 travfreeze_syms(sytp->n_head);
 sytp->stsyms = __wrkstab;
 sytp->n_head = NULL;
 /* non mod symbol table size wrong */
 if (__last_sy + 1 != sytp->numsyms) __misc_terr(__FILE__, __LINE__);
}

/*
 * traversal in sorted preorder
 */
static void travfreeze_syms(register struct tnode_t *tnp)
{
 if (tnp->lp != NULL) travfreeze_syms(tnp->lp);
 __wrkstab[++__last_sy] = tnp->ndp;
 if (tnp->rp != NULL) travfreeze_syms(tnp->rp);
}

/*
 * depth first symbol table tree traversal across offspring
 */
static void travfreeze_lowsymtab(register struct symtab_t *sytp)
{
 for (; sytp != NULL; sytp = sytp->sytsib)
  {
   if (sytp->numsyms == 0) sytp->stsyms = NULL;
   else __freeze_1symtab(sytp);
   if (sytp->sytofs != NULL) travfreeze_lowsymtab(sytp->sytofs);
  }
}

/*
 * convert list of module ports to array of ptrs to module ports
 * need so port can be accessed from its index
 * port order is list order that came from header list of ports
 */
static void bld_mdpin_table(struct mod_t *mdp)
{
 register int32 pi;
 register struct mod_pin_t *mpp;
 int32 pnum;
 struct mod_pin_t *mphdr, *mpp2;

 if ((pnum = mdp->mpnum) == 0) return;
 mphdr = (struct mod_pin_t *) __my_malloc(pnum*sizeof(struct mod_pin_t));
 for (pi = 0, mpp = mdp->mpins; mpp != NULL; mpp = mpp->mpnxt, pi++)
  {
   mphdr[pi] = *mpp;
   mphdr[pi].mpnxt = NULL;
  }
 /* now free all old ports - contents copied - and need to keep expr */
 for (mpp = mdp->mpins; mpp != NULL;) 
  {
   mpp2 = mpp->mpnxt;
   __my_free((char *) mpp, sizeof(struct mod_pin_t));
   mpp = mpp2;
  }
 mdp->mpins = mphdr;
}

/*
 * check for and emit a redefinition error
 */
extern int32 __chk_redef_err(char *nam, struct sy_t *syp,
 char *newtnam, word32 styp)
{
 if (!syp->sydecl)
  {
   /* when see mod or udp, assume mod - may turn out to be udp and no err */
   if (syp->sytyp == SYM_M && styp == SYM_UDP) return(TRUE);
  }

 if (syp->sytyp != styp)
  {
   __pv_ferr(977, "cannot redefine %s as a %s - previous type was %s at %s",
    nam, newtnam, __to_sytyp(__xs, syp->sytyp), __bld_lineloc(__xs2,
    syp->syfnam_ind, syp->sylin_cnt));
   return(FALSE);
  }
 if (syp->sydecl)
  {
   __pv_ferr(978, "cannot redefine %s %s - previous definition %s",
    newtnam, nam, __bld_lineloc(__xs, syp->syfnam_ind, syp->sylin_cnt));
   return(FALSE);
  }
 return(TRUE);
}

/*
 * process an the module header iolist
 * must read leading ( or (empty ;) and reads trailing ;
 *
 * if return T, even if error parsing can continue in module
 * sometimes guesses that continuing ok, error caught by next routine
 */
static int32 rd_modhdr(struct mod_t *mp)
{
 struct mod_pin_t *mpp, *last_mpp;

 /* empty I/O list legal */
 __get_vtok();

 /* P1364 2001 allows #(list of normal parameter decls) here that allows */
 /* param decls in addition to body param decls */
 if (__toktyp == SHARP)
  {
   /* if error and sync failed, know synced to module level item */
   if (!rd_hdrpnd_parmdecls()) goto ret_end;
  }
 
 if (__toktyp == SEMI) return(TRUE);
 /* norma end reads ending rpar but on error mayhave synced to lpar */
 if (__toktyp != LPAR) __get_vtok();

 if (__toktyp != LPAR)
  {
   __pv_ferr(979, 
    "module header list of ports initial left parenthesis expected - %s read",
    __prt_vtok());
   if (__vskipto2_any(RPAR, SEMI)) 
    {
     if (__toktyp == RPAR) __get_vtok();
     /* if not semi, assume semi left out - if bad, next rout. will catch */
     if (__toktyp != SEMI) __unget_vtok();
     return(TRUE);
    }
ret_end:
   switch ((byte) __syncto_class) {
    case SYNC_FLEVEL: return(FALSE);
    case SYNC_MODLEVEL: return(TRUE);
    /* should never sync to statement up here */
    case SYNC_STMT:
     __vskipto_modend(ENDMODULE);
     return(FALSE);
    default: __case_terr(__FILE__, __LINE__);
   }
  }
 __get_vtok();
 if (__toktyp == RPAR)
  {
do_end:
   __get_vtok();
do_end2:
   if (__toktyp == SEMI) return(TRUE);

   __pv_ferr(980,
    "module header list of ports end semicolon expected - %s read",
    __prt_vtok());
   __unget_vtok();
   return(TRUE);
  }

 /* SJM 05/23/04 - branch point for separate list of port decl header form */
 if (__toktyp == INPUT || __toktyp == OUTPUT || __toktyp == INOUT)
  {
   /* reads ending ; after ) - also sets flags that prevents further */
   /* port decls that would be legal for port name (explicit too) forms */
   if (!rd_list_of_ports_decl(mp)) goto ret_end;
   if (__toktyp == RPAR) __get_vtok();
   goto do_end2;
  }

 for (last_mpp = NULL;;)
  {
   /* this declares the symbols in the header */

   /* SJM 08/30/00 - need to ignore ,, and not count as a port in hdr def. */
   if (__toktyp == COMMA)
    {
     __pv_fwarn(3103,
      "empty ,, in module header port definition list ignored - not used in ordered instance connection list"); 
     goto nxt_port;
    }
   if (__toktyp == RPAR)
    {
     __pv_fwarn(3103,
      "empty ,) in module header port definition list ignored"); 
     goto do_end;
    } 

   if (!rd_portref())
    {
     /* on error ignore this port and move on to next */
do_resync:
     if (__vskipto3_any(COMMA, SEMI, RPAR)) 
      {
       /* port effectively not seen - error emitted already */
       if (__toktyp == COMMA) goto nxt_port;
       if (__toktyp == RPAR) goto do_end;
       break;
      }
     goto ret_end;
    }

   /* assume unvectored 1 bit port */
   mpp = __alloc_modpin();
   /* think possiblity unnamed ports can appear here */
   if (strcmp(__portnam, "") == 0) mpp->mpsnam = NULL;
   else mpp->mpsnam = __pv_stralloc(__portnam);
   mpp->mp_explicit = (__mpref_explicit) ? TRUE : FALSE;

   if (last_mpp == NULL) mp->mpins = mpp; else last_mpp->mpnxt = mpp;
   last_mpp = mpp;
   mpp->mpref = __root_ndp;
   /* count number of ports */
   (mp->mpnum)++;
   if (mp->mpnum >= MAXNUMPORTS)
    {
     __pv_ferr(314, "INTERNAL FATAL - module has more than %d ports - contact Pragmatic C",
      MAXNUMPORTS);
     (mp->mpnum)--; 
    }

   if (__toktyp == RPAR) goto do_end;
   if (__toktyp != COMMA)
    {
     __pv_ferr(984,
      "module header comma expected - %s read", __prt_vtok());
     goto do_resync;
    }
nxt_port:
   __get_vtok();
  }
 return(TRUE);
}

/*
 * read the module define #(param decl list) parameter delcarations
 * know the leading # read and reads one past ending ')' (probably '(')
 *
 * format is: module xx #([parameter decl list], ...) (port list) ...
 * notice that only legal for module definitions
 */
static int32 rd_hdrpnd_parmdecls(void)
{
 __get_vtok();
 if (__toktyp != LPAR)
  {
   __pv_ferr(984,
    "module header #([parameter decl], ..) form starting left paren expected - %s read",
    __prt_vtok());
   if (!__vskipto3_any(RPAR, LPAR, SEMI)) return(FALSE);
   return(TRUE);
  }
 __get_vtok();
 for (;;)
  {
   if (__toktyp == RPAR) return(TRUE);
   if (__toktyp != PARAMETER)
    {
     if (!__vskipto4_any(PARAMETER, COMMA, RPAR, SEMI)) return(FALSE);
     if (__toktyp == PARAMETER) continue;
     if (__toktyp == COMMA) { __get_vtok(); continue; }
     if (__toktyp == RPAR || __toktyp == SEMI) return(TRUE);
    }

   /* AIV 09/27/06 - can never be a local param here */
   if (!rd_paramdecl(TRUE, FALSE)) return(FALSE);
  }
}

/*
 * read module header port .[port name]([port expr.]) or [port expr.] form
 * know 1st token name read and reads one past end , or ) if no error
 * if no error, expression in root_ndp
 *
 * medium level - caller syncs if returns error F
 */
static int32 rd_portref(void)
{
 int32 nd_rpar;

 if (__toktyp == DOT)
  {
   __mpref_explicit = TRUE;
   __get_vtok();
   if (__toktyp != ID)
    {
     __pv_ferr(985, "module definition header name of port expected - %s read",
      __prt_kywrd_vtok());
     return(FALSE);
    }
   strcpy(__portnam, __token);
   __get_vtok();
   if (__toktyp != LPAR)
    {
     __pv_ferr(986,
     "module definition header .[port]([port expr.]) form left parenthesis expected - %s read",
      __prt_vtok());
     return(FALSE);
    }
   nd_rpar = TRUE;
   /* read collect the expression that must end with ) only */
   __get_vtok();
   /* this cannot be empty */
   if (__toktyp == RPAR)
    { 
     __finform(3004,
      "empty module definition header explicit form () port expression no effect");
     __last_xtk = 0;
     __set_opempty(0);
    }
   else 
    {
     if (!__col_parenexpr(-1)) return(FALSE);
    }
  }
 else
  {
   __mpref_explicit = FALSE;
   nd_rpar = FALSE;
   strcpy(__portnam, "");
   /* read/collect expression that must end with ) or , */
   /* know 1st token of expression read */
   if (!__col_connexpr(-1)) return(FALSE);
  }
 /* build the tree, copy/allocate nodes, sets root_ndp to its root */
 __bld_xtree(0);
 /* must set all net like things as IO ports */
 set_ioprtnets(__root_ndp);

 if (!nd_rpar)
  {
   struct expr_t *idx;

   /* for unnamed port is simple id, that is port name */
   if (__root_ndp->optyp == ID) idx = __root_ndp;
   else if (__root_ndp->optyp == LSB || __root_ndp->optyp == PARTSEL)
    idx = __root_ndp->lu.x;
   else idx = NULL;
   if (idx != NULL) strcpy(__portnam, idx->lu.sy->synam);
  }
 else 
  {
   if (__toktyp != RPAR)
    {
     __pv_ferr(988,
     "module definition header named port form right parenthesis expected - %s read",
      __prt_vtok());
     return(FALSE);
    }
   __get_vtok();
  }
 return(TRUE);
}

/*
 * mark all nets in I/O port expression as I/O of unknown direction
 * can be arbitrary expressions here because not yet checked 
 */
static void set_ioprtnets(struct expr_t *ndp)
{
 struct sy_t *syp;

 if (__isleaf(ndp))
  {
   if (ndp->optyp == ID)
    {
     syp = ndp->lu.sy;
     /* module header wire %s declaration inconsistent */
     /* DBG remove -- */
     if (syp->sytyp != SYM_N)
      {
       __pv_ferr(684,
        "I/O port definition %s name %s type %s illegal - must be a net",
        __msgexpr_tostr(__xs, ndp), syp->synam, __to_sytyp(__xs2, syp->sytyp));
       syp->sytyp = SYM_N;
      }
     /* --- */
     syp->el.enp->iotyp = IO_UNKN;
    }
   return;
  }
 if (ndp->lu.x != NULL) set_ioprtnets(ndp->lu.x);
 if (ndp->ru.x != NULL) set_ioprtnets(ndp->ru.x);
}

/*
 * allocate a module pin (port) element
 */
extern struct mod_pin_t *__alloc_modpin(void)
{
 struct mod_pin_t *mpp;

 mpp = (struct mod_pin_t *) __my_malloc(sizeof(struct mod_pin_t));
 mpp->mpsnam = NULL;
 mpp->mptyp = IO_UNKN;
 /* gets set and used only for bidirects */ 
 mpp->mp_explicit = FALSE;
 mpp->mp_jmpered = FALSE;
 mpp->inout_unc = FALSE;
 mpp->assgnfunc_set = FALSE;
 mpp->has_mipd = FALSE;
 mpp->has_scalar_mpps = FALSE;

 /* assume 1 for prim */
 mpp->mpwide = 1;
 /* expression for .[name]({...}) form but usually same internal net */
 mpp->mpref = NULL;
 mpp->mpattrs = NULL;
 mpp->mpfnam_ind = __cur_fnam_ind;
 mpp->mplin_cnt = __lin_cnt;
 mpp->mpaf.mpp_upassgnfunc = NULL;
 mpp->pbmpps = NULL;
 mpp->mpnxt = NULL;
 return(mpp);
}

/*
 * ROUTINES TO READ AND ADD LIST OF PORTS STYLE HEADER PORT DECLATIONS 
 */

/*
 * read list of header port declarations
 * new alternative ANSII style port header decl form added to 2001 LRM
 *
 * first port type keyword read and reads ending );
 *
 * if return T, even if error parsing can continue in module
 * on error must sync to semi and back up one - mod item which just returns
 */
static int32 rd_list_of_ports_decl(struct mod_t *mp)
{
 int32 first_time, wtyp, ptyp, attr_ttyp, has_attr, decl_signed;
 struct sy_t *syp;
 struct net_t *np;
 struct expr_t *x1, *x2, *ox1, *ox2;
 struct mod_pin_t *mpp, *last_mpp;
 char s1[RECLEN];

 ptyp = -1;
 /* even if syntax error, T once a port type keyword appears in hdr */
 mp->mod_lofp_decl = TRUE;
 __lofp_port_decls = TRUE;

 last_mpp = NULL;
 for (;;)
  {
   /* attribute collected by scanner - but need to save so associates with */
   /* right port */
   if (__attr_prefix)
    {
     __wrk_attr.attr_tok = __toktyp;
     __wrk_attr.attr_seen = TRUE;
     /* for now this is unparsed entire attr. string */
     __wrk_attr.attrnam = __pv_stralloc(__attrwrkstr);
     __wrk_attr.attr_fnind = __attr_fnam_ind;
     __wrk_attr.attrlin_cnt = __attr_lin_cnt;
    }
   else __wrk_attr.attr_seen = FALSE;

   attr_ttyp = __toktyp;
   if (__toktyp == INPUT) ptyp = IO_IN;
   else if (__toktyp == OUTPUT) ptyp = IO_OUT;
   else if (__toktyp == INOUT) ptyp = IO_BID;
   else __case_terr(__FILE__, __LINE__); 

   __get_vtok();

   /* defaults to wire if net type omitted - can be var/reg type */
   if ((wtyp = __fr_wtnam(__toktyp)) != -1) __get_vtok();
   else wtyp = N_WIRE;

   if (wtyp == N_INT || wtyp == N_REAL) decl_signed = TRUE;
   else decl_signed = FALSE;

   /* vectored or scalared keywords never appear in port decls */
   if (__toktyp == SIGNED)
    {
     decl_signed = TRUE;
     if (wtyp == N_TIME || wtyp == N_INT || wtyp == N_REAL || wtyp == N_EVENT) 
      {
       __pv_ferr(3423,
        "signed keyword illegal when task or function variable type %s",
        __to_wtnam2(s1, wtyp));
      }
     __get_vtok();
    }

   /* even if error if 1 past ending ] continue */
   if (!__rd_decl_rng(&ox1, &ox2))
    {
     /* bad decl - but if sync to new I/O port direction, caller will cont */
     if (!__vskipto_lofp_end()) return(FALSE); 
     if (__toktyp == RPAR) { __get_vtok(); return(TRUE); } 
     /* semi read */
     return(TRUE);
    }

   /* use local has attr flag so can turn glb seen off before return */
   if (__wrk_attr.attr_seen)
    { has_attr = TRUE; __wrk_attr.attr_seen = FALSE; }
   else has_attr = FALSE;

   x1 = x2 = NULL;
   for (first_time = TRUE;;)
    {
     if (__toktyp != ID)
      {
       __pv_ferr(992, "list of port form %s port name expected - %s read",
        __to_ptnam(s1, ptyp), __prt_kywrd_vtok());

       if (__vskipto2_lofp_end())
        {
         if (__toktyp == SEMI) return(TRUE);
         if (__toktyp == RPAR) { __get_vtok(); return(TRUE); }
         /* only other possibility is the port name separating comma */
         continue;
        }
       /* can't recover (resync) from error - synced to module item */
       return(FALSE);
      }

     if ((syp = __get_sym_env(__token)) != NULL)
      {
       __pv_ferr(3418, "list of port form %s port name %s redeclared", 
        __to_ptnam(s1, ptyp), __token);
       goto nxt_port;
      }

     if (first_time) { x1 = ox1; x2 = ox2; first_time = FALSE; } 
     else
      {
       if (x1 == NULL) x1 = x2 = NULL;
       else { x1 = __copy_expr(ox1); x2 = __copy_expr(ox2); }
      }

     /* first declare the port's wire/reg */
     if ((np = __decl_wirereg(wtyp, x1, x2, NULL)) == NULL) goto nxt_port;

     /* if previously used will be treated as reg - must set to compatible */
     /* wire type if declared as time or int32 */
     np->ntyp = wtyp;
     syp = np->nsym;

     /* if saw an (* *) attribute for module item token, seen on */
     if (has_attr)
      {
       /* this add to net's attr list on end if also net decl first */
       add_net_attr(np, attr_ttyp);
      }

     /* SJM 10/02/03 - signed can be turned on either in port or wire decl */ 
     if (decl_signed) np->n_signed = TRUE;

     np->iotyp = ptyp;
     /* for list of ports mod header decl form, all info in hdr decl */ 
     np->nu.ct->n_iotypknown = TRUE;

     syp->sydecl = TRUE;
     /* need I/O decl. place not header or wire decl. */
     syp->syfnam_ind = __cur_fnam_ind;
     syp->sylin_cnt = __lin_cnt;

     /* then add the port to the port list - know port and net name same */
     mpp = __alloc_modpin();
     mpp->mpsnam = __pv_stralloc(np->nsym->synam);
     mpp->mp_explicit = FALSE;

     if (last_mpp == NULL) mp->mpins = mpp; else last_mpp->mpnxt = mpp;
     last_mpp = mpp;

     /* name of port still in token - expr here always ID */
     __last_xtk = -1;
     if (!__bld_expnode()) __set_xtab_errval();
     __bld_xtree(0);
     mpp->mpref = __root_ndp;

     /* count number of ports */
     (mp->mpnum)++;
     if (mp->mpnum >= MAXNUMPORTS)
      {
       __pv_ferr(314,
        "INTERNAL FATAL - module has more than %d ports - contact Pragmatic C",
        MAXNUMPORTS);
       (mp->mpnum)--; 
      }

nxt_port:
     __get_vtok();
     if (__toktyp == RPAR) return(TRUE);

     if (__toktyp != COMMA)
      {
       __pv_ferr(995,
        "list of ports form declaration list comma or right paren expected - %s read",
        __prt_vtok());
       /* try to resync */
       if (!__vskipto_lofp_end()) return(FALSE); 
       if (__toktyp == COMMA) goto nxt_net;
       /* misplaced semi or sync to rpar */
       return(TRUE);
      }
nxt_net:
     __get_vtok();
     if (__toktyp == INPUT || __toktyp == OUTPUT || __toktyp == INOUT)  
      break;
    }
  }
 __misc_terr(__FILE__, __LINE__);
 return(TRUE);
}

/*
 * MODULE ITEM ROUTINES
 */

/*
 * read module body and process the various module items
 * know __inst_mod points to current module structure
 * know module port list end ; read
 *
 * if returns T must be synced on end mod 
 */
static int32 rd_modbody(void)
{
 int32 rv, iattyp, ialcnt, iafnind, wtyp;
 struct st_t *stp;
 struct ialst_t *ialp;
 char typnam[IDLEN];

 for (;;)
  {
   /* routines called in switch expected to read ending ; or token */
   __get_vtok();
   /* SJM 03/20/00 - save attribute info for mod items */
   if (__attr_prefix)
    {
     __wrk_attr.attr_tok = __toktyp;
     __wrk_attr.attr_seen = TRUE;
     /* for now this is unparsed entire attr. string */
     __wrk_attr.attrnam = __pv_stralloc(__attrwrkstr);
     __wrk_attr.attr_fnind = __attr_fnam_ind;
     __wrk_attr.attrlin_cnt = __attr_lin_cnt;
    }
   else __wrk_attr.attr_seen = FALSE;

   switch((byte) __toktyp) {
    case TEOF:
     __pv_ferr(989, "endmodule missing");
     return(FALSE);
    case INPUT:
     if (!rd_iodecl(IO_IN))
      {
moditem_resync:
       /* on error - reset attribute prefix state */
       __wrk_attr.attr_seen = FALSE;
       switch ((byte) __syncto_class) {
        case SYNC_FLEVEL: return(FALSE);
        case SYNC_MODLEVEL: break;
	/* if sync. to statement assume initial left out */
        case SYNC_STMT:
         /* here must clear any pushed back */
         if (__lasttoktyp != UNDEF) __get_vtok(); 
	 /*FALLTHRU */
        case SYNC_TARG:
	 iattyp = INITial;
         iafnind= __cur_fnam_ind;
         ialcnt = __lin_cnt;
	 goto moditem_stmt;
        default: __case_terr(__FILE__, __LINE__);
       }
      }
     continue;
    case OUTPUT:
     if (!rd_iodecl(IO_OUT)) goto moditem_resync;
     continue;
    case INOUT:
     if (!rd_iodecl(IO_BID)) goto moditem_resync;
     continue;
    case EVENT:
     if (!rd_eventdecl(FALSE)) goto moditem_resync;
     continue;
    case INITial:
    case ALWAYS:
     iafnind = __cur_fnam_ind;
     ialcnt = __lin_cnt;
     iattyp = __toktyp;
     __get_vtok();
     /* statement must synchronize */
moditem_stmt:
     if ((stp = __rd_stmt()) != NULL)
      {
       ialp = (struct ialst_t *) __my_malloc(sizeof(struct ialst_t));
       ialp->iatyp = iattyp;
       ialp->iastp = stp;
       ialp->ia_first_ifi = iafnind;
       ialp->ia_first_lini = ialcnt;
       ialp->ia_last_ifi = __cur_fnam_ind;
       ialp->ia_last_lini = __lin_cnt;
       ialp->ialnxt = NULL;

       if (__end_ialst == NULL) __inst_mod->ialst = ialp;
       else __end_ialst->ialnxt = ialp;
       __end_ialst = ialp;
      }
     else goto moditem_resync;
     continue;
    case ASSIGN:
     if (!rd_contassign()) goto moditem_resync;
     continue;
    case PARAMETER:
     if (!rd_paramdecl(FALSE, FALSE)) goto moditem_resync;
     continue;
    case LOCALPARAM:
     if (!rd_paramdecl(FALSE, TRUE)) goto moditem_resync;
     continue;
    case DEFPARAM:
     if (!rd_dfparam_stmt()) goto moditem_resync;
     continue;
    case SPECIFY:
     if (!__rd_spfy(__inst_mod)) goto moditem_resync;
     continue;
    case TASK:
     __cur_declobj = TASK;
     rv = rd_task();
     __cur_declobj = MODULE;
     if (!rv) goto moditem_resync;
     continue;
    case FUNCTION:
     __cur_declobj = TASK;
     rv = rd_func();
     __cur_declobj = MODULE;
     if (!rv) goto moditem_resync;
     continue;
    case ENDMODULE:
     /* save end so can put in module's end souce range fields */
     __inst_mod->mod_last_lini = __lin_cnt;
     __inst_mod->mod_last_ifi = __cur_fnam_ind;

     /* catch common extra ; error here */
     __get_vtok();
     if (__toktyp == SEMI)
      __pv_ferr(999, "semicolon following endmodule illegal"); 
     else __unget_vtok();
     break;
    case ENDPRIMITIVE:
     /* but assume still in sync */
     __pv_ferr(990, "module definitition cannot end with endprimitive");
     break;
    case GENERATE:
     /* AIV 06/27/05 - catch generate */
     __pv_ferr(3549, "generate feature not implemented in this version");
     if (!__vskipto_modend(ENDGENERATE)) break;
     continue;
    default:
     if ((wtyp = __fr_wtnam(__toktyp)) != -1)
      {
       /* false here means out of sync - must skip rest of module */
       /* if T will have skipped to semi */
       if (!rd_vardecl((word32) wtyp)) goto moditem_resync;
       /* needed to add attribute to every net in list - can now reset */
       __wrk_attr.attr_seen = FALSE;
       continue;
      }
     /* must be instance (udp, gate, module instantiation) */
     if (__toktyp != ID)
      {
       __pv_ferr(991, "module type, gate or udp name expected - %s read",
        __prt_kywrd_vtok());
       /* can only sync to ; here - else need names to decl. */
       if (!__vskipto_any(SEMI)) goto moditem_resync;
       break;
      }
     strcpy(typnam, __token);
     if (!rd_inst(typnam)) goto moditem_resync;
     /* needed to add attr to every instance, now can reset attr seen */ 
     __wrk_attr.attr_seen = FALSE;
     continue;
   }
   break;
  }
 return(TRUE);
}

/*
 * MODULE DECLARATION ROUTINES
 */

/*
 * process an I/O or wire declaration
 * mostly sets the iotyp field
 * I/O decl. does not declare symbol even though wire decl. not required
 */
static int32 rd_iodecl(word32 typ)
{
 int32 first_time, ttyp, has_attr, decl_signed, is_complete, wtyp;
 struct sy_t *syp;
 struct net_t *np;
 struct expr_t *x1, *x2, *ox1, *ox2;
 char s1[RECLEN];

 wtyp = -1;
 decl_signed = FALSE;
 is_complete = FALSE;
 /* vectored or scalared keywords only appear on wire decls */
 __get_vtok();
 if (__toktyp == SIGNED)
  {
   decl_signed = TRUE;
   __get_vtok();
  }
 /* AIV 07/20/04 port decl can now contain net type making it complete  */
 else if ((wtyp = __fr_wtnam(__toktyp)) != -1)
  {
    /* if complete set the flag set the type */
    is_complete = TRUE;
    __get_vtok();
    /* must check for sign again */
    if (__toktyp == SIGNED)
     {
      decl_signed = TRUE;
      __get_vtok();
     }
   }
  /* type defaults to reg if undefined */
  else wtyp = N_REG;

 /* even if error if 1 past ending ] continue */
 if (!__rd_decl_rng(&ox1, &ox2))
  {
   /* ignore decl but continue with mod. item */
   if (!__vskipto2_any(SEMI, RSB)) return(FALSE); 
   if (__toktyp == SEMI) return(TRUE);
   __get_vtok();
  }
 /* use local has attr flag so can turn glb seen of before return */
 if (__wrk_attr.attr_seen) { has_attr = TRUE; __wrk_attr.attr_seen = FALSE; }
 else has_attr = FALSE;
 x1 = x2 = NULL;
 for (first_time = TRUE;;)
  {
   if (__toktyp != ID)
    {
     __pv_ferr(992, "%s port name expected - %s read", __to_ptnam(s1, typ),
      __prt_kywrd_vtok());
     /* need token and symbol and cannot parse */
     return(__vskipto_any(SEMI));
    }

   /* any port decl illegal if hdr list of port form used */
   if (__lofp_port_decls)
    {
     __pv_ferr(3421,
      "%s declaration of \"%s\" illegal - module uses list of ports declaration form",
     __to_ptnam(s1, typ), __prt_kywrd_vtok());
     goto nxt_port; 
    }

   /* 3 ways to not be in I/O port header list */
   /* know only one symbol level here */
   /* also know must be defined since added when header read */
   if ((syp = __get_sym_env(__token)) == NULL)
    {
not_a_port:
     __pv_ferr(993,
     "%s declaration of \"%s\" illegal - not in module header list of ports",
      __to_ptnam(s1, typ), __token);
     goto nxt_port;
    }
   if (syp->sytyp != SYM_N) goto not_a_port;
   np = syp->el.enp;

   /* SJM 10/02/03 - signed can be turned on either in port or wire decl */ 
   if (decl_signed) np->n_signed = TRUE;

   /* when module header list of ports read, port set to IO_UNKN */
   if (np->iotyp == NON_IO) goto not_a_port;

   /* header port changed when I/O port decl. seen */
   if (np->nu.ct->n_iotypknown)
    {
     __pv_ferr(994, "I/O port %s previously declared as %s", __token,
      __to_ptnam(s1, np->iotyp));
    }
   else
    {
     np->iotyp = typ;
     np->nu.ct->n_iotypknown = TRUE;
    }

   /* if saw an (* *) attribute for module item token, seen on */
   if (has_attr)
    {
     if (typ == IO_IN) ttyp = INPUT; else if (typ == IO_OUT) ttyp = OUTPUT;
     else ttyp = INOUT;

     /* this add to net's attr list on end if also net decl first */
     add_net_attr(np, ttyp);
    }

   if (first_time) { x1 = ox1; x2 = ox2; first_time = FALSE; } 
   else
    {
     if (x1 == NULL) x1 = x2 = NULL;
     else { x1 = __copy_expr(ox1); x2 = __copy_expr(ox2); }
    }
   if (!chkset_wdrng(np, x1, x2)) goto nxt_port;

   /* 2 cases, if so far only appeared in port header wirtypknown F */
   /* and need to set as wire not reg (default for ports), else already set */
   /* but notice if set, type still not known just implicitly wire */
   if (is_complete)
    {
     np->ntyp = wtyp;
     /* must make as  */
     np->nu.ct->n_iscompleted = TRUE;
    }
   else if (!np->nu.ct->n_wirtypknown) np->ntyp = N_WIRE;

   syp->sydecl = TRUE;
   /* need I/O decl. place not header or wire decl. */
   syp->syfnam_ind = __cur_fnam_ind;
   syp->sylin_cnt = __lin_cnt;

nxt_port:
   __get_vtok();
   if (__toktyp == SEMI) break;
   if (__toktyp != COMMA)
    {
     __pv_ferr(995,
      "I/O port declaration list comma or semicolon expected - %s read",
      __prt_vtok());
     /* try to resync */
     if (!__vskipto2_any(COMMA, SEMI)) return(FALSE); 
     if (__toktyp == COMMA) goto nxt_var;
     break;
    }
nxt_var:
   __get_vtok();
  }
 return(TRUE);
}

/*
 * read a decl range (either reg, wire, array, or gate/inst)
 * know possible for range present [ read and reads one past end ]
 * this fills exprs that are evaluated later
 *
 * on error caller handles skipping
 * x1 and x2 only point to non nil if succeeds
 */
extern int32 __rd_decl_rng(struct expr_t **x1, struct expr_t **x2)
{
 *x1 = *x2 = NULL;

 if (__toktyp == LSB) 
  {
   if (!__col_rangeexpr())
    {
     /* error, but structure right */
     if (__toktyp == RSB) { __get_vtok(); return(TRUE); }
     return(FALSE);
    }
   __bld_xtree(0);
   __get_vtok();
   /* this really is number range */
   if (__root_ndp->optyp != PARTSEL)
    __pv_ferr(998, "illegal declaration range expression");
   else { *x1 = __root_ndp->ru.x->lu.x; *x2 = __root_ndp->ru.x->ru.x; }
  }
 return(TRUE);
}

/*
 * add a net attribute 
 * SJM - 03/20/00 - if I/O decl attrs really net attrs for port net
 */
static void add_net_attr(struct net_t *np, int32 ttyp)
{
 register struct attr_t *attrp, *new_attrp, *last_attrp;

 /* need to set token type so each parsed attr_spec has right tok type */
 __wrk_attr.attr_tok = ttyp;
 
 /* return nil on error */
 if ((new_attrp = __rd_parse_attribute(&__wrk_attr)) != NULL)
  { 
   if (np->nattrs == NULL) np->nattrs = new_attrp;
   else
    {
     /* linear search not problem because only 2 decls possible */
     last_attrp = NULL;
     /* move to tail of list */
     for (attrp = np->nattrs; attrp != NULL; attrp = attrp->attrnxt) 
      { last_attrp = attrp; }
     /* SJM 08/02/01 - add if to keep lint happy */
     if (last_attrp != NULL) last_attrp->attrnxt = new_attrp;
    }
  }
 __my_free(__wrk_attr.attrnam, __attr_line_len + 1);
 __wrk_attr.attr_seen = FALSE;
}

/*
 * [wire type] [charge or drive stren] [range] [delay] [list of variables]
 * [charge strength] is ([cap. size])
 */

/*
 * read and process a wire/reg/time/int/real variable declaration
 * know wire type read and reads final semi
 * semantic routines detect errors later
 * tricky because wires can also be continuous assignments
 * capacitor strength indicated by v_stren1 = NO_STREN
 *
 * if returns F synced to next module, else synced to SEMI
 */
static int32 rd_vardecl(word32 wtyp)
{
 int32 first_time, split_state, decl_signed, has_attr;
 struct expr_t *x1, *x2, *ox1, *ox2, *xm1, *xm2;
 struct sy_t *syp;
 struct net_t *np;
 struct paramlst_t *pmphdr;
 char s1[RECLEN];

 pmphdr = NULL;
 __v0stren = __v1stren = NO_STREN;
 if (wtyp == N_INT || wtyp == N_REAL) decl_signed = TRUE;
 else decl_signed = FALSE;
 
 __get_vtok();
 if (__toktyp == LPAR)
  {
   __get_vtok();
   /* normal use of strengths if for wire assign net decl form */
   if (!rd_verstrens())
    {
     if (!__vskipto2_any(RPAR, SEMI)) return(FALSE);
     if (__toktyp == RPAR) { __get_vtok(); goto rd_rng; }
     return(TRUE);
    }
   /* if error strength turned off */
   chk_capwdecl_strens(wtyp);
  }

rd_rng:
 split_state = SPLT_DFLT;
 if (__toktyp == VECTORED) { __get_vtok(); split_state = SPLT_VECT; }
 else if (__toktyp == SCALARED) { split_state = SPLT_SCAL; __get_vtok(); }

 if (__toktyp == SIGNED)
  {
   decl_signed = TRUE;
   if (wtyp == N_TIME || wtyp == N_INT || wtyp == N_REAL || wtyp == N_EVENT) 
    {
     __pv_ferr(3423,
      "signed keyword illegal when variable type %s", __to_wtnam2(s1, wtyp));
    }
   __get_vtok();
  }
 if (!__rd_decl_rng(&ox1, &ox2))
  {
   if (!__vskipto2_any(SEMI, RSB)) return(FALSE); 
   if (__toktyp == SEMI) return(TRUE);
   /* on success this reads one past ] */
   __get_vtok();
  } 
 if (split_state != SPLT_DFLT)
  {
   char s2[RECLEN];

   /* SJM 07/19/02 - notice only one of these errors can be emitted */
   if (wtyp >= NONWIRE_ST)
    {
     __pv_ferr(997,
      "%s keyword illegal for type \"%s\"", __to_splt_nam(s1, split_state),
      __to_wtnam2(s2, wtyp));
     split_state = SPLT_DFLT; 
     ox1 = ox2 = NULL;
    }
   else if (ox1 == NULL)
    {
     __pv_ferr(996, "%s keyword required following range missing",
      __to_splt_nam(s1, split_state));
     split_state = SPLT_DFLT; 
    }
  }
 if (ox1 != NULL)
  {
   if (wtyp == N_INT || wtyp == N_TIME || wtyp == N_REAL || wtyp == N_EVENT)
    {
     __pv_ferr(1002, "%s %s vector range illegal", __to_wtnam2(s1, wtyp),
      __token);
     ox1 = ox2 = NULL;
    }
  }
 /* leave scalar with SPLT_DFLT state */
 /* returning F means must try to skip to semi */ 
 if (__toktyp == SHARP)
  {
   if (!rd_oparamdels(&pmphdr))
    {
bad_end:
     return(__vskipto_any(SEMI));
    } 
  }

 /* use local has attr flag so can turn glb seen of before return */
 if (__wrk_attr.attr_seen) { has_attr = TRUE; __wrk_attr.attr_seen = FALSE; }
 else has_attr = FALSE;
 /* know ox1 and ox2 contain statement decl range */ 
 for (first_time = TRUE;;)
  {
   /* save line count since for conta form need lhs var location */
   if (__toktyp != ID)
    {
     __pv_ferr(1001, "wire or reg declaration wire name expected - %s read",
      __prt_kywrd_vtok());
     /* must move over this token in case it is vendor 1 keyword */
     __get_vtok();
     goto bad_end;
    }
  
  /* if hdr list of port form used, decls giving additional info illegal */
  if ((syp = __get_sym_env(__token)) != NULL && syp->sytyp == SYM_N
   && syp->el.enp->iotyp != NON_IO)
   {
    if (__lofp_port_decls || syp->el.enp->nu.ct->n_iscompleted)
     {
      if (__lofp_port_decls)
       {
        __pv_ferr(3421,
         "%s declaration of port \"%s\" illegal - module uses list of ports declarations form",
         __to_wtnam2(s1, wtyp), __prt_kywrd_vtok());
       }
      else
       {
        __pv_ferr(3424,
         "%s declaration of port \"%s\" illegal - net type is previously defined",
         __to_wtnam2(s1, wtyp), __prt_kywrd_vtok()); 
       }

      /* ignore rest of declaration - should resync if no syntax error */
      if (!__vskipto2_any(COMMA, SEMI)) return(FALSE); 
      if (__toktyp == SEMI) return(TRUE);
      goto nxt_wire;
     }
   }

   /* set implied range for time and integer */
   /* each time through need to call this to make copy */
   if (ox1 == NULL) set_reg_widths(wtyp, &x1, &x2);
   else if (first_time) { x1 = ox1; x2 = ox2; }
   else { x1 = __copy_expr(ox1); x2 = __copy_expr(ox2); }

   /* must skip to ending ; here since cannot decl. any of list */
   /* and lack information to read rest of this decl. */ 
   if ((np = __decl_wirereg(wtyp, x1, x2, NULL)) == NULL)
    {
     /* resync at , or ; - should succeed */
try_resync:
     if (!__vskipto2_any(COMMA, SEMI)) return(FALSE); 
     if (__toktyp == SEMI) return(TRUE);
     goto nxt_wire;
    }
   /* if previously used will be treated as reg - must set to compatible */
   /* wire type if declared as time or int32 */
   np->ntyp = wtyp;
   syp = np->nsym;

   /* SJM - 03/20/00 - save wire decl attrs */
   /* if saw an (* *) attribute for module item token, seen on */
   if (has_attr)
    {
     /* add to end if I/O decl seen first for ports - still on net not port */
     add_net_attr(np, WIRE);
    }

   /* different array range expr. for every array */
   __get_vtok();
   if (!__rd_decl_rng(&xm1, &xm2))
    {
     if (!__vskipto2_any(SEMI, RSB)) return(FALSE); 
     if (__toktyp == SEMI) return(TRUE);
     __get_vtok();
    }
   if (xm1 != NULL && (wtyp < NONWIRE_ST || wtyp == N_EVENT))
    {
     __pv_ferr(1003, "%s %s cannot be an array", __to_wtnam2(s1, wtyp),
      syp->synam);
     xm1 = xm2 = NULL;
    }
   if (pmphdr != NULL && wtyp >= NONWIRE_ST)
    {
     __pv_ferr(1004,
      "%s %s not a wire - cannot have delay(s)", __to_wtnam2(s1, wtyp),
      syp->synam);
     pmphdr = NULL;
    }
   if (xm1 != NULL)
    { np->n_isarr = TRUE; np->nu.ct->ax1 = xm1; np->nu.ct->ax2 = xm2; }
   np->nu.ct->n_spltstate = split_state;

   /* SJM 10/02/03 - now signed keyword or int real implies signed */
   /* must not turn off since if port and turned on there, stays on */
   if (decl_signed) np->n_signed = TRUE;

   /* AIV 09/29/04 - now if there is an = not necessarily a cont assgn */
   /* could be a variable initialization i.e. integer i = 12; */
   if (__toktyp == EQ)
    {
     if (np->ntyp >= NONWIRE_ST)
      {
       /* cannot init an array - illegal syntax */
       if (np->n_isarr)  
        {
         __pv_ferr(3429,
          "variable assign initialize form illegal for %s - arrays cannot be initialized",
          syp->synam); 
         goto try_resync;
        }
       if (!rdbld_mod_varinitlst(syp)) goto try_resync;
      }
     else if (!do_wdecl_assgn(syp, pmphdr, first_time)) goto try_resync;
    }
   else
    {
     /* copying delay list here since gets freed and converted later */
     if (first_time) np->nu.ct->n_dels_u.pdels = pmphdr;
     else np->nu.ct->n_dels_u.pdels = __copy_dellst(pmphdr);
     /* know strength good or will be turned off by here */
     if (__v0stren != NO_STREN)
      {
       if (wtyp == N_TRIREG)
        {
         /* know capacitor strength already checked */
         np->n_capsiz = __to_cap_size(__v0stren);
        } 
       else chk_drvstren(wtyp);
      }
     /* set trireg default to medium (always needed) */
     else if (wtyp == N_TRIREG) np->n_capsiz = CAP_MED;  
    }

nxt_wire:
   if (first_time) first_time = FALSE;
   if (__toktyp == SEMI) break;
   if (__toktyp != COMMA)
    {
     __pv_ferr(1005,
      "wire declaration comma separator or semicolon expected - %s read",
      __prt_vtok());
     /* try to resync */
     if (!__vskipto2_any(COMMA, SEMI)) return(FALSE); 
     if (__toktyp == SEMI) break;
    }
   __get_vtok();
   first_time = FALSE;
  }
 return(TRUE);
}

/*
 * check cap. wire declaration strength - no strength appeared
 */
static void chk_capwdecl_strens(word32 wtyp)
{
 char s1[RECLEN], s2[RECLEN];

 if (!__is_capstren(__v0stren))
  {
   if (wtyp == N_TRIREG)
    {
     __pv_ferr(1006,
      "trireg wire declaration non capacitor size strength %s illegal",
      __to_stren_nam(s1, __v0stren, __v1stren));
     __v0stren = __v1stren = NO_STREN;
    }
   return;
  }
 /* know this is cap. strenght */
 if (wtyp != N_TRIREG)
  {
   __pv_ferr(1007,
   "non trireg wire type %s declaration has illegal capacitor size strength \"%s\"",
    __to_wtnam2(s1, wtyp), __to_stren_nam(s2, __v0stren, __v1stren));
   __v0stren = __v1stren = NO_STREN;
  }
}

/*
 * know non assign wire decl. has strength - emit error
 */
static void chk_drvstren(word32 wtyp)
{
 char s1[RECLEN], s2[RECLEN];

 /* SJM 04/17/03 - must not use stren to string routine if not set in src */
 if (__v0stren == 0 || __v1stren == 0)
  {
   __pv_ferr(1008,
    "%s declaration required tow driving strengths not present in non wire assign form",
    __to_wtnam2(s1, wtyp));
  }
 else
  {
   __pv_ferr(1008,
    "%s declaration driving strength \"%s\" illegal in non wire assign form",
     __to_wtnam2(s1, wtyp), __to_stren_nam(s2, __v0stren, __v1stren));
  }
 __v0stren = __v1stren = NO_STREN;
}


/*
 * get old style only implicit # type parameters i.e. delay expr. list
 *
 * know # read (if needs to be present) and reads one past end
 * builds a parameter/delay list and returns pointer to header
 * this routine for # form and path delay () list (no #) only
 * specparam and deparam rhs no # or ( ok
 * error if #() 
 *
 * this routine returns F on sync error - caller must resync
 * but in places with known delimiter attempt to resync to delim 
 */
static int32 rd_oparamdels(struct paramlst_t **pmphdr)
{
 struct paramlst_t *pmp, *last_pmp;

 *pmphdr = NULL;
 /* this is #[number] or #id - not (..) form - min:typ:max requires () */
 /* for path delay will never see this form */
 __get_vtok();
 if (__toktyp != LPAR)
  {
   /* notice must surround m:t:m with () */
   if (__toktyp != ID && __toktyp != NUMBER && __toktyp != REALNUM)
    {
     __pv_ferr(1049,
     "non parenthesized delay parameter name or number expected - %s read",
      __prt_kywrd_vtok());
     return(FALSE);
    }
   __last_xtk = -1;
   /* on error, set as error expr. - maybe since param should be 0 */
   if (!__bld_expnode()) __set_xtab_errval();
   /* here does the allocating */
   __bld_xtree(0);
   pmp = __alloc_pval();
   pmp->plxndp = __root_ndp;
   pmp->pmlnxt = NULL;
   *pmphdr = pmp;
  }
 else
  {
   /* #(...) form */
   for (last_pmp = NULL;;)
    {
     __get_vtok();
     if (!__col_delexpr())
      {
       if (!__vskipto3_modend(COMMA, RPAR, SEMI)) return(FALSE);
       if (__toktyp == SEMI) return(FALSE);
      }       
     __bld_xtree(0);
     pmp = __alloc_pval();
     pmp->plxndp = __root_ndp;

     /* link on front */
     if (last_pmp == NULL) *pmphdr = pmp; else last_pmp->pmlnxt = pmp;
     last_pmp = pmp;

     if (__toktyp == COMMA) continue;
     if (__toktyp == RPAR) break;
     /* should never happen - sync on err above, if does give up */
     __pv_ferr(1050,
      "delay parameter list comma or right parenthesis expected - %s read",
      __prt_vtok());
     return(FALSE);
    }
  }
 __get_vtok();
 return(TRUE);
}

/*
 * allocate a # style parameter value (also for specify delays)
 */
extern struct paramlst_t *__alloc_pval(void)
{
 struct paramlst_t *pmp;

 pmp = (struct paramlst_t *) __my_malloc(sizeof(struct paramlst_t));
 pmp->plxndp = NULL;
 pmp->pmlnxt = NULL;
 return(pmp);
}

/*
 * process a wire decl. assign
 * notice # delays and strengths are put into globals
 * also notice wire values in this case moved here no wire delay
 */
static int32 do_wdecl_assgn(struct sy_t *syp, struct paramlst_t *pmphdr,
 int32 first_time)
{
 struct conta_t *cap;
 struct expr_t *lhs_ndp;
 int32 sfnind, slcnt;

 /* need lhs wire as location of conta */
 sfnind = syp->syfnam_ind;
 slcnt = __lin_cnt;
 /* wire decl. form continuous assignment */
 /* generate expression from node that is simply wire name */
 lhs_ndp = __gen_wireid_expr(syp);
 /* collect rhs and build expression tree */
 __get_vtok();
 if (!__col_comsemi(-1)) return(FALSE);
 __bld_xtree(0);
 cap = add_conta(lhs_ndp, __root_ndp, sfnind, slcnt);
 /* uses wire decl delay and strength */
 if (first_time) cap->ca_du.pdels = pmphdr;
 else cap->ca_du.pdels = __copy_dellst(pmphdr);
 if (__v0stren != NO_STREN) 
  {
   cap->ca_hasst = TRUE;
   cap->ca_stval = ((__v0stren << 3) | __v1stren) & 0x3f;
  }
 return(TRUE);
}

/*
 * read the initialize to expr and add to mod's var init list 
 * format example: reg r = 12;
 *
 * build the net and expr pair lists here - check to make sure constant
 * expr during fixup and initialize as first sim step in pv_sim
 *
 * notice can't check the constant expr here since parameter decl may 
 * follow in source order
 */
static int32 rdbld_mod_varinitlst(struct sy_t *syp)
{
 struct varinitlst_t *initp;

 /* collect rhs and build expression tree */
 __get_vtok();
 if (!__col_comsemi(-1)) return(FALSE);
 __bld_xtree(0);

 initp = (struct varinitlst_t *) __my_malloc(sizeof(struct varinitlst_t));
 initp->init_syp = syp;
 initp->init_xp = __root_ndp;
 initp->varinitnxt = NULL;

 if (__end_mod_varinitlst == NULL) __inst_mod->mvarinits = initp;
 else __end_mod_varinitlst->varinitnxt = initp;
 __end_mod_varinitlst = initp;

 return(TRUE);
}

/*
 * add the wire type decl. symbol and associated wire/reg
 *
 * caller must set wire type after checking for duplicates
 * for declares only at top level
 * need null ranges for real, width set later or special case
 * returns null on error
 * x1 and x2 passed must be copies for multiple decls in one stmt case 
 *
 * for non vendor 1 specific dsyp will always be nil
 */
extern struct net_t *__decl_wirereg(word32 wtyp, struct expr_t *x1,
 struct expr_t *x2, struct sy_t *dsyp)
{
 struct net_t *np;
 struct sy_t *syp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 /* this find and sets type for already read and processed I/O port decl. */
 syp = __decl_sym(__token, __venviron[0]);
 if (__sym_is_new)
  {
   np = __add_net(syp);
   np->iotyp = NON_IO;
   np->ntyp = wtyp;
   if (x1 != NULL)
    { np->n_isavec = TRUE; np->nu.ct->nx1 = x1; np->nu.ct->nx2 = x2; }
  }
 else
  {
   if (syp->sytyp != SYM_N)
    {
     __pv_ferr(1028,
      "cannot declare %s as %s - previously declared as %s at %s",
      syp->synam, __to_wtnam2(s2, wtyp), __to_wtnam2(s1, syp->sytyp),
      __bld_lineloc(__xs, syp->syfnam_ind, syp->sylin_cnt));
     return(NULL);
    }

   /* wire/reg decl. after I/O decl. may set range */
   np = syp->el.enp;
   /* need special handling for module I/O ports - declared in header, */
   /* I/O direction and maybe wire */
   if (np->iotyp != NON_IO)
    {
     /* if wire decl. for I/O port use it */
     /* any wire type, reg, int32, and time ok here, but not real or event */
     if (wtyp == N_REAL || wtyp == N_EVENT)
      {
       __pv_ferr(1009,
        "%s port %s %s illegal", __to_ptnam(s1, np->iotyp), syp->synam,
        __to_wtnam2(s2, wtyp));
       return(NULL);
      }
     /* this only has meaning for I/O port redecls */
     if (np->nu.ct->n_wirtypknown)
      {
       if (dsyp == NULL)
        {
         __pv_ferr(1010,
          "%s port %s previously declared as %s cannot be %s",
	  __to_ptnam(s1, np->iotyp), syp->synam, __to_wtnam(s2, np),
	  __to_wtnam2(s3, wtyp));
        }
       else
        {
         __pv_ferr(1010,
          "%s port %s previously declared as %s - unknown: %s",
	  __to_ptnam(s1, np->iotyp), syp->synam, __to_wtnam(s2, np),
          dsyp->synam);
        }
       return(NULL);
      }
     np->ntyp = wtyp;
    }
   else
    {
     if (is_decl_err(syp, SYM_N, wtyp)) return(NULL);
     /* must set wire type - may override guessed wire type from use */  
     np->ntyp = wtyp;
    }
   if (!chkset_wdrng(np, x1, x2)) return(NULL);
   np->nu.ct->n_wirtypknown = TRUE;
  }
 /* port header wire, require I/O port dir. decl., else this is decl. */
 if (np->iotyp == NON_IO)
  {
   syp->sydecl = TRUE;
   /* even if used before, must set to declaration place */
   syp->syfnam_ind = __cur_fnam_ind;
   syp->sylin_cnt = __lin_cnt;
  }
 return(np);
}

/*
 * allocate a new net for a symbol that is just seen for first time
 */
extern struct net_t *__add_net(struct sy_t *syp)
{
 struct net_t *np;

 np = (struct net_t *) __my_malloc(sizeof(struct net_t));
 np->nsym = syp;
 /* allocate during compilation part - free and change to storage later */
 np->nrngrep = NX_CT;
 np->nu.ct = (struct ncomp_t *) __alloc_arrncomp();
 /* also initialize */
 np->nu.ct->nx1 = np->nu.ct->nx2 = NULL;
 np->nu.ct->ax1 = np->nu.ct->ax2 = NULL;
 np->nu.ct->n_pb_drvtyp = NULL;
 np->nu.ct->n_drvtyp = DRVR_NONE;
 np->nu.ct->n_dels_u.pdels = NULL;
 np->nu.ct->n_iotypknown = FALSE;
 np->nu.ct->n_wirtypknown = FALSE;
 np->nu.ct->n_rngknown = FALSE;
 np->nu.ct->n_impldecl = FALSE;
 np->nu.ct->n_in_giarr_rng = FALSE;
 np->nu.ct->n_onrhs = FALSE;
 np->nu.ct->n_onlhs = FALSE;
 np->nu.ct->n_2ndonlhs = FALSE;
 np->nu.ct->num_prtconns = 0;
 /* this is default implies word32 a/b type also for reals */
 np->srep = SR_VEC;

 /* fields for parameters only */
 np->nu.ct->n_widthdet = FALSE;
 np->nu.ct->n_indir_widthdet = FALSE;
 np->nu.ct->p_specparam = FALSE;
 np->nu.ct->p_rhs_has_param = FALSE;
 np->nu.ct->p_locparam = FALSE;
 np->nu.ct->p_setby_defpnd = FALSE;
 np->nu.ct->prngdecl = FALSE;
 np->nu.ct->ptypdecl = FALSE;
 np->nu.ct->psigndecl = FALSE;
 np->nu.ct->parm_srep = SR_VEC;
 np->nu.ct->pbase = BNONE;
 np->nu.ct->pstring = FALSE;

 /* init fields for comiled sim */
 np->nu.ct->frc_assgn_in_src = FALSE;
 np->nu.ct->monit_in_src = FALSE;
 np->nu.ct->dmpv_in_src = FALSE;

 /* assumes normal wire e when in header these will be changed */
 np->iotyp = NON_IO;
 np->n_isaparam = FALSE;
 np->n_isavec = FALSE;
 np->nwid = 0;
 np->n_isarr = FALSE;
 /* cap. strength of non cap. strength strong is no cap. strength */
 np->n_capsiz = CAP_NONE;
 np->n_signed = FALSE;
 np->nu.ct->n_iscompleted = FALSE;
 np->nu.ct->n_spltstate = SPLT_DFLT;
 /* this gets sets in v_prep if vector - vectored is scalared */
 np->vec_scalared = TRUE;
 np->n_stren = FALSE;
 np->n_mark = FALSE;
 np->n_multfi = FALSE;
 np->n_isapthsrc = FALSE;
 np->n_isapthdst = FALSE;
 np->n_hasdvars = FALSE;
 np->n_onprocrhs = FALSE;
 np->n_gone = FALSE;
 np->nchg_nd_chgstore = FALSE;
 np->nchg_has_dces = FALSE;
 np->nchg_has_lds = FALSE;
 /* 03/21/01 - these are fields from removed separate optim table */ 
 np->frc_assgn_allocated = FALSE;
 np->dmpv_in_src = FALSE;
 np->monit_in_src = FALSE;
 np->n_onrhs = FALSE;
 np->n_onlhs = FALSE;
 np->n_drvtyp = DRVR_NONE;
 np->dcelst = NULL;
 np->ndrvs = NULL;
 np->nlds = NULL;
 np->ntraux = NULL;
 np->nchgaction = NULL;
 np->vpi_ndrvs = NULL;
 np->regwir_putv_tedlst = NULL;
 np->nva.wp = NULL;
 np->nu2.nnxt = NULL;
 np->nattrs = NULL;
 syp->sytyp = SYM_N;
 syp->el.enp = np;
 /* assume reg */
 np->ntyp = N_REG;
 return(np);
}

/*
 * allocate a ncomp element from a preallocated block for fast freeing
 */
extern struct ncomp_t *__alloc_arrncomp(void)
{
 struct ncablk_t *ncabp;
 struct ncomp_t *ncmp;

 if (__ncablk_nxti == -1)
  {
   ncabp = (struct ncablk_t *) __my_malloc(sizeof(struct ncablk_t));
   ncabp->ancmps = (struct ncomp_t *) __my_malloc(BIG_ALLOC_SIZE);
   ncabp->ncablknxt = __hdr_ncablks;
   __hdr_ncablks = ncabp;
   __ncablk_nxti = 0;
  }
 ncmp = (struct ncomp_t *) &(__hdr_ncablks->ancmps[__ncablk_nxti]);
 if (++__ncablk_nxti > ((BIG_ALLOC_SIZE/sizeof(struct ncomp_t)) - 1))
  __ncablk_nxti = -1;
 return(ncmp);
}

/*
 * print message and return true if declaration error
 * called for declaration when symbol is not new
 * for symbols that are like variables
 */
static int32 is_decl_err(struct sy_t *syp, word32 dclsytyp,
 word32 dclwtyp)
{
 struct net_t *np;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 if (syp->sytyp == SYM_N) np = syp->el.enp; else np = NULL;
 /* symbol already declared */
 /* see if declaration repeated */
 if (syp->sydecl || syp->sytyp != dclsytyp)
  {
   /* current symbol */
   if (np != NULL) __to_wtnam(s1, np); else __to_sytyp(s1, syp->sytyp);
   /* declared type */
   if (dclsytyp == SYM_N) __to_wtnam2(s2, dclwtyp);
   else __to_sytyp(s2, dclsytyp);

   if (syp->sydecl) strcpy(s3, "declared"); else strcpy(s3, "used");
   __pv_ferr(1014, "%s %s previously %s as %s at %s", s2, syp->synam,
    s3, s1, __bld_lineloc(__xs, syp->syfnam_ind, syp->sylin_cnt));
   return(TRUE);
  }
 return(FALSE);
}

/*
 * set a register width
 */
static void set_reg_widths(word32 wtyp, struct expr_t **x1,
 struct expr_t **x2)
{
 word32 rhigh;

 if (wtyp == N_INT) rhigh = WBITS - 1;
 else if (wtyp == N_TIME) rhigh = TIMEBITS - 1;
 else if (wtyp == N_REAL) rhigh = REALBITS - 1; 
 else { *x1 = NULL; *x2 = NULL; return; }
 *x1 = __bld_rng_numxpr(rhigh, 0L, WBITS);
 *x2 = __bld_rng_numxpr(0L, 0L, WBITS);
}

/*
 * for constant predefined ranges need to build a number expr. 
 * so param substitution and folding will work but do nothing
 *
 * LOOKATME - siz must be <= WBITS so why pass it 
 */
extern struct expr_t *__bld_rng_numxpr(word32 av, word32 bv, int32 siz)
{
 struct expr_t *ndp;

 /* this also initializes node */
 ndp = __alloc_newxnd();
 __set_numval(ndp, av, bv, siz);
 return(ndp);
}

/*
 * check and possibly set wire range
 */
static int32 chkset_wdrng(struct net_t *np, struct expr_t *x1,
 struct expr_t *x2)
{
 int32 cval;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 /* know range - either previous I/O or wire decl. with range */
 if (np->nu.ct->n_rngknown)
  {
   if (x1 == NULL) return(TRUE);
   if (np->nu.ct->nx1 != NULL)
    cval = cmp_rng(x1, x2, np->nu.ct->nx1, np->nu.ct->nx2);
   else cval = 1;
   if (cval != 0)
    {
     __to_wrange(s2, np);
     __pv_fwarn(568,
     "%s port %s declaration range %s mismatch with port range %s",
      __to_wtnam(s1, np), np->nsym->synam,
      __msgtox_wrange(s3, x1, x2), s2);
    }
   return(TRUE);
  }
 if (x1 != NULL)
  {
   np->nu.ct->n_rngknown = TRUE;
   np->n_isavec = TRUE;
   np->nu.ct->nx1 = x1;
   np->nu.ct->nx2 = x2;
  }
 return(TRUE);
}

/*
 * compare 2 range exprs during compilation - before params known
 * equal if same known numbers (or numeric expr.) or if exprs the same
 */
static int32 cmp_rng(struct expr_t *x1, struct expr_t *x2,
 struct expr_t *nx1, struct expr_t *nx2)
{
 if (!__cmp_xpr(x1, nx1)) return(1);
 if (!__cmp_xpr(x2, nx2)) return(1);
 return(0);
}

/*
 * compare 2 expressions for identicalness
 * if incorrect real, same and error caught later
 */
extern int32 __cmp_xpr(struct expr_t *nx, struct expr_t *ox)
{
 int32 retval, owlen, nwlen;
 word32 *owp, *nwp;

 switch ((byte) nx->optyp) { 
  case NUMBER:
   if (ox->optyp != NUMBER) return(FALSE);
   owlen = wlen_(ox->szu.xclen); 
   nwlen = wlen_(nx->szu.xclen); 
   /* since implied assignment to 32 bit value - just use low words */
   owp = &(__contab[ox->ru.xvi]); 
   nwp = &(__contab[nx->ru.xvi]); 
   if (owp[0] != nwp[0]) return(FALSE); 
   if (owp[owlen] != nwp[nwlen]) return(FALSE);
   break;
  case ID:
   /*FALLTHRU */
  case GLBREF:
   if (ox->lu.sy != nx->lu.sy) return(FALSE);
   break;
  default:
   if (ox->optyp != nx->optyp) return(FALSE);
   retval = TRUE;
   if (ox->lu.x != NULL) retval = __cmp_xpr(nx->lu.x, ox->lu.x); 
   if (!retval) return(FALSE);
   if (ox->ru.x != NULL) retval = __cmp_xpr(nx->ru.x, ox->ru.x); 
   return(retval);
 }
 return(TRUE);
}

/*
 * get optional strengths
 * know first strength read and reads one past ending )
 * can be cap size or strength pair - because of cont. assignments gets
 * checked later
 * set globals __v0stren and __v1stren
 */
static int32 rd_verstrens(void)
{
 int32 strentyp;

 __v0stren = __v1stren = NO_STREN;
 /* this sets __v0stren and __v1stren F means structural problem */
 if (!rd_1verstren(&strentyp)) return(FALSE);
 /* returned T and 2nd strength null means cap. size */
 if (strentyp == CAP_STREN) { __get_vtok(); return(TRUE); }
 if (__toktyp == RPAR)
  {
   __pv_ferr(1015,
    "required 2nd drive strength missing - %s read", __prt_vtok());
   return(FALSE);
  }

 /* know comma read to get here */
 __get_vtok();
 if (!rd_1verstren(&strentyp)) return(FALSE);
 if (__toktyp == COMMA)
  {
   __pv_ferr(1016, "strength list has more than 2 strengths");
   return(FALSE);
  }
 __get_vtok();
 if (__v0stren == NO_STREN || __v1stren == NO_STREN)
  {
   __pv_ferr(1017, "0 or 1 transition strength repeated (other missing)");
   if (__v0stren == NO_STREN) __v0stren = __v1stren;
   else __v1stren = __v0stren;
  }
 if (__v0stren == ST_HIGHZ && __v1stren == ST_HIGHZ)
  {
   __pv_ferr(1018, "(highz0, highz1) strength illegal");
   __v0stren = __v1stren = NO_STREN;
  }
 else if (__v0stren == ST_STRONG && __v1stren == ST_STRONG)
  {
   __finform(424,
    "explicit (strong0, strong1) removed to speed up simulation"); 
   __v0stren = __v1stren = NO_STREN;
  }
 return(TRUE);
}


/*
 * get 1 strength - know leading '(' and strength read
 * reads ending ')' or ',', sets __v1stren and __v0stren
 *
 * notice this returns symbolic constant that is strength type set either
 * __v0stren or __v1stren depending on token type
 * returns F on token error else F, if bad sets to none
 */
static int32 rd_1verstren(int32 *strentyp)
{
 int32 strenval;

 *strentyp = NO_STREN;
 if ((*strentyp = is_tokstren(__toktyp)) == CAP_STREN)
  {
   strenval = __fr_stren_nam(__toktyp);
   __get_vtok();
   if (__toktyp != RPAR)
    {
     __pv_ferr(1019, "trireg charge strength ending ')' expected - %s read",
      __prt_vtok());
     return(FALSE);
    }
   __v0stren = strenval;
   return(TRUE);
  }
 if (*strentyp == LOW_STREN)
  {
   if (__v0stren != NO_STREN)
    __pv_fwarn(569, "both strengths are 0 transition - 2nd changed");
   __v0stren = __fr_stren_nam(__toktyp);
  }
 else if (*strentyp == HIGH_STREN)
  {
   if (__v1stren != NO_STREN)
    __pv_fwarn(569, "both strengths are 1 transition - 2nd changed");
   __v1stren = __fr_stren_nam(__toktyp);
  }
 else
  {
   __pv_ferr(1020, "expected strength missing - %s read", __prt_vtok());
   return(FALSE);
  }
 __get_vtok();
 if (__toktyp != COMMA && __toktyp != RPAR)
  {
   __pv_ferr(1021,
    "strength not followed by comma or right parenthesis - %s read",
    __prt_vtok());
   return(FALSE);
  }
 return(TRUE);
}

/*
 * determine if token type strength and whether 0 or 1 group
 */
static int32 is_tokstren(int32 ttyp)
{
 switch ((byte) ttyp) {
  case SUPPLY0: case STRONG0: case PULL0: case WEAK0: case HIGHZ0:
   return(LOW_STREN);
  case SUPPLY1: case STRONG1: case PULL1: case WEAK1: case HIGHZ1:
   return(HIGH_STREN);
  case SMALL: case MEDIUM: case LARGE:
   return(CAP_STREN);
 }
 return(NO_STREN);
}

/*
 * read the parameter statement (declares the parameter)
 * form: parameter [name] = [value], [name] = [value], ...;
 * where name is a simple id and [value] is a constant expr.
 *
 * no # or () around delay in parameter decl. but min:typ:max form
 * needs () since rhs is constant expr. in grammar not mintypmax expr.
 *
 * if returns F synced to next module, else synced to SEMI
 *
 * also reads vendor1 specific param types
 *
 * SJM 10/07/03 - add optional signed declaration - following normal
 * rule for parameter typing, if signed not present determined from rhs
 *
 * SJM 05/25/04 - added new P1364 module #(<param decl>, ...) form but only 
 * for modules parameter declarations and unlike header list of ports both
 * types can be combined 
 */
static int32 rd_paramdecl(int32 is_hdr_form, int32 is_local_param)
{
 int32 ptyp_decl, prng_decl, pwtyp, pwid, r1, r2, wlen;
 int32 psign_decl; 
 word32 *wp;
 struct expr_t *dx1, *dx2, *x1, *x2, *ax1, *ax2;
 struct net_t *np;
 struct xstk_t *xsp;
 char paramnam[IDLEN], ptnam[RECLEN];

 if (is_local_param) strcpy(ptnam, "localparam");
 else strcpy(ptnam, "parameter");

 dx1 = dx2 = x1 = x2 = ax1 = ax2 = NULL;
 ptyp_decl = FALSE; 
 prng_decl = FALSE;
 pwtyp = -1;
 pwid = 0;
 psign_decl = FALSE; 
 __get_vtok();
 if (__toktyp == SIGNED)
  {
   psign_decl = TRUE;
   __get_vtok();
  }

 /* case 1: range but not decl type */
 if (__toktyp == LSB)
  {
   /* also check to make sure ranges are non x/z 32 bit values */
   if (!__rd_opt_param_vec_rng(&dx1, &dx2, is_hdr_form)) return(FALSE);
   if (dx1 == NULL || dx2 == NULL) goto rd2_param_list;

   r1 = (int32) __contab[dx1->ru.xvi];
   r2 = (int32) __contab[dx2->ru.xvi];
   pwid = (r1 >= r2) ? r1 - r2 + 1 : r2 - r1 + 1; 
   pwtyp = N_REG;
   x1 = dx1; x2 = dx2;
   prng_decl = TRUE;
   /* DBG remove --- */
   if (x1 == NULL) goto rd_param_list;
   /* --- */
   /* know parameter name read */
   goto rd2_param_list;
  }

 /* if next token not optional - if range, implied reg type */ 
 if ((pwtyp = __fr_wtnam(__toktyp)) == -1)
  {
   pwtyp = N_REG;
   x1 = dx1; x2 = dx2;
   /* implied decl as range */  
   if (x1 != NULL) ptyp_decl = TRUE;
   goto rd2_param_list; 
  }
 else
  {
   if (pwtyp == N_EVENT || pwtyp < NONWIRE_ST)
    {
     __pv_ferr(685, "%s declaration illegal type %s", ptnam, __prt_vtok());
     x1 = x2 = NULL;
     pwtyp = N_REG;
     goto rd_param_list;
    }

   ptyp_decl = TRUE; 
   switch ((byte) pwtyp) {
    case N_REAL:
     pwid = REALBITS;
     goto chk_norng;
    case N_TIME:
     pwid = TIMEBITS;
     goto chk_norng;
    case N_INT:
     pwid = WBITS;
chk_norng:
     if (dx1 != NULL)
      {
       __pv_ferr(686, "%s declaration range illegal for opt_type %s",
        ptnam, __to_wtnam2(__xs, (word32) pwtyp));
      }
     x1 = __bld_rng_numxpr(pwid - 1, 0L, WBITS);
     x2 = __bld_rng_numxpr(0L, 0L, WBITS);
     prng_decl = FALSE;
     break;
    default:
     __case_terr(__FILE__, __LINE__);
     return(FALSE);
   }
  }

rd_param_list:
 __get_vtok();
rd2_param_list:
 /* SJM 10/06/03 - signed keyword not allowed with var types */
 if (psign_decl && (pwtyp == N_TIME || pwtyp == N_INT || pwtyp == N_REAL))
  {
   __pv_ferr(3423,
    "signed keyword illegal when parameter variable type %s",
    __to_wtnam2(__xs, pwtyp));
   psign_decl = FALSE;
  }

 /* if ptyp decl F, then must attempt to determine param type from rhs */ 
 for (;;)
  {
   if (__toktyp != ID)
    {
     __pv_ferr(1023,
      "%s declaration parameter name expected - %s read", ptnam,
      __prt_kywrd_vtok());
bad_end:
     /* part of delay expression may have been built */
     if (!is_hdr_form)
      {
       if (!__vskipto2_any(COMMA, SEMI)) return(FALSE);
       if (__toktyp == COMMA) { __get_vtok(); continue; }
      }
     else
      {
       if (!__vskipto2_any(COMMA, RPAR)) return(FALSE); 
       if (__toktyp == COMMA) { __get_vtok(); continue; }
      }
     return(TRUE);
    }
   strcpy(paramnam, __token);

   /* notice the initial value is required */
   __get_vtok();
   /* parameter arrays now but only declarator present legal */
   /* also range for arrays must be repeated for each one */ 
   if (__toktyp == LSB)
    {
     /* also check to make sure ranges are non x/z 32 bit values */
     if (!rd_opt_param_array_rng(&ax1, &ax2, is_hdr_form)) return(FALSE);
    }
   if (ax1 != NULL && !ptyp_decl)
    {
     __pv_ferr(687,
      "parameter array %s illegal - explicit type declaration required for parameter arrays",
      paramnam); 
     /* notice not freeing expr. memory on syntax error */ 
     ax1 = ax2 = NULL;
    }

   if (__toktyp != EQ)
    {
     __pv_ferr(1024,
      "%s declaration equal expected - %s read", ptnam, __prt_vtok());
     goto bad_end;
    }

   /* notice initial value required */
   __get_vtok();
   /* this can collect array construct that will look like concat */
   if (is_hdr_form)
    {
     /* SJM 05/26/04 - new module decl #(list of param decls) form needs */
     /* different collect routine because semi illegal */
     if (!__col_lofp_paramrhsexpr()) goto bad_end;
    }
   else
    {
     if (!__col_newparamrhsexpr()) goto bad_end;
    }
   __bld_xtree(0);

   if (__has_top_mtm)
    {
     __pv_fwarn(652,
      "%s %s declaration min:typ:max expression needs parentheses under 1364 - unportable",
      ptnam, paramnam);
    }

   /* handle parameter arrays as special case */
   if (ax1 != NULL)
    {
     /* warning if parameter array used */
     __pv_fwarn(643,
      "parameter array %s enhancement - not part of P1364 standard",
      paramnam);
     /* notice range and type for all in possible param list but */
     /* array range and initializer different for each */
     /* if error returns nil and try to resync */
     if ((np = chkadd_array_param(paramnam, pwtyp, pwid, psign_decl, x1, x2,
       ax1, ax2))
       == NULL) goto bad_end;

     goto nxt_param;
    }


   /* checking rhs does no evaluation but set sizes and checks for */
   /* only params that are defined previously in module */
   /* SJM 10/06/03 - rd chk paramexpr routine set expr signed bit */
   if (__expr_has_glb(__root_ndp) || !__src_rd_chk_paramexpr(__root_ndp, 0))
    {
     __pv_ferr(1025,
      "%s %s declaration right hand side \"%s\" error - defined %ss and constants only",
      ptnam, paramnam, __msgexpr_tostr(__xs, __root_ndp), ptnam);
     /* need to still add value of x to prevent further errors */
     __free2_xtree(__root_ndp);
     __root_ndp->szu.xclen = WBITS;
     __set_numval(__root_ndp, ALL1W, ALL1W, WBITS);  
     /* SJM 03/15/00 - on err if decl as real - must convert to non real */
     if (pwtyp == N_REAL) pwtyp = N_REG;
    }

   /* check and links on modules parameter list */
   /* when rhs expr. evaluated, if real will change */
   /* LOOKATME - problem with all params in list sharing range xprs? */ 
   /* SJM 01/24/00 - works since for globalparam runs in virt glb param mod */
   if ((np = __add_param(paramnam, x1, x2, is_local_param)) == NULL) 
    {
     return(FALSE);
    }

   /* require that at this point all param rhs expressions are numbers */
   /* know possible and needed for copying and later defparam assign */
   /* rule from LRM is that all param initial values known when first read */
   xsp = __src_rd_eval_xpr(__root_ndp);

   /* case parameter type declared - maybe convert - must eval before here */
   /* if range, implied pwtyp set to reg type */
   if (ptyp_decl || prng_decl)
    {
     if (pwtyp == N_REAL)
      {
       np->ntyp = N_REAL; 
       np->n_signed = TRUE;
       np->nwid = REALBITS;
       np->nu.ct->pbase = BDBLE;
       if (!__root_ndp->is_real)
        __src_rd_cnv_stk_fromreg_toreal(xsp, (__root_ndp->has_sign == 1));
      }
     else
      {
       np->ntyp = pwtyp;
       /* if declared always know width */
       np->nwid = pwid;
       if (np->ntyp == N_INT || (np->ntyp == N_REG && psign_decl))
        np->n_signed = TRUE;

       /* even if declared use rhs expr. for param ncomp expr formats */
       if (__root_ndp->is_string) np->nu.ct->pstring = TRUE;
       np->nu.ct->pbase = __root_ndp->ibase;

       /* convert declared param type real rhs to int/reg */
       if (__root_ndp->is_real)
        {
         __src_rd_cnv_stk_fromreal_toreg32(xsp);
         np->nu.ct->pbase = BDEC;
        }
       /* xsp always right width for parameter net width */
       if (xsp->xslen != pwid) __sizchgxs(xsp, pwid);

       /* SJM 09/29/03 - change to handle sign extension and separate types */
       if (xsp->xslen != pwid) __narrow_sizchg(xsp, pwid);
       else if (xsp->xslen < pwid)
        {
         if (__root_ndp->has_sign) __sgn_xtnd_widen(xsp, pwid);
         else __sizchg_widen(xsp, pwid);
        }
      }
    }
   else
    {
     /* no parameter range given or will not get here */ 
     if (__root_ndp->is_real)
      {
       np->ntyp = N_REAL;
       np->n_signed = TRUE;
      }
     else
      {
       np->ntyp = N_REG;
       /* SJM 10/06/03 - no range or var type but signed may be present */
       if (psign_decl) np->n_signed = TRUE;
       else
        {
         if (__root_ndp->has_sign) np->n_signed = TRUE;
        }
      }

     np->nwid = __root_ndp->szu.xclen;
     if (np->nwid > 1)
      {
       np->n_isavec = TRUE;
       np->vec_scalared = TRUE;
       np->nu.ct->nx1 = __bld_rng_numxpr(np->nwid - 1, 0L, WBITS);
       np->nu.ct->nx2 = __bld_rng_numxpr(0L, 0L, WBITS);
      }
     /* always true for real and int32 - maybe true from others */
     if (__root_ndp->is_string) np->nu.ct->pstring = TRUE;
     /* this works because param expr checking always sets ibase */ 
     np->nu.ct->pbase = __root_ndp->ibase;
    }

   if (ptyp_decl) np->nu.ct->ptypdecl = TRUE;
   if (prng_decl) np->nu.ct->prngdecl = TRUE;
   if (psign_decl) np->nu.ct->psigndecl = TRUE;
   if (__xpr_has_param(__root_ndp)) 
    {
     np->nu.ct->p_rhs_has_param = TRUE;
     __inst_mod->mod_rhs_param = TRUE;
    }

   /* using ncomp delay union to store original expresssion - set first */
   np->nu.ct->n_dels_u.d1x = __root_ndp;
   np->nu.ct->parm_srep = SR_PXPR;

   /* value must be evaluated to constant expr - since may need to */
   /* change to IS form */
   /* assign the value as SR PNUM form because now always "declared" - has */
   /* kown type so can store as net value */
   wlen = wlen_(xsp->xslen);
   wp = (word32 *) __my_malloc(2*WRDBYTES*wlen);
   memcpy(wp, xsp->ap, 2*wlen*WRDBYTES);
   np->nva.wp = wp;
   np->srep = SR_PNUM;
   __pop_xstk();


   if (__debug_flg)
    {
     char s1[RECLEN], s2[RECLEN], *chp;
     
     strcpy(s1, "");
     strcpy(s2, "");
     if (__root_ndp->is_real) strcpy(s1, "real ");
     else 
      {
       sprintf(s2, " width %d", __root_ndp->szu.xclen);
       if (__root_ndp->is_string) strcpy(s1, "string ");
       else if (__root_ndp->has_sign) strcpy(s1, "signed ");
      }
     /* SJM 04/20/00 - changed so uses param type for printing */
     /* SJM 05/24/00 - trim leading spaces */
     __pregab_tostr(__xs, wp, &(wp[wlen]), np);
     for (chp = __xs;; chp++) { if (*chp != ' ') break; }
     __dbg_msg(
      "%s%s %s defined at **%s(%d) has initial value %s%s\n", s1,
      ptnam, paramnam, __cur_fnam, __lin_cnt, chp, s2);
    }

nxt_param:
   if (is_hdr_form)
    {
     if (__toktyp == RPAR) break;
     if (__toktyp != COMMA)
      {
       __pv_ferr(1026,
        "%s module header form declaration right paren or comma expected - %s read",
        ptnam, __prt_vtok());
       if (!__vskipto2_any(COMMA, RPAR)) return(FALSE); 
       if (__toktyp == RPAR) break;
      }
    }
   else
    {
     if (__toktyp == SEMI) break;
     if (__toktyp != COMMA)
      {
       __pv_ferr(1026,
        "%s declaration semicolon or comma separator expected - %s read",
        ptnam, __prt_vtok());
       if (!__vskipto2_any(COMMA, SEMI)) return(FALSE); 
       if (__toktyp == SEMI) break;
      }
    }
   __get_vtok();
   if (is_hdr_form)
    {
     /* if , followed by ID, part of list else new parameter decl */
     if (__toktyp == PARAMETER) break;
    }
  }
 return(TRUE);
}

/*
 * return T if parameter define rhs expr contains any param
 *
 * set ncomp rhs has param bit causes re-eval of param value 
 * to use new pound and defparam values if changed
 */
extern int32 __xpr_has_param(struct expr_t *ndp)
{
 struct sy_t *syp;
 struct net_t *np;
 struct expr_t *fandp;

 switch ((byte) ndp->optyp) { 
  case NUMBER: case REALNUM: case ISNUMBER: case ISREALNUM: return(FALSE);
  case ID:
   syp = ndp->lu.sy;
   if (!syp->sydecl || syp->sytyp != SYM_N) return(FALSE);
   np = syp->el.enp; 
   if (np->n_isaparam) return(TRUE);
   return(FALSE);
  case GLBREF: return(FALSE);
  case FCALL:
   for (fandp = ndp->ru.x; fandp != NULL; fandp = fandp->ru.x)
    {
     /* LOOKATME - even if real param not allowed arg to const systf */
     /* can be real */
     if (__xpr_has_param(fandp->lu.x)) return(TRUE);
    }
   return(FALSE);
 } 
 if (ndp->lu.x != NULL) if (__xpr_has_param(ndp->lu.x)) return(TRUE);
 if (ndp->ru.x != NULL) if (__xpr_has_param(ndp->ru.x)) return(TRUE);
 return(FALSE);
}

/*
 * version of chk paramexpr that is called for parameters during
 * source input 
 */
extern int32 __src_rd_chk_paramexpr(struct expr_t *ndp, int32 xwid)
{
 int32 rv, sav_sfnam_ind, sav_slin_cnt;

 /* SJM 10/01/99 - improve error location for param checking */
 /* chk param expr needs sim locations set - set temporary guess here */
 sav_sfnam_ind = __sfnam_ind;
 sav_slin_cnt = __slin_cnt;
 __sfnam_ind = __cur_fnam_ind;
 __slin_cnt = __lin_cnt;

 rv = __chk_paramexpr(ndp, xwid);

 __sfnam_ind = sav_sfnam_ind;
 __slin_cnt = sav_slin_cnt;

 return(rv);
}

/*
 * read a parameter vector declaration range 
 *
 * know [ read and reads one past ]
 */
extern int32 __rd_opt_param_vec_rng(struct expr_t **ax1, struct expr_t **ax2,
 int32 is_hdr_form)
{
 int32 rngerr;
 struct expr_t *x1, *x2;
 char ptnam[RECLEN], s1[RECLEN];

 if (__cur_declobj == SPECIFY) strcpy(ptnam, "specparam");
 strcpy(ptnam, "parameter");

 *ax1 = *ax2 = NULL;
 rngerr = FALSE;
 if (!__rd_decl_rng(&x1, &x2))
  {
   if (!is_hdr_form)
    {
     if (!__vskipto2_any(SEMI, RSB)) return(FALSE); 
     if (__toktyp == SEMI) return(TRUE);
    }
   else
    {
     if (!__vskipto3_any(COMMA, RPAR, RSB)) return(FALSE); 
     if (__toktyp != RSB) return(TRUE);
    }
   rngerr = TRUE; 
   __get_vtok();
   goto done;
  }
 if (__expr_has_glb(x1) || !__src_rd_chk_paramexpr(x1, 0))
  {
   __pv_ferr(1025,
    "%s declaration first range \"%s\" illegal - defined %ss and constants only",
    ptnam, __msgexpr_tostr(__xs, x1), ptnam);
   rngerr = TRUE;
  }
 else
  {
   /* because of previous check, this can not fail */
   __eval_param_rhs_tonum(x1);
   sprintf(s1, "%s declaration first range", ptnam);
   if (!__nd_ndxnum(x1, s1, TRUE)) rngerr = TRUE;
  }
 if (__expr_has_glb(x2) || !__chk_paramexpr(x2, 0))
  {
   __pv_ferr(1025,
    "%s declaration second range \"%s\" illegal - defined %ss and constants only",
    ptnam, __msgexpr_tostr(__xs, x2), ptnam);
   rngerr = TRUE;
  }
 else
  {
   /* because of previous check, this can not fail */
   __eval_param_rhs_tonum(x2);
   sprintf(s1, "%s declaration second range", ptnam);
   if (!__nd_ndxnum(x2, s1, TRUE)) rngerr = TRUE;
  }

done:
 if (rngerr)
  {
   if (x1 != NULL) __free_xtree(x1);
   if (x2 != NULL) __free_xtree(x2);
   x1 = x2 = NULL;
  }
 *ax1 = x1;
 *ax2 = x2;
 return(TRUE);
}

/*
 * read a parameter array declaration range 
 *
 * know [ read and reads one past ]
 */
static int32 rd_opt_param_array_rng(struct expr_t **ax1, struct expr_t **ax2,
 int32 is_hdr_form)
{
 int32 rngerr;
 struct expr_t *x1, *x2;
 
 rngerr = FALSE;
 if (!__rd_decl_rng(&x1, &x2) || x1 == NULL || x2 == NULL)
  {
   if (!is_hdr_form)
    {
     if (!__vskipto2_any(SEMI, RSB)) return(FALSE); 
     if (__toktyp == SEMI) return(TRUE);
    }
   else
    {
     if (!__vskipto3_any(COMMA, RPAR, RSB)) return(FALSE); 
     if (__toktyp != RSB) return(TRUE);
    }
   rngerr = TRUE;
   __get_vtok();
   goto done;
  }
 /* one past possible ] read */
 /* convert ranges to constant index */
 if (__expr_has_glb(x1) || !__src_rd_chk_paramexpr(x1, 0))
  {
   __pv_ferr(1025,
    "parameter array declaration first range \"%s\" illegal - defined parameters and constants only",
    __msgexpr_tostr(__xs, x1));
   rngerr = TRUE;
  }
 else
  {
   /* because of previous check, this can not fail */
   __eval_param_rhs_tonum(x1);
   if (!__nd_ndxnum(x1, "parameter array declaration first range", TRUE))
     rngerr = TRUE;
  }
 if (__expr_has_glb(x2) || !__src_rd_chk_paramexpr(x2, 0))
  {
   __pv_ferr(1025,
    "parameter array declaration second range \"%s\" illegal - defined parameters and constants only",
    __msgexpr_tostr(__xs, x2));
   rngerr = TRUE;
  }
 else
  {
   /* because of previous check, this can not fail */
   __eval_param_rhs_tonum(x2);
   if (!__nd_ndxnum(x2, "parameter array declaration second range", TRUE))
    rngerr = TRUE;
  }
done:
 if (rngerr)
  {
   if (x1 != NULL) __free_xtree(x1);
   if (x2 != NULL) __free_xtree(x2);
   x1 = x2 = NULL;
  }
 *ax1 = x1;
 *ax2 = x2;
 return(TRUE);
}
     
/*
 * routine to check and declare array param 
 *
 * also sets initial array value
 * know cur mod set when this is called
 */
static struct net_t *chkadd_array_param(char *paramnam, int32 pwtyp, int32 pwid,
 int32 psign, struct expr_t *x1, struct expr_t *x2, struct expr_t *ax1,
  struct expr_t *ax2)
{
 register int32 ai;
 word32 *wp;
 int32 unnorm_ai, r1, r2, awid, wlen;
 int32 initerr, some_str, all_str;
 struct expr_t *catndp, *xp, **avalxtab;
 struct net_t *np;
 struct xstk_t *xsp;
 char s1[RECLEN];

 if (__root_ndp->optyp != LCB)
  {
   __pv_ferr(688,
    "parameter %s array initial value %s not array constructor ({} concatenate) ",
    paramnam, __msgexpr_tostr(__xs, x2));
   return(NULL);
  }
 /* build table of parameters */
 r1 = (int32) __contab[ax1->ru.xvi];
 r2 = (int32) __contab[ax2->ru.xvi];
 /* number of cells in array */
 awid = (r1 >= r2) ? r1 - r2 + 1 : r2 - r1 + 1; 
 avalxtab = (struct expr_t **) __my_malloc(awid*sizeof(struct expr_t *));
 for (ai = 0; ai < awid; ai++) avalxtab[ai] = NULL;

 /* first step unwind array constructor repeats */
 unwind_param_array_constructor(__root_ndp);
 if (__debug_flg)
  {
   __dbg_msg("parameter %s declare array initializer unwound to: %s\n", 
   paramnam, __msgexpr_tostr(__xs, __root_ndp));
  }

 /* fill table of expressions */
 catndp = __root_ndp->ru.x;
 /* first is internal high cell */
 initerr = FALSE;
 for (ai = awid - 1; catndp != NULL; catndp = catndp->ru.x)
  {
   unnorm_ai = (r1 >= r2) ? (r2 + ai) : (r2 - ai);
   xp = catndp->lu.x;
   /* DBG remove --- */
   if (xp->optyp == CATREP) __misc_terr(__FILE__, __LINE__);
   /* --- */
   chk1_arrinit_expr(xp, paramnam, unnorm_ai);
   if (ai < 0) 
    {
     __pv_ferr(689,
      "parameter array initializer at %s[%d] %s illegal - fills past end",
      paramnam, unnorm_ai, __msgexpr_tostr(__xs, xp));
     initerr = TRUE;
     continue;
    }
   avalxtab[ai] = __copy_expr(xp); 
   ai--; 
  }
 /* finally check to make sure fill width correct */
 if (ai >= 0)
  {
   __pv_ferr(689,
    "parameter array %s initializer width %d illegal - only %d filled",
    paramnam, awid, ai + 1);
   return(NULL);
  }
 /* do not add unless initializer good */
 if (initerr) return(NULL);

 /* know all cells in aval xtab filled - add param - must be declared */
 if ((np = __add_param(paramnam, x1, x2, TRUE)) == NULL) return(NULL); 

 np->nu.ct->ax1 = ax1; 
 np->nu.ct->ax2 = ax2; 
 np->n_isarr = TRUE;
 /* index of this cannot be used in pound params */
 np->nu.ct->p_locparam = TRUE;
 np->srep = SR_PNUM;

 if (pwtyp == N_REAL)
  {
   np->ntyp = N_REAL;
   np->n_signed = TRUE;
   np->nwid = REALBITS;
   np->nu.ct->pbase = BDBLE;
  }
 else
  {
   np->ntyp = pwtyp;
   /* if declared always know width */
   np->nwid = pwid;
   if (np->ntyp == N_INT) np->n_signed = TRUE;
   else
    {
     if (psign && np->ntyp != N_TIME) np->n_signed = TRUE; 
    }

   /* if some but not all strings needs warning */
   for (some_str = FALSE, all_str = TRUE, ai = awid - 1; ai >= 0; ai--)
    {
     if (avalxtab[ai]->is_string) some_str = TRUE;
     else all_str = FALSE;
    }
   if (some_str && !all_str)
    {
     __pv_fwarn(615,
      "parameter array %s some but not all cells initialized to strings - strings treated as numbers",
      paramnam);
    }
   if (all_str) np->nu.ct->pstring = TRUE;
   np->nu.ct->pbase = __root_ndp->ibase;
  }
 /* allocate and fill parameter array - never packed */

 /* allocate and fill parameter array */
 wlen = wlen_(np->nwid);
 np->nva.wp = (word32 *) __my_malloc(2*WRDBYTES*awid*wlen);
 for (ai = awid - 1; ai >= 0; ai--)
  {
   unnorm_ai = (r1 >= r2) ? (r2 + ai) : (r2 - ai);
   xsp = __src_rd_eval_xpr(avalxtab[ai]);
   sprintf(s1, "parameter array cell %s[%d] initializer", np->nsym->synam,
    unnorm_ai); 
   /* afer here know xsp width match declared parameter width */
   cnvt_to_pdecl(xsp, avalxtab[ai], np, s1); 
   wp = &(np->nva.wp[2*wlen*ai]);
   memcpy(wp, xsp->ap, 2*wlen*WRDBYTES);
   __pop_xstk();
  }

 /* save expression that is original array constructor expression */
 np->nu.ct->n_dels_u.d1x = __root_ndp;
 np->nu.ct->parm_srep = SR_PXPR;

 /* free expression table */
 for (ai = awid - 1; ai >= 0; ai--) __free_xtree(avalxtab[ai]);
 __my_free((char *) avalxtab, awid*sizeof(struct expr_t *));

 if (__debug_flg)
  {
   if (np->ntyp == N_REAL)
    sprintf(s1, "[%d:%d] cell real array", r1, r2);
   else if (pwtyp == N_INT)
    sprintf(s1, "[%d:%d] cell integer array", r1, r2);
   else
    {
     sprintf(s1, " [%d:%d] cell %d bit reg array", r1, r2, np->nwid);
     if (np->nu.ct->pstring) strcat(s1, " [string]");
     else if (np->n_signed && pwtyp != N_INT) strcat(s1, " [signed]");
    }
   __dbg_msg("%s parameter %s defined at **%s(%d):\n", s1, paramnam,
    __cur_fnam, __lin_cnt);
   for (ai = awid - 1; ai >= 0; ai--)
    {
     unnorm_ai = (r1 >= r2) ? (r2 + ai) : (r2 - ai);
     
     wp = &(np->nva.wp[2*wlen*ai]);
     __dbg_msg("  %s[%d] = %s\n", paramnam, unnorm_ai, __pregab_tostr(__xs,
      wp, &(wp[wlen]), np));
    }
  }
 return(np);
}

/*
 * special xstk conversion routine where must convert to declared size
 *
 * this converts to type and size of param - new algorithm assumes
 * that parameters are somehow declared (possibly from initial rhs expr)
 */
static void cnvt_to_pdecl(struct xstk_t *xsp, struct expr_t *xrhs,
 struct net_t *np, char *innam)
{
 char s1[RECLEN];

 /* DBG remove -- */
 if (!np->n_isaparam) __arg_terr(__FILE__, __LINE__);
 /* --- */

 if (np->ntyp == N_REAL)
  { 
   if (!xrhs->is_real)
    {
     if (xrhs->szu.xclen == WBITS && xrhs->has_sign) strcpy(s1, "integer");
     else sprintf(s1, "%d bit register", xrhs->szu.xclen);
     __gfinform(486, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
      "parameter %s in %s assign required conversion from %s to real",
      np->nsym->synam, innam, s1); 

     __cnv_stk_fromreg_toreal(xsp, (xrhs->has_sign == 1));
    }
  }
 else 
  { 
   /* know param is non real */ 
   if (xrhs->is_real)
    {
     if (np->nwid == WBITS && np->n_signed) strcpy(s1, "integer");
     else sprintf(s1, "%d bit register", np->nwid);

     __gfinform(487, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
      "parameter %s in %s assign required conversion from real to %s",
      np->nsym->synam, innam, s1); 

     __cnv_stk_fromreal_toreg32(xsp);
    }
   /* but it may have wrong width - in new algorithm param assigns */
   if (xsp->xslen != np->nwid)
    {
     __gfinform(488, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
      "parameter %s in %s assign required width change from %d to %d",
      np->nsym->synam, innam, xsp->xslen, np->nwid); 
     /* key always convert to declared */

     /* SJM 09/29/03 - change to handle sign extension and separate types */
     if (xsp->xslen > np->nwid) __narrow_sizchg(xsp, np->nwid);
     else if (xsp->xslen < np->nwid)
      {
       if (xrhs->has_sign) __sgn_xtnd_widen(xsp, np->nwid);
       else __sizchg_widen(xsp, np->nwid);
      }
    }
  }
}

/*
 * check and unwind array constructor
 *
 * this will create normal array constructor from repeat count form
 * when done concatenate is one level with catreps removed
 */
static void unwind_param_array_constructor(struct expr_t *ndp)
{
 if (__isleaf(ndp)) return;

 /* array constructor (concatenate) unwinding must be done bottom up */
 if (ndp->lu.x != NULL) { unwind_param_array_constructor(ndp->lu.x); }
 if (ndp->ru.x != NULL) { unwind_param_array_constructor(ndp->ru.x); }

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
      case NUMBER: case REALNUM:
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

        if (__src_rd_chk_paramexpr(lop->lu.x, 0))
	 {
	  xsp = __src_rd_eval_xpr(lop->lu.x);
	  if (xsp->xslen > WBITS)
	   {
            if (!vval_is0_(&(xsp->ap[1]), xsp->xslen - WBITS) 
             || !vval_is0_(&(xsp->bp[1]), xsp->xslen - WBITS)
	     || xsp->bp[0] != 0L) goto bad_rep;
	   }
          else if (xsp->bp[0] != 0) goto bad_rep;
	  repval = (int32) xsp->ap[0];
          if (repval == 1) 
           __finform(442,
            "array constructore repeat value of 1 has no effect");
	 }
        else
	 {
bad_rep:
          __pv_ferr(814,
           "array constructor repeat value %s not an integral constant expression",
	   __msgexpr_tostr(__xs, lop->lu.x));
          repval = 1;
         }

	__pop_xstk();
        /* know the rhs thing must be a concatenate */
        dupndp = __dup_concat(repval, lop->ru.x->ru.x);

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
 * check one array initializer expression
 */
static void chk1_arrinit_expr(struct expr_t *xp, char *paramnam, int32 ai)
{
 if (__expr_has_glb(xp) || !__src_rd_chk_paramexpr(xp, 0))
  {
   __pv_ferr(1025,
    "parameter array initializer cell %s[%d] \"%s\" illegal - parameters and constants only",
    paramnam, ai, __msgexpr_tostr(__xs, xp));
   /* need to still add value of x to prevent further errors */
   __free2_xtree(xp);
   xp->szu.xclen = WBITS;
   __set_numval(xp, ALL1W, ALL1W, WBITS);  
  }
}

/*
 * return T if expression has any global reference
 */
extern int32 __expr_has_glb(struct expr_t *xp)
{
 if (__isleaf(xp))
  {
   if (xp->optyp == GLBREF) return(TRUE);
   return(FALSE);
  }
 if (xp->lu.x != NULL) { if (__expr_has_glb(xp->lu.x)) return(TRUE); }  
 if (xp->ru.x != NULL) { if (__expr_has_glb(xp->ru.x)) return(TRUE); }
 return(FALSE);
}

/*
 * routine to evaluate parameters or defparam rhs and produce constant
 *
 * this expects itree location to be set but no IS forms allowed by here
 * know rhs legal (only params and constants)
 * this freezes any parameter, # param or defparam to number from now on
 */
extern void __eval_param_rhs_tonum(struct expr_t *ndp)
{ 
 int32 wlen, is_str, xbase;
 double d1;
 struct xstk_t *xsp;

 if (ndp->is_string) is_str = TRUE; else is_str = FALSE;
 xbase = ndp->ibase;
 
 /* possibly different expr. */
 switch ((byte) ndp->optyp) {  
  case NUMBER: case REALNUM: break;
  /* IS forms can only be created by # or defparam assignments later */
  /* can never be in source */ 
  case ISNUMBER: case ISREALNUM: __arg_terr(__FILE__, __LINE__); break;
 default:
  if (ndp->optyp == ID && ndp->lu.sy->el.enp->n_isaparam)
   {
    is_str = ndp->lu.sy->el.enp->nu.ct->pstring;
    xbase = ndp->lu.sy->el.enp->nu.ct->pbase;
   }
  else if (ndp->optyp == LCB) is_str = TRUE;
  xsp = __src_rd_eval_xpr(ndp);
  __free2_xtree(ndp);
  wlen = wlen_(xsp->xslen);
  if (xsp->xslen <= WBITS)
   {
    if (ndp->is_real)
     {
      memcpy(&d1, xsp->ap, sizeof(double)); 
      ndp->ru.xvi = __alloc_shareable_rlcval(d1);
     }
    else
     {
      ndp->ru.xvi = __alloc_shareable_cval(xsp->ap[0], xsp->bp[0], xsp->xslen);
     }
   }
  else
   {
    ndp->ru.xvi = __allocfill_cval_new(xsp->ap, xsp->bp, wlen);
   }
  __pop_xstk();
  /* notice string from rhs param expr. impossible */
  if (ndp->is_real) ndp->optyp = REALNUM;
  else
   {
    ndp->optyp = NUMBER;
    /* notice leave rest of ndp fields the same */
    ndp->is_string = is_str;
   }
  ndp->ibase = xbase;
 } 
}

/*
 * add newly declared parameter - better not be already defined
 * called from parameter declaration item only
 * range must be passed because of parameter "v[h:l] =" form (not yet in) 
 *
 * uses initial expressions to set wire type - defparam can change
 * but one type for all instance
 *
 * this is for both parameters and specparams indicated by current decl obj
 * for specparam top of scope symbol table stack is special symbol table
 * just for specparams - no other symbols legal
 *
 * code that reads parameter arrays calls this then sets fields it
 */
extern struct net_t *__add_param(char *nam, struct expr_t *x1,
 struct expr_t *x2, int32 is_local_param)
{
 int32 is_spec;
 struct tnode_t *tnp;
 struct sy_t *syp;
 struct net_t *np;
 char s1[RECLEN], ptypnam[RECLEN]; 

 if (__cur_declobj == SPECIFY)
  { is_spec = TRUE; strcpy(ptypnam, "specparam"); } 
 else
  {
   is_spec = FALSE;
   if (is_local_param) strcpy(ptypnam, "localparam");
   else strcpy(ptypnam, "parameter");
  }
 /* just look in local scope here since parameter decl. must be local */
 tnp = __vtfind(nam, __venviron[__top_sti]);
 if (__sym_is_new)
  {
   __add_sym(nam, tnp);
   /* notice still need to update total symbol count */
   (__venviron[__top_sti]->numsyms)++;
   syp = tnp->ndp;
   /* this initializes ncomp for all net forms including params */
   np = __add_net(syp);
  }
 else
  {
   syp = tnp->ndp;
   if (syp->sytyp == SYM_N) np = syp->el.enp; else np = NULL;
   if (!syp->sydecl)
    {
     /* is previously used as net, then ok */
     if (syp->sytyp != SYM_N)
      {
       __pv_ferr(1027,
        "cannot declare %s %s - previously used as %s at %s", 
        nam, ptypnam, __to_sytyp(s1, syp->sytyp), __bld_lineloc(__xs,
        syp->syfnam_ind, syp->sylin_cnt));
       return(NULL);
      }
    }
   else
    {
     /* this needs to be explicit to include param wire types */
     if (np != NULL) __to_wtnam2(s1, np->ntyp);
     else __to_sytyp(s1, syp->sytyp);
     __pv_ferr(1028, "cannot declare %s %s - previously declared as %s at %s",
      nam, ptypnam, s1, __bld_lineloc(__xs, syp->syfnam_ind,
      syp->sylin_cnt));
     return(NULL);
    }
   /* know will be wire to get to be declared here */
   syp->syfnam_ind = __cur_fnam_ind;
   syp->sylin_cnt = __lin_cnt;
  }
 syp->sydecl = TRUE;

 /* change to proper type of wire even if already used */
 /* io type for parameter unused instead used for wire type */
 np->iotyp = NON_IO;
 np->n_isaparam = TRUE;
 np->nu.ct->p_locparam = is_local_param;
 if (is_spec) np->nu.ct->p_specparam = TRUE;
 
 /* if has range know is vector */ 
 if (x1 != NULL) { np->n_isavec = TRUE; np->vec_scalared = TRUE; }
 np->nu.ct->nx1 = x1;
 np->nu.ct->nx2 = x2;

 /* notice already linked into wire list - must also link into param list */
 /* link on end to preserve order */
 /* link the LOCAL parameters on a seperate list than the regular params */
 if (is_local_param)
  {
   if (__cur_declobj == MODULE)
    {
     if (__end_loc_paramnp == NULL) __inst_mod->mlocprms = np;
     else __end_loc_paramnp->nu2.nnxt = np;
     __end_loc_paramnp = np;
    }
   else if (__cur_declobj == TASK)
    {
     if (__end_tsk_loc_paramnp == NULL) __cur_tsk->tsk_locprms = np;
     else __end_tsk_loc_paramnp->nu2.nnxt = np;
     __end_tsk_loc_paramnp = np;
    }
   else __case_terr(__FILE__, __LINE__); 
  }
 else
  {
   /* regular parameter list */
   if (__cur_declobj == MODULE)
    {
     /* module parameter declaration */
     if (__end_paramnp == NULL) __inst_mod->mprms = np;
     else __end_paramnp->nu2.nnxt = np;
     __end_paramnp = np;
    }
   else if (__cur_declobj == SPECIFY)
    {
     if (__end_msprms == NULL) __cur_spfy->msprms = np;
     else __end_msprms->nu2.nnxt = np;
     __end_msprms = np;
    }
   else if (__cur_declobj == TASK)
    {
     if (__end_tskparamnp == NULL) __cur_tsk->tsk_prms = np;
     else __end_tskparamnp->nu2.nnxt = np;
     __end_tskparamnp = np;
    }
   else __case_terr(__FILE__, __LINE__); 
  }
 return(np);
}

/*
 * read a continuous assign module item
 * assign statement read and reads ending ;
 * return F if cannot sync to ending ;, if F will be sync to next mod/prim
 * list of assignments allowed here
 */
static int32 rd_contassign(void)
{
 struct expr_t *lhsndp, *rhsndp;
 struct paramlst_t *pmphdr;
 struct conta_t *cap;
 int32 first_time, sfnind, slcnt;

 /* must read drive strength and delay */
 pmphdr = NULL;
 __v0stren = __v1stren = NO_STREN;

 __get_vtok();
 if (__toktyp == LPAR)
  {
   __get_vtok();
   if (!rd_verstrens())
    {
     if (!__vskipto2_any(RPAR, SEMI)) return(FALSE);
     if (__toktyp == RPAR) { __get_vtok(); goto rd_parms; }
     return(TRUE);
    }
  }

rd_parms:
 if (__toktyp == SHARP)
  {
   if (!rd_oparamdels(&pmphdr))
    {
bad_end:
     return(__vskipto_any(SEMI));
    }
  }
 for (first_time = TRUE;;)
  {
   sfnind = __cur_fnam_ind;
   slcnt = __lin_cnt;
   /* collect lhs */
   if (!__col_lval()) goto bad_end;
   __bld_xtree(0);

   lhsndp = __root_ndp;
   __get_vtok();
   if (!__col_comsemi(-1)) goto bad_end;
   __bld_xtree(0);
   rhsndp = __root_ndp;
   cap = add_conta(lhsndp, rhsndp, sfnind, slcnt);
   if (first_time) cap->ca_du.pdels = pmphdr;
   else cap->ca_du.pdels = __copy_dellst(pmphdr);

   if (__v0stren != NO_STREN)
    {
     cap->ca_hasst = TRUE;
     cap->ca_stval = ((__v0stren << 3) | __v1stren) & 0x3f;
    }
   if (__toktyp == SEMI) break;
   if (__toktyp != COMMA) 
    {
     /* try to sync to next list el. if present */
     if (!__vskipto2_any(COMMA, SEMI)) return(FALSE);
     if (__toktyp == SEMI) break;
    }
   first_time = FALSE;
   __get_vtok();
  }
 return(TRUE);
}

/*
 * add a continuous assignment form cell
 * even if lhs 1 bit stored in non gate form here
 */
static struct conta_t *add_conta(struct expr_t *lhsndp,
 struct expr_t *rhsndp, int32 sfnind, int32 slcnt)
{
 struct conta_t *cap;
 struct sy_t *syp;

 syp = __bld_loc_symbol(__conta_num, __venviron[0], "assgn",
  "continuous assign");
 syp->sytyp = SYM_CA;
 /* this is place of declaration */
 syp->syfnam_ind = sfnind;
 syp->sylin_cnt = slcnt;
 syp->sydecl = TRUE;

 cap = (struct conta_t *) __my_malloc(sizeof(struct conta_t));
 cap->casym = syp;
 syp->el.ecap = cap;
 cap->ca_hasst = FALSE;
 cap->ca_stval = ST_STRVAL;
 cap->ca_delrep = DT_CMPLST;
 cap->ca_du.pdels = NULL; 
 cap->ca_4vdel = FALSE;
 cap->ca_gone = FALSE;
 cap->ca_pb_sim = FALSE;
 cap->ca_pb_el = FALSE;
 cap->lhsx = lhsndp;
 cap->rhsx = rhsndp;
 cap->caschd_tevs = NULL; 
 cap->ca_drv_wp.wp = NULL;
 cap->schd_drv_wp.wp = NULL;
 /* SJM 12/19/04 - when chk contas done - contas now tab of size m ca num */
 /* removed nxt field from conta type - can be in pbca's built in prp pass */
 cap->pbcau.canxt = NULL;
 /* link on to list */
 if (__end_ca == NULL) __inst_mod->mcas = cap;
 else __end_ca->pbcau.canxt = cap;
 __end_ca = cap;
 __conta_num++;
 return(cap);
}

/*
 * read an event declaration
 *
 * parsing routine to read and declare an event decl (can't be array/vec)
 * need to declare as task var if reading task/func/named block
 */
static int32 rd_eventdecl(int32 reading_tsk)
{
 int32 first_time, has_attr;
 struct net_t *np;

 /* use local has attr flag so can turn glb seen of before return */
 if (__wrk_attr.attr_seen) { has_attr = TRUE; __wrk_attr.attr_seen = FALSE; }
 else has_attr = FALSE;
 for (first_time = TRUE;;)
  {
   __get_vtok();
   if (__toktyp != ID)
    {
     __pv_ferr(1029,
      "event declaration event name expected - %s read", __prt_kywrd_vtok());
     goto try_resync;
    }
   /* since no range, if fails just try next one*/
   if (reading_tsk) np = decl_taskvar(N_EVENT, NULL, NULL);
   else np = __decl_wirereg(N_EVENT, NULL, NULL, NULL);

   /* SJM - 03/20/00 - save wire decl attrs */
   if (has_attr)
    {
     /* until Verilog 2000 will not see also event port attrs */
     if (np != NULL) add_net_attr(np, EVENT);
    }

   if (first_time) first_time = FALSE;

   __get_vtok();
   if (__toktyp == SEMI) break;
   if (__toktyp != COMMA)
    {
     __pv_ferr(1033,
      "event declaration comma or semicolon separator expected - %s read",
      __prt_vtok());
     /* try to resync */
try_resync:
     if (!__vskipto2_any(COMMA, SEMI)) return(FALSE); 
     if (__toktyp == SEMI) break;
    }
  }
 return(TRUE);
}

/*
 * allocate a statement entry
 * for empty statement (;) never get here
 */
extern struct st_t *__alloc_stmt(int32 styp)
{
 struct st_t *stp;
 struct for_t *frp;
 struct qconta_t *qcafs;

 stp = (struct st_t *) __my_malloc(sizeof(struct st_t));
 __init_stmt(stp, styp);
 (__inst_mod->mstnum)++; 

 /* DBG remove --
 if (__debug_flg)
  {
    extern char *__to_sttyp(char *, word32);

    __dbg_msg("AT %s %s - STMT ALLOC (%s)\n",
     __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt),
     __inst_mod->msym->synam, __to_sttyp(__xs2, styp));
  }
 --- */
 /* DBG remove --
 if (__debug_flg)
  {
   extern char *__to_sttyp(char *, unsigned);

   __dbg_msg("%04d: AT %s %s - STMT ALLOC (%s)\n",
    __inst_mod->mstnum - 1, __bld_lineloc(__xs, stp->stfnam_ind,
    stp->stlin_cnt), __inst_mod->msym->synam, __to_sttyp(__xs2, styp));
  }
 --- */
 /* ALTERNATE DBG remove --
 if (__debug_flg)
  {
   extern char *__to_sttyp(char *, unsigned);

   __dbg_msg("AT %s %s - STMT ALLOC %04d (%s)\n",
    __bld_lineloc(__xs, stp->stfnam_ind, stp->stlin_cnt),
    __inst_mod->msym->synam, __inst_mod->mstnum - 1, __to_sttyp(__xs2, styp));
  }
 --- */

 switch ((byte) styp) {
  /* null just has type value and NULL pointer */
  case S_NULL: case S_STNONE: break;
  case S_PROCA: case S_FORASSGN: case S_RHSDEPROCA: case S_NBPROCA:
   stp->st.spra.lhsx = NULL;
   stp->st.spra.rhsx = NULL;
   break;
  case S_IF:
   stp->st.sif.condx = NULL;
   stp->st.sif.thenst = NULL;
   stp->st.sif.elsest = NULL;
   break;
  case S_CASE:
   stp->st.scs.castyp = UNDEF;
   stp->st.scs.maxselwid = 0;
   stp->st.scs.csx = NULL;
   stp->st.scs.csitems = NULL;
   break;
  case S_REPEAT:
   stp->st.srpt.repx = NULL;
   stp->st.srpt.reptemp = NULL;
   stp->st.srpt.repst = NULL;
   break;
  case S_FOREVER:
  case S_WHILE:
   stp->st.swh.lpx = NULL;
   stp->st.swh.lpst = NULL;
   break;
  case S_WAIT:
   stp->st.swait.lpx = NULL;
   stp->st.swait.lpst = NULL;
   stp->st.swait.wait_dctp = __alloc_dctrl();
   break;
  case S_FOR:
   frp = (struct for_t *) __my_malloc(sizeof(struct for_t));
   stp->st.sfor = frp;
   frp->forassgn = NULL;
   frp->fortermx = NULL;
   frp->forinc = NULL;
   frp->forbody = NULL;
   break;
  case S_DELCTRL:
   stp->st.sdc = __alloc_dctrl();
   break;
  case S_NAMBLK:
   stp->st.snbtsk = NULL;
   break;
  case S_UNBLK:
   stp->st.sbsts = NULL;
   break;
  case S_UNFJ:
   stp->st.fj.fjstps = NULL;
   stp->st.fj.fjlabs = NULL;
   break;
  case S_TSKCALL:
   stp->st.stkc.targs = NULL;
   stp->st.stkc.tsksyx = NULL;
   stp->st.stkc.tkcaux.trec = NULL;
   break;
  case S_QCONTA:
   /* SJM 06/23/02 - since pre-building dce for qcaf need more fields */
   qcafs = (struct qconta_t *) __my_malloc(sizeof(struct qconta_t));
   stp->st.sqca = qcafs;
   qcafs->qcatyp = UNDEF;
   qcafs->regform = FALSE;
   qcafs->qclhsx = NULL;
   qcafs->qcrhsx = NULL;
   qcafs->rhs_qcdlstlst = NULL;
   break;
  case S_QCONTDEA:
   stp->st.sqcdea.qcdatyp = UNDEF;
   stp->st.sqcdea.qcdalhs = NULL;
   break;
  case S_CAUSE:
   stp->st.scausx = NULL;
   break;
  case S_DSABLE:
   stp->st.sdsable.dsablx = NULL;
   stp->st.sdsable.func_nxtstp = NULL;
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
  case S_GOTO:
   stp->st.sgoto = NULL;
   break;
  default: __case_terr(__FILE__, __LINE__);
  }
 return(stp);
}

/*
 * initialize stmt
 */
extern void __init_stmt(struct st_t *stp, int32 styp)
{
 stp->stlin_cnt = __lin_cnt;
 stp->stfnam_ind = __cur_fnam_ind;
 stp->stmttyp = styp;
 stp->st_unbhead = FALSE;
 /* notice this always has the statement type even if break point not set */
 stp->rl_stmttyp = styp;
 stp->strb_seen_now = FALSE;
 stp->lpend_goto = FALSE;
 stp->dctrl_goto = FALSE;
 stp->lstend_goto = FALSE;
 stp->st_schd_ent = FALSE;
 stp->lpend_goto_dest = FALSE;
 /* assume if non blocking need the sched tev table */
 stp->stnxt = NULL;
}

/*
 * allocate and initialize a delay control record
 */
extern struct delctrl_t *__alloc_dctrl(void)
{
 struct delctrl_t *dctp;

 dctp = (struct delctrl_t *) __my_malloc(sizeof(struct delctrl_t));
 dctp->dctyp = DC_NONE;
 dctp->dc_iact = FALSE;
 dctp->dc_delrep = DT_CMPLST;
 dctp->dc_nblking = FALSE;
 dctp->implicit_evxlst = FALSE;
 dctp->dc_du.pdels = NULL;
 dctp->repcntx = NULL;
 dctp->dceschd_tevs = NULL;
 dctp->actionst = NULL;
 /* this is allocated during prep - nil for now */
 dctp->dce_repcnts = NULL;
 return(dctp);
}

/*
 * variant of alloc stmt that takes "real" location as args
 */
extern struct st_t *__alloc2_stmt(int32 styp, int32 fnind, int32 lcnt)
{
 int32 sav_fnami, sav_flini;
 struct st_t *stp;

 sav_fnami = __cur_fnam_ind;
 sav_flini = __lin_cnt;
 __cur_fnam_ind = fnind;
 __lin_cnt = lcnt;

 stp = __alloc_stmt(styp);

 __cur_fnam_ind = sav_fnami;
 __lin_cnt = sav_flini;
 return(stp);
}

/*
 * DEFPARAM READING ROUTINES
 */

/*
 * read the parameter defparam statement
 * assigns values to this or other params
 * form: defparam [hieriarch path or id] = [param. value expr.], ... ;
 *
 * if returns F synced to next mod/prim else synced to ;
 * list of assignments allowed here
 */
static int32 rd_dfparam_stmt(void)
{
 struct dfparam_t *dfpp;
 struct expr_t *lhsndp;

 for (;;)	
  {
   __get_vtok();
   /* notice only for lhs not right */
   if (!__col_lval())
    {
     /* part of lhs may have been built */
     if (!__vskipto3_any(EQ, COMMA, SEMI)) return(FALSE);
     if (__toktyp == EQ) goto do_rhs; 
     if (__toktyp == COMMA) continue;
     return(TRUE);
    }
do_rhs:
   __bld_xtree(0);
   lhsndp = __root_ndp;
   if (lhsndp->optyp != ID && lhsndp->optyp != GLBREF)
    {
     __pv_ferr(1034, "defparam lvalue %s not identifier or hierarchical path",
      __msgexpr_tostr(__xs, lhsndp));
    }
   /* know = read */
   __get_vtok();
   if (!__col_comsemi(-1))
    {
err_cont:
     if (!__vskipto2_any(COMMA, SEMI)) return(FALSE);
     if (__toktyp == COMMA) continue;
     return(TRUE);
    }
   __bld_xtree(0);

   /* checking rhs does no evaluation but set sizes and checks for only */
   /* numbers and previously defined in source order parameters */
   /* defparam rhs params must be defined previously in module */
   /* notice must be converted to number immediately else can have */
   /* circular dependency */
   /* LRM requires source order definition before use */
   /* SJM 08/07/96 */
   if (__expr_has_glb(__root_ndp) || !__src_rd_chk_paramexpr(__root_ndp, 0))
    {
     __pv_ferr(1025,
      "defparam right hand side \"%s\" illegal - parameters and constants only",
      __msgexpr_tostr(__xs, __root_ndp));
     /* need to still add value of x to prevent further errors */
     __free2_xtree(__root_ndp);
     __root_ndp->szu.xclen = WBITS;
     __set_numval(__root_ndp, ALL1W, ALL1W, WBITS);  
    }
   /* SJM 01/27/04 - must leave as rhs expr since just gets evaled */
   /* bug if convert when downward propagation of defparams used on rhs */
   /* of other defparams used */

   /* notice cannot check rhs here since defparam statement rhs can be */
   /* any local parameter(possibly defined later in source) from LRM */ 
   dfpp = alloc_dfpval();
   dfpp->dfpxlhs = lhsndp;
   dfpp->dfpxrhs = __root_ndp;
   dfpp->in_mdp = __inst_mod;

   /* must always put on end of list - order important */
   /* since reading in source order list stays in source order */
   /* within each module */
   if (__end_dfp == NULL) __inst_mod->mdfps = dfpp;
   else __end_dfp->dfpnxt = dfpp;
   __end_dfp = dfpp;

   /* can be comma separated assignment list */
   if (__toktyp == SEMI) break;
   if (__toktyp != COMMA)
    {
     __pv_ferr(1039,
      "defparam semicolon or comma separator expected - %s read",
      __prt_vtok());
     goto err_cont;
    }
  }
 return(TRUE);
}

/*
 * allocate a module defparam structure for later processing
 */
static struct dfparam_t *alloc_dfpval(void)
{
 struct dfparam_t *dfpp;

 dfpp = (struct dfparam_t *) __my_malloc(sizeof(struct dfparam_t));
 dfpp->dfpxlhs = NULL;
 dfpp->dfpxrhs = NULL;
 dfpp->in_mdp = NULL;
 dfpp->dfpfnam_ind = __cur_fnam_ind;
 dfpp->dfplin_cnt = __lin_cnt;
 dfpp->dfp_local = FALSE;
 dfpp->dfp_rooted = FALSE;
 dfpp->dfp_done = FALSE;
 dfpp->dfp_mustsplit = FALSE;
 dfpp->dfp_has_idents = FALSE;
 dfpp->dfp_checked = FALSE;
 dfpp->dfpnxt = NULL;

 dfpp->dfpiis = NULL;
 dfpp->last_dfpi = -1;
 dfpp->gdfpnam = NULL;
 dfpp->targsyp = NULL;
 dfpp->idntmastdfp = NULL;
 dfpp->idntnxt = NULL;
 dfpp->rooted_dfps = NULL;
 dfpp->dfptskp = NULL;
 return(dfpp);
}

/*
 * TASK DEFINITION ROUTINES
 * here because uses common declaration code
 */

/*
 * process a task definition
 * know task or function keyword read, reads the endtask
 * no F return on error, since either build d.s. or not
 */
static int32 rd_task(void)
{
 struct st_t *stp;

 __lofp_port_decls = FALSE;
 __get_vtok();
 if (__toktyp != ID)
  {
   __pv_ferr(1130, "task name expected - %s read", __prt_kywrd_vtok());
sync_to_endtask:
   /* could change to dummy tsk name and continue here */
   /* but for now must find ENDTASK or skip to module level item */
   if (__vskipto_modend(ENDTASK)) return(TRUE);
   __syncto_class = SYNC_FLEVEL;
   return(FALSE);
  }
 /* FALSE means previously defined - must not read */
 if (!__bld_tsk(__token, TASK)) goto sync_to_endtask;
  
 __get_vtok();
 if (__toktyp == LPAR)
  {
   /* if couldn't sync to end of list of tf decls list ); */
   if (!rd_tf_list_of_ports_decl(__cur_tsk, "task"))
    {
     switch ((byte) __syncto_class) {
      case SYNC_FLEVEL: case SYNC_MODLEVEL: return(FALSE);
      case SYNC_STMT: __get_vtok(); goto more_stmts;
      default: __case_terr(__FILE__, __LINE__);
     }
    }
   if (__toktyp == RPAR) __get_vtok();
  }
 if (__toktyp != SEMI)
  {
   __pv_ferr(1131,
    "task declaration name not followed by semicolon - %s read",
    __prt_vtok());
  }
 else __get_vtok();

 /* first decl. type token read */
 if (!__rd_tfdecls("task")) return(FALSE);
more_stmts:
 if ((stp = __rd_stmt()) == NULL)
  {
   switch ((byte) __syncto_class) {
    case SYNC_FLEVEL: case SYNC_MODLEVEL: return(FALSE);
    case SYNC_STMT: __get_vtok(); goto more_stmts;
    case SYNC_TARG: break;
    default: __case_terr(__FILE__, __LINE__);
   }
  }
 /* only one task statement */
 __cur_tsk->tskst = stp;
 __get_vtok();
 if (__toktyp != ENDTASK)
  {
   __pv_ferr(1132, "endtask expected - %s read", __prt_vtok());
   /* must free task symbol table on error */
   if (__top_sti > 0) __top_sti = 0;
   /* this will sync to next module item if possible */
   return(__vskipto_any(ENDTASK));
  }
 __cur_tsk->tsk_last_lini = __lin_cnt;
 __cur_tsk->tsk_last_ifi = __cur_fnam_ind;

 __get_vtok();
 if (__toktyp == SEMI)
  __pv_ferr(999, "semicolon following endtask illegal"); 
 else __unget_vtok();
 /* if error will not get linked in to module's task list */
 if (__end_tbp == NULL) __inst_mod->mtasks = __cur_tsk;
 else __end_tbp->tsknxt = __cur_tsk;
 __end_tbp = __cur_tsk;
 /* symbols no longer accessible */
 __top_sti--;
 return(TRUE);
}

/*
 * build a task structure from declaration (label block/task/function)
 * expects task name and return task symbol - __cur_tsk set to inited task
 * notice that this does not link task into task chain
 */
extern int32 __bld_tsk(char *tnam, int32 tsktok)
{
 int32 tstyp;
 struct symtab_t *upsyt;
 struct sy_t *syp;
 char s1[RECLEN], s2[RECLEN];

 if (*tnam == '$')
  {
   __pv_ferr(1133, "system function or task %s cannot be redefined", tnam);
   return(FALSE);
  }
 tstyp = to_tasksytyp(tsktok);
 
 /* must be find sym since if redefines something higher error */
 syp = __find_sym(tnam);
 if (!__sym_is_new)
  {
   /* if disable caused decl. as task, change to named block */
   if (syp->sytyp == SYM_TSK && tstyp == SYM_LB) syp->sytyp = SYM_LB;
   if (syp->sydecl || syp->sytyp != tstyp)
    {
     __pv_ferr(1134,
     "%s definition of \"%s\" illegal - previously defined as %s at %s",
      __get_vkeynam(s1, tsktok), __token, __to_sytyp(s2, syp->sytyp),
      __bld_lineloc(__xs, syp->syfnam_ind, syp->sylin_cnt));
     return(FALSE);
    }
  }
 /* possibilities include named block fork and join */
 syp->sytyp = tstyp;
 __cur_tsk = __alloc_task(syp);
 /* all different task types still point to etskp element */
 syp->el.etskp = __cur_tsk;
 syp->sydecl = TRUE;
 syp->syfnam_ind = __cur_fnam_ind;
 syp->sylin_cnt = __lin_cnt;

 /* must also allocate the new symbol table */
 __cur_tsk->tsksymtab = __alloc_symtab(TRUE);
 __cur_tsk->tsktyp = tsktok;
 /* link symbol table to object it is symbol of */
 __cur_tsk->tsksymtab->sypofsyt = syp;
 /* set list ends for elements that must be kept in order */
 __end_tpp = NULL;
 __end_tskparamnp = NULL;
 __end_tsk_loc_paramnp = NULL;
 __venviron[++__top_sti] = __cur_tsk->tsksymtab;

 /* link in symbol table structure */
 upsyt = __venviron[__top_sti - 1];
 __cur_tsk->tsksymtab->sytpar = upsyt;
 if (upsyt->sytofs == NULL) upsyt->sytofs = __cur_tsk->tsksymtab;
 /* link on front */
 else
  {
   __cur_tsk->tsksymtab->sytsib = upsyt->sytofs;
   upsyt->sytofs = __cur_tsk->tsksymtab;
  }
 return(TRUE);
}

/*
 * convert one of the task token types to corresponding symbol type
 */
static word32 to_tasksytyp(int32 ttyp)
{
 int32 styp;

 styp = SYM_UNKN;
 switch ((byte) ttyp) {
  case TASK: styp = SYM_TSK; break;
  case FUNCTION: styp = SYM_F; break;
  case FORK: case Begin: styp = SYM_LB; break;
  default: __case_terr(__FILE__, __LINE__);
 }
 return(styp);
}

/*
 * allocate a new task
 */
extern struct task_t *__alloc_task(struct sy_t *syp)
{
 struct task_t *tskp;

 tskp = (struct task_t *) __my_malloc(sizeof(struct task_t));
 init_task(tskp);
 tskp->tsksyp = syp;
 return(tskp);
}

/*
 * initialize a task 
 */
static void init_task(struct task_t *tskp)
{
 tskp->tsksyp = NULL;
 tskp->tsk_last_lini = -1;
 tskp->tsk_last_ifi = -1;
 tskp->tsktyp = UNDEF;
 tskp->t_used = FALSE;
 tskp->thas_outs = FALSE;
 tskp->thas_tskcall = FALSE;
 tskp->fhas_fcall = FALSE;
 tskp->tf_lofp_decl = FALSE;
 tskp->tsksymtab = NULL;
 tskp->st_namblkin = NULL;
 tskp->tskpins = NULL;
 tskp->tsk_prms = NULL;
 tskp->tsk_locprms = NULL;
 tskp->tprmnum = 0;
 tskp->tsk_regs = NULL;
 tskp->trnum = 0;
 tskp->tlocprmnum = 0;
 tskp->tskst = NULL;
 tskp->tsknxt = NULL;
 tskp->tthrds = NULL;
}

/*
 * ROUTINES TO READ AND ADD LIST OF PORTS STYLE TASK/FUNC HEADER PORT DECLS 
 */

/*
 * read list of task/func header port declarations
 * new alternative ANSII style port header decl form added to 2001 LRM
 *
 * initial ( read and reads ending );
 * think now () form legal
 *
 * if return T, even if error parsing can continue in module
 * on error must sync to end of tf list of decls ')' - if not returns F
 * and sets sync class to right place to continue in t/f
 * may also sync to ; on error with T return
 */
static int32 rd_tf_list_of_ports_decl(struct task_t *tskp, char *tftypnam)
{
 int32 first_time, wtyp, ptyp, attr_ttyp, has_attr, decl_signed;
 struct sy_t *syp;
 struct net_t *np;
 struct expr_t *x1, *x2, *ox1, *ox2;
 struct task_pin_t *tpp;
 char s1[RECLEN];

 ptyp = -1;
 /* even if syntax error, T once a port type keyword appears in hdr */
 tskp->tf_lofp_decl = TRUE;
 __lofp_port_decls = TRUE;

 __get_vtok();
 for (;;)
  {
   if (__toktyp == RPAR)
    {
     __pv_fwarn(3136,
      "%s %s header list of ports decl form - but list of ports empty",
      tftypnam, tskp->tsksyp->synam);
     /* assuming this forces list of ports header form */ 
     return(TRUE);
    }

   if (__toktyp != INPUT && __toktyp != OUTPUT && __toktyp != INOUT)
    {
     __pv_ferr(3422, "%s list of ports form port direction expected - %s read",
      tftypnam, __prt_kywrd_vtok());
     if (!__vskipto_lofp_end()) return(FALSE); 
     return(TRUE);
    }

   /* attribute collected by scanner - but need to save so associates with */
   /* right port */
   if (__attr_prefix)
    {
     __wrk_attr.attr_tok = __toktyp;
     __wrk_attr.attr_seen = TRUE;
     /* for now this is unparsed entire attr. string */
     __wrk_attr.attrnam = __pv_stralloc(__attrwrkstr);
     __wrk_attr.attr_fnind = __attr_fnam_ind;
     __wrk_attr.attrlin_cnt = __attr_lin_cnt;
    }
   else __wrk_attr.attr_seen = FALSE;

   attr_ttyp = __toktyp; 
   if (__toktyp == INPUT) ptyp = IO_IN;
   else if (__toktyp == OUTPUT) ptyp = IO_OUT;
   else if (__toktyp == INOUT) ptyp = IO_BID;
   else __case_terr(__FILE__, __LINE__); 

   __get_vtok();

   /* defaults to reg if net type omitted - can be var/reg type */
   if ((wtyp = __fr_wtnam(__toktyp)) != -1) __get_vtok();
   else wtyp = N_REG;

   if (wtyp == N_INT || wtyp == N_REAL) decl_signed = TRUE;
   else decl_signed = FALSE;
   /* vectored or scalared keywords never appear in port decls */

   if (__toktyp == SIGNED)
    {
     decl_signed = TRUE;
     if (wtyp == N_TIME || wtyp == N_INT || wtyp == N_REAL
      || wtyp == N_EVENT) 
      {
       __pv_ferr(3423,
        "signed keyword illegal when task or function variable has type %s",
        __to_wtnam2(s1, wtyp));
      }
     __get_vtok();
    }

   /* even if error if 1 past ending ] continue */
   if (!__rd_decl_rng(&ox1, &ox2))
    {
     /* bad decl - but if sync to new I/O port direction, caller will cont */
     if (!__vskipto_lofp_end()) return(FALSE); 
     if (__toktyp == RPAR) return(TRUE); 
     /* semi read */
     return(TRUE);
    }

   /* use local has attr flag so can turn glb seen off before return */
   if (__wrk_attr.attr_seen)
    { has_attr = TRUE; __wrk_attr.attr_seen = FALSE; }
   else has_attr = FALSE;

   x1 = x2 = NULL;
   for (first_time = TRUE;;)
    {
     if (__toktyp != ID)
      {
       __pv_ferr(992,
        "%s header list of port form %s port name expected - %s read",
        tftypnam, __to_ptnam(s1, ptyp), __prt_kywrd_vtok());

       if (__vskipto2_lofp_end())
        {
         if (__toktyp == SEMI) return(TRUE);
         if (__toktyp == RPAR) { __get_vtok(); return(TRUE); }
         /* only other possibility is the port name separating comma */
         continue;
        }
       /* can't recover (resync) from error - synced to module item */
       return(FALSE);
      }

     /* SJM 05/25/04 - must just search for redeclare in tf sym tab */
     if ((syp = __get_sym(__token, __venviron[__top_sti])) != NULL)
      {
       __pv_ferr(3418,
        "%s header list of ports form %s port name %s redeclared", 
        tftypnam, __to_ptnam(s1, ptyp), __token);
       goto nxt_port;
      }

     if (first_time) { x1 = ox1; x2 = ox2; first_time = FALSE; } 
     else
      {
       if (x1 == NULL) x1 = x2 = NULL;
       else { x1 = __copy_expr(ox1); x2 = __copy_expr(ox2); }
      }

     /* first declare the port's wire/reg */
     if ((np = decl_taskvar(wtyp, x1, x2)) == NULL) goto nxt_port;

     /* if previously used will be treated as reg - must set to compatible */
     /* wire type if declared as time or int32 */
     syp = np->nsym;

     /* if saw an (* *) attribute for module item token, seen on */
     if (has_attr)
      {
       /* this add to net's attr list on end if also net decl first */
       add_net_attr(np, attr_ttyp);
      }

     /* SJM 10/02/03 - signed can be turned on either in port or wire decl */ 
     if (decl_signed) np->n_signed = TRUE;
     np->iotyp = ptyp;

     /* alloc port and add to end of list - order here crucial */
     tpp = alloc_tskpin();
     tpp->tpsy = syp;
     tpp->trtyp = ptyp;

     /* although with hdr list of ports form list known, for other form */
     /* don't know task/func ports until end of task/func */ 
     if (__end_tpp == NULL) __cur_tsk->tskpins = tpp;
     else __end_tpp->tpnxt = tpp;
     __end_tpp = tpp;

nxt_port:
     __get_vtok();
     if (__toktyp == RPAR) return(TRUE);

     if (__toktyp != COMMA)
      {
       __pv_ferr(995,
        "%s list of ports form declaration list comma or right paren expected - %s read",
        tftypnam, __prt_vtok());
       /* try to resync */
       if (!__vskipto_lofp_end()) return(FALSE); 
       if (__toktyp == COMMA) goto nxt_var;
       /* misplaced semi or sync to rpar */
       return(TRUE);
      }
nxt_var:
     __get_vtok();
     if (__toktyp == INPUT || __toktyp == OUTPUT || __toktyp == INOUT)  
      break;
    }
  }
 __misc_terr(__FILE__, __LINE__);
 return(TRUE);
}

/*
 * read task declarations
 * notice in any kind of task, function, or named block, decls come first
 * with one statement at end
 * expects first token to have been read and reads start of 1st statement
 */
extern int32 __rd_tfdecls(char *tftypnam)
{
 word32 wtyp, pntyp;

 for (;;)
  {
   /* routines called in switch expected to read ending ; or token */
   switch((byte) __toktyp) {
    case TEOF:
     __pv_ferr(1135, "%s unexpected EOF", tftypnam);
     return(FALSE);
    case PARAMETER:
    case LOCALPARAM:
     /* this add to symbol table and list */
     /* notice for these, if error but synced to ;, still returns T */
     if (!rd_paramdecl(FALSE, (__toktyp == LOCALPARAM))) 
      {
tfdecl_sync:
       switch ((byte) __syncto_class) {
        case SYNC_FLEVEL: return(FALSE);
        case SYNC_MODLEVEL: break;
        /* statement follows task decls */
        case SYNC_STMT: return(TRUE);
        case SYNC_TARG: break;
        default: __case_terr(__FILE__, __LINE__);
       }
      }
     break;
    case INOUT:
     pntyp = IO_BID;
     goto decl_port;
    case OUTPUT:
     pntyp = IO_OUT;
     goto decl_port;
    case INPUT: 
     pntyp = IO_IN;
decl_port:
     if (!rd_taskvardecl(pntyp, TRUE, tftypnam)) goto tfdecl_sync;
     break;
    case REG:
     wtyp = N_REG;
do_tfwdecl:
     if (!rd_taskvardecl(wtyp, FALSE, tftypnam)) goto tfdecl_sync;
     break;
    case TIME: wtyp = N_TIME; goto do_tfwdecl;
    case INTEGER: wtyp = N_INT; goto do_tfwdecl;
    case REAL: case REALTIME: wtyp = N_REAL; goto do_tfwdecl;
    case EVENT:
     if (!rd_eventdecl(TRUE)) goto tfdecl_sync;
     break;
    default:
     /* assume start of statement */
     goto decl_end;
   }
   __get_vtok();
  }
decl_end:
 return(TRUE);
}

/*
 * read and process a task reg/time/int/real declaration
 * know reg type read and reads final semi
 */
static int32 rd_taskvardecl(word32 regwtyp, int32 is_io, char *tftypnam)
{
 int32 decl_signed, first_time, ttyp, has_attr;
 word32 wtyp;
 struct sy_t *syp;
 struct net_t *np;
 struct task_pin_t *tpp;
 struct expr_t *x1, *x2, *ox1, *ox2, *xa1, *xa2;
 char s1[RECLEN], s2[RECLEN];

 ttyp = __toktyp;
 if (is_io) wtyp = N_REG; else wtyp = regwtyp;
 /* SJM 10/02/03 - need sign bit for reals even though always signed */
 if (wtyp == N_INT || wtyp == N_REAL) decl_signed = TRUE;
 else decl_signed = FALSE;
 __get_vtok();

 if (__toktyp == SIGNED)
  {
   decl_signed = TRUE;
   if (wtyp == N_TIME || wtyp == N_INT || wtyp == N_REAL || wtyp == N_EVENT) 
    {
     __pv_ferr(3423,
      "signed keyword illegal when task or function variable type %s",
      __to_wtnam2(s1, wtyp));
    }
   __get_vtok();
  }

 if (!__rd_decl_rng(&ox1, &ox2))
  {
   if (!__vskipto2_any(SEMI, RSB)) return(FALSE); 
   if (__toktyp == SEMI) return(TRUE);
   __get_vtok();
  }
 if (ox1 != NULL)
  {
   if (wtyp == N_INT || wtyp == N_TIME || wtyp == N_REAL
    || wtyp == N_EVENT)
    {
     __pv_ferr(1142, "%s %s %s vector range illegal", tftypnam,
      __to_wtnam2(s1, wtyp), __token);
     x1 = x2 = NULL;
    }
  }
 /* use local has attr flag so can turn glb seen of before return */
 if (__wrk_attr.attr_seen) { has_attr = TRUE; __wrk_attr.attr_seen = FALSE; }
 else has_attr = FALSE;
 for (first_time = TRUE;;)
  {
   if (__toktyp != ID)
    {
     __pv_ferr(1140, "%s declaration name of reg expected - %s read",
      tftypnam, __prt_kywrd_vtok());
bad_end:
     return(__vskipto_any(SEMI));
    }

   /* any port decl illegal - new ones or re-decls */
   if (__lofp_port_decls)
    {
     if (is_io)
      {
       __pv_ferr(3421,
       "port declaration of \"%s\" illegal - list of ports declaration form used",
       __prt_kywrd_vtok());
       /* if I/O decl, know read entire decl, i.e. can't be array */
       goto nxt_var; 
      }
     else
      {
       if (((syp = __get_sym_env(__token)) != NULL) && syp->sytyp == SYM_N
        && syp->el.enp->iotyp != NON_IO)
        {
         __pv_ferr(3421,
          "%s declaration of port \"%s\" illegal - %s uses list of ports declarations form",
          __to_wtnam2(s1, wtyp), __prt_kywrd_vtok(), tftypnam);

         /* here may need to skip the possible array decl */
         if (!__vskipto2_any(COMMA, SEMI)) return(FALSE); 
         if (__toktyp == SEMI) return(TRUE);
         goto nxt_var;
        }
      }
    }

   /* each time through need to call this to make copy */
   if (ox1 == NULL) set_reg_widths(wtyp, &x1, &x2);
   else if (first_time) { x1 = ox1; x2 = ox2; first_time = FALSE; }
   else { x1 = __copy_expr(ox1); x2 = __copy_expr(ox2); }

   /* this handles all normal wire setting and checking */
   if ((np = decl_taskvar(wtyp, x1, x2)) == NULL)
    {     
     /* if no np, cannot read possible declaration - must skip over it */
     if (__toktyp == LSB)
      {
       if (!__vskipto2_any(SEMI, RSB)) return(FALSE); 
       if (__toktyp == SEMI) return(TRUE);
       __get_vtok();
      } 
     goto nxt_var;
    }
   syp = np->nsym;

   /* SJM - 03/20/00 - save reg decl attrs */
   /* if saw an (* *) attribute for module item token, seen on */
   if (has_attr) add_net_attr(np, ttyp);

   __get_vtok();
   /* notice task/function ports do not allow array syntax */
   /* also notice no strength or wire delay syntax */
   if (is_io) xa1 = xa2 = NULL;
   else
    {
     if (!__rd_decl_rng(&xa1, &xa2))
      {
       if (!__vskipto2_any(SEMI, RSB)) return(FALSE); 
       if (__toktyp == SEMI) return(TRUE);
       __get_vtok();
      }
     if (xa1 != NULL && wtyp == N_EVENT)
      {
       __pv_ferr(1143, "%s %s %s cannot be a array", tftypnam,
        __to_wtnam2(s1, wtyp), syp->synam);
       xa1 = xa2 = NULL;
      }
    }
   if (xa1 != NULL)
    { np->n_isarr = TRUE; np->nu.ct->ax1 = xa1; np->nu.ct->ax2 = xa2; }
   if (decl_signed) np->n_signed = TRUE;

   if (is_io)
    {
     /* check for repeated I/O decls - wrong */
     if (np->iotyp != NON_IO)
      {
       if (np->iotyp == regwtyp)
        {
         __pv_fwarn(574, "%s %s port declaration of \"%s\" repeated",
	  tftypnam, __to_ptnam(s1, regwtyp), syp->synam);
	}
       else
        {
         __pv_ferr(1144, "%s %s port %s previously declared as %s port",
          tftypnam, __to_ptnam(s1, regwtyp), syp->synam,
          __to_ptnam(s2, np->iotyp));
	 goto nxt_var;
        }
      }
     np->iotyp = regwtyp;
     /* alloc port and add to end of list - order here crucial */
     tpp = alloc_tskpin();
     tpp->tpsy = syp;
     tpp->trtyp = regwtyp;
     if (__end_tpp == NULL) __cur_tsk->tskpins = tpp;
     else __end_tpp->tpnxt = tpp;
     __end_tpp = tpp;
    }

nxt_var:
   if (__toktyp == SEMI) break;
   if (__toktyp != COMMA)
    {
     __pv_ferr(1145,
      "%s reg declaration list comma or semicolon expected - %s read",
      tftypnam, __prt_vtok());
     goto bad_end;
    }
   __get_vtok();
  }
 return(TRUE);
}

/*
 * add the task variable type decl. symbol and associated reg
 * caller must set reg type after checking for duplicates
 * returns null on error
 */
static struct net_t *decl_taskvar(word32 wtyp, struct expr_t *x1,
 struct expr_t *x2)
{
 struct net_t *np;
 struct sy_t *syp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 syp = __decl_sym(__token, __venviron[__top_sti]);
 if (__sym_is_new)
  {
   np = __add_net(syp);
   np->iotyp = NON_IO;
   if (x1 != NULL)
    {
     np->nu.ct->n_rngknown = TRUE;
     np->n_isavec = TRUE;
     np->nu.ct->nx1 = x1;
     np->nu.ct->nx2 = x2;
    }
   np->ntyp = wtyp;
   syp->sydecl = TRUE;
   /* even if used before, must set to declaration place */
   syp->syfnam_ind = __cur_fnam_ind;
   syp->sylin_cnt = __lin_cnt;
  }
 else
  {
   if (syp->sytyp != SYM_N)
    {
     __pv_ferr(1028,
      "cannot declare %s as %s - previously declared as %s at %s",
      syp->synam, __to_wtnam2(s2, wtyp), __to_wtnam2(s1, syp->sytyp),
      __bld_lineloc(__xs, syp->syfnam_ind, syp->sylin_cnt));
     return(NULL);
    }
   /* wire/reg decl. after I/O decl. may set range */
   np = syp->el.enp;
   /* need special handling for I/O ports - declared in header, */
   /* I/O direction and maybe wire */
   if (np->iotyp != NON_IO)
    {
     /* task vars can be anthing providing output is lhs */
     /* this only has meaning for I/O port redecls */
     if (np->nu.ct->n_wirtypknown)
      {
       __pv_ferr(1146, "%s port %s previously declared as %s cannot be %s",
	__to_ptnam(s1, np->iotyp), syp->synam, __to_wtnam(s2, np),
	__to_wtnam2(s3, wtyp));
       /* must cause skipping because no np */
       return(NULL);
      }
    }
   else { if (is_decl_err(syp, SYM_N, wtyp)) return(NULL); }
   np->ntyp = wtyp;
   if (!chkset_wdrng(np, x1, x2)) return(NULL);
   np->nu.ct->n_wirtypknown = TRUE;

   /* if I/O decl follows reg decl, symbol uses I/O decl. place */
   if (np->iotyp != NON_IO)
    {
     syp->sydecl = TRUE;
     /* even if used before, must set to declaration place */
     syp->syfnam_ind = __cur_fnam_ind;
     syp->sylin_cnt = __lin_cnt;
    }
  }
 return(np);
}

/*
 * allocate a task pin (port) list element
 */
static struct task_pin_t *alloc_tskpin(void)
{
 struct task_pin_t *tpp;

 tpp = (struct task_pin_t *) __my_malloc(sizeof(struct task_pin_t));
 tpp->tpsy = NULL;
 tpp->trtyp = NON_IO;
 tpp->tpnxt = NULL;
 return(tpp);
}

/*
 * process a function definition
 * keyword function reads and reads decl. and final endfunction
 * no F return on error since either build d.s. or not
 */
static int32 rd_func(void)
{
 int32 frwtyp, decl_signed;
 word32 rhigh;
 struct st_t *stp;
 struct expr_t *x1, *x2, *dx1, *dx2;

 __lofp_port_decls = FALSE;
 dx1 = dx2 = NULL;
 decl_signed = FALSE;
 __get_vtok();
 if (__toktyp == SIGNED)
  {
   decl_signed = TRUE;
   __get_vtok();
  }

 if (__toktyp == LSB)
  {
   if (!__rd_decl_rng(&dx1, &dx2))
    {
     if (!__vskipto2_any(SEMI, RSB)) return(FALSE); 
     if (__toktyp == SEMI) return(TRUE);
     __get_vtok();
    }
  } 
 /* if not a wire - assume next token is func. name (if not error below) */
 /* this is 1 bit wire if not range */
 if ((frwtyp = __fr_wtnam(__toktyp)) == -1)
  { frwtyp = N_REG; x1 = dx1; x2 = dx2; }
 else
  {
   if (frwtyp == N_EVENT || frwtyp < NONWIRE_ST)
    {
     __pv_ferr(1141, "function cannot return type %s", __prt_vtok());
     frwtyp = N_REG;
     x1 = x2 = NULL;
     goto get_funcnam;
    }
  switch ((byte) frwtyp) {
   case N_REAL:
    rhigh = REALBITS - 1;
    goto chk_norng;
   case N_INT:
    rhigh = WBITS - 1;
chk_norng:
    if (dx1 != NULL)
     {
      __pv_ferr(1149,
       "function returning %s range illegal", __to_wtnam2(__xs,
       (word32) frwtyp));
     }
    x1 = __bld_rng_numxpr(rhigh, 0L, WBITS);
    x2 = __bld_rng_numxpr(0L, 0L, WBITS);
    break;
   case N_TIME:
    rhigh = TIMEBITS - 1;
    goto chk_norng;
   default:
    __case_terr(__FILE__, __LINE__);
    return(FALSE);
  }
  __get_vtok();
 }

get_funcnam:
 if (__toktyp != ID)
  {
   __pv_ferr(1148, "function name expected - %s read", __prt_kywrd_vtok());
no_sym:
   if (__vskipto_modend(ENDFUNCTION)) return(TRUE);
   __syncto_class = SYNC_FLEVEL;
   return(FALSE);
  }
 if (!__bld_tsk(__token, FUNCTION)) goto no_sym;
 
 if (decl_signed && (frwtyp == N_TIME || frwtyp == N_INT || frwtyp == N_REAL)) 
  {
   __pv_ferr(3423,
    "signed keyword illegal when function declaration return type %s",
    __to_wtnam2(__xs, frwtyp));
  }
 if (frwtyp == N_INT || frwtyp == N_REAL) decl_signed = TRUE;
 add_funcretdecl(__token, (word32) frwtyp, x1, x2, decl_signed);

 __get_vtok();
 if (__toktyp == LPAR)
  {
   /* if couldn't sync to end of list of tf decls list ); */
   if (!rd_tf_list_of_ports_decl(__cur_tsk, "function"))
    {
     switch ((byte) __syncto_class) {
      case SYNC_FLEVEL: case SYNC_MODLEVEL: return(FALSE);
      case SYNC_STMT: __get_vtok(); goto more_stmts;
      default: __case_terr(__FILE__, __LINE__);
     }
    }
   if (__toktyp == RPAR) __get_vtok();
  }

 if (__toktyp != SEMI)
  {
   __pv_ferr(1153,
    "function declaration name not followed by semicolon - %s read",
    __prt_vtok());
  }
 else __get_vtok();

 if (!__rd_tfdecls("function")) return(FALSE); 
more_stmts:
 if ((stp = __rd_stmt()) == NULL)
  {
   /* only get here on error */
   if (__toktyp == ENDFUNCTION)
    {
     __get_vtok();
     if (__toktyp == SEMI)
      __pv_ferr(999, "semicolon following endfunction illegal"); 
     else __unget_vtok();
     return(TRUE);
    }
   switch ((byte) __syncto_class) {
    case SYNC_FLEVEL: case SYNC_MODLEVEL: return(FALSE);
    /* legally only 1 stmt - but try to parse all */
    case SYNC_STMT:
     __get_vtok();
     goto more_stmts;
    case SYNC_TARG: break;
    default: __case_terr(__FILE__, __LINE__);
   }
  }
 __cur_tsk->tskst = stp;
 __get_vtok();
 if (__toktyp != ENDFUNCTION)
  {
   __pv_ferr(1154, "endfunction expected - %s read", __prt_vtok());
   /* if missing endfunction, need to deallocate symbol table */
   if (__top_sti > 0) __top_sti = 0;
   return(__vskipto_any(ENDFUNCTION));
  }
 /* set line of end */
 __cur_tsk->tsk_last_lini = __lin_cnt;
 __cur_tsk->tsk_last_ifi = __cur_fnam_ind;

 /* if error will not get linked in to module's task list */
 /* notice functions get linked in task order on module task list */
 if (__end_tbp == NULL) __inst_mod->mtasks = __cur_tsk;
 else __end_tbp->tsknxt = __cur_tsk;
 __end_tbp = __cur_tsk;
 /* symbols no longer accessible */
 __top_sti--;
 return(TRUE);
}

/*
 * add implicit first output port return value decl. to task d.s.
 */
static void add_funcretdecl(char *rvnam, word32 frwtyp,
 struct expr_t *x1, struct expr_t *x2, int32 decl_signed)
{
 struct sy_t *syp;
 struct net_t *np;
 struct task_pin_t *tpp;

 /* notice symbol already in one up task decl. also name of port */
 syp = __decl_sym(rvnam, __venviron[__top_sti]);
 /* function declaration symbol table inconsisent */
 if (!__sym_is_new) __misc_fterr(__FILE__, __LINE__);

 np = __add_net(syp);
 if (x1 != NULL)
  {
   np->nu.ct->n_rngknown = TRUE;
   np->n_isavec = TRUE;
   np->nu.ct->nx1 = x1;
   np->nu.ct->nx2 = x2;
  }
 np->iotyp = IO_OUT;
 np->ntyp = frwtyp;
 np->nu.ct->n_rngknown = TRUE;
 np->nu.ct->n_iotypknown = TRUE;
 np->nu.ct->n_wirtypknown = TRUE;
 if (decl_signed) np->n_signed = TRUE; else np->n_signed = FALSE;

 syp->sydecl = TRUE;
 syp->syfnam_ind = __cur_fnam_ind;
 syp->sylin_cnt = __lin_cnt;

 /* alloc port and add to end of list - order here crucial */
 tpp = alloc_tskpin();
 tpp->tpsy = syp;
 tpp->trtyp = IO_OUT;
 if (__end_tpp == NULL) __cur_tsk->tskpins = tpp;
 else __end_tpp->tpnxt = tpp;
 __end_tpp = tpp;
}

/*
 * INSTANCE READING ROUTINES
 */

/*
 * [module type] [# param list] [inst] ([mod connections]) ;
 * [udp/prim type] [strength] [#param list] [inst] ([prim. conn.]);
 */

/*
 * read an instance - treat as if not yet defined or resolved
 * module type name read, reads final ;
 * at this point do not know if instance, gate or udp
 * return F if synced to next mod/prim else T if synced to ; even if err
 */
static int32 rd_inst(char *typnam)
{
 int32 first_time, has_iname, strenval, has_attr;
 struct cell_t *cp;
 struct sy_t *syp;
 struct tnode_t *tnp;
 struct namparam_t *nprmhdr; 
 struct expr_t *x1, *x2;
 char s1[IDLEN];

 nprmhdr = NULL;
 first_time = TRUE;
 __v0stren = __v1stren = NO_STREN;
 has_iname = TRUE;
 x1 = x2 = NULL;

 /* must go here because for gate maybe no inam */
 /* use local has attr flag so can turn glb seen of before return */
 if (__wrk_attr.attr_seen) { has_attr = TRUE; __wrk_attr.attr_seen = FALSE; }
 else has_attr = FALSE;

 if (*typnam == '$')
  {
   __pv_ferr(1042,
   "instance/gate type \"%s\" cannot begin with $ - reserved for system tasks",
    typnam);
bad_end:
   return(__vskipto_any(SEMI));
  }
 __get_vtok();
 if (__toktyp == LPAR)
  {
   __get_vtok();
   if (is_tokstren(__toktyp) == NO_STREN)
    {
     sprintf(s1, "__gate$$%d", __cp_num);
     has_iname = FALSE;
     goto no_inam;
    }
   /* need special strength read routine for pull */
   if (strcmp(typnam, "pullup") == 0 || strcmp(typnam, "pulldown") == 0)
    {
     if (!rd_pull_stren(typnam, &strenval)) goto bad_stren;

     /* here syntax good but strength values illegal - assume strong */
     /* error already emitted */
     if (strenval == NO_STREN)
      {
       if (strcmp(typnam, "pullup") == 0) strenval = STRONG1;
       else strenval = STRONG0;
      }
     /* LOOKATME - both strength must be same and right selected one */
     /* since simualtion uses low 3 bits of strength value */ 
     __v0stren = __v1stren = strenval;  
     __get_vtok();
     goto rd_parms;
    }

   if (!rd_verstrens())
    {
bad_stren:
     if (!__vskipto2_any(RPAR, SEMI)) return(FALSE);
     if (__toktyp == RPAR) { __get_vtok(); goto rd_parms; }
     /* bad strengths do not provide any punctuation to sync to */
     return(TRUE);
    }
  }

rd_parms:
 if (__toktyp == SHARP)
  {
   if ((nprmhdr = rd_npndparams()) == NULL) goto bad_end;
  }
 else nprmhdr = NULL;

 for (;;)
  {
   if (__toktyp == LPAR)
    {
     /* name is [module name]$[unique number] */
     sprintf(s1, "__gate$$%d", __cp_num);
     has_iname = FALSE;
    }
   else
    {
     if (__toktyp != ID)
      {
       __pv_ferr(1043, "instance/gate name for type \"%s\" expected - %s read",
        typnam, __prt_kywrd_vtok());
       /* resyncing of comma list of same type insts not port lists */ 
       if (!__vskipto2_any(COMMA, SEMI)) return(FALSE); 
       if (__toktyp == SEMI) return(TRUE);
       __get_vtok();
       continue;
      }
     strcpy(s1, __token);
     __get_vtok();
     /* new arrays of gates/instances [h:l] becomes "_"[number] suffix later */
     /* if no range, this just returns T and sets x1 and x2 to nil */
     /* if range, sets x1, x2 and reads one past ] */
     if (!__rd_decl_rng(&x1, &x2))
      {
       if (!__vskipto2_any(SEMI, RSB)) return(FALSE); 
       if (__toktyp == SEMI) return(TRUE);
       __get_vtok();
      }
     if (__toktyp != LPAR)
      {
       __pv_ferr(1044,
        "instance/gate \"%s\" type \"%s\" connection list expected - %s read",
  	s1, typnam, __prt_vtok());
        goto bad_end;     
      }
    }
   __get_vtok();
no_inam:
   /* if port errors just inst. with no ports */
   if ((cp = add_cell(s1)) != NULL)
    {
     /* no checking for type sep. name space - if never decled err later */
     tnp = __vtfind(typnam, __modsyms);
     if (__sym_is_new)
      {
       __add_sym(typnam, tnp);
       (__modsyms->numsyms)++;
       syp = tnp->ndp;
       syp->sytyp = SYM_M;
       /* getting here means module/udp referenced before defined */
       /* and only place mod/udp can be seen */
       __add_syp_to_undefs(syp);
      }
     else syp = tnp->ndp;
     /* AIV 06/01/04 - mark all as not in config - config processing */ 
     /* will mark as true later */ 
     syp->cfg_needed = FALSE;
     /* instance must be named, error caught only after lib. processed */
     /* using inst. num that is unused until design wide checking */
     if (has_iname) cp->c_named = TRUE;
     if (x1 != NULL) { cp->cx1 = x1; cp->cx2 = x2; } 

     cp->cmsym = syp;
     if (first_time) cp->c_nparms = nprmhdr;
     else cp->c_nparms = copy_namparamlst(nprmhdr);
     if (__v0stren != NO_STREN) 
      {
       cp->c_hasst = TRUE;
       cp->c_stval = ((__v0stren << 3) | __v1stren) & 0x3f;
      }
    }
   /* if synced to ;, T even if errors */ 
   if (!rd_iports(s1)) return(TRUE);
   if (cp != NULL) cp->cpins = __cphdr;

   /* if saw an (* *) attribute for module item token, seen on */
   if (has_attr)
    {
     if (cp != NULL) add_cell_attr(cp);
    }

   __get_vtok();
   if (__toktyp == SEMI) break;
   first_time = FALSE;
   if (__toktyp == COMMA) { __get_vtok(); continue; }

   __pv_ferr(1048,
   "instance or gate terminal list ending semicolon or comma missing - %s read",
    __prt_vtok());
   /* must find a semi to continue */
   goto bad_end;
  }
 return(TRUE);
}

/*
 * add an inst attribute 
 */
static void add_cell_attr(struct cell_t *cp)
{
 /* DBG remove -- */
 if (cp->cattrs != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 /* need to set token type so each parsed attr_spec has right tok type */
 __wrk_attr.attr_tok = MODULE;

 /* return nil on error */
 cp->cattrs = __rd_parse_attribute(&__wrk_attr);
 /* SJM 07/30/01 - this is work read value, but now done with it */
 __my_free(__wrk_attr.attrnam, __attr_line_len + 1);
 __wrk_attr.attr_seen = FALSE;
}

/*
 * read the pull strength
 * know leading '(' and strength read, reads optional second , stren and
 * then ending ')' or just ending ')'
 * on F return trys to sync to ending ')' or ';'
 *
 * SJM 10/01/99 - allow both 0 and 1 strength where unused one dropped 
 * required by 1999 LRM
 *
 * where two strengths given, drops unused one here
 * know ehen called first token is some kind of strength
 */
static int32 rd_pull_stren(char *pullnam, int32 *strenval)
{
 int32 st1typ, st1val, st2val, st2typ, err_seen; 
 int32 strentyp, strenval1, strenval2;

 err_seen = FALSE;
 st1typ = st2typ = strentyp = NO_STREN;
 st1val = st2val = TOK_NONE;
 *strenval = NO_STREN;
 /* know 1st required */
 st1val = __toktyp;
 if ((st1typ = is_tokstren(__toktyp)) == CAP_STREN || st1typ == NO_STREN)
  {
   __pv_ferr(1032, "%s strength %s non driving or illegal", pullnam,
    __prt_vtok());  
   st1typ = NO_STREN;
   err_seen = TRUE;
  }

 __get_vtok();
 if (__toktyp == COMMA)
  {
   /* second strength present */
   __get_vtok();
   st2val = __toktyp;
   if ((st2typ = is_tokstren(__toktyp)) == CAP_STREN || st2typ == NO_STREN)
    {
     __pv_ferr(1032, "%s second strength %s non driving or illegal", pullnam,
      __prt_vtok());  
     st2typ = NO_STREN;
     err_seen = TRUE;
    }
   __get_vtok();
  }
 if (__toktyp != RPAR)
  {
   __pv_ferr(1031, "%s strength ending ')' expected - %s read", pullnam,
    __prt_vtok());
   return(FALSE);
  }
 if (err_seen) return(TRUE);

 /* know one or two strengths read and legal */
 /* case 1: old only one strength form - check to make sure right type */
 if (st2typ == NO_STREN)
  {
   strentyp = st1typ; 
   /* map from strength token to strength constant value */
   *strenval = __fr_stren_nam(st1val);
   if (strcmp(pullnam, "pullup") == 0)
    {
     if (strentyp == LOW_STREN) 
      {
       __pv_fwarn(608,
        "%s single strength form low (0) strength %s should be high (1) - changed", 
        pullnam, __to1_stren_nam(__xs, *strenval, st1typ));
      }
    } 
   else
    {
     if (strentyp == HIGH_STREN) 
      {
       __pv_fwarn(608,
        "%s single strength form high (1) strength %s should be low (0) - changed", 
        pullnam, __to1_stren_nam(__xs, *strenval, st1typ));
      }
    }
   if (*strenval == ST_HIGHZ)
    {
     __pv_ferr(1018,
      "highz strength illegal for single strength form %s gate",
      pullnam);
     strentyp = NO_STREN;
     *strenval = NO_STREN;
    }
   return(TRUE);
  }
 /* case 2 */
 /* make sure not both 0 strens and not both 1 strens */
 if (st1typ == LOW_STREN && st2typ == LOW_STREN)
  {
   __pv_ferr(1032, "%s two strength form both strengths low (0)", pullnam);
   return(TRUE);
  }
 if (st1typ == LOW_STREN && st2typ == LOW_STREN)
  {
   __pv_ferr(1032, "%s two strength form both strengths high (1)", pullnam);
   return(TRUE);
  }
 /* map from strength token to strength constant value for both */
 strenval1 = __fr_stren_nam(st1val);
 strenval2 = __fr_stren_nam(st2val);
 if (strenval1 == ST_HIGHZ || strenval2 == ST_HIGHZ)
  {
   __pv_ferr(1018, "highz strength illegal as either strength for %s gate",
    pullnam);
   strentyp = NO_STREN;
   *strenval = NO_STREN;
   return(TRUE);
  }
 /* select right strength from two */
 if (strcmp(pullnam, "pullup") == 0)
  {
   if (st1typ == LOW_STREN) *strenval = strenval2; else *strenval = strenval1;
  }
 else
  {
   if (st1typ == HIGH_STREN) *strenval = strenval2; else *strenval = strenval1;
  }
 return(TRUE);
}

/*
 * read a new style instance only old implicit or new explicit param form
 *
 * know # read and reads one past list ending ) - maybe only one and no ()
 * must also read new style for gates since until types resolved do not
 * know if gate or instance - error during fixup in new style for gate/udp
 */
static struct namparam_t *rd_npndparams(void)
{
 int32 prm_err;
 struct namparam_t *npmphdr, *npmp, *last_npmp;

 __get_vtok();
 /* case 1: old #[one token] case */
 if (__toktyp != LPAR)
  {
   /* notice must surround m:t:m with () */
   if (__toktyp != ID && __toktyp != NUMBER && __toktyp != REALNUM)
    {
     __pv_ferr(1049,
     "non parenthesized pound parameter one element identifier or number expected - %s read",
      __prt_kywrd_vtok());
     return(NULL);
    }
   __last_xtk = -1;
   /* on error, set as error expr. - maybe since param should be 0 */
   if (!__bld_expnode()) __set_xtab_errval();
   /* here does the allocating */
   __bld_xtree(0);
   npmphdr = __alloc_namparam();
   npmphdr->pxndp = __root_ndp;
   npmphdr->prmfnam_ind = __cur_fnam_ind;
   npmphdr->prmlin_cnt = __lin_cnt;
   __get_vtok();
   return(npmphdr);
  }
 
 /* case 2: #(...) - either , list (no empties) or explicit form */
 for (npmphdr = NULL, last_npmp = NULL, prm_err = FALSE;;)
  {
   __get_vtok();

   /* read the pound parameter (maybe new explicit form) and one token past */
   /* illegal forms caught during fix up */
   if ((npmp = rd1_namedparam()) == NULL) goto bad_skipend;
   if (npmphdr == NULL) npmphdr = npmp; else last_npmp->nprmnxt = npmp;
   last_npmp = npmp;

   if (__toktyp == RPAR) break;
   if (__toktyp == COMMA) continue;

   __pv_ferr(1051, "pound parameter list comma or ) expected - %s read ",
    __prt_vtok());

bad_skipend:
   prm_err = TRUE;
   if (!__vskipto3_any(RPAR, COMMA, SEMI)) return(FALSE); 
   if (__toktyp == COMMA) { __get_vtok(); continue; }
   /* if ) or ; done and synced to right place */
   if (__toktyp == SEMI) __unget_vtok();
   break;
  }
 __get_vtok();

 if (prm_err)
  {
   if (npmphdr != NULL) __free_namedparams(npmphdr);
   return(NULL);
  }
 return(npmphdr);
}

/*
 * read one instance (cell) or gate param (may have new named form)
 *
 * know 1st token read and reads punctuation after , or )
 * returns built named param record 
 *
 * on error returns nil, caller (not in this routine) tries to resync
 * on , and read next param * but on .[id](<some error> ..., this trys
 * to resync to list ending ) 
 *
 * LOOKATME - allowing ,, form
 */
static struct namparam_t *rd1_namedparam(void)
{
 int32 namedparam_form, slcnt, sfnind;
 struct namparam_t *npmp;
 char nam[IDLEN];

 slcnt = __lin_cnt;
 sfnind = __cur_fnam_ind;
 if (__toktyp == DOT)
  {
   __get_vtok();
   if (__toktyp != ID)
    {
     __pv_ferr(1052, "name of pound param expected - %s read",
      __prt_kywrd_vtok());
     return(NULL);
    }
   strcpy(nam, __token);
   __get_vtok();
   if (__toktyp != LPAR)
    {
     __pv_ferr(1053,
      "pound param explicitly named form left parenthesis expected - %s read",
      __prt_vtok());
     return(NULL);
    }
   /* 1st token in expr. must be read */
   __get_vtok();
   /* explicit param name .[param]() for unc. is legal */
   if (__toktyp == RPAR)
    {
     __last_xtk = 0;
     __set_opempty(0);
    }
   else
    {
     /* need to collect delay expr. because min-typ-max without () ok */
     if (!__col_delexpr()) return(NULL);
    }
   namedparam_form = TRUE;
  }
 else
  {
   namedparam_form = FALSE;
   /* (, - ,, and ,) all legal */
   if (__toktyp == COMMA || __toktyp == RPAR)
    {  __last_xtk = 0; __set_opempty(0); }
   else
    {
     /* need to collect delay expr. because min-typ-max without () ok */
     if (!__col_delexpr()) return(NULL);
    }
  }
 /* build the tree, copy/allocate nodes, sets __root_ndp to its root */
 /* this must be a constant expr but checked later - this will decl */
 __bld_xtree(0);

 if (namedparam_form)
  {
   if (__toktyp != RPAR)
    {
     __pv_ferr(1055,
      "explicitly named pound param form right parenthesis expected - %s read",
      __prt_vtok());
     return(NULL);
    }
   __get_vtok();
   /* LOOKATME - why is this check here - think no longer needed */
   /* but catching user or PLI sys function here does not hurt */
   if (__root_ndp->optyp == FCALL )
    {
     struct sy_t *syp; 

     /* only built in sysfuncs allowed - const args checked later */
     syp = __root_ndp->lu.x->lu.sy;
     if (syp->sytyp == SYM_SF && syp->el.esyftbp->tftyp == SYSF_BUILTIN)
      goto named_ok;

     __pv_ferr(1055,
      "explicitly named pound param %s illegal -  not required .[name]([value])",
      __msgexpr_tostr(__xs, __root_ndp));
     return(NULL);
    }
  }
named_ok:
 npmp = __alloc_namparam();
 npmp->pxndp = __root_ndp;
 if (namedparam_form) npmp->pnam = __pv_stralloc(nam);
 else npmp->pnam = NULL;
 npmp->prmfnam_ind = sfnind;
 npmp->prmlin_cnt = slcnt;
 return(npmp);
}

/*
 * free list of named param records
 */
extern void __free_namedparams(struct namparam_t *npmphdr)
{
 register struct namparam_t *npmp, *npmp2;
 int32 slen;

 for (npmp = npmphdr; npmp != NULL;)
  {
   npmp2 = npmp->nprmnxt;
   
   /* expr. may be nil, when freeing after expr. copied */
   if (npmp->pxndp != NULL) __free_xtree(npmp->pxndp);
   if (npmp->pnam != NULL)
    {
     slen = strlen(npmp->pnam);
     __my_free((char *) npmp->pnam, slen + 1); 
    }
   __my_free((char *) npmp, sizeof(struct namparam_t));

   npmp = npmp2;
  }
}

/*
 * add module symbol (possibly later changed to udp) to undef list
 */
extern void __add_syp_to_undefs(struct sy_t *syp)
{
 struct undef_t *undefp;

 undefp = (struct undef_t *) __my_malloc(sizeof(struct undef_t));
 undefp->msyp = syp;
 undefp->undefnxt = NULL;
 undefp->dfi = -1;
 undefp->modnam = NULL; 
 syp->syundefmod = TRUE;
 syp->el.eundefp = undefp;
 
 if (__undeftail == NULL)
  {
   __undeftail = __undefhd = undefp;
   undefp->undefprev = NULL;
  }
 else 
  {
   undefp->undefprev = __undeftail;
   __undeftail->undefnxt = undefp;
   __undeftail = undefp;
  } 
 __undef_mods++;
}

/*
 * copy possibly named param list 
 *
 * only needed during instance reading because converted to dellst form
 * when module copying needed
 */
static struct namparam_t *copy_namparamlst(struct namparam_t *old_npmp)  
{
 register struct namparam_t *npmphdr, *npmp, *onpmp, *last_npmp;
 
 if (old_npmp == NULL) return(NULL);

 npmphdr = NULL;
 last_npmp = NULL;
 for (onpmp = old_npmp; onpmp != NULL; onpmp = onpmp->nprmnxt)
  {
   npmp = (struct namparam_t *) __my_malloc(sizeof(struct namparam_t));
   /* since mallocing, need to fill all fields */
   npmp->prmfnam_ind = onpmp->prmfnam_ind;
   npmp->prmlin_cnt = onpmp->prmlin_cnt;
   npmp->pxndp = __copy_expr(onpmp->pxndp);
   if (onpmp->pnam != NULL) npmp->pnam = __pv_stralloc(onpmp->pnam);
   else npmp->pnam = NULL;
   npmp->nprmnxt = NULL;

   if (last_npmp == NULL) npmphdr = npmp; else last_npmp->nprmnxt = npmp;
   last_npmp = npmp;
  }
 return(npmphdr);
}

/*
 * allocate and initialize a inst/gate pound param record
 */
extern struct namparam_t *__alloc_namparam(void)
{
 struct namparam_t *npmp;

 npmp = (struct namparam_t *) __my_malloc(sizeof(struct namparam_t));
 npmp->pxndp = NULL;
 npmp->prmfnam_ind = 0;
 npmp->prmlin_cnt = -1;
 npmp->pnam = NULL;
 npmp->nprmnxt = NULL;
 return(npmp);
}

/*
 * make a copy of a param list
 * know at copy point delay is DT_CMPLST list
 */
extern struct paramlst_t *__copy_dellst(struct paramlst_t *oplp)
{
 register struct paramlst_t *plp;
 struct paramlst_t *nplphdr, *nplp, *last_nplp;

 if (oplp == NULL) return(NULL);

 nplphdr = NULL;
 for (last_nplp = NULL, plp = oplp; plp != NULL; plp = plp->pmlnxt)
  {
   nplp = __alloc_pval();
   nplp->plxndp = __copy_expr(plp->plxndp);
   if (last_nplp == NULL) nplphdr = nplp; else last_nplp->pmlnxt = nplp;
   nplp->pmlnxt = NULL;
   last_nplp = nplp;
  }
 return(nplphdr);
}

/*
 * read instance ports - probably no module def. at this point
 * know 1st token of port expr. read and reads final )
 */
static int32 rd_iports(char *inam)
{
 for (__cphdr = NULL;;)
  {
   /* read the cell-pin reference and one token past */
   /* illegal forms caught during fix up */
   if (!rd_cpin_conn()) goto bad_trynxt;

   if (__toktyp == RPAR) break;
   if (__toktyp == COMMA) { __get_vtok(); continue; }
   __pv_ferr(1051,
    "instance/gate %s port connection list comma or ) expected - %s read ",
    inam, __prt_vtok());

bad_trynxt:
   if (!__vskipto3_any(RPAR, COMMA, SEMI)) return(FALSE); 
   if (__toktyp == COMMA) { __get_vtok(); continue; }
   /* if ) or ; done and synced to right place */
   if (__toktyp == SEMI) __unget_vtok();
   break;
  }
 return(TRUE);
}

/*
 * read an instance port connection
 * know 1st token read and reads punctuation after , or )
 * then adds to end of global cell pin list header __cphdr
 *
 * on error returns F, caller tries to resync on , and read next port
 * but on .[id](<some error> ..., this trys to resync to port ending ) 
 */
static int32 rd_cpin_conn(void)
{
 int32 namedport_form;
 struct cell_pin_t *cpp;

 if (__toktyp == DOT)
  {
   __get_vtok();
   if (__toktyp != ID)
    {
     __pv_ferr(1052,
      "instance/gate connection name of port expected - %s read",
      __prt_kywrd_vtok());
     return(FALSE);
    }
   strcpy(__portnam, __token);
   __get_vtok();
   if (__toktyp != LPAR)
    {
     __pv_ferr(1053,
      "explicit port name form left parenthesis expected - %s read",
      __prt_vtok());
     return(FALSE);
    }
   /* 1st token in expr. must be read */
   __get_vtok();
   /* instance connection .[port]() for unc. is legal */
   if (__toktyp == RPAR)
    {
     __last_xtk = 0;
     __set_opempty(0);
     /* need to leave right paren in token since checked for later */
    }
   else
    {
     if (!__col_parenexpr(-1)) 
      {
       /* if can resync to ) move to next tok - then caller resyncs again */
       if (__vskipto_modend(RPAR)) __get_vtok();
       return(FALSE);
      }
    }
   namedport_form = TRUE;
  }
 else
  {
   namedport_form = FALSE;
   /* (, - ,, and ,) all legal */
   if (__toktyp == COMMA || __toktyp == RPAR)
    {  __last_xtk = 0; __set_opempty(0); }
   else { if (!__col_connexpr(-1)) return(FALSE); }
  }
 /* this declares undeclared wire */
 /* build the tree, copy/allocate nodes, sets __root_ndp to its root */
 __bld_xtree(0);

 if (namedport_form)
  {
   if (__toktyp != RPAR)
    {
     __pv_ferr(1055,
      "instance explicit named port form right parenthesis expected - %s read",
      __prt_vtok());
     if (__vskipto_modend(RPAR)) { __get_vtok(); return(TRUE); } 
     return(FALSE);
    }
   __get_vtok();
  }
 /* this save the global portnam as a string if present */
 cpp = __alloc_cpin(namedport_form);
 cpp->cpxnd = __root_ndp;
 if (__cphdr == NULL) __cphdr = cpp; else __cpp_last->cpnxt = cpp;
 __cpp_last = cpp;

 return(TRUE);
}

/*
 * allocate a cell pin - fill mostly from global data
 */
extern struct cell_pin_t *__alloc_cpin(int32 has_name)
{
 struct cell_pin_t *cpp;

 cpp = alloc_memcpins();
 if (has_name) cpp->pnam = alloc_cpnam(__portnam);
 else cpp->pnam = NULL;
 cpp->cplin_cnt = __lin_cnt;
 cpp->cpfnam_ind = __cur_fnam_ind;
 cpp->cpxnd = NULL;
 cpp->cpnxt = NULL;

 return(cpp);
}

/*
 * allocate a string element a preallocated block for fast freeing
 */
static char *alloc_cpnam(char *s)
{
 char *cp;
 int32 slen, rem, real_size;
 struct cpnblk_t *cpnbp;

 slen = strlen(s) + 1;
 if ((rem = slen % 4) != 0) real_size = slen + 4 - rem;
 else real_size = slen;

 if ((__hdr_cpnblks->cpn_start_sp + real_size + 4) >= 
  __hdr_cpnblks->cpn_end_sp)
  {
   cpnbp = (struct cpnblk_t *) __my_malloc(sizeof(struct cpnblk_t));
   cpnbp->cpnblknxt = __hdr_cpnblks;
   __hdr_cpnblks = cpnbp;
   cpnbp->cpnblks = cpnbp->cpn_start_sp = __my_malloc(BIG_ALLOC_SIZE);
   cpnbp->cpn_end_sp = cpnbp->cpn_start_sp + BIG_ALLOC_SIZE - 16;
  }
 cp = __hdr_cpnblks->cpn_start_sp;
 __hdr_cpnblks->cpn_start_sp += real_size;
 strcpy(cp, s);
 return(cp);
}

/*
 * allocate a ncomp element from a preallocated block for fast freeing
 */
static struct cell_pin_t *alloc_memcpins(void)
{
 struct cppblk_t *cppbp;
 struct cell_pin_t *cpp;

 if (__cppblk_nxti == -1)
  {
   cppbp = (struct cppblk_t *) __my_malloc(sizeof(struct cppblk_t));
   cppbp->cppblks = (struct cell_pin_t *) __my_malloc(BIG_ALLOC_SIZE);
   cppbp->cppblknxt = __hdr_cppblks;
   __hdr_cppblks = cppbp;
   __cppblk_nxti = 0;
  }
 cpp = (struct cell_pin_t *) &(__hdr_cppblks->cppblks[__cppblk_nxti]);
 if (++__cppblk_nxti > ((BIG_ALLOC_SIZE/sizeof(struct cell_pin_t)) - 1))
  __cppblk_nxti = -1;
 return(cpp);
}

/*
 * add cell - better not be already defined at module top level
 * at this point both gates and module instances cells
 * cells always declared at top level
 */
static struct cell_t *add_cell(char *inam)
{
 struct cell_t *cp;
 struct sy_t *syp;
 char s1[RECLEN];

 __cp_num++;
 syp = __decl_sym(inam, __venviron[0]);
 if (__sym_is_new)
  {
treat_as_new:
   syp->sytyp = SYM_I;
   cp = __alloc_cell(syp);
   syp->el.ecp = cp;

   syp->sydecl = TRUE;
    /* this is place of declaration */
   syp->syfnam_ind = __cur_fnam_ind;
   syp->sylin_cnt = __lin_cnt;

   /* must link on end to preserve inst. order */
   if (__end_cp == NULL) __inst_mod->mcells = cp;
   else __end_cp->cnxt = cp;
   __end_cp = cp;
   return(cp);
  }
 /* since symbol may be used as down 1 xmr in some system tasks */
 /* if not declared just assume is instance - checked later */
 if (syp->sytyp != SYM_I) 
  {
   if (syp->sydecl) 
    {
     __pv_ferr(1056, "instance/gate name %s previously declared as %s at %s",
      syp->synam, __to_sytyp(s1, syp->sytyp),
      __bld_lineloc(__xs, syp->syfnam_ind, syp->sylin_cnt));
    }
   else goto treat_as_new;
  }
 else __pv_ferr(1057, "instance/gate name %s repeated - previous %s",
  syp->synam, __bld_lineloc(__xs, syp->syfnam_ind, syp->sylin_cnt));
 return(NULL);
}

/*
 * allocate the cell - at this point can be gate, udp or inst.
 */
extern struct cell_t *__alloc_cell(struct sy_t *syp)
{
 struct cell_t *cp;

 /* alloc the element */
 cp = alloc_memcell();
 /* initialize contents union by zeroing all bytes of entire cell */
 cp->csym = syp;
 /* need to fill module type later */
 cp->cmsym = NULL;
 cp->cnxt = NULL;
 cp->c_hasst = FALSE;
 cp->c_stval = ST_STRVAL; 
 cp->cp_explicit = FALSE;
 cp->c_named = FALSE;
 cp->c_iscell = FALSE;
 cp->cx1 = cp->cx2 = NULL;
 cp->c_nparms = NULL;
 cp->cattrs = NULL;
 cp->cpins = NULL;
 return(cp);
}

/*
 * allocate a ncomp element from a preallocated block for fast freeing
 */
static struct cell_t *alloc_memcell(void)
{
 struct cpblk_t *cpbp;
 struct cell_t *cp;

 if (__cpblk_nxti == -1)
  {
   cpbp = (struct cpblk_t *) __my_malloc(sizeof(struct cpblk_t));
   cpbp->cpblks = (struct cell_t *) __my_malloc(BIG_ALLOC_SIZE);
   cpbp->cpblknxt = __hdr_cpblks;
   __hdr_cpblks = cpbp;
   __cpblk_nxti = 0;
  }
 cp = (struct cell_t *) &(__hdr_cpblks->cpblks[__cpblk_nxti]);
 if (++__cpblk_nxti > ((BIG_ALLOC_SIZE/sizeof(struct cell_t)) - 1))
  __cpblk_nxti = -1;
 return(cp);
}
