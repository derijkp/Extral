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

proc cp {args} {
	set cl ""
	while {[string match \-* [lindex $args 0]]} {
		set item [lshift args]
		lappend cl $item
	}
	set target [lpop args]
	eval lappend cl [eval glob -nocomplain $args]
	eval exec cp $cl $target
}

proc mv {args} {
	set cl ""
	while {[string match \-* [lindex $args 0]]} {
		set item [lshift args]
		lappend cl $item
	}
	set target [lpop args]
	eval lappend cl [eval glob -nocomplain $args]
	eval exec mv $cl $target
}

proc rm {args} {
	set cl ""
	while {[string match \-* [lindex $args 0]]} {
		set item [lshift args]
		lappend cl $item
	}
	eval lappend cl [eval glob -nocomplain $args]
	eval exec rm $cl
}

if {"$tcl_platform(platform)"=="windows"} {
#
# Same for Windows ?
# This is just a very quick hack to get the most important things working.
# Don't expect to much of it ;-)
# ------------------------------------------------------------------------
proc mkdir {dir} {
	global env
	set keep [pwd]
	set todo ""
	while {![file exists $dir]} {
		lunshift todo [file tail $dir]
		set dir [file dir $dir]
	}
	foreach tail $todo {
		cd $dir
		win_mkdir $tail
		set dir [file join $dir $tail]
	}
	cd $keep
}

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

proc cp {args} {
	set recurse 0
	while {[string match \-* [lindex $args 0]]} {
		set opt [lshift args]
		switch -glob -- $opt {
			-r {set recurse 1}
			-- break
			default {
				error "Unknown argument $opt, should be one of: -r"
			}
		}
	}
	if {[llength $args]<2} {error "Need at least 2 arguments"}
	set to [lpop args]
	set files [eval glob -nocomplain $args]
	if {"$files"==""} {return "Nothing copied"}

	set len [llength $files]
	set isdir [file isdir $to]
	if [file exists $to] {
		if ![file isdir $to] {
			if {$len!=1} {error "Cannot copy multiple files a file"}
			set file [lindex $files 0]
			if $isdir {error "Cannot copy directory to file"}
		}
	}
	if ![file exists [file dir $to]] {
		error "Directory [file dir $to] does not exist"
	}

	foreach file $files {
		if {($len==1)&&($isdir==0)} {
			set target $to
		} else {
			set target [file join $to [file tail $file]]
		}
		if [file isdir $file] {
			if $recurse {
				win_mkdir $target
				cp -r [file join $file *] $target
			}
		} else {
			win_cp $file $target
		}
	}
}

proc mv {args} {
	while {[string match \-* [lindex $args 0]]} {
		set opt [lshift args]
		switch -glob -- $opt {
			-- break
			default {
				error "Unknown argument $opt, should be one of: -r"
			}
		}
	}
	if {[llength $args]<2} {error "Need at least 2 arguments"}
	set to [lpop args]
	set files [eval glob -nocomplain $args]
}

proc rm {args} {
	set recurse 0
	while {[string match \-* [lindex $args 0]]} {
		set opt [lshift args]
		switch -glob -- $opt {
			-r {set recurse 1}
			-- break
			default {
				error "Unknown argument $opt, should be one of: -r"
			}
		}
	}
	set files [eval glob -nocomplain $args]
	foreach file $files {
		if ![file isdir $file] {
			win_remove $file
		} elseif $recurse {
			rm -r [file join $file *]
			win_rmdir $file
		}
	}
}

}
