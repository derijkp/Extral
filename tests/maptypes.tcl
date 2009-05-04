#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

auto_load time_scan
auto_load time_format

test map_set-types {any} {
	set struct {a {*any ?}}
	set try {}
	map_set -map $struct $try a {try 10}
} {a {try 10}}

test map_set-types {int} {
	set struct {a {*int ?}}
	set try {}
	map_set -map $struct $try a 10
} {a 10}

test map_set-types {int error: give string} {
	set struct {a {*int ?}}
	set try {}
	map_set -map $struct $try a try
} {expected integer but got "try" at field "a"} error

test map_set-types {int error in subfield} {
	set struct {a {a {*int ?}}}
	set try {}
	map_set -map $struct $try a {a try}
} {expected integer but got "try" at field "a" at field "a"} error

test map_set-types {int error: give double} {
	set struct {a {*int ?}}
	set try {}
	map_set -map $struct $try a 10.2
} {expected integer but got "10.2" at field "a"} error

test map_set-types {double} {
	set struct {a {*double ?}}
	set try {}
	map_set -map $struct $try a 10.2
} {a 10.2}

test map_set-types {double: give int} {
	set struct {a {*double ?}}
	set try {}
	map_set -map $struct $try a 10
} {a 10}

test map_set-types {double error: give string} {
	set struct {a {*double ?}}
	set try {}
	map_set -map $struct $try a try
} {expected floating-point number but got "try" at field "a"} error

test map_set-types {bool} {
	set struct {a {*bool ?}}
	set try {}
	map_set -map $struct $try a true
} {a 1}

test map_set-types {bool} {
	set struct {a {*bool ?}}
	set try {}
	map_set -map $struct $try a no
} {a 0}

test map_set-types {bool error} {
	set struct {a {*bool ?}}
	set try {}
	map_set -map $struct $try a try
} {expected boolean value but got "try" at field "a"} error

test map_set-types {regexp} {
	set struct {a {*regexp ^a "does not start with an a" ?}}
	set try {}
	map_set -map $struct $try a a10
} {a a10}

test map_set-types {regexp error} {
	set struct {a {*regexp ^a "does not start with an a" ?}}
	set try {}
	map_set -map $struct $try a b10
} {error: "b10" does not start with an a at field "a"} error

test map_set-types {regexp: one of} {
	set struct {a {*regexp ^try|any|yes|no$ "is not one of try, any, yes or no" ?}}
	set try {}
	map_set -map $struct $try a any
} {a any}

test map_set-types {regexp: one of error} {
	set struct {a {*regexp ^try|any|yes|no$ "is not one of try, any, yes or no" ?}}
	set try {}
	map_set -map $struct $try a test
} {error: "test" is not one of try, any, yes or no at field "a"} error

test map_set-types {regexp: not enough arguments} {
	set struct {a {*regexp}}
	set try {}
	map_set -map $struct $try a try
} {error: wrong number of arguments in map "*regexp": should be "*regexp pattern errormsg default" at field "a"} error

test map_set-types {between: ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	map_set -map $struct $try a 5
} {a 5}

test map_set-types {between: lower ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	map_set -map $struct $try a 0
} {a 0}

test map_set-types {between: higher ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	map_set -map $struct $try a 10
} {a 10}

test map_set-types {between: too low} {
	set struct {a {*between 0 10 ?}}
	set try {}
	map_set -map $struct $try a -1
} {error: -1 is not between 0 and 10 at field "a"} error

test map_set-types {between: too high} {
	set struct {a {*between 0 10 ?}}
	set try {}
	map_set -map $struct $try a 11
} {error: 11 is not between 0 and 10 at field "a"} error

test map_set-types {between: error} {
	set struct {a {*between}}
	set try {}
	map_set -map $struct $try a 5
} {error: wrong number of arguments in map "*between" at field "a"} error

test map_set-types {dbetween: ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	map_set -map $struct $try a 1.2
} {a 1.2}

