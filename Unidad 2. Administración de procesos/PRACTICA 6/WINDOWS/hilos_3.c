#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <windows.h>
#include <string.h>

/**
 * Definiciones de constantes para el tamaño de la matriz
 * 
 * N: Tamaño de la matriz.
 */
#define N 10

// Declaración de matrices globales
int Matrizuno[N][N];
int Matrizdos[N][N];
int Resultado[N][N];
int TraspuestaUno[N][N];
int TraspuestaDos[N][N];
double InversaUno[N][N];
double InversaDos[N][N];

/**
 * Llena una matriz con valores aleatorios entre 0 y 100.
 * 
 * @param matriz La matriz a llenar.
 */
void LlenarMatriz(int matriz[N][N]) 
{
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            matriz[i][j] = rand() % 101; // Genera un número aleatorio entre 0 y 100
        }
    }
}

/**
 * Imprime una matriz de enteros.
 * 
 * @param matriz La matriz de enteros a imprimir.
 */
void ImprimirMatriz(int matriz[N][N]) 
{
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            printf("%d\t", matriz[i][j]); // Imprime cada elemento de la matriz con tabulación
        }
        printf("\n"); // Nueva línea al final de cada fila
    }
}

/**
 * Guarda una matriz en un archivo dentro del directorio 'resultados'.
 * 
 * @param matriz Puntero a la matriz que se desea guardar.
 * @param nombre_archivo Nombre del archivo donde se guardará la matriz, sin la ruta.
 * @param operacion Descripción de la operación realizada que se incluirá en el archivo.
 * @param tipo Tipo de matriz ('i' para enteros, 'd' para dobles).
 */
void Archivo(void *matriz, char *nombre_archivo, char *operacion, char tipo) 
{
    // Crear el directorio 'resultados' si no existe
    struct stat st = {0};
    if (stat("resultados", &st) == -1) 
    {
        mkdir("resultados", 0700); // Crea el directorio con permisos de lectura/escritura/ejecución para el usuario
    }

    // Crear la ruta completa para el archivo
    char ruta_archivo[256];
    snprintf(ruta_archivo, sizeof(ruta_archivo), "resultados/%s", nombre_archivo);

    // Abrir el archivo para escribir
    FILE *archivo = fopen(ruta_archivo, "w");
    if (archivo == NULL) 
    {
        printf("Error al abrir el archivo %s\n", nombre_archivo);
        exit(1); // Terminar el programa si hay un error al abrir el archivo
    }

    // Escribir la descripción de la operación en el archivo
    fprintf(archivo, "\n--- Operacion---: %s\n", operacion);  

    // Escribir los datos de la matriz en el archivo dependiendo del tipo
    if (tipo == 'i') 
    {
        int (*matriz_int)[N] = (int (*)[N]) matriz;
        for (int i = 0; i < N; i++) 
        {
            for (int j = 0; j < N; j++) 
            {
                fprintf(archivo, "%d ", matriz_int[i][j]);  
            }
            fprintf(archivo, "\n");  
        }
    } 
    else if (tipo == 'd') 
    {
        double (*matriz_double)[N] = (double (*)[N]) matriz;
        for (int i = 0; i < N; i++) 
        {
            for (int j = 0; j < N; j++) 
            {
                fprintf(archivo, "%lf ", matriz_double[i][j]);  
            }
            fprintf(archivo, "\n");  
        }
    }

    fclose(archivo); // Cerrar el archivo
}

/**
 * Hilo para sumar las matrices.
 */
DWORD WINAPI hilosuma(LPVOID lpParam)
{
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            Resultado[i][j] = Matrizuno[i][j] + Matrizdos[i][j]; // Sumar los elementos correspondientes
        }
    }
    Archivo(Resultado, "Suma.txt", "Suma de matrices", 'i'); // Guardar el resultado en un archivo
    return 0;
}

/**
 * Hilo para restar las matrices.
 */
DWORD WINAPI hiloresta(LPVOID lpParam)
{
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            Resultado[i][j] = Matrizuno[i][j] - Matrizdos[i][j]; // Restar los elementos correspondientes
        }
    }
    Archivo(Resultado, "Resta.txt", "Resta de matrices", 'i'); // Guardar el resultado en un archivo
    return 0;
}

/**
 * Hilo para multiplicar las matrices.
 */
DWORD WINAPI hilomultiplicacion(LPVOID lpParam)
{
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            Resultado[i][j] = 0; // Inicializar el resultado en 0
            for (int k = 0; k < N; k++) 
            {
                Resultado[i][j] += Matrizuno[i][k] * Matrizdos[k][j]; // Multiplicar y acumular
            }
        }
    }
    Archivo(Resultado, "Multiplicacion.txt", "Multiplicacion de matrices", 'i'); // Guardar el resultado en un archivo
    return 0;
}

