#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Definiciones de constantes para el tamaño de la matriz
 * 
 * N: Tamaño de la matriz.
 */
#define N 10


/**
 * Multiplica dos matrices cuadradas y guarda el resultado en una tercera matriz.
 * 
 * @param matrizuno La primera matriz de entrada.
 * @param matrizdos La segunda matriz de entrada.
 * @param resultado La matriz de salida donde se guarda el resultado de la multiplicación.
 */
void MultiplicarMatrices(int matrizuno[N][N], int matrizdos[N][N], int resultado[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            resultado[i][j] = 0; // Inicializa el elemento de resultado a 0.
            for (int k = 0; k < N; k++)
            {
                resultado[i][j] += matrizuno[i][k] * matrizdos[k][j]; // Calcula el producto escalar.
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // Para lectura
    HANDLE hLecturaPipe_hijo_nieto = GetStdHandle(STD_INPUT_HANDLE);
    DWORD lectura;

    // Para escritura
    HANDLE hEscrituraPipe_hijo_nieto = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD escritura;

    int Matrizuno[N][N];
    int Matrizdos[N][N];

    // Leer las matrices del hijo
    ReadFile(hLecturaPipe_hijo_nieto, Matrizuno, sizeof(Matrizuno), &lectura, NULL);
    ReadFile(hLecturaPipe_hijo_nieto, Matrizdos, sizeof(Matrizdos), &lectura, NULL);

    int Resultado[N][N];
    MultiplicarMatrices(Matrizuno,Matrizdos,Resultado);

    //Escribir el resultado en la tuberia
    WriteFile(hEscrituraPipe_hijo_nieto, Resultado, sizeof(Resultado), &escritura, NULL);

    CloseHandle(hLecturaPipe_hijo_nieto);
    CloseHandle(hEscrituraPipe_hijo_nieto);

    return 0;
}