
// (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_EMPTY_HPP_INCLUDED
#define AUTOBOOST_TT_IS_EMPTY_HPP_INCLUDED

#include <autoboost/type_traits/is_convertible.hpp>
#include <autoboost/type_traits/detail/config.hpp>
#include <autoboost/type_traits/intrinsics.hpp>

#include <autoboost/type_traits/remove_cv.hpp>
#include <autoboost/type_traits/is_class.hpp>
#include <autoboost/type_traits/add_reference.hpp>

#ifndef AUTOBOOST_INTERNAL_IS_EMPTY
#define AUTOBOOST_INTERNAL_IS_EMPTY(T) false
#else
#define AUTOBOOST_INTERNAL_IS_EMPTY(T) AUTOBOOST_IS_EMPTY(T)
#endif

namespace autoboost {

namespace detail {


#ifdef AUTOBOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4624) // destructor could not be generated
#endif

template <typename T>
struct empty_helper_t1 : public T
{
    empty_helper_t1();  // hh compiler bug workaround
    int i[256];
private:
   // suppress compiler warnings:
   empty_helper_t1(const empty_helper_t1&);
   empty_helper_t1& operator=(const empty_helper_t1&);
};

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

struct empty_helper_t2 { int i[256]; };

#if !AUTOBOOST_WORKAROUND(__BORLANDC__, < 0x600)

template <typename T, bool is_a_class = false>
struct empty_helper
{
    AUTOBOOST_STATIC_CONSTANT(bool, value = false);
};

template <typename T>
struct empty_helper<T, true>
{
    AUTOBOOST_STATIC_CONSTANT(
        bool, value = (sizeof(empty_helper_t1<T>) == sizeof(empty_helper_t2))
        );
};

template <typename T>
struct is_empty_impl
{
    typedef typename remove_cv<T>::type cvt;
    AUTOBOOST_STATIC_CONSTANT(
        bool,
        value = ( ::autoboost::detail::empty_helper<cvt,::autoboost::is_class<T>::value>::value || AUTOBOOST_INTERNAL_IS_EMPTY(cvt)));
};

#else // __BORLANDC__

template <typename T, bool is_a_class, bool convertible_to_int>
struct empty_helper
{
    AUTOBOOST_STATIC_CONSTANT(bool, value = false);
};

template <typename T>
struct empty_helper<T, true, false>
{
    AUTOBOOST_STATIC_CONSTANT(bool, value = (
        sizeof(empty_helper_t1<T>) == sizeof(empty_helper_t2)
        ));
};

template <typename T>
struct is_empty_impl
{
   typedef typename remove_cv<T>::type cvt;
   typedef typename add_reference<T>::type r_type;

   AUTOBOOST_STATIC_CONSTANT(
       bool, value = (
              ::autoboost::detail::empty_helper<
                  cvt
                , ::autoboost::is_class<T>::value
                , ::autoboost::is_convertible< r_type,int>::value
              >::value || AUTOBOOST_INTERNAL_IS_EMPTY(cvt));
};

#endif // __BORLANDC__

} // namespace detail

template <class T> struct is_empty : integral_constant<bool, ::autoboost::detail::is_empty_impl<T>::value> {};

} // namespace autoboost

#undef AUTOBOOST_INTERNAL_IS_EMPTY

#endif // AUTOBOOST_TT_IS_EMPTY_HPP_INCLUDED

