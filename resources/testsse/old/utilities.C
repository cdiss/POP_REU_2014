/***************************************************************************
 *cr                                                                       
 *cr            (C) Copyright 1995-2011 The Board of Trustees of the           
 *cr                        University of Illinois                       
 *cr                         All Rights Reserved                        
 *cr                                                                   
 ***************************************************************************/

/***************************************************************************
 * RCS INFORMATION:
 *
 *	$RCSfile: utilities.C,v $
 *	$Author: johns $	$Locker:  $		$State: Exp $
 *	$Revision: 1.133 $	$Date: 2013/01/30 23:06:27 $
 *
 ***************************************************************************
 * DESCRIPTION:
 *
 * General utility routines and definitions.
 *
 ***************************************************************************/

// #define VMDUSESSE 1

#if defined(__SSE2__)
#include <emmintrin.h>
#endif
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#if 0
//
// XXX array init/copy routines that avoid polluting cache, where possible
//
// Fast 16-byte-aligned integer assignment loop for use in the
// VMD color scale routines
void set_1fv_aligned(const int *iv, int n, const int val) {
  int i=0;

#if defined(VMDUSESSE) && defined(__SSE2__)
  __m128i = _mm_set_p
  // do groups of four elements
  for (; i<(n-3); i+=4) {
  }
#endif
}
#endif


//
// Small inlinable SSE helper routines to make code easier to read
//
#if defined(VMDUSESSE) && defined(__SSE2__)

static void print_m128i(__m128i mask4) {
  int * iv = (int *) &mask4;
  printf("vec: %08x %08x %08x %08x\n", iv[0], iv[1], iv[2], iv[3]);
}

static int hand_m128i(__m128i mask4) {
  __m128i tmp = mask4;
  tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(2, 3, 0, 1));
  tmp = _mm_and_si128(mask4, tmp);
  mask4 = tmp;
  tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(1, 0, 3, 2));
  tmp = _mm_and_si128(mask4, tmp);
  mask4 = tmp; // all 4 elements are now set to the reduced mask
  
  int mask = _mm_cvtsi128_si32(mask4); // return zeroth element
  return mask; 
}

static int hor_m128i(__m128i mask4) {
  __m128i tmp = mask4;
  tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(2, 3, 0, 1));
  tmp = _mm_or_si128(mask4, tmp);
  mask4 = tmp;
  tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(1, 0, 3, 2));
  tmp = _mm_or_si128(mask4, tmp);
  mask4 = tmp; // all 4 elements are now set to the reduced mask
  
  int mask = _mm_cvtsi128_si32(mask4); // return zeroth element
  return mask; 
}

static int hadd_m128i(__m128i sum4) {
  __m128i tmp = sum4;
  tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(2, 3, 0, 1));
  tmp = _mm_add_epi32(sum4, tmp);
  sum4 = tmp;
  tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(1, 0, 3, 2));
  tmp = _mm_add_epi32(sum4, tmp);
  sum4 = tmp; // all 4 elements are now set to the sum
  
  int sum = _mm_cvtsi128_si32(sum4); // return zeroth element
  return sum; 
}

static __m128i _mm_sel_m128i(const __m128i &a, const __m128i &b, const __m128i &mask) {
  // (((b ^ a) & mask)^a)
  return _mm_xor_si128(a, _mm_and_si128(mask, _mm_xor_si128(b, a)));
}

static __m128 _mm_sel_ps(const __m128 &a, const __m128 &b, const __m128 &mask) {
  // (((b ^ a) & mask)^a)
  return _mm_xor_ps(a, _mm_and_ps(mask, _mm_xor_ps(b, a)));
}


