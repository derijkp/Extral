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
		Tcl_AppendResult(interp,"error: wrong number of arguments in structure \"", Tcl_GetStringFromObj(structure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	error = Tcl_ListObjIndex(interp, structure, 1, &patternObj);
	if (patternObj == NULL) {patternObj = Tcl_NewObj();}
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
		if (patternObj == NULL) {patternObj = Tcl_NewObj();}
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
	if (startObj == NULL) {startObj = Tcl_NewObj();}
	error = Tcl_GetIntFromObj(interp,startObj,&start);
	if (error != TCL_OK) {return error;}

	error = Tcl_ListObjIndex(interp, structure, 2, &endObj);
	if (endObj == NULL) {endObj = Tcl_NewObj();}
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
	if (startObj == NULL) {startObj = Tcl_NewObj();}
	error = Tcl_GetDoubleFromObj(interp,startObj,&start);
	if (error != TCL_OK) {return error;}

	error = Tcl_ListObjIndex(interp, structure, 2, &endObj);
	if (error != TCL_OK) {return error;}
	if (endObj == NULL) {endObj = Tcl_NewObj();}
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
	if Tcl_IsShared(*value) {
		*value = Tcl_DuplicateObj(*value);
	}
	Tcl_SetDoubleObj(*value,result);
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

	Tcl_GetStringFromObj(*value,&len);
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
		}
	} else {
		error = ExtraL_FormatTime(interp,time,"%e %b %Y",&result);
		if (error != TCL_OK) {return error;}
		*value = Tcl_NewStringObj(result,strlen(result));
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
	if Tcl_IsShared(*value) {
		*value = Tcl_DuplicateObj(*value);
	}
	Tcl_SetDoubleObj(*value,result);
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

	Tcl_GetStringFromObj(*value,&len);
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
		}
	} else {
		error = ExtraL_FormatTime(interp,time,"%e %b %Y %H:%M:%S",&result);
		if (error != TCL_OK) {return error;}
		*value = Tcl_NewStringObj(result,strlen(result));
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
	Tcl_Obj *struc, *result, *empty, *res, *tagObj;
	Tcl_Obj **listv;
	int listc;
	int error;
	int taglen;
	int i;
	char *tag;

	if (tagsc == 0) {
		tag = "";
		taglen = 0;
		tagObj = Tcl_NewObj();
	} else {
		tagObj = tagsv[0];
		tag = Tcl_GetStringFromObj(tagsv[0],&taglen);
		tagsc--;
		tagsv++;
	}
	error = Tcl_ListObjIndex(interp, structure, 1, &struc);
	if (error != TCL_OK) {return error;}
	if (struc == NULL) {struc = Tcl_NewObj();}
	if (taglen==0) {
		Tcl_Obj **oldv;
		int oldc;
		error = Tcl_ListObjGetElements(interp, *value, &listc, &listv);
		if (error != TCL_OK) {return error;}
		error = Tcl_ListObjGetElements(interp, oldvalue, &oldc, &oldv);
		if (error != TCL_OK) {return error;}
		result = Tcl_NewObj();
		i=0;
		empty = Tcl_NewObj();
		while(i<listc) {
			if (i<oldc) {
				error = ExtraL_StructlsetStruct(interp,struc,data,oldv[i],tagsc,tagsv,listv[i],&res);
			} else {
				error = ExtraL_StructlsetStruct(interp,struc,data,empty,tagsc,tagsv,listv[i],&res);
			}
			if (error == 5) {
				Tcl_ListObjAppendElement(interp,result,Tcl_NewObj());
			} else if (error != TCL_OK) {
				return TCL_ERROR;
			} else {
				Tcl_ListObjAppendElement(interp,result,res);
			}
			i++;
		}
		*value = result;
		return TCL_OK;
	} else if (strncmp(tag,"next",taglen)==0) {
		empty = Tcl_NewObj();
		error = ExtraL_StructlsetStruct(interp,struc,data,empty,tagsc,tagsv,*value,&res);
		if (error == 5) {
			res = Tcl_NewObj();
		} else if (error != TCL_OK) {
			return TCL_ERROR;
		}
		result = Tcl_DuplicateObj(oldvalue);
		error = Tcl_ListObjAppendElement(interp,result,res);
		if (error != TCL_OK) {return error;}
		*value = result;
		return TCL_OK;
	} else {
		Tcl_Obj **tagv, *oldval;
		int tagc,pos,len;
		error = Tcl_ListObjGetElements(interp, tagObj, &tagc, &tagv);
		if (error != TCL_OK) {return error;}
		if (tagc==1) {
			error = Tcl_ListObjLength(interp,oldvalue,&len);
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
			if (oldval == NULL) {oldval = Tcl_NewObj();}

			error = ExtraL_StructlsetStruct(interp,struc,data,oldval,tagsc,tagsv,*value,&res);
			if (error == 5) {
				res = Tcl_NewObj();
			} else if (error != TCL_OK) {
				return TCL_ERROR;
			}
			result = Tcl_DuplicateObj(oldvalue);
			error = Tcl_ListObjReplace(interp,result,pos,1,1,&res);
			if (error != TCL_OK) {return error;}
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
	int error;
	int taglen;
	char *tag;

	if (tagsc == 0) {
		*value = NULL;
		return 5;
	} else {
		tagObj = tagsv[0];
		tag = Tcl_GetStringFromObj(tagsv[0],&taglen);
		tagsc--;
		tagsv++;
	}
	error = Tcl_ListObjIndex(interp, structure, 1, &struc);
	if (error != TCL_OK) {return error;}
	if (struc == NULL) {struc = Tcl_NewObj();}
	if (taglen==0) {
		*value = NULL;
		return 5;
	} else {
		Tcl_Obj **tagv, *oldval;
		int tagc,pos,len;
		error = Tcl_ListObjGetElements(interp, tagObj, &tagc, &tagv);
		if (error != TCL_OK) {return error;}
		if (tagc==1) {
			error = Tcl_ListObjLength(interp,oldvalue,&len);
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
			if (oldval == NULL) {oldval = Tcl_NewObj();}
			if (tagsc == 0) {
				error = 5;
			} else {
				error = ExtraL_StructlunsetStruct(interp,struc,data,oldval,tagsc,tagsv,&res);
				if (error == TCL_ERROR) {return TCL_ERROR;}
			}
			if (error != 5) {
				result = Tcl_DuplicateObj(oldvalue);
				error = Tcl_ListObjReplace(interp,result,pos,1,1,&res);
			} else {
				result = Tcl_DuplicateObj(oldvalue);
				error = Tcl_ListObjReplace(interp,result,pos,1,0,NULL);
			}
			if (error != TCL_OK) {return error;}
			*value = result;
			return TCL_OK;
		} else {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"wrong # args to list",(char *)NULL);
			return TCL_ERROR;
		}
	}
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
	int listc;
	int error;
	int taglen;
	int i;
	char *tag;

	if (tagsc == 0) {
		tag = "";
		taglen = 0;
		tagObj = Tcl_NewObj();
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
	if (struc == NULL) {struc = Tcl_NewObj();}
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
		error = Tcl_ListObjGetElements(interp, tagObj, &tagc, &tagv);
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
		} else if (tagc==2) {
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
			*value = result;
			return TCL_OK;
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
	Tcl_Obj *struc, *result, *empty, *res, *oldval, *def;
	int error;
	int taglen;
	int pos,end;
	char *tag;

	if (tagsc == 0) {
		int tempc;
		Tcl_Obj **tempv, *temp;
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
				temp = tempv[pos+1];
				error = ExtraL_StructlSetNamed(interp,structure,data,result,1,tempv+pos,&temp);
				if (error == TCL_ERROR) {
					return TCL_ERROR;
				}
				result = temp;
			}
		} else {
			result = oldvalue;
		}
	} else {
		error = Tcl_ListObjIndex(interp, structure, 1, &struc);
		if (error != TCL_OK) {return error;}
		if (struc == NULL) {struc = Tcl_NewObj();}

		tag = Tcl_GetStringFromObj(tagsv[0],&taglen);
		error = ExtraL_StructlFindTag(interp, oldvalue, tag, taglen, &oldval, &pos);
		if (error != TCL_OK) {return TCL_ERROR;}
		if (pos == -1) {
			empty = Tcl_NewObj();
			error = ExtraL_StructlsetStruct(interp,struc,data,empty,tagsc-1,tagsv+1,*value,&res);
			if (error == TCL_ERROR) {
				Tcl_AppendResult(interp," in named \"",tag ,"\"",(char *) NULL);
				return TCL_ERROR;
			} else if (error == 5) {
				result = oldvalue;
			} else {
				result = Tcl_DuplicateObj(oldvalue);
				error = Tcl_ListObjAppendElement(interp,result,Tcl_DuplicateObj(tagsv[0]));
				if (error != TCL_OK) {return error;}
				error = Tcl_ListObjAppendElement(interp,result,res);
				if (error != TCL_OK) {return error;}
			}
		} else {
			error = ExtraL_StructlsetStruct(interp,struc,data,oldval,tagsc-1,tagsv+1,*value,&res);
			if (error == TCL_ERROR) {
				Tcl_AppendResult(interp," in named \"",tag ,"\"",(char *) NULL);
				return TCL_ERROR;
			} else if (error == 5) {
				result = Tcl_DuplicateObj(oldvalue);
				error = Tcl_ListObjReplace(interp,result,pos-1,2,0,NULL);
				if (error != TCL_OK) {return error;}
			} else {
				result = Tcl_DuplicateObj(oldvalue);
				error = Tcl_ListObjReplace(interp,result,pos,1,1,&res);
				if (error != TCL_OK) {return error;}
			}
		}
	}

	*value = result;
	Tcl_GetStringFromObj(result,&end);
	if (end == 0) {
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
	Tcl_Obj *struc, *result, *empty, *res, *oldval, *def;
	int error;
	int taglen;
	int pos,end;
	char *tag;

	if (tagsc == 0) {
		*value = NULL;
		return 5;
	} else {
		error = Tcl_ListObjIndex(interp, structure, 1, &struc);
		if (error != TCL_OK) {return error;}
		if (struc == NULL) {struc = Tcl_NewObj();}

		tag = Tcl_GetStringFromObj(tagsv[0],&taglen);
		error = ExtraL_StructlFindTag(interp, oldvalue, tag, taglen, &oldval, &pos);
		if (error != TCL_OK) {return TCL_ERROR;}
		if (pos == -1) {
			empty = Tcl_NewObj();
			error = ExtraL_StructlunsetStruct(interp,struc,data,empty,tagsc-1,tagsv+1,&res);
			if (error == TCL_ERROR) {
				Tcl_AppendResult(interp," in named \"",tag ,"\"",(char *) NULL);
				return TCL_ERROR;
			} else if (error == 5) {
				result = oldvalue;
			} else {
				result = Tcl_DuplicateObj(oldvalue);
				error = Tcl_ListObjAppendElement(interp,result,Tcl_DuplicateObj(tagsv[0]));
				if (error != TCL_OK) {return error;}
				error = Tcl_ListObjAppendElement(interp,result,res);
				if (error != TCL_OK) {return error;}
			}
		} else {
			error = ExtraL_StructlunsetStruct(interp,struc,data,oldval,tagsc-1,tagsv+1,&res);
			if (error == TCL_ERROR) {
				Tcl_AppendResult(interp," in named \"",tag ,"\"",(char *) NULL);
				return TCL_ERROR;
			} else if (error == 5) {
				result = Tcl_DuplicateObj(oldvalue);
				error = Tcl_ListObjReplace(interp,result,pos-1,2,0,NULL);
				if (error != TCL_OK) {return error;}
			} else {
				result = Tcl_DuplicateObj(oldvalue);
				error = Tcl_ListObjReplace(interp,result,pos,1,1,&res);
				if (error != TCL_OK) {return error;}
			}
		}
	}

	*value = result;
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
	if (struc == NULL) {struc = Tcl_NewObj();}
	if (tagsc == 0) {
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
		tag = Tcl_GetStringFromObj(tagsv[0],&taglen);
		tagsc--;
		tagsv++;
		error = ExtraL_StructlFindTag(interp, *value, tag, taglen, &oldval, &pos);
		if (error != TCL_OK) {return TCL_ERROR;}
		if (oldval == NULL) {oldval = Tcl_NewObj();}
		error = ExtraL_StructlgetStruct(interp,struc,data,oldval,tagsc,tagsv,&res);
		if (error == TCL_ERROR) {return TCL_ERROR;}
		*value = res;
		return TCL_OK;
	}
}
