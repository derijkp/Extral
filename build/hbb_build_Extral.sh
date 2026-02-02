#!/bin/bash

# This script builds Extral using the Holy build box environment and a dirtcl
# options:
# -b|-bits|--bits: 32 for 32 bits build (default 64)
# -d|-builddir|--builddir: top directory to build in (default ~/build/bin-$arch)
# -t|-tclversion|--tclversion: tcl version (default 8.6.11); the dirtcl build used is ~/build/bin-$arch/dirtcl$tclversion-$arch

# The Holy build box environment requires docker, make sure it is installed
# e.g. on ubuntu and derivatives
# sudo apt install docker.io
# Also make sure you have permission to use docker
# sudo usermod -a -G docker $USER

# stop on error
set -e

# Prepare and start docker with Holy Build box
# ============================================

script="$(readlink -f "$0")"
dir="$(dirname "$script")"

source "${dir}/start_hbb.sh"

# Vars
# ====

name=Extral
version=2.1.0
tclversion=8.6.14

# Parse extra arguments
# =====================

while [[ "$#" -gt 0 ]]; do case $1 in
	-t|-tclversion|--tclversion) tclversion="$2"; shift;;
	*) echo "Unknown parameter: $1"; exit 1;;
esac; shift; done

# Script run within Holy Build box
# ================================

# do not activate Holy Build Box environment.
# Tk does not compile with these settings (X)
# only use HBB for glibc compat, not static libs
# source /hbb_shlib/activate

# print all executed commands to the terminal
set -x

# set up environment
# ------------------

if [ $arch = "linux-x86_64" ] || [ $arch = "windows-x86_64" ]; then
	yuminstall devtoolset-9
	## use source instead of scl enable so it can run in a script
	## scl enable devtoolset-9 bash
	source /opt/rh/devtoolset-9/enable
fi

if [ -d /build/dirtcl$tclversion-$arch ]; then
	echo "Using tcldir /build/dirtcl$tclversion-$arch"
	prefixopt="--prefix=/build/dirtcl$tclversion-$arch"
	tcl=/build/dirtcl$tclversion-$arch/tclsh
else
	yuminstall tcl-devel
	prefixopt=""
	tcl=`which tclsh`
fi

configureopts=""
if [[ $arch =~ "linux" ]]; then
	os="Linux"
else
	os="Windows"
	if [ -d /build/dirtcl$tclversion-$arch ]; then
		echo "$builddir/dirtcl$tclversion-$arch found"
	else
		echo "coud not find dirtcl dirtcl$tclversion-$arch" > stderr
		exit 1
	fi
	configureopts="--host=$HOST --build=i686-unknown-linux-gnu"
	tclfile=`ls /build/dirtcl$tclversion-$arch/tclsh*.exe`
	tcl="wine $tclfile"
fi

# Build
# -----
cd /io

$tcl build/version.tcl || true

mkdir $arch || true
cd $arch
make distclean || true
../configure $prefixopt $configureopts
# makedoc uses tclsh, which wont work for crosswin build, so make docs separately
make binaries libraries
$tcl /io/build/makedoc.tcl /io /io


if [ -d /build/dirtcl$tclversion-$arch ]; then
	cd /io
	dirtcl=/build/dirtcl$tclversion-$arch
	rm -rf $dirtcl/exts/${name}$version
	$tcl build/install.tcl $dirtcl/exts
	echo "Installed binary $name $builddir/dirtcl$tclversion-$arch/exts"
fi

echo "Finished building $name so in $srcdir/$arch"
