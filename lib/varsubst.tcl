# Copyright (c) 1996 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

if 0 {
proc Extral::varsubst {} {}
}
Extral::export {varsubst} {

#doc {convenience varsubst} cmd {
#varsubst varlist string
#} descr {
#	substitutes only the variables in varlist for their content
#	in the given string
#} example {
#	% set try {try it}
#	try it
#	% varsubst {try} {
#		puts [list $try $try2]
#	}
#		puts [list {try it} $try2]
#}

proc varsubst {varlist string} {
	append string " "
	foreach arg $varlist {
		upvar $arg temp
		if ![info exists temp] {
			return -code error "can't read \"$arg\": no such variable"
		}
		regsub -all {&} [list $temp] {\\\&} rpattern
		regsub -all {\\([0-9])} $rpattern {\\\\\1} rpattern
		append rpattern "\\1"
		regsub -all -- "\\$\{[set arg]\}" $string $rpattern string
		regsub -all -- "\\$[set arg]\(\[^a-zA-Z0-9:\]\)" $string $rpattern string
	}
	regsub { $} $string {} string
	return $string
}

}