/*	
 *	 File:    bsd-dbm.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tcl.h"
#include "extral.h"
#include <fcntl.h>
#include "db_185.h"

int ExtraL_BsddbmCreate(
	Tcl_Interp *interp,
	Tcl_Obj *database,
	int mode)
{
	DB *dbf;
	char *name;
	int error;

	name = Tcl_GetStringFromObj(database,&error);

	dbf = dbopen(name, O_RDONLY, mode, DB_BTREE, (void *)NULL);
	if (dbf != NULL) {
		dbf->close(dbf);
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not create database \"",name, "\": exists already", (char *)NULL);
		return TCL_ERROR;
	}
	dbf = dbopen(name, O_RDWR|O_CREAT|O_EXCL, mode, DB_BTREE, (void *)NULL);
	if (dbf == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not create database \"",name, "\"",
			(char *)NULL);
		return TCL_ERROR;
	}
	dbf->close(dbf);
	return TCL_OK;
}

int ExtraL_BsddbmClose(ClientData token)
{
	DB *dbf = token;
	Tcl_DeleteExitHandler((Tcl_ExitProc *)ExtraL_BsddbmClose, token);
	dbf->close(dbf);
	return TCL_OK;
}

int ExtraL_BsddbmOpen(
	Tcl_Interp *interp,
	Tcl_Obj *database,
	int read_write,
	ClientData *token)
{
	DB *dbf;
	char *name;
	int error;

	name = Tcl_GetStringFromObj(database,&error);

	if (read_write == DBM_READ) {
		read_write = O_RDONLY;
	} else if (read_write == DBM_WRITE) {
		read_write = O_RDWR;
	} else {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: incorrect parameter for read_write", (char *)NULL);
		return TCL_ERROR;
	}

	dbf = dbopen(name, read_write, 0, DB_BTREE, NULL);
	if (dbf == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not open database \"",name, "\": ",
			(char *)NULL);
		return TCL_ERROR;
	}
	Tcl_CreateExitHandler((Tcl_ExitProc *)ExtraL_BsddbmClose, (ClientData)dbf);
	*token = (ClientData)dbf;
	return TCL_OK;
}

int ExtraL_BsddbmSet(
	Tcl_Interp *interp,
	ClientData token,
	Tcl_Obj *keyObj,
	Tcl_Obj *valueObj)
{
	DB *dbf = token;
	DBT key, value;
	int error;

	key.data = Tcl_GetStringFromObj(keyObj,&(key.size));
	value.data = Tcl_GetStringFromObj(valueObj,&(value.size));

	error = dbf->put(dbf,&key,&value,0);
	if (error == -1) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: trying to set value from reader", (char *)NULL);
		return TCL_ERROR;
	}
	
	return TCL_OK;
}

int ExtraL_BsddbmGet(
	Tcl_Interp *interp,
	ClientData token,
	Tcl_Obj *keyObj,
	Tcl_Obj *valueObj)
{
	DB *dbf = token;
	DBT key, value;
	int error;

	key.data = Tcl_GetStringFromObj(keyObj,&(key.size));

	error = dbf->get(dbf,&key,&value,0);
	if (error == 1) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: key \"", key.data, "\" not found", (char *)NULL);
		return TCL_ERROR;
	}
	Tcl_SetStringObj(valueObj,value.data,value.size);
	return TCL_OK;
}

int Bsddbm_Init(interp)
	Tcl_Interp *interp;
{
	DbmType dbmtype;
	dbmtype.create = ExtraL_BsddbmCreate; 
	dbmtype.open = ExtraL_BsddbmOpen;
	dbmtype.set = ExtraL_BsddbmSet;
	dbmtype.get = ExtraL_BsddbmGet;
	dbmtype.close = ExtraL_BsddbmClose;
	return ExtraL_DbmCreateType(interp,"bsddbm",dbmtype);
}