// helper routine to perform a min among all 4 elements of an __m128
static float fmin_m128(__m128 min4) {
  __m128 tmp;
  tmp = min4;
  tmp = _mm_shuffle_ps(tmp, tmp, _MM_SHUFFLE(2, 3, 0, 1));
  tmp = _mm_min_ps(min4, tmp);
  min4 = tmp;
  tmp = _mm_shuffle_ps(tmp, tmp, _MM_SHUFFLE(1, 0, 3, 2));
  tmp = _mm_min_ps(min4, tmp);
  min4 = tmp; // all 4 elements are now set to the min

  float fmin;
  _mm_store_ss(&fmin, min4);
  return fmin;
}


// helper routine to perform a max among all 4 elements of an __m128
static float fmax_m128(__m128 max4) {
  __m128 tmp = max4;
  tmp = _mm_shuffle_ps(tmp, tmp, _MM_SHUFFLE(2, 3, 0, 1));
  tmp = _mm_max_ps(max4, tmp);
  tmp = _mm_shuffle_ps(tmp, tmp, _MM_SHUFFLE(1, 0, 3, 2));
  tmp = _mm_max_ps(max4, tmp);
  max4 = tmp; // all 4 elements are now set to the max

  float fmax;
  _mm_store_ss(&fmax, max4);
  return fmax;
}
#endif


// Compute min/max values for a 16-byte-aligned array of floats
void minmax_1fv_aligned(const float *f, int n, float *fmin, float *fmax) {
  if (n < 1)
    return;

#if defined(VMDUSESSE) && defined(__SSE2__)
  // SSE vectorized min/max loop
  __m128 min4 = _mm_set_ps1(f[0]);
  __m128 max4 = _mm_set_ps1(f[0]);
  int i=0;

  // do groups of 32 elements
  for (; i<(n-31); i+=32) {
    __m128 f4 = _mm_load_ps(&f[i]); // assume 16-byte aligned array!
    min4 = _mm_min_ps(min4, f4);
    max4 = _mm_max_ps(max4, f4);
    f4 = _mm_load_ps(&f[i+4]); // assume 16-byte aligned array!
    min4 = _mm_min_ps(min4, f4);
    max4 = _mm_max_ps(max4, f4);
    f4 = _mm_load_ps(&f[i+8]); // assume 16-byte aligned array!
    min4 = _mm_min_ps(min4, f4);
    max4 = _mm_max_ps(max4, f4);
    f4 = _mm_load_ps(&f[i+12]); // assume 16-byte aligned array!
    min4 = _mm_min_ps(min4, f4);
    max4 = _mm_max_ps(max4, f4);

    f4 = _mm_load_ps(&f[i+16]); // assume 16-byte aligned array!
    min4 = _mm_min_ps(min4, f4);
    max4 = _mm_max_ps(max4, f4);
    f4 = _mm_load_ps(&f[i+20]); // assume 16-byte aligned array!
    min4 = _mm_min_ps(min4, f4);
    max4 = _mm_max_ps(max4, f4);
    f4 = _mm_load_ps(&f[i+24]); // assume 16-byte aligned array!
    min4 = _mm_min_ps(min4, f4);
    max4 = _mm_max_ps(max4, f4);
    f4 = _mm_load_ps(&f[i+28]); // assume 16-byte aligned array!
    min4 = _mm_min_ps(min4, f4);
    max4 = _mm_max_ps(max4, f4);
  }

  // do groups of 4 elements
  for (; i<(n-3); i+=4) {
    __m128 f4 = _mm_load_ps(&f[i]); // assume 16-byte aligned array!
    min4 = _mm_min_ps(min4, f4);
    max4 = _mm_max_ps(max4, f4);
  }

  // finish last elements off
  for (; i<n; i++) {
    __m128 f4 = _mm_set_ps1(f[i]);
    min4 = _mm_min_ps(min4, f4);
    max4 = _mm_max_ps(max4, f4);
  }

  // compute min/max among the final 4-element vectors by shuffling
  // and and reducing the elements within the vectors
  *fmin = fmin_m128(min4);
  *fmax = fmax_m128(max4);
#else
  // scalar min/max loop
  float min1 = f[0];
  float max1 = f[0];
  for (int i=1; i<n; i++) {
    if (f[i] < min1) min1 = f[i];
    if (f[i] > max1) max1 = f[i];
  }
  *fmin = min1;
  *fmax = max1;
#endif
}


