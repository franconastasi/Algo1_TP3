#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comunes.h"

state_t validarCantArg(size_t correct_cant, size_t input_cant)
{
    if (correct_cant != input_cant)
    {   
        return ST_ERROR_INVALID_CANT_ARG;
    }
    else
    {
        return ST_OK;
    }
}

state_t handle_error(state_t st)
{
    const char *error_msg[] = { MSG_MEMORY_ERROR,
                                MSG_ERROR_LOAD_DB ,
                                MSG_ERROR_CONVERSION,
                                MSG_ERROR_NULL_PTR,
                                MSG_ERROR_COMMAND,
                                MSG_ERROR_INVALID_CANT_ARG,
                                MSG_ERROR_UNABLE_TO_OPEN_FILE,
                                MSG_ERROR_UNABLE_TO_READ_FROM_FILE,
                                MSG_ERROR_WRITE_FILE,
                                MSG_ERROR_PARAM_NOT_FOUND,
                              };
    if (st != ST_OK)
        fprintf(stderr, "%s:%s\n", MSG_ERROR_PREFIX, error_msg[st - 1]);
    return st;
}

state_t openFile(FILE **fl, const char *file_name, char *mode)
{
    if ((*fl = fopen(file_name, mode)) == NULL)
    {
        return ST_ERROR_UNABLE_TO_OPEN_FILE;
    }   
    else
    {
        return ST_OK;
    }
}