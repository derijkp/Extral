/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LmanipCmd --
 *
 *		This procedure is invoked to process the "lmanip" command.
 *		It manipulates lists in all kinds of ways
 *		lmanip option list ?args ...?
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LmanipCmd(notUsed, interp, argc, argv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;				 /* Current interpreter. */
	int argc;								/* Number of arguments. */
	char **argv;						/* Argument strings. */
{
	int listArgc;
	char **listArgv;
	char *line=NULL;
	int *list;
	int c,len;
	int i;

	if (argc < 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" option ?args ...?\"", (char *) NULL);
		return TCL_ERROR;
	}
	c=argv[1][0];
	len=strlen(argv[1]);
	if ((c == 's')&&(strncmp(argv[1], "subindex",len) == 0)) {
		int listArgcres;
		char **listArgvres;
		int pos;
	
		if (argc != 4) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" subindex list ?pos?\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[3], &pos) != TCL_OK) {
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		i=0;
		while(i<listArgc) {
			line=listArgv[i];
			if (Tcl_SplitList(interp, line, &listArgcres, &listArgvres) != TCL_OK) {
				return TCL_ERROR;
			}
			if (pos<listArgcres) {Tcl_AppendElement(interp, listArgvres[pos]);}
			else {Tcl_AppendElement(interp, "");}
			ckfree((char *) listArgvres);
			i++;
		}
		ckfree((char *) listArgv);
	} else if ((c == 'm')&&(strncmp(argv[1], "merge",len) == 0)) {
		int listArgc2;
		char **listArgv2;
		Tcl_DString element;
		int i;
	
		if (argc != 4) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" merge list list\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[3], &listArgc2, &listArgv2) != TCL_OK) {
			return TCL_ERROR;
		}

		Tcl_DStringInit(&element);
		i=0;
		while(i<listArgc) {
			Tcl_DStringStartSublist(&element);
			Tcl_DStringAppendElement(&element, listArgv[i]);
			if (i<listArgc2) {
				Tcl_DStringAppendElement(&element, listArgv2[i]);
			}
			Tcl_DStringEndSublist(&element);
			i++;
		}
		ckfree((char *) listArgv);
		ckfree((char *) listArgv2);
		Tcl_DStringResult(interp,&element);
		Tcl_DStringFree(&element);
		return TCL_OK;
	} else if ((c == 'm')&&(strncmp(argv[1], "mangle",len) == 0)) {
		int listArgc2;
		char **listArgv2;
		int spacing=1;
		int i,j,k;
	
		if ((argc != 4)&&(argc != 5)) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" mangle list list ?spacing?\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (argc==5) {
			if (Tcl_GetInt(interp, argv[4], &spacing) != TCL_OK) {
				return TCL_ERROR;
			}
			if (spacing<=0) {
				Tcl_AppendResult(interp, "Spacing must be > 0", (char *) NULL);
				return TCL_ERROR;
			}
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[3], &listArgc2, &listArgv2) != TCL_OK) {
			ckfree((char *) listArgv);
			return TCL_ERROR;
		}

		i=0;
		j=0;
		while(i<listArgc) {
			for(k=0;k<spacing;k++) {
				Tcl_AppendElement(interp, listArgv[i]);
				i++;
				if (i==listArgc) break;
			}
			if (j<listArgc2) {
				Tcl_AppendElement(interp, listArgv2[j]);
				j++;
			}
		}
		ckfree((char *) listArgv);
		ckfree((char *) listArgv2);
		return TCL_OK;
	} else if ((c == 'u')&&(strncmp(argv[1], "unmangle",len) == 0)) {
		int spacing=1;
		int i,k;
	
		if ((argc < 3)||(argc > 5)) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" unmangle list ?spacing? ?var?\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (argc>3) {
			if (Tcl_GetInt(interp, argv[3], &spacing) != TCL_OK) {
				return TCL_ERROR;
			}
			if (spacing<=0) {
				Tcl_AppendResult(interp, "Spacing must be > 0", (char *) NULL);
				return TCL_ERROR;
			}
		}
		if (argv[2][0]=='\0') {
			Tcl_AppendResult(interp, "", (char *) NULL);
			if (argc==5) {Tcl_SetVar(interp, argv[4], "", 0);}
			return TCL_OK;
			}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		if (argc==5) {Tcl_SetVar(interp, argv[4], "", 0);}
		Tcl_AppendElement(interp, listArgv[0]);
		i=1;
		while(i<listArgc) {
			for(k=0;k<spacing;k++) {
			if (argc==5) {
					Tcl_SetVar(interp, argv[4], listArgv[i], TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
			}
			i++;
			if (i==listArgc) break;
			}
			if (i==listArgc) break;
			Tcl_AppendElement(interp, listArgv[i]);
			i++;
		}
		ckfree((char *) listArgv);
		return TCL_OK;
	} else if ((c == 'e')&&(strncmp(argv[1], "extract",len) == 0)) {
		regexp *regexpPtr;
		Tcl_DString element;
		char savedChar, *first, *last;
		int match = 0;
		int nr;
		int i;
	
		if (argc != 4) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" extract list expression\"", (char *) NULL);
			return TCL_ERROR;
		}
		 regexpPtr = (regexp *)Tcl_RegExpCompile(interp, argv[3]);
				 if (regexpPtr==NULL) {
			Tcl_AppendResult(interp, "error in regexp expression", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		nr=0;
		while(nr<listArgc) {
			line=listArgv[nr];
			match = Tcl_RegExpExec(interp,(Tcl_RegExp)regexpPtr, line, line);
			if (match == -1) {
				return TCL_ERROR;
			}
			if (!match) {
				Tcl_AppendElement(interp, "");
			} else {
			if (regexpPtr->startp[1]==NULL) {
				Tcl_AppendElement(interp,line);
			} else if (regexpPtr->startp[2]==NULL) {
				first = regexpPtr->startp[1];
				last = regexpPtr->endp[1];
				savedChar = *last;
				*last = '\0';
				Tcl_AppendElement(interp, first);
				*last = savedChar;
			} else {
				Tcl_DStringInit(&element);
				i=1;
				while (regexpPtr->startp[i]!=NULL) {
					first = regexpPtr->startp[i];
					last = regexpPtr->endp[i];
					savedChar = *last;
					*last = '\0';
					Tcl_DStringAppendElement(&element, first);
					*last = savedChar;
					i++;
				}
				Tcl_AppendElement(interp, Tcl_DStringValue(&element));
				Tcl_DStringFree(&element);
			}
			}
			nr++;
		}
		ckfree((char *) listArgv);
	} else if ((c == 'r')&&(strncmp(argv[1], "remdup",len) == 0)) {
		int i, j, dble;
	
		if (argc != 3) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" remdup list\"\n - returns the list in which duplicates are removed", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		i=0;
		while(i<listArgc) {
			line=listArgv[i];
			dble=0;
			j=0;
			while(j<i) {
				if (strcmp(line, listArgv[j])==0) dble=1;
			j++;
			}
			if (dble==0) {Tcl_AppendElement(interp, line);}
			i++;
		}
		ckfree((char *) listArgv);
	} else if ((c == 's')&&(strncmp(argv[1], "split",len) == 0)) {
		Tcl_DString element;
		char savedChar, *first, *last;
		PBOOL before;
		char *string;
		int *list;
		int number, nr;
		int i;
	
		if (argc != 5) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" split list -before/-after/-outside positions\"", (char *) NULL);
			return TCL_ERROR;
		}
		before=ExtraL_find_bool(argv[3],"-before","-after");
		list=ExtraL_get_intlist(interp, argv[4], &number, 0);
		if (list==NULL) {
			return TCL_ERROR;
		}

		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		nr=0;
		Tcl_DStringInit(&element);
		while(nr<listArgc) {
			i=0;
			while(i<number) {
			if (nr==list[i]) break;
			i++;
			}
			if (i<number) {
				if (before==true) {
				Tcl_AppendElement(interp, Tcl_DStringValue(&element));
				Tcl_DStringSetLength(&element, 0);
					Tcl_DStringAppendElement(&element, listArgv[nr]);
			} else if (before==false) {
					Tcl_DStringAppendElement(&element, listArgv[nr]);
				Tcl_AppendElement(interp, Tcl_DStringValue(&element));
				Tcl_DStringSetLength(&element, 0);
				nr++;
				if (nr<listArgc) Tcl_DStringAppendElement(&element, listArgv[nr]);
			} else {
				Tcl_AppendElement(interp, Tcl_DStringValue(&element));
				Tcl_DStringSetLength(&element, 0);
				nr++;
				if (nr<listArgc) Tcl_DStringAppendElement(&element, listArgv[nr]);
			}
			} else {
				Tcl_DStringAppendElement(&element, listArgv[nr]);
			}
			nr++;
		}
	string = Tcl_DStringValue(&element);
	if (string[0] != 0) {
		Tcl_AppendElement(interp, string);
	}
		Tcl_DStringFree(&element);
		ckfree((char *) listArgv);
	} else if ((c == 'j')&&(strncmp(argv[1], "join",len) == 0)) {
		Tcl_DString element;
		char savedChar, *first, *last;
		char *string, *join;
		int *list;
		int number=1, elements=0;
		int nr,every=0;
		int i,count;
	
		if ((argc != 5)&&(argc != 6)) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" join list joinstring ?-every? positions\"", (char *) NULL);
			return TCL_ERROR;
		}
		join=argv[3];
		if ((argc == 6)&&(strcmp(argv[4], "-every") == 0)) {
			list=NULL;
			if (Tcl_GetInt(interp, argv[5], &every) != TCL_OK) {
				return TCL_ERROR;
			}
		} else {
			if (strcmp(argv[4], "all") == 0) {
				list=NULL;
			} else {
				list=ExtraL_get_intlist(interp, argv[4], &number, 0);
				if (list==NULL) {
				return TCL_ERROR;
				}
			}
		}
	
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}
		
		nr=0;
		Tcl_DStringInit(&element);
		
		i=0;
		count=1;
		while(nr<listArgc) {
			Tcl_DStringAppend(&element, listArgv[nr], -1);
			if (list!=NULL) {
				i=0;
				while(i<number) {
					if (nr==list[i]) break;
					i++;
				}
			} else {
				if (every==0) {
					i=0;
				} else {
					count--;
					if (count==0) {i=0;count=every;} else {i=number+1;}
				}
			}
			nr++;
			if (i>=number) {
				Tcl_AppendElement(interp, Tcl_DStringValue(&element));
				elements++;
				Tcl_DStringSetLength(&element, 0);
			} else {
				Tcl_DStringAppend(&element, join, -1);
			}
		}
		string = Tcl_DStringValue(&element);
		if (string[0] != 0) {
			if (elements!=0) Tcl_AppendElement(interp, string);
			else Tcl_DStringResult(interp, &element);
		}
		Tcl_DStringFree(&element);
		ckfree((char *) listArgv);
	} else if ((c == 'l')&&(strncmp(argv[1], "lengths",len) == 0)) {
		int i;
		char val[30];
	
		if (argc != 3) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" lengths list\"\n - returns a list of the lengths", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		i=0;
		while(i<listArgc) {
			sprintf(val, "%d", strlen(listArgv[i]));
			Tcl_AppendElement(interp, val);
			i++;
		}
		ckfree((char *) listArgv);
	} else if ((c == 'f')&&(strncmp(argv[1], "fill",len) == 0)) {
		char var[30];
		int size,start,incr;
		int i;
		
		if ((argc != 4)&&(argc != 5)) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" fill size start ?incr?\n - fills a list with the value in start, can be incremented by ?incr?\"\n", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[2], &size) != TCL_OK) {
			return TCL_ERROR;
		}
		if ((argc==4)||(strlen(argv[4])==0)) {
			for(i=0;i<size;i++) {
				Tcl_AppendElement(interp, argv[3]);
			}
		}else {
			if (Tcl_GetInt(interp, argv[3], &start) != TCL_OK) {
				return TCL_ERROR;
			}
			if (Tcl_GetInt(interp, argv[4], &incr) != TCL_OK) {
				return TCL_ERROR;
			}
			for(i=0;i<size;i++) {
				sprintf(var,"%d",start);
				Tcl_AppendElement(interp, var);
				start+=incr;
			}
		}
		return TCL_OK;
	} else if ((c == 'f')&&(strncmp(argv[1], "ffill",len) == 0)) {
		char var[30];
		double start,incr;
		int size,i;
		
		if ((argc != 4)&&(argc != 5)) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" fill size start ?incr?\n - fills a list with the floating value in start, can be incremented by ?incr?\"\n", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[2], &size) != TCL_OK) {
			return TCL_ERROR;
		}
		if ((argc==4)||(strlen(argv[4])==0)) {
			for(i=0;i<size;i++) {
				Tcl_AppendElement(interp, argv[3]);
			}
		}else {
			if (Tcl_GetDouble(interp, argv[3], &start) != TCL_OK) {
				return TCL_ERROR;
			}
			if (Tcl_GetDouble(interp, argv[4], &incr) != TCL_OK) {
				return TCL_ERROR;
			}
			for(i=0;i<size;i++) {
				sprintf(var,"%g",start);
				Tcl_AppendElement(interp, var);
				start+=incr;
			}
		}
		return TCL_OK;
