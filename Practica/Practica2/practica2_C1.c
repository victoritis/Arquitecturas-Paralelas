#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//En este programa el proceso 0 no realiza ningun calculo de suma de filas de matrices

int main (int argc,char *argv[]){

	int mirango,tamano;
	int longitud, i;
	char nombre[32];
	double start_time, end_time;

	MPI_Init (&argc,&argv);
	start_time = MPI_Wtime();

	MPI_Status status;

	MPI_Comm_rank (MPI_COMM_WORLD,&mirango);
	MPI_Comm_size (MPI_COMM_WORLD,&tamano);
	MPI_Get_processor_name (nombre,&longitud);
	int n = tamano;
	int MA[n*n];
  int MB[n*n];
  int MR[n];
	int MS[n*n];

	if (mirango == 0){
		printf("En este programa el proceso 0 realiza el calculo de suma de filas de matrices\n\n\n");


    // Sembrar la semilla para generar números aleatorios
    srand(time(NULL));

    // Rellenar la matriz A y B con números aleatorios entre -10 y 10
    for (int i = 0; i < n*n; i++) {
          MA[i] = rand() % 21 - 10; // Genera números entre -10 y 10 para la matriz A
          MB[i] = rand() % 21 - 10; // Genera números entre -10 y 10 para la matriz B
      }
		//IMPRIMIR MATRICES
       printf("Matriz A\tMatriz B\n");
       for (int i = 0; i < n; i++) {
           for (int j = 0; j < n; j++) {
               printf("%d ", MA[i * n + j]);
           }
           printf("\t\t");
           for (int j = 0; j < n; j++) {
               printf("%d ", MB[i * n + j]);
           }
           printf("\n");
       }
       printf("\n");
	}

  //Esperamos que todos los procesos lleguen a este punto
  MPI_Barrier(MPI_COMM_WORLD);

  //Enviamos desde el proceso root al resto de procesos la matriz MA,MB, MR, MS
  MPI_Bcast(MA, n * n , MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(MB, n * n , MPI_INT, 0, MPI_COMM_WORLD);
	// Cada proceso realiza la suma de su fila correspondiente


	int fila_a_sumar = mirango; // Fila a sumar para el proceso mirango

	for (int j = 0; j < n; j++) {
			MR[j] = MA[fila_a_sumar * n + j ] + MB[fila_a_sumar * n + j ];
	}


  // Sincronización para asegurarse de que todos los cálculos se completen antes de continuar
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Gather(MR, n, MPI_INT, MS, n, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	// El proceso 0 imprime los resultados

	if (mirango == 0) {
			printf("Matriz MR (Resultados de la suma de matrices):\n");
			for (int i = 0; i < n; i++) {
					for (int j = 0; j < n; j++) {
							printf("%d ", MS[i * n + j ]);
					}
					printf("\n");
			}
			end_time = MPI_Wtime();
			double elapsed_time = end_time - start_time;
    	printf("\n\n\nTiempo de cómputo en paralelo es: %f segundos\n", elapsed_time);	//El ultimo proceso en ejecutarse va a ser 0 porque antes de el hay una barrera
	}



	MPI_Barrier(MPI_COMM_WORLD);

	fflush(stdout);
	MPI_Finalize();
	return 0;
	}
