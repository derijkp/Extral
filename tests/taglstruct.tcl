#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

test taglset-struct {check empty taglist with set} {
	set struct {a {*int ?}}
	set try {}
	taglset -struct $struct $try {} 1
} {error: empty taglist} 1

test taglset-struct {basic} {
	set struct {a {a {*int ?}}}
	set try {}
	taglset -struct $struct $try {a a} 10
} {a {a 10}}

test taglset-struct {basic: set to default} {
	set struct {a {a {*int ?}}}
	set try {}
	taglset -struct $struct $try {a a} ?
} {}

test taglset-struct {basic: set to default: remove old} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	taglset -struct $struct $try {a a} ?
} {}

test taglset-struct {basic: set one of 2 to default} {
	set struct {a {a {*int ?} b {*int ?}}}
	set try {a {a 1 b 2}}
	taglset -struct $struct $try {a a} ?
} {a {b 2}}

test taglset-struct {basic: add one} {
	set struct {a {* {*int ?}}}
	set try {a {a 1}}
	taglset -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test taglset-struct {basic: change one} {
	set struct {a {* {*int ?}}}
	set try {a {a 1 b 1}}
	taglset -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test taglset-struct {basic: error} {
	set struct {a {a {*int ?}}}
	set try {}
	taglset -struct $struct $try a 10
} {error: incorrect value trying to assign "10" to struct "a {*int ?}"} 1

test taglset-struct {basic: error too much tags} {
	set struct {a {a {*int ?}}}
	set try {}
	taglset -struct $struct $try {a a a} 10
} {error: tag "a" not present in structure "*int ?"} 1

test taglset-struct {basic: error with *} {
	set struct {a {* {*int ?}}}
	set try {}
	taglset -struct $struct $try a 10
} {error: incorrect value trying to assign "10" to struct "* {*int ?}"} 1

test taglset-struct {basic: set one by value} {
	set struct {a {a {*int ?}}}
	set try {}
	taglset -struct $struct $try a {a 10}
} {a {a 10}}

test taglset-struct {basic: add one by value} {
	set struct {a {* {*int ?}}}
	set try {a {a 1}}
	taglset -struct $struct $try a {b 10}
} {a {a 1 b 10}}

test taglset-struct {basic: change one by value} {
	set struct {a {* {*int ?}}}
	set try {a {a 1 b 1}}
	taglset -struct $struct $try a {b 10}
} {a {a 1 b 10}}

test taglset-struct {basic: change one} {
	set struct {a {* {*int ?}}}
	set try {a {a 1 b 1}}
	taglset -struct $struct $try {a b} 10
} {a {a 1 b 10}}

test taglset-struct {basic: set empty} {
	set struct {a {* {*int ?}}}
	set try {a {a 1 b 1}}
	taglset -struct $struct $try a {}
} {a {a 1 b 1}}

test taglset-struct {change struct} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	taglset -struct $struct $try {c b a} 10
} {a 1 b 2 c {a 1 b {a 10 b 2}}}

test taglset-struct {new in struct: 1} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	taglset -struct $struct $try {c d} 10
} {a 1 b 2 c {a 1 b {a 1 b 2} d 10}}

test taglset-struct {new in struct: 2} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	taglset -struct $struct $try {c b d} 10
} {a 1 b 2 c {a 1 b {a 1 b 2 d 10}}}

test taglset-struct {check conformance to structure} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	taglset -struct $struct $try {c d a} 10
} {error: tag "a" not present in structure "*int ?"} 1

test taglset-struct {1} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	taglset -struct $struct $try {c b} 10
} {error: incorrect value trying to assign "10" to struct "* {*int ?}"} 1

test taglset-struct {6} {
	set struct {a {*int ?} b {*int ?} c {* {*int ?} a {*int ?} b {* {*int ?}}}}
	set try {a 1 b 2 c {a 1 b {a 1 b 2}}}
	taglset -struct $struct $try {c b} {d 10}
} {a 1 b 2 c {a 1 b {a 1 b 2 d 10}}}

# get tests

test taglget-struct {check empty taglist with get} {
	set struct {a {*int ?}}
	set try {}
	taglget -struct $struct $try {}
} {error: empty taglist} 1

test taglget-struct {basic: get value} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	taglget -struct $struct $try {a a}
} {10}

test taglget-struct {basic: get substructure} {
	set struct {a {a {*int ?}}}
	set try {a {a 10}}
	taglget -struct $struct $try a
} {a 10}

test taglget-struct {basic: get substructure: mixed with default} {
	set struct {a {a {*int ?} b {*int ?}}}
	set try {a {a 10}}
	taglget -struct $struct $try a
} {a 10 b ?}

test taglget-struct {basic: default} {
	set struct {a {a {*int ?}}}
	set try {}
	taglget -struct $struct $try {a a}
} {?}

test taglget-struct {basic: default struct} {
	set struct {a {a {*int ?}}}
	set try {}
	taglget -struct $struct $try a
} {a ?}

test taglget-struct {basic: default with pattern} {
	set struct {a {* {*int ?} a {*int ?}}}
	set try {}
	taglget -struct $struct $try a
} {a ?}


testsummarize
