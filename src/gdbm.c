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
	int objc,
	Tcl_Obj *CONST objv[])
{
	GDBM_FILE dbf;
	char *name;
	int namelen, mode, error, i;

	mode = 0666;
	i = 0;
	while(i<objc) {
		name = Tcl_GetStringFromObj(objv[i],&namelen);
		if (strcmp(name,"-mode") == 0) {
			i++;
			if (i>=objc) {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"no parameter given for option: \"",name ,"\"", (char *)NULL);
				return TCL_ERROR;
			}
			error = Tcl_GetIntFromObj(interp,objv[i],&mode);
			if (error != TCL_OK) {return error;}
		} else {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"unknown option: \"",name ,"\"",
					", should be one of -mode", (char *)NULL);
				return TCL_ERROR;
		}
		i++;
	}

	name = Tcl_GetStringFromObj(database,&error);
	dbf = gdbm_open(name, 0, GDBM_READER, mode, NULL);
	if (dbf != NULL) {
		gdbm_close(dbf);
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not create database \"",name, "\"", (char *)NULL);
		return TCL_ERROR;
	}
	dbf = gdbm_open(name, 0, GDBM_WRCREAT, mode, NULL);
	if (dbf == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not create database \"",name, "\"",(char *)NULL);
		return TCL_ERROR;
	}
	gdbm_close(dbf);
	return TCL_OK;
}

int ExtraL_GdbmOpen(
	Tcl_Interp *interp,
	Tcl_Obj *database,
	ClientData *token,
	int readonly,
	int objc,
	Tcl_Obj *CONST objv[])
{
	GDBM_FILE dbf;
	char *name;
	int namelen, mode, error, i;
	int fast,blocksize;

	fast = 0;
	blocksize = 0;
	i = 0;
	while(i<objc) {
		name = Tcl_GetStringFromObj(objv[i],&namelen);
		if (strcmp(name,"-blocksize") == 0) {
			i++;
			if (i>=objc) {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"no parameter given for option: \"",name ,"\"", (char *)NULL);
				return TCL_ERROR;
			}
			error = Tcl_GetIntFromObj(interp,objv[i],&blocksize);
			if (error != TCL_OK) {return error;}
		} else if (strcmp(name,"-fast") == 0) {
			fast = GDBM_FAST;
		} else {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"unknown option: \"",name ,"\"",
					", should be one of -fast or -blocksize", (char *)NULL);
				return TCL_ERROR;
		}
		i++;
	}
	if (readonly == 1) {
		readonly = GDBM_READER;
	} else {
		readonly = GDBM_WRITER;
	}

	name = Tcl_GetStringFromObj(database,&error);
	dbf = gdbm_open(name, blocksize, readonly, fast, NULL);
	if (dbf == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not open database \"",name, "\"", (char *)NULL);
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

int ExtraL_GdbmUnset(
	Tcl_Interp *interp,
	ClientData token,
	Tcl_Obj *keyObj)
{
	GDBM_FILE dbf = token;
	datum key, value;
	int error;

	key.dptr = Tcl_GetStringFromObj(keyObj,&(key.dsize));

	error = gdbm_delete(dbf,key);
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

int ExtraL_GdbmKeys(
	Tcl_Interp *interp,
	ClientData token,
	char *pattern)
{
	GDBM_FILE dbf = token;
	datum key;
	Tcl_Obj *tempObj, *resultObj;
	char *temp;
	int templen;
	int error;

	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);

	key = gdbm_firstkey(dbf);
	while(1) {
		if (key.dptr == NULL) break;
		tempObj = Tcl_NewStringObj(key.dptr, key.dsize);
		temp = Tcl_GetStringFromObj(tempObj, &templen);
		if ((pattern == NULL)||(Tcl_StringMatch(temp, pattern) == 1)) {
			error = Tcl_ListObjAppendElement(interp,resultObj,tempObj);
			if (error != TCL_OK) {return TCL_ERROR;}
		} else {
			Tcl_DecrRefCount(tempObj);
		}
		key = gdbm_nextkey(dbf,key);
	}
	Tcl_SetObjResult(interp,resultObj);
	return TCL_OK;
}

int ExtraL_GdbmClose(ClientData token)
{
	GDBM_FILE dbf = token;
	gdbm_close(dbf);
	return TCL_OK;
}

int ExtraL_GdbmSync(Tcl_Interp *interp,ClientData token)
{
	GDBM_FILE dbf = token;
	gdbm_sync(dbf);
	return TCL_OK;
}

int ExtraL_Gdbmreorganize(Tcl_Interp *interp,ClientData token)
{
	GDBM_FILE dbf = token;
	gdbm_reorganize(dbf);
	return TCL_OK;
}

EXTERN int Gdbm_Init(interp)
	Tcl_Interp *interp;
{
	DbmType dbmtype;
	dbmtype.create = ExtraL_GdbmCreate; 
	dbmtype.open = ExtraL_GdbmOpen;
	dbmtype.keys = ExtraL_GdbmKeys;
	dbmtype.set = ExtraL_GdbmSet;
	dbmtype.unset = ExtraL_GdbmUnset;
	dbmtype.get = ExtraL_GdbmGet;
	dbmtype.close = ExtraL_GdbmClose;
	dbmtype.sync = ExtraL_GdbmSync;
	dbmtype.reorganize = ExtraL_Gdbmreorganize;
	return ExtraL_DbmCreateType(interp,"gdbm",dbmtype);
}

