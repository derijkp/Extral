#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

auto_load time_scan
auto_load time_format

test structlist_set-types {any} {
	set struct {a {*any ?}}
	set try {}
	structlist_set -struct $struct $try a {try 10}
} {a {try 10}}

test structlist_set-types {int} {
	set struct {a {*int ?}}
	set try {}
	structlist_set -struct $struct $try a 10
} {a 10}

test structlist_set-types {int error: give string} {
	set struct {a {*int ?}}
	set try {}
	structlist_set -struct $struct $try a try
} {expected integer but got "try" at field "a"} 1

test structlist_set-types {int error in subfield} {
	set struct {a {a {*int ?}}}
	set try {}
	structlist_set -struct $struct $try a {a try}
} {expected integer but got "try" at field "a" at field "a"} 1

test structlist_set-types {int error: give double} {
	set struct {a {*int ?}}
	set try {}
	structlist_set -struct $struct $try a 10.2
} {expected integer but got "10.2" at field "a"} 1

test structlist_set-types {double} {
	set struct {a {*double ?}}
	set try {}
	structlist_set -struct $struct $try a 10.2
} {a 10.2}

test structlist_set-types {double: give int} {
	set struct {a {*double ?}}
	set try {}
	structlist_set -struct $struct $try a 10
} {a 10}

test structlist_set-types {double error: give string} {
	set struct {a {*double ?}}
	set try {}
	structlist_set -struct $struct $try a try
} {expected floating-point number but got "try" at field "a"} 1

test structlist_set-types {bool} {
	set struct {a {*bool ?}}
	set try {}
	structlist_set -struct $struct $try a true
} {a 1}

test structlist_set-types {bool} {
	set struct {a {*bool ?}}
	set try {}
	structlist_set -struct $struct $try a no
} {a 0}

test structlist_set-types {bool error} {
	set struct {a {*bool ?}}
	set try {}
	structlist_set -struct $struct $try a try
} {expected boolean value but got "try" at field "a"} 1

test structlist_set-types {regexp} {
	set struct {a {*regexp ^a "does not start with an a" ?}}
	set try {}
	structlist_set -struct $struct $try a a10
} {a a10}

test structlist_set-types {regexp error} {
	set struct {a {*regexp ^a "does not start with an a" ?}}
	set try {}
	structlist_set -struct $struct $try a b10
} {error: "b10" does not start with an a at field "a"} 1

test structlist_set-types {regexp: one of} {
	set struct {a {*regexp ^try|any|yes|no$ "is not one of try, any, yes or no" ?}}
	set try {}
	structlist_set -struct $struct $try a any
} {a any}

test structlist_set-types {regexp: one of error} {
	set struct {a {*regexp ^try|any|yes|no$ "is not one of try, any, yes or no" ?}}
	set try {}
	structlist_set -struct $struct $try a test
} {error: "test" is not one of try, any, yes or no at field "a"} 1

test structlist_set-types {regexp: not enough arguments} {
	set struct {a {*regexp}}
	set try {}
	structlist_set -struct $struct $try a try
} {error: wrong number of arguments in structure "*regexp": should be "*regexp pattern errormsg default" at field "a"} 1

test structlist_set-types {between: ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlist_set -struct $struct $try a 5
} {a 5}

test structlist_set-types {between: lower ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlist_set -struct $struct $try a 0
} {a 0}

test structlist_set-types {between: higher ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlist_set -struct $struct $try a 10
} {a 10}

test structlist_set-types {between: too low} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlist_set -struct $struct $try a -1
} {error: -1 is not between 0 and 10 at field "a"} 1

test structlist_set-types {between: too high} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlist_set -struct $struct $try a 11
} {error: 11 is not between 0 and 10 at field "a"} 1

test structlist_set-types {between: error} {
	set struct {a {*between}}
	set try {}
	structlist_set -struct $struct $try a 5
} {error: wrong number of arguments in structure "*between" at field "a"} 1

test structlist_set-types {dbetween: ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlist_set -struct $struct $try a 1.2
} {a 1.2}

test structlist_set-types {dbetween: lower ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlist_set -struct $struct $try a 0.5
} {a 0.5}

