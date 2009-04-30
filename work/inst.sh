#!/bin/sh

# $Format: "export version=$ProjectMajorVersion$.$ProjectMinorVersion$.$ProjectPatchLevel$"$
export version=2.1.0

# full compile and install linux
cd /home/peter/dev/Extral/Linux-i686
../build/version.tcl
make distclean
../configure --prefix=/home/peter/tcl/dirtcl
make
rm -rf /home/peter/build/tca/Linux-i686/exts/Extral$version
/home/peter/dev/Extral/build/install.tcl /home/peter/build/tca/Linux-i686/exts

# full cross-compile and install windows
cd /home/peter/dev/Extral/windows-intel
make distclean
cross-bconfigure.sh --prefix=/home/peter/tcl/win-dirtcl
cross-make.sh
rm -rf /home/peter/build/tca/Windows-intel/exts/Extral$version
wine z:/home/peter/build/tca/Windows-intel/tclsh84.exe z:/home/peter/dev/Extral/build/install.tcl z:/home/peter/build/tca/Windows-intel/exts

/home/peter/dev/Extral/build/version.tcl

# install in httpd
rm -rf /home/peter/dev/httpd/exts/Extral$version
/home/peter/dev/Extral/build/install.tcl /home/peter/dev/httpd/exts
