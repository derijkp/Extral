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
#In a structured list names alternate with the value attached to this name,
#eg.: {name1 {value of name1} name2 {value of name2} ...}
#Using the structured list commands, you can use a list as a sort of array.
#However, structured lists have some advantages to arrays:
#<ul>
#<li>They can be passed to functions easily
#<li>structured lists can be nested: an element of a structured list can
#   contain another structured list, etc.
#<li>although finding a value in an array should be faster,
#   creating the array can take more time.
#<li>structured list can be handled according to a certain schema using the -struct option.
#</ul>
# Using structured lists, data can be stored in a treelike structure (see examples further down).
# Using the struclget and structlset functions, data in any of the branches or leaves can be
# easily obtained or set, using a field (a list of names of successive branches).
# <p>
# Using the -struct option a schema can be specified that puts constraints on which branches are 
# allowed, and what values are allowed in the branches.
# A schema is also organised as a structured list. when the first element in a value starts with 
# an asterisk, it is an endnode. Otherwise it is the schema of the substructure starting
# at the name of that value.
# An endnode consists of a type indicator (the first element starting with an asterisk) and 
# type parameters. A number of types 
# are available by default (*any, *int, *regexp, *date, *named *list, ...). 
# New types can be added using either Tcl or C code.
# <p>
# If a schema contains names consisting of a list where element 0 is a questionmark these are treated specially: 
# The list must have 2 further elements: element 1 is the long name for the value, and element
# 2 the short name. Both long and short name can be used to set or get values from the structured list.
# However, structlset will always return a struct with the short name (efficient storage), while
# structlget will return the long form.
#}

