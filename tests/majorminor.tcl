#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test majorminor {basic} {
	major try
	minor try do {a} {
		return $a
	}
	try do 1
} {1}

test majorminor {basic: wrong # of arguments} {
	major try
	minor try do {a} {
		return $a
	}
	try do 1 1
} {called "try::do" with too many arguments} 1

test majorminor {basic: non existing minor} {
	major try
	minor try do {a} {
		return $a
	}
	try try 1
} {invalid command name "try::try"} 1


testsummarize
