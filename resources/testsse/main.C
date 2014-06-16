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
#include "msmloops.h"

void print_firstlast_selection(wkf_timerhandle timer, int n, int *on) {
  int sel, rc;
  wkf_timer_start(timer);
  rc = find_first_selection_aligned(n, on, &sel);
  wkf_timer_stop(timer);
  printf("first selection: %d  time: %f\n", sel, wkf_timer_time(timer));

  wkf_timer_start(timer);
  rc = find_last_selection_aligned(n, on, &sel);
  wkf_timer_stop(timer);
  printf("last selection: %d  time: %f\n", sel, wkf_timer_time(timer));
}

void print_analyze_selection(wkf_timerhandle timer, int n, int *on) {
  int first, last, selected;
  wkf_timer_start(timer);
  analyze_selection_aligned(n, on, &first, &last, &selected);
  wkf_timer_stop(timer);
  printf("selection stats: first %d  last %d  sel %d  time: %f\n", 
         first, last, selected, wkf_timer_time(timer));
}

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

    wkf_timer_start(timer);
    minmax_3fv_aligned(fv, sz/3, minfv, maxfv);
    wkf_timer_stop(timer);
    printf("minmax_3fv_aligned: %f\n", wkf_timer_time(timer));
    for (i=0; i<3; i++) {
      minf += minfv[i];
      maxf += maxfv[i];
    }

    // set exactly one selected atom to measure perf of the
    // fast first/last selection traversal loops
    on[isz/2 + 7] = 1;
    print_firstlast_selection(timer, isz, on);
    print_analyze_selection(timer, isz, on);

#if 1
    wkf_timer_start(timer);
    minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
    wkf_timer_stop(timer);
    printf("minmax_selected_3fv_aligned(  0%%): %f\n", wkf_timer_time(timer));

    for (i=0; i<(isz-7); i+=8) {
      on[i+7] = 1;
    }
    print_analyze_selection(timer, isz, on);
    wkf_timer_start(timer);
    minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
    wkf_timer_stop(timer);
    printf("minmax_selected_3fv_aligned( 12%%): %f\n", wkf_timer_time(timer));
    for (i=0; i<3; i++) {
      minf += minfv[i];
      maxf += maxfv[i];
    }


    for (i=0; i<(isz-7); i+=8) {
      on[i+6] = 1;
    }
    print_analyze_selection(timer, isz, on);
    wkf_timer_start(timer);
    minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
    wkf_timer_stop(timer);
    printf("minmax_selected_3fv_aligned( 25%%): %f\n", wkf_timer_time(timer));
    for (i=0; i<3; i++) {
      minf += minfv[i];
      maxf += maxfv[i];
    }


    for (i=0; i<(isz-7); i+=8) {
      on[i+4] = 1;
      on[i+5] = 1;
      on[i+6] = 1;
      on[i+7] = 1;
    }
    print_analyze_selection(timer, isz, on);
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
    print_analyze_selection(timer, isz, on);
    wkf_timer_start(timer);
    minmax_selected_3fv_aligned(fv, on, isz, 0, isz-1, minfv, maxfv);
    wkf_timer_stop(timer);
    printf("minmax_selected_3fv_aligned(100%%): %f\n", wkf_timer_time(timer));
    for (i=0; i<3; i++) {
      minf += minfv[i];
      maxf += maxfv[i];
    }
#endif

    msmparms mp;
    setupmsm(mp);
    wkf_timer_start(timer);
    testmsm(mp);
    wkf_timer_stop(timer);
    printf("MSM testmsm(): %f\n", wkf_timer_time(timer));
    finishmsm(mp);

    printf("\n");
  }

  wkf_timer_destroy(timer);  

  printf("checking sum: %g\n", minf+maxf);

  return 0;
}




