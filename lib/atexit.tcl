# Create atexit handler
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

rename exit Extral::exit
proc exit {{returnCode 0}} {
	global Extral::atexit
	if [info exists Extral::atexit] {
		foreach command $Extral::atexit {
			eval $command
		}
	}
	Extral::exit $returnCode
}

auto_load laddnew

Extral::export {atexit} {

proc atexit {action command} {
	variable atexit
	switch $action {
		add {
			laddnew atexit $command
		}
		remove {
			lremove atexit $command
		}
	}
}

}