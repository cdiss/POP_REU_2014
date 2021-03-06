#include <stdio.h>
#include <stdint.h>
#include "omp.h"

// Intrinsics header files
#ifdef __SSE2__
#include "xmmintrin.h"
#endif
#ifdef __AVX__
#include "immintrin.h"
#endif
#ifdef __MIC__
#include "immintrin.h"
#endif

// ISPC include file automatically generated by make
#if defined __SSE2__ || defined  __AVX__
//#include "mandel_ispc.h" 
#endif

// MULTITHREADED: Compile with -DMULTITHREADED to use multithreading (recommended)
// UNROLL_FACTOR: must be compiled with -DUNROLL_FACTOR=<num> where <num>=1, 2, 4, or 8

// this function prototype should be included in the auto-generated include file above.
// Despite the fact that is indeed included there, for some reason the code does not recognize "ispcMandel_inner" unless this prototype is included directly here.
extern "C" {
    extern void ispcMandel_inner(float startReal, float startImag, int32_t steps, int32_t horizsteps, float step, uint32_t * output, uint32_t maxiters);
}

void serialMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxIters) {
    
#ifdef MULTITHREADED
#pragma omp parallel for schedule(dynamic)
#endif
#pragma novector
    for (int i = 0; i < steps; i++) {
#pragma novector
        for (int j = 0; j < horizsteps; j++) {
            float real = startReal + step*j;
            float imag = startImag - step*i;
            unsigned iters = 0;
            float z_real = 0.0f, z_imag = 0.0f, z_sum = 0.0f;
#if UNROLL_FACTOR > 1
            unsigned iters_old = 0;
            float z_real_old, z_imag_old, z_sum_old;
#endif
            while (z_sum < 4.0f && iters < maxIters) {
#if UNROLL_FACTOR > 1
                iters_old = iters;
                z_real_old = z_real;
                z_imag_old = z_imag;
                z_sum_old = z_sum;
#endif
                
                iters += UNROLL_FACTOR;
                
                float z_real_sq = z_real*z_real;
                float z_imag_sq = z_imag*z_imag;
                z_sum = z_real_sq + z_imag_sq;
                z_imag = 2.0f*z_real*z_imag + imag;
                z_real = z_real_sq - z_imag_sq + real;

#define EXTRA_ITERATION_SERIAL \
z_real_sq = z_real*z_real; \
z_imag_sq = z_imag*z_imag; \
z_sum = z_real_sq + z_imag_sq; \
z_imag = 2.0f*z_real*z_imag + imag; \
z_real = z_real_sq - z_imag_sq + real; 

#if UNROLL_FACTOR >= 2
                EXTRA_ITERATION_SERIAL
#if UNROLL_FACTOR >= 4
                EXTRA_ITERATION_SERIAL
                EXTRA_ITERATION_SERIAL
#if UNROLL_FACTOR >= 8
                EXTRA_ITERATION_SERIAL
                EXTRA_ITERATION_SERIAL
                EXTRA_ITERATION_SERIAL
                EXTRA_ITERATION_SERIAL
#endif
#endif
#endif
            } 

#if UNROLL_FACTOR > 1
            iters = iters_old;
            z_real = z_real_old;
            z_imag = z_imag_old;
            z_sum = z_sum_old;
            while (z_sum < 4.0f && iters < maxIters) {
                iters++;
                float z_real_sq = z_real*z_real;
                float z_imag_sq = z_imag*z_imag;
                z_sum = z_real_sq + z_imag_sq;
                z_imag = 2.0f*z_real*z_imag + imag;
                z_real = z_real_sq - z_imag_sq + real;
            }
#endif
            output[i*horizsteps+j] = iters;
        }
    }
}


void autoVecMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxIters) {
    
#ifdef MULTITHREADED
#pragma omp parallel for schedule(dynamic)
#endif
    for (int i = 0; i < steps; i++) {
#pragma simd assert
        for (int j = 0; j < horizsteps; j++) {
            float real = startReal + step*j;
            float imag = startImag - step*i;
            unsigned iters = 0;
            float z_real = 0.0f, z_imag = 0.0f, z_sum = 0.0f;
#if UNROLL_FACTOR > 1
            unsigned iters_old = 0;
            float z_real_old, z_imag_old, z_sum_old;
#endif
            while (z_sum < 4.0f && iters < maxIters) {
#if UNROLL_FACTOR > 1
                iters_old = iters;
                z_real_old = z_real;
                z_imag_old = z_imag;
                z_sum_old = z_sum;
#endif
                
                iters += UNROLL_FACTOR;
                
                float z_real_sq = z_real*z_real;
                float z_imag_sq = z_imag*z_imag;
                z_sum = z_real_sq + z_imag_sq;
                z_imag = 2.0f*z_real*z_imag + imag;
                z_real = z_real_sq - z_imag_sq + real;

#if UNROLL_FACTOR >= 2
                EXTRA_ITERATION_SERIAL
#if UNROLL_FACTOR >= 4
                EXTRA_ITERATION_SERIAL
                EXTRA_ITERATION_SERIAL
#if UNROLL_FACTOR >= 8
                EXTRA_ITERATION_SERIAL
                EXTRA_ITERATION_SERIAL
                EXTRA_ITERATION_SERIAL
                EXTRA_ITERATION_SERIAL
#endif
#endif
#endif
            } 

#if UNROLL_FACTOR > 1
            iters = iters_old;
            z_real = z_real_old;
            z_imag = z_imag_old;
            z_sum = z_sum_old;
            while (z_sum < 4.0f && iters < maxIters) {
                iters++;
                float z_real_sq = z_real*z_real;
                float z_imag_sq = z_imag*z_imag;
                z_sum = z_real_sq + z_imag_sq;
                z_imag = 2.0f*z_real*z_imag + imag;
                z_real = z_real_sq - z_imag_sq + real;
            }
#endif
            output[i*horizsteps+j] = iters;
        }
    }
}

void intrinsicsMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, signed maxiters) {
    
#ifdef __SSE2__
#ifndef __AVX__
// Use SSE2 intrinsics

    /* unsigned integer comparisons not available in SSE2 */

#ifdef MULTITHREADED
#pragma omp parallel for schedule(dynamic)
#endif
    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < horizsteps; j+=4) {
            __m128 reals = _mm_set1_ps(startReal + step*j);
            __m128 deltas = _mm_set_ps(0.0f, step, 2.0f*step, 3.0f*step);
            reals = _mm_add_ps(reals, deltas);
            __m128 imags = _mm_set1_ps(startImag - step*i);
            __m128i iters = _mm_setzero_si128();
            __m128i maxIters = _mm_set1_epi32(maxiters);
            __m128 z_reals = _mm_setzero_ps();
            __m128 z_imags = _mm_setzero_ps();
            __m128 z_sums = _mm_setzero_ps();
            __m128i cmp_val = _mm_set1_epi32(-1);
            __m128 fours = _mm_set1_ps(4.0f);
            __m128 twos = _mm_set1_ps(2.0f);
            __m128 negativeFours = _mm_set1_ps(-4.0f);
#if UNROLL_FACTOR > 1
            __m128i iters_old = _mm_setzero_si128();
            __m128 z_reals_old, z_imags_old, z_sums_old;
            __m128i cmp_val_old;
            __m128i unroll_factors = _mm_set1_epi32(UNROLL_FACTOR);
#endif
            while ( _mm_movemask_epi8(cmp_val)  &&  !_mm_movemask_epi8(_mm_cmpeq_epi32(iters, maxIters)) ) {
#if UNROLL_FACTOR > 1
                iters_old = iters;
                z_reals_old = z_reals;
                z_imags_old = z_imags;
                z_sums_old = z_sums;
                cmp_val_old = cmp_val;
#endif

#if UNROLL_FACTOR == 1
                /* iters++ */ iters = _mm_sub_epi32(iters, cmp_val);
#else
                /* iters += UNROLL_FACTOR */
                __m128i masked_unroll = _mm_and_si128(unroll_factors, cmp_val);   // mask some entries to 0
                iters = _mm_add_epi32(iters, masked_unroll);
#endif

                __m128 z_real_sq = _mm_mul_ps(z_reals, z_reals);
                __m128 z_imag_sq = _mm_mul_ps(z_imags, z_imags);
                z_sums = _mm_add_ps(z_real_sq, z_imag_sq);
                z_imags = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(twos, z_reals), z_imags), imags);
                z_reals = _mm_add_ps(_mm_sub_ps(z_real_sq, z_imag_sq), reals);
             
#define EXTRA_ITERATION_SSE2 \
z_real_sq = _mm_mul_ps(z_reals, z_reals); \
z_imag_sq = _mm_mul_ps(z_imags, z_imags); \
z_sums = _mm_add_ps(z_real_sq, z_imag_sq); \
z_imags = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(twos, z_reals), z_imags), imags); \
z_reals = _mm_add_ps(_mm_sub_ps(z_real_sq, z_imag_sq), reals);

