/*	
 *	 File:    structl.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tcl.h"

int ExtraL_StructlSetInt(interp,substructure,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj **value;
{
	int temp, error;

	error = Tcl_GetIntFromObj(interp,*value,&temp);
	if (error != TCL_OK) {return error;}
	return TCL_OK;
}

int ExtraL_StructlSetDouble(interp,substructure,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj **value;
{
	double temp;
	int error;

	error = Tcl_GetDoubleFromObj(interp,*value,&temp);
	if (error != TCL_OK) {return error;}
	return TCL_OK;
}

int ExtraL_StructlSetBool(interp,substructure,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj **value;
{
	int temp, error;

	error = Tcl_GetBooleanFromObj(interp,*value,&temp);
	if (error != TCL_OK) {return error;}
	if Tcl_IsShared(*value) {
		*value = Tcl_DuplicateObj(*value);
	}
	Tcl_SetBooleanObj(*value,temp);
	return TCL_OK;
}

int ExtraL_StructlSetRegexp(interp,substructure,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj **value;
{
	int temp,error;

	Tcl_Obj *patternObj;
	char *string, *pattern;
	error = Tcl_ListObjLength(interp,substructure,&temp);
	if (error != TCL_OK) {return error;}
	if (temp != 3) {
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
		Tcl_AppendResult(interp,"error: \"", string,"\" does not match pattern \"", pattern, "\"",(char *)NULL);
		return TCL_ERROR;
	}
	return TCL_OK;
}

int ExtraL_StructlSetBetween(interp,substructure,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj **value;
{
	int temp,error;
	Tcl_Obj *startObj, *endObj;
	int start,end;

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

int ExtraL_StructlSetDBetween(interp,substructure,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj **value;
{
	int temp,error;
	Tcl_Obj *startObj, *endObj;
	double start, end, val;

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

int ExtraL_StructlSetDate(interp,substructure,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj **value;
{
	double result;
	int temp, error;

	error = ExtraL_ScanTime(interp,1,0,*value,&result);
	if (error != TCL_OK) {return error;}
	if Tcl_IsShared(*value) {
		*value = Tcl_DuplicateObj(*value);
	}
	Tcl_SetDoubleObj(*value,result);
	return TCL_OK;
}

int ExtraL_StructlGetDate(interp,substructure,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj **value;
{
	double time;
	char *result;
	int error;

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

int ExtraL_StructlSetTime(interp,substructure,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj **value;
{
	double result;
	int temp, error;

	error = ExtraL_ScanTime(interp,1,1,*value,&result);
	if (error != TCL_OK) {return error;}
	if Tcl_IsShared(*value) {
		*value = Tcl_DuplicateObj(*value);
	}
	Tcl_SetDoubleObj(*value,result);
	return TCL_OK;
}

int ExtraL_StructlGetTime(interp,substructure,value)
	Tcl_Interp *interp;
	Tcl_Obj *substructure;
	Tcl_Obj **value;
{
	double time;
	char *result;
	int error;

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
