# structl
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

#doc structl title {
#Structured lists
#} shortdescr {
#store structured data in lists
#} descr {
#In a structured list tags alternate with the value attached to this tag,
#eg.: {tag1 {value of tag1} tag2 {value of tag2} ...}
#Using the structured list commands, you can use a list as a sort of array.
#However, structured lists have some advantages to arrays:
#<ul>
#<li>They can be passed to functions easily
#<li>tagged lists can be nested: an element of a tagged list can
#   contain another tagged list, etc.
#<li>although finding a value in an array should be faster,
#   creating the array can take more time.
#<li>tagged list can be handled according to a certain structure
#</ul>
#}

Extral::export {structlset structlget structlunset structlfields structlfind} {

#proc structlsetstruct {structure list taglen taglist value} {
##putsvars structure list taglen taglist value
#	set tag [lpop taglist 0]
#	incr taglen -1
#	set structpos [structlfind $structure $tag]
#	if {$structpos == -1} {
#		return -code error "error: tag \"$tag\" not present in structure \"$structure\""
#	}
#	set substructure [lindex $structure $structpos]
#	set listpos [structlfind $list $tag]
#	if {$listpos == -1} {
#		set sublist ""
#	} else {
#		set sublist [lindex $list $listpos]
#	}
#
#	# Is the substructure an endnode
#	# ------------------------------
#	set ctag [lindex $substructure 0]
#	set endnode 0
#	if [regexp {^\*.+} $ctag] {
#		if {$taglen > 0} {
#			set endnode 1
#			set sublist [::Extral::set[string range $ctag 1 end] $substructure $sublist $taglist $value]
##			return -code error "error: tag \"[lindex $taglist 0]\" not present in structure \"$substructure\""
#		} elseif {"$value" == "[lindex $substructure end]"} {
#			if {$listpos != -1} {
#				set list [lreplace $list [expr $listpos-1] $listpos]
#			}
#			return $list
#		} else	{
#			set endnode 1
#			set sublist [::Extral::set[string range $ctag 1 end] $substructure $sublist $taglist $value]
#		}
#	} elseif {$taglen == 0} {
#		# Go further down structure by value
#		# ----------------------------------
#		if [expr [llength $value] & 1] {
#			return -code error "error: incorrect value trying to assign \"$value\" to struct \"$substructure\""
#		}
#		foreach {t val} $value {
#			set sublist [structlsetstruct $substructure $sublist 1 $t $val]
#		}
#	} else {
#		# Go further down structure by tags
#		# ---------------------------------
#		set sublist [structlsetstruct $substructure $sublist $taglen $taglist $value]
#	}
#
#	if {$listpos != -1} {
#		if {($endnode == 1)||("$sublist" != "")} {
#			set list [lreplace $list $listpos $listpos $sublist]
#		} else {
#			set list [lreplace $list [expr $listpos-1] $listpos]
#		}
#	} else {
#		if {($endnode == 1)||("$sublist" != "")} {
#			lappend list $tag $sublist
#		}
#	}
#	return $list
#}

proc structlsetstruct {structure list taglen taglist value} {
#putsvars structure list taglen taglist value
	set ctag [lindex $structure 0]
	if [regexp {^\*.+} $ctag] {
		# An endnode
		# ----------
		if {"$value" == "[lindex $structure end]"} {
			return -code 5 $value
		} else {
			return [::Extral::set[string range $ctag 1 end] $structure $list $taglist $value]
		}
	} elseif {$taglen == 0} {
		# Go further down structure by value
		# ----------------------------------
		set len [llength $value]
		if [expr $len & 1] {
			return -code error "error: incorrect value trying to assign \"$value\" to struct \"$structure\""
		}
		foreach {tag val} $value {
			set structpos [structlfind $structure $tag]
			if {$structpos == -1} {
				return -code error "error: tag \"$tag\" not present in structure \"$structure\""
			}
			set substructure [lindex $structure $structpos]
			set sublistpos [structlfind $list $tag]
			if {$sublistpos == -1} {
				set sublist ""
			} else {
				set sublist [lindex $list $sublistpos]
			}
			set code [catch {structlsetstruct $substructure $sublist 0 "" $val} sublist]
			if {$code == 1} {
				error $sublist
			} elseif {$code == 5} {
				if {$sublistpos != -1} {
					set list [lreplace $list [expr $sublistpos-1] $sublistpos]
				}
			} else {
				if {$sublistpos != -1} {
					set list [lreplace $list $sublistpos $sublistpos $sublist]
				} else {
					lappend list $tag $sublist
				}
			}
		}
	} else {
		# Go further down structure by tags
		# ---------------------------------
		set tag [lpop taglist 0]
		incr taglen -1
		set structpos [structlfind $structure $tag]
		if {$structpos == -1} {
			return -code error "error: tag \"$tag\" not present in structure \"$structure\""
		}
		set substructure [lindex $structure $structpos]
		set sublistpos [structlfind $list $tag]
		if {$sublistpos == -1} {
			set sublist ""
		} else {
			set sublist [lindex $list $sublistpos]
		}
		set code [catch {structlsetstruct $substructure $sublist $taglen $taglist $value} sublist]
		if {$code == 1} {
			error $sublist
		} elseif {$code == 5} {
			if {$sublistpos != -1} {
				set list [lreplace $list [expr $sublistpos-1] $sublistpos]
			}
		} else {
			if {$sublistpos != -1} {
				set list [lreplace $list $sublistpos $sublistpos $sublist]
			} else {
				lappend list $tag $sublist
			}
		}
	}
	if {"$list" == ""} {
		return -code 5 ""
	} else {
		return $list
	}
}

proc structlsetnostruct {list taglen taglist value} {
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
		return [concat $list [list $tag] [list $value]]
	} else {
		set value [list [lpop taglist] $value]
		incr taglen -1
		for {set i 0} {$i<$taglen} {incr i} {
			set ctag [lpop taglist]
			set value [list $ctag $value]
		}
		return [concat $list [list $tag] [list $value]]
	}
}

