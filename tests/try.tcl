package require Extral

source tools.tcl

set value {
SPTREMBL; Q37382; Q37382.
SWISS-PROT; P46753; RT02_ACACA.
SWISS-PROT; P46754; RT03_ACACA.
}

puts 1
string_change $value [list \; {} \n {}]

puts 2
string_change $value "\; {} \\n {}"

