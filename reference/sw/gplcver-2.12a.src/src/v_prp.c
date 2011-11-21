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

#include "vpi_user.h"

/* local prototypes */
static void rem_inc_dupes(void);
static void prep_udps(void);
static void prep1_udp(struct udp_t *);
static void prep_comb_udptab(struct udp_t *);
static void xpnd_1wcard(int32, word32);
static void init_utab(word32 *, int32);
static void chg_unfilled_tox(word32 *, int32);
static void bld_wcardtab(register char *, word32, word32);
static word32 bld_uinitndx(char *, word32, int32);
static void setchk_uval(word32);
static char *bld_udploc(char *, word32, word32, word32);
static char *udploc_to_line(char *, char *);
static void dmp_comb_udptab(struct udp_t *);
static void dmp_udp3v_tab(word32 *, word32);
static char *bld3vndx_str(char *, word32, word32);
static void dmp_udp2b_tab(word32 *, word32);
static int32 udmp_impossible_value(word32, word32);
static char *bldndx_str(char *, word32, word32);
static void dmp_edge_udptab(struct udp_t *);
static void dmp_udp3v_etab(word32 *, word32, int32, int32);
static void dmp_udp2b_etab(word32 *, word32, int32, int32);
static void prep_edge_udptab(struct udp_t *);
static void xpnd_edge_wcard(word32, int32, int32, word32);
static void free_udp_lines(struct udp_t *);
static void change_all_rngreps(void);
static void set_optim_nflds(struct net_t *);
static void free_ncablks(void);
static void emit_varunused_informs(struct net_t *, struct task_t *);
static void rt_change_rngrep(struct net_t *);
static void bld_gstate(void);
static void alloc_gstate(struct gate_t *, int32);
static word32 cmp_udpind(word32, word32);
static void prep_conta_dels(void);
static int32 rhs_cat_separable(struct expr_t *);
static int32 rhs_modpin_separable(struct expr_t *);
static void bld_pbsep_input_mpps(void);
static void bld_pbsep_output_mpps(void);
static int32 output_pb_separable(void);
static void bld_pb_mpps(struct mod_pin_t *);
static void bld_pb_contas(void);
static struct pbexpr_t *bld_pb_expr_map(struct expr_t *, int32);
static void init_pbexpr_el(struct pbexpr_t *);
static struct expr_t *bld_1sep_pbit_expr(struct pbexpr_t *, int32);
static struct expr_t *cnvt_to_bsel_expr(struct expr_t *, int32);
static void bld_nplist(void);
static void bld_lhsexpr_npins(struct expr_t *, int32);
static void bld2_lhsexpr_npins(struct expr_t *, int32);
static void bld_rhsexpr_npins(struct expr_t *, int32);
static void conn_rtxmr_npin(struct net_t *, int32, int32, int32, int32,
 struct gref_t *, int32, char *);
static void conn_xmr_npin(struct net_t *, int32, int32, int32, int32,
 struct gref_t *, int32, char *);
static struct net_pin_t *conn2_npin(struct net_t *, int32, int32, int32,
 int32);
static void set_chgsubfld(struct net_pin_t *, int32, char *);
static void add_netdel_pnp(struct net_t *, struct paramlst_t *);
static void init_pnp(struct parmnet_pin_t *);
static void addto_parmnplst(struct expr_t *, struct parmnet_pin_t *);
static void add_gatedel_pnp(struct gate_t *, struct paramlst_t *);
static void add_contadel_pnp(struct conta_t *, struct paramlst_t *);
static void free_1parm_pnps(struct net_t *);
static void realloc_npplist_to_tab(void);  
static void realloc_1net_npplist(struct net_t *);
static int32 cnt_npps(struct net_pin_t *);
static int32 cnt_dces(struct dcevnt_t *);
static void eat_gates(void);
static int32 has_muststay_npp(register struct net_pin_t *);
static void mark_muststay_wires(struct expr_t *);
static void eat_nets(int32);
static void rem_del_npps(void);
static void remove_all_npps(struct net_t *);
static void bld1vec_fifo(struct net_t *);
static void update_vec_fifo(struct net_t *, word32 *, int32 *, int32 *,
 int32 *);
static int32 wire_implied_driver(struct net_t *);
static void eat_cells(int32 *);
static int32 conn_expr_gone(struct expr_t *);
static void mark_maybe_gone_nets(struct expr_t *);
static void getbit_fifo(struct net_t *, int32, int32 *, int32 *);

/* extern prototypes (maybe defined in this module) */
extern void __prep_sim(void);
extern void __set_init_gstate(struct gate_t *, int32, int32);
extern void __set_init_udpstate(struct gate_t *, int32, int32);
extern void __conn_npin(struct net_t *, int32, int32, int32, int32,
 struct gref_t *, int32, char *);
extern struct net_pin_t *__alloc_npin(int32, int32, int32);
extern struct npaux_t *__alloc_npaux(void);
extern void __add_dctldel_pnp(struct st_t *);
extern void __add_tchkdel_pnp(struct tchk_t *, int32);
extern void __add_pathdel_pnp(struct spcpth_t *);
extern void __free_design_pnps(void);
extern int32 __get_acc_class(struct gate_t *);
extern int32 __add_gate_pnd0del(struct gate_t *, struct mod_t *, char *);
extern int32 __add_conta_pnd0del(struct conta_t *, struct mod_t *, char *);

extern void __prep_xmrs(void);
extern void __alloc_nchgaction_storage(void);
extern void __alloc_sim_storage(void);
extern void __bld_bidandtran_graph(void);
extern void __setchk_all_fifo(void);
extern void __prep_exprs_and_ports(void);
extern void __prep_contas(void);
extern void __prep_stmts(void);
extern void __set_nchgaction_bits(void);
extern void __set_optimtab_bits(void);
extern void __set_mpp_assign_routines(void);
extern void __set_pb_mpp_assign_routines(void);
extern void __set_mpp_aoff_routines(void);
extern void __dmpmod_nplst(struct mod_t *, int32);
extern void __do_decompile(void);
extern void __prep_specify(void);
extern void __show_allvars(void);
extern char *__my_malloc(int32);
extern void __my_free(char *, int32);
extern char *__to_uvvnam(char *, word32);
extern char *__to_wtnam(char *, struct net_t *);
extern char *__to_ptnam(char *, word32);
extern void __prep_delay(struct gate_t *, struct paramlst_t *, int32, int32,
 char *, int32, struct sy_t *, int32);
extern void __free_xtree(struct expr_t *);
extern void __init_vec_var(register word32 *, int32, int32, int32, word32,
 word32);
extern char *__to_mpnam(char *, char *);
extern int32 __isleaf(struct expr_t *);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern void __free_dellst(struct paramlst_t *);
extern void __free_del(union del_u, word32, int32);
extern void __bld_pb_fifo(struct net_t *, int32 *, int32 *, int32 *, int32);
extern int32 __gate_is_acc(struct gate_t *);
extern char *__bld_lineloc(char *, word32, int32);
extern void __allocinit_perival(union pck_u *, int32, int32, int32);
extern int32 __comp_ndx(register struct net_t *, register struct expr_t *);
extern void __rem_0path_dels(void);
extern int32 __chk_0del(word32, union del_u, struct mod_t *);
extern void __push_wrkitstk(struct mod_t *, int32);
extern void __pop_wrkitstk(void);
extern struct expr_t *__copy_expr(struct expr_t *);
extern int32 __cnt_cat_size(struct expr_t *);
extern struct expr_t *__alloc_newxnd(void);
extern struct expr_t *__bld_rng_numxpr(word32, word32, int32);
extern int32 __get_const_bselndx(register struct expr_t *);
extern void __getwir_range(struct net_t *, int32 *, int32 *);
extern int32 __is_const_expr(struct expr_t *);

extern void __cv_msg(char *, ...);
extern void __dbg_msg(char *, ...);
extern void __pv_ferr(int32, char *, ...);
extern void __gfinform(int32, word32, int32, char *, ...);
extern void __gfwarn(int32, word32, int32, char *, ...);
extern void __gferr(int32, word32, int32, char *, ...);
extern void __sgferr(int32, char *, ...);
extern void __sgfinform(int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);

extern void __vpi_err(int32, int32, char *, ...);

extern word32 __masktab[];

/*
 * SIMULATION PREPARATION ROUTINES
 */

/*
 * prepare for simulation
 */
extern void __prep_sim(void)
{
 register struct mod_t *mdp; 
 int32 sav_declobj;

 /* because checking delay expressions need object type global set */
 sav_declobj = __cur_declobj;
 __cur_declobj = MODULE;

 /* done reading source files - remove all duplicates from inc dbg list */
 if (__inclst_hdr != NULL) rem_inc_dupes();

 /* build per type udp tables - 1st since can free lots of storage */
 prep_udps();

 /* first change all wire ranges to constant form - need to build np list */
 change_all_rngreps();

 /* allocate and fill xmr table - used when building np list */
 __prep_xmrs();

 /* SJM 09/18/02 - build the per bit decomposed ports and iconns */
 bld_pbsep_input_mpps();
 bld_pbsep_output_mpps();
 bld_pb_contas();

 /* build the various net pin lists */
 bld_nplist();

 /* if errors building np list can't check further */
 if (__pv_err_cnt != 0) return;

 /* SJM 05/03/05 - now always allocate nchg action byte table separately */
 __alloc_nchgaction_storage();

 /* allocate storage for wire and regs (variables) and gate states */
 __alloc_sim_storage();

 bld_gstate();
 prep_conta_dels();

 /* build inout tran and tran channel connection graphs */
 __bld_bidandtran_graph();

 /* SJM 12/19/04 - after tran chans built, tran npps removed so can convert */
 /* npp list to a table - but still set npnxt since add/rem during sim */ 
 realloc_npplist_to_tab();  

 if (__gateeater_on) eat_gates();

 /* mark each wire that has multi fan in for any bit */
 __setchk_all_fifo();

 /* allocate inout port memory and mark expr. fi>1 */
 __prep_exprs_and_ports();

 /* special preparation for contas (alloc drive values) and check getpats */
 __prep_contas();

 /* modify statements where needed */
 /* this also needs sim storage to be allocated */
 __prep_stmts();

 /* new first step toward compiler optimization routines (this for ports) */ 
 if (__accelerate)
  {
   __set_mpp_assign_routines();
   __set_pb_mpp_assign_routines();
  }
 else __set_mpp_aoff_routines();

 /* SJM 07/19/02 - need to prep specify to add tchk and path loads */
 /* before setting nchg action bits */

 /* all modules processed, object now only specify */
 __cur_declobj = SPECIFY;
 __prep_specify();
 /* interactive must run in module declare object for expr. */
 __cur_declobj = sav_declobj;

 /* SJM 08/08/03 - order was wrong, this is needed before setting chg bits */
 /* LOOKATME - only setting optim fields for dmpvars so far */
 __set_optimtab_bits();

 /* SJM 05/05/05 - since using new nchgbtab for per inst nchg bytes */
 /* must always initialize the nchg action bits here */
 /* notice all event control dces added by here */
 __set_nchgaction_bits();

 if (__debug_flg)
  {
   for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
    __dmpmod_nplst(mdp, FALSE); 
  }
 if (__debug_flg && __decompile) __do_decompile();

 if (__debug_flg) __show_allvars();
}

/*
 * ROUTINES TO PREPARE SOURCE FOR DEBUGGER CAN BE DONE ANY TIME
 */

/*
 * remove all duplicated (exactly same name) include files 
 */
static void rem_inc_dupes(void)
{
 register struct incloc_t *ilp, *ilp2, *last_ilp2, *ilp3;

 for (ilp = __inclst_hdr; ilp != NULL; ilp = ilp->inclocnxt) 
  {
   last_ilp2 = ilp;
   for (ilp2 = ilp->inclocnxt; ilp2 != NULL;)
    {
     ilp3 = ilp2->inclocnxt;
     /* notice if same file included with different path ref. just left */
     if (strcmp(__in_fils[ilp->inc_fnind], __in_fils[ilp2->inc_fnind]) == 0)
      {
       /* if same leave first and link out next and keep looking */
       last_ilp2->inclocnxt = ilp2->inclocnxt;
       __my_free((char *) ilp2, sizeof(struct incloc_t));
      }
     else last_ilp2 = ilp2;
     ilp2 = ilp3;
    }
  }
}

/*
 * ROUTINES TO BUILD UDP STATE CHANGE TABLES
 */

/*
 * prepare all udp types - i.e. build the tables
 */
static void prep_udps(void)
{
 register struct udp_t *udpp;
 struct udp_t *udpp2, *last_udpp;
 long sav_mem_use;

 sav_mem_use = __mem_use;
 for (last_udpp = NULL, udpp = __udphead; udpp != NULL;)
  {
   udpp2 = udpp->udpnxt;
   if (udpp->u_used)
    {
     __mem_use = sav_mem_use;
     prep1_udp(udpp);
     __mem_udpuse += __mem_use - sav_mem_use;
     __mem_use = sav_mem_use; 

     /* cannot free lines since dumping source for debug after prep DBG */
     /* free_udp_lines(udpp); */
     last_udpp = udpp;
    }
   else
    {
     free_udp_lines(udpp);
     __my_free((char *) udpp, sizeof(struct udp_t));
     if (last_udpp == NULL) __udphead = udpp2;
     else last_udpp->udpnxt = udpp2;  
     /* do not change last_udpp here */ 
    }
   udpp = udpp2;
  }
}

/*
 * prepare one udp
 */
static void prep1_udp(struct udp_t *udpp)
{
 /* either combinatorial (no state) or level */
 if (udpp->utyp != U_EDGE)
  {
   udpp->utab = (struct udptab_t *) __my_malloc(sizeof(struct udptab_t));
   udpp->utab->eudptabs = NULL;
   prep_comb_udptab(udpp);
   /* array of pointers to tables is of size [number of edge tables-1] */
   udpp->utab->ludptab = __cur_utab;
   /* DBG remove --
   if (__debug_flg) dmp_comb_udptab(udpp);
   --- */
   return;
  }
 /* build the edge table - for now always build even if no level entries */
 prep_edge_udptab(udpp);
 /* -- DBG remove
 if (__debug_flg) dmp_edge_udptab(udpp);
 --- */
}

/* table of combinatorial table output sizes in bits (2 per entry) */
/* constant must be <= 7 since 7 inputs is 4k bytes per table */
static int32 combtabsiz[] = { 0, 8, 32, 128, 512, 2048, 8192, 32768 };

/* table of slower encoding scheme sizes in bits (standard <= 10 ins) */
static int32 comb2tabsiz[] = { 0, 6, 18, 54, 162, 486, 1458, 4374, 13122,
 39366, 118098, 354294, 1062882, 3188646, 9565938, 28697814 };

/*
 * build the combinatorial table
 * also needed for sequential udps where levels override edges
 * use 2 bit per input form for up to 6 inputs
 *
 * notice output can not be wild card since would means 1 input combination
 * to multiple outputs
 */
static void prep_comb_udptab(struct udp_t *udpp)
{
 register struct utline_t *utlp;
 word32 nstates;
 int32 blen, bytsiz;
 word32 ndx;
 char out_ch;

 /* for comb. errors must indicate no edge */
 __cur_ueipnum = NO_VAL;
 __cur_utabsel = NO_VAL;
 /* udp number of states includes output for sequentials */
 nstates = udpp->numstates;
 /* set size of table */
 if (udpp->u_wide) blen = comb2tabsiz[nstates];
 else blen = combtabsiz[nstates];
 /* notice Verilog assumes 8 bit bytes */
 /* SJM 05/19/01 - since fill using word32 ptr need to round to wrd bytes */
 bytsiz = WRDBYTES*wlen_(blen);
 /* RELEASE remove --
 if (__debug_flg)
  __dbg_msg("## comb (part?) udp %s - %u states with table size %d bytes\n",
   udpp->usym->synam, nstates, bytsiz); 
 --- */
 __cur_utab = (word32 *) __my_malloc(bytsiz);
 /* initialize to unfilled (3) - maps table x (2) to real 3 as last step */
 init_utab(__cur_utab, blen);
 __cur_udp = udpp;

 for (utlp = udpp->utlines; utlp != NULL; utlp = utlp->utlnxt)
  {
   /* if this is edge line, goes in edge not combinatorial table */
   if (utlp->uledinum != NO_VAL) continue;

   __sfnam_ind = utlp->utlfnam_ind;
   __slin_cnt = utlp->utlin_cnt;

   /* next output state can be wildcard */
   if (utlp->ulhas_wcard) bld_wcardtab(utlp->tline, nstates, nstates + 1);

   /* build 2 bit form for narrow or signature for wide */
   /* wildcards here always make 0 contribution */  
   ndx = bld_uinitndx(utlp->tline, nstates, -1);

   /* out ch gets the output value (as char here) */
   out_ch = utlp->tline[udpp->numstates];
   /* if output - (no change form) must set output (__cur_uoval) during */
   /* wild card expansion */  
   if (out_ch == '-') __cur_unochange = TRUE;
   else
    {
     /* else can set it immediately */
     __cur_unochange = FALSE;
     /* must be for checking so will match x */
     __cur_uoval = (word32) ((out_ch == '0') ? 0 : ((out_ch == '1') ? 1 : 2));  
    }
   /* this sets 1st to each and recursives call self to expand others */
   if (utlp->ulhas_wcard) xpnd_1wcard(0, ndx);
   /* if no wildcards, even if '-' output, will be 0/1/x (3 since out) */
   /* this sets current output value */
   else setchk_uval(ndx);
  }
 /* finally for all locations left unitialized changed to udp x (2) */
 chg_unfilled_tox(__cur_utab, blen);
}

extern word32 __pow3tab[];

/*
 * recursively expand 1 wild card
 * if wildcard, must be used to set all o states (even 1st)
 * notice recursion depends on fact that passed ndx is by value
 */
static void xpnd_1wcard(int32 wci, word32 ndx)
{
 int32 wcvi, wchval, i;
 word32 ndx2;
 
 wchval = (__wcardtab[wci].wcchar == '?') ? 2 : 1;
 i = __wcardtab[wci].wcinum;
 /* notice need to go through for 0 since table value not yet set */
 for (wcvi = 0; wcvi <= wchval; wcvi++)
  {
   if (!__cur_udp->u_wide)
    {
     /* know i input 2 bits will always be 0 */
     if (wcvi == 0) ndx2 = ndx;
     else
      { 
       /* needed because even though now sticking 2 in as x output */
       /* index must be 3 which will be the value after table prepared */
       /* for non wide table entry */
       if (wcvi == 2) wcvi = 3;
       ndx2 = ndx | (word32) (wcvi << (2*i));
      }
    }
   else
    { 
     /* know here contribution from i, is 0 in ndx */
     if (wcvi != 0) ndx2 = ndx + wcvi*__pow3tab[i];
     else ndx2 = ndx;
     /* know if this is true, will always be rightmost wild card */
     if (i == __cur_udp->numstates - 1) __cur_upstate = (word32) wcvi;
     if (ndx2 >= __pow3tab[__cur_udp->numstates])
      __misc_terr(__FILE__, __LINE__);
    }
   /* - (no change) output handled in level value setting */
   if (wci == __last_wci) setchk_uval(ndx2);
   else xpnd_1wcard(wci + 1, ndx2);
  }
}

/*
 * init a udp table to 3 (not set) - later change 2 (x for now) to x (3)
 * notice udp 2 is unused not z (no z in udp world) 
 * inputs always just 3 for normal rep, 2 add value for signature
 *
 * when done here all 2's changed to 3's (real x's stored in output gstate)
 * radix form uses 3's in vector but changed to 2 in signature update
 * notice any unused in high word32 just set to unfilled
 */
static void init_utab(word32 *taddr, int32 blen)
{
 register int32 i;
 register word32 *wp;
 int32 wlen;

 wlen = wlen_(blen);
 /* initialize to real x's - during table building x's are 2 that are then */
 /* changed to 3's where needed */
 /* since most of table is x's can check and change to x much faster */
 for (wp = taddr, i = 0; i < wlen; i++) wp[i] = ALL1W;
 wp[wlen - 1] &= __masktab[ubits_(blen)];
}

/*
 * convert 1 unfilled (defaults to x) from unfill 2 to x (3)
 */
static void chg_unfilled_tox(word32 *taddr, int32 blen)
{
 register word32 tmp;
 register int32 wi, bi;
 int32 wlen, ubits;

 wlen = wlen_(blen);
 for (wi = 0; wi < wlen - 1; wi++) 
  {
   /* if all bits already literally set to x's continue */
   if ((tmp = taddr[wi]) == ALL1W) continue;

   /* must change 2 (tmp. udp table build x) to 3 - real x */
   for (bi = 0; bi < WBITS; bi += 2)
    {
     if (((tmp >> bi) & 0x3L) == 2L)
      tmp |= (3L << bi);
    }
   taddr[wi] = tmp;
  }
 /* if by accident high word32 full and already all x's - done */
 if ((tmp = taddr[wlen - 1]) == ALL1W) return; 

 /* high word32 needs special handling */
 if ((ubits = ubits_(blen)) == 0) ubits = WBITS;
 for (bi = 0; bi < ubits; bi += 2) 
  {
   if (((tmp >> bi) & 0x3L) == 2L) tmp |= (3L << bi);
  }
 taddr[wlen - 1] = tmp;
}

/*
 * build wild card table from input line
 * for each wild card table contains input no. and wild card char
 *
 * even if edge (\?\?) form (really *) do not include in wild card table
 * for combinatorial table pass one past last input
 */
static void bld_wcardtab(register char *chp, word32 nstates, word32 einum)
{
 register word32 i;
 struct wcard_t *wcp;

 for (__last_wci = -1, i = 0; i < nstates; i++, chp++)
  {
   if (i == einum) continue;

   if (*chp == 'b' || *chp == '?')
    {
     wcp = &(__wcardtab[++__last_wci]);
     wcp->wcinum = i;
     wcp->wcchar = *chp;
    }
  }
}

/*
 * build the initial udp table index - 
 * either 2 bit per input with x as 3 or signature with x as 2 contribution
 */
