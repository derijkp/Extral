# Version number
VERSION=0.94

# The directory containing the Tcl sources and headers
# To build against Tcl7.4
# TCL_DIR =	../tcl7.4
# TCL_DIR2 =	../tcl7.4
# To build against Tcl7.5
TCL_DIR =	/home/peter/tcl/tcl7.5b3/generic
TCL_DIR2 =	/home/peter/tcl/tcl7.5b3/unix
INST_DIR =	/home/peter/bin/Extral$(VERSION)

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

all: libextral.so

libextral.so: extral.o extralInit.o
	$(LD) -o libextral.so -shared extral.o extralInit.o

install: libextral.so
	mkdir $(INST_DIR)
	cp libextral.so $(INST_DIR)
	cp ../inst/extral.tcl $(INST_DIR)
	cp ../inst/pkgIndex.tcl $(INST_DIR)
	cp ../inst/extral.txt $(INST_DIR)
	cp ../inst/readme $(INST_DIR)

depend:
	makedepend -- $(CC_SWITCHES) -- $(SRCS)

.c.o:
	$(CC) -c $(CC_SWITCHES) $<

# DO NOT DELETE THIS LINE -- make depend depends on it.
