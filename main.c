#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "treemap.h"
#include "map.h"
#include "list.h"

typedef struct 
{
    char *word;
    float apariciones;
    float frecuencia;
    char frecString[10];
    float relevancia;
} tipoPalabra;

typedef struct
{
    int id;
    int *IDlibro;
    float cantidadPalabras;
    char *titulo;
} tipoLibro;

typedef struct
{
    int totalArchivos;
    TreeMap *mapaLibros; //Clave: titulo libro ; valor: variable tipo libro  
    Map *mapaTitulos; //Clave: titulo libro ; valor: lista de variables tipo palabra (nombre, apariciones, frecuencia, relevancia)   
    Map *mapaIDs; //Clave: ID libro ; valor: lista de variables tipo palabra (nombre, apariciones, frecuencia, relevancia)
    TreeMap *mapaFrecuencia; //Clave: frecuencia palabra ; valor: variable tipo palabra (contiene todos los datos de esa palabra)
    Map *mapaPalabras; //Clave: palabra ; valor: lista de libros que contienen esa palabra
} tipoLibreria;

//Función para poder hacer los "createMap" de los TreeMap
int lower_than_string(void* key1, void* key2)
{
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}

//Función para poder hacer los "createMap" de los Map
int stringEqual(void * key1, void * key2) {
    char * A = key1;
    char * B = key2;
    return strcmp(A, B) == 0;
}

