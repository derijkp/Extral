#set auto_path {/home/peter/bin /usr/local/lib/tcl8.0 /usr/local/lib /usr/local/lib/tk8.0}
package require Peos 1
wm withdraw .
eval edit [glob lib/*.tcl] [glob src/*.c] extral.txt src/Makefile.in
eval edit try.tcl st [glob tests/*.tcl]
