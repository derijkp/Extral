/*	
 *	 File:    date.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tcl.h"

int ExtraL_TaglScanDateObj(interp,dateObj,resultPtr)
	Tcl_Interp *interp;
	Tcl_Obj *dateObj;
	Tcl_Obj **resultPtr;
{
	Tcl_RegExp regexp;
	char *date;
	char result[23];
	char temp[6];
	char *ctemp = NULL;
	char ch;
	int year=-1,month=-1,day=-1,hour=-1,min=-1,sec=-1,ms=-1,bc=0;
	int first=-1,second=-1;
	int incldate = 1, incltime = 1;
	int busyhour=0;
	int i,j,len,temppos;
	int error;

	date = Tcl_GetStringFromObj(dateObj,&len);

	temppos = 0;
	i=0;
	while(i<=len) {
		if (i!=len) {
			ch = date[i];
		} else {
			ch = ' ';
		}
		if ((ch == ':')||((ch == ' ')&&(busyhour == 1))) {
			if (ch == ':') {
				busyhour = 1;
			} else {
				busyhour = 0;
			}
			temp[temppos] = '\0';
			if (hour == -1) {
				error = Tcl_GetInt(interp, temp, &hour);
				if (error != TCL_OK) {return error;}
			} else if (min == -1) {
				error = Tcl_GetInt(interp, temp, &min);
				if (error != TCL_OK) {return error;}
			} else if (sec == -1) {
				error = Tcl_GetInt(interp, temp, &sec);
				if (error != TCL_OK) {return error;}
			} else if (ms == -1) {
				error = Tcl_GetInt(interp, temp, &ms);
				if (error != TCL_OK) {return error;}
			}
			temppos = 0;
		} else if ((ispunct(ch)!=0)||(ch==' ')) {
			temp[temppos] = '\0';
			if (temppos==4) {
				error = Tcl_GetInt(interp, temp, &year);
				if (error != TCL_OK) {return error;}
			} else if (isalpha(temp[0])) {
				if (strncmp(temp,"jan",3)==0) {month = 1;}
				else if (strncmp(temp,"feb",3)==0) {month = 2;}
				else if (strncmp(temp,"mar",3)==0) {month = 3;}
				else if (strncmp(temp,"apr",3)==0) {month = 4;}
				else if (strncmp(temp,"may",3)==0) {month = 5;}
				else if (strncmp(temp,"jun",3)==0) {month = 6;}
				else if (strncmp(temp,"jul",3)==0) {month = 7;}
				else if (strncmp(temp,"aug",3)==0) {month = 8;}
				else if (strncmp(temp,"sep",3)==0) {month = 9;}
				else if (strncmp(temp,"oct",3)==0) {month = 10;}
				else if (strncmp(temp,"nov",3)==0) {month = 11;}
				else if (strncmp(temp,"dec",3)==0) {month = 12;}
				else if (strncmp(temp,"bc",2)==0) {bc = 1;}
			} else {
				error = Tcl_GetInt(interp, temp, &j);
				if (error != TCL_OK) {return error;}
				if (first == -1) {
					first = j;
				} else {
					second = j;
				}
			}
			temppos = 0;
		} else {
			temp[temppos++] = tolower(ch);
			if (temppos == 6) {
				temp[5]='\0';
				temppos = 0;
				Tcl_ResetResult(interp);
				Tcl_AppendResult(interp,"error while parsing date \"",date, "\": part \"",temp,"\"... too big \"",(char *)NULL);
				return TCL_ERROR;
			}
		}
		i++;
	}

	if (month == -1) {
		day = first;
		month = second;
	} else {
		day = first;
	}
	if ((year == -1)&&(month == -1)&&(day == -1)) {
		incldate = 0;
	} else if (year == -1) {
		ctemp = "year";
	} else if (month == -1) {
		ctemp = "month";
	} else if (day == -1) {
		ctemp = "day";
	}
	if (ctemp != NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error while parsing date \"",date, "\": ", ctemp, " not found",(char *)NULL);
		return TCL_ERROR;
	}

	if ((hour == -1)&&(min == -1)) {
		incltime = 0;
	} else if (hour == -1) {
		ctemp = "hours";
	} else if (min == -1) {
		ctemp = "minutes";
	}
	if (ctemp != NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error while parsing time \"",date, "\": ", ctemp, " not found",(char *)NULL);
		return TCL_ERROR;
	}

	i=0;j=0;
	if (incldate == 1) {
		if ((month<1)||(month>12)) {
			ctemp = "impossible month";
		}
		if ((day<1)||(day>31)) {
			ctemp = "impossible day";
		}
		if (ctemp != NULL) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error while parsing date in \"",date, "\": ", ctemp, (char *)NULL);
			return TCL_ERROR;
		}
		if (incltime == 1) {
			i = sprintf(result,"%4.4d/%2.2d/%2.2d#",year,month,day);
		} else {
			i = sprintf(result,"%4.4d/%2.2d/%2.2d",year,month,day);
		}
	}
	if (incltime == 1) {
		if ((hour<0)||(hour>23)) {
			ctemp = "impossible hour";
		}
		if ((min<0)||(min>59)) {
			ctemp = "impossible minutes";
		}
		if ((sec!=-1)&&((sec<0)||(sec>59))) {
			ctemp = "impossible seconds";
		}
		if ((ms!=-1)&&((ms<0)||(ms>99))) {
			ctemp = "impossible miliseconds";
		}
		if (ctemp != NULL) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error while parsing time in \"",date, "\": ", ctemp, (char *)NULL);
			return TCL_ERROR;
		}
		if (ms != -1) {
			j = sprintf(result+i,"%2.2d:%2.2d:%2.2d:%2.2d",hour,min,sec,ms);
		} else if (sec != -1) {
			j = sprintf(result+i,"%2.2d:%2.2d:%2.2d",hour,min,sec);
		} else {
			j = sprintf(result+i,"%2.2d:%2.2d",hour,min);
		}
	}
	*resultPtr = Tcl_NewStringObj(result,i+j);
	return TCL_OK;
}

/*
int ExtraL_TaglScanDateObj(interp,date,resultPtr)
	Tcl_Interp *interp;
	Tcl_Obj *date;
	Tcl_Obj **resultPtr;
{
	Tcl_RegExp regexp;
	char *datestring;
	char *start, *end;
	char temp[5];
	int datelen, error, match, day;
	int len,i;

	datestring = Tcl_GetStringFromObj(date,&datelen);

	regexp = Tcl_RegExpCompile(interp, "-?[0-9][0-9][0-9][0-9]");
	if (regexp == NULL) {return TCL_ERROR;}
	match = Tcl_RegExpExec(interp, regexp, datestring, datestring);
	if (match == -1) {return TCL_ERROR;}
	if (match == 0) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error while parsing date \"",datestring, "\": no year found (must be fully specified!)",(char *)NULL);
			return TCL_ERROR;
	}
	Tcl_RegExpRange(regexp, 0, &start, &end);
	len = end-start;
	*resultPtr = Tcl_NewStringObj(start,len);

	regexp = Tcl_RegExpCompile(interp, "([0-9][0-9]?)( |/|-)(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sept|Oct|Nov|Dec|[0-9]?[0-9])[^0-9]");
	if (regexp == NULL) {return TCL_ERROR;}
	match = Tcl_RegExpExec(interp, regexp, datestring, datestring);
	if (match == -1) {return TCL_ERROR;}
	if (match == 1) {
		Tcl_RegExpRange(regexp, 1, &start, &end);
		len = end-start;
		strncpy(temp,start,len);
		temp[len] = '\0';
		error = Tcl_GetInt(interp,temp,&day);
		if (error != TCL_OK) {return error;}

		Tcl_RegExpRange(regexp, 3, &start, &end);
	} else {
		regexp = Tcl_RegExpCompile(interp, "(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sept|Oct|Nov|Dec)( |/|-)([0-9][0-9]?)[^0-9]");
		if (regexp == NULL) {return TCL_ERROR;}
		match = Tcl_RegExpExec(interp, regexp, datestring, datestring);
		if (match == -1) {return TCL_ERROR;}
		if (match == 0) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp,"error while parsing date \"",datestring, "\": no month found",(char *)NULL);
			return TCL_ERROR;
		}
		Tcl_RegExpRange(regexp, 3, &start, &end);
		len = end-start;
		strncpy(temp,start,len);
		temp[len] = '\0';
		error = Tcl_GetInt(interp,temp,&day);
		if (error != TCL_OK) {return error;}

		Tcl_RegExpRange(regexp, 1, &start, &end);
	}
	strncpy(temp,start,3);
	temp[3] = '\0';
	if (strncmp(temp,"Jan",3)==0) {i = 1;}
	else if (strncmp(temp,"Feb",3)==0) {i = 2;}
	else if (strncmp(temp,"Mar",3)==0) {i = 3;}
	else if (strncmp(temp,"Apr",3)==0) {i = 4;}
	else if (strncmp(temp,"May",3)==0) {i = 5;}
	else if (strncmp(temp,"Jun",3)==0) {i = 6;}
	else if (strncmp(temp,"Jul",3)==0) {i = 7;}
	else if (strncmp(temp,"Aug",3)==0) {i = 8;}
	else if (strncmp(temp,"Sep",3)==0) {i = 9;}
	else if (strncmp(temp,"Oct",3)==0) {i = 10;}
	else if (strncmp(temp,"Nov",3)==0) {i = 11;}
	else if (strncmp(temp,"Dec",3)==0) {i = 12;}
	else {
		len = end-start;
		if (len>2) {
			error = TCL_ERROR;
		} else {
			temp[len] = '\0';
			error = Tcl_GetInt(interp,temp,&i);
		}
	}
	if ((i<0)||(i>12)) {error = TCL_ERROR;}
	if (error == TCL_ERROR) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error in month while parsing date \"",datestring, "\"",(char *)NULL);
		return TCL_ERROR;
	}
	sprintf(temp,"/%2.2d",i);
	Tcl_AppendToObj(*resultPtr,temp,4);

	if ((day<0)||(day>31)) {error = TCL_ERROR;}
	if (error == TCL_ERROR) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"error in day while parsing date \"",datestring, "\"",(char *)NULL);
		return TCL_ERROR;
	}
	sprintf(temp,"/%2.2d",day);
	Tcl_AppendToObj(*resultPtr,temp,4);
	return TCL_OK;
}
*/

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_ScanDatCmd --
 *
 *		This procedure is invoked to process the "scandate" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_ScanDateObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *result;
	int error;

	if ((objc != 2)) {
		Tcl_WrongNumArgs(interp, 1, objv, "datestring");
		return TCL_ERROR;
	}

	error = ExtraL_TaglScanDateObj(interp,objv[1],&result);
	if (error != TCL_OK) {return error;}
	Tcl_SetObjResult(interp,result);
	return TCL_OK;
}

