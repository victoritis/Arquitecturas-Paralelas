#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    int mirango;                // Variable para almacenar el rango del proceso
    int numero = 1;             // Número ingresado por el usuario
    int factorial = 1;          // Resultado del cálculo del factorial
    int flag = 0;               // Bandera para verificar si la operación asincrónica ha terminado
    MPI_Status estado;          // Estructura para el estado de la comunicación
    MPI_Request referencia;     // Solicitud de comunicación asincrónica

    MPI_Init(&argc, &argv);      // Iniciar MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &mirango);  // Obtener el rango del proceso

    // Bucle principal: continuar hasta que el usuario ingrese 0
    while (numero != 0) {
        if (mirango == 0) {  // Proceso con rango 0
            // Impresión por pantalla
            printf("\nPor favor, introduzca un número: ");
            fflush(stdout);
            scanf("%d", &numero);  // Leer un número del usuario

            if (numero == 0) {
                printf("\n\nSaliendo...");  // Mensaje de salida si el número es 0
                fflush(stdout);
            }

            // Envío y recepción de datos a otro proceso
            MPI_Isend(&numero, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &referencia);  // Recibir número del proceso con rango 0
            // Mensaje de espera hasta que la operación termine
            while (flag == 0) {
                MPI_Test(&referencia, &flag, &estado);
            }

            MPI_Irecv(&factorial, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &referencia);  // Iniciar la recepción asincrónica

            // Comprobación de la finalización de la recepción asincrónica
            MPI_Test(&referencia, &flag, &estado);  // Verificar si la operación ha terminado

            // Mensaje de espera hasta que la operación termine
            while (flag == 0) {
                printf("\nEsperando...");
                fflush(stdout);
                MPI_Test(&referencia, &flag, &estado);
            }


            // Salida de datos
            printf("\n\nEl número factorial de %d es: %d\n\n", numero, factorial);
            fflush(stdout);

        }

        if (mirango == 1) {  // Proceso con rango 1
            // Recepción del número enviado por el proceso 0
            MPI_Recv(&numero, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &estado);

            // Cálculo del factorial
            for (int i = 1; i <= numero; i++) {
                factorial *= i;
            }

            // Envío del resultado del factorial al proceso 0
            MPI_Send(&factorial, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

            // Restablecer el valor de factorial para futuros cálculos
            factorial = 1;
        }
    }

    MPI_Finalize();  // Finalizar MPI
    return 0;
}
