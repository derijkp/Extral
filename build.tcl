#!/bin/sh
# the next line restarts using wish \
exec tclsh7.5 "$0" "$@"
#package require extral

#set targetdir $env(HOME)/bin/[file tail [pwd]]
set targetdir $argv

proc mkdir {dir} {
	exec mkdir $dir
}

proc lshift {ulist} {
	upvar $ulist list
	if {[llength $list]==0} {return ""}
	set result [lindex $list 0]
	set list [lrange $list 1 end]
	return $result
}

proc chmod {args} {
	set cl ""
	while 1 {
		set item [lshift args]
		if {"[string index $item 0]"!="-"} break
		lappend cl $item
	}

	while 1 {
		if [regexp {\*} $item] {
			eval lappend cl [glob $item]
		} else {
			eval lappend cl $item
		}
		set item [lshift args]
		if {"$item"==""} break
	}
	eval exec chmod $cl
}

proc cp {args} {
	set cl ""
	while 1 {
		set item [lshift args]
		if {"[string index $item 0]"!="-"} break
		lappend cl $item
	}

	while 1 {
		if [regexp {\*} $item] {
			eval lappend cl [glob $item]
		} else {
			eval lappend cl $item
		}
		set item [lshift args]
		if {"$item"==""} break
	}
	eval exec cp $cl
}

proc mv {args} {
	set cl ""
	while 1 {
		set item [lshift args]
		if {"[string index $item 0]"!="-"} break
		lappend cl $item
	}

	while 1 {
		if [regexp {\*} $item] {
			eval lappend cl [glob $item]
		} else {
			eval lappend cl $item
		}
		set item [lshift args]
		if {"$item"==""} break
	}
	eval exec mv $cl
}

proc rm {args} {
	set cl ""
	while 1 {
		set item [lshift args]
		if {"[string index $item 0]"!="-"} break
		lappend cl $item
	}

	while 1 {
		if [regexp {\*} $item] {
			eval lappend cl [glob $item]
		} else {
			eval lappend cl $item
		}
		set item [lshift args]
		if {"$item"==""} break
	}
	eval exec rm $cl
}

proc access {mode pattern} {
	set list [glob $pattern]
	chmod $mode $list
	foreach file $list {
		if [file isdirectory $file] {
			access $mode [file join $pattern *]
		}
	}
}

proc difaccess {filemode dirmode dir} {
	set list [glob $dir]
	eval chmod $filemode $list
	foreach file $list {
		if [file isdirectory $file] {
			chmod $dirmode $file
			difaccess $filemode $dirmode [file join $dir *]
		}
	}
}

proc setfirstline {file fline} {
	set f [open $file "r"]
	set c [read $f]
	close $f
	regsub "^\[^\n\]*\n" $c "$fline\n" c
	set f [open $file w]
	puts $f $c -nonewline
	close $f
}

proc build {targetdir} {
	if [file exists $targetdir] {
		error "Target directory $targetdir exists"
	}
	mkdir $targetdir

	cp README extral.so extral.txt pkgIndex.tcl $targetdir		
	cp -r lib $targetdir		
	catch {rm [file join $targetdir lib *~]}
	difaccess 0644 0755 $targetdir
}

build $targetdir
