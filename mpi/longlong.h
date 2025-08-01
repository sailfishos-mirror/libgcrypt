/* longlong.h -- definitions for mixed size 32/64 bit arithmetic.
   Note: This is the Libgcrypt version


Copyright (C) 1991, 1992, 1993, 1994, 1996, 1998,
              2000, 2001, 2002, 2003, 2004, 2011 Free Software Foundation, Inc.

This file is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with this file; see the file COPYING.LIB.  If not, see <https://www.gnu.org/licenses/>.
SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "const-time.h"

/* On 32-bit, use 64-bit 'unsigned long long' for UDWtype, if available. */
#if !defined (UDWtype) && SIZEOF_UNSIGNED_LONG_LONG * 8 == W_TYPE_SIZE * 2
#  define UDWtype unsigned long long
#endif

/* On 64-bit, use 128-bit 'unsigned __int128' for UDWtype, if available. */
#if !defined (UDWtype) && SIZEOF_UNSIGNED___INT128 * 8 == W_TYPE_SIZE * 2
#  define UDWtype unsigned __int128
#endif

/* You have to define the following before including this file:

   UWtype -- An unsigned type, default type for operations (typically a "word").
   UHWtype -- An unsigned type, at least half the size of UWtype.
   UDWtype -- An unsigned type, at least twice as large a UWtype.
   W_TYPE_SIZE -- size in bits of UWtype.

   SItype, USItype -- Signed and unsigned 32 bit types.
   DItype, UDItype -- Signed and unsigned 64 bit types.

   On a 32 bit machine UWtype should typically be USItype.
   On a 64 bit machine, UWtype should typically be UDItype.
*/

#define __BITS4 (W_TYPE_SIZE / 4)
#define __ll_B ((UWtype) 1 << (W_TYPE_SIZE / 2))
#define __ll_lowpart(t) ((UWtype) (t) & (__ll_B - 1))
#define __ll_highpart(t) ((UWtype) (t) >> (W_TYPE_SIZE / 2))

/* This is used to make sure no undesirable sharing between different libraries
   that use this file takes place.  */
#ifndef __MPN
# define __MPN(x) __##x
#endif

/* Define auxiliary asm macros.

   1) umul_ppmm(high_prod, low_prod, multipler, multiplicand) multiplies two
   UWtype integers MULTIPLER and MULTIPLICAND, and generates a two UWtype
   word product in HIGH_PROD and LOW_PROD.

   2) __umulsidi3(a,b) multiplies two UWtype integers A and B, and returns a
   UDWtype product.  This is just a variant of umul_ppmm.

   3) udiv_qrnnd(quotient, remainder, high_numerator, low_numerator,
   denominator) divides a UDWtype, composed by the UWtype integers
   HIGH_NUMERATOR and LOW_NUMERATOR, by DENOMINATOR and places the quotient
   in QUOTIENT and the remainder in REMAINDER.	HIGH_NUMERATOR must be less
   than DENOMINATOR for correct operation.  If, in addition, the most
   significant bit of DENOMINATOR must be 1, then the pre-processor symbol
   UDIV_NEEDS_NORMALIZATION is defined to 1.

   4) sdiv_qrnnd(quotient, remainder, high_numerator, low_numerator,
   denominator).  Like udiv_qrnnd but the numbers are signed.  The quotient
   is rounded towards 0.

   5) count_leading_zeros(count, x) counts the number of zero-bits from the
   msb to the first non-zero bit in the UWtype X.  This is the number of
   steps X needs to be shifted left to set the msb.  Undefined for X == 0,
   unless the symbol COUNT_LEADING_ZEROS_0 is defined to some value.

   6) count_trailing_zeros(count, x) like count_leading_zeros, but counts
   from the least significant end.

   7) add_ssaaaa(high_sum, low_sum, high_addend_1, low_addend_1,
   high_addend_2, low_addend_2) adds two UWtype integers, composed by
   HIGH_ADDEND_1 and LOW_ADDEND_1, and HIGH_ADDEND_2 and LOW_ADDEND_2
   respectively.  The result is placed in HIGH_SUM and LOW_SUM.  Overflow
   (i.e. carry out) is not stored anywhere, and is lost.

   8) sub_ddmmss(high_difference, low_difference, high_minuend, low_minuend,
   high_subtrahend, low_subtrahend) subtracts two two-word UWtype integers,
   composed by HIGH_MINUEND_1 and LOW_MINUEND_1, and HIGH_SUBTRAHEND_2 and
   LOW_SUBTRAHEND_2 respectively.  The result is placed in HIGH_DIFFERENCE
   and LOW_DIFFERENCE.	Overflow (i.e. carry out) is not stored anywhere,
   and is lost.

   If any of these macros are left undefined for a particular CPU,
   C macros are used.  */

/* The CPUs come in alphabetical order below.

   Please add support for more CPUs here, or improve the current support
   for the CPUs below!	*/

#ifdef __riscos__
#pragma continue_after_hash_error
#else /* !__riscos__ */
#if defined (__GNUC__) && !defined (NO_ASM)

/* We sometimes need to clobber "cc" with gcc2, but that would not be
   understood by gcc1.	Use cpp to avoid major code duplication.  */
#if __GNUC__ < 2
# define __CLOBBER_CC
# define __AND_CLOBBER_CC
#else /* __GNUC__ >= 2 */
# define __CLOBBER_CC : "cc"
# define __AND_CLOBBER_CC , "cc"
#endif /* __GNUC__ < 2 */

/***************************************
 ****  Begin CPU Specific Versions  ****
 ***************************************/

/***************************************
 **************  A29K  *****************
 ***************************************/
#if (defined (__a29k__) || defined (_AM29K)) && W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("add %1,%4,%5\n"   \
           "addc %0,%2,%3"                                              \
	   : "=r" ((USItype)(sh)),                                      \
	    "=&r" ((USItype)(sl))                                       \
	   : "%r" ((USItype)(ah)),                                      \
	     "rI" ((USItype)(bh)),                                      \
	     "%r" ((USItype)(al)),                                      \
	     "rI" ((USItype)(bl)))
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("sub %1,%4,%5\n"                                             \
	   "subc %0,%2,%3"                                              \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "r" ((USItype)(ah)),                                       \
	     "rI" ((USItype)(bh)),                                      \
	     "r" ((USItype)(al)),                                       \
	     "rI" ((USItype)(bl)))
# define umul_ppmm(xh, xl, m0, m1) \
  do {									\
    USItype __m0 = (m0), __m1 = (m1);					\
    __asm__ ("multiplu %0,%1,%2"                                        \
	     : "=r" ((USItype)(xl))                                     \
	     : "r" (__m0),                                              \
	       "r" (__m1));                                             \
    __asm__ ("multmu %0,%1,%2"                                          \
	     : "=r" ((USItype)(xh))                                     \
	     : "r" (__m0),                                              \
	       "r" (__m1));                                             \
  } while (0)
# define udiv_qrnnd(q, r, n1, n0, d) \
  __asm__ ("dividu %0,%3,%4"                                            \
	   : "=r" ((USItype)(q)),                                       \
	     "=q" ((USItype)(r))                                        \
	   : "1" ((USItype)(n1)),                                       \
	     "r" ((USItype)(n0)),                                       \
	     "r" ((USItype)(d)))
# define count_leading_zeros(count, x) \
    __asm__ ("clz %0,%1"                                                \
	     : "=r" ((USItype)(count))                                  \
	     : "r" ((USItype)(x)))
# define COUNT_LEADING_ZEROS_0 32
#endif /* __a29k__ */


#if defined (__alpha) && W_TYPE_SIZE == 64
# define umul_ppmm(ph, pl, m0, m1) \
  do {									\
    UDItype __m0 = (m0), __m1 = (m1);					\
    __asm__ ("umulh %r1,%2,%0"                                          \
	     : "=r" ((UDItype)(ph))                                     \
	     : "%rJ" (__m0),                                            \
	       "rI" (__m1));                                            \
    (pl) = __m0 * __m1; 						\
  } while (0)
# define UMUL_TIME 46
# ifndef LONGLONG_STANDALONE
#  define udiv_qrnnd(q, r, n1, n0, d) \
  do { UDItype __r;							\
    (q) = __udiv_qrnnd (&__r, (n1), (n0), (d)); 			\
    (r) = __r;								\
  } while (0)
extern UDItype __udiv_qrnnd (UDItype *, UDItype, UDItype, UDItype);
#  define UDIV_TIME 220
# endif /* !LONGLONG_STANDALONE */
#endif /* __alpha */

/***************************************
 **************  ARM  ******************
 ***************************************/
#if defined (__arm__) && W_TYPE_SIZE == 32 && \
    (!defined (__thumb__) || defined (__thumb2__))
/* The __ARM_ARCH define is provided by gcc 4.8.  Construct it otherwise.  */
# ifndef __ARM_ARCH
#  ifdef __ARM_ARCH_2__
#   define __ARM_ARCH 2
#  elif defined (__ARM_ARCH_3__) || defined (__ARM_ARCH_3M__)
#   define __ARM_ARCH 3
#  elif defined (__ARM_ARCH_4__) || defined (__ARM_ARCH_4T__)
#   define __ARM_ARCH 4
#  elif defined (__ARM_ARCH_5__) || defined (__ARM_ARCH_5E__) \
        || defined(__ARM_ARCH_5T__) || defined(__ARM_ARCH_5TE__) \
        || defined(__ARM_ARCH_5TEJ__)
#   define __ARM_ARCH 5
#  elif defined (__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) \
        || defined (__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__) \
        || defined (__ARM_ARCH_6K__) || defined(__ARM_ARCH_6T2__)
