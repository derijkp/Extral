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

int ExtraL_StructlSetInt(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	int temp, error;
	if (tagsc > 0) {
		int temp;
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: field \"", Tcl_GetStringFromObj(tagsv[0],&temp),
		"\" not present in structure \"", Tcl_GetStringFromObj(structure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	{
		Tcl_Obj *temp;
		int pos;
		error = Tcl_ListObjLength(interp,structure, &pos);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structure, pos-1, &temp);
		if (error != TCL_OK) {return error;}
		if (ExtraL_ObjEqual(temp,*value)==1) {
			return 5;
		}
	}

	error = Tcl_GetIntFromObj(interp,*value,&temp);
	if (error != TCL_OK) {return error;}
	return TCL_OK;
}

int ExtraL_StructlSetDouble(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	double temp;
	int error;
	if (tagsc > 0) {
		int temp;
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: field \"", Tcl_GetStringFromObj(tagsv[0],&temp),
		"\" not present in structure \"", Tcl_GetStringFromObj(structure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	{
		Tcl_Obj *temp;
		int pos;
		error = Tcl_ListObjLength(interp,structure, &pos);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structure, pos-1, &temp);
		if (error != TCL_OK) {return error;}
		if (ExtraL_ObjEqual(temp,*value)==1) {
			return 5;
		}
	}

	error = Tcl_GetDoubleFromObj(interp,*value,&temp);
	if (error != TCL_OK) {return error;}
	return TCL_OK;
}

int ExtraL_StructlSetBool(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	int temp, error;
	if (tagsc > 0) {
		int temp;
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: field \"", Tcl_GetStringFromObj(tagsv[0],&temp),
		"\" not present in structure \"", Tcl_GetStringFromObj(structure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	{
		Tcl_Obj *temp;
		int pos;
		error = Tcl_ListObjLength(interp,structure, &pos);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structure, pos-1, &temp);
		if (error != TCL_OK) {return error;}
		if (ExtraL_ObjEqual(temp,*value)==1) {
			return 5;
		}
	}

	error = Tcl_GetBooleanFromObj(interp,*value,&temp);
	if (error != TCL_OK) {return error;}
	if Tcl_IsShared(*value) {
		*value = Tcl_DuplicateObj(*value);
	}
	Tcl_SetBooleanObj(*value,temp);
	return TCL_OK;
}

int ExtraL_StructlSetRegexp(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	int temp,error;
	Tcl_Obj *patternObj;
	char *string, *pattern;
	if (tagsc > 0) {
		int temp;
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: field \"", Tcl_GetStringFromObj(tagsv[0],&temp),
		"\" not present in structure \"", Tcl_GetStringFromObj(structure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	{
		Tcl_Obj *temp;
		int pos;
		error = Tcl_ListObjLength(interp,structure, &pos);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structure, pos-1, &temp);
		if (error != TCL_OK) {return error;}
		if (ExtraL_ObjEqual(temp,*value)==1) {
			return 5;
		}
	}

	error = Tcl_ListObjLength(interp,structure,&temp);
	if (error != TCL_OK) {return error;}
	if (temp != 4) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: wrong number of arguments in structure \"", Tcl_GetStringFromObj(structure,&temp), "\": should be \"*regexp pattern errormsg default\"",(char *)NULL);
		return TCL_ERROR;
	}
	error = Tcl_ListObjIndex(interp, structure, 1, &patternObj);
	if (patternObj == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error in structure \"",Tcl_GetStringFromObj(structure,NULL),"\"",(char *)NULL);
		return TCL_ERROR;
	}
	if (error != TCL_OK) {return error;}
	pattern = Tcl_GetStringFromObj(patternObj,&temp);
	string = Tcl_GetStringFromObj(*value,&temp);
	error = Tcl_RegExpMatch(interp, string, pattern);
	if (error == -1) {
		return TCL_ERROR;
	} else if (error == 0) {
		Tcl_ResetResult(interp);
		error = Tcl_ListObjIndex(interp, structure, 2, &patternObj);
		if (error != TCL_OK) {return error;}
		pattern = Tcl_GetStringFromObj(patternObj,&temp);
		Tcl_AppendResult(interp,"error: \"", string,"\" ", pattern, (char *)NULL);
		return TCL_ERROR;
	}
	return TCL_OK;
}

int ExtraL_StructlSetBetween(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	int temp,error;
	Tcl_Obj *startObj, *endObj;
	int start,end;
	if (tagsc > 0) {
		int temp;
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: field \"", Tcl_GetStringFromObj(tagsv[0],&temp),
		"\" not present in structure \"", Tcl_GetStringFromObj(structure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	{
		Tcl_Obj *temp;
		int pos;
		error = Tcl_ListObjLength(interp,structure, &pos);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structure, pos-1, &temp);
		if (error != TCL_OK) {return error;}
		if (ExtraL_ObjEqual(temp,*value)==1) {
			return 5;
		}
	}

	error = Tcl_ListObjLength(interp,structure,&temp);
	if (error != TCL_OK) {return error;}
	if (temp != 4) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: wrong number of arguments in structure \"", Tcl_GetStringFromObj(structure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	error = Tcl_GetIntFromObj(interp,*value,&temp);
	if (error != TCL_OK) {return error;}

	error = Tcl_ListObjIndex(interp, structure, 1, &startObj);
	if (error != TCL_OK) {return error;}
	error = Tcl_GetIntFromObj(interp,startObj,&start);
	if (error != TCL_OK) {return error;}

	error = Tcl_ListObjIndex(interp, structure, 2, &endObj);
	if (error != TCL_OK) {return error;}
	error = Tcl_GetIntFromObj(interp,endObj,&end);
	if (error != TCL_OK) {return error;}
	if ((temp<start)||(temp>end)) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: ", Tcl_GetStringFromObj(*value,&temp)," is not between ",
			Tcl_GetStringFromObj(startObj,&temp), " and ", Tcl_GetStringFromObj(endObj,&temp), (char *)NULL);
		return TCL_ERROR;
	}
	return TCL_OK;

}

int ExtraL_StructlSetDBetween(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	int temp,error;
	Tcl_Obj *startObj, *endObj;
	double start, end, val;
	if (tagsc > 0) {
		int temp;
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: field \"", Tcl_GetStringFromObj(tagsv[0],&temp),
		"\" not present in structure \"", Tcl_GetStringFromObj(structure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	{
		Tcl_Obj *temp;
		int pos;
		error = Tcl_ListObjLength(interp,structure, &pos);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structure, pos-1, &temp);
		if (error != TCL_OK) {return error;}
		if (ExtraL_ObjEqual(temp,*value)==1) {
			return 5;
		}
	}

	error = Tcl_ListObjLength(interp,structure,&temp);
	if (error != TCL_OK) {return error;}
	if (temp != 4) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: wrong number of arguments in structure \"", Tcl_GetStringFromObj(structure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	error = Tcl_GetDoubleFromObj(interp,*value,&val);
	if (error != TCL_OK) {return error;}

	error = Tcl_ListObjIndex(interp, structure, 1, &startObj);
	if (error != TCL_OK) {return error;}
	error = Tcl_GetDoubleFromObj(interp,startObj,&start);
	if (error != TCL_OK) {return error;}

	error = Tcl_ListObjIndex(interp, structure, 2, &endObj);
	if (error != TCL_OK) {return error;}
	error = Tcl_GetDoubleFromObj(interp,endObj,&end);
	if (error != TCL_OK) {return error;}
	if ((val<start)||(val>end)) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: ", Tcl_GetStringFromObj(*value,&temp)," is not between ",
			Tcl_GetStringFromObj(startObj,&temp), " and ", Tcl_GetStringFromObj(endObj,&temp), (char *)NULL);
		return TCL_ERROR;
	}
	return TCL_OK;
}

int ExtraL_StructlSetDate(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	double result;
	int error;
	if (tagsc > 0) {
		int temp;
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: field \"", Tcl_GetStringFromObj(tagsv[0],&temp),
		"\" not present in structure \"", Tcl_GetStringFromObj(structure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	{
		Tcl_Obj *temp;
		int pos;
		error = Tcl_ListObjLength(interp,structure, &pos);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structure, pos-1, &temp);
		if (error != TCL_OK) {return error;}
		if (ExtraL_ObjEqual(temp,*value)==1) {
			return 5;
		}
	}

	error = ExtraL_ScanTime(interp,1,0,*value,&result);
	if (error != TCL_OK) {return error;}
	*value = Tcl_NewDoubleObj(result);
	return TCL_OK;
}

int ExtraL_StructlGetDate(interp,structure,data,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	double time;
	char *result;
	int error;
	int len;

	if (*value == NULL) {
		len = 0;
	} else {
		Tcl_GetStringFromObj(*value,&len);
	}
	if (len == 0) {
		error = Tcl_ListObjLength(interp, structure, &len);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp,structure,len-1,value);
		if (error != TCL_OK) {return error;}
		return TCL_OK;
	}
	error = Tcl_GetDoubleFromObj(interp,*value,&time);
	if (error != TCL_OK) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"time should be a double", (char *)NULL);
		return TCL_ERROR;
	}
	if (tagsc > 0) {
		char *string = Tcl_GetStringFromObj(tagsv[0],NULL);
		if (strcmp(string,"val") != 0) {
			error = ExtraL_FormatTime(interp,time,string,&result);
			if (error != TCL_OK) {return error;}
			*value = Tcl_NewStringObj(result,strlen(result));
			Tcl_Free(result);
		}
	} else {
		error = ExtraL_FormatTime(interp,time,"%e %b %Y",&result);
		if (error != TCL_OK) {return error;}
		*value = Tcl_NewStringObj(result,strlen(result));
		Tcl_Free(result);
	}
	return TCL_OK;
}

