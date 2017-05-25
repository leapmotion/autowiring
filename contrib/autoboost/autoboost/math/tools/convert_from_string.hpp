//  Copyright John Maddock 2016.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_TOOLS_CONVERT_FROM_STRING_INCLUDED
#define AUTOBOOST_MATH_TOOLS_CONVERT_FROM_STRING_INCLUDED

#ifdef _MSC_VER
#pragma once
#endif

#include <autoboost/type_traits/is_constructible.hpp>
#include <autoboost/type_traits/conditional.hpp>
#include <autoboost/lexical_cast.hpp>

namespace autoboost{ namespace math{ namespace tools{

   template <class T>
   struct convert_from_string_result
   {
      typedef typename autoboost::conditional<autoboost::is_constructible<T, const char*>::value, const char*, T>::type type;
   };

   template <class Real>
   Real convert_from_string(const char* p, const mpl::false_&)
   {
#ifdef AUTOBOOST_MATH_NO_LEXICAL_CAST
      // This function should not compile, we don't have the necesary functionality to support it:
      AUTOBOOST_STATIC_ASSERT(sizeof(Real) == 0);
#else
      return autoboost::lexical_cast<Real>(p);
#endif
   }
   template <class Real>
   AUTOBOOST_CONSTEXPR const char* convert_from_string(const char* p, const mpl::true_&) AUTOBOOST_NOEXCEPT
   {
      return p;
   }
   template <class Real>
   AUTOBOOST_CONSTEXPR typename convert_from_string_result<Real>::type convert_from_string(const char* p) AUTOBOOST_NOEXCEPT_IF((autoboost::is_constructible<Real, const char*>::value))
   {
      return convert_from_string<Real>(p, autoboost::is_constructible<Real, const char*>());
   }

} // namespace tools
} // namespace math
} // namespace autoboost

#endif // AUTOBOOST_MATH_TOOLS_CONVERT_FROM_STRING_INCLUDED

