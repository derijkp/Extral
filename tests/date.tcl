#!/usr/local/bin/tclsh8.0
source tools.tcl

set max 1994/01/01#10:30:20:99

test date-scandate {date} {
	scandate {4 Jan 1994}
} {1994/01/04}

test date-scandate {date} {
	scandate {Jan 4 1994 dfg}
} {1994/01/04}

test date-scandate {date} {
	scandate {4/1/1994}
} {1994/01/04}

test date-scandate {month error} {
	scandate {4/15/1994}
} {error while parsing date in "4/15/1994": impossible month} 1

test date-scandate {day error} {
	scandate {40/1/1994}
} {error while parsing date in "40/1/1994": impossible day} 1

test date-scandate {time} {
	scandate {1:30:40}
} {01:30:40}

test date-scandate {hour error} {
	scandate {40:30:40}
} {error while parsing time in "40:30:40": impossible hour} 1

test date-scandate {date and time} {
	scandate {1/1/1994 1:30:40}
} {1994/01/01#01:30:40}

testsummarize
