#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test readfile {basic} {
	set f [open try.txt w]
	puts $f "try it"
	close $f
	readfile try.txt
} {try it
}

test writefile {basic} {
	writefile try.txt "try it"
	set f [open try.txt]
	set c [read $f]
	close $f
	set c
} {try it}

test lwrite-lload {basic} {
	set list {a {b c} {d e}}
	file delete try.txt
	lwrite try.txt $list
	lload try.txt
} {a {b c} {d e}}

testsummarize
