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
 * MODULE TO HANDLE TRAN CHANNELS - PREPARATION AND ASSIGNMENT
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
static void free_vtx_and_edge_freelsts(void);
static void emit_chan_size_table(void);
static void bld_trauxs(void);
static void alloc_traux(struct mod_t *, struct net_t *);
static void bld_trgraph(struct mod_t *, struct traux_t *, struct net_t *);
static void init_chanrec(struct chanrec_t *);
static struct vbinfo_t *add_vtxtraux(struct net_t *, int32, int32);
static void add_1chan_vtxes_and_edges(int32);
static struct vtx_t *alloc_vtx(struct net_t *, int32);
static void add_vtx_to_future_q(struct vtx_t *, struct itree_t *itp);
static void add_edges_from_npps(struct vtx_t *, int32, struct itree_t *);
static int32 add1_oside_edge(struct vtx_t *, struct net_pin_t *, struct net_t *,
 int32, int32, struct itree_t *, struct itree_t *);
static word32 cnvt_tobase_ntyp(word32);
static void chg_bidchan_to_vtxlist(struct chanrec_t *);
static void process_bid_vertices(struct chanrec_t *, struct vtx_t *,
 struct itree_t *);
static void add_vtx_to_bidvtxlist(struct chanrec_t *, struct vtx_t *,
 struct itree_t *);
static struct edge_t *alloc_edge(struct net_pin_t *);
static void chkchannel(struct chanrec_t *);
static void off_bichan_marks(struct chanrec_t *); 
static void add_vtx_to_offlist(struct vtx_t *);
static void chkchan_edge(struct chanrec_t *, struct edge_t *);
static void save_bidandtran_npps(void);
static void init_sttranif_chan(struct chanrec_t *);
static void init_add_vtx_and_subtree(struct vtx_t *, struct itree_t *itp);
static void off_stvtxtab_marks(void);

static void stren_schd_bidpthdrvrs(struct net_t *, byte *, byte *);
static void schd_bidpthdrvrs(struct net_t *, struct xstk_t *,
 struct xstk_t *);
static int32 schd_1bitpthdrvr(struct net_t *, register int32, i_tev_ndx *);
static int32 evtr_schd_1bitpthdrvr(struct net_t *, register int32,
 i_tev_ndx *);
static void eval_assign_bid_chan(struct chanrec_t *);
static void eval_assign_stbid_chan(struct chanrec_t *);
static void ld_vtx_netbit(word32 *, word32 *, struct net_t *, int32);
static void st_vtx_netbit(struct net_t *, int32, word32, word32);
static void trmsg_frc_inhibit(struct net_t *, int32);
static void transtore_trmsg(struct net_t *, int32, int32, word32, word32);
static void stassign_1tranbit(struct net_t *, register int32, register word32);
static void assign_1tranbit(struct net_t *, register int32, register word32,
 register word32);
static void eval_update_1w_tranchan(struct vtx_t *);
static struct vtxlst_t *add_stchan_chged_vtx(struct vtx_t *, struct itree_t *);
static void assign_chged_vtxs(void);
static void find_chgvtx_vicinity(struct vtxlst_t *);
static void stchan_trif_relax(void);
static void add_to_chg_vtx_list(struct vtxlst_t *, int32);
static void dmp_perturb_list(void);
static char *to_vtx_info(char *, struct vtx_t *, struct itree_t *);
static int32 try_reduce_tranif_stren(word32 *, struct gate_t *);
static void try_reduce_tran_stren(word32 *, int32);
static int32 get_switch_tranif_onoff(struct gate_t *, int32);
static int32 get_bidchan_val(struct chanrec_t *, register struct net_t *,
 int32, int32);
static void dmp_vtx_edges(struct vtx_t *, struct itree_t *);
static void dmp_vtx_and_out_edges(struct vtx_t *, struct itree_t *);
static void prt_edge(struct vtx_t *, struct edge_t *, int32);
static struct net_t *xldrvld_to_netbit(struct expr_t *, int32, int32 *,
 struct itree_t *);
static void getv2_itp(struct edge_t *, struct itree_t *, struct itree_t **,
 struct itree_t **);
static char *to_vtx(char *, struct vtx_t *);
static struct itree_t *trchan_get_oside_itp(register struct net_pin_t *, int32,
 int32 *, struct expr_t **, struct itree_t *);
static struct expr_t *find_cat_oside_xp(struct expr_t *, int32, int32 *);
static void fill_port_npps(struct net_t *, int32, struct itree_t *);
static void add_match_vtxs(struct net_t *, struct net_pin_t *, int32);
static int32 get_xldl_vtx(struct itree_t *, struct net_t *, int32, int32 *);
static int32 add_xldl_vtx(struct itree_t *, struct net_t *, int32, int32);

static void do_qc_wire_intran_force(struct net_t *, int32, word32, word32,
 struct itree_t *);
static void do_qc_wire_intran_release(struct net_t *, int32, struct expr_t *,
 struct itree_t *);
static void do_vpi_wire_intran_force(struct net_t *, int32, word32 *, word32 *);
static void do_vpi_wire_intran_release(struct net_t *, int32);
static void do_putv_wire_intran_softforce(struct net_t *, int32, word32 *, word32 *);

/* extern prototypes (maybe defined in this module) */
extern void __dmp_modtrans(struct mod_t *);
extern struct net_t *__find_tran_conn_np(struct expr_t *);
extern void __allocinit_stperival(union pck_u *, int32, struct net_t *, int32);
extern void __allocinit_perival(union pck_u *, int32, int32, int32);
extern char *__msg2_blditree(char *, struct itree_t *);
extern char *__my_realloc(char *, int32, int32);
extern void __get_bidnpp_sect(struct net_t *, struct net_pin_t *, int32 *,
 int32 *);
extern struct net_t *__tranx_to_netbit(register struct expr_t *, int32, int32 *,
 struct itree_t *);
extern void __xmrpush_refgrp_to_targ(struct gref_t *);
extern int32 __get_const_bselndx(register struct expr_t *);
extern void __eval_tran_bits(register struct net_t *);
extern void __eval_tran_1bit(register struct net_t *, register int32);
extern char *__my_malloc(int32);
extern void __my_free(char *, int32);
extern struct xstk_t *__stload_mdrwire(struct net_t *);
extern struct xstk_t *__load_mdrwire(register struct net_t *);
extern void __grow_xstk(void);
extern void __chg_xstk_width(struct xstk_t *, int32);
extern void __ld_perinst_val(register word32 *, register word32 *, union pck_u,
 int32);
extern void __st_perinst_val(union pck_u, int32, register word32 *,
 register word32 *);
extern char *__st_regab_tostr(char *, byte *, int32);
extern char *__regab_tostr(char *, word32 *, word32 *, int32, int32, int32);
extern char *__to_wtnam(char *, struct net_t *);
extern void __lhsbsel(register word32 *, register int32, word32);
extern struct pthdst_t *__get_path_del(struct rngdwir_t *, int32, word64 *);
extern void __emit_path_distinform(struct net_t *, struct pthdst_t *,
 word64 *);
extern int32 __em_suppr(int32);
extern void __emit_path_pulsewarn(struct pthdst_t *, struct tev_t *, word64 *,
 word64 *, char *, word32);
extern void __emit_path_samewarn(struct net_t *, int32, struct tev_t *,
 word64 *, char *, word32);
extern void __schedule_1wev(struct net_t *, int32, int32, word64, word64,
 word32, i_tev_ndx *, int32);
extern void __reschedule_1wev(i_tev_ndx, word32, word64, word64, i_tev_ndx *);
extern void __cancel_1wev(struct tev_t *t);
extern char *__to_evtrwnam(char *, struct net_t *, int32, int32,
 struct itree_t *);
extern char *__to_timstr(char *, word64 *);
extern char *__to_vnam(char *, word32, word32);
extern char *__bld_lineloc(char *, word32, int32);
extern int32 __move_to_npprefloc(struct net_pin_t *);
extern word32 __comb_1bitsts(word32, register word32, register word32);
extern void __eval_1w_nonstren(register word32 *, register word32 *,
 register word32, register word32, word32);
extern void __chg_st_val(struct net_t *, register word32 *, register word32 *);
extern void __st_val(struct net_t *, register word32 *, register word32 *);
extern void __chg_st_bit(struct net_t *, int32, register word32, register word32);
extern void __st_bit(struct net_t *, int32, register word32, register word32);
extern char *__to_vvstnam(char *, word32);
extern void __dmp_trchan(struct vtx_t *);
extern void __dmp_bidchan(struct chanrec_t *);
extern void __dmp1_nplstel(struct mod_t *, struct net_t *,
 struct net_pin_t *);
extern void __dmp_bidnet_drvs(struct net_t *, struct mod_t *);
extern void __add_dmpv_chglst_el(struct net_t *);
extern void __add_nchglst_el(register struct net_t *);
extern void __add_select_nchglst_el(register struct net_t *, register int32,
 register int32);
extern void __wakeup_delay_ctrls(register struct net_t *, register int32,
 register int32);
extern char *__msgexpr_tostr(char *, struct expr_t *);
extern void __ld_bit(register word32 *, register word32 *,
 register struct net_t *, int32);
extern void __qc_tran_wireforce(struct net_t *, int32, int32, int32,
 struct itree_t *, struct st_t *);
extern void __qc_tran_wirerelease(struct net_t *, int32, int32, struct itree_t *,
 struct expr_t *);
extern void __tran_wire_vpi_force(struct net_t *, word32 *, word32 *, int32); 
extern void __tran_wire_vpi_release(struct net_t *, int32); 
extern void __tran_exec_putv_wire_softforce(struct net_t *, word32 *, word32 *,
 int32); 
extern void __alloc_qcval(struct net_t *);
extern void __bit1_vpi_or_tran_wireforce(struct net_t *, word32 *, word32 *,
 int32, int32, int32, char *);
extern int32 __correct_forced_newwireval(struct net_t *, word32 *, word32 *);
extern void __find_call_force_cbs(struct net_t *, int32);
extern void __cb_all_rfs(struct net_t *, int32, int32);
extern void __find_call_rel_cbs(struct net_t *, int32);
extern void __assign_1mdrwire(register struct net_t *);
extern int32 __unnormalize_ndx(struct net_t *, int32);
extern struct xstk_t *__eval_assign_rhsexpr(register struct expr_t *,
 register struct expr_t *);

extern void __cv_msg(char *, ...);
extern void __tr_msg(char *, ...);
extern void __dbg_msg(char *, ...);
extern void __sgfinform(int32, char *, ...);
extern void __vpi_err(int32, int32, char *, ...);
extern void __arg_terr(char *, int32);
extern void __case_terr(char *, int32);
extern void __misc_terr(char *, int32);
extern void __vpi_terr(char *, int32);

extern word32 __mos_stmap[];
extern word32 __rmos_stmap[];
extern word32 __cap_to_stren[];

/*
 * ROUTINES TO BUILD TRAN AND UNCOLLAPSED INOUT TRAN CHANNEL CONN. GRAPHS
 */

/*
 * build the separate inout tran graphs
 * assumes called before building tran gates so cross instance trans
 * allow all normal trans to be added to inter instance channels
 * 
 * must build from both side because channel can go up and and
 * then down into other instance
 * FIXME - for now never will see concatenate here
 */
extern void __bld_bidandtran_graph(void)
{
 register int32 ni, chi;
 register struct net_t *np;
 register struct chanrec_t *chanp;
 int32 si, design_has_trans, start_chanid;
 struct mod_t *mdp;
 struct traux_t *trap;

 if (__switch_verbose)
  { 
   __cv_msg("  SWITCH: begin switch channel graph construction.\n");
  }

 /* need to count number of switch (have tranif or different wire types) */
 bld_trauxs();

 /* DBG remove --
 malloc_chain_check(1);
 -- */

 /* alloc channel info table - size increased when needed */
 __nxt_chan_id = 0;
 __chantab = (struct chanrec_t *) __my_malloc(1000*sizeof(struct chanrec_t));
 __chanallocsize = 1000;
 design_has_trans = FALSE;
 for (si = 0; si < 7; si++)
  { __stvtxtab[si] = NULL; __stvtxtabend[si] = NULL; }
 __chg_vtxlst_hdr = __chg_vtxlst_end = NULL;
 __off_vtxlst_hdr = __off_vtxlst_end = NULL;
 __vtx_freelst = NULL;
 __edge_freelst = NULL;
 __vtxlst_freelst = NULL;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mnnum == 0) continue;
   /* if no trans and no inouts module will have no tran channels */
   if (!mdp->mod_hasbidtran && !mdp->mod_hastran) continue;
   start_chanid = __nxt_chan_id;
   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
    {
     if ((trap = np->ntraux) == NULL) continue;
     bld_trgraph(mdp, trap, np);
     design_has_trans = TRUE;
    }
   free_vtx_and_edge_freelsts();
   if (__switch_verbose && __nxt_chan_id > start_chanid)
    {
     __cv_msg(
      "  SWITCH: %d channels in module %s (%d insts, %d nets) built.\n",
      __nxt_chan_id - start_chanid,
      mdp->msym->synam, mdp->flatinum, mdp->mnnum);
    }
  }

 __num_switch_chans = 0;
 for (chi = 0; chi < __nxt_chan_id; chi++)
  {
   chanp = &(__chantab[chi]);
   if (chanp->chtyp != TRPROC_TRAN && chanp->chan_diffwtyps)
    {
     if (chanp->chtyp == TRPROC_STBID) chanp->chtyp = TRPROC_STWTYPBID;
     else if (chanp->chtyp == TRPROC_BID)
      {
       /* SJM 04/21/01 - internal error if inout only wired trchan non stren */
       __case_terr(__FILE__, __LINE__);
      }
     else __case_terr(__FILE__, __LINE__); 
    }
   if (chanp->chtyp == TRPROC_STWTYPBID || chanp->chtyp == TRPROC_TRAN)
    __num_switch_chans++;

   /* only check tran channels */
   /* DBG remove --- */
   if (__debug_flg)
    {  
     if (!chanp->chan_no_vtxs) chkchannel(chanp);
    }
   /* --- */
  }
 if (__switch_verbose) emit_chan_size_table();
 /* done with tran npps (nlds and ndrvs must now only have hard ones) */
 /* move to list in traux for vpi_ local load/driver processing */
 save_bidandtran_npps();
 if (__debug_flg && design_has_trans)
  {
   __dbg_msg("++ dumping all channels after optimization:\n");
/* -- DBG remove -- 
   for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
    {
     if (mdp->mnnum == 0) continue;
     if (!mdp->mod_hasbidtran && !mdp->mod_hastran) continue;
     __dmp_modtrans(mdp);
    }
--- */
   /* 04/12/00 - */
   for (chi = 0; chi < __nxt_chan_id; chi++)
    {
     chanp = &(__chantab[chi]);
     if (chanp->chan_no_vtxs) __dmp_bidchan(chanp);
     else
      {
       /* dump from vertex although vtx is just used to get back to chan id */
       __push_itstk(chanp->chvitp);
       __dmp_trchan(chanp->chvxp);
       __pop_itstk();
      }
    }
  }
}

/*
 * free vtx and edge free lists
 * 
 * because freeing pure bid channels now faster to use free list for
 * vertices and edges
 * SJM 07/27/01 - also needed because vtx list els used in building channels
 */
static void free_vtx_and_edge_freelsts(void)
{
 register struct vtx_t *vtxp, *vtxp2;
 register struct edge_t *ep, *ep2;
 
 for (vtxp = __vtx_freelst; vtxp != NULL;)
  {
   /* free list linked using edge field */
   vtxp2 = (struct vtx_t *) vtxp->vedges;
   __my_free((char *) vtxp, sizeof(struct vtx_t));
   vtxp = vtxp2;
  }
 __vtx_freelst = NULL;

 for (ep = __edge_freelst; ep != NULL;)
  {
   ep2 = ep->enxt;
   __my_free((char *) ep, sizeof(struct edge_t));
   ep = ep2;
  }
 __edge_freelst = NULL;
}

/*
 * routine for switch verbose mode to give table of switch chan sizes 
 */
static void emit_chan_size_table(void)
{
 register struct chanrec_t *chanp;
 register int32 chi;
 int32 size1_5, size5_10, size10_20, size20_50, size50_100, size100_200;
 int32 size200_500, size500_1000, more1000, i1;
 struct net_t *np;
 struct itree_t *itp;
 char s1[RECLEN], s2[RECLEN];

 itp = NULL;
 size1_5 = size5_10 = size10_20 = size20_50 = size50_100 = size100_200 = 0;
 size200_500 = size500_1000 = more1000 = 0;
 for (chi = 0; chi < __nxt_chan_id; chi++)
  {
   chanp = &(__chantab[chi]);

   if (chanp->numvtxs <= 5) size1_5++;
   else if (chanp->numvtxs <= 10) size5_10++;
   else if (chanp->numvtxs <= 20) size10_20++;
   else if (chanp->numvtxs <= 50) size20_50++;
   else if (chanp->numvtxs <= 100) size50_100++;
   else if (chanp->numvtxs <= 200) size100_200++;
   else if (chanp->numvtxs <= 500) size200_500++;
   else if (chanp->numvtxs <= 1000) size500_1000++;
   else
    {
     more1000++;

     /* SJM 04/23/01 - now need to access vtx for inout from bid vtxlp list */
     if (chanp->chtyp == TRPROC_TRAN || chanp->chtyp == TRPROC_STWTYPBID)
      {
       strcpy(s1, "tran");
       np = chanp->chvxp->vnp;
       i1 = chanp->chvxp->vi1;
       itp = chanp->chvitp;
      }
     else
      {
       strcpy(s1, "inout"); 
       np = chanp->bid_vtxlp->vnp;
       i1 = chanp->bid_vtxlp->vi1;
       itp = chanp->bid_vtxlp->bidvtx_itp;
      }
     if (i1 != -1)
      {
       sprintf(s2, "%s.%s[%d]", __msg2_blditree(__xs, itp), np->nsym->synam,
        i1);
      }
     else sprintf(s2, "%s.%s", __msg2_blditree(__xs, itp), np->nsym->synam);

     __cv_msg(
      "  Large %s switch channel has %d terminals (ID=%d) a vertex: %s\n",
      s1, chanp->numvtxs, chi, s2);  
    }
  }
 __cv_msg("  Total Number of Switch Channels: %d\n", __nxt_chan_id);
 if (size1_5 != 0)
  __cv_msg("  %d channels with less than 5 terminals.\n", size1_5);
 if (size5_10 != 0)
  __cv_msg("  %d channels with 6 to 10 terminals.\n", size5_10);
 if (size10_20 != 0)
  __cv_msg("  %d channels with 11 to 20 terminals.\n", size10_20);
 if (size20_50 != 0)
  __cv_msg("  %d channels with 21 to 50 terminals.\n", size20_50);
 if (size50_100 != 0)
  __cv_msg("  %d channels with 51 to 100 terminals.\n", size50_100);
 if (size100_200 != 0)
  __cv_msg("  %d channels with 101 to 200 terminals.\n", size100_200);
 if (size200_500 != 0)
  __cv_msg("  %d channels with 201 to 500 terminals.\n", size200_500);
 if (size500_1000 != 0)
  __cv_msg("  %d channels with 501 to 1000 terminals.\n", size500_1000);
 if (more1000 != 0)
  __cv_msg("  %d channels with more than 1000 terminals.\n", more1000);
}

/*
 * routine to build the net tran aux records for all nets that need them
 *
 * pre-allocates all trauxs including per bit forms for xmrs and inouts
 * net connection type and type of channel element determine is per inst
 *
 * after here all trauxes allocated and all per inst. bits set but 
 * do not know channel type (processing type)
 */
static void bld_trauxs(void) 
{
 register struct net_pin_t *npp;
 register int32 ni, gi, gri;
 register struct net_t *np;
 int32 chg;
 struct gref_t *grp;
 struct gate_t *gp;
 struct mod_t *mdp, *refmdp;
 struct net_t *np1;

 /* all xmr lhs net targs need per inst. and define per inst. channel */
 /* even if rooted with only one itree loc. target */
 /* this will get all xmr def. (target) wires from the ref */ 
 /* this will mark tran terminals so can find all per inst. other sides */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   for (gri = 0, grp = &(mdp->mgrtab[0]); gri < mdp->mgrnum; gri++, grp++)  
    {
     /* if ref. point is rhs, channel does not need to be per inst. */
     if (grp->gr_gone || !grp->gxndp->x_islhs) continue;

     /* for upward rel. can use 1st for mod type since all same */
     /* SJM 08/15/00 - using 0th ref loc inst. table entry - always exists */ 
     if (grp->upwards_rel)
      {
       refmdp = grp->targu.uprel_itps[0]->itip->imsym->el.emdp;
      }
     else refmdp = grp->targmdp;

     np = grp->targsyp->el.enp;
     /* must do all or will not set all needed mod having tran/bid trans */
     for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
      {
       if (npp->npntyp == NP_BIDMDPRT && npp->npntyp == NP_BIDICONN)
        refmdp->mod_hasbidtran = TRUE; 
       else if (npp->npntyp == NP_TRAN) refmdp->mod_hastran = TRUE;
       else continue;
       /* may have lots of xmrs to same target - all must be per bit/inst */
       if (np->ntraux == NULL) alloc_traux(refmdp, np);
      }
    }
  }

 /* any inout or inout iconn even if only 1 inst.*/
 /* need per inst. even if 1 inst in case other side has per inst. */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mnnum == 0) continue;
   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
    {
     if (np->ntraux != NULL) continue;
     for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
      {
       if (npp->npntyp == NP_TRAN) { mdp->mod_hastran = TRUE; continue; }
       if (npp->npntyp == NP_BIDMDPRT || npp->npntyp == NP_BIDICONN)
        {  
         /* must go through all to set has bid tran for module */ 
         mdp->mod_hasbidtran = TRUE; 
         /* if only one inst. per inst. does nothing */
         if (np->ntraux == NULL) alloc_traux(mdp, np);
        }
      }
    }
  }

 /* all nets that connect to other side of per. inst trans */
 /* all inout trans are alloc and marked by here - look only at trans */
 /* all tran nets that are per inst from xmr have traux by here */ 
 /* and all module having tranor bid tran set by here */ 
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (!mdp->mod_hastran) continue;
   chg = TRUE;
   while (chg)
    {
     chg = FALSE;
     for (gi = 0; gi < mdp->mgnum; gi++)
      {
       gp = &(mdp->mgates[gi]);

       if (gp->g_class != GC_TRAN && gp->g_class != GC_TRANIF) continue;

       /* 04/26/01 - if terminals same, removed so ignore here */
       if (gp->g_gone) continue;

       /* if xmr, the net will be per inst already */ 
       np = __find_tran_conn_np(gp->gpins[0]);
       np1 = __find_tran_conn_np(gp->gpins[1]);
       if (np->ntraux != NULL)
        {
         if (np1->ntraux == NULL)
          { alloc_traux(mdp, np1); chg = TRUE; }
        }
       else if (np1->ntraux != NULL)
        {
         if (np->ntraux == NULL)
          { alloc_traux(mdp, np); chg = TRUE; }
        }
      }
    }
  }
 /* allocate non per inst - can only be non xmr/inout conn. tran gate */
 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mnnum == 0) continue;
   if (!mdp->mod_hastran) continue;
   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
    {
     if (np->ntraux != NULL) continue;

     for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
      {
       /* if on other side of inout/iconn to inout in chan or xmr tran */
       /* already allocated */
       if (npp->npntyp == NP_TRAN) { alloc_traux(mdp, np); break; }  
      }
    }
  }
}

