#include "tcl.h"
#include <sys/types.h>
#include <time.h>
#include <math.h>

extern int ExtraL_LpopObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_LshiftObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_LfindCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_FfindCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_LsubCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_LcorCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_LmanipCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_LmathCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_LregsubCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_RandomCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_AmanipCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_ReplaceCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_SSortCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_LfileCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_AtexitCmd _ANSI_ARGS_((ClientData clientData));

#ifdef windows
/*
extern int ExtraL_MkdirCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));
extern int ExtraL_RemoveCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));
extern int ExtraL_RmdirCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));
extern int ExtraL_RenameCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));
*/
extern int ExtraL_ChmodCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));
/*
extern int ExtraL_CpCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));
*/
#endif

int
Extral_Init(interp)
	Tcl_Interp *interp;		/* Interpreter to add extra commands */
{
	char *libDir;

	Tcl_PkgProvide(interp, "extral", "0.94");
	Tcl_CreateObjCommand(interp,"lpop",4,(Tcl_ObjCmdProc *)ExtraL_LpopObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lshift",6,(Tcl_ObjCmdProc *)ExtraL_LshiftObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);


	dld_AddTclCommand(interp, "lfind", ExtraL_LfindCmd);
	dld_AddTclCommand(interp, "ffind", ExtraL_FfindCmd);
	dld_AddTclCommand(interp, "lsub", ExtraL_LsubCmd);
	dld_AddTclCommand(interp, "lcor", ExtraL_LcorCmd);
	dld_AddTclCommand(interp, "lmanip", ExtraL_LmanipCmd);
	dld_AddTclCommand(interp, "lmath", ExtraL_LmathCmd);
	dld_AddTclCommand(interp, "lregsub", ExtraL_LregsubCmd);
	dld_AddTclCommand(interp, "amanip", ExtraL_AmanipCmd);
	dld_AddTclCommand(interp, "replace", ExtraL_ReplaceCmd);
	dld_AddTclCommand(interp, "ssort", ExtraL_SSortCmd);
	dld_AddTclCommand(interp, "lfile", ExtraL_LfileCmd);
	dld_AddTclCommand(interp, "random", ExtraL_RandomCmd);
#ifdef windows
/*
	dld_AddTclCommand(interp, "win_mkdir", ExtraL_MkdirCmd);
	dld_AddTclCommand(interp, "win_remove", ExtraL_RemoveCmd);
	dld_AddTclCommand(interp, "win_rmdir", ExtraL_RmdirCmd);
	dld_AddTclCommand(interp, "win_rename", ExtraL_RenameCmd);
*/
	dld_AddTclCommand(interp, "win_chmod", ExtraL_ChmodCmd);
/*
	dld_AddTclCommand(interp, "win_cp", ExtraL_CpCmd);
*/
#endif
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
