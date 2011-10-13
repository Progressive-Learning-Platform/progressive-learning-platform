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
 * third source module reads tasks/functions, udps and specify section
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#if defined(__CYGWIN32__) || defined(__SVR4)
#include <sys/stat.h>
#endif

#if defined(__CYGWIN32__) || defined(__SVR4) || defined(__hpux)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif

#ifdef __DBMALLOC__
#include "../malloc.h"
#endif

/* REMOVEME - no longer supporting SunOS - maybe needed for hpux? */
#if defined(__sparc) && !defined(__SVR4)  
extern int32 tolower(int32);
#endif

#include "v.h"
#include "cvmacros.h"

/* local prototypes */
static struct udp_t *alloc_udp(struct sy_t *);
static int32 rd_udp_hdr(struct udp_t *);
static int32 rd_udp_decls(void);
static int32 rd_udp_init(struct udp_t *);
static int32 chkcnt_uports(struct udp_t *);
static int32 rd_udp_table(struct udp_t *);
static void str_tolower(char *, char *);
static int32 cvrt_udpedges(char *, char *);
static int32 to_udp_levsym(char);
static int32 chk_comb_udpline(char *, struct udp_t *, int32 *);
static int32 chk_sequdpline(char *, struct udp_t *, int32 *);
static char to_edgech(int32);
static int32 is_edgesym(char);
static char *to_codedge_line(char *, char *);
static void extra_chk_edgeudp(struct udp_t *);
static char *to_udp_prtnam(struct udp_t *, int32);
static void dmp_udp_lines(FILE *, struct udp_t *);
static struct spfy_t *alloc_spfy(void);
static int32 rd_specparamdecl(void);
static void assign_1specparam(struct net_t *, struct expr_t *, int32, int32);
static int32 rd_delay_pth(void);
static struct exprlst_t *rd_pthtermlst(void);
static int32 col_pthexpr(void);
static int32 rd_pathdelaylist(struct paramlst_t **);
static void init_spcpth(struct spcpth_t *);
static int32 rd_setup_or_hold_tchk(word32);
static int32 rd_tchk_part(word32, struct tchk_t *, struct expr_t **);
static int32 rd_setuphold_tchk(void);
static int32 rd_recrem_tchk(void);
static int32 rd_width_tchk(void);
static int32 rd_period_tchk(void);
static int32 rd_skew_recov_rem_tchk(word32);
static int32 rd_nochg_tchk(void);
static int32 rd_tchk_selector(int32 *, struct expr_t **, struct expr_t **);
static int32 rd_edges(int32 *);
static struct sy_t *rd_notifier(void);
static struct attr_t *chk_dup_attrs(struct attr_t *);
static void rd1_cfg_file(FILE *);
static void rd_cfg_library(FILE *);
static struct libel_t *rd_cfg_fspec_list(FILE *, int32);
static void rd_cfg_cfg(FILE *);
static int32 chk_libid(char *);
static int32 chk_escid(char *);
static void init_rule(struct cfgrule_t *);
static int32 extract_design_nam(char *, char *, char *);
static int32 bld_inst_xmr_comptab(char *);
static void grow_bind_comps(void);
static int32 extract_libcell_nam(char *, char *, char *);
static int32 rd_use_clause(FILE *, char *, char *, int32 *);
static int32 extract_use_nam(char *, char *, int32 *, char *);
static struct cfgnamlst_t *rd_liblist(FILE *);

static void init_cfglib(struct cfglib_t *);
static void init_cfg(struct cfg_t *);
static int32 cfg_skipto_semi(int32, FILE *);
static int32 cfg_skipto_comma_semi(int32, FILE *);
static int32 cfg_skipto_semi_endconfig(int32, FILE *);
static int32 cfg_skipto_eof(int32, FILE *);

static void expand_dir_pats(struct cfglib_t *, struct libel_t *, char *);
static void expand_hier_files(struct cfglib_t *, struct libel_t *, 
 struct xpndfile_t *);
static void match_dir_pats(struct libel_t *, struct xpndfile_t *, char *, 
 char *, int32, int32);
static void movedir_match_dir_pats(struct libel_t *, struct xpndfile_t *); 
static void find_hier(struct libel_t *, struct xpndfile_t *, char *, char *);
static int32 match_hier_name(struct xpndfile_t *, char *);
static int32 match_wildcard_str(char *, struct xpndfile_t *);
static void expand_libel(struct libel_t *, char *);
static int32 expand_single_hier(struct cfglib_t *, struct libel_t *, char *);
static int32 has_wildcard(char *);
static void prep_cfg_vflist(void);
static void dump_config_info(void);
static void dump_lib_expand(void);
static int32 bind_cfg_design(struct cfg_t *, int32);
static struct cfglib_t *find_cfglib(char *);
static void free_undef_list(void);
static void bind_cells_in1mod(struct cfg_t *, struct cfglib_t *,
 struct mod_t *);
static int32 try_match_rule(struct cfglib_t *, struct cell_t *,
 struct cfgrule_t *);
static void build_rule_error(struct cfg_t *, struct cfglib_t *, 
struct cfgrule_t *);
static int32 bind_liblist_rule(struct cfg_t *, struct cell_t *,
 struct cfgrule_t *);
static int32 bind_use_rule(struct cfg_t *, struct cfglib_t *, struct cell_t *,
 struct cfgrule_t *);
static struct cfg_t *fnd_cfg_by_name(char *);
static void bind_cells_inside(struct cfg_t *, struct cell_t *,
 struct mod_t *, struct cfglib_t *);
static struct mod_t *find_cell_in_cfglib(char *, struct cfglib_t *);
static int32 open_cfg_lbfil(char *);
static void rd_cfg_srcfil(struct libel_t *);
static int32 init_chk_cfg_sytab(struct libel_t *, char *);
static void free_unused_cfgmods(void);
static void partially_free_mod(struct mod_t *);
static void add_cfg_libsyms(struct cfglib_t *);
static void add_cfgsym(char *, struct tnode_t *);

/* extern prototypes (maybe defined in this module) */
extern char *__pv_stralloc(char *);
extern char *__my_malloc(int32);
extern char *__my_realloc(char *, int32 , int32);
extern void __my_free(char *, int32);
extern char *__prt_vtok(void);
extern char *__prt_kywrd_vtok(void);
extern char *__to_uvvnam(char *, word32);
extern char *__to_tcnam(char *, word32);
extern char *__to_sytyp(char *, word32);
extern struct sy_t *__get_sym(char *, struct symtab_t *);
extern struct sy_t *__decl_sym(char *, struct symtab_t *);
extern struct sy_t *__bld_loc_symbol(int32, struct symtab_t *, char *, char *);
extern struct exprlst_t *__alloc_xprlst(void);
extern struct tnode_t *__vtfind(char *, struct symtab_t *);
extern struct symtab_t *__alloc_symtab(int32);
extern struct expr_t *__alloc_newxnd(void);
extern struct mod_pin_t *__alloc_modpin(void);
extern struct paramlst_t *__alloc_pval(void);
extern struct expr_t *__bld_rng_numxpr(word32, word32, int32);
extern int32 __vskipto_modend(int32);
extern void __add_sym(char *, struct tnode_t *);
extern int32 __chk_redef_err(char *, struct sy_t *, char *, word32);
extern void __remove_undef_mod(struct sy_t *);
extern void __get_vtok(void);
extern void __unget_vtok(void);
extern void __dmp_udp(FILE *, struct udp_t *);
extern int32 __udp_vskipto_any(int32);
extern int32 __udp_vskipto2_any(int32, int32);
extern int32 __udp_vskipto3_any(int32, int32, int32);
extern int32 __wide_vval_is0(register word32 *, int32);
extern void __wrap_puts(char *, FILE *);
extern void __wrap_putc(int32, FILE *);
extern void __nl_wrap_puts(char *, FILE *);
extern int32 __fr_tcnam(char *);
extern int32 __spec_vskipto_any(int32);
extern int32 __spec_vskipto2_any(int32, int32);
extern int32 __spec_vskipto3_any(int32, int32, int32);
extern int32 __rd_opt_param_vec_rng(struct expr_t **, struct expr_t **, int32);
extern int32 __col_paramrhsexpr(void);
extern int32 __col_connexpr(int32);
extern int32 __col_comsemi(int32);
extern void __bld_xtree(int32);
extern int32 __src_rd_chk_paramexpr(struct expr_t *, int32);
extern void __set_numval(struct expr_t *, word32, word32, int32);
extern struct net_t *__add_param(char *, struct expr_t *, struct expr_t *, 
 int32);
extern int32 __col_parenexpr(int32);
extern int32 __bld_expnode(void);
extern void __set_xtab_errval(void);
extern int32 __col_delexpr(void);
extern int32 __vskipto3_modend(int32, int32, int32);
extern void __init_tchk(struct tchk_t *, word32);
extern void __set_0tab_errval(void);
extern void __free_xtree(struct expr_t *);
extern void __free2_xtree(struct expr_t *);
extern void __skipover_line(void);
extern int32 __my_getlin(register char *);
extern int32 __pop_vifstk(void);
extern int32 __open_sfil(void);
extern void __do_include(void);
extern void __do_foreign_lang(void);
extern void __exec_vpi_langlinecbs(char *, char *, int32);
extern int32 __notokenize_skiplines(char *);
extern char *__bld_lineloc(char *, word32, int32);
extern char *__match_cdir(char *, char *);
extern int32 __exec_rdinserted_src(char *);
extern void __push_vinfil(void);
extern void __rd_ver_mod(void);
extern int32 __expr_has_glb(struct expr_t *);
extern struct xstk_t *__eval2_xpr(struct expr_t *);
extern void __sizchgxs(struct xstk_t *, int32);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern void __cnv_stk_fromreal_toreg32(struct xstk_t *);
extern void __eval_param_rhs_tonum(struct expr_t *);
extern int32 __cmp_xpr(struct expr_t *, struct expr_t *);
extern FILE *__tilde_fopen(char *, char *);
extern int32 __get_cfgtok(FILE *);
extern int32 __vskipto_modend(int32);
extern void __grow_infils(int32);
extern int32 __rd_cfg(void);
extern char *__to_cfgtoknam(char *, int32);
extern void __my_fclose(FILE *);
extern void __expand_lib_wildcards(void);
extern void __process_cdir(void);
extern int32 __rd_moddef(struct symtab_t *, int32);
extern int32 __vskipto2_modend(int32, int32);
extern char *__cfg_lineloc(char *s, char *, int32);
extern char *__schop(char *, char *);
extern void __sym_addprims(void);

