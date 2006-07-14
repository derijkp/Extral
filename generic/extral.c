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
#include "general.h"
#define EXACT		0
#define GLOB		1
#define REGEXP		2

int
Extral_inlist(Tcl_Interp *interp, Tcl_Obj *listObj, Tcl_Obj *testObj,int *result)
{
	Tcl_Obj **listobjv;
	char *teststring,*string;
	int testlen, len, listobjc,j,error;
	teststring = Tcl_GetStringFromObj(testObj, &testlen);
	error = Tcl_ListObjGetElements(interp, listObj, &listobjc, &listobjv);
	if (error != TCL_OK) {return error;}
	for ( j = 0 ; j < listobjc ; j++ ) {
		string = Tcl_GetStringFromObj(listobjv[j],&len);
		if ((len == testlen) && (strncmp(string,teststring,len) == 0)) {
			*result = 1;
			return TCL_OK;
		}
	}
	*result = 0;
	return TCL_OK;
}

int
Extral_lcommon(Tcl_Interp *interp, Tcl_Obj *listObj, Tcl_Obj *testObj,int *result)
{
	Tcl_Obj **listobjv,**testobjv;
	char *teststring,*string;
	int testlen, len, listobjc,testobjc,i,j,error;
	error = Tcl_ListObjGetElements(interp, listObj, &listobjc, &listobjv);
	if (error != TCL_OK) {return error;}
	error = Tcl_ListObjGetElements(interp, testObj, &testobjc, &testobjv);
	if (error != TCL_OK) {return error;}

	for ( j = 0 ; j < listobjc ; j++ ) {
		for ( i = 0 ; i < testobjc ; i++ ) {
			teststring = Tcl_GetStringFromObj(testobjv[i], &testlen);
			string = Tcl_GetStringFromObj(listobjv[j],&len);
			if ((len == testlen) && (strncmp(string,teststring,len) == 0)) {
				*result = 1;
				return TCL_OK;
			}
		}
	}
	*result = 0;
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_List_findCmd --
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
ExtraL_List_findObjCmd(clientData, interp, objc, objv)
	ClientData clientData;	/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument values. */
{
#define EXACT	0
#define GLOB	1
#define REGEXP	2
#define INLIST	3
#define OFLIST	4
#define LCOMMON	5
	char *bytes, *patternBytes;
	int i, j, match, mode, result, listLen, length, elemLen;
	Tcl_Obj **elemPtrs;
	Tcl_Obj *indexObj, *resultObj;
	static char *switches[] =
		{"-exact", "-glob", "-regexp", "-inlist", "-oflist", "-lcommon", (char *) NULL};

	mode = EXACT;
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
			case INLIST:
				result = Extral_inlist(interp,elemPtrs[i],objv[objc-1],&match);
				if (result != TCL_OK) {return result;}
				break;
			case OFLIST:
				result = Extral_inlist(interp,objv[objc-1],elemPtrs[i],&match);
				if (result != TCL_OK) {return result;}
				break;
			case LCOMMON:
				result = Extral_lcommon(interp,elemPtrs[i],objv[objc-1],&match);
				if (result != TCL_OK) {return result;}
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

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_List_subCmd --
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
ExtraL_List_subObjCmd(dummy, interp, objc, objv)
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
		char *map = NULL;
		map = Tcl_Alloc(listLen*sizeof(char));
		memset(map,0,listLen);
		if (indexlistLen==0) {
			Tcl_SetObjResult(interp, listPtr);
			Tcl_Free(map);return TCL_OK;
		}
		for(i=0;i<indexlistLen;i++) {
			result=Tcl_GetIntFromObj(interp,indexelemPtrs[i],&index);
			if (result!=TCL_OK) {Tcl_Free(map);return result;}
			if ((index >= 0) && (index < listLen)) {
				map[index] = 1;
			}
		}
		for(i=0;i<listLen;i++) {

			if (map[i] == 0) {
				result=Tcl_ListObjAppendElement(interp,resultObj,elemPtrs[i]);
				if (result!=TCL_OK) {Tcl_Free(map);return result;}
			}
		}
		Tcl_Free(map);
	}
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_List_corCmd --
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
ExtraL_List_corObjCmd(notUsed, interp, objc, objv)
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
 * ExtraL_List_remdupCmd --
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
ExtraL_List_remdupObjCmd(dummy, interp, objc, objv)
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
	int i,len,checklen,sort,var = 0;

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
 * ExtraL_List_lremoveCmd --
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
ExtraL_List_lremoveObjCmd(notUsed, interp, objc, objv)
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
 * ExtraL_List_unmergeCmd --
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
ExtraL_List_unmergeObjCmd(notUsed, interp, objc, objv)
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
 * ExtraL_List_mergeCmd --
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
ExtraL_List_mergeObjCmd(notUsed, interp, objc, objv)
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

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_List_reverseObjCmd --
 *
 *		This procedure is invoked to process the "lreverse" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_List_reverseObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj **Argv;
	int Argc;
	Tcl_Obj *resultObj;
	int error;
	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "list");
		return TCL_ERROR;
	}
	if (Tcl_ListObjGetElements(interp, objv[1], &Argc, &Argv) != TCL_OK) {
		return TCL_ERROR;
	}
	resultObj = Tcl_NewObj();
	Argc--;
	while (Argc >= 0) {
		error = Tcl_ListObjAppendElement(interp,resultObj,Argv[Argc]);
		if (error != TCL_OK) {Tcl_DecrRefCount(resultObj);return error;}
		Argc--;
	}
	Tcl_SetObjResult(interp,resultObj);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_List_changeCmd --
 *
 *		This procedure is invoked to process the "list_change" command.
 *		It creates a subset of a list
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_List_changeObjCmd(dummy, interp, objc, objv)
	ClientData dummy;		/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *listPtr;
	Tcl_Obj **elemPtrs, **changeObjv;
	Tcl_Obj *resultObj;
	int listLen, changeObjc, result;
	char *string,*cstring;
	int i,y,len,clen;
	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "list changelist");
		return TCL_ERROR;
	}
	if (Tcl_ListObjGetElements(interp, objv[2], &changeObjc, &changeObjv) != TCL_OK) {
		return TCL_ERROR;
	}
	/*
	 * Convert the first argument to a list if necessary.
	 */
	listPtr = objv[1];
	result = Tcl_ListObjGetElements(interp, listPtr, &listLen, &elemPtrs);
	if (result != TCL_OK) {return result;}
	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);
	if (listLen==0) {
		Tcl_SetObjResult(interp, listPtr);
		return TCL_OK;
	}
	for(i = 0 ; i < listLen ; i++) {
		string = Tcl_GetStringFromObj(elemPtrs[i], &len);
		for(y = 0 ; y < changeObjc ; y += 2) {	
			cstring = Tcl_GetStringFromObj(changeObjv[y], &clen);
			if ((len == clen) && (strncmp(string,cstring,len) == 0)) {
				break;
			}
		}
		if (y < changeObjc) {
			result = Tcl_ListObjAppendElement(interp,resultObj,changeObjv[y+1]);
		} else {
			result = Tcl_ListObjAppendElement(interp,resultObj,elemPtrs[i]);
		}
	}
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_GetObjCmd --
 *
 *		This procedure is invoked to process the "get" command.
 *		It returns the value of a variable, if the variable does not exist, 
 *		an empty string or a default value is returned
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_GetObjCmd(dummy, interp, objc, objv)
	ClientData dummy;		/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *result;
	if ((objc != 2) && (objc != 3)) {
		Tcl_WrongNumArgs(interp, 1, objv, "varName ?default?");
		return TCL_ERROR;
	}
	result = Tcl_ObjGetVar2(interp,objv[1],NULL,0);
	if (result == NULL) {
		if (objc == 2) {
			Tcl_SetObjResult(interp,Tcl_NewObj());
		} else {
			Tcl_SetObjResult(interp,objv[2]);
		}
	} else {
		Tcl_SetObjResult(interp,result);
	}
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_List_fillObjCmd --
 *
 *		This procedure is invoked to process the "list_fill" command.
 *		It returns the value of a variable, if the variable does not exist, 
 *		an empty string or a default value is returned
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_List_fillObjCmd(dummy, interp, objc, objv)
	ClientData dummy;		/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *result,el;
	double start,incr;
	int size, error, error2, i, type, starti,incri;
	if ((objc != 3) && (objc != 4)) {
		Tcl_WrongNumArgs(interp, 1, objv, "size start ?incr?");
		return TCL_ERROR;
	}
	error = Tcl_GetIntFromObj(interp,objv[1],&size);
	if (error) {return error;}
	if (objc == 3) {
		result = Tcl_NewListObj(0,NULL);
		for (i = 0 ; i < size ; i++) {
			error = Tcl_ListObjAppendElement(interp,result,objv[2]);
		}
		Tcl_SetObjResult(interp,result);
		return TCL_OK;
	}
	error = Tcl_GetIntFromObj(interp,objv[2],&starti);
	error2 = Tcl_GetIntFromObj(interp,objv[3],&incri);
	if (!error && !error2) {
		result = Tcl_NewListObj(0,NULL);
		for (i = 0 ; i < size ; i++) {
			error = Tcl_ListObjAppendElement(interp,result,Tcl_NewIntObj(starti));
			if (error) {
				Tcl_DecrRefCount(result);
				return error;
			}
			starti += incri;
		}
		Tcl_SetObjResult(interp,result);
		return TCL_OK;
	}
	error = Tcl_GetDoubleFromObj(interp,objv[2],&start);
	if (error) {return error;}
	error = Tcl_GetDoubleFromObj(interp,objv[3],&incr);
	if (error) {return error;}
	result = Tcl_NewListObj(0,NULL);
	for (i = 0 ; i < size ; i++) {
		error = Tcl_ListObjAppendElement(interp,result,Tcl_NewDoubleObj(start));
		if (error) {
			Tcl_DecrRefCount(result);
			return error;
		}
		start += incr;
	}
	Tcl_SetObjResult(interp,result);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_List_concatObjCmd --
 *
 *		This procedure is invoked to process the "list_concat" command.
 *		It returns the value of a variable, if the variable does not exist, 
 *		an empty string or a default value is returned
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_List_concatObjCmd(dummy, interp, objc, objv)
	ClientData dummy;		/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *result,**listobjv;
	int i,error,listobjc;
	if (objc == 2) {
		error = Tcl_ListObjGetElements(interp,objv[1],&listobjc,&listobjv);
		if (error) {return error;}
		i = 0;
	} else {
		listobjc = objc;
		listobjv = objv;
		i = 1;
	}
	result = Tcl_NewListObj(0,NULL);
	for (; i < listobjc ; i++) {
		error = Tcl_ListObjAppendList(interp,result,listobjv[i]);
		if (error) {Tcl_DecrRefCount(result);return error;}
	}
	Tcl_SetObjResult(interp,result);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_List_inlistObjCmd --
 *
 *		This procedure is invoked to process the "list_ffill" command.
 *		It returns the value of a variable, if the variable does not exist, 
 *		an empty string or a default value is returned
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_List_inlistObjCmd(dummy, interp, objc, objv)
	ClientData dummy;		/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listobjc;
	Tcl_Obj **listobjv;
	char *string1,*string;
	int i,error,len1,len;
	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "list value");
		return TCL_ERROR;
	}
	string = Tcl_GetStringFromObj(objv[2],&len);
	if (Tcl_ListObjGetElements(interp, objv[1], &listobjc, &listobjv) != TCL_OK) {
		return TCL_ERROR;
	}
	for ( i = 0 ; i < listobjc ; i++ ) {
		string1 = Tcl_GetStringFromObj(listobjv[i],&len1);
		if ((len1 == len) && (strncmp(string1,string,len) == 0)) break;
	}
	if (i == listobjc) {
		Tcl_SetObjResult(interp,Tcl_NewIntObj(0));
	} else {
		Tcl_SetObjResult(interp,Tcl_NewIntObj(1));
	}
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_List_subindexCmd --
 *
 *		This procedure is invoked to process the "list_subindex" command.
 *		It creates a subset of a list
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_List_subindexObjCmd(dummy, interp, objc, objv)
	ClientData dummy;		/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj **listPtr,**linePtr;
	Tcl_Obj *tempObj = NULL,*nullObj = NULL;
	Tcl_Obj *resultObj, *indexObj;
	int listLen, lineLen, index, error;
	int i,j,*pos = NULL,posLen;
	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "list pos ?pos ...?");
		return TCL_ERROR;
	}
	/*
	 * Convert the first argument to a list if necessary.
	 */
	error = Tcl_ListObjGetElements(interp, objv[1], &listLen, &listPtr);
	if (error) {return error;}
	posLen = objc-2;
	pos = (int *)Tcl_Alloc(posLen*sizeof(int));
	for (i = 2 ; i < objc ; i++) {
		error = Tcl_GetIntFromObj(interp,objv[i],pos+i-2);
		if (error) {goto error;}
		if (pos[i-2] < 0) {
			Tcl_AppendResult(interp, "negative index not allowed", (char *) NULL);
			return TCL_ERROR;
		}
	}
	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);
	nullObj = Tcl_NewObj();
	Tcl_IncrRefCount(nullObj);
	for(i = 0 ; i < listLen ; i++) {
		error = Tcl_ListObjGetElements(interp, listPtr[i], &lineLen, &linePtr);
		if (error) {goto error;}
		if (objc == 3) {
			if (pos[0] < lineLen) {
				tempObj = linePtr[pos[0]];
			} else {
				tempObj = nullObj;
			}
		} else {
			tempObj = Tcl_NewListObj(0,NULL);
			if (tempObj == NULL) {goto error;}
			for (j = 0; j < posLen ; j++) {
				if (pos[j] < lineLen) {
					error = Tcl_ListObjAppendElement(interp,tempObj,linePtr[pos[j]]);
				} else {
					error = Tcl_ListObjAppendElement(interp,tempObj,nullObj);
				}
				if (error) {goto error;}
			}

		}
		error = Tcl_ListObjAppendElement(interp,resultObj,tempObj);
		if (error) {goto error;}
		tempObj = NULL;
	}
	Tcl_DecrRefCount(nullObj);
	Tcl_Free((char *)pos);
	return TCL_OK;
	error:
		if (pos != NULL) {Tcl_Free((char *)pos);}
		if (tempObj != NULL) {Tcl_DecrRefCount(tempObj);}
		if (nullObj != NULL) {Tcl_DecrRefCount(nullObj);}
		return TCL_ERROR;
}

