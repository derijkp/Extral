/*	
 *	 File:    structl.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tcl.h"
#include "extral.h"

extern ExtraL_StructlTypeProc ExtraL_StructlSetDouble;
extern ExtraL_StructlTypeProc ExtraL_StructlSetInt;
extern ExtraL_StructlTypeProc ExtraL_StructlSetBool;
extern ExtraL_StructlTypeProc ExtraL_StructlSetRegexp;
extern ExtraL_StructlTypeProc ExtraL_StructlSetBetween;
extern ExtraL_StructlTypeProc ExtraL_StructlSetDBetween;
extern ExtraL_StructlTypeProc ExtraL_StructlSetDate;
extern ExtraL_StructlTypeProc ExtraL_StructlGetDate;
extern ExtraL_StructlTypeProc ExtraL_StructlSetTime;
extern ExtraL_StructlTypeProc ExtraL_StructlGetTime;

#ifdef unix
#define extern
#endif

struct Type {
	ExtraL_StructlTypeProc *setproc;
	ExtraL_StructlTypeProc *getproc;
};

static Tcl_HashTable typesTable;

extern int ExtraL_StructlCreateType(interp,key,setproc,getproc)
	Tcl_Interp *interp;
	char *key;
	ExtraL_StructlTypeProc *setproc;
	ExtraL_StructlTypeProc *getproc;
{
	Tcl_HashEntry *entry, *newentry;
	struct Type *type;
	int new;

	entry = Tcl_CreateHashEntry(&typesTable,key,&new);
	if (new == 1) {
		type = (struct Type *)Tcl_Alloc(sizeof(struct Type));
		Tcl_SetHashValue(entry,(ClientData)type);
	} else {
		type = (struct Type *)Tcl_GetHashValue(entry);
	}
	type->setproc = setproc;
	type->getproc = getproc;
}

int ExtraL_StructlsetValidate(interp,substructure,ctag,clen,oldvalue,value) 
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	char *ctag;
	int clen;
	Tcl_Obj *oldvalue;
	Tcl_Obj **value;
{
	Tcl_HashEntry *entry;
	struct Type *type;
	ExtraL_StructlTypeProc *cmd;
	int error;
	int temp;
	
	entry = Tcl_FindHashEntry(&typesTable, ctag);
	if (entry != NULL) {
		type = (struct Type *)Tcl_GetHashValue(entry);
		cmd = type->setproc;
		if (cmd!=NULL) {
			error = (*cmd)(interp,substructure,value);
			if (error != TCL_OK) {return error;}
		}
		return TCL_OK;
	}

	{
		Tcl_Obj *cmdObj, resultObj;
		Tcl_Obj **listv;
		int listc;
		int i;

		error = Tcl_ListObjGetElements(interp, substructure, &listc, &listv);
		if (error != TCL_OK) {return error;}
	
		cmdObj = Tcl_NewStringObj("::Extral::set",13);
		Tcl_AppendToObj(cmdObj,ctag+1,clen-1);
		error = Tcl_ListObjAppendElement(interp,cmdObj,substructure);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjAppendElement(interp,cmdObj,oldvalue);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjAppendElement(interp,cmdObj,*value);
		if (error != TCL_OK) {return error;}

		error = Tcl_EvalObj(interp,cmdObj);
		if (error != TCL_OK) {return error;}
		*value = Tcl_GetObjResult(interp);
		return TCL_OK;
	}
}

int ExtraL_StructlgetValidate(interp,substructure,ctag,clen,value) 
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	char *ctag;
	int clen;
	Tcl_Obj **value;
{
	Tcl_HashEntry *entry;
	struct Type *type;
	ExtraL_StructlTypeProc *cmd;
	int error;
	int temp;
	
	entry = Tcl_FindHashEntry(&typesTable, ctag);
	if (entry != NULL) {
		type = (struct Type *)Tcl_GetHashValue(entry);
		cmd = type->getproc;
		if (cmd!=NULL) {
			error = (*cmd)(interp,substructure,value);
			if (error != TCL_OK) {return error;}
		}
		return TCL_OK;
	}

	{
		Tcl_Obj *cmdObj, resultObj;
		Tcl_Obj **listv;
		int listc;
		int i;

		error = Tcl_ListObjGetElements(interp, substructure, &listc, &listv);
		if (error != TCL_OK) {return error;}
	
		cmdObj = Tcl_NewStringObj("::Extral::get",13);
		Tcl_AppendToObj(cmdObj,ctag+1,clen-1);
		error = Tcl_ListObjAppendElement(interp,cmdObj,substructure);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjAppendElement(interp,cmdObj,*value);
		if (error != TCL_OK) {return error;}

		error = Tcl_EvalObj(interp,cmdObj);
		if (error != TCL_OK) {return error;}
		*value = Tcl_GetObjResult(interp);
		return TCL_OK;
	}
}

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

int ExtraL_StructlFindTag(interp, list, tag, structlen, resultPtr, posPtr)
	Tcl_Interp *interp;
	Tcl_Obj *list;
	char *tag;
	int structlen;
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
   if (listc == 0) {
		*posPtr = -1;
		*resultPtr = NULL;
		return TCL_OK;
   }
	for(pos=0;pos<listc;pos+=2) {
		ctag=Tcl_GetStringFromObj(listv[pos],&clen);
		if ((clen==structlen)&&(memcmp(ctag,tag,structlen)==0)) {
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
 *		C backend for structlset with struct
 *     list gets changed directly, so should not be shared
 *
 *----------------------------------------------------------------------
 */

