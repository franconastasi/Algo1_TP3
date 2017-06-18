#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "comunes.h"
#include "ranking.h"

int main(int argc, char const *argv[])
{
	operacion_t operacion;
	
	char db_filename[FIELD_SIZE_STR], op_filename[FIELD_SIZE_STR], log_filename[FIELD_SIZE_STR];

	state_t st;
	
	FILE *db, *pf, *log, *temp;

	if(handle_error(validarArg(&operacion, argc, &argv[0], &db_filename, &op_filename , &log_filename)) != ST_OK)
		return EXIT_FAILURE;
	if (handle_error(openFile(&db, db_filename, "rb"))  != ST_OK) return EXIT_FAILURE;
	if (handle_error(openFile(&pf, op_filename, "rb")) != ST_OK) {
		fclose(db);
		return EXIT_FAILURE;
	}
	if (handle_error(openFile(&log, log_filename, "ab")) != ST_OK) {
		fclose(db);
		fclose(pf);		
		return EXIT_FAILURE;
	}
	if (handle_error(openFile(&temp, NEW_DB_FILE_NAME , "wb")) != ST_OK) {
		fclose(db);
		fclose(pf);
		fclose(log);
		return EXIT_FAILURE;
	}
	switch(operacion)
	{
		case OPERACION_ALTA:
			st = realizarAlta(db, pf, log, temp);
			break;
		case OPERACION_BAJA:
			st = realizarBaja(db, pf, log, temp);
			break;
		case OPERACION_MOD:
			st = realizarMod(db, pf, log, temp);
			break;
	}
	fclose(db);
	fclose(pf);
	fclose(log);
	fclose(temp);

	if (st != ST_OK)
	{
		handle_error (st);
		return EXIT_FAILURE;
	}
	remove(db_filename);
	rename(NEW_DB_FILE_NAME, db_filename);
	return EXIT_SUCCESS;
}

/* A validarAlEscribir se le pasa un puntero a state_t para que en caso de que se quiera añadir otras validaciones, no
	compliqué la escalabilidad del programa (solo hay que agregar la validación y cómo al final la devolvés)
*/

state_t ValidarAlEscribir(peli_t *ptr_peli, FILE *temp_file, state_t *st)
{
	if(!ptr_peli) return ST_ERROR_NULL_PTR;

	if ((fwrite(ptr_peli, sizeof(peli_t), 1, temp_file)) != 1 )
	{
		*st = ST_ERROR_WRITE_FILE;
	}else{
		*st = ST_OK;
	}
	return *st;
}


state_t realizarAlta(FILE *db, FILE *pf, FILE *log, FILE *temp)
{
	peli_t db_struct, file_struct;

	state_t st;

	size_t n_db, n_pf;
	/* No se hacen validaciones sobre los punteros porque ya se hicieron en open file*/

	n_db = fread(&db_struct, sizeof(peli_t), 1, db);
	n_pf = fread(&file_struct, sizeof(peli_t), 1, pf);

	while(n_db && n_pf)
	{
		if(db_struct.id < file_struct.id)
		{
			/*fwrite(&db_struct, sizeof(peli_t), 1, temp)*/;
			if ((ValidarAlEscribir(&db_struct, temp ,&st)) != ST_OK) return st;
			if (leerValidar(&n_db, &db_struct, db, &st) != ST_OK) return st;
		}
		else if(db_struct.id == file_struct.id)
		{
			/*fwrite(&db_struct, sizeof(peli_t), 1, temp);*/
			if ((ValidarAlEscribir(&db_struct, temp ,&st)) != ST_OK) return st;
			fprintf(stderr, "%s:%s\n", MSG_ERROR_PREFIX, MSG_ERROR_ALTA);
			if ((ValidarAlEscribir(&file_struct, log ,&st)) != ST_OK) return st;
			/*fwrite(&file_struct, sizeof(peli_t), 1, log);*/
			if(leerValidar(&n_db, &db_struct, db, &st) != ST_OK) return st;
			if(leerValidar(&n_pf, &file_struct, pf, &st) != ST_OK) return st;
		}
		else
		{
			if ((ValidarAlEscribir(&file_struct, temp ,&st)) != ST_OK) return st;
			/*fwrite(&file_struct, sizeof(peli_t), 1, temp);*/
			if(leerValidar(&n_pf, &file_struct, pf, &st) != ST_OK) return st;
		}
	}
	while(n_db)
	{
		if ((ValidarAlEscribir(&db_struct, temp ,&st)) != ST_OK) return st;
		/*fwrite(&db_struct, sizeof(peli_t), 1, temp);*/
		if(leerValidar(&n_db, &db_struct, db, &st) != ST_OK) return st;
	}
	while(n_pf)
	{
		if ((ValidarAlEscribir(&file_struct, temp ,&st)) != ST_OK) return st;
		/*fwrite(&file_struct, sizeof(peli_t), 1, temp);*/
		if(leerValidar(&n_pf, &file_struct, pf, &st) != ST_OK) return st;
	}
	
	return ST_OK;
}


