package provide extral 0.92

# Some list commands
# ==================
proc lpop {ulist args} {
    upvar $ulist list
    if {[llength $args]>1} {
        error "Format is \"lpop listname ?position?\""
    }
    if {[llength $list]==0} {return ""}
    if {"$args"==""} { 
        set end [llength $list]
        incr end -1
        set result [lindex $list $end]
        incr end -1
        set list [lrange $list 0 $end]
    } else {
        set pos $args
        if {$pos>=[llength $list]} {
            return {}
        }
        set result [lindex $list $pos]
        set list [lsub $list -exclude $pos]
    }
    return $result
}

proc lpush {ulist item args} {
    upvar $ulist list
    if {[llength $args]>1} {
        error "Format is \"lpush listname item ?position?\""
    }
    if {"$args"==""} { 
        lappend list $item
    } else {
        set pos [expr $args-1]
        if {$pos>=[llength $list]} {
            return {}
        }
        set temp [lindex $list $pos]
        set list [lreplace $list $pos $pos $temp $item]
    }
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

proc lset {listref value indices} {
    upvar $listref list
    foreach index $indices {
        set list [lreplace $list $index $index $value]
    }
    return $list
}

proc larrayset {array varlist valuelist} {
    uplevel "array set $array \[lmanip join \[lmanip merge [list $varlist] [list $valuelist]\] \{ \} all\]"
}

proc leor {list1 list2} {
    set cor [lcor $list1 $list2]
    set exclusive [lfind $cor -1]
    set join [lsub $cor -exclude $exclusive]
    set result [lsub $list1 -exclude $join]
    eval lappend result [lsub $list2 $exclusive]
}

proc lunion {args} {
    set result [lmanip join $args { } all]
    return [lmanip remdup $result]
}

proc lcommon {args} {
    set result [lpop args]
    foreach arg $args {
        set result [lsub $arg [lcor $arg $result]]
    }
    return [lmanip remdup $result]
}

proc lremove {listref args} {
    upvar $listref list
    foreach item $args {
        set list [lsub $list -exclude [lfind $list $item]]
    }
    return $list
}

# Code to let a variable iterate over a list
# handy when checking a foreach loop manually
# literate ?variable name? ?list?
# ===========================================

proc literate {var list} {
    global extraL__Priv_iterate
    upvar $var ref
    set extraL__Priv_iterate($var:list) $list
    set extraL__Priv_iterate($var:len) [llength $list]
    set extraL__Priv_iterate($var:pos) 0
    set ref [lindex $extraL__Priv_iterate($var:list) 0]
}

proc lnext {var} {
    global extraL__Priv_iterate
    upvar $var ref
    if ![info exists extraL__Priv_iterate($var:list)] {
        error "No iteration over $var set"
    }
    incr extraL__Priv_iterate($var:pos)
    if {$extraL__Priv_iterate($var:pos)==$extraL__Priv_iterate($var:len)} {
        unset extraL__Priv_iterate($var:list)
        unset extraL__Priv_iterate($var:len)
        unset extraL__Priv_iterate($var:pos)
        unset ref
    } else {
        set ref [lindex $extraL__Priv_iterate($var:list) $extraL__Priv_iterate($var:pos)]
    }
}

# Code to use references and structures in Tcl
# The example creates, prints and destroys a simple linked list
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
#        struct arrayset $current->array {a b c} {1 2 3}
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

# Some convenience functions I often use, so they ended up here
# =============================================================
# Remark
# rem: 
#      does nothing
#      I use this to put some example or testing code in a program
#      without all the #'s
proc rem {args} {
}

# REM:
#      when the procedure remof is called, REM will also do nothing
#      when the procedure remon is called, REM will put its arguments
#      to the stdout
if {"[info commands REM]"==""} {
     proc remon {} {                            
         proc REM {args} {                      
             puts stdout $args   
         }
     }
     proc remof {} {
         proc REM {args} {}
     }
     remon
}

# true expr
# returns 1 when expression is yes, true or 1
proc true {expr} {
    set result 0
    switch $expr {
        yes {set result 1}
        true {set result 1}
        1 {set result 1}
    }
    return $result
}

proc setglobal {globalvar args} {
    upvar #0 $globalvar var
    if {"$args" == ""} {
        if ![info exists var] {
            error "can't read \"$globalvar\": no such global variable"
        } else {         
            return $var    
        }                
    } else {             
        set var [lindex $args 0]
    }
}

