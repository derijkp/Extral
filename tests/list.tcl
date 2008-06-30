#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test list_regsub {c$} {
	list_regsub {c$} {afdsg asdc sfgh {dfgh shgfc} dfhg} {!}
} {afdsg asd! sfgh {dfgh shgf!} dfhg}

test list_regsub {^([^.]+)\.([^.]+)$} {
	list_regsub {^([^.]+)\.([^.]+)$} {start.sh help.ps h.sh} {\2 \1}
} {{sh start} {ps help} {sh h}}

test list_find {} {
	list_find -regexp {Ape Ball Field {Antwerp city} Egg} {^A}
} {0 3}

test list_find {} {
	list_find -inlist {Ape Ball Field {Antwerp city} Egg} {Antwerp}
} {3}

test list_find {} {
	list_find -oflist {Ape Ball Field Egg {Antwerp city} Egg} {Ape Egg}
} {0 3 5}

test list_find {} {
	list_find -lcommon {Ape Ball Field {Antwerp city} Egg} {Antwerp Egg}
} {3 4}

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

test list_sub {bugfix: -exclude assumed positions are sorted} {
	list_sub {a b c d} -exclude {3 0}
} {b c}

test list_cor {} {
	list_cor {a b c d e f} {d b}
} {3 1}

test list_cor {2 times} {
	list_cor {a b c d e f} {b d d}
} {1 3 -1}

test list_cor {number bug} {
	list_cor {hobbit.seq orc.seq} {sphinx.seq hobbit.seq orc.seq centaur.seq}
} {-1 0 1 -1}

test list_remdup {} {
	list_remdup {a b c a b d}
} {a b c d}

test list_remdup {sorted} {
	list_remdup -sorted {a a b b c d}
} {a b c d}

test list_remdup {check removed} {
	list_remdup {a b c a b d b} temp
	set temp
} {a b b}

test list_remdup {sorted, check removed} {
	list_remdup -sorted {a a b b b c d} temp
	set temp
} {a b b}

test list_remdup {large} {
	set list {}
	for {set i 0} {$i < 10000} {incr i} {lappend list "try $i"}
	lappend list "try 100" "try 200"
	llength [list_remdup $list]
} 10000

test list_remdup {small sorted} {
	set list {}
	for {set i 0} {$i < 10} {incr i} {lappend list "try $i"}
	lappend list "try 1" "try 2"
	set list [lsort $list]
	llength [list_remdup -sorted $list]
} 10

test list_remdup {large sorted} {
	set list {}
	for {set i 0} {$i < 10000} {incr i} {lappend list "try $i"}
	lappend list "try 100" "try 200"
	set list [lsort $list]
	llength [list_remdup -sorted $list]
} 10000

test list_merge {} {
	list_merge {a b c} {1 2 3}
} {a 1 b 2 c 3}

test list_merge {size 2} {
	list_merge {a b c d} {1 2} 2
} {a b 1 c d 2}

test list_merge {first too long} {
	list_merge {a b c d e} {1 2}
} {a 1 b 2 c {} d {} e {}}

test list_merge {second too long} {
	list_merge {a b} {1 2 3}
} {a 1 b 2}

test list_merge {second too long, spacing 2} {
	list_merge {a b c d} {1 2 3} 2
} {a b 1 c d 2}

test list_merge {first too long, spacing 2} {
	list_merge {a b c d e f} {1 2} 2
} {a b 1 c d 2 e f {}}

test list_unmerge {} {
	list_unmerge {a 1 b 2 c 3}
} {a b c}

test list_unmerge {size 2} {
	list_unmerge {a b 1 c d 2} 2 var
} {a b c d}

test list_unmerge {size 2, test var} {
	list_unmerge {a b 1 c d 2 e f 3} 2 var
	set var
} {1 2 3}

test list_unmerge {size 2, test strange string} {
	list_unmerge {a b 1 c d 2 e} 2 var
} {a b c d e}

test list_unmerge {size 2, test var with strange string} {
	list_unmerge {a b 1 c d 2 e} 2 var
	set var
} {1 2}

test list_pop {} {
	set try {a b c}
	list_pop try 1
} {b}

test list_pop {several times} {
	set try {a b c}
	list_pop try 1
	list_pop try
	list_pop try
} {a}

test list_pop {empty} {
	set try {a b c}
	list_pop try 1
	list_pop try
	list_pop try
	list_pop try
} {}