test structlist_set-types {dbetween: higher ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlist_set -struct $struct $try a 1.8
} {a 1.8}

test structlist_set-types {dbetween: too low} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlist_set -struct $struct $try a 0
} {error: 0 is not between 0.5 and 1.8 at field "a"} 1

test structlist_set-types {dbetween: too high} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlist_set -struct $struct $try a 11
} {error: 11 is not between 0.5 and 1.8 at field "a"} 1

test structlist_set-types {proc} {
	namespace eval ::Extral {
		proc setproc {structure data oldvalue field value} {
			return "try:$value"
		}
	}
	set struct {a {*proc ?}}
	set try {}
	structlist_set -struct $struct $try a 11
} {a try:11}

test structlist_set-types {proc with argument: set} {
	namespace eval ::Extral {
		proc setproc {structure data oldvalue field value} {
			return "[lindex $structure 1]:$value"
		}
	}
	set struct {a {*proc t ?}}
	set try {}
	structlist_set -struct $struct $try a 11
} {a t:11}

test structlist_set-types {proc set: use oldvalue} {
	namespace eval ::Extral {
		proc setproc {structure data oldvalue field value} {
			return "$oldvalue -> $value"
		}
	}
	set struct {a {*proc t ?}}
	set try {a 8}
	structlist_set -struct $struct $try a 9
} {a {8 -> 9}}

test structlget-types {get int default} {
	set struct {a {*int ?}}
	set try {}
	structlget -struct $struct $try a
} {?}

test structlist_set-types {set int default} {
	set struct {a {*int ?}}
	set try {a 1}
	structlist_set -struct $struct $try a ?
} {}

test structlist_set-types {set date default} {
	set struct {a {*date ?}}
	set try {a 1}
	structlist_set -struct $struct $try a ?
} {}

test structlget-types {get any default} {
	set struct {a {*any ?}}
	set try {}
	structlget -struct $struct $try a
} {?}

test structlget-types {get date default} {
	set struct {*date ?}
	set try {}
	structlget -struct $struct $try {}
} {?}

test structlget-types {get date default sub} {
	set struct {a {*date ?}}
	set try {}
	structlget -struct $struct $try a
} {?}

test structlget-types {get time default sub} {
	set struct {a {*time ?}}
	set try {}
	structlget -struct $struct $try a
} {?}

test structlget-types {get int} {
	set struct {a {*int ?}}
	set try {a 10}
	structlget -struct $struct $try a
} {10}

test structlget-types {proc get} {
	namespace eval ::Extral {
		proc getproc {structure data field value} {
			return "try:$value"
		}
	}
	set struct {a {*proc ?}}
	set try {a 10}
	structlget -struct $struct $try a
} {try:10}

test structlget-types {proc get with argument} {
	namespace eval ::Extral {
		proc getproc {structure data field value} {
			return "[lindex $structure 1]:$value"
		}
	}
	set struct {a {*proc t ?}}
	set try {a 10}
	structlget -struct $struct $try a
} {t:10}

test structlist_set-types {set date} {
	set struct {a {*date t ?}}
	set try {}
	structlist_set -struct $struct $try a {9 May 1997}
} {a 62998732800.0}

test structlist_set-types {set date 2} {
	set struct {a {*date t ?}}
	set try {}
	structlist_set -struct $struct $try a {05/09/1997}
} {a 62998732800.0}

test structlget-types {get date} {
	set struct {a {*date t ?}}
	set try {a 62998732800.0}
	structlget -struct $struct $try a
} {9 May 1997}

test structlget-types {get date} {
	set struct {a {*date ?}}
	set try {a 62998732800.0}
	structlget -struct $struct $try {a val}
} {62998732800.0}

test structlist_set-types {set time} {
	set struct {a {*time ?}}
	set try {}
	structlist_set -struct $struct $try a {9 May 1997 12:30:24}
} {a 62998777824.0}

test structlist_set-types {set time 2} {
	set struct {a {*time ?}}
	set try {}
	structlist_set -struct $struct $try a {05/09/1997 12:30:24}
} {a 62998777824.0}

