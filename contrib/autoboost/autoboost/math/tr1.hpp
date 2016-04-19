// Copyright John Maddock 2008.
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_TR1_HPP
#define AUTOBOOST_MATH_TR1_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <math.h> // So we can check which std C lib we're using

#ifdef __cplusplus

#include <autoboost/config.hpp>
#include <autoboost/static_assert.hpp>

namespace autoboost{ namespace math{ namespace tr1{ extern "C"{

#else

#define AUTOBOOST_PREVENT_MACRO_SUBSTITUTION /**/

#endif // __cplusplus

// we need to import/export our code only if the user has specifically
// asked for it by defining either AUTOBOOST_ALL_DYN_LINK if they want all autoboost
// libraries to be dynamically linked, or AUTOBOOST_MATH_TR1_DYN_LINK
// if they want just this one to be dynamically liked:
#if defined(AUTOBOOST_ALL_DYN_LINK) || defined(AUTOBOOST_MATH_TR1_DYN_LINK)
// export if this is our own source, otherwise import:
#ifdef AUTOBOOST_MATH_TR1_SOURCE
# define AUTOBOOST_MATH_TR1_DECL AUTOBOOST_SYMBOL_EXPORT
#else
# define AUTOBOOST_MATH_TR1_DECL AUTOBOOST_SYMBOL_IMPORT
#endif  // AUTOBOOST_MATH_TR1_SOURCE
#else
#  define AUTOBOOST_MATH_TR1_DECL
#endif  // DYN_LINK
//
// Set any throw specifications on the C99 extern "C" functions - these have to be
// the same as used in the std lib if any.
//
#if defined(__GLIBC__) && defined(__THROW)
#  define AUTOBOOST_MATH_C99_THROW_SPEC __THROW
#else
#  define AUTOBOOST_MATH_C99_THROW_SPEC
#endif

//
// Now set up the libraries to link against:
//
#if !defined(AUTOBOOST_MATH_TR1_NO_LIB) && !defined(AUTOBOOST_MATH_TR1_SOURCE) \
   && !defined(AUTOBOOST_ALL_NO_LIB) && defined(__cplusplus)
#  define AUTOBOOST_LIB_NAME autoboost_math_c99
#  if defined(AUTOBOOST_MATH_TR1_DYN_LINK) || defined(AUTOBOOST_ALL_DYN_LINK)
#     define AUTOBOOST_DYN_LINK
#  endif
#  include <autoboost/config/auto_link.hpp>
#endif
#if !defined(AUTOBOOST_MATH_TR1_NO_LIB) && !defined(AUTOBOOST_MATH_TR1_SOURCE) \
   && !defined(AUTOBOOST_ALL_NO_LIB) && defined(__cplusplus)
#  define AUTOBOOST_LIB_NAME autoboost_math_c99f
#  if defined(AUTOBOOST_MATH_TR1_DYN_LINK) || defined(AUTOBOOST_ALL_DYN_LINK)
#     define AUTOBOOST_DYN_LINK
#  endif
#  include <autoboost/config/auto_link.hpp>
#endif
#if !defined(AUTOBOOST_MATH_TR1_NO_LIB) && !defined(AUTOBOOST_MATH_TR1_SOURCE) \
   && !defined(AUTOBOOST_ALL_NO_LIB) && defined(__cplusplus) \
   && !defined(AUTOBOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS)
#  define AUTOBOOST_LIB_NAME autoboost_math_c99l
#  if defined(AUTOBOOST_MATH_TR1_DYN_LINK) || defined(AUTOBOOST_ALL_DYN_LINK)
#     define AUTOBOOST_DYN_LINK
#  endif
#  include <autoboost/config/auto_link.hpp>
#endif
#if !defined(AUTOBOOST_MATH_TR1_NO_LIB) && !defined(AUTOBOOST_MATH_TR1_SOURCE) \
   && !defined(AUTOBOOST_ALL_NO_LIB) && defined(__cplusplus)
#  define AUTOBOOST_LIB_NAME autoboost_math_tr1
#  if defined(AUTOBOOST_MATH_TR1_DYN_LINK) || defined(AUTOBOOST_ALL_DYN_LINK)
#     define AUTOBOOST_DYN_LINK
#  endif
#  include <autoboost/config/auto_link.hpp>
#endif
#if !defined(AUTOBOOST_MATH_TR1_NO_LIB) && !defined(AUTOBOOST_MATH_TR1_SOURCE) \
   && !defined(AUTOBOOST_ALL_NO_LIB) && defined(__cplusplus)
#  define AUTOBOOST_LIB_NAME autoboost_math_tr1f
#  if defined(AUTOBOOST_MATH_TR1_DYN_LINK) || defined(AUTOBOOST_ALL_DYN_LINK)
#     define AUTOBOOST_DYN_LINK
#  endif
#  include <autoboost/config/auto_link.hpp>
#endif
#if !defined(AUTOBOOST_MATH_TR1_NO_LIB) && !defined(AUTOBOOST_MATH_TR1_SOURCE) \
   && !defined(AUTOBOOST_ALL_NO_LIB) && defined(__cplusplus) \
   && !defined(AUTOBOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS)
#  define AUTOBOOST_LIB_NAME autoboost_math_tr1l
#  if defined(AUTOBOOST_MATH_TR1_DYN_LINK) || defined(AUTOBOOST_ALL_DYN_LINK)
#     define AUTOBOOST_DYN_LINK
#  endif
#  include <autoboost/config/auto_link.hpp>
#endif

#if !(defined(AUTOBOOST_INTEL) && defined(__APPLE__)) && !(defined(__FLT_EVAL_METHOD__) && !defined(__cplusplus))
#if !defined(FLT_EVAL_METHOD)
typedef float float_t;
typedef double double_t;
#elif FLT_EVAL_METHOD == -1
typedef float float_t;
typedef double double_t;
#elif FLT_EVAL_METHOD == 0
typedef float float_t;
typedef double double_t;
#elif FLT_EVAL_METHOD == 1
typedef double float_t;
typedef double double_t;
#else
typedef long double float_t;
typedef long double double_t;
#endif
#endif

// C99 Functions:
double AUTOBOOST_MATH_TR1_DECL autoboost_acosh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_acoshf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_acoshl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

double AUTOBOOST_MATH_TR1_DECL autoboost_asinh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_asinhf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_asinhl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

double AUTOBOOST_MATH_TR1_DECL autoboost_atanh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_atanhf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_atanhl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

double AUTOBOOST_MATH_TR1_DECL autoboost_cbrt AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_cbrtf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_cbrtl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

double AUTOBOOST_MATH_TR1_DECL autoboost_copysign AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_copysignf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_copysignl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y) AUTOBOOST_MATH_C99_THROW_SPEC;

double AUTOBOOST_MATH_TR1_DECL autoboost_erf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_erff AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_erfl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

double AUTOBOOST_MATH_TR1_DECL autoboost_erfc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_erfcf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_erfcl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#if 0
double AUTOBOOST_MATH_TR1_DECL autoboost_exp2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_exp2f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_exp2l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#endif
double AUTOBOOST_MATH_TR1_DECL autoboost_expm1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_expm1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_expm1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#if 0
double AUTOBOOST_MATH_TR1_DECL autoboost_fdim AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_fdimf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_fdiml AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y) AUTOBOOST_MATH_C99_THROW_SPEC;
double AUTOBOOST_MATH_TR1_DECL autoboost_fma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y, double z) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_fmaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y, float z) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_fmal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y, long double z) AUTOBOOST_MATH_C99_THROW_SPEC;
#endif
double AUTOBOOST_MATH_TR1_DECL autoboost_fmax AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_fmaxf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_fmaxl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y) AUTOBOOST_MATH_C99_THROW_SPEC;

