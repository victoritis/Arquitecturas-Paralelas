#include <mpi.h>
#include <stdio.h>
#define N 4
int main(int argc, char* argv[]) {
int mirango, tamano;
int lectura[N];
int escritura[N];
MPI_Status estado;
MPI_File nuevo;
remove("file.txt");
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &mirango);
MPI_Comm_size(MPI_COMM_WORLD, &tamano);
char representacion[] = "native";
char fichero[] = "file.txt";
for (int i = 0;i < N;i++) {
escritura[i] = mirango + 48;
}
//Escritura de datos
MPI_File_open(MPI_COMM_WORLD, fichero, MPI_MODE_CREATE | MPI_MODE_WRONLY,
MPI_INFO_NULL, &nuevo);
MPI_File_set_view(nuevo, mirango * tamano * sizeof(int), MPI_INT, MPI_INT,
representacion, MPI_INFO_NULL);
MPI_File_write_at(nuevo, mirango, escritura, N, MPI_INT, &estado);
MPI_File_close(&nuevo);
//Lectura de datos
MPI_File_open(MPI_COMM_WORLD, fichero, MPI_MODE_RDONLY, MPI_INFO_NULL,
&nuevo);
MPI_File_set_view(nuevo, mirango * tamano * sizeof(int), MPI_INT, MPI_INT,
representacion, MPI_INFO_NULL);
MPI_File_read_at(nuevo, mirango, lectura, N, MPI_INT, &estado);
MPI_File_close(&nuevo);
//Salida por pantalla
printf("[Proceso %s]: ", lectura);
for (int j = 0;j < N;j++) {
printf("%d\t", mirango);
fflush(stdout);
}
printf("\n"); //Salto de linea para que se vea mejor
//Finalización
MPI_Finalize();
return 0;
}
