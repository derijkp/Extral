#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test varsubst {} {
set try {try it}
varsubst {try} {
puts [list $try $try2]
}
} {
puts [list {try it} $try2]
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
} {called "::Extral::aproc1" with too many arguments}

test aproc {more} {
	[aproc {a args} {list $a $args}] 1 2 3
	[aproc {a args} {concat $a $args}] 1 2 3 4
	[aproc {a args} {list $a $args}] 1 2 3
} {1 {2 3}}

testsummarize
