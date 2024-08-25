Summary:	extra commands for Tcl
Name:		extral
Version:	2.0.3
Release:	1
Copyright:	BSD
Group:	Development/Languages/Tcl
Source:	Extral-2.0.3.src.tar.gz
URL: http://extral.sourceforge.net/
Packager: Peter De Rijk <Peter.DeRijk@ua.ac.be>
Requires: tcl >= 8.3.2
Prefix: /usr
%description
 Extral is a generally useful library which extends Tcl with a.o.:
        - extral list manipulation commands
        - extra string manipulation commands
        - array manipulation
        - atexit
        - tempfile
        - filing commands

%prep
%setup -n Extral

%build
cd build
./configure --prefix=/usr
make clean
make

%install
cd build
make install
rm -rf /usr/doc/extral-$RPM_PACKAGE_VERSION
mkdir /usr/doc/extral-$RPM_PACKAGE_VERSION
ln -s /usr/lib/Extral2.0/docs /usr/doc/extral-$RPM_PACKAGE_VERSION/docs

%files
%doc README.md
/usr/lib/Extral2.0
/usr/lib/libExtral2.0.so
