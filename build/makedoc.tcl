#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

cd [file dir [info script]]
cd ..
lappend auto_path [pwd]
package require Extral
file mkdir docs/html
Extral::makedoc [lsort [glob lib/*.tcl]] docs/html Extral {
	listcommands lmath stringcommands arraycommands cmd validatecommands
	map
	filing infocommands
	time atexit tempfile struct
	convenience ssort
}
