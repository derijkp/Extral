# Some convenience functions I often use, so they ended up here
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================
#doc convenience title {Convenience functions}

#doc {convenience invoke} cmd {
#invoke vars cmd ...
#} descr {
# invoke simply evals $cmd in a private space. This eg. allows using
# temporary variables in bindings without creating these in global scope.
# It is also very convenient to use values appended to a command given
# to a binding:
# Further arguments (when given) are parameters that will be available in the
# variables given in vars. If more parameters are supplied than vars are given,
# the remaining parameters will be stored in the variable args.
#}
proc invoke {vars cmd args} {
	foreach var $vars {
		set $var [lshift args]
	}
	eval $cmd
}

#doc {convenience aproc} cmd {
#aproc args body
#} descr {
# aproc creates an 'anonymous' procedure; this means you don't have to provide a name. It
# returns the name to invoke it. These procedures are cached based on the arguments and body.
# This is actually somewhat similar to the invoke command, but is faster when the proc
# is being reused many times.
# Typical use would be in parameters that expect a command name that will be called later with 
# a number of arguments:
# .ctable configure -getcommand [aproc {args} {return $args}]
#}
set Extral::aproc() 0
proc aproc {args body} {
	upvar #0 Extral::aproc aproc
	set key [list $args $body]
	if ![info exists aproc($key)] {
		incr aproc()
		proc ::Extral::aproc$aproc() $args $body
		set aproc($key) ::Extral::aproc$aproc()
	}
	return $aproc($key)
}


#doc {convenience arraytrans} cmd {
#arraytrans varName list ?default?
#} descr {
# returns a list of all the values in the array corresponding to the
# indices in the given list. If the index is not present in the array,
# the index itself will be returned.
# If $default is given, it will be returned for indices not in the array.
#}
proc arraytrans {varName list args} {
	upvar $varName var
	set result ""
	if {"$args" == ""} {
		foreach item $list {
			if [info exists var($item)] {
				lappend result $var($item)
			} else {
				lappend result $item
			}
		}
	} else {
		set def [lindex $args 0]
		foreach item $list {
			if [info exists var($item)] {
				lappend result $var($item)
			} else {
				lappend result $def
			}
		}
	}
	return $result
}

#doc {convenience ?} cmd {
#? expr truevalue falsevalue
#} descr {
# ? expr truevalue falsevalue
#}
proc ? {expr truevalue falsevalue} {
	uplevel if [list $expr] {{set ::Extral::temp 1} else {set ::Extral::temp 0}}
	if $::Extral::temp {return $truevalue} else {return $falsevalue}
}

#doc {convenience ?} cmd {
#echo string
#} descr {
# echo returns its argument as a result
# This is useful when you want a command that will be evalled or upleveled
# to return a certain value
# 
#}
proc echo {string} {
	return $string
}

#doc {convenience ?} cmd {
#get varName ?default?
#} descr {
# get returns the value of the variable given by varName if it exists.
# If the variable does not exists, it returns an empty string, or
# value given by $default if present
# 
#}
proc get {varName {default {}}} {
	upvar $varName var
	if [info exists var] {
		return $var
	} else {
		return $default
	}
}

#doc {convenience rem} cmd {
#rem args
#} descr {
#	does nothing<br>
#	I use this to put some example or testing code in a program
#	without all the #'s
#}
proc rem {args} {
}

# REM:
#doc {convenience REM} cmd {
#REM args
#} descr {
#	when the procedure remof is called, REM will also do nothing
#	when the procedure remon is called, REM will put its arguments
#	to the stdout
#}
proc REM {args} {}
proc remon {} {
	proc REM {args} {	  
		puts stdout $args   
	}
}
proc remoff {} {
	proc REM {args} {}
}

#doc {convenience true} cmd {
#true expression
#} descr {
# true expr
#	returns 1 when expression is yes, true or 1<br>
#	otherwise it returns 0.
#}
proc true {expr} {
	set result 0
	return [regexp -nocase {^(1|yes|true|on)$} $expr]
}

#doc {convenience setglobal} cmd {
# setglobal varName ?newValue?
#} descr {
#	same as the set command, but then for global variables
#}
proc setglobal {varName args} {
	upvar #0 $varName var
	if {"$args" == ""} {
		if ![info exists var] {
			error "can't read \"$varName\": no such global variable"
		} else {
			return $var
		}
	} else {
		set var [lindex $args 0]
	}
}

#doc {convenience random} cmd {
#random min max
#} descr {
#returns a random number between min and max
#}
proc random {min max} {
	set r [expr $max-$min+1]
	return [expr int($min+rand()*$r)]
}

#doc {convenience random} cmd {
#inlist list value
#} descr {
#returns 1 if $value is an element of list $list
#returns 0 if $value is not an element of list $list
#}
proc inlist {list value} {
        if {[lsearch $list $value]==-1} {
                return 0
        } else {
                return 1
        }
}

#doc {convenience putsvars} cmd {
#putsvars varname ?varname ...?
#} descr {
#returns the values of the given variables in the form:<br>
#set variable1 value1<br>
#set variable2 value2
#}
proc putsvars {args} {
	foreach var $args {
		set value [uplevel set $var]
		puts [list set $var $value]
	}
}

proc extractoption {listName option default} {
	upvar $listName l
	set pos [lsearch $l $option]
	if {$pos != -1} {
		lpop l $pos
		return [lpop l $pos]
	} else {
		return $default
	}
}

proc extractbool {listName option} {
	upvar $listName l
	set pos [lsearch $l $option]
	if {$pos != -1} {
		lpop l $pos
		return 1
	} else {
		return 0
	}
}

