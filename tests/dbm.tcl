#!/usr/local/bin/tclsh8.0

test [type] {create} {
	catch {file delete -force db.test}
	dbm create [type] db.test
} {}

test [type] {create error: exists} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm create [type] db.test
} {could not create database "db.test": exists already} 1

test [type] {create error: file exists} {
	catch {file delete -force db.test}
	set f [open db.test w]
	puts $f try
	close $f
	dbm create [type] db.test
} {could not create database "db.test"} 1

test [type] {create error: wrong # args} {
	catch {file delete -force db.test}
	dbm create [type] db.test rw fdg
} {wrong # args: should be "dbm create type database ?mode?"} 1

test [type] {create error: wrong # args} {
	catch {file delete -force db.test}
	dbm create
} {wrong # args: should be "dbm create/open type args"} 1

test [type] {create error: wrong mode} {
	catch {file delete -force db.test}
	dbm create [type] db.test try
} {expected integer but got "try"} 1

test [type] {open} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
} {db}

test [type] {open error # args} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db
} {wrong # args: should be "dbm open type dbcmd database ?read/write?"} 1

test [type] {open error # args} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test read try
} {wrong # args: should be "dbm open type dbcmd database ?read/write?"} 1

test [type] {error: try to write in reader} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test read
	db set try {try it}
} {error: trying to set value from reader} 1

test [type] {set error wrong # args} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test write
	db set try
} {wrong # args: should be "db set key value"} 1

test [type] {set and get} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test write
	db set try {try it}
	db get try
} {try it}

test [type] {set and get error: key not present} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test write
	db set try {try it}
	db get test
} {error: key "test" not found} 1

test [type] {set and get: close by rename in between} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test write
	db set try {try it}
	rename db {}
	dbm open [type] db db.test
	db get try
} {try it}

test [type] {set and get: 2} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test write
	db set try {try it}
	db set try2 {try it twice}
	db get try
} {try it}

test [type] {set and get: binary clean?} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test write
	db set try "try it\0and more"
	string length [db get try]
} {15}

test [type] {try several} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test write
time {
	for {set i 0} {$i<1000} {incr i} {
		db set "try $i" "try it the $i time"
	}
}
time {
	for {set i 0} {$i<1000} {incr i} {
		set try [db get "try $i"]
	}
}
	db get {try 500}
} {try it the 500 time}

test [type] {try several large} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test write
	set data ""
	for {set i 0} {$i<1000} {incr i} {
		lappend data $i
	}
time {
	for {set i 0} {$i<1000} {incr i} {
		db set "try $i" "$i $data"
	}
}
time {
	for {set i 0} {$i<1000} {incr i} {
		set try [db get "try $i"]
	}
}
	lrange [db get {try 500}] 0 1
} {500 0}

catch {file delete -force db.test}

testsummarize

