# The directory containing the Tcl sources and headers appropriate
# for this version of Tk ("." will be replaced or has already
# been replaced by the configure script):
TCL_DIR =	/usr/people/peter/tcl/tcl7.3

# The directory containing the Tk sources and headers appropriate
# for this version of Tk ("." will be replaced or has already
# been replaced by the configure script):
TK_DIR =	/usr/people/peter/tcl/tk3.6

# The directory containing the Tcl library archive file appropriate
# for this version of Tk:
TCL_DIR =	/usr/people/peter/tcl/tcl7.3

CFLAGS = -O2 -KPICT

# Some versions of make, like SGI's, use the following variable to
# determine which shell to use for executing commands:
SHELL =		/bin/sh

CC =		cc
CC_SWITCHES =	${CFLAGS} -I./ -I${TCL_DIR} -I${TK_DIR}

all: libextraL.so

libextraL.so: extral.o extraLInit.o
	$(LD) -o libextraL.so -shared extral.o extraLInit.o
install:
	cp libextraL.so ../xdcse/lib

depend:
	makedepend -- $(CC_SWITCHES) -- $(SRCS)

.c.o:
	$(CC) -c $(CC_SWITCHES) $<

# DO NOT DELETE THIS LINE -- make depend depends on it.

