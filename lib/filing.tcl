# filing.tcl --
#
# Some filing functions I often use, so they ended up here
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

proc mkdir {args} {
	eval file mkdir $args
}

proc cp {args} {
	eval file copy -force $args
}

proc mv {args} {
	eval file rename $args
}

proc rm {args} {
	eval file delete $args
}

# Using the Unix tools
# --------------------
proc ls {args} {
	global env tcl_interactive
	if {$tcl_interactive&&[info exists env(LS_OPTIONS)]} {
		set cl $env(LS_OPTIONS)
	} else {
		set cl ""
	}
	while {[string match \-* [lindex $args 0]]} {
		lappend cl [lshift args]
	}
	if {"$args"==""} {set args *}

	eval lappend cl [eval glob -nocomplain $args]
	if {$tcl_interactive&&([info level]==1)} {
		eval exec ls $cl >&@stdout
	} else {
		eval exec ls $cl
	}
}

proc chmod {args} {
	set cl ""
	while {[string match \-* [lindex $args 0]]} {
		set item [lshift args]
		lappend cl $item
	}
	eval lappend cl [lshift args]
	eval lappend cl [eval glob -nocomplain $args]
	eval exec chmod $cl
}


if {"$tcl_platform(platform)"=="windows"} {
#
# Same for Windows ?
# This is just a very quick hack to get the most important things working.
# Don't expect to much of it ;-)
# ------------------------------------------------------------------------

proc ls {args} {
	set recurse 1
	while {[string match \-* [lindex $args 0]]} {
		set opt [lshift args]
		switch -glob -- $opt {
			-d* {set recurse 0}
			-- break
			default {
				error "Unknown argument $opt, should be one of: -d"
			}
		}
	}
	if {"$args"==""} {set args *}
	set files [lsort [eval glob -nocomplain $args]]
	set result ""
	foreach file $files {
		if [file isdir $file] {
			lappend result $file
			if $recurse {
				lappend result "\t[lsort [eval glob -nocomplain [file join $file *]]]"
			}
		} else {
			lappend result $file
		}
	}
	return [join $result "\n"]
}

proc chmod {args} {
	set files ""
	set item [lshift args]
	set num [string index $item 1]
	if {$num==6} {set mod read} else {set mod write}
	foreach file [eval glob -nocomplain $args] {
		win_chmod $mod $file
	}
}

}


