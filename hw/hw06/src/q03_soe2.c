#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "macros.h"



int main(int argc, char** argv)		{
  //int     count;                /* local prime count */
  double  time;         /* parallel execution time */
  int     first;                /* index of first multiple */
  //int     global_count;         /* global prime count */
  int     lNum_mx;           /* highest value on this proc */
  int     i;
  int     rank;                   /* process rank number */
  %int     index;                /* index of current prime */
  int     lNum_mn;            /* lowest value on this proc */
  //int     n;                    /* sieving from 2, ..., n */
  int     p;                    /* number of processes */
  int     lArrLen;           /* size of proc 0's subarray */
  int     prime;                /* current prime */
  int     lArrLen;                 /* elements in lArr string */
  int     lArr_i;
  int     prime_step;
  int     lPrime_x2;
  int     sqrtNum;
  int     primeM; /* prime multiple */
  int     num_per_block;
  int     lArr_lNum;
  int     lArr_rNum;
  int     lArr_Idx;
  char*   lArr;               /* portion of 2, ..., n */
  char*   primes;


  int Num; // user input number, calc primes <= Num
  double time;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &pSize);

  int* gArr = NULL; /* init gArr ptr */

  if (rank == ROOT) {
    gArr = get_userInput(rank, pSize, &Num);
    /* calc lArr len and check if too many proc have been selected */
    lArrLen = (Num-1) / pSize;
    if ((2 + lArrLen) < (int)sqrt((double)Num)) {
      printf("[ROOT]: too many processors!!\n");
      MPI_Finalize();
      exit(1);
    }
  }


  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&Num, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  print_arrLen(rank, pSize, Num);

  lNum_mn = FIRST_INT + get_lArr_mn(rank, pSize, Num-1) * SKIP_INT;
  lNum_mx = FIRST_INT + get_lArr_mx(rank, pSize, Num-1) * SKIP_INT;
  lArrLen = (Num-1) / pSize;
  /* all proc premark primes smaller and equal to sqrtNum */
  sqrtNum = sqrt(Num);
  char* primes;
  primes = (char*)calloc(sqrtNum+1, 1);

  for (int i=2; i<=sqrtNum; i+=2) {
    primes[i] = 1;
  }

  for (int i= 3; i<=sqrtNum; i+= 2) {
    if (primes[i] == 1) {
      continue;
    }
    for (ps = i*2;  ps <= ps; ps += i) { // mark prime multiples
      primes[ps] = 1;
    }
  }

    /* start marking local array */
    lArr = (char*)calloc(lArrLen*sizeof(char),1);
    assert(lArr != NULL);

    num_per_block    = 1024*1024; // <------------------------------ why
    lArr_lNum  = lNum_mn;
    lArr_rNum = MIN(lNum_mx, lNum_mn + num_per_block * SKIP_INT);

    for (lArr_Idx = 0; lArr_Idx < lArrLen; lArr_Idx += num_per_block)    {

      /* find the first local prime  */
      for (int lPrime = 3; lPrime <= sqrtNum; lPrime++)       {
        if (primes[lPrime] == 1) {
          continue;
        }
        else {
          if (lPrime * lPrime > lArr_lNum) {
            first = lPrime * lPrime;
          }
          else {
            if (!(lArr_lNum % lPrime)) {
              first = lArr_lNum;
            }
            else {
              first = lPrime - (lArr_lNum % lPrime) + lArr_lNum;
            }
          }
        }



        /*
        * optimization - consider only odd multiples
        *                of the prime number
        */
        if ((first + lPrime) & 1) // is odd
          first += lPrime;

        lArr_i = (first - FIRST_INT) / SKIP_INT -  get_lArr_mn(rank, pSize, Num-1);
        prime_step        = (lPrime*2) / SKIP_INT;
        for (int i = first; i <= lNum_mx; i += lPrime*2) {
            lArr[lArr_i] = 1;
            lArr_i += prime_step;
        }
      }

      lArr_lNum += num_per_block * SKIP_INT;
      lArr_rNum = MIN(lNum_mx, lArr_rNum + num_per_block * SKIP_INT);
    } /* for lArr_Idx */


    time = MPI_Wtime() - time;

    /* print the results */
    if (rank == 0)   {
        global_count += 1; /* add first prime, 2 */
        printf("%d primes are less than or equal to %d\n",
               global_count, Num);
        printf("Total elapsed time: %10.6fs\n",
               time);
    } /* if */

    MPI_Finalize();

    return 0;
}

void soe(int s, int leftEnd, int rightEnd, int szLocalSievingArray, int primeIndex, char sievingArray[]) {
  int offset, prime;
  int calcOffset(int prime, int leftEnd, int rightEnd);
  prime = 2*primeIndex+3; /* Sieving prime */
  if(s == 0) {
    offset = 2*primeIndex*(primeIndex+3)+3;
  } else {
    /* Save some time in proc 0. Might be redundant if
    the bottleneck is somewhere else */
    offset = calcOffset(prime, leftEnd, rightEnd);
  }
  if(offset != -1) {
    while(offset < szLocalSievingArray){
      /* Since we do not need to sieve the even
      primes we sieve at every double multiple
      of the sieving prime. */
      sievingArray[offset] = 0;
      offset = offset + prime;
      }
  }
} /* end of soe */

int calcOffset(int prime, int leftEnd, int rightEnd){
  if(leftEnd % prime == 0)
    return 0;
  int res = -(leftEnd % prime) + prime;
  int value = res + leftEnd;
  if(value > rightEnd) {
   /** For small n we might skip a whole interval. e.g.
    * when n=25, p=4 and when sieving with 5 we skip
    * the second interval.
    */
    return -1;
  }
  if(value % 2 == 0) {
    /* We need this to be odd because we only represent odd numbers */
    return (res+prime)/2;
  } else {
    return res/2;
  }
} // end of calcOffset
