#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>
#include "papi.h"
static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

int en = 4;
const char* event_names[] = { "CACHE MISS", "CACHE HIT","FLOATING POINTS EXEC","CYCLES WAITING FOR RES"};
int events[] = { PAPI_L2_DCM, PAPI_L2_DCH,PAPI_FP_INS,PAPI_RES_STL
};
//const char* event_names[] = { "PAPI_FP_OPS", "PAPI_L1_DCM" };
//int events[] = { PAPI_FP_OPS, PAPI_L1_DCM };
long long* values;
int eventSet = PAPI_NULL;
int papi_err;
int papi_supported = 1;

void start_papi() {
    values = calloc(en, sizeof(long long));

    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
        fprintf(stderr, "PAPI is unsupported.\n");
        papi_supported = 0;
    }

    if (PAPI_num_counters() < en) {
        fprintf(stderr, "PAPI is unsupported.\n");
        papi_supported = 0;
    }
    
    if ((papi_err = PAPI_create_eventset(&eventSet)) != PAPI_OK) {
        fprintf(stderr, "Could not create event set: %s\n", PAPI_strerror(papi_err));
    }

    for (int i=0; i<en; ++i) {
        if ((papi_err = PAPI_add_event(eventSet, events[i])) != PAPI_OK ) {
            fprintf(stderr, "Could not add event: %d %s\n", i, PAPI_strerror(papi_err));
        }
    }

    /* start counters */

    if (papi_supported) {
        if ((papi_err = PAPI_start(eventSet)) != PAPI_OK) {
            fprintf(stderr, "Could not start counters: %s\n", PAPI_strerror(papi_err));
        }
    }
}

void stop_papi() {
    if (papi_supported) {
        if ((papi_err = PAPI_stop(eventSet, values)) != PAPI_OK) {
            fprintf(stderr, "Could not get values: %s\n", PAPI_strerror(papi_err));
        }
        
        int i;
        for (i = 0; i < en; ++i) {
            printf("%s: %ld\n", event_names[i], values[i]);
        }
    }
}


double dclock()
{
  double         the_time, norm_sec;
  struct timeval tv;
  gettimeofday( &tv, NULL );
  if ( gtod_ref_time_sec == 0.0 )
    gtod_ref_time_sec = ( double ) tv.tv_sec;
  norm_sec = ( double ) tv.tv_sec - gtod_ref_time_sec;
  the_time = norm_sec + tv.tv_usec * 1.0e-6;
  return the_time;
}

int mm(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  int i,j,k; 
  double sum = 0;
  for (i = 0; i < SIZE; i++) { //rows in multiply
    for (j = 0; j < SIZE; j++) { //columns in multiply
      for (k = 0; k < SIZE; k++) { //columns in first and rows in second
	    sum = sum + first[i][k]*second[k][j];
	  } 
          multiply[i][j] = sum;
	  sum = 0;
    }
  }
  return 0;
}

int main( int argc, const char* argv[] )
{
  int i,j,iret;
  double first[SIZE][SIZE];
  double second[SIZE][SIZE];
  double multiply[SIZE][SIZE];
  double dtime;


  for (i = 0; i < SIZE; i++) { //rows in first
    for (j = 0; j < SIZE; j++) { //columns in first
      first[i][j]=i+j;
      second[i][j]=i-j;
      multiply[i][j]=0.0;
    }
  }

  start_papi();
  iret=mm(first,second,multiply); 
  stop_papi();

  double check=0.0;
  for(i=0;i<SIZE;i++){
    for(j=0;j<SIZE;j++){
      check+=multiply[i][j];
    }
  }
  printf("check %le \n",check); 
  fflush( stdout );

  return iret;
}

