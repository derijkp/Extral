proc ssort {args} {
	set list [lpop args]
	set pos [lsearch $args -reflist]
	if {$pos==-1} {
		return [eval lsort $args {$list}]
	} else {
		lpop args $pos
		set temp [lmanip merge $list [lpop args $pos]]
		set temp [eval lsort $args {-index 1 $temp}]
		return [lmanip subindex $temp 0]
	}
}
