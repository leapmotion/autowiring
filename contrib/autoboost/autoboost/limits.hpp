
//  (C) Copyright John maddock 1999.
//  (C) David Abrahams 2002.  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// use this header as a workaround for missing <limits>

//  See http://www.boost.org/libs/compatibility/index.html for documentation.

#ifndef AUTOBOOST_LIMITS
#define AUTOBOOST_LIMITS

#include <autoboost/config.hpp>

#ifdef AUTOBOOST_NO_LIMITS
#  error "There is no std::numeric_limits suppport available."
#else
# include <limits>
#endif

#if (defined(AUTOBOOST_HAS_LONG_LONG) && defined(AUTOBOOST_NO_LONG_LONG_NUMERIC_LIMITS)) \
      || (defined(AUTOBOOST_HAS_MS_INT64) && defined(AUTOBOOST_NO_MS_INT64_NUMERIC_LIMITS))
// Add missing specializations for numeric_limits:
#ifdef AUTOBOOST_HAS_MS_INT64
#  define AUTOBOOST_LLT __int64
#  define AUTOBOOST_ULLT unsigned __int64
#else
#  define AUTOBOOST_LLT  ::autoboost::long_long_type
#  define AUTOBOOST_ULLT  ::autoboost::ulong_long_type
#endif

#include <climits>  // for CHAR_BIT

namespace std
{
  template<>
  class numeric_limits<AUTOBOOST_LLT>
  {
   public:

      AUTOBOOST_STATIC_CONSTANT(bool, is_specialized = true);
#ifdef AUTOBOOST_HAS_MS_INT64
      static AUTOBOOST_LLT min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return 0x8000000000000000i64; }
      static AUTOBOOST_LLT max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return 0x7FFFFFFFFFFFFFFFi64; }
#elif defined(LLONG_MAX)
      static AUTOBOOST_LLT min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return LLONG_MIN; }
      static AUTOBOOST_LLT max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return LLONG_MAX; }
#elif defined(LONGLONG_MAX)
      static AUTOBOOST_LLT min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return LONGLONG_MIN; }
      static AUTOBOOST_LLT max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return LONGLONG_MAX; }
#else
      static AUTOBOOST_LLT min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return 1LL << (sizeof(AUTOBOOST_LLT) * CHAR_BIT - 1); }
      static AUTOBOOST_LLT max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return ~(min)(); }
#endif
      AUTOBOOST_STATIC_CONSTANT(int, digits = sizeof(AUTOBOOST_LLT) * CHAR_BIT -1);
      AUTOBOOST_STATIC_CONSTANT(int, digits10 = (CHAR_BIT * sizeof (AUTOBOOST_LLT) - 1) * 301L / 1000);
      AUTOBOOST_STATIC_CONSTANT(bool, is_signed = true);
      AUTOBOOST_STATIC_CONSTANT(bool, is_integer = true);
      AUTOBOOST_STATIC_CONSTANT(bool, is_exact = true);
      AUTOBOOST_STATIC_CONSTANT(int, radix = 2);
      static AUTOBOOST_LLT epsilon() throw() { return 0; };
      static AUTOBOOST_LLT round_error() throw() { return 0; };

      AUTOBOOST_STATIC_CONSTANT(int, min_exponent = 0);
      AUTOBOOST_STATIC_CONSTANT(int, min_exponent10 = 0);
      AUTOBOOST_STATIC_CONSTANT(int, max_exponent = 0);
      AUTOBOOST_STATIC_CONSTANT(int, max_exponent10 = 0);

      AUTOBOOST_STATIC_CONSTANT(bool, has_infinity = false);
      AUTOBOOST_STATIC_CONSTANT(bool, has_quiet_NaN = false);
      AUTOBOOST_STATIC_CONSTANT(bool, has_signaling_NaN = false);
      AUTOBOOST_STATIC_CONSTANT(bool, has_denorm = false);
      AUTOBOOST_STATIC_CONSTANT(bool, has_denorm_loss = false);
      static AUTOBOOST_LLT infinity() throw() { return 0; };
      static AUTOBOOST_LLT quiet_NaN() throw() { return 0; };
      static AUTOBOOST_LLT signaling_NaN() throw() { return 0; };
      static AUTOBOOST_LLT denorm_min() throw() { return 0; };

      AUTOBOOST_STATIC_CONSTANT(bool, is_iec559 = false);
      AUTOBOOST_STATIC_CONSTANT(bool, is_bounded = true);
      AUTOBOOST_STATIC_CONSTANT(bool, is_modulo = true);

      AUTOBOOST_STATIC_CONSTANT(bool, traps = false);
      AUTOBOOST_STATIC_CONSTANT(bool, tinyness_before = false);
      AUTOBOOST_STATIC_CONSTANT(float_round_style, round_style = round_toward_zero);

  };

  template<>
  class numeric_limits<AUTOBOOST_ULLT>
  {
   public:

      AUTOBOOST_STATIC_CONSTANT(bool, is_specialized = true);
#ifdef AUTOBOOST_HAS_MS_INT64
      static AUTOBOOST_ULLT min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return 0ui64; }
      static AUTOBOOST_ULLT max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return 0xFFFFFFFFFFFFFFFFui64; }
