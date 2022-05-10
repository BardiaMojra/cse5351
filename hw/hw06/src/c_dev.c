// #include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
/* #define MSG "This sentence has a length of 64 characters, xxxxxxxxxxxxxxxx." */
#define ARR_LEN 8

int* intdup(int const * src, size_t len);

int main(int argc, char *argv[])
{
  int rank, size;
  rank = 0;
  size = 4;

  /* init */
  int arr[ARR_LEN];
  int sbuf[size * ARR_LEN];
  int rbuf[size * ARR_LEN];
  memset(sbuf,0,sizeof(sbuf));
  memset(rbuf,0,sizeof(rbuf));


  /* load buffers */
  for (int i=0; i<size; i++) {

    /* construct msg array */
    printf("    msg[%2d > %2d]: ", rank, i);
    for (int j=0; j<ARR_LEN; j++) {
      arr[j] = ((i*ARR_LEN)+j);
      printf("%3d ", arr[j]);
    }
    printf("\n");

    /* load msg array to send buffer */
    memcpy(sbuf, arr, ARR_LEN*sizeof(int));
    printf("[%2d/%2d] sbuf[%2d]: ", rank, size, i);
    for (int j=0; j<ARR_LEN; j++) {
      printf("%3d ", sbuf[(i*ARR_LEN)+j]);
    }
    printf("[%2d/%2d] rbuf[%2d]: ", rank, size, i);
    for (int j=0; j<ARR_LEN; j++) {
      printf("%3d ", rbuf[(i*ARR_LEN)+j]);
    }
    printf("\n");

    fflush(stdout);
  }

  return 0;
}
