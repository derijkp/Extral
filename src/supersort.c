/*
 * The important part of this code originates from the lsort function posted
 * by D. Richard Hipp -- drh@tobit.vnet.net -- 704.948.4565
 * -reflist option and building into Extral by Peter De Rijk
 */

#include "tcl.h"

/*
 * During execution of the "lsort" command, a Tcl list is represented as
 * a linked list of the following structures.
 */

typedef struct SortElement {
	char *reftextPtr;                   /* Text of the ref list element */
	char *textPtr;                      /* Text of the list element */
	struct SortElement *nextPtr;        /* Next element on the list */
} SortElement;

/*
 * The "lsort" command needs to pass certain information down to the
 * function that compares two list elements, and the comparison function
 * needs to pass success or failure information back up to the top-level
 * "lsort" command.  The following structure is used to pass this
 * information.
 */

typedef struct SortInfo {
	int isIncreasing;                   /* True to sort in increasing order */
	int sortMode;                       /* The sort mode.  One of SORTMODE_???
	                                     * values defined below */
	Tcl_DString compareCmd;             /* The Tcl comparison command when
	                                     * sortMode==COMMAND */
	Tcl_Interp *interp;                 /* The interpreter running the sort */
	int resultCode;                     /* If compareCmd every fails, this
	                                     * is changed from TCL_OK to 
	                                     * TCL_ERROR */
} SortInfo;

/*
 * The "sortMode" field of the SortInfo structure can take on any of the
 * following values.
 */

#define SORTMODE_ASCII      0
#define SORTMODE_INTEGER    1
#define SORTMODE_REAL       2
#define SORTMODE_COMMAND    3
#define SORTMODE_DICTIONARY 4

/*
 * Forward declarations for procedures defined in this file:
 */


static SortElement *    MergeSort _ANSI_ARGS_((SortElement*, SortInfo*));
static SortElement *    MergeLists _ANSI_ARGS_((SortElement*,SortElement*,
                            SortInfo*));
static int		SortCompareProc _ANSI_ARGS_((char *first,
			    char *second, SortInfo *));
