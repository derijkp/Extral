#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test table {empty table from list} {
	set table [table_fromlist {id a b c} {{a 5 2 3} {b 3 5 4} {c 8 8 9} {d 8 5 8}}]
	table_tolist [table_sort $table {a c}]
} {{b 3 5 4} {a 5 2 3} {d 8 5 8} {c 8 8 9}}

test table {empty table from list} {
	set table [table_fromlist {id a b c} {{a 5 2 3} {b 3 5 4} {c 8 8 9} {d 8 5 8}}]
	table_tolist [table_sort $table {a c} -decreasing]
} {{c 8 8 9} {d 8 5 8} {a 5 2 3} {b 3 5 4}}

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
