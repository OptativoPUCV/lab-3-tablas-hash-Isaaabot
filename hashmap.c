#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}

/*
2.- Implemente la función void insertMap(HashMap * map, char * key, void * value). Esta función 
inserta un nuevo dato (key,value) en el mapa y actualiza el índice current a esa posición. Recuerde que 
para insertar un par (clave,valor) debe:

a) Aplicar la función hash a la clave para obtener la posición donde debería insertar el nuevo par
b) Si la casilla se encuentra ocupada, avance hasta una casilla disponible (método de resolución de colisiones). 
Una casilla disponible es una casilla nula, pero también una que tenga un par inválido (key==NULL).
c) Ingrese el par en la casilla que encontró.
- No inserte claves repetidas. Recuerde que el arreglo es circular. Recuerde actualizar la variable size.
*/
void insertMap(HashMap *map, char *key, void *value)
{
    long indice = hash(key, map->capacity) ;
    long indice_original = indice ;
    int flag = 1 ; // Flag para saber si ya a salido del bucle

    while (map->buckets[indice] != NULL && map->buckets[indice]->key != NULL) // Buscamos una casilla disponible
    {
        if (strcmp(map->buckets[indice]->key, key) == 0) // Verificación por si la clave ya existe, no insertarla
            return ; // Clave duplicada, por lo tanto no la insertamos
        
        indice = (indice + 1) % map->capacity ;

        if (!flag && indice == indice_original) // Si damos la vuelta, salimos
            break ;
        
        flag = 0 ;
    }
    
    Pair *nuevo_par = createPair(key, value) ; // Creamos un nuevo par

    // Insertamos en la posición encontrada
    map->buckets[indice] = nuevo_par ;
    map->current = indice ;
    (map->size)++ ;
}

/*
6.- Implemente la función void enlarge(HashMap * map). Esta función agranda la capacidad del arreglo buckets y reubica todos 
sus elementos. Para hacerlo es recomendable mantener referenciado el arreglo actual/antiguo de la tabla con un puntero auxiliar. 
Luego, los valores de la tabla se reinicializan con un nuevo arreglo con el doble de capacidad. Por último los elementos del arreglo 
antiguo se insertan en el mapa vacío con el método insertMap. Puede seguir los siguientes pasos:
*/
void enlarge(HashMap * map) 
{
    enlarge_called = 1 ; //no borrar (testing purposes)
    
    // a) Cree una variable auxiliar de tipo Pair** para matener el arreglo map->buckets (old_buckets);
    Pair **old_buckets = map->buckets ;
    
    // b) Duplique el valor de la variable capacity.
    map->capacity *= 2 ;

    // c) Asigne a map->buckets un nuevo arreglo con la nueva capacidad.
    map->buckets = (Pair **) malloc(sizeof(Pair *) * map->capacity) ;

    if (map->buckets == NULL)
        exit(EXIT_FAILURE) ;
    
    for (long k = 0 ; k < map->capacity ; k++) // Inicializar los buckets en NULL
    {
        map->buckets[k] = NULL ;
    }

    // d) Inicialice size a 0.
    map->size = 0 ;

    // e) Inserte los elementos del arreglo old_buckets en el mapa (use la función insertMap que ya implementó).
    for (long k = 0 ; k < map->capacity / 2 ; k++)
    {
        if (old_buckets[k] != NULL && old_buckets[k]->key != NULL)
            insertMap(map, old_buckets[k]->key, old_buckets[k]->value) ;
    }
    free(old_buckets) ; // Liberar memoria
}