extern int ExtraL_StructlsetStruct(interp, structure, list, tagsc, tagsv, value, resultPtr)
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
	int sublistpos, structpos, endnode;
	int clen,len;
	int error;
	int pos;

	tag = Tcl_GetStringFromObj(tagsv[0],&len);
/*
printf("struct: %s\nlist: %s\ntags: %d\ntag: %s\nvalue: %s\n\n",Tcl_GetStringFromObj(structure,&error),Tcl_GetStringFromObj(list,&error),tagsc,tag,Tcl_GetStringFromObj(value,&error));
fflush(stdout);
*/
	/* check structure if needed */
	error = ExtraL_StructlFindTag(interp, structure, tag, len, &substructure, &structpos);
	if (error != TCL_OK) {return TCL_ERROR;}
	if (structpos == -1) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: tag \"", tag, "\" not present in structure \"", Tcl_GetStringFromObj(structure,&len),"\"",(char *)NULL);
		return TCL_ERROR;	
	}

	/* try to find the next tag */
	error = ExtraL_StructlFindTag(interp, list, tag, len, &sublist, &sublistpos);
	if (error != TCL_OK) {return TCL_ERROR;}
	if (sublistpos == -1) {
		sublist = Tcl_NewStringObj("",0);
	}

	Tcl_ListObjIndex(interp, substructure, 0, &temp);
	ctag = Tcl_GetStringFromObj(temp,&clen);
	endnode = 0;
	if ((clen>1)&&(ctag[0]=='*')) {
		if (tagsc>1) {
			tag = Tcl_GetStringFromObj(tagsv[1],&error);
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: tag \"", tag, "\" not present in structure \"", Tcl_GetStringFromObj(substructure,&len),"\"",(char *)NULL);
			return TCL_ERROR;	
		}
		/* endnode */
		error = Tcl_ListObjLength(interp,substructure, &pos);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, substructure, pos-1, &temp);
		if (error != TCL_OK) {return error;}
		if (ExtraL_ObjEqual(interp,temp,value)==1) {
			if (sublistpos != -1) {
				error = Tcl_ListObjReplace(interp,list,sublistpos-1,2,0,NULL);
				if (error != TCL_OK) {
					Tcl_DecrRefCount(list);
					return error;
				}
			}
			*resultPtr=list;
			return TCL_OK;
		} else {
			endnode = 1;
			error = ExtraL_StructlsetValidate(interp,substructure,ctag,clen,sublist,&value);
			if (error != TCL_OK) {return TCL_ERROR;}
			sublist = value;
		}			
	} else if (tagsc == 1) {
		/*
		# Go further down structure by value
		# ----------------------------------
		*/
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
				error=ExtraL_StructlsetStruct(interp, substructure, sublist, 1, tempv+pos, tempv[pos+1], &temp);
				if (error != TCL_OK) {
					return error;
				}
				sublist=temp;
			}
		}
	} else {
		/*
		# Go further down structure by tags
		# ---------------------------------
		*/
		error=ExtraL_StructlsetStruct(interp, substructure, sublist, tagsc-1, tagsv+1, value, &temp);
		if (error != TCL_OK) {return error;}
		sublist=temp;
	}

	/* change or ad the element */

	list = Tcl_DuplicateObj(list);
	Tcl_GetStringFromObj(sublist,&pos);
	if (sublistpos != -1) {
		if ((endnode == 1)||(pos != 0)) {
			error = Tcl_ListObjReplace(interp,list,sublistpos,1,1,&sublist);
			if (error != TCL_OK) {
				Tcl_DecrRefCount(list);
				return error;
			}
		} else {
			error = Tcl_ListObjReplace(interp,list,sublistpos-1,2,0,NULL);
			if (error != TCL_OK) {
				Tcl_DecrRefCount(list);
				return error;
			}
		}
	} else {
		if ((endnode == 1)||(pos != 0)) {
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
 *		C backend for structlset
 *     list gets changed directly, so should not be shared
 *
 *----------------------------------------------------------------------
 */

extern int ExtraL_Structlset(interp, list, tagsc, tagsv, value, resultPtr)
	Tcl_Interp *interp;
	Tcl_Obj *list;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj *value;
	Tcl_Obj **resultPtr;
{
	Tcl_Obj **listv;
	int listc;
	Tcl_Obj *temp;
	char *ctag,*tag;
	Tcl_Obj *sublist;
	int sublistpos;
	int clen,len;
	int error;
	int pos,i;

	tag = Tcl_GetStringFromObj(tagsv[0],&len);

	/* try to find the next tag */
	error = ExtraL_StructlFindTag(interp, list, tag, len, &sublist, &sublistpos);
	if (error != TCL_OK) {return TCL_ERROR;}

	/* change or ad the element */
	if (sublistpos != -1) {
		if (tagsc != 1) {
			error=ExtraL_Structlset(interp, sublist, tagsc-1, tagsv+1, value, &temp);
			if (error != TCL_OK) {return error;}
			value=temp;
		}
		list = Tcl_DuplicateObj(list);
		error = Tcl_ListObjReplace(interp,list,sublistpos,1,1,&value);
		if (error != TCL_OK) {Tcl_DecrRefCount(list);return error;}
		*resultPtr=list;
		return TCL_OK;
	} else if (tagsc == 1) {
		list = Tcl_DuplicateObj(list);
		error = Tcl_ListObjAppendElement(interp,list,tagsv[0]);
		if (error != TCL_OK) {Tcl_DecrRefCount(list);return error;}
		error = Tcl_ListObjAppendElement(interp,list,value);
		if (error != TCL_OK) {Tcl_DecrRefCount(list);return error;}
	} else {
		temp = Tcl_DuplicateObj(tagsv[tagsc-1]);
		error = Tcl_ListObjAppendElement(interp, temp, value);
		if (error != TCL_OK) {return error;}
		value = temp;

		i = tagsc-2;
		while (1) {
			if (i==0) break;
			sublist = value;
			value = Tcl_DuplicateObj(tagsv[i]);
			error = Tcl_ListObjAppendElement(interp, value, sublist);
			if (error != TCL_OK) {return error;}
			i--;
		}
		list = Tcl_DuplicateObj(list);
		error = Tcl_ListObjAppendElement(interp,list,tagsv[0]);
		if (error != TCL_OK) {Tcl_DecrRefCount(list);return error;}
		error = Tcl_ListObjAppendElement(interp,list,value);
		if (error != TCL_OK) {Tcl_DecrRefCount(list);return error;}
	}
	*resultPtr=list;
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 *		C backend for structlunset
 *     list gets changed directly, so should not be shared
 *
 *----------------------------------------------------------------------
 */

extern int ExtraL_Structlunset(interp, list, tagsc, tagsv, resultPtr)
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
			if (memcmp(ctag,tag,len)==0) {
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
					result=ExtraL_Structlunset(interp, workv[pos], tagsc-1, tagsv+1, &temp);
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
 *		C backend for structlget with struct
 *
 *----------------------------------------------------------------------
 */

extern int ExtraL_StructlgetStruct(interp, structure, list, tagsc, tagsv, resultPtr)
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
	int clen,len,listlen;
	int error;
	int pos,i;

/*
printf("struct: %s\n",Tcl_GetStringFromObj(structure,&error));
if (list != NULL) {printf("list: %s\n",Tcl_GetStringFromObj(list,&error));}
if (tagsc != 0) {printf("tags: %d\ntag: %s\n",tagsc,Tcl_GetStringFromObj(tagsv[0],&error));}
printf("\n");
fflush(stdout);
*/
	if (list != NULL) {
		error = Tcl_ListObjLength(interp, list, &listlen);
		if (error != TCL_OK) {return error;}
	} else {
		listlen = 0;
	}
	Tcl_ListObjIndex(interp, structure, 0, &temp);
	ctag=Tcl_GetStringFromObj(temp,&clen);
	/* 
		Is this an endnode
		------------------
	*/
	if ((clen>1)&&(ctag[0]=='*')) {
		if (tagsc>0) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: not all tags in structure",(char *)NULL);
			return TCL_ERROR;	
		} else if (listlen == 0) {
			error = Tcl_ListObjLength(interp, structure, &i);
			if (error != TCL_OK) {return error;}
			error = Tcl_ListObjIndex(interp, structure, i-1, resultPtr);
			if (error != TCL_OK) {return error;}
			return TCL_OK;
		} else {
			error=ExtraL_StructlgetValidate(interp,structure,ctag,clen,&list);
			if (error != TCL_OK) {return TCL_ERROR;}
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
		Tcl_Obj *temp, *result, *wstructure;
		int i;

		error = Tcl_ListObjGetElements(interp, structure, &tempc, &tempv);
		if (error != TCL_OK) {return error;}
		if (tempc & 1) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: structure \"", Tcl_GetStringFromObj(substructure,&len),"\" does not have an even number of elements",(char *)NULL);
			return TCL_ERROR;
		} else if (tempc != 0) {
			result = Tcl_NewObj();
			wstructure = NULL;
			for(pos=0;pos<tempc;pos+=2) {
				ctag = Tcl_GetStringFromObj(tempv[pos],&clen);
				if ((clen==1)&&(ctag[0] == '*')) {
					wstructure = tempv[pos+1];
					continue;
				} else {
					error = Tcl_ListObjAppendElement(interp,result,tempv[pos]);
					if (error != TCL_OK) {return error;}
					error = ExtraL_StructlFindTag(interp,list,ctag,clen,&sublist,&i);
					if (error != TCL_OK) {return error;}
					error = ExtraL_StructlgetStruct(interp, tempv[pos+1], sublist, 0, NULL, &temp);
					if (error != TCL_OK) {return error;}
					error = Tcl_ListObjAppendElement(interp,result,temp);
					if (error != TCL_OK) {return error;}
				}
			}
			if ((wstructure != NULL)&&(list != NULL)) {
				error = Tcl_ListObjGetElements(interp, list, &tempc, &tempv);
				if (error != TCL_OK) {return error;}
				for(pos=0;pos<tempc;pos+=2) {
					ctag = Tcl_GetStringFromObj(tempv[pos],&clen);
					error = ExtraL_StructlFindTag(interp,result,ctag,clen,&sublist,&i);
					if (error != TCL_OK) {return error;}
					if (i == -1) {
						error = Tcl_ListObjAppendElement(interp,result,tempv[pos]);
						if (error != TCL_OK) {return error;}
						error = ExtraL_StructlgetStruct(interp, wstructure, tempv[pos+1], 0, NULL, &temp);
						if (error != TCL_OK) {return error;}
						error = Tcl_ListObjAppendElement(interp,result,temp);
						if (error != TCL_OK) {return error;}
					}
				}
			}
			Tcl_IncrRefCount(result);
			*resultPtr = result;
			return TCL_OK;
		} else {
			Tcl_IncrRefCount(list);
			*resultPtr = list;
			return TCL_OK;
		}
	}
	

	/* 
		find substructure corresponding to tag 
		--------------------------------------
	*/

	tag=Tcl_GetStringFromObj(tagsv[0],&len);
	error=ExtraL_StructlFindTag(interp,structure,tag,len,&substructure,&pos);
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
	error=ExtraL_StructlFindTag(interp,list,tag,len,&sublist,&pos);
	if (error != TCL_OK) {return error;}

	/* set the result */
	error=ExtraL_StructlgetStruct(interp, substructure, sublist, tagsc-1, tagsv+1, &temp);
	if (error != TCL_OK) {return error;}
	*resultPtr = temp;
	return TCL_OK;
}
/*
 *----------------------------------------------------------------------
 *
 *		C backend for structlget
 *
 *----------------------------------------------------------------------
 */

extern int ExtraL_Structlget(interp, list, tags, result)
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
		tag = Tcl_GetStringFromObj(tagsv[curtag],&len);
		for(pos = 0;pos < listc;pos += 2) {
			ctag=Tcl_GetStringFromObj(listv[pos],&clen);
			if (clen == len) {
				if (memcmp(ctag,tag,len) == 0) {
					list = listv[++pos];
					break;
				}
			}
		}
		if (pos == listc) {
			list = NULL;
			break;
		}
	}
	*result = list;
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_StructlsetObjCmd --
 *
 *		This procedure is invoked to process the "structlset" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_StructlsetObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *resultObj;
	Tcl_Obj **tagsv;
	int tagsc;
	Tcl_Obj *structure;
	int pos, error,i;

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
	error = Tcl_ListObjGetElements(interp, objv[pos+1], &tagsc, &tagsv);
	if (error != TCL_OK)	{return error;}
	if (structure == NULL) {
		if (tagsc == 0) {
			resultObj = objv[pos+2];
		} else {
			error = ExtraL_Structlset(interp, objv[pos], tagsc, tagsv, objv[pos+2], &resultObj);
			if (error != TCL_OK)	{return error;}
		}
	} else {
		if (tagsc == 0) {
			error = Tcl_ListObjGetElements(interp, objv[pos+2], &tagsc, &tagsv);
			if (error != TCL_OK)	{return error;}
			resultObj = objv[pos];
			for(i = 0 ; i < tagsc ; i+=2) {
				error = ExtraL_StructlsetStruct(interp, structure, resultObj, 1, tagsv+i, tagsv[i+1], &resultObj);
				if (error != TCL_OK)	{return error;}				
			}
		} else {
			error = ExtraL_StructlsetStruct(interp, structure, objv[pos], tagsc, tagsv, objv[pos+2], &resultObj);
			if (error != TCL_OK)	{return error;}
		}
	}
	Tcl_SetObjResult(interp,resultObj);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_StructlunsetCmd --
 *
 *		This procedure is invoked to process the "structlunset" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_StructlunsetObjCmd(notUsed, interp, objc, objv)
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

	if (ExtraL_Structlunset(interp,objv[1], tagsc, tagsv, &resultObj) != TCL_OK) {
		return TCL_ERROR;
	}
	Tcl_SetObjResult(interp,resultObj);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_StructlgetCmd --
 *
 *		This procedure is invoked to process the "structlget" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_StructlgetObjCmd(notUsed, interp, objc, objv)
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
		if (ExtraL_StructlgetStruct(interp, objv[2], objv[3], tagsc, tagsv, &result)==TCL_ERROR) {
			return TCL_ERROR;
		}
		pos=4;
	} else {
		if (tagsc == 0) {
			Tcl_SetObjResult(interp,objv[1]);
			return TCL_OK;
		} else if (ExtraL_Structlget(interp, objv[1], objv[2], &result)==TCL_ERROR) {
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
 * ExtraL_StructlfieldsCmd --
 *
 *		This procedure is invoked to process the "structlfields" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_StructlfieldsObjCmd(notUsed, interp, objc, objv)
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
 * ExtraL_StructlfindCmd --
 *
 *		This procedure is invoked to process the "structlfind" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_StructlfindObjCmd(notUsed, interp, objc, objv)
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
			if (memcmp(ctag,tag,len)==0) {
				Tcl_SetIntObj(resultObj,++pos);
				return TCL_OK;
			}
		}
	}
	Tcl_ResetResult(interp);
	Tcl_SetIntObj(resultObj,-1);
	return TCL_OK;
}

