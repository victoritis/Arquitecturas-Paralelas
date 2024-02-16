#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm parent;
    MPI_Comm_get_parent(&parent);

    int myid;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (parent != MPI_COMM_NULL) {
        int numHijos;
        // Recibir el número de procesos hijo
        MPI_Recv(&numHijos, 1, MPI_INT, 0, 0, parent, MPI_STATUS_IGNORE);

        // Recibir mensaje de saludo del padre
        char mensajePadre[100];
        MPI_Recv(mensajePadre, 100, MPI_CHAR, 0, 0, parent, MPI_STATUS_IGNORE);
        printf("Mensaje del padre: %s\n", mensajePadre);

        // El proceso con el menor rango envía un mensaje a los demás
        if (myid == 0) {
            char mensajeHijo[] = "Saludos del hijo de menor rango.\n";
            for (int i = 1; i < numHijos; i++) {
                printf("Enviando mensaje al proceso hermano mayor %d\n", i);
                MPI_Send(mensajeHijo, strlen(mensajeHijo) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            }
        } else {
            char mensajeHermano[100];
            MPI_Recv(mensajeHermano, 100, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Mensaje recibido del hermano: %s\n", mensajeHermano);
        }
    }

    MPI_Finalize();
    return 0;
}
