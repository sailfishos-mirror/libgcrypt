/* mpih-pow.c  -  MPI helper functions
 * Copyright (C) 2025  g10 Code GmbH
 *
 * This file is part of Libgcrypt.
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <https://www.gnu.org/licenses/>.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi-internal.h"
#include "longlong.h"

/*
  Compute -n^(-1) mod 2^BITS_PER_MPI_LIMB

  For n^(-1) mod 2^N:

  An Improved Integer Modular Multiplicative Inverse (modulo 2w)
  Jeffrey Hurchalla

  https://arxiv.org/abs/2204.04342
*/
static mpi_limb_t
compute_minv (mpi_limb_t n)
{
  mpi_limb_t x, y;

  gcry_assert (n%2 == 1);

  x = (3*n)^2;
  y = 1 - n*x;
  x = x*(1 + y);
  y *= y;
  x = x*(1 + y);
  y *= y;
  x = x*(1 + y);
#if BITS_PER_MPI_LIMB == 32
  return -x;
#elif BITS_PER_MPI_LIMB == 64
  y *= y;
  x = x*(1 + y);
  return -x;
#else
# error "Please implement multiplicative inverse mod power of 2"
#endif
}

#define MAX_SCRATCH_SPACE 256

static void
mont_reduc (mpi_ptr_t rp, mpi_ptr_t tp,
            mpi_ptr_t mp, mpi_size_t n, mpi_limb_t minv)
{
  mpi_size_t i;
  mpi_limb_t cy;

  for (i = 0; i < n; i++)
    {
      mpi_limb_t ui = tp[i] * minv;

      cy = _gcry_mpih_addmul_1 (tp + i, mp, n, ui);
      tp[n+i] += cy;
    }

#define TESTING 1
#ifdef TESTING
  {
    mpi_limb_t res = 0;

    for (i = 0; i < n; i++)
      res |= tp[i];

    gcry_assert (res == 0);
  }
#endif

  cy = _gcry_mpih_sub_n (rp, tp + n, mp, n);
  _gcry_mpih_set_cond (rp, tp + n, n, cy != 0);
}

/* RP should have 2*N limbs */
static void
mont_mul (mpi_ptr_t rp, mpi_ptr_t xp, mpi_ptr_t yp, mpi_ptr_t mp,
          mpi_size_t n, mpi_limb_t minv)
{
  mpi_limb_t temp0[MAX_SCRATCH_SPACE*2];

  _gcry_mpih_mul_sec (temp0, xp, n, yp, n);
  mont_reduc (rp, temp0, mp, n, minv);
}

void
_gcry_mpih_powm_sec (mpi_ptr_t rp, mpi_ptr_t bp, mpi_ptr_t mp, mpi_size_t n,
                     mpi_ptr_t ep, mpi_size_t en)
{
  mpi_limb_t temp0[MAX_SCRATCH_SPACE*2];
  mpi_limb_t temp1[MAX_SCRATCH_SPACE];
  mpi_limb_t temp2[MAX_SCRATCH_SPACE];
  mpi_limb_t a[MAX_SCRATCH_SPACE];
  mpi_limb_t x_tilde[MAX_SCRATCH_SPACE];
  mpi_limb_t minv;
  mpi_size_t i;
  mpi_limb_t e;
  int c;
  int mod_shift_cnt;

  gcry_assert (n < MAX_SCRATCH_SPACE);

  minv = compute_minv (mp[0]);

  gcry_assert (mp[0]*(-minv) == 1);

  MPN_ZERO (temp0, MAX_SCRATCH_SPACE);

  /* TEMP0 := R mod m */
  count_leading_zeros (mod_shift_cnt, mp[n-1]);
  if (mod_shift_cnt)
    {
      _gcry_mpih_lshift (temp2, mp, n, mod_shift_cnt);
      temp0[n] = (mpi_limb_t)1 << mod_shift_cnt;
    }
  else
    {
      MPN_COPY (temp2, mp, n);
      temp0[n] = 1;
    }
  _gcry_mpih_divrem (temp1, 0, temp0, n+1, temp2, n);
  if (mod_shift_cnt)
    _gcry_mpih_rshift (temp0, temp0, n, mod_shift_cnt);
  /* A := R mod m */
  MPN_COPY (a, temp0, n);

  /* TEMP0 := (R mod m)^2 */
  _gcry_mpih_sqr_n_basecase (temp0, a, n);

  /* TEMP0 := R^2 mod m */
  if (mod_shift_cnt)
    _gcry_mpih_lshift (temp0, temp0, n*2, mod_shift_cnt);
  _gcry_mpih_divrem (temp1, 0, temp0, n*2, temp2, n);
  if (mod_shift_cnt)
    _gcry_mpih_rshift (temp0, temp0, n, mod_shift_cnt);
  /* x~ := Mont(x, R^2 mod m) */
  mont_mul (x_tilde, bp, temp0, mp, n, minv);

  mont_mul (a, a, x_tilde, mp, n, minv);
#ifdef TESTING
  {
    int i;
    for (i = 0; i < n; i++)
      gcry_assert (a[i] == x_tilde[i]);
  }
#endif
#if 0
  MPN_COPY (a, x_tilde, n);
#endif
  i = en - 1;
  e = ep[i];
  count_leading_zeros (c, e);
  e = (e << c) << 1;
  c = BITS_PER_MPI_LIMB - 1 - c;
  for (;;)
    {
      while (c)
        {
          mont_mul (a, a, a, mp, n, minv);
          mont_mul (temp0, a, x_tilde, mp, n, minv);
          _gcry_mpih_set_cond (a, temp0, n, ((mpi_limb_signed_t)e < 0));
          e <<= 1;
          c--;
        }

      i--;
      if (i < 0)
        break;
      e = ep[i];
      c = BITS_PER_MPI_LIMB;
    }

  MPN_ZERO (temp0, MAX_SCRATCH_SPACE);
  temp0[0] = 1;
  mont_mul (a, a, temp0, mp, n, minv);

  MPN_COPY (rp, a, n);
}
