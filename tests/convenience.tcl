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

testsummarize
