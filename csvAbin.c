#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "comunes.h"
#include "csvAbin.h"


int main(int argc, char const *argv[])
{

	state_t st;
	peli_t *films, *aux;
  	char **csv_fields, *endptr, line[MAX_STR];
 	size_t n, used_size, alloc_size;
	FILE *entrada, *salida;
	struct tm time_info;

	if(validarCantArg(CANT_PARAM_CREAR_BASE, argc) != ST_OK) return EXIT_FAILURE;
	if(handle_error(openFile(&salida,argv[POS_CREAR_BASE_DB], "wb")) != ST_OK)  return EXIT_FAILURE;
	if(handle_error(openFile(&entrada,argv[POS_CREAR_BASE_CSV], "rt")) != ST_OK) return EXIT_FAILURE;


	if ((films = (peli_t *)calloc(INIT_CHOP, sizeof(peli_t))) == NULL)
    {
    	handle_error(ST_MEMORY_ERROR);
        fclose(entrada);
        fclose(salida);
	    return EXIT_FAILURE;
	}

	used_size = 0;
	alloc_size = INIT_CHOP;

	while (fgets(line, MAX_STR, entrada) != NULL)
    {

	    if ((endptr = strrchr(line, '\n')) != NULL)	*endptr = '\0';

	    if ((st = split(line, SEPARADOR_LINEAS, &csv_fields, &n)) != ST_OK)
        {
	      	handle_error(st);
	      	del_films_array(&films, &used_size);
    		fclose(entrada);
            fclose(salida);
	      	return EXIT_FAILURE;
	    }
	    if(used_size == alloc_size)
	    {
	      if ((aux = (peli_t *)realloc(films, (alloc_size + CHOP_SIZE) * sizeof(peli_t))) == NULL)
            {
            handleAndReset(ST_MEMORY_ERROR, &films, &used_size, &csv_fields , &n, entrada, salida);
	        return EXIT_FAILURE;
            }
	      alloc_size += CHOP_SIZE;
	      films = aux;
      	}

      	/* PASO TODOS LOS DATOS DE csv_fields a el lugar de films */
	    films[used_size].id =strtol(csv_fields[ID_FIELD_POS], &endptr, 10);
	    if(*endptr != '\0')
        {
        	handleAndReset(ST_ERROR_CONVERSION, &films, &used_size, &csv_fields , &n, entrada, salida);
		    return EXIT_FAILURE;
	  	} /*fin del if*/

		strcpy(films[used_size].titulo, csv_fields[TITLE_FIELD_POS]);
		strcpy(films[used_size].guion, csv_fields[SCRIPT_FIELD_POS]);
		strcpy(films[used_size].director, csv_fields[DIRECTOR_FIELD_POS]);

		films[used_size].puntaje = strtod(csv_fields[SCORE_FIELD_POS], &endptr);
		if(*endptr != '\0')
        {
        	handleAndReset(ST_ERROR_CONVERSION, &films, &used_size, &csv_fields , &n, entrada, salida);
		    return EXIT_FAILURE;
	  	}
        
		films[used_size].reviews = strtol(csv_fields[REVIEWS_FIELD_POS], &endptr, 10);
		if(*endptr != '\0')
        {
		    handleAndReset(ST_ERROR_CONVERSION, &films, &used_size, &csv_fields , &n, entrada, salida);
		    return EXIT_FAILURE;
	  	}

	  	if ((films[used_size].fecha = mktime(fecha_calendario_extendida_a_time(csv_fields[TIME_FIELD_POS], &time_info, &endptr))) == -1)
	  	{
	  		handleAndReset(ST_ERROR_CONVERSION, &films, &used_size, &csv_fields ,&n, entrada, salida);
		    return EXIT_FAILURE;
	  	}
		used_size++;

    	del_str_array(&csv_fields, &n);

    }/*fin del while*/


	if(fwrite(films, sizeof(peli_t), used_size, salida) != used_size)
    {
	    handleAndReset(ST_ERROR_WRITE_FILE, &films, &used_size, &csv_fields , &n, entrada, salida);
	    return EXIT_FAILURE;
  	}
  	free(films);
  	fclose(entrada);
    fclose(salida);

   return EXIT_SUCCESS;
}


