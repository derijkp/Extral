# struct.tcl --
#
# Code to use references and structures in Tcl
# The example creates, prints and destroys a simple linked list
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

namespace eval extraLstruct {
}

proc example {} {
	set pointer [examplecreate]
	exampleread $pointer
	exampledestruct $pointer
}

proc examplecreate {} {
	set current [struct new]
	set root $current
	foreach value {a b c d} {
		struct set $current->value $value
#		struct arrayset $current->array {a b c} {1 2 3}
		struct set $current->next [struct new]
		set keep $current
		set current [struct set $current->next]
	}
	struct set $keep->next ""
	return $root
}

proc exampleread {current} {
	while 1 {
		puts stdout [struct set $current->value]
		puts [struct arrayget $current->array]
		if {[struct set $current->next] == ""} {break}
		set keep $current
		set current [struct set $current->next]
	}
}

proc exampledestruct {current} {
	while 1 {
		if {[struct set $current->next] == ""} {break}
		set keep $current
		set current [struct set $current->next]
		struct unset $keep
	}
}

proc struct {option args} {
	global extraL__Struct

	if [string match $option new] {
		if ![string match $args ""] {
			error "wrong # args: should be \"struct new\""
		}
		if ![info exists extraL__Struct(pointernr)] {
			set extraL__Struct(pointernr) 1
		}
		incr extraL__Struct(pointernr)
		set name Struct$extraL__Struct(pointernr)
		set ::extraLstruct::s$name-> $extraL__Struct(pointernr)
		return $name
	}
	set var [lindex $args 0]
	switch $option {
		names {
			set names [namespace eval ::extraLstruct {info vars s*->}]
			set result ""
			foreach name $names {
				regexp {^s(.+)->} $name temp name
				lappend result $name
			}
			return $result
		}
		fields {
			set len [llength $args]
			if {$len!=1} {
				error "wrong # args: should be \"struct fields struct\""
			}
			set names [namespace eval ::extraLstruct [list info vars s$var->*]]
			set result ""
			foreach name $names {
				if [regexp {^s.+->(.+)$} $name temp name] {
					lappend result $name
				}
			}
			return $result
		}
		set {
			set len [llength $args]
			if {($len!=1)&&($len!=2)} {
				error "wrong # args: should be \"struct set struct ?value?\""
			}
			if {$len==1} {
				set error [catch {set ::extraLstruct::s$var} result]
			} else {
				set error [catch {set ::extraLstruct::s$var [lindex $args 1]} result]
			}
			if $error {
				regsub {::extraLstruct::s} $result {} result
				error $result
			} else {
				return $result
			}
		}
		unset {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"struct unset struct\""
			}
			if ![regexp -- {->} $var] {
				set error [catch {namespace eval ::extraLstruct "foreach var \[info vars s$var->*\] \{unset \$var\}"} result]
			} else {
				set error [catch {unset extraLstruct::s$var} result]
			}
			if $error {
				regsub {::extraLstruct::s} $result {} result
				error $result
			} else {
				return $result
			}
		}
		array {
			set len [llength $args]
			if {$len<2} {
				error "wrong # args: should be \"struct array option arrayName ?arg ...? \""
			}
			set option [lshift args]
			set var [lshift args]
			set error [catch {eval {array $option ::extraLstruct::s$var} $args} result]
			if $error {
				regsub {::extraLstruct::s} $result {} result
				error $result
			} else {
				return $result
			}
		}
		default {
			error "bad option \"$option\": should be new, set, unset"
		}
	}
}
