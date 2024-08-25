#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

# see where we are, add the lib dir in the original location to auto_path, load extension
set script [file join [pwd] [info script]]
while 1 {
	if {[catch {set script [file join [file dir $script] [file readlink $script]]}]} break
}
set srcdir [file dir [file dir $script]]
cd $srcdir

# settings
# --------

namespace eval Extral {}
lappend auto_path $srcdir $srcdir/lib $srcdir/libnoc
puts $srcdir/lib

# generate tml docs
# -----------------

if {![file exists $srcdir/docs.old]} {file copy -force $srcdir/docs $srcdir/docs.old}
file delete -force $srcdir/docs
set destdir $srcdir/docs
file mkdir $destdir/cmds-tmp

Extral::makedoc [glob -nocomplain $srcdir/lib/*.tcl $srcdir/libnoc/*.tcl] $srcdir/docs/cmds-tmp
file mkdir $destdir/cmds
set files [glob -nocomplain $destdir/cmds-tmp/*.html]
foreach file $files {
	puts $file
	set data [file_read $file]
	regexp -nocase <body>(.*)</body> $data temp data
	regsub -all {\\} $data {\\\\} data
	regsub -all {\$} $data {\\$} data
	regsub -all {\[} $data {\\[} data
	regsub -nocase {<h1>([^<]*)</h1>} $data {[extral::header "\1"]} data
	append data "</ul>\n"
	append data {[extral::footer]}
	file_write $destdir/cmds-tmp/[file root [file tail $file]].tml $data
}

# tml2html
# --------

namespace eval extral {}

proc sfdownload {file {project extral}} {
	return "<a href=\"http://prdownloads.sourceforge.net/$project/$file?download\">$file</a>"
}

proc listfiles {files} {
	set result {}
	foreach file $files {
		append result <li>[sfdownload $file]
	}
	return $result
}

proc extral::header {title} {
global page
set pre $page(root)
return [subst {<HTML>
<HEAD>
   <META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=iso-8859-1">
   <META NAME="Author" CONTENT="Peter De Rijk">
   <TITLE>Extral Documentation</TITLE>
</HEAD>
<BODY BGCOLOR="#FEFEFE">
<CENTER>
<H1>Extral $title</H1>
</CENTER>
<CENTER>
<A HREF="${pre}index.html">Documentation index</A>
&nbsp; &nbsp; &nbsp; &nbsp;
<A HREF="https://derijkp.github.io/Extral">repo</a> 
</CENTER>
<p>
}]
}

proc extral::footer {} {
set projectid 108004
set project extral
return [subst {<HR WIDTH="100%">
<table width=100%><tr><td>
</td><td align=right>
hosted at <a href="https://derijkp.github.io/Extral">https://derijkp.github.io/Extral</a>
</td></tr><table>
</BODY>
</HTML>
}]
}

proc tml2html {file destfile {root {}}} {
	global page
	set page(template) $file
	set destfile [file root $destfile].html
	set f [open $file]
	set data [read $f]
	close $f
	set page(filename) $destfile
	set num [llength [file split $destfile]]
	incr num -2
	set page(root) $root
	set page(url) /$destfile
	set data [subst $data]
	set f [open $destfile w]
	puts -nonewline $f $data
	close $f
}

file mkdir $destdir/cmds
set files [glob -nocomplain $srcdir/docs/cmds-tmp/*.tml]
foreach file $files {
	set destfile [file join $destdir [file tail $file]]
	set destfile [file root $destfile].html
	puts "Making $destfile"
	tml2html $file $destfile
}
file delete -force $destdir/cmds-tmp
