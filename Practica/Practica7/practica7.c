#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 6

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int mirango, tamano;
    MPI_Comm_rank(MPI_COMM_WORLD, &mirango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);

    // Medición del tiempo de inicio
    double tiempo_inicio, tiempo_final;
    tiempo_inicio = MPI_Wtime();

    // Verificamos que el número de procesos sea exactamente 3
    if (tamano != 3) {
        if (mirango == 0) {
            printf("Error: Este programa requiere exactamente 3 procesos.\n");
        }
        MPI_Finalize();
        return 1;
    }


    int matriz[N][N], matrizSup[N], matrizInf[N];
    MPI_Datatype types[N], typeSup, typeInf;
    MPI_Aint desplazamientoSup[N], desplazamentoInf[N];
    MPI_Status status1, status2;


    for (int i = 0; i < N; i++) {
        desplazamientoSup[i] = ((N + 1) * i) * sizeof(int);
        desplazamentoInf[i] = (N * i) * sizeof(int);
        matrizSup[i] = N - i;
        matrizInf[i] = i + 1;
        types[i] = MPI_INT;
    }

    MPI_Type_struct(N, matrizInf, desplazamentoInf, types, &typeInf);
    MPI_Type_commit(&typeInf);
    MPI_Type_struct(N, matrizSup, desplazamientoSup, types, &typeSup);
    MPI_Type_commit(&typeSup);


    if (mirango == 0) {
        // Rellenamos y mostramos la matriz original
        srand(time(NULL)); // Inicializamos el generador de números aleatorios
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matriz[i][j] = rand() % 9 + 1;
            }
        }

        printf("\nMatriz:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d\t", matriz[i][j]);
            }
            printf("\n");
        }

        // Envío de las matrices a los procesos 1 y 2
        MPI_Send(matriz, 1, typeSup, 1, 0, MPI_COMM_WORLD);
        MPI_Send(matriz, 1, typeInf, 2, 1, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    int flag = 1;
    // Procesos 1 y 2 reciben y procesan las matrices
    if (mirango == 1 || mirango == 2) {
        // Inicializamos la matriz con ceros
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matriz[i][j] = 0;
            }
        }

        if (mirango == 1) {
            MPI_Recv(matriz, 1, typeSup, 0, 0, MPI_COMM_WORLD, &status1);
            printf("\nMatriz Superior:\n");
        } else {
            MPI_Recv(matriz, 1, typeInf, 0, 1, MPI_COMM_WORLD, &status2);
            MPI_Recv(&flag, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("\nMatriz Inferior:\n");
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d\t", matriz[i][j]);
            }
            printf("\n");
        }
        if(mirango == 1){
            MPI_Send(&flag, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);

        }
    }

    // Medición del tiempo de finalización y cálculo del tiempo total
    tiempo_final = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);

    if (mirango == 0) {
        printf("Tiempo de ejecución: %f segundos\n", tiempo_final - tiempo_inicio);
    }
    MPI_Type_free(&typeSup);
    MPI_Type_free(&typeInf);

    MPI_Finalize();
    return 0;
}
