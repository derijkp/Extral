#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

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

testsummarize

