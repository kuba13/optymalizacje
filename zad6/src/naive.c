#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <papi.h>

#define INDEX(i, j) ((i * n) + j)
#define PATH "C:\Users\Kuba\Desktop\informatyka\semestr7\optymalizacje\repo\optymalizacje\zad6"

int en = 2;
const char* event_names[] = { "FP_OPS", "L1_DCM" };
int events[] = { PAPI_FP_OPS, PAPI_L1_DCM };

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
            fprintf(stderr, "Could not add event: %s %s\n", event_names[i], PAPI_strerror(papi_err));
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
            printf("%s: %lld\n", event_names[i], values[i]);
        }
    }
}


static double gtod_ref_time_sec = 0.0;
double last_time = 0.0;
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

void start_clock() {
    last_time = dclock();
}

void stop_clock() {
    printf("Time: %lf\n", dclock() - last_time);
}

double* cholesky(double *A, int n) {
    int i, j, k;
    double *L = (double*)calloc(n * n, sizeof(double));
    for (i = 0; i < n; ++i) {
        for (j = 0; j < i; ++j) {
            L[INDEX(i, j)] = A[INDEX(i, j)];
            for (k = 0; k < j; ++k) {
                L[INDEX(i, j)] -= L[INDEX(i, k)] * L[INDEX(j, k)];
            }

            L[INDEX(i, j)] /= L[INDEX(j, j)];
        }

        L[INDEX(j, j)] = A[INDEX(j, j)];
        for (k = 0; k < i; ++k) {
            L[INDEX(i, i)] -= L[INDEX(i, k)] * L[INDEX(i, k)];
        }
       
        L[INDEX(i, i)] = sqrt(L[INDEX(i, i)]);
    }

    return L;
} 

int main(int argc, char* argv[]) {
    int i, j;
    int n = (SIZE);
    double A[(SIZE) * (SIZE)] = {0};
    double *L = NULL;
    FILE *fp = NULL;
    char filename[150];

    sprintf(filename, "%s/matrices/%d", PATH, SIZE);
    fp = fopen(filename, "r");
    if (fp == NULL) {
        return 1;
    }

    for (i = 0; i < n * n; ++i) {
        fscanf(fp, "%lf", A + i);  
    }

    start_clock();
    start_papi();
    L = cholesky(A, n);
    stop_papi();
    stop_clock();

    double checksum = 0.0;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            checksum += i + j + L[INDEX(i, j)];
        }
    }

    printf("Size: %d\n", n);
    printf("Checksum: %lf\n", checksum);
    return 0;
}