#doc {structl structlset} cmd {
#structlset ?-struct schema? list taglist value ?taglist value?
#} descr {
#set the value of a field in the structured list
#} example {
#	set the value for tag
#		% set list {a 1 b 4}
#		a 1 b 4
#		% set list [structlset $list c 3]
#		a 1 b 4 c 3
#		% structlset {a 1 b 4 c 3} b 2
#		a 1 b 2 c 3
#	example of nesting:
#		% structlset {a 1 b {a 1 b 4} c 3} {b b} 2
#		a 1 b {a 1 b 2} c 3
#	example of structure:
#		% set struct {
#			reg {*regexp {^a[0-9]} ?}
#			sub {
#				a {*any ?}
#				b {*between 0 10 ?}
#			}
#			ints {
#				* {*int ?}
#			}
#		}
#		% set data {}
#		% set data [structlset -struct $struct $data {sub b} 9]
#		sub {b 9}
#		% set data [structlset -struct $struct $data {sub b} 11]
#		error: 11 is not between 0 and 10
#		% set data [structlset -struct $struct $data ints {a 9}]
#		sub {b 9} ints {a 9}
#		% set data [structlset -struct $struct $data {sub b} ?]
#		ints {a 9}
#}
proc structlset {args} {
	if {"[lindex $args 0]" == "-struct"} {
		set struct [lindex $args 1]
		set args [lrange $args 2 end]
		set usestr 1
	} else {
		set usestr 0
	}
	set len [llength $args]
	if {($len == 0)||(![expr $len&1])} {
		return -code error "wrong # args: should be \"structlset ?-struct schema? list taglist value ?taglist value?\""
	}
	if {$usestr == 1} {
		set list [lshift args]
		foreach {taglist value} $args {
			set taglen [llength $taglist]
			set code [catch {structlsetstruct $struct $list $taglen $taglist $value} result]
			if {"$code" == 1} {
				error $result
			} elseif {"$code" == 5} {
				set list ""
			} else {
				set list $result
			}
		}
	} else {
		set list [lshift args]
		foreach {taglist value} $args {
			set taglen [llength $taglist]
			if {$taglen == 0} {
				return $value
			}
			set list [structlsetnostruct $list $taglen $taglist $value]
		}
	}
	return $list
}