extern void __cv_msg(char *s, ...);
extern void __finform(int32, char *, ...);
extern void __pv_ferr(int32, char *, ...);
extern void __pv_fwarn(int32, char *, ...);
extern void __gfinform(int32, word32, int32, char *, ...);
extern void __gfwarn(int32, word32, int32, char *, ...);
extern void __gferr(int32, word32, int32, char *, ...);
extern void __ia_err(int32, char *, ...);
extern void __dbg_msg(char *, ...);
extern void __pv_terr(int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __fterr(int32, char *, ...);
extern void __misc_terr(char *, int32);
extern void __misc_fterr(char *, int32);
extern void __pv_err(int32, char *, ...);
extern void __pv_warn(int32, char *, ...);

/*
 * UDP PROCESSING ROUTINES
 */

/*
 * process a udp definition
 * added to end of list of udps with header __udphead
 * name of udp has already been read
 * notice there is a separate design wide list of udps
 *
 * primitive keyword read and reads endprimitive
 */
extern int32 __rd_udpdef(struct symtab_t *cfg_sytab)
{
 int32 retval, initlcnt, initsfnind;
 struct udp_t *udpp;
 struct tnode_t *tnp;
 struct sy_t *syp;
 char *cp;

 initlcnt = 0;
 initsfnind = 0;
 /* notice that Verilog keywords are reserved words */
 retval = TRUE;
 /* for now must be able to declare to continue syntax checking */
 if (__toktyp != ID)
  {
   __pv_ferr(1155, "udp name expected - %s read", __prt_kywrd_vtok());
skip_end:
   retval = __vskipto_modend(ENDPRIMITIVE);
   return(retval);
  }
 cp = __token;

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
 else
  {
   tnp = __vtfind(cp, __modsyms);
   if (__sym_is_new)
    {
     __add_sym(__token, tnp);
     (__modsyms->numsyms)++;
     syp = tnp->ndp;
    }
   else
    {
     syp = tnp->ndp;
     /* if previously guessed as module, will just change */
     if (!__chk_redef_err(__token, syp, "udp", SYM_UDP)) goto skip_end;
     /* chk fail means never in module undef list */
     __remove_undef_mod(syp);
    }
  } 
 syp->sytyp = SYM_UDP;
 udpp = alloc_udp(syp);
 syp->el.eudpp = udpp;
 syp->sydecl = TRUE;
 /* need place where udp declared */
 syp->syfnam_ind = __cur_fnam_ind;
 syp->sylin_cnt = __lin_cnt;

 /* must also allocate the new symbol table */
 /* udps have no internal structure, sym table discarded when done */
 udpp->usymtab = __alloc_symtab(FALSE);
 __cur_udp = udpp;
 /* link symbol table back to module symbol */
 udpp->usymtab->sypofsyt = syp;

 /* do not need to build type until entire module read */
 /* any return here means skipped to endprimitive or next file level */
 if (!rd_udp_hdr(udpp)) return(FALSE);
 if (!rd_udp_decls()) return(FALSE);
 if (__toktyp == INITial)
  {
   initlcnt = __lin_cnt;
   initsfnind = __cur_fnam_ind;
   if (!rd_udp_init(udpp)) return(FALSE);
   __get_vtok();
   if (__toktyp != TABLE)
    {
     __pv_ferr(1156, "udp table section missing - %s read", __prt_vtok());
     goto skip_end;
    }
  }
 /* sets type to U_LEVEL if not combinatorial - edge type detected later */
 if (!chkcnt_uports(udpp)) retval = FALSE;
 if ((int32) udpp->numstates >= __ualtrepipnum) udpp->u_wide = TRUE;
 else udpp->u_wide = FALSE;

 /* this reads the endprimitive */
 if (!rd_udp_table(udpp)) return(FALSE);
 if (udpp->utyp == U_COMB && udpp->ival != NO_VAL)
  {
   __gferr(1157, initsfnind, initlcnt,
    "combinatorial udp %s cannot have initial value", syp->synam);
   udpp->ival = NO_VAL;
  }

 __get_vtok();
 if (__toktyp != ENDPRIMITIVE)
  {
   __pv_ferr(1158,
    "udp endprimitive keyword expected - %s read", __prt_vtok());
   goto skip_end;
  }
 if (!retval) return(TRUE);

 /* catch common extra ; error here */
 __get_vtok();
 if (__toktyp == SEMI)
  __pv_ferr(1152, "semicolon following endprimitive illegal"); 
 else __unget_vtok();

 extra_chk_edgeudp(udpp);

 /* notice if error before here not added to list */ 
 if (__udp_last == NULL) __udphead = udpp; else __udp_last->udpnxt = udpp;
 __udp_last = udpp;
 if (__debug_flg) __dmp_udp(stdout, udpp);
 return(TRUE);
}

/*
 * allocate a udp
 */
static struct udp_t *alloc_udp(struct sy_t *syp)
{
 struct udp_t *udpp;

 udpp = (struct udp_t *) __my_malloc(sizeof(struct udp_t));
 udpp->usym = syp;
 udpp->usymtab = NULL;
 udpp->upins = NULL;
 udpp->utyp = U_COMB;
 udpp->numins = 0;
 udpp->numstates = 0;
 udpp->u_used = FALSE;
 udpp->u_wide = FALSE;
 /* initial value - assume none that becomes 1'bx for level */
 udpp->ival = NO_VAL;
 udpp->utlines = NULL;
 udpp->udpnxt = NULL;
 udpp->utab = NULL;
 udpp->uidnum = 0;
 return(udpp);
}

/*
 * read the udp header
 * only simple variable names allowed in this header
 * reads ending ;
 * handles skipping - 
 */
static int32 rd_udp_hdr(struct udp_t *udpp)
{
 struct mod_pin_t *upp, *last_upp;
 struct sy_t *syp;

 /* empty I/O list illegal */
 __get_vtok();
 if (__toktyp == SEMI)
  {
   __pv_ferr(1162, "required udp header list of ports missing");
   return(TRUE);
  }

 if (__toktyp != LPAR)
  {
   __pv_ferr(1164,
    "udp header list of ports initial left parenthesis expected - %s read",
    __prt_vtok());
   if (__udp_vskipto2_any(RPAR, SEMI)) 
    {
     if (__toktyp == RPAR) __get_vtok();
     /* if not semi, assume semi left out - if bad, next rout. will catch */
     if (__toktyp != SEMI) __unget_vtok();
     return(TRUE);
    }
ret_end:
   if (__syncto_class == SYNC_FLEVEL) return(FALSE);
   else return(TRUE);
  }
 __get_vtok();
 if (__toktyp == RPAR)
  {
   __pv_ferr(1165, "empty udp header list of ports () form illegal");
do_end:
   __get_vtok();
   if (__toktyp == SEMI) return(TRUE);
   __pv_ferr(980,
    "module header list of ports end semicolon expected - %s read",
    __prt_vtok());
   __unget_vtok();
   return(TRUE);
  }
 for (last_upp = NULL;;)
  {
   /* this declares the symbols in the header */
   if (__toktyp != ID)
    {
     __pv_ferr(1166, "udp port variable name expected - %s read",
      __prt_kywrd_vtok());
do_resync:
     if (__udp_vskipto3_any(COMMA, SEMI, RPAR)) 
      {
       /* port effectively not seen - error emitted already */
       if (__toktyp == COMMA) goto nxt_port;
       if (__toktyp == RPAR) goto do_end;
       return(TRUE);
      }
     goto ret_end;
    }
   syp = __decl_sym(__token, __cur_udp->usymtab);
   /* must fill in connection to port still */
   if (__sym_is_new) syp->sytyp = SYM_N;
   else
    {
     __pv_ferr(1167,
      "udp port %s repeated in header list of ports", syp->synam);
     goto nxt_port;
    }
   upp = __alloc_modpin();
   upp->mptyp = IO_UNKN;
   upp->mpsnam = syp->synam;
   upp->mpref = NULL;
   syp->el.empp = upp;

   if (last_upp == NULL) udpp->upins = upp; else last_upp->mpnxt = upp;
   last_upp = upp;

nxt_port:
   __get_vtok();
   if (__toktyp == RPAR)
    {
     __get_vtok();
     if (__toktyp == SEMI) break;
     __pv_ferr(1168,
      "udp header list of ports ending semicolon expected - %s read",
      __prt_vtok());
     goto do_end;
    }
   if (__toktyp != COMMA)
    {
     __pv_ferr(1169,
      "udp header comma or semicolon separator expected - %s read",
      __prt_vtok());
     goto do_resync;
    }
   __get_vtok();
  }
 return(TRUE);
}

/*
 * read the udp declarations
 * must read first port type and reads following initial or table
 * eliminates illegal vector ports by not parsing
 */
static int32 rd_udp_decls(void)
{
 struct mod_pin_t *mpp;
 struct sy_t *syp;
 int32 outdecl_seen, regdecl_seen;

 regdecl_seen = outdecl_seen = FALSE;
 for (;;)
  {
again:
   __get_vtok();
   if (__toktyp == INITial || __toktyp == TABLE) break;
   switch ((byte) __toktyp) {
    case INPUT:
     for (;;)
      {
       __get_vtok();
       if (__toktyp != ID)
        {
         __pv_ferr(1170, "udp input port name expected - %s read",
          __prt_kywrd_vtok());
sync_in:
         if (__udp_vskipto2_any(COMMA, SEMI)) 
          {
           /* port effectively not seen - error emitted already */
           if (__toktyp == COMMA) continue;
           goto again;
          }
         if (__syncto_class == SYNC_FLEVEL) return(FALSE);
         else goto again;
        }
       if ((syp = __get_sym(__token, __cur_udp->usymtab)) == NULL)
        {
not_in_port:
          __pv_ferr(1173,
           "udp input declaration of \"%s\" - non header input port", __token);
         goto nxt_port;
        }
       if (syp->sytyp != SYM_N)
        {
bad_sytab:
         /* udp symbol table inconsistent */
         __misc_fterr(__FILE__, __LINE__);
        }
       mpp = syp->el.empp;
       if (syp->sydecl || mpp->mptyp != IO_UNKN) goto not_in_port;
       mpp->mptyp = IO_IN;
       syp->sydecl = TRUE;

nxt_port:
       __get_vtok();
       if (__toktyp == SEMI) break;
       if (__toktyp == COMMA) continue;
       __pv_ferr(1174,
        "udp port declaration comma or semicolon separator expected - %s read",
        __prt_vtok());
       goto sync_in;
      }
     break;
    case OUTPUT:
     if (outdecl_seen)
      {
       __pv_ferr(1178, "only one udp output port declaration permitted");
       __get_vtok();
       goto end_out_port;
      }
     outdecl_seen = TRUE;

      __get_vtok();
     if (__toktyp != ID)
      {
       __pv_ferr(1179, "udp output port name expected - %s read",
        __prt_kywrd_vtok());
sync_out:
       if (__udp_vskipto_any(SEMI)) return(TRUE); 
       if (__syncto_class == SYNC_FLEVEL) return(FALSE);
       else goto again;
      }
     if ((syp = __get_sym(__token, __cur_udp->usymtab)) == NULL)
      {
not_out_port:
       __pv_ferr(1180,
        "udp output port declaration of \"%s\" that is not in header port list",
        __token);
       goto end_out_port;
      }
     if (syp->sytyp != SYM_N) goto bad_sytab;
     mpp = syp->el.empp;
     /* NON_IO means already declared as reg so nothing to do */
     if (mpp->mptyp != NON_IO)
      {
       if (syp->sydecl || mpp->mptyp != IO_UNKN) goto not_out_port;
       mpp->mptyp = IO_OUT;
       syp->sydecl = TRUE;
      }
end_out_port:
     __get_vtok();
     if (__toktyp != SEMI)
      {
       __pv_ferr(1181,
        "udp output declaration not followed by semicolon - %s read",
        __prt_vtok());
       goto sync_out;
      }
     break;
    case REG:
     if (regdecl_seen)
      {
       __pv_ferr(1182, "only one udp reg declaration permitted");
       __get_vtok();
       goto end_reg;
      }
     regdecl_seen = TRUE;
     __get_vtok();
     if (__toktyp != ID)
      {
       __pv_ferr(1183,
        "udp reg declaration output port name expected - %s read",
        __prt_kywrd_vtok());
sync_reg:
       if (__udp_vskipto_any(SEMI)) return(TRUE); 
       if (__syncto_class == SYNC_FLEVEL) return(FALSE);
       else goto again;
      }
     if ((syp = __get_sym(__token, __cur_udp->usymtab)) == NULL)
      {
not_reg_port:
       __pv_ferr(1184,
        "udp reg declaration of \"%s\" that is not in header port list",
        __token);
       goto end_reg;
      }
     if (syp->sytyp != SYM_N) goto bad_sytab;
     mpp = syp->el.empp;
     if (mpp->mptyp == IO_OUT) mpp->mptyp = NON_IO;
     else
      {
       /* if not output must be undeclared */
       if (syp->sydecl || mpp->mptyp != IO_UNKN) goto not_reg_port;
       mpp->mptyp = NON_IO;
      }
end_reg:
     __get_vtok();
     if (__toktyp != SEMI)
      {
       __pv_ferr(1187,
        "udp output reg declaration ending semicolon expected - %s read",
        __prt_vtok());
       goto sync_reg;
      }
     break;
    default:
     __pv_ferr(1188,
      "udp declaration I/O declaration keyword expected - %s read",
      __prt_vtok());
    return(FALSE);
   }
  }
 return(TRUE);
}

/*
 * read the one optional initial statement for the one udp output
 * know initial read - format is intial [output term] = [1 bit const]
 * complicated because no mechanism for conversion from 32 1 bit vals
 */
static int32 rd_udp_init(struct udp_t *udpp)
{
 int32 blen;

 __get_vtok();
 if (__toktyp != ID) goto bad_init;
 __get_vtok();
 if (__toktyp != EQ) goto bad_init;
 __get_vtok();
 if (__toktyp != NUMBER) goto bad_init;
 __get_vtok();
 if (__toktyp != SEMI) goto bad_init;
 if (__itoklen > WBITS)
  {
   blen = __itoklen - WBITS;
   if (!vval_is0_(&(__acwrk[1]), blen)) goto bad_val; 
   if (!vval_is0_(&(__bcwrk[1]), blen)) goto bad_val; 
  }
 /* this must be a 1 bit value but wider with all zero's ok */
 if (__acwrk[0] == 0L && __bcwrk[0] == 0L) udpp->ival = 0;
 else if (__acwrk[0] == 1L && __bcwrk[0] == 0L)
  udpp->ival = 1;
 else if (__acwrk[0] == 0L && __bcwrk[0] == 1L)
  {
   __pv_fwarn(576, "udp initial value 1'bz illegal - changed to 1'bx");
   udpp->ival = 3;
  }
 else if (__acwrk[0] == 1L && __bcwrk[0] == 1L) udpp->ival = 3;
 else
  {
bad_val:
   __pv_ferr(1191, "udp initial value must be one of: 0, 1, 1'bx - %s read",
    __prt_vtok());
   udpp->ival = NO_VAL;
  }
 return(TRUE);

bad_init:
 __pv_ferr(1192, "udp initial statement syntax error");
 if (__udp_vskipto_any(SEMI)) return(TRUE);
 if (__syncto_class == SYNC_FLEVEL) return(FALSE);
 return(TRUE);
}

/*
 * check and count number of ports and set to default sequential if needed
 * number of ports in number of inputs (+ 1 if level or edge)
 * udp type number udp inputs and type set here
 * error if header port not declared
 * return FALSE on error
 */
static int32 chkcnt_uports(struct udp_t *udpp)
{
 register struct mod_pin_t *mpp;
 int32 retval;
 int32 unumins, unumstates;

 mpp = udpp->upins;
 retval = TRUE;
 if (mpp->mptyp == IO_IN)
  {
   __gferr(1193, udpp->usym->syfnam_ind, udpp->usym->sylin_cnt,
    "first udp header port %s not the output", mpp->mpsnam);
   retval = FALSE;
  }
 unumins = unumstates = 0;
 if (mpp->mptyp == NON_IO) { udpp->utyp = U_LEVEL; unumstates++; }

 mpp = mpp->mpnxt;
 for (; mpp != NULL; mpp = mpp->mpnxt)
  {
   if (mpp->mptyp != IO_IN)
    {
     __gferr(1194, udpp->usym->syfnam_ind, udpp->usym->sylin_cnt,
      "after first udp port %s must be an input", mpp->mpsnam);
     retval = FALSE;
    }
   unumins++;
  }
 udpp->numins = unumins;
 udpp->numstates = unumins + unumstates;
 if (udpp->numins > MAXUPRTS)
  {
   __gferr(1195, udpp->usym->syfnam_ind, udpp->usym->sylin_cnt,
    "udp definition has %d ports - maximum allowed is %d",
    udpp->numins, MAXUPRTS);
   retval = FALSE;
  }
 return(retval);
}

/*
 * read the udp table
 * know table keyword read and reads endtable
 * when done udp lines are array of chars of length numins + 1 (for out)
 * numins includes state for non combinatorial
 *
 * if edge, 1 allowed edge, char in line is 2nd plus uledinum index of edge
 * and ultabsel is 1st (maybe wildcard) - need to convert back to r/f abbrev.
 */
static int32 rd_udp_table(struct udp_t *udpp)
{
 int32 ulcnt, has_wcard, ulfnam_ind;
 struct utline_t *utlp, *last_utlp;
 char uline[RECLEN], coduline[RECLEN], s1[RECLEN];

 __letendnum_state = TRUE;
 /* initialized for malformed ; only line - error caught later */
 ulfnam_ind = __cur_fnam_ind;
 ulcnt = __lin_cnt;

 for (last_utlp = NULL;;)
  {
   __get_vtok();
   if (__toktyp == ENDTABLE) break;
   strcpy(uline, "");
   for (;;)
    {
     if (__toktyp == SEMI) goto end_line;

     /* need beginning of udp entry line no. */
     ulfnam_ind = __cur_fnam_ind;
     ulcnt = __lin_cnt;
     switch ((byte) __toktyp) {
      case LPAR: strcat(uline, "("); break;
      case RPAR: strcat(uline, ")"); break;
      case QUEST: strcat(uline, "?"); break;
      case MINUS: strcat(uline, "-"); break;
      case TIMES: strcat(uline, "*"); break;
      case COLON: strcat(uline, ":"); break;
      /* this requires that even non sized numbers stored in token */
      /* works because ' not legal in udp table line */
      case ID: strcat(uline, __token); break;
      /* SJM 03/20/00 - must assemble from num token for numbers */
      case NUMBER: strcat(uline, __numtoken); break;
      default:
       __pv_ferr(1198, "invalid udp table line symbol %s", __prt_vtok());
       if (__udp_vskipto_any(SEMI)) continue;
       if (__toktyp == ENDTABLE) goto done;
       if (__syncto_class == SYNC_FLEVEL) goto bad_end;
       __vskipto_modend(ENDPRIMITIVE);
bad_end:
       __letendnum_state = FALSE;
       return(FALSE);
     }
     __get_vtok();
    }
end_line:
   /* at this point line collected and contains only 1 char values and punct */
   /* utyp only U_COMB if does not have reg declaration */
   if (udpp->utyp == U_COMB)
    {
     __cur_utabsel = NO_VAL;
     __cur_ueipnum = NO_VAL;
     str_tolower(coduline, uline);
     if (!chk_comb_udpline(coduline, udpp, &has_wcard)) goto bad_end;
    }
   else
    {
     str_tolower(s1, uline);
     /* edge temporarily converted to 0x80 form */
     if (!cvrt_udpedges(coduline, s1)) return(FALSE);
     /* edge converted to 1st char __cur_utabsel plus 2nd char here in uline */
     __cur_utabsel = NO_VAL;
     if (!chk_sequdpline(coduline, udpp, &has_wcard)) continue;
    }
   utlp = (struct utline_t *) __my_malloc(sizeof(struct utline_t));
   utlp->tline = __pv_stralloc(coduline);
   utlp->ullen = (word32) strlen(coduline);
   utlp->ulhas_wcard = (has_wcard) ? TRUE : FALSE;
   utlp->uledinum = __cur_ueipnum;
   utlp->utabsel = __cur_utabsel;
   utlp->utlfnam_ind = ulfnam_ind;
   utlp->utlin_cnt = ulcnt;
   utlp->utlnxt = NULL;
   if (last_utlp == NULL) udpp->utlines = utlp; else last_utlp->utlnxt = utlp;
   last_utlp = utlp;
  }
done:
 __letendnum_state = FALSE;
 return(TRUE);
}

static void str_tolower(char *to, char *from)
{
 while (*from)
  {
   if (isupper(*from)) *to++ = (char) tolower(*from); else *to++ = *from;
   from++;
  }
 *to = '\0';
}

/*
 * convert (..) form edges to one coded char - real edge processing
 * in check seqential udp line routine
 * edge has high bit on and then bits 5-3 is e1 and 2-0 is e2
 *
 * know all legal upper case edges converted to lower case before called
 * first step in udp table line checking - edge abbreviation not seen here
 */
static int32 cvrt_udpedges(char *culine, char *uline)
{
 register char *culp, *ulp;
 char *chp, ech1, ech2;
 int32 ie1, ie2, no_err;
 char s1[RECLEN];

 no_err = FALSE;
 for (no_err = TRUE, ulp = uline, culp = culine;;)
  {
   switch (*ulp) {
    case '\0': *culp = '\0'; goto done;
    case '(':
     ech1 = *(++ulp);
     if ((ie1 = to_udp_levsym(ech1)) == -1)
      {
       __pv_ferr(1202,
        "udp table line edge first symbol '%c' not a level symbol", ech1);
edge_err:
       no_err = FALSE;
       /* making error into (? ?) edge */
       *culp++ = (char) (0x80 + (UV_Q << 3) + UV_Q);
       if ((chp = strchr(ulp, ')')) == NULL) { *culp = '\0'; return(no_err); }
       ulp = ++chp;
       continue;
      }
     ech2 = *(++ulp);
     if ((ie2 = to_udp_levsym(ech2)) == -1)
      {
       __pv_ferr(1203,
        "udp table line edge second symbol '%c' not a level symbol", ech2);
       goto edge_err;
      }
     if (*(++ulp) != ')')
      {
       __pv_ferr(1204,
        "udp table line edge symbol closing ')' expected - %c read", *ulp);
       goto edge_err;
      }
     /* edge has high bit on and bits 5-3 is e1 and 2-0 is e2 */
     *culp++ = (char) (0x80 | (ie1 << 3) | ie2);
     ulp++;
     break;
    default:
     *culp++ = *ulp++;
    }
  }
done:
 if (__debug_flg)
  __dbg_msg("&&& converted from %s to %s\n", uline, to_codedge_line(s1,
   culine));
 return(no_err);
}

/*
 * return value of level symbol (-1 if not level symbol)
 */
static int32 to_udp_levsym(char ch)
{
 switch (ch) {
  case '0': return(UV_0);
  case '1': return(UV_1);
  case 'x': return(UV_X);
  case '?': return(UV_Q);
  case 'b': return(UV_B);
 }
 return(-1);
}

/*
 * check coded combinatorial udp uline
 */
static int32 chk_comb_udpline(char *uline, struct udp_t *udpp,
 int32 *has_wcard)
{
 register char *chp;
 int32 ins;
 char ch;

 *has_wcard = FALSE;
 /* separate off ending :[output] */
 if ((chp = strrchr(uline, ':')) == NULL)
  {
   __pv_ferr(1205,
    "combinatorial udp line expected ':' output separator missing");
   return(FALSE);
  }
 *chp++ = '\0';
 ch = *chp++;
 if (ch == '-')
  {
   __pv_ferr(1206,
    "combinatorial udp line '-' output symbol illegal - has no state");
   return(FALSE);
  }
 if (ch != '0' && ch != '1' && ch != 'x')
  {
   __pv_ferr(1207,
    "combinatorial udp line output symbol (%x) '%c' illegal", ch, ch);
   return(FALSE);
  }
 if (*chp != '\0')
  {
   __pv_ferr(1208,
    "combinatorial udp line has second output symbol '%c' - only one allowed",
    *chp);
   return(FALSE);
  }
 /* check inputs - up to rightmost : */
 for (chp = uline, ins = 0; *chp != '\0'; chp++, ins++)
  {
   switch (*chp) {
    case '(': case 'r': case 'f': case 'p': case 'n': case '*':
     __pv_ferr(1209, "edge symbol %c illegal in combinatorial udp line", *chp); 
     return(FALSE);
   }
   if (to_udp_levsym(*chp) == -1)
    {
     __pv_ferr(1213,
      "combinatorial udp line input symbol '%c' (position %d) not a level symbol",
      *chp, ins + 1);
     return(FALSE);
    }
   if (ins >= 254)
    {
     __pv_ferr(1214,
      "udp has so many inputs (%d) - it cannot be checked", 254); 
     return(FALSE);
    }
   if (*chp == 'b' || *chp == '?') *has_wcard = TRUE;
  }
 if (ins != udpp->numins)
  {
   __pv_ferr(1215,
    "combinatorial udp line wrong number of input symbols (%d) - should be %d",
    ins, udpp->numins);
   return(FALSE);
  }
 /* finally add output as last symbol */
 *chp++ = ch;
 *chp = '\0';
 return(TRUE);
}

/*
 * check coded sequential upd uline
 * know all (..) edge convert to 1 char by here (0x80 on)
 * if no edge __cur_ueipnum has value NO_VAL
 * old 0x80 bit on form edge converted to 1st as __cur_utabsel, 2nd as char 
 * unless edge wildcard (i.e. r,f,n, etc.) in which case has edge wildcard
 */
static int32 chk_sequdpline(char *uline, struct udp_t *udpp,
 int32 *has_wcard)
{
 register char *chp;
 char ch1, ch2;
 int32 ins, t1;

 *has_wcard = FALSE;
 /* separate off : before previous state :[prev. state]:[output] */
 if ((chp = strchr(uline, ':')) == NULL)
  {
   __pv_ferr(1216,
    "sequential udp line expected colon before old state symbol missing");
   return(FALSE);
  }
 /* end uline */
 *chp = '\0';
 chp++;
 /* ch1 is state symbol and -1 means not 1 of legasl 0,1,x,?,b */
 ch1 = *chp++;
 if (to_udp_levsym(ch1) == -1)
  {
   __pv_ferr(1218,
    "sequential udp line state level symbol '%c' illegal", ch1);
   return(FALSE);
  }
 /* state can be wildcard but not edge */
 if (ch1 == 'b' || ch1 == '?') *has_wcard = TRUE;
 if (*chp++ != ':')
  {
   __pv_ferr(1219,
    "sequential udp line expected colon before output symbol missing");
   return(FALSE);
  }
 /* ch2 is output symbol */
 ch2 = *chp++;
 if (ch2 != '0' && ch2 != '1' && ch2 != 'x' && ch2 != '-')
  {
   __pv_ferr(1221, "sequential udp line output symbol '%c' illegal", ch2);
   return(FALSE);
  }
 if (*chp != '\0')
  {
   __pv_ferr(1222,
    "sequential udp line has extra output symbol '%c' - only one allowed",
    *chp);
   return(FALSE);
  }
 /* previous state and output done, finally check inputs */
 __cur_utabsel = NO_VAL;
 __cur_ueipnum = NO_VAL;
 for (chp = uline, ins = 0; *chp != '\0'; chp++, ins++)
  {
   /* know if edge, already checked - wild card only for level sensitive */
   if (is_edgesym(*chp))
    {
     if (__cur_ueipnum != NO_VAL)
      {
       __pv_ferr(1223,
       "sequential udp line has more than one edge symbol (second input %d)",
        ins + 1);
       return(FALSE);
      }
     __cur_ueipnum = ins;
     if ((*chp & 0x80) != 0)
      {
       /* know if (..) edge then both letters are edge symbols */
       /* edge has high bit on and then bits 5-3 is ie1 and 2-0 is ie2 */
       t1 = *chp & 0x7f;
       *chp = to_edgech(t1 & 0x7);
       __cur_utabsel = to_edgech((t1 >> 3) & 0x7);
       if ((__cur_utabsel == '0' && *chp == '0')
        || (__cur_utabsel == '1' && *chp == '1')
        || (__cur_utabsel == 'x' && *chp == 'x'))
        {
         __pv_ferr(1224,
          "sequential udp line edge (%c%c) (input %d) illegal - no transition",
          __cur_utabsel, *chp, __cur_ueipnum + 1);
         return(FALSE);
        }
      }
     else if (*chp == 'r') { __cur_utabsel = '0'; *chp = '1'; }
     else if (*chp == 'f') { __cur_utabsel = '1'; *chp = '0'; }
     /* if special edge abbrev. but not r or f make both edges have abbrev. */
     else __cur_utabsel = *chp;
     continue;
    }
   if (to_udp_levsym(*chp) == -1)
    {
     __pv_ferr(1225,
      "sequential udp line symbol '%c' (input %d) not edge or level", *chp,
      ins + 1);
     return(FALSE);
    }
   if (*chp == 'b' || *chp == '?') *has_wcard = TRUE;
  }
 if (ins != udpp->numstates - 1 || ins != udpp->numins)
  {
   __pv_ferr(1226,
    "sequential udp line wrong number of input symbols (%d) - should be %d",
    ins, udpp->numins - 1);
   return(FALSE);
  }
 /* finally add previous state input and next state output as last 2 */
 *chp++ = ch1;
 *chp++ = ch2;
 *chp = '\0';
 if (__cur_ueipnum != NO_VAL) udpp->utyp = U_EDGE;
 return(TRUE);
}

/*
 * convert edge 4 bit encoding to normal level edge symbol
 */
static char to_edgech(int32 encodee)
{
 switch ((byte) encodee) {
  case UV_0: return('0');
  case UV_1: return('1');
  case UV_X: break;
  case UV_Q: return('?');
  case UV_B: return('b');
  default: __case_terr(__FILE__, __LINE__);
 }
 return('x');
}

/*
 * return T if symbol is an edge symbol (code 0x80 or edge letter)
 */
static int32 is_edgesym(char ch)
{
 if ((ch & 0x80) != 0) return(TRUE);
 switch (ch) {
  case 'r': case 'f': case 'p': case 'n': case '*': return(TRUE);
 }
 return(FALSE);
}

/*
 * convert a coded edge line to a string 
 * in form during input before converted to line separate edge char and 
 * edge destination in line char position 
 */
static char *to_codedge_line(char *s, char *culine)
{
 register char *cpi, *cpo;
 int32 uch;

 for (cpi = culine, cpo = s; *cpi != '\0'; cpi++)
  {
   if ((*cpi & 0x80) != 0)
    {
     *cpo++ = '(';
     /* notice 5-3 are e1 and 2-0 are e2 */
     uch = (int32) *cpi;
     *cpo++ = to_edgech((uch >> 3) & 0x7);
     *cpo++ = to_edgech(uch & 0x7);
     *cpo++ = ')'; 
    }
   else *cpo++ = *cpi;
  }
 *cpo = '\0';
 return(s);
}

/*
 * perform some consistency checks on edge udps
 * - input column probably needs edge in some row
 * - output column probably needs - somewhere
 */
static void extra_chk_edgeudp(struct udp_t *udpp)
{
 register int32 i;
 int32 out_hasbar;
 struct utline_t *utlp;

 for (i = 0; i < (int32) udpp->numins; i++)
  {
   for (utlp = udpp->utlines; utlp != NULL; utlp = utlp->utlnxt)
    {
     if (utlp->uledinum == i) goto next;
    }
   __gfinform(421, udpp->usym->syfnam_ind, udpp->usym->sylin_cnt,
    "sequential udp \"%s\" column for input %s lacks any edge(s)",
     udpp->usym->synam, to_udp_prtnam(udpp, i + 1));
next:;
  }
 /* inputs are 0 to num ins - 1 then previous state, then next state */
 i = udpp->numins + 1;
 out_hasbar = FALSE;
 for (utlp = udpp->utlines; utlp != NULL; utlp = utlp->utlnxt)
  {
   if (utlp->tline[i] == '-') out_hasbar = TRUE;
  }
 if (!out_hasbar)
  {
   __gfinform(422, udpp->usym->syfnam_ind, udpp->usym->sylin_cnt,
    "sequential udp %s output column lacks any no change (-) symbols",
    udpp->usym->synam);
  }
}

/*
 * find input position %d (first is output)
 * know position number legal and starts at 1
 */
static char *to_udp_prtnam(struct udp_t *udpp, int32 inum)
{
 register struct mod_pin_t *mpp;
 int32 nins;

 nins = 1;
 mpp = udpp->upins->mpnxt;
 for (; mpp != NULL; mpp = mpp->mpnxt, nins++)
  {
   if (nins == inum) goto done;
  }
 __misc_terr(__FILE__, __LINE__);
done:
 return(mpp->mpsnam);
}

/*
 * UDP DUMP ROUTINES - FOR DEBUGGING
 */

/*
 * dump a udp for debugging
 * f cannot be nil to put in string
 */
extern void __dmp_udp(FILE *f, struct udp_t *udpp)
{
 register struct mod_pin_t *mpp;
 int32 first_time;
 char s1[RECLEN], s2[RECLEN];

 if (f == NULL) __arg_terr(__FILE__, __LINE__);
 __cv_msg("\n");
 __cur_sofs = 0;
 __outlinpos = 0;
 __pv_stlevel = 0;

 __wrap_puts("primitive ", f);
 __wrap_puts(udpp->usym->synam, f);
 first_time = TRUE;
 /* notice udp module pin lists continue to use next pointer */
 for (mpp = udpp->upins; mpp != NULL; mpp = mpp->mpnxt)
  {
   if (first_time) { __wrap_putc('(', f); first_time = FALSE; }
   else __wrap_puts(", ", f);
   /* know udp pins must be named */
   __wrap_puts(mpp->mpsnam, f);
  }
 __nl_wrap_puts(");", f);

 /* notice here mpsnam must exist or earlier syntax error */
 __pv_stlevel = 1;
 mpp = udpp->upins;
 __wrap_puts("output ", f);
 __wrap_puts(mpp->mpsnam, f);
 __wrap_putc(';', f);
 if (udpp->utyp != U_COMB)
  {
   __wrap_puts("  reg ", f);
   __wrap_puts(mpp->mpsnam, f);
   __wrap_putc(';', f); 
  }
 __nl_wrap_puts("", f);
 
 __wrap_puts("input ", f);
 first_time = TRUE;
 for (mpp = mpp->mpnxt; mpp != NULL; mpp = mpp->mpnxt)
  {
   if (first_time) first_time = FALSE; else __wrap_puts(", ", f);
   __wrap_puts(mpp->mpsnam, f);
  }
 __nl_wrap_puts(";", f);

 if (udpp->ival != NO_VAL)
  {
   __wrap_puts("initial ", f);
   __wrap_puts(udpp->upins->mpsnam, f);
   sprintf(s1, " = 1'b%s", __to_uvvnam(s2, (word32) udpp->ival));
   __wrap_puts(s1, f);
   __nl_wrap_puts(";", f);
  }
 __nl_wrap_puts("", f);
 __nl_wrap_puts("table", f);
 dmp_udp_lines(f, udpp);
 __nl_wrap_puts("endtable", f);
 __pv_stlevel--;
 __nl_wrap_puts("endprimitive", f);
}

/*
 * dump udp lines
 * need to also dump initial value
 */
static void dmp_udp_lines(FILE *f, struct udp_t *udpp)
{
 register int32 i;
 register struct utline_t *utlp;
 int32 numins, sav_stlevel;
 char *chp, s1[RECLEN];

 sav_stlevel = __pv_stlevel;
 __pv_stlevel = 4;
 __outlinpos = 0;
 numins = udpp->numins;
 for (utlp = udpp->utlines; utlp != NULL; utlp = utlp->utlnxt)
  {
   for (chp = utlp->tline, i = 0; i < numins; i++, chp++)
    {    
     /* the one possible edge */
     if (utlp->uledinum == i)
      {
       /* special wild card types edges are kept as original char */
       /* 01 and 10 are converted from rise-fall - convert back */
       if (utlp->utabsel == '0' && *chp == '1') __wrap_puts("    r", f);
       else if (utlp->utabsel == '1' && *chp == '0') __wrap_puts("    f", f);
       else if (utlp->utabsel == '*') __wrap_puts("    *", f);
       else if (utlp->utabsel == 'p') __wrap_puts("    p", f);
       else if (utlp->utabsel == 'n') __wrap_puts("    n", f);
       else
        {
         sprintf(s1, " (%c%c)", (char) utlp->utabsel, *chp);
         __wrap_puts(s1, f);
        }
      }
     /* various wild cards and states left as input char */
     else { sprintf(s1, "%5c", *chp); __wrap_puts(s1, f); }
    }
   if (udpp->utyp != U_COMB)
    { sprintf(s1, " : %c ", *chp); __wrap_puts(s1, f); chp++; }
   sprintf(s1, " : %c ;", *chp);
   __nl_wrap_puts(s1, f);
  }
 __pv_stlevel = sav_stlevel;
}

/*
 * READ SPECIFY SECTION ROUTINES
 */

/*
 * read and build d.s for specify section items
 * expects specify to have been read and reads endspecify
 * current approach concatenates all specify sections in one mod.
 */
extern int32 __rd_spfy(struct mod_t *mdp)
{
 int32 tmpi, sav_decl_obj;
 word32 tchktyp;
 char s1[RECLEN];
 
 __path_num = __tchk_num = 1;
 sav_decl_obj = __cur_declobj;
 __cur_declobj = SPECIFY;
 /* all specify sections concatenated together */
 if (mdp->mspfy == NULL) mdp->mspfy = alloc_spfy();
 __cur_spfy = mdp->mspfy;
 /* at this point only module symbol tabl on scope stack since specify */
 /* is module item */
 if (__top_sti != 0) __misc_terr(__FILE__, __LINE__);
 /* place special symbol table for specparams on scope stack */
 __venviron[++__top_sti] = __cur_spfy->spfsyms;

 for (;;)
  {
   __get_vtok();
   switch ((byte) __toktyp)
    {
     case SPECPARAM: 
      if (!rd_specparamdecl())
       {
        /* notice unless T (found ;) will not sync to normal ( path */
specitem_resync:
        switch ((byte) __syncto_class) {
         case SYNC_FLEVEL: case SYNC_MODLEVEL: case SYNC_STMT:
          __top_sti--;
          return(FALSE);
         case SYNC_SPECITEM:
          break;
         /* if sync. to statement assume initial left out */
         default: __case_terr(__FILE__, __LINE__);
        }
       }
      continue;
     case IF: case LPAR: case IFNONE:
      if (!rd_delay_pth()) goto specitem_resync;
      break;
     case ENDSPECIFY: goto done;
     case ID:
      /* this must be timing check */
      if (*__token == '$')
       {
        if ((tmpi = __fr_tcnam(__token)) == -1)
         {
          __pv_ferr(1228,
           "system task %s illegal in specify section", __token);
          goto err_skip;
         }
        tchktyp = tmpi;
        __get_vtok();
        if (__toktyp != LPAR)
         {
          __pv_ferr(1231,
          "timing check %s argument list left parenthesis expected - %s read",
          __to_tcnam(s1, tchktyp) , __prt_vtok());
          goto err_skip;
         } 
        /* specify system timing check tasks */
        /* the routines fill cur tchk */
        switch ((byte) tchktyp) {
         case TCHK_SETUP: case TCHK_HOLD:
          if (!rd_setup_or_hold_tchk(tchktyp)) goto specitem_resync;
          break;
         case TCHK_SETUPHOLD:
          if (!rd_setuphold_tchk()) goto specitem_resync;
          break;
         case TCHK_WIDTH:
          if (!rd_width_tchk()) goto specitem_resync;
          break;
         case TCHK_PERIOD:
          if (!rd_period_tchk()) goto specitem_resync;
          break;
         case TCHK_SKEW: case TCHK_RECOVERY: case TCHK_REMOVAL:
          if (!rd_skew_recov_rem_tchk(tchktyp)) goto specitem_resync;
          break;
         case TCHK_RECREM:
          if (!rd_recrem_tchk()) goto specitem_resync;
          break;
         case TCHK_NOCHANGE:
          if (!rd_nochg_tchk()) goto specitem_resync;
          break;
         case TCHK_FULLSKEW:
         case TCHK_TIMESKEW:
          /* SJM 11/21/03 - this and other new 2001 tchks not supported */
          /* for now just skip and later add support for this and others */
          __pv_fwarn(3124,"%s timing check not yet supported - ignored",
           __to_tcnam(__xs, tchktyp)); 
          goto err_skip;
         default: __case_terr(__FILE__, __LINE__);
        }
        /* add to timing check list */
        if (__end_tchks == NULL) __cur_spfy->tchks = __cur_tchk;
        else __end_tchks->tchknxt = __cur_tchk;
        __end_tchks = __cur_tchk;
        break;
       }
      /* fall through to error since ID not specify item */
      /*FALLTHRU*/
     default:
      __pv_ferr(1229, "specify section item expected - %s read",
       __prt_vtok());
err_skip:
     if (!__spec_vskipto_any(SEMI)) goto specitem_resync;
     /* just fall through if seme to next */
    }
  }
done:
 __top_sti = 0;
 __cur_declobj = sav_decl_obj;
 /* notice freezing at module end for specparam symbol table too */
 return(TRUE);
}

/*
 * allocate a specify block - called when first specify block seen
 */
static struct spfy_t *alloc_spfy(void)
{
 struct spfy_t *spcp;

 spcp = (struct spfy_t *) __my_malloc(sizeof(struct spfy_t));
 /* notice this symbol table links to mod through this special specify */
 /* block but otherwise no symbol table links */
 spcp->spfsyms = __alloc_symtab(TRUE);
 spcp->spcpths = NULL; 
 spcp->tchks = NULL;
 spcp->msprms = NULL; 
 spcp->sprmnum = 0;
 __end_spcpths = NULL;
 __end_tchks = NULL;
 __end_msprms = NULL;
 return(spcp);
}

/*
 * read the specparam declaration statement 
 * form: specparam [name] = [constant?], ...
 * no  # and () around delay is optional in parameter decl.
 * reads parameter name through ending ;
 *
 * here width actual bit width - later converted to int32 or real
 * and then usually to 64 bit delay
 */
static int32 rd_specparamdecl(void)
{
 int32 good, sav_slin_cnt, sav_sfnam_ind, prng_decl, pwid, rwid, r1, r2;
 struct net_t *np;
 struct expr_t *dx1, *dx2, *x1, *x2;
 char paramnam[IDLEN];

 pwid = 0;
 dx1 = dx2 = x1 = x2 = NULL;
 prng_decl = FALSE;
 __get_vtok();
 if (__toktyp == LSB)
  {
   /* also check to make sure ranges are non x/z 32 bit values */
   if (!__rd_opt_param_vec_rng(&dx1, &dx2, FALSE)) return(FALSE);
   if (dx1 == NULL || dx2 == NULL) goto rd_param_list;

   r1 = (int32) __contab[dx1->ru.xvi];
   r2 = (int32) __contab[dx2->ru.xvi];
   pwid = (r1 >= r2) ? r1 - r2 + 1 : r2 - r1 + 1; 
   x1 = dx1; x2 = dx2;
   prng_decl = TRUE;
   /* know parameter name read */
  }

rd_param_list:
 for (;;)
  {
   if (__toktyp != ID)
    {
     __pv_ferr(1230, "specparam name expected - %s read", __prt_kywrd_vtok());
bad_end:
     /* part of delay expression may have been built */
     if (!__spec_vskipto2_any(COMMA, SEMI)) return(FALSE);
     if (__toktyp == COMMA) { __get_vtok(); continue; }
     return(TRUE);
    }
   strcpy(paramnam, __token);

   /* notice the initial value is required */
   __get_vtok();
   if (__toktyp != EQ)
    {
     __pv_ferr(1232,
      "specparam declaration equal expected - %s read", __prt_vtok());
     goto bad_end;
    }
   /* 06/22/00 - SJM - special path pulse form, for now ignore with warn */
   if (strncmp(paramnam, "PATHPULSE$", 10) == 0)
    {
     __pv_fwarn(3102,
      "%s special path pulse specparam ignored - use +show_canceled_e option instead",
      paramnam);
     /* 06/22/00 - SJM - FIXME - need to really parse this */ 
     if (!__spec_vskipto_any(SEMI)) 
      {
       __pv_ferr(3408,
        "PATHPULSE$ specparam %s right hand side value illegal format",
        paramnam);
       return(FALSE);
      }
     goto nxt_sparam;
    }

   /* know symbol table env. in specify specparam rhs specparam local */
   __get_vtok();
   __sav_spsytp = __venviron[0];
   __venviron[0] = __venviron[1];
   __top_sti = 0;
   good = __col_paramrhsexpr();
   __venviron[0] = __sav_spsytp;
   __top_sti = 1; 
   /* on error, does not add spec param */
   if (!good) goto bad_end;
   __bld_xtree(0);

   /* checking rhs does no evaluation (not known yet) but set sizes */
   /* and makes sure contains only num and previously defined specparams */
   if (__expr_has_glb(__root_ndp)
    || !__src_rd_chk_paramexpr(__root_ndp, WBITS))
    {
     __pv_ferr(1233,
      "specparam %s right hand side %s illegal - defined specparams and constants only",
      paramnam, __msgexpr_tostr(__xs, __root_ndp));
     goto nxt_sparam;
    }

   /* SJM 06/17/99 - illegal to assign string literal to specparam */
   /* SJM 02/04/00 - must allow since needed for models where PLI used */
   /* to read parameter value - and should be this is just wide reg */
   /* --- REMOVED --- 
   if (__root_ndp->is_string)
    {
     __pv_fwarn(659,
      "specparam %s right hand side string not a delay value - converted to 0 delay",
      paramnam);
     -* need to still add value of x to prevent further errors *-
     __free2_xtree(__root_ndp);
     __root_ndp->szu.xclen = WBITS;
     __set_numval(__root_ndp, 0L, 0L, WBITS);  
     -* notice x1, x2 range expressions always WBITS-1 to 0 for specparams *-  
    }
   --- */

   /* if range declared that is used, else if non scalar expr, use that */
   if (prng_decl)
    {
     if (pwid == 1) x1 = x2 = NULL;
     else
      {
       /* for specparam - assume int/word32, convert to real if needed */
       x1 = __bld_rng_numxpr(pwid - 1L, 0L, WBITS);
       x2 = __bld_rng_numxpr(0L, 0L, WBITS);
      }
    }
   else
    {
     if (__root_ndp->is_real) rwid = REALBITS; 
     else rwid = __root_ndp->szu.xclen;

     if (rwid == 1) x1 = x2 = NULL;
     else
      {
       /* if expr value unsized, know will be 32 bit width already */  
       x1 = __bld_rng_numxpr(rwid - 1, 0, WBITS);
       x2 = __bld_rng_numxpr(0, 0, WBITS);
      }
    }

   /* check and links on modules parameter list */
   /* when rhs expr. evaluated, if real will change */
   /* LOOKATME - problem with all params in list sharing range xprs? */ 
   np = __add_param(paramnam, x1, x2, FALSE);
 
   /* using ncomp delay union to store original expresssion - set first */
   /* need this separate copy even after parameter value assigned */
   np->nu.ct->n_dels_u.d1x = __root_ndp;
   np->nu.ct->parm_srep = SR_PXPR;

   /* can assign specparam value immediately */
   /* SJM 06/17/99 - needs to run in run/fixup mode - statement loc set */
   sav_slin_cnt = __slin_cnt;
   sav_sfnam_ind = __sfnam_ind;
   __sfnam_ind = (int32) np->nsym->syfnam_ind;
   __slin_cnt = np->nsym->sylin_cnt;

   assign_1specparam(np, __root_ndp, prng_decl, pwid);

   __slin_cnt = sav_slin_cnt;
   __sfnam_ind = sav_sfnam_ind;

   /* specparams can never be strings or IS forms */ 
   __mod_specparams++;

nxt_sparam:
   if (__toktyp == SEMI) break;
   if (__toktyp != COMMA)
    {
     __pv_ferr(1236,
      "specparam ; or , separator expected - %s read", __prt_vtok());
     if (!__spec_vskipto2_any(COMMA, SEMI)) return(FALSE); 
     if (__toktyp == SEMI) break;
    }
   __get_vtok();
  }
 return(TRUE);
}

/*
 * assign values to specparams - like defparams but can never be IS form
 *
 * 02/04/00 - SJM change to move toward v2k LRM and match XL better
 * type determined from RHS - range allowed and used for width
 *
 * SJM 10/06/03 - signed keyword illegal and specparams never signed
 * unless real
 */
static void assign_1specparam(struct net_t *np, struct expr_t *ndp,
 int32 prng_decl, int32 pwid)
{
 int32 wlen;
 word32 *wp;
 struct xstk_t *xsp;

 /* need dummy itree place on itree stack for eval */
 xsp = __eval_xpr(ndp);

 /* case 1: range declaration - may need to convert value */
 if (prng_decl)
  {
   /* convert declared param type real rhs to int/reg */
   if (ndp->is_real)
    {
     __cnv_stk_fromreal_toreg32(xsp);
     np->nu.ct->pbase = BDEC;
     np->nu.ct->prngdecl = TRUE;
     np->nwid = xsp->xslen;
     np->ntyp = N_REG;
     np->n_signed = TRUE;
    }
   else
    {
     if (xsp->xslen != pwid) __sizchgxs(xsp, pwid);

     np->nu.ct->prngdecl = TRUE;
     np->nwid = xsp->xslen;
     np->ntyp = N_REG;
     if (np->nwid > 1) { np->n_isavec = TRUE; np->vec_scalared = TRUE; } 
     np->nu.ct->pbase = ndp->ibase;
    }

   wlen = wlen_(np->nwid);
   wp = (word32 *) __my_malloc(2*WRDBYTES*wlen); 
   memcpy(wp, xsp->ap, 2*WRDBYTES*wlen);
   __pop_xstk();
   np->nva.wp = wp;
   np->srep = SR_PNUM;
   return;
  }

 /* SJM 10/06/03 - since specparams never signed, interpret as word32 */
 /* even if rhs signed with sign bit on */

 /* case 2: type determined from constant expr */
 /* spec params either reg or real - by here if range decl ndp fixed */
 if (ndp->is_real)
  {
   np->nwid = REALBITS;
   np->ntyp = N_REAL;
   np->n_signed = TRUE;
   np->n_isavec = TRUE;
   np->nu.ct->pbase = BDBLE;
   wp = (word32 *) __my_malloc(2*WRDBYTES); 
   memcpy(wp, xsp->ap, 2*WRDBYTES);
  }
 else
  {
   np->nwid = xsp->xslen;
   np->ntyp = N_REG;
   if (np->nwid > 1) { np->n_isavec = TRUE; np->vec_scalared = TRUE; } 
   if (ndp->is_string) np->nu.ct->pstring = TRUE;

   np->nu.ct->pbase = ndp->ibase;
   wlen = wlen_(np->nwid);
   wp = (word32 *) __my_malloc(2*WRDBYTES*wlen); 
   memcpy(wp, xsp->ap, 2*WRDBYTES*wlen);
  }
 __pop_xstk();
 /* build wire value - this is assign to wire so wire flags unchged */
 np->nva.wp = wp;
 np->srep = SR_PNUM;
}

/*
 * DELAY PATH ROUTINES
 */

/*
 * read the ([path desc. list?] [=*]> [path desc. list?]) = [path val.]
 * know initial ( read and if conditional present in condx
 * notice no path if any error but may still return T
 *
 * here when collecting expressions both specparams and top module symbol
 * table accessible for wires but only local specparams for delay
 */
static int32 rd_delay_pth(void)
{
 int32 good;
 int32 pdtyp, pth_edge, pthpolar, datasrcpolar, is_ifnone;
 struct sy_t *pthsyp;
 struct exprlst_t *ilstx, *olstx;
 struct paramlst_t *pmphdr;
 struct expr_t *condx, *datsrcx, *lop, *last_dsx;
 struct spcpth_t *pthp;

 is_ifnone = FALSE;
 condx = datsrcx = NULL;
 /* needed since gcc sees as unset - do not think so */
 olstx = NULL;
 pdtyp = PTH_NONE;
 datasrcpolar = POLAR_NONE;
 pthpolar = POLAR_NONE;
 if (__toktyp == IFNONE)
  { 
   __get_vtok();
   if (__toktyp != LPAR)
    {
     __pv_ferr(1197,
      "sdpd ifnone token not followed by path beginning ( - %s read",
      __prt_vtok());
     /* here skipping to ) will skip path - no start tok - can not correct */
     if (!__spec_vskipto_any(SEMI)) return(FALSE);
     return(TRUE);
    }
   is_ifnone = TRUE;
   goto no_pthcond;
  }
 if (__toktyp == LPAR) goto no_pthcond;
 /* must be if token to get here, read optional condition expr. 1st */
 __get_vtok();
 if (__toktyp != LPAR)
  {
   __pv_ferr(1251,
    "sdpd conditional expression if token not followed by ( - %s read",
     __prt_vtok());
bad_sdp:
   if (!__spec_vskipto2_any(SEMI, RPAR)) return(FALSE);
   if (__toktyp == SEMI) return(TRUE);
   /* have ) make cond x NULL - this is not delay this is if actual expr */
   __set_numval(__exprtab[0], 1L, 1L, 1);
   __last_xtk = 0;
   goto bad_sdpx;
  }
 __get_vtok();
 if (!__col_parenexpr(-1)) goto bad_sdp;
bad_sdpx:
 __bld_xtree(0);
 condx = __root_ndp;
 if (__expr_has_glb(condx))
  {
   __pv_ferr(1022,
    "global hierarchical reference illegal in state dependent path condition %s",
    __msgexpr_tostr(__xs, condx));
  }
 __get_vtok();
 if (__toktyp != LPAR)
  {
   __pv_ferr(1252,
    "sdpd conditional expression not followed by path start ( - %s read",
    __prt_vtok());
   /* here skipping to ) will skip path */
   if (!__spec_vskipto_any(SEMI)) return(FALSE);
   return(TRUE);
  }

no_pthcond:
 /* for edge sensitive path can have edge before input */
 /* but only pos and neg edges */
 __get_vtok();
 if (__toktyp == NEGEDGE || __toktyp == POSEDGE) 
  {
   pth_edge = (__toktyp == NEGEDGE) ? E_NEGEDGE : E_POSEDGE;
   __get_vtok();
  }
 else pth_edge = NOEDGE;

 /* this requires read 1st token of path */ 
 if ((ilstx = rd_pthtermlst()) == NULL)
  {
bad_pth:
   if (!__spec_vskipto3_any(SEMI, RPAR, EQ)) return(FALSE);
   if (__toktyp == RPAR) goto get_eq; 
   if (__toktyp == EQ) goto got_eq;
   return(TRUE);
  }
 /* this just attempts to catch some common errors */
 if (__toktyp == RPAR || __toktyp == TCHKEVAND)
  {
   __pv_ferr(1253,
    "input path description connector operator or semicolon expected - %s read",
    __prt_vtok());
   goto bad_pth;
  }
 /* path polarity is option stored for pli but not used */
 if (__toktyp == PLUS || __toktyp == MINUS)
  {
   pthpolar = (__toktyp == PLUS) ? POLAR_PLUS : POLAR_MINUS;
   __get_vtok();
  }
 else pthpolar = POLAR_NONE;

 /* path type required */
 if (__toktyp == FPTHCON) pdtyp = PTH_FULL; 
 else if (__toktyp == PPTHCON) pdtyp = PTH_PAR;
 else
  {
   __pv_ferr(1258,
    "either full => or parallel *> path operator expected - %s read",
    __prt_vtok());
   goto bad_pth;
  }
 /* if ( here then form is ([dst. term list] [polarity?]:[data src. expr.]) */
 __get_vtok();
 if (__toktyp == LPAR)
  { 
   /* this requires read 1st token of path */ 
   __get_vtok();
   if ((olstx = rd_pthtermlst()) == NULL) goto bad_end;
   /* data source polarity determines delay selection */
   if (__toktyp == PLUS || __toktyp == MINUS)
    {
     datasrcpolar = (__toktyp == PLUS) ? POLAR_PLUS : POLAR_MINUS;
     __get_vtok();
    }
   else datasrcpolar = POLAR_NONE;
   if (__toktyp != COLON)  
    {
     __pv_ferr(1254,
      "data source path destination colon terminator expected - %s read",
      __prt_vtok());
     goto bad_pth;
    }
   __get_vtok();
   /* comma separated list allowed here - width must match dest. width */
   if (!__col_parenexpr(-1)) goto bad_pth;
   __bld_xtree(0);
   /* common edge path trigger as 1 expression not list case */
   if (__toktyp != COMMA)
    { datsrcx = __root_ndp; __get_vtok(); goto chk_endrpar; }

   /* this is complicated data source expression list case */
   lop = __alloc_newxnd();
   lop->optyp = FCCOM;
   lop->ru.x = NULL;
   lop->lu.x = __root_ndp;
   datsrcx = lop;
   for (last_dsx = lop;;) 
    {
     /* if good this reads trailing delimiter */
     if (!__col_connexpr(-1)) goto bad_end;
     __bld_xtree(0);
     lop = __alloc_newxnd();
     lop->optyp = FCCOM;
     lop->ru.x = NULL;
     lop->lu.x = __root_ndp;
     last_dsx->ru.x = lop;
     if (__toktyp == RPAR) { __get_vtok(); break; }
     if (__toktyp != COMMA) 
      {
       __pv_ferr(1255,
        "edge sensitive path data source expression list separator expected - %s read",
        __prt_vtok());
       goto bad_end;
      }
     __get_vtok();
     if (__toktyp == COMMA || __toktyp == RPAR) 
      {
        __pv_ferr(1259,
         "edge sensitive path data source expression ,, or ,) forms illegal");
        goto bad_end;
      }
    }
  }
 else if ((olstx = rd_pthtermlst()) == NULL) goto bad_pth;

chk_endrpar:
 if (__toktyp != RPAR)
  {
   __pv_ferr(1256,
    "path output terminal list right parenthesis expected - %s read",
    __prt_vtok());
   goto bad_pth;
  }
get_eq:
 __get_vtok();
 if (__toktyp != EQ) 
  {
   __pv_ferr(1257, "path delay equal sign expected - %s read",
    __prt_vtok());
bad_end:
   if (!__spec_vskipto_any(SEMI)) return(FALSE);
   return(TRUE);
  }
got_eq:
 /* know symbol table env. in specify always specparam local on mod. lev. */
 __sav_spsytp = __venviron[0];
 __venviron[0] = __venviron[1];
 __top_sti = 0;
 /* notice = read but not ( - unusual case of no 1st token read before call */ 
 good = rd_pathdelaylist(&pmphdr);
 __venviron[0] = __sav_spsytp;
 __top_sti++; 
 if (!good) goto bad_end;

 /* build the path delay element */
 pthp = (struct spcpth_t *) __my_malloc(sizeof(struct spcpth_t));
 init_spcpth(pthp);
 pthp->pthtyp = pdtyp; 
 pthp->pthpolar = pthpolar; 
 pthp->peins = (struct pathel_t *) ilstx;
 pthp->peouts = (struct pathel_t *) olstx;

 /* add the location identifying symbol */
 pthsyp = __bld_loc_symbol(__path_num, __venviron[0], "path", "delay path");
 pthsyp->sytyp = SYM_PTH;
 pthsyp->syfnam_ind = __cur_fnam_ind;
 pthsyp->sylin_cnt = __lin_cnt;
 pthp->pthsym = pthsyp;
 pthsyp->el.epthp = pthp;
 pthsyp->sydecl = TRUE;
 __path_num++;

 /* set delay part */
 pthp->pth_delrep = DT_CMPLST;
 pthp->pth_du.pdels = pmphdr;

 /* set sdpd and conditional path values */ 
 pthp->pthedge = pth_edge;  
 pthp->dsrc_polar = datasrcpolar;
 pthp->datasrcx = datsrcx;
 pthp->pth_ifnone = is_ifnone;
 pthp->pthcondx = condx;

 if (__end_spcpths == NULL) __cur_spfy->spcpths = pthp;
 else __end_spcpths->spcpthnxt = pthp;
 __end_spcpths = pthp;
 return(TRUE);
}

/*
 * read a path terminal list
 * know 1st token read and reads ending )
 */
static struct exprlst_t *rd_pthtermlst(void)
{
 struct exprlst_t *xpmphdr, *xpmp, *last_xpmp;

 /* build specify terminal list - semantics that requires subset of mod. */
 /* I/O port terminals checked later */
 /* no ,, and at least one required */
 for (xpmphdr = NULL, last_xpmp = NULL;;)
  {
   /* this will have skipped to end of statement on error */
   if (!col_pthexpr()) return(NULL);
   __bld_xtree(0);
   xpmp = __alloc_xprlst();
   xpmp->xp = __root_ndp;
   if (last_xpmp == NULL) xpmphdr = xpmp; else last_xpmp->xpnxt = xpmp;
   last_xpmp = xpmp;
   if (__toktyp == RPAR || __toktyp == FPTHCON || __toktyp == PPTHCON
    || __toktyp == PLUS || __toktyp == MINUS || __toktyp == TCHKEVAND
    || __toktyp == COLON) break;
   __get_vtok();
  }
 return(xpmphdr);
}

/*
 * collect a delay path terminal expression
 * expects 1st token to be read and read ending token
 * ends with ) or ',' or => or *> or - or + or : preceding connection op.
 * for timing checks can end with TCHKEVAND &&&
 * allows full expressions because port bit select can be full const. expr.
 *
 * notice ending thing not included in expr. but left in token
 *
 * this collects a specify expr. - caller must eliminate wrong tokens for
 * either tchk or path 
 *
 * notice this is lhs element not delay constants
 * maybe should be empty on error
 */
static int32 col_pthexpr(void)
{
 int32 parlevel, sqblevel;

 for (__last_xtk = -1, parlevel = 0, sqblevel = 0;;)
  {
   switch ((byte) __toktyp) {
    case LPAR: parlevel++; break;
    /* any expression (must later be constant) legal in selects */
    case LSB: sqblevel++; break;
    case RPAR: 
     if (parlevel <= 0 && sqblevel <= 0) return(TRUE); else parlevel--;
     break;
   case RSB:
    sqblevel--;
    break;
   case COMMA:
    /* illegal here but parse and check later */
    if (parlevel <= 0 && sqblevel <= 0) return(TRUE);
    break;
   case PLUS: case MINUS:
    if (parlevel <= 0 && sqblevel <= 0) return(TRUE);
    break;
   case COLON:
    if (parlevel <= 0 && sqblevel <= 0) return(TRUE);
    break;
   /* notice these are never nested */
   case FPTHCON: case PPTHCON: case TCHKEVAND:
    return(TRUE);
   case TEOF:
   case SEMI:
    goto bad_end;
   }
   if (!__bld_expnode()) goto bad_end;
   __get_vtok();
  }

bad_end:
 __set_xtab_errval();
 return(FALSE);
}

/*
 * read delay path list 
 * almost same as read oparams del but surrounding () always optional
 *
 * reads and checks for ending ; 
 * builds a parameter/delay list and returns pointer to header
 * returns F on sync error - caller must resync
 * but in places with known delimiter attempt to resync to delim 
 */
static int32 rd_pathdelaylist(struct paramlst_t **pmphdr)
{
 int32 rv;
 struct paramlst_t *pmp, *last_pmp;

 *pmphdr = NULL;

 /* this is #[number] or #id - not (..) form - min:typ:max requires () */
 /* for path delay will never see this form */
 __get_vtok();
 /* case 1: has optional () surround list */
 if (__toktyp == LPAR) 
  {
   for (last_pmp = NULL;;)
    {
     __get_vtok();
     if (!__col_delexpr())
      {
       /* need to look to skip to any possible end ( may be unmatched */
       if (!__vskipto3_modend(COMMA, RPAR, SEMI)) return(FALSE);
       if (__toktyp == SEMI) return(FALSE);
       if (__toktyp == RPAR) { __get_vtok(); rv = FALSE; goto chk_semi; }
       /* if comman do not add but continue checking */
       continue;
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
      "path delay delay list comma or right parenthesis expected - %s read",
      __prt_vtok());
     return(FALSE);
    }
   rv = TRUE;
   __get_vtok();
chk_semi:
   if (__toktyp != SEMI)
    {
     __pv_ferr(1279, "path delay final ; expected - %s read", __prt_vtok());
     return(FALSE);
    }
   return(rv);
  }
 /* case 2: optional surrounding omitted */ 
 for (last_pmp = NULL;;)
  {
   /* this reads the end , or ; */
   if (!__col_paramrhsexpr())
    {
     /* need to look to skip to any possible end ( may be unmatched */
     if (!__vskipto3_modend(COMMA, RPAR, SEMI)) return(FALSE);
     if (__toktyp == SEMI) return(FALSE);
     if (__toktyp == RPAR) { __get_vtok(); rv = FALSE; goto chk_semi; }
     __get_vtok();
     continue;
    }       
   __bld_xtree(0);
   pmp = __alloc_pval();
   pmp->plxndp = __root_ndp;
   if (last_pmp == NULL) *pmphdr = pmp; else last_pmp->pmlnxt = pmp;
   last_pmp = pmp;

   if (__toktyp == COMMA) { __get_vtok(); continue; }
   if (__toktyp == SEMI) break;
   /* should never happen - sync on err above, if does give up */
   __pv_ferr(1050,
    "path delay delay list comma or semicolon expected - %s read",
     __prt_vtok());
   return(FALSE);
  }
 return(TRUE);
}
 
/*
 * initialize a specify section delay path
 */
static void init_spcpth(struct spcpth_t *pthp)
{
 pthp->pthtyp = PTH_NONE;
 pthp->pth_gone = FALSE;
 pthp->pth_as_xprs = TRUE;
 pthp->pth_delrep = DT_NONE;
 pthp->pthpolar = FALSE;
 pthp->pthedge = NOEDGE;
 pthp->dsrc_polar = POLAR_NONE; 
 pthp->pth_ifnone = FALSE;
 pthp->pth_0del_rem = FALSE;
 pthp->pthsym = NULL;
 pthp->last_pein = -1;
 pthp->last_peout = -1;
 pthp->peins = NULL;
 pthp->peouts = NULL;
 pthp->pth_du.d1v = NULL;
 pthp->datasrcx = NULL;
 pthp->pthcondx = NULL;
 pthp->spcpthnxt = NULL;
}

/*
 * TIMING CHECK READ ROUTINES
 */

/*
 * read setup or hold timing check
 * know system task keyword and ( read and reads ending ; if possible
 *
 * read and parse timing checks as is - during prep changed to needed form
 */
static int32 rd_setup_or_hold_tchk(word32 ttyp)
{
 int32 fnum, lnum;
 struct sy_t *syp, *tcsyp;
 struct paramlst_t *pmp;
 struct tchk_t tmptchk;
 struct expr_t *limx;

 __init_tchk(&tmptchk, ttyp);
 /* must save location since, need system task line as tchk loc. */
 fnum = __cur_fnam_ind;
 lnum = __lin_cnt;
 if (!rd_tchk_part(ttyp, &tmptchk, &limx)) return(FALSE);
 /* notice can only be ID if present */
 if (__toktyp == COMMA) { syp = rd_notifier(); __get_vtok(); }
 else syp = NULL;
 /* even even error end, still add since good */
 if (__toktyp != RPAR) goto noparsemi;
 __get_vtok();
 if (__toktyp != SEMI)
  {
noparsemi:
   __pv_ferr(1261, "%s timing check does not end with ); - %s read",
    __to_tcnam(__xs, ttyp), __prt_vtok());
   if (!__spec_vskipto_any(SEMI)) return(FALSE);
  }
 __cur_tchk = (struct tchk_t *) __my_malloc(sizeof(struct tchk_t));
 *__cur_tchk = tmptchk; 
 /* add the location idnentifying symbol */
 tcsyp = __bld_loc_symbol(__tchk_num, __venviron[0], "tchk", "timing check");
 tcsyp->sytyp = SYM_TCHK;
 tcsyp->syfnam_ind = fnum;
 tcsyp->sylin_cnt = lnum;
 tcsyp->el.etcp = __cur_tchk;
 tcsyp->sydecl = TRUE;
 __cur_tchk->tcsym = tcsyp;
 tcsyp->el.etcp = __cur_tchk;
 __tchk_num++;

 __cur_tchk->ntfy_np = (struct net_t *) syp;
 /* setup and hold identical - users changes order of args - ref. event */
 /* always first so $setup(data, clk, ...), $hold(clk, data, ...) */
 pmp = __alloc_pval(); pmp->plxndp = limx; pmp->pmlnxt = NULL;
 __cur_tchk->tclim_du.pdels = pmp;
 return(TRUE);
} 

/*
 * build timing check symbol
 *
 * even if already declared, change to delay object
 * if used previously, change to delay object - error if previously declared
 * if used in previous expr. but later declared, error emitted at declaration
 * common case first used in $set[it]delay expr. and added to symbol table
 * as wire then here changed to delay object
 */
extern struct sy_t *__bld_loc_symbol(int32 num, struct symtab_t *sytp,
 char *pref, char *emsgnam)
{
 char s1[RECLEN];
 struct sy_t *syp;

 sprintf(s1, "__%s$$%d", pref, num);
 syp = __decl_sym(s1, sytp);
 if (!__sym_is_new)
  {
   if (syp->sydecl)
    {
     __pv_ferr(1160,
      "%s constructed internal name %s conflicts with declared %s",
      emsgnam, s1, __to_sytyp(__xs, syp->sytyp));
    }
   else
    {
     __pv_ferr(1160, "%s constructed internal name %s conflicts with wire",
      emsgnam, s1);
    }
  }
 return(syp);
}

/*
 * read the event and first limit part of all timing checks
 * common code for all timing checks, limits differ
 * this must read 1st token before reading tchk events
 * if returns F, parameters not set
 * this syncs to ; or item location and returns F 
 * on T reads ) or , after first (maybe only) limit
 * notice 1 limit always required
 */
static int32 rd_tchk_part(word32 ttyp, struct tchk_t *tcp,
 struct expr_t **limx)
{
 struct expr_t *xp, *condx;
 int32 edgval;

 /* notice ref. always first */
 __get_vtok();
 if (!rd_tchk_selector(&edgval, &xp, &condx))
  {
   if (!__spec_vskipto2_any(SEMI, COMMA)) return(FALSE);
   if (__toktyp == SEMI)
    {
got_semi:
     __syncto_class = SYNC_SPECITEM;
     return(FALSE);
    }
  }
 tcp->startedge = edgval;  
 tcp->startxp = xp;
 tcp->startcondx = condx;

 /* tcp initialized to empty fields */
 if (ttyp != TCHK_WIDTH && ttyp != TCHK_PERIOD) 
  {
   __get_vtok();
   if (!rd_tchk_selector(&edgval, &xp, &condx))
    {
     if (!__spec_vskipto2_any(SEMI, COMMA)) return(FALSE);
     if (__toktyp == SEMI) goto got_semi;
    }  
   tcp->chkedge = edgval;  
   tcp->chkxp = xp;
   tcp->chkcondx = condx;
  }
 __get_vtok();
 __sav_spsytp = __venviron[0];
 __venviron[0] = __cur_spfy->spfsyms;
 /* limit is one number but can be d:d:d form - but know ends with , or ) */
 if (!__col_delexpr())
  { 
   if (!__spec_vskipto2_any(SEMI, COMMA))
    { __venviron[0] = __sav_spsytp; return(FALSE); }
   if (__toktyp == SEMI)
    { __venviron[0] = __sav_spsytp; goto got_semi; }
   /* set error, if ,, form will not get here */
   /* make it look like ,, form */
   __set_0tab_errval();
  }
 __bld_xtree(0);
 if (__has_top_mtm)
  {
   __pv_fwarn(653,
    "%s timing check min:typ:max limit expression needs parentheses under 1364 - unportable",
    __to_tcnam(__xs, ttyp));
  }
 *limx = __root_ndp;
 __venviron[0] = __sav_spsytp;
 return(TRUE);
}

/*
 * initialize a timing check record
 */
extern void __init_tchk(struct tchk_t *tcp, word32 ttyp)
{
 tcp->tchktyp = ttyp; 
 /* notice del rep applies to both limits if present */
 tcp->tc_delrep = DT_CMPLST; 
 tcp->tc_delrep2 = DT_CMPLST; 
 tcp->tc_gone = FALSE;
 tcp->tc_supofsuphld = FALSE;
 tcp->tc_recofrecrem = FALSE;
 tcp->tc_haslim2 = FALSE;
 tcp->startedge = NOEDGE;
 tcp->startxp = NULL;
 tcp->tcsym = NULL;
 tcp->startcondx = NULL;
 tcp->chkedge = NOEDGE;
 tcp->chkxp = NULL;
 tcp->chkcondx = NULL;
 /* notice this may be nil */
 tcp->ntfy_np = NULL;
 tcp->tclim_du.pdels = NULL;
 tcp->tclim2_du.pdels = NULL;
 tcp->tchknxt = NULL;
}

/*
 * read setuphold timing check (both with 2 limits)
 * know system task keyword read
 */
static int32 rd_setuphold_tchk(void)
{
 word32 ttyp;
 int32 fnum, lnum;
 struct tchk_t tmptchk;
 struct expr_t *limx, *lim2x;
 struct sy_t *syp, *tcsyp;
 struct paramlst_t *pmp;

 ttyp = TCHK_SETUPHOLD;
 __init_tchk(&tmptchk, ttyp);
 fnum = __cur_fnam_ind;
 lnum = __lin_cnt;

 if (!rd_tchk_part(ttyp, &tmptchk, &limx)) return(FALSE);
 if (__toktyp != COMMA)
  {
   __pv_ferr(1267,
    "$setuphold hold limit expression , terminator expected - %s read",
    __prt_vtok());
   __spec_vskipto_any(SEMI);
   return(FALSE);
  }
 __get_vtok();
 __sav_spsytp = __venviron[0];
 __venviron[0] = __cur_spfy->spfsyms;
 /* can be ,, or ,) empty but required */
 if (!__col_delexpr())
  { 
   if (!__spec_vskipto2_any(SEMI, COMMA))
    { __venviron[0] = __sav_spsytp; return(FALSE); }
   if (__toktyp == SEMI)
    {
     __venviron[0] = __sav_spsytp;
     __syncto_class = SYNC_SPECITEM;
     return(FALSE);
    }
   /* set rhs error expr. */
   __set_0tab_errval();
  }
 __bld_xtree(0);
 if (__has_top_mtm)
  {
   __pv_fwarn(653,
    "%s timing check min:typ:max 2nd limit expression needs parentheses under 1364 - unportable",
    __to_tcnam(__xs, ttyp));
  }
 lim2x = __root_ndp;
 __venviron[0] = __sav_spsytp;

 /* notice can only be ID if present */
 if (__toktyp == COMMA) { syp = rd_notifier(); __get_vtok(); }
 else syp = NULL;
 /* even even error end, still add since good */
 if (__toktyp != RPAR) goto noparsemi;
 __get_vtok();
 if (__toktyp != SEMI)
  {
noparsemi:
   __pv_ferr(1262, "$setuphold timing check does not end with ); - %s read",
    __prt_vtok());
   if (!__spec_vskipto_any(SEMI)) return(FALSE);
  }
 __cur_tchk = (struct tchk_t *) __my_malloc(sizeof(struct tchk_t));
 *__cur_tchk = tmptchk; 

 /* add the location idnentifying symbol */
 tcsyp = __bld_loc_symbol(__tchk_num, __venviron[0], "tchk", "timing check");
 tcsyp->sytyp = SYM_TCHK;
 tcsyp->syfnam_ind = fnum;
 tcsyp->sylin_cnt = lnum;
 __cur_tchk->tcsym = tcsyp;
 tcsyp->el.etcp = __cur_tchk;
 __tchk_num++;
 __cur_tchk->ntfy_np = (struct net_t *) syp;
 pmp = __alloc_pval();
 pmp->plxndp = limx;
 pmp->pmlnxt = NULL;
 __cur_tchk->tclim_du.pdels = pmp;
 pmp = __alloc_pval();
 pmp->plxndp = lim2x;
 pmp->pmlnxt = NULL;
 __cur_tchk->tclim2_du.pdels = pmp;
 __cur_tchk->tc_haslim2 = TRUE;
 return(TRUE);
}

/*
 * read recrem timing check (both with 2 limits)
 * know system task keyword read
 *
 * SJM 01/16/04 - almost same as setup hold but 2001 LRM has extra stuff
 * that is not yet added
 */
static int32 rd_recrem_tchk(void)
{
 word32 ttyp;
 int32 fnum, lnum;
 struct tchk_t tmptchk;
 struct expr_t *limx, *lim2x;
 struct sy_t *syp, *tcsyp;
 struct paramlst_t *pmp;
 char s1[RECLEN];

 ttyp = TCHK_RECREM;
 __init_tchk(&tmptchk, ttyp);
 fnum = __cur_fnam_ind;
 lnum = __lin_cnt;

 if (!rd_tchk_part(ttyp, &tmptchk, &limx)) return(FALSE);

 /* for recrem both terminals must be edges */ 
 if (tmptchk.startedge == NOEDGE)
  {
   __pv_ferr(1260,
   "%s timing check first recovery reference event missing required edge",
   __to_tcnam(s1, ttyp));
  }
 if (tmptchk.chkedge == NOEDGE)
  {
   __pv_ferr(1260,
    "%s timing 2nd removal reference event missing required edge",
   __to_tcnam(s1, ttyp));
  }

 if (__toktyp != COMMA)
  {
   __pv_ferr(1267,
    "$recrem removal limit expression , terminator expected - %s read",
    __prt_vtok());
   __spec_vskipto_any(SEMI);
   return(FALSE);
  }
 __get_vtok();
 __sav_spsytp = __venviron[0];
 __venviron[0] = __cur_spfy->spfsyms;
 /* can be ,, or ,) empty but required */
 if (!__col_delexpr())
  { 
   if (!__spec_vskipto2_any(SEMI, COMMA))
    { __venviron[0] = __sav_spsytp; return(FALSE); }
   if (__toktyp == SEMI)
    {
     __venviron[0] = __sav_spsytp;
     __syncto_class = SYNC_SPECITEM;
     return(FALSE);
    }
   /* set rhs error expr. */
   __set_0tab_errval();
  }
 __bld_xtree(0);
 if (__has_top_mtm)
  {
   __pv_fwarn(653,
    "%s timing check min:typ:max 2nd limit expression needs parentheses under 1364 - unportable",
    __to_tcnam(__xs, ttyp));
  }
 lim2x = __root_ndp;
 __venviron[0] = __sav_spsytp;

 /* notice can only be ID if present */
 if (__toktyp == COMMA) { syp = rd_notifier(); __get_vtok(); }
 else syp = NULL;
 /* even even error end, still add since good */
 if (__toktyp != RPAR) goto noparsemi;
 __get_vtok();
 if (__toktyp != SEMI)
  {
noparsemi:
   __pv_ferr(1262, "$setuphold timing check does not end with ); - %s read",
    __prt_vtok());
   if (!__spec_vskipto_any(SEMI)) return(FALSE);
  }
 __cur_tchk = (struct tchk_t *) __my_malloc(sizeof(struct tchk_t));
 *__cur_tchk = tmptchk; 

 /* add the location idnentifying symbol */
 tcsyp = __bld_loc_symbol(__tchk_num, __venviron[0], "tchk", "timing check");
 tcsyp->sytyp = SYM_TCHK;
 tcsyp->syfnam_ind = fnum;
 tcsyp->sylin_cnt = lnum;
 __cur_tchk->tcsym = tcsyp;
 tcsyp->el.etcp = __cur_tchk;
 __tchk_num++;
 __cur_tchk->ntfy_np = (struct net_t *) syp;
 pmp = __alloc_pval();
 pmp->plxndp = limx;
 pmp->pmlnxt = NULL;
 __cur_tchk->tclim_du.pdels = pmp;
 pmp = __alloc_pval();
 pmp->plxndp = lim2x;
 pmp->pmlnxt = NULL;
 __cur_tchk->tclim2_du.pdels = pmp;
 __cur_tchk->tc_haslim2 = TRUE;
 return(TRUE);
}

/*
 * read width timing check
 * know system task keyword read
 * width has 2 limits (but 2nd can be omitted), period has 1
 */
static int32 rd_width_tchk(void)
{
 word32 ttyp;
 int32 fnum, lnum;
 struct tchk_t tmptchk;
 struct expr_t *limx, *lim2x;
 struct sy_t *syp, *tcsyp;
 struct paramlst_t *pmp;

 ttyp = TCHK_WIDTH;
 __init_tchk(&tmptchk, ttyp);

 fnum = __cur_fnam_ind;
 lnum = __lin_cnt;

 if (!rd_tchk_part(ttyp, &tmptchk, &limx)) return(FALSE);
 if (tmptchk.startedge != E_NEGEDGE && tmptchk.startedge != E_POSEDGE)
  __pv_ferr(1266,
   "$width timing check event missing required negedge or posedge"); 
 /* 2nd limit optional and becomes NULL */
 if (__toktyp == RPAR) { syp = NULL; lim2x = NULL; goto done; }
 if (__toktyp != COMMA)
  {
   __pv_ferr(1268,
    "$width first limit expression , terminator expected - %s read",
    __prt_vtok());
   __spec_vskipto_any(SEMI);
   return(FALSE);
  }
 __get_vtok();
 __sav_spsytp = __venviron[0];
 __venviron[0] = __cur_spfy->spfsyms;
 /* lrm says no ,, or ,) forms for width */
 /* since ignored by sim value of 0 ok place holder and just ignored */
 if (!__col_delexpr())
  { 
   if (!__spec_vskipto2_any(SEMI, COMMA))
    { __venviron[0] = __sav_spsytp; return(FALSE); }
   if (__toktyp == SEMI)
    {
     __venviron[0] = __sav_spsytp;
     __syncto_class = SYNC_SPECITEM;
     return(FALSE);
    }
   /* make it look like ,, form */
   __set_0tab_errval();
  }
 __bld_xtree(0);
 lim2x = __root_ndp;
 __venviron[0] = __sav_spsytp;

 /* notice can only be ID and may be omited */
 if (__toktyp == COMMA) { syp = rd_notifier(); __get_vtok(); }
 else syp = NULL;
 /* even even error end, still add since good */
 if (__toktyp != RPAR) goto noparsemi;
done:
 __get_vtok();
 if (__toktyp != SEMI)
  {
noparsemi:
   __pv_ferr(1263, "$width timing check does not end with ); - %s read",
    __prt_vtok());
   if (!__spec_vskipto_any(SEMI)) return(FALSE);
  }

 /* notice no data even here */
 __cur_tchk = (struct tchk_t *) __my_malloc(sizeof(struct tchk_t));
 *__cur_tchk = tmptchk; 

 /* add the location idnentifying symbol */
 tcsyp = __bld_loc_symbol(__tchk_num, __venviron[0], "tchk", "timing check");
 tcsyp->sytyp = SYM_TCHK;
 tcsyp->syfnam_ind = fnum;
 tcsyp->sylin_cnt = lnum;
 tcsyp->sydecl = TRUE;
 __cur_tchk->tcsym = tcsyp;
 tcsyp->el.etcp = __cur_tchk;
 __tchk_num++;

 __cur_tchk->ntfy_np = (struct net_t *) syp;
 pmp = __alloc_pval();
 pmp->plxndp = limx;
 pmp->pmlnxt = NULL;
 __cur_tchk->tclim_du.pdels = pmp;
 /* always build 2nd limit as 0 if missing (during fix) so present */
 __cur_tchk->tc_haslim2 = TRUE;
 /* notice missing 2nd limit ok since only timing verifier threshold */
 /* that stops errors if pulse less than threshold */ 
 if (lim2x == NULL) lim2x = __bld_rng_numxpr(0L, 0L, WBITS);
 pmp = __alloc_pval();
 pmp->plxndp = lim2x;
 pmp->pmlnxt = NULL;
 __cur_tchk->tclim2_du.pdels = pmp;
 return(TRUE);
}

/*
 * read period timing check
 * know system task keyword read
 * period has 1 limit (required), width has 2
 */
static int32 rd_period_tchk(void)
{
 word32 ttyp;
 int32 fnum, lnum;
 struct tchk_t tmptchk;
 struct expr_t *limx;
 struct sy_t *syp, *tcsyp;
 struct paramlst_t *pmp;

 ttyp = TCHK_PERIOD;
 __init_tchk(&tmptchk, ttyp);

 fnum = __cur_fnam_ind;
 lnum = __lin_cnt;

 if (!rd_tchk_part(ttyp, &tmptchk, &limx)) return(FALSE);
 if (tmptchk.startedge == NOEDGE)
  __pv_ferr(1269, "$period timing check event missing required edge"); 
 /* notice can only be ID if present */
 if (__toktyp == COMMA) { syp = rd_notifier(); __get_vtok(); }
 else syp = NULL;
 /* even even error end, still add since good */
 if (__toktyp != RPAR) goto noparsemi;
 __get_vtok();
 if (__toktyp != SEMI)
  {
noparsemi:
   __pv_ferr(1264, "$period timing check does not end with ); - %s read",
    __prt_vtok());
   if (!__spec_vskipto_any(SEMI)) return(FALSE);
  }
 __cur_tchk = (struct tchk_t *) __my_malloc(sizeof(struct tchk_t));
 *__cur_tchk = tmptchk; 

 /* add the location idnentifying symbol */
 tcsyp = __bld_loc_symbol(__tchk_num, __venviron[0], "tchk", "timing check");
 tcsyp->sytyp = SYM_TCHK;
 tcsyp->syfnam_ind = fnum;
 tcsyp->sylin_cnt = lnum;
 tcsyp->sydecl = TRUE;
 __cur_tchk->tcsym = tcsyp;
 tcsyp->el.etcp = __cur_tchk;
 __tchk_num++;

 __cur_tchk->ntfy_np = (struct net_t *) syp;
 pmp = __alloc_pval();
 pmp->plxndp = limx;
 pmp->pmlnxt = NULL;
 __cur_tchk->tclim_du.pdels = pmp;
 return(TRUE);
}

/*
 * read skew or recovery timing check
 * know system task keyword read
 * different timing checks have identical arguments
 *
 * SJM 01/16/04 - added removal first terminal is the ref events that
 * needs to be an edge for both
 */
static int32 rd_skew_recov_rem_tchk(word32 ttyp)
{
 int32 fnum, lnum;
 struct tchk_t tmptchk;
 struct expr_t *limx;
 struct sy_t *syp, *tcsyp;
 struct paramlst_t *pmp;
 char s1[RECLEN];

 __init_tchk(&tmptchk, ttyp);
 /* must save location since, need system task line as tchk loc. */
 fnum = __cur_fnam_ind;
 lnum = __lin_cnt;
 if (!rd_tchk_part(ttyp, &tmptchk, &limx)) return(FALSE);
 if (ttyp == TCHK_RECOVERY || ttyp == TCHK_REMOVAL)
  {
   if (tmptchk.startedge == NOEDGE)
    __pv_ferr(1260,
     "%s timing check first reference event missing required edge",
     __to_tcnam(s1, ttyp));
  }

 /* notice can only be ID if present */
 if (__toktyp == COMMA) { syp = rd_notifier(); __get_vtok(); }
 else syp = NULL;
 /* even even error end, still add since good */
 if (__toktyp != RPAR) goto noparsemi;
 __get_vtok();
 if (__toktyp != SEMI)
  {
noparsemi:
   __pv_ferr(1265, "%s timing check does not end with ); - %s read",
    __to_tcnam(s1, ttyp), __prt_vtok());
   if (!__spec_vskipto_any(SEMI)) return(FALSE);
  }
 __cur_tchk = (struct tchk_t *) __my_malloc(sizeof(struct tchk_t));
 *__cur_tchk = tmptchk; 

 /* add the location identifying symbol */
 tcsyp = __bld_loc_symbol(__tchk_num, __venviron[0], "tchk", "timing check");
 tcsyp->sytyp = SYM_TCHK;
 tcsyp->syfnam_ind = fnum;
 tcsyp->sylin_cnt = lnum;
 tcsyp->sydecl = TRUE;
 __cur_tchk->tcsym = tcsyp;
 tcsyp->el.etcp = __cur_tchk;
 __tchk_num++;

 __cur_tchk->ntfy_np = (struct net_t *) syp;
 pmp = __alloc_pval(); pmp->plxndp = limx; pmp->pmlnxt = NULL;
 __cur_tchk->tclim_du.pdels = pmp;
 return(TRUE);
}

/*
 * must correctly parse $nochange but ignore with warning
 * this does not build and data structure
 */
static int32 rd_nochg_tchk(void)
{
 word32 ttyp;
 struct tchk_t tmptchk;
 struct expr_t *limx;
 char s1[RECLEN];

 ttyp = TCHK_NOCHANGE;
 __init_tchk(&tmptchk, ttyp);
 /* this reads the first limit but not second */
 if (!rd_tchk_part(ttyp, &tmptchk, &limx)) return(FALSE);
 if (tmptchk.startedge != E_NEGEDGE && tmptchk.startedge != E_POSEDGE)
  __pv_ferr(1271,
   "$nochange timing check first reference event missing negedge or posedge"); 
 __free_xtree(limx);
 __free_xtree(tmptchk.startxp);
 __free_xtree(tmptchk.startcondx);
 __free_xtree(tmptchk.chkxp);
 __free_xtree(tmptchk.chkcondx);

 if (__toktyp != COMMA)
  {
   __pv_ferr(1272,
    "$nochange second event - comma expected - %s read", __prt_vtok());
   __spec_vskipto_any(SEMI);
   return(FALSE);
  }
 __get_vtok();
 __sav_spsytp = __venviron[0];
 __venviron[0] = __cur_spfy->spfsyms;
 /* lrm says no ,, or ,) forms for nochange */
 if (!__col_delexpr())
  { 
   if (!__spec_vskipto2_any(SEMI, COMMA))
    { __venviron[0] = __sav_spsytp; return(FALSE); }
   if (__toktyp == SEMI)
    {
     __venviron[0] = __sav_spsytp;
     __syncto_class = SYNC_SPECITEM;
     return(FALSE);
    }
   /* make it look like ,, form */
   __set_0tab_errval();
  }
 /* this is needed for checking */
 __bld_xtree(0);
 __free_xtree(__root_ndp);
 __venviron[0] = __sav_spsytp;
 /* even even error end, still add since good */
 if (__toktyp != RPAR) goto noparsemi;

 __get_vtok();
 if (__toktyp != SEMI)
  {
noparsemi:
   __pv_ferr(1273, "%s timing check does not end with ); - %s read",
    __to_tcnam(s1, ttyp), __prt_vtok());
   if (!__spec_vskipto_any(SEMI)) return(FALSE);
  }
 __pv_fwarn(599, "$nochange timing check has no effect in simulation");
 return(TRUE);
}

/*
 * read a timing check event - know always followed by , 
 * know 1st token read and reads ending ,
 * returns NULL on error, caller must skip to ;
 * caller syncs - returns NULL if syntax error (will need syncing)
 */
static int32 rd_tchk_selector(int32 *edgval, struct expr_t **xp,
 struct expr_t **condx)
{
 *edgval = NOEDGE;
 switch ((byte) __toktyp) {  
  case NEGEDGE: *edgval = E_NEGEDGE; break; 
  case POSEDGE: *edgval = E_POSEDGE; break; 
  case EDGE:
   __get_vtok();
   if (__toktyp != LSB) 
    {
     __pv_ferr(1281, "timing check event edge list [ expected - %s read",
      __prt_vtok());
edge_sync:
     /* caller must synchronize - except try for enclosing ] */
     if (!__spec_vskipto_any(RSB)) return(FALSE);
     goto get_pthx;
    }
   if (!rd_edges(edgval)) goto edge_sync;
   break;
  default: goto no_edge;
 }

get_pthx:
 __get_vtok();
no_edge:
 if (!col_pthexpr()) return(FALSE);
 __bld_xtree(0);
 *xp = __root_ndp;
 if (__toktyp != COMMA)
  {
   if (__toktyp != TCHKEVAND)
    {
     __pv_ferr(1282,
      "timing check data or reference event, comma or &&& expected - %s read",
      __prt_vtok());
     return(FALSE);
    }
   /* read &&& expr. */
   __get_vtok();
   if (!__col_connexpr(-1)) return(FALSE);
   if (__toktyp != COMMA)
    {
     __pv_ferr(1283,
      "timing check event &&& expression comma expected - %s read",
      __prt_vtok());
     return(FALSE);
    }
   __bld_xtree(0);
   *condx = __root_ndp;
  }
 else *condx = NULL;
 return(TRUE);
}

/*
 * read an edge list
 * know [ read and reads trailing ]
 * if error tries to sync to , ], ), ;
 */
static int32 rd_edges(int32 *edge)
{
 char s1[RECLEN];
 byte etmp, e1;

 *edge = etmp = NOEDGE;
 __letendnum_state = TRUE;
 for (;;)
  {
   strcpy(s1, "");
   for (;;)
    {
     __get_vtok();
     switch ((byte) __toktyp) {
      case COMMA: case RSB: goto got_pair;
      case ID: strcat(s1, __token); break;
      case NUMBER: strcat(s1, __numtoken); break;
      default:
       __pv_ferr(1284, "edge list edge value pair expected - %s read",
        __prt_vtok());
       __letendnum_state = FALSE;
       return(FALSE);
      }
    }
got_pair:
   if (strlen(s1) > 2)
    {
bad_edge:
     __pv_ferr(1286, "edge list element %s illegal", s1);
     continue;
    }
   switch (s1[0]) {
    case '0':
     if (s1[1] == '1') e1 = EDGE01;
     else if (s1[1] == 'x') e1 = EDGE0X;
     else goto bad_edge;
     break;
    case '1':
     if (s1[1] == '0') e1 = EDGE10;
     else if (s1[1] == 'x') e1 = EDGE1X;
     else goto bad_edge;
     break;
    case 'x':
     if (s1[1] == '0') e1 = EDGEX0;
     else if (s1[1] == '1') e1 = EDGEX1;
     else goto bad_edge;
     break;
    default: goto bad_edge;
   }
   if ((etmp & e1) != 0)
    __pv_fwarn(577, "edge %s repeated in edge list", s1);
   else etmp |= e1;
   /* notice last edge will be vv with __toktyp of ] - must proces last */
   if (__toktyp == RSB) break;
  }
 __letendnum_state = FALSE;
 *edge = etmp;
 return(TRUE);
}

/*
 * read an notifier 
 * know leading , read and reads just the notifier reg
 */
static struct sy_t *rd_notifier(void) 
{ 
 struct sy_t *syp;

 __get_vtok();
 if (__toktyp != ID)
  {
bad_notfy:
   __pv_ferr(1285, "notifier register name expected - %s read",
    __prt_kywrd_vtok());
   return(NULL);
  } 
 /* this declares thing as a net - fixed later and checked even later */
 __last_xtk = -1;
 /* FIXME - since can never fail should change to arg terr */
 if (!__bld_expnode()) goto bad_notfy;
 __bld_xtree(0);
 syp = __root_ndp->lu.sy;
 /* type will be checked later */
 return(syp);
}

/*
 * ROUTINES TO PROCESS `language INCLUDE CONSTRUCT
 */

extern char __pv_ctab[];

/*
 * read lines after language up to `endlanguage
 *
 * know first token of line read and it is `language
 * reads through `endlanguage
 *
 * both `language and `endlanguage lines passed to call back if registered
 */
extern void __do_foreign_lang(void)
{
 register char *chp, *chp2;
 int32 first_time, done, savfnam_ind, sav_lin_cnt;

 if (!__rding_top_level || __rescanning_lib)
  {
   if (!__rescanning_lib)
    {
     __pv_ferr(2657,
      "`language compiler directive inside Verilog construct - skipping to `endlanguage");
    }
   savfnam_ind = __cur_fnam_ind;
   sav_lin_cnt = __lin_cnt;
   if (__notokenize_skiplines("`endlanguage") == TEOF)
    {
     __pv_terr(327,
      "skipped `language line %s no matching `endlanguage before **EOF**",
      __bld_lineloc(__xs, (word32) savfnam_ind, sav_lin_cnt));  
    }
   if (__langstr != NULL) strcpy(__langstr, "");
   return;
  }
 if (__lib_verbose || __verbose)
  {
   __cv_msg("  Processing `language directive at **%s(%d)\n", 
    __in_fils[__cur_fnam_ind], __lin_cnt);
  }
 __total_lang_dirs++;
 if (__iact_state)
  {
   __ia_err(1401,
    "`language compiler directive illegal in interactive commands");
   __skipover_line();
   if (__langstr != NULL) strcpy(__langstr, "");
   return; 
  }
 if (__langstr == NULL) __langstr = __my_malloc(IDLEN + 1);

 __doing_langdir = TRUE;
 for (first_time = TRUE, done = FALSE;;)
  {
rd_again:
   chp = __langstr;
   /* this does not move line count to next line - code here must */
   if (__my_getlin(__langstr) == EOF) 
    {
     /* first try to pop some sort of outer nested thing */
     if (__pop_vifstk()) goto rd_again;
     /* next try to replace just finished 0th element with new input file */
     if (__cur_infi + 1 > __last_inf) goto eof_error;
     __cur_infi++; 
     if (!__open_sfil()) goto eof_error;
     /* know first token of file flag now on */
     __file_just_op = FALSE;
     __first_num_eol = TRUE; 
     goto rd_again;

eof_error:
     __pv_ferr(2657,
      "while processing foreign `language section **EOF** read before `endlanguage");
     if (first_time)
      {
       if (__langstr != NULL) strcpy(__langstr, "");
       __doing_langdir = FALSE;
       return;
      }

     strcpy(__langstr, "`endlanguage");
     __langstr[12] = '\n';
     __langstr[13] = '\0';
     done = TRUE;
     goto try_callback;
    }
   if (first_time)
    {
     char s1[IDLEN];

     if (strlen(__langstr) + 11 >= IDLEN)
      {
       __pv_ferr(2679,
        "`language section line too long (%d) - truncated", IDLEN - 1);
      }
     strcpy(s1, __langstr);
     strcpy(__langstr, "`language");
     if (s1[0] != ' ' && s1[0] != '\t') strcat(__langstr, " ");
     strcat(__langstr, s1);
     first_time = FALSE;
    }
   else
    {
     if ((chp = __match_cdir(__langstr, "`endlanguage")) != NULL) 
      done = TRUE;
     else if ((chp = __match_cdir(__langstr, "`include")) != NULL) 
      {
       chp2 = &(chp[8]);  
       while (vis_white_(*chp2)) chp2++;
       strcpy(__macwrkstr, chp2);
       /* correct for right line because line getting moves to next line */
       __do_include();
       __lin_cnt++;
       __total_rd_lines++;
       /* no need to set beginning of lne because new file just opened */
       goto rd_again;
      } 
     else chp = __langstr;
    }

try_callback:
   /* execute call back if any registered - if none just returns */
   /* by passing chp, know for `langauge/`endlanguage no leading whie space */
   __exec_vpi_langlinecbs(chp, __in_fils[__cur_fnam_ind], __lin_cnt);
   __lin_cnt++;
   __total_rd_lines++;
   /* this set first token on line using 2 step flag needed for push back */
   __first_num_eol = TRUE;
   if (done)
    {
     strcpy(__langstr, "");
     __doing_langdir = FALSE;
     return;
    }
  }
}

/*
 * routine to skip over lines to keyword (usually `endlanguage)
 *
 * special routine that can not tokenize since probably non Verilog
 * returns last character read on success else EOF on error of EOF
 *
 * this must read and expand `include files because `endlanguage can be
 * in included file
 */
extern int32 __notokenize_skiplines(char *match_prefix)
{
 if (__langstr == NULL) __langstr = __my_malloc(IDLEN + 1);
 for (;;)
  {
   if (__my_getlin(__langstr) == EOF)
    {
     /* first try to pop some sort of outer nested thing */
     /* this sets line number to right outer line and no line here */
     if (__pop_vifstk()) continue;

     /* next try to replace just finished 0th element with new input file */
     if (__cur_infi + 1 > __last_inf) goto eof_error;
     __cur_infi++; 
     if (!__open_sfil()) goto eof_error;
     /* know first token of file flag now on */
     __file_just_op = FALSE;
     /* this set first token on line using 2 step flag needed for push back */
     __first_num_eol = TRUE; 
     continue;

eof_error:
     __pv_ferr(2657,
      "while processing foreign `language section **EOF** read before `endlanguage");
     return(TEOF);
    }

   if (__match_cdir(__langstr, match_prefix) != NULL)
    {
     __lin_cnt++;
     __total_rd_lines++;
     /* this set first token on line using 2 step flag needed for push back */
     __first_num_eol = TRUE;
     break;
    }
   /* becausing section ifdefed out, just ignore include */
   __lin_cnt++;
   __total_rd_lines++;
   /* this set first token on line using 2 step flag needed for push back */
   __first_num_eol = TRUE;
  }
 return(UNDEF);
}

/*
 * match a directive prefix (may be leading white space) 
 * returns char ptr to first character of matched if matched
 * else returns nil if no match
 */
extern char *__match_cdir(char *lp, char *match_prefix)
{
 register char *chp;
 int32 slen;

 /* possible lang str not yet allocated */ 
 if (lp == NULL) return(NULL);
 slen = strlen(match_prefix);
 for (chp = __langstr;; chp++) { if (!vis_nonnl_white_(*chp)) break; }
 if (strncmp(chp, match_prefix, slen) == 0) return(chp); 
 return(NULL);
}

/*
 * execute vpi_control vpiInsertSource operation
 *
 * know in `endlanguage line callback or will not get here 
 *
 * BEWARE - this works because no longjmp in source reading
 */
extern int32 __exec_rdinserted_src(char *buf)
{
 register int32 vi;
 int32 sav_ecnt, retv, sav_vin_top, sav_lincnt, sav_cur_fnamind, len;
 struct vinstk_t **sav_vinstk;

 /* save lin_cnt to restore after buffer parsed */ 
 sav_lincnt = __lin_cnt; 
 sav_cur_fnamind = __cur_fnam_ind;

 sav_ecnt = __pv_err_cnt;
 /* save the nested open file stack */
 sav_vinstk = (struct vinstk_t **)
  __my_malloc((__vin_top + 1)*sizeof(struct vinstk_t *));
 /* this moves the ptrs to be pointed to by same */
 for (vi = 0; vi <= __vin_top; vi++)
  {
   sav_vinstk[vi] = __vinstk[vi]; 
   __vinstk[vi] = NULL;
  }
 sav_vin_top = __vin_top;
 __vin_top = -1;

 /* push string on top (only one now on) of read stack */
 __push_vinfil();
 __visp = __vinstk[__vin_top];
 __visp->vichp = __visp->vichp_beg = buf;
 len = strlen(buf);
 __visp->vichplen = len;
 __in_s = NULL;

 for (;;)
  {
   __get_vtok();
   if (__toktyp == TEOF) break;
   __rding_top_level = FALSE;
   __rd_ver_mod();
   __rding_top_level = TRUE;
   if (__toktyp == TEOF) break;
  }
 /* restore the nested open file stack */
 /* first free any allocated vin stk records from includes */
 for (vi = 0; vi < MAXFILNEST; vi++)
  {
   if (__vinstk[vi] != NULL)
    {
     __my_free((char *) __vinstk[vi], sizeof(struct vinstk_t));
     __vinstk[vi] = NULL;
    }
  }
 /* next copy back and restore */
 for (vi = 0; vi <= sav_vin_top; vi++) __vinstk[vi] = sav_vinstk[vi];
 __lin_cnt = sav_lincnt;
 __cur_fnam_ind = sav_cur_fnamind;
 __vin_top = sav_vin_top;
 __visp = __vinstk[__vin_top];
 __in_s = __visp->vi_s;

 /* LOOKATME - why is this needed */
 __toktyp = UNDEF;
 __lasttoktyp = UNDEF;

 if (__pv_err_cnt > sav_ecnt) retv = FALSE; else retv = TRUE;
 return(retv);
}

/*
 * VERILOG 2001 ATTRIBUTE READING ROUTINES
 */

/*
 * read, parse and build attribute list from attribute string
 * builds list and returns header of list or nil on error
 *
 * new verilog 2000 feature
 * know string between (* and *) stored on entry in attr name field
 * trick is to push string onto file stack as if it is no arg macro 
 *
 * expression value converted to constant number here because
 * attributes need to be used by tools that do not know pound param vals
 * i.e. can be fed, post generate source
 */
extern struct attr_t *__rd_parse_attribute(struct attr_t *rd_attrp)
{
 register char *chp; 
 int32 sav_ecnt, sav_tot_lines, sav_fnam_ind, attllen;
 struct attr_t *attrp, *attr_hd, *last_attrp;
 char *attlin, attnam[IDLEN];

 attrp = attr_hd = last_attrp = NULL;
 attlin = rd_attrp->attrnam;
 /* SJM 07/30/01 - need to read chars and parse out of global */
 /* needed so can free work attrnam after rec built */ 
 if ((attllen = strlen(attlin)) >= __attrparsestrlen - 1)
  {
   __attrparsestr = __my_realloc((char *) __attrparsestr, __attrparsestrlen,
    attllen + 1);
   __attrparsestrlen = attllen + 1;
  }
 strcpy(__attrparsestr, attlin); 

 /* need to save total lines read since counted in attr when collected */
 /* parsing here counts lines because new lines not escaped */
 sav_tot_lines = __total_rd_lines;
 sav_fnam_ind = __cur_fnam_ind;
 sav_ecnt = __pv_err_cnt;

 /* if currently reading file, must preserve line count */
 if (__visp->vi_s != NULL) __visp->vilin_cnt = __lin_cnt;
 /* push string on top of read stack */
 __push_vinfil();
 __visp->vichp = __visp->vichp_beg = __attrparsestr;

 /* make sure not freeing line even if somehow hit eof - never should */
 __visp->vichplen = -1;
 __in_s = NULL;
 /* DBG remove --- */
 if (__debug_flg) __dbg_msg("parsing attribute string %s\n", attlin);
 /* --- */

 __cur_fnam_ind = rd_attrp->attr_fnind;
 __lin_cnt = rd_attrp->attrlin_cnt;

 __get_vtok();
 /* ; added to end of saved attribute string if not there */
 if (__toktyp == SEMI) 
  {
   __pv_ferr(3405,
    "attribute_instance illegal - at least one attr_spec required");
chk_eol: 
   for (chp = __visp->vichp; *chp != '\0'; chp++)
    {
     if (!vis_white_(*chp))
      {
       __pv_ferr(3407,
        "attribute_instance comma separator expected - semicolon read");
       /* on error always skip to end of string - need EOF next read */
       while (*chp != '\0') chp++;
       goto done;
      }
    }
   goto done;
  }
 for (;;)
  {
   if (__toktyp != ID)
    {
     __pv_ferr(3404, "attribute name expected - %s read", __prt_vtok());
err_skip_eol:
     /* on error always skip to end of string - need EOF next read */
     for (chp = __visp->vichp; *chp != '\0'; chp++) ;
     goto done;
    }
   strcpy(attnam, __token);
   __get_vtok();
   __root_ndp = NULL; 
   if (__toktyp == EQ)
    {
     __get_vtok();
     /* LOOKATME - should try to resync on errors */
     __last_xtk = -1;
     /* on success (T), this reads either , or ; */
     if (!__col_comsemi(-1)) goto err_skip_eol;
     __bld_xtree(0);
     if (__expr_has_glb(__root_ndp) || !__src_rd_chk_paramexpr(__root_ndp, 0))
      {
       __pv_ferr(3404,
        "attr_spec for attribute %s expression error - defined parameters and constants only", 
        attnam); 
       /* need to still add value of x to prevent further errors */
       __free2_xtree(__root_ndp);
       __root_ndp->szu.xclen = WBITS;
       /* default value is on 1 (non zero) */
       __set_numval(__root_ndp, 1, 0, WBITS);  
      }
     else
      {
       /* because of previous check, this can not fail */
       __eval_param_rhs_tonum(__root_ndp);
      }
    }
   else __root_ndp = NULL;

   /* allocate in link in attribute */
   attrp = (struct attr_t *) __my_malloc(sizeof(struct attr_t));
   attrp->attr_tok = rd_attrp->attr_tok;
   attrp->attrnam = __pv_stralloc(attnam); 
   /* must eval. after all param setting is done */
   attrp->attr_xp = __root_ndp; 
   /* LOOKATME - think should just use attr inst loc */
   attrp->attr_fnind = __cur_fnam_ind;
   attrp->attrlin_cnt = __lin_cnt;
   attrp->attrnxt = NULL;
   if (last_attrp == NULL) attr_hd = attrp; else last_attrp->attrnxt = attrp;
   last_attrp = attrp;

   if (__toktyp == SEMI) goto chk_eol;
   if (__toktyp != COMMA)
    {
     __pv_ferr(3406, "attr_spec separator or end \"*)\" expected - %s read",
      __prt_vtok());
     goto err_skip_eol;
    }
   __get_vtok();
   continue;
  }

done:
 /* caller must free attribute string when pased for all instances */

 /* restore total lines read */
 __total_rd_lines = sav_tot_lines;
 __cur_fnam_ind = sav_fnam_ind;
 /* SJM 07/30/01 - was using visp but that was not set or index */
 __cur_fnam = __in_fils[__cur_fnam_ind];
 /* small memory leak if syntax error */
 if (__pv_err_cnt > sav_ecnt) return(NULL);
 /* emit warnings if attr duplicated with different value - inform if same */
 if (attr_hd != NULL) attr_hd = chk_dup_attrs(attr_hd);
 return(attr_hd);
}

/*
 * check attribute list for duplicates
 * if duplicate remove - if different value warn if same value inform
 * 
 * LOOKATME - if lots of attributes need to sort and match
 */
static struct attr_t *chk_dup_attrs(struct attr_t *attr_hd)
{
 register struct attr_t *attrp1, *attrp2, *last_attrp1;
 struct attr_t *new_attrhd, *attrp3;
 char s1[RECLEN], s2[RECLEN]; 

 new_attrhd = attr_hd;
 last_attrp1 = NULL;
 for (attrp1 = attr_hd; attrp1 != NULL;)
  {
   for (attrp2 = attrp1->attrnxt; attrp2 != NULL; attrp2 = attrp2->attrnxt)
    {
     if (strcmp(attrp1->attrnam, attrp2->attrnam) == 0) 
      {
       /* know both numbers but still use xpr cmp */ 
       if (__cmp_xpr(attrp1->attr_xp, attrp2->attr_xp) == 0)
        {
         __gfinform(3001, attrp2->attr_fnind, attrp2->attrlin_cnt,
          "attribute %s duplicated with same value (first at %s) - first discared",
         attrp1->attrnam, __bld_lineloc(s1, attrp1->attr_fnind,
         attrp1->attrlin_cnt));  
        }
       else
        {
         __gfwarn(3101, attrp2->attr_fnind, attrp2->attrlin_cnt,
          "attribute %s value %s duplicated with different values - first at %s value %s discarded",
         attrp1->attrnam, __msgexpr_tostr(s1, attrp2->attr_xp),
         __bld_lineloc(__xs, attrp1->attr_fnind, attrp1->attrlin_cnt), 
         __msgexpr_tostr(s2, attrp1->attr_xp));
        }
       /* SJM 10/16/00 - must set next before freeing and splicing */
       attrp3 = attrp1->attrnxt;

       /* splice out first - if more duplicates will catch later */
       if (last_attrp1 == NULL) new_attrhd = attrp1->attrnxt; 
       else last_attrp1->attrnxt = attrp1->attrnxt;
       __free_xtree(attrp1->attr_xp);

       __my_free((char *) attrp1, sizeof(struct attr_t));
       /* must not advance last attr */
       attrp1 = attrp3;
       goto chk_nxt_attr;
      }
    }
   attrp1 = attrp1->attrnxt;
   last_attrp1 = attrp1;     
chk_nxt_attr:;
  }
 return(new_attrhd);
}

/*
 * ROUTINES TO READ CFG LIB.MAP INPUT FILE LIST
 */

/*
 * read a cfg file - returns F on error else T
 * reads both library mapping file and the config blocks
 *
 * may have list of config map library files (if none given using map.lib)
 *
 * if passed the command line, insrc = FALSE, and mapfile is the file name
 * otherwise TRUE, NULL if in source
 *
 * SJM 11/29/03 - contrary to LRM but following NC, cfg can't appear in src
 * but allowing list of lib.map files
 */
extern int32 __rd_cfg(void)
{
 int32 i, sav_ecnt, sav_lin_cnt;
 FILE *fp;
 struct mapfiles_t *mapfp;
 char *sav_cur_fnam;

 /* DBG remove -- */
 if (__map_files_hd == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 /* initialize the instance clause rule binding XMR path component glb tab */
 __siz_bind_comps = 50;
 __bind_inam_comptab = (char **) __my_malloc(__siz_bind_comps*sizeof(char *)); 
 __last_bind_comp_ndx = -1;
 for (i = 0; i < __siz_bind_comps; i++) __bind_inam_comptab[i] = NULL;

 /* SJM 01/15/04 - reading cfg does not use in fils buts must save as cntxt */
 sav_lin_cnt = __lin_cnt;
 sav_cur_fnam = __cur_fnam;

 sav_ecnt = __pv_err_cnt; 
 for (mapfp = __map_files_hd; mapfp != NULL; mapfp = mapfp->mapfnxt) 
  {
   /* must set cur file and line count for error messages */
   __cur_fnam = __pv_stralloc(mapfp->mapfnam);
   __lin_cnt = 1;
   if ((fp = __tilde_fopen(__cur_fnam, "r")) == NULL)
    {
     __pv_err(3500, "cannot open config map library file %s - skipped",
      __cur_fnam);
     continue;
    }
   if (feof(fp))
    {
     __pv_warn(3121, "config map library file %s empty", __cur_fnam);
     continue; 
    }
   rd1_cfg_file(fp);
  }
 /* and then put back */
 __lin_cnt = sav_lin_cnt;
 __cur_fnam = sav_cur_fnam;
 
 if (__pv_err_cnt != sav_ecnt) return(FALSE);
 return(TRUE);
}

/*
 * read contents of one config file
 */
static void rd1_cfg_file(FILE *fp)
{ 
 register int32 ttyp;
 int32 ttyp2, sav_lin_cnt;
 FILE *incfp ;
 char *sav_cur_fnam;

 for (;;)
  {
   ttyp = __get_cfgtok(fp);
   if (ttyp == CFG_INCLUDE)
    {
     if ((ttyp2 = __get_cfgtok(fp)) != CFG_ID)
      {
       __pv_ferr(3501,
        "config map library include statement non wildcard file path name expected - %s read",
        __to_cfgtoknam(__xs, ttyp2));
       if (cfg_skipto_semi(ttyp2, fp) == CFG_EOF) return;  
       continue;
      }
     if ((incfp = __tilde_fopen(__token, "r")) == NULL)
      {
       __pv_ferr(3502,
        "cannot open config map library include file %s - skipped",
        __token);
       if (cfg_skipto_semi(ttyp2, fp) == CFG_EOF) return;  
       continue;
      }
     if (feof(incfp))
      {
       __pv_warn(3121, "config map library file %s empty", __token);
       goto skipto_semi;
      }

     /* SJM 01/15/04 - save ptr and malloc name for later err msgs */
     sav_lin_cnt = __lin_cnt;
     sav_cur_fnam = __cur_fnam;
     __cur_fnam = __pv_stralloc(__token);
     __lin_cnt = 1;

     rd1_cfg_file(incfp); 

     __cur_fnam = sav_cur_fnam;
     __lin_cnt = sav_lin_cnt;

skipto_semi:
     ttyp = __get_cfgtok(fp);
     if (ttyp != CFG_SEMI)
      {
       if (cfg_skipto_semi(ttyp2, fp) == CFG_EOF) return;  
      }
     continue;
    }
   if (ttyp == CFG_LIBRARY)
    {
     rd_cfg_library(fp);
     continue;
    }
   if (ttyp == CFG_CFG) 
    {
     rd_cfg_cfg(fp);
     continue;
    }
   if (ttyp == CFG_EOF) return;
  }
 /* -- DBG remove ---
 dump_mod_info();
 --- */ 
}

/*
 * read a library map file library list
 * expects library keyword to have been read and reads ending ; (or CFG_EOF)
 *
 * if no libraries specified and unresolved references after reading
 * source files (either from config or from list of source files)
 * elaboration will fail with unresolved lib refs
 */
static void rd_cfg_library(FILE *fp)
{
 int32 ttyp;
 struct cfglib_t *lbp;
 struct libel_t *lbep;

 /* get the library name */
 if ((ttyp =__get_cfgtok(fp)) != CFG_ID)
  {
   __pv_ferr(3503,
    "library map file library description library name expected - %s read", 
    __to_cfgtoknam(__xs, ttyp));
   cfg_skipto_semi(ttyp, fp);
   return;
  }
 lbp = (struct cfglib_t *) __my_malloc(sizeof(struct cfglib_t));
 init_cfglib(lbp);
 /* needed for expand error messages */
 lbp->cfglb_fnam = __cur_fnam;
 lbp->cfglb_lno = __lin_cnt;
 lbp->sym_added = FALSE;

 if (!chk_libid(__token))
  { 
   __pv_ferr(3504, "library name %s illegal simple Verilog identifier",
    __token);
  }
 lbp->lbname = __pv_stralloc(__token);
 lbep = rd_cfg_fspec_list(fp, FALSE);
 lbp->lbels = lbep; 
 if (__cfglib_tail == NULL) __cfglib_hd = __cfglib_tail = lbp; 
 else
  {
   __cfglib_tail->lbnxt = lbp;
   __cfglib_tail = lbp;
  }
}

/*
 * read a list of library file spec (wildcard) paths build and return list
 * reads first element and reads ending EOF (for --incdir) or semi 
 *
 * SJM 12/11/03 - notice old config dir code was wrong - comma separated list
 *
 * LOOKATME - maybe should return nil on error
 */
static struct libel_t *rd_cfg_fspec_list(FILE *fp, int32 in_incdir)
{
 int32 ttyp, ttyp2, sav_lin_cnt;
 struct libel_t *lbep, *lbep2, *lbep_hd, *last_lbep;
 FILE *incfp;
 char *sav_cur_fnam;

 for (lbep_hd = last_lbep = NULL;;)
  {
   ttyp = __get_cfgtok(fp);
   if (ttyp == CFG_SEMI || ttyp == CFG_EOF)
    {
     if (in_incdir)
      {
       if (ttyp == CFG_SEMI)
        {
         __pv_ferr(3507,
          "config library description file path spec in -incdir ';' illegal");
         cfg_skipto_eof(ttyp, fp);
        }
       break; 
      }
     if (ttyp == CFG_EOF)
      {
       __pv_ferr(3507,
        "config library description file path spec in -incdir ';' illegal");
         cfg_skipto_eof(ttyp, fp);
       /* even if hit wrong early EOF return list build so far */
      }
     /* know correct sem read or error emitted */
     break;
    }

   if (ttyp != CFG_ID)
    {
     __pv_ferr(3506,
      "config library description file path spec expected - %s read",
      __to_cfgtoknam(__xs, ttyp)); 

     if ((ttyp2 = cfg_skipto_comma_semi(ttyp, fp)) == CFG_COMMA) continue;
     if (in_incdir && ttyp2 == CFG_SEMI)
      {
       __pv_ferr(3507,
        "config library description file path spec in -incdir ';' illegal");
       cfg_skipto_eof(ttyp, fp);
      }
     return(NULL);
    }
   
   /* -incdir [name of file contains comma separated lib list] */
   /* can be nested */
   /* case 1: -incdir file containg comma separated list but end with EOF */ 
   if (strcmp(__token, "-incdir") == 0) 
    {
     /* read the config list from a file (comma separated) */
     if ((ttyp2 = __get_cfgtok(fp)) != CFG_ID)
      {
       __pv_ferr(3501,
        "config library description -incdir non wildcard file path name expected - %s read",
        __to_cfgtoknam(__xs, ttyp2));
inc_err_skip:
       if (cfg_skipto_comma_semi(ttyp, fp) == CFG_COMMA) continue;
       return(NULL);
      }
     if ((incfp = __tilde_fopen(__token, "r")) == NULL)
      {
       __pv_ferr(3502,
        "cannot open config library description -incdir file %s - skipped",
        __token);
       goto inc_err_skip; 
      }
     if (feof(incfp))
      {
       __pv_fwarn(3121,
        "config library description -incdir file %s empty", __token);
       goto inc_err_skip;
      }

     /* SJM 01/15/04 - save ptr and malloc name for later err msgs */
     sav_cur_fnam = __cur_fnam;
     sav_lin_cnt = __lin_cnt;
     __cur_fnam = __pv_stralloc(__token);
     __lin_cnt = 1;

     lbep = rd_cfg_fspec_list(incfp, TRUE);
     if (lbep != NULL) 
      {
       /* link onto end and update last to end of new add (maybe long) list */
       if (last_lbep == NULL) lbep_hd = lbep;
       else last_lbep->lbenxt = lbep;

       /* SJM 12/08/03 - think this is wrong ??? - need a last */
       for (lbep2 = lbep; lbep2->lbenxt != NULL; lbep2 = lbep2->lbenxt) ; 
       last_lbep = lbep2;
      } 
     __my_fclose(incfp); 

     __cur_fnam = sav_cur_fnam;
     __lin_cnt = sav_lin_cnt;
    }
   /* case 2: file spec - only other possibility */
   lbep = (struct libel_t *) __my_malloc(sizeof(struct libel_t));
   lbep->lbelsrc_rd = FALSE;
   lbep->lbefnam = __pv_stralloc(__token);
   lbep->lbcelndx = NULL;
   lbep->lbel_sytab = NULL;
   lbep->lbenxt = NULL;
   lbep->expanded = FALSE;

   if (last_lbep == NULL) lbep_hd = lbep; else last_lbep->lbenxt = lbep;
   last_lbep = lbep;
  }
 return(lbep_hd);
}

/*
 * read map library config block 
 * know config keyword read and reads the endconfig keyword 
 *
 * idea is that the library lists are separate from the config blocks
 */
static void rd_cfg_cfg(FILE *fp)
{
 int32 ttyp, len, cfg_beg_lno, nbytes, expl_config;
 struct cfgdes_t *desp, *des_hd, *des_tail;
 struct cfg_t *cfgp;
 struct cfgrule_t *rulp, *rule_beg, *rule_end;
 struct cfgnamlst_t *lblp;
 char objnam[IDLEN], libnam[IDLEN], celnam[IDLEN];
 char cfgnam[IDLEN], s1[IDLEN], s2[IDLEN];

 cfg_beg_lno = __lin_cnt;
 /* get the config name */
 if ((ttyp =__get_cfgtok(fp)) != CFG_ID)
  {
   __pv_ferr(3503, "config declaration config name expected - %s read", 
    __to_cfgtoknam(__xs, ttyp));
   if (cfg_skipto_semi(ttyp, fp) == CFG_EOF) return;
   strcpy(cfgnam, "**none**");
  }
 else
  { 
   strcpy(cfgnam, __token);
   ttyp = __get_cfgtok(fp);
  }
 if (ttyp != CFG_SEMI)
  {
   __pv_ferr(3531,
    "config declaration config name not followed by semicolon - %s read",
    __to_cfgtoknam(__xs, ttyp));
   if (cfg_skipto_semi(ttyp, fp) == CFG_EOF) return;
  }
 /* config names may need to match cell type names so can be escaped */
 if (!chk_libid(cfgnam))
  {
   if (chk_escid(cfgnam))
    {
     /* remove the leading escaping back slash and ending ' ' */
     strcpy(s1, &(cfgnam[1]));
     len = strlen(s1);
     s1[len - 1] = '\0';
     strcpy(cfgnam, s1);
    }
   else
    {
     __pv_ferr(3534,
      "illegal config name %s - must be legal Verlog identifier", cfgnam);
    }
  }

 cfgp = (struct cfg_t *) __my_malloc(sizeof(struct cfg_t));
 init_cfg(cfgp);
 cfgp->cfgnam = __pv_stralloc(cfgnam);
 /* config location info for tracing and error msgs */
 cfgp->cfg_fnam = __pv_stralloc(__cur_fnam); 
 cfgp->cfg_lno = cfg_beg_lno;

 ttyp = __get_cfgtok(fp);
 /* config design statement must come first if used */
 if (ttyp == CFG_DESIGN)
  {
   /* SJM 12/08/03 - as I read LRM, every top module needs separate config */ 
   /* therefore only one design mod allowed - FIXME- text of LRM contradicts */

   des_hd = des_tail = NULL;
   for (;;)
    {
     ttyp =__get_cfgtok(fp);
     if (ttyp == CFG_SEMI)
      {
       if (des_hd == NULL)
        {
         __pv_ferr(3532,
          "config design statement requires at least one [lib name].[cell name]");
        }
       break;
      }
     /* get the design specifier [library].[mod name] */
     if (ttyp != CFG_ID)
      {
       __pv_ferr(3533,
        "config design statement design specifier expected - %s read", 
        __to_cfgtoknam(__xs, ttyp));
       if (cfg_skipto_semi(ttyp, fp) == CFG_EOF) return;
       /* here just leave objnam nil for none */
      }
     else
      { 
       /* library name required design cell name */ 
       /* notice cell nam can be escaped and if so escapes removed */
       if (!extract_design_nam(libnam, celnam, __token))
        {
         __pv_ferr(3535,
          "config design statement [library identifier].[cell identifier] expected - %s illegal",
          __to_cfgtoknam(__xs, ttyp));
         /* skip on error */
         continue;
        }
      desp = (struct cfgdes_t *) __my_malloc(sizeof(struct cfgdes_t)); 
      desp->deslbnam = __pv_stralloc(libnam);
      desp->deslbp = NULL;
      desp->topmodnam = __pv_stralloc(celnam);
      desp->desnxt = NULL;

      if (des_hd == NULL) des_hd = des_tail = desp;
      else
       {
        des_tail->desnxt = desp;
        des_tail = desp;
       }
     }
    }
   cfgp->cfgdeslist = des_hd;

   /* know ';' read to get here */
   ttyp =__get_cfgtok(fp);
  }
 rule_beg = rule_end = NULL;
 for (;;)
  {
   /* liblist or use clauses never start a config rule */
   switch(ttyp) { 
    case CFG_DEFAULT:
     if (cfgp->cfgdflt != NULL)
      {
       __pv_ferr(3538, "config %s default clause repeated - new one replaces",
       cfgp->cfgnam);
      }
   
     /* format: default liblist [space sep list of library names]; */ 
     /* may return nil */
     if ((ttyp = __get_cfgtok(fp)) != CFG_LIBLIST) 
      {
       __pv_ferr(3537,
        "config declaration default clause not followed by liblist keyword - %s read",
        __to_cfgtoknam(__xs, ttyp));
       if (cfg_skipto_semi_endconfig(ttyp, fp) != CFG_SEMI)
        return;
       continue;
      }
     if ((lblp = rd_liblist(fp)) != NULL)
      {
       if (cfgp->cfgdflt != NULL) 
        {
         /* SJM 12/19/03 - LOOKATME - what if repated */
         __pv_ferr(3539, "config declaration default clause repeated - 2nd ignord"); 
        }
      }
     else
      {
       __pv_fwarn(3127, "config declaration default clause liblist empty");
      }
     rulp = (struct cfgrule_t *) __my_malloc(sizeof(struct cfgrule_t));
     init_rule(rulp);
     rulp->rul_libs = lblp;
     rulp->rultyp = CFG_DEFAULT;
     cfgp->cfgdflt = rulp;
     break;
    case CFG_ENDCFG:
     /* no semi after end config */
     goto endcfg_read;
    case CFG_INSTANCE:
     /* format: instance [inst name] liblist [space sep lib name list]; */ 
     /* format: instance [inst name] use [qualified mod type name]; */
 
     /* instance name can be XMR but take apart later */
     if ((ttyp = __get_cfgtok(fp)) != CFG_ID) 
      {
       __pv_ferr(3540,
        "config instance clause instance named expected - %s read",
          __to_cfgtoknam(__xs, ttyp));
        if (cfg_skipto_semi_endconfig(ttyp, fp) != CFG_SEMI)
         return;
       continue;
      }
     /* save the instance name */
     strcpy(objnam, __token);
     /* check config XMR path and build malloced cfg nam list of components */
     /* if return F, table not built */
     if (!bld_inst_xmr_comptab(__token))
      {
       __pv_ferr(3540,
        "config instance clause instance name %s illegal Verilog hierarchical name",
       __token);
       continue;
      }  

     lblp = NULL;
     if ((ttyp = __get_cfgtok(fp)) == CFG_LIBLIST) 
      {
       /* case 1: liblist form - always reads ending ; */
       if ((lblp = rd_liblist(fp)) == NULL)
        {
         __pv_fwarn(3129, "config instance clause liblist empty - ignored");
         goto rd_nxt_tok;
        }
       rulp = (struct cfgrule_t *) __my_malloc(sizeof(struct cfgrule_t));
       init_rule(rulp);
       rulp->rultyp = CFG_INSTANCE;
       /* just set the instance name and lib names are in lblp */
       rulp->objnam = __pv_stralloc(objnam);
       rulp->rul_libs = lblp;
      }
     else if (ttyp == CFG_USE) 
      {
       if (rd_use_clause(fp, s1, s2, &expl_config) == CFG_ENDCFG)
        return;
       /* on error s1 not set */
       if (strcmp(s1, "") == 0 && strcmp(s2, "") == 0) goto rd_nxt_tok;

       rulp = (struct cfgrule_t *) __my_malloc(sizeof(struct cfgrule_t));
       init_rule(rulp);
       rulp->rultyp = CFG_INSTANCE;
       /* instance objnam use s1.s2 */
       rulp->rul_use_libnam = __pv_stralloc(s1);
       rulp->rul_use_celnam = __pv_stralloc(s2);
       rulp->objnam = __pv_stralloc(objnam);
       rulp->use_rule_cfg = expl_config;
       rulp->is_use = TRUE; 
      }
     else
      {
       __pv_ferr(3548,
        "config inst clause not followed by liblist or use clause - %s read",
        __to_cfgtoknam(__xs, ttyp));
       if (cfg_skipto_semi(ttyp, fp) == CFG_EOF) return;
       goto rd_nxt_tok;
      }

     /* SJM 01/14/03 - can't bld the inst XMR components tab until here */
     nbytes = (__last_bind_comp_ndx + 1)*sizeof(char *);
     rulp->inam_comptab = (char **) __my_malloc(nbytes);
     memcpy(rulp->inam_comptab, __bind_inam_comptab, nbytes);
     rulp->inam_comp_lasti = __last_bind_comp_ndx;
     /* head of instance name must match config name a design cell name */
     for (desp = cfgp->cfgdeslist; desp != NULL; desp = desp->desnxt)
      {
       if (strcmp(desp->topmodnam, rulp->inam_comptab[0]) == 0)
        goto fnd_match;
      }
     __pv_ferr(3541,
      "config instance clause hierachical path %s head does not match any design statement top level module name",
      s1);

fnd_match: 
     /* add to end of list since must search in order of appearance */
     if (rule_beg == NULL) cfgp->cfgrules = rule_beg = rule_end = rulp;
     else
      {
       rule_end->rulnxt = rulp;
       rule_end = rulp;
      }
     
     break;
    case CFG_CELL:
     /* format: cell [<lib:>cell] liblist [space sep lib name list]; */ 
     /* format: cell [<lib>:cell] use [qualified mod type name]; */
     if ((ttyp = __get_cfgtok(fp)) != CFG_ID) 
      {
       __pv_ferr(3551,
        "config cell clause [library].cell name expected - %s read",
        __to_cfgtoknam(__xs, ttyp));
       if (cfg_skipto_semi_endconfig(ttyp, fp) != CFG_SEMI) return;
       continue;
      }
     if (!extract_libcell_nam(libnam, objnam, __token))
      {
       goto rd_nxt_tok;
      }
     lblp = NULL;
     if ((ttyp = __get_cfgtok(fp)) == CFG_LIBLIST) 
      {
       /* AIV - LRM (pg 217) states lib.cell with liblist is an error */
       /* if stmt will work because libnam init in extract_libcell  */
       if (libnam[0] != '\0') 
       {
         __pv_ferr(3552,
          "config cell clause library.cell (%s.%s) cannot be used with 'liblist' clause", libnam, objnam);
        if (cfg_skipto_semi_endconfig(ttyp, fp) != CFG_SEMI) return;
        goto rd_nxt_tok;
       }

       /* case 1: liblist form - always reads ending ; */
       if ((lblp = rd_liblist(fp)) == NULL)
        {
         __pv_fwarn(3131, "config cell clause liblist empty - ignored");
         goto rd_nxt_tok;
        }
       rulp = (struct cfgrule_t *) __my_malloc(sizeof(struct cfgrule_t));
       init_rule(rulp);
       rulp->rultyp = CFG_CELL;
       /* libnam optional    libnam.objnam */
       rulp->libnam = __pv_stralloc(libnam);
       rulp->objnam = __pv_stralloc(objnam);
       rulp->rul_libs = lblp;
      }
     else if (ttyp == CFG_USE) 
      {
       if (rd_use_clause(fp, s1, s2, &expl_config) == CFG_ENDCFG) return;
       /* on error s1 not set */
       if (strcmp(s1, "") == 0 && strcmp(s2, "") == 0) goto rd_nxt_tok;

       rulp = (struct cfgrule_t *) __my_malloc(sizeof(struct cfgrule_t));
       init_rule(rulp);
       /* cell objnam use s1.s2 */
       rulp->rul_use_libnam = __pv_stralloc(s1);
       rulp->rul_use_celnam = __pv_stralloc(s2);
       rulp->use_rule_cfg = expl_config;
       /* objnam is the cell type to match */
       rulp->objnam = __pv_stralloc(objnam);
       rulp->libnam = __pv_stralloc(libnam);
       rulp->rultyp = CFG_CELL;
       rulp->is_use = TRUE; 
      }
     else
      {
       __pv_ferr(3559,
        "config cell clause not followed by liblist or use keywords - %s read",
        __to_cfgtoknam(__xs, ttyp));
       if (cfg_skipto_semi(ttyp, fp) == CFG_EOF) return;
       goto rd_nxt_tok;
      }

     /* add to end of list since must search in order of appearance */
     if (rule_beg == NULL) cfgp->cfgrules = rule_beg = rule_end = rulp;
     else
      {
       rule_end->rulnxt = rulp;
       rule_end = rulp;
      }
     break;
    default:
     __pv_ferr(3561,
      "config declaration rule statement or endconfig expected - %s read",
      __to_cfgtoknam(__xs, ttyp));
     if (cfg_skipto_semi(ttyp, fp) == CFG_EOF) return;
   }
rd_nxt_tok:
   ttyp =__get_cfgtok(fp);
  }
endcfg_read:
   /* AIV add the config to the list */
   if (__cfg_hd == NULL) __cfg_hd = __cur_cfg = cfgp;
   else { __cur_cfg->cfgnxt = cfgp;  __cur_cfg = cfgp; }
 return;
}

/*
 * initialize a rule record
 */
static void init_rule(struct cfgrule_t *rulp)
{
 rulp->rultyp = CFG_UNKNOWN;
 rulp->use_rule_cfg = FALSE;
 rulp->objnam = NULL;
 rulp->libnam = NULL;
 rulp->rul_use_libnam = NULL; 
 rulp->rul_use_celnam = NULL; 
 rulp->inam_comptab = NULL;
 rulp->inam_comp_lasti = -1;
 rulp->rul_libs = NULL;
 rulp->rulnxt = NULL;
 /* AIV just set the line to current line  */
 rulp->rul_lno =  __lin_cnt;
 rulp->matched =  FALSE;
 rulp->is_use =  FALSE;
}

/*
 * extract a [lib].[cell] design name - format [lib].[cell] 
 * lib ID lexical pattern is: [let or _]{let | num | $ | _}
 *
 * SJM 12/19/03 - LRM wrong since config can't be in Verilog source lib
 * and cell names both required
 *
 * SJM 01/12/04 - library identifiers (names) can't be escaped
 */
static int32 extract_design_nam(char *libnam, char *celnam, char *desnam)
{
 register char *chp;
 int32 len;
 char s1[IDLEN], s2[IDLEN];

 strcpy(libnam, "");
 strcpy(celnam, "");

 if ((chp = strchr(desnam, '.')) == NULL) return(FALSE);
 strncpy(s1, desnam, chp - desnam);
 s1[chp - desnam] = '\0';
 if (!chk_libid(s1)) return(-1);
 strcpy(libnam, s1);

 chp++;
 strcpy(s2, chp);
 if (!chk_libid(s2))
  {
   if (chk_escid(s2))
    {
     /* remove the leading escaping back slash and ending ' ' */
     strcpy(s1, &(s2[1]));
     len = strlen(s1);
     s1[len - 1] = '\0';
    }
   else
    {
     __pv_ferr(3534,
      "illegal cell name %s - must be legal Verlog identifier", s2);
    }
   strcpy(celnam, s1);
   return(TRUE);
  }
 strcpy(celnam, s2);
 return(TRUE);
}

/*
 * check and return T if library name is legal unescaped ID
 */
static int32 chk_libid(char *lbnam)
{
 register char *chp;

 chp = lbnam;
 if (!isalpha(*chp) && *chp != '_') return(FALSE);
 for (++chp; *chp != '\0'; chp++)
  {
   if (!isalnum(*chp) && *chp!= '$' && *chp != '_') return(FALSE);
  }
 return(TRUE);
}

/*
 * check and return T if path component or cell type name is legal escaped ID
 */
static int32 chk_escid(char *nam)
{
 int32 len;
 char *chp;

 chp = nam;
 if (*chp != '\\') return(FALSE); 
 len = strlen(nam);
 if (nam[len - 1] != ' ') return(FALSE);
 return(TRUE);
}

/*
 * build table of ptrs to strings in global cfg bind table of instance comps
 * return F on error
 *
 * grows global table - caller will copy to malloced memory
 */
static int32 bld_inst_xmr_comptab(char *inam)
{
 register int32 ci;
 register char *chp, *chp2;
 int32 len;
 char s1[IDLEN], s2[IDLEN];

 /* AIV need to reset the global, after it is copied */
 __last_bind_comp_ndx = -1;
 
 for (chp = inam;;)
  {
   if (*chp == '\\')
    {
     if ((chp2 = strchr(chp, ' ')) == NULL)
      {
bad_end:
       for (ci = 0; ci <= __last_bind_comp_ndx; ci++)
        {
         __my_free(__bind_inam_comptab[ci],
          strlen(__bind_inam_comptab[ci]) + 1);
         __bind_inam_comptab[ci] = NULL;
        }
       return(TRUE);
      }

     strncpy(s1, chp, chp2 - chp);
     s1[chp2 - chp] = '\0';
     if (!chk_escid(s1)) goto bad_end;
     strcpy(s2, &(s1[1]));
     s2[chp2 - chp - 2] = '\0';
     chp++;
    }
   else
    {
     if ((chp2 = strchr(chp, '.')) == NULL)
      {
       strcpy(s2, chp);
       len = strlen(chp);
       chp = &(chp[len]);
      }
     else
      {
       /* non escaped and non tail component */
       strncpy(s2, chp, chp2 - chp);
       s2[chp2 - chp] = '\0';
       chp = chp2;
      }
    }
   /* add malloced comp name to table - table copied so do not need to free */ 
   if (++__last_bind_comp_ndx >= __siz_bind_comps) grow_bind_comps();
   __bind_inam_comptab[__last_bind_comp_ndx] = __pv_stralloc(s2); 

   if (*chp == '\0') break;
   if (*chp != '.') goto bad_end;
   chp++;
  }
 return(TRUE);
}

/*
 * routine to grow global bind comp table
 */
static void grow_bind_comps(void)
{
 int32 osize, nsize;
     
 osize = __siz_bind_comps*sizeof(char *);
 /* SJM 01/13/04 - maybe growing too fast */
 __siz_bind_comps *= 2;
 nsize = __siz_bind_comps*sizeof(char *);
 __bind_inam_comptab = (char **) __my_realloc((char *) __bind_inam_comptab,
  osize, nsize);
}


/*
 * extract a <lib>.[cell] name where [lib] optional
 *
 * almost same as extracting design [lib].[cell] but there lib name required
 */
static int32 extract_libcell_nam(char *libnam, char *celnam, char *nam)
{
 register char *chp;
 char s1[IDLEN], s2[IDLEN];

 strcpy(libnam, "");
 strcpy(celnam, "");

 /* case 1: library omitted and escaped cell name */
 if (*nam == '\\')
  {
   strcpy(s2, nam); 

do_cell_tail:
   if ((chp = strchr(nam, ' ')) == NULL) return(FALSE);
   strncpy(celnam, nam, chp - nam);
   celnam[chp - nam] = '\0';
   /* checking esc ID but for now will never fail */
   if (!chk_escid(s2)) return(FALSE); 
   strncpy(s1, &(nam[1]), chp - nam - 2);
   s1[chp - nam - 2] = '\0';
   strcpy(celnam, s1);
   chp++;
   if (*chp != '\0') return(FALSE);
   return(TRUE);
  }
 /* if lib name before '.' present, check and fill */
 if ((chp = strchr(nam, '.')) != NULL)
  { 
   strncpy(s1, nam, chp - nam);
   s1[chp - nam] = '\0';
   if (!chk_libid(s1)) return(FALSE);
   strcpy(libnam, s1);
   chp++;
   strcpy(s1, chp);
  }
 else strcpy(s1, nam);
 
 /* case 3: lib name present and escaped ID */
 if (*s1 == '\\') goto do_cell_tail;
 
 /* case 4: lib name non escaped ID */
 if (!chk_libid(s1)) { strcpy(libnam, ""); return(FALSE); }
 strcpy(celnam, s1);
 return(TRUE);
}

/*
 * read a use clause and ending SEMI
 *
 * know use keyword read and reads ending SEMI unless error where resync 
 * on error return F and set libnam and cell name to empty
 */
static int32 rd_use_clause(FILE *fp, char *libnam, char *celnam,
 int32 *expl_config)
{
 int32 ttyp, ttyp2, has_cfg_suffix;

 strcpy(libnam, "");
 strcpy(celnam, "");

 if ((ttyp = __get_cfgtok(fp)) != CFG_ID)
  {
   __pv_ferr(3542,
    "config use clause not followed by use specifier - %s read",
    __to_cfgtoknam(__xs, ttyp));
   ttyp2 = cfg_skipto_semi_endconfig(ttyp, fp);
   return(ttyp2);
  }
 /* this always sets has cfg suffix */  
 if (!extract_use_nam(libnam, celnam, &has_cfg_suffix, __token))
  {
   __pv_ferr(3546,
    "config use clause %s illegal - [lib].[cell] or [cell]:config allowed - P1364 disallows configs in library source files",
    __token);
  }
 *expl_config = has_cfg_suffix;

 if ((ttyp = __get_cfgtok(fp)) != CFG_SEMI)
  {
   __pv_ferr(3544,
    "config use clause not followed by semicolon - %s read",
    __to_cfgtoknam(__xs, ttyp));
   ttyp = cfg_skipto_semi_endconfig(ttyp, fp);
   return(ttyp);
  }
 return(SEMI);
}

/*
 * extract a use clause cell identifier
 * format: <lib name>[cell type name]<:config>
 * 
 * if lib name is omitted parent (current) cell's lib used
 * if :config used, the use config matching [cell type name] for binding
 */
static int32 extract_use_nam(char *libnam, char *celnam, int32 *has_config,
 char *use_spec)
{
 register char *chp;
 char s1[IDLEN], s2[IDLEN];

 strcpy(libnam, "");
 strcpy(celnam, "");
 *has_config = FALSE;

 /* case 1: library omitted and escaped cell name */
 if (*use_spec == '\\')
  {
   strcpy(s2, use_spec); 

do_cell_tail:
   if ((chp = strchr(use_spec, ' ')) == NULL) return(FALSE);
   strncpy(celnam, use_spec, chp - use_spec);
   celnam[chp - use_spec] = '\0';
   /* checking esc ID but for now will never fail */
   if (!chk_escid(s2)) return(FALSE); 
   strncpy(s1, &(use_spec[1]), chp - use_spec - 2);
   s1[chp - use_spec - 2] = '\0';
   strcpy(celnam, s1);
   chp++;
   if (*chp == ':') 
    {
     if (strcmp(chp, ":config") != 0) return(FALSE);
     /* SJM 05/18/04 - :config hierarchical config indirection indicator */ 
     /* can't be used with library name since library are for Verilog src */
     if (strcmp(libnam, "") != 0) return(FALSE);
     *has_config = TRUE;
    }
   else 
    {
     if (*chp != '\0') return(FALSE);
    }
   return(TRUE);
  }
 /* if lib name before '.' present, check and fill */
 if ((chp = strchr(use_spec, '.')) != NULL)
  { 
   strncpy(s1, use_spec, chp - use_spec);
   s1[chp - use_spec] = '\0';
   if (!chk_libid(s1)) return(FALSE);
   strcpy(libnam, s1);
   chp++;
   strcpy(s1, chp);
  }
 else strcpy(s1, use_spec);
 
 /* case 3: lib name present and escaped ID */
 if (*s1 == '\\') goto do_cell_tail;
 
 /* case 4: lib name non escaped ID */
 if ((chp = strchr(s1, ':')) == NULL)
  {
   if (!chk_libid(s1)) { strcpy(libnam, ""); return(FALSE); }
   strcpy(celnam, s1);
   return(TRUE);
  }
 /* :config suffix present */
 if (strcmp(chp, ":config") != 0) return(FALSE);

 strncpy(s2, s1, chp - s1);
 s2[chp - s1] = '\0';
 if (!chk_libid(s2)) return(FALSE);
 strcpy(celnam, s2);

 /* SJM 05/18/04 - :config hierarchical config indirection indicator */ 
 /* can't be used with library name since library are for Verilog src */
 if (strcmp(libnam, "") != 0) return(FALSE);

 *has_config = TRUE;
 return(TRUE);
}

/*
 * read a liblist non comma separated list of libraries
 *
 * know liblist keyword read and reads first libr and keeps reading lib
 * names (no wildcards) until ending semicolon read (i.e. list ends with ;
 * and no commas)
 *
 * library names are simple IDs
 * even if error continues reading to ; or EOF
 */
static struct cfgnamlst_t *rd_liblist(FILE *fp)
{
 int32 ttyp;
 struct cfgnamlst_t *lbp, *lbp_hd, *lbp_tail;

 lbp_hd = lbp_tail = NULL;
 for (;;)
  {
   ttyp = __get_cfgtok(fp);
   if (ttyp == CFG_SEMI) break; 

   if (ttyp != CFG_ID)
    {
     __pv_ferr(3562, "config liblist library name expected - %s read",
      __to_cfgtoknam(__xs, ttyp));
     cfg_skipto_semi(ttyp, fp);
     return(NULL);
    }
   if (!chk_libid(__token))
    {
     __pv_ferr(3563,
      "config liblist library name %s illegal - must be simple Verilog ID",
      __token);
    }
   lbp = (struct cfgnamlst_t *) __my_malloc(sizeof(struct cfgnamlst_t)); 
   lbp->nam = __pv_stralloc(__token);
   lbp->cnlnxt = NULL; 
   if (lbp_hd == NULL) lbp_hd = lbp_tail = lbp;
   else { lbp_tail->cnlnxt = lbp;  lbp_tail = lbp; }
  }
 return(lbp_hd);
}

/*
 * LOW LEVEL ROUTINES FOR CFG INITIALIZATION AND ERROR RECOVERY  
 */

/*
 * initialize a cfg lib record
 */
static void init_cfglib(struct cfglib_t *lbp)
{
 lbp->lbsrc_rd = FALSE;
 lbp->lbname = NULL;
 lbp->lbels = NULL;
 lbp->lbnxt = NULL;
}

/*
 * initialize a cfg block record
 */
static void init_cfg(struct cfg_t *cfgp)
{
 cfgp->cfgnam = NULL;
 cfgp->cfgdeslist = NULL;
 cfgp->cfgrules = NULL;
 cfgp->cfgdflt = NULL;
 cfgp->cfg_fnam = NULL;
 cfgp->cfg_lno = -1;
 cfgp->cfgnxt = NULL;
}

/*
 * cfg get token error recovery skip to semi
 *
 * notice config token number not related to source reading numbers
 * but using __token global for names still
 */
static int32 cfg_skipto_semi(int32 ttyp, FILE *fp)
{
 for (;;) 
  {
   if (ttyp == CFG_SEMI || ttyp == CFG_EOF) break;
   ttyp = __get_cfgtok(fp);
  }
 return(ttyp);
}

/*
 * cfg get token error recovery skip to semi or comma
 *
 * notice config token number not related to source reading numbers
 * but using __token global for names still
 */
static int32 cfg_skipto_comma_semi(int32 ttyp, FILE *fp)
{
 for (;;) 
  {
   if (ttyp == CFG_SEMI || ttyp == CFG_EOF) break;
   ttyp = __get_cfgtok(fp);
  }
 return(ttyp);
}

/*
 * cfg get token error recovery skip to semi or endconfig
 *
 * notice config token number not related to source reading numbers
 * but using __token global for names still
 */
static int32 cfg_skipto_semi_endconfig(int32 ttyp, FILE *fp)
{
 for (;;) 
  {
   if (ttyp == CFG_SEMI || ttyp == CFG_ENDCFG || ttyp == CFG_EOF) break;
   ttyp = __get_cfgtok(fp);
  }
 return(ttyp);
}


/*
 * cfg get token error recovery skip 
 *
 * notice config token number not related to source reading numbers
 */
static int32 cfg_skipto_eof(int32 ttyp, FILE *fp)
{
 for (;;) 
  {
   if (ttyp == CFG_EOF) break;
   ttyp = __get_cfgtok(fp);
  }
 return(ttyp);
}

/*
 * ROUTINES TO EXPAND AND REPLACE LIBRARY WILDCARD FILE LISTS
 */

/* names for the special wildcard characters - see LRM */
#define STAR 1
#define QMARK 2
#define HIER 3

/*
 * return TRUE if the string contains a wildcard
 * '*', '?', '...', or ending in a / => TRUE
 */
static int32 has_wildcard(char *cp)
{
 int32 i, slen;

 slen = strlen(cp);
 /* if it ends in a slash return TRUE - include all case */
 if (cp[slen -1] == '/') return(TRUE);
 for (i = 0; i <= slen - 1; i++, cp++)
  {
   if (*cp == '*') return(TRUE);
   if (*cp == '?') return(TRUE);
   if (i < slen + 2 && strncmp(cp, "...", 3) == 0) return(TRUE);
  }
 return(FALSE);
}

/*
 * expand all wild cards in library file name lists
 *
 * first step in cfg elaboration after all map and cfg files read
 */
extern void __expand_lib_wildcards(void)
{
 register struct cfglib_t *lbp;
 register struct libel_t *lbep;
 int32 sav_lin_cnt;
 char *sav_cur_fnam, *cp;
 FILE *fp;

 /* expand for library */
 for (lbp = __cfglib_hd; lbp != NULL; lbp = lbp->lbnxt)
  {
   sav_lin_cnt = __lin_cnt;
   sav_cur_fnam = __cur_fnam;
   __cur_fnam = lbp->cfglb_fnam;
   __lin_cnt = lbp->cfglb_lno;

   /* for each fspec in one library's fspec list, expand any wildcards */
   for (lbep = lbp->lbels; lbep != NULL; lbep = lbep->lbenxt)
    {
     /* AIV mark the expanded so the pattern string is replaced */
     lbep->expanded = FALSE;
     cp = lbep->lbefnam;
     /* if it doesn't contain a wildcard char must be a file so simple open */
     if (!has_wildcard(cp))
      {
       /* if it returns NULL file no such file */
       if ((fp = __tilde_fopen(cp, "r")) == NULL)
        {
         __pv_ferr(3564, "config library %s unable to match pattern %s\n",
          lbp->lbname, cp); 
        }
       else 
        {
         /* no need to change the file name, just mark the flag as expanded */
         lbep->expanded = TRUE;
         /* close the open file */
         __my_fclose(fp); 
        }
      }
     else if (strcmp(cp, "...") == 0)
      {
       /* include all files below the current directory */
       if (!expand_single_hier(lbp, lbep, NULL))
        {
         __pv_ferr(3564, "config library %s unable to match pattern %s\n",
          lbp->lbname, cp); 
        }
      }
     else 
      {
        /* match the pattern case */
        expand_dir_pats(lbp, lbep, cp);
      }
   }
   /* put back for further reading */
   __lin_cnt = sav_lin_cnt;
   __cur_fnam = sav_cur_fnam;
  }
}

/*
 * match a hier name with the given pattern name
 */
static int32 match_hier_name(struct xpndfile_t *xfp_hd, char *name) 
{
 char *cp, *last;
 char str[RECLEN]; 
 struct xpndfile_t *xfp;
 
 /* skip past ./ meaningless */
 if (name[0] == '.' && name[1] == '/') name += 2;
 last = name;
 xfp = xfp_hd; 
 cp = strchr(name, '/');
 if (cp == NULL)
  {
   /* Special case ../../\*.v  - last pattern and matches wildcard */
   if (xfp->xpfnxt == NULL && match_wildcard_str(name, xfp)) return(TRUE);
   /* Special case .../\*.v  */
   else if (xfp->wildcard == HIER && xfp->xpfnxt->xpfnxt == NULL 
    && match_wildcard_str(name, xfp->xpfnxt)) return(TRUE);
  }
 else
  {
   for (; xfp != NULL && cp != NULL; cp = strchr(cp, '/'))
    {
     /* check the string to the next '/' to match the pattern */
     /* copy /string/ into str to check with pattern */
     strncpy(str, last, cp - last); 
     str[cp-last] ='\0';
     /* if doesn't match pattern return */
     if (!match_wildcard_str(str, xfp)) return(FALSE);
     /* handle special ... case */
     if (xfp->wildcard == HIER)
      { 
       /* no more patterns it is a match */
       if (xfp->xpfnxt == NULL) return(TRUE);

       /* match all the remaining patterns after .../ */
       /* move pattern up one and get the next /string/ */
hier:
       xfp = xfp->xpfnxt;
       for (; cp != NULL;  )
        {
         /* special case it is the last one */
         if ((cp - last) == 0) strcpy(str, cp);
         else
          {
           strncpy(str, last, cp - last); 
           str[cp-last] ='\0';
          }
         /* if matches continue */
         if (match_wildcard_str(str, xfp))
          {
           xfp = xfp->xpfnxt; 
           if (xfp == NULL) return(TRUE);
          }
         last = ++cp;
         /* last pattern in the string dir/dir2/lastpattern */
         if ((cp = strchr(cp, '/')) == NULL)
          {
           /* if more patterns continue */
           if (xfp->xpfnxt != NULL) return(FALSE); strcpy(str, last); 
           /* match the lastpattern and gets here it is a match */
           if (match_wildcard_str(str, xfp)) return(TRUE);
          }
        }
      }
     last = ++cp;

     /* include all in the directory and the last in the char name */ 
     if (xfp->incall && strchr(cp, '/') == NULL) return(TRUE);

     /* if the last pattern and didn't match FALSE  */ 
     xfp = xfp->xpfnxt; 
     if (xfp == NULL)  return(FALSE);
     if (xfp->wildcard == HIER && xfp->xpfnxt != NULL) goto hier;
     /* try to match the last of the string */ 
     if (*cp != '\0' && strchr(cp, '/') == NULL)
      {
       if (xfp->xpfnxt == NULL && match_wildcard_str(cp, xfp)) return(TRUE);
       else return(FALSE);
      }
    }
  }
 return(FALSE);
}

/*
 * match the special hierarchical wildcard in the pattern
 * is recursive call which takes the parameters -
 *
 * xfp_hd - the start of the split pattern to match 
 * bpath - the beginning part of the path (the non-wildcard start of the path) 
 * path - the current path
 *
 * builds the strings to match according to attempt to match to the xfp list 
 */
static void find_hier(struct libel_t *lbep, struct xpndfile_t *xfp_hd, 
 char *bpath, char *path)
{
 char str[RECLEN];
 char str2[RECLEN];
 char dirstr[RECLEN];
 char *cp;
 DIR *dp;
 struct dirent *dir;
#if defined(__CYGWIN32__) || defined(__SVR4)
 struct stat sbuf;
#endif
  
 /* start from the current directory */
 if (path == NULL) { strcpy(str, "."); cp = str; }
 else cp = path;

 if ((dp = opendir(cp)) == NULL)
  {
   __pv_ferr(1368, "during config library expansion cannot open dir %s : %s\n",
    cp, strerror(errno)); 
  }
 
 while ((dir = readdir(dp)) != NULL)
  {
   if (dir->d_ino == 0) continue;
   if (strcmp(dir->d_name, ".") == 0) continue;
   if (strcmp(dir->d_name, "..") == 0) continue;
#if defined(__CYGWIN32__) || defined(__SVR4)
   if (stat(dir->d_name, &sbuf) == -1) continue;
   if ((S_IFMT & sbuf.st_mode) == S_IFDIR)
#else
   if (dir->d_type == DT_DIR)
#endif
    {
     /* directory concat name and call recursively */
     if (path == NULL) sprintf(dirstr, "./%s", dir->d_name);
     else sprintf(dirstr, "%s/%s", path, dir->d_name);
     find_hier(lbep, xfp_hd, bpath, dirstr);
    }
#if defined(__CYGWIN32__) || defined(__SVR4)
   else if ((S_IFMT & sbuf.st_mode) == S_IFREG)
#else
   else if (dir->d_type == DT_REG)
#endif
    {
     str[0] ='\0';
     /* concat the file name to the directory */
     if (path == NULL) strcpy(str, dir->d_name);
     else sprintf(str, "%s/%s", cp, dir->d_name);
     /* check if the name matches the xfp list */
     if (match_hier_name(xfp_hd, str))
      {
       if (bpath != NULL)
        {
         sprintf(str2, "%s/%s", bpath, str);
         expand_libel(lbep, str2);
        }
       else expand_libel(lbep, str);
      }
    }
  }
 if (dp != NULL) closedir(dp);
}

/*
 * routine to actually do the hierarchical search 
 * moves to the first wildcard xfp and does search 
 */
static void expand_hier_files(struct cfglib_t *lbp, struct libel_t *lbep,
 struct xpndfile_t *xfp_hd)
{
 char dirstr[RECLEN];
 char bpath[RECLEN];
 char tmp[RECLEN];
 int32 first;
 
 /* if the first xfp has a wildcard do the search do search with current xfp */ 
 if (xfp_hd->wildcard) 
  {
   find_hier(lbep, xfp_hd, NULL, NULL);
   return;
  }
 first = TRUE;
 /* save current dir */
 getcwd(dirstr, RECLEN); 
 strcpy(bpath, "");
 while (!xfp_hd->wildcard && xfp_hd->incall != TRUE) 
  {
   if (chdir(xfp_hd->fpat) < 0)
    {
     if (first)
      {
       __pv_ferr(3564, "config library %s no such directory %s\n",
       lbp->lbname, xfp_hd->fpat); 
      }
     else
      {
       __pv_ferr(3564, "config library %s no such directory %s/%s\n",
        lbp->lbname, bpath, xfp_hd->fpat);
       chdir(dirstr);
       return;
      }
    }
   /* move to non-wildcard dir and buld beginning path name */
   if (first)
    {
     strcpy(bpath, xfp_hd->fpat);
     first = FALSE;
    }
   else
    {
     strcpy(tmp, bpath);
     sprintf(bpath, "%s/%s", tmp, xfp_hd->fpat);
    }
   xfp_hd = xfp_hd->xpfnxt;
  }
 /* do the search */
 find_hier(lbep, xfp_hd, bpath, NULL);
 /* go back to the original dir  */
 chdir(dirstr);
}

/*
 * routine to break up the original user pattern by '/' - xfp1/xfp2/xfpn
 */
static void expand_dir_pats(struct cfglib_t *lbp, struct libel_t *lbep,
 char *pat)
{
 int32 slen, i, ndx, clevel, last_star;
 int32  wildcard, hier, cur_hier, last_hier, back_dir;
 char str[RECLEN]; 
 char *last, *cp;
 struct xpndfile_t *xfp, *xfp2, *xfp_hd, *xfp_tail;

 str[0] = '\0';
 clevel = -1;
 xfp_hd = xfp_tail = NULL;
 slen = strlen(pat);
 cp = last = pat;
 last_star = last_hier = wildcard = FALSE;
 back_dir = cur_hier = hier = FALSE;
 /* ndx if current index of the string */
 ndx = 0;
 for (i = 0; i < slen; i++, ndx++, cp++)
  {
   /* split add a xfp to the list */
   if (*cp == '/')
    {
     cur_hier = FALSE;
     /* special verilog escape char */
     if (i + 1 < slen && *(cp+1) == '/') 
      {
       /* LOOKATME FIXME - check if this works */
       /* special espcaped char // read until next ' ' or end of str */
       i++;
       cp++;
       for (; i < slen && *cp != ' '; i++, cp++) ;
      }
     /* skip a /./ since it doesn't do anything */
     if (ndx == 1 &&  str[0] == '.') { last = (cp + 1); continue; }
     str[ndx] = '\0'; 
     if (ndx == 3 && strcmp(str, "...") == 0) 
      {
       /* if ... and so was the last skip this one */
       if (last_hier)
        {
         __pv_warn(3124, "config can't have .../... - treating as only one\n");
         ndx = -1;
         continue;
        }
       cur_hier = hier = TRUE;
      }
     else if (ndx == 2 && back_dir && strcmp(str, "..") == 0)
      {
       __pv_warn(3125,
        "Back directory '..' can only be used at the beginning of pattern string\n");
      }
      /* AIV 05/25/04 - on the first non '..' set back_dir to true */
      /* there can be multilple ../.. prior to the pattern */
     else if(ndx != 2 || strcmp(str, "..") != 0)
            back_dir = TRUE;

     xfp = (struct xpndfile_t *) __my_malloc(sizeof(struct xpndfile_t)); 
     xfp->fpat = __pv_stralloc(str);
     if (cur_hier) 
      {
       /* set the wildcar to hierarch */
       xfp->wildcard = HIER;
       last_hier = TRUE;
      }
     else
      {
       /* set the wildcar STAR or QMARK */
       xfp->wildcard = wildcard;
       last_hier = FALSE;
      }
     xfp->nmatch = 0; 
     /* special case that ends in '/' inc all the files */
     if (i + 1 == slen) xfp->incall = TRUE; 
     else xfp->incall = FALSE; 
     xfp->xpfnxt = NULL; 

     /* set the current depth level */
     xfp->level = ++clevel;
     if (xfp_hd == NULL) xfp_hd = xfp_tail = xfp;
     else { xfp_tail->xpfnxt = xfp;  xfp_tail = xfp; }

     /* set the last char to one past the / */
     if (i < slen) last = (cp + 1);
     /* reset wildcard and last_star and string index */
     wildcard = FALSE;
     last_star = FALSE;
     ndx = -1;
    }
   else if (*cp == '*')
    {
     /* previous chars was star as well so just set ndx back one to skip */
     if (last_star) ndx--;
     last_star = TRUE;
     wildcard = STAR;
     /* AIV 05/18/04 - can never be negative since last_star is a flag */
     /* that can only be set when ndx > 0 */
     str[ndx] = *cp;
    }
   else
    {
     /* star takes wildcard precedence over qmark - needed for pat matching */
     if (*cp == '?' && wildcard != STAR) wildcard = QMARK;
     last_star = FALSE;
     str[ndx] = *cp;
    }
  } 
 /* LOOKATME will /a still work */
 /* add the last one this has to be a file or file pattern */
 cur_hier = FALSE;
 if (cp != last)
  {
   if (ndx == 1 && *(cp - 1) == '.') goto done;
   str[ndx] = '\0'; 
   if (strcmp(str, "...") == 0) 
    {
     /* just skip the last ... if there is two in a row */
     if (last_hier) goto done; cur_hier = hier = TRUE;
    }
   xfp = (struct xpndfile_t *) __my_malloc(sizeof(struct xpndfile_t)); 
   xfp->fpat = __pv_stralloc(str);
   if (cur_hier) xfp->wildcard = HIER;
   else xfp->wildcard = wildcard;
   xfp->nmatch = 0; 
   xfp->xpfnxt = NULL; 
   xfp->level = ++clevel;
   if (xfp_hd == NULL) xfp_hd = xfp_tail = xfp;
   else { xfp_tail->xpfnxt = xfp;  xfp_tail = xfp; }
  }

 /* DGB REMOVE */
 if (xfp_hd == NULL) __misc_terr(__FILE__, __LINE__);

done:
 /* doesn't contain a hieracrh ... */
 if (!hier)
  {
   /* if the first xfp contains a wildcard just call match overwise */
   /* move to on wildcard dir and then match */ 
   if (xfp_hd->wildcard) match_dir_pats(lbep, xfp_hd, NULL, NULL, FALSE, 0); 
   else movedir_match_dir_pats(lbep, xfp_hd); 
  }
 else
  {
   /* match the hier case */
   expand_hier_files(lbp, lbep, xfp_hd); 
  }
 /* free xfp list */
 /* SJM 11/05/04 - need 2nd pointer since xpfnxt can't be accessed */
 /* after freed */
 for (xfp = xfp_hd ; xfp != NULL;)
  {
   xfp2 = xfp->xpfnxt;
   __my_free((char *) xfp, sizeof(struct xpndfile_t ));
   xfp = xfp2; 
  }
}

/*
 * move xfp_hd to the first wildcard to start the search
 */
static void movedir_match_dir_pats(struct libel_t *lbep, 
 struct xpndfile_t *xfp_hd) 
{
 int32 level;
 char dirstr[RECLEN]; 
 char bpath[RECLEN]; 

 level = 0;
 /* save current directory */
 getcwd(dirstr, RECLEN); 
 while (!xfp_hd->wildcard && xfp_hd->incall != TRUE) 
  {
   if (chdir(xfp_hd->fpat) < 0)
    {
     /* SJM 05/11/04 - FIXME ### ??? - need way to locate these */
     /* AIV 05/18/04 if the currect level print current pattern name */
     if (level == 0)
      { 
       __pv_warn(3132, "no such directory %s\n", xfp_hd->fpat);
      }
     else
      {
      /* if lower level print all previous path and current pattern name */
       __pv_warn(3132, "Error - no such directory path %s/%s\n",
        bpath, xfp_hd->fpat);
      }
     chdir(dirstr);
     return;
    }
   /* goto the next directory and inc the depth level */
   if (level == 0) strcpy(bpath, xfp_hd->fpat);
   else sprintf(bpath, "%s/%s", bpath, xfp_hd->fpat);
   level++;
   xfp_hd = xfp_hd->xpfnxt;
  }
 /* search directories */
 match_dir_pats(lbep, xfp_hd, NULL, bpath, FALSE, level); 
 /* do back to the current directory */
 chdir(dirstr);
}

/*
 * match the patterns for each xfp->fpat per directory
 *
 * xfp_hd - points to the first wildcard name1/ 
 * bpath - start of path not containing a wildcard 
 * path - points to all current path 
 * incall - is the flag to include all the files in the dir, end in '/'
 */
static void match_dir_pats(struct libel_t *lbep, struct xpndfile_t *xfp_hd,
char *path, char *bpath, int32 incall, int32 level) 
{
 char dirstr[RECLEN]; 
 char str[RECLEN]; 
 char str2[RECLEN]; 
 char *cp; 
 struct xpndfile_t *xfp;
 struct dirent *dir;
 DIR *dp;
#if defined(__CYGWIN32__) || defined(__SVR4)
 struct stat sbuf;
#endif
     
 /* if it's null just add the ./ */
 if (path == NULL) { strcpy(str, "./"); cp = str; }
 else cp = path;

 xfp = xfp_hd; 
 dp = NULL;
 for (; xfp != NULL; xfp = xfp->xpfnxt)
  {
   /* if the xfp->level is greater than the current level return */
   if (xfp->level > level) return;
   if ((dp = opendir(cp)) == NULL)
    {
     __pv_ferr(3569, "in config libary file %s cannot open dir %s : %s\n",
      lbep->lbefnam, cp, strerror(errno)); 
     return;
    }
   while ((dir = readdir(dp)) != NULL)
    {
     if (dir->d_ino == 0) continue;
     if (strcmp(dir->d_name, ".") == 0) continue;
     /* handle directories */
#if defined(__CYGWIN32__) || defined(__SVR4)
     if (stat(dir->d_name, &sbuf) == -1) continue;
     if ((S_IFMT & sbuf.st_mode) == S_IFDIR)
#else
     if (dir->d_type == DT_DIR)
#endif
      {
       /* if not include all and it matches the wildcard go to next dir */
       if (!incall && match_wildcard_str(dir->d_name, xfp))
        {
         /* path is null copy else concat dir name */
         if (path == NULL) sprintf(dirstr, "%s", dir->d_name);
         else sprintf(dirstr, "%s/%s", path, dir->d_name);
         /* if include all (end's in /) include all files of dir */
         if (xfp->incall)
          {
           match_dir_pats(lbep, xfp, dirstr, bpath, TRUE, level + 1);
          }
         else
          {
           match_dir_pats(lbep, xfp->xpfnxt, dirstr, bpath, incall,
            level + 1);
          }
         /* if no wildcard and doesn't end in / return */
         if (!xfp->wildcard && !xfp->incall) return;
        }
      }
#if defined(__CYGWIN32__) || defined(__SVR4)
     else if ((S_IFMT & sbuf.st_mode) == S_IFREG)
#else
     else if (dir->d_type == DT_REG)
#endif
      {
       /* handle files */
       /* if not include all in current directory */ 
       if (!incall)
        {
         /* if another pattern to macth or pattern ends in / continue */ 
         if (xfp->xpfnxt != NULL || xfp->incall) continue;
 
         /* if doesn't match the wildcard continue */ 
         if (!match_wildcard_str(dir->d_name, xfp)) continue;
        }
       /* if it gets here include the file */
       if (path == NULL) sprintf(str, "%s", dir->d_name);
       else sprintf(str, "%s/%s", cp, dir->d_name);
       if (bpath != NULL)
        {
         sprintf(str2, "%s/%s", bpath, str);
         expand_libel(lbep, str2);
        }
       else expand_libel(lbep, str);
      }
    }
  }
 if (dp != NULL) closedir(dp);
}

/*
 * routine to return T if a wildcard pattern matches a file name 
 * matches '...', '*', '?'
 * or file name with any of the wild chars
 */
static int32 match_wildcard_str(char *file, struct xpndfile_t *xfp) 
{
 int32 fndx, pndx, flen, plen; 
 int32 ondx, wildcard;
 char *opat, *patp, *filep;

 patp = xfp->fpat;
 wildcard = xfp->wildcard;
 /* if hier include all patterns */
 if (wildcard == HIER || strcmp(patp, "*") == 0) return(TRUE);
 
 /* if string is an exact match return true */
 if (strcmp(file, patp) == 0) return(TRUE);
 /* if it doesn't have a wildcard return */
 if (!xfp->wildcard) return(FALSE);

 flen = strlen(file);
 plen = strlen(patp); 
 
 /* special case if it has a star at the end match exact file - start */
 if (wildcard == STAR && flen == plen - 1 && xfp->fpat[plen-1] == '*')
  {
   if (strncmp(file, xfp->fpat, flen) == 0) return(TRUE);
  }
 filep = file;
 fndx = ondx = 0;
 /* skip the regular characters */
 while (*patp != '?' && *patp != '*') 
  {
   if (*filep != *patp) return(FALSE);
   filep++;
   patp++;
   fndx++;
   ondx++; 
  }
 
 /* reset used for * can to reset to location of last special char */
 opat = patp;

reset:
 patp = opat;
 pndx = ondx;
 for (; fndx < flen && pndx < plen; fndx++, pndx++, filep++, patp++)
  {
   /* if strings are equal or '?' goto the next char */
   if (*filep == *patp || *patp == '?') continue;
   else if (*patp == '*')
    {
     /* special case the \*\*\?\?  */
     if (*(patp + 1) == '?')
      {
       opat = (patp + 1);
       ondx = pndx + 1;
       patp++;
       pndx++;
       while (*patp == '?' && pndx < plen && fndx < flen)
        { fndx++, pndx++, filep++, patp++; }
       /* matching chars return */
       if (pndx == plen) return(TRUE);
       goto reset;
      }
     opat = patp; ondx = pndx;
     /* if a star case just move pattern to next char */
     while (*patp == '*' && pndx < plen)
      { 
       patp++; pndx++; 
      }
     if (pndx == plen) return(TRUE);

     /* if a qmark just continue and match the next char */
     if (*patp == '?') continue;

     /* while not equal move file forward */
     while (fndx < flen && *filep != *patp)
      { filep++; fndx++; }
     
     /* reached the end without finding an equal char */
     if (fndx == flen &&  *filep != *patp) return(FALSE);
     if (*filep != *patp && fndx < flen){filep++; fndx++; goto reset; }
    }
   /* if not eq, '?', or '*' doesn't match*/
   else if (fndx < flen && wildcard == STAR) goto reset;
   else return(FALSE);
  }
 if (fndx < flen && wildcard == STAR) goto reset;
 /* if string reaches the end it is a match */
 if (flen == fndx)
  {
   if (pndx == plen) return(TRUE);
   /* special case patp ends in the '*' */ 
   else if (pndx == plen - 1 && *patp == '*') return(TRUE);
  }
 return(FALSE);
} 

/*
 * expand the lib element
 */
static void expand_libel(struct libel_t *lbep, char *file)
{
 struct libel_t *newlbp;

 /* if F replace the pattern name with the expanded file name */
 if (!lbep->expanded)
  {
   /* only rename/free if it isn't the orginal string */
   /* there is no wildcard so the same stays the same */
   if (lbep->lbefnam != NULL)
    {
     __my_free(lbep->lbefnam, strlen(lbep->lbefnam) + 1);
     lbep->lbefnam = (char *) __pv_stralloc(file);
    }
   lbep->expanded = TRUE;
  }
 else
  {
   /* link on a new expanded file name  */
   newlbp = (struct libel_t *) __my_malloc(sizeof(struct libel_t));
   memcpy(newlbp, lbep, sizeof(struct libel_t));
   newlbp->lbefnam = (char *) __pv_stralloc(file);
   lbep->lbenxt = newlbp;
   lbep = newlbp;
   lbep->expanded = TRUE;
  }
}

/*
 * return to expand all patterns underneath hierarchy
 *
 * if pattern is only '...' return all files below the current path
 * recursively calls itself returning all files
 */
static int32 expand_single_hier(struct cfglib_t *lbp, struct libel_t *lbep,
 char *path)
{
 int32 count;
 char str[RECLEN];
 char dirstr[RECLEN];
 char *cp;
 DIR *dp;
 struct dirent *dir;
#if defined(__CYGWIN32__) || defined(__SVR4)
 struct stat sbuf;
#endif

 if (path == NULL) { strcpy(str, "."); cp = str; }
 else cp = path;

 count = 0;
 if ((dp = opendir(cp)) == NULL)
  {
   __pv_ferr(3569, "in config libary %s cannot open dir %s : %s\n",
    lbp->lbname, cp, strerror(errno)); 
   return(0);
  }
 while ((dir = readdir(dp)) != NULL)
  {
   if (dir->d_ino == 0) continue;
   if (strcmp(dir->d_name, ".") == 0) continue;
   if (strcmp(dir->d_name, "..") == 0) continue;
#if defined(__CYGWIN32__) || defined(__SVR4)
   if (stat(dir->d_name, &sbuf) == -1) continue;
   if ((S_IFMT & sbuf.st_mode) == S_IFDIR)
#else
   if (dir->d_type == DT_DIR)
#endif
    {
     if (path == NULL) sprintf(dirstr, "./%s", dir->d_name);
     else sprintf(dirstr, "%s/%s", path, dir->d_name);
     expand_single_hier(lbp, lbep, dirstr);
    }
#if defined(__CYGWIN32__) || defined(__SVR4)
   else if ((S_IFMT & sbuf.st_mode) == S_IFREG)
#else
   else if (dir->d_type == DT_REG)
#endif
    {
     str[0] ='\0';
     if (path == NULL) sprintf(str, "%s", dir->d_name);
     else sprintf(str, "%s/%s", cp, dir->d_name);
     count++;
     expand_libel(lbep, str);
    }
  }
 if (dp != NULL) closedir(dp);
 return(count);
}

/*
 * ROUTINES TO READ CFG LIBRARY SPECIFIED VERILOG SOURCE
 */

/*
 * read and bind cells as directed by previously read  config block
 *
 * reads cfg design statement and then read libraries according to cfg rules
 * user must not give and .v files on command line
 */
extern void __rd_ver_cfg_src(void)
{
 register struct cfg_t *cfgp;
 
 /* SJM 05/18/04 - LOOKATME - why doesn't this test work? */
 /* ### if (__last_inf != __cmd_ifi) __misc_terr(__FILE__, __LINE__); */

 prep_cfg_vflist();

 if (__cfg_verbose) dump_config_info();

 for (cfgp = __cfg_hd; cfgp != NULL; cfgp = cfgp->cfgnxt)
  {
   if (__cfg_verbose)
       __cv_msg("BINDING RULES IN CONFIG %s \n",  cfgp->cfgnam);
   bind_cfg_design(cfgp, FALSE);
  }

 /* AIV 05/24/04 - free and link out of mod list all cfg lib modules */
 /* that are in scanned files but never instantiated */
 free_unused_cfgmods();
}

/*
 * cfg verbose routine to dump names of expanded library files
 */
static void dump_lib_expand(void)
{
 struct cfglib_t *lbp;
 struct libel_t *lbep;

 __cv_msg("  Library expasion file names:\n");
 for (lbp = __cfglib_hd; lbp != NULL; lbp = lbp->lbnxt)
  {
   __cv_msg("  Libname %s\n", lbp->lbname);
   for (lbep = lbp->lbels; lbep != NULL; lbep = lbep->lbenxt)
    {
     __cv_msg("    %s\n", lbep->lbefnam);
    }
  }
}

static void dump_config_info(void)
{
 char typ[RECLEN];
 struct cfg_t *cfgp;
 struct cfgdes_t *desp;
 struct cfgrule_t *rulp;
 struct cfgnamlst_t *cnlp;

 __cv_msg("\n  DUMPING CONFIG INFORMAION:\n");
 dump_lib_expand();
 for (cfgp = __cfg_hd; cfgp != NULL; cfgp = cfgp->cfgnxt)
  {
   __cv_msg("    Config %s in %s lineno %d \n", cfgp->cfgnam, cfgp->cfg_fnam, 
    cfgp->cfg_lno);

   /* dump design info */
   for (desp = cfgp->cfgdeslist; desp != NULL; desp = desp->desnxt)
    {
     __cv_msg("      Design %s \n", desp->deslbnam); 
    }

   /* dump rule default info */
   if (cfgp->cfgdflt != NULL) __cv_msg("    Default rule:\n"); 
   rulp = cfgp->cfgdflt;
   for (cnlp = rulp->rul_libs; cnlp != NULL; cnlp = cnlp->cnlnxt)
    {
     __cv_msg("      %s \n", cnlp->nam); 
    }

   /* dump rule info */
   for (rulp = cfgp->cfgrules; rulp != NULL; rulp = rulp->rulnxt)
    {
     __cv_msg("    Rule \n"); 
     if (rulp->rultyp == CFG_INSTANCE)
        strcpy(typ, "Instance");
     else
        strcpy(typ, "Cell");
     if (rulp->use_rule_cfg)
       __cv_msg("      %s %s using hierarchical config : %s\n", 
         typ, rulp->objnam, rulp->rul_use_celnam); 
     else if (rulp->is_use)
        __cv_msg("      %s %s use %s.%s\n", rulp->objnam, 
          typ, rulp->rul_use_libnam, rulp->rul_use_celnam); 
     else
      {
       __cv_msg("      %s %s liblist:\n", typ, rulp->objnam); 
       for (cnlp = rulp->rul_libs; cnlp != NULL; cnlp = cnlp->cnlnxt)
        {
         __cv_msg("       %s \n", cnlp->nam); 
        }
      }
    }
  }
 __cv_msg("  END CONFIG DUMP\n\n");
}

/*
 * prepare the cfg input file stack - never more than one cfg lib file 
 *
 * but macro expansions and `include put on top of stack so still needed
 * this puts one open file struct on tos and inits vinstk  
 */
static void prep_cfg_vflist(void)
{
 register int32 fi;

 __last_lbf = __last_inf;
 /* set open file/macro exp./include stack to empty */
 for (fi = 0; fi < MAXFILNEST; fi++) __vinstk[fi] = NULL;
 __vin_top = -1;
 __lasttoktyp = UNDEF;
 __last_attr_prefix = FALSE;
 /* this builds the empty top of stack entry */
 __push_vinfil();
 __cur_infi = __last_inf;
}

/*
 * build error message if the module didn't match any in the
 * given library
 */ 
static void build_rule_error(struct cfg_t *cfgp, struct cfglib_t *cntxt_lbp, 
 struct cfgrule_t *rulp)
{
     
 /* if this didn't match there is no such instance */
 if (rulp->rultyp == CFG_INSTANCE)
  {
   __pv_err(3576, "config %s at %s: unable to bind rule - no such module %s",
    cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, rulp->rul_lno), 
    rulp->objnam);
  }
 else if (rulp->rultyp == CFG_CELL)
  {
   if (rulp->libnam != NULL)
    {
     /* no such library cell */ 
     __pv_err(3576,
      "config %s at %s: unable to bind rule - no such cell %s",
      cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, rulp->rul_lno), 
      rulp->objnam);
    } 
  }
}

