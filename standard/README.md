Standard Project Layout Auxiliaries
===

This directory contains a set of standard CMake modules and auxiliary configuration files that are intended to both simplify and standardize the strategy by which Leap Motion's own external C++11 libraries are built.

There are a few major pieces which should be used as a part of every project, and they're used in the following order.

## Getting Started

Why is everything in a subdirectory?  So you can merge in the whole repo as a subtree without too much fuss.  Or if you prefer, copy and paste the subdirectory into your source tree directly.  This repo doesn't define any standard, it only implements it.

For a subtree merge, do the following from your repo root:

```
git remote add standard git@github.com:leapmotion/standard.git
git fetch standard
git merge standard/master
```

### [Standard](Standard.cmake)

Provides a sort of global header that can be used by all customers of this library.  Users should generally include this one near the top of their root `CMakeLists.txt`.  Users should include this as follows:

```
include("standard/Standard.cmake")
```

### [SetStandardOutputVariables](SetStandardOutputVariables.cmake)

There are a lot of options that have to be configured in order to get CMake building on all platforms with broadly compatible options for C++11.  This file handles all of that work, but also handles the task of standardizing the build directories and making sure that your compiler of choice can actually handle C++11.

This module defines one function taking no arguments which should be invoked as so:

```CMake
set_standard_output_variables()

# Now your source code
add_subdirectory(src)
```

Do this before including your sources or some of your projects won't have the right flags.

### [InstallHeaders](InstallHeaders.cmake)
This is a utility module provided to make it a little easier to install the headers associated with a target.  The target name is passed, the sources of that target are queried, and only the sources with an extension of `.h` will be installed.

The destination path is given relative to the root, and you are welcome to install headers wherever you wish, but you are strongly recommended to install them to a directory named `include/${CMAKE_PROJECT_NAME}`.  You should also consider specifying a lower-case project name.  If you want to separate components, put them in a directory under `${CMAKE_PROJECT_NAME}`.

Use the `DESTINATION` argument name to override the install path.  Note that, by default, `stdafx.h` doesn't get installed, you can use `NOEXCLUDE_STDAFX` to override this.

```CMake
add_library(MyLibrary)
install_headers(TARGET MyLibrary)
```

### [GenerateVersion](GenerateVersion.cmake)

This module is responsible for ensuring that your CMake config finders are properly configured and generated.  It's actually quite difficult to get this right sometimes--for starters, a system that has multiple versions of the same library installed on it.  If you've ever accidentally linked an x86 binary from an ARM build, you will have experienced this problem.

This module ensures this by doing the following:

0. Figures out the architecture that you're targeting.  Right now we support ARM, x86, and x64
0. Creates a file called `MyLibraryTargets.cmake` which includes all of the projects in your library which were installed with the cmake [`install`](https://cmake.org/cmake/help/v3.5/command/install.html) command.
0. Creates `projectname-config.cmake` which specifies the right include directories for installed and local builds, and brings in `MyLibraryTargets.cmake`
0. Creates `projectname-configVersion` which... 
  * ...uses semantic versioning to ensure that `find_package(mylibrary 2.0.2)` correctly matches version `2.0.5` but not version `1.9.2` 
  * ...validates architecture compatibility by not allowing an ARM build to accidentally link with an x86 library
  * ...handles the case of fat binaries, where both x86 and x64 builds can link to the same spot

This command takes no arguments, and should run after _all_ of your libraries are configured.  Furthermore, there are some constraints on how your libraries should be installed--a sample file should look more or less like this:

```CMake
project(MyPackage VERSION 1.0.0)

add_library(MyLibrary ${MyLibrary_SRCS})
install(
  TARGETS MyLibrary
  EXPORT MyPackageTargets
  DESTINATION lib
  COMPONENT MyLibrary
  CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
)
install_headers(TARGET MyLibrary)
generate_version()  
```

### [CombinedInstaller](CombinedInstaller.cmake)

This project creates a "combined installer".  There's nothing wrong with the way CPack works, but the good old `include(CPack)` command does rely quite a bit on variables that you have to set before including it, making it a bit more prone to error.  On top of that, the `PACKAGE` build target created by `CPack` will only build _one_ configuration on multi-configuration platforms, such as Windows.  This means that you're stuck distributing separate installers for both versions.

The combined installer, on Windows, creates a WIX installer for your application based on the specified README and LICENSE text files, generates a GUID for you based on the major version number, vendor, product name, and architecture.  The result is that you can install multiple versions of your library side-by-side on Windows, and that a minor version update will upgrade your system installation in-place whereas a major version upgrade will leave the prior version intact.