#   define __ARM_ARCH 6
#  elif defined (__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) \
        || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) \
        || defined(__ARM_ARCH_7EM__)
#   define __ARM_ARCH 7
#  else
   /* could not detect? */
#  endif
# endif /* !__ARM_ARCH */

# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("adds %1, %4, %5\n"                                          \
	   "adc  %0, %2, %3"                                            \
	   : "=r" ((sh)),                                               \
	     "=&r" ((sl))                                               \
	   : "%r" ((USItype)(ah)),                                      \
	     "rI" ((USItype)(bh)),                                      \
	     "%r" ((USItype)(al)),                                      \
	     "rI" ((USItype)(bl)) __CLOBBER_CC)
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("subs %1, %4, %5\n"                                          \
	   "sbc  %0, %2, %3"                                            \
	   : "=r" ((sh)),                                               \
	     "=&r" ((sl))                                               \
	   : "r" ((USItype)(ah)),                                       \
	     "rI" ((USItype)(bh)),                                      \
	     "r" ((USItype)(al)),                                       \
	     "rI" ((USItype)(bl)) __CLOBBER_CC)
# if (defined __ARM_ARCH && __ARM_ARCH <= 3)
#  define umul_ppmm(xh, xl, a, b) \
  __asm__ ("@ Inlined umul_ppmm\n"                                      \
	"mov	%|r0, %2, lsr #16		@ AAAA\n"               \
	"mov	%|r2, %3, lsr #16		@ BBBB\n"               \
	"bic	%|r1, %2, %|r0, lsl #16		@ aaaa\n"               \
	"bic	%0, %3, %|r2, lsl #16		@ bbbb\n"               \
	"mul	%1, %|r1, %|r2			@ aaaa * BBBB\n"        \
	"mul	%|r2, %|r0, %|r2		@ AAAA * BBBB\n"        \
	"mul	%|r1, %0, %|r1			@ aaaa * bbbb\n"        \
	"mul	%0, %|r0, %0			@ AAAA * bbbb\n"        \
	"adds	%|r0, %1, %0			@ central sum\n"        \
	"addcs	%|r2, %|r2, #65536\n"                                   \
	"adds	%1, %|r1, %|r0, lsl #16\n"                              \
	"adc	%0, %|r2, %|r0, lsr #16"                                \
	   : "=&r" ((xh)),                                              \
	     "=r" ((xl))                                                \
	   : "r" ((USItype)(a)),                                        \
	     "r" ((USItype)(b))                                         \
	   : "r0", "r1", "r2" __AND_CLOBBER_CC)
# else /* __ARM_ARCH >= 4 */
#  define umul_ppmm(xh, xl, a, b)                                         \
  __asm__ ("@ Inlined umul_ppmm\n"                                      \
	   "umull %1, %0, %2, %3"                                       \
		   : "=&r" ((xh)),                                      \
		     "=r" ((xl))                                        \
		   : "r" ((USItype)(a)),                                \
		     "r" ((USItype)(b)))
# endif /* __ARM_ARCH >= 4 */
# define UMUL_TIME 20
# define UDIV_TIME 100
# if (defined __ARM_ARCH && __ARM_ARCH >= 5)
#  define count_leading_zeros(count, x) \
  __asm__ ("clz %0, %1"                                                 \
		   : "=r" ((count))                                     \
		   : "r" ((USItype)(x)))
# endif /* __ARM_ARCH >= 5 */
#endif /* __arm__ */

/***************************************
 **********  ARM64 / Aarch64  **********
 ***************************************/
#if defined(__aarch64__) && W_TYPE_SIZE == 64
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("adds %1, %4, %5\n"                                          \
           "adc  %0, %2, %3\n"                                          \
           : "=r" ((sh)),                                               \
             "=&r" ((sl))                                               \
           : "r" ((UDItype)(ah)),                                       \
             "r" ((UDItype)(bh)),                                       \
             "r" ((UDItype)(al)),                                       \
             "r" ((UDItype)(bl)) __CLOBBER_CC)
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("subs %1, %4, %5\n"                                          \
           "sbc  %0, %2, %3\n"                                          \
           : "=r" ((sh)),                                               \
             "=&r" ((sl))                                               \
           : "r" ((UDItype)(ah)),                                       \
             "r" ((UDItype)(bh)),                                       \
             "r" ((UDItype)(al)),                                       \
             "r" ((UDItype)(bl)) __CLOBBER_CC)
# define umul_ppmm(ph, pl, m0, m1) \
  do {                                                                  \
    UDItype __m0 = (m0), __m1 = (m1), __ph;                             \
    (pl) = __m0 * __m1;                                                 \
    __asm__ ("umulh %0,%1,%2"                                           \
             : "=r" (__ph)                                              \
             : "r" (__m0),                                              \
               "r" (__m1));                                             \
    (ph) = __ph; \
  } while (0)
# define count_leading_zeros(count, x) \
  do {                                                                  \
    UDItype __co;                                                       \
    __asm__ ("clz %0, %1\n"                                             \
             : "=r" (__co)                                              \
             : "r" ((UDItype)(x)));                                     \
    (count) = __co;                                                     \
  } while (0)
#endif /* __aarch64__ */

/***************************************
 **************  CLIPPER  **************
 ***************************************/
#if defined (__clipper__) && W_TYPE_SIZE == 32
# define umul_ppmm(w1, w0, u, v) \
  ({union {UDItype __ll;						\
	   struct {USItype __l, __h;} __i;				\
	  } __xx;							\
  __asm__ ("mulwux %2,%0"                                               \
	   : "=r" (__xx.__ll)                                           \
	   : "%0" ((USItype)(u)),                                       \
	     "r" ((USItype)(v)));                                       \
  (w1) = __xx.__i.__h; (w0) = __xx.__i.__l;})
# define smul_ppmm(w1, w0, u, v) \
  ({union {DItype __ll; 						\
	   struct {SItype __l, __h;} __i;				\
	  } __xx;							\
  __asm__ ("mulwx %2,%0"                                                \
	   : "=r" (__xx.__ll)                                           \
	   : "%0" ((SItype)(u)),                                        \
	     "r" ((SItype)(v)));                                        \
  (w1) = __xx.__i.__h; (w0) = __xx.__i.__l;})
# define __umulsidi3(u, v) \
  ({UDItype __w;							\
    __asm__ ("mulwux %2,%0"                                             \
	     : "=r" (__w)                                               \
	     : "%0" ((USItype)(u)),                                     \
	       "r" ((USItype)(v)));                                     \
    __w; })
#endif /* __clipper__ */


/***************************************
 **************  GMICRO  ***************
 ***************************************/
#if defined (__gmicro__) && W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("add.w %5,%1\n"                                              \
	   "addx %3,%0"                                                 \
	   : "=g" ((USItype)(sh)),                                      \
	     "=&g" ((USItype)(sl))                                      \
	   : "%0" ((USItype)(ah)),                                      \
	     "g" ((USItype)(bh)),                                       \
	     "%1" ((USItype)(al)),                                      \
	     "g" ((USItype)(bl)))
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("sub.w %5,%1\n"                                              \
	   "subx %3,%0"                                                 \
	   : "=g" ((USItype)(sh)),                                      \
	     "=&g" ((USItype)(sl))                                      \
	   : "0" ((USItype)(ah)),                                       \
	     "g" ((USItype)(bh)),                                       \
	     "1" ((USItype)(al)),                                       \
	     "g" ((USItype)(bl)))
# define umul_ppmm(ph, pl, m0, m1) \
  __asm__ ("mulx %3,%0,%1"                                              \
	   : "=g" ((USItype)(ph)),                                      \
	     "=r" ((USItype)(pl))                                       \
	   : "%0" ((USItype)(m0)),                                      \
	     "g" ((USItype)(m1)))
# define udiv_qrnnd(q, r, nh, nl, d) \
  __asm__ ("divx %4,%0,%1"                                              \
	   : "=g" ((USItype)(q)),                                       \
	     "=r" ((USItype)(r))                                        \
	   : "1" ((USItype)(nh)),                                       \
	     "0" ((USItype)(nl)),                                       \
	     "g" ((USItype)(d)))
# define count_leading_zeros(count, x) \
  __asm__ ("bsch/1 %1,%0"                                               \
	   : "=g" (count)                                               \
	   : "g" ((USItype)(x)),                                        \
	     "0" ((USItype)0))
#endif


/***************************************
 **************  HPPA  *****************
 ***************************************/
#if defined (__hppa) && W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("add %4,%5,%1\n\t"                                           \
	   "addc %2,%3,%0"                                              \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "%rM" ((USItype)(ah)),                                     \
	     "rM" ((USItype)(bh)),                                      \
	     "%rM" ((USItype)(al)),                                     \
	     "rM" ((USItype)(bl)) __CLOBBER_CC)
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("sub %4,%5,%1\n\t"                                           \
	   "subb %2,%3,%0"                                              \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "rM" ((USItype)(ah)),                                      \
	     "rM" ((USItype)(bh)),                                      \
	     "rM" ((USItype)(al)),                                      \
	     "rM" ((USItype)(bl)) __CLOBBER_CC)
# if defined (_PA_RISC1_1)
#  define umul_ppmm(wh, wl, u, v) \
  do {									\
    union {UDItype __ll;						\
	   struct {USItype __h, __l;} __i;				\
	  } __xx;							\
    __asm__ ("	xmpyu %1,%2,%0"                                           \
	     : "=*f" (__xx.__ll)                                        \
	     : "*f" ((USItype)(u)),                                     \
	       "*f" ((USItype)(v)));                                    \
    (wh) = __xx.__i.__h;						\
    (wl) = __xx.__i.__l;						\
  } while (0)
