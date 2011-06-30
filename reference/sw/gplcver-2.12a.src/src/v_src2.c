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
 * second source module for statement reading and expressions parsing
 * tasks in src 3
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
static struct st_t *rd_block(int32, int32);
static struct st_t *convert_to_fork(struct st_t *);
static int32 rd_lstofsts(int32, struct st_t **, int32, int32);
static struct st_t *rd_namblock(int32, int32, int32, int32);
static struct st_t *rd_if(void);
static struct st_t *rd_case(int32);
static struct st_t *rd_loop(int32);
static struct st_t *rd_for(void);
static struct st_t *rd_cause(void);
static struct st_t *rd_disable(void);
static struct expr_t *find_bldxpr_tfsy(char *, int32);
static struct st_t *rd_wireassign(int32);
static struct st_t *rd_wiredeassign(int32);
static struct st_t *rd_taske_or_proc_assign(void);
static struct st_t *rd_dctrl_st(void);
static struct expr_t *rd_delctrl(int32 *, int32 *);
static int32 col2_lval(void);
static int32 col_dctrl_xmr(void);
static int32 col_evctrlexpr(void);
static void init_gref(struct gref_t *, char *);
static struct expr_t *parse_qcexpr(void);
static void xskip_toend(void);
static struct expr_t *parse_boolorop(void);
static struct expr_t *parse_boolandop(void);
static struct expr_t *parse_borop(void);
static struct expr_t *parse_bxorop(void);
static struct expr_t *parse_bandop(void);
static struct expr_t *parse_eqop(void);
static struct expr_t *parse_ltgtop(void);
static struct expr_t *parse_shop(void);
static struct expr_t *parse_addop(void);
static struct expr_t *parse_mulop(void);
static struct expr_t *parse_unopterm(void);
static void skip_3valend(void);
static int32 is_unop(word32);
static struct expr_t *parse_term(void);
static int32 decl_id_inexpr(struct expr_t *, struct expridtab_t *);
static struct expr_t *parse_concat(void);
static struct expr_t *parse_select(struct expr_t *);
static struct expr_t *parse_glbref(struct expr_t *, struct expridtab_t *);
static struct expr_t *bld_1cmp_global(struct expr_t *, struct expridtab_t *);
static char *alloc_glbndp_tostr(struct expr_t *);
static void grow_grtab(void);
static struct expr_t *parse_fcall(struct expr_t *, struct expridtab_t *, int32);
static int32 chk_decl_func(int32 *, struct expr_t *, struct expridtab_t *);
static void cnvt_forw_tfcall_1cmpglb(struct expr_t *, char *, int32, int32);
static struct expr_t *parse_evexpr(void);
static void grow_exprtab(void);
static struct expr_t *my_xndalloc(void);
static struct expr_t *alloc_xtnd(int32);
static void set2_opempty(struct expr_t *);
static char *to_xndnam(char *, int32 xndi);
static word32 get_hash(word32 *, word32 *, int32);

/* extern prototypes (maybe defined in this module) */
extern char *__pv_stralloc(char *);
extern char *__my_malloc(int32);
extern char *__my_realloc(char *, int32, int32);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern char *__prt_vtok(void);
extern char *__prt_kywrd_vtok(void);
extern struct sy_t *__get_sym_env(char *);
extern struct sy_t *__get_sym(char *, struct symtab_t *);
extern struct sy_t *__decl_sym(char *, struct symtab_t *);
extern struct net_t *__add_net(struct sy_t *);
extern char *__to_idnam(struct expr_t *);
extern char *__to_sytyp(char *, word32);
extern struct st_t *__alloc_stmt(int32);
extern char *__get_vkeynam(char *, int32);
extern void __find_matchendblk(int32, int32);
extern void __find_matchendcase(void);
extern int32 __col_caseexpr(void);
extern struct exprlst_t *__alloc_xprlst(void);
extern struct csitem_t *__alloc_csitem(void);
extern char *__to_opname(word32);
extern struct paramlst_t *__alloc_pval(void);
extern char *__regab_tostr(char *, word32 *, word32 *, int32, int32, int32);
extern char *__bld_lineloc(char *, word32, int32);
extern struct gref_t *__bld_glbref(struct expr_t *, int32, int32);
extern struct expr_t *__alloc_newxnd(void);
extern void __get_vtok(void);
extern void __unget_vtok(void);
extern int32 __vskipto_any(int32);
extern int32 __vskipto2_any(int32, int32);
extern int32 __vskipto3_any(int32, int32, int32);
extern int32 __vskipto4_any(int32, int32, int32, int32);
extern int32 __bld_tsk(char *, int32);
extern int32 __rd_tfdecls(char *);
extern void __set_xtab_errval(void);
extern void __bld_xtree(int32);
extern void __bld_evxtree(void);
extern int32 __col_parenexpr(int32);
extern int32 __col_lval(void);
extern int32 __col_comsemi(int32);
extern void __resolve_glbnam(struct gref_t *);
extern void __fill_grp_targu_fld(struct gref_t *);
extern void __set_opempty(int32);
extern int32 __col_connexpr(int32);
extern int32 __bld_expnode(void);
extern void __init_xnd(struct expr_t *);
extern void __set_numval(struct expr_t *, word32, word32, int32);
extern void __my_free(char *, int32);
extern void __free2_xtree(struct expr_t *);
extern struct st_t *__rd_tskenable(char *, struct expr_t *, int32);
extern void __setup_contab(void);
extern int32 __alloc_shareable_cval(word32, word32, int32);
extern int32 __alloc_shareable_rlcval(double);
extern int32 __alloc_is_cval(int32);
extern int32 __allocfill_cval_new(word32 *, word32 *, int32);
extern char *__alloc_vval_to_cstr(word32 *, int32, int32, int32);
extern void __grow_contab(int32);
extern struct expridtab_t *__alloc_expridnd(char *);
extern struct expr_t *__alloc_exprnd(void);

extern void __pv_ferr(int32, char *, ...);
extern void __pv_fwarn(int32, char *, ...);
extern void __gferr(int32, word32, int32, char *, ...);
extern void __dbg_msg(char *, ...);
extern void __my_fprintf(FILE *, char *, ...);
extern void __fterr(int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);
extern void __misc_fterr(char *, int32);
extern void __ia_err(int32 id_num, char *s, ...);

extern struct opinfo_t __opinfo[];
extern word32 __masktab[];

/*
 * STATEMENT PROCESSING ROUTINES
 */

/*
 * read statement 
 * expects 1st token read and reads ending ; or end or del. ctrl.
 * statement routines allocate and return statement - caller links it in
 *
 * this routine cannot skip, but if it calls rd_stmt must handle skipping
 */
extern struct st_t *__rd_stmt(void)
{
 struct st_t *stp;

 __saverr_cnt = __pv_err_cnt;
 stp = NULL;
 /* statement routines for bracketed statements will have skipped to end */
 switch ((byte) __toktyp) {
  case IF:
   if ((stp = rd_if()) == NULL) return(NULL);
   break;
  case Begin:
   /* will have skipped to block end on error */
   /* if possible this returns list of statements instead of block */
   if ((stp = rd_block(Begin, END)) == NULL) return(NULL);
   break;
  case FORK:
   if ((stp = rd_block(FORK, JOIN)) == NULL) return(NULL);
   break;
  case SEMI:
   stp = __alloc_stmt(S_NULL);
   break;
  case CASE: case CASEX: case CASEZ:
   if ((stp = rd_case(__toktyp)) == NULL) return(NULL);
   break;
  case FOREVER: case WHILE: case WAIT: case REPEAT:
   /* notice no repeat form del ctrls since repeat keyword indicates stmt */
   if ((stp = rd_loop(__toktyp)) == NULL) return(NULL);
   break;
  case FOR:
   if ((stp = rd_for()) == NULL) return(NULL);
   break;
  case SHARP: case AT:
   if ((stp = rd_dctrl_st()) == NULL) return(NULL);
   break;
  case DISABLE:
   if ((stp = rd_disable()) == NULL) return(NULL);
   break;
  case CAUSE:
   if ((stp = rd_cause()) == NULL) return(NULL);
   break;
  case ASSIGN:
   if ((stp = rd_wireassign(ASSIGN)) == NULL) return(NULL);
   break;
  case DEASSIGN:
   if ((stp = rd_wiredeassign(DEASSIGN)) == NULL) return(NULL);
   break;
  case FORCE:
   if ((stp = rd_wireassign(FORCE)) == NULL) return(NULL);
   break;
  case RELEASE:
   if ((stp = rd_wiredeassign(RELEASE)) == NULL) return(NULL);
   break;
  /* these are resync error conditions - must handle out of sync to end */
  case ENDTASK:
  case ENDFUNCTION:
  case ENDSPECIFY:
   __pv_ferr(1060, "module item end bracket problem - %s read",
    __prt_vtok());
   /* SJM 01/18/1999 - must stay at module level end to try to finish */ 
   /* item gracefully */
   /* ??? __get_vtok(); */
   __syncto_class = SYNC_MODLEVEL;
   return(NULL);
  case ENDCASE:
  case END:
  case JOIN:
   __syncto_class = SYNC_STMT;
bad_struct:
   __pv_ferr(1061, "statement structure end bracket problem - %s read",
    __prt_vtok());
   __get_vtok();
   return(NULL);
  case ENDMODULE:
  case ENDPRIMITIVE:
  case ENDTABLE:
   __syncto_class = SYNC_FLEVEL;
   goto bad_struct;

  /* non keyword - only 2 possibilities */
  case ID: case LCB:
   /* exactly 2 things can start proc lhs [id (glb ref)] = , {...} = */
   /* also task enable [id (glb ref)](... legal - nothing else */
   if ((stp = rd_taske_or_proc_assign()) == NULL) return(NULL);
   break;
  default:
   __pv_ferr(1062, "statement starting token expected - %s read",
    __prt_vtok());
   /* must advance one token here else end = uu; never advances */
   __get_vtok();
   __vskipto_any(SEMI);
   return(NULL);
 }
 return(stp);
}

/*
 * read a sequential or fork/join block
 * begin/fork read, reads end
 * notice for unnamed begin-end block will return list of stmts
 * with first have unb head turned on 
 */
static struct st_t *rd_block(int32 btok, int32 endbtok)
{
 int32 slcnt, sfnind;
 struct st_t *stp, *hdstp;

 slcnt = __lin_cnt;
 sfnind = __cur_fnam_ind;
 __get_vtok();
 if (__toktyp == COLON)
  {
   if (__iact_state)
    {
     /* cannot catch this later because no declarations in interactive */
     __pv_err_cnt++;
     __ia_err(1431, "named blocks illegal in interactive mode");
     return(NULL);
    }
   /* task without params embedded in statement list */
   __cur_declobj = TASK;
   stp = rd_namblock(btok, endbtok, slcnt, sfnind);
   __cur_declobj = MODULE;
   return(stp);
  }
 /* first step is to read list of statements in block */
 /* if all statements in error will look like empty statement */
 if (!rd_lstofsts(endbtok, &hdstp, __lin_cnt, __cur_fnam_ind)) return(NULL);
 if (btok == Begin)
  {
   if (hdstp != NULL)
    {
     hdstp->st_unbhead = TRUE;
     /* correct to line of begin */
     /* ---
     hdstp->stfnam_ind = sfnind;
     hdstp->stlin_cnt = slcnt;
     -- */
    }
   return(hdstp);
  }
 /* this can never return nil */ 
 stp = convert_to_fork(hdstp);
 /* correct to line of fork */
 /* SJM ??? UNDO FOR RELEASE ---
 stp->stfnam_ind = sfnind;
 stp->stlin_cnt = slcnt;
 --- */
 return(stp);
}

/*
 * convert a statement list to a fork-join statement
 * 
 * at this point st nxt fields point to next in list
 * converts to table where nexts all empty and nil at end of table
 */
static struct st_t *convert_to_fork(struct st_t *hdstp)
{
 register int32 fji;
 register struct st_t *stp;
 int32 num_fjs;
 struct st_t *stp2, *fjstp;

 /* first count how many */
 for (num_fjs = 0, stp = hdstp; stp != NULL; stp = stp->stnxt)
  {
   /* SJM 09/24/01 - for assign followed by for pair count as one */
   if (stp->stmttyp == S_FORASSGN) continue;
   num_fjs++;
  }

 /* know this is fork-join (never named) */
 fjstp = __alloc_stmt(S_UNFJ);
 /* one extra for nil fence at end */ 
 fjstp->st.fj.fjstps = (struct st_t **)
  __my_malloc((num_fjs + 1)*sizeof(struct st_t *));
 fjstp->st.fj.fjlabs = (int32 *) __my_malloc((num_fjs + 1)*sizeof(int32));

 /* fill table of ptrs to stmts */
 /* for now leaving begin-end as unnamed block - not making st. list */ 
 /* even if empty (because of error) need body as NULL statement */ 
 for (stp = hdstp, fji = 0; fji < num_fjs; fji++)
  {
   /* SJM 09/24/01 - for for need both S FOR ASSIGN and for as one "block" */
   /* since unnamed begin end blocks are just lists, works */
   /* notice other setup inserted not inserted by here so can ignore */
   if (stp->stmttyp == S_FORASSGN)
    {
     stp2 = stp->stnxt->stnxt;
     fjstp->st.fj.fjstps[fji] = stp;
     fjstp->st.fj.fjlabs[fji] = -1;
     stp->stnxt->stnxt = NULL;
    }
   else
    {
     stp2 = stp->stnxt;
     fjstp->st.fj.fjstps[fji] = stp;
     fjstp->st.fj.fjlabs[fji] = -1;
     /* fork-join is exactly one statement - unnamed block if begin-end */
     stp->stnxt = NULL;
    }
   stp = stp2;
  }
 /* set ending fence */
 fjstp->st.fj.fjstps[num_fjs] = NULL;
 fjstp->st.fj.fjlabs[num_fjs] = -1;
 return(fjstp);
}

/*
 * read a statement list ending with block end token
 * for contents of block, know block begin and optional label read
 * reads block end token
 * must use local variables since recursively nested blocks legal
 */
static int32 rd_lstofsts(int32 endbtok, struct st_t **stpp, int32 slcnt,
 int32 sfnind)
{
 struct st_t *stp, *last_stp, *unbstp;

 /* block body statements may be missing i.e begin end ok */
 last_stp = NULL;
 if (__toktyp == endbtok)
  {
   stp = __alloc_stmt(S_STNONE);
   stp->stfnam_ind = sfnind;
   stp->stlin_cnt = slcnt;
   *stpp = stp;
   return(TRUE);
  }
 for (*stpp = NULL;;)
  {
   /* on error, this attempts to read to 1 before next statement token */
   /* this expects 1st token of statement to have been read */
   if ((stp = __rd_stmt()) == NULL)
    {
     if (__syncto_class == SYNC_STMT || __syncto_class == SYNC_TARG)
      {
       __get_vtok();
       if (__toktyp == TEOF)
        { __syncto_class = SYNC_FLEVEL; return(FALSE); }
       continue;
      }
     /* know hit module or file level thing - think unresyncable */
     return(FALSE);
    }
   /* if statement is unnamed block, change normal list to unnamed block */ 
   /* need since in list next already used */
   /* but need lists for blocks normally so can insert gotos, etc. */
   if (stp->st_unbhead)
    {
     unbstp = __alloc_stmt(S_UNBLK);
     unbstp->st.sbsts = stp;
     unbstp->stfnam_ind = sfnind;
     unbstp->stlin_cnt = slcnt;
     stp = unbstp;
    }
   /* try to parse as much as possible even if errors */
   if (last_stp == NULL) *stpp = stp; else last_stp->stnxt = stp;
   /* for returns 2 statement list - since reading list just 1 longer */
   if (stp->stmttyp == S_FORASSGN) last_stp = stp->stnxt; 
   else last_stp = stp;
   __get_vtok();
   if (__toktyp == endbtok) break;
  }
 return(TRUE);
}

/*
 * read a named block
 * know : read and reads block end token
 * notice named block linked on task list and from stmt
 * also notice body is pointer to statement that may or may not be list
 */
static struct st_t *rd_namblock(int32 btok, int32 endbtok, int32 slcnt,
 int32 sfnind)
{
 struct task_t *sav_cur_tsk;
 struct st_t *hdstp, *stp;

 __get_vtok();
 if (__toktyp != ID)
  {
   __pv_ferr(1063, "block label expected - %s read - skipped",
     __prt_kywrd_vtok());
   /* here just assume left out */
   strcpy(__token, "**filler**");
   __toktyp = ID;
   __unget_vtok();
  }
 /* save cur. task - since recursively called and on stack nesting, works */
 sav_cur_tsk = __cur_tsk;

 /* this increments top sti and changes current task */
 /* if error here, must give up - find like level end */
 if (!__bld_tsk(__token, btok))
  {
   __find_matchendblk(btok, endbtok);
   return(NULL);
  }
 /* illegal I/O decls here caught later */
 __get_vtok();
 /* allocate even though wasted if error since need line no. */
 stp = __alloc_stmt(S_NAMBLK);
 /* correct to line of begin */
 stp->stlin_cnt = slcnt;
 stp->stfnam_ind = sfnind;

 /* statement has a task body as it data */
 stp->st.snbtsk = __cur_tsk;
 /* also fill back ptr to the one location named block task in */ 
 __cur_tsk->st_namblkin = stp;

 /* continues through all decls - even if error, if F then must give up */
 /* T possible on error if hit statement */
 if (!__rd_tfdecls("named block"))
  {
   __find_matchendblk(btok, endbtok);
bad_end:
   __top_sti--;
   __cur_tsk = sav_cur_tsk;
   return(NULL);
  } 
 /* according to grammar this must be a statement list */
 /* format is begin/fork : [name] [decls?] [stmt list] end/join */
 /* this will have sync to end of block - or mod/file level thing */
 /* SJM 10/19/05 - line count and file arguments name were reversed */ 
 if (!rd_lstofsts(endbtok, &hdstp, slcnt, sfnind)) goto bad_end;
 __cur_tsk->tsk_last_lini = __lin_cnt;
 __cur_tsk->tsk_last_ifi = __cur_fnam_ind;

 /* symbols no longer accessible */
 __top_sti--;
 if (btok == Begin) __cur_tsk->tskst = hdstp;
 else __cur_tsk->tskst = convert_to_fork(hdstp);

 /* must link named block on task list */
 if (__end_tbp == NULL) __inst_mod->mtasks = __cur_tsk;
 else __end_tbp->tsknxt = __cur_tsk;
 __end_tbp = __cur_tsk;

 __cur_tsk = sav_cur_tsk;
 /* statement here is the pointer to the task */
 return(stp);
}

/*
 * find a matching end block (end or join)
 */
extern void __find_matchendblk(int32 btok, int32 endbtok)
{
 int32 blklev = 0;

 for (;;)
  {
   if (__syncto_class != SYNC_STMT) return;
   if (__vskipto2_any(btok, endbtok))
    {
     if (__toktyp == endbtok) { if (blklev <= 0) return; blklev--; }
     else blklev++;
    }
  }
}

/*
 * read if statement - if keyword read, reads statement end token
 */
static struct st_t *rd_if(void)
{
 struct st_t *stp, *thenstp, *elsestp;
 struct expr_t *ifxnd;
 int32 slcnt, sfnind;

 slcnt = __lin_cnt;
 sfnind = __cur_fnam_ind;
 __get_vtok();
 if (__toktyp != LPAR)
  {
   __pv_ferr(1064, "if not followed by required left parenthesis - %s read",
    __prt_vtok());
sync_xpr:
   __vskipto3_any(RPAR, ELSE, SEMI);
   if (__syncto_class == SYNC_TARG)
    {
     __set_xtab_errval();
     __bld_xtree(0);
     ifxnd = __root_ndp;
     if (__toktyp == RPAR) goto rd_then;
     thenstp = __alloc_stmt(S_NULL);
     if (__toktyp == ELSE) goto rd_else;
     /* skipped to ; */
     goto maybe_else;
    }
   if (__syncto_class == SYNC_STMT)
    {
     __set_xtab_errval();
     __bld_xtree(0);
     ifxnd = __root_ndp;
     goto rd_then;
    }
   /* something completely out of place */
   __get_vtok();
   return(NULL);
  }
 __get_vtok();
 /* collection routines that fail must emit error */
 if (!__col_parenexpr(-1))
  {
   goto sync_xpr;
  }
 /* need to build and save expr */
 __bld_xtree(0);
 ifxnd = __root_ndp;

rd_then:
 __get_vtok();
 if ((thenstp = __rd_stmt()) == NULL)
  {
   if (__syncto_class == SYNC_STMT || (__syncto_class == SYNC_TARG
    && __toktyp == SEMI))
    {
     thenstp = __alloc_stmt(S_NULL);
     goto maybe_else;
    }
   /* anything else - return and caller syncs */
   return(NULL);
  } 
maybe_else:
 __get_vtok();
 if (__toktyp == ELSE)
  {
rd_else:
   __get_vtok();
   if ((elsestp = __rd_stmt()) == NULL) return(NULL);
  }
 /* should try to get rid of this unget */
 else
  {
   /* unget needed because routines expect to read 1st st. token */
   if (__iact_state) 
    {
     __pv_err_cnt++; 
     __ia_err(1432, "interactive if statement required else missing");
     return(NULL);
    }
   __unget_vtok();
   elsestp = NULL;
  }
 stp = __alloc_stmt(S_IF);
 /* correct to line of if */
 stp->stlin_cnt = slcnt;
 stp->stfnam_ind = sfnind;

