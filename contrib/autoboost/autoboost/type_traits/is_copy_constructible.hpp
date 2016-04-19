//  (C) Copyright Antony Polukhin 2013.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_COPY_CONSTRUCTIBLE_HPP_INCLUDED
#define AUTOBOOST_TT_IS_COPY_CONSTRUCTIBLE_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(AUTOBOOST_NO_CXX11_DECLTYPE) && !AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1800) && !AUTOBOOST_WORKAROUND(AUTOBOOST_GCC_VERSION, < 40900)

#include <autoboost/type_traits/is_constructible.hpp>

#if !AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1800)

namespace autoboost {

template <class T> struct is_copy_constructible : public autoboost::is_constructible<T, const T&>{};

template <> struct is_copy_constructible<void> : public false_type{};
template <> struct is_copy_constructible<void const> : public false_type{};
template <> struct is_copy_constructible<void const volatile> : public false_type{};
template <> struct is_copy_constructible<void volatile> : public false_type{};

} // namespace autoboost

#else
//
// Special version for VC12 which has a problem when a base class (such as non_copyable) has a deleted
// copy constructor.  In this case the compiler thinks there really is a copy-constructor and tries to
// instantiate the deleted member.  std::is_copy_constructible has the same issue (or at least returns
// an incorrect value, which just defers the issue into the users code) as well.  We can at least fix
// autoboost::non_copyable as a base class as a special case:
//
#include <autoboost/type_traits/is_base_and_derived.hpp>
#include <autoboost/noncopyable.hpp>

namespace autoboost {

   namespace detail
   {

      template <class T, bool b> struct is_copy_constructible_imp : public autoboost::is_constructible<T, const T&>{};
      template <class T> struct is_copy_constructible_imp<T, true> : public false_type{};

   }

   template <class T> struct is_copy_constructible : public detail::is_copy_constructible_imp<T, is_base_and_derived<autoboost::noncopyable, T>::value>{};

   template <> struct is_copy_constructible<void> : public false_type{};
   template <> struct is_copy_constructible<void const> : public false_type{};
   template <> struct is_copy_constructible<void const volatile> : public false_type{};
   template <> struct is_copy_constructible<void volatile> : public false_type{};

} // namespace autoboost

#endif

#else

#include <autoboost/type_traits/detail/yes_no_type.hpp>
#include <autoboost/type_traits/is_base_and_derived.hpp>
#include <autoboost/type_traits/add_reference.hpp>
#include <autoboost/type_traits/is_rvalue_reference.hpp>
#include <autoboost/type_traits/declval.hpp>
#include <autoboost/type_traits/is_array.hpp>
#include <autoboost/type_traits/declval.hpp>
#include <autoboost/noncopyable.hpp>

#ifdef AUTOBOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4181)
#endif

namespace autoboost {

   namespace detail{

      template <bool DerivedFromNoncopyable, class T>
      struct is_copy_constructible_impl2 {

         // Intel compiler has problems with SFINAE for copy constructors and deleted functions:
         //
         // error: function *function_name* cannot be referenced -- it is a deleted function
         // static autoboost::type_traits::yes_type test(T1&, decltype(T1(autoboost::declval<T1&>()))* = 0);
         //                                                        ^
         //
         // MSVC 12.0 (Visual 2013) has problems when the copy constructor has been deleted. See:
         // https://connect.microsoft.com/VisualStudio/feedback/details/800328/std-is-copy-constructible-is-broken
#if !defined(AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS) && !defined(AUTOBOOST_INTEL_CXX_VERSION) && !(defined(AUTOBOOST_MSVC) && _MSC_VER == 1800)

#ifdef AUTOBOOST_NO_CXX11_DECLTYPE
         template <class T1>
         static autoboost::type_traits::yes_type test(const T1&, autoboost::mpl::int_<sizeof(T1(autoboost::declval<const T1&>()))>* = 0);
#else
         template <class T1>
         static autoboost::type_traits::yes_type test(const T1&, decltype(T1(autoboost::declval<const T1&>()))* = 0);
#endif

         static autoboost::type_traits::no_type test(...);
#else
         template <class T1>
         static autoboost::type_traits::no_type test(const T1&, typename T1::autoboost_move_no_copy_constructor_or_assign* = 0);
         static autoboost::type_traits::yes_type test(...);
#endif

         // If you see errors like this:
         //
         //      `'T::T(const T&)' is private`
         //      `autoboost/type_traits/is_copy_constructible.hpp:68:5: error: within this context`
         //
         // then you are trying to call that macro for a structure defined like that:
         //
         //      struct T {
         //          ...
         //      private:
         //          T(const T &);
         //          ...
         //      };
         //
         // To fix that you must modify your structure:
         //
         //      // C++03 and C++11 version
         //      struct T: private autoboost::noncopyable {
         //          ...
         //      private:
         //          T(const T &);
         //          ...
         //      };
         //
         //      // C++11 version
         //      struct T {
         //          ...
         //      private:
         //          T(const T &) = delete;
         //          ...
         //      };
         AUTOBOOST_STATIC_CONSTANT(bool, value = (
            sizeof(test(
            autoboost::declval<AUTOBOOST_DEDUCED_TYPENAME autoboost::add_reference<T const>::type>()
            )) == sizeof(autoboost::type_traits::yes_type)
            &&
            !autoboost::is_rvalue_reference<T>::value
            && !autoboost::is_array<T>::value
            ));
      };

      template <class T>
      struct is_copy_constructible_impl2<true, T> {
         AUTOBOOST_STATIC_CONSTANT(bool, value = false);
      };

      template <class T>
      struct is_copy_constructible_impl {

         AUTOBOOST_STATIC_CONSTANT(bool, value = (
            autoboost::detail::is_copy_constructible_impl2<
            autoboost::is_base_and_derived<autoboost::noncopyable, T>::value,
            T
            >::value
            ));
      };

   } // namespace detail

   template <class T> struct is_copy_constructible : public integral_constant<bool, ::autoboost::detail::is_copy_constructible_impl<T>::value>{};
   template <> struct is_copy_constructible<void> : public false_type{};
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
   template <> struct is_copy_constructible<void const> : public false_type{};
   template <> struct is_copy_constructible<void volatile> : public false_type{};
   template <> struct is_copy_constructible<void const volatile> : public false_type{};
#endif

} // namespace autoboost

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

#endif

#endif // AUTOBOOST_TT_IS_COPY_CONSTRUCTIBLE_HPP_INCLUDED
