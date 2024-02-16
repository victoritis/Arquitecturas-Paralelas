#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    int rangoPadre;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rangoPadre);

    if (rangoPadre == 0) {
        int numHijos;
        printf("Ingrese el número de procesos hijo a lanzar: ");
        scanf("%d", &numHijos);

        MPI_Comm child;
        int spawnError[numHijos];

        // Lanzar procesos hijo
        MPI_Comm_spawn("./execHijo", MPI_ARGV_NULL, numHijos, MPI_INFO_NULL, 0, MPI_COMM_SELF, &child, spawnError);

	// Verificar errores en el spawn
        for (int i = 0; i < numHijos; ++i) {
            if (spawnError[i] != MPI_SUCCESS) {
                fprintf(stderr, "Error al lanzar el proceso hijo %d\n", i);
                MPI_Abort(MPI_COMM_WORLD, spawnError[i]);
            }
        }

        // Enviar el número de hijos a cada hijo
        for (int i = 0; i < numHijos; i++) {
            MPI_Send(&numHijos, 1, MPI_INT, i, 0, child);
        }


        // Enviar mensaje de saludo a los hijos
        char mensajePadre[] = "Saludos desde el proceso padre.\n";
        for (int i = 0; i < numHijos; i++) {
            printf("Enviando mensaje al proceso hijo %d", i);
            MPI_Send(mensajePadre, strlen(mensajePadre) + 1, MPI_CHAR, i, 0, child);
        }
    }

    MPI_Finalize();
    return 0;
}
