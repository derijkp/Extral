#doc {listcommands list_fill} cmd {
#list_fill ?size? ?start? ??incr??
#} descr {
#	fills a list of ?size? elements with ?start?; if ?incr? is given and ?size? is a number, each element in the list will be the former incremented with ?incr?
#   This works for integers or strings
#} example {
#	% list_fill 4 "Hello world"
#	{Hello world} {Hello world} {Hello world} {Hello world}
#	% list_fill 5 2 2
#	2 4 6 8 10
#	% list_fill 5 10 -2
#	10 8 6 4 2
#}
proc list_fill {args} {
	if {([llength $args]!=2)&&([llength $args]!=3)} {
		error "wrong # args: should be \"list_fill size start ?incr?\""
	}
	set result ""
	foreach {size start incr} $args break
	if {[llength $args]==2} {
		for {set i 0} {$i<$size} {incr i} {
			lappend result $start
		}
	} else {
		for {set i 0} {$i<$size} {incr i} {
			lappend result $start
			set start [expr {$start + $incr}]
		}
	}
	return $result
}

interp alias {} list_ffill {} list_fill 