static int              DictionaryCompare _ANSI_ARGS_((char*,char*));

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_SsortCmd --
 *
 *	This procedure is invoked to process the "lsort" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_SSortCmd(notUsed, interp, argc, argv)
	ClientData notUsed;			/* Not used. */
	Tcl_Interp *interp;			/* Current interpreter. */
	int argc;				/* Number of arguments. */
	char **argv;			/* Argument strings. */
{
	int listArgc, i, c;
	int reflistArgc=0;
	size_t length;
	char **listArgv;
	char **reflistArgv=NULL;
	char *reflist=NULL;
	char *command = NULL;		/* Initialization needed only to
					 * prevent compiler warning. */
	SortElement *elementArray;
	SortElement *elementPtr;
	SortInfo sortInfo;                  /* Information about this sort that
	                                     * needs to be passed to the 
	                                     * comparison function */

	if (argc < 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" ?-ascii? ?-integer? ?-real? ?-increasing? ?-decreasing? ?-dictionary?",
			" ?-command string? ?-reflist list? list\"", (char *) NULL);
		return TCL_ERROR;
	}

	/*
	 * Parse arguments to set up the mode for the sort.
	 */

	sortInfo.interp = interp;
	sortInfo.sortMode = SORTMODE_ASCII;
	sortInfo.isIncreasing = 1;
	sortInfo.resultCode = TCL_OK;
	for (i = 1; i < argc-1; i++) {
		length = strlen(argv[i]);
		if (length < 2) {
			badSwitch:
			Tcl_AppendResult(interp, "bad switch \"", argv[i],
				"\": must be -ascii, -integer, -real, -increasing,",
				" -decreasing, -dictionary -reflist or -command", (char *) NULL);
			sortInfo.resultCode = TCL_ERROR;
			goto done;
		}
		c = argv[i][1];
		if ((c == 'a') && (strncmp(argv[i], "-ascii", length) == 0)) {
			sortInfo.sortMode = SORTMODE_ASCII;
		} else if ((c == 'c') && (strncmp(argv[i], "-command", length) == 0)) {
			if (i == argc-2) {
				Tcl_AppendResult(interp, "\"-command\" must be",
					" followed by comparison command", (char *) NULL);
				sortInfo.resultCode = TCL_ERROR;
				goto done;
			}
			sortInfo.sortMode = SORTMODE_COMMAND;
			command = argv[i+1];
			i++;
		} else if ((c == 'r') && (strncmp(argv[i], "-reflist", length) == 0)) {
			if (i == argc-2) {
				Tcl_AppendResult(interp, "\"-reflist\" must be",
					" followed by a list", (char *) NULL);
				sortInfo.resultCode = TCL_ERROR;
				goto done;
			}
			reflist = argv[i+1];
			i++;
		} else if ((c == 'd')
			&& (strncmp(argv[i], "-decreasing", length) == 0)) {
			sortInfo.isIncreasing = 0;
		} else if ((c == 'd') && (length >= 4)
			&& (strncmp(argv[i], "-dictionary", length) == 0)) {
			sortInfo.sortMode = SORTMODE_DICTIONARY;
		} else if ((c == 'i') && (length >= 4)
			&& (strncmp(argv[i], "-increasing", length) == 0)) {
			sortInfo.isIncreasing = 1;
		} else if ((c == 'i') && (length >= 4)
			&& (strncmp(argv[i], "-integer", length) == 0)) {
			sortInfo.sortMode = SORTMODE_INTEGER;
		} else if ((c == 'r')
			&& (strncmp(argv[i], "-real", length) == 0)) {
			sortInfo.sortMode = SORTMODE_REAL;
		} else {
			goto badSwitch;
		}
	}
	if (sortInfo.sortMode == SORTMODE_COMMAND) {
		Tcl_DStringInit(&sortInfo.compareCmd);
		Tcl_DStringAppend(&sortInfo.compareCmd, command, -1);
	}

	if (Tcl_SplitList(interp, argv[argc-1], &listArgc, &listArgv) != TCL_OK) {
		sortInfo.resultCode = TCL_ERROR;
		goto done;
	}

	if (reflist!=NULL) {
		if (Tcl_SplitList(interp, reflist, &reflistArgc, &reflistArgv) != TCL_OK) {
			sortInfo.resultCode = TCL_ERROR;
			goto done;
		}
	}

	elementArray = (SortElement*)ckalloc( listArgc*sizeof(SortElement) );
	for(i=0; i<listArgc; i++){
		if (reflist!=NULL) {
			elementArray[i].reftextPtr = reflistArgv[i];
		} else {
			elementArray[i].reftextPtr = listArgv[i];
		}
		elementArray[i].textPtr = listArgv[i];
		elementArray[i].nextPtr = &elementArray[i+1];
	}
	elementArray[listArgc-1].nextPtr = 0;
	elementPtr = MergeSort(elementArray,&sortInfo);
	for(i=0; elementPtr; i++, elementPtr = elementPtr->nextPtr){
		listArgv[i] = elementPtr->textPtr;
	}
	ckfree((char*) elementArray);
	if (sortInfo.resultCode == TCL_OK) {
		Tcl_ResetResult(interp);
		interp->result = Tcl_Merge(listArgc, listArgv);
		interp->freeProc = TCL_DYNAMIC;
	}
	if (sortInfo.sortMode == SORTMODE_COMMAND) {
		Tcl_DStringFree(&sortInfo.compareCmd);
	}
	ckfree((char *) listArgv);
	ckfree((char *) reflistArgv);

	done:
	return sortInfo.resultCode;
}

/*
 *----------------------------------------------------------------------
 *
 * MergeSort -
 *
 *	This procedure sorts a linked list of SortElement structures
 *	use the merge-sort algorithm.
 *
 * Results:
 *      A pointer to the head of the list after sorting is returned.
 *
 * Side effects:
 *	None, unless a user-defined comparison command does something
 *	weird.
 *
 *----------------------------------------------------------------------
 */

