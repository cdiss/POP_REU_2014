#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#ifdef __SSE2__
#include "xmmintrin.h"
#endif
#ifdef __AVX__
#include "immintrin.h"
#endif
#include "WKFUtils.h"
#include "omp.h"
#include "cudaFloat.h"

bool testCorrect(unsigned *output, int size) {
    FILE* f = fopen("reference.out", "r");
    if(!f) {printf("Could not open reference.out"); return false;}
    for(int i = 0; i < size; i++) {
        unsigned ref;
        fscanf(f, "%u", &ref);
        if(ref != output[i]) return false;        
    }
    return true;
}

void genReference(unsigned *output, int size) {
    FILE* f = fopen("reference.out", "w");
    for(int i = 0; i < size; i++) {
        fprintf(f, "%u ", output[i]);
    }
}

#ifdef __SSE2__
static int hor_m128i(__m128i mask4) {
    __m128i tmp = _mm_shuffle_epi32(mask4, _MM_SHUFFLE(2, 3, 0, 1));
    tmp = _mm_or_si128(mask4, tmp);
    mask4 = tmp;
    tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(1, 0, 3, 2));
    mask4 = _mm_or_si128(mask4, tmp);   // all elements now equal to the hor

    int mask = _mm_cvtsi128_si32(mask4); // return zeroth element
    return mask;
}
#endif

#ifdef __AVX__
//adapted from function found in StackOverflow article "How to sum __m256 horizontally"
static bool hor_m256(__m256 mask8) {
    __m128 half0 = _mm256_extractf128_ps(mask8, 0);
    __m128 half1 = _mm256_extractf128_ps(mask8, 1);
    __m128 orQuad = _mm_or_ps(half0, half1);
    // sum holds 3+7, 2+6, 1+5, 0+4
    __m128 loDual = orQuad;
    __m128 hiDual = _mm_movehl_ps(orQuad, orQuad);
    __m128 orDual = _mm_or_ps(loDual, hiDual);
    __m128 lo = orDual;
    __m128 hi = _mm_shuffle_ps(orDual, orDual, 0x1);
    __m128 hor = _mm_or_ps(lo, hi);
    return !(_mm_cvtss_f32(hor) == 0);
}
#endif

#ifdef __SSE2__
void debugPrint_m128i(__m128i reg) {
    printf("%i %i %i %i\n", ((int*)(&reg))[3], ((int*)(&reg))[2], ((int*)(&reg))[1], ((int*)(&reg))[0]);
}

void debugPrint_m128(__m128 reg) {
    printf("%f %f %f %f\n", ((float*)(&reg))[3],  ((float*)(&reg))[2],  ((float*)(&reg))[1],  ((float*)(&reg))[0]); 
}
#endif

#ifdef __AVX__
void debugPrint_m256(__m256 reg) {
    printf("%f %f %f %f %f %f %f %f\n", ((float*)(&reg))[7], ((float*)(&reg))[6], ((float*)(&reg))[5], ((float*)(&reg))[4], ((float*)(&reg))[3], ((float*)(&reg))[2], ((float*)(&reg))[1], ((float*)(&reg))[0]);
} 
#endif

void serialDouble(double startReal, double startImag, int steps, int horizsteps, double step, unsigned* output, unsigned maxIters) {
 
    unsigned oindex = 0;
    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < horizsteps; j++) {
            double real = startReal + step*j;
            double imag = startImag - step*i;
            unsigned iters = 0;
            double z_real = 0.0, z_imag = 0.0, z_sum = 0.0;
            while (z_sum < 4.0f && iters < maxIters) {
                iters++;
                double z_real_sq = z_real*z_real;
                double z_imag_sq = z_imag*z_imag;
                z_sum = z_real_sq + z_imag_sq;
                z_imag = 2.0*z_real*z_imag + imag;
                z_real = z_real_sq - z_imag_sq + real;
            } 
            output[oindex++] = iters;
        }
    }
}

void serialFloat(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxIters) {
    
    unsigned oindex = 0;
    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < horizsteps; j++) {
            float real = startReal + step*j;
            float imag = startImag - step*i;
            unsigned iters = 0;
            float z_real = 0.0f, z_imag = 0.0f, z_sum = 0.0f;
            while (z_sum < 4.0f && iters < maxIters) {
                iters++;
                float z_real_sq = z_real*z_real;
                float z_imag_sq = z_imag*z_imag;
                z_sum = z_real_sq + z_imag_sq;
                z_imag = 2.0f*z_real*z_imag + imag;
                z_real = z_real_sq - z_imag_sq + real;
            } 
            output[oindex++] = iters;
        }
    }
}

