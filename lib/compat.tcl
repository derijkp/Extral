proc Extral::compat {} {
namespace eval ::string {}
namespace eval ::list {}
foreach {old new} {
	::string::split string_split
	::string::change string_change
	::string::reverse string_reverse
	::string::find string_find
	::string::replace string_replace
	::list::change list_change
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
	arraytrans array_trans
	scantime time_scan
	formattime time_format
	getcomplete cmd_get
	splitcomplete cmd_split
	parsecommand cmd_parse
	cload cmd_load
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
