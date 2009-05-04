#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl
teststart
# testleak 100

testfile list.tcl
testfile lmanip.tcl
testfile lmath.tcl
testfile string.tcl
#testfile convenience.tcl
testfile array.tcl
testfile time.tcl
testfile filing.tcl
testfile cmd.tcl
testfile tempfile.tcl
testfile struct.tcl
testfile map.tcl
testfile map-map.tcl
testfile maptypes.tcl
testfile ssort.tcl
testfile is.tcl

testsummarize
