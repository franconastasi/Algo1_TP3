#ifndef PELICULA__H
#define PELICULA__H

#define FIELD_SIZE_STR 50

#include <time.h>

typedef struct peliculas
{
	size_t id;
    char titulo[FIELD_SIZE_STR];
    char guion [FIELD_SIZE_STR];
    char director[FIELD_SIZE_STR];
    time_t fecha;
    double puntaje;
    size_t reviews;
} peli_t;

state_t peli_cpy(peli_t *dest, peli_t *src);

#endif