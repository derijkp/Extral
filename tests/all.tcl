#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

if [info exists env(TCL_TEST_DIR)] {
	cd $env(TCL_TEST_DIR)
}
if ![info exists env(TCL_TEST_ONLYERRORS)] {
	proc alltest file {
		global currenttest
		set currenttest $file
		puts "-----------------------------------------------------"
		puts "Test file $file"
		puts "-----------------------------------------------------"
		uplevel #0 source $file
	}
} else {
	proc alltest file {
		uplevel #0 source $file
	}
}

alltest list.tcl
alltest lmanip.tcl
alltest string.tcl
alltest convenience.tcl
alltest array.tcl
alltest date.tcl
alltest filing.tcl
alltest cmd.tcl
alltest tempfile.tcl
alltest struct.tcl
alltest structlist.tcl
alltest structlstruct.tcl
alltest structltypes.tcl
alltest ssort.tcl
