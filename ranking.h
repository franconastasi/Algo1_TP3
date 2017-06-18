#ifndef RANKING__H
#define RANKING__H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_ALTA "ALTA"
#define STR_BAJA "BAJA"
#define STR_MOD "MODIFICAR"
#define STR_ALTA_MIN "alta"
#define STR_BAJA_MIN "baja"
#define STR_MOD_MIN "modificar"
#define COMMAND_DB "-if"
#define COMMAND_FILE "-f"
#define COMMAND_LOG "-log"
#define POS_COMMAND_CLA 1
#define CANT_TOTAL_PARAM_RANK 8
#define CANT_ELEMENTOS_LEIDOS 1
#define NEW_DB_FILE_NAME "db_modificada"
#define STR_ALTA "ALTA"
#define STR_BAJA "BAJA"
#define STR_MOD "MODIFICAR"
#define STR_ALTA_MIN "alta"
#define STR_BAJA_MIN "baja"
#define STR_MOD_MIN "modificar"

typedef enum
{
	OPERACION_ALTA,
	OPERACION_BAJA,
	OPERACION_MOD
} operacion_t;

state_t validarArg(operacion_t *ptr_operacion, int argc, const char *argv[], char (*db_filename)[FIELD_SIZE_STR], char (*op_filename)[FIELD_SIZE_STR], char (*log_filename)[FIELD_SIZE_STR]);

size_t obtener_pos_cla(char *pos_cla, int argc, const char *argv[]);

state_t realizarAlta(FILE *db, FILE *pf, FILE *log, FILE *temp);

state_t realizarBaja(FILE *db, FILE *pf, FILE *log, FILE *temp);

state_t realizarMod(FILE *db, FILE *pf, FILE *log, FILE *temp);

state_t leerValidar(size_t *n_db, peli_t *ptr_struct_read, FILE *pf_to_read, state_t *st);

state_t ValidarAlEscribir(peli_t *ptr_peli, FILE *file, state_t *st);

#endif