#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl
proc r {} {
	uplevel source ../lib/structl.tcl
	uplevel source ../lib/structltypes.tcl
}

test structlist_set-struct {basic set one} {
	set struct {a {*int ?}}
	set try {}
	structlist_set -struct $struct $try a 10
} {a 10}

test structlist_set-struct {basic: 2 tags} {
	set struct {a {a {*int ?}}}
	set try {}
	structlist_set -struct $struct $try {a a} 10
} {a {a 10}}

test structlist_set-struct {basic: set to default} {
	set struct {a {*int ?}}
	set try {}
	structlist_set -struct $struct $try a ?
} {}

test structlist_set-struct {basic: set to default: 2 tags} {
	set struct {a {a {*int ?}}}
	set try {}
	structlist_set -struct $struct $try {a a} ?
} {}

test structlist_set-struct {basic: set to default: remove old} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	structlist_set -struct $struct $try {a a} ?
} {}

test structlist_set-struct {check empty field with set} {
	set struct {a {*int ?}}
	set try {}
	structlist_set -struct $struct $try {} {a 1}
} {a 1}

test structlist_set-struct {basic: set one of 2 to default} {
	set struct {a {a {*int ?} b {*int ?}}}
	set try {a {a 1 b 2}}
	structlist_set -struct $struct $try {a a} ?
} {a {b 2}}

test structlist_set-struct {named: replace one} {
	set struct {*named {*int ?} {}}
	set try {a 1}
	structlist_set -struct $struct $try a 10
} {a 10}

test structlist_set-struct {named: add one} {
	set struct {*named {*int ?} {}}
	set try {a 1}
	structlist_set -struct $struct $try b 10
} {a 1 b 10}

test structlist_set-struct {basic: set multi} {
	set struct {*named {*int ?} {}}
	set try {a 1}
	structlist_set -struct $struct $try b 10 a 2
} {a 2 b 10}

test structlist_set-struct {basic: add one: 2 tags} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1}}
	structlist_set -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test structlist_set-struct {basic: change one} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	structlist_set -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test structlist_set-struct {basic long format} {
	set struct {{? aa a} {*int ?}}
	set try {}
	structlist_set -struct $struct $try aa 10
} {a 10}

test structlist_set-struct {basic long format error} {
	set struct {? {*int ?}}
	set try {}
	structlist_set -struct $struct $try aa 10
} {error: tag "aa" not present in structure "? {*int ?}"} 1

test structlist_set-struct {basic: error} {
	set struct {a {a {*int ?}}}
	set try {}
	structlist_set -struct $struct $try a 10
} {error: incorrect value trying to assign "10" to struct "a {*int ?}" at field "a"} 1

test structlist_set-struct {basic cmd error} {
	set struct {a {*int ?}}
	set try {}
	structlist_set -struct $struct $try a
} {wrong # args: should be "structlist_set ?-struct schema? ?-data clientdata? list field value ?field value ...?"} 1

test structlist_set-struct {basic: error too much tags} {
	set struct {a {a {*int ?}}}
	set try {}
	structlist_set -struct $struct $try {a a a} 10
} {error: field "a" not present in structure "*int ?" at field "a" at field "a"} 1

test structlist_set-struct {basic: error with *named} {
	set struct {a {*named {*int ?} {}}}
	set try {}
	structlist_set -struct $struct $try a 10
} {error: "10" does not have an even number of elements at field "a"} 1

test structlist_set-struct {empty -struct: set} {
	set try {}
	structlist_set -struct {} $try a 10
} {error: tag "a" not present in structure ""} 1

test structlist_get-struct {empty -struct: get} {
	set try {}
	structlist_get -struct {} $try a
} {error: tag "a" not present in structure ""} 1

test structlist_set-struct {basic: set one by value} {
	set struct {a {a {*int ?}}}
	set try {}
	structlist_set -struct $struct $try a {a 10}
} {a {a 10}}

test structlist_set-struct {basic: add one by value} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1}}
	structlist_set -struct $struct $try a {b 10}
} {a {a 1 b 10}}

test structlist_set-struct {basic: named remove default} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1}}
	structlist_set -struct $struct $try {a a} ?
} {}

test structlist_set-struct {basic: named remove default} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1}}
	structlist_set -struct $struct $try a {a ?}
} {}

