#doc {stringcommands string_change} cmd {
#string_change string changelist
#} descr {
# change some parts of a string<br>
# changelist gives alternating a substring to be changed, and what it should be changed into.
# The command returns a string that is the given string where each occurence of the
# substrings in the changelist have been changed.
#}
proc string_change {string changelist} {
	array set a $changelist
	set rem $string
	set string ""
	while 1 {
		set names [array names a]
		if {"$names" == ""} break
		set fpos [string length $rem]
		foreach name $names {
			set pos [string first $name $rem]
			if {$pos == -1} {
				unset a($name)
			} elseif {$pos < $fpos} {
				set found $name
				set fpos $pos
			}
		}
		if ![info exists found] {
			append string $rem
			break
		}
		append string [string range $rem 0 [expr {$fpos-1}]]
		append string $a($found)
		set rem [string range $rem [expr {$fpos+[string length $found]}] end]
		if {"$rem" == ""} break
		unset found
	}
	return $string
}

#doc {stringcommands string_reverse} cmd {
#string_reverse list
#} descr {
# returns the reverse of list.
#}
proc string_reverse {string} {
	set i [string length $string]
	set result ""
	for {incr i -1} {$i >= 0} {incr i -1} {
		append result [string index $string $i]
	}
	return $result
}

#doc {stringcommands string_find} cmd {
#string_find mode list pattern
#} descr {
#	returns a list of all indices which match a pattern.
#	mode can be -exact, -glob, or -regexp
#	The default mode is -exact
#} example {
#	% string_find -regexp {Ape Ball Field {Antwerp city} Egg} {^A}
#	0 16
#}
proc string_find {args} {
	if {[llength $args]==2} {
		set string [lindex $args 0]
		set pattern [lindex $args 1]
		set mode -exact
	} elseif {[llength $args]==3} {
		set mode [lindex $args 0]
		set string [lindex $args 1]
		set pattern [lindex $args 2]
	} else {
		error "Format is \"list_find ?mode? list pattern\""
	}
	set result ""
	set pos 0
	set len [string length $string]
	switch -- $mode {
		{-exact} {
			set end [expr {[string length $pattern] - 1}]
			for {set i 0} {$i < $len} {incr i} {
				if {"[string range $string $i $end]"=="$pattern"} {lappend result $i}
				incr end
			}
		}
		{-glob} {
			for {set i 0} {$i < $len} {incr i} {
				if [string match $pattern [string range $string $i end]] {lappend result $i}
			}
		}
		{-regexp} {
			for {set i 0} {$i < $len} {incr i} {
				if [regexp $pattern [string range $string $i end]] {lappend result $i}
			}
		}
		default {
			error "Unkown mode \"$mode\""
		}
	}
	return $result
}

#doc {stringcommands string_replace} cmd {
#string_replace string first last replacement
#} descr {
# replace a part of a string
#}
proc string_replace {string first last replacement} {
	if {$last < $first} {
		set last $first
	} else {
		incr last 1
	}
	incr first -1
	set diff [expr {$first - [string length $string]}]
	if {$diff > 0} {
		for {set i 0} {$i < $diff} {incr i} {append string " "}
	}
	return [string range $string 0 $first]$replacement[string range $string $last end]
}
