#ifndef AUTOBOOST_MOVE_DETAIL_PLACEMENT_NEW_HPP
#define AUTOBOOST_MOVE_DETAIL_PLACEMENT_NEW_HPP
///////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2014-2015. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

struct autoboost_move_new_t{};

//avoid including <new>
inline void *operator new(std::size_t, void *p, autoboost_move_new_t)
{  return p;  }

inline void operator delete(void *, void *, autoboost_move_new_t)
{}

#endif   //AUTOBOOST_MOVE_DETAIL_PLACEMENT_NEW_HPP
