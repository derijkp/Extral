/*	
 *	 File:    extral.c
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
#include "tclRegexp.h"
#include "general.h"
#define EXACT		0
#define GLOB		1
#define REGEXP		2
 
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_StringChangeObjCmd --
 *
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */
int
ExtraL_StringChangeObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *result;
	int listObjc;
	Tcl_Obj **listObjv;
	char *p,*lp;
	int plen,lplen,ppos,tppos,lppos,prev;
	int j;

	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "string changelist");
		return TCL_ERROR;
	}
	if (Tcl_ListObjGetElements(interp, objv[2], &listObjc, &listObjv) != TCL_OK) {
		return TCL_ERROR;
	}
	result = Tcl_NewObj();
	p = Tcl_GetStringFromObj(objv[1],&plen);
	ppos = 0;
	prev = 0;
	while (ppos < plen) {
		for(j=0;j<listObjc;j+=2) {
			lp = Tcl_GetStringFromObj(listObjv[j],&lplen);
			tppos = ppos;
			lppos = 0;
			while ((lppos < lplen)&&(tppos < plen)) {
				if (p[tppos] != lp[lppos]) break;
				tppos++;lppos++;
			}
			if (lppos == lplen) {
				if (prev != ppos) {
					Tcl_AppendToObj(result,p + prev,ppos - prev);
				}
				lp = Tcl_GetStringFromObj(listObjv[j+1],&lplen);
				Tcl_AppendToObj(result,lp,lplen);
				ppos = tppos-1;
				prev = tppos;
				break;
			}
		}
		ppos++;
	}
	if (ppos == plen) {
		Tcl_AppendToObj(result,p + prev,plen - prev);
	}
	Tcl_SetObjResult(interp,result);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_StringReplaceObjCmd --
 *
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */
int
ExtraL_StringReplaceObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *result;
	char *replacement,*string;
	int i,error;
	int first,last,slen,rlen;
	/* */
	if (objc != 5) {
		Tcl_WrongNumArgs(interp, 1, objv, "string first last replacement");
		return TCL_ERROR;
	}
	error = Tcl_GetIntFromObj(interp, objv[2], &first);
	if (error) {return error;}
	error = Tcl_GetIntFromObj(interp, objv[3], &last);
	if (error) {return error;}
	last++;
	replacement = Tcl_GetStringFromObj(objv[4],&rlen);
	string = Tcl_GetStringFromObj(objv[1],&slen);
	result = Tcl_NewStringObj("",0);
	if (first < slen) {
		if (first+rlen < slen) {
			Tcl_AppendToObj(result, string, first);
			Tcl_AppendToObj(result, replacement, rlen);
			Tcl_AppendToObj(result, string+last, slen-last);
		} else {
			Tcl_AppendToObj(result, string, first);
			Tcl_AppendToObj(result, replacement, rlen);
		}
	} else {
		Tcl_AppendToObj(result, string, slen);
		for (i = slen + 1 ; i < first ; i++) {Tcl_AppendToObj(result, " ", 1);}
		Tcl_AppendToObj(result, replacement, rlen);
	}
	Tcl_SetObjResult(interp,result);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_SreverseObjCmd --
 *
 *		This procedure is invoked to process the "sreverse" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_SreverseObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	char *string, *result,*pos;
	int stringlen,i;
	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "string");
		return TCL_ERROR;
	}
	string = Tcl_GetStringFromObj(objv[1], &stringlen);
	result = (char *)Tcl_Alloc(stringlen*sizeof(char));
	pos = result;
	i = stringlen;
	for(i--;i >= 0;i--) {
		*pos++ = string[i];
	}
	Tcl_SetObjResult(interp,Tcl_NewStringObj(result,stringlen));
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_SfindCmd --
 *
 *		This procedure is invoked to process the "sfind" command.
 *		It finds all occurences of a pattern in a list, and returns
 *		their indexes as a list.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_SFindObjCmd(clientData, interp, objc, objv)
	ClientData clientData;	/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument values. */
{
	char *string;
	char *bytes, *patternBytes;
	int i, match, mode, result, length, stringlen;
	Tcl_Obj *indexObj, *resultObj;
	static char *switches[] =
		{"-exact", "-glob", "-regexp", (char *) NULL};
	mode = EXACT;
	if (objc == 4) {
		if (Tcl_GetIndexFromObj(interp, objv[1], switches,
			"search mode", 0, &mode) != TCL_OK) {
			return TCL_ERROR;
		}
	} else if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "?mode? string pattern");
		return TCL_ERROR;
	}
	string = Tcl_GetStringFromObj(objv[objc-2], &stringlen);
	patternBytes = Tcl_GetStringFromObj(objv[objc-1], &length);
 	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);
	for (i = 0; i < stringlen; i++) {
		match = 0;
		bytes = string+i;
		switch (mode) {
			case EXACT:
				if (i+length < stringlen) {
					match = (memcmp(bytes, patternBytes,(size_t) length) == 0);
				}
				break;
			case GLOB:
				/*
				 * WARNING: will not work with data containing NULLs.
				 */
				match = Tcl_StringMatch(bytes, patternBytes);
				break;
			case REGEXP:
				/*
				 * WARNING: will not work with data containing NULLs.
				 */
				match = Tcl_RegExpMatch(interp, bytes, patternBytes);
				if (match < 0) {
					return TCL_ERROR;
				}
				break;
		}
		if (match) {
			indexObj = Tcl_NewIntObj(i);
			result = Tcl_ListObjAppendElement(interp,resultObj,indexObj);
			if (result != TCL_OK) {return result;}
		}
	}
	Tcl_SetObjResult(interp,resultObj);
	return TCL_OK;
}
