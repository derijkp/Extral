#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

auto_load scantime
auto_load formattime

test structlset-types {any} {
	set struct {a {*any ?}}
	set try {}
	structlset -struct $struct $try a {try 10}
} {a {try 10}}

test structlset-types {int} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try a 10
} {a 10}

test structlset-types {int error: give string} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try a try
} {expected integer but got "try"} 1

test structlset-types {int error: give double} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try a 10.2
} {expected integer but got "10.2"} 1

test structlset-types {double} {
	set struct {a {*double ?}}
	set try {}
	structlset -struct $struct $try a 10.2
} {a 10.2}

test structlset-types {double: give int} {
	set struct {a {*double ?}}
	set try {}
	structlset -struct $struct $try a 10
} {a 10}

test structlset-types {double error: give string} {
	set struct {a {*double ?}}
	set try {}
	structlset -struct $struct $try a try
} {expected floating-point number but got "try"} 1

test structlset-types {bool} {
	set struct {a {*bool ?}}
	set try {}
	structlset -struct $struct $try a true
} {a 1}

test structlset-types {bool} {
	set struct {a {*bool ?}}
	set try {}
	structlset -struct $struct $try a no
} {a 0}

test structlset-types {bool error} {
	set struct {a {*bool ?}}
	set try {}
	structlset -struct $struct $try a try
} {expected boolean value but got "try"} 1

test structlset-types {regexp} {
	set struct {a {*regexp ^a "does not start with an a" ?}}
	set try {}
	structlset -struct $struct $try a a10
} {a a10}

test structlset-types {regexp error} {
	set struct {a {*regexp ^a "does not start with an a" ?}}
	set try {}
	structlset -struct $struct $try a b10
} {error: "b10" does not start with an a} 1

test structlset-types {regexp: one of} {
	set struct {a {*regexp ^try|any|yes|no$ "is not one of try, any, yes or no" ?}}
	set try {}
	structlset -struct $struct $try a any
} {a any}

test structlset-types {regexp: one of error} {
	set struct {a {*regexp ^try|any|yes|no$ "is not one of try, any, yes or no" ?}}
	set try {}
	structlset -struct $struct $try a test
} {error: "test" is not one of try, any, yes or no} 1

test structlset-types {regexp: not enough arguments} {
	set struct {a {*regexp}}
	set try {}
	structlset -struct $struct $try a try
} {error: wrong number of arguments in structure "*regexp"} 1

test structlset-types {between: ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlset -struct $struct $try a 5
} {a 5}

test structlset-types {between: lower ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlset -struct $struct $try a 0
} {a 0}

test structlset-types {between: higher ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlset -struct $struct $try a 10
} {a 10}

test structlset-types {between: too low} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlset -struct $struct $try a -1
} {error: -1 is not between 0 and 10} 1

test structlset-types {between: too high} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlset -struct $struct $try a 11
} {error: 11 is not between 0 and 10} 1

test structlset-types {dbetween: ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlset -struct $struct $try a 1.2
} {a 1.2}

test structlset-types {dbetween: lower ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlset -struct $struct $try a 0.5
} {a 0.5}

test structlset-types {dbetween: higher ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlset -struct $struct $try a 1.8
} {a 1.8}

test structlset-types {dbetween: too low} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlset -struct $struct $try a 0
} {error: 0 is not between 0.5 and 1.8} 1

test structlset-types {dbetween: too high} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlset -struct $struct $try a 11
} {error: 11 is not between 0.5 and 1.8} 1

test structlset-types {proc} {
	namespace eval ::Extral {
		proc setproc {stucture oldvalue field value} {
			return "try:$value"
		}
	}
	set struct {a {*proc ?}}
	set try {}
	structlset -struct $struct $try a 11
} {a try:11}

test structlset-types {proc with argument: set} {
	namespace eval ::Extral {
		proc setproc {structure oldvalue field value} {
			return "[lindex $structure 1]:$value"
		}
	}
	set struct {a {*proc t ?}}
	set try {}
	structlset -struct $struct $try a 11
} {a t:11}

test structlset-types {proc set: use oldvalue} {
	namespace eval ::Extral {
		proc setproc {structure oldvalue field value} {
			return "$oldvalue -> $value"
		}
	}
	set struct {a {*proc t ?}}
	set try {a 8}
	structlset -struct $struct $try a 9
} {a {8 -> 9}}

test structlget-types {get int} {
	set struct {a {*int ?}}
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
		proc getproc {structure field value} {
			return "try:$value"
		}
	}
	set struct {a {*proc ?}}
	set try {a 10}
	structlget -struct $struct $try a
} {try:10}

test structlget-types {proc get with argument} {
	namespace eval ::Extral {
		proc getproc {structure field value} {
			return "[lindex $structure 1]:$value"
		}
	}
	set struct {a {*proc t ?}}
	set try {a 10}
	structlget -struct $struct $try a
} {t:10}

test structlset-types {set date} {
	set struct {a {*date t ?}}
	set try {}
	structlset -struct $struct $try a {9 May 1997}
} {a 62998732800.0}

test structlset-types {set date 2} {
	set struct {a {*date t ?}}
	set try {}
	structlset -struct $struct $try a {05/09/1997}
} {a 62998732800.0}

test structlget-types {get date} {
	set struct {a {*date t ?}}
	set try {a 62998732800.0}
	structlget -struct $struct $try a
} {9 May 1997}

test structlset-types {set time} {
	set struct {a {*time t ?}}
	set try {}
	structlset -struct $struct $try a {9 May 1997 12:30:24}
} {a 62998777824.0}

