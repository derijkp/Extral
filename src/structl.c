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
#include <string.h>

extern ExtraL_StructlTypeSetProc ExtraL_StructlSetDouble;
extern ExtraL_StructlTypeSetProc ExtraL_StructlSetInt;
extern ExtraL_StructlTypeSetProc ExtraL_StructlSetBool;
extern ExtraL_StructlTypeSetProc ExtraL_StructlSetRegexp;
extern ExtraL_StructlTypeSetProc ExtraL_StructlSetBetween;
extern ExtraL_StructlTypeSetProc ExtraL_StructlSetDBetween;
extern ExtraL_StructlTypeSetProc ExtraL_StructlSetDate;
extern ExtraL_StructlTypeGetProc ExtraL_StructlGetDate;
extern ExtraL_StructlTypeSetProc ExtraL_StructlSetTime;
extern ExtraL_StructlTypeGetProc ExtraL_StructlGetTime;
extern ExtraL_StructlTypeSetProc ExtraL_StructlSetList;
extern ExtraL_StructlTypeGetProc ExtraL_StructlGetList;

#ifdef unix
#define extern
#endif

struct Type {
	ExtraL_StructlTypeSetProc *setproc;
	ExtraL_StructlTypeGetProc *getproc;
};

static Tcl_HashTable typesTable;

extern int ExtraL_StructlCreateType(interp,key,setproc,getproc)
	Tcl_Interp *interp;
	char *key;
	ExtraL_StructlTypeSetProc *setproc;
	ExtraL_StructlTypeGetProc *getproc;
{
	Tcl_HashEntry *entry;
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
	return TCL_OK;
}

int ExtraL_StructlsetValidate(interp,substructure,ctag,clen,oldvalue,tagsc,tagsv,value) 
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	char *ctag;
	int clen;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	Tcl_HashEntry *entry;
	struct Type *type;
	ExtraL_StructlTypeSetProc *cmd;
	int error;
	
	entry = Tcl_FindHashEntry(&typesTable, ctag);
	if (entry != NULL) {
		type = (struct Type *)Tcl_GetHashValue(entry);
		cmd = type->setproc;
		if (cmd!=NULL) {
			error = (*cmd)(interp,substructure,oldvalue,tagsc,tagsv,value);
			if (error != TCL_OK) {return error;}
		}
		return TCL_OK;
	}

	{
		Tcl_Obj *cmdObj;
		Tcl_Obj **listv;
		int listc;

		error = Tcl_ListObjGetElements(interp, substructure, &listc, &listv);
		if (error != TCL_OK) {return error;}
	
		cmdObj = Tcl_NewStringObj("::Extral::set",13);
		Tcl_AppendToObj(cmdObj,ctag+1,clen-1);
		error = Tcl_ListObjAppendElement(interp,cmdObj,substructure);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjAppendElement(interp,cmdObj,oldvalue);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewListObj(tagsc,tagsv));
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjAppendElement(interp,cmdObj,*value);
		if (error != TCL_OK) {return error;}

		error = Tcl_EvalObj(interp,cmdObj);
		if (error != TCL_OK) {return error;}
		*value = Tcl_GetObjResult(interp);
		return TCL_OK;
	}
}

