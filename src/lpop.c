/*	
 *	 File:	extral.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "tclInt.h"
#include "tcl.h"
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LpopCmd --
 *
 *		This procedure is invoked to process the "lpop" command.
 *		It pops an item out of a list
 *
 * Results:
 *		A standard Tcl result: the popped out elemeny.
 *
 *
 *----------------------------------------------------------------------
 */
int
ExtraL_LpopObjCmd(dummy, interp, objc, objv)
	ClientData dummy;		/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj **objv;		/* Argument objects. */
{
	Interp *iPtr = (Interp *) interp;
	Tcl_Obj *resultPtr = iPtr->objResult;
	register Tcl_Obj *listObjPtr;
	Tcl_Obj *popPtr;
	char *firstStr;
	int listLen;
	long first;
	int result;

	if ((objc != 2)&&(objc != 3)) {
		Tcl_StringObjAppend(resultPtr, "wrong # args: should be \"", -1);
		Tcl_StringObjAppendObj(resultPtr, objv[0]);
		Tcl_StringObjAppend(resultPtr,
	 	 	   " listName pos\"", -1);
	return TCL_ERROR;
	}

	listObjPtr = Tcl_ObjGetVar2(interp, objv[1], (Tcl_Obj *) NULL,
		(TCL_LEAVE_ERR_MSG | TCL_PART1_NOT_PARSED));
	if (listObjPtr == NULL) {
		return TCL_ERROR;
	}
	

	/*
	 * THIS FAILS IF THE OBJECT'S STRING REP CONTAINS NULLS.
	 */
	
	result=Tcl_ListObjLength(interp,listObjPtr,&listLen);
	if (result==TCL_ERROR) {
		return TCL_ERROR;
	}
	if (listLen==0) {
		return TCL_OK;
	}
	if (objc==2) {
		first = (listLen - 1);
	} else {
		firstStr = Tcl_GetStringFromObj(objv[2], (int *)NULL);
		if ((*firstStr == 'e') && (strcmp(firstStr, "end") == 0)) {
			first = (listLen - 1);
		} else {
			result=Tcl_GetIntFromObj(interp,objv[2],&first);
			if (result==TCL_ERROR) {
				return TCL_ERROR;
			}
		}
		if (first < 0)  {
			first = 0;
		}
		if (first >= listLen) {
			Tcl_ResetObjResult(interp);
			resultPtr = iPtr->objResult;
			Tcl_StringObjAppend(resultPtr, "list doesn't contain element ", -1);
			Tcl_StringObjAppendObj(resultPtr, objv[2]);
			return TCL_ERROR;
		}
	}

	result = Tcl_ListObjIndex(interp,listObjPtr,first,&popPtr);
	if (result != TCL_OK) {
		return result;
	}
	Tcl_SetObjResult(interp,popPtr);
	result = Tcl_ListObjReplace(interp, listObjPtr, first, 1, 0, NULL);
	if (result != TCL_OK) {
		return result;
	}

	/*
	 * Set the interpreter's object result. 
	 */

	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LshiftCmd --
 *
 *		This procedure is invoked to process the "lshift" command.
 *		It pops the first item out of a list
 *
 * Results:
 *		A standard Tcl result: the popped out elemeny.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LshiftObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj **objv;		/* Argument objects. */
{
	Interp *iPtr = (Interp *) interp;
	Tcl_Obj *resultPtr = iPtr->objResult;
	register Tcl_Obj *listObjPtr;
	Tcl_Obj *popPtr;
	int result,len;

	if (objc != 2) {
		Tcl_StringObjAppend(resultPtr, "wrong # args: should be \"", -1);
		Tcl_StringObjAppendObj(resultPtr, objv[0]);
		Tcl_StringObjAppend(resultPtr,
	 	 	   " listName\"", -1);
	return TCL_ERROR;
	}

	listObjPtr = Tcl_ObjGetVar2(interp, objv[1], (Tcl_Obj *) NULL,
		(TCL_LEAVE_ERR_MSG | TCL_PART1_NOT_PARSED));
	if (listObjPtr == NULL) {
		return TCL_ERROR;
	}
	
	result=Tcl_ListObjLength(interp,listObjPtr,&len);
	if (result==TCL_ERROR) {
		return TCL_ERROR;
	}
	if (len==0) {
		return TCL_OK;
	}
	result = Tcl_ListObjIndex(interp,listObjPtr,0,&popPtr);
	if (result != TCL_OK) {
		return result;
	}
	Tcl_SetObjResult(interp,popPtr);
	result = Tcl_ListObjReplace(interp, listObjPtr, 0, 1, 0, NULL);
	if (result != TCL_OK) {
		return result;
	}

	/*
	 * Set the interpreter's object result. 
	 */

	return TCL_OK;
}
