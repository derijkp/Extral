if 0 {
	set auto_path {/opt/tcltk/lib/tcl8.2 /opt/tcltk/lib /home/peter/tmp/install/lib /home/peter/tmp/install/i386/lib}
	package require Extral
}
source tools.tcl

test structlist_get-list {list test type, 1 to end} {
	set struct {a {*list {*int ?}}}
	set try {a {1 2 3 4}}
	structlist_get -struct $struct $try {a {0 end {lmath_sum}}}
} {10}


