#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test taglset {set} {
	taglset {a 1 bb 2 ccc 3} bb try
} {a 1 bb try ccc 3}

test taglset {set, 2 tags} {
	taglset {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c a} try
} {a 1 b 2 c {a try b 2 c {a 1 b 2}}}

test taglset {set new} {
	taglset {a 1 bb 2 ccc 3} dddd 4
} {a 1 bb 2 ccc 3 dddd 4}

test taglset {set new, 2 tags} {
	taglset {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c d} try
} {a 1 b 2 c {a 1 b 2 c {a 1 b 2} d try}}

test taglset {set, 3 tags} {
	taglset {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c c a} try
} {a 1 b 2 c {a 1 b 2 c {a try b 2}}}

test taglset {check uneven} {
	taglset {a 1 bb 2 ccc} dddd 4
} {error: "a 1 bb 2 ccc" does not have an even number of elements} 1

test taglset {check uneven, 2 tags} {
	taglset {a 1 b 2 c {a 1 b}} {c a} 4
} {error: "a 1 b" does not have an even number of elements} 1

test taglset {empty list} {
	taglset {} a 1
} {a 1}

test taglset {check for object errors in C code} {
	set try {a 1 bb 2 ccc 3}
	taglset $try bb try
	set try 
} {a 1 bb 2 ccc 3}

test taglget {} {
	taglget {a 1 bb 2 ccc 3} bb
} {2}

test taglget {with structure} {
	taglget {a 1 bb {a bb1 b bb2} ccc 3} {bb b}
} {bb2}

test taglget {with larger structure} {
	taglget {a 1 b {a {a baa b bab} b try}} {b a b}
} {bab}

test taglget {tag not present} {
	taglget {a 1 bb 2 ccc 3} e
} {taglist "e" not found} 1

test taglget {taglist larger than structure} {
	taglget {a 1 bb 2 ccc 3} {a b}
} {error: list "1" does not have an even number of elements} 1

test taglget {taglist in struct not present} {
	taglget {a {a 1 b 2} bb 2 ccc 3} {a c}
} {taglist "a c" not found} 1

test taglget {get partial} {
	taglget {a {a 1 b 2} bb 2 ccc 3} a
} {a 1 b 2}

test taglget {check uneven} {
	taglget {a 1 bb 2 ccc} bb
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test taglunset {} {
	taglunset {a 1 bb 2 ccc 3} bb
} {a 1 ccc 3}

test taglunset {} {
	taglunset {a 1 bb 2 ccc 3} d
} {a 1 bb 2 ccc 3}

test taglunset {check uneven} {
	taglunset {a 1 bb 2 ccc} bb
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test taglunset {unset, 2 tags} {
	taglunset {a 1 b 2 c {a 1 b 2}} {c c}
} {a 1 b 2 c {a 1 b 2}}

test taglunset {unset, 3 tags} {
	taglunset {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c c a}
} {a 1 b 2 c {a 1 b 2 c {b 2}}}

test taglfields {} {
	taglfields {a 1 bb 2 ccc 3}
} {a bb ccc}

test taglfields {see values} {
	taglfields {a 1 bb 2 ccc 3} values
	set values
} {1 2 3}

test taglunset {check uneven} {
	taglfields {a 1 bb 2 ccc}
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test taglfind {present} {
	taglfind {a try1 bb try2 ccc try3} bb
} 3

test taglfind {not present} {
	taglfind {a try1 bb try2 ccc try3} try
} 0


testsummarize

# no test yet for
# ffind <switches> filelist pattern ?varName? ?pattern? ?varname?
# ffind -matches -allfiles <switches> filelist pattern nulvalue ?varName? ?pattern? ?nulvalue? ?varname? ..
# lload <filename>
# lwrite ?file? ?list?