test map_set-types {dbetween: lower ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	map_set -map $struct $try a 0.5
} {a 0.5}

test map_set-types {dbetween: higher ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	map_set -map $struct $try a 1.8
} {a 1.8}

test map_set-types {dbetween: too low} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	map_set -map $struct $try a 0
} {error: 0 is not between 0.5 and 1.8 at field "a"} error

test map_set-types {dbetween: too high} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	map_set -map $struct $try a 11
} {error: 11 is not between 0.5 and 1.8 at field "a"} error

test map_set-types {proc} {
	namespace eval ::Extral {
		proc setproc {map data oldvalue field value} {
			return "try:$value"
		}
	}
	set struct {a {*proc ?}}
	set try {}
	map_set -map $struct $try a 11
} {a try:11}

test map_set-types {proc with argument: set} {
	namespace eval ::Extral {
		proc setproc {map data oldvalue field value} {
			return "[lindex $map 1]:$value"
		}
	}
	set struct {a {*proc t ?}}
	set try {}
	map_set -map $struct $try a 11
} {a t:11}

test map_set-types {proc set: use oldvalue} {
	namespace eval ::Extral {
		proc setproc {map data oldvalue field value} {
			return "$oldvalue -> $value"
		}
	}
	set struct {a {*proc t ?}}
	set try {a 8}
	map_set -map $struct $try a 9
} {a {8 -> 9}}

test map_get-types {get int default} {
	set struct {a {*int ?}}
	set try {}
	map_get -map $struct $try a
} {?}

test map_set-types {set int default} {
	set struct {a {*int ?}}
	set try {a 1}
	map_set -map $struct $try a ?
} {}

test map_set-types {set date default} {
	set struct {a {*date ?}}
	set try {a 1}
	map_set -map $struct $try a ?
} {}

test map_get-types {get any default} {
	set struct {a {*any ?}}
	set try {}
	map_get -map $struct $try a
} {?}

test map_get-types {get date default} {
	set struct {*date ?}
	set try {}
	map_get -map $struct $try {}
} {?}

test map_get-types {get date default sub} {
	set struct {a {*date ?}}
	set try {}
	map_get -map $struct $try a
} {?}

test map_get-types {get time default sub} {
	set struct {a {*time ?}}
	set try {}
	map_get -map $struct $try a
} {?}

test map_get-types {get int} {
	set struct {a {*int ?}}
	set try {a 10}
	map_get -map $struct $try a
} {10}

test map_get-types {proc get} {
	namespace eval ::Extral {
		proc getproc {map data field value} {
			return "try:$value"
		}
	}
	set struct {a {*proc ?}}
	set try {a 10}
	map_get -map $struct $try a
} {try:10}

test map_get-types {proc get with argument} {
	namespace eval ::Extral {
		proc getproc {map data field value} {
			return "[lindex $map 1]:$value"
		}
	}
	set struct {a {*proc t ?}}
	set try {a 10}
	map_get -map $struct $try a
} {t:10}

test map_set-types {set date} {
	set struct {a {*date t ?}}
	set try {}
	map_set -map $struct $try a {9 May 1997}
} {a {729152 0}}

test map_set-types {set date 2} {
	set struct {a {*date t ?}}
	set try {}
	map_set -map $struct $try a {05/09/1997}
} {a {729152 0}}

test map_get-types {get date} {
	set struct {a {*date t ?}}
	set try {a {729152 0}}
	map_get -map $struct $try a
} {9 May 1997}

test map_get-types {get date} {
	set struct {a {*date ?}}
	set try {a {729152 0}}
	map_get -map $struct $try {a val}
} {729152 0}

test map_set-types {set time} {
	set struct {a {*time ?}}
	set try {}
	map_set -map $struct $try a {9 May 1997 12:30:24}
} {a {729152 45024000}}

test map_set-types {set time 2} {
	set struct {a {*time ?}}
	set try {}
	map_set -map $struct $try a {05/09/1997 12:30:24}
} {a {729152 45024000}}

