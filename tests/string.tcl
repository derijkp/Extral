#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test string::change {basic} {
	string::change "abc%Wefg" {%% % %W $w}
} {abc$wefg}

test string::change {basic} {
	string::change "%W %%%W" {%% % %W $w}
} {$w %$w}

test string::change {basic 2} {
	string::change {tkButtonEnter %W} {%W {[::class::bind %W]}}
} {tkButtonEnter [::class::bind %W]}

test string::change {basic 3} {
	string::change {tkButtonEnter [::class::bind %W]} {{[::class::bind %W]} %W}
} {tkButtonEnter %W}

test string::change {basic 4} {
	string::change .try.dedit.work [list .try.dedit.work {$window}]
} {$window}

test string::change {close} {
	string::change abc {a 1 b 2 c 3}
} {123}

test string::change {close, order!} {
	string::change abc {c 3 a 1 b 2}
} {123}

test string::change {close, order!} {
	string::change abc {b 2 a 1 c 3}
} {123}

test string::change {close} {
	string::change cga {a t u a t a g c c g}
} {gct}

test string::reverse {basic} {
	string::reverse {abc def}
} {fed cba}

test string::find {exact} {
	string::find {abc adef} a
} {0 4}

test string::find {exact} {
	string::find -exact {abc adef} a
} {0 4}

test string::find {glob} {
	string::find -glob {abc adef} {a*}
} {0 4}

test string::find {regexp} {
	string::find -regexp {abc adef} {^[ab]}
} {0 1 4}

test string::replace {replace} {
	string::replace "abcdefgh" 2 3 23
} ab23efgh

test string::replace {replace 1} {
	string::replace "abcdefgh" 2 2 23
} ab23defgh

test string::replace {replace by none} {
	string::replace "abcdefgh" 2 3 {}
} abefgh

test string::replace {replace after end} {
	string::replace "abcde" 10 10 23
} {abcde    23}

test string::replace {replace over end} {
	string::replace "abcde" 4 5 23
} {abcd23}

test string::replace {insert} {
	string::replace "abcdefgh" 2 1 23
} ab23cdefgh

testsummarize
