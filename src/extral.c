/*	
 *	 File:		extral.c
 *	 Purpose: extraL extension to Tcl
 *	 Author:	Copyright (c) 1995 Peter De Rijk
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "tcl.h"
#include "tclRegexp.h"
#define UCHAR(c) ((unsigned char) (c))
 
typedef enum {false,true,other} PBOOL;

extern Tcl_RegExp Tcl_RegExpCompile(Tcl_Interp *interp,char *string);

typedef struct {
	int number;
	long *list;
} INDEX;

typedef struct {
	FILE *file;
	INDEX index;
	char *sep;
	PBOOL before;
} Lfile_index;

typedef struct {
	int line;
	int begin,end;
	regexp *regexpPtr;
} Lposition;

	PBOOL Compile_L_pos(Tcl_Interp *interp,Lposition *L_pos, char *positions);
	PBOOL get_L_field(Tcl_Interp *interp,Lfile_index *LfilePtr,Lposition *L_pos,int org);
	void ref_saveto(FILE *file,Lfile_index *LfilePtr,int org);

/* These should go in a general file later */
/*------------------------------------------------------------------*/
PBOOL find_bool(char *string, char *trues,char *falses)
{
	if (strcmp(string,trues)==0) return(true);
	else if (strcmp(string,falses)==0) return(false);
	else return(other);
}
/*------------------------------------------------------------------*/
char *numstr(int num)
{
	char *result=NULL;
	int number;
	int i;

	i=1;
	number=10;
	while(num>=number) {
		 number*=10;
		 i++;
	}
	result=(char *)malloc((i+1)*sizeof(char));
	sprintf(result, "%*d", i,	num);
	return(result);
}
/*--------------------------------------------------------------------*/
PBOOL read_string(FILE *file,char *place,int size)
{
	int l;
	clearerr(file);
	if (size==0) {
		place[0]='\0';
		return(true);
	}
	if (fgets(place,size+1,file)==NULL) return(false);
	l=strlen(place)-1;
	if (place[l]=='\n') {
		place[l]='\0';
		return(other);
	} 
	else return(true);
}
/*--------------------------------------------------------------------*/
int *get_intlist(Tcl_Interp *interp, char *string, int *number, int min)
{
	int *list;
	int listArgc;
	char **listArgv;
	int i;
	int temp;

	if (Tcl_SplitList(interp, string, &listArgc, &listArgv) != TCL_OK) {
		return(NULL);
	}

	/*---- get index list ----*/
	*number=listArgc;
	list=(int *)malloc(listArgc*sizeof(int));
	if (list==NULL) {
		ckfree((char *) listArgv);
		Tcl_AppendResult(interp, "Couldn't allocate memory", (char *) NULL);
		return(NULL);
	}
	i=0;
	while(i<listArgc) {
		if (Tcl_GetInt(interp, listArgv[i], &temp) != TCL_OK) {
			ckfree((char *) listArgv);
			free(list);
			return(NULL);
		}
		if (temp<min) {
			Tcl_AppendResult(interp, "Index ", listArgv[i], "too small!",	(char *) NULL);
			ckfree((char *) listArgv);
			free(list);
			return(NULL);
		}
	list[i]=temp;
	i++;
	}
	ckfree((char *) listArgv);
	return(list);
}
/*------------------------------------------------------------------*/
PBOOL skip_lines(FILE *file,int number)
{
	int i;
	int ch;
	if (number==0) return(true);
	for (i=0;i<number;i++) {
		do {
			ch=getc(file);
			} while ((ch!='\n')&&(ch!=EOF));
		}
		if (ch==EOF) {
		clearerr(file);
		return(false);
	}
	return(true);
}
/*--------------------------------------------------------------------*/
char *read_line(FILE *file)
{
	PBOOL b;
	char buffer[501];
	char *result=NULL, *temp;
	int bl=0;

	do {
		b=read_string(file,buffer,500);
		if (b==false) break;
		if (b==true) {
			temp=(char *)realloc(result,((bl+1)*500+1)*sizeof(char));
		}
		else {
			temp=(char *)realloc(result,(bl*500+strlen(buffer)+1)*sizeof(char));
		}
		if (temp==NULL) {
			free(result);
			return(NULL);
		}
		result=temp;
		strcpy(result+(bl*500),buffer);
		bl++;
	} while (b==true);
	return(result);
}
/*--------------------------------------------------------------------*/
#define FILEBUFFER 1000
char *read_file(FILE *file)
{
	PBOOL b;
	char *result=NULL, *place, *temp;
	int i, r, bl=0;
	int end;

	clearerr(file);
	result=(char *)malloc((FILEBUFFER+1)*sizeof(char));
	place=result;
	while(1) {
		for(i=0;i<FILEBUFFER;i++) {
			r=getc(file);
			if (r==EOF) break;
			if (r==0) break;
			*place=r;
			place++;
		}
		if (i!=FILEBUFFER) break;
		bl++;
		temp=(char *)realloc(result,((bl+1)*FILEBUFFER+1)*sizeof(char));
		if (temp==NULL) {free(result);return(NULL);} else {result=temp;}
		place=result+bl*FILEBUFFER;
	} 
	end=bl*FILEBUFFER+i;
	result=(char *)realloc(result,(end+2)*sizeof(char));
	result[end]='\0';
	return(result);
}
/*--------------------------------------------------------------------*/
void DeleteLfile(ClientData clientData) {
	Lfile_index *LfilePtr = (Lfile_index *) clientData;

	fclose(LfilePtr->file);
	free(LfilePtr->index.list);
	free(LfilePtr->sep);
	free(LfilePtr);
}
/*------------------------------------------------------------------*/
int Lfile_ObjectCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]) {
	Lfile_index *LfilePtr = (Lfile_index *) clientData;
	char resultstring[10];
	int c;

	if (argc < 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				argv[0], " option ?arg arg ...?\"", (char *) NULL);
		return TCL_ERROR;
	}

	c=argv[1][0];
	if ((c == 'n') && (strcmp(argv[1], "number") == 0)) {
		sprintf(resultstring,"%d",LfilePtr->index.number);
		Tcl_AppendResult(interp, resultstring, (char *) NULL);
		return TCL_OK;
	} else if ((c == 'c')&&(strcmp(argv[1], "close") == 0)) {
		Tcl_DeleteCommand(interp, argv[0]);
		return TCL_OK;
	} else if ((c == 'g')&&(strcmp(argv[1], "get") == 0)) {
		Lposition L_pos;
		INDEX ind_ref;
		PBOOL all=true;
		FILE *file;
		int *list;
		int rnumber,number,org,temp,begin;
		int i;
		
		ind_ref=LfilePtr->index;
		if ((argc != 4)&&(argc != 5)) {
			Tcl_AppendResult(interp, "wrong # args: should be \"",
				argv[0], " get line:begin-end?|regexp? ?modifier:-all/-exclude? ??indexlist??\"", (char *) NULL);
			return TCL_ERROR;
		}

		all=find_bool(argv[3],"-all","-exclude");
		if (all==other) {begin=3;} else {begin=4;}
		if (all!=true) {
			list=get_intlist(interp, argv[begin], &number, 0);
			if (list==NULL) {
				return TCL_ERROR;
			}
		}
		
		if (Compile_L_pos(interp,&L_pos, argv[2])==false) {return TCL_ERROR;}
		if (all==other) {
			i=0;
			while(i<number) {
				if (get_L_field(interp,LfilePtr,&L_pos,list[i])==false) {
					free(list);
					return TCL_ERROR;
				}
				i++;
			}
		} else {
			rnumber=ind_ref.number;
			org=0;
			while (org<rnumber) {
				if (all==false) {
					i=0;
					while(i<number) {
						if (org==list[i]) break;
						i++;
					}
					if (i<number) {org++;continue;}
				}
				if (get_L_field(interp,LfilePtr,&L_pos,org)==false) {
					free(list);
					return TCL_ERROR;
				}
				org++;
			}
		}
		if (all!=true) free(list);
		return TCL_OK;
	} else {
		Tcl_AppendResult(interp, "wrong option: should be:",
			"number, get, close", (char *) NULL);
		return TCL_ERROR;
	}
}
/*------------------------------------------------------------------*/
	PBOOL index_L_file(Tcl_Interp *interp,FILE *file,INDEX *index,char *sep,PBOOL before)
	{
	int ch;
	long ind;

	index->number=1;
	index->list=(long *)malloc(sizeof(long));
	if (index->list==NULL) {return(false);}
	index->list[0]=ftell(file);
	if (sep==NULL) {
		while (1) {
		ch=fgetc(file);
		if (ch==EOF) break;
		if (ch=='\n') {
			ind=ftell(file);
			index->list=(long *)realloc(index->list,(index->number+1)*sizeof(long));
			if (index->list==NULL) {return(false);}
			index->list[index->number]=ind;
			index->number++;
			}
		}
		index->number--;
	} else {
		char *line;
		while (1) {
		if (before==true) {ind=ftell(file);}
		line=read_line(file);
		if (line==NULL) break;
		if (Tcl_RegExpMatch(interp, line, sep)==1) {
			if (before==false) {ind=ftell(file);}
			index->list=(long *)realloc(index->list,(index->number+1)*sizeof(long));
			if (index->list==NULL) {return(false);}
			index->list[index->number]=ind;
			index->number++;
			}
		}
	}
	return(true);
	}
