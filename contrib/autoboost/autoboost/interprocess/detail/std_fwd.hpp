//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2014-2015. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_INTERPROCESS_DETAIL_STD_FWD_HPP
#define AUTOBOOST_INTERPROCESS_DETAIL_STD_FWD_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

//////////////////////////////////////////////////////////////////////////////
//                        Standard predeclarations
//////////////////////////////////////////////////////////////////////////////

#include <autoboost/move/detail/std_ns_begin.hpp>
AUTOBOOST_MOVE_STD_NS_BEG

struct input_iterator_tag;
struct forward_iterator_tag;
struct bidirectional_iterator_tag;
struct random_access_iterator_tag;

template<class T>
struct char_traits;

#if defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC >= 1800) &&defined(AUTOBOOST_DINKUMWARE_STDLIB)
#define AUTOBOOST_INTERPROCESS_STD_FWD_MSVC_IOS_BUG
// Compiler bug workaround. Previous versions (<= VC11)
// used dummy virtual functions
#  pragma vtordisp(push, 2)
#endif

template<class CharT, class Traits>
class basic_ostream;

template<class CharT, class Traits>
class basic_istream;

#ifdef AUTOBOOST_INTERPROCESS_STD_FWD_MSVC_IOS_BUG
#  pragma vtordisp(pop)
#  undef AUTOBOOST_INTERPROCESS_STD_FWD_MSVC_IOS_BUG
#endif

AUTOBOOST_MOVE_STD_NS_END
#include <autoboost/move/detail/std_ns_end.hpp>

#endif //#ifndef AUTOBOOST_INTERPROCESS_DETAIL_STD_FWD_HPP
