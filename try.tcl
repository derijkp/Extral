source tools.tcl

set cmd test
set options {}
set vars {?a b? c d}
set arg {1 2 3}

test cmd_args {optional range with error} {
	catch {unset a}
	catch {unset b}
	cmd_args test {} {?a b? c} {1 2}
} {wrong # of args: should be "test ?a b? c"} 1

load ./compress.so
set f [open ~/lsu/total.ali]
#set f [open /IDE2/peter/ssu/scomp_jan1999.ali]
catch {gets $f ; gets $f ; gets $f ; gets $f}
catch {set seq [gets $f]}
close $f
time {set try [compressseq $seq]}
string length $try

time {set try [compress -level 1 $seq]}

set test " - - - - - - - - - - - - - - - - - - - - - - - - - - - - A G C "
set test " - - - - A G - - C - - - - A G - - C - - - - A G - - C"
catch {set try [compress $test]}
string length $test
string length $try
uncompress $try


package require Extral 1

set try {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}}
structlist_unset $try {c c a}
structlist_unset $try {c a}
structlist_unset $try c
structlist_unset $try {c d}

set try

set auto_path {/home/peter/dev/Extral /home/peter/bin/Peos}
package require Extral 1
tk appname test
wm withdraw .

catch {unset try}
set try {a 1 b 2 c 3 d 4 e 5 f 6 g 7 h 8 i 9 j 10}
time {
structlist_get try g
} 100

catch {unset try}
for {set i 0} {$i<100} {incr i} {
lappend try tryingit$i tryingit$i
}
time {
structlist_get try tryingit98
} 100


# 210
time {
structlist_set $try g try
}
# 501 422
time {
structlist_unset $try g
}

set file temp
#set file /data/test/ssu/temp
time {
set c [lfile_read $file]
set try [lindex $c 10]
set try2 [lindex $c 11]
}

time {
list_file_write temp2 $c
}

proc file_read {file} {
	set f [open $file]
	fconfigure $f -buffersize 100000
	set c [read $f]
	close $f
	return $c
}

time {
	set c [file_read $file]
	set try [lindex $c 10]
	set try2 [lindex $c 11]
}


proc ptime args {
time {uplevel $args}
}

for {set i 0} {$i<100} {incr i} {
	lappend try a b asdf g {fdg shg} "fdg \{sdf" fdg a b fsd
}
set big {}
for {set i 0} {$i<5000} {incr i} {
	lappend big [random 0 10000]
}

time {replace {thg th rty ty ey} {a $ t # y \\&}} 100

time {list_remove $try {b a}} 10
time {list_remove $big {1 4 5}} 10
time {list_remove {1 2 8067 3} $big} 10

list_subindex {{a 1} {b 2} {c 3}} 1

time {list_remdup $try} 100
time {list_remdup $big}
time {lmanip2 remdup $big}
set temp $big
ptime list_remove temp 100 1000

set list [list_find $try a]
list_sub {a b c d} {1 3}
list_sub {a b c d} -except {}
ptime list_sub $try $list
set newtry [lsort $try]
ptime list_cor {a b c d e f g h i j} {f i g a h b c d e j}
ptime list_cor $try $newtry
ptime list_remove temp a

set long [list_fill 10000 1 1]
time {list_regsub {c$} $long {!}}

load extral.so
ssort -reflist {a a b a b c b a} {1 2 3 4 5 6 7 8}

set list1 {a {b sdf} c {d dsfg}}
set list2 {c {d dsfg} {e sd} f}

set try $list
time {eval list_remove try $args}

ssort -dictionary -increasing -reflist {{org 2} {org 20} {org 10} {org 8}} {1 2 3 4}

set try {dfg {gfxh j} "File dfg"}
list_shift try
set try

package require Peos
array set table {Action 1 Adjust 2 Menu 3 ActionM "B1" AdjustM "B2" MenuM "B3"}

proc Peos__button {event {object .}} {
	foreach type {Action Adjust Menu} {
		if {[string first $type $event]!=-1} {
			regexp {^(.*)([1-5])$} [option get $object button$type Button$type] temp mod button
			regsub "$type\$" $event $button end
			if [info exists end] {
				set event $mod$end
			}
			regsub -all $type $event ${mod}B$button event
		}
	}
	return $event
}

proc Peos__button {event {object .}} {
	global table
	set list [split $event "-"]
	set last [list_pop list]
	foreach el $list {
		if [info exists table($el)] {
			append result $table(${el}M)-
		} else {append result $el-}
	}
	if [info exists table($last)] {
		append result $table($last)
	} else {append result $el}
	return $result
}

time {Peos__button Adjust-Motion-Action .} 50
time {Peos__button Action .} 50

proc Peos__bind {window patterns command {w {}}} {
	if {"$w"!=""} {
	} elseif [winfo exists $window] {
		set w $window
	} else {
		if {"[winfo class .peos__temp]"!="$window"} {
			destroy .peos__temp
			frame .peos__temp -class $window
		}
		set w .peos__temp
	}
	if [regexp {^(.*)Key(.*)$} $patterns temp pre key] {
		set keys [option get $w key$key Key$key]
		foreach key $keys {
			bind $window <$key> $command
		}
	} else {
#		regsub Action$ $patterns 1 patterns
#		regsub Adjust$ $patterns 2 patterns
#		regsub Menu$ $patterns 3 patterns
#		regsub -all Action $patterns 1 patterns
#		regsub -all Adjust $patterns 2 patterns
#		regsub -all Menu $patterns 3 patterns
		foreach pattern $patterns {
#			bind $window <$pattern> $command
			bind $window <[Peos__button $pattern $w]> $command
		}
	}
}

set try {a b c d e f g}
set try {a {b c} d e {ff sfgh} g}
list_pop try 4
list_pop try
time {
	set try {a b c d e f g}
	list_pop try 1
} 100
puts $try

replace Adjust-Motion-Action {Action- B1- -Action -1 Adjust- B2- -Adjust -2}
time {replace Adjust-Motion-Action {Action- B1- -Action -1 Adjust- B2- -Adjust -2}} 100

set trydata {
menu
	menu file "File" Alt-f
	menu find "Find"
	action Trytop "Test" {%W insert insert "Test: %W"} Alt-t
	check SearchReopen "Search Reopen" {-variable [test %W] -onvalue yes -offvalue no} Control-Alt-t
menu file
	action Load "Open file" {%W insert insert "Open: %W"}
	action LoadNext "Open next" {%W insert insert "Open next: %W"}
	action Try "Test" {%W insert insert "Test: %W"}
	menu file.try "Trying"
	action Save Save {%W save}
# The find menu
menu find
	action Goto "Goto line" {Peos__InputBox %W.goto -label "Goto line" -title Goto -buttontext Goto -command {%W gotoline [%W.goto get]}}
	action Find "Find" {Peos__Editor__finddialog %W}
	separator
	action ReplaceFindNext "Replace & Find next" {%W replace-find -forwards}
	check SearchReopen "Search Reopen" {-variable test%W -onvalue yes -offvalue no}
	action FindFunction "Find Tcl function" {%W findfunction}
menu file.try
	action Try "Trying" {puts try}
}
	set data [split $trydata "\n"]
	set data [list_remove $data {}]
	set lines [list_sub $data -exclude [list_find -regexp $data {^#}]]
	foreach line $lines {
		if [regexp {^menu} $line] {
			set men [lindex $line 1]
			if {"$men"==""} {set curmenu $menu} else {set curmenu $menu.$men}
			menu $curmenu
			set num 1
		} else {
			set type [list_shift line]
			set key [list_shift line]
			set text [list_shift line]
		}
	}

