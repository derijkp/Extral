#!/bin/sh
# the next line restarts using wish \
exec tclsh8.0 "$0" "$@"

source tools.tcl

proc testtime {time} {
	set descr "scan and format: $time"
	set cmd "time_format \[time_scan \{$time\}\] \"%e %b %Y %H:%M:%S\""
	uplevel 0 {test time_scan $descr $cmd $time}
}

test time_scan {scan: 1 Jan 0001} {
	time_scan {1 Jan 0001}
} {0.0}

test time_format {format: 0.0} {
	time_format 0.0
} {0001 Jan 01 00:00:00}

test time_scan {scan: 1 Jan 0001 0:0:10} {
	time_scan {1 Jan 0001 0:0:10}
} {10.0}

test time_format {format: 10.0} {
	time_format {10.0}
} {0001 Jan 01 00:00:10}

test time_scan {scan: 2 Jan 0001} {
	time_scan {2 Jan 0001}
} {86400.0}

test time_format {format: 86400.0} {
	time_format 86400.0
} {0001 Jan 02 00:00:00}

test time_format {format: -86400.0} {
	time_format -86400.0
} {0001 BC Dec 31 00:00:00}

test time_scan {error: 1 Jan 0000} {
	time_scan {1 Jan 0000}
} {error while parsing date in "1 Jan 0000": (unfortunately) there is no year 0} 1

test time_scan {scan and format: 1 Feb 0001} {
	time_format [time_scan {1 Feb 0001}]
} {0001 Feb 01 00:00:00}

test time_scan {scan and format: 31 Jan 0001 1:30} {
	time_format [time_scan {31 Jan 0001 1:30}]
} {0001 Jan 31 01:30:00}

test time_scan {scan and format: 31 Dec 0001 23:59 BC} {
	time_format [time_scan {31 Dec 0001 23:59 BC}]
} {0001 BC Dec 31 23:59:00}

test time_scan {scan and format: 31 Jan 0001 BC 23:50} {
	time_format [time_scan {31 Jan 0001 BC 23:50}]
} {0001 BC Jan 31 23:50:00}

test time_scan {scan and format: 1 Feb 0001 BC} {
	time_format [time_scan {1 Feb 0001 BC}]
} {0001 BC Feb 01 00:00:00}

test time_scan {Thu May  4 12:13:09 EDT 1995} {
	time_format [time_scan {Thu May  4 12:10:09 EDT 1995}]
} {1995 May 04 12:10:09}

test time_scan {format: -60*60*24} {
	time_format [expr -60*60*24]
} {0001 BC Dec 31 00:00:00}

test time_scan {format: 60*60*24} {
	time_format [expr 60*60*24]
} {0001 Jan 02 00:00:00}

test time_scan {scan and format: 1 Feb 0001 BC 1:30} {
	time_format [time_scan {1 Feb 0001 BC 1:30}]
} {0001 BC Feb 01 01:30:00}

test time_scan {1 Jan 0002} {
	time_format [time_scan {1 Jan 0002}]
} {0002 Jan 01 00:00:00}

test time_scan {1 Jan 1500} {
	time_format [time_scan {1 Jan 1500}]
} {1500 Jan 01 00:00:00}

test time_scan {4 Jan 1994} {
	time_format [time_scan {4 Jan 1994}]
} {1994 Jan 04 00:00:00}

test time_scan {Dec 31 1994 dfg} {
	time_format [time_scan {Dec 31 1994 dfg}]
} {1994 Dec 31 00:00:00}

test time_scan {4/1/1994} {
	time_format [time_scan {4/1/1994}]
} {1994 Apr 01 00:00:00}

test time_scan {8/31/0050 23:59:59:90} {
	time_format [time_scan {8/31/0050 23:59:59:90}]
} {0050 Aug 31 23:59:59}

test time_scan {8/31/0050 bc 23:59:59:90} {
	time_format [time_scan {8/31/0050 bc 23:59:59:90}]
} {0050 BC Aug 31 23:59:59}

test time_scan {time} {
	time_scan {1:30:40} time
} {5440.0}

test time_scan {hour error} {
	time_scan {40:30:40} time
} {error while parsing time in "40:30:40": invalid hour} 1

test time_scan {date: check endspace} {
	time_format [time_scan {5/20/1995 }]
} {1995 May 20 00:00:00}

test time_scan {month error} {
	time_scan {15/4/1994}
} {error while parsing date in "15/4/1994": invalid month} 1

test time_scan {day error} {
	time_scan {1/40/1994}
} {error while parsing date in "1/40/1994": invalid day} 1

test time_scan {date: schrikkeljaar 1997 error} {
	time_scan {29 Feb 1997}
} {error while parsing date in "29 Feb 1997": invalid day} 1

test time_scan {date: schrikkeljaar 2000} {
	time_format [time_scan {29 Feb 2000}]
} {2000 Feb 29 00:00:00}

test time_scan {date: schrikkeljaar 1900 error} {
	time_scan {29 Feb 1900}
} {error while parsing date in "29 Feb 1900": invalid day} 1

test time_scan {} {
	time_format [time_scan {1 Jan 1500 bc}]
} {1500 BC Jan 01 00:00:00}

test time_scan {1 Jan 1500 bc 23:59:59:90} {
	time_format [time_scan {1 Jan 1500 bc 23:59:59:90}]
} {1500 BC Jan 01 23:59:59}

test time_scan {4/1/1600 bc 18:23:59} {
	time_format [time_scan {4/1/1600 bc 18:23:59}]
} {1600 BC Apr 01 18:23:59}

test time_scan {1/1/1994 1:30:40} {
	time_format [time_scan {1/1/1994 1:30:40}]
} {1994 Jan 01 01:30:40}

test time_scan {date: schrikkeljaar 1996} {
	time_format [time_scan {29 Feb 1996}]
} {1996 Feb 29 00:00:00}

test time_scan {date: schrikkeljaar 1996} {
	time_format [time_scan {09/08/1996 0:0:0}]
} {1996 Sep 08 00:00:00}

test time_format {} {
	time_format [time_scan {29 Feb 1996 17:30:15:80}] "%% %Y %d %e %j %m %b %B %H %M %S %s"
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
