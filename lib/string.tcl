# File containing the Tcl part of the Extral extension
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

#doc stringcommands title {
#Extra string manipulation commands
#}

#doc {stringcommands string_split} cmd {
#string__split string splitstring
#} descr {
# split string on exact occurence off splitstring<br>
#} example {
#	% string_split "test1||test2" "||"
#	test1 test2
#}
proc string_split {string splitstring} {
	set result ""
	set len [string length $splitstring]
	while 1 {
		set pos [string first $splitstring $string]
		if {$pos == -1} {
			lappend result $string
			break
		}
		lappend result [string range $string 0 [expr {$pos-1}]]
		set string [string range $string [expr {$pos+$len}] end]
	}
	return $result
}

#doc {stringcommands string_equal} cmd {
#string_equal s1 s2
#} descr {
# returns 1 if the strings are equal, 0 if the are not
#}
proc string_equal {s1 s2} {
	if {[string length $s1] != [string length $s2]} {
		return 0
	}
	if {"$s1" == "$s2"} {
		return 1
	} else {
		return 0
	}
}

#doc {stringcommands string_fill} cmd {
#string_fill string number
#} descr {
# returns a string consisting of the argument string number times repeated
#}
proc string_fill {string number} {
	set result ""
	if {![isint $number]} {
		error "$number is not an integer"
	}
	if {$number < 0} {
		return {}
	}
	for {set i 0} {$i < $number} {incr i} {
		append result $string
	}
	return $result
}

