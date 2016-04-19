//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000-2003.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_IS_CLASS_HPP_INCLUDED
#define AUTOBOOST_TT_IS_CLASS_HPP_INCLUDED

#include <autoboost/type_traits/detail/config.hpp>
#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/integral_constant.hpp>
#ifndef AUTOBOOST_IS_CLASS
#   include <autoboost/type_traits/is_union.hpp>

#ifdef AUTOBOOST_TT_HAS_CONFORMING_IS_CLASS_IMPLEMENTATION
#   include <autoboost/type_traits/detail/yes_no_type.hpp>
#else
#   include <autoboost/type_traits/is_scalar.hpp>
#   include <autoboost/type_traits/is_array.hpp>
#   include <autoboost/type_traits/is_reference.hpp>
#   include <autoboost/type_traits/is_void.hpp>
#   include <autoboost/type_traits/is_function.hpp>
#endif

#endif // AUTOBOOST_IS_CLASS

namespace autoboost {

namespace detail {

#ifndef AUTOBOOST_IS_CLASS
#ifdef AUTOBOOST_TT_HAS_CONFORMING_IS_CLASS_IMPLEMENTATION

// This is actually the conforming implementation which works with
// abstract classes.  However, enough compilers have trouble with
// it that most will use the one in
// autoboost/type_traits/object_traits.hpp. This implementation
// actually works with VC7.0, but other interactions seem to fail
// when we use it.

// is_class<> metafunction due to Paul Mensonides
// (leavings@attbi.com). For more details:
// http://groups.google.com/groups?hl=en&selm=000001c1cc83%24e154d5e0%247772e50c%40c161550a&rnum=1
#if defined(__GNUC__)  && !defined(__EDG_VERSION__)

template <class U> ::autoboost::type_traits::yes_type is_class_tester(void(U::*)(void));
template <class U> ::autoboost::type_traits::no_type is_class_tester(...);

template <typename T>
struct is_class_impl
{

    AUTOBOOST_STATIC_CONSTANT(bool, value =
            sizeof(is_class_tester<T>(0)) == sizeof(::autoboost::type_traits::yes_type)
            && ! ::autoboost::is_union<T>::value
        );
};

#else

template <typename T>
struct is_class_impl
{
    template <class U> static ::autoboost::type_traits::yes_type is_class_tester(void(U::*)(void));
    template <class U> static ::autoboost::type_traits::no_type is_class_tester(...);

    AUTOBOOST_STATIC_CONSTANT(bool, value =
            sizeof(is_class_tester<T>(0)) == sizeof(::autoboost::type_traits::yes_type)
            && ! ::autoboost::is_union<T>::value
        );
};

#endif

#else

template <typename T>
struct is_class_impl
{
    AUTOBOOST_STATIC_CONSTANT(bool, value =
        ! ::autoboost::is_union<T>::value >::value
        && ! ::autoboost::is_scalar<T>::value
        && ! ::autoboost::is_array<T>::value
        && ! ::autoboost::is_reference<T>::value
        && ! ::autoboost::is_void<T>::value
        && ! ::autoboost::is_function<T>::value
        );
};

# endif // AUTOBOOST_TT_HAS_CONFORMING_IS_CLASS_IMPLEMENTATION
# else // AUTOBOOST_IS_CLASS
template <typename T>
struct is_class_impl
{
    AUTOBOOST_STATIC_CONSTANT(bool, value = AUTOBOOST_IS_CLASS(T));
};
# endif // AUTOBOOST_IS_CLASS

} // namespace detail

template <class T> struct is_class : public integral_constant<bool, ::autoboost::detail::is_class_impl<T>::value> {};
# ifdef __EDG_VERSION__
template <class T> struct is_class<const T> : public is_class<T>{};
template <class T> struct is_class<const volatile T> : public is_class<T>{};
template <class T> struct is_class<volatile T> : public is_class<T>{};
# endif

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_CLASS_HPP_INCLUDED