int ExtraL_StructlSetTime(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	double result;
	int error;
	if (tagsc > 0) {
		int temp;
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: field \"", Tcl_GetStringFromObj(tagsv[0],&temp),
		"\" not present in structure \"", Tcl_GetStringFromObj(structure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	{
		Tcl_Obj *temp;
		int pos;
		error = Tcl_ListObjLength(interp,structure, &pos);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp, structure, pos-1, &temp);
		if (error != TCL_OK) {return error;}
		if (ExtraL_ObjEqual(temp,*value)==1) {
			return 5;
		}
	}

	error = ExtraL_ScanTime(interp,0,0,*value,&result);
	if (error != TCL_OK) {return error;}
	*value = Tcl_NewDoubleObj(result);
	return TCL_OK;
}

int ExtraL_StructlGetTime(interp,structure,data,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	double time;
	char *result;
	int error;
	int len;

	if (*value == NULL) {
		len = 0;
	} else {
		Tcl_GetStringFromObj(*value,&len);
	}
	if (len == 0) {
		error = Tcl_ListObjLength(interp, structure, &len);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjIndex(interp,structure,len-1,value);
		if (error != TCL_OK) {return error;}
		return TCL_OK;
	}
	error = Tcl_GetDoubleFromObj(interp,*value,&time);
	if (error != TCL_OK) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"time should be a double", (char *)NULL);
		return TCL_ERROR;
	}
	if (tagsc > 0) {
		char *string = Tcl_GetStringFromObj(tagsv[0],NULL);
		if (strcmp(string,"val") != 0) {
			error = ExtraL_FormatTime(interp,time,string,&result);
			if (error != TCL_OK) {return error;}
			*value = Tcl_NewStringObj(result,strlen(result));
			Tcl_Free(result);
		}
	} else {
		error = ExtraL_FormatTime(interp,time,"%e %b %Y %H:%M:%S",&result);
		if (error != TCL_OK) {return error;}
		*value = Tcl_NewStringObj(result,strlen(result));
		Tcl_Free(result);
	}
	return TCL_OK;
}

