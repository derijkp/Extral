#!/bin/sh
# the next line restarts using wish \
exec tclsh7.5 "$0" "$@"
#package require extral
load /peter/dev/Extral/extral.dll

lappend auto_path /peter/dev/Extral/lib
cd /peter/dev/Extral
set targetdir /tcl/lib/Extral0.94

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
	mkdir $targetdir

	cp README extral.dll extral.txt pkgIndex.tcl $targetdir
	cp -r lib $targetdir
	catch {rm [file join $targetdir lib *~]}
	difaccess 0644 0755 $targetdir

