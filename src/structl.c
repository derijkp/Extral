/*	
 *	 File:	structl.c
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
extern ExtraL_StructlTypeSetProc ExtraL_StructlSetNamed;
extern ExtraL_StructlTypeGetProc ExtraL_StructlGetNamed;
extern ExtraL_StructlTypeUnsetProc ExtraL_StructlUnsetList;
extern ExtraL_StructlTypeUnsetProc ExtraL_StructlUnsetNamed;

#ifdef unix
#define extern
#endif

struct Type {
	ExtraL_StructlTypeSetProc *setproc;
	ExtraL_StructlTypeGetProc *getproc;
	ExtraL_StructlTypeUnsetProc *unsetproc;
};

static Tcl_HashTable typesTable;

/*
int ExtraL_CopyObj(Tcl_Obj *objPtr,Tcl_Obj *dupPtr) {
	register Tcl_ObjType *typePtr = objPtr->typePtr;
	if (objPtr->bytes == NULL) {
		dupPtr->bytes = NULL;
	} else if (objPtr->bytes != tclEmptyStringRep) {
		int len = objPtr->length;

		dupPtr->bytes = (char *) ckalloc((unsigned) len+1);
		if (len > 0) {
			memcpy((VOID *) dupPtr->bytes, (VOID *) objPtr->bytes,(unsigned) len);
		}
		dupPtr->bytes[len] = '\0';
		dupPtr->length = len;
	}
	if (typePtr != NULL) {
		typePtr->dupIntRepProc(objPtr, dupPtr);
	}
*/
/*	string = Tcl_GetStringFromObj(src,&len);
	Tcl_SetStringObj(dst,string,len);
*/
/*}*/

extern int ExtraL_StructlCreateType(interp,key,setproc,getproc,unsetproc)
	Tcl_Interp *interp;
	char *key;
	ExtraL_StructlTypeSetProc *setproc;
	ExtraL_StructlTypeGetProc *getproc;
	ExtraL_StructlTypeUnsetProc *unsetproc;
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
	type->unsetproc = unsetproc;
	return TCL_OK;
}

int ExtraL_StructlsetValidate(interp,substructure,data,ctag,clen,oldvalue,tagsc,tagsv,value,resultPtr) 
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj *data;
	char *ctag;
	int clen;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj *value;
	Tcl_Obj **resultPtr;
{
	Tcl_HashEntry *entry;
	struct Type *type;
	ExtraL_StructlTypeSetProc *cmd;
	int error;
	
	entry = Tcl_FindHashEntry(&typesTable, ctag);
	if (entry != NULL) {
		type = (struct Type *)Tcl_GetHashValue(entry);
		*resultPtr = value;
		cmd = type->setproc;
		if (cmd!=NULL) {
			error = (*cmd)(interp,substructure,data,oldvalue,tagsc,tagsv,resultPtr);
			if (error != TCL_OK) {return error;}
		}
		return TCL_OK;
	} else {
		Tcl_Obj *cmdObj;
		Tcl_Obj **listv;
		int listc;

		error = Tcl_ListObjGetElements(interp, substructure, &listc, &listv);
		if (error != TCL_OK) {return error;}
		cmdObj = Tcl_NewStringObj("::Extral::set",13);
		Tcl_AppendToObj(cmdObj,ctag+1,clen-1);
		error = Tcl_ListObjAppendElement(interp,cmdObj,substructure);
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}
		if (data != NULL) {
			error = Tcl_ListObjAppendElement(interp,cmdObj,data);
		} else {
			error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewObj());
		}
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}
		if (oldvalue == NULL) {
			error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewObj());
		} else {
			error = Tcl_ListObjAppendElement(interp,cmdObj,oldvalue);
		}
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}
		error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewListObj(tagsc,tagsv));
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}
		error = Tcl_ListObjAppendElement(interp,cmdObj,value);
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}
		error = Tcl_EvalObj(interp,cmdObj);
		Tcl_DecrRefCount(cmdObj);
		if (error == 5) {
			return 5;
		} else if (error != TCL_OK) {
			return error;
		} else {
			*resultPtr = Tcl_GetObjResult(interp);
			return TCL_OK;
		}
	}
}

int ExtraL_StructlunsetValidate(interp,substructure,data,ctag,clen,oldvalue,tagsc,tagsv,resultPtr)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj *data;
	char *ctag;
	int clen;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **resultPtr;
{
	Tcl_HashEntry *entry;
	struct Type *type;
	ExtraL_StructlTypeSetProc *cmd;
	int error;
	
	entry = Tcl_FindHashEntry(&typesTable, ctag);
	if (entry != NULL) {
		type = (struct Type *)Tcl_GetHashValue(entry);
		cmd = type->unsetproc;
		if (cmd!=NULL) {
			error = (*cmd)(interp,substructure,data,oldvalue,tagsc,tagsv,resultPtr);
			if (error != TCL_OK) {return error;}
			return TCL_OK;
		} else {
			return 5;
		}
	}

	{
		Tcl_Obj *cmdObj;
		Tcl_Obj **listv;
		int listc;

		error = Tcl_ListObjGetElements(interp, substructure, &listc, &listv);
		if (error != TCL_OK) {return error;}
		cmdObj = Tcl_NewStringObj("::Extral::unset",13);
		Tcl_AppendToObj(cmdObj,ctag+1,clen-1);
		error = Tcl_ListObjAppendElement(interp,cmdObj,substructure);
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}
		if (data != NULL) {
			error = Tcl_ListObjAppendElement(interp,cmdObj,data);
		} else {
			error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewObj());
		}
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}
		if (oldvalue == NULL) {
			error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewObj());
		} else {
			error = Tcl_ListObjAppendElement(interp,cmdObj,oldvalue);
		}
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}
		error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewListObj(tagsc,tagsv));
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}

		error = Tcl_EvalObj(interp,cmdObj);		Tcl_DecrRefCount(cmdObj);
		if (error == 5) {
			return 5;
		} else if (error != TCL_OK) {
			return error;
		} else {
			*resultPtr = Tcl_GetObjResult(interp);
			return TCL_OK;
		}
	}
}

