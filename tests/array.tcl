#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test array_lappend {basic} {
	array set try {a a b b}
	array_lappend try {a 1 b 2}
	array get try
} {a {a 1} b {b 2}}

test array_lget {use def} {
	array set try {a 1 b 2}
	array_lget try {b c} def
} {2 def}

test array_lget {no def} {
	array set try {a 1 b 2 c 3}
	array_lget try {b c}
} {b 2 c 3}

test array_lget {use def 2} {
	array set try {a 1 b 2}
	array_lget try {b c}
} {b 2}

test array_trans {basic} {
	array set a {a 1 b 2 c 3 d 4}
	array_trans a {a c}
} {1 3}

test array_trans {basic with default} {
	array set a {a 1 b 2 c 3 d 4}
	array_trans a {a e} def
} {1 def}

testsummarize

