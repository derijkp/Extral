/*	
 *	 File:    tagl.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tcl.h"

int ExtraL_ObjEqual(interp, obj1, obj2)
	Tcl_Interp *interp;
	Tcl_Obj *obj1;
	Tcl_Obj *obj2;
{
	char *obj1string, *obj2string;
	int obj1len, obj2len;
	obj1string=Tcl_GetStringFromObj(obj1,&obj1len);
	obj2string=Tcl_GetStringFromObj(obj2,&obj2len);
	if ((obj1len == obj2len)&&(memcmp(obj1string,obj2string,obj1len)==0)) {
		return 1;
	} else {
		return 0;
	}
}

int ExtraL_TaglFindTag(interp, list, tag, taglen, resultPtr, posPtr)
	Tcl_Interp *interp;
	Tcl_Obj *list;
	char *tag;
	int taglen;
	Tcl_Obj **resultPtr;
	int *posPtr;
{
	Tcl_Obj **listv;
	int listc;
	char *ctag;
	int clen, pos;

	if (list == NULL) {
		*posPtr = -1;
		*resultPtr = NULL;
		return TCL_OK;
	}

	if (Tcl_ListObjGetElements(interp, list, &listc, &listv) != TCL_OK) {
		return TCL_ERROR;
	}
	if ((listc != 0)&&(listc & 1)) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: \"", Tcl_GetStringFromObj(list,&pos),"\" does not have an even number of elements",(char *)NULL);
		return TCL_ERROR;
	}
	for(pos=0;pos<listc;pos+=2) {
		ctag=Tcl_GetStringFromObj(listv[pos],&clen);
		if ((clen==taglen)&&(strncmp(ctag,tag,taglen)==0)) {
			pos++;
			*posPtr = pos;
			*resultPtr = listv[pos];
			return TCL_OK;
		}
	}
	ctag=Tcl_GetStringFromObj(listv[0],&clen);
	if (strcmp(ctag,"*")==0) {
		*posPtr = 1;
		*resultPtr = listv[1];
	} else {
		*posPtr = -1;
		*resultPtr = NULL;
	}
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 *		C backend for taglset
 *     list gets changed directly, so should not be shared
 *
 *----------------------------------------------------------------------
 */