int ExtraL_StructlgetValidate(interp,substructure,data,ctag,clen,tagsc,tagsv,resultPtr) 
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj *data;
	char *ctag;
	int clen;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **resultPtr;
{
	Tcl_HashEntry *entry;
	struct Type *type;
	ExtraL_StructlTypeGetProc *cmd;
	int error,i;
	
	entry = Tcl_FindHashEntry(&typesTable, ctag);
	if (entry != NULL) {
		type = (struct Type *)Tcl_GetHashValue(entry);
		cmd = type->getproc;
		if (cmd!=NULL) {
			error = (*cmd)(interp,substructure,data,tagsc,tagsv,resultPtr);
			if (error != TCL_OK) {return error;}
		} else {
			int listlen;
			if (*resultPtr != NULL) {Tcl_GetStringFromObj(*resultPtr, &listlen);} else {listlen = 0;}
			if (listlen == 0) {
				error = Tcl_ListObjLength(interp, substructure, &i);
				if (error != TCL_OK) {return error;}
				error = Tcl_ListObjIndex(interp, substructure, i-1, resultPtr);
				if (error != TCL_OK) {return error;}
			}
			return TCL_OK;
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
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}
		if (data != NULL) {
			error = Tcl_ListObjAppendElement(interp,cmdObj,data);
		} else {
			error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewObj());
		}
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}
		error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewListObj(tagsc,tagsv));
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}
		if (*resultPtr != NULL) {
			error = Tcl_ListObjAppendElement(interp,cmdObj,*resultPtr);
		} else {
			error = Tcl_ListObjAppendElement(interp,cmdObj,Tcl_NewObj());
		}
		if (error != TCL_OK) {Tcl_DecrRefCount(cmdObj);return error;}

		error = Tcl_EvalObj(interp,cmdObj);
		Tcl_DecrRefCount(cmdObj);
		if (error != TCL_OK) {return error;}
		*resultPtr = Tcl_GetObjResult(interp);
		return TCL_OK;
	}
}

int ExtraL_ObjEqual(obj1, obj2)
	Tcl_Obj *obj1;
	Tcl_Obj *obj2;
{
	char *obj1string, *obj2string;
	int obj1len, obj2len;
	if ((obj1==NULL)||(obj2==NULL)) {return 0;}
	obj1string=Tcl_GetStringFromObj(obj1,&obj1len);
	obj2string=Tcl_GetStringFromObj(obj2,&obj2len);
	if ((obj1len == obj2len)&&(memcmp(obj1string,obj2string,obj1len)==0)) {
		return 1;
	} else {
		return 0;
	}
}

int ExtraL_StructlFindTag(interp, list, tag, taglen, resultPtr, posPtr)
	Tcl_Interp *interp;
	Tcl_Obj *list;
	char *tag;
	int taglen;
	Tcl_Obj **resultPtr;
	int *posPtr;
{
	Tcl_Obj **listv, *subtag;
	int listc;
	char *ctag;
	int clen, pos, i, error;

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
		error = Tcl_ListObjIndex(interp, listv[pos], 0, &subtag);
		if (error != TCL_OK) {return error;}
		if (subtag == NULL) {
			ctag = "";
			clen = 0;
		} else {
			ctag = Tcl_GetStringFromObj(subtag,&clen);
		}
		if ((clen==1)&&(ctag[0]=='?')) {
			for (i=1; i<=2; i++) {
				error = Tcl_ListObjIndex(interp, listv[pos], i, &subtag);
				if (error != TCL_OK) {return error;}
				if (subtag == NULL) {
					ctag = "";
					clen = 0;
				} else {
					ctag = Tcl_GetStringFromObj(subtag,&clen);
				}
				if ((clen==taglen)&&(memcmp(ctag,tag,taglen)==0)) {
					pos++;
					*posPtr = pos;
					*resultPtr = listv[pos];
					return TCL_OK;
				}
			}
		} else {
			ctag = Tcl_GetStringFromObj(listv[pos],&clen);
			if ((clen==taglen)&&(memcmp(ctag,tag,taglen)==0)) {
				pos++;
				*posPtr = pos;
				*resultPtr = listv[pos];
				return TCL_OK;
			}
		}
	}
	ctag=Tcl_GetStringFromObj(listv[0],&clen);
	*posPtr = -1;
	*resultPtr = NULL;
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 *		C backend for structlset with struct
 *	 list gets changed directly, so should not be shared
 *
 *----------------------------------------------------------------------
 */

