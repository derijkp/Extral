tk appname test
source /home/peter/dev/Extral/experiment/lib/class.tcl

Class subclass Test
Test method new {} {
	private $object a b
	set a t1
	set b t2
}
Test method try {ai bi} {
	private $object a b
	set a $ai
	set b $bi
}
Test method test {} {
	private $object a b
	puts "a:$a\nb:$b"
}
Test new try
try try 8 9
try test
time {try nop} 1000

Test try2
try2 try 9 10
try2 test








Class method try {ai bi} {
	private $object a b
	set a $ai
	set b $bi
}
Class method test {} {
	private $object a b
	puts "a:$a\nb:$b"
}
Class try 1 1 1
Class try 1
Class try 1 1
Class test
Class try 2 4
Class test


Class subclass Test
Test method new {} {
	private $object a b
	set a t1
	set b t2
}
Test method try {ai bi} {
	private $object a b
	set a $ai
	set b $bi
}
Test method test {} {
	private $object a b
	puts "a:$a\nb:$b"
}
Test method nop {} {}
Test try 1 1 1
Test try 1
Test try 1 1
Test test
time {Test nop} 100

Test new try
try try 8 9
try test
time {try nop} 1000

Test try2
try2 try 9 10
try2 test
