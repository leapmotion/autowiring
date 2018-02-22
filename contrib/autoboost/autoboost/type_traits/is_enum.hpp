
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_IS_ENUM_HPP_INCLUDED
#define AUTOBOOST_TT_IS_ENUM_HPP_INCLUDED

#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/integral_constant.hpp>
#ifndef AUTOBOOST_IS_ENUM
#include <autoboost/type_traits/add_reference.hpp>
#include <autoboost/type_traits/is_arithmetic.hpp>
#include <autoboost/type_traits/is_reference.hpp>
#include <autoboost/type_traits/is_convertible.hpp>
#include <autoboost/type_traits/is_array.hpp>
#ifdef __GNUC__
#include <autoboost/type_traits/is_function.hpp>
#endif
#include <autoboost/type_traits/detail/config.hpp>
#if defined(AUTOBOOST_TT_HAS_CONFORMING_IS_CLASS_IMPLEMENTATION)
#  include <autoboost/type_traits/is_class.hpp>
#  include <autoboost/type_traits/is_union.hpp>
#endif
#endif

namespace autoboost {

#ifndef AUTOBOOST_IS_ENUM
#if !(defined(__BORLANDC__) && (__BORLANDC__ <= 0x551))

namespace detail {

#if defined(AUTOBOOST_TT_HAS_CONFORMING_IS_CLASS_IMPLEMENTATION)

template <typename T>
struct is_class_or_union
{
   AUTOBOOST_STATIC_CONSTANT(bool, value = ::autoboost::is_class<T>::value || ::autoboost::is_union<T>::value);
};

#else

template <typename T>
struct is_class_or_union
{
# if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x581))// we simply can't detect it this way.
    AUTOBOOST_STATIC_CONSTANT(bool, value = false);
# else
    template <class U> static ::autoboost::type_traits::yes_type is_class_or_union_tester(void(U::*)(void));

#  if AUTOBOOST_WORKAROUND(__MWERKS__, <= 0x3000) // no SFINAE
    static ::autoboost::type_traits::no_type is_class_or_union_tester(...);
    AUTOBOOST_STATIC_CONSTANT(
        bool, value = sizeof(is_class_or_union_tester(0)) == sizeof(::autoboost::type_traits::yes_type));
#  else
    template <class U>
    static ::autoboost::type_traits::no_type is_class_or_union_tester(...);
    AUTOBOOST_STATIC_CONSTANT(
        bool, value = sizeof(is_class_or_union_tester<T>(0)) == sizeof(::autoboost::type_traits::yes_type));
#  endif
# endif
};
#endif

struct int_convertible
{
    int_convertible(int);
};

// Don't evaluate convertibility to int_convertible unless the type
// is non-arithmetic. This suppresses warnings with GCC.
template <bool is_typename_arithmetic_or_reference = true>
struct is_enum_helper
{
    template <typename T> struct type
    {
        AUTOBOOST_STATIC_CONSTANT(bool, value = false);
    };
};

template <>
struct is_enum_helper<false>
{
    template <typename T> struct type
    {
       static const bool value = ::autoboost::is_convertible<typename autoboost::add_reference<T>::type, ::autoboost::detail::int_convertible>::value;
    };
};

template <typename T> struct is_enum_impl
{
   //typedef ::autoboost::add_reference<T> ar_t;
   //typedef typename ar_t::type r_type;

#if defined(__GNUC__)

#ifdef AUTOBOOST_TT_HAS_CONFORMING_IS_CLASS_IMPLEMENTATION

   // We MUST check for is_class_or_union on conforming compilers in
   // order to correctly deduce that noncopyable types are not enums
   // (dwa 2002/04/15)...
   AUTOBOOST_STATIC_CONSTANT(bool, selector =
           ::autoboost::is_arithmetic<T>::value
         || ::autoboost::is_reference<T>::value
         || ::autoboost::is_function<T>::value
         || is_class_or_union<T>::value
         || is_array<T>::value);
#else
   // ...however, not checking is_class_or_union on non-conforming
   // compilers prevents a dependency recursion.
   AUTOBOOST_STATIC_CONSTANT(bool, selector =
           ::autoboost::is_arithmetic<T>::value
         || ::autoboost::is_reference<T>::value
         || ::autoboost::is_function<T>::value
         || is_array<T>::value);
#endif // AUTOBOOST_TT_HAS_CONFORMING_IS_CLASS_IMPLEMENTATION

#else // !defined(__GNUC__):

   AUTOBOOST_STATIC_CONSTANT(bool, selector =
           ::autoboost::is_arithmetic<T>::value
         || ::autoboost::is_reference<T>::value
         || is_class_or_union<T>::value
         || is_array<T>::value);

#endif

#if AUTOBOOST_WORKAROUND(__BORLANDC__, < 0x600)
    typedef ::autoboost::detail::is_enum_helper<
          ::autoboost::detail::is_enum_impl<T>::selector
        > se_t;
#else
    typedef ::autoboost::detail::is_enum_helper<selector> se_t;
#endif

    typedef typename se_t::template type<T> helper;
    AUTOBOOST_STATIC_CONSTANT(bool, value = helper::value);
};

} // namespace detail

template <class T> struct is_enum : public integral_constant<bool, ::autoboost::detail::is_enum_impl<T>::value> {};

#else // __BORLANDC__
//
// buggy is_convertible prevents working
// implementation of is_enum:
template <class T> struct is_enum : public integral_constant<bool, false> {};

#endif

#else // AUTOBOOST_IS_ENUM

template <class T> struct is_enum : public integral_constant<bool, AUTOBOOST_IS_ENUM(T)> {};

#endif

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_ENUM_HPP_INCLUDED