extern int ExtraL_StructlsetStruct(interp, structure, data, list, tagsc, tagsv, value, resultPtr)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *list;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj *value;
	Tcl_Obj **resultPtr;
{
	Tcl_Obj *temp, *tagObj,*result,*res;
	char *ctag,*tag;
	Tcl_Obj *substructure, *sublist, *structtag, *subtag;
	int sublistpos, structpos;
	int clen,len;
	int error;
	int pos;

	result = NULL;
	error = Tcl_ListObjIndex(interp, structure, 0, &temp);
	if (error != TCL_OK) {return error;}
	if (temp != NULL) {
		ctag = Tcl_GetStringFromObj(temp,&clen);
	} else {
		clen = 0;
	}
	if ((clen>1)&&(ctag[0]=='*')&&(ctag[1]!=' ')) {
		/* endnode */
		error = ExtraL_StructlsetValidate(interp,structure,data,ctag,clen,list,tagsc,tagsv,value,&res);
		if (error != TCL_OK) {return error;}
		*resultPtr = res;
		return TCL_OK;
	} else if (tagsc == 0) {
		/*
		# Go further down structure by value
		# ----------------------------------
		*/
		int tempc;
		Tcl_Obj **tempv;
		if (Tcl_ListObjGetElements(interp, value, &tempc, &tempv) != TCL_OK) {
			return TCL_ERROR;
		}
		if (tempc & 1) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: incorrect value trying to assign \"", Tcl_GetStringFromObj(value,&len),"\" to struct \"", Tcl_GetStringFromObj(structure,&len),"\"",(char *)NULL);
			return TCL_ERROR;
		} else if (tempc!=0) {
			if (list == NULL) {
				result = Tcl_NewObj();
			} else {
				result = Tcl_DuplicateObj(list);
			}
			for(pos=0;pos<tempc;pos+=2) {
				tagObj = tempv[pos];
				tag = Tcl_GetStringFromObj(tagObj,&len);
				/* check structure if needed */
				error = ExtraL_StructlFindTag(interp, structure, tag, len, &substructure, &structpos);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return TCL_ERROR;}
				if (structpos == -1) {
					Tcl_ResetResult(interp);
					Tcl_AppendResult(interp,"error: tag \"", tag, "\" not present in structure \"", Tcl_GetStringFromObj(structure,&len),"\"",(char *)NULL);
					Tcl_DecrRefCount(result);
					return TCL_ERROR;	
				}
			
				/* try to find the next tag */
				error = Tcl_ListObjIndex(interp, structure, structpos-1, &structtag);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
				error = Tcl_ListObjIndex(interp, structtag, 0, &subtag);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
				ctag = Tcl_GetStringFromObj(subtag,&clen);
				if ((clen==1)&&(ctag[0]=='?')) {
					error = Tcl_ListObjIndex(interp, structtag, 2, &tagObj);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
					if (tagObj == NULL) {tagObj = Tcl_NewObj();}
					tag = Tcl_GetStringFromObj(tagObj,&len);
				}
				error = ExtraL_StructlFindTag(interp, result, tag, len, &sublist, &sublistpos);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return TCL_ERROR;}
		
				error = ExtraL_StructlsetStruct(interp, substructure, data, sublist, 0, NULL, tempv[pos+1], &res);
				if (error == TCL_ERROR) {
					Tcl_AppendResult(interp," at field \"",tag ,"\"",(char *) NULL);
					Tcl_DecrRefCount(result);return error;
				} else if (error == 5) {
					if (sublistpos != -1) {
						error = Tcl_ListObjLength(interp,result,&len);
						if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
						if (len == 2) {
							Tcl_DecrRefCount(result);
							result = NULL; 
						} else {
							error = Tcl_ListObjReplace(interp,result,sublistpos-1,2,0,NULL);
							if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
						}
					}
				} else {
					if (sublistpos != -1) {
						error = Tcl_ListObjReplace(interp,result,sublistpos,1,1,&res);
						if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
					} else {
						error = Tcl_ListObjAppendElement(interp,result,tagObj);
						if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
						error = Tcl_ListObjAppendElement(interp,result,res);
						if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
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
		error = Tcl_ListObjIndex(interp, structure, structpos-1, &structtag);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structtag, 0, &subtag);
		if (error != TCL_OK) {return error;}
		ctag = Tcl_GetStringFromObj(subtag,&clen);
		if ((clen==1)&&(ctag[0]=='?')) {
			error = Tcl_ListObjIndex(interp, structtag, 2, &tagObj);
			if (error != TCL_OK) {return error;}
			if (tagObj == NULL) {tagObj = Tcl_NewObj();}
			tag = Tcl_GetStringFromObj(tagObj,&len);
		}
		error = ExtraL_StructlFindTag(interp, list, tag, len, &sublist, &sublistpos);
		if (error != TCL_OK) {return TCL_ERROR;}

		error = ExtraL_StructlsetStruct(interp, substructure, data, sublist, tagsc, tagsv, value, &res);
		if (error == TCL_ERROR) {
			Tcl_AppendResult(interp," at field \"",tag ,"\"",(char *) NULL);
			return error;
		} else if (error == 5) {
			sublist = res;
			if (sublistpos != -1) {
				error = Tcl_ListObjLength(interp,list,&len);
				if (error != TCL_OK) {return error;}
				if (len == 2) {
					result = NULL; 
				} else {
					result = Tcl_DuplicateObj(list);
					error = Tcl_ListObjReplace(interp,result,sublistpos-1,2,0,NULL);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
				}
			}
		} else {
			if (list == NULL) {
				result = Tcl_NewObj();
			} else {
				result = Tcl_DuplicateObj(list);
			}
			if (sublistpos != -1) {
				error = Tcl_ListObjReplace(interp,result,sublistpos,1,1,&res);
				if (error != TCL_OK) {
					Tcl_DecrRefCount(result);
					return error;
				}
			} else {
				error = Tcl_ListObjAppendElement(interp,result,tagObj);
				if (error != TCL_OK) {
					Tcl_DecrRefCount(result);
					return error;
				}
				error = Tcl_ListObjAppendElement(interp,result,res);
				if (error != TCL_OK) {
					Tcl_DecrRefCount(result);
					return error;
				}
			}
		}
	}
	if (result == NULL) {
		return 5;
	} else {
		*resultPtr = result;
		return TCL_OK;
	}
}

