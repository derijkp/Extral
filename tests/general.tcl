#!/usr/local/bin/tclsh8.0
source tools.tcl

test lregsub {c$} {
	lregsub {c$} {afdsg asdc sfgh {dfgh shgfc} dfhg} {!}
} {afdsg asd! sfgh {dfgh shgf!} dfhg}

test lregsub {^([^.]+)\.([^.]+)$} {
	lregsub {^([^.]+)\.([^.]+)$} {start.sh help.ps h.sh} {\2 \1}
} {{sh start} {ps help} {sh h}}

test lfind {} {
	lfind -regexp {Ape Ball Field {Antwerp city} Egg} {^A}
} {0 3}

test lsub {} {
	lsub {Ape Ball Field {Antwerp city} Egg} {0 3}
} {Ape {Antwerp city}}

test lsub {exclude} {
	lsub {Ape Ball Field {Antwerp city} Egg} -exclude {0 3}
} {Ball Field Egg}

test lsub {exclude {}} {
	lsub {Ape Ball Field Egg} -exclude {}
} {Ape Ball Field Egg}

test lsub {lsub with {}} {
	lsub {Ape Ball Field} {}
} {}

test lsub {negative index} {
	lsub {Ape Ball Field} {1 -1 100}
} {Ball}

test lcor {} {
	lcor {a b c d e f} {d b}
} {3 1}

test lcor {2 times} {
	lcor {a b c d e f} {b d d}
} {1 3 -1}

test lmath {calc} {
	lmath calc {1 2 3.2 4} + {1 2 3.3 4}
} {2 4 6.5 8}

test lmath {sum} {
	lmath sum {1 4 5}
} {10}

test lmath {min} {
	lmath min {5 1 100}
} {1}

test lmath {max} {
	lmath max {5 1 100 50}
} {100}

test lmath {cumul} {
	lmath cumul {5 1 100}
} {5 6 106}

test lmath {incr} {
	lmath incr {8 18 100} 2
} {10 20 102}

test lmanip {subindex} {
	lmanip subindex {{a 1} {b 2} {c 3}} 1
} {1 2 3}

test lmanip {mangle} {
	lmanip mangle {a b c} {1 2 3}
} {{a 1} {b 2} {c 3}}

test lmanip {extract} {
	lmanip extract {Results {A: 50%} {B: 25%} {C: 25%}} { ([0-9+]+)\%}
} {{} 50 25 25}

test lmanip {extract} {
	lmanip extract {} {}
} {}

test lmanip {remdup} {
	lmanip remdup {a b c a b d}
} {a b c d}

test lmanip {split} {
	lmanip split {a b c d e} -before {1 3}
} {a {b c} {d e}}

test lmanip {join} {
	lmanip join {a b c {a d} e} { } {0 2}
} {{a b} {c a d} e}

test lmanip {join} {
	lmanip join {a b c {a d} e} {} {0 2}
} {ab {ca d} e}

test lmanip {join} {
	lmanip join {a b c {a d} e} {} all
} {abca de}


test lmanip {lengths} {
	lmanip lengths {abc abcdef}
} {3 6}


test lmanip {fill} {
	lmanip fill 4 "Hello world"
} {{Hello world} {Hello world} {Hello world} {Hello world}}

test lmanip {fill counting} {
	lmanip fill 5 2 2
} {2 4 6 8 10}

test lmanip {fill negative counting} {
	lmanip fill 5 10 -2
} {10 8 6 4 2}

test lmerge {} {
	lmerge {a b c} {1 2 3}
} {a 1 b 2 c 3}

test lmerge {size 2} {
	lmerge {a b c d} {1 2} 2
} {a b 1 c d 2}

test lmerge {first too long} {
	lmerge {a b c d e} {1 2}
} {a 1 b 2 c {} d {} e {}}

test lmerge {second too long} {
	lmerge {a b} {1 2 3}
} {a 1 b 2}

test lmerge {second too long, spacing 2} {
	lmerge {a b c d} {1 2 3} 2
} {a b 1 c d 2}

test lmerge {first too long, spacing 2} {
	lmerge {a b c d e f} {1 2} 2
} {a b 1 c d 2 e f {}}

test lunmerge {} {
	lunmerge {a 1 b 2 c 3}
} {a b c}

test lunmerge {size 2} {
	lunmerge {a b 1 c d 2} 2 var
} {a b c d}

test lunmerge {size 2, test var} {
	lunmerge {a b 1 c d 2 e f 3} 2 var
	set var
} {1 2 3}

test lunmerge {size 2, test strange string} {
	lunmerge {a b 1 c d 2 e} 2 var
} {a b c d e}

test lunmerge {size 2, test var with strange string} {
	lunmerge {a b 1 c d 2 e} 2 var
	set var
} {1 2}

test lpop {} {
	set try {a b c}
	lpop try 1
} {b}

test lpop {several times} {
	set try {a b c}
	lpop try 1
	lpop try
	lpop try
} {a}

