/*	
 *	 File:    dbm.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include <string.h>
#include "tcl.h"
#include "extral.h"

#ifdef unix
#define export
#endif

Tcl_HashTable dbmtypesTable;

/*
 * fdbm implementation
 */

typedef struct Datum {
	char *dptr;
	int dsize;
} datum;

typedef struct Fdbm_Info_str {
	char *dir;
	int dirlen;
	int namebufferlen;
	char *buffer;
	int bufferlen;
} Fdbm_Info;

int ExtraL_FdbmCreate(
	Tcl_Interp *interp,
	Tcl_Obj *database,
	int objc,
	Tcl_Obj *CONST objv[])
{
	char *name;
	int namelen;
	int error;

	if (objc != 0) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"fdbm create has no options", (char *)NULL);
		return TCL_ERROR;
	}
	name = Tcl_GetStringFromObj(database,&namelen);
	error = Tcl_VarEval(interp,"if [file exists {",name,"}] {error {could not create database \"",name,"\"}}",NULL);
	if (error != TCL_OK) {return error;}
	error = Tcl_VarEval(interp,"file mkdir {",name,"}",NULL);
	if (error != TCL_OK) {return error;}
	return TCL_OK;
}

int ExtraL_FdbmOpen(
	Tcl_Interp *interp,
	Tcl_Obj *database,
	ClientData *token,
	int readonly,
	int objc,
	Tcl_Obj *CONST objv[])
{
	Fdbm_Info *fdbm;
	char *name;
	int namelen;
	int error;

	if (objc != 0) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"fdbm open has no options", (char *)NULL);
		return TCL_ERROR;
	}
	name = Tcl_GetStringFromObj(database,&namelen);
	fdbm = (Fdbm_Info *)Tcl_Alloc(sizeof(Fdbm_Info));
	fdbm->dir = (char *)Tcl_Alloc((namelen+102)*sizeof(char));
	strncpy(fdbm->dir,name,namelen);
	fdbm->dir[namelen] = '\0';
	error = Tcl_VarEval(interp,"if ![file exists ",fdbm->dir,"] {error {could not open database \"",fdbm->dir,"\"}}",NULL);
	fdbm->dir[namelen] = '/';
	fdbm->dirlen = namelen+1;
	fdbm->namebufferlen = 100;
	fdbm->buffer = (char *)Tcl_Alloc(10000*sizeof(char));
	fdbm->bufferlen = 10000;
	if (error != TCL_OK) {
		Tcl_Free((char *)fdbm->dir);
		Tcl_Free((char *)fdbm);
		return error;
	}
	*token = (ClientData)fdbm;
	return TCL_OK;
}

int ExtraL_FdbmClose(ClientData token)
{
	Fdbm_Info *fdbm = token;
	Tcl_Free(fdbm->dir);
	Tcl_Free(fdbm->buffer);
	Tcl_Free((char *)fdbm);
	return TCL_OK;
}

int ExtraL_FdbmSet(
	Tcl_Interp *interp,
	ClientData token,
	Tcl_Obj *keyObj,
	Tcl_Obj *valueObj)
{
	Fdbm_Info *fdbm = token;
	Tcl_Channel file;
	datum key, value;

	key.dptr = Tcl_GetStringFromObj(keyObj,&(key.dsize));
	value.dptr = Tcl_GetStringFromObj(valueObj,&(value.dsize));
	if (key.dsize > fdbm->namebufferlen) {
		fdbm->dir = (char *)Tcl_Realloc(fdbm->dir,(fdbm->dirlen+key.dsize+1)*sizeof(char));
		fdbm->namebufferlen = key.dsize;
	}
	strncpy(fdbm->dir+fdbm->dirlen,key.dptr,key.dsize);
	fdbm->dir[fdbm->dirlen+key.dsize] = '\0';

	file = Tcl_OpenFileChannel(interp,fdbm->dir,"w",0666);
	if (file == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not store key \"",key.dptr ,"\"", (char *)NULL);
		return TCL_ERROR;
	}
	Tcl_Write(file, value.dptr, value.dsize);
	Tcl_Close(interp,file);
	return TCL_OK;
}

