package require Extral
catch {tk appname test}

#if ![info exists testleak] {
#	if {"$argv" != ""} {
#		set testleak [lindex $argv 0]
#	} else {
#		set testleak 0
#	}
#}
set testleak [get testleak 0]

proc putsvars args {
	foreach arg $args {
		puts [list set $arg [uplevel set $arg]]
	}
	puts "\n"
}

proc test {name description script expected {causeerror 0} args} {
	global errors testleak
	
	if ![info exists ::env(TCL_TEST_ONLYERRORS)] {puts "testing $name: $description"}
	proc tools__try {} $script
	set error [catch tools__try result]
	if $causeerror {
		if !$error {
			if [info exists ::env(TCL_TEST_ONLYERRORS)] {puts "-- test $name: $description --"}
			puts "test should cause an error\nresult is \n$result"
			lappend errors "$name:$description" "test should cause an error\nresult is \n$result"
			return
		}	
	} else {
		if $error {
			if [info exists ::env(TCL_TEST_ONLYERRORS)] {puts "-- test $name: $description --"}
			puts "test caused an error\nerror is \n$result\n"
			lappend errors "$name:$description" "-- test caused an error\nerror is \n$result\n"
			return
		}
	}
	if ![string_equal $result $expected] {
		if [info exists ::env(TCL_TEST_ONLYERRORS)] {puts "-- test $name: $description --"}
		puts "error: result is:\n$result\nshould be\n$expected"
		lappend errors "$name:$description" "error: result is:\n$result\nshould be\n$expected"
	}
	if $testleak {
		set line1 [lindex [split [exec ps l [pid]] "\n"] 1]
		time {set error [catch tools__try result]} $testleak
		set line2 [lindex [split [exec ps l [pid]] "\n"] 1]
		if {([lindex $line1 6] != [lindex $line2 6])||([lindex $line1 7] != [lindex $line2 7])} {
			if {"$args" != "noleak"} {
				if [info exists ::env(TCL_TEST_ONLYERRORS)] {puts "-- test $name: $description --"}
				puts "possible leak:"
				puts $line1
				puts $line2
				puts "\n"
			}
		}
	}
	return
}


proc testsummarize {} {
	if [info exists ::env(TCL_TEST_ONLYERRORS)] return
	global errors
	if [info exists errors] {
		set error "***********************\nThere were errors in the tests"
		foreach {test err} $errors {
			append error "\n$test  ----------------------------"
			append error "\n$err"
		}
		return -code error $error
	} else {
		puts "All tests ok"
	}
}

catch {unset errors}

if $testleak {
	test test {initialise all memory for testing with leak detection} {
		set try 1
	} 1 0 noleak
}
