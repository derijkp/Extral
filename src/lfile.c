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

		all=ExtraL_find_bool(argv[3],"-all","-exclude");
		if (all==other) {begin=3;} else {begin=4;}
		if (all!=true) {
			list=ExtraL_get_intlist(interp, argv[begin], &number, 0);
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
		line=ExtraL_read_line(file);
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
		LfilePtr->before=ExtraL_find_bool(argv[3],"-before","-after");
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
	ExtraL_skip_lines(LfilePtr->file,L_pos->line);
	tline=ExtraL_read_line(f_ref);
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
