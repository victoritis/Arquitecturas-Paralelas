#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void inicializarYMostrarMatrices(float **MatrizA, float **MatrizB, float **Producto, int N);
void realizarMultiplicacionMatrices(float **MatrizA, float **MatrizB, float **Producto, int N, int miRango, int tamano);

int main(int argc, char* argv[]) {
    int N = atoi(argv[1]); // Tamaño de la matriz
    int miRango, tamano;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &miRango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);

    // Inicialización de matrices y variables
    float *filaAEnviar = malloc(N * sizeof(float));
    float *filaFinal = malloc(N * sizeof(float));
    float **MatrizA = malloc(N * sizeof(float*));
    float **MatrizB = malloc(N * sizeof(float*));
    float **Producto = malloc(N * sizeof(float*));
    float *pA = malloc(N * N * sizeof(float));
    float *pB = malloc(N * N * sizeof(float));
    float *pP = malloc(N * N * sizeof(float));

    for (int i = 0; i < N; i++) {
        MatrizA[i] = pA + i * N;
        MatrizB[i] = pB + i * N;
        Producto[i] = pP + i * N;
    }

    if (miRango == 0) {
        // Inicialización y visualización de las matrices
        inicializarYMostrarMatrices(MatrizA, MatrizB, Producto, N);
    }

    // Multiplicación de matrices utilizando MPI
    realizarMultiplicacionMatrices(MatrizA, MatrizB, Producto, N, miRango, tamano);

    MPI_Finalize();
    return 0;
}

void inicializarYMostrarMatrices(float **MatrizA, float **MatrizB, float **Producto, int N) {
    // Inicialización de las matrices
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            MatrizA[i][j] = (float)(rand() % 30) / 3;
            MatrizB[i][j] = (float)(rand() % 30) / 3;
            Producto[i][j] = 0;
        }
    }

    // Impresión de Matriz A
    printf("\nMatriz A:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.1f\t", MatrizA[i][j]);
        }
        printf("\n");
    }

    // Impresión de Matriz B
    printf("\nMatriz B:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.1f\t", MatrizB[i][j]);
        }
        printf("\n");
    }
}

void realizarMultiplicacionMatrices(float **MatrizA, float **MatrizB, float **Producto, int N, int miRango, int tamano) {
    MPI_Status estado;
    double inicio, stop;

    // Buffer para enviar y recibir filas
    float* filaAEnviar = malloc(N * sizeof(float));
    float* filaFinal = malloc(N * sizeof(float));

    // Variables de control
    int desp = 0, contador = 0, numeroDeFila = 0, flag = 0;
    int* lista = NULL;

    // Inicio del temporizador para el proceso 0
    if (miRango == 0) {
        inicio = MPI_Wtime();
        lista = malloc(N * sizeof(int));
    }

    // Envío de la matriz B a todos los procesos
    MPI_Bcast(*MatrizB, N * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (miRango == 0) {
        // Proceso 0: Distribuye filas de MatrizA y recoge los resultados
        for (int i = 1; i < tamano; i++) {
            if (desp < N * N) {
                MPI_Send(MatrizA[desp / N], N, MPI_FLOAT, i, 1, MPI_COMM_WORLD);
                lista[i] = desp / N;
                desp += N;
                contador++;
            }
        }

        while (numeroDeFila < N) {
            MPI_Recv(filaFinal, N, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &estado);
            if (estado.MPI_TAG == 1) {
                int fuente = estado.MPI_SOURCE;
                for (int i = 0; i < N; i++) {
                    Producto[lista[fuente]][i] = filaFinal[i];
                }
                numeroDeFila++;
                if (contador < N) {
                    MPI_Send(MatrizA[desp / N], N, MPI_FLOAT, fuente, 1, MPI_COMM_WORLD);
                    lista[fuente] = desp / N;
                    desp += N;
                    contador++;
                } else {
                    MPI_Send(NULL, 0, MPI_FLOAT, fuente, 2, MPI_COMM_WORLD);
                }
            }
        }

        // Impresión de la matriz resultado
        printf("\nMatriz Producto:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%.1f\t", Producto[i][j]);
            }
            printf("\n");
        }

        stop = MPI_Wtime();
        printf("\nTIEMPO DE EJECUCION: %f\n", stop - inicio);
    } else {
        // Procesos con rangos distintos de 0: Reciben, calculan y envían resultados
        while (!flag) {
            MPI_Recv(filaAEnviar, N, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &estado);
            if (estado.MPI_TAG == 1) {
                for (int j = 0; j < N; j++) {
                    filaFinal[j] = 0;
                    for (int i = 0; i < N; i++) {
                        filaFinal[j] += filaAEnviar[i] * MatrizB[i][j];
                    }
                }
                MPI_Send(filaFinal, N, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
            } else if (estado.MPI_TAG == 2) {
                flag = 1;
            }
        }
    }

    // Liberación de recursos
    free(filaAEnviar);
    free(filaFinal);
    if (miRango == 0) {
        free(lista);
    }
}