/*
	} else if ((c == 'r')&&(strncmp(argv[1], "replace",len) == 0)) {
		int i, j, dble;
	
		if (argc != 3) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" replace list table\"\n - replaces elements in a list", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		i=0;
		while(i<listArgc) {
			line=listArgv[i];
			dble=0;
			j=0;
			while(j<i) {
				if (strcmp(line, listArgv[j])==0) dble=1;
			j++;
			}
			if (dble==0) {Tcl_AppendElement(interp, line);}
			i++;
		}
		ckfree((char *) listArgv);
*/
	} else {
		Tcl_AppendResult(interp, "wrong option: should be:",
					 "subindex, merge, extract, remdup, split, join, lengths, fill, ffill, mangle, unmangle", (char *) NULL);
		return TCL_ERROR;
	}
	return TCL_OK;
}

/*
wish
extinit extraL
lmanip group { ab cd {ef gh} {kl mn} op {qr st}} -after {2 4}
set try {10:11 20-21 30:31}
lmanip extract $try {([0-9]*):([0-9]*)}


set try {{a1 a2} {b1 b2} {c1 c2}}
set try2 {abcde sdfhsfh dfgabcfdg}
lmanip extract $try2 {[ ^]?(.*abc.*)[$ ]?}
lmanip merge $try {a3 b3 c3 c4}
lmanip subindex $try 0
lmanip merge [lmanip subindex $try 1] [lmanip subindex $try 0]
lmanip remdup {1 3 2 5 6 3}
lmanip remdup {aa bc aa b d b}
*/
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LmathCmd --
 *
 *		This procedure is invoked to process the "lmath" command.
 *		It calculates things from lists in all kinds of ways
 *		lmath option list ?arg ...?
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LmathCmd(notUsed, interp, argc, argv)
	ClientData notUsed;					/* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int argc;						/* Number of arguments. */
	char **argv;					/* Argument strings. */
{
	int listArgc;
	char **listArgv;
	char val[TCL_DOUBLE_SPACE];
	int *list;
	int c,len;
	int i;

	if (argc < 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" option list ?arg ...?\"", (char *) NULL);
		return TCL_ERROR;
	}
	c=argv[1][0];
	len=strlen(argv[1]);
	if ((c == 's')&&(strncmp(argv[1], "sum",len) == 0)) {
		int listArgcres;
		char **listArgvres;
		double value, sum=0;
	
		if (argc != 3) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" sum list\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		i=0;
		while(i<listArgc) {
			if (Tcl_GetDouble(interp, listArgv[i], &value) != TCL_OK) {
			return TCL_ERROR;
			}
			sum+=value;
			i++;
		}
		Tcl_PrintDouble(interp,sum,val);
		Tcl_AppendResult(interp, val, (char *) NULL);
		ckfree((char *) listArgv);
	} else if ((c == 'm')&&(strncmp(argv[1], "max",len) == 0)) {
		int listArgcres;
		char **listArgvres;
		double value, max;
	
		if (argc != 3) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" max list\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		i=0;
		if (Tcl_GetDouble(interp, listArgv[i], &max) != TCL_OK) {
			return TCL_ERROR;
		}
		while(i<listArgc) {
			if (Tcl_GetDouble(interp, listArgv[i], &value) != TCL_OK) {
			return TCL_ERROR;
			}
			if (value>max) max=value;
			i++;
		}
		Tcl_PrintDouble(interp,max,val);
		Tcl_AppendResult(interp, val, (char *) NULL);
		ckfree((char *) listArgv);
	} else if ((c == 'm')&&(strncmp(argv[1], "min",len) == 0)) {
		int listArgcres;
		char **listArgvres;
		double value, min;
	
		if (argc != 3) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" min list\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		i=0;
		if (Tcl_GetDouble(interp, listArgv[i], &min) != TCL_OK) {
			return TCL_ERROR;
		}
		while(i<listArgc) {
			if (Tcl_GetDouble(interp, listArgv[i], &value) != TCL_OK) {
			return TCL_ERROR;
			}
			if (value<min) min=value;
			i++;
		}
		Tcl_PrintDouble(interp,min,val);
		Tcl_AppendResult(interp, val, (char *) NULL);
		ckfree((char *) listArgv);
	} else if ((c == 'c')&&(strncmp(argv[1], "cumul",len) == 0)) {
		int listArgcres;
		char **listArgvres;
		double value, current=0;
	
		if (argc != 3) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" cumul list\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		i=0;
		if (Tcl_GetDouble(interp, listArgv[i], &value) != TCL_OK) {
			return TCL_ERROR;
		}
		while(i<listArgc) {
			if (Tcl_GetDouble(interp, listArgv[i], &value) != TCL_OK) {
			return TCL_ERROR;
			}
			current+=value;
			Tcl_PrintDouble(interp,current,val);
			Tcl_AppendElement(interp, val);
			i++;
		}
		ckfree((char *) listArgv);
	} else if ((c == 'i')&&(strncmp(argv[1], "incr", len) == 0)) {
		int listArgcres;
		char **listArgvres;
		double value, incr=0;
	
		if (argc != 4) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" incr list value\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_GetDouble(interp, argv[3], &incr) != TCL_OK) {
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		i=0;
		if (Tcl_GetDouble(interp, listArgv[i], &value) != TCL_OK) {
			return TCL_ERROR;
		}
		while(i<listArgc) {
			if (Tcl_GetDouble(interp, listArgv[i], &value) != TCL_OK) {
			return TCL_ERROR;
			}
			value+=incr;
			Tcl_PrintDouble(interp,value,val);
			Tcl_AppendElement(interp, val);
			i++;
		}
		ckfree((char *) listArgv);
	} else if ((c == 'c')&&(strncmp(argv[1], "calc", len) == 0)) {
		int listArgc2;
		char **listArgv2;
		int what;
		double value1, value2,value;
	
		if (argc != 5) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" calc list action(+-*/) list\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}
		what=argv[3][0];
		switch(what) {
			case '+': break;
			case '-': break;
			case '*': break;
			case '/': break;
			default : Tcl_AppendResult(interp, "action must be +, -, * or /", (char *) NULL);
					return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[4], &listArgc2, &listArgv2) != TCL_OK) {
			ckfree((char *) listArgv);
			return TCL_ERROR;
		}

		if (listArgc2<listArgc) {
			Tcl_AppendResult(interp, "second list shorter", (char *) NULL);
		return TCL_ERROR;
		}
		i=0;
		while(i<listArgc) {
			if (Tcl_GetDouble(interp, listArgv[i], &value1) != TCL_OK) {
			return TCL_ERROR;
			}
			if (Tcl_GetDouble(interp, listArgv2[i], &value2) != TCL_OK) {
			return TCL_ERROR;
			}
			switch(what) {
				case '+': value=value1+value2;break;
				case '-': value=value1-value2;break;
				case '*': value=value1*value2;break;
				case '/': value=value1/value2;break;
			}
			Tcl_PrintDouble(interp, value, val);
			Tcl_AppendElement(interp, val);
			i++;
		}
		ckfree((char *) listArgv);
		ckfree((char *) listArgv2);
