proc major {cmd} {
	set namespace [join $cmd "::"]
	namespace eval $namespace {}
	if {[llength $cmd]==1} {
		proc $cmd {cmd args} "eval ${namespace}::\$cmd \$args"
	} else {
		if {"[info commands [lindex $cmd 0]]"==""} {
			error "major \"[lindex $cmd 0]\" does not exist"
		} 
		set try "eval ${namespace}::\[lindex \$args 0\] \[lrange \$args 1 end\]"
		if {"[info body [lindex $cmd 0]]"=="$try"} {
			error "command \"[lindex $cmd 0]\" is not a major"
		}
	}
}

proc minor {major name args body} {
	set namespace [join $major "::"]
	proc ${namespace}::$name $args $body
}

proc deletemajor {cmd} {
	set namespace [join $cmd "::"]
	namespace delete $namespace	
	rename $cmd {}
}