/*
 *----------------------------------------------------------------------
 *
 *		C backend for structlset
 *	 list gets changed directly, so should not be shared
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
	Tcl_Obj *res,*result;
	char *tag;
	Tcl_Obj *sublist;
	int sublistpos;
	int len,i;
	int error;

	tag = Tcl_GetStringFromObj(tagsv[0],&len);

	/* try to find the next tag */
	error = ExtraL_StructlFindTag(interp, list, tag, len, &sublist, &sublistpos);
	if (error != TCL_OK) {return TCL_ERROR;}

	*resultPtr = NULL;
	result = Tcl_DuplicateObj(list);
	/* change or ad the element */
	if (sublistpos != -1) {
		if (tagsc != 1) {
			error = ExtraL_Structlset(interp, sublist, tagsc-1, tagsv+1, value, &res);
			if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
			error = Tcl_ListObjReplace(interp,result,sublistpos,1,1,&res);
			if (error != TCL_OK) {Tcl_DecrRefCount(result);Tcl_DecrRefCount(res);return error;}
		} else {
			error = Tcl_ListObjReplace(interp,result,sublistpos,1,1,&value);
			if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
		}
	} else if (tagsc == 1) {
		error = Tcl_ListObjAppendElement(interp,result,tagsv[0]);
		if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
		error = Tcl_ListObjAppendElement(interp,result,value);
		if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
	} else {
		res = Tcl_DuplicateObj(tagsv[tagsc-1]);
		error = Tcl_ListObjAppendElement(interp, res, value);
		if (error != TCL_OK) {Tcl_DecrRefCount(result);Tcl_DecrRefCount(res);return error;}

		i = tagsc-2;
		while (1) {
			if (i==0) break;
			sublist = res;
			res = Tcl_DuplicateObj(tagsv[i]);
			error = Tcl_ListObjAppendElement(interp, res, sublist);
			if (error != TCL_OK) {Tcl_DecrRefCount(result);Tcl_DecrRefCount(res);Tcl_DecrRefCount(sublist);return error;}
			i--;
		}

		error = Tcl_ListObjAppendElement(interp,result,tagsv[0]);
		if (error != TCL_OK) {Tcl_DecrRefCount(result);Tcl_DecrRefCount(res);return error;}
		error = Tcl_ListObjAppendElement(interp,result,res);
		if (error != TCL_OK) {Tcl_DecrRefCount(result);Tcl_DecrRefCount(res);return error;}
	}
	*resultPtr = result;
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 *		C backend for structlunset with struct
 *	 list gets changed directly, so should not be shared
 *
 *----------------------------------------------------------------------
 */

