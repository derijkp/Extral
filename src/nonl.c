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
/*
ffind -regexp -matches -allmatches [glob ../test/*] "\nt2:(\[^\n\]*)\n"
ffind -regexp -matches -allfiles null [glob ../test/*] "\nt2:(\[^\n\]*)\n"
ffind -regexp [glob ../test/*] "\nt2:(\[^\n\]*)\n"
*/
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
 * ExtraL_RandomCmd --
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_RandomCmd(notUsed, interp, argc, argv)
	ClientData notUsed;					/* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int argc;							 	/* Number of arguments. */
	char **argv;						 /* Argument strings. */
{
	FILE *file;
	char *string=NULL;
	long number;
	int deler;
	int result;
	int min, max;
	char *resultstring;

	if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" min max\"", (char *) NULL);
		return TCL_ERROR;
	}
	if (Tcl_GetInt(interp, argv[1], &min) != TCL_OK) {
		return TCL_ERROR;
	}
	if (Tcl_GetInt(interp, argv[2], &max) != TCL_OK) {
		return TCL_ERROR;
	}
	if (max<=min) {
		Tcl_AppendResult(interp, "wrong arguments", (char *) NULL);
		return TCL_ERROR;
	}
	number=rand();
	deler=RAND_MAX/(max-min);
	result=number/deler;
	if (result<min) {
		Tcl_AppendResult(interp, "Something strange happened: Result too small", (char *) NULL);
		return TCL_ERROR;
	}
	if (result>max) {
		Tcl_AppendResult(interp, "Something strange happened: Result too big", (char *) NULL);
		return TCL_ERROR;
	}
	resultstring=ExtraL_numstr(result+min);
	Tcl_AppendResult(interp, resultstring, (char *) NULL);
	free(resultstring);
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
	char *line=NULL;
	char *array;
	int *list;
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
	if ((c == 'a')&&(strncmp(argv[1], "append",len) == 0)) {
		if (argc != 4) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" append arrayName list\"", (char *) NULL);
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
					 "append", (char *) NULL);
		return TCL_ERROR;
	}
	return TCL_OK;
}
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_ReplaceCmd --
 *
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

		/* ARGSUSED */
int
ExtraL_ReplaceCmd(notUsed, interp, argc, argv)
	ClientData notUsed;						/* Not used. */
	Tcl_Interp *interp;						/* Current interpreter. */
	int argc;								/* Number of arguments. */
	char **argv;						/* Argument strings. */
{
	Tcl_DString result;
	int listArgc;
	char **listArgv;
	char *string,*p,*lp,*tp;
	int i,j,pos,len;

	Tcl_DStringInit(&result);

	if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" string replacelist\"", (char *) NULL);
		return TCL_ERROR;
	}
	if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}
	p=argv[1];
	while (*p!='\0') {
		for(j=0;j<listArgc;j+=2) {
			lp=listArgv[j];
			tp=p;
			while ((*lp!='\0')&tp!='\0') {
				if (*tp!=*lp) break;
				tp++;lp++;
			}
			if (*lp=='\0') {
				Tcl_DStringAppend(&result,listArgv[j+1],-1);
				p=tp;
				if (*tp=='\0') {
					Tcl_DStringResult(interp,&result);
					return TCL_OK;
				}
			}
		}
		Tcl_DStringAppend(&result,p,1);
		p++;
	}
	Tcl_DStringResult(interp,&result);
	return TCL_OK;
}
