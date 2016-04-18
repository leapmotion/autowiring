
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes,
//      Howard Hinnant and John Maddock 2000.
//  (C) Copyright Mat Marcus, Jesse Jones and Adobe Systems Inc 2001

//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

//    Fixed is_pointer, is_reference, is_const, is_volatile, is_same,
//    is_member_pointer based on the Simulated Partial Specialization work
//    of Mat Marcus and Jesse Jones. See  http://opensource.adobe.com or
//    http://groups.yahoo.com/group/autoboost/message/5441
//    Some workarounds in here use ideas suggested from "Generic<Programming>:
//    Mappings between Types and Values"
//    by Andrei Alexandrescu (see http://www.cuj.com/experts/1810/alexandr.html).


#ifndef AUTOBOOST_TT_IS_CONST_HPP_INCLUDED
#define AUTOBOOST_TT_IS_CONST_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

#   include <autoboost/type_traits/detail/cv_traits_impl.hpp>
#   ifdef __GNUC__
#       include <autoboost/type_traits/is_reference.hpp>
#   endif
#   if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1400)
#       include <autoboost/type_traits/remove_bounds.hpp>
#   endif

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

#if defined( __CODEGEARC__ )

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_const,T,__is_const(T))

#else

namespace detail{
//
// We can't filter out rvalue_references at the same level as
// references or we get ambiguities from msvc:
//
template <class T>
struct is_const_rvalue_filter
{
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1400)
   AUTOBOOST_STATIC_CONSTANT(bool, value = ::autoboost::detail::cv_traits_imp<typename autoboost::remove_bounds<T>::type*>::is_const);
#else
   AUTOBOOST_STATIC_CONSTANT(bool, value = ::autoboost::detail::cv_traits_imp<AUTOBOOST_TT_AUX_CV_TRAITS_IMPL_PARAM(T)>::is_const);
#endif
};
#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
template <class T>
struct is_const_rvalue_filter<T&&>
{
   AUTOBOOST_STATIC_CONSTANT(bool, value = false);
};
#endif
}

//* is a type T  declared const - is_const<T>
AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_const,T,::autoboost::detail::is_const_rvalue_filter<T>::value)
AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_const,T&,false)

#if  defined(AUTOBOOST_ILLEGAL_CV_REFERENCES)
// these are illegal specialisations; cv-qualifies applied to
// references have no effect according to [8.3.2p1],
// C++ Builder requires them though as it treats cv-qualified
// references as distinct types...
AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_const,T& const,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_const,T& volatile,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_const,T& const volatile,false)
#endif

#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_CONST_HPP_INCLUDED

