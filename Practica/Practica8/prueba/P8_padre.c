
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    int rangoPadre;
    MPI_Comm intercom, intracom;
    char archivo[] = "execHijo";
    printf("Ingrese el número de procesos hijo a lanzar: ");

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rangoPadre);

    int numHijos;

    scanf("%d", &numHijos);
    numHijos = 2;

    printf("AQUI LLEGA\n");
    MPI_Comm_spawn(archivo, MPI_ARGV_NULL, numHijos, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercom, MPI_ERRCODES_IGNORE);
    MPI_Intercomm_merge(intercom, 0, &intracom);
    printf("AQUI LLEGA\n");
    char mensaje[] = "¿Sabia usted que yo soy tu padre?\n";
    MPI_Bcast(mensaje, strlen(mensaje) + 1, MPI_CHAR, 0, intracom); // Corregido

    MPI_Finalize();
    return 0;
}


