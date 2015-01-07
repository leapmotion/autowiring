/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2014-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTRUSIVE_DETAIL_TO_RAW_POINTER_HPP
#define AUTOBOOST_INTRUSIVE_DETAIL_TO_RAW_POINTER_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/intrusive/detail/config_begin.hpp>
#include <autoboost/intrusive/detail/pointer_element.hpp>

namespace autoboost {
namespace intrusive {
namespace detail {

template <class T>
inline T* to_raw_pointer(T* p)
{  return p; }

template <class Pointer>
inline typename autoboost::intrusive::pointer_element<Pointer>::type*
to_raw_pointer(const Pointer &p)
{  return autoboost::intrusive::detail::to_raw_pointer(p.operator->());  }

} //namespace detail
} //namespace intrusive
} //namespace autoboost

#include <autoboost/intrusive/detail/config_end.hpp>

#endif //AUTOBOOST_INTRUSIVE_DETAIL_UTILITIES_HPP