/*
 * read source of and parse all cells in a design 
 *
 * SJM 01/09/04 FIXME ??? - must allow more than one top level design mod 
 */
static int32 bind_cfg_design(struct cfg_t *cfgp, int32 is_hier)
{
 register struct cfgdes_t *desp;
 register struct cfgrule_t *rulp;
 struct cfglib_t *lbp;
 struct mod_t *mdp;

 if (is_hier)
  {
   if (cfgp->cfgdeslist == NULL || cfgp->cfgdeslist->desnxt != NULL)
    {
     __pv_err(3571,
      "hierarchical config %s at %s - only one design statement allowed", 
      cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, cfgp->cfg_lno));
     return(FALSE);
    }
  }

 for (desp = cfgp->cfgdeslist; desp != NULL; desp = desp->desnxt)
  {
   if (desp->deslbnam == NULL)
    {
     /* SJM 01/09/04 - FIXME - need to use default rule */
     __pv_err(3571, "config %s at %s: design library name missing",
      cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, cfgp->cfg_lno));

     /* --- DBG remove -- */
     __misc_terr(__FILE__, __LINE__);
     /* --- */
    }

   if ((lbp = find_cfglib(desp->deslbnam)) == NULL)
    {
     __pv_err(3572, "config %s at %s: unable to find design library %s", 
      cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, cfgp->cfg_lno),
      desp->deslbnam);
     continue;
    }
   desp->deslbp = lbp;
 
   /* find the top level module in library lbp and parse source of file it */ 
   /* is in - normally will be in file by itself */

   /* SJM 01/13/04 - may not be top mod but from config view work down */
   /* design modules can be non top mods (instantiated somewhere) but */   
   /* that just works because sub tree just gets bound */
   /* SJM 01/13/04 - FIXME - but that means inst and type names must be same */
   if (desp->topmodnam == NULL)
    {
     __pv_err(3573,
      "config %s at %s: top level design module name missing - for design lib %s", 
      cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, cfgp->cfg_lno),
      desp->deslbnam);
     continue;
    }

   if ((mdp = find_cell_in_cfglib(desp->topmodnam, lbp)) == NULL)
    {
     __pv_err(3574,
      "config %s at %s: unable to find design top level module %s in library %s", 
      cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, cfgp->cfg_lno),
      desp->topmodnam, desp->deslbnam);
     continue;
    }

   /* can't use undef hd list because only undef inst mod types from */
   /* the one design top mod added - rest of the mods must be read and */
   /* parsed but there cells can't be added to undef list */
 
   /* free undef list added from reading one config top level module */
   free_undef_list();
 
   __last_bind_comp_ndx = 0;
   __bind_inam_comptab[0] = __pv_stralloc(mdp->msym->synam);

   /* SJM 05/18/04 - binding of mdp uses current cfg library */
   mdp->mod_cfglbp = lbp;
   
   /* AIV rare case with instance/cell rules but no undefined mods */
   /* that means rules are not matched */
   if (mdp->mcells == NULL)
    {
     for (rulp = cfgp->cfgrules; rulp != NULL; rulp = rulp->rulnxt)
      {
       /* just warn because no cells need mapping */
       /* AIV LOOKATME message can msym be NULL here ?? */
       __pv_warn(3122,
        "config %s at %s: unable to bind rule - no modules to map in design %s module %s",
        cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, rulp->rul_lno),
        desp->deslbnam, mdp->msym != NULL ? mdp->msym->synam : "undefined");
      }
    }
   else
    {
     bind_cells_in1mod(cfgp, desp->deslbp, mdp);
     /* emit errors for hierarchical paths in rules that do not exist */
     for (rulp = cfgp->cfgrules; rulp != NULL; rulp = rulp->rulnxt)
      {
       if (!rulp->matched)
        {
         build_rule_error(cfgp, desp->deslbp, rulp);
        }
      }
    } 
   /* DBG remove -- */
   if (__last_bind_comp_ndx > 0) __misc_terr(__FILE__, __LINE__);
   /* -- */
   __my_free(__bind_inam_comptab[0], strlen(__bind_inam_comptab[0]) + 1);
   __last_bind_comp_ndx = -1;
  }
 return(TRUE);
}

