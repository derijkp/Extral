package require Extral

proc test {name description script expected {causeerror 0}} {
	global errors
	
	puts "testing $name: $description"
	proc try {} $script
	set error [catch try result]
	if $causeerror {
		if !$error {
			puts "test should cause an error\nresult is \n$result"
			lappend errors "$name:$description" "test should cause an error\nresult is \n$result"
			return
		}	
	} else {
		if $error {
			puts "test caused an error\nerror is \n$result\n"
			lappend errors "$name:$description" "test caused an error\nerror is \n$result\n"
			return
		}
	}
	if {"$result"!="$expected"} {
		puts "error: result is:\n$result\nshould be\n$expected"
		lappend errors "$name:$description" "error: result is:\n$result\nshould be\n$expected"
	}
	return
}


proc testsummarize {} {
global errors
if [info exists errors] {
	set error "***********************\nThere were errors in the tests"
	foreach {test err} $errors {
		append error "\n$test  ----------------------------"
		append error "\n$err"
	}
	error $error
}
}
