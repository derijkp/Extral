#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test lmanip {subindex} {
	list_subindex {{a 1} {b 2} {c 3}} 1
} {1 2 3}

test lmanip {mangle} {
	list_mangle {a b c} {1 2 3}
} {{a 1} {b 2} {c 3}}

test lmanip {extract} {
	list_extract {Results {A: 50%} {B: 25%} {C: 25%}} { ([0-9+]+)\%}
} {{} 50 25 25}

test lmanip {extract} {
	list_extract {} {}
} {}

test lmanip {split} {
	list_split {a b c d e} -before {1 3}
} {a {b c} {d e}}

test lmanip {join} {
	list_join {a b c {a d} e} { } {0 2}
} {{a b} {c a d} e}

test lmanip {join} {
	list_join {a b c {a d} e} {} {0 2}
} {ab {ca d} e}

test lmanip {join} {
	list_join {a b c {a d} e} {} all
} {abca de}

test lmanip {lengths} {
	list_lengths {abc abcdef}
} {3 6}

test lmanip {fill} {
	list_fill 4 "Hello world"
} {{Hello world} {Hello world} {Hello world} {Hello world}}

test lmanip {fill counting} {
	list_fill 5 2 2
} {2 4 6 8 10}

test lmanip {fill negative counting} {
	list_fill 5 10 -2
} {10 8 6 4 2}

test lmanip {fill} {
	list_fill 3 0.0
} {0.0 0.0 0.0}

test lmanip {ffill negative counting} {
	list_fill 5 10.2 -2
} {10.2 8.2 6.2 4.2 2.2}

test lmanip {ffill counting} {
	list_fill 5 10 0.1
} {10.0 10.1 10.2 10.3 10.4}

testsummarize

