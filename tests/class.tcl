tk appname test
source tools.tcl

proc clean {} {
	catch {Class destroy}
}

test class {object parent} {
	clean
	Class new try
	try parent
} {Class}

test class {object command} {
	clean
	Class new try
	try
} {bad option: should be one of class, destroy, parent} 1

test class {object command} {
	clean
	Class new try
	try destroy
	try
} {invalid command name "try"} 1

test class {don't overwrite objects} {
	clean
	Class new try
	Class new try
} {object "try" exists} 1

test class {don't overwrite classes} {
	clean
	Class subclass Test
	Class subclass Test
} {class "Test" exists} 1

test class {subclass parent} {
	clean
	Class subclass Subclass
	Subclass parent
} {Class}

test class {subclass cmd} {
	clean
	Class subclass Subclass
	Subclass
} {wrong # args: should be "Subclass option ..."} 1

test class {subclass methods} {
	clean
	Class subclass Subclass
	Subclass methods
} {class destroy parent}

test class {subclass destroy: test command} {
	clean
	Class subclass Test
	Test destroy
	Test
} {invalid command name "Test"} 1

test class {subclass with instance destroy: test command} {
	clean
	Class subclass Test
	Test new try
	Test destroy
	try
} {invalid command name "try"} 1

test class {subclass with instance destroy: test children} {
	clean
	Class subclass Test
	Test new try
	Test destroy
	Class children
} {}

test class {subclass with instance and subclass destroy: test children} {
	clean
	Class subclass Test
	Test new try
	Test subclass Test2
	Test destroy
	Class children
} {}

test Class {subclass destroy: test method} {
	clean
	Class subclass Test
	Test destroy
	Test method nop {} {}
} {invalid command name "Test"} 1


test class {Class destroy: destroyed subclass?} {
	clean
	Class subclass Subclass
	Class destroy
	Subclass
} {invalid command name "Subclass"} 1

test class {add method} {
	clean
	Class method nop {} {}
	Class methods
} {class destroy nop parent}

test class {add method: works?} {
	clean
	Class method try {} {return ok}
	Class try
} {ok}

test class {add method: works with arguments} {
	clean
	Class method try {a} {return $a}
	Class try ok
} {ok}

test class {add method: wrong # arguments} {
	clean
	Class method try {a} {return $a}
	Class try
} {wrong # args: should be "Class try a"} 1

test class {subclass inherits new methods} {
	clean
	Class method nop {} {}
	Class subclass Subclass
	Subclass methods
} {class destroy nop parent}

test class {inherit method: works?} {
	clean
	Class method try {} {return ok}
	Class subclass Subclass
	Subclass try
} {ok}

test class {inherit method: works with arguments} {
	clean
	Class method try {a} {return $a}
	Class subclass Subclass
	Subclass try ok
} {ok}

test class {inherit method: wrong # arguments} {
	clean
	Class method try {a} {return $a}
	Class subclass Subclass
	Subclass try
} {wrong # args: should be "Subclass try a"} 1

test class {new} {
	clean
	Class subclass Subclass
	Class new try
	Class new try2
	Class children
} {Subclass try try2}

test class {redefining new} {
	clean
	Class subclass Test
	Test method new {} {
		return [list [$parent init $class $object] 1]
	}
	Test subclass Test2
	Test2 method new {} {
		return [list [$parent init $class $object] 2]
	}
	Test2 new try
} {{try 1} 2}

test class {redefining new: test class} {
	clean
	Class subclass Test
	Test method new {} {
		return [list [$parent init $class $object] 1]
	}
	Test subclass Test2
	Test2 method new {} {
		return [list [$parent init $class $object] 2]
	}
	Test2 new try
	try class
} {Test2}

test Class {method} {
	clean
	Class subclass Test
	Test method try {} {return try}
	Test try
} {try}

test Class {redefine destroy: check redefinition} {
	clean
	Class subclass Test
	Test method destroy {} {return ok}
	Test new try
	try destroy
} {ok}

test Class {redefine destroy: check destruction} {
	clean
	Class subclass Test
	Test method destroy {} {return ok}
	Test new try
	try destroy
	try
} {invalid command name "try"} 1

test Class {redefine destroy: give arguments error} {
	clean
	Class subclass Test
	Test method destroy {test} {return ok}
} {destroy method cannot have arguments} 1

test Class {set private vars in new} {
	clean
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
	Test method nop {} {}
	Test method test {} {
		private $object a b
		return "$a $b"
	}
	Test new try
	try test
} {t1 t2}

test Class {set private vars in method} {
	clean
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
	Test method nop {} {}
	Test method test {} {
		private $object a b
		return "$a $b"
	}
	Test new try
	try try 8 9
	try test
} {8 9}

test Class {test method arguments} {
	clean
	Class method try {ai bi} {
		private $object a b
		set a $ai
		set b $bi
	}
	Class try 1 1 1
} {wrong # args: should be "Class try ai bi"} 1

test Class {test method arguments} {
	clean
	Class method try {ai bi} {
		private $object a b
		set a $ai
		set b $bi
	}
	Class try 1
} {wrong # args: should be "Class try ai bi"} 1

test Class {test common variables} {
	clean
	Class method set {val} {
		common $class a
		set a $val
	}
	Class method test {} {
		common $class a
		return $a
	}
	Class new try1
	Class new try2
	try1 set 1
	try2 set 2
	try1 test
} {2}