int ExtraL_Taglset(interp, structure, list, tagsc, tagsv, value, resultPtr)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *list;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj *value;
	Tcl_Obj **resultPtr;
{
	Tcl_Obj **listv;
	int listc;
	Tcl_Obj **structurev;
	int structurec;
	Tcl_Obj *temp;
	char *ctag,*tag;
	Tcl_Obj *substructure, *sublist;
	int sublistpos;
	int clen,len;
	int error;
	int pos;

	/*
			if (Tcl_ListObjGetElements(interp, list, &listc, &listv) != TCL_OK) {
				return TCL_ERROR;
			}
			if ((listc != 0)&&(listc & 1)) {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"error: list \"", Tcl_GetStringFromObj(list,&len),"\" does not have an even number of elements",(char *)NULL);
				return TCL_ERROR;
			}
	*/
	tag=Tcl_GetStringFromObj(tagsv[0],&len);

	/* check structure if needed */
	if (structure!=NULL) {
		error = ExtraL_TaglFindTag(interp, structure, tag, len, &substructure, &pos);
		if (error != TCL_OK) {return TCL_ERROR;}
		if (pos == -1) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: tag \"", tag, "\" not present in structure \"", Tcl_GetStringFromObj(structure,&len),"\"",(char *)NULL);
			return TCL_ERROR;	
		}
	} else {
		substructure = NULL;
	}

	/* try to find the next tag */
	error = ExtraL_TaglFindTag(interp, list, tag, len, &sublist, &sublistpos);
	if (error != TCL_OK) {return TCL_ERROR;}
	/*
			if (sublistpos == -1) {
				sublist=Tcl_NewObj();
			}
	*/

	/* more tags to go: change sublist accordingly */
	if (structure == NULL) {
		if (tagsc>1) {
			error=ExtraL_Taglset(interp, NULL, sublist, tagsc-1, tagsv+1, value, &temp);
			if (error != TCL_OK) {return error;}
			sublist=temp;
		} else {
			sublist=value;
		}
	} else {
		if (tagsc>1) {
			error=ExtraL_Taglset(interp, substructure, sublist, tagsc-1, tagsv+1, value, &temp);
			if (error != TCL_OK) {return error;}
			sublist=temp;
		} else {
			/* check element to structure if needed */
			Tcl_ListObjIndex(interp, substructure, 0, &temp);
			ctag=Tcl_GetStringFromObj(temp,&clen);
			if ((clen>1)&&(ctag[0]=='*')) {
				/* endnode */
				error = Tcl_ListObjLength(interp,substructure, &pos);
				if (error != TCL_OK) {return error;}
				error = Tcl_ListObjIndex(interp, substructure, pos-1, &temp);
				if (error != TCL_OK) {return error;}
				if (ExtraL_ObjEqual(interp,temp,value)==1) {
					sublist = NULL;
				} else {
					sublist = value;
				}			
				/*
				error=ExtraL_TaglsetValidate(interp,substructure,&value);
				if (error == 1) {
					return TCL_ERROR;
				}
				*/
			} else {
				/* no endnode */
				int tempc;
				Tcl_Obj **tempv, *temp;
				if (Tcl_ListObjGetElements(interp, value, &tempc, &tempv) != TCL_OK) {
					return TCL_ERROR;
				}
				if (tempc & 1) {
					Tcl_ResetResult(interp);
					Tcl_AppendResult(interp,"error: incorrect value trying to assign \"", Tcl_GetStringFromObj(value,&len),"\" to struct \"", Tcl_GetStringFromObj(substructure,&len),"\"",(char *)NULL);
					return TCL_ERROR;
				} else if (tempc!=0) {
					for(pos=0;pos<tempc;pos+=2) {
						error=ExtraL_Taglset(interp, substructure, sublist, 1, tempv+pos, tempv[pos+1], &temp);
						if (error != TCL_OK) {
							return error;
						}
						sublist=temp;
					}
				}
			}
		}
	}

	/* change or ad the element */
	if (sublist == NULL) {
		if (sublistpos != -1) {
			if (list != NULL) {
				error = Tcl_ListObjLength(interp,list,&len);
				if (error != TCL_OK) {return error;}
				if (len > 2) {
					list = Tcl_DuplicateObj(list);
					error = Tcl_ListObjReplace(interp,list,sublistpos-1,2,0,NULL);
					if (error != TCL_OK) {
						Tcl_DecrRefCount(list);
						return error;
					}
				} else {
					list = NULL;
				}
			}
		}
	} else {
		if (list == NULL) {
			list = Tcl_NewObj();
		} else {
			list = Tcl_DuplicateObj(list);
		}
		if (sublistpos != -1) {
			error = Tcl_ListObjReplace(interp,list,sublistpos,1,1,&sublist);
			if (error != TCL_OK) {
				Tcl_DecrRefCount(list);
				return error;
			}
		} else {
			error = Tcl_ListObjAppendElement(interp,list,tagsv[0]);
			if (error != TCL_OK) {
				Tcl_DecrRefCount(list);
				Tcl_DecrRefCount(temp);
				return error;
			}
			error = Tcl_ListObjAppendElement(interp,list,sublist);
			if (error != TCL_OK) {
				Tcl_DecrRefCount(list);
				Tcl_DecrRefCount(temp);
				return error;
			}
		}
	}
	*resultPtr=list;
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 *		C backend for taglunset
 *     list gets changed directly, so should not be shared
 *
 *----------------------------------------------------------------------
 */

int ExtraL_Taglunset(interp, list, tagsc, tagsv, resultPtr)
	Tcl_Interp *interp;
	Tcl_Obj *list;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **resultPtr;
{
	int workc;
	Tcl_Obj **workv;
	Tcl_Obj *temp;
	char *ctag,*tag;
	int clen,len;
	int result;
	int pos;

	if (Tcl_ListObjGetElements(interp, list, &workc, &workv) != TCL_OK) {
		return TCL_ERROR;
	}
	if ((workc != 0)&&(workc & 1)) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: list \"", Tcl_GetStringFromObj(list,&len),"\" does not have an even number of elements",(char *)NULL);
		return TCL_ERROR;
	}
	tag=Tcl_GetStringFromObj(tagsv[0],&len);
	for(pos=0;pos<workc;pos+=2) {
		ctag=Tcl_GetStringFromObj(workv[pos],&clen);
		if (clen==len) {
			if (strncmp(ctag,tag,len)==0) {
				if (tagsc==1) {
					list = Tcl_DuplicateObj(list);
					result = Tcl_ListObjReplace(interp,list,pos,2,0,NULL);
					if (result != TCL_OK) {
						Tcl_DecrRefCount(list);
						return result;
					}
					break;
				} else {
					pos++;
					result=ExtraL_Taglunset(interp, workv[pos], tagsc-1, tagsv+1, &temp);
					if (result != TCL_OK) {
						return result;
					}
					list = Tcl_DuplicateObj(list);
					result = Tcl_ListObjReplace(interp,list,pos,1,1,&temp);
					if (result != TCL_OK) {
						Tcl_DecrRefCount(list);
						return result;
					}
					break;
				}
			}
		}
	}
	*resultPtr=list;
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 *		C backend for taglget with struct
 *
 *----------------------------------------------------------------------
 */