#if UNROLL_FACTOR >= 2
                EXTRA_ITERATION_SSE2
#if UNROLL_FACTOR >= 4
                EXTRA_ITERATION_SSE2
                EXTRA_ITERATION_SSE2
#if UNROLL_FACTOR >= 8
                EXTRA_ITERATION_SSE2
                EXTRA_ITERATION_SSE2
                EXTRA_ITERATION_SSE2
                EXTRA_ITERATION_SSE2
#endif
#endif
#endif

                __m128i isLessThan4 = _mm_castps_si128(_mm_cmplt_ps(z_sums, fours));
                __m128i isGtrThanNeg4 = _mm_castps_si128(_mm_cmpgt_ps(z_sums, negativeFours));
                cmp_val = _mm_and_si128(isLessThan4, isGtrThanNeg4);
            }

#if UNROLL_FACTOR > 1
            iters = iters_old;
            z_reals = z_reals_old;
            z_imags = z_imags_old;
            z_sums = z_sums_old;
            cmp_val = cmp_val_old;
            while ( _mm_movemask_epi8(cmp_val) && !_mm_movemask_epi8(_mm_cmpeq_epi32(iters, maxIters)) ) {
                /* iters++ */ iters = _mm_sub_epi32(iters, cmp_val);
                __m128 z_real_sq = _mm_mul_ps(z_reals, z_reals);
                __m128 z_imag_sq = _mm_mul_ps(z_imags, z_imags);
                z_sums = _mm_add_ps(z_real_sq, z_imag_sq);
                z_imags = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(twos, z_reals), z_imags), imags);
                z_reals = _mm_add_ps(_mm_sub_ps(z_real_sq, z_imag_sq), reals);
               
                __m128i isLessThan4 = _mm_castps_si128(_mm_cmplt_ps(z_sums, fours));
                __m128i isGtrThanNeg4 = _mm_castps_si128(_mm_cmpgt_ps(z_sums, negativeFours));
                cmp_val = _mm_and_si128(isLessThan4, isGtrThanNeg4);
            }
            #endif

            int startIndex = i*horizsteps+j;    
            output[startIndex++] = ((unsigned*)(&iters))[3];
            output[startIndex++] = ((unsigned*)(&iters))[2];
            output[startIndex++] = ((unsigned*)(&iters))[1];
            output[startIndex++] = ((unsigned*)(&iters))[0]; 
        }
    }

#else
// Use AVX intrinsics

    float maxitersfloat = (float)maxiters;
