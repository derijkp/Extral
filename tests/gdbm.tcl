#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl
proc type {} {return gdbm}

source dbm.tcl

test [type] {open options: -fast} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test -fast
	db set try 1
	db set try2 2
	db get try
} {1}

test [type] {open options: -blocksize} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test -blocksize 1024
	db set try 1
	db set try2 2
	db get try
} {1}

test [type] {open options: -blocksize error} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test -blocksize
} {no parameter given for option: "-blocksize"} 1


catch {file delete -force db.test}

testsummarize

