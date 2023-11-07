#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> // Agregamos la inclusión de <stdlib.h> para usar la función remove
#include<time.h>
#define N 3

int main(int argc, char* argv[]) {
  double start_time, end_time;
  int mirango, tamano;
  int lectura[N];
  int escritura[N];
  MPI_Status estado;
  MPI_File nuevo;

  // Eliminamos el archivo existente para evitar sobrescribirlo
  //remove("file.txt");

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mirango);
  MPI_Comm_size(MPI_COMM_WORLD, &tamano);

  if (mirango == 0) {
    start_time = MPI_Wtime();
  }

  char representacion[] = "native";
  char fichero[] = "file.txt";

  for (int i = 0; i < N; i++) {
    escritura[i] = mirango + 48; // Asignamos un valor a escritura
  }

  // Escritura de datos en un archivo
  MPI_File_open(MPI_COMM_WORLD, fichero, MPI_MODE_CREATE | MPI_MODE_WRONLY,
                MPI_INFO_NULL, &nuevo);
  MPI_File_set_view(nuevo, mirango * tamano * sizeof(int), MPI_INT, MPI_INT,
                   representacion, MPI_INFO_NULL);
  MPI_File_write_at(nuevo, mirango * tamano * sizeof(int), escritura, N, MPI_INT, &estado);
  MPI_File_close(&nuevo);

  // Lectura de datos desde el archivo
  MPI_File_open(MPI_COMM_WORLD, fichero, MPI_MODE_RDONLY, MPI_INFO_NULL, &nuevo);
  MPI_File_set_view(nuevo, mirango * tamano * sizeof(int), MPI_INT, MPI_INT, representacion, MPI_INFO_NULL);
  MPI_File_read_at(nuevo, mirango * tamano * sizeof(int), lectura, N, MPI_INT, &estado);
  MPI_File_close(&nuevo);


  // Salida por pantalla
  for(int i=0;i<tamano;i++){
    if(mirango==i){
      printf("[Proceso %d]: ", mirango); // Corregimos %s a %d para mostrar el identificador del proceso
      for (int j = 0; j < N; j++) {
        printf("%d\t", lectura[j] - 48); // Corregimos para mostrar los valores de lectura
        fflush(stdout);
      }
      printf("\n"); // Salto de línea para que se vea mejor
    }
    //MPI_Barrier(MPI_COMM_WORLD); // Todos los procesos esperarán aquí
  }

  if (mirango == 0) {
    end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    printf("\n\n\nTiempo de cómputo en paralelo es: %f segundos\n", elapsed_time);
  }


  // Finalización de MPI
  MPI_Finalize();

  return 0;
}