#  define UMUL_TIME 8
#  define UDIV_TIME 60
# else
#  define UMUL_TIME 40
#  define UDIV_TIME 80
# endif
# if !defined(LONGLONG_STANDALONE) && !defined(ASM_DISABLED)
#  define udiv_qrnnd(q, r, n1, n0, d) \
  do { USItype __r;							\
    (q) = __udiv_qrnnd (&__r, (n1), (n0), (d)); 			\
    (r) = __r;								\
  } while (0)
extern USItype __udiv_qrnnd (USItype *, USItype, USItype, USItype);
# endif /* !LONGLONG_STANDALONE && !ASM_DISABLED */
# define count_leading_zeros(count, x) \
  do {								       \
    USItype __tmp;						       \
    __asm__ (				                               \
       "	ldi             1,%0                                       \n" \
       "	extru,= 	%1,15,16,%%r0  ; Bits 31..16 zero?         \n" \
       "	extru,tr	%1,15,16,%1    ; No.  Shift down, skip add.\n" \
       "	ldo		16(%0),%0      ; Yes.	Perform add.       \n" \
       "	extru,= 	%1,23,8,%%r0   ; Bits 15..8 zero?          \n" \
       "	extru,tr	%1,23,8,%1     ; No.  Shift down, skip add.\n" \
       "	ldo		8(%0),%0       ; Yes.	Perform add.       \n" \
       "	extru,= 	%1,27,4,%%r0   ; Bits 7..4 zero?           \n" \
       "	extru,tr	%1,27,4,%1     ; No.  Shift down, skip add.\n" \
       "	ldo		4(%0),%0       ; Yes.	Perform add.       \n" \
       "	extru,= 	%1,29,2,%%r0   ; Bits 3..2 zero?           \n" \
       "	extru,tr	%1,29,2,%1     ; No.  Shift down, skip add.\n" \
       "	ldo		2(%0),%0       ; Yes.	Perform add.       \n" \
       "	extru		%1,30,1,%1     ; Extract bit 1.            \n" \
       "	sub		%0,%1,%0       ; Subtract it.              "   \
       : "=r" (count), "=r" (__tmp) : "1" (x));                        \
  } while (0)
#endif /* hppa */


/***************************************
 **************  I370  *****************
 ***************************************/
#if (defined (__i370__) || defined (__mvs__)) && W_TYPE_SIZE == 32
# define umul_ppmm(xh, xl, m0, m1) \
  do {									\
    union {UDItype __ll;						\
	   struct {USItype __h, __l;} __i;				\
	  } __xx;							\
    USItype __m0 = (m0), __m1 = (m1);					\
    __asm__ ("mr %0,%3"                                                 \
	     : "=r" (__xx.__i.__h),                                     \
	       "=r" (__xx.__i.__l)                                      \
	     : "%1" (__m0),                                             \
	       "r" (__m1));                                             \
    (xh) = __xx.__i.__h; (xl) = __xx.__i.__l;				\
    (xh) += ((((SItype) __m0 >> 31) & __m1)				\
	     + (((SItype) __m1 >> 31) & __m0)); 			\
  } while (0)
# define smul_ppmm(xh, xl, m0, m1) \
  do {									\
    union {DItype __ll; 						\
	   struct {USItype __h, __l;} __i;				\
	  } __xx;							\
    __asm__ ("mr %0,%3"                                                 \
	     : "=r" (__xx.__i.__h),                                     \
	       "=r" (__xx.__i.__l)                                      \
	     : "%1" (m0),                                               \
	       "r" (m1));                                               \
    (xh) = __xx.__i.__h; (xl) = __xx.__i.__l;				\
  } while (0)
# define sdiv_qrnnd(q, r, n1, n0, d) \
  do {									\
    union {DItype __ll; 						\
	   struct {USItype __h, __l;} __i;				\
	  } __xx;							\
    __xx.__i.__h = (n1); __xx.__i.__l = (n0);				\
    __asm__ ("dr %0,%2"                                                 \
	     : "=r" (__xx.__ll)                                         \
	     : "0" (__xx.__ll), "r" (d));                               \
    (q) = __xx.__i.__l; (r) = __xx.__i.__h;				\
  } while (0)
#endif


/***************************************
 **************  I386  *****************
 ***************************************/
#if (defined (__i386__) || defined (__i486__)) && W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("addl %5,%1\n"                                               \
	   "adcl %3,%0"                                                 \
	   : "=r" ((sh)),                                               \
	     "=&r" ((sl))                                               \
	   : "%0" ((USItype)(ah)),                                      \
	     "g" ((USItype)(bh)),                                       \
	     "%1" ((USItype)(al)),                                      \
	     "g" ((USItype)(bl))                                        \
	   __CLOBBER_CC)
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("subl %5,%1\n"                                               \
	   "sbbl %3,%0"                                                 \
	   : "=r" ((sh)),                                               \
	     "=&r" ((sl))                                               \
	   : "0" ((USItype)(ah)),                                       \
	     "g" ((USItype)(bh)),                                       \
	     "1" ((USItype)(al)),                                       \
	     "g" ((USItype)(bl))                                        \
	   __CLOBBER_CC)
# define umul_ppmm(w1, w0, u, v) \
  __asm__ ("mull %3"                                                    \
	   : "=a" ((w0)),                                               \
	     "=d" ((w1))                                                \
	   : "%0" ((USItype)(u)),                                       \
	     "rm" ((USItype)(v))                                        \
	   __CLOBBER_CC)
# define udiv_qrnnd(q, r, n1, n0, d) \
  __asm__ ("divl %4"                                                    \
	   : "=a" ((q)),                                                \
	     "=d" ((r))                                                 \
	   : "0" ((USItype)(n0)),                                       \
	     "1" ((USItype)(n1)),                                       \
	     "rm" ((USItype)(d))                                        \
	   __CLOBBER_CC)
# define count_leading_zeros(count, x) \
  do {									\
    USItype __cbtmp;							\
    __asm__ ("bsrl %1,%0"                                               \
	     : "=r" (__cbtmp) : "rm" ((USItype)(x))                     \
	     __CLOBBER_CC);						\
    (count) = __cbtmp ^ 31;						\
  } while (0)
# define count_trailing_zeros(count, x) \
  __asm__ ("rep;bsfl %1,%0" : "=r" (count) : "rm" ((USItype)(x)) __CLOBBER_CC)
# ifndef UMUL_TIME
#  define UMUL_TIME 40
# endif
# ifndef UDIV_TIME
#  define UDIV_TIME 40
# endif
#endif /* 80x86 */

/***************************************
 *********** AMD64 / x86-64 ************
 ***************************************/
#if defined(__x86_64) && W_TYPE_SIZE == 64
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("addq %5,%1\n"                                               \
	   "adcq %3,%0"                                                 \
	   : "=r" ((sh)),                                               \
	     "=&r" ((sl))                                               \
	   : "0" ((UDItype)(ah)),                                       \
	     "g"  ((UDItype)(bh)),                                      \
	     "1" ((UDItype)(al)),                                       \
	     "g"  ((UDItype)(bl))                                       \
	   __CLOBBER_CC)
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("subq %5,%1\n"                                               \
	   "sbbq %3,%0"                                                 \
	   : "=r" ((sh)),                                               \
	     "=&r" ((sl))                                               \
	   : "0" ((UDItype)(ah)),                                       \
	     "g" ((UDItype)(bh)),                                       \
	     "1" ((UDItype)(al)),                                       \
	     "g" ((UDItype)(bl))                                        \
	   __CLOBBER_CC)
# define umul_ppmm(w1, w0, u, v) \
  __asm__ ("mulq %3"                                                    \
	   : "=a" ((w0)),                                               \
	     "=d" ((w1))                                                \
	   : "0" ((UDItype)(u)),                                        \
	     "rm" ((UDItype)(v))                                        \
	   __CLOBBER_CC)
# define udiv_qrnnd(q, r, n1, n0, d) \
  __asm__ ("divq %4"                                                    \
	   : "=a" ((q)),                                                \
	     "=d" ((r))                                                 \
	   : "0" ((UDItype)(n0)),                                       \
	     "1" ((UDItype)(n1)),                                       \
	     "rm" ((UDItype)(d))                                        \
	   __CLOBBER_CC)
# define count_leading_zeros(count, x) \
  do {                                                                  \
    UDItype __cbtmp;                                                    \
    __asm__ ("bsrq %1,%0"                                               \
             : "=r" (__cbtmp) : "rm" ((UDItype)(x))                     \
             __CLOBBER_CC);                                             \
    (count) = __cbtmp ^ 63;                                             \
  } while (0)
# define count_trailing_zeros(count, x) \
  do {                                                                  \
    UDItype __cbtmp;                                                    \
    __asm__ ("rep;bsfq %1,%0"                                           \
             : "=r" (__cbtmp) : "rm" ((UDItype)(x))                     \
             __CLOBBER_CC);                                             \
    (count) = __cbtmp;                                                  \
  } while (0)
# ifndef UMUL_TIME
#  define UMUL_TIME 40
# endif
# ifndef UDIV_TIME
#  define UDIV_TIME 40
# endif
#endif /* __x86_64 */


/***************************************
 **************  I860  *****************
 ***************************************/
#if defined (__i860__) && W_TYPE_SIZE == 32
# define rshift_rhlc(r,h,l,c) \
  __asm__ ("shr %3,r0,r0\n"  \
           "shrd %1,%2,%0"   \
	   "=r" (r) : "r" (h), "r" (l), "rn" (c))
#endif /* i860 */

/***************************************
 **************  I960  *****************
 ***************************************/
