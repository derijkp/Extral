#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test structlist_set {set} {
	structlist_set {a 1 bb 2 ccc 3} bb try
} {a 1 bb try ccc 3}

test structlist_set {set, list check} {
	structlist_set {a 1 bb 2 ccc 3} bb {try it}
} {a 1 bb {try it} ccc 3}

test structlist_set {set, 2 tags} {
	structlist_set {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c a} try
} {a 1 b 2 c {a try b 2 c {a 1 b 2}}}

test structlist_set {set new} {
	structlist_set {a 1 bb 2 ccc 3} dddd 4
} {a 1 bb 2 ccc 3 dddd 4}

test structlist_set {set new, list value} {
	structlist_set {a 1 bb 2 ccc 3} dddd {try it}
} {a 1 bb 2 ccc 3 dddd {try it}}

test structlist_set {set new, list tag, list value} {
	structlist_set {a 1 bb 2 ccc 3} {{d d}} {try it}
} {a 1 bb 2 ccc 3 {d d} {try it}}

test structlist_set {set new, 2 tags} {
	structlist_set {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c d} try
} {a 1 b 2 c {a 1 b 2 c {a 1 b 2} d try}}

test structlist_set {set, 3 tags} {
	structlist_set {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c c a} try
} {a 1 b 2 c {a 1 b 2 c {a try b 2}}}

test structlist_set {set multi} {
	structlist_set {a 1} b 2 a a
} {a a b 2}

test structlist_set {set multi multiple levels} {
	structlist_set {a 1} {b a} 2 a a {b b} 3
} {a a b {a 2 b 3}}

test structlist_set {check uneven} {
	structlist_set {a 1 bb 2 ccc} dddd 4
} {error: "a 1 bb 2 ccc" does not have an even number of elements} 1

test structlist_set {check uneven, 2 tags} {
	structlist_set {a 1 b 2 c {a 1 b}} {c a} 4
} {error: "a 1 b" does not have an even number of elements} 1

test structlist_set {empty list} {
	structlist_set {} a 1
} {a 1}

test structlist_set {empty field} {
	structlist_set {} {} {a 1}
} {a 1}

test structlist_set {empty sublist} {
	structlist_set {a {}} {a b} 1
} {a {b 1}}

test structlist_set {check for object errors in C code} {
	set try {a 1 bb 2 ccc 3}
	structlist_set $try bb try
	set try 
} {a 1 bb 2 ccc 3}

test structlist_set {multiple tags not found} {
	structlist_set {a 1 bb 2} {c a b} 1
} {a 1 bb 2 c {a {b 1}}}

test structlist_set {with space} {
	structlist_set "" {Fonts {Basic Fonts} BoldItalicFont} *Font
} {Fonts {{Basic Fonts} {BoldItalicFont *Font}}}

test structlist_set {with space} {
	structlist_set "" {Fonts {Basic Fonts} {other font}} *Font
} {Fonts {{Basic Fonts} {{other font} *Font}}}

test structlist_get {} {
	structlist_get {a 1 bb 2 ccc 3} bb
} {2}

test structlist_get {with structure} {
	structlist_get {a 1 bb {a bb1 b bb2} ccc 3} {bb b}
} {bb2}

test structlist_get {with larger structure} {
	structlist_get {a 1 b {a {a baa b bab} b try}} {b a b}
} {bab}

test structlist_get {tag not present} {
	structlist_get {a 1 bb 2 ccc 3} e
} {tag "e" not found} 1

test structlist_get {field larger than structure} {
	structlist_get {a 1 bb 2 ccc 3} {a b}
} {error: list "1" does not have an even number of elements} 1

test structlist_get {field in struct not present} {
	structlist_get {a {a 1 b 2} bb 2 ccc 3} {a c}
} {tag "c" not found} 1

test structlist_get {get partial} {
	structlist_get {a {a 1 b 2} bb 2 ccc 3} a
} {a 1 b 2}

test structlist_get {check uneven} {
	structlist_get {a 1 bb 2 ccc} bb
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test structlist_get {def} {
	structlist_get {a 1 b 2} b a
} {2 1}

test structlist_get {not found} {
	structlist_get {a 1} b
} {tag "b" not found} 1

test structlist_unset {} {
	structlist_unset {a 1 bb 2 ccc 3} bb
} {a 1 ccc 3}

test structlist_unset {not present} {
	structlist_unset {a 1 bb 2 ccc 3} d
} {a 1 bb 2 ccc 3}

test structlist_unset {unset 2} {
	structlist_unset {a 1 bb 2 ccc 3} bb ccc
} {a 1}

test structlist_unset {check uneven} {
	structlist_unset {a 1 bb 2 ccc} bb
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test structlist_unset {present, 2 tags} {
	structlist_unset {a 1 b 2 c {a 1 b 2}} {c a}
} {a 1 b 2 c {b 2}}

test structlist_unset {not present, 2 tags} {
	structlist_unset {a 1 b 2 c {a 1 b 2}} {c c}
} {a 1 b 2 c {a 1 b 2}}

test structlist_unset {unset, 3 tags} {
	structlist_unset {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c c a}
} {a 1 b 2 c {a 1 b 2 c {b 2}}}

test structlist_unset {check bug} {
	structlist_unset {a 1 b 2} a 1
} {b 2}

test structlist_fields {} {
	structlist_fields {a 1 bb 2 ccc 3}
} {a bb ccc}

test structlist_fields {see values} {
	structlist_fields {a 1 bb 2 ccc 3} {} values
	set values
} {1 2 3}

test structlist_fields {see values} {
	structlist_fields {a {a 1 b 2 c 3}} a
} {a b c}

test structlist_fields {check uneven} {
	structlist_fields {a 1 bb 2 ccc}
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test structlist_find {present} {
	structlist_find {a try1 bb try2 ccc try3} bb
} 3

test structlist_find {not present} {
	structlist_find {a try1 bb try2 ccc try3} try
} -1

testsummarize

# no test yet for
# list_load <filename>
# list_write ?file? ?list?
