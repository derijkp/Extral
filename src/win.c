#include <stdio.h>
#include <sys\stat.h>
#include "tcl.h"
#include "dir.h"
int
ExtraL_MkdirCmd(notUsed, interp, argc, argv)
	 ClientData notUsed;        	        /* Not used. */
	 Tcl_Interp *interp;        	        /* Current interpreter. */
	 int argc;        	        	/* Number of arguments. */
	 char **argv;        	        /* Argument strings. */
{
	int err;
	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" dirname\"", (char *) NULL);
		return TCL_ERROR;
	}
	err=mkdir(argv[1]);
	if (err==0) {return TCL_OK;}
	else {return TCL_ERROR;}
}

int
ExtraL_RemoveCmd(notUsed, interp, argc, argv)
	 ClientData notUsed;        	        /* Not used. */
	 Tcl_Interp *interp;        	        /* Current interpreter. */
	 int argc;        	        	/* Number of arguments. */
	 char **argv;        	        /* Argument strings. */
{
	int err;
	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" filename\"", (char *) NULL);
		return TCL_ERROR;
	}
	err=remove(argv[1]);
	if (err==0) {return TCL_OK;}
	else {return TCL_ERROR;}
}

int
ExtraL_RmdirCmd(notUsed, interp, argc, argv)
	 ClientData notUsed;        	        /* Not used. */
	 Tcl_Interp *interp;        	        /* Current interpreter. */
	 int argc;        	        	/* Number of arguments. */
	 char **argv;        	        /* Argument strings. */
{
	int err;
	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" dirname\"", (char *) NULL);
		return TCL_ERROR;
	}
	err=rmdir(argv[1]);
	if (err==0) {return TCL_OK;}
	else {return TCL_ERROR;}
}

int
ExtraL_RenameCmd(notUsed, interp, argc, argv)
	 ClientData notUsed;        	        /* Not used. */
	 Tcl_Interp *interp;        	        /* Current interpreter. */
	 int argc;        	        	/* Number of arguments. */
	 char **argv;        	        /* Argument strings. */
{
	int err;
	if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" filename filename\"", (char *) NULL);
		return TCL_ERROR;
	}
	err=rename(argv[1],argv[2]);
	if (err==0) {return TCL_OK;}
	else {return TCL_ERROR;}
}

int
ExtraL_ChmodCmd(notUsed, interp, argc, argv)
	 ClientData notUsed;        	        /* Not used. */
	 Tcl_Interp *interp;        	        /* Current interpreter. */
	 int argc;        	        	/* Number of arguments. */
	 char **argv;        	        /* Argument strings. */
{
	char c;
	int temp;
	int err;
	if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" mod filename\"", (char *) NULL);
		return TCL_ERROR;
	}
	c=argv[1][0];
	if (c=='n') temp=0;
	else if (c=='r') temp=S_IREAD;
	else temp=S_IREAD | S_IWRITE;
	err=chmod(argv[2],temp);
	if (err==0) {return TCL_OK;}
	else {return TCL_ERROR;}
}

int
ExtraL_CpCmd(notUsed, interp, argc, argv)
	 ClientData notUsed;        	        /* Not used. */
	 Tcl_Interp *interp;        	        /* Current interpreter. */
	 int argc;        	        	/* Number of arguments. */
	 char **argv;        	        /* Argument strings. */
{
	FILE *src, *dst;
	int c;
	if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			" filename filename\"", (char *) NULL);
		return TCL_ERROR;
	}
	src=fopen(argv[1],"rb");
	if (src==NULL) {
		Tcl_AppendResult(interp,"Couldn't open ",argv[1], " for reading",(char *) NULL);
		return TCL_ERROR;
	}
	remove(argv[2]);
	dst=fopen(argv[2],"wb");
	if (src==NULL) {
		Tcl_AppendResult(interp,"Couldn't open ",argv[2], " for writing",(char *) NULL);
		return TCL_ERROR;
	}
	while (!feof(src)) {
		c=fgetc(src);
		if (c==EOF) break;
		fputc(c,dst);
	}
	fclose(src);
	fclose(dst);
}

