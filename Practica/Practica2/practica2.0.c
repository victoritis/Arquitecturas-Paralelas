#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main (int argc,char *argv[]){

	int numero_enviado;
    	//printf("Elige un numero para ser enviado : \n");
   	//scanf("%d", &numero_enviado);

	int mirango,tamano;
	int longitud, i;
	char nombre[32];

	MPI_Init (&argc,&argv);

	MPI_Status status;

	MPI_Comm_rank (MPI_COMM_WORLD,&mirango);
	MPI_Comm_size (MPI_COMM_WORLD,&tamano);
	MPI_Get_processor_name (nombre,&longitud);
	int n = tamano - 1;
	int MA[n*n+n];
  int MB[n*n+n];
  int MR[n*n+n];
	int MS[n*n+n];

	if (mirango == 0){
		//INICIALIZAMOS LAS MATRICES A 0
	/*	for (int i = 0; i < n * n + n; i++) {
				MS[i] = 0;
		}
		for (int i = 0; i < n * n + n; i++) {
				MR[i] = 0;
		}
*/
    // Sembrar la semilla para generar números aleatorios
    srand(time(NULL));
		// Initialize all elements to 0


    // Rellenar la matriz A y B con números aleatorios entre -10 y 10
    for (int i = 0; i < n*n; i++) {
          MA[i + n] = rand() % 21 - 10; // Genera números entre -10 y 10 para la matriz A
          MB[i + n] = rand() % 21 - 10; // Genera números entre -10 y 10 para la matriz B
      }
		//IMPRIMIR MATRICES
       printf("Matriz A\tMatriz B\n");
       for (int i = 0; i < n; i++) {
           for (int j = 0; j < n; j++) {
               printf("%d ", MA[i * n + j + n]);
           }
           printf("\t\t");
           for (int j = 0; j < n; j++) {
               printf("%d ", MB[i * n + j + n]);
           }
           printf("\n");
       }
       printf("\n");
	}

  //Esperamos que todos los procesos lleguen a este punto
  MPI_Barrier(MPI_COMM_WORLD);

  //Enviamos desde el proceso root al resto de procesos la matriz MA,MB, MR, MS
  MPI_Bcast(MA, n * n + n, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(MB, n * n + n, MPI_INT, 0, MPI_COMM_WORLD);
	//MPI_Bcast(MR, n * n + n, MPI_INT, 0, MPI_COMM_WORLD);
	// Cada proceso realiza la suma de su fila correspondiente
	if (mirango > 0) {
			int fila_a_sumar = mirango - 1; // Fila a sumar para el proceso mirango
			for (int j = 0; j < n; j++) {
					MR[fila_a_sumar * n + j + n] = MA[fila_a_sumar * n + j + n] + MB[fila_a_sumar * n + j + n];
			}
	}


  // Sincronización para asegurarse de que todos los cálculos se completen antes de continuar
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Gather(&MR[mirango*n], n, MPI_INT, &MS, n, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	// El proceso 0 imprime los resultados
	if (mirango == 0) {
			printf("Matriz MR (Resultados de la suma por filas):\n");
			for (int i = 0; i < n; i++) {
					for (int j = 0; j < n; j++) {
							printf("%d ", MS[i * n + j + n]);
					}
					printf("\n");
			}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	fflush(stdout);
	MPI_Finalize();
	return 0;
	}
