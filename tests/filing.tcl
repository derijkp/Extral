#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test file_read {basic} {
	set f [open try.txt w]
	puts $f "try it"
	close $f
	file_read try.txt
} {try it
}

test file_write {basic} {
	file_write try.txt "try it"
	set f [open try.txt]
	set c [read $f]
	close $f
	set c
} {try it}

test file_read {binary} {
	set f [open try.txt w]
	puts $f "try\n\r\nit"
	close $f
	file_read -translation binary try.txt
} "try\n\r\nit\n"

test list_write-list_load {basic} {
	set list {a {b c} {d e}}
	file delete try.txt
	list_write try.txt $list
	list_load try.txt
} {a {b c} {d e}}

test file_fullpath {basic} {
	file_fullpath /tmp/try.txt
} /tmp/try.txt

test file_fullpath {basic} {
	file_fullpath try.txt
} [file join [pwd] try.txt]

testsummarize
