#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

foreach {v r} {
	2 1 a 0 1.8 0 1.0 0 -1 1
} {
	test list_regsub "isint $v" "isint $v" $r
}

foreach {v r} {
	2 1  a 0  1.8 1  -1.8 1
} {
	test list_regsub "isdouble $v" "isdouble $v" $r
}

testsummarize
