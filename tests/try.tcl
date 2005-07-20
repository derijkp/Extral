source tools.tcl

test lmanip {subindex} {
	list_subindex {{a 1} {b {2 2}} {c 3}} 1
} {1 {2 2} 3}

test lmanip {subindex missing value} {
	list_subindex {{a 1} {b} {c 3}} 1
} {1 {} 3}

test lmanip {subindex} {
	list_subindex {{A a 1} {{B B} b 2} {c 3}} 2 0 1
} {{1 A a} {2 {B B} b} {3 C c}}


set value {
SPTREMBL; Q37382; Q37382.
SWISS-PROT; P46753; RT02_ACACA.
SWISS-PROT; P46754; RT03_ACACA.
}

puts 1
string_change $value [list \; {} \n {}]

puts 2
string_change $value "\; {} \\n {}"

