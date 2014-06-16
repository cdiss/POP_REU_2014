#include "msmmacros.h"
#include "msmloops.h"
#include "WKFUtils.h"
#include "utilities.h"

#define MATSZ (C1_VECTOR_SIZE * C1_VECTOR_SIZE * C1_VECTOR_SIZE)


//
// Helper routine for use when coping with unaligned
// buffers returned by malloc() on many GNU systems:
//   http://gcc.gnu.org/bugzilla/show_bug.cgi?id=24261
//   http://www.sourceware.org/bugzilla/show_bug.cgi?id=206
//
// XXX until all compilers support uintptr_t, we have to do
//     dangerous and ugly things with pointer casting here...
//
#if 1
/* sizeof(unsigned long) == sizeof(void*) */
#define myintptrtype unsigned long
#elif 1
/* sizeof(size_t) == sizeof(void*) */
#define myintptrtype size_t
#else
/* C99 */
#define myintptrtype uintptr_t
#endif

// arbitrary pointer alignment test
static int is_Nbyte_aligned(const void *ptr, int N) {
  return ((((myintptrtype) ptr) % N) == 0);
}


// Aligment test routine for x86 16-byte SSE vector instructions
static int is_16byte_aligned(const void *ptr) {
  return (((myintptrtype) ptr) == (((myintptrtype) ptr) & (~0xf)));
}


// Aligment test routine for x86 32-byte AVX vector instructions
static int is_32byte_aligned(const void *ptr) {
  return (((myintptrtype) ptr) == (((myintptrtype) ptr) & (~0x1f)));
}


// Aligment test routine for x86 LRB/MIC 64-byte vector instructions
static int is_64byte_aligned(const void *ptr) {
  return (((myintptrtype) ptr) == (((myintptrtype) ptr) & (~0x3f)));
}


int setupmsm(msmparms &parms) {
  int sz = MATSZ * sizeof(float);
  parms.v = (float *) calloc(1, sz + 4096);
  parms.a = (float *) calloc(1, sz + 4096);
  parms.u = (float *) calloc(1, sz + 4096);

  parms.av = parms.v;
  parms.aa = parms.a;
  parms.au = parms.u;

  if (!is_32byte_aligned(parms.av)) {
    printf("aligning av");
    while (!is_32byte_aligned(parms.av)) {
      parms.av++;
      printf(".");
    }
    printf("\n");
  }
  if (!is_32byte_aligned(parms.aa)) {
    printf("aligning aa");
    while (!is_32byte_aligned(parms.aa)) {
      parms.aa++;
      printf(".");
    }
    printf("\n");
  }
  if (!is_32byte_aligned(parms.au)) {
    printf("aligning au");
    while (!is_32byte_aligned(parms.au)) {
      parms.au++;
      printf(".");
    }
    printf("\n");
  }

  vmd_srandom(31415926);
  int i;
  for (i=0; i<MATSZ; i++) {
    parms.av[i] = 0.0f;
    parms.aa[i] = 1.0 + double(vmd_random()) / double(VMD_RAND_MAX);
    parms.au[i] = 1.0 + double(vmd_random()) / double(VMD_RAND_MAX);
  }

  return 0;
}

int finishmsm(msmparms &parms) { 
  int i;
  if (1)
  for (i=0; i<C1_VECTOR_SIZE; i++) {
    printf("[%d] v: %g  a: %g  u: %g\n",
           i, parms.av[i], parms.aa[i], parms.au[i]);
  }

  free(parms.v);
  free(parms.a);
  free(parms.u); 

  return 0;
}

int testmsm(msmparms &parms) { 
  int i;
  for (i=0; i<(10 * 1000000); i++) {
//    parms.aa[3] += 0.000001 * (1.0 + double(vmd_random()) / double(VMD_RAND_MAX));
    parms.aa[3] += 1.000001;

#if 1 && defined(__AVX__) && defined(VMDUSEAVX)
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
#elif 1 && defined(__AVX__) && defined(VMDUSEAVX)
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
#elif 1 && defined(__AVX__) && defined(VMDUSEAVX)
    for (int j=0;  j < C1_VECTOR_SIZE;  j++) {
      __m256 melem8 = _mm256_load_ps( parms.aa + (j<<3) );
      __m256 tmp8 = _mm256_dp_ps(melem8, uelem8, 0xff);
#if 1
      tmp8 = _mm256_add_ps(tmp8, _mm256_permute2f128_ps(tmp8, tmp8, 0x01));
      parms.av[j] += *((float *) &tmp8);
#else
      parms.av[j] += ((float *) &tmp8)[0] + ((float *) &tmp8)[4];
#endif
    }
#elif defined(__AVX__) && defined(VMDUSEAVX)
    FAST_MATVEC8_AVX(parms.av, parms.aa, parms.au);
#elif defined(__SSE2__) && defined(VMDUSESSE)
    FAST_MATVEC8_SSE2(parms.av, parms.aa, parms.au);
#else
    FAST_MATVEC8_C(parms.av, parms.aa, parms.au);
#endif
  }

  return 0;
}

