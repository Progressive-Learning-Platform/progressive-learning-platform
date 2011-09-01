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
 * various code macro definitions
 * mostly for speed in long value processing
 * notice these macros sometimes use globals and call procedures
 * that are assumed to be defined
 */

/* DBG remove - */
extern void __push_itstk(struct itree_t *);
extern void __pop_itstk(void);
/* --- */

/* these make only 1 or 2 percent different in worse case now */
/* RELEASE NOLONGERUSED MAYBE ADD --
#define __push_itstk(itp) \
 do { \
  __itstk[++__itspi] = __inst_ptr = (itp); \
  __inst_mod = __inst_ptr->itip->imsym->el.emdp; \
  __inum = __inst_ptr->itinum; \
 } while (0)
--- */

/* remove assign of nil to inst mod to improve speed if needed */
/* RELEASE NOLONGERUSED MAYBE ADD --
#define __pop_itstk() \
 do { \
  __inst_ptr = __itstk[--__itspi]; \
  if (__itspi != -1) \
   { \
    __inst_mod = __inst_ptr->itip->imsym->el.emdp; \
    __inum = __inst_ptr->itinum; \
   } \
  -* else { __inst_mod = NULL; __inum = 0xffffffff; } *- \
  else __inst_mod = NULL; \
 } while (0)
-- */

/* DBG ??? add --
extern void __pop_xstk(void);
-- */

/* --- release add */
#define __pop_xstk() __xspi--
/* -- */

/* DBG ??? add -- 
extern struct xstk_t *__push_xstk(int32);
#define push_xstk_(xsp, blen) (xsp) = __push_xstk((blen))
--- */

/* BEWARE - a and b parts must be contiguous because often refed as only a */ 
/*--- release add --- */
#define push_xstk_(xsp, blen) \
 do { \
  if (++__xspi >= __maxxnest) __grow_xstk(); \
  (xsp) = __xstk[__xspi]; \
  if (wlen_(blen) > (xsp)->xsawlen) __chg_xstk_width((xsp), wlen_(blen)); \
  (xsp)->bp = &((xsp)->ap[wlen_(blen)]); \
  (xsp)->xslen = (blen); \
 } while (0)
/* --- */

/* DBG ??? add --
extern struct xstk_t *__eval_xpr(struct expr_t *);
--- */
/* --- release add */
#define __eval_xpr __eval2_xpr
/* --- */

/* DBG ??? add --- */
extern i_tev_ndx __alloc_tev(int32, struct itree_t *, word64);
#define alloc_tev_(tevpi, etyp, itp, absetime) \
 (tevpi) = __alloc_tev((etyp), (itp), (absetime))
/*--- */

/* -- ??? release add
#define alloc_tev_(tevpi, etyp, itp, absetime) \
 do { \
  register struct tev_t *tevp__; \
  if (__tefreelsti != -1) \
   { tevpi = __tefreelsti; __tefreelsti = __tevtab[__tefreelsti].tenxti; } \
  else \
   { \
    if (++__numused_tevtab >= __size_tevtab) __grow_tevtab(); \
    tevpi = __numused_tevtab; \
   } \
  tevp__ = &(__tevtab[tevpi]); \
  memset(tevp__, 0, sizeof(struct tev_t)); \
  tevp__->tetyp = etyp; \
  tevp__->teitp = itp; \
  tevp__->etime = absetime; \
  tevp__->tenxti = -1; \
 } while (0)
-- */

#define ld_scalval_(ap, bp, bytp) \
 do { \
  (ap)[0] = (word32) ((bytp)[__inum]); \
  (bp)[0] = ((ap)[0] >> 1) & 1L; \
  (ap)[0] &= 1L; \
 } while(0)

/* LOOKATME _ notice this assumes av and bv values only occupy low bit */ 
#define st_scalval_(bp, av, bv) \
 (bp)[__inum] = (byte) ((av) | ((bv) << 1))

#define st2_scalval_(bp, val) (bp)[__inum] = (byte) (val)

#define chg_st_scalval_(bp, av, bv) \
 do { \
  register word32 nval__; \
  nval__ = (av) | ((bv) << 1); \
  if (((word32) (bp)[__inum]) != (nval__)) \
   { (bp)[__inum] = (byte) (nval__); __lhs_changed = TRUE; } \
 } while(0)

