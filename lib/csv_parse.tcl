proc csv_parse {data {sep ,}} {
	set result {}
	set resultline {}
	set newline 0
	set data [split $data \n]
	if {[info exists quotedstring]} {unset quotedstring}
	set quotereplace {{""} {"}}
	set quoteconnect $sep
	foreach line $data {
		set line [split $line $sep]
		foreach el $line {
			if {![info exists quotedstring]} {
				if {[string equal [string index $el 0] \"]} {
					if {[regexp {([^"]|\A)("")*"$} $el]} {
						lappend resultline [string map $quotereplace [string range $el 1 end-1]]
					} else {
						set quotedstring [string range $el 1 end]
					}
				} else {
					lappend resultline $el
				}
			} else {
				if {[regexp {([^"]|\A)("")*"$} $el]} {
					append quotedstring $quoteconnect[string range $el 0 end-1]
					set quotedstring [string map $quotereplace $quotedstring]
					lappend resultline $quotedstring
					unset quotedstring
				} else {
					append quotedstring $quoteconnect$el
				}
				set quoteconnect $sep
			}
		}
		if {![info exists quotedstring]} {
			lappend result $resultline
			set resultline ""
		} else {
			set quoteconnect \n
		}
	}
	if {[llength $resultline]}	{lappend result $resultline}
	return $result
}