 stp->st.sif.condx = ifxnd;
 stp->st.sif.thenst = thenstp;
 stp->st.sif.elsest = elsestp;
 return(stp);
}

/*
 * know case/casex,casez keyword read and reads endcase
 */
static struct st_t *rd_case(int32 casttyp)
{
 struct st_t *casp, *dflsp, *stp;
 struct csitem_t *cip, *last_cip, *csihdr, *dflt_csip;
 struct expr_t *csndp;
 struct exprlst_t *csixhdr, *xplp, *last_xplp;
 int32 slcnt, sfnind;

 slcnt = __lin_cnt;
 sfnind = __cur_fnam_ind;

 __get_vtok();
 if (__toktyp != LPAR)
  {
   __pv_ferr(1065,
    "case statement selection expression left parenthesis expected - %s read",
    __prt_vtok());
bad_csx:
   if (__vskipto2_any(RPAR, SEMI)) { __set_xtab_errval(); goto bld_csx; }
   return(NULL);
  }
 __get_vtok();
 if (!__col_parenexpr(-1)) goto bad_csx;
bld_csx:
 __bld_xtree(0);
 csndp = __root_ndp;
 for (last_cip = NULL, csihdr = NULL, dflsp = NULL;;)
  {
   __get_vtok();
   switch ((byte) __toktyp) {
    case ENDCASE: goto bld_case;
    case DEFAULT:
do_dflt:
     __get_vtok();
     if (__toktyp == COLON) __get_vtok();
     if (dflsp != NULL)
      __pv_ferr(1067, "more than one case default item not permitted");
     if ((dflsp = __rd_stmt()) == NULL)
      {
       if (__vskipto_any(SEMI)) continue;
       /* here must skip to endcase of right level and return */
give_up:
       __find_matchendcase();
       return(NULL);
      }
     continue;
    default:
     /* expr. collection will fail if not required one expr. */
     for (last_xplp = NULL, csixhdr = NULL;;)
      {
more_xprs:
       if (!__col_caseexpr())
        {
         if (__vskipto3_any(COLON, SEMI, COMMA))
          {
           if (__toktyp == SEMI) goto nxt_case;
           __set_xtab_errval();
          }
         else goto give_up;
        }
       __bld_xtree(0);
       /* allocate and link in the expression list item */
       xplp = __alloc_xprlst();
       xplp->xp = __root_ndp;
       if (last_xplp == NULL) csixhdr = xplp; else last_xplp->xpnxt = xplp;
       last_xplp = xplp;

       if (__toktyp != COMMA) break;
       __get_vtok();
      }
     if (__toktyp != COLON)
      {
       __pv_ferr(1068, "case item expression list colon expected - %s read",
        __prt_vtok());
       if (__vskipto3_any(COLON, SEMI, COMMA))
        {
         if (__toktyp == SEMI) continue;
         if (__toktyp == COLON) goto do_cstmt;
         goto more_xprs;
        }
       else goto give_up;
      }
do_cstmt:
     __get_vtok();
     /* ; null statement ok here */
     if ((casp = __rd_stmt()) == NULL)
      {
       if (__vskipto2_any(SEMI, DEFAULT))
        {
         if (__toktyp == DEFAULT) goto do_dflt;
         continue;
        }
       else goto give_up;  
      }
     /* allocate and link in the case item element */
     cip = __alloc_csitem();
     cip->csixlst = csixhdr;
     cip->csist = casp;
     if (last_cip == NULL) csihdr = cip; else last_cip->csinxt = cip;
     last_cip = cip;
   }
nxt_case:;
  }
bld_case:
 stp = __alloc_stmt(S_CASE);
 /* correct to line of case */
 stp->stlin_cnt = slcnt;
 stp->stfnam_ind = sfnind;

 stp->st.scs.castyp = casttyp;
 stp->st.scs.csx = csndp;

 /* SJM 08/27/99 - change so no default in case rec but first on item list */
 dflt_csip = __alloc_csitem();
 dflt_csip->csixlst = NULL;
 /* no default: indicated by first nil */
 if (dflsp != NULL) dflt_csip->csist = dflsp; else dflt_csip->csist = NULL;

 /* link onto front of list */
 dflt_csip->csinxt = csihdr;
 stp->st.scs.csitems = dflt_csip;
 return(stp);
}

/*
 * find a matching endcase
 */
extern void __find_matchendcase(void)
{
 int32 caselev = 0;
 for (;;)
  {
   if (__syncto_class != SYNC_STMT) return;
   if (__vskipto4_any(ENDCASE, CASE, CASEX, CASEZ))
    {
     if (__toktyp == ENDCASE) { if (caselev <= 0) return; caselev--; }
     else caselev++;
    }
   else return;
  }
}

/*
 * allocate an expression list element
 */
extern struct exprlst_t *__alloc_xprlst(void)
{
 struct exprlst_t *xplp;

 xplp = (struct exprlst_t *) __my_malloc(sizeof(struct exprlst_t));
 xplp->xp = NULL;
 xplp->xpnxt = NULL;
 return(xplp);
}

/*
 * allocate a case item - case statement is list of case items
 */
extern struct csitem_t *__alloc_csitem(void)
{
 struct csitem_t *cip;

 cip = (struct csitem_t *) __my_malloc(sizeof(struct csitem_t));
 cip->csixlst = NULL;
 cip->csist = NULL;
 cip->csinxt = NULL;
 return(cip);
}

/*
 * read a simple (non for) loop statement
 */
static struct st_t *rd_loop(int32 ttyp)
{
 struct st_t *stp, *repstp;
 struct expr_t *loopx;
 char s1[RECLEN];

 /* notice this is done at 1st statement so no line number fix up */
 if (ttyp == REPEAT) stp = __alloc_stmt(S_REPEAT);
 else if (ttyp == WAIT) stp = __alloc_stmt(S_WAIT); 
 else stp = __alloc_stmt(S_WHILE);
 loopx = NULL;
 repstp = NULL;

 switch ((byte) ttyp) {
  case FOREVER:
   /* first build proc assign */
   stp->stmttyp = S_FOREVER;
   loopx = NULL;
   __get_vtok();
   if ((repstp = __rd_stmt()) == NULL) return(NULL);
   break;
  case REPEAT: 
   stp->stmttyp = S_REPEAT;
   goto get_expr;
  case WHILE:
   stp->stmttyp = S_WHILE;
   goto get_expr;
  case WAIT:
   stp->stmttyp = S_WAIT;
    /* here because wait expression globals need special xmrtype fields */
get_expr:
   __get_vtok();
   if (__toktyp != LPAR)
    {
     __pv_ferr(1070,
      "%s loop statement conditional expression not preceded by left parenthesis - %s read",
      __get_vkeynam(s1, ttyp), __prt_vtok());
sync_lpx:
     if (__vskipto2_any(SEMI, RPAR))
      {
       if (__toktyp == RPAR) {  __set_xtab_errval(); goto bldx; }
       __syncto_class = SYNC_STMT;
      }
     /* here have synced to beginning of next statement */
     return(NULL);
    }
   __get_vtok();
   if (!__col_parenexpr(-1)) goto sync_lpx;
bldx:
   __bld_xtree(0);
   loopx = __root_ndp;
   __get_vtok();
   if ((repstp = __rd_stmt()) == NULL) return(NULL);
   break;
   default: __case_terr(__FILE__, __LINE__);
  }
 /* repeat tmp per inst. storage filed not set till preparation phase */
 if (stp->stmttyp == S_REPEAT)
  { stp->st.srpt.repx = loopx; stp->st.srpt.repst = repstp; }
 else if (stp->stmttyp == S_WAIT)
  { stp->st.swait.lpx = loopx; stp->st.swait.lpst = repstp; }
 else { stp->st.swh.lpx = loopx; stp->st.swh.lpst = repstp; }
 return(stp);
}

/*
 * read a for statement
 * know for token read and reads end of for body
 * notice this returns 2 statement short list (since unnamed begin 
 * returns list - this always is ok)
 */
static struct st_t *rd_for(void)
{
 struct for_t *frs;
 struct st_t *stp, *inita, *inca, *forbd;
 struct expr_t *lhsndp, *rhsndp, *stopndp;
 int32 slcnt, sfnind;

 slcnt = __lin_cnt;
 sfnind = __cur_fnam_ind;

 /* must read expressions separately for assigns */
 /* first build initialization assign */
 __get_vtok();
 if (__toktyp != LPAR)
  {
   __pv_ferr(1071,
    "for statement left parenthesis expected - %s read", __prt_vtok());
bad_for:
   if (__vskipto2_any(RPAR, SEMI))
    {
     if (__toktyp == SEMI) { inita = __alloc_stmt(S_NULL); goto bldtrmx; }
    }
rd_forst:
   /* if module or file level item, just return caller will resync */
   if (__syncto_class == SYNC_STMT || __syncto_class == SYNC_TARG)
    {
     __get_vtok();
     __rd_stmt();
    }
   return(NULL);
  }
 __get_vtok();
 /* collect lhs to = */
 if (!__col_lval()) goto bad_for;
 __bld_xtree(0);
 lhsndp = __root_ndp;
 __get_vtok();
 if (!__col_comsemi(-1)) goto bad_for;
 if (__toktyp != SEMI)
  {
   __pv_ferr(1076,
    "for statement initial assignment ending semicolon expected - %s read",
    __prt_vtok());
   __vskipto_any(RPAR);
   goto rd_forst;
  }

 __bld_xtree(0);
 rhsndp = __root_ndp;
 /* build proc assign */
 inita = __alloc_stmt(S_FORASSGN);
 inita->st.spra.lhsx = lhsndp;
 inita->st.spra.rhsx = rhsndp;

 /* build termination expression */
bldtrmx:
 __get_vtok();
 if (!__col_comsemi(-1))
  {
   if (__vskipto2_any(RPAR, SEMI))
    {
     if (__toktyp == SEMI) { __set_xtab_errval(); goto bldcondx; } 
    }
   goto rd_forst;
  }
 if (__toktyp != SEMI)
  {
   __pv_ferr(1078,
    "for statement termination expression ending semicolon expected - %s read",
    __prt_vtok());
   __vskipto_any(RPAR);
   goto rd_forst;
  }

bldcondx:
 __bld_xtree(0);
 stopndp = __root_ndp;

 /* collect assignment ending in ) */
 __get_vtok();
 /* collect lhs to = - this can only end with = */
 if (!__col_lval())
  {
bad2_for:
   if (__vskipto_any(RPAR)) { inca = __alloc_stmt(S_NULL); goto do_body; }
   goto rd_forst;
  }
   
 __bld_xtree(0);
 lhsndp = __root_ndp;
 __get_vtok();
 if (!__col_parenexpr(-1)) goto bad2_for;
 __bld_xtree(0);
 rhsndp = __root_ndp;

 /* build proc assign */
 inca = __alloc_stmt(S_PROCA);
 inca->st.spra.lhsx = lhsndp;
 inca->st.spra.rhsx = rhsndp;

do_body:
 __get_vtok();
 if ((forbd = __rd_stmt()) == NULL) return(NULL);

 stp = __alloc_stmt(S_FOR);
 /* correct to line of for */
 stp->stlin_cnt = slcnt;
 stp->stfnam_ind = sfnind;

 frs = stp->st.sfor;
 frs->forassgn = inita;
 frs->fortermx = stopndp;
 frs->forinc = inca;
 frs->forbody = forbd;
 /* notice for assign (initializer) inserted before for but for assgn */
 /* still points back to it */
 inita->stnxt = stp;
 return(inita);
}

/*
 * read a -> cause statement, know -> read and read ending ;
 */
static struct st_t *rd_cause(void)
{
 struct st_t *stp;
 struct sy_t *syp;

 __get_vtok();
 if (!__col_comsemi(-1))
  {
skp_end:
   __vskipto_any(SEMI);
   return(NULL);
  }
 if (__toktyp != SEMI)
  {
   __pv_ferr(1072, "cause statement semicolon expected - %s read",
    __prt_vtok());
   goto skp_end;
  }
 __bld_xtree(0);
 /* this error does not effect synchronization */
 if (__root_ndp->optyp != ID && __root_ndp->optyp != GLBREF)
  __pv_ferr(1073, "cause statement event name expected");
 if (__toktyp == ID)
  {
   syp = __root_ndp->lu.sy;
   /* if net wire and not declared, convert to event wire */
   if (syp->sytyp == SYM_N && !syp->sydecl) syp->el.enp->ntyp = N_EVENT;
   /* declaration errors caught later */
  }
 stp = __alloc_stmt(S_CAUSE);
 stp->st.scausx = __root_ndp;
 return(stp);
}

/*
 * read a disable statement - must be check later
 * know disable read
 */
static struct st_t *rd_disable(void)
{
 struct st_t *stp;
 struct expr_t *dsxndp;
 struct expridtab_t *xidp;
 
 __get_vtok();
 if (!__col_comsemi(-1))
  {
   __vskipto_any(SEMI);
   return(NULL);
  }
 if (__toktyp != SEMI)
  {
   __pv_ferr(1129, "disable statement semicolon expected - %s read",
    __prt_vtok());
   return(NULL);
  }
 /* tricky code for simple local just ID task/named block disable */
 /* can not parse because will be found incorrectly as local wire */
 /* this is side effect of stupid xmr as expression new feature */
 /* ending fence undef not yet added because bld xtree not called */ 
 if (__last_xtk == 0 && __exprtab[0]->optyp == ID)
  {
   /* must allocate name of task or fixup if previously used */
   xidp = __expr_idtab[0];
   if ((dsxndp = find_bldxpr_tfsy(xidp->idnam, UNDEF)) == NULL) return(NULL);

   /* also can't disable funcs - because run in no time but caught later */
   stp = __alloc_stmt(S_DSABLE);
   stp->st.sdsable.dsablx = dsxndp;
   return(stp);
  }
 /* this must be xmr - because xmr now expressions must parse to construct */
 __bld_xtree(0);
 dsxndp = __root_ndp;
 if (dsxndp->optyp != GLBREF)
  {
   __pv_ferr(1074,
    "disable statement hierarchical named block or task name reference expected - %s read",
    __msgexpr_tostr(__xs, dsxndp));
   return(NULL);
  }
 stp = __alloc_stmt(S_DSABLE);
 stp->st.sdsable.dsablx = dsxndp;
 return(stp);
}

/*
 * find (and maybe add) task symbol - only called for enable/disable
 * know definition at top mod level and non gref
 * may get here on named block disable (tfsytyp UNDEF) or enable
 */
static struct expr_t *find_bldxpr_tfsy(char *nam, int32 tfsytyp)
{
 struct expr_t *enable_ndp;
 struct sy_t *syp;

 /* if id declared in currently accessible name evironment use it */
 if ((syp = __get_sym_env(nam)) != NULL)
  {
   /* if disable must check in disable statement fixup */
   if (tfsytyp != UNDEF)
    {
     /* error if declared or used as non task */
     /* illegal to enable or disable a function */
     switch ((byte) syp->sytyp) {
      case SYM_F: case SYM_I: case SYM_M: case SYM_PRIM: case SYM_UDP:
      case SYM_N: case SYM_DEF:
       {
	char s1[RECLEN], s2[RECLEN];

        if (syp->sydecl) strcpy(s1, "declared"); else strcpy(s1, "used");
        __to_sytyp(s2, syp->sytyp);
        __pv_ferr(963, "task or named block %s previously %s as %s at %s",
	 syp->synam, s1, s2, __bld_lineloc(__xs, syp->syfnam_ind,
         syp->sylin_cnt));
        return(NULL);
       }
     }
    }
   /* rest of fields gets filled later - notice no width not in expr */
   enable_ndp = __alloc_newxnd();
   enable_ndp->optyp = ID;
   enable_ndp->lu.sy = syp;
   return(enable_ndp);
  }
 /* error if not declared in interactive command input */
 if (__iact_state)
  {
   __pv_err_cnt++;
   __ia_err(1435, "task or named block %s undeclared", nam);
   return(NULL);
  }

 enable_ndp = __alloc_newxnd();
 enable_ndp->optyp = ID;

 /* here building the enable so just need to pass place holder - for fcall */
 /* need the parsed expr where its insides are changed to xmr */
 cnvt_forw_tfcall_1cmpglb(enable_ndp, nam, __cur_fnam_ind, __lin_cnt); 
 return(enable_ndp);
}

/*
 * read proc. quasi-cont assign - normal wire assign but only enable when
 * time token moves here, force for wires and 2nd level of qc assign
 */
static struct st_t *rd_wireassign(int32 qcattyp)
{
 struct st_t *stp;
 struct expr_t *lhsndp, *rhsndp;

 /* allocate at top (wastes storage on error) to get line no. right */
 stp = __alloc_stmt(S_QCONTA);
 /* collect lhs to = */
 __get_vtok();
 if (!__col_lval())
  {
bad_qassgn:
   __vskipto_any(SEMI);
   return(NULL);
  }
 __bld_xtree(0);
 /* must check later to make sure lvalue */
 lhsndp = __root_ndp;

 __get_vtok();
 if (!__col_comsemi(-1)) goto bad_qassgn;
 if (__toktyp != SEMI)
  {
   char s1[RECLEN];

   if (qcattyp == FORCE) strcpy(s1, "force");
   else if (qcattyp == ASSIGN) strcpy(s1, "quasi-continuous assign");
   else __case_terr(__FILE__, __LINE__);

   __pv_ferr(1076, "%s statement ending semicolon expected - %s read", s1,
    __prt_vtok());
   goto bad_qassgn;
  }
 __bld_xtree(0);
 rhsndp = __root_ndp;

 /* build quasi cont. assign */
 stp->st.sqca->qcatyp = qcattyp;
 /* assume reg. form - requried for assign/deassign */
 stp->st.sqca->regform = TRUE;
 stp->st.sqca->qclhsx = lhsndp;
 stp->st.sqca->qcrhsx = rhsndp;
 stp->st.sqca->rhs_qcdlstlst = NULL;
 return(stp);
}

/*
 * read wire deassign/release (difference is force arg. can be wire)
 * form is deassign/release [lvalue];
 */
static struct st_t *rd_wiredeassign(int32 qcdeattyp)
{
 struct st_t *stp;

 /* allocate at top (wastes storage on error) to get line no. right */
 stp = __alloc_stmt(S_QCONTDEA);
 /* collect lhs to = */
 __get_vtok();
 if (!__col_comsemi(-1))
  {
skp_end:
   __vskipto_any(SEMI);
   return(NULL);
  }
 if (__toktyp == COMMA)
  {
   char s1[RECLEN];

   __pv_ferr(1077, "%s statement ending semicolon expected - %s read",
    __get_vkeynam(s1, qcdeattyp), __prt_vtok());
   goto skp_end;
  }
 __bld_xtree(0);

 /* build quasi cont. assign */
 stp->st.sqcdea.qcdatyp = qcdeattyp;
 /* assume reg. form */
 stp->st.sqcdea.regform = TRUE;
 stp->st.sqcdea.qcdalhs = __root_ndp;
 return(stp);
}

/*
 * read and build task enable or procedural assignment statement
 *
 * expects 1st lhs element (ID or '{')to have been read
 * reads stmt ending ;
 * 
 * for assign builds blocking or non blocking rhs del ctrl assign
 * uses 1st token as location for both delay control and action stmt
 *
 * notice for assign not read here - lhs and rhs read separately 
 * notice recovery here can just be skip to ; [if correct must appear]
 */