/*
 * build a **<file>(<line. no.) reference for config where in fils not used
 * this chops file name so know will fit
 * s must be at least RECLEN wide
 */
extern char *__cfg_lineloc(char *s, char *fnam, int32 fnlcnt)
{
 char s1[RECLEN];

 sprintf(s, "**%s(%d)", __schop(s1, fnam), fnlcnt);
 return(s);
}

/*
 * find a library by name
 */
static struct cfglib_t *find_cfglib(char *lbnam)
{
 struct cfglib_t *lbp;

 for (lbp = __cfglib_hd; lbp != NULL; lbp = lbp->lbnxt) 
  {
   if (strcmp(lbp->lbname, lbnam) == 0) return(lbp);
  }
 return(NULL);
}

/*
 * free (empty) undef list
 *
 * because config file reading requires parsing all modules in any file
 * read, can't use undef hd list - must scan cells and resolve
 * from mcells whose mod type symbol is syundefmod
 */
static void free_undef_list(void)
{
 struct undef_t *undefp, *undefp2;

 /* final step is to free temp undef list */
 for (undefp = __undefhd; undefp != NULL;) 
  {
   undefp2 = undefp->undefnxt;
   __my_free((char *) undefp, sizeof(struct undef_t)); 
   undefp = undefp2;
  }
 /* SJM 02/24/05 - must set tail to nil too */
 __undefhd = __undeftail = NULL;
}

