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
	int objc,
	Tcl_Obj *CONST objv[])
{
	DB *dbf;
	char *name;
	DBTYPE dbtype;
	int namelen, mode, error, i;

	mode = 0666;
	dbtype = DB_BTREE;
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
		} else if (strcmp(name,"-btree") == 0) {
			dbtype = DB_BTREE;
		} else if (strcmp(name,"-hash") == 0) {
			dbtype = DB_HASH;
		} else {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"unknown option: \"",name ,"\"",
					", should be one of -mode, -btree, -hash", (char *)NULL);
				return TCL_ERROR;
		}
		i++;
	}

	name = Tcl_GetStringFromObj(database,&error);
	dbf = dbopen(name, O_RDONLY, mode, dbtype, (void *)NULL);
	if (dbf != NULL) {
		dbf->close(dbf);
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not create database \"",name, "\"", (char *)NULL);
		return TCL_ERROR;
	}
	dbf = dbopen(name, O_RDWR|O_CREAT|O_EXCL, mode, dbtype, (void *)NULL);
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
	ClientData *token,
	int readonly,
	int objc,
	Tcl_Obj *CONST objv[])
{
	DB *dbf;
	char *name;
	DBTYPE dbtype;
	int namelen, mode, error, i;

	dbtype = DB_BTREE;
	i = 0;
	while(i<objc) {
		name = Tcl_GetStringFromObj(objv[i],&namelen);
		if (strcmp(name,"-btree") == 0) {
			dbtype = DB_BTREE;
		} else if (strcmp(name,"-hash") == 0) {
			dbtype = DB_HASH;
		} else {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"unknown option: \"",name ,"\"",
					", should be one of -btree, -hash", (char *)NULL);
				return TCL_ERROR;
		}
		i++;
	}
	name = Tcl_GetStringFromObj(database,&error);

	if (readonly == 1) {
		readonly = O_RDONLY;
	} else {
		readonly = O_RDWR;
	}

	dbf = dbopen(name, readonly, 0, dbtype, NULL);
	if (dbf == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not open database \"",name, "\"",
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

int ExtraL_BsddbmUnset(
	Tcl_Interp *interp,
	ClientData token,
	Tcl_Obj *keyObj)
{
	DB *dbf = token;
	DBT key, value;
	int error;

	key.data = Tcl_GetStringFromObj(keyObj,&(key.size));

	error = dbf->del(dbf,&key,0);
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


int ExtraL_BsddbmKeys(
	Tcl_Interp *interp,
	ClientData token,
	char *pattern)
{
	DB *dbf = token;
	DBT key, value;
	Tcl_Obj *tempObj, *resultObj;
	char *temp;
	int flag,len,templen;
	int error;
	int i;

	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);
	len = 0;
	if (pattern != NULL) {
		while (pattern[len]!='\0') {
			if ((pattern[len]=='*')||(pattern[len]=='?')||
					(pattern[len]=='[')||(pattern[len]=='\\')) {
				break;
			}
			len++;
		}
	}
	key.data = pattern;
	key.size = len;
	if (len) flag = R_CURSOR;
	else flag = R_FIRST;

	error = dbf->seq(dbf, &key, &value, flag);
	while(1) {
		if (error == 1) {
			break;
		} else if (error == -1) {
			Tcl_AppendResult(interp,"error getting keys", (char *)NULL);
		}
		tempObj = Tcl_NewStringObj(key.data, key.size);
		temp = Tcl_GetStringFromObj(tempObj, &templen);
		if ((pattern == NULL)||(Tcl_StringMatch(temp, pattern) == 1)) {
			error = Tcl_ListObjAppendElement(interp,resultObj,tempObj);
			if (error != TCL_OK) {return TCL_ERROR;}
		} else {
			Tcl_DecrRefCount(tempObj);
		}
		error = dbf->seq(dbf, &key, &value, R_NEXT);
	}
	Tcl_SetObjResult(interp,resultObj);
	return TCL_OK;
}

int ExtraL_BsddbmSync(Tcl_Interp *interp,ClientData token)
{
	DB *dbf = token;
	dbf->sync(dbf,0);
	return TCL_OK;
}

int Bsddbm_Init(interp)
	Tcl_Interp *interp;
{
	DbmType dbmtype;
	dbmtype.create = ExtraL_BsddbmCreate; 
	dbmtype.open = ExtraL_BsddbmOpen;
	dbmtype.keys = ExtraL_BsddbmKeys;
	dbmtype.set = ExtraL_BsddbmSet;
	dbmtype.unset = ExtraL_BsddbmUnset;
	dbmtype.get = ExtraL_BsddbmGet;
	dbmtype.close = ExtraL_BsddbmClose;
	dbmtype.sync = ExtraL_BsddbmSync;
	dbmtype.reorganize = NULL;
	return ExtraL_DbmCreateType(interp,"bsddbm",dbmtype);
}

