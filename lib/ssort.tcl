# File containing the Tcl part of the list commands in the Extral extension
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

Extral::export ssort {

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

}