double AUTOBOOST_MATH_TR1_DECL autoboost_fmin AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_fminf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_fminl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y) AUTOBOOST_MATH_C99_THROW_SPEC;

double AUTOBOOST_MATH_TR1_DECL autoboost_hypot AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_hypotf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_hypotl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y) AUTOBOOST_MATH_C99_THROW_SPEC;
#if 0
int AUTOBOOST_MATH_TR1_DECL autoboost_ilogb AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
int AUTOBOOST_MATH_TR1_DECL autoboost_ilogbf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
int AUTOBOOST_MATH_TR1_DECL autoboost_ilogbl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#endif
double AUTOBOOST_MATH_TR1_DECL autoboost_lgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_lgammaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_lgammal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#ifdef AUTOBOOST_HAS_LONG_LONG
#if 0
::autoboost::long_long_type AUTOBOOST_MATH_TR1_DECL autoboost_llrint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
::autoboost::long_long_type AUTOBOOST_MATH_TR1_DECL autoboost_llrintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
::autoboost::long_long_type AUTOBOOST_MATH_TR1_DECL autoboost_llrintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#endif
::autoboost::long_long_type AUTOBOOST_MATH_TR1_DECL autoboost_llround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
::autoboost::long_long_type AUTOBOOST_MATH_TR1_DECL autoboost_llroundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
::autoboost::long_long_type AUTOBOOST_MATH_TR1_DECL autoboost_llroundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#endif
double AUTOBOOST_MATH_TR1_DECL autoboost_log1p AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_log1pf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_log1pl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#if 0
double AUTOBOOST_MATH_TR1_DECL log2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL log2f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL log2l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

double AUTOBOOST_MATH_TR1_DECL logb AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL logbf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL logbl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
long AUTOBOOST_MATH_TR1_DECL lrint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
long AUTOBOOST_MATH_TR1_DECL lrintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long AUTOBOOST_MATH_TR1_DECL lrintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#endif
long AUTOBOOST_MATH_TR1_DECL autoboost_lround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
long AUTOBOOST_MATH_TR1_DECL autoboost_lroundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long AUTOBOOST_MATH_TR1_DECL autoboost_lroundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#if 0
double AUTOBOOST_MATH_TR1_DECL nan AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(const char *str) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL nanf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(const char *str) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL nanl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(const char *str) AUTOBOOST_MATH_C99_THROW_SPEC;
double AUTOBOOST_MATH_TR1_DECL nearbyint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL nearbyintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL nearbyintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#endif
double AUTOBOOST_MATH_TR1_DECL autoboost_nextafter AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_nextafterf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_nextafterl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y) AUTOBOOST_MATH_C99_THROW_SPEC;

double AUTOBOOST_MATH_TR1_DECL autoboost_nexttoward AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, long double y) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_nexttowardf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, long double y) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_nexttowardl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y) AUTOBOOST_MATH_C99_THROW_SPEC;
#if 0
double AUTOBOOST_MATH_TR1_DECL autoboost_remainder AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_remainderf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_remainderl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y) AUTOBOOST_MATH_C99_THROW_SPEC;
double AUTOBOOST_MATH_TR1_DECL autoboost_remquo AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y, int *pquo) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_remquof AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y, int *pquo) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_remquol AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y, int *pquo) AUTOBOOST_MATH_C99_THROW_SPEC;
double AUTOBOOST_MATH_TR1_DECL autoboost_rint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_rintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_rintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#endif
double AUTOBOOST_MATH_TR1_DECL autoboost_round AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_roundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_roundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#if 0
double AUTOBOOST_MATH_TR1_DECL autoboost_scalbln AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, long ex) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_scalblnf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, long ex) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_scalblnl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long ex) AUTOBOOST_MATH_C99_THROW_SPEC;
double AUTOBOOST_MATH_TR1_DECL autoboost_scalbn AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, int ex) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_scalbnf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, int ex) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_scalbnl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, int ex) AUTOBOOST_MATH_C99_THROW_SPEC;
#endif
double AUTOBOOST_MATH_TR1_DECL autoboost_tgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_tgammaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_tgammal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