#ifdef __SSE2__
void sse2Float(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, signed maxiters) {
    
    /* unsigned integer comparisons not available in SSE2 */

    //#pragma omp parallel for
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
            while ( hor_m128i(cmp_val)  &&  !hor_m128i(_mm_cmpeq_epi32(iters, maxIters)) ) {
                /* iters++ */ iters = _mm_sub_epi32(iters, cmp_val);
                __m128 z_real_sq = _mm_mul_ps(z_reals, z_reals);
                __m128 z_imag_sq = _mm_mul_ps(z_imags, z_imags);
                z_sums = _mm_add_ps(z_real_sq, z_imag_sq);
                __m128i isLessThan4 = _mm_castps_si128(_mm_cmplt_ps(z_sums, fours));
                __m128i isGtrThanNeg4 = _mm_castps_si128(_mm_cmpgt_ps(z_sums, negativeFours));
                cmp_val = _mm_and_si128(isLessThan4, isGtrThanNeg4);
                z_imags = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(twos, z_reals), z_imags), imags);
                z_reals = _mm_add_ps(_mm_sub_ps(z_real_sq, z_imag_sq), reals);
            }
            int startIndex = i*horizsteps+j;    
            //#pragma omp critical
            {
            output[startIndex++] = ((unsigned*)(&iters))[3];
            output[startIndex++] = ((unsigned*)(&iters))[2];
            output[startIndex++] = ((unsigned*)(&iters))[1];
            output[startIndex++] = ((unsigned*)(&iters))[0]; 
            }
        }
    }
}
#endif

#ifdef __AVX__
void avxFloat(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxiters) {

    float maxitersfloat = (float)maxiters;
    //#pragma omp parallel for
    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < horizsteps; j+=8) {
            __m256 reals = _mm256_set1_ps(startReal + step*j);
            __m256 deltas = _mm256_set_ps(0.0f, step, 2.0f*step, 3.0f*step, 4.0f*step, 5.0f*step, 6.0f*step, 7.0f*step);
            reals = _mm256_add_ps(reals, deltas);
            __m256 imags = _mm256_set1_ps(startImag - step*i);
            __m256 iters = _mm256_setzero_ps();
            __m256 maxIters = _mm256_set1_ps(maxitersfloat);
            __m256 z_reals = _mm256_setzero_ps();
            __m256 z_imags = _mm256_setzero_ps();
            __m256 z_sums = _mm256_setzero_ps();
            __m256 cmp_val = _mm256_castsi256_ps(_mm256_set1_epi32(-1));
            __m256 fours = _mm256_set1_ps(4.0f);
            __m256 twos = _mm256_set1_ps(2.0f);
            __m256 negativeFours = _mm256_set1_ps(-4.0f);
            while ( hor_m256(cmp_val)  &&  !hor_m256(_mm256_cmp_ps(iters, maxIters, _CMP_GE_OQ)) ) {
                // iters++ performed by using integer ops separately on the halves of iters
                __m128 itershalf0 = _mm256_extractf128_ps(iters, 0);  // half of iters
                __m128 itershalf1 = _mm256_extractf128_ps(iters, 1);  // other half
                __m128 cmpvalhalf0 = _mm256_extractf128_ps(cmp_val, 0);
                __m128 cmpvalhalf1 = _mm256_extractf128_ps(cmp_val, 1);
                itershalf0 = _mm_cvtepi32_ps(_mm_sub_epi32(_mm_cvtps_epi32(itershalf0), _mm_castps_si128(cmpvalhalf0)));
                itershalf1 = _mm_cvtepi32_ps(_mm_sub_epi32(_mm_cvtps_epi32(itershalf1), _mm_castps_si128(cmpvalhalf1)));
                iters = _mm256_insertf128_ps(iters, itershalf0, 0); // reassemble iters
                iters = _mm256_insertf128_ps(iters, itershalf1, 1);    
                __m256 z_real_sq = _mm256_mul_ps(z_reals, z_reals);
                __m256 z_imag_sq = _mm256_mul_ps(z_imags, z_imags);
                z_sums = _mm256_add_ps(z_real_sq, z_imag_sq);
                __m256 isLessThan4 = _mm256_cmp_ps(z_sums, fours, _CMP_LT_OQ);
                __m256 isGtrThanNeg4 = _mm256_cmp_ps(z_sums, negativeFours, _CMP_GT_OQ);
                cmp_val = _mm256_and_ps(isLessThan4, isGtrThanNeg4);
                z_imags = _mm256_add_ps(_mm256_mul_ps(_mm256_mul_ps(twos, z_reals), z_imags), imags);
                z_reals = _mm256_add_ps(_mm256_sub_ps(z_real_sq, z_imag_sq), reals);
            }
            int startIndex = i*horizsteps+j;    
            //#pragma omp critical
            {
            output[startIndex++] = (unsigned)( ((float*)(&iters))[7] );
            output[startIndex++] = (unsigned)( ((float*)(&iters))[6] );
            output[startIndex++] = (unsigned)( ((float*)(&iters))[5] );
            output[startIndex++] = (unsigned)( ((float*)(&iters))[4] );
            output[startIndex++] = (unsigned)( ((float*)(&iters))[3] );
            output[startIndex++] = (unsigned)( ((float*)(&iters))[2] );
            output[startIndex++] = (unsigned)( ((float*)(&iters))[1] );
            output[startIndex++] = (unsigned)( ((float*)(&iters))[0] ); 
            }
        }
    }
}
#endif

