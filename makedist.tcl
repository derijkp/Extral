#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

set targetdir $argv

#set targetdir $env(HOME)/src/[file tail [pwd]]0.9

file mkdir $targetdir
file copy Readme.txt pkgIndex.tcl extral.so $targetdir
file copy dbm docs extern lib src tests win $targetdir