double AUTOBOOST_MATH_TR1_DECL autoboost_trunc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_truncf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_truncl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.1] associated Laguerre polynomials:
double AUTOBOOST_MATH_TR1_DECL autoboost_assoc_laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, unsigned m, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_assoc_laguerref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, unsigned m, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_assoc_laguerrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, unsigned m, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.2] associated Legendre functions:
double AUTOBOOST_MATH_TR1_DECL autoboost_assoc_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_assoc_legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_assoc_legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.3] beta function:
double AUTOBOOST_MATH_TR1_DECL autoboost_beta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_betaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_betal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.4] (complete) elliptic integral of the first kind:
double AUTOBOOST_MATH_TR1_DECL autoboost_comp_ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double k) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_comp_ellint_1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_comp_ellint_1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.5] (complete) elliptic integral of the second kind:
double AUTOBOOST_MATH_TR1_DECL autoboost_comp_ellint_2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double k) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_comp_ellint_2f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_comp_ellint_2l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.6] (complete) elliptic integral of the third kind:
double AUTOBOOST_MATH_TR1_DECL autoboost_comp_ellint_3 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double k, double nu) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_comp_ellint_3f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k, float nu) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_comp_ellint_3l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k, long double nu) AUTOBOOST_MATH_C99_THROW_SPEC;
#if 0
// [5.2.1.7] confluent hypergeometric functions:
double AUTOBOOST_MATH_TR1_DECL conf_hyperg AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double a, double c, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL conf_hypergf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float a, float c, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL conf_hypergl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double a, long double c, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#endif
// [5.2.1.8] regular modified cylindrical Bessel functions:
double AUTOBOOST_MATH_TR1_DECL autoboost_cyl_bessel_i AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double nu, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_cyl_bessel_if AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_cyl_bessel_il AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.9] cylindrical Bessel functions (of the first kind):
double AUTOBOOST_MATH_TR1_DECL autoboost_cyl_bessel_j AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double nu, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_cyl_bessel_jf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_cyl_bessel_jl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.10] irregular modified cylindrical Bessel functions:
double AUTOBOOST_MATH_TR1_DECL autoboost_cyl_bessel_k AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double nu, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_cyl_bessel_kf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_cyl_bessel_kl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.11] cylindrical Neumann functions AUTOBOOST_MATH_C99_THROW_SPEC;
// cylindrical Bessel functions (of the second kind):
double AUTOBOOST_MATH_TR1_DECL autoboost_cyl_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double nu, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_cyl_neumannf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_cyl_neumannl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.12] (incomplete) elliptic integral of the first kind:
double AUTOBOOST_MATH_TR1_DECL autoboost_ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double k, double phi) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_ellint_1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k, float phi) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_ellint_1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k, long double phi) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.13] (incomplete) elliptic integral of the second kind:
double AUTOBOOST_MATH_TR1_DECL autoboost_ellint_2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double k, double phi) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_ellint_2f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k, float phi) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_ellint_2l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k, long double phi) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.14] (incomplete) elliptic integral of the third kind:
double AUTOBOOST_MATH_TR1_DECL autoboost_ellint_3 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double k, double nu, double phi) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_ellint_3f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k, float nu, float phi) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_ellint_3l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k, long double nu, long double phi) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.15] exponential integral:
double AUTOBOOST_MATH_TR1_DECL autoboost_expint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_expintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_expintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.16] Hermite polynomials:
double AUTOBOOST_MATH_TR1_DECL autoboost_hermite AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_hermitef AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_hermitel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

#if 0
// [5.2.1.17] hypergeometric functions:
double AUTOBOOST_MATH_TR1_DECL hyperg AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double a, double b, double c, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL hypergf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float a, float b, float c, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL hypergl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double a, long double b, long double c,
long double x) AUTOBOOST_MATH_C99_THROW_SPEC;
#endif

// [5.2.1.18] Laguerre polynomials:
double AUTOBOOST_MATH_TR1_DECL autoboost_laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_laguerref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_laguerrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.19] Legendre polynomials:
double AUTOBOOST_MATH_TR1_DECL autoboost_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.20] Riemann zeta function:
double AUTOBOOST_MATH_TR1_DECL autoboost_riemann_zeta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_riemann_zetaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_riemann_zetal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.21] spherical Bessel functions (of the first kind):
double AUTOBOOST_MATH_TR1_DECL autoboost_sph_bessel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_sph_besself AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_sph_bessell AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.22] spherical associated Legendre functions:
double AUTOBOOST_MATH_TR1_DECL autoboost_sph_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, double theta) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_sph_legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, float theta) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_sph_legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, long double theta) AUTOBOOST_MATH_C99_THROW_SPEC;

// [5.2.1.23] spherical Neumann functions AUTOBOOST_MATH_C99_THROW_SPEC;
// spherical Bessel functions (of the second kind):
double AUTOBOOST_MATH_TR1_DECL autoboost_sph_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, double x) AUTOBOOST_MATH_C99_THROW_SPEC;
float AUTOBOOST_MATH_TR1_DECL autoboost_sph_neumannf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x) AUTOBOOST_MATH_C99_THROW_SPEC;
long double AUTOBOOST_MATH_TR1_DECL autoboost_sph_neumannl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x) AUTOBOOST_MATH_C99_THROW_SPEC;