test map_get-types {get time} {
	set struct {a {*time ?}}
	set try {a {729152 45024000}}
	map_get -map $struct $try a
} {9 May 1997 12:30:24}

test map_get-types {get time: check empty} {
	set struct {a {*time ?}}
	map_get -map $struct {} a
} {?}

test map_get-types {get time: check empty with val} {
	set struct {a {*time ?}}
	map_get -map $struct {} {a val}
} {?}

test map_get-types {get time: check empty with param} {
	set struct {a {*time ?}}
	map_get -map $struct {} {a "%t"}
} {?}

test map_get-types {get date: check empty with val} {
	set struct {a {*date ?}}
	map_get -map $struct {} {a val}
} {?}

test map_get-types {get date: check empty with param} {
	set struct {a {*date ?}}
	map_get -map $struct {} {a "%t"}
} {?}

test map_get-types {set time: check empty} {
	set struct {a {*time ?}}
	map_set -map {a {*time ?}} {} a ?
} {}

test map_get-types {get time with format} {
	set struct {a {*time ?}}
	set try {a {729152 45024000}}
	map_get -map $struct $try {a "%H:%M:%S %e %b %Y"}
} {12:30:24 9 May 1997}

test map_get-types {get time with format} {
	set struct {a {*time ?}}
	set try {a {729152 45024000}}
	map_get -map $struct $try {a val}
} {729152 45024000}

test map_set-types {proc get with field} {
	namespace eval ::Extral {
		proc getproc {map data field value} {
			return [list $field $value]
		}
	}
	set struct {a {*proc ?}}
	set try {a 11}
	map_get -map $struct $try {a b}
} {b 11}

test map_set-types {proc set: check data} {
	namespace eval ::Extral {
		proc setproc {map data oldvalue field value} {
			return [list $data $oldvalue $value]
		}
	}
	set struct {a {*proc t ?}}
	set try {a 8}
	map_set -data d -map $struct $try a 9
} {a {d 8 9}}

test map_get-types {proc get: test data} {
	namespace eval ::Extral {
		proc getproc {map data field value} {
			return [list $data $value]
		}
	}
	set struct {a {*proc ?}}
	set try {a 10}
	map_get -data d -map $struct $try a
} {d 10}

test map_get-types {quotes in any} {
	set dbstruct {t {*any {}}}
	set db {t { "try}}
	map_get -map $dbstruct $db t
} { "try}

#
#	list
#

test map_set-list {list set} {
	set struct {a {*list {*int ?}}}
	set try {}
	map_set -map $struct $try a {1 2 3}
} {a {1 2 3}}

test map_set-list {list parameter next} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	map_set -map $struct $try {a next} 4
} {a {1 2 3 4}}

test map_set-list {list parameter next from empty} {
	set struct {a {*list {*int ?}}}
	set try {a {}}
	map_set -map $struct $try {a next} 4
} {a 4}

test map_set-list {list parameter next from empty, no subtag} {
	set struct {*list {*int ?}}
	set try {}
	map_set -map $struct $try next 4
} {4}

test map_set-list {list parameter num} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	map_set -map $struct $try {a 1} 20
} {a {1 20 3}}

test map_set-list {list parameter: non existing num} {
	set struct {a {*list {*int ?}}}
	set try {}
	map_set -map $struct $try {a 2} 4
} {empty list at field "a"} error

test map_set-list {list parameter end} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	map_set -map $struct $try {a end} 30
} {a {1 2 30}}

test map_set-list {list error} {
	set struct {a {*list {*int ?}}}
	set try {}
	map_set -map $struct $try a {1 2 c}
} {expected integer but got "c" at field "a"} error

test map_set-list {struct in list: simple} {
	set struct {
		a {*list {
			a {*int ?}
			b {*int ?}
		}}
	}
	set try {}
	map_set -map $struct {} {a next a} 1
} {a {{a 1}}}

test map_set-list {struct in list} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1}}}
	map_set -map $struct $try {a "" b} {1 2 3}
} {a {{a 1 b 1} {b 2} {b 3}}}