int ExtraL_StructlSetList(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	Tcl_Obj *struc, *result, *res, *tagObj;
	Tcl_Obj **listv;
	int listc,len;
	int error;
	int taglen;
	int i;
	char *tag;

	error = Tcl_ListObjLength(interp,structure,&len);
	if (error != TCL_OK) {return error;}
	if (len != 2) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: wrong number of arguments in structure \"", Tcl_GetStringFromObj(structure,NULL), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	if (tagsc == 0) {
		tag = "";
		taglen = 0;
		tagObj = NULL;
	} else {
		tagObj = tagsv[0];
		tag = Tcl_GetStringFromObj(tagsv[0],&taglen);
		tagsc--;
		tagsv++;
	}
	error = Tcl_ListObjIndex(interp, structure, 1, &struc);
	if (error != TCL_OK) {return error;}
	if (taglen==0) {
		Tcl_Obj **oldv;
		int oldc;
		error = Tcl_ListObjGetElements(interp, *value, &listc, &listv);
		if (error != TCL_OK) {return error;}
		if (oldvalue != NULL) {
			error = Tcl_ListObjGetElements(interp, oldvalue, &oldc, &oldv);
			if (error != TCL_OK) {return error;}
		} else {
			oldc = 0;
		}
		result = Tcl_NewObj();
		i=0;
		while(i<listc) {
			if (i<oldc) {
				error = ExtraL_StructlsetStruct(interp,struc,data,oldv[i],tagsc,tagsv,listv[i],&res);
			} else {
				error = ExtraL_StructlsetStruct(interp,struc,data,NULL,tagsc,tagsv,listv[i],&res);
			}
			if (error == 5) {
				Tcl_ListObjAppendElement(interp,result,Tcl_NewObj());
			} else if (error != TCL_OK) {
				Tcl_DecrRefCount(result);
				return TCL_ERROR;
			} else {
				Tcl_ListObjAppendElement(interp,result,res);
			}
			i++;
		}
		*value = result;
		return TCL_OK;
	} else if (strncmp(tag,"next",taglen)==0) {
		error = ExtraL_StructlsetStruct(interp,struc,data,NULL,tagsc,tagsv,*value,&res);
		if (error == 5) {
			res = Tcl_NewObj();
		} else if (error != TCL_OK) {
			return TCL_ERROR;
		}
		if (oldvalue == NULL) {
			result = Tcl_NewObj();
		} else {
			result = Tcl_DuplicateObj(oldvalue);
		}
		error = Tcl_ListObjAppendElement(interp,result,res);
		if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
		*value = result;
		return TCL_OK;
	} else {
		Tcl_Obj **tagv, *oldval;
		int tagc,pos,len;
		if (tagObj == NULL) {
			tagObj = Tcl_NewObj();
			error = Tcl_ListObjGetElements(interp, tagObj, &tagc, &tagv);
			Tcl_DecrRefCount(tagObj);
		} else {
			error = Tcl_ListObjGetElements(interp, tagObj, &tagc, &tagv);
		}
		if (error != TCL_OK) {return error;}
		if (tagc==1) {
			if (oldvalue == NULL) {
				len = 0;
			} else {
				error = Tcl_ListObjLength(interp,oldvalue,&len);
			}
			if (len == 0) {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"empty list",(char *)NULL);
				return TCL_ERROR;
			}
			if (strcmp(tag,"end") == 0) {
				pos = len-1;
				if (error != TCL_OK) {return error;}
			} else {
				error = Tcl_GetIntFromObj(interp, tagv[0], &pos);
				if (pos>=len) {
					Tcl_ResetResult(interp);
					Tcl_AppendResult(interp,"list doesn't contain element ",
						Tcl_GetStringFromObj(tagv[0],NULL),(char *)NULL);
					return TCL_ERROR;
				}
				if (error != TCL_OK) {return error;}
			}
			error = Tcl_ListObjIndex(interp, oldvalue, pos, &oldval);
			if (error != TCL_OK) {return error;}
			error = ExtraL_StructlsetStruct(interp,struc,data,oldval,tagsc,tagsv,*value,&res);
			if (error == 5) {
				res = Tcl_NewObj();
			} else if (error != TCL_OK) {
				return TCL_ERROR;
			}
			result = Tcl_DuplicateObj(oldvalue);
			error = Tcl_ListObjReplace(interp,result,pos,1,1,&res);
			if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
			*value = result;
			return TCL_OK;
		} else {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"invalid argument to list",(char *)NULL);
			return TCL_ERROR;
		}
	}
}

