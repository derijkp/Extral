
# Some list commands
# ==================
proc lpop {ulist} {
    upvar $ulist list
    if {[llength $list]==0} {return ""}
    set end [llength $list]
    incr end -1
    set result [lindex $list $end]
    incr end -1
    set list [lrange $list 0 $end]
    return $result
}

proc lpush {ulist item} {
    upvar $ulist list
    lappend list $item
}

proc lshift {ulist} {
    upvar $ulist list
    if {[llength $list]==0} {return ""}
    set result [lindex $list 0]
    set list [lrange $list 1 end]
    return $result
}

proc lunshift {ulist item} {
    upvar $ulist list
    if {[llength $list]==0} {
        set list $item
        return $item
    }
    set temp [lindex $list 0]
    set list [lreplace $list 0 0 $item $temp]
}

proc leor {l1 l2} {
    set cor [lcor $l1 $l2]
    set exclusive [lfind $cor -1]
    set join [lsub $cor -exclude $exclusive]
    set result [lsub $l1 -exclude $join]
    eval lappend result [lsub $l2 $exclusive]
}

proc lremove {listref args} {
    upvar $listref list
    foreach item $args {
        set list [lsub $list -exclude [lfind $list $item]]
    }
}

# Remark: does nothing
# I use this to put some example or testing code in a program
# without all the #'s
# ===========================================================
proc rem {args} {
}

# Code to let a variable iterate over a list
# handy when checking a foreach loop manually
# literate ?variable name? ?list?
# ===========================================

proc literate {var list} {
    global extraL_Priv_iterate
    upvar $var ref
    set extraL_Priv_iterate($var:list) $list
    set extraL_Priv_iterate($var:len) [llength $list]
    set extraL_Priv_iterate($var:pos) 0
    set ref [lindex $extraL_Priv_iterate($var:list) 0]
}

proc lnext {var} {
    global extraL_Priv_iterate
    upvar $var ref
    if ![info exists extraL_Priv_iterate($var:list)] {
        error "No iteration over $var set"
    }
    incr extraL_Priv_iterate($var:pos)
    if {$extraL_Priv_iterate($var:pos)==$extraL_Priv_iterate($var:len)} {
        unset extraL_Priv_iterate($var:list)
        unset extraL_Priv_iterate($var:len)
        unset extraL_Priv_iterate($var:pos)
        unset ref
    } else {
        set ref [lindex $extraL_Priv_iterate($var:list) $extraL_Priv_iterate($var:pos)]
    }
}

# Code to use structures in Tcl
# The example creates, prints and destroys a simple linked list
# =============================================================

proc example {} {
    set pointer [examplecreate]
    exampleread $pointer
    exampledestruct $pointer
}

proc examplecreate {} {
    set current [struct new]
    set root $current
    foreach value {a b c d} {
        struct set current->value $value
        struct set current->next [struct new]
        set keep $current
        set current [struct value current->next]
    }
    struct set keep->next ""
    return $root
}

proc exampleread {current} {
    while 1 {
        puts stdout [struct value current->value]
        if {[struct value current->next] == ""} {break}
        set keep $current
        set current [struct value current->next]
    }
}

proc exampledestruct {current} {
    while 1 {
        if {[struct value current->next] == ""} {break}
        set keep $current
        set current [struct value current->next]
        struct unset keep
    }
}

proc struct {option args} {
    switch $option {
        new {
            if ![string match $args ""] {
                error "wrong # args: should be \"struct new\""
            }
            global extral__Priv
            if ![info exists extral__Priv(pointernr)] {
        	    set extral__Priv(pointernr) 1
            }
            incr extral__Priv(pointernr)
            return extral__Priv__Pointer$extral__Priv(pointernr)
        }
        set {
            if {[llength $args]!=2} {
                error "wrong # args: should be \"struct set struct value\""
            }
            if ![regexp {^(.+)->(.+)$} [lindex $args 0] temp variable field] {
	        error "No field specified"
	    }
            set pointer [uplevel set $variable]->$field
            regsub {\([^()]*\)$} $pointer {} gpointer
            global $gpointer
            if [catch {set $pointer [lindex $args 1]} result] {
	        regsub {extral__Priv__Pointer[0-9]+} $result $variable error
                error $error
            }
            return $result
        }
        unset {
            if {[llength $args]!=1} {
                error "wrong # args: should be \"struct unset struct\""
            }
            if [regexp {^(.+)->(.+)$} [lindex $args 0] temp variable field] {
                set pointer [uplevel set $variable]->$field
                regsub {\([^()]*\)$} $pointer {} gpointer
		global $gpointer
		eval unset $pointer
	    } else {
		set pointer [uplevel set [lindex $args 0]]
		set vars [uplevel #0 info vars $pointer->*]
		catch {eval global $vars}
		catch {eval unset $vars}
            }
        }
	value {
            if {[llength $args]!=1} {
                error "wrong # args: should be \"struct value struct\""
            }
            if ![regexp {^(.+)->(.+)$} [lindex $args 0] temp variable field] {
	        error "No field specified"
	    }
            set pointer [uplevel set $variable]->$field
            regsub {\([^()]*\)$} $pointer {} gpointer
            global $gpointer
            if [catch {set $pointer} result] {
	        regsub {extral__Priv__Pointer[0-9]+} $result $variable error
                error $error
            }
            return $result
	}
	var {
            if {[llength $args]!=1} {
                error "wrong # args: should be \"struct var struct\""
            }
            if ![regexp {^(.+)->(.+)$} [lindex $args 0] temp variable field] {
	        error "No field specified"
	    }
            set pointer [uplevel set $variable]->$field
            return $pointer
	}
        default {
            error "bad option \"$option\": should be new, set, value, var, unset"
        }
    }
}
