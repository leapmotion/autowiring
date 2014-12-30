//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTRUSIVE_DETAIL_WRKRND_HPP
#define AUTOBOOST_INTRUSIVE_DETAIL_WRKRND_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#ifndef AUTOBOOST_CONFIG_HPP
#include <autoboost/config.hpp>
#endif

#if    !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
   #define AUTOBOOST_INTRUSIVE_PERFECT_FORWARDING
#endif

//Macros for documentation purposes. For code, expands to the argument
#define AUTOBOOST_INTRUSIVE_IMPDEF(TYPE) TYPE
#define AUTOBOOST_INTRUSIVE_SEEDOC(TYPE) TYPE

#endif   //#ifndef AUTOBOOST_INTRUSIVE_DETAIL_WRKRND_HPP
