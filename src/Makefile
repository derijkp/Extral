# Generated automatically from Makefile.in by configure.
# This Makefile is used to create the extral loadable module.
#

# The directory containing the Tcl sources and headers appropriate
# for this version of Tk ("srcdir" will be replaced or has already
# been replaced by the configure script):
TCL_GENERIC_DIR =	/home/peter/tcl/tcl8.0/generic
TCL_UNIX_DIR =	/home/peter/tcl/tcl8.0/unix

# The directory containing the Tcl library archive file appropriate
# for this version of Tk:
TCL_BIN_DIR = @TCL_BIN_DIR@

# Libraries to use when linking:
LIBS = -L/home/peter/tcl/tcl8.0/unix -ltcl8.0 -ldl  -lieee -lm

#----------------------------------------------------------------
# The information below is modified by the configure script when
# Makefile is generated from Makefile.in.  You shouldn't normally
# modify any of this stuff by hand.
#----------------------------------------------------------------

CC = gcc
SHLIB_CFLAGS =		-fPIC
SHLIB_LD =		gcc -shared
SHLIB_SUFFIX =		.so
SHLIB_VERSION =		
SRC_DIR =		.
CFLAGS = -g
CC_SWITCHES = $(CFLAGS) $(SHLIB_CFLAGS) -I${TCL_GENERIC_DIR} -I$(TCL_UNIX_DIR) ${SHLIB_CFLAGS} -I../extern/

OBJECTS = extralInit.o extral.o lpop.o nonl.o supersort.o general.o \
			structl.o structltypes.o date.o leval.o dbm.o

all: extral${SHLIB_SUFFIX} extratcl

extral$(SHLIB_SUFFIX): $(OBJECTS)
	$(SHLIB_LD) $(SHLIB_CFLAGS) -o ./extral$(SHLIB_SUFFIX) $(OBJECTS)  ${LIBS}
	cp ./extral$(SHLIB_SUFFIX) ../extral$(SHLIB_SUFFIX)

bsddbm$(SHLIB_SUFFIX): bsddbm.o ../extern/libdb.a
	$(SHLIB_LD) $(SHLIB_CFLAGS) -o ./bsddbm$(SHLIB_SUFFIX) bsddbm.o ../extern/libdb.a  ${LIBS}
	mv ./bsddbm$(SHLIB_SUFFIX) ../dbm/bsddbm$(SHLIB_SUFFIX)

gdbm$(SHLIB_SUFFIX): gdbm.o ../extern/libgdbm.a
	$(SHLIB_LD) $(SHLIB_CFLAGS) -o ./gdbm$(SHLIB_SUFFIX) gdbm.o ../extern/libgdbm.a  ${LIBS}
	mv ./gdbm$(SHLIB_SUFFIX) ../dbm/gdbm$(SHLIB_SUFFIX)

extratcl: tclAppInit.o $(OBJECTS)
	$(CC) $(CC_SWITCHES) -o extratcl tclAppInit.o $(OBJECTS) ${TCL_LIB_SPEC} ${LIBS}

clean:
	rm -f *.o *${SHLIB_SUFFIX} config.cache config.log lib.exp
	rm -f core ../core extratcl
	rm -f ../*.so ../dbm/*.so ../*.dll ./*.so ./*.dll ../win/tmp/*.obj ../win/*.csm

Makefile: Makefile.in
	./config.status

distclean:
	rm -f Makefile config.cache config.log config.status 

depend:
	makedepend -- $(DEPEND_SWITCHES) -- $(SRCS)

.c.o:
	$(CC) -c $(CC_SWITCHES) $<

# DO NOT DELETE THIS LINE -- make depend depends on it.
