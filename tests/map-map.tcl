#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl
proc r {} {
	uplevel source ../lib/map.tcl
	uplevel source ../lib/maptypes.tcl
}

test map_set-map {basic set one} {
	set map {a {*int ?}}
	set try {}
	map_set -map $map $try a 10
} {a 10}

test map_set-map {basic: 2 tags} {
	set map {a {a {*int ?}}}
	set try {}
	map_set -map $map $try {a a} 10
} {a {a 10}}

test map_set-map {basic: set to default} {
	set map {a {*int ?}}
	set try {}
	map_set -map $map $try a ?
} {}

test map_set-map {basic: set to default: 2 tags} {
	set map {a {a {*int ?}}}
	set try {}
	map_set -map $map $try {a a} ?
} {}

test map_set-map {basic: set to default: remove old} {
	set map {a {a {*int ?}}}
	set try {a {a 10}}
	map_set -map $map $try {a a} ?
} {}

test map_set-map {check empty field with set} {
	set map {a {*int ?}}
	set try {}
	map_set -map $map $try {} {a 1}
} {a 1}

test map_set-map {basic: set one of 2 to default} {
	set map {a {a {*int ?} b {*int ?}}}
	set try {a {a 1 b 2}}
	map_set -map $map $try {a a} ?
} {a {b 2}}

test map_set-map {named: replace one} {
	set map {*named {*int ?} {}}
	set try {a 1}
	map_set -map $map $try a 10
} {a 10}

test map_set-map {named: add one} {
	set map {*named {*int ?} {}}
	set try {a 1}
	map_set -map $map $try b 10
} {a 1 b 10}

test map_set-map {basic: set multi} {
	set map {*named {*int ?} {}}
	set try {a 1}
	map_set -map $map $try b 10 a 2
} {a 2 b 10}

test map_set-map {basic: add one: 2 tags} {
	set map {a {*named {*int ?} {}}}
	set try {a {a 1}}
	map_set -map $map $try {a b} 10
} {a {a 1 b 10}}