// Compute min/max values for a 16-byte-aligned array of float3s
// input value n3 is the number of 3-element vectors to process
void minmax_3fv_aligned(const float *f, const int n3, float *fmin, float *fmax) {
  float minx, maxx, miny, maxy, minz, maxz;
  const int end = n3*3;
  int i=0;

  if (n3 < 1)
    return;

  minx=maxx=f[0];
  miny=maxy=f[1];
  minz=maxz=f[2];

#if defined(VMDUSESSE) && defined(__SSE2__)
  // initialize min/max values
  __m128 xmin4 = _mm_set_ps1(f[0]);
  __m128 xmax4 = xmin4;
  __m128 ymin4 = _mm_set_ps1(f[1]);
  __m128 ymax4 = ymin4;
  __m128 zmin4 = _mm_set_ps1(f[2]);
  __m128 zmax4 = zmin4;

  for (; i<(end-11); i+=12) {
    // aligned load of four consecutive 3-element vectors into
    // three 4-element vectors
    __m128 x0y0z0x1 = _mm_load_ps(f+i+0);
    __m128 y1z1x2y2 = _mm_load_ps(f+i+4);
    __m128 z2x3y3z3 = _mm_load_ps(f+i+8);

    // convert rgb3f AOS format to 4-element SOA vectors using shuffle instructions
    __m128 x2y2x3y3 = _mm_shuffle_ps(y1z1x2y2, z2x3y3z3, _MM_SHUFFLE(2, 1, 3, 2));
    __m128 y0z0y1z1 = _mm_shuffle_ps(x0y0z0x1, y1z1x2y2, _MM_SHUFFLE(1, 0, 2, 1));
    __m128 x        = _mm_shuffle_ps(x0y0z0x1, x2y2x3y3, _MM_SHUFFLE(2, 0, 3, 0)); // x0x1x2x3
    __m128 y        = _mm_shuffle_ps(y0z0y1z1, x2y2x3y3, _MM_SHUFFLE(3, 1, 2, 0)); // y0y1y2y3
    __m128 z        = _mm_shuffle_ps(y0z0y1z1, z2x3y3z3, _MM_SHUFFLE(3, 0, 3, 1)); // z0y1z2z3

    // compute mins and maxes
    xmin4 = _mm_min_ps(xmin4, x);
    xmax4 = _mm_max_ps(xmax4, x);
    ymin4 = _mm_min_ps(ymin4, y);
    ymax4 = _mm_max_ps(ymax4, y);
    zmin4 = _mm_min_ps(zmin4, z);
    zmax4 = _mm_max_ps(zmax4, z);
  }

  minx = fmin_m128(xmin4);
  miny = fmin_m128(ymin4);
  minz = fmin_m128(zmin4);

  maxx = fmax_m128(xmax4);
  maxy = fmax_m128(ymax4);
  maxz = fmax_m128(zmax4);
#endif

  // regular C code... 
  for (; i<end; i+=3) {
    float tmpx = f[i  ];
    if (tmpx < minx) minx = tmpx;
    if (tmpx > maxx) maxx = tmpx;

    float tmpy = f[i+1];
    if (tmpy < miny) miny = tmpy;
    if (tmpy > maxy) maxy = tmpy;

    float tmpz = f[i+2];
    if (tmpz < minz) minz = tmpz;
    if (tmpz > maxz) maxz = tmpz;
  }

  fmin[0] = minx;
  fmax[0] = maxx;
  fmin[1] = miny;
  fmax[1] = maxy;
  fmin[2] = minz;
  fmax[2] = maxz;
}


