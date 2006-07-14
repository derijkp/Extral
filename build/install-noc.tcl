#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

package require pkgtools
cd [pkgtools::startdir]

# settings
# --------

set libfiles {lib libnoc README pkgIndex.tcl init.tcl DESCRIPTION.txt}
set shareddatafiles README
set headers {}
set libbinaries [::pkgtools::findlib [file dir [pkgtools::startdir]] Extral]
puts "libbinaries: $libbinaries pkgtools::startdir:[pkgtools::startdir]"
set binaries {}

# standard
# --------
pkgtools::install $argv

