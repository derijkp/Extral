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

# quick install (lib only)
rm -rf /home/peter/build/tca/Windows-intel/exts/Extral2.0.4
rm -rf /home/peter/build/tca/Windows-intel/exts/Extral2.0.4/lib
cp -r /home/peter/build/tca/Linux-i686/exts/Extral2.0.4/lib /home/peter/build/tca/Windows-intel/exts/Extral2.0.4/

# full compile and install linux
cd /home/peter/dev/Extral/Linux-i686
make distclean
../configure --prefix=/home/peter/tcl/dirtcl/dirtcl-build
make
rm -rf /home/peter/build/tca/Linux-i686/exts/Extral2.0.4
/home/peter/dev/Extral/build/install.tcl /home/peter/build/tca/Linux-i686/exts

# full cross-compile and install windows
cd /home/peter/dev/Extral/windows-intel
make distclean
cross-bconfigure.sh --prefix=/home/peter/tcl/win-dirtcl/dirtcl-build
cross-make.sh
rm -rf /home/peter/build/tca/Windows-intel/exts/Extral2.0.4
wine /home/peter/build/tca/Windows-intel/tclsh84.exe /home/peter/dev/Extral/build/install.tcl /home/peter/build/tca/Windows-intel/exts