static word32 bld_uinitndx(char *tlp, word32 nstates, int32 einum)
{
 register word32 i;
 register char *chp;
 word32 ndx;

 /* ? (01x) and b (01) are only that later require loops */
 ndx = 0;
 /* build initial state vector and set rep table start */
 /* wild cards 0 for now */
 /* notice cannot build incrementally, pins in initial signature backward*/ 
 for (i = 0, chp = tlp; i < nstates; i++, chp++)
  {
   /* for edge tab, if wildcard table select do not include, added in xpnd */
   if (i == (int32) einum) continue;
   switch (*chp) {
    /* make edge wildcards 0 here also - r/f just (01) and (10) by here */
    case '0': case 'b': case '?': case '*': case 'n': case 'p':
     if (__cur_udp->u_wide) __cur_upstate = 0L;
     break;
    case '1':
     if (__cur_udp->u_wide) { ndx += __pow3tab[i]; __cur_upstate = 1L; }
     else { ndx &= ~(3L << (2*i)); ndx |= (1L << (2*i)); }
     break;
    /* explicit x is always 2 here for normal udp (later changed to 3) */
    case 'x':
     if (__cur_udp->u_wide) { ndx += 2*__pow3tab[i]; __cur_upstate = 2L; }
     else ndx |= (3L << (2*i));
     break;
    default: __case_terr(__FILE__, __LINE__);
   }
  }
 return(ndx);
}

/*
 * set and check if already set a normal non signature udp table entry
 * this is only place in udp preparation code that udp tables indexed
 * this is passed input state in 2 bit form with x == 2
 * uninitialized is 3 here that statys as real x (3) before exec
 * set to x is 2 here that is changed to 3, real x  
 */
static void setchk_uval(word32 init_ndx)
{
 register word32 tw, tw2;
 word32 ndx;
 int32 wi, bi;
 char s1[RECLEN], s2[RECLEN];

 /* by here init_ndx is correct for this wildcard */ 
 if (__cur_udp->u_wide) ndx = init_ndx;
 else ndx = init_ndx & (int32) __masktab[2*__cur_udp->numstates];

 /* set 2: get old value */
 wi = get_wofs_(2*ndx);
 bi = get_bofs_(2*ndx);
 /* notice for edge cur utab set to right table from edge pair */
 tw = __cur_utab[wi];
 tw2 = (tw >> bi) & 3L;

 /* step 2a: if - output form get output */
 /* if output is '-' use last input that is state */
 /* if not '-' form, know __cur_uoval not set */
 if (__cur_unochange)
  {
   /* notice this can only happen for udp's with state */
   if (__cur_udp->u_wide) __cur_uoval = __cur_upstate;
   else
    {
     __cur_uoval = (init_ndx >> (2*(__cur_udp->numins))) & 0x3L;
     /* must be 2 so if table set (old value == 2) will get warn if 2 */
     if (__cur_uoval == 3L) __cur_uoval = 2L;
    }
  }
 /* step3: if table value already set - check it */
 if (tw2 != 3L)
  {
   if (__cur_uoval == tw2)
    {
     /* do not know previous line - could save but complicated */
    __sgfinform(462, "udp %s selector %s repeated", __cur_udp->usym->synam,
      bld_udploc(s1, ndx, __cur_uoval, __cur_udp->numstates));
     goto done;
    }
  __sgferr(970, "udp %s selector %s conflicts with previous %s",
   __cur_udp->usym->synam, bld_udploc(s1, ndx, __cur_uoval,
   __cur_udp->numstates), bld_udploc(s2, ndx, tw2, __cur_udp->numstates));
   /* fall thru here, even though error, use latest */
  }
 
 /* step4: change output to new value, here x must be represented as 2 */
 /* that gets changed later */
 __cur_utab[wi] = (tw & ~(3L << bi)) | (__cur_uoval << bi);

done:;
 /* --- RELEASE ---
 if (__debug_flg)
  { 
   __dbg_msg("+++ udp set: %s, val=%x, bi=%d, wi=%d\n", 
    bld_udploc(s1, ndx, __cur_uoval, __cur_udp->numstates),
    __cur_utab[wi], bi, wi);
   __dbg_msg("+++ udp set: %s\n", bld_udploc(s1, ndx, __cur_uoval,
    __cur_udp->numstates));
  }
 --- */
}

/*
 * build a udp location entry - for error messages
 * notice for wide signature form index must be wp[1] actual table index
 */
static char *bld_udploc(char *s, word32 ndx, word32 val, word32 nstates)
{
 char s1[RECLEN], s2[RECLEN];

 if (__cur_udp->u_wide) bld3vndx_str(s1, ndx, nstates);
 else bldndx_str(s1, ndx, nstates);
 udploc_to_line(s2, s1);

 sprintf(s, "%s : %s", s2, __to_uvvnam(s1, (word32) val));
 return(s);
}

/*
 * convert a simple edge char array to edge line 
 */
static char *udploc_to_line(char *s, char *line)
{
 register int32 i;
 int32 slen;
 char *chp, *lchp;

 /* first add edge */
 if (__cur_ueipnum != NO_VAL)
  {
   lchp = line;
   chp = s;
   for (i = 0; *lchp != '\0'; i++)
    {
     if (i == __cur_ueipnum)
      {
       *chp++ = '('; 
       *chp++ = (__cur_utabsel == 0) ? '0': (__cur_utabsel == 1) ? '1' : 'x'; 
       *chp++ = *lchp++;
       *chp++ = ')';   
      }
     else *chp++ = *lchp++;     
    }
   *chp = '\0';
  }
 else strcpy(s, line);
 /* then add : if has state */
 if (__cur_udp->numins != __cur_udp->numstates)
  {
   slen = strlen(s);
   s[slen + 1] = '\0';
   s[slen] = s[slen - 1];
   s[slen - 1] = ':';
  }
 return(s);
}

/*
 * dump a non edge udp table only 6 states or less
 */
static void dmp_comb_udptab(struct udp_t *udpp)
{
 word32 *utabp;
 word32 nstates;

 utabp = udpp->utab->ludptab;
 nstates = udpp->numstates;
 if (nstates > 6) return;

 __dbg_msg(".. dumping combinatorial udp %s with %d states\n",
  udpp->usym->synam, nstates);
 if (udpp->u_wide) dmp_udp3v_tab(utabp, nstates);
 else dmp_udp2b_tab(utabp, nstates);
 __dbg_msg("... end dump\n");
}

/*
 * dump a non superposition 2b per element form udp table
 * input is array of words and number of inputs 
 * need more sophisticated version
 */
static void dmp_udp3v_tab(word32 *tabp, word32 nstates) 
{
 register word32 i;
 int32 bi, wi;
 word32 val, ndx;
 char s1[RECLEN], s2[RECLEN];

 ndx = 0;
 for (i = 0, bi = 0; i < __pow3tab[nstates]; i++) 
  { 
   ndx = i;
   wi = get_wofs_(2*ndx);
   bi = get_bofs_(2*ndx); 
   val = (tabp[wi] >> bi) & 0x3L;
   /* RELEASE ---
   __dbg_msg("%s: %s, val=%lx, bi=%d, wi=%d\n", bld3vndx_str(s1, ndx,
    nstates), __to_uvvnam(s2, val), tabp[wi], bi, wi);
   --- */
   __dbg_msg("%s: %s\n", bld3vndx_str(s1, ndx, nstates),
    __to_uvvnam(s2, val));
  }
}

/*
 * build the input string with high value (rightmost on left)
 */
static char *bld3vndx_str(char *s, word32 ndx, word32 nstates)
{
 register word32 i;
 word32 val;
 char s1[10];

 for (i = 0; i < nstates; i++)
  { val = ndx % 3; ndx /= 3; __to_uvvnam(s1, val); s[i] = s1[0]; }
 s[i] = '\0';
 return(s);
}

/*
 * dump a non superposition 2b per element form udp table
 * input is array of words and number of inputs 
 */
static void dmp_udp2b_tab(word32 *tabp, word32 nstates)
{
 register word32 i;
 int32 bi, wi;
 word32 val, ndx;
 char s1[RECLEN], s2[RECLEN];

 ndx = 0;
 for (i = 0; i < (1 << (2*nstates)); i++)
  { 
   ndx = (word32) i;
   /* for narrow case, z's in signature but never used */
   if (udmp_impossible_value(ndx, nstates)) continue;

   wi = get_wofs_(2*ndx);
   bi = get_bofs_(2*ndx); 
   val = (tabp[wi] >> bi) & 0x3L;
   /* --- RELEASE
   if (__debug_flg)
    { 
     __dbg_msg("%s: %s, val=%lx, bi=%d, wi=%d\n", bldndx_str(s1, ndx,
      nstates), __to_uvvnam(s2, val), tabp[wi], bi, wi);
    }
   -- */
   __dbg_msg("%s: %s\n", bldndx_str(s1, ndx, nstates),
    __to_uvvnam(s2, val));
  }
}

/*
 * return T if somewhere there is a 2 bit 10 pattern in word
 * since for narrow know 10 index never used just 00, 01, 11
 */
static int32 udmp_impossible_value(word32 ndx, word32 nstates)
{
 register word32 i;

 for (i = 0; i < nstates; i++)
  { if (((ndx >> (2*i)) & 3L) == 2L) return(TRUE); }
 return(FALSE);
}

/*
 * build the input string with high value (rightmost on left)
 */
static char *bldndx_str(char *s, word32 ndx, word32 nstates)
{
 register word32 i;
 word32 val;
 char vs1[10];

 for (i = 0; i < nstates; i++)
  { val = (ndx >> (2*i)) & 3L; __to_uvvnam(vs1, val); s[i] = vs1[0]; }
 s[i] = '\0';
 return(s);
}

/*
 * dump an combinatorial udp table if not more than 5 inputs
 */
static void dmp_edge_udptab(struct udp_t *udpp)
{
 word32 *utabp;
 int32 nins, i, v;

 nins = udpp->numins;
 if (nins > 5) return;

 __dbg_msg(".. dumping edge udp %s with %d inputs\n", udpp->usym->synam,
  nins);
 __dbg_msg(" - combinatorial table:\n");
 dmp_comb_udptab(udpp);

 __dbg_msg(" - edge tables:\n");
 /* notice, edge udp always sequential and no edge for state input */
 for (i = 0; i < nins; i++)  
  {
   for (v = 0; v < 3; v++)
    {
     __dbg_msg(
      "--> input no. %d changed from %c\n", i, ((v > 1) ? 'x': '0' + v ));
     utabp = udpp->utab->eudptabs[3*i + v];
     if (udpp->u_wide) dmp_udp3v_etab(utabp, udpp->numstates, i, v);
     else dmp_udp2b_etab(utabp, udpp->numstates, i, v);
    }
  }
 __dbg_msg("... end dump\n");
}

/*
 * dump a superposition form edge udp table
 * input is array of words and number of inputs 
 * leave out lines where edge input and edge new value are same
 * know eipnum never state value
 */
static void dmp_udp3v_etab(word32 *tabp, word32 nstates, int32 eipnum,
 int32 e1val) 
{
 register word32 i;
 int32 bi, wi, ndxev;
 word32 val, ndx;
 char s1[RECLEN], s2[RECLEN];

 ndx = 0;
 for (i = 0, bi = 0; i < __pow3tab[nstates]; i++) 
  { 
   ndx = i;
   bld3vndx_str(s1, ndx, nstates),
   ndxev = (s1[eipnum] == 'x') ? 2 : (s1[eipnum] - '0'); 
   if (e1val == ndxev) continue;

   wi = get_wofs_(2*ndx);
   bi = get_bofs_(2*ndx); 
   val = (tabp[wi] >> bi) & 0x3L;
   /* --- RELEASE remove
   __dbg_msg("%s: %s, val=%lx, bi=%d, wi=%d\n", s1, __to_uvvnam(s2, val),
    tabp[wi], bi, wi);
   --- */
   __dbg_msg("%s: %s\n", s1, __to_uvvnam(s2, val));
  }
}

/*
 * dump a non superposition 2b per element form udp table
 * input is array of words and number of inputs 
  * notice for 11 states size will be 1 million lines
 */
static void dmp_udp2b_etab(word32 *tabp, word32 nstates, int32 eipnum,
 int32 e1val) 
{
 register word32 i;
 int32 bi, wi, ndxev;
 word32 val, ndx;
 char s1[RECLEN], s2[RECLEN];

 ndx = 0L;
 for (i = 0; i < (1 << (2*nstates)); i++)
  { 
   ndx = (word32) i;
   /* for narrow case, z's in signature but never used */
   if (udmp_impossible_value(ndx, nstates)) continue;

   bldndx_str(s1, ndx, nstates);
   ndxev = (s1[eipnum] == 'x') ? 2 : (s1[eipnum] - '0'); 
   if (e1val == ndxev) continue;

   wi = get_wofs_(2*ndx);
   bi = get_bofs_(2*ndx); 
   val = (tabp[wi] >> bi) & 0x3L;
   /* RELEASE remove --- 
   __dbg_msg("%s: %s, val=%lx, bi=%d, wi=%d\n", s1, __to_uvvnam(s2, val),
    tabp[wi], bi, wi);
   --- */
   __dbg_msg("%s: %s\n", s1, __to_uvvnam(s2, val));
  }
}

/*
 * allocate the edge index and all the edge tables and set values from lines
 * know level table already filled and edge tables pointer array allocated
 */
static void prep_edge_udptab(struct udp_t *udpp)
{
 register int32 i;
 register struct utline_t *utlp;
 int32 nins, eutabels, blen, bytsiz, tabi, e1val, e2val;
 word32 nstates;
 word32 ustate;
 char ech, out_ch, ech2;

 nstates = udpp->numstates;
 nins = udpp->numins;
 /* these are size for 1 table in bits */
 if (udpp->u_wide) blen = comb2tabsiz[nstates];
 else blen = combtabsiz[nstates];
 /* SJM 05/19/01 - since fill using word32 ptr need to round to wrd bytes */
 bytsiz = WRDBYTES*wlen_(blen);

 /* idea is that old input value selects table, new is index in table */
 /* 1 input 3 table 0,1,x, 2 inputs 6, 3 for 1st and 3 for 2nd, ... */  
 /* state input cannot have edge */
 eutabels = 3*nins;
 /* -- RELEASE remove
 if (__debug_flg)
  __dbg_msg("## edge udp %s - %d inputs %d tables of size %d bytes\n",
   udpp->usym->synam, nins, eutabels, bytsiz); 
 -- */

 /* number of separate tables is 1 per 3*[non state ins (ins-1)] + 1 */
 /* 3 is possible values and extra 1 is for level lines without edges */
 udpp->utab = (struct udptab_t *) __my_malloc(sizeof(struct udptab_t));
 udpp->utab->ludptab = NULL;
 udpp->utab->eudptabs = (word32 **) __my_malloc(eutabels*4);
 /* build the level tab - for now always build even if no level entries */
 prep_comb_udptab(udpp);
 udpp->utab->ludptab = __cur_utab;

 /* initialize all edge tables to */
 for (i = 0; i < eutabels; i++)
  {
   __cur_utab = (word32 *) __my_malloc(bytsiz);
   init_utab(__cur_utab, blen);
   udpp->utab->eudptabs[i] = __cur_utab;
  }

 for (utlp = udpp->utlines; utlp != NULL; utlp = utlp->utlnxt)
  {
   __cur_utlp = utlp;
   /* if no edge, already used to set level table output */
   if (utlp->uledinum == NO_VAL) continue;

   __slin_cnt = utlp->utlin_cnt;
   __sfnam_ind = utlp->utlfnam_ind;

   /* notice 1 possible edge wild card not in wild card table */
   /* but state can be wild card */
   /* never add in edge 2nd value here, added in xpnd or const. case */
   if (utlp->ulhas_wcard) bld_wcardtab(utlp->tline, nstates, utlp->uledinum);

   /* sets any wild cards including edge 1st values will be 00 */
   /* gets converted if wide form */
   ustate = bld_uinitndx(utlp->tline, nstates, (int32) utlp->uledinum);

   /* use the previuos state - if wildcard just */
   if ((out_ch = utlp->tline[nstates]) == '-') __cur_unochange = TRUE;
   else
    { 
     /* if no '-' current output value fixed for all iterations */
     __cur_unochange = FALSE;
     /* must be 2 so if output set will match x */
     __cur_uoval = (out_ch == '0') ? 0 : ((out_ch == '1') ? 1 : 2);
    }
   /* for edge wild cards, both values same so can use first tab index */
   switch ((byte) utlp->utabsel) {
    case '*':
do_star:
     xpnd_edge_wcard(ustate, 0, 1, utlp->uledinum);
     xpnd_edge_wcard(ustate, 0, 2, utlp->uledinum);
     xpnd_edge_wcard(ustate, 1, 0, utlp->uledinum);
     xpnd_edge_wcard(ustate, 1, 2, utlp->uledinum);
     xpnd_edge_wcard(ustate, 2, 0, utlp->uledinum);
     xpnd_edge_wcard(ustate, 2, 1, utlp->uledinum);
     break;
    case 'p':
     /* potential rising edge */
     xpnd_edge_wcard(ustate, 0, 1, utlp->uledinum);
     xpnd_edge_wcard(ustate, 0, 2, utlp->uledinum);
     xpnd_edge_wcard(ustate, 2, 1, utlp->uledinum);
     break;
    case 'n':
     /* potential falling edge */
     xpnd_edge_wcard(ustate, 1, 0, utlp->uledinum);
     xpnd_edge_wcard(ustate, 1, 2, utlp->uledinum);
     xpnd_edge_wcard(ustate, 2, 0, utlp->uledinum);
     break;
    case '?':
     ech2 = utlp->tline[utlp->uledinum];
     /* notice (..) form can be value or b or ? but not edge abbreviation */
     /* also notice for edge only wildcard is possible '-' */
     switch (ech2) {
      case '?':
       /* (\?\?) is same as * */
       goto do_star;
      case 'b':
       /* (?b) */
       xpnd_edge_wcard(ustate, 1, 0, utlp->uledinum);
       xpnd_edge_wcard(ustate, 2, 0, utlp->uledinum);
       xpnd_edge_wcard(ustate, 0, 1, utlp->uledinum);
       xpnd_edge_wcard(ustate, 2, 1, utlp->uledinum);
       break;
      case '0':
       /* (?0) */
       xpnd_edge_wcard(ustate, 1, 0, utlp->uledinum);
       xpnd_edge_wcard(ustate, 2, 0, utlp->uledinum);
       break;
      case '1':
       /* (?1) */
       xpnd_edge_wcard(ustate, 0, 1, utlp->uledinum);
       xpnd_edge_wcard(ustate, 2, 1, utlp->uledinum);
       break;
      case 'x':  
       /* (?x) */
       xpnd_edge_wcard(ustate, 0, 2, utlp->uledinum);
       xpnd_edge_wcard(ustate, 1, 2, utlp->uledinum);
       break;
      default: __case_terr(__FILE__, __LINE__);
     }
     break;
    case 'b': 
     ech2 = utlp->tline[utlp->uledinum];
     switch (ech2) {
      case '?':
       /* (b?) */
       xpnd_edge_wcard(ustate, 0, 1, utlp->uledinum);
       xpnd_edge_wcard(ustate, 0, 2, utlp->uledinum);
       xpnd_edge_wcard(ustate, 1, 0, utlp->uledinum);
       xpnd_edge_wcard(ustate, 1, 2, utlp->uledinum);
       break;
      case 'b':
       /* (bb) */
       xpnd_edge_wcard(ustate, 0, 1, utlp->uledinum);
       xpnd_edge_wcard(ustate, 1, 0, utlp->uledinum);
       break;
      case '0':
       /* (b0) */
       xpnd_edge_wcard(ustate, 1, 0, utlp->uledinum);
       break;
      case '1':
       /* (b1) */
       xpnd_edge_wcard(ustate, 0, 0, utlp->uledinum);
       break;
      case 'x':
       /* (bx) */
       xpnd_edge_wcard(ustate, 0, 2, utlp->uledinum);
       xpnd_edge_wcard(ustate, 1, 2, utlp->uledinum);
       break;
      default: __case_terr(__FILE__, __LINE__);
     }
     break;
    case '0': case '1': case 'x':
     /* notice ech is case index for output tab too */
     ech = (char) utlp->utabsel;
     /* may have ([01x][?b]) style wild card form  */
     ech2 = utlp->tline[utlp->uledinum];
     if (ech2 == '?')
      {
       switch (ech) {
        case '0':
         xpnd_edge_wcard(ustate, 0, 1, utlp->uledinum);
         xpnd_edge_wcard(ustate, 0, 2, utlp->uledinum);
         break;
        case '1':
         xpnd_edge_wcard(ustate, 1, 0, utlp->uledinum);
         xpnd_edge_wcard(ustate, 1, 2, utlp->uledinum);
         break;
        case 'x':
         xpnd_edge_wcard(ustate, 2, 0, utlp->uledinum);
         xpnd_edge_wcard(ustate, 2, 1, utlp->uledinum);
         break;
        default: __case_terr(__FILE__, __LINE__); 	   
       }
       break;
      }
     if (ech2 == 'b')
      {
       switch (ech) {
        case '0': xpnd_edge_wcard(ustate, 0, 1, utlp->uledinum); break;
        case '1': xpnd_edge_wcard(ustate, 1, 0, utlp->uledinum); break;
        case 'x':
         xpnd_edge_wcard(ustate, 2, 0, utlp->uledinum);
         xpnd_edge_wcard(ustate, 2, 1, utlp->uledinum);
         break;
        default: __case_terr(__FILE__, __LINE__);
       }
       break;
      }

     e1val = (ech == '0') ? 0 : ((ech == '1') ? 1 : 2);
     tabi = 3*utlp->uledinum + e1val;

     /* non edge wild card case - must add in 2nd edge part of state */
     /* know contribution before here 0 */
     e2val = (ech2 == '0') ? 0 : ((ech2 == '1') ? 1 : 2);
     if (udpp->u_wide) ustate += (e2val*__pow3tab[utlp->uledinum]); 
     else ustate |= (e2val << (2*utlp->uledinum));

     /* RELEASE remove --- 
     if (__debug_flg)
      __dbg_msg(
       "=== non wildcard input edge (%c%c) with from value %d - table %d\n",
       utlp->uledinum, ech, ech2, tabi);
     --- */
     /* once current table corresponding to edge is set, just like level */
     __cur_utab = udpp->utab->eudptabs[tabi];
     if (!utlp->ulhas_wcard) setchk_uval(ustate);
     else xpnd_1wcard(0, ustate);
     break;
    default: __case_terr(__FILE__, __LINE__);
   }
  }
 /* final step is conversion of x (set x) to 3 real x */ 
 for (i = 0; i < eutabels; i++)
  {
   __cur_utab = udpp->utab->eudptabs[i];
   chg_unfilled_tox(__cur_utab, blen);
  }
}