int ExtraL_StructlUnsetList(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	Tcl_Obj *struc, *result, *res, *tagObj;
	int error,len;
	int taglen;
	char *tag;

	if (oldvalue == NULL) {
		*value = NULL;
		return 5;
	}
	if (tagsc == 0) {
		*value = NULL;
		return 5;
	} else {
		tagObj = tagsv[0];
		tag = Tcl_GetStringFromObj(tagsv[0],&taglen);
		tagsc--;
		tagsv++;
	}
	error = Tcl_ListObjLength(interp,structure,&len);
	if (error != TCL_OK) {return error;}
	if (len != 2) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: wrong number of arguments in structure \"", Tcl_GetStringFromObj(structure,NULL), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	error = Tcl_ListObjIndex(interp, structure, 1, &struc);
	if (error != TCL_OK) {return error;}
	if (tagsc != 0) {
		if (taglen == 0) {
			Tcl_Obj **oldv;
			int oldc,i;
			error = Tcl_ListObjGetElements(interp, oldvalue, &oldc, &oldv);
			if (error != TCL_OK) {return error;}
			result = Tcl_NewObj();
			i=0;
			while(i<oldc) {
				error = ExtraL_StructlunsetStruct(interp,struc,data,oldv[i],tagsc,tagsv,&res);
				if (error == 5) {
					Tcl_ListObjAppendElement(interp,result,Tcl_NewObj());
				} else if (error != TCL_OK) {
					Tcl_DecrRefCount(result);
					return TCL_ERROR;
				} else {
					Tcl_ListObjAppendElement(interp,result,res);
				}
				i++;
			}
			*value = result;
			return TCL_OK;
		} else {
			Tcl_Obj **tagv, *oldval;
			int tagc,pos,len;
			error = Tcl_ListObjGetElements(interp, tagObj, &tagc, &tagv);
			if (error != TCL_OK) {return error;}
			if (tagc==1) {
				error = Tcl_ListObjLength(interp,oldvalue,&len);
				if (error != TCL_OK) {return error;}
				if (strcmp(tag,"end") == 0) {
					pos = len-1;
				} else {
					error = Tcl_GetIntFromObj(interp, tagv[0], &pos);
					if (error != TCL_OK) {return error;}
					if (pos>=len) {
						*value = oldvalue;
						return TCL_OK;
					}
				}
				error = Tcl_ListObjIndex(interp, oldvalue, pos, &oldval);
				if (error != TCL_OK) {return error;}
				error = ExtraL_StructlunsetStruct(interp,struc,data,oldval,tagsc,tagsv,&res);
				if (error == TCL_ERROR) {
					return TCL_ERROR;
				} else if (error == 5) {
					*value = Tcl_DuplicateObj(oldvalue);
					res = Tcl_NewObj();
					error = Tcl_ListObjReplace(interp,*value,pos,1,1,&res);
					if (error != TCL_OK) {Tcl_DecrRefCount(*value);}
					return error;
				} else {
					*value = Tcl_DuplicateObj(oldvalue);
					error = Tcl_ListObjReplace(interp,*value,pos,1,1,&res);
					if (error != TCL_OK) {Tcl_DecrRefCount(*value);}
					return error;
				}
			} else {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"wrong # args to list",(char *)NULL);
				*value = NULL;
				return TCL_ERROR;
			}
		}
	} else {
		if (taglen==0) {
			*value = NULL;
			return 5;
		} else {
			Tcl_Obj **tagv;
			int tagc,pos,len;
			error = Tcl_ListObjGetElements(interp, tagObj, &tagc, &tagv);
			if (error != TCL_OK) {return error;}
			if (tagc==1) {
				error = Tcl_ListObjLength(interp,oldvalue,&len);
				if (error != TCL_OK) {return error;}
				if (strcmp(tag,"end") == 0) {
					pos = len-1;
				} else {
					error = Tcl_GetIntFromObj(interp, tagv[0], &pos);
					if (error != TCL_OK) {return error;}
					if (pos>=len) {
						Tcl_ResetResult(interp);
						Tcl_AppendResult(interp,"list doesn't contain element ",
							Tcl_GetStringFromObj(tagv[0],NULL),(char *)NULL);
						return TCL_ERROR;
					}
				}
				if (len==1) {
					*value = NULL;
					return 5;
				} else {
					*value = Tcl_DuplicateObj(oldvalue);
					error = Tcl_ListObjReplace(interp,*value,pos,1,0,NULL);
					if (error != TCL_OK) {Tcl_DecrRefCount(*value);}
					return error;
				}
			} else {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"wrong # args to list",(char *)NULL);
				*value = NULL;
				return TCL_ERROR;
			}
		}
	}
	*value = result;
	return TCL_OK;
}

