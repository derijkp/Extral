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

namespace eval Extral {}

if [file exists [file join ${Extral::dir} extral[info sharedlibextension]]] {
	if {"[info commands Extral::lpop]" == ""} {
		load [file join ${Extral::dir} extral[info sharedlibextension]]
	}
} else {
	set Extral::noc 1
	source [file join ${Extral::dir} lib noc.tcl]
}

#
# The lib dir contains the Tcl code defining the public Extral 
# functions. The lib dir is added to the auto_path so that
# these functions will be loaded on demand.
#

lappend auto_path [file join ${Extral::dir} lib]

source [file join ${Extral::dir} lib atexit.tcl]