// Compute min/max values for a 16-byte-aligned array of float3s
// input value n3 is the number of 3-element vectors to process
void minmax_selected_3fv_aligned(const float *f, const int *on, const int n3, 
                                 const int firstsel, const int lastsel,
                                 float *fmin, float *fmax) {
  float minx, maxx, miny, maxy, minz, maxz;

  if ((n3 < 1) || (firstsel < 0) || (lastsel >= n3))
    return;

  // start at first selected atom
  int i=firstsel;
  minx=maxx=f[i*3  ];
  miny=maxy=f[i*3+1];
  minz=maxz=f[i*3+2];

  int end=lastsel+1;

#if defined(VMDUSESSE) && defined(__SSE2__)
  // since we may not be on a 48-byte boundary, when we start, we roll 
  // through the first few items with plain C until we get to one.
  for (; i<end; i++) {
    // exit once we have reached a 48-byte boundary
    if (i % 4 == 0)
      break;

    if (on[i]) {
      int ind3 = i * 3;
      float tmpx = f[ind3  ];
      if (tmpx < minx) minx = tmpx;
      if (tmpx > maxx) maxx = tmpx;

      float tmpy = f[ind3+1];
      if (tmpy < miny) miny = tmpy;
      if (tmpy > maxy) maxy = tmpy;

      float tmpz = f[ind3+2];
      if (tmpz < minz) minz = tmpz;
      if (tmpz > maxz) maxz = tmpz;
    }
  }

  // initialize min/max values
  __m128 xmin4 = _mm_set_ps1(minx);
  __m128 xmax4 = xmin4;
  __m128 ymin4 = _mm_set_ps1(miny);
  __m128 ymax4 = ymin4;
  __m128 zmin4 = _mm_set_ps1(minz);
  __m128 zmax4 = zmin4;

  const __m128i true4 = _mm_set1_epi32(1);
  for (; i<(end-3); i+=4) {
    // aligned load of four selection flags
    __m128i on4 = _mm_load_si128((__m128i*) &on[i]);

    // compute atom selection mask
    __m128i mask = _mm_cmpeq_epi32(true4, on4);
    if (!hor_m128i(mask))
      continue; // no atoms selected 

    // aligned load of four consecutive 3-element vectors into
    // three 4-element vectors
    int ind3 = i*3;
    __m128 x0y0z0x1 = _mm_load_ps(&f[ind3+0]);
    __m128 y1z1x2y2 = _mm_load_ps(&f[ind3+4]);
    __m128 z2x3y3z3 = _mm_load_ps(&f[ind3+8]);

    // convert rgb3f AOS format to 4-element SOA vectors using shuffle instructions
    __m128 x2y2x3y3 = _mm_shuffle_ps(y1z1x2y2, z2x3y3z3, _MM_SHUFFLE(2, 1, 3, 2));
    __m128 y0z0y1z1 = _mm_shuffle_ps(x0y0z0x1, y1z1x2y2, _MM_SHUFFLE(1, 0, 2, 1));
    __m128 x        = _mm_shuffle_ps(x0y0z0x1, x2y2x3y3, _MM_SHUFFLE(2, 0, 3, 0)); // x0x1x2x3
    __m128 y        = _mm_shuffle_ps(y0z0y1z1, x2y2x3y3, _MM_SHUFFLE(3, 1, 2, 0)); // y0y1y2y3
    __m128 z        = _mm_shuffle_ps(y0z0y1z1, z2x3y3z3, _MM_SHUFFLE(3, 0, 3, 1)); // z0y1z2z3

    // compute mins and maxes
    xmin4 = _mm_sel_ps(xmin4, _mm_min_ps(xmin4, x), (__m128) mask);
    xmax4 = _mm_sel_ps(xmax4, _mm_max_ps(xmax4, x), (__m128) mask);
    ymin4 = _mm_sel_ps(ymin4, _mm_min_ps(ymin4, y), (__m128) mask);
    ymax4 = _mm_sel_ps(ymax4, _mm_max_ps(ymax4, y), (__m128) mask);
    zmin4 = _mm_sel_ps(zmin4, _mm_min_ps(zmin4, z), (__m128) mask);
    zmax4 = _mm_sel_ps(zmax4, _mm_max_ps(zmax4, z), (__m128) mask);
  }

  minx = fmin_m128(xmin4);
  miny = fmin_m128(ymin4);
  minz = fmin_m128(zmin4);

  maxx = fmax_m128(xmax4);
  maxy = fmax_m128(ymax4);
  maxz = fmax_m128(zmax4);
#endif

  // regular C code... 
  for (; i<end; i++) {
    if (on[i]) {
      int ind3 = i * 3;
      float tmpx = f[ind3  ];
      if (tmpx < minx) minx = tmpx;
      if (tmpx > maxx) maxx = tmpx;

      float tmpy = f[ind3+1];
      if (tmpy < miny) miny = tmpy;
      if (tmpy > maxy) maxy = tmpy;

      float tmpz = f[ind3+2];
      if (tmpz < minz) minz = tmpz;
      if (tmpz > maxz) maxz = tmpz;
    }
  }

  fmin[0] = minx;
  fmax[0] = maxx;
  fmin[1] = miny;
  fmax[1] = maxy;
  fmin[2] = minz;
  fmax[2] = maxz;
}