/*
 * process edge wildcard entry - passed fixed edge generated from iteration
 * e1 val selects table and e2 val is table value (changed to)  
 */
static void xpnd_edge_wcard(word32 ustate, int32 e1val, int32 e2val,
 word32 einpnum)
{
 int32 tabi;

 /* first select the table */
 tabi = 3*einpnum + e1val;
 /* --- RELEASE remove 
 if (__debug_flg)
  __dbg_msg("=== expand wildcard input edge %d from value %d - table %d\n",
   einpnum, e1val, tabi);
 --- */
 /* once current table corresponding to edge is set, just like level */
 __cur_utab = __cur_udp->utab->eudptabs[tabi];

 /* must set ustate input no. einpnum to ech2 value - know 0 on input */
 /* know this can never be last previous state input or will not get here */
 /* ustate is 2 bit per element form */
 /* ustate is table index so for narrow form must be 3 */
 if (__cur_udp->u_wide) ustate += e2val*__pow3tab[einpnum];
 else
  {
   /* ustate is index which for narrow must be 3 - value is 2 */
   if (e2val == 2) e2val = 3;
   ustate |= (((word32) e2val) << (2*einpnum));
  }

 /* need some global to be set for errors */
 __cur_ueipnum = einpnum;
 __cur_utabsel = e1val;

 /* ulhas_wcard is only level wildcard */
 if (!__cur_utlp->ulhas_wcard) setchk_uval(ustate);
 else xpnd_1wcard(0, ustate);
}

/*
 * free udp lines
 * only called for uninstantiated udps since size small and needed for PLI
 */
static void free_udp_lines(struct udp_t *udpp)
{
 register struct utline_t *utlp;
 struct utline_t *utlp2;

 for (utlp = udpp->utlines; utlp != NULL;)
  {
   utlp2 = utlp->utlnxt;
   /* notice this is not 0 terminated string */
   __my_free(utlp->tline, (int32) utlp->ullen);
   __my_free((char *) utlp, sizeof(struct utline_t));
   utlp = utlp2;
  }
 udpp->utlines = NULL;
}

/*
 * ROUTINES TO CHANGE ALL WIRE RANGE REPS 
 */
/*
 * change representation and allocate sim net struct for every net range
 * must do unused var. checking before change range rep since uses ncomp
 * range form bits
 *
 * BEWARE - for normal nets allocating nu.ct to large chunk area because
 * freeing too slow - but here for params (also specparams) since cannot
 * distinguish must copy since now can be annotated too
 *
 * AIV 01/19/07 - need to do local params here as well for mod and tasks
 * 09/27/06 - local params not used in delay annotation so not copied here
 */
static void change_all_rngreps(void)
{
 register int32 ni;
 register struct net_t *np;
 register struct ncomp_t *oncomp, *nncomp;
 struct mod_t *mdp;
 int32 pi, sav_declobj;
 struct task_t *tskp;

 sav_declobj = __cur_declobj;
 __cur_declobj = MODULE;
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);
   __cur_declobj = MODULE;

   if (__inst_mod->mnnum != 0) 
    {
     for (ni = 0, np = &(__inst_mod->mnets[0]); ni < __inst_mod->mnnum;
      ni++, np++)
      {
//SJM FIXME - why both { {
        {
         /* expect inout ports to be set but not used or used but not set */
         if (np->iotyp != IO_BID)
          emit_varunused_informs(np, (struct task_t *) NULL);
        }
       set_optim_nflds(np);
       rt_change_rngrep(np); 
      }
    } 
   /* copy ncomp that are still needed for delay annotation */
   /* needed because ncomps allocated and freed in blocks */
   for (pi = 0; pi < __inst_mod->mprmnum; pi++)  
    {
     np = &(__inst_mod->mprms[pi]);
     oncomp = np->nu.ct;
     nncomp = (struct ncomp_t *) __my_malloc(sizeof(struct ncomp_t));
     /* expressions not freed and copied will point to right ones */ 
     memcpy(nncomp, oncomp, sizeof(struct ncomp_t));
     np->nu.ct = nncomp;
    }

   /* AIV 01/19/07 - need to do local params here as well */
   for (pi = 0; pi < __inst_mod->mlocprmnum; pi++)  
    {
     np = &(__inst_mod->mlocprms[pi]);
     oncomp = np->nu.ct;
     nncomp = (struct ncomp_t *) __my_malloc(sizeof(struct ncomp_t));
     /* expressions not freed and copied will point to right ones */ 
     memcpy(nncomp, oncomp, sizeof(struct ncomp_t));
     np->nu.ct = nncomp;
    }

   /* AIV 09/27/06 - do not need copy for local params since */
   /* can't be set with SDF label annotate */
   __cur_declobj = TASK; 
   for (tskp = __inst_mod->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     if (tskp->trnum == 0) continue;

     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       emit_varunused_informs(np, tskp);

       set_optim_nflds(np);
       rt_change_rngrep(np); 
      }
     /* copy ncomp that are still needed for delay annotation */
     /* needed because ncomps allocated and freed in blocks */
     for (pi = 0; pi < tskp->tprmnum; pi++)  
      {
       np = &(tskp->tsk_prms[pi]);
       oncomp = np->nu.ct;
       nncomp = (struct ncomp_t *) __my_malloc(sizeof(struct ncomp_t));
       /* expressions not freed and copied will point to right ones */ 
       memcpy(nncomp, oncomp, sizeof(struct ncomp_t ));
       np->nu.ct = nncomp;
      }
     /* AIV 01/19/07 - need to do local params here as well */
     for (pi = 0; pi < tskp->tlocprmnum; pi++)  
      {
       np = &(tskp->tsk_locprms[pi]);
       oncomp = np->nu.ct;
       nncomp = (struct ncomp_t *) __my_malloc(sizeof(struct ncomp_t));
       /* expressions not freed and copied will point to right ones */ 
       memcpy(nncomp, oncomp, sizeof(struct ncomp_t ));
       np->nu.ct = nncomp;
      }
    }
   if (__inst_mod->mspfy != NULL)
    {
     __cur_declobj = SPECIFY;
     for (pi = 0; pi < __inst_mod->mspfy->sprmnum; pi++)  
      {
       np = &(__inst_mod->mspfy->msprms[pi]);
       oncomp = np->nu.ct;
       nncomp = (struct ncomp_t *) __my_malloc(sizeof(struct ncomp_t));
       /* expressions not freed and copied will point to right ones */ 
       memcpy(nncomp, oncomp, sizeof(struct ncomp_t ));
       np->nu.ct = nncomp;
      }
    }
   __pop_wrkitstk();
  }

 __cur_declobj = sav_declobj;
 free_ncablks();
}


/*
 * copy fields form ncomp into run time net fields
 *
 * LOOAKTME - think no longer need 2 step setting of optim fields
 */
static void set_optim_nflds(struct net_t *np)
{
 np->frc_assgn_allocated = FALSE;
 np->dmpv_in_src = FALSE;
 np->monit_in_src = FALSE;
 np->n_onrhs = FALSE;
 np->n_onlhs = FALSE;
 
 if (np->nu.ct->frc_assgn_in_src) np->frc_assgn_allocated = TRUE;
 if (np->nu.ct->dmpv_in_src) np->dmpv_in_src = TRUE;
 if (np->nu.ct->monit_in_src) np->monit_in_src = TRUE;
 if (np->nu.ct->n_onrhs) np->n_onrhs = TRUE;
 if (np->nu.ct->n_onlhs) np->n_onlhs = TRUE;
}


/*
 * routine to free all allocated ncmp blks when no longed used at all
 *
 * this frees all ncomps but param comp previously copied because needed
 * for delay annotation
 */
static void free_ncablks(void)
{
 register struct ncablk_t *ncabp, *ncabp2;

 /* free all ncomp ablks since ncomp form now gone */
 for (ncabp = __hdr_ncablks; ncabp != NULL;)
  {
   ncabp2 = ncabp->ncablknxt;
   __my_free((char *) ncabp->ancmps, BIG_ALLOC_SIZE);
   __my_free((char *) ncabp, sizeof(struct ncablk_t));
   ncabp = ncabp2;
  }
}

/*
 * emit any unused inform
 * notice this is called just before change to non comp representation
 */
static void emit_varunused_informs(struct net_t *np, struct task_t *tskp)
{
 struct ncomp_t *ncmp;
 int32 infnum;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[RECLEN];

 ncmp = np->nu.ct;
 if (ncmp->n_onrhs && ncmp->n_onlhs) return;

 if (tskp != NULL)
  {
   switch ((byte) tskp->tsktyp) {
    case Begin: strcpy(s2, "in begin block"); break;
    case FORK: strcpy(s2, "in fork block"); break;
    case FUNCTION: strcpy(s2, "in function"); break;
    case TASK: strcpy(s2, "in task"); break;
    default: __case_terr(__FILE__, __LINE__);   
   }
   sprintf(s1, "%s %s in %s", s2, tskp->tsksyp->synam,
    __inst_mod->msym->synam);
  }
 else sprintf(s1, "in module %s", __inst_mod->msym->synam);
 
 if (np->n_isarr) strcpy(s2, "array"); else __to_wtnam(s2, np);

 if (np->ntyp == N_EVENT)
  {
   if (!ncmp->n_onrhs && !ncmp->n_onlhs)
    { strcpy(s3, "unused"); infnum = 436; }
   else if (!ncmp->n_onrhs && ncmp->n_onlhs)
    { strcpy(s3, "caused but used in no event control"); infnum = 437; }
   else { strcpy(s3, "used in event control but not caused"); infnum = 438; }
  }
 else
  {
   if (!ncmp->n_onrhs && !ncmp->n_onlhs) { strcpy(s3, "unused");
    infnum = 436; }
   else if (!ncmp->n_onrhs && ncmp->n_onlhs)
    { strcpy(s3, "set but not accessed"); infnum = 437; }
   else { strcpy(s3, "accessed but not set"); infnum = 438; }
  } 

 /* if completely unreferenced normal inform */
 if (np->iotyp == NON_IO || infnum == 436)
  {
   __gfinform(infnum, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
    "%s: %s %s %s", s1, s2, np->nsym->synam, s3);
  } 
 else
  {
   if (infnum == 437) infnum = 467;
   else if (infnum == 438) infnum = 468;
   else __case_terr(__FILE__, __LINE__);

   __gfinform(infnum, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
    "%s: %s %s %s %s", s1, __to_ptnam(s4, np->iotyp), s2, np->nsym->synam,
    s3);
  }
}

/*
 * change auxiliary nx from compile to run time (numbers) form
 * only for wires (and regs)
 *
 * notice this is place nwid set
 * also after here ncomps gone
 */
static void rt_change_rngrep(struct net_t *np)
{
 register int32 bi;
 int32 nni1, nni2, nai1, nai2, bits;
 struct ncomp_t *ncmp;
 struct rngarr_t *rap;
 struct rngdwir_t *rdwp;
 struct gate_t gwrk;
 struct rngwir_t *rwp;

 /* in XL parameters can be regs at run time */
 /* not legal in standard so must catch before here */
 /* DBG remove --- */
 if (np->n_isaparam || np->nrngrep != NX_CT)
  __arg_terr(__FILE__, __LINE__);
 /* -- */

 ncmp = np->nu.ct;


 /* know this is number since range */
 if (np->n_isavec)
  {
   nni1 = (int32) __contab[ncmp->nx1->ru.xvi];
   nni2 = (int32) __contab[ncmp->nx2->ru.xvi];

   if (nni1 == -1 || nni2 == -2) __arg_terr(__FILE__, __LINE__);
   np->nwid = ((nni1 >= nni2) ? (nni1 - nni2 + 1) : (nni2 - nni1 + 1));
   /* need to handle options that set default splitting state */
   switch (ncmp->n_spltstate) {
    case SPLT_DFLT:
     np->vec_scalared = (__no_expand) ? FALSE : TRUE;
     break;
    case SPLT_SCAL: np->vec_scalared = TRUE; break;
    case SPLT_VECT: np->vec_scalared = FALSE; break;
    default: __case_terr(__FILE__, __LINE__);
   }
  }
 /* scalar wire has vec scalared off since 1 bit treated as entity */
 else { np->vec_scalared = FALSE; np->nwid = 1; nni1 = nni2 = 0; }

 /* registers and arrays always vectored (non split into bits) */  
 if (np->ntyp >= NONWIRE_ST) np->vec_scalared = FALSE;
 else
  {
   if (np->n_stren && np->n_isavec && !np->vec_scalared)
    {
     __gferr(982, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
      "non scalar strength wire %s cannot be vectored", np->nsym->synam);
    }
  }

 if (np->n_isarr)
  {
   nai1 = (int32) __contab[ncmp->ax1->ru.xvi];
   nai2 = (int32) __contab[ncmp->ax2->ru.xvi];

   rap = (struct rngarr_t *) __my_malloc(sizeof(struct rngarr_t));
   rap->ni1 = nni1;
   rap->ni2 = nni2;
   rap->ai1 = nai1;
   rap->ai2 = nai2;
   np->nu.rngarr = rap;
   np->nrngrep = NX_ARR;
  }
 else if (ncmp->n_dels_u.pdels != NULL || np->n_isapthdst)
  {
   /* path dest. can not have delays */
   /* if has delay and path dest., turn off dst. to cause later error */
   if (ncmp->n_dels_u.pdels != NULL)
    { if (np->n_isapthdst) np->n_isapthdst = FALSE; }

   rdwp = (struct rngdwir_t *) __my_malloc(sizeof(struct rngdwir_t));
   rdwp->ni1 = nni1;
   rdwp->ni2 = nni2;

   if (!np->n_isapthdst)
    {
     /* add any defparams (or spec but impossible) to param np list */
     add_netdel_pnp(np, np->nu.ct->n_dels_u.pdels);
     /* preprocess wire delays into indexable array or value */
     __prep_delay(&gwrk, np->nu.ct->n_dels_u.pdels, FALSE,
      (np->ntyp == N_TRIREG), "path delay", TRUE, np->nsym, FALSE); 

     if (__nd_neg_del_warn)
      {
       __gferr(971, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
        "wire %s delay negative (0 used)", np->nsym->synam);
       __nd_neg_del_warn = FALSE;
      } 

     rdwp->n_delrep = gwrk.g_delrep;
     /* is union assign portable */
     rdwp->n_du = gwrk.g_du;
    }
   /* this is allocated and filled later */
   else { rdwp->n_delrep = DT_PTHDST; rdwp->n_du.pb_pthdst = NULL; }

   /* must allocate the per bit pending scheduled value */
   bits = __inst_mod->flatinum*np->nwid;
   rdwp->wschd_pbtevs = (i_tev_ndx *) __my_malloc(bits*sizeof(i_tev_ndx));
   for (bi = 0; bi < bits; bi++) rdwp->wschd_pbtevs[bi] = -1;
   np->nu.rngdwir = rdwp;
   np->nrngrep = NX_DWIR;
  }
 else
  {
   /* finally any other wire */
   if (np->n_isavec) 
    {
     rwp = (struct rngwir_t *) __my_malloc(sizeof(struct rngwir_t));
     rwp->ni1 = nni1;
     rwp->ni2 = nni2;
     np->nu.rngwir = rwp;
     np->nrngrep = NX_WIR;
    }
   else { np->nu.rngwir = NULL; np->nrngrep = NX_SCALWIR; }
  }
 /* just free the expressions since if arg. nil does nothing */
 __free_xtree(ncmp->nx1);
 __free_xtree(ncmp->nx2);
 __free_xtree(ncmp->ax1);
 __free_xtree(ncmp->ax2);
} 

/*
 * ROUTINES TO ALLOCATE AND INITIALIZE GATE INTERNAL STATE
 */

/*
 * 
 * build the schedule array and gate state value for each gate
 * also prepares delays
 */
static void bld_gstate(void)
{
 register int32 gi;
 struct mod_t *mdp;
 struct gate_t *gp;

 /* for each module in design */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   for (gi = 0; gi < mdp->mgnum; gi++)
    {
     gp = &(mdp->mgates[gi]);

     /* pull gate (really source) has no state or scheduled events */
     /* no state for trans */ 
     if (gp->g_class == GC_PULL || gp->g_class == GC_TRAN) continue;

     /* if output unc. (OPEMPTY), changes are not seen (do not propagate) */
     if (gp->g_class != GC_TRANIF && gp->gpins[0]->optyp == OPEMPTY)
      continue;

     if (gp->g_du.pdels == NULL)
      { gp->g_du.d1v = NULL; gp->g_delrep = DT_NONE; }
     else
      {
       add_gatedel_pnp(gp, gp->g_du.pdels);
       __prep_delay(gp, gp->g_du.pdels, FALSE, FALSE, "primitive delay",
        FALSE, gp->gsym, FALSE);
       if (__nd_neg_del_warn)
        {
         __gferr(972, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
          "gate %s delay negative (0 used)", gp->gsym->synam);
         __nd_neg_del_warn = FALSE;
        } 
      }
     /* remove #0 if special option on - annotation may put back */ 
     if (__rm_gate_pnd0s)
      {
       if (__chk_0del(gp->g_delrep, gp->g_du, mdp) == DBAD_0) 
        {
         __num_rem_gate_pnd0s++; 
         __num_flat_rem_gate_pnd0s += mdp->flatinum;
         __free_del(gp->g_du, gp->g_delrep, mdp->flatinum);
         gp->g_du.d1v = NULL;
         gp->g_delrep = DT_NONE;
        }
      }
     /* only gates have gstate, eval. for conta (can be func. on rhs) */
     alloc_gstate(gp, mdp->flatinum);
    }
   __pop_wrkitstk();
  }
}

/*
 * allocate per instance gstate or usate and intialize to x
 */
static void alloc_gstate(struct gate_t *gp, int32 insts)
{
 register int32 gsi;
 int32 nbytes;

 /* first allocate and NULL per instantiating module schedule event ptrs */
 /* only allocate inertial schedule array if has delay (includes #0) */
 if (gp->g_delrep != DT_NONE)
  {
   nbytes = insts*sizeof(i_tev_ndx); 
   gp->schd_tevs = (i_tev_ndx *) __my_malloc(nbytes);
   for (gsi = 0; gsi < insts; gsi++) gp->schd_tevs[gsi] = -1;
  }
 
 /* no state for continuous assign */
 if (gp->g_class != GC_UDP) __set_init_gstate(gp, insts, TRUE);
 else __set_init_udpstate(gp, insts, TRUE);
}

/* SJM 12/16/99 - notices packing only into words does not change this */
/* mask table for <= 16 bit wide pck elements (1,z,x) - 0 just all 0's */
/* 0 width impossible and never used for scalar */ 
/* SJM 07/15/00- only for gates since variables no longer packed */
word32 __pack_imaskx[17] = {
 0L, 0x3L, 0xfL, 0x3fL, 0xffL, 0x03ff, 0x0fffL, 0x3fffL, 0xffffL,
 0x3ffffL, 0xfffffL, 0x3fffffL, 0xffffffL, 0x3ffffffL, 0xfffffffL,
 0x3fffffffL, ALL1W
};

/* what is this value of 16 bit should 1 not 8 1's ? */
word32 __pack_imask1[17] = {
 0L, 0x1L, 0x3L, 0x07L, 0x0fL, 0x001fL, 0x003fL, 0x007fL, 0x00ffL,
 0x01ffL, 0x03ffL, 0x07ffL, 0x0fffL, 0x1fffL, 0x03fffL, 0x7fffL, 0xffffL
};

word32 __pack_imaskz[17] = {
 0L, 0x2L, 0xcL, 0x38L, 0xf0L, 0x03e0L, 0x0fc0L ,0x3f80L, 0xff00L,
 0x3fe00L, 0xffc00L, 0x3ff800L, 0xfff000L, 0x3ffe000L, 0xfffc000L,
 0x3fff8000L, 0xffff0000L
};

/*
 * set initial state for builtin gate
 * 
 * LOOKATME - SJM 12/19/99 maybe should also only pck into word32 here but
 * wastes lotsof storage for 4 state gates
 */