test structlget-types {get time} {
	set struct {a {*time ?}}
	set try {a 62998777824.0}
	structlget -struct $struct $try a
} {9 May 1997 12:30:24}

test structlget-types {get time: check empty} {
	set struct {a {*time ?}}
	structlget -struct $struct {} a
} {?}

test structlget-types {get time: check empty with val} {
	set struct {a {*time ?}}
	structlget -struct $struct {} {a val}
} {?}

test structlget-types {get time: check empty with param} {
	set struct {a {*time ?}}
	structlget -struct $struct {} {a "%t"}
} {?}

test structlget-types {get date: check empty with val} {
	set struct {a {*date ?}}
	structlget -struct $struct {} {a val}
} {?}

test structlget-types {get date: check empty with param} {
	set struct {a {*date ?}}
	structlget -struct $struct {} {a "%t"}
} {?}

test structlget-types {set time: check empty} {
	set struct {a {*time ?}}
	structlist_set -struct {a {*time ?}} {} a ?
} {}

test structlget-types {get time with format} {
	set struct {a {*time ?}}
	set try {a 62998777824.0}
	structlget -struct $struct $try {a "%H:%M:%S %e %b %Y"}
} {12:30:24 9 May 1997}

test structlget-types {get time with format} {
	set struct {a {*time ?}}
	set try {a 62998777824.0}
	structlget -struct $struct $try {a val}
} {62998777824.0}

test structlist_set-types {proc get with field} {
	namespace eval ::Extral {
		proc getproc {structure data field value} {
			return [list $field $value]
		}
	}
	set struct {a {*proc ?}}
	set try {a 11}
	structlget -struct $struct $try {a b}
} {b 11}

test structlist_set-types {proc set: check data} {
	namespace eval ::Extral {
		proc setproc {structure data oldvalue field value} {
			return [list $data $oldvalue $value]
		}
	}
	set struct {a {*proc t ?}}
	set try {a 8}
	structlist_set -data d -struct $struct $try a 9
} {a {d 8 9}}

test structlget-types {proc get: test data} {
	namespace eval ::Extral {
		proc getproc {structure data field value} {
			return [list $data $value]
		}
	}
	set struct {a {*proc ?}}
	set try {a 10}
	structlget -data d -struct $struct $try a
} {d 10}

test structlget-types {quotes in any} {
	set dbstruct {t {*any {}}}
	set db {t { "try}}
	structlget -struct $dbstruct $db t
} { "try}

#
#	list
#

test structlist_set-list {list set} {
	set struct {a {*list {*int ?}}}
	set try {}
	structlist_set -struct $struct $try a {1 2 3}
} {a {1 2 3}}

test structlist_set-list {list parameter next} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	structlist_set -struct $struct $try {a next} 4
} {a {1 2 3 4}}

test structlist_set-list {list parameter next from empty} {
	set struct {a {*list {*int ?}}}
	set try {a {}}
	structlist_set -struct $struct $try {a next} 4
} {a 4}

test structlist_set-list {list parameter next from empty, no subtag} {
	set struct {*list {*int ?}}
	set try {}
	structlist_set -struct $struct $try next 4
} {4}

test structlist_set-list {list parameter num} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	structlist_set -struct $struct $try {a 1} 20
} {a {1 20 3}}

test structlist_set-list {list parameter: non existing num} {
	set struct {a {*list {*int ?}}}
	set try {}
	structlist_set -struct $struct $try {a 2} 4
} {empty list at field "a"} 1

test structlist_set-list {list parameter end} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	structlist_set -struct $struct $try {a end} 30
} {a {1 2 30}}

test structlist_set-list {list error} {
	set struct {a {*list {*int ?}}}
	set try {}
	structlist_set -struct $struct $try a {1 2 c}
} {expected integer but got "c" at field "a"} 1

test structlist_set-list {struct in list: simple} {
	set struct {
		a {*list {
			a {*int ?}
			b {*int ?}
		}}
	}
	set try {}
	structlist_set -struct $struct {} {a next a} 1
} {a {{a 1}}}