int Extral_StructlInit(interp)
	Tcl_Interp *interp;
{
	Tcl_InitHashTable(&typesTable,TCL_STRING_KEYS);
	Tcl_CreateObjCommand(interp,"Extral::structlset",(Tcl_ObjCmdProc *)ExtraL_StructlsetObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::structlunset",(Tcl_ObjCmdProc *)ExtraL_StructlunsetObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::structlget",(Tcl_ObjCmdProc *)ExtraL_StructlgetObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::structlfields",(Tcl_ObjCmdProc *)ExtraL_StructlfieldsObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::structlfind",(Tcl_ObjCmdProc *)ExtraL_StructlfindObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	ExtraL_StructlCreateType(interp,"*any",(ExtraL_StructlTypeProc *)NULL,(ExtraL_StructlTypeProc *)NULL);
	ExtraL_StructlCreateType(interp,"*int",(ExtraL_StructlTypeProc *)ExtraL_StructlSetInt,(ExtraL_StructlTypeProc *)NULL);
	ExtraL_StructlCreateType(interp,"*double",(ExtraL_StructlTypeProc *)ExtraL_StructlSetDouble,(ExtraL_StructlTypeProc *)NULL);
	ExtraL_StructlCreateType(interp,"*bool",(ExtraL_StructlTypeProc *)ExtraL_StructlSetBool,(ExtraL_StructlTypeProc *)NULL);
	ExtraL_StructlCreateType(interp,"*regexp",(ExtraL_StructlTypeProc *)ExtraL_StructlSetRegexp,(ExtraL_StructlTypeProc *)NULL);
	ExtraL_StructlCreateType(interp,"*between",(ExtraL_StructlTypeProc *)ExtraL_StructlSetBetween,(ExtraL_StructlTypeProc *)NULL);
	ExtraL_StructlCreateType(interp,"*dbetween",(ExtraL_StructlTypeProc *)ExtraL_StructlSetDBetween,(ExtraL_StructlTypeProc *)NULL);
	ExtraL_StructlCreateType(interp,"*date",(ExtraL_StructlTypeProc *)ExtraL_StructlSetDate,(ExtraL_StructlTypeProc *)ExtraL_StructlGetDate);
	ExtraL_StructlCreateType(interp,"*time",(ExtraL_StructlTypeProc *)ExtraL_StructlSetTime,(ExtraL_StructlTypeProc *)ExtraL_StructlGetTime);
}

