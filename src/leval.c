/*
	patch converted to extension by Peter De Rijk

	The attached patch implements the new Tcl command "leval".
This command is a fast light "eval" specifically designed to execute
zero or more Tcl lists (concatenated) by invoking the command specified
by the first list element, with the remaining list elements as "literal"
arguments.  No variable or command substitution takes place on the
arguments.

	Most useful in situations where one normally use eval merely
	to turn "$args" from an argument list to a list of arguments:

	eval command $args <=> leval command $args,

	but the latter is faster and safer.

		Viktor Dukhovni <viktor@esm.com>		: ARPA
		<...!netcom.com!esmc!viktor>		: UUCP
		14742 Newport Ave., Suite 207, Tustin, CA 92680 : US-Post
		+1-(714)-505-7686 x108			: VOICE

*/

#include "tcl.h"
#include "tclInt.h"

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_GetCmdInfoFromObj --
 *
 *	Returns various information about a Tcl command.  The command
 *	name is passed in as an object.
 *
 * Results:
 *	If cmdName exists in interp, then *infoPtr is modified to
 *	hold information about cmdName and 1 is returned.  If the
 *	command doesn't exist then 0 is returned and *infoPtr isn't
 *	modified.
 *
 * Side effects:
 *	May update the internal representation for the object, caching
 *	the command reference so that the next time this procedure is
 *	called with the same object, the command can be found quickly.
 *
 *----------------------------------------------------------------------
 */

int
ExtraL_GetCmdInfoFromObj(interp, cmdObj, infoPtr)
    Tcl_Interp *interp;			/* Interpreter in which to look
                           * for command. */
    Tcl_Obj *cmdObj;					 /* Object for desired command. */
    Tcl_CmdInfo *infoPtr;	/* Where to store information about
					          * command. */
{
	Tcl_Command  cmd;
	Command	*cmdPtr;

	cmd = Tcl_GetCommandFromObj(interp, cmdObj);

	if (cmd == (Tcl_Command)NULL) {
		return 0;
	}

	/*
	 * Set isNativeObjectProc 1 if objProc was registered by a call to
	 * Tcl_CreateObjCommand. Otherwise set it to 0.
	*/

	cmdPtr = (Command *)cmd;
	infoPtr->isNativeObjectProc =
	(cmdPtr->objProc != TclInvokeStringCommand);
	infoPtr->objProc = cmdPtr->objProc;
	infoPtr->objClientData = cmdPtr->objClientData;
	infoPtr->proc = cmdPtr->proc;
	infoPtr->clientData = cmdPtr->clientData;
	infoPtr->deleteProc = cmdPtr->deleteProc;
	infoPtr->deleteData = cmdPtr->deleteData;
	infoPtr->namespacePtr = (Tcl_Namespace *) cmdPtr->nsPtr;

	return 1;
 }

/*
 *----------------------------------------------------------------------
 *
 * ExtraL_LevalObjCmd --
 *
 *	This object-based procedure is invoked to process the "leval" Tcl
 *	command. See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl object result.
 *
 * Side effects:
 *	See the user documentation.
 *
 * TODO:
 *	Should leval invoke command traces?
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
ExtraL_LevalObjCmd(dummy, interp, objc, objv)
	ClientData dummy;		/* Not used. */
	Tcl_Interp *interp;		/* Current interpreter. */
	int objc;			/* Number of arguments. */
	Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	int	i;
	int	result;			/* Hopefully TCL_OK */
	int	cmdObjc = 0;			/* The number of arguments. */
	Tcl_Obj	**cmdObjv;		/* The array of argument objects. */
	void	*newMem;		/* Allocated space for above array */
	Tcl_Obj	**objPtrPtr;		/* Pointer to cmdObjv element */
	char	*cmdName;		/* String name of command */
	int	cmdLen;			/* Byte count of above */
	Tcl_CmdInfo	cmdInfo;		/* Command info structure */

	/*
	 * Compute cmdObjc,  so we only allocate cmdObjv once.
	 * This has the side effect of checking that all the arguments are
	 * valid lists,  and converting them to their list representation.
	 * The subsequent call to ListObjGetElements should be fast!
	 */
	for (i = 1; i < objc; ++i) {
		int listLen;
		if (Tcl_ListObjLength(interp, objv[i], &listLen) != TCL_OK)
			return TCL_ERROR;
		cmdObjc += listLen;
	}

	if (cmdObjc == 0) {
		/*
		 * Empty list
		*/
		return TCL_OK;
	}

	/*
	 * Allocate one extra slot for "unknown" command
	 */
	newMem = ckalloc((cmdObjc + 2) * sizeof(cmdObjv[0]));
	cmdObjv = ((Tcl_Obj **)newMem) + 1;
	cmdObjv[cmdObjc] = (Tcl_Obj *)NULL;

	/*
	 * Fill cmdObjv with consecutive elements from each list
	 */
	objPtrPtr = cmdObjv;
	for (i=1; i < objc; ++i) {
		int listLen;
		Tcl_Obj **elemPtr;
	
		(void) Tcl_ListObjGetElements(interp, objv[i], &listLen, &elemPtr);
		while (--listLen >= 0) {
			*objPtrPtr++ = *elemPtr++;
		}
	}


	cmdName = Tcl_GetStringFromObj(cmdObjv[0], &cmdLen);

	/*
	 * Optimize for success
	 */
	if (ExtraL_GetCmdInfoFromObj(interp, cmdObjv[0], &cmdInfo)) {
		result = cmdInfo.objProc(cmdInfo.objClientData, interp, cmdObjc, cmdObjv);
	}
	else if (cmdLen == strlen(cmdName)) {
		/*
		 * Command not found,  try "unknown", but only if command
		 * contains no NULLs
		*/
		Tcl_Obj *unknownObj = Tcl_NewStringObj("unknown", 7);
		Tcl_IncrRefCount(*--cmdObjv = unknownObj);
		++cmdObjc;
	
		if (ExtraL_GetCmdInfoFromObj(interp, unknownObj, &cmdInfo)) {
			result = cmdInfo.objProc(cmdInfo.objClientData, interp,
						 cmdObjc, cmdObjv);
		} else {
			Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
						"invalid command name \"",
						cmdName, "\"", (char *) NULL);
			result = TCL_ERROR;
		}
		Tcl_DecrRefCount(unknownObj);
	} else {
		Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
			"embedded NULL in command name after \"",
			cmdName, "\"", (char *) NULL);
		result = TCL_ERROR;
	}

	ckfree(newMem);
	return result;
}