#ifdef MULTITHREADED
#pragma omp parallel for schedule(dynamic)
#endif
    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < horizsteps; j+=8) {
            __m256 reals = _mm256_set1_ps(startReal + step*j);
            __m256 deltas = _mm256_set_ps(0.0f, step, 2.0f*step, 3.0f*step, 4.0f*step, 5.0f*step, 6.0f*step, 7.0f*step);
            reals = _mm256_add_ps(reals, deltas);
            __m256 imags = _mm256_set1_ps(startImag - step*i);
            __m256i iters = _mm256_setzero_si256();
            __m256 maxIters = _mm256_set1_ps(maxitersfloat);
            __m256 z_reals = _mm256_setzero_ps();
            __m256 z_imags = _mm256_setzero_ps();
            __m256 z_sums = _mm256_setzero_ps();
            __m256i cmp_val = _mm256_set1_epi32(-1);
            __m256 fours = _mm256_set1_ps(4.0f);
            __m256 twos = _mm256_set1_ps(2.0f);
            __m256 negativeFours = _mm256_set1_ps(-4.0f);
#if UNROLL_FACTOR > 1
            __m256i iters_old = _mm256_setzero_si256();
            __m256 z_reals_old, z_imags_old, z_sums_old;
            __m256i cmp_val_old;
            __m256i unroll_factors = _mm256_set1_epi32(UNROLL_FACTOR);
#endif
            while ( _mm256_movemask_ps(_mm256_castsi256_ps(cmp_val)) && !_mm256_movemask_ps(_mm256_cmp_ps(_mm256_cvtepi32_ps(iters), maxIters, _CMP_GE_OQ)) ) { 
#if UNROLL_FACTOR > 1
                iters_old = iters;
                z_reals_old = z_reals;
                z_imags_old = z_imags;
                z_sums_old = z_sums;
                cmp_val_old = cmp_val;
#endif
                
                // incrementing iters performed by using integer ops separately on the halves of iters
                __m128i itershalf0 = _mm256_extractf128_si256(iters, 0);  // half of iters
                __m128i itershalf1 = _mm256_extractf128_si256(iters, 1);  // other half
                
#if UNROLL_FACTOR == 1
                // iters++
                __m128i cmpvalhalf0 = _mm256_extractf128_si256(cmp_val, 0);
                __m128i cmpvalhalf1 = _mm256_extractf128_si256(cmp_val, 1);
                itershalf0 = _mm_sub_epi32(itershalf0, cmpvalhalf0);
                itershalf1 = _mm_sub_epi32(itershalf1, cmpvalhalf1);
#else
                // iters += UNROLL_FACTOR
                __m256i masked_unroll = _mm256_castps_si256(_mm256_and_ps(_mm256_castsi256_ps(unroll_factors), _mm256_castsi256_ps(cmp_val)));
                __m128i maskedunrollhalf0 = _mm256_extractf128_si256(masked_unroll, 0);
                __m128i maskedunrollhalf1 = _mm256_extractf128_si256(masked_unroll, 1);
                itershalf0 = _mm_add_epi32(itershalf0, maskedunrollhalf0);
                itershalf1 = _mm_add_epi32(itershalf1, maskedunrollhalf1);
#endif

                // reassemble iters
                iters = _mm256_insertf128_si256(iters, itershalf0, 0);
                iters = _mm256_insertf128_si256(iters, itershalf1, 1);    
                
                __m256 z_real_sq = _mm256_mul_ps(z_reals, z_reals);
                __m256 z_imag_sq = _mm256_mul_ps(z_imags, z_imags);
                z_sums = _mm256_add_ps(z_real_sq, z_imag_sq);
                z_imags = _mm256_add_ps(_mm256_mul_ps(_mm256_mul_ps(twos, z_reals), z_imags), imags); \
                z_reals = _mm256_add_ps(_mm256_sub_ps(z_real_sq, z_imag_sq), reals);

#define EXTRA_ITERATION_AVX \
z_real_sq = _mm256_mul_ps(z_reals, z_reals); \
z_imag_sq = _mm256_mul_ps(z_imags, z_imags); \
z_sums = _mm256_add_ps(z_real_sq, z_imag_sq); \
z_imags = _mm256_add_ps(_mm256_mul_ps(_mm256_mul_ps(twos, z_reals), z_imags), imags); \
z_reals = _mm256_add_ps(_mm256_sub_ps(z_real_sq, z_imag_sq), reals);

#if UNROLL_FACTOR >= 2
                EXTRA_ITERATION_AVX
#if UNROLL_FACTOR >= 4
                EXTRA_ITERATION_AVX
                EXTRA_ITERATION_AVX
#if UNROLL_FACTOR >= 8
                EXTRA_ITERATION_AVX
                EXTRA_ITERATION_AVX
                EXTRA_ITERATION_AVX
                EXTRA_ITERATION_AVX
#endif
#endif
#endif
                
                __m256 isLessThan4 = _mm256_cmp_ps(z_sums, fours, _CMP_LT_OQ);
                __m256 isGtrThanNeg4 = _mm256_cmp_ps(z_sums, negativeFours, _CMP_GT_OQ);
                cmp_val = _mm256_castps_si256(_mm256_and_ps(isLessThan4, isGtrThanNeg4));
            }

#if UNROLL_FACTOR > 1
            iters = iters_old;
            z_reals = z_reals_old;
            z_imags = z_imags_old;
            z_sums = z_sums_old;
            cmp_val = cmp_val_old;
            while ( _mm256_movemask_ps(_mm256_castsi256_ps(cmp_val)) && !_mm256_movemask_ps(_mm256_cmp_ps(_mm256_cvtepi32_ps(iters), maxIters, _CMP_GE_OQ)) ) { 
                __m128i itershalf0 = _mm256_extractf128_si256(iters, 0);  // half of iters
                __m128i itershalf1 = _mm256_extractf128_si256(iters, 1);  // other half
                // iters++
                __m128i cmpvalhalf0 = _mm256_extractf128_si256(cmp_val, 0);
                __m128i cmpvalhalf1 = _mm256_extractf128_si256(cmp_val, 1);
                itershalf0 = _mm_sub_epi32(itershalf0, cmpvalhalf0);
                itershalf1 = _mm_sub_epi32(itershalf1, cmpvalhalf1);
                // reassemble iters
                iters = _mm256_insertf128_si256(iters, itershalf0, 0);
                iters = _mm256_insertf128_si256(iters, itershalf1, 1);    
                
                __m256 z_real_sq = _mm256_mul_ps(z_reals, z_reals);
                __m256 z_imag_sq = _mm256_mul_ps(z_imags, z_imags);
                z_sums = _mm256_add_ps(z_real_sq, z_imag_sq);
                z_imags = _mm256_add_ps(_mm256_mul_ps(_mm256_mul_ps(twos, z_reals), z_imags), imags); \
                z_reals = _mm256_add_ps(_mm256_sub_ps(z_real_sq, z_imag_sq), reals);

                __m256 isLessThan4 = _mm256_cmp_ps(z_sums, fours, _CMP_LT_OQ);
                __m256 isGtrThanNeg4 = _mm256_cmp_ps(z_sums, negativeFours, _CMP_GT_OQ);
                cmp_val = _mm256_castps_si256(_mm256_and_ps(isLessThan4, isGtrThanNeg4));
            }
#endif
            
            int startIndex = i*horizsteps+j;    
            output[startIndex++] = ((unsigned*)(&iters))[7];
            output[startIndex++] = ((unsigned*)(&iters))[6];
            output[startIndex++] = ((unsigned*)(&iters))[5];
            output[startIndex++] = ((unsigned*)(&iters))[4];
            output[startIndex++] = ((unsigned*)(&iters))[3];
            output[startIndex++] = ((unsigned*)(&iters))[2];
            output[startIndex++] = ((unsigned*)(&iters))[1];
            output[startIndex++] = ((unsigned*)(&iters))[0]; 
        }
    }