int ExtraL_FdbmGet(
	Tcl_Interp *interp,
	ClientData token,
	Tcl_Obj *keyObj,
	Tcl_Obj *valueObj)
{
	Fdbm_Info *fdbm = token;
	Tcl_Channel file;
	datum key;
	int i;

	key.dptr = Tcl_GetStringFromObj(keyObj,&(key.dsize));
	if (key.dsize > fdbm->namebufferlen) {
		fdbm->dir = (char *)Tcl_Realloc(fdbm->dir,(fdbm->dirlen+key.dsize+1)*sizeof(char));
		fdbm->namebufferlen = key.dsize;
	}
	strncpy(fdbm->dir+fdbm->dirlen,key.dptr,key.dsize);
	fdbm->dir[fdbm->dirlen+key.dsize] = '\0';
	Tcl_SetStringObj(valueObj,"",0);

	file = Tcl_OpenFileChannel(interp, fdbm->dir, "r", 0666);
	if (file == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: key \"", key.dptr, "\" not found", (char *)NULL);
		return TCL_ERROR;
	}
	while(1) {
		i = Tcl_Read(file,fdbm->buffer,fdbm->bufferlen);
		if (i != 0) {
			Tcl_AppendToObj(valueObj,fdbm->buffer,i);
		}
		if (i<fdbm->bufferlen) break;
	}
	Tcl_Close(interp,file);
	return TCL_OK;
}

int ExtraL_FdbmKeys(
	Tcl_Interp *interp,
	ClientData token,
	char *pattern)
{
	Fdbm_Info *fdbm = token;
	int error;

	if (pattern == NULL) {
		pattern = "*";
	}
	fdbm->dir[fdbm->dirlen] = '\0';
	error = Tcl_VarEval(interp,"dirglob ",fdbm->dir," ",pattern,(char *)NULL);
	if (error != TCL_OK) {return error;}
	return TCL_OK;
}

int ExtraL_FdbmUnset(
	Tcl_Interp *interp,
	ClientData token,
	Tcl_Obj *keyObj)
{
	Fdbm_Info *fdbm = token;
	datum key;
	int error;

	key.dptr = Tcl_GetStringFromObj(keyObj,&(key.dsize));
	if (key.dsize > fdbm->namebufferlen) {
		fdbm->dir = (char *)Tcl_Realloc(fdbm->dir,(fdbm->dirlen+key.dsize+1)*sizeof(char));
		fdbm->namebufferlen = key.dsize;
	}
	strncpy(fdbm->dir+fdbm->dirlen,key.dptr,key.dsize);
	fdbm->dir[fdbm->dirlen+key.dsize] = '\0';
	error = Tcl_VarEval(interp,"file delete {",fdbm->dir,"}",NULL);
	return TCL_OK;
}

/* 
 * general Dbm routines from here
 * ------------------------------
 */

