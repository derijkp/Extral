# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================
#doc {lmath calc} cmd {
#	lmath calc ?list1? ?action? ?list2?
#} descr {
#	makes calculations on lists. Action can be one of +, -, * or /
#} example {
#	% lmath calc {1 2 3.2 4} + {1 2 3.2 4}
#	2.0 4.0 6.4 8.0
#}
#doc {lmath sum} cmd {
#	lmath sum ?list?
#} descr {
#	returns the sum of the numbers in the list
#}
#doc {lmath min} cmd {
#	lmath min ?list?
#} descr {
#	returns the minimum of the numbers in the list
#}
#doc {lmath max} cmd {
#	lmath max ?list?
#} descr {
#	returns the maximum of the numbers in the list
#}
#doc {lmath cumul} cmd {
#	lmath cumul ?list?
#} descr {
#	returns a list containing the cumulative sums of the numbers in the list
#}
#doc {lmath incr} cmd {
#	lmath incr ?list? ?value?
#} descr {
#	returns a list containing the numbers in the list incremented by value
#}
#doc {lmath between} cmd {
#	lmath between ?list? ?min? ?max?
#} descr {
#	returns a list containing the numbers in the list but with a minimum and maximum:
#	Any value higher than the maximum is changed to the maximum.
#	Any value lower than the minimum is changed to the minimum.
#}

if 0 {
proc Extral::lmath {} {}
}
Extral::export {lmath} {

proc lmath {option args} {
	switch $option {
		calc {
			if {[llength $args]!=3} {
				error "wrong # args: should be \"lmath calc list1 action list2\""
			}
			set result ""
			set calc [lindex $args 1]
			foreach e1 [lindex $args 0] e2 [lindex $args 2] {
				lappend result [expr $e1 $calc $e2]
			}
			return $result
		}
		sum {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"lmath sum list\""
			}
			set result 0
			foreach e1 [lindex $args 0]  {
				set result [expr $result+$e1]
			}
			return $result
		}
		min {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"lmath min list\""
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
		max {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"lmath max list\""
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
		cumul {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"lmath cumul list\""
			}
			set prev 0
			foreach e1 [lindex $args 0]  {
				set prev [expr $prev+$e1]
				lappend result $prev
			}
			return $result
		}
		incr {
			if {[llength $args]!=2} {
				error "wrong # args: should be \"lmath incr list value\""
			}
			set result ""
			set incr [lindex $args 1]
			foreach e1 [lindex $args 0]  {
				lappend result [expr $e1+$incr]
			}
			return $result
		}
		between {
			if {[llength $args]!=3} {
				error "wrong # args: should be \"lmath between list min max\""
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
		default {
			error "bad option \"$option\": should be calc, sum, min, max, cumul or incr"
		}
	}
}

}
