#include "tcl.h"
#include "extral.h"
#include <sys/types.h>
#include <time.h>
#include <math.h>

int Extral_StructlInit _ANSI_ARGS_((Tcl_Interp *interp));


/*
extern int	ExtraL_LfileCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));
*/
extern int	ExtraL_LevalObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));

extern int ExtraL_LpopObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *argv[]));

extern int ExtraL_LshiftObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *argv[]));

extern int ExtraL_LfindObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *argv[]));

extern int ExtraL_LsubObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *argv[]));

extern int ExtraL_LremdupObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *argv[]));

extern int ExtraL_FfindCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_LcorObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *argv[]));

extern int ExtraL_LlremoveObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *argv[]));

extern int ExtraL_LunmergeObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *argv[]));

extern int ExtraL_LmergeObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, Tcl_Obj *argv[]));

extern int ExtraL_AmanipCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_ReplaceCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_SSortCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_AtexitCmd _ANSI_ARGS_((ClientData clientData));

extern int ExtraL_ScanTimeObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_FormatTimeObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int Extral_DbmInit _ANSI_ARGS_((Tcl_Interp *interp));

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
dld_AddTclCommand(interp, command, function)
	Tcl_Interp *interp;
	char *command;
	Tcl_CmdProc *function;
{

	Tcl_CreateCommand(interp, command, *function, (ClientData)NULL,
	(Tcl_CmdDeleteProc *)NULL);
	return TCL_OK;
}

int
Extral_Init(interp)
	Tcl_Interp *interp;		/* Interpreter to add extra commands */
{
	Tcl_PkgProvide(interp, "extral", "0.94");
	Tcl_CreateObjCommand(interp,"Extral::lpop",(Tcl_ObjCmdProc *)ExtraL_LpopObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::lshift",(Tcl_ObjCmdProc *)ExtraL_LshiftObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::lfind",(Tcl_ObjCmdProc *)ExtraL_LfindObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::lsub",(Tcl_ObjCmdProc *)ExtraL_LsubObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::lcor",(Tcl_ObjCmdProc *)ExtraL_LcorObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::lremdup",(Tcl_ObjCmdProc *)ExtraL_LremdupObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::llremove",(Tcl_ObjCmdProc *)ExtraL_LlremoveObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::lunmerge",(Tcl_ObjCmdProc *)ExtraL_LunmergeObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::lmerge",(Tcl_ObjCmdProc *)ExtraL_LmergeObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::scantime",(Tcl_ObjCmdProc *)ExtraL_ScanTimeObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::formattime",(Tcl_ObjCmdProc *)ExtraL_FormatTimeObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"Extral::leval",(Tcl_ObjCmdProc *)ExtraL_LevalObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);

	dld_AddTclCommand(interp, "Extral::amanip", ExtraL_AmanipCmd);
	dld_AddTclCommand(interp, "Extral::replace", ExtraL_ReplaceCmd);
	dld_AddTclCommand(interp, "Extral::ssort", ExtraL_SSortCmd);
/*	dld_AddTclCommand(interp, "Extral::ffind", ExtraL_FfindCmd); */
/*	dld_AddTclCommand(interp, "lfile", ExtraL_LfileCmd); */

	Extral_StructlInit(interp);
	Extral_DbmInit(interp);
	return TCL_OK;
}
