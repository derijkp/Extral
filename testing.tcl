#!/usr/local/bin/tclsh8.0
package require Extral

set data {
{lregsub {c$} {afdsg asdc sfgh {dfgh shgfc} dfhg} {!}}
{afdsg asd! sfgh {dfgh shgf!} dfhg}

{lregsub {^([^.]+)\.([^.]+)$} {start.sh help.ps h.sh} {\2 \1}}
{{sh start} {ps help} {sh h}}

{lfind -regexp {Ape Ball Field {Antwerp city} Egg} {^A}}
{0 3}

{lsub {Ape Ball Field {Antwerp city} Egg} {0 3}}
{Ape {Antwerp city}}

{lsub {Ape Ball Field {Antwerp city} Egg} -exclude {0 3}}
{Ball Field Egg}

{lsub {Ape Ball Field Egg} -exclude {}}
{Ape Ball Field Egg}

{lsub {Ape Ball Field} {}}
{}

{lsub {Ape Ball Field} {1 -1 100}}
{Ball}

{lcor {a b c d e f} {d b}}
{3 1}

{lcor {a b c d e f} {b d d}}
{1 3 -1}

{lmath calc {1 2 3.2 4} + {1 2 3.3 4}}
{2 4 6.5 8}

{lmath sum {1 4 5}}
{10}

{lmath min {5 1 100}}
{1}

{lmath max {5 1 100 50}}
{100}

{lmath cumul {5 1 100}}
{5 6 106}

{lmath incr {8 18 100} 2}
{10 20 102}

{lmanip subindex {{a 1} {b 2} {c 3}} 1}
{1 2 3}

{lmanip merge {a b c} {1 2 3}}
{{a 1} {b 2} {c 3}}

{lmanip extract {Results {A: 50%} {B: 25%} {C: 25%}} { ([0-9+]+)\%}}
{{} 50 25 25}

{lmanip extract {} {}}
{}

{lmanip remdup {a b c a b d}}
{a b c d}

{lmanip split {a b c d e} -before {1 3}}
{a {b c} {d e}}

{lmanip join {a b c {a d} e} { } {0 2}}
{{a b} {c a d} e}
{lmanip join {a b c {a d} e} {} {0 2}}
{ab {ca d} e}
{lmanip join {a b c {a d} e} {} all}
{abca de}

{lmanip lengths {abc abcdef}}
{3 6}

{lmanip fill 4 "Hello world"}
{{Hello world} {Hello world} {Hello world} {Hello world}}
{lmanip fill 5 2 2}
{2 4 6 8 10}
{lmanip fill 5 10 -2}
{10 8 6 4 2}

{lmanip mangle {a b c} {1 2 3}}
{a 1 b 2 c 3}
{lmanip mangle {a b c d} {1 2} 2}
{a b 1 c d 2}

{lmanip unmangle {a 1 b 2 c 3}}
{a b c}
{lmanip unmangle {a b 1 c d 2} 2 var}
{a b c d}
{set var}
{1 2}

{set try {a b c};lpop try 1}
{b}
{lpop try}
{c}
{lpop try}
{a}
{lpop try}
{}
{lpop try}
{}

{set try {a b};lshift try}
{a}
{lshift try}
{b}
{lshift try}
{}
{lshift try}
{}

{ssort -dict {a10 a9 b2 a11}}
{a9 a10 a11 b2}
{ssort {a10 a9 b2 a11}}
{a10 a11 a9 b2}

{lpush try a}
{a}
{lpush try b}
{a b}

{lunshift try 1}
{1 a b}

{set try {a b c d};lset try test {1 3}}
{a test c test}

{larrayset a {a b c} {1 2 3};array get a}
{a 1 b 2 c 3}

{lcommon {a b c d} {a d e} {a d f h}}
{a d}

{lunion {a b c} {c d e}}
{a b c d e}

{leor {a b c} {c d e}}
{a b d e}

{set try {a b a c};lremove try a}
{b c}

{set try {a b};laddnew try a}
{a b}
{set try {a b};laddnew try c}
{a b c}

{set list {a b c};set l $list;lpop list;set l}
{a b c}
{set list {a b c};set l $list;lshift list;set l}
{a b c}

{	set data {{fdfg sdfg gh} sdfh {dgh sdh}}
	set line [lindex $data 0]
	lshift line
	set line
}
{sdfg gh}

{	set try {a b c d e}
	foreach t $try {
		lshift t
		lshift t
	}
	foreach t $try {
		lshift t
		lshift t
	}
}
{}
}

# Run tests
# =========
set error 0
foreach {test result} $data {
	puts "testing:$test"
	set real [eval $test]
	if {"$real"!="$result"} {
		incr error
		puts "error: result is:\n$real\nshould be\n$result"
		lappend errors $test "error: result is:\n$real\nshould be\n$result"
	}
}

if $error {
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
