#include <stdio.h>

int main() {

    printf("Ingrese el número de procesos hijo a lanzar: ");


    // Suponemos que el usuario introduce 3
    int numHijos = 3;
    printf("El número de procesos hijo a lanzar es: %d\n", numHijos);


    // Simulación de envío de mensaje a los procesos hijo
    for (int i = 0; i < numHijos; i++) {
        printf("Enviando mensaje al proceso hijo %d\n", i);
    }

    // Suponiendo que los procesos hijo responden
    for (int i = 0; i < numHijos; i++) {
        printf("Proceso hijo %d de %d recibió el mensaje: ¿Sabia usted que yo soy tu padre?\n", i, numHijos);
    }

    // Simulación de los hijos comunicándose entre sí (solo si hay más de 1 hijo)
    if (numHijos > 1) {
        printf("Proceso hijo 0 enviando mensaje a otros hijos.\n");
        for (int i = 1; i < numHijos; i++) {
            printf("Proceso hijo %d recibió el mensaje del hijo de menor rango: Soy el hijo de menor rango enviando un saludo!\n", i);
        }
    }

    return 0;
}