/*
 * allocate and initialize a traux for a net not yet on any tran channel
 */
static void alloc_traux(struct mod_t *mdp, struct net_t *np)
{
 struct traux_t *trap; 
 int32 nbytes;

 /* DBG remove --
 __dbg_msg("allocating traux for net %s in module %s\n",
  np->nsym->synam, mdp->msym->synam);
 --- */
 
 trap = (struct traux_t *) __my_malloc(sizeof(struct traux_t));
 np->ntraux = trap; 
 /* alloc storage for hard drivers - must be per instance */
 if (np->n_stren)
  __allocinit_stperival(&trap->trnva, mdp->flatinum, np, TRUE);
 else __allocinit_perival(&trap->trnva, mdp->flatinum, np->nwid, TRUE);
 /* allocate per inst per bit bit chan */ 
 nbytes = np->nwid*mdp->flatinum*sizeof(struct vbinfo_t *);
 /* allocate per bit vtx and chan id */
 /* BEWARE - assumes can set array of nil ptrs by zeroing */
 trap->vbitchans = (struct vbinfo_t **) __my_malloc(nbytes);
 memset(trap->vbitchans, 0, nbytes);
 trap->tran_npps = NULL;
}

/*
 * routine to build tran channel graph for 1 for net
 * must fill channel for each instance
 * for type tran channel np insts will be 1 even though maybe more
 *
 * SJM - 08/26/00 - changed so storing dest. itree loc (if different) in
 * edge - so tracing edge just pushes new itree loc
 */
static void bld_trgraph(struct mod_t *mdp, struct traux_t *trap,
 struct net_t *np)
{
 register int32 ii, bi2;
 int32 bi, insts, osize;
 struct vbinfo_t *vbip;
 struct chanrec_t *chanp;

 /* know all wires bits in per-inst channels, mark per inst by here */
 insts = mdp->flatinum;
     
 /* build tran channel for each bit of net */ 
 for (bi2 = np->nwid - 1; bi2 >= 0; bi2--)
  {
   /* DBG remove --
   __cv_msg("++ malloc chain check before processing %s[%d] tran:\n",
    np->nsym->synam, (!np->n_isavec ? -1 : bi2));
   malloc_chain_check(1);
   -- */

   bi = (!np->n_isavec) ? -1 : bi2;
   /* DBG remove ---
   if (__debug_flg)
    {
     __dbg_msg("++ processing %s[%d] tran:\n", np->nsym->synam,
      (!np->n_isavec ? -1 : bi));
    }
   --- */
   for (ii = 0; ii < insts; ii++) 
    {
     /* notice know that ii and itinum same */
     __push_itstk(mdp->moditps[ii]);
     if ((vbip = trap->vbitchans[np->nwid*ii + bi2]) != NULL)
      {
       if (__debug_flg)
        {
         sprintf(__xs2, ", itp=%s", __msg2_blditree(__xs, __inst_ptr));

         /* DBG remove -- */
         if (__debug_flg)
          {
           __dbg_msg(
            "-- vertex %s (id=%d%s) already in channel\n",
            to_vtx(__xs, vbip->vivxp), vbip->chan_id, __xs2);
          }
         /* --- */
        }
       __pop_itstk();
       continue; 
      }
     /* add the new vertex, bld net to vtx conn and maybe alloc vb tab */
     vbip = add_vtxtraux(np, bi, __nxt_chan_id++); 
     trap->vbitchans[np->nwid*ii + bi2] = vbip;
    
     if (__nxt_chan_id >= __chanallocsize)
      {
       osize = __chanallocsize;
       __chanallocsize += (3*osize)/2;
       __chantab = (struct chanrec_t *)
        __my_realloc((char *) __chantab, osize*sizeof(struct chanrec_t),
         (int32) (__chanallocsize*sizeof(struct chanrec_t)));
      }
     chanp = &(__chantab[__nxt_chan_id - 1]);
     init_chanrec(chanp);
     chanp->chvxp = vbip->vivxp;
     chanp->chvitp = __inst_ptr;
     chanp->numvtxs = 1;
     /* SJM 09/18/00 - since no longer checking channels need rigth first */
     /* channel type for 1 element (unc.) channels */
     if (np->n_stren) chanp->chtyp = TRPROC_STBID;
     else chanp->chtyp = TRPROC_BID;

     /* SJM 07/27/01 - change algorithm to use priority queue */
     /* new algorithm saves oside vtxes on vtx list and adds edges from cur */
     /* vertex (not both edges at once as before) before moving to next */
     /* DBG add -- sav_mem_use = __mem_use; */
     add_vtx_to_future_q(vbip->vivxp, __inst_ptr);
     add_1chan_vtxes_and_edges(__nxt_chan_id - 1);

     /* st bid and simple bid, convert to simple vtx list and free chan */ 
     if ((chanp->chtyp == TRPROC_STBID || chanp->chtyp ==TRPROC_BID)
      && !chanp->chan_diffwtyps)
      {
       chg_bidchan_to_vtxlist(chanp);
      }
     /* DBG add --
     else
      {
       if (__switch_verbose) 
        {
         __cv_msg("tran channel with %d vertices requires %d bytes.\n",
          chanp->numvtxs, __mem_use - sav_mem_use);
        }
      }
     --- */

     if (__switch_verbose && ((__nxt_chan_id % 5000) == 0))
      {
       __cv_msg(
        "  SWITCH: %d switch/inout channel graphs constructed.\n",
        __nxt_chan_id);
      }
     __pop_itstk();
    }
  }
}

/*
 * initialize a record for a new channel
 */
static void init_chanrec(struct chanrec_t *chanp)
{
 chanp->chtyp = TRPROC_UNKN;
 chanp->chan_diffwtyps = FALSE;
 chanp->chan_no_vtxs = FALSE;
 chanp->chvxp = NULL;
 chanp->chvitp = NULL;
 chanp->bid_vtxlp = NULL;
 chanp->numvtxs = 0;
}

/*
 * add a new vertex and add net to link vertex
 * vertices are net and edges are gates or cross port assigns (npps) 
 */
static struct vbinfo_t *add_vtxtraux(struct net_t *np, int32 bi, int32 chanid) 
{
 struct vtx_t *vtxp;
 struct vbinfo_t *vbip;

 vbip = (struct vbinfo_t *) __my_malloc(sizeof(struct vbinfo_t));  
 vbip->chan_id = chanid;
 /* build the new tran channel - first add this vertex on front */
 vtxp = alloc_vtx(np, bi);
 vbip->vivxp = vtxp;
 return(vbip);
}

/*
 * add all rest of vertices and all edges or rest of channel
 *
 * LOOKATME - using chg vtxlst list to build tran graph and same list
 * used later to build perturb list 
 */
static void add_1chan_vtxes_and_edges(int32 chanid)
{
 register struct vtxlst_t *vtxlp;
 register struct vtx_t *vtxp;
 struct itree_t *itp;
 
 /* know at least one vtx list element or will not get here */
 for (vtxlp = __chg_vtxlst_hdr; vtxlp != NULL; vtxlp = vtxlp->vtxnxt)  
  {
   /* see if already processed */
   vtxp = vtxlp->vtxp;
   itp = vtxlp->vtx_itp;

   /* mark set when all edges from vertex added */
   if (vtxp->vtx_mark) continue;

   add_edges_from_npps(vtxp, chanid, itp);
   vtxp->vtx_mark = TRUE; 
  }
 /* turn off all marks */
 for (vtxlp = __chg_vtxlst_hdr; vtxlp != NULL; vtxlp = vtxlp->vtxnxt)  
  vtxlp->vtxp->vtx_mark = FALSE;

 /* add entire vtx list to vtx list free list */
 __chg_vtxlst_end->vtxnxt = __vtxlst_freelst; 
 __vtxlst_freelst = __chg_vtxlst_hdr;
 __chg_vtxlst_hdr = __chg_vtxlst_end = NULL;
}

/*
 * allocate a tran graph vertex (wire-bit)
 */
static struct vtx_t *alloc_vtx(struct net_t *np, int32 bi)
{
 struct vtx_t *vtxp;

 if (__vtx_freelst != NULL)
  {
   vtxp = __vtx_freelst;
   __vtx_freelst = (struct vtx_t *) __vtx_freelst->vedges;
  }
 else vtxp = (struct vtx_t *) __my_malloc(sizeof(struct vtx_t)); 

 vtxp->new_vtxval = ST_HIZ;
 vtxp->old_vtxval = ST_HIZ;
 vtxp->vtx_chged = FALSE;
 vtxp->vtx_in_vicinity = FALSE;
 vtxp->vtx_forced = FALSE;
 vtxp->vtx_supply = FALSE;
 vtxp->vtx_mark = FALSE;
 vtxp->vtx_mark2 = FALSE;
 vtxp->vnp = np;
 vtxp->vi1 = bi;
 vtxp->vedges = NULL;
 return(vtxp);
}

/*
 * add vertex to future add vtx from edges fifo queue
 */
static void add_vtx_to_future_q(struct vtx_t *vtxp, struct itree_t *itp)
{
 register struct vtxlst_t *vtxlp;

 /* alloc and add to list */
 if (__vtxlst_freelst == NULL)
  vtxlp = (struct vtxlst_t *) __my_malloc(sizeof(struct vtxlst_t));
 else
  {
   vtxlp = __vtxlst_freelst;
   __vtxlst_freelst = __vtxlst_freelst->vtxnxt;
  }
 vtxlp->vtxp = vtxp;
 vtxlp->vtx_itp = itp;
 vtxlp->vtxnxt = NULL;
 if (__chg_vtxlst_hdr == NULL) __chg_vtxlst_hdr = __chg_vtxlst_end = vtxlp;
 else
  {
   __chg_vtxlst_end->vtxnxt = vtxlp;
   __chg_vtxlst_end = vtxlp; 
  }
}

/*
 * process all edge's from current vtx list el and add one from this
 * vtx edge for every npp and add all vertices not in a switch channel
 *
 * 05/21/01 SJM - now passing vertex itree loc (for edge it is vtx 1)
 * 07/27/01 SJM - now add all vertices breadth first
 */
static void add_edges_from_npps(struct vtx_t *vtxp, int32 chanid,
 struct itree_t *vt1_itp)
{
 register struct net_pin_t *npp;
 int32 ri1, ri2, bi, bi2, osbi, fromr_bi, catel_bi;
 int32 cat_oside_bi, oside_vtxndx;
 struct net_t *np, *osnp;
 struct expr_t *xp;
 struct itree_t *vt2_itp, *oside_itp;

 np = vtxp->vnp;
 bi = vtxp->vi1;
 /* bi2 is edge start bit of vectored net */
 bi2 = (bi == -1) ? 0 : bi;
 for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
  {
   /* ignore hard (channel input) drivers */
   if (npp->npntyp != NP_BIDICONN && npp->npntyp != NP_BIDMDPRT
    && npp->npntyp != NP_TRAN) continue;

   /* if npp only applies to one inst, filter if wrong inst */
   /* this is only for rooted */
   if (npp->npproctyp == NP_PROC_FILT && npp->npaux->npu.filtitp != vt1_itp)
    continue;
       
   /* any IS bit select forms in tran chans removed from splitting */
   __get_bidnpp_sect(np, npp, &ri1, &ri2);
   /* unc. because index mismatch */
   if (ri1 != -1 && (bi2 > ri1 || bi2 < ri2)) continue;

   /* get oside vtx, if same as this side vt2 itp nil */
   vt2_itp = trchan_get_oside_itp(npp, bi2, &catel_bi, &xp, vt1_itp);
   /* if other side unc., nothing to add */
   if (xp->optyp == OPEMPTY) continue;

   oside_itp = (vt2_itp != NULL) ? vt2_itp : vt1_itp; 
   cat_oside_bi = -1;
   /* this looks ahead to other side but not used until bottom */
   /* no itree loc. change */
   if (npp->npntyp == NP_TRAN)
    {
     osnp = __tranx_to_netbit(xp, 0, &osbi, oside_itp); 
     if (osbi == -2) continue;

     /* flags are not per inst. - use near side bit index */
     /* for now no special routine for channels with tranifs */
     __chantab[chanid].chtyp = TRPROC_TRAN;
     /* FIXME - set tranif processing mode separate value here when used */
    }
   else if (npp->npntyp == NP_BIDICONN || npp->npntyp == NP_BIDMDPRT)
    { 
     /* set index offset to match other side from this side npp */
     if (npp->npaux == NULL) fromr_bi = bi2;
     else
      { 
       if (npp->npaux->nbi1 == -1) fromr_bi = bi2;
       /* ??? LOOKATME - why is npp low psel bit subtracted off */  
       else fromr_bi = bi2 - npp->npaux->nbi2.i;

       /* in case this side expr in concat need low of where in concat */
       /* so can add to otherside index to get matching oside bit */
       if (npp->npaux->lcbi1 != -1) cat_oside_bi = npp->npaux->lcbi2;
      }

     /* set type of channel if changes */
     if (__chantab[chanid].chtyp != TRPROC_TRAN)
      {
       if (np->n_stren)
        {
         /* DBG remove --- */
         if (__chantab[chanid].chtyp == TRPROC_BID)
          __misc_terr(__FILE__, __LINE__); 
         /* --- */
         __chantab[chanid].chtyp = TRPROC_STBID;
        }
       else
        {
         /* DBG remove --- */
         if (__chantab[chanid].chtyp == TRPROC_STBID)
          __misc_terr(__FILE__, __LINE__); 
         /* --- */
         __chantab[chanid].chtyp = TRPROC_BID;
        }
      }
     osnp = __tranx_to_netbit(xp, ((catel_bi == -1) ? fromr_bi : catel_bi),
      &osbi, oside_itp); 
     if (osbi == -2) continue;
    }
   else { __case_terr(__FILE__, __LINE__); continue; }

   /* compute other side vertex index */
   /* osbi is other side connecting expr object index (maybe -1) */ 
   /* cat oside bi is offset of this side npp in other side cat (none -1) */
   if (osbi == -1)
    {
     if (cat_oside_bi != -1) oside_vtxndx = cat_oside_bi;
     else oside_vtxndx = -1;
    }
   else
    { 
     if (cat_oside_bi == -1) oside_vtxndx = osbi;
     else oside_vtxndx = osbi + cat_oside_bi;
    }

   /* alloc oside vertex if needed and add one edge to it */
   if (!add1_oside_edge(vtxp, npp, osnp, oside_vtxndx, chanid, vt1_itp,
    vt2_itp)) continue;

   /* DBG remove --
   if (__debug_flg)
    { 
     int32 ei;
     struct edge_t *ep;

     -* this must run in itree loc. of this side edge *-
     __push_itstk(vt1_itp);
     sprintf(__xs2, ", itp=%s", __msg2_blditree(__xs, __inst_ptr));

     __dbg_msg(">< vertex %s(id=%d%s) added, edges:\n",
      to_vtx(__xs, vtxp), chanid, __xs2);
     for (ei = 1, ep = vtxp->vedges; ep != NULL; ep = ep->enxt, ei++) 
      prt_edge(vtxp, ep, ei);
     __pop_itstk();
    }
   --- */
  }
}

/*
 * convert npp into range - uses current instance 
 */ 
extern void __get_bidnpp_sect(struct net_t *np, struct net_pin_t *npp,
 int32 *bi1, int32 *bi2)
{
 struct npaux_t *auxp;

 if ((auxp = npp->npaux) == NULL || auxp->nbi1 == -1)
  {
   /* all internal indices normalized to h:0 */
   if (np->n_isavec) { *bi1 = np->nwid - 1; *bi2 = 0; }
   else *bi1 = *bi2 = -1;
   return;
  }
 /* DBG remove -- */
 if (auxp->nbi1 == -2) __arg_terr(__FILE__, __LINE__);
 /* --- */
 *bi1 = auxp->nbi1;
 *bi2 = auxp->nbi2.i;
}

/*
 * move from trchan npp to other side or 2nd vtx itree loc. 
 * returns other side itree loc if different else nil
 *
 * does not use net (col. to in vtx), so for col. to chges only itp 
 * LOOKATME - think this code may be wrong?
 */
static struct itree_t *trchan_get_oside_itp(register struct net_pin_t *npp,
 int32 bi2, int32 *catel_bi, struct expr_t **xp2, struct itree_t *vt1_itp)
{
 register struct itree_t *vt2_itp, *itp2;
 register struct expr_t *xp;
 register struct mod_pin_t *mpp;
 register struct expr_t *idndp;
 struct gate_t *gp;
 struct npaux_t *auxp;
 struct mod_t *vt2_mdp;
 int32 netbi;

 *catel_bi = -1;
 vt2_itp = NULL; 
 switch ((byte) npp->npntyp) {
  case NP_TRAN:
   gp = npp->elnpp.egp;
   if (npp->obnum == 0) xp = gp->gpins[1]; else xp = gp->gpins[0];
   break;
  case NP_BIDMDPRT:
   xp = vt1_itp->itip->ipins[npp->obnum];
   vt2_itp = vt1_itp->up_it; 
   break;
  case NP_BIDICONN: 
   if (npp->np_xmrtyp != XNP_LOC)
    {
     /* SJM 10/05/99 - for down relative may be at top or at bottom */
     /* correct by detecting up */
     if (npp->np_xmrtyp == XNP_RTXMR)
      {
       /* always moved to ref. point even if at top - since rooted works */
       __push_itstk(vt1_itp); 
       /* SJM 04/17/03 - this can't fail */
       __move_to_npprefloc(npp);
       itp2 = __inst_ptr;
       __pop_itstk();
       __pop_itstk();
      }
     else
      {
       /* for non rooted, if at ref. point, do not move */
       if (vt1_itp->itip->imsym->el.emdp == npp->npaux->npu.npgrp->gin_mdp)
        itp2 = vt1_itp;
       else
        {
         /* if F, this instance not xmr, if T pushes ref loc on it stk */
         __push_itstk(vt1_itp); 
         /* SJM 04/17/03 - if not matching XMR path, must not move */
         if (!__move_to_npprefloc(npp)) __misc_terr(__FILE__, __LINE__);
         itp2 = __inst_ptr;
         __pop_itstk();
         __pop_itstk();
        }
      }
    }
   /* local non xmr - set itree loc for use below */
   else itp2 = vt1_itp;

   /* DBG remove -- */
   if (itp2->in_its == NULL) __misc_terr(__FILE__, __LINE__);
   if (itp2->itip->imsym->el.emdp->minum <= npp->elnpp.eii)
    __misc_terr(__FILE__, __LINE__);
   /* --- */
   vt2_itp = &(itp2->in_its[npp->elnpp.eii]);
   vt2_mdp = vt2_itp->itip->imsym->el.emdp;
   /* DBG remove -- */
   if (npp->obnum >= vt2_mdp->mpnum) __misc_terr(__FILE__, __LINE__);
   /* --- */
   mpp = &(vt2_mdp->mpins[npp->obnum]);
   xp = mpp->mpref;
   break;
  default:  __case_terr(__FILE__, __LINE__); xp = NULL;
 }

 /* need to find channel */
 if (xp->optyp == ID || xp->optyp == GLBREF) idndp = xp;  
 else if (xp->optyp == LSB || xp->optyp == PARTSEL) idndp = xp->lu.x;
 else if (xp->optyp == OPEMPTY) { *xp2 = xp; return(vt2_itp); }
 else if (xp->optyp == LCB)
  {
   /* LOOKATME - for entire wire no lcb ofset */
   if ((auxp = npp->npaux) == NULL || auxp->lcbi1 == -1) netbi = bi2;
   else netbi = bi2 + auxp->lcbi1;
   
   /* this does not need itree loc */
   *xp2 = find_cat_oside_xp(xp, netbi, catel_bi);
   return(vt2_itp);
  }
 else { __case_terr(__FILE__, __LINE__); idndp = NULL; }
 
 if (idndp->optyp == GLBREF) 
  {
   /* SJM 07/23/01 - for other side of mod port xmr, need to push oside itp */
   /* not entering down side - can't happen for ICONN since oside never xmr */
   /* notice if not mod port will be tran where vt2 itp not set */
   if (npp->npntyp == NP_BIDMDPRT) __push_itstk(vt2_itp);
   else __push_itstk(vt1_itp);

   __xmrpush_refgrp_to_targ(idndp->ru.grp);
   vt2_itp = __inst_ptr;
   __pop_itstk();
   __pop_itstk();
  }
 *xp2 = xp;
 return(vt2_itp);
}

/*
 * routine to find non concat lhs xp from concat and bit index in concat 
 *
 * returns bit index in xp net if bit or part select or vectored net
 * returns -1 for scalar
 */
