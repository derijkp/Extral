#!/usr/local/bin/tclsh8.0
source tools.tcl

test leval-1.1 {basic tests} {leval list a b c} {a b c}
test leval-1.2 {basic tests} {leval list {a b c}} {a b c}
test leval-1.3 {basic tests} {leval list {$a} {$b} {$c}} {{$a} {$b} {$c}}
test leval-1.3 {basic tests} {leval list {$a $b $c}} {{$a} {$b} {$c}}
test leval-1.4 {basic tests} {leval list {[list $a $b $c]}} {{[list} {$a} {$b} {$c]}}

rename unknown old-unknown
proc unknown {args} {return $args}

test leval-1.5 {unknown invocation} {
	leval {xyz $a} {1 2 3} b
} {xyz {$a} 1 2 3 b}

rename unknown {}
rename old-unknown unknown

test leval-1.6 {unknown invocation} {
	set code [catch {leval {xyz a} {1 2 3} b} msg]
	list $code $msg
} {1 {invalid command name "xyz"}}
test leval-1.7 {embedded NULL} {
	set code [catch {leval "abc\0def" 1 2 3} msg]
	list $code $msg
} {1 {embedded NULL in command name after "abc"}}


testsummarize

