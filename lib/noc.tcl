set Extral_noc 1
proc lpop {listname {pos end}} {
	upvar $listname list
	if {"$list"==""} {
		return ""
	}
	set result [lindex $list $pos]
	set list [lreplace $list $pos $pos]
	return $result
}

proc lshift {listname} {
	upvar $listname list
	set result [lindex $list 0]
	set list [lrange $list 1 end]
	return $result
}

proc lsub {list args} {
	if {[llength $args]==1} {
		set result ""
		set len [llength $list]
		foreach index [lindex $args 0] {
			if {($index>-1)&&($index<$len)} {
				lappend result [lindex $list $index]
			}
		}
		return $result
	} elseif {"[lindex $args 0]"=="-exclude"} {
		set result ""
		foreach index [lsort -integer -decreasing [lindex $args 1]] {
			set list [lreplace $list	$index $index]
		}
		return $list
	} else {
		error "Format is \"lsub list ?-exclude? indices\""
	}
}

proc lfind {args} {
	if {[llength $args]==2} {
		set list [lindex $args 0]
		set pattern [lindex $args 1]
		set mode -exact
	} elseif {[llength $args]==3} {
		set mode [lindex $args 0]
		set list [lindex $args 1]
		set pattern [lindex $args 2]
	} else {
		error "Format is \"lfind ?mode? list pattern\""
	}
	set result ""
	set pos 0
	switch -- $mode {
		{-exact} {
			foreach el $list {
				if {"$el"=="$pattern"} {lappend result $pos}
				incr pos
			}
		}
		{-glob} {
			foreach el $list {
				if [string match $pattern $el] {lappend result $pos}
				incr pos
			}
		}
		{-regexp} {
			foreach el $list {
				if [regexp $pattern $el] {lappend result $pos}
				incr pos
			}
		}
		default {
			error "Unkown mode \"$mode\""
		}
	}
	return $result
}

proc lcor {reflist list} {
	set pos 0
	foreach item $reflist {
		lappend grid($item) $pos
		incr pos
	}
	foreach item $list {
		if [info exists grid($item)] {
			lappend result [lshift grid($item)]
			if {"$grid($item)"==""} {unset grid($item)}
		} else {
			lappend result -1
		}
	}
	return $result
}

proc lremdup list {
	set done ""
	foreach e $list {
		if {[lsearch $done $e]==-1} {
			lappend done $e
		}
	}
	return $done
}

proc llremove {list removelist} {
	if {"$removelist"==""} {
		set removelist {{}}
	}
	set result ""
	foreach item $list {
		set pos [lsearch $removelist $item]
		if {$pos==-1} {
			lappend result $item
		}
	}
	return $result
}
 
proc lmerge {args} {
	if {([llength $args]!=2)&&([llength $args]!=3)} {
		error "wrong # args: should be \"lmerge list1 list2 ?spacing?\""
	}
	set result ""
	set list1 [lindex $args 0]
	set list2 [lindex $args 1]
	if ([llength $args]==3) {
		set spacing [lindex $args 2]
	} else {
		set spacing 1
	}
	if {($spacing!=1)||([llength $list1]<[llength $list2])} {
		set c $spacing
		foreach e1 $list1 {
			lappend result $e1
			incr c -1
			if !$c {
				lappend result [lshift list2]
				set c $spacing
			}
		}
		return $result
		
	} else {
		foreach e1 $list1 e2 $list2 {
			lappend result $e1 $e2
		}
		return $result
	}
}

proc lunmerge {args} {
	if {([llength $args]<1)||([llength $args]>3)} {
		error "wrong # args: should be \"lunmerge list ?spacing? ?var?\""
	}
	set result ""
	if {[llength $args]==3} {
		upvar [lindex $args 2] var
		set var ""
	}
	if {[llength $args]>1} {
		set spacing [lindex $args 1]
	} else {
		set spacing 1
	}
	if {$spacing==1} {
		foreach {e1 e2} [lindex $args 0] {
			lappend result $e1
			if [info exists var] {lappend var $e2}
		}
		return $result
	} else {
		set c $spacing
		foreach e1 [lindex $args 0] {
			if !$c {
				if [info exists var] {lappend var $e1}
				set c $spacing
			} else {
				lappend result $e1
				incr c -1
			}
		}
		return $result
		
	}
}

#not really the same
proc replace {string replacelist} {
	foreach {pattern new} $replacelist {
		regsub -all -- $pattern $string $new string
	}
	return $string
}

#ffind

#tagl
proc taglget {list tag args} {
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

proc taglset {list tag value} {
	set pos 1
	foreach {ctag cvalue} $list {
		if {"$tag"=="$ctag"} {
			return [lreplace $list $pos $pos $value]
		}
		incr pos 2
	}
	return [concat $list $tag $value]
}

proc taglunset {list tag} {
	set pos 0
	foreach {ctag cvalue} $list {
		if {"$tag"=="$ctag"} {
			return [lreplace $list $pos [expr $pos+1]]
		}
		incr pos 2
	}
	return $list
}