#ifdef __cplusplus

}}}}  // namespaces

#include <autoboost/math/tools/promotion.hpp>

namespace autoboost{ namespace math{ namespace tr1{
//
// Declare overload of the functions which forward to the
// C interfaces:
//
// C99 Functions:
inline double acosh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_acosh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float acoshf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_acoshf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double acoshl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_acoshl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float acosh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::acoshf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double acosh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::acoshl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type acosh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::acosh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }

inline double asinh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_asinh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float asinhf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_asinhf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double asinhl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_asinhl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float asinh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::asinhf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double asinh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::asinhl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type asinh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::asinh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }

inline double atanh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_atanh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float atanhf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_atanhf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double atanhl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_atanhl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float atanh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::atanhf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double atanh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::atanhl(x); }
template <class T>
inline typename tools::promote_args<T>::type atanh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::atanh AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }

inline double cbrt AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_cbrt AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float cbrtf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_cbrtf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double cbrtl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_cbrtl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float cbrt AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::cbrtf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double cbrt AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::cbrtl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type cbrt AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::cbrt AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }

inline double copysign AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y)
{ return autoboost::math::tr1::autoboost_copysign AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline float copysignf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::autoboost_copysignf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double copysignl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::autoboost_copysignl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline float copysign AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::copysignf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double copysign AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::copysignl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type copysign AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 x, T2 y)
{ return autoboost::math::tr1::copysign AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(x), static_cast<typename tools::promote_args<T1, T2>::type>(y)); }

inline double erf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_erf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float erff AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_erff AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double erfl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_erfl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float erf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::erff AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double erf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::erfl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type erf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::erf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }

inline double erfc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_erfc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float erfcf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_erfcf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double erfcl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_erfcl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float erfc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::erfcf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double erfc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::erfcl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type erfc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::erfc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }

#if 0
double exp2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x);
float exp2f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x);
long double exp2l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x);
#endif

inline float expm1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_expm1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline double expm1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_expm1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double expm1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_expm1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float expm1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::expm1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double expm1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::expm1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type expm1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::expm1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }

#if 0
double fdim AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y);
float fdimf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y);
long double fdiml AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y);
double fma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y, double z);
float fmaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y, float z);
long double fmal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y, long double z);
#endif
inline double fmax AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y)
{ return autoboost::math::tr1::autoboost_fmax AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline float fmaxf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::autoboost_fmaxf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double fmaxl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::autoboost_fmaxl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline float fmax AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::fmaxf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double fmax AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::fmaxl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type fmax AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 x, T2 y)
{ return autoboost::math::tr1::fmax AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(x), static_cast<typename tools::promote_args<T1, T2>::type>(y)); }

inline double fmin AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y)
{ return autoboost::math::tr1::autoboost_fmin AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline float fminf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::autoboost_fminf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double fminl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::autoboost_fminl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline float fmin AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::fminf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double fmin AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::fminl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type fmin AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 x, T2 y)
{ return autoboost::math::tr1::fmin AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(x), static_cast<typename tools::promote_args<T1, T2>::type>(y)); }

inline float hypotf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::autoboost_hypotf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline double hypot AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y)
{ return autoboost::math::tr1::autoboost_hypot AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double hypotl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::autoboost_hypotl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline float hypot AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::hypotf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double hypot AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::hypotl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type hypot AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 x, T2 y)
{ return autoboost::math::tr1::hypot AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(x), static_cast<typename tools::promote_args<T1, T2>::type>(y)); }

#if 0
int ilogb AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x);
int ilogbf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x);
int ilogbl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x);
#endif

inline float lgammaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_lgammaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline double lgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_lgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double lgammal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_lgammal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float lgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::lgammaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double lgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::lgammal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type lgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::lgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }

#ifdef AUTOBOOST_HAS_LONG_LONG
#if 0
::autoboost::long_long_type llrint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x);
::autoboost::long_long_type llrintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x);
::autoboost::long_long_type llrintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x);
#endif

inline ::autoboost::long_long_type llroundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_llroundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline ::autoboost::long_long_type llround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_llround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline ::autoboost::long_long_type llroundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_llroundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline ::autoboost::long_long_type llround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::llroundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline ::autoboost::long_long_type llround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::llroundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline ::autoboost::long_long_type llround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return llround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<double>(x)); }
#endif

inline float log1pf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_log1pf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline double log1p AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_log1p AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double log1pl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_log1pl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float log1p AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::log1pf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double log1p AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::log1pl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type log1p AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::log1p AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }
#if 0
double log2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x);
float log2f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x);
long double log2l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x);

double logb AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x);
float logbf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x);
long double logbl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x);
long lrint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x);
long lrintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x);
long lrintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x);
#endif
inline long lroundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_lroundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long lround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_lround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long lroundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_lroundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long lround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::lroundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long lround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::lroundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
long lround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::lround AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<double>(x)); }
#if 0
double nan AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(const char *str);
float nanf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(const char *str);
long double nanl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(const char *str);
double nearbyint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x);
float nearbyintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x);
long double nearbyintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x);
#endif
inline float nextafterf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::autoboost_nextafterf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline double nextafter AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y)
{ return autoboost::math::tr1::autoboost_nextafter AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double nextafterl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::autoboost_nextafterl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline float nextafter AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::nextafterf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double nextafter AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::nextafterl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type nextafter AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 x, T2 y)
{ return autoboost::math::tr1::nextafter AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(x), static_cast<typename tools::promote_args<T1, T2>::type>(y)); }

