package require Extral
source /home/peter/dev/Extral/experiment/lib/object.tcl
source /home/peter/dev/Extral/experiment/lib/datasitter.tcl

catch {file delete -force try}
Ds create try
Ds new db try
db addclass test {a {*int ?} b {*string ?}}

object new new
new method new {ai bi} {
	private $object a b
	set a $ai
	set b $bi
}
new method test {} {
	private $object a b
	puts "a:$a\nb:$b"
}
new new try 1 1 1
new new try 1
new new try 1 1
try test

new method try {a} {
	private $object try
	public $object p
	set p $a
	set try $a
}

new new try
try try 1
new method try {a} {puts [expr $a+1]}
try try 1

object method try {a} {puts $a}
object method nop {} {}
object try 1
time {object nop} 1000
object try

