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

proc lmerge {args} {
	if {([llength $args]!=2)&&([llength $args]!=3)} {
		error "wrong # args: should be \"lmerge list1 list2 ?spacing?\""
	}
	set result ""
	if {[llength $args]==3} {
		set spacing [lindex $args 2]
		set list2 [lindex $args 1]
		set c $spacing
		foreach e1 [lindex $args 0] {
			lappend result $e1
			incr c -1
			if !$c {
				lappend result [lshift list2]
				set c $spacing
			}
		}
		return $result
		
	} else {
		foreach e1 [lindex $args 0] e2 [lindex $args 1] {
			lappend result $e1 $e2
		}
		return $result
	}
}

proc lunmerge {args} {
	if {([llength $args]<1)&&([llength $args]>3)} {
		error "wrong # args: should be \"lunmerge list ?spacing? ?var?\""
	}
	set result ""
	if {[llength $args]==3} {
		upvar [lindex $args 2] var
		set var ""
	}
	if {[llength $args]>1} {
		set spacing [lindex $args 1]
	} else {
		set spacing 1
	}
	if {$spacing==1} {
		foreach {e1 e2} [lindex $args 0] {
			lappend result $e1
			if [info exists var] {lappend var $e2}
		}
		return $result
	} else {
		set c $spacing
		foreach e1 [lindex $args 0] {
			if !$c {
				if [info exists var] {lappend var $e1}
				set c $spacing
			} else {
				lappend result $e1
				incr c -1
			}
		}
		return $result
		
	}
}

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