test map_set-map {basic: change one} {
	set map {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	map_set -map $map $try {a b} 10
} {a {a 1 b 10}}

test map_set-map {basic long format} {
	set map {{? aa a} {*int ?}}
	set try {}
	map_set -map $map $try aa 10
} {a 10}

test map_set-map {basic long format error} {
	set map {? {*int ?}}
	set try {}
	map_set -map $map $try aa 10
} {error: tag "aa" not present in map "? {*int ?}"} 1

test map_set-map {basic: error} {
	set map {a {a {*int ?}}}
	set try {}
	map_set -map $map $try a 10
} {error: incorrect value trying to assign "10" to map "a {*int ?}" at field "a"} 1

test map_set-map {basic cmd error} {
	set map {a {*int ?}}
	set try {}
	map_set -map $map $try a
} {wrong # args: should be "map_set ?-map schema? ?-data clientdata? list field value ?field value ...?"} 1

test map_set-map {basic: error too much tags} {
	set map {a {a {*int ?}}}
	set try {}
	map_set -map $map $try {a a a} 10
} {error: field "a" not present in map "*int ?" at field "a" at field "a"} 1

test map_set-map {basic: error with *named} {
	set map {a {*named {*int ?} {}}}
	set try {}
	map_set -map $map $try a 10
} {error: "10" does not have an even number of elements at field "a"} 1

test map_set-map {empty -map: set} {
	set try {}
	map_set -map {} $try a 10
} {error: tag "a" not present in map ""} 1

test map_get-map {empty -map: get} {
	set try {}
	map_get -map {} $try a
} {error: tag "a" not present in map ""} 1

test map_set-map {basic: set one by value} {
	set map {a {a {*int ?}}}
	set try {}
	map_set -map $map $try a {a 10}
} {a {a 10}}

test map_set-map {basic: add one by value} {
	set map {a {*named {*int ?} {}}}
	set try {a {a 1}}
	map_set -map $map $try a {b 10}
} {a {a 1 b 10}}

test map_set-map {basic: named remove default} {
	set map {a {*named {*int ?} {}}}
	set try {a {a 1}}
	map_set -map $map $try {a a} ?
} {}

test map_set-map {basic: named remove default} {
	set map {a {*named {*int ?} {}}}
	set try {a {a 1}}
	map_set -map $map $try a {a ?}
} {}

test map_set-map {basic: change one by value} {
	set map {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	map_set -map $map $try a {b 10}
} {a {a 1 b 10}}

test map_set-map {basic: change one} {
	set map {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	map_set -map $map $try {a b} 10
} {a {a 1 b 10}}

test map_set-map {basic: set empty} {
	set map {a {*named {*int ?} {}}}
	set try {a {a 1 b 1}}
	map_set -map $map $try a {}
} {a {a 1 b 1}}

# get tests

test map_get-map {check empty field with get} {
	set map {a {*int ?}}
	set try {}
	map_get -map $map $try {}
} {a ?}

test map_get-map {basic: get value} {
	set map {a {a {*int ?}}}
	set try {a {a 10}}
	map_get -map $map $try {a a}
} {10}

test map_get-map {basic: get submap} {
	set map {a {a {*int ?}}}
	set try {a {a 10}}
	map_get -map $map $try a
} {a 10}

test map_get-map {basic: get submap: mixed with default} {
	set map {a {a {*int ?} b {*int ?}}}
	set try {a {a 10}}
	map_get -map $map $try a
} {a 10 b ?}

test map_get-map {basic: get multi: submap: mixed with default} {
	set map {a {a {*int ?} b {*int ?}}}
	set try {a {a 10}}
	map_get -map $map $try a {a b}
} {{a 10 b ?} ?}

test map_get-map {basic: default} {
	set map {a {a {*int ?}}}
	set try {}
	map_get -map $map $try {a a}
} {?}

test map_get-map {basic: default map} {
	set map {a {a {*int ?}}}
	set try {}
	map_get -map $map $try a
} {a ?}

test map_set-map {by value} {
	set map {i {*int ?} s {*any ?}}
	map_set -map $map {} {} {i 2 s try}
} {i 2 s try}

test map_get-map {basic: long format} {
	set map {{? aa a} {*int ?}}
	set try {a {a 10}}
	map_get -map $map $try aa
} {a 10}

test map_get-map {basic: long format error} {
	set map {? {*int ?}}
	set try {a {a 10}}
	map_get -map $map $try aa
} {error: tag "aa" not present in map "? {*int ?}"} 1

test map_get-map {check for error: map not even} {
	set map accn
	map_get -map $map {} {}
} {error: map "accn" does not have an even number of elements} 1

test map_get-map {check for error} {
	set map {accn {*a} {*a} {}}
	map_get -map $map {} {}
} {invalid command name "::Extral::geta"} 1

test map_set-map {named by field} {
	set map {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10} b {}}
	map_set -map $map $try {a s} 1
} {a {s 1 n 10} b {}}

test map_set-map {named by field and value} {
	set map {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10}}
	map_set -map $map $try a {s a n 1}
} {a {s a n 1}}

test map_set-map {named no name: not even} {
	set map {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10}}
	map_set -map $map $try {} try
} {error: "try" does not have an even number of elements} 1

test map_set-map {named no name} {
	set map {*named {a {*int ?}} {}}
	set try {}
	map_set -map $map $try {} {try 1}
} {error: incorrect value trying to assign "1" to map "a {*int ?}" in named "try"} 1

test map_set-map {named no name} {
	set map {*named {a {*int ?}} {}}
	set try {}
	map_set -map $map $try {} {try {a a}}
} {expected integer but got "a" at field "a" in named "try"} 1

test map_get-map {named by value} {
	set map {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10} b {}}
	map_get -map $map $try {}
} {a {s {Some Street} n 10} b {s ? n ?}}

test map_get-map {named by field} {
	set map {
		*named {
			s {*any ?}
			n {*int ?}
		} {}
	}
	set try {a {s {Some Street} n 10} b {}}
	map_get -map $map $try {a s}
} {Some Street}

