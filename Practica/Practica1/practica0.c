#include <mpi.h>
#include <stdio.h>


int main (int argc,char *argv[]){

	int numero_enviado;
	int mirango,tamano;
	int longitud;
	char nombre[32];
	int cantidad = 1;
  int numero_recibido;

	MPI_Init (&argc,&argv);

	MPI_Status status;

	MPI_Comm_rank (MPI_COMM_WORLD,&mirango);
	MPI_Comm_size (MPI_COMM_WORLD,&tamano);
	MPI_Get_processor_name (nombre,&longitud);

	if (mirango == 0){
		if (tamano == 1){
			return 0;
		}
		printf("Elige un numero para ser enviado : \n");
   		scanf("%d", &numero_enviado);
			for (int i = 1; i < tamano; i++) {
				MPI_Send(&numero_enviado, cantidad, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
  }
  if (mirango != 0){
      		MPI_Recv(&numero_recibido, cantidad, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      		printf("\n El numero recibido en el proceso %s %d es : %d ",nombre, mirango , numero_recibido);

  }


	fflush(stdout);
	MPI_Finalize();
	return 0;
	}