int ExtraL_StructlgetValidate(interp,substructure,ctag,clen,tagsc,tagsv,value) 
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	char *ctag;
	int clen;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	Tcl_HashEntry *entry;
	struct Type *type;
	ExtraL_StructlTypeGetProc *cmd;
	int error;
	
	entry = Tcl_FindHashEntry(&typesTable, ctag);
	if (entry != NULL) {
		type = (struct Type *)Tcl_GetHashValue(entry);
		cmd = type->getproc;
		if (cmd!=NULL) {
			error = (*cmd)(interp,substructure,tagsc,tagsv,value);
			if (error != TCL_OK) {return error;}
		}
		return TCL_OK;
	}

	{
		Tcl_Obj *cmdObj;
		Tcl_Obj **listv;
		int listc;

		error = Tcl_ListObjGetElements(interp, substructure, &listc, &listv);
		if (error != TCL_OK) {return error;}
	
		cmdObj = Tcl_NewStringObj("::Extral::get",13);
		Tcl_AppendToObj(cmdObj,ctag+1,clen-1);
		error = Tcl_ListObjAppendElement(interp,cmdObj,substructure);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewListObj(tagsc,tagsv));
		if (error != TCL_OK) {return error;}
		if (*value != NULL) {
			error = Tcl_ListObjAppendElement(interp,cmdObj,*value);
		} else {
			error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewObj());
		}
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
	Tcl_Obj *temp, *tagObj;
	char *ctag,*tag;
	Tcl_Obj *substructure, *sublist;
	int sublistpos, structpos;
	int clen,len;
	int error;
	int pos;

	if (list == NULL) {
		list = Tcl_NewObj();
	}
	Tcl_ListObjIndex(interp, structure, 0, &temp);
	if (temp != NULL) {
		ctag = Tcl_GetStringFromObj(temp,&clen);
	} else {
		clen = 0;
	}
	if ((clen>1)&&(ctag[0]=='*')) {
		/* endnode */
		error = Tcl_ListObjLength(interp,structure, &pos);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structure, pos-1, &temp);
		if (error != TCL_OK) {return error;}
		if (ExtraL_ObjEqual(interp,temp,value)==1) {
			return 5;
		} else {
			error = ExtraL_StructlsetValidate(interp,structure,ctag,clen,list,tagsc,tagsv,&value);
			if (error != TCL_OK) {return TCL_ERROR;}
			*resultPtr = value;
			return TCL_OK;
		}			
	} else if (tagsc == 0) {
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
			Tcl_AppendResult(interp,"error: incorrect value trying to assign \"", Tcl_GetStringFromObj(value,&len),"\" to struct \"", Tcl_GetStringFromObj(structure,&len),"\"",(char *)NULL);
			return TCL_ERROR;
		} else if (tempc!=0) {
			for(pos=0;pos<tempc;pos+=2) {
				tagObj = tempv[pos];
				tag = Tcl_GetStringFromObj(tagObj,&len);
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
		
				error = ExtraL_StructlsetStruct(interp, substructure, sublist, 0, NULL, tempv[pos+1], &temp);
				if (error == TCL_ERROR) {
					return error;
				} else if (error == 5) {
					sublist=temp;
					if (sublistpos != -1) {
						list = Tcl_DuplicateObj(list);
						error = Tcl_ListObjReplace(interp,list,sublistpos-1,2,0,NULL);
						if (error != TCL_OK) {
							Tcl_DecrRefCount(list);
							return error;
						}
					}
				} else {
					sublist=temp;
					list = Tcl_DuplicateObj(list);
					Tcl_GetStringFromObj(sublist,&pos);
					if (sublistpos != -1) {
						error = Tcl_ListObjReplace(interp,list,sublistpos,1,1,&sublist);
						if (error != TCL_OK) {
							Tcl_DecrRefCount(list);
							return error;
						}
					} else {
						error = Tcl_ListObjAppendElement(interp,list,tagObj);
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
			}
		}
	} else {
		/*
		# Go further down structure by tags
		# ---------------------------------
		*/
		tagObj = tagsv[0];
		tag = Tcl_GetStringFromObj(tagsv[0],&len);
		tagsc--;
		tagsv++;
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

		error = ExtraL_StructlsetStruct(interp, substructure, sublist, tagsc, tagsv, value, &temp);
		if (error == TCL_ERROR) {
			return error;
		} else if (error == 5) {
			sublist=temp;
			if (sublistpos != -1) {
				list = Tcl_DuplicateObj(list);
				error = Tcl_ListObjReplace(interp,list,sublistpos-1,2,0,NULL);
				if (error != TCL_OK) {
					Tcl_DecrRefCount(list);
					return error;
				}
			}
		} else {
			sublist = temp;
			list = Tcl_DuplicateObj(list);
			Tcl_GetStringFromObj(sublist,&pos);
			if (sublistpos != -1) {
				error = Tcl_ListObjReplace(interp,list,sublistpos,1,1,&sublist);
				if (error != TCL_OK) {
					Tcl_DecrRefCount(list);
					return error;
				}
			} else {
				error = Tcl_ListObjAppendElement(interp,list,tagObj);
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
	}
	Tcl_GetStringFromObj(list,&pos);
	if (pos == 0) {
		return 5;
	} else {
		*resultPtr=list;
		return TCL_OK;
	}
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
	Tcl_Obj *temp;
	char *tag;
	Tcl_Obj *sublist;
	int sublistpos;
	int len;
	int error;
	int i;

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
	if (temp == NULL) {
		ctag="";
	} else {
		ctag = Tcl_GetStringFromObj(temp,&clen);
	}
	/* 
		Is this an endnode
		------------------
	*/
	if ((clen>1)&&(ctag[0]=='*')) {
		if (tagsc>0) {
			error = ExtraL_StructlgetValidate(interp,structure,ctag,clen,tagsc,tagsv,&list);
			if (error != TCL_OK) {return TCL_ERROR;}
			*resultPtr = list;
			return TCL_OK;
		} else if (listlen == 0) {
			error = Tcl_ListObjLength(interp, structure, &i);
			if (error != TCL_OK) {return error;}
			error = Tcl_ListObjIndex(interp, structure, i-1, resultPtr);
			if (error != TCL_OK) {return error;}
			return TCL_OK;
		} else {
			error = ExtraL_StructlgetValidate(interp,structure,ctag,clen,0,NULL,&list);
			if (error != TCL_OK) {return TCL_ERROR;}
			*resultPtr = list;
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
	} else {
		/* 
			find substructure corresponding to tag 
			--------------------------------------
		*/
	
		tag = Tcl_GetStringFromObj(tagsv[0],&len);
		error = ExtraL_StructlFindTag(interp,structure,tag,len,&substructure,&pos);
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
		for(pos = 0; pos < listc; pos += 2) {
			ctag=Tcl_GetStringFromObj(listv[pos],&clen);
			if (clen == len) {
				if (memcmp(ctag,tag,len) == 0) {
					list = listv[++pos];
					break;
				}
			}
		}
		if (pos == listc) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"tag \"", tag,"\" not found",(char *)NULL);
			return TCL_ERROR;
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
	int pos, error, i;

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-struct schema? list taglist value ?taglist value?");
		return TCL_ERROR;
	}
	if (strncmp(Tcl_GetStringFromObj(objv[1],NULL),"-struct",7) == 0) {
		structure = objv[2];
		pos = 3;
	} else {
		pos = 1;
		structure = NULL;
	}

	if (objc & 1) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-struct schema? list taglist value ?taglist value?");
		return TCL_ERROR;
	}

	resultObj = objv[pos];
	for(i = pos+1 ; i < objc ; i+=2) {
		error = Tcl_ListObjGetElements(interp, objv[i], &tagsc, &tagsv);
		if (error != TCL_OK)	{return error;}
		if (structure == NULL) {
			if (tagsc == 0) {
				resultObj = objv[i+1];
			} else {
				error = ExtraL_Structlset(interp, resultObj, tagsc, tagsv, objv[i+1], &resultObj);
				if (error != TCL_OK)	{return error;}
			}
		} else {
			error = ExtraL_StructlsetStruct(interp, structure, resultObj, tagsc, tagsv, objv[i+1], &resultObj);
			if (error == TCL_ERROR)	{
				return error;
			} else if (error == 5) {
				resultObj = Tcl_NewObj();
				return TCL_OK;
			}
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

	if ((objc < 3)||(objc > 6)) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-struct schema? list taglist ?def?");
		return TCL_ERROR;
	}

	if ((objc == 5)||(objc == 6)) {
		if (Tcl_ListObjGetElements(interp, objv[4], &tagsc, &tagsv) != TCL_OK) {
			return TCL_ERROR;
		}
		if (ExtraL_StructlgetStruct(interp, objv[2], objv[3], tagsc, tagsv, &result) == TCL_ERROR) {
			if (objc == 6) {
				result = Tcl_DuplicateObj(objv[5]);
			} else {
				return TCL_ERROR;
			}
		}
		pos=4;
	} else {
		if (Tcl_ListObjLength(interp, objv[2], &tagsc) != TCL_OK) {
			return TCL_ERROR;
		}
		if (tagsc == 0) {
			Tcl_SetObjResult(interp,objv[1]);
			return TCL_OK;
		} else if (ExtraL_Structlget(interp, objv[1], objv[2], &result) == TCL_ERROR) {
			if (objc == 4) {
				result = Tcl_DuplicateObj(objv[3]);
			} else {
				return TCL_ERROR;
			}
		}
		pos=2;
	}

	Tcl_SetObjResult(interp,result);
	return TCL_OK;
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
	int pos;
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
	ExtraL_StructlCreateType(interp,"*any",(ExtraL_StructlTypeSetProc *)NULL,(ExtraL_StructlTypeGetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*int",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetInt,(ExtraL_StructlTypeGetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*double",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetDouble,(ExtraL_StructlTypeGetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*bool",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetBool,(ExtraL_StructlTypeGetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*regexp",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetRegexp,(ExtraL_StructlTypeGetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*between",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetBetween,(ExtraL_StructlTypeGetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*dbetween",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetDBetween,(ExtraL_StructlTypeGetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*date",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetDate,(ExtraL_StructlTypeGetProc *)ExtraL_StructlGetDate);
	ExtraL_StructlCreateType(interp,"*time",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetTime,(ExtraL_StructlTypeGetProc *)ExtraL_StructlGetTime);
	ExtraL_StructlCreateType(interp,"*list",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetList,(ExtraL_StructlTypeGetProc *)ExtraL_StructlGetList);
	return TCL_OK;
}

