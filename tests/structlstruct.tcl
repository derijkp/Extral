#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test structlset-struct {basic} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try a 10
} {a 10}

test structlset-struct {basic: 2 tags} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try {a a} 10
} {a {a 10}}

test structlset-struct {basic: set to default} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try a ?
} {}

test structlset-struct {basic: set to default: 2 tags} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try {a a} ?
} {}

test structlset-struct {basic: set to default: remove old} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	structlset -struct $struct $try {a a} ?
} {}

test structlset-struct {check empty taglist with set} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try {} {a 1}
} {a 1}

test structlset-struct {basic: set one of 2 to default} {
	set struct {a {a {*int ?} b {*int ?}}}
	set try {a {a 1 b 2}}
	structlset -struct $struct $try {a a} ?
} {a {b 2}}

test structlset-struct {basic: add one} {
	set struct {*named {*int ?} {}}
	set try {a 1}
	structlset -struct $struct $try b 10
} {a 1 b 10}

test structlset-struct {basic: set multi} {
	set struct {*named {*int ?} {}}
	set try {a 1}
	structlset -struct $struct $try b 10 a 2
} {a 2 b 10}

test structlset-struct {basic: add one: 2 tags} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1}}
	structlset -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test structlset-struct {basic: change one} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	structlset -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test structlset-struct {basic: error} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try a 10
} {error: incorrect value trying to assign "10" to struct "a {*int ?}" at field "a"} 1

test structlset-struct {basic: error too much tags} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try {a a a} 10
} {error: field "a" not present in structure "*int ?" at field "a" at field "a"} 1

test structlset-struct {basic: error with *} {
	set struct {a {*named {*int ?} {}}}
	set try {}
	structlset -struct $struct $try a 10
} {error: "10" does not have an even number of elements at field "a"} 1

test structlset-struct {empty -struct: set} {
	set try {}
	structlset -struct {} $try a 10
} {error: tag "a" not present in structure ""} 1

test structlget-struct {empty -struct: get} {
	set try {}
	structlget -struct {} $try a
} {error: tag "a" not present in structure ""} 1

test structlset-struct {basic: set one by value} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try a {a 10}
} {a {a 10}}

test structlset-struct {basic: add one by value} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1}}
	structlset -struct $struct $try a {b 10}
} {a {a 1 b 10}}

test structlset-struct {basic: named remove default} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1}}
	Extral::structlset -struct $struct $try {a a} ?
} {}

test structlset-struct {basic: named remove default} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1}}
	Extral::structlset -struct $struct $try a {a ?}
} {}

test structlset-struct {basic: change one by value} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	structlset -struct $struct $try a {b 10}
} {a {a 1 b 10}}

test structlset-struct {basic: change one} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	structlset -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test structlset-struct {basic: set empty} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	structlset -struct $struct $try a {}
} {}

# get tests

test structlget-struct {check empty taglist with get} {
	set struct {a {*int ?}}
	set try {}
	structlget -struct $struct $try {}
} {a ?}

test structlget-struct {basic: get value} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	structlget -struct $struct $try {a a}
} {10}

test structlget-struct {basic: get substructure} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	structlget -struct $struct $try a
} {a 10}

test structlget-struct {basic: get substructure: mixed with default} {
	set struct {a {a {*int ?} b {*int ?}}}
	set try {a {a 10}}
	structlget -struct $struct $try a
} {a 10 b ?}

test structlget-struct {basic: get multi: substructure: mixed with default} {
	set struct {a {a {*int ?} b {*int ?}}}
	set try {a {a 10}}
	structlget -struct $struct $try a {a b}
} {{a 10 b ?} ?}

test structlget-struct {basic: default} {
	set struct {a {a {*int ?}}}
	set try {}
	structlget -struct $struct $try {a a}
} {?}

test structlget-struct {basic: default struct} {
	set struct {a {a {*int ?}}}
	set try {}
	structlget -struct $struct $try a
} {a ?}

test structlset-struct {by value} {
	set struct {i {*int ?} s {*any ?}}
	structlset -struct $struct {} {} {i 2 s try}
} {i 2 s try}

test structlget-struct {check for error: struct not even} {
	set struct accn
	Extral::structlget -struct $struct {} {}
} {error: structure "accn" does not have an even number of elements} 1

test structlget-struct {check for error} {
	set struct {accn {*a} {*a} {}}
	Extral::structlget -struct $struct {} {}
} {accn *a *a {}}

test structlset-struct {named by field} {
	set struct {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10} b {}}
	structlset -struct $struct $try {a s} 1
} {a {s 1 n 10} b {}}

test structlset-struct {named by field and value} {
	set struct {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10}}
	structlset -struct $struct $try a {s a n 1}
} {a {s a n 1}}

test structlset-struct {named no name: not even} {
	set struct {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10}}
	structlset -struct $struct $try {} try
} {error: "try" does not have an even number of elements} 1

test structlset-struct {named no name} {
	set struct {*named {a {*int ?}} {}}
	set try {}
	structlset -struct $struct $try {} {try 1}
} {error: incorrect value trying to assign "1" to struct "a {*int ?}" in named "try"} 1

test structlset-struct {named no name} {
	set struct {*named {a {*int ?}} {}}
	set try {}
	structlset -struct $struct $try {} {try {a a}}
} {expected integer but got "a" at field "a" in named "try"} 1

test structlget-struct {named by value} {
	set struct {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10} b {}}
	structlget -struct $struct $try {}
} {a {s {Some Street} n 10} b {s ? n ?}}

test structlget-struct {named by field} {
	set struct {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10} b {}}
	structlget -struct $struct $try {a s}
} {Some Street}

#test structlget-struct {long format} {
#source ../lib/structl.tcl
#source ../lib/structltypes.tcl
#	set struct {
#		{Address a} {
#			{Street s} {*any ?}
#			{Number n} {*int ?}
#		}
#		* {*int ?}
#	}
#	set try {a {s {Some Street} n 10}}
#	structlget -struct $struct $try {}
#} {a ?}


testsummarize
