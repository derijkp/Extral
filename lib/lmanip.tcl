# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================
#doc {lmanip subindex} cmd {
#lmanip subindex ?list? ?pos?
#} descr {
#	returns a list of the 'pos' element in each of the elements of the given list
#} example {
#	% lmanip subindex {{a 1} {b 2} {c 3}} 1
#	1 2 3
#}
#doc {lmanip mangle} cmd {
#lmanip mangle ?list1? ?list2?
#} descr {
#	mangles two lists into one
#} example {
#	% lmanip mangle {a b c} {1 2 3}
#	{a 1} {b 2} {c 3}
#}
#doc {lmanip extract} cmd {
#lmanip extract ?list? ?expression?
#} descr {
#	tries to match each element in a list; if the element matches, it extracts the 
#	parenthesised part. It returns a list of all extracted parts. If there was no match,
#	an empty element is put in the list.
#} example { 
#	% lmanip extract {Results {A: 50%} {B: 25%} {C: 25%}} { ([0-9+]+)\%}
#		{} 50 25 25
#}
#doc {lmanip split} cmd {
#lmanip split ?list? -before/-after/-outside ?positions?
#} descr {
#	splits a list at positions into sublists
#} example {
#	% lmanip split {a b c d e} -before {1 3}
#	a {b c} {d e}
#}
#doc {lmanip join} cmd {
#lmanip join ?list? ?join string? ?position list?
#} descr {
#	joins list elements at positions given in the ?position list?. When you
#	specify all, all elements will be joined.
#} example {
#	% lmanip join {a b c {a d} e} { } {0 2}
#		{a b} {c a d} e
#	% lmanip join {a b c {a d} e} {} {0 2}
#		ab {ca d} e
#	% lmanip join {a b c {a d} e} {} all
#	abca de
#}
#doc {lmanip lengths} cmd {
#lmanip lengths ?list?
#} descr {
#	returns a list with the lengths of the elements
#} example {
#	% lmanip lengths {abc abcdef}
#	3 6
#}
#doc {lmanip fill} cmd {
#lmanip fill ?size? ?start? ??incr??
#} descr {
#	fills a list of ?size? elements with ?start?; if ?incr? is given and ?size? is an integer, each element in the list will be the former incremented with ?incr?
#} example {
#	% lmanip fill 4 "Hello world"
#	{Hello world} {Hello world} {Hello world} {Hello world}
#	% lmanip fill 5 2 2
#	2 4 6 8 10
#	% lmanip fill 5 10 -2
#	10 8 6 4 2
#}
if 0 {
proc Extral::lmanip {} {}
}
Extral::export lmanip {

proc lmanip {option args} {
	switch $option {
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
		mangle {
			if {[llength $args]!=2} {
				error "wrong # args: should be \"lmanip mangle list1 list2\""
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
		default {
			error "bad option \"$option\": should be subindex, mangle, extract, split, join, lengths, fill or ffill"
		}
	}
}

}
