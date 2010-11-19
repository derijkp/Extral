proc Extral::exec-get {o} {
#puts [list Extral::exec-get $o]
	global Extral::exec
	if {![info exists Extral::exec(progress,$o)] || [catch {eof $o} eof]} return
	if {!$eof} {
		set line [gets $o]
		catch {eof $o} eof
		if {!$eof || ($line ne "")} {
			append Extral::exec(result,$o) $line\n
			if {$Extral::exec(progress,$o) ne ""} {
				catch {uplevel #0 $Extral::exec(progress,$o) [list $line]}
			}
		}
	}
	if {$eof} {
		set Extral::exec(done,$o) 1
		if {[info exists Extral::exec(cmd,$o)]} {
			foreach {result err} [Extral::bgexec_close $o] break
			set cmd $Extral::exec(cmd,$o)
			unset Extral::exec(cmd,$o)
			uplevel #0 $cmd [list $result]
		}
	}
}

proc Extral::bgexec_close {o} {
#puts [list Extral::bgexec_close $o]
	global Extral::exec
	if {![info exists Extral::exec(progress,$o)]} {
		return {{} {}}
	}
	if {![eof $o]} {
		set pid [pid $o]
		catch {exec kill $pid}
	}
	catch {read $o}
	catch {close $o}
	after cancel set Extral::exec(done,$o) 1
	set result $Extral::exec(result,$o)
	if {$Extral::exec(err,$o) ne ""} {
		set err [file_read $Extral::exec(err,$o)]
	} else {
		set err {}
	}
	unset Extral::exec(result,$o)
	unset Extral::exec(err,$o)
	unset Extral::exec(done,$o)
	unset Extral::exec(progress,$o)
	unset -nocomplain Extral::exec(cancel,$o)
	if {$err ne ""} {
		return -code error $err
	}
	return [list $result $err]
}

proc Extral::bgexec_cancel {o} {
#puts [list Extral::bgexec_cancel $o]
	global Extral::exec
	if {![info exists Extral::exec(progress,$o)]} {
		return {}
	}
	after cancel set Extral::exec(done,$o) 1
	if {[info exists Extral::exec(cmd,$o)]} {
		foreach {result err} [Extral::bgexec_close $o] break
		set cmd $Extral::exec(cmd,$o)
		unset Extral::exec(cmd,$o)
		uplevel #0 $cmd {{}}
	} else {
		set Extral::exec(cancel,$o) 1
		set Extral::exec(done,$o) 1
	}
}

#doc {convenience Extral::bgexec} cmd {
#Extral::bgexec ?options? arg ?arg ...?
#} descr {
# Without the -command option, this command works like exec does, but runs the executed 
# processes in background. While the command will wait until the process is finished and returns the
# results, events will still be processed while the process is running; the bgexec
# does e.g. not block the interface from redisplaying when needed.<br>
# The command supports the folowing options
#<dl>
#<dt>-command ?command?</dt>
#<dd>
# With the -command option, bgerror does not wait for the process to finish. Instead, when the 
# process is finished, the command in the option will be run (toplevel scope) with the result appended.
# In plain Tcl, the event loop must be running. More than one background jobs can be run at the same time 
# using the -command option. The command supports the folowing options:
#</dd>
#<dt>-timeout number</dt>
#<dd>After the given number of miliseconds the process is stopped</dd>
#<dt>-progresscommand command</dt>
#<dd>This option is used to execute a command each time new data arrives. command is used as a prefix to run with the new data appended</dd>
#<dt>-channelvar</dt>
#<dd>name of a (global) variable to which the channel controlling the proces started wil be saved, can be used with Extral::bgexec_cancel to cancel running process</dd>
#<dt>-pidvar varName</dt>
#<dd>store the pid of the process in the variable varName</dd>
#<dt>-no_error_redir</dt>
#<dd>This option can turn of redirection of stderr; by default, if error output is present, bgexec will stop with an error, and the error output is in the result. Using this option, you can redirect error yourself, eg to stdout using \"2>@ stdput\" on programs where stderr is used for progress reporting</dd>
#</dl>
#} example {
# Extral::bgexec ./testcmd_bgexec.tcl
# Extral::bgexec -command {set v} ./testcmd_bgexec.tcl 2
# vwait ::v
#}
proc Extral::bgexec {args} {
	global Extral::exec
	cmd_args Extral::bgexec {
		-timeout {int "number of miliseconds after which to stop background process" {}}
		-progresscommand {any "code to call when new data arrives from background process, newly arrived data is added as an argument" ""}
		-command {any "code to call when new process is finished, the resulting data is added as an argument, bgexec will not wait till finished" ""}
		-pidvar {any "name of a (global) variable to which the pid of the proces started wil be saved" ""}
		-channelvar {any "name of a (global) variable to which the channel controlling the proces started wil be saved, can be used with Extral::bgexec_cancel to cancel running process" ""}
		-no_error_redir {switch "This option can turn of redirection of stderr; by default, if error output is present, bgexec will stop with an error, and the error output is in the result. Using this option, you can redirect error yourself, eg to stdout using \"2>@ stdput\" on programs where stderr is used for progress reporting" ""}
	} {cmd ?...?} $args
	set tempstderr [tempfile]
	if {[true $opt(-no_error_redir)]} {
		set o [open "| $cmd [join $args " "]"]
		set Extral::exec(err,$o) {}
	} else {
		set o [open "| $cmd [join $args " "] 2>>$tempstderr"]
		set Extral::exec(err,$o) $tempstderr
	}
	fconfigure $o -blocking 0
	set Extral::exec(progress,$o) $opt(-progresscommand)
	if {$opt(-pidvar) ne ""} {
		upvar #0 $opt(-pidvar) pid
		set pid [pid $o]
	}
	if {$opt(-channelvar) ne ""} {
		upvar #0 $opt(-channelvar) channelvar
		set channelvar $o
	}
	set Extral::exec(result,$o) {}
	fileevent $o readable [list Extral::exec-get $o]
	set Extral::exec(done,$o) 0
	if {[isint $opt(-timeout)]} {
		after $opt(-timeout) set Extral::exec(done,$o) 1
	}
	if {$opt(-command) eq ""} {
		vwait Extral::exec(done,$o)
		foreach {result err} [Extral::bgexec_close $o] break
		if {[info exists Extral::exec(cancel,$o)]} {
			return -code error "Action canceled"
		} elseif {$err ne ""} {
			return -code error $err
		} else {
			return $result
		}
	} else {
		set Extral::exec(cmd,$o) $opt(-command)
	}
}
