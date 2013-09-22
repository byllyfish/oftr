==================
Build Instructions
==================

Before you can build the software, you need `cmake`:

	sudo apt-get install cmake

To obtain the software:

   git clone --recursive git://...
   cd ofp

If you cloned the repository without the --recursive option, you must initialize
the submodule manually. 

   git submodule update --init

Create a directory named "Build" and enter it.

   mkdir Build
   cd Build

First you are going to use cmake to create the Makefiles for your system:

   cmake ..

Then, you are going to build the software.

   make

Finally, you can run the test suite:

   make test

======================
Developer Instructions
======================

To build a debug version of the software, create a new directory named
"Build+Debug" instead, then run cmake and make:

    mkdir Build+Debug
    cd Build+Debug
    cmake ..
    make

The "+debug" in the directory tells CMake to prepare a debug build instead of
a release build.

============================
Cross-Compiling Instructions
============================

The CMakeLists.txt file supports building 32 or 64 bit builds. To set the options,
create a Build directory with your options in the directory name. The following
options are supported:

    +debug    Build a debug version.
    +32bit    Build a 32-bit version.
    +64bit    Build a 64-bit version.

For example, here is the build directory name for a 32-bit debug build:

    Build+Debug+32bit

The build options are case-insensitive. The initial part of the directory name 
can be anything -- it doesn't have to be 'Build'.
