proc amanip {option args} {
	set len [llength $args]
	switch $option {
		lappend {
			if {$len!=2} {
				error "wrong # args: should be \"amanip append arrayName list\""
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
			set default [lindex $args 2]
			foreach e [lindex $args 1] {
				if [info exists var($e)] {
					lappend result $e $var($e)
				} else {
					lappend result $e $default
				}
			}
			return $result
		}
		default {
			error "bad option \"$option\": should be lappend or get"
		}
	}
}
