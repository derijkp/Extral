package require Extral

catch {namespace delete object}

proc putsvars {args} {
	uplevel [list foreach var $args {
		puts "$var:[set $var]"
	}]
	puts ""
}

namespace eval object {
	namespace eval methods {
	}
	namespace eval Class {
	}
	set methods::Class(method) object::method
	set methods::Class(subclass) object::subclass
	set parent(Class) ""
}

proc Class {args} {object::classcmd Class $args}

proc ::object::classcmd {class arg} {
	set cmd [lpop arg 0]
	if {"$cmd" == "new"} {
		set object [lindex $arg 0]
		::object::new $class $object
		if [info exists ::object::methods::${class}(new)] {
			leval [set ::object::methods::${class}(new)] [list $class] $arg
		}
	} elseif ![info exists ::object::methods::${class}($cmd)] {
		set object $cmd
		set cmd new
		set arg [concat [list $object] $arg]
		::object::new $class $object
		if [info exists ::object::methods::${class}(new)] {
			leval [set ::object::methods::${class}(new)] [list $class] $arg
		}
	} elseif [catch {leval [set ::object::methods::${class}($cmd)] [list $class] $arg} result] {
		global errorInfo
		set ::object::error $result
		set ::object::errorInfo $errorInfo
		if [regexp "called \"(.*)\" with too many arguments" $result temp fcmd] {
			set result "wrong # args: should be \"$class $cmd [lrange [info args $fcmd] 1 end]\""
		} elseif [regexp "no value given for parameter \".*\" to \"(.*)\"" $result temp fcmd] {
			set result "wrong # args: should be \"$class $cmd [lrange [info args $fcmd] 1 end]\""
		}
		return -code error -errorinfo ${::object::errorInfo} $result
	} else {
		return $result
	}
}

proc ::object::objectcmd {class object arg} {
	set cmd [lpop arg 0]
	if [catch {leval [set ::object::methods::${class}($cmd)] [list $object] $arg} result] {
		if ![info exists ::object::methods::${class}($cmd)] {
			set options [join [lsort [array names ::object::methods::${class}]] ", "]
			return -code error "bad option: should be one of $options"
		}
		global errorInfo
		set ::object::error $result
		set ::object::errorInfo $errorInfo
		if [regexp "called \"(.*)\" with too many arguments" $result temp fcmd] {
			set result "wrong # args: should be \"$class $cmd [lrange [info args $fcmd] 1 end]\""
		} elseif [regexp "no value given for parameter \".*\" to \"(.*)\"" $result temp fcmd] {
			set result "wrong # args: should be \"$class $cmd [lrange [info args $fcmd] 1 end]\""
		}
		return -code error -errorinfo ${::object::errorInfo} $result
	} else {
		return $result
	}
}

proc object::method {class name args body} {
	upvar ::object::methods::$class methods
	if {"$name" == "subclass"} {
		return -code error "subclass method cannot be redefined"
	} elseif {"$name" == "new"} {
		if {"$class" == "Class"} {
			return -code error "new method of base Class cannot be redefined"
		}
		set args [concat parent object $args]
	} else {
		set args [concat object $args]
	}

	set procname [list ::object::${class}__$name] 
	set methods($name) $procname
	proc $procname $args $body
}

proc object::subclass {parent class} {
	upvar ::object::methods::${parent} parentmethods
	upvar ::object::methods::${class} classmethods
	namespace eval ::object::$class {}
	set ::object::parent($class) $parent
	set ::object::children__${parent}($class) 1
	array set classmethods [array get parentmethods]
	uplevel 2 [list proc $class {args} "::object::classcmd [list $class] \$args"]
}

proc object::new {class object} {
	namespace eval ::object::$object {}
	set ::object::parent($object) $class
	set ::object::children__${class}($object) 1
	uplevel 2 [list proc $object {args} "::object::objectcmd [list $class] [list $object] \$args"]
}

Class method parent {} {
	return [set ::object::parent($object)]
}

Class method children {} {
	return [array names ::object::children__$object]
}

proc ::object::private {object args} {
	foreach var $args {
		uplevel upvar #0 [list ::object::${object}::$var] [list $var]
	}
}

proc ::object::privatevar {object var} {
	return object::${object}::$var
}

proc ::object::common {object args} {
	foreach var $args {
		set mobj ::object::parent($object)
		while 1 {
			if [info exists ::object::${mobj}::$var)] {
				break
			}
			if {"$mobj" == "object"} {
				return -code error "common variable not found"
			}
			set mobj [set ::object::parent($mobj)]
		}
		uplevel upvar #0 [list ::object::${mobj}::$var] [list $var]
	}
}

proc ::object::commonvar {object var} {
	set mobj ::object::parent($object)
	while 1 {
		if [info exists ::object::${mobj}::$var)] {
			break
		}
		if {"$mobj" == "object"} {
			return -code error "common variable not found"
		}
		set mobj [set ::object::parent($mobj)]
	}
	return ::object::${mobj}::$var
}

namespace export private privatevar
namespace export common commonvar
namespace import ::object::private ::object::privatevar 
namespace import ::object::common ::object::commonvar


