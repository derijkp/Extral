#!/usr/local/bin/tclsh8.0
source tools.tcl

test taglset-struct {any} {
	set struct {a {*any ?}}
	set try {}
	taglset -struct $struct $try a {try 10}
} {a {try 10}}

test taglset-struct {int} {
	set struct {a {*int ?}}
	set try {}
	taglset -struct $struct $try a 10
} {a 10}

test taglset-struct {int error: give string} {
	set struct {a {*int ?}}
	set try {}
	taglset -struct $struct $try a try
} {expected integer but got "try"} 1

test taglset-struct {int error: give double} {
	set struct {a {*int ?}}
	set try {}
	taglset -struct $struct $try a 10.2
} {expected integer but got "10.2"} 1

test taglset-struct {double} {
	set struct {a {*double ?}}
	set try {}
	taglset -struct $struct $try a 10.2
} {a 10.2}

test taglset-struct {double: give int} {
	set struct {a {*double ?}}
	set try {}
	taglset -struct $struct $try a 10
} {a 10}

test taglset-struct {double error: give string} {
	set struct {a {*double ?}}
	set try {}
	taglset -struct $struct $try a try
} {expected floating-point number but got "try"} 1

test taglset-struct {bool} {
	set struct {a {*bool ?}}
	set try {}
	taglset -struct $struct $try a true
} {a 1}

test taglset-struct {bool} {
	set struct {a {*bool ?}}
	set try {}
	taglset -struct $struct $try a no
} {a 0}

test taglset-struct {bool error} {
	set struct {a {*bool ?}}
	set try {}
	taglset -struct $struct $try a try
} {expected boolean value but got "try"} 1

test taglset-struct {regexp} {
	set struct {a {*regexp ^a ?}}
	set try {}
	taglset -struct $struct $try a a10
} {a a10}

test taglset-struct {regexp: one of} {
	set struct {a {*regexp ^try|any|yes|no$ ?}}
	set try {}
	taglset -struct $struct $try a any
} {a any}

test taglset-struct {regexp: one of error} {
	set struct {a {*regexp ^try|any|yes|no$ ?}}
	set try {}
	taglset -struct $struct $try a test
} {error: "test" does not match pattern "^try|any|yes|no$"} 1

test taglset-struct {regexp error} {
	set struct {a {*regexp ^a ?}}
	set try {}
	taglset -struct $struct $try a try
} {error: "try" does not match pattern "^a"} 1

test taglset-struct {regexp: not enough arguments} {
	set struct {a {*regexp}}
	set try {}
	taglset -struct $struct $try a try
} {error: wrong number of arguments in structure "*regexp"} 1

test taglset-struct {between: ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	taglset -struct $struct $try a 5
} {a 5}

test taglset-struct {between: lower ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	taglset -struct $struct $try a 0
} {a 0}

test taglset-struct {between: higher ok} {
	set struct {a {*between 0 10 ?}}
	set try {}
	taglset -struct $struct $try a 10
} {a 10}

test taglset-struct {between: too low} {
	set struct {a {*between 0 10 ?}}
	set try {}
	taglset -struct $struct $try a -1
} {error: -1 is not between 0 and 10} 1

test taglset-struct {between: too high} {
	set struct {a {*between 0 10 ?}}
	set try {}
	taglset -struct $struct $try a 11
} {error: 11 is not between 0 and 10} 1

test taglset-struct {dbetween: ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	taglset -struct $struct $try a 1.2
} {a 1.2}

test taglset-struct {dbetween: lower ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	taglset -struct $struct $try a 0.5
} {a 0.5}

test taglset-struct {dbetween: higher ok} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	taglset -struct $struct $try a 1.8
} {a 1.8}

test taglset-struct {dbetween: too low} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	taglset -struct $struct $try a 0
} {error: 0 is not between 0.5 and 1.8} 1

test taglset-struct {dbetween: too high} {
	set struct {a {*dbetween 0.5 1.8 ?}}
	set try {}
	taglset -struct $struct $try a 11
} {error: 11 is not between 0.5 and 1.8} 1

test taglset-struct {proc} {
	namespace eval ::Extral {
		proc setproc {default value} {
			return "try:$value"
		}
	}
	set struct {a {*proc ?}}
	set try {}
	taglset -struct $struct $try a 11
} {a try:11}

test taglset-struct {proc with argument} {
	namespace eval ::Extral {
		proc setproc {arg default value} {
			return "$arg:$value"
		}
	}
	set struct {a {*proc t ?}}
	set try {}
	taglset -struct $struct $try a 11
} {a t:11}

test taglget-struct {proc} {
	namespace eval ::Extral {
		proc getproc {default value} {
			return "try:$value"
		}
	}
	set struct {a {*proc ?}}
	set try {a 10}
	taglget -struct $struct $try a
} {try:10}

test taglget-struct {proc with argument} {
	namespace eval ::Extral {
		proc getproc {arg default value} {
			return "$arg:$value"
		}
	}
	set struct {a {*proc t ?}}
	set try {a 10}
	taglget -struct $struct $try a
} {a t:10}

test taglset-struct {date} {
	set struct {a {*date t ?}}
	set try {}
	taglset -struct $struct $try a 10
} {try}

test taglget-struct {date} {
	set struct {a {*date t ?}}
	set try {a 10}
	taglget -struct $struct $try a
} {try}


testsummarize