extern void __set_init_gstate(struct gate_t *gp, int32 insts, int32 nd_alloc)
{
 register int32 i;
 register word32 *wp;
 register hword *hwp;
 register byte *bp;
 int32 pnum, wlen;
 word32 maska;

 switch ((byte) gp->g_class) {
  case GC_LOGIC:
   pnum = gp->gpnum;
   if (pnum > 16)
    {
     wlen = wlen_(pnum);
     if (nd_alloc)
      gp->gstate.wp = (word32 *) __my_malloc(2*WRDBYTES*insts*wlen);
     __init_vec_var(gp->gstate.wp, insts, wlen, pnum, ALL1W, ALL1W);
     break;
    }
   /* case 1, fits in 8 bits */
   maska = __pack_imaskx[pnum];
   if (pnum <= 4) 
    {
     if (nd_alloc) gp->gstate.bp = (byte *) __my_malloc(insts);
     bp = gp->gstate.bp;
     for (i = 0; i < insts; i++) bp[i] = (byte) maska;
    }
   else if (pnum <= 8)
    {     
     if (nd_alloc) gp->gstate.hwp = (hword *) __my_malloc(2*insts);
     hwp = gp->gstate.hwp;
     for (i = 0; i < insts; i++) hwp[i] = (hword) maska;
    }
   else 
    {
     if (nd_alloc) gp->gstate.wp = (word32 *) __my_malloc(WRDBYTES*insts);
     wp = gp->gstate.wp;
     for (i = 0; i < insts; i++) wp[i] = maska;
    }
   break;
  case GC_BUFIF:
   if (nd_alloc) gp->gstate.hwp = (hword *) __my_malloc(2*insts); 
   /* this starts by driving given strength x (if no expl. <st1:st0>= x) */
   hwp = gp->gstate.hwp;
   for (i = 0; i < insts; i++) hwp[i] = (hword) (0x3f | (gp->g_stval << 6));
   break;
  case GC_MOS:
   if (nd_alloc) gp->gstate.wp = (word32 *) __my_malloc(WRDBYTES*insts);
   wp = gp->gstate.wp;
   /* pattern here in highz in, control x, and high z out */
   for (i = 0; i < insts; i++) wp[i] = 0x00020302L;
   break;
  case GC_CMOS:
   if (nd_alloc) gp->gstate.wp = (word32 *) __my_malloc(WRDBYTES*insts);
   /* pattern here in highz in, controls both x, and high z out */
   wp = gp->gstate.wp;
   for (i = 0; i < insts; i++) wp[i] = 0x02030302L;
   return;
  case GC_TRANIF:
   /* tranif states are 2 bit - 1 conducting, 0 no conducting, 3 unknown */
   /* notice for delay case, schedule in tev */
   wlen = wlen_(2*insts);
   if (nd_alloc) gp->gstate.wp = (word32 *) __my_malloc(WRDBYTES*wlen);
   wp = gp->gstate.wp;
   /* start with tranif gates in unknown conducting state */
   for (i = 0; i < wlen; i++) wp[i] = 0xffffffffL;
   wp[wlen - 1] &= __masktab[ubits_(2*insts)];
   return;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * set initial state for udp
 * for udps >6 also need 1 word32 per inst. for current table index value
 *
 * notice value is stored as x (3) - but indexing uses 2 for nins > 6
 * conversions from normal 4 value to 3 value made when indexing
 *
 * ustate values are stored as n 2 bit scalars not separate a and b parts
 * since no need for reduction word32 evaluation operations
 */
extern void __set_init_udpstate(struct gate_t *gp, int32 insts,
 int32 nd_alloc)
{
 register int32 gsi;
 register word32 *wp;
 register hword *hwp;
 int32 nins;
 word32 iwval, indval;
 struct udp_t *udpp;

 udpp = gp->gmsym->el.eudpp;
 /* notice nins is total number of input pins: (state not included) */
 nins = udpp->numins;
 /* assume initial value x - notice inputs plus 1 output no pending  */
 iwval = __masktab[2*nins + 2];
 /* initialize to x here - if initial value assign instead of normal eval */ 
 if (udpp->u_wide)
  {
   /* 2 words for each superposition form udp */
   if (nd_alloc) gp->gstate.wp = (word32 *) __my_malloc(2*WRDBYTES*insts);
   wp = gp->gstate.wp;
   for (gsi = 0; gsi < insts; gsi++)
    {
     wp[2*gsi] = iwval;
     indval = cmp_udpind(iwval, udpp->numstates);
     wp[2*gsi + 1] = indval;
     /* --- RELEASE remove 
     if (__debug_flg)
      __dbg_msg("++ wide udp init (inst %d): w0=%lx, w1=%lx\n",
       gsi, wp[2*gsi], wp[2*gsi + 1]);
     --- */
    }
  }
 else
  {
   /* also store signature form udp in half word32 - 2 bit udp pointless */
   if (nd_alloc) gp->gstate.hwp = (hword *) __my_malloc(2*insts);
   hwp = gp->gstate.hwp;
   /* -- RELEASE remove
   if (__debug_flg)
    __dbg_msg("-- narrow udp init: initial state h=%x\n", (hword) iwval); 
   -- */
   for (gsi = 0; gsi < insts; gsi++) hwp[gsi] = (hword) iwval;
  }
}

/*
 * eval input state (including possible previous state) to get table index
 * for initialization only since normally must use transition to select tab
 *
 * assuming here that 30 bit and 2 bit shifts take same time
 */
static word32 cmp_udpind(word32 ustate, word32 nstates)
{
 register word32 ui;

 word32 ind;
 word32 ival;

 /* know ustate has proper initialized state */
 /* notice here need to use state that may have been initialized */ 
 for (ind = 0, ui = 0; ui < nstates; ui++)
  {
   ival = (ustate >> ui) & 3L; 
   /* input x converted to udp x (2) to index - z is x and left as is */
   if (ival == 3) ival = 2;
   ind += ival*__pow3tab[ui];
  } 
 return(ind);
}
/*
 * prep conta delays
 *
 * SJM 09/28/02 - even for rhs concat PB separated, delay in master
 */
static void prep_conta_dels(void)
{
 register struct mod_t *mdp;
 register struct conta_t *cap;
 int32 cai;
 struct gate_t gwrk;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   for (cap = &(mdp->mcas[0]), cai = 0; cai < mdp->mcanum; cai++, cap++)
    {
     /* 1 bit lhs conta delays like gate delays - 3rd arg determines */
     if (cap->ca_du.pdels == NULL)
      { cap->ca_du.d1v = NULL; cap->ca_delrep = DT_NONE; }
     else
      {
       add_contadel_pnp(cap, cap->ca_du.pdels);
       __prep_delay(&gwrk, cap->ca_du.pdels, FALSE, FALSE,
        "continuous assignment delay", FALSE, cap->casym, FALSE);
       if (__nd_neg_del_warn)
        {
         __gferr(973, cap->casym->syfnam_ind, cap->casym->sylin_cnt,
          "continuous assign delay negative (0 used)");
         __nd_neg_del_warn = FALSE;
        } 
       cap->ca_du = gwrk.g_du;
       cap->ca_delrep = gwrk.g_delrep;
       /* SJM 09/28/02 - now set ca 4v del during fixup */ 
      }
    }
   __pop_wrkitstk();
  }
}

/*
 * ROUTINES TO BUILD PER BIT INPUT PORT ICONN PINS, MOD PORTS, and CONCATS
 */

/*
 * bld the separated per bit iconn rhs net pins for one iconn
 *
 * notice building one PB for every down mod port bit - if highconn inst
 * conn wider nothing to change if and mod port wider unc. no value
 * since pb not used for initialization
 */
static void bld_pbsep_input_mpps(void)
{
 register int32 pi, bi;
 register struct mod_t *mdp;
 int32 ii, pi2, numpins, ptyp;
 struct mod_t *imdp;
 struct mod_pin_t *mpp;
 struct inst_t *ip;
 struct expr_t *xp, *xp2;
 struct pbexpr_t *pbexpr;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   /* first process all instances in module */
   for (ii = 0; ii < mdp->minum; ii++)
    {
     ip = &(mdp->minsts[ii]);
     imdp = ip->imsym->el.emdp;
     if ((numpins = imdp->mpnum) == 0) continue;

     for (pi = 0; pi < numpins; pi++)
      {
       mpp = &(imdp->mpins[pi]);
       xp = ip->ipins[pi];
       ptyp = mpp->mptyp;
       if (ptyp != IO_IN) continue; 
       /* notice decl lhs always separable but no separate unless hconn cat */
       if (!rhs_cat_separable(xp)) continue;

       if (ip->pb_ipins_tab == NULL)
        {
         ip->pb_ipins_tab = (struct expr_t ***)
          __my_malloc(numpins*sizeof(struct expr_t **));
         for (pi2 = 0; pi2 < numpins; pi2++) ip->pb_ipins_tab[pi2] = NULL;
        }
       /* all bits filled below */
       ip->pb_ipins_tab[pi] = (struct expr_t **)
        __my_malloc(mpp->mpwide*sizeof(struct expr_t *));
       
       pbexpr = bld_pb_expr_map(xp, mpp->mpwide);
       for (bi = 0; bi < mpp->mpwide; bi++)
        {
         /* for unc. (lhs wider) builds the constant rhs 0/z */
         xp2 = bld_1sep_pbit_expr(&(pbexpr[bi]), xp->x_stren);
         ip->pb_ipins_tab[pi][bi] = xp2;
        }
       __my_free((char *) pbexpr, numpins*sizeof(struct pbexpr_t));

       /* T means at least one highconn iconn is per bit concat */
       mpp->has_scalar_mpps = TRUE;
      }
    }
   __pop_wrkitstk();
  }

 /* always build separated per bit mpps if any hconn separable concat */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   numpins = mdp->mpnum;
   for (pi = 0; pi < numpins; pi++)
    {
     mpp = &(mdp->mpins[pi]);
     ptyp = mpp->mptyp;
     if (ptyp != IO_IN) continue; 
     if (!mpp->has_scalar_mpps) continue;
     bld_pb_mpps(mpp);
    }
   __pop_wrkitstk();
  }
}

/*
 * bld the separated per bit output mod port lhs net pins for one iconn
 *
 * this is symmetric case to bld per bit separable mpps and and iconn 
 * pb ipins tabs for output but here as long as all highconns for port
 * can be separated into bits will separate if any low conn or high concats
 *
 * can only build PB records for output ports if all iconn widths exactly
 * same as mod port width
 */
static void bld_pbsep_output_mpps(void)
{
 register int32 pi, bi;
 register struct mod_t *mdp;
 int32 ii, pi2, numpins, ptyp;
 struct mod_t *imdp;
 struct mod_pin_t *mpp;
 struct inst_t *ip;
 struct expr_t *xp, *xp2;
 struct pbexpr_t *pbexpr;

 if (!output_pb_separable()) return;

 /* first bld per bit ipins tab for output port lhs separable highconns */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   /* process all instances in module */
   for (ii = 0; ii < mdp->minum; ii++)
    {
     ip = &(mdp->minsts[ii]);
     imdp = ip->imsym->el.emdp;
     if ((numpins = imdp->mpnum) == 0) continue;

     for (pi = 0; pi < numpins; pi++)
      {
       mpp = &(imdp->mpins[pi]);
       ptyp = mpp->mptyp;
       if (ptyp != IO_OUT) continue; 
       if (!mpp->has_scalar_mpps) continue;
       xp = ip->ipins[pi];

       if (ip->pb_ipins_tab == NULL)
        {
         ip->pb_ipins_tab = (struct expr_t ***)
          __my_malloc(numpins*sizeof(struct expr_t **));
         for (pi2 = 0; pi2 < numpins; pi2++) ip->pb_ipins_tab[pi2] = NULL;
        }
       /* all bits filled below - know lhs iconn and rhs mpp same width */
       ip->pb_ipins_tab[pi] = (struct expr_t **)
        __my_malloc(mpp->mpwide*sizeof(struct expr_t *));
       
       pbexpr = bld_pb_expr_map(xp, mpp->mpwide);
       for (bi = 0; bi < mpp->mpwide; bi++)
        {
         xp2 = bld_1sep_pbit_expr(&(pbexpr[bi]), xp->x_stren);
         ip->pb_ipins_tab[pi][bi] = xp2;
        }
       __my_free((char *) pbexpr, numpins*sizeof(struct pbexpr_t));
      }
    }
   __pop_wrkitstk();
  }

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   numpins = mdp->mpnum;
   for (pi = 0; pi < numpins; pi++)
    {
     mpp = &(mdp->mpins[pi]);
     ptyp = mpp->mptyp;
     if (ptyp != IO_IN) continue; 
     if (!mpp->has_scalar_mpps) continue;
     bld_pb_mpps(mpp);
    }
   __pop_wrkitstk();
  }
}

/*
 * mark each mod output port that can be and gains from separate into PB form
 * return T if some ports can be separated into PB form
 * 
 * separable only if more than half total highconn inst connections separble
 * and concats not too narrow only counted
 */
static int32 output_pb_separable(void)
{
 register int32 pi, ii;
 register struct mod_t *mdp;
 register struct mod_pin_t *mpp;
 int32 numpins, nels, num_cat_insts, not_pbsep, some_pbsep;
 struct itree_t *down_itp;
 struct expr_t *up_lhsx;

 /* first go through and make sure all high conns can be separated into pb */ 
 some_pbsep = FALSE;
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   /* SJM 11/19/02 - if top level mod, never separable since no highconn */
   if (mdp->minstnum == 0) continue;

   if ((numpins = mdp->mpnum) == 0) continue;
   __push_wrkitstk(mdp, 0);
   for (pi = 0; pi < numpins; pi++)
    {
     mpp = &(mdp->mpins[pi]);
     if (mpp->mptyp != IO_OUT) continue;


     /* if down rhs lowconn mod port not separable, can't PB separate any */
     if (!rhs_modpin_separable(mpp->mpref)) continue;
   
     num_cat_insts = 0; 
     not_pbsep = FALSE;
     for (ii = 0; ii < mdp->flatinum; ii++) 
      {
       down_itp = mdp->moditps[ii];
       /* LOOKATME - is nil here possible? */
       up_lhsx = down_itp->itip->ipins[pi];
       if (up_lhsx->optyp != LCB) continue;

       /* if any width differences, can't separate */
       /* LOOKATME - is this really required? */
       if (mpp->mpref->szu.xclen != up_lhsx->szu.xclen)  
        { not_pbsep = TRUE; break; }

       nels = __cnt_cat_size(up_lhsx);
       /* only gain from separate if 4 or more els and wider than 4 bits */
       /* LOOKATME - if many wide per bit maybe not better */
       if (nels < 4 || up_lhsx->szu.xclen < 4) continue;
       num_cat_insts++;
      }
     if (num_cat_insts/2 < mdp->flatinum) not_pbsep = TRUE;
     if (!not_pbsep)
      {
       some_pbsep = TRUE;
       mpp->has_scalar_mpps = TRUE;
      }
    }
   __pop_wrkitstk();
  }
 return(some_pbsep);
}

/*
 * build per bit mpps for simulation 
 */
static void bld_pb_mpps(struct mod_pin_t *mpp)
{
 register int32 bi;
 register struct mod_pin_t *mpp2;
 struct expr_t *xp;
 struct pbexpr_t *pbexpr;

 xp = mpp->mpref;
 pbexpr = bld_pb_expr_map(xp, mpp->mpwide);
 mpp->pbmpps = (struct mod_pin_t *)
  __my_malloc(mpp->mpwide*sizeof(struct mod_pin_t));
 for (bi = 0; bi < mpp->mpwide; bi++)
  {
   /* fields except for per bit same */
   mpp->pbmpps[bi] = *mpp;
   mpp2 = &(mpp->pbmpps[bi]);
   /* since expr copied for ID/XMR part, stren and other bits set right */
   xp = bld_1sep_pbit_expr(&(pbexpr[bi]), mpp2->mpref->x_stren);
   mpp2->mpref = xp;
   mpp2->mpwide = 1;
   mpp2->pbmpps = NULL;
   mpp2->has_scalar_mpps = FALSE;
  }
 __my_free((char *) pbexpr, mpp->mpwide*sizeof(struct pbexpr_t));
}

/*
 * bld the separated per bit rhs concat - stored in non pb concat 
 */
static void bld_pb_contas(void)
{
 register int32 bi;
 register struct mod_t *mdp;
 int32 cawid, cai;
 struct conta_t *cap, *pbcap;
 struct expr_t *lhsx, *xp2, *xp3;
 struct pbexpr_t *rhs_pbexpr, *lhs_pbexpr;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   for (cap = &(mdp->mcas[0]), cai = 0; cai < mdp->mcanum; cai++, cap++)
    {
     /* if not concat expr, should not separate so this returns F */
     if (!rhs_cat_separable(cap->rhsx)) continue;

     /* SJM 09/28/02 - now always use per bit for rhs concat */
     /* fi>1 works and may allow per bit driver load and 4v delay works */
     /* because evaluates all bits of rhs during sim */
     lhsx = cap->lhsx;
     cawid = lhsx->szu.xclen;
     cap->ca_pb_sim = TRUE;
     cap->pbcau.pbcaps = (struct conta_t *)
      __my_malloc(cawid*sizeof(struct conta_t));
     for (bi = 0; bi < cawid; bi++)
      {
       cap->pbcau.pbcaps[bi] = *cap;
       pbcap = &(cap->pbcau.pbcaps[bi]);
       /* SJM 08/28/02 - sym and delays in mast not per bit */
       pbcap->casym = NULL;
       pbcap->ca_du.pdels = NULL;
       pbcap->ca_pb_sim = FALSE;
       pbcap->ca_pb_el = TRUE;
       pbcap->pbcau.mast_cap = cap;
      }

     rhs_pbexpr = bld_pb_expr_map(cap->rhsx, cawid);
     lhs_pbexpr = bld_pb_expr_map(lhsx, cawid);
     for (bi = 0; bi < cawid; bi++)
      {
       pbcap = &(cap->pbcau.pbcaps[bi]);
       /* if lhs too narrow, 0/z determened by whether lhs stren */
       xp2 = bld_1sep_pbit_expr(&(rhs_pbexpr[bi]), lhsx->x_stren);
       pbcap->rhsx = xp2;

       xp3 = bld_1sep_pbit_expr(&(lhs_pbexpr[bi]), lhsx->x_stren);
       pbcap->lhsx = xp3;
       if (cap->lhsx->x_multfi) pbcap->lhsx->x_multfi = TRUE;
      }
     __my_free((char *) rhs_pbexpr, cawid*sizeof(struct pbexpr_t));
     __my_free((char *) lhs_pbexpr, cawid*sizeof(struct pbexpr_t));
    }
   __pop_wrkitstk();
  }
}

/*
 * return T if separable into per bit rhs concat 
 * now called with any expr 
 *
 * all elements must be simple
 * think width self determined exps legal in cats but can't per bit separate
 */
static int32 rhs_cat_separable(struct expr_t *rhsx)
{
 register struct expr_t *catndp;
 struct net_t *np;

 if (rhsx->optyp != LCB) return(FALSE);
 /* if only one element in rhs concat - no optimize */
 if (rhsx->ru.x->ru.x == NULL) return(FALSE);

 for (catndp = rhsx->ru.x; catndp != NULL; catndp = catndp->ru.x)
  {
   switch (catndp->lu.x->optyp) {
    case ID: case GLBREF:
     np = catndp->lu.x->lu.sy->el.enp;
     if (!np->vec_scalared) return(FALSE);
     break;
    case PARTSEL:
     np = catndp->lu.x->lu.x->lu.sy->el.enp;
     if (!np->vec_scalared) return(FALSE);
     break; 
    case LSB:
     /* SJM 12/17/02 - old vectored keyword also prevents pb separation */ 
     np = catndp->lu.x->lu.x->lu.sy->el.enp;
     if (np->n_isarr || !np->vec_scalared)
      return(FALSE);
     /* SJM 03/03/07 - bit select inside concat must be constant bsel */ 
     if (!__is_const_expr(catndp->lu.x->ru.x)) return(FALSE);
     break;
    case NUMBER: 
     break;
    default: return(FALSE);
   }
  }
 return(TRUE);
}

/*
 * return T if rhs mod pin separable (can be cat or not)
 */
static int32 rhs_modpin_separable(struct expr_t *rhsx)
{
 struct expr_t *catndp;

 /* 1 bit ports never separable */
 switch (rhsx->optyp) {
  case ID: case GLBREF:
   /* evan regs always separable */
   break;
  case PARTSEL:
   break; 
  case LSB:
   /* bsel not separable if array select */
   if (rhsx->lu.x->lu.sy->el.enp->n_isarr) return(FALSE);
   break;
  case NUMBER: 
   break;
  case LCB:  
   for (catndp = rhsx->ru.x; catndp != NULL; catndp = catndp->ru.x)
    {
     if (!rhs_modpin_separable(catndp->lu.x)) return(FALSE);
    }
  default: return(FALSE);
 }
 return(TRUE);
}

/*
 * decompose expr into per bit expr record 
 */
static struct pbexpr_t *bld_pb_expr_map(struct expr_t *xp, int32 xwid)
{
 register int32 bi, bi2;
 register struct expr_t *catndp;
 int32 nels, xi, xofs, biti, bitj, wi, wlen;
 word32 av, bv;
 struct expr_t **xtab, *cur_xp, *idndp;
 struct pbexpr_t *pbexpr, *pbxp;
 struct net_t *np;

 /* make sure both rhs and lhs in low to high order tables */
 /* needed because for concat, first in list is high order part */
 if (xp->optyp == LCB)
  {
   nels = __cnt_cat_size(xp);
   xtab = (struct expr_t **) __my_malloc(nels*sizeof(struct expr_t *));
   xi = nels - 1;
   /* fill expr tab in reverse order */
   catndp = xp->ru.x;
   for (; catndp != NULL; catndp = catndp->ru.x, xi--)
    { xtab[xi] = catndp->lu.x; }
  }
 else
  {
   xtab = (struct expr_t **) __my_malloc(sizeof(struct expr_t *));
   xtab[0] = xp;
   nels = 1;
  }

 /* size determined by lhs expr */
 pbexpr = (struct pbexpr_t *) __my_malloc(xwid*sizeof(struct pbexpr_t));
 for (bi = 0; bi < xwid; bi++) init_pbexpr_el(&(pbexpr[bi]));
 
