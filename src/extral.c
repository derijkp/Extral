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
 * ExtraL_LfindCmd --
 *
 *		This procedure is invoked to process the "lfind" command.
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
ExtraL_LfindObjCmd(clientData, interp, objc, objv)
	ClientData clientData;	/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument values. */
{
#define EXACT	0
#define GLOB	1
#define REGEXP	2
	char *bytes, *patternBytes;
	int i, match, mode, result, listLen, length, elemLen;
	Tcl_Obj **elemPtrs;
	Tcl_Obj *indexObj, *resultObj;
	static char *switches[] =
		{"-exact", "-glob", "-regexp", (char *) NULL};

	mode = GLOB;
	if (objc == 4) {
		if (Tcl_GetIndexFromObj(interp, objv[1], switches,
			"search mode", 0, &mode) != TCL_OK) {
			return TCL_ERROR;
		}
	} else if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "?mode? list pattern");
		return TCL_ERROR;
	}

	/*
	 * Make sure the list argument is a list object and get its length and
	 * a pointer to its array of element pointers.
	 */

	result = Tcl_ListObjGetElements(interp, objv[objc-2], &listLen, &elemPtrs);
	if (result != TCL_OK) {
		return result;
	}

	patternBytes = Tcl_GetStringFromObj(objv[objc-1], &length);
 
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);
	for (i = 0; i < listLen; i++) {
		match = 0;
		bytes = Tcl_GetStringFromObj(elemPtrs[i], &elemLen);
		switch (mode) {
			case EXACT:
				if (length == elemLen) {
					match = (memcmp(bytes, patternBytes,
					(size_t) length) == 0);
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
			indexObj=Tcl_NewIntObj(i);
			result=Tcl_ListObjAppendElement(interp,resultObj,indexObj);
			if (result!=TCL_OK) {return result;}
		}
	}

	Tcl_SetObjResult(interp,resultObj);
	return TCL_OK;
}
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LsubCmd --
 *
 *		This procedure is invoked to process the "lsub" command.
 *		It creates a subset of a list
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LsubObjCmd(dummy, interp, objc, objv)
	ClientData dummy;		/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *listPtr;
	Tcl_Obj **elemPtrs;
	Tcl_Obj *indexlistPtr;
	Tcl_Obj **indexelemPtrs;
	Tcl_Obj *resultObj, *indexObj;
	int listLen, indexlistLen, index, result;
	int i;
	char *mode=NULL;

	if (objc == 4) {
		mode=Tcl_GetStringFromObj(objv[2],(int *)NULL);
		if (strcmp(mode,"-exclude")!=0) {
		    Tcl_AppendResult(interp, "wrong arg: \"", mode, "\"", (char *) NULL);
		    return TCL_ERROR;
		}
	} else if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "list ?-exclude? indices");
		return TCL_ERROR;
	}

	/*
	 * Convert the first argument to a list if necessary.
	 */

	listPtr = objv[1];
	result = Tcl_ListObjGetElements(interp, listPtr, &listLen, &elemPtrs);
	if (result != TCL_OK) {
		return result;
	}

	/*
	 * Convert the indices to a list if necessary.
	 */

	indexlistPtr = objv[objc-1];
	result = Tcl_ListObjGetElements(interp, indexlistPtr, &indexlistLen, &indexelemPtrs);
	if (result != TCL_OK) {
		return result;
	}

	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);

	if (mode==NULL) {
		if (indexlistLen==0) {
			Tcl_SetResult(interp, "",TCL_STATIC);
			return TCL_OK;
		}
		for(i=0;i<indexlistLen;i++) {
			result=Tcl_GetIntFromObj(interp,indexelemPtrs[i],&index);
			if (result!=TCL_OK) {return result;}
			result=Tcl_ListObjIndex(interp,listPtr,index,&indexObj);
			if (result!=TCL_OK) {return result;}
			if (indexObj!=NULL)	{
				result=Tcl_ListObjAppendElement(interp,resultObj,indexObj);
			}
			if (result!=TCL_OK) {return result;}
		}
	} else {
		int curindex=0;
		if (indexlistLen==0) {
			Tcl_SetObjResult(interp, listPtr);
			return TCL_OK;
		}
		index=-1;
		while ((index<0)&&(curindex<indexlistLen)) {
			result=Tcl_GetIntFromObj(interp,indexelemPtrs[curindex++],&index);
			if (result!=TCL_OK) {return result;}
		}
		for(i=0;i<listLen;i++) {
			if (i==index)	{
				index=-1;
				while ((index<=1)&&(curindex<indexlistLen)) {
					result=Tcl_GetIntFromObj(interp,indexelemPtrs[curindex++],&index);
					if (result!=TCL_OK) {return result;}
				}
			} else {
				result=Tcl_ListObjAppendElement(interp,resultObj,elemPtrs[i]);
				if (result!=TCL_OK) {return result;}
			}
		}
	}

	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LcorCmd --
 *
 *		This procedure is invoked to process the "lcor" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LcorObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
