#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "WKFUtils.h"
#include <math.h>
#include "omp.h"
#include "mandelKernels.h"

// MULTITHREADED: Compile with -DMULTITHREADED to use multithreading (recommended)

double testCorrect(unsigned *output, int size) {
    double error = 0.0;
    FILE* f = fopen("reference.out", "r");
    if(!f) {printf("Could not open reference.out\n"); return -1.0;}
    for(int i = 0; i < size; i++) {
        int ref;
        fscanf(f, "%i ", &ref);
        error += (double)abs((ref - output[i]))/((double)ref);
    }
    fclose(f);
    return error/((double)size);
}

void genReference(unsigned *output, int size) {
    FILE* f = fopen("reference.out", "w");
    for(int i = 0; i < size; i++) {
        fprintf(f, "%u ", output[i]);
    }
    fclose(f);
}

unsigned* boxFilter(unsigned * output, int size, int horSize) {
    int newSize = size-1;
    int newHorSize = horSize-1;
    int numPix = (newSize)*(newHorSize);
    unsigned * filter = (unsigned*)malloc(numPix*sizeof(unsigned));
#pragma omp parallel for
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
void printUsage() {
    printf("Usage: testHarness serial|autovec|intrin|interchange|omp|ISPC -c centerReal centerImag -s size -d steps -f filename\n");
}

int main(int argc, char* argv[]) {
    // Standardized benchmarking test parameters are now the defaults.
    float centerReal = -0.54f, centerImag = 0.5f;
    float size = 0.1f;
    int steps = 864;
    char filename[50] = "mandelbrot.ppm";
    unsigned maxIters = 50000;

    // change to true, compile & run to create reference.out
    bool initRefData = false;

    if (argc < 2) {
      printUsage();
      return 0;
    }

    // parse serial|autovec|intrin|interchange|omp|ISPC
    bool serial = false;
    bool autovec = false;
    bool intrin = false;
    bool interchange = false;
    bool omp = false;
    bool ispc = false;
    if (!strcmp(argv[1], "serial\0")) serial = true;
    else if (!strcmp(argv[1], "autovec\0")) autovec = true;
    else if (!strcmp(argv[1], "intrin\0")) intrin = true;
    else if (!strcmp(argv[1], "interchange\0")) interchange = true;
    else if (!strcmp(argv[1], "omp\0")) omp = true;
    else if (!strcmp(argv[1], "ISPC\0")) ispc = true;
    else {
      printUsage();
      return 0;
    }

    // parse all other command line args
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
        } else {
            printUsage();
            return 0;
        }
    }

    if (intrin || interchange)
#ifdef __SSE2__
#ifndef __AVX__
        // make steps divisible by 4
        while (steps % 4 != 0) steps++;
#else // #ifdef __AVX__
        // make steps divisible by 8
        while (steps % 8 != 0) steps++;
#endif
#else // #ifndef __SSE2__
#ifdef __MIC__
        // make steps divisible by 16
        while (steps % 16 != 0) steps++;
#endif // __MIC__
#endif // __SSE2__

    int horizsteps = steps*16/9;
    if (horizsteps % 16 != 0) {
        printf("Use one of these numbers of steps: 576, 864, 1152, 1440, 1728, 2304\n");
        printf("steps: %i, horizsteps: %i\n", steps, horizsteps);
        return 0;
    }

    int numPixels = steps*horizsteps;
    unsigned* output = (unsigned*)malloc(numPixels*sizeof(unsigned));
    float horizsize = size*16.0f/9.0f;
    float startReal = centerReal - (horizsize/2.0f), startImag = centerImag + (size/2.0f);
    float stepsize = size/steps;
    float time;
    
    if(initRefData) {
        serialMandel(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
        output = boxFilter(output, steps, horizsteps);
        numPixels = (steps-1)*(horizsteps-1);
        genReference(output, numPixels);
        printf("At 5%% tolerance, the percent error was  %.3f%%.\n", 100.0*testCorrect(output, numPixels)); 
        return 0;
    }

    int warmUpRuns = 10;
    for (int i = 0; i < warmUpRuns; i++) {
        serialMandel(-2.0f, 2.0f, steps, horizsteps, 0.004f, output, 500);
    }
    printf("======================\n");

    // MAIN TEST RUN
    wkf_timerhandle timer = wkf_timer_create();
    wkf_timer_start(timer); 
    if (serial) serialMandel(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
    if (autovec) autoVecMandel(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
    if (intrin) intrinsicsMandel(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
    if (interchange) interchangeMandel(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
    if (omp) ompMandel(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
    if (ispc) ispcMandel(startReal, startImag, steps, horizsteps, stepsize, output, maxIters);
    wkf_timer_stop(timer);
    time = wkf_timer_time(timer);
    printf("Time: %f\n", time);

    // box filter algorithm
    output = boxFilter(output, steps--, horizsteps--);
    int oldNumPixels = numPixels;
    numPixels = steps*horizsteps;

    // performance metrics
    uint64_t sum = 0;
#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < numPixels; i++) {
        unsigned iters = output[i];
        sum += iters;
    }
    printf("Iters per second: %f\nIters per pixel per second: %f\nGFLOPS: %f\n", sum/time, (sum/oldNumPixels)/time, sum*9/time/1000000000.0f);

    // correctness test
    double error = testCorrect(output, numPixels);
    if (error < 0) { printf("Aborting.\n"); return 0; }  // testCorrect failed to find the reference file
    printf("The percent error was %.3f%%. ", 100.0*error);
    if(error < 0.05) {
        printf("Error within acceptable region.\n");
    } else {
        printf("Error outside acceptable region. Aborting.\n");
        return 0;
    }
    printf("======================\n");

    return 0;
}