/**
 * Hilo para calcular la transpuesta de las matrices.
 */
DWORD WINAPI hilotraspuesta(LPVOID lpParam)
{
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            TraspuestaUno[j][i] = Matrizuno[i][j]; // Calcular la transpuesta de la primera matriz
            TraspuestaDos[j][i] = Matrizdos[i][j]; // Calcular la transpuesta de la segunda matriz
        }
    }
    Archivo(TraspuestaUno, "Transpuesta1.txt", "Traspuesta de la Matriz 1", 'i'); // Guardar la primera transpuesta en un archivo
    Archivo(TraspuestaDos, "Transpuesta2.txt", "Traspuesta de la Matriz 2", 'i'); // Guardar la segunda transpuesta en un archivo
    return 0;
}

// ***** PROCESO PARA CALCULAR LA INVERSA DE UNA MATRIZ *****

// Estructura para almacenar los datos necesarios para calcular la inversa
typedef struct 
{
    int matriz[N][N];
    double inversa[N][N];
    double determinante;
} DatosInversa;

/**
 * Calcula el cofactor de una matriz cuadrada.
 *
 * Esta función toma una matriz cuadrada de tamaño N y calcula el cofactor
 * correspondiente a la posición (p, q). El cofactor se calcula eliminando la
 * fila p y la columna q de la matriz original y almacenando el resultado en
 * la matriz temporal.
 *
 * @param matriz La matriz original de tamaño N x N.
 * @param temp La matriz temporal de tamaño (N-1) x (N-1) donde se almacenará el cofactor.
 * @param p La fila de la matriz a eliminar.
 * @param q La columna de la matriz a eliminar.
 * @param n El tamaño de la matriz.
 */
void Cofactor(int matriz[N][N], int temp[N][N], int p, int q, int n) 
{
    for (int fila = 0; fila < n; fila++) 
    {
        for (int col = 0; col < n; col++) 
        {
            if (fila != p && col != q) 
            {
                temp[fila < p ? fila : fila - 1][col < q ? col : col - 1] = matriz[fila][col]; // Copiar los elementos que no están en la fila y columna eliminada
            }
        }
    }
}

/**
 * Calcula el determinante de una matriz cuadrada utilizando el enfoque recursivo.
 * 
 * @param matriz La matriz cuadrada de la que se desea calcular el determinante.
 * @param n El tamaño de la matriz.
 * @return El determinante de la matriz.
 */
int DeterminanteeMatriz(int matriz[N][N], int n)
{
    if (n == 1)
        return matriz[0][0];  // Si la matriz es 1x1, el determinante es el único elemento

    int determinantee = 0;
    int temp[N][N];  // Matriz temporal para almacenar los cofactores
    int signo = 1;

    for (int f = 0; f < n; f++)
    {
        Cofactor(matriz, temp, 0, f, n);  // Calcular el cofactor
        determinantee += signo * matriz[0][f] * DeterminanteeMatriz(temp, n - 1);  // Sumar al determinante
        signo = -signo;  // Alternar el signo
    }
    return determinantee;  // Devolver el determinante
}

/**
 * Calcula la matriz adjunta de una matriz dada.
 * 
 * @param matriz La matriz de entrada.
 * @param adjunta La matriz adjunta resultante.
 * @param n El tamaño de la matriz.
 */
void AdjuntaMatriz(int matriz[N][N], double adjunta[N][N], int n)
{
    int temp[N][N];  // Matriz temporal para almacenar cofactores
    int signo;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            Cofactor(matriz, temp, i, j, n);  // Calcular el cofactor
            signo = ((i + j) % 2 == 0) ? 1 : -1;  // Calcular el signo
            adjunta[j][i] = signo * DeterminanteeMatriz(temp, n - 1);  // Asignar el cofactor a la matriz adjunta
        }
    }
}

/**
 * Calcula la inversa de una matriz.
 */
DWORD WINAPI calcular_inversa(LPVOID lpParam)
{
    DatosInversa *datos = (DatosInversa *)lpParam; // Cast del argumento a DatosInversa
    double determinantee = (double)DeterminanteeMatriz(datos->matriz, N); // Calcular el determinante
    datos->determinante = determinantee;

    if (determinantee == 0) 
    {
        printf("La matriz es singular, no se puede calcular la inversa.\n"); // No se puede calcular la inversa si el determinante es cero
        return 0;
    }

    double adjunta[N][N];
    AdjuntaMatriz(datos->matriz, adjunta, N); // Calcular la adjunta

    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            datos->inversa[i][j] = adjunta[i][j] / determinantee; // Calcular la inversa
        }
    }

    return 0;
}