/*------------------------------------------------------------------*/
int ExtraL_LfileCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]) {
	Lfile_index *LfilePtr;

	if ((argc != 3)&&(argc != 5)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				argv[0], " pathName fileName ?-before/-after seperator?\"", (char *) NULL);
		return TCL_ERROR;
	}
	Tcl_VarEval(interp,"info commands ",argv[1],(char *)NULL);
	if (strcmp(interp->result,"")!=0) {
		Tcl_AppendResult(interp, " exists already", (char *) NULL);
		return TCL_ERROR;
	}

	LfilePtr=(Lfile_index *)malloc(sizeof(Lfile_index));
	LfilePtr->sep=NULL;
	if (argc==5) {
		LfilePtr->before=find_bool(argv[3],"-before","-after");
		if (LfilePtr->before==other) {
			Tcl_AppendResult(interp, "option should be -before or -after", (char *) NULL);
			return TCL_ERROR;
		}
		LfilePtr->sep=strdup(argv[4]);
	}

	LfilePtr->file=fopen(argv[2],"r");
	if (LfilePtr->file==NULL) {
		free(LfilePtr);
		interp->result = "Couldn't open file";
		return TCL_ERROR;
	}

	if (index_L_file(interp,LfilePtr->file,&(LfilePtr->index),LfilePtr->sep,LfilePtr->before)==false) {
		free(LfilePtr);
		fclose(LfilePtr->file);
		interp->result = "Couldn't index EMBL file";
		return TCL_ERROR;
	}
	interp->result = argv[1];
	Tcl_CreateCommand(interp, interp->result, Lfile_ObjectCmd, (ClientData) LfilePtr,DeleteLfile);
	return TCL_OK;
}
/*------------------------------------------------------------------*/
	PBOOL Compile_L_pos(Tcl_Interp *interp,Lposition *L_pos, char *positions)
	{
	char *regpattern=NULL;
	char *pos=NULL;
	int begin,end;
	int i;

	regpattern=strchr(positions,'|');
	if (regpattern!=NULL) {
		regpattern[0]='\0';
		regpattern++;
	}

	pos=strtok(positions,":-,");
	if (pos==NULL) {
		Tcl_AppendResult(interp, "error in line specifier", (char *) NULL);
		return(false);
	}
	L_pos->line=atol(pos);

	pos=strtok(NULL,":-,");
	if (pos==NULL) {
		Tcl_AppendResult(interp, "error in begin specifier", (char *) NULL);
		return(false);
	}
	L_pos->begin=atol(pos);

	pos=strtok(NULL,":-,");
	if (pos==NULL) {
		Tcl_AppendResult(interp, "error in end specifier", (char *) NULL);
		return(false);
	}
	if (strcmp(pos,"end")==0) {
		L_pos->end=-1;
	} else {
		L_pos->end=atol(pos);
	}
	
	if (regpattern==NULL) {
		L_pos->regexpPtr=NULL;
	} else {
		L_pos->regexpPtr = (regexp *)Tcl_RegExpCompile(interp, regpattern);
		if (L_pos->regexpPtr == NULL) {
			Tcl_AppendResult(interp, "error in regexp specifier", (char *) NULL);
			return(false);
		}
	}
	return(true);
	}