test map_get-map {long format} {
	set map {
		{? Address a} {
			{? Street s} {*any ?}
			{? Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	map_get -map $map $try {}
} {Address {Street {Some Street} Number 10}}

test map_get-map {long format} {
	set map {
		{? Address a} {
			{? Street s} {*any ?}
			{? Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	map_get -map $map $try Address
} {Street {Some Street} Number 10}

test map_set-map {long format: set by field} {
	set map {
		{? Address a} {
			{? Street s} {*any ?}
			{? Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	map_set -map $map $try {Address Street} {try it}
} {a {s {try it} n 10}}

test map_set-map {long format: set by value} {
	set map {
		{? Address a} {
			{? Street s} {*any ?}
			{? Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	map_set -map $map $try Address {Street {try it} Number 1}
} {a {s {try it} n 1}}

test map_set-map {long format: withnamed} {
	set map {*named {{? try t} {*int ?}} {}}
	set try {a {t 1}}
	map_set -map $map $try b {try 2} a {try 4}
} {a {t 4} b {t 2}}

test map_fields {see values of long format} {
	map_fields {a {{? aa a} 1 b 2 c 3}} a
} {aa b c}

test map_fields {error in long format} {
	map_fields {a {? 1 b 2 c 3}} a
} {{} b c}

test map_unset-map {basic} {
	set map {a {*int ?}}
	set try {a 1}
	map_unset -map $map $try a
} {}

test map_unset-map {basic 2} {
	set map {a {*int ?} b {*int ?}}
	set try {a 1 b 2}
	map_unset -map $map $try a
} {b 2}

test map_unset-map {basic 2 and 2 tags} {
	set map {a {a {*int ?}} b {*int ?}}
	set try {a {a 1} b 2}
	map_unset -map $map $try {a a}
} {b 2}

test map_unset-map {basic long format} {
	set map {{? aa a} {a {*int ?}} b {*int ?}}
	set try {a {a 1} b 2}
	map_unset -map $map $try {aa a}
} {b 2}

test map_unset-map {basic named} {
	set map {a {*named {*int ?} {}} b {*int ?}}
	set try {a {a 1 b 2} b 2}
	map_unset -map $map $try a
} {b 2}

test map_unset-map {basic one of named} {
	set map {a {*named {*int ?} {}} b {*int ?}}
	set try {a {a 1 b 2} b 2}
	map_unset -map $map $try {a a}
} {a {b 2} b 2}

test map_unset-map {basic one of named subfields} {
	set map {a {*named {a {*int ?} b {*int ?}} {}}}
	set try {a {a {a 1 b 2} b {a 2}}}
	map_unset -map $map $try {a a a}
} {a {a {b 2} b {a 2}}}

test map_unset-map {basic one of named subfields} {
	set map {a {*named {a {*int ?} b {*int ?}} {}}}
	set try {a {a {a 1 b 2} b {a 2}}}
	map_unset -map $map $try {a c}
} {a {a {a 1 b 2} b {a 2}}}

test map_unset-map {basic list} {
	set map {a {*list {*any ?}} b {*int ?}}
	set try {a {a b} b 2}
	map_unset -map $map $try a
} {b 2}

test map_unset-map {basic one of list} {
	set map {a {*list {*any ?}} b {*int ?}}
	set try {a {a b} b 2}
	map_unset -map $map $try {a 0}
} {a b b 2}

test map_unset-map {basic one of list} {
	set map {a {*list {*any ?}} b {*int ?}}
	set try {a {a b} b 2}
	map_unset -map $map $try {a end}
} {a a b 2}

test map_unset-map {basic one in list subfield} {
	set map {a {*list {a {*any ?}}} b {*int ?}}
	set try {a {{a a} {a b}} b 2}
	map_unset -map $map $try {a 0 a}
} {a {{} {a b}} b 2}

test map_unset-map {one not present} {
	set map {a {*list {a {*any ?}}} b {*int ?}}
	set try {a {{a a} {a b}} b 2}
	map_unset -map $map $try {a 0 a} d
} {error: tag "d" not present in map "a {*list {a {*any ?}}} b {*int ?}"} 1

test map_set-map {parameters} {
	set map {*any {}}
	map_set -map $map {}
} {wrong # args: should be "map_set ?-map schema? ?-data clientdata? list field value ?field value ...?"} 1

testsummarize

