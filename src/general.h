/*	
 *	 Purpose: extraL extension to Tcl
 *	 Author:  Copyright (c) 1995 Peter De Rijk
 *
 *	 See the file "README" for information on usage and redistribution
 *	 of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
/*------------------------------------------------------------------*/

#define UCHAR(c) ((unsigned char) (c))
typedef enum {false,true,other} PBOOL;
PBOOL find_bool(char *string, char *trues,char *falses);
char *numstr(int num);
PBOOL read_string(FILE *file,char *place,int size);
int *get_intlist(Tcl_Interp *interp, char *string, int *number, int min);
PBOOL skip_lines(FILE *file,int number);
char *read_line(FILE *file);
char *read_file(FILE *file);
