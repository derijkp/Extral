proc Extral::dbmtype_gdbm {} {
	if {"[::Extral::dbm implementation]" != "tcl"} {
		if [file exists [file join $::Extral::dir dbm gdbm[info sharedlibextension]]] {
			return 1
		}
	}
	return 0
}

proc Extral::dbminittype_gdbm {} {
	load [file join $::Extral::dir dbm gdbm[info sharedlibextension]]
}
