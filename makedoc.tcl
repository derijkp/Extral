#!/bin/sh
# the next line restarts using tclsh \
exec tclsh8.0 "$0" "$@"

package require Extral
Extral::makedoc [lsort [glob lib/*.tcl]] docs Extral
