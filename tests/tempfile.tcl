#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test struct {tempfile} {
	set tempfile [tempfile get]
	expr {[string length $tempfile] > 0}
} 1

testsummarize
