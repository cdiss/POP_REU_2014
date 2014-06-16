// #define VMDUSESSE 1

#if defined(__SSE2__)
#include <emmintrin.h>
#endif
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "WKFUtils.h"
#include "utilities.h"

int main() {
  int i;
  float minfv[3], maxfv[3];
  float minf, maxf;
  int isz = 100000000;
  int sz = 3 * isz;
  int *on = (int *) malloc(isz * sizeof(int));
  float *fv = (float *) malloc(sz * sizeof(float));

  for (i=0; i<sz; i++) 
    fv[i] = (float) i; 

  wkf_timerhandle timer = wkf_timer_create();

  int j;
  for (j=0; j<1; j++) {
    memset(on, 0, isz*sizeof(int));

    wkf_timer_start(timer);
    minmax_1fv_aligned(fv, sz, &minf, &maxf);
    wkf_timer_stop(timer);
    printf("minmax_1fv_aligned: %f\n", wkf_timer_time(timer));
//  printf("min: %f max: %f\n", minf, maxf);


    wkf_timer_stop(timer);
    minmax_3fv_aligned(fv, sz/3, minfv, maxfv);
    wkf_timer_stop(timer);
    printf("minmax_3fv_aligned: %f\n", wkf_timer_time(timer));
    for (i=0; i<3; i++) {
      minf += minfv[i];
      maxf += maxfv[i];
    }

#if 0
    wkf_timer_start(timer);
    minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
    wkf_timer_stop(timer);
    printf("minmax_selected_3fv_aligned(  0%%): %f\n", wkf_timer_time(timer));

    for (i=0; i<isz; i+=8) {
      on[i  ] = 1;
    }
    wkf_timer_start(timer);
    minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
    wkf_timer_stop(timer);
    printf("minmax_selected_3fv_aligned( 12%%): %f\n", wkf_timer_time(timer));
    for (i=0; i<3; i++) {
      minf += minfv[i];
      maxf += maxfv[i];
    }


    for (i=0; i<isz; i+=8) {
      on[i  ] = 1;
      on[i+2] = 1;
    }
    wkf_timer_start(timer);
    minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
    wkf_timer_stop(timer);
    printf("minmax_selected_3fv_aligned( 25%%): %f\n", wkf_timer_time(timer));
    for (i=0; i<3; i++) {
      minf += minfv[i];
      maxf += maxfv[i];
    }


    for (i=0; i<isz; i+=8) {
      on[i  ] = 1;
      on[i+1] = 1;
      on[i+2] = 1;
      on[i+3] = 1;
    }
    wkf_timer_start(timer);
    minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
    wkf_timer_stop(timer);
    printf("minmax_selected_3fv_aligned( 50%%): %f\n", wkf_timer_time(timer));
    for (i=0; i<3; i++) {
      minf += minfv[i];
      maxf += maxfv[i];
    }


    for (i=0; i<isz; i++)
      on[i] = 1;
    wkf_timer_start(timer);
    minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
    wkf_timer_stop(timer);
    printf("minmax_selected_3fv_aligned(100%%): %f\n", wkf_timer_time(timer));
    for (i=0; i<3; i++) {
      minf += minfv[i];
      maxf += maxfv[i];
    }
#endif

    int k;
    for (k=0; k<12; k++) {
      wkf_timer_stop(timer);
      minmax_3fv_aligned(fv, sz/3, minfv, maxfv);
      wkf_timer_stop(timer);
      printf("minmax_3fv_aligned: %f\n", wkf_timer_time(timer));
      for (i=0; i<3; i++) {
        minf += minfv[i];
        maxf += maxfv[i];
      }
    }

    printf("\n");
  }

  wkf_timer_destroy(timer);  

  printf("checking sum: %g\n", minf+maxf);

  return 0;
}




