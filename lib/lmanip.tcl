proc lmanip {command args} {
	switch $command {
		subindex {
			if {[llength $args]!=2} {
				error "wrong # args: should be \"lmanip subindex list ?pos?\""
			}
			set result ""
			set index [lindex $args 1]
			foreach elem [lindex $args 0] {
				lappend result [lindex $elem $index]
			}
			return $result
		}
		merge {
			if {[llength $args]!=2} {
				error "wrong # args: should be \"lmanip merge list1 list2\""
			}
			set result ""
			foreach e1 [lindex $args 0] e2 [lindex $args 1] {
				lappend result [list $e1 $e2]
			}
			return $result
		}
		extract {
			if {[llength $args]!=2} {
				error "wrong # args: should be \"lmanip extract list expression\""
			}
			set result ""
			set pattern [lindex $args 1]
			foreach e [lindex $args 0] {
				if [regexp $pattern $e temp res] {
					lappend result $res
				} else {
					lappend result ""
				}
			}
			return $result
		}
		remdup {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"lmanip remdup list\""
			}
			set done ""
			foreach e [lindex $args 0] {
				if {[lsearch $done $e]==-1} {
					lappend done $e
				}
			}
			return $done
		}
		split {
			if {[llength $args]!=3} {
				error "wrong # args: should be \"lmanip split list -before/-after/-outside ?positions?\""
			}
			set result ""
			set list [lindex $args 0]
			set mode [lindex $args 1]
			if {"$mode"=="-before"} {
				set ii -1
				set ip 1
			} elseif {"$mode"=="-after"} {
				set ii 0
				set ip 1
			} elseif {"$mode"=="-outside"} {
				set ii -1
				set ip 2
			} else {
				error "wrong arg: $mode"
			}
			set prev 0
			foreach index [lindex $args 2] {
				incr index $ii
				lappend result [lrange $list $prev $index]
				set prev $index
				incr prev $ip
			}			
			if {[llength $list]>$prev} {
				lappend result [lrange $list $prev end]
			}
			return $result
		}
		join {
			if {[llength $args]!=3} {
				error "wrong # args: should be \"lmanip join list <join string> <positions>\""
			}
			set list [lindex $args 0]
			set join [lindex $args 1]
			if {"[lindex $args 2]"=="all"} {return [join $list $join]}
			foreach index [lsort -integer -decreasing [lindex $args 2]] {
				set i2 [expr $index+1]
				set list [lreplace $list $index $i2 [join [lrange $list $index $i2] $join]]
			}
			return $list
		}
		lengths {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"lmanip lengths list\""
			}
			set result ""
			foreach el [lindex $args 0] {
				lappend result [string length $el]
			}
			return $result
		}
		fill {
			if {([llength $args]!=2)&&([llength $args]!=3)} {
				error "wrong # args: should be \"lmanip fill ?size? ?start? ??incr??\""
			}
			set result ""
			set size [lindex $args 0]
			set item [lindex $args 1]
			if {[llength $args]==3} {
				set incr [lindex $args 2]
			}
			for {set i 0} {$i<$size} {incr i} {
				lappend result $item
				if [info exists incr] {incr item $incr}
			}
			return $result
		}
		mangle {
			if {([llength $args]!=2)&&([llength $args]!=3)} {
				error "wrong # args: should be \"lmanip mangle list1 list2 ?spacing?\""
			}
			set result ""
			if {[llength $args]==3} {
				set spacing [lindex $args 2]
				set list2 [lindex $args 1]
				set c $spacing
				foreach e1 [lindex $args 0] {
					lappend result $e1
					incr c -1
					if !$c {
						lappend result [lshift list2]
						set c $spacing
					}
				}
				return $result
				
			} else {
				foreach e1 [lindex $args 0] e2 [lindex $args 1] {
					lappend result $e1 $e2
				}
				return $result
			}
		}
		unmangle {
			if {([llength $args]<1)&&([llength $args]>3)} {
				error "wrong # args: should be \"lmanip mangle list ?spacing? ?var?\""
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
		ffill {
			if {([llength $args]!=2)&&([llength $args]!=3)} {
				error "wrong # args: should be \"lmanip ffill size start ?incr?\n - fills a list with the floating value in start, can be incremented by ?incr?\""
			}
			set result ""
			set size [lindex $args 0]
			set item [lindex $args 1]
			if {[llength $args]==3} {
				set incr [lindex $args 2]
			}
			for {set i 0} {$i<$size} {incr i} {
				lappend result $item
				if [info exists incr] {set item [expr $item+$incr]}
			}
			return $result
		}
	}
}

