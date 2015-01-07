//////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/thread for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_THREAD_DETAIL_IS_CONVERTIBLE_HPP
#define AUTOBOOST_THREAD_DETAIL_IS_CONVERTIBLE_HPP

#include <autoboost/type_traits/is_convertible.hpp>
#include <autoboost/thread/detail/move.hpp>

namespace autoboost
{
  namespace thread_detail
  {
    template <typename T1, typename T2>
    struct is_convertible : autoboost::is_convertible<T1,T2> {};

#if defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#if defined(AUTOBOOST_INTEL_CXX_VERSION) && (AUTOBOOST_INTEL_CXX_VERSION <= 1300)

#if defined AUTOBOOST_THREAD_USES_MOVE
    template <typename T1, typename T2>
    struct is_convertible<
      rv<T1> &,
      rv<rv<T2> > &
    > : false_type {};
#endif

#elif defined __GNUC__ && (__GNUC__ < 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ <= 4 ))

    template <typename T1, typename T2>
    struct is_convertible<T1&, T2&> : autoboost::is_convertible<T1, T2> {};
#endif

#endif
  }

} // namespace autoboost


#endif //  AUTOBOOST_THREAD_DETAIL_MEMORY_HPP
