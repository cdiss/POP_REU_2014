// This function implements the entire kernel.
// Compare with ispcMandel_inner (and note) below
export void ispcMandel_entire(uniform float startReal, uniform float startImag, uniform int32 steps, uniform int32 horizsteps, uniform float step, uniform unsigned int32 * uniform output, uniform unsigned int32 maxiters) {
    
    foreach(i = 0 ... steps, j = 0 ... horizsteps) {
        float real = startReal + step*j;
        float imag = startImag - step*i;
        unsigned int32 iters = 0;
        float z_real = 0.0f, z_imag = 0.0f, z_sum = 0.0f;
        while (z_sum < 4.0f && iters < maxiters) {
            iters++;
            float z_real_sq = z_real*z_real;
            float z_imag_sq = z_imag*z_imag;
            z_sum = z_real_sq + z_imag_sq;
            z_imag = 2.0f*z_real*z_imag + imag;
            z_real = z_real_sq - z_imag_sq + real;
        }
        int index = i*horizsteps + j;
        output[index] = iters;
    }
}

// This function implements only the inner loop.
// This way we can use the OMP-parallelized outer loop in the wrapper function "ispcMandel" in the main code to parallelize across threads, while ISPC is doing all of the vectorization inside the inner loop
export void ispcMandel_inner(uniform float startReal, uniform float startImag, uniform int32 i, uniform int32 horizsteps, uniform float step, uniform unsigned int32 * uniform output, uniform unsigned int32 maxiters) {
    
    foreach(j = 0 ... horizsteps) {
        float real = startReal + step*j;
        float imag = startImag - step*i;
        unsigned int32 iters = 0;
        float z_real = 0.0f, z_imag = 0.0f, z_sum = 0.0f;
        while (z_sum < 4.0f && iters < maxiters) {
            iters++;
            float z_real_sq = z_real*z_real;
            float z_imag_sq = z_imag*z_imag;
            z_sum = z_real_sq + z_imag_sq;
            z_imag = 2.0f*z_real*z_imag + imag;
            z_real = z_real_sq - z_imag_sq + real;
        }
        int index = i*horizsteps + j;
        output[index] = iters;
    }
}
