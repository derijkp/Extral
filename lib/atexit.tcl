# extral.tcl --
#
# Create atexit handler
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

rename exit Extral__exit
proc exit {{returnCode 0}} {
	global Extral__priv
	if [info exists Extral__priv(atexit)] {
		foreach command $Extral__priv(atexit) {
			eval $command
		}
	}
	Extral__exit $returnCode
}

proc atexit {action command} {
	global Extral__priv
	switch $action {
		add {
			laddnew Extral__priv(atexit) $command
		}
		remove {
			lremove Extral__priv(atexit) $command
		}
	}
}
