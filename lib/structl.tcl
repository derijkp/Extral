# File containing the Tcl part of the list commands in the Extral extension
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

Extral::export {structlset structlget structlunset structlfields structlfind} {

proc structlgetstruct {structure list taglen taglist} {
	# Is this an endnode
	# ------------------
	set ctag [lindex $structure 0]
	if [regexp {^\*.+} $ctag] {
		if {$taglen > 0} {
			return -code error "error: not all tags in structure"
		} elseif {"$list" == ""} {
			return [lindex $structure end]
		} else {	
			return [::Extral::get[string range $ctag 1 end] $structure $list]
		}
	}

	# out of tags
	# -----------
	if {$taglen == 0} {
		set structlen [llength $structure]
		if [expr $structlen&1] {
			return -code error "error: structure \"$structure\" does not have an even number of elements"
		} elseif {$structlen != 0} {
			set result ""
			foreach {tag str} $structure {
				if {"$tag" != "*"} {
					set sublist [structlfind $list $tag value]
					lappend result $tag [structlgetstruct $str $sublist 0 ""]
				} else {
					set wstructure $str
				}
			}
			if [info exists wstructure] {
				foreach {tag sublist} $list {
					if {[structlfind $result $tag]== -1} {
						lappend result $tag [structlgetstruct $wstructure $sublist 0 ""]
					}
				}
			}
			return $result
		} else {
			return $list
		}
	}

	# find substructure corresponding to tag 
	# --------------------------------------
	set tag [lpop taglist 0]
	incr taglen -1
	set pos [structlfind $structure $tag]
	if {$pos == -1} {
		return -code error "error: tag \"$tag\" not present in structure \"$structure\""
	}
	set substructure [lindex $structure $pos]

	# find the tag
	# ------------
	set sublist [structlfind $list $tag value]
	return [structlgetstruct $substructure $sublist $taglen $taglist]
}

proc structlgetnostruct {list taglist taglen} {
	foreach tag $taglist {
		set len [llength $list]
		if {[expr $len%2] != 0} {
			return -code error "error: list \"$list\" does not have an even number of elements"
		}
		foreach {ctag value} $list {
			if {"$tag"=="$ctag"} {
				set found $value
			}
		}
		if ![info exists found] {
			return -code error "taglist \"$taglist\" not found"
		} else {
			set list $found
			unset found
		}
	}
	return $list
}

proc structlget {args} {
	set len	[llength $args]
	if {($len != 2)&&($len != 4)} {
		return -code error "wrong # args: should be \"structlget ?-struct schema? list taglist\""
	}
	if {$len == 4} {
		if {"[lindex $args 0]" != "-struct"} {
			return -code error "wrong option [lindex $args 0]"
		} else {
			foreach arg $args var {temp struct list taglist} {
				set $var $arg
			}
		}
		set len [llength $taglist]
		return [structlgetstruct $struct $list $len $taglist]
	} else {
		foreach arg $args var {list taglist} {
			set $var $arg
		}
		set len [llength $taglist]
		if {$len == 0} {
			return $list
		}
		return [structlgetnostruct $list $taglist $len]
	}
}

proc structlsetstruct {structure list taglen taglist value} {
#putsvars structure list taglen taglist value
	set tag [lpop taglist 0]
	incr taglen -1
	set structpos [structlfind $structure $tag]
	if {$structpos == -1} {
		return -code error "tag \"$tag\" not present in structure \"$structure\""
	}
	set substructure [lindex $structure $structpos]
	set listpos [structlfind $list $tag]
	if {$listpos == -1} {
		set sublist ""
	} else {
		set sublist [lindex $list $listpos]
	}

	# Is the substructure an endnode
	# ------------------------------
	set ctag [lindex $substructure 0]
	set endnode 0
	if [regexp {^\*.+} $ctag] {
		if {$taglen > 0} {
			return -code error "error: tag \"[lindex $taglist 0]\" not present in structure \"$substructure\""
		} elseif {"$value" == "[lindex $substructure end]"} {
			if {$listpos != -1} {
				set list [lreplace $list [expr $listpos-1] $listpos]
			}
			return $list
		} else	{
			set endnode 1
			set sublist [::Extral::set[string range $ctag 1 end] $substructure $sublist $value]
		}
	} elseif {$taglen == 0} {
		# Go further down structure by value
		# ----------------------------------
		if [expr [llength $value] & 1] {
			return -code error "error: incorrect value trying to assign \"$value\" to struct \"$substructure\""
		}
		foreach {t val} $value {
			set sublist [structlsetstruct $substructure $sublist 1 $t $val]
		}
	} else {
		# Go further down structure by tags
		# ---------------------------------
		set sublist [structlsetstruct $substructure $sublist $taglen $taglist $value]
	}

	if {$listpos != -1} {
		if {($endnode == 1)||("$sublist" != "")} {
			set list [lreplace $list $listpos $listpos $sublist]
		} else {
			set list [lreplace $list [expr $listpos-1] $listpos]
		}
	} else {
		if {($endnode == 1)||("$sublist" != "")} {
			lappend list $tag $sublist
		}
	}
	return $list
}

proc structlsetnostruct {list taglen taglist value} {
#putsvars list taglen taglist value
	set tag [lpop taglist 0]
	incr taglen -1
	set pos 1
	set len [llength $list]
	if {[expr $len%2] != 0} {
		return -code error "error: \"$list\" does not have an even number of elements"
	}
	foreach {ctag cvalue} $list {
		if {"$tag"=="$ctag"} {
			if {$taglen != 0} {
				set value [structlsetnostruct [lindex $list $pos] $taglen $taglist $value]
			}
			return [lreplace $list $pos $pos $value]
		}
		incr pos 2
	}
	if {$taglen == 0} {
		return [concat $list $tag $value]
	} else {
		set value [list [lpop taglist] $value]
		incr taglen -1
		for {set i 0} {$i<$taglen} {incr i} {
			set ctag [lpop taglist]
			set value [list $ctag $value]
		}
		return [concat $list $tag [list $value]]
	}
}

proc structlset {args} {
	set len	[llength $args]
	if {($len != 3)&&($len != 5)} {
		return -code error "wrong # args: should be \"structlset ?-struct schema? list taglist value\""
	}
	if {$len == 5} {
		if {"[lindex $args 0]" != "-struct"} {
			return -code error "wrong option [lindex $args 0]"
		} else {
			foreach arg $args var {temp struct list taglist value} {
				set $var $arg
			}
		}
		set taglen [llength $taglist]
		if {$taglen != 0} {
			return [structlsetstruct $struct $list $taglen $taglist $value]
		} else {
			foreach {t val} $value {
				set list [structlsetstruct $struct $list 1 $t $val]
			}
			return $list
		}

	} else {
		foreach arg $args var {list taglist value} {
			set $var $arg
		}
		set taglen [llength $taglist]
		if {$taglen == 0} {
			return $value
		}
		return [structlsetnostruct $list $taglen $taglist $value]
	}
}

proc structlunset {list taglist} {
	set len [llength $list]
	if {[expr $len%2] != 0} {
		return -code error "error: list \"$list\" does not have an even number of elements"
	}
	set pos 1
	set tag [lpop taglist 0]
	set taglen [llength $taglist]
	foreach {ctag cvalue} $list {
		if {"$tag"=="$ctag"} {
			if {$taglen == 0} {
				return [lreplace $list [expr $pos-1] $pos]
			} else {
				set temp [structlunset [lindex $list $pos] $taglist]
				return [lreplace $list $pos $pos $temp]
			}
		}
		incr pos 2
	}
	return $list
}

proc structlfields {list args} {
	set len [llength $args]
	if {($len != 0)&&($len != 1)} {
		return -code error "wrong # args: should be \"structlfields list ?valueVar?\""
	}
	set len [llength $list]
	if {[expr $len%2] != 0} {
		return -code error "error: list \"$list\" does not have an even number of elements"
	}
	if {$len == 0} {
		foreach {tag val} $list {
			lappend result $tag
		}	
	} else {
		upvar [lindex $args 0] values
		set values ""
		foreach {tag val} $list {
			lappend result $tag
			lappend values $val
		}	
	}
	return $result
}

proc structlfind {list tag args} {
	set pos 1
	foreach {ctag val} $list {
		if {"$tag" == "$ctag"} {
			if {"$args" == ""} {
				return $pos
			} else {
				return $val
			}
		}
		incr pos 2
	}	
	if {"[lindex $list 0]" == "*"} {
		if {"$args" == ""} {
			return 1
		} else {
			return [lindex $list 1]
		}
	}
	if {"$args" == ""} {
		return -1
	} else {
		return ""
	}
}

}