static struct expr_t *find_cat_oside_xp(struct expr_t *xp, int32 catbi,
 int32 *catel_bi)
{
 register struct expr_t *catxp;
 register int32 catxlen, bi1;

 for (catxp = xp->ru.x; catxp != NULL; catxp = catxp->ru.x)
  {
   catxlen = catxp->lu.x->szu.xclen;
   /* bi1 is low bit of first (high) concat element */
   bi1 = catxp->szu.xclen - catxlen;

   /* DBG remove ---
   if (__debug_flg)
    __dbg_msg(
     "+++rhs: total concat wid=%u, low index=%d, wid=%u, remaining wid=%u\n",
     xp->szu.xclen, bi1, catxlen, catxp->szu.xclen);
   --- */ 
   /* since goes from high (left internal) bit to low if passed above right */ 
   if (catbi >= bi1)
    {
     /* for low bit or scalar catel_bi is 0 not 1 */
     *catel_bi = catbi - bi1; 

     /* -- DBG remove ---
     if (__debug_flg)
      {
       if (catxlen == 1) __msgexpr_tostr(__xs, catxp->lu.x);
       else sprintf(__xs, "%s[%d]", __msgexpr_tostr(__xs2, catxp->lu.x), 
        *catel_bi);
       __dbg_msg(
        "*** other side concat component %s width %d of concat size %d\n", 
        __xs, catxlen, xp->szu.xclen);  
      }
     --- */
     return(catxp->lu.x);
    }
  }
 __arg_terr(__FILE__, __LINE__);
 return(NULL);
}

/*
 * given a tran channel lhs non concatenate expression get net and bit
 *
 * sets bi to -2 for other side out of this side range
 * uses other side offset from low (right) to determine index
 * this uses itree loc to get module for constant table
 */
extern struct net_t *__tranx_to_netbit(register struct expr_t *xp,
 int32 fromr_bi, int32 *bi, struct itree_t *oside_itp)
{
 register struct net_t *np;
 int32 ri1, ri2;

 np = __find_tran_conn_np(xp);
 if (xp->optyp == LSB)
 {
   __push_itstk(oside_itp);
   *bi = __get_const_bselndx(xp);
   __pop_itstk();
   if (fromr_bi > 0) *bi = -2;
   return(np);
  }
 if (xp->optyp == PARTSEL)
  {
   ri1 = __contab[xp->ru.x->lu.x->ru.xvi];
   ri2 = __contab[xp->ru.x->ru.x->ru.xvi];
   if (fromr_bi == -1) *bi = ri2;
   else { *bi = ri2 + fromr_bi; if (*bi > ri1) *bi = -2;  }
   return(np);
  }
 /* this side is 1 bit scalar */
 if (!np->n_isavec)
  {
   *bi = -1;
   if (fromr_bi > 0) *bi = -2;
  }
 else { if (fromr_bi >= np->nwid) *bi = -2; else *bi = fromr_bi;  }
 return(np);
}

/*
 * for every npp, add other vertex if needed and connect edge from vtxp
 *
 * SJM 07/27/01 - new algorithm that uses breadth first building 
 * here if edge never seen in channel, aalloc and add vbip and put on
 * future processing fifo
 *
 * npp is one that other side of need to have edges added
 * vt1 itp always set, vt2 itp itree loc for other edge only if different
 */
static int32 add1_oside_edge(struct vtx_t *vtxp, struct net_pin_t *npp,
 struct net_t *osnp, int32 osbi, int32 chanid, struct itree_t *vt1_itp,
 struct itree_t *vt2_itp)
{
 register struct edge_t *ep1;
 register struct vbinfo_t *osvbip;
 int32 osbi2;
 struct vtx_t *osvtxp;
 struct traux_t *ostrap;
 struct itree_t *itp;

 ostrap = osnp->ntraux;
 osbi2 = (osbi == -1) ? 0 : osbi;
 /* for non per inst. itinum will be 0 */
 /* not in channel - add it */
 if (vt2_itp == NULL) itp = vt1_itp; else itp = vt2_itp;
 if ((osvbip = ostrap->vbitchans[osnp->nwid*itp->itinum + osbi2])
  == NULL)
  {
   /* add the new vertex, bld net to vtx conn */
   osvbip = add_vtxtraux(osnp, osbi, chanid); 
   ostrap->vbitchans[osnp->nwid*itp->itinum + osbi2] = osvbip;
   osvtxp = osvbip->vivxp;
   __chantab[chanid].numvtxs++;

   /* SJM 09/11/00 - at edge add set diff wtypes if edge nets ntyp differ */
   /* SJM 04/23/01 - need to handle wire type synonyms (tri=wire say) */
   if (cnvt_tobase_ntyp(osnp->ntyp) != cnvt_tobase_ntyp(vtxp->vnp->ntyp))
    __chantab[chanid].chan_diffwtyps = TRUE;

   /* finally, add to future add from edges fifo list */
   add_vtx_to_future_q(osvtxp, itp);
  }
 else osvtxp = osvbip->vivxp;
 
 /* need to add 2 opposite direct edges */ 
 ep1 = alloc_edge(npp);
 /* SJM 08/26/00 - if vt2 itp nil, then no itree change on edges */
 /* notice for cross inout ports always have itree loc change */
 if (vt2_itp != NULL) ep1->edgoside_itp = vt2_itp;
 ep1->ev2 = osvtxp;
 ep1->enxt = vtxp->vedges;
 vtxp->vedges = ep1;

 /* DBG remove --
 if (__debug_flg)
  {
   __push_itstk(vt1_itp);
   __dbg_msg(
    "!!! adding other side vertex %s and forward edge from %s:\n",
    to_vtx(__xs, osvtxp), to_vtx(__xs2, vtxp));
   -* --
   prt_edge(vtxp, ep1, -1); 
   -- *-
   __pop_itstk();
  }
 --- */
 return(TRUE);
}

/*
 * routine to convert wire types to base
 *
 * used for finding mixed wire type chans that require relax
 */
static word32 cnvt_tobase_ntyp(word32 ntyp)
{
 switch (ntyp) {
  case N_TRI: return(N_WIRE);
  case N_TRIAND: return(N_WA);
  case N_TRIOR: return(N_WO);
  default: break;
 }  
 return(ntyp);
}

/*
 * given a tran or tranif connecting port, find the net
 *
 * will never see concatenate here because xp is component of concat
 */
extern struct net_t *__find_tran_conn_np(struct expr_t *xp)
{
 struct net_t *np;

 switch ((byte) xp->optyp) { 
  case ID: case GLBREF: np = xp->lu.sy->el.enp; return(np);
  case LSB: case PARTSEL: np = xp->lu.x->lu.sy->el.enp; return(np);
  /* for opempty will be no npp */  
  default: __case_terr(__FILE__, __LINE__); 
 }
 return(NULL);
}

/*
 * routine to convert bid only same wire type channel to simple 
 *
 * SJM 04/23/01 - can process bid only channels by just going through
 * list (key is that vtx list element contains itree loc for each one)
 */
static void chg_bidchan_to_vtxlist(struct chanrec_t *chanp)
{
 register struct edge_t *ep, *ep2;
 /* DBG remove - int32 sav_mem_use; */
 struct vtx_t *vtxp;
 struct vtxlst_t *vtxlp;

 /* free every vertex and its edge list */
 /* build list of all vertices */
 vtxp = chanp->chvxp; 
 /* need to pass itree context since itstk may overflow, use call stack */
 process_bid_vertices(chanp, vtxp, chanp->chvitp);

 /* build the off list - use this to free */
 add_vtx_to_offlist(vtxp);

 for (vtxlp = __off_vtxlst_hdr; vtxlp != NULL; vtxlp = vtxlp->vtxnxt)
  {
   /* first free edge list */
   for (ep = vtxlp->vtxp->vedges; ep != NULL;)
    {
     ep2 = ep->enxt;
     __my_free((char *) ep, sizeof(struct edge_t)); 
     ep = ep2;
    }
   /* SJM 08/08/01 - can't free vertices still used by iout channels */
   /* ??? __my_free((char *) vtxlp->vtxp, sizeof(struct vtx_t)); */
  }

 /* DBG remove --
 if (__switch_verbose)
  {
   register struct bidvtxlst_t *bvtxlp;
   int32 chanid;

   chanid = chanp - __chantab;
   __cv_msg("-- chan %d (%d vertices): conversion freed %d bytes\n",
    chanid, chanp->numvtxs, sav_mem_use - __mem_use); 
  }
 --- */

 /* chan now stored as bid vtx list */
 chanp->chvxp = NULL;
 chanp->chvitp = NULL;
 chanp->chan_no_vtxs = TRUE; 

 /* then add entire off vtx list to end of vtx list free list */
 if (__off_vtxlst_hdr != NULL)
  {
   __off_vtxlst_end->vtxnxt =  __vtxlst_freelst; 
   __vtxlst_freelst = __off_vtxlst_hdr;
  }
 __off_vtxlst_hdr = __off_vtxlst_end = NULL;
}

/*
 * add vertex and all connected to linear vertex list
 */
static void process_bid_vertices(struct chanrec_t *chanp, struct vtx_t *vtxp,
 struct itree_t *vt1_itp)
{
 register struct edge_t *ep;
 struct itree_t *oside_itp;

 /* mark vertex visited and and add to list */ 
 vtxp->vtx_mark = TRUE;
 add_vtx_to_bidvtxlist(chanp, vtxp, vt1_itp);

 for (ep = vtxp->vedges; ep != NULL; ep = ep->enxt)
  {
   if (ep->ev2->vtx_mark) continue;

   /* SJM 04/23/01 - this moves to right other side edge itree loc */
   if (ep->edgoside_itp != NULL) oside_itp = ep->edgoside_itp;
   else oside_itp = vt1_itp;

   process_bid_vertices(chanp, ep->ev2, oside_itp);
  }
}

/*
 * add a vertex to bid chan linear list
 */
static void add_vtx_to_bidvtxlist(struct chanrec_t *chanp, struct vtx_t *vtxp,
 struct itree_t *vt1_itp)
{
 register struct bidvtxlst_t *bidvtxlp;

 bidvtxlp = (struct bidvtxlst_t *) __my_malloc(sizeof(struct bidvtxlst_t));

 bidvtxlp->vnp = vtxp->vnp;
 bidvtxlp->vi1 = vtxp->vi1;
 bidvtxlp->bidvtx_itp = vt1_itp;
 bidvtxlp->bidvtxnxt = NULL;

 /* must always put on end */
 if (chanp->bid_vtxlp != NULL)
  { bidvtxlp->bidvtxnxt = chanp->bid_vtxlp; chanp->bid_vtxlp = bidvtxlp; }
 else chanp->bid_vtxlp = bidvtxlp;
}

/*
 * allocate a tran graph edge (npp tran gate)
 */
static struct edge_t *alloc_edge(struct net_pin_t *npp)  
{
 struct edge_t *ep;

 if (__edge_freelst != NULL)
  { ep = __edge_freelst; __edge_freelst = __edge_freelst->enxt; }
 else ep = (struct edge_t *) __my_malloc(sizeof(struct edge_t));

 ep->enpp = npp;
 ep->edgoside_itp = NULL;
 ep->ev2 = NULL;
 ep->enxt = NULL;
 return(ep);
}

/*
 * return T if bit inout only channel, has all same wire types
 */
static void chkchannel(struct chanrec_t *chanp)
{
 register struct edge_t *ep;
 int32 bi, chanid, base;
 struct vtx_t *vtxp;
 struct traux_t *trap;
 struct net_t *np;

 vtxp = chanp->chvxp;
 __push_itstk(chanp->chvitp);
 np = vtxp->vnp;
 /* DBG remove -- */
 if ((trap = np->ntraux) == NULL) __misc_terr(__FILE__, __LINE__);
 /* --- */ 
 bi = vtxp->vi1;
 if (bi == -1) bi = 0;
 base = __inum*np->nwid;
 chanid = trap->vbitchans[base + bi]->chan_id;
 /* DBG remove -- */
 if (&(__chantab[chanid]) != chanp) __misc_terr(__FILE__, __LINE__);
 /* --- */ 
 vtxp->vtx_mark = TRUE;

 for (ep = vtxp->vedges; ep != NULL; ep = ep->enxt)
  {
   if (ep->ev2->vtx_mark) continue;
   ep->ev2->vtx_mark = TRUE;
   chkchan_edge(chanp, ep);
  }
 /* SJM 09/11/00 - not set ch typ even if this not called */
 __pop_itstk();

 /* off vertex marks, to be ready for next traversal */
 off_bichan_marks(chanp);
}

/*
 * routine to turn off all vertex marks for
 */
static void off_bichan_marks(struct chanrec_t *chanp) 
{
 register struct vtxlst_t *vtxlp;

 add_vtx_to_offlist(chanp->chvxp);
 
 /* turn of marks in all vertices on list */
 for (vtxlp = __off_vtxlst_hdr; vtxlp != NULL; vtxlp = vtxlp->vtxnxt)
  {
   vtxlp->vtxp->vtx_mark = FALSE;
   vtxlp->vtxp->vtx_mark2 = FALSE;
  }

 /* then add entire list to end of free list */
 if (__off_vtxlst_hdr != NULL)
  {
   __off_vtxlst_end->vtxnxt =  __vtxlst_freelst; 
   __vtxlst_freelst = __off_vtxlst_hdr;
  }
 __off_vtxlst_hdr = __off_vtxlst_end = NULL;
} 

/*
 * add all vertices to list using vtx mark2 for cutting cyles
 * also used as first step in freeing channel
 */
static void add_vtx_to_offlist(struct vtx_t *vtxp)
{
 register struct vtxlst_t *vtxlp;
 register struct edge_t *ep;

 /* alloc and add to list */
 if (__vtxlst_freelst == NULL)
  vtxlp = (struct vtxlst_t *) __my_malloc(sizeof(struct vtxlst_t));
 else
  {
   vtxlp = __vtxlst_freelst;
   __vtxlst_freelst = __vtxlst_freelst->vtxnxt;
  }
 /* add it */
 vtxlp->vtxp = vtxp;
 vtxlp->vtxnxt = NULL;
 vtxp->vtx_mark2 = TRUE; 

 /* put on end */
 if (__off_vtxlst_end != NULL)
  { __off_vtxlst_end->vtxnxt = vtxlp; __off_vtxlst_end = vtxlp; }
 else __off_vtxlst_end = __off_vtxlst_hdr = vtxlp;

 /* all all other side vertices of this one's edges */ 
 for (ep = vtxp->vedges; ep != NULL; ep = ep->enxt)
  {
   if (ep->ev2->vtx_mark2) continue;

   add_vtx_to_offlist(ep->ev2);
  }
}

/*
 * check edge and vertex on other side and update chanp fields
 * 09/11/00 - SJM - DBG now only called if debug flag on
 */
static void chkchan_edge(struct chanrec_t *chanp, struct edge_t *ep)
{
 struct vtx_t *vtxp;
 struct net_t *np;
 struct edge_t *ep2;

 vtxp = ep->ev2;
 np = vtxp->vnp;
 /* DBG remove -- */
 if (np->ntraux == NULL) __misc_terr(__FILE__, __LINE__);
 if (chanp->chvxp->vnp->n_stren != np->n_stren)
  __misc_terr(__FILE__, __LINE__);
 /* --- */ 
 if (chanp->chvxp->vnp->ntyp != np->ntyp)
  {
   if (chanp->chan_diffwtyps != TRUE) __misc_terr(__FILE__, __LINE__); 
  }
 
 /* depth first process vertices edges */
 /* back edge vertex already part of processed tree */
 for (ep2 = vtxp->vedges; ep2 != NULL; ep2 = ep2->enxt)
  {
   if (ep2->ev2->vtx_mark) continue;
   ep2->ev2->vtx_mark = TRUE;
   chkchan_edge(chanp, ep2);
  }
}

/*
 * finished with tran soft drivers remove from in tran channel
 * np list in traux
 *
 * know one traux per net with any bits in tran channel
 * SJM 05/15/99 - move tran channel npps to traux - needed for PLI
 */
static void save_bidandtran_npps(void)
{
 register struct net_pin_t *npp;
 register int32 ni;
 register struct mod_t *mdp;
 register struct net_t *np;
 struct net_pin_t *last_npp, *npp2;

 for (mdp = __modhdr; mdp != NULL; mdp = mdp->mnxt)
  {
   if (mdp->mnnum == 0) continue;
   for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
    {  
     for (last_npp = NULL, npp = np->ndrvs; npp != NULL;)
      {
       /* save next field */
       npp2 = npp->npnxt;
       if (npp->npntyp == NP_BIDMDPRT || npp->npntyp == NP_BIDICONN
        || npp->npntyp == NP_TRAN) 
        {
         /* link out but pointed to by graph edges so cannot free */
         if (last_npp == NULL) np->ndrvs = npp->npnxt;
         else last_npp->npnxt = npp->npnxt;

         /* and link onto front of tran channel npp list in traux */
         /* for first one, sets nil then it migrates to end */ 
         npp->npnxt = np->ntraux->tran_npps;
         np->ntraux->tran_npps = npp;
        }
       else last_npp = npp; 
       npp = npp2;
      }
    }
  }
}

/*
 * ROUTINES TO EVALUATE AND STORE TRAN CHANNEL AND INOUT WIRE GRAPHS
 */

/*
 * evaluate stren tran channels including inouts during initialization
 * 
 * non stren wires do not need to be evaluated since z unless driver
 * causes change but when initializing driver will cause re-eval 
 * for rtran since everything z to start attenuation can't go weaker than z 
 *
 * also since any driver change will cause trnva change, can use initialized
 * values do not need to reevaluate drivers
 *
 * must do for every instance of modules
 * notice evaluating each channel (all vertices) only once
 * LOOKATME - for now initializing all channels, but think only some needed
 * maybe not since need all trans for sure here and if inout only has
 * pull on it will never be evaluated from gate evaluation
 */
extern void __init_all_trchans(void) 
{
 register int32 ii;
 int32 tot_tran_vtxs, tot_bid_vtxs, num_tran_done, num_bid_done;
 struct chanrec_t *chanp;
 struct vtxlst_t *vtxlp, *vtxlp2;

 if (__switch_verbose && __nxt_chan_id > 0)
  {
   __cv_msg("  SWITCH: begin switch channel initialization (%d channels).\n",
    __nxt_chan_id);
  }

 /* if no tran in design nothing to do here */
 if (__nxt_chan_id == 0) return;

 tot_tran_vtxs = tot_bid_vtxs = 0;
 num_tran_done = num_bid_done = 0;
 /* first initialize all tran(if) chans using relaxation of entire chan */ 
 for (ii = 0; ii < __nxt_chan_id; ii++)
  {
   chanp = &(__chantab[ii]);
   if (chanp->chtyp == TRPROC_STWTYPBID || chanp->chtyp == TRPROC_TRAN)
    {
     init_sttranif_chan(chanp);
     tot_tran_vtxs += chanp->numvtxs;
     num_tran_done++;
    }
   else
    {
     if (chanp->chtyp == TRPROC_BID) eval_assign_bid_chan(chanp);
     else eval_assign_stbid_chan(chanp);
     tot_bid_vtxs += chanp->numvtxs;
     num_bid_done++;
    }

   /* SJM only emit message if switch verbose and some channels inited */
   if (__switch_verbose && ((num_tran_done + num_bid_done) % 5000) == 0)
    {
     double d1, d2;

     d1 = 100.0*(((double) num_tran_done)/((double) __num_switch_chans));
     d2 = 100.0*(((double) num_bid_done)
      / ((double) __nxt_chan_id - __num_switch_chans));
     __cv_msg(
     "  SWITCH: %d (%.2f%%) tran and %d (%.2f%%) inout channels completed.\n",
     num_tran_done, d1, num_bid_done, d2);
    }
  } 
 if (__switch_verbose)
  { 
   if (__num_switch_chans == 0)
    { __cv_msg("  SWITCH: design contains no tran(if) switch channels.\n"); }
   else
    {
     __cv_msg(
      "  SWITCH: tran channels initialized (%d containing %d nodes in design).\n",
      __num_switch_chans, tot_tran_vtxs);
    }
   if (__num_switch_chans == __nxt_chan_id)
    { __cv_msg("  SWITCH: design contains no inout switch channels.\n"); }
   else
    {
     __cv_msg(
      "  SWITCH: inout channels initialized (%d containing %d nodes in design).\n",
      __nxt_chan_id - __num_switch_chans, tot_bid_vtxs); 
    }
  }

 /* when done with initialization full relax, reclaim storage */
 /* since lists will be much shorter from now on */
 for (vtxlp = __vtxlst_freelst; vtxlp != NULL;)
  {
   vtxlp2 = vtxlp->vtxnxt;
   __my_free((char *) vtxlp, sizeof(struct vtxlst_t));
   vtxlp = vtxlp2;
  }
 __vtxlst_freelst = NULL;
}

/*
 * initialize entire tran(if) channel at once (relax the whole thing)
 */
static void init_sttranif_chan(struct chanrec_t *chanp)
{
 /* add all vertices to st vtx tab lists indexed by low stren */
 /* starts with the one random chan vertex (since digraph any works) */
 __push_itstk(chanp->chvitp);
 init_add_vtx_and_subtree(chanp->chvxp, chanp->chvitp);
 __pop_itstk();

 /* now that all vertices in st vtx tab, turn off marks so can be used again */
 /* if needed - mostly for dumping channels */
 /* works in st vtx tab - know all vertices in it */
 off_stvtxtab_marks();
 
 stchan_trif_relax();

 /* final step, assign all changed vertices */
 if (__chg_vtxlst_hdr != NULL) assign_chged_vtxs();
}

/*
 * routine to add all vertices to st vtx tab lists indexed by low stren
 * also set internal tran vertex value from net(bit) driving value
 * works because force release not possible before here
 *
 * NOTICE - this must be called after all normal initialization
 * LOOKATME - think it pushing does not work for inout port edges
 */
static void init_add_vtx_and_subtree(struct vtx_t *vtxp,
 struct itree_t *vt1_itp)
{
 register struct edge_t *ep;
 int32 bi, inum;
 word32 vtxval, lowst, st0, st1;
 struct vtxlst_t *vtxlp;
 struct net_t *np; 
 struct itree_t *oside_itp;

 np = vtxp->vnp;
 bi = vtxp->vi1;
 bi = (bi == -1) ? 0 : bi;

 inum = vt1_itp->itinum;
 /* save current value of net as old vtx value */
 vtxp->old_vtxval = np->nva.bp[inum*np->nwid + bi];

 /* get switch contribution from hard driver's value */
 vtxval = (word32) np->ntraux->trnva.bp[inum*np->nwid + bi];

 /* and use as first new channel value guess */
 vtxp->new_vtxval = (byte) vtxval;
 if (vtxval == ST_SUPPLY0 || vtxval == ST_SUPPLY1) vtxp->vtx_supply = TRUE;

 /* get low strength */
 st1 = (vtxval >> 2) & 0x7;
 st0 = (vtxval >> 5) & 0x7;
 lowst = (int32) ((st1 < st0) ? st1 : st0);