test lpop {empty} {
	set try {a b c}
	lpop try 1
	lpop try
	lpop try
	lpop try
} {}

test lpop {stays empty} {
	set try {a b c}
	lpop try 1
	lpop try
	lpop try
	lpop try
	lpop try
} {}

test lshift {} {
	set try {a b}
	lshift try
} {a}

test lshift {2} {
	set try {a b}
	lshift try
	lshift try
} {b}

test lshift {empty} {
	set try {a b}
	lshift try
	lshift try
	lshift try
} {}

test lshift {stays empty} {
	set try {a b}
	lshift try
	lshift try
	lshift try
	lshift try
} {}

test ssort {dict} {
	ssort -dict {a10 a9 b2 a11}
} {a9 a10 a11 b2}

test ssort {normal} {
	ssort {a10 a9 b2 a11}
} {a10 a11 a9 b2}

test lpush {} {
	lpush try a
} {a}

test lpush {2} {
	lpush try a
	lpush try b
} {a b}

test lunshift {} {
	lpush try a
	lpush try b
	lunshift try 1
} {1 a b}

test lset {} {
	set try {a b c d}
	lset try test {1 3}
} {a test c test}

test larrayset {} {
	larrayset a {a b c} {1 2 3}
	array get a
} {a 1 b 2 c 3}

test lcommon {} {
	lcommon {a b c d} {a d e} {a d f h}
} {a d}

test lunion {} {
	lunion {a b c} {c d e}
} {a b c d e}

test leor {} {
	leor {a b c} {c d e}
} {a b d e}

test lremove {} {
	set try {a b a c}
	lremove $try a
} {b c}

test lremove {2} {
	set try {a b a c}
	lremove $try a c
} {b}

test llremove {} {
	set try {a b a c}
	llremove $try {a c}
} {b}

test laddnew {exists} {
	set try {a b}
	laddnew try a
} {a b}

test laddnew {new} {
	set try {a b}
	laddnew try c
} {a b c}

test lpop {test duplicate} {
	set list {a b c}
	set l $list
	lpop list
	set l
} {a b c}

test lshift {test duplicate} {
	set list {a b c}
	set l $list
	lshift list
	set l
} {a b c}

test lshift {object tests} {
	set data {{fdfg sdfg gh} sdfh {dgh sdh}}
	set line [lindex $data 0]
	lshift line
	set line
} {sdfg gh}

test lshift {with foreach} {
	set try {a b c d e}
	foreach t $try {
		lshift t
		lshift t
	}
	foreach t $try {
		lshift t
		lshift t
	}
} {}

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
	set pointer [struct new]
	struct set $pointer->b(try) 1
	struct set $pointer->a 1
	struct unset $pointer->a
	struct set $pointer->a
} {can't read "Struct6->a": no such variable} 1

test struct {unset element, check other} {
	set pointer [struct new]
	struct set $pointer->a 1
	struct set $pointer->b 2
	struct unset $pointer->a
	struct set $pointer->b
} {2}

test struct {unset struct} {
	set pointer [struct new]
	struct set $pointer->a 1
	struct set $pointer->b 2
	struct unset $pointer
	struct set $pointer->a
} {can't read "Struct8->a": no such variable} 1

test taglset {} {
	taglset {a 1 bb 2 ccc 3} bb try
} {a 1 bb try ccc 3}

test taglset {new value} {
	taglset {a 1 bb 2 ccc 3} dddd 4
} {a 1 bb 2 ccc 3 dddd 4}

test taglset {check uneven} {
	taglset {a 1 bb 2 ccc} dddd 4
} {tagged list must have an even number of elements} 1

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

test taglget {tag not present} {
	taglget {a 1 bb 2 ccc 3} e
} {tag "e" not found} 1

test taglget {check uneven} {
	taglget {a 1 bb 2 ccc} bb
} {tagged list must have an even number of elements} 1

test taglget {tag not present with default} {
	taglget {a 1 bb 2 cc 3} e def
} {def}

test taglunset {} {
	taglunset {a 1 bb 2 ccc 3} bb
} {a 1 ccc 3}

test taglunset {} {
	taglunset {a 1 bb 2 ccc 3} d
} {a 1 bb 2 ccc 3}

test taglunset {check uneven} {
	taglunset {a 1 bb 2 ccc} bb
} {tagged list must have an even number of elements} 1

test taglfields {} {
	taglfields {a 1 bb 2 ccc 3}
} {a bb ccc}

test taglfields {see values} {
	taglfields {a 1 bb 2 ccc 3} values
	set values
} {1 2 3}

test taglunset {check uneven} {
	taglfields {a 1 bb 2 ccc}
} {tagged list must have an even number of elements} 1


testsummarize

# no test yet for
# ffind <switches> filelist pattern ?varName? ?pattern? ?varname?
# ffind -matches -allfiles <switches> filelist pattern nulvalue ?varName? ?pattern? ?nulvalue? ?varname? ..
# lload <filename>
# lwrite ?file? ?list?