int ExtraL_StructlGetList(interp,structure,data,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	Tcl_Obj *struc, *result, *res, *tagObj;
	Tcl_Obj **listv;
	int listc,len;
	int error;
	int taglen;
	int i;
	char *tag;

	error = Tcl_ListObjLength(interp,structure,&len);
	if (error != TCL_OK) {return error;}
	if (len != 2) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: wrong number of arguments in structure \"", Tcl_GetStringFromObj(structure,NULL), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	if (*value == NULL) {
		*value = Tcl_NewObj();
		return TCL_OK;
	}
	if (tagsc == 0) {
		tag = "";
		taglen = 0;
		tagObj = NULL;
		tagsv = NULL;
	} else {
		tagObj = tagsv[0];
		tag = Tcl_GetStringFromObj(tagsv[0],&taglen);
		tagsc--;
		tagsv++;
		if (tagsc == 0) {tagsv = NULL;}
	}
	error = Tcl_ListObjIndex(interp, structure, 1, &struc);
	if (error != TCL_OK) {return error;}
	if (taglen==0) {
		error = Tcl_ListObjGetElements(interp, *value, &listc, &listv);
		if (error != TCL_OK) {return error;}
		result = Tcl_NewObj();
		i=0;
		while(i<listc) {
			error = ExtraL_StructlgetStruct(interp,struc,data,listv[i],tagsc,tagsv,&res);
			if (error == TCL_ERROR) {
				return TCL_ERROR;
			} else {
				Tcl_ListObjAppendElement(interp,result,res);
			}
			i++;
		}
		*value = result;
		return TCL_OK;
	} else {
		Tcl_Obj **tagv, *oldval;
		int tagc,pos,len;
		if (tagObj == NULL) {
			tagObj = Tcl_NewObj();
			error = Tcl_ListObjGetElements(interp, tagObj, &tagc, &tagv);
			Tcl_DecrRefCount(tagObj);
		} else {
			error = Tcl_ListObjGetElements(interp, tagObj, &tagc, &tagv);
		}
		if (error != TCL_OK) {return error;}
		if (tagc==1) {
			error = Tcl_ListObjLength(interp,*value,&len);
			if (len == 0) {
				*value = Tcl_NewObj();
				return TCL_OK;
			}
			if (strcmp(tag,"end") == 0) {
				pos = len-1;
				if (error != TCL_OK) {return error;}
			} else {
				error = Tcl_GetIntFromObj(interp, tagv[0], &pos);
				if (pos>=len) {
					*value = Tcl_NewObj();
					return TCL_OK;
				}
				if (error != TCL_OK) {return error;}
			}
			error = Tcl_ListObjIndex(interp, *value, pos, &oldval);
			if (error != TCL_OK) {return error;}
			if (oldval == NULL) {oldval = Tcl_NewObj();}
			error = ExtraL_StructlgetStruct(interp,struc,data,oldval,tagsc,tagsv,&res);
			if (error == TCL_ERROR) {return TCL_ERROR;}
			*value = res;
			return TCL_OK;
		} else if ((tagc==2)||(tagc==3)) {
			int p[2];
			error = Tcl_ListObjLength(interp,*value,&len);
			if (len == 0) {
				*value = Tcl_NewObj();
				return TCL_OK;
			}

			for(i=0;i<2;i++) {
				tag = Tcl_GetStringFromObj(tagv[i],NULL);
				if (strcmp(tag,"end") == 0) {
					p[i] = len-1;
					if (error != TCL_OK) {return error;}
				} else {
					error = Tcl_GetIntFromObj(interp, tagv[i], &pos);
					if (pos>=len) {
						if (i==0) {
							*value = Tcl_NewObj();
							return TCL_OK;
						} else {
							pos = len-1;
						}
					}
					if (error != TCL_OK) {return error;}
					p[i] = pos;
				}
			}
			result = Tcl_NewObj();
			for(i=p[0];i<=p[1];i++) {
				error = Tcl_ListObjIndex(interp, *value, i, &oldval);
				if (error != TCL_OK) {return error;}
				if (oldval == NULL) {oldval = Tcl_NewObj();}
				error = ExtraL_StructlgetStruct(interp,struc,data,oldval,tagsc,tagsv,&res);
				if (error == TCL_ERROR) {return TCL_ERROR;}
				Tcl_ListObjAppendElement(interp,result,res);
			}
			if (tagc == 2) {
				*value = result;
				return TCL_OK;
			} else {
				Tcl_Obj *cmd;
				cmd = Tcl_DuplicateObj(tagv[2]);
				Tcl_IncrRefCount(cmd);
				error = Tcl_ListObjAppendElement(interp,cmd,result);
				if (error != TCL_OK) {Tcl_DecrRefCount(cmd);return error;}
				error = Tcl_EvalObj(interp,cmd);
				Tcl_DecrRefCount(cmd);
				if (error != TCL_OK) {return error;}
				*value = Tcl_GetObjResult(interp);
				return TCL_OK;
			}
		} else {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"wrong # args to list: \"", tag,"\"",(char *)NULL);
			return TCL_ERROR;
		}
	}
}

