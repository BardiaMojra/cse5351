# c test
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
/* #define MSG "This sentence has a length of 64 characters, xxxxxxxxxxxxxxxx." */
#define MSG_LEN 64

int main(int argc, char *argv[])
{
  int rank, size;
  MPI_Request request;
  MPI_Status status;

  /* init */
  char msg[MSG_LEN];
  int loop = 0;
  char sendbuf[size * MSG_LEN];
  char recvbuf[size * MSG_LEN];
  memset(sendbuf,0,sizeof(sendbuf));
  memset(recvbuf,0,sizeof(recvbuf));
  char *ptr;

  /* load send buffer */
  for (int i=0; i<size; i++) {
    sprintf(msg, "message from (%d) to (%d). This message is %d char long.\n\0",\
    rank, i, MSG_LEN);
    printf(msg);
    char *ptr =  sendbuf+(i*MSG_LEN);
    strncpy(ptr, msg, MSG_LEN-1);
    sendbuf[(i*MSG_LEN)-1] = '\0';
    printf("(%d) sendbuf[%d]: %s\n", rank, i, ptr);

    memset(msg,'\0',MSG_LEN);
    printf("%s", msg);
    char *ptrr =  recvbuf+(i*MSG_LEN);
    strncpy(ptrr, msg, MSG_LEN-1);
    printf("(%d) recvbuf[%d]: %s\n", rank, i, ptrr);
  }

  return 0;
}
