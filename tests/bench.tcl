package require Extral
# remove duplicates (keeping the sort order of the given list)
time {list_remdup {b a c d a e f g h i b j}} 5000
# -> C:25  Tcl:320

# merge two lists into one
time {list_merge {a b c d e f g h i j} {1 2 3 4 5 6 7 8 9 10}} 5000
# -> C:9   Tcl:170

# return the reverse of a string
time {string_reverse {abc def}} 5000
# -> C:3 Tcl:140