/*
 * ROUTINES TO BIND CELLS
 */

/*
 * bind all cells inside one already bound module
 */
static void bind_cells_in1mod(struct cfg_t *cfgp, struct cfglib_t *cntxt_lbp,
 struct mod_t *mdp)
{
 register struct cfgrule_t *rulp;
 register struct libel_t *lbep;
 int32 cell_matched;
 struct cell_t *cp;
 struct sy_t *lbsyp;
 char *mnp;
 
 mdp->cfg_scanned = TRUE;
 for (cp = mdp->mcells; cp != NULL; cp = cp->cnxt)
  {
   if (!cp->cmsym->syundefmod) continue;
   cell_matched = FALSE;
   for (rulp = cfgp->cfgrules; rulp != NULL; rulp = rulp->rulnxt)  
    {
     if (!try_match_rule(cntxt_lbp, cp, rulp)) continue; 
  
     if (__cfg_verbose)
      {
       __cv_msg("  ** Rule matched for instance: %s (%s:%s) for rule config file: %s at line %d.\n\n",   
        cp->csym->synam, __in_fils[cp->cmsym->syfnam_ind], 
        mdp->msym->synam, cfgp->cfg_fnam, rulp->rul_lno);
      }
    
     if (cell_matched)
      {
       __pv_warn(3123, "config %s at %s: overriding previous defined matching rule(s)", 
        cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, rulp->rul_lno));
      }

     /* SJM 05/18/04 - cells inside bound to this lib (for %l) */
     mdp->mod_cfglbp = cntxt_lbp;
    
     cell_matched = TRUE;
     /* if it gets here the module exists so mark as TRUE */
     rulp->matched = TRUE; 
     if (!rulp->is_use)
      {
       if (!bind_liblist_rule(cfgp, cp, rulp))
        {
         /* AIV 06/01/04 - FIXME should print out the entire liblist */ 
         /* unable to match type of the ins mod in the specified library */
         __pv_err(3577,
          "config %s at %s: unable to bind instance rule - module type (%s) never found in liblist (line number %d)",
          cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, rulp->rul_lno),
          cp->cmsym->synam, rulp->rul_lno);
        }
      }
     else if (!bind_use_rule(cfgp, cntxt_lbp, cp, rulp))
      {
       /* Unable to match the use type of the mod in the specified lib */
       __pv_err(3578, "config %s at %s: unable to bind use rule - module type (%s) never found in library (%s)",
       cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, rulp->rul_lno),
       cp->cmsym->synam, rulp->objnam);
      }
      goto nxt_cell;
    }
 
   /* AIV if a file read via a rule, and in the file it contains mod 'foo' */
   /* it must also bind the 'foo' in the file without a rule */
   /* AIV LOOKATME ### ??? - is there a better way to do this */
   /* get the current file name */
   if (mdp->mod_last_ifi == - 1) goto nxt_cell; 
   mnp = __in_fils[mdp->mod_last_ifi]; 
   for (lbep = cntxt_lbp->lbels; lbep != NULL; lbep = lbep->lbenxt)
    {
     /* match the name of the expanded library file name */
     if (strcmp(lbep->lbefnam, mnp) == 0)
      {
       /* find the symbol to be bound in the library symbol table */
       if ((lbsyp = __get_sym(cp->cmsym->synam, lbep->lbel_sytab)) != NULL) 
        {
         /* bind the cell symbols */
         cp->cmsym = lbsyp;
         cp->cmsym->cfg_needed = TRUE;

         if (__cfg_verbose)
          {
           __cv_msg("  ++ Bound in config: %s\n      instance: %s (%s:%s) bound to cell: %s in module: %s from file: %s in library: %s (SCANNED).\n\n",  
            cfgp->cfgnam, cp->csym->synam, __in_fils[cp->csym->syfnam_ind], 
            cp->cmsym->synam, lbsyp->synam, mdp->msym->synam,
            mnp, cntxt_lbp->lbname);
           }
          /* cells inside bound to this lib (for %l) */
          cp->cmsym->el.emdp->mod_cfglbp = cntxt_lbp;

          /* if a module the unconnected module could have mods */
          /* that need to be connected as well  */
          /* if the connecting cell hasn't been sanned and is mod check it */
          if (!cp->cmsym->el.emdp->cfg_scanned && cp->cmsym->sytyp == SYM_M
           && mdp->mcells != NULL)
           {
            if (__cfg_verbose)
             {
              __cv_msg("Binding cells in module: %s in file: %s.\n",
               mdp->msym->synam, mnp);
             }
            if (++__last_bind_comp_ndx >= __siz_bind_comps) grow_bind_comps();
            __bind_inam_comptab[__last_bind_comp_ndx] = 
            __pv_stralloc(cp->csym->synam);

            /* bind cells in this one passing lib used to bind this one */
            bind_cells_in1mod(cfgp, cntxt_lbp, cp->cmsym->el.emdp);
           
            __my_free(__bind_inam_comptab[__last_bind_comp_ndx],
            strlen(__bind_inam_comptab[__last_bind_comp_ndx]) + 1);
            __last_bind_comp_ndx--;
           }
          goto nxt_cell;
         }
       }
     }

   /* must match rules in order */
   if ((rulp = cfgp->cfgdflt) != NULL)
    {
     /* notice default is always liblist form rule - never use form */
     if (bind_liblist_rule(cfgp, cp, rulp)) goto nxt_cell;
    }
   /* error message if cound not bind cell */
   __pv_err(3575,
    "config %s at %s: unable to bind cell %s (instance %s) (current lib %s in file %s:%d) - no rule matches",
    cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, cfgp->cfg_lno),
    cp->cmsym->synam, cp->csym->synam, cntxt_lbp->lbname,
     __in_fils[cp->csym->syfnam_ind], cp->csym->sylin_cnt); 

   nxt_cell:;
  } 
}