inline float nexttowardf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::autoboost_nexttowardf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline double nexttoward AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y)
{ return autoboost::math::tr1::autoboost_nexttoward AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double nexttowardl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::autoboost_nexttowardl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline float nexttoward AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::nexttowardf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double nexttoward AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::nexttowardl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type nexttoward AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 x, T2 y)
{ return autoboost::math::tr1::nexttoward AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(x), static_cast<long double>(y)); }
#if 0
double remainder AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y);
float remainderf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y);
long double remainderl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y);
double remquo AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y, int *pquo);
float remquof AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y, int *pquo);
long double remquol AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y, int *pquo);
double rint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x);
float rintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x);
long double rintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x);
#endif
inline float roundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_roundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline double round AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_round AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double roundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_roundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float round AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::roundf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double round AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::roundl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type round AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::round AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }
#if 0
double scalbln AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, long ex);
float scalblnf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, long ex);
long double scalblnl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long ex);
double scalbn AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, int ex);
float scalbnf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, int ex);
long double scalbnl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, int ex);
#endif
inline float tgammaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_tgammaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline double tgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_tgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double tgammal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_tgammal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float tgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::tgammaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double tgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::tgammal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type tgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::tgamma AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }

inline float truncf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_truncf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline double trunc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_trunc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double truncl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_truncl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float trunc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::truncf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double trunc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::truncl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type trunc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::trunc AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }

# define NO_MACRO_EXPAND /**/
// C99 macros defined as C++ templates
template<class T> bool signbit NO_MACRO_EXPAND(T x)
{ AUTOBOOST_STATIC_ASSERT(sizeof(T) == 0); return false; } // must not be instantiated
template<> bool AUTOBOOST_MATH_TR1_DECL signbit<float> NO_MACRO_EXPAND(float x);
template<> bool AUTOBOOST_MATH_TR1_DECL signbit<double> NO_MACRO_EXPAND(double x);
template<> bool AUTOBOOST_MATH_TR1_DECL signbit<long double> NO_MACRO_EXPAND(long double x);

template<class T> int fpclassify NO_MACRO_EXPAND(T x)
{ AUTOBOOST_STATIC_ASSERT(sizeof(T) == 0); return false; } // must not be instantiated
template<> int AUTOBOOST_MATH_TR1_DECL fpclassify<float> NO_MACRO_EXPAND(float x);
template<> int AUTOBOOST_MATH_TR1_DECL fpclassify<double> NO_MACRO_EXPAND(double x);
template<> int AUTOBOOST_MATH_TR1_DECL fpclassify<long double> NO_MACRO_EXPAND(long double x);

template<class T> bool isfinite NO_MACRO_EXPAND(T x)
{ AUTOBOOST_STATIC_ASSERT(sizeof(T) == 0); return false; } // must not be instantiated
template<> bool AUTOBOOST_MATH_TR1_DECL isfinite<float> NO_MACRO_EXPAND(float x);
template<> bool AUTOBOOST_MATH_TR1_DECL isfinite<double> NO_MACRO_EXPAND(double x);
template<> bool AUTOBOOST_MATH_TR1_DECL isfinite<long double> NO_MACRO_EXPAND(long double x);

template<class T> bool isinf NO_MACRO_EXPAND(T x)
{ AUTOBOOST_STATIC_ASSERT(sizeof(T) == 0); return false; } // must not be instantiated
template<> bool AUTOBOOST_MATH_TR1_DECL isinf<float> NO_MACRO_EXPAND(float x);
template<> bool AUTOBOOST_MATH_TR1_DECL isinf<double> NO_MACRO_EXPAND(double x);
template<> bool AUTOBOOST_MATH_TR1_DECL isinf<long double> NO_MACRO_EXPAND(long double x);

template<class T> bool isnan NO_MACRO_EXPAND(T x)
{ AUTOBOOST_STATIC_ASSERT(sizeof(T) == 0); return false; } // must not be instantiated
template<> bool AUTOBOOST_MATH_TR1_DECL isnan<float> NO_MACRO_EXPAND(float x);
template<> bool AUTOBOOST_MATH_TR1_DECL isnan<double> NO_MACRO_EXPAND(double x);
template<> bool AUTOBOOST_MATH_TR1_DECL isnan<long double> NO_MACRO_EXPAND(long double x);

template<class T> bool isnormal NO_MACRO_EXPAND(T x)
{ AUTOBOOST_STATIC_ASSERT(sizeof(T) == 0); return false; } // must not be instantiated
template<> bool AUTOBOOST_MATH_TR1_DECL isnormal<float> NO_MACRO_EXPAND(float x);
template<> bool AUTOBOOST_MATH_TR1_DECL isnormal<double> NO_MACRO_EXPAND(double x);
template<> bool AUTOBOOST_MATH_TR1_DECL isnormal<long double> NO_MACRO_EXPAND(long double x);

#undef NO_MACRO_EXPAND

// [5.2.1.1] associated Laguerre polynomials:
inline float assoc_laguerref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, unsigned m, float x)
{ return autoboost::math::tr1::autoboost_assoc_laguerref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, m, x); }
inline double assoc_laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, unsigned m, double x)
{ return autoboost::math::tr1::autoboost_assoc_laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, m, x); }
inline long double assoc_laguerrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, unsigned m, long double x)
{ return autoboost::math::tr1::autoboost_assoc_laguerrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, m, x); }
inline float assoc_laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, unsigned m, float x)
{ return autoboost::math::tr1::assoc_laguerref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, m, x); }
inline long double assoc_laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, unsigned m, long double x)
{ return autoboost::math::tr1::assoc_laguerrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, m, x); }
template <class T>
inline typename tools::promote_args<T>::type assoc_laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, unsigned m, T x)
{ return autoboost::math::tr1::assoc_laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, m, static_cast<typename tools::promote_args<T>::type>(x)); }

