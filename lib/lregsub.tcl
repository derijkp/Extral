# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

Extral::export {lregsub} {
proc lregsub {args} {
	set len [llength $args]
	if {$len<3} {
		error "wrong # args: should be \"lregsub ?switches? exp list subSpec\""
	}
	set result ""
	incr len -1
	set sub [lindex $args $len]
	incr len -1
	set list [lindex $args $len]
	incr len -1
	set expr [lindex $args $len]
	if {$len>0} {
		incr len -1
		set args [lrange $args 0 $len]
		foreach e $list {
			eval regsub $args {$expr $e $sub e}
			lappend result $e
		}
	} else {
		foreach e $list {
			regsub $expr $e $sub e
			lappend result $e
		}
	}
	return $result
}

}
