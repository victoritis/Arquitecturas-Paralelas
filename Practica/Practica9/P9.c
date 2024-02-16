#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    int N; // Tamaño de las matrices
    int mirango, tamano;
    MPI_Status estado;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mirango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);

    // El proceso 0 obtiene el tamaño de la matriz de los argumentos o solicita al usuario
    if (mirango == 0) {
        if (argc > 1) {
            N = atoi(argv[1]); // Tamaño de la matriz desde argumento de línea de comando
        } else {
            printf("Introduzca el tamaño de la matriz: ");
            scanf("%d", &N);
        }
    }

    // Se transmite el tamaño de la matriz a todos los procesos
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Declaraciones y reserva dinámica de memoria
    float *pA, *pB, *pP, *filaRecibida, *filaResultado;
    pA = (float*)malloc(N * N * sizeof(float));
    pB = (float*)malloc(N * N * sizeof(float));
    pP = (float*)malloc(N * N * sizeof(float));
    filaRecibida = (float*)malloc(N * sizeof(float));
    filaResultado = (float*)malloc(N * sizeof(float));

   // Semilla para la generación de números aleatorios
	srand(time(NULL) + mirango); // Añadir el rango para asegurar diferentes semillas

	// Inicializar matrices A y B con números aleatorios con decimales
    if (mirango == 0) {
    	for (int i = 0; i < N; i++) {
       	   for (int j = 0; j < N; j++) {
              pA[i * N + j] = (float)rand() / (float)(RAND_MAX) * 10.0; // Números entre 0 y 10
              pB[i * N + j] = (float)rand() / (float)(RAND_MAX) * 10.0; // Números entre 0 y 10
              pP[i * N + j] = 0.0; // Inicializa la matriz de producto a 0
            }
         }
    }
    
    // Distribuir la matriz B a todos los procesos
    MPI_Bcast(pB, N * N, MPI_FLOAT, 0, MPI_COMM_WORLD);


    // Iniciar medición de tiempo
    double tiempo_inicio, tiempo_final;
    if (mirango == 0) {
        tiempo_inicio = MPI_Wtime();
    }
    



    // El proceso 0 reparte las filas de la matriz A entre los demás procesos
    if (mirango == 0) {
        for (int i = 0; i < N; i++) {
            int destino = i % (tamano - 1) + 1; // Determinar el proceso destinatario
            MPI_Send(&pA[i * N], N, MPI_FLOAT, destino, 0, MPI_COMM_WORLD);
        }
    }

    // Todos los procesos, excepto el proceso 0, reciben filas y realizan la multiplicación
    if (mirango != 0) {
        for (int i = 0; i < N; i++) {
            if (i % (tamano - 1) + 1 == mirango) {
                MPI_Recv(filaRecibida, N, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &estado);

                // Realizar la multiplicación de la fila por la matriz B
                for (int j = 0; j < N; j++) {
                    filaResultado[j] = 0.0;
                    for (int k = 0; k < N; k++) {
                        filaResultado[j] += filaRecibida[k] * pB[k * N + j];
                    }
                }

                // Enviar el resultado de la fila al proceso 0
                MPI_Send(filaResultado, N, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
            }
        }
    }

    // El proceso 0 recoge los resultados y los almacena en la matriz Producto
    if (mirango == 0) {
        for (int i = 0; i < N; i++) {
            int fuente = i % (tamano - 1) + 1;
            MPI_Recv(&pP[i * N], N, MPI_FLOAT, fuente, MPI_ANY_TAG, MPI_COMM_WORLD, &estado);
        }
    }
	// Imprimir las matrices con formato
	if (mirango == 0) {
	    printf("Matriz A:\n");
	    for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
		    printf("%10.2f ", pA[i * N + j]);
		}
		printf("\n");
	    }

	    printf("Matriz B:\n");
	    for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
		    printf("%10.2f ", pB[i * N + j]);
		}
		printf("\n");
	    }

	    // Después de la multiplicación y recopilación de resultados
	    printf("Matriz Producto:\n");
	    for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
		    printf("%10.2f ", pP[i * N + j]);
		}
		printf("\n");
	    }
	}

    // Finalizar la medición de tiempo
    if (mirango == 0) {
        tiempo_final = MPI_Wtime();
        printf("Tiempo empleado en la multiplicación: %f segundos\n", tiempo_final - tiempo_inicio);
    }

    // Liberar la memoria asignada
    free(pA);
    free(pB);
    free(pP);
    free(filaRecibida);
    free(filaResultado);

    // Finalizar el entorno MPI
    MPI_Finalize();
    return 0;
}



