/*	
 *	 File:    extral.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "tcl.h"
#include "tclRegexp.h"
#include "general.h"
#define UCHAR(c) ((unsigned char) (c))
#define EXACT		0
#define GLOB		1
#define REGEXP		2
 
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_FfindCmd --
 *
 *		This procedure is invoked to process the "ffind" command.
 *		It scans a number of files, and returns those which match
 *		given patterns.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

		/* ARGSUSED */
int
ExtraL_FfindCmd(notUsed, interp, argc, argv)
	ClientData notUsed;						/* Not used. */
	Tcl_Interp *interp;						/* Current interpreter. */
	int argc;								/* Number of arguments. */
	char **argv;						/* Argument strings. */
{

	Tcl_RegExp regexp;
	char *start, *end;
	Tcl_Channel file;
	char **argPtr;
	int listArgc;
	char **listArgv;
	int allfiles;
	int matches, allmatches, multiple, number;
	int i, j, match, mode, index;
	int step=2;

	if (argc < 4) {
		wrongNumArgs:
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" ?switches? fileList pattern ?nullvalue? ?varName pattern ?nullvalue? varName ...?\"", (char *) NULL);
		return TCL_ERROR;
	}
	argPtr = argv+1;
	argc--;
	matches = 0;
	allmatches = 0;
	allfiles = 0;
	multiple = 0;
	mode = EXACT;
	while ((argc > 0) && (argPtr[0][0] == '-')) {
		if (strcmp(argPtr[0], "-matches") == 0) {
			matches = 1;
		} else if (strcmp(argPtr[0], "-allmatches") == 0) {
			allmatches = 1;
		} else if (strcmp(argPtr[0], "-allfiles") == 0) {
/*
			argPtr++;
			argc--;
			if (argc==0) break;
			allfiles = argPtr[0];
*/
			allfiles = 1;
			step=3;
		} else if (strcmp(argPtr[0], "-exact") == 0) {
			mode = EXACT;
		} else if (strcmp(argPtr[0], "-glob") == 0) {
			mode = GLOB;
		} else if (strcmp(argPtr[0], "-regexp") == 0) {
			mode = REGEXP;
		} else if (strcmp(argPtr[0], "--") == 0) {
			argPtr++;
			argc--;
			break;
		} else {
			Tcl_AppendResult(interp, "bad switch \"", argPtr[0],
				"\": must be -matches, -allmatches, -allfiles, -exact, -glob, -regexp or --", (char *) NULL);
			return TCL_ERROR;
		}
		argPtr++;
		argc--;
	}
	if ((matches==1)&&(mode!=REGEXP)) {
		Tcl_AppendResult(interp, "-matches only applicable with -regexp matching", (char *) NULL);
		return TCL_ERROR;
	}
	if ((allmatches==1)&&(matches==0)) {
		Tcl_AppendResult(interp, "-allmatches only applicable with -matches options", (char *) NULL);
		return TCL_ERROR;
	}
	if ((allfiles!=0)&&(matches==0)) {
		Tcl_AppendResult(interp, "-allfiles only applicable with -matches options", (char *) NULL);
		return TCL_ERROR;
	}
	if ((allmatches==1)&&(allfiles!=0)) {
		Tcl_AppendResult(interp, "-allmatches and -allfiles not compatible", (char *) NULL);
		return TCL_ERROR;
	}
	if (argc == 2) {
		if (allfiles==1) goto wrongNumArgs;
		multiple=0;
		number=1;
		if (mode == REGEXP) {
			regexp=Tcl_RegExpCompile(interp, argPtr[1]);
			if (regexp==NULL) {
				return TCL_ERROR;
			}
		}
	} else if ((allfiles==1)&&(argc == 3)) {
		multiple=0;
		number=1;
		if (mode == REGEXP) {
			regexp=Tcl_RegExpCompile(interp, argPtr[1]);
			if (regexp==NULL) {
			return TCL_ERROR;
			}
		}
	} else if (argc > 2) {
		multiple=1;
		number=argc-1;
		if (allfiles==0) {
			if ((2*(number/2))!=number) {
				goto wrongNumArgs;
			}
		} else {
			if ((3*(number/3))!=number) {
				goto wrongNumArgs;
			}
		}
	} else {
		goto wrongNumArgs;
	}
	if (Tcl_SplitList(interp, argPtr[0], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}
	argPtr++;
	argc--;
	index = -1;

	if (multiple) {
		for(j=0;j<number;j+=step) {
			Tcl_SetVar(interp, argPtr[j+step-1], "", 0);
		}
	}
	for (i = 0; i < listArgc; i++) {
		Tcl_DString string;
		char *search,*cstring;
		file=Tcl_OpenFileChannel(interp,listArgv[i],"r",644);
		if (file==NULL) {
			return TCL_ERROR;
		}
		Tcl_DStringInit(&string);
		if (ExtraL_read_file(file,&string)==TCL_ERROR) {
			return TCL_ERROR;
		}
		cstring=Tcl_DStringValue(&string);
		search=cstring;
		for(j=0;j<number;j+=step) {
			int any=0;
			if (multiple) {
				regexp=Tcl_RegExpCompile(interp, argPtr[j]);
				if (regexp==NULL) {
					Tcl_DStringFree(&string);
					return TCL_ERROR;
				}
			}
			while(1) {
				match = 0;
				switch (mode) {
					case EXACT:
						match = (strstr(search, argPtr[j]) != NULL);
						break;
					case GLOB:
						match = Tcl_StringMatch(search, argPtr[j]);
						break;
					case REGEXP:
						match = Tcl_RegExpExec(interp, regexp, search, cstring);
						if (match < 0) {
							ckfree((char *) listArgv);
							return TCL_ERROR;
						}
						break;
				}
				if (match==0) {
					if (any==0) {
						if (allfiles!=0) {
							if (multiple==0) {
								Tcl_AppendElement(interp, argPtr[1]);
							} else {
								Tcl_SetVar(interp, argPtr[j+2], argPtr[j+1], TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
							}
						}
					}
					break;
				}
				any=1;
				if (multiple==0) {
					if (allfiles==0) Tcl_AppendElement(interp, listArgv[i]);
					if (matches == 1) {
						Tcl_RegExpRange(regexp, 1, &start, &end);
						if (start==NULL) {Tcl_AppendElement(interp, "");}
						else {
							char savedChar;
							savedChar = *end;
							*end = 0;
							Tcl_AppendElement(interp, start);
							*end = savedChar;
						}
					}
				} else {
					if (allfiles==0) Tcl_SetVar(interp, argPtr[j+1], listArgv[i], TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
					if (matches == 1) {
						Tcl_RegExpRange(regexp, 1, &start, &end);
						if (start==NULL) {
							Tcl_SetVar(interp, argPtr[j+step-1], "", TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
						}
						else {
							char savedChar;
							savedChar = *end;
							*end = 0;
							Tcl_SetVar(interp, argPtr[j+step-1], start, TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
							*end = savedChar;
						}
					}
				}
				if (allmatches==0) break;
				search = end;
			} /* while */
		}
		Tcl_DStringFree(&string);
		if (Tcl_UnregisterChannel(interp, file) != TCL_OK) {
			int len;
			ckfree((char *) listArgv);
			len = strlen(interp->result);
			if ((len > 0) && (interp->result[len - 1] == '\n')) {
				interp->result[len - 1] = '\0';
			}   
			return TCL_ERROR;
		}
		if (Tcl_Close(interp, file) != TCL_OK) {
			return TCL_ERROR;
		}
	}
	ckfree((char *) listArgv);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_AmanipCmd --
 *
 *		This procedure is invoked to process the "amanip" command.
 *		It manipulates arrays
 *		amanip option array ?arg ...?
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */
int
ExtraL_AmanipCmd(notUsed, interp, argc, argv)
	ClientData notUsed;					/* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int argc;						/* Number of arguments. */
	char **argv;					/* Argument strings. */
{
	int listArgc;
	char **listArgv;
	char *array;
	int c,len;
	int i;
	if (argc < 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" option ?arg ...?\"", (char *) NULL);
		return TCL_ERROR;
	}
	array=argv[2];
	c=argv[1][0];
	len=strlen(argv[1]);
	if ((c == 'l')&&(strncmp(argv[1], "lappend",len) == 0)) {
		if (argc != 4) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" lappend arrayName list\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[3], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}
		i=0;
		len=listArgc-1;
		while(i<len) {
			Tcl_SetVar2(interp, array, listArgv[i], listArgv[i+1], TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
			i+=2;
		}
		ckfree((char *) listArgv);
	} else if ((c == 'g')&&(strncmp(argv[1], "get",len) == 0)) {
/*
array set try {a 1 b 2 c 3 d 4 e 5 f 6}
amanip get try {a d g f}
amanip get try {a d g f} null
*/
		char *defval, *result;
		if ((argc != 4)&&(argc != 5)) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" get arrayName list ?alldefault?\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[3], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}
		if (argc==5) {
			defval=argv[4];
		}
		i=0;
		while(i<listArgc) {
			result=Tcl_GetVar2(interp,array,listArgv[i],0);
			if (result!=NULL) {
				if (argc!=5) Tcl_AppendElement(interp, listArgv[i]);
				Tcl_AppendElement(interp, result);
			} else if (argc==5) {
				Tcl_AppendElement(interp, defval);
			}
			i++;
		}
		ckfree((char *) listArgv);
	} else {
		Tcl_AppendResult(interp, "wrong option: should be:",
					 "lappend or get", (char *) NULL);
		return TCL_ERROR;
	}
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_AmanipObjCmd --
 *
 *		This procedure is invoked to process the "amanip" command.
 *		It manipulates arrays
 *		amanip option array ?arg ...?
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_AmanipObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int listobjc;
	Tcl_Obj **listobjv;
	char *string;
	int c,len;
	int i;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
		return TCL_ERROR;
	}
	string = Tcl_GetStringFromObj(objv[1],&len);
	c = string[0];
	if ((c == 'l')&&(strncmp(string,"lappend",len) == 0)) {
		if (objc != 4) {
			Tcl_WrongNumArgs(interp, 2, objv, "arrayName list");
			return TCL_ERROR;
		}
		if (Tcl_ListObjGetElements(interp, objv[3], &listobjc, &listobjv) != TCL_OK) {
			return TCL_ERROR;
		}
		i=0;
		len=listobjc-1;
		while(i<len) {
			Tcl_ObjSetVar2(interp, objv[2], listobjv[i], listobjv[i+1], TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
			i+=2;
		}
	} else if ((c == 'g')&&(strncmp(string,"get",len) == 0)) {
		/*
			array set try {a 1 b 2 c 3 d 4 e 5 f 6}
			amanip get try {a d g f}
			amanip get try {a d g f} null
		*/
		Tcl_Obj *defval, *result, *element;
		if ((objc != 4)&&(objc != 5)) {
			Tcl_WrongNumArgs(interp, 2, objv, "arrayName list ?alldefault?");
			return TCL_ERROR;
		}
		if (Tcl_ListObjGetElements(interp, objv[3], &listobjc, &listobjv) != TCL_OK) {
			return TCL_ERROR;
		}
		if (objc == 5) {
			defval = objv[4];
		}
		i = 0;
		result = Tcl_NewObj();
		while(i<listobjc) {
			element = Tcl_ObjGetVar2(interp,objv[2],listobjv[i],0);
			if (element != NULL) {
				if (objc!=5) Tcl_ListObjAppendElement(interp, result, listobjv[i]);
				Tcl_ListObjAppendElement(interp, result, element);
			} else if (objc==5) {
				Tcl_ListObjAppendElement(interp, result, defval);
			}
			i++;
		}
		Tcl_SetObjResult(interp,result);
		return TCL_OK;
	} else {
		Tcl_AppendResult(interp, "wrong option: should be: lappend or get", (char *) NULL);
		return TCL_ERROR;
	}
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_StringChangeObjCmd --
 *
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */
int
ExtraL_StringChangeObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *result;
	int listObjc;
	Tcl_Obj **listObjv;
	char *p,*lp;
	int plen,lplen,ppos,tppos,lppos,prev;
	int j;

	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "string changelist");
		return TCL_ERROR;
	}
	if (Tcl_ListObjGetElements(interp, objv[2], &listObjc, &listObjv) != TCL_OK) {
		return TCL_ERROR;
	}
	result = Tcl_NewObj();
	p = Tcl_GetStringFromObj(objv[1],&plen);
	ppos = 0;
	prev = 0;
	while (ppos < plen) {
		for(j=0;j<listObjc;j+=2) {
			lp = Tcl_GetStringFromObj(listObjv[j],&lplen);
			tppos = ppos;
			lppos = 0;
			while ((lppos < lplen)&&(tppos < plen)) {
				if (p[tppos] != lp[lppos]) break;
				tppos++;lppos++;
			}
			if (lppos == lplen) {
				if (prev != ppos) {
					Tcl_AppendToObj(result,p + prev,ppos - prev);
				}
				lp = Tcl_GetStringFromObj(listObjv[j+1],&lplen);
				Tcl_AppendToObj(result,lp,lplen);
				ppos = tppos-1;
				prev = tppos;
				break;
			}
		}
		ppos++;
	}
	if (ppos == plen) {
		Tcl_AppendToObj(result,p + prev,plen - prev);
	}
	Tcl_SetObjResult(interp,result);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_StringReplaceObjCmd --
 *
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */
int
ExtraL_StringReplaceObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	Tcl_Obj *result;
	char *replacement,*string;
	int i,error;
	int first,last,slen,rlen;
	/* */
	if (objc != 5) {
		Tcl_WrongNumArgs(interp, 1, objv, "string first last replacement");
		return TCL_ERROR;
	}
	error = Tcl_GetIntFromObj(interp, objv[2], &first);
	if (error) {return error;}
	error = Tcl_GetIntFromObj(interp, objv[3], &last);
	if (error) {return error;}
	last++;
	replacement = Tcl_GetStringFromObj(objv[4],&rlen);
	string = Tcl_GetStringFromObj(objv[1],&slen);
	result = Tcl_NewStringObj("",0);
	if (first < slen) {
		if (first+rlen < slen) {
			Tcl_AppendToObj(result, string, first);
			Tcl_AppendToObj(result, replacement, rlen);
			Tcl_AppendToObj(result, string+last, slen-last);
		} else {
			Tcl_AppendToObj(result, string, first);
			Tcl_AppendToObj(result, replacement, rlen);
		}
	} else {
		Tcl_AppendToObj(result, string, slen);
		for (i = slen + 1 ; i < first ; i++) {Tcl_AppendToObj(result, " ", 1);}
		Tcl_AppendToObj(result, replacement, rlen);
	}
	Tcl_SetObjResult(interp,result);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_SreverseObjCmd --
 *
 *		This procedure is invoked to process the "sreverse" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_SreverseObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	char *string, *result,*pos;
	int stringlen,i;
	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "string");
		return TCL_ERROR;
	}
	string = Tcl_GetStringFromObj(objv[1], &stringlen);
	result = (char *)Tcl_Alloc(stringlen*sizeof(char));
	pos = result;
	i = stringlen;
	for(i--;i >= 0;i--) {
		*pos++ = string[i];
	}
	Tcl_SetObjResult(interp,Tcl_NewStringObj(result,stringlen));
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_SfindCmd --
 *
 *		This procedure is invoked to process the "sfind" command.
 *		It finds all occurences of a pattern in a list, and returns
 *		their indexes as a list.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_SFindObjCmd(clientData, interp, objc, objv)
	ClientData clientData;	/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument values. */
{
#define EXACT	0
#define GLOB	1
#define REGEXP	2
	char *string;
	char *bytes, *patternBytes;
	int i, match, mode, result, length, stringlen;
	Tcl_Obj *indexObj, *resultObj;
	static char *switches[] =
		{"-exact", "-glob", "-regexp", (char *) NULL};
	mode = EXACT;
	if (objc == 4) {
		if (Tcl_GetIndexFromObj(interp, objv[1], switches,
			"search mode", 0, &mode) != TCL_OK) {
			return TCL_ERROR;
		}
	} else if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "?mode? string pattern");
		return TCL_ERROR;
	}
	string = Tcl_GetStringFromObj(objv[objc-2], &stringlen);
	patternBytes = Tcl_GetStringFromObj(objv[objc-1], &length);
 	Tcl_ResetResult(interp);
	resultObj = Tcl_GetObjResult(interp);
	for (i = 0; i < stringlen; i++) {
		match = 0;
		bytes = string+i;
		switch (mode) {
			case EXACT:
				if (i+length < stringlen) {
					match = (memcmp(bytes, patternBytes,(size_t) length) == 0);
				}
				break;
			case GLOB:
				/*
				 * WARNING: will not work with data containing NULLs.
				 */
				match = Tcl_StringMatch(bytes, patternBytes);
				break;
			case REGEXP:
				/*
				 * WARNING: will not work with data containing NULLs.
				 */
				match = Tcl_RegExpMatch(interp, bytes, patternBytes);
				if (match < 0) {
					return TCL_ERROR;
				}
				break;
		}
		if (match) {
			indexObj = Tcl_NewIntObj(i);
			result = Tcl_ListObjAppendElement(interp,resultObj,indexObj);
			if (result != TCL_OK) {return result;}
		}
	}
	Tcl_SetObjResult(interp,resultObj);
	return TCL_OK;
}
