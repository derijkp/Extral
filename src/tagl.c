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
	int result;
	int pos;

	if (Tcl_ListObjGetElements(interp, list, &listc, &listv) != TCL_OK) {
		return TCL_ERROR;
	}
	if ((listc != 0)&&(listc & 1)) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: list \"", Tcl_GetStringFromObj(list,&len),"\" does not have an even number of elements",(char *)NULL);
		return TCL_ERROR;
	}

	tag=Tcl_GetStringFromObj(tagsv[0],&len);

	/* check structure if needed */
	if (structure!=NULL) {
		if (Tcl_ListObjGetElements(interp, structure, &structurec, &structurev) != TCL_OK) {
			return TCL_ERROR;
		}
		if ((structurec != 0)&&(structurec & 1)) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: structure \"", Tcl_GetStringFromObj(structure,&len),"\" does not have an even number of elements",(char *)NULL);
			return TCL_ERROR;
		}
		for(pos=0;pos<structurec;pos+=2) {
			ctag=Tcl_GetStringFromObj(structurev[pos],&clen);
			if ((clen==len)&&(strncmp(ctag,tag,len)==0)) {
				break;
			}
		}
		if (pos<structurec) {
			substructure=structurev[++pos];
		} else {
			ctag=Tcl_GetStringFromObj(structurev[0],&clen);
			if (strcmp(ctag,"*")==0) {
				substructure=structurev[1];
			} else {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"error: tag \"", tag, "\" not present in structure \"", Tcl_GetStringFromObj(structure,&len),"\"",(char *)NULL);
				return TCL_ERROR;	
			}
		}
	}

	/* try to find the next tag */
	for(pos=0;pos<listc;pos+=2) {
		ctag=Tcl_GetStringFromObj(listv[pos],&clen);
		if (clen==len) {
			if (strncmp(ctag,tag,len)==0) break;
		}
	}
	if (pos<listc) {
		pos++;
		sublist=listv[pos];
		sublistpos=pos;
	} else {
		sublist=Tcl_NewObj();
		sublistpos=0;
	}

	/* more tags to go: change sublist accordingly */
	if (tagsc>1) {
		result=ExtraL_Taglset(interp, substructure, sublist, tagsc-1, tagsv+1, value, &temp);
		if (result != TCL_OK) {return result;}
		sublist=temp;
	} else if (structure!=NULL) {
		/* check element to structure if needed */
		Tcl_ListObjIndex(interp, substructure, 0, &temp);
		ctag=Tcl_GetStringFromObj(temp,&clen);
		if ((clen>1)&&(ctag[0]=='*')) {
			/* endnode */
			/*
			result=ExtraL_TaglsetValidate(interp,substructure,&value);
			if (result == 1) {
				return TCL_ERROR;
			} else if (result == 0) {
				list = Tcl_DuplicateObj(list);
				result = Tcl_ListObjAppendElement(interp,list,tagsv[0]);
				if (result != TCL_OK) {
					Tcl_DecrRefCount(list);
					return result;
				}
				result = Tcl_ListObjAppendElement(interp,list,value);
				if (result != TCL_OK) {
					Tcl_DecrRefCount(list);
					return result;
				}
			}
			*/
			sublist=value;
		} else {
			/* no endnode */
			int tempc;
			Tcl_Obj **tempv, *temp;
			if (Tcl_ListObjGetElements(interp, value, &tempc, &tempv) != TCL_OK) {
				return TCL_ERROR;
			}
			if (tempc==0) {
				sublist=value;
			} else if (tempc & 1) {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"error: incorrect value trying to assign \"", Tcl_GetStringFromObj(value,&len),"\" to struct \"", Tcl_GetStringFromObj(substructure,&len),"\"",(char *)NULL);
				return TCL_ERROR;
			} else {
				for(pos=0;pos<tempc;pos+=2) {
					result=ExtraL_Taglset(interp, substructure, sublist, 1, tempv+pos, tempv[pos+1], &temp);
					if (result != TCL_OK) {
						return result;
					}
					sublist=temp;
				}
			}
		}
	}

	/* change or ad the element */
	if (sublistpos!=0) {
		list = Tcl_DuplicateObj(list);
		result = Tcl_ListObjReplace(interp,list,sublistpos,1,1,&sublist);
		if (result != TCL_OK) {
			Tcl_DecrRefCount(list);
			return result;
		}
	} else {
		list = Tcl_DuplicateObj(list);
		result = Tcl_ListObjAppendElement(interp,list,tagsv[0]);
		if (result != TCL_OK) {
			Tcl_DecrRefCount(list);
			Tcl_DecrRefCount(temp);
			return result;
		}
		result = Tcl_ListObjAppendElement(interp,list,sublist);
		if (result != TCL_OK) {
			Tcl_DecrRefCount(list);
			Tcl_DecrRefCount(temp);
			return result;
		}
	}
	*resultPtr=list;
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
	Tcl_Obj *structure;
	int pos;
	int tagsc;

	if ((objc != 4)&&(objc != 6)) {
		Tcl_WrongNumArgs(interp, 1, objv, "list ?-struct schema? taglist value");
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
	if (ExtraL_Taglset(interp, structure, objv[pos], tagsc, tagsv, objv[pos+2], &resultObj) != TCL_OK) {
		return TCL_ERROR;
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
	Tcl_Obj *result;
	int error;

	if ((objc != 3)&&(objc != 4)) {
		Tcl_WrongNumArgs(interp, 1, objv, "list taglist ?default?");
		return TCL_ERROR;
	}

	if (ExtraL_Taglget(interp,objv[1], objv[2], &result)==TCL_ERROR) {
		return error;
	}
	if (result!=NULL) {
		Tcl_SetObjResult(interp,result);
		return TCL_OK;
	} else if (objc==4) {
		Tcl_SetObjResult(interp,objv[3]);
		return TCL_OK;
	} else {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"taglist \"", Tcl_GetStringFromObj(objv[2],&error),"\" not found",(char *) NULL);
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