int ExtraL_DbmObjectCmd(
	ClientData dbminfo,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj *CONST objv[])
{
	DbmType *type = ((DbmInfo *)dbminfo)->type;
	ClientData token = ((DbmInfo *)dbminfo)->token;
	Tcl_Obj *resultObj;
	char *cmd;
	int cmdlen;
	char *name;
	int namelen;
	int error;
	

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "option ...");
		return TCL_ERROR;
	}

	cmd = Tcl_GetStringFromObj(objv[1],&cmdlen);
	if (cmdlen == 3) {
		if (strcmp(cmd,"set") == 0) {
			if (objc != 4) {
				Tcl_WrongNumArgs(interp, 1, objv, "set key value");
				return TCL_ERROR;
			}
			if (((DbmInfo *)dbminfo)->readonly) {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"error: trying to set value from reader", (char *)NULL);
				return TCL_ERROR;
			}
			error = type->set(interp,token,objv[2],objv[3]);
			if (error != TCL_OK) {return error;}
			return TCL_OK;
		} else if (strcmp(cmd,"get") == 0) {
			if ((objc != 3)&&(objc != 4)) {
				Tcl_WrongNumArgs(interp, 1, objv, "get key ?default?");
				return TCL_ERROR;
			}
			resultObj = Tcl_GetObjResult(interp);
			error = type->get(interp,token,objv[2],resultObj);
			if (error != TCL_OK) {
				if (objc == 3) {
					return error;
				} else {
					Tcl_ResetResult(interp);
					Tcl_SetObjResult(interp,objv[3]);
					return TCL_OK;
				}
			}
			return TCL_OK;
		}
	} else if (cmdlen == 4) {
		if (strcmp(cmd,"keys") == 0) {
			if ((objc != 2)&&(objc != 3)) {
				Tcl_WrongNumArgs(interp, 1, objv, "keys ?pattern?");
				return TCL_ERROR;
			}
			if (objc == 3) {
				name = Tcl_GetStringFromObj(objv[2],&namelen);
			} else {
				name = NULL;
			}
			error = type->keys(interp,token,name);
			if (error != TCL_OK) {return error;}
			return TCL_OK;
		} else if (strcmp(cmd,"sync") == 0) {
			if (objc != 2) {
				Tcl_WrongNumArgs(interp, 1, objv, "sync");
				return TCL_ERROR;
			}
			if (type->sync != NULL) {
				error = type->sync(interp,token);
				if (error != TCL_OK) {return error;}
			}
			return TCL_OK;
		}
	} else if (cmdlen == 5) {
		if (strcmp(cmd,"unset") == 0) {
			if (objc !=3) {
				Tcl_WrongNumArgs(interp, 1, objv, "unset key");
				return TCL_ERROR;
			}
			if (((DbmInfo *)dbminfo)->readonly) {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"error: trying to unset value from reader", (char *)NULL);
				return TCL_ERROR;
			}
			error = type->unset(interp,token,objv[2]);
			if (error != TCL_OK) {return error;}
			return TCL_OK;
		} else if (strcmp(cmd,"close") == 0) {
			if (objc != 2) {
				Tcl_WrongNumArgs(interp, 1, objv, "close");
				return TCL_ERROR;
			}
			error = Tcl_DeleteCommand(interp, Tcl_GetStringFromObj(objv[0],&error));
			if (error != TCL_OK) {return error;}
			Tcl_ResetResult(interp);
			return TCL_OK;
		}
	} else if (cmdlen == 10) {
		if (strcmp(cmd,"reorganize") == 0) {
			if (objc != 2) {
				Tcl_WrongNumArgs(interp, 1, objv, "reorganize");
				return TCL_ERROR;
			}
			if (type->reorganize != NULL) {
				error = type->reorganize(interp,token);
				if (error != TCL_OK) {return error;}
			}
			return TCL_OK;
		}
	}
	Tcl_ResetResult(interp);
	Tcl_AppendResult(interp,"bad option \"", cmd, "\": must be one of set, get, unset, keys, sync or reorganize", (char *)NULL);
	return TCL_ERROR;
}

void ExtraL_DbmClose(ClientData dbminfo)
{
	DbmType *type = ((DbmInfo *)dbminfo)->type;
	ClientData token = ((DbmInfo *)dbminfo)->token;
	type->close(token);
	Tcl_Free(dbminfo);
}

int ExtraL_DbmObjCmd(
	ClientData notUsed,
	Tcl_Interp *interp,
	int objc,
	Tcl_Obj *CONST objv[])
{
	Tcl_HashEntry *entry;
	DbmType *type;
	char *typestring;
	char *cmd;
	int cmdlen;
	char *name;
	int namelen, readonly;
	int error;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "option args");
		return TCL_ERROR;
	}
	
	cmd = Tcl_GetStringFromObj(objv[1],&cmdlen);
	if (cmdlen == 4) {
		if (strcmp(cmd,"open") == 0) {
			DbmInfo *dbminfo;
			if (objc < 5) {
				Tcl_WrongNumArgs(interp, 1, objv, "open ?-readonly? type dbcmd database ?options?");
				return TCL_ERROR;
			}
			name = Tcl_GetStringFromObj(objv[2],&namelen);
			if (strcmp(name,"-readonly") == 0) {
				readonly = 1;
				objc -= 3;
				objv += 3;
			} else {
				readonly = 0;
				objc -= 2;
				objv += 2;
			}
			typestring = Tcl_GetStringFromObj(objv[0],&error);
			dbminfo = ExtraL_DbmOpen(interp,typestring,objv[2],readonly,objc-3,objv+3);
			if (dbminfo == NULL) {
				return TCL_ERROR;
			}
			type = dbminfo->type;
			Tcl_CreateObjCommand(interp,Tcl_GetStringFromObj(objv[1],&error),
				(Tcl_ObjCmdProc *)ExtraL_DbmObjectCmd,dbminfo,
				(Tcl_CmdDeleteProc *)ExtraL_DbmClose);
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp, Tcl_GetStringFromObj(objv[1],&error), (char *)NULL);
			return TCL_OK;
		}
	} else if (cmdlen == 5) {
		if (strcmp(cmd,"types") == 0) {
			Tcl_HashSearch search;
			char *key;

			if (objc != 2) {
				Tcl_WrongNumArgs(interp, 1, objv, "types");
				return TCL_ERROR;
			}
			Tcl_ResetResult(interp);
			entry = Tcl_FirstHashEntry(&dbmtypesTable, &search);
			while(1) {
				if (entry == NULL) break;
				key = Tcl_GetHashKey(&dbmtypesTable, entry);
				Tcl_AppendElement(interp,key);
				entry = Tcl_NextHashEntry(&search);
			}
			return TCL_OK;
		}
	} else if (cmdlen == 6) {
		if (strcmp(cmd,"create") == 0) {
			if (objc < 4) {
				Tcl_WrongNumArgs(interp, 1, objv, "create type database ?options?");
				return TCL_ERROR;
			}
			typestring = Tcl_GetStringFromObj(objv[2],&error);
			entry = Tcl_FindHashEntry(&dbmtypesTable, typestring);
			if (entry == NULL) {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"no such type: \"",typestring ,"\"", (char *)NULL);
				return TCL_ERROR;
			}
			type = (DbmType *)Tcl_GetHashValue(entry);
			error = (type->create)(interp,objv[3],objc-4,objv+4);
			if (error != TCL_OK) {return error;}
			return TCL_OK;
		}
	}
	Tcl_ResetResult(interp);
	Tcl_AppendResult(interp,"bad option \"", cmd, "\": must be one of create, open or types", (char *)NULL);
	return TCL_ERROR;
}

