package require Extral

catch {namespace delete object}

proc putsvars {args} {
	uplevel [list foreach var $args {
		puts "$var:[set $var]"
	}]
	puts ""
}

# Create main namespaces
namespace eval object {
	namespace eval methods {
	}
	namespace eval Class {
	}
	set parent(Class) ""
}

# Create the base Class access command and cmd reference
set ::object::cmds(Class) [namespace current]::Class
proc Class {args} {object::classcmd Class $args}

# Base Class new method: the rest is implemented in the classcmd
set ::object::new(Class) ::object::new
proc object::new {class object} {
	return $object
}

# Base Class and objects destroy method: the rest is implemented in the classcmd
set ::object::methods::Class(destroy) ::object::Class__destroy
proc object::Class__destroy {class object} {
	# destroy object
	set parent [set ::object::parent($object)]
	unset ::object::parent($object)
	if {"$parent" != ""} {
		unset ::object::children__[set parent]($object)
	}
	rename [set ::object::cmds($object)] {}
	unset ::object::cmds($object)
	namespace delete $object
	return {}
}

proc ::object::classcmd {class arg} {
	if {[llength $arg] < 1} {
		return -code error "wrong # args: should be \"$class option ...\""
	}
	set cmd [lpop arg 0]
	switch $cmd {
		destroy {
			if {[llength $arg] != 0} {
				return -code error "wrong # args: should be \"$class destroy\""
			}
			foreach child [array names ::object::children__$class] {
				$child destroy
			}
			if [info exists ::object::methods::${class}] {
				unset ::object::methods::${class}
			}
			if [info exists ::object::new($class)] {
				unset ::object::new($class)
			}
			if [info exists ::object::destroy($class)] {
				unset ::object::destroy($class)
			}
			::object::Class__destroy $class $class
			foreach cmd [info commands ${class}__*] {
				rename $cmd {}
			}
			return {}
		}
		subclass {
			if {[llength $arg] != 1} {
				return -code error "wrong # args: should be \"$class sublcass class\""
			}
			set child [lindex $arg 0]
			if [info exists ::object::cmds($child)] {
				return -code error "object \"$child\" exists"
			}
			set cmdname [uplevel 2 namespace current]::$child
			if {"[info commands $cmdname]" != ""} {
				return -code error "command \"$child\" exists"
			}

			namespace eval ::object::$child {}
			set ::object::parent($child) $class
			set ::object::children__${class}($child) 2
			array set ::object::methods::${child} [array get ::object::methods::${class}]

			# create "destroy" method
			upvar ::object::methods::$child methods
			set body "\[set ::object::methods::[set class](destroy)\] $child \$object"
			set procname [list ::object::${child}__destroy] 
			set methods(destroy) $procname
			proc $procname {parent object} $body

			# create "new" method
			set body "set parent [list $class]\n"
			append body {return [list [$parent init $class $object] 1]}
			set args [concat class object args]
			set procname [list ::object::${child}__new] 
			proc $procname $args $body
			set ::object::new($child) $procname

			# create cmd
			set body "::object::classcmd [list $child] \$args"
			proc $cmdname {args} $body
			set ::object::cmds($child) $cmdname
			return $child
		}
		method {
			if {[llength $arg] != 3} {
				return -code error "wrong # args: should be \"$class method name args body\""
			}
			set name [lindex $arg 0]
			if [regexp {^subclass$|^parent$|^method$|^methods$|^class$} $name] {
				return -code error "$name cannot be redefined"
			}
			set args [lindex $arg 1]
			set body [lindex $arg 2]
			switch $name {
				new {
					if {"$class" == "Class"} {
						return -code error "new method of base Class cannot be redefined"
					}
					set body "set parent [list [set ::object::parent($class)]]\n$body"
					set args [concat class object $args]
					set procname [list ::object::${class}__new] 
					proc $procname $args $body
					set ::object::new($class) $procname
					return {}
				}
				destroy {
					upvar ::object::methods::$class methods
					if {"$class" == "Class"} {
						return -code error "destroy method of base Class cannot be redefined"
					}
					if {"$args" != ""} {
						return -code error "destroy method cannot have arguments"
					}
					set parent [set ::object::parent($class)]
					set line "${parent}__destroy [list $class] \$object"
					regsub -all return $body "$line;return" body
					append body "\n$line"
					set procname [list ::object::${class}__destroy] 
					set methods(destroy) $procname
					proc $procname {parent object} $body
				}
				default {
					upvar ::object::methods::$class methods
					set args [concat class object $args]
					set procname [list ::object::${class}__$name] 
					set methods($name) $procname
					proc $procname $args $body
				}
			}
			return $name
		}
		children {
			if {[llength $arg] != 0} {
				return -code error "wrong # args: should be \"$class children\""
			}
			set object [lindex $arg 0]
			return [lsort [array names ::object::children__$class]]
		}
		methods {
			if {[llength $arg] != 0} {
				return -code error "wrong # args: should be \"$class methods\""
			}
			return [lsort [array names ::object::methods::$class]]
		}
		init {
			if {[llength $arg] < 2} {
				return -code error "wrong # args: should be \"$class init class object args\""
			}
			return [leval [set ::object::new($class)] $arg]
		}
		default {
			if ![info exists ::object::methods::${class}($cmd)] {
				if {"$cmd" != "new"} {
					set arg [concat [list $cmd] $arg]
				}
				set object [lindex $arg 0]
				if [info exists ::object::cmds($object)] {
					return -code error "object \"$object\" exists"
				}
				set cmdname [uplevel 2 namespace current]::$object
				if {"[info commands $cmdname]" != ""} {
					return -code error "command \"$object\" exists"
				}
				namespace eval ::object::$object {}
				set ::object::parent($object) $class
				set ::object::children__${class}($object) 1

				set body "::object::objectcmd [list $class] [list $object] \$args"
				proc $cmdname {args} $body
				set ::object::cmds($object) $cmdname

				set cmd [set ::object::new($class)]
				return [leval $cmd [list $class] $arg]
			} elseif [catch {leval [set ::object::methods::${class}($cmd)] [list $class] [list $class] $arg} result] {
				global errorInfo
				set ::object::error $result
				set ::object::errorInfo $errorInfo
				if [regexp "called \"(.*)\" with too many arguments" $result temp fcmd] {
					set result "wrong # args: should be \"$class $cmd [lrange [info args $fcmd] 2 end]\""
				} elseif [regexp "no value given for parameter \".*\" to \"(.*)\"" $result temp fcmd] {
					set result "wrong # args: should be \"$class $cmd [lrange [info args $fcmd] 2 end]\""
				}
				return -code error -errorinfo ${::object::errorInfo} $result
			} else {
				return $result
			}
		}
	}
}

proc ::object::objectcmd {class object arg} {
	set cmd [lpop arg 0]
	if [catch {leval [set ::object::methods::${class}($cmd)] [list $class] [list $object] $arg} result] {
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

set ::object::methods::Class(class) object::Class__class
proc ::object::Class__class {class object} {
	return $class
}

set ::object::methods::Class(parent) object::Class__parent
proc ::object::Class__parent {class object} {
	return [set ::object::parent($object)]
}

# =========================================
# Variables
# =========================================

proc ::object::private {object args} {
	foreach var $args {
		uplevel upvar #0 [list ::object::${object}::$var] [list $var]
	}
}

proc ::object::privatevar {object var} {
	return object::${object}::$var
}

proc ::object::common {class args} {
	foreach var $args {
		uplevel upvar #0 [list ::object::${class}::$var] [list $var]
	}
}

proc ::object::commonvar {class var} {
	return object::${class}::$var
}

namespace export private privatevar
namespace export common commonvar
namespace import ::object::private ::object::privatevar 
namespace import ::object::common ::object::commonvar

