# File containing the Tcl part of the list commands in the Extral extension
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================
#doc cmd title {
#Command manipulation commands
#}

#doc {cmd cmd_get} cmd {
#	cmd_get channelId
#} descr {
#	get one complete Tcl command, in the sense of having no unclosed quotes, 
#	braces, brackets or array element names, from the open file given by channelId
#}
proc cmd_get {channelId} {
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

#doc {cmd cmd_split} cmd {
#	cmd_split data
#} descr {
#	split data into complete Tcl commands in the sense of having no unclosed quotes, 
#	braces, brackets or array element names.
#}
proc cmd_split {data} {
	set result ""
	set current ""
	foreach line [split $data "\n"] {
		if {"$current" != ""} {
			append current "\n"
		}
		append current $line
		set end [string length $line]
		incr end -1
		if ![regexp {\\$} $current] {
			if [info complete $current] {
				lappend result $current
				set current ""
			}
		}
	}
	return $result
}

#proc splitesccomplete {data} {
#	set result ""
#	set current ""
#	foreach line [split $data "\n"] {
#		if {"$current" != ""} {
#			append current "\n"
#		}
#		append current $line
#		if ![regexp {\\$} $current] {
#			if [info complete $current] {
#				lappend result $current
#				set current ""
#			}
#		}
#	}
#	return $result
#}

#doc {cmd cmd_parse} cmd {
#	cmd_parse line
#} descr {
#	parses a cmdline. it returns a list where each element is the part of the cmdline that
#	will result in one element when the cmdline would be evaluated.
#	eg.
#	% cmd_parse {set test [format "%2.2f" 4.3]}
#	set test {[format "%2.2f" 4.3]}
#}
proc cmd_parse {line {recurse 0}} {
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
					switch -- [string index $line $i] {
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
							set i [cmd_parse $line $i]
						}
					}
				}
			}
			"\[" {
				incr i
				set i [cmd_parse $line $i]
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
#cmd_parse $line

#doc {cmd cmd_load} cmd {
#	cmd_load filename
#} descr {
#	returns the contents of the given file als a list of complete Tcl commands.
#	
#}
proc cmd_load {filename} {
	set f [open $filename "r"]
	while {[eof $f] != 1} {
		lappend result [cmd_get $f]
	}
	close $f
	return $result
}

