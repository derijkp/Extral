# Dbm extra commands
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

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
