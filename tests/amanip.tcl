#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test amanip {lappend} {
	array set try {a a b b}
	amanip lappend try {a 1 b 2}
	array get try
} {a {a 1} b {b 2}}

test amanip {get} {
	array set try {a 1 b 2}
	amanip get try {b c} def
} {2 def}

test amanip {get} {
	array set try {a 1 b 2 c 3}
	amanip get try {b c}
} {b 2 c 3}

test amanip {get} {
	array set try {a 1 b 2}
	amanip get try {b c}
} {b 2}

testsummarize
