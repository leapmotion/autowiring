# Purpose

Autowiring is an [inversion-of-control](http://en.wikipedia.org/wiki/Inversion_of_control) framework for C++11. It provides a declarative way to manage resources through
[dependency injection](http://en.wikipedia.org/wiki/Dependency_injection). Instead of explicitly instantiating dependencies, simply declare what you need and Autowiring
will manage object creation and wiring.

# Build [![Build Status](https://travis-ci.org/leapmotion/autowiring.png)](https://travis-ci.org/leapmotion/autowiring)

Autowiring project structure is specified with [CMake](http://www.cmake.org/). Simply point CMake to the root project directory and generate your desired project file.
The `C++11/` directory provides [boost](http://www.boost.org/) shims for platforms that have incomplete C++11 support.

### Mac

Mac dependencies are installed with [port](http://guide.macports.org/) or [brew](http://brew.sh/).  If you have port installed, this will build the project:

    git clone git@github.com:leapmotion/autowiring.git
    cd autowiring
    sudo port install cmake
    cmake .
    make
    make test
    sudo make install

This will configure the project to build fat binaries by default.  If you wish to build only 64-bit binaries, use the following CMake command instead:

    cmake . -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64

### Linux

The commands for Unix are different depending on what package manager you're using.  Ubuntu's package manager is apt-get, OpenSUSE uses zypper, and
CentOS systems use yum.  The major apparent difference to the user will be that the package to install has a different name.  For Ubuntu, do this:

    git clone git@github.com:leapmotion/autowiring.git
    cd autowiring
    sudo apt-get install cmake
    cmake .
    make
    make test
    sudo make install

If you want to build for 32-bit Linux, run the following CMake command instead:

    cmake . -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32

### Windows

Unfortunately, Windows doesn't have any sort of nifty package manager, and this requires that you download and install cmake yourself.  Once that's done,
Here's the magic incantation required, assuming you're running from a MinGW terminal (comes for free with [Git Extensions](https://code.google.com/p/gitextensions/).
You're also going to need a copy of Visual Studio 2012 or better.  Other build systems will probably work, but they aren't officially supported here.

    git clone git@github.com:leapmotion/autowiring.git
    cd autowiring
    cmake .

At this point, you'll have a solution file in your root directory called "Autowiring.sln", if you run Visual Studio as an administrator and build the INSTALL
target then Autowiring will be installed on your system.  As with the other platforms, CMake will be able to find autowiring when it's installed this way
via the [find_package](http://www.cmake.org/cmake/help/v3.0/command/find_package.html) command.  Alternatively, if you prefer to build from the command line:

    cmake --build . --config Release

### Arm-linux

Building on ARM-Linux requires the use of a toolchain file.  This file is included with Autowiring.  To configure, use the following invocation:

    cmake . -DCMAKE_TOOLCHAIN_FILE=standard/toolchain-arm32.cmake -DCMAKE_PREFIX_PATH:PATH=/your/lib/path

For 64-bit ARMv8 (AArch64), use toolchain-arm64.cmake

### Android

Similar requirements to Arm-linux, you must specify a toolchain file.  You must also specify the path to your Android toolchain directory.  Make sure you update `/opt/android-standalone-toolchain` to point to your actual Android standalone toolchain directory. To build for 64 bit android systems, export the environment variable `export ARMv8=true`. If you aren't cross-compiling, then simply run cmake with no options.

    cmake . -DCMAKE_TOOLCHAIN_FILE=standard/toolchain-android.cmake -DLLVM_ANDROID_TOOLCHAIN_DIR=/opt/android-standalone-toolchain

# Test

To run all Autowiring unit tests as a sanity check:

    bin/AutowiringTest

If running through valgrind, make sure to specify fair thread scheduling or else some pathological test cases may hang:

    valgrind --fair-sched=yes bin/AutowiringTest

# Install

Autowiring uses CPack, which knows how to use rpm/deb/wix/dragndrop, and will target the correct version depending on which system you're on.  To build
the installer on Windows, you must first download the [wix toolset](http://wixtoolset.org/).  Other CMake installations on the current system will be able
to find Autowiring once the package is installed by this means.

Generally speaking, there is not really much reason to build an installer yourself unless you're testing out the bleeding edge.  The
[releases page](https://github.com/leapmotion/autowiring/releases) lists the officially supported Autowiring releases.

[![ZenHub Badge](https://raw.githubusercontent.com/ZenHubIO/support/master/zenhub-badge.png)](https://zenhub.io)