void handleAndReset(state_t st, peli_t **films, size_t *used_size,char ***csv_fields, size_t *n, FILE *fl, FILE *db)
{
	handle_error(st); 
	handle_error(del_films_array(films, used_size));
	del_str_array(csv_fields, n);
    fclose(fl);
    fclose(db);
}


state_t del_films_array (peli_t **films, size_t *n)
{
  if(films == NULL)
  {
    return ST_ERROR_NULL_PTR;
  }
  free (*films);
  *films = NULL;
  *n = 0;
  return ST_OK;
}
struct tm* fecha_calendario_extendida_a_time(const char *str_time, struct tm *time_info_ptr, char **endptrf)
{

	char time_info_year[CANT_CHAR_FECHA_ANIO + 1] = "";
	char time_info_mon[CANT_CHAR_FECHA_MES + 1] = "";
	char time_info_day[CANT_CHAR_FECHA_DIA + 1] = "";
	/*VER SI TODO ESTO SE PUEDE HACER FUNCION*/
	time_info_ptr ->tm_sec = 0;
	time_info_ptr ->tm_min = 0;
	time_info_ptr ->tm_hour = 0;
    time_info_ptr ->tm_wday = 0;
    time_info_ptr ->tm_yday = 0;
    time_info_ptr ->tm_isdst = 0;

    strncpy(time_info_year,(str_time + POS_YEAR),CANT_CHAR_FECHA_ANIO);
    strncpy(time_info_mon,(str_time + POS_MONTH),CANT_CHAR_FECHA_MES);
    strncpy(time_info_day,(str_time + POS_DAY),CANT_CHAR_FECHA_DIA);
    time_info_year[CANT_CHAR_FECHA_ANIO] = '\0';
    time_info_mon[CANT_CHAR_FECHA_MES]= '\0';
    time_info_day[CANT_CHAR_FECHA_DIA]= '\0';

    time_info_ptr ->tm_year = (strtol(time_info_year, endptrf, 10) - 1900);
    time_info_ptr ->tm_mon = (strtol(time_info_mon, endptrf, 10) - 1);
    time_info_ptr ->tm_mday = strtol(time_info_day, endptrf, 10);
    return time_info_ptr;
}
state_t split(char *str_fields, char delim, char ***str_array, size_t *L)
{
    char **strings;
    size_t n, i;
    char *campo, *aux, *line;
    char sdelim[2];

    if (str_fields == NULL || str_array == NULL || L == NULL)
    {
    return ST_ERROR_NULL_PTR;
    }

    for(n = 1, i = 0; str_fields[i]; i++)
    {
        if(str_fields[i] == delim)
        {
            n++;
        }
    }
    if (((strings = (char **)malloc(n * sizeof(char *))) == NULL))
    {
        return ST_MEMORY_ERROR;
    }
    if ((line=strdup(str_fields)) == NULL)
    {
        free(strings);
        return ST_MEMORY_ERROR;
    }
    sdelim[0] = delim;
    sdelim[1] = '\0';
    i = 0;
    aux = line;
    campo = strtok(aux, sdelim);
    do
    {
        if(( strings[i] = strdup(campo)) == NULL)
        {
            free(line);
            del_str_array(&strings, &n);
            *str_array = NULL;
            *L = 0;
            return ST_MEMORY_ERROR;
        }
        i++;
    } while ((campo = strtok(NULL,sdelim)) != NULL);
    *str_array = strings;
    *L = n;
    free(line);
    return ST_OK;
}

char* strdup(const char *str)
{
    char* str2;

    if((str2 = (char *)malloc(sizeof(char) * (strlen(str) + 1))) == NULL)
    {
        return NULL;
    }
    return strcpy(str2,str);

}

void del_str_array(char ***strings, size_t *L)
{
    unsigned int i;

    for(i = 0; i < (*L); i++)
    {
        free((*strings)[i]);
        (*strings)[i] = NULL;
    }
    free(*strings);
    *strings = NULL;
    *L = 0;
}