static struct st_t *rd_taske_or_proc_assign(void)
{
 int32 dtyp, is_nb, slcnt, sfnind, is_evctl_impl;
 struct st_t *stp, *dcstp;
 struct expr_t *lhsndp, *rhsndp, *delxndp, *repcntx;
 struct delctrl_t *dctp;
 struct paramlst_t *pmp;
 char s1[IDLEN];

 slcnt = __lin_cnt;
 sfnind = __cur_fnam_ind;

 /* collect lhs to '=' or '(' or ';' (for task enable) else fails */
 if (!col2_lval())
  {
bad_assgn:
   __vskipto_any(SEMI);
   return(NULL);
  }
 /* case 1: ID task enable, not parsed need special task sy decl */
 if (__toktyp == LPAR || __toktyp == SEMI)
  { 
   if (__last_xtk != 0 || __exprtab[0]->optyp != ID) goto try_parsing;
   /* on error returns nil and that is also returned from here */ 
   /* notice must copy expr idtab value because overwritten by parsing */
   strcpy(s1, __expr_idtab[0]->idnam);
   stp = __rd_tskenable(s1, NULL, FALSE);
   return(stp);
  }

try_parsing:
 __bld_xtree(0);
 lhsndp = __root_ndp;
 /* case 1a: xmr task enable */
 if (__toktyp == LPAR || __toktyp == SEMI)
  {
   /* error if enable anything but xmr */
   if (lhsndp->optyp != GLBREF)
    { 
     __pv_ferr(1086,
      "attempted task enable of %s - maybe assignment = missing",
      __msgexpr_tostr(__xs, lhsndp));
     goto bad_assgn;
    }
   stp = __rd_tskenable(NULL, lhsndp, TRUE);
   return(stp);
  }
 
 /* case 2: assign */
 /* for now block procedural assignment not supported */
 if (__toktyp == RELLE) is_nb = TRUE; else is_nb = FALSE;
 /* must check later to make sure lvalue */

 __get_vtok();

 /* handle optional rhs delay control */
 /* 10/27/00 SJM - add support for rhs repeat delay control forms */ 
 repcntx = NULL;
 if (__toktyp == REPEAT)
  {
   __get_vtok();
   /* surrounding parentheses required by syntax */
   if (__toktyp != LPAR)
    {
     __pv_ferr(3413,
      "repeat event control count expression beginning left parenthesis expected - %s read",  
       __prt_vtok());
     goto bad_assgn;
    }
   __get_vtok();
   /* this read trailing left parenthsis - here syncing only to ; */
   if (!__col_parenexpr(-1)) goto bad_assgn;

   __bld_xtree(0);
   repcntx = __root_ndp;
   /* read the delay control so can sync to normal rhs event control case */
   __get_vtok();
   if (__toktyp != AT)
    {
     __pv_ferr(3414,
      "repeat event control count expression not followed by event control at sign - %s read",  
      __prt_vtok());
     goto bad_assgn;
    }
   /* set flag and fall thru - now same as normal rhs event control */
  }
 
 dtyp = DC_NONE;
 if (__toktyp == SHARP || __toktyp == AT)
  {
   /* know this reads one past end of delay control */
   if ((delxndp = rd_delctrl(&dtyp, &is_evctl_impl)) == NULL)
    {
     if (dtyp == DC_EVENT && is_evctl_impl)
      {
       __pv_ferr(3427,
        "right hand side event control implicit form @(*) illegal");
      }
     goto bad_assgn;
    }
   if (dtyp == DC_EVENT) dtyp = DC_RHSEVENT;
   else if (dtyp == DC_DELAY) dtyp = DC_RHSDELAY;
  }
 else delxndp = NULL;

 /* this reads end ; */
 if (!__col_comsemi(-1)) goto bad_assgn;
 if (__toktyp != SEMI)
  {
   __pv_ferr(1078,
    "assignment statement ending semicolon expected - %s read", __prt_vtok());
   goto bad_assgn;
  }

 __bld_xtree(0);
 rhsndp = __root_ndp;

 /* first build proc assign */
 if (!is_nb)
  {
   if (dtyp == DC_RHSEVENT || dtyp == DC_RHSDELAY)
    stp = __alloc_stmt(S_RHSDEPROCA);
   else stp = __alloc_stmt(S_PROCA);
  }
 else stp = __alloc_stmt(S_NBPROCA);
 stp->st.spra.lhsx = lhsndp;
 stp->st.spra.rhsx = rhsndp;
 stp->stlin_cnt = slcnt;
 stp->stfnam_ind = sfnind;

 if (delxndp == NULL) return(stp);

 dcstp = __alloc_stmt(S_DELCTRL);
 dcstp->stlin_cnt = slcnt;
 dcstp->stfnam_ind = sfnind;

 dctp = dcstp->st.sdc;
 dctp->dctyp = dtyp;
 dctp->dc_delrep = DT_CMPLST;
 pmp = __alloc_pval();
 pmp->plxndp = delxndp; 
 dctp->dc_du.pdels = pmp;
 /* 10/28/00 SJM - only indication of rep form is repeat cnt expr non nil */
 dctp->repcntx = repcntx;
 /* 06/11/02 SJM - need to indicate non blocking rhs ev ctrl in dc evnt */  
 if (is_nb) dctp->dc_nblking = TRUE;

 dctp->dceschd_tevs = NULL;

 dctp->actionst = stp;
 return(dcstp);
}

/*
 * read a task enable (call) statement
 *
 *  '(' or ';' (if no args read and reads ending ;
 * need special parsing for system task since ,, form legal there
 */
extern struct st_t *__rd_tskenable(char *tknam, struct expr_t *glbndp,
 int32 is_glbenable)
{
 register int32 i;
 int32 rd_semi, nd_glb_conv;
 struct sy_t *syp;
 struct st_t *stp;
 struct tskcall_t *tkcp;
 struct expr_t *last_fcomxp, *lop;
 struct expr_t *enable_ndp;
 struct systsk_t *stbp;

 rd_semi = FALSE;
 nd_glb_conv = FALSE;
 stbp = NULL;
 /*  case 1: parsed global ref enable */
 if (is_glbenable)
  {
   stp = __alloc_stmt(S_TSKCALL);
   tkcp = &(stp->st.stkc);
   /* notice there is no tkexp lu. sy here */
   tkcp->tsksyx = glbndp;
   tknam = glbndp->ru.grp->gnam;
   goto get_args;
  }

 /* DBG remove --- */
 if (tknam == NULL) __arg_terr(__FILE__, __LINE__);
 /* --- */ 
 /* case 2 simple ID enable - not parsed */
 if (*tknam == '$')
  {
   /* look up in special system func. and task symbol table */
   if ((syp = __get_sym(tknam, __syssyms)) == NULL)
    {
     /* this is completely unknown case */
     __pv_ferr(1083,
      "task enable of unknown system task or undefined PLI task \"%s\"",
      tknam);
     goto err_end;
    }
   stbp = syp->el.esytbp;
   if (syp->sytyp != SYM_STSK)
    {
     __pv_ferr(1080, "task enable of system function \"%s\" illegal",
      syp->synam);
     goto err_end;
    }
   /* systam task table inconsistent - value is 0 */
   if (stbp->stsknum == 0) __misc_fterr(__FILE__, __LINE__);

   /* need location for xmr type setting */
   switch (stbp->stsknum) {
    case STN_MONITOR: case STN_MONITORB: case STN_MONITORH: case STN_MONITORO:
     if (__iact_state && __optimized_sim)
      {
opt_dbg_illegal:
       /* SJM 01/02/03 - can't call $dumpvars system task from iact */
       /* from interactive debugger if -O on */
       __pv_ferr(2901,
        "system task %s can't be called from debugger when optimizer on - run without -O",
        syp->synam);
       goto err_end;
      }
     break;
    /* these can have module/inst. ending xmrs */
    case STN_DUMPVARS:
     if (__iact_state && __optimized_sim) goto opt_dbg_illegal;
     nd_glb_conv = TRUE;
     break;
    case STN_SDF_ANNOTATE:
    case STN_PRINTTIMESCALE: case STN_SCOPE: case STN_LIST:
     nd_glb_conv = TRUE;
     break;
    default:
     /* PLI system tasks/functions always allow XMR's */
     if (stbp->stsknum >= BASE_VERIUSERTFS
      && (int32) stbp->stsknum <= __last_systf) nd_glb_conv = TRUE;
     break;
   }
   enable_ndp = __alloc_newxnd();
   enable_ndp->optyp = ID;
   enable_ndp->lu.sy = syp;
  }
 else
  {
   /* if user task not declared must add to symbol table */
   /* return of NULL to here on error - message already written */
   if ((enable_ndp = find_bldxpr_tfsy(tknam, SYM_TSK)) == NULL)
    {
err_end:
     __vskipto_any(SEMI);
     return(NULL);
    }
  }
 /* build the task enable statement */
 stp = __alloc_stmt(S_TSKCALL);
 tkcp = &(stp->st.stkc);
 tkcp->tsksyx = enable_ndp;

get_args:
 tkcp->targs = NULL;
 /* no arguments */
 if (__toktyp == SEMI) goto done;

 /* build argument expressions in a list */
 __get_vtok();
 /* at this point ,, form ok (legal for system tasks? */
 for (last_fcomxp = NULL, i = 0;; i++)
  {
   /* ,, or ,) need to be check/fixed later */
   /* for display and other system task legal, but for user task error */
   if (__toktyp == COMMA || __toktyp == RPAR)
    {
     /* for xmr task enable, point to gnam - need for sys task testing */
     /* error messages */
     if (*tknam != '$')
      {
       __pv_ferr(1081,
        "user task %s enable empty '()' or ',,' argument form illegal (pos. %d)",
        tknam, i + 1);
       /* if error build rhs x, cannot be op empty for non system task */
       __set_xtab_errval();
      }
     else { __last_xtk = 0; __set_opempty(0); }
     goto do_parse;
    }
   /* by case code to allow special forms, made globals here */
   if (nd_glb_conv)
    {
     switch (stbp->stsknum) {
      /* for dumpvars all but 1st argument can be special form */
      case STN_DUMPVARS:
       if (i != 0) __allow_scope_var = TRUE;
       break; 
      case STN_SDF_ANNOTATE:
       if (i == 1) __allow_scope_var = TRUE;
       break;
      /* case where all inst. forms */
      case STN_SCOPE: case STN_LIST: case STN_PRINTTIMESCALE:
       __allow_scope_var = TRUE;
       break;
      default:
       /* any argument to PLI system task can be xmr */
       if (stbp->stsknum >= BASE_VERIUSERTFS
        && (int32) stbp->stsknum <= __last_systf)
        {
         __allow_scope_var = TRUE;
        }
       else __case_terr(__FILE__, __LINE__);
     }
    }
   /* either path here turns off allowing scope var */
   if (!__col_connexpr(-1))
    {
     __allow_scope_var = FALSE;
     if (__vskipto3_any(COMMA, RPAR, SEMI))     
      { if (__toktyp == SEMI) rd_semi = TRUE; }
     else goto err_end;
     /* if error build rhs x, then parse */
     __set_xtab_errval();
    }

do_parse:
   __bld_xtree(0);
   /* parsing in bld xtree used this flag to allow inst. ending global */
   /* so now must turn off in case turned on */
   __allow_scope_var = FALSE;

   /* first arg. done for dumpvars - rest special inst xmr or wire form */
   lop = __alloc_newxnd();
   lop->optyp = FCCOM;
   lop->ru.x = NULL;
   lop->lu.x = __root_ndp;

   if (last_fcomxp == NULL) tkcp->targs = lop; else last_fcomxp->ru.x = lop;
   last_fcomxp = lop;
   /* some kind of error, seen ;, all done - probably more errors */
   if (__toktyp == rd_semi) goto done;
   if (__toktyp == RPAR) break;
   __get_vtok();
  }
 /* assume just left out */
 __get_vtok();
 if (__toktyp != SEMI)
  {
   __pv_ferr(1084,
    "task enable statement semicolon expected - %s read", __prt_vtok());
  }
done:
 return(stp);
}

/*
 * read a delay/event control statement (non rhs)
 */
static struct st_t *rd_dctrl_st(void)
{
 int32 dtyp, slcnt, sfnind, is_evctl_impl;
 struct st_t *dcstp, *stp;
 struct expr_t *delxndp;
 struct delctrl_t *dctp;
 struct paramlst_t *pmp;

 slcnt = __lin_cnt;
 sfnind = __cur_fnam_ind;
 is_evctl_impl = FALSE;
 /* this reads one past one control or one past ending ) */
 /* only NULL error if cannot sync to following stmt */
 if ((delxndp = rd_delctrl(&dtyp, &is_evctl_impl)) == NULL)
  {
   /* SJM 06/01/04 - @(*) or @* forms - have nil delxndp but still bld */
   if (!is_evctl_impl) return(NULL);
  }
 dcstp = __alloc_stmt(S_DELCTRL);
 /* fix up so statement location starts with delay control not action stmt */
 dcstp->stlin_cnt = slcnt;
 dcstp->stfnam_ind = sfnind;

 dctp = dcstp->st.sdc;
 dctp->dctyp = dtyp;
 dctp->dc_delrep = DT_CMPLST;
 pmp = __alloc_pval();
 pmp->plxndp = delxndp; 
 dctp->dc_du.pdels = pmp;
 dctp->dceschd_tevs = NULL;
 /* SJM 06/01/04 - in this case del xndp nil - build ev list during fixup */
 dctp->implicit_evxlst = is_evctl_impl; 

 /* if begin-end block will return statement list */
 if ((stp = __rd_stmt()) == NULL) return(NULL);
 /* need some statement here even if it is only NULL statement */
 /* delay control on block turned into delay control on st. list */
 dctp->actionst = stp;
 return(dcstp);
}

/*
 * read a delay control @ or # read - builds expression and type
 * know first token read and reads one past ending token
 *
 * this must have skipped past delay control to stmt if possible 
 */
static struct expr_t *rd_delctrl(int32 *dtyp, int32 *ev_impl)
{
 struct expr_t *ndp;

 *ev_impl = FALSE;
 /* read one after except for non parenthesized ID because there need to */
 /* read one ahead to see if xmr */
 if (__toktyp == SHARP) *dtyp = DC_DELAY;
 else
  {
   *dtyp = DC_EVENT;
   __canbe_impl_evctrl = TRUE; 
  }
 __get_vtok();

 if (__toktyp == LPAR)
  {
   /* if @(* that looks like attribute to scanner - glb canbe impl evctrl */
   /* on will prevent seeing as atrribute since attrs illegal here */
   __get_vtok();
   __canbe_impl_evctrl = FALSE;

   if (*dtyp == DC_DELAY)
    {
     /* undeclared names treated as wires - maybe changed to event later */
     /* need to surround with () in expr. so m:t:m parses ok */
     /* but need to use actual surrounding for parse indicators */
     __last_xtk = -1;
     ndp = __alloc_exprnd();
     ndp->optyp = LPAR;
     /* if no error, surround with parentheseses and parse as normal */
     if (!__col_parenexpr(0))
      {
       __pv_ferr(1278, "delay control expression error");

       __vskipto_any(RPAR);
       if (__syncto_class == SYNC_TARG || __syncto_class == SYNC_STMT) 
        {
bad_dctrl:
         /* this is 1 bit x - cannot be op empty */
         __set_xtab_errval();
         __get_vtok();
         goto bld_evx;
        }
       __get_vtok();
       return(NULL);
      }
     ndp = __alloc_exprnd();
     ndp->optyp = RPAR;
    }
   else
    {
     if (__toktyp == TIMES) 
      {
       __get_vtok();
       if (__toktyp != RPAR)
        {
         __pv_ferr(3428, "implicit @(*) event control form illegal - %s read",
          __prt_vtok());
         __vskipto_any(RPAR);
         if (__syncto_class == SYNC_TARG || __syncto_class == SYNC_STMT) 
          return(NULL);
         else { __get_vtok(); return(NULL); }
        }
       *ev_impl = TRUE;
       __get_vtok();
       return(NULL);
      }  

     /* event control cannot have added surrounding () because of evor */
     if (!col_evctrlexpr())
      {
       __pv_ferr(1087, "event control expression error");
       /* if possible, now ready to read statement */
       __vskipto_any(RPAR);
       if (__syncto_class == SYNC_TARG || __syncto_class == SYNC_STMT) 
        goto bad_dctrl; 
       __get_vtok();
       return(NULL);
      }
    }
   /* on any non error surrounding () case, read one after end ) */
   __get_vtok();
  }
 else
  {
   /* turn off glb to prevent seeing @(* as scanner attribute */
   __canbe_impl_evctrl = FALSE;

   /* simple case can be one name form */
   /* should really check to see if next token can start stmt */
   __last_xtk = -1;
   /* this declares ID as wire - maybe later declared to be event */
   /* LOOKATME - could collect xmr without surrounding () by */
   /* looking for each .ID(optional [...]) after first */ 
   switch ((byte) __toktyp) {
    case NUMBER: case REALNUM: case ID: break;
    /* only create IS number at param assign */ 
    case TIMES:
     /* AIV 07/21/04 - handle @* without () */
     *ev_impl = TRUE;
      __get_vtok();
     return(NULL);
    case ISNUMBER: case ISREALNUM:
    /* here on error, assume right structure but 1 thing wrong */
     __arg_terr(__FILE__, __LINE__);
     break;
    default:
     /* one token after delay/event control case - no sync possible */
     __pv_ferr(1088,
     "non parenthesized delay/event control not identifier or number - %s read",
      __prt_vtok());
     goto bad_dctrl;
    }
   if (__toktyp == ID)
    {
     /* even if simple ID (i.e. always) this reads one past end */
     if (!col_dctrl_xmr()) goto bad_dctrl;
    }
   else
    {
     if (!__bld_expnode()) goto bad_dctrl;
     __get_vtok(); 
    }
  }
bld_evx:
 if (*dtyp == DC_EVENT) __bld_evxtree(); else __bld_xtree(0);
 return(__root_ndp);
}

/*
 * collect a delay or event control variable
 *
 * know @ or # and following ID read - done unless part of XMR
 * after ID can have optional select [..] then if not dot done else read
 * next component - collected XMR later parsed into glbref
 *
 * know ID read and reads one past end of XMR (if not simple ID)
 * also emits error on non ID or GLBREF - select at end illegal - need ()
 */
static int32 col_dctrl_xmr(void)
{
 int32 sblevel;

 __last_xtk = -1;
 if (!__bld_expnode()) goto bad_end;
 for (;;)
  {
   /* each time hear know ID read and added to exprtab */
   __get_vtok();
   if (__toktyp == LSB) 
    {
     if (!__bld_expnode()) goto bad_end;
     for (sblevel = 0;;)
      {
       __get_vtok();
       if (__toktyp == TEOF || __toktyp == SEMI)
        {
         __pv_ferr(1082,
          "illegal token %s in delay or event control global variable",
          __prt_vtok());
         goto bad_end;
        }
       if (__toktyp == LSB) sblevel++;
       else if (__toktyp == RSB)
        {
         if (sblevel <= 0) break;
         sblevel--; 
        }
       if (!__bld_expnode()) goto bad_end;
      }
     if (!__bld_expnode()) goto bad_end;
     __get_vtok();
     if (__toktyp != DOT)
      {
       __pv_ferr(1075,
        "delay or event control global variable contains select - must surround with parentheses");
       goto bad_end;
      }
    }
   /* anything after component but dot ends and not collected */
   /* works because this reads one past end */
   if (__toktyp != DOT) return(TRUE); 
   if (!__bld_expnode()) goto bad_end;
   /* read required ID after DOT */ 
   __get_vtok();
   if (__toktyp != ID)
    {
     __pv_ferr(1058,
      "delay/event control hierarchical reference identifier must follow dot - %s read", 
      __prt_kywrd_vtok());
    }
   if (!__bld_expnode()) goto bad_end;
  }

bad_end:
 __set_xtab_errval();
 return(FALSE);
}

/*
 * EXPRESSION COLLECTION ROUTINES
 * these return FALSE and 1'bx on error - caller must emit err
 */

/*
 * collect delay expr. make 0 if error - will never make it to fixup so ok
 * this collects and adds surrounding parenthesis - need for possible
 * min:typ:max with surrounding ()
 * but otherwise is identical to collect connecting expr.
 * expects 1st token to have been read
 */
