/* 
 * tclAppInit.c --
 *
 *	Provides a default version of the main program and Tcl_AppInit
 *	procedure for Tcl applications (without Tk).
 *
 * Copyright (c) 1993 The Regents of the University of California.
 * Copyright (c) 1994-1997 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * SCCS: @(#) tclAppInit.c 1.19 97/01/21 18:14:20
 */

#include "tcl.h"

/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl.
 */

#ifdef	__cplusplus
extern "C" {
#endif
  
extern int matherr _ANSI_ARGS_((void));
int (*tclDummyMathPtr) _ANSI_ARGS_((void)) = matherr;

#ifdef	__cplusplus
}
#endif

#ifdef TCL_TEST
extern int		Tcltest_Init _ANSI_ARGS_((Tcl_Interp *interp));
extern int		Tclptest_Init _ANSI_ARGS_((Tcl_Interp *interp));
extern int		Tclobjtest_Init _ANSI_ARGS_((Tcl_Interp *interp));
#endif /* TCL_TEST */


/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	This is the main program for the application.
 *
 * Results:
 *	None: Tcl_Main never returns here, so this procedure never
 *	returns either.
 *
 * Side effects:
 *	Whatever the application does.
 *
 *----------------------------------------------------------------------
 */

int
#ifdef _USING_PROTOTYPES_
main (int    argc,		/* Number of command-line arguments. */
     char **argv)		/* Values of command-line arguments. */
#else
main(argc, argv)
    int argc;			/* Number of command-line arguments. */
    char **argv;		/* Values of command-line arguments. */
#endif
{
    Tcl_Main(argc, argv, Tcl_AppInit);
    return 0;			/* Needed only to prevent compiler warning. */
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppInit --
 *
 *	This procedure performs application-specific initialization.
 *	Most applications, especially those that incorporate additional
 *	packages, will have their own version of this procedure.
 *
 * Results:
 *	Returns a standard Tcl completion code, and leaves an error
 *	message in interp->result if an error occurs.
 *
 * Side effects:
 *	Depends on the startup script.
 *
 *----------------------------------------------------------------------
 */

int
#ifdef _USING_PROTOTYPES_
Tcl_AppInit (Tcl_Interp *interp)/* Interpreter for application. */
#else
Tcl_AppInit(interp)
    Tcl_Interp *interp;		/* Interpreter for application. */
#endif
{
    if (Tcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }

#ifdef TCL_TEST
    if (Tcltest_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Tcltest", Tcltest_Init,
            (Tcl_PackageInitProc *) NULL);
    if (Tclobjtest_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Tclobjtest", Tclobjtest_Init,
            (Tcl_PackageInitProc *) NULL);
    if (Tclptest_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Tclptest", Tclptest_Init,
            (Tcl_PackageInitProc *) NULL);
#endif /* TCL_TEST */

    /*
     * Call the init procedures for included packages.  Each call should
     * look like this:
     *
     * if (Mod_Init(interp) == TCL_ERROR) {
     *     return TCL_ERROR;
     * }
     *
     * where "Mod" is the name of the module.
     */

	if (Extral_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
	}

    /*
     * Call Tcl_CreateCommand for application-specific commands, if
     * they weren't already created by the init procedures called above.
     */

    /*
     * Specify a user-specific startup file to invoke if the application
     * is run interactively.  Typically the startup file is "~/.apprc"
     * where "app" is the name of the application.  If this line is deleted
     * then no user-specific startup file will be run under any conditions.
     */

    Tcl_SetVar(interp, "tcl_rcFileName", "~/.tclshrc", TCL_GLOBAL_ONLY);
    return TCL_OK;
}
