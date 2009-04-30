#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test varsubst {} {
set try {try it}
varsubst try {
puts [list $try $try2]
}
} {
puts [list {try it} $try2]
}

test varsubst {} {
set try {try it}
varsubst try {
puts [list $try $try2]
} {{try it now}}
} {
puts [list {try it now} $try2]
}

test ? {basic true} {
	set try 1
	? {$try<10} under over
} {under}

test ? {basic false} {
	set try 20
	? {$try<10} under over
} {over}

test invoke {basic} {
	invoke {a} {list $a $args} 1 2 3
} {1 {2 3}}

test aproc {basic} {
	[aproc {a args} {list $a $args}] 1 2 3
} {1 {2 3}}

test aproc {error} {
	[aproc {a} {list $a $args}] 1 2 3
} {wrong # args: should be "::Extral::aproc2 a"} 1

test aproc {more} {
	[aproc {a args} {list $a $args}] 1 2 3
	[aproc {a args} {concat $a $args}] 1 2 3 4
	[aproc {a args} {list $a $args}] 1 2 3
} {1 {2 3}}

test aproc {get} {
	set a 1
	list [get a ?] [get b ?]
} {1 ?}

test aproc {get variable with space} {
	set "a a" 1
	list [get "a a" ?] [get "b b" ?]
} {1 ?}

test Extral::event {basic test} {
	set ::a {}
	Extral::event debug puts
	Extral::event clear
	Extral::event listen peter testevent "lappend ::a peter"
	Extral::event generate testevent a 1
	lappend ::a -
	Extral::event listen other testevent "lappend ::a other"
	Extral::event generate testevent b 2
	lappend ::a -
	Extral::event remove peter testevent
	Extral::event generate testevent c 3
	set a
} {peter a 1 - peter b 2 other b 2 - other c 3}

test Extral::event {Extral::bgexec} {
	Extral::bgexec ./testcmd_bgexec.tcl
} {1
2
3
}

test Extral::event {Extral::bgexec parameters} {
	Extral::bgexec ./testcmd_bgexec.tcl 2
} {1
2
}

test Extral::event {Extral::bgexec error} {
	Extral::bgexec ./testcmd_bgexec.tcl bla
} {arg must be an integer
} 1

test Extral::event {Extral::bgexec -timeout} {
	Extral::bgexec -timeout 500 ./testcmd_bgexec.tcl
} {1
}

test Extral::event {Extral::bgexec -pidvar and kill process} {
	after 500 {exec kill $::pid}
	Extral::bgexec -pidvar pid ./testcmd_bgexec.tcl
} {1
}

test Extral::event {Extral::bgexec -progresscommand} {
	set ::r {}
	Extral::bgexec -progresscommand {lappend r} ./testcmd_bgexec.tcl 2
	set ::r
} {1 2}

test Extral::event {Extral::bgexec error in -progresscommand} {
	unset -nocomplain ::r
	set ::r() {}
	Extral::bgexec -progresscommand {lappend r} ./testcmd_bgexec.tcl 2
	lindex [split $::errorInfo \n] 0
} {can't set "r": variable is array}

test Extral::event {Extral::bgexec -command} {
	unset -nocomplain ::v
	Extral::bgexec -command {set v} ./testcmd_bgexec.tcl 2
	vwait ::v
	set ::v
} {1
2
}

test Extral::event {Extral::bgexec -command 2 together} {
	unset -nocomplain ::v
	unset -nocomplain ::w
	Extral::bgexec -command {set v} ./testcmd_bgexec.tcl 2
	Extral::bgexec -command {set w} ./testcmd_bgexec.tcl 3
	vwait ::w
	list [split $::v \n] [split $::w \n]
} {{1 2 {}} {1 2 3 {}}}

if 0 {
	Extral::bgexec lpstat -s
	Extral::bgexec -timeout 500 lpstat -s
	set args {-timeout 500 lpstat -s}
}

testsummarize
