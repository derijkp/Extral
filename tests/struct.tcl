#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test struct {set} {
	set pointer [struct new]
	struct set $pointer->a 1
	struct set $pointer->a
} {1}

test struct set {
	set pointer [struct new]
	struct set $pointer->b(try) 1
	struct set $pointer->b(try)
} {1}

test struct {array} {
	set pointer [struct new]
	struct set $pointer->b(try) 1
	struct array set $pointer->b {ok yes new 2}
	struct array get $pointer->b
} {new 2 ok yes try 1}

test struct {fields} {
	set pointer [struct new]
	struct set $pointer->a 1
	struct set $pointer->b(try) 1
	struct fields $pointer
} {a b}

test struct {unset element} {
	struct clearall
	set pointer [struct new]
	struct set $pointer->b(try) 1
	struct set $pointer->a 1
	struct unset $pointer->a
	struct set $pointer->a
} {can't read "Struct1->a": no such variable} 1

test struct {unset element, check other} {
	set pointer [struct new]
	struct set $pointer->a 1
	struct set $pointer->b 2
	struct unset $pointer->a
	struct set $pointer->b
} {2}

test struct {unset struct} {
	struct clearall
	set pointer [struct new]
	struct set $pointer->a 1
	struct set $pointer->b 2
	struct unset $pointer
	struct set $pointer->a
} {can't read "Struct1->a": no such variable} 1

testsummarize