export DbmInfo *ExtraL_DbmOpen(
	Tcl_Interp *interp,
	char *typestring,
	Tcl_Obj *database,
	int readonly,
	int objc,
	Tcl_Obj *CONST objv[])
{
	Tcl_HashEntry *entry;
	DbmType *type;
	ClientData token;
	DbmInfo *dbminfo;
	int error;

	entry = Tcl_FindHashEntry(&dbmtypesTable, typestring);
	if (entry == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"no such type: \"",typestring ,"\"", (char *)NULL);
		return NULL;
	}
	type = (DbmType *)Tcl_GetHashValue(entry);

	error = type->open(interp,database,&token,readonly,objc,objv);
	if (error != TCL_OK) {return NULL;}
	dbminfo = (DbmInfo *)Tcl_Alloc(sizeof(DbmInfo));
	dbminfo->type = type;
	dbminfo->token = token;
	dbminfo->readonly = readonly;
	return dbminfo;
}

export int ExtraL_DbmCreateType(
	Tcl_Interp *interp,
	char *key,
	DbmType dbmtype)
{
	Tcl_HashEntry *entry;
	DbmType *type;
	int new;

	entry = Tcl_CreateHashEntry(&dbmtypesTable,key,&new);
	if (new == 1) {
		type = (DbmType *)Tcl_Alloc(sizeof(DbmType));
		Tcl_SetHashValue(entry,(ClientData)type);
	} else {
		type = (DbmType *)Tcl_GetHashValue(entry);
	}
	type->create = dbmtype.create;
	type->open = dbmtype.open;
	type->keys = dbmtype.keys;
	type->set = dbmtype.set;
	type->get = dbmtype.get;
	type->close = dbmtype.close;
	type->unset = dbmtype.unset;
	type->sync = dbmtype.sync;
	type->reorganize = dbmtype.reorganize;
	return TCL_OK;
}

int Extral_DbmInit(interp)
	Tcl_Interp *interp;
{
	DbmType dbmtype;
	Tcl_InitHashTable(&dbmtypesTable,TCL_STRING_KEYS);
	dbmtype.create = ExtraL_FdbmCreate; 
	dbmtype.open = ExtraL_FdbmOpen;
	dbmtype.keys = ExtraL_FdbmKeys;
	dbmtype.set = ExtraL_FdbmSet;
	dbmtype.get = ExtraL_FdbmGet;
	dbmtype.close = ExtraL_FdbmClose;
	dbmtype.unset = ExtraL_FdbmUnset;
	dbmtype.sync = NULL;
	dbmtype.reorganize = NULL;
	ExtraL_DbmCreateType(interp,"fdbm",dbmtype);
	Tcl_CreateObjCommand(interp,"Extral::dbm",(Tcl_ObjCmdProc *)ExtraL_DbmObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	return TCL_OK;
}

