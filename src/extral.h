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
	Tcl_Obj *structure,Tcl_Obj *data,Tcl_Obj *oldvalue,int tagsc,Tcl_Obj **tagsv,
	Tcl_Obj **value));
typedef int ExtraL_StructlTypeGetProc _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *structure,Tcl_Obj *data,int tagsc,Tcl_Obj **tagsv,
	Tcl_Obj **value));
typedef int ExtraL_StructlTypeUnsetProc _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *structure,Tcl_Obj *data,Tcl_Obj *oldvalue,int tagsc,Tcl_Obj **tagsv,
	Tcl_Obj **value));
EXTERN int ExtraL_StructlsetStruct _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *structure,
	Tcl_Obj *data,
	Tcl_Obj *list,
	int tagsc,
	Tcl_Obj **tagsv,
	Tcl_Obj *value,
	Tcl_Obj **resultPtr));

EXTERN int ExtraL_StructlunsetStruct _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *structure,
	Tcl_Obj *data,
	Tcl_Obj *list,
	int tagsc,
	Tcl_Obj **tagsv,
	Tcl_Obj **resultPtr));

EXTERN int ExtraL_StructlgetStruct _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *structure,
	Tcl_Obj *data,
	Tcl_Obj *list,
	int tagsc,
	Tcl_Obj **tagsv,
	Tcl_Obj **resultPtr));


EXTERN int ExtraL_StructlCreateType _ANSI_ARGS_((Tcl_Interp *interp,
	char *key,
	ExtraL_StructlTypeSetProc *setproc,
	ExtraL_StructlTypeGetProc *getproc,
	ExtraL_StructlTypeUnsetProc *unsetproc));

EXTERN int ExtraL_StructlFindTag _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *list,
	char *tag,
	int taglen,
	Tcl_Obj **resultPtr,
	int *posPtr));

int ExtraL_ObjEqual _ANSI_ARGS_((Tcl_Obj *obj1,Tcl_Obj *obj2));
 