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

proc Extral::types {} {
	set list ""
	foreach item [lunion [array names ::auto_index ::Extral::dbmtype_*] [info commands ::Extral::dbmtype_*]] {
		if [uplevel #0 $item] {
			regsub ::Extral::dbmtype_ $item {} item
			lappend list $item
		}
	}
	return $list
}

proc Extral::loaddbm {type} {
	if [info exists ::Extral::dbm_loaded_types($type)] {
		return
	}
	if [catch {uplevel #0 ::Extral::dbminittype_$type} result] {
		return -code error -errorinfo $::errorInfo "Could not load type \"$type\": $result"
	}
	set ::Extral::dbm_loaded_types($type) 1
}

if {"[info commands dbm]" != ""} {
	set Extral::temp [dbm implementation]
} else {
	set Extral::temp tcl
}
if {"$Extral::temp" == "tcl"} {

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
			Extral::loaddbm $type
			Extral::${type}__open $object $readonly $database [lrange $args 3 end]
			set ::Extral::dbm($object,type) $type
			set ::Extral::dbm($object,readonly) $readonly
			proc ::$object {args} "Extral::dbmcmd $object \$args"
			return $object
		}
		create {
			if {$len < 2} {
				return -code error "wrong # args: should be \"dbm create type database ?options?\""
			}
			set type [lindex $args 0]
			Extral::loaddbm $type
			Extral::${type}__create [lindex $args 1] [lrange $args 2 end]
		}
		types {
			return [::Extral::types]
		}
		implementation {
			return tcl
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
#doc {dbm dbmcmd keys} cmd {
#	dbmcmd keys ?pattern?
#} descr {
#		return the keys in the database.
#}
#doc {dbm dbmcmd close} cmd {
#	dbmcmd close
#} descr {
#		close the database. The dbmcmd will be removed
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
			return [Extral::${type}__set $object [lindex $arg 1] [lindex $arg 2]]
		}
		get {
			if {($len != 2)&&($len != 3)} {
				return -code error "wrong # args: should be \"$object get key ?default?\""
			}
			if {$len == 2} {
				return [Extral::${type}__get $object [lindex $arg 1]]
			} else {
				set error [catch {Extral::${type}__get $object [lindex $arg 1]} result]
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
			return [Extral::${type}__unset $object [lindex $arg 1]]
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
			return [Extral::${type}__keys $object $pattern]
		}
		sync {
			if {$len != 1} {
				return -code error "wrong # args: should be \"$object sync\""
			}
			return [Extral::${type}__sync $object]
		}
		reorganize {
			if {$len != 1} {
				return -code error "wrong # args: should be \"$object reorganize\""
			}
			return [Extral::${type}__reorganize $object]
		}
		close {
			if {$len != 1} {
				return -code error "wrong # args: should be \"$object close\""
			}
			Extral::${type}__close $object
			unset ::Extral::dbm($object,type)
			unset ::Extral::dbm($object,readonly)
			rename $object {}
		}
		default {
			return -code error "bad option \"[lindex $arg 0]\": must be one of set, get, unset, keys, sync or reorganize"
		}
	}
}

}
