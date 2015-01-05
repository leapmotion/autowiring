//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2014-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_MOVE_DETAIL_WORKAROUND_HPP
#define AUTOBOOST_MOVE_DETAIL_WORKAROUND_HPP

#if    !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
   #define AUTOBOOST_MOVE_PERFECT_FORWARDING
#endif

//Macros for documentation purposes. For code, expands to the argument
#define AUTOBOOST_MOVE_IMPDEF(TYPE) TYPE
#define AUTOBOOST_MOVE_SEEDOC(TYPE) TYPE
#define AUTOBOOST_MOVE_DOC0PTR(TYPE) TYPE
#define AUTOBOOST_MOVE_DOC1ST(TYPE1, TYPE2) TYPE2
#define AUTOBOOST_MOVE_I ,
#define AUTOBOOST_MOVE_DOCIGN(T1) T1

#endif   //#ifndef AUTOBOOST_MOVE_DETAIL_WORKAROUND_HPP
