# Tcl package index file, version 1.0
# This file is sourced either when an application starts up or
# by a "package unknown" script.  It invokes the
# "package ifneeded" command to set up package-related
# information so that packages will be loaded automatically
# in response to "package require" commands.  When this
# script is sourced, the variable $dir must contain the
# full path name of this file's directory.

namespace eval __temp [list set dir $dir]
namespace eval __temp {
	# $Format: "set version $ProjectVersion$"$
set version 1a.25
	regsub -all {[ab]} $version {} version
	set loadcmd {
		package provide Extral @version@
		namespace eval Extral {set dir @dir@}
		source [file join @dir@ lib init.tcl]
		namespace eval Extral {set version @version@}
	}
	regsub -all {@version@} $loadcmd [list $version] loadcmd
	regsub -all {@dir@} $loadcmd [list $dir] loadcmd
	package ifneeded Extral $version $loadcmd
}
namespace delete __temp
