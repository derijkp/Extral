# Version number
VERSION=0.92

# The directory containing the Tcl sources and headers
# To build against Tcl7.4
# TCL_DIR =	../tcl7.4
# TCL_DIR2 =	../tcl7.4
# To build against Tcl7.5
TCL_DIR =	../tcl7.5a2/generic
TCL_DIR2 =	../tcl7.5a2/unix

# compiler flags: -KPICT is to create position independed code, this might be
# different for your compiler
# Linux
CFLAGS = -O2 -fpic -DHAVE_UNISTD_H=1 -DSTDC_HEADERS=1 -Dvfork=fork
# IRIX 5.3
# CFLAGS = -O2 -KPIC 


# Some versions of make, like SGI's, use the following variable to
# determine which shell to use for executing commands:
SHELL =		/bin/sh

CC =		cc
CC_SWITCHES =	${CFLAGS} -I./ -I${TCL_DIR} -I${TCL_DIR2}

all: libextral$(VERSION).so

libextral$(VERSION).so: extral.o extralInit.o
	$(LD) -o libextral$(VERSION).so -shared extral.o extralInit.o

install: libextral$(VERSION).so
	cp  libextral$(VERSION).so /home/peter/tcl/wex/lib
	cp  extral.tcl /home/peter/tcl/wex/lib/extral0.9

depend:
	makedepend -- $(CC_SWITCHES) -- $(SRCS)

.c.o:
	$(CC) -c $(CC_SWITCHES) $<

# DO NOT DELETE THIS LINE -- make depend depends on it.






