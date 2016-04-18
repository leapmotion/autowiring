
//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_IS_SIGNED_HPP_INCLUDED
#define AUTOBOOST_TT_IS_SIGNED_HPP_INCLUDED

#include <autoboost/type_traits/is_integral.hpp>
#include <autoboost/type_traits/remove_cv.hpp>
#include <autoboost/type_traits/is_enum.hpp>
#include <autoboost/type_traits/detail/ice_or.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

#if !defined( __CODEGEARC__ )

namespace detail{

#if !(defined(__EDG_VERSION__) && __EDG_VERSION__ <= 238) && !defined(AUTOBOOST_NO_INCLASS_MEMBER_INITIALIZATION)

template <class T>
struct is_signed_values
{
   //
   // Note that we cannot use AUTOBOOST_STATIC_CONSTANT here, using enum's
   // rather than "real" static constants simply doesn't work or give
   // the correct answer.
   //
   typedef typename remove_cv<T>::type no_cv_t;
   static const no_cv_t minus_one = (static_cast<no_cv_t>(-1));
   static const no_cv_t zero = (static_cast<no_cv_t>(0));
};

template <class T>
struct is_signed_helper
{
   typedef typename remove_cv<T>::type no_cv_t;
   AUTOBOOST_STATIC_CONSTANT(bool, value = (!(::autoboost::detail::is_signed_values<T>::minus_one  > autoboost::detail::is_signed_values<T>::zero)));
};

template <bool integral_type>
struct is_signed_select_helper
{
   template <class T>
   struct rebind
   {
      typedef is_signed_helper<T> type;
   };
};

template <>
struct is_signed_select_helper<false>
{
   template <class T>
   struct rebind
   {
      typedef false_type type;
   };
};

template <class T>
struct is_signed_imp
{
   typedef is_signed_select_helper<
      ::autoboost::type_traits::ice_or<
         ::autoboost::is_integral<T>::value,
         ::autoboost::is_enum<T>::value>::value
   > selector;
   typedef typename selector::template rebind<T> binder;
   typedef typename binder::type type;
   AUTOBOOST_STATIC_CONSTANT(bool, value = type::value);
};

#else

template <class T> struct is_signed_imp : public false_type{};
template <> struct is_signed_imp<signed char> : public true_type{};
template <> struct is_signed_imp<const signed char> : public true_type{};
template <> struct is_signed_imp<volatile signed char> : public true_type{};
template <> struct is_signed_imp<const volatile signed char> : public true_type{};
template <> struct is_signed_imp<short> : public true_type{};
template <> struct is_signed_imp<const short> : public true_type{};
template <> struct is_signed_imp<volatile short> : public true_type{};
template <> struct is_signed_imp<const volatile short> : public true_type{};
template <> struct is_signed_imp<int> : public true_type{};
template <> struct is_signed_imp<const int> : public true_type{};
template <> struct is_signed_imp<volatile int> : public true_type{};
template <> struct is_signed_imp<const volatile int> : public true_type{};
template <> struct is_signed_imp<long> : public true_type{};
template <> struct is_signed_imp<const long> : public true_type{};
template <> struct is_signed_imp<volatile long> : public true_type{};
template <> struct is_signed_imp<const volatile long> : public true_type{};
#ifdef AUTOBOOST_HAS_LONG_LONG
template <> struct is_signed_imp<long long> : public true_type{};
template <> struct is_signed_imp<const long long> : public true_type{};
template <> struct is_signed_imp<volatile long long> : public true_type{};
template <> struct is_signed_imp<const volatile long long> : public true_type{};
#endif
#if defined(CHAR_MIN) && (CHAR_MIN != 0)
template <> struct is_signed_imp<char> : public true_type{};
template <> struct is_signed_imp<const char> : public true_type{};
template <> struct is_signed_imp<volatile char> : public true_type{};
template <> struct is_signed_imp<const volatile char> : public true_type{};
#endif
#if defined(WCHAR_MIN) && (WCHAR_MIN != 0)
template <> struct is_signed_imp<wchar_t> : public true_type{};
template <> struct is_signed_imp<const wchar_t> : public true_type{};
template <> struct is_signed_imp<volatile wchar_t> : public true_type{};
template <> struct is_signed_imp<const volatile wchar_t> : public true_type{};
#endif

#endif

}

#endif // !defined( __CODEGEARC__ )

#if defined( __CODEGEARC__ )
AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_signed,T,__is_signed(T))
#else
AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_signed,T,::autoboost::detail::is_signed_imp<T>::value)
#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_MEMBER_FUNCTION_POINTER_HPP_INCLUDED