test structlset-types {set time 2} {
	set struct {a {*time t ?}}
	set try {}
	structlset -struct $struct $try a {05/09/1997 12:30:24}
} {a 62998777824.0}

test structlget-types {get time} {
	set struct {a {*time t ?}}
	set try {a 62998777824.0}
	structlget -struct $struct $try a
} {9 May 1997 12:30:24}

test structlset-types {proc get with field} {
	namespace eval ::Extral {
		proc getproc {structure field value} {
			return [list $field $value]
		}
	}
	set struct {a {*proc ?}}
	set try {a 11}
	structlget -struct $struct $try {a b}
} {b 11}

test structlget-types {check for non existing error} {
	proc ::Extral::getproc {stucture field value} {
		return [list $field $value]
	}
	set struct {
		auth {
			* {*proc Auth {}}
		}
		jou {*link Jou {}}
	}
	set data {auth {1 auth1.Auth}}
	structlget -struct $struct $data {auth 2 name}
} {name {}}

test structlset-types {list} {
	set struct {a {*list {*int ?} {}}}
	set try {}
	structlset -struct $struct $try a {1 2 3}
} {a {1 2 3}}

test structlset-types {list parameter next} {
	set struct {a {*list {*int ?} {}}}
	set try {a {1 2 3}}
	structlset -struct $struct $try {a next} 4
} {a {1 2 3 4}}

test structlset-types {list parameter num} {
	set struct {a {*list {*int ?} {}}}
	set try {a {1 2 3}}
	structlset -struct $struct $try {a 1} 20
} {a {1 20 3}}

test structlset-types {list parameter: non existing num} {
	set struct {a {*list {*int ?} {}}}
	set try {}
	Extral::structlset -struct $struct $try {a 2} 4
} {empty list} 1

test structlset-types {list parameter end} {
	set struct {a {*list {*int ?} {}}}
	set try {a {1 2 3}}
	structlset -struct $struct $try {a end} 30
} {a {1 2 30}}

test structlset-types {list error} {
	set struct {a {*list {*int ?} {}}}
	set try {}
	structlset -struct $struct $try a {1 2 c}
} {expected integer but got "c"} 1

test structlset-types {struct in list: simple} {
	set struct {
		a {*list {
			a {*int ?}
			b {*int ?}
		} {}}
	}
	set try {}
	Extral::structlset -struct $struct {} {a next a} 1
} {a {{a 1}}}

test structlset-types {struct in list} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	} {}}}
	set try {a {{a 1}}}
	Extral::structlset -struct $struct $try {a "" b} {1 2 3}
} {a {{a 1 b 1} {b 2} {b 3}}}

test structlset-types {struct in list with parameter} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	} {}}}
	set try {a {{a 1} {a 2}}}
	Extral::structlset -struct $struct $try {a 1 b} 2
} {a {{a 1} {a 2 b 2}}}

test structlset-types {struct in list with parameter 0} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	} {}}}
	set try {a {{a 1} {a 2}}}
	Extral::structlset -struct $struct $try {a 0 b} 2
} {a {{a 1 b 2} {a 2}}}

test structlset-types {struct in list error} {
	set struct {a {*list {
		a {*int ?}
		b {*int ?}
	} {}}}
	set try {}
	Extral::structlset -struct $struct $try {a "" b} {1 c 3}
} {expected integer but got "c"} 1

test structlget-types {list test type, end index} {
	set struct {a {*list {*date ?} {}}}
	set try [structlset -struct $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	structlget -struct $struct $try {a end}
} {2 Jan 1998}

test structlget-types {list test type, 0 index} {
	set struct {a {*list {*date ?} {}}}
	set try [structlset -struct $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	Extral::structlget -struct $struct $try {a 0}
} {1 Jan 1998}

test structlget-types {list test type, 1 to end} {
	set struct {a {*list {*date ?} {}}}
	set try {a {1 2 3}}
	set try [structlset -struct $struct $try a {{1 Jan 1998} {2 Jan 1998} {3 Jan 1998}}]
	structlget -struct $struct $try {a {1 end}}
} {{2 Jan 1998} {3 Jan 1998}}

test structlget-types {list index error} {
	set struct {a {*list {*date ?} {}}}
	set try [structlset -struct $struct {} a {{1 Jan 1998}}]
	structlget -struct $struct $try {a 2}
} {list doesn't contain element 2} 1

test structlget-types {list index error: invalid argument} {
	set struct {a {*list {*date ?} {}}}
	set try [structlset -struct $struct {} a {{1 Jan 1998}}]
	structlget -struct $struct $try {a {1 2 3}}
} {wrong # args to list: "1 2 3"} 1

test structlget-types {list index error} {
	set struct {a {*list {*date ?} {}}}
	set try [structlset -struct $struct {} a {{1 Jan 1998}}]
	structlget -struct $struct $try {a {2 4}}
} {list doesn't contain element 2} 1

test structlget-types {list 2 indices error} {
	set struct {a {*list {*date ?} {}}}
	set try [structlset -struct $struct {} a {{1 Jan 1998}}]
	structlget -struct $struct $try {a {0 4}}
} {list doesn't contain element 4} 1

test structlget-types {list 2 indices} {
	set struct {a {*list {*date ?} {}}}
	set try [structlset -struct $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	structlget -struct $struct $try {a {0 end}}
} {{1 Jan 1998} {2 Jan 1998}}

test structlget-types {list 2 indices} {
	set struct {a {*list {*date ?} {}}}
	set try [structlset -struct $struct {} a {{1 Jan 1998} {2 Jan 1998}}]
	structlget -struct $struct $try {a {0 1}}
} {{1 Jan 1998} {2 Jan 1998}}

testsummarize
