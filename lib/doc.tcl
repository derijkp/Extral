# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

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
