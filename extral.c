/*
    ####      ###    ####   ######     Dedicated
    #   #    #   #  #    #  #          Comparative
    #    #  #       #       #          Sequence
    #    #  #        ####   ###        Editor
    #    #  #            #  #          _____________________________________
    #   #    #   #  #    #  #                        
    ####      ###    ####   ######                   Peter De Rijk
    ________________________________________________________________________

    File:    lfile.h
    Author:  Copyright � 1993 Peter De Rijk
    Purpose: extraL extension to Tcl
*/
/*
 #include "general.h"
 #include "filing.h"
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "tclRegexp.h"
  typedef enum {false,true,other} PBOOL;

extern regexp *TclCompileRegexp(Tcl_Interp *interp,char *string);
extern char *		tclRegexpError;

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
  sprintf(result, "%*d", i,  num);
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
int *get_intlist(Tcl_Interp *interp, char *string, int *number)
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
	if (temp<0) {
            ckfree((char *) listArgv);
	    free(list);
	    Tcl_AppendResult(interp, "negative index present!", (char *) NULL);
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
      }while ((ch!='\n')&&(ch!=EOF));
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
        int *get_intlist(Tcl_Interp *interp, char *string, int *number);
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
                    argv[0], " get ?line?:?begin?-?end???|regexp?? ?modifier:-all/-exclude? ??indexlist??\"", (char *) NULL);
            return TCL_ERROR;
        }

	all=find_bool(argv[3],"-all","-exclude");
	if (all==other) {begin=3;} else {begin=4;}
	if (all!=true) {
	    list=get_intlist(interp, argv[begin], &number);
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
int Dcse_LfileCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]) {
    Lfile_index *LfilePtr;

    if ((argc != 3)&&(argc != 5)) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
                argv[0], " pathName ?file? ??-before/-after seperator??\"", (char *) NULL);
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
      L_pos->regexpPtr = TclCompileRegexp(interp, regpattern);
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
      tclRegexpError = NULL;
      match = TclRegExec(regexpPtr, result, result);
      if (tclRegexpError != NULL) {
	  Tcl_AppendResult(interp, "error while matching pattern: ",
		    tclRegexpError, (char *) NULL);
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
 * Dcse_LfindCmd --
 *
 *	This procedure is invoked to process the "lfind" command.
 *      It finds all occurences of a pattern in a list, and returns
 *      their indexes as a list.
 *
 * Results:
 *	A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Dcse_LfindCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
#define EXACT	0
#define GLOB	1
#define REGEXP	2
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
 * Dcse_LsubCmd --
 *
 *	This procedure is invoked to process the "lsub" command.
 *      It creates a subset of a list
 *
 * Results:
 *	A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Dcse_LsubCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
#define INCLUDE	0
#define EXCLUDE	1
    int *get_intlist(Tcl_Interp *interp, char *string, int *number);
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

    list=get_intlist(interp, argv[begin], &number);
    if (list==NULL) {
	return TCL_ERROR;
    }

    /*---- do the extraction ----*/
    if (Tcl_SplitList(interp, argv[1], &listArgc, &listArgv) != TCL_OK) {
	return TCL_ERROR;
    }

    if (mode==INCLUDE) {
        for(i=0;i<number;i++) {
	    if (list[i]>=listArgc) {
		ckfree((char *) listArgv);
		free(list);
		Tcl_AppendElement(interp, "index out of range!");
		return TCL_ERROR;
	    }
	    Tcl_AppendElement(interp, listArgv[list[i]]);
	}
    } else {
	index = -1;
	for (i = 0; i < listArgc; i++) {
	    match = 1;
            for(j=0;j<number;j++) {
		if (list[j]>=listArgc) {
		    ckfree((char *) listArgv);
		    free(list);
		    Tcl_AppendElement(interp, "index out of range!");
		    return TCL_ERROR;
		}
	        if (i==list[j]) {match=0;break;}
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
 * Dcse_LcorCmd --
 *
 *	This procedure is invoked to process the "lcor" command.
 *      It creates a subset of a list
 *
 * Results:
 *	A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Dcse_LcorCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
#define INCLUDE	0
#define EXCLUDE	1
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
 * Dcse_LloadCmd --
 *
 *	This procedure is invoked to process the "lload" command.
 *      It creates a list from a file
 *      lload ?file?  ??nosep??
 *
 * Results:
 *	A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
Dcse_LloadCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    FILE *file;
    char *string=NULL;
    int pos;
    int i;

    if ((argc != 2)&&(argc != 3)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" ?file? ??nosep??\"", (char *) NULL);
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
	    Tcl_AppendResult(interp, "wrong argument ",  argv[2] , (char *) NULL);
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
 * Dcse_LwriteCmd --
 *
 *	This procedure is invoked to process the "lwrite" command.
 *      It creates a file from a list
 *      lwrite ?file? ?list?
 *
 * Results:
 *	A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
Dcse_LwriteCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    FILE *file;
    int listArgc;
    char **listArgv;
    int index;
    int i;

    if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" ?file? ?list?\"", (char *) NULL);
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
 * Dcse_LmanipCmd --
 *
 *	This procedure is invoked to process the "lmanip" command.
 *      It manipulates lists in all kinds of ways
 *      lmanip ?option? ?list? ?arg? ...
 *
 * Results:
 *	A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
Dcse_LmanipCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int *get_intlist(Tcl_Interp *interp, char *string, int *number);
    int listArgc;
    char **listArgv;
    char *line=NULL;
    int *list;
    int c,len;
    int i;

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" ?option? ?list? ??arg?? ...\"", (char *) NULL);
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
		    " subindex ?list? ??pos??\"", (char *) NULL);
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
		    " merge ?list1? ?list2?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_SplitList(interp, argv[3], &listArgc2, &listArgv2) != TCL_OK) {
	    return TCL_ERROR;
	}

	i=0;
	while(i<listArgc) {
	    Tcl_DStringInit(&element);
	    Tcl_DStringAppend(&element, listArgv[i],-1);
	    if (i<listArgc2) {
	        Tcl_DStringAppend(&element, " ",-1);
	        Tcl_DStringAppend(&element, listArgv2[i],-1);
	    }
	    Tcl_AppendElement(interp, Tcl_DStringValue(&element));
	    Tcl_DStringFree(&element);
	    i++;
	}
	ckfree((char *) listArgv);
	ckfree((char *) listArgv2);
    } else if ((c == 'e')&&(strncmp(argv[1], "extract",len) == 0)) {
        regexp *regexpPtr;
        Tcl_DString element;
	char savedChar, *first, *last;
        int match = 0;
	int nr;
	int i;
    
	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " extract ?list? ?expression?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	regexpPtr = TclCompileRegexp(interp, argv[3]);
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
	    tclRegexpError = NULL;
	    match = TclRegExec(regexpPtr, line, line);
	    if (tclRegexpError != NULL) {
	        Tcl_AppendResult(interp, "error while matching pattern: ",
	  		tclRegexpError, (char *) NULL);
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
		    " remdup ?list?\"\n - returns the list in which duplicates are removed", (char *) NULL);
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
	int *list;
	int number, nr;
	int i;
    
	if (argc != 5) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " split ?list? -before/-after/-outside ?positions?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	before=find_bool(argv[3],"-before","-after");
	list=get_intlist(interp, argv[4], &number);
	if (list==NULL) {
	    return TCL_ERROR;
	}

	if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
	    return TCL_ERROR;
	}

	nr=0;
        Tcl_DStringInit(&element);
	Tcl_DStringAppend(&element, listArgv[nr++], -1);
	while(nr<listArgc) {
	    i=0;
	    while(i<number) {
		if (nr==list[i]) break;
		i++;
	    }
	    if (i<number) {
	        if (before==true) {
		    Tcl_AppendElement(interp, Tcl_DStringValue(&element));
		    Tcl_DStringFree(&element);
		    Tcl_DStringInit(&element);
	            Tcl_DStringAppendElement(&element, listArgv[nr]);
		} else if (before==false) {
	            Tcl_DStringAppendElement(&element, listArgv[nr]);
		    Tcl_AppendElement(interp, Tcl_DStringValue(&element));
		    Tcl_DStringFree(&element);
		    Tcl_DStringInit(&element);
		    nr++;
		    if (nr<listArgc) Tcl_DStringAppendElement(&element, listArgv[nr]);
		} else {
		    Tcl_AppendElement(interp, Tcl_DStringValue(&element));
		    Tcl_DStringFree(&element);
		    Tcl_DStringInit(&element);
		    nr++;
		    if (nr<listArgc) Tcl_DStringAppendElement(&element, listArgv[nr]);
		}
	    } else {
	        Tcl_DStringAppendElement(&element, listArgv[nr]);
	    }
	    nr++;
	}
	Tcl_AppendElement(interp, Tcl_DStringValue(&element));
	Tcl_DStringFree(&element);
	ckfree((char *) listArgv);
    } else if ((c == 'j')&&(strncmp(argv[1], "join",len) == 0)) {
        Tcl_DString element;
	char savedChar, *first, *last;
	PBOOL before;
	int *list;
	int number, nr;
	int i;
    
	if (argc != 5) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " join ?list? -before/-after ?positions?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	before=find_bool(argv[3],"-before","-after");
        if (strcmp(argv[4], "all") == 0) {
	    list=NULL;
	    before=true;
	} else {
	    list=get_intlist(interp, argv[4], &number);
	    if (list==NULL) {
		return TCL_ERROR;
	    }
	}

	if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
	    return TCL_ERROR;
	}

	nr=0;
        Tcl_DStringInit(&element);
	if (before==true) {
	    Tcl_DStringAppend(&element, listArgv[nr++], -1);
        }

	i=0;
	while(nr<listArgc) {
	    if (list!=NULL) {
		i=0;
		while(i<number) {
		    if (nr==list[i]) break;
		    i++;
		}
	    }
	    if (i<number) {
	        Tcl_DStringAppend(&element, listArgv[nr], strlen(listArgv[nr]));
	    } else {
	        if (before==false) {
		    Tcl_DStringAppend(&element, listArgv[nr], strlen(listArgv[nr]));
		    Tcl_AppendElement(interp, Tcl_DStringValue(&element));
		    Tcl_DStringFree(&element);
		    Tcl_DStringInit(&element);
		} else {
		    Tcl_AppendElement(interp, Tcl_DStringValue(&element));
		    Tcl_DStringFree(&element);
		    Tcl_DStringInit(&element);
		    Tcl_DStringAppend(&element, listArgv[nr], strlen(listArgv[nr]));
		}
	    }
	    nr++;
	}

	if (before==true) {
	    Tcl_AppendElement(interp, Tcl_DStringValue(&element));
	}

	Tcl_DStringFree(&element);
	ckfree((char *) listArgv);
    } else if ((c == 't')&&(strncmp(argv[1], "toarray",len) == 0)) {
	int list2Argc;
	char **list2Argv;
        Tcl_DString element;
	char savedChar, *first, *last;
	int number, nr;
	int i;
    
	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " toarray ?list? ?variable?\"", (char *) NULL);
	    return TCL_ERROR;
	}

	if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
	    return TCL_ERROR;
	}

	nr=0;
	while(nr<listArgc) {
	    if (Tcl_SplitList(interp, listArgv[nr], &list2Argc, &list2Argv) != TCL_OK) {
	        ckfree((char *) listArgv);
		return TCL_ERROR;
	    }
	    if (list2Argc<2) {
		Tcl_AppendResult(interp, "Some elements contain lest then 2 elements", (char *) NULL);
	        ckfree((char *) listArgv);
		return TCL_ERROR;
	    } else {
	        Tcl_SetVar2(interp, argv[3], list2Argv[0], list2Argv[1], TCL_LEAVE_ERR_MSG);
            }
            ckfree((char *) list2Argv);
	    nr++;
	}
	ckfree((char *) listArgv);
    } else if ((c == 'l')&&(strncmp(argv[1], "lengths",len) == 0)) {
	int i;
	char val[30];
    
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " lengths ?list?\"\n - returns a list of the lengths", (char *) NULL);
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
    } else {
	Tcl_AppendResult(interp, "wrong option: should be:",
	           "subindex, merge, extract, remdup, split, join, lengths", (char *) NULL);
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
 * Dcse_LmathCmd --
 *
 *	This procedure is invoked to process the "lmath" command.
 *      It calculates things from lists in all kinds of ways
 *      lmath ?option? ?list? ?arg? ...
 *
 * Results:
 *	A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
Dcse_LmathCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int *get_intlist(Tcl_Interp *interp, char *string, int *number);
    int listArgc;
    char **listArgv;
    char *result;
    int *list;
    int c,len;
    int i;

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" ?option? ?list? ??arg?? ...\"", (char *) NULL);
	return TCL_ERROR;
    }
    c=argv[1][0];
    len=strlen(argv[1]);
    if ((c == 's')&&(strncmp(argv[1], "sum",len) == 0)) {
        int listArgcres;
        char **listArgvres;
	int value, sum=0;
    
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " sum ?list?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
	    return TCL_ERROR;
	}

        i=0;
	while(i<listArgc) {
	    if (Tcl_GetInt(interp, listArgv[i], &value) != TCL_OK) {
		return TCL_ERROR;
	    }
	    sum+=value;
	    i++;
	}
	Tcl_ResetResult(interp);
	result=numstr(sum);
	Tcl_AppendResult(interp, result, (char *) NULL);
	free(result);
	ckfree((char *) listArgv);
    } else if ((c == 'm')&&(strncmp(argv[1], "max",len) == 0)) {
        int listArgcres;
        char **listArgvres;
	int value, max;
    
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " max ?list?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
	    return TCL_ERROR;
	}

        i=0;
	if (Tcl_GetInt(interp, listArgv[i], &max) != TCL_OK) {
	    return TCL_ERROR;
	}
	while(i<listArgc) {
	    if (Tcl_GetInt(interp, listArgv[i], &value) != TCL_OK) {
		return TCL_ERROR;
	    }
	    if (value>max) max=value;
	    i++;
	}
	Tcl_ResetResult(interp);
	result=numstr(max);
	Tcl_AppendResult(interp, result, (char *) NULL);
	free(result);
	ckfree((char *) listArgv);
    } else if ((c == 'm')&&(strncmp(argv[1], "min",len) == 0)) {
        int listArgcres;
        char **listArgvres;
	int value, min;
    
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " min ?list?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
	    return TCL_ERROR;
	}

        i=0;
	if (Tcl_GetInt(interp, listArgv[i], &min) != TCL_OK) {
	    return TCL_ERROR;
	}
	while(i<listArgc) {
	    if (Tcl_GetInt(interp, listArgv[i], &value) != TCL_OK) {
		return TCL_ERROR;
	    }
	    if (value<min) min=value;
	    i++;
	}
	Tcl_ResetResult(interp);
	result=numstr(min);
	Tcl_AppendResult(interp, result, (char *) NULL);
	free(result);
	ckfree((char *) listArgv);
    } else if ((c == 'c')&&(strncmp(argv[1], "cumul",len) == 0)) {
        int listArgcres;
        char **listArgvres;
	int value, current=0;
	char val[30];
    
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " cumul ?list?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (Tcl_SplitList(interp, argv[2], &listArgc, &listArgv) != TCL_OK) {
	    return TCL_ERROR;
	}

        i=0;
	if (Tcl_GetInt(interp, listArgv[i], &value) != TCL_OK) {
	    return TCL_ERROR;
	}
	while(i<listArgc) {
	    if (Tcl_GetInt(interp, listArgv[i], &value) != TCL_OK) {
		return TCL_ERROR;
	    }
	    current+=value;
	    sprintf(val, "%d", current);
	    Tcl_AppendElement(interp, val);
	    i++;
	}
	ckfree((char *) listArgv);
    } else {
	Tcl_AppendResult(interp, "wrong option: should be:",
	           "sum, min, max, cumul", (char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}
/*
 *----------------------------------------------------------------------
 *
 * Dcse_RandomCmd --
 *
 *	This procedure is invoked to process the "lload" command.
 *      It creates a list from a file
 *      lload ?file? ?variable? ??match?? ??variable2?? ...
 *
 * Results:
 *	A standard Tcl result.
 *
 *
 *----------------------------------------------------------------------
 */

int
Dcse_RandomCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
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
		" ?min? ?max?\"", (char *) NULL);
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
    deler=SHRT_MAX/(max-min);
    result=number/deler;
    if (result<min) {
	Tcl_AppendResult(interp, "Something fishy: Result too small", (char *) NULL);
	return TCL_ERROR;
    }
    if (result>max) {
	Tcl_AppendResult(interp, "Something fishy: Result too big", (char *) NULL);
	return TCL_ERROR;
    }
    resultstring=numstr(result+min);
    Tcl_AppendResult(interp, resultstring, (char *) NULL);
    free(resultstring);
    return TCL_OK;
}