/*
	} else if ((c == 'e')&&(strncmp(argv[1], "expr", len) == 0)) {
		int listArgcres;
		char **listArgvres;
		double value;
		char val[TCL_DOUBLE_SPACE];
	
		if (argc != 3) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" expr list\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
			return TCL_ERROR;
		}

		while(i<listArgc) {
			Tcl_ExprDouble(interp, listArgv[i], &value);
			Tcl_PrintDouble(interp, value, val);
			Tcl_AppendElement(interp, val);
			i++;
		}
		ckfree((char *) listArgv);
*/
	} else {
		Tcl_AppendResult(interp, "wrong option: should be:",
					 "sum, min, max, cumul, incr, calc", (char *) NULL);
		return TCL_ERROR;
	}
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LregsubCmd --
 *
 *		This procedure is invoked to process the "lregsub" Tcl command.
 *
 * Results:
 *		A standard Tcl result.
 *
 * Side effects:
 *		See the user documentation.
 *
 * lregsub {c$} {abc acb dc} {e} 
 * lregsub {^a$} {g u k fsgh aasdf a fgh daa a} {}
 * lregsub {\.exe$} {a.try help.exe dd.txt dd.exe} {.sh}
 * lregsub {^([^ ]+) ([^ ]+)$} {{a 1} {b 2} {c 3}} {\1->\2}
 *
 *----------------------------------------------------------------------
 */

		/* ARGSUSED */
