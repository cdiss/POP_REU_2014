// compile with: gcc test.C -o test -g -pthread -D UNROLL_FACTOR=8 -flax-vector-conversions -O3 -fexpensive-optimizations -ffast-math -funsafe-math-optimizations -fomit-frame-pointer -fno-strict-aliasing -march=armv7-a -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=vfpv3 -mfpu=neon


#include <arm_neon.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define NUM_THREADS 4

#ifndef UNROLL_FACTOR
#define UNROLL_FACTOR 1
#endif

struct func_data {
	float startReal;
	float startImag;
	int steps;
	int horizsteps;
	float step;
	unsigned * output;
	unsigned maxIters;
};

struct thread_data {
	int threadID;
	func_data * data;
};

int32_t hor_s32(int32x4_t vals) {
	int32x2_t lows = vget_low_s32(vals);
	int32x2_t highs = vget_high_s32(vals);
	int32x2_t ors = vorr_s32(lows, highs);
	int32_t low = vget_lane_s32(ors, 0);
	int32_t high = vget_lane_s32(ors,1);
	return low|high;
}

void debugPrint(int32x4_t vals) {
	printf("%d ", vgetq_lane_s32(vals, 0));
	printf("%d ", vgetq_lane_s32(vals, 1));
	printf("%d ", vgetq_lane_s32(vals, 2));
	printf("%d\n", vgetq_lane_s32(vals, 3));
}

void debugPrint(float32x4_t vals) {
	printf("%f ", vgetq_lane_f32(vals, 0));
	printf("%f ", vgetq_lane_f32(vals, 1));
	printf("%f ", vgetq_lane_f32(vals, 2));
	printf("%f\n", vgetq_lane_f32(vals, 3));
}

void *serialThread(void * threadData) {
	struct thread_data * tdata = (struct thread_data *)threadData;
	int startStep = tdata->threadID*(tdata->data->steps/4);
	int stopStep = startStep + (tdata->data->steps/4);
	int horizsteps = tdata->data->horizsteps;
	float step = tdata->data->step;
	float startReal = tdata->data->startReal;
	float startImag = tdata->data->startImag;
	float  maxIters = tdata->data->maxIters;
	unsigned *output = tdata->data->output;
	for (int i = startStep; i < stopStep; i++) {
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
			while(z_sum < 4.0f && iters < maxIters) {
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

void serialThreaded(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxIters) {
	struct func_data data;
	data.startReal = startReal;
	data.startImag = startImag;
	data.steps = steps;
	data.horizsteps = horizsteps;
	data.step = step;
	data.output = output;
	data.maxIters = maxIters;
	pthread_t threads[NUM_THREADS];
	struct thread_data tdata[NUM_THREADS];
	for(int i = 0; i < NUM_THREADS; i++) {
		tdata[i].threadID = i;
		tdata[i].data = &data;
		
		pthread_create(&threads[i], NULL, serialThread, (void *)&tdata[i]);
	}
	for(int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
}

void *neonThread(void * threadData) {
	struct thread_data * tdata = (struct thread_data *)threadData;
	int startStep = tdata->threadID*(tdata->data->steps/4);
	int stopStep = startStep + (tdata->data->steps/4);
	int horizsteps = tdata->data->horizsteps;
	float step = tdata->data->step;
	float startReal = tdata->data->startReal;
	float startImag = tdata->data->startImag;
	float  maxiters = tdata->data->maxIters;
	unsigned *output = tdata->data->output;
	for(int i = startStep; i < stopStep; i++) {
		for(int j=0; j < horizsteps; j+=4) {
			float32x4_t reals = vdupq_n_f32(startReal + step*j);
			float32x4_t deltas = vdupq_n_f32(0.0f);
			deltas = vsetq_lane_f32(step, deltas, 1);
			deltas = vsetq_lane_f32(step*2.0f, deltas, 2);
			deltas = vsetq_lane_f32(step*3.0f, deltas, 3);
			reals = vaddq_f32(reals, deltas);
			float32x4_t imags = vdupq_n_f32(startImag - step*i);
			int32x4_t iters = vdupq_n_s32(0);
			int32x4_t maxIters = vdupq_n_s32(maxiters);
			float32x4_t z_reals = vdupq_n_f32(0.0f);
			float32x4_t z_imags = vdupq_n_f32(0.0f);
			float32x4_t z_sums = vdupq_n_f32(0.0f);
			int32x4_t cmp_val =vdupq_n_s32(-1);
			float32x4_t fours = vdupq_n_f32(4.0f);
			float32x4_t twos = vdupq_n_f32(2.0f);
			float32x4_t negFours = vdupq_n_f32(-4.0f);
#if UNROLL_FACTOR > 1
			int32x4_t iters_old = vdupq_n_s32(0);
			float32x4_t z_reals_old, z_imags_old, z_sums_old;
			int32x4_t cmp_val_old;
			int32x4_t unroll = vdupq_n_s32(UNROLL_FACTOR);
#endif

			while(hor_s32(cmp_val) && !hor_s32(vceqq_s32(iters, maxIters))) {
#if UNROLL_FACTOR > 1
				iters_old = iters;
				z_reals_old = z_reals;
				z_imags_old = z_imags;
				z_sums_old = z_sums;
				cmp_val_old = cmp_val;
#endif

#if UNROLL_FACTOR == 1
				iters = vsubq_s32(iters, cmp_val);
#else
				int32x4_t masked_unroll = vandq_s32(unroll, cmp_val);
				iters = vaddq_s32(iters, masked_unroll);
#endif

				float32x4_t z_real_sq = vmulq_f32(z_reals, z_reals);
				float32x4_t z_imag_sq = vmulq_f32(z_imags, z_imags);
				z_sums = vaddq_f32(z_real_sq, z_imag_sq);
				z_imags = vaddq_f32(vmulq_f32(vmulq_f32(twos, z_reals), z_imags), imags);
				z_reals = vaddq_f32(vsubq_f32(z_real_sq, z_imag_sq), reals);

#define EXTRA_ITERATION_NEON \
z_real_sq = vmulq_f32(z_reals, z_reals); \
z_imag_sq = vmulq_f32(z_imags, z_imags); \
z_sums = vaddq_f32(z_real_sq, z_imag_sq); \
z_imags = vaddq_f32(vmulq_f32(vmulq_f32(twos, z_reals), z_imags), imags); \
z_reals = vaddq_f32(vsubq_f32(z_real_sq, z_imag_sq), reals);

#if UNROLL_FACTOR >= 2
				EXTRA_ITERATION_NEON
#if UNROLL_FACTOR >= 4	
				EXTRA_ITERATION_NEON
				EXTRA_ITERATION_NEON
#if UNROLL_FACTOR >= 8
				EXTRA_ITERATION_NEON
				EXTRA_ITERATION_NEON
				EXTRA_ITERATION_NEON
				EXTRA_ITERATION_NEON
#endif
#endif
#endif

				int32x4_t less4 = vcltq_f32(z_sums, fours);
				int32x4_t gtrNeg4 = vcgtq_f32(z_sums, negFours);
				cmp_val = vandq_s32(less4, gtrNeg4);
			}
#if UNROLL_FACTOR > 1
			iters = iters_old;
			z_reals = z_reals_old;
			z_imags = z_imags_old;
			z_sums = z_sums_old;
			cmp_val = cmp_val_old;

			while(hor_s32(cmp_val) && !hor_s32(vceqq_s32(iters, maxIters))) {
				iters = vsubq_s32(iters, cmp_val);
				float32x4_t z_real_sq = vmulq_f32(z_reals, z_reals);
                                float32x4_t z_imag_sq = vmulq_f32(z_imags, z_imags);
                                z_sums = vaddq_f32(z_real_sq, z_imag_sq);
                                z_imags = vaddq_f32(vmulq_f32(vmulq_f32(twos, z_reals), z_imags), imags);
                                z_reals = vaddq_f32(vsubq_f32(z_real_sq, z_imag_sq), reals);
				int32x4_t less4 = vcltq_f32(z_sums, fours);
                                int32x4_t gtrNeg4 = vcgtq_f32(z_sums, negFours);
                                cmp_val = vandq_s32(less4, gtrNeg4);
			}
#endif
			int startIndex = i*horizsteps+j;
			output[startIndex++] = (unsigned)vgetq_lane_s32(iters, 0); 
			output[startIndex++] = (unsigned)vgetq_lane_s32(iters, 1); 
			output[startIndex++] = (unsigned)vgetq_lane_s32(iters, 2); 
			output[startIndex] = (unsigned)vgetq_lane_s32(iters, 3); 
		}
	}
}

void neonThreaded(float startReal, float startImag, int steps, int horizsteps, float step, unsigned * output, signed maxiters) {
	struct func_data data;
	data.startReal = startReal;
	data.startImag = startImag;
	data.steps = steps;
	data.horizsteps = horizsteps;
	data.step = step;
	data.output = output;
	data.maxIters = maxiters;
	pthread_t threads[NUM_THREADS];
	struct thread_data tdata[NUM_THREADS];
	for(int i = 0; i < NUM_THREADS; i++) {
		tdata[i].threadID = i;
		tdata[i].data = &data;
		
		pthread_create(&threads[i], NULL, neonThread, (void *)&tdata[i]);
	}
	for(int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
}

void serialFloat(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxIters) {

    for (int i = 0; i < steps; i++) {
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
			while(z_sum < 4.0f && iters < maxIters) {
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

void neonFloat(float startReal, float startImag, int steps, int horizsteps, float step, unsigned * output, signed maxiters) {
	for(int i = 0; i < steps; i++) {
		for(int j=0; j < horizsteps; j+=4) {
			float32x4_t reals = vdupq_n_f32(startReal + step*j);
			float32x4_t deltas = vdupq_n_f32(0.0f);
			deltas = vsetq_lane_f32(step, deltas, 1);
			deltas = vsetq_lane_f32(step*2.0f, deltas, 2);
			deltas = vsetq_lane_f32(step*3.0f, deltas, 3);
			reals = vaddq_f32(reals, deltas);
			float32x4_t imags = vdupq_n_f32(startImag - step*i);
			int32x4_t iters = vdupq_n_s32(0);
			int32x4_t maxIters = vdupq_n_s32(maxiters);
			float32x4_t z_reals = vdupq_n_f32(0.0f);
			float32x4_t z_imags = vdupq_n_f32(0.0f);
			float32x4_t z_sums = vdupq_n_f32(0.0f);
			int32x4_t cmp_val =vdupq_n_s32(-1);
			float32x4_t fours = vdupq_n_f32(4.0f);
			float32x4_t twos = vdupq_n_f32(2.0f);
			float32x4_t negFours = vdupq_n_f32(-4.0f);
#if UNROLL_FACTOR > 1
			int32x4_t iters_old = vdupq_n_s32(0);
			float32x4_t z_reals_old, z_imags_old, z_sums_old;
			int32x4_t cmp_val_old;
			int32x4_t unroll = vdupq_n_s32(UNROLL_FACTOR);
#endif

			while(hor_s32(cmp_val) && !hor_s32(vceqq_s32(iters, maxIters))) {
#if UNROLL_FACTOR > 1
				iters_old = iters;
				z_reals_old = z_reals;
				z_imags_old = z_imags;
				z_sums_old = z_sums;
				cmp_val_old = cmp_val;
#endif

#if UNROLL_FACTOR == 1
				iters = vsubq_s32(iters, cmp_val);
#else
				int32x4_t masked_unroll = vandq_s32(unroll, cmp_val);
				iters = vaddq_s32(iters, masked_unroll);
#endif

				float32x4_t z_real_sq = vmulq_f32(z_reals, z_reals);
				float32x4_t z_imag_sq = vmulq_f32(z_imags, z_imags);
				z_sums = vaddq_f32(z_real_sq, z_imag_sq);
				z_imags = vaddq_f32(vmulq_f32(vmulq_f32(twos, z_reals), z_imags), imags);
				z_reals = vaddq_f32(vsubq_f32(z_real_sq, z_imag_sq), reals);

#if UNROLL_FACTOR >= 2
				EXTRA_ITERATION_NEON
#if UNROLL_FACTOR >= 4	
				EXTRA_ITERATION_NEON
				EXTRA_ITERATION_NEON
#if UNROLL_FACTOR >= 8
				EXTRA_ITERATION_NEON
				EXTRA_ITERATION_NEON
				EXTRA_ITERATION_NEON
				EXTRA_ITERATION_NEON
#endif
#endif
#endif

				int32x4_t less4 = vcltq_f32(z_sums, fours);
				int32x4_t gtrNeg4 = vcgtq_f32(z_sums, negFours);
				cmp_val = vandq_s32(less4, gtrNeg4);
			}
#if UNROLL_FACTOR > 1
			iters = iters_old;
			z_reals = z_reals_old;
			z_imags = z_imags_old;
			z_sums = z_sums_old;
			cmp_val = cmp_val_old;

			while(hor_s32(cmp_val) && !hor_s32(vceqq_s32(iters, maxIters))) {
				iters = vsubq_s32(iters, cmp_val);
				float32x4_t z_real_sq = vmulq_f32(z_reals, z_reals);
                                float32x4_t z_imag_sq = vmulq_f32(z_imags, z_imags);
                                z_sums = vaddq_f32(z_real_sq, z_imag_sq);
                                z_imags = vaddq_f32(vmulq_f32(vmulq_f32(twos, z_reals), z_imags), imags);
                                z_reals = vaddq_f32(vsubq_f32(z_real_sq, z_imag_sq), reals);
				int32x4_t less4 = vcltq_f32(z_sums, fours);
                                int32x4_t gtrNeg4 = vcgtq_f32(z_sums, negFours);
                                cmp_val = vandq_s32(less4, gtrNeg4);
			}
#endif
			int startIndex = i*horizsteps+j;
			output[startIndex++] = (unsigned)vgetq_lane_s32(iters, 0); 
			output[startIndex++] = (unsigned)vgetq_lane_s32(iters, 1); 
			output[startIndex++] = (unsigned)vgetq_lane_s32(iters, 2); 
			output[startIndex] = (unsigned)vgetq_lane_s32(iters, 3); 
		}
	}
}


double testCorrect(unsigned *output, int size) {
    double error = 0.0;
    FILE* f = fopen("reference.out", "r");
    if(!f) {printf("Could not open reference.out\n"); return -1.0;}
    for(int i = 0; i < size; i++) {
        int ref;
        int e = fscanf(f, "%i ", &ref);
        error += (double)abs((ref - output[i]))/((double)ref);
    }
    fclose(f);
    return error/((double)size);
}

unsigned* boxFilter(unsigned * output, int size, int horSize) {
    int newSize = size-1;
    int newHorSize = horSize-1;
    int numPix = (newSize)*(newHorSize);
    unsigned * filter = (unsigned*)malloc(numPix*sizeof(unsigned));
    for(int i = 0; i < newSize; i++) {
        for(int j = 0; j < newHorSize; j++) {
            unsigned sum = output[i*horSize+j] + output[i*horSize+j+1]
                            + output[(i+1)*horSize+j] + output[(i+1)*horSize+j+1];
            unsigned average = (sum+2)/4;
            filter[i*newHorSize+j] = average;
        }
    }
    free(output);
    return filter;
}

int main() {
	float centerReal = -0.54f;
        float centerImag = 0.5f;
        float size = 0.1f;
        int steps = 864;
        int maxIters = 50000;
	int horizsteps = steps*16/9;
	int numPixels = steps*horizsteps;
	unsigned* output = (unsigned*)malloc(numPixels*sizeof(unsigned));
	float horizsize = size*16.0f/9.0f;
	float startReal = centerReal - (horizsize/2.0f), startImag = centerImag + (size/2.0f);
	float stepsize = size/steps;
	serialFloat(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
	
	float time = 0.0;	
	timeval start, stop;
 	gettimeofday(&start, NULL);
	serialFloat(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
	gettimeofday(&stop, NULL);
	time = (float)(stop.tv_sec-start.tv_sec) + (float)(stop.tv_usec-start.tv_usec)/1000000.0f;
	printf("serial float ARM: %f seconds\n", time);
	
	gettimeofday(&start, NULL);
	serialThreaded(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
	gettimeofday(&stop, NULL);
	time = (float)(stop.tv_sec-start.tv_sec) + (float)(stop.tv_usec-start.tv_usec)/1000000.0f;
	printf("serial threaded ARM: %f seconds\n", time);
	
	gettimeofday(&start, NULL);
	neonFloat(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
	gettimeofday(&stop, NULL);
	time = (float)(stop.tv_sec-start.tv_sec) + (float)(stop.tv_usec-start.tv_usec)/1000000.0f;
	printf("neon float ARM: %f seconds\n", time);
 
	gettimeofday(&start, NULL);
	neonThreaded(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
	gettimeofday(&stop, NULL);
	time = (float)(stop.tv_sec-start.tv_sec) + (float)(stop.tv_usec-start.tv_usec)/1000000.0f;
	printf("neon threaded ARM: %f seconds\n", time);
 	
	output = boxFilter(output, steps, horizsteps);
        numPixels = (steps-1)*(horizsteps-1);
	uint64_t sum = 0; 
    	for (int i = 0; i < numPixels; i++) {
       		unsigned iters = output[i];
       		sum += iters;
    	}
        printf("The percent error was  %.3f%%.\n", 100.0*testCorrect(output, numPixels));
	return 0;
}