extern int ExtraL_StructlunsetStruct(interp, structure, data, list, tagsc, tagsv, resultPtr)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *list;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **resultPtr;
{
	Tcl_Obj *temp, *tagObj,*result,*res;
	char *ctag,*tag;
	Tcl_Obj *substructure, *sublist, *structtag, *subtag;
	int sublistpos, structpos;
	int clen,len;
	int error;

	result = NULL;
	error = Tcl_ListObjIndex(interp, structure, 0, &temp);
	if (error != TCL_OK) {return error;}
	if (temp != NULL) {
		ctag = Tcl_GetStringFromObj(temp,&clen);
	} else {
		clen = 0;
	}
	if ((clen>1)&&(ctag[0]=='*')&&(ctag[1]!=' ')) {
		/* endnode */
		error = ExtraL_StructlunsetValidate(interp,structure,data,ctag,clen,list,tagsc,tagsv,&res);
		if (error != TCL_OK) {return error;}
		*resultPtr = res;
		return TCL_OK;
	} else if (tagsc == 0) {
		*resultPtr = NULL;
		return 5;
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
		error = Tcl_ListObjIndex(interp, structure, structpos-1, &structtag);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structtag, 0, &subtag);
		if (error != TCL_OK) {return error;}
		ctag = Tcl_GetStringFromObj(subtag,&clen);
		if ((clen==1)&&(ctag[0]=='?')) {
			error = Tcl_ListObjIndex(interp, structtag, 2, &tagObj);
			if (error != TCL_OK) {return error;}
			if (tagObj == NULL) {tagObj = Tcl_NewObj();}
			tag = Tcl_GetStringFromObj(tagObj,&len);
		}
		error = ExtraL_StructlFindTag(interp, list, tag, len, &sublist, &sublistpos);
		if (error != TCL_OK) {return TCL_ERROR;}
		if (sublistpos == -1) {
			sublist = NULL;
		}

		error = ExtraL_StructlunsetStruct(interp, substructure, data, sublist, tagsc, tagsv, &res);
		if (error == TCL_ERROR) {
			Tcl_AppendResult(interp," at field \"",tag ,"\"",(char *) NULL);
			return error;
		} else if (error == 5) {
			sublist=temp;
			if (sublistpos != -1) {
				error = Tcl_ListObjLength(interp,list,&len);
				if (error != TCL_OK) {return error;}
				if (len == 2) {
					result = NULL; 
				} else {
					result = Tcl_DuplicateObj(list);
					error = Tcl_ListObjReplace(interp,result,sublistpos-1,2,0,NULL);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
				}
			}
		} else {
			if (list == NULL) {
				result = Tcl_NewObj();
			} else {
				result = Tcl_DuplicateObj(list);
			}
			if (sublistpos != -1) {
				error = Tcl_ListObjReplace(interp,result,sublistpos,1,1,&res);
				if (error != TCL_OK) {
					Tcl_DecrRefCount(result);
					return error;
				}
			} else {
				error = Tcl_ListObjAppendElement(interp,result,tagObj);
				if (error != TCL_OK) {
					Tcl_DecrRefCount(result);
					return error;
				}
				error = Tcl_ListObjAppendElement(interp,result,res);
				if (error != TCL_OK) {
					Tcl_DecrRefCount(result);
					return error;
				}
			}
		}
	}
	if (result == NULL) {
		return 5;
	} else {
		*resultPtr = result;
		return TCL_OK;
	}
}

/*
 *----------------------------------------------------------------------
 *
 *		C backend for structlunset without struct
 *	 list gets changed directly, so should not be shared
 *
 *----------------------------------------------------------------------
 */

extern int ExtraL_Structlunsetnostruct(interp, list, tagsc, tagsv, resultPtr)
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
					result = ExtraL_Structlunsetnostruct(interp, workv[pos], tagsc-1, tagsv+1, &temp);
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

extern int ExtraL_StructlgetStruct(interp, structure, data, list, tagsc, tagsv, resultPtr)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *list;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **resultPtr;
{
	Tcl_Obj *temp, *substructure, *sublist, *res;
	char *ctag,*tag;
	int clen,len;
	int error;
	int pos;

/*
printf("struct: %s\n",Tcl_GetStringFromObj(structure,&error));
if (list != NULL) {printf("list: %s\n",Tcl_GetStringFromObj(list,&error));}
if (tagsc != 0) {printf("tags: %d\ntag: %s\n",tagsc,Tcl_GetStringFromObj(tagsv[0],&error));}
printf("\n");
fflush(stdout);
*/
	Tcl_ListObjIndex(interp, structure, 0, &temp);
	if (temp == NULL) {
		ctag="";
	} else {
		ctag = Tcl_GetStringFromObj(temp,&clen);
	}
	/* 
	# Is this an endnode
	# ------------------
	*/
	if ((clen>1)&&(ctag[0]=='*')&&(ctag[1]!=' ')) {
		if (tagsc>0) {
			error = ExtraL_StructlgetValidate(interp,structure,data,ctag,clen,tagsc,tagsv,&list);
			if (error != TCL_OK) {return TCL_ERROR;}
			*resultPtr = list;
			return TCL_OK;
		} else {
			error = ExtraL_StructlgetValidate(interp,structure,data,ctag,clen,0,NULL,&list);
			if (error != TCL_OK) {return TCL_ERROR;}
			*resultPtr = list;
			return TCL_OK;
		}
	}

	/* 
	# out of tags
	# -----------
	*/
	if (tagsc == 0) {
		Tcl_Obj **tempv;
		int tempc;
		Tcl_Obj *result, *subtag;
		int i;

		error = Tcl_ListObjGetElements(interp, structure, &tempc, &tempv);
		if (error != TCL_OK) {return error;}
		if (tempc & 1) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: structure \"", Tcl_GetStringFromObj(structure,&len),"\" does not have an even number of elements",(char *)NULL);
			return TCL_ERROR;
		} else if (tempc != 0) {
			result = Tcl_NewObj();
			for(pos=0;pos<tempc;pos+=2) {
				error = Tcl_ListObjIndex(interp, tempv[pos], 0, &subtag);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
				ctag = Tcl_GetStringFromObj(subtag,&clen);
				if ((clen==1)&&(ctag[0]=='?')) {
					error = Tcl_ListObjIndex(interp, tempv[pos], 1, &subtag);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
					if (subtag == NULL) {subtag = Tcl_NewObj();}
					error = Tcl_ListObjAppendElement(interp,result,subtag);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
					error = Tcl_ListObjIndex(interp, tempv[pos], 2, &subtag);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
					if (subtag == NULL) {
						ctag = "";
						clen = 0;
					} else {
						ctag = Tcl_GetStringFromObj(subtag,&clen);
					}
					error = ExtraL_StructlFindTag(interp,list,ctag,clen,&sublist,&i);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
					error = ExtraL_StructlgetStruct(interp, tempv[pos+1], data, sublist, 0, NULL, &res);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
					error = Tcl_ListObjAppendElement(interp,result,res);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
				} else {
					ctag = Tcl_GetStringFromObj(tempv[pos],&clen);
					error = Tcl_ListObjAppendElement(interp,result,tempv[pos]);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
					error = ExtraL_StructlFindTag(interp,list,ctag,clen,&sublist,&i);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
					error = ExtraL_StructlgetStruct(interp, tempv[pos+1], data, sublist, 0, NULL, &res);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
					error = Tcl_ListObjAppendElement(interp,result,res);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
				}
			}
			*resultPtr = result;
			return TCL_OK;
		} else {
			*resultPtr = list;
			return TCL_OK;
		}
	} else {
		Tcl_Obj *structtag, *subtag;
		/* 
		# find substructure corresponding to tag 
		# --------------------------------------
		*/

		tag = Tcl_GetStringFromObj(tagsv[0],&len);
		error = ExtraL_StructlFindTag(interp,structure,tag,len,&substructure,&pos);
		if (error != TCL_OK) {return error;}
		if (pos == -1) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: tag \"", tag, "\" not present in structure \"", Tcl_GetStringFromObj(structure,&len),"\"",(char *)NULL);
			return TCL_ERROR;	
		}
		error = Tcl_ListObjIndex(interp, structure, pos-1, &structtag);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structtag, 0, &subtag);
		if (error != TCL_OK) {return error;}
		ctag = Tcl_GetStringFromObj(subtag,&clen);
		if ((clen==1)&&(ctag[0]=='?')) {
			error = Tcl_ListObjIndex(interp, structtag, 2, &subtag);
			if (error != TCL_OK) {return error;}
			if (subtag == NULL) {subtag = Tcl_NewObj();}
			tag = Tcl_GetStringFromObj(subtag,&len);
		}
	
		/* 
			find the tag
			------------
		*/
		error = ExtraL_StructlFindTag(interp,list,tag,len,&sublist,&pos);
		if (error != TCL_OK) {return error;}
	
		/* set the result */
		error = ExtraL_StructlgetStruct(interp, substructure, data, sublist, tagsc-1, tagsv+1, &res);
		if (error != TCL_OK) {return error;}
		*resultPtr = res;
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

