/*	
 *	 File:    tagl.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tcl.h"

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_TaglsetCmd --
 *
 *		This procedure is invoked to process the "taglset" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_TaglsetObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listArgc;
	Tcl_Obj **listArgv;
	Tcl_Obj *resultObj;
	char *ctag,*tag;
	int clen,len;
	int pos,result;
	int i;

	if (objc != 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "list tag value");
		return TCL_ERROR;
	}

	if (Tcl_ListObjLength(interp,objv[1],&len) != TCL_OK) {
		return TCL_ERROR;
	}
	if (len & 1) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"tagged list must have an even number of elements",(char *)NULL);
		return TCL_ERROR;
	}

	tag=Tcl_GetStringFromObj(objv[2],&len);

	/* Initialise result */
	resultObj=Tcl_DuplicateObj(objv[1]);
	if (Tcl_ListObjGetElements(interp, resultObj, &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}

	for(pos=0;pos<listArgc;pos+=2) {
		ctag=Tcl_GetStringFromObj(listArgv[pos],&clen);
		if (clen==len) {
			if (strncmp(ctag,tag,len)==0) {
				result = Tcl_ListObjReplace(interp,resultObj,++pos,1,1,objv+3);
				if (result != TCL_OK) {
					Tcl_DecrRefCount(resultObj);
					return result;
				}
				Tcl_SetObjResult(interp,resultObj);
				return TCL_OK;
			}
		}
	}
	result=Tcl_ListObjReplace(interp,resultObj,pos,0,2,objv+2);
	if (result != TCL_OK) {
		Tcl_DecrRefCount(resultObj);
		return result;
	}
	Tcl_SetObjResult(interp,resultObj);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_TaglunsetCmd --
 *
 *		This procedure is invoked to process the "taglunset" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_TaglunsetObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listArgc;
	Tcl_Obj **listArgv;
	Tcl_Obj *resultObj;
	char *ctag,*tag;
	int clen,len;
	int pos,result;
	int i;

	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "list tag");
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[1], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}
	if (listArgc & 1) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"tagged list must have an even number of elements",(char *)NULL);
		return TCL_ERROR;
	}

	tag=Tcl_GetStringFromObj(objv[2],&len);

	/* Initialise result */

	for(pos=0;pos<listArgc;pos+=2) {
		ctag=Tcl_GetStringFromObj(listArgv[pos],&clen);
		if (clen==len) {
			if (strncmp(ctag,tag,len)==0) {
				resultObj=Tcl_DuplicateObj(objv[1]);
				result = Tcl_ListObjReplace(interp,resultObj,pos,2,0,NULL);
				if (result != TCL_OK) {
					Tcl_DecrRefCount(resultObj);
					return result;
				}
				Tcl_SetObjResult(interp,resultObj);
				return TCL_OK;
			}
		}
	}
	Tcl_SetObjResult(interp,objv[1]);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_TaglgetCmd --
 *
 *		This procedure is invoked to process the "taglget" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_TaglgetObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listArgc;
	Tcl_Obj **listArgv;
	char *ctag,*tag;
	int clen,len;
	int pos,result;
	int i;

	if ((objc != 3)&&(objc != 4)) {
		Tcl_WrongNumArgs(interp, 1, objv, "list tag ?default?");
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[1], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}
	if (listArgc & 1) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"tagged list must have an even number of elements",(char *)NULL);
		return TCL_ERROR;
	}

	tag=Tcl_GetStringFromObj(objv[2],&len);

	/* Initialise result */

	for(pos=0;pos<listArgc;pos+=2) {
		ctag=Tcl_GetStringFromObj(listArgv[pos],&clen);
		if (clen==len) {
			if (strncmp(ctag,tag,len)==0) {
				Tcl_SetObjResult(interp,listArgv[++pos]);
				return TCL_OK;
			}
		}
	}
	if (objc==4) {
		Tcl_SetObjResult(interp,objv[3]);
		return TCL_OK;
	} else {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"tag \"",tag,"\" not found",(char *) NULL);
		return TCL_ERROR;
	}
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_TaglfieldsCmd --
 *
 *		This procedure is invoked to process the "taglfields" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_TaglfieldsObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listArgc;
	Tcl_Obj **listArgv;
	Tcl_Obj *resultObj;
	Tcl_Obj *valueObj;
	char *tag;
	int pos,result;
	int i;

	if ((objc != 2)&&(objc != 3)) {
		Tcl_WrongNumArgs(interp, 1, objv, "list ?valueVar?");
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[1], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}
	if (listArgc & 1) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"tagged list must have an even number of elements",(char *)NULL);
		return TCL_ERROR;
	}


	if (objc==3) {
		valueObj = Tcl_NewObj();
	}

	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);

	for(pos=0;pos<listArgc;pos+=2) {
		result=Tcl_ListObjAppendElement(interp,resultObj,listArgv[pos]);
		if (result!=TCL_OK) {return result;}
	}
	if (objc==3) {
		for(pos=1;pos<listArgc;pos+=2) {
			result=Tcl_ListObjAppendElement(interp,valueObj,listArgv[pos]);
			if (result!=TCL_OK) {return result;}
		}
		if (Tcl_ObjSetVar2(interp, objv[2], (Tcl_Obj *) NULL,
			valueObj, (TCL_LEAVE_ERR_MSG | TCL_PARSE_PART1)) == NULL) {
				return TCL_ERROR;
		}
	}
	return TCL_OK;
}

