
//VICTOR GONZALEZ DEL CAMPO

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int rango, tamano, inicio, fin;
    int **matrix;
    int suma_local = 0, suma_total = 0;
    int cuenta_local = 0, cuenta_total = 0;
    double media;
    int N=0; // Tamaño de la matriz
    double S_time, F_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);
    
    if(rango==0){
    
	S_time = MPI_Wtime();    	
    
    }

   // Pedir número al usuario para el tamaño de la matriz (mayor o igual que 10)
	if (rango == 0) {   
	    do {
		printf("Ingrese el tamaño de la matriz (N >= 10): ");
		fflush(stdout);  // Asegura que el mensaje se imprima antes de leer la entrada
		scanf("%d", &N);
	    } while (N < 10);
	}

      
    // Distribuir el tamaño de la matriz a todos los procesos
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Calcular el inicio y el fin de las filas para este proceso
    int filas_por_proceso_base = N / tamano;
    int filas_extra = N % tamano;

    // Todos los procesos manejan al menos 'filas_por_proceso_base' filas
    inicio = rango * filas_por_proceso_base + (rango < filas_extra ? rango : filas_extra);
    fin = inicio + filas_por_proceso_base - 1 + (rango < filas_extra);
	
	
	
    // Inicializar y asignar memoria para la matriz
    matrix = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        matrix[i] = (int *)malloc(N * sizeof(int));
    }

    // Inicialización de la matriz en el proceso 0   
    if (rango == 0) {
        srand(time(NULL));  // Inicialización del generador de números aleatorios
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = rand() % 101;  // Números aleatorios entre 0 y 100
            }
        }
    }

	// Distribución de las filas de la matriz a cada proceso
	if (rango == 0) {
	    for (int i = 0; i < tamano; i++) {
		int inicio_i, fin_i;
		inicio_i = i * filas_por_proceso_base + (i < filas_extra ? i : filas_extra);
		fin_i = inicio_i + filas_por_proceso_base - 1 + (i < filas_extra);

		if (i != 0) {
		    // Enviar cada fila asignada al proceso i
		    for (int j = inicio_i; j <= fin_i; j++) {
		        MPI_Send(matrix[j], N, MPI_INT, i, 0, MPI_COMM_WORLD);
		    }
		}
	    }
	} else {
	    // Recibir las filas asignadas
	    for (int i = inicio; i <= fin; i++) {
		matrix[i] = (int *)malloc(N * sizeof(int));
		MPI_Recv(matrix[i], N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	    }
	}

	MPI_Barrier(MPI_COMM_WORLD);
	
	// Calcular la suma local y la cuenta de elementos en cada proceso
	for (int i = inicio; i <= fin; i++) {
	    for (int j = 0; j < N; j++) {
		suma_local += matrix[i][j];
	    }
	}
	cuenta_local = (fin - inicio + 1) * N;
	printf("Proceso %d - Suma local: %d, Cuenta local: %d\n", rango, suma_local, cuenta_local);

	// Recolectar las sumas y cuentas locales en el proceso 0
	MPI_Reduce(&suma_local, &suma_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&cuenta_local, &cuenta_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	// Calcular y mostrar el promedio en el proceso 0
	if (rango == 0) {
	    media = (double)suma_total / cuenta_total;
	    printf("Promedio de la matriz: %f\n", media);
	    // Opcional: Mostrar la matriz
	    for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
		    printf("%d ", matrix[i][j]);
		}
		printf("\n");
	    }
	}
    //calcular tiempo ejecucion
    if(rango==0){
	F_time = MPI_Wtime();    
	printf("El tiempo dejecucion en paralelo es: %d", F_time-S_time);	
    }




    // Liberar memoria
    for (int i = 0; i < N; i++) {
        free(matrix[i]);
    }
    free(matrix);

    MPI_Finalize();
    return 0;
}

