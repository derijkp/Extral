# This file provides for an alternative loading of extensions
# based on directory.
# in order to load the given package, this file is sourced
# When this script is sourced, the variable $dir must contain the
# full path name of the xtensions directory.

namespace eval ::Extral {}
set ::Extral::dir $dir
source [file join $dir lib init.tcl]
extension provide Extral 2.1.0
