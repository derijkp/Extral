#include "tcl.h"
#include "extral.h"
#include <sys/types.h>
#include <time.h>
#include <math.h>

int Extral_taglInit _ANSI_ARGS_((Tcl_Interp *interp));

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

extern int ExtraL_TaglsetObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_TaglunsetObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_TaglgetObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_TaglfieldsObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_TaglfindObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_ScanDateObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));


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
	Tcl_CreateObjCommand(interp,"lpop",(Tcl_ObjCmdProc *)ExtraL_LpopObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lshift",(Tcl_ObjCmdProc *)ExtraL_LshiftObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lfind",(Tcl_ObjCmdProc *)ExtraL_LfindObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lsub",(Tcl_ObjCmdProc *)ExtraL_LsubObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lcor",(Tcl_ObjCmdProc *)ExtraL_LcorObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lremdup",(Tcl_ObjCmdProc *)ExtraL_LremdupObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"llremove",(Tcl_ObjCmdProc *)ExtraL_LlremoveObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lunmerge",(Tcl_ObjCmdProc *)ExtraL_LunmergeObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lmerge",(Tcl_ObjCmdProc *)ExtraL_LmergeObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"taglset",(Tcl_ObjCmdProc *)ExtraL_TaglsetObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"taglunset",(Tcl_ObjCmdProc *)ExtraL_TaglunsetObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"taglget",(Tcl_ObjCmdProc *)ExtraL_TaglgetObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"taglfields",(Tcl_ObjCmdProc *)ExtraL_TaglfieldsObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"taglfind",(Tcl_ObjCmdProc *)ExtraL_TaglfindObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"scandate",(Tcl_ObjCmdProc *)ExtraL_ScanDateObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Extral_taglInit(interp);

/*	dld_AddTclCommand(interp, "ffind", ExtraL_FfindCmd); */
	dld_AddTclCommand(interp, "amanip", ExtraL_AmanipCmd);
	dld_AddTclCommand(interp, "replace", ExtraL_ReplaceCmd);
	dld_AddTclCommand(interp, "ssort", ExtraL_SSortCmd);
#ifdef windows
/*
	dld_AddTclCommand(interp, "win_mkdir", ExtraL_MkdirCmd);
	dld_AddTclCommand(interp, "win_remove", ExtraL_RemoveCmd);
	dld_AddTclCommand(interp, "win_rmdir", ExtraL_RmdirCmd);
	dld_AddTclCommand(interp, "win_rename", ExtraL_RenameCmd);
	dld_AddTclCommand(interp, "win_chmod", ExtraL_ChmodCmd);
	dld_AddTclCommand(interp, "win_cp", ExtraL_CpCmd);
*/
#endif
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
