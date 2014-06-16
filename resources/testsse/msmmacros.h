//
// innermost loop from new MSM code
//

// #define VMDUSESSE 1
// #define VMDUSEAVX 1
// #define VMDUSENEON 1

#if defined(VMDUSESSE) && defined(__SSE2__)
#include <emmintrin.h>
#endif
#if defined(VMDUSEAVX) && defined(__AVX__)
#include <immintrin.h>
#endif
#if defined(VMDUSENEON) && defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "WKFUtils.h"

#define C1_VECTOR_SIZE 8

// Calculate sum of matrix-vector product for 8x8 A and 8x1 u:  v += A u
// where all are float *
//
// Hand-coded SSE2 vectorization
// This loop requires that the single-precision input arrays be
// aligned on 16-byte boundaries, such that array[index % 4 == 0]
// can be safely accessed with aligned load/store operations
#define FAST_MATVEC8_SSE2( v, A, u ) \
  do { \
    for (int k=0, j=0;  j < C1_VECTOR_SIZE;  j++,k+=8) { \
      __m128 melem4 = _mm_load_ps( A + k ); \
      __m128 uelem4 = _mm_load_ps( u ); \
      __m128 tmp4 = _mm_mul_ps(melem4, uelem4); \
      melem4 = _mm_load_ps( A + k + 4 ); \
      uelem4 = _mm_load_ps( u + 4 ); \
      tmp4 = _mm_add_ps(tmp4, _mm_mul_ps(melem4, uelem4));  \
\
      /* do a 4-element reduction and accumulate result */ \
      __m128 sum4 = tmp4; \
      sum4 = _mm_shuffle_ps(sum4, sum4, _MM_SHUFFLE(2, 3, 0, 1)); \
      sum4 = _mm_add_ps(sum4, tmp4); \
      tmp4 = sum4; \
      sum4 = _mm_shuffle_ps(sum4, sum4, _MM_SHUFFLE(1, 0, 3, 2)); \
      sum4 = _mm_add_ps(sum4, tmp4); \
\
      /* all 4 elements are now set to the sum */ \
      float sum; \
      _mm_store_ss(&sum, sum4); /* store lowest element */ \
      v[j] += sum; \
    } \
  } while (0) // terminate macro with ;


// XXX AVX code not tested yet
//
// Calculate sum of matrix-vector product for 8x8 A and 8x1 u:  v += A u
// where all are float *
//
// Hand-coded AVX vectorization
// This loop requires that the single-precision input arrays be
// aligned on 32-byte boundaries, such that array[index % 8 == 0]
// can be safely accessed with aligned load/store operations
#if 0
#define FAST_MATVEC8_AVX( v, A, u )
#elif 0
// XXX hack for performance testing 
#define FAST_MATVEC8_AVX( v, A, u ) \
  do { \
    __m256 uelem8 = _mm256_load_ps( u ); \
    for (int k=0, j=0;  j < C1_VECTOR_SIZE;  j++,k+=8) { \
      __m256 melem8 = _mm256_load_ps( A + k ); \
      __m256 tmp8 = _mm256_add_ps(melem8, uelem8); \
      v[j] += ((float *) &tmp8)[0] + ((float *) &tmp8)[4]; \
    } \
  } while (0) // terminate macro with ;
#elif 1
#define FAST_MATVEC8_AVX( v, A, u ) \
  do { \
    __m256 uelem8 = _mm256_load_ps( u ); \
    for (int j=0;  j < C1_VECTOR_SIZE;  j++) { \
      __m256 melem8 = _mm256_load_ps( A + (j<<3) ); \
      __m256 tmp8 = _mm256_dp_ps(melem8, uelem8, 0xff); \
      v[j] += ((float *) &tmp8)[0] + ((float *) &tmp8)[4]; \
    } \
  } while (0) // terminate macro with ;
#else
#define FAST_MATVEC8_AVX( v, A, u ) \
  do { \
    __m256 uelem8 = _mm256_load_ps( u ); \
    for (int k=0, j=0;  j < C1_VECTOR_SIZE;  j++,k+=8) { \
      __m256 melem8 = _mm256_load_ps( A + k ); \
      __m256 tmp8 = _mm256_mul_ps(melem8, uelem8); \
\
      /* XXX this still needs to be rewritten a bit for AVX */ \
      /* do an 8-element reduction and accumulate result */ \
      __m256 sum8 = tmp8; \
      sum8 = _mm256_hadd_ps(sum8, sum8); \
      sum8 = _mm256_hadd_ps(sum8, sum8); \
      tmp8 = sum8; \
      tmp8 = _mm256_permute2f128_ps(tmp8, tmp8, 1); \
      sum8 = _mm256_hadd_ps(tmp8, sum8); \
\
      /* all 8 elements are now set to the sum */ \
      float sum; \
      _mm_store_ss(&sum, sum8); /* store lowest element */ \
      v[j] += sum; \
    } \
  } while (0) // terminate macro with ;
#endif


#define FAST_MATVEC8_C( v, A, u ) \
  do { \
    for (int k=0, j=0;  j < C1_VECTOR_SIZE;  j++) { \
      for (int i = 0;  i < C1_VECTOR_SIZE;  i++, k++) { \
        v[j] += A[k] * u[i]; \
      } \
    } \
  } while (0) // terminate macro with ;







