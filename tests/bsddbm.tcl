#!/usr/local/bin/tclsh8.0
source tools.tcl
proc type {} {return bsddbm}

source dbm.tcl

test [type] {create error: wrong mode} {
	catch {file delete -force db.test}
	dbm create [type] db.test -mode try
} {expected integer but got "try"} 1

test [type] {create options: -btree} {
	catch {file delete -force db.test}
	dbm create [type] db.test -btree
	dbm open [type] db db.test
	db set try 1
	db set try2 2
	db get try
} {1}

test [type] {create options: -hash} {
	catch {file delete -force db.test}
	dbm create [type] db.test -hash
	dbm open [type] db db.test -hash
	db set try 1
	db set try2 2
	db get try
} {1}


catch {file delete -force db.test}
testsummarize
