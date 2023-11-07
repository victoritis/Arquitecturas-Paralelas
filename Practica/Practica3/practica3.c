#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void imprimirV(int *V1, int *V2, int V_length);
void mostrar_sumas(int mirango, int resultado_proceso, int V_length);

int main (int argc,char *argv[]){

	double start_time, end_time;
	int mirango,tamano;
	int longitud, i;
	char nombre[32];


	MPI_Init (&argc,&argv);
	start_time = MPI_Wtime();

	MPI_Status status;

	MPI_Comm_rank (MPI_COMM_WORLD,&mirango);
	MPI_Comm_size (MPI_COMM_WORLD,&tamano);
	MPI_Get_processor_name (nombre,&longitud);

	int datoV1, datoV2, resultado_proceso;
	int V_length = tamano;
	int *V1 = (int *)malloc(V_length * sizeof(int));
  int *V2 = (int *)malloc(V_length * sizeof(int));
	int R;


	//Inicializacion de V1 y V2
	if(mirango==0){
		srand(time(NULL));
    for (i = 0; i < V_length; i++) {
        V1[i] = rand() % 21 - 10;
        V2[i] = rand() % 21 - 10;
        }
		imprimirV(V1,V2,V_length);
	}
	MPI_Scatter(V1, 1, MPI_INT, &datoV1, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(V2, 1, MPI_INT, &datoV2, 1, MPI_INT, 0, MPI_COMM_WORLD);

	resultado_proceso = datoV1 * datoV2;

	//MPI_Barrier(MPI_COMM_WORLD);

	mostrar_sumas(mirango, resultado_proceso, V_length);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&resultado_proceso, &R, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (mirango == 0) {
		printf("\n\nEl resultado de la suma es : %d\n", R);
		end_time = MPI_Wtime();
		double elapsed_time = end_time - start_time;
		printf("\n\n\nTiempo de cómputo en paralelo es: %f segundos\n", elapsed_time);
	}

	fflush(stdout);
	MPI_Finalize();
	free(V1);
	free(V2);
	return 0;
	}

	void mostrar_sumas(int mirango, int resultado_proceso, int V_length) {
	    for (int i = 0; i < V_length; i++) {
	        if (mirango == i) {
	            if (i == 0) {
	                printf("\nLa suma de cada posición por cada proceso es:\n");
	            }
	            printf("%d ", resultado_proceso);
	        }
	        MPI_Barrier(MPI_COMM_WORLD);
	    }
	}



	//Funcion para imprimir los Vectores
	void imprimirV(int *V1, int *V2, int V_length) {
	    int i;

	    printf("Array 1 : {");
	    for (i = 0; i < V_length; i++) {
	        if (i + 1 == V_length) {
	            printf(" %d ", V1[i]);
	        } else {
	            printf(" %d :", V1[i]);
	        }
	    }
	    printf("}\n");

	    printf("Array 2 : {");
	    for (i = 0; i < V_length; i++) {
	        if (i + 1 == V_length) {
	            printf(" %d ", V2[i]);
	        } else {
	            printf(" %d :", V2[i]);
	        }
	    }
	    printf("}\n");
	}