// [5.2.1.2] associated Legendre functions:
inline float assoc_legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, float x)
{ return autoboost::math::tr1::autoboost_assoc_legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, x); }
inline double assoc_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, double x)
{ return autoboost::math::tr1::autoboost_assoc_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, x); }
inline long double assoc_legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, long double x)
{ return autoboost::math::tr1::autoboost_assoc_legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, x); }
inline float assoc_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, float x)
{ return autoboost::math::tr1::assoc_legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, x); }
inline long double assoc_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, long double x)
{ return autoboost::math::tr1::assoc_legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, x); }
template <class T>
inline typename tools::promote_args<T>::type assoc_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, T x)
{ return autoboost::math::tr1::assoc_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, static_cast<typename tools::promote_args<T>::type>(x)); }

// [5.2.1.3] beta function:
inline float betaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::autoboost_betaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline double beta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x, double y)
{ return autoboost::math::tr1::autoboost_beta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double betal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::autoboost_betal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline float beta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x, float y)
{ return autoboost::math::tr1::betaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
inline long double beta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x, long double y)
{ return autoboost::math::tr1::betal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x, y); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type beta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T2 x, T1 y)
{ return autoboost::math::tr1::beta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(x), static_cast<typename tools::promote_args<T1, T2>::type>(y)); }

// [5.2.1.4] (complete) elliptic integral of the first kind:
inline float comp_ellint_1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k)
{ return autoboost::math::tr1::autoboost_comp_ellint_1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k); }
inline double comp_ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double k)
{ return autoboost::math::tr1::autoboost_comp_ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k); }
inline long double comp_ellint_1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k)
{ return autoboost::math::tr1::autoboost_comp_ellint_1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k); }
inline float comp_ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k)
{ return autoboost::math::tr1::comp_ellint_1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k); }
inline long double comp_ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k)
{ return autoboost::math::tr1::comp_ellint_1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k); }
template <class T>
inline typename tools::promote_args<T>::type comp_ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T k)
{ return autoboost::math::tr1::comp_ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(k)); }

// [5.2.1.5]  (complete) elliptic integral of the second kind:
inline float comp_ellint_2f(float k)
{ return autoboost::math::tr1::autoboost_comp_ellint_2f(k); }
inline double comp_ellint_2(double k)
{ return autoboost::math::tr1::autoboost_comp_ellint_2(k); }
inline long double comp_ellint_2l(long double k)
{ return autoboost::math::tr1::autoboost_comp_ellint_2l(k); }
inline float comp_ellint_2(float k)
{ return autoboost::math::tr1::comp_ellint_2f(k); }
inline long double comp_ellint_2(long double k)
{ return autoboost::math::tr1::comp_ellint_2l(k); }
template <class T>
inline typename tools::promote_args<T>::type comp_ellint_2(T k)
{ return autoboost::math::tr1::comp_ellint_2(static_cast<typename tools::promote_args<T>::type> AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k)); }

// [5.2.1.6]  (complete) elliptic integral of the third kind:
inline float comp_ellint_3f(float k, float nu)
{ return autoboost::math::tr1::autoboost_comp_ellint_3f(k, nu); }
inline double comp_ellint_3(double k, double nu)
{ return autoboost::math::tr1::autoboost_comp_ellint_3(k, nu); }
inline long double comp_ellint_3l(long double k, long double nu)
{ return autoboost::math::tr1::autoboost_comp_ellint_3l(k, nu); }
inline float comp_ellint_3(float k, float nu)
{ return autoboost::math::tr1::comp_ellint_3f(k, nu); }
inline long double comp_ellint_3(long double k, long double nu)
{ return autoboost::math::tr1::comp_ellint_3l(k, nu); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type comp_ellint_3(T1 k, T2 nu)
{ return autoboost::math::tr1::comp_ellint_3(static_cast<typename tools::promote_args<T1, T2>::type> AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k), static_cast<typename tools::promote_args<T1, T2>::type> AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu)); }

#if 0
// [5.2.1.7] confluent hypergeometric functions:
double conf_hyperg AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double a, double c, double x);
float conf_hypergf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float a, float c, float x);
long double conf_hypergl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double a, long double c, long double x);
#endif

// [5.2.1.8] regular modified cylindrical Bessel functions:
inline float cyl_bessel_if AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x)
{ return autoboost::math::tr1::autoboost_cyl_bessel_if AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline double cyl_bessel_i AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double nu, double x)
{ return autoboost::math::tr1::autoboost_cyl_bessel_i AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline long double cyl_bessel_il AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x)
{ return autoboost::math::tr1::autoboost_cyl_bessel_il AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline float cyl_bessel_i AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x)
{ return autoboost::math::tr1::cyl_bessel_if AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline long double cyl_bessel_i AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x)
{ return autoboost::math::tr1::cyl_bessel_il AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type cyl_bessel_i AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 nu, T2 x)
{ return autoboost::math::tr1::cyl_bessel_i AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(nu), static_cast<typename tools::promote_args<T1, T2>::type>(x)); }

