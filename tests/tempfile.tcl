#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test struct {tempfile} {
	set tempfile [temfile get]
} {1}


testsummarize

