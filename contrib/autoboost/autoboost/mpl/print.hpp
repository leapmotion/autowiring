
#ifndef AUTOBOOST_MPL_PRINT_HPP_INCLUDED
#define AUTOBOOST_MPL_PRINT_HPP_INCLUDED

// Copyright David Abrahams 2003
// Copyright Aleksey Gurtovoy 2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/identity.hpp>

namespace autoboost { namespace mpl {

namespace aux {
#if defined(AUTOBOOST_MSVC)
# pragma warning(push, 3)
// we only want one warning from MSVC, so turn off the other one
# pragma warning(disable: 4307)
#elif defined(__MWERKS__)
# pragma warn_hidevirtual on
   struct print_base { virtual void f() {} };
#endif

#if defined(__EDG_VERSION__)
  template <class T>
  struct dependent_unsigned
  {
      static const unsigned value = 1;
  };
#endif
} // namespace aux

template <class T>
struct print
    : mpl::identity<T>
#if defined(__MWERKS__)
    , aux::print_base
#endif
{
#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wc++11-extensions"
    const int m_x = 1 / (sizeof(T) - sizeof(T));
# pragma clang diagnostic pop
#elif defined(AUTOBOOST_MSVC)
    enum { n = sizeof(T) + -1 };
#elif defined(__MWERKS__)
    void f(int);
#else
    enum {
        n =
# if defined(__EDG_VERSION__)
           aux::dependent_unsigned<T>::value > -1
# else
           sizeof(T) > -1
# endif
        };
#endif
};

#if defined(AUTOBOOST_MSVC)
# pragma warning(pop)
#elif defined(__MWERKS__)
# pragma warn_hidevirtual reset
#endif

}}

#endif // AUTOBOOST_MPL_PRINT_HPP_INCLUDED
