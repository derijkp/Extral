source tools.tcl

test list_sub {} {
	list_sub {Ape Ball Field {Antwerp city} Egg} {0 3}
} {Ape {Antwerp city}}

test list_sub {exclude} {
	list_sub {Ape Ball Field {Antwerp city} Egg} -exclude {0 3}
} {Ball Field Egg}

test list_sub {exclude {}} {
	list_sub {Ape Ball Field Egg} -exclude {}
} {Ape Ball Field Egg}

test list_sub {list_sub with {}} {
	list_sub {Ape Ball Field} {}
} {}

test list_sub {negative index} {
	list_sub {Ape Ball Field} {1 -1 100}
} {Ball}

test list_sub {problem} {
	set findex {{Acibri.ACR: Acidianus brierleyi} {Aciinf.ACR: Acidianus infernus} {Desmob.ACR: Desulfurococcus mobilis} {Thecel.AEU: Thermococcus celer}}
	set pattern "\\.ACR"
	set poss [list_find -regexp $findex $pattern]
	set result [list_sub $findex $poss]
	set rest [list_sub $findex -exclude $poss]
} {{Thecel.AEU: Thermococcus celer}}

test list_sub {-exclude assumes positions are sorted} {
	list_sub {a b c d} -exclude {3 0}
} {b c}

rm -rf /home/peter/build/tca/Windows-intel/exts/Extral2.0.4/lib
cp -r /home/peter/build/tca/Linux-i686/exts/Extral2.0.4/lib /home/peter/build/tca/Windows-intel/exts/Extral2.0.4/
