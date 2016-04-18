//  (C) Copyright Ion Gaztanaga 2014.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_COPY_ASSIGNABLE_HPP_INCLUDED
#define AUTOBOOST_TT_IS_COPY_ASSIGNABLE_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <autoboost/type_traits/detail/yes_no_type.hpp>
#include <autoboost/type_traits/is_base_and_derived.hpp>
#include <autoboost/noncopyable.hpp>

#if !defined(AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS) && !defined(AUTOBOOST_NO_CXX11_DECLTYPE) \
   && !defined(AUTOBOOST_INTEL_CXX_VERSION) && \
      !(defined(AUTOBOOST_MSVC) && _MSC_VER == 1800)
#define AUTOBOOST_TT_CXX11_IS_COPY_ASSIGNABLE
#include <autoboost/utility/declval.hpp>
#else
   //For compilers without decltype
   #include <autoboost/type_traits/is_const.hpp>
   #include <autoboost/type_traits/is_array.hpp>
   #include <autoboost/type_traits/add_reference.hpp>
   #include <autoboost/type_traits/remove_reference.hpp>
#endif


// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

namespace detail{

template <bool DerivedFromNoncopyable, class T>
struct is_copy_assignable_impl2 {

// Intel compiler has problems with SFINAE for copy constructors and deleted functions:
//
// error: function *function_name* cannot be referenced -- it is a deleted function
// static autoboost::type_traits::yes_type test(T1&, decltype(T1(autoboost::declval<T1&>()))* = 0);
//                                                        ^
//
// MSVC 12.0 (Visual 2013) has problems when the copy constructor has been deleted. See:
// https://connect.microsoft.com/VisualStudio/feedback/details/800328/std-is-copy-constructible-is-broken
#if defined(AUTOBOOST_TT_CXX11_IS_COPY_ASSIGNABLE)
    typedef autoboost::type_traits::yes_type yes_type;
    typedef autoboost::type_traits::no_type  no_type;

    template <class U>
    static decltype(::autoboost::declval<U&>() = ::autoboost::declval<const U&>(), yes_type() ) test(int);

    template <class>
    static no_type test(...);

    static const bool value = sizeof(test<T>(0)) == sizeof(yes_type);

#else
    static AUTOBOOST_DEDUCED_TYPENAME autoboost::add_reference<T>::type produce();

    template <class T1>
    static autoboost::type_traits::no_type test(T1&, typename T1::autoboost_move_no_copy_constructor_or_assign* = 0);

    static autoboost::type_traits::yes_type test(...);
    // If you see errors like this:
    //
    //      `'T::operator=(const T&)' is private`
    //      `autoboost/type_traits/is_copy_assignable.hpp:NN:M: error: within this context`
    //
    // then you are trying to call that macro for a structure defined like that:
    //
    //      struct T {
    //          ...
    //      private:
    //          T & operator=(const T &);
    //          ...
    //      };
    //
    // To fix that you must modify your structure:
    //
    //      // C++03 and C++11 version
    //      struct T: private autoboost::noncopyable {
    //          ...
    //      private:
    //          T & operator=(const T &);
    //          ...
    //      };
    //
    //      // C++11 version
    //      struct T {
    //          ...
    //      private:
    //          T& operator=(const T &) = delete;
    //          ...
    //      };
    AUTOBOOST_STATIC_CONSTANT(bool, value = (
            sizeof(test(produce())) == sizeof(autoboost::type_traits::yes_type)
    ));
   #endif
};

template <class T>
struct is_copy_assignable_impl2<true, T> {
    AUTOBOOST_STATIC_CONSTANT(bool, value = false);
};

template <class T>
struct is_copy_assignable_impl {

#if !defined(AUTOBOOST_TT_CXX11_IS_COPY_ASSIGNABLE)
    //For compilers without decltype, at least return false on const types, arrays
    //types derived from autoboost::noncopyable and types defined as AUTOBOOST_MOVEABLE_BUT_NOT_COPYABLE
    typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::remove_reference<T>::type unreferenced_t;
    AUTOBOOST_STATIC_CONSTANT(bool, value = (
        autoboost::detail::is_copy_assignable_impl2<
            autoboost::is_base_and_derived<autoboost::noncopyable, T>::value
            || autoboost::is_const<unreferenced_t>::value || autoboost::is_array<unreferenced_t>::value
            ,T
        >::value
    ));
    #else
    AUTOBOOST_STATIC_CONSTANT(bool, value = (
        autoboost::detail::is_copy_assignable_impl2<
            autoboost::is_base_and_derived<autoboost::noncopyable, T>::value,T
        >::value
    ));
    #endif
};

} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_copy_assignable,T,::autoboost::detail::is_copy_assignable_impl<T>::value)
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(is_copy_assignable,void,false)
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(is_copy_assignable,void const,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(is_copy_assignable,void const volatile,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(is_copy_assignable,void volatile,false)
#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_COPY_ASSIGNABLE_HPP_INCLUDED
