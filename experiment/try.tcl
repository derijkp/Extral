set db ::object::db::db
Ds create try
Ds new db try
db addclass Test {i {*int ?} s {*string ?}}
db set Test/try1 i 1
db get Test/try1 i
db get Test/try1 s
db set Test/try1 {} {i 2 s try}
db get Test/try1 i
