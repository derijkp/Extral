package require Peos 1
wm withdraw .
eval edit [glob [pwd]/lib/*.tcl] [glob [pwd]/src/*.c] [pwd]/extral.txt [pwd]/src/Makefile
