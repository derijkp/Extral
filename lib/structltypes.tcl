proc ::Extral::setany {structure data oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if {"$value" == "[lindex $structure end]"} {
		return -code 5 $value
	}
	return $value
}

proc ::Extral::unsetany {structure data oldvalue field} {
	return -code 5 ""
}

proc ::Extral::getany {structure data field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setint {structure data oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if {"$value" == "[lindex $structure end]"} {
		return -code 5 $value
	}
	if ![regexp {^-?[0-9]+$} $value] {
		return -code error "expected integer but got \"$value\""
	}
	return $value
}

proc ::Extral::getint {structure data field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setdouble {structure data oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if {"$value" == "[lindex $structure end]"} {
		return -code 5 $value
	}
	if [catch {expr $value}] {
		return -code error "expected floating-point number but got \"$value\""
	}
	return $value
}

proc ::Extral::getdouble {structure data field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setbool {structure data oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if {"$value" == "[lindex $structure end]"} {
		return -code 5 $value
	}
	set value [string tolower $value]
	if [regexp {^1$|^true$|^t$|^yes$|^y$} $value] {
		return 1
	} elseif [regexp {^0$|^false$|^f$|^no$|^n$} $value] {
		return 0
	} else {
		return -code error "expected boolean value but got \"$value\""
	}
}

proc ::Extral::getbool {structure data field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setregexp {structure data oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if {"$value" == "[lindex $structure end]"} {
		return -code 5 $value
	}
	if {[llength $structure] != 4} {
		return -code error "error: wrong number of arguments in structure \"$structure\""
	}
	set pattern [lindex $structure 1]
	if ![regexp $pattern $value] {
		return -code error "error: \"$value\" [lindex $structure 2]"
	}
	return $value
}

proc ::Extral::getregexp {structure data field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setbetween {structure data oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if {"$value" == "[lindex $structure end]"} {
		return -code 5 $value
	}
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

proc ::Extral::getbetween {structure data field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setdbetween {structure data oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if {"$value" == "[lindex $structure end]"} {
		return -code 5 $value
	}
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

proc ::Extral::getdbetween {structure data field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	return $value
}

proc ::Extral::setdate {structure data oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if {"$value" == "[lindex $structure end]"} {
		return -code 5 $value
	}
	return [scantime $value]
}

proc ::Extral::getdate {structure data field value} {
	if {"$value" == ""} {return [lindex $structure end]}
	if {"$field" != ""} {
		if {"$field" == "val"} {
			return $value
		} else {
			return [formattime $value [lindex $field 0]]
		}
	} else {
		return [formattime $value "%e %b %Y"]
	}
}

proc ::Extral::settime {structure data oldvalue field value} {
	if {"$field" != ""} {return -code error "error: field \"$field\" not present in structure \"$structure\""}
	if {"$value" == "[lindex $structure end]"} {
		return -code 5 $value
	}
	return [scantime $value]
}

proc ::Extral::gettime {structure data field value} {
	if {"$value" == ""} {return [lindex $structure end]}
	if {"$field" != ""} {
		if {"$field" == "val"} {
			return $value
		} else {
			return [formattime $value [lindex $field 0]]
		}
	} else {
		return [formattime $value "%e %b %Y %H:%M:%S"]
	}
}

proc ::Extral::setnamed {structure data oldvalue field value} {
#putsvars structure data oldvalue field value
	if {"$field" == ""} {
		set len [llength $value]
		if {[expr $len%2] != 0} {
			return -code error "error: \"$value\" does not have an even number of elements"
		}
		foreach {name val} $value {
			set code [catch {::Extral::setnamed $structure $data $oldvalue $name $val} oldvalue]
			if {$code == 1} {
				error $oldvalue
			}
		}
		set result $oldvalue
	} else {
		set struct [lindex $structure 1]
		set tag [lshift field]
		set pos [structlfind $oldvalue $tag]
		if {$pos == -1} {
			set code [catch {::Extral::structlsetstruct $struct $data "" [llength $field] $field $value} res]
			if {$code == 1} {
				error "$res in named \"$tag\""
			} elseif {$code == 5} {
				set result $oldvalue
			} else {
				lappend oldvalue $tag
				lappend oldvalue $res
				set result $oldvalue
			}
		} else {
			set code [catch {::Extral::structlsetstruct $struct $data [lindex $oldvalue $pos] [llength $field] $field $value} res]
			if {$code == 1} {
				error "$res in named \"$tag\""
			} elseif {$code == 5} {
				set result [lreplace $oldvalue [expr {$pos-1}] $pos]
			} else {
				set result [lreplace $oldvalue $pos $pos $res]
			}
		}
	}
	if {"$result" == "[lindex $structure end]"} {
		return -code 5 $result
	} else {
		return $result
	}
}

proc ::Extral::unsetnamed {structure data oldvalue field} {
#putsvars structure data oldvalue field
	if {"$field" == ""} {
		return -code 5 ""
	} else {
		set struct [lindex $structure 1]
		set tag [lshift field]
		set pos [structlfind $oldvalue $tag]
		if {$pos == -1} {
			set code [catch {::Extral::structlunsetstruct $struct $data "" [llength $field] $field} res]
			if {$code == 1} {
				error "$res in named \"$tag\""
			} elseif {$code == 5} {
				set result $oldvalue
			} else {
				lappend oldvalue $tag
				lappend oldvalue $res
				set result $oldvalue
			}
		} else {
			set code [catch {::Extral::structlunsetstruct $struct $data [lindex $oldvalue $pos] [llength $field] $field} res]
			if {$code == 1} {
				error "$res in named \"$tag\""
			} elseif {$code == 5} {
				set result [lreplace $oldvalue [expr {$pos-1}] $pos]
			} else {
				set result [lreplace $oldvalue $pos $pos $res]
			}
		}
	}
	if {"$result" == "[lindex $structure end]"} {
		return -code 5 $result
	} else {
		return $result
	}
}

proc ::Extral::getnamed {structure data field value} {
#putsvars structure data field value
	set struc [lindex $structure 1]
	if {"$field" == ""} {
		set result ""
		foreach {name val} $value {
			lappend result $name [structlgetstruct $struc $data $val 0 ""]
		}
		return $result
	} else {
		set tag [lshift field]
		set pos [structlfind $value $tag]
		return [structlgetstruct $struc $data [lindex $value $pos] [llength $field] $field]
	}
}

proc ::Extral::setlist {structure data oldvalue field value} {
#putsvars structure oldvalue field value
	set tag [lshift field]
	set struct [lindex $structure 1]
	if {"$tag" == ""} {
		set result ""
		foreach val $value oldval $oldvalue {
			set code [catch {::Extral::structlsetstruct $struct $data $oldval [llength $field] $field $val} res]
			if {$code == 1} {
				error $res
			} else {
				lappend result $res
			}
		}
		return $result
	} elseif {"$tag" == "next"} {
		set code [catch {::Extral::structlsetstruct $struct $data "" [llength $field] $field $value} res]
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
		set code [catch {::Extral::structlsetstruct $struct $data [lindex $oldvalue $tag] [llength $field] $field $value} res]
		if {$code == 1} {
			error $res
		} else {
			return [lreplace $oldvalue $tag $tag $res]
		}
	} else {
		return -code error "wrong # args to list: \"$tag\""
	}
}

proc ::Extral::unsetlist {structure data oldvalue field} {
#putsvars structure oldvalue field
	set tag [lshift field]
	set struct [lindex $structure 1]
	if {"$tag" == ""} {
		return -code 5 ""
	} elseif {[llength $tag] == 1} {
		set len [llength $oldvalue]
		if {$len == 0} {
			return $oldvalue
		}
		incr len -1
		if {("$tag"!="end")&&($tag>$len)} {
			return $oldvalue
		}
		set code [catch {::Extral::structlunsetstruct $struct $data [lindex $oldvalue $tag] [llength $field] $field} res]
		if {$code == 1} {
			error $res
		} elseif {$code == 5} {
			return [lreplace $oldvalue $tag $tag]
		} else {
			return [lreplace $oldvalue $tag $tag $res]
		}
	} else {
		return -code error "wrong # args to list: \"$tag\""
	}
}

proc ::Extral::getlist {structure data field value} {
#putsvars structure field value
	set tag [lshift field]
	set struct [lindex $structure 1]
	if {"$tag" == ""} {
		set result ""
		foreach val $value {
			set code [catch {::Extral::structlgetstruct $struct $data $val [llength $field] $field} res]
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
			return ""
		}
		incr len -1
		if {("$tag"!="end")&&($tag>$len)} {
			return ""
		}
		set code [catch {::Extral::structlgetstruct $struct $data [lindex $value $tag] [llength $field] $field} res]
		if {$code == 1} {
			error $res
		} else {
			return $res
		}
	} elseif {[llength $tag] == 2} {
		set len [llength $value]
		if {$len == 0} {
			return ""
		}
		incr len -1
		set start [lindex $tag 0]
		set end [lindex $tag 1]
		if {("$start"!="end")&&($start>$len)} {
			return ""
		}
		if {("$end"!="end")&&($end>$len)} {
			set end $len
		}
		set result ""
		foreach val [lrange $value $start $end] {
			set code [catch {::Extral::structlgetstruct $struct $data $val [llength $field] $field} res]
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
