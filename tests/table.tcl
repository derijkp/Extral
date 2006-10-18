#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

test table {fromlist} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5}}]
	list [table_fields $table] [table_tolist $table]
} {{id a b c} {{a 1 2 3} {b 4 5 {}}}}

test table {table_tolist} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 d2 d3}}]
	table_tolist $table {} {0 3}
} {{a 1 2 3} {d d1 d2 d3}}

test table {table_tolist} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 d2 d3}}]
	table_tolist $table {a c}
} {{1 3} {4 {}} {7 9} {d1 d3}}

test table {table_tolist} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 d2 d3}}]
	table_tolist $table {a c} {0 3}
} {{1 3} {d1 d3}}

test table {table_get} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5}}]
	table_get $table 1
} {id b a 4 b 5 c {}}

test table {table_get with fields} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 {b 5}}}]
	table_get $table 1 a b
} {4 {b 5}}

test table {table_get with one field} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 {b 5}}}]
	table_get $table 1 b
} {b 5}

test table {table_get with num field} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5}}]
	table_get $table 1 1
} 4

test table {table_get with the same field} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 {b 5}}}]
	table_get $table 1 a a
} {4 4}

test table {table_get with *} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5}}]
	table_get $table 0 *
} {a 1 2 3}

test table {table_col} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 {b 5}}}]
	table_col $table b
} {2 {b 5}}

test table {table_set and table_get} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5}}]
	table_set table 1 a a1 b b2
	table_get $table 1 id a b c
} {b a1 b2 {}}

test table {table_getid} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5}}]
	table_getid $table b
} {id b a 4 b 5 c {}}

test table {table_getid with fields} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5}}]
	table_getid $table b a b
} {4 5}

test table {table_getid with *} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5}}]
	table_getid $table a *
} {a 1 2 3}

test table {table_setid existing and table_getid} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5}}]
	table_setid table b a a1 b b2
	table_getid $table b id a b c
} {b a1 b2 {}}

test table {table_setid rename} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5}}]
	table_setid table b id b2
	table_getid $table b2
} {id b2 a 4 b 5 c {}}

test table {table_setid new and table_getid} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5}}]
	table_setid table c a 7 b 8 c 9
	table_getid $table c id a b c
} {c 7 8 9}

test table {table_setid new empty table} {
	set table [table_create {id a b c} {}]
	table_setid table c a 7 b 8 c 9
	table_getid $table c id a b c
} {c 7 8 9}

test table {table_delete} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 d2 d3}}]
	table_delete table 1 3
	table_tolist $table
} {{a 1 2 3} {c 7 8 9}}

test table {table_deleteids} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 d2 d3}}]
	table_deleteids table b d
	table_tolist $table
} {{a 1 2 3} {c 7 8 9}}

test table {table_find ==} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_find $table b == 5 id a b
} {{b 4 5} {d d1 5}}

test table {table_find == with index} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_index table b
	table_find $table b == 5 id a b
} {{b 4 5} {d d1 5}}

test table {table_find !=} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_find $table b != 5 id
} {a c}

test table {table_find != with index} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_index table b
	table_find $table b != 5 id
} {a c}

test table {table_find oneof} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_find $table b oneof {2 5} id
} {a b d}

test table {table_find oneof with index} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_index table b
	table_find $table b oneof {2 5} id
} {a b d}

test table {table_find !oneof} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_find $table b !oneof {2 5} id
} c

test table {table_find !oneof with index} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_index table b
	table_find $table b !oneof {2 5} id
} c

test table {table_slice} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_tolist [table_slice $table {1 3}]
} {{b 4 5 {}} {d d1 5 d3}}

test table {table_existsid} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	list [table_existsid $table b] [table_existsid $table test]
} {1 0}

test table {table_addfield} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_addfield table extra
	list [table_fields $table] [table_tolist $table]
} {{id a b c extra} {{a 1 2 3 {}} {b 4 5 {} {}} {c 7 8 9 {}} {d d1 5 d3 {}}}}

test table {table_addfield with def} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_addfield table extra N
	list [table_fields $table] [table_tolist $table]
} {{id a b c extra} {{a 1 2 3 N} {b 4 5 {} N} {c 7 8 9 N} {d d1 5 d3 N}}}

test table {table_delfield} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_delfield table b
	list [table_fields $table] [table_tolist $table]
} {{id a c} {{a 1 3} {b 4 {}} {c 7 9} {d d1 d3}}}

test table {table_renamefield} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_renamefield table b test
	list [table_fields $table] [table_getid $table b test]
} {{id a test c} 5}

test table {table_reorderfields} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_reorderfields table {id c a b}
	list [table_getid $table b b] [table_fields $table] [table_tolist $table]
} {5 {id c a b} {{a 3 1 2} {b {} 4 5} {c 9 7 8} {d d3 d1 5}}}

test table {table_foreach} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	set result {}
	table_foreach table {c a} {
		lappend result [list $c $a]
	}
	set result
} {{3 1} {{} 4} {9 7} {d3 d1}}

test table {table_foreach with limited poss} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	set result {}
	table_foreach table {c a} {
		lappend result [list $c $a]
	} {1 2}
	set result
} {{{} 4} {9 7}}

test table {table_foreach with difficult variablename} {
	set ::Extral::table,test [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	set result {}
	table_foreach ::Extral::table,test {c a} {
		lappend result [list $c $a]
	} {1 2}
	set result
} {{{} 4} {9 7}}

test table {table_index} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	set t1 [lindex [time {set v1 [table_find $table b == 5]}] 0]
	table_index table b
	set t2 [lindex [time {set v2 [table_find $table b == 5]}] 0]
	list [expr {$v1 eq $v2}] [expr {$t2 < $t1}]
} {1 1}

test table {table_set and table_append with index} {
	set table [table_fromlist {id a b c} {{a 1 2 3} {b 4 5} {c 7 8 9} {d d1 5 d3}}]
	table_index table b
	table_set table 1 b 4
	table_set table 0 b 5
	table_append table id e a 1 b 2
	table_append table id f a 4 b 5
	table_find $table b == 5
} {0 3 5}

test table {create empty table} {
	set table [table_create {id a b c} {}]
	table_size $table
} 0

test table {empty table from list} {
	set table [table_fromlist {id a b c} {}]
	table_size $table
} 0

if 0 {
	set file /data/peter/molgen-project/maaike/skea.csv
	set f [open $file]
	set data [csv_file $f \t]
	close $f
	set fields [lindex $data 0]
	set list [lrange $data 1 end]
	
	set table [table_fromlist $fields $list]
	set keep $table
	table_get $table 1000
	set id skea2c-I1814
	table_getid $table $id
	table_setid table $id comment test
	table_getid $table $id comment
	table_setid table skel6785 comment test
	table_col $table comment
	table_find $table comment == test
	table_get $table [table_find $table id == $id]
	table_set table 1000 comment try
	table_get $table 1000 comment
	table_set table 30000 comment try2
	table_append table id test firstn test familyn it comment added
	table_getid $table test
	table_setid table test2 firstn test2 familyn it comment added2
	table_getid $table test2 firstn familyn
	table_setid table test firstn try
	table_getid $table test firstn familyn
}

testsummarize
