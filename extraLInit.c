#include "tcl.h"
#include <sys/types.h>
#include <time.h>
#include <math.h>

extern int Dcse_LfindCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int Dcse_LsubCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int Dcse_LcorCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int Dcse_LloadCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int Dcse_LfileCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int Dcse_LmanipCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int Dcse_LmathCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int Dcse_RandomCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));


int
ExtraL_Init(interp)
    Tcl_Interp *interp;		/* Interpreter to add extra commands */
{
    char *libDir;

    dld_AddTclCommand(interp, "lfind", Dcse_LfindCmd);
    dld_AddTclCommand(interp, "lsub", Dcse_LsubCmd);
    dld_AddTclCommand(interp, "lcor", Dcse_LcorCmd);
    dld_AddTclCommand(interp, "lload", Dcse_LloadCmd);
    dld_AddTclCommand(interp, "lfile", Dcse_LfileCmd);
    dld_AddTclCommand(interp, "lmanip", Dcse_LmanipCmd);
    dld_AddTclCommand(interp, "lmath", Dcse_LmathCmd);
    dld_AddTclCommand(interp, "random", Dcse_RandomCmd);
    srand((unsigned int)time(0));
    return TCL_OK;
}

int
dld_AddTclCommand(interp, command, function)
    Tcl_Interp *interp;
    char *command;
    Tcl_CmdProc *function;
{

    Tcl_CreateCommand(interp, command, *function, (ClientData)NULL,
	(Tcl_CmdDeleteProc *)NULL);
    return TCL_OK;
}

