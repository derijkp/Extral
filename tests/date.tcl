#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

proc testtime {time} {
	set descr "scan and format: $time"
	set cmd "formattime \[scantime \{$time\}\] \"%e %b %Y %H:%M:%S\""
	uplevel 0 {test scantime $descr $cmd $time}
}

test scantime {scan: 1 Jan 0001} {
	scantime {1 Jan 0001}
} {0.0}

test formattime {format: 0.0} {
	formattime 0.0
} {0001 Jan 01 00:00:00}

test scantime {scan: 1 Jan 0001 0:0:10} {
	scantime {1 Jan 0001 0:0:10}
} {10.0}

test formattime {format: 10.0} {
	formattime {10.0}
} {0001 Jan 01 00:00:10}

test scantime {scan: 2 Jan 0001} {
	scantime {2 Jan 0001}
} {86400.0}

test formattime {format: 86400.0} {
	formattime 86400.0
} {0001 Jan 02 00:00:00}

test formattime {format: -86400.0} {
	formattime -86400.0
} {0001 BC Dec 31 00:00:00}

test scantime {error: 1 Jan 0000} {
	scantime {1 Jan 0000}
} {error while parsing date in "1 Jan 0000": (unfortunately) there is no year 0} 1

test scantime {scan and format: 1 Feb 0001} {
	formattime [scantime {1 Feb 0001}]
} {0001 Feb 01 00:00:00}

test scantime {scan and format: 31 Jan 0001 1:30} {
	formattime [scantime {31 Jan 0001 1:30}]
} {0001 Jan 31 01:30:00}

test scantime {scan and format: 31 Dec 0001 23:59 BC} {
	formattime [scantime {31 Dec 0001 23:59 BC}]
} {0001 BC Dec 31 23:59:00}

test scantime {scan and format: 31 Jan 0001 BC 23:50} {
	formattime [scantime {31 Jan 0001 BC 23:50}]
} {0001 BC Jan 31 23:50:00}

test scantime {scan and format: 1 Feb 0001 BC} {
	formattime [scantime {1 Feb 0001 BC}]
} {0001 BC Feb 01 00:00:00}

test scantime {format: -60*60*24} {
	formattime [expr -60*60*24]
} {0001 BC Dec 31 00:00:00}

test scantime {format: 60*60*24} {
	formattime [expr 60*60*24]
} {0001 Jan 02 00:00:00}

test scantime {scan and format: 1 Feb 0001 BC 1:30} {
	formattime [scantime {1 Feb 0001 BC 1:30}]
} {0001 BC Feb 01 01:30:00}

test scantime {1 Jan 0002} {
	formattime [scantime {1 Jan 0002}]
} {0002 Jan 01 00:00:00}

test scantime {1 Jan 1500} {
	formattime [scantime {1 Jan 1500}]
} {1500 Jan 01 00:00:00}

test scantime {4 Jan 1994} {
	formattime [scantime {4 Jan 1994}]
} {1994 Jan 04 00:00:00}

test scantime {Dec 31 1994 dfg} {
	formattime [scantime {Dec 31 1994 dfg}]
} {1994 Dec 31 00:00:00}

test scantime {4/1/1994} {
	formattime [scantime {4/1/1994}]
} {1994 Apr 01 00:00:00}

test scantime {8/31/0050 23:59:59:90} {
	formattime [scantime {8/31/0050 23:59:59:90}]
} {0050 Aug 31 23:59:59}

test scantime {8/31/0050 bc 23:59:59:90} {
	formattime [scantime {8/31/0050 bc 23:59:59:90}]
} {0050 BC Aug 31 23:59:59}

test scantime {time} {
	scantime {1:30:40} time
} {5440.0}

test scantime {hour error} {
	scantime {40:30:40} time
} {error while parsing time in "40:30:40": invalid hour} 1

test scantime {date: check endspace} {
	formattime [scantime {5/20/1995 }]
} {1995 May 20 00:00:00}

test scantime {month error} {
	scantime {15/4/1994}
} {error while parsing date in "15/4/1994": invalid month} 1

test scantime {day error} {
	scantime {1/40/1994}
} {error while parsing date in "1/40/1994": invalid day} 1

test scantime {date: schrikkeljaar 1997 error} {
	scantime {29 Feb 1997}
} {error while parsing date in "29 Feb 1997": invalid day} 1

test scantime {date: schrikkeljaar 2000} {
	formattime [scantime {29 Feb 2000}]
} {2000 Feb 29 00:00:00}

test scantime {date: schrikkeljaar 1900 error} {
	scantime {29 Feb 1900}
} {error while parsing date in "29 Feb 1900": invalid day} 1

test scantime {} {
	formattime [scantime {1 Jan 1500 bc}]
} {1500 BC Jan 01 00:00:00}

test scantime {1 Jan 1500 bc 23:59:59:90} {
	formattime [scantime {1 Jan 1500 bc 23:59:59:90}]
} {1500 BC Jan 01 23:59:59}

test scantime {4/1/1600 bc 18:23:59} {
	formattime [scantime {4/1/1600 bc 18:23:59}]
} {1600 BC Apr 01 18:23:59}

test scantime {1/1/1994 1:30:40} {
	formattime [scantime {1/1/1994 1:30:40}]
} {1994 Jan 01 01:30:40}

test scantime {date: schrikkeljaar 1996} {
	formattime [scantime {29 Feb 1996}]
} {1996 Feb 29 00:00:00}

test scantime {date: schrikkeljaar 1996} {
	formattime [scantime {09/08/1996 0:0:0}]
} {1996 Sep 08 00:00:00}

test formattime {} {
	formattime [scantime {29 Feb 1996 17:30:15:80}] "%% %Y %d %e %j %m %b %B %H %M %S %s"
} {% 1996 29 29 060 02 Feb February 17 30 15 80}

testtime {1 Jan 0001 BC 00:00:00}
testtime {5 Jan 0001 BC 00:00:00}
testtime {1 Jan 0001 00:00:00}
testtime {31 Dec 0050 BC 00:00:00}
testtime {1 Jan 0050 00:00:00}
testtime {1 Jan 0100 BC 00:00:00}
testtime {1 Jan 1500 BC 00:00:00}
testtime {1 Apr 0050 BC 23:59:59}
testtime {18 Mar 15000 BC 15:00:00}
testtime {18 Mar 15000 15:00:00}
testtime {1 Jun 0003 00:00:00}
testtime {9 May 1997 12:30:24}
testtime {9 May 1997 12:30:24}

testsummarize
