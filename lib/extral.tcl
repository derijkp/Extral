# File containing the Tcl part of the list commands in the Extral extension
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

Extral::export {
	lremove lpush lunshift lset larrayset lcommon lunion leor laddnew
} {

if ![info exists noc] {
proc lremove {list args} {
	if {"$args"==""} {
		return $list
	}
	set result ""
	foreach item $list {
		set pos [lsearch $args $item]
		if {$pos==-1} {
			lappend result $item
		}
	}
	return $result
}
} else {
proc lremove {list args} {
	if {"$args"==""} {
		return $list
	} else {
		return [llremove $list $args]
	}
}
}

proc lpush {ulist item {pos {}}} {
	upvar $ulist list
	if {![info exists list]||("$list"=="")||("$pos"=="")} { 
		lappend list $item
	} else {
		if {$pos>=[llength $list]} {
			return $list
		}
		set temp [lindex $list $pos]
		set list [lreplace $list $pos $pos $temp $item]
	}
}

proc lunshift {ulist item} {
	upvar $ulist list
	if {[llength $list]==0} {
		lappend list $item
	} else {
		set temp [lindex $list 0]
		set list [lreplace $list 0 0 $item $temp]
	}
}

proc lset {listref value indices} {
	upvar $listref list
	foreach index $indices {
		set list [lreplace $list $index $index $value]
	}
	return $list
}

proc larrayset {array varlist valuelist} {
	upvar $array v
	foreach var $varlist value $valuelist {
		set v($var) $value
	}
}

proc lcommon {args} {
	set result [lpop args]
	set result [lmanip remdup $result]
	foreach arg $args {
		set list [lcor $arg $result]]
		set result [lsub $arg [lcor $arg $result]]
	}
	return [lmanip remdup $result]
}

proc lunion {args} {
	set result [lmanip join $args { } all]
	return [lmanip remdup $result]
}

proc leor {list1 list2} {
	set cor [lcor $list1 $list2]
	set exclusive [lfind $cor -1]
	set join [lsub $cor -exclude $exclusive]
	set result [lsub $list1 -exclude $join]
	eval lappend result [lsub $list2 $exclusive]
}

proc laddnew {listref args} {
	upvar $listref list
	if ![info exists list] {set list ""}
	foreach item $args {
		if {[lsearch -exact $list $item] < 0} {
			lappend list $item
		}
	}
	return $list
}

}