 /* first fill lhs, then another loop to fill rhs matching */
 /* process one lhs expr element every time through */
 for (xofs = 0, xi = 0; xi < nels; xi++)
  {
   cur_xp = xtab[xi];
   pbxp = &(pbexpr[xofs]);
   /* fill lhs */
   switch ((byte) cur_xp->optyp) {
    case GLBREF: case ID:
     idndp = cur_xp; 
     np = idndp->lu.sy->el.enp;
     /* scalar */
     /* lhs bi is offset in object, -1 is entire object such as out of rng */
     if (!np->n_isavec)
      {
       pbxp = &(pbexpr[xofs]);
       pbxp->xp = idndp;
       pbxp->bi = -1;
       /* SJM 05/25/05 - need gt-eq since must stop at xwid not 1 past */
       if (++xofs >= xwid) goto done;
       break;
      }
     /* vector */
     for (bi2 = 0; bi2 < np->nwid; bi2++) 
      {
       pbxp = &(pbexpr[xofs]);
       pbxp->xp = idndp;
       /* this is offset in expr */
       pbxp->bi = bi2;
       if (++xofs >= xwid) goto done;
      }        
     break;
    case LSB:
     idndp = cur_xp->lu.x; 
     np = idndp->lu.sy->el.enp;
     pbxp->xp = cur_xp;
     /* DBG remove */
     if (np->n_isarr) __misc_terr(__FILE__, __LINE__);
     /* --- */
     /* SJM 09/03/02 - IS number legal since scalar decl assigns allow it */  
     if (cur_xp->ru.x->optyp == ISNUMBER) biti = -2;
     else biti = __get_const_bselndx(cur_xp);
     /* LOOKATME - think out of rng for const index not possible */ 
     if (biti == -1) pbxp->ndx_outofrng = TRUE;
     /* offset is offset in ID/XMR object */
     pbxp->bi = biti; 
     if (++xofs >= xwid) goto done;
     break;
    case PARTSEL:
     idndp = cur_xp->lu.x; 
     np = idndp->lu.sy->el.enp;
     /* know part select never IS */
     biti = __contab[cur_xp->ru.x->lu.x->ru.xvi];
     bitj = __contab[cur_xp->ru.x->ru.x->ru.xvi];
     for (bi2 = bitj; bi2 <= biti; bi2++)
      {
       pbxp = &(pbexpr[xofs]);
       pbxp->xp = idndp;
       /* this is offset in expr */
       pbxp->bi = bi2;
       /* this is offset from low end of concat element */
       if (++xofs >= xwid) goto done;
      } 
     break;
    case NUMBER:
     /* LOOKATME - could include ISNUMBER here if always has inst context */
     for (bi = 0; bi < cur_xp->szu.xclen; bi++)
      {
       pbxp = &(pbexpr[xofs]);
       pbxp->xp = cur_xp; 
       pbxp->rhs_const = TRUE;
       /* this is offset in expr */
       pbxp->bi = -1;
       wlen = wlen_(cur_xp->szu.xclen);
       wi = get_wofs_(bi);
       bi2 = get_bofs_(bi);
       av = (__contab[cur_xp->ru.xvi + wi] >> bi) & 1;
       bv = (__contab[cur_xp->ru.xvi + wlen + wi] >> bi) & 1;
       pbxp->aval = av;
       pbxp->bval = bv;
       /* SJM 05/25/05 - if out of lhs - remaining rhs unused so ignore */
       if (++xofs >= xwid) goto done; 
      } 
     break;
    default: __case_terr(__FILE__, __LINE__);
   } 
  }
 /* if still more lhs elements - rhs too narrow - this is bit by bit */
 for (; xofs < xwid; xofs++)
  {
   pbxp = &(pbexpr[xofs]);
   /* for extra lhs, need z assign if stren else 0 assign */
   pbxp->no_rhs_expr = TRUE;
   /* LOOKATME - just leaving rhs fields empty for now */
  }

done:
 if (xtab != NULL) __my_free((char *) xtab, nels*sizeof(struct expr_t *));
 return(pbexpr);
} 

/*
 * initialize decomposed into per bit expr 1 bit table entry 
 */
static void init_pbexpr_el(struct pbexpr_t *pbxp)
{
 pbxp->ndx_outofrng = FALSE;
 pbxp->rhs_const = FALSE;
 pbxp->no_rhs_expr = FALSE;
 pbxp->xp = NULL;
 pbxp->bi = -1;
 /* initialize to x */
 pbxp->aval = 1;
 pbxp->bval = 1;
}

/*
 * fill separated per bit assign expr pair table
 */
static struct expr_t *bld_1sep_pbit_expr(struct pbexpr_t *pbxp,
 int32 is_stren)
{
 struct expr_t *xp;

 /* some special cases */
 if (pbxp->ndx_outofrng)
  {
   xp = __bld_rng_numxpr(1, 1, 1);
   xp->ibase = BHEX;
   return(xp); 
  }
 if (pbxp->no_rhs_expr)
  {
   if (is_stren)
    {
     xp = __bld_rng_numxpr(0, 0, 1);
     xp->ibase = BDEC;
    }
   else
    {
     /* SJM 06/27/05 - rhs widening - always widen with 0's */
     xp = __bld_rng_numxpr(0, 0, 1);
     xp->ibase = BHEX;
    }
   return(xp); 
  }

 /* lhs bsel or ID/XMR scalar - copy the expr  */ 
 if (pbxp->xp->optyp == LSB ||
  ((pbxp->xp->optyp == ID || pbxp->xp->optyp == GLBREF)
  && !pbxp->xp->lu.sy->el.enp->n_isavec))
  { xp = __copy_expr(pbxp->xp); }
 /* rhs number - can never be IS number */
 else if (pbxp->xp->optyp == NUMBER)
  {
   xp = __bld_rng_numxpr(pbxp->aval, pbxp->bval, 1);
   xp->ibase = BDEC;
  }
 /* ID/XMR vector or psel, build bsel expr */
 else xp = cnvt_to_bsel_expr(pbxp->xp, pbxp->bi);
 return(xp);
}

/*
 * convert id or psel to constant bsel
 * passed index must be normalized to h:0 form
 *
 * LOOKATME - for bsel just copying expression - ignoring various IS
 * and out of range conditions - could get rid of flags
 */
static struct expr_t *cnvt_to_bsel_expr(struct expr_t *xp, int32 i1)
{
 int32 ri1, ri2;
 struct expr_t *new_xp, *new_xp2, *idndp, *selxp;
 struct net_t *np;

 if (xp->optyp == PARTSEL) idndp = xp->lu.x; else idndp = xp;
 new_xp = __copy_expr(idndp);
 np = idndp->lu.sy->el.enp;

 /* root expr of bit select */
 new_xp2 = __alloc_newxnd();
 new_xp2->optyp = LSB;
 new_xp2->szu.xclen = 1;
 new_xp2->lu.x = new_xp;
 selxp = __bld_rng_numxpr((word32) i1, 0L, WBITS);
 selxp->ibase = BDEC;
 
 __getwir_range(np, &ri1, &ri2); 
 if (ri2 != 0 || ri1 < ri2) selxp->ind_noth0 = TRUE;
 new_xp2->ru.x = selxp;
 return(new_xp2);
}

/*
 * compute number of elements in concat (passed LSB expr node)
 */
extern int32 __cnt_cat_size(struct expr_t *xp)
{
 register struct expr_t *catndp;
 int32 nels;

 for (nels = 0, catndp = xp->ru.x; catndp != NULL; catndp = catndp->ru.x)
  nels++;
 return(nels);
}

/*
 * ROUTINES TO BUILD SYMETRIC NET PIN LIST
 */

/*
 * build the net pin list and gate state and set port expr. storarge ptrs.
 *
 * this will automatically include xmr's
 * net ranges must have been changed from NX_CT
 * also short circuits expression node id's to point to storage
 *
 * know will not see def or spec params since frozed before here
 * and different code for special param np lists disjoint
 */
static void bld_nplist(void)
{
 register int32 pi, pbi, cai;
 register struct mod_pin_t *mpp;
 int32 ii, gi, ptyp, pnum;
 struct inst_t *ip;
 struct mod_t *mdp, *imdp;
 struct gate_t *gp;
 struct conta_t *cap, *pb_cap;
 struct expr_t *xp, *pb_xp;
 struct tfrec_t *tfrp; 
 struct tfarg_t *tfap;
 struct mod_pin_t *pb_mpp;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   /* first process all instances in module */
   for (ii = 0; ii < mdp->minum; ii++)
    {
     __cur_npii = ii;
     ip = &(mdp->minsts[ii]);
     imdp = ip->imsym->el.emdp;
     if ((pnum = imdp->mpnum) == 0) continue;

     for (pi = 0; pi < pnum; pi++)
      {
       mpp = &(imdp->mpins[pi]);
       xp = ip->ipins[pi];
       ptyp = mpp->mptyp;

       __cur_npnum = pi;
       __cur_pbi = -1;
       /* inst. input port rhs is driver that propagates down into module */
       if (ptyp == IO_IN)
        { 
         /* SJM 09/18/02 - scalarize if separable concat hconn */
         if (mpp->has_scalar_mpps && rhs_cat_separable(xp))
          {
           /* lhs width is mpp width since input port down assign */
           for (pbi = 0; pbi < mpp->mpwide; pbi++)
            {
             pb_xp = ip->pb_ipins_tab[pi][pbi];
             __cur_pbi = pbi;
             bld_rhsexpr_npins(pb_xp, NP_PB_ICONN);
            }
          }
         else bld_rhsexpr_npins(xp, NP_ICONN);
        }

       /* inst output port lhs is load propagated up from down module */
       /* whether in load or driver list determines interpretation */
       else if (ptyp == IO_OUT)
        {
         if (mpp->has_scalar_mpps)
          {
           for (pbi = 0; pbi < mpp->mpwide; pbi++)
            {
             pb_xp = ip->pb_ipins_tab[pi][pbi];
             __cur_pbi = pbi;
             bld_lhsexpr_npins(pb_xp, NP_PB_ICONN);
            }
          }
         else bld_lhsexpr_npins(xp, NP_ICONN);
        }
       /* SJM 08/23/00 - for empty port IO type unknown and no npins */
       else if (ptyp == IO_UNKN) ;
       else
        {
         /* SJM - 08/25/00 - now allowing concat inouts */ 
         /* FIXME - SJM - 08/27/00 - still but when concat on both sides */
         if (xp->optyp == LCB && mpp->mpref->optyp == LCB)  
          {
           __gferr(711, ip->isym->syfnam_ind, ip->isym->sylin_cnt,
            "instance %s of type %s inout port %s (pos. %d) concatenate on both sides unsupported",
            ip->isym->synam, ip->imsym->synam, __to_mpnam(__xs, mpp->mpsnam),
            pi + 1);
          }
         else bld_lhsexpr_npins(xp, NP_BIDICONN);
	}
      }
    }

   /* next gates including udps */
   /* need for rhs select for lhs concat never possible for gates */
   __cur_pbi = -1;
   __cur_lhscati1 = __cur_lhscati2 = -1;
   for (gi = 0; gi < mdp->mgnum; gi++)
    {
     gp = &(mdp->mgates[gi]);
     __cur_npgp = gp;
     switch ((byte) gp->g_class) {
      case GC_PULL:
       /* one gate all drivers can have multiple pins */
       for (pi = 0; pi < (int32) gp->gpnum; pi++)
        {
         xp = gp->gpins[pi];
         __cur_npnum = pi;
         bld_lhsexpr_npins(xp, NP_PULL);
        }
       continue;
      /* trans in separate switch channels, drvs needed here for building */
      /* and to store tran graph edge, but removed from ndrvs */ 
      case GC_TRAN: 
       /* SJM 04/26/01 - if both terminals same, no effect - omit from nl */ 
       if (gp->g_gone) continue;

       /* build for directional tran to first lhs port */
       /* first driver for forward to port 0 lhs tran */
bld_tran_ports:
       __cur_npnum = 0;
       xp = gp->gpins[0];
       if (xp->optyp != OPEMPTY) bld_lhsexpr_npins(xp, NP_TRAN);
       /* then driver for backwards port 1 lhs tran */
       __cur_npnum = 1;
       xp = gp->gpins[1];
       if (xp->optyp != OPEMPTY) bld_lhsexpr_npins(xp, NP_TRAN);
       continue;
      case GC_TRANIF:
       /* SJM 04/26/01 - if both terminals same, no effect - omit from nl */ 
       if (gp->g_gone) continue;

       /* build rhs load for 3rd port enable input */
       /* 3rd input port is only a load - never assigned to */
       __cur_npnum = 2;
       bld_rhsexpr_npins(gp->gpins[2], NP_TRANIF);
       goto bld_tran_ports;
      default:
       /* if output unc. (OPEMPTY), chges are not seen (do not propagate) */
       if (gp->gpins[0]->optyp == OPEMPTY) continue;
       __cur_npnum = 0;
       bld_lhsexpr_npins(gp->gpins[0], NP_GATE);
       /* notice pnum is only input pins */
       for (pi = 1; pi < (int32) gp->gpnum; pi++)
        {
         xp = gp->gpins[pi];
         __cur_npnum = pi;
         bld_rhsexpr_npins(xp, NP_GATE);
        }
     }
    }
   __cur_npnum = 0;
   for (cap = &(mdp->mcas[0]), cai = 0; cai < mdp->mcanum; cai++, cap++)
    {
     if (cap->ca_pb_sim)
      { 
       /* for rhs cat without 4v delay, simulator as per bit */ 
       for (pbi = 0; pbi < cap->lhsx->szu.xclen; pbi++)
        {
         pb_cap = &(cap->pbcau.pbcaps[pbi]);
        /* notice for PB cap still need actual cap - will index during sim */
         __cur_npcap = cap;
         __cur_pbi = pbi;
         bld_lhsexpr_npins(pb_cap->lhsx, NP_CONTA);
         bld_rhsexpr_npins(pb_cap->rhsx, NP_CONTA);
        }
      }
     else
      {
       __cur_npcap = cap;
       __cur_pbi = -1;
       /* for conta expression determines width */
       bld_lhsexpr_npins(cap->lhsx, NP_CONTA);
       bld_rhsexpr_npins(cap->rhsx, NP_CONTA);
      }
    }

   /* SJM - 04/30/99 - no longer removing ports but still no npps */
   /* for top level modules that have ports still - no npps */
   if (mdp->minstnum == 0) goto nxt_mod;

   /* module ports */
   pnum = mdp->mpnum;
   for (pi = 0; pi < pnum; pi++)
    {
     mpp = &(mdp->mpins[pi]);
     xp = mpp->mpref;
     ptyp = mpp->mptyp;
     __cur_npmdp = mdp;
     __cur_npnum = pi;
     __cur_pbi = -1;

     /* module inputs are lvalues (opposite cells) */
     if (ptyp == IO_IN)
      {
       /* SJM 09/18/02 - scalarize if any concat hconn */
       /* LOOKATME ??? - what if some high conns not pb seperable concats? */ 
       /* since only for inputs only drivers here, think for non sep */
       /* does not hurt to use per bit drivers */
       if (mpp->has_scalar_mpps)
        {
         /* lhs width is mpp width since input port down assign */
         for (pbi = 0; pbi < mpp->mpwide; pbi++)
          {
           pb_mpp = &(mpp->pbmpps[pbi]);
           pb_xp = pb_mpp->mpref;
           __cur_pbi = pbi;
           bld_lhsexpr_npins(pb_xp, NP_PB_MDPRT);
          }
        }
       else bld_lhsexpr_npins(xp, NP_MDPRT);
      }
     else if (ptyp == IO_OUT)
      {
       if (mpp->has_scalar_mpps)
        {
         for (pbi = 0; pbi < mpp->mpwide; pbi++)
          {
           pb_mpp = &(mpp->pbmpps[pbi]);
           pb_xp = pb_mpp->mpref;
           __cur_pbi = pbi;
           bld_rhsexpr_npins(pb_xp, NP_PB_MDPRT);
          }
        }
       else bld_rhsexpr_npins(xp, NP_MDPRT);
      }
     /* inouts have no loads or drivers since tran channel eval instead */
     /* but for PLI nlds and ndrvs set but never used for simulation */
     else if (ptyp == IO_BID)
      {
       /* SJM - 08/25/00 - now allowing concat connected inout port defs */ 
       bld_lhsexpr_npins(xp, NP_BIDMDPRT);
      }
     /* SJM 08/23/00 - for empty port IO type unknown and no npins */
     else if (ptyp == IO_UNKN) ;
     else __case_terr(__FILE__, __LINE__);
    }
nxt_mod:
   __pop_wrkitstk();
  }

 /* keeping one module wide tf rec list */
 __cur_pbi = -1;
 for (tfrp = __tfrec_hdr; tfrp != NULL; tfrp = tfrp->tfrnxt)
  { 
   for (pi = 1; pi < tfrp->tfanump1; pi++)
    {
     tfap = &(tfrp->tfargs[pi]);
     xp = tfap->arg.axp; 
     if (!xp->tf_isrw) continue;

     /* DBG remove --- */
     if (tfap->anp == NULL) __misc_terr(__FILE__, __LINE__);
     /* --- */

     if (tfap->anp->ntyp >= NONWIRE_ST) continue; 
     __cur_nptfrp = tfrp; 
     __cur_npnum = pi;
     bld_lhsexpr_npins(xp, NP_TFRWARG);
    } 
  } 
}

/*
 * build net_pin elements for wire only (lhs) output or inout port
 * or gate or conta output port
 * know bit/part select not declared with vectored attribute or prev. error
 * know concatenates must be exactly 1 level 
 *
 * this also sets has delay bit in top level noded if any element has delay
 */
static void bld_lhsexpr_npins(struct expr_t *xp, int32 npctyp)
{
 __cur_lhscati1 = __cur_lhscati2 = -1;
 __lhsxpr_has_ndel = FALSE;
 bld2_lhsexpr_npins(xp, npctyp);
 if (__lhsxpr_has_ndel) xp->lhsx_ndel = TRUE;
}

/*
 * notice lhs xmr's automatically work on lhs no cross module propagation
 * either assign to wires where gref info used to find storage and propagate
 * in target inst. or eval. drivers using target inst. where needed
 * propagation after assignment in cur. itp of destination is wire by wire
 */
