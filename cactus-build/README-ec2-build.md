# BUILD ON EC2

If this application is built on ubuntu or any other linux system normally used for
development there are many libraries that are not built into the application 
because they are available in the environment therefore the constructed program will
not run in an EC2 environment.

The obvious solution is to build in an ec2 environment that is at least partially
compatible with the lambda environment.

So this must be amazon linux 2.

## construct a virtual machine

Construct an ec2 vm either at amazon or locally.

If using amazon hosted machine t2.2xlarge is adequate, though picking something with
more cores will speed up compilation, though most of what you do is manual this may be
only of minor benefit.

https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/amazon-linux-2-virtual-machine.html

gives a set of instructions which are reasonable comprehensive. I ran a kvm image,
the one they provide is ideal a 25GB disk image containing a ready installed version
of the OS. You will need to follow the instructions though in order to setup a user,
so you can access the account.

So do what you wish to create a working AWS-Linux-2 machine.

# Configuring aws linus 2 for compiling LibreOffice

There are many libraries and application not available or of too great an age to be
used for the compilation of the latest version of LibreOffice.

The following commands Install the necessary applications to compile libreoffice,
these are the commands I ran in the order I ran them and they worked for me.

```
$ suso yum install autoconf -y
$ sudo yum install rpmtools.noarch -y
$ sudo yum groupinstall "Development Tools" -y
$ sudo yum groupinstall buildsys-build -y
$ sudo yum install fontconfig.x86_64 fontconfig-devel.x86_64  -y
```
The next stage requires make version 4, at the time of doing this the newest available
from the amazon repository was 3.8.1 so we need to download and build make 4.X

```
$ wget https://ftp.gnu.org/gnu/make/make-4.3.tar.gz
$ tar -xzf make-4.3.tar.gz
$ cd make-4.3
$ ./configure
$ make
$ sudo make install
$ cd ~
```
A few more downloads:
```
$ sudo yum install cpan -y
$ sudo cpan -i Test
$ sudo cpan -i Test::More
$ sudo cpan -i Digest::MD5
```

We now need to install gperf then upgrade it to 3.1, the initial yum command is
probably superfluous:

```
$ sudo yum install gperf -y

$ wget http://ftp.gnu.org/pub/gnu/gperf/gperf-3.1.tar.gz
$ tar -xzf gperf-3.1.tar.gz 
$ cd gperf-3.1/
$ ./configure
$ make
$ sudo make install
$ cd ~
```

At the time of writing the following downloads java 17 which is the most recent
long term release, though jdk-9 is all that is required and jdk-11 is recommended
jdk-17 seemed to work.
```
$ sudo yum install java -y
$ sudo yum install nasm -y
$ sudo yum install krb5-devel.x86_64 -y
$ sudo yum install nss-devel.x86_64 -y
```

Now install flex.
```
$ wget https://sourceforge.net/projects/flex/files/flex-2.6.0.tar.gz/download
$ mv download flex.tgz
$ cd flex-2.6.0
$ ./configure
$ make
$ sudo make install
$ cd ~
```

The version of gtk3 downloaded with yum was too old, so some effort here was done
to update it, however at the pont that epoxy d been installed and finding gtk3's configure
script  would still insist it was not present I gave up on this and removed gtk3
from the build script.  This would have been done earlier but even with it removed
the libreoffice build still complained about the lackk of some X11 and related
libraries so it was left in, however ulimately it was removed and just the needed
libraries added, so some of the following is redundant, unfortunately I do not
know which bits.

the line
```
--disable-gtk3-kde5
```
was removed from libreoffice/distro-configs/LibreOfficeCactus.conf

```
$ sudo yum install gtk3 -y
$ sudo yum install gobject-introspection-devel.x86_64 -y
$ sudo yum install avahi.x86_64 -y
$ sudo yum install avahi-devel.x86_64 -y
$ sudo yum install ant.noarch -y

$ sudo yum install gtk-doc.x86_64 -y
$ sudo yum install pango-devel.x86_64 -y
$ sudo yum install cairo-gobject-devel.x86_64 -y
$ sudo yum install gdk-pixbuf2-devel.x86_64 -y
$ sudo yum install atk-devel.x86_64 -y
$ sudo yum install xorg-x11* -y
$ sudo yum install libXi-devel.x86_64 -y
$ sudo yum install libepoxy-devel.x86_64 -y
$ sudo yum install cmake -y
$ sudo yum install openssl-devel -y
$ sudo yum install libXt-devel.x86_64 -y
```

The version of cmake ws too old for something so:
```
$ wget https://github.com/Kitware/CMake/releases/download/v3.25.1/cmake-3.25.1.tar.gz
$ tar -xzf cmake-3.25.1.tar.gz
$ cd cmake-3.25.1
$ ./bootstrap
$ make
$ sudo make install
$ cd ~
```
As was libepoxy

```
$ pip3 install meson
$ pip3 install ninja

$ wget https://github.com/anholt/libepoxy/archive/refs/tags/1.5.10.tar.gz
$ tar -xvf 1.5.10.tar.gz
$ cd libeproxy-1.5.10
$ mkdir build
$ cd build
$ meson
$ ninja
```
Most of the other install scripts used required that you run them as a superuser
who can write to system areas i.e sudo make install, however ninja does not work
like this, it appears the installation of ninja above prodced a ninja that could
not be seen by root, so you just do ninja install and it internally does a sudo
and requires your password.
```
$ ninja install     # note this will request a password
$ cd ~
```

Just one more thing the file we use in zipped using brotli which is not available
on the ec2.

So download and install brotli.

```
$ wget https://sourceforge.net/projects/brotli.mirror/files/latest/download
$ mv download brotli.zip
$ unzip brotli.zip
$ cd google-brotli-e61745a/
$ mkdir out
$ cd out
$ ../configure-cmake 
$ make
$ make test
$ sudo make install
$ cd ~
```

Finally we are ready to build libreoffice:
```
$ git clone git@github.com:cactus-bm/libreoffice-core.git
$ cd libreoffice-core
$ ./cactus-make
```
Even if you have a lot of cores this will still take a while, best go and have tea.

With this completed you now have a directory instdir at the root of libreoffice-core,
it can be tested with:

```
$ cd cactus-build
$ ./expand.sh    ## will create and instance for testing with not required libraries removed.
$ ./test.sh html ## will run create html tests, there are several other tests that can be run
```

If this worked there will be a number of html files in the __converted__ subdirectory 
that have been converted correctly.

So final build the zip for deploying in the lambda, this again will take a while,
so assuming you had tea whilst the compilation happened it is either time for dinner 
or supper once you have started this

```
$ ./build.sh
```
Note this uses Brotli compression so also takes an age to complete.

Now we have a file __cactus-soffice_X-X-X-X_YYYYYY.zip__ which can be deployed as an Amazon
Lambda layer, X-X-X-X is the libreoffice release number, YYYYY is the latest cactus build
tag.

