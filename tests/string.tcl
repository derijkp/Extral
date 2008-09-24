#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

set changelist {%% % %W $w}
set string "abc%Wefg"
set pos 3
time {
	string_change "abc%Wefg" {%% % %W $w}
}

test string_change {basic} {
	string_change "abc%Wefg" {%% % %W $w}
} {abc$wefg}

test string_change {longer} {
	string range [string_change [string_fill abcdefghijklmnopqrstuvwxyz 100] {b 2 d 4}] 0 25
} {a2c4efghijklmnopqrstuvwxyz}

test string_change {basic} {
	string_change "%W %%%W" {%% % %W $w}
} {$w %$w}

test string_change {basic 2} {
	string_change {tkButtonEnter %W} {%W {[::class::bind %W]}}
} {tkButtonEnter [::class::bind %W]}

test string_change {basic 3} {
	string_change {tkButtonEnter [::class::bind %W]} {{[::class::bind %W]} %W}
} {tkButtonEnter %W}

test string_change {basic 4} {
	string_change .try.dedit.work [list .try.dedit.work {$window}]
} {$window}

test string_change {close} {
	string_change abc {a 1 b 2 c 3}
} {123}

test string_change {close, order!} {
	string_change abc {c 3 a 1 b 2}
} {123}

test string_change {close, order!} {
	string_change abc {b 2 a 1 c 3}
} {123}

test string_change {close} {
	string_change cga {a t u a t a g c c g}
} {gct}

test string_change {zero length} {
	string_change {} {a t u a t a g c c g}
} {}

test string_change {in index, but not in list} {
string_change {tkEntryButton1 %W %x} {{[::Classy::rebind %W]} %W %W %O}
} {tkEntryButton1 %O %x}

test string_change {bugfix} {
set value {
SPTREMBL; Q37382; Q37382.
SWISS-PROT; P46753; RT02_ACACA.
SWISS-PROT; P46754; RT03_ACACA.
}
string_change $value "\; {} \n {}"
} {changelist does not have an even number of elements} 1

test string_reverse {basic} {
	string_reverse {abc def}
} {fed cba}

test string_reverse {longer} {
	string range [string_reverse [string_fill abcdefghijklmnopqrstuvwxyz 100]] 0 25
} {zyxwvutsrqponmlkjihgfedcba}

test string_find {exact} {
	string_find {abc adef} a
} {0 4}

test string_find {exact} {
	string_find -exact {abc adef} a
} {0 4}

test string_find {glob} {
	string_find -glob {abc adef} {a*}
} {0 4}

test string_find {regexp} {
	string_find -regexp {abc adef} {^[ab]}
} {0 1 4}

test string_replace {replace} {
	string_replace "abcdefgh" 2 3 23
} ab23efgh

test string_replace {replace 1} {
	string_replace "abcdefgh" 2 2 23
} ab23defgh

test string_replace {replace by none} {
	string_replace "abcdefgh" 2 3 {}
} abefgh

test string_replace {replace after end} {
	string_replace "abcde" 10 10 23
} {abcde     23}

test string_replace {replace over end} {
	string_replace "abcde" 4 5 23
} {abcd23}

test string_replace {insert} {
	string_replace "abcdefgh" 2 1 23
} ab23cdefgh

test string_replace {negative index} {
	string_replace "abcdefgh" -10 -10 23
} {first position < 0} 1

test string_replace {insert} {
	string_replace ABCDEFG 5 -1 ----
} ABCDE----FG

test string_replace {insert before} {
	string_replace ABCDEFG 0 -1 ----
} ----ABCDEFG

test string_replace {insert after} {
	string_replace ABCDEFG 10 -1 ----
} {ABCDEFG   ----}

test string_fill {basic} {
	string_fill ab 5
} ababababab

test string_fill {error} {
	string_fill 5 a
} {a is not an integer} 1

test string_fill {< 0} {
	string_fill a -1
} {}

test string_equal {basic} {
	string_equal ab ab
} 1

test string_equal {basic} {
	string_equal ab ac
} 0

test string_equal {basic} {
	string_equal 2.1 2.10
} 0

test string_foreach {basic} {
	set temp {}
	string_foreach a 12345 {lappend temp $a}
	set temp
} {1 2 3 4 5}

test string_foreach {two vars} {
	set temp {}
	string_foreach {a b} 12345 {lappend temp [list $a $b]}
	set temp
} {{1 2} {3 4} {5 {}}}

test string_foreach {two strings, one two vars} {
	set temp {}
	string_foreach {a b} 12345 c abcde {lappend temp [list $c $a $b]}
	set temp
} {{a 1 2} {b 3 4} {c 5 {}} {d {} {}} {e {} {}}}

test string_foreach {mix} {
	set temp {}
	string_foreach a ab {
		string_foreach b 12 {
			lappend temp [list $a $b]
		}
	}
	set temp
} {{a 1} {a 2} {b 1} {b 2}}

test string_sounds {De Rijk} {
	string_sounds "De Rijk"
} drg

test string_sounds {DeReyck} {
	string_sounds "DeReyck"
} drg

test string_sounds {Smith} {
	string_sounds "Smith"
} snd

test string_sounds {jerome} {
	string_sounds "jerome"
} grn

test string_sounds {jan} {
	string_sounds "jan"
} gn

test string_sounds {sam} {
	string_sounds "sam"
} sn

test string_sounds {fil%} {
	string_sounds "fil%"
} bl

test string_sounds {filip} {
	string_sounds "filip"
} blb

testsummarize
