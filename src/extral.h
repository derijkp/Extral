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

#define DBM_READ 1
#define DBM_WRITE 2
typedef int ExtraL_DbmCreateProc _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *database,int mode));

typedef int ExtraL_DbmOpenProc _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *database,int read_write,ClientData *token));

typedef int ExtraL_DbmSetProc _ANSI_ARGS_((Tcl_Interp *interp,
	ClientData token,Tcl_Obj *keyObj,Tcl_Obj *valueObj));

typedef int ExtraL_DbmGetProc _ANSI_ARGS_((Tcl_Interp *interp,
	ClientData token,Tcl_Obj *keyObj,Tcl_Obj *valueObj));

typedef int ExtraL_DbmCloseProc _ANSI_ARGS_((ClientData token));

typedef struct DbmType {
	ExtraL_DbmCreateProc *create;
	ExtraL_DbmOpenProc *open;
	ExtraL_DbmSetProc *set;
	ExtraL_DbmGetProc *get;
	ExtraL_DbmCloseProc *close;
} DbmType;

typedef struct DbmInfo {
	DbmType *type;
	ClientData token;
} DbmInfo;

EXTERN int ExtraL_DbmCreateType _ANSI_ARGS_((Tcl_Interp *interp,
	char *key,DbmType dbmtype));

EXTERN DbmInfo *ExtraL_DbmOpen(Tcl_Interp *interp,
	char *typestring,Tcl_Obj *database,int rwcode);
