#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

package require pkgtools
cd [pkgtools::startdir]

# settings
# --------

set srcdir [file dir [pkgtools::startdir]]
lappend auto_path $srcdir
package require Extral
Extral::makedoc [glob -nocomplain $srcdir/lib/*.tcl $srcdir/libnoc/*.tcl] $srcdir/docs/html
