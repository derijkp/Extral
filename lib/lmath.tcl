# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

Extral::export {lmath} {

proc lmath {option args} {
	switch $option {
		calc {
			if {[llength $args]!=3} {
				error "wrong # args: should be \"lmanip calc list1 action list2\""
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
				error "wrong # args: should be \"lmanip sum list\""
			}
			set result 0
			foreach e1 [lindex $args 0]  {
				set result [expr $result+$e1]
			}
			return $result
		}
		min {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"lmanip min list\""
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
				error "wrong # args: should be \"lmanip max list\""
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
				error "wrong # args: should be \"lmanip cumul list\""
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
				error "wrong # args: should be \"lmanip incr list value\""
			}
			set result ""
			set incr [lindex $args 1]
			foreach e1 [lindex $args 0]  {
				lappend result [expr $e1+$incr]
			}
			return $result
		}
		default {
			error "bad option \"$option\": should be calc, sum, min, max, cumul or incr"
		}
	}
}

}