if 0 {
proc Extral::structlset {} {}
proc Extral::structlget {} {}
proc Extral::structlunset {} {}
proc Extral::structlfields {} {}
proc Extral::structlfind {} {}
}
Extral::export {structlset structlget structlunset structlfields structlfind} {

proc structlsetstruct {structure data list taglen taglist value} {
#putsvars structure list taglen taglist value
	set ctag [lindex $structure 0]
	if [regexp {^\*[^ ]} $ctag] {
		# An endnode
		# ----------
		return [::Extral::set[string range $ctag 1 end] $structure $data $list $taglist $value]
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
			set structtag [lindex $structure [expr {$structpos-1}]]
			if {"[lindex $structtag 0]" == "?"} {
				set tag [lindex $structtag 2]
			}
			set substructure [lindex $structure $structpos]
			set sublistpos [structlfind $list $tag]
			if {$sublistpos == -1} {
				set sublist ""
			} else {
				set sublist [lindex $list $sublistpos]
			}
			set code [catch {structlsetstruct $substructure $data $sublist 0 "" $val} sublist]
			if {$code == 1} {
				error "$sublist at field \"$tag\""
			} elseif {$code == 5} {
				if {$sublistpos != -1} {
					set list [lreplace $list [expr $sublistpos-1] $sublistpos]
				}
			} else {
				if {$sublistpos != -1} {
					set list [lreplace $list $sublistpos $sublistpos $sublist]
				} else {
					if {"[lindex $tag 0]" == "?"} {
						lappend list [lindex $tag 2] $sublist
					} else {
						lappend list $tag $sublist
					}
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
		set structtag [lindex $structure [expr {$structpos-1}]]
		if {"[lindex $structtag 0]" == "?"} {
			set tag [lindex $structtag 2]
		}
		set substructure [lindex $structure $structpos]
		set sublistpos [structlfind $list $tag]
		if {$sublistpos == -1} {
			set sublist ""
		} else {
			set sublist [lindex $list $sublistpos]
		}
		set code [catch {structlsetstruct $substructure $data $sublist $taglen $taglist $value} sublist]
		if {$code == 1} {
			error "$sublist at field \"$tag\""
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
#structlset ?-struct schema? ?-data clientdata? list field value ?field value ...?
#} descr {
# set the value of a field in the structured list. The -data option can be used to
# pass data to self defined data types.
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
#				*named {*int ?}
#			}
#		}
#		% set data {}
#		% set data [structlset -struct $struct $data {sub b} 9]
#		sub {b 9}
#		% set data [structlset -struct $struct $data {sub b} 11]
#		error: 11 is not between 0 and 10 at field "b" at field "sub"
#		% set data [structlset -struct $struct $data ints {a 9}]
#		sub {b 9} ints {a 9}
#		% set data [structlset -struct $struct $data {sub b} ?]
#		ints {a 9}
#}
proc structlset {args} {
	set usestr 0
	set data {}
	set len [llength $args]
	while {$len >= 0} {
		if {"[lindex $args 0]" == "-struct"} {
			set struct [lindex $args 1]
			set args [lrange $args 2 end]
			incr len -2
			set usestr 1
		} elseif {"[lindex $args 0]" == "-data"} {
			set data [lindex $args 1]
			set args [lrange $args 2 end]
			incr len -2
		} else break
	}
	if {($len < 3)||(![expr $len&1])} {
		return -code error "wrong # args: should be \"structlset ?-struct schema? ?-data clientdata? list field value ?field value ...?\""
	}
	if {$usestr == 1} {
		set list [lshift args]
		foreach {taglist value} $args {
			set taglen [llength $taglist]
			set code [catch {structlsetstruct $struct $data $list $taglen $taglist $value} result]
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

proc structlgetstruct {structure data list taglen taglist} {
#putsvars structure data list taglen taglist
	# Is this an endnode
	# ------------------
	set ctag [lindex $structure 0]
	if [regexp {^\*[^ ]} $ctag] {
		return [::Extral::get[string range $ctag 1 end] $structure $data $taglist $list]
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
				if {"[lindex $tag 0]" == "?"} {
					set sublist [structlfind $list [lindex $tag 2] value]
					lappend result [lindex $tag 1] [structlgetstruct $str $data $sublist 0 ""]
				} else {
					set sublist [structlfind $list $tag value]
					lappend result $tag [structlgetstruct $str $data $sublist 0 ""]
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
		set pos [structlfind $structure $tag]
		if {$pos == -1} {
			return -code error "error: tag \"$tag\" not present in structure \"$structure\""
		}
		set substructure [lindex $structure $pos]
		set structtag [lindex $structure [expr {$pos-1}]]
		if {"[lindex $structtag 0]" == "?"} {
			set tag [lindex $structtag 2]
		}
	
		# find the tag
		# ------------
		set sublist [structlfind $list $tag value]
		return [structlgetstruct $substructure $data $sublist $taglen $taglist]
	}
}

proc structlgetnostruct {list taglen taglist} {
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
#structlget ?-struct schema? list field ?field ...?
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
#			*named {*int ?}
#		}
#	}
#	% structlget -struct $struct {ints {a 9}} {sub b}
#	?
#	% structlget -struct $struct {ints {a 9}} {ints}
#	a 9
#}
proc structlget {args} {
	set usestr 0
	set data {}
	set alen [llength $args]
	set pos 0
	while {$alen >= 0} {
		if {"[lindex $args 0]" == "-struct"} {
			set struct [lindex $args 1]
			set args [lrange $args 2 end]
			incr alen -2
			set usestr 1
		} elseif {"[lindex $args 0]" == "-data"} {
			set data [lindex $args 1]
			set args [lrange $args 2 end]
			incr alen -2
		} else break
	}
	if {$alen < 2} {
		return -code error "wrong # args: should be \"structlget ?-struct schema? list field ?field ...?\""
	}
	set list [lshift args]
	if {$alen == 2} {
		set taglist [lindex $args 0]
		set len [llength $taglist]
		if {$usestr == 1} {
			return [structlgetstruct $struct $data $list $len $taglist]
		} else {
			return [structlgetnostruct $list $len $taglist]
		}
	} else {
		set result ""
		foreach taglist $args {
			set len [llength $taglist]
			if {$usestr == 1} {
				lappend result [structlgetstruct $struct $data $list $len $taglist]
			} else {
				lappend result [structlgetnostruct $list $len $taglist]
			}
		}
		return $result
	}
}

proc structlunsetstruct {structure data list taglen taglist} {
#putsvars structure list taglen taglist
	set ctag [lindex $structure 0]
	if [regexp {^\*[^ ]} $ctag] {
		# An endnode
		# ----------
		set cmd ::Extral::unset[string range $ctag 1 end]
		if {"[info commands $cmd]" != ""} {
			set code [catch {$cmd $structure $data $list $taglist} res]
			return -code $code $res
		} else {
			return -code 5 ""
		}
	} elseif {$taglen == 0} {
		return -code 5 ""
	} else {
		# Go further down structure by tags
		# ---------------------------------
		set tag [lpop taglist 0]
		incr taglen -1
		set structpos [structlfind $structure $tag]
		if {$structpos == -1} {
			return -code error "error: tag \"$tag\" not present in structure \"$structure\""
		}
		set structtag [lindex $structure [expr {$structpos-1}]]
		if {"[lindex $structtag 0]" == "?"} {
			set tag [lindex $structtag 2]
		}
		set substructure [lindex $structure $structpos]
		set sublistpos [structlfind $list $tag]
		if {$sublistpos == -1} {
			set sublist ""
		} else {
			set sublist [lindex $list $sublistpos]
		}
		set code [catch {structlunsetstruct $substructure $data $sublist $taglen $taglist} sublist]
		if {$code == 1} {
			error "$sublist at field \"$tag\""
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

proc structlunsetnostruct {list taglist} {
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

#doc {structl structlunset} cmd {
#structlunset ?-struct schema? ?-data clientdata? list field ?field ...?
#} descr {
#unset the value of a field in the structured list
#} example {
#	% structlunset {a 1 b 2} b
#	a 1
#}
proc structlunset {args} {
	set usestr 0
	set data {}
	set len [llength $args]
	set pos 0
	while {$len >= 0} {
		if {"[lindex $args 0]" == "-struct"} {
			set struct [lindex $args 1]
			set args [lrange $args 2 end]
			incr len -2
			set usestr 1
		} elseif {"[lindex $args 0]" == "-data"} {
			set data [lindex $args 1]
			set args [lrange $args 2 end]
			incr len -2
		} else break
	}
	if {$len < 2} {
		return -code error "wrong # args: should be \"structlunset ?-struct schema? ?-data clientdata? list field ?field ... ?\""
	}
	if {$usestr == 1} {
		set list [lshift args]
		foreach taglist $args {
			set taglen [llength $taglist]
			set code [catch {structlunsetstruct $struct $data $list $taglen $taglist} result]
			if {"$code" == 1} {
				error $result
			} elseif {"$code" == 5} {
				set list ""
			} else {
				set list $result
			}
		}
		return $list
	} else {
		return [structlunsetnostruct [lindex $args 0] [lindex $args 1]]
	}
}

#doc {structl structlfields} cmd {
#structlfields list field ?valueVar?
#} descr {
#returns the fields present in the structure list
#}
proc structlfields {list {field {}} args} {
	set len [llength $args]
	if {($len != 0)&&($len != 1)} {
		return -code error "wrong # args: should be \"structlfields list field ?valueVar?\""
	}
	set list [structlget $list $field]
	set len [llength $list]
	if {[expr $len%2] != 0} {
		return -code error "error: list \"$list\" does not have an even number of elements"
	}
	if {$len == 0} {
		foreach {tag val} $list {
			if {"[lindex $tag 0]" == "?"} {
				lappend result [lindex $tag 1]
			} else {
				lappend result $tag
			}
		}	
	} else {
		upvar [lindex $args 0] values
		set values ""
		foreach {tag val} $list {
			if {"[lindex $tag 0]" == "?"} {
				lappend result [lindex $tag 1]
			} else {
				lappend result $tag
			}
			lappend values $val
		}	
	}
	return $result
}

proc structlfind {list tag args} {
	set pos 1
	foreach {ctag val} $list {
		if {"[lindex $ctag 0]" == "?"} {
			if {("$tag" == "[lindex $ctag 2]")||("$tag" == "[lindex $ctag 1]")} {
				if {"$args" == ""} {
					return $pos
				} else {
					return $val
				}
			}
		} else {
			if {"$tag" == "$ctag"} {
				if {"$args" == ""} {
					return $pos
				} else {
					return $val
				}
			}
		}
		incr pos 2
	}	
	if {"$args" == ""} {
		return -1
	} else {
		return ""
	}
}

}
