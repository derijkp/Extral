# Some convenience functions I often use, so they ended up here
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================
#doc convenience title {Convenience functions}

#doc {convenience invoke} cmd {
#invoke vars cmd ...
#} descr {
# invoke simply evals $cmd in a private space. This eg. allows using
# temporary variables in bindings without creating these in global scope.
# It is also very convenient to use values appended to a command given
# to a binding:
# Further arguments (when given) are parameters that will be available in the
# variables given in vars. If more parameters are supplied than vars are given,
# the remaining parameters will be stored in the variable args.
#}
proc invoke {vars cmd args} {
	foreach var $vars {
		set $var [lshift args]
	}
	eval $cmd
}

#doc {convenience arraytrans} cmd {
#arraytrans varName list ?default?
#} descr {
# returns a list of all the values in the array corresponding to the
# indices in the given list. If the index is not present in the array,
# the index itself will be returned.
# If $default is given, it will be returned for indices not in the array.
#}
proc arraytrans {varName list args} {
	upvar $varName var
	set result ""
	if {"$args" == ""} {
		foreach item $list {
			if [info exists var($item)] {
				lappend result $var($item)
			} else {
				lappend result $item
			}
		}
	} else {
		set def [lindex $args 0]
		foreach item $list {
			if [info exists var($item)] {
				lappend result $var($item)
			} else {
				lappend result $def
			}
		}
	}
	return $result
}

#doc {convenience ?} cmd {
#? expr truevalue falsevalue
#} descr {
# ? expr truevalue falsevalue
#}
proc ? {expr truevalue falsevalue} {
	uplevel if [list $expr] {{set ::Extral::temp 1} else {set ::Extral::temp 0}}
	if $::Extral::temp {return $truevalue} else {return $falsevalue}
}

#doc {convenience ?} cmd {
#echo string
#} descr {
# echo returns its argument as a result
# This is useful when you want a command that will be evalled or upleveled
# to return a certain value
# 
#}
proc echo {string} {
	return $string
}

#doc {convenience ?} cmd {
#get varName ?default?
#} descr {
# get returns the value of the variable given by varName if it exists.
# If the variable does not exists, it returns an empty string, or
# value given by $default if present
# 
#}
proc get {varName {default {}}} {
	upvar $varName var
	if [info exists var] {
		return $var
	} else {
		return $default
	}
}

#doc {convenience rem} cmd {
#rem args
#} descr {
#	does nothing<br>
#	I use this to put some example or testing code in a program
#	without all the #'s
#}
proc rem {args} {
}

# REM:
#doc {convenience REM} cmd {
#REM args
#} descr {
#	when the procedure remof is called, REM will also do nothing
#	when the procedure remon is called, REM will put its arguments
#	to the stdout
#}
proc REM {args} {}
proc remon {} {
	proc REM {args} {	  
		puts stdout $args   
	}
}
proc remoff {} {
	proc REM {args} {}
}

#doc {convenience true} cmd {
#true expression
#} descr {
# true expr
#	returns 1 when expression is yes, true or 1<br>
#	otherwise it returns 0.
#}
proc true {expr} {
	set result 0
	return [regexp -nocase {^(1|yes|true|on)$} $expr]
}

#doc {convenience setglobal} cmd {
# setglobal varName ?newValue?
#} descr {
#	same as the set command, but then for global variables
#}
proc setglobal {varName args} {
	upvar #0 $varName var
	if {"$args" == ""} {
		if ![info exists var] {
			error "can't read \"$varName\": no such global variable"
		} else {
			return $var
		}
	} else {
		set var [lindex $args 0]
	}
}

#doc {convenience random} cmd {
#random min max
#} descr {
#returns a random number between min and max
#}
proc random {min max} {
	set r [expr $max-$min+1]
	return [expr int($min+rand()*$r)]
}

#doc {convenience random} cmd {
#inlist list value
#} descr {
#returns 1 if $value is an element of list $list
#returns 0 if $value is not an element of list $list
#}
proc inlist {list value} {
        if {[lsearch $list $value]==-1} {
                return 0
        } else {
                return 1
        }
}

proc extractoption {listName option default} {
	upvar $listName l
	set pos [lsearch $l $option]
	if {$pos != -1} {
		lpop l $pos
		return [lpop l $pos]
	} else {
		return $default
	}
}

proc extractbool {listName option} {
	upvar $listName l
	set pos [lsearch $l $option]
	if {$pos != -1} {
		lpop l $pos
		return 1
	} else {
		return 0
	}
}