#elif defined(ULLONG_MAX) && defined(ULLONG_MIN)
      static AUTOBOOST_ULLT min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return ULLONG_MIN; }
      static AUTOBOOST_ULLT max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return ULLONG_MAX; }
#elif defined(ULONGLONG_MAX) && defined(ULONGLONG_MIN)
      static AUTOBOOST_ULLT min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return ULONGLONG_MIN; }
      static AUTOBOOST_ULLT max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return ULONGLONG_MAX; }
#else
      static AUTOBOOST_ULLT min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return 0uLL; }
      static AUTOBOOST_ULLT max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (){ return ~0uLL; }
#endif
      AUTOBOOST_STATIC_CONSTANT(int, digits = sizeof(AUTOBOOST_LLT) * CHAR_BIT);
      AUTOBOOST_STATIC_CONSTANT(int, digits10 = (CHAR_BIT * sizeof (AUTOBOOST_LLT)) * 301L / 1000);
      AUTOBOOST_STATIC_CONSTANT(bool, is_signed = false);
      AUTOBOOST_STATIC_CONSTANT(bool, is_integer = true);
      AUTOBOOST_STATIC_CONSTANT(bool, is_exact = true);
      AUTOBOOST_STATIC_CONSTANT(int, radix = 2);
      static AUTOBOOST_ULLT epsilon() throw() { return 0; };
      static AUTOBOOST_ULLT round_error() throw() { return 0; };

      AUTOBOOST_STATIC_CONSTANT(int, min_exponent = 0);
      AUTOBOOST_STATIC_CONSTANT(int, min_exponent10 = 0);
      AUTOBOOST_STATIC_CONSTANT(int, max_exponent = 0);
      AUTOBOOST_STATIC_CONSTANT(int, max_exponent10 = 0);

      AUTOBOOST_STATIC_CONSTANT(bool, has_infinity = false);
      AUTOBOOST_STATIC_CONSTANT(bool, has_quiet_NaN = false);
      AUTOBOOST_STATIC_CONSTANT(bool, has_signaling_NaN = false);
      AUTOBOOST_STATIC_CONSTANT(bool, has_denorm = false);
      AUTOBOOST_STATIC_CONSTANT(bool, has_denorm_loss = false);
      static AUTOBOOST_ULLT infinity() throw() { return 0; };
      static AUTOBOOST_ULLT quiet_NaN() throw() { return 0; };
      static AUTOBOOST_ULLT signaling_NaN() throw() { return 0; };
      static AUTOBOOST_ULLT denorm_min() throw() { return 0; };

      AUTOBOOST_STATIC_CONSTANT(bool, is_iec559 = false);
      AUTOBOOST_STATIC_CONSTANT(bool, is_bounded = true);
      AUTOBOOST_STATIC_CONSTANT(bool, is_modulo = true);

      AUTOBOOST_STATIC_CONSTANT(bool, traps = false);
      AUTOBOOST_STATIC_CONSTANT(bool, tinyness_before = false);
      AUTOBOOST_STATIC_CONSTANT(float_round_style, round_style = round_toward_zero);

  };
}
#endif

#endif

