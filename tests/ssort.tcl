#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test ssort {dict} {
	ssort -dict {a10 a9 b2 a11}
} {a9 a10 a11 b2}

test ssort {normal} {
	ssort {a10 a9 b2 a11}
} {a10 a11 a9 b2}

test ssort {-reflist} {
	ssort -reflist {b c a d} {1 2 3 4}
} {3 1 2 4}

test ssort {error in option} {
	ssort -abc {b c a d} {1 2 3 4}
} {bad option "-abc": must be -ascii, -command, -decreasing, -dictionary, -increasing, -index, -integer, -real, or -reflist} 1

testsummarize

