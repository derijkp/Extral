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

# Using the Unix tools
# --------------------
proc mkdir {dir} {
	set todo ""
	while {![file exists $dir]} {
		lunshift todo [file tail $dir]
		set dir [file dir $dir]
	}
	foreach tail $todo {
		set dir [file join $dir $tail]
		exec mkdir $dir
	}
}

proc ls {args} {
	global env tcl_interactive
	if {$tcl_interactive&&[info exists env(LS_OPTIONS)]} {
		set cl $env(LS_OPTIONS)
	} else {
		set cl ""
	}
	while 1 {
		set item [lshift args]
		if {"[string index $item 0]"!="-"} break
		lappend cl $item
	}

	while 1 {
		if [regexp {\*} $item] {
			eval lappend cl [glob $item]
		} else {
			eval lappend cl $item
		}
		set item [lshift args]
		if {"$item"==""} break
	}
	if {$tcl_interactive&&([info level]==1)} {
		eval exec ls $cl >&@stdout
	} else {
		eval exec ls $cl
	}
}

proc chmod {args} {
	set cl ""
	while 1 {
		set item [lshift args]
		if {"[string index $item 0]"!="-"} break
		lappend cl $item
	}

	while 1 {
		if [regexp {\*} $item] {
			eval lappend cl [glob $item]
		} else {
			eval lappend cl $item
		}
		set item [lshift args]
		if {"$item"==""} break
	}
	eval exec chmod $cl
}

proc cp {args} {
	set cl ""
	while 1 {
		set item [lshift args]
		if {"[string index $item 0]"!="-"} break
		lappend cl $item
	}

	while 1 {
		if [regexp {\*} $item] {
			eval lappend cl [glob $item]
		} else {
			eval lappend cl $item
		}
		set item [lshift args]
		if {"$item"==""} break
	}
	eval exec cp $cl
}

proc mv {args} {
	set cl ""
	while 1 {
		set item [lshift args]
		if {"[string index $item 0]"!="-"} break
		lappend cl $item
	}

	while 1 {
		if [regexp {\*} $item] {
			eval lappend cl [glob $item]
		} else {
			eval lappend cl $item
		}
		set item [lshift args]
		if {"$item"==""} break
	}
	eval exec mv $cl
}

proc rm {args} {
	set cl ""
	while 1 {
		set item [lshift args]
		if {"[string index $item 0]"!="-"} break
		lappend cl $item
	}

	while 1 {
		if [regexp {\*} $item] {
			eval lappend cl [glob $item]
		} else {
			eval lappend cl $item
		}
		set item [lshift args]
		if {"$item"==""} break
	}
	eval exec rm $cl
}

if {"$tcl_platform(platform)"=="windows"} {
#
# Same for Windows ?
# ------------------
}
