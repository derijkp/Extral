#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test cmd_split {basic} {
set data {
	proc try {} {
		puts ok
	}
	# test
}
append data	"\t\$w configure \\\n"
append data "\t\t-test try\n"
set t [cmd_split $data]
lindex $t 3
} "\t\$w configure \\\n\t	-test try"

test cmd_parse {basic} {
	cmd_parse {set test [format "%2.2f" 4.3]}
} {set test {[format "%2.2f" 4.3]}}

test cmd_parse {difficult} {
set line "\t \$window.try configure \t -title \[puts \"\[tk appname\]\"\]
\\\n-command \"\$window command\\\"\\n\\\"\" -test \{try \nit \"\"\}"
cmd_parse $line
} {{$window.try} configure -title {[puts "[tk appname]"]} -command {"$window command\"\n\""} -test {{try 
it ""}}}

testsummarize