static SortElement *
MergeSort(headPtr, infoPtr)
    SortElement *headPtr;               /* First element on the list */
    SortInfo *infoPtr;                  /* Information needed by the
                                         * comparison operator */
{
#   define N_SUBLIST 30
    SortElement *subList[N_SUBLIST];
    SortElement *elementPtr;
    int i;

    for(i=0; i<N_SUBLIST; i++){
        subList[i] = 0;
    }
    while( headPtr ){
        elementPtr = headPtr;
        headPtr = headPtr->nextPtr;
        elementPtr->nextPtr = 0;
        for(i=0; (i < N_SUBLIST) && (subList[i] != 0); i++){
           elementPtr = MergeLists(elementPtr, subList[i], infoPtr);
           subList[i] = 0;
        }
        if (i >= N_SUBLIST) {
          subList[N_SUBLIST-1] = MergeLists(elementPtr, 
                   subList[N_SUBLIST-1], infoPtr);
        } else {
          subList[i] = elementPtr;
        }
    }
    elementPtr = 0;
    for(i=0; i < N_SUBLIST; i++){
        elementPtr = MergeLists(elementPtr, subList[i], infoPtr);
    }
    return elementPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * MergeLists -
 *
 *	This procedure combines two sorted lists of SortElement structures
 *	into a single sorted list.
 *
 * Results:
 *      The unified list of SortElement structures.
 *
 * Side effects:
 *	None, unless a user-defined comparison command does something
 *	weird.
 *
 *----------------------------------------------------------------------
 */

static SortElement *
MergeLists(leftPtr, rightPtr, infoPtr)
    SortElement *leftPtr;               /* First list to be merged */
    SortElement *rightPtr;              /* Second list to be merged */
    SortInfo *infoPtr;                  /* Information needed by the
                                         * comparison operator */
{
    SortElement *headPtr;
    SortElement *tailPtr;

    if (leftPtr == 0) {
        headPtr = rightPtr;
    } else if (rightPtr == 0) {
        headPtr = leftPtr;
    } else {
        if (SortCompareProc(leftPtr->reftextPtr, rightPtr->reftextPtr, infoPtr) < 0) {
            tailPtr = leftPtr;
            leftPtr = leftPtr->nextPtr;
        } else {
            tailPtr = rightPtr;
            rightPtr = rightPtr->nextPtr;
        }
        headPtr = tailPtr;
        while ((leftPtr != 0) && (rightPtr != 0)) {
            if (SortCompareProc(leftPtr->reftextPtr, rightPtr->reftextPtr,
                                infoPtr) < 0) {
                tailPtr->nextPtr = leftPtr;
                tailPtr = leftPtr;
                leftPtr = leftPtr->nextPtr;
            } else {
                tailPtr->nextPtr = rightPtr;
                tailPtr = rightPtr;
                rightPtr = rightPtr->nextPtr;
            }
        }
        if (leftPtr != 0) {
           tailPtr->nextPtr = leftPtr;
        } else {
           tailPtr->nextPtr = rightPtr;
        }
    }
    return headPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * SortCompareProc --
 *
 *	This procedure is invoked by MergeLists to determine the proper
 *	ordering between two elements.
 *
 * Results:
 *      A negative results means the the first element comes before the
 *      second, and a positive results means that the second element
 *      should come first.  A result of zero means the two elements
 *      are equal and it doesn't matter which comes first.
 *
 * Side effects:
 *	None, unless a user-defined comparison command does something
 *	weird.
 *
 *----------------------------------------------------------------------
 */

static int
SortCompareProc(firstString, secondString, infoPtr)
    char *firstString, *secondString;	/* Elements to be compared. */
    SortInfo *infoPtr;                  /* Information passed from the
                                         * top-level "lsort" command */
{
    int order;

	order = 0;
	if (infoPtr->resultCode != TCL_OK) {
		/*
		 * Once an error has occurred, skip any future comparisons
		 * so as to preserve the error message in sortInterp->result.
		 */
	
		return order;
	}
	if (infoPtr->sortMode == SORTMODE_ASCII) {
		order = strcmp(firstString, secondString);
	} else if (infoPtr->sortMode == SORTMODE_DICTIONARY) {
		order = DictionaryCompare(firstString, secondString);
	} else if (infoPtr->sortMode == SORTMODE_INTEGER) {
		int a, b;
	
		if ((Tcl_GetInt(infoPtr->interp, firstString, &a) != TCL_OK)
			|| (Tcl_GetInt(infoPtr->interp, secondString, &b) != TCL_OK)) {
			Tcl_AddErrorInfo(infoPtr->interp,
				"\n    (converting list element from string to integer)");
			infoPtr->resultCode = TCL_ERROR;
			return order;
		}
		if (a > b) {
			order = 1;
		} else if (b > a) {
			order = -1;
		}
	} else if (infoPtr->sortMode == SORTMODE_REAL) {
		double a, b;
	
		if ((Tcl_GetDouble(infoPtr->interp, firstString, &a) != TCL_OK)
			  || (Tcl_GetDouble(infoPtr->interp, secondString, &b) != TCL_OK)) {
			Tcl_AddErrorInfo(infoPtr->interp,
				"\n    (converting list element from string to real)");
			infoPtr->resultCode = TCL_ERROR;
			return order;
		}
		if (a > b) {
			order = 1;
		} else if (b > a) {
			order = -1;
		}
	} else {
		int oldLength;
		char *end;
	
		/*
		 * Generate and evaluate a command to determine which string comes
		 * first.
		 */
	
		oldLength = Tcl_DStringLength(&infoPtr->compareCmd);
		Tcl_DStringAppendElement(&infoPtr->compareCmd, firstString);
		Tcl_DStringAppendElement(&infoPtr->compareCmd, secondString);
		infoPtr->resultCode = Tcl_Eval(infoPtr->interp, Tcl_DStringValue(&infoPtr->compareCmd));
		Tcl_DStringTrunc(&infoPtr->compareCmd, oldLength);
		if (infoPtr->resultCode != TCL_OK) {
			Tcl_AddErrorInfo(infoPtr->interp,
				"\n    (user-defined comparison command)");
			return order;
		}
	
		/*
		 * Parse the result of the command.
		 */
	
		order = strtol(infoPtr->interp->result, &end, 0);
		if ((end == infoPtr->interp->result) || (*end != 0)) {
			Tcl_ResetResult(infoPtr->interp);
			Tcl_AppendResult(infoPtr->interp,
				"comparison command returned non-numeric result",
				(char *) NULL);
			infoPtr->resultCode = TCL_ERROR;
			return order;
		}
	}
	if (!infoPtr->isIncreasing) {
		order = -order;
	}
	return order;
}

/*
 *----------------------------------------------------------------------
 *
 * DictionaryCompare
 *
 *	This function compares two strings as if they were being used in
 *	an index or card catalog.  The case of alphabetic characters is
 *	ignored, except to break ties.  Thus "B" comes before "b" but
 *	after "a".  Also, integers embedded in the strings compare in
 *	numerical order.  In other words, "x10y" comes after "x9y", not
 *      before it as it would when using strcmp().
 *
 * Results:
 *      A negative results means the the first element comes before the
 *      second, and a positive results means that the second element
 *      should come first.  A result of zero means the two elements
 *      are equal and it doesn't matter which comes first.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
DictionaryCompare(left, right)
    char *left, *right;          /* The strings to compare */
{
    int diff;
    int secondaryDiff = 0;

    while (1) {
        diff = *left - *right;
        if (diff) {
            if (isupper(*left) && islower(*right)) {
                diff = tolower(*left) - *right;
                if (diff) {
                   return diff;
                } else if (secondaryDiff == 0) {
                   secondaryDiff = -1;
                }
            } else if (isupper(*right) && islower(*left)) {
                diff = *left - tolower(*right);
                if (diff) {
                   return diff;
                } else if (secondaryDiff == 0) {
                   secondaryDiff = 1;
                }
            } else if (isdigit(*right) && isdigit(*left)) {
                int rightCnt = 0;
                int leftCnt = 0;
                while (isdigit(*right)) {
                   rightCnt++;
                   right++;
                }
                while (isdigit(*left)) {
                   leftCnt++;
                   left++;
                }
                return (rightCnt != leftCnt) ? leftCnt - rightCnt : diff;
            } else {
                return diff;
            }
        }
        if (*left == 0) break;
        left++;
        right++;
    }
    if (diff == 0) diff = secondaryDiff;
    return diff;
}
