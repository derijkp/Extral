# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

#doc leval title {
#eval Light
#} shortdescr {
#a faster but more limited eval (Viktor Dukhovni)
#}
#doc {leval leval} cmd {
#leval command $args
#} descr {
#	converted the leval patch by Viktor Dukhovni <viktor@esm.com> to
#	a dynamically loadable version:
#	This command is a fast light "eval" specifically designed to execute
#	zero or more Tcl lists (concatenated) by invoking the command specified
#	by the first list element, with the remaining list elements as "literal"
#	arguments.  No variable or command substitution takes place on the
#	arguments.
#	The Tcl only version is not faster
#}

proc leval {args} {
	eval [eval concat $args]
}