test map_set-list {struct in list with parameter} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1} {a 2}}}
	map_set -map $struct $try {a 1 b} 2
} {a {{a 1} {a 2 b 2}}}

test map_set-list {struct in list with parameter 0} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1} {a 2}}}
	map_set -map $struct $try {a 0 b} 2
} {a {{a 1 b 2} {a 2}}}

test map_set-list {struct in list error} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {}
	map_set -map $struct $try {a "" b} {1 c 3}
} {expected integer but got "c" at field "b" at field "a"} error

#	list unset
#
test map_unset-list {list set} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	map_unset -map $struct $try a
} {}

test map_unset-list {list parameter num} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	map_unset -map $struct $try {a 1}
} {a {1 3}}

test map_unset-list {list parameter: non existing num} {
	set struct {a {*list {*int ?}}}
	set try {}
	map_unset -map $struct $try {a 2}
} {}

test map_unset-list {list parameter end} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3}}
	map_unset -map $struct $try {a end}
} {a {1 2}}

test map_unset-list {struct in list: simple} {
	set struct {
		a {*list {
			a {*int ?}
			b {*int ?}
		}}
	}
	set try {a {{a 1}}}
	map_unset -map $struct $try {a end a}
} {a {{}}}

test map_unset-list {struct in list} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1 b 1} {b 2} {b 3}}}
	map_unset -map $struct $try {a "" b}
} {a {{a 1} {} {}}}

test map_unset-list {struct in list with parameter} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1} {a 2 b 2}}}
	map_unset -map $struct $try {a 1 b}
} {a {{a 1} {a 2}}}

test map_unset-list {struct in list with parameters; gets empty} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1 b 1} {b 2} {b 3}}}
	map_unset -map $struct $try {a 1 b}
} {a {{a 1 b 1} {} {b 3}}}

test map_unset-list {struct in list with parameter 0} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	}}}
	set try {a {{a 1 b 2} {a 2}}}
	map_unset -map $struct $try {a 0 b}
} {a {{a 1} {a 2}}}

#	list get
#
test map_get-list {list test type, end index} {
	set struct {a {*list {*date ?}}}
	set try [map_set -map $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	map_get -map $struct $try {a end}
} {2 Jan 1998}

test map_set-list {set ints in list from empty} {
	set struct {a {*list {*int ?}}}
	map_set -map $struct {} a {1 2}
} {a {1 2}}

test map_set-list {set dates in list from empty} {
	set struct {a {*list {*date ?}}}
	map_set -map $struct {} a {{1 Jan 1998} {2 Jan 1998}}
} {a {{729389 0} {729390 0}}}

test map_get-list {list test type, 0 index} {
	set struct {a {*list {*date ?}}}
	set try [map_set -map $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	map_get -map $struct $try {a 0}
} {1 Jan 1998}

test map_get-list {list test type, 1 to end} {
	set struct {a {*list {*date ?}}}
	set try {a {1 2 3}}
	set try [map_set -map $struct $try a {{1 Jan 1998} {2 Jan 1998} {3 Jan 1998}}]
	map_get -map $struct $try {a {1 end}}
} {{2 Jan 1998} {3 Jan 1998}}

test map_get-list {list test type, 1 to end} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3 4}}
	expr {double([map_get -map $struct $try {a {0 end {lmath_sum}}}])}
} {10.0}

test map_get-list {list test type, 1 to end} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3 4}}
	map_get -map $struct $try {a {0 end {lmath_average}}}
} {2.5}

test map_get-list {list index out of range} {
	set struct {a {*list {*date ?}}}
	set try [map_set -map $struct {} a {{1 Jan 1998}}]
	map_get -map $struct $try {a 2}
} {}

test map_get-list {list index error: invalid command} {
	set struct {a {*list {*int ?}}}
	set try [map_set -map $struct {} a {1 2 4}]
	map_get -map $struct $try {a {1 2 3}}
} {invalid command name "3"} error

