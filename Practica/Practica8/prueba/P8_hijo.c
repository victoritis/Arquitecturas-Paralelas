#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    MPI_Comm intercom, intracom;
    int rangoHijo, tamano;

    MPI_Init(&argc, &argv);
    MPI_Comm_get_parent(&intercom);
    MPI_Intercomm_merge(intercom, 1, &intracom);
    MPI_Comm_rank(intracom, &rangoHijo);
    MPI_Comm_size(intracom, &tamano);

    char mensajePadre[100];
    MPI_Bcast(mensajePadre, 100, MPI_CHAR, 0, intracom);
    printf("Proceso hijo %d de %d recibió el mensaje: %s\n", rangoHijo, tamano, mensajePadre);

    if (rangoHijo == 1) {
        // El hijo de menor rango envía un mensaje a los demás
        printf("EL HIJO SE GENERO CORRECTAMENTE");
        char mensajeHijo[] = "Soy el hijo de menor rango enviando un saludo!";
        for (int i = 1; i < tamano; i++) {
            MPI_Send(mensajeHijo, strlen(mensajeHijo) + 1, MPI_CHAR, i, 0, intracom);
        }
    } else {
        // Los demás hijos reciben el mensaje
        char mensajeRecibido[100];
        MPI_Recv(mensajeRecibido, 100, MPI_CHAR, 0, 0, intracom, MPI_STATUS_IGNORE);
        printf("Proceso hijo %d recibió el mensaje del hijo de menor rango: %s\n", rangoHijo, mensajeRecibido);
    }

    MPI_Finalize();
    return 0;
}

