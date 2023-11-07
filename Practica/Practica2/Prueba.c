#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int mirango, tamano;
    int longitud;
    char nombre[32];
    int cantidad = 1;
    int n = 5, m = 4;
    int MA[n][m], MB[n][m];

    MPI_Init(&argc, &argv);

    MPI_Status status;

    MPI_Comm_rank(MPI_COMM_WORLD, &mirango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);
    MPI_Get_processor_name(nombre, &longitud);

    if (mirango == 0) {
        // Sembrar la semilla para generar números aleatorios
        srand(time(NULL));

        // Rellenar la matriz A y B con números aleatorios entre -10 y 10
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                MA[i][j] = rand() % 21 - 10; // Genera números entre -10 y 10 para la matriz A
                MB[i][j] = rand() % 21 - 10; // Genera números entre -10 y 10 para la matriz B
            }
        }

        // PASAR MATRICES A LOS DEMÁS PROCESOS
        MPI_Bcast(MA, n * m, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        // RECIBIR MATRICES DE OTROS PROCESOS
        MPI_Bcast(MA, n * m, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Imprimir la matriz MA en todos los procesos
    printf("Proceso %d:\n", mirango);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%d ", MA[i][j]);
        }
        printf("\n");
    }

    fflush(stdout);
    MPI_Finalize();
    return 0;
}
