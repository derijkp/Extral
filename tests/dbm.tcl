
test [type] {create} {
	catch {file delete -force db.test}
	dbm create [type] db.test
} {}

test [type] {create error: exists} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm create [type] db.test
} {could not create database "db.test"} 1

test [type] {create error: file exists} {
	catch {file delete -force db.test}
	set f [open db.test w]
	puts $f try
	close $f
	dbm create [type] db.test
} {could not create database "db.test"} 1

test [type] {create error: wrong # args} {
	catch {file delete -force db.test}
	dbm create
} {wrong # args: should be "dbm create type database ?options?"} 1

test [type] {open} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
} {db}

test [type] {open error # args} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db
} {wrong # args: should be "dbm open ?-readonly? type dbcmd database ?options?"} 1

test [type] {error: try to set in reader} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open -readonly [type] db db.test
	db set try {try it}
} {error: trying to set value from reader} 1

test [type] {error: try to unset in reader} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db set try 1
	db close
	dbm open -readonly [type] db db.test
	db unset try
} {error: trying to unset value from reader} 1

test [type] {set error wrong # args} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db set try
} {wrong # args: should be "db set key value"} 1

test [type] {set and get} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db set try {try it}
	db get try
} {try it}

test [type] {get default} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db get try ?
} {?}

test [type] {set and get error: key not present} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db set try {try it}
	db get test
} {error: key "test" not found} 1

test [type] {set and get: close by rename in between} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db set try {try it}
	rename db {}
	dbm open [type] db db.test
	db get try
} {try it}

test [type] {set and get: 2} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db set try {try it}
	db set try2 {try it twice}
	db get try
} {try it}

test [type] {set and get: binary clean?} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db set try "try it\0and more"
	string length [db get try]
} {15}

test [type] {try several} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
puts [time {
	for {set i 0} {$i<1000} {incr i} {
		db set "try $i" "try it the $i time"
	}
}]
puts [time {
	for {set i 0} {$i<1000} {incr i} {
		set try [db get "try $i"]
	}
}]
	db get {try 500}
} {try it the 500 time}

test [type] {try several large} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	set data ""
	for {set i 0} {$i<1000} {incr i} {
		lappend data $i
	}
puts [time {
	for {set i 0} {$i<1000} {incr i} {
		db set "try $i" "$i $data"
	}
}]
puts [time {
	for {set i 0} {$i<1000} {incr i} {
		set try [db get "try $i"]
	}
}]
	lrange [db get {try 500}] 0 1
} {500 0}

test [type] {keys} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db set try {try it}
	db set {try 2} {try it twice}
	db set test {try it twice}
	lsort [db keys]
} {test try {try 2}}

test [type] {keys with pattern} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db set try {try it}
	db set {try 2} {try it twice}
	db set test {try it twice}
	lsort [db keys try*]
} {try {try 2}}

test [type] {keys with pattern} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	for {set i 0} {$i<50} {incr i} {
		db set "try $i" "$i time"
	}
	lsort [db keys *0]
} {{try 0} {try 10} {try 20} {try 30} {try 40}}

test [type] {keys with pattern} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	foreach {key val} {allo ta btree tb cello tc bt tb do td} {
		db set $key $val
	}
	lsort [db keys b*]
} {bt btree}

test [type] {unset: test with get} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db set try {try it}
	db set {try 2} {try it twice}
	db unset try
	db get try
} {error: key "try" not found} 1

test [type] {unset: test with keys} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	for {set i 0} {$i<50} {incr i} {
		db set "try $i" "$i time"
	}
	db unset {try 20}
	lsort [db keys *0]
} {{try 0} {try 10} {try 30} {try 40}}

test [type] {error: no key} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db unset
} {wrong # args: should be "db unset key"} 1

test [type] {unsetting non existing key should not give an error} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db unset try
} {}

test [type] {close by renaming} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	rename db {}
} {}

test [type] {try to open non existing database} {
	catch {file delete -force db.test}
	dbm open [type] db db.test
} {could not open database "db.test"} 1

test [type] {error in dbm access cmd} {
	catch {file delete -force db.test}
	dbm try
} {bad option "try": must be one of create, open or types} 1

test [type] {error in db access cmd} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	db try
} {bad option "try": must be one of set, get, unset, keys, sync or reorganize} 1

test [type] {see if sync doesn't fail} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	for {set i 0} {$i<100} {incr i} {
		db set "try $i" "try it the $i time"
	}
	db sync
} {}

test [type] {see if reorganize doesn't fail} {
	catch {file delete -force db.test}
	dbm create [type] db.test
	dbm open [type] db db.test
	for {set i 0} {$i<100} {incr i} {
		db set "try $i" "try it the $i time"
	}
	db unset {try 10}
	db unset {try 20}
	db reorganize
	db get {try 18}
} {try it the 18 time}
