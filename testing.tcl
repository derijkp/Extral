#!/usr/local/bin/tclsh8.0
package require Extral

catch {unset errors}
proc test {command expected {causeerror 0}} {
	global errors
	
	puts "testing:\n$command"
	set error [catch {uplevel $command} result]
	if $causeerror {
		if !$error {
			puts "test should cause error"
			lappend errors $test "test should cause error"
		}	
	} else {
		if $error {
			puts "test should not cause error"
			lappend errors $test "test should not cause error"
		}
	}
	if {"$result"!="$expected"} {
		puts "error: result is:\n$result\nshould be\n$expected"
		lappend errors $command "error: result is:\n$result\nshould be\n$expected"
	}
}

test \
{lregsub {c$} {afdsg asdc sfgh {dfgh shgfc} dfhg} {!}} \
{afdsg asd! sfgh {dfgh shgf!} dfhg}

test \
{lregsub {^([^.]+)\.([^.]+)$} {start.sh help.ps h.sh} {\2 \1}} \
{{sh start} {ps help} {sh h}}

test \
{lfind -regexp {Ape Ball Field {Antwerp city} Egg} {^A}} \
{0 3}

test \
{lsub {Ape Ball Field {Antwerp city} Egg} {0 3}} \
{Ape {Antwerp city}}

test \
{lsub {Ape Ball Field {Antwerp city} Egg} -exclude {0 3}} \
{Ball Field Egg}

test \
{lsub {Ape Ball Field Egg} -exclude {}} \
{Ape Ball Field Egg}

test \
{lsub {Ape Ball Field} {}} \
{}

test \
{lsub {Ape Ball Field} {1 -1 100}} \
{Ball}

test \
{lcor {a b c d e f} {d b}} \
{3 1}

test \
{lcor {a b c d e f} {b d d}} \
{1 3 -1}

test \
{lmath calc {1 2 3.2 4} + {1 2 3.3 4}} \
{2 4 6.5 8}

test \
{lmath sum {1 4 5}} \
{10}

test \
{lmath min {5 1 100}} \
{1}

test \
{lmath max {5 1 100 50}} \
{100}

test \
{lmath cumul {5 1 100}} \
{5 6 106}

test \
{lmath incr {8 18 100} 2} \
{10 20 102}

test \
{lmanip subindex {{a 1} {b 2} {c 3}} 1} \
{1 2 3}

test \
{lmanip mangle {a b c} {1 2 3}} \
{{a 1} {b 2} {c 3}}

test \
{lmanip extract {Results {A: 50%} {B: 25%} {C: 25%}} { ([0-9+]+)\%}} \
{{} 50 25 25}

test \
{lmanip extract {} {}} \
{}

test \
{lmanip remdup {a b c a b d}} \
{a b c d}

test \
{lmanip split {a b c d e} -before {1 3}} \
{a {b c} {d e}}

test \
{lmanip join {a b c {a d} e} { } {0 2}} \
{{a b} {c a d} e}

test \
{lmanip join {a b c {a d} e} {} {0 2}} \
{ab {ca d} e}

test \
{lmanip join {a b c {a d} e} {} all} \
{abca de}


test \
{lmanip lengths {abc abcdef}} \
{3 6}


test \
{lmanip fill 4 "Hello world"} \
{{Hello world} {Hello world} {Hello world} {Hello world}}
test \
{lmanip fill 5 2 2} \
{2 4 6 8 10}
test \
{lmanip fill 5 10 -2} \
{10 8 6 4 2}

test \
{lmerge {a b c} {1 2 3}} \
{a 1 b 2 c 3}
test \
{lmerge {a b c d} {1 2} 2} \
{a b 1 c d 2}

test \
{lunmerge {a 1 b 2 c 3}} \
{a b c}
test \
{lunmerge {a b 1 c d 2} 2 var} \
{a b c d}
test \
{set var} \
{1 2}

test \
{set try {a b c};lpop try 1} \
{b}
test \
{lpop try} \
{c}
test \
{lpop try} \
{a}
test \
{lpop try} \
{}
test \
{lpop try} \
{}

test \
{set try {a b};lshift try} \
{a}
test \
{lshift try} \
{b}
test \
{lshift try} \
{}
test \
{lshift try} \
{}

test \
{ssort -dict {a10 a9 b2 a11}} \
{a9 a10 a11 b2}
test \
{ssort {a10 a9 b2 a11}} \
{a10 a11 a9 b2}

test \
{lpush try a} \
{a}
test \
{lpush try b} \
{a b}

test \
{lunshift try 1} \
{1 a b}

test \
{set try {a b c d};lset try test {1 3}} \
{a test c test}
 \
test \
{larrayset a {a b c} {1 2 3};array get a} \
{a 1 b 2 c 3}

test \
{lcommon {a b c d} {a d e} {a d f h}} \
{a d}

test \
{lunion {a b c} {c d e}} \
{a b c d e}

test \
{leor {a b c} {c d e}} \
{a b d e}

test \
{set try {a b a c};lremove $try a} \
{b c}

test \
{set try {a b a c};lremove $try a c} \
{b}

test \
{set try {a b a c};llremove $try {a c}} \
{b}

test \
{set try {a b};laddnew try a} \
{a b}
test \
{set try {a b};laddnew try c} \
{a b c}

test \
{set list {a b c};set l $list;lpop list;set l} \
{a b c}
test \
{set list {a b c};set l $list;lshift list;set l} \
{a b c}

test {
	set data {{fdfg sdfg gh} sdfh {dgh sdh}}
	set line [lindex $data 0]
	lshift line
	set line
} \
{sdfg gh}

test {
	set try {a b c d e}
	foreach t $try {
		lshift t
		lshift t
	}
	foreach t $try {
		lshift t
		lshift t
	}
} \
{}

test {
set pointer [struct new]
struct set $pointer->a 1
struct set $pointer->a
} \
{1}

test {
struct set $pointer->b(try) 1
struct set $pointer->b(try)
} \
{1}

test {
struct array set $pointer->b {ok yes new 2}
struct array get $pointer->b
} \
{new 2 ok yes try 1}

test {
struct fields $pointer
} \
{a b}

test {
struct unset $pointer
struct set $pointer->a
} \
{can't read "Struct2->a": no such variable} 1

# Summarize results
# =================
if [info exists errors] {
	set error "***********************\nThere were $error errors in the tests"
	foreach {test err} $errors {
		append error "\nTest-------------:\n$test"
		append error "\n$err"
	}
	error $error
}

puts "Done"
# no test yet for
# ffind <switches> filelist pattern ?varName? ?pattern? ?varname?
# ffind -matches -allfiles <switches> filelist pattern nulvalue ?varName? ?pattern? ?nulvalue? ?varname? ..
# lload <filename>
# lwrite ?file? ?list?
