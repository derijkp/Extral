#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

package require pkgtools
cd [pkgtools::startdir]

# settings
# --------

set extname Extral
set libfiles {lib README.md pkgIndex.tcl init.tcl DESCRIPTION.txt}
set shareddatafiles {}
set headers {}
set libbinaries [::pkgtools::findlib [file dir [pkgtools::startdir]] Extral]
puts "libbinaries: $libbinaries pkgtools::startdir:[pkgtools::startdir]"
set binaries {}

# standard
# --------
pkgtools::install $argv

