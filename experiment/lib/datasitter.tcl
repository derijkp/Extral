proc extractoption {list option default} {
	upvar $list l
	set pos [lsearch $l $option]
	if {$pos != -1} {
		lpop l $pos
		return [lpop l $pos]
	} else {
		return $default
	}
}

proc extractbool {list option} {
	upvar $list l
	set pos [lsearch $l $option]
	if {$pos != -1} {
		lpop l $pos
		return 1
	} else {
		return 0
	}
}

object new Ds

Ds method create {database args} {
	::set type [extractoption args -type fdbm]
	file mkdir $database
	file mkdir [file join $database classes]
	::set f [open [file join $database info] "w"]
	puts $f [list type $type args $args]
	close $f
	::set db(data) [file join $database data]
	eval {dbm create $type $db(data)} $args
}

Ds method addclass {class schema} {
	private $object db
	::set db(schema,$class) $schema
	writefile [file join $db(classdir) $class] $schema
}

Ds method new {database args} {
	private $object db
	::set db(dir) $database
	::set db(classdir) [file join $database classes]
	::set db(data) [file join $database data]
	::set db(info) [file join $database info]
	array set db [readfile $db(info)]

	::set db(classes) [Extral::dirglob $db(classdir) *]
	foreach class $db(classes) {
		::set db(schema,$class) [reafile [file join $db(classdir) $class]]
	}
	eval {dbm open $db(type) ${object}::db $db(data)} $db(args)
}

Ds method set {obj field value} {
	private $object db
	::set class [file dir $obj]
	::set data [$object::db get $obj]
	::set data [structlset -struct db(schema,$class) $data $field $value]
	$object::db set $obj $data
	return $value
}

Ds method get {obj field} {
	private $object db
	::set class [file dir $obj]
	::set data [$object::db get $obj]
	return [structlget -struct db(schema,$class) $data $field]
}