/*
 * routine to attempt to match one rule and return T if matches
 * does not bind
 */
static int32 try_match_rule(struct cfglib_t *cntxt_lbp, struct cell_t *cp,
 struct cfgrule_t *rulp)
{
 register int32 ci;
     
 if (rulp->rultyp == CFG_INSTANCE)
  {
   /* match inst */
   if (!cp->c_named) return(FALSE);

   if (strcmp(cp->csym->synam, rulp->inam_comptab[rulp->inam_comp_lasti])
    != 0) return(FALSE);

   /* if instance path length from config design root different */
   /* then can't match */
   if (__last_bind_comp_ndx + 1 != rulp->inam_comp_lasti) return(FALSE);

   for (ci = __last_bind_comp_ndx; ci >= 0; ci--)
    {
     if (strcmp(__bind_inam_comptab[ci], rulp->inam_comptab[ci]) != 0)
      return(FALSE);
    }
   return(TRUE);
  }
 else if (rulp->rultyp == CFG_CELL)
  {
   if (rulp->libnam != NULL && rulp->libnam[0] != '\0')
    {
     if (strcmp(rulp->libnam, cntxt_lbp->lbname) != 0) return(FALSE);
    }
   if (strcmp(cp->cmsym->synam, rulp->objnam) == 0) return(TRUE);
  }
 return(FALSE);
}

