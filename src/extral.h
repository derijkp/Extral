/*	
 *	 File:    tagl.h
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

typedef int ExtraL_TaglTypeProc _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *structure, Tcl_Obj **value));

int ExtraL_TaglCreateType _ANSI_ARGS_((Tcl_Interp *interp,
	char *key,ExtraL_TaglTypeProc *setproc,ExtraL_TaglTypeProc *getproc));