// [5.2.1.9] cylindrical Bessel functions (of the first kind):
inline float cyl_bessel_jf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x)
{ return autoboost::math::tr1::autoboost_cyl_bessel_jf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline double cyl_bessel_j AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double nu, double x)
{ return autoboost::math::tr1::autoboost_cyl_bessel_j AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline long double cyl_bessel_jl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x)
{ return autoboost::math::tr1::autoboost_cyl_bessel_jl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline float cyl_bessel_j AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x)
{ return autoboost::math::tr1::cyl_bessel_jf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline long double cyl_bessel_j AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x)
{ return autoboost::math::tr1::cyl_bessel_jl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type cyl_bessel_j AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 nu, T2 x)
{ return autoboost::math::tr1::cyl_bessel_j AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(nu), static_cast<typename tools::promote_args<T1, T2>::type>(x)); }

// [5.2.1.10] irregular modified cylindrical Bessel functions:
inline float cyl_bessel_kf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x)
{ return autoboost::math::tr1::autoboost_cyl_bessel_kf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline double cyl_bessel_k AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double nu, double x)
{ return autoboost::math::tr1::autoboost_cyl_bessel_k AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline long double cyl_bessel_kl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x)
{ return autoboost::math::tr1::autoboost_cyl_bessel_kl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline float cyl_bessel_k AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x)
{ return autoboost::math::tr1::cyl_bessel_kf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline long double cyl_bessel_k AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x)
{ return autoboost::math::tr1::cyl_bessel_kl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type cyl_bessel_k AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 nu, T2 x)
{ return autoboost::math::tr1::cyl_bessel_k AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type> AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu), static_cast<typename tools::promote_args<T1, T2>::type>(x)); }

// [5.2.1.11] cylindrical Neumann functions;
// cylindrical Bessel functions (of the second kind):
inline float cyl_neumannf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x)
{ return autoboost::math::tr1::autoboost_cyl_neumannf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline double cyl_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double nu, double x)
{ return autoboost::math::tr1::autoboost_cyl_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline long double cyl_neumannl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x)
{ return autoboost::math::tr1::autoboost_cyl_neumannl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline float cyl_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float nu, float x)
{ return autoboost::math::tr1::cyl_neumannf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
inline long double cyl_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double nu, long double x)
{ return autoboost::math::tr1::cyl_neumannl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(nu, x); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type cyl_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 nu, T2 x)
{ return autoboost::math::tr1::cyl_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(nu), static_cast<typename tools::promote_args<T1, T2>::type>(x)); }

// [5.2.1.12] (incomplete) elliptic integral of the first kind:
inline float ellint_1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k, float phi)
{ return autoboost::math::tr1::autoboost_ellint_1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, phi); }
inline double ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double k, double phi)
{ return autoboost::math::tr1::autoboost_ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, phi); }
inline long double ellint_1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k, long double phi)
{ return autoboost::math::tr1::autoboost_ellint_1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, phi); }
inline float ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k, float phi)
{ return autoboost::math::tr1::ellint_1f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, phi); }
inline long double ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k, long double phi)
{ return autoboost::math::tr1::ellint_1l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, phi); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 k, T2 phi)
{ return autoboost::math::tr1::ellint_1 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(k), static_cast<typename tools::promote_args<T1, T2>::type>(phi)); }

// [5.2.1.13] (incomplete) elliptic integral of the second kind:
inline float ellint_2f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k, float phi)
{ return autoboost::math::tr1::autoboost_ellint_2f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, phi); }
inline double ellint_2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double k, double phi)
{ return autoboost::math::tr1::autoboost_ellint_2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, phi); }
inline long double ellint_2l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k, long double phi)
{ return autoboost::math::tr1::autoboost_ellint_2l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, phi); }
inline float ellint_2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k, float phi)
{ return autoboost::math::tr1::ellint_2f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, phi); }
inline long double ellint_2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k, long double phi)
{ return autoboost::math::tr1::ellint_2l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, phi); }
template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type ellint_2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 k, T2 phi)
{ return autoboost::math::tr1::ellint_2 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2>::type>(k), static_cast<typename tools::promote_args<T1, T2>::type>(phi)); }

// [5.2.1.14] (incomplete) elliptic integral of the third kind:
inline float ellint_3f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k, float nu, float phi)
{ return autoboost::math::tr1::autoboost_ellint_3f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, nu, phi); }
inline double ellint_3 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double k, double nu, double phi)
{ return autoboost::math::tr1::autoboost_ellint_3 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, nu, phi); }
inline long double ellint_3l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k, long double nu, long double phi)
{ return autoboost::math::tr1::autoboost_ellint_3l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, nu, phi); }
inline float ellint_3 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float k, float nu, float phi)
{ return autoboost::math::tr1::ellint_3f AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, nu, phi); }
inline long double ellint_3 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double k, long double nu, long double phi)
{ return autoboost::math::tr1::ellint_3l AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(k, nu, phi); }
template <class T1, class T2, class T3>
inline typename tools::promote_args<T1, T2, T3>::type ellint_3 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T1 k, T2 nu, T3 phi)
{ return autoboost::math::tr1::ellint_3 AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T1, T2, T3>::type>(k), static_cast<typename tools::promote_args<T1, T2, T3>::type>(nu), static_cast<typename tools::promote_args<T1, T2, T3>::type>(phi)); }

// [5.2.1.15] exponential integral:
inline float expintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::autoboost_expintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline double expint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{ return autoboost::math::tr1::autoboost_expint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double expintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::autoboost_expintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline float expint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float x)
{ return autoboost::math::tr1::expintf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
inline long double expint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double x)
{ return autoboost::math::tr1::expintl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(x); }
template <class T>
inline typename tools::promote_args<T>::type expint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T x)
{ return autoboost::math::tr1::expint AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(x)); }

