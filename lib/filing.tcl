# Some handy filing functions I often use, so they ended up here
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

Extral::export {dirglob lload lwrite readfile writefile} {

proc dirglob {dir pattern} {
	set pwd [pwd]
	cd $dir
	set result [glob -nocomplain -- $pattern]
	cd $pwd
	return $result
}

proc lload {filename} {
	set f [open $filename "r"]
	set result [split [read $f] "\n"]
	close $f
	return $result
}

proc lwrite {filename list} {
	set f [open $filename "a"]
	puts $f [join $list "\n"] nonewline
	close $f
}

proc readfile {filename} {
	set f [open $filename "r"]
	fconfigure $f -buffersize 100000
	set result [read $f]
	close $f
	return $result
}

proc writefile {filename list} {
	set f [open $filename "w"]
	fconfigure $f -buffersize 100000
	puts -nonewline $f $list
	close $f
}

}

# Using the Unix tools
# --------------------

Extral::export {chmod} {
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
}


if {"$tcl_platform(platform)"=="windows"} {
#
# ls and chmod for Windows ?
# This is just a very quick hack to get the most important things working.
# Don't expect to much of it ;-)
# ------------------------------------------------------------------------

Extral::export {ls mkdir cp mv rm chmod} {
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

}
