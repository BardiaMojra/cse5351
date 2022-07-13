/**
  @author Bardia Mojra
  @link https://www.math.tu-cottbus.de/~kd/parallel/mpi/mpi-course.book_133.html
  @link http://mpi.deino.net/mpi_functions/MPI_Cart_shift.html
*/


#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/* config */
#define ROOT 0
#define INPUT_LEN 10 // user input cmd buffer size

/* private declarations */
void get_userInput(int rank, int size, int* len);
void prt_var(int rank, int size, int* var, char** lab);
void getInput_arrEntries(int rank, int size, int* arr, int len);
void getInput_arr(int rank, int size, int* arr, int len);
void get_randArr(int rank, int size, int* arr, int len);
void get_userArr(int rank, int size, int* arr, int len);
void prt_buff(int rank, int size, int* buff, int len);
void prt_lBuffs(int rank, int size, int* buff, int len);
void getInput_shift(int rank, int size, int len, int* run_stat, int* shift);
int  get_destRank(int rank, int size, int dir);
void SHIFT(int rank, int size, int* arr, int len, int shift);
void load_sBuf(int rank, int size, int* arr, int len, int* sbuf, int shift);
void update_arr(int rank, int size, int* arr, int len, int* rbuf, int shift);

int main (int argc, char ** argv) {
  char* lab = NULL; //todo NBUG
  int len = 0;
  int* arr = NULL;
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == ROOT){
    get_userInput(rank, size, &len);
  }
  MPI_Bcast(&len, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  arr = (int*) calloc(len, sizeof(int));
  assert(arr != NULL);
  memset(arr, 0, len);
  prt_lBuffs(rank, size, arr, len);








  getInput_arrEntries(rank, size, arr, len);
  prt_lBuffs(rank, size, arr, len);

  int shift = 0;
  int run_stat = 1;  //todo add gracefull exit (low priority)
  while(run_stat) {
    if(rank == ROOT) {
      getInput_shift(rank, size, len, &run_stat, &shift);
    }

    /* MPI_Barrier(MPI_COMM_WORLD); */
    MPI_Bcast(&shift, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    prt_lBuffs(rank, size, arr, len);
    lab = "shift"; prt_var(rank, size, &shift, &lab); //todo remove line (NBUG)

    MPI_Barrier(MPI_COMM_WORLD);
    if(shift!=0) {
      SHIFT(rank, size, arr, len, shift); // perform circular SHIFT
      prt_lBuffs(rank, size, arr, len);
    }

  } // end of while(1)
  if(rank == ROOT) {
    printf("[%2d/%2d]: end of process...\n\n\n", rank, size);
  }
  free(arr);
  MPI_Finalize();
  return 0;
}

/* private definitions */
void get_userInput(int rank, int size, int* len) {
  printf("[%2d/%2d]: <=> [rank/size]\n", rank, size); fflush(stdout);
  printf("[%2d/%2d]: number of processors 'size': %d\n", rank, size, size); fflush(stdout);
  printf("[%2d/%2d]: note: all arrays will have the same size.\n", rank, size); fflush(stdout);
  printf("[%2d/%2d]: enter array length: \n", rank, size); fflush(stdout);
  scanf("%d", len);
  printf("[%2d/%2d]: next, each processor will ask for its array entries.\n", rank, size); fflush(stdout);
  printf("[%2d/%2d]: note: array entries and the length variable are type int.\n", rank, size); fflush(stdout);
  return;
}

void prt_lBuffs(int rank, int size, int* buff, int len) {
  for (int i=0; i<size; i++) {
    if (i==rank) {
      prt_buff(rank, size, buff, len);
    } MPI_Barrier(MPI_COMM_WORLD);
  } MPI_Barrier(MPI_COMM_WORLD);
  return;
}

/*
void prt_shift_cmd(int rank, int size, char* cmd, int dir, int disp) {
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == ROOT) {
    printf("[%2d/%2d]: cmd:%s dir:%d  disp:%d \n", rank, size, cmd, dir, disp); fflush(stdout);
  } MPI_Barrier(MPI_COMM_WORLD);
  return;
}
*/

void prt_var(int rank, int size, int* var, char** lab) {
  printf("[%2d/%2d]: %7s: %6d\n", rank, size, *lab, *var); fflush(stdout);
  return;
}

void getInput_arrEntries(int rank, int size, int* arr, int len) {
  for (int i=0; i<size; i++) {
    if (i==rank) {
      getInput_arr(rank, size, arr, len);
    } MPI_Barrier(MPI_COMM_WORLD);
  } MPI_Barrier(MPI_COMM_WORLD);
  return;
}

void prt_buff(int rank, int size, int* buff, int len) {
  printf("[%2d/%2d]: ", rank, size); fflush(stdout);
  for(int j=0; j<len; j++) {
    printf("%2d ", buff[j]); fflush(stdout);
  } printf("\n"); fflush(stdout);
  return;
}

void getInput_arr(int rank, int size, int* arr, int len) {
  char* input;
  input = (char*) calloc(10, sizeof(char));
  printf("[%2d/%2d]: generate random input integers?[yes/no]\n", rank, size); fflush(stdout);
  scanf("%s", input);
  if (strcmp(input, "yes") == 0) {
    printf("[%2d/%2d]: will generate random entries.\n", rank, size); fflush(stdout);
    get_randArr(rank, size, arr, len);
  } else if (strcmp(input, "no") == 0) {
    printf("[%2d/%2d]: will get user entries.\n", rank, size); fflush(stdout);
    get_userArr(rank, size, arr, len);
  } else {
    printf("[%2d/%2d]: unknown input!\n", rank, size); fflush(stdout);
    printf("[%2d/%2d]: will generate random entries.\n", rank, size); fflush(stdout);
    get_randArr(rank, size, arr, len);
  }
  prt_buff(rank, size, arr, len);
  free(input);
  return;
}

void get_randArr(int rank, int size, int* arr, int len) {
  srand(time(NULL) + rank);
  printf("[%2d/%2d]: rand: ", rank, size); fflush(stdout);
  for(int i=0; i<len; i++) {
    arr[i] = rand(); // scanf("%d", arr[i]);
    printf("%2d ", arr[i]); fflush(stdout);
  }
  printf("\n"); fflush(stdout);
  return;
}

void get_userArr(int rank, int size, int* arr, int len) {
  printf("[%2d/%2d]: uInput: \n", rank, size); fflush(stdout);
  for(int i=0; i<len; i++) {
    scanf("%d", arr[i]);
  }
  return;
}

void getInput_shift(int rank, int size, int len, int* run_stat, int* shift) {
  printf("[%2d/%2d]: enter a shift value: \n", rank, size); fflush(stdout);
  scanf("%d", shift);
  if(shift==0) {
    run_stat = 0; // reset for graceful exit
  }
  return;
}

int get_destRank(int rank, int size, int shift) {
  int destRank;
  if(shift>0) {
    destRank = rank + 1;
  } else if(shift<0) {
    destRank = rank - 1;
  } else if(shift==0) {
    destRank = rank;
  } else {
    assert(0 && "invalid shift value!!\n");
  }
  char* lab = NULL; //todo NBUG
  lab = "destRank"; prt_var(rank, size, &destRank, &lab); //todo remove line (NBUG)
  destRank = destRank % size;
  lab = "destRank"; prt_var(rank, size, &destRank, &lab); //todo remove line (NBUG)
  return destRank;
}


void SHIFT(int rank, int size, int* arr, int len, int shift) {
  int dest = get_destRank(rank, size, shift);
  int disp = abs(shift);
  MPI_Status status;

  int* sbuf = NULL;
  int* rbuf = NULL;
  sbuf = (int*) calloc(disp, sizeof(int));
  rbuf = (int*) calloc(disp, sizeof(int));
  assert(sbuf != NULL);
  assert(rbuf != NULL);
  memset(sbuf, 0, disp);
  memset(rbuf, 0, disp);

  prt_lBuffs(rank, size, sbuf, disp);  //todo remove line (NBUG)
  load_sBuf(rank, size, arr, len, sbuf, shift);
  prt_lBuffs(rank, size, sbuf, disp);  //todo remove line (NBUG)
  prt_lBuffs(rank, size, arr, len);  //todo remove line (NBUG)

  MPI_Sendrecv(sbuf, disp, MPI_INT, dest, 0, rbuf, disp, MPI_INT, rank, \
    0, MPI_COMM_WORLD, &status);

  prt_lBuffs(rank, size, rbuf, disp);  //todo remove line (NBUG)
  update_arr(rank, size, arr, len, rbuf, shift);

  free(sbuf);
  free(rbuf);
}

void load_sBuf(int rank, int size, int* arr, int len, int* sbuf, int shift) {
  int disp = abs(shift);
  int st_idx = 0;
  if(shift > 0) {
    st_idx = len - disp;
  }
  for(int i = 0; i<disp; i++) {
    sbuf[i] = arr[st_idx + i];
  }
  return;
}

void update_arr(int rank, int size, int* arr, int len, int* rbuf, int shift) {
  int disp = abs(shift);
  int* temp = NULL;
  temp = (int*) calloc(len, sizeof(int));
  assert(temp != NULL);
  memset(temp, 0, len);
  if(shift > 0) { // positive shift
    for(int i=0; i<disp; i++) { // load rbuf to temp
      temp[i] = rbuf[i];
    }
    for(int i=disp; i<len; i++) { // load arr to temp
      temp[i] = arr[i - disp];
    }
  } else if(shift < 0) { // negative shift
    for(int i=0; i<len-disp; i++) { // load arr to temp
      temp[i] = arr[i + disp];
    }
    for(int i=len-disp; i<len; i++) { // load rbuf to temp
      temp[i] = rbuf[i - (len - disp)];
    }
  } else { // shift == 0
    return;
  }
  for(int i=0; i<len; i++) { // update arr
      arr[i] = temp[i];
  }
  free(temp);
  return;
}
/* EOF */
