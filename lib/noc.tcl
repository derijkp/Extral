# File containing the Tcl part of the list commands in the Extral extension
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

Extral::export {
	lpop lshift lsub lfind lcor lremdup llremove lmerge lunmerge replace leval
} {

#doc {listcommands lpop} cmd {
#lpop listName ?pos?
#} descr {
#	returns the last element from a list, thereby removing it from the list.
#	If pos is given it will return the pos element of the list.
#}
proc lpop {listname {pos end}} {
	upvar $listname list
	if {"$list"==""} {
		return ""
	}
	set result [lindex $list $pos]
	set list [lreplace $list $pos $pos]
	return $result
}

#doc {listcommands lshift} cmd {
#lshift listName
#} descr {
#	returns the first element from a list, thereby removing it from the list.
#}
proc lshift {listname} {
	upvar $listname list
	set result [lindex $list 0]
	set list [lrange $list 1 end]
	return $result
}

#doc {listcommands lsub} cmd {
#lsub list ?-exclude? [index list]
#} descr {
#	create a sublist from a set of indices
#	When -exclude is specified, the elements of which the indexes are not in the list 
#	will be given.
#} example {
#	% lsub {Ape Ball Field {Antwerp city} Egg} {0 3}
#	Ape {Antwerp city}
#	% lsub {Ape Ball Field {Antwerp city} Egg} -exclude {0 3}
#	Ball Field Egg
#}
proc lsub {list args} {
	set len [llength $args]
	if {$len==1} {
		set result ""
		set len [llength $list]
		foreach index [lindex $args 0] {
			if {($index>-1)&&($index<$len)} {
				lappend result [lindex $list $index]
			}
		}
		return $result
	} elseif {($len == 2)&&("[lindex $args 0]"=="-exclude")} {
		set result ""
		foreach index [lsort -integer -decreasing [lindex $args 1]] {
			set list [lreplace $list	$index $index]
		}
		return $list
	} else {
		error "Format is \"lsub list ?-exclude? indices\""
	}
}

#doc {listcommands lfind} cmd {
#lfind mode list pattern
#} descr {
#	returns a list of all indices which match a pattern.
#	mode can be -exact, -glob, or -regexp
#} example {
#	% lfind -regexp {Ape Ball Field {Antwerp city} Egg} {^A}
#	0 3
#}
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

#doc {listcommands lcor} cmd {
#lcor <referencelist> <list>
#} descr {
#	gives the positions of the elements in list in the reference list. If an element is not
#	found in the reference list, it returns -1. Elements are matched only once.
#} example {
#	% lcor {a b c d e f} {d b}
#	3 1
#	% lcor {a b c d e f} {b d d}
#	1 3 -1
#}
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


#doc {listcommands lremdup} cmd {
#lremdup list
#} descr {
#returns a list in which all duplactes are removed
#}
proc lremdup list {
	set done ""
	foreach e $list {
		if {[lsearch $done $e]==-1} {
			lappend done $e
		}
	}
	return $done
}

#doc {listcommands lremove} cmd {
#llremove list1 list2
#} descr {
#        returns a list with all items in list1 that are not in list2
#}
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
 
#doc {listcommands lmerge} cmd {
#lmerge ?list1? ?list2? ??spacing??
#} descr {
#	merges two lists into one
#} example {
#	% lmerge {a b c} {1 2 3}
#	a 1 b 2 c 3
#	% lmerge {a b c d} {1 2} 2
#	a b 1 c d 2
#}
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

#doc {listcommands lunmerge} cmd {
#lunmerge ?list? ??spacing?? ??var??
#} descr {
#	unmerges items from a list to the result; the remaining items are stored
#	in the given variable ?var?
#} example {
#	% lunmerge {a 1 b 2 c 3}
#	a b c
#	% lunmerge {a b 1 c d 2} 2 var
#	a b c d
#	% set var
#	1 2
#}
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

#doc leval title {
#eval Light
#} shortdescr {
#a faster but more limited eval (Viktor Dukhovni)
#}
#doc {leval leval} cmd {
#leval command $args
#} descr {
#	converted the leval patch by Viktor Dukhovni <viktor@esm.com> to
#	a dynamically loadable version:
#	This command is a fast light "eval" specifically designed to execute
#	zero or more Tcl lists (concatenated) by invoking the command specified
#	by the first list element, with the remaining list elements as "literal"
#	arguments.  No variable or command substitution takes place on the
#	arguments.
#}

proc leval {args} {
	eval [eval concat $args]
}

#not really the same
proc replace {string replacelist} {
	foreach {pattern new} $replacelist {
		regsub -all -- $pattern $string $new string
	}
	return $string
}

#ffind
#doc ffind title {
#ffind
#} shortdescr {
#filesearcher (obsolete, only in C)
#}
#doc {ffind ffind} cmd {
#ffind <switches> filelist pattern ?varName? ?pattern? ?varname?
#} descr {
#	returns the files in filelist whose content match the given pattern.
#	if varName is given, the results will be stored in this variable.
#	several patterns can be searched, the results for each being stored
#	in the apropriate variable.
#	<switches> can be -matches, -all -exact, -glob, or -regexp
#<dl>
#	<dt>-matches    :<dd>the text matched by the bracketed part of 
#		              the pattern will be mangled into the result. 
#	<dt>-allmatches :<dd>all matches in the file will be returned in
#		              the form of: 
#		              file1 match1 file1 match2 file2 match2
#	<dt>-allfiles   :<dd>see next
#</dl>
#}
#doc {ffind ffindall} cmd {
#ffind -matches -allfiles <switches> filelist pattern nulvalue ?varName? ?pattern? ?nulvalue? ?varname? ..
#} descr {
#	ffind with these options will return a list containing one element
#	for each file in the filelist. if the pattern was found in a file,
#	the the element contains the match; if it was not found, it will
#	contain the nulvalue. This is not compatible with the -allmatches
#	options
#}

}
