# Some convenience functions I often use, so they ended up here
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

Extral::export {rem REM remoff true setglobal random} {

# Remark
# rem: 
#	does nothing
#	I use this to put some example or testing code in a program
#	without all the #'s
proc rem {args} {
}

# REM:
#	when the procedure remof is called, REM will also do nothing
#	when the procedure remon is called, REM will put its arguments
#	to the stdout
proc REM {args} {}
proc remon {} {
	proc REM {args} {	  
		puts stdout $args   
	}
}
proc remoff {} {
	proc REM {args} {}
}

# true expr
#	returns 1 when expression is yes, true or 1
proc true {expr} {
	set result 0
	switch $expr {
		yes {set result 1}
		true {set result 1}
		1 {set result 1}
	}
	return $result
}

# setglobal varName ?newValue?
#	same as the set command, but then for global variables
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

proc random {min max} {
	set r [expr $max-$min+1]
	return [expr int($min+rand()*$r)]
}

}