static void bld2_lhsexpr_npins(struct expr_t *xp, int32 npctyp)
{
 int32 biti, bitj;
 struct expr_t *idndp;
 struct net_t *np;
 struct expr_t *catxp;
 struct gref_t *grp;

 grp = NULL;
 switch ((byte) xp->optyp) {
  /* this can happen for unc. */
  case GLBREF:
   idndp = xp;
   np = xp->lu.sy->el.enp;
   grp = idndp->ru.grp;
   if (np->nrngrep == NX_DWIR) __lhsxpr_has_ndel = TRUE;
   __conn_npin(np, -1, -1, TRUE, npctyp, grp, NPCHG_NONE, (char *) NULL);
   break;
  case ID:
   np = xp->lu.sy->el.enp;
   if (np->nrngrep == NX_DWIR) __lhsxpr_has_ndel = TRUE;
   /* by here anything that must be scalared has been or error */
   __conn_npin(np, -1, -1, TRUE, npctyp, grp, NPCHG_NONE, (char *) NULL);
   break;
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM: case OPEMPTY:
   return;
  case LSB:
   idndp = xp->lu.x;
   np = idndp->lu.sy->el.enp;
   if (np->nrngrep == NX_DWIR) __lhsxpr_has_ndel = TRUE;
   /* know will be constant or will not be on decl. lhs */
   /* DBG remove --- */ 
   if (!np->vec_scalared) __misc_terr(__FILE__, __LINE__);
   /* --- */
   if (idndp->optyp == GLBREF) grp = idndp->ru.grp;
   if (xp->ru.x->optyp == ISNUMBER)
    {
     /* SJM 10/12/04 - IS const must be contab ndx since contab realloced */
     __isform_bi_xvi = xp->ru.x->ru.xvi;
     __conn_npin(np, -2, 0, TRUE, npctyp, grp, NPCHG_NONE, (char *) NULL);
    }
   else
    {
     biti = (int32 ) __contab[xp->ru.x->ru.xvi];
     __conn_npin(np, biti, biti, TRUE, npctyp, grp, NPCHG_NONE,
      (char *) NULL);
    }
   break;
  case PARTSEL:
   idndp = xp->lu.x;
   np = idndp->lu.sy->el.enp;

   /* DBG remove --- */ 
   if (!np->vec_scalared) __misc_terr(__FILE__, __LINE__);
   /* --- */
   if (idndp->optyp == GLBREF) grp = idndp->ru.grp;

   /* for path dest. this is on */
   if (np->nrngrep == NX_DWIR) __lhsxpr_has_ndel = TRUE;
   /* array stored from 0 to size even though bits go high to 0 */
   /* never IS form */
   biti = (int32) (__contab[xp->ru.x->lu.x->ru.xvi]);
   bitj = (int32) (__contab[xp->ru.x->ru.x->ru.xvi]);

   __conn_npin(np, biti, bitj, TRUE, npctyp, grp, NPCHG_NONE, (char *) NULL);
   break;
  case LCB:
   for (catxp = xp->ru.x; catxp != NULL; catxp = catxp->ru.x)
    {
     /* catxp length is distance from high bit to rhs end (0) */
     __cur_lhscati1 = catxp->szu.xclen - 1;
     __cur_lhscati2 = __cur_lhscati1 - catxp->lu.x->szu.xclen + 1;
     bld2_lhsexpr_npins(catxp->lu.x, npctyp);
    }
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
}

/*
 * build net_pin elements for rhs reg or wire
 * these are things that when wire changes (is assigned to) are traced to
 * propagate change by evaluating rhs these are in
 *
 * idea here is too build as normal, then add special itp stuff if needed
 * for any global needed because when wire changes in target (eval itp),
 * must propagate cross module to place used
 */
static void bld_rhsexpr_npins(struct expr_t *xp, int32 npctyp)
{
 int32 biti, bitj;
 struct net_t *np;
 struct expr_t *idndp, *selxp;
 struct gref_t *grp;

 grp = NULL;
 switch ((byte) xp->optyp) {
  case NUMBER: case ISNUMBER: case REALNUM: case ISREALNUM: return;
  case OPEMPTY:
   /* could handle `unconndrive by adding some kind of driver here ? */
   return; 
  case GLBREF:
   idndp = xp;
   np = xp->lu.sy->el.enp;
   grp = idndp->ru.grp;
   __conn_npin(np, -1, -1, FALSE, npctyp, grp, NPCHG_NONE, (char *) NULL);
   break;
  case ID:
   np = xp->lu.sy->el.enp;
   __conn_npin(np, -1, -1, FALSE, npctyp, grp, NPCHG_NONE, (char *) NULL);
   break;
  case LSB:
   idndp = xp->lu.x;
   np = idndp->lu.sy->el.enp;
   if (idndp->optyp == GLBREF) grp = idndp->ru.grp;
   selxp = xp->ru.x;
   /* for registers or arrays, never split, change must always propagate */
   /* this means no per array cell tab, if any cell changes, reevaluate */
   /* if reg (not wire) or vectored and not arr, any chg causes propagation */ 
   if (!np->vec_scalared && !np->n_isarr)
    {
     /* if this constant just does nothing */
     bld_rhsexpr_npins(selxp, npctyp);
     __conn_npin(np, -1, -1, FALSE, npctyp, grp, NPCHG_NONE, (char *) NULL);
    }
   else if (selxp->optyp == NUMBER)
    {
     biti = (int32) (__contab[selxp->ru.xvi]);
     __conn_npin(np, biti, biti, FALSE, npctyp, grp, NPCHG_NONE,
      (char *) NULL);
    }
   else if (selxp->optyp == ISNUMBER)
    {
     /* SJM 10/12/04 - IS const must be contab ndx since contab realloced */
     __isform_bi_xvi = selxp->ru.xvi;
     __conn_npin(np, -2, 0, FALSE, npctyp, grp, NPCHG_NONE, (char *) NULL);
    }
   else
    {
     __conn_npin(np, -1, -1, FALSE, npctyp, grp, NPCHG_NONE, (char *) NULL);
     bld_rhsexpr_npins(selxp, npctyp);
    }
   break;
  case PARTSEL:
   idndp = xp->lu.x;
   np = idndp->lu.sy->el.enp;
   if (idndp->optyp == GLBREF) grp = idndp->ru.grp;
   /* for part select of reg - still need to match all bits */
   /* know here both range will be constants */
   if (!np->vec_scalared)
    __conn_npin(np, -1, -1, FALSE, npctyp, grp, NPCHG_NONE, (char *) NULL);
   else
    {
     /* array stored from 0 to size even though bits go high to 0 */
     selxp = xp->ru.x;
     /* never IS form */
     biti = (int32) (__contab[selxp->lu.x->ru.xvi]);
     bitj = (int32) (__contab[selxp->ru.x->ru.xvi]);
     __conn_npin(np, biti, bitj, FALSE, npctyp, grp, NPCHG_NONE,
      (char *) NULL);
    }
   break;
  case FCALL:
   {
    register struct expr_t *fax;

    /* if any args of system or user functions change, monitor triggers */
    for (fax = xp->ru.x; fax != NULL; fax = fax->ru.x)
     bld_rhsexpr_npins(fax->lu.x, npctyp);
   }
   return;
  case LCB:
   {
    register struct expr_t *catxp;

    for (catxp = xp->ru.x; catxp != NULL; catxp = catxp->ru.x)
     bld_rhsexpr_npins(catxp->lu.x, npctyp);
   }
   return;
  default:
   if (xp->lu.x != NULL) bld_rhsexpr_npins(xp->lu.x, npctyp);
   if (xp->ru.x != NULL) bld_rhsexpr_npins(xp->ru.x, npctyp);
   return;
 }
}

/*
 * scheme for various types of xmr npps
 *
 * not xmr (0)          - simple no bits set, no move to ref, no filter
 * xmr (not root)   (1) - xmrtyp, npu gref, no filter 
 * any rooted xmr   (2) - xmrtyp/1inst, npu filtitp to ref, npaux npdownitp 
 */

/*
 * routine to build 1 net pin connection per static location 
 *
 * LOOKATME - here do matching to find identical static tree upward inst_t
 * path but have static tree so maybe could traverse to not need to match?
 */
extern void __conn_npin(struct net_t *np, int32 ni1, int32 ni2, int32 islhs,
 int32 npctyp, struct gref_t *grp, int32 chgtyp, char *chgp)
{
 struct net_pin_t *npp;

 /* case: non xmr */ 
 if (grp == NULL)
  {
   npp = conn2_npin(np, ni1, ni2, islhs, npctyp);
   if (chgtyp != NPCHG_NONE) set_chgsubfld(npp, chgtyp, chgp);
   return;
  }
 /* case 2: rooted xmr */
 if (grp->is_rooted)
  conn_rtxmr_npin(np, ni1, ni2, islhs, npctyp, grp, chgtyp, chgp);
 /* case 3: non rooted xmr */ 
 else conn_xmr_npin(np, ni1, ni2, islhs, npctyp, grp, chgtyp, chgp);
}

/*
 * connect any rooted xmr net pin
 *
 * idea here is that there will be one npp for each module containing
 * reference the one driver or load (rooted xmr) drivers or is a load
 * of all instances
 *
 * for rooted:
 *  when wire changes must match npauxp npdownitp inst or not load or driver
 *  to move from target (wire that changes) to ref. one loc. is npdownitp
 */
static void conn_rtxmr_npin(struct net_t *np, int32 ni1, int32 ni2,
 int32 islhs, int32 npctyp, struct gref_t *grp, int32 chgtyp, char *chgp)
{
 register int32 ii;
 struct net_pin_t *npp; 
 struct itree_t *targitp;

 targitp = grp->targu.targitp;
 /* here npp is not instance specific */ 
 for (ii = 0; ii < grp->gin_mdp->flatinum; ii++)
  {
   npp = conn2_npin(np, ni1, ni2, islhs, npctyp);
   if (chgtyp != NPCHG_NONE) set_chgsubfld(npp, chgtyp, chgp);  
   if (npp->npaux == NULL) npp->npaux = __alloc_npaux();
   /* this is place npp referenced from */
   npp->npaux->npdownitp = (struct itree_t *) grp->gin_mdp->moditps[ii];

   /* for rooted, target (place wire in) must be the one changed wire in */
   /* itree location - need to filter */
   npp->npaux->npu.filtitp = targitp;

   npp->np_xmrtyp = XNP_RTXMR;
   npp->npproctyp = NP_PROC_FILT;
  }
}

/*
 * connect an xmr that is not rooted
 * for upwards or downwards relative search using normal routines
 * no filtering since one npp where all refs have different target
 */
static void conn_xmr_npin(struct net_t *np, int32 ni1, int32 ni2, int32 islhs,
 int32 npctyp, struct gref_t *grp, int32 chgtyp, char *chgp)
{
 struct net_pin_t *npp;

 npp = conn2_npin(np, ni1, ni2, islhs, npctyp);
 if (chgtyp != NPCHG_NONE) set_chgsubfld(npp, chgtyp, chgp);  
 if (npp->npaux == NULL) npp->npaux = __alloc_npaux();
 /* use gref to move from target (npp on wire/inst) to ref. */
 npp->npaux->npu.npgrp = grp;
 if (grp->upwards_rel) npp->np_xmrtyp = XNP_UPXMR;
 else npp->np_xmrtyp = XNP_DOWNXMR;
 npp->npproctyp = NP_PROC_GREF;
}

/*
 * connect a range of net bits
 * not for register output or trigger net pin list building
 * all allocation and connecting of net pins done through here
 *
 * the xmr fields must be set by caller if needed
 */
static struct net_pin_t *conn2_npin(struct net_t *np, int32 ni1, int32 ni2,
 int32 islhs, int32 npctyp)
{
 struct net_pin_t *npp;
 
 /* if load of vectored vector - always just mark change of entire wire */ 
 if (!islhs && !np->n_isarr && np->n_isavec && !np->vec_scalared)
  ni1 = ni2 = -1; 

 npp = __alloc_npin(npctyp, ni1, ni2);
 /* notice 32 bit dependent since assumes ptr and int32 both 4 bytes */
 /* also __isform_bi xvi is contab index so can just use it */
 if (ni1 == -2)
  npp->npaux->nbi2.xvi = __isform_bi_xvi;

 /* link on front */
 if (islhs)
  {
   if (__cur_lhscati1 != -1)
    {
     if (npp->npaux == NULL) npp->npaux = __alloc_npaux();
     npp->npaux->lcbi1 = __cur_lhscati1;
     npp->npaux->lcbi2 = __cur_lhscati2;
    }
   npp->npnxt = np->ndrvs;
   np->ndrvs = npp;
  }
 else
  {
   /* never insert mipd nchg using this routine */
   /* DBG remove -- */
   if (npp->npntyp == NP_MIPD_NCHG) __misc_terr(__FILE__, __LINE__);
   /* --- */

   /* SJM 07/10/01 - MIPD load must always be first on list */
   /* SJM 07/24/01 - logic was wrong - now if front mipd nchg insert after */
   if (np->nlds != NULL && np->nlds->npntyp == NP_MIPD_NCHG)
    { npp->npnxt = np->nlds->npnxt; np->nlds->npnxt = npp; }
   else { npp->npnxt = np->nlds; np->nlds = npp; }
  }
 /* DBG remove ---
 if (__debug_flg)
  {
   struct npaux_t *npauxp;

   __dbg_msg(".. mod %s adding net %s pin type %d to front",
    __inst_mod->msym->synam, np->nsym->synam, npp->npntyp);
   if ((npauxp = npp->npaux) != NULL && npauxp->lcbi1 != -1)
    __dbg_msg("([%d:%d])\n", npauxp->lcbi1, npauxp->lcbi2);
   else __dbg_msg("\n");
  }
 --- */
 /* just put on front */
 return(npp);
}

/*
 * allocate a net pin element 
 * net pin form converted to table for all net pins in current module
 *
 * notice [i1:i2] are corrected to h:0 form by here
 * also notice some globals such as obnum (port) must be set before calling
 */
extern struct net_pin_t *__alloc_npin(int32 nptyp, int32 i1, int32 i2)
{
 struct net_pin_t *npp;
 struct primtab_t *ptp;

 npp = (struct net_pin_t *) __my_malloc(sizeof(struct net_pin_t));
 npp->npntyp = (word32) nptyp;
 npp->npproctyp = NP_PROC_INMOD;
 /* value explicitly set if needed */
 npp->chgsubtyp = 0; 
 
 npp->np_xmrtyp = XNP_LOC;
 npp->pullval = 0;
 /* assume more common load */
 npp->obnum = __cur_npnum;
 npp->pbi = -1;
 npp->npaux = NULL;
 switch ((byte) nptyp) {
  case NP_ICONN: case NP_BIDICONN:
   npp->elnpp.eii = __cur_npii;
   break;
  case NP_PB_ICONN:
   npp->elnpp.eii = __cur_npii;
   npp->pbi = __cur_pbi;
   break;
  case NP_GATE: case NP_TRAN: case NP_TRANIF:
   npp->elnpp.egp = __cur_npgp;
   break;
  case NP_CONTA:
   npp->elnpp.ecap = __cur_npcap;
   npp->pbi = __cur_pbi;
   break;
  case NP_MDPRT: case NP_BIDMDPRT:
   npp->elnpp.emdp = __cur_npmdp;
   break;
  case NP_PB_MDPRT:
   npp->elnpp.emdp = __cur_npmdp;
   npp->pbi = __cur_pbi;
   break;
  /* set later */
  case NP_TCHG: npp->elnpp.etchgp = NULL; break;
  case NP_PULL:
   npp->elnpp.egp = __cur_npgp;
   ptp = __cur_npgp->gmsym->el.eprimp; 
   npp->pullval = (ptp->gateid == G_PULLUP) ? 1 : 0; 
   break;
  case NP_TFRWARG:
   npp->elnpp.etfrp = __cur_nptfrp; 
   npp->obnum = __cur_npnum;
   break;
  case NP_VPIPUTV:
   npp->elnpp.enp = __cur_npnp;
   /* caller sets to index number of this driver */
   npp->obnum = 0;
   break;
  case NP_MIPD_NCHG: 
   npp->elnpp.enp = __cur_npnp;
   npp->obnum = 0;
   break;
  default: __case_terr(__FILE__, __LINE__);
 }
 npp->npnxt = NULL;

 if (i1 != -1)
  {
   npp->npaux = __alloc_npaux();
   npp->npaux->nbi1 = i1;
   npp->npaux->nbi2.i = i2;
  }
 return(npp);
}

/*
 * allocate and initialize the aux. net pin record
 */
extern struct npaux_t *__alloc_npaux(void)
{
 struct npaux_t *npauxp;

 npauxp = (struct npaux_t *) __my_malloc(sizeof(struct npaux_t));
 npauxp->nbi1 = -1;
 npauxp->nbi2.i = -1;
 npauxp->npu.npgrp = NULL;
 npauxp->npdownitp = NULL;
 npauxp->lcbi1 = npauxp->lcbi2 = -1;
 return(npauxp);
}

/*
 * set change subtype fields for a npp
 */
static void set_chgsubfld(struct net_pin_t *npp, int32 chgtyp, char *chgp) 
{
 npp->chgsubtyp = chgtyp;
 switch (chgtyp) {
  case NPCHG_TCSTART: npp->elnpp.etchgp = (struct tchg_t *) chgp; break;
  case NPCHG_TCCHK: npp->elnpp.echktchgp = (struct chktchg_t *) chgp; break;
  case NPCHG_PTHSRC: npp->elnpp.etchgp = (struct tchg_t *) chgp; break;
  default: __case_terr(__FILE__, __LINE__); 
 }
}

/*
 * ROUTINES TO BUILD DEFPARAM AND SPECPARAM CONTAINING EXPR LIST
 */

/*
 * add delay list parameters to param parm nplst for a net
 */
static void add_netdel_pnp(struct net_t *np, struct paramlst_t *pdels)
{
 register struct paramlst_t *pmp;
 int32 sav_fnam_ind, sav_slin_cnt;
 struct expr_t *dxp;
 struct parmnet_pin_t tmpl_pnp;

 init_pnp(&tmpl_pnp);
 tmpl_pnp.pnptyp = PNP_NETDEL;
 /* object delay for */ 
 tmpl_pnp.elpnp.enp = np;
 /* not copied but moved from previous pdels whose field is overwritten */
 tmpl_pnp.pnplp = pdels;

 sav_fnam_ind = __sfnam_ind;
 sav_slin_cnt = __slin_cnt; 
 __sfnam_ind = (int32) np->nsym->syfnam_ind;
 __slin_cnt = np->nsym->sylin_cnt;

 /* only free first one */
 __nd_parmpnp_free = TRUE;
 for (pmp = pdels; pmp != NULL; pmp = pmp->pmlnxt)
  { dxp = pmp->plxndp; addto_parmnplst(dxp, &tmpl_pnp); }

 __sfnam_ind = sav_fnam_ind;
 __slin_cnt = sav_slin_cnt;
}

/*
 * initialize a param net pin elemnt 
 */
static void init_pnp(struct parmnet_pin_t *pnp)
{
 pnp->pnp_free = FALSE;
 pnp->elpnp.enp = NULL;
 pnp->pnplp = NULL;
 pnp->pnpnxt = NULL;
}

/*
 * build a param nplst element from a param in an expression
 */
static void addto_parmnplst(struct expr_t *xp,
 struct parmnet_pin_t *tmplate_pnp)
{
 struct net_t *np;
 struct parmnet_pin_t *pnp;

 if (__isleaf(xp))
  {
   if (xp->optyp == GLBREF)
    {
     np = xp->lu.sy->el.enp;
      __sgfinform(457, 
       "hierarchical parameter %s used in delay expression can not be annotated to",
       __msgexpr_tostr(__xs, xp));
     return;
    }
   if (xp->optyp != ID || xp->lu.sy->sytyp != SYM_N) return;

   np = xp->lu.sy->el.enp;
   if (!np->n_isaparam) return;

   pnp = (struct parmnet_pin_t *) __my_malloc(sizeof(struct parmnet_pin_t));   
   *pnp = *tmplate_pnp; 
   if (__nd_parmpnp_free)
    { pnp->pnp_free = TRUE; __nd_parmpnp_free = FALSE; }
   else pnp->pnp_free = FALSE;
   /* reverse order, put on front */
   if (np->nlds == NULL) np->nlds = (struct net_pin_t *) pnp;
   else
    {
     pnp->pnpnxt = (struct parmnet_pin_t *) np->nlds; 
     np->nlds = (struct net_pin_t *) pnp;
    }
   return;
  }
 if (xp->lu.x != NULL) addto_parmnplst(xp->lu.x, tmplate_pnp);
 if (xp->ru.x != NULL) addto_parmnplst(xp->ru.x, tmplate_pnp);
}

/*
 * add delay list parameters to param parm nplst for a gate
 */
static void add_gatedel_pnp(struct gate_t *gp, struct paramlst_t *pdels)
{
 register struct paramlst_t *pmp;
 int32 sav_fnam_ind, sav_slin_cnt;
 struct expr_t *dxp;
 struct parmnet_pin_t tmpl_pnp;

 init_pnp(&tmpl_pnp);
 tmpl_pnp.pnptyp = PNP_GATEDEL;
 /* object delay for */ 
 tmpl_pnp.elpnp.egp = gp;
 /* not copied but moved from previous pdels whose field is overwritten */
 tmpl_pnp.pnplp = pdels;

 sav_fnam_ind = __sfnam_ind;
 sav_slin_cnt = __slin_cnt; 
 __sfnam_ind = (int32) gp->gsym->syfnam_ind;
 __slin_cnt = gp->gsym->sylin_cnt;

 /* only free first use - all other uses point to same parm npp */
 __nd_parmpnp_free = TRUE;
 for (pmp = pdels; pmp != NULL; pmp = pmp->pmlnxt)
  { dxp = pmp->plxndp; addto_parmnplst(dxp, &tmpl_pnp); }

 __sfnam_ind = sav_fnam_ind;
 __slin_cnt = sav_slin_cnt;
}

/*
 * add delay list parameters to param parm nplst for a conta
 */
static void add_contadel_pnp(struct conta_t *cap, struct paramlst_t *pdels)
{
 register struct paramlst_t *pmp;
 int32 sav_fnam_ind, sav_slin_cnt;
 struct expr_t *dxp;
 struct parmnet_pin_t tmpl_pnp;

 init_pnp(&tmpl_pnp);
 tmpl_pnp.pnptyp = PNP_CONTADEL;
 /* object delay for */ 
 tmpl_pnp.elpnp.ecap = cap;
 /* not copied but moved from previous pdels whose field is overwritten */
 tmpl_pnp.pnplp = pdels;

 sav_fnam_ind = __sfnam_ind;
 sav_slin_cnt = __slin_cnt; 
 __sfnam_ind = (int32) cap->casym->syfnam_ind;
 __slin_cnt = cap->casym->sylin_cnt;

 /* only free first one */
 __nd_parmpnp_free = TRUE;
 for (pmp = pdels; pmp != NULL; pmp = pmp->pmlnxt)
  { dxp = pmp->plxndp; addto_parmnplst(dxp, &tmpl_pnp); }

 __sfnam_ind = sav_fnam_ind;
 __slin_cnt = sav_slin_cnt;
}

/*
 * add delay list parameters to param parm nplst for a procedural delay ctrl
 *
 * know will only be one delay
 */
extern void __add_dctldel_pnp(struct st_t *stp)
{
 register struct paramlst_t *pmp;
 int32 sav_fnam_ind, sav_slin_cnt;
 struct delctrl_t *dctp;
 struct paramlst_t *pdels;
 struct expr_t *dxp;
 struct parmnet_pin_t tmpl_pnp;

 init_pnp(&tmpl_pnp);
 tmpl_pnp.pnptyp = PNP_PROCDCTRL;
 /* object delay for */ 
 dctp = stp->st.sdc;
 tmpl_pnp.elpnp.edctp = dctp;
 pdels = dctp->dc_du.pdels;
 /* not copied but moved from previous pdels whose field is overwritten */
 tmpl_pnp.pnplp = pdels;

 sav_fnam_ind = __sfnam_ind;
 sav_slin_cnt = __slin_cnt; 
 __sfnam_ind = (int32) stp->stfnam_ind;
 __slin_cnt = stp->stlin_cnt;

 /* only free first one */
 __nd_parmpnp_free = TRUE;
 /* DBG remove -- */
 if (pdels->pmlnxt != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */
 for (pmp = pdels; pmp != NULL; pmp = pmp->pmlnxt)
  { dxp = pmp->plxndp; addto_parmnplst(dxp, &tmpl_pnp); }

 __sfnam_ind = sav_fnam_ind;
 __slin_cnt = sav_slin_cnt;
}

/*
 * add delay list parameters to param parm nplst for a timing check
 *
 * know will only be one delay
 */
extern void __add_tchkdel_pnp(struct tchk_t *tcp, int32 is_1st)
{
 register struct paramlst_t *pmp;
 int32 sav_fnam_ind, sav_slin_cnt;
 struct paramlst_t *pdels;
 struct expr_t *dxp;
 struct parmnet_pin_t tmpl_pnp;

 init_pnp(&tmpl_pnp);
 tmpl_pnp.elpnp.etcp = tcp;
 if (is_1st)
  { tmpl_pnp.pnptyp = PNP_TCHKP1; pdels = tcp->tclim_du.pdels; }
 else { tmpl_pnp.pnptyp = PNP_TCHKP2; pdels = tcp->tclim2_du.pdels; }
 /* not copied but moved from previous pdels whose field is overwritten */
 tmpl_pnp.pnplp = pdels;

 sav_fnam_ind = __sfnam_ind;
 sav_slin_cnt = __slin_cnt; 
 __sfnam_ind = (int32) tcp->tcsym->syfnam_ind;
 __slin_cnt = tcp->tcsym->sylin_cnt;

 /* only free first one */
 __nd_parmpnp_free = TRUE;
 /* DBG remove -- */
 if (pdels->pmlnxt != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */
 for (pmp = pdels; pmp != NULL; pmp = pmp->pmlnxt)
  { dxp = pmp->plxndp; addto_parmnplst(dxp, &tmpl_pnp); }

 __sfnam_ind = sav_fnam_ind;
 __slin_cnt = sav_slin_cnt;
}

/*
 * add delay list parameters to param parm nplst for a timing check
 */
extern void __add_pathdel_pnp(struct spcpth_t *pthp)
{
 register struct paramlst_t *pmp;
 int32 sav_fnam_ind, sav_slin_cnt;
 struct paramlst_t *pdels;
 struct expr_t *dxp;
 struct parmnet_pin_t tmpl_pnp;

 init_pnp(&tmpl_pnp);
 tmpl_pnp.elpnp.epthp = pthp;
 tmpl_pnp.pnptyp = PNP_PATHDEL;
 pdels = pthp->pth_du.pdels;
 /* not copied but moved from previous pdels whose field is overwritten */
 tmpl_pnp.pnplp = pdels;

 sav_fnam_ind = __sfnam_ind;
 sav_slin_cnt = __slin_cnt; 
 __sfnam_ind = (int32) pthp->pthsym->syfnam_ind;
 __slin_cnt = pthp->pthsym->sylin_cnt;

 /* only free first one */
 __nd_parmpnp_free = TRUE;
 for (pmp = pdels; pmp != NULL; pmp = pmp->pmlnxt)
  { dxp = pmp->plxndp; addto_parmnplst(dxp, &tmpl_pnp); }

 __sfnam_ind = sav_fnam_ind;
 __slin_cnt = sav_slin_cnt;
}

/*
 * free all parm net pin in design
 *
 * after this is called information required to annotate delays from
 * params (labels) gone
 */
extern void __free_design_pnps(void)
{
 register int32 ni;
 register struct net_t *prmp;
 struct mod_t *mdp;
 struct task_t *tskp;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   if (__inst_mod->mprmnum != 0) 
    {
     for (ni = 0, prmp = &(__inst_mod->mprms[0]); ni < __inst_mod->mprmnum;
      ni++, prmp++)
      {
       /* DBG remove -- */
       if (!prmp->n_isaparam) __misc_terr(__FILE__, __LINE__);
       /* --- */
        
       if (prmp->nlds == NULL) continue;
       free_1parm_pnps(prmp); 
      }
    }
   for (tskp = __inst_mod->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     if (tskp->tprmnum == 0) continue;
     prmp = &(tskp->tsk_prms[0]);
     for (ni = 0; ni < tskp->tprmnum; ni++, prmp++)
      {
       if (!prmp->n_isaparam) continue;
       if (prmp->nlds == NULL) continue;
       free_1parm_pnps(prmp); 
      }
    }
   __pop_wrkitstk();
  }
}

/*
 * free the pnp parm net list for one parameter
 *
 * called after SDF and vpi_ cbEndOfCompile call back since from then
 * on assign to parameters in delay expressions (by vpi_, SDF annotate done)
 * has no effect since delays elaborated (or frozen)
 */
static void free_1parm_pnps(struct net_t *prmp)
{
 register struct parmnet_pin_t *pnp, *pnp2;

 for (pnp = (struct parmnet_pin_t *) prmp->nlds; pnp != NULL;)
  {
   pnp2 = pnp->pnpnxt;   
   if (pnp->pnp_free) __free_dellst(pnp->pnplp);
   __my_free((char *) pnp, sizeof(struct parmnet_pin_t));
   pnp = pnp2;
  }
}

/*
 * ROUTINES TO CONVERT KNOWN AT COMPILE TIME NPPS TO TABLE (ARRAY)
 */

/*
 * re allocate npp list into indexable table
 *
 * only for the in src (non bid/tran npps that are in tran channels
 * edges so removed) - keep as linked list since during sim will add
 * and remove - table allows generation of .s that does not change
 * from malloc differences - MIPD and vpi added npps can't be accessed
 * by index
 *
 * since bid tran npps pointed to by edges, can't realloc to table
 * but know that all tran channel npps (BID and TRAN) removed so
 * list can be converted to indexable table but only for in src npps
 */
static void realloc_npplist_to_tab(void)  
{
 register struct mod_t *mdp;
 register struct net_t *np;
 int32 ni;
 struct task_t *tskp;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++)
    {
     realloc_1net_npplist(np);
    }
   for (tskp = mdp->mtasks; tskp != NULL; tskp = tskp->tsknxt)
    {
     for (ni = 0, np = &(tskp->tsk_regs[0]); ni < tskp->trnum; ni++, np++)
      {
       realloc_1net_npplist(np);
      }
    }
  }
}

