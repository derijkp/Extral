Struct
------
	This code emulates structures in a way. all data is actually stored in 
	one global array extraL__Struct. You cannot use the generic array command 
	on structure structure members
Commands are:
struct new
	returns an unused pointer to a structure.
	eg.
		set current [struct new]

struct set struct->member value
	sets the value of a member
	eg.
		struct set $current->field Test
		struct set $current->data(a) 1
	
struct value struct->member
	returns the value of a member
	eg.
		struct value $current->field
		struct value $current->data(a)

struct unset struct?->member?
	unsets a member or the entire struct
	eg.
		struct unset $current->field
		struct unset $current

struct var struct?->member?
	gives the actual variable name where the member is stored (global): 
	This can be used in -textvariable options etc.
	eg.
		entry .try -textvariable [struct var $current->field]
		pack .try

struct arrayset struct->member items values
	sets values in an array member
	eg.
		% struct arrayset $current->value {a b c} {1 2 3}
		% struct value $current->value(b)
		2

struct arrayget struct->member
		% struct arrayget $current->value
	b 2 a 1 c 3

struct arraynames struct->member
		% struct arraynames $current->value
	b a c

struct arraysize struct->member
		% struct arraysize $current->value
	3

