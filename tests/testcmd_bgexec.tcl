#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

# testcmd_bgexec.tcl
if {![llength $argv]} {set max 3} else {set max [lindex $argv 0]}
if {[llength $argv] > 1} {puts stderr progress...}
if {![string is int $max]} {puts stderr "arg must be an integer"; exit 1}
for {set num 1} {$num <= $max} {incr num} {
	puts $num
	after 1000
}