/*
 * realloc npp list into table (only for npps and dces in src) 
 *
 * nothing points at dce and the tran channel edges that point to npps
 * linked out by here
 *
 * fields pointed to from npps and dces can just be copied
 */
static void realloc_1net_npplist(struct net_t *np)
{
 register int32 i;
 int32 num_dces, num_npps;
 struct dcevnt_t *dcetab, *dcep, *dcep2;
 struct net_pin_t *npptab, *npp, *npp2;

 if (np->dcelst != NULL) 
  {
   num_dces = cnt_dces(np->dcelst);
   dcetab = (struct dcevnt_t *) __my_malloc(num_dces*sizeof(struct dcevnt_t)); 
   for (i = 0, dcep = np->dcelst; i < num_dces; i++)
    {
     dcep2 = dcep->dcenxt;

     dcetab[i] = *dcep;
     if (i > 0) dcetab[i - 1].dcenxt = &(dcetab[i]);

     __my_free((char *) dcep, sizeof(struct dcevnt_t));
     dcep = dcep2;
    }
   /* last one's nil, copied right */ 
   np->dcelst = dcetab;
  }
 if (np->nlds != NULL)
  {
   num_npps = cnt_npps(np->nlds);
   npptab = (struct net_pin_t *)
    __my_malloc(num_npps*sizeof(struct net_pin_t)); 
   for (i = 0, npp = np->nlds; i < num_npps; i++)
    {
     npp2 = npp->npnxt;

     npptab[i] = *npp;
     if (i > 0) npptab[i - 1].npnxt = &(npptab[i]);

     __my_free((char *) npp, sizeof(struct net_pin_t));
     npp = npp2;
    }
   /* last one's nil, copied right */ 
   np->nlds = npptab;
  }
 if (np->ndrvs != NULL)
  {
   num_npps = cnt_npps(np->ndrvs);
   npptab = (struct net_pin_t *)
    __my_malloc(num_npps*sizeof(struct net_pin_t)); 
   for (i = 0, npp = np->ndrvs; i < num_npps; i++)
    {
     npp2 = npp->npnxt;

     npptab[i] = *npp;
     if (i > 0) npptab[i - 1].npnxt = &(npptab[i]);

     __my_free((char *) npp, sizeof(struct net_pin_t));
     npp = npp2;
    }
   /* last one's nil, copied right */ 
   np->ndrvs = npptab;
  }
}

/*
 * count number of npps
 */
static int32 cnt_npps(struct net_pin_t *npp)
{
 int32 num_npps;

 for (num_npps = 0; npp != NULL; npp = npp->npnxt) num_npps++;
 return(num_npps);
}

/*
 * count number of dces
 */
static int32 cnt_dces(struct dcevnt_t *dcep)
{
 int32 num_dces;

 for (num_dces = 0; dcep != NULL; dcep = dcep->dcenxt) num_dces++;
 return(num_dces);
}

/*
 * ROUTINES TO IMPLEMENT GATE EATER
 */

/*
 * remove all gates and nets that are not driver or drive nothing
 *
 * if wire connects to I/O port never removed
 * if instance source or destination of xmr never removed 
 * 
 * this uses n_ispthsrc flag since deletable wires never I/O ports
 * also only I/O ports can be deleted so since I/O ports not deletable
 * will never see here 
 * this builds nqc_u (nu2) for keepping track of fan-in/fan-out but always
 * freed when done
 */
static void eat_gates(void)
{
 register struct net_t *np;
 register int32 pi, ni;
 register struct tfrec_t *tfrp;
 int32 some_eaten, first_time, wlen;
 word32 *wp;
 struct mod_t *mdp;
 struct tfarg_t *tfap;

 /* first mark all wires in design that are undeletable */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mnnum == 0) continue;

   __push_wrkitstk(mdp, 0);
   /* this sets n_mark for all nets not candidates for deletion */
   for (ni = 0, np = &(__inst_mod->mnets[0]); ni < __inst_mod->mnnum;
    ni++, np++)
    {
     np->n_mark = FALSE;

     /* cannot delete i/o ports or arrays and must be wire to delete */
     /* temporary n_mark indicates not deletable */
     /* never delete wire in tran channel really has drivers and loads */

     if (np->ntyp >= NONWIRE_ST || np->iotyp != NON_IO || np->ntraux != NULL)
      { np->n_mark = TRUE; continue; }

     /* if only drivers or loads but has must stay npp's cannot delete */
     /* cannot delete if xmr or IS form */
     if (np->ndrvs != NULL && has_muststay_npp(np->ndrvs))
      { np->n_mark = TRUE; continue; }
     if (np->nlds != NULL && has_muststay_npp(np->nlds))
      { np->n_mark = TRUE; continue; }
    }
   __pop_wrkitstk();
  }
 /* also mark all wires that connect to tf_ args - list is design wide */
 /* even if only used in rhs connection */
 for (tfrp = __tfrec_hdr; tfrp != NULL; tfrp = tfrp->tfrnxt)
  {
   for (pi = 1; pi < tfrp->tfanump1; pi++)
    {
     tfap = &(tfrp->tfargs[pi]);
     mark_muststay_wires(tfap->arg.axp);
    }
  }

 /* next do deleting module by module */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   __push_wrkitstk(mdp, 0);

   some_eaten = TRUE;
   first_time = TRUE;
   while (some_eaten)
    {
     /* notice only deleted cells can allow new net deletions */
     eat_nets(first_time);
     first_time = FALSE;
     eat_cells(&some_eaten);
    } 
   /* this module done free any qcval fields */
   /* if any qc forms, will be rebuilt and used for different purpose */
   if (__inst_mod->mnnum != 0)
    {
     for (ni = 0, np = &(__inst_mod->mnets[0]); ni < __inst_mod->mnnum;
      ni++, np++)
      {
       /* turn mark off */
       np->n_mark = FALSE;
       if (np->nu2.wp != NULL)
        {
         wlen = 2*wlen_(np->nwid);
         wp = np->nu2.wp;
         __my_free((char *) wp, wlen*WRDBYTES);
         np->nu2.wp = NULL;
        }
      }
    }
   __pop_wrkitstk();
  }
}

/*
 * return T if has some kind of net pin that cannot be deleted
 * 1) if IS form undeletable 
 * 2) if net pin is xmr then net it is on cannot be deleted
 *
 * also) if has I/O port connection, but should never see here
 */
static int32 has_muststay_npp(register struct net_pin_t *npp)
{
 struct npaux_t *npauxp;

 for (; npp != NULL; npp = npp->npnxt)
  {
   if (npp->np_xmrtyp != XNP_LOC) return(TRUE);
   if ((npauxp = npp->npaux) != NULL && npauxp->nbi1 == -2) return(TRUE); 
   /* --- DBG remove */
   if (npp->npntyp == NP_MDPRT || npp->npntyp == NP_PB_MDPRT)
    __misc_terr(__FILE__, __LINE__);
  }
 return(FALSE);
}

/*
 * mark wires that must remain uneaten because connect to tf_ arg
 */
static void mark_muststay_wires(struct expr_t *xp)
{
 struct net_t *np;

 if (__isleaf(xp))
  {
   if (xp->optyp == ID || xp->optyp == GLBREF)
    {
     np = xp->lu.sy->el.enp;
     if (np->ntyp < NONWIRE_ST) np->n_mark = TRUE;
    }
   return;
  }
 if (xp->lu.x != NULL) mark_muststay_wires(xp->lu.x);
 if (xp->ru.x != NULL) mark_muststay_wires(xp->ru.x);
}

/*
 * remove net and npp's for unc. bits 
 * know only nets marked n_ispthsrc (tmp. flag) 
 * tri0/tri1 implied driver nets always have fi = 1
 * 
 */
static void eat_nets(int32 first_time)
{
 register int32 ni, wi;
 register struct net_t *np;
 int32 nfi, nfo, impl_drv, wlen;
 word32 *wpfi, *wpfo; 

 if (__inst_mod->mnnum == 0) return;
 if (!first_time) rem_del_npps();
 for (ni = 0, np = &(__inst_mod->mnets[0]); ni < __inst_mod->mnnum; ni++, np++)
  {
   if (np->n_mark || np->n_gone) continue;

   impl_drv = wire_implied_driver(np);
   /* assume has fi and fan out */
   nfi = nfo = 1;
   if (!np->n_isavec)
    {
     /* any type of driver including tri0/1 implied prevents deletion */
     /* wire used in any rhs (i.e. procedural) has loads even if no wire */
     /* loads that require event propagation */  
     if (np->nlds == NULL && !np->n_onprocrhs) nfo = 0;
     if (np->ndrvs == NULL && !impl_drv) nfi = 0; 
     if (nfo != 0 && nfi != 0) continue;

do_delete:
     if (nfo == 0 && nfi == 0)
      __gfinform(447, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
       "net %s in %s disconnected: no drivers, no loads and no procedural connections",
       np->nsym->synam, __inst_mod->msym->synam);
     else if (nfi == 0)
      __gfinform(447, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
       "net %s in %s disconnected: no drivers", np->nsym->synam,
       __inst_mod->msym->synam);
     else 
      __gfinform(447, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
       "net %s in %s disconnected: no loads and no procedural connections",
       np->nsym->synam, __inst_mod->msym->synam);
del_done:
     np->n_gone = TRUE;
     remove_all_npps(np);
     __flnets_removable += __inst_mod->flatinum;
     __nets_removable++;
     continue; 
    }
   /* if vector no loads (or proc rhs conns) or drivers can just delete */
   if (np->nlds == NULL && !np->n_onprocrhs) nfo = 0;
   if (np->ndrvs == NULL && !impl_drv) nfi = 0;
   if (nfi == 0 || nfo == 0)
    {
     /* in case last npp removed, mark processed */
     /* here know non i/o that can be deleted is never path source so use */
     /* as temporary need to delete this time flag */
     np->n_isapthsrc = FALSE;
     goto do_delete; 
    }

   /* if npp list changed or not yet built, must build */ 
   if ((wpfi = np->nu2.wp) == NULL || np->n_isapthsrc)
    {
     bld1vec_fifo(np);
     np->n_isapthsrc = FALSE;
     /* make sure wpfi set from nu2 wp union member */
     wpfi = (word32 *) np->nu2.wp;
    }

   /* if at least one driver and used on rhs cannot delete */
   /* but still need per bit fifo vec since if gate connecting bit */
   /* has no fi and no fo even though has proc. connection gate del. gate */
   if (np->ndrvs != NULL && np->n_onprocrhs) goto nxt_net;

   /* must check bit by bit - possible for some bits have fi others fo */  
   wlen = wlen_(np->nwid);
   wpfo = &(wpfi[wlen]);
   for (wi = 0; wi < wlen; wi++)
    {
     /* if even 1 bit fi/fo cannot delete net - but maybe still cells */
     if ((wpfi[wi] & wpfo[wi]) != 0) goto nxt_net;
    }  
   __gfinform(447, np->nsym->syfnam_ind, np->nsym->sylin_cnt,
    "net %s disconnected: no procedural connections and no bit has both drivers and loads",
    np->nsym->synam);
   goto del_done;

nxt_net:;
  }
}

/*
 * routine to remove all marked (n_isapthsrc is tmp flag)
 * this routine is called only after eat cells (not first time)
 * error if at least one not deleted
 * only called if has nets
 */
static void rem_del_npps(void)
{
 register int32 ni;
 register struct net_t *np;
 register struct net_pin_t *npp;
 struct net_pin_t *npp2, *last_npp;
 int32 net_chged;

 for (ni = 0, np = &(__inst_mod->mnets[0]); ni < __inst_mod->mnnum; ni++, np++)
  {
   if (np->n_mark || !np->n_isapthsrc || np->n_gone) continue;

   /* first remove net pin elements connected to removed cells */ 
   for (net_chged = FALSE, last_npp = NULL, npp = np->ndrvs; npp != NULL;)
    {
     switch ((byte) npp->npntyp) {
      case NP_GATE:
       /* know this is at most 1 wire and net pin element no xmr field */ 
       if (!npp->elnpp.egp->g_gone) break;
do_gate_npp_del:
       if (last_npp == NULL) np->ndrvs = npp->npnxt;
       else last_npp->npnxt = npp->npnxt;
       npp2 = npp->npnxt;      
       __my_free((char *) npp, sizeof(struct net_pin_t));  
       npp = npp2;
       net_chged = TRUE;
       continue;
      case NP_CONTA:
       if (npp->elnpp.ecap->ca_gone) goto do_gate_npp_del;
       break;
     }
     last_npp = npp;
     npp = npp->npnxt; 
    }
   for (last_npp = NULL, npp = np->nlds; npp != NULL;)
    {
     switch ((byte) npp->npntyp) {
      case NP_GATE:
       /* know this is at most 1 wire and net pin element no xmr field */ 
       if (!npp->elnpp.egp->g_gone) break;
do_npp_del:
       if (last_npp == NULL) np->nlds = npp->npnxt;
       else last_npp->npnxt = npp->npnxt;
       npp2 = npp->npnxt;      
       __my_free((char *) npp, sizeof(struct net_pin_t));  
       npp = npp2;
       net_chged = TRUE;
       continue;
      case NP_CONTA:
       if (npp->elnpp.ecap->ca_gone) goto do_npp_del;
       break;
       /* tf form should only be driver */
     }
     last_npp = npp;
     npp = npp->npnxt; 
    }
   /* no net pin list change - do not rebuild the per bit table */
   if (!net_chged) np->n_isapthsrc = FALSE;
  }
}

/* 
 * when net deleted, remove all npps 
 */
static void remove_all_npps(struct net_t *np)
{
 register struct net_pin_t *npp;
 struct net_pin_t *npp2;

 for (npp = np->ndrvs; npp != NULL;)
  {
   npp2 = npp->npnxt;      
   __my_free((char *) npp, sizeof(struct net_pin_t));  
   npp = npp2;
  }
 np->ndrvs = NULL;
 for (npp = np->nlds; npp != NULL;)
  {
   npp2 = npp->npnxt;      
   __my_free((char *) npp, sizeof(struct net_pin_t));  
   npp = npp2;
  }
 np->nlds = NULL;
}

/*
 * build (rebuild) the per bit fi/fo table
 * sets nfi/nfo to 0 if entire wire unc. and 1 if at least one bit has conn.
 * for implied driver nets, set to 1 even if no "real" drivers
 */
static void bld1vec_fifo(struct net_t *np)
{
 int32 *pbfi, *pbfo, *pbtcfo, wlen;
 word32 *wp;

 /* first try simple case */
 pbfi = (int32 *) __my_malloc(sizeof(int32)*np->nwid);
 pbfo = (int32 *) __my_malloc(sizeof(int32)*np->nwid);
 pbtcfo = (int32 *) __my_malloc(sizeof(int32)*np->nwid);
 /* inst. number not used here since never remove net iwth IS form */
 __bld_pb_fifo(np, pbfi, pbfo, pbtcfo, 0);
 wp = np->nu2.wp;
 if (wp == NULL)
  {
   wlen = 2*wlen_(np->nwid);
   wp = (word32 *) __my_malloc(wlen*WRDBYTES);
   memset(wp, 0, wlen*WRDBYTES);
   np->nu2.wp = wp;
  }
 update_vec_fifo(np, wp, pbfi, pbfo, pbtcfo);
 __my_free((char *) pbfi, sizeof(int32)*np->nwid);
 __my_free((char *) pbfo, sizeof(int32)*np->nwid);
 __my_free((char *) pbtcfo, sizeof(int32)*np->nwid);
}

/*
 * set per bit fi/fo values in net working fi/fo ares
 * tri0/tri1 implied driver net has has fi 1
 * if wire bit drivers timing check then still has driver and must stay
 */
static void update_vec_fifo(struct net_t *np, word32 *wpfi, int32 *pbfi, 
 int32 *pbfo, int32 *pbtcfo)
{
 register int32 bi;
 register int32 wi, biti;
 int32 wlen, implied_drv;
 word32 *wpfo;

 wlen = wlen_(np->nwid);
 wpfo = &(wpfi[wlen]);
 implied_drv = wire_implied_driver(np);
 for (bi = 0; bi < np->nwid; bi++)
  {
   wi = get_wofs_(bi);
   biti = get_bofs_(bi);
   if (pbfi[bi] == 0 && !implied_drv) wpfi[wi] &= ~(1L << biti);
   else wpfi[wi] |= (1L << biti);
   /* know 2 bits never cross word32 boundary */
   if (pbfo[bi] == 0 && pbtcfo[bi] == 0) wpfo[wi] &= ~(1L << biti);
   else wpfo[wi] |= (1L << biti);
  }
}

/*
 * return T if wire is implied driver type
 */
static int32 wire_implied_driver(struct net_t *np) 
{
 switch ((byte) np->ntyp) {
  case N_TRI0: case N_TRI1: case N_TRIREG: case N_SUPPLY0: case N_SUPPLY1:
   return(TRUE);
 }
 return(FALSE);
}

/*
 * delete cells (gate and non xmr source/destination module instances
 * only gate and continuous assignments are removed
 */
static void eat_cells(int32 *some_eaten)
{
 register int32 i, gi;
 register struct gate_t *gp;
 register struct conta_t *cap;
 int32 cai, out_unconn, ins_unconn;

 /* go through module gates tryng to remove (disconnect) */
 *some_eaten = FALSE;
 for (gi = 0; gi < __inst_mod->mgnum; gi++)
  {
   gp = &(__inst_mod->mgates[gi]);
   if (gp->g_gone) continue;

   /* can never delete pullup or pulldown */
   out_unconn = ins_unconn = FALSE;
   if (gp->g_class == GC_PULL) continue;
   /* for trans, in tran channels so do not process here */
   if (gp->g_class == GC_TRAN || gp->g_class == GC_TRANIF) continue;

   /* if output not gone, see if all inputs gone */
   if (conn_expr_gone(gp->gpins[0])) out_unconn = TRUE;
   for (i = 1; i < (int32) gp->gpnum; i++)
    { if (!conn_expr_gone(gp->gpins[i])) goto try_delete; }
   ins_unconn = TRUE;
   /* mark the gate deleted */
try_delete:
   if (ins_unconn || out_unconn)
    {
     if (ins_unconn && out_unconn)
      __gfinform(448, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
       "%s gate %s deleteable because all ports unconnected",
       gp->gmsym->synam, gp->gsym->synam);
     else if (out_unconn)
      __gfinform(448, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
       "%s gate %s deleteable because output unconnected",
       gp->gmsym->synam, gp->gsym->synam);
     else 
      __gfinform(448, gp->gsym->syfnam_ind, gp->gsym->sylin_cnt,
       "%s gate %s deleteable because all inputs unconnected",
       gp->gmsym->synam, gp->gsym->synam);
     gp->g_gone = TRUE;
     /* gate is gone - mark all nets not already deleted for fi-fo re-eval */
     for (i = 1; i < (int32) gp->gpnum; i++)
      mark_maybe_gone_nets(gp->gpins[i]);
     *some_eaten = TRUE;
     __flgates_removable += __inst_mod->flatinum;
     __gates_removable++;
    }
  }
 /* no module removable - better to first use some kind of inlining */
 /* and then remove gates from that if possible */

 /* try to remove (disconnect) continuous assigns */ 
 /* go through module gates tryng to remove (disconnect) */
 for (cap = __inst_mod->mcas, cai = 0; cai < __inst_mod->mcanum; cai++, cap++)
  {
   if (cap->ca_gone) continue;

   /* can never delete pullup or pulldown */
   out_unconn = ins_unconn = FALSE;  
   /* if output not gone, see if all inputs gone */
   if (conn_expr_gone(cap->lhsx)) out_unconn = TRUE;
   if (conn_expr_gone(cap->rhsx)) ins_unconn = TRUE;
   if (ins_unconn || out_unconn)
    {
     if (ins_unconn && out_unconn)
      __gfinform(448, cap->casym->syfnam_ind, cap->casym->sylin_cnt,
       "continuous assign deleteable because it has no connections");
     else if (out_unconn)
      __gfinform(448, cap->casym->syfnam_ind, cap->casym->sylin_cnt,
       "continuous assign deletable because left hand side drives nothing");
     else 
      __gfinform(448, cap->casym->syfnam_ind, cap->casym->sylin_cnt,
       "continuous assign deletable because right hand side unconnected");
     cap->ca_gone = TRUE;
     mark_maybe_gone_nets(cap->lhsx);
     mark_maybe_gone_nets(cap->rhsx);
     *some_eaten = TRUE;
     __flcontas_removable += __inst_mod->flatinum;
     __contas_removable++;
    }
  }
}