proc Extral::makedoc {files dstdir {indextitle ""}} {
	catch {unset ::Extral::doc}
	catch {unset ::Extral::sections}
	foreach file $files {
		set f [open $file]
		while 1 {
			if ![info exists earlydoc] {
				set line [gets $f]
			} else {
				unset earlydoc
			}
			if [regexp ^#doc $line] {
				set list [string range $line 5 end]
				while 1 {
					if [info complete $list] break
					set line [gets $f]
					if [regexp ^# $line] {
						if [regexp ^#doc $line] {
							set earlydoc 1
							append list "\n\}"
							break
						}
						append list "\n"
						append list [string range $line 1 end]
					} else {
						append list "\n\}"
						break
					}
				}
				set section [lshift list]
				set full ""
				foreach level $section {
					laddnew ::Extral::sections($full) $level
					lappend full $level
				}
				eval lappend {::Extral::doc($section)} $list
			}
			if [eof $f] break
		}
		close $f
	}

	set index() ""
	foreach section $::Extral::sections() {
		if ![info exists ::Extral::doc($section)] {
			set ::Extral::doc($section) {}
		}
		set list $::Extral::doc($section)
		if [catch {set title [structlget $list title]}] {
			set title $section
		}
		if [catch {set descr [structlget $list descr]}] {
			set descr ""
		}
		if [catch {set ind [structlget $list index]}] {
			set ind ""
		}
		if {"$ind" != "none"} {
			append index($ind) "<li><a href=\"$section.html\">$title</a>"
			if ![catch {set shortdescr [structlget $list shortdescr]}] {
				append index($ind) ": $shortdescr"
			}
		}
		set f [open [file join $dstdir $section.html] w]
		puts $f "<HEAD>"
		puts $f "<TITLE>$title</TITLE>"
		puts $f "</HEAD>"
		puts $f "<BODY>"
		puts $f "<h1>$title</h1>"
		puts $f $descr
		set ::Extral::prevcmd 0
		if [info exists ::Extral::sections($section)] {
			foreach subsection $::Extral::sections($section) {
				puts $f [::Extral::outputdoc [list $section $subsection]]
			}
		}
		if $::Extral::prevcmd {
			append result "</dl>\n"
		}
		puts $f "</body>"
		close $f
	}
	set f [open [file join $dstdir index.html] w]
	puts $f "<HEAD>"
	puts $f "<TITLE>$indextitle</TITLE>"
	puts $f "</HEAD>"
	puts $f "<BODY>"
	puts $f "<h1>$indextitle</h1>"
	puts $f <ul>$index()</ul>
	unset index()
	foreach subtitle [lsort [array names index]] {
		puts $f "<h2>$subtitle</h2>"
		puts $f <ul>$index($subtitle)</ul>
	}
	puts $f "</body>"
	close $f
}

proc Extral::outputdoc {section} {
	if ![info exists ::Extral::doc($section)] return
	set list $::Extral::doc($section)
	set result ""
	if [catch {set type [structlget $list type]}] {
		set type [lindex $list 0]
	}
	switch -regexp $type {
		^cmd$ {
			if !$::Extral::prevcmd {
				append result "<dl>"
				set ::Extral::prevcmd 1
			}
			if [catch {set cmd [structlget $list cmd]}] {
				set cmd [lindex $section end]
			}
			append result "<b><dt>$cmd</b>"
			if ![catch {set descr [structlget $list descr]}] {
				append result "<dd>$descr"
			}
			if ![catch {set example [structlget $list example]}] {
				append result "<br>eg.:<pre>$example</pre>"
			}
		}
		^option$ {
			if !$::Extral::prevcmd {
				append result "<dl>"
				set ::Extral::prevcmd 1
			}
			if [catch {set option [structlget $list option]}] {
				set option [lindex $section end]
			}
			append result "<b><dt>Command-Line Name: [lindex $option 0]</b>"
			append result "<b><dt>Database Name: [lindex $option 1]</b>"
			append result "<b><dt>Database Class: [lindex $option 2]</b>"
			if ![catch {set descr [structlget $list descr]}] {
				append result "<dd>$descr"
			}
			if ![catch {set example [structlget $list example]}] {
				append result "<br>eg.:<pre>$example</pre>"
			}
		}
		{^h[0-9]+} {
			if $::Extral::prevcmd {
				append result "</dl>"
				set ::Extral::prevcmd 0
			}
			if [catch {set title [structlget $list $type]}] {
				set title [lindex $section end]
			}
			append result "<$type>$title</$type>"
			if ![catch {set descr [structlget $list descr]}] {
				append result $descr
			}
			if [info exists ::Extral::sections($section)] {
				foreach subsection $::Extral::sections($section) {
					append result [Extral::outputdoc [concat $section $subsection]]
				}
			}
		}
	}
	return $result
}
