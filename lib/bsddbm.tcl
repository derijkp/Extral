proc Extral::dbmtype_bsddbm {} {
	if {"[dbm implementation]" != "tcl"} {
		if [file exists [file join $::Extral::dir dbm bsddbm[info sharedlibextension]]] {
			return 1
		}
	}
	return 0
}

proc Extral::dbminittype_bsddbm {} {
	load [file join $::Extral::dir dbm bsddbm[info sharedlibextension]]
}

