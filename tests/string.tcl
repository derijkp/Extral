#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

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

test string_reverse {basic} {
	string_reverse {abc def}
} {fed cba}

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
} {abcde    23}

test string_replace {replace over end} {
	string_replace "abcde" 4 5 23
} {abcd23}

test string_replace {insert} {
	string_replace "abcdefgh" 2 1 23
} ab23cdefgh

testsummarize