/**
 * Hilo para calcular la inversa de las matrices.
 */
DWORD WINAPI hiloinversa(LPVOID lpParam)
{
    DatosInversa datos1 = { .matriz = {0}, .inversa = {0}, .determinante = 0 };
    DatosInversa datos2 = { .matriz = {0}, .inversa = {0}, .determinante = 0 };

    memcpy(datos1.matriz, Matrizuno, sizeof(Matrizuno));  // Copiar la primera matriz
    memcpy(datos2.matriz, Matrizdos, sizeof(Matrizdos));  // Copiar la segunda matriz

    HANDLE hThread1 = CreateThread(NULL, 0, calcular_inversa, &datos1, 0, NULL);  // Crear hilo para calcular la inversa de la primera matriz
    HANDLE hThread2 = CreateThread(NULL, 0, calcular_inversa, &datos2, 0, NULL);  // Crear hilo para calcular la inversa de la segunda matriz

    WaitForSingleObject(hThread1, INFINITE);  // Esperar a que termine el hilo de la primera matriz
    WaitForSingleObject(hThread2, INFINITE);  // Esperar a que termine el hilo de la segunda matriz

    if (datos1.determinante != 0)
    {
        Archivo(datos1.inversa, "Inversa1.txt", "Inversa de la Matriz 1", 'd');  // Guardar la inversa de la primera matriz en un archivo
    }
    if (datos2.determinante != 0)
    {
        Archivo(datos2.inversa, "Inversa2.txt", "Inversa de la Matriz 2", 'd');  // Guardar la inversa de la segunda matriz en un archivo
    }

    return 0;
}

/**
 * Muestra el contenido de un archivo en la consola.
 * 
 * @param archivo Dirección relativa del archivo a mostrar.
 */
void mostrar(char *archivo)
{
    FILE *file = fopen(archivo, "r");
    if (file == NULL)
    {
        printf("Error al abrir el archivo %s\n", archivo);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        printf("%s", line);
    }

    fclose(file);
}

/**
 * Hilo para manejar la ejecución de los archivos.
 */
DWORD WINAPI hiloarchivos(LPVOID lpParam)
{
    mostrar("resultados/Suma.txt");
    mostrar("resultados/Resta.txt");
    mostrar("resultados/Multiplicacion.txt");
    mostrar("resultados/Transpuesta1.txt");
    mostrar("resultados/Transpuesta2.txt");
    mostrar("resultados/Inversa1.txt");
    mostrar("resultados/Inversa2.txt");

    return 0;
}

/**
 * Función principal que inicializa las matrices, crea hilos y gestiona la ejecución.
 */
int main(void)
{
    srand(time(NULL));  // Inicializa la semilla para números aleatorios

    LlenarMatriz(Matrizuno);  // Llena la primera matriz
    LlenarMatriz(Matrizdos);  // Llena la segunda matriz

    printf("Matriz 1:\n");
    ImprimirMatriz(Matrizuno);  // Imprime la primera matriz
    printf("\nMatriz 2:\n");
    ImprimirMatriz(Matrizdos);  // Imprime la segunda matriz

    // Declaración de variables para los hilos
    DWORD idHilo;
    HANDLE manHilo;

    // Crear hilos para las operaciones de matrices
    manHilo = CreateThread(NULL, 0, hilosuma, NULL, 0, &idHilo);  // Crear hilo para sumar las matrices
    manHilo = CreateThread(NULL, 0, hiloresta, NULL, 0, &idHilo);  // Crear hilo para restar las matrices
    manHilo = CreateThread(NULL, 0, hilomultiplicacion, NULL, 0, &idHilo);  // Crear hilo para multiplic
    manHilo = CreateThread(NULL, 0, hilotraspuesta, NULL, 0, &idHilo);  // Crear hilo para calcular
    manHilo = CreateThread(NULL, 0, hiloinversa, NULL, 0, &idHilo);  // Crear hilo para calcular

    // Esperar a que todos los hilos terminen
    WaitForSingleObject(manHilo, INFINITE);

    // Crear un hilo para mostrar los resultados guardados
    manHilo = CreateThread(NULL, 0, hiloarchivos, NULL, 0, &idHilo);
    WaitForSingleObject(manHilo, INFINITE);

    return 0;
}