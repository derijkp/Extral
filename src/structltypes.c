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

int ExtraL_StructlSetInt(interp,substructure,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
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
		"\" not present in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}

	error = Tcl_GetIntFromObj(interp,*value,&temp);
	if (error != TCL_OK) {return error;}
	return TCL_OK;
}

int ExtraL_StructlSetDouble(interp,substructure,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
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
		"\" not present in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}

	error = Tcl_GetDoubleFromObj(interp,*value,&temp);
	if (error != TCL_OK) {return error;}
	return TCL_OK;
}

int ExtraL_StructlSetBool(interp,substructure,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
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
		"\" not present in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}

	error = Tcl_GetBooleanFromObj(interp,*value,&temp);
	if (error != TCL_OK) {return error;}
	if Tcl_IsShared(*value) {
		*value = Tcl_DuplicateObj(*value);
	}
	Tcl_SetBooleanObj(*value,temp);
	return TCL_OK;
}

int ExtraL_StructlSetRegexp(interp,substructure,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
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
		"\" not present in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}

	error = Tcl_ListObjLength(interp,substructure,&temp);
	if (error != TCL_OK) {return error;}
	if (temp != 4) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: wrong number of arguments in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	error = Tcl_ListObjIndex(interp, substructure, 1, &patternObj);
	if (error != TCL_OK) {return error;}
	pattern = Tcl_GetStringFromObj(patternObj,&temp);
	string = Tcl_GetStringFromObj(*value,&temp);
	error = Tcl_RegExpMatch(interp, string, pattern);
	if (error == -1) {
		return TCL_ERROR;
	} else if (error == 0) {
		Tcl_ResetResult(interp);
		error = Tcl_ListObjIndex(interp, substructure, 2, &patternObj);
		if (error != TCL_OK) {return error;}
		pattern = Tcl_GetStringFromObj(patternObj,&temp);
		Tcl_AppendResult(interp,"error: \"", string,"\" ", pattern, (char *)NULL);
		return TCL_ERROR;
	}
	return TCL_OK;
}

int ExtraL_StructlSetBetween(interp,substructure,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
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
		"\" not present in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}

	error = Tcl_ListObjLength(interp,substructure,&temp);
	if (error != TCL_OK) {return error;}
	if (temp != 4) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: wrong number of arguments in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	error = Tcl_GetIntFromObj(interp,*value,&temp);
	if (error != TCL_OK) {return error;}

	error = Tcl_ListObjIndex(interp, substructure, 1, &startObj);
	if (error != TCL_OK) {return error;}
	error = Tcl_GetIntFromObj(interp,startObj,&start);
	if (error != TCL_OK) {return error;}

	error = Tcl_ListObjIndex(interp, substructure, 2, &endObj);
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

int ExtraL_StructlSetDBetween(interp,substructure,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
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
		"\" not present in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}

	error = Tcl_ListObjLength(interp,substructure,&temp);
	if (error != TCL_OK) {return error;}
	if (temp != 4) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: wrong number of arguments in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}
	error = Tcl_GetDoubleFromObj(interp,*value,&val);
	if (error != TCL_OK) {return error;}

	error = Tcl_ListObjIndex(interp, substructure, 1, &startObj);
	if (error != TCL_OK) {return error;}
	error = Tcl_GetDoubleFromObj(interp,startObj,&start);
	if (error != TCL_OK) {return error;}

	error = Tcl_ListObjIndex(interp, substructure, 2, &endObj);
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

int ExtraL_StructlSetDate(interp,substructure,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
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
		"\" not present in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}

	error = ExtraL_ScanTime(interp,1,0,*value,&result);
	if (error != TCL_OK) {return error;}
	if Tcl_IsShared(*value) {
		*value = Tcl_DuplicateObj(*value);
	}
	Tcl_SetDoubleObj(*value,result);
	return TCL_OK;
}

int ExtraL_StructlGetDate(interp,substructure,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	double time;
	char *result;
	int error;
	if (tagsc > 0) {
		int temp;
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: field \"", Tcl_GetStringFromObj(tagsv[0],&temp),
		"\" not present in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}

	error = Tcl_GetDoubleFromObj(interp,*value,&time);
	if (error != TCL_OK) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"time should be a double", (char *)NULL);
		return TCL_ERROR;
	}
	error = ExtraL_FormatTime(interp,time,"%e %b %Y",&result);
	if (error != TCL_OK) {return error;}
	*value = Tcl_NewStringObj(result,strlen(result));
	return TCL_OK;
}

int ExtraL_StructlSetTime(interp,substructure,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
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
		"\" not present in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}

	error = ExtraL_ScanTime(interp,1,1,*value,&result);
	if (error != TCL_OK) {return error;}
	if Tcl_IsShared(*value) {
		*value = Tcl_DuplicateObj(*value);
	}
	Tcl_SetDoubleObj(*value,result);
	return TCL_OK;
}

int ExtraL_StructlGetTime(interp,substructure,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	int tagsc;
	Tcl_Obj **tagsv;
	Tcl_Obj **value;
{
	double time;
	char *result;
	int error;
	if (tagsc > 0) {
		int temp;
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error: field \"", Tcl_GetStringFromObj(tagsv[0],&temp),
		"\" not present in structure \"", Tcl_GetStringFromObj(substructure,&temp), "\"",(char *)NULL);
		return TCL_ERROR;
	}

	error = Tcl_GetDoubleFromObj(interp,*value,&time);
	if (error != TCL_OK) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"time should be a double", (char *)NULL);
		return TCL_ERROR;
	}
	error = ExtraL_FormatTime(interp,time,"%e %b %Y %H:%M:%S",&result);
	if (error != TCL_OK) {return error;}
	*value = Tcl_NewStringObj(result,strlen(result));
	return TCL_OK;
}

int ExtraL_StructlSetList(interp,structure,oldvalue,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
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
				error = ExtraL_StructlsetStruct(interp,struc,oldv[i],tagsc,tagsv,listv[i],&res);
			} else {
				error = ExtraL_StructlsetStruct(interp,struc,empty,tagsc,tagsv,listv[i],&res);
			}
			if (error == TCL_ERROR) {
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
		error = ExtraL_StructlsetStruct(interp,struc,empty,tagsc,tagsv,*value,&res);
		if (error == TCL_ERROR) {return TCL_ERROR;}
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

			error = ExtraL_StructlsetStruct(interp,struc,oldval,tagsc,tagsv,*value,&res);
			if (error == TCL_ERROR) {return TCL_ERROR;}
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

int ExtraL_StructlGetList(interp,structure,tagsc,tagsv,value)
	Tcl_Interp *interp;
	Tcl_Obj *structure;
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
		empty = Tcl_NewObj();
		while(i<listc) {
			error = ExtraL_StructlgetStruct(interp,struc,listv[i],tagsc,tagsv,&res);
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
			error = Tcl_ListObjIndex(interp, *value, pos, &oldval);
			if (error != TCL_OK) {return error;}
			if (oldval == NULL) {oldval = Tcl_NewObj();}
			error = ExtraL_StructlgetStruct(interp,struc,oldval,tagsc,tagsv,&res);
			if (error == TCL_ERROR) {return TCL_ERROR;}
			*value = res;
			return TCL_OK;
		} else if (tagc==2) {
			int p[2];
			error = Tcl_ListObjLength(interp,*value,&len);
			if (len == 0) {
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"empty list",(char *)NULL);
				return TCL_ERROR;
			}

			for(i=0;i<2;i++) {
				tag = Tcl_GetStringFromObj(tagv[i],NULL);
				if (strcmp(tag,"end") == 0) {
					p[i] = len-1;
					if (error != TCL_OK) {return error;}
				} else {
					error = Tcl_GetIntFromObj(interp, tagv[i], &pos);
					if (pos>=len) {
						Tcl_ResetResult(interp);
						Tcl_AppendResult(interp,"list doesn't contain element ",
							Tcl_GetStringFromObj(tagv[i],NULL),(char *)NULL);
						return TCL_ERROR;
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
				error = ExtraL_StructlgetStruct(interp,struc,oldval,tagsc,tagsv,&res);
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
