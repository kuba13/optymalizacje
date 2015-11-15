#include <stdio.h>
#include <stdlib.h>
 

#include <sys/time.h>
#include <time.h>

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

int mm(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  register unsigned int i,j,k; //<- OPT 1
  register unsigned int local_size=SIZE; //<- OPT 2
  double sum = 0;
  for (i = 0; i<SIZE ; i++ ) { //rows in multiply <- OPT 3
    for (j=0; j<SIZE; ) {
      for (k=0; k<SIZE; ) {

	sum = sum + first[i][k]*second[j][k] //<- OPT 5 (k<->j)
  	 + first[i][k+1]*second[j][k+1]
	 + first[i][k+2]*second[j][k+2]
	 + first[i][k+3]*second[j][k+3]
	 + first[i][k+4]*second[j][k+4]
	 + first[i][k+5]*second[j][k+5]
	 + first[i][k+6]*second[j][k+6]
	 + first[i][k+7]*second[j][k+7];
        multiply[i][j]+=sum; sum=0;

	sum = sum + first[i][k]*second[j+1][k] //<- OPT 5 (k<->j+1)
  	 + first[i][k+1]*second[j+1][k+1]
	 + first[i][k+2]*second[j+1][k+2]
	 + first[i][k+3]*second[j+1][k+3]
	 + first[i][k+4]*second[j+1][k+4]
	 + first[i][k+5]*second[j+1][k+5]
	 + first[i][k+6]*second[j+1][k+6]
	 + first[i][k+7]*second[j+1][k+7];
        multiply[i][j+1]+=sum; sum=0;

	sum = sum + first[i][k]*second[j+2][k] //<- OPT 5 (k<->j+2)
  	 + first[i][k+1]*second[j+2][k+1]
	 + first[i][k+2]*second[j+2][k+2]
	 + first[i][k+3]*second[j+2][k+3]
	 + first[i][k+4]*second[j+2][k+4]
	 + first[i][k+5]*second[j+2][k+5]
	 + first[i][k+6]*second[j+2][k+6]
	 + first[i][k+7]*second[j+2][k+7];
        multiply[i][j+2]+=sum; sum=0;

	sum = sum + first[i][k]*second[j+3][k] //<- OPT 5 (k<->j+3)
  	 + first[i][k+1]*second[j+3][k+1]
	 + first[i][k+2]*second[j+3][k+2]
	 + first[i][k+3]*second[j+3][k+3]
	 + first[i][k+4]*second[j+3][k+4]
	 + first[i][k+5]*second[j+3][k+5]
	 + first[i][k+6]*second[j+3][k+6]
	 + first[i][k+7]*second[j+3][k+7];
        multiply[i][j+3]+=sum; sum=0;

	sum = sum + first[i][k]*second[j+4][k] //<- OPT 5 (k<->j+4)
  	 + first[i][k+1]*second[j+4][k+1]
	 + first[i][k+2]*second[j+4][k+2]
	 + first[i][k+3]*second[j+4][k+3]
	 + first[i][k+4]*second[j+4][k+4]
	 + first[i][k+5]*second[j+4][k+5]
	 + first[i][k+6]*second[j+4][k+6]
	 + first[i][k+7]*second[j+4][k+7];
        multiply[i][j+4]+=sum; sum=0;

	sum = sum + first[i][k]*second[j+5][k] //<- OPT 5 (k<->j+5)
  	 + first[i][k+1]*second[j+5][k+1]
	 + first[i][k+2]*second[j+5][k+2]
	 + first[i][k+3]*second[j+5][k+3]
	 + first[i][k+4]*second[j+5][k+4]
	 + first[i][k+5]*second[j+5][k+5]
	 + first[i][k+6]*second[j+5][k+6]
	 + first[i][k+7]*second[j+5][k+7];
        multiply[i][j+5]+=sum; sum=0;

	sum = sum + first[i][k]*second[j+6][k] //<- OPT 5 (k<->j+6)
  	 + first[i][k+1]*second[j+6][k+1]
	 + first[i][k+2]*second[j+6][k+2]
	 + first[i][k+3]*second[j+6][k+3]
	 + first[i][k+4]*second[j+6][k+4]
	 + first[i][k+5]*second[j+6][k+5]
	 + first[i][k+6]*second[j+6][k+6]
	 + first[i][k+7]*second[j+6][k+7];
        multiply[i][j+6]+=sum; sum=0;

	sum = sum + first[i][k]*second[j+7][k] //<- OPT 5 (k<->j+7)
  	 + first[i][k+1]*second[j+7][k+1]
	 + first[i][k+2]*second[j+7][k+2]
	 + first[i][k+3]*second[j+7][k+3]
	 + first[i][k+4]*second[j+7][k+4]
	 + first[i][k+5]*second[j+7][k+5]
	 + first[i][k+6]*second[j+7][k+6]
	 + first[i][k+7]*second[j+7][k+7];
        multiply[i][j+7]+=sum; sum=0;
	
	k=k+8;
      }
      j=j+8;
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
      second[j][i]=i-j;
      multiply[i][j]=0.0;
    }
  }
  dtime = dclock();
  start_papi();
  iret=mm(first,second,multiply);
  stop_papi();
  dtime = dclock()-dtime;
  printf( "Time: %le \n", dtime);
  fflush( stdout );

  double check=0.0;
  for(i=0;i<SIZE;i++){
    for(j=0;j<SIZE;j++){
      check+=multiply[i][j];      
    }
  }
  printf("check %le \n",check);


  return iret;
}