 if (__vtxlst_freelst == NULL)
  vtxlp = (struct vtxlst_t *) __my_malloc(sizeof(struct vtxlst_t));
 else
  {
   vtxlp = __vtxlst_freelst;
   __vtxlst_freelst = __vtxlst_freelst->vtxnxt;
  }
 vtxlp->vtxp = vtxp;
 vtxlp->vtx_itp = vt1_itp;
 vtxlp->vtxnxt = NULL;
 if (__stvtxtabend[lowst] != NULL)
  { __stvtxtabend[lowst]->vtxnxt = vtxlp; __stvtxtabend[lowst] = vtxlp; }
 else __stvtxtab[lowst] = __stvtxtabend[lowst] = vtxlp;

 vtxp->vtx_mark = TRUE;
 __num_switch_vtxs_processed++;

 /* DBG remove -- */ 
 if (__debug_flg && __ev_tracing)
  {
   __dbg_msg("** vertex %s.%s level %d added to perturb list\n",
    __msg2_blditree(__xs, vtxlp->vtx_itp), to_vtx(__xs2, vtxp), lowst);
  }

 for (ep = vtxp->vedges; ep != NULL; ep = ep->enxt)
  {
   if (ep->ev2->vtx_mark) continue;
   
   /* SJM - 05/21/01 - compute oside edge and use call stack since itstk */
   /* not deep enough */
   if (ep->edgoside_itp != NULL) oside_itp = ep->edgoside_itp;
   else oside_itp = vt1_itp;
   
   init_add_vtx_and_subtree(ep->ev2, oside_itp);
  }
}

/*
 * routine to turn off all mark in vertices in st vtx tab
 *
 * needed to save marking memory (8 byte per vtx) and now uses just 2 bits 
 * but need to build and free list of all vertices in channel when done
 * 
 * done after all verties are added to tab for intial relax - since marks
 * not needed in relax can turn off using full st vtx tab
 */
static void off_stvtxtab_marks(void)
{
 register int32 si;
 register struct vtxlst_t *vtxlp;
 
 for (si = 7; si >= 0; si--)
  {
   for (vtxlp = __stvtxtab[si]; vtxlp != NULL; vtxlp = vtxlp->vtxnxt)
    {
     vtxlp->vtxp->vtx_mark = FALSE;
     vtxlp->vtxp->vtx_mark2 = FALSE;
    }
  }
}

/*
 * TOP LEVEL ROUTINES TO EVAL TRAN CHANNEL
 */

/*
 * evaluate tran channels bit by bit - process for exactly one itree loc.
 *
 * bits may be part of empty tran channel (no trans or inouts on chan)
 * in which case just store/schedule the trnva hard driver value
 *
 * for tran or inout, some hard driver (bid. tran chan in) must have
 * changed to cause evaluation of channel, except for initialization,
 * any fi>1 eval. goes through here if wire in tran channel
 */
extern void __eval_tran_bits(register struct net_t *np)
{
 register int32 bi;
 int32 base;
 word32 nav, nbv;
 struct traux_t *trap;
 struct vbinfo_t *vbip;
 struct chanrec_t *chanp;

 /* SJM 04/11/01 - can't do incremental relaxation until wire init done */
 if (__wire_init) return;

 trap = np->ntraux;
 /* SJM 04/23/01 - all channels now per inst, no extra cost for 1 inst mod */
 base = __inum*np->nwid;

 /* all others are per bit */
 for (bi = np->nwid - 1; bi >= 0; bi--) 
  {
   /* bit of wire is not really in any tran channel - treated as 1 wire */
   /* tran channel - just access stored internal hard trnva value */
   if ((vbip = trap->vbitchans[base + bi]) == NULL)
    {
     /* 03/15/00 - SJM - if forced getting drivers ok since st vtx inhibits */
     /* actual assign */
     if (np->n_stren)
      {
       nav = (word32) trap->trnva.bp[__inum*np->nwid + bi];
       /* really unused but passing it */
       nbv = 0;
      }
     else ld_vtx_netbit(&nav, &nbv, np, bi);
     /* notice if strength, nbv not used */
     st_vtx_netbit(np, bi, nav, nbv);
    }
   else
    {
     chanp = &(__chantab[vbip->chan_id]);
     /* BEWARE when this bit on, vb info vtx field invalid (can't use) */
     if (chanp->chan_no_vtxs)
      {
       if (chanp->chtyp == TRPROC_BID) eval_assign_bid_chan(chanp);
       else eval_assign_stbid_chan(chanp);
      }
     /* real tran switch chan - use change vertex to perturb and relax chan */
     else eval_update_1w_tranchan(vbip->vivxp);
    } 
  }
}

/*
 * evaluate one tran channel bit - version for path dest. event 
 * so know always per inst form
 * just one bit part of all bits routine
 */
extern void __eval_tran_1bit(register struct net_t *np, register int32 bi)
{
 int32 base;
 word32 nav, nbv;
 struct chanrec_t *chanp;
 struct traux_t *trap;
 struct vbinfo_t *vbip;

 /* SJM 04/11/01 - can't do incremental relaxation until wire init done */
 if (__wire_init) return;

 trap = np->ntraux;
 /* SJM - 12/19/00 - new using 1 bit form in many place so must set */ 
 /* per instance form because if all in one inst. not per inst*/
 base = __inum*np->nwid;

 /* bit of wire is not really in any tran channel - treated as 1 wire */
 /* tran channel - just access stored internal hard trnva value */
 if ((vbip = trap->vbitchans[base + bi]) == NULL)
  {
   /* 03/15/00 - SJM - if forced getting drivers ok since st vtx inhibits */
   /* actual assign */
   if (np->n_stren)
    {
     nav = (word32) trap->trnva.bp[__inum*np->nwid + bi];
     /* really unused but passing it */
     nbv = 0;
    }
   else ld_vtx_netbit(&nav, &nbv, np, bi);
   /* notice if strength, nbv not used */
   st_vtx_netbit(np, bi, nav, nbv);
  }
 else
  {
   chanp = &(__chantab[vbip->chan_id]);
   /* BEWARE when this bit on, vb info vtx field invalid (can't use) */
   if (chanp->chan_no_vtxs)
    {
     if (chanp->chtyp == TRPROC_BID) eval_assign_bid_chan(chanp);
     else eval_assign_stbid_chan(chanp);
    }
   /* real tran switch chan - use change vertex to perturb and relax chan */
   else eval_update_1w_tranchan(vbip->vivxp);
  } 
}

/*
 * update tran and/or inout channel changed wire hard drivers (chan ins)
 * returns T if hard drivers of wire changed, F if unchanged
 *
 * called from target (definition) changed instance 
 * for special wires (stren only) such as supply0 mdr load use wire stren
 * this only changes entire nets trnva per instance hard drivers
 * update the hard driver for this net-instance
 *
 * know driver of np changed, but no other drivers therefore
 * only need to update hard drivers for the one net but entire channel
 * anywhere in it can change - must update all of wire
 * if no hard driver change, returns F, channel cannot change
 * if only conducting state of tranif change no need to eval. driver
 * but must always re-eval all chan wires
 *
 * notice for rare bits of wire that are not part of tran channel
 * they are part of empty tran channel and can just store the saved
 * trnva value - since must eval. entire wire drivers will have those bits
 * i.e. this is not bit by bit
 */
extern int32 __update_tran_harddrvs(struct net_t *np)
{
 register byte *sbp, *sbp2;
 register struct xstk_t *xsp, *xsp2;

 sbp = NULL;
 if (np->n_stren)
  {
   /* this will make chan wire section of input drivers have wire type */
   /* i.e. supply will probably override its drivers */
   xsp = __stload_mdrwire(np);
   sbp = (byte *) xsp->ap;
   /* SJM - 03/15/01 - even if forced wtill need to update hard drvrs */
   /* in case ever released - just do not use them when force in effect */
   sbp2 = &(np->ntraux->trnva.bp[__inum*np->nwid]);

   /* if this is path dest. and part of inout tran channel, must sched */  
   /* change to internal stored hard driver values */
   /* some bits may need immed. assign - routine handles updating tran chan */
   if (np->iotyp == IO_BID && np->n_isapthdst && !__wire_init)
    {
     stren_schd_bidpthdrvrs(np, sbp, sbp2);
     __pop_xstk();
     /* for any real changes this re-evals 1 bit of channel */
     /* therefore return F to stop another chan re-eval and store vtx */
     return(FALSE);
    }
   /* SJM 11/24/00 - for path destinations can't eliminate schedule using */ 
   /* current switch channel state because there may be pending event that */
   /* requires inertial rescheduling */

   /* SJM 12/13/00 malloc lib fails when bcmp of 1 byte scalar */
   if (np->nwid == 1)
    {
     if (sbp[0] != sbp2[0]) sbp2[0] = sbp[0]; 
     goto chg;
   }

   /* if all changed drivers (tran channel input) same, nothing to do */
   if (memcmp(sbp, sbp2, np->nwid) == 0) goto no_chg;

   /* update this wire drvrs (chan in sect), copy 2nd arg is dest. */
   memcpy(sbp2, sbp, np->nwid);
   /* now done with sbp and xsp */ 
   goto chg; 
  }

 xsp = __load_mdrwire(np);
 /* but must update driving value fron tran channel of xmr from inst */
 push_xstk_(xsp2, np->nwid);
 __ld_perinst_val(xsp2->ap, xsp2->bp, np->ntraux->trnva, np->nwid);

 /* some bit changed update or schedule path dest. per bit channels */
 if (np->iotyp == IO_BID && np->n_isapthdst && !__wire_init)
  {
   schd_bidpthdrvrs(np, xsp, xsp2);
   __pop_xstk();
   __pop_xstk();
   /* for any real changes this re-evals 1 bit of channel */
   /* therefore return F to stop another chan re-eval and store vtx */
   return(FALSE);
  }

 /* SJM 11/24/00 - also for non stren case, must always use inertial */ 
 /* rescheduling on inout paths */
 if (np->nwid <= WBITS)
  {
   if (xsp->ap[0] == xsp2->ap[0] && xsp->bp[0] == xsp2->bp[0]) goto no_chg;
  }
 else
  {
   if (cmp_vval_(xsp->ap, xsp2->ap, np->nwid) == 0
    && cmp_vval_(xsp->bp, xsp2->bp, np->nwid) == 0) goto no_chg;
  }

 /* update ins (drvs) for this tran chan wire section */ 
 __st_perinst_val(np->ntraux->trnva, np->nwid, xsp->ap, xsp->bp);
chg:
 if (__ev_tracing)
  {
   char s1[RECLEN], s2[RECLEN], s3[RECLEN];

   if (np->n_stren)
    { strcpy(s1, " strength"); __st_regab_tostr(s2, sbp, np->nwid); }
   else
    {
     strcpy(s1, "");
     __regab_tostr(s2, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE);
    }
   __tr_msg(
    "-- driver (hard tran input) of%s channel %s %s changed new value %s\n",  
    s1, __to_wtnam(s3, np), np->nsym->synam, s2);
  }
 __pop_xstk();
 if (!np->n_stren) __pop_xstk();
 return(TRUE);

no_chg:
 if (__ev_tracing)
  {
   char s1[RECLEN], s2[RECLEN], s3[RECLEN];

   if (np->n_stren)
    { strcpy(s1, " strength"); __st_regab_tostr(s2, sbp, np->nwid); }
   else
    {
     strcpy(s1, "");
     __regab_tostr(s2, xsp->ap, xsp->bp, xsp->xslen, BHEX, FALSE);
    }
   __tr_msg(
    "-- a driver (tran channel input) of%s channel %s %s changed but value %s unchanged\n",  
    s1, __to_wtnam(s3, np), np->nsym->synam, s2);
  }
 __pop_xstk();
 if (!np->n_stren) __pop_xstk();
 return(FALSE);
}

/*
 * schedule stren inout channel path destination hard drivers and channel
 * some bits may not have changed or need immediate assign and chan update
 */
static void stren_schd_bidpthdrvrs(struct net_t *np, byte *drv_sbp, 
 byte *trnva_sbp)
{
 register int32 bi; 
 int32 nd_chan_upd;
 i_tev_ndx *itevpi;
 struct rngdwir_t *dwirp; 

 dwirp = np->nu.rngdwir;
 itevpi = &(dwirp->wschd_pbtevs[__inum*np->nwid]);
 for (bi = 0; bi < np->nwid; bi++)
  {
   __new_gateval = (word32) drv_sbp[bi];
   __old_gateval = (word32) trnva_sbp[bi];

   if (__ev_tracing) nd_chan_upd = evtr_schd_1bitpthdrvr(np, bi, itevpi);
   else nd_chan_upd = schd_1bitpthdrvr(np, bi, itevpi);

   if (nd_chan_upd)
    {
     /* update stren tran driver and re-eval and store channel new value */ 
     trnva_sbp[bi] = (byte) __new_gateval;
     __eval_tran_1bit(np, bi);
    }
  }
}

/*
 * schedule non stren inout channel path dest. hard drivers and channel
 * some bits may not have changed or need immediate assign and chan update
 */
static void schd_bidpthdrvrs(struct net_t *np, struct xstk_t *drv_xsp,
 struct xstk_t *trnva_xsp)
{
 register int32 bi; 
 register word32 tmp;
 int32 nd_chan_upd;
 i_tev_ndx *itevpi;
 struct rngdwir_t *dwirp; 

 dwirp = np->nu.rngdwir;
 itevpi = &(dwirp->wschd_pbtevs[__inum*np->nwid]);
 for (bi = 0; bi < np->nwid; bi++)
  {
   tmp = rhsbsel_(drv_xsp->bp, bi);
   __new_gateval = (tmp << 1) | (rhsbsel_(drv_xsp->ap, bi));
   tmp = rhsbsel_(trnva_xsp->bp, bi);
   __old_gateval = (tmp << 1) | (rhsbsel_(trnva_xsp->ap, bi));

   if (__ev_tracing) nd_chan_upd = evtr_schd_1bitpthdrvr(np, bi, itevpi);
   else nd_chan_upd = schd_1bitpthdrvr(np, bi, itevpi);

   if (nd_chan_upd)
    {
     /* update stren tran driver and re-eval and store channel new value */ 
     __lhsbsel(trnva_xsp->ap, bi, (__new_gateval & 1L));
     __lhsbsel(trnva_xsp->bp, bi, ((__new_gateval >> 1) & 1L));
     __st_perinst_val(np->ntraux->trnva, np->nwid, trnva_xsp->ap,
      trnva_xsp->bp);
     __eval_tran_1bit(np, bi);
    }
  }
}

/*
 * schedule 1 bit path bidirect driver
 *
 * show cancel e analysis including non inout path distributed delay
 *
 * this works for both strength 8 bit nval and oval and non strength
 * if scalar bi must be 0 (i.e. biti can not be -1)
 * nval is new value to schedule change to, old value is current wire value
 *
 * old and new gate values in globals - maybe changed since caller saves
 */
static int32 schd_1bitpthdrvr(struct net_t *np, register int32 biti,
 i_tev_ndx *itevpi)
{
 register word32 nval, oval;
 word32 is_stren;
 word64 schtim;
 i_tev_ndx tevpi;
 struct rngdwir_t *dwirp; 
 struct pthdst_t *pdp;
 struct tev_t *tevp;

 dwirp = np->nu.rngdwir;
 tevpi = itevpi[biti];

 /* DBG remove ---
 if (tevpi != -1 && __tevtab[tevpi].tetyp != TE_BIDPATH)
  __misc_terr(__FILE__, __LINE__);
 --- */

 nval = __new_gateval;
 oval = __old_gateval; 

 /* since always use last changed value, if last same as current */
 /* because gate style glitch nothing to do since already right value */   
 if (tevpi == -1 && nval == oval)
  return(FALSE);

 is_stren = np->n_stren;

 /* possible for some bits to not be path destinations */
 if (oval != nval)
  {
   if ((pdp = __get_path_del(dwirp, biti, &schtim)) == NULL)
    return(TRUE);   
  }
 else { pdp = NULL; schtim = 0ULL; }

 /* special case 0 - distributed delay longer - immediate assign */
 /* normal cause is path (probably from multiple input final driving gate) */
 /* that has not path delay on it - this may be ok */
 if (pdp != NULL && schtim <= __simtime)
  {
   /* problem with modeling - distributed delay longer than path */
   if (!__no_informs) __emit_path_distinform(np, pdp, &__pdmindel);

   /* modeling anomally style spike possible - know immed. assign earlier */ 
   if (tevpi != -1)
    {
     /* calls here take ptr not index */
     tevp = &(__tevtab[tevpi]); 
     if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
      {
       __emit_path_pulsewarn(pdp, tevp, &__simtime, &(tevp->etime),
        "distributed longer or no path delay", is_stren);
      }
     /* always cancel pending */
     __cancel_1wev(tevp);
     itevpi[biti] = -1;

     /* this is same for on detect and on event */
     if (__show_cancel_e)
      {
       /* this is special case where immediate assign must be to x */ 
       /* and cancel future event that can be scheduled for now */
set_on_detect_x:
       /* set global causes use in tran channel re-eval */
       if (is_stren) __new_gateval = (word32) ST_STRONGX;
       else __new_gateval = (word32) 3;
       return(TRUE);
      }
     /* if no show canceled e, just assign later */  
    }
   /* no schedule, distributed longer - global new gateval right and used */
   return(TRUE);
  }

 /* no pending event */
 /* SJM 11/24/00 - know if no pending event will have path */
 if (tevpi == -1)
  {
   /* because no pending event must be different */
   __schedule_1wev(np, biti, TE_BIDPATH, __pdmindel, schtim, nval,
    itevpi, FALSE);
   return(FALSE);
  }    
 /* pending event */
 tevp = &(__tevtab[tevpi]); 
 /* new and old same but know scheduled different - classic pulse/glitch */
 if (nval == oval)
  {
   /* have delay to use to select pa0th */  
   if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
    {
     __emit_path_samewarn(np, biti, tevp, &(tevp->etime), "pulse",
      is_stren);
    }
   
   /* if spike, suppress future but schedule to x at currently scheduled */
   if (__show_cancel_e)
    {
     if (__showe_onevent)
      { tevp->outv = (is_stren) ? ST_STRONGX : 3; return(FALSE); }

     __cancel_1wev(tevp);
     itevpi[biti] = -1;
     goto set_on_detect_x;
    }
   /* remove pulse */
   __cancel_1wev(tevp);
   itevpi[biti] = -1;
   return(FALSE);
  }

 /* now know pdp set */

 /* new schedule to same value case */
 /* here delay can be different because different path selected */
 /* and maybe other reasons */
 /* done silently here - trace message only below */
 if (tevp->outv == (byte) nval) return(FALSE);

 /* inertial reschedule */
 if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
  __emit_path_pulsewarn(pdp, tevp, &(tevp->etime), &schtim,
   "inout unstable", is_stren);   

 /* easy show cancel (set to x case) - no new event may or may not switch */
 if (__show_cancel_e)
  {
   /* LOOKATME - maybe need to check old tevp and new schd time and if 2nd */
   /* input change results in earlier edge cancel and schedule earlier */
   if (__showe_onevent)
    { tevp->outv = (is_stren) ? ST_STRONGX : 3; return(FALSE); }

   __cancel_1wev(tevp);
   itevpi[biti] = -1;
   goto set_on_detect_x;
  }
 /* inertial reschedule, this handles cancel if needed */
 __reschedule_1wev(tevpi, nval, __pdmindel, schtim, itevpi);
 return(FALSE);
}

/*
 * tracing version of schedule stren 1 bit inout channel path dest drivers
 *
 * show cancel e analysis including non inout path distributed delay
 *
 * this works for both strength 8 bit nval and oval and non strength
 * if scalar bi must be 0 (i.e. biti can not be -1)
 * nval is new value to schedule change to, old value is current wire value
 *
 * old and new gate values in globals - maybe changed since caller saves
 */