extern int ExtraL_Structlget(interp, list, tags, resultPtr)
	Tcl_Interp *interp;
	Tcl_Obj *list;
	Tcl_Obj *tags;
	Tcl_Obj **resultPtr;
{
	int listc;
	Tcl_Obj **listv,*result;
	int tagsc;
	Tcl_Obj **tagsv;
	char *ctag,*tag;
	int clen,len,curtag;
	int pos;

	if (Tcl_ListObjGetElements(interp, tags, &tagsc, &tagsv) != TCL_OK) {
		return TCL_ERROR;
	}
	result = list;
	for(curtag=0;curtag<tagsc;curtag++) {
		if (Tcl_ListObjGetElements(interp, result, &listc, &listv) != TCL_OK) {
			return TCL_ERROR;
		}
		if ((listc != 0)&&(listc & 1)) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: list \"", Tcl_GetStringFromObj(result,&len),"\" does not have an even number of elements",(char *)NULL);
			return TCL_ERROR;
		}
		tag = Tcl_GetStringFromObj(tagsv[curtag],&len);
		for(pos = 0; pos < listc; pos += 2) {
			ctag=Tcl_GetStringFromObj(listv[pos],&clen);
			if (clen == len) {
				if (memcmp(ctag,tag,len) == 0) {
					result = listv[++pos];
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
	*resultPtr = result;
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

int ExtraL_StructlsetObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *current, *res;
	Tcl_Obj **tagsv;
	int tagsc;
	Tcl_Obj *structure, *data;
	char *string;
	int pos, error, i;

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-struct schema? ?-data clientdata? list field value ?field value ...?");
		return TCL_ERROR;
	}
	structure = NULL;
	data = NULL;
	pos = 1;
	while(pos < objc) {
		string = Tcl_GetStringFromObj(objv[pos], &i);
		if ((i==7)&&(strncmp(string,"-struct",7) == 0)) {
			structure = objv[pos+1];
			pos += 2;
		} else if ((i==5)&&(strncmp(string,"-data",5) == 0)) {
			data = objv[pos+1];
			pos += 2;
		} else {
			break;
		}
	}

	if ((objc < (pos+3))||(objc & 1)) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-struct schema? ?-data clientdata? list field value ?field value ...?");
		return TCL_ERROR;
	}

	current = objv[pos];
	Tcl_IncrRefCount(current);
	for(i = pos+1 ; i < objc ; i+=2) {
		error = Tcl_ListObjGetElements(interp, objv[i], &tagsc, &tagsv);
		if (error != TCL_OK)	{Tcl_DecrRefCount(current);return error;}
		if (structure == NULL) {
			if (tagsc == 0) {
				Tcl_DecrRefCount(current);
				current = objv[i+1];
				Tcl_IncrRefCount(current);
			} else {
				error = ExtraL_Structlset(interp, current, tagsc, tagsv, objv[i+1],&res);
				Tcl_DecrRefCount(current);
				if (error != TCL_OK)	{return error;}
				current = res;
			}
		} else {
			error = ExtraL_StructlsetStruct(interp, structure, data, current, tagsc, tagsv, objv[i+1],&res);
			Tcl_DecrRefCount(current);
			if (error == TCL_ERROR)	{
				return error;
			} else if (error == 5) {
				current = Tcl_NewObj();
			} else {
				current = res;
			}
		}
	}
	Tcl_SetObjResult(interp,current);
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

int ExtraL_StructlunsetObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *current,*res;
	Tcl_Obj **tagsv;
	int tagsc;
	Tcl_Obj *structure, *data;
	char *string;
	int pos, error, i;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-struct schema? ?-data clientdata? list field ?field ...?");
		return TCL_ERROR;
	}
	structure = NULL;
	data = NULL;
	pos = 1;
	while(pos < objc) {
		string = Tcl_GetStringFromObj(objv[pos], &i);
		if ((i==7)&&(strncmp(string,"-struct",7) == 0)) {
			structure = objv[pos+1];
			pos += 2;
		} else if ((i==5)&&(strncmp(string,"-data",5) == 0)) {
			data = objv[pos+1];
			pos += 2;
		} else {
			break;
		}
	}

	if ((objc-pos) < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-struct schema? ?-data clientdata? list field ?field ...?");
		return TCL_ERROR;
	}


	current = objv[pos];
	Tcl_IncrRefCount(current);
	for(i = pos+1 ; i < objc ; i++) {
		error = Tcl_ListObjGetElements(interp, objv[i], &tagsc, &tagsv);
		if (error != TCL_OK)	{return error;}
		if (structure == NULL) {
			if (tagsc == 0) {
				Tcl_DecrRefCount(current);
				current = objv[i+1];
				Tcl_IncrRefCount(current);
			} else {
				error = ExtraL_Structlunsetnostruct(interp, current, tagsc, tagsv, &res);
				Tcl_DecrRefCount(current);
				if (error != TCL_OK)	{return error;}
				current = res;
			}
		} else {
			error = ExtraL_StructlunsetStruct(interp, structure, data, current, tagsc, tagsv, &res);
			Tcl_DecrRefCount(current);
			if (error == TCL_ERROR)	{
				return error;
			} else if (error == 5) {
				current = Tcl_NewObj();
			} else {
				current = res;
			}
		}
	}
	Tcl_SetObjResult(interp,current);
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

