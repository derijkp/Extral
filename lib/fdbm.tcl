proc Extral::dbmtype_fdbm {} {
	return 1
}

proc Extral::dbminittype_fdbm {} {
}

proc Extral::fdbm__create {database arg} {
	if {"$arg" != ""} {
		return  -code error "fdbm create has no options"
	}
	if [file exists $database] {
		return -code error "could not create database \"$database\""
	}
	file mkdir $database
}

proc Extral::fdbm__open {object readonly database arg} {
	if {"$arg" != ""} {
		return  -code error "fdbm open has no options"
	}
	set ::Extral::dbm($object,dir) $database
	if ![file exists $database] {
		return -code error "could not open database \"$database\""
	}
}

proc Extral::fdbm__close {object} {
	unset ::Extral::dbm($object,dir)
}

proc Extral::fdbm__set {object key value} {
	set file [file join [set ::Extral::dbm($object,dir)] $key]
	if [catch {writefile $file $value}] {
		return -code error "could not store key \"$key\""
	}
	return ""
}

proc Extral::fdbm__get {object key} {
	set file [file join [set ::Extral::dbm($object,dir)] $key]
	if ![file readable $file] {
		return -code error "error: key \"$key\" not found"
	}
	return [readfile $file]
}

proc Extral::fdbm__keys {object {pattern *}} {
	return [dirglob [set ::Extral::dbm($object,dir)] $pattern]
}

proc Extral::fdbm__unset {object key} {
	set file [file join [set ::Extral::dbm($object,dir)] $key]
	catch {file delete $file}
	return ""
}

proc Extral::fdbm__sync {object} {
}

proc Extral::fdbm__reorganize {object} {
}

