# File containing the Tcl part of the list commands in the Extral extension
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================
#doc ssort title {
#ssort
#} descr {
#	enhanced lsort:
#	<dl>
#	<dt>by D. Richard Hipp -- drh@tobit.vnet.net -- 704.948.4565 :
#	<dd><ul>
#		<li> re-entrant and thread-safe: eg. ssort can be used in its
#		  own -command proc
#		<li> 10% faster
#		<li> option -dictionary: Using -dict, "B" comes in between
#		  "a" and "b".  Also "x10" comes after "x9" 
#	</ul>
#	<dt>by Peter De Rijk :
#	<dd>extra option -reflist: sort the elements in the list according
#		to the comparisons of the corresponding elements in the 
#		reflist.
#	</dl>
#}

#doc {ssort ssort} cmd {
#ssort ?-ascii? ?-integer? ?-real? ?-increasing? ?-decreasing? ?-dictionary? ?-command string? ?-reflist list? list
#}
proc ssort {args} {
	set list [lpop args]
	set pos [lsearch $args -reflist]
	if {$pos==-1} {
		return [eval lsort $args {$list}]
	} else {
		lpop args $pos
		set temp [lmanip mangle $list [lpop args $pos]]
		set temp [eval lsort $args {-index 1 $temp}]
		return [lmanip subindex $temp 0]
	}
}