#if defined (__i960__) && W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("cmpo 1,0\n"      \
           "addc %5,%4,%1\n" \
           "addc %3,%2,%0"   \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "%dI" ((USItype)(ah)),                                     \
	     "dI" ((USItype)(bh)),                                      \
	     "%dI" ((USItype)(al)),                                     \
	     "dI" ((USItype)(bl)))
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("cmpo 0,0\n"      \
           "subc %5,%4,%1\n" \
           "subc %3,%2,%0"   \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "dI" ((USItype)(ah)),                                      \
	     "dI" ((USItype)(bh)),                                      \
	     "dI" ((USItype)(al)),                                      \
	     "dI" ((USItype)(bl)))
# define umul_ppmm(w1, w0, u, v) \
  ({union {UDItype __ll;						\
	   struct {USItype __l, __h;} __i;				\
	  } __xx;							\
  __asm__ ("emul        %2,%1,%0"                                       \
	   : "=d" (__xx.__ll)                                           \
	   : "%dI" ((USItype)(u)),                                      \
	     "dI" ((USItype)(v)));                                      \
  (w1) = __xx.__i.__h; (w0) = __xx.__i.__l;})
# define __umulsidi3(u, v) \
  ({UDItype __w;							\
    __asm__ ("emul      %2,%1,%0"                                       \
	     : "=d" (__w)                                               \
	     : "%dI" ((USItype)(u)),                                    \
	       "dI" ((USItype)(v)));                                    \
    __w; })
# define udiv_qrnnd(q, r, nh, nl, d) \
  do {									\
    union {UDItype __ll;						\
	   struct {USItype __l, __h;} __i;				\
	  } __nn;							\
    __nn.__i.__h = (nh); __nn.__i.__l = (nl);				\
    __asm__ ("ediv %d,%n,%0"                                            \
	   : "=d" (__rq.__ll)                                           \
	   : "dI" (__nn.__ll),                                          \
	     "dI" ((USItype)(d)));                                      \
    (r) = __rq.__i.__l; (q) = __rq.__i.__h;				\
  } while (0)
# define count_leading_zeros(count, x) \
  do {									\
    USItype __cbtmp;							\
    __asm__ ("scanbit %1,%0"                                            \
	     : "=r" (__cbtmp)                                           \
	     : "r" ((USItype)(x)));                                     \
    (count) = __cbtmp ^ 31;						\
  } while (0)
# define COUNT_LEADING_ZEROS_0 (-32) /* sic */
# if defined (__i960mx)  /* what is the proper symbol to test??? */
#  define rshift_rhlc(r,h,l,c) \
  do {									\
    union {UDItype __ll;						\
	   struct {USItype __l, __h;} __i;				\
	  } __nn;							\
    __nn.__i.__h = (h); __nn.__i.__l = (l);				\
    __asm__ ("shre %2,%1,%0"                                            \
	     : "=d" (r) : "dI" (__nn.__ll), "dI" (c));                  \
  }
# endif /* i960mx */
#endif /* i960 */


/***************************************
 **************  68000	****************
 ***************************************/
#if (defined (__mc68000__) || defined (__mc68020__)                     \
     || defined (__NeXT__) || defined(mc68020)) && W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("add%.l %5,%1\n"                                             \
	   "addx%.l %3,%0"                                              \
	   : "=d" ((USItype)(sh)),                                      \
	     "=&d" ((USItype)(sl))                                      \
	   : "%0" ((USItype)(ah)),                                      \
	     "d" ((USItype)(bh)),                                       \
	     "%1" ((USItype)(al)),                                      \
	     "g" ((USItype)(bl)))
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("sub%.l %5,%1\n"                                             \
	   "subx%.l %3,%0"                                              \
	   : "=d" ((USItype)(sh)),                                      \
	     "=&d" ((USItype)(sl))                                      \
	   : "0" ((USItype)(ah)),                                       \
	     "d" ((USItype)(bh)),                                       \
	     "1" ((USItype)(al)),                                       \
	     "g" ((USItype)(bl)))
# if (defined (__mc68020__) || defined (__NeXT__) || defined(mc68020))
#  define umul_ppmm(w1, w0, u, v) \
  __asm__ ("mulu%.l %3,%1:%0"                                           \
	   : "=d" ((USItype)(w0)),                                      \
	     "=d" ((USItype)(w1))                                       \
	   : "%0" ((USItype)(u)),                                       \
	     "dmi" ((USItype)(v)))
#  define UMUL_TIME 45
#  define udiv_qrnnd(q, r, n1, n0, d) \
  __asm__ ("divu%.l %4,%1:%0"                                           \
	   : "=d" ((USItype)(q)),                                       \
	     "=d" ((USItype)(r))                                        \
	   : "0" ((USItype)(n0)),                                       \
	     "1" ((USItype)(n1)),                                       \
	     "dmi" ((USItype)(d)))
#  define UDIV_TIME 90
#  define sdiv_qrnnd(q, r, n1, n0, d) \
  __asm__ ("divs%.l %4,%1:%0"                                           \
	   : "=d" ((USItype)(q)),                                       \
	     "=d" ((USItype)(r))                                        \
	   : "0" ((USItype)(n0)),                                       \
	     "1" ((USItype)(n1)),                                       \
	     "dmi" ((USItype)(d)))
#  define count_leading_zeros(count, x) \
  __asm__ ("bfffo %1{%b2:%b2},%0"                                       \
	   : "=d" ((USItype)(count))                                    \
	   : "od" ((USItype)(x)), "n" (0))
#  define COUNT_LEADING_ZEROS_0 32
# else /* not mc68020 */
#  define umul_ppmm(xh, xl, a, b) \
  do { USItype __umul_tmp1, __umul_tmp2;			  \
	__asm__ ("| Inlined umul_ppmm                         \n" \
 "        move%.l %5,%3                                       \n" \
 "        move%.l %2,%0                                       \n" \
 "        move%.w %3,%1                                       \n" \
 "        swap	%3                                            \n" \
 "        swap	%0                                            \n" \
 "        mulu	%2,%1                                         \n" \
 "        mulu	%3,%0                                         \n" \
 "        mulu	%2,%3                                         \n" \
 "        swap	%2                                            \n" \
 "        mulu	%5,%2                                         \n" \
 "        add%.l	%3,%2                                 \n" \
 "        jcc	1f                                            \n" \
 "        add%.l	%#0x10000,%0                          \n" \
 "1:	move%.l %2,%3                                         \n" \
 "        clr%.w	%2                                    \n" \
 "        swap	%2                                            \n" \
 "        swap	%3                                            \n" \
 "        clr%.w	%3                                    \n" \
 "        add%.l	%3,%1                                 \n" \
 "        addx%.l %2,%0                                       \n" \
 "        | End inlined umul_ppmm"                                \
	      : "=&d" ((USItype)(xh)), "=&d" ((USItype)(xl)),     \
		"=d" (__umul_tmp1), "=&d" (__umul_tmp2)           \
	      : "%2" ((USItype)(a)), "d" ((USItype)(b)));         \
  } while (0)
#  define UMUL_TIME 100
#  define UDIV_TIME 400
# endif /* not mc68020 */
#endif /* mc68000 */


/***************************************
 **************  88000	****************
 ***************************************/
#if defined (__m88000__) && W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("addu.co %1,%r4,%r5\n"                                       \
	   "addu.ci %0,%r2,%r3"                                         \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "%rJ" ((USItype)(ah)),                                     \
	     "rJ" ((USItype)(bh)),                                      \
	     "%rJ" ((USItype)(al)),                                     \
	     "rJ" ((USItype)(bl)))
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("subu.co %1,%r4,%r5\n"                                       \
	   "subu.ci %0,%r2,%r3"                                         \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "rJ" ((USItype)(ah)),                                      \
	     "rJ" ((USItype)(bh)),                                      \
	     "rJ" ((USItype)(al)),                                      \
	     "rJ" ((USItype)(bl)))
# define count_leading_zeros(count, x) \
  do {									\
    USItype __cbtmp;							\
    __asm__ ("ff1 %0,%1"                                                \
	     : "=r" (__cbtmp)                                           \
	     : "r" ((USItype)(x)));                                     \
    (count) = __cbtmp ^ 31;						\
  } while (0)
# define COUNT_LEADING_ZEROS_0 63 /* sic */
# if defined (__m88110__)
#  define umul_ppmm(wh, wl, u, v) \
  do {									\
    union {UDItype __ll;						\
	   struct {USItype __h, __l;} __i;				\
	  } __x;							\
    __asm__ ("mulu.d %0,%1,%2" : "=r" (__x.__ll) : "r" (u), "r" (v));   \
    (wh) = __x.__i.__h; 						\
    (wl) = __x.__i.__l; 						\
  } while (0)
#  define udiv_qrnnd(q, r, n1, n0, d) \
  ({union {UDItype __ll;						\
	   struct {USItype __h, __l;} __i;				\
	  } __x, __q;							\
  __x.__i.__h = (n1); __x.__i.__l = (n0);				\
  __asm__ ("divu.d %0,%1,%2"                                            \
	   : "=r" (__q.__ll) : "r" (__x.__ll), "r" (d));                \
  (r) = (n0) - __q.__l * (d); (q) = __q.__l; })
#  define UMUL_TIME 5
#  define UDIV_TIME 25
# else
#  define UMUL_TIME 17
#  define UDIV_TIME 150
# endif /* __m88110__ */
#endif /* __m88000__ */

/***************************************
 **************  MIPS  *****************
 ***************************************/
#if defined (__mips__) && W_TYPE_SIZE == 32
# if defined (__clang__) || (__GNUC__ >= 5) || (__GNUC__ == 4 && \
                                               __GNUC_MINOR__ >= 4)
