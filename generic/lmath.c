/*	
 *	 File:    lmath.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "tcl.h"
#include "general.h"
#define UCHAR(c) ((unsigned char) (c))
 
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_Lmath_averageObjCmd --
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_Lmath_averageObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listobjc;
	Tcl_Obj **listobjv;
	double el,result;
	int i,error;
	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "list");
		return TCL_ERROR;
	}
	if (Tcl_ListObjGetElements(interp, objv[1], &listobjc, &listobjv) != TCL_OK) {
		return TCL_ERROR;
	}
	result = 0.0;
	for (i = 0 ; i<listobjc ; i++) {
		error  = Tcl_GetDoubleFromObj(interp,listobjv[i],&el);
		if (error) {return error;}
		result += el;
	}
	Tcl_SetObjResult(interp,Tcl_NewDoubleObj(result/listobjc));
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_Lmath_maxObjCmd --
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_Lmath_maxObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listobjc;
	Tcl_Obj **listobjv;
	double el,result;
	int i,error,resultpos;
	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "list");
		return TCL_ERROR;
	}
	if (Tcl_ListObjGetElements(interp, objv[1], &listobjc, &listobjv) != TCL_OK) {
		return TCL_ERROR;
	}
	if (listobjc == 0) {return TCL_OK;}
	error  = Tcl_GetDoubleFromObj(interp,listobjv[0],&el);
	if (error) {return error;}
	result = el;
	resultpos = 0;
	for (i = 1 ; i<listobjc ; i++) {
		error  = Tcl_GetDoubleFromObj(interp,listobjv[i],&el);
		if (error) {return error;}
		if (el > result) {
			result = el;
			resultpos = i;
		}
	}
	Tcl_SetObjResult(interp,listobjv[resultpos]);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_Lmath_minObjCmd --
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_Lmath_minObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listobjc;
	Tcl_Obj **listobjv;
	double el,result;
	int i,error,resultpos;
	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "list");
		return TCL_ERROR;
	}
	if (Tcl_ListObjGetElements(interp, objv[1], &listobjc, &listobjv) != TCL_OK) {
		return TCL_ERROR;
	}
	if (listobjc == 0) {return TCL_OK;}
	error  = Tcl_GetDoubleFromObj(interp,listobjv[0],&el);
	if (error) {return error;}
	result = el;
	resultpos = 0;
	for (i = 1 ; i < listobjc ; i++) {
		error  = Tcl_GetDoubleFromObj(interp,listobjv[i],&el);
		if (error) {return error;}
		if (el < result) {
			result = el;
			resultpos = i;
		}
	}
	Tcl_SetObjResult(interp,listobjv[resultpos]);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_Lmath_sumObjCmd --
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_Lmath_sumObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listobjc;
	Tcl_Obj **listobjv;
	double el,result;
	int i,error;
	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "list");
		return TCL_ERROR;
	}
	if (Tcl_ListObjGetElements(interp, objv[1], &listobjc, &listobjv) != TCL_OK) {
		return TCL_ERROR;
	}
	result = 0.0;
	for (i = 0 ; i<listobjc ; i++) {
		error  = Tcl_GetDoubleFromObj(interp,listobjv[i],&el);
		if (error) {return error;}
		result += el;
	}
	Tcl_SetObjResult(interp,Tcl_NewDoubleObj(result));
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_Lmath_calcObjCmd --
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_getnum(interp, obj, typePtr, iPtr, dPtr)
	Tcl_Interp *interp;
	Tcl_Obj *CONST obj;
	char *typePtr;
	int *iPtr;
	double *dPtr;
{
	int error;
	error = Tcl_GetIntFromObj(interp,obj,iPtr);
	if (error) {
		error = Tcl_GetDoubleFromObj(interp,obj,dPtr);
		if (error) {return error;}
		*typePtr = 'd';
	} else {
		*typePtr = 'i';
	}
	return TCL_OK;
}

Tcl_Obj *ExtraL_docalc(calc,t1,el1i,el1d,t2,el2i,el2d)
	char calc;
	char t1;
	char t2;
	int el1i;
	int el2i;
	double el1d;
	double el2d;
{
	Tcl_Obj *resultEl;
	if ((t1 == 'i') && (t2 == 'i')) {
		if (calc == '+') {
			resultEl = Tcl_NewIntObj(el1i + el2i);
		} else if (calc == '-') {
			resultEl = Tcl_NewIntObj(el1i - el2i);
		} else if (calc == '*') {
			resultEl = Tcl_NewIntObj(el1i * el2i);
		} else if (calc == '/') {
			resultEl = Tcl_NewIntObj(el1i / el2i);
		}
	} else if ((t1 == 'i') && (t2 == 'd')) {
		if (calc == '+') {
			resultEl = Tcl_NewDoubleObj(el1i + el2d);
		} else if (calc == '-') {
			resultEl = Tcl_NewDoubleObj(el1i - el2d);
		} else if (calc == '*') {
			resultEl = Tcl_NewDoubleObj(el1i * el2d);
		} else if (calc == '/') {
			resultEl = Tcl_NewDoubleObj(el1i / el2d);
		}
	} else if ((t1 == 'd') && (t2 == 'i')) {
		if (calc == '+') {
			resultEl = Tcl_NewDoubleObj(el1d + el2i);
		} else if (calc == '-') {
			resultEl = Tcl_NewDoubleObj(el1d - el2i);
		} else if (calc == '*') {
			resultEl = Tcl_NewDoubleObj(el1d * el2i);
		} else if (calc == '/') {
			resultEl = Tcl_NewDoubleObj(el1d / el2i);
		}
	} else {
		if (calc == '+') {
			resultEl = Tcl_NewDoubleObj(el1d + el2d);
		} else if (calc == '-') {
			resultEl = Tcl_NewDoubleObj(el1d - el2d);
		} else if (calc == '*') {
			resultEl = Tcl_NewDoubleObj(el1d * el2d);
		} else if (calc == '/') {
			resultEl = Tcl_NewDoubleObj(el1d / el2d);
		}
	}
	return resultEl;
}

int
ExtraL_Lmath_calcObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listobjc1,listobjc2;
	Tcl_Obj **listobjv1,**listobjv2,*result,*resultEl;
	char *string,t1,t2;
	int el1i,el2i;
	double el1d,el2d;
	int i,error,len;
	if (objc != 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "list1 action list2");
		return TCL_ERROR;
	}
	string = Tcl_GetStringFromObj(objv[2],&len);
	if ((len != 1) || (string[0] != '+' && string[0] != '-' && string[0] != '*' && string[0] != '/')) {
		Tcl_AppendResult(interp,"action must be one of +,-,*,/");
		return TCL_ERROR;
	}
	if (Tcl_ListObjGetElements(interp, objv[1], &listobjc1, &listobjv1) != TCL_OK) {
		return TCL_ERROR;
	}
	if (Tcl_ListObjGetElements(interp, objv[3], &listobjc2, &listobjv2) != TCL_OK) {
		return TCL_ERROR;
	}
	if ((listobjc1 != 1) && (listobjc2 != 1)) {
		if (listobjc1 < listobjc2) {
			listobjc2 = listobjc1;
		} else if (listobjc1 > listobjc2) {
			listobjc1 = listobjc2;
		}
	}
	result = Tcl_NewListObj(0,NULL);
	if (listobjc1 == 1) {
		error = ExtraL_getnum(interp,listobjv1[0],&t1,&el1i,&el1d);
		if (error) {Tcl_DecrRefCount(result);return error;}
		for (i = 0 ; i < listobjc2 ; i++) {
			error = ExtraL_getnum(interp,listobjv2[i],&t2,&el2i,&el2d);
			if (error) {Tcl_DecrRefCount(result);return error;}
			resultEl = ExtraL_docalc(string[0],t1,el1i,el1d,t2,el2i,el2d);
			error = Tcl_ListObjAppendElement(interp,result,resultEl);
			if (error) {Tcl_DecrRefCount(resultEl);Tcl_DecrRefCount(result);return error;}
		}
	} else if (listobjc2 == 1) {
		error = ExtraL_getnum(interp,listobjv2[0],&t2,&el2i,&el2d);
		if (error) {Tcl_DecrRefCount(result);return error;}
		for (i = 0 ; i < listobjc1 ; i++) {
			error = ExtraL_getnum(interp,listobjv1[i],&t1,&el1i,&el1d);
			if (error) {Tcl_DecrRefCount(result);return error;}
			resultEl = ExtraL_docalc(string[0],t1,el1i,el1d,t2,el2i,el2d);
			error = Tcl_ListObjAppendElement(interp,result,resultEl);
			if (error) {Tcl_DecrRefCount(resultEl);Tcl_DecrRefCount(result);return error;}
		}
	} else {
		for (i = 0 ; i < listobjc1 ; i++) {
			error = ExtraL_getnum(interp,listobjv1[i],&t1,&el1i,&el1d);
			if (error) {Tcl_DecrRefCount(result);return error;}
			error = ExtraL_getnum(interp,listobjv2[i],&t2,&el2i,&el2d);
			if (error) {Tcl_DecrRefCount(result);return error;}
			resultEl = ExtraL_docalc(string[0],t1,el1i,el1d,t2,el2i,el2d);
			error = Tcl_ListObjAppendElement(interp,result,resultEl);
			if (error) {Tcl_DecrRefCount(resultEl);Tcl_DecrRefCount(result);return error;}
		}
	}
	Tcl_SetObjResult(interp,result);
	return TCL_OK;
}

