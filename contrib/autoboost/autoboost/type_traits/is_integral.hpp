
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_INTEGRAL_HPP_INCLUDED
#define AUTOBOOST_TT_IS_INTEGRAL_HPP_INCLUDED

#include <autoboost/config.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

//* is a type T an [cv-qualified-] integral type described in the standard (3.9.1p3)
// as an extension we include long long, as this is likely to be added to the
// standard at a later date
#if defined( __CODEGEARC__ )
AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_integral,T,__is_integral(T))
#else
AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_integral,T,false)

AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned char,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned short,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned int,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned long,true)

AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,signed char,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,signed short,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,signed int,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,signed long,true)

AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,bool,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,char,true)

#ifndef AUTOBOOST_NO_INTRINSIC_WCHAR_T
// If the following line fails to compile and you're using the Intel
// compiler, see http://lists.boost.org/MailArchives/autoboost-users/msg06567.php,
// and define AUTOBOOST_NO_INTRINSIC_WCHAR_T on the command line.
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,wchar_t,true)
#endif

// Same set of integral types as in autoboost/type_traits/integral_promotion.hpp.
// Please, keep in sync. -- Alexander Nasonov
#if (defined(AUTOBOOST_INTEL_CXX_VERSION) && defined(_MSC_VER) && (AUTOBOOST_INTEL_CXX_VERSION <= 600)) \
    || (defined(__BORLANDC__) && (__BORLANDC__ == 0x600) && (_MSC_VER < 1300))
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned __int8,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,__int8,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned __int16,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,__int16,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned __int32,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,__int32,true)
#ifdef __BORLANDC__
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned __int64,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,__int64,true)
#endif
#endif

# if defined(AUTOBOOST_HAS_LONG_LONG)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral, ::autoboost::ulong_long_type,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral, ::autoboost::long_long_type,true)
#elif defined(AUTOBOOST_HAS_MS_INT64)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,unsigned __int64,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,__int64,true)
#endif

#ifdef AUTOBOOST_HAS_INT128
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,autoboost::int128_type,true)
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,autoboost::uint128_type,true)
#endif
#ifndef AUTOBOOST_NO_CXX11_CHAR16_T
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,char16_t,true)
#endif
#ifndef AUTOBOOST_NO_CXX11_CHAR32_T
AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(is_integral,char32_t,true)
#endif

#endif  // non-CodeGear implementation

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_INTEGRAL_HPP_INCLUDED