int ExtraL_StructlgetObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
{
	Tcl_Obj **tagsv;
	int tagsc;
	Tcl_Obj *result;
	Tcl_Obj *structure, *data;
	char *string;
	int i, error;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-struct schema? ?-data clientdata? list field ?field ...?");
		return TCL_ERROR;
	}
	structure = NULL;
	data = NULL;
	objc -= 1;
	objv += 1;
	while(objc > 0) {
		string = Tcl_GetStringFromObj(objv[0], &i);
		if ((i==7)&&(strncmp(string,"-struct",7) == 0)) {
			structure = objv[1];
			objc -= 2;
			objv += 2;
		} else if ((i==5)&&(strncmp(string,"-data",5) == 0)) {
			data = objv[1];
			objc -= 2;
			objv += 2;
		} else {
			break;
		}
	}
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "?-struct schema? ?-data clientdata? list field ?field ...?");
		return TCL_ERROR;
	}
	if (objc == 2) {
		if (structure != NULL) {
			error = Tcl_ListObjGetElements(interp, objv[1], &tagsc, &tagsv);
			if (error != TCL_OK) {return TCL_ERROR;}
			error = ExtraL_StructlgetStruct(interp, structure, data, objv[0], tagsc, tagsv, &result);
		} else {
			error = ExtraL_Structlget(interp, objv[0], objv[1], &result);
		}
		if (error != TCL_OK) {return TCL_ERROR;}
	} else {
		Tcl_Obj *res;
		result = Tcl_NewObj();
		if (structure != NULL) {
			for(i=1;i<objc;i++) {
				error = Tcl_ListObjGetElements(interp, objv[i], &tagsc, &tagsv);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return TCL_ERROR;}
				error = ExtraL_StructlgetStruct(interp, structure, data, objv[0], tagsc, tagsv, &res);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return TCL_ERROR;}
				error = Tcl_ListObjAppendElement(interp, result, res);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return TCL_ERROR;}
			}
		} else {
			for(i=1;i<objc;i++) {
				error = ExtraL_Structlget(interp, objv[0], objv[i], &res);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return TCL_ERROR;}
				error = Tcl_ListObjAppendElement(interp, result, res);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return TCL_ERROR;}
			}
		}
	}
	Tcl_SetObjResult(interp,result);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_StructlfieldsObjCmd --
 *
 *		This procedure is invoked to process the "structlfields" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int ExtraL_StructlfieldsObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listArgc;
	Tcl_Obj **listArgv, *resultObj, *valueObj, *tags, *subtag, *list;
	int pos, error, i, clen;
	char *ctag;

	if ((objc != 2)&&(objc != 3)&&(objc != 4)) {
		Tcl_WrongNumArgs(interp, 1, objv, "list field ?valueVar?");
		return TCL_ERROR;
	}
	list = objv[1];
	if (objc==2) {
		tags = Tcl_NewObj();
		error = ExtraL_Structlget(interp, list, tags, &list);
		Tcl_DecrRefCount(tags);
	} else {
		tags = objv[2];
		error = ExtraL_Structlget(interp, list, tags, &list);
	}

	if (error != TCL_OK) {return error;}
	if (Tcl_ListObjGetElements(interp, list, &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}
	if ((listArgc != 0)&&(listArgc & 1)) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: list \"", Tcl_GetStringFromObj(objv[1],&i),"\" does not have an even number of elements",(char *)NULL);
		return TCL_ERROR;
	}



	/* Initialise result */
	Tcl_ResetResult(interp);
	resultObj = Tcl_NewObj();

	for(pos=0;pos<listArgc;pos+=2) {
		error = Tcl_ListObjIndex(interp, listArgv[pos], 0, &subtag);
		if (error != TCL_OK) {return error;}
		ctag = Tcl_GetStringFromObj(subtag,&clen);
		if ((clen==1)&&(ctag[0]=='?')) {
			error = Tcl_ListObjIndex(interp, listArgv[pos], 1, &subtag);
			if (subtag == NULL) {subtag = Tcl_NewObj();}
			if (error != TCL_OK) {return error;}
			error = Tcl_ListObjAppendElement(interp,resultObj,subtag);
		} else {
			error = Tcl_ListObjAppendElement(interp,resultObj,listArgv[pos]);
		}
		if (error!=TCL_OK) {return error;}
	}
	if (objc == 4) {
		valueObj = Tcl_NewObj();
		for(pos=1;pos<listArgc;pos+=2) {
			error = Tcl_ListObjAppendElement(interp,valueObj,listArgv[pos]);
			if (error!=TCL_OK) {return error;}
		}
		if (Tcl_ObjSetVar2(interp, objv[3], (Tcl_Obj *) NULL,
			valueObj, (TCL_LEAVE_ERR_MSG | TCL_PARSE_PART1)) == NULL) {
				return TCL_ERROR;
		}
	}
	Tcl_SetObjResult(interp,resultObj);
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