test structlist_set-list {struct in list} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1}}}
	structlist_set -struct $struct $try {a "" b} {1 2 3}
} {a {{a 1 b 1} {b 2} {b 3}}}

test structlist_set-list {struct in list with parameter} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1} {a 2}}}
	structlist_set -struct $struct $try {a 1 b} 2
} {a {{a 1} {a 2 b 2}}}

test structlist_set-list {struct in list with parameter 0} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1} {a 2}}}
	structlist_set -struct $struct $try {a 0 b} 2
} {a {{a 1 b 2} {a 2}}}

test structlist_set-list {struct in list error} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {}
	structlist_set -struct $struct $try {a "" b} {1 c 3}
} {expected integer but got "c" at field "b" at field "a"} 1

#	list unset
#
test structlunset-list {list set} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	structlunset -struct $struct $try a
} {}

test structlunset-list {list parameter num} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	structlunset -struct $struct $try {a 1}
} {a {1 3}}

test structlunset-list {list parameter: non existing num} {
	set struct {a {*list {*int ?}}}
	set try {}
	structlunset -struct $struct $try {a 2}
} {}

test structlunset-list {list parameter end} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	structlunset -struct $struct $try {a end}
} {a {1 2}}

test structlunset-list {struct in list: simple} {
	set struct {
		a {*list {
			a {*int ?}
			b {*int ?}
		}}
	}
	set try {a {{a 1}}}
	structlunset -struct $struct $try {a end a}
} {a {{}}}

test structlunset-list {struct in list} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1 b 1} {b 2} {b 3}}}
	structlunset -struct $struct $try {a "" b}
} {a {{a 1} {} {}}}

test structlunset-list {struct in list with parameter} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1} {a 2 b 2}}}
	structlunset -struct $struct $try {a 1 b}
} {a {{a 1} {a 2}}}

test structlunset-list {struct in list with parameters; gets empty} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1 b 1} {b 2} {b 3}}}
	structlunset -struct $struct $try {a 1 b}
} {a {{a 1 b 1} {} {b 3}}}

test structlunset-list {struct in list with parameter 0} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1 b 2} {a 2}}}
	structlunset -struct $struct $try {a 0 b}
} {a {{a 1} {a 2}}}

#	list get
#
test structlget-list {list test type, end index} {
	set struct {a {*list {*date ?}}}
	set try [structlist_set -struct $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	structlget -struct $struct $try {a end}
} {2 Jan 1998}

test structlist_set-list {set ints in list from empty} {
	set struct {a {*list {*int ?}}}
	structlist_set -struct $struct {} a {1 2}
} {a {1 2}}

test structlist_set-list {set dates in list from empty} {
	set struct {a {*list {*date ?}}}
	structlist_set -struct $struct {} a {{1 Jan 1998} {2 Jan 1998}}
} {a {63019209600.0 63019296000.0}}

test structlget-list {list test type, 0 index} {
	set struct {a {*list {*date ?}}}
	set try [structlist_set -struct $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	structlget -struct $struct $try {a 0}
} {1 Jan 1998}

test structlget-list {list test type, 1 to end} {
	set struct {a {*list {*date ?}}}
	set try {a {1 2 3}}
	set try [structlist_set -struct $struct $try a {{1 Jan 1998} {2 Jan 1998} {3 Jan 1998}}]
	structlget -struct $struct $try {a {1 end}}
} {{2 Jan 1998} {3 Jan 1998}}

test structlget-list {list test type, 1 to end} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3 4}}
	structlget -struct $struct $try {a {0 end {lmath_sum}}}
} {10}

test structlget-list {list test type, 1 to end} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3 4}}
	structlget -struct $struct $try {a {0 end {lmath_average}}}
} {2.5}

test structlget-list {list index out of range} {
	set struct {a {*list {*date ?}}}
	set try [structlist_set -struct $struct {} a {{1 Jan 1998}}]
	structlget -struct $struct $try {a 2}
} {}

test structlget-list {list index error: invalid command} {
	set struct {a {*list {*int ?}}}
	set try [structlist_set -struct $struct {} a {1 2 4}]
	structlget -struct $struct $try {a {1 2 3}}
} {invalid command name "3"} 1

