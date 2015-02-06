# Purpose

Autowiring is an [inversion-of-control](http://en.wikipedia.org/wiki/Inversion_of_control) framework for C++11. It provides a declarative way to manage resources through [dependency injection](http://en.wikipedia.org/wiki/Dependency_injection). Instead of explicitly instantiating dependencies, simply declare what you need and Autowiring will manage object creation and wiring.

# Build [![Build Status](https://travis-ci.org/leapmotion/autowiring.png)](https://travis-ci.org/leapmotion/autowiring)

Autowiring project structure is specified with [CMake](http://www.cmake.org/). Simply point CMake to the root project directory and generate your desired project file. While Autowiring is written using C++11 features, it supports linking the non-C++11 STL. The `C++11/` directory provides [boost](http://www.boost.org/) shims for missing C++11 library features.

### CMake Options

Because AutoNet has a Boost dependency, it can sometimes be desirable to influence how Boost should be linked to the project.  You can influence the decision
making process by setting CMake Boost attributes, which are described by the [cmake documentation](http://www.cmake.org/cmake/help/v3.0/module/FindBoost.html).
A common use case is to statically link to a pre-specified installation of boost.  To do this, run CMake as follows:

    cmake . \ 
      -DBOOST_ROOT:PATH=/path/to/boost \ 
      -DBoost_USE_STATIC_LIBS:BOOL=ON \ 
      -DBoost_NO_SYSTEM_PATHS:BOOL=ON

Watch the cases, you will get "unused variable" warnings if you don't match what is written above exactly.

### Mac

Mac dependencies are installed with [port](http://guide.macports.org/) or [brew](http://brew.sh/).  If you have port installed, this will build the project:

    git clone git@github.com:leapmotion/autowiring.git
    cd autowiring
    sudo port install boost cmake
    cmake .
    make
    make test
    sudo make install

This will configure the project to build fat binaries by default.  If you wish to build only 64-bit binaries, use the following CMake command instead:

    cmake . -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64

Note that AutoNet will be built for 64-bit only, unless your Boost installation was built as fat binaries.

### Unix

The commands for Unix are different depending on what package manager you're using.  Ubuntu's package manager is apt-get, OpenSUSE uses zypper, and
CentOS systems use yum.  The major apparent difference to the user will be that the package to install has a different name.  For Ubuntu, do this:

    git clone git@github.com:leapmotion/autowiring.git
    cd autowiring
    sudo apt-get install libboost-dev cmake
    cmake .
    make
    make test
    sudo make install

If you want to build for 32-bit Linux, run the following CMake command instead:

    cmake . -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32

### Windows

Unfortunately, Windows doesn't have any sort of nifty package manager, and this requires that you download and install the boost dependency by hand.  Once
you've followed the instructions for installing boost as indicated [here](http://www.boost.org/doc/libs/1_55_0/doc/html/bbv2/installation.html) you will
need to set BOOST_ROOT during cmake configuration time to point to the path where boost was built.  Here's the magic incantation required, assuming you're
running from a MinGW terminal (comes for free with [Git Extensions](https://code.google.com/p/gitextensions/).  You're also going to need a copy of Visual
Studio 2012 or better.  Other build systems will probably work, but they aren't officially supported here.

    git clone git@github.com:leapmotion/autowiring.git
    cd autowiring
    cmake . -DBOOST_ROOT:Path=<path/to/boost/with/forward/slashes>

At this point, you'll have a solution file in your root directory called "Autowiring.sln", if you run Visual Studio as an adminstrator and build the INSTALL
target then Autowiring will be installed on your system.  As with the other platforms, CMake will be able to find autowiring when it's installed this way
via the [find_package](http://www.cmake.org/cmake/help/v3.0/command/find_package.html) command.

### Arm-linux

Building on Android requires the use of a toolchain file.  You will need to use an alternate prefix path if you are trying to cross-compile, the prefix path should contain your version of the Boost libraries built for Android.  To configure, use the following invocation:

    cmake . -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake -DCMAKE_PREFIX_PATH:PATH=/your/lib/path

### Android

Similar requirements to Arm-linux, you must specify a toolchain file.  You must also specify the path to your Android toolchain directory.  Make sure you update `/opt/android-standalone-toolchain` to point to your actual Android standalone toolchain directory. To build for 64 bit android systems, export the environment variable `export ARMv8=true`. If you aren't cross-compiling, then simply run cmake with no options.

    cmake . -DCMAKE_TOOLCHAIN_FILE=toolchain-android.cmake -DLLVM_ANDROID_TOOLCHAIN_DIR=/opt/android-standalone-toolchain

# Install

Autowiring uses CPack, which knows how to use rpm/deb/wix/dragndrop, and will target the correct version depending on which system you're on.  To build
the installer on Windows, you must first download the [wix toolset](http://wixtoolset.org/).  Other CMake installations on the current system will be able
to find Autowiring once the package is installed by this means.

Generally speaking, there is not really much reason to build an installer yourself unless you're testing out the bleeding edge.  The
[releases page](https://github.com/leapmotion/autowiring/releases) lists the officially supported Autowiring releases.