#  define umul_ppmm(w1, w0, u, v) \
  do {                                                                  \
    UDItype __r;                                                        \
    __r = (UDItype)(u) * (v);                                           \
    (w1) = __r >> 32;                                                   \
    (w0) = (USItype) __r;                                               \
  } while (0)
# elif __GNUC__ > 2 || __GNUC_MINOR__ >= 7
#  define umul_ppmm(w1, w0, u, v) \
  __asm__ ("multu %2,%3"                                                \
	   : "=l" ((USItype)(w0)),                                      \
	     "=h" ((USItype)(w1))                                       \
	   : "d" ((USItype)(u)),                                        \
	     "d" ((USItype)(v)))
# else
#  define umul_ppmm(w1, w0, u, v) \
  __asm__ ("multu %2,%3 \n" \
	   "mflo %0 \n"     \
	   "mfhi %1"                                                    \
	   : "=d" ((USItype)(w0)),                                      \
	     "=d" ((USItype)(w1))                                       \
	   : "d" ((USItype)(u)),                                        \
	     "d" ((USItype)(v)))
# endif
# define UMUL_TIME 10
# define UDIV_TIME 100
#endif /* __mips__ */

/***************************************
 **************  MIPS/64  **************
 ***************************************/
#if (defined (__mips) && __mips >= 3) && W_TYPE_SIZE == 64
# if defined (__clang__) || (__GNUC__ >= 5) || (__GNUC__ == 4 && \
                                               __GNUC_MINOR__ >= 4)
typedef unsigned int UTItype __attribute__ ((mode (TI)));
#  define umul_ppmm(w1, w0, u, v) \
  do {                                                                  \
    UTItype __r;                                                        \
    __r = (UTItype)(u) * (v);                                           \
    (w1) = __r >> 64;                                                   \
    (w0) = (UDItype) __r;                                               \
  } while (0)
# elif __GNUC__ > 2 || __GNUC_MINOR__ >= 7
#  define umul_ppmm(w1, w0, u, v) \
  __asm__ ("dmultu %2,%3"                                               \
	   : "=l" ((UDItype)(w0)),                                      \
	     "=h" ((UDItype)(w1))                                       \
	   : "d" ((UDItype)(u)),                                        \
	     "d" ((UDItype)(v)))
# else
#  define umul_ppmm(w1, w0, u, v) \
  __asm__ ("dmultu %2,%3 \n"    \
	   "mflo %0 \n"         \
	   "mfhi %1"                                                    \
	   : "=d" ((UDItype)(w0)),                                      \
	     "=d" ((UDItype)(w1))                                       \
	   : "d" ((UDItype)(u)),                                        \
	     "d" ((UDItype)(v)))
# endif
# define UMUL_TIME 20
# define UDIV_TIME 140
#endif /* __mips__ */


/***************************************
 **************  32000	****************
 ***************************************/
#if defined (__ns32000__) && W_TYPE_SIZE == 32
# define umul_ppmm(w1, w0, u, v) \
  ({union {UDItype __ll;						\
	   struct {USItype __l, __h;} __i;				\
	  } __xx;							\
  __asm__ ("meid %2,%0"                                                 \
	   : "=g" (__xx.__ll)                                           \
	   : "%0" ((USItype)(u)),                                       \
	     "g" ((USItype)(v)));                                       \
  (w1) = __xx.__i.__h; (w0) = __xx.__i.__l;})
# define __umulsidi3(u, v) \
  ({UDItype __w;							\
    __asm__ ("meid %2,%0"                                               \
	     : "=g" (__w)                                               \
	     : "%0" ((USItype)(u)),                                     \
	       "g" ((USItype)(v)));                                     \
    __w; })
# define udiv_qrnnd(q, r, n1, n0, d) \
  ({union {UDItype __ll;						\
	   struct {USItype __l, __h;} __i;				\
	  } __xx;							\
  __xx.__i.__h = (n1); __xx.__i.__l = (n0);				\
  __asm__ ("deid %2,%0"                                                 \
	   : "=g" (__xx.__ll)                                           \
	   : "0" (__xx.__ll),                                           \
	     "g" ((USItype)(d)));                                       \
  (r) = __xx.__i.__l; (q) = __xx.__i.__h; })
# define count_trailing_zeros(count,x) \
  do {
    __asm__ ("ffsd      %2,%0"                                          \
	     : "=r" ((USItype) (count))                                 \
	     : "0" ((USItype) 0),                                       \
	       "r" ((USItype) (x)));                                    \
  } while (0)
#endif /* __ns32000__ */


/***************************************
 **************  PPC  ******************
 ***************************************/
/* Powerpc 32 bit support taken from GCC longlong.h. */
#if (defined (_ARCH_PPC) || defined (__powerpc__)) && W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  do {									\
    if (__builtin_constant_p (bh) && (bh) == 0)				\
      __asm__ ("add%I4c %1,%3,%4\n\taddze %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (ah), "%r" (al), "rI" (bl)	\
              __CLOBBER_CC);						\
    else if (__builtin_constant_p (bh) && (bh) == ~(USItype) 0)		\
      __asm__ ("add%I4c %1,%3,%4\n\taddme %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (ah), "%r" (al), "rI" (bl)	\
              __CLOBBER_CC);						\
    else								\
      __asm__ ("add%I5c %1,%4,%5\n\tadde %0,%2,%3"			\
              : "=r" (sh), "=&r" (sl)					\
              : "%r" (ah), "r" (bh), "%r" (al), "rI" (bl)		\
              __CLOBBER_CC);						\
  } while (0)
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  do {									\
    if (__builtin_constant_p (ah) && (ah) == 0)				\
      __asm__ ("subf%I3c %1,%4,%3\n\tsubfze %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (bh), "rI" (al), "r" (bl)	\
              __CLOBBER_CC);						\
    else if (__builtin_constant_p (ah) && (ah) == ~(USItype) 0)		\
      __asm__ ("subf%I3c %1,%4,%3\n\tsubfme %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (bh), "rI" (al), "r" (bl)	\
              __CLOBBER_CC);						\
    else if (__builtin_constant_p (bh) && (bh) == 0)			\
      __asm__ ("subf%I3c %1,%4,%3\n\taddme %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (ah), "rI" (al), "r" (bl)	\
              __CLOBBER_CC);						\
    else if (__builtin_constant_p (bh) && (bh) == ~(USItype) 0)		\
      __asm__ ("subf%I3c %1,%4,%3\n\taddze %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (ah), "rI" (al), "r" (bl)	\
              __CLOBBER_CC);						\
    else								\
      __asm__ ("subf%I4c %1,%5,%4\n\tsubfe %0,%3,%2"			\
              : "=r" (sh), "=&r" (sl)					\
              : "r" (ah), "r" (bh), "rI" (al), "r" (bl)			\
              __CLOBBER_CC);						\
  } while (0)
# define count_leading_zeros(count, x) \
  __asm__ ("cntlzw %0,%1" : "=r" (count) : "r" (x))
# define COUNT_LEADING_ZEROS_0 32
# define umul_ppmm(ph, pl, m0, m1) \
  do {									\
    USItype __m0 = (m0), __m1 = (m1);					\
    __asm__ ("mulhwu %0,%1,%2" : "=r" (ph) : "%r" (m0), "r" (m1));	\
    (pl) = __m0 * __m1;							\
  } while (0)
# define UMUL_TIME 15
# define smul_ppmm(ph, pl, m0, m1) \
  do {									\
    SItype __m0 = (m0), __m1 = (m1);					\
    __asm__ ("mulhw %0,%1,%2" : "=r" (ph) : "%r" (m0), "r" (m1));	\
    (pl) = __m0 * __m1;							\
  } while (0)
# define SMUL_TIME 14
# define UDIV_TIME 120
#endif /* 32-bit POWER architecture variants.  */

/* Powerpc 64 bit support taken from GCC longlong.h. */
/* We should test _IBMR2 here when we add assembly support for the system
   vendor compilers.  */
