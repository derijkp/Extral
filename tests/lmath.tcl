#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test lmath {calc} {
	lmath_calc {1 2.2 3.2 4} + {1 2 3.3 4.1}
} {2 4.2 6.5 8.1}

test lmath {calc} {
	lmath_calc {1 2 3.2 4} + 1
} {2 3 4.2 5}

test lmath {calc} {
	lmath_calc 10 - {1 2.0}
} {9 8.0}

test lmath {calc different lengths} {
	lmath_calc {1.0 2.0} + {1.0 2.0 3.0}
} {2.0 4.0}

test lmath {sum} {
	expr {round([lmath_sum {1 4 5}])}
} {10}

test lmath {sum doubles} {
	lmath_sum {1.2 4 5.2}
} {10.4}

test lmath {min} {
	lmath_min {5 1 100}
} {1}

test lmath {min bugfix} {
	lmath_min {{} 5 100 50}
} {expected floating-point number but got ""} 1

test lmath {max} {
	lmath_max {5 1 100 50}
} {100}

test lmath {max bugfix} {
	lmath_max {{} 5 100 50}
} {expected floating-point number but got ""} 1

test lmath {cumul} {
	lmath_cumul {5 1 100}
} {5 6 106}

test lmath {incr} {
	lmath_incr {8 18 100} 2
} {10 20 102}

test lmath {between} {
	lmath_between {-1 4 9 11 8} 0 10
} {0 4 9 10 8}

test lmath {average} {
	lmath_average {10 8 10 8}
} 9.0

test lmath {average} {
	lmath_average {9.8 9.9 10 10.1 10.2}
} 10.0

testsummarize