int
ExtraL_LregsubCmd(dummy, interp, argc, argv)
	ClientData dummy;					/* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int argc;						/* Number of arguments. */
	char **argv;					/* Argument strings. */
{
	int listArgc;
	char **listArgv;
	int noCase = 0, all = 0;
	Tcl_RegExp regExpr;
	char *fullstring, *string, *pattern, *p, *firstChar, *newValue, **argPtr;
	int match, flags, code=TCL_OK, numMatches;
	char *start, *end, *subStart, *subEnd;
	register char *src, c;
	Tcl_DString stringDString, patternDString, resultDString;
	int i;

	if (argc < 4) {
		wrongNumArgs:
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" ?switches? expr list subSpec\"", (char *) NULL);
		return TCL_ERROR;
	}
	argPtr = argv+1;
	argc--;
	while (argPtr[0][0] == '-') {
		if (strcmp(argPtr[0], "-nocase") == 0) {
			noCase = 1;
		} else if (strcmp(argPtr[0], "-all") == 0) {
			all = 1;
		} else if (strcmp(argPtr[0], "--") == 0) {
			argPtr++;
			argc--;
			break;
		} else {
			Tcl_AppendResult(interp, "bad switch \"", argPtr[0],
				"\": must be -all, -nocase, or --", (char *) NULL);
			return TCL_ERROR;
		}
		argPtr++;
		argc--;
	}
	if (argc != 3) {
		goto wrongNumArgs;
	}

	/*
	 * Convert the string and pattern to lower case, if desired.
	 */

	if (noCase) {
		Tcl_DStringInit(&patternDString);
		Tcl_DStringAppend(&patternDString, argPtr[0], -1);
		pattern = Tcl_DStringValue(&patternDString);
		for (p = pattern; *p != 0; p++) {
			if (isupper(UCHAR(*p))) {
			*p = tolower(*p);
			}
		}
		Tcl_DStringInit(&stringDString);
		Tcl_DStringAppend(&stringDString, argPtr[1], -1);
		fullstring = Tcl_DStringValue(&stringDString);
		for (p = fullstring; *p != 0; p++) {
			if (isupper(UCHAR(*p))) {
			*p = tolower(*p);
			}
		}
	} else {
		pattern = argPtr[0];
		fullstring = argPtr[1];
	}
	if (Tcl_SplitList(interp, fullstring, &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}
	Tcl_DStringInit(&resultDString);

	regExpr = Tcl_RegExpCompile(interp, pattern);
	if (regExpr == NULL) {
		code = TCL_ERROR;
		goto enddone;
	}

	for (i = 0; i < listArgc; i++) {
		Tcl_DStringSetLength(&resultDString,0);
		string=listArgv[i];
		/*
		 * The following loop is to handle multiple matches within the
		 * same source string;	each iteration handles one match and its
		 * corresponding substitution.	If "-all" hasn't been specified
		 * then the loop body only gets executed once.
		 */

		flags = 0;
		numMatches = 0;
		for (p = string; *p != 0; ) {
			match = Tcl_RegExpExec(interp, regExpr, p, string);
			if (match < 0) {
				code = TCL_ERROR;
				goto done;
			}
			if (!match) {
				break;
			}
			numMatches += 1;

			/*
			 * Copy the portion of the source string before the match to the
			 * result variable.
			 */

			Tcl_RegExpRange(regExpr, 0, &start, &end);
			src = listArgv[i] + (start - string);
			c = *src;
			*src = 0;
			Tcl_DStringAppend(&resultDString, listArgv[i] + (p - string), -1);
			*src = c;
			flags = TCL_APPEND_VALUE;
		
			/*
			 * Append the subSpec argument to the variable, making appropriate
			 * substitutions.	This code is a bit hairy because of the backslash
			 * conventions and because the code saves up ranges of characters in
			 * subSpec to reduce the number of calls to Tcl_SetVar.
			 */
		
			for (src = firstChar = argPtr[2], c = *src; c != 0; src++, c = *src) {
				int index;
		
				if (c == '&') {
					index = 0;
				} else if (c == '\\') {
					c = src[1];
					if ((c >= '0') && (c <= '9')) {
						index = c - '0';
					} else if ((c == '\\') || (c == '&')) {
						*src = c;
						src[1] = 0;
						Tcl_DStringAppend(&resultDString, firstChar, -1);
						*src = '\\';
						src[1] = c;
						firstChar = src+2;
						src++;
						continue;
					} else {
						continue;
					}
				} else {
					continue;
				}
				if (firstChar != src) {
					c = *src;
					*src = 0;
					Tcl_DStringAppend(&resultDString, firstChar, -1);
					*src = c;
				}
				Tcl_RegExpRange(regExpr, index, &subStart, &subEnd);
				if ((subStart != NULL) && (subEnd != NULL)) {
					char *first, *last, saved;
		
					first = listArgv[i] + (subStart - string);
					last = listArgv[i] + (subEnd - string);
					saved = *last;
					*last = 0;
					Tcl_DStringAppend(&resultDString, first, -1);
					*last = saved;
				}
				if (*src == '\\') {
					src++;
				}
				firstChar = src+1;
			}
			if (firstChar != src) {
				Tcl_DStringAppend(&resultDString, firstChar, -1);
			}
			if (end == p) {
				char tmp[2];

				/*
				 * Always consume at least one character of the input string
				 * in order to prevent infinite loops.
				 */

				tmp[0] = listArgv[i][p - string];
				tmp[1] = 0;
				Tcl_DStringAppend(&resultDString, tmp, -1);
				p = end + 1;
			} else {
				p = end;
			}
			if (!all) {
				break;
			}
		}

		/*
		 * Copy the portion of the source string after the last match to the
		 * result variable.
		 */

		if ((*p != 0) || (numMatches == 0)) {
			Tcl_DStringAppend(&resultDString, listArgv[i] + (p - string), -1);
		}
		done:
		Tcl_AppendElement(interp,Tcl_DStringValue(&resultDString));
	}

	enddone:
	if (noCase) {
		Tcl_DStringFree(&stringDString);
		Tcl_DStringFree(&patternDString);
	}
	Tcl_DStringFree(&resultDString);
	ckfree((char *) listArgv);
	return code;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LreadFileCmd --
 *
 *		This procedure is invoked to process the "lreadfile" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

#define BUFFERSIZE 512

int
ExtraL_LreadFileObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	FILE *file;
	char *fileName, *nativeName, *new;
	Tcl_DString buffer;
	Tcl_Obj *lineObj, *listObj;
	char fbuffer[BUFFERSIZE];
	int length, pos, ch, err;

	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "filename");
		return TCL_ERROR;
	}
	fileName = Tcl_GetStringFromObj(objv[1], &length);

	nativeName = Tcl_TranslateFileName(interp, fileName, &buffer);
	if (nativeName == NULL) {
		return TCL_ERROR;
	}
	file = fopen(nativeName, "r");

    /*
     * If nativeName is not NULL, the buffer is valid and we must free
     * the storage.
     */
    
    Tcl_DStringFree(&buffer);

	listObj = Tcl_NewListObj(0,NULL);
	lineObj = Tcl_NewStringObj("",0);
	pos=0;
	while(1) {
		ch = fgetc(file);
		if (ch=='\\') {
			ch = fgetc(file);
			if (ch=='n') {
				fbuffer[pos++] = '\n';
			} else if (ch=='\\') {
				fbuffer[pos++] = '\\';
			} else {
				fbuffer[pos++] = '\\';
				fbuffer[pos++] = ch;
			}
		} else if (ch==EOF) {
			Tcl_AppendToObj(lineObj,fbuffer,pos);
			err = Tcl_ListObjAppendElement(interp,listObj,Tcl_DuplicateObj(lineObj));
			if (err) {Tcl_DecrRefCount(lineObj);return TCL_ERROR;}
			break;
		} else if (ch=='\n') {
			Tcl_AppendToObj(lineObj,fbuffer,pos);
			err = Tcl_ListObjAppendElement(interp,listObj,Tcl_DuplicateObj(lineObj));
			if (err) {Tcl_DecrRefCount(lineObj);return TCL_ERROR;}
			Tcl_SetObjLength(lineObj,0);
			pos=0;
		} else {
			if (pos==BUFFERSIZE) {
				Tcl_AppendToObj(lineObj,fbuffer,BUFFERSIZE);
				pos=0;
			}
			fbuffer[pos++] = ch;
		}
	}
	Tcl_DecrRefCount(lineObj);
	Tcl_SetObjResult(interp,listObj);
	fclose(file);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LwriteFileCmd --
 *
 *		This procedure is invoked to process the "lwritefile" command.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

