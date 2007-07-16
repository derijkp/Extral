#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test csv_parse {"123","""a""",,hello} {
	lindex [csv_parse {"123","""a""",,hello}] 0
} {123 {"a"} {} hello}

test csv_parse {1," o, ""a"" ,b ", 3} {
	lindex [csv_parse {1," o, ""a"" ,b ", 3}] 0
} {1 { o, "a" ,b } 3}

test csv_parse {"1"," o, "","" ,b ", 3} {
	lindex [csv_parse {"1"," o, "","" ,b ", 3}] 0
} {1 { o, "," ,b } 3}

test csv_parse {1," foo,bar,baz", 3} {
	lindex [csv_parse {1," foo,bar,baz", 3}] 0
} {1 { foo,bar,baz} 3}

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
} {10 {a,1 b,2 c,3 d,4}}

test csv_parse {long 3} {
	set data [string repeat "\"a,1\",\"b,2\",\"c,3\"\n" 100]
	set data [csv_parse $data]
	list [llength $data] [lindex $data 5]
} {100 {a,1 b,2 c,3}}

test csv_parse {long 4} {
	set data [string repeat "\"a1\",\"b2\",\"c3\"\n" 100]
	set data [csv_parse $data]
	list [llength $data] [lindex $data 5]
} {100 {a1 b2 c3}}

test csv_parse {long 5} {
	set data [string repeat "\"a\"\"1\",\"b\"\"2\"\n" 100]
	set data [csv_parse $data]
	list [llength $data] [lindex $data 5]
} {100 {a\"1 b\"2}}

test csv_parse {long 6} {
	set data [string repeat [string repeat "\"a1\",\"b2\",\"c3\"," 50]\n 100]
	set data [csv_parse $data]
	list [llength $data] [lrange [lindex $data 5] 0 4]
} {100 {a1 b2 c3 a1 b2}}

test csv_parse {empty at the end} {
	set data "\"test\"\t\"\"\n\t\t\t\n\"test2\""
	lindex [csv_parse $data \t] 0
} {test {}}

test csv_parse {1," o, ""a"" ,b ", 3, linecmd} {
	set result {}
	csv_parse {
		1," o, ""a"" ,b ", 3
		2,2
	} , {lappend result $line}
	set result
} {{} {1 { o, "a" ,b } 3} {2 2} {{}}}

test csv_file {basic} {
	set f [open test.csv] 
	set result [csv_file $f \t]
	close $f
	set result
} {{a 1} {b 2} {{C c} 3}}

test csv_file {complex} {
	set f [open test2.csv] 
	set result [csv_file $f ,]
	close $f
	set result
} {{John Doe {100 some st.} somewhere B 2980} {John {Doe "Who"} {100 some st.} somewhere B 2980} {John {Doe "Who"} {100 some st.} {somewhere,
somehow} B 2980}}

test csv_write {complex} {
	set data {{John Doe {100 some st.} somewhere B 2980} {John {Doe "Who"} {100 some st.} somewhere B 2980} {John {Doe "Who"} {100 some st.} {somewhere,
somehow} B 2980}}
	set f [open test3.csv w]
	csv_write $f $data
	close $f
	file_read test3.csv
} {John,Doe,"100 some st.",somewhere,B,2980
John,"Doe ""Who""","100 some st.",somewhere,B,2980
John,"Doe ""Who""","100 some st.","somewhere,
somehow",B,2980
}

test csv_parse {\n\n in field} {
	set data "a\t\"b\n\n2\"\tc"
	lindex [csv_parse $data \t] 0 1
} "b\n\n2"

test csv_parse {\n\n\n in field} {
	set data "a\t\"b\n\n2\"\tc"
	lindex [csv_parse $data \t] 0 1
} "b\n\n2"

testsummarize
