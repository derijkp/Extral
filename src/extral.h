/*	
 *	 File:    extral.h
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

/*
 * format- and scantime functions and definitions
 */

EXTERN int ExtraL_ScanTime _ANSI_ARGS_((Tcl_Interp *interp,
	int musthavedate,
	int musthavetime,
	Tcl_Obj *dateObj,
	double *resultPtr));

EXTERN int ExtraL_FormatTime _ANSI_ARGS_((Tcl_Interp *interp,
	double time,
	char *format,
	char **result));

/*
 * structl functions and definitions
 */

typedef int ExtraL_StructlTypeSetProc _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *structure,Tcl_Obj *oldvalue,int tagsc,Tcl_Obj **tagsv,
	Tcl_Obj **value));
typedef int ExtraL_StructlTypeGetProc _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *structure,int tagsc,Tcl_Obj **tagsv,
	Tcl_Obj **value));
EXTERN int ExtraL_StructlsetStruct _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *structure,
	Tcl_Obj *list,
	int tagsc,
	Tcl_Obj **tagsv,
	Tcl_Obj *value,
	Tcl_Obj **resultPtr));

EXTERN int ExtraL_StructlCreateType _ANSI_ARGS_((Tcl_Interp *interp,
	char *key,ExtraL_StructlTypeSetProc *setproc,ExtraL_StructlTypeGetProc *getproc));

EXTERN int ExtraL_StructlgetStruct _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *structure,
	Tcl_Obj *list,
	int tagsc,
	Tcl_Obj **tagsv,
	Tcl_Obj **resultPtr));

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
