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

{lcor {a b c d e f} {d b}}
{3 1}

{lcor {a b c d e f} {b d d}}
{1 3 -1}

{lmath calc {1 2 3.2 4} + {1 2 3.2 4}}
{2.0 4.0 6.4 8.0}

{lmath sum {1 4 5}}
{10}

{lmath min {5 1 100}}
{1.0}

{lmath max {5 1 100 50}}
{100.0}

{lmath cumul {5 1 100}}
{5.0 6.0 106.0}

{lmath incr {8 18 100} 2}
{10.0 20.0 102.0}
{lmanip subindex {{a 1} {b 2} {c 3}} 1}
{1 2 3}

{lmanip merge {a b c} {1 2 3}}
{{a 1} {b 2} {c 3}}

{lmanip extract {Results {A: 50%} {B: 25%} {C: 25%}} { ([0-9+]+)\%}}
{{} 50 25 25}

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
{lmanip unmangle {1 a b 2 c d} 2 var}
{1 2}
{set var}
{a b c d}

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

}

# Run tests
# =========
foreach {test result} $data {
	puts "testing:$test"
	set real [eval $test]
	if {"$real"!="$result"} {
		error "error: result is:\n$real\nshould be\n$result"
	}
}

puts "Done"
exit
# no test yet for
# ffind <switches> filelist pattern ?varName? ?pattern? ?varname?
# ffind -matches -allfiles <switches> filelist pattern nulvalue ?varName? ?pattern? ?nulvalue? ?varname? ..
# lload <filename>
# lwrite ?file? ?list?
