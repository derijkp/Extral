#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test arraytrans {basic} {
	array set a {a 1 b 2 c 3 d 4}
	arraytrans a {a c}
} {1 3}

test arraytrans {basic with default} {
	array set a {a 1 b 2 c 3 d 4}
	arraytrans a {a e} def
} {1 def}

testsummarize

