#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test csv_parse {"123","""a""",,hello} {
	lindex [csv_parse {"123","""a""",,hello}] 0
} {123 {"a"} {} hello}

test csv_parse {1," o, ""a"" ,b ", 3} {
	lindex [csv_parse {1," o, ""a"" ,b ", 3}] 0
} {1 { o, "a" ,b } { 3}}

test csv_parse {"1"," o, "","" ,b ", 3} {
	lindex [csv_parse {"1"," o, "","" ,b ", 3}] 0
} {1 { o, "," ,b } { 3}}

test csv_parse {1," foo,bar,baz", 3} {
	lindex [csv_parse {1," foo,bar,baz", 3}] 0
} {1 { foo,bar,baz} { 3}}

test csv_parse {1,"""""a""""",b} {
	lindex [csv_parse {1,"""""a""""",b}] 0
} {1 {""a""} b}

test csv_parse {123,"123,521.2","Mary says "",""Hello, I am Mary"""} {
	lindex [csv_parse {123,"123,521.2","Mary says "",""Hello, I am Mary"""}] 0
} {123 123,521.2 {Mary says ","Hello, I am Mary"}}

test csv_parse {mary 2} {
	set data {123,"123,521.2","Mary says ""Hello, I am Mary"""
123,"123,521.2","Mary says ""Hello,
 I am Mary"""}
	lindex [csv_parse $data] 1
} {123 123,521.2 {Mary says "Hello,
 I am Mary"}}

test csv_parse {a,b,c,d,e,f} {
	lindex [csv_parse {a,b,c,d,e,f}] 0
} {a b c d e f}

test csv_parse {"a,1","b,2","c,3","d,4"} {
	lindex [csv_parse {"a,1","b,2","c,3","d,4"}] 0
} {a,1 b,2 c,3 d,4}

test csv_parse {"a,1","b,2","c,3","d,4"} {
	lindex [csv_parse {"a,1","b,2","c,3","d,4"}] 0
} {a,1 b,2 c,3 d,4}

test csv_parse long {
	set data [string repeat ab 1000],\"[string repeat xx 500]\"\"[string repeat xx 500]\",asdfsg"
	llength [lindex [csv_parse $data] 0]
} 3

test csv_parse {long 2} {
	set pattern "\"a,1\",\"b,2\",\"c,3\",\"d,4\"\n"
	set data [csv_parse [string repeat $pattern 10]]
	list [llength $data] [lindex $data 5]
} {11 {a,1 b,2 c,3 d,4}}

test csv_parse {long 3} {
	set data [string repeat "\"a,1\",\"b,2\",\"c,3\"\n" 100]
	set data [csv_parse $data]
	list [llength $data] [lindex $data 5]
} {101 {a,1 b,2 c,3}}

test csv_parse {long 4} {
	set data [string repeat "\"a1\",\"b2\",\"c3\"\n" 100]
	set data [csv_parse $data]
	list [llength $data] [lindex $data 5]
} {101 {a1 b2 c3}}

test csv_parse {long 5} {
	set data [string repeat "\"a\"\"1\",\"b\"\"2\"\n" 100]]
	set data [csv_parse $data]
	list [llength $data] [lindex $data 5]
} {101 {a\"1 b\"2}}

test csv_parse {long 6} {
	set data [string repeat [string repeat "\"a1\",\"b2\",\"c3\"," 50]\n 100]
	set data [csv_parse $data]
	list [llength $data] [lrange [lindex $data 5] 0 4]
} {101 {a1 b2 c3 a1 b2}}


testsummarize

if 0 {

proc csv:parse {line {sepa ,}} {
     set lst [split $line $sepa]
     set nlst {}
     set l [llength $lst]
     for {set i 0} {$i < $l} {incr i} {
         if {[string index [lindex $lst $i] 0] == "\""} {
            # start of a stringhttp://purl.org/thecliff/tcl/wiki/721.html
            if {[string index [lindex $lst $i] end] == "\""} {
               # check for completeness, on our way we repair double double quotes
               set c1 [string range [lindex $lst $i] 1 end]
               set n1 [regsub -all {""} $c1 {"} c2]
               set n2 [regsub -all {"} $c2 {"} c3]
               if {$n1 == $n2} {
                  # string extents to next list element
                  set new_el [join [lrange $lst $i [expr {$i + 1}]] $sepa]
                  set lst [lreplace $lst $i [expr {$i + 1}] $new_el]
                  incr i -1
                  incr l -1
                  continue
                  } else {
                  # we are done with this element
                  lappend nlst [string range $c2 0 [expr {[string length $c2] - 2}]]
                  continue
                  }
               } else {
               # string extents to next list element
               set new_el [join [lrange $lst $i [expr {$i + 1}]] $sepa]
               set lst [lreplace $lst $i [expr {$i + 1}] $new_el]
               incr i -1
               incr l -1
               continue
               }
            } else {
            # the most simple case
            lappend nlst [lindex $lst $i]
            continue
            }
         }
     return $nlst
}
proc csv2list {str {sepChar ,}} {
    regsub -all {(\A\"|\"\Z)} $str \0 str
    set str [string map [list $sepChar\"\"\" $sepChar\0\" \
                              \"\"\"$sepChar \"\0$sepChar \
                              \"\" \" \" \0 ] $str]
    set end 0
    while {[regexp -indices -start $end {(\0)[^\0]*(\0)} $str \
            -> start end]} {
        set start [lindex $start 0]
        set end   [lindex $end 0]
        set range [string range $str $start $end]
        set first [string first $sepChar $range]
        if {$first >= 0} {
            set str [string replace $str $start $end \
                [string map [list $sepChar \1] $range]]
        }
        incr end
    }
    set str [string map [list $sepChar \0 \1 $sepChar \0 {} ] $str]
    return [split $str \0]
 }
set str(01) {"123","""a""",,hello}
set str(02) {1," o, ""a"" ,b ", 3}
set str(03) {"1"," o, "","" ,b ", 3}
set str(04) {1," foo,bar,baz", 3}
set str(05) {1,"""""a""""",b}
set str(06) {123,"123,521.2","Mary says "",""Hello, I am Mary"""}
set str(07) {a,b,c,d,e,f}
set str(08) {"a,1","b,2","c,3","d,4"}
set str(08) {"a,1","b,2","c,3","d,4"}
set str(09) "[string repeat ab 1000],\"[string repeat xx 500]\"\"[string repeat xx 500]\",asdfsg"
set str(10) [string repeat {"a,1","b,2","c,3","d,4",} 50]
set str(11) [string repeat {a,b,c,d,e} 50]
set tests [lsort [array names str]]
set results [format "%-10s" {}]
foreach name $tests {
	append results [format "%6s" $name]
}
puts $results
foreach cmd {csv:parse csv2list csv_parse1 csv_parse} {
	set results [format "%-10s" $cmd]
	foreach name $tests {
		# puts $cmd,$name:[$cmd $str($name)]
		set time [lindex [time {$cmd $str($name)} 100] 0]
		append results [format "%6d" $time]
	}
	puts $results
}

}