proc structlgetstruct {structure list taglen taglist} {
#putsvars structure list taglen taglist
	# Is this an endnode
	# ------------------
	set ctag [lindex $structure 0]
		# The dot is important to make the distinction between * and *proc
	if [regexp {^\*.} $ctag] {
		if {$taglen > 0} {
			return [::Extral::get[string range $ctag 1 end] $structure $taglist $list]
		} elseif {"$list" == ""} {
			return [lindex $structure end]
		} else {	
			return [::Extral::get[string range $ctag 1 end] $structure $taglist $list]
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
	} else {
	
		# find substructure corresponding to tag 
		# --------------------------------------
		set tag [lpop taglist 0]
		incr taglen -1
#		if {"$tag" == "*"} {
#			set dotags ""
#			set result ""
#			foreach {tag val} $structure {
#				if {"$tag" != "*"} {
#					lappend dotags $tag
#				}
#			}	
#			foreach {tag val} $list {
#				if {"$tag" != "*"} {
#					if {[lsearch $dotags $tag] == -1} {
#						lappend dotags $tag
#					}
#				}
#			}
#			foreach tag $dotags {
#				set pos [structlfind $structure $tag]
#				set substructure [lindex $structure $pos]
#			
#				# find the tag
#				# ------------
#				set sublist [structlfind $list $tag value]
#				lappend result $tag [structlgetstruct $substructure $sublist $taglen $taglist]
#			}
#			return $result
#		}
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
			return -code error "tag \"$tag\" not found"
		} else {
			set list $found
			unset found
		}
	}
	return $list
}

#doc {structl structlget} cmd {
#structlget ?-struct schema? list taglist ?def?
#} descr {
#get the value of a field in the structured list
#} example {
#	% set struct {
#		reg {*regexp {^a[0-9]} ?}
#		sub {
#			a {*any ?}
#			b {*between 0 10 ?}
#		}
#		ints {
#			* {*int ?}
#		}
#	}
#	% structlget -struct $struct {ints {a 9}} {sub b}
#	?
#	% structlget -struct $struct {ints {a 9}} {ints}
#	a 9
#}
proc structlget {args} {
	set alen	[llength $args]
	if {($alen < 2)||($alen > 5)} {
		return -code error "wrong # args: should be \"structlget ?-struct schema? list taglist ?def?\""
	}
	if {($alen == 4)||($alen == 5)} {
		if {"[lindex $args 0]" != "-struct"} {
			return -code error "wrong option [lindex $args 0]"
		} else {
			foreach arg $args var {temp struct list taglist} {
				set $var $arg
			}
		}
		set len [llength $taglist]
		if {$alen == 4} {
			return [structlgetstruct $struct $list $len $taglist]
		} else {
			if [catch {structlgetstruct $struct $list $len $taglist} result] {
				return [lindex $args 4]
			} else {
				return $result
			}
		}
	} else {
		foreach arg $args var {list taglist} {
			set $var $arg
		}
		set len [llength $taglist]
		if {$len == 0} {
			return $list
		}
		if {$alen == 2} {
			return [structlgetnostruct $list $taglist $len]
		} else {
			if [catch {structlgetnostruct $list $taglist $len} result] {
				return [lindex $args 2]
			} else {
				return $result
			}
		}
	}
}

#doc {structl structlunset} cmd {
#structlunset list taglist
#} descr {
#unset the value of a field in the structured list
#} example {
#	% structlunset {a 1 b 2} b
#	a 1
#}
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

#doc {structl structlfields} cmd {
#structlfields list ?valueVar?
#} descr {
#returns the fields present in the structure list
#}
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