extern int32 __col_delexpr(void)
{
 struct expr_t *ndp;

 __last_xtk = -1;
 ndp = __alloc_exprnd();
 ndp->optyp = LPAR;
 /* if no error, surround with parentheseses and parse as normal */
 if (!__col_connexpr(0))
  {
bad_del:
   __set_xtab_errval();
   /* need constant 32 bit 0 here */
   __exprtab[0]->szu.xclen = WBITS;
   __exprtab[0]->ru.xvi = __alloc_shareable_cval(0, 0, WBITS);
   return(FALSE);
  }
 /* if col conn T, then know at least 1 token */
 if (__last_xtk == 1 && __exprtab[1]->optyp == OPEMPTY)
  {
   __pv_ferr(1095, "empty delay expression illegal");
   goto bad_del;
  }
 ndp = __alloc_exprnd();
 ndp->optyp = RPAR;
 return(TRUE);
}


/*
 * collect parm or specparam rhs parameter value
 * this collects and adds surrounding parenthesis
 * needed for possible min:typ:max with surrounding ()
 * but otherwise is identical to collect comsemi
 * expects 1st token to have been read
 * 
 * on error sets value to 0 not x - is this right? 
 * if this is used where only ; can end, caller must check for and emit err
 */
extern int32 __col_paramrhsexpr(void)
{
 struct expr_t *ndp;

 __last_xtk = -1;
 ndp = __alloc_exprnd();
 ndp->optyp = LPAR;
 /* if no error, surround with parentheseses and parse as normal */
 if (!__col_comsemi(0))
  {
   __set_xtab_errval();
   /* need constant 32 bit 0 here */
   __exprtab[0]->szu.xclen = WBITS;
   __exprtab[0]->ru.xvi = __alloc_shareable_cval(0, 0, WBITS);
   return(FALSE);
  }
 ndp = __alloc_exprnd();
 ndp->optyp = RPAR;
 return(TRUE);
}

/*
 * collect paren surround expression and leave tree in __exprtab[0]
 * expects 1st expr. token to have been read (after '(') reads end token
 * need to reuse or move nodes here to tree
 * surrounding parentheses not included
 */
extern int32 __col_parenexpr(int32 start_xtk)
{
 int32 parlevel;

 /* this is illegal () case */
 if (__toktyp == RPAR)
  {
   __pv_ferr(1089, "empty parentheses ending expression illegal");
bad_end:
   /* notice cannot free here since in __exprtab (links just overwritten) */
   /* also make look like empty expr. */
   __set_xtab_errval();
   return(FALSE);
  }
 parlevel = 0;
 if (__toktyp == LPAR) parlevel++;

 for (__last_xtk = start_xtk;;)
  {
   if (!__bld_expnode()) goto bad_end;
   __get_vtok();
   if (__toktyp == LPAR) parlevel++;
   else if (__toktyp == RPAR)
    {
     if (parlevel > 0) parlevel--; else break;
    }
   else if (__toktyp == TEOF || __toktyp == SEMI)
    {
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1090, "illegal token %s in parenthesis ending expression",
       __prt_vtok());
     goto bad_end;
    }
  }
 return(TRUE);
}

/*
 * collect event expr - for new VER 2001 , as alternative to evor legal
 * same as collect paren expr except commas allowed and special
 * ev or comma token substituted
 *
 * expects 1st expr. token to have been read (after '(') reads end token
 *
 * know implicit @* and @(*) never seen here
 * need to reuse or move nodes here to tree
 */
static int32 col_evctrlexpr(void)
{
 int32 parlevel, catlevel;

 __last_xtk = -1;
 /* this is illegal () case */
 if (__toktyp == RPAR)
  {
   __pv_ferr(1089, "empty parentheses ending expression illegal");
bad_end:
   /* notice cannot free here since in __exprtab (links just overwritten) */
   /* also make look like empty expr. */
   __set_xtab_errval();
   return(FALSE);
  }
 parlevel = catlevel = 0;
 if (__toktyp == LPAR) parlevel++;
 /* AIV 09/27/06 - catlevel was off for first case LCB */
 /* was incorrectly handling @({a, b}) */
 if (__toktyp == LCB) catlevel++;

 for (;;)
  {
   /* SJM 06/01/04 - this is unusual ',' just like event "or" but different */
   /* expr node op typ */
   /* bld expnode can't see comma because mashes error recovery */
   if (__toktyp == COMMA && catlevel == 0) __toktyp = OPEVCOMMAOR;
   if (!__bld_expnode()) goto bad_end;

   __get_vtok();
   switch ((byte) __toktyp) {
    case LCB: catlevel++; break;
    case RCB: catlevel--; break;
    case LPAR: parlevel++; break;
    case RPAR:
     /* only non nested rpar can cause correct exit */
     if (parlevel > 0) { parlevel--; break; }
     goto done;
    default:
     if (__toktyp == TEOF || __toktyp == SEMI)
      {
       if (__pv_err_cnt <= __saverr_cnt)
        {
         __pv_ferr(1090, "illegal token %s in parenthesis ending expression",
          __prt_vtok());
        }
       goto bad_end;
      }
    } 
  }
done:
 return(TRUE);
}

/*
 * collect a range expression
 * expects first token ([) to have been read and reads ending ] token
 * includes ] in expression
 */
extern int32 __col_rangeexpr(void)
{
 int32 brklevel;
 struct expr_t *ndp;

 /* must add dummy symbol so range parses as x[cexpr:cexpr] */
 __last_xtk = -1;
 ndp = __alloc_exprnd();
 ndp->optyp = ID;
 /* need dummy to fill id table - not used and overwritten each time */ 
 /* this checked for during term parsing and not decled */ 
 __alloc_expridnd("[]");
 /* node for already read [ */
 if (!__bld_expnode()) goto bad_end;

 for (brklevel = 0;;)
  {
   __get_vtok();
   if (__toktyp == LSB) brklevel++;
   else if (__toktyp == RSB)
    {
     if (brklevel <= 0)
      {
       if (__last_xtk == 1)
        {
         __pv_ferr(1097, "select range empty form ([]) illegal");
         goto bad_end;
        }
       /* ending ] must be part of expression */
       if (!__bld_expnode()) goto bad_end;
       break;
      }
     else brklevel--;
    }
   else if (__toktyp == TEOF || __toktyp == SEMI)
    {
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1091, "illegal token %s in select range expression",
       __prt_vtok());
     
bad_end:
     /* must be 1 bit x not unc. */
     __set_xtab_errval();
     __last_xtk = 0;
     return(FALSE);
    }
   if (!__bld_expnode()) goto bad_end;
  }
 return(TRUE);
}

/*
 * collect an inst., port header, function call, delay, cat expr. or
 * instance pound parameter list
 *
 * expects 1st token to be read and read ending token
 * ends with ) or ,
 * allows full expressions but also used for param subset exprs.
 * need start_xtk for delay params where min:typ:max does not need (),
 * scheme adds surrounding () and then calls normal bld_xtree
 * for each comma sep.  * piece
 * normal start of expr. is -1
 */
extern int32 __col_connexpr(int32 start_xtk)
{
 int32 parlevel, catlevel;

 /* ,, or ,) form ok here - if not caller will check */
 if (__toktyp == COMMA || __toktyp == RPAR)
  { __last_xtk = start_xtk + 1; __set_opempty(__last_xtk); return(TRUE); }

 for (__last_xtk = start_xtk, parlevel = 0, catlevel = 0;;)
  {
   switch ((byte) __toktyp) {
    case LPAR: parlevel++; break;
    case LCB: catlevel++; break;
    case RPAR:
     if (parlevel <= 0 && catlevel <= 0) return(TRUE); else parlevel--;
     break;
   case RCB:
    catlevel--;
    break;
   case COMMA:
    if (parlevel <= 0 && catlevel <= 0) return(TRUE);
    break;
   case TEOF:
   case SEMI:
    if (__pv_err_cnt <= __saverr_cnt)
     __pv_ferr(1092, "illegal token %s in expression list", __prt_vtok());
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
 * collect something from an assignment list rhs
 * expects 1st token of assignment rhs expr to have been read
 * rhs expr. can have (..) function calls and concatenates
 * also (..) delay triples 
 *
 * reads trailing , or ;
 * empty here will not collect - error
 */
extern int32 __col_comsemi(int32 last_xti)
{
 int32 parlevel, catlevel;

 __last_xtk = last_xti; 
 for (parlevel = 0, catlevel = 0;;)
  {
   switch ((byte) __toktyp) {
    case LPAR: parlevel++; break;
    case RPAR: parlevel--; break;
    case LCB: catlevel++; break;
    case RCB: catlevel--; break;
    case SEMI:
chk_empty:
     /* empty = ; form illegal */
     if (__last_xtk == last_xti) 
      {
       __pv_ferr(1106, "right hand side expression missing");
       goto bad_end;
      }
     return(TRUE);
    case COMMA:
     if (parlevel <= 0 && catlevel <= 0) goto chk_empty;
     break;
    case TEOF:
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1093, "illegal token %s in right hand side expression",
       __prt_vtok());
     goto bad_end;
   }
   if (!__bld_expnode()) goto bad_end;
   __get_vtok();
  }

bad_end:
 /* replace entire expression with 1 bit x - not op empty */
 __set_xtab_errval();
 return(FALSE);
}

/*
 * new parameter format collect param (not specify) rhs expr
 *
 * expects 1st token of assignment rhs expr to have been read
 * and reads one past end , or ;
 *
 * for mismatched nesting parse will catch error
 */
extern int32 __col_newparamrhsexpr(void)
{
 int32 parlevel, catlevel;
 struct expr_t *ndp;

 __last_xtk = -1; 
 ndp = __alloc_exprnd();
 ndp->optyp = LPAR;
 for (parlevel = 0, catlevel = 0;;)
  {
   switch ((byte) __toktyp) {
    case LPAR: parlevel++; break;
    case RPAR: parlevel--; break;
    case LCB: catlevel++; break;
    case RCB: catlevel--; break;
    case SEMI:
chk_empty:
     /* empty = ; form illegal */
     if (__last_xtk == -1) 
      {
       __pv_ferr(1106, "right hand side expression missing");
       goto bad_end;
      }
     ndp = __alloc_exprnd(); 
     ndp->optyp = RPAR;
     return(TRUE);
    case COMMA:
     if (parlevel <= 0 && catlevel <= 0) goto chk_empty;
     break;
    case TEOF:
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1093,
       "illegal token %s in parameter right hand side expression",
       __prt_vtok());
     goto bad_end;
   }
   if (!__bld_expnode()) goto bad_end;
   __get_vtok();
  }

bad_end:
 /* replace entire expression with 1 bit x - not op empty */
 __set_xtab_errval();
 return(FALSE);
}

/*
 * ver 2001 #(list of param decls) form format collect param rhs expr
 *
 * expects 1st token of assignment rhs expr to have been read
 * and reads one past end , or )
 *
 * for mismatched nesting parse will catch error
 */
extern int32 __col_lofp_paramrhsexpr(void)
{
 int32 parlevel, catlevel;
 struct expr_t *ndp;

 __last_xtk = -1; 
 ndp = __alloc_exprnd();
 ndp->optyp = LPAR;
 for (parlevel = 0, catlevel = 0;;)
  {
   switch ((byte) __toktyp) {
    case LPAR: parlevel++; break;
    case LCB: catlevel++; break;
    case RCB: catlevel--; break;
    case RPAR:
     if (parlevel <= 0 && catlevel <= 0) goto chk_empty;
     else parlevel--;
     break;
    case COMMA:
     if (parlevel <= 0 && catlevel <= 0)
      {
chk_empty:
       /* empty = ) form illegal */
       if (__last_xtk == -1) 
        {
         __pv_ferr(1106, "right hand side expression missing");
         goto bad_end;
        }
       ndp = __alloc_exprnd(); 
       ndp->optyp = RPAR;
       return(TRUE);
      }
     break;
    case TEOF: case SEMI:
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1093,
       "illegal token %s in parameter right hand side expression",
       __prt_vtok());
     goto bad_end;
   }
   if (!__bld_expnode()) goto bad_end;
   __get_vtok();
  }

bad_end:
 /* replace entire expression with 1 bit x - not op empty */
 __set_xtab_errval();
 return(FALSE);
}

/*
 * collect an lvalue (all but procedural assign lhs)
 * expects 1st token of assignment lhs expr to have been read and reads
 * trailing =
 *
 * must emit error on 1st extra right parenthesis or will get error line
 * wrong - also if returns F must emit error
 */
extern int32 __col_lval(void)
{
 int32 parlevel;

 parlevel = 0;
 for (__last_xtk = -1;;)
  {
   switch ((byte) __toktyp) {
    case TEOF: case SEMI:
     {
maybe_err:
      __pv_ferr(1094,
       "illegal token %s in lvalue - probable missing = or assign to function call",
       __prt_vtok());
      goto bad_end;
     }
    case EQ:
     if (__last_xtk == -1)
      {
       /* empty = ; form illegal */
       __pv_ferr(1096, "assignment lvalue missing");
       goto bad_end;
      }
     return(TRUE);
    case LPAR: parlevel++; break;
    case RPAR:
     if (--parlevel < 0) goto maybe_err;
     break;
   }
   if (!__bld_expnode()) goto bad_end;
   __get_vtok();
  }

bad_end:
 __set_xtab_errval();
 return(FALSE);
}

/*
 * collect a procedural assignment lvalue (can end with <= non blocking
 * procedural assignment symbol or ( for task enable)
 *
 * expects 1st token of assignment lhs expr to have been read and reads
 * trailing '(' or ';' (no argumnet form), or '=' or '<=' for assign
 */
static int32 col2_lval(void)
{
 int32 sblevel, cblevel;

 sblevel = cblevel = 0;
 /* set flag for bld expnode error recovery - must be off when done */
 __expr_is_lval = TRUE;
 for (__last_xtk = -1;;)
  {
   switch ((byte) __toktyp) {
    case TEOF:
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1099,
       "illegal token %s in procedural assign lvalue or task enable",
        __prt_vtok());
     goto bad_end;
    case EQ:
     /* = can not appear in lhs because relational never '=' */
chk_emp:
     if (__last_xtk == -1)
      {
       /* empty lhs "= <rhs side>" form illegal */
       __pv_ferr(1117, "empty assignment or task enable lvalue illegal");
       goto bad_end;
      }
     __expr_is_lval = FALSE;
     return(TRUE);
    case SEMI:
     /* no argument task enable "$stop;" - must not be empty */
     /* ; is error correction marker - can not be nested in anything */
     goto chk_emp;
    case RELLE:
     if (sblevel == 0 && cblevel == 0) goto chk_emp;
     break;
    case LPAR:
     if (sblevel == 0 && cblevel == 0) goto chk_emp;
     break;
    case LSB: sblevel++; break;
    case RSB: sblevel--; break;
    case LCB: cblevel++; break;
    case RCB: cblevel--; break;
   }
   if (!__bld_expnode()) goto bad_end;
   __get_vtok();
  }

bad_end:
 __expr_is_lval = FALSE;
 __set_xtab_errval();
 return(FALSE);
}

/*
 * read a case expression list that ends with : or ,
 * also reads DEFAULT and ENDCASE expressions
 * expects 1st token to be read and reads ending token
 * but it is not included
 */
extern int32 __col_caseexpr(void)
{
 int32 parlevel, collevel, sblevel, cblevel;

 parlevel = collevel = sblevel = cblevel = 0;
 for (__last_xtk = -1;;)
  {
   switch ((byte) __toktyp) {
    case QUEST: collevel++; break;
    case COLON:
     /* ? token must be followed by : for ?: expression */
     if (collevel > 0) collevel--;
     else if (sblevel <= 0) goto good_end;
     break;
    case LSB: sblevel++; break;
    case RSB: sblevel--; break;
    case LCB: cblevel++; break; 
    case RCB: cblevel--; break;
    case LPAR: parlevel++; break;
    case RPAR: parlevel--; break;
    case COMMA:
     if (parlevel <= 0 && cblevel <= 0) goto good_end;
     break;
    case TEOF:
    case SEMI:
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1101, "illegal token %s in case item expression",
       __prt_vtok());
bad_end:
     __set_xtab_errval();
     return(FALSE);
   }
   if (!__bld_expnode()) goto bad_end;
   __get_vtok();
  }
good_end:
 if (__last_xtk == -1)
  {
   /* empty form illegal */
   __pv_ferr(1098, "case expression list missing");
   goto bad_end;
  }
 return(TRUE);
}

/*
 * build an expression tree node from current token
 * always places it in next free place in __exprtab
 * reuses storage so bld_xtree must allocate nodes
 *
 * puts ID name in expr_idtab that is changed to symbol in
 * parse term routine (either ID or xmr component)
 *
 * 'or' in expression always evor
 * anything part of expression goes through here
 */
extern int32 __bld_expnode(void)
{
 int32 wlen;
 struct opinfo_t *oip;
 struct expr_t *ndp;

 ndp = __alloc_exprnd();
 switch ((byte) __toktyp) {
  case ID:
   /* if can be inst. ref, assume global fix later */
   /* notice anthing in this case but id and glb wrong - caught later */
   if (strcmp(__token, "or") == 0) { ndp->optyp = OPEVOR; break; }
   ndp->optyp = ID;
   __alloc_expridnd(__token);
   break;
  case POSEDGE:
   ndp->optyp = OPPOSEDGE;
   break;
  case NEGEDGE:
   ndp->optyp = OPNEGEDGE;
   break;
  case OPEVCOMMAOR:
   ndp->optyp = OPEVCOMMAOR; 
   break;
  case NUMBER:
   /* notice scanner only returns number */  
   ndp->optyp = NUMBER;
   ndp->szu.xclen = __itoklen;
   if (__itoksized) ndp->unsiznum = FALSE; else ndp->unsiznum = TRUE;

   /* only true for decimal without 'd even 'd[num] is word32 */
   /* SJM 10/02/03 - scanner sets if signed - depends on new 2001 LRM rules */ 
   if (__itok_signed) ndp->has_sign = TRUE;

   ndp->ibase = (word32) __itokbase;
   ndp->sizdflt = (__itoksizdflt) ? TRUE : FALSE;
   /* Verilog values are really word32 bit patterns */

   wlen = wlen_(__itoklen);
   if (__itoklen <= WBITS)
    {
     /* always sets value - if not sharable uses non sharable alloc cval */
     ndp->ru.xvi = __alloc_shareable_cval(__acwrk[0], __bcwrk[0], __itoklen);
    }
   else
    {
     ndp->ru.xvi = __allocfill_cval_new(__acwrk, __bcwrk, wlen);
    }
   break;
  case REALNUM:
   /* num storage pted to by a part - usually (except xstk) no b part */ 
   ndp->optyp = REALNUM;
   ndp->szu.xclen = REALBITS;
   ndp->ibase = BDBLE;
   ndp->is_real = TRUE;
   ndp->has_sign = TRUE;
   /* LOOKATME - SIZE assuming size of real is 8 bytes here */
   ndp->ru.xvi = __alloc_shareable_rlcval(__itok_realval);
   break;
  case LITSTR:
   /* string copied to alloced storage and filled */
   ndp->optyp = NUMBER;
   ndp->szu.xclen = __itoklen;

   wlen = wlen_(__itoklen);
   ndp->ru.xvi = __allocfill_cval_new(__acwrk, __bcwrk, wlen);
   ndp->is_string = TRUE;
   ndp->unsiznum = FALSE;
   ndp->has_sign = FALSE;
   break;
  case TEOF:
   /* since no error recovery in interactive - this is finish path */
   /* caller sets to error expression */ 
   ndp->optyp = BADOBJ;
   return(FALSE);
  default:
   if (__toktyp > LASTOP)
    {
     /* keywords can not be in or end expressions */
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1104,
       "illegal token %s in expression - probable missing semicolon",
       __prt_vtok());
     return(FALSE);
    }
   /* build an operator node */
   oip = &(__opinfo[__toktyp]);
   if (oip->opclass == NOTANOP)
    {
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1105, "illegal punctuation %s in expression",
      __prt_vtok());
     return(FALSE);
    }
   /* build an operator node */
   ndp->optyp = __toktyp;
  }
 return(TRUE);
}

#define HASHTABSIZ 3011     /* prime number size of consts hash table */
#define HASHSEED 0x371546dc    /* seed of first word of the hash table */


/*
 * setup the constant tables
 *
 * allocate design wide constant table - so fixed constants at bottom
 * BEWARE - since constant table is design and reallocated must always ndx
 * allocate initial small constant table - grows when needed
 */ 
