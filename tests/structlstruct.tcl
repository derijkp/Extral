#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl
proc r {} {
	uplevel source ../lib/structl.tcl
	uplevel source ../lib/structltypes.tcl
}

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

test structlset-struct {basic long format} {
	set struct {{* aa a} {*int ?}}
	set try {}
	structlset -struct $struct $try aa 10
} {a 10}

test structlset-struct {basic long format error} {
	set struct {* {*int ?}}
	set try {}
	Extral::structlset -struct $struct $try aa 10
} {error: tag "aa" not present in structure "* {*int ?}"} 1

test structlset-struct {basic: error} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try a 10
} {error: incorrect value trying to assign "10" to struct "a {*int ?}" at field "a"} 1

test structlset-struct {basic cmd error} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try a
} {wrong # args: should be "structlset ?-struct schema? ?-data clientdata? list taglist value ?taglist value?"} 1

test structlset-struct {basic: error too much tags} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try {a a a} 10
} {error: field "a" not present in structure "*int ?" at field "a" at field "a"} 1

test structlset-struct {basic: error with *named} {
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
} {a {a 1 b 1}}

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

test structlget-struct {basic: long format} {
	set struct {{* aa a} {*int ?}}
	set try {a {a 10}}
	Extral::structlget -struct $struct $try aa
} {a 10}

test structlget-struct {basic: long format error} {
	set struct {* {*int ?}}
	set try {a {a 10}}
	Extral::structlget -struct $struct $try aa
} {error: tag "aa" not present in structure "* {*int ?}"} 1

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

test structlget-struct {long format} {
	set struct {
		{* Address a} {
			{* Street s} {*any ?}
			{* Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	structlget -struct $struct $try {}
} {Address {Street {Some Street} Number 10}}

test structlget-struct {long format} {
	set struct {
		{* Address a} {
			{* Street s} {*any ?}
			{* Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	structlget -struct $struct $try Address
} {Street {Some Street} Number 10}

test structlset-struct {long format: set by field} {
	set struct {
		{* Address a} {
			{* Street s} {*any ?}
			{* Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	structlset -struct $struct $try {Address Street} {try it}
} {a {s {try it} n 10}}

test structlset-struct {long format: set by value} {
	set struct {
		{* Address a} {
			{* Street s} {*any ?}
			{* Number n} {*int ?}
		}
	}
	set try {a {s {Some Street} n 10}}
	structlset -struct $struct $try Address {Street {try it} Number 1}
} {a {s {try it} n 1}}

test structlset-struct {long format: withnamed} {
	set struct {*named {{* try t} {*int ?}} {}}
	set try {a {t 1}}
	structlset -struct $struct $try b {try 2} a {try 4}
} {a {t 4} b {t 2}}

test structlfields {see values of long format} {
	Extral::structlfields {a {{* aa a} 1 b 2 c 3}} a
} {aa b c}

test structlfields {error in long format} {
	Extral::structlfields {a {* 1 b 2 c 3}} a
} {{} b c}

test structlunset-struct {basic} {
	set struct {a {*int ?}}
	set try {a 1}
	Extral::structlunset -struct $struct $try a
} {}

test structlunset-struct {basic 2} {
	set struct {a {*int ?} b {*int ?}}
	set try {a 1 b 2}
	structlunset -struct $struct $try a
} {b 2}

test structlunset-struct {basic 2 and 2 tags} {
	set struct {a {a {*int ?}} b {*int ?}}
	set try {a {a 1} b 2}
	structlunset -struct $struct $try {a a}
} {b 2}

test structlunset-struct {basic long format} {
	set struct {{* aa a} {a {*int ?}} b {*int ?}}
	set try {a {a 1} b 2}
	structlunset -struct $struct $try {aa a}
} {b 2}

test structlunset-struct {basic named} {
	set struct {a {*named {*int ?} {}} b {*int ?}}
	set try {a {a 1 b 2} b 2}
	structlunset -struct $struct $try a
} {b 2}

test structlunset-struct {basic one of named} {
	set struct {a {*named {*int ?} {}} b {*int ?}}
	set try {a {a 1 b 2} b 2}
	structlunset -struct $struct $try {a a}
} {a {b 2} b 2}

test structlunset-struct {basic one of named subfields} {
	set struct {a {*named {a {*int ?} b {*int ?}} {}}}
	set try {a {a {a 1 b 2} b {a 2}}}
	structlunset -struct $struct $try {a a a}
} {a {a {b 2} b {a 2}}}

test structlunset-struct {basic one of named subfields} {
	set struct {a {*named {a {*int ?} b {*int ?}} {}}}
	set try {a {a {a 1 b 2} b {a 2}}}
	structlunset -struct $struct $try {a c}
} {a {a {a 1 b 2} b {a 2}}}

test structlunset-struct {basic list} {
	set struct {a {*list {*any ?} {}} b {*int ?}}
	set try {a {a b} b 2}
	structlunset -struct $struct $try a
} {b 2}

test structlunset-struct {basic one of list} {
	set struct {a {*list {*any ?} {}} b {*int ?}}
	set try {a {a b} b 2}
	structlunset -struct $struct $try {a 0}
} {a b b 2}

test structlunset-struct {basic one of list} {
	set struct {a {*list {*any ?} {}} b {*int ?}}
	set try {a {a b} b 2}
	structlunset -struct $struct $try {a end}
} {a a b 2}

test structlunset-struct {basic one in list subfield} {
	set struct {a {*list {a {*any ?}} {}} b {*int ?}}
	set try {a {{a a} {a b}} b 2}
	Extral::structlunset -struct $struct $try {a 0 a}
} {a {{a b}} b 2}


testsummarize