test map_get-list {list index out of range} {
	set struct {a {*list {*date ?}}}
	set try [map_set -map $struct {} a {{1 Jan 1998}}]
	map_get -map $struct $try {a {2 4}}
} {}

test map_get-list {list 2 indices out of range} {
	set struct {a {*list {*date ?}}}
	set try [map_set -map $struct {} a {{1 Jan 1998}}]
	map_get -map $struct $try {a {0 4}}
} {{1 Jan 1998}}

test map_get-list {list 2 indices} {
	set struct {a {*list {*date ?}}}
	set try [map_set -map $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	map_get -map $struct $try {a {0 end}}
} {{1 Jan 1998} {2 Jan 1998}}

test map_get-list {list 2 indices} {
	set struct {a {*list {*date ?}}}
	set try [map_set -map $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	map_get -map $struct $try {a {0 1}}
} {{1 Jan 1998} {2 Jan 1998}}

test map_set-list {list regexp} {
	set struct {{? parts pts} {*list {*regexp ^a "does not start with an a" ?}}}
	set try {}
	map_set -map $struct $try pts {b}
} {error: "b" does not start with an a at field "pts"} error

test map_set-list {set empty list sub element} {
	set dbstruct {*list {a {*any {}}}}
	map_set -map $dbstruct {} next {}
} {{}}

test map_set-list {set empty list sub element} {
	set dbstruct {
	        {? article art} {
	            *list {
	                {? author a} {*any {}}
	            }
	        }
	}
	set db [map_set -map $dbstruct {} {art next} {}]
} {art {{}}}

test map_get-list {non empty default} {
	set struct {*list {*any {}}} 
	map_get -map $struct {} {}
} {}

test map_set-list {non empty default} {
	set struct {*list {*any {}}} 
	set data b
	map_set -map $struct $data 0 a
} {a}

#
#	named
#

test map_set-named {named set} {
	set struct {*named {{? try t} {*int ?}}}
	set try {a {t 1}}
	map_set -map $struct $try b {t 2}
} {a {t 1} b {t 2}}

test map_set-named {named set existing to default} {
	set struct {*named {{? try t} {*int ?}}}
	set try {a {t 1}}
	map_set -map $struct $try a {try ?}
} {}

test map_set-named {named set to default} {
	set struct {*named {{? try t} {*int ?}}}
	set try {a {t 1}}
	map_set -map $struct $try b {try 2} a {try ?}
} {b {t 2}}

test map_set-named {named set to empty} {
	set struct {t {*named {*int ?}}}
	set try {t {a 1}}
	map_set -map $struct $try t {}
} {t {a 1}}

test map_set-named {named set to default 2} {
	set struct {t {*named {*int ?}}}
	set try {t {a 1}}
	map_set -map $struct $try t {a ?}
} {}

test map_set-named {check empty list with named} {
	set struct {auth {*list {*named {*any {}}}}}
	map_get -map $struct {} {auth end name}
} {}

test map_get-named {check for non existing error} {
	proc ::Extral::getproc {map data field value} {
		return [list $field $value]
	}
	set struct {
		auth {
			*named {*proc Auth {}}
		}
		jou {*link Jou {}}
	}
	set data {auth {1 auth1.Auth}}
	map_get -map $struct $data {auth 2 name}
} {name {}}

test map_get-named {get all from empty} {
	set schema {nums {*named {*any ?}}}
	map_get -map $schema {} {}
} {nums {}}

test map_get-named {get all from empty tag before named} {
	set schema {nums {*named {*any ?}}}
	map_get -map $schema {} nums
} {}

test map_get-named {get all from empty tag in named} {
	set schema {nums {*named {*any ?}}}
	map_get -map $schema {} {nums a}
} ?

test map_get-named {with bool} {
	set schema {{? bool b} {*named {*bool 0}}}
	map_set -map $schema {} {bool gvf} 1
} {b {gvf 1}}

testsummarize