/*
1.- Implemente la función createMap en el archivo hashmap.c. Esta función crea una variable de 
tipo HashMap, inicializa el arreglo de buckets con casillas nulas, inicializa el resto de variables 
y retorna el mapa. Inicialice el índice current a -1.
*/
HashMap *createMap(long capacity)
{
    HashMap *nuevo_mapa = (HashMap *) malloc(sizeof(HashMap)) ; // Reservamos memoria para el HashMap

    if (nuevo_mapa == NULL)
        exit(EXIT_FAILURE) ;

    nuevo_mapa->buckets = (Pair **) malloc(sizeof(Pair *) * capacity) ; // Reservamos memoria para los buckets
    
    if (nuevo_mapa->buckets == NULL)
    {
        free(nuevo_mapa) ; // Liberar memoria en caso de que falle la reserva de buckets
        exit(EXIT_FAILURE) ;
    }

    for (long k = 0 ; k < capacity ; k++) // Inicializamos las casillas en NULL
    {
        nuevo_mapa->buckets[k] = NULL ;
    }
    
    // Inicializamos el resto de variables 
    nuevo_mapa->size = 0 ;
    nuevo_mapa->capacity = capacity ;
    nuevo_mapa->current = -1 ;
    
    return nuevo_mapa ;
}

/*
4.- Implemente la función void eraseMap(HashMap * map, char * key). Está función elimina el dato correspondiente a 
la clave key. Para hacerlo debe buscar el dato y luego marcarlo para que no sea válido. No elimine el par, sólo 
invalídelo asignando NULL a la clave (pair->key=NULL). Recuerde actualizar la variable size.
*/
void eraseMap(HashMap *map,  char *key) 
{    
    Pair *par_buscado = searchMap(map, key) ; // Buscamos el par con función searchMap
    
    if (par_buscado != NULL) // Si no es NULL quiere decir que existe
    {
        par_buscado->key = NULL ; // Invalidamos par
        (map->size)-- ; // Disminuimos en 1 la talla
    }
}

/*
3.- Implemente la función Pair * searchMap(HashMap * map, char * key), la cual retorna el Pair asociado 
a la clave ingresada. Recuerde que para buscar el par debe:

a) Usar la función hash para obtener la posición donde puede encontrarse el par con la clave
b) Si la clave no se encuentra avance hasta encontrarla (método de resolución de colisiones)
c) Si llega a una casilla nula, retorne NULL inmediatamente (no siga avanzando, la clave no está)
- Recuerde actualizar el índice current a la posición encontrada. Recuerde que el arreglo es circular.
*/
Pair *searchMap(HashMap *map,  char *key) 
{   
    long indice = hash(key, map->capacity) ;
    long indice_original = indice ;
    int flag = 1 ;

    while (map->buckets[indice] != NULL)
    {
        if (map->buckets[indice]->key != NULL) // Si el par en la posicion actual es valido
        {
            if (strcmp(map->buckets[indice]->key, key) == 0) // Compara las claves
            {
                map->current = indice ; // Se actualiza el indice current
                return map->buckets[indice] ; // El par encontrado
            }
        }
        indice = (indice + 1) % map->capacity ; // Siguiente posición

        if (!flag && indice == indice_original) // Para evitar bucle infinito
            break ;

        flag = 0 ;
    }
    return NULL ; // No se encontró
}

/*
5.- Implemente las funciones para recorrer la estructura: Pair * firstMap(HashMap * map) retorna el primer Pair 
válido del arreglo buckets. Pair * nextMap(HashMap * map) retorna el siguiente Pair del arreglo buckets a partir 
índice current. Recuerde actualizar el índice.
*/
Pair *firstMap(HashMap *map) 
{
    for (long k = 0 ; k < map->capacity ; k++) // Se recorre la estructura
    {
        if (map->buckets[k] != NULL && map->buckets[k]->key != NULL) // Si la posicion y la llave son distintos de NULL
        {
            map->current = k ; // Actualizamos el current
            return map->buckets[k] ; // Se retorna el primer par
        }
    }
    return NULL ;
}

Pair *nextMap(HashMap *map) 
{
    for (long k = map->current + 1 ; k < map->capacity ; k++) // Se recorre la estructura desde el map->current
    {
        if (map->buckets[k] != NULL && map->buckets[k]->key != NULL) // Si la posicion y la llave son distintos de NULL
        {
            map->current = k ; // Actualizamos el current
            return map->buckets[k] ; // Se retorna el siguiente par
        }
    }
    return NULL ;
}