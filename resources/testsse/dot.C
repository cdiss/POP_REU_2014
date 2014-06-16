#include <stdio.h>

#include <emmintrin.h>
#include <immintrin.h>
#include "msmloops.h"

#define C1_VECTOR_SIZE 8

int testmsm(msmparms &parms) {
#if 1
    __m256 uelem8 = _mm256_load_ps(parms.au);

    __m256 v8 = _mm256_set1_ps(0.0f);
    for (int j=0;  j < C1_VECTOR_SIZE;  j++) {
      __m256 us8 = _mm256_broadcast_ss(&parms.au[j]);
      __m256 melem8 = _mm256_load_ps(parms.aa + j*8);
      __m256 tmp8 = _mm256_mul_ps(melem8, us8);
      v8 = _mm256_add_ps(v8, tmp8);
    }
    __m256 newv8 = _mm256_load_ps(parms.av);
    newv8 = _mm256_add_ps(newv8, v8);
    _mm256_store_ps(parms.av, newv8);
#elif 1
  __m256 uelem8 = _mm256_load_ps(parms.au);
  __m256 v8;

  for (int j=0;  j < C1_VECTOR_SIZE;  j++) {
    __m256 melem8 = _mm256_load_ps(parms.aa + j*8);
    __m256 tmp8 = _mm256_dp_ps(melem8, uelem8, 0xff);
    tmp8 = _mm256_add_ps(tmp8, _mm256_permute2f128_ps(tmp8, tmp8, 0x01));
    v8 = _mm256_blend_ps(v8, tmp8, 1<<j);
  }
  __m256 newv8 = _mm256_load_ps(parms.av);
  newv8 = _mm256_add_ps(newv8, v8);
  _mm256_store_ps(parms.av, newv8);
#else
  __m256 uelem8 = _mm256_load_ps(parms.au);
  for (int j=0;  j < C1_VECTOR_SIZE;  j++) {
    __m256 melem8 = _mm256_load_ps( parms.aa + (j<<3) );
    __m256 tmp8 = _mm256_dp_ps(melem8, uelem8, 0xff);
    parms.av[j] += ((float *) &tmp8)[0] + ((float *) &tmp8)[4];
  }
#endif
  return 0;
}

int main() {
  float a[8] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f };
  float b[8] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f };
  float c[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

  __m256 a8 = _mm256_loadu_ps(a);
  __m256 b8 = _mm256_loadu_ps(b);
  __m256 c8 = _mm256_dp_ps(a8, b8, 0xff);
  c8 = _mm256_add_ps(c8, _mm256_permute2f128_ps(c8, c8, 0x01));

//  c8 = _mm256_shuffle_ps(a8, b8, _MM_SHUFFLE(0, 3, 2, 1));
//  c8 = _mm256_hadd_ps(c8, c8);
//  c8 = _mm256_unpackhi_ps(c8, c8);

  _mm256_storeu_ps(c, c8);

  int i;
  for (i=0; i<8; i++) {
    printf("a*b[%d]: %g   c[%d]: %g\n", i, a[i]*b[i], i, c[i]);
  }
  return 0;
}
