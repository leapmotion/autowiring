
//  (C) Copyright John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_ALIGNMENT_OF_HPP_INCLUDED
#define AUTOBOOST_TT_ALIGNMENT_OF_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <cstddef>

#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/integral_constant.hpp>

#ifdef AUTOBOOST_MSVC
#   pragma warning(push)
#   pragma warning(disable: 4121 4512) // alignment is sensitive to packing
#endif
#if defined(__BORLANDC__) && (__BORLANDC__ < 0x600)
#pragma option push -Vx- -Ve-
#endif

namespace autoboost {

template <typename T> struct alignment_of;

// get the alignment of some arbitrary type:
namespace detail {

#ifdef AUTOBOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4324) // structure was padded due to __declspec(align())
#endif
template <typename T>
struct alignment_of_hack
{
    char c;
    T t;
    alignment_of_hack();
};
#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

template <unsigned A, unsigned S>
struct alignment_logic
{
    AUTOBOOST_STATIC_CONSTANT(std::size_t, value = A < S ? A : S);
};


template< typename T >
struct alignment_of_impl
{
#if defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC >= 1400)
    //
    // With MSVC both the native __alignof operator
    // and our own logic gets things wrong from time to time :-(
    // Using a combination of the two seems to make the most of a bad job:
    //
    AUTOBOOST_STATIC_CONSTANT(std::size_t, value =
        (::autoboost::detail::alignment_logic<
            sizeof(::autoboost::detail::alignment_of_hack<T>) - sizeof(T),
            __alignof(T)
        >::value));
#elif !defined(AUTOBOOST_ALIGNMENT_OF)
    AUTOBOOST_STATIC_CONSTANT(std::size_t, value =
        (::autoboost::detail::alignment_logic<
            sizeof(::autoboost::detail::alignment_of_hack<T>) - sizeof(T),
            sizeof(T)
        >::value));
#else
   //
   // We put this here, rather than in the definition of
   // alignment_of below, because MSVC's __alignof doesn't
   // always work in that context for some unexplained reason.
   // (See type_with_alignment tests for test cases).
   //
   AUTOBOOST_STATIC_CONSTANT(std::size_t, value = AUTOBOOST_ALIGNMENT_OF(T));
#endif
};

} // namespace detail

template <class T> struct alignment_of : public integral_constant<std::size_t, ::autoboost::detail::alignment_of_impl<T>::value>{};

// references have to be treated specially, assume
// that a reference is just a special pointer:
template <typename T> struct alignment_of<T&> : public alignment_of<T*>{};

#ifdef __BORLANDC__
// long double gives an incorrect value of 10 (!)
// unless we do this...
struct long_double_wrapper{ long double ld; };
template<> struct alignment_of<long double> : public alignment_of<long_double_wrapper>{};
#endif

// void has to be treated specially:
template<> struct alignment_of<void> : integral_constant<std::size_t, 0>{};
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
template<> struct alignment_of<void const> : integral_constant<std::size_t, 0>{};
template<> struct alignment_of<void const volatile> : integral_constant<std::size_t, 0>{};
template<> struct alignment_of<void volatile> : integral_constant<std::size_t, 0>{};
#endif

} // namespace autoboost

#if defined(__BORLANDC__) && (__BORLANDC__ < 0x600)
#pragma option pop
#endif
#ifdef AUTOBOOST_MSVC
#   pragma warning(pop)
#endif

#endif // AUTOBOOST_TT_ALIGNMENT_OF_HPP_INCLUDED

