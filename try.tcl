package require Extral 1
set try {a 1 b 2 c {a 1 b 2 c {a 1 b 2}}}
taglunset $try {c c a}
taglunset $try {c a}
taglunset $try c
taglunset $try {c d}

set try

set auto_path {/home/peter/dev/Extral /home/peter/bin/Peos}
package require Extral 1
tk appname test
wm withdraw .

catch {unset try}
set try {a 1 b 2 c 3 d 4 e 5 f 6 g 7 h 8 i 9 j 10}
time {
taglget try g
} 100

catch {unset try}
for {set i 0} {$i<100} {incr i} {
lappend try tryingit$i tryingit$i
}
time {
taglget try tryingit98
} 100


# 210
time {
taglset $try g try
}
# 501 422
time {
taglunset $try g
}

set file temp
#set file /data/test/ssu/temp
time {
set c [lreadfile $file]
set try [lindex $c 10]
set try2 [lindex $c 11]
}

time {
lwritefile temp2 $c
}

proc readfile {file} {
	set f [open $file]
	fconfigure $f -buffersize 100000
	set c [read $f]
	close $f
	return $c
}

time {
	set c [readfile $file]
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

time {lremove $try {b a}} 10
time {lremove $big {1 4 5}} 10
time {lremove {1 2 8067 3} $big} 10

lmanip subindex {{a 1} {b 2} {c 3}} 1

time {lremdup $try} 100
time {lremdup $big}
time {lmanip2 remdup $big}
set temp $big
ptime lremove temp 100 1000

set list [lfind $try a]
lsub {a b c d} {1 3}
lsub {a b c d} -except {}
ptime lsub $try $list
set newtry [lsort $try]
ptime lcor {a b c d e f g h i j} {f i g a h b c d e j}
ptime lcor $try $newtry
ptime lremove temp a

set long [lmanip fill 10000 1 1]
time {lregsub {c$} $long {!}}

ffind -regexp -matches -allmatches [glob ../test/*] "\norg:(\[^\n\]*)\n"
ffind -regexp -matches -allfiles [glob ../test/*] "\norg:(\[^\n\]*)\n" null
ffind -regexp [glob ../test/*] "\norg:(\[^\n\]*)\n"

load extral.so
ssort -reflist {a a b a b c b a} {1 2 3 4 5 6 7 8}

set list1 {a {b sdf} c {d dsfg}}
set list2 {c {d dsfg} {e sd} f}

set try $list
time {eval lremove try $args}

ffind -regexp -matches -allfiles [glob test/*] "\norg:(\[^\n\]*)\n" ::NULL::

ffind -regexp -matches -allfiles [glob test/*] "\norg:(\[^\n\]*)\n" ::NULL:: org "\nsrc:(\[^\n\]*)\n" none src
ffind -regexp -allmatches -matches [glob test/*] "\norg:(\[^\n\]*)\n" org "\n(s..):" try
ffind -regexp -matches -allfiles ::NULL:: [glob test/*] "\norg:(\[^\n\]*)\n"
ffind -regexp -matches -allfiles {} [glob test/*] "\naut:(\[^\n\]*)\n" aut "\norg:(\[^\n\]*)\n" org

ffind -regexp -matches -allmatches [glob test/*] "\nt2:(\[^\n\]*)\n"
ffind -regexp -matches -allfiles null [glob test/*] "\nt2:(\[^\n\]*)\n"
ffind -regexp [glob test/*] "\nt2:(\[^\n\]*)\n"

ssort -dictionary -increasing -reflist {{org 2} {org 20} {org 10} {org 8}} {1 2 3 4}

set try {dfg {gfxh j} "File dfg"}
lshift try
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
	set last [lpop list]
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
lpop try 4
lpop try
time {
	set try {a b c d e f g}
	lpop try 1
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
	set data [lremove $data {}]
	set lines [lsub $data -exclude [lfind -regexp $data {^#}]]
	foreach line $lines {
		if [regexp {^menu} $line] {
			set men [lindex $line 1]
			if {"$men"==""} {set curmenu $menu} else {set curmenu $menu.$men}
			menu $curmenu
			set num 1
		} else {
			set type [lshift line]
			set key [lshift line]
			set text [lshift line]
		}
	}

