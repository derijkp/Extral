Class subclass Ds

Ds method create {database args} {
	set type [extractoption args -type fdbm]
	file mkdir $database
	file mkdir [file join $database classes]
	set f [open [file join $database info] "w"]
	puts $f [list type $type args $args]
	close $f
	set db(data) [file join $database data]
	eval {dbm create $type $db(data)} $args
}

Ds method addclass {name schema} {
	private $object db
	set db(schema,$name) $schema
	writefile [file join $db(classdir) $name] $schema
	if {"$db(type)" == "fdbm"} {
		file mkdir [file join $db(data) $name]
	}
}

Ds method new {database args} {
	private $object db
	set db(dir) $database
	set db(classdir) [file join $database classes]
	set db(data) [file join $database data]
	set db(info) [file join $database info]
	set db(cmd) ::object::${object}::db
	array set db [readfile $db(info)]

	set db(classes) [Extral::dirglob $db(classdir) *]
	foreach class $db(classes) {
		set db(schema,$class) [readfile [file join $db(classdir) $class]]
	}
	eval {dbm open $db(type) $db(cmd) $db(data)} $db(args)
}

Ds method object {obj} {
	private $object db
	set class [file dir $obj]
	set data [$db(cmd) get $obj]
	set data [structlset -struct db(schema,$class) $data $field $value]
	$db(cmd) set $obj {}
	return $value
}

Ds method set {obj field value} {
	private $object db
	set class [file dir $obj]
	set data [$db(cmd) get $obj {}]
	set data [structlset -struct $db(schema,$class) $data $field $value]
	$db(cmd) set $obj $data
	return $value
}

Ds method get {obj field} {
	private $object db
	set class [file dir $obj]
	set data [$db(cmd) get $obj {}]
	return [structlget -struct $db(schema,$class) $data $field]
}

