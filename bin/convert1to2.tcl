#!/bin/sh
# the next line restarts using tclsh \
exec tclsh8.0 "$0" "$@"

package require Extral 1.1
set Extral::changes {
	string_split string_split
	string_change string_change
	string_reverse string_reverse
	string_find string_find
	string_replace string_replace
	list_change list_change
	list_pop list_pop
	list_shift list_shift
	list_sub list_sub
	list_find list_find
	list_cor list_cor
	list_remdup list_remdup
	list_lremove list_list_remove
	list_remove list_remove
	list_merge list_merge
	list_unmerge list_unmerge
	list_reverse list_reverse
	list_push list_push
	list_unshift list_unshift
	list_set list_set
	list_arrayset list_arrayset
	list_common list_common
	list_union list_union
	list_eor list_eor
	list_addnew list_addnew
	inlist inlist
	list_load list_load
	list_write list_write
	list_regsub list_regsub
	list_iterate list_iterate
	list_next list_next
	file_read file_read
	file_write file_write
	"lmath_calc" lmath_calc
	"lmath_sum" lmath_sum
	"lmath_min" lmath_min
	"lmath_max" lmath_max
	"lmath_cumul" lmath_cumul
	"lmath_incr" lmath_incr
	"lmath_between" lmath_between
	"lmath_average" lmath_average
	"list_subindex" list_subindex
	"list_mangle" list_mangle
	"list_extract" list_extract
	"list_split" list_split
	"list_join" list_join
	"list_lengths" list_lengths
	"list_fill" list_fill
	"list_ffill" list_ffill
	"array_lappend" array_lappend
	"array_lget" array_lget
	array_trans array_trans
	time_scan time_scan
	time_format time_format
	cmd_get cmd_get
	cmd_split cmd_split
	cmd_parse cmd_parse
	cmd_load cmd_load
	structlist_set structlist_set
	structlist_get structlist_get
	structlist_unset structlist_unset
	structlist_fields structlist_fields
	structlist_find structlist_find
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
