#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test map_set {set} {
	map_set {a 1 bb 2 ccc 3} bb try
} {a 1 bb try ccc 3}

test map_set {set, list check} {
	map_set {a 1 bb 2 ccc 3} bb {try it}
} {a 1 bb {try it} ccc 3}

test map_set {set, 2 tags} {
	map_set {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c a} try
} {a 1 b 2 c {a try b 2 c {a 1 b 2}}}

test map_set {set new} {
	map_set {a 1 bb 2 ccc 3} dddd 4
} {a 1 bb 2 ccc 3 dddd 4}

test map_set {set new, list value} {
	map_set {a 1 bb 2 ccc 3} dddd {try it}
} {a 1 bb 2 ccc 3 dddd {try it}}

test map_set {set new, list tag, list value} {
	map_set {a 1 bb 2 ccc 3} {{d d}} {try it}
} {a 1 bb 2 ccc 3 {d d} {try it}}

test map_set {set new, 2 tags} {
	map_set {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c d} try
} {a 1 b 2 c {a 1 b 2 c {a 1 b 2} d try}}

test map_set {set, 3 tags} {
	map_set {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c c a} try
} {a 1 b 2 c {a 1 b 2 c {a try b 2}}}

test map_set {set multi} {
	map_set {a 1} b 2 a a
} {a a b 2}

test map_set {set multi multiple levels} {
	map_set {a 1} {b a} 2 a a {b b} 3
} {a a b {a 2 b 3}}

test map_set {check uneven} {
	map_set {a 1 bb 2 ccc} dddd 4
} {error: "a 1 bb 2 ccc" does not have an even number of elements} 1

test map_set {check uneven, 2 tags} {
	map_set {a 1 b 2 c {a 1 b}} {c a} 4
} {error: "a 1 b" does not have an even number of elements} 1

test map_set {empty list} {
	map_set {} a 1
} {a 1}

test map_set {empty field} {
	map_set {} {} {a 1}
} {a 1}

test map_set {empty sublist} {
	map_set {a {}} {a b} 1
} {a {b 1}}

test map_set {check for object errors in C code} {
	set try {a 1 bb 2 ccc 3}
	map_set $try bb try
	set try 
} {a 1 bb 2 ccc 3}

test map_set {multiple tags not found} {
	map_set {a 1 bb 2} {c a b} 1
} {a 1 bb 2 c {a {b 1}}}

test map_set {with space} {
	map_set "" {Fonts {Basic Fonts} BoldItalicFont} *Font
} {Fonts {{Basic Fonts} {BoldItalicFont *Font}}}

test map_set {with space} {
	map_set "" {Fonts {Basic Fonts} {other font}} *Font
} {Fonts {{Basic Fonts} {{other font} *Font}}}

test map_get {} {
	map_get {a 1 bb 2 ccc 3} bb
} {2}

test map_get {with map} {
	map_get {a 1 bb {a bb1 b bb2} ccc 3} {bb b}
} {bb2}

test map_get {with larger map} {
	map_get {a 1 b {a {a baa b bab} b try}} {b a b}
} {bab}

test map_get {tag not present} {
	map_get {a 1 bb 2 ccc 3} e
} {tag "e" not found} 1

test map_get {field larger than map} {
	map_get {a 1 bb 2 ccc 3} {a b}
} {error: list "1" does not have an even number of elements} 1

test map_get {field in struct not present} {
	map_get {a {a 1 b 2} bb 2 ccc 3} {a c}
} {tag "c" not found} 1

test map_get {get partial} {
	map_get {a {a 1 b 2} bb 2 ccc 3} a
} {a 1 b 2}

test map_get {check uneven} {
	map_get {a 1 bb 2 ccc} bb
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test map_get {def} {
	map_get {a 1 b 2} b a
} {2 1}

test map_get {not found} {
	map_get {a 1} b
} {tag "b" not found} 1

test map_unset {} {
	map_unset {a 1 bb 2 ccc 3} bb
} {a 1 ccc 3}

test map_unset {not present} {
	map_unset {a 1 bb 2 ccc 3} d
} {a 1 bb 2 ccc 3}

test map_unset {unset 2} {
	map_unset {a 1 bb 2 ccc 3} bb ccc
} {a 1}

test map_unset {check uneven} {
	map_unset {a 1 bb 2 ccc} bb
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test map_unset {present, 2 tags} {
	map_unset {a 1 b 2 c {a 1 b 2}} {c a}
} {a 1 b 2 c {b 2}}

test map_unset {not present, 2 tags} {
	map_unset {a 1 b 2 c {a 1 b 2}} {c c}
} {a 1 b 2 c {a 1 b 2}}

test map_unset {unset, 3 tags} {
	map_unset {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}} {c c a}
} {a 1 b 2 c {a 1 b 2 c {b 2}}}

test map_unset {check bug} {
	map_unset {a 1 b 2} a 1
} {b 2}

test map_fields {} {
	map_fields {a 1 bb 2 ccc 3}
} {a bb ccc}

test map_fields {see values} {
	map_fields {a 1 bb 2 ccc 3} {} values
	set values
} {1 2 3}

test map_fields {see values} {
	map_fields {a {a 1 b 2 c 3}} a
} {a b c}

test map_fields {check uneven} {
	map_fields {a 1 bb 2 ccc}
} {error: list "a 1 bb 2 ccc" does not have an even number of elements} 1

test map_find {present} {
	map_find {a try1 bb try2 ccc try3} bb
} 3

test map_find {not present} {
	map_find {a try1 bb try2 ccc try3} try
} -1

test map_find {bugfix} {
	map_fields {File {{Open file} {} {Open next} {} Test {} Trying {Trying {} Trying2 {}} Save {} {} {{} {{} {}}} {Radio try} {} {Radio try2} {}} Find {{Goto line} {} Find {} separator1 {} {Replace & Find next} {} {Search Reopen} {}} Test {}} File
} {{Open file} {Open next} Test Trying Save {} {Radio try} {Radio try2}}

test map_find {bugfix} {
	map_fields {File {Save {} {} {{} {{} {}}}} Find {{Goto line} {}} Test {}} File
} {Save {}}

test map_find {bugfix empty ke} {
	map_fields {{} {}} {}
} {{}}

testsummarize

# no test yet for
# list_load <filename>
# list_write ?file? ?list?
