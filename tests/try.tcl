source tools.tcl

test string_replace {replace over end} {
	string_replace "abcde" 4 5 23
} {abcd23}


	string_replace ABCDEFG 5 -1 ----

test string_replace {insert} {
	string_replace ABCDEFG 5 -1 ----
} ABCDE----FG

test string_replace {insert before} {
	string_replace ABCDEFG 0 -1 ----
} ----ABCDEFG

test string_replace {insert after} {
	string_replace ABCDEFG 10 -1 ----
} {ABCDEFG  ----}

test lmanip {subindex} {
	list_subindex {{a 1} {b {2 2}} {c 3}} 1
} {1 {2 2} 3}

test lmanip {subindex missing value} {
	list_subindex {{a 1} {b} {c 3}} 1
} {1 {} 3}

test lmanip {subindex} {
	list_subindex {{A a 1} {{B B} b 2} {c 3}} 2 0 1
} {{1 A a} {2 {B B} b} {3 C c}}

test lmanip {subindex negtive subindex} {
	list_subindex {{A a 1} {B b 2} {C c 3}} -1
} {{1 A a} {2 {B B} b} {3 C c}}

test lmanip {subindex large subindex} {
	list_subindex {{A a 1} {B b 2} {C c 3}} 100
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

