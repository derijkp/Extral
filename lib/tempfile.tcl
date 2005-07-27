# Handling of temporary files
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

namespace eval Extral {
	global env
	# Set variable temp_dir to the temporary directory.
	# Make one if one doesn't exist.
	# 
	
	if [info exists env(TEMP)] {
		set temp_dir $env(TEMP)
	} elseif [file writable /usr/tmp] {
		set temp_dir /usr/tmp
	} elseif [info exists env(HOME)] {
		set temp_dir [file join $env(HOME) tcltemp]
	} else {
		set temp_dir [file join [pwd] tcltemp]
	}
	
	if ![file exists $temp_dir] {
		if [catch {file mkdir $temp_dir}] {
			puts stdout "Didn't find temporary directory"
			puts stdout "Couldn't make temporary directory \"$temp_dir\""
		}
	} elseif ![file isdirectory $temp_dir] {
		puts stdout "Didn't find temporary directory"
		puts stdout "Temporary directory \"$temp_dir\" exists, and is not a directory"
	}
	
	# Set lock
	set num 1
	while 1 {
		set file [file join $temp_dir tempExtral$num.lck]
		if ![file exists $file] break
		incr num
	}
	set f [open $file w]
	puts $f "lock"
	close $f
	set templock tempExtral$num 
	set tempnum 0
	catch {file delete -force [file join $temp_dir ${templock}_*]}
	atexit add {tempfile clean}
}

# Procedures
# ----------------------------------------------------------------

#doc tempfile title {
#tempfile
#} shortdescr {
# get names for temporary files
#}

#doc {tempfile get} cmd {
#tempfile ?get? ?file|dir?
#} descr {
#	creates an (empty) temporary file and returns its name. You should remove
#	the temporary file when not used any longer. However, leftover temporary 
#	files will be removed by an atexit handler
#	The last parameter (file or dir) determines whether a file or directory is created   
#}
#doc {tempfile clean} cmd {
#tempfile clean
#} descr {
#	remove all temporary files for the running program.
#}
#doc {tempfile cleanall} cmd {
#tempfile cleanall
#} descr {
#	remove all temporary files, including those of other Extral programs.
#}
proc tempfile {{action {get}} {type file}} {
	upvar ::Extral::temp_dir temp_dir
	upvar ::Extral::templock templock
	switch $action {
		get {
			set i 1
			while 1 {
				set file [file join $temp_dir ${templock}_$i]
				if ![file exists $file] break
				incr i
			}
			if {"$type" == "dir"} {
				file mkdir $file
			} else {
				set f [open $file w]
				puts $f ""
				close $f
			}
			return $file
		}
		clean {
			catch {eval file delete -force [glob [file join $temp_dir ${templock}_*]]}
			catch {file delete -force [file join $temp_dir $templock.lck]}
		}
		cleanall {
			catch {eval file delete -force [glob [file join $temp_dir tempExtral*]]}
		}
		default {
			return -code error "bad option \"$action\": must be get, clean or cleanall"
		}
	}
}

#doc tempdir title {
#tempdir
#} shortdescr {
# returns a directory in which temporary files can be stored. This directory is specific to one proces:
# no other processes will (should) write in this directory. Subsequent calls to the function within one process
# will allways be the same directory, The program has to take care not to overwrite its own files
# 
#}

proc tempdir {} {
	upvar ::Extral::tempdir tempdir
	if {![info exists tempdir]} {
		set tempdir [tempfile get dir]
	}
	return $tempdir
}
