# Dbm support
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

#doc dbm title {
#Dbm
#} shortdescr {
#Support for various dbms (simple databses)
#} descr {
#The database support in dbm can have different backends: availble backend 
#systems are
#<dl>
#<dt>fdbm
#<dd>file system based; slow but easy, relatively safe and very portable
#<dt>gdbm
#<dd>using the gdbm routines; as a loadable module (gdbm.so)
#<dt>bsddbm
#<dd>using the bsd db routines; as a loadable module (bsddbm.so)
#this seems to be the faster and more reliable than gdbm
#</dl>
#Several of the commands can have options depending on the type of dbm.
#New backends can be added (in C) using the ExtraL_DbmCreateType function.
#The database code can also be used from C using the ExtraL_DbmOpen function.
#}

Extral::export loaddbm {
	proc loaddbm {name} {
		variable dir
		if [catch {source [file join $dir dbm $name.tcl]}] {
			return -code error "could not load dbm type \"$name\""
		}
		source [file join $dir dbm $name.tcl]
	}
}

proc fdbm__create {database arg} {
	if {"$arg" != ""} {
		return  -code error "fdbm create has no options"
	}
	if [file exists $database] {
		return -code error "could not create database \"$database\""
	}
	file mkdir $database
}

proc fdbm__open {object readonly database arg} {
	if {"$arg" != ""} {
		return  -code error "fdbm open has no options"
	}
	set ::Extral::dbm($object,dir) $database
	if ![file exists $database] {
		return -code error "could not open database \"$database\""
	}
}

proc fdbm__close {object} {
	unset ::Extral::dbm($object,dir)
}

proc fdbm__set {object key value} {
	set file [file join [set ::Extral::dbm($object,dir)] $key]
	if [catch {writefile $file $value}] {
		return -code error "could not store key \"$key\""
	}
	return ""
}

proc fdbm__get {object key} {
	set file [file join [set ::Extral::dbm($object,dir)] $key]
	if ![file readable $file] {
		return -code error "error: key \"$key\" not found"
	}
	return [readfile $file]
}

proc fdbm__keys {object {pattern *}} {
	return [dirglob [set ::Extral::dbm($object,dir)] $pattern]
}

proc fdbm__unset {object key} {
	set file [file join [set ::Extral::dbm($object,dir)] $key]
	catch {file delete $file}
	return ""
}

proc fdbm__sync {object} {
}

proc fdbm__reorganize {object} {
}

laddnew ::Extral::dbm(types) fdbm

proc Extral::dbmcmd {object arg} {
	set len [llength $arg]
	if {$len < 1} {
		return -code error "wrong # args: should be \"$object option ...\""
	}
	set type [set ::Extral::dbm($object,type)]
	switch [lindex $arg 0] {
		set {
			if {$len != 3} {
				return -code error "wrong # args: should be \"$object set key value\""
			}
			if [set ::Extral::dbm($object,readonly)] {
				return -code error "error: trying to set value from reader"
			}
			return [${type}__set $object [lindex $arg 1] [lindex $arg 2]]
		}
		get {
			if {($len != 2)&&($len != 3)} {
				return -code error "wrong # args: should be \"$object get key ?default?\""
			}
			if {$len == 2} {
				return [${type}__get $object [lindex $arg 1]]
			} else {
				set error [catch {${type}__get $object [lindex $arg 1]} result]
				if {$error == 1} {
					return [lindex $arg 2]
				} else {
					return $result
				}
			}
		}
		unset {
			if {$len != 2} {
				return -code error "wrong # args: should be \"$object unset key\""
			}
			if [set ::Extral::dbm($object,readonly)] {
				return -code error "error: trying to unset value from reader"
			}
			return [${type}__unset $object [lindex $arg 1]]
		}
		keys {
			if {($len != 1)&&($len != 2)} {
				return -code error "wrong # args: should be \"$object keys ?pattern?\""
			}
			if {$len ==1} {
				set pattern "*"
			} else {
				set pattern [lindex $arg 1]
			}
			return [${type}__keys $object $pattern]
		}
		sync {
			if {$len != 1} {
				return -code error "wrong # args: should be \"$object sync\""
			}
			return [${type}__sync $object]
		}
		reorganize {
			if {$len != 1} {
				return -code error "wrong # args: should be \"$object reorganize\""
			}
			return [${type}__reorganize $object]
		}
		close {
			if {$len != 1} {
				return -code error "wrong # args: should be \"$object close\""
			}
			${type}__close $object
			unset ::Extral::dbm($object,type)
			unset ::Extral::dbm($object,readonly)
			rename $object {}
		}
		default {
			return -code error "bad option \"[lindex $arg 0]\": must be one of set, get, unset, keys, sync or reorganize"
		}
	}
}

#doc {dbm dbm} h2 "dbm command"
#doc {dbm dbm types} cmd {
#dbm types
#} descr {
#	list the available type of database systems; fdbm is provided,
#	gdbm and bsddbm are dynamically loadable
#}
#doc {dbm dbm type} cmd {
#dbm create type database ?options?
#} descr {
#	create a database of the given type (fdbm, gdbm, bsddbm) in 
#	the "file" database. The database is not opened by this command.
#}
#doc {dbm dbm open} cmd {
#dbm open type dbcmd database ?options?
#} descr {
#	open a database for reading (read = default) or reading and 
#	writing (write).
#	type: the type of the database (fdbm, gdbm, bsddbm).
#	dbcmd: the command by which the opened database can be queried
#	database: place of the database in the filing system
#}

proc dbm {cmd args} {
	set len [llength $args]
	switch $cmd {
		open {
			if {$len < 3} {
				return -code error "wrong # args: should be \"dbm open ?-readonly? type dbcmd database ?options?\""
			}
			set readonly [extractbool args -readonly]
			set type [lindex $args 0]
			set object [lindex $args 1]
			set database [lindex $args 2]
			if {[lsearch -exact [set ::Extral::dbm(types)] $type] == -1} {
				return -code error "no such type: \"$type\""
			}
			${type}__open $object $readonly $database [lrange $args 3 end]
			set ::Extral::dbm($object,type) $type
			set ::Extral::dbm($object,readonly) $readonly
			proc $object {args} "Extral::dbmcmd $object \$args"
			return $object
		}
		create {
			if {$len < 2} {
				return -code error "wrong # args: should be \"dbm create type database ?options?\""
			}
			set type [lindex $args 0]
			if {[lsearch -exact [set ::Extral::dbm(types)] $type] == -1} {
				return -code error "no such type: \"$type\""
			}
			${type}__create [lindex $args 1] [lrange $args 2 end]
		}
		types {
			return [set ::Extral::dbm(types)]
		}
		default {
			return -code error "bad option \"$cmd\": must be one of create, open or types"
		}
	}
}

#doc {dbm dbmcmd} h2 {
#database commands
#} descr {
#The dbmcmd created by the "dbm open" above will have the following options:
#}
#doc {dbm dbmcmd set} cmd {
#	dbmcmd set key value
#} descr {
#		set key in the database to value.
#}
#doc {dbm dbmcmd get} cmd {
#	dbmcmd get key
#} descr {
#		get the value associated with key from the database.
#}
#doc {dbm dbmcmd unset} cmd {
#	dbmcmd unset key
#} descr {
#		remove a key from the database.
#}
#doc {dbm dbmcmd sync} cmd {
#	dbmcmd sync
#} descr {
#		sync the database (for those types that need it).
#}
#doc {dbm dbmcmd reorganize} cmd {
#	dbmcmd reorganize
#} descr {
#		reorganize the database (for those types that need it).
#}
