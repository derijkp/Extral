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

#ffind
#amanip
#replace

