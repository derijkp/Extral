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
time {lpop try 1}
puts $try
