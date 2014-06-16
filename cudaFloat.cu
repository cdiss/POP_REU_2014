#include <stdio.h>
#include "WKFUtils.h"

__global__ void kernelFloat1D(float startReal, float startImag, int steps, float step, unsigned* output, unsigned maxIters) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int i = idx / steps;
    int j = idx % steps;
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
    output[idx] = iters;
}


__global__ void kernelFloat2D(float startReal, float startImag, float step, unsigned* output, unsigned maxIters) {
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int idx = i * gridDim.x * blockDim.x + j;
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
    output[idx] = iters;
}


__global__ void kernelFloat1D_unrolled(float startReal, float startImag, int steps, float step, unsigned* output, unsigned maxIters) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int i = idx / steps;
    int j = idx % steps;
    float real = startReal + step*j;
    float imag = startImag - step*i;
    unsigned iters = 0, iters_old = 0;
    float z_real = 0.0f, z_imag = 0.0f, z_sum = 0.0f;
    float z_real_old, z_imag_old, z_sum_old;

    while (z_sum < 4.0f && iters < maxIters) {
        iters_old = iters;
        z_real_old = z_real;
        z_imag_old = z_imag;
        z_sum_old = z_sum;        

        iters+=8;

        float z_real_sq = z_real*z_real;
        float z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;
    }

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


    output[idx] = iters;
}

__global__ void kernelDouble2D_unrolled(double startReal, double startImag, double step, unsigned* output, unsigned maxIters) {
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int idx = i * gridDim.x * blockDim.x + j;
    double real = startReal + step*j;
    double imag = startImag - step*i;
    unsigned iters = 0, iters_old = 0;
    double z_real = 0.0, z_imag = 0.0, z_sum = 0.0;
    double z_real_old, z_imag_old, z_sum_old;

    while (z_sum < 4.0f && iters < maxIters) {
        iters_old = iters;
        z_real_old = z_real;
        z_imag_old = z_imag;
        z_sum_old = z_sum;        

        iters+=8;

        double z_real_sq = z_real*z_real;
        double z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;

        z_real_sq = z_real*z_real;
        z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;
    }

    iters = iters_old;
    z_real = z_real_old;
    z_imag = z_imag_old;
    z_sum = z_sum_old;
    while (z_sum < 4.0f && iters < maxIters) {
        iters++;
        double z_real_sq = z_real*z_real;
        double z_imag_sq = z_imag*z_imag;
        z_sum = z_real_sq + z_imag_sq;
        z_imag = 2.0f*z_real*z_imag + imag;
        z_real = z_real_sq - z_imag_sq + real;
    }


    output[idx] = iters;
}


void cudaDouble(double startReal, double startImag, int steps, int horizsteps, double step, unsigned* output, unsigned maxiters) {

    cudaError err;

    wkf_timerhandle timer = wkf_timer_create();
    
    int numPixels = steps*horizsteps;

    unsigned* d_output;
    err = cudaMalloc(&d_output, (numPixels+511)*sizeof(unsigned));
    if (err != cudaSuccess) {printf("Error with cudaMalloc: %i\n", err); return;}
    
    wkf_timer_start(timer);
    dim3 dimGrid(horizsteps/16, steps/16, 1);
    dim3 dimBlock(16, 16, 1);
    kernelDouble2D_unrolled<<<dimGrid, dimBlock>>>(startReal, startImag, step, d_output, maxiters);
    err = cudaThreadSynchronize();
    if (err != cudaSuccess) {printf("Error with cudaMalloc: %i\n", err); return;}
    wkf_timer_stop(timer);
    printf("kernelDouble2D_unrolled: Time: %f\n", wkf_timer_time(timer));


    err = cudaMemcpy(output, d_output, numPixels*sizeof(unsigned), cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {printf("Error with cudaMemcpy\n"); return;}

    err = cudaFree(d_output);
    if (err != cudaSuccess) {printf("Error with cudaFree\n"); return;}

}

