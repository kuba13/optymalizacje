/* C source code is found in dgemm_example.c */

#define min(x,y) (((x) < (y)) ? (x) : (y))

#include <stdio.h>
#include <stdlib.h>
#include "mkl.h"
#include <sys/time.h>

#include "papi_helper.h"

static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

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


int main()
{
  double *A, *B, *C;
  int m, n, k, i, j;
  double alpha, beta;
  double dtime;
  printf ("\n This example computes real matrix C=alpha*A*B+beta*C using \n"
            " Intel(R) MKL function dgemm, where A, B, and  C are matrices and \n"
	  " alpha and beta are double precision scalars\n\n");

  m = SIZE; k = SIZE; n = SIZE;
  printf (" Initializing data for matrix multiplication C=A*B for matrix \n"
	  " A(%ix%i) and matrix B(%ix%i)\n\n", m, k, k, n);
  alpha = 1.0; beta = 0.0;

  printf (" Allocating memory for matrices aligned on 64-byte boundary for better \n"
	  " performance \n\n");
  A = (double *)mkl_malloc( m*k*sizeof( double ), 64 );
  B = (double *)mkl_malloc( k*n*sizeof( double ), 64 );
  C = (double *)mkl_malloc( m*n*sizeof( double ), 64 );
  if (A == NULL || B == NULL || C == NULL) {
    printf( "\n ERROR: Can't allocate memory for matrices. Aborting... \n\n");
    mkl_free(A);
    mkl_free(B);
    mkl_free(C);
    return 1;
  }

  printf (" Intializing matrix data \n\n");
  for (i = 0; i < (m*k); i++) {
    A[i] = (double)(i+1);
  }
  for (i = 0; i < (k*n); i++) {
    B[i] = (double)(-i-1);
  }

  for (i = 0; i < (m*n); i++) {
    C[i] = 0.0;
  }


  printf (" Computing matrix product using Intel(R) MKL dgemm function via CBLAS interface \n\n");
  dtime = dclock();
  start_papi();
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
	      m, n, k, alpha, A, k, B, n, beta, C, n);
  stop_papi();
  dtime = dclock()-dtime;
  printf ("\n Computations completed.\n\n");
  printf( "Time: %le \n", dtime);
  fflush( stdout );


  printf ("\n Deallocating memory \n\n");
  mkl_free(A);
  mkl_free(B);
  mkl_free(C);

  printf (" Example completed. \n\n");
  return 0;
}