int ExtraL_StructlSetNamed(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	Tcl_Obj *struc, *result, *res, *oldval;
	int error;
	int taglen;
	int pos,len;
	char *tag;

	if (tagsc == 0) {
		int tempc;
		Tcl_Obj **tempv;
		if (Tcl_ListObjGetElements(interp, *value, &tempc, &tempv) != TCL_OK) {
			return TCL_ERROR;
		}
		if ((tempc != 0)&&(tempc & 1)) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: \"", Tcl_GetStringFromObj(*value,NULL),"\" does not have an even number of elements",(char *)NULL);
			return TCL_ERROR;
		} else if (tempc != 0) {
			result = oldvalue;
			for(pos=0;pos<tempc;pos+=2) {
				res = tempv[pos+1];
				error = ExtraL_StructlSetNamed(interp,structure,data,result,1,tempv+pos,&res);
				if (error == TCL_ERROR) {
					return TCL_ERROR;
				}
				result = res;
			}
		} else {
			result = oldvalue;
		}
	} else {
		error = Tcl_ListObjIndex(interp, structure, 1, &struc);
		if (error != TCL_OK) {return error;}
		if (struc == NULL) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error in structure \"",Tcl_GetStringFromObj(structure,NULL),"\"",(char *)NULL);
			return TCL_ERROR;
		}
		tag = Tcl_GetStringFromObj(tagsv[0],&taglen);
		if (oldvalue != NULL) {
			error = ExtraL_StructlFindTag(interp, oldvalue, tag, taglen, &oldval, &pos);
			if (error != TCL_OK) {return TCL_ERROR;}
		} else {
			pos = -1;
		}
		if (pos == -1) {
			error = ExtraL_StructlsetStruct(interp,struc,data,NULL,tagsc-1,tagsv+1,*value,&res);
			if (error == TCL_ERROR) {
				Tcl_AppendResult(interp," in named \"",tag ,"\"",(char *) NULL);
				return TCL_ERROR;
			} else if (error == 5) {
				result = oldvalue;
			} else {
				if (oldvalue != NULL) {
					result = Tcl_DuplicateObj(oldvalue);
				} else {
					result = Tcl_NewObj();
				}
				error = Tcl_ListObjAppendElement(interp,result,Tcl_DuplicateObj(tagsv[0]));
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
				error = Tcl_ListObjAppendElement(interp,result,res);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
			}
		} else {
			error = ExtraL_StructlsetStruct(interp,struc,data,oldval,tagsc-1,tagsv+1,*value,&res);
			if (error == TCL_ERROR) {
				Tcl_AppendResult(interp," in named \"",tag ,"\"",(char *) NULL);
				return TCL_ERROR;
			} else if (error == 5) {
				error = Tcl_ListObjLength(interp,oldvalue,&len);
				if (error != TCL_OK) {return error;}
				if (len == 2) {
					result = NULL; 
				} else {
					result = Tcl_DuplicateObj(oldvalue);
					error = Tcl_ListObjReplace(interp,result,pos-1,2,0,NULL);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
				}
			} else {
				result = Tcl_DuplicateObj(oldvalue);
				error = Tcl_ListObjReplace(interp,result,pos,1,1,&res);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
			}
		}
	}

	*value = result;
	if (result == NULL) {
		return 5;
	} else {
		return TCL_OK;
	}
}

