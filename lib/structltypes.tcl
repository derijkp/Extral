proc ::Extral::setany {structure oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::getany {structure field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setint {structure oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if ![regexp {^-?[0-9]+$} $value] {
		return -code error "expected integer but got \"$value\""
	}
	return $value
}

proc ::Extral::getint {structure field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setdouble {structure oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if [catch {expr $value}] {
		return -code error "expected floating-point number but got \"$value\""
	}
	return $value
}

proc ::Extral::getdouble {structure field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setbool {structure oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	set value [string tolower $value]
	if [regexp {^1$|^true$|^t$|^yes$|^y$} $value] {
		return 1
	} elseif [regexp {^0$|^false$|^f$|^no$|^n$} $value] {
		return 0
	} else {
		return -code error "expected boolean value but got \"$value\""
	}
}

proc ::Extral::getbool {structure field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setregexp {structure oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if {[llength $structure] != 4} {
		return -code error "error: wrong number of arguments in structure \"$structure\""
	}
	set pattern [lindex $structure 1]
	if ![regexp $pattern $value] {
		return -code error "error: \"$value\" [lindex $structure 2]"
	}
	return $value
}

proc ::Extral::getregexp {structure field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setbetween {structure oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
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

proc ::Extral::getbetween {structure field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setdbetween {structure oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
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

proc ::Extral::getdbetween {structure field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setdate {structure oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return [scantime $value]
}

proc ::Extral::getdate {structure field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return [formattime $value "%e %b %Y"]
}

proc ::Extral::settime {structure oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return [scantime $value]
}

proc ::Extral::gettime {structure field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return [formattime $value "%e %b %Y %H:%M:%S"]
}

#proc ::Extral::setnamed {structure oldvalue field value} {
#	if {"$field" != ""} {return -code error "error: no name given to named type"}
#	set tag [lpop field]
#	set pos [structlfind $value $tag]
#	if {$pos == -1} {
#		lappend oldvalue structlsetstruct $structure 
#	} else {
#		structlsetstruct $structure 
#	}
#}
#
#proc ::Extral::getnamed {structure field value} {
#	if {"$field" == ""} {
#		return $value
#	} else {
#		set tag [lpop field]
#		set pos [structlfind $value $tag]
#		return [structlgetstruct [lindex $structure 0] [lindex $value $pos] [llength $field] $field]
#	}
#}

proc ::Extral::setlist {structure oldvalue field value} {
#putsvars structure oldvalue field value
	set tag [lshift field]
	set struct [lindex $structure 1]
	if {"$tag" == ""} {
		set result ""
		foreach val $value oldval $oldvalue {
			set code [catch {::Extral::structlsetstruct $struct $oldval [llength $field] $field $val} res]
			if {$code == 1} {
				error $res
			} else {
				lappend result $res
			}
		}
		return $result
	} elseif {"$tag" == "next"} {
		set code [catch {::Extral::structlsetstruct $struct "" [llength $field] $field $value} res]
		if {$code == 1} {
			error $res
		} else {
			lappend oldvalue $res
			return $oldvalue
		}
	} elseif {[llength $tag] == 1} {
		set len [llength $oldvalue]
		if {$len == 0} {
			return -code error "empty list"
		}
		incr len -1
		if {("$tag"!="end")&&($tag>$len)} {
			return -code error "list doesn't contain element $tag"
		}
		set code [catch {::Extral::structlsetstruct $struct [lindex $oldvalue $tag] [llength $field] $field $value} res]
		if {$code == 1} {
			error $res
		} else {
			return [lreplace $oldvalue $tag $tag $res]
		}
	} else {
		return -code error "wrong # args to list: \"$tag\""
	}
}

proc ::Extral::getlist {structure field value} {
#putsvars structure field value
	set tag [lshift field]
	set struct [lindex $structure 1]
	if {"$tag" == ""} {
		set result ""
		foreach val $value {
			set code [catch {::Extral::structlgetstruct $struct $val [llength $field] $field} res]
			if {$code == 1} {
				error $res
			} else {
				lappend result $res
			}
		}
		return $result
	} elseif {[llength $tag] == 1} {
		set len [llength $value]
		if {$len == 0} {
			return -code error "empty list"
		}
		incr len -1
		if {("$tag"!="end")&&($tag>$len)} {
			return -code error "list doesn't contain element $tag"
		}
		set code [catch {::Extral::structlgetstruct $struct [lindex $value $tag] [llength $field] $field} res]
		if {$code == 1} {
			error $res
		} else {
			return $res
		}
	} elseif {[llength $tag] == 2} {
		set len [llength $value]
		if {$len == 0} {
			return -code error "empty list"
		}
		incr len -1
		set start [lindex $tag 0]
		set end [lindex $tag 1]
		if {("$start"!="end")&&($start>$len)} {
			return -code error "list doesn't contain element $start"
		}
		if {("$end"!="end")&&($end>$len)} {
			return -code error "list doesn't contain element $end"
		}
		set result ""
		foreach val [lrange $value $start $end] {
			set code [catch {::Extral::structlgetstruct $struct $val [llength $field] $field} res]
			if {$code == 1} {
				error $res
			} else {
				lappend result $res
			}
		}
		return $result
	} else {
		return -code error "wrong # args to list: \"$tag\""
	}
}
