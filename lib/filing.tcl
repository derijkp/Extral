# Some handy filing functions I often use, so they ended up here
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================
#doc filing title {
#Filing commands
#}

#doc {filing dirglob} cmd {
#	dirglob dir pattern
#} descr {
#	returns a list of all filenames in directory dir mathing the given pattern
#}
proc dirglob {dir pattern} {
	set pwd [pwd]
	if [catch {cd $dir}] {return ""}
	set result [glob -nocomplain -- $pattern]
	cd $pwd
	return $result
}

#doc {filing lload} cmd {
#lload filename
#} descr {
#	returns all lines in the specified files as a list 
#}
proc lload {filename} {
	set f [open $filename "r"]
	set result [split [read $f] "\n"]
	close $f
	return $result
}

#doc {filing lwrite} cmd {
#lwrite file list
#} descr {
#	writes a list to a file
#}
proc lwrite {filename list} {
	set f [open $filename "a"]
	puts $f [join $list "\n"] nonewline
	close $f
}

#doc {filing readfile} cmd {
#	readfile filename
#} descr {
#	returns the contents of the file given by filename
#}
proc readfile {filename} {
	set f [open $filename "r"]
	fconfigure $f -buffersize 100000
	set result [read $f]
	close $f
	return $result
}

#doc {filing writefile} cmd {
#	writefile filename data
#} descr {
#	create a file by the name filename with data as its content
#}
proc writefile {filename list} {
	set f [open $filename "w"]
	fconfigure $f -buffersize 100000
	puts -nonewline $f $list
	close $f
}

#doc {filing getcomplete} cmd {
#	getcomplete channelId
#} descr {
#	get one complete Tcl command, in the sense of having no unclosed quotes, 
#	braces, brackets or array element names, from the open file given by channelId
#}
proc getcomplete {channelId} {
	if [eof $channelId] {return ""}
	while 1 {
		append line "[gets $channelId]"
		if [info complete $line] break
		if [eof $channelId] {
			error "file end: \"$line\" is incomplete"
		}
		append line "\n"
	}
	return $line
}

#doc {filing splitcomplete} cmd {
#	splitcomplete data
#} descr {
#	split data into complete Tcl commands in the sense of having no unclosed quotes, 
#	braces, brackets or array element names.
#}
proc splitcomplete {data} {
	set result ""
	set current ""
	regsub -all "\\\\\n" $data {} data
	foreach line [split $data "\n"] {
		if {"$current" != ""} {
			append current "\n"
		}
		append current $line
		if [info complete $current] {
			lappend result $current
			set current ""
		}
	}
	return $result
}

proc splitesccomplete {data} {
	set result ""
	set current ""
	regsub -all "\\\\\n" $data {} data
	foreach line [split $data "\n"] {
		if {"$current" != ""} {
			append current "\n"
		}
		append current $line
		if ![regexp {\\$} $current] {
			if [info complete $current] {
				lappend result $current
				set current ""
			}
		}
	}
	return $result
}

#doc {filing splitcomplete} cmd {
#	parsecommand line
#} descr {
#	parses a cmdline. it returns a list where each element is the part of the cmdline that
#	will result in one element when the cmdline would be evaluated.
#	eg.
#	% parsecommand {set test [format "%2.2f" 4.3]}
#	set test {[format "%2.2f" 4.3]}
#}
proc parsecommand {line {recurse 0}} {
	regsub -all "\\\\\n" $line {} line
	set len [string length $line]
	set i $recurse
	while {$i < $len} {
		if ![regexp "\[ \t\]" [string index $line $i]] break
		incr i
	}
	set prev $i
	set result ""
	while {$i < $len} {
		switch -- [string index $line $i] {
			" " - "\n" {
				lappend result [string range $line $prev [expr {$i-1}]]
				while {$i < $len} {
					incr i
					set char [string index $line $i]
					if {("$char" != " ")&&("$char" != "\t")} {
						set prev $i
						incr i -1
						break
					}
				}
			}
			"\\" {
				incr i
			}
			"\{" {
				set level 1
				while 1 {
					incr i
					if {$i == $len} break
					switch [string index $line $i] {
						"\{" {
							incr level
						}
						"\}" {
							incr level -1
							if {$level == 0} break
						}
					}
				}
			}
			"\"" {
				while 1 {
					incr i
					if {$i == $len} {
						error "missing \""
					}
					switch -- [string index $line $i] {
						"\\" {incr i}
						"\"" break
						"\[" {
							incr i
							set i [parsecommand $line $i]
						}
					}
				}
			}
			"\[" {
				incr i
				set i [parsecommand $line $i]
			}
			"\]" {
				if $recurse {
					return $i
				}
			}
		}
		incr i
	}
	lappend result [string range $line $prev end]
	return $result
}
#set line "\t \$window.try configure \t -title \[puts \"\[tk appname\]\"\] \\\n-command \"\$window command\\\"\\n\\\"\" -test \{try \nit \"\"\}"
#parsecommand $line

#doc {filing cload} cmd {
#	cload filename
#} descr {
#	returns the contents of the given file als a list of complete Tcl commands.
#	
#}
proc cload {filename} {
	set f [open $filename "r"]
	while {[eof $f] != 1} {
		lappend result [getcomplete $f]
	}
	close $f
	return $result
}

# Using the Unix tools
# --------------------

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
# ls and chmod for Windows ?
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
