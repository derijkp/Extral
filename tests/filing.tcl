#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test readfile {basic} {
	set f [open try.txt w]
	puts $f "try it"
	close $f
	readfile try.txt
} {try it
}

test writefile {basic} {
	writefile try.txt "try it"
	set f [open try.txt]
	set c [read $f]
	close $f
	set c
} {try it}

test lwrite-lload {basic} {
	set list {a {b c} {d e}}
	file delete try.txt
	lwrite try.txt $list
	lload try.txt
} {a {b c} {d e}}

test splitcomplete {basic} {
set data {
	proc try {} {
		puts ok
	}
	# test
	$w configure \
		-test try
}
splitcomplete $data
} {{} {	proc try {} {
		puts ok
	}} {	# test} {	$w configure  -test try} {}}

test parsecommand {basic} {
	parsecommand {set test [format "%2.2f" 4.3]}
} {set test {[format "%2.2f" 4.3]}}

test parsecommand {difficult} {
set line "\t \$window.try configure \t -title \[puts \"\[tk appname\]\"\]
\\\n-command \"\$window command\\\"\\n\\\"\" -test \{try \nit \"\"\}"
parsecommand $line
} {{$window.try} configure -title {[puts "[tk appname]"]} -command {"$window command\"\n\""} -test {{try 
it ""}}}

testsummarize