int ExtraL_StructlUnsetNamed(interp,structure,data,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	Tcl_Obj *oldvalue;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	Tcl_Obj *struc, *result, *res, *oldval, *def;
	int error;
	int taglen;
	int pos,end,len;
	char *tag;

	if (tagsc == 0) {
		*value = NULL;
		return 5;
	} else {
		error = Tcl_ListObjIndex(interp, structure, 1, &struc);
		if (error != TCL_OK) {return error;}
		if (struc == NULL) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error in structure \"",Tcl_GetStringFromObj(structure,NULL),"\"",(char *)NULL);
			return TCL_ERROR;
		}
		tag = Tcl_GetStringFromObj(tagsv[0],&taglen);
		error = ExtraL_StructlFindTag(interp, oldvalue, tag, taglen, &oldval, &pos);
		if (error != TCL_OK) {return TCL_ERROR;}
		if (pos == -1) {
			error = ExtraL_StructlunsetStruct(interp,struc,data,NULL,tagsc-1,tagsv+1,&res);
			if (error == TCL_ERROR) {
				Tcl_AppendResult(interp," in named \"",tag ,"\"",(char *) NULL);
				return TCL_ERROR;
			} else if (error == 5) {
				result = oldvalue;
			} else {
				result = Tcl_DuplicateObj(oldvalue);
				error = Tcl_ListObjAppendElement(interp,result,Tcl_DuplicateObj(tagsv[0]));
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
				error = Tcl_ListObjAppendElement(interp,result,res);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
			}
		} else {
			error = ExtraL_StructlunsetStruct(interp,struc,data,oldval,tagsc-1,tagsv+1,&res);
			if (error == TCL_ERROR) {
				Tcl_AppendResult(interp," in named \"",tag ,"\"",(char *) NULL);
				return TCL_ERROR;
			} else if (error == 5) {
				error = Tcl_ListObjLength(interp,oldvalue,&len);
				if (error != TCL_OK) {return error;}
				if (len == 2) {
					result = NULL; 
				} else {
					result = Tcl_DuplicateObj(oldvalue);
					error = Tcl_ListObjReplace(interp,result,pos-1,2,0,NULL);
					if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
				}
			} else {
				result = Tcl_DuplicateObj(oldvalue);
				error = Tcl_ListObjReplace(interp,result,pos,1,1,&res);
				if (error != TCL_OK) {Tcl_DecrRefCount(result);return error;}
			}
		}
	}

	*value = result;
	if (result == NULL) {
		return 5;
	}
	error = Tcl_ListObjLength(interp,structure,&end);
	if (error == TCL_ERROR) {return TCL_ERROR;}
	error = Tcl_ListObjIndex(interp,structure,end-1,&def);
	if (error == TCL_ERROR) {return TCL_ERROR;}
	if (ExtraL_ObjEqual(result,def) == 1) {
		return 5;
	} else {
		return TCL_OK;
	}
}