test list_pop {stays empty} {
	set try {a b c}
	list_pop try 1
	list_pop try
	list_pop try
	list_pop try
	list_pop try
} {}

test list_shift {} {
	set try {a b}
	list_shift try
} {a}

test list_shift {2} {
	set try {a b}
	list_shift try
	list_shift try
} {b}

test list_shift {empty} {
	set try {a b}
	list_shift try
	list_shift try
	list_shift try
} {}

test list_shift {stays empty} {
	set try {a b}
	list_shift try
	list_shift try
	list_shift try
	list_shift try
} {}

test list_push {} {
	list_push try a
} {a}

test list_push {2} {
	list_push try a
	list_push try b
} {a b}

test list_unshift {} {
	list_push try a
	list_push try b
	list_unshift try 1
} {1 a b}

test list_set {} {
	set try {a b c d}
	list_set try test {1 3}
} {a test c test}

test list_arrayset {} {
	list_arrayset a {a b c} {1 2 3}
	array get a
} {a 1 b 2 c 3}

test list_common {} {
	list_common {a b c d} {a d e} {a d f h}
} {a d}

test list_common {} {
	list_common {d} {a b c d e}
} {d}

test list_common {} {
	list_common {hobbit.seq orc.seq} {sphinx.seq hobbit.seq orc.seq centaur.seq}
} {hobbit.seq orc.seq}

test list_common {} {
	set selfiles {Lepbor.BCY Lepfov.BCY Lepsp.BCY Oscaga.BCY Osccor.BCY Trisp10.BCY Pseaga.BPG Psealc.BPG Pseamy.BPG Pseasp.BPG Pseaur.BPG Psecic.BPG Psecit.BPG Psecor.BPG Psefic.BPG Pseflu.BPG Psemar.BPG Pseole.BPG Pseres.BPG Psetol.BPG Psevir.BPG Xanalb.BPG Xanaxo.BPG Xancam.BPG Xanfra.BPG Xanory.BPG Xanpop.BPG Haesan.EAN Eisfet.EAN Lancon.EAN Nerlim.EAN Eurcal.EAN Artsal.EAN Tenmol.EAN Linlin.EAN Anesul.EAN Ochery.EAN Alcgel.EAN Barben.EAN Pedcer.EAN Lepsqu.EAN Barvir.EAN Galtak.EAN Glysp.EAN Trisp.EAN Scuven.EAN Aplsp.EAN Balbip.EAN Burran.EAN Faslig.EAN Laealt.EAN Limkam.EAN Litlit.EAN Litobt.EAN Monlab.EAN Nassin.EAN Neralb.EAN Onccel.EAN Oxysp.EAN Pisstr.EAN Sipalg.EAN Thacla.EAN Acajap.EAN Lepcor.EAN Antvul.EAN Goraqu.EAN Linsp.EAN Phoarc.EAN Bipsp.EAN Sibfio.EAN Pricau.EAN Brapli.EAN Phagra.EAN Ridpis.EAN Ascapi.EFU Sclscl.EFU Conapo.EFU Conper.EFU Consp.EFU Phadem.EFU Sarcru.EFU Moncas.EFU Sorfim.EFU Canalb.EFU Cantro.EFU Clalus.EFU Debhan.EFU Dekbru.EFU Dipalb.EFU Endfib.EFU Galgeo.EFU Hanuva.EFU Issori.EFU Klupol.EFU Metbic.EFU Picano.EFU Picmem.EFU Saclud.EFU Saccap.EFU Tordel.EFU Wallip.EFU Zygrou.EFU Filneo.EFU Tricut.EFU Bulalb.EFU Leusco.EFU Rhoglu.EFU Sporos.EFU Ustmay.EFU Lotvac.EPR Hypcat.EPR Palpal.EPR}
	set files {Bipsp.EAN Sibfio.EAN}
	list_common $files $selfiles
} {Bipsp.EAN Sibfio.EAN}

test list_common {large} {
	set list1 [list_fill 200 1 1]
	set list2 [list_fill 200 100 1]
	set list [list_common $list1 $list2]
	llength $list
} {101}

test list_union {} {
	list_union {a b c} {c d e}
} {a b c d e}

test list_eor {} {
	list_eor {a b c} {c d e}
} {a b d e}

test list_remove {} {
	set try {a b a c}
	list_remove $try a
} {b c}

test list_remove {2} {
	set try {a b a c}
	list_remove $try a c
} {b}

test list_lremove {} {
	set try {a b a c}
	list_lremove $try {a c}
} {b}

