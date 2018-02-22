//  (C) Copyright John Maddock 2015.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPE_TRAITS_INTEGRAL_CONSTANT_HPP
#define AUTOBOOST_TYPE_TRAITS_INTEGRAL_CONSTANT_HPP

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

#if (AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, AUTOBOOST_TESTED_AT(1400)) \
   || AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610)) \
   || AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840)) \
   || AUTOBOOST_WORKAROUND(__MWERKS__, AUTOBOOST_TESTED_AT(0x3202)) \
   || AUTOBOOST_WORKAROUND(AUTOBOOST_INTEL_CXX_VERSION, AUTOBOOST_TESTED_AT(810)) )


namespace autoboost{
   namespace mpl
   {
      template <bool B> struct bool_;
      template <class I, I val> struct integral_c;
      struct integral_c_tag;
   }
}

#else

namespace mpl_{

   template <bool B> struct bool_;
   template <class I, I val> struct integral_c;
   struct integral_c_tag;
}

namespace autoboost
{
   namespace mpl
   {
      using ::mpl_::bool_;
      using ::mpl_::integral_c;
      using ::mpl_::integral_c_tag;
   }
}

#endif

namespace autoboost{

   template <class T, T val>
   struct integral_constant
   {
      typedef mpl::integral_c_tag tag;
      typedef T value_type;
      typedef integral_constant<T, val> type;
      static const T value = val;
      //
      // This helper function is just to disable type-punning
      // warnings from GCC:
      //
      template <class U>
      static U& dereference(U* p) { return *p; }

      operator const mpl::integral_c<T, val>& ()const
      {
         static const char data[sizeof(long)] = { 0 };
         return dereference(reinterpret_cast<const mpl::integral_c<T, val>*>(&data));
      }
      AUTOBOOST_CONSTEXPR operator T()const { return val; }
   };

   template <class T, T val>
   T const integral_constant<T, val>::value;

   template <bool val>
   struct integral_constant<bool, val>
   {
      typedef mpl::integral_c_tag tag;
      typedef bool value_type;
      typedef integral_constant<bool, val> type;
      static const bool value = val;
      //
      // This helper function is just to disable type-punning
      // warnings from GCC:
      //
      template <class T>
      static T& dereference(T* p) { return *p; }

      operator const mpl::bool_<val>& ()const
      {
         static const char data = 0;
         return dereference(reinterpret_cast<const mpl::bool_<val>*>(&data));
      }
      AUTOBOOST_CONSTEXPR operator bool()const { return val; }
   };

   template <bool val>
   bool const integral_constant<bool, val>::value;

   typedef integral_constant<bool, true> true_type;
   typedef integral_constant<bool, false> false_type;

}

#endif
