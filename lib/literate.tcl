# Code to let a variable iterate over a list
# handy when checking a foreach loop manually
# literate ?variable name? ?list?
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# ===========================================

Extral::export {literate lnext} {

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

}
