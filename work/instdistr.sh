#!/bin/sh

/home/peter/dev/Extral/build/version.tcl

# $Format: "export version=$ProjectMajorVersion$.$ProjectMinorVersion$.$ProjectPatchLevel$"$
export version=2.1.0

# sources
cd /home/peter/dev/Extral
rs ~/dev/Extral ~/net/prog-extral/
rm -rf ~/net/prog-extral/Extral/work
cd ~/net/prog-extral
tar cvzf Extral-$version.src.tar.gz Extral

# linux
cd /home/peter/build/tca/Linux-i686/exts/
tar cvzf ~/net/prog-extral/Extral-$version-Linux-i686.tar.gz Extral$version

cd /home/peter/build/tca/Linux-x86_64/exts/
tar cvzf ~/net/prog-extral/Extral-$version-Linux-x86_64.tar.gz Extral$version

# windows
cd /home/peter/build/tca/Windows-intel/exts/
zip -r ~/net/prog-extral/Extral-$version-Windows-intel.zip Extral$version

# docs to net
rs /home/peter/dev/Extral/docs/html/* ~/net/www-extral/htdocs/doc/

cd /home/peter/dev/Extral

echo "
# to upload
ssh -t derijkp,extral@shell.sourceforge.net create
rsync -v -e ssh ~/net/prog-extral/Extral-$version-* derijkp,extral@frs.sourceforge.net:/home/frs/project/e/ex/extral
"
