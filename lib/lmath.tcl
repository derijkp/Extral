# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================
#doc lmath title {
#Mathematical commands on whole lists
#}

#doc {lmath lmath_cumul} cmd {
#	lmath_cumul ?list?
#} descr {
#	returns a list containing the cumulative sums of the numbers in the list
#}
proc lmath_cumul {args} {
	if {[llength $args]!=1} {
		error "wrong # args: should be \"lmath_cumul list\""
	}
	set prev 0
	foreach e1 [lindex $args 0]  {
		set prev [expr {$prev+$e1}]
		lappend result $prev
	}
	return $result
}

#doc {lmath lmath_incr} cmd {
#	lmath_incr ?list? ?value?
#} descr {
#	returns a list containing the numbers in the list incremented by value
#}
proc lmath_incr {args} {
	if {[llength $args]!=2} {
		error "wrong # args: should be \"lmath_incr list value\""
	}
	set result ""
	set incr [lindex $args 1]
	foreach e1 [lindex $args 0]  {
		lappend result [expr {$e1+$incr}]
	}
	return $result
}

#doc {lmath lmath_between} cmd {
#	lmath_between ?list? ?min? ?max?
#} descr {
#	returns a list containing the numbers in the list but with a minimum and maximum:
#	Any value higher than the maximum is changed to the maximum.
#	Any value lower than the minimum is changed to the minimum.
#}
proc lmath_between {args} {
	if {[llength $args]!=3} {
		error "wrong # args: should be \"lmath_between list min max\""
	}
	set result ""
	set min [lindex $args 1]
	set max [lindex $args 2]
	if {$max<$min} {
		return -code error "$max must be larger than $min"
	}
	foreach e1 [lindex $args 0]  {
		if {$e1<$min} {
			lappend result $min
		} elseif {$e1>$max} {
			lappend result $max
		} else {
			lappend result $e1
		}
	}
	return $result
}

#doc {lmath lmath_stdev} cmd {
#	lmath_stdev ?list?
#} descr {
#	returns the standard deviation of the numbers in the list.
#}
proc lmath_stdev {args} {
	if {[llength $args]!=1} {
		error "wrong # args: should be \"lmath_dev list\""
	}
	set list [lindex $args 0]
	set mean [lmath_average $list]
	set result 0.0
	foreach e1 $list  {
		set result [expr {$result+pow(($e1-$mean),2)}]
	}
	return [expr {sqrt($result/[llength $list])}]
}
