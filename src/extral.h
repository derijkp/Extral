/*	
 *	 File:    extral.h
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

typedef int ExtraL_TaglTypeProc _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *structure, Tcl_Obj **value));

EXTERN int ExtraL_TaglCreateType _ANSI_ARGS_((Tcl_Interp *interp,
	char *key,ExtraL_TaglTypeProc *setproc,ExtraL_TaglTypeProc *getproc));

/*
 * dbm functions and definitions
 */

typedef int ExtraL_DbmCreateProc _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *database,int objc,Tcl_Obj *CONST objv[]));

typedef int ExtraL_DbmOpenProc _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *database,ClientData *token,int readonly,int objc,Tcl_Obj *CONST objv[]));

typedef int ExtraL_DbmKeysProc _ANSI_ARGS_((Tcl_Interp *interp,
	ClientData token,char *pattern));

typedef int ExtraL_DbmSetProc _ANSI_ARGS_((Tcl_Interp *interp,
	ClientData token,Tcl_Obj *keyObj,Tcl_Obj *valueObj));

typedef int ExtraL_DbmUnsetProc _ANSI_ARGS_((Tcl_Interp *interp,
	ClientData token,Tcl_Obj *keyObj));

typedef int ExtraL_DbmGetProc _ANSI_ARGS_((Tcl_Interp *interp,
	ClientData token,Tcl_Obj *keyObj,Tcl_Obj *valueObj));

typedef int ExtraL_DbmSyncProc _ANSI_ARGS_((Tcl_Interp *interp,
	ClientData token));

typedef int ExtraL_DbmReorganizeProc _ANSI_ARGS_((Tcl_Interp *interp,
	ClientData token));

typedef int ExtraL_DbmCloseProc _ANSI_ARGS_((ClientData token));

typedef struct DbmType {
	ExtraL_DbmCreateProc *create;
	ExtraL_DbmOpenProc *open;
	ExtraL_DbmKeysProc *keys;
	ExtraL_DbmSetProc *set;
	ExtraL_DbmGetProc *get;
	ExtraL_DbmUnsetProc *unset;
	ExtraL_DbmCloseProc *close;
	ExtraL_DbmSyncProc *sync;
	ExtraL_DbmReorganizeProc *reorganize;
} DbmType;

typedef struct DbmInfo {
	DbmType *type;
	ClientData token;
	int readonly;
} DbmInfo;

EXTERN int ExtraL_DbmCreateType _ANSI_ARGS_((Tcl_Interp *interp,
	char *key,DbmType dbmtype));

EXTERN DbmInfo *ExtraL_DbmOpen(Tcl_Interp *interp,
	char *typestring,Tcl_Obj *database,int readonly,int objc,Tcl_Obj *CONST objv[]);
