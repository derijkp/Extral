package require Peos 1
wm withdraw .
eval edit [glob lib/*.tcl] [glob src/*.c] extral.txt src/Makefile.in
eval edit try.tcl st [glob tests/*.tcl]
