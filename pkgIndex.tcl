# Tcl package index file, version 1.0
# This file is sourced either when an application starts up or
# by a "package unknown" script.  It invokes the
# "package ifneeded" command to set up package-related
# information so that packages will be loaded automatically
# in response to "package require" commands.  When this
# script is sourced, the variable $dir must contain the
# full path name of this file's directory.

# $Format: "set version $ProjectVersion$"$
set version 1a.9
regsub -all {[ab]} $Extral__version {} Extral__version
set Extral__temp {
	if [file exists [file join $dir extral[info sharedlibextension]]] {
		load [file join $dir extral[info sharedlibextension]]
	} else {
		source [file join $dir lib noc.tcl]
	}
	lappend auto_path [file join $dir lib]
	set Extral_version $Extral__version
	package provide Extral $Extral__version
}
regsub -all {\$Extral__version} $Extral__temp [list $Extral__version] Extral__temp
regsub -all {\$dir} $Extral__temp [list $dir] Extral__temp

#package ifneeded Extral $Extral__version $Extral__temp
package ifneeded Extral $Extral__version $Extral__temp
unset Extral__version
unset Extral__temp
