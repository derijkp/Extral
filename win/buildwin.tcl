#!/bin/sh
# the next line restarts using wish \
exec tclsh7.5 "$0" "$@"
#package require extral
cd C:/peter/dev/Extral
load C:/peter/dev/Extral/extral.dll

lappend auto_path [pwd]/lib
set targetdir "C:/Program Files/tcl8.0/lib/Extral1a"

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

# Main Program
# ---------------------------------------------------------
	if [file exists $targetdir] {
		error "Target directory $targetdir exists"
	}
	file mkdir $targetdir

	file copy Readme.txt extral.dll extral.txt pkgIndex.tcl $targetdir
	file copy lib $targetdir
	catch {file delete [file join $targetdir lib *~]}
	difaccess 0644 0755 $targetdir
exit
