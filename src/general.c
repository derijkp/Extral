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
#include "general.h"
/*------------------------------------------------------------------*/
PBOOL ExtraL_find_bool(char *string, char *trues,char *falses)
{
	if (strcmp(string,trues)==0) return(true);
	else if (strcmp(string,falses)==0) return(false);
	else return(other);
}
/*------------------------------------------------------------------*/
char *ExtraL_numstr(int num)
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
PBOOL ExtraL_read_string(FILE *file,char *place,int size)
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
int *ExtraL_get_intlist(Tcl_Interp *interp, char *string, int *number, int min)
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
	if (listArgc!=0) {
		list=(int *)malloc(listArgc*sizeof(int));
	} else {
		list=(int *)malloc(1*sizeof(int));
	}
	if (list==NULL) {
		ckfree((char *) listArgv);
		Tcl_AppendResult(interp, "GetInt couldn't allocate memory", (char *) NULL);
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
PBOOL ExtraL_skip_lines(FILE *file,int number)
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
char *ExtraL_read_line(FILE *file)
{
	PBOOL b;
	char buffer[501];
	char *result=NULL, *temp;
	int bl=0;

	do {
		b=ExtraL_read_string(file,buffer,500);
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
char *ExtraL_read_file(FILE *file)
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