int ExtraL_TaglgetStruct(interp, structure, list, tagsc, tagsv, resultPtr)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *list;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **resultPtr;
{
	Tcl_Obj **listv;
	int listc;
	Tcl_Obj **structurev;
	int structurec;
	Tcl_Obj *temp;
	char *ctag,*tag;
	Tcl_Obj *substructure, *sublist;
	int clen,len;
	int endnode;
	int error;
	int pos,i;

	Tcl_ListObjIndex(interp, structure, 0, &temp);
	ctag=Tcl_GetStringFromObj(temp,&clen);
	if ((clen>1)&&(ctag[0]=='*')) {
		endnode=1;
	} else {
		endnode=0;
	}

	/* 
		is this an endnode
		------------------
	*/
	if ((endnode == 1)) {
		if (list == NULL) {
			if (Tcl_ListObjLength(interp, structure, &i) != TCL_OK) {
				return TCL_ERROR;
			}
			error=Tcl_ListObjIndex(interp, structure, i-1, resultPtr);
			if (error!=TCL_OK) {
				return error;
			}
			return TCL_OK;
		} else if (tagsc>0) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: not all tags in structure",(char *)NULL);
			return TCL_ERROR;	
		} else {
			*resultPtr=list;
			return TCL_OK;
		}
	}

	/* 
		out of tags
		-----------
	*/
	if (tagsc == 0) {
		Tcl_Obj **tempv;
		int tempc;
		Tcl_Obj *temp, *result;
		int i;

		if (Tcl_ListObjGetElements(interp, structure, &tempc, &tempv) != TCL_OK) {
			return TCL_ERROR;
		}
		if ((temp != 0)&&(tempc & 1)) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: structure \"", Tcl_GetStringFromObj(substructure,&len),"\" does not have an even number of elements",(char *)NULL);
			return TCL_ERROR;
		} else if (tempc != 0) {
			result=Tcl_NewObj();
			for(pos=0;pos<tempc;pos+=2) {
				ctag=Tcl_GetStringFromObj(tempv[pos],&clen);
				if (ctag[0] == '*') continue;
				error = Tcl_ListObjAppendElement(interp,result,tempv[pos]);
				if (error != TCL_OK) {return error;}

				error=ExtraL_TaglFindTag(interp,list,ctag,clen,&sublist,&i);
				if (error != TCL_OK) {return error;}
				error=ExtraL_TaglgetStruct(interp, tempv[pos+1], sublist, 0, NULL, &temp);
				if (error != TCL_OK) {return error;}
				error = Tcl_ListObjAppendElement(interp,result,temp);
				if (error != TCL_OK) {return error;}
			}
			Tcl_IncrRefCount(result);
		}
		*resultPtr = result;
		return TCL_OK;
	}
	

	/* 
		find substructure corresponding to tag 
		--------------------------------------
	*/
	tag=Tcl_GetStringFromObj(tagsv[0],&len);
	error=ExtraL_TaglFindTag(interp,structure,tag,len,&substructure,&pos);
	if (error != TCL_OK) {return error;}
	if (pos == -1) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: tag \"", tag, "\" not present in structure \"", Tcl_GetStringFromObj(structure,&len),"\"",(char *)NULL);
		return TCL_ERROR;	
	}

	/* 
		find the tag
		------------
	*/
	error=ExtraL_TaglFindTag(interp,list,tag,len,&sublist,&pos);
	if (error != TCL_OK) {return error;}

	/* set the result */
	error=ExtraL_TaglgetStruct(interp, substructure, sublist, tagsc-1, tagsv+1, &temp);
	if (error != TCL_OK) {return error;}
	*resultPtr = temp;
	return TCL_OK;
}
/*
 *----------------------------------------------------------------------
 *
 *		C backend for taglget
 *
 *----------------------------------------------------------------------
 */