#endif
#else  // #ifndef __SSE2__
#ifdef __MIC__
// Use Phi intrinsics

#ifdef MULTITHREADED
#pragma omp parallel for schedule(dynamic)
#endif
    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < horizsteps; j+=16) {
            __m512 reals = _mm512_set1_ps(startReal + step*j);
            __m512 deltas = _mm512_set_ps(0.0f, step, 2.0f*step, 3.0f*step, 4.0f*step, 5.0f*step, 6.0f*step, 7.0f*step, 8.0f*step, 9.0f*step, 10.0f*step, 11.0f*step, 12.0f*step, 13.0f*step, 14.0f*step, 15.0f*step);
            reals = _mm512_add_ps(reals, deltas);
            __m512 imags = _mm512_set1_ps(startImag - step*i);
            __m512i iters = _mm512_setzero_epi32();
            __m512i maxIters = _mm512_set1_epi32(maxiters);
            __m512 z_reals = _mm512_setzero_ps();
            __m512 z_imags = _mm512_setzero_ps();
            __m512 z_sums = _mm512_setzero_ps();
            __mmask16 cmp_val = _mm512_int2mask(-1);
            __m512 fours = _mm512_set1_ps(4.0f);
            __m512 twos = _mm512_set1_ps(2.0f);
            __m512i ones = _mm512_set1_epi32(1);
            __m512i unroll_factors = _mm512_set1_epi32(UNROLL_FACTOR);
#if UNROLL_FACTOR > 1
            __m512i iters_old = _mm512_setzero_epi32();
            __m512 z_reals_old, z_imags_old, z_sums_old;
#endif
            while ( (cmp_val)  &&  !(_mm512_cmpeq_epi32_mask(iters, maxIters)) ) {
#if UNROLL_FACTOR > 1
                iters_old = iters;
                z_reals_old = z_reals;
                z_imags_old = z_imags;
                z_sums_old = z_sums;
#endif
                
                /* iters += UNROLL_FACTOR */ 
                // Since masking is free on Phi, we can go without the #if we used for SSE2 and AVX here
                iters = _mm512_mask_add_epi32(iters, cmp_val, iters, unroll_factors);

                __m512 z_real_sq = _mm512_mul_ps(z_reals, z_reals);
                __m512 z_imag_sq = _mm512_mul_ps(z_imags, z_imags);
                z_sums = _mm512_mask_add_ps(z_sums, cmp_val, z_real_sq, z_imag_sq);
                z_imags = _mm512_fmadd_ps(_mm512_mul_ps(twos, z_reals), z_imags, imags);
                z_reals = _mm512_add_ps(_mm512_sub_ps(z_real_sq, z_imag_sq), reals);
               
#define EXTRA_ITERATION_MIC \
z_real_sq = _mm512_mul_ps(z_reals, z_reals); \
z_imag_sq = _mm512_mul_ps(z_imags, z_imags); \
z_sums = _mm512_mask_add_ps(z_sums, cmp_val, z_real_sq, z_imag_sq); \
z_imags = _mm512_fmadd_ps(_mm512_mul_ps(twos, z_reals), z_imags, imags); \
z_reals = _mm512_add_ps(_mm512_sub_ps(z_real_sq, z_imag_sq), reals);

#if UNROLL_FACTOR >= 2
                EXTRA_ITERATION_MIC
#if UNROLL_FACTOR >= 4
                EXTRA_ITERATION_MIC
                EXTRA_ITERATION_MIC
#if UNROLL_FACTOR >= 8
                EXTRA_ITERATION_MIC
                EXTRA_ITERATION_MIC
                EXTRA_ITERATION_MIC
                EXTRA_ITERATION_MIC
#endif
#endif
#endif

                // Option 1
                // __mmask16 isLessThan4 = _mm512_cmplt_ps_mask(z_sums, fours);
                // nle: not less than or equal (i.e. greater than)
                // __mmask16 isGtrThanNeg4 = _mm512_cmpnle_ps_mask(z_sums, negativeFours); 
                // cmp_val = _mm512_kand(isLessThan4, isGtrThanNeg4);
                
                // Option 2
                // Only the <4 comparison is needed because we are doing masked adds to z_sums 
                // (in Phi architecture masked add is same cost as add)
                cmp_val = _mm512_cmplt_ps_mask(z_sums, fours);
               
            }

#if UNROLL_FACTOR > 1
            iters = iters_old;
            z_reals = z_reals_old;
            z_imags = z_imags_old;
            z_sums = z_sums_old;
            while ( (cmp_val)  &&  !(_mm512_cmpeq_epi32_mask(iters, maxIters)) ) {
                /* iters++ */ iters = _mm512_mask_add_epi32(iters, cmp_val, iters, ones);
                __m512 z_real_sq = _mm512_mul_ps(z_reals, z_reals);
                __m512 z_imag_sq = _mm512_mul_ps(z_imags, z_imags);
                z_sums = _mm512_mask_add_ps(z_sums, cmp_val, z_real_sq, z_imag_sq);
                z_imags = _mm512_fmadd_ps(_mm512_mul_ps(twos, z_reals), z_imags, imags);
                z_reals = _mm512_add_ps(_mm512_sub_ps(z_real_sq, z_imag_sq), reals);
                cmp_val = _mm512_cmplt_ps_mask(z_sums, fours);
            }
#endif

            int index = i*horizsteps+j;    
            output[index++] = ((unsigned*)(&iters))[15];
            output[index++] = ((unsigned*)(&iters))[14];
            output[index++] = ((unsigned*)(&iters))[13];
            output[index++] = ((unsigned*)(&iters))[12];
            output[index++] = ((unsigned*)(&iters))[11];
            output[index++] = ((unsigned*)(&iters))[10];
            output[index++] = ((unsigned*)(&iters))[9];
            output[index++] = ((unsigned*)(&iters))[8];
            output[index++] = ((unsigned*)(&iters))[7];
            output[index++] = ((unsigned*)(&iters))[6];
            output[index++] = ((unsigned*)(&iters))[5];
            output[index++] = ((unsigned*)(&iters))[4];
            output[index++] = ((unsigned*)(&iters))[3];
            output[index++] = ((unsigned*)(&iters))[2];
            output[index++] = ((unsigned*)(&iters))[1];
            output[index++] = ((unsigned*)(&iters))[0];
        }
    }

