ExtraL
======  extra commands for Tcl
        by Peter De Rijk (Universiteit Antwerpen) 

What is ExtraL
--------------

Extral is a generally useful library which extends Tcl with a.o.:
	- extral list manipulation commands
	- array manipulation
	- atexit
	- dbm support: file based, gdbm and bsd-db support
	- tempfile
	- filing commands
	- light eval (Viktor Dukhovni)
	
The name comes from the fact that ExtraL originally only contained
extra list processing commands to overcome some of the performance 
problems in Tcl when processing large lists. Later other commands
were added. With the advent of the Tcl byte compiler, the 
performance is not such a great issue any longer, but I still find 
many of the list manipulation functions very convenient, and some 
of those that were now rewritten using the object system in C are 
still faster. 
Most functions are now also available in Tcl-only form as well. Using 
the C code just gives a speedup.

You can find a short description of the commands in the files in the 
docs subdirectory. For some of the commands, you can find out a lot 
by checking the testing suite in the directory tests.

INCOMPATIBLE CHANGES
--------------------
There were some incompatible changes with previous releases:
lremove syntax is now: lremove list indices

I feel like I misnamed some commands in the previous releases. This 
has been changed now:
lmerge acts like the former "lmanip mangle"
lunmerge acts like the reverse of lmerge
lmanip mangle does the previous "lmanip merge"

Installation
------------
You should be able to obtain the latest version of ExtraL via anonymous ftp
on rrna.uia.ac.be. in the directory /pub/tcl

This package can be build as a loadable object to Tcl8.0. Go to the
src directory and type
./configure --with-tcl=<path of tcl distribution>
Then run make. This should produce the loadable module. The build.tcl and
buildwin.tcl files are a tool to make a nice package in a different
directory.

If you want to make gdbm and bsd-db support, you will have to create a 
libgdbm.a and libdb.a (should position independend code) and put these
in the extern directory together with their header files. (gdbm.h and 
db_185.h) and do a 
make gdbm.so
make bsddbm.so

How to contact me
-----------------
I will do my best to reply as fast as I can to any problems, etc.
However, the development of extraL is not my only task,
which is why my response might not be always as fast as you would
like (although I get a very good average).

Peter De Rijk
University of Antwerp (UIA)
Department of Biochemistry
Universiteitsplein 1
B-2610 Antwerp

tel.: 32-03-820.23.16
fax: 32-03-820.22.48
E-mail: derijkp@uia.ua.ac.be
web: http://rrna.uia.ac.be/~peter/personal/peter.html

Legalities
----------

ExtraL is Copyright Peter De Rijk, University of Antwerp (UIA), 1995 The
following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.

The author hereby grant permission to use, copy, modify, distribute,
and license this software and its documentation for any purpose, provided
that existing copyright notices are retained in all copies and that this
notice is included verbatim in any distributions. No written agreement,
license, or royalty fee is required for any of the authorized uses.
Modifications to this software may be copyrighted by their authors
and need not follow the licensing terms described here, provided that
the new terms are clearly indicated on the first page of each file where
they apply.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
MODIFICATIONS.