int ExtraL_Taglget(interp, list, tags, result)
	Tcl_Interp *interp;
	Tcl_Obj *list;
	Tcl_Obj *tags;
	Tcl_Obj **result;
{
	int listc;
	Tcl_Obj **listv;
	int tagsc;
	Tcl_Obj **tagsv;
	char *ctag,*tag;
	int clen,len,curtag;
	int pos;
	int i;

	if (Tcl_ListObjGetElements(interp, tags, &tagsc, &tagsv) != TCL_OK) {
		return TCL_ERROR;
	}
	for(curtag=0;curtag<tagsc;curtag++) {
		if (Tcl_ListObjGetElements(interp, list, &listc, &listv) != TCL_OK) {
			return TCL_ERROR;
		}
		if ((listc != 0)&&(listc & 1)) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: list \"", Tcl_GetStringFromObj(list,&len),"\" does not have an even number of elements",(char *)NULL);
			return TCL_ERROR;
		}
		tag=Tcl_GetStringFromObj(tagsv[curtag],&len);
		for(pos=0;pos<listc;pos+=2) {
			ctag=Tcl_GetStringFromObj(listv[pos],&clen);
			if (clen==len) {
				if (strncmp(ctag,tag,len)==0) {
					list=listv[++pos];
					break;
				}
			}
		}
		if (pos==listc) {
			list=NULL;
			break;
		}
	}
	*result=list;
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_TaglsetObjCmd --
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
	Tcl_Obj *resultObj;
	Tcl_Obj **tagsv;
	int tagsc;
	Tcl_Obj *structure;
	int pos;

	if ((objc != 4)&&(objc != 6)) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-struct schema? list taglist value");
		return TCL_ERROR;
	}

	pos=1;
	if (objc == 6) {
		structure=objv[2];
		pos+=2;
	} else {
		structure=NULL;
	}
	if (Tcl_ListObjGetElements(interp, objv[pos+1], &tagsc, &tagsv) != TCL_OK) {
		return TCL_ERROR;
	}
	if (tagsc == 0) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: empty taglist",(char *)NULL);
		return TCL_ERROR;
	}
	if (ExtraL_Taglset(interp, structure, objv[pos], tagsc, tagsv, objv[pos+2], &resultObj) != TCL_OK) {
		return TCL_ERROR;
	}
	if (resultObj == NULL) {
		resultObj = Tcl_NewObj();
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
	Tcl_Obj *resultObj;
	Tcl_Obj **tagsv;
	int tagsc;

	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "list taglist");
		return TCL_ERROR;
	}
	if (Tcl_ListObjGetElements(interp, objv[2], &tagsc, &tagsv) != TCL_OK) {
		return TCL_ERROR;
	}

	if (ExtraL_Taglunset(interp,objv[1], tagsc, tagsv, &resultObj) != TCL_OK) {
		return TCL_ERROR;
	}
	Tcl_SetObjResult(interp,resultObj);
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
	ClientData notUsed;
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
{
	Tcl_Obj **tagsv;
	int tagsc;
	Tcl_Obj *result;
	int pos;

	if ((objc != 3)&&(objc != 5)) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-struct schema? list taglist");
		return TCL_ERROR;
	}

	if (objc == 5) {
		if (Tcl_ListObjGetElements(interp, objv[4], &tagsc, &tagsv) != TCL_OK) {
			return TCL_ERROR;
		}
		if (tagsc == 0) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: empty taglist",(char *)NULL);
			return TCL_ERROR;
		}
		if (ExtraL_TaglgetStruct(interp, objv[2], objv[3], tagsc, tagsv, &result)==TCL_ERROR) {
			return TCL_ERROR;
		}
		pos=4;
	} else {
		if (ExtraL_Taglget(interp, objv[1], objv[2], &result)==TCL_ERROR) {
			return TCL_ERROR;
		}
		pos=2;
	}

	if (result!=NULL) {
		Tcl_SetObjResult(interp,result);
		return TCL_OK;
	} else {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"taglist \"", Tcl_GetStringFromObj(objv[pos],&pos),"\" not found",(char *) NULL);
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
	if ((listArgc != 0)&&(listArgc & 1)) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: list \"", Tcl_GetStringFromObj(objv[1],&i),"\" does not have an even number of elements",(char *)NULL);
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

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_TaglfindCmd --
 *
 *		This procedure is invoked to process the "taglfind" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_TaglfindObjCmd(notUsed, interp, objc, objv)
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

	if ((objc != 3)) {
		Tcl_WrongNumArgs(interp, 1, objv, "list tag");
		return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements(interp, objv[1], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}
	if ((listArgc != 0)&&(listArgc & 1)) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: list \"", Tcl_GetStringFromObj(objv[1],&i),"\" does not have an even number of elements",(char *)NULL);
		return TCL_ERROR;
	}

	tag=Tcl_GetStringFromObj(objv[2],&len);

	/* Initialise result */

	resultObj=Tcl_GetObjResult(interp);
	for(pos=0;pos<listArgc;pos+=2) {
		ctag=Tcl_GetStringFromObj(listArgv[pos],&clen);
		if (clen==len) {
			if (strncmp(ctag,tag,len)==0) {
				Tcl_SetIntObj(resultObj,++pos);
				return TCL_OK;
			}
		}
	}
	Tcl_ResetResult(interp);
	Tcl_SetIntObj(resultObj,0);
	return TCL_OK;
}
