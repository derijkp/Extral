# File containing the Tcl part of the list commands in the Extral extension
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

Extral::export {structlset structlget structlunset} {

proc structlget {list tag args} {
	foreach {ctag value} $list {
		if {"$tag"=="$ctag"} {
			return $value
		}
	}
	if {"$args"==""} {
		error "tag \"$tag\" not found"
	} else {
		return [lindex $args 0]
	}
}

proc structlset {list tag value} {
	set pos 1
	foreach {ctag cvalue} $list {
		if {"$tag"=="$ctag"} {
			return [lreplace $list $pos $pos $value]
		}
		incr pos 2
	}
	return [concat $list $tag $value]
}

proc structlunset {list tag} {
	set pos 0
	foreach {ctag cvalue} $list {
		if {"$tag"=="$ctag"} {
			return [lreplace $list $pos [expr $pos+1]]
		}
		incr pos 2
	}
	return $list
}

}
