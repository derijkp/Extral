# extral.tcl --
#
# File containing the Tcl part of the list commands in the Extral extension
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

proc lload {filename} {
	set f [open $filename "r"]
	set result [split [read $f] "\n"]
	close $f
	return $result
}

proc lwrite {filename list} {
	set f [open $filename "a"]
	puts $f [join $list "\n"] nonewline
	close $f
}

proc lpush {ulist item {pos {}}} {
	upvar $ulist list
	if {("$list"=="")||("$pos"=="")} { 
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
	uplevel "array set $array \[lmanip join \[lmanip merge [list $varlist] [list $valuelist]\] \{ \} all\]"
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

if ![info exists Extral__noc] {
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
} else {
proc lremove {listref args} {
	upvar $listref list
	foreach item $args {
		while 1 {
			set pos [lsearch -exact $list $item]
			if {$pos==-1} break
			set list [lreplace $list $pos $pos]
		}
	}
	return $list	
}
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
