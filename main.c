#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"
#include "list.h"

typedef struct
{
    int id;
    int cantidadPalabras;
    int cantidadCaracteres;
    char *titulo;
    TreeMap *mapaPalabras;
} tipoLibro;

typedef struct
{
    int totalArchivos;
    List *listaLibros;
    TreeMap *mapaTitulos;
} tipoLibreria;

//Función para poder hacer los "createMap"
int lower_than_string(void* key1, void* key2)
{
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}

tipoLibreria* crearLibreria()
{
    tipoLibreria *aux = (tipoLibreria*) calloc (1, sizeof(tipoLibreria));
    aux->listaLibros = createList();
    aux->mapaTitulos = createTreeMap(lower_than_string);
    aux->totalArchivos = 0;
    return aux;
}

void leerChar (char** nombreArchivo)
{
  char nombre[200];
  int largo;
  scanf("%[^\n]s", nombre);
  getchar();
  largo = strlen(nombre) + 1;
  (*nombreArchivo) = (char*) calloc (largo, sizeof(char));
  strcpy((*nombreArchivo), nombre);
}

void mostrarOpcionesMenu()
{
    printf("\n1.- Cargar documentos.\n");
    printf("2.- Mostrar documentos ordenados.\n");
    printf("3.- Buscar libro por título.\n");
    printf("4.- Mostrar las palabras con mayor frecuencia dentro de un libro.\n");
    printf("5.- Mostrar las palabras más relevantes dentro de un libro.\n");
    printf("6.- Buscar una palabra dentro de los libros.\n");
    printf("7.- Mostrar palabra en su contexto dentro del libro.\n");
    printf("8.- Salir del programa.\n");
    printf("Seleccione una opcion.\n");
}

int main() 
{
    tipoLibreria *libreria = crearLibreria(); 
    int opcion;

    while(1)
    {
        do
        {
            mostrarOpcionesMenu();
            scanf("%d", &opcion);
            getchar();
        } while (opcion < 1 || opcion > 8);

        switch (opcion)
        {
            case 1:
            {
                break;
            }
            case 2:
            {
                break;
            }
            case 3:
            {
                break;
            }
            case 4:
            {
                break;
            }
            case 5:
            {
                break;
            }
            case 6:
            {
                break;
            }
            case 7:
            {
                break;
            }
            case 8:
            {
                printf("\nGracias por utilizar el programa\n");
                return 0;
            }
        }

        do
        {
            printf("\n¿Desea continuar?\n");
            printf("1- Si. / 2- NO.\n");
            scanf("%d", &opcion);
            getchar();
        } while (opcion < 1 || opcion > 2);

        if (opcion == 2)
        {
            printf("\nGracias por utilizar el programa\n");
            break;
        }
    }
    return 0;
}