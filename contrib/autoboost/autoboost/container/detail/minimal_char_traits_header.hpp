/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2014-2015
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_CONTAINER_DETAIL_MINIMAL_CHAR_TRAITS_HEADER_HPP
#define AUTOBOOST_CONTAINER_DETAIL_MINIMAL_CHAR_TRAITS_HEADER_HPP
#
#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif
#
#//Try to avoid including <string>, as it's quite big
#if defined(_MSC_VER) && defined(AUTOBOOST_DINKUMWARE_STDLIB)
   #include <iosfwd>   //Dinkum libraries for MSVC define std::char_traits there
#elif defined(AUTOBOOST_GNU_STDLIB)
   #include <bits/char_traits.h>
#else
   #include <string>  //Fallback
#endif

#endif //AUTOBOOST_CONTAINER_DETAIL_MINIMAL_CHAR_TRAITS_HEADER_HPP
