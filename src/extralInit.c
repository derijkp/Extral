#include "tcl.h"
#include "extral.h"
#include <sys/types.h>
#include <time.h>
#include <math.h>

int Extral_StructlInit _ANSI_ARGS_((Tcl_Interp *interp));


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

extern int ExtraL_SSortCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_ScanTimeObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_FormatTimeObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_LreverseObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_SreverseObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_ReplaceObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_AmanipObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_SSortObjCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

extern int ExtraL_SFindObjCmd _ANSI_ARGS_((ClientData clientData,
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
Extral_Init(interp)
	Tcl_Interp *interp;		/* Interpreter to add extra commands */
{
	/* $Format: "\tTcl_PkgProvide(interp,"Extral","1.$ProjectMajorVersion$.$ProjectMinorVersion$""$ */
	Tcl_PkgProvide(interp,
	Tcl_CreateObjCommand(interp,"lpop",(Tcl_ObjCmdProc *)ExtraL_LpopObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lshift",(Tcl_ObjCmdProc *)ExtraL_LshiftObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lfind",(Tcl_ObjCmdProc *)ExtraL_LfindObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lsub",(Tcl_ObjCmdProc *)ExtraL_LsubObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lcor",(Tcl_ObjCmdProc *)ExtraL_LcorObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lremdup",(Tcl_ObjCmdProc *)ExtraL_LremdupObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"llremove",(Tcl_ObjCmdProc *)ExtraL_LlremoveObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lunmerge",(Tcl_ObjCmdProc *)ExtraL_LunmergeObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lmerge",(Tcl_ObjCmdProc *)ExtraL_LmergeObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"scantime",(Tcl_ObjCmdProc *)ExtraL_ScanTimeObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"formattime",(Tcl_ObjCmdProc *)ExtraL_FormatTimeObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"leval",(Tcl_ObjCmdProc *)ExtraL_LevalObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"lreverse",(Tcl_ObjCmdProc *)ExtraL_LreverseObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"replace",(Tcl_ObjCmdProc *)ExtraL_ReplaceObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"sreverse",(Tcl_ObjCmdProc *)ExtraL_SreverseObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"amanip",(Tcl_ObjCmdProc *)ExtraL_AmanipObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"ssort",(Tcl_ObjCmdProc *)ExtraL_SSortObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"sfind",(Tcl_ObjCmdProc *)ExtraL_SFindObjCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);

/*	Tcl_CreateCommand(interp,"ffind",ExtraL_FfindCmd,(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL); */

	Extral_StructlInit(interp);
	Extral_DbmInit(interp);
	return TCL_OK;
}
