# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================
#doc {amanip lappend} cmd {
#amanip lappend arrayName list
#} example {
#	% array set try {a 1 b 2}
#	% amanip lappend try {a a c 3}
#	% array get try
#	a {1 a} b 2 c 3
#}
#doc {amanip get} cmd {
#amanip get arrayName list ?default?
#} example {
#	% array set try {a 1 b 2}
#	% amanip get try {b c} def
#	b 2 c def
#}
proc amanip {option args} {
	set len [llength $args]
	switch $option {
		lappend {
			if {$len!=2} {
				error "wrong # args: should be \"amanip lappend arrayName list\""
			}
			upvar [lindex $args 0] var
			foreach {e val} [lindex $args 1] {
				lappend var($e) $val
			}
		}
		get {
			if {($len!=2)&&($len!=3)} {
				error "wrong # args: should be \"amanip get arrayName list ?default?\""
			}
			upvar [lindex $args 0] var
			set result ""
			if {[llength $args] == 3} {
				set default [lindex $args 2]
				foreach e [lindex $args 1] {
					if [info exists var($e)] {
						lappend result $var($e)
					} else {
						lappend result $default
					}
				}
			} else {
				foreach e [lindex $args 1] {
					if [info exists var($e)] {
						lappend result $e $var($e)
					}
				}
			}
			return $result
		}
		default {
			error "bad option \"$option\": should be lappend or get"
		}
	}
}
