namespace eval package {}

proc package::architecture {} {
	global tcl_platform
	if {[string equal $tcl_platform(platform) unix]} {
		return $tcl_platform(os)-$tcl_platform(machine)
	} else {
		return $tcl_platform(platform)-$tcl_platform(machine)
	}
}

proc ::package::init {dir name {testcmd {}} {noc_lib {}}} {
	global tcl_platform noc
	#
	# Try to find the compiled library in several places
	#
	if {[string equal $testcmd ""] || ![string equal [info commands $testcmd] $testcmd]} {
		if {"$tcl_platform(platform)" == "windows"} {
			set libpattern \{lib,\}$name\[0-9\]*[info sharedlibextension]
		} else {
			set libpattern lib${name}\[0-9\]*[info sharedlibextension]
		}
		foreach libfile [list \
			[file join $dir [package::architecture] $libpattern] \
			[file join $dir build $libpattern] \
			[file join $dir $libpattern] \
			[file join $dir .. $libpattern]
		] {
			set libfile [lindex [glob -nocomplain $libfile] 0]
			if [file exists $libfile] {break}
		}
		#
		# Load the shared library if present
		# If not, Tcl code will be loaded when necessary
		#
		if [file exists $libfile] {
			if {"[info commands $testcmd]" == ""} {
				namespace eval :: [list load $libfile]
			}
		} else {
			set noc 1
			if {![string equal $noc_lib ""]} {
				lappend ::auto_path $noc_lib
			}
		}
	}
}

