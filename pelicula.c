#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "peliculas.h"

state_t peli_cpy(peli_t *dest, peli_t *src)
{
    if ((dest == NULL) || (src == NULL))
    {
        return ST_ERROR_NULL_PTR;
    }
    *dest = *src;
    return ST_OK;
}