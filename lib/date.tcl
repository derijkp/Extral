# Tcl version of scantime and formattime
#
# Copyright (c) 1997 Peter De Rijk
#
# See the file "README.txt" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# =============================================================

Extral::export {scantime formattime} {

proc scantime {time {musthave date}} {
	foreach {var val} {
		bc 0 year -1 month -1 day -1 hour -1 min -1 sec -1 ms -1
		first -1 second -1 days -1 schrikkel 0
	} {
		set $var $val
	}
	switch $musthave {
		date {set musthavedate 1;set musthavetime 0}
		time {set musthavedate 0;set musthavetime 1}
		both {set musthavedate 1;set musthavetime 1}
	}
	set list [split $time "/- _"]
	foreach item $list {
		switch -glob $item {
			{[0-9][0-9][0-9][0-9]*} {
				set year [string trimleft $item 0]
				if {"$year"==""} {set year 0}
			}
			{*:*} {
				foreach item [split $item ":"] var {hour min sec ms} {
					set $var $item
				}
			}
			{[0-9]*} {
				set temp [string trimleft $item 0]
				if {"$temp"==""} {set temp 0}
				if {$first == -1} {
					set first $item
				} else {
					set second $item
				}
			}
			{[bB][cC]} {
				set bc 1
			}
			default {
				set i 1
				foreach m {Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec} {
					if {"$item" == "$m"} {set month $i}
					incr i
				}
			}
		}
	}
	if {$month == -1} {
		set month $first
		set day $second
	} else {
		set day $first
	}

	if $musthavedate {
		if {$year == -1} {error "no year found"}
		if {$month == -1} {error "no month found"}
		if {$day == -1} {error "no day found"} 
	}
	if $musthavetime {
		if {$hour == -1} {error "no hour found"}
		if {$min == -1} {error "no minutes found"}
	}
	foreach var {year month day} {
		if {[set $var] == -1} {set $var 1}
	}
	foreach var {hour min sec ms} {
		if {[set $var] == -1} {set $var 0}
		if {[set $var] == ""} {set $var 0}
	}
	if {($month<1)||($month>12)} {
		error "error while parsing date in \"$time\": invalid month"
	}
#puts "$year $month $day $hour:$min:$sec:$ms"

	# Start calculation
	if {$year == 0} {
		error "error while parsing date in \"$time\": (unfortunately) there is no year 0"
	} elseif $bc {
		set i $year
	} else {
		set i [expr {$year-1}]
	}
	set result [expr {$i*365+int($i/4)-int($i/100)+int($i/400)}]
	set schrikkel 0
	if {[expr {$year%4}] == 0} {
		if {([expr {$year%100}] != 0)||([expr {$year%400}] == 0)} {
			set schrikkel 1;
		}
	}
	if $bc {
		set result [expr {-$result}]
	}

	switch $month {
		1 {
			set days 31;
		} 
		2 {
			set days [expr {28+$schrikkel}]
			incr result 31;
		}
		3 {
			set days 31;
			incr result [expr {59+$schrikkel}]
		}
		4 {
			set days 30;
			incr result [expr {90+$schrikkel}]
		}
		5 {
			set days 31;
			incr result [expr {120+$schrikkel}]
		}
		6 {
			set days 30;
			incr result [expr {151+$schrikkel}]
		}
		7 {
			set days 31;
			incr result [expr {181+$schrikkel}]
		}
		8 {
			set days 31;
			incr result [expr {212+$schrikkel}]
		} 
		9 {
			set days 30;
			incr result [expr {243+$schrikkel}]
		} 
		10 {
			set days 31;
			incr result [expr {273+$schrikkel}]
		} 
		11 {
			set days 30;
			incr result [expr {304+$schrikkel}]
		} 
		12 {
			set days 31;
			incr result [expr {334+$schrikkel}]
		}
	}
	if {($day<1)||($day>$days)} {
		error "error while parsing date in \"$time\": invalid day"
	}

	incr result [expr {$day-1}]
#puts "$days\n$year $month $day $hour:$min:$sec:$ms"
	return [expr {$result*86400.0 + $hour*3600 + $min*60 + $sec + $ms/100.0}]
}

proc formattime {time {format {%Y %b %d %H:%M:%S}}} {
	set bc 0
	if ($time<0) {
		set bc 1;
		set time [expr -$time]
	}
	set days [expr int($time/86400.0)]
	set year [expr int($days/365.25)]
	while 1 {
		set i [expr $year+1]
		set i [expr $i*365+int($i/4)-int($i/100)+int($i/400)]
		if $bc {
			if {$i>=$days} break
		} elseif {$i>$days} break
		incr year
	}
	set seconds [expr $time - $days*86400.0]
	set days [expr $days - ($year*365+int($year/4)-int($year/100)+int($year/400))]
	incr year
	set schrikkel 0
	if {[expr $year%4] == 0} {
		if {([expr $year%100] != 0)||([expr $year%400] == 0)} {
			set schrikkel 1;
		}
	}
	if {$bc == 1} {
		if {$seconds == 0} {
			set days [expr 365 + $schrikkel - $days + 1]
		} else {
			set days [expr 365 + $schrikkel - $days]
			set seconds [expr 86400.0 - $seconds]
		}
	} else {
		incr days
	}
	if {$days>[expr {334+$schrikkel}]} {
		set day [expr {$days-(334+$schrikkel)}]
		set smonth "December";
		set month 12;
	} elseif {$days>[expr {304+$schrikkel}]} {
		set day [expr {$days-(304+$schrikkel)}]
		set smonth "November";
		set month 11;
	} elseif {$days>[expr {273+$schrikkel}]} {
		set day [expr {$days-(273+$schrikkel)}]
		set smonth "October"
		set month 10
	} elseif {$days>[expr {243+$schrikkel}]} {
		set day [expr {$days-(243+$schrikkel)}]
		set smonth "September"
		set month 9
	} elseif {$days>[expr {212+$schrikkel}]} {
		set day [expr {$days-(212+$schrikkel)}]
		set smonth "August"
		set month 8
	} elseif {$days>[expr {181+$schrikkel}]} {
		set day [expr {$days-(181+$schrikkel)}]
		set smonth "July"
		set month 7
	} elseif {$days>[expr {151+$schrikkel}]} {
		set day [expr {$days-(151+$schrikkel)}]
		set smonth "June"
		set month 6
	} elseif {$days>[expr {120+$schrikkel}]} {
		set day [expr {$days-(120+$schrikkel)}]
		set smonth "May"
		set month 5
	} elseif {$days>[expr {90+$schrikkel}]} {
		set day [expr {$days-(90+$schrikkel)}]
		set smonth "April"
		set month 4
	} elseif {$days>[expr {59+$schrikkel}]} {
		set day [expr {$days-(59+$schrikkel)}]
		set smonth "March"
		set month 3
	} elseif ($days>31) {
		set day [expr {$days-31}]
		set smonth "February"
		set month 2
	} else {
		set day $days
		set smonth "January"
		set month 1
	}

	set hour [expr {int($seconds/3600.0)}]
	set seconds [expr {$seconds-($hour*3600.0)}]
	set min [expr {int($seconds/60.0)}]
	set seconds [expr {$seconds - ($min*60.0)}]
	set sec [expr {int($seconds)}]
	set seconds [expr {$seconds - $sec}]
	set ms [expr {round($seconds*100)}]

	set buffer ""
	set pos 0
	set len [string length $format]
	while {$pos < $len} {
		set char [string index $format $pos]
		if {"$char" == "%"} {
			incr pos
			set char [string index $format $pos]
			switch $char {
				% {
					append buffer "%"
				}
				Y {
					append buffer [format "%4.4d" $year]
					if $bc {
						append buffer " BC"
					}
				}
				d {
					append buffer [format "%2.2d" $day]
				}
				e {
					append buffer $day
				}
				j {
					append buffer [format "%3.3d" $days]
				}
				m {
					append buffer [format "%2.2d" $month]
				}
				b {
					append buffer [format "%3.3s" $smonth]
				}
				B {
					append buffer [format "%s" $smonth]
				}
				H {
					append buffer [format "%2.2d" $hour]
				}
				M {
					append buffer [format "%2.2d" $min]
				}
				S {
					append buffer [format "%2.2d" $sec]
				}
				s {
					append buffer [format "%2.2d" $ms]
				}
				default {
					error "format option $char not supported"
				}
			}
		} else {
			append buffer $char
		}
		incr pos
	}
	return $buffer
}

}
