#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

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

foreach {cmd options vars arg} {test {} {a b} {1 2}} break

test cmd_args {basic} {
	catch {unset a}
	catch {unset b}
	cmd_args test {} {a b} {1 2}
	list $a $b
} {1 2}

test cmd_args {one optional} {
	catch {unset a}
	catch {unset b}
	cmd_args test {} {?a? b} {1 2}
	list $a $b
} {1 2}

test cmd_args {one optional not used} {
	catch {unset a}
	catch {unset b}
	cmd_args test {} {?a? b} {1}
	list [get a ""] $b
} {{} 1}

test cmd_args {optional range} {
	catch {unset a}
	catch {unset b}
	cmd_args test {} {?a b? c} {1 2 3}
	list $a $b $c
} {1 2 3}

test cmd_args {optional range with error} {
	catch {unset a}
	catch {unset b}
	catch {unset c}
	catch {unset d}
	cmd_args test {} {?a b? c d} {1 2 3}
} {wrong # of args: should be "test ?a b? c d"} 1

test cmd_args {multiple optional not used} {
	catch {unset a}
	catch {unset b}
	catch {unset args}
	cmd_args test {} {?a? ?...? b} {1 2 3 4}
	list [get a ""] [get args ""] $b
} {1 {2 3} 4}

test cmd_args {multiple optional not used} {
	catch {unset a}
	catch {unset b}
	catch {unset args}
	cmd_args test {} {?a? ?...? b} {1 2}
	list [get a ""] [get args ""] $b
} {1 {} 2}

test cmd_args {multiple optional none used} {
	catch {unset a}
	catch {unset b}
	catch {unset args}
	cmd_args test {} {?a? ?...? b} {1}
	list [get a ""] [get args ""] $b
} {{} {} 1}

test cmd_args {error} {
	catch {unset a}
	catch {unset b}
	cmd_args test {} {?a? b} {}
} {wrong # of args: should be "test ?a? b"} 1

test cmd_args {error} {
	catch {unset a}
	catch {unset b}
	cmd_args test {} {?a? b} {1 2 3}
} {wrong # of args: should be "test ?a? b"} 1

test cmd_args {options, none given} {
	catch {unset opt}
	catch {unset a}
	catch {unset b}
	cmd_args test {
		-test {any "test value"}
		-b {switch "true or false"}
		-o {{oneof a b c} "a, b or c"}
	} {?a? b} {1}
	list [get opt(-test) ""] [get opt(-b) ""] [get opt(-o) ""] [get a ""] $b
} {{} {} {} {} 1}

test cmd_args {options given} {
	catch {unset opt}
	catch {unset a}
	catch {unset b}
	cmd_args test {
		-test {any "test value"}
		-b {switch "true or false"}
		-o {{oneof a b c} "a, b or c"}
	} {?a? b} {-b -test try -o b 1 2}
	list [get opt(-test) ""] [get opt(-b) ""] [get opt(-o) ""] [get a ""] $b
} {try 1 b 1 2}

test cmd_args {options} {
	catch {unset opt}
	catch {unset a}
	catch {unset b}
	cmd_args test {
		-test {any "test value"}
		-b {switch "true or false"}
		-o {{oneof a b c} "a, b or c"}
	} {?a? b} {-bla bla 1 2}
} {bad option "-bla":
Possible options are:
	-test any "test value"
	-b "true or false"
	-o {oneof a b c} "a, b or c"} 1

test cmd_args {options: wrong value oneof} {
	catch {unset opt}
	catch {unset a}
	catch {unset b}
	cmd_args test {
		-test {any "test value"}
		-b {switch "true or false"}
		-o {{oneof a b c} "a, b or c"}
	} {?a? b} {-o d 1 2}
} {invalid value "d" for option -o: should be one of: a b c} 1

test cmd_args {options: wrong value int} {
	catch {unset opt}
	catch {unset a}
	catch {unset b}
	cmd_args test {
		-test {int "test value"}
	} {?a? b} {-test try 1 2}
} {invalid value "try" for option -test: should be an integer} 1

test cmd_args {options} {
	catch {unset opt}
	catch {unset a}
	catch {unset b}
	cmd_args test {
		-test {int "test value"}
	} {?a? b} {-test try 1 2}
} {invalid value "try" for option -test: should be an integer} 1

test cmd_args {test} {
	cmd_args test {
		-append {switch "append to destination file"}
		-overwrite {switch "overwrite destination file"}
		-log {channel "channel on which to write logs"}
		-ftkey {any "value of ftkey in new features (dcse formats)"}
		-type {any "feature type (dcse formats)"}
	} {db file ?...? resultfile} {-ftkey rRNA -type LSU -log stdout db test.ref test2.ref test.seqdb}
	set args
} test2.ref

test cmd_args {empty vars} {
	cmd_args "test" {
		-text {any "text displayed on node"}
	} {} {}
	set a 1
} 1

testsummarize