#define INCLUDE		0
#define EXCLUDE		1
	int refArgc;
	Tcl_Obj **refArgv;
	int listArgc;
	Tcl_Obj **listArgv;
	Tcl_Obj *indexObj, *resultObj;
	int *done;
	char *refstring,*string;
	int reflen,len;
	int pos,result;
	int i;

	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "referencelist list");
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[1], &refArgc, &refArgv) != TCL_OK) {
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[2], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}

	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);

	done=(int *)Tcl_Alloc(refArgc*sizeof(int));
	for(i=0;i<refArgc;i++) {done[i]=0;}
	for(pos=0;pos<listArgc;pos++) {
		string=Tcl_GetStringFromObj(listArgv[pos],&len);
		for(i=0;i<refArgc;i++) {
			refstring=Tcl_GetStringFromObj(refArgv[i],&reflen);
			if ((done[i]==0)&&(len==reflen)&&(strcmp(refstring,string)==0)) {
				indexObj=Tcl_NewIntObj(i);
				result=Tcl_ListObjAppendElement(interp,resultObj,indexObj);
				if (result!=TCL_OK) {return result;}
				done[i]=1;
				break;
			}
		}
		if (i==refArgc) Tcl_AppendElement(interp,"-1");
	}

	Tcl_Free((char *)done);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LremdupCmd --
 *
 *		This procedure is invoked to process the "lremdup" command.
 *		It creates a subset of a list
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LremdupObjCmd(dummy, interp, objc, objv)
	ClientData dummy;		/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *listPtr;
	Tcl_Obj **elemPtrs;
	Tcl_Obj *resultObj, *indexObj;
	int listLen, indexlistLen, result;
	char *string,*checkstring;
	int i,j,len,checklen;

	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "list");
		return TCL_ERROR;
	}

	/*
	 * Convert the first argument to a list if necessary.
	 */

	listPtr = objv[1];
	result = Tcl_ListObjGetElements(interp, listPtr, &listLen, &elemPtrs);
	if (result != TCL_OK) {
		return result;
	}

	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);

	if (indexlistLen==0) {
		Tcl_SetObjResult(interp, listPtr);
		return TCL_OK;
	}
	for(i=0;i<listLen;i++) {
		string = Tcl_GetStringFromObj(elemPtrs[i], &len);
		for(j=0;j<i;j++) {
			checkstring = Tcl_GetStringFromObj(elemPtrs[j], &checklen);
			if ((len==checklen)&&(strcmp(string,checkstring)==0))	{
				break;
			}
		}
		if (i==j)	{
			result=Tcl_ListObjAppendElement(interp,resultObj,elemPtrs[i]);
			if (result!=TCL_OK) {return result;}
		}
	}

	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LremoveCmd --
 *
 *		This procedure is invoked to process the "lremove" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LremoveObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
#define INCLUDE		0
#define EXCLUDE		1
	int refArgc;
	Tcl_Obj **refArgv;
	int listArgc;
	Tcl_Obj **listArgv;
	Tcl_Obj *indexObj, *resultObj;
	char *refstring,*string;
	int reflen,len;
	int pos,result;
	int i;

	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "list removelist");
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[1], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[2], &refArgc, &refArgv) != TCL_OK) {
		return TCL_ERROR;
	}

	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);

	for(pos=0;pos<listArgc;pos++) {
		string=Tcl_GetStringFromObj(listArgv[pos],&len);
		if (refArgc==0) {
			if (len!=0) {
				result=Tcl_ListObjAppendElement(interp,resultObj,listArgv[pos]);
			}
		} else {
			for(i=0;i<refArgc;i++) {
				refstring=Tcl_GetStringFromObj(refArgv[i],&reflen);
				if ((len==reflen)&&(strcmp(refstring,string)==0)) {
					break;
				}
			}
			if (i==refArgc) {
				result=Tcl_ListObjAppendElement(interp,resultObj,listArgv[pos]);
				if (result!=TCL_OK) {return result;}
			}
		}
	}
	return TCL_OK;
}