#define BUFFERSIZE 512

int
ExtraL_LwriteFileObjCmd(notUsed, interp, objc, objv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int objc;						/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	FILE *file;
	char *fileName, *nativeName;
	Tcl_Obj **listArgv;
	int listArgc;
	Tcl_DString buffer;
	char *string;
	int length, pos, i, err;

	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "filename list");
		return TCL_ERROR;
	}
	fileName = Tcl_GetStringFromObj(objv[1], &length);

	nativeName = Tcl_TranslateFileName(interp, fileName, &buffer);
	if (nativeName == NULL) {
		return TCL_ERROR;
	}

	file = fopen(nativeName, "w");

    /*
     * If nativeName is not NULL, the buffer is valid and we must free
     * the storage.
     */

    Tcl_DStringFree(&buffer);

	if (Tcl_ListObjGetElements(interp, objv[2], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}

	for(pos=0;pos<listArgc;pos++) {
		string = Tcl_GetStringFromObj(listArgv[pos],&length);
		i=0;
		while(i<length) {
			if (string[i]=='\\') {
				fputc('\\',file);
				fputc('\\',file);
			} else if (string[i]=='\n') {
				fputc('\\',file);
				fputc('n',file);
			} else {
				fputc(string[i],file);
			}
			i++;
		}
		if (pos!=(listArgc-1)) {fputc('\n',file);}
	}
	fclose(file);
	return TCL_OK;
}
