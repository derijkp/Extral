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
PBOOL ExtraL_find_bool(char *string, char *trues,char *falses);
char *ExtraL_numstr(int num);
PBOOL ExtraL_read_string(FILE *file,char *place,int size);
int *ExtraL_get_intlist(Tcl_Interp *interp, char *string, int *number, int min);
PBOOL ExtraL_skip_lines(FILE *file,int number);
char *ExtraL_read_line(FILE *file);
char *ExtraL_read_file(FILE *file);