//Función para crear la variable que almacenará todos los mapas
tipoLibreria* crearLibreria()
{
    tipoLibreria *aux = (tipoLibreria*) calloc (1, sizeof(tipoLibreria));
    aux->mapaLibros = createTreeMap(lower_than_string);
    aux->mapaTitulos = createMap(stringEqual);
    aux->mapaIDs = createMap(stringEqual);
    aux->mapaFrecuencia = createTreeMap(lower_than_string);
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

char *next_word(FILE *f) 
{
    char x[1024];
    //assumes no word exceeds length of 1023 
    if (fscanf(f, " %1023s", x) == 1) return strdup(x);
    else return NULL;
}

//Función implementada por Matías Barrientos 
int hayQueEliminar(char c, char* string_chars)
{
    for (int i=0 ; i<strlen(string_chars) ; i++)
    {
        if (string_chars[i]==c) return 1;
    }
    return 0;
}

//Función implementada por Matías Barrientos para eliminar caracteres de una palabra
char *quitar_caracteres(char* string, char* c)
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

//Función para crear un mapa en el que cada tupla está compuesta de una palabra (clave) y una lista
//de los libros que contienen esa palabra en su contenido (valor)
void guardarPalabras(tipoLibreria *libreria, FILE *archivo, tipoLibro *libro)
{
    List *aux;
    tipoLibro *auxLibro;
    char *word = next_word(archivo);
    int flag, k;
    
    //Se recorre el libro palabra a palabra
    while (word != NULL)
    {
        flag = 0;

        for (k = 0; word[k]; k++) word[k] = tolower(word[k]);

        word = quitar_caracteres(word, "!?\";,.");

        //Si la palabra no está, se crea una lista, se le ingresa el libro y luego se insertan la palabra y la lista al mapa
        if (searchMap(libreria->mapaPalabras, word) == NULL)
        {
            List *listaLibros = createList();
            pushBack(listaLibros, libro);
            insertMap(libreria->mapaPalabras, word, listaLibros);
        }

        //Si la palabra ya estaba, se recorre su lista de libros asociada para saber si el libro que se está leyendo ya se
        //insertó en ella, si ya está, no se hace nada, si no está, se inserta
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

//Función para calcular las apariciones y frecuencia de cada palabra
void repeticionesPalabras(tipoLibreria *libreria, FILE *archivo, tipoLibro *libro)
{
    tipoPalabra *auxPalabra, *auxPalabra2 , *aux;
    List *listaPalabrasLibro = createList(); //Se crea un alista que va a contener todas las palabras de cada libro (sin repetir)
    char *word = next_word(archivo);
    int largo, flag, i, k;
    int largoLista = 0;

    //Se recorre el libro palabra a palabra
    while (word != NULL)
    {
        flag = 0;

        for (k = 0; word[k]; k++) word[k] = tolower(word[k]);

        word = quitar_caracteres(word, "!?\";,.");

        //Se crea una variable que almacenará todos los datos de cada palabra
        tipoPalabra *palabra = (tipoPalabra*) malloc(1 * sizeof(tipoPalabra));
        largo = strlen(word) + 1;
        palabra->word = (char*) malloc(largo * sizeof(char));
        strcpy(palabra->word, word);
        palabra->apariciones = 1;
        palabra->frecuencia = 0;
        palabra->relevancia = 0;

        //Se recorre la lista
        auxPalabra = firstList(listaPalabrasLibro);

        do
        {
            //Si la lista está vacía o la palabra no se encuentra en ella, entonces la palabra se agrega a la lista
            if (auxPalabra == NULL) 
            {
                flag = 1;
                break;
            }
                
            if (strcmp(auxPalabra->word, palabra->word) != 0) flag = 1;

            //Si la palabra se encuentra, se aumentan sus apariciones y frecuencia
            if (strcmp(auxPalabra->word, palabra->word) == 0) 
            {
                flag = 0;
                auxPalabra->apariciones++;
                auxPalabra->frecuencia = (auxPalabra->apariciones) / (libro->cantidadPalabras); 
                gcvt(auxPalabra->frecuencia, 10, auxPalabra->frecString);
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

    //Al terminar de leer el libro, se recorre la lista creada y cada una de sus variables se almacenan en un mapa con clave frecuencia
    auxPalabra = firstList(listaPalabrasLibro);

    while (auxPalabra != NULL)
    {
        insertTreeMap(libreria->mapaFrecuencia, auxPalabra->frecString, auxPalabra);
        auxPalabra = nextList(listaPalabrasLibro);
    }

    //El mapa de frecuencias se inserta como clave en los mapas de ID y título
    insertMap(libreria->mapaIDs, libro->IDlibro, libreria->mapaFrecuencia);
    insertMap(libreria->mapaTitulos, libro->titulo, libreria->mapaFrecuencia);
}

//Función para leer cada archivo que el usuario ingresa
void cargarDocumentos(tipoLibreria *libreria, char *todosLosArchivos)
{
    const char separador[2] = " ";
    char *archivoActual = strtok(todosLosArchivos, separador); //Se usa esta función para separar el string con todos los archivos
                                                               //en strings más pequeños (en este caso separamos por espacio)
    char cadena[1024];

    if (archivoActual != NULL)
    {
        //Se lee hasta que se acaben los documentos
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

            //Se crea una variable que almacena los datos del libro que se está leyendo
            tipoLibro *libro = (tipoLibro*) malloc(1 * sizeof(tipoLibro));
            libro->cantidadPalabras = 0;
            libro->id = atoi(archivoActual);
            libro->IDlibro = (int*) malloc(1 * sizeof(int));
            libro->IDlibro = &(libro->id);

            //Se usa el fseek para moverse directamente hasta el inicio del título del libro en la primera línea del archivo
            fseek(archivo, 33, SEEK_SET);

            fgets(cadena, 1023, archivo);

            //Se lee la primera línea del archivo y se guarda como el título del libro
            int largo = strlen(cadena) + 1;
            libro->titulo = (char*) malloc(largo * sizeof(char));
            strcpy(libro->titulo, cadena);

            char *newline = strchr(libro->titulo, '\n');
            if (newline != NULL) *newline = '\0';

            char *cr = strchr(libro->titulo, '\r');
            if(cr != NULL) *cr = '\0';

            insertTreeMap(libreria->mapaLibros, libro->titulo, libro);

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

//Función para mostrar los documentos cargados y sus datos en orden alfabético
void mostrarDocumentos(tipoLibreria *libreria)
{
    Pair *aux = firstTreeMap(libreria->mapaLibros);
    tipoLibro *libro;

    while (aux != NULL)
    {
        libro = aux->value;
        printf("ID libro: %d\n", libro->id);
        printf("Título: %s\n", (char*) aux->key);
        printf("Cantidad de palabras: %.0f\n\n", libro->cantidadPalabras);

        aux = nextTreeMap(libreria->mapaLibros);
    }
}

//Función para buscar los títulos de libros que contienen todas las palabras ingresadas por el usuario
void buscarLibroPorTitulo(tipoLibreria *libreria, char *todasLasPalabras)
{
    const char separador[2] = " ";
    char *palabra, *titulo, *palabraTitulo;
    int largo, flag, i, k;
    int cont = 0;

    int palabrasIngresadas = 0;
    char arregloPalabrasIngresadas[1024][1024] = {""};

    palabra = strtok(todasLasPalabras, separador);

    //Las palabras ingresadas por el usuario se guardan en una matriz (arreglo de arreglos)
    while (palabra != NULL)
    {
        strcpy(arregloPalabrasIngresadas[palabrasIngresadas], palabra);
        palabrasIngresadas++;
        palabra = strtok(NULL, separador);
    }

    Pair *aux = firstTreeMap(libreria->mapaLibros);

    //Se recorre hasta leer todos los libros cargados en la aplicación
    while (aux != NULL)
    {
        largo = strlen(aux->key) + 1;
        titulo = (char*) malloc(largo * sizeof(char));

        flag = 0;

        //Se lee cada palabra ingresada por el usuario
        for (i = 0 ; i < palabrasIngresadas ; i++)
        {
            strcpy(titulo, aux->key);
            palabraTitulo = strtok(titulo, separador);

            //Se lee cada palabra presente en el título del libro que se está leyendo
            while (palabraTitulo != NULL)
            {
                for (k = 0 ; palabraTitulo[k] ; k++) palabraTitulo[k] = tolower(palabraTitulo[k]);
                palabraTitulo = quitar_caracteres(palabraTitulo, "!?\";,.");

                //Se compara la palabra actual del usuario con cada palabra del título del libro actual
                if (strcmp(arregloPalabrasIngresadas[i], palabraTitulo) == 0) 
                {
                    flag = 1;
                    break;
                }

                if (strcmp(arregloPalabrasIngresadas[i], palabraTitulo) != 0) flag = 0;
        
                palabraTitulo = strtok(NULL, separador);
            }

            if (flag == 0) break;
        }

        if (flag == 1)
        {
            printf("%s contiene todas las palabras ingresadas en su título.\n\n", (char*) aux->key);
            cont++;
        }

        aux = nextTreeMap(libreria->mapaLibros);
    }

    if (cont == 0) printf("Ningún libro contenía todas las palabras ingresadas en su título.\n");
}

void mostrarPalabrasMayorFrecuencia(tipoLibreria *libreria, int *id)
{
    tipoPalabra *palabra;
    int cont = 0;

    //Se busca el ID ingresado
    if (searchMap(libreria->mapaIDs, id) != NULL)
    {
        //Si el ID existe, entonces se busca su mapa de frecuencias asociado y se recorre
        TreeMap *mapaFrecuencias = searchMap(libreria->mapaIDs, id);
        Pair *aux = firstTreeMap(libreria->mapaFrecuencia);

        while (aux != NULL)
        {
            palabra = aux->value;
            printf("Palabra: %-20s", palabra->word);
            printf("Apariciones: %-20.0f", palabra->apariciones);
            printf("Frecuencia: %-20f\n", palabra->frecuencia);
            aux = nextTreeMap(libreria->mapaFrecuencia);
            cont++;
            if (cont == 10) break;
        }
    }
    else printf("El ID ingresado no pertenece a ningún libro.\n");
}

int contarDocumentosConPalabra(tipoLibreria *libreria, char *palabra)
{
    int cont = 0;

    if (searchMap(libreria->mapaPalabras, palabra) != NULL)
    {
        List *listaLibros = searchMap(libreria->mapaPalabras, palabra);
        tipoLibro *recorrer = firstList(listaLibros);

        while (recorrer != NULL)
        {
            cont++;
            recorrer = nextList(listaLibros);
        }
    }

    return cont;
}

void mostrarPalabrasMayorRelevancia(tipoLibreria *libreria, char *titulo)
{
    int totalLibrosConPalabra = 0;

    if (searchTreeMap(libreria->mapaLibros, titulo) != NULL)
    {
        Pair *aux = searchTreeMap(libreria->mapaLibros, titulo);
        tipoLibro *libro = aux->value;

        List *listaPalabras = searchMap(libreria->mapaTitulos, titulo);
        tipoPalabra *recorrer = firstList(listaPalabras);

        while (recorrer != NULL)
        {
            totalLibrosConPalabra = contarDocumentosConPalabra(libreria, recorrer->word);
            recorrer->relevancia = ((recorrer->apariciones) / (libro->cantidadPalabras)) * log(libreria->totalArchivos / totalLibrosConPalabra);
            recorrer = nextList(listaPalabras);
        }
    }
    else 
    {
        printf("El título ingresado no pertenece a ningún libro.\n");
        return;
    }

    int cont = 0;

    if (searchMap(libreria->mapaTitulos, titulo) != NULL)
    {
        List *listaPalabras = searchMap(libreria->mapaTitulos, titulo);
        tipoPalabra *recorrer = firstList(listaPalabras);

        while (recorrer != NULL)
        {
            printf("Palabra: %s\n", recorrer->word);
            printf("Relevancia: %.0f\n", recorrer->relevancia);
            recorrer = nextList(listaPalabras);
            cont++;
            if (cont == 10) break;
        }
    }
}

//Función para buscar una palabra que se encuentre en el contenido de los libros cargados
void buscarPalabra(tipoLibreria *libreria, char *palabra)
{
    if (searchMap(libreria->mapaPalabras, palabra) != NULL)
    {
        //Si la palabra está en un libro, se recorre la lista de los libros que la contienen
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

void palabraEnContexto(tipoLibreria *libreria, char *titulo, char *palabra)
{
    if (searchTreeMap(libreria->mapaLibros, titulo) != NULL)
    {
        
    }
    else 
    {
        printf("El título ingresado no pertenece a ningún libro.\n");
        return;
    }
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
    tipoLibreria *libreria = crearLibreria(); //Se crea la variable que almacenará todos los mapas del programa
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
                printf("\nIngrese el título de un libro para conocer sus 10 palabras más relevantes:\n");
                char *titulo = NULL;
                leerChar(&titulo);
                mostrarPalabrasMayorRelevancia(libreria, titulo);
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
                printf("\nIngrese el título de un libro:\n");
                char *titulo = NULL;
                leerChar(&titulo);
                printf("\nIngrese una palabra a buscar para conocer sus apariciones en el contexto del libro:\n");
                char *palabra = NULL;
                leerChar(&palabra);
                palabraEnContexto(libreria, titulo, palabra);
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