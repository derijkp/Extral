package require Extral

catch {namespace delete object}
namespace eval object {
	variable methods

	proc objectcmd {object arg} {
		set cmd [lpop arg 0]
		set mobj ${object}
		while 1 {
			if [info exists ::object::methods(${mobj}::$cmd)] {
				break
			}
			if {"$mobj" == "object"} {
				return -code error "method not found"
			}
			set mobj [set ::object::parent($mobj)]
		}
#		return [leval ${mobj}::$cmd [list $object] $arg]
		if [catch {leval ${mobj}::$cmd [list $object] $arg} result] {
			global errorInfo
			set ::object::error $result
			set ::object::errorInfo $errorInfo
			if [regexp "called \"(.*)\" with too many arguments" $result temp fcmd] {
				set result "wrong # args: should be \"$object $cmd [lrange [info args $fcmd] 1 end]\""
			} elseif [regexp "no value given for parameter \".*\" to \"(.*)\"" $result temp fcmd] {
				if {"$cmd" != "new"} {
					set args [lrange [info args $fcmd] 1 end]
				} else {
					set args [lrange [info args $fcmd] 2 end]
				}
				set result "wrong # args: should be \"$object $cmd $args\""
			}
			return -code error -errorinfo ${::object::errorInfo} $result
		} else {
			return $result
		}
	}

	namespace eval object {
		proc method {object name args body} {
			if {"$name" == "new"} {
				if {"$object" == "object"} {
					return -code error "new method of base object cannot be redefined"
				}
				set parent [set ::object::parent($object)]
				set line {::object::object::new $parent $object}
				set args [concat parent object $args]
				set body "$line\n$body"
			} else {
				set args [concat object $args]
			}

			set ::object::methods(${object}::$name) 1
			proc ::object::${object}::$name $args $body
		}
	
		proc new {parent object} {
			namespace eval ::object::$object {}
			set ::object::parent($object) $parent
			laddnew ::object::children($parent) $object
			uplevel 2 [list proc $object {args} "::object::objectcmd [list $object] \$args"]
		}
	}
	set methods(object::method) {3 3 "name args body"}
	set methods(object::new) {1 1 "child"}
	set parent(object) ""
	return {}
}

proc object {args} {object::objectcmd object $args}

namespace eval object {
	proc parent {object} {
		return [set ::object::parent($object)]
	}
	
	proc children {object} {
		return [set ::object::children($object)]
	}
	
	proc private {object args} {
		foreach var $args {
			uplevel upvar #0 [list ::object::${object}::$var] [list $var]
		}
	}
	
	proc privatevar {object var} {
		return object::${object}::$var
	}
	
	proc common {object args} {
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
	
	proc commonvar {object var} {
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
}	

namespace import ::object::private ::object::privatevar 
namespace import ::object::common ::object::commonvar

