#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "treemap.h"
#include "map.h"
#include "list.h"

typedef struct 
{
    char *word;
    int cont;
} tipoPalabra;

typedef struct
{
    int id;
    int cantidadPalabras;
    int cantidadCaracteres;
    char *titulo;
} tipoLibro;

typedef struct
{
    int totalArchivos;
    Map *mapaLibros; //Clave: titulo libro ; valor: variable tipo libro     
    Map *mapaTitulos; //Clave: titulo libro ; valor: lista de variables tipo palabra (nombre palabra, cantidad de apariciones)
    Map *mapaPalabras; //Clave: palabra ; valor: lista de libros que contienen esa palabra
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
    aux->mapaLibros = createMap(lower_than_string);
    aux->mapaTitulos = createMap(lower_than_string);
    aux->mapaPalabras = createMap(lower_than_string);
    aux->totalArchivos = 0;
    return aux;
}

void leerChar(char** nombreArchivo)
{
  char nombre[200];
  int largo;
  scanf("%[^\n]s", nombre);
  getchar();
  largo = strlen(nombre) + 1;
  (*nombreArchivo) = (char*) calloc (largo, sizeof(char));
  strcpy((*nombreArchivo), nombre);
}


char* next_word (FILE *f) 
{
    char x[1024];
    //assumes no word exceeds length of 1023 
    if (fscanf(f, " %1023s", x) == 1) return strdup(x);
    else return NULL;
}

/*
int hayQueEliminar(char c, char* string_chars)
{
    for (int i=0 ; i<strlen(string_chars) ; i++)
    {
        if (string_chars[i]==c) return 1;
    }
    return 0;
}

char* quitar_caracteres(char* string, char* c)
{
    int i, j;

    for(i=0 ; i < strlen(string) ; i++)
    {
        if (hayQueEliminar(string[i], c))
        {
            for (j=i ; j<strlen(string)-1 ;j++)
            {
                string[j] = string[j+1];
            }
            string[j]='\0';
            i--;
        }
    }
    return string;
}

void contadorDePalabras(tipoLibreria *libreria, FILE *archivo)
{
    tipoPalabra *palabra, *aux;
    int k, largo;
    char *word = next_word(archivo);

    while (word)
    {
        for (k = 0; word[k]; k++) word[k] = tolower(word[k]);

        word = quitar_caracteres(word, "!?\";");

        if (searchMap(libreria->mapaLibros, word) == NULL)
        {
            palabra = (tipoPalabra*) malloc(1 * sizeof(tipoPalabra));

            largo = strlen(word) + 1;
            palabra->word = (char*) malloc(largo * sizeof(char));

            strcpy(palabra->word, word);
            palabra->cont = 1;

            insertMap(libreria->mapaLibros, word, palabra);
        }
        else
        {
            aux = searchMap(libreria->mapaLibros, word);
            aux->cont++;
        }
        word = next_word(archivo);
    }
}
*/

void cargarDocumentos(tipoLibreria *libreria, char *todosLosArchivos)
{
    const char separador[2] = " ";
    char *archivoActual = strtok(todosLosArchivos, separador);
    char cadena[101];

    if (archivoActual != NULL)
    {
        while (archivoActual != NULL)
        {
            printf("\nNombre documento: %s\n", archivoActual);
                        
            FILE *archivo = fopen(archivoActual, "r");
 
            if (archivo == NULL) 
            {
                printf("No existe este documento.\n");
                archivoActual = strtok(NULL, separador);
                continue;
            }

            tipoLibro *libro = (tipoLibro*) malloc(1 * sizeof(tipoLibro));
            libro->cantidadPalabras = 0;
            printf("%d\n", libro->cantidadPalabras);
            libro->id = atoi(archivoActual);
            printf("%d\n", libro->id);

            /*char x[1024];
            int cont = 0;

            while (1) 
            {
                int pos = ftell(archivo);
                if (fscanf(archivo, " %1023s", x) == 1) cont++;
                //printf("%s: pos %d\n",x,pos);
                else break;
            }*/

            while (strcmp(fgets(cadena, 100, archivo), "\n") != 0)
            {
                int largo = strlen(cadena) + 1;
                printf("%d\n", largo);
                libro->titulo = (char*) malloc(largo * sizeof(char));
                strcpy(libro->titulo, cadena);
                printf("%s\n", libro->titulo);
                insertMap(libreria->mapaLibros, libro->titulo, libro);
                printf("Insertó en el mapa.\n");
                break;
            }

            /*contadorDePalabras(libreria, archivo);
            
            while (fgets(archivoActual, 50, archivo)) 
            {
                printf("%s", archivoActual);
                //Aquí deberiamos empezar a guardar todos los datos que necesitamos de cada archivo
            }*/
                        
            fclose(archivo);
            archivoActual = strtok(NULL, separador);
            printf("Pasó al siguiente token.\n");
        }
    }
}

void mostrarDocumentos(tipoLibreria *libreria)
{
    tipoLibro *aux = firstMap(libreria->mapaLibros);

    while (aux != NULL)
    {
        printf("ID: %d\n", aux->id);
        printf("Título: %s", aux->titulo);
        printf("Cantidad de palabras: %d\n", aux->cantidadPalabras);

        aux = nextMap(libreria->mapaLibros);
        printf("Siguiente key.\n");
    }
}

void buscarPalabra(tipoLibreria *libreria, char *palabra)
{
    if (searchMap(libreria->mapaPalabras, palabra) != NULL)
    {
        List *listaLibros = searchMap(libreria->mapaPalabras, palabra);
        tipoLibro *recorrer = firstList(listaLibros);

        while (recorrer != NULL)
        {
            printf("%d\n", recorrer->id);
            printf("%s\n", recorrer->titulo);
            recorrer = nextList(listaLibros);
        }
    }
    else printf("Ningún libro contiene esta palabra en su contenido.\n");
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
                printf("\nIngrese todos los nombres de los documentos que desea cargar, cada uno separado por un espacio:\n");
                char *todosLosArchivos = NULL;
                leerChar(&todosLosArchivos);
                cargarDocumentos(libreria, todosLosArchivos);
                break;
            }
            case 2:
            {
                printf("\nA continuación se mostrarán todos los documentos ordenados:\n");
                mostrarDocumentos(libreria);
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
                printf("\nIngrese una palabra para saber en que libros se encuentra:\n");
                char *palabra = NULL;
                leerChar(&palabra);
                buscarPalabra(libreria, palabra);
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