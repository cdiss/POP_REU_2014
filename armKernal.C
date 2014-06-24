#include <arm_neon.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

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

void serialFloat(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxIters) {

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
            output[i*horizsteps+j] = iters;
        }
    }
}

void armFloat(float startReal, float startImag, int steps, int horizsteps, float step, unsigned * output, signed maxiters) {
	for(int i = 0; i < steps; i++) {
		for(int j=0; j < horizsteps; j+=4) {
			float32x4_t reals = vdupq_n_f32(startReal + step*j);
			float32x4_t deltas = vdupq_n_f32(0.0f);
			deltas = vsetq_lane_f32(step, deltas, 1);
			deltas = vsetq_lane_f32(step*2.0f, deltas, 2);
			deltas = vsetq_lane_f32(step*3.0f, deltas, 3);
		//	debugPrint(deltas);
			reals = vaddq_f32(reals, deltas);
		//	debugPrint(reals);
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
			while(hor_s32(cmp_val) && !hor_s32(vceqq_s32(iters, maxIters))) {
				iters = vsubq_s32(iters, cmp_val);
			//	debugPrint(cmp_val);
			//	debugPrint(iters);
				float32x4_t z_real_sq = vmulq_f32(z_reals, z_reals);
				float32x4_t z_imag_sq = vmulq_f32(z_imags, z_imags);
				z_sums = vaddq_f32(z_real_sq, z_imag_sq);
			//	debugPrint(z_sums);
				int32x4_t less4 = vcltq_f32(z_sums, fours);
				int32x4_t gtrNeg4 = vcgtq_f32(z_sums, negFours);
				cmp_val = vandq_s32(less4, gtrNeg4);
				z_imags = vaddq_f32(vmulq_f32(vmulq_f32(twos, z_reals), z_imags), imags);
				z_reals = vaddq_f32(vsubq_f32(z_real_sq, z_imag_sq), reals);
			}
			int startIndex = i*horizsteps+j;
			output[startIndex++] = (unsigned)vgetq_lane_s32(iters, 0); 
			output[startIndex++] = (unsigned)vgetq_lane_s32(iters, 1); 
			output[startIndex++] = (unsigned)vgetq_lane_s32(iters, 2); 
			output[startIndex] = (unsigned)vgetq_lane_s32(iters, 3); 
		//	printf("Finished %d\n", omp_get_thread_num(), startIndex);
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
//	clock_t start = clock(), diff;
	serialFloat(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
//	diff = clock() - start;
//	int msec = diff * 1000 / CLOCKS_PER_SEC;
//	printf("Serial code took %d seconds %d milliseconds", msec/1000, msec%1000);
//
//	start = clock();
//	armFloat(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
//	diff = clock() - start;
//	msec = diff * 1000 / CLOCKS_PER_SEC;
//	printf("ARM Intrinsics took %d seconds %d milliseconds", msec/1000, msec%1000);
        output = boxFilter(output, steps, horizsteps);
        numPixels = (steps-1)*(horizsteps-1);
        printf("At 5%% tolerance, the percent error was  %.3f%%.\n", 100.0*testCorrect(output, numPixels));
	FILE *f = fopen("output.txt", "w");
	for(int i = 0; i < numPixels; i++) {
		fprintf(f, "%i ", output[i]);
	}
}
