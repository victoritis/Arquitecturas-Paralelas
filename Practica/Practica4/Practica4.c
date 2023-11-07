#include <mpi.h>
#include <stdio.h>
#include<stdlib.h>
#include<time.h>

void encontrarMatrizMasCuadrada(int numero, int *dim1, int *dim2);


int main(int argc,char *argv[]) {


    double start_time, end_time;
    int mirango,tamano;

    // Inicializar el entorno MPI
    MPI_Init(&argc, &argv);
    int dim1, dim2;
    double inicio;
    // Obtener el rango (identificador) de este proceso
    MPI_Comm_rank(MPI_COMM_WORLD, &mirango);

    if (mirango == 0) {
      start_time = MPI_Wtime();
    }

    // Definir el número de procesos
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);
    encontrarMatrizMasCuadrada(tamano, &dim2, &dim1);
    int dim_size[2] = { dim2,dim1 };
    int period = 1;
    MPI_Comm comm2;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dim_size, &period, 1, &comm2);

    int numero_a_enviar;
    int MA[dim1*dim2];
    int MB[dim1*dim2];

  	//Inicializacion de V1 y V2
  	if(mirango==0){
      printf("%d\n%d",dim1,dim2);
      // Sembrar la semilla para generar números aleatorios
      srand(time(NULL));

      // Rellenar la matriz A y B con números aleatorios entre -10 y 10
      for (int i = 0; i < dim1*dim2; i++) {
            MA[i] = rand() % 21 - 10; // Genera números entre -10 y 10 para la matriz A
            MB[i] = rand() % 21 - 10; // Genera números entre -10 y 10 para la matriz B
        }
      //IMPRIMIR MATRICES
         printf("Matriz A\tMatriz B\n");
         for (int i = 0; i < dim2; i++) {
             for (int j = 0; j < dim1; j++) {
                 printf("%d ", MA[i * dim1 + j]);
             }
             printf("\t\t");
             for (int j = 0; j < dim1; j++) {
                 printf("%d ", MB[i * dim1 + j]);
             }
             printf("\n");
         }
         printf("\n");

       }
    //Esperamos que todos los procesos lleguen a este punto
    MPI_Barrier(MPI_COMM_WORLD);

    //Enviamos desde el proceso root al resto de procesos la matriz MA,MB
    MPI_Bcast(MA, dim1 * dim2 , MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(MB, dim1 * dim2 , MPI_INT, 0, MPI_COMM_WORLD);


    int vector_pos[2];
    MPI_Cart_coords(comm2, mirango, 2, vector_pos);

    //calculamos posicion que suma cada proceso
    int posicion = vector_pos[0] * dim1 + vector_pos[1];
    //calculamos suma
    numero_a_enviar = MA[posicion] + MB[posicion];


    int matriz_recibida[dim1 * dim2];


    MPI_Barrier(MPI_COMM_WORLD);
    //recoger datos
    MPI_Gather(&numero_a_enviar, 1, MPI_INT, matriz_recibida, 1, MPI_INT, 0, MPI_COMM_WORLD);



    //imprimir resultado
    if (mirango == 0) {

        printf("\nLa matriz resultado es:\n");
        for (int i = 0; i < dim2; i++) {
            for (int j = 0; j < dim1; j++) {
                printf("|%d|", matriz_recibida[i * dim1 + j]);
            }
            printf("\n");
        }
    }
    if (mirango == 0) {
      end_time = MPI_Wtime();
      double elapsed_time = end_time - start_time;
      printf("\n\n\nTiempo de cómputo en paralelo es: %f segundos\n", elapsed_time);
    }
    // Finalizar el entorno MPI
    MPI_Finalize();

    return 0;
}


//"devuelve" el valor de las dimensiones 1 y 2
void encontrarMatrizMasCuadrada(int numero, int *filas, int *columnas) {
    int dimension = 1;
    int mejorDiferencia = numero;

    for (int i = 1; i <= numero; i++) {
        if (numero % i == 0) {
            int diferencia = abs(i - (numero / i));
            if (diferencia < mejorDiferencia) {
                dimension = i;
                mejorDiferencia = diferencia;
            }
        }
    }

    *filas = dimension;
    *columnas = numero / dimension;
}
