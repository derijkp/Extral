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

static Tcl_HashTable dbmtypesTable;

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
	int read_write;
	char *buffer;
	int bufferlen;
} Fdbm_Info;

int ExtraL_FdbmCreate(
	Tcl_Interp *interp,
	Tcl_Obj *database,
	int mode)
{
	Fdbm_Info *fdbm;
	char *name;
	int namelen;
	int error;

	name = Tcl_GetStringFromObj(database,&namelen);

	error = Tcl_VarEval(interp,"file mkdir ",name,NULL);
	if (error != TCL_OK) {return error;}
	return TCL_OK;
}

int ExtraL_FdbmOpen(
	Tcl_Interp *interp,
	Tcl_Obj *database,
	int read_write,
	ClientData *token)
{
	Fdbm_Info *fdbm;
	char *name;
	int namelen;
	int error;

	name = Tcl_GetStringFromObj(database,&namelen);
	fdbm = (Fdbm_Info *)Tcl_Alloc(sizeof(Fdbm_Info));
	fdbm->dir = (char *)Tcl_Alloc((namelen+102)*sizeof(char));
	strncpy(fdbm->dir,name,namelen);
	fdbm->dir[namelen] = '/';
	fdbm->dirlen = namelen+1;
	fdbm->namebufferlen = 100;
	fdbm->buffer = (char *)Tcl_Alloc(10000*sizeof(char));
	fdbm->bufferlen = 10000;
	fdbm->read_write = read_write;
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
	FILE *file;
	char *name;
	datum key, value;
	int error;
	int i;

	if (fdbm->read_write == DBM_READ) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: trying to set value from reader", (char *)NULL);
		return TCL_ERROR;
	}
	key.dptr = Tcl_GetStringFromObj(keyObj,&(key.dsize));
	value.dptr = Tcl_GetStringFromObj(valueObj,&(value.dsize));
	if (key.dsize > fdbm->namebufferlen) {
		fdbm->dir = (char *)Tcl_Realloc(fdbm->dir,(fdbm->dirlen+key.dsize+1)*sizeof(char));
		fdbm->namebufferlen = key.dsize;
	}
	strncpy(fdbm->dir+fdbm->dirlen,key.dptr,key.dsize);
	fdbm->dir[fdbm->dirlen+key.dsize] = '\0';

	file = fopen(fdbm->dir,"w");
	if (file == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"could not store key \"",key.dptr ,"\"", (char *)NULL);
		return TCL_ERROR;
	}
	fwrite(value.dptr,sizeof(char),value.dsize,file);
	fclose(file);
	return TCL_OK;
}

