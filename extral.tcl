
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

# Remark: does nothing
# I use this to put some example or testing code in a program
# without all the #'s
# ===========================================================
proc rem {args} {
}

# Code to let a variable iterate over a list
# handy when checking a foreach loop manually
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

# Code to use something like references and structures in Tcl
# The example creates and prints a simple linked list
# ===========================================================

rem example {
    set current root
    foreach value {a b c d} {
        sset current value $value
        sset current next [new]
        set keep $current
        set current [value current next]
    }
    sset keep next ""

    set value 1
    set current root
    while 1 {
        puts [value current value]
        if {[value current next] == ""} {break}
        set current [value current next]
    }
}

proc new {} {
    global peos_Priv
    if ![info exists peos_Priv(pointernr)] {
        set peos_Priv(pointernr) 1
    }
    incr peos_Priv(pointernr)
    return peos_Priv_Pointer$peos_Priv(pointernr)
}

proc value {var item} {
    upvar $var v
    uplevel set ${v}($item)
}

proc sset {var item args} {
    upvar $var v
    if ![info exists v] {
        set v [new]
    }
    if ![string match $args ""] {
        uplevel set ${v}($item) $args
    } else {
        uplevel set ${v}($item)
    }
}



