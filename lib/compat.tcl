proc Extral::compat {} {
namespace eval ::string {}
namespace eval ::list {}
foreach {old new} {
	::string_split string_split
	::string_change string_change
	::string_reverse string_reverse
	::string_find string_find
	::string_replace string_replace
	::list_change list_change
	list_pop list_pop
	list_shift list_shift
	list_sub list_sub
	list_find list_find
	list_cor list_cor
	list_remdup list_remdup
	list_lremove list_lremove
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
	array_trans array_trans
	time_scan time_scan
	time_format time_format
	cmd_get cmd_get
	cmd_split cmd_split
	cmd_parse cmd_parse
	cmd_load cmd_load
} {
	catch {interp alias {} $old {} $new}
}

catch {
proc lmath {option args} {
	eval lmath_$option $args
}
}

catch {
proc lmanip {option args} {
	eval list_$option $args
}
}

catch {
proc amanip {option args} {
	eval array_$option $args
}
}

}
# obsolete 
# ffind
