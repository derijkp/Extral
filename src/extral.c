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
				while ((index < 1)&&(curindex < indexlistLen)) {
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
	resultObj = Tcl_NewObj();

	done=(int *)Tcl_Alloc(refArgc*sizeof(int));
	for(i=0;i<refArgc;i++) {done[i]=0;}
	for(pos=0;pos<listArgc;pos++) {
		string=Tcl_GetStringFromObj(listArgv[pos],&len);
		for(i=0;i<refArgc;i++) {
			if (done[i] == 1) continue;
			refstring = Tcl_GetStringFromObj(refArgv[i],&reflen);
			if ((len == reflen)&&(strcmp(refstring,string) == 0)) {
				indexObj = Tcl_NewIntObj(i);
				result = Tcl_ListObjAppendElement(interp,resultObj,indexObj);
				if (result != TCL_OK) {Tcl_DecrRefCount(resultObj);return result;}
				done[i] = 1;
				break;
			}
		}
		if (i == refArgc) {
			result = Tcl_ListObjAppendElement(interp,resultObj,Tcl_NewIntObj(-1));
			if (result != TCL_OK) {Tcl_DecrRefCount(resultObj);return result;}
		}
	}
	Tcl_SetObjResult(interp,resultObj);

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
	Tcl_Obj *resultObj;
	int listLen, result;
	char *string,*checkstring;
	int i,len,checklen,sort,var=0;

	sort = 0;
	if (objc>2) {
		string = Tcl_GetStringFromObj(objv[1],NULL);
		if (strcmp(string,"-sorted") == 0) {
			sort = 1;
		}
	}
	if ((objc != (sort+2))&&(objc != (sort+3))) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-sorted? list ?var?");
		return TCL_ERROR;
	}
	if (objc==(sort+3)) {var = 1;}

	/*
	 * Convert the first argument to a list if necessary.
	 */

	listPtr = objv[sort+1];
	result = Tcl_ListObjGetElements(interp, listPtr, &listLen, &elemPtrs);
	if (result != TCL_OK) {
		return result;
	}

	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);

	if ((listLen==0)||(listLen==1)) {
		Tcl_SetObjResult(interp, listPtr);
		return TCL_OK;
	}
	if (var == 1) {
		if (Tcl_ObjSetVar2(interp,objv[sort+2],NULL,Tcl_NewObj(),
			TCL_LEAVE_ERR_MSG|TCL_PARSE_PART1) == NULL) {return TCL_ERROR;}
	}
	if (sort == 0) {
		Tcl_HashTable table;
		int new;
		Tcl_InitHashTable(&table,TCL_STRING_KEYS);
		for(i=0;i<listLen;i++) {
			string = Tcl_GetStringFromObj(elemPtrs[i], &len);
			Tcl_CreateHashEntry(&table,string,&new);
			if (new != 0) {
				result = Tcl_ListObjAppendElement(interp,resultObj,elemPtrs[i]);
				if (result!=TCL_OK) {Tcl_DeleteHashTable(&table);return result;}
			} else if (var == 1) {
				if (Tcl_ObjSetVar2(interp,objv[sort+2],NULL,elemPtrs[i],
					TCL_LEAVE_ERR_MSG|TCL_APPEND_VALUE|
					TCL_LIST_ELEMENT|TCL_PARSE_PART1) == NULL) {Tcl_DeleteHashTable(&table);return TCL_ERROR;}
			}
		}
		Tcl_DeleteHashTable(&table);
	} else {
		checkstring = Tcl_GetStringFromObj(elemPtrs[0], &checklen);
		result = Tcl_ListObjAppendElement(interp,resultObj,elemPtrs[0]);
		if (result != TCL_OK) {return result;}
		for(i=1;i<listLen;i++) {
			string = Tcl_GetStringFromObj(elemPtrs[i], &len);
			if ((len==checklen)&&(memcmp(string,checkstring,len)==0))	{
				if (var == 1) {
					if (Tcl_ObjSetVar2(interp,objv[sort+2],NULL,elemPtrs[i],
						TCL_LEAVE_ERR_MSG|TCL_APPEND_VALUE|
						TCL_LIST_ELEMENT|TCL_PARSE_PART1) == NULL) {return TCL_ERROR;}
				}
			} else {
				result = Tcl_ListObjAppendElement(interp,resultObj,elemPtrs[i]);
				if (result != TCL_OK) {return result;}
				checkstring = string;
				checklen = len;
			}
		}
	}

	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LlremoveCmd --
 *
 *		This procedure is invoked to process the "llremove" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LlremoveObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int refArgc;
	Tcl_Obj **refArgv;
	int listArgc;
	Tcl_Obj **listArgv;
	Tcl_Obj *resultObj;
	char *refstring,*string;
	int reflen,len,sort,var=0,add;
	int pos,result;
	int i;

	sort = 0;
	if (objc>3) {
		string = Tcl_GetStringFromObj(objv[1],NULL);
		if (strcmp(string,"-sorted") == 0) {
			sort = 1;
		}
	}
	if ((objc != (sort+3))&&(objc != (sort+4))) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-sorted? list removelist ?var?");
		return TCL_ERROR;
	}
	if (objc==(sort+4)) {var = 1;}

	if (Tcl_ListObjGetElements(interp, objv[sort+1], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[sort+2], &refArgc, &refArgv) != TCL_OK) {
		return TCL_ERROR;
	}

	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);

	if (var == 1) {
		if (Tcl_ObjSetVar2(interp,objv[sort+3],NULL,Tcl_NewObj(),
			TCL_LEAVE_ERR_MSG|TCL_PARSE_PART1) == NULL) {return TCL_ERROR;}
	}
	if (refArgc == 0) {
		Tcl_SetObjResult(interp,objv[sort+1]);
		return TCL_OK;
	}
	if (sort == 0) {
		Tcl_HashTable table;
		Tcl_HashEntry *entry;
		int new;
		if (refArgc!=0) {
			Tcl_InitHashTable(&table,TCL_STRING_KEYS);
			for(i=0;i<refArgc;i++) {
				refstring = Tcl_GetStringFromObj(refArgv[i],&reflen);
				Tcl_CreateHashEntry(&table,refstring,&new);
			}
		}
		for(pos=0;pos<listArgc;pos++) {
			if (refArgc==0) {
				if (len!=0) {
					result=Tcl_ListObjAppendElement(interp,resultObj,listArgv[pos]);
					if (result!=TCL_OK) {return result;}
				}
			} else {
				string = Tcl_GetStringFromObj(listArgv[pos],&len);
				entry = Tcl_FindHashEntry(&table,string);
				if (entry == NULL) {
					result = Tcl_ListObjAppendElement(interp,resultObj,listArgv[pos]);
					if (result!=TCL_OK) {Tcl_DeleteHashTable(&table);return result;}
				} else if (var == 1) {
					if (Tcl_ObjSetVar2(interp,objv[sort+3],NULL,listArgv[pos],
						TCL_LEAVE_ERR_MSG|TCL_APPEND_VALUE|
						TCL_LIST_ELEMENT|TCL_PARSE_PART1) == NULL) {Tcl_DeleteHashTable(&table);return TCL_ERROR;}
				}
			}
		}
		Tcl_DeleteHashTable(&table);
	} else {
		pos = 0;
		refstring = Tcl_GetStringFromObj(refArgv[pos],&reflen);
		for(i=0;i<listArgc;i++) {
			if (pos>= refArgc) {
				add = 1;
			} else {
				string = Tcl_GetStringFromObj(listArgv[i],&len);
				while(1) {
					result = memcmp(string,refstring,(reflen<len)?reflen:len);
					if (result == 0) {result = len - reflen;}
					if (result < 0) {
						add = 1;
						break;
					} else if (result > 0) {
						pos++;
						if (pos>= refArgc) {
							add = 1;
							break;
						} else {
							add = 0;
						}
						refstring = Tcl_GetStringFromObj(refArgv[pos],&reflen);
					} else {
						add = 0;
						break;
					}
				}
			}
			if (add == 1) {
				result=Tcl_ListObjAppendElement(interp,resultObj,listArgv[i]);
				if (result!=TCL_OK) {return result;}
			} else if (var == 1) {
				if (Tcl_ObjSetVar2(interp,objv[sort+3],NULL,listArgv[i],
					TCL_LEAVE_ERR_MSG|TCL_APPEND_VALUE|
					TCL_LIST_ELEMENT|TCL_PARSE_PART1) == NULL) {return TCL_ERROR;}
			}
		}
	}
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LunmergeCmd --
 *
 *		This procedure is invoked to process the "lunmerge" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LunmergeObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listArgc;
	Tcl_Obj **listArgv;
	Tcl_Obj *resultObj;
	int spacing=1;
	int pos,result;
	int i;

	if ((objc < 2)||(objc > 4)) {
		Tcl_WrongNumArgs(interp, 1, objv, "list ?spacing? ?varName?");
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[1], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}
	if (objc>=3) {
		result=Tcl_GetIntFromObj(interp, objv[2], &spacing);
		if (result!=TCL_OK) {return result;}
	}

	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);

	i=spacing;
	for(pos=0;pos<listArgc;pos++) {
		if (i!=0) {
			result=Tcl_ListObjAppendElement(interp,resultObj,listArgv[pos]);
			if (result!=TCL_OK) {return result;}
			i--;
		} else {
			i=spacing;
		}
	}
	spacing++;
	if (objc==4) {
		Tcl_Obj *valueObj;
		valueObj = Tcl_NewObj();
		for(pos=spacing-1;pos<listArgc;pos+=spacing) {
			result = Tcl_ListObjAppendElement(interp,valueObj,listArgv[pos]);
			if (result!=TCL_OK) {Tcl_DecrRefCount(valueObj);return result;}
		}
		if (Tcl_ObjSetVar2(interp, objv[3], (Tcl_Obj *) NULL,
			valueObj, (TCL_LEAVE_ERR_MSG | TCL_PARSE_PART1)) == NULL) {
				return TCL_ERROR;
		}
	}
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LmergeCmd --
 *
 *		This procedure is invoked to process the "lmerge" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LmergeObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int list1Argc;
	Tcl_Obj **list1Argv;
	int list2Argc;
	Tcl_Obj **list2Argv;
	Tcl_Obj *resultObj;
	Tcl_Obj *emptyObj;
	int spacing=1;
	int pos,pos2,result;
	int i;

	if ((objc != 3)&&(objc != 4)) {
		Tcl_WrongNumArgs(interp, 1, objv, "list1 list2 ?spacing?");
		return TCL_ERROR;
	}

	if (objc==4) {
		result=Tcl_GetIntFromObj(interp, objv[3], &spacing);
		if (result!=TCL_OK) {return result;}
	}

	if (Tcl_ListObjGetElements(interp, objv[1], &list1Argc, &list1Argv) != TCL_OK) {
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[2], &list2Argc, &list2Argv) != TCL_OK) {
		return TCL_ERROR;
	}

	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);

	pos=0;
	pos2=0;
	i=spacing;
	emptyObj = Tcl_NewStringObj("",0);
	Tcl_IncrRefCount(emptyObj);
	while(pos<list1Argc) {
		if (i!=0) {
			result=Tcl_ListObjAppendElement(interp,resultObj,list1Argv[pos]);
			if (result!=TCL_OK) {return result;}
			pos++;
			i--;
		} else {
			if (pos2<list2Argc) {
				result=Tcl_ListObjAppendElement(interp,resultObj,list2Argv[pos2]);
				if (result!=TCL_OK) {return result;}
				pos2++;
			} else {
				result=Tcl_ListObjAppendElement(interp,resultObj,emptyObj);
				if (result!=TCL_OK) {return result;}
			}
			i=spacing;
		}
	}
	if (pos2<list2Argc) {
		result = Tcl_ListObjAppendElement(interp,resultObj,list2Argv[pos2]);
		if (result!=TCL_OK) {return result;}
		pos2++;
	} else {
		result=Tcl_ListObjAppendElement(interp,resultObj,emptyObj);
		if (result!=TCL_OK) {return result;}
	}
	Tcl_DecrRefCount(emptyObj);
	return TCL_OK;
}
