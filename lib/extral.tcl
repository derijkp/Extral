# File containing the Tcl part of the list commands in the Extral extension
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================
#doc listcommands title {
#General list manipulation commands
#}

if 0 {
proc Extral::lremove {} {}
proc Extral::lpush {} {}
proc Extral::lunshift {} {}
proc Extral::lset {} {}
proc Extral::larrayset {} {}
proc Extral::lcommon {} {}
proc Extral::lunion {} {}
proc Extral::leor {} {}
proc Extral::laddnew {} {}
}
Extral::export {
	lremove lpush lunshift lset larrayset lcommon lunion leor laddnew oneof
} {

#doc {listcommands lremove} cmd {
#lremove listName ?item? ...
#} descr {
#removes the items from the list
#}
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

#doc {listcommands lpush} cmd {
#lpush listName ?item? ?position?
#} descr {
#	opposite of lpop.
#}
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

#doc {listcommands lunshift} cmd {
#lunshift listName ?item?
#} descr {
#	opposite of lshift: prepends ?item? to the list.
#}
proc lunshift {ulist item} {
	upvar $ulist list
	if {[llength $list]==0} {
		lappend list $item
	} else {
		set temp [lindex $list 0]
		set list [lreplace $list 0 0 $item $temp]
	}
}

#doc {listcommands lset} cmd {
#lset listName ?item? ?indexlist?
#} descr {
#	sets all elements of the list at the given indices to value ?item?
#}
proc lset {listref value indices} {
	upvar $listref list
	foreach index $indices {
		set list [lreplace $list $index $index $value]
	}
	return $list
}

#doc {listcommands larrayset} cmd {
#larrayset array varlist valuelist
#} descr {
#	sets the values of valuelist to the respective elements in varlist for
#	the given array
#}
proc larrayset {array varlist valuelist} {
	upvar $array v
	foreach var $varlist value $valuelist {
		set v($var) $value
	}
}

#doc {listcommands lcommon} cmd {
#lcommon list list ...
#} descr {
#	returns the common elements of the lists
#}
proc lcommon {args} {
	set result [lsort [::Extral::lpop args]]
	foreach list $args {
		llremove -sorted $result [lsort $list] result
	}
	return $result
}

#doc {listcommands lunion} cmd {
#lunion list list ...
#} descr {
#	returns the union of the lists
#}
proc lunion {args} {
	set result [eval concat $args]
	return [lmanip remdup $result]
}

#doc {listcommands leor} cmd {
#leor list1 list2
#} descr {
#	returns the elements that are not shared between both lists
#}
proc leor {list1 list2} {
	set list1 [lsort $list1]
	set list2 [lsort $list2]
	set result [llremove $list1 $list2 rem]
	return [concat $result [llremove $list2 $rem]]
}

#doc {listcommands laddnew} cmd {
#laddnew listName ?item? ...
#} descr {
#	adds the items to the list if not already there
#}
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

#doc {listcommands oneof} cmd {
#oneof element list
#} descr {
#	returns 1 if the lement occurs in the list, 0 if it does not.
#}
proc oneof {item list} {
	if {[lsearch -exact $list $item] == -1} {
		return 0
	} else {
		return 1
	}
}
}
