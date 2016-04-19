// (C) Copyright David Abrahams 2001, Howard Hinnant 2001.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Template class numeric_traits<Number> --
//
//    Supplies:
//
//      typedef difference_type -- a type used to represent the difference
//      between any two values of Number.
//
//    Support:
//      1. Not all specializations are supplied
//
//      2. Use of specializations that are not supplied will cause a
//      compile-time error
//
//      3. Users are free to specialize numeric_traits for any type.
//
//      4. Right now, specializations are only supplied for integer types.
//
//      5. On implementations which do not supply compile-time constants in
//      std::numeric_limits<>, only specializations for built-in integer types
//      are supplied.
//
//      6. Handling of numbers whose range of representation is at least as
//      great as autoboost::intmax_t can cause some differences to be
//      unrepresentable in difference_type:
//
//        Number    difference_type
//        ------    ---------------
//        signed    Number
//        unsigned  intmax_t
//
// template <class Number> typename numeric_traits<Number>::difference_type
// numeric_distance(Number x, Number y)
//    computes (y - x), attempting to avoid overflows.
//

// See http://www.boost.org for most recent version including documentation.

// Revision History
// 11 Feb 2001 - Use AUTOBOOST_STATIC_CONSTANT (David Abrahams)
// 11 Feb 2001 - Rolled back ineffective Borland-specific code
//               (David Abrahams)
// 10 Feb 2001 - Rolled in supposed Borland fixes from John Maddock, but
//               not seeing any improvement yet (David Abrahams)
// 06 Feb 2001 - Factored if_true out into autoboost/detail/select_type.hpp
//               (David Abrahams)
// 23 Jan 2001 - Fixed logic of difference_type selection, which was
//               completely wack. In the process, added digit_traits<>
//               to compute the number of digits in intmax_t even when
//               not supplied by numeric_limits<>. (David Abrahams)
// 21 Jan 2001 - Created (David Abrahams)

#ifndef AUTOBOOST_NUMERIC_TRAITS_HPP_DWA20001901
# define AUTOBOOST_NUMERIC_TRAITS_HPP_DWA20001901

# include <autoboost/config.hpp>
# include <autoboost/cstdint.hpp>
# include <autoboost/static_assert.hpp>
# include <autoboost/type_traits.hpp>
# include <autoboost/detail/select_type.hpp>
# include <autoboost/limits.hpp>

namespace autoboost { namespace detail {

  // Template class is_signed -- determine whether a numeric type is signed
  // Requires that T is constructable from the literals -1 and 0.  Compile-time
  // error results if that requirement is not met (and thus signedness is not
  // likely to have meaning for that type).
  template <class Number>
  struct is_signed
  {
#if defined(AUTOBOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS)
    AUTOBOOST_STATIC_CONSTANT(bool, value = (Number(-1) < Number(0)));
#else
    AUTOBOOST_STATIC_CONSTANT(bool, value = std::numeric_limits<Number>::is_signed);
#endif
  };

# ifndef AUTOBOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS
  // digit_traits - compute the number of digits in a built-in integer
  // type. Needed for implementations on which numeric_limits is not specialized
  // for intmax_t (e.g. VC6).
  template <bool is_specialized> struct digit_traits_select;

  // numeric_limits is specialized; just select that version of digits
  template <> struct digit_traits_select<true>
  {
      template <class T> struct traits
      {
          AUTOBOOST_STATIC_CONSTANT(int, digits = std::numeric_limits<T>::digits);
      };
  };

  // numeric_limits is not specialized; compute digits from sizeof(T)
  template <> struct digit_traits_select<false>
  {
      template <class T> struct traits
      {
          AUTOBOOST_STATIC_CONSTANT(int, digits = (
              sizeof(T) * std::numeric_limits<unsigned char>::digits
              - (is_signed<T>::value ? 1 : 0))
              );
      };
  };

  // here's the "usable" template
  template <class T> struct digit_traits
  {
      typedef digit_traits_select<
                ::std::numeric_limits<T>::is_specialized> selector;
      typedef typename selector::template traits<T> traits;
      AUTOBOOST_STATIC_CONSTANT(int, digits = traits::digits);
  };
#endif

  // Template class integer_traits<Integer> -- traits of various integer types
  // This should probably be rolled into autoboost::integer_traits one day, but I
  // need it to work without <limits>
  template <class Integer>
  struct integer_traits
  {
# ifndef AUTOBOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS
   private:
      typedef Integer integer_type;
      typedef std::numeric_limits<integer_type> x;
   public:
      typedef typename
      if_true<(int(x::is_signed)
              && (!int(x::is_bounded)
                 // digits is the number of no-sign bits
                  || (int(x::digits) + 1 >= digit_traits<autoboost::intmax_t>::digits)))>::template then<
        Integer,

      typename if_true<(int(x::digits) + 1 < digit_traits<signed int>::digits)>::template then<
        signed int,

      typename if_true<(int(x::digits) + 1 < digit_traits<signed long>::digits)>::template then<
        signed long,

   // else
        intmax_t
      >::type>::type>::type difference_type;
#else
      AUTOBOOST_STATIC_ASSERT(autoboost::is_integral<Integer>::value);

      typedef typename
      if_true<(sizeof(Integer) >= sizeof(intmax_t))>::template then<

        typename if_true<(is_signed<Integer>::value)>::template then<
          Integer,
          intmax_t
        >::type,

        typename if_true<(sizeof(Integer) < sizeof(std::ptrdiff_t))>::template then<
          std::ptrdiff_t,
          intmax_t
        >::type
      >::type difference_type;
# endif
  };

  // Right now, only supports integers, but should be expanded.
  template <class Number>
  struct numeric_traits
  {
      typedef typename integer_traits<Number>::difference_type difference_type;
  };

  template <class Number>
  typename numeric_traits<Number>::difference_type numeric_distance(Number x, Number y)
  {
      typedef typename numeric_traits<Number>::difference_type difference_type;
      return difference_type(y) - difference_type(x);
  }
}}

#endif // AUTOBOOST_NUMERIC_TRAITS_HPP_DWA20001901
