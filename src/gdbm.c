/*	
 *	 File:    dbm.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tcl.h"
#include "extral.h"
#include "gdbm.h"

int ExtraL_GdbmCreate(
	Tcl_Interp *interp,
	Tcl_Obj *database,
	int mode)
{
	GDBM_FILE dbf;
	char *name;
	int error;

	name = Tcl_GetStringFromObj(database,&error);

	dbf = gdbm_open(name, 0, GDBM_READER, mode, NULL);
	if (dbf != NULL) {
		gdbm_close(dbf);
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not create database \"",name, "\": exists already", (char *)NULL);
		return TCL_ERROR;
	}
	dbf = gdbm_open(name, 0, GDBM_WRCREAT, mode, NULL);
	if (dbf == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not create database \"",name, "\"",
		(char *)NULL);
		return TCL_ERROR;
	}
	gdbm_close(dbf);
	return TCL_OK;
}

int ExtraL_GdbmOpen(
	Tcl_Interp *interp,
	Tcl_Obj *database,
	int read_write,
	ClientData *token)
{
	GDBM_FILE dbf;
	char *name;
	int error;

	name = Tcl_GetStringFromObj(database,&error);

	if (read_write == DBM_READ) {
		read_write = GDBM_READER;
	} else if (read_write == DBM_WRITE) {
		read_write = GDBM_WRITER;
	} else {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: incorrect parameter for read_write", (char *)NULL);
		return TCL_ERROR;
	}

	dbf = gdbm_open(name, 0, read_write, 0, NULL);
	if (dbf == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not open database \"",name, "\": ",
			gdbm_strerror(gdbm_errno), (char *)NULL);
		return TCL_ERROR;
	}
	*token = (ClientData)dbf;
	return TCL_OK;
}

int ExtraL_GdbmSet(
	Tcl_Interp *interp,
	ClientData token,
	Tcl_Obj *keyObj,
	Tcl_Obj *valueObj)
{
	GDBM_FILE dbf = token;
	datum key, value;
	int error;

	key.dptr = Tcl_GetStringFromObj(keyObj,&(key.dsize));
	value.dptr = Tcl_GetStringFromObj(valueObj,&(value.dsize));

	error = gdbm_store(dbf,key,value,GDBM_REPLACE);
	if (error == -1) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: trying to set value from reader", (char *)NULL);
		return TCL_ERROR;
	}
	
	return TCL_OK;
}

int ExtraL_GdbmGet(
	Tcl_Interp *interp,
	ClientData token,
	Tcl_Obj *keyObj,
	Tcl_Obj *valueObj)
{
	GDBM_FILE dbf = token;
	datum key, value;
	int error;

	key.dptr = Tcl_GetStringFromObj(keyObj,&(key.dsize));

	value = gdbm_fetch(dbf,key);
	if (value.dptr == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: key \"", key.dptr, "\" not found", (char *)NULL);
		return TCL_ERROR;
	}
	Tcl_SetStringObj(valueObj,value.dptr,value.dsize);
	free(value.dptr);
	return TCL_OK;
}

int ExtraL_GdbmClose(ClientData token)
{
	GDBM_FILE dbf = token;
	gdbm_close(dbf);
	return TCL_OK;
}

EXTERN int Gdbm_Init(interp)
	Tcl_Interp *interp;
{
	DbmType dbmtype;
	dbmtype.create = ExtraL_GdbmCreate; 
	dbmtype.open = ExtraL_GdbmOpen;
	dbmtype.set = ExtraL_GdbmSet;
	dbmtype.get = ExtraL_GdbmGet;
	dbmtype.close = ExtraL_GdbmClose;
	return ExtraL_DbmCreateType(interp,"gdbm",dbmtype);
}

