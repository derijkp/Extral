#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test structlset {set} {
	structlset {a 1 bb 2 ccc 3} bb try
} {a 1 bb try ccc 3}

test structlset {set, list check} {
	structlset {a 1 bb 2 ccc 3} bb {try it}
} {a 1 bb {try it} ccc 3}

test structlset {set, 2 tags} {
	structlset {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c a} try
} {a 1 b 2 c {a try b 2 c {a 1 b 2}}}

test structlset {set new} {
	structlset {a 1 bb 2 ccc 3} dddd 4
} {a 1 bb 2 ccc 3 dddd 4}

test structlset {set new, list value} {
	structlset {a 1 bb 2 ccc 3} dddd {try it}
} {a 1 bb 2 ccc 3 dddd {try it}}

test structlset {set new, list tag, list value} {
	structlset {a 1 bb 2 ccc 3} {{d d}} {try it}
} {a 1 bb 2 ccc 3 {d d} {try it}}

test structlset {set new, 2 tags} {
	structlset {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c d} try
} {a 1 b 2 c {a 1 b 2 c {a 1 b 2} d try}}

test structlset {set, 3 tags} {
	structlset {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c c a} try
} {a 1 b 2 c {a 1 b 2 c {a try b 2}}}

test structlset {set multi} {
	structlset {a 1} b 2 a a
} {a a b 2}

test structlset {set multi multiple levels} {
	structlset {a 1} {b a} 2 a a {b b} 3
} {a a b {a 2 b 3}}

test structlset {check uneven} {
	structlset {a 1 bb 2 ccc} dddd 4
} {error: "a 1 bb 2 ccc" does not have an even number of elements} 1

test structlset {check uneven, 2 tags} {
	structlset {a 1 b 2 c {a 1 b}} {c a} 4
} {error: "a 1 b" does not have an even number of elements} 1

test structlset {empty list} {
	structlset {} a 1
} {a 1}

test structlset {empty field} {
	structlset {} {} {a 1}
} {a 1}

test structlset {empty sublist} {
	structlset {a {}} {a b} 1
} {a {b 1}}

test structlset {check for object errors in C code} {
	set try {a 1 bb 2 ccc 3}
	structlset $try bb try
	set try 
} {a 1 bb 2 ccc 3}

test structlset {multiple tags not found} {
	structlset {a 1 bb 2} {c a b} 1
} {a 1 bb 2 c {a {b 1}}}

test structlget {} {
	structlget {a 1 bb 2 ccc 3} bb
} {2}

test structlget {with structure} {
	structlget {a 1 bb {a bb1 b bb2} ccc 3} {bb b}
} {bb2}

test structlget {with larger structure} {
	structlget {a 1 b {a {a baa b bab} b try}} {b a b}
} {bab}

test structlget {tag not present} {
	structlget {a 1 bb 2 ccc 3} e
} {tag "e" not found} 1

test structlget {field larger than structure} {
	structlget {a 1 bb 2 ccc 3} {a b}
} {error: list "1" does not have an even number of elements} 1

test structlget {field in struct not present} {
	structlget {a {a 1 b 2} bb 2 ccc 3} {a c}
} {tag "c" not found} 1

test structlget {get partial} {
	structlget {a {a 1 b 2} bb 2 ccc 3} a
} {a 1 b 2}

test structlget {check uneven} {
	structlget {a 1 bb 2 ccc} bb
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test structlget {def} {
	structlget {a 1 b 2} b a
} {2 1}

test structlget {not found} {
	structlget {a 1} b
} {tag "b" not found} 1

test structlunset {} {
	structlunset {a 1 bb 2 ccc 3} bb
} {a 1 ccc 3}

test structlunset {not present} {
	structlunset {a 1 bb 2 ccc 3} d
} {a 1 bb 2 ccc 3}

test structlunset {check uneven} {
	structlunset {a 1 bb 2 ccc} bb
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test structlunset {present, 2 tags} {
	structlunset {a 1 b 2 c {a 1 b 2}} {c a}
} {a 1 b 2 c {b 2}}

test structlunset {not present, 2 tags} {
	structlunset {a 1 b 2 c {a 1 b 2}} {c c}
} {a 1 b 2 c {a 1 b 2}}

test structlunset {unset, 3 tags} {
	structlunset {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c c a}
} {a 1 b 2 c {a 1 b 2 c {b 2}}}

test structlfields {} {
	structlfields {a 1 bb 2 ccc 3}
} {a bb ccc}

test structlfields {see values} {
	structlfields {a 1 bb 2 ccc 3} {} values
	set values
} {1 2 3}

test structlfields {see values} {
	structlfields {a {a 1 b 2 c 3}} a
} {a b c}

test structlfields {check uneven} {
	structlfields {a 1 bb 2 ccc}
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test structlfind {present} {
	structlfind {a try1 bb try2 ccc try3} bb
} 3

test structlfind {not present} {
	structlfind {a try1 bb try2 ccc try3} try
} -1


testsummarize

# no test yet for
# ffind <switches> filelist pattern ?varName? ?pattern? ?varname?
# ffind -matches -allfiles <switches> filelist pattern nulvalue ?varName? ?pattern? ?nulvalue? ?varname? ..
# lload <filename>
# lwrite ?file? ?list?