static int32 evtr_schd_1bitpthdrvr(struct net_t *np, register int32 biti,
 i_tev_ndx *itevpi)
{
 register word32 nval, oval;
 i_tev_ndx tevpi;
 word32 is_stren;
 word32 outval;
 word64 schtim, distdel, tevptim;
 struct rngdwir_t *dwirp; 
 struct pthdst_t *pdp;
 struct spcpth_t *pthp;
 struct tev_t *tevp;
 char s1[RECLEN], s2[RECLEN], vs1[10], vs2[10], vs3[10];

 is_stren = np->n_stren;
 dwirp = np->nu.rngdwir;
 tevpi = itevpi[biti];
 nval = __new_gateval;
 oval = __old_gateval; 

 if (tevpi != -1)
  {
   /* DBG remove --- */
   if (__tevtab[tevpi].tetyp != TE_BIDPATH) __misc_terr(__FILE__, __LINE__); 
   /* --- */
   strcpy(s1, " (pending event)");
  }
 else strcpy(s1, "");

 /* if no change and do not need schedule time for cancel, done */
 __tr_msg("-- path delay inout destination %s driver change%s now %s:\n",
  __to_evtrwnam(__xs, np, biti, biti, __inst_ptr), s1,
  __to_timstr(__xs2, &__simtime));

 /* since always use last changed value, if last same as current */
 /* because gate style glitch nothing to do since already right value */   
 if (tevpi == -1 && nval == oval)
  {
   __tr_msg(" PATHDEL, NOCHG <OV=%s> at %s\n", __to_vnam(vs1, is_stren, nval),
    __to_timstr(__xs, &__simtime));
   return(FALSE);
  }

 /* possible for some bits to not be path desitinations - just immed assign */
 if (nval != oval)
  {
   if ((pdp = __get_path_del(dwirp, biti, &schtim)) == NULL)   
    {
     __tr_msg(" BIT %d NOT PATH DEST: IMMED ASSIGN <OV=%s, NV=%s>\n",
      biti,__to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren, nval));
     return(TRUE);
    } 
   pthp = pdp->pstchgp->chgu.chgpthp;
   __tr_msg(" PATH (at line %s) SRC CHG TIME %s\n",
    __bld_lineloc(s1, pthp->pthsym->syfnam_ind, pthp->pthsym->sylin_cnt),
    __to_timstr(__xs, &__pdlatechgtim));
  }
 else { pdp = NULL; schtim = 0ULL; }

 /* special case 0 - distributed delay longer - immediate assign */
 if (schtim <= __simtime)
  {
   /* problem with modeling - distributed delay longer than path */
   /* or changed path has no path delay */
   if (!__no_informs && pdp != NULL)
     __emit_path_distinform(np, pdp, &__pdmindel);

   /* modeling anomally style spike possible - know immed. assign earlier */ 
   if (tevpi != -1)
    {
     /* most routines here need ptr to event not index */
     tevp = &(__tevtab[tevpi]); 
     if (__warn_cancel_e && !__no_warns && !__em_suppr(592) && pdp != NULL)
      __emit_path_pulsewarn(pdp, tevp, &__simtime, &(tevp->etime),
       "distributed longer or no path delay", is_stren);

     /* always cancel pending */
     outval = (word32) tevp->outv;
     tevptim = tevp->etime;
     /* always cancel pending */
     __cancel_1wev(tevp);
     itevpi[biti] = -1;

     /* this is same for on detect and on event since immed. assign */
     if (__show_cancel_e)
      {
       /* this is special case where immediate assign must be to x */ 
       /* and cancel future event that can be scheduled for now */
       __tr_msg(
        " INOUT PATH, DIST DELAY PULSE <OV=%s, OSV=%s at %s NV=%s SHOWING X FROM NOW>\n",
        __to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren, outval),
        __to_timstr(s1, &tevptim), __to_vnam(vs3, is_stren, nval));
set_on_detect_x:
       if (is_stren) __new_gateval = (word32) ST_STRONGX;
       else __new_gateval = (word32) 3;
       return(TRUE);
      }
     __tr_msg(
      " INOUT PATH, DIST DELAY PULSE <OV=%s, OSV=%s at %s - NV=%s ASSIGN AND CANCEL>\n",
      __to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren,
      (word32) tevp->outv), __to_timstr(s1, &(tevp->etime)),
      __to_vnam(vs3, is_stren, nval));
     /* no schedule, distributed delay longer - new gate val used for chan. */
     return(TRUE); 
    }

   /* know if no pending event, pdp not nil */
   /* no pending event store - know must be different */
   distdel = __simtime - __pdlatechgtim;
   __tr_msg(
    " DIST DELAY %s LONGER THAN INOUT PATH %s: IMMED ASSIGN <OV=%s, NV=%s>\n",
    __to_timstr(__xs2, &distdel), __to_timstr(s1, &__pdmindel), 
    __to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren, nval));
   /* use new gateval in tran channel eval */
   return(TRUE);
  }

 /* real path delay */
 /* case 1: no pending event */
 /* know if no pending event, pdp not nil */
 if (tevpi == -1)
  {
   /* because no pending event must be different */
   __tr_msg(" PATH DEL, SCHD AT %s <OV=%s, NSV=%s>\n",
    __to_timstr(s1, &schtim), __to_vnam(vs1, is_stren, oval),
    __to_vnam(vs2, is_stren, nval));
   __schedule_1wev(np, biti, TE_BIDPATH, __pdmindel, schtim, nval,
    itevpi, FALSE);
   return(FALSE);
  }    

 /* pending event */
 tevp = &(__tevtab[tevpi]); 
 /* new and old same but know scheduled different - classic pulse/glitch */
 if (nval == oval)
  {
   /* preform show cancel e analysis, know scheduled different - tell user */
   /* this is classical spike analysis */
   if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
    {
     __emit_path_samewarn(np, biti, tevp, &(tevp->etime), "pulse",
      is_stren);
    }

   /* if spike, suppress future but schedule to x at currently scheduled */
   if (__show_cancel_e)
    {
     if (__showe_onevent) sprintf(s1, "%s (on event)", __to_timstr(__xs,
      &(tevp->etime)));
     else sprintf(s1, "%s (on detect)", __to_timstr(__xs, &__simtime));

     /* LOOKATME - think on event pulse should use schedule if earlier? */
     __tr_msg(
      " INOUT PATH DEL, PEND AT %s, PULSE <OV=NSV=%s, OSV=%s SHOWING X FROM %s>\n",
      __to_timstr(__xs, &(tevp->etime)), __to_vnam(vs1, is_stren, oval),
      __to_vnam(vs2, is_stren, (word32) tevp->outv), s1);

     if (__showe_onevent)
      { tevp->outv = (is_stren) ? ST_STRONGX : 3; return(FALSE); }

     __cancel_1wev(tevp); 
     itevpi[biti] = -1;
     goto set_on_detect_x;
    }
   /* remove pulse */
   __tr_msg(
    " INOUT PATH DEL, PEND, PULSE, INERTIAL CANCEL AT %s <OV=%s, OSV=%s>\n",
    __to_timstr(s1, &(tevp->etime)), __to_vnam(vs1, is_stren, oval),
    __to_vnam(vs2, is_stren, (word32) tevp->outv));
   /* no spike, but newly scheduled to same so no event */
   __cancel_1wev(tevp);
   itevpi[biti] = -1;
   return(FALSE);
  }

 /* form here on know pdp set */
 /* new schedule to same value case */
 /* know that delay same and later so just discard new event */
 /* done silently here - trace message only */
 if (tevp->outv == (byte) nval)
  {
   __tr_msg(
    " INOUT PATH DEL, MODEL ANOMALLY IGNORE SCHED TO SAME <OSV=NSV=%s> OLD AT %s NEW %s\n",
    __to_vnam(vs1, is_stren, nval), __to_timstr(s1, &(tevp->etime)),
    __to_timstr(s2, &schtim));
   return(FALSE);
  }

 /* inertial reschedule */
 if (__warn_cancel_e && !__no_warns && !__em_suppr(592))
  __emit_path_pulsewarn(pdp, tevp, &(tevp->etime), &schtim, "inout unstable",
   is_stren);

 /* easy show cancel (set to x case) - no new event may or may not switch */
 if (__show_cancel_e)
  {
   if (__showe_onevent) sprintf(s2, "%s (on event)", __to_timstr(__xs,
    &(tevp->etime)));
   else sprintf(s2, "%s (on detect)", __to_timstr(__xs, &__simtime));

   __tr_msg(
    " INOUT PATH DEL, PEND AT %s, UNSTABLE <OV=%s, OSV=%s, NSV=%s SHOWING X FROM %s>\n",
    __to_timstr(s1, &(tevp->etime)), __to_vnam(vs1, is_stren, oval),
    __to_vnam(vs2, is_stren, (word32) tevp->outv), __to_vnam(vs3, is_stren,
    nval), s2);
   if (__showe_onevent)
    { tevp->outv = (is_stren) ? ST_STRONGX : 3; return(FALSE); }

   __cancel_1wev(tevp);
   itevpi[biti] = -1;
   goto set_on_detect_x;
  }

 /* inertial reschedule, this handles cancel if needed */ 
 __tr_msg(
  " INOUT PATH DEL, PEND, UNSTABLE, INERTIAL RESCHD <OV=%s, OSV=%s AT %s, NSV=%s AT %s>\n",
  __to_vnam(vs1, is_stren, oval), __to_vnam(vs2, is_stren, (word32) tevp->outv),
  __to_timstr(s1, &(tevp->etime)), __to_vnam(vs3, is_stren, nval),
  __to_timstr(s2, &schtim)); 

 __reschedule_1wev(tevpi, nval, __pdmindel, schtim, itevpi);
 return(FALSE);
}

/*
 * ROUTINES FOR BID ONLY ALL SAME WIRE TYPE CHANNELS 
 */

/*
 * for stren bid only per bit same wire type tran channel
 * combine all nodes into one stren value using chan rec list
 *
 * this accumlates combined channel strength values in __acum_sb
 *
 * SJM 04/23/01 - changed so always eval from one chan distinguished vtx
 * works since no stren reduction or wired nets in channel always
 * need to combine all nodes into one value that is then stored everywhere 
 * this allows removing back edges (i.e. no longer digraph)
 */
static void eval_assign_stbid_chan(struct chanrec_t *chanp)
{
 register struct bidvtxlst_t *bidvtxlp; 
 register word32 sb2;
 register struct net_t *np;
 register int32 bi;

 /* first eval all contributors to the one universal new value */
 __acum_sb = ST_HIZ;
 bidvtxlp = chanp->bid_vtxlp;
 for (; bidvtxlp != NULL; bidvtxlp = bidvtxlp->bidvtxnxt)
  {
   __push_itstk(bidvtxlp->bidvtx_itp);

   np = bidvtxlp->vnp;
   bi = bidvtxlp->vi1;
   bi = (bi == -1) ? 0 : bi;
   /* SJM - 03/15/00 - if wire is forced, use its values as "driving" val */
   if (np->frc_assgn_allocated
    && np->nu2.qcval[__inum*np->nwid + bi].qc_active)
    {
     sb2 = np->nva.bp[__inum*np->nwid + bi];
    }
   else sb2 = np->ntraux->trnva.bp[__inum*np->nwid + bi];
   __acum_sb = (word32) __comb_1bitsts(np->ntyp, __acum_sb, sb2);
   __pop_itstk();
  }

 /* then assign it to every vertex */
 bidvtxlp = chanp->bid_vtxlp;
 for (; bidvtxlp != NULL; bidvtxlp = bidvtxlp->bidvtxnxt)
  {
   __push_itstk(bidvtxlp->bidvtx_itp);

   np = bidvtxlp->vnp;
   bi = bidvtxlp->vi1;
   stassign_1tranbit(np, bi, __acum_sb);
   __pop_itstk();
  }
}

/*
 * for oon stren bid only per bit same wire type tran channel
 * combine all nodes into one stren value using chan rec list
 *
 * SJM 04/23/01 - changed so always eval from one chan distinguished vtx
 * works since no stren reduction or wired nets in channel always
 * need to combine all nodes into one value that is then stored everywhere 
 * this allows removing back edges (i.e. no longer digraph)
 */
static void eval_assign_bid_chan(struct chanrec_t *chanp) 
{
 register struct bidvtxlst_t *bidvtxlp; 
 register struct net_t *np;
 word32 nav, nbv; 
 int32 bi, bi2;
 
 /* initialize to 2 (hiz) */
 __acum_a = 0;
 __acum_b = 1;

 /* first eval all contributors to the one universal new value */
 /* uses global accum - in gcc global access faster */
 bidvtxlp = chanp->bid_vtxlp;
 for (; bidvtxlp != NULL; bidvtxlp = bidvtxlp->bidvtxnxt)
  {
   __push_itstk(bidvtxlp->bidvtx_itp);
   np = bidvtxlp->vnp;
   bi = bidvtxlp->vi1;
   bi2 = (bi == -1) ? 0 : bi;
   if (np->frc_assgn_allocated
    && np->nu2.qcval[__inum*np->nwid + bi2].qc_active)
    {
     if (bi == -1) ld_scalval_(&nav, &nbv, np->nva.bp);
     else __ld_bit(&nav, &nbv, np, bi);
    }
   else ld_vtx_netbit(&nav, &nbv, np, bi);
   __eval_1w_nonstren(&__acum_a, &__acum_b, nav, nbv, np->ntyp);
   __pop_itstk();
  }

 /* then assign it to every vertex */
 bidvtxlp = chanp->bid_vtxlp;
 for (; bidvtxlp != NULL; bidvtxlp = bidvtxlp->bidvtxnxt)
  {
   __push_itstk(bidvtxlp->bidvtx_itp);
   np = bidvtxlp->vnp;
   bi = bidvtxlp->vi1;
   assign_1tranbit(np, bi, __acum_a, __acum_b);
   __pop_itstk();
  }
}

/*
 * load the bit value of one non strength vertex 
 * called from itree location of wire 
 */
static void ld_vtx_netbit(word32 *ap, word32 *bp, struct net_t *np, int32 bi)
{
 struct xstk_t *xsp;

 if (!np->n_isavec) ld_scalval_(ap, bp, np->ntraux->trnva.bp); 
 else
  {
   /* know bi not -1 since vector */ 
   push_xstk_(xsp, np->nwid);
   __ld_perinst_val(xsp->ap, xsp->bp, np->ntraux->trnva, np->nwid);
   ap[0] = rhsbsel_(xsp->ap, bi);
   bp[0] = rhsbsel_(xsp->bp, bi);
   __pop_xstk();
  }
}

/*
 * routine to assign 1 non stren bid only channel tran bit
 */
static void assign_1tranbit(struct net_t *np, int32 bi, word32 nav, word32 nbv)
{
 /* non stren case */ 
 if (bi == -1) bi = 0;
 /* tran channel elements in P1364 can not have wire delays */
 /* no meaning since drivers of all in channel and wires must be same */

 /* SJM 06/02/99 - will have DWIR and no path if all 0 paths removed */
 /* to optimize invalidating internal consistency check */

 /* for path dest., need immediate assign - internal hard driver value */
 /* change is delayed */

 /* if wire bit forced (qcval inst non nil), must not assign to wire */
 if (np->frc_assgn_allocated
  && np->nu2.qcval[__inum*np->nwid + bi].qc_active)
  {
   /* need not pass bi -1 since checks separately for scalar net */
   if (__debug_flg && __ev_tracing) trmsg_frc_inhibit(np, bi);
   return;
  }

 if (!np->n_isavec) __chg_st_val(np, &nav, &nbv);
 else __chg_st_bit(np, bi, nav, nbv);

 if (__debug_flg && __ev_tracing)
  {
   transtore_trmsg(np, bi, __lhs_changed, nav, nbv);
  }
 /* SJM 08/24/03 - since record reset must test to see if chged */
 if (__lhs_changed) record_sel_nchg_(np, bi, bi);
}

/*
 * store one vertex net bit from passed a and b values
 * called from itree location of np definition
 */
static void st_vtx_netbit(struct net_t *np, int32 bi, word32 nav, word32 nbv)
{
 /* this does not use lhs changed */
 if (np->n_stren) stassign_1tranbit(np, bi, nav);
 else assign_1tranbit(np, bi, nav, nbv);
}

/*
 * emit a tran channel force inhibit of tran channel store
 */
static void trmsg_frc_inhibit(struct net_t *np, int32 bi)
{
 char s1[RECLEN], s2[RECLEN];

 if (np->n_isavec) sprintf(s1, "%s[%d]", np->nsym->synam, bi);
 else strcpy(s1, np->nsym->synam);
 if (np->n_stren) strcpy(s2, " strength"); else strcpy(s2, ""); 
 __tr_msg("## tran/inout channel store of%s %s inhibited - active force\n",
  s2, s1);
}

/*
 * emit a tran channel store trace message
 */
static void transtore_trmsg(struct net_t *np, int32 bi, int32 chg, word32 nav,
 word32 nbv)
{
 char s1[RECLEN], s2[RECLEN], s3[RECLEN], s4[RECLEN];
 byte sb2;
 
 if (np->n_stren)
  {
   strcpy(s1, " strength");
   sb2 = (byte) nav;
   __st_regab_tostr(s2, &sb2, 1);
  }
 else
  {
   strcpy(s1, "");
   __regab_tostr(s2, &nav, &nbv, 1, BHEX, FALSE);
  }
 if (np->n_isavec) sprintf(s3, "%s[%d]", np->nsym->synam, bi);
 else strcpy(s3, np->nsym->synam);
 if (chg)
  __tr_msg("## tran/inout channel store of%s %s %s NV=%s\n", s1,
   __to_wtnam(s4, np), s3, s2);
 else
  __tr_msg("## tran/inout channel no change of%s %s %s OV=%s\n",
   s1, __to_wtnam(s4, np), s3, s2);
}

/*
 * assign one strength tran channel element (wire-bit)
 * bi is -1 for scalar else bit index
 * handles trireg and any forced assign inhibition
 * called from itree loc. of np 
 *
 * routine does not traverse edge graph
 */
static void stassign_1tranbit(struct net_t *np, register int32 bi,
 register word32 sbv)
{
 register byte *sbp2;
 register int32 bind;

 bind = (bi == -1) ? 0 : bi;

 /* get strength wire address */
 get_stwire_addr_(sbp2, np);

 /* tran channel elements in P1364 can not have wire delays */
 /* no meaning since drivers of all in channel and wires must be same */
 /* SJM 06/02/99 - will have DWIR and no path if all 0 paths removed */
 /* to optimize invalidating internal consistency check */

 /* stren non delay wire */ 
 /* if bit forced, must not assign */
 if (np->frc_assgn_allocated
  && np->nu2.qcval[__inum*np->nwid + bind].qc_active)
   { 
    if (__debug_flg && __ev_tracing) trmsg_frc_inhibit(np, bi);
    return;
   }

 /* this may change sbv from tran channel new value to trireg value */ 
 /* SJM 10/16/00 - this was commented out but that caused Samsung trireg */
 /* tests to fail - must have had a reason to comment out but why? */
 /* --- */
 if (np->ntyp == N_TRIREG)
  {
   if (__wire_init) sbv = (byte) (3 | __cap_to_stren[np->n_capsiz]);
   else 
    {  
     if (sbv == ST_HIZ)
      sbv = (byte) ((sbp2[bind] & 3) | __cap_to_stren[np->n_capsiz]);
    }
  }
 /* --- */
 if (__debug_flg && __ev_tracing)
  {
   int32 chg;

   if (sbv != (word32) sbp2[bind]) chg = TRUE; else chg = FALSE;
   transtore_trmsg(np, bi, chg, sbv, sbv);
  } 
 if (sbv != (word32) sbp2[bind])
  {
   sbp2[bind] = (byte) sbv;
   /* notice needs to be bi since need -1 if scalar */
   /* always record since immediate assign - no setting of lhs changed */ 
   /* SJM 08/24/03 - and no need to now since this resets anyway */
   record_sel_nchg_(np, bi, bi);
  }  
}

/*
 * ROUTINES FOR BID TRAN STREN CHANNEL (ALSO BID DIFFERENT WIRE TYPES)
 */

/*
 * assign or schedule channel when hard driver of transistor enable adjacent
 * to this vertex changed
 *
 * normally pass only argument but when tranif enable changes pass both
 * this allows doing only one relaxation
 *
 * this must handle assign inhibition from forcing and trireg decays
 * build the vincincity lists and relax only those vertices
 * uses Bryant algorithm
 *
 * called from itree loc. of start vertex 
 */

/*
 * eval and asssign locally changed part of tran always stren channel 
 *
 * itee context on it stk
 * has trans or bid strength and different wire types 
 * add this vertex to relaxation list
 */
static void eval_update_1w_tranchan(struct vtx_t *vtxp)
{
 struct vtxlst_t *vtxlp;

 vtxlp = add_stchan_chged_vtx(vtxp, __inst_ptr);
 vtxp->vtx_in_vicinity = TRUE;
 find_chgvtx_vicinity(vtxlp);

 /* do the relaxation only on perturbed in vicinity vertices */
 stchan_trif_relax();

 /* final step, assign (maybe schedule) all changed vertices */
 if (__chg_vtxlst_hdr != NULL) assign_chged_vtxs();
}

/*
 * add a changed vertex to st vtx tab in preparation for relaxation
 * passed itree context of vertex
 *
 * this adds the vertex that has changed because hard driver changed, or
 * it was forced/released or it is tranif terminal and enable changed 
 */
static struct vtxlst_t *add_stchan_chged_vtx(struct vtx_t *vtxp,
 struct itree_t *vt1_itp)
{
 int32 bi, inum;
 word32 vtxval, st0, st1, lowst;
 struct net_t *np;
 struct vtxlst_t *vtxlp;

 np = vtxp->vnp;
 bi = (vtxp->vi1 == -1) ? 0 : vtxp->vi1;
 inum = vt1_itp->itinum;

 /* always move new vtx value to old */
 /* for trireg and net's with delay (when supported?) net val may differ */
 /* NO - only update old value when store new value ---
 vtxp->old_vtxval = vtxp->new_vtxval;
 --- */

 /* add vtx to change list and solve channel using relaxation */ 
 /* first guess at new (current) is hard driver value unless forced */ 
 if (np->frc_assgn_allocated
  && np->nu2.qcval[__inum*np->nwid + bi].qc_active)
  {
   vtxp->vtx_forced = TRUE; 
   vtxval = (word32) np->nva.bp[inum*np->nwid + bi];
   vtxp->new_vtxval = vtxval;
  }
 else
  {
   /* when released tran channel relaxed again - this turns off force */
   vtxval = (word32) np->ntraux->trnva.bp[inum*np->nwid + bi];
   /* 04/25/01 - SJM - must turn off forcing each time since some releases */
   /* may have happened but release not connectd to tran vertices */
   vtxp->vtx_forced = FALSE; 
  }
 vtxp->new_vtxval = (byte) vtxval;
 if (vtxval == ST_SUPPLY0 || vtxval == ST_SUPPLY1) vtxp->vtx_supply = TRUE;
 /* think needed since vpi can drive to supply then remove */
 else vtxp->vtx_supply = FALSE;

 /* SJM 04/09/01 - for stren ranges using low - think that will work */
 st1 = (vtxval >> 2) & 0x7;
 st0 = (vtxval >> 5) & 0x7;
 lowst = (st1 < st0) ? st1 : st0;

 /* alloc and link on changed one vertex */
 /* because will be large at start then tiny better to malloc/free */
 if (__vtxlst_freelst == NULL)
  vtxlp = (struct vtxlst_t *) __my_malloc(sizeof(struct vtxlst_t));
 else
  {
   vtxlp = __vtxlst_freelst;
   __vtxlst_freelst = __vtxlst_freelst->vtxnxt;
  }
 vtxlp->vtxp = vtxp;
 vtxlp->vtx_itp = vt1_itp;
 vtxlp->vtxnxt = NULL;

 /* must always put on end */
 if (__stvtxtabend[lowst] != NULL)
  { __stvtxtabend[lowst]->vtxnxt = vtxlp; __stvtxtabend[lowst] = vtxlp; }
 else __stvtxtab[lowst] = __stvtxtabend[lowst] = vtxlp;

 __num_switch_vtxs_processed++;

 /* DBG remove -- */ 
 if (__debug_flg && __ev_tracing)
  {
   __dbg_msg("** vertex %s.%s level %d added to incremental perturb list\n",
    __msg2_blditree(__xs, vtxlp->vtx_itp), to_vtx(__xs2, vtxp), lowst);
  }
 return(vtxlp);
}

/*
 * routine to store into nets all changed vertices 
 */
