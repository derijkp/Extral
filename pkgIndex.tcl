# Tcl package index file, version 1.0
# This file is sourced either when an application starts up or
# by a "package unknown" script.  It invokes the
# "package ifneeded" command to set up package-related
# information so that packages will be loaded automatically
# in response to "package require" commands.  When this
# script is sourced, the variable $dir must contain the
# full path name of this file's directory.

# $Format: "set version $ProjectVersion$"$
set version 1a.5
regsub -all {[ab]} $version {} version
package ifneeded Extral $version [subst {
	load [file join $dir extral[info sharedlibextension]]
	lappend auto_path [file join $dir lib]
	set Extral_version $version
	package provide Extral $version
}]