#define get_stwire_addr_(sbp, np) \
 (sbp) = &((np)->nva.bp[np->nwid*__inum])

/* DBG ??? add --
extern void __record_nchg(struct net_t *);
#define record_nchg_(np) __record_nchg(np)
--- */

/* SJM 08/08/03 - can't assume caller turns off chged flag any more */
/* but one record called, it must be off for dctrl processing - not needed */ 

/* recording var change macros */
/* DBG ??? release add --- */
#define record_nchg_(np) \
 do { \
  __lhs_changed = FALSE; \
  if (np->nchg_has_dces) \
   __wakeup_delay_ctrls(np, -1, -1); \
  if (((np)->nchgaction[__inum] & NCHG_ALL_CHGED) == 0) __add_nchglst_el(np); \
  if (((np)->nchgaction[__inum] & (NCHG_DMPVARNOW | NCHG_DMPVNOTCHGED)) \
   == (NCHG_DMPVARNOW | NCHG_DMPVNOTCHGED)) \
   { \
    (np)->nchgaction[__inum] &= ~(NCHG_DMPVNOTCHGED); \
    __add_dmpv_chglst_el(np); \
   } \
 } while (0) 

/* --- */

/* DBG ??? add --- 
extern void __record_sel_nchg(struct net_t *, int32, int32);
#define record_sel_nchg_(np, i1, i2) __record_sel_nchg(np, i1, i2)
 --- */

/* SJM 08/08/03 - can't assume caller turns off chged flag any more */
/* but one record called, it must be off for dctrl processing - not needed */ 
/* DBG ??? release add --- */
#define record_sel_nchg_(np, i1, i2) \
 do { \
  __lhs_changed = FALSE; \
  if ((np)->dcelst != NULL) __wakeup_delay_ctrls(np, (i1), (i2)); \
  if (((np)->nchgaction[__inum] & NCHG_ALL_CHGED) == 0) \
   __add_select_nchglst_el((np), (i1), (i2)); \
  if (((np)->nchgaction[__inum] & (NCHG_DMPVARNOW | NCHG_DMPVNOTCHGED)) \
   == (NCHG_DMPVARNOW | NCHG_DMPVNOTCHGED)) \
   { \
    (np)->nchgaction[__inum] &= ~(NCHG_DMPVNOTCHGED); \
    __add_dmpv_chglst_el(np); \
   } \
 } while (0) 
/* --- */

/* conversion macros */
#define valtoch_(c) (((c) < 10) ? '0' + (c) : 'a' + (c) - 10)
/* conversion from real to int32 in Verilog is rounding away from 0 */
#define ver_rint_(x) ((int32) (((x) >= 0.0) \
  ? ((x) + 0.500000000) : ((x) - 0.500000000))) 
#define ver_rword(x) ((word32) ((x) + 0.500000000))

/* convert a 64 bit delay to no. of sim ticks */ 
#define cnv_num64to_ticks_(tickstim, inttim, mdp) \
 (tickstim) = __itoticks_tab[__des_timeprec - (mdp)->mtime_units]*(inttim)

/* value extraction macros */
#define get_packintowrd_(nva, inum, vecw) \
 (((vecw) <= 4) ? ((word32) (nva).bp[(inum)]) \
 : (((vecw) > 8) ? (nva).wp[(inum)] : (word32) (nva).hwp[(inum)]))

#define st_packintowrd_(nva, ind, uwrd, vecw) \
 (((vecw) <= 4) ? ((nva).bp[(ind)] = (byte) (uwrd)) \
 : (((vecw) > 8) ? ((nva).wp[(ind)] = (uwrd)) : ((nva).hwp[(ind)] = (hword) (uwrd))))

#define ubits_(blen) ((blen) & 0x1f)
#define wlen_(blen) (((blen) + 31) >> 5)
#define get_wofs_(bi) ((bi) >> 5)
#define get_bofs_(bi) ((bi) & 0x1f)

/* version of ubits that returns 32 for all used not 0 */
#define ubits2_(blen) (((blen & 0x1f) == 0) ? WBITS : (blen & 0x1f))

/* get conducting tranif state for cur. instance (for non input 3rd gate) */
/* possibilities are 1 on, 0 off, or 3 unknown */
#define get_tranif_onoff_(gp) \
  (((gp)->gstate.wp[get_wofs_(2*__inum)] >> get_bofs_(2*__inum)) & 3L)

