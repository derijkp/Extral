# Initialisation of the Extral package
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

#
# Load the shared library if present
# If not, Tcl code will be loaded when necessary
#

namespace eval Extral {
	proc export {items cmds} {
		eval $cmds
		eval namespace export $items		
		namespace eval [namespace parent] \
			[list foreach item $items {namespace import Extral::$item}]
	}
}

if [file exists [file join ${Extral::dir} extral[info sharedlibextension]]] {
	if {"[info commands Extral::lpop]" == ""} {
		load [file join ${Extral::dir} extral[info sharedlibextension]]
		namespace eval Extral {
			foreach command {
				lpop lshift lfind lsub lcor
				lremdup llremove lunmerge lmerge
				scantime formattime 
				leval amanip replace ssort ffind
				structlset structlunset structlget structlfields structlfind
				dbm
			} {
				namespace export $command
			}
		}
		namespace import Extral::*
	}
} else {
	set Extral::noc 1
	source [file join ${Extral::dir} lib noc.tcl]
}

#
# The lib dir contains the Tcl code defining the public Extral 
# functions. The lib dir is added to the auto_path so that
# these functions will be loaded on demand. The export function
# is used in the Tcl files to automatically export the
# public functions from the Extral namespace, and import them 
# in the parent namespace.
#

lappend auto_path [file join ${Extral::dir} lib]

source [file join ${Extral::dir} lib atexit.tcl]