test list_lremove {large} {
	set try {}
	set try2 {}
	for {set i 0} {$i < 10000} {incr i} {lappend try "try $i"}
	for {set i 1} {$i < 10000} {incr i} {lappend try2 "try $i"}
	set list [lsort $try]
	list_lremove $list $try2
} {{try 0}}

test list_lremove {large sorted} {
	set try {}
	set try2 {}
	for {set i 0} {$i < 10000} {incr i} {lappend try "try $i"}
	for {set i 1} {$i < 10000} {incr i} {lappend try2 "try $i"}
	set list [lsort $try]
	set removelist [lsort $try2]
	list_lremove -sorted $list $removelist
} {{try 0}}

test list_lremove {difficult cases} {
	set list {a b bd ab ab ac}
	set removelist {ab b}
	list_lremove $list $removelist
} {a bd ac}

test list_lremove {difficult cases, sorted} {
	set list {a ab ab ac b bd}
	set removelist {ab b}
	list_lremove -sorted $list $removelist
} {a ac bd}

test list_lremove {difficult cases, check removed} {
	set list {a b bd ab ab ac}
	set removelist {ab b try}
	list_lremove $list $removelist temp
	set temp
} {b ab ab}

test list_lremove {difficult cases, sorted, check removed} {
	set list {a ab ab ac b bd}
	set removelist {ab b try}
	list_lremove -sorted $list $removelist temp
	set temp
} {ab ab b}

test list_lremove {empty} {
	list_lremove 1 {}
} {1}

test list_addnew {exists} {
	set try {a b}
	list_addnew try a
} {a b}

test list_addnew {new} {
	set try {a b}
	list_addnew try c
} {a b c}

test list_pop {test duplicate} {
	set list {a b c}
	set l $list
	list_pop list
	set l
} {a b c}

test list_shift {test duplicate} {
	set list {a b c}
	set l $list
	list_shift list
	set l
} {a b c}

test list_shift {object tests} {
	set data {{fdfg sdfg gh} sdfh {dgh sdh}}
	set line [lindex $data 0]
	list_shift line
	set line
} {sdfg gh}

test list_shift {with foreach} {
	set try {a b c d e}
	foreach t $try {
		list_shift t
		list_shift t
	}
	foreach t $try {
		list_shift t
		list_shift t
	}
} {}

test list_reverse {basic} {
	list_reverse {{a b} c {d e}}
} {{d e} c {a b}}

test list_change {basic} {
	list_change {a b aa c aa g} {aa x g y}
} {a b x c x y}

test list_select {basic} {
	list_select {a b ab bc} a*
} {a ab}

test list_select {-regexp} {
	list_select -regexp {a ab aa bc} {^[ab]*$}
} {a ab aa}

test list_concat {2} {
	list_concat {a b c} {1 2 3}
} {a b c 1 2 3}

test list_concat {3} {
	list_concat {a b c} {d e f} {1 2 3}
} {a b c d e f 1 2 3}

test list_concat {1} {
	list_concat {{a b c} {1 2 3}}
} {a b c 1 2 3}

test inlist {true} {
	inlist {a b c} b
} 1

test inlist {false} {
	inlist {a b c} d
} 0

test list_foreach {basic} {
	set result {}
	list_foreach {a b} {{1 2} {3 4}} {lappend result $a,$b}
	set result
} {1,2 3,4}

test list_foreach {different lengths} {
	set result {}
	list_foreach {a b} {{1 2 3} {4}} {lappend result $a,$b}
	set result
} {1,2 4,}

test list_foreach {different number of entries in separate lists} {
	set result {}
	set alist 3
	set blist {}
	list_foreach a $alist {b1 b2} $blist {
		lappend result $a $b1 $b2
	}
	set result
} {3 {} {}}

test list_lappend {basic} {
	set list {{a 1} {b 2}}
	list_lappend list 1 c
} {{a 1} {b 2 c}}

test list_lappend {no index} {
	set list {{a 1} {b 2}}
	list_lappend list c
} {{a 1} {b 2} c}

test list_lappend {2 index levels} {
	set list {{a 1} {b 2}}
	list_lappend list 0 1 c
} {{a {1 c}} {b 2}}

test list_lappend {index end} {
	set list {{a 1} {b 2}}
	list_lappend list end c
} {{a 1} {b 2 c}}

# no test yet for
# list_load <filename>
# list_write ?file? ?list?

testsummarize
