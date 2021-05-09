# This script is meant to be sourced in a Holy build box build script to start the HBB docker
# it interprets the options:
# -b|-bits|--bits: 32 for 32 bits build (default 64)
# -d|-builddir|--builddir: top directory to build in (default ~/build/tcl-$arch)
# and presents all options (excluding -bbuilddir) to the program in the docker

# you can use the following to use in the build script
## Prepare and start docker with Holy Build box
## ============================================
#script="$(readlink -f "$0")"
#dir="$(dirname "$script")"
#source "${dir}/hbb_start.sh"

# The Holy build box environment requires docker, make sure it is installed
# e.g. on ubuntu and derivatives
# sudo apt install docker.io
# Also make sure you have permission to use docker
# sudo usermod -a -G docker $USER

# Prepare and start docker with Holy Build box
# ============================================

# check if we are already in holy build box
if [ ! -f /hbb_exe/activate ]; then
	# find directory of script
	script="$(readlink -f "$0")"
	dir="$(dirname "$script")"
	file="$(basename "$script")"
	if [ $(basename "$dir") = "build" ] ; then
		srcdir="$(dirname "$dir")"
		file="build/$file"
	else
		srcdir="$dir"
	fi

	echo "Running script $dir/$file"
	builddir=""
	bits=64
	argumentspos=1; 
	while [[ "$#" -gt 0 ]]; do case $1 in
		-b|-bits|--bits) bits="$2"; shift;;
		-d|-builddir|--builddir) builddir="$(readlink -f "$2")" ; shift;;
		*) arguments[$argumentspos]="$1"; argumentspos+=1 ; arguments[$argumentspos]="$2"; argumentspos+=1 ; shift;;
	esac; shift; done
	if [ "$builddir" = "" ] ; then
		if [ "$bits" = "32" ] ; 	then
			builddir="$HOME/build/bin-ix86"
		else
			builddir="$HOME/build/bin-x86_64"
		fi
	fi
	mkdir -p "$builddir"
	echo "Build $bits bits version"
	echo "builddir=$builddir"
	echo "srcdir=$srcdir"
	# run the script in holy build box
	uid=$(id -u)
	gid=$(id -g $uid)
	
	if [ "$bits" = "32" ] ; 	then
		if docker image list | grep --quiet hbb32; then
			buildbox=hbb32
		else
			buildbox=phusion/holy-build-box-32:2.2.0
		fi
		docker run --net=host -t -i --rm -v "$srcdir:/io" -v "$builddir:/build" "$buildbox" linux32 bash "/io/$file" "stage2" "$file" "$bits" "$uid" "$gid" "$srcdir" "$builddir" ${arguments[*]}
	else
		if docker image list | grep --quiet hbb64; then
			buildbox=hbb64
		else
			buildbox=phusion/holy-build-box-64:2.2.0
		fi
		docker run --net=host -t -i --rm -v "$srcdir:/io" -v "$builddir:/build" "$buildbox" bash "/io/$file" "stage2" "$file" "$bits" "$uid" "$gid" "$srcdir" "$builddir" ${arguments[*]}
	fi
	exit
fi

if [ "$1" = "stage2" ] ; then
	# in stage 2 we will create the user build with sudo access
	# then restart the script (skipping to stage 3)
	file=$2
	bits=$3
	uid=$4
	gid=$5
	srcdir=$6
	builddir=$7
	# prepare the user build with sudo rights
	echo "installing sudo"
	# to stop "checksum is invalid" errors when using yum in 32 bit docker
	if [ "$bits" = 32 ] ; then
		rm /etc/yum.repos.d/phusion_centos-6-scl-i386.repo | true
		yum upgrade --nogpgcheck -y
		if ! rpm --quiet --query yum-plugin-ovl; then
			yum install -q -y yum-plugin-ovl
		fi
	fi
	if ! rpm --quiet --query sudo; then
		yum install -q -y sudo
	fi
	echo "preparing user build with uid=$uid and gid=$gid"
	groupadd build --gid $gid
	useradd build --uid $uid --gid $gid
	# usermod -a -G wheel build
	echo "build ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/90-build
	# default nr of processes (for user build) is sometimes not enough
	sudo sed -i 's/1024/10240/' /etc/security/limits.d/90-nproc.conf
	# (re)start script for stage 3: running the actual code
	sudo -u build bash /io/$file "stage3" ${@:2}
	exit
fi

# stage 3: run the actual script (first do some settings)

function yuminstall {
	echo "yuminstall $1"
	if ! rpm --quiet --query "$1"; then
		sudo yum install -y "$1"
	fi
}

file=$2

if [ $(basename "$file") = "start_hbb.sh" ] ; then
	# install yuminstall in .bashrc so it will be available in the new shell started here
	mkdir -p /home/build
	echo 'function yuminstall {
		echo "yuminstall $1"
		if ! rpm --quiet --query "$1"; then
			sudo yum install -y "$1"
		fi
	}
	if [ "$3" = '32' ] ; then
		ARCH='-ix86'
		arch=ix86
		bits=32
	else
		ARCH=''
		arch=x86_64
		bits=64
	fi
	uid=$4;
	gid=$5;
	srcdir=$6;
	builddir=$7;
	' >> /home/build/.bashrc
	# if run as start_hbb.sh directly, show a shell
	echo "shell sstarted by start_hbb.sh"
	bash
	exit
fi

# if sourced in another script, continue executing this other script
if [ "$3" = '32' ] ; then
	ARCH='-ix86'
	arch=ix86
	bits=32
else
	ARCH=''
	arch=x86_64
	bits=64
fi
uid=$4;
gid=$5;
srcdir=$6;
builddir=$7;
shift 7;

echo "Entering Holy Build Box environment; building using $bits bits, uid=$uid, gid=$gid"