static void assign_chged_vtxs(void)
{
 register struct vtxlst_t *vtxlp;
 register int32 bi, bi2;
 register struct vtx_t *vtxp;
 word32 sbv;
 byte *sbp2;
 struct net_t *np;

 for (vtxlp = __chg_vtxlst_hdr; vtxlp != NULL; vtxlp = vtxlp->vtxnxt)
  {
   __push_itstk(vtxlp->vtx_itp);

   vtxp = vtxlp->vtxp;
   np = vtxp->vnp;
   bi = vtxp->vi1;
   bi2 = (bi == -1) ? 0 : bi;
   /* sbp2 of index is addr to store new net value into */
   get_stwire_addr_(sbp2, np);
   /* sbv is new value to store */
   sbv = (word32) vtxp->new_vtxval;
   /* for next relax old value is current node val, old val no longer need */
   vtxp->old_vtxval = vtxp->new_vtxval;

   /* know if vertex forced will never be on change list */

   /* this may change sbv from tran channel new value to trireg value */ 
   /* SJM 10/16/00 - this was commented out but that caused Samsung trireg */
   /* tests to fail - must have had a reason to comment out but why? */
   if (np->ntyp == N_TRIREG)
    {
     if (__wire_init) sbv = (byte) (3 | __cap_to_stren[np->n_capsiz]);
     else 
      {  
       if (sbv == ST_HIZ)
        sbv = (byte) ((sbp2[bi2] & 3) | __cap_to_stren[np->n_capsiz]);
      }
    }
   /* DBG remove --- */
   if (__debug_flg && __ev_tracing)
    {
     int32 chg;

     if (sbv != (word32) sbp2[bi2]) chg = TRUE; else chg = FALSE;
     transtore_trmsg(np, bi, chg, sbv, sbv);
    } 
   if (sbv != (word32) sbp2[bi2])
    {
     sbp2[bi2] = (byte) sbv;

     /* notice needs to be bi since need -1 if scalar */
     /* always record since this assign does not set lhs changed */ 
     if (bi == -1) record_nchg_(np);
     else record_sel_nchg_(np, bi, bi);
    } 
   vtxlp->vtxp->vtx_chged = FALSE;
   
   __pop_itstk();
  }
 /* add entire list to end of free list */
 if (__chg_vtxlst_hdr != NULL)
  {
   __chg_vtxlst_end->vtxnxt =  __vtxlst_freelst; 
   __vtxlst_freelst = __chg_vtxlst_hdr;
  }
 __chg_vtxlst_hdr = __chg_vtxlst_end = NULL;
}

/*
 * find all nodes in vicinity of a changed node and add to change list
 *
 * any node on other side of x/1 conducting edge with lower stren is 
 * in vicincity (usually undriven nodes) - forced/assigned never in this set
 * this gets itree context from passed vtx list
 */
static void find_chgvtx_vicinity(struct vtxlst_t *vtxlp)
{
 register struct edge_t *ep;
 int32 bi, bi2, gid;
 word32 conducting;
 struct vtx_t *vtxp, *vtxp2;
 struct vtxlst_t *vtxlp2;
 struct gate_t *gp;
 struct itree_t *oside_itp; 

 vtxp = vtxlp->vtxp;
 bi = vtxp->vi1;
 if (bi == -1) bi = 0; 

 for (ep = vtxp->vedges; ep != NULL; ep = ep->enxt)
  { 
   vtxp2 = ep->ev2;
   if (vtxp2->vtx_in_vicinity) continue;

   bi2 = vtxp2->vi1;
   if (bi2 == -1) bi2 = 0;

   /* if vicinty node forced - can never be in vicinity, can't change */
   if (vtxp2->vtx_forced) continue;

   /* supply can only change if opposite supply across inout */
   if (vtxp2->vtx_supply)
    {
     /* if other side not supply, never can change */
     if (!vtxp->vtx_supply) continue;

     /* if edge tran (not port) reducing so can never change */
     if (ep->enpp->npntyp != NP_BIDMDPRT && ep->enpp->npntyp != NP_BIDICONN)
      continue;
     /* if both supplies same (values never changes so good here) can't chg */
     if (vtxp->new_vtxval == vtxp2->new_vtxval) continue;
    }

   /* SJM 08/26/00 - all xmr/inout edges per inst. so store oside itp */
   if (ep->edgoside_itp != NULL) oside_itp = ep->edgoside_itp;
   else oside_itp = vtxlp->vtx_itp;

   if (ep->enpp->npntyp == NP_TRAN)
    {
     gp = ep->enpp->elnpp.egp;
     if (gp->g_class == GC_TRANIF)
      {
       gid = gp->gmsym->el.eprimp->gateid; 

       __push_itstk(oside_itp);
       conducting = get_switch_tranif_onoff(gp, gid);
       __pop_itstk();

       if (__debug_flg && __ev_tracing)
        {
         char s1[RECLEN]; 

         if (conducting == 0) strcpy(s1, "*OFF*");
         else if (conducting == 1) strcpy(s1, "*ON*");
         else strcpy(s1, "*UNKNOWN*"); 
         __dbg_msg( "-- tranif vicinity switch %s at %s conducting %s\n",
          gp->gsym->synam, __bld_lineloc(__xs, gp->gsym->syfnam_ind,
          gp->gsym->sylin_cnt), s1);
        }
       /* if off, no contribution - if x, conducting stronger */ 
       if (conducting == 0) continue;
      } 
    } 
   else gp = NULL;
   vtxlp2 = add_stchan_chged_vtx(vtxp2, oside_itp);

   vtxp2->vtx_in_vicinity = TRUE;
   /* find vicinity of this node */
   find_chgvtx_vicinity(vtxlp2);
  }
}

/*
 * routine to solve tran(if) channel by relaxation
 *
 * st vtxtab filled with changed nodes (hard drivers/stren tranif enable chged)
 * works from highest to lowest stren - key is that values can only lessen
 * this uses Bryant algorithm
 *
 * this does not call routines which pushes itree stk can't grow too big
 * and gets vtx itree loc from vtx list
 */
static void stchan_trif_relax(void)
{
 register int32 si;
 register struct edge_t *ep;
 word32 cur_vtxval, oside_val, st0, st1, lowst, conducting;
 int32 bi, bi2, nd_itpop, chged, gid, stable;
 struct vtx_t *vtxp, *vtxp2;
 struct vtxlst_t *vtxlp, *vtxlp2, *last_vtxlp;
 struct gate_t *gp;
 struct net_t *np;

 /* DBG remove --
 if (__debug_flg && __ev_tracing)
  {
   __dbg_msg("=== starting channel relaxtion ===\n");
  }
 --- */

 for (;;)
  {
   /* process all level from top down */
   for (stable = TRUE, si = 7; si >= 0; si--) 
    {
     /* DBG remove --
     if (__debug_flg && __ev_tracing) dmp_perturb_list();
     -- */

     last_vtxlp = NULL;
     /* relax all vertices at level si */
     for (vtxlp = __stvtxtab[si]; vtxlp != NULL;)
      {
       /* always push and start at vtx itree loc */
       __push_itstk(vtxlp->vtx_itp);

       /* find new val of this vtx by stren competition with all neighbors */ 
       vtxp = vtxlp->vtxp;
       np = vtxp->vnp;
       vtxp->vtx_in_vicinity = FALSE;
       bi = vtxp->vi1;
       if (bi == -1) bi = 0; 
       /* DBG remove -- */
       if (__debug_flg && __ev_tracing)
        {
         __dbg_msg( "<> relaxing vertex %s.%s at level %d\n",
          __msg2_blditree(__xs, __inst_ptr), to_vtx(__xs2, vtxp), si);
        }
       /* --- */

       /* if this changed node is forced, always wins */
       /* but need to eval supply in case inout port opposite also supply */
       if (vtxp->vtx_forced)
        {
         __pop_itstk();
         last_vtxlp = vtxlp;
         vtxlp = vtxlp->vtxnxt;
         continue;
        }
  
       /* first quess is hard driver val for current changed vtx */
       cur_vtxval = (word32) vtxp->new_vtxval;
       /* add in all contributing edges - only exit is fall thru bottom */
       for (chged = FALSE, ep = vtxp->vedges; ep != NULL; ep = ep->enxt)
        { 
         vtxp2 = ep->ev2;

         /* value for competition is most recent other side vtx value */
         oside_val = vtxp2->new_vtxval;
         /* if value is hiz, skip since know will always lose */
         if (oside_val == ST_HIZ) continue;
    
         bi2 = vtxp2->vi1;
         if (bi2 == -1) bi2 = 0;
  
         if (ep->enpp->npntyp == NP_TRAN)
          {
           gp = ep->enpp->elnpp.egp;
           gid = gp->gmsym->el.eprimp->gateid;

           if (gp->g_class == GC_TRANIF)
            {
             /* move to itree loc of edge's other side vtx to eval on/off */
             /* SJM 08/26/00 - all  edges per inst. so store oside itp*/
             if (ep->edgoside_itp != NULL)
              { __push_itstk(ep->edgoside_itp); nd_itpop = TRUE; }
             else nd_itpop = FALSE; 

             /* compute conducting state if tranif and reduce if tran/tranif */
             /* if conducting x/z, other val stren H/L */
             conducting = try_reduce_tranif_stren(&oside_val, gp);

             if (nd_itpop) __pop_itstk();
  
             /* if tranif not conducting,  no contribution for other side */
             if (conducting == 0) continue; 
            } 
           /* tran just reduces stren, always on - itree cntxt not needed */
           else try_reduce_tran_stren(&oside_val, gid);
          }
         /* if inout, oside value is unchanged */
  
         /* if cur val exactly the same as oside no need for stren */
         /* competition since know result will be same */
         if (cur_vtxval != oside_val)
          {
           /* do stren competition with vtx being determined and oside value */
           /* oside value may have been corrected if edge stren reducing */
           cur_vtxval = (word32) __comb_1bitsts(np->ntyp, cur_vtxval, oside_val);
  
           /* if cur (new) value changed (oside at last partially won), */  
           /* need to combine to produce actual cur (new) val */
           if (cur_vtxval != vtxp->new_vtxval)
            {
             /* combine the cur (latest) val with last relax latest */
             /* this handles stren ranges */
             cur_vtxval = (word32) __comb_1bitsts(np->ntyp, cur_vtxval,
              vtxp->new_vtxval);
             /* use cur (new) as vtx val from now on, rest can all lose */ 
             vtxp->new_vtxval = cur_vtxval;

             /* chg cur to new imoproved val that was assigned to vtx val */
             /* LOOKATME - can this happen - think yes but rare */
             if (cur_vtxval == ST_SUPPLY0 || cur_vtxval == ST_SUPPLY1)
              vtxp->vtx_supply = TRUE;
              
             /* relax value changes so must relax again */
             chged = TRUE; 
            }
          }
         /* if oside same, cur vtx val does not change */
        }
       /* now have new relaxed value - record and move if needed */

       /* SJM 04/23/01 - during wire init do not record - must store */ 
       /* all internal tran chan new values into nets */
       /* old may not be same as net's if net pdst or del so scheduled */
       if (cur_vtxval != vtxp->old_vtxval && !vtxp->vtx_chged)
        {
         add_to_chg_vtx_list(vtxlp, si);
        }

       /* independent of whether new changed value differs from old relax */
       /* entry value, if changed must move to new correct stren tab index */
       if (chged)
        {
         stable = FALSE;
         /* DBG remove -- */
         if (__debug_flg && __ev_tracing)
          {
           char s1[RECLEN];
    
           __dbg_msg("<> vertex %s.%s at level %d changed to %s\n",
            __msg2_blditree(__xs, vtxlp->vtx_itp), to_vtx(__xs2, vtxp), si,
            __to_vvstnam(s1, cur_vtxval));
          }
         /* --- */
        
         /* put on new higher (probably) st list - need to relax list again */
         /* can be lower if tranif disabled */ 
         st1 = (cur_vtxval >> 2) & 0x7;
         st0 = (cur_vtxval >> 5) & 0x7;
         lowst = (st1 < st0) ? st1 : st0;

         /* this can be same stren as previous but stren range or new val */
         /* low st will usually be higher but if tranif off can be lower */
         if (lowst != si)  
          {
           vtxlp2 = vtxlp->vtxnxt;

           /* link out and put on end of other list */
           if (last_vtxlp == NULL) __stvtxtab[si] = vtxlp->vtxnxt;
           else last_vtxlp->vtxnxt = vtxlp->vtxnxt;
           if (vtxlp == __stvtxtabend[si]) __stvtxtabend[si] = last_vtxlp;
  
           /* linked out is now end of higher list */ 
           vtxlp->vtxnxt = NULL;
     
           /* since vtxlp has been removed from prev list do not need new */
           /* alloc, can just link onto end of higher stren list */
           if (__stvtxtabend[lowst] != NULL)
            {
             __stvtxtabend[lowst]->vtxnxt = vtxlp;
             __stvtxtabend[lowst] = vtxlp;
            }
           else __stvtxtab[lowst] = __stvtxtabend[lowst] = vtxlp;
  
           /* this will make move to right next one work - no chg of last */
           __pop_itstk();
           vtxlp = vtxlp2;
           continue;
          }
         /* if stren same leave in list */
        }
       __pop_itstk();
       last_vtxlp = vtxlp;
       vtxlp = vtxlp->vtxnxt;
      }
    }
   if (stable) break;
  }
 /* final step free all st vtx tab level lists */
 for (si = 7; si >=0; si--) 
  { 
   /* finished with last level can free entire perturbed vtx list */
   /* i.e. all nodes of higher strength now known */
   if (__stvtxtab[si] != NULL)
    {
     __stvtxtabend[si]->vtxnxt = __vtxlst_freelst;
     __vtxlst_freelst = __stvtxtab[si];
     __stvtxtab[si] = __stvtxtabend[si] = NULL;  
    }
  }
 /* DBG remove --
 if (__debug_flg && __ev_tracing)
  {
   __dbg_msg("=== relaxation completed - channel stable ===\n");
  }
 --- */
}

/*
 * add vertxx to chg vtx list
 * put on store when done list - only called if not already on list
 * does not need itree context 
 */
static void add_to_chg_vtx_list(struct vtxlst_t *vtxlp, int32 si)
{
 struct vtx_t *vtxp;
 struct vtxlst_t *chg_vtxlp;

 vtxp = vtxlp->vtxp;
 if (__vtxlst_freelst == NULL)
  chg_vtxlp = (struct vtxlst_t *) __my_malloc(sizeof(struct vtxlst_t));
 else
  {
   chg_vtxlp = __vtxlst_freelst;
   __vtxlst_freelst = __vtxlst_freelst->vtxnxt;
  }
 *chg_vtxlp = *vtxlp;
 chg_vtxlp->vtxnxt = NULL;
  
 if (__chg_vtxlst_end == NULL) __chg_vtxlst_hdr = __chg_vtxlst_end = chg_vtxlp;
 else
  {
   __chg_vtxlst_end->vtxnxt = chg_vtxlp;
   __chg_vtxlst_end = chg_vtxlp;
  }
 vtxp->vtx_chged = TRUE; 
 /* DBG remove -- */
 if (__debug_flg && __ev_tracing)
  {
   __dbg_msg("++ vertex %s.%s level %d added to store net change list\n",
    __msg2_blditree(__xs, vtxlp->vtx_itp), to_vtx(__xs2, vtxp), si);
  }
 /* --- */
}

/*
 * dump perturbed queue (element for each stren level)
 */
static void dmp_perturb_list(void)
{
 register int32 si;
 register struct vtxlst_t *vtxlp;
 char s1[RECLEN];

 for (si = 7; si >= 0; si--) 
  {
   if (__stvtxtab[si] == NULL) __tr_msg(" Level %d **empty**\n", si);
   else
    {
     __dbg_msg(" Level %d:\n", si);
     for (vtxlp = __stvtxtab[si]; vtxlp != NULL; vtxlp = vtxlp->vtxnxt)
      {
       __dbg_msg("  %s\n", to_vtx_info(s1, vtxlp->vtxp, vtxlp->vtx_itp));
       /* DBG remove -- */
       if (vtxlp->vtxnxt == NULL)
        {
         if (vtxlp != __stvtxtabend[si]) __misc_terr(__FILE__, __LINE__);
        }
       /* --- */
      }
    }
  }
}

/*
 * dump all vertices in switch channel
 */
static char *to_vtx_info(char *s, struct vtx_t *vtxp, struct itree_t *itp)
{
 char s1[RECLEN], s2[RECLEN];

 sprintf(s,
  "  vertex %s.%s new=%s old=%s chg=%d, vicinity=%d, frc=%d, sup=%d", 
  __msg2_blditree(__xs, itp), to_vtx(__xs2, vtxp),
  __to_vvstnam(s1, vtxp->new_vtxval), __to_vvstnam(s2, vtxp->old_vtxval),
  vtxp->vtx_chged, vtxp->vtx_in_vicinity, vtxp->vtx_forced, vtxp->vtx_supply);
 return(s); 
}

/*
 * compute conducting state and oside value for tranif gate 
 *
 * returns conducting value if not 0 sets oside_val to reduced val
 * SJM 04/23/01 - also if tranif and conducting x/z (3), value is H or L
 */
static int32 try_reduce_tranif_stren(word32 *oside_val, struct gate_t *gp)
{
 int32 conducting, is_resist, gid;
 word32 sb2, sb3, st1, st0;
 
 gid = gp->gmsym->el.eprimp->gateid;
 conducting = get_switch_tranif_onoff(gp, gid);
 if (__debug_flg && __ev_tracing)
  {
   char s1[RECLEN]; 
  
   if (conducting == 0) strcpy(s1, "*OFF*");
   else if (conducting == 1) strcpy(s1, "*ON*");
   else strcpy(s1, "*UNKNOWN*"); 
   __dbg_msg("-- tranif switch %s at %s conducting %s\n",
    gp->gsym->synam, __bld_lineloc(__xs, gp->gsym->syfnam_ind,
    gp->gsym->sylin_cnt), s1);
  }
 /* if off, no contribution */
 if (conducting == 0) return(0);

 /* LOOKATME - maybe should use g_gone or other bit - but adds whole word32 */
 if (gid == G_RTRANIF0 || gid == G_RTRANIF1) is_resist = TRUE;
 else is_resist = FALSE;

 sb2 = *oside_val;
 /* reduce stren for tran and rtran - if conducting off, won't get here */
 if (is_resist)
  {
   /* SJM 04/23/01 - had stren backward st0 is high bit 5-7, st1 2-4 */ 
   st0 = __rmos_stmap[(sb2 >> 5) & 0x7];
   st1 = __rmos_stmap[(sb2 >> 2) & 0x7];
  }
 else
  {
   st0 = __mos_stmap[(sb2 >> 5) & 0x7];
   st1 = __mos_stmap[(sb2 >> 2) & 0x7];
  }

 /* SJM 12/04/00 - fixed typo where 0 stren was mixed with 1 stren */
 /* so reduction was wrong */
 sb3 = sb2 & 3;
 if (conducting == 3)
  {
   /* L */
   /* SJM 04/23/01 - had stren backward st0 is high bit 5-7, st1 2-4 */ 
   if (sb3 == 0) *oside_val = (st0 << 5) | 3;
   /* H */
   else if (sb3 == 1) *oside_val = (st1 << 2) | 3;
   /* LOOKATME - think is z won't get here so only possible is x */
   else *oside_val = sb3 | (st1 << 2) | (st0 << 5);
   return(3);
  }
 *oside_val = sb3 | (st1 << 2) | (st0 << 5);
 return(conducting);
}

/*
 * compute conducting state and oside value for tran
 * here conducting state always implied 1
 * notice this does not need itree context
 *
 * returns conducting value if not 0 sets oside_val to reduced val
 * SJM 04/23/01 - also if tranif and conducting x/z (3), value is H or L
 */
static void try_reduce_tran_stren(word32 *oside_val, int32 gid)
{
 int32 is_resist;
 word32 sb2, st1, st0;
 
 /* LOOKATME - maybe should use g_gone or other bit - but adds whole word32 */
 if (gid == G_RTRAN) is_resist = TRUE; else is_resist = FALSE;

 sb2 = *oside_val;
 /* reduce stren for tran and rtran - if conducting off, won't get here */
 if (is_resist)
  {
   /* SJM 04/23/01 - had stren backward st0 is high bit 5-7, st1 2-4 */ 
   st0 = __rmos_stmap[(sb2 >> 5) & 0x7];
   st1 = __rmos_stmap[(sb2 >> 2) & 0x7];
  }
 else
  {
   st0 = __mos_stmap[(sb2 >> 5) & 0x7];
   st1 = __mos_stmap[(sb2 >> 2) & 0x7];
  }
 *oside_val = (sb2 & 3) | (st0 << 5) | (st1 << 2);
}

/*
 * routine to access tranif on/off during switch channel evaluation 
 * called with itree context of tranif gate 
 *
 * since vertex changes of tran switch channels not made and propagated until
 * channel completed, if channel in any switch channel must use vtx value
 *
 * LOOKATME - could use state unless in same switch channel
 */
