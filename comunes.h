#ifndef COMUNES__H
#define COMUNES__H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "language.h"
#include "types.h"
#include "pelicula.h"

#define SEPARADOR_LINEAS ','
#define INIT_CHOP 4
#define CHOP_SIZE 4
#define CANT_CAMPOS 10

state_t validarCantArg(size_t, size_t);
state_t handle_error(state_t st);
state_t openFile(FILE **fl, const char *file_name, char *mode);

#endif