# extral.tcl --
#
# File containing the Tcl part of the list commands in the Extral extension
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

proc lpop {ulist args} {
	upvar $ulist list
	if {[llength $args]>1} {
		error "Format is \"lpop listname ?position?\""
	}
	if {[llength $list]==0} {return ""}
	if {"$args"==""} { 
		set end [llength $list]
		incr end -1
		set result [lindex $list $end]
		incr end -1
		set list [lrange $list 0 $end]
	} else {
		set pos $args
		if {$pos>=[llength $list]} {
			return {}
		}
		set result [lindex $list $pos]
		set list [lsub $list -exclude $pos]
	}
	return $result
}

proc lpush {ulist item args} {
	upvar $ulist list
	if {[llength $args]>1} {
		error "Format is \"lpush listname item ?position?\""
	}
	if {"$args"==""} { 
		lappend list $item
	} else {
		set pos [expr $args-1]
		if {$pos>=[llength $list]} {
			return {}
		}
		set temp [lindex $list $pos]
		set list [lreplace $list $pos $pos $temp $item]
	}
}

proc lshift {ulist} {
	upvar $ulist list
	if {[llength $list]==0} {return ""}
	set result [lindex $list 0]
	set list [lrange $list 1 end]
	return $result
}

proc lunshift {ulist item} {
	upvar $ulist list
	if {[llength $list]==0} {
		set list $item
		return $item
	}
	set temp [lindex $list 0]
	set list [lreplace $list 0 0 $item $temp]
}

proc lset {listref value indices} {
	upvar $listref list
	foreach index $indices {
		set list [lreplace $list $index $index $value]
	}
	return $list
}

proc larrayset {array varlist valuelist} {
	uplevel "array set $array \[lmanip join \[lmanip merge [list $varlist] [list $valuelist]\] \{ \} all\]"
}

proc leor {list1 list2} {
	set cor [lcor $list1 $list2]
	set exclusive [lfind $cor -1]
	set join [lsub $cor -exclude $exclusive]
	set result [lsub $list1 -exclude $join]
	eval lappend result [lsub $list2 $exclusive]
}

proc lunion {args} {
	set result [lmanip join $args { } all]
	return [lmanip remdup $result]
}

proc lcommon {args} {
	set result [lpop args]
	foreach arg $args {
		set result [lsub $arg [lcor $arg $result]]
	}
	return [lmanip remdup $result]
}

proc lremove {listref args} {
	upvar $listref list
	foreach item $args {
		set poss [lfind $list $item]
		if {"$poss"!=""} {
			set list [lsub $list -exclude $poss]
		}
	}
	return $list
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

proc lcorsort {list sortlist args} {
	set sorted [eval lsort $args {$sortlist}]
	set cor [lcor $sortlist $sorted]
	return [lsub $list $cor]
}

# Code to let a variable iterate over a list
# handy when checking a foreach loop manually
# literate ?variable name? ?list?
# ===========================================

proc literate {var list} {
	global extraL__Priv_iterate
	upvar $var ref
	set extraL__Priv_iterate($var:list) $list
	set extraL__Priv_iterate($var:len) [llength $list]
	set extraL__Priv_iterate($var:pos) 0
	set ref [lindex $extraL__Priv_iterate($var:list) 0]
}

proc lnext {var} {
	global extraL__Priv_iterate
	upvar $var ref
	if ![info exists extraL__Priv_iterate($var:list)] {
		error "No iteration over $var set"
	}
	incr extraL__Priv_iterate($var:pos)
	if {$extraL__Priv_iterate($var:pos)==$extraL__Priv_iterate($var:len)} {
		unset extraL__Priv_iterate($var:list)
		unset extraL__Priv_iterate($var:len)
		unset extraL__Priv_iterate($var:pos)
		unset ref
	} else {
		set ref [lindex $extraL__Priv_iterate($var:list) $extraL__Priv_iterate($var:pos)]
	}
}
