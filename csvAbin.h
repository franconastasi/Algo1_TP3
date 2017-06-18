#ifndef CSVABIN__H
#define CSVABIN__H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CANT_PARAM_CREAR_BASE 3
#define POS_CREAR_BASE_DB 2
#define POS_CREAR_BASE_CSV 1

#define CANT_CHAR_FECHA_ANIO 4
#define CANT_CHAR_FECHA_MES 2
#define CANT_CHAR_FECHA_DIA 2
#define ID_FIELD_POS 0
#define TITLE_FIELD_POS 1
#define SCRIPT_FIELD_POS 2
#define DIRECTOR_FIELD_POS 3
#define TIME_FIELD_POS 4
#define SCORE_FIELD_POS 5
#define REVIEWS_FIELD_POS 6
#define SEPARADOR_FECHA '-'
#define MAX_STR 200
#define POS_YEAR 0
#define POS_MONTH 5
#define POS_DAY 8

void handleAndReset(state_t st, peli_t **films, size_t *used_size,char ***csv_fields, size_t *n, FILE *fl, FILE *db);

state_t split(char *str_fields, char delim, char ***str_array, size_t *L);

state_t del_films_array (peli_t **films, size_t *n);

struct tm *fecha_calendario_extendida_a_time(const char *str_time, struct tm *time_info_ptr, char **endptrf);

char *strdup(const char *str);

void del_str_array(char ***strings, size_t *L);

#endif