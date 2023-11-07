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
	int cantidad = 1;
	int TamañoMatriz;


	MPI_Init (&argc,&argv);

	MPI_Status status;

	MPI_Comm_rank (MPI_COMM_WORLD,&mirango);
	MPI_Comm_size (MPI_COMM_WORLD,&tamano);
	MPI_Get_processor_name (nombre,&longitud);
	TamañoMatriz = tamano;
	int n = tamano;
	int MA[n][n], MB[n][n], MR[n][n], MS[n];

	if (mirango == 0){

    // Sembrar la semilla para generar números aleatorios
    srand(time(NULL));

    // Rellenar la matriz A y B con números aleatorios entre -10 y 10
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
          MA[i][j] = rand() % 21 - 10; // Genera números entre -10 y 10 para la matriz A
          MB[i][j] = rand() % 21 - 10; // Genera números entre -10 y 10 para la matriz B
        }
      }
    //IMPRIMIR MATRICES
    printf("Matriz A\t\tMatriz B\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", MA[i][j]);
        }
        printf("\t\t");
        for (int j = 0; j < n; j++) {
            printf("%d ", MB[i][j]);
        }
        printf("\n");
    }

	printf("\n");
    }

  //Esperamos que todos los procesos lleguen a este punto
  MPI_Barrier(MPI_COMM_WORLD);
  //Enviamos desde el proceso root al resto de procesos la matriz MA y MB
  MPI_Bcast(MA, n * n, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(MB, n * n, MPI_INT, 0, MPI_COMM_WORLD);


  if (mirango == 1){

		printf("Matriz A\t\tMatriz B del proceso 1\n");
		for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					printf("%d ", MA[i][j]);
				}
				printf("\t\t");
				for (int j = 0; j < n; j++) {
					printf("%d ", MB[i][j]);
				}
				printf("\n");
		}
	}

	// Cada proceso realiza la suma de su fila correspondiente
	if(mirango > 0) {
		int fila_a_sumar = mirango - 1; // Fila a sumar para el proceso mirango
		for (int j = 0; j < n; j++) {
				MR[fila_a_sumar][j] = MA[fila_a_sumar][j] + MB[fila_a_sumar][j];
		}
	}


  // Sincronización para asegurarse de que todos los cálculos se completen antes de continuar
  MPI_Barrier(MPI_COMM_WORLD);

	//QUIERO QUE ENVIE LA FILA CORRESPONDIENTE DE CADA PROCESO AL PROCESO 0
	MPI_Gather(MR[mirango], n, MPI_INT, MR[0], n, MPI_INT, 0, MPI_COMM_WORLD);
	if(mirango > 0){
		// Recopilación de las filas de MR en el proceso 0


	}
	// El proceso 0 imprime los resultados
	if(mirango == 0) {
			printf("Matriz MR (Resultados de la suma por filas):\n");
			for (int i = 0; i < n; i++) {
					for (int j = 0; j < n; j++) {
							printf("%d ", MR[i][j]);
					}
					printf("\n");
			}
	}





	fflush(stdout);
	MPI_Finalize();
	return 0;
	}
