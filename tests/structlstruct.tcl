#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test structlset-struct {basic} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try a 10
} {a 10}

test structlset-struct {basic: 2 tags} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try {a a} 10
} {a {a 10}}

test structlset-struct {basic: set to default} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try a ?
} {}

test structlset-struct {basic: set to default: 2 tags} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try {a a} ?
} {}

test structlset-struct {basic: set to default: remove old} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	structlset -struct $struct $try {a a} ?
} {}

test structlset-struct {check empty taglist with set} {
	set struct {a {*int ?}}
	set try {}
	structlset -struct $struct $try {} {a 1}
} {a 1}

test structlset-struct {basic: set one of 2 to default} {
	set struct {a {a {*int ?} b {*int ?}}}
	set try {a {a 1 b 2}}
	structlset -struct $struct $try {a a} ?
} {a {b 2}}

test structlset-struct {basic: add one} {
	set struct {* {*int ?}}
	set try {a 1}
	structlset -struct $struct $try b 10
} {a 1 b 10}

test structlset-struct {basic: add one: 2 tags} {
	set struct {a {* {*int ?}}}
	set try {a {a 1}}
	structlset -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test structlset-struct {basic: change one} {
	set struct {a {* {*int ?}}}
	set try {a {a 1 b 1}}
	structlset -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test structlset-struct {basic: error} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try a 10
} {error: incorrect value trying to assign "10" to struct "a {*int ?}"} 1

test structlset-struct {basic: error too much tags} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try {a a a} 10
} {error: tag "a" not present in structure "*int ?"} 1

test structlset-struct {basic: error with *} {
	set struct {a {* {*int ?}}}
	set try {}
	structlset -struct $struct $try a 10
} {error: incorrect value trying to assign "10" to struct "* {*int ?}"} 1

test structlset-struct {basic: set one by value} {
	set struct {a {a {*int ?}}}
	set try {}
	structlset -struct $struct $try a {a 10}
} {a {a 10}}

test structlset-struct {basic: add one by value} {
	set struct {a {* {*int ?}}}
	set try {a {a 1}}
	structlset -struct $struct $try a {b 10}
} {a {a 1 b 10}}

test structlset-struct {basic: change one by value} {
	set struct {a {* {*int ?}}}
	set try {a {a 1 b 1}}
	structlset -struct $struct $try a {b 10}
} {a {a 1 b 10}}

test structlset-struct {basic: change one} {
	set struct {a {* {*int ?}}}
	set try {a {a 1 b 1}}
	structlset -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test structlset-struct {basic: set empty} {
	set struct {a {* {*int ?}}}
	set try {a {a 1 b 1}}
	structlset -struct $struct $try a {}
} {a {a 1 b 1}}

test structlset-struct {change struct} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	structlset -struct $struct $try {c b a} 10
} {a 1 b 2 c {a 1 b {a 10 b 2}}}

test structlset-struct {new in struct: 1} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	structlset -struct $struct $try {c d} 10
} {a 1 b 2 c {a 1 b {a 1 b 2} d 10}}

test structlset-struct {new in struct: 2} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	structlset -struct $struct $try {c b d} 10
} {a 1 b 2 c {a 1 b {a 1 b 2 d 10}}}

test structlset-struct {check conformance to structure} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	structlset -struct $struct $try {c d a} 10
} {error: tag "a" not present in structure "*int ?"} 1

test structlset-struct {1} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	structlset -struct $struct $try {c b} 10
} {error: incorrect value trying to assign "10" to struct "* {*int ?}"} 1

test structlset-struct {6} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	structlset -struct $struct $try {c b} {d 10}
} {a 1 b 2 c {a 1 b {a 1 b 2 d 10}}}

# get tests

test structlget-struct {check empty taglist with get} {
	set struct {a {*int ?}}
	set try {}
	structlget -struct $struct $try {}
} {a ?}

test structlget-struct {basic: get value} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	structlget -struct $struct $try {a a}
} {10}

test structlget-struct {basic: get substructure} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	structlget -struct $struct $try a
} {a 10}

test structlget-struct {basic: get substructure: mixed with default} {
	set struct {a {a {*int ?} b {*int ?}}}
	set try {a {a 10}}
	structlget -struct $struct $try a
} {a 10 b ?}

test structlget-struct {basic: default} {
	set struct {a {a {*int ?}}}
	set try {}
	structlget -struct $struct $try {a a}
} {?}

test structlget-struct {basic: default struct} {
	set struct {a {a {*int ?}}}
	set try {}
	structlget -struct $struct $try a
} {a ?}

test structlget-struct {get default with pattern: 1 tag} {
	set struct {* {*int ?} a {*int ?}}
	structlget -struct $struct {} {}
} {a ?}

test structlget-struct {get default with pattern: 2 tags} {
	set struct {a {* {*int ?} a {*int ?}}}
	set try {}
	structlget -struct $struct $try a
} {a ?}

test structlget-struct {mixed default and value} {
	set struct {* {*int ?} a {*int ?}}
	set try {b 1}
	structlget -struct $struct $try {}
} {a ? b 1}

test structlget-struct {mixed default and value} {
	set struct {a {* {*int ?} a {*int ?}}}
	set try {a {b 1}}
	structlget -struct $struct $try a
} {a ? b 1}


testsummarize