/*
 * bind instance rule with liblist clause - return T if succeeds else F
 */
static int32 bind_liblist_rule(struct cfg_t *cfgp, struct cell_t *cp,
 struct cfgrule_t *rulp)
{
 struct cfgnamlst_t *cnlp;
 struct cfglib_t *lbp;
 struct mod_t *bind_mdp;
 char s1[RECLEN];

 /* match every lib in lib list in order until find match or fail */
 for (cnlp = rulp->rul_libs; cnlp != NULL; cnlp = cnlp->cnlnxt)
  {
   /* find the current lib */
   if ((lbp = find_cfglib(cnlp->nam)) == NULL)
    {
     if (rulp->rultyp == CFG_DEFAULT)
       strcpy(s1, "default");
     else
       strcpy(s1, rulp->objnam);

     __pv_err(3571,
      "config %s at %s: binding object %s lib list clause library %s not found",
      cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, cfgp->cfg_lno),
      s1, cnlp->nam);
     continue;
    }

   /* attempt to bind - cp is cell containing */
   if ((bind_mdp = find_cell_in_cfglib(cp->cmsym->synam, lbp)) == NULL)
    continue;
   
   /* AIV 05/18/04 - binding of mdp uses current cfg library */
   bind_mdp->mod_cfglbp = lbp;