/*------------------------------------------------------------------*/
	PBOOL get_L_field(Tcl_Interp *interp,Lfile_index *LfilePtr,Lposition *L_pos,int org)
	{
	FILE *f_ref=LfilePtr->file;
	INDEX ind_ref=LfilePtr->index;
	regexp *regexpPtr=L_pos->regexpPtr;
	char *tline;
	char *result=NULL;
	int match = 0;
	int begin,end,len;
	int i;

	if ((org<0)||(org>=ind_ref.number)) {
		Tcl_AppendElement(interp, "number out of range");
		return(false);
	}

	fseek(f_ref,ind_ref.list[org],SEEK_SET);
	skip_lines(LfilePtr->file,L_pos->line);
	tline=read_line(f_ref);
	if (tline==NULL) {
		Tcl_AppendElement(interp, "beyond EOF");
		return(false);
	}

	begin=L_pos->begin;
	end=L_pos->end;
	len=strlen(tline);
	if (begin>len) {begin=len;}
	if (end>len) {end=len;}
	if (end==-1) {
		end=len;
	}
	tline[end]='\0';
	result=tline+begin;
	
	if (regexpPtr==NULL) {
		Tcl_AppendElement(interp, result);
	} else {
		match = Tcl_RegExpExec(interp,(Tcl_RegExp)regexpPtr, result, result);
		if (match == -1) {
			return(false);
		}
		if (!match) {
			Tcl_AppendElement(interp, "");
			return(true);
		} else {
			Tcl_DString element;
			char savedChar, *first, *last;
			if (regexpPtr->startp[1]==NULL) {
				Tcl_AppendElement(interp,result);
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
	}
	free(tline);
	return(true);
	}
/*------------------------------------------------------------------*/
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LfindCmd --
 *
 *		This procedure is invoked to process the "lfind" command.
 *		It finds all occurences of a pattern in a list, and returns
 *		their indexes as a list.
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

		/* ARGSUSED */
int
ExtraL_LfindCmd(notUsed, interp, argc, argv)
	ClientData notUsed;						/* Not used. */
	Tcl_Interp *interp;						/* Current interpreter. */
	int argc;								/* Number of arguments. */
	char **argv;						/* Argument strings. */
{
#define EXACT		0
#define GLOB		1
#define REGEXP		2
	int listArgc;
	char **listArgv;
	char *line=NULL;
	int i, match, mode, index;

	mode = GLOB;
	if (argc == 4) {
		if (strcmp(argv[1], "-exact") == 0) {
			mode = EXACT;
		} else if (strcmp(argv[1], "-glob") == 0) {
			mode = GLOB;
		} else if (strcmp(argv[1], "-regexp") == 0) {
			mode = REGEXP;
		} else {
			Tcl_AppendResult(interp, "bad search mode \"", argv[1],
					"\": must be -exact, -glob, or -regexp", (char *) NULL);
			return TCL_ERROR;
		}
	} else if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" ?mode? list pattern\"", (char *) NULL);
		return TCL_ERROR;
	}
	if (Tcl_SplitList(interp, argv[argc-2], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}
	index = -1;
	for (i = 0; i < listArgc; i++) {
		match = 0;
		switch (mode) {
			case EXACT:
				match = (strcmp(listArgv[i], argv[argc-1]) == 0);
				break;
			case GLOB:
				match = Tcl_StringMatch(listArgv[i], argv[argc-1]);
				break;
			case REGEXP:
				match = Tcl_RegExpMatch(interp, listArgv[i], argv[argc-1]);
				if (match < 0) {
					ckfree((char *) listArgv);
					return TCL_ERROR;
				}
				break;
		}
		if (match) {
			line=numstr(i);
			Tcl_AppendElement(interp, line);
			free(line);
		}
	}
	ckfree((char *) listArgv);
	return TCL_OK;
}
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
	FILE *file=NULL;
	char **argPtr;
	int listArgc;
	char **listArgv;
	char *allfiles;
	int matches, allmatches, multiple, number;
	int i, j, match, mode, index;

	if (argc < 4) {
		wrongNumArgs:
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" ?switches? fileList pattern ?varName pattern varName ...?\"", (char *) NULL);
		return TCL_ERROR;
	}
	argPtr = argv+1;
	argc--;
	matches = 0;
	allmatches=0;
	allfiles=NULL;
	multiple = 0;
	mode = EXACT;
	while ((argc > 0) && (argPtr[0][0] == '-')) {
		if (strcmp(argPtr[0], "-matches") == 0) {
			matches = 1;
		} else if (strcmp(argPtr[0], "-allmatches") == 0) {
			allmatches = 1;
		} else if (strcmp(argPtr[0], "-allfiles") == 0) {
			argPtr++;
			argc--;
			if (argc==0) break;
			allfiles = argPtr[0];
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
	if ((allfiles!=NULL)&&(matches==0)) {
		Tcl_AppendResult(interp, "-allfiles only applicable with -matches options", (char *) NULL);
		return TCL_ERROR;
	}
	if ((allmatches==1)&&(allfiles!=NULL)) {
		Tcl_AppendResult(interp, "-allmatches and -allfiles not compatible", (char *) NULL);
		return TCL_ERROR;
	}
	if (argc == 2) {
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
		if ((2*(number/2))!=number) {
			goto wrongNumArgs;
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
		for(j=0;j<number;j+=2) {
			Tcl_SetVar(interp, argPtr[j+1], "", 0);
		}
	}
	for (i = 0; i < listArgc; i++) {
		char *string, *search;
		file=fopen(listArgv[i],"r");
		if (file==NULL) {
			fclose(file);
			free(file);
			Tcl_AppendResult(interp, "Couldn't open file ", listArgv[i], (char *) NULL);
			return TCL_ERROR;
		}
		string=read_file(file);
		if (string==NULL) {
			fclose(file);
			free(file);
			continue;
		}
		search=string;
		for(j=0;j<number;j+=2) {
			if (multiple) {
				regexp=Tcl_RegExpCompile(interp, argPtr[j]);
				if (regexp==NULL) {
					free(string);
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
						match = Tcl_RegExpExec(interp, regexp, search, string);
						if (match < 0) {
							ckfree((char *) listArgv);
							return TCL_ERROR;
						}
						break;
				}
				if (match==0) {
					if (allfiles!=NULL) {
/*						Tcl_AppendElement(interp, listArgv[i]); */
						Tcl_AppendElement(interp, allfiles);
					}
					break;
				}
				if (multiple==0) {
					if (allfiles==NULL) Tcl_AppendElement(interp, listArgv[i]);
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
					if (allfiles==NULL) Tcl_SetVar(interp, argPtr[j+1], listArgv[i], TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
					if (matches == 1) {
						Tcl_RegExpRange(regexp, 1, &start, &end);
						if (start==NULL) {
							Tcl_SetVar(interp, argPtr[j+1], "", TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
						}
						else {
							char savedChar;
							savedChar = *end;
							*end = 0;
							Tcl_SetVar(interp, argPtr[j+1], start, TCL_APPEND_VALUE|TCL_LIST_ELEMENT);
							*end = savedChar;
						}
					}
				}
				if (allmatches==0) break;
				search = end;
			} /* while */
		}
		free(string);
		fclose(file);
	}
	ckfree((char *) listArgv);
	return TCL_OK;
}
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LsubCmd --
 *
 *		This procedure is invoked to process the "lsub" command.
 *		It creates a subset of a list
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

		/* ARGSUSED */
int
ExtraL_LsubCmd(notUsed, interp, argc, argv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;				 /* Current interpreter. */
	int argc;							 /* Number of arguments. */
	char **argv;						 /* Argument strings. */
{
#define INCLUDE		0
#define EXCLUDE		1
	int listArgc;
	char **listArgv;
	char *line=NULL;
	int *list;
	int match,mode,index,number,begin;
	int i,j;

	if ((argc != 3)&&(argc != 4)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" list ?-exclude? indexlist\"", (char *) NULL);
		return TCL_ERROR;
	}

	mode = INCLUDE;
	if (strcmp(argv[2], "-exclude") == 0) {
		mode = EXCLUDE;
		begin=3;
	} else {
		begin=2;
	}

	list=get_intlist(interp, argv[begin], &number, -1);
	if (list==NULL) {
		return TCL_ERROR;
	}

	/*---- do the extraction ----*/
	if (Tcl_SplitList(interp, argv[1], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}

	if (mode==INCLUDE) {
		for(i=0;i<number;i++) {
			if ((list[i] > -1)&&(list[i]<listArgc)) {
				Tcl_AppendElement(interp, listArgv[list[i]]);
			}
		}
	} else {
		index = -1;
		for (i = 0; i < listArgc; i++) {
			match = 1;
			for(j=0;j<number;j++) {
				if (list[j]<listArgc) {
					if (i==list[j]) {match=0;break;}
				}
			}
			if (match) {
				Tcl_AppendElement(interp, listArgv[i]);
			}
		}
	}
	free(list);
	ckfree((char *) listArgv);
	return TCL_OK;
}
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LcorCmd --
 *
 *		This procedure is invoked to process the "lcor" command.
 *		It creates a subset of a list
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

		/* ARGSUSED */
int
ExtraL_LcorCmd(notUsed, interp, argc, argv)
	ClientData notUsed;				 /* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int argc;						/* Number of arguments. */
	char **argv;					/* Argument strings. */
{
#define INCLUDE		0
#define EXCLUDE		1
	int refArgc;
	char **refArgv;
	int listArgc;
	char **listArgv;
	char **item=NULL;
	char res[10];
	int pos;
	int i;

	if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				 " referencelist list\"", (char *) NULL);
		return TCL_ERROR;
	}

	if (Tcl_SplitList(interp, argv[1], &refArgc, &refArgv) != TCL_OK) {
		return TCL_ERROR;
	}

	if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
		ckfree((char *) refArgv);
		return TCL_ERROR;
	}

	item=listArgv;
	for(pos=0;pos<listArgc;pos++) {
		for(i=0;i<refArgc;i++) {
			if ((refArgv[i]!=NULL)&&(strcmp(refArgv[i],*item)==0)) {
			sprintf(res, "%d", i);
				Tcl_AppendElement(interp,res);
			refArgv[i]=NULL;
			break;
			}
		}
		if (i==refArgc) Tcl_AppendElement(interp,"-1");
		item++;
	}

	ckfree((char *) refArgv);
	ckfree((char *) listArgv);
	return TCL_OK;
}
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LloadCmd --
 *
 *		This procedure is invoked to process the "lload" command.
 *		It creates a list from a file
 *		lload fileName	?nosep?
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LloadCmd(notUsed, interp, argc, argv)
	ClientData notUsed;					/* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int argc;						/* Number of arguments. */
	char **argv;					/* Argument strings. */
{
	FILE *file;
	char *string=NULL;
	int pos;
	int i;

	if ((argc != 2)&&(argc != 3)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" fileName ?nosep?\"", (char *) NULL);
		return TCL_ERROR;
	}

	file=fopen(argv[1],"r");
	if (file==NULL) {
		Tcl_AppendResult(interp, "Couldn't open file ", argv[1], (char *) NULL);
		return TCL_ERROR;
	}
	if (argc==2) {
		while(1) {
			string=read_line(file);
			if (string==NULL) break;
			Tcl_AppendElement(interp,string);
			free(string);
		}
	} else {
		if (strcmp(argv[2], "nosep") != 0) {
			Tcl_AppendResult(interp, "wrong argument ",	argv[2] , (char *) NULL);
			return TCL_ERROR;
		}
		while(1) {
			string=read_line(file);
			if (string==NULL) break;
			Tcl_AppendResult(interp,string,NULL);
			free(string);
		}
	}
	fclose(file);
	return TCL_OK;
}
/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LwriteCmd --
 *
 *		This procedure is invoked to process the "lwrite" command.
 *		It creates a file from a list
 *		lwrite fileName list
 *
 * Results:
 *		A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_LwriteCmd(notUsed, interp, argc, argv)
	ClientData notUsed;					/* Not used. */
	Tcl_Interp *interp;					/* Current interpreter. */
	int argc;						/* Number of arguments. */
	char **argv;					/* Argument strings. */
{
	FILE *file;
	int listArgc;
	char **listArgv;
	int index;
	int i;

	if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" fileName list\"", (char *) NULL);
		return TCL_ERROR;
	}

	file=fopen(argv[1],"a");
	if (file==NULL) {
		Tcl_AppendResult(interp, "Couldn't open file ",argv[1], (char *) NULL);
		return TCL_ERROR;
	}
	if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
		return TCL_ERROR;
	}
	index = -1;
	for (i = 0; i < listArgc; i++) {
		fputs(listArgv[i], file);
		fputs("\n", file);
	}
	ckfree((char *) listArgv);
	fclose(file);
	return TCL_OK;
}
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
	
		if ((argc < 3)&&(argc > 5)) {
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
		before=find_bool(argv[3],"-before","-after");
		list=get_intlist(interp, argv[4], &number, 0);
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
				list=get_intlist(interp, argv[4], &number, 0);
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
	resultstring=numstr(result+min);
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
