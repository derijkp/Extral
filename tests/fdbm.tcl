#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl
proc type {} {return fdbm}

test [type] {types} {
	dbm types
} {fdbm}

source dbm.tcl

test fdbm {create error: no options} {
	catch {file delete -force db.test}
	dbm create fdbm db.test -mode 000
} {fdbm create has no options} 1

test fdbm {create error: no options} {
	catch {file delete -force db.test}
	dbm open fdbm db db.test -mode 000
} {fdbm open has no options} 1


catch {file delete -force db.test}
testsummarize