test structlist_set-struct {basic: change one by value} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	structlist_set -struct $struct $try a {b 10}
} {a {a 1 b 10}}

test structlist_set-struct {basic: change one} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	structlist_set -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test structlist_set-struct {basic: set empty} {
	set struct {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	structlist_set -struct $struct $try a {}
} {a {a 1 b 1}}

# get tests

test structlist_get-struct {check empty field with get} {
	set struct {a {*int ?}}
	set try {}
	structlist_get -struct $struct $try {}
} {a ?}

test structlist_get-struct {basic: get value} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	structlist_get -struct $struct $try {a a}
} {10}

test structlist_get-struct {basic: get substructure} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	structlist_get -struct $struct $try a
} {a 10}

test structlist_get-struct {basic: get substructure: mixed with default} {
	set struct {a {a {*int ?} b {*int ?}}}
	set try {a {a 10}}
	structlist_get -struct $struct $try a
} {a 10 b ?}

test structlist_get-struct {basic: get multi: substructure: mixed with default} {
	set struct {a {a {*int ?} b {*int ?}}}
	set try {a {a 10}}
	structlist_get -struct $struct $try a {a b}
} {{a 10 b ?} ?}

test structlist_get-struct {basic: default} {
	set struct {a {a {*int ?}}}
	set try {}
	structlist_get -struct $struct $try {a a}
} {?}

test structlist_get-struct {basic: default struct} {
	set struct {a {a {*int ?}}}
	set try {}
	structlist_get -struct $struct $try a
} {a ?}

test structlist_set-struct {by value} {
	set struct {i {*int ?} s {*any ?}}
	structlist_set -struct $struct {} {} {i 2 s try}
} {i 2 s try}

test structlist_get-struct {basic: long format} {
	set struct {{? aa a} {*int ?}}
	set try {a {a 10}}
	structlist_get -struct $struct $try aa
} {a 10}

test structlist_get-struct {basic: long format error} {
	set struct {? {*int ?}}
	set try {a {a 10}}
	structlist_get -struct $struct $try aa
} {error: tag "aa" not present in structure "? {*int ?}"} 1

test structlist_get-struct {check for error: struct not even} {
	set struct accn
	structlist_get -struct $struct {} {}
} {error: structure "accn" does not have an even number of elements} 1

test structlist_get-struct {check for error} {
	set struct {accn {*a} {*a} {}}
	structlist_get -struct $struct {} {}
} {invalid command name "::Extral::geta"} 1

test structlist_set-struct {named by field} {
	set struct {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10} b {}}
	structlist_set -struct $struct $try {a s} 1
} {a {s 1 n 10} b {}}

test structlist_set-struct {named by field and value} {
	set struct {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10}}
	structlist_set -struct $struct $try a {s a n 1}
} {a {s a n 1}}

test structlist_set-struct {named no name: not even} {
	set struct {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10}}
	structlist_set -struct $struct $try {} try
} {error: "try" does not have an even number of elements} 1

test structlist_set-struct {named no name} {
	set struct {*named {a {*int ?}} {}}
	set try {}
	structlist_set -struct $struct $try {} {try 1}
} {error: incorrect value trying to assign "1" to struct "a {*int ?}" in named "try"} 1

test structlist_set-struct {named no name} {
	set struct {*named {a {*int ?}} {}}
	set try {}
	structlist_set -struct $struct $try {} {try {a a}}
} {expected integer but got "a" at field "a" in named "try"} 1

test structlist_get-struct {named by value} {
	set struct {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10} b {}}
	structlist_get -struct $struct $try {}
} {a {s {Some Street} n 10} b {s ? n ?}}

test structlist_get-struct {named by field} {
	set struct {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10} b {}}
	structlist_get -struct $struct $try {a s}
} {Some Street}

