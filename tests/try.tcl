if 0 {
	set auto_path {/opt/tcltk/lib/tcl8.2 /opt/tcltk/lib /home/peter/tmp/install/lib /home/peter/tmp/install/i386/lib}
	package require Extral
}
source tools.tcl

set value {
SPTREMBL; Q37382; Q37382.
SWISS-PROT; P46753; RT02_ACACA.
SWISS-PROT; P46754; RT03_ACACA.
}

string_change $value [list \; {} \n {}]

string_change $value "\; {} \n {}"

