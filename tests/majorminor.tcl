#!/usr/local/bin/tclsh8.0
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
} {} 1

test majorminor {basic: non existing minor} {
	major try
	minor try do {a} {
		return $a
	}
	try try 1
} {} 1


testsummarize