#if (defined (_ARCH_PPC64) || defined (__powerpc64__)) && W_TYPE_SIZE == 64
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  do {									\
    if (__builtin_constant_p (bh) && (bh) == 0)				\
      __asm__ ("add%I4c %1,%3,%4\n\taddze %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (ah), "%r" (al), "rI" (bl)	\
              __CLOBBER_CC);						\
    else if (__builtin_constant_p (bh) && (bh) == ~(UDItype) 0)		\
      __asm__ ("add%I4c %1,%3,%4\n\taddme %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (ah), "%r" (al), "rI" (bl)	\
              __CLOBBER_CC);						\
    else								\
      __asm__ ("add%I5c %1,%4,%5\n\tadde %0,%2,%3"			\
              : "=r" (sh), "=&r" (sl)					\
              : "%r" (ah), "r" (bh), "%r" (al), "rI" (bl)		\
              __CLOBBER_CC);						\
  } while (0)
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  do {									\
    if (__builtin_constant_p (ah) && (ah) == 0)				\
      __asm__ ("subf%I3c %1,%4,%3\n\tsubfze %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (bh), "rI" (al), "r" (bl)	\
              __CLOBBER_CC);						\
    else if (__builtin_constant_p (ah) && (ah) == ~(UDItype) 0)		\
      __asm__ ("subf%I3c %1,%4,%3\n\tsubfme %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (bh), "rI" (al), "r" (bl)	\
              __CLOBBER_CC);						\
    else if (__builtin_constant_p (bh) && (bh) == 0)			\
      __asm__ ("subf%I3c %1,%4,%3\n\taddme %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (ah), "rI" (al), "r" (bl)	\
              __CLOBBER_CC);						\
    else if (__builtin_constant_p (bh) && (bh) == ~(UDItype) 0)		\
      __asm__ ("subf%I3c %1,%4,%3\n\taddze %0,%2"			\
              : "=r" (sh), "=&r" (sl) : "r" (ah), "rI" (al), "r" (bl)	\
              __CLOBBER_CC);						\
    else								\
      __asm__ ("subf%I4c %1,%5,%4\n\tsubfe %0,%3,%2"			\
              : "=r" (sh), "=&r" (sl)					\
              : "r" (ah), "r" (bh), "rI" (al), "r" (bl)			\
              __CLOBBER_CC);						\
  } while (0)
# define count_leading_zeros(count, x) \
  __asm__ ("cntlzd %0,%1" : "=r" (count) : "r" (x))
# define COUNT_LEADING_ZEROS_0 64
# define umul_ppmm(ph, pl, m0, m1) \
  do {									\
    UDItype __m0 = (m0), __m1 = (m1);					\
    __asm__ ("mulhdu %0,%1,%2" : "=r" (ph) : "%r" (m0), "r" (m1));	\
    (pl) = __m0 * __m1;							\
  } while (0)
# define UMUL_TIME 15
# define smul_ppmm(ph, pl, m0, m1) \
  do {									\
    DItype __m0 = (m0), __m1 = (m1);					\
    __asm__ ("mulhd %0,%1,%2" : "=r" (ph) : "%r" (m0), "r" (m1));	\
    (pl) = __m0 * __m1;							\
  } while (0)
# define SMUL_TIME 14  /* ??? */
# define UDIV_TIME 120 /* ??? */
#endif /* 64-bit PowerPC.  */

/***************************************
 **************  PYR  ******************
 ***************************************/
#if defined (__pyr__) && W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("addw        %5,%1 \n" \
	   "addwc	%3,%0"                                          \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "%0" ((USItype)(ah)),                                      \
	     "g" ((USItype)(bh)),                                       \
	     "%1" ((USItype)(al)),                                      \
	     "g" ((USItype)(bl)))
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("subw        %5,%1 \n" \
	   "subwb	%3,%0"                                          \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "0" ((USItype)(ah)),                                       \
	     "g" ((USItype)(bh)),                                       \
	     "1" ((USItype)(al)),                                       \
	     "g" ((USItype)(bl)))
/* This insn works on Pyramids with AP, XP, or MI CPUs, but not with SP.  */
# define umul_ppmm(w1, w0, u, v) \
  ({union {UDItype __ll;						\
	   struct {USItype __h, __l;} __i;				\
	  } __xx;							\
  __asm__ ("movw %1,%R0 \n" \
	   "uemul %2,%0"                                                \
	   : "=&r" (__xx.__ll)                                          \
	   : "g" ((USItype) (u)),                                       \
	     "g" ((USItype)(v)));                                       \
  (w1) = __xx.__i.__h; (w0) = __xx.__i.__l;})
#endif /* __pyr__ */


/***************************************
 **************  RT/ROMP  **************
 ***************************************/
#if defined (__ibm032__) /* RT/ROMP */	&& W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("a %1,%5 \n" \
	   "ae %0,%3"                                                   \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "%0" ((USItype)(ah)),                                      \
	     "r" ((USItype)(bh)),                                       \
	     "%1" ((USItype)(al)),                                      \
	     "r" ((USItype)(bl)))
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("s %1,%5\n" \
	   "se %0,%3"                                                   \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "0" ((USItype)(ah)),                                       \
	     "r" ((USItype)(bh)),                                       \
	     "1" ((USItype)(al)),                                       \
	     "r" ((USItype)(bl)))
