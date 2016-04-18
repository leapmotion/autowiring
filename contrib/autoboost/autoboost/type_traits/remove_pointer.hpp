
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_REMOVE_POINTER_HPP_INCLUDED
#define AUTOBOOST_TT_REMOVE_POINTER_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

#if defined(AUTOBOOST_MSVC)
#include <autoboost/type_traits/remove_cv.hpp>
#include <autoboost/type_traits/is_pointer.hpp>
#endif

// should be the last #include
#include <autoboost/type_traits/detail/type_trait_def.hpp>

namespace autoboost {

#ifdef AUTOBOOST_MSVC

namespace detail{

   //
   // We need all this crazy indirection because a type such as:
   //
   // T (*const)(U)
   //
   // Does not bind to a <T*> or <T*const> partial specialization with VC10 and earlier
   //
   template <class T>
   struct remove_pointer_imp
   {
      typedef T type;
   };

   template <class T>
   struct remove_pointer_imp<T*>
   {
      typedef T type;
   };

   template <class T, bool b>
   struct remove_pointer_imp3
   {
      typedef typename remove_pointer_imp<typename autoboost::remove_cv<T>::type>::type type;
   };

   template <class T>
   struct remove_pointer_imp3<T, false>
   {
      typedef T type;
   };

   template <class T>
   struct remove_pointer_imp2
   {
      typedef typename remove_pointer_imp3<T, ::autoboost::is_pointer<T>::value>::type type;
   };
}

AUTOBOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_pointer,T,typename autoboost::detail::remove_pointer_imp2<T>::type)

#else

AUTOBOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_pointer,T,T)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_pointer,T*,T)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_pointer,T* const,T)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_pointer,T* volatile,T)
AUTOBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_pointer,T* const volatile,T)

#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/type_trait_undef.hpp>

#endif // AUTOBOOST_TT_REMOVE_POINTER_HPP_INCLUDED