static int32 get_switch_tranif_onoff(struct gate_t *gp, int32 gid)
{ 
 register struct expr_t *termxp;
 register struct net_t *np2;
 register int32 val, bi;
 int32 ibi;
 struct vbinfo_t *vbip;
 struct chanrec_t *chanp;

 termxp = gp->gpins[2];
 if (termxp->optyp == ID || termxp->optyp == GLBREF)
  np2 = termxp->lu.sy->el.enp;
 else if (termxp->optyp == LSB) np2 = termxp->lu.x->lu.sy->el.enp;
 else goto no_traux;

 if (np2->ntraux == NULL) goto no_traux;
 
 switch ((byte) termxp->optyp) {
  case ID:
   ibi = np2->nwid*__inum;
   if ((vbip = np2->ntraux->vbitchans[ibi]) == NULL) goto no_traux;
   chanp = &(__chantab[vbip->chan_id]);
   /* SJM 10/29/01 - need to access var value for inout chans, no vtx state */ 
   if (chanp->chan_no_vtxs) val = get_bidchan_val(chanp, np2, ibi, 0); 
   else val = (int32) vbip->vivxp->new_vtxval;
   break;
  case GLBREF:
   __xmrpush_refgrp_to_targ(termxp->ru.grp);
   ibi = np2->nwid*__inum;
   if ((vbip = np2->ntraux->vbitchans[ibi]) == NULL)
    { __pop_itstk(); goto no_traux; }

   chanp = &(__chantab[vbip->chan_id]);
   if (chanp->chan_no_vtxs) val = get_bidchan_val(chanp, np2, ibi, 0); 
   else val = (int32) vbip->vivxp->new_vtxval;
   __pop_itstk();
   break;
  case LSB:
   /* BEWARE - this assume constant select expr folded */ 
   if (termxp->ru.x->optyp != NUMBER) goto no_traux;
   bi = __get_const_bselndx(termxp);
   if (termxp->lu.x->optyp == ID)
    {
     ibi = np2->nwid*__inum;
     if ((vbip = np2->ntraux->vbitchans[ibi + bi]) == NULL) goto no_traux;
     chanp = &(__chantab[vbip->chan_id]);
     if (chanp->chan_no_vtxs) val = get_bidchan_val(chanp, np2, ibi, bi); 
     else val = (int32) vbip->vivxp->new_vtxval;
     break;
    }
   __xmrpush_refgrp_to_targ(termxp->lu.x->ru.grp);
   ibi = np2->nwid*__inum;
   if ((vbip = np2->ntraux->vbitchans[ibi + bi]) == NULL)
    { __pop_itstk(); goto no_traux; }
   chanp = &(__chantab[vbip->chan_id]);
   if (chanp->chan_no_vtxs) val = get_bidchan_val(chanp, np2, ibi, bi); 
   else val = (int32) vbip->vivxp->new_vtxval;
   __pop_itstk();
   break;
  default: goto no_traux;
 }
 val &= 3;
 if (val == 2 || val == 3) return(3);
 /* SJM 04/20/01 - for tranif0 gates, conducting 1 is vtx value 0 */ 
 if (gid == G_RTRANIF0 || gid == G_TRANIF0) 
  {
   val = (val == 0) ? 1 : 0;
  }
 return(val);

no_traux:
 /* notice state is stored as conducting after correct for tranif0 gates */
 return((int32) get_tranif_onoff_(gp));
}

/*
 * get bid non vertex channel value - i.e. the net value 
 * since that determines on off state
 *
 * SJM 10/29/01 - need to access var value for inout chans, no vtx state
 */
static int32 get_bidchan_val(struct chanrec_t *chanp, register struct net_t *np,
 int32 ibi, int32 bi)
{
 register int32 val, chtyp;
 word32 nav, nbv;

 if ((chtyp = chanp->chtyp) == TRPROC_STBID) val = np->nva.bp[ibi + bi];
 else if (chtyp == TRPROC_BID) 
  {
   /* LOOKATME - can this be low bit of vector - think yes */
   if (!np->n_isavec) ld_scalval_(&nav, &nbv, np->nva.bp);
   else __ld_bit(&nav, &nbv, np, bi);
   val = nav | (nbv << 1);
  }
 else { val = 3; __case_terr(__FILE__, __LINE__); }
 return(val);
}

/*
 * evaluate a tranif channel when 3rd control input changes
 * called from itree ref. location of gate
 *
 * since no hard driver change, can ignore non tran channel bits
 * because they only change when hard drivers change
 *
 * SJM 04/11/01 - put back to immediately eval switch channel when enable chgs
 * LOOKATME - think should change so only called when from to 0 (off)
 * think to/from 1/x do not change channel
 */
extern void __immed_eval_trifchan(struct gate_t *gp)
{
 int32 nd_itpop, bi;
 struct net_t *np;
 struct traux_t *trap;
 struct expr_t *xp, *idndp;
 struct vbinfo_t *vbip;
 struct vtxlst_t *vtxlp;

 /* SJM 04/23/01 - remove non peri channels since only used for 1 inst case */
 /* but only one graph for that in any case */
 /* need to find tranif transistor first terminal and perturb */
 xp = gp->gpins[0]; 
 if (xp->optyp == ID || xp->optyp == GLBREF) idndp = xp;  
 else if (xp->optyp == LSB) idndp = xp->lu.x;
 else { __case_terr(__FILE__, __LINE__); idndp = NULL; }

 if (idndp->optyp == GLBREF) 
  { __xmrpush_refgrp_to_targ(idndp->ru.grp); nd_itpop = TRUE; }
 else nd_itpop = FALSE;

 np = idndp->lu.sy->el.enp;
 bi = -1;
 if (xp->optyp == LSB) bi = __get_const_bselndx(xp);
 if (bi == -1) bi = 0;
 trap = np->ntraux;
 vbip = trap->vbitchans[np->nwid*__inum + bi];

 /* add this vertex to relaxation list */
 vtxlp = add_stchan_chged_vtx(vbip->vivxp, __inst_ptr);
 vbip->vivxp->vtx_in_vicinity = TRUE;
 find_chgvtx_vicinity(vtxlp);

 if (nd_itpop) __pop_itstk();

 /* also find and perturb 2nd terminal */
 xp = gp->gpins[1]; 
 if (xp->optyp == ID || xp->optyp == GLBREF) idndp = xp;  
 else if (xp->optyp == LSB) idndp = xp->lu.x;
 else { __case_terr(__FILE__, __LINE__); idndp = NULL; }

 if (idndp->optyp == GLBREF) 
  { __xmrpush_refgrp_to_targ(idndp->ru.grp); nd_itpop = TRUE; }
 else nd_itpop = FALSE;

 np = idndp->lu.sy->el.enp;
 bi = -1;
 if (xp->optyp == LSB) bi = __get_const_bselndx(xp);
 if (bi == -1) bi = 0;
 trap = np->ntraux;
 vbip = trap->vbitchans[np->nwid*__inum + bi];

 /* add this vertex to relaxation list */
 if (!vbip->vivxp->vtx_in_vicinity)
  {
   vtxlp = add_stchan_chged_vtx(vbip->vivxp, __inst_ptr);
   vbip->vivxp->vtx_in_vicinity = TRUE;
   find_chgvtx_vicinity(vtxlp);
  }

 if (nd_itpop) __pop_itstk();

 /* do the relaxation only on perturbed in vicinity vertices */
 stchan_trif_relax();
 /* final step, assign (maybe schedule) all changed vertices */
 if (__chg_vtxlst_hdr != NULL) assign_chged_vtxs();
}

/*
 * ROUTINES TO DUMP TRAN CHANNELS
 */

/*
 * dump all trans in a module
 */
extern void __dmp_modtrans(struct mod_t *mdp)
{
 register int32 ni, ii;
 register struct net_t *np;
 int32 insts, bi2;
 struct traux_t *trap;
 struct vbinfo_t *vbip;
 struct vtx_t *vtxp;
 char s1[RECLEN], s2[RECLEN];

 for (ni = 0, np = &(mdp->mnets[0]); ni < mdp->mnnum; ni++, np++) 
  {
   if ((trap = np->ntraux) == NULL)
    continue;

   insts = mdp->flatinum;
   for (ii = 0; ii < insts; ii++)
    {
     __push_itstk(mdp->moditps[ii]);
     for (bi2 = np->nwid - 1; bi2 >= 0; bi2--)
      {
       if ((vbip = trap->vbitchans[__inum*np->nwid + bi2])
        == NULL)
        {
         sprintf(s1, "%s.", __msg2_blditree(__xs, __inst_ptr));
         if (!np->n_isavec) strcpy(s2, ""); else sprintf(s2, "[%d]", bi2);  
         __dbg_msg("-- net %s%s%s not in any tran channel\n", s1,
          np->nsym->synam, s2);
        }  
       else
        {
         if (__chantab[vbip->chan_id].chan_no_vtxs)
          __dmp_bidchan(&(__chantab[vbip->chan_id]));
         else { vtxp = vbip->vivxp; __dmp_trchan(vtxp); }
        }
      }
     __pop_itstk();
    }
  }
}

/*
 * dump a chan. channel - requires set itree loc. context to work
 */
extern void __dmp_trchan(struct vtx_t *vtxp)
{
 register struct edge_t *ep;
 int32 bi, chanid, base, ei;
 struct net_t *np;
 struct chanrec_t *chanp;

 np = vtxp->vnp;
 bi = (vtxp->vi1 == -1) ? 0 : vtxp->vi1;
 base = __inum*np->nwid;
 chanid = vtxp->vnp->ntraux->vbitchans[base + bi]->chan_id;
 chanp = &(__chantab[chanid]);

 /* __dbg_msg("<<> marking %s\n", to_vtx(__xs2, vtxp)); */
 vtxp->vtx_mark = TRUE;

 /* dump the distinguished vertex */
 __dbg_msg(
  "-** vertex %s.%s(id=%d, type=%d, mixed wires=%d, mark=%u) edges:\n",
   __msg2_blditree(__xs, __inst_ptr), to_vtx(__xs2, vtxp), chanid,
   chanp->chtyp, chanp->chan_diffwtyps, vtxp->vtx_mark); 
  for (ei = 1, ep = vtxp->vedges; ep != NULL; ep = ep->enxt, ei++)
   { prt_edge(vtxp, ep, ei); }

 dmp_vtx_edges(vtxp, __inst_ptr); 

 /* turn off marks, to be ready for next change */
 off_bichan_marks(chanp);

 __dbg_msg("=== end of channel ===\n");
}

/*
 * dump inout channel - simple list and each node has own itree context
 */
extern void __dmp_bidchan(struct chanrec_t *chanp)
{
 register struct bidvtxlst_t *bidvtxlp;
 int32 chanid;
 char s1[RECLEN];

 chanid = chanp - __chantab;
 __dbg_msg(" *** inout channel (id=%d, type=%d, mixed_wires=%d):\n",
  chanid, chanp->chtyp, chanp->chan_diffwtyps); 

 bidvtxlp = chanp->bid_vtxlp;
 for (; bidvtxlp != NULL; bidvtxlp = bidvtxlp->bidvtxnxt)
  {
   if (bidvtxlp->vi1 == -1) strcpy(s1, bidvtxlp->vnp->nsym->synam);
   else sprintf(s1, "%s[%d]", bidvtxlp->vnp->nsym->synam, bidvtxlp->vi1);
   __dbg_msg(" -- vertex %s.%s\n", __msg2_blditree(__xs,
    bidvtxlp->bidvtx_itp), s1);
  }
 __dbg_msg("=== end of channel ===\n");
}

/*
 * dump vertices on other side of edges of a vtx 
 * passed vtx itree loc since itree stack probably not high enough
 */
static void dmp_vtx_edges(struct vtx_t *vtxp, struct itree_t *vt1_itp)
{
 register struct edge_t *ep;
 struct itree_t *oside_itp;

 /* mark the root vertex */
 for (ep = vtxp->vedges; ep != NULL; ep = ep->enxt)
  { 
   /* DBG remove -- */
   if (ep->ev2 == NULL) __misc_terr(__FILE__, __LINE__);
   /* --- */

   if (ep->ev2->vtx_mark)
    {
     /* __dbg_msg("<<> vertex %s already marked\n", to_vtx(__xs2, ep->ev2)); */
     continue;
    }
   /* __dbg_msg("<<> vertex %s not marked\n", to_vtx(__xs2, ep->ev2)); */

   /* for mdprt will always be local */  
   /* DBG remove --- */
   if (ep->enpp->npntyp == NP_BIDMDPRT
    && ep->enpp->npproctyp != NP_PROC_INMOD)
    __misc_terr(__FILE__, __LINE__); 
   /* --- */ 

   /* SJM - 05/21/01 - compute oside edge and use call stack since itstk */
   /* not deep enough */
   if (ep->edgoside_itp != NULL) oside_itp = ep->edgoside_itp;
   else oside_itp = vt1_itp;

   dmp_vtx_and_out_edges(ep->ev2, oside_itp);

   /* __dbg_msg("<<> marking %s\n", to_vtx(__xs2, ep->ev2)); */
   ep->ev2->vtx_mark = TRUE;

   dmp_vtx_edges(ep->ev2, oside_itp);
  }
}

/*
 * dump a vertex and all of its out edges
 */
static void dmp_vtx_and_out_edges(struct vtx_t *vtxp,
 struct itree_t *oside_itp)
{
 register int32 ei;
 register struct edge_t *ep;

 if (vtxp->vtx_mark)
  {
   /* ---
   __dbg_msg("<<> %s marked - out edges not printed\n",
    to_vtx(__xs2, vtxp));
   --- */
   return; 
  }
 /* else __dbg_msg("<<> %s not marked\n", to_vtx(__xs2, vtxp)); */

 __push_itstk(oside_itp);
 __dbg_msg(
  "=== vertex %s.%s edges:\n", __msg2_blditree(__xs, __inst_ptr),
  to_vtx(__xs2, vtxp));
 for (ei = 1, ep = vtxp->vedges; ep != NULL; ep = ep->enxt, ei++)
  { prt_edge(vtxp, ep, ei); }
 __pop_itstk();
}

/*
 * print edge - itree version (expects start to be on itree stack) 
 */
static void prt_edge(struct vtx_t *vtxp, struct edge_t *ep, int32 ei)
{
 struct itree_t *eitp, *v2itp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 if (ei == -1) strcpy(s1, ""); else sprintf(s1, " %d:", ei); 
 getv2_itp(ep, __inst_ptr, &eitp, &v2itp);
 __dbg_msg(" -- edge%s %s.%s->%s.%s: npp:\n", s1,
  __msg2_blditree(__xs, __inst_ptr), to_vtx(s2, vtxp),
  __msg2_blditree(__xs2, v2itp), to_vtx(s3, ep->ev2));  
 /* LOOKATME - shouldn't test be internal error */
 if (vtxp != NULL) __dmp1_nplstel(__inst_mod, vtxp->vnp, ep->enpp);
}

/*
 * build an vertex identifying string
 */
static char *to_vtx(char *s, struct vtx_t *vp)
{
 if (vp == NULL) strcpy(s, "*NONE*");
 if (vp->vi1 == -1) strcpy(s, vp->vnp->nsym->synam);
 else sprintf(s, "%s[%d]", vp->vnp->nsym->synam, vp->vi1);
 return(s);
}

/*
 * called with itree loc. of first edge in vitp - 2nd edge and 2nd vtx itp
 */
static void getv2_itp(struct edge_t *ep, struct itree_t *vitp, 
 struct itree_t **eitp, struct itree_t **v2itp)
{
 int32 num_itpops;

 if (ep->ev2 == NULL) __arg_terr(__FILE__, __LINE__);
 /* move forward - vitp is first */
 num_itpops = 1;
 __push_itstk(vitp);
 if (ep->enpp->npproctyp == NP_PROC_INMOD) *eitp = vitp;
 else
  {
   /* SJM 04/17/03 - if XMR does not match - do not change itree loc */
   if (__move_to_npprefloc(ep->enpp))
    {
     *eitp = __inst_ptr;
     num_itpops++;
    }
   else __misc_terr(__FILE__, __LINE__);
  }
 /* LOOKATME - bit for concat must be index that is from vtx */
 /* SJM 08/26/00 - because all xmr/inout edges per inst. can store new itree */
 if (ep->edgoside_itp != NULL)
  {
   __push_itstk(ep->edgoside_itp);
   num_itpops++;
  }
 *v2itp = __inst_ptr;
 while (num_itpops-- > 0) __pop_itstk();
}

/*
 * build drivers on a net
 *
 * entire net drivers are not net bit
 * nothing on itree stack here
 */
extern void __dmp_bidnet_drvs(struct net_t *np, struct mod_t *mdp)
{
 register struct edge_t *ep;
 register int32 ei;
 struct traux_t *trap;
 struct vbinfo_t *vbip;
 struct vtx_t *vtxp;
 char s1[RECLEN], s2[RECLEN], s3[RECLEN];

 if (np->ntraux == NULL) return;
 if ((trap = np->ntraux) == NULL) return; 
 if ((vbip = trap->vbitchans[0]) == NULL) return;
 vtxp = vbip->vivxp;
 if ((ep = vtxp->vedges) == NULL) return;

 for (ei = 0; ep != NULL; ep = ep->enxt, ei++)
  { 
   if (ei == -1) strcpy(s1, ""); else sprintf(s1, " %d:", ei); 
   __dbg_msg(" -- edge%s %s->%s: npp:\n", s1,
    to_vtx(s2, vtxp), to_vtx(s3, ep->ev2));  
   if (vtxp != NULL) __dmp1_nplstel(mdp, vtxp->vnp, ep->enpp);
  }
}

/*
 * ROUTINES TO BUILD XL STYLE CONNECTED LOAD/DRIVER NET/BIT TABLES
 */

/*
 * build net/bit vertex table for all net/bits that contribute xl style 
 * loads to acc_ or vpi_ iterator
 *
 * LOOKATME - trying to mimic xl style flattening for loads here
 */
extern int32 __bld_xl_drvld_vtxtab(struct net_t *np, int32 bi,
 struct itree_t *itp, int32 is_load)
{
 register struct net_pin_t *npp;
 int32 ix_insert, vi, osbi, fromr_bi, catel_bi;
 struct xldlnpp_t *xldlp, *xldlp2;
 struct itree_t *ositp;
 struct mod_t *osmdp;
 struct mod_pin_t *mpp;
 struct net_t *osnp;
 struct expr_t *xp, *catxp;

 __xldl_hdr = __last_xldl = NULL;

 /* know always at least one vertex - needed for actual lds/drvrs in iter */
 /* this sets ix insert loc. */
 vi = get_xldl_vtx(itp, np, bi, &ix_insert);
 /* DBG remove --- */
 if (vi != -1) __vpi_terr(__FILE__, __LINE__);
 /* -- */
 /* add first net/bit vertex */
 vi = add_xldl_vtx(itp, np, bi, ix_insert);

 /* add any iconn or mod port 1 bit npp's to a list for more processing */ 
 fill_port_npps(np, bi, itp);
 /* common case of xl style and local ld/driver iterator same */
 if (__xldl_hdr == NULL) return(__num_xldlvtxs);
 
 for (; __xldl_hdr != NULL;)
  {
   /* get first unprocessed npp */
   xldlp = __xldl_hdr;
   npp = xldlp->xlnpp;
   __push_itstk(xldlp->xlitp);

   /* move to other side */
   switch ((byte) npp->npntyp) {
    /* SJM 09/20/02 - never see PB forms for inouts */ 
    case NP_ICONN: case NP_BIDICONN:
     /* other side is mdprt */
     ositp = &(__inst_ptr->in_its[npp->elnpp.eii]);
     osmdp = ositp->itip->imsym->el.emdp;
     mpp = &(osmdp->mpins[npp->obnum]);
     if (is_load) 
      {
       if (npp->npntyp != NP_BIDICONN && mpp->mptyp != IO_IN) 
        goto nxt_xlld_npp;
      }
     else
      {
       if (npp->npntyp != NP_BIDICONN && mpp->mptyp != IO_OUT) 
        goto nxt_xlld_npp;
      }
     xp = mpp->mpref;
     break;
    case NP_MDPRT: case NP_BIDMDPRT:
     /* other side is iconn */
     /* access port before mosing up */
     mpp = &(__inst_mod->mpins[npp->obnum]);

     /* DBG remove -- */
     if (__inst_ptr->up_it == NULL) __vpi_terr(__FILE__, __LINE__);
     /* --- */
     ositp = __inst_ptr->up_it;
     osmdp = ositp->itip->imsym->el.emdp;
     if (is_load) 
      {
       if (npp->npntyp != NP_BIDMDPRT && mpp->mptyp != IO_OUT) 
        goto nxt_xlld_npp;
      }
     else
      {
       if (npp->npntyp != NP_BIDMDPRT && mpp->mptyp != IO_IN) 
        goto nxt_xlld_npp;
      }
     xp = __inst_ptr->itip->ipins[npp->obnum]; 
     break;
    default:
     goto nxt_xlld_npp;
   }
   /* add other side npp to end of list - expr, net, bit set */
   catel_bi = -1;
   if (npp->npaux == NULL) fromr_bi = bi;
   else
    { 
     if (npp->npaux->nbi1 == -1) fromr_bi = bi;
     /* ??? LOOKATME - why is npp low psel bit subtracted off */  
     else fromr_bi = bi - npp->npaux->nbi2.i;

     /* in case this side expr in concat need low of where in concat */
     /* so can add to otherside index to get matching oside bit */
     if (npp->npaux->lcbi1 != -1)
      { 
       fromr_bi = bi + npp->npaux->lcbi1;
       catxp = xp;
       xp = find_cat_oside_xp(catxp, fromr_bi, &(catel_bi));
      }
    }
   osnp = xldrvld_to_netbit(xp, ((catel_bi == -1) ? fromr_bi : catel_bi),
    &(osbi), ositp); 
   if (osbi == -2) goto nxt_xlld_npp;

   /* try to add net vertex - added unless already in table */
   if ((vi = get_xldl_vtx(ositp, osnp, osbi, &ix_insert)) != -1)
    {
     if (__debug_flg)
      {
       __dbg_msg(
        " -- net %s bit %d in %s already in xl style load/drive list\n",
        osnp->nsym->synam, osbi, __msg2_blditree(__xs, ositp));
      }
     goto nxt_xlld_npp;
    }

   /* add this in sorted position in table */ 
   add_xldl_vtx(ositp, osnp, osbi, ix_insert);
   /* also add all its connecting npp's */
   fill_port_npps(osnp, osbi, ositp);

nxt_xlld_npp:
   __pop_itstk();
   /* done with current that is on front so free and update header */
   xldlp2 = __xldl_hdr->xlnxt; 
   __my_free((char *) __xldl_hdr, sizeof(struct xldlnpp_t)); 
   __xldl_hdr = xldlp2;
  }
 return(__num_xldlvtxs);
}

/*
 * get net and bit from expr for buildin
 * given an driv tran channel lhs non concatenate expression get net and bit
 *
 * almost same as tranx to net bit
 * sets bi to -2 for other side out of this side range or not constant ndx
 */
static struct net_t *xldrvld_to_netbit(register struct expr_t *xp,
 int32 fromr_bi, int32 *bi, struct itree_t *oside_itp)
{
 register struct net_t *np;
 int32 ri1, ri2;

 np = __find_tran_conn_np(xp);
 if (xp->optyp == LSB)
 {
   __push_itstk(oside_itp);
   if (xp->ru.x->optyp != NUMBER && xp->ru.x->optyp != ISNUMBER) *bi = -2;
   else
    {
     *bi = __get_const_bselndx(xp);
     if (fromr_bi > 0) *bi = -2;
    }
   __pop_itstk();
   return(np);
  }
 if (xp->optyp == PARTSEL)
  {
   ri1 = __contab[xp->ru.x->lu.x->ru.xvi];
   ri2 = __contab[xp->ru.x->ru.x->ru.xvi];
   if (fromr_bi == -1) *bi = ri2;
   else { *bi = ri2 + fromr_bi; if (*bi > ri1) *bi = -2;  }
   return(np);
  }
 /* this side is 1 bit scalar */
 if (!np->n_isavec)
  {
   *bi = -1;
   if (fromr_bi > 0) *bi = -2;
  }
 else { if (fromr_bi >= np->nwid) *bi = -2; else *bi = fromr_bi;  }
 return(np);
}