extern void __setup_contab(void)
{
 register int32 i, j;

 __contabwsiz = 400;
 __contab = (word32 *) __my_malloc(__contabwsiz*sizeof(word32));
 /* 0th and 1st unused - marker to catch errors */
 for (i = 0; i < 4; i++) __contab[i] = ALL1W;
 /* 3rd is one bit z */
 __contab[4] = 0;
 __contab[5] = 1;
 /* 4th is 1 bit x */
 __contab[6] = 1;
 __contab[7] = 1;
 /* 5th is 32 bit z */
 __contab[8] = 0;
 __contab[9] = ALL1W;
 /* 6th is 32 bit x */
 __contab[10] = ALL1W;
 __contab[11] = ALL1W;

 for (i = 12, j = 0; i <= 12 + 2*128; i += 2, j++)
  {
   __contab[i] = j; 
   __contab[i + 1] = 0;
  }
 /* also need build in special contab index for opempty */
 /* all rhs igen expr eval OP EMPTY nodes point to this */
 __opempty_contabi = 270;
 __contab[270] = ' ';
 __contab[271] = 0;
 __contabwi = 272;

 __contab_hash = (struct contab_info_t **)
  __my_malloc(HASHTABSIZ*sizeof(struct contab_info_t *));
 memset(__contab_hash, 0, HASHTABSIZ*sizeof(struct contab_info_t *));

}
 
/* some local use only constant for fixed con table locations */
#define FIXCON_1BITZ 4
#define FIXCON_1BITX 6
#define FIXCON_32BITZ 8
#define FIXCON_32BITX 10
#define NONXZ_CONBASE 12

/*
 * sharable (non IS form) any 32 bit or any 32 or less 0 
 * plus some non x/z low values
 */
extern int32 __alloc_shareable_cval(word32 aval, word32 bval, int32 bwid)
{
 int32 wi;

 if (bwid == 1)
  {
   if (bval == 0) goto do_nonxz;
   if (aval == 0) return(FIXCON_1BITZ);
   return(FIXCON_1BITX);
  }
  
 if (bval == ALL1W) 
  {
   if (bwid != WBITS) goto do_alloc;
   if (aval == 0) return(FIXCON_32BITZ);
   if (aval == ALL1W) return(FIXCON_32BITX);
   goto do_alloc;
  }
 if (bval != 0) goto do_alloc;
do_nonxz:
 if (aval <= 128) return(NONXZ_CONBASE + 2*aval);

do_alloc:
 aval &= __masktab[bwid];
 bval &= __masktab[bwid];
 wi = __allocfill_cval_new(&(aval), &(bval), 1);
 return(wi); 
}

/*
 * allocate a constant value in design wide constant table
 *
 * now only for IS forms where must allocate big region and can't use
 * hashing to share because each inst must be filled with mem copy
 *
 * wlen is really wlen times no. of instances in IS form - multiply by
 * 2 works since need b parts for each inst
 * know a and b parts contiguous
 */
extern int32 __alloc_is_cval(int32 wlen)
{
 int32 wi;

 if (__contabwi + 2*wlen >= __contabwsiz) __grow_contab(2*wlen);
 wi = __contabwi;
 __contabwi += 2*wlen;
 return(wi);
}

/*
 * sharable (non IS form) for real constants
 * SJM 05/03/05 - now reals go into hashed part of contab
 */
extern int32 __alloc_shareable_rlcval(double d1)
{
 int32 wi;
 word32 rword[2];

 memcpy(&(rword), &(d1), sizeof(double)); 
 /* notice word length is 1 since reals have no b part */
 wi = __allocfill_cval_new(&(rword[0]), &(rword[1]), 1);
 return(wi); 
}

/*
 * new version of allocate a constant value in design wide constant table
 * notice passing word len but need room for 2*wlen for b part
 *
 * that uses hashing to prevent too much growth of contab during
 * non blocking assign realloc and also just a better algorithm
 */
extern int32 __allocfill_cval_new(word32 *ap, word32 *bp, int32 wlen)
{
 register word32 hti;
 register struct contab_info_t *cip;
 int32 wi, bytsiz;
 word32 *wp;

 hti = get_hash(ap, bp, wlen);
 bytsiz = wlen*WRDBYTES;
 if ((cip = __contab_hash[hti]) != NULL)
  {
   for (; cip != NULL; cip = cip->cnxt)
    {
     if (cip->cwid != wlen) continue;

     wp = &(__contab[cip->xvi]);
     if (memcmp(wp, ap, bytsiz) == 0 && memcmp(&(wp[wlen]), bp, bytsiz) == 0) 
       return(cip->xvi);
    }
  }

 if (__contabwi + 2*wlen >= __contabwsiz) __grow_contab(2*wlen);
 wi = __contabwi;
 __contabwi += 2*wlen;
 memcpy(&(__contab[wi]), ap, bytsiz);
 memcpy(&(__contab[wi + wlen]), bp, bytsiz);

 cip = (struct contab_info_t *) __my_malloc(sizeof(struct contab_info_t));
 cip->xvi = wi;
 cip->cwid = wlen;
 /* put on front */ 
 cip->cnxt = __contab_hash[hti];
 __contab_hash[hti] = cip;
 return(wi);
}

/*
 * hash function
 */
static word32 get_hash(word32 *ap, word32 *bp, int32 wlen)
{
 register int32 wi;
 word32 hval;

 hval = ap[0] * HASHSEED;
 for (wi = 1; wi < wlen; wi++)
  {
   hval ^= (ap[wi] ^ bp[wi]);
  }
 hval += wlen;
 return(hval % HASHTABSIZ);
}

/*
 * grow design wide constant table
 *
 * when growing, grow with current needed words in case IS array large  
 * all sizes are in words because need word32 alignment
 */
extern void __grow_contab(int32 ndwrds)
{
 int32 old_ctabsiz, obsize, nbsize;
 
 old_ctabsiz = __contabwsiz;
 obsize = __contabwsiz*sizeof(word32);
 /* fibronacci growth */
 __contabwsiz = 3*(old_ctabsiz + ndwrds)/2; 
 nbsize = __contabwsiz*sizeof(word32);
 __contab = (word32 *) __my_realloc((char *) __contab, obsize, nbsize);
}


/*
 * EXPRESSION PROCESSING CODE
 */

/*
 * operator table
 * SPECOP means requires separate case entry and special processing
 * BEWARE - this table requires operator number k to be on row k
 */
struct opinfo_t __opinfo[] = {
 /* place holders since using token number used as index into op table */
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "00E" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "idE" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "bnE" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "nE" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "inE" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "rE" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "irE" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "sE" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "giE" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "rmE" },
 /* 10 */
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "rmE" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "rmE" },
 /* notice ; now 12 */
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, ";" },
 { SPECOP, NOTREALOP, NOTPTHOP, WIDSUM, "," },
 { SPECOP, NOTREALOP, PTHOP, WIDNONE, ":" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "#" },
 /* left grouping operators treated specially */
 { BINOP, NOTREALOP, NOTPTHOP, WIDNONE, "(" },  /* ( function call op. */
 { SPECOP, NOTREALOP, PTHOP, WIDNONE, ")" },    /* ) RPAR */
 { SPECOP, NOTREALOP, PTHOP, WIDNONE, "[" },    /* [ LSB */
 { SPECOP, NOTREALOP, PTHOP, WIDNONE, "]" },    /* ] RSB */
 /* 20 */
 { BINOP, NOTREALOP, PTHOP, WIDSUM, "{" },      /* { LCB - empty rght but bin */
 { SPECOP, NOTREALOP, PTHOP, WIDNONE, "}" },    /* } RCB */
 { SPECOP, NOTREALOP, PTHOP, WIDNONE, "." },    /* . DOT */
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "@" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "->" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "=" },
 /* unary ops */
 /* LOOKATME - this is not really self determined result context determines */
 /* but does not fit in pattern so always checked for */ 
 { UNOP, NOTREALOP, PTHOP, WIDMAX, "~" },       /* ~ BITNOT - self determine */
 { UNOP, REALOP, PTHOP, WIDONE, "!" },          /* ! NOT */
 { RUNOP, REALOP, NOTPTHOP, WIDONE, "!" },      /* ! REALNOT */
 /* both unary and binary */
 { BOTHOP, NOTREALOP, PTHOP, WIDMAX, "^~" },    /* REDXNOR - must be ^~ */
 /* 30 */
 { BOTHOP, REALOP, NOTPTHOP, WIDMAX, "+" },     /* + PLUS */
 { BOTHOP, REALOP, NOTPTHOP, WIDMAX, "-" },     /* - MINUS (un fixed to self)*/
 { BOTHOP, NOTREALOP, PTHOP, WIDMAX, "&" },     /* & BITREDAND */
 { BOTHOP, NOTREALOP, PTHOP, WIDMAX, "|" },     /* | BITREDOR */
 { BOTHOP, NOTREALOP, PTHOP, WIDMAX, "^" },     /* ^ BITREDXOR */
 { RBOTHOP, REALOP, NOTPTHOP, WIDMAX, "-" },     /* + REALINUS */
 /* binary operators */
 { BINOP, REALOP, NOTPTHOP, WIDMAX, "*" },      /* * TIMES */
 { BINOP, REALOP, NOTPTHOP, WIDMAX, "/" },      /* / DIV */
 { BINOP, NOTREALOP, NOTPTHOP, WIDMAX, "%" },   /* % MOD */
 { BINOP, REALOP, NOTPTHOP, WIDENONE, ">=" },   /* >= RELGE */
 /* 40 */
 { BINOP, REALOP, NOTPTHOP, WIDENONE, ">" },    /* > RELGT */
 { BINOP, REALOP, NOTPTHOP, WIDENONE, "<=" },   /* <= RELLE (not assgn) */
 { BINOP, REALOP, NOTPTHOP, WIDENONE, "<" },    /* < RELLT */
 { BINOP, NOTREALOP, NOTPTHOP, WIDENONE, "===" }, /* === RELCEQ */
 { BINOP, REALOP, PTHOP, WIDENONE, "==" },      /* == RELEQ */
 { BINOP, NOTREALOP, NOTPTHOP, WIDENONE, "!==" }, /* !== RELCNEQ */
 { BINOP, REALOP, PTHOP, WIDENONE, "!=" },      /* != RELNEQ */
 { BINOP, REALOP, PTHOP, WIDONE, "&&" },        /* && BOOLAND */
 { BINOP, REALOP, PTHOP, WIDONE, "||" },        /* || BOOLOR */
 { BINOP, NOTREALOP, NOTPTHOP, WIDLEFT, "<<" }, /* << SHIFTL */
 /* 50 */
 { BINOP, NOTREALOP, NOTPTHOP, WIDLEFT, "<<<" }, /* <<< ASHIFTL */
 { BINOP, NOTREALOP, NOTPTHOP, WIDLEFT, ">>" }, /* >> SHIFTR */
 { BINOP, NOTREALOP, NOTPTHOP, WIDLEFT, ">>>" }, /* >>> ASHIFTR */
 { BINOP, NOTREALOP, NOTPTHOP, WIDONE, "*>" },  /* *> FPTHCON */
 { BINOP, NOTREALOP, NOTPTHOP, WIDONE, "=>" },  /* => PPTHCON */
 { BINOP, NOTREALOP, NOTPTHOP, WIDONE, "&&&" }, /* TCHK &&& (cond. event) */
 { RBINOP, REALOP, NOTPTHOP, WIDMAX, "+" },     /* + REALPLUS */ 
 { RBINOP, REALOP, NOTPTHOP, WIDMAX, "*" },     /* * REALTIMES */ 
 { RBINOP, REALOP, NOTPTHOP, WIDMAX, "/" },     /* / REALDIV */ 
 { RBINOP, REALOP, NOTPTHOP, WIDENONE, ">" },   /* > REALRELGT */
 /* 60 */
 { RBINOP, REALOP, NOTPTHOP, WIDENONE, ">=" },  /* >= REALRELGE */
 { RBINOP, REALOP, NOTPTHOP, WIDENONE, "<" },   /* < REALRELLT */
 { RBINOP, REALOP, NOTPTHOP, WIDENONE, "<=" },  /* <= REALRELLE */
 { RBINOP, REALOP, NOTPTHOP, WIDENONE, "==" },  /* == REALRELEQ */
 { RBINOP, REALOP, NOTPTHOP, WIDENONE, "!=" },  /* != REALRELEQ */
 { RBINOP, REALOP, NOTPTHOP, WIDONE, "&&" },    /* && REALBOOLAND */
 { RBINOP, REALOP, NOTPTHOP, WIDONE, "||" },    /* != REALBOOLOR */
 /* tree will be left of : will be (cond)?(1st expr) and right will be */
 /* 2nd must be fixed, need for a[x?y:z:...] */
 /* width is max of operators (i.e. narrow must be padded) */
 { SPECOP, REALOP, PTHOP, WIDMAX, "?" },        /* ? QUEST (lhs part of ?:) */
 { SPECOP, NOTREALOP, PTHOP, WIDMAX, "?:" },    /* : QCOL (rhs part of ?:)  */
 { SPECOP, NOTREALOP, PTHOP, WIDSELF, "PARTSEL" },/* [ PARTSEL (2 val sel) */
 /* 70 */
 { BINOP, NOTREALOP, PTHOP, WIDSUM, "CATCOMMA" }, /* , part of { } */
 /* probably should have real quad op. for these */
 { BINOP, NOTREALOP, PTHOP, WIDSELF, "CATREP" },  /* [num]{ } concat repeat */
 { BINOP, NOTREALOP, NOTPTHOP, WIDSELF, "FCALL OP" }, /* function call */
 { BINOP, NOTREALOP, NOTPTHOP, WIDSELF, "LIST COMMA" },/* , of fcall () list*/
 /* only legal in event expressions */
 { SPECOP, NOTREALOP, NOTPTHOP, WIDONE, "or" },   /* OR event or */
 { SPECOP, NOTREALOP, NOTPTHOP, WIDONE, "," },    /* , event or as comma */
 /* these are normal unaries except semantics only operand of evor */
 { UNOP, NOTREALOP, NOTPTHOP, WIDONE, "posedge " }, /* OPPOSEDGE ev prefix */
 { UNOP, NOTREALOP, NOTPTHOP, WIDONE, "negedge " }, /* OPNEGEDGE ev prefix */
 /* this do not use table - handle in code */
 { SPECOP, REALOP, NOTPTHOP, WIDMAX, "?" },     /* REALREALQUEST (lhs of ?:) */
 { SPECOP, REALOP, NOTPTHOP, WIDMAX, "?" },     /* REALREGQUEST (lhs of ?:) */
 /* 80 */
 { SPECOP, REALOP, NOTPTHOP, WIDMAX, "?:" },    /* REGREALQCOL (rhs of ?:) */
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "rmE" }, /* GLBREF */
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "rmE" }, /* GLBPATH */
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "XMR ID" }, /* XMRID */
 { BINOP, NOTREALOP, NOTPTHOP, WIDSELF, "XMR LIST COMMA" },/* XMR comma */
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "EXPR END" },
 { NOTANOP, NOTREALOP, NOTPTHOP, WIDNONE, "TEOF" },
};

/*
 * convert expression table to expression tree
 * for specparam may start at 1 instead of 0 if not min:typ:max form
 */
extern void __bld_xtree(int32 start_xndi)
{
 struct expr_t *ndp;

 /* add end of expression fence */
 __has_top_mtm = FALSE;
 ndp = __alloc_exprnd();
 ndp->optyp = UNDEF;
 __xndi = start_xndi;
 __saverr_cnt = __pv_err_cnt;
 /* __xndi should point to last token which is added UNDEF fence */
 if ((__root_ndp = parse_qcexpr()) == NULL || __xndi != __last_xtk)
  {
   /* if for some reason, empty exprs (no error), emit some kind of err */
   if (__saverr_cnt == __pv_err_cnt)
    {
     if (__xndi > __last_xtk)
      __pv_ferr(1108, "expression missing required operator or operand");
     else if (__xndi < __last_xtk)
      {
       if (__exprtab[__xndi]->optyp == COMMA)
        __pv_ferr(1111,
         "expression list illegal - check for missing ')' or extra ','"); 
       else __pv_ferr(1109, "expression illegal token [%s]",
        to_xndnam(__xs, __xndi));
      }
     else
      {
       __pv_ferr(1110, "expression parse error at token [%s]",
	to_xndnam(__xs, __xndi));
      }
    }
   /* notice by here already moved to allocated nodes from exprtab */   
   __root_ndp = __alloc_newxnd();
   __set_numval(__root_ndp, 1L, 1L, 1);
  }
}

/*
 * EXPRESSION PARSING ROUTINES
 */

/*
 * parse an expression that can contain ?: operators
 *
 * ?: is lowest precedence - everything has stronger attraction 
 */
static struct expr_t *parse_qcexpr(void)
{
 struct expr_t *ndp, *qcndp, *tndp, *fndp;

 /* handle most common cases without parsing */
 /* before parsing bld xtree always places end marker - undef at end */
 /* so last xtk here is one past end i.e. 1 for ID */
 /* also because not yet parsed part select operator still LSB */ 
 if (__last_xtk == 1
  || (__last_xtk == 3 && __exprtab[1]->optyp == LSB)
  || (__last_xtk == 6 && __exprtab[1]->optyp == LSB
     && __exprtab[3]->optyp == COLON))
  {
   qcndp = parse_term();
   return(qcndp);
  }

 /* DBG need special case code, just set __last_xtk == 1 term */
 /* routine leaves __xndi at next place to look */
 if ((qcndp = parse_boolorop()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 if (ndp->optyp == QUEST)
  {
   ndp = alloc_xtnd(__xndi);
   ndp->lu.x = qcndp;
   qcndp = ndp;
   __xndi++;
   /* this can be recursive since no left recursion problem for ?: */
   /* this forces right to left grouping since reduces right then backs */
   /* up recursion tree to build reduced pieces from left */
   if ((tndp = parse_qcexpr()) == NULL)
    {
bad_qcexpr:
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1112, "?: parse error at token [%s]", to_xndnam(__xs,
       __xndi));
     /* move x ndi to end */
     xskip_toend();

     /* on error here make a 1 bit x this is part of longer expr. */
     ndp = alloc_xtnd(__xndi);
     __set_numval(ndp, 1L, 1L, 1);
     return(ndp);
    }
   ndp = __exprtab[__xndi];
   if (ndp->optyp != COLON) goto bad_qcexpr;
   ndp = alloc_xtnd(__xndi);
   ndp->optyp = QCOL;
   qcndp->ru.x = ndp;
   ndp->lu.x = tndp;
   __xndi++;
   if ((fndp = parse_qcexpr()) == NULL) goto bad_qcexpr;
   ndp->ru.x = fndp;
  }
 return(qcndp);
}

/*
 * skip past 1 expression node of node type ndtyp
 * this is needed
 */
static void xskip_toend(void)
{
 struct expr_t *ndp;

 for (;;__xndi++)
  {
   /* never skip past expr. ending fence */
   ndp = __exprtab[__xndi];
   if (ndp->optyp == UNDEF) break;
  }
}

/*
 * parse boolean or (||) operators
 * precdence one up from lowest
 */
static struct expr_t *parse_boolorop(void)
{
 int32 savi;
 struct expr_t *ndp, *rhsndp, *leftopndp;