#include "WKFUtils.h"

int main() {
  int isz = 100000000;
  int sz = 3 * isz;
  int *on = (int *) calloc(1, isz * sizeof(int));
  float *fv = (float *) malloc(sz * sizeof(float));

  for (int i=0; i<sz; i++) 
    fv[i] = (float) i; 

  wkf_timerhandle timer = wkf_timer_create();

  wkf_timer_start(timer);
  float minf, maxf;
  minmax_1fv_aligned(fv, sz, &minf, &maxf);
  wkf_timer_stop(timer);
  printf("minmax_1fv_aligned: %f\n", wkf_timer_time(timer));
//  printf("min: %f max: %f\n", minf, maxf);

  float minfv[3], maxfv[3];
  wkf_timer_stop(timer);
  minmax_3fv_aligned(fv, sz/3, minfv, maxfv);
  wkf_timer_stop(timer);
  printf("minmax_3fv_aligned: %f\n", wkf_timer_time(timer));

  int i;
  for (i=0; i<3; i++) {
    minf += minfv[i];
    maxf += maxfv[i];
  }

  wkf_timer_start(timer);
  minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
  wkf_timer_stop(timer);
  printf("minmax_selected_3fv_aligned(  0%%): %f\n", wkf_timer_time(timer));

  for (i=0; i<isz; i+=8) {
    on[i  ] = 1;
  }
  wkf_timer_start(timer);
  minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
  wkf_timer_stop(timer);
  printf("minmax_selected_3fv_aligned( 12%%): %f\n", wkf_timer_time(timer));
  for (i=0; i<3; i++) {
    minf += minfv[i];
    maxf += maxfv[i];
  }


  for (i=0; i<isz; i+=8) {
    on[i  ] = 1;
    on[i+1] = 1;
    on[i+2] = 1;
    on[i+3] = 1;
  }
  wkf_timer_start(timer);
  minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
  wkf_timer_stop(timer);
  printf("minmax_selected_3fv_aligned( 50%%): %f\n", wkf_timer_time(timer));
  for (i=0; i<3; i++) {
    minf += minfv[i];
    maxf += maxfv[i];
  }


  for (i=0; i<isz; i++)
    on[i] = 1;
  wkf_timer_start(timer);
  minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
  wkf_timer_stop(timer);
  printf("minmax_selected_3fv_aligned(100%%): %f\n", wkf_timer_time(timer));

  for (i=0; i<3; i++) {
    minf += minfv[i];
    maxf += maxfv[i];
  }

  wkf_timer_destroy(timer);  

  printf("checking sum: %g\n", minf+maxf);

  return 0;
}