int ExtraL_StructlfindObjCmd(notUsed, interp, objc, objv)
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
	Tcl_CreateObjCommand(interp,"structlset",(Tcl_ObjCmdProc *)ExtraL_StructlsetObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"structlunset",(Tcl_ObjCmdProc *)ExtraL_StructlunsetObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"structlget",(Tcl_ObjCmdProc *)ExtraL_StructlgetObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"structlfields",(Tcl_ObjCmdProc *)ExtraL_StructlfieldsObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"structlfind",(Tcl_ObjCmdProc *)ExtraL_StructlfindObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	ExtraL_StructlCreateType(interp,"*string",(ExtraL_StructlTypeSetProc *)NULL,(ExtraL_StructlTypeGetProc *)NULL,(ExtraL_StructlTypeUnsetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*text",(ExtraL_StructlTypeSetProc *)NULL,(ExtraL_StructlTypeGetProc *)NULL,(ExtraL_StructlTypeUnsetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*any",(ExtraL_StructlTypeSetProc *)NULL,(ExtraL_StructlTypeGetProc *)NULL,(ExtraL_StructlTypeUnsetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*int",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetInt,(ExtraL_StructlTypeGetProc *)NULL,(ExtraL_StructlTypeUnsetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*double",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetDouble,(ExtraL_StructlTypeGetProc *)NULL,(ExtraL_StructlTypeUnsetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*bool",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetBool,(ExtraL_StructlTypeGetProc *)NULL,(ExtraL_StructlTypeUnsetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*regexp",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetRegexp,(ExtraL_StructlTypeGetProc *)NULL,(ExtraL_StructlTypeUnsetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*between",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetBetween,(ExtraL_StructlTypeGetProc *)NULL,(ExtraL_StructlTypeUnsetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*dbetween",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetDBetween,(ExtraL_StructlTypeGetProc *)NULL,(ExtraL_StructlTypeUnsetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*date",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetDate,(ExtraL_StructlTypeGetProc *)ExtraL_StructlGetDate,(ExtraL_StructlTypeUnsetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*time",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetTime,(ExtraL_StructlTypeGetProc *)ExtraL_StructlGetTime,(ExtraL_StructlTypeUnsetProc *)NULL);
	ExtraL_StructlCreateType(interp,"*list",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetList,(ExtraL_StructlTypeGetProc *)ExtraL_StructlGetList,(ExtraL_StructlTypeUnsetProc *)ExtraL_StructlUnsetList);
	ExtraL_StructlCreateType(interp,"*named",(ExtraL_StructlTypeSetProc *)ExtraL_StructlSetNamed,(ExtraL_StructlTypeGetProc *)ExtraL_StructlGetNamed,(ExtraL_StructlTypeUnsetProc *)ExtraL_StructlUnsetNamed);
	return TCL_OK;
}

