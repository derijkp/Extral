proc ::Extral::setany {structure oldvalue value} {
	return $value
}

proc ::Extral::getany {structure value} {
	return $value
}

proc ::Extral::setint {structure oldvalue value} {
	if ![regexp {^-?[0-9]+$} $value] {
		return -code error "expected integer but got \"$value\""
	}
	return $value
}

proc ::Extral::getint {structure value} {
	return $value
}

proc ::Extral::setdouble {structure oldvalue value} {
	if [catch {expr $value}] {
		return -code error "expected floating-point number but got \"$value\""
	}
	return $value
}

proc ::Extral::getdouble {structure value} {
	return $value
}

proc ::Extral::setbool {structure oldvalue value} {
	set value [string tolower $value]
	if [regexp {^1$|^true$|^t$|^yes$|^y$} $value] {
		return 1
	} elseif [regexp {^0$|^false$|^f$|^no$|^n$} $value] {
		return 0
	} else {
		return -code error "expected boolean value but got \"$value\""
	}
}

proc ::Extral::getbool {structure value} {
	return $value
}

proc ::Extral::setregexp {structure oldvalue value} {
	if {[llength $structure] != 3} {
		return -code error "error: wrong number of arguments in structure \"$structure\""
	}
	set pattern [lindex $structure 1]
	if ![regexp $pattern $value] {
		return -code error "error: \"$value\" does not match pattern \"$pattern\""
	}
	return $value
}

proc ::Extral::getregexp {structure value} {
	return $value
}

proc ::Extral::setbetween {structure oldvalue value} {
	if {[llength $structure] != 4} {
		return -code error "error: wrong number of arguments in structure \"$structure\""
	}
	set min [lindex $structure 1]
	set max [lindex $structure 2]
	if ![regexp {^-?[0-9]+$} $value] {
		return -code error "expected integer but got \"$value\""
	}
	if {($value<$min)||($value>$max)} {
		return -code error "error: $value is not between $min and $max"
	}
	return $value
}

proc ::Extral::getbetween {structure value} {
	return $value
}

proc ::Extral::setdbetween {structure oldvalue value} {
	if {[llength $structure] != 4} {
		return -code error "error: wrong number of arguments in structure \"$structure\""
	}
	set min [lindex $structure 1]
	set max [lindex $structure 2]
	if {($value<$min)||($value>$max)} {
		return -code error "error: $value is not between $min and $max"
	}
	return $value
}

proc ::Extral::getdbetween {structure value} {
	return $value
}

proc ::Extral::setdate {structure oldvalue value} {
	return [scantime $value]
}

proc ::Extral::getdate {structure value} {
	return [formattime $value "%e %b %Y"]
}

proc ::Extral::settime {structure oldvalue value} {
	return [scantime $value]
}

proc ::Extral::gettime {structure value} {
	return [formattime $value "%e %b %Y %H:%M:%S"]
}