void printUsage() {
    printf("Usage: mandelbrot serial|SSE2|AVX|CUDA -c centerReal centerImag -s size -d steps -f filename -b\n");
}

int main(int argc, char* argv[]) {
    float centerReal = 0.0f, centerImag = 0.0f;
    float size = 4.0;
    int steps = 864;
    char filename[50] = "mandelbrot";
    bool bench = false;

    if (argc < 2) {
      printUsage();
      return 0;
    }

    // parse serial|SSE2||AVX|CUDA
    bool serial = false;
    bool sse2 = false;
    bool avx = false;
    bool cuda = false;
    if (!strcmp(argv[1], "serial\0")) serial = true;
    else if (!strcmp(argv[1], "SSE2\0")) sse2 = true;
    else if (!strcmp(argv[1], "AVX\0")) avx = true;
    else if (!strcmp(argv[1], "CUDA\0")) cuda = true;
    else {
      printUsage();
      return 0;
    }

    int counter = 2;
    while ( counter < argc ) {
        if (!strcmp(argv[counter], "-c\0") && argc >= (counter + 3)) {
            sscanf(argv[++counter], "%f", &centerReal);
            sscanf(argv[++counter], "%f", &centerImag);
            counter++;
        } else if (!strcmp(argv[counter], "-s\0") && argc >= (counter + 2)) {
            sscanf(argv[++counter], "%f", &size);
            counter++;
        } else if (!strcmp(argv[counter], "-d\0") && argc >= (counter + 2)) {
            sscanf(argv[++counter], "%i", &steps);
            counter++;
        } else if (!strcmp(argv[counter], "-f\0") && argc >= (counter + 2)) {
            sscanf(argv[++counter], "%s", filename);
            counter++;
        } else if (!strcmp(argv[counter], "-b\0")) {
            bench = true;
            counter++;
        } else {
            printUsage();
            return 0;
        }
    }
    
    if (sse2)
        // make steps divisible by 4
        while (steps % 4 != 0) steps++;
    else if (avx)
        // make steps divisible by 8
        while (steps % 8 != 0) steps++;
    else if (cuda)
        // make steps divisible by 16
        while (steps % 16 != 0) steps++;
    
    int horizsteps = steps*16/9;
    if (horizsteps % 16 != 0) {
        printf("Use one of these numbers of steps: 576, 864, 1152, 1440, 1728, 2304\n");
        printf("steps: %i, horizsteps: %i\n", steps, horizsteps);
        return 0;
    }

    int numPixels = steps*horizsteps;
    unsigned* output = (unsigned*)malloc(numPixels*sizeof(unsigned));
    
    int NUM_FRAMES = 3000;
    for (int frame = 0; frame < NUM_FRAMES; frame++, size *= 0.97) {

    float horizsize = size*16.0f/9.0f;

    wkf_timerhandle timer = wkf_timer_create();


    unsigned maxIters = 20050;
    float startReal = centerReal - (horizsize/2.0f), startImag = centerImag + (size/2.0f);
    float stepsize = size/steps;
    float time;
 
    if (bench) {
        int warmUpRuns = 10;
        for (int i = 0; i < warmUpRuns; i++) {
            serialFloat(0.0f, 0.0f, steps, horizsteps, 0.004f, output, 500);
        }
    }

    if (serial) {

        wkf_timer_start(timer); 
        serialFloat(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
        wkf_timer_stop(timer);
        time = wkf_timer_time(timer);
        if(bench) printf("serialFloat: Time: %f\n", time);

        wkf_timer_start(timer); 
        serialDouble(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
        wkf_timer_stop(timer);
        time = wkf_timer_time(timer);
        if(bench) printf("serialDouble: Time: %f\n", time);

    } else if (sse2) {
    
#ifdef __SSE2__
        wkf_timer_start(timer); 
        sse2Float(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
        wkf_timer_stop(timer);
        time = wkf_timer_time(timer);
        if(bench) printf("sse2Float: Time: %f\n", time);
#endif

    } else if (avx) {
       
#ifdef __AVX__
        wkf_timer_start(timer);
        avxFloat(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
        wkf_timer_stop(timer);
        time = wkf_timer_time(timer);
        if(bench) printf("avxFloat: Time: %f\n", time);
#endif
       
    } else {

        wkf_timer_start(timer);
        cudaDouble(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
        wkf_timer_stop(timer);
        time = wkf_timer_time(timer);
        if(bench) printf("cudaDouble including memory transfer: Time: %f\n", time);

    }

    // performance metrics
    if (bench) {
        uint64_t sum = 0;
        unsigned iters_max = output[0];
        unsigned iters_min = output[0];
        for (int i = 0; i < numPixels; i++) {
            unsigned iters = output[i];
            sum += iters;
            if (iters > iters_max) iters_max = iters;
            if (iters < iters_min) iters_min = iters;
        }
        printf("Performance:\nIters per second: %f\nGFLOPS: %f\nIters per pixel per second: %f\nMaximum iters per pixel: %u\nMinimum iters per pixel: %u\n", sum/time, sum/time*8/1000000000, (sum/numPixels)/time, iters_max, iters_min);
    } 

    #define ITERS_CYCLE 100
    #define COLORMIN 0
    #define COLORMAX 220

    // write picture data
    char realFilename[60];
    sprintf(realFilename, "ppms/%s%i.ppm", filename, frame);
    printf("Writing %s\n", realFilename);
    FILE* f = fopen(realFilename, "w");
    if (!f) { printf("Error opening file\n"); return(0); }
    fprintf(f, "P6\n%i %i\n255\n", horizsteps, steps);    
    unsigned char* buffer = (unsigned char*)malloc(3*numPixels*sizeof(unsigned char));
    int bufferIndex = 0;
    for (int i = 0; i < numPixels; i++) {
        int itersForColor = (maxIters-output[i]) % (ITERS_CYCLE*4);
        char red, green, blue;
        switch(itersForColor/ITERS_CYCLE) {
            case 0: // black to green region
                red = COLORMIN;
                green = COLORMIN + (itersForColor*(COLORMAX-COLORMIN))/ITERS_CYCLE;
                blue = COLORMIN;
                break;               
            case 1: // green to red region
                itersForColor -= ITERS_CYCLE;
                red = COLORMIN + (itersForColor*(COLORMAX-COLORMIN))/ITERS_CYCLE;
                green = COLORMAX - (itersForColor*(COLORMAX-COLORMIN))/ITERS_CYCLE;
                blue = COLORMIN;
                break;
            case 2: // red to blue region
                itersForColor -= (2*ITERS_CYCLE);
                red = COLORMAX - (itersForColor*(COLORMAX-COLORMIN))/ITERS_CYCLE;
                green = COLORMIN;
                blue = COLORMIN + (itersForColor*(COLORMAX-COLORMIN))/ITERS_CYCLE;
                break;
            case 3: // blue to black region
                itersForColor -= (3*ITERS_CYCLE);
                red = COLORMIN; 
                green = COLORMIN;
                blue = COLORMAX - (itersForColor*(COLORMAX-COLORMIN))/ITERS_CYCLE;
                break;
            default: // should never happen
                printf("Error in color mapping\n");
                break;
        }
        buffer[bufferIndex++] = red;
        buffer[bufferIndex++] = green;
        buffer[bufferIndex++] = blue;
    }
     
    fwrite(buffer, sizeof(unsigned char), 3*numPixels, f);

    fclose(f);

    }

    return 0;
}

