#!/bin/sh

# $Format: "export version=$ProjectMajorVersion$.$ProjectMinorVersion$.$ProjectPatchLevel$"$
export version=2.1.0

# quick install (lib only)

# quick install linux
cd /home/peter/dev/Extral/Linux-i686
../build/version.tcl
rm -rf /home/peter/build/tca/Linux-i686/exts/Extral$version
/home/peter/dev/Extral/build/install.tcl /home/peter/build/tca/Linux-i686/exts

rm -rf /home/peter/build/tca/Windows-intel/exts/Extral$version/lib
cp -r /home/peter/build/tca/Linux-i686/exts/Extral$version/lib /home/peter/build/tca/Windows-intel/exts/Extral2.1.0/