/*
 * add mod port and iconn edges (npps) connecting to net or net bit to list
 *
 * this include all npp's that are one bit and match bit if bit select
 * filtering out other type (non load or non driver) done elsewhere
 */
static void fill_port_npps(struct net_t *np, int32 bi, struct itree_t *itp)
{
 register struct net_pin_t *npp;
 
 __push_itstk(itp);
 /* first all npp loads connected to ports */
 for (npp = np->nlds; npp != NULL; npp = npp->npnxt)
  {
   /* only consider iconn and mod port npps */
   if (npp->npntyp != NP_ICONN && npp->npntyp != NP_MDPRT) continue;

   add_match_vtxs(np, npp, bi);
  }
 /* will be no I/O drivers and no tran channel for reg */
 if (np->ntyp >= NONWIRE_ST)
  {
   /* DBG remove --- */
   if (np->ndrvs != NULL || np->ntraux != NULL) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   goto done;
  }
 /* then all npp drivers connected to ports */
 for (npp = np->ndrvs; npp != NULL; npp = npp->npnxt)
  {
   /* only consider iconn and mod port npps */
   if (npp->npntyp != NP_ICONN && npp->npntyp != NP_MDPRT) continue;

   add_match_vtxs(np, npp, bi);
  }
 /* tran channel connections if they exist */
 if (np->ntraux == NULL) goto done;

 /* iconn tran channel connections are all loads and all drivers */
 for (npp = np->ntraux->tran_npps; npp != NULL; npp = npp->npnxt)
  {
   /* only consider bid iconn and bid mod port npps */
   if (npp->npntyp != NP_BIDICONN && npp->npntyp != NP_BIDMDPRT) continue;

   add_match_vtxs(np, npp, bi);
  } 
done:
 __pop_itstk();
}

/*
 * add right inst and right bit npps to port vertex list
 */
static void add_match_vtxs(struct net_t *np, struct net_pin_t *npp, int32 bi)  
{
 int32 ri1, ri2;
 struct xldlnpp_t *xldlp;

 /* filter out per inst. that is not this instance */
 if (npp->npproctyp == NP_PROC_FILT
  && npp->npaux->npu.filtitp != __inst_ptr) return;

 /* LOOKATME - what happens with xmr iconns - for now not in xl iters */
 if (npp->npproctyp == NP_PROC_GREF) return;

 /* this must run with right itree loc. (for vector ri1, ri2 h:0 forms */
 __get_bidnpp_sect(np, npp, &ri1, &ri2);
 /* xl drives and loads only combined for scalar or bit select connects */
 /* any scalar must match */
 if (ri1 == -1)
  {
   /* DBG remove --- */
   if (bi != -1) __vpi_terr(__FILE__, __LINE__);
   /* --- */
   goto got_match;
  }
 /* SJM 01/28/05 - old algorithm that stop on entire vector or psel */
 /* wrong - must match as usual */
 if (bi > ri1 || bi < ri2) return;

got_match:
  xldlp = (struct xldlnpp_t *) __my_malloc(sizeof(struct xldlnpp_t));
  xldlp->xlnpp = npp;
  xldlp->xlitp = __inst_ptr;
  xldlp->xlnxt = NULL;

  if (__last_xldl == NULL) __xldl_hdr = xldlp;
  else __last_xldl->xlnxt = xldlp;
  __last_xldl = xldlp;
}

/*
 * search sorted index into xl drive/load vertex table for matching vertex
 *
 * sets ix_insert index if not found 
 * binary search better even for small table
 */
static int32 get_xldl_vtx(struct itree_t *itp, struct net_t *np, int32 bi,
 int32 *ix_insert)
{
 register int32 m, cv;
 int32 l, h;
 register struct xldlvtx_t *xldlvp;

 if (__num_xldlvtxs <= 0) { *ix_insert = 0; return(-1); }
 l = 0; h = __num_xldlvtxs - 1;
 for (;;)
  {
   m = (l + h)/2;
   xldlvp = __xldlvtxind[m];
   /* first sort by module name */
   if ((cv = strcmp(xldlvp->dlitp->itip->imsym->synam,
    itp->itip->imsym->synam)) == 0)
    {
     /* then by instance of module */
     if ((cv = xldlvp->dlitp->itinum - itp->itinum) == 0) return(m);
     /* then by net name */
     if ((cv = strcmp(xldlvp->dlnp->nsym->synam, np->nsym->synam)) == 0)
      {
       /* finally by bit index */
       if ((cv = xldlvp->dlbi - bi) == 0) return(m);
      }
    }
   if (cv < 0) l = m + 1; else h = m - 1;
   if (h < l) { *ix_insert = l; break; }
  }
 return(-1);
}

/*
 * add an a new xl drv/ld vertex (net/bit)
 *
 * know now in table and ix_insert set to place to insert or not called 
 * must call get xldl vtx before calling this to set ix_insert
 */
static int32 add_xldl_vtx(struct itree_t *itp, struct net_t *np, int32 bi,
 int32 ix_insert)
{
 register int32 ki;
 int32 osize, nsize;
 struct xldlvtx_t *xldlvp;

 if (++__num_xldlvtxs > __siz_xldlvtxtab)
  {
   /* first grow table because few new keywords, fibronaci growth */
   if (__siz_xldlvtxtab == 0)
    {
     nsize = 100*sizeof(struct xldlvtx_t *);
     __xldlvtxind = (struct xldlvtx_t **) __my_malloc(nsize);
     __siz_xldlvtxtab = 100;
    }
   else
    {
     osize = __siz_xldlvtxtab*sizeof(struct xldlvtx_t *);
     nsize = 2*osize;
     __siz_xldlvtxtab *= 2;
     __xldlvtxind = (struct xldlvtx_t **) __my_realloc((char *) __xldlvtxind,
      osize, nsize);
   }
  }
 /* allocate and fill new record */
 xldlvp = (struct xldlvtx_t *) __my_malloc(sizeof(struct xldlvtx_t));
 xldlvp->dlnp = np;
 xldlvp->dlbi = bi;
 xldlvp->dlitp = itp;
 
 /* copy downward from end making room at ix_insert */
 for (ki = __num_xldlvtxs - 1; ki > ix_insert; ki--)
  __xldlvtxind[ki] = __xldlvtxind[ki - 1];
 __xldlvtxind[ix_insert] = xldlvp;
 
 return(ix_insert);
}

/*
 * ROUTINES TO FORCE/RELEASE ALL WIRES IN SWITCH CHANNEL
 */

/*
 * qc force wrapper than decomposes vector net object into bit selects
 * result is that for every bit a switch channel is forced
 *
 * know biti down to bitj where values is wid -1 to 0 for wire or psel rng
 * rhsbi is rhs value bit matching low bit j
 * this is called in stmt itree context but if lhs xmr, itp passed
 * and returns also in stmt itree context - called proc handle push/pop
 */
extern void __qc_tran_wireforce(struct net_t *np, int32 biti, int32 bitj,
 int32 rhsbi, struct itree_t *itp, struct st_t *qcfstp)
{
 register int32 bi;
 word32 aval, bval;
 struct xstk_t *xsp;
 struct expr_t *rhsx;

 rhsx = qcfstp->st.sqca->qcrhsx; 
 /* this converts rhs if needed and makes lhs right width */
 /* must eval rhs in itree context of rhs (i.e. statement) */
 xsp = __eval_assign_rhsexpr(rhsx, qcfstp->st.sqca->qclhsx);
 /* do rhs bsel here so pass a/b words with low bit asign bit */ 
 if (rhsbi == -1) rhsbi = 0;

 if (!np->n_isavec)
  {
   /* for scalar biti and bitj will be 0 */
   /* DBG remove -- */
   if (biti != 0 || bitj != 0) __misc_terr(__FILE__, __LINE__);
   /* -- */

   aval = rhsbsel_(xsp->ap, rhsbi);
   bval = rhsbsel_(xsp->bp, rhsbi);
   /* SJM 03/15/01 - now just forcing the one wire - caller re-evals channel */
   /* SJM 04/15/01 - must pass any xmr itree context */
   do_qc_wire_intran_force(np, -1, aval, bval, itp);
  }
 else
  {
   for (bi = biti; bi >= bitj; bi--)
    {
     aval = rhsbsel_(xsp->ap, rhsbi + (bi - bitj));
     bval = rhsbsel_(xsp->bp, rhsbi + (bi - bitj));
     /* SJM 04/15/01 - must pass any xmr itree context */
     do_qc_wire_intran_force(np, bi, aval, bval, itp);
    }
  }
 __pop_xstk();
}

/*
 * do force for one net-bit or scalar in tran channel 
 */
static void do_qc_wire_intran_force(struct net_t *np, int32 biti, word32 aval,
 word32 bval, struct itree_t *lhs_itp)
{
 register int32 ibase;
 int32 nd_itpop;

 /* debug message must use statment not lhs itree context */
 if (__debug_flg && __ev_tracing)
  {
   char s1[RECLEN], s2[RECLEN];

   if (np->n_isavec) sprintf(s1, "%s[%d]", np->nsym->synam, biti); 
   else strcpy(s1, np->nsym->synam);
   __tr_msg(
    ":: quasi-continuous force of switch channel wire %s in %s from force at %s now %s\n",
    s1, __msg2_blditree(__xs2, __inst_ptr),
     __bld_lineloc(__xs, (word32) __sfnam_ind, __slin_cnt),
    __to_timstr(s2, &__simtime));
  }

 nd_itpop = FALSE;
 if (lhs_itp != NULL) { __push_itstk(lhs_itp); nd_itpop = TRUE; }

 /* this must run in itree context of lhs (maybe an xmr) */
 ibase = __inum*np->nwid;
 if (!np->n_isavec)
  {
   __bit1_vpi_or_tran_wireforce(np, &aval, &bval, ibase, 0, 0,
    "switch channel");

   /* DBG remove -- */
   if (biti != -1) __misc_terr(__FILE__, __LINE__); 
   /* --- */
  }
 else
  {
   /* DBG remove -- */
   if (biti == -1) __misc_terr(__FILE__, __LINE__); 
   /* --- */
   /* because right bit value already selected into a/b low bit, rhsbi is 0 */  
   __bit1_vpi_or_tran_wireforce(np, &aval, &bval, ibase, biti, 0,
    "switch channel");
  }

 /* notice can have both many wire specific and many all cbs */
 /* call backs also need to match itree loc for lhs xmr */
 if (__num_vpi_force_cbs > 0) __find_call_force_cbs(np, biti);
 if (__vpi_force_cb_always) __cb_all_rfs(np, biti, TRUE);
 
 if (nd_itpop) __pop_itstk();
}

/*
 * qc release wrapper than decomposes vector net object into bit selects
 * result is that for every bit a switch channel is released
 *
 * SJM 11/24/00 - because no need to get original force value this
 * can use lhs expr and net/bit
 * SJM 04/15/01 - need to pass any lhs itree context if lhs xmr
 */
extern void __qc_tran_wirerelease(struct net_t *np, int32 biti, int32 bitj,
 struct itree_t *itp, struct expr_t *lhsx)
{
 register int32 bi;

 if (!np->n_isavec)
  {
   /* DBG remove -- */
   if (biti != 0 || bitj != 0) __misc_terr(__FILE__, __LINE__);
   /* -- */
   do_qc_wire_intran_release(np, -1, lhsx, itp);
  }
 else
  {
   for (bi = biti; bi >= bitj; bi--)
    { do_qc_wire_intran_release(np, bi, lhsx, itp); }
  }
}

/*
 * do release for one net-bit or scalar in tran channel 
 *
 * this must push lhs xmr itree loc that it is passed if needed 
 * this handles release PLI callbacks if needed
 */
static void do_qc_wire_intran_release(struct net_t *np, int32 biti,
 struct expr_t *lhsx, struct itree_t *itp)
{
 register int32 bi, ibase;
 int32 bit_forced, nd_itpop;
 struct qcval_t *frc_qcp;
 char s1[RECLEN];

 ibase = __inum*np->nwid;
 /* messages must use stmt itree loc */
 for (bi = 0; bi < np->nwid; bi++)
  {
   if (np->nu2.qcval[ibase + bi].qc_active) goto some_bit_forced;
  }
 strcpy(s1, " - no bits forced");
 __sgfinform(465, "attempted release of %s in instance %s failed%s",
  __msgexpr_tostr(__xs2, lhsx), __msg2_blditree(__xs, __inst_ptr), s1);
 /* SJM - 04/15/01 - if nothing forced must not exec any call backs */ 
 return;

some_bit_forced:
 if (__debug_flg && __ev_tracing)
  {
   char s2[RECLEN];

   if (np->n_isavec) sprintf(s1, "%s[%d]", np->nsym->synam, biti); 
   else strcpy(s1, np->nsym->synam);

   __tr_msg(
    ":: quasi-continuous release of switch channel wire %s in %s from release at %s now %s\n",
    s1, __msg2_blditree(__xs, __inst_ptr),
    __bld_lineloc(__xs2, (word32) __sfnam_ind, __slin_cnt),
    __to_timstr(s2, &__simtime));
  }

 nd_itpop = FALSE; 
 if (itp != NULL) { nd_itpop = TRUE; __push_itstk(itp); }

 /* know some forced or will not get here */
 bit_forced = TRUE;
 bi = (biti == -1) ? 0 : biti;
 frc_qcp = &(np->nu2.qcval[ibase + bi]);
 if (!frc_qcp->qc_active) bit_forced = FALSE;
 else frc_qcp->qc_active = FALSE;

 /* now that released, assign value from current drivers */ 
 __assign_1mdrwire(np);

 if (!bit_forced)
  {
   if (nd_itpop) __pop_itstk();

   strcpy(s1, " - some bits forced");
   __sgfinform(465, "attempted release of %s in instance %s failed%s",
    __msgexpr_tostr(__xs2, lhsx), __msg2_blditree(__xs, __inst_ptr), s1);

   if (nd_itpop) __push_itstk(itp);
  } 

 /* FIXME - why not calling for every bit in range - only 1 bit possible? */
 /* notice can have both many wire specific and many all cbs */
 if (__num_vpi_rel_cbs > 0) __find_call_rel_cbs(np, biti);
 if (__vpi_rel_cb_always) __cb_all_rfs(np, biti, FALSE);

 if (nd_itpop) __pop_itstk(); 
}

/*
 * VPI ROUTINES TO FORCE/RELEASE ALL WIRES IN SWITCH CHANNEL
 */

/*
 * vpi force wrapper than decomposes vector net object into bit selects
 * result is that for every bit a switch channel is forced
 * here since comes from vpi passed right force itree context
 */
extern void __tran_wire_vpi_force(struct net_t *np, word32 *ap, word32 *bp,
 int32 ndx) 
{
 register int32 bi;

 if (ndx == -1 && np->n_isavec)
  {
   for (bi = np->nwid - 1; bi >= 0; bi--)
    { do_vpi_wire_intran_force(np, bi, ap, bp); }
  }
 else do_vpi_wire_intran_force(np, ndx, ap, bp);
}

/*
 * do vpi force for one net-bit or scalar in tran channel 
 *
 * know itree change already made if needed (no different lhs context)
 * never a need for separate rhs vpi force since user sets and passes value
 * also do not need dces for these
 */
static void do_vpi_wire_intran_force(struct net_t *np, int32 ndx,
 word32 *ap, word32 *bp)
{
 register int32 ibase;
 char s3[RECLEN]; 

 /* make sure assign/force table exists */ 
 /* AIV 03/09/05 - force inhibition requires allocated bit set */
 if (!np->frc_assgn_allocated) __alloc_qcval(np);

 if (__debug_flg && __ev_tracing)
  {
   if (ndx == -1) strcpy(__wrks1, np->nsym->synam);
   else sprintf(__wrks1, "%s[%d]", np->nsym->synam,
    __unnormalize_ndx(np, ndx));
   __tr_msg(":: vpi_put_value force of wire %s in %s now %s\n", __wrks1,
    __msg2_blditree(__wrks2, __inst_ptr), __to_timstr(s3, &__simtime));
  }

 ibase = __inum*np->nwid;
 if (!np->n_isavec)
  {
   /* SJM 02/23/05 - since scalar do not need ndx check */
   /* SJM 11/14/00 - must also check cbs for scalar case */
   __bit1_vpi_or_tran_wireforce(np, ap, bp, ibase, 0, 0,
    "switch channel vpi_put_value");
   ndx = -1;
   goto chk_cbs;
  }
 /* SJM 02/23/05 - it is legal to force bit selects too */ 
 __bit1_vpi_or_tran_wireforce(np, ap, bp, ibase, ndx, ndx,
  "switch channel vpi_put_value");

chk_cbs:
 /* notice can have both many wire specific and many all cbs */
 if (__num_vpi_force_cbs > 0) __find_call_force_cbs(np, ndx);
 if (__vpi_force_cb_always) __cb_all_rfs(np, ndx, TRUE);
}

/*
 * vpi release wrapper than decomposes vector net object into bit selects
 *
 * result is that for every bit a switch channel released
 * never a need for separate rhs vpi force since user sets and passes value
 * here since comes from vpi passed right force itree context
 */
extern void __tran_wire_vpi_release(struct net_t *np, int32 ndx) 
{
 register int32 bi;

 if (ndx == -1 && np->n_isavec)
  {
   for (bi = np->nwid - 1; bi >= 0; bi--)
    {
     /* SJM 03/15/01 - now release entire wire but nothing else in chan */
     do_vpi_wire_intran_release(np, bi);
     /* after release must re-compute tran channel */
     __eval_tran_1bit(np, bi);
    }
  }
 else
  {
   /* SJM 03/15/01 - now just release one wire */
   do_vpi_wire_intran_release(np, ndx);
   /* since this is scalar eval entire wire */ 
   __eval_tran_bits(np);
  }
}

/*
 * do vpi release for one net-bit or scalar in tran channel 
 * know itree change already made if needed
 */
static void do_vpi_wire_intran_release(struct net_t *np, int32 ndx)
{
 register int32 bi, ibase;
 int32 bit_forced;
 struct qcval_t *frc_qcp;
 char s3[RECLEN];
 
 ibase = __inum*np->nwid;
 if (ndx == -1) bi = 0; else bi = ndx;
 frc_qcp = &(np->nu2.qcval[ibase + bi]);
 if (!frc_qcp->qc_active)
  {
no_force: 
   strcpy(__wrks1, " - not forced");
   if (ndx == -1) strcpy(__wrks2, np->nsym->synam);  
   else sprintf(__wrks2, "%s[%d]", np->nsym->synam,
    __unnormalize_ndx(np, ndx));
   __vpi_err(2102, vpiNotice,
    "attempted vpi_put_value release of wire %s in %s failed%s", __wrks2,
    __msg2_blditree(s3, __inst_ptr), __wrks1);
   goto done;
  }

 if (__debug_flg && __ev_tracing)
  {
   if (ndx == -1) strcpy(__wrks2, np->nsym->synam);  
   else sprintf(__wrks2, "%s[%d]", np->nsym->synam,
    __unnormalize_ndx(np, ndx));
   __tr_msg(":: vpi_put_value release of wire %s in %s now %s\n", __wrks2,
    __msg2_blditree(__wrks1, __inst_ptr), __to_timstr(s3, &__simtime));
  }

 /* know some forced or will not get here */
 bit_forced = TRUE;
 if (!frc_qcp->qc_active) bit_forced = FALSE;
 else frc_qcp->qc_active = FALSE;
 
 /* release is immedate assign even if wire has delay no schedule */
 /* LOOKATME - could this be 1 bit eval? */
 __assign_1mdrwire(np);
 if (!bit_forced && ndx == -1) goto no_force;

done:
 /* notice can have both many wire specific and many all cbs */
 if (__num_vpi_rel_cbs > 0) __find_call_rel_cbs(np, ndx);
 if (__vpi_rel_cb_always) __cb_all_rfs(np, ndx, FALSE);
}

/*
 * vpi force wrapper than decomposes vector net object into bit selects
 * result is that for every bit a switch channel is forced
 */
extern void __tran_exec_putv_wire_softforce(struct net_t *np, word32 *ap,
 word32 *bp, int32 ndx) 
{
 register int32 bi;

 if (ndx == -1 && np->n_isavec)
  {
   for (bi = np->nwid - 1; bi >= 0; bi--)
    { do_putv_wire_intran_softforce(np, bi, ap, bp); }
  }
 else do_putv_wire_intran_softforce(np, ndx, ap, bp);
}

/*
 * do vpi force for one net-bit or scalar in tran channel 
 * know itree change already made if needed
 */
static void do_putv_wire_intran_softforce(struct net_t *np, int32 ndx,
 word32 *ap, word32 *bp)
{
 struct qcval_t *frc_qcp;
 /* case 1: scalar */
 if (!np->n_isavec)
  {
   /* DBG remove -- */
   if (ndx != -1) __misc_terr(__FILE__, __LINE__);
   /* -- */

   /* this add the changed wire to nchglst if needed */
   if (np->nu2.qcval != NULL)
    {
     /* return F if all of wire forced, nothing to do */ 
     /* if T, this will correct bits in ap and bp so actual assign is right */
     if (!__correct_forced_newwireval(np, ap, bp)) return;
    }
   if (np->nchg_nd_chgstore) __chg_st_val(np, ap, bp);
   else __st_val(np, ap, bp);
   return;
  }

 /* DBG remove -- */
 if (ndx == -1) __misc_terr(__FILE__, __LINE__);
 /* -- */

 /* case 2: wire bit select */
 /* if the 1 bit is really forced nothing to do */
 /* SJM 12/23/02 - if not allocated can't be active */
 if (!np->frc_assgn_allocated) return;
 frc_qcp = &(np->nu2.qcval[np->nwid*__inum + ndx]);
 if (frc_qcp->qc_active) return;

 /* else simple bit assign */
 if (np->nchg_nd_chgstore) __chg_st_bit(np, ndx, ap[0], bp[0]);
 else __st_bit(np, ndx, ap[0], bp[0]); 
}
