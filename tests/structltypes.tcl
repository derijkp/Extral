#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test structlset-struct {any} {
	set struct {a {*any ?}}
	set try {}
	structlset -struct $struct $try a {try 10}
} {a {try 10}}

test structlset-struct {int} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try a 10
} {a 10}

test structlset-struct {int error: give string} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try a try
} {expected integer but got "try"} 1

test structlset-struct {int error: give double} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try a 10.2
} {expected integer but got "10.2"} 1

test structlset-struct {double} {
	set struct {a {*double ?}}
	set try {}
	structlset -struct $struct $try a 10.2
} {a 10.2}

test structlset-struct {double: give int} {
	set struct {a {*double ?}}
	set try {}
	structlset -struct $struct $try a 10
} {a 10}

test structlset-struct {double error: give string} {
	set struct {a {*double ?}}
	set try {}
	structlset -struct $struct $try a try
} {expected floating-point number but got "try"} 1

test structlset-struct {bool} {
	set struct {a {*bool ?}}
	set try {}
	structlset -struct $struct $try a true
} {a 1}

test structlset-struct {bool} {
	set struct {a {*bool ?}}
	set try {}
	structlset -struct $struct $try a no
} {a 0}

test structlset-struct {bool error} {
	set struct {a {*bool ?}}
	set try {}
	structlset -struct $struct $try a try
} {expected boolean value but got "try"} 1

test structlset-struct {regexp} {
	set struct {a {*regexp ^a ?}}
	set try {}
	structlset -struct $struct $try a a10
} {a a10}

test structlset-struct {regexp: one of} {
	set struct {a {*regexp ^try|any|yes|no$ ?}}
	set try {}
	structlset -struct $struct $try a any
} {a any}

test structlset-struct {regexp: one of error} {
	set struct {a {*regexp ^try|any|yes|no$ ?}}
	set try {}
	structlset -struct $struct $try a test
} {error: "test" does not match pattern "^try|any|yes|no$"} 1

test structlset-struct {regexp error} {
	set struct {a {*regexp ^a ?}}
	set try {}
	structlset -struct $struct $try a try
} {error: "try" does not match pattern "^a"} 1

test structlset-struct {regexp: not enough arguments} {
	set struct {a {*regexp}}
	set try {}
	structlset -struct $struct $try a try
} {error: wrong number of arguments in structure "*regexp"} 1

test structlset-struct {between: ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlset -struct $struct $try a 5
} {a 5}

test structlset-struct {between: lower ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlset -struct $struct $try a 0
} {a 0}

test structlset-struct {between: higher ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlset -struct $struct $try a 10
} {a 10}

test structlset-struct {between: too low} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlset -struct $struct $try a -1
} {error: -1 is not between 0 and 10} 1

test structlset-struct {between: too high} {
	set struct {a {*between 0 10 ?}}
	set try {}
	structlset -struct $struct $try a 11
} {error: 11 is not between 0 and 10} 1

test structlset-struct {dbetween: ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlset -struct $struct $try a 1.2
} {a 1.2}

test structlset-struct {dbetween: lower ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlset -struct $struct $try a 0.5
} {a 0.5}

test structlset-struct {dbetween: higher ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlset -struct $struct $try a 1.8
} {a 1.8}

test structlset-struct {dbetween: too low} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlset -struct $struct $try a 0
} {error: 0 is not between 0.5 and 1.8} 1

test structlset-struct {dbetween: too high} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	structlset -struct $struct $try a 11
} {error: 11 is not between 0.5 and 1.8} 1

test structlset-struct {proc} {
	namespace eval ::Extral {
		proc setproc {default value} {
			return "try:$value"
		}
	}
	set struct {a {*proc ?}}
	set try {}
	structlset -struct $struct $try a 11
} {a try:11}

test structlset-struct {proc with argument: set} {
	namespace eval ::Extral {
		proc setproc {arg default value} {
			return "$arg:$value"
		}
	}
	set struct {a {*proc t ?}}
	set try {}
	structlset -struct $struct $try a 11
} {a t:11}

test structlget-struct {proc} {
	namespace eval ::Extral {
		proc getproc {default value} {
			return "try:$value"
		}
	}
	set struct {a {*proc ?}}
	set try {a 10}
	structlget -struct $struct $try a
} {try:10}

test structlget-struct {proc with argument: get} {
	namespace eval ::Extral {
		proc getproc {arg default value} {
			return "$arg:$value"
		}
	}
	set struct {a {*proc t ?}}
	set try {a 10}
	structlget -struct $struct $try a
} {t:10}

test structlset-struct {set date} {
	set struct {a {*date t ?}}
	set try {}
	structlset -struct $struct $try a {9 May 1997}
} {a 62998732800.0}

test structlset-struct {set date 2} {
	set struct {a {*date t ?}}
	set try {}
	structlset -struct $struct $try a {05/09/1997}
} {a 62998732800.0}

test structlget-struct {get date} {
	set struct {a {*date t ?}}
	set try {a 62998732800.0}
	structlget -struct $struct $try a
} {9 May 1997}

test structlset-struct {set time} {
	set struct {a {*time t ?}}
	set try {}
	structlset -struct $struct $try a {9 May 1997 12:30:24}
} {a 62998777824.0}

test structlset-struct {set time 2} {
	set struct {a {*time t ?}}
	set try {}
	structlset -struct $struct $try a {05/09/1997 12:30:24}
} {a 62998777824.0}

test structlget-struct {get time} {
	set struct {a {*time t ?}}
	set try {a 62998777824.0}
	structlget -struct $struct $try a
} {9 May 1997 12:30:24}


testsummarize
