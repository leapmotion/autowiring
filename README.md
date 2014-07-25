# Purpose

Autowiring is an [inversion-of-control](http://en.wikipedia.org/wiki/Inversion_of_control) framework for C++11. It provides a declarative way to manage resources through [dependency injection](http://en.wikipedia.org/wiki/Dependency_injection). Instead of explicitly instantiating dependencies, simply declare what you need and Autowiring will manage object creation and wiring.

# Platform Support

While Autowiring is written using C++11 features, it supports linking the non-C++11 STL. The `C++11/` directory provides [boost](http://www.boost.org/) shims for missing C++11 library features.

Supported Compilers:
* Clang
* GCC
* VS2012+

# How to Build

Autowiring project structure is specified with [CMake](http://www.cmake.org/). Simply point CMake to the root project directory and generate your desired project file.