test structlget-list {list index out of range} {
	set struct {a {*list {*date ?}}}
	set try [structlist_set -struct $struct {} a {{1 Jan 1998}}]
	structlget -struct $struct $try {a {2 4}}
} {}

test structlget-list {list 2 indices out of range} {
	set struct {a {*list {*date ?}}}
	set try [structlist_set -struct $struct {} a {{1 Jan 1998}}]
	structlget -struct $struct $try {a {0 4}}
} {{1 Jan 1998}}

test structlget-list {list 2 indices} {
	set struct {a {*list {*date ?}}}
	set try [structlist_set -struct $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	structlget -struct $struct $try {a {0 end}}
} {{1 Jan 1998} {2 Jan 1998}}

test structlget-list {list 2 indices} {
	set struct {a {*list {*date ?}}}
	set try [structlist_set -struct $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	structlget -struct $struct $try {a {0 1}}
} {{1 Jan 1998} {2 Jan 1998}}

test structlist_set-list {list regexp} {
	set struct {{? parts pts} {*list {*regexp ^a "does not start with an a" ?}}}
	set try {}
	structlist_set -struct $struct $try pts {b}
} {error: "b" does not start with an a at field "pts"} 1

test structlist_set-list {set empty list sub element} {
	set dbstruct {*list {a {*any {}}}}
	structlist_set -struct $dbstruct {} next {}
} {{}}

test structlist_set-list {set empty list sub element} {
	set dbstruct {
	        {? article art} {
	            *list {
	                {? author a} {*any {}}
	            }
	        }
	}
	set db [structlist_set -struct $dbstruct {} {art next} {}]
} {art {{}}}

test structlget-list {non empty default} {
	set struct {*list {*any {}}} 
	structlget -struct $struct {} {}
} {}

test structlist_set-list {non empty default} {
	set struct {*list {*any {}}} 
	set data b
	structlist_set -struct $struct $data 0 a
} {a}

#
#	named
#

test structlist_set-named {named set} {
	set struct {*named {{? try t} {*int ?}}}
	set try {a {t 1}}
	structlist_set -struct $struct $try b {t 2}
} {a {t 1} b {t 2}}

test structlist_set-named {named set existing to default} {
	set struct {*named {{? try t} {*int ?}}}
	set try {a {t 1}}
	structlist_set -struct $struct $try a {try ?}
} {}

test structlist_set-named {named set to default} {
	set struct {*named {{? try t} {*int ?}}}
	set try {a {t 1}}
	structlist_set -struct $struct $try b {try 2} a {try ?}
} {b {t 2}}

test structlist_set-named {named set to empty} {
	set struct {t {*named {*int ?}}}
	set try {t {a 1}}
	structlist_set -struct $struct $try t {}
} {t {a 1}}

test structlist_set-named {named set to default 2} {
	set struct {t {*named {*int ?}}}
	set try {t {a 1}}
	structlist_set -struct $struct $try t {a ?}
} {}

test structlist_set-named {check empty list with named} {
	set struct {auth {*list {*named {*any {}}}}}
	structlget -struct $struct {} {auth end name}
} {}

test structlget-named {check for non existing error} {
	proc ::Extral::getproc {structure data field value} {
		return [list $field $value]
	}
	set struct {
		auth {
			*named {*proc Auth {}}
		}
		jou {*link Jou {}}
	}
	set data {auth {1 auth1.Auth}}
	structlget -struct $struct $data {auth 2 name}
} {name {}}

test structlget-named {get all from empty} {
	set schema {nums {*named {*any ?}}}
	structlget -struct $schema {} {}
} {nums {}}

test structlget-named {get all from empty tag before named} {
	set schema {nums {*named {*any ?}}}
	structlget -struct $schema {} nums
} {}

test structlget-named {get all from empty tag in named} {
	set schema {nums {*named {*any ?}}}
	structlget -struct $schema {} {nums a}
} ?

test structlget-named {with bool} {
	set schema {{? bool b} {*named {*bool 0}}}
	structlist_set -struct $schema {} {bool gvf} 1
} {b {gvf 1}}

testsummarize