state_t realizarBaja(FILE *db, FILE *pf, FILE *log, FILE *temp)
{
	peli_t db_struct, file_struct;

	state_t st;

	size_t n_db, n_pf;

	n_db = fread(&db_struct, sizeof(peli_t), 1, db);
	n_pf = fread(&file_struct, sizeof(peli_t), 1, pf);

	while(n_db && n_pf)
	{
		if(db_struct.id < file_struct.id)
		{
			if ((ValidarAlEscribir(&db_struct, temp ,&st) ) != ST_OK ) return st;
			/*fwrite(&file_struct, sizeof(peli_t), 1, temp);*/
			if(leerValidar(&n_db, &db_struct, db, &st) != ST_OK) return st;
		}
		else if(db_struct.id == file_struct.id)
		{
			if(leerValidar(&n_db, &db_struct, db, &st) != ST_OK) return st;
			if(leerValidar(&n_pf, &file_struct, pf, &st) != ST_OK) return st;
		}
		else
		{
			fprintf(stderr, "%s:%s\n", MSG_ERROR_PREFIX, MSG_ERROR_BAJA);
			if ((ValidarAlEscribir(&file_struct, log ,&st) ) != ST_OK ) return st;
			/*fwrite(&file_struct, sizeof(peli_t), 1, log);*/
			if(leerValidar(&n_pf, &file_struct, pf, &st) != ST_OK) return st;	
		}
	}
	while(n_db)
	{
		if ((ValidarAlEscribir(&db_struct, temp ,&st) ) != ST_OK ) return st;
		/*fwrite(&db_struct, sizeof(peli_t), 1, temp);*/
		if(leerValidar(&n_db, &db_struct, db, &st) != ST_OK) return st;
	}
	while(n_pf)
	{

		fprintf(stderr, "%s:%s\n", MSG_ERROR_PREFIX, MSG_ERROR_BAJA);
		if ((ValidarAlEscribir(&file_struct, log ,&st) ) != ST_OK ) return st;
		/*fwrite(&file_struct, sizeof(peli_t), 1, log);*/
		if(leerValidar(&n_pf, &file_struct, pf, &st) != ST_OK) return st;
	}
	/*while(fread(&db_temp, sizeof(peli_t), 1, temp) != EOF){
		if((ValidarAlEscribir(&db_temp,db,&st)) != ST_OK) return st;
		fwrite(&db_temp, sizeof(peli_t), 1, db);
	}*/
	
	return ST_OK;
}


