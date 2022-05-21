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
    float apariciones;
    float frecuencia;
} tipoPalabra;

typedef struct
{
    int id;
    int *IDlibro;
    float cantidadPalabras;
    int cantidadCaracteres;
    char *titulo;
} tipoLibro;

typedef struct
{
    int totalArchivos;
    TreeMap *mapaLibros; //Clave: titulo libro ; valor: variable tipo libro     
    Map *mapaIDs; //Clave: ID libro ; valor: lista de variables tipo palabra (nombre palabra, cantidad de apariciones, frecuencia)
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

//Función para poder hacer los "createMap"
int stringEqual(void * key1, void * key2) {
    char * A = key1;
    char * B = key2;
    return strcmp(A, B) == 0;
}

tipoLibreria* crearLibreria()
{
    tipoLibreria *aux = (tipoLibreria*) calloc (1, sizeof(tipoLibreria));
    aux->mapaLibros = createTreeMap(lower_than_string);
    aux->mapaIDs = createMap(stringEqual);
    aux->mapaPalabras = createMap(stringEqual);
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

void guardarPalabras(tipoLibreria *libreria, FILE *archivo, tipoLibro *libro)
{
    List *aux;
    tipoLibro *auxLibro;
    char *word = next_word(archivo);
    int flag, k;
    
    while (word != NULL)
    {
        flag = 0;

        for (k = 0; word[k]; k++) word[k] = tolower(word[k]);

        word = quitar_caracteres(word, "!?\";,.");

        if (searchMap(libreria->mapaPalabras, word) == NULL)
        {
            List *listaLibros = createList();
            pushBack(listaLibros, libro);
            insertMap(libreria->mapaPalabras, word, listaLibros);
        }

        if (searchMap(libreria->mapaPalabras, word) != NULL)
        {
            aux = searchMap(libreria->mapaPalabras, word);
            auxLibro = firstList(aux);

            while (auxLibro != NULL)
            {
                if (strcmp(auxLibro->titulo, libro->titulo) == 0) flag = 1;
                auxLibro = nextList(aux);
            }

            if (flag == 0) pushBack(aux, libro);
        }

        libro->cantidadPalabras++;
        word = next_word(archivo);
    }
}


void repeticionesPalabras(tipoLibreria *libreria, FILE *archivo, tipoLibro *libro)
{
    tipoPalabra *auxPalabra, *auxPalabra2 , *aux;
    List *listaPalabrasLibro = createList();
    char *word = next_word(archivo);
    int largo, flag, i, k;
    int largoLista = 0;

    while (word != NULL)
    {
        flag = 0;

        for (k = 0; word[k]; k++) word[k] = tolower(word[k]);

        word = quitar_caracteres(word, "!?\";,.");
       
        tipoPalabra *palabra = (tipoPalabra*) malloc(1 * sizeof(tipoPalabra));
        largo = strlen(word) + 1;
        palabra->word = (char*) malloc(largo * sizeof(char));
        strcpy(palabra->word, word);
        palabra->apariciones = 1;
        palabra->frecuencia = 0;

        auxPalabra = firstList(listaPalabrasLibro);

        do
        {
            if (auxPalabra == NULL) 
            {
                flag = 1;
                break;
            }
                
            if (strcmp(auxPalabra->word, palabra->word) != 0) flag = 1;
            if (strcmp(auxPalabra->word, palabra->word) == 0) 
            {
                flag = 0;
                auxPalabra->apariciones++;
                auxPalabra->frecuencia = (auxPalabra->apariciones) / (libro->cantidadPalabras);
                break;
            }
            auxPalabra = nextList(listaPalabrasLibro);

        } while (auxPalabra != NULL);

        if (flag == 1) 
        {
            pushBack(listaPalabrasLibro, palabra);
            largoLista++;
        }
        
        word = next_word(archivo);
    }

    /*printf("Largo lista: %d\n", largoLista);
    printf("Se cae en el bubble sort\n");

    auxPalabra = firstList(listaPalabrasLibro);
    auxPalabra2 = auxPalabra;
    auxPalabra2 = nextList(listaPalabrasLibro);*/

    //while (auxPalabra2 != NULL)
    //{
        //if (auxPalabra->frecuencia < auxPalabra2->frecuencia)
        //{
            /*aux = auxPalabra2;
            auxPalabra2 = auxPalabra;
            auxPalabra = aux;*/
            //pushCurrent(listaPalabrasLibro, auxPalabra);
            //pushBack(listaPalabrasLibro, aux);
        //}

        //auxPalabra = nextList(listaPalabrasLibro);
        //auxPalabra2 = nextList(listaPalabrasLibro);
    //}

    /*for (i = 0 ; i < largoLista - 1 ; i++)
    {
        auxPalabra = firstList(listaPalabrasLibro);
        auxPalabra2 = auxPalabra;

        for (k = 0 ; k < largoLista - i - 1 ; k++)
        {
            auxPalabra2 = nextList(listaPalabrasLibro);
            //if (auxPalabra2 == NULL) break;

            if (auxPalabra->frecuencia < auxPalabra2->frecuencia)
            {
                aux = auxPalabra2;
                auxPalabra2 = auxPalabra;
                auxPalabra = aux;
                //printf("Palabra: %s --- Apariciones: %.0f\n", auxPalabra2->word, auxPalabra2->apariciones);
            }

            auxPalabra = nextList(listaPalabrasLibro);
            //if (auxPalabra == NULL) break;
        }
    }*/

    insertMap(libreria->mapaIDs, libro->IDlibro, listaPalabrasLibro);
}

void cargarDocumentos(tipoLibreria *libreria, char *todosLosArchivos)
{
    const char separador[2] = " ";
    char *archivoActual = strtok(todosLosArchivos, separador);
    char cadena[1024];

    if (archivoActual != NULL)
    {
        while (archivoActual != NULL)
        {
            printf("\nNombre del documento que se está cargando: %s\n", archivoActual);
                        
            FILE *archivo = fopen(archivoActual, "r");
 
            if (archivo == NULL) 
            {
                printf("No existe este documento.\n");
                archivoActual = strtok(NULL, separador);
                continue;
            }

            tipoLibro *libro = (tipoLibro*) malloc(1 * sizeof(tipoLibro));
            libro->cantidadPalabras = 0;
            libro->id = atoi(archivoActual);
            libro->IDlibro = (int*) malloc(1 * sizeof(int));
            libro->IDlibro = &(libro->id);

            while (strcmp(fgets(cadena, 1023, archivo), "\n") != 0)
            {
                int largo = strlen(cadena) + 1;
                libro->titulo = (char*) malloc(largo * sizeof(char));
                strcpy(libro->titulo, cadena);
                insertTreeMap(libreria->mapaLibros, libro->titulo, libro);
                break;
            }

            printf("Por favor espere unos segundos...\n");
            guardarPalabras(libreria, archivo, libro);
            fclose(archivo);

            archivo = fopen(archivoActual, "r");
            repeticionesPalabras(libreria, archivo, libro);
            fclose(archivo);

            archivoActual = strtok(NULL, separador);
            libreria->totalArchivos++;
            printf("Documento cargado correctamente.\n");
        }
    }
}

void mostrarDocumentos(tipoLibreria *libreria)
{
    Pair *aux = firstTreeMap(libreria->mapaLibros);
    tipoLibro *libro;

    while (aux != NULL)
    {
        libro = aux->value;
        printf("ID libro: %d\n", libro->id);
        printf("Título: %s", (char*) aux->key);
        printf("Cantidad de palabras: %.0f\n\n", libro->cantidadPalabras);

        aux = nextTreeMap(libreria->mapaLibros);
    }
}

void buscarLibroPorTitulo(tipoLibreria *libreria, char *todasLasPalabras)
{
    const char separador[2] = " ";
    char *palabra, *titulo, *palabraTitulo;
    int largo, flag, k;
    int cont = 0;

    largo = strlen(todasLasPalabras) + 1;
    char *todasLasPalabrasAux = (char*) malloc(largo * sizeof(char));
    todasLasPalabrasAux = todasLasPalabras;

    Pair *aux = firstTreeMap(libreria->mapaLibros);

    while (aux != NULL)
    {
        largo = strlen(aux->key) + 1;
        titulo = (char*) malloc(largo * sizeof(char));

        flag = 0;

        todasLasPalabras = todasLasPalabrasAux;
        palabra = strtok(todasLasPalabras, separador);

        while (palabra != NULL)
        {
            strcpy(titulo, aux->key);
            palabraTitulo = strtok(titulo, separador);
            
            while (palabraTitulo != NULL)
            {
                for (k = 0; palabraTitulo[k]; k++) palabraTitulo[k] = tolower(palabraTitulo[k]);
                palabraTitulo = quitar_caracteres(palabraTitulo, "!?\";,.");

                if (strcmp(palabra, palabraTitulo) == 0) 
                {
                    flag = 1;
                    break;
                }

                if (strcmp(palabra, palabraTitulo) != 0) flag = 0;
        
                palabraTitulo = strtok(NULL, separador);
            }

            if (flag == 0) break;

            palabra = strtok(NULL, separador);
        }

        if (flag == 1)
        {
            printf("%s contiene todas las palabras ingresadas.\n\n", (char*) aux->key);
            cont++;
        }

        aux = nextTreeMap(libreria->mapaLibros);
    }

    if (cont == 0) printf("Ningún libro contenía todas las palabras ingresadas en su título.\n");
}

void mostrarPalabrasMayorFrecuencia(tipoLibreria *libreria, int *id)
{
    int cont = 0;

    if (searchMap(libreria->mapaIDs, id) != NULL)
    {
        List *listaPalabras = searchMap(libreria->mapaIDs, id);
        tipoPalabra *recorrer = firstList(listaPalabras);

        while (recorrer != NULL)
        {
            printf("Palabra: %s\n", recorrer->word);
            printf("Apariciones: %.0f\n", recorrer->apariciones);
            printf("Frecuencia: %f\n", recorrer->frecuencia);
            recorrer = nextList(listaPalabras);
            cont++;
            if (cont == 10) break;
        }
    }
    else printf("El ID ingresado no pertenece a ningún libro.\n");
}

void buscarPalabra(tipoLibreria *libreria, char *palabra)
{
    if (searchMap(libreria->mapaPalabras, palabra) != NULL)
    {
        List *listaLibros = searchMap(libreria->mapaPalabras, palabra);
        tipoLibro *recorrer = firstList(listaLibros);

        while (recorrer != NULL)
        {
            printf("ID libro: %d\n", recorrer->id);
            printf("Título: %s\n", recorrer->titulo);
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
    libreria->totalArchivos = 0;
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
                printf("\nA continuación se mostrarán todos los documentos ordenados:\n\n");
                mostrarDocumentos(libreria);
                break;
            }
            case 3:
            {
                printf("\nIngrese algunas palabras separadas por espacio para saber que títulos de libros contienen todas esas palabras:\n");
                char *todasLasPalabras = NULL;
                leerChar(&todasLasPalabras);
                buscarLibroPorTitulo(libreria, todasLasPalabras);
                break;
            }
            case 4:
            {
                printf("\nIngrese el ID de un libro para conocer sus 10 palabras que se repiten con mayor frecuencia:\n");
                int id;
                scanf("%d", &id);
                mostrarPalabrasMayorFrecuencia(libreria, &id);
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
                printf("\n");
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