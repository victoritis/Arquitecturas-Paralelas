#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define DEBUG 2

/* Translation of the DNA bases
   A -> 0
   C -> 1
   G -> 2
   T -> 3
   N -> 4*/

#define M  10 // Number of sequences (filas)
#define N  2  // Number of bases per sequence (columnas)

unsigned int g_seed = 0;

int fast_rand(void) {
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16) % 5;
}

// The distance between two bases
int base_distance(int base1, int base2){

  if((base1 == 4) || (base2 == 4)){
    return 3;
  }

  if(base1 == base2) {
    return 0;
  }

  if((base1 == 0) && (base2 == 3)) {
    return 1;
  }

  if((base2 == 0) && (base1 == 3)) {
    return 1;
  }

  if((base1 == 1) && (base2 == 2)) {
    return 1;
  }

  if((base2 == 2) && (base1 == 1)) {
    return 1;
  }

  return 2;
}

int main(int argc, char *argv[] ) {

  int i, j, size, rank;
  int *data1, *data2, *rec_buf1, *rec_buf2;
  int *result;
  struct timeval  tv1, tv2;

  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  data1 = (int *) malloc(M*N*sizeof(int));
  data2 = (int *) malloc(M*N*sizeof(int));
  rec_buf1 = (int *) malloc(M*N*sizeof(int));
  rec_buf2 = (int *) malloc(M*N*sizeof(int));
  result = (int *) malloc(M*sizeof(int));

  int reparto = M / size;                     //filas por proceso
  int resto = M % size;                       //filas sobrantes
  int *sendcounts = malloc(sizeof(int)*size); //array de elementos a enviar por proceso
  int *displs = malloc(sizeof(int)*size);     //desplazamiento por proceso
  int sum = 0;


  for(int i = 0; i < size; i++){

    sendcounts[i] = reparto; //filas por proceso

    if(resto > 0){
      sendcounts[i]++; //en caso de haber resto, sumamos una fila más
      resto--;
    }

    displs[i] = sum;            //calculamos el desplazamiento, que empieza siendo 0 para el primer proceso
    sum += sendcounts[i] * N;   //actualizamos el desplazamiento
    
    /*
    una vez sabemos cuantas filas va a tener cada proceso, multiplicamos
    por el número de elementos por fila, para saber cuantos elementos enviar
    a cada proceso  
    */
    sendcounts[i] = sendcounts[i]*N;  
  }  

  if(rank == 0){
    /* Initialize Matrices */
    for(i=0;i<M;i++) {
      for(j=0;j<N;j++) {
        /* random with 20% gap proportion */
        data1[i*N+j] = fast_rand();
        data2[i*N+j] = fast_rand();
      }
    }
  }

  MPI_Scatterv(&data1, sendcounts, displs, MPI_INT, &rec_buf1, sendcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatterv(&data2, sendcounts, displs, MPI_INT, &rec_buf2, sendcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);




  gettimeofday(&tv1, NULL);

  for(i=0;i<M;i++) {
    result[i]=0;
    for(j=0;j<N;j++) {
      result[i] += base_distance(data1[i*N+j], data2[i*N+j]);
    }
  }

  gettimeofday(&tv2, NULL);
    
  int microseconds = (tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec);

  /* Display result */
  if (DEBUG == 1) {
    int checksum = 0;
    for(i=0;i<M;i++) {
      checksum += result[i];
    }
    printf("Checksum: %d\n ", checksum);
  } else if (DEBUG == 2) {
    for(i=0;i<M;i++) {
      printf(" %d \t ",result[i]);
    }
  } else {
    printf ("Time (seconds) = %lf\n", (double) microseconds/1E6);
  }    

  free(data1); free(data2); free(result);

  return 0;
}