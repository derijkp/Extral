# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

Extral::export {major minor deletemajor deleteminor} {

namespace eval majors {}

proc execmajor {execspace args} {
	set cmd ""
	set i 0
	while 1 {
		set name [lrange $args 0 $i]
		if ![info exsists major($name)] break
	}
	if {$i
		incr i
	set args [lrange 
	set namespace [join $cmd "::"]
	namespace eval majors::$namespace {}
	set command {
		if [catch {namespace eval @execspace@ @namespace@$cmd $args} result] {
			regsub @namespace@ $result "@major@ " result
			return -code error $result
		} else {
			return $result
		}
	}
	regsub -all {@execspace@} $command [list [uplevel namespace current]] command
	regsub -all {@namespace@} $command [list [namespace current]::majors::${namespace}::] command
	regsub -all {@major@} $command [list $cmd] command
	proc majors::$cmd {cmd args} $command
	namespace eval majors namespace export $cmd
	namespace eval [uplevel namespace current] namespace import [namespace current]::majors::$cmd
}

proc major {major} {
	set namespace [join $cmd "::"]
	namespace eval majors::$namespace {}
	set command {
		if [catch {namespace eval @execspace@ @namespace@$cmd $args} result] {
			regsub @namespace@ $result "@major@ " result
			return -code error $result
		} else {
			return $result
		}
	}
	regsub -all {@execspace@} $command [list [uplevel namespace current]] command
	regsub -all {@namespace@} $command [list [namespace current]::majors::${namespace}::] command
	regsub -all {@major@} $command [list $cmd] command
	proc majors::$cmd {cmd args} $command
	namespace eval majors namespace export $cmd
	namespace eval [uplevel namespace current] namespace import [namespace current]::majors::$cmd
}

proc minor {major name args body} {
	set namespace [join $major "::"]
	puts $namespace
	proc majors::${namespace}::$name $args $body
}

proc deletemajor {cmd} {
	set namespace [join $cmd "::"]
	namespace delete majors::$namespace	
	rename majors::$cmd {}
}

proc deleteminor {cmd} {
	set namespace [join $cmd "::"]
	rename majors::$namespace {}
}

}