test structlist_get-struct {long format} {
	set struct {
		{? Address a} {
			{? Street s} {*any ?}
			{? Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	structlist_get -struct $struct $try {}
} {Address {Street {Some Street} Number 10}}

test structlist_get-struct {long format} {
	set struct {
		{? Address a} {
			{? Street s} {*any ?}
			{? Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	structlist_get -struct $struct $try Address
} {Street {Some Street} Number 10}

test structlist_set-struct {long format: set by field} {
	set struct {
		{? Address a} {
			{? Street s} {*any ?}
			{? Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	structlist_set -struct $struct $try {Address Street} {try it}
} {a {s {try it} n 10}}

test structlist_set-struct {long format: set by value} {
	set struct {
		{? Address a} {
			{? Street s} {*any ?}
			{? Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	structlist_set -struct $struct $try Address {Street {try it} Number 1}
} {a {s {try it} n 1}}

test structlist_set-struct {long format: withnamed} {
	set struct {*named {{? try t} {*int ?}} {}}
	set try {a {t 1}}
	structlist_set -struct $struct $try b {try 2} a {try 4}
} {a {t 4} b {t 2}}

test structlist_fields {see values of long format} {
	structlist_fields {a {{? aa a} 1 b 2 c 3}} a
} {aa b c}

test structlist_fields {error in long format} {
	structlist_fields {a {? 1 b 2 c 3}} a
} {{} b c}

test structlist_unset-struct {basic} {
	set struct {a {*int ?}}
	set try {a 1}
	structlist_unset -struct $struct $try a
} {}

test structlist_unset-struct {basic 2} {
	set struct {a {*int ?} b {*int ?}}
	set try {a 1 b 2}
	structlist_unset -struct $struct $try a
} {b 2}

test structlist_unset-struct {basic 2 and 2 tags} {
	set struct {a {a {*int ?}} b {*int ?}}
	set try {a {a 1} b 2}
	structlist_unset -struct $struct $try {a a}
} {b 2}

test structlist_unset-struct {basic long format} {
	set struct {{? aa a} {a {*int ?}} b {*int ?}}
	set try {a {a 1} b 2}
	structlist_unset -struct $struct $try {aa a}
} {b 2}

test structlist_unset-struct {basic named} {
	set struct {a {*named {*int ?} {}} b {*int ?}}
	set try {a {a 1 b 2} b 2}
	structlist_unset -struct $struct $try a
} {b 2}

test structlist_unset-struct {basic one of named} {
	set struct {a {*named {*int ?} {}} b {*int ?}}
	set try {a {a 1 b 2} b 2}
	structlist_unset -struct $struct $try {a a}
} {a {b 2} b 2}

test structlist_unset-struct {basic one of named subfields} {
	set struct {a {*named {a {*int ?} b {*int ?}} {}}}
	set try {a {a {a 1 b 2} b {a 2}}}
	structlist_unset -struct $struct $try {a a a}
} {a {a {b 2} b {a 2}}}

test structlist_unset-struct {basic one of named subfields} {
	set struct {a {*named {a {*int ?} b {*int ?}} {}}}
	set try {a {a {a 1 b 2} b {a 2}}}
	structlist_unset -struct $struct $try {a c}
} {a {a {a 1 b 2} b {a 2}}}

test structlist_unset-struct {basic list} {
	set struct {a {*list {*any ?}} b {*int ?}}
	set try {a {a b} b 2}
	structlist_unset -struct $struct $try a
} {b 2}

test structlist_unset-struct {basic one of list} {
	set struct {a {*list {*any ?}} b {*int ?}}
	set try {a {a b} b 2}
	structlist_unset -struct $struct $try {a 0}
} {a b b 2}

test structlist_unset-struct {basic one of list} {
	set struct {a {*list {*any ?}} b {*int ?}}
	set try {a {a b} b 2}
	structlist_unset -struct $struct $try {a end}
} {a a b 2}

test structlist_unset-struct {basic one in list subfield} {
	set struct {a {*list {a {*any ?}}} b {*int ?}}
	set try {a {{a a} {a b}} b 2}
	structlist_unset -struct $struct $try {a 0 a}
} {a {{} {a b}} b 2}

test structlist_unset-struct {one not present} {
	set struct {a {*list {a {*any ?}}} b {*int ?}}
	set try {a {{a a} {a b}} b 2}
	structlist_unset -struct $struct $try {a 0 a} d
} {error: tag "d" not present in structure "a {*list {a {*any ?}}} b {*int ?}"} 1

test structlist_set-struct {parameters} {
	set struct {*any {}}
	structlist_set -struct $struct {}
} {wrong # args: should be "structlist_set ?-struct schema? ?-data clientdata? list field value ?field value ...?"} 1

testsummarize

