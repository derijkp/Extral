#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

package require Extral 1.1
set Extral::changes {
	string::split string_split
	string::change string_change
	string::reverse string_reverse
	string::find string_find
	string::replace string_replace
	list::change list_change
	lpop list_pop
	lshift list_shift
	lsub list_sub
	lfind list_find
	lcor list_cor
	lremdup list_remdup
	llremove list_lremove
	lremove list_remove
	lmerge list_merge
	lunmerge list_unmerge
	lreverse list_reverse
	lpush list_push
	lunshift list_unshift
	lset list_set
	larrayset list_arrayset
	lcommon list_common
	lunion list_union
	leor list_eor
	laddnew list_addnew
	oneof inlist
	lload list_load
	lwrite list_write
	lregsub list_regsub
	literate list_iterate
	lnext list_next
	readfile file_read
	writefile file_write
	"lmath calc" lmath_calc
	"lmath sum" lmath_sum
	"lmath min" lmath_min
	"lmath max" lmath_max
	"lmath cumul" lmath_cumul
	"lmath incr" lmath_incr
	"lmath between" lmath_between
	"lmath average" lmath_average
	"lmanip subindex" list_subindex
	"lmanip mangle" list_mangle
	"lmanip extract" list_extract
	"lmanip split" list_split
	"lmanip join" list_join
	"lmanip lengths" list_lengths
	"lmanip fill" list_fill
	"lmanip ffill" list_ffill
	"amanip lappend" array_lappend
	"amanip get" array_lget
	arraytrans array_trans
	scantime time_scan
	formattime time_format
	getcomplete cmd_get
	splitcomplete cmd_split
	parsecommand cmd_parse
	cload cmd_load
}

proc convert {files} {
	foreach file $files {
		if [file isdir $file] {
			convert [glob -nocomplain [file join $file *]]
		} elseif {"[file extension $file]" == ".tcl"} {
			puts "Converting $file"
			set c [file_read $file]
			set c [string_change $c $::Extral::changes]
			file_write $file $c
		}
	}
}

convert $argv
