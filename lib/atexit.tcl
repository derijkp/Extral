# Create atexit handler
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================
#doc atexit title {
#atexit
#}

auto_load Extral::laddnew
auto_load Extral::lremove
rename exit ::Extral::exit
	proc exit {{returnCode 0}} {
		global Extral::atexit
		if [info exists Extral::atexit] {
			foreach command $Extral::atexit {
				eval $command
			}
		}
		Extral::exit $returnCode
	}


Extral::export {atexit} {

#doc {atexit atexit} cmd {
#atexit add command
#} descr {
#	adds a command to the atexit handler: This command will be executed when
#	the program exits. It can be used to do a cleanup. This command redefines
#	the exit command. If you use it in Tk, and you exit by calling "destroy ."
#	it will not work. You can redefine the destroy command to call exit when it 
#	has . as an argument.
#}
proc atexit {action command} {
	variable atexit
	switch $action {
		add {
			laddnew atexit $command
		}
		remove {
			lremove atexit $command
		}
	}
}

}