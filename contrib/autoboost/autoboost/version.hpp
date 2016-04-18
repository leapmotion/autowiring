//  Boost version.hpp configuration header file  ------------------------------//

//  (C) Copyright John maddock 1999. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for documentation

#ifndef AUTOBOOST_VERSION_HPP
#define AUTOBOOST_VERSION_HPP

//
//  Caution, this is the only autoboost header that is guarenteed
//  to change with every autoboost release, including this header
//  will cause a recompile every time a new autoboost version is
//  released.
//
//  AUTOBOOST_VERSION % 100 is the patch level
//  AUTOBOOST_VERSION / 100 % 1000 is the minor version
//  AUTOBOOST_VERSION / 100000 is the major version

#define AUTOBOOST_VERSION 105700

//
//  AUTOBOOST_LIB_VERSION must be defined to be the same as AUTOBOOST_VERSION
//  but as a *string* in the form "x_y[_z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define AUTOBOOST_LIB_VERSION "1_57"

#endif