int ExtraL_StructlGetNamed(interp,structure,data,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
	Tcl_Obj *data;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	Tcl_Obj *struc, *result, *res, *oldval;
	Tcl_Obj **listv;
	int listc;
	int error;
	int taglen;
	int i,pos;
	char *tag;

	error = Tcl_ListObjIndex(interp, structure, 1, &struc);
	if (error != TCL_OK) {return error;}
	if (struc == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error in structure \"",Tcl_GetStringFromObj(structure,NULL),"\"",(char *)NULL);
		return TCL_ERROR;
	}
	if (tagsc == 0) {
		if (*value == NULL) {
			*value = Tcl_NewObj();
			return TCL_OK;
		}
		error = Tcl_ListObjGetElements(interp, *value, &listc, &listv);
		if (error != TCL_OK) {return error;}
		if ((listc != 0)&&(listc & 1)) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error: list \"", Tcl_GetStringFromObj(*value,NULL),"\" does not have an even number of elements",(char *)NULL);
			return TCL_ERROR;
		}
		result = Tcl_NewObj();
		i=0;
		while(i<listc) {
			error = ExtraL_StructlgetStruct(interp,struc,data,listv[i+1],0,NULL,&res);
			if (error == TCL_ERROR) {
				return TCL_ERROR;
			} else {
				Tcl_ListObjAppendElement(interp,result,listv[i]);
				Tcl_ListObjAppendElement(interp,result,res);
			}
			i+=2;
		}
		*value = result;
		return TCL_OK;
	} else {
		if (*value == NULL) {
			error = ExtraL_StructlgetStruct(interp,struc,data,NULL,tagsc,tagsv,&res);
			if (error == TCL_ERROR) {return TCL_ERROR;}
		} else {
			tag = Tcl_GetStringFromObj(tagsv[0],&taglen);
			tagsc--;
			tagsv++;
			error = ExtraL_StructlFindTag(interp, *value, tag, taglen, &oldval, &pos);
			if (error != TCL_OK) {return TCL_ERROR;}
			error = ExtraL_StructlgetStruct(interp,struc,data,oldval,tagsc,tagsv,&res);
			if (error == TCL_ERROR) {return TCL_ERROR;}
		}
		*value = res;
		return TCL_OK;
	}
}
