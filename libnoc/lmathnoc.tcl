# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

#doc {lmath lmath_calc} cmd {
#	lmath_calc ?list1? ?action? ?list2?
#} descr {
#	makes calculations on lists. Action can be one of +, -, * or /
#	list2 may also be a single number, in which case
#	this number will be used for all elements in list1
#} example {
#	% lmath_calc {1 2 3.2 4} + {1 2 3.2 4}
#	2.0 4.0 6.4 8.0
#}
proc lmath_calc {args} {
	if {[llength $args]!=3} {
		error "wrong # args: should be \"lmath_calc list1 action list2\""
	}
	set result ""
	set calc [lindex $args 1]
	set l2 [lindex $args 2]
	if {[llength $l2] != 1} {
		foreach e1 [lindex $args 0] e2 $l2 {
			lappend result [expr $e1 $calc $e2]
		}
	} else {
		set e2 [lindex $l2 0]
		foreach e1 [lindex $args 0] {
			lappend result [expr $e1 $calc $e2]
		}
	}
	return $result
}

#doc {lmath lmath_sum} cmd {
#	lmath_sum ?list?
#} descr {
#	returns the sum of the numbers in the list
#}
proc lmath_sum {args} {
	if {[llength $args]!=1} {
		error "wrong # args: should be \"lmath_sum list\""
	}
	set result 0
	foreach e1 [lindex $args 0]  {
		set result [expr {$result+$e1}]
	}
	return $result
}

#doc {lmath lmath_average} cmd {
#	lmath_between ?list?
#} descr {
#	returns the average of the numbers in the list.
#}
proc lmath_average {args} {
	if {[llength $args]!=1} {
		error "wrong # args: should be \"lmath_average list\""
	}
	set result 0.0
	set list [lindex $args 0]
	foreach e1 $list  {
		set result [expr {$result+$e1}]
	}
	return [expr {$result/[llength $list]}]
}

#doc {lmath lmath_min} cmd {
#	lmath_min ?list?
#} descr {
#	returns the minimum of the numbers in the list
#}
proc lmath_min {args} {
	if {[llength $args]!=1} {
		error "wrong # args: should be \"lmath_min list\""
	}
	set list [lindex $args 0]
	set result [lindex $list 0]
	foreach e1 $list  {
		if {$e1<$result} {
			set result $e1
		}
	}
	return $result
}

#doc {lmath lmath_max} cmd {
#	lmath_max ?list?
#} descr {
#	returns the maximum of the numbers in the list
#}
proc lmath_max {args} {
	if {[llength $args]!=1} {
		error "wrong # args: should be \"lmath_max list\""
	}
	set list [lindex $args 0]
	set result [lindex $list 0]
	foreach e1 $list  {
		if {$e1>$result} {
			set result $e1
		}
	}
	return $result
}

