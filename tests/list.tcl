#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

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

test lsub {problem} {
	set findex {{Acibri.ACR: Acidianus brierleyi} {Aciinf.ACR: Acidianus infernus} {Desmob.ACR: Desulfurococcus mobilis} {Thecel.AEU: Thermococcus celer}}
	set pattern "\\.ACR"
	set poss [lfind -regexp $findex $pattern]
	set result [lsub $findex $poss]
	set rest [lsub $findex -exclude $poss]
} {{Thecel.AEU: Thermococcus celer}}

test lcor {} {
	lcor {a b c d e f} {d b}
} {3 1}

test lcor {2 times} {
	lcor {a b c d e f} {b d d}
} {1 3 -1}

test lcor {number bug} {
	lcor {hobbit.seq orc.seq} {sphinx.seq hobbit.seq orc.seq centaur.seq}
} {-1 0 1 -1}

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

test lmath {between} {
	lmath between {-1 4 9 11 8} 0 10
} {0 4 9 10 8}

test lremdup {} {
	lremdup {a b c a b d}
} {a b c d}

test lremdup {sorted} {
	lremdup -sorted {a a b b c d}
} {a b c d}

test lremdup {check removed} {
	lremdup {a b c a b d b} temp
	set temp
} {a b b}

test lremdup {sorted, check removed} {
	lremdup -sorted {a a b b b c d} temp
	set temp
} {a b b}

test lremdup {large} {
	set list {}
	for {set i 0} {$i < 10000} {incr i} {lappend list "try $i"}
	lappend list "try 100" "try 200"
	llength [lremdup $list]
} 10000

test lremdup {small sorted} {
	set list {}
	for {set i 0} {$i < 10} {incr i} {lappend list "try $i"}
	lappend list "try 1" "try 2"
	set list [lsort $list]
	llength [lremdup -sorted $list]
} 10

test lremdup {large sorted} {
	set list {}
	for {set i 0} {$i < 10000} {incr i} {lappend list "try $i"}
	lappend list "try 100" "try 200"
	set list [lsort $list]
	llength [lremdup -sorted $list]
} 10000

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

test lcommon {} {
	lcommon {hobbit.seq orc.seq} {sphinx.seq hobbit.seq orc.seq centaur.seq}
} {hobbit.seq orc.seq}

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

test llremove {large} {
	set try {}
	set try2 {}
	for {set i 0} {$i < 10000} {incr i} {lappend try "try $i"}
	for {set i 1} {$i < 10000} {incr i} {lappend try2 "try $i"}
	set list [lsort $try]
	llremove $list $try2
} {{try 0}}

test llremove {large sorted} {
	set try {}
	set try2 {}
	for {set i 0} {$i < 10000} {incr i} {lappend try "try $i"}
	for {set i 1} {$i < 10000} {incr i} {lappend try2 "try $i"}
	set list [lsort $try]
	set removelist [lsort $try2]
	llremove -sorted $list $removelist
} {{try 0}}

test llremove {difficult cases} {
	set list {a b bd ab ab ac}
	set removelist {ab b}
	llremove $list $removelist
} {a bd ac}

test llremove {difficult cases, sorted} {
	set list {a ab ab ac b bd}
	set removelist {ab b}
	llremove -sorted $list $removelist
} {a ac bd}

test llremove {difficult cases, check removed} {
	set list {a b bd ab ab ac}
	set removelist {ab b try}
	llremove $list $removelist temp
	set temp
} {b ab ab}

test llremove {difficult cases, sorted, check removed} {
	set list {a ab ab ac b bd}
	set removelist {ab b try}
	llremove -sorted $list $removelist temp
	set temp
} {ab ab b}

test llremove {empty} {
	llremove 1 {}
} {1}

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

test lreverse {basic} {
	lreverse {{a b} c {d e}}
} {{d e} c {a b}}

# no test yet for
# ffind <switches> filelist pattern ?varName? ?pattern? ?varname?
# ffind -matches -allfiles <switches> filelist pattern nulvalue ?varName? ?pattern? ?nulvalue? ?varname? ..
# lload <filename>
# lwrite ?file? ?list?

testsummarize