   /* this does the binding */
   cp->cmsym = bind_mdp->msym;
   cp->cmsym->cfg_needed = TRUE;

   if (__cfg_verbose)
    {
     if (rulp->rultyp == CFG_DEFAULT)
       strcpy(s1, "default");
     else
       strcpy(s1, rulp->objnam);
      __cv_msg("  ++ Bound in config: %s\n      instance: %s (%s:%s) bound to cell: %s in library: %s (%s) (LIBLIST %s).\n\n",  
         cfgp->cfgnam, cp->csym->synam, __in_fils[cp->cmsym->syfnam_ind], 
         cp->cmsym->synam, bind_mdp->msym->synam,  lbp->lbname, 
         __in_fils[bind_mdp->msym->syfnam_ind], s1);
    }

   /* bind cells inside */
   bind_cells_inside(cfgp, cp, bind_mdp, lbp);
   return(TRUE);
  } 
 return(FALSE);
}

/*
 * bind use rule
 *
 * use clause - easy because [lib].cell explicitly given
 * SJM 01/14/03 - WRITEME - handle different cfg (:config)
 */
static int32 bind_use_rule(struct cfg_t *cfgp, struct cfglib_t *cntxt_lbp,
 struct cell_t *cp, struct cfgrule_t *rulp)
{
 struct cfg_t *use_replace_cfgp;
 struct cfglib_t *lbp;
 struct mod_t *bind_mdp;
 struct sy_t *msyp;

 /* if use clause heirarchical config form find the config to use */ 
 if (rulp->use_rule_cfg)
  {
   if ((use_replace_cfgp = fnd_cfg_by_name(rulp->rul_use_celnam)) == NULL)
    {
     __pv_err(3579,
      "config %s at %s: hierichical use clause config name %s undefined - config not changed",
      cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, cfgp->cfg_lno),
      rulp->rul_use_celnam);
    }

   if (bind_cfg_design(use_replace_cfgp, TRUE))
    {
     /* SJM 05/18/04 - BEWARE - assuming (and must check) only one */
     /* design statement for hierarchical sub configs */
     if ((msyp = __get_sym(use_replace_cfgp->cfgdeslist->topmodnam,
      __modsyms)) != NULL) 
      cp->cmsym = msyp;
     cp->cmsym->cfg_needed = TRUE;
     if (__cfg_verbose)
      {
       __cv_msg("  ++ Bound in config: %s using hierarchical config: %s\n      binding instance: %s (%s:%s) bound to cell: %s (%s) (USE CLAUSE).\n\n",  
       cfgp->cfgnam, use_replace_cfgp->cfgnam, cp->csym->synam, 
       __in_fils[cp->cmsym->syfnam_ind], cp->cmsym->synam, msyp->synam,
        __in_fils[msyp->syfnam_ind]);
      }
    }
   return(TRUE);
  }

 if (rulp->rul_use_libnam == NULL || rulp->rul_use_libnam[0] == '\0') 
  {
   lbp = cntxt_lbp;
  }
 else
  { 
   if ((lbp = find_cfglib(rulp->rul_use_libnam)) == NULL)
    {
     __pv_err(3571,
      "config %s at %s: object %s use clause %s:%s library %s not found",
      cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, cfgp->cfg_lno),
      rulp->objnam, rulp->rul_use_libnam, rulp->rul_use_celnam,
      rulp->rul_use_libnam);
     return(FALSE);
    }
  }

 /* use the rul_use ins name 'use lib.rul_use_celnam' */
 if ((bind_mdp = find_cell_in_cfglib(rulp->rul_use_celnam, lbp)) == NULL)
  {
    __pv_err(3573, "config %s at %s: object %s use clause %s.%s cell %s not found",
     cfgp->cfgnam, __cfg_lineloc(__xs, cfgp->cfg_fnam, cfgp->cfg_lno),
     rulp->objnam, lbp->lbname, rulp->rul_use_celnam,
     rulp->rul_use_celnam);
    return(FALSE);
   }

 /* bind the library name (%l) */
 bind_mdp->mod_cfglbp = lbp;

 /* found cell */
 cp->cmsym = bind_mdp->msym;
 cp->cmsym->cfg_needed = TRUE;
 if (__cfg_verbose)
  {
   __cv_msg("  ++ Bound in config: %s \n       binding instance: %s (%s:%s) bound to cell: %s in library: %s (%s) (USE CLAUSE).\n\n",  
   cfgp->cfgnam, cp->csym->synam, __in_fils[cp->cmsym->syfnam_ind], 
   cp->csym->synam, bind_mdp->msym->synam, lbp->lbname, 
   __in_fils[bind_mdp->msym->syfnam_ind]);
  }

 /* bind cells inside */
 bind_cells_inside(cfgp, cp, bind_mdp, lbp);
 return(TRUE);
}

/*
 * find a config given a cfg name
 */
static struct cfg_t *fnd_cfg_by_name(char *confnam)
{
 register struct cfg_t *cfgp;

 for (cfgp = __cfg_hd; cfgp != NULL; cfgp = cfgp->cfgnxt)
  {
   if (strcmp(cfgp->cfgnam, confnam) == 0)
    {
     return(cfgp);
    }
  }
 return(NULL);
}

/*
 * after binding one cell bind cells depth first inside it
 */
static void bind_cells_inside(struct cfg_t *cfgp, struct cell_t *cp,
 struct mod_t *bind_mdp, struct cfglib_t *lbp)
{
 if (++__last_bind_comp_ndx >= __siz_bind_comps) grow_bind_comps();
 __bind_inam_comptab[__last_bind_comp_ndx] = __pv_stralloc(cp->csym->synam);

 /* bind cells in this one passing library used to bind this one */
 bind_cells_in1mod(cfgp, lbp, bind_mdp);
           
 __my_free(__bind_inam_comptab[__last_bind_comp_ndx],
   strlen(__bind_inam_comptab[__last_bind_comp_ndx]) + 1);
 __last_bind_comp_ndx--;
}

/*
 * ROUTINE TO FIND A CFG CELL IN A LIBRARY AND FIRST PASS READ SRC 
 */

/*
 * find a cell in a config library list of cells
 *
 * if config library file compiled, find pre-fixup d.s module (cell)
 * else read all source in file and search for cell
 *
 * keeps reading until finding cell or reaching end of library file list
 * if any part of file is read all cells in file are read and put in
 * lib el's symbol table with ptr to mod pre-fixup d.s. 
 */
static struct mod_t *find_cell_in_cfglib(char *celnam, struct cfglib_t *lbp)
{
 register struct libel_t *lbep;
 struct sy_t *msyp;

 for (lbep = lbp->lbels; lbep != NULL; lbep = lbep->lbenxt)
  {
   if (!lbep->lbelsrc_rd)
    {
     /* move keep adding files read into in fils for error locations */
     if (++__last_lbf >= __siz_in_fils) __grow_infils(__last_lbf);
     __in_fils[__last_lbf] = __pv_stralloc(lbep->lbefnam);

     /* returns F and emits error if can't open llb file */
     if (!open_cfg_lbfil(lbp->lbname)) continue;

     rd_cfg_srcfil(lbep);
     lbep->lbelsrc_rd = TRUE;
    }

   /* AIV - 05/24/04 - the rare case there is no symbol table */
   /* if the source only contains `define, `tiemscale, etc, continue */
   if (lbep->lbel_sytab == NULL) continue;
   if ((msyp = __get_sym(celnam, lbep->lbel_sytab)) != NULL) 
   {
    return(msyp->el.emdp); 
   }
  }
 return(NULL);
}

/*
 * try to open the config library file and repl. top of stack with its info
 *
 * except for includes and macros size of vinstk will always be 1
 * file must be openable and have contents
 * return F on fail
 * in fils of last lbf must be filled with file name
 * since know last_lbf > last_inf - on EOF get_vtok will resturn to caller
 */
static int32 open_cfg_lbfil(char *lbnam)
{
 char dirstr[RECLEN]; 

 /* know called with last_lbf index of file to process */
 __cur_fnam = __in_fils[__last_lbf];
 /* AIV PUTBACK print directory for now - for debugging */
 if ((__in_s = __tilde_fopen(__cur_fnam, "r")) == NULL)
  {
   __pv_err(710, "cannot open config library %s file %s in dir : %s - skipped",
    lbnam, __cur_fnam,  getcwd(dirstr, RECLEN)); 
   return(FALSE);
  }
 if (feof(__in_s))
  {
   __pv_warn(512, "config library %s file %s empty", lbnam, __cur_fnam);
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
 if (__cfg_verbose)
  {
   __cv_msg("  Parsing config library %s file \"%s\".\n", lbnam, __cur_fnam);
  }
 return(TRUE);
}

/*
 * read cfg source file to find and elaborate current module
 *
 * read src and to lbel symbol table for every module in library
 */
static void rd_cfg_srcfil(struct libel_t *lbep)
{
 __get_vtok();
 if (__toktyp == TEOF)
  {
   __pv_fwarn(609, "config library file %s contains no tokens",
    lbep->lbefnam);
   /* since empty, mark so not re-read */
   lbep->lbelsrc_rd = TRUE;
   return;
  }

 for (;;)
  {
   /* may be a compiler directive */
   if (__toktyp >= CDIR_TOKEN_START && __toktyp <= CDIR_TOKEN_END)
    {
     __process_cdir();
     goto nxt_tok;
    }
   switch ((byte) __toktyp) {
    case TEOF: return;
    case MACROMODULE:
     __get_vtok();
     __finform(423,
      "macromodules in config library not expanded - %s translated as module",
      __token);
     goto chk_name;
    case MODULE:
     __get_vtok();
chk_name:
     if (__toktyp != ID)
      {
       __pv_ferr(707, "config library file module name expected - %s read",
        __prt_vtok());
       /* since error, just try to resynchronize */
       __vskipto_modend(ENDMODULE);
       goto nxt_tok;
      }
     if (!init_chk_cfg_sytab(lbep, "module"))
      {
       __vskipto_modend(ENDMODULE);
       goto nxt_tok;
      }

     /* know error here will cause skipping to file level thing */
     /* this adds mod name to lbel one file's symbol table */
     if (!__rd_moddef(lbep->lbel_sytab, TRUE)) goto nxt_tok;

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
     break;

    case PRIMITIVE:
     __get_vtok();
     if (__toktyp != ID)
      {
       __pv_ferr(708,
        "config library file udp primitive name expected - %s read",
        __prt_vtok());
       /* since err, just try to skip to end primitive */
       __vskipto_modend(ENDPRIMITIVE);
       goto nxt_tok;
      }
     if (!init_chk_cfg_sytab(lbep, "udp primitive"))
      {
       __vskipto_modend(ENDPRIMITIVE);
       goto nxt_tok;
      }

     if (!__rd_udpdef(lbep->lbel_sytab)) goto nxt_tok;
     break;
    default:
     __pv_ferr(709,
     "config library file module, primitive or directive expected - %s read",
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
       __pv_err(924, "last `ifdef unterminated in config libary file %s",
        __cur_fnam);
      }
     break;
    }
   __get_vtok();
   if (__toktyp == TEOF) goto chk_ifdef;
  }
}

/*
 * check module/udp name and alloc lib el symbol table if needed
 * returns F on error
 */
static int32 init_chk_cfg_sytab(struct libel_t *lbep, char *celtyp)
{
 struct sy_t *syp;

 /* if first mod in file, build the lbel's symbol table */ 
 if (lbep->lbel_sytab == NULL) lbep->lbel_sytab = __alloc_symtab(FALSE);

 /* check to see if name repeated in this library file */
 if ((syp = __get_sym(__token, lbep->lbel_sytab)) != NULL) 
  {
   __pv_ferr(3474,
    "%s %s repeated in config library file %s - previous at %s",
    celtyp, syp->synam, lbep->lbefnam,
    __bld_lineloc(__xs, syp->syfnam_ind, syp->sylin_cnt));
   return(FALSE);
  }

 if (__lib_verbose)
  {
   __cv_msg("  Scanning config library %s %s (%s).\n",
    lbep->lbefnam, celtyp, __token);
  } 
 return(TRUE);
}

/*
 * ROUTINES TO FREE COMPILED LIBRARY MODULE NEVER INSTANTIATED
 */
static void add_cfgsym(char *libnam, struct tnode_t *tnp)
{
 struct tnode_t *ntnp;
 char s1[RECLEN];

  if (tnp == NULL) return;
  add_cfgsym(libnam, tnp->lp);

  // AIV FIXME ??? sprintf(s1, "%s.%s", libnam, tnp->ndp->synam);
  if (tnp->ndp->cfg_needed)
   {
    strcpy(s1, tnp->ndp->synam);
    ntnp = __vtfind(s1, __modsyms);
   //AIV FIXME ###
    if (!__sym_is_new)
     {
      __fterr(305,
       "Sorry - config code to rename module with same name from different libraries not implemented yet.");
     }
    __add_sym(s1, ntnp);
    ntnp->ndp = tnp->ndp;
    strcpy(ntnp->ndp->synam, s1);
    ntnp->ndp->sydecl = TRUE;
    (__modsyms->numsyms)++;
   }
  add_cfgsym(libnam, tnp->rp);
}


/* 
 * add all the used symbols from a config to __modsyms
 */
static void add_cfg_libsyms(struct cfglib_t *cfgp)
{
 struct libel_t *lbp;
 struct symtab_t *symt;
 struct tnode_t *tnp;
 char *cp;
 int32 i;


 for (lbp = cfgp->lbels; lbp != NULL; lbp = lbp->lbenxt)
  {
   if ((symt = lbp->lbel_sytab) == NULL) continue;
   if (symt->stsyms != NULL)
    {
     for (i = 0; i < symt->numsyms; i++)
      {
       if (!symt->stsyms[i]->cfg_needed) continue;
       cp = symt->stsyms[i]->synam;
       tnp = __vtfind(cp, __modsyms);
      //AIV FIXME ###
       if (!__sym_is_new)
        {
         __fterr(305,
          "Sorry - config code to rename module with same name from different libraries not implemented yet.");
        }
       __add_sym(cp, tnp);
       (__modsyms->numsyms)++;
       tnp->ndp = symt->stsyms[i];
      }
    }
   else
    {
       add_cfgsym(cfgp->lbname, symt->n_head);
    }
  }
}


/* 
 * removes all modules that were scanned in from config libraries 
 * but are never needed remove from the __modhdr list
 */
static void free_unused_cfgmods(void)
{
 struct mod_t *mdp, *mdp2, *last_mdp; 
 long sav_mem_use;

//AIV FIXME ### need to free
 __modsyms = NULL;
 __modsyms = __alloc_symtab(FALSE);
 __sym_addprims();
 sav_mem_use = __mem_use;
 last_mdp = NULL;
 for (mdp = __modhdr; mdp != NULL;)
  {
   mdp2 = mdp->mnxt;

   /* if module is from a config and hasn't been linked to a library rm */
   if (mdp->m_inconfig && mdp->mod_cfglbp == NULL)
    {
     /* SJM 05/28/04 FIXME ### ??? - need to do some freeing of libs */
     /* partially_free_mod(mdp); */

     if (last_mdp != NULL) last_mdp->mnxt = mdp2;
     mdp = mdp2;
     continue;
    }
   if (!mdp->mod_cfglbp->sym_added)
   {
     add_cfg_libsyms(mdp->mod_cfglbp);
     mdp->mod_cfglbp->sym_added = TRUE;
   }
   last_mdp = mdp;
   mdp = mdp2;
  }
 /* AIV 05/31/05 - FIXME ### if not freeing no need to print message */
 /*
 if (__cfg_verbose)
  {
   __cv_msg(
   "  Config freeing most memory in unused library modules - %ld bytes freed\n",
    sav_mem_use - __mem_use);
  }
  */
}

/* 
 * partially free a module (just the larger parts such as stmts and cells)
 *
 * for now freeing most of the inisdes of modules that were parsed during
 * config library loading - eventually will moved to precompiled lib scheme
 * so this will be removed
 *
 * not free mdp but now nothing points to it
 *
 * SJM 05/26/04 - LOOKATME - since only free large d.s. in modules there is
 * quite a bit of memory leakage here
 */
static void partially_free_mod(struct mod_t *mdp)
{
 register struct cell_t *cp, *cp2;

 for (cp = mdp->mcells; cp != NULL; )
  {
   cp2 = cp->cnxt;
   __my_free((char *) cp, sizeof(struct cell_t));
   cp = cp2;
  }

 /* SJM 05/26/04 - LOOKATME ??? ### can't free stmt until fixup ---
 struct ialst_t *ialp;
 struct task_t *tskp;
 for (ialp = mdp->ialst; ialp != NULL; ialp = ialp->ialnxt) 
  {
   __free_stlst(ialp->iastp);
  }

 for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
  {
   __free_stlst(tskp->tskst);
  }
 --- */

}