#endif   // #ifdef __MIC__
#endif   // #ifdef __SSE2__

}  // end function intrinsicsMandel

void interchangeMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned * output, unsigned maxiters) {
#ifdef __SSE2__
#ifndef __AVX__
/* SSE2 */
#define VECTOR_WIDTH 4
#else  // #ifdef __AVX__
/* AVX */
#define VECTOR_WIDTH 8
#endif
#else  // #ifndef __SSE2__
#ifdef __MIC__
#define VECTOR_WIDTH 16
#endif // __MIC__
#endif // __SSE2__


#define INITIALIZE(type, varname, value) \
    type varname[VECTOR_WIDTH]; \
    for (int index = 0; index < VECTOR_WIDTH; index++ ) \
        varname[index] = value;

#ifdef MULTITHREADED
#pragma omp parallel for schedule(dynamic)
#endif
    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < horizsteps; j += VECTOR_WIDTH) {
            INITIALIZE(float, real, startReal + step*(j+index))
            const float imagcoord = startImag - step*i;   // the value for all lanes of the vector
            INITIALIZE(float, imag, imagcoord)
            INITIALIZE(unsigned, iters, 0)
            INITIALIZE(float, z_real, 0.0f)
            INITIALIZE(float, z_imag, 0.0f)
            INITIALIZE(bool, on, true)
            bool any_on = true;
#if UNROLL_FACTOR > 1
            INITIALIZE(unsigned, iters_old, 0)
            float z_real_old[VECTOR_WIDTH], z_imag_old[VECTOR_WIDTH], z_sum_old[VECTOR_WIDTH];
            bool on_old[VECTOR_WIDTH];
#endif
            while (any_on) {
                any_on = false;
#pragma vector always    // tell Intel compiler to vectorize even if it seems inefficient
                for (int index = 0; index < VECTOR_WIDTH; index++) {
                    if (on[index]) {
#if UNROLL_FACTOR > 1
                        iters_old[index] = iters[index];
                        z_real_old[index] = z_real[index];
                        z_imag_old[index] = z_imag[index];
                        on_old[index] = true;
#endif

                        iters[index] += UNROLL_FACTOR;
                        
                        float z_real_sq = z_real[index]*z_real[index];
                        float z_imag_sq = z_imag[index]*z_imag[index];
                        float z_sum = z_real_sq + z_imag_sq;
                        z_imag[index] = 2.0f*z_real[index]*z_imag[index] + imag[index];
                        z_real[index] = z_real_sq - z_imag_sq + real[index];
                        
#define EXTRA_ITERATION_INTEL \
z_real_sq = z_real[index]*z_real[index]; \
z_imag_sq = z_imag[index]*z_imag[index]; \
z_sum = z_real_sq + z_imag_sq; \
z_imag[index] = 2.0f*z_real[index]*z_imag[index] + imag[index]; \
z_real[index] = z_real_sq - z_imag_sq + real[index];

#if UNROLL_FACTOR >= 2
                        EXTRA_ITERATION_INTEL
#if UNROLL_FACTOR >= 4
                        EXTRA_ITERATION_INTEL
                        EXTRA_ITERATION_INTEL
#if UNROLL_FACTOR >= 8
                        EXTRA_ITERATION_INTEL
                        EXTRA_ITERATION_INTEL
                        EXTRA_ITERATION_INTEL
                        EXTRA_ITERATION_INTEL
#endif
#endif
#endif
                        if (z_sum < 4.0f && iters[index] < maxiters)
                           any_on = true;
                        else 
                           on[index] = false;
                    } else {   // was not on[index]
#if UNROLL_FACTOR > 1
                        on_old[index] = false;
#endif
                    }
                }
            }
            
#if UNROLL_FACTOR > 1
            for (int index = 0; index < VECTOR_WIDTH; index++) {
                iters[index] = iters_old[index];
                z_real[index] = z_real_old[index];
                z_imag[index] = z_imag_old[index];
                on[index] = on_old[index];
            }
            any_on = true;
            while (any_on) {
                any_on = false;
#pragma vector always    // tell Intel compiler to vectorize even if it seems inefficient
                for (int index = 0; index < VECTOR_WIDTH; index++) {
                    if (on[index]) {
                        iters[index]++;
                        float z_real_sq = z_real[index]*z_real[index];
                        float z_imag_sq = z_imag[index]*z_imag[index];
                        float z_sum = z_real_sq + z_imag_sq;
                        z_imag[index] = 2.0f*z_real[index]*z_imag[index] + imag[index];
                        z_real[index] = z_real_sq - z_imag_sq + real[index];
                        if (z_sum < 4.0f && iters[index] < maxiters)
                           any_on = true;
                        else 
                           on[index] = false;
                    }
                }
            } 
#endif

            for (int index = 0; index < VECTOR_WIDTH; index++)
                output[i*horizsteps+j+index] = iters[index];
        }
    }
}

void ompMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned * output, unsigned maxiters) {
#ifdef __SSE2__
#ifndef __AVX__
/* SSE2 */
#define VECTOR_WIDTH 4
#else  // #ifdef __AVX__
/* AVX */
#define VECTOR_WIDTH 8
#endif
#else  // #ifndef __SSE2__
#ifdef __MIC__
#define VECTOR_WIDTH 16
#endif // __MIC__
#endif // __SSE2__


#define INITIALIZE(type, varname, value) \
    type varname[VECTOR_WIDTH]; \
    for (int index = 0; index < VECTOR_WIDTH; index++ ) \
        varname[index] = value;

#ifdef MULTITHREADED
#pragma omp parallel for schedule(dynamic)
#endif
    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < horizsteps; j += VECTOR_WIDTH) {
            INITIALIZE(float, real, startReal + step*(j+index))
            const float imagcoord = startImag - step*i;   // the value for all lanes of the vector
            INITIALIZE(float, imag, imagcoord)
            INITIALIZE(unsigned, iters, 0)
            INITIALIZE(float, z_real, 0.0f)
            INITIALIZE(float, z_imag, 0.0f)
            INITIALIZE(bool, on, true)
            bool any_on = true;
#if UNROLL_FACTOR > 1
            INITIALIZE(unsigned, iters_old, 0)
            float z_real_old[VECTOR_WIDTH], z_imag_old[VECTOR_WIDTH], z_sum_old[VECTOR_WIDTH];
            bool on_old[VECTOR_WIDTH];
#endif
            while (any_on) {
                any_on = false;
#pragma omp simd
                for (int index = 0; index < VECTOR_WIDTH; index++) {
                    if (on[index]) {
#if UNROLL_FACTOR > 1
                        iters_old[index] = iters[index];
                        z_real_old[index] = z_real[index];
                        z_imag_old[index] = z_imag[index];
                        on_old[index] = true;
#endif

                        iters[index] += UNROLL_FACTOR;
                        
                        float z_real_sq = z_real[index]*z_real[index];
                        float z_imag_sq = z_imag[index]*z_imag[index];
                        float z_sum = z_real_sq + z_imag_sq;
                        z_imag[index] = 2.0f*z_real[index]*z_imag[index] + imag[index];
                        z_real[index] = z_real_sq - z_imag_sq + real[index];
                        
#define EXTRA_ITERATION_OMP \
z_real_sq = z_real[index]*z_real[index]; \
z_imag_sq = z_imag[index]*z_imag[index]; \
z_sum = z_real_sq + z_imag_sq; \
z_imag[index] = 2.0f*z_real[index]*z_imag[index] + imag[index]; \
z_real[index] = z_real_sq - z_imag_sq + real[index];

#if UNROLL_FACTOR >= 2
                        EXTRA_ITERATION_OMP
#if UNROLL_FACTOR >= 4
                        EXTRA_ITERATION_OMP
                        EXTRA_ITERATION_OMP
#if UNROLL_FACTOR >= 8
                        EXTRA_ITERATION_OMP
                        EXTRA_ITERATION_OMP
                        EXTRA_ITERATION_OMP
                        EXTRA_ITERATION_OMP
#endif
#endif
#endif
                        if (z_sum < 4.0f && iters[index] < maxiters)
                           any_on = true;
                        else 
                           on[index] = false;
                    } else {   // was not on[index]
#if UNROLL_FACTOR > 1
                        on_old[index] = false;
#endif
                    }
                }
            }
            
#if UNROLL_FACTOR > 1
            for (int index = 0; index < VECTOR_WIDTH; index++) {
                iters[index] = iters_old[index];
                z_real[index] = z_real_old[index];
                z_imag[index] = z_imag_old[index];
                on[index] = on_old[index];
            }
            any_on = true;
            while (any_on) {
                any_on = false;
#pragma omp simd
                for (int index = 0; index < VECTOR_WIDTH; index++) {
                    if (on[index]) {
                        iters[index]++;
                        float z_real_sq = z_real[index]*z_real[index];
                        float z_imag_sq = z_imag[index]*z_imag[index];
                        float z_sum = z_real_sq + z_imag_sq;
                        z_imag[index] = 2.0f*z_real[index]*z_imag[index] + imag[index];
                        z_real[index] = z_real_sq - z_imag_sq + real[index];
                        if (z_sum < 4.0f && iters[index] < maxiters)
                           any_on = true;
                        else 
                           on[index] = false;
                    }
                }
            } 
#endif

            for (int index = 0; index < VECTOR_WIDTH; index++)
                output[i*horizsteps+j+index] = iters[index];
        }
    }
}

void ispcMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned * output, unsigned maxiters) {
#ifdef MULTITHREADED
#pragma omp parallel for schedule(dynamic)
#endif
    for(int i = 0; i < steps; i++) {
        ispcMandel_inner(startReal, startImag, i, horizsteps, step, output, maxiters);
    }
}