/*
 * return T if expression is removable (no connectons) 
 * notice constants cannot be deleted
 *
 * also n_mark set for undeletable nets that do not have fi-fo table
 * know if n_mark off and not already deleted, know fi-fo table exists
 */
static int32 conn_expr_gone(struct expr_t *xp)
{
 register int32 bi;
 int32 bi2, nfi, nfo;
 struct net_t *np;
 struct expr_t *ndx, *ndx2; 

 switch ((byte) xp->optyp) {
  case ID:
   np = xp->lu.sy->el.enp; 
   if (!np->n_gone) return(FALSE);
   break;
  case OPEMPTY: break;
  case LSB: 
   np = xp->lu.x->lu.sy->el.enp; 
   if (!np->n_gone) 
    {
     if (np->n_mark) return(FALSE);
     ndx = xp->ru.x; 
     if (ndx->optyp == NUMBER)
      {
       bi = __comp_ndx(np, ndx);
       if (bi == -1) return(FALSE);  
       getbit_fifo(np, bi, &nfi, &nfo);
       if (nfi == 0 || (nfo == 0 && !np->n_onprocrhs)) break;
      }
     return(FALSE);
    }
   break;
  case PARTSEL:
   np = xp->lu.x->lu.sy->el.enp; 
   if (!np->n_gone) 
    {
     if (np->n_mark) return(FALSE);
     ndx = xp->ru.x->lu.x;
     if ((bi = __comp_ndx(np, ndx)) == -1) return(FALSE);
     ndx2 = xp->ru.x->ru.x;
     if ((bi2 = __comp_ndx(np, ndx2)) == -1) return(FALSE);
     for (; bi >= bi2; bi--)
      {
       getbit_fifo(np, bi, &nfi, &nfo);
       if (nfi != 0 && (nfo != 0 || np->n_onprocrhs)) return(FALSE);
      }
     /* fall thru, all bits of part select unc. */
    }
   break;
  case LCB:
   /* for concatenate all components must be gone */
   for (ndx2 = xp->ru.x; ndx2 != NULL; ndx2 = ndx2->ru.x)
    { if (!conn_expr_gone(ndx2->lu.x)) return(FALSE); }
   break;
  /* anything else (such as function or arithmetic expr. must stay */
  default: return(FALSE);
 }
 return(TRUE);
}

/*
 * set n_isapthsrc to indicate net can be deleted and must have nlds and
 * ndrvs updated and maybe removed
 *
 * must mark all nets in expr.
 * notice constants cannot be deleted
 */
static void mark_maybe_gone_nets(struct expr_t *xp)
{
 struct net_t *np;
 struct expr_t *ndx2;

 switch ((byte) xp->optyp) {
  case ID:
   np = xp->lu.sy->el.enp; 
   if (!np->n_gone) np->n_isapthsrc = TRUE;
   break;
  case OPEMPTY: break;
  case LSB: case PARTSEL:
   np = xp->lu.x->lu.sy->el.enp; 
   if (!np->n_gone) np->n_isapthsrc = TRUE;
   break;
  case LCB:
   /* for concatenate all components must be gone */
   for (ndx2 = xp->ru.x; ndx2 != NULL; ndx2 = ndx2->ru.x)
    mark_maybe_gone_nets(ndx2->lu.x);
   break;
 }
}

/*
 * the the fi and fo for 1 bit
 */
static void getbit_fifo(struct net_t *np, int32 bi, int32 *nfi, int32 *nfo)
{
 int32 wi, biti, wlen;
 word32 *wpfi, *wpfo;

 /* must check the bit if constant */
 wi = get_wofs_(bi);
 biti = get_bofs_(bi);
 /* DBG remove */
 if ((wpfi = (word32 *) np->nu2.wp) == NULL) __misc_terr(__FILE__, __LINE__);
 wlen = wlen_(np->nwid);
 wpfo = &(wpfi[wlen]);
 *nfi = ((wpfi[wi] & (1L << biti)) != 0L) ? 1 : 0;
 *nfo = ((wpfo[wi] & (1L << biti)) != 0L) ? 1 : 0;
}

/*
 * ROUTINES TO DETERMINE IF GATE CAN BE OPTIMIZED (USED ACC ROUTINE)
 */

/*
 * return T if gate is acceleratable buf or not
 * accelerate classes are 0 std non accel., 2 buf/not, 3 acc. logic
 *
 * will not accelerate if: 1) >3 inputs, 2) drives strength, 3) 1 in and style 
 * 4) drives fi>1 wire, 5) output is not scalar or constant bit select,
 * 6) inputs not accelerable
 *
 * notice whether or not has delay does effect acc class
 */
extern int32 __get_acc_class(struct gate_t *gp)
{
 register int32 gi;
 int32 acc_class, st_on_input, wire_pthdel;
 word32 gatid;
 struct expr_t *xp;
 struct net_t *np;

 acc_class = ACC_NONE;
 st_on_input = FALSE;
 /* can have up to 3 inputs and 1 output for acceleration - fits in byte */
 if (gp->g_hasst || gp->gpnum > 4) return(ACC_STD);
 gatid = gp->gmsym->el.eprimp->gateid;
 switch ((byte) gatid) { 
  case G_NOT: case G_BUF: case G_ASSIGN: acc_class = ACC_BUFNOT; break; 
  case G_NAND: case G_BITREDAND: case G_BITREDOR:
  case G_NOR: case G_BITREDXOR: case G_REDXNOR:
   /* never accelerate degenerate 2 input gates that are usually >=3 in */
   if (gp->gpnum == 2) return(ACC_STD);
   acc_class = ACC_4IGATE;
   break;
  default: return(ACC_STD);
 }  
 /* only get here for logic gates - mos, tran, etc. always std */
 /* output must be simple wire or constant bsel from wire */
 xp = gp->gpins[0];
 /* cannot be fi>1 and know if in tran channel will be fi>1 */
 if (xp->x_multfi) return(ACC_STD);
 if (xp->optyp == ID)
  {
   np = xp->lu.sy->el.enp; 
   if (np->n_isavec || np->n_stren) return(ACC_STD);
  }
 else if (xp->optyp == LSB)
  {
   if (xp->ru.x->optyp != NUMBER) return(ACC_STD);
   np = xp->lu.x->lu.sy->el.enp; 
   if (np->n_stren) return(ACC_STD);
  }
 else return(ACC_STD);
 /* DBG remove */
 if (np->ntraux != NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */
 if (np->nrngrep == NX_DWIR) wire_pthdel = TRUE; else wire_pthdel = FALSE;

 /* all inputs must be XL accelerateable */
 for (st_on_input = FALSE, gi = 0; gi < (int32) gp->gpnum; gi++)
  {
   xp = gp->gpins[gi];
   if (xp->optyp == ID)
    {
     np = xp->lu.sy->el.enp;
     if (np->n_isavec) return(ACC_STD);
     if (np->n_stren) st_on_input = TRUE; 
    }
   /* strength on bit select ok, slower does not need separate case */
   else if (xp->optyp == LSB)
    {
     if (xp->ru.x->optyp != NUMBER) return(ACC_STD);
     /* SJM 05/17/03 - array (memory) selects on input must not accelerate */
     if (xp->lu.x->lu.sy->el.enp->n_isarr) return(ACC_STD);
    }
   else return(ACC_STD);
  }
 if (st_on_input)
  {
   if (acc_class == ACC_BUFNOT) acc_class = ACC_STIBUFNOT;
   else if (acc_class == ACC_4IGATE) acc_class = ACC_ST4IGATE;
  }
 gp->g_pdst = wire_pthdel;
 return(acc_class);
}

/*
 * ROUTINES TO CONVERT FROM DT_NONE TO #0 FOR DELAY ANNOTATION
 */

/*
 * add delay (set to #0) to gate
 *
 * know called after prep
 */
extern int32 __add_gate_pnd0del(struct gate_t *gp, struct mod_t *mdp,
 char *sdfmsg)
{
 register int32 gsi;
 int32 nbytes;
 struct net_t *np;
 struct primtab_t *ptp;
 struct expr_t *xp;

 ptp = gp->gmsym->el.eprimp;
 /* make sure gate can have delay */
 if (ptp->gateid == G_TRAN || ptp->gateid == G_RTRAN)
  {
   /* rnotice error here will stop simulation */
   if (sdfmsg != NULL)
    __pv_ferr(1199, 
     "%s delay annotate to %s gate %s for which delay illegal", sdfmsg,
     gp->gmsym->synam, gp->gsym->synam);
   else __vpi_err(1893, vpiError,
    "vpi_put_delays illegal for %s gate %s for which delay illegal",
    gp->gmsym->synam, gp->gsym->synam);
   return(FALSE);
  }
 /* set delay to pnd 0 */
 gp->g_du.d1v = (word64 *) __my_malloc(sizeof(word64));
 gp->g_du.d1v[0] = 0ULL;
 gp->g_delrep = DT_1V;

 /* allocate and initialize the inertial pending schd event table */
 nbytes = mdp->flatinum*sizeof(i_tev_ndx); 
 gp->schd_tevs = (i_tev_ndx *) __my_malloc(nbytes);
 for (gsi = 0; gsi < mdp->flatinum; gsi++) gp->schd_tevs[gsi] = -1;

 /* if accelerated set g resist if driven wire has delay or path dest */
 if (__gate_is_acc(gp))
  {
   xp = gp->gpins[0];
   if (xp->optyp == ID) np = xp->lu.sy->el.enp; 
   else if (xp->optyp == LSB) np = xp->lu.x->lu.sy->el.enp; 
   else { np = NULL;  __case_terr(__FILE__, __LINE__); }
   if (np->nrngrep == NX_DWIR) gp->g_pdst = TRUE; 
  }
 return(TRUE);
}

/*
 * add conta delay (add drivers and internal conta value too) (set to #0)
 *
 * know called after prep and know no delay 
 * SJM 09/29/02 - if per bit this must set master delay and PB drv/schd
 */
extern int32 __add_conta_pnd0del(struct conta_t *cap, struct mod_t *mdp,
 char *sdfmsg)
{
 register int32 i, bi;
 struct conta_t *pbcap;

 /* make sure gate can have delay */
 if (cap->lhsx->getpatlhs)
  {
   /* rnotice error here will stop simulation */
   __bld_lineloc(__xs, cap->casym->syfnam_ind, cap->casym->sylin_cnt);
   if (sdfmsg != NULL)
    __pv_ferr(1379, 
     "%s delay annotate to $getpattern continuous assign at %s illegal",
     sdfmsg, __xs);
   else __vpi_err(1905, vpiError,
     "vpi_put_delays illegal for $getpattern continuous assign at %s", __xs);
   return(FALSE);
  }
 
 if (!cap->ca_pb_sim)
  {
   /* since before any sim or new PLI added del, initialized value correct */
   if (!cap->lhsx->x_multfi)
    {
     /* DBG remove --- */
     if (cap->ca_drv_wp.wp != NULL) __misc_terr(__FILE__, __LINE__);
     /* --- */
     __allocinit_perival(&(cap->ca_drv_wp), mdp->flatinum,
      cap->lhsx->szu.xclen, TRUE);
    }
   /* DBG remove --- */
   if (cap->schd_drv_wp.wp != NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */
   /* nothing will be scheduled and this will not be allocated */
   __allocinit_perival(&(cap->schd_drv_wp), mdp->flatinum,
   cap->lhsx->szu.xclen, TRUE);
   cap->caschd_tevs = (i_tev_ndx *)
    __my_malloc(mdp->flatinum*sizeof(i_tev_ndx));
   for (i = 0; i < mdp->flatinum; i++) cap->caschd_tevs[i] = -1;
  }
 else
  {
   for (bi = 0; bi < cap->lhsx->szu.xclen; bi++)
    {
     pbcap = &(cap->pbcau.pbcaps[bi]);
     /* DBG remove */
     if (pbcap->lhsx->szu.xclen != 1) __misc_terr(__FILE__, __LINE__);
     /* --- */

     /* since before any sim or new PLI added del, init value correct */
     /* SJM 09/28/02 - if master cat expr fi>1, need all per bits */
     if (!cap->lhsx->x_multfi)
      {
       /* DBG remove --- */
       if (pbcap->ca_drv_wp.wp != NULL) __misc_terr(__FILE__, __LINE__);
       /* --- */
       __allocinit_perival(&(pbcap->ca_drv_wp), mdp->flatinum, 1, TRUE);
      }
     /* DBG remove --- */
     if (pbcap->schd_drv_wp.wp != NULL) __misc_terr(__FILE__, __LINE__);
     /* --- */
     /* nothing will be scheduled and this will not be allocated */
     __allocinit_perival(&(pbcap->schd_drv_wp), mdp->flatinum, 1, TRUE);
     pbcap->caschd_tevs = (i_tev_ndx *)
      __my_malloc(mdp->flatinum*sizeof(i_tev_ndx));
     for (i = 0; i < mdp->flatinum; i++) pbcap->caschd_tevs[i] = -1;
    } 
  }

 /* SJM 09/28/02 - delay always in master never per bit */
 /* set delay to pnd 0 */
 cap->ca_du.d1v = (word64 *) __my_malloc(sizeof(word64));
 cap->ca_du.d1v[0] = 0ULL;
 cap->ca_delrep = DT_1V;

 return(TRUE);
}

/*
 * ROUTINES TO REMOVE ALL 0 DELAY PATHS
 */

/*
 * remove 0 path delays
 *
 * all this does is link out npp - rest of d.s. small and interlinked
 * only remove source tchg npps if not part of other non removed path
 *
 * FIXME - for now not removing source changes - set ref. count but not using
 * will do extra work in recording each path source change but no path 
 * processing because destinations not path dests any more
 */
extern void __rem_0path_dels(void)
{
 register int32 pi;
 register struct net_pin_t *npp;
 struct spcpth_t *pthp; 
 int32 has_0del, dbi, dbi2, ni;
 int32 num_pthrem_mods, num_pthrems, num_flat_pthrems;
 int32 num_pthrem_nets, num_flat_pthrem_nets;
 struct mod_t *mdp;
 struct pathel_t *spep, *dpep;
 struct npaux_t *npauxp;
 struct net_t *snp, *dnp;
 struct pthdst_t *pdp, *pdp2, *last_pdp; 
 struct rngdwir_t *dwirp; 
 struct expr_t *lhsx;

 num_pthrem_mods =  num_pthrems = num_flat_pthrems = 0;
 num_pthrem_nets = num_flat_pthrem_nets = 0;
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mspfy == NULL || mdp->mspfy->spcpths == NULL) continue;

   /* know each delay is NUMBER or REALNUM */ 
   has_0del = FALSE;
   for (pthp = mdp->mspfy->spcpths; pthp != NULL; pthp = pthp->spcpthnxt)
    {
     if (pthp->pth_gone) continue;

     if (__chk_0del(pthp->pth_delrep, pthp->pth_du, mdp) == DBAD_0) 
      {
       pthp->pth_0del_rem = TRUE;
       num_pthrem_mods++;
       has_0del = TRUE;
      } 
    }
   if (!has_0del) continue;

   /* know at least one path must be removed */
   /* step 1: set all path src ref counts - >1 dests for one src possible */
   for (pthp = mdp->mspfy->spcpths; pthp != NULL; pthp = pthp->spcpthnxt)
    {
     /* think impossible for some paths to be gone and make it to prep */   
     if (pthp->pth_gone) continue;

     for (pi = 0; pi <= pthp->last_pein; pi++)
      {
       spep = &(pthp->peins[pi]);
       snp = spep->penp;
       for (npp = snp->nlds; npp != NULL; npp = npp->npnxt) 
        {
         if (npp->npntyp != NP_TCHG || npp->chgsubtyp != NPCHG_PTHSRC)
          continue;
         /* know tchg sources always per bit - for scalar 0 */

         /* path source npp's always one bit, -1 only if scalar */
         if ((npauxp = npp->npaux) != NULL) dbi = npauxp->nbi1;
         else dbi = -1;
         if (spep->pthi1 == -1 || (dbi <= spep->pthi1 && dbi >= spep->pthi2)) 
          {
           /* inc ref. count - know value previously inited to 0 */
           (npp->elnpp.etchgp->lastchg[0])++; 
          }
        }
      }
    } 
   /* step 2: actually remove path dst npp's and decr. pstchg source counts */
   for (pthp = mdp->mspfy->spcpths; pthp != NULL; pthp = pthp->spcpthnxt)
    {
     if (!pthp->pth_0del_rem) continue;
     
     num_pthrems++;
     num_flat_pthrems += mdp->flatinum;
  
     for (pi = 0; pi <= pthp->last_peout; pi++)
      {
       dpep = &(pthp->peouts[pi]);
       dnp = dpep->penp;
  
       for (npp = dnp->nlds; npp != NULL; npp = npp->npnxt) 
        {
         if (npp->npntyp != NP_TCHG || npp->chgsubtyp != NPCHG_PTHSRC)
          continue;
         if (!dnp->n_isavec) dbi = dbi2 = 0; 
         else
          {
           if (dpep->pthi1 == -1) { dbi = dnp->nwid - 1; dbi2 = 0; }  
           else { dbi = dpep->pthi1; dbi2 = dpep->pthi2; }
          }
  
         /* for every bit of destination path element */
         dwirp = dnp->nu.rngdwir;
         for (; dbi >= dbi2; dbi--)
          {
           last_pdp = NULL;
           for (pdp = dwirp->n_du.pb_pthdst[dbi]; pdp != NULL;)
            {
             pdp2 = pdp->pdnxt;
             if (pdp->pstchgp->chgu.chgpthp == pthp)
              {
               if (last_pdp == NULL) dwirp->n_du.pb_pthdst[dbi] = pdp->pdnxt; 
               else last_pdp->pdnxt = pdp->pdnxt;
               __my_free((char *) pdp, sizeof(struct pthdst_t)); 
              }
             else last_pdp = pdp;
             pdp = pdp2; 
            }
          }
        }
      }
    }

   /* for path dest. nets with all pthdst removed - set as non path dest. */
   for (ni = 0, dnp = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, dnp++) 
    {
     if (!dnp->n_isapthdst) continue;

     dwirp = dnp->nu.rngdwir;
     if (!dnp->n_isavec)
      {
       if (dwirp->n_du.pb_pthdst[0] != NULL) continue;
      }
     else
      {
       for (dbi = dnp->nwid - 1; dbi >= 0; dbi--)
        {
         if (dwirp->n_du.pb_pthdst[dbi] != NULL) continue;
        }
      } 
     /* convert to non path dest. */
     __my_free((char *) dwirp->n_du.pb_pthdst,
      dnp->nwid*sizeof(struct pthdst_t *));
     dwirp->n_du.d1v = NULL;
     dwirp->n_delrep = DT_NONE;
     dnp->n_isapthdst = FALSE;
     num_pthrem_nets++;
     num_flat_pthrem_nets += mdp->flatinum;
     /* SJM 07/23/03 - must turn off all lhsx ndel marks for completely */
     /* removed paths where driver is gate out or conta */
     for (npp = dnp->ndrvs; npp != NULL; npp = npp->npnxt) 
      {
       if (npp->npntyp == NP_GATE) lhsx = npp->elnpp.egp->gpins[0];
       else if (npp->npntyp == NP_CONTA) lhsx = npp->elnpp.ecap->lhsx; 
       else continue;
  
       /* DBG remove */
       if (!lhsx->lhsx_ndel) __misc_terr(__FILE__, __LINE__);
       /* -- */
       if (lhsx->lhsx_ndel) lhsx->lhsx_ndel = FALSE;
      }
    }

   /* step 3: reset lastchg field and remove if needed - no longer ref cnt */
   for (pthp = mdp->mspfy->spcpths; pthp != NULL; pthp = pthp->spcpthnxt)
    {
     if (pthp->pth_gone) continue;

     for (pi = 0; pi <= pthp->last_pein; pi++)
      {
       spep = &(pthp->peins[pi]);
       snp = spep->penp;
       for (npp = snp->nlds; npp != NULL; npp = npp->npnxt) 
        {
         if (npp->npntyp != NP_TCHG || npp->chgsubtyp != NPCHG_PTHSRC)
          continue;
         /* path source npp's always one bit, -1 only if scalar */
         if ((npauxp = npp->npaux) != NULL) dbi = npauxp->nbi1;
         else dbi = -1;
         if (spep->pthi1 == -1 || (dbi <= spep->pthi1 && dbi >= spep->pthi2)) 
          {
           /* reset ref. count */
           npp->elnpp.etchgp->lastchg[0] = 0ULL; 
          }
        }
      }
    } 
  }
 if (num_pthrem_mods != 0 && __verbose) 
  {
   __cv_msg(
    "  %d zero delay paths (%d flat) in %d types removed - no effect.\n",
    num_pthrems, num_flat_pthrems, num_pthrem_mods);
  }
 if (num_pthrem_nets != 0 && __verbose)
  {
   __cv_msg(
    "  %d nets (%d flat) no longer path destinations (all paths zero delay).\n",
    num_pthrem_nets, num_flat_pthrem_nets);
  }
}
