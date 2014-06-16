#include <emmintrin.h>
#include <immintrin.h>
#include "msmloops.h"

#define C1_VECTOR_SIZE 8

int testmsm(msmparms &parms) {
  __m256 uelem8 = _mm256_load_ps(parms.au);
  for (int j=0;  j < C1_VECTOR_SIZE;  j++) {
    __m256 melem8 = _mm256_load_ps( parms.aa + (j<<3) );
    __m256 tmp8 = _mm256_dp_ps(melem8, uelem8, 0xff);
    parms.av[j] += ((float *) &tmp8)[0] + ((float *) &tmp8)[4];
  }
  return 0;
}



