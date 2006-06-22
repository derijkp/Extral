#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

package require pkgtools
cd [pkgtools::startdir]

# settings
# --------

set libfiles {lib README pkgIndex.tcl init.tcl DESCRIPTION.txt}
set shareddatafiles README
set headers {}
set libbinaries [::pkgtools::findlib [file dir [pkgtools::startdir]] class]
set binaries {}

# standard
# --------
pkgtools::install $argv