/* -- unused */
/* #define cmpxlen(w1, b1, w2, b2) \ */
/*  (((w1)==(w2)) ? ((b1) - (b2)) : ((w1) - (w2))) */

/* comparison macros */
#define cvt_wrdbool_(av, bv)  \
 ((((av) & ~(bv)) != 0L) ? 1 : (((bv) != 0L) ? 3 : 0))

#define vval_is0_(wp, blen) \
 (((blen) <= WBITS) ? (*(wp) == 0L) : __wide_vval_is0((wp), (blen)))

/* comparing entire 2 wlen section - unused in both must be 0 */
#define cmp_vval_(wp1, wp2, blen) \
 memcmp((wp2), (wp1), (WRDBYTES*(wlen_(blen))))
#define cmp_wvval_(wp1, wp2, wlen) memcmp((wp2), (wp1), (WRDBYTES*wlen))

/* bit load and store macros */
#define rhsbsel_(wp, bi) ((wp)[get_wofs_(bi)] >> get_bofs_(bi)) & 1L

/* value change macros - can't assume no overlap here */
#define cp_walign_(dwp, swp, blen) \
 memmove((dwp), (swp), (int32) (WRDBYTES*(wlen_(blen)))); \
 (dwp)[wlen_(blen) - 1] &= __masktab[ubits_(blen)]

#define zero_allbits_(wp, blen) memset(((wp)), 0, \
 (int32) (WRDBYTES*(wlen_(blen))))

#define one_allbits_(wp, blen) \
 do { register int32 __i; \
  for (__i = 0; __i < wlen_(blen); __i++) (wp)[__i] = ALL1W; \
  (wp)[(wlen_(blen)) - 1] &= __masktab[ubits_(blen)]; \
 } while (0)

/* macro must be passed a byte pointer */
#define set_byteval_(sbp, len, stval) \
 do { register int32 __i; \
  for (__i = 0; __i < (len); __i++) (sbp)[__i] = ((byte) (stval)); \
 } while (0)

/* this macro may need to be surrounded by { } in some places */
#define set_regtox_(ap, bp, len) \
 one_allbits_((ap), len); one_allbits_((bp), len);

/* map index from source [i1:i2] to internal h:0 */
#define normalize_ndx_(ndxval, mi1, mi2) \
 (((mi1) >= (mi2)) ? (ndxval - mi2) : (mi2 - ndxval))

/* misc macros */
#define reg_fr_inhibit_(np) \
 (np->nu2.qcval[2*__inum].qc_active \
 || np->nu2.qcval[2*__inum + 1].qc_active)

/* i/o macros */

#define vis_white_(c) \
 ((((__pv_ctv = __pv_ctab[(c) & 0x7f]) == 1) || __pv_ctv == 3) ? TRUE : FALSE)

#define vis_nonnl_white_(c) ((__pv_ctab[(c) & 0x7f] == 1) ? TRUE : FALSE)

/* notice this does not leave line null terminated */
#define addch_(ch) \
 do { \
  if (__cur_sofs >= __exprlinelen - 1) __chg_xprline_size(1); \
  __exprline[__cur_sofs++] = (ch); \
 } while (0)

#define my_puts_(s, f) \
 do { \
   fputs((s), (f)); \
   if ((f) == stdout && __log_s != NULL) fputs((s), __log_s); \
  } while (0)

#define my_putc_(c, f) \
 do { \
   fputc((c), (f)); \
   if ((f) == stdout && __log_s != NULL) fputc((c), __log_s); \
  } while (0)

#define my_ungetc_(c, f) \
 if (f == NULL) \
 { if (c == EOF) *__visp->vichp = '\0'; else *(--__visp->vichp) = c; } \
 else ungetc(c, f)

/* must follow convention only immediately read char can be pushed back */
/* --- DBG remove --- 
#define my_ungetc_(c, f) \
  if (f == NULL) \
   { \
    if (c == EOF) \
     { \
      if (*__visp->vichp != '\0') __arg_terr(__FILE__, __LINE__); \
      *__visp->vichp = '\0'; \
     } \
    else \
     { \
      (__visp->vichp)--; \
      if (*__visp->vichp != c) __arg_terr(__FILE__, __LINE__); \
      *(__visp->vichp) = c; \
     } \
   } \
  else ungetc(c, f) \
---- */ 
