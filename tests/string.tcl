#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test replace {basic} {
	replace "abc%Wefg" {%% % %W $w}
} {abc$wefg}

test replace {basic} {
	replace "%W %%%W" {%% % %W $w}
} {$w %$w}

test replace {basic 2} {
	replace {tkButtonEnter %W} {%W {[::class::bind %W]}}
} {tkButtonEnter [::class::bind %W]}

test replace {basic 3} {
	replace {tkButtonEnter [::class::bind %W]} {{[::class::bind %W]} %W}
} {tkButtonEnter %W}

test replace {basic 4} {
	replace .try.dedit.work [list .try.dedit.work {$window}]
} {$window}

test replace {close} {
	replace abc {a 1 b 2 c 3}
} {123}

test replace {close, order!} {
	replace abc {c 3 a 1 b 2}
} {123}

test replace {close, order!} {
	replace abc {b 2 a 1 c 3}
} {123}

test replace {close} {
	replace cga {a t u a t a g c c g}
} {gct}

test sreverse {basic} {
	sreverse {abc def}
} {fed cba}

test sfind {exact} {
	sfind {abc adef} a
} {0 4}

test sfind {exact} {
	sfind -exact {abc adef} a
} {0 4}

test sfind {glob} {
	sfind -glob {abc adef} {a*}
} {0 4}

test sfind {regexp} {
	sfind -regexp {abc adef} {^[ab]}
} {0 1 4}

testsummarize
