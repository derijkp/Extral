# tempfile.tcl --
#
# Handling of temporary files
#
# Copyright (c) 1996 Peter De Rijk
#
# See the file "README" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

# Set variable Extral__priv(temp_dir) to the temporary directory.
# Make one if one doesn't exist.
# 

if [info exists env(TEMP)] {
	set Extral__priv(temp_dir) $env(TEMP)
} elseif [file writable /usr/tmp] {
	set Extral__priv(temp_dir) /usr/tmp
} elseif [info exists env(HOME)] {
	set Extral__priv(temp_dir) [file join $env(HOME) tcltemp]
} else {
	set Extral__priv(temp_dir) [file join [pwd] tcltemp]
}

if ![file exists $Extral__priv(temp_dir)] {
	if [catch {mkdir $Extral__priv(temp_dir)}] {
		puts stdout "Didn't find temporary directory"
		puts stdout "Couldn't make temporary directory \"$Extral__priv(temp_dir)\""
	}
} elseif ![file isdirectory $Extral__priv(temp_dir)] {
	puts stdout "Didn't find temporary directory"
	puts stdout "Temporary directory \"$Extral__priv(temp_dir)\" exists, and is not a directory"
}

# Set lock
set num 1
while 1 {
	set file [file join $Extral__priv(temp_dir) xtrl$num.lck]
	if ![file exists $file] break
	incr num
}
set f [open $file w]
puts $f "lock"
close $f
set Extral__priv(templock) xtrl$num 
set Extral__priv(tempnum) 0
catch {rm [file join $Extral__priv(temp_dir) $Extral__priv(templock)_*]}
atexit add {tempfile clean}

# Procedures
# ----------------------------------------------------------------

proc tempfile {{action {get}}} {
	global Extral__priv
	switch $action {
		get {
			set i 1
			while 1 {
				set file [file join $Extral__priv(temp_dir) $Extral__priv(templock)_$i]
				if ![file exists $file] break
				incr i
			}
			set f [open $file w]
			puts $f ""
			close $f
			return $file
		}
		clean {
			catch {rm [file join $Extral__priv(temp_dir) $Extral__priv(templock)_*]}
			catch {rm [file join $Extral__priv(temp_dir) $Extral__priv(templock).lck]}
		}
	}
}