int ExtraL_FdbmGet(
	Tcl_Interp *interp,
	ClientData token,
	Tcl_Obj *keyObj,
	Tcl_Obj *valueObj)
{
	Fdbm_Info *fdbm = token;
	FILE *file;
	char *name;
	datum key, value;
	int error;
	int i;

	key.dptr = Tcl_GetStringFromObj(keyObj,&(key.dsize));
	if (key.dsize > fdbm->namebufferlen) {
		fdbm->dir = (char *)Tcl_Realloc(fdbm->dir,(fdbm->dirlen+key.dsize+1)*sizeof(char));
		fdbm->namebufferlen = key.dsize;
	}
	strncpy(fdbm->dir+fdbm->dirlen,key.dptr,key.dsize);
	fdbm->dir[fdbm->dirlen+key.dsize] = '\0';
	Tcl_SetStringObj(valueObj,"",0);

	file = fopen(fdbm->dir,"r");
	if (file == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: key \"", key.dptr, "\" not found", (char *)NULL);
		return TCL_ERROR;
	}
	while(1) {
		i = fread(fdbm->buffer,sizeof(char),fdbm->bufferlen,file);
		if (i != 0) {
			Tcl_AppendToObj(valueObj,fdbm->buffer,i);
		}
		if (i<fdbm->bufferlen) break;
	}
	fclose(file);
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
			error = type->set(interp,token,objv[2],objv[3]);
			if (error != TCL_OK) {return error;}
			return TCL_OK;
		} else if (strcmp(cmd,"get") == 0) {
			if (objc != 3) {
				Tcl_WrongNumArgs(interp, 1, objv, "get key");
				return TCL_ERROR;
			}
			resultObj = Tcl_GetObjResult(interp);
			error = type->get(interp,token,objv[2],resultObj);
			if (error != TCL_OK) {return error;}
			return TCL_OK;
		}
	}
	Tcl_ResetResult(interp);
	Tcl_AppendResult(interp,"bad option \"", cmd, "\": must be one of set, get", (char *)NULL);
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
	int error;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "option args");
		return TCL_ERROR;
	}
	
	cmd = Tcl_GetStringFromObj(objv[1],&cmdlen);
	if (cmdlen == 4) {
		if (strcmp(cmd,"open") == 0) {
			DbmInfo *dbminfo;
			ClientData token;
			char *rw;
			int rwcode;
			if ((objc != 5)&&(objc != 6)) {
				Tcl_WrongNumArgs(interp, 1, objv, "open type dbcmd database ?read/write?");
				return TCL_ERROR;
			}
			if (objc == 6) {
				rw = Tcl_GetStringFromObj(objv[5],&error);
				if (rw[0]=='w') {
					rwcode = DBM_WRITE;
				} else {
					rwcode = DBM_READ;
				}
			} else {
				rwcode = DBM_READ;
			}
			typestring = Tcl_GetStringFromObj(objv[2],&error);
			dbminfo = ExtraL_DbmOpen(interp,typestring,objv[4],rwcode);
			if (dbminfo == NULL) {
				return TCL_ERROR;
			}
			type = dbminfo->type;
			Tcl_CreateObjCommand(interp,Tcl_GetStringFromObj(objv[3],&error),
				(Tcl_ObjCmdProc *)ExtraL_DbmObjectCmd,dbminfo,
				(Tcl_CmdDeleteProc *)ExtraL_DbmClose);
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp, Tcl_GetStringFromObj(objv[3],&error), (char *)NULL);
			return TCL_OK;
		}
	} else if (cmdlen == 5) {
		if (strcmp(cmd,"types") == 0) {
			Tcl_HashSearch *searchPtr;
			char *key;
			int mode;

			if (objc != 2) {
				Tcl_WrongNumArgs(interp, 1, objv, "types");
				return TCL_ERROR;
			}
			Tcl_ResetResult(interp);
			entry = Tcl_FirstHashEntry(&dbmtypesTable, searchPtr);
			while(1) {
				if (entry == NULL) break;
				key = Tcl_GetHashKey(&dbmtypesTable, entry);
				Tcl_AppendElement(interp,key);
				entry = Tcl_NextHashEntry(searchPtr);
			}
			return TCL_OK;
		}
	} else if (cmdlen == 6) {
		if (strcmp(cmd,"create") == 0) {
			int mode;
			if ((objc != 4)&&(objc != 5)) {
				Tcl_WrongNumArgs(interp, 1, objv, "create type database ?mode?");
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
			if (objc == 5) {
				error = Tcl_GetIntFromObj(interp,objv[4],&mode);
				if (error != TCL_OK) {return error;}
			} else {
				mode = 0666;
			}
			error = (type->create)(interp,objv[3],mode);
			if (error != TCL_OK) {return error;}
			return TCL_OK;
		}
	}
	Tcl_ResetResult(interp);
	Tcl_AppendResult(interp,"bad option \"", cmd, "\": must be one of create, open, types", (char *)NULL);
	return TCL_ERROR;
}

EXTERN DbmInfo *ExtraL_DbmOpen(
	Tcl_Interp *interp,
	char *typestring,
	Tcl_Obj *database,
	int rwcode)
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

	error = (type->open)(interp,database,rwcode,&token);
	dbminfo = (DbmInfo *)Tcl_Alloc(sizeof(DbmInfo));
	dbminfo->type = type;
	dbminfo->token = token;
	return dbminfo;
}

EXTERN int ExtraL_DbmCreateType (Tcl_Interp *interp,
	char *key,
	DbmType dbmtype)
{
	Tcl_HashEntry *entry, *newentry;
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
	type->set = dbmtype.set;
	type->get = dbmtype.get;
	type->close = dbmtype.close;
	return TCL_OK;
}

EXTERN int Extral_DbmInit(interp)
	Tcl_Interp *interp;
{
	DbmType dbmtype;
	Tcl_InitHashTable(&dbmtypesTable,TCL_STRING_KEYS);
	dbmtype.create = ExtraL_FdbmCreate; 
	dbmtype.open = ExtraL_FdbmOpen;
	dbmtype.set = ExtraL_FdbmSet;
	dbmtype.get = ExtraL_FdbmGet;
	dbmtype.close = ExtraL_FdbmClose;
	ExtraL_DbmCreateType(interp,"fdbm",dbmtype);
	Tcl_CreateObjCommand(interp,"dbm",(Tcl_ObjCmdProc *)ExtraL_DbmObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	return TCL_OK;
}