# define umul_ppmm(ph, pl, m0, m1) \
  do {									\
    USItype __m0 = (m0), __m1 = (m1);					\
    __asm__ (								\
       "s       r2,r2    \n" \
       "mts	r10,%2   \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "m	r2,%3    \n" \
       "cas	%0,r2,r0 \n" \
       "mfs	r10,%1"                                                 \
	     : "=r" ((USItype)(ph)),                                    \
	       "=r" ((USItype)(pl))                                     \
	     : "%r" (__m0),                                             \
		"r" (__m1)                                              \
	     : "r2");                                                   \
    (ph) += ((((SItype) __m0 >> 31) & __m1)				\
	     + (((SItype) __m1 >> 31) & __m0)); 			\
  } while (0)
# define UMUL_TIME 20
# define UDIV_TIME 200
# define count_leading_zeros(count, x) \
  do {									\
    if ((x) >= 0x10000) 						\
      __asm__ ("clz     %0,%1"                                          \
	       : "=r" ((USItype)(count))                                \
	       : "r" ((USItype)(x) >> 16));                             \
    else								\
      { 								\
	__asm__ ("clz   %0,%1"                                          \
		 : "=r" ((USItype)(count))                              \
		 : "r" ((USItype)(x)));                                 \
	(count) += 16;							\
      } 								\
  } while (0)
#endif /* RT/ROMP */


/***************************************
 **************  SH2  ******************
 ***************************************/
#if (defined (__sh2__) || defined(__sh3__) || defined(__SH4__) ) \
    && W_TYPE_SIZE == 32
# define umul_ppmm(w1, w0, u, v) \
  __asm__ (								\
        "dmulu.l %2,%3\n"  \
	"sts	macl,%1\n" \
	"sts	mach,%0"                                                \
	   : "=r" ((USItype)(w1)),                                      \
	     "=r" ((USItype)(w0))                                       \
	   : "r" ((USItype)(u)),                                        \
	     "r" ((USItype)(v))                                         \
	   : "macl", "mach")
# define UMUL_TIME 5
#endif

/***************************************
 **************  SPARC	****************
 ***************************************/
#if defined (__sparc__) && W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("addcc %r4,%5,%1\n" \
	   "addx %r2,%3,%0"                                             \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "%rJ" ((USItype)(ah)),                                     \
	     "rI" ((USItype)(bh)),                                      \
	     "%rJ" ((USItype)(al)),                                     \
	     "rI" ((USItype)(bl))                                       \
	   __CLOBBER_CC)
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("subcc %r4,%5,%1\n" \
	   "subx %r2,%3,%0"                                             \
	   : "=r" ((USItype)(sh)),                                      \
	     "=&r" ((USItype)(sl))                                      \
	   : "rJ" ((USItype)(ah)),                                      \
	     "rI" ((USItype)(bh)),                                      \
	     "rJ" ((USItype)(al)),                                      \
	     "rI" ((USItype)(bl))                                       \
	   __CLOBBER_CC)
# if defined (__sparc_v8__) || defined(__sparcv8)
/* Don't match immediate range because, 1) it is not often useful,
   2) the 'I' flag thinks of the range as a 13 bit signed interval,
   while we want to match a 13 bit interval, sign extended to 32 bits,
   but INTERPRETED AS UNSIGNED.  */
#  define umul_ppmm(w1, w0, u, v) \
  __asm__ ("umul %2,%3,%1;rd %%y,%0"                                    \
	   : "=r" ((USItype)(w1)),                                      \
	     "=r" ((USItype)(w0))                                       \
	   : "r" ((USItype)(u)),                                        \
	     "r" ((USItype)(v)))
#  define UMUL_TIME 5
#  ifndef SUPERSPARC	/* SuperSPARC's udiv only handles 53 bit dividends */
#   define udiv_qrnnd(q, r, n1, n0, d) \
  do {									\
    USItype __q;							\
    __asm__ ("mov %1,%%y;nop;nop;nop;udiv %2,%3,%0"                     \
	     : "=r" ((USItype)(__q))                                    \
	     : "r" ((USItype)(n1)),                                     \
	       "r" ((USItype)(n0)),                                     \
	       "r" ((USItype)(d)));                                     \
    (r) = (n0) - __q * (d);						\
    (q) = __q;								\
  } while (0)
#   define UDIV_TIME 25
#  endif /*!SUPERSPARC */
# else /* ! __sparc_v8__ */
#  if defined (__sparclite__)
/* This has hardware multiply but not divide.  It also has two additional
   instructions scan (ffs from high bit) and divscc.  */
#   define umul_ppmm(w1, w0, u, v) \
  __asm__ ("umul %2,%3,%1;rd %%y,%0"                                    \
	   : "=r" ((USItype)(w1)),                                      \
	     "=r" ((USItype)(w0))                                       \
	   : "r" ((USItype)(u)),                                        \
	     "r" ((USItype)(v)))
#   define UMUL_TIME 5
#   define udiv_qrnnd(q, r, n1, n0, d) \
  __asm__ ("! Inlined udiv_qrnnd                                     \n" \
 "        wr	%%g0,%2,%%y	! Not a delayed write for sparclite  \n" \
 "        tst	%%g0                                                 \n" \
 "        divscc	%3,%4,%%g1                                   \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%%g1                                 \n" \
 "        divscc	%%g1,%4,%0                                   \n" \
 "        rd	%%y,%1                                               \n" \
 "        bl,a 1f                                                    \n" \
 "        add	%1,%4,%1                                             \n" \
 "1:	! End of inline udiv_qrnnd"                                     \
	   : "=r" ((USItype)(q)),                                       \
	     "=r" ((USItype)(r))                                        \
	   : "r" ((USItype)(n1)),                                       \
	     "r" ((USItype)(n0)),                                       \
	     "rI" ((USItype)(d))                                        \
	   : "%g1" __AND_CLOBBER_CC)
#   define UDIV_TIME 37
#   define count_leading_zeros(count, x) \
  __asm__ ("scan %1,0,%0"                                               \
	   : "=r" ((USItype)(x))                                        \
	   : "r" ((USItype)(count)))
/* Early sparclites return 63 for an argument of 0, but they warn that future
   implementations might change this.  Therefore, leave COUNT_LEADING_ZEROS_0
   undefined.  */
#  endif /* !__sparclite__ */
# endif /* !__sparc_v8__ */
/* Default to sparc v7 versions of umul_ppmm and udiv_qrnnd.  */
# ifndef umul_ppmm
#  define umul_ppmm(w1, w0, u, v) \
  __asm__ ("! Inlined umul_ppmm                                        \n" \
 "        wr	%%g0,%2,%%y	! SPARC has 0-3 delay insn after a wr  \n" \
 "        sra	%3,31,%%g2	! Don't move this insn                 \n" \
 "        and	%2,%%g2,%%g2	! Don't move this insn                 \n" \
 "        andcc	%%g0,0,%%g1	! Don't move this insn                 \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,%3,%%g1                                   \n" \
 "        mulscc	%%g1,0,%%g1                                    \n" \
 "        add	%%g1,%%g2,%0                                           \n" \
 "        rd	%%y,%1"                                                 \
	   : "=r" ((USItype)(w1)),                                      \
	     "=r" ((USItype)(w0))                                       \
	   : "%rI" ((USItype)(u)),                                      \
	     "r" ((USItype)(v))                                         \
	   : "%g1", "%g2" __AND_CLOBBER_CC)
#  define UMUL_TIME 39		/* 39 instructions */
# endif /* umul_ppmm */
# ifndef udiv_qrnnd
#  ifndef LONGLONG_STANDALONE
#   define udiv_qrnnd(q, r, n1, n0, d) \
  do { USItype __r;							\
    (q) = __udiv_qrnnd (&__r, (n1), (n0), (d)); 			\
    (r) = __r;								\
  } while (0)
extern USItype __udiv_qrnnd (USItype *, USItype, USItype, USItype);
#   define UDIV_TIME 140
#  endif /* LONGLONG_STANDALONE */
# endif /* udiv_qrnnd */
#endif /* __sparc__ */


/***************************************
 **************  VAX  ******************
 ***************************************/
#if defined (__vax__) && W_TYPE_SIZE == 32
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("addl2 %5,%1\n" \
	   "adwc %3,%0"                                                 \
	   : "=g" ((USItype)(sh)),                                      \
	     "=&g" ((USItype)(sl))                                      \
	   : "%0" ((USItype)(ah)),                                      \
	     "g" ((USItype)(bh)),                                       \
	     "%1" ((USItype)(al)),                                      \
	     "g" ((USItype)(bl)))
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("subl2 %5,%1\n" \
	   "sbwc %3,%0"                                                 \
	   : "=g" ((USItype)(sh)),                                      \
	     "=&g" ((USItype)(sl))                                      \
	   : "0" ((USItype)(ah)),                                       \
	     "g" ((USItype)(bh)),                                       \
	     "1" ((USItype)(al)),                                       \
	     "g" ((USItype)(bl)))
# define umul_ppmm(xh, xl, m0, m1) \
  do {									\
    union {UDItype __ll;						\
	   struct {USItype __l, __h;} __i;				\
	  } __xx;							\
    USItype __m0 = (m0), __m1 = (m1);					\
    __asm__ ("emul %1,%2,$0,%0"                                         \
	     : "=g" (__xx.__ll)                                         \
	     : "g" (__m0),                                              \
	       "g" (__m1));                                             \
    (xh) = __xx.__i.__h; (xl) = __xx.__i.__l;				\
    (xh) += ((((SItype) __m0 >> 31) & __m1)				\
	     + (((SItype) __m1 >> 31) & __m0)); 			\
  } while (0)
# define sdiv_qrnnd(q, r, n1, n0, d) \
  do {									\
    union {DItype __ll; 						\
	   struct {SItype __l, __h;} __i;				\
	  } __xx;							\
    __xx.__i.__h = (n1); __xx.__i.__l = (n0);				\
    __asm__ ("ediv %3,%2,%0,%1"                                         \
	     : "=g" (q), "=g" (r)                                       \
	     : "g" (__xx.__ll), "g" (d));                               \
  } while (0)
#endif /* __vax__ */


/***************************************
 **************  Z8000	****************
 ***************************************/
#if defined (__z8000__) && W_TYPE_SIZE == 16
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("add %H1,%H5\n\tadc  %H0,%H3"                                \
	   : "=r" ((unsigned int)(sh)),                                 \
	     "=&r" ((unsigned int)(sl))                                 \
	   : "%0" ((unsigned int)(ah)),                                 \
	     "r" ((unsigned int)(bh)),                                  \
	     "%1" ((unsigned int)(al)),                                 \
	     "rQR" ((unsigned int)(bl)))
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("sub %H1,%H5\n\tsbc  %H0,%H3"                                \
	   : "=r" ((unsigned int)(sh)),                                 \
	     "=&r" ((unsigned int)(sl))                                 \
	   : "0" ((unsigned int)(ah)),                                  \
	     "r" ((unsigned int)(bh)),                                  \
	     "1" ((unsigned int)(al)),                                  \
	     "rQR" ((unsigned int)(bl)))
# define umul_ppmm(xh, xl, m0, m1) \
  do {									\
    union {long int __ll;						\
	   struct {unsigned int __h, __l;} __i; 			\
	  } __xx;							\
    unsigned int __m0 = (m0), __m1 = (m1);				\
    __asm__ ("mult      %S0,%H3"                                        \
	     : "=r" (__xx.__i.__h),                                     \
	       "=r" (__xx.__i.__l)                                      \
	     : "%1" (__m0),                                             \
	       "rQR" (__m1));                                           \
    (xh) = __xx.__i.__h; (xl) = __xx.__i.__l;				\
    (xh) += ((((signed int) __m0 >> 15) & __m1) 			\
	     + (((signed int) __m1 >> 15) & __m0));			\
  } while (0)
#endif /* __z8000__ */


/***************************************
 *********** s390x/zSeries  ************
 ***************************************/
#if defined (__s390x__) && W_TYPE_SIZE == 64 && __GNUC__ >= 4
# define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  __asm__ ("algr %1,%5\n"                                               \
	   "alcgr %0,%3\n"                                              \
	   : "=r" ((sh)),                                               \
	     "=&r" ((sl))                                               \
	   : "0" ((UDItype)(ah)),                                       \
	     "r"  ((UDItype)(bh)),                                      \
	     "1" ((UDItype)(al)),                                       \
	     "r"  ((UDItype)(bl))                                       \
	   __CLOBBER_CC)
# define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  __asm__ ("slgr %1,%5\n"                                               \
	   "slbgr %0,%3\n"                                              \
	   : "=r" ((sh)),                                               \
	     "=&r" ((sl))                                               \
	   : "0" ((UDItype)(ah)),                                       \
	     "r" ((UDItype)(bh)),                                       \
	     "1" ((UDItype)(al)),                                       \
	     "r" ((UDItype)(bl))                                        \
	   __CLOBBER_CC)
typedef unsigned int UTItype __attribute__ ((mode (TI)));
#  define umul_ppmm(w1, w0, u, v) \
  do {                                                                  \
    UTItype ___r;                                                       \
    __asm__ ("mlgr %0,%2"                                               \
	     : "=r" (___r)                                              \
	     : "0" ((UDItype)(u)),                                      \
	       "r" ((UDItype)(v)));                                     \
    (w1) = ___r >> 64;                                                  \
    (w0) = (UDItype) ___r;                                              \
  } while (0)
# define udiv_qrnnd(q, r, n1, n0, d) \
  do {                                                                  \
    UTItype ___r = ((UTItype)n1 << 64) | n0;                            \
    __asm__ ("dlgr %0,%2"                                               \
	     : "=r" (___r)                                              \
	     : "0" (___r),                                              \
	       "r" ((UDItype)(d)));                                     \
    (r) = ___r >> 64;                                                   \
    (q) = (UDItype) ___r;                                               \
  } while (0)
#endif /* __s390x__ */


/***************************************
 *****  End CPU Specific Versions  *****
 ***************************************/

#endif /* __GNUC__ */
#endif /* !__riscos__ */


/***************************************
 ***********  Generic Versions	********
 ***************************************/
#if !defined (umul_ppmm) && defined (__umulsidi3)
#  define umul_ppmm(ph, pl, m0, m1) \
  {									\
    UDWtype __ll = __umulsidi3 (m0, m1);				\
    ph = (UWtype) (__ll >> W_TYPE_SIZE);				\
    pl = (UWtype) __ll; 						\
  }
#endif

#if !defined (__umulsidi3)
#  define __umulsidi3(u, v) \
  ({UWtype __hi, __lo;							\
    umul_ppmm (__hi, __lo, u, v);					\
    ((UDWtype) __hi << W_TYPE_SIZE) | __lo; })
#endif

/* If this machine has no inline assembler, use C macros.  */

#if !defined (add_ssaaaa) && defined (UDWtype)
/* Use double word type when available. */
#  define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  do {									\
    UDWtype __audw = (ah);						\
    UWtype __auwh, __auwl;						\
    UDWtype __budw = (bh);						\
    __audw <<= W_TYPE_SIZE;						\
    __audw |= (al);							\
    __budw <<= W_TYPE_SIZE;						\
    __budw |= (bl);							\
    __audw += __budw;							\
    __auwh = (UWtype)(__audw >> W_TYPE_SIZE);				\
    __auwl = (UWtype)(__audw);						\
    CT_DEOPTIMIZE_VAR(__auwh);						\
    (sh) = __auwh;							\
    (sl) = __auwl;							\
  } while (0)
#elif !defined (add_ssaaaa)
#  define add_ssaaaa(sh, sl, ah, al, bh, bl) \
  do {									\
    UWtype __xl, __xh; 							\
    __xl = (al) + (bl);							\
    __xh = __xl < (al);							\
    __xh = (ah) + (bh) + __xh;						\
    CT_DEOPTIMIZE_VAR(__xh);						\
    (sh) = __xh;							\
    (sl) = __xl; 							\
  } while (0)
