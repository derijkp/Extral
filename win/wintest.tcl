lappend auto_path /peter/dev
package require Extral 1.8
tk appname test
wm withdraw .
proc s {} {uplevel source lib/list_regsub.tcl}
wm withdraw .

for {set i 0} {$i<100} {incr i} {
	lappend try a b asdf g {fdg shg} "fdg \{sdf" fdg a b fsd
}
set big {}
for {set i 0} {$i<5000} {incr i} {
	lappend big [random 0 10000]
}

