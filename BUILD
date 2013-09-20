==================
Build Instructions
==================

Before you can build the software, you need cmake:

	sudo apt-get install cmake

To obtain the software:

   git clone --recursive git://...
   cd ofp

If you cloned the repository without the --recursive option, you must initialize
the submodule manually. 

   git submodule update --init

Create a directory named "Build+Release" and enter it.

   mkdir Build+Release
   cd Build+Release

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
"Build+Debug", then run cmake and make:

    mkdir Build+Debug
    cd Build+Debug
    cmake ..
    make

The "+debug" in the directory tells CMake to prepare a debug build instead of
a release build.

