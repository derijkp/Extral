#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test majorminor {basic} {
	catch {deletemajor try}
	major try
	minor try do {a} {
		return $a
	}
	try do 1
} {1}

test majorminor {basic: wrong # of arguments} {
	catch {deletemajor try}
	major try
	minor try do {a} {
		return $a
	}
	try do 1 1
} {called "try do" with too many arguments} 1

test majorminor {basic: non existing minor} {
	catch {deletemajor try}
	major try
	minor try do {a} {
		return $a
	}
	try try 1
} {invalid command name "try try"} 1

test majorminor {redefine minor} {
	catch {deletemajor try}
	major try
	minor try do {a} {
		return $a
	}
	minor try do {a} {
		return [expr $a+1]
	}
	try do 1
} {2}

test majorminor {multi major} {
	catch {deletemajor try}
	major try
	major {try try}
	minor {try try} do {a} {
		return $a
	}
	try try do 1
} {2}


testsummarize
