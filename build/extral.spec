Summary:	extra commands for Tcl
Name:		extral
Version:	1.1.19
Release:	1
Copyright:	BSD
Group:	Development/Languages/Tcl
Source:	Extral-1.1.19.src.tar.gz
URL: http://rrna.uia.ac.be/extral
Packager: Peter De Rijk <derijkp@uia.ua.ac.be>
Requires: tcl >= 8.0.4
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
./configure --prefix=/usr --disable-stubs
make

%install
cd build
make install
rm -rf /usr/doc/extral-$RPM_PACKAGE_VERSION
mkdir /usr/doc/extral-$RPM_PACKAGE_VERSION
ln -s /usr/lib/Extral1.1/docs /usr/doc/extral-$RPM_PACKAGE_VERSION/docs

%files
%doc README
/usr/lib/Extral1.1
/usr/lib/libExtral1.1.so