state_t realizarMod(FILE *db, FILE *pf, FILE *log, FILE *temp)
{
	peli_t db_struct, file_struct;

	state_t st;
	
	size_t n_db, n_pf;
	/* No se hacen validaciones sobre los punteros porque ya se hicieron en open file*/

	n_db = fread(&db_struct, sizeof(peli_t), 1, db);
	n_pf = fread(&file_struct, sizeof(peli_t), 1, pf);

	while(n_db && n_pf)
	{
		if(db_struct.id < file_struct.id)
		{
			if ((ValidarAlEscribir(&db_struct, temp ,&st) ) != ST_OK ) return st;
			/*fwrite(&db_struct, sizeof(peli_t), 1, temp);*/
			if(leerValidar(&n_db, &db_struct, db, &st) != ST_OK) return st;
		}	
		else if(db_struct.id == file_struct.id)
		{
			if ((ValidarAlEscribir(&file_struct, temp ,&st) ) != ST_OK ) return st;
			/*fwrite(&file_struct, sizeof(peli_t), 1, temp);*/
			if(leerValidar(&n_db, &db_struct, db, &st) != ST_OK) return st;
			if(leerValidar(&n_db, &file_struct, pf, &st) != ST_OK) return st;
		}	
		else
			{
				/*fwrite(&db_struct, sizeof(peli_t), 1, temp);*/
				/*imprimir_peli(&file_struct, log);*/
				fprintf(stderr, "%s:%s\n", MSG_ERROR_PREFIX, MSG_ERROR_MOD);
				if ((ValidarAlEscribir(&file_struct, log ,&st) ) != ST_OK ) return st;
				if(leerValidar(&n_pf, &file_struct, pf, &st) != ST_OK) return st;
			}
	}
	while(n_db)
	{
		if ((ValidarAlEscribir(&db_struct, temp ,&st) ) != ST_OK ) return st;
		if(leerValidar(&n_db, &db_struct, db, &st) != ST_OK) return st;
	}
	while(n_pf)
	{
		fprintf(stderr, "%s:%s\n", MSG_ERROR_PREFIX, MSG_ERROR_MOD);
		if ((ValidarAlEscribir(&file_struct, log ,&st) ) != ST_OK ) return st;
		if(leerValidar(&n_pf, &file_struct, pf, &st) != ST_OK) return st;
	}
	/*while(fread(&db_temp, sizeof(peli_t), 1, temp) != EOF){
		fwrite(&db_temp, sizeof(peli_t), 1, db);
	}*/
	
	return ST_OK;
}


state_t validarArg(operacion_t *ptr_operacion, int argc, const char *argv[], char (*db_filename)[FIELD_SIZE_STR], char (*op_filename)[FIELD_SIZE_STR], char (*log_filename)[FIELD_SIZE_STR])
{
	int pos_db_cla, pos_file_cla, pos_log_cla;
	
	pos_db_cla = obtener_pos_cla(COMMAND_DB, argc, argv);
	pos_file_cla = obtener_pos_cla(COMMAND_FILE, argc, argv);
	pos_log_cla = obtener_pos_cla(COMMAND_LOG, argc, argv);

	if(handle_error(validarCantArg(CANT_TOTAL_PARAM_RANK, argc))!= ST_OK)
	{
		return ST_ERROR_INVALID_CANT_ARG;
	}

	if(pos_db_cla == (argc)  || pos_file_cla == (argc) || pos_log_cla == (argc) )
	{

		return ST_ERROR_PARAM_NOT_FOUND;
	}
	strcpy(*db_filename, argv[pos_db_cla]);
	strcpy(*op_filename,argv[pos_file_cla]);
	strcpy(*log_filename, argv[pos_log_cla]);

	if((strcmp(argv[POS_COMMAND_CLA], STR_ALTA) == 0) || (strcmp(argv[POS_COMMAND_CLA], STR_ALTA_MIN) == 0))
	{
		*ptr_operacion = OPERACION_ALTA;
	}
	else if((strcmp(argv[POS_COMMAND_CLA], STR_BAJA) == 0) || (strcmp(argv[POS_COMMAND_CLA], STR_BAJA_MIN) == 0))
	{
		*ptr_operacion = OPERACION_BAJA;
	}
	else if((strcmp(argv[POS_COMMAND_CLA], STR_MOD) == 0) || (strcmp(argv[POS_COMMAND_CLA], STR_MOD_MIN) == 0))
	{
		*ptr_operacion = OPERACION_MOD;
	}
	else
	{
		return ST_ERROR_COMMAND;
	}
	return ST_OK;
}

size_t obtener_pos_cla(char *pos_cla, int argc, const char *argv[])
{
    int i;
    for (i = 0; i <= argc; ++i)
    {
    	/* se deja el menor o igual para usar ek valor argc como validación*/
        if (strcmp(pos_cla, argv[i]) == 0)
        {
            return (i + 1);
        }
    }
    return i;
}

state_t leerValidar(size_t *n, peli_t *ptr_struct_read, FILE *pf_to_read, state_t* st)
{
	if((*n = fread(ptr_struct_read, sizeof(peli_t), 1, pf_to_read)) != 1)
	{
		if(ferror(pf_to_read))
		{
			*st = ST_ERROR_UNABLE_TO_READ_FROM_FILE;			
		}
	}else
		{
			*st = ST_OK;
		}
	return *st;
}