 /* routine leaves __xndi at next place to look */
 if ((leftopndp = parse_boolandop()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 for (;;)
  {
   if (ndp->optyp == BOOLOR)
    {
     savi = __xndi;
     __xndi++;
     /* treat [term [op] error] as term - could improve recovery here */
     if ((rhsndp = parse_boolandop()) == NULL) break;
     /* this makes sure optyp right */
     ndp = alloc_xtnd(savi);
     ndp->lu.x = leftopndp;
     ndp->ru.x = rhsndp;
     leftopndp = ndp;
     ndp = __exprtab[__xndi];
    }
   else break;
  }
 return(leftopndp);
}

/*
 * parse boolean and (&&) operators
 */
static struct expr_t *parse_boolandop(void)
{
 int32 savi;
 struct expr_t *ndp, *rhsndp, *leftopndp;

 /* routine leaves __xndi at next place to look */
 if ((leftopndp = parse_borop()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 for (;;)
  {
   if (ndp->optyp == BOOLAND)
    {
     savi = __xndi;
     __xndi++;
     if ((rhsndp = parse_borop()) == NULL) break;

     /* this makes sure optyp right */
     ndp = alloc_xtnd(savi);
     ndp->lu.x = leftopndp;
     ndp->ru.x = rhsndp;
     leftopndp = ndp;
     ndp = __exprtab[__xndi];
    }
   else break;
  }
 return(leftopndp);
}

/*
 * parse bit wise or operators
 */
static struct expr_t *parse_borop(void)
{
 int32 savi;
 struct expr_t *ndp, *rhsndp, *leftopndp;

 /* routine leaves __xndi at next place to look */
 if ((leftopndp = parse_bxorop()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 for (;;)
  {
   if (ndp->optyp == BITREDOR)
    {
     /* fence and must check at binary bit wise | for following un red | */
     if (__exprtab[__xndi + 1]->optyp == BITREDOR)
      {
       __pv_fwarn(564,
	"[expr] | |[expr] invalid - unary should be surrounded by parentheses");
      }
     savi = __xndi;
     __xndi++;
     if ((rhsndp = parse_bxorop()) == NULL) break;
     /* this makes sure optyp right */
     ndp = alloc_xtnd(savi);
     ndp->lu.x = leftopndp;
     ndp->ru.x = rhsndp;
     leftopndp = ndp;
     ndp = __exprtab[__xndi];
    }
   else break;
  }
 return(leftopndp);
}

/*
 * parse bit wise xor operators
 */
static struct expr_t *parse_bxorop(void)
{
 int32 savi;
 struct expr_t *ndp, *rhsndp, *leftopndp;

 /* routine leaves __xndi at next place to look */
 if ((leftopndp = parse_bandop()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 for (;;)
  {
   if (ndp->optyp == BITREDXOR || ndp->optyp == REDXNOR)
    {
     savi = __xndi;
     __xndi++;
     /* treat [term [op] error] as term */
     if ((rhsndp = parse_bandop()) == NULL) break;
     /* this makes sure optyp right */
     ndp = alloc_xtnd(savi);
     ndp->lu.x = leftopndp;
     ndp->ru.x = rhsndp;
     leftopndp = ndp;
     ndp = __exprtab[__xndi];
    }
   else break;
  }
 return(leftopndp);
}

/*
 * parse bit wise and operators
 */
static struct expr_t *parse_bandop(void)
{
 int32 savi;
 struct expr_t *ndp, *rhsndp, *leftopndp;

 /* routine leaves __xndi at next place to look */
 if ((leftopndp = parse_eqop()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 for (;;)
  {
   if (ndp->optyp == BITREDAND)
    {
     /* notice when see binary & must see if following unary red. & */
     /* know fence always present */
     if (__exprtab[__xndi + 1]->optyp == BITREDAND)
      {
       __pv_fwarn(564,
	"[expr] & &[expr] invalid - unary should be surrounded by parentheses");
      }
     savi = __xndi;
     __xndi++;
     if ((rhsndp = parse_eqop()) == NULL) break;
     /* this makes sure optyp right */
     ndp = alloc_xtnd(savi);
     ndp->lu.x = leftopndp;
     ndp->ru.x = rhsndp;
     leftopndp = ndp;
     ndp = __exprtab[__xndi];
    }
   else break;
  }
 return(leftopndp);
}

/*
 * parse equal not equal operators
 */
static struct expr_t *parse_eqop(void)
{
 int32 savi;
 struct expr_t *ndp, *rhsndp, *leftopndp;

 /* routine leaves __xndi at next place to look */
 if ((leftopndp = parse_ltgtop()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 for (;;)
  {
   if (ndp->optyp == RELEQ || ndp->optyp == RELNEQ || ndp->optyp == RELCEQ
    || ndp->optyp == RELCNEQ)
    {
     savi = __xndi;
     __xndi++;
     if ((rhsndp = parse_ltgtop()) == NULL) break;
     /* this makes sure optyp right */
     ndp = alloc_xtnd(savi);
     ndp->lu.x = leftopndp;
     ndp->ru.x = rhsndp;
     leftopndp = ndp;
     ndp = __exprtab[__xndi];
    }
   else break;
  }
 return(leftopndp);
}

/*
 * parse a less than or greater than type op
 */
static struct expr_t *parse_ltgtop(void)
{
 int32 savi;
 struct expr_t *ndp, *rhsndp, *leftopndp;

 /* routine leaves __xndi at next place to look */
 if ((leftopndp = parse_shop()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 for (;;)
  {
   if (ndp->optyp == RELLT || ndp->optyp == RELLE || ndp->optyp == RELGT
    || ndp->optyp == RELGE)
    {
     savi = __xndi;
     __xndi++;
     if ((rhsndp = parse_shop()) == NULL) break;
     /* this makes sure optyp right */
     ndp = alloc_xtnd(savi);
     ndp->lu.x = leftopndp;
     ndp->ru.x = rhsndp;
     leftopndp = ndp;
     ndp = __exprtab[__xndi];
    }
   else break;
  }
 return(leftopndp);
}

/*
 * parse a shift type op
 */
static struct expr_t *parse_shop(void)
{
 int32 savi;
 struct expr_t *ndp, *rhsndp, *leftopndp;

 /* routine leaves __xndi at next place to look */
 if ((leftopndp = parse_addop()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 for (;;)
  {
   if (ndp->optyp == SHIFTL || ndp->optyp == SHIFTR
    || ndp->optyp == ASHIFTL || ndp->optyp == ASHIFTR)
    {
     savi = __xndi;
     __xndi++;
     if ((rhsndp = parse_addop()) == NULL) break;
     /* this makes sure optyp right */
     ndp = alloc_xtnd(savi);
     ndp->lu.x = leftopndp;
     ndp->ru.x = rhsndp;
     leftopndp = ndp;
     ndp = __exprtab[__xndi];
    }
   else break;
  }
 return(leftopndp);
}

/*
 * parse a add type op
 */
static struct expr_t *parse_addop(void)
{
 int32 savi;
 /* struct expr_t *ndp, *lhsndp, *rhsndp, *opndp, *last_opndp; */
 struct expr_t *ndp, *rhsndp, *leftopndp;

 /* routine leaves __xndi at next place to look */
 if ((leftopndp = parse_mulop()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 /* last_opndp = NULL; */
 for (;;)
  {
   if (ndp->optyp == PLUS || ndp->optyp == MINUS)
    {
     savi = __xndi;
     __xndi++;
     if ((rhsndp = parse_mulop()) == NULL) break;
     /* this makes sure optyp right */
     ndp = alloc_xtnd(savi);
     ndp->lu.x = leftopndp;
     ndp->ru.x = rhsndp;
     leftopndp = ndp;
     ndp = __exprtab[__xndi];
    }
   else break;
  }
 return(leftopndp);
}

/*
 * parse a mult/div type op
 */
static struct expr_t *parse_mulop(void)
{
 int32 savi;
 struct expr_t *ndp, *rhsndp, *leftopndp;

 /* routine leaves __xndi at next place to look */
 if ((leftopndp = parse_unopterm()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 for (;;)
  {
   if (ndp->optyp == TIMES || ndp->optyp == DIV || ndp->optyp == MOD)
    {
     savi = __xndi;
     __xndi++;
     if ((rhsndp = parse_unopterm()) == NULL) break;
     /* this makes sure optyp right */
     ndp = alloc_xtnd(savi);
     ndp->lu.x = leftopndp;
     ndp->ru.x = rhsndp;
     leftopndp = ndp;
     ndp = __exprtab[__xndi];
    }
   else break;
  }
 return(leftopndp);
}

/*
 * parse [un. op] <term>
 * expects __xndi to point to next place in expr. and moves to 1 after end
 *
 * parens here surround subexpressions not fcall
 */
static struct expr_t *parse_unopterm(void)
{
 int32 sav_xndi;
 struct expr_t *ndp, *ndp2, *unopndp, *lastndp;

 lastndp = NULL;
 unopndp = NULL;
 ndp = __exprtab[__xndi];
 switch ((byte) ndp->optyp) {
  /* notice real number can not start with . (.33 illegal) */
  case ID: case NUMBER: case REALNUM: return(parse_term());
  case LPAR:
do_parens:
   sav_xndi = __xndi++;
   if ((ndp2 = parse_qcexpr()) == NULL) return(NULL);
   ndp = __exprtab[__xndi];
   /* any time in ( expr. may be (expr:expr:expr) form */
   /* only parsable because all 3 required if : present */
   if (ndp->optyp == COLON)
    {
     struct expr_t *minxndp, *nomxndp;

     minxndp = ndp2;
     __xndi++;
     if ((ndp2 = parse_qcexpr()) == NULL)
      {
mintyp_err:
       if (__pv_err_cnt <= __saverr_cnt)
        __pv_ferr(1113, "mintypmax expression error at token [%s]",
         to_xndnam(__xs, __xndi));
       /* must skip one past 3 value form ending ) */
       skip_3valend();
       return(NULL);
      }
     nomxndp = ndp2;
     ndp = __exprtab[__xndi];
     if (ndp->optyp != COLON) goto mintyp_err;
     __xndi++;
     if ((ndp2 = parse_qcexpr()) == NULL) goto mintyp_err;
     ndp = __exprtab[__xndi];
     if (ndp->optyp != RPAR) goto mintyp_err;
     if (__mintypmax_sel == DEL_MIN) ndp2 = minxndp;
     else if (__mintypmax_sel == DEL_TYP) ndp2 = nomxndp;
     __xndi++;
     if (sav_xndi == 0) __has_top_mtm = TRUE;
     goto unop_ret;
    }
   /* either eox or ) seen or will not get here */
   if (ndp->optyp != RPAR)
    {
     __pv_ferr(1113,
      "expression primary or mintypmax expression ending ) expected - %s read",
      to_xndnam(__xs, __xndi));
     return(NULL);
    }
   __xndi++;
   goto unop_ret;
 }
 if (is_unop(ndp->optyp))
  {
rep_unops:
   if (ndp->optyp != PLUS)
    {
     ndp = alloc_xtnd(__xndi);
     ndp->ru.x = NULL;
     if (lastndp == NULL) unopndp = ndp; else lastndp->lu.x = ndp;
     lastndp = ndp;
    }

   ndp2 = __exprtab[++__xndi];
   if (ndp2->optyp == LPAR) goto do_parens;
   /* unary operator chain */
   if (is_unop(ndp2->optyp))
    {
     ndp = ndp2;
     goto rep_unops;
    }

   if ((ndp2 = parse_term()) == NULL) return(NULL);
   /* add to end of possible linear unary operator chain */
unop_ret:
   if (lastndp == NULL) unopndp = ndp2; else lastndp->lu.x = ndp2;
   return(unopndp);
  }
 return(parse_term());
}

/*
 * on error in 3 value expression must skip to eox or 1 past same level )
 */
static void skip_3valend(void)
{
 int32 paren_level;

 for (paren_level = 0; __xndi <= __last_xtk; __xndi++)
  {
   switch ((byte) __exprtab[__xndi]->optyp) {
    case UNDEF: return;
    case LPAR: paren_level++; break;
    case RPAR:
     if (paren_level == 0) { __xndi++; return; }
     else paren_level--;
   }
  }
}

/*
 * return T if expression node is a unary operator that does not require
 * special processing
 */
static int32 is_unop(word32 otyp)
{
 struct opinfo_t *opip;

 opip = &(__opinfo[otyp]);
 if (opip->opclass == UNOP || opip->opclass == BOTHOP) return(TRUE);
 return(FALSE);
}

/*
 * parse a terminal 
 *
 * expects __xndi at current place and leaves __xndi index one
 * past end of terminal
 *
 * notice Verilog terminals need special routine - too complicated
 * to parse using grammar 
 *
 * here can use expridtab and exprtab as arguments to routine because
 * these are not overwritten but globals that parsing marches through
 *
 * no error recovery because already have collected only expr
 */
static struct expr_t *parse_term(void)
{
 struct expr_t *ndp, *ndp2, *idndp, *glbndp;
 struct sy_t *syp;
 struct expridtab_t *xidp;

 /* some kind of terminal */
 ndp = __exprtab[__xndi];
 switch ((byte) ndp->optyp) {
  case NUMBER: case REALNUM: case OPEMPTY:
   /* 06/22/00 - SJM - if 2 numeric tokens in a row assume sized num */ 
   /* FIXME - need error recovery here - indicate 2nd is part of 2 tok num */
   if (__exprtab[__xndi + 1]->optyp == NUMBER)
    {
     struct expr_t *ndp3;

     ndp3 = __exprtab[__xndi + 1];
     if (!ndp->unsiznum || ndp->ibase != BDEC || ndp3->unsiznum
      || ndp3->sizdflt) goto not_2tok_num;
     __xndi++;
    }

not_2tok_num:
   /* this copies any constant value from __exprtab[] */
   ndp = alloc_xtnd(__xndi);
   __xndi++;
   return(ndp);
  case LCB:
   ndp2 = parse_concat();
   return(ndp2);
  case ID:
   idndp = alloc_xtnd(__xndi);
   xidp = __expr_idtab[__xndi];
   /* DBG remove --- */
   if (xidp == NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */

   /* move one past ID */
   __xndi++;
   ndp = __exprtab[__xndi];

   /* special case determine and return no argument sys fcall */
   /* system function no args legal - $ no ( implies function call */
   /* for range expr. ndp sy == NULL true */
   /* ID that is system func call, can not be glb. ref. */
   /* LOOKATME - can access functions start with $ - think not */
   if (xidp->idnam[0] == '$' && ndp->optyp != LPAR)
    {
     if ((syp = __get_sym(xidp->idnam, __syssyms)) == NULL)
      {
       __pv_ferr(683,
        "call of unknown built-in or unregistered PLI system function %s illegal - for dynamic, register generic place holder",
        xidp->idnam);
       return(NULL); 
      }
     /* attempt to call ID starting with $ (must be system task?) illegal */
     if (syp->sytyp != SYM_SF)
      {
       __pv_ferr(681,
        "illegal call of system task %s - only functions calleable",
        syp->synam);
       return(NULL); 
      }

     idndp->lu.sy = syp;
     /* error if system function used in xmr */
     if (ndp->optyp == DOT)
      {
       __pv_ferr(1127,
        "call of system function %s followed by '.' -  illegal hierarchical reference",
        idndp->lu.sy->synam);
       return(NULL);
      }
     ndp2 = __alloc_newxnd();
     ndp2->optyp = FCALL;
     ndp2->lu.x = idndp;
     ndp2->ru.x = NULL;
     return(ndp2);
    }


   /* if system func with args, continue to normal fcall */
   if (ndp->optyp == LSB)
    {
     /* inst array ref. then fix global resolution code to make select */
     /* on error returns nil - can not parse possible rest of expr */
     if ((ndp2 = parse_select(idndp)) == NULL) return(NULL);

     /* xndi one past end of select - xmr is special case */ 
     /* routine will not add path components to current sym table */
     if (__exprtab[__xndi]->optyp == DOT)
      {
       if ((glbndp = parse_glbref(ndp2, xidp)) == NULL) return(NULL);
       if (__exprtab[__xndi]->optyp != LPAR) return(glbndp); 

       /* xmr followed by ( is function call - 1st comp. inst arr case */
       /* can not be system func and therefore '(' required */ 
       ndp2 = parse_fcall(glbndp, NULL, TRUE);
       return(ndp2);
      }
     /* part select never legal for one comp array of insts select */ 
     if (__allow_scope_var && ndp2->optyp == LSB)
      {
       /* if already declared as net t in current env., know not glb */
       /* must also be net - since net is never xmr */
       if ((syp = __get_sym_env(xidp->idnam)) != NULL && syp->sytyp == SYM_N)
        goto do_decl;

       /* notice this can not fail - make ID into global ref. */
       return(bld_1cmp_global(ndp2, xidp));
      }

do_decl:
     /* not xmr - declare the ID symbol */
     if (!decl_id_inexpr(idndp, xidp)) return(NULL);
     return(ndp2);
    }
   if (ndp->optyp == LPAR)
    {

     /* fcall can be xmr, handle in global parsing routine */ 
     /* this works because user function (no $ first char) must have args */
     ndp2 = parse_fcall(idndp, xidp, FALSE);
     return(ndp2);
    }
   /* simple ID - current location one after ID */
   /* non special terminal - next is lower precedence rest of expr */
   if (__exprtab[__xndi]->optyp == DOT)
    {
     if ((glbndp = parse_glbref(idndp, xidp)) == NULL) return(NULL);

     if (__exprtab[__xndi]->optyp != LPAR) return(glbndp);

     /* xmr followed by ( is function call - non inst array 1st comp case */
     ndp2 = parse_fcall(glbndp, NULL, TRUE);
     return(ndp2);
    }  
   if (__allow_scope_var)
    {
     /* notice this can not fail - make ID into global ref. */
     return(bld_1cmp_global(idndp, xidp));
    }

   if (!decl_id_inexpr(idndp, xidp)) return(NULL);
   return(idndp);
  case DOT:
   /* need exact error for ID that starts with . */
   __pv_ferr(1128,
    "hierarchical reference or real constant initial '.' illegal");
   return(NULL);
  default:
   if (__pv_err_cnt <= __saverr_cnt)
    __pv_ferr(1116, "expression terminal expected - %s read",
     to_xndnam(__xs, __xndi));
  }
 return(NULL);
}


/*
 * routine to find symbol in symbol table adding if needed
 *
 * must not call until ID known not to be part of xmr
 * name of variable taken from expr id nam global table
 */
static int32 decl_id_inexpr(struct expr_t *ndp, struct expridtab_t *xidp)
{ 
 struct sy_t *syp;
 struct net_t *np;

 /* routine never called for function call or task enable */
 /* DBG remove --- */
 if (xidp->idnam[0] == '$') __arg_terr(__FILE__, __LINE__);
 /* --- */

 ndp->lu.x = NULL;
 /* since parse range as select with dummy var leave sy field nil for "[]" */
 if (xidp->idnam[0] == '[') return(TRUE); 

 if ((syp = __get_sym_env(xidp->idnam)) == NULL)
  {
   /* undeclared specify declared as net var in current sym tab */
   if (__cur_declobj == SPECIFY)
    syp = __decl_sym(xidp->idnam, __venviron[__top_sti]);
   /* undeclared var. declared at module level because task/block */
   /* declaration must come first and therefore will be declared */
   else 
    {
     if (__iact_state)
      {
       /* interactive statement - all variables must be declared */
       /* for $dumpvars will avoid this code and jump to assume glb */
       __pv_err_cnt++;
       if (strcmp(xidp->idnam, "") == 0)
        {
         __ia_err(1436,
          "escaped identifier empty - probable white space before escaped continuation new line");
        }
       else __ia_err(1436, "variable %s not declared", xidp->idnam);
       return(FALSE);
      }
     else syp = __decl_sym(xidp->idnam, __venviron[0]);
    }

   np = __add_net(syp);
   /* assume register - this is forward decl */
   /* even if in specify assume wire and fix up when declared */
   np->ntyp = N_REG;
   syp->sylin_cnt = xidp->idlin_cnt;
   syp->syfnam_ind = xidp->idfnam_ind;
  }
 ndp->lu.sy = syp;
 return(TRUE);
}

/*
 * find a a system task/func symbol (know name starts with $)
 * must be system function or task (maybe PLI)
 *
 * notice no special processing of scale arg. since must be var.
 * in module from which scale taken
 */
static struct sy_t *find_systf_sym(struct expridtab_t *xidp)
{
 struct sy_t *syp;

 /* look up in system task/func symbol table - must be there*/
 /* PLI redefine legal and will be in this symbol table */
 if ((syp = __get_sym(xidp->idnam, __syssyms)) == NULL)
  {
   if (__expr_is_lval)
    __gferr(1102, xidp->idfnam_ind, xidp->idlin_cnt,
     "enable of unknown system task or undefined PLI task %s",
     xidp->idnam);
   else
    __gferr(1102, xidp->idfnam_ind, xidp->idlin_cnt,
     "call of unknown system or undefined PLI function %s",
     xidp->idnam);
   return(NULL);
  }
 /* if lvalue, must be system table enable */
 if (__expr_is_lval)
  { 
   if (syp->sytyp != SYM_STSK)
    { 
     __gferr(1100, xidp->idfnam_ind, xidp->idlin_cnt,
      "attempt to enable (not call) system function %s", xidp->idnam);
     return(NULL);
    }
  }
 else
  {
   if (syp->sytyp != SYM_SF)
    {
     __gferr(1103, xidp->idfnam_ind, xidp->idlin_cnt,
      "call of system task %s illegal", xidp->idnam);
     return(NULL);
    }
  }
 return(syp);
}

/*
 * parse a concatenate
 * know current expr. token is leading { and points to one after }
 * on non error (non NULL)  return
 *
 * form is: {el, el, ..., el}, where el. is <expr.>, { concat, or
 * <expr> concat
 *
 * must be optimized after building tree
 * for now optimize just remove one element concatenates and dups
 * rep concat. form
 */
static struct expr_t *parse_concat(void)
{
 struct expr_t *ndp, *ndp2, *argndp, *last_ndp;
 struct expr_t *lop;

 ndp2 = alloc_xtnd(__xndi);
 __xndi++;
 ndp2->lu.x = NULL;

 for (last_ndp = NULL;;)
  {
   if ((argndp = parse_qcexpr()) == NULL) return(NULL);
add_to_tree:
   ndp = __exprtab[__xndi];

   /* processing depends on thing after 1st concat el. */
   switch ((byte) ndp->optyp) {
    case COMMA:
     /* build node since at least one more concat element */
     lop = __alloc_newxnd();
     lop->optyp = CATCOM;
     if (last_ndp == NULL) ndp2->ru.x = lop; else last_ndp->ru.x = lop;
     lop->lu.x = argndp;
     last_ndp = lop;
     /* must skip over comma */
     __xndi++;
     break;
    case RCB:
     /* done - notice last CATCOM node has NULL right pointer */
     lop = __alloc_newxnd();
     lop->optyp = CATCOM;
     if (last_ndp == NULL) ndp2->ru.x = lop; else last_ndp->ru.x = lop;
     lop->lu.x = argndp;
     lop->ru.x = NULL;
     __xndi++;
     return(ndp2);
    case LCB:
     {
      struct expr_t *repndp, *catndp;

      repndp = argndp;
      if ((catndp = parse_concat()) == NULL) return(NULL);

      argndp = __alloc_newxnd();
      argndp->optyp = CATREP;
      argndp->lu.x = repndp;
      argndp->ru.x = catndp;
     }
     goto add_to_tree;
    default:
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1119,
       "nested concatenate or comma separator expected - %s read",
       to_xndnam(__xs, __xndi));
     /* as special case for concat, must skip to } if possible */
     __vskipto_any(RCB);
     return(NULL);
    }
  }
}

/*
 * parse a select 
 * copies into malloced storage and return select expr subtree
 * know id ndp already copied to malloced storage
 *
 * [ read - reads one past ending ] 
 */
static struct expr_t *parse_select(struct expr_t *idndp)
{
 struct expr_t *ndp, *sel_ndp, *i1ndp, *i2ndp;

 /* build (alloc and copy) select sub expression root */
 sel_ndp = alloc_xtnd(__xndi);
 __xndi++;
 /* side effect of parsing sub expression is advancing global x ndi */
 /* returned i1ndp is malloced not built in __exprtab */
 if ((i1ndp = parse_qcexpr()) == NULL) return(NULL);
 sel_ndp->lu.x = idndp;
 ndp = __exprtab[__xndi];
 if (ndp->optyp == RSB)
  {
   sel_ndp->ru.x = i1ndp;
   __xndi++;
   /* next may be dot (handled as term by caller) or rest of expr */
   return(sel_ndp);
  }
 /* part select - if illegal in context caller must detect */
 if (ndp->optyp != COLON)
  {
   if (__pv_err_cnt <= __saverr_cnt)
    __pv_ferr(1114,
     "bit/part select expression : or ] separator expected - %s read",
     to_xndnam(__xs, __xndi));
   /* if T, current is one past ending ] */
   return(NULL);
  }
 /* part select after : */
 __xndi++;
 if ((i2ndp = parse_qcexpr()) == NULL) return(NULL);
 ndp = __exprtab[__xndi];
 if (ndp->optyp != RSB)
  {
   if (__pv_err_cnt <= __saverr_cnt)
    __pv_ferr(1115,
     "part select expression ending ] expected - %s read",
     to_xndnam(__xs, __xndi));
   return(NULL);
  }
 sel_ndp->optyp = PARTSEL;
 ndp = alloc_xtnd(__xndi);
 ndp->optyp = COLON;
 sel_ndp->ru.x = ndp;
 ndp->lu.x = i1ndp;
 ndp->ru.x = i2ndp;
 /* move one past RSB if possible */
 if (__xndi < __last_xtk) __xndi++;
 /* part select never xmr */
 return(sel_ndp);
}

/*
 * parse and and build global record for xmr
 *
 * current expr table node is one past id (dot or [) when called
 * reads one past end of xmr
 * argument is xmr first component expression and id name tab element
 * returns expression that may be select (if var tail is select)
 *
 * allowing constant selects for arrays of instances and gates  
 * for say a[a+b].b.c.w[kk], w[kk] is bit select of variable 
 * for xmr instance allowed context - gets fixed up to instance
 *
 * changes IDs where unparsed name in expr id tab move to
 * expr node with special XMRID node type
 */
static struct expr_t *parse_glbref(struct expr_t *cmp1_ndp,
 struct expridtab_t *cmp1_xidp)
{
 int32 is_sel;
 struct expr_t *ndp, *glbndp, *last_cmp;
 struct expr_t *idndp, *ndp2, *cmp_ndp; 
 struct expridtab_t *xidp;
 struct gref_t *grp;

 /* expr. has top level global then each xmr comma left points to id */
 /* or select for inst or gate array */
 glbndp = __alloc_newxnd();
 glbndp->optyp = GLBREF;
 glbndp->ru.x = __alloc_newxnd();
 glbndp->ru.x->optyp = XMRCOM;
 glbndp->ru.x->lu.x = cmp1_ndp;

 if (cmp1_ndp->optyp == LSB)
  {
   /* select already parsed and bsel expr. built */
   /* cmp ndp ru field set to idnam for first comp. of global */  
   cmp1_ndp->lu.x->ru.qnchp = __pv_stralloc(cmp1_xidp->idnam);
   /* current xndi now one past end of select */ 
   cmp1_ndp->lu.x->optyp = XMRID;
  }
 else if (cmp1_ndp->optyp == ID)
  {
   /* unused ru field used for name, lu.sy to xmr dest. filled later */
   cmp1_ndp->ru.qnchp = __pv_stralloc(cmp1_xidp->idnam);
   cmp1_ndp->optyp = XMRID;
  }
 else if (cmp1_ndp->optyp != DOT)
  {
   /* notice here gnam no build and rest of gref expr no read */
   __pv_ferr(682, "hierarchical reference contains illegal operator %s",
    __to_opname(cmp1_ndp->optyp));
   return(NULL);
  }

 /* DBG remove --- */
 if (__exprtab[__xndi]->optyp != DOT) __arg_terr(__FILE__, __LINE__);
 /* --- */

 /* last comp is always right tree end expr. comp */
 last_cmp = glbndp->ru.x;
 /* know this will be dot at start */
 for (;;)
  {
   /* move one past dot */ 
   __xndi++;
   ndp = __exprtab[__xndi];
   
   /* only use of dot in expr is for xmr - must be followed by ID */
   if (ndp->optyp != ID) 
    {
     __pv_ferr(1136,
      "hierarchical reference '.' separator not followed by identifier: %s read",
      to_xndnam(__xs, __xndi));
     return(NULL);
    }
   idndp = alloc_xtnd(__xndi);
   xidp = __expr_idtab[__xndi];
   /* DBG remove --- */
   if (xidp == NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */

   /* move past identifier to dot separator or [ select - any other end */
   __xndi++;
   ndp = __exprtab[__xndi];
   /* assume not a end of xmr select */ 
   is_sel = FALSE;
   /* access possible select - if not select add ID */ 
   if (ndp->optyp == LSB)
    {
     /* cmp ndp copied to malloced storage */
     /* in select non xmr expressions added to sym table since */
     /* they exist in reference context */
     if ((cmp_ndp = parse_select(idndp)) == NULL) return(NULL);
     /* current xndi now one past end of select */ 
     cmp_ndp->lu.x->ru.qnchp = __pv_stralloc(xidp->idnam);
     cmp_ndp->lu.x->optyp = XMRID;
     /* move to next which must be dot after ending ] */
     ndp = __exprtab[__xndi];
     if (!__allow_scope_var) is_sel = TRUE;
     /* inst. part select inside xmrs illegal */
     if (cmp_ndp->optyp == PARTSEL && ndp->optyp == DOT)
      {
       __pv_ferr(682,
        "hierarchical reference part select of array of instances illegal");
       return(NULL);
      }
    } 
   /* know seen ID not followed by . - done */ 
   else 
    {
     /* simple ID component (maybe at end) */ 
     cmp_ndp = idndp;
     cmp_ndp->ru.qnchp = __pv_stralloc(xidp->idnam);
     cmp_ndp->optyp = XMRID;
    }

   if (ndp->optyp != DOT)
    {
     /* end of xmr - not followed by dot - must be end of expr */ 
     if (!is_sel)
      {
       /* end is simple ID but must not be added to symbol table */
       ndp2 = __alloc_newxnd();
       ndp2->optyp = XMRCOM;
       ndp2->lu.x = cmp_ndp;
       ndp2->ru.x = NULL;
       last_cmp->ru.x = ndp2; 
      }
     else
      {
       /* need to add comp ID to end as ID not select because select of xmr */
       ndp2 = __alloc_newxnd();
       ndp2->optyp = XMRCOM;
       ndp2->lu.x = cmp_ndp->lu.x;
       ndp2->ru.x = NULL;
       last_cmp->ru.x = ndp2;

       cmp_ndp->lu.x = glbndp;
       glbndp = cmp_ndp;
      }
     break;
    }

   /* if followed by '.', add to tail of xmr expr and get next cmp */
   ndp2 = __alloc_newxnd();
   ndp2->optyp = XMRCOM;
   /* know cmp ndp copied to malloced storage */ 
   ndp2->lu.x = cmp_ndp;
   ndp2->ru.x = NULL;
   last_cmp->ru.x = ndp2; 
   last_cmp = ndp2;
  } 
 /* tail component is select - assume wire - i.e. not part of inst arr */ 
 /* rare inst. allowd context fixed up later by caller if needed */
 /* cmp ndx rotated to become top of select expr. */
 /* first add ID part of select to end of xmr */
 if (is_sel)
  {
   grp = __bld_glbref(glbndp->lu.x, cmp1_xidp->idfnam_ind,
    cmp1_xidp->idlin_cnt);
  }
 else
  {
   /* know x ndi pointing to one past end of xmr */
   grp = __bld_glbref(glbndp, cmp1_xidp->idfnam_ind, cmp1_xidp->idlin_cnt);
  }

 /* for interactive now can resolve global (case for ID in expr.) */
 if (__iact_state)
  { __resolve_glbnam(grp); __fill_grp_targu_fld(grp); }
 return(glbndp);
}

/*
 * IDs when allow scope var on (scope system task contexts) make globals
 *
 * during resolution will be converted back to wires if needed
 * for these special contexts since need scope ref. even if net with
 * same name instance takes precedence (conflict can be labeled block
 * local variable versus task name)
 */
static struct expr_t *bld_1cmp_global(struct expr_t *ndp,
 struct expridtab_t *xidp)
{
 struct expr_t *glbndp, *cmp_ndp;
 struct gref_t *grp;

 /* build the one component */ 
 cmp_ndp = ndp;
 if (ndp->optyp == ID)
  {
   cmp_ndp->ru.qnchp = __pv_stralloc(xidp->idnam);
   cmp_ndp->optyp = XMRID;
  }
 else
  {
   /* build the one component as select form */ 
   cmp_ndp = ndp;
   cmp_ndp->lu.x->optyp = XMRID;
   cmp_ndp->lu.x->ru.qnchp = __pv_stralloc(xidp->idnam);
  }

 /* build the top level global */
 glbndp = __alloc_newxnd();
 glbndp->optyp = GLBREF;
 glbndp->ru.x = __alloc_newxnd();
 glbndp->ru.x->optyp = XMRCOM;
 glbndp->ru.x->lu.x = cmp_ndp;

 grp = __bld_glbref(glbndp, xidp->idfnam_ind, xidp->idlin_cnt);

 /* for interactive now can resolve global (case for ID in expr.) */
 if (__iact_state)
  {
   /* if really local since dumpvars illegal, caller emits error */
   __resolve_glbnam(grp);
   if (!grp->gr_gone && !grp->gr_err) __fill_grp_targu_fld(grp);
  }
 return(glbndp);
}

/*
 * build a global reference entry in work table and convert glb ref ndp
 *
 * later work table is copied into right size allocated mod table
 *
 * at this build point, only link if from expression
 * caller must set line and file location
 */
extern struct gref_t *__bld_glbref(struct expr_t *glbndp, int32 gfnam_ind,
 int32 glin_cnt)
{
 struct gref_t *grp;
 struct expr_t *glbpth_ndp;
 char *gnam;

 /* convert global expr to name */
 /* for inst/gate arrays later will free and regenerate with const indices */
 /* notice gnam malloced because xmr expr. can be long here */
 /* and malloced to exact size of string (plus room for ending \0) */
 gnam = alloc_glbndp_tostr(glbndp);

 if ((++__grwrknum) >= __grwrktabsiz) grow_grtab();   
 grp = &(__grwrktab[__grwrknum - 1]);

 /* SJM 03/19/00 - gnam malloced above, now linked onto grp - must not free */
 init_gref(grp, gnam);
 grp->grfnam_ind = gfnam_ind;
 grp->grflin_cnt = glin_cnt;

 /* first save global as path expr. then convert to gref */
 /* must use glbref because that expr. is linked correctly in source stmts */
 glbpth_ndp = glbndp->ru.x; 
 grp->gxndp = glbndp;
 grp->gxndp->ru.grp = grp;

 /* link global reference as XMR expr. off of gref */ 
 grp->glbref = __alloc_newxnd();
 grp->glbref->optyp = GLBPTH;
 grp->glbref->ru.x = glbpth_ndp;

 /* lu.x gets filled later with destination symbol (of net usually) */
 /* DBG remove --- */
 if (grp->gxndp->lu.x != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */

 /* accessed through symbol field to gref */
 if (__allow_scope_var) grp->gr_inst_ok = TRUE;
 if (__iact_state)
  {
   /* for interact global, current scope determines */
   if (__scope_tskp != NULL) grp->grsytp = __scope_tskp->tsksymtab;
   else grp->grsytp = __scope_ptr->itip->imsym->el.emdp->msymtab;
   grp->gin_mdp = __scope_ptr->itip->imsym->el.emdp;
   /* for interactive will be resolved when expression parsed */ 
  }
 else
  {
   /* this is the symbol table gref appears in */
   grp->grsytp = __venviron[__top_sti];
   grp->gin_mdp = __inst_mod;
  }
 return(grp);
}

/*
 * grow global gref work table
 */
static void grow_grtab(void)
{
 int32 old_grtsiz, osize, nsize;

 old_grtsiz = __grwrktabsiz;
 osize = old_grtsiz*sizeof(struct gref_t);
 __grwrktabsiz = (3*__grwrktabsiz)/2; 
 nsize = __grwrktabsiz*sizeof(struct gref_t);
 __grwrktab = (struct gref_t *) __my_realloc((char *) __grwrktab, osize,
  nsize);
}

/*
 * generate a name for a global refence
 *
 * uses msg expr to string mechanism for instance array selects
 * does not assume select is constant expression
 *
 * LOOKATME - assume xmr component name ID[<expr>] is name and
 * therefore expression plus ID must be less the ID max length
 */
static char *alloc_glbndp_tostr(struct expr_t *glbndp)
{ 
 register struct expr_t *gcmp_ndp, *ndp;
 int32 slen, glen, gstrsiz;
 char *chp, *gnam, s1[IDLEN];

 /* DBG remove --- */
 if (glbndp->optyp != GLBREF) __arg_terr(__FILE__, __LINE__);
 /* --- */
 /* this is starting size only - can grow */
 gstrsiz = IDLEN;
 gnam = __my_malloc(gstrsiz);
 /* must move right down tree one to get first component */
 for (gcmp_ndp = glbndp->ru.x, glen = 0;;) 
  {
   ndp = gcmp_ndp->lu.x;
   if (ndp->optyp == XMRID) chp = ndp->ru.qnchp;
   else if (ndp->optyp == LSB || ndp->optyp == PARTSEL)
    {
     /* build the <id>[<expr>] expression as string */
     /* works because XMRID nodes know to dmp expr */
     chp = __msgexpr_tostr(s1, ndp);
    }
   else { __arg_terr(__FILE__, __LINE__); return(NULL); }

   /* DBG LINT remove -- */
   if (chp == NULL) __misc_terr(__FILE__, __LINE__);
   /* -- */

   slen = strlen(chp); 
   /* add room for '.' separator */
   if (glen != 0) slen++;
   /* need room for one final \0 to end string */
   if (glen + slen + 1 >= gstrsiz) 
    {
     gnam = __my_realloc(gnam, gstrsiz, gstrsiz + IDLEN);
     gstrsiz += IDLEN;
    }
   if (glen == 0) strcpy(gnam, chp);
   else { gnam[glen] = '.'; strcpy(&(gnam[glen + 1]), chp); } 
   glen += slen;

   if ((gcmp_ndp = gcmp_ndp->ru.x) == NULL) break;
   /* DBG remove --- */
   if (gcmp_ndp->optyp != XMRCOM) __arg_terr(__FILE__, __LINE__);
   /* --- */   
  }
 gnam[glen] = '\0';
 /* realloc to exact string size */
 if (glen + 1 != gstrsiz)
  {
   gnam = __my_realloc(gnam, gstrsiz, glen + 1);
  }
 return(gnam);
}

/*
 * convert glb ref expression into string
 *
 * when first read now since do not know index constant expression values  
 * is printed name - fixed later to use number indices
 */

/*
 * allocate a input phase global ref. record
 *
 * BEWARE - gnam must be malloced not fixed string
 */
static void init_gref(struct gref_t *grp, char *gnam)
{
 grp->gnam = gnam;
 grp->gr_err = FALSE;
 /* assume in decl. code */
 grp->upwards_rel = FALSE;
 grp->is_upreltsk = FALSE;
 grp->is_rooted = FALSE;
 grp->path_has_isel = FALSE;
 grp->pathisel_done = FALSE;
 grp->gr_inst_ok = FALSE;
 grp->gr_defparam = FALSE; 
 grp->gr_gone = FALSE;

 grp->gxndp = NULL;
 grp->glbref = NULL;
 grp->grsytp = NULL;
 grp->targmdp = NULL;
 grp->targtskp = NULL;
 grp->grfnam_ind = 0;
 grp->grflin_cnt = 0;
 grp->grcmps = NULL;
 grp->grxcmps = NULL;
 grp->last_gri = -1;
 grp->targu.targitp = NULL;
 grp->targsyp = NULL;
 grp->spltgrp = NULL;
}

/*
 * parse a function call
 * called when ( seen immediately after
 * know __xndi points to ( and moves one past ending )
 * ,, form legal for system functions but not user - caught elsewhere
 */
static struct expr_t *parse_fcall(struct expr_t *fcall_ndp,
 struct expridtab_t *xidp, int32 is_glb)
{
 int32 is_sysfunc;
 struct expr_t *ndp, *fchdrx, *argndp, *last_ndp, *lop;

 is_sysfunc = FALSE;
 /* for non global function call, check symbol type and add if needed */ 
 if (!is_glb)
  {
   /* only fails if interactive and unable to find - stmt parser handles */
   /* error skipping */
   if (!chk_decl_func(&is_sysfunc, fcall_ndp, xidp)) return(NULL);
  }
 else
  {
   /* because can not tell if select of last component is from wire */
   /* error if global is select - no other checking possible here */
   if (fcall_ndp->optyp != GLBREF)
    {
     __pv_ferr(1137,
      "hierarchical function call name %s illegal select",
      __msgexpr_tostr(__xs, fcall_ndp));
     return(NULL);
    }
  }

 fchdrx = alloc_xtnd(__xndi);
 fchdrx->optyp = FCALL;
 fchdrx->lu.x = fcall_ndp;
 __xndi++;
 ndp = __exprtab[__xndi];
 if (ndp->optyp == RPAR && !is_sysfunc)
  {
   __pv_ferr(1120,
   "Verilog user defined function call requires at least one argument - () illegal");
   return(NULL);
  }
 /* now checking moved 1 past 1st token of 1st expr. */
 /* expr. structure identical to concatenate , present even for 1 arg. */
 /* can trace through br x rights until NULL */
 /* task port list for functs has extra 1st ret. argument but not expr. */
 for (last_ndp = fchdrx;;)
  {
   ndp = __exprtab[__xndi];
   /* case 1: empty expr. only allowed for built-in or PLI sysfuncs */
   if (ndp->optyp == COMMA || ndp->optyp == RPAR)
    {
     if (!is_sysfunc)
      {
       __pv_ferr(1121,
        "user function %s call empty ',,' argument form illegal",
        __to_idnam(fcall_ndp));
       argndp = __alloc_newxnd();
       /* user function error, make 1 bit x */
       __set_numval(argndp, 1L, 1L, 1);
      }
     else
      { 
       /* must allocate new xnode because need both ,'s of ,, */
       argndp = __alloc_newxnd();
       argndp->optyp = OPEMPTY;
       argndp->folded = TRUE;
      }
    }
   else
    {
     /* case 2: actual expr */
     /* case 2a: PLI system function - xmr arguments legal for any arg */ 
     if (is_sysfunc)
      {
       struct sy_t *syp;
       struct sysfunc_t *sfbp;

       syp = fcall_ndp->lu.sy;
       sfbp = syp->el.esyftbp;

       /* XMR's (scope args) legal for built-in system functions */
       /* but can never be function scope since functions always called */
       if (sfbp->syfnum >= BASE_VERIUSERTFS)
        {
         if (sfbp->syfnum > __last_systf) __misc_terr(__FILE__, __LINE__);

         /* PLI user defined system function - must allow */
         __allow_scope_var = TRUE;
        }
       /* turning on allow scope global - turns on conversion in here */
       if ((argndp = parse_qcexpr()) == NULL)
        {
         __allow_scope_var = FALSE;
         return(NULL);
        }
       /* must turn off allowing scope for both paths thru ifs */
       __allow_scope_var = FALSE;
      }
     else
      {
       /* case 2b: use defined (non sysf or PLI sysf)  function */
       /* SJM 07/26/99 - was return FALSE but same NULL */
       if ((argndp = parse_qcexpr()) == NULL) return(NULL);

        /* DBG remove ---
        if (__debug_flg)
         __dbg_msg("ARG FCALL: %s\n", __msgexpr_tostr(__xs, argndp));
        --- */
      } 
    }

   ndp = __exprtab[__xndi];
   if (ndp->optyp != RPAR && ndp->optyp != COMMA)
    {
     if (__pv_err_cnt <= __saverr_cnt)
      __pv_ferr(1122,
       "function call argument , or ) separator expected - %s read",
       __to_opname(ndp->optyp));
     return(NULL);
    }
   lop = __alloc_newxnd();
   lop->optyp = FCCOM;
   lop->ru.x = NULL;
   lop->lu.x = argndp;
   last_ndp->ru.x = lop;
   last_ndp = lop;
   __xndi++;
   if (ndp->optyp == RPAR) break;
  }
 /* DBG remove ---
 if (__debug_flg)
  __dbg_msg("FCALL: %s\n", __msgexpr_tostr(__xs, fchdrx));
 --- */
 return(fchdrx);
}

/*
 * check and declare a non xmr function call expression
 *
 * know ID and following '(' read - must find or maybe add func symbol
 * xidp ok here since only declaring no movement in symbol table
 */
static int32 chk_decl_func(int32 *is_sysfunc, struct expr_t *idndp,
 struct expridtab_t *xidp)
{
 struct sy_t *syp;
 char s1[RECLEN];

 *is_sysfunc = FALSE;
 /* if starts with '$' must be system function */
 if (xidp->idnam[0] == '$')
  {
   if ((syp = find_systf_sym(xidp)) == NULL) return(FALSE);
   idndp->lu.sy = syp;
   *is_sysfunc = TRUE;
   return(TRUE);
  }
 /* SJM 02/01/05 - change to use tf undef work symtab - for uprel 1 cmp glb */
 /* look up to see if previously declared as function or used as func */
 if ((syp = __get_sym(xidp->idnam, __venviron[0])) == NULL)
  {
   /* for interactive must already be declared or error */
   if (__iact_state)
    {
     /* interactive statement - all variables must be declared */
     /* for $dumpvars will avoid this code and jump to assume glb */
     __pv_err_cnt++;
     __ia_err(1436, "function %s not declared", xidp->idnam);
     return(FALSE);
    }
   /* SJM 02/01/05 - must assume 1 comp XMR - if declared later will be */
   /* changed back in resolv local path routine */
   cnvt_forw_tfcall_1cmpglb(idndp, xidp->idnam, xidp->idfnam_ind,
    xidp->idlin_cnt);
   return(TRUE);
  }
 /* if already in symbol table as wire error - used as wire */
 idndp->lu.sy = syp;

 /* if declared (or just seen) but not as fcall error */
 if (syp->sytyp != SYM_F)
  {
   if (syp->sydecl)
    {
     __pv_ferr(1123,
      "%s called as function but symbol declared with type %s",
      __to_idnam(idndp), __to_sytyp(s1, syp->sytyp));
    }
   else
    {
     __pv_ferr(1123,
      "%s called as function but symbol previously used with type %s",
      __to_idnam(idndp), __to_sytyp(s1, syp->sytyp));
    }
   /* return T since can continue parsing with wrong type symbol */ 
  }
 return(TRUE);
}

/*
 * routine to build func call 1 comp global for forward refs
 *
 * special case because must convert top node to xmr and make left
 * offspring newly allocated with same contents
 *
 * needed because f([args]) may be upward relative xmr to f in above
 * module - can't tell so any function call use before declares must
 * be assumed to be XMRs - will get changed back if decl later in mod
 */
static void cnvt_forw_tfcall_1cmpglb(struct expr_t *ndp, char *tfnam, 
 int32 fnam_ind, int32 lin_cnt)
{
 struct expr_t *cmp_ndp, sav_xnod;
 struct gref_t *grp;

 /* DBG remove -- */
 if (ndp->optyp != ID) __misc_terr(__FILE__, __LINE__);
 /* -- */

 sav_xnod = *ndp;
 ndp->optyp = GLBREF;
 ndp->ru.x = __alloc_newxnd();
 ndp->ru.x->optyp = XMRCOM;
 cmp_ndp = __alloc_newxnd();
 *cmp_ndp = sav_xnod;
 ndp->ru.x->lu.x = cmp_ndp;
 ndp->ru.x->ru.x = NULL;

 cmp_ndp->ru.qnchp = __pv_stralloc(tfnam);
 cmp_ndp->optyp = XMRID;

 grp = __bld_glbref(ndp, fnam_ind, lin_cnt);
}

/*
 * convert event expression table to expression tree
 */
extern void __bld_evxtree(void)
{
 struct expr_t *ndp;

 /* add end of expression fence */
 ndp = __alloc_exprnd();
 ndp->optyp = UNDEF;
 __xndi = 0;
 if ((__root_ndp = parse_evexpr()) == NULL || __xndi != __last_xtk)
  {
   if (__pv_err_cnt <= __saverr_cnt)
    __pv_ferr(1124, "event expression error at token [%s]",
     to_xndnam(__xs, __xndi));
   __root_ndp = __alloc_newxnd();
   __set_numval(__root_ndp, 1L, 1L, 1);
  }
 /* if (__debug_flg) __dbg_msg(__msgexpr_tostr(__xs, __root_ndp)); */
}

/*
 * parse a specialized event expression
 */
static struct expr_t *parse_evexpr(void)
{
 struct expr_t *ndp, *ndp2, *ndp3, *ndp4, *rhsndp, *leftopndp;

 /* SJM 08/22/00 - need to separate of possible leading pos/neg edge */
 /* because they apply to entire rest of expr to end or ev or */
 ndp = __exprtab[__xndi];
 if (ndp->optyp == OPPOSEDGE || ndp->optyp == OPNEGEDGE)
  {
   ndp3 = alloc_xtnd(__xndi);
   __xndi++;
  }
 else ndp3 = NULL;

 /* routine leaves __xndi at next place to look */
 if ((leftopndp = parse_qcexpr()) == NULL) return(NULL);
 if (ndp3 != NULL) { ndp3->lu.x = leftopndp; leftopndp = ndp3; }

 ndp = __exprtab[__xndi];
 for (;;)
  {
   if (ndp->optyp == OPEVOR || ndp->optyp == OPEVCOMMAOR)
    {
     ndp2 = alloc_xtnd(__xndi);
     __xndi++;

     /* SJM 08/22/00 - need to separate of possible leading pos/neg edge */
     /* because they apply to entire rest of expr to end or ev or */
     ndp4 = __exprtab[__xndi]; 
     if (ndp4->optyp == OPPOSEDGE || ndp4->optyp == OPNEGEDGE)
      {
       ndp3 = alloc_xtnd(__xndi);
      __xndi++;
      }
     else ndp3 = NULL;

     if ((rhsndp = parse_qcexpr()) == NULL) break;
     if (ndp3 != NULL) { ndp3->lu.x = rhsndp; rhsndp = ndp3; }

     /* this makes sure optyp right */
     ndp2->lu.x = leftopndp;
     ndp2->ru.x = rhsndp;
     leftopndp = ndp2;
     ndp = __exprtab[__xndi];
    }
   else break;
  }
 return(leftopndp);
}

/*
 * LOW LEVEL EXPRESSION NODE ROUTINES
 */

/*
 * convert a symbol into a expression 
 *
 * easy because all the normal work of parse term for ID is  
 * declaring symbol
 */
extern struct expr_t *__gen_wireid_expr(struct sy_t *syp)
{
 struct expr_t *ndp;

 ndp = __alloc_newxnd();
 ndp->lu.sy = syp;
 ndp->optyp = ID;
 return(ndp);
} 

/*
 * allocate an expression node in fixed current expr. array
 *
 * this sets node type to be operator
 * if used as number must have a/b val allocated
 * when allocated for expr. allocated a/b values used
 */
extern struct expr_t *__alloc_exprnd(void)
{
 struct expr_t *ndp;

 /* SJM 11/22/01 - can't use fixed operator precedence xpr tab */
 /* need malloced for very large bus concatenates */
 if (++__last_xtk >= __exprtabsiz - 1) grow_exprtab();
 if ((ndp = __exprtab[__last_xtk]) == NULL)
  {
   ndp = my_xndalloc();
   __exprtab[__last_xtk] = ndp;
  }
 __init_xnd(ndp);
 return(ndp);
}

/*
 *
 * grow exprtab used during operator precedence expr tab * parsing
 *
 * can realloc because contains ptr's to malloced expr nodes where
 * node but not table is linked together to form run time expr
 *
 * but notice expr id table must increased to same size to avoid
 * overflow check
 * 
 * SJM 10/22/01
 */
static void grow_exprtab(void)
{
 register int32 i;
 int32 old_xtabsiz, osize, nsize;

 old_xtabsiz = __exprtabsiz;
 osize = old_xtabsiz*sizeof(struct expr_t *);
 __exprtabsiz = (3*__exprtabsiz)/2; 
 nsize = __exprtabsiz*sizeof(struct expr_t *);
 __exprtab = (struct expr_t **) __my_realloc((char *) __exprtab,
  osize, nsize);
 for (i = old_xtabsiz - 1; i < __exprtabsiz; i++) __exprtab[i] = NULL;

 /* also must grow expr id table */
 osize = old_xtabsiz*sizeof(struct expridtab_t *);
 nsize = __exprtabsiz*sizeof(struct expridtab_t *);
 __expr_idtab = (struct expridtab_t **)
  __my_realloc((char *) __expr_idtab, osize, nsize);
 for (i = old_xtabsiz - 1; i < __exprtabsiz; i++) __expr_idtab[i] = NULL;
}

/*
 * allocate a expr ID table entry
 */
extern struct expridtab_t *__alloc_expridnd(char *idnam)
{
 int32 slen;
 struct expridtab_t *xidp;

 if ((xidp = __expr_idtab[__last_xtk]) == NULL)
  { 
   xidp = (struct expridtab_t *) __my_malloc(sizeof(struct expridtab_t));
   __expr_idtab[__last_xtk] = xidp;  
   xidp->idnam = NULL;
   xidp->idfldwid = 0;
  }
 slen = strlen(idnam); 
 if (slen + 1 > xidp->idfldwid) 
  {
   /* guess 2x so prevent increase by one byte allocs */
   xidp->idnam = __my_malloc(2*slen + 1);
   xidp->idfldwid = 2*slen + 1;
  }
 strcpy(xidp->idnam, idnam);
 xidp->idlin_cnt = __lin_cnt;
 xidp->idfnam_ind = __cur_fnam_ind;
 return(xidp);
}

/*
 * allocate an expr node 
 */
static struct expr_t *my_xndalloc(void)
{
 struct expr_t *ndp;

 ndp = (struct expr_t *) __my_malloc(sizeof(struct expr_t));
 ndp->optyp = UNDEF;
 if (__inst_mod != NULL) (__inst_mod->mexprnum)++;
 return(ndp);
}

/*
 * allocate a copy of __exprtab node in malloced storage
 * this uses the allocated values in the __exprtab node
 *
 * numeric values never changed because __exprtab node just points to
 * atokptr and btokptr and those values moved to malloced node
 */
static struct expr_t *alloc_xtnd(int32 ndi)
{
 struct expr_t *ndp, *ndp2;

 /* xtnd out of range */
 if (ndi < 0 || ndi > __last_xtk) __misc_terr(__FILE__, __LINE__);
 ndp = my_xndalloc();
 ndp2 = __exprtab[ndi];
 *ndp = *ndp2;
 return(ndp);
}

/*
 * initialize a new node - assume expression node for now
 */
extern void __init_xnd(struct expr_t *ndp)
{
 ndp->optyp = UNDEF;
 ndp->has_sign = FALSE;
 ndp->rel_ndssign = FALSE;
 ndp->is_string = FALSE;
 /* unless simple decimal, must be sized */
 ndp->unsiznum = FALSE;
 ndp->ibase = BHEX;
 ndp->sizdflt = FALSE;
 ndp->is_real = FALSE;
 ndp->cnvt_to_real = FALSE;
 ndp->unsgn_widen = FALSE;
 ndp->consubxpr = FALSE;
 ndp->consub_is = FALSE;
 ndp->folded = FALSE;
 ndp->getpatlhs = FALSE;
 ndp->ind_noth0 = FALSE;   /* assume h:0 form */
 ndp->x_multfi = FALSE;
 ndp->tf_isrw = FALSE;
 ndp->x_islhs = FALSE;
 ndp->locqualnam = FALSE;
 ndp->lhsx_ndel = FALSE;
 ndp->x_stren = FALSE;
 ndp->unc_pull = NO_UNCPULL; 
 ndp->szu.xclen = 0;
 ndp->lu.x = ndp->ru.x = NULL;
}

/*
 * allocate and initialize new expression node (may be table)
 * this initializes a node so if copied to number will have value alloced
 */
extern struct expr_t *__alloc_newxnd(void)
{
 struct expr_t *ndp;

 ndp = my_xndalloc();
 __init_xnd(ndp);
 return(ndp);
}

/*
 * run time (during sim or xform) allocate new xnode
 * does not get copied to mod's expr table
 */
extern struct expr_t *__sim_alloc_newxnd(void)
{
 struct expr_t *ndp;

 ndp = (struct expr_t *) __my_malloc(sizeof(struct expr_t));
 ndp->optyp = UNDEF;
 __init_xnd(ndp);
 return(ndp);
}

/*
 * set an collection routine exprtab entry to x
 * this must be called for empty expr. tab - all fields reclaimed
 */
extern void __set_xtab_errval(void)
{ 
 __last_xtk = 0;
 __init_xnd(__exprtab[0]);
 __set_numval(__exprtab[0], 1L, 1L, 1);
}

/*
 * set an collection routine exprtab entry to 0 - for specify
 * this must be called for empty expr. tab - all fields reclaimed
 */
extern void __set_0tab_errval(void)
{ 
 __last_xtk = 0;
 __set_numval(__exprtab[0], 0L, 0L, 1);
}

/*
 * free an expression tree
 * notice cannot free symbols or wires pointed to by these nodes
 * ndp now points to garbage - caller must unlink
 */
extern void __free_xtree(struct expr_t *ndp)
{
 if (ndp == NULL) return;
 /* first free underneath */
 __free2_xtree(ndp);
 /* then node itself */
 __my_free((char *) ndp, sizeof(struct expr_t));
}

/*
 * free all nodes under expression node and free number if needed
 * notice cannot free symbols or wires pointed to by these nodes
 * expects __inst_mod to be set if form is IS
 * caller to reuse ndp must change fields
 * 
 * notice freeing here assumes that all nodes have both a and b parts
 */
extern void __free2_xtree(struct expr_t *ndp)
{
 switch ((byte) ndp->optyp) {
  case NUMBER: case REALNUM: case OPEMPTY:
   ndp->ru.xvi = -1;
   return;
  case ISNUMBER: case ISREALNUM:
   /* LOOKATME - unable to free IS constant value parts - is this leak? */
   ndp->ru.xvi = -1;
   return;
  case ID:
   ndp->lu.sy = NULL;
   return;
  case GLBREF:
   ndp->lu.sy = NULL;
   /* globals free elsewhere - through glogal list */ 
   ndp->ru.grp = NULL;
   return;
  case XMRID:
   __my_free(ndp->ru.qnchp, strlen(ndp->ru.qnchp) + 1);
   /* DBG remove --- */
   ndp->ru.x = NULL;
   /* --- */
   return;
  /* fall thru for GLBPTH that has no contents - just subexprs under */
 }
 if (ndp->lu.x != NULL) __free_xtree(ndp->lu.x);
 if (ndp->ru.x != NULL) __free_xtree(ndp->ru.x);
}

/*
 * build an unc. expression - may have width set to 1 for now - know later
 * before calling must free exprtab and anything pointed to __root_ndp
 */
extern void __bld_unc_expr(void)
{
 __root_ndp = __alloc_newxnd();
 set2_opempty(__root_ndp);
}

/*
 * this build a 1 token empty expr that may or may not be parsed
 * this is for 1 bit expression
 *
 * this does not free partially built x trees - caller must free if needed
 */
extern void __set_opempty(int32 ndi)
{
 __init_xnd(__exprtab[ndi]);
 set2_opempty(__exprtab[ndi]);
}

/*
 * version of set opempty that is passed allocated expr. value
 */
static void set2_opempty(struct expr_t *ndp)
{
 ndp->optyp = OPEMPTY;
 ndp->folded = TRUE;
 ndp->szu.xclen = 1;
 /* if unconnected drive active, must set in node */
 if (__unconn_drive != TOK_NONE)
  {
   ndp->unc_pull = (__unconn_drive == PULL0) ? UNCPULL0 : UNCPULL1;
   ndp->x_stren = TRUE;
   /* must force propogating strengths up and down */
   /* but maybe no wire marked as strength */
   __design_no_strens = FALSE;
  }
}

/*
 * set an already allocated leaf node (no subnodes) to a numeric value
 * only for <= WBITS values and node must be new - know previous alloc num
 *
 * notice this must not change __root_ndp
 */
extern void __set_numval(struct expr_t *ndp, word32 av, word32 bv, int32 blen)
{
 ndp->optyp = NUMBER;
 if (blen <= WBITS) 
  {
   ndp->ru.xvi = __alloc_shareable_cval(av, bv, blen);
  }
 else __case_terr(__FILE__, __LINE__);
 ndp->szu.xclen = blen;
 if (bv == 0) ndp->ibase = BDEC;
}

/*
 * print an expression node for error messages
 * must be wide enough for id
 * this is a routine that uses wr to exprline routines so cannot be
 * called to add to exprline - must use disp of expr. routine
 *
 * BEWARE - can only be called during expression reading because needs
 *          __exprtab and __expridtab
 */
static char *to_xndnam(char *s, int32 xndi)
{
 int32 wlen;
 word32 *ap, *bp;
 struct expr_t *ndp;
 struct expridtab_t *xidp;
 char s1[2*IDLEN], s2[RECLEN];

 ndp = __exprtab[xndi];
 switch ((byte) ndp->optyp) {
  case ID:
   xidp = __expr_idtab[xndi];
   if (xidp == NULL || strcmp(xidp->idnam, "") == 0)
    { strcpy(s, "**range**"); return(s); }
   sprintf(s1, "IDENTIFIER: %s", xidp->idnam);
   break;
   /* LOOKATME - can GLBREF occur here */
  case NUMBER:
  case ISNUMBER:
   ap = &(__contab[ndp->ru.xvi]);
   wlen = wlen_(ndp->szu.xclen);
   bp = &ap[wlen];
   sprintf(s1, "NUMBER: %s", __regab_tostr(s2, ap, bp, ndp->szu.xclen,
    BHEX, FALSE));
   break;
  case REALNUM: case ISREALNUM:
   /* LOOKATME - better to just format as double */ 
   /* just pass a part for both here */ 
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
