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

proc new {} {
	global extraL__Priv
	if ![info exists extraL__Priv(pointernr)] {
		set extraL__Priv(pointernr) 1
	}
	incr extraL__Priv(pointernr)
	uplevel global extraL__Priv__Pointer$extraL__Priv(pointernr)
	return extraL__Priv__Pointer$extraL__Priv(pointernr)
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
		set current [struct value $current->next]
	}
	struct set $keep->next ""
	return $root
}

proc exampleread {current} {
	while 1 {
		puts stdout [struct value $current->value]
		puts [struct arrayget $current->array]
		if {[struct value $current->next] == ""} {break}
		set keep $current
		set current [struct value $current->next]
	}
}

proc exampledestruct {current} {
	while 1 {
		if {[struct value $current->next] == ""} {break}
		set keep $current
		set current [struct value $current->next]
		struct unset $keep
	}
}

proc structvar {} {
	global extraL__Struct
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
	return Struct$extraL__Struct(pointernr)
	}
	set var [lindex $args 0]
	switch $option {
		set {
			if {[llength $args]!=2} {
				error "wrong # args: should be \"struct set struct value\""
			}
			set extraL__Struct($var) [lindex $args 1]
		}
		unset {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"struct unset struct\""
			}
		if [regexp -- {->} $var] {
			set list [array names extraL__Struct "$var\(*"]
				catch {unset extraL__Struct($var)}
				catch {eval unset [lregsub {^(.+)$} $list {extraL__Struct(\1)}]}
			} else {
				set list [array names extraL__Struct "$var->*"]
				catch {eval unset [lregsub {^(.+)$} $list {extraL__Struct(\1)}]}
			}
		}
	value {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"struct value struct\""
			}
			return $extraL__Struct($var)
	}
	var {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"struct var struct\""
			}
			return extraL__Struct($var)
	}
		arrayset {
			if {[llength $args]!=3} {
				error "wrong # args: should be \"struct arrayset struct items values\""
			}
		set items [lregsub {^(.+)$} [lindex $args 1] "$var\(\\1\)"]
			array set extraL__Struct [lmanip join [lmanip merge $items [lindex $args 2]] { } all]
		}
		arrayget {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"struct arrayget struct\""
			}
		set list [array get extraL__Struct "$var\(*"]
			return [lregsub "^$var\\((.*)\\)\$" $list {\1}]
		}
		arraynames {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"struct arrayget struct\""
			}
		set list [array names extraL__Struct "$var\(*"]
			return [lregsub "^$var\\((.*)\\)\$" $list {\1}]
		}
		arraysize {
			if {[llength $args]!=1} {
				error "wrong # args: should be \"struct arraysize struct\""
			}
		set list [array names extraL__Struct "$var\(*"]
			return [llength $list]
		}
		default {
			error "bad option \"$option\": should be new, set, value, var, unset"
		}
	}
}
