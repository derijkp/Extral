# Version number
VERSION=0.9

# The directory containing the Tcl sources and headers
TCL_DIR =	../tcl7.4

# compiler flags: -KPICT is to create position independed code, this might be
# different for your compiler
CFLAGS = -O2 -fpic -DHAVE_UNISTD_H=1 -DSTDC_HEADERS=1 -Dvfork=fork

# Some versions of make, like SGI's, use the following variable to
# determine which shell to use for executing commands:
SHELL =		/bin/sh

CC =		cc
CC_SWITCHES =	${CFLAGS} -I./ -I${TCL_DIR}

all: libextral.so.$(VERSION)

libextral.so.$(VERSION): extral.o extralInit.o
	$(LD) -o libextral.so.$(VERSION) -shared extral.o extralInit.o

install: libextral.so.$(VERSION)
	cp  libextral.so.$(VERSION) /home/peter/tcl/wex/lib
	cp  extral.tcl /home/peter/tcl/wex/lib/extral0.9

depend:
	makedepend -- $(CC_SWITCHES) -- $(SRCS)

.c.o:
	$(CC) -c $(CC_SWITCHES) $<

# DO NOT DELETE THIS LINE -- make depend depends on it.