#endif

#if !defined (sub_ddmmss) && defined (UDWtype)
/* Use double word type when available. */
#  define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  do {									\
    UDWtype __audw = (ah);						\
    UWtype __auwh, __auwl;						\
    UDWtype __budw = (bh);						\
    __audw <<= W_TYPE_SIZE;						\
    __audw |= (al);							\
    __budw <<= W_TYPE_SIZE;						\
    __budw |= (bl);							\
    __audw -= __budw;							\
    __auwh = (UWtype)(__audw >> W_TYPE_SIZE);				\
    __auwl = (UWtype)(__audw);						\
    CT_DEOPTIMIZE_VAR(__auwh);						\
    (sh) = __auwh;							\
    (sl) = __auwl;							\
  } while (0)
#elif !defined (sub_ddmmss)
#  define sub_ddmmss(sh, sl, ah, al, bh, bl) \
  do {									\
    UWtype __xl, __xh; 							\
    __xl = (al) - (bl);							\
    __xh = (__xl > (al));						\
    __xh = (ah) - (bh) - __xh;						\
    CT_DEOPTIMIZE_VAR(__xh);						\
    (sh) = __xh;							\
    (sl) = __xl; 							\
  } while (0)
#endif

#if !defined (umul_ppmm) && defined (UDWtype)
#  define umul_ppmm(w1, w0, u, v) 					\
  do {									\
    UDWtype __x = (u);							\
    __x *= (v);								\
    (w1) = (UWtype)(__x >> W_TYPE_SIZE);				\
    (w0) = (UWtype)(__x);						\
  } while (0)
#elif !defined (umul_ppmm)
#  define umul_ppmm(w1, w0, u, v) 					\
  do {									\
    UWtype __x0, __x1, __x2, __x3;					\
    UHWtype __ul, __vl, __uh, __vh;					\
    UWtype __u = (u), __v = (v);					\
									\
    __ul = __ll_lowpart (__u);						\
    __uh = __ll_highpart (__u); 					\
    __vl = __ll_lowpart (__v);						\
    __vh = __ll_highpart (__v); 					\
									\
    __x0 = (UWtype) __ul * __vl;					\
    __x1 = (UWtype) __ul * __vh;					\
    __x2 = (UWtype) __uh * __vl;					\
    __x3 = (UWtype) __uh * __vh;					\
									\
    __x1 += __ll_highpart (__x0);/* this can't give carry */            \
    /* but this indeed can, and if so, add it in the proper pos: */	\
    add_ssaaaa(__x2, __x1, 0, __x1, 0, __x2);				\
    __x3 += __x2 << (W_TYPE_SIZE / 2);					\
    (w1) = __x3 + __ll_highpart (__x1); 				\
    (w0) = (__ll_lowpart (__x1) << W_TYPE_SIZE/2) + __ll_lowpart (__x0);\
  } while (0)
#endif

#if !defined (smul_ppmm)
#  define smul_ppmm(w1, w0, u, v) 					\
  do {									\
    UWtype __w1;							\
    UWtype __m0 = (u), __m1 = (v);					\
    umul_ppmm (__w1, w0, __m0, __m1);					\
    (w1) = __w1 - (-(__m0 >> (W_TYPE_SIZE - 1)) & __m1) 		\
		- (-(__m1 >> (W_TYPE_SIZE - 1)) & __m0);		\
  } while (0)
#endif

/* Define this unconditionally, so it can be used for debugging.  */
#define __udiv_qrnnd_c(q, r, n1, n0, d) \
  do {									\
    UWtype __d1, __d0, __q1, __q0, __r1, __r0, __m;			\
    __d1 = __ll_highpart (d);						\
    __d0 = __ll_lowpart (d);						\
									\
    __r1 = (n1) % __d1; 						\
    __q1 = (n1) / __d1; 						\
    __m = (UWtype) __q1 * __d0; 					\
    __r1 = __r1 * __ll_B | __ll_highpart (n0);				\
    if (__r1 < __m)							\
      { 								\
	__q1--, __r1 += (d);						\
	if (__r1 >= (d)) /* i.e. we didn't get carry when adding to __r1 */\
	  if (__r1 < __m)						\
	    __q1--, __r1 += (d);					\
      } 								\
    __r1 -= __m;							\
									\
    __r0 = __r1 % __d1; 						\
    __q0 = __r1 / __d1; 						\
    __m = (UWtype) __q0 * __d0; 					\
    __r0 = __r0 * __ll_B | __ll_lowpart (n0);				\
    if (__r0 < __m)							\
      { 								\
	__q0--, __r0 += (d);						\
	if (__r0 >= (d))						\
	  if (__r0 < __m)						\
	    __q0--, __r0 += (d);					\
      } 								\
    __r0 -= __m;							\
									\
    (q) = (UWtype) __q1 * __ll_B | __q0;				\
    (r) = __r0; 							\
  } while (0)

/* Use double word type if available. */
#if !defined (udiv_qrnnd) && defined (UDWtype)
#  define udiv_qrnnd(q, r, nh, nl, d) \
  do {									\
    UWtype __d = (d);							\
    UDWtype __nudw = (nh);						\
    __nudw <<= W_TYPE_SIZE;						\
    __nudw |= (nl);							\
    (q) = (UWtype)(__nudw / __d);					\
    (r) = (UWtype)(__nudw % __d);					\
  } while (0)
#endif

/* If the processor has no udiv_qrnnd but sdiv_qrnnd, go through
   __udiv_w_sdiv (defined in libgcc or elsewhere).  */
#if !defined (udiv_qrnnd) && defined (sdiv_qrnnd)
#  define udiv_qrnnd(q, r, nh, nl, d) \
  do {									\
    UWtype __r; 							\
    (q) = __MPN(udiv_w_sdiv) (&__r, nh, nl, d); 			\
    (r) = __r;								\
  } while (0)
#endif

/* If udiv_qrnnd was not defined for this processor, use __udiv_qrnnd_c.  */
#if !defined (udiv_qrnnd)
#  define UDIV_NEEDS_NORMALIZATION 1
#  define udiv_qrnnd __udiv_qrnnd_c
#endif

#if !defined (count_leading_zeros)
#  if defined (HAVE_BUILTIN_CLZL) && SIZEOF_UNSIGNED_LONG * 8 == W_TYPE_SIZE
#    define count_leading_zeros(count, x) (count = __builtin_clzl(x))
#    undef COUNT_LEADING_ZEROS_0 /* Input X=0 is undefined for the builtin. */
#  elif defined (HAVE_BUILTIN_CLZ) && SIZEOF_UNSIGNED_INT * 8 == W_TYPE_SIZE
#    define count_leading_zeros(count, x) (count = __builtin_clz(x))
#    undef COUNT_LEADING_ZEROS_0 /* Input X=0 is undefined for the builtin. */
#  endif
#endif

#if !defined (count_trailing_zeros)
#  if defined (HAVE_BUILTIN_CTZL) && SIZEOF_UNSIGNED_LONG * 8 == W_TYPE_SIZE
#    define count_trailing_zeros(count, x) (count = __builtin_ctzl(x))
#    undef COUNT_LEADING_ZEROS_0 /* Input X=0 is undefined for the builtin. */
#  elif defined (HAVE_BUILTIN_CTZ) && SIZEOF_UNSIGNED_INT * 8 == W_TYPE_SIZE
#    define count_trailing_zeros(count, x) (count = __builtin_ctz(x))
#    undef COUNT_LEADING_ZEROS_0 /* Input X=0 is undefined for the builtin. */
#  endif
#endif

#if !defined (count_leading_zeros)
extern
#  ifdef __STDC__
const
#  endif
unsigned char _gcry_clz_tab[];
#  define MPI_INTERNAL_NEED_CLZ_TAB 1
#  define count_leading_zeros(count, x) \
  do {									\
    UWtype __xr = (x);							\
    UWtype __a; 							\
									\
    if (W_TYPE_SIZE <= 32)						\
      { 								\
	__a = __xr < ((UWtype) 1 << 2*__BITS4)				\
	  ? (__xr < ((UWtype) 1 << __BITS4) ? 0 : __BITS4)		\
	  : (__xr < ((UWtype) 1 << 3*__BITS4) ?  2*__BITS4 : 3*__BITS4);\
      } 								\
    else								\
      { 								\
	for (__a = W_TYPE_SIZE - 8; __a > 0; __a -= 8)			\
	  if (((__xr >> __a) & 0xff) != 0)				\
	    break;							\
      } 								\
									\
    (count) = W_TYPE_SIZE - (_gcry_clz_tab[__xr >> __a] + __a);		\
  } while (0)
/* This version gives a well-defined value for zero. */
#  define COUNT_LEADING_ZEROS_0 W_TYPE_SIZE
#endif /* !count_leading_zeros */

#if !defined (count_trailing_zeros)
/* Define count_trailing_zeros using count_leading_zeros.  The latter might be
   defined in asm, but if it is not, the C version above is good enough.  */
#  define count_trailing_zeros(count, x) \
  do {									\
    UWtype __ctz_x = (x);						\
    UWtype __ctz_c;							\
    count_leading_zeros (__ctz_c, __ctz_x & -__ctz_x);			\
    (count) = W_TYPE_SIZE - 1 - __ctz_c;				\
  } while (0)
#endif /* !count_trailing_zeros */

#ifndef UDIV_NEEDS_NORMALIZATION
#  define UDIV_NEEDS_NORMALIZATION 0
#endif

/***************************************
 ******  longlong.h ends here  *********
 ***************************************/
