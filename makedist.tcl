#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" ${1+"$@"}
#

cd [file dir [info script]]

if {[llength $argv] == 0} {
	set targetdir [file dir [pwd]]
} else {
	set targetdir [lindex $argv 0]
}

# $Format: "set version $ProjectMajorVersion$.$ProjectMinorVersion$"$
set version 2
# $Format: "set minorversion $ProjectMinorVersion$"$
set minorversion 0

set targetdir [file join $targetdir Extral-$tcl_platform(os)-$version.$minorversion]
puts "Building binary distribution in $targetdir"

proc clean {filemode dirmode dir} {
	if [catch {glob [file join $dir *]} files] return
	foreach file $files {
		if [regexp {~$} $file] {
			file delete $file
		} elseif [regexp {.save$} $file] {
			file delete $file
		} elseif [regexp "[info sharedlibextension]\$" $file] {
		} elseif [file isdirectory $file] {
			catch {file attributes $file -permissions $dirmode}
			clean $filemode $dirmode $file
		} else {
			catch {file attributes $file -permissions $filemode}
		}
	}
}

# Main Program
# ---------------------------------------------------------
	if [file exists $targetdir] {
		error "Target build directory $targetdir exists"
	}
	auto_mkindex lib
	file mkdir $targetdir
	file copy docs lib $targetdir
	file copy README pkgIndex.tcl $targetdir
	if [catch {file copy extral[info sharedlibextension] $targetdir}] {
		puts stderr "Warning, no compiled version available"
	}
	clean 0644 0755 $targetdir
	auto_mkindex [file join $targetdir lib]
exit