// [5.2.1.16] Hermite polynomials:
inline float hermitef AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x)
{ return autoboost::math::tr1::autoboost_hermitef AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline double hermite AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, double x)
{ return autoboost::math::tr1::autoboost_hermite AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline long double hermitel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x)
{ return autoboost::math::tr1::autoboost_hermitel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline float hermite AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x)
{ return autoboost::math::tr1::hermitef AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline long double hermite AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x)
{ return autoboost::math::tr1::hermitel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
template <class T>
inline typename tools::promote_args<T>::type hermite AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, T x)
{ return autoboost::math::tr1::hermite AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, static_cast<typename tools::promote_args<T>::type>(x)); }

#if 0
// [5.2.1.17] hypergeometric functions:
double hyperg AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double a, double b, double c, double x);
float hypergf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float a, float b, float c, float x);
long double hypergl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double a, long double b, long double c,
long double x);
#endif

// [5.2.1.18] Laguerre polynomials:
inline float laguerref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x)
{ return autoboost::math::tr1::autoboost_laguerref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline double laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, double x)
{ return autoboost::math::tr1::autoboost_laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline long double laguerrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x)
{ return autoboost::math::tr1::autoboost_laguerrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline float laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x)
{ return autoboost::math::tr1::laguerref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline long double laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x)
{ return autoboost::math::tr1::laguerrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
template <class T>
inline typename tools::promote_args<T>::type laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, T x)
{ return autoboost::math::tr1::laguerre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, static_cast<typename tools::promote_args<T>::type>(x)); }

// [5.2.1.19] Legendre polynomials:
inline float legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, float x)
{ return autoboost::math::tr1::autoboost_legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, x); }
inline double legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, double x)
{ return autoboost::math::tr1::autoboost_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, x); }
inline long double legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, long double x)
{ return autoboost::math::tr1::autoboost_legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, x); }
inline float legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, float x)
{ return autoboost::math::tr1::legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, x); }
inline long double legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, long double x)
{ return autoboost::math::tr1::legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, x); }
template <class T>
inline typename tools::promote_args<T>::type legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, T x)
{ return autoboost::math::tr1::legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, static_cast<typename tools::promote_args<T>::type>(x)); }

// [5.2.1.20] Riemann zeta function:
inline float riemann_zetaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float z)
{ return autoboost::math::tr1::autoboost_riemann_zetaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(z); }
inline double riemann_zeta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(double z)
{ return autoboost::math::tr1::autoboost_riemann_zeta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(z); }
inline long double riemann_zetal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double z)
{ return autoboost::math::tr1::autoboost_riemann_zetal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(z); }
inline float riemann_zeta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(float z)
{ return autoboost::math::tr1::riemann_zetaf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(z); }
inline long double riemann_zeta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(long double z)
{ return autoboost::math::tr1::riemann_zetal AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(z); }
template <class T>
inline typename tools::promote_args<T>::type riemann_zeta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(T z)
{ return autoboost::math::tr1::riemann_zeta AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<typename tools::promote_args<T>::type>(z)); }

// [5.2.1.21] spherical Bessel functions (of the first kind):
inline float sph_besself AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x)
{ return autoboost::math::tr1::autoboost_sph_besself AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline double sph_bessel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, double x)
{ return autoboost::math::tr1::autoboost_sph_bessel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline long double sph_bessell AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x)
{ return autoboost::math::tr1::autoboost_sph_bessell AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline float sph_bessel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x)
{ return autoboost::math::tr1::sph_besself AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline long double sph_bessel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x)
{ return autoboost::math::tr1::sph_bessell AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
template <class T>
inline typename tools::promote_args<T>::type sph_bessel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, T x)
{ return autoboost::math::tr1::sph_bessel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, static_cast<typename tools::promote_args<T>::type>(x)); }

// [5.2.1.22] spherical associated Legendre functions:
inline float sph_legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, float theta)
{ return autoboost::math::tr1::autoboost_sph_legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, theta); }
inline double sph_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, double theta)
{ return autoboost::math::tr1::autoboost_sph_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, theta); }
inline long double sph_legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, long double theta)
{ return autoboost::math::tr1::autoboost_sph_legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, theta); }
inline float sph_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, float theta)
{ return autoboost::math::tr1::sph_legendref AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, theta); }
inline long double sph_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, long double theta)
{ return autoboost::math::tr1::sph_legendrel AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, theta); }
template <class T>
inline typename tools::promote_args<T>::type sph_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned l, unsigned m, T theta)
{ return autoboost::math::tr1::sph_legendre AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(l, m, static_cast<typename tools::promote_args<T>::type>(theta)); }

// [5.2.1.23] spherical Neumann functions;
// spherical Bessel functions (of the second kind):
inline float sph_neumannf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x)
{ return autoboost::math::tr1::autoboost_sph_neumannf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline double sph_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, double x)
{ return autoboost::math::tr1::autoboost_sph_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline long double sph_neumannl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x)
{ return autoboost::math::tr1::autoboost_sph_neumannl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline float sph_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, float x)
{ return autoboost::math::tr1::sph_neumannf AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
inline long double sph_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, long double x)
{ return autoboost::math::tr1::sph_neumannl AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, x); }
template <class T>
inline typename tools::promote_args<T>::type sph_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(unsigned n, T x)
{ return autoboost::math::tr1::sph_neumann AUTOBOOST_PREVENT_MACRO_SUBSTITUTION(n, static_cast<typename tools::promote_args<T>::type>(x)); }

}}} // namespaces

#else // __cplusplus

#include <autoboost/math/tr1_c_macros.ipp>

#endif // __cplusplus

#endif // AUTOBOOST_MATH_TR1_HPP

