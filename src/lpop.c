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
#include "tclInt.h"
#include "tcl.h"
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LpopCmd --
 *
 *		This procedure is invoked to process the "lpop" command.
 *		It pops an item out of a list
 *
 * Results:
 *		A standard Tcl result: the popped out elemeny.
 *
 *
 *----------------------------------------------------------------------
 */

		/* ARGSUSED */
int
ExtraL_LpopCmd(notUsed, interp, argc, argv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int argc;						/* Number of arguments. */
	char **argv;					    /* Argument strings. */
{
	int listArgc;
	char **listArgv;
	int start,end,pos;
    char *p, *element, savedChar, *next, *string;
    int index, size, parenthesized, result, returnLast;

	if ((argc < 2)||(argc > 3)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				 " list ?pos?\"", (char *) NULL);
		return TCL_ERROR;
	}
	
	string=Tcl_GetVar(interp,argv[1],TCL_LEAVE_ERR_MSG);
	if (string == NULL) {
	    return TCL_ERROR;
	}
    if ((argc<3)||((*argv[2] == 'e') && (strncmp(argv[2], "end", strlen(argv[2])) == 0))) {
		returnLast = 1;
		index = INT_MAX;
    } else {
		returnLast = 0;
		if (Tcl_GetInt(interp, argv[2], &index) != TCL_OK) {
		    Tcl_ResetResult(interp);
		    Tcl_AppendResult(interp, "bad index \"", argv[2],
			    "\": must be integer or \"end\"", (char *) NULL);
		    return TCL_ERROR;
		}
    }
    if (index < 0) {
		index = 0;
    }

    size = 0;
    element = string;
    for (p = string; index >= 0; index--) {
		result = TclFindElement(interp, p, &element, &next, &size, &parenthesized);
		if (result != TCL_OK) {return result;}
		if ((*next == 0) && returnLast) {break;}
		p = next;
    }
    if ((returnLast==0)&&(*next == 0)&&(index != -1)) {
		Tcl_AppendResult(interp, "list doesn't contain element ",argv[2], (char *) NULL);
		return TCL_ERROR;
    }

    if (size == 0) {
		return TCL_OK;
    }
    if (size >= TCL_RESULT_SIZE) {
		interp->result = (char *) ckalloc((unsigned) size+1);
		interp->freeProc = TCL_DYNAMIC;
    }
    if (parenthesized) {
		memcpy((VOID *) interp->result, (VOID *) element, (size_t) size);
		interp->result[size] = 0;
		element--;
    } else {
		TclCopyAndCollapse(size, element, interp->result);
    }

	if (element[-1]=='"') element--;
    /*
     * Add the first part to the list.
     */

    *element = 0;
	if (Tcl_SetVar(interp, argv[1], string,TCL_LEAVE_ERR_MSG)==NULL) {
		return TCL_ERROR;
	}

    /*
     * Append the remainder of the original list.
     */

    if (*next != 0) {
			Tcl_SetVar(interp, argv[1], next,TCL_APPEND_VALUE);
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LshiftCmd --
 *
 *		This procedure is invoked to process the "lshift" command.
 *		It pops the first item out of a list
 *
 * Results:
 *		A standard Tcl result: the popped out elemeny.
 *
 *
 *----------------------------------------------------------------------
 */

		/* ARGSUSED */
int
ExtraL_LshiftCmd(notUsed, interp, argc, argv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int argc;						/* Number of arguments. */
	char **argv;					    /* Argument strings. */
{
	int listArgc;
	char **listArgv;
	int start,end,pos;
    char *element, savedChar, *next, *string;
    int size, parenthesized, result, returnLast;

	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				 " list\"", (char *) NULL);
		return TCL_ERROR;
	}
	
	string=Tcl_GetVar(interp,argv[1],TCL_LEAVE_ERR_MSG);
	if (string == NULL) {
	    return TCL_ERROR;
	}

    size = 0;
    element = string;
	result = TclFindElement(interp, string, &element, &next, &size, &parenthesized);
	if (result != TCL_OK) {return result;}
    if (size == 0) {
		return TCL_OK;
    }
    if (size >= TCL_RESULT_SIZE) {
		interp->result = (char *) ckalloc((unsigned) size+1);
		interp->freeProc = TCL_DYNAMIC;
    }
    if (parenthesized) {
		memcpy((VOID *) interp->result, (VOID *) element, (size_t) size);
		interp->result[size] = 0;
		element--;
    } else {
		TclCopyAndCollapse(size, element, interp->result);
    }

    /*
     * Append the remainder of the original list.
     */

	if (Tcl_SetVar(interp, argv[1], next,TCL_LEAVE_ERR_MSG)==NULL) {
		return TCL_ERROR;
	}
    return TCL_OK;
}
