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
int *ExtraL_get_intlist(Tcl_Interp *interp, char *string, int *number, int min);
int ExtraL_read_file(Tcl_Channel file, Tcl_DString *result);

/* compatibility with Tcl 9 */

#if TCL_MAJOR_VERSION > 8
#  define MIN_TCL_VERSION "9.0"
#else
#  define MIN_TCL_VERSION "8.1"
#endif

#ifndef CONST
#define CONST const
#endif

#ifndef TCL_PARSE_PART1
#define TCL_PARSE_PART1 0
#endif

#if TCL_MAJOR_VERSION < 9
#define Tcl_Size int
#endif

#ifndef _ANSI_ARGS_
#define _ANSI_ARGS_(x) x
#endif

#ifndef Tcl_DStringTrunc
#define Tcl_DStringTrunc Tcl_DStringSetLength
#endif

#if TCL_MAJOR_VERSION > 8
#  define Tcl_GetSizeIntFromObj(interp, obj, ptr) \
       Tcl_GetWideIntFromObj((interp), (obj), (ptr))
#else
/* In Tcl 8, Tcl_Size is int, so Tcl_GetIntFromObj works directly */
#  define Tcl_GetSizeIntFromObj(interp, obj, ptr) \
       Tcl_GetIntFromObj((interp), (obj), (ptr))
#endif
