wish
load /usr/people/peter/tcl/tk4.1/Extral0.91/libextral.so.0.91 ExtraL
source extral.tcl
#lmanip join {{a 1} {b 2} {c {3 aaa}} {d 4}} {#} 1
wm deiconify .;set files [glob *.ref]
set pattern1 "\nt1:(\[^\n\]+)\n"
set pattern2 "\nt2:(\[^\n\]+)\n"
ffind -all -matches -regexp $files $pattern1

ffind -all -matches -regexp $files $pattern1 try1 $pattern2 try2
set try1

set files [glob /usr/local/anonymous/pub/ssu/*]
ffind $files "\nta1:Bacteria